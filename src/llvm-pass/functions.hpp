#pragma once

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
   using function_map_t = std::unordered_map<std::string, llvm::Function*>;

   Functions();

   void initialize(llvm::Module& module);

   llvm::Function* Wrapper_finish() const;
   llvm::Function* Wrapper_post_lock_instruction() const;
   llvm::Function* Wrapper_post_memory_instruction() const;
   llvm::Function* Wrapper_spawn_thread() const;

   llvm::Function* Function_pthread_create() const;

   bool blacklisted(const llvm::Function* F) const;

private:
   void add_wrapper_prototype(llvm::Module& module, const std::string& name,
                              llvm::FunctionType* type, llvm::AttributeSet& attributes);
   void register_c_function(const llvm::Module& module, const std::string& name);

   function_map_t m_wrappers;
   function_map_t m_c_functions;

   std::set<std::string> m_black_listed;

}; // end class Functions

} // end namespace concurrency_passes
