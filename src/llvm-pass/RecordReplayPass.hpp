#pragma once

// LLVM_PASS
#include "functions.hpp"
#include "llvm_visible_instruction.hpp"
#include "VisibleInstructionPass.hpp"

// LLVM
#include <llvm/IR/InstIterator.h>

// STL
#include <set>

//--------------------------------------------------------------------------------------90
/// @file RecordReplayPass.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//----------------------------------------------------------------------------------------

namespace concurrency_passes {
   //-------------------------------------------------------------------------------------
   
   /// @brief A VisibleInstructionPass that instruments an LLVM IR module of an input
   /// program with a Scheduler object and wraps visible instructions in handles that
   /// allow the Scheduler to record and/or replay the program under a given thread
   /// interleaving.
   
   class LightWeightPass : public VisibleInstructionPass
   {
   public:
      
      //----------------------------------------------------------------------------------
        
      static char ID;
      
      //----------------------------------------------------------------------------------
      
      /// @brief Default constructor.
      
      LightWeightPass();
      
      //----------------------------------------------------------------------------------
      
      void onStartOfPass(llvm::Module& module) override;
      void runOnVisibleInstruction(llvm::Function& function,
                                   llvm::inst_iterator inst_it,
                                   const visible_instruction_t& visible_instruction) override;
      void runOnThreadExit(llvm::Function& function, llvm::inst_iterator inst_it) override;
      void onEndOfPass(llvm::Module& module) override;
      
      //----------------------------------------------------------------------------------
        
   private:
      
      //----------------------------------------------------------------------------------
      
      bool isBlackListed(const llvm::Function& function) const override;
      
      //----------------------------------------------------------------------------------
        
      using FunctionSet = std::set<llvm::Function*>;
      
      //----------------------------------------------------------------------------------
        
      FunctionSet mStartRoutines;
      Functions mFunctions;
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a new function named program_main to llvm::Module M with main's body
      /// spliced into it. Returns a pointer to the newly created function.
      
      llvm::Function* create_program_main(llvm::Module& module);
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a call to Scheduler's constructor, a call to program_main, and a
      /// call to Scheduler's destructor to main, followed by an instruction returning 0.
      /// @note Assumes that main does not take arguments that need to be forwarded to
      /// program_main.

      void restore_main(llvm::Module& module);
      
      //----------------------------------------------------------------------------------

      /// @brief Replaces all calls to pthread_create(args) in program_main with calls to
      /// Wrapper_spawn_thread(args) and adds the start routine from each call to the set
      /// mStartRoutines.

      void instrument_pthread_create_calls(llvm::Function* program_main);
      
      //----------------------------------------------------------------------------------

      /// @brief Adds a call to Wrapper_wait_registered as a first instruction in every
      /// start routine in mStartRoutines.
      
      void instrument_start_routines();
      
      //----------------------------------------------------------------------------------
      
      std::string outputname() const;
      
      //----------------------------------------------------------------------------------
      
   }; // end class LightWeightPass
   
   //-------------------------------------------------------------------------------------
} // end namespace concurrency_passes
