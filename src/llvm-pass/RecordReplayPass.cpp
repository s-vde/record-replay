
#include "RecordReplayPass.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// STL
#include <exception>

#include <cxxabi.h>

namespace concurrency_passes {
   //-------------------------------------------------------------------------------------
   
   char LightWeightPass::ID = 0;
   
   //-------------------------------------------------------------------------------------
    
   LightWeightPass::LightWeightPass()
   : VisibleInstructionPass(ID)
   {
   }
   
   //-------------------------------------------------------------------------------------
   
   void LightWeightPass::onStartOfPass(llvm::Module& module)
   {
      mFunctions.initialize(module);
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* LightWeightPass::create_program_main(llvm::Module& module)
   {
      PRINTF(outputname(), "create_program_main", "", "\n");
      llvm::Function* main = module.getFunction("main");
      if (main)
      {
         llvm::Function* program_main = instrumentation_utils::create_function(module, "program_main", main, {});
         program_main->getBasicBlockList().splice(program_main->begin(),
                                                  main->getBasicBlockList());
         program_main->dump();
         return program_main;
      }
      throw std::invalid_argument("Module does not contain function main");
   }
   
   //-------------------------------------------------------------------------------------

   void LightWeightPass::restore_main(llvm::Module& module)
   {
      using namespace llvm;
      
      PRINTF(outputname(), "restore_main", "", "\n");
      Function* main = module.getFunction("main");
      if (main)
      {
         BasicBlock* BB = BasicBlock::Create(module.getContext(), "", main, nullptr);
         CallInst::Create(module.getFunction("program_main"), { }, "", BB);
         ConstantInt* return_value = ConstantInt::get(IntegerType::getInt32Ty(module.getContext()), 0, true);
         ReturnInst::Create(module.getContext(), return_value, BB);
         main->dump();
         return;
      }
      throw std::invalid_argument("Module does not contain function main");
   }
   
   //-------------------------------------------------------------------------------------

   void LightWeightPass::instrument_pthread_create_calls(llvm::Function* program_main)
   {
      PRINTF(outputname(), "instrument_pthread_functions", "", "\n");
      auto PthreadCreateCalls = instrumentation_utils::call_instructions(program_main, "pthread_create");
      for (const auto call : PthreadCreateCalls)
      {
         PRINT("\tptread_create(" << call->getArgOperand(2)->getName().str() << ") called by main\n");
         instrumentation_utils::replace_call(
            call,
            mFunctions.Function_pthread_create(),
            mFunctions.Wrapper_spawn_thread(),
            {}
         );
         mStartRoutines.insert(llvm::cast<llvm::Function>(call->getArgOperand(2)));
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_start_routines()
   {
      PRINTF(outputname(), "instrument_start_routines", "", "\n");
      for (auto start_routine : mStartRoutines)
      {
         instrumentation_utils::add_call_begin(
            start_routine,
            mFunctions.Wrapper_wait_registered(),
            {}
         );
      }
   }
   
   //-------------------------------------------------------------------------------------
   
   void LightWeightPass::runOnVisibleInstruction(llvm::Function& function,
                                                 llvm::inst_iterator inst_it,
                                                 const visible_instruction_t& visible_instruction)
   {
      //
      boost::apply_visitor(concurrency_passes::dump(), visible_instruction);
      //
      const int operation = boost::apply_visitor(program_model::operation_as_int<operand_t>(),
                                                 visible_instruction);
      
      llvm::Value* operand = boost::apply_visitor(construct_operand(&*inst_it), visible_instruction);
      
      llvm::CallInst::Create(mFunctions.Wrapper_post_task(),
                             { llvm::ConstantInt::get(function.getContext(),
                                                       llvm::APInt(32, operation, false)),
                                operand },
                             "",
                             &*inst_it);
      llvm::CallInst::Create(mFunctions.Wrapper_yield(), {}, "", &*(++inst_it));
      ++m_nr_instrumented;
   }
   
   //-------------------------------------------------------------------------------------
   
   bool LightWeightPass::isBlackListed(const llvm::Function& function) const
   {
      return mFunctions.blacklisted(&function);
   }
   
   //-------------------------------------------------------------------------------------
   
   void LightWeightPass::onEndOfPass(llvm::Module& module)
   {
      try
      {
         auto program_main = create_program_main(module);
         restore_main(module);
         instrument_pthread_create_calls(program_main);
         instrument_start_routines();
      }
      catch(const std::invalid_argument& e)
      {
      }
   }
   
   //-------------------------------------------------------------------------------------
   
   void LightWeightPass::runOnThreadExit(llvm::Function& function, llvm::inst_iterator inst_it)
   {
      PRINTF(outputname(), "runOnThreadExit", "", "\n");
      llvm::IRBuilder<> builder(&*inst_it);
      builder.CreateCall(mFunctions.Wrapper_finish(), {}, "");
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string LightWeightPass::outputname() const
   {
      using namespace utils::io;
      return text_color("LightWeightPass", Color::MAGENTA);
   }
   
   //-------------------------------------------------------------------------------------
   
} // end concurrency_passes

static llvm::RegisterPass<concurrency_passes::LightWeightPass> X("instrument-record-replay-lw", "concurrency_passes::LightWeightPass", false, false);
