
#include "VisibleInstructionPass.hpp"

// LLVM
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

namespace concurrency_passes {
//--------------------------------------------------------------------------------------------------
   
VisibleInstructionPass::VisibleInstructionPass(char& ID)
: llvm::ModulePass(ID)
, m_nr_visible_instructions(0)
, m_nr_instrumented(0)
{
}
   
//--------------------------------------------------------------------------------------------------
   
bool VisibleInstructionPass::runOnModule(llvm::Module& module)
{
   try
   {
      onStartOfPass(module);
      auto& functions = module.getFunctionList();
      std::for_each(functions.begin(), functions.end(), [this](auto& function)
      {
         runOnFunction(function);
      });
      onEndOfPass(module);
      
      // print statistics
      llvm::errs() << "number of visible instructions:\t" << m_nr_visible_instructions << "\n";
      llvm::errs() << "number of instrumented instructions:\t" << m_nr_instrumented << "\n";
   }
   catch(const std::exception& e)
   {
      llvm::errs() << e.what() << "\n";
   }
   return false;
}
   
//--------------------------------------------------------------------------------------------------
   
bool VisibleInstructionPass::runOnFunction(llvm::Function& function)
{
   using namespace llvm;

   if (!isBlackListed(function))
   {
      llvm::errs() << "\n--------------------\nrunOnFunction(" << function.getName() << ")\n";
      llvm_visible_instruction::creator creator;
      for (auto inst_it = inst_begin(function); inst_it != inst_end(function); ++inst_it)
      {
         auto& instruction = *inst_it;
         const auto visible_instruction = creator.visit(instruction);
         if (visible_instruction)
         {
            runOnVisibleInstruction(function, inst_it, *visible_instruction);
            ++m_nr_visible_instructions;
         }
         /// @todo Include pthread_exit as visible instruction
         else if (llvm::CallInst* call_instruction = llvm::dyn_cast<llvm::CallInst>(&instruction))
         {
            llvm::Function* callee = call_instruction->getCalledFunction();
            if (callee && callee->getName() == "pthread_exit")
            {
               runOnThreadExit(function, inst_it);
            }
         }
      }
   }
   else
   {
      llvm::errs() << "\n--------------------\n" << function.getName() << " is blacklisted\n";
   }
   return false;
}
   
//--------------------------------------------------------------------------------------------------

bool VisibleInstructionPass::isBlackListed(const llvm::Function& function) const
{
   return false;
}

//--------------------------------------------------------------------------------------------------
} // end namespace concurrency_passes
