
#include "scheduler.hpp"

#include <execution_io.hpp>
#include <visible_instruction_io.hpp>

#include <container_io.hpp>
#include <debug.hpp>
#include <error.hpp>
#include <utils_io.hpp>

#include <boost/range/algorithm/find_if.hpp>

#include <exception>
#include <iomanip>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

namespace {
std::string pid_to_string(const pthread_t& pid)
{
   std::stringstream stream("0x");
   stream << std::setfill('0') << std::setw(2);
   unsigned char* pidc = (unsigned char*)(void*)(&pid);
   for (size_t i = 0; i < sizeof(pid); ++i)
   {
      stream << (unsigned)(pidc[i]);
   }
   return stream.str();
}
} // end namespace


//--------------------------------------------------------------------------------------------------

class unregistered_thread : public std::exception
{
};

//--------------------------------------------------------------------------------------------------

Scheduler::Scheduler()
: mLocVars(std::make_unique<LocalVars>())
, mPool()
, mThreads()
, mControllableThreads()
, mNrRegistered(0)
, mRegMutex()
, mRegCond()
, mStatus(Execution::Status::RUNNING)
, mStatusMutex()
, mSettings(SchedulerSettings::read_from_file("schedules/settings.txt"))
, mSelector(selector_factory(mSettings.strategy_tag()))
, mThread([this] { return run(); })
{
   DEBUG_SYNC("Starting Scheduler\n");
   DEBUG_SYNC("schedule:\t" << mLocVars->schedule() << "\n");
}

//--------------------------------------------------------------------------------------------------

void Scheduler::register_main_thread()
{
   register_thread(pthread_self(), boost::none);
}

//--------------------------------------------------------------------------------------------------

Thread::tid_t Scheduler::register_thread(const pthread_t& pid,
                                         boost::optional<program_model::Thread::tid_t> tid)
{
   DEBUGF_SYNC(thread_str(pid), "register_thread", "", "\n");
   std::lock_guard<std::mutex> lock(mRegMutex);
   if (!tid)
   {
      tid = get_fresh_tid(lock);
   }
   mThreads.insert(TidMap::value_type(pid, *tid));
   mControllableThreads.emplace(std::piecewise_construct, std::forward_as_tuple(*tid),
                                std::forward_as_tuple(*tid, mThread.get_id()));
   mPool.register_thread(*tid);

   if (tid == 0)
      mMainThreadRegistered.store(true);

   DEBUGF_SYNC(thread_str(*tid), "register_thread", "pid=" << pid_to_string(pid), "\n");
   mRegCond.notify_all();
   return *tid;
}

//--------------------------------------------------------------------------------------------------

program_model::Thread::tid_t Scheduler::post_spawn_instruction(pthread_t* pid,
                                                               const std::string& file_name,
                                                               unsigned int line_number)
{
   const auto new_tid = get_fresh_tid(std::lock_guard<std::mutex>(mRegMutex));

   post_task([new_tid, &file_name, line_number](const auto tid) {
      return program_model::thread_management_instruction(tid, thread_management_operation::Spawn,
                                                          program_model::Thread(new_tid),
                                                          {file_name, line_number});
   });

   // The thread about to be spawn cannot be registered yet, because pid only holds useful
   // data once pthread_create returns
   return new_tid;
}

//--------------------------------------------------------------------------------------------------

void Scheduler::post_join_instruction(pthread_t pid, const std::string& file_name,
                                      unsigned int line_number)
{
   try
   {
      const auto tid_joined = find_tid(pid);
      post_task([tid_joined, &file_name, line_number](const auto tid) {
         return program_model::thread_management_instruction(tid, thread_management_operation::Join,
                                                             program_model::Thread(tid_joined),
                                                             {file_name, line_number});
      });
   }
   catch (const unregistered_thread&)
   {
      DEBUGF_SYNC("", "post_join_thread", "unregistered thread " << pid_to_string(pid), "\n");
   }
}

//--------------------------------------------------------------------------------------------------

void Scheduler::post_memory_instruction(const int op, const Object& obj, bool is_atomic,
                                        const std::string& file_name, unsigned int line_number)
{
   post_task([op, &obj, is_atomic, &file_name, line_number](const auto tid) {
      return program_model::memory_instruction(tid, static_cast<memory_operation>(op), obj,
                                               is_atomic, {file_name, line_number});
   });
}

//--------------------------------------------------------------------------------------------------

void Scheduler::post_lock_instruction(const int op, const Object& obj, const std::string& file_name,
                                      unsigned int line_number)
{
   post_task([op, &obj, &file_name, line_number](const auto tid) {
      return program_model::lock_instruction(tid, static_cast<lock_operation>(op), obj,
                                             {file_name, line_number});
   });
}

//--------------------------------------------------------------------------------------------------

void Scheduler::enter_function(const std::string& function_name)
{
   DEBUGF_SYNC(thread_str(pthread_self()), "enter_function", function_name, "\n");

   const auto tid = wait_until_registered();
   get_controllable_thread(tid).enter_function(function_name);
}

//--------------------------------------------------------------------------------------------------

void Scheduler::exit_function(const std::string& function_name)
{
   const auto tid = wait_until_registered();
   try
   {
      get_controllable_thread(tid).exit_function(function_name);
   }
   catch (const controllable_thread::finished&)
   {
      if (runs_controlled())
      {
         mPool.yield(tid);

         DEBUGF_SYNC(thread_str(tid), "finish", "", "\n");
         mPool.finish(tid);
      }

      // The main thread is responsible for joining the scheduler thread
      if (tid == 0)
      {
         join();
      }
   }
}

//--------------------------------------------------------------------------------------------------

void Scheduler::join()
{
   if (mThread.joinable())
   {
      mThread.join();
      DEBUG_SYNC("mTread.joined\n");
   }
   else
   {
      DEBUG_SYNC("!mThread.joinable()\n");
   }
}

//--------------------------------------------------------------------------------------------------
// SCHEDULER INTERNAL
//--------------------------------------------------------------------------------------------------

Thread::tid_t Scheduler::get_fresh_tid(const std::lock_guard<std::mutex>& registration_lock)
{
   const Thread::tid_t tid = mNrRegistered;
   ++mNrRegistered;
   return tid;
}

//--------------------------------------------------------------------------------------------------

void Scheduler::post_task(const create_instruction_t& create_instruction)
{
   if (!mMainThreadRegistered.load())
   {
      DEBUGF_SYNC("unregistered thread", "post_task", "", "\n");
      return;
   }

   const auto tid = wait_until_registered();
   auto instruction = create_instruction(tid);
   DEBUGF_SYNC(thread_str(tid), "post_task",
               boost::apply_visitor(program_model::instruction_to_short_string(), instruction),
               "\n");
   if (runs_controlled())
   {
      mPool.yield(tid);
      mPool.post(tid, instruction);
      get_controllable_thread(tid).post_task();
   }
}

//--------------------------------------------------------------------------------------------------

program_model::Thread::tid_t Scheduler::wait_until_registered()
{
   const auto pid = pthread_self();
   Thread::tid_t tid;
   std::unique_lock<std::mutex> lock(mRegMutex);
   mRegCond.wait(lock, [this, &pid, &tid]() {
      DEBUGF_SYNC(thread_str(pid), "wait_until_registered", "", "\n");
      const auto it = mThreads.find(pid);
      if (it != mThreads.end())
      {
         tid = it->second;
         return true;
      }
      return false;
   });
   return tid;
}

//--------------------------------------------------------------------------------------------------

Thread::tid_t Scheduler::find_tid(const pthread_t& pid)
{
   std::lock_guard<std::mutex> guard(mRegMutex);
   const auto it = boost::range::find_if(
      mThreads, [&pid](const auto& entry) { return pthread_equal(entry.first, pid); });
   if (it != mThreads.end())
      return it->second;
   throw unregistered_thread();
}

//--------------------------------------------------------------------------------------------------

controllable_thread& Scheduler::get_controllable_thread(const program_model::Thread::tid_t tid)
{
   std::lock_guard<std::mutex> lock(mRegMutex);
   auto it = mControllableThreads.find(tid);
   assert(it != mControllableThreads.end());
   return it->second;
}

//--------------------------------------------------------------------------------------------------

bool Scheduler::runs_controlled()
{
   const Execution::Status s = status();
   return s != Execution::Status::BLOCKED && s != Execution::Status::ERROR;
}

//--------------------------------------------------------------------------------------------------

Execution::Status Scheduler::status()
{
   std::lock_guard<std::mutex> guard(mStatusMutex);
   return mStatus;
}

//--------------------------------------------------------------------------------------------------

void Scheduler::set_status(const Execution::Status& s)
{
   std::lock_guard<std::mutex> guard(mStatusMutex);
   mStatus = s;
}

//--------------------------------------------------------------------------------------------------

std::string Scheduler::thread_str(const Thread::tid_t& tid)
{
   std::stringstream ss("");
   ss << "[thread" << tid << "]\t\t";
   return ss.str();
}

//--------------------------------------------------------------------------------------------------

std::string Scheduler::thread_str(const pthread_t& pid)
{
   std::stringstream ss("");
   ss << "[thread" << pid << "]\t\t";
   return ss.str();
}

//--------------------------------------------------------------------------------------------------

// THREAD

// #todo Look at mutex protection in updating E (i.e. retreiving current_task),
// and in selecting the next thread. At this point, no other thread should operate
// on mPool, but this is not checked/enforced.
void Scheduler::run()
{
   wait_until_main_thread_registered();
   mPool.wait_until_unfinished_threads_have_posted();
   DEBUG_SYNC(mPool << "\n");

   Execution E(mPool.program_state());

   while (status() == Execution::Status::RUNNING)
   {
      DEBUG_SYNC("---------- [round" << mLocVars->task_nr() << "]\n");
      if (mLocVars->task_nr() > 0)
      {
         E.push_back(*mPool.current_task(), mPool.program_state());
      }
      try
      {
         auto selection = mSelector->select(mPool, mLocVars->schedule(), mLocVars->task_nr());
         if (selection.first == Execution::Status::RUNNING)
         {
            assert(selection.second >= 0);
            if (!schedule_thread(selection.second))
            {
               report_error("selection error");
               break;
            }
         }
         else
         {
            set_status(selection.first);
            break;
         }
         mPool.wait_until_unfinished_threads_have_posted();
         DEBUG_SYNC(mPool << "\n");
      }
      catch (const deadlock_exception& deadlock)
      {
         write_to_stream(std::cout, deadlock.get());
         set_status(Execution::Status::DEADLOCK);
         break;
      }
      catch (const controllable_thread::permission_denied& e)
      {
         std::cout << e.what() << "\n";
         throw;
      }
   }
   close(E);
}

//--------------------------------------------------------------------------------------------------

// THREAD (PRIVATE)

void Scheduler::wait_until_main_thread_registered()
{
   std::unique_lock<std::mutex> lock(mRegMutex);
   mRegCond.wait(lock, [this]() {
      DEBUGF_SYNC("Scheduler", "wait_until_main_thread_registered", "", "\n");
      return mMainThreadRegistered.load();
   });
}

//--------------------------------------------------------------------------------------------------

/// Sets tid as the current thread in TaskPool, removing and obtaining the current
/// posted task by Thread tid from the TaskPool. Removing the task is to guarantee that
/// the Scheduler thread blocks on mPool.all_enabled_collected, because the thread only
/// updates its status when it yields in task_done. Calls Control to grant Thread tid
/// execution right.

bool Scheduler::schedule_thread(const Thread::tid_t& tid)
{
   if (mPool.status_protected(tid) == Thread::Status::ENABLED)
   {
      const program_model::visible_instruction_t task = mPool.set_current(tid);
      DEBUGF_SYNC(
         "Scheduler", "schedule_thread", tid,
         "next task = " << boost::apply_visitor(program_model::instruction_to_short_string(), task)
                        << "\n");
      get_controllable_thread(tid).grant_execution_right();
      mLocVars->increase_task_nr();
      return true;
   }
   return false;
}

//--------------------------------------------------------------------------------------------------

/// @note As soon as internal Scheduler status is set to ERROR new threads are not
/// waiting anymore, but we already waiting threads have to be "waken-up".

void Scheduler::report_error(const std::string& what)
{
   ERROR("Scheduler::report_error", what);
   set_status(Execution::Status::ERROR);
}

//--------------------------------------------------------------------------------------------------

void Scheduler::close(Execution& E)
{
   DEBUGF_SYNC("Scheduler", "close", "", to_string(status()) << "\n");
   if (!runs_controlled())
   {
      std::lock_guard<std::mutex> lock(mRegMutex);
      std::for_each(mControllableThreads.begin(), mControllableThreads.end(),
                    [](auto& entry) { entry.second.grant_execution_right(); });
   }
   // finish execution
   try
   {
      E.last().set_post(mPool.program_state());
   }
   catch (const std::out_of_range& e)
   {
      ERROR("Scheduler::close", e.what());
   }
   E.set_status(status());
   dump_execution(E);
   dump_data_races();
}

//--------------------------------------------------------------------------------------------------

void Scheduler::dump_execution(const Execution& E) const
{
   if (!E.empty())
   {
      std::ofstream record;
      record.open("record.txt");
      record << E;
      record.close();

      std::ofstream record_short;
      record_short.open("record_short.txt");
      record_short << to_short_string(E);
      record_short.close();
   }
}

//--------------------------------------------------------------------------------------------------

void Scheduler::dump_data_races() const
{
   std::ofstream ofs;
   ofs.open("data_races.txt", std::ofstream::app);
   for (const auto& data_race : mPool.data_races())
   {
      write_to_stream(ofs, data_race);
   }
   ofs << "\n>>>>>\n\n";
   ofs.close();
}

//--------------------------------------------------------------------------------------------------

// Class Scheduler::LocalVars

Scheduler::LocalVars::LocalVars()
: mSchedule()
, mTaskNr(0)
{
   if (!utils::io::read_from_file("schedules/schedule.txt", mSchedule))
   {
      ERROR("Scheduler::LocalVars()", "reading schedules/schedule.txt");
      mSchedule = {};
   }
}

//--------------------------------------------------------------------------------------------------

const schedule_t& Scheduler::LocalVars::schedule() const
{
   return mSchedule;
}

//--------------------------------------------------------------------------------------------------

int Scheduler::LocalVars::task_nr() const
{
   return mTaskNr;
}

//--------------------------------------------------------------------------------------------------

void Scheduler::LocalVars::increase_task_nr()
{
   mTaskNr++;
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler

//--------------------------------------------------------------------------------------------------


void wrapper_register_main_thread()
{
   the_scheduler.register_main_thread();
}

//--------------------------------------------------------------------------------------------------

void wrapper_register_thread(const pthread_t* const pid, const int tid)
{
   the_scheduler.register_thread(*pid, tid);
}

//--------------------------------------------------------------------------------------------------

int wrapper_post_spawn_instruction(pthread_t* pid, const char* file_name, unsigned int line_number)
{
   return the_scheduler.post_spawn_instruction(pid, file_name, line_number);
}

//--------------------------------------------------------------------------------------------------

void wrapper_post_pthread_join_instruction(pthread_t pid, const char* file_name,
                                           unsigned int line_number)
{
   return the_scheduler.post_join_instruction(pid, file_name, line_number);
}

//--------------------------------------------------------------------------------------------------

void wrapper_post_stdthread_join_instruction(std::thread* thr, const char* file_name,
                                             unsigned int line_number)
{
   return the_scheduler.post_join_instruction(thr->native_handle(), file_name, line_number);
}

//--------------------------------------------------------------------------------------------------

void wrapper_post_memory_instruction(int operation, void* operand, bool is_atomic,
                                     const char* file_name, unsigned int line_number)
{
   the_scheduler.post_memory_instruction(operation, program_model::Object(operand), is_atomic,
                                         file_name, line_number);
}

//--------------------------------------------------------------------------------------------------

void wrapper_post_lock_instruction(int operation, void* operand, const char* file_name,
                                   unsigned int line_number)
{
   the_scheduler.post_lock_instruction(operation, program_model::Object(operand), file_name,
                                       line_number);
}

//--------------------------------------------------------------------------------------------------

void wrapper_enter_function(const char* function_name)
{
   the_scheduler.enter_function(function_name);
}

//--------------------------------------------------------------------------------------------------

void wrapper_exit_function(const char* function_name)
{
   the_scheduler.exit_function(function_name);
}

//--------------------------------------------------------------------------------------------------
