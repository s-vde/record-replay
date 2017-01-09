
#include "pass.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Module.h>

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   char LightWeightPass::ID = 0;
   
   //-------------------------------------------------------------------------------------
    
   LightWeightPass::LightWeightPass()
   : llvm::ModulePass(ID)
   , mStartRoutines()
   , mFunctions()
   , mScheduler(nullptr) { }
   
   //-------------------------------------------------------------------------------------
    
   bool LightWeightPass::runOnModule(llvm::Module& M)
   {
      add_scheduler(M);
      if (mFunctions.initialize(M))
      {
         auto program_main = create_program_main(M);
//         mStartRoutines.insert(program_main);
        	restore_main(M);
        	instrument_pthread_create_calls(M, program_main);
        	instrument_start_routines(M);
        	instrument_functions(M);
		}
      else
      {
         PRINT("Scheduler Wrapper functions not found\n");
      }
      PRINT("nr_instrumented_instructions:\t" << m_nr_instrumented_instructions << "\n");
      return false;
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::add_scheduler(llvm::Module& M)
   {
      llvm::Type* scheduler_type = M.getTypeByName("class.scheduler::Scheduler");
      mScheduler = new llvm::GlobalVariable(
         M,                                                  // Module
         scheduler_type,                                     // Type
         false,                                              // isConstant
         llvm::GlobalValue::CommonLinkage,                   // Linkage
         llvm::ConstantAggregateZero::get(scheduler_type),   // Initializer (zeroinitializer)
         "the_scheduler"                                     // Name
      );
      mScheduler->setAlignment(8);
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* LightWeightPass::create_program_main(llvm::Module& M)
   {
      PRINTF(outputname(), "create_program_main", "", "\n");
      auto main = M.getFunction("main");
      auto program_main = instrumentation_utils::create_function(M, "program_main", main, {});
      program_main->getBasicBlockList().splice(
         program_main->begin(),
         main->getBasicBlockList()
      );
      return program_main;
   }
   
   //-------------------------------------------------------------------------------------

   void LightWeightPass::restore_main(llvm::Module& M)
   {
      PRINTF(outputname(), "create_main", "", "\n");
      llvm::Function* main = M.getFunction("main");
      // llvm::BasicBlock
      llvm::BasicBlock* BB = llvm::BasicBlock::Create(M.getContext(), "", main, nullptr);
      llvm::CallInst::Create(mFunctions.Scheduler_ctor(), { mScheduler }, "", BB);
      llvm::CallInst::Create(M.getFunction("program_main"), { }, "", BB);
      llvm::CallInst::Create(mFunctions.Scheduler_dtor(), { mScheduler }, "", BB);
      llvm::ReturnInst::Create(
         M.getContext(),
         llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(M.getContext()), 0, true),
         BB
      );
      main->dump();
   }
   
   //-------------------------------------------------------------------------------------

   void LightWeightPass::instrument_pthread_create_calls(llvm::Module& M, llvm::Function* program_main)
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
            { mScheduler }
         );
         mStartRoutines.insert(llvm::cast<llvm::Function>(call->getArgOperand(2)));
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_start_routines(llvm::Module& M)
   {
      PRINTF(outputname(), "instrument_start_routines", "", "\n");
      for (auto start_routine : mStartRoutines)
      {
         instrumentation_utils::add_call_begin(
            start_routine,
            mFunctions.Wrapper_wait_registered(),
            { mScheduler }
         );
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_functions(llvm::Module& M)
   {
      FunctionSet Done{};
      FunctionSet ToInstrument{};
      std::copy(mStartRoutines.begin(), mStartRoutines.end(), std::inserter(ToInstrument, ToInstrument.end()));
      while (!ToInstrument.empty())
      {
         llvm::Function* F = *(ToInstrument.begin());
         ToInstrument.erase(ToInstrument.begin());
         instrument_function(M, F, ToInstrument, Done);
         Done.insert(F);
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_function(llvm::Module& M,
                                             llvm::Function* F,
                                             FunctionSet& ToInstrument,
                                             const FunctionSet& Done)
   {
      PRINTF("\n----------\n" << outputname(), "instrument_function", F->getName(), "\n");
      llvm_visible_instruction::creator creator;
      for (auto instr = llvm::inst_begin(F); instr != llvm::inst_end(F); ++instr)
      {
         //
         instr->dump();
         //
         const auto visible_instr = creator.visit(&*instr);
         if (visible_instr)
         {
            using namespace utils::io;
            PRINT("-----\n");
            boost::apply_visitor(::record_replay::dump(), *visible_instr);
            PRINT("-----\n");
            wrap_visible_instruction(M, &*instr, *visible_instr);
            ++m_nr_instrumented_instructions;
         }
         else if (isa_thread_end(F, &*instr))
         {
            add_thread_finished(M, *instr);
         }
         else
         {
            check_to_be_instrumented(&*instr, F->getName(), ToInstrument, Done);
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::check_to_be_instrumented(llvm::Instruction* I,
                                                  const std::string& fname,
                                                  FunctionSet& ToInstrument,
                                                  const FunctionSet& Done) const
   {
      if (llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(I))
      {
         llvm::Function* callee = CI->getCalledFunction();
         if (callee->getName() != fname &&
             Done.find(callee) == Done.end() &&
             !mFunctions.blacklisted(callee))
         {
            PRINT("Adding " << callee->getName() << " to ToInstrument.\n");
            ToInstrument.insert(callee);
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::wrap_visible_instruction(llvm::Module& M,
                                                  llvm::BasicBlock::iterator I,
                                                  const visible_instruction_t& instr)
   {
      llvm::CallInst::Create(
         mFunctions.Wrapper_post_task(),
         {
            mScheduler,
            llvm::ConstantInt::get(M.getContext(),
                                   llvm::APInt(32,
                                               boost::apply_visitor(program_model::operation_as_int<operand_t>(), instr),
                                               false)),
            boost::apply_visitor(construct_operand(M, I), instr)
         },
         "",
         I
      );
      llvm::CallInst::Create(mFunctions.Wrapper_yield(), { mScheduler }, "", ++I);
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::add_thread_finished(llvm::Module& M, llvm::BasicBlock::iterator I)
   {
      PRINTF(outputname(), "add_thread_finished", "", "\n");
      llvm::CallInst::Create(mFunctions.Wrapper_finish(), { mScheduler }, "", I);
   }
   
   //-------------------------------------------------------------------------------------
    
   bool LightWeightPass::isa_thread_end(llvm::Function* F, llvm::Instruction* I) const
   {
      if (llvm::isa<llvm::ReturnInst>(I) && mStartRoutines.find(F) != mStartRoutines.end())
      {
         return true;
      }
      else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(I))
      {
         return call->getCalledFunction()->getName() == "pthread_exit";
      }
      else
      {
         return false;
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string LightWeightPass::outputname() const
   {
      using namespace utils::io;
      return text_color("LightWeightPass", Color::MAGENTA);
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay

static llvm::RegisterPass<record_replay::LightWeightPass> X("instrument-record-replay-lw", "record_replay::LightWeightPass", false, false);
