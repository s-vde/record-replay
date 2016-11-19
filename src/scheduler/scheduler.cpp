
#include "scheduler.hpp"

// PROGRAM_MODEL
#include "execution_io.hpp"

// UTILS
#include "container_io.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "utils_io.hpp"

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   Scheduler::Scheduler()
   : mLocVars(std::make_unique<LocalVars>())
   , mPool(mLocVars->nr_threads())
   , mControl()
   , mThreads()
   , mNrRegistered(0)
   , mRegMutex()
   , mRegCond()
   , mStatus(Execution<State>::Status::RUNNING)
   , mStatusMutex()
   , mSettings(SchedulerSettings::read_from_file("schedules/settings.txt"))
   , mSelector(selector_factory(mSettings.strategy_tag()))
   , mThread([this] { return run(); })
   {
      DEBUGNL("Starting Scheduler");
      DEBUGNL("schedule:\t" << mLocVars->schedule());
   }
   
   //-------------------------------------------------------------------------------------
    
   Scheduler::~Scheduler()
   {
      DEBUGNL("~Scheduler");
      join();
   }
   
   //-------------------------------------------------------------------------------------
    
   int Scheduler::spawn_thread(pthread_t* pid,
                               const pthread_attr_t* attr,
                               void* (*start_routine)(void*),
                               void* args)
   {
      std::lock_guard<std::mutex> guard(mRegMutex);
      // Create the thread
      int ret = pthread_create(pid, attr, start_routine, args);
      // Register the thread
      const Thread::tid_t tid = mNrRegistered;
      mThreads.insert(TidMap::value_type(*pid, tid));
      mPool.register_thread(tid);
      mControl.register_thread(tid);
      DEBUGFNL(thread_str(tid), "spawn_thread", pid, "");
      ++mNrRegistered;
      // cond SIGNAL mRegCond
      mRegCond.notify_all();
      return ret;
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::wait_registered()
   {
      std::unique_lock<std::mutex> ul(mRegMutex);
      // cond WAIT mRegCond
      mRegCond.wait(ul, [this] ()
      {
         DEBUGFNL(thread_str(pthread_self()), "wait_registered", "", "");
         return mThreads.find(pthread_self()) != mThreads.end();
      });
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::post_task(const int op, const Object& obj)
   {
      if (runs_controlled())
      {
         const Thread::tid_t tid = find_tid(pthread_self());
         const Instruction instr(tid, static_cast<Object::Op>(op), obj);
         DEBUGFNL(thread_str(tid), "post_task", instr, "");
         mPool.post(tid, instr);
         DEBUGFNL(thread_str(tid), "wait_for_turn", "", "");
         mControl.wait_for_turn(tid);
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::yield()
   {
      if (runs_controlled())
      {
         const Thread::tid_t tid = find_tid(pthread_self());
         DEBUGFNL(thread_str(tid), "yield", "", "");
         mPool.yield(tid);
      }
   }
   
   //-------------------------------------------------------------------------------------

   void Scheduler::finish()
   {
      if (runs_controlled())
      {
         const Thread::tid_t tid = find_tid(pthread_self());
         DEBUGFNL(thread_str(tid), "finish", "", "");
         mPool.set_status_protected(tid, Thread::Status::FINISHED);
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::join()
   {
      if (mThread.joinable())
      {
         mThread.join();
         DEBUGNL("mTread.joined");
      }
      else
      {
         DEBUGNL("!mThread.joinable()");
      }
   }
   
   //-------------------------------------------------------------------------------------

   // SCHEDULER INTERNAL
    
   Thread::tid_t Scheduler::find_tid(const pthread_t& pid)
   {
      std::lock_guard<std::mutex> guard(mRegMutex);
      auto it = mThreads.find(pid);
      /// @pre mThreads.find(pid) != mThreads.end()
      assert(it != mThreads.end());
      return it->second;
   }
   
   //-------------------------------------------------------------------------------------

   bool Scheduler::runs_controlled()
   {
      const Execution<State>::Status s = status();
      return
         s != Execution<State>::Status::BLOCKED &&
         s != Execution<State>::Status::ERROR;
   }
   
   //-------------------------------------------------------------------------------------
    
   Execution<State>::Status Scheduler::status()
   {
      std::lock_guard<std::mutex> guard(mStatusMutex);
      return mStatus;
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::set_status(const Execution<State>::Status &s)
   {
      std::lock_guard<std::mutex> guard(mStatusMutex);
      mStatus = s;
   }
   
   //-------------------------------------------------------------------------------------

   std::string Scheduler::thread_str(const Thread::tid_t& tid)
   {
      std::stringstream ss("");
      ss << "[thread" << tid << "]\t\t";
      return ss.str();
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string Scheduler::thread_str(const pthread_t& pid)
   {
      std::stringstream ss("");
      ss << "[thread" << pid << "]\t\t";
      return ss.str();
   }
   
   //-------------------------------------------------------------------------------------
    
   // THREAD
    
   // #todo Look at mutex protection in updating E (i.e. retreiving current_task),
   // and in selecting the next thread. At this point, no other thread should operate
   // on mPool, but this is not checked/enforced.
   void Scheduler::run()
   {
      mControl.set_owner(mThread.get_id()); // mThread.get_id() == std::this_thread::get_id()
      wait_all_registered();
      mPool.wait_enabled_collected();
        
      Execution<State> E(mLocVars->nr_threads(), mPool.program_state());
        
      while (status() == Execution<State>::Status::RUNNING)
      {
         DEBUGNL("---------- [round" << mLocVars->task_nr() << "]");
         if (mLocVars->task_nr() > 0)
         {
            E.push_back(*mPool.current_task(), mPool.program_state());
         }
         auto selection = mSelector->select(mPool,
                                            mLocVars->schedule(),
                                            mLocVars->task_nr());
         if (selection.first == Execution<State>::Status::RUNNING)
         {
            /// @pre
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
         mPool.wait_enabled_collected();
      }
      close(E);
   }
   
   //-------------------------------------------------------------------------------------
    
   // THREAD (PRIVATE)
    
   void Scheduler::wait_all_registered()
   {
      std::unique_lock<std::mutex> ul(mRegMutex);
      // cond WAIT mRegCond
      mRegCond.wait(ul, [this] () {
            DEBUGFNL("Scheduler", "wait_all_registered", "", "");
            return mNrRegistered == mLocVars->nr_threads(); });
   }
   
   //-------------------------------------------------------------------------------------
    
   /// Sets tid as the current thread in TaskPool, removing and obtaining the current
   /// posted task by Thread tid from the TaskPool. Removing the task is to guarantee that
   /// the Scheduler thread blocks on mPool.all_enabled_collected, because the thread only
   /// updates its status when it yields in task_done. Calls Control to grant Thread tid
   /// execution right.

   bool Scheduler::schedule_thread(const Thread::tid_t& tid)
   {
      if (mPool.status_protected(tid) == Thread::Status::ENABLED)
      {
         const Instruction task = mPool.set_current(tid);
         DEBUGFNL("Scheduler", "schedule_thread", tid, "next task = " << task);
         mControl.grant_execution_right(tid);
         mLocVars->increase_task_nr();
         return true;
      }
      return false;
   }
   
   //-------------------------------------------------------------------------------------
    
   /// @note As soon as internal Scheduler status is set to ERROR new threads are not
   /// waiting anymore, but we already waiting threads have to be "waken-up".

   void Scheduler::report_error(const std::string& what)
   {
      ERROR("Scheduler::report_error", what);
      set_status(Execution<State>::Status::ERROR);
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::close(Execution<State>& E)
   {
      DEBUGFNL("Scheduler", "close", "", to_string(status()));
      if (!runs_controlled())
      {
         mControl.grant_execution_right_all();
      }
      // finish execution
      try
      {
         E.last().set_post(mPool.program_state());
      }
      catch(const std::out_of_range& e)
      {
         ERROR("Scheduler::close", e.what());
      }
      E.set_status(status());
      dump_execution(E);
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::dump_execution(const Execution<State>& E) const
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
   
   //-------------------------------------------------------------------------------------
    
   // Class Scheduler::LocalVars
    
   Scheduler::LocalVars::LocalVars()
   : mNrThreads(0)
   , mSchedule()
   , mTaskNr(0)
   {
      if (!utils::io::read_from_file("schedules/schedule.txt", mSchedule))
      {
         ERROR("Scheduler::LocalVars()", "reading schedules/schedule.txt");
         mSchedule = {};
      }
      if (!utils::io::read_from_file("schedules/threads.txt", mNrThreads))
      {
         ERROR("Scheduler::LocalVars()", "reading schedules/threads.txt");
         // #todo Handle such an error
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   int Scheduler::LocalVars::nr_threads() const
   {
      return mNrThreads;
   }
   
   //-------------------------------------------------------------------------------------
    
   const schedule_t& Scheduler::LocalVars::schedule() const
   {
      return mSchedule;
   }
   
   //-------------------------------------------------------------------------------------
    
   int Scheduler::LocalVars::task_nr() const
   {
      return mTaskNr;
   }
   
   //-------------------------------------------------------------------------------------
    
   void Scheduler::LocalVars::increase_task_nr()
   {
      mTaskNr++;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler

