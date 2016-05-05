
#include "instruction.hpp"
#include <sstream>

namespace program_model
{
    Instruction::Instruction(
        const Thread::tid_t tid,
        const Object::Op& op,
        const Object& obj)
    : mTid(tid)
    , mOp(op)
    , mObj(obj) { }
    
    Instruction::Instruction()
    : Instruction(-1, Object::Op::READ, Object()) { }
    
    bool Instruction::operator==(const Instruction& other) const {
        return
            mTid == other.mTid &&
            mOp == other.mOp &&
            mObj == other.mObj;
    }
    
    Thread::tid_t Instruction::tid() const
    {
        return mTid;
    }
    
    const Object::Op& Instruction::op() const
    {
        return mOp;
    }
    
    const Object& Instruction::obj() const
    {
        return mObj;
    }
    
    bool is_lock_access(const Instruction& instr)
    {
        return
            instr.op() == Object::Op::LOCK
            || instr.op() == Object::Op::UNLOCK;
    }
    
} // end namespace program_model
