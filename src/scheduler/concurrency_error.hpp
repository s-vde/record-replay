#pragma once

// PROGRAM_MODEL
#include "instruction.hpp"

// STL
#include <exception>
#include <iosfwd>
#include <vector>

//-----------------------------------------------------------------------------------------------100
/// @file concurrency_error.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

namespace scheduler
{
//--------------------------------------------------------------------------------------------------

using data_race_t = std::pair<program_model::Instruction, program_model::Instruction>;

std::ostream& write_to_stream(std::ostream& os, const data_race_t& data_race);

//-------------------------------------------------------------------------------------------------- 

using deadlock_t = std::vector<program_model::Instruction>;

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

//-------------------------------------------------------------------------------------------------- 
} // end namespace scheduler
