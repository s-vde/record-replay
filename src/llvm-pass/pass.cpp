
#include "pass.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/Module.h>

// STL
#include <exception>

#include <cxxabi.h>

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
      try
      {
         add_scheduler(M);
         mFunctions.initialize(M);
         auto program_main = create_program_main(M);
         //         mStartRoutines.insert(program_main);
        	restore_main(M);
        	instrument_pthread_create_calls(program_main);
        	instrument_start_routines();
        	instrument_functions(M);
      }
      catch(const std::invalid_argument& e)
      {
         llvm::errs() << e.what() << "\n";
      }
      
      PRINT("nr_instrumented_instructions:\t" << m_nr_instrumented_instructions << "\n");
      return false;
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::add_scheduler(llvm::Module& module)
   {
      llvm::Type* scheduler_type = module.getTypeByName("class.scheduler::Scheduler");
      if (scheduler_type)
      {
         mScheduler = new llvm::GlobalVariable(module,
                                               scheduler_type,
                                               // isConstant
                                               false,
                                               llvm::GlobalValue::CommonLinkage,
                                               // Initializer (zeroinitializer)
                                               llvm::ConstantAggregateZero::get(scheduler_type),
                                               "the_scheduler");
         mScheduler->setAlignment(8);
         return;
      }
      throw std::invalid_argument("Type class.scheduler::Scheduler not found in module");
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* LightWeightPass::create_program_main(llvm::Module& module)
   {
      llvm::Function* main = module.getFunction("main");
      if (main)
      {
         auto program_main = instrumentation_utils::create_function(module, "program_main", main, {});
         program_main->getBasicBlockList().splice(program_main->begin(),
                                                  main->getBasicBlockList());
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
         CallInst::Create(mFunctions.Scheduler_ctor(), { mScheduler }, "", BB);
         CallInst::Create(module.getFunction("program_main"), { }, "", BB);
         CallInst::Create(mFunctions.Scheduler_dtor(), { mScheduler }, "", BB);
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
            { mScheduler }
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
            { mScheduler }
         );
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_functions(llvm::Module& module)
   {
      FunctionSet Done{};
      FunctionSet ToInstrument{};
      std::copy(mStartRoutines.begin(), mStartRoutines.end(), std::inserter(ToInstrument, ToInstrument.end()));
      while (!ToInstrument.empty())
      {
         llvm::Function* F = *(ToInstrument.begin());
         ToInstrument.erase(ToInstrument.begin());
         instrument_function(module, F, ToInstrument, Done);
         Done.insert(F);
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::instrument_function(llvm::Module& module,
                                             llvm::Function* function,
                                             FunctionSet& ToInstrument,
                                             const FunctionSet& Done)
   {
      PRINTF("\n----------\n" << outputname(), "instrument_function", function->getName(), "\n");
      llvm_visible_instruction::creator creator;
      for (auto instr = llvm::inst_begin(function); instr != llvm::inst_end(function); ++instr)
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
            wrap_visible_instruction(module, instr, *visible_instr);
            ++m_nr_instrumented_instructions;
         }
         else if (isa_thread_end(function, &*instr))
         {
            add_thread_finished(&*instr);
         }
         else
         {
            check_to_be_instrumented(&*instr, function->getName(), ToInstrument, Done);
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::check_to_be_instrumented(llvm::Instruction* instruction,
                                                  const std::string& fname,
                                                  FunctionSet& ToInstrument,
                                                  const FunctionSet& Done) const
   {
      if (llvm::CallInst* call_instruction = llvm::dyn_cast<llvm::CallInst>(instruction))
      {
         llvm::Function* callee = call_instruction->getCalledFunction();
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
    
   void LightWeightPass::wrap_visible_instruction(llvm::Module& module,
                                                  llvm::inst_iterator inst_it,
                                                  const visible_instruction_t& instruction)
   {
      using namespace boost;
      using namespace llvm;
      using namespace program_model;
      
      const int operation_int = apply_visitor(operation_as_int<operand_t>(), instruction);
      ConstantInt* operation = ConstantInt::get(module.getContext(), APInt(32, operation_int, false));
      llvm::Value* operand = apply_visitor(construct_operand(module, &*inst_it), instruction);
      CallInst::Create(mFunctions.Wrapper_post_task(),
                       { mScheduler, operation, operand },
                       "",
                       &*inst_it);
      CallInst::Create(mFunctions.Wrapper_yield(), { mScheduler }, "", &*(++inst_it));
   }
   
   //-------------------------------------------------------------------------------------
    
   void LightWeightPass::add_thread_finished(llvm::Instruction* instruction)
   {
      PRINTF(outputname(), "add_thread_finished", "", "\n");
      llvm::CallInst::Create(mFunctions.Wrapper_finish(), { mScheduler }, "", instruction);
   }
   
   //-------------------------------------------------------------------------------------
    
   bool LightWeightPass::isa_thread_end(llvm::Function* function, llvm::Instruction* instruction) const
   {
      if (llvm::isa<llvm::ReturnInst>(instruction)
          && mStartRoutines.find(function) != mStartRoutines.end())
      {
         return true;
      }
      else if (llvm::CallInst* call_instruction = llvm::dyn_cast<llvm::CallInst>(instruction))
      {
         return call_instruction->getCalledFunction()->getName() == "pthread_exit";
      }
      return false;
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
