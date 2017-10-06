
#include "functions.hpp"

#include "instrumentation_utils.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <exception>


namespace concurrency_passes {

//-----------------------------------------------------------------------------------------------

Functions::Functions() = default;

//-----------------------------------------------------------------------------------------------

void Functions::initialize(llvm::Module& module)
{
   m_black_listed = {
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
   attributes = attributes.addAttribute(module.getContext(), AttributeSet::FunctionIndex,
                                        Attribute::NoUnwind);

   IRBuilder<> builder(module.getContext());
   Type* void_type = Type::getVoidTy(module.getContext());
   Type* void_ptr_type = builder.getInt8PtrTy();
   Type* type_char_ptr = builder.getInt8PtrTy();

   // pthread_t
   Type* type_opaque_pthread = module.getTypeByName("struct._opaque_pthread_t");
   if (!type_opaque_pthread)
   {
      throw std::invalid_argument("Type opaque_pthread_t not found in module");
   }

   m_types.insert({"pthread_t", type_opaque_pthread->getPointerTo()});
   Type* type_pthread_id = m_types["pthread_t"]->getPointerTo();

   // std::thread
   Type* type_stdthread = module.getTypeByName("class.std::__1::thread");
   if (type_stdthread)
   {
      m_types.insert({"std::thread", type_stdthread});
   }

   // Wrapper_register_main_thread
   {
      auto* type = FunctionType::get(void_type, {}, false);
      add_wrapper_prototype(module, "wrapper_register_main_thread", type, attributes);
   }

   // Wrapper_register_thread
   {
      auto* type = FunctionType::get(void_type, {type_pthread_id, builder.getInt32Ty()}, false);
      add_wrapper_prototype(module, "wrapper_register_thread", type, attributes);
   }

   // wrapper_post_spawn_instruction
   {
      auto* type = FunctionType::get(builder.getInt32Ty(),
                                     {type_pthread_id, type_char_ptr, builder.getInt32Ty()}, false);
      add_wrapper_prototype(module, "wrapper_post_spawn_instruction", type, attributes);
   }

   // wrapper_post_pthread_join_instruction
   {
      auto* type = FunctionType::get(
         void_type, {m_types["pthread_t"], type_char_ptr, builder.getInt32Ty()}, false);
      add_wrapper_prototype(module, "wrapper_post_pthread_join_instruction", type, attributes);
   }

   // wrapper_post_stdthread_join_instruction
   if (type_stdthread)
   {
      auto* type = FunctionType::get(
         void_type, {type_stdthread->getPointerTo(), type_char_ptr, builder.getInt32Ty()}, false);
      add_wrapper_prototype(module, "wrapper_post_stdthread_join_instruction", type, attributes);
   }

   // wrapper_post_lock_instruction
   {
      auto* type = FunctionType::get(
         void_type, {builder.getInt32Ty(), void_ptr_type, type_char_ptr, builder.getInt32Ty()},
         false);
      add_wrapper_prototype(module, "wrapper_post_lock_instruction", type, attributes);
   }

   // wrapper_post_memory_instruction
   {
      auto* type = FunctionType::get(void_type,
                                     {builder.getInt32Ty(), void_ptr_type, builder.getInt8Ty(),
                                      type_char_ptr, builder.getInt32Ty()},
                                     false);
      add_wrapper_prototype(module, "wrapper_post_memory_instruction", type, attributes);
   }

   // wrapper_enter_function
   {
      auto* type = FunctionType::get(void_type, {type_char_ptr}, false);
      add_wrapper_prototype(module, "wrapper_enter_function", type, attributes);
   }

   // wrapper_exit_function
   {
      auto* type = FunctionType::get(void_type, {type_char_ptr}, false);
      add_wrapper_prototype(module, "wrapper_exit_function", type, attributes);
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

llvm::Function* Functions::Wrapper_post_lock_instruction() const
{
   return m_wrappers.find("wrapper_post_lock_instruction")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_post_memory_instruction() const
{
   return m_wrappers.find("wrapper_post_memory_instruction")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_post_spawn_instruction() const
{
   return m_wrappers.find("wrapper_post_spawn_instruction")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_post_pthread_join_instruction() const
{
   return m_wrappers.find("wrapper_post_pthread_join_instruction")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_post_stdthread_join_instruction() const
{
   return m_wrappers.find("wrapper_post_stdthread_join_instruction")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_register_main_thread() const
{
   return m_wrappers.find("wrapper_register_main_thread")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_register_thread() const
{
   return m_wrappers.find("wrapper_register_thread")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_enter_function() const
{
   return m_wrappers.find("wrapper_enter_function")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Wrapper_exit_function() const
{
   return m_wrappers.find("wrapper_exit_function")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Function* Functions::Function_pthread_create() const
{
   return m_c_functions.find("pthread_create")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Type* Functions::Type_pthread_t() const
{
   return m_types.find("pthread_t")->second;
}

//-----------------------------------------------------------------------------------------------

llvm::Type* Functions::Type_stdthread() const
{
   const auto it = m_types.find("std::thread");
   if (it == m_types.end())
      throw std::runtime_error("Type std::thread not found in this module");
   return it->second;
}

//-----------------------------------------------------------------------------------------------

bool Functions::blacklisted(const llvm::Function* function) const
{
   return m_c_functions.find(function->getName()) != m_c_functions.end() ||
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

} // end namespace concurrency_passes
