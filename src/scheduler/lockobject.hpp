
#ifndef LOCKOBJECT_HPP_INCLUDED
#define LOCKOBJECT_HPP_INCLUDED

#include <assert.h>
#include <unordered_map>
#include "debug.hpp"
#include "instruction.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file lockobject.hpp
 @brief Definition of class LockObject and declaration of output operator
 overload for LockObject.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    class LockObject
    {
    public:
        
        using oid_t = std::string;
        using WaitSet_t = std::unordered_map<Thread::tid_t,Instruction>;
		
		// CTOR
		
        LockObject(const oid_t& oid);
		
		//
		
        oid_t oid() const;
        
        /**
         @brief Returns true iff !mLocked (tid enabled).
         @note Alternatively, the function could return true if instr
         is not a lock operation.
         */
        bool request_lock(const Instruction&);
        
        /**
         @returns true iff locking succeeded, i.e. mLocked == false.
         */
        bool lock(const Thread::tid_t&);
        
        /**
         @returns true iff unlocking succeeded, i.e. mLocked == true
         && mOwner == tid.
         */
        bool unlock(const Thread::tid_t& tid);
        
        WaitSet_t::const_iterator waiting_begin() const;
        
        WaitSet_t::const_iterator waiting_end() const;
        
    private:
        
        // DATA MEMBERS
        
        oid_t mOid;
        
        bool mLocked;
        
        /**
         @brief Current owner of this LockObject (-1 if !mLocked).
         */
        Thread::tid_t mOwner;
        
        WaitSet_t mWaitSet;
        
    //friendly:
        
        friend std::ostream& operator<<(std::ostream&, const LockObject&);
        
    }; // end class LockObject
    
    std::ostream& operator<<(std::ostream&, const LockObject&);
    
} // end namespace program_model

#endif
