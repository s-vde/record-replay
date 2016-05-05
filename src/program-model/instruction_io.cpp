
#include "instruction_io.hpp"
#include <iostream>
#include "instruction.hpp"
#include "object_io.hpp"

namespace program_model
{
    std::ostream& operator<<(std::ostream& os, const Instruction& instr)
    {
        os << instr.tid() << " " << instr.op() << " " << instr.obj();
        return os;
    }
    
    std::istream& operator>>(std::istream& is, Instruction& instr)
    {
        is >> instr.mTid >> instr.mOp >> instr.mObj;
        return is;
    }
} // end namespace program_model
