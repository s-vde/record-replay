#pragma once

#include <llvm/ADT/ArrayRef.h>

#include <string>

//--------------------------------------------------------------------------------------------------
/// @file instrumentation_utils.hpp
/// @brief Set of utility functions for instrumentation of LLVM IR modules.
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace llvm {
class CallInst;
class Function;
class Instruction;
class Module;
class Value;
} // end namespace llvm


namespace instrumentation_utils {

llvm::Value* get_or_create_global_string_ptr(llvm::Module& module, llvm::Instruction& before,
                                             const std::string& variable_name,
                                             const std::string& str);

/// @brief Add a call to callee at the beginning of function F.

llvm::CallInst* add_call_begin(llvm::Function* F, llvm::Function* callee,
                               const llvm::ArrayRef<llvm::Value*>& args,
                               const std::string& call_name = "");

/// @brief Add a call to callee before the return Instruction of function F.

llvm::CallInst* add_call_end(llvm::Function* F, llvm::Function* callee,
                             const llvm::ArrayRef<llvm::Value*>& args,
                             const std::string& call_name = "");

} // end namespace instrumentation_utils
