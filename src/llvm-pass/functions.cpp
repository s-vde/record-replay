
#include "functions.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"

// LLVM
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

// STL
#include <exception>

namespace record_replay
{
   //-----------------------------------------------------------------------------------------------

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
		
      using namespace llvm;
      AttributeSet attributes;
      attributes = attributes.addAttribute(module.getContext(),
                                           AttributeSet::FunctionIndex,
                                           Attribute::NoUnwind);
      
      IRBuilder<> builder(module.getContext());
      Type* void_type = Type::getVoidTy(module.getContext());
      Type* void_ptr_type = builder.getInt8PtrTy();
      
      // wrapper_spawn_thread
      {
         Type* type_pthread = module.getTypeByName("struct._opaque_pthread_t");
         if (!type_pthread) { throw std::invalid_argument("Type pthread_t not found in module"); }
         Type* type_pthread_id = type_pthread->getPointerTo()->getPointerTo();
         Type* type_pthread_attr = module.getTypeByName("struct._opaque_pthread_attr_t");
         if (!type_pthread_attr) { throw std::invalid_argument("Type pthread_attr_t not found in module"); }
         FunctionType* type_start_routine = FunctionType::get(void_ptr_type, { void_ptr_type }, false);
         
         auto* function_type = FunctionType::get(builder.getInt32Ty(),
                                                 {  type_pthread_id,
                                                    type_pthread_attr->getPointerTo(),
                                                    type_start_routine->getPointerTo(),
                                                    void_ptr_type },
                                                 false);
         add_wrapper_prototype(module, "wrapper_spawn_thread", function_type, attributes);
      }
      
      // wrapper_wait_registered
      {
         FunctionType* function_type = FunctionType::get(void_type, false);
         add_wrapper_prototype(module, "wrapper_wait_registered", function_type, attributes);
      }
      
      // wrapper_post_task_type
      {
         FunctionType* function_type = FunctionType::get(void_type,
                                                         { IntegerType::get(module.getContext(), 32),
                                                           void_ptr_type },
                                                         false);
         add_wrapper_prototype(module, "wrapper_post_task", function_type, attributes);
      }
      
      // wrapper_yield
      {
         FunctionType* function_type = FunctionType::get(void_type, false);
         add_wrapper_prototype(module, "wrapper_yield", function_type, attributes);
      }
      
      // wrapper_finish
      {
         FunctionType* function_type = FunctionType::get(void_type, false);
         add_wrapper_prototype(module, "wrapper_finish", function_type, attributes);
      }
		
      register_c_function(module, "pthread_create");
   }
   
   //-----------------------------------------------------------------------------------------------
   
   void Functions::add_wrapper_prototype(llvm::Module& module, const std::string& name,
                                         llvm::FunctionType* type, llvm::AttributeSet& attributes)
   {
      using namespace llvm;
      Function* function = cast<Function>(module.getOrInsertFunction(name, type, attributes));
      m_wrappers.insert(function_map_t::value_type(name, function));
      m_black_listed.insert(function->getName().str());
   }
	
   //-----------------------------------------------------------------------------------------------
   
   llvm::Function* Functions::Wrapper_finish() const
   {
      return m_wrappers.find("wrapper_finish")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_post_task() const
   {
      return m_wrappers.find("wrapper_post_task")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_spawn_thread() const
   {
      return m_wrappers.find("wrapper_spawn_thread")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_wait_registered() const
   {
      return m_wrappers.find("wrapper_wait_registered")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Wrapper_yield() const
   {
      return m_wrappers.find("wrapper_yield")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   llvm::Function* Functions::Function_pthread_create() const
   {
      return m_c_functions.find("pthread_create")->second;
   }
   
   //-----------------------------------------------------------------------------------------------
    
   bool Functions::blacklisted(llvm::Function* function) const
   {
      return
         m_c_functions.find(function->getName()) != m_c_functions.end() ||
        	m_black_listed.find(function->getName()) != m_black_listed.end();
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
