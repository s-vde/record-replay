#pragma once

// LLVM_PASS
#include "functions.hpp"
#include "visible_instruction.hpp"

// LLVM
#include <llvm/IR/BasicBlock.h>
#include <llvm/Pass.h>

// STL
#include <set>
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file pass.hpp
/// @brief Definition of llvm::ModulePass LightWeightPass.
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   /// @brief An LLVM ModulePass that instruments an LLVM IR module of an input program
   /// with a Scheduler object and handles that allow the Scheduler to record and/or
   /// replay the program under a given thread interleaving.
   
   class LightWeightPass : public llvm::ModulePass
   {
   public:
      
      //----------------------------------------------------------------------------------
        
      static char ID;
      
      //----------------------------------------------------------------------------------
      
      /// @brief Default constructor.
      
      LightWeightPass();
      
      //----------------------------------------------------------------------------------
      
      virtual bool runOnModule(llvm::Module& M);
      
      //----------------------------------------------------------------------------------
        
   private:
        
      //----------------------------------------------------------------------------------
        
      using FunctionSet = std::set<llvm::Function*>;
      
      //----------------------------------------------------------------------------------
        
      FunctionSet mStartRoutines;
      Functions mFunctions;
      llvm::GlobalVariable* mScheduler;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Creates a new llvm::GlobalVariable of type Scheduler to llvm::Module M.
      
      void add_scheduler(llvm::Module& M);
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a new function named program_main to llvm::Module M with main's body
      /// spliced into it. Returns a pointer to the newly created function.
      
      llvm::Function* create_program_main(llvm::Module& M);
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a call to Scheduler's constructor, a call to program_main, and a
      /// call to Scheduler's destructor to main, followed by an instruction returning 0.
      /// @note Assumes that main does not take arguments that need to be forwarded to
      /// program_main.

      void restore_main(llvm::Module&);
      
      //----------------------------------------------------------------------------------

      /// @brief Replaces all calls to pthread_create(args) in program_main with calls to
      /// Wrapper_spawn_thread(args) and adds the start routine from each call to the set
      /// mStartRoutines.

      void instrument_pthread_create_calls(llvm::Module&, llvm::Function* program_main);
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a call to Wrapper_wait_registered as a first instruction in every
      /// start routine in mStartRoutines.
      
      void instrument_start_routines(llvm::Module&);
      
      //----------------------------------------------------------------------------------
        
      void instrument_functions(llvm::Module&);
      
      //----------------------------------------------------------------------------------

      /// @brief Iterates through the instructions of F, calls wrap_visible_instruction on
      /// instructions that are visible and calls add_thread_finished on instructions that
      /// mark a thread's end.

      void instrument_function(llvm::Module&,
                               llvm::Function* F,
                               FunctionSet& ToInstrument,
                               const FunctionSet& Done);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Checks whether llvm::Instruction I is an llvm::CallInst whose callee has
      /// to be instrumented. If so, it adds the callee to ToInstrument.

      void check_to_be_instrumented(llvm::Instruction* I,
                                    const std::string& fname,
                                    FunctionSet& ToInstrument,
                                    const FunctionSet& Done) const;
      
      //----------------------------------------------------------------------------------

      /// @brief Wraps a visible instruction between a call to  Wrapper_post_task and
      /// Wrapper_yield.

      void wrap_visible_instruction(llvm::Module&,
                                    llvm::BasicBlock::iterator,
                                    const visible_instruction_t&);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Adds a call to Wrapper_finish after I.

      void add_thread_finished(llvm::Module&, llvm::BasicBlock::iterator I);
      
      //----------------------------------------------------------------------------------
        
      bool isa_thread_end(llvm::Function*, llvm::Instruction*) const;
      
      //----------------------------------------------------------------------------------
      
      std::string outputname() const;
      
      //----------------------------------------------------------------------------------
        
   }; // end class LightWeightPass
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
