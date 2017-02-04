
#include "functions.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// LLVM
#include <llvm/IR/Module.h>

// STL
#include <exception>

namespace record_replay
{
   namespace
   {
      //--------------------------------------------------------------------------------------------
   
      void check_type_exists(const llvm::Module& module, const std::string& type_name)
      {
         if (module.getTypeByName(type_name) == nullptr)
         {
            throw std::invalid_argument("Type " + type_name + " does not exists in module");
         }
      }
      
      //--------------------------------------------------------------------------------------------
      
   } // end namespace
   
   Functions::Functions() = default;

   //-----------------------------------------------------------------------------------------------
   
   void Functions::initialize(llvm::Module& module)
   {
      m_black_listed =
      {
         "fflush",
         "llvm.memcpy.p0i8.p0i8.i64",
         "malloc",
         "printf",
         "pthread_exit",
         "pthread_mutex_lock",
         "pthread_mutex_unlock",
      };
		
      register_wrapper(module, "finish()");
      register_wrapper(module, "post_task(int, program_model::Object const&)");
      register_wrapper(module, "spawn_thread(_opaque_pthread_t**, _opaque_pthread_attr_t const*, void* (*)(void*), void*)");
      register_wrapper(module, "wait_registered()");
      register_wrapper(module, "yield()");
      register_wrapper(module, "Scheduler()");
      register_wrapper(module, "~Scheduler()");
		
      register_c_function(module, "pthread_create");
      
      check_type_exists(module, "class.program_model::Object");
   }
   
   //-----------------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Wrapper_finish() const
   {
      return m_wrappers.find("finish()")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_post_task() const
   {
      return m_wrappers.find("post_task(int, program_model::Object const&)")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_spawn_thread() const
   {
      return m_wrappers.find("spawn_thread(_opaque_pthread_t**, _opaque_pthread_attr_t const*, void* (*)(void*), void*)")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_wait_registered() const
   {
      return m_wrappers.find("wait_registered()")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_yield() const
   {
      return m_wrappers.find("yield()")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Function_pthread_create() const
   {
      return m_c_functions.find("pthread_create")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
	
   llvm::Function* Functions::Scheduler_ctor() const
   {
      return m_wrappers.find("Scheduler()")->second;
   }

   //-----------------------------------------------------------------------------------------------
   
   llvm::Function* Functions::Scheduler_dtor() const
   {
      return m_wrappers.find("~Scheduler()")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   bool Functions::blacklisted(llvm::Function* function) const
   {
      return
         m_c_functions.find(function->getName()) != m_c_functions.end() ||
        	m_black_listed.find(function->getName()) != m_black_listed.end();
   }
   
   //-----------------------------------------------------------------------------------------------
	
   void Functions::register_wrapper(llvm::Module& module, const std::string& wrapper_name)
   {
      using namespace instrumentation_utils;
      llvm::Function* wrapper = get_function_with_unmangled_name(module, "scheduler::Scheduler::" + wrapper_name);
      m_wrappers.insert(function_map_t::value_type(wrapper_name, wrapper));
      m_black_listed.insert(wrapper->getName().str());
   }
   
   //-----------------------------------------------------------------------------------------------
	
   void Functions::register_c_function(const llvm::Module& module, const std::string& name)
   {
      llvm::Function* function = llvm::cast<llvm::Function>(module.getFunction(name));
      if (function)
      {
         m_c_functions.insert(function_map_t::value_type(name, function));
         return;
      }
      throw std::invalid_argument("C function " + name + " not found in module");
   }
   
   //-----------------------------------------------------------------------------------------------
	
} // end namespace record_replay
