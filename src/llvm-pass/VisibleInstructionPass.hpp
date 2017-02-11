#pragma once

// VISIBLE_INSTRUCTION_PASS
#include "llvm_visible_instruction.hpp"

// LLVM
#include <llvm/IR/InstIterator.h>
#include <llvm/Pass.h>

//-----------------------------------------------------------------------------------------------100
/// @file VisibleInstructionPass.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

namespace llvm
{
//--------------------------------------------------------------------------------------------------
   
// forward declarations
class Instruction;
class Function;
class Module;
   
//--------------------------------------------------------------------------------------------------
} // end namespace llvm


namespace concurrency_passes
{
//--------------------------------------------------------------------------------------------------
   
class VisibleInstructionPass : public llvm::ModulePass
{
public:
      
   /// @brief Constructor.
   VisibleInstructionPass(char& ID);
      
   virtual void onStartOfPass(llvm::Module& module) = 0;
   virtual void runOnVisibleInstruction(llvm::Function& function,
                                        llvm::inst_iterator inst_it,
                                        const visible_instruction_t& visible_instruction) = 0;
   virtual void runOnThreadExit(llvm::Function& function, llvm::inst_iterator inst_it) = 0;
   virtual void onEndOfPass(llvm::Module& module) = 0;
   
protected:
   
   unsigned int m_nr_instrumented;
      
private:
      
   bool runOnModule(llvm::Module& module) override;
   bool runOnFunction(llvm::Function& function);
   
   virtual bool isBlackListed(const llvm::Function& function) const;
      
   /// @brief The number of visible instructions encountered during the pass.
   unsigned int m_nr_visible_instructions;
      
}; // end class VisibleInstructionPass
   
//--------------------------------------------------------------------------------------------------
} // end namespace concurrency_passes
