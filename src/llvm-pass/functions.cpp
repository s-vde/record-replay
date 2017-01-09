
#include "functions.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// LLVM
#include <llvm/IR/Module.h>

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   Functions::Functions()
   : mWrappers()
   , mStandard()
   , mBlackListed()
 	, mSucceeded(true) { }
   
   //-------------------------------------------------------------------------------------
   
   bool Functions::initialize(const llvm::Module& M)
   {
      mBlackListed = {
         "fflush",
         "llvm.memcpy.p0i8.p0i8.i64",
         "malloc",
         "printf",
         "pthread_exit",
         "pthread_mutex_lock",
         "pthread_mutex_unlock",
      };
		
      register_wrapper(M, "finish");
      register_wrapper(M, "post_task");
      register_wrapper(M, "spawn_thread");
      register_wrapper(M, "wait_registered");
      register_wrapper(M, "yield");
      register_wrapper(M, "C1");
      register_wrapper(M, "D1");
		
      register_standard(M, "pthread_create");
		
      if (mSucceeded)
      {
         mSucceeded =
            instrumentation_utils::get_mangled_name(M, "program_model", "Object", "C") &&
            M.getTypeByName("class.scheduler::Scheduler") != nullptr &&
            M.getTypeByName("class.program_model::Object") != nullptr;
      }
		
      return mSucceeded;
   }
   
   //-------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Wrapper_finish() const
   {
      return mWrappers.find("finish")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_post_task() const
   {
      return mWrappers.find("post_task")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_spawn_thread() const
   {
      return mWrappers.find("spawn_thread")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_wait_registered() const
   {
      return mWrappers.find("wait_registered")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_yield() const
   {
      return mWrappers.find("yield")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Function_pthread_create() const
   {
      return mStandard.find("pthread_create")->second;
   }
   
   //-------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Scheduler_ctor() const
   {
      return mWrappers.find("C1")->second;
   }

   //-------------------------------------------------------------------------------------
   
   llvm::Function* Functions::Scheduler_dtor() const
   {
      return mWrappers.find("D1")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool Functions::blacklisted(llvm::Function* F) const
   {
      return
         mStandard.find(F->getName()) != mStandard.end() ||
        	mBlackListed.find(F->getName()) != mBlackListed.end();
   }
   
   //-------------------------------------------------------------------------------------
	
   void Functions::register_wrapper(const llvm::Module& M, const std::string& name)
   {
      using instrumentation_utils::get_mangled_name;
      auto mangled = get_mangled_name(M, "scheduler", "Scheduler", name);
      if (mangled)
      {
         mWrappers.insert(FunctionMap::value_type(
            name, llvm::cast<llvm::Function>(M.getFunction(*mangled))
         ));
         mBlackListed.insert(*mangled);
		}
      else
      {
         mSucceeded = false;
      }
   }
   
   //-------------------------------------------------------------------------------------
	
   void Functions::register_standard(const llvm::Module& M, const std::string& name)
   {
      mStandard.insert(FunctionMap::value_type(
         name,
         llvm::cast<llvm::Function>(M.getFunction(name))
      ));
   }
   
   //-------------------------------------------------------------------------------------
	
} // end namespace record_replay
