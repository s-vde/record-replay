
#ifndef INSTRUCTION_HPP_INCLUDED
#define INSTRUCTION_HPP_INCLUDED

#include "object.hpp"
#include "thread.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file instruction.hpp
 @brief Definition of class Instruction.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    class Instruction
    {
    public:
        
        // CTORS / DTOR
        
        Instruction(const Thread::tid_t tid, const Object::Op&, const Object&);
        
        Instruction();
        
        Instruction(const Instruction&) = default;
        Instruction(Instruction&&) = default;
        ~Instruction() = default;
        
        // OPERATORS
        
        Instruction& operator=(const Instruction&) = default;
        Instruction& operator=(Instruction&&) = default;
        
        bool operator==(const Instruction&) const;
        
        //
        
        Thread::tid_t tid() const;
        const Object::Op& op() const;
        const Object& obj() const;
        
    private:
        
        // DATA MEMBERS
        
        Thread::tid_t mTid;
        Object::Op mOp;
        Object mObj;
        
    //friendly:
        
        friend std::istream& operator>>(std::istream&, Instruction&);
        
    }; // end class Instruction
    
    bool is_lock_access(const Instruction&);

} // end namespace program_model

#endif
