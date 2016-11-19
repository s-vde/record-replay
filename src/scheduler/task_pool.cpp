
#include "task_pool.hpp"

// PROGRAM_MODEL
#include "object_io.hpp"
#include "instruction_io.hpp"
#include "thread_io.hpp"

// UTILS
#include "debug.hpp"
#include "utils_io.hpp"
#include "utils_map.hpp"

// STL
#include <algorithm>
#include <assert.h>
#include <iostream>

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   TaskPool::TaskPool(const unsigned int nr_threads)
   : mMutex()
   , mModified()
   , mTasks()
   , mCurrentTask(nullptr)
   , mLockObs()
   , mLockObsMutex() { }
   
   //-------------------------------------------------------------------------------------
    
   void TaskPool::register_thread(const Thread::tid_t& tid)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      mThreads.insert(Threads::value_type(tid, Thread(tid)));
      mThreads.find(tid)->second.set_status(Thread::Status::ENABLED);
      DEBUG(mThreads.find(tid)->second);
   }
   
   //-------------------------------------------------------------------------------------
    
   /// @see TaskPool::post_lock_instruction
 
   void TaskPool::post(const Thread::tid_t& tid, const Instruction& task)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      auto task_it(mTasks.find(tid));
      /// @pre mTasks.find(tid) == mTasks.end()
      assert(task_it == mTasks.end());
      mTasks.insert(Tasks::value_type(tid, task));
      if (task.op() == Object::Op::LOCK)
      {
         post_lock_instruction(tid, task);
      }
      else
      {
         set_status(tid, Thread::Status::ENABLED);
      }
      // cond SIGNAL mModified
      mModified.notify_one();
   }
   
   //-------------------------------------------------------------------------------------
    
   void TaskPool::yield(const Thread::tid_t& tid)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      DEBUGFNL("Taskpool", "yield", tid, "");
      /// @pre mCurrentTask->tid == tid
      assert(mCurrentTask != nullptr && mCurrentTask->tid() == tid);
      if (is_lock_access(*mCurrentTask))
      {
         update_lockobj(*mCurrentTask);
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void TaskPool::wait_enabled_collected()
   {
      std::unique_lock<std::mutex> ul(mMutex);
      // cond WAIT mModified
      mModified.wait(ul, [this] {
         DEBUGFNL("TaskPool", "wait_enabled_collected", "", "");
         return all_enabled_collected();
      });
   }
   
   //-------------------------------------------------------------------------------------
    
   void TaskPool::wait_all_finished()
   {
      std::unique_lock<std::mutex> ul(mMutex);
      // cond WAIT mModified
      mModified.wait(ul, [this] {
         DEBUGFNL("TaskPool", "wait_all_finished", "", "");
         return all_finished();
      });
   }
   
   //-------------------------------------------------------------------------------------

   Instruction TaskPool::set_current(const Thread::tid_t& tid)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      auto it = mTasks.find(tid);
      /// @pre mTasks.find(tid) != mTasks.end()
      assert(it != mTasks.end());
      mCurrentTask = std::shared_ptr<Instruction>(new Instruction(it->second));
      mTasks.erase(it);       // noexcept
      return *mCurrentTask;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool TaskPool::has_next(const Thread::tid_t& tid) const
   {
      return mTasks.find(tid) != mTasks.end();
   }
   
   //-------------------------------------------------------------------------------------
    
   TaskPool::Tasks::const_iterator TaskPool::task(const Thread::tid_t& tid) const
   {
      return mTasks.find(tid);
   }
   
   //-------------------------------------------------------------------------------------
   
   TaskPool::Tasks::const_iterator TaskPool::tasks_cend() const
   {
      return mTasks.cend();
   }
   
   //-------------------------------------------------------------------------------------
    
   std::shared_ptr<const Instruction> TaskPool::current_task() const
   {
      return mCurrentTask;
   }
   
   //-------------------------------------------------------------------------------------
    
   size_t TaskPool::size() const
   {
      return mTasks.size();
   }

   //-------------------------------------------------------------------------------------
   
   Thread::Status TaskPool::status_protected(const Thread::tid_t& tid)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      return status(tid);
   }
   
   //-------------------------------------------------------------------------------------

   void TaskPool::set_status_protected(const Thread::tid_t& tid,
                                       const Thread::Status& status)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      set_status(tid, status);
      if (status == Thread::Status::DISABLED || status == Thread::Status::FINISHED)
      {
         mModified.notify_one();
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   Tids TaskPool::enabled_set() const
   {
      /// @see utils::maps::keys
      return utils::maps::keys(mThreads,
                               [] (const auto& thread)
                               {
                                  return thread.status() == Thread::Status::ENABLED;
                               });
   }
   
   //-------------------------------------------------------------------------------------
    
   Tids TaskPool::enabled_set_protected()
   {
      std::lock_guard<std::mutex> guard(mMutex);
      return enabled_set();
   }
   
   //-------------------------------------------------------------------------------------
    
   NextSet TaskPool::nextset_protected()
   {
      using zip_function_t = std::function<next_t(Instruction, Thread)>;
      const static zip_function_t zip_function = [] (const auto& task, const auto& thread)
      {
         return next_t{ task, thread.status() == Thread::Status::ENABLED };
      };
      std::lock_guard<std::mutex> guard(mMutex);
      /// @see utils::maps::zip
      /// @throws std::out_of_range (rethrow)
      return utils::maps::zip(mTasks, mThreads, zip_function);
   }
   
   //-------------------------------------------------------------------------------------
    
    std::unique_ptr<State> TaskPool::program_state()
    {
      std::lock_guard<std::mutex> guard(mMutex);
      NextSet N{};
      Tids Enabled{};
      for (const auto& entry : mTasks)
      {
         bool enabled = (status(entry.first) == Thread::Status::ENABLED);
         N.insert(NextSet::value_type(entry.first, next_t{ entry.second, enabled }));
         if (enabled)
         {
            Enabled.insert(entry.first);
         }
      }
      return std::make_unique<State>(std::move(Enabled), std::move(N));
   }
   
   //-------------------------------------------------------------------------------------

   Thread::Status TaskPool::status(const Thread::tid_t& tid) const
   {
      auto it = mThreads.find(tid);
      /// @pre mThreads.find(tid) != mThreads.end()
      assert(it != mThreads.end());
      return it->second.status();
   }
   
   //-------------------------------------------------------------------------------------

   void TaskPool::set_status(const Thread::tid_t& tid, const Thread::Status& status)
   {
      auto thread_it = mThreads.find(tid);
      assert(thread_it != mThreads.end());
      thread_it->second.set_status(status);
   }
   
   //-------------------------------------------------------------------------------------

   /// @note If task is a LOCK Instruction, its status has to be set within the same
   /// mLockObsMutex block as the request_lock statement, because otherwise the
   /// enabledness may be changed by a task_done by the lock object's owner.
   /// @see LockObject::request_lock

   void TaskPool::post_lock_instruction(const Thread::tid_t& tid, const Instruction& task)
   {
      const std::string name(to_pretty_string(task.obj()));
      std::lock_guard<std::mutex> guard(mLockObsMutex);
      // Create a new LockObject if one with name does not exist in mLockObs
      if (mLockObs.find(name) == mLockObs.end())
      {
         mLockObs.insert(std::pair<std::string,LockObject>(name, LockObject(name)));
      }
      bool enabled = mLockObs.find(name)->second.request_lock(task);
      set_status(tid, (enabled ? Thread::Status::ENABLED : Thread::Status::DISABLED));
   }
   
   //-------------------------------------------------------------------------------------
    
   void TaskPool::update_lockobj(const Instruction& task)
   {
      /// @pre task == *mCurrentTask && is_lock_access(task)
      assert(task == *mCurrentTask && is_lock_access(task));
      DEBUGFNL("Taskpool", "update_lockobj", task, "");

      std::lock_guard<std::mutex> guard(mLockObsMutex);
      auto obj = mLockObs.find(to_pretty_string(task.obj()));
      /// @pre mLockObs.find(task.obj()) != mLockObs.end()
      assert(obj != mLockObs.end());
      bool lock = task.op() == Object::Op::LOCK;
      bool succeed = lock ? obj->second.lock(task.tid()) : obj->second.unlock(task.tid());
      /// @pre lock -> obj->second.lock(task.tid()) && !lock -> obj->second.unlock(task.tid())
      assert(succeed);
      set_status_of_waiting_on(obj->second,
                               (lock ? Thread::Status::DISABLED : Thread::Status::ENABLED));
   }
   
   //-------------------------------------------------------------------------------------

   void TaskPool::set_status_of_waiting_on(const LockObject& obj,
                                           const Thread::Status& status)
   {
      DEBUGFNL("TaskPool", "set_status_of_waiting_on", obj.oid() << ", " << to_string(status), "");
      std::for_each(
         obj.waiting_begin(), obj.waiting_end(),
         [&status, this] (const auto& request) { set_status(request.first, status); }
      );
   }
   
   //-------------------------------------------------------------------------------------

   bool TaskPool::all_enabled_collected() const
   {
      return std::all_of(mThreads.begin(), mThreads.end(),
                         [this] (const auto& thread)
                         {
                            return
                              thread.second.status() != Thread::Status::ENABLED ||
                              mTasks.find(thread.first) != mTasks.end();
                         });
   }
   
   //-------------------------------------------------------------------------------------
   
   bool TaskPool::all_finished() const
   {
      return std::all_of(mThreads.begin(), mThreads.end(),
                         [] (const auto& thread)
                         {
                            return thread.second.status() != Thread::Status::FINISHED;
                         });
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
