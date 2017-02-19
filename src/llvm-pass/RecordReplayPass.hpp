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
      void runOnVisibleInstruction(llvm::Module& module, 
                                   llvm::Function& function,
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

      /// @brief Replaces all calls to pthread_create(args) in main with calls to
      /// Wrapper_spawn_thread(args) and adds the start routine from each call to the set
      /// mStartRoutines.

      void instrument_pthread_create_calls(llvm::Function* main);
      
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
