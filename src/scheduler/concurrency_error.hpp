#pragma once

#include <object.hpp>
#include <visible_instruction.hpp>

#include <exception>
#include <iosfwd>
#include <vector>

//--------------------------------------------------------------------------------------------------
/// @file concurrency_error.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace scheduler {

// Type definitions
using instruction_t = program_model::visible_instruction_t;
using data_race_t = std::pair<program_model::memory_instruction, program_model::memory_instruction>;

std::ostream& write_to_stream(std::ostream& os, const data_race_t& data_race);

//--------------------------------------------------------------------------------------------------

using deadlock_t = std::vector<instruction_t>;

std::ostream& write_to_stream(std::ostream& os, const deadlock_t& deadlock);

//--------------------------------------------------------------------------------------------------

class deadlock_exception : public std::exception
{
public:
   deadlock_exception(const deadlock_t& deadlocks);
   const deadlock_t get() const;

private:
   deadlock_t m_deadlock;

}; // end class deadlock_exception

} // end namespace scheduler
