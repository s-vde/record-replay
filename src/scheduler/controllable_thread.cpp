
#include "controllable_thread.hpp"

#include <thread_io.hpp>

#include <debug.hpp>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

controllable_thread::controllable_thread(const program_model::Thread::tid_t tid,
                                         const std::thread::id owner_id)
: m_tid(tid)
, m_owner_id(owner_id)
, m_control_handle(tid)
{
}

//--------------------------------------------------------------------------------------------------

void controllable_thread::post_task()
{
   DEBUGF_SYNC("[thread" << m_tid << "]", "wait_for_turn", "", std::endl);
   m_control_handle.wait();
   DEBUGF_SYNC("[thread" << m_tid << "]", "take_turn", "", std::endl);
}

//--------------------------------------------------------------------------------------------------

void controllable_thread::enter_function(const std::string& function_name)
{
   DEBUGF_SYNC("[thread" << m_tid << "]", "enter_function", function_name, "\n");
   m_call_stack.push(function_name);
}

//--------------------------------------------------------------------------------------------------

void controllable_thread::exit_function(const std::string& function_name)
{
   DEBUGF_SYNC("[thread" << m_tid << "]", "exit_function", function_name, "\n");
   if (m_call_stack.empty() || m_call_stack.top() != function_name)
      throw std::invalid_argument("invalid call stack");

   m_call_stack.pop();

   if (m_call_stack.empty())
      throw finished();
}

//--------------------------------------------------------------------------------------------------

void controllable_thread::grant_execution_right()
{
   if (std::this_thread::get_id() != m_owner_id)
      throw permission_denied();
   m_control_handle.post(true, utils::threads::BinarySem::BroadcastMode::NOTIFY_ONE);
}

//--------------------------------------------------------------------------------------------------

controllable_thread::permission_denied::permission_denied()
: std::runtime_error("controllable_thread::permission_denied")
{
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
