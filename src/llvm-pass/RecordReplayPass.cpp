
#include "RecordReplayPass.hpp"

#include "instrumentation_utils.hpp"
#include "print.hpp"

#include "color_output.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <cxxabi.h>

#include <exception>


namespace concurrency_passes {
    
//--------------------------------------------------------------------------------------------------

char LightWeightPass::ID = 0;

//--------------------------------------------------------------------------------------------------

LightWeightPass::LightWeightPass()
: VisibleInstructionPass(ID)
{
}

//--------------------------------------------------------------------------------------------------

void LightWeightPass::onStartOfPass(llvm::Module& module)
{
   mFunctions.initialize(module);
}

//--------------------------------------------------------------------------------------------------

void LightWeightPass::runOnVisibleInstruction(llvm::Module& module, llvm::Function& function,
                                              llvm::inst_iterator inst_it,
                                              const visible_instruction_t& visible_instruction)
{
   //
   // boost::apply_visitor(concurrency_passes::dump(), visible_instruction);
   //
   auto wrapper = concurrency_passes::wrap(module, mFunctions, inst_it);
   visible_instruction.apply_visitor(wrapper);
   ++m_nr_instrumented;
}

//--------------------------------------------------------------------------------------------------

bool LightWeightPass::isBlackListed(const llvm::Function& function) const
{
   return mFunctions.blacklisted(&function);
}

//--------------------------------------------------------------------------------------------------

void LightWeightPass::onEndOfPass(llvm::Module& module)
{
   if (auto* main = module.getFunction("main"))
   {
      instrumentation_utils::add_call_begin(main, mFunctions.Wrapper_register_main_thread(), {});
      for (auto inst_it = inst_begin(main); inst_it != inst_end(main); ++inst_it)
      {
         auto& instruction = *inst_it;
         if (llvm::ReturnInst* return_instruction = llvm::dyn_cast<llvm::ReturnInst>(&instruction))
         {
            llvm::IRBuilder<> builder(&instruction);
            builder.CreateCall(mFunctions.Wrapper_finish(), {}, "");
         }
      }
   }
}

//--------------------------------------------------------------------------------------------------

void LightWeightPass::runOnThreadExit(llvm::Function& function, llvm::inst_iterator inst_it)
{
   llvm::IRBuilder<> builder(&*inst_it);
   builder.CreateCall(mFunctions.Wrapper_finish(), {}, "");
}

//--------------------------------------------------------------------------------------------------

std::string LightWeightPass::outputname() const
{
   using namespace utils::io;
   return text_color("LightWeightPass", Color::MAGENTA);
}

//--------------------------------------------------------------------------------------------------

} // end concurrency_passes


static llvm::RegisterPass<concurrency_passes::LightWeightPass> X(
   "instrument-record-replay-lw", "concurrency_passes::LightWeightPass", false, false);
   
//--------------------------------------------------------------------------------------------------
