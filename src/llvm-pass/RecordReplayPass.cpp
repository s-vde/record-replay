
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

void LightWeightPass::instrument_pthread_create_calls()
{
   using namespace llvm;
   Function* pthread_create = mFunctions.Function_pthread_create();
   for (auto* user : pthread_create->users())
   {
      if (CallInst* call = dyn_cast<CallInst>(user))
      {
         instrumentation_utils::replace_call(call, mFunctions.Function_pthread_create(),
                                             mFunctions.Wrapper_spawn_thread(), {});
      }
      else if (InvokeInst* invoke = dyn_cast<InvokeInst>(user))
      {
         instrumentation_utils::replace_invoke(invoke, mFunctions.Function_pthread_create(),
                                               mFunctions.Wrapper_spawn_thread(), {});
      }
   }
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
   instrument_pthread_create_calls();
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
