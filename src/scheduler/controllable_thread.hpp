#pragma once

#include <thread.hpp>

#include <threads/binary_sem.hpp>

//--------------------------------------------------------------------------------------------------
/// @file controllable_thread.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

class controllable_thread
{
public:
   controllable_thread(const program_model::Thread::tid_t tid, const std::thread::id owner_id);
   void post_task();
   void grant_execution_right();

   struct permission_denied : public std::runtime_error
   {
      permission_denied();
   }; // end struct permission_denied

private:
   /// @brief The id of this thread
   program_model::Thread::tid_t m_tid;

   /// @brief The id of the thread that is controlling this thread
   std::thread::id m_owner_id;

   utils::threads::BinarySem m_control_handle;

}; // end class controllable_thread
} // end namespace scheduler
