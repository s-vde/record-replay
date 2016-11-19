
#include "instruction_io.hpp"

// PROGRAM_MODEL
#include "instruction.hpp"
#include "object_io.hpp"

// STL
#include <iostream>

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::ostream& operator<<(std::ostream& os, const Instruction& instr)
   {
      os << instr.tid() << " " << instr.op() << " " << instr.obj();
      return os;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::istream& operator>>(std::istream& is, Instruction& instr)
   {
      is >> instr.mTid >> instr.mOp >> instr.mObj;
      return is;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
