#pragma once

// SCHEDULER
#include "control.hpp"
#include "schedule.hpp"
#include "scheduler_settings.hpp"
#include "selector_register.hpp"

// PROGRAM_MODEL
#include "execution.hpp"

// STL
#include <thread>
#include <unordered_map>

//--------------------------------------------------------------------------------------90
/// @file scheduler.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------

   /// @details The Scheduler class provides a start routine for a scheduler thread that
   /// has access to an encapsulated Control object that it can use to control a
   /// multithreaded input program's thread interleaving. It further encapsulates the
   /// scheduler's state, which includes a TaskPool object. The class provides an
   /// interface for the input program's threads.

   class Scheduler
   {
   public:

      //----------------------------------------------------------------------------------

      using TidMap = std::unordered_map<pthread_t,const Thread::tid_t>;

      //----------------------------------------------------------------------------------

      /// @brief Constructor.

      Scheduler();

      //----------------------------------------------------------------------------------

      /// @brief Destructor.

      ~Scheduler();

      //----------------------------------------------------------------------------------

      Scheduler(const Scheduler&) = delete;
      Scheduler(Scheduler&&) = delete;
      Scheduler& operator=(const Scheduler&) = delete;
      Scheduler& operator=(Scheduler&&) = delete;

      //----------------------------------------------------------------------------------

      // WRAPPERS

      /// @brief Calls pthread_create(pid, attr, start_routine, args) and registers the
      /// created thread with the Scheduler.
      /// @details Associates pid with a unique Scheduler-internal id in mThreads and
      /// registers the created thread with mPool and mControl. Signals mRegCond when the
      /// registration is finished. Only a single thread at a time can use this functionality.
      /// This is important when threads can spawn other threads and hence multiple threads
      /// can be in spawn_thread at the same time.
      /// @returns The return value of the pthread_create call.

      int spawn_thread(pthread_t* pid,
                       const pthread_attr_t* attr,
                       void* (*start_routine)(void*),
                       void* args);

      //----------------------------------------------------------------------------------

      /// @brief Lets the calling thread wait until its registration with the Scheduler is
      /// complete.

      void wait_registered();

      //----------------------------------------------------------------------------------

      /// @brief If the program runs Scheduler-controlled, this function constructs an
      /// Instruction object from the the input, posts this Instruction in mPool and calls
      /// Scheduler::wait_for_turn.

      void post_task(const int op, const Object& obj, bool is_atomic,
                     const std::string& file_name, unsigned int line_number);

      //----------------------------------------------------------------------------------

      /// @brief If the program runs Scheduler-controlled, this function updates the
      /// status of the calling thread in mPool.

      void finish();

      //----------------------------------------------------------------------------------

      /// @brief Lets the main thread of the input program join the Scheduler thread.

      void join();

      //----------------------------------------------------------------------------------

   private:

      //----------------------------------------------------------------------------------

      // Forward declaration
      class LocalVars;

      //----------------------------------------------------------------------------------

      std::unique_ptr<LocalVars> mLocVars;
      TaskPool mPool;
      Control mControl;

      //----------------------------------------------------------------------------------

      /// @brief Protects mThreads, mNrRegistered and mRegCond.
      // #todo Look at possibility of using shared_mutex
      std::mutex mRegMutex;
      TidMap mThreads;
      int mNrRegistered;
      std::condition_variable mRegCond;

      //----------------------------------------------------------------------------------

      Execution::Status mStatus;
      std::mutex mStatusMutex;

      //----------------------------------------------------------------------------------

      SchedulerSettings mSettings;
      SelectorUniquePtr mSelector;

      //----------------------------------------------------------------------------------

      std::thread mThread;

      //----------------------------------------------------------------------------------

      // SCHEDULER INTERNAL

      Thread::tid_t find_tid(const pthread_t& pid);

      //----------------------------------------------------------------------------------

      bool runs_controlled();

      //----------------------------------------------------------------------------------

      Execution::Status status();

      //----------------------------------------------------------------------------------

      void set_status(const Execution::Status&);

      //----------------------------------------------------------------------------------

      std::string thread_str(const Thread::tid_t&);
      std::string thread_str(const pthread_t&);

      //----------------------------------------------------------------------------------

      // THREAD

      /// @brief Start routine of mThread.

      void run();

      //----------------------------------------------------------------------------------

      /// @brief Waits until mNrRegistered == LV.nr_threads.

      void wait_all_registered();

      //----------------------------------------------------------------------------------

      /// @brief Schedule the next task of given tid.
      /// @returns true iff scheduling the thread succeeded (i.e. tid is ENABLED.

      bool schedule_thread(const Thread::tid_t& tid);

      //----------------------------------------------------------------------------------

      void report_error(const std::string& what);

      //----------------------------------------------------------------------------------

      void close(Execution& E);

      //----------------------------------------------------------------------------------

      void dump_execution(const Execution& E) const;
      void dump_data_races() const;

      //----------------------------------------------------------------------------------

   }; // end class Scheduler

   //-------------------------------------------------------------------------------------

   /// @brief Encapsulates the data that is modified by the Scheduler thread mThread only.

   class Scheduler::LocalVars
   {
   public:

      //----------------------------------------------------------------------------------

      /// @brief Constructor.

      LocalVars();

      //----------------------------------------------------------------------------------

      /// @brief Getter.

      int nr_threads() const;

      //----------------------------------------------------------------------------------

      /// @brief Getter.

      const schedule_t& schedule() const;

      //----------------------------------------------------------------------------------

      /// @brief Getter.

      int task_nr() const;

      //----------------------------------------------------------------------------------

      void increase_task_nr();

      //----------------------------------------------------------------------------------

   private:

      //----------------------------------------------------------------------------------

      /// @brief The (fixed) number of threads in the program.

      int mNrThreads;

      //----------------------------------------------------------------------------------

      /// @brief The schedule under which the scheduler is driving the program.

      schedule_t mSchedule;

      //----------------------------------------------------------------------------------

      /// @brief Counter of already scheduled tasks.

      int mTaskNr;

      //----------------------------------------------------------------------------------

   }; // end class Scheduler::LocalVars

   //-------------------------------------------------------------------------------------

} // end namespace scheduler

//----------------------------------------------------------------------------------------

// Wrapper functions

static scheduler::Scheduler the_scheduler;

extern "C"
{
int wrapper_spawn_thread(pthread_t* pid, const pthread_attr_t* attr, void* (*start_routine)(void*), void* args);

void wrapper_wait_registered();

void wrapper_post_instruction(int operation, void* operand,
                              const char* file_name, unsigned int line_number);

void wrapper_post_memory_instruction(int operation, void* operand, bool is_atomic,
                                     const char* file_name, unsigned int line_number);

void wrapper_finish();

} // end extern "C"
