#pragma once

// STL
#include <set>
#include <string>
#include <unordered_map>

//--------------------------------------------------------------------------------------90
/// @file functions.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace llvm
{
   class Function;
   class Module;
}

//----------------------------------------------------------------------------------------

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   class Functions
   {
   public:
      
      //----------------------------------------------------------------------------------
		
      using FunctionMap = std::unordered_map<std::string,llvm::Function*>;
        
      //----------------------------------------------------------------------------------
      
      Functions();
        
      //----------------------------------------------------------------------------------
        
      bool initialize(llvm::Module& M);
      
      //----------------------------------------------------------------------------------
        
      llvm::Function* Wrapper_finish() const;
      llvm::Function* Wrapper_post_task() const;
      llvm::Function* Wrapper_spawn_thread() const;
      llvm::Function* Wrapper_wait_registered() const;
      llvm::Function* Wrapper_yield() const;
      
      //----------------------------------------------------------------------------------
      
      llvm::Function* Function_pthread_create() const;
      llvm::Function* Scheduler_ctor() const;
      llvm::Function* Scheduler_dtor() const;
      
      //----------------------------------------------------------------------------------
		
      bool blacklisted(llvm::Function* F) const;
      
      //----------------------------------------------------------------------------------
        
   private:

      //----------------------------------------------------------------------------------
      
      FunctionMap mWrappers;
      FunctionMap mStandard;
      std::set<std::string> mBlackListed;
      bool mSucceeded;
      
      //----------------------------------------------------------------------------------

      void register_wrapper(llvm::Module& M, const std::string& name);
		
		void register_standard(const llvm::Module& M, const std::string& name);
      
      //----------------------------------------------------------------------------------
		
   }; // end class Functions
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
