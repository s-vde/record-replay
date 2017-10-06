
#include "VisibleInstructionPass.hpp"

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
      std::for_each(functions.begin(), functions.end(),
                    [this, &module](auto& function) { runOnFunction(module, function); });
      onEndOfPass(module);

      // print statistics
      llvm::errs() << "number of visible instructions:\t" << m_nr_visible_instructions << "\n";
      llvm::errs() << "number of instrumented instructions:\t" << m_nr_instrumented << "\n";
   }
   catch (const std::exception& e)
   {
      llvm::errs() << e.what() << "\n";
   }
   return false;
}

//--------------------------------------------------------------------------------------------------

bool VisibleInstructionPass::runOnFunction(llvm::Module& module, llvm::Function& function)
{
   using namespace llvm;

   if (!isBlackListed(function))
   {
      instrumentFunction(module, function);
      llvm_visible_instruction::creator creator;
      for (auto inst_it = inst_begin(function); inst_it != inst_end(function); ++inst_it)
      {
         auto& instruction = *inst_it;
         if (const auto visible_instruction = creator.visit(instruction))
         {
            runOnVisibleInstruction(module, function, inst_it, *visible_instruction);
            ++m_nr_visible_instructions;
         }
      }
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
