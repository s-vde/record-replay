#pragma once

#include "concurrency_error.hpp"
#include "object_state.hpp"
#include "thread_state.hpp"

#include "state.hpp"

#include <thread>
#include <unordered_map>

using namespace program_model;

//--------------------------------------------------------------------------------------------------
/// @file task_pool.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

/// TaskPool encapsulates
/// - a map mThreads mapping Thread::tid_t's to a Thread object;
/// - a map mTasks mapping Thread::tid_t's to the next instruction posted by the
/// corresponding Thread.
/// A TaskPool is equiped with a locking mechanism that allows threads to safely
/// operate on its data concurrently.
/// @note The TaskPool assumes that threads don't cheat and post under false id.

class TaskPool
{
public:
   // Type definitions
   using object_t = program_model::Object;
   using instruction_t = program_model::visible_instruction_t;
   using Tasks = std::unordered_map<Thread::tid_t, instruction_t>;
   using Threads = std::unordered_map<Thread::tid_t, Thread>;
   using objects_t = std::unordered_map<object_t::ptr_t, object_state>;
   using thread_states_t = std::unordered_map<Thread::tid_t, thread_state>;

   /// @brief Mytex protecting mTasks, mStatus, mNr_registered, and mModified.

   std::mutex mMutex;

   /// @brief Used to conditionally wait until for a relevant change in the TaskPool.
   /// @details A relevant change (i.e. a Thread's became DISABLED or FINISHED, or a
   /// new task has been posted, both affecting whether or not all enabled Threads have
   /// posted their task).

   std::condition_variable mModified;

   /// @{
   /// Lifetime
   TaskPool() = default;
   TaskPool(const TaskPool&) = delete;
   TaskPool(TaskPool&&) = delete;
   ~TaskPool() = default;
   TaskPool& operator=(const TaskPool&) = delete;
   TaskPool& operator=(TaskPool&&) = delete;
   /// @}

   /// @brief Registers a thread with Thread::tid tid to the TaskPool by creating an
   /// entry for it in mThreads. The Thread is registered with Thread::Status ENABLED.

   void register_thread(const Thread::tid_t& tid);

   /// @brief Posts the given task for Thread tid in mTasks.

   void post(const Thread::tid_t& tid, const instruction_t& task);

   /// @brief Handles a yield if tid is the currently executing Thread.

   void yield(const Thread::tid_t& tid);
   
   /// @brief Handles a finished thread.
   
   void finish(const Thread::tid_t& tid);

   /// @brief Wait until all unfinished threads have posted a task.

   void wait_until_unfinished_threads_have_posted();

   /// @brief Wait until all registered threads are FINISHED.

   void wait_all_finished();

   /// @brief Sets mCurrentTask to the task posted by Thread tid and removes the
   /// instruction from mTasks. Returns a copy of the current task.

   instruction_t set_current(const Thread::tid_t& tid);

   /// @brief Returns the size of mTasks.

   size_t size() const;

   bool has_next(const Thread::tid_t& tid) const;

   /// @brief Find function on mTasks.

   Tasks::const_iterator task(const Thread::tid_t& tid) const;

   Tasks::const_iterator tasks_cbegin() const;
   Tasks::const_iterator tasks_cend() const;

   std::shared_ptr<const instruction_t> current_task() const;

   /// @brief mMutex-protected read-only access to the status of Thread tid.

   Thread::Status status_protected(const Thread::tid_t& tid);

   /// @brief mMutex-protected write-access to the status of Thread tid.
   /// @details Signals mModified if status is set to DISABLED or FINISHED.

   void set_status_protected(const Thread::tid_t&, const Thread::Status&);

   /// @brief Returns { tid | mThreads[tid].status = ENABLED }.
   /// @note The unprotected version is needed in Scheduler::select.

   Tids enabled_set() const;

   /// @brief mMutex-protected version of TaskPool::enabled_set.

   Tids enabled_set_protected();

   /// @brief Constructs and returns the NextSet
   /// { (tid, (mTasks[tid], mThread[tid.status == ENABLED)) } from this TaskPool.

   NextSet nextset_protected();

   /// @brief Creates a program_model::State object from this TaskPool and returns a
   /// unique_ptr to it.

   std::unique_ptr<State> program_state();

   std::vector<data_race_t> data_races() const;

private:
   /// @brief Datastrucure mapping Thread::tid_t's to the associated Thread's posted
   /// next task.

   Tasks mTasks;

   /// @brief A shared pointer to the task currently being executed.

   std::shared_ptr<instruction_t> mCurrentTask;

   /// @brief Datastructure mapping Thread::tid_t's to the associated Thread.

   Threads mThreads;

   /// @brief Datastructure containing the objects operated on by the program.

   objects_t m_objects;
   thread_states_t m_thread_states;

   std::vector<data_race_t> m_data_races;

   /// @brief Mutex protecting m_objects and m_data_races.

   mutable std::mutex m_objects_mutex;

   // HELPER FUNCTIONS

   /// @brief Unprotected read-only access to the status of Thread tid.

   Thread::Status status(const Thread::tid_t& tid) const;

   /// @brief Unprotected write access to the status of Thread tid.

   void set_status(const Thread::tid_t&, const Thread::Status&);

   void update_object_post(const Thread::tid_t& tid, const instruction_t& task);

   void update_object_yield(const instruction_t& task);

   /// @brief Set the status of all Threads with lock requests on obj to the given one.

   void set_status_of_waiting_on(const object_state& obj, const Thread::Status&);

   /// @brief Returns whether all registered threads are FINISHED.

   bool all_finished() const;

}; // end class TaskPool

std::ostream& operator<<(std::ostream&, TaskPool&);

} // end namespace scheduler
