#pragma once

#include "controllable_thread.hpp"
#include "schedule.hpp"
#include "scheduler_settings.hpp"
#include "selector_register.hpp"

#include <execution.hpp>

#include <boost/optional.hpp>

#include <thread>
#include <unordered_map>

//--------------------------------------------------------------------------------------------------
/// @file scheduler.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

/// @details The Scheduler class provides a start routine for a scheduler thread that
/// has access to an encapsulated Control object that it can use to control a
/// multithreaded input program's thread interleaving. It further encapsulates the
/// scheduler's state, which includes a TaskPool object. The class provides an
/// interface for the input program's threads.

class Scheduler
{
public:
   /// @{
   /// Lifetime
   Scheduler();
   /// @}

   // WRAPPERS

   void register_main_thread();

   /// @details Associates pid with a unique Scheduler-internal thread id (tid) in mThreads and
   /// creates a controllable_thread in mControllableThreads. Signals mRegCond when the registration
   /// is finished. Only a single thread at a time can use this functionality, which is important
   /// when threads can spawn other threads and hence multiple threads can be in spawn_thread
   /// concurrently.

   Thread::tid_t register_thread(const pthread_t& pid,
                                 boost::optional<program_model::Thread::tid_t> tid);

   /// @brief Wrapper around pthread_create(pid, attr, start_routine, args). Registers the newly
   /// created thread with the Scheduler. Called by the spawning thread.
   /// @returns The return value of the pthread_create call.

   Thread::tid_t post_spawn_instruction(pthread_t* pid, const std::string& file_name,
                                        unsigned int line_number);

   void post_join_instruction(pthread_t pid, const std::string& file_name,
                              unsigned int line_number);

   void post_memory_instruction(const int op, const Object& obj, bool is_atomic,
                                const std::string& file_name, unsigned int line_number);

   void post_lock_instruction(const int op, const Object& obj, const std::string& file_name,
                              unsigned int line_number);

   /// @brief If the program runs Scheduler-controlled, this function updates the
   /// status of the calling thread in mPool.

   void finish();

   /// @brief Lets the main thread of the input program join the Scheduler thread.

   void join();

private:
   // Forward declarations
   class LocalVars;

   // Type definitions
   using TidMap = std::unordered_map<pthread_t, const Thread::tid_t>;
   using Threads = std::unordered_map<program_model::Thread::tid_t, controllable_thread>;

   std::unique_ptr<LocalVars> mLocVars;
   TaskPool mPool;

   /// @brief Protects mThreads, mNrRegistered and mRegCond.
   // #todo Look at possibility of using shared_mutex

   std::mutex mRegMutex;
   TidMap mThreads;
   Threads mControllableThreads;
   int mNrRegistered;
   std::atomic<bool> mMainThreadRegistered;
   std::condition_variable mRegCond;

   Execution::Status mStatus;
   std::mutex mStatusMutex;

   SchedulerSettings mSettings;
   SelectorUniquePtr mSelector;

   std::thread mThread;

   // SCHEDULER INTERNAL

   Thread::tid_t get_fresh_tid(const std::lock_guard<std::mutex>& registration_lock);

   using create_instruction_t =
      std::function<program_model::visible_instruction_t(program_model::Thread::tid_t)>;

   void post_task(const create_instruction_t&);

   program_model::Thread::tid_t wait_until_registered();

   Thread::tid_t find_tid(const pthread_t& pid);

   controllable_thread& get_controllable_thread(const program_model::Thread::tid_t tid);

   bool runs_controlled();

   Execution::Status status();

   void set_status(const Execution::Status&);

   std::string thread_str(const Thread::tid_t&);
   std::string thread_str(const pthread_t&);

   // THREAD

   /// @brief Start routine of mThread.

   void run();

   /// @brief Waits until mNrRegistered == LV.nr_threads.

   void wait_until_main_thread_registered();

   /// @brief Schedule the next task of given tid.
   /// @returns true iff scheduling the thread succeeded (i.e. tid is ENABLED.

   bool schedule_thread(const Thread::tid_t& tid);

   void report_error(const std::string& what);

   void close(Execution& E);

   void dump_execution(const Execution& E) const;
   void dump_data_races() const;

}; // end class Scheduler

//--------------------------------------------------------------------------------------------------

/// @brief Encapsulates the data that is modified by the Scheduler thread mThread only.

class Scheduler::LocalVars
{
public:
   /// @brief Constructor.

   LocalVars();

   /// @brief Getter.

   const schedule_t& schedule() const;

   /// @brief Getter.

   int task_nr() const;

   void increase_task_nr();

private:
   /// @brief The schedule under which the scheduler is driving the program.

   schedule_t mSchedule;

   /// @brief Counter of already scheduled tasks.

   int mTaskNr;

}; // end class Scheduler::LocalVars
} // end namespace scheduler

//--------------------------------------------------------------------------------------------------

// Wrapper functions

static scheduler::Scheduler the_scheduler;

extern "C" {

void wrapper_register_main_thread();

void wrapper_register_thread(const pthread_t* const pid, int tid);

int wrapper_post_spawn_instruction(pthread_t*, const char* file_name, unsigned int line_number);

void wrapper_post_pthread_join_instruction(pthread_t, const char* file_name, unsigned int line_number);

void wrapper_post_stdthread_join_instruction(std::thread*, const char* file_name, unsigned int line_number);

void wrapper_post_memory_instruction(int operation, void* operand, bool is_atomic,
                                     const char* file_name, unsigned int line_number);

void wrapper_post_lock_instruction(int operation, void* operand, const char* file_name,
                                   unsigned int line_number);

void wrapper_finish();

} // end extern "C"
