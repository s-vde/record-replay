#pragma once

// PROGRAM_MODEL
#include "instruction.hpp"

// UTILS
#include "debug.hpp"

// STL
#include <assert.h>
#include <unordered_map>

//--------------------------------------------------------------------------------------90
/// @file lockobject.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   class LockObject
   {
   public:
      
      //----------------------------------------------------------------------------------
        
      using oid_t = std::string;
      using WaitSet_t = std::unordered_map<Thread::tid_t,Instruction>;
      
      //----------------------------------------------------------------------------------
		
		/// @brief Constructor.
      
      LockObject(const oid_t& oid);
		
		//----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      oid_t oid() const;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Returns true iff !mLocked (tid enabled).
      /// @note Alternatively, the function could return true if instr is not a lock
      /// operation.

      bool request_lock(const Instruction&);
      
      //----------------------------------------------------------------------------------
        
      /// @returns true iff locking succeeded, i.e. mLocked == false.

      bool lock(const Thread::tid_t&);
      
      //----------------------------------------------------------------------------------
        
      /// @returns true iff unlocking succeeded, i.e. mLocked == true && mOwner == tid.

      bool unlock(const Thread::tid_t& tid);
      
      //----------------------------------------------------------------------------------
        
      WaitSet_t::const_iterator waiting_begin() const;
      
      //----------------------------------------------------------------------------------
        
      WaitSet_t::const_iterator waiting_end() const;
      
      //----------------------------------------------------------------------------------
        
   private:
      
      //----------------------------------------------------------------------------------
        
      oid_t mOid;
        
      bool mLocked;
        
      /// @brief Current owner of this LockObject (-1 if !mLocked).

      Thread::tid_t mOwner;
        
      WaitSet_t mWaitSet;
        
      //----------------------------------------------------------------------------------
        
      friend std::ostream& operator<<(std::ostream&, const LockObject&);
        
   }; // end class LockObject
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream&, const LockObject&);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace program_model
