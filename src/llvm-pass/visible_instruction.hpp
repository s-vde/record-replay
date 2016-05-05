
#ifndef VISIBLE_INSTRUCTION_HPP_INCLUDED
#define VISIBLE_INSTRUCTION_HPP_INCLUDED

#include "llvm/IR/Module.h"
#include "object.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file visible_instruction.hpp
 @brief Dfinition of class VisibleInstruction and declaration of free 
 functions for returning a VisibleInstruction from a visible llvm::Instruction.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using Op = program_model::Object::Op;

namespace record_replay
{
    class VisibleInstruction
    {
    public:
        
        // LIFETIME
        
        explicit VisibleInstruction(
            const Op& op=Op::READ,
            llvm::GlobalVariable* gvar=nullptr,
            llvm::Value* index=nullptr
        );
        
        VisibleInstruction(const VisibleInstruction&) = default;
        VisibleInstruction(VisibleInstruction&&) = default;
        ~VisibleInstruction() = default;
        
        // OPERATORS
        
        VisibleInstruction& operator=(const VisibleInstruction&) = default;
        VisibleInstruction& operator=(VisibleInstruction&&) = default;
        
        //
        
        const Op& op() const;
        
        /**
         @brief Adds the construction of a program_model::Object corresponding
         to this VisibleInstruction after I.
         */
        llvm::Value* object(llvm::Module& M, llvm::Instruction* I) const;
        
    private:
        
        // DATA MEMBERS
        
        Op mOp;
        llvm::GlobalVariable* mGVar;
        llvm::Value* mIndex;
        
    }; // end class VisibleInstruction
    
    std::pair<bool,VisibleInstruction> is_visible(llvm::Instruction* I);
    
    std::pair<bool,VisibleInstruction> is_visible(
        const Op& op,
        llvm::Value* operand
    );
    
    std::pair<bool,VisibleInstruction> is_visible(
        const Op& op,
        llvm::Value* ptrop,
        llvm::Value* indexop1,
        llvm::Value* indexop2
    );
    
} // end namespace record_replay

#endif
