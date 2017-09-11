#pragma once

#include <thread.hpp>
#include <visible_instruction.hpp>

#include <unordered_map>

//--------------------------------------------------------------------------------------------------
/// @file thread_state.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

class thread_state
{
public:
   using thread_t = program_model::Thread;
   using instruction_t = program_model::thread_management_instruction;
   using waitset_t = std::unordered_map<thread_t::tid_t, instruction_t>;

   /// @brief Constructor.

   explicit thread_state(thread_t& object);

   bool request(const instruction_t& instr);
   void perform(const thread_t::tid_t& tid);

   waitset_t::const_iterator begin() const;
   waitset_t::const_iterator end() const;

   // std::string str() const;

private:
   thread_t& m_object;
   waitset_t m_waiting;

   friend std::ostream& operator<<(std::ostream&, const thread_state&);

}; // end class object

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream&, const thread_state&);

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
