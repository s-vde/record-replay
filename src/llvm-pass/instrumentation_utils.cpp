
#include "instrumentation_utils.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Module.h>


namespace instrumentation_utils {

//--------------------------------------------------------------------------------------------------

llvm::Value* get_or_create_global_string_ptr(llvm::Module& module, llvm::Instruction& before,
                                             const std::string& variable_name,
                                             const std::string& str)
{
   llvm::IRBuilder<> builder(&before);
   llvm::GlobalVariable* global_variable = module.getGlobalVariable(variable_name, true);
   if (global_variable)
   {
      llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(module.getContext()), 0);
      return builder.CreateInBoundsGEP(global_variable->getValueType(), global_variable,
                                       {zero, zero}, variable_name);
   }
   return builder.CreateGlobalStringPtr(str, variable_name);
}

//--------------------------------------------------------------------------------------------------

llvm::CallInst* add_call_begin(llvm::Function* F, llvm::Function* callee,
                               const llvm::ArrayRef<llvm::Value*>& args,
                               const std::string& call_name)
{
   llvm::IRBuilder<> builder(&*(llvm::inst_begin(F)));
   return builder.CreateCall(callee, args, call_name);
}

//--------------------------------------------------------------------------------------------------

llvm::CallInst* add_call_end(llvm::Function* F, llvm::Function* callee,
                             const llvm::ArrayRef<llvm::Value*>& args, const std::string& call_name)
{
   llvm::IRBuilder<> builder(&*(--((--(F->end()))->end())));
   return builder.CreateCall(callee, args, call_name);
}

//--------------------------------------------------------------------------------------------------

} // end namespace instrumentation_utils
