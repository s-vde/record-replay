
#include "lockobject.hpp"

// PROGRAM_MODEL
#include "instruction_io.hpp"

// UTILS
#include "utils_io.hpp"

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   LockObject::LockObject(const oid_t& oid)
   : mOid(oid)
   , mLocked(false)
   , mOwner(-1)
   , mWaitSet() { }
   
   //-------------------------------------------------------------------------------------
    
   LockObject::oid_t LockObject::oid() const
   {
      return mOid;
   }
   
   //-------------------------------------------------------------------------------------
    
   LockObject::WaitSet_t::const_iterator LockObject::waiting_begin() const
   {
      return mWaitSet.cbegin();
   }
   
   //-------------------------------------------------------------------------------------
    
   LockObject::WaitSet_t::const_iterator LockObject::waiting_end() const
   {
      return mWaitSet.cend();
   }
   
   //-------------------------------------------------------------------------------------
    
   bool LockObject::request_lock(const Instruction& instr)
   {
      /// @pre instr.op == Object::Op::LOCK && mWaitSet.find(instr.tid) == mWaitSet.end()
      assert(instr.op() == Object::Op::LOCK && mWaitSet.find(instr.tid()) == mWaitSet.end());
      DEBUGFNL("LockObject" << mOid, "request_lock", instr, "");
      mWaitSet.insert(WaitSet_t::value_type(instr.tid(), instr));
      DEBUGNL(*this);
      return !mLocked;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool LockObject::lock(const Thread::tid_t& tid)
   {
      DEBUGFNL("LockObject" << mOid, "lock", tid, "");
      if (!mLocked)
      {
         /// @pre mWaitSet.find(tid) != mWaitSet.end()
         assert(mWaitSet.find(tid) != mWaitSet.end());
         mLocked = true;
         mOwner = tid;
         mWaitSet.erase(tid);
         return true;
      }
      return false;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool LockObject::unlock(const Thread::tid_t& tid)
   {
      DEBUGFNL("LockObject" << mOid, "unlock", tid, "");
      if (mLocked && tid == mOwner)
      {
         mLocked = false;
         mOwner = -1;
         return true;
      }
      return false;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream& os, const LockObject& obj)
   {
      os << "LockObject(oid = " << obj.mOid << " status=";
      if (obj.mLocked)    { os << "locked by " << obj.mOwner; }
      else                { os << "available";                }
      os << " waiting={";
      for (const auto& it : obj.mWaitSet)
      {
         os << utils::io::to_string(it.second) << " ";
      }
      os << "}";
      return os;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
