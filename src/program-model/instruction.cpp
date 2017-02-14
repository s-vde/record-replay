
#include "instruction.hpp"

// STL
#include <sstream>

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   Instruction::Instruction(const Thread::tid_t tid,
                            const Object::Op& op,
                            const Object& obj,
                            const bool is_atomic)
   : mTid(tid)
   , mOp(op)
   , mObj(obj)
   , m_is_atomic(is_atomic) { }
   
   //-------------------------------------------------------------------------------------
    
   Instruction::Instruction()
   : Instruction(-1, Object::Op::READ, Object(), false) { }
   
   //-------------------------------------------------------------------------------------
    
   bool Instruction::operator==(const Instruction& other) const
   {
      return
         mTid == other.mTid &&
         mOp == other.mOp &&
         mObj == other.mObj &&
         m_is_atomic == other.m_is_atomic;
   }
   
   //-------------------------------------------------------------------------------------
    
   Thread::tid_t Instruction::tid() const
   {
      return mTid;
   }
   
   //-------------------------------------------------------------------------------------
    
   const Object::Op& Instruction::op() const
   {
      return mOp;
   }
   
   //-------------------------------------------------------------------------------------
    
   const Object& Instruction::obj() const
   {
      return mObj;
   }
   
   //-------------------------------------------------------------------------------------
   
   bool Instruction::is_atomic() const
   {
      return m_is_atomic;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool is_lock_access(const Instruction& instr)
   {
      return instr.op() == Object::Op::LOCK || instr.op() == Object::Op::UNLOCK;
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace program_model
