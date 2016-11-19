#pragma once

// SCHEDULER
#include "lockobject.hpp"

// PROGRAM_MODEL
#include "state.hpp"

// STL
#include <thread>
#include <unordered_map>

using namespace program_model;

//--------------------------------------------------------------------------------------90
/// @file task_pool.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   /// TaskPool encapsulates
   /// - a map mThreads mapping Thread::tid_t's to a Thread object;
   /// - a map mTasks mapping Thread::tid_t's to the next Instruction posted by the
   /// corresponding Thread.
   /// A TaskPool is equiped with a locking mechanism that allows threads to safely
   /// operate on its data concurrently.
   /// @note The TaskPool assumes that threads don't cheat and post under false id.

   class TaskPool
   {
   public:
      
      //----------------------------------------------------------------------------------

      using Tasks = std::unordered_map<Thread::tid_t,Instruction>;
      using Threads = std::unordered_map<Thread::tid_t,Thread>;
      using LockObs = std::unordered_map<std::string,LockObject>;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Mytex protecting mTasks, mStatus, mNr_registered, and mModified.

      std::mutex mMutex;
      
      //----------------------------------------------------------------------------------

      /// @brief Used to conditionally wait until for a relevant change in the TaskPool.
      /// @details A relevant change (i.e. a Thread's became DISABLED or FINISHED, or a
      /// new task has been posted, both affecting whether or not all enabled Threads have
      /// posted their task).

      std::condition_variable mModified;
      
      //----------------------------------------------------------------------------------
      
      /// @brief Constructor.
        
      explicit TaskPool(const unsigned int nr_threads);
      
      //----------------------------------------------------------------------------------
      
      TaskPool(const TaskPool&) = delete;
      TaskPool(TaskPool&&) = delete;
      ~TaskPool() = default;
      TaskPool& operator=(const TaskPool&) = delete;
      TaskPool& operator=(TaskPool&&) = delete;
        
      //----------------------------------------------------------------------------------

      /// @brief Registers a thread with Thread::tid tid to the TaskPool by creating an
      /// entry for it in mThreads. The Thread is registered with Thread::Status ENABLED.

      void register_thread(const Thread::tid_t& tid);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Posts the given task for Thread tid in mTasks.

      void post(const Thread::tid_t& tid, const Instruction& task);
      
      //----------------------------------------------------------------------------------
    
      /// @brief Handles a yield of the currently executing Thread.
      
      void yield(const Thread::tid_t& tid);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Wait until all ENABLED threads have posted a task.
  
      void wait_enabled_collected();
      
      //----------------------------------------------------------------------------------
        
      /// @brief Wait until all registered threads are FINISHED.

      void wait_all_finished();
      
      //----------------------------------------------------------------------------------

      /// @brief Sets mCurrentTask to the task posted by Thread tid and removes the
      /// Instruction from mTasks. Returns a copy of the current task.

      Instruction set_current(const Thread::tid_t& tid);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Returns the size of mTasks.

      size_t size() const;
      
      //----------------------------------------------------------------------------------
        
      bool has_next(const Thread::tid_t& tid) const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Find function on mTasks.

      Tasks::const_iterator task(const Thread::tid_t& tid) const;
      
      //----------------------------------------------------------------------------------
        
      Tasks::const_iterator tasks_cend() const;
      
      //----------------------------------------------------------------------------------
        
      std::shared_ptr<const Instruction> current_task() const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief mMutex-protected read-only access to the status of Thread tid.

      Thread::Status status_protected(const Thread::tid_t& tid);
      
      //----------------------------------------------------------------------------------

      /// @brief mMutex-protected write-access to the status of Thread tid.
      /// @details Signals mModified if status is set to DISABLED or FINISHED.

      void set_status_protected(const Thread::tid_t&, const Thread::Status&);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Returns { tid | mThreads[tid].status = ENABLED }.
      /// @note The unprotected version is needed in Scheduler::select.
      
      Tids enabled_set() const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief mMutex-protected version of TaskPool::enabled_set.

      Tids enabled_set_protected();
      
      //----------------------------------------------------------------------------------
        
      /// @brief Constructs and returns the NextSet
      /// { (tid, (mTasks[tid], mThread[tid.status == ENABLED)) } from this TaskPool.

      NextSet nextset_protected();
      
      //----------------------------------------------------------------------------------

      /// @brief Creates a program_model::State object from this TaskPool and returns a
      /// unique_ptr to it.

      std::unique_ptr<State> program_state();
      
      //----------------------------------------------------------------------------------
        
   private:
        
      //----------------------------------------------------------------------------------

      /// @brief Datastrucure mapping Thread::tid_t's to the associated Thread's posted
      /// next task.

      Tasks mTasks;
      
      //----------------------------------------------------------------------------------
        
      /// @brief A shared pointer to the task currently being executed.

      std::shared_ptr<Instruction> mCurrentTask;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Datastructure mapping Thread::tid_t's to the associated Thread.
      
      Threads mThreads;
      
      //----------------------------------------------------------------------------------

      /// @brief Datastructure containing the LockObjects operated on by the program.

      LockObs mLockObs;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Mutex protecting mLockObs.

      std::mutex mLockObsMutex;
      
      //----------------------------------------------------------------------------------
        
      // HELPER FUNCTIONS

      /// @brief Unprotected read-only access to the status of Thread tid.

      Thread::Status status(const Thread::tid_t& tid) const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Unprotected write access to the status of Thread tid.

      void set_status(const Thread::tid_t&, const Thread::Status&);
      
      //----------------------------------------------------------------------------------

      /// @brief Takes care of registration of tasks operating on a lock object.
      /// @details Enabledness of such a task is determined based on its Op op and the
      /// status of the Object obj it operates on.
      /// - If op == LOCK, the task is enabled iff !obj.locked;
      /// - If op == UNLOCK, we assume that tid holds the lock obj and the task is
      /// enabled;
      /// - Otherwise, the task is enabled.
      /// If obj is not yet registered in mLockObs, an entry is created.

      void post_lock_instruction(const Thread::tid_t& tid, const Instruction& task);
      
      //----------------------------------------------------------------------------------

      /// @brief Updates the status of the Object the given task operates on.
      /// @details
      /// - If task.op == LOCK, all threads waiting on obj are set to DISABLED in mThreads.
      /// - If task.op == UNLOCK, all threads waiting on obj are set to ENABLED in mThreads.
      /// @returns true iff locking/unlocking the Object succeeded.
      
      void  update_lockobj(const Instruction& task);
      
      //----------------------------------------------------------------------------------

      /// @brief Set the status of all Threads with lock requests on obj to the given one.

      void set_status_of_waiting_on(const LockObject& obj, const Thread::Status&);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Returns whether all ENABLED threads have posted a task.

      bool all_enabled_collected() const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Returns whether all registered threads are FINISHED.

      bool all_finished() const;
      
      //----------------------------------------------------------------------------------
        
   }; // end class TaskPool
   
   //-------------------------------------------------------------------------------------
    
   // #todo Keep a container of Threads that have unknown status to make the
   // implementation more natural.
   // #todo mMutex and mModified should be private.
   // #todo Use tasks datastructure compatible with State's datastructures.
   // #todo current_task() returns a pointer to shared object.
    
} // end namespace scheduler
