
#include "thread_state.hpp"

#include <visible_instruction_io.hpp>

#include <debug.hpp>
#include <utils_io.hpp>

#include <algorithm>
#include <assert.h>
#include <exception>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

thread_state::thread_state(thread_t& object)
: m_object(object)
{
}

//--------------------------------------------------------------------------------------------------

bool thread_state::request(const instruction_t& instr)
{
   DEBUGF_SYNC("thread_state", "request", instr, "\n");
   using namespace program_model;
   if (instr.operation() == thread_management_operation::Join)
   {
      if (m_waiting.find(instr.tid()) == m_waiting.end())
      {
         m_waiting.insert({instr.tid(), instr});
         DEBUG_SYNC(*this << "\n");
         return m_object.status() == Thread::Status::FINISHED;
      }
      throw std::logic_error("requesting thread already has instruction waiting");
   }
   return true;
}

//--------------------------------------------------------------------------------------------------

void thread_state::perform(const thread_t::tid_t& tid)
{
   DEBUGF_SYNC("thread_state", "perform", tid, "\n");
   const auto it = m_waiting.find(tid);
   if (it != m_waiting.end())
   {
      m_waiting.erase(it);
      DEBUG_SYNC(*this << "\n");
      return;
   }
   throw std::invalid_argument("requesting thread has no instruction waiting");
}

//--------------------------------------------------------------------------------------------------

auto thread_state::begin() const -> waitset_t::const_iterator
{
   return m_waiting.begin();
}

//--------------------------------------------------------------------------------------------------

auto thread_state::end() const -> waitset_t::const_iterator
{
   return m_waiting.end();
}

// //--------------------------------------------------------------------------------------------------
// 
// std::string thread_state::str() const
// {
//    return to_string(m_object);
// }

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const thread_state& state)
{
   os << "thread_state(thread=" << state.m_object << " waiting={";
   for (const auto& it : state.m_waiting)
   {
      os << utils::io::to_string(it.second) << " ";
   }
   os << "}";
   return os;
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
