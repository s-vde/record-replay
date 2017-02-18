#pragma once

// LLVM
#include <llvm/ADT/ArrayRef.h>

// BOOST
#include <boost/optional.hpp>

// STL
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file instrumentation_utils.hpp
/// @brief Set of utility functions for instrumentation of LLVM IR modules.
/// @author Susanne van den Elsen
/// @date 2015-2017
//----------------------------------------------------------------------------------------

namespace llvm
{
   class CallInst;
   class Constant;
   class Function;
   class FunctionType;
   class Instruction;
   class Module;
   class Value;
   class Type;
}

//----------------------------------------------------------------------------------------

namespace instrumentation_utils
{
   //-------------------------------------------------------------------------------------
    
   using param         = std::pair<llvm::Type*,const std::string>;
   using ParamVec      = std::vector<param>;
   using TypeVec       = std::vector<llvm::Type*>;
   using ValueVec      = std::vector<llvm::Value*>;
   using ConstVec      = std::vector<llvm::Constant*>;
   using CallInstVec   = std::vector<llvm::CallInst*>;
   
   //-------------------------------------------------------------------------------------
   
   std::string demangle(const std::string& mangled_name);
   
   llvm::Function* get_function_with_unmangled_name(llvm::Module& module,
                                                    const std::string& unmangled_name);
                        
//--------------------------------------------------------------------------------------------------

llvm::Value* get_or_create_global_string_ptr(llvm::Module& module, llvm::Instruction& before, 
                                             const std::string& variable_name, 
                                             const std::string& str);
   
//--------------------------------------------------------------------------------------------------
    
   /// @brief Creates function signature F->ReturnType(NewParams, F->Params).

   llvm::FunctionType* create_signature(llvm::Function* F, const ParamVec& NewParams);
   
   //-------------------------------------------------------------------------------------
    
   /// @brief Creates a new llvm::Function with given name and F's signature extended with
   /// parameters in Params. Returns a pointer to the newly created function and a nullptr
   /// when a function with given name already exists in llvm::Module M.
   ///  @remark F's parameters are immutable.

   llvm::Function* create_function(llvm::Module& M,
                                   const std::string& name,
                                   llvm::Function* F,
                                   const ParamVec& Params);
   
   //-------------------------------------------------------------------------------------
    
   /// @brief Add a call to callee at the beginning of function F.

   llvm::CallInst* add_call_begin(llvm::Function* F,
                                  llvm::Function* callee,
                                  const llvm::ArrayRef<llvm::Value*>& args,
                                  const std::string& call_name="");
   
   //-------------------------------------------------------------------------------------
    
   /// @brief Add a call to callee before the return Instruction of function F.

   llvm::CallInst* add_call_end(llvm::Function* F,
                                llvm::Function* callee,
                                const llvm::ArrayRef<llvm::Value*>& args,
                                const std::string& call_name="");
   
   //-------------------------------------------------------------------------------------
    
   /// @brief Returns a vector containing all CallInst* with given callee in Function* F.
   
   CallInstVec call_instructions(llvm::Function* F, const std::string& callee);
   
   //-------------------------------------------------------------------------------------
    
   /// @brief Replace the given call to Function callee(args) with a call to
   /// newcallee(NewArgs, args).

   void replace_call(llvm::CallInst* call,
                     llvm::Function* callee,
                     llvm::Function* newcallee,
                     ValueVec NewArgs={});
   
   //-------------------------------------------------------------------------------------
    
   std::string outputname();
   
   //-------------------------------------------------------------------------------------
    
} // end namespace instrumentation_utils
