
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
   
   bool Functions::initialize(llvm::Module& M)
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
		
      register_wrapper(M, "finish()");
      register_wrapper(M, "post_task(int, program_model::Object const&)");
      register_wrapper(M, "spawn_thread(_opaque_pthread_t**, _opaque_pthread_attr_t const*, void* (*)(void*), void*)");
      register_wrapper(M, "wait_registered()");
      register_wrapper(M, "yield()");
      register_wrapper(M, "Scheduler()");
      register_wrapper(M, "~Scheduler()");
		
      register_standard(M, "pthread_create");
		
      if (mSucceeded)
      {
         mSucceeded =
            M.getTypeByName("class.scheduler::Scheduler") != nullptr &&
            M.getTypeByName("class.program_model::Object") != nullptr;
      }
		
      return mSucceeded;
   }
   
   //-------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Wrapper_finish() const
   {
      return mWrappers.find("finish()")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_post_task() const
   {
      return mWrappers.find("post_task(int, program_model::Object const&)")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_spawn_thread() const
   {
      return mWrappers.find("spawn_thread(_opaque_pthread_t**, _opaque_pthread_attr_t const*, void* (*)(void*), void*)")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_wait_registered() const
   {
      return mWrappers.find("wait_registered()")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_yield() const
   {
      return mWrappers.find("yield()")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Function_pthread_create() const
   {
      return mStandard.find("pthread_create")->second;
   }
   
   //-------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Scheduler_ctor() const
   {
      return mWrappers.find("Scheduler()")->second;
   }

   //-------------------------------------------------------------------------------------
   
   llvm::Function* Functions::Scheduler_dtor() const
   {
      return mWrappers.find("~Scheduler()")->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool Functions::blacklisted(llvm::Function* F) const
   {
      return
         mStandard.find(F->getName()) != mStandard.end() ||
        	mBlackListed.find(F->getName()) != mBlackListed.end();
   }
   
   //-------------------------------------------------------------------------------------
	
   void Functions::register_wrapper(llvm::Module& M, const std::string& name)
   {
      using namespace instrumentation_utils;
      llvm::Function* wrapper = get_function_with_unmangled_name(M, "scheduler::Scheduler::" + name);
      if (wrapper)
      {
         mWrappers.insert(FunctionMap::value_type(name, wrapper));
         mBlackListed.insert(wrapper->getName().str());
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
