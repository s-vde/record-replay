#pragma once

#include "object_io.hpp"
#include "thread_io.hpp"
#include "visible_instruction.hpp"

#include <iostream>

//--------------------------------------------------------------------------------------------------
/// @file visible_instruction_io.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const meta_data_t& meta_data);
std::istream& operator>>(std::istream& is, meta_data_t& meta_data);

//--------------------------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& os, const memory_operation& operation);
std::istream& operator>>(std::istream& is, memory_operation& operation);

template <typename thread_id_t, typename memory_location_t>
std::ostream& operator<<(
   std::ostream& os, const detail::memory_instruction<thread_id_t, memory_location_t>& instruction)
{
   os << "memory_instruction " << (instruction.is_atomic() ? "atomic " : "non-atomic ")
      << instruction.tid() << " " << instruction.operation() << " "
      << to_string(instruction.operand()) << " " << instruction.meta_data();
   return os;
}

//--------------------------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& os, const lock_operation& operation);
std::istream& operator>>(std::istream& is, lock_operation& operation);

template <typename thread_id_t, typename memory_location_t>
std::ostream& operator<<(
   std::ostream& os, const detail::lock_instruction<thread_id_t, memory_location_t>& instruction)
{
   os << "lock_instruction " << instruction.tid() << " " << instruction.operation()
      << " " << to_string(instruction.operand()) << " " << instruction.meta_data();
   return os;
}

//--------------------------------------------------------------------------------------------------


std::ostream& operator<<(std::ostream& os, const thread_management_operation& operation);
std::istream& operator>>(std::istream& is, thread_management_operation& operation);

template <typename thread_id_t, typename thread_t>
std::ostream& operator<<(
   std::ostream& os,
   const detail::thread_management_instruction<thread_id_t, thread_t>& instruction)
{
   os << "thread_management_instruction " << instruction.tid() << " "
      << instruction.operation() << " " << instruction.operand() << " "
      << instruction.meta_data();
   return os;
}

//--------------------------------------------------------------------------------------------------


template <typename thread_id_t, typename memory_location_t, typename thread_t>
std::istream& operator>>(
   std::istream& is,
   detail::visible_instruction_t<thread_id_t, memory_location_t, thread_t>& instruction)
{
   std::string type;
   is >> type;
   if (type == "memory_instruction")
   {
      std::string atomic;
      thread_id_t tid;
      memory_operation operation;
      memory_location_t operand;
      meta_data_t meta_data;
      is >> atomic >> tid >> operation >> operand >> meta_data;
      instruction = detail::memory_instruction<thread_id_t, memory_location_t>(
         tid, operation, operand, atomic == "atomic", meta_data);
   }
   else if (type == "lock_instruction")
   {
      thread_id_t tid;
      lock_operation operation;
      memory_location_t operand;
      meta_data_t meta_data;
      is >> tid >> operation >> operand >> meta_data;
      instruction = detail::lock_instruction<thread_id_t, memory_location_t>(tid, operation,
                                                                             operand, meta_data);
   }
   else if (type == "thread_management_instruction")
   {
      thread_id_t tid;
      thread_management_operation operation;
      thread_t operand;
      meta_data_t meta_data;
      is >> tid >> operation >> operand >> meta_data;
      instruction = detail::thread_management_instruction<thread_id_t, thread_t>(
         tid, operation, operand, meta_data);
   }
   else
   {
      is.setstate(std::ios::failbit);
   }
   return is;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
