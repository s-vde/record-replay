
#include "instruction_io.hpp"

#include "instruction.hpp"
#include "object_io.hpp"

#include <iostream>

namespace program_model {

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Instruction& instruction)
{
   os << instruction.tid() << " " << instruction.op() << " " << instruction.obj() << " "
      << (instruction.is_atomic() ? "atomic" : "nonatomic");
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, Instruction& instruction)
{
   std::string atomic_str;
   is >> instruction.mTid >> instruction.mOp >> instruction.mObj >> atomic_str;
   instruction.m_is_atomic = (atomic_str == "atomic");
   return is;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
