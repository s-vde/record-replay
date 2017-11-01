#pragma once

#include <thread.hpp>

#include <threads/binary_sem.hpp>

#include <pthread.h>
#include <stack>
#include <string>

//--------------------------------------------------------------------------------------------------
/// @file controllable_thread.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

class controllable_thread
{
public:
   controllable_thread(const program_model::Thread::tid_t tid, const pthread_t pid,
                       const std::thread::id owner_id);

   /// @brief Should only be called by the thread to be controlled
   void post_task();

   /// @brief Should only be called by the thread to be controlled
   void enter_function(const std::string& function_name);

   /// @brief Should only be called by the thread to be controlled
   void exit_function(const std::string& function_name);

   /// @brief Should only be called by the owning thread
   void grant_execution_right();

   struct permission_denied : public std::runtime_error
   {
      permission_denied();
   }; // end struct permission_denied

   struct finished : public std::exception
   {
   };

private:
   /// @brief The id of this thread
   program_model::Thread::tid_t m_tid;

   /// @brief The pthread id of this thread
   pthread_t m_pid;

   /// @brief The id of the thread that is controlling this thread
   std::thread::id m_owner_id;

   utils::threads::BinarySem m_control_handle;

   using call_stack_t = std::stack<std::string>;
   call_stack_t m_call_stack;

}; // end class controllable_thread
} // end namespace scheduler
