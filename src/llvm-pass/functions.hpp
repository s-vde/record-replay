#pragma once

#include <llvm/IR/Attributes.h>

#include <set>
#include <string>
#include <unordered_map>


//--------------------------------------------------------------------------------------------------
/// @file functions.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace llvm {
class AttributeSet;
class Function;
class FunctionType;
class Module;
class Type;
} // end namespace llvm


namespace concurrency_passes {

class Functions
{
public:
   using type_map_t = std::unordered_map<std::string, llvm::Type*>;
   using function_map_t = std::unordered_map<std::string, llvm::Function*>;

   Functions();

   void initialize(llvm::Module& module);

   llvm::Function* Wrapper_post_lock_instruction() const;
   llvm::Function* Wrapper_post_memory_instruction() const;
   llvm::Function* Wrapper_post_spawn_instruction() const;
   llvm::Function* Wrapper_post_pthread_join_instruction() const;
   llvm::Function* Wrapper_post_stdthread_join_instruction() const;
   llvm::Function* Wrapper_register_main_thread() const;
   llvm::Function* Wrapper_register_thread() const;
   llvm::Function* Wrapper_enter_function() const;
   llvm::Function* Wrapper_exit_function() const;

   llvm::Function* Function_pthread_create() const;

   llvm::Type* Type_pthread_t() const;
   llvm::Type* Type_stdthread() const;

   bool blacklisted(const llvm::Function* F) const;

private:
   void add_wrapper_prototype(llvm::Module& module, const std::string& name,
                              llvm::FunctionType* type, llvm::AttributeList& attributes);
   void register_c_function(const llvm::Module& module, const std::string& name);

   type_map_t m_types;
   function_map_t m_wrappers;
   function_map_t m_c_functions;

   std::set<std::string> m_black_listed;

}; // end class Functions

} // end namespace concurrency_passes
