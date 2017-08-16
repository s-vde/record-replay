#pragma once

#include "visible_instruction.hpp"

#include <iosfwd>

//--------------------------------------------------------------------------------------------------
/// @file visible_instruction_io.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const meta_data_t& meta_data);

//--------------------------------------------------------------------------------------------------

std::string to_string(const memory_operation& operation);

template <typename memory_location_t>
std::string to_string(const memory_instruction<memory_location_t>& instruction)
{
   std::string str = (instruction.is_atomic() ? "atomic " : "");
   str += to_string(instruction.operation());
   str += " ";
   str += to_string(instruction.operand());
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string(const lock_operation& operation);

template <typename memory_location_t>
std::string to_string(const lock_instruction<memory_location_t>& instruction)
{
   std::string str = to_string(instruction.operation());
   str += " ";
   str += to_string(instruction.operand());
   return str;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
