
#include "instrumentation_utils.hpp"

// LLVM_PASS
#include "print.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Module.h>

// STL
#include <cxxabi.h>
#include <exception>

namespace instrumentation_utils
{
   //-------------------------------------------------------------------------------------
   
   std::string demangle(const std::string& mangled_name)
   {
      int status = -1;
      const char* demangled_name = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
      return (status==0) ? demangled_name : "";
   }
   
   //-------------------------------------------------------------------------------------
   
   llvm::Function* get_function_with_unmangled_name(llvm::Module& module,
                                                    const std::string& unmangled_name)
   {
      auto& function_list = module.getFunctionList();
      for (auto& function : function_list)
      {
         if (demangle(function.getName().str()) == unmangled_name)
         {
            llvm::errs() << "Function " + unmangled_name + " with mangled name " + function.getName().str() + " found in module\n";
            return &function;
         }
      }
      throw std::invalid_argument("Function " + unmangled_name + " not found in module");
   }
   
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
                                       { zero, zero }, variable_name);
   }
   return builder.CreateGlobalStringPtr(str, variable_name);
}

//--------------------------------------------------------------------------------------------------
   
   llvm::FunctionType* create_signature(llvm::Function* F, const ParamVec& NewParams)
   {
      const llvm::FunctionType* Type_F = F->getFunctionType();
      TypeVec Params(Type_F->param_begin(), Type_F->param_end());
      for (auto& it : NewParams)
      {
         Params.push_back(it.first);
         // Here attributes may be set
         // e.g. Attr.addAttribute(F->getContext(), Params.size(), llvm::Attribute::ByVal);
      }
      return llvm::FunctionType::get(F->getReturnType(),
                                     llvm::ArrayRef<llvm::Type*>(Params),
                                     Type_F->isVarArg());
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::Function* create_function(llvm::Module& M,
                                   const std::string& name,
                                   llvm::Function* F,
                                   const ParamVec& NewParams)
   {
      if (M.getFunction(name) == NULL)
      {
         llvm::Function* NewF = llvm::cast<llvm::Function>(
            M.getOrInsertFunction(
               name,
               create_signature(F, NewParams),
               F->getAttributes()
            )
         );
         llvm::Function::arg_iterator NA = NewF->arg_begin();
         for (llvm::Function::arg_iterator A = F->arg_begin(); A != F->arg_end(); ++A, ++NA)
         {
            NA->takeName(&*A);
            A->replaceAllUsesWith(&*NA);
         }
         for (const auto& NP : NewParams)
         {
            NA->setName(NP.second);
            ++NA;
         }
         return NewF;
      }
      return nullptr;
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::CallInst* add_call_begin(llvm::Function* F,
                                  llvm::Function* callee,
                                  const llvm::ArrayRef<llvm::Value*>& args,
                                  const std::string& call_name)
   {
      llvm::IRBuilder<> builder(&*(llvm::inst_begin(F)));
      return builder.CreateCall(callee, args, call_name);
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::CallInst* add_call_end(llvm::Function* F,
                                llvm::Function* callee,
                                const llvm::ArrayRef<llvm::Value*>& args,
                                const std::string& call_name)
   {
      llvm::IRBuilder<> builder(&*(--((--(F->end()))->end())));
      return builder.CreateCall(callee, args, call_name);
   }
   
   //-------------------------------------------------------------------------------------
    
   CallInstVec call_instructions(llvm::Function* F, const std::string& callee)
   {
      CallInstVec calls{};
      for (auto I = llvm::inst_begin(F); I != llvm::inst_end(F); ++I)
      {
         if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&*I))
         {
            if (call->getCalledFunction()->getName() == callee)
            {
               calls.push_back(call);
            }
         }
      }
      return calls;
   }
   
   //-------------------------------------------------------------------------------------
    
   void replace_call(llvm::CallInst* call, llvm::Function* callee,
                     llvm::Function* newcallee, ValueVec NewArgs)           // by value
   {
      const unsigned nr_args = call->getNumArgOperands();
      for (unsigned int i = 0; i < nr_args; ++i)
      {
         NewArgs.push_back(call->getArgOperand(i));
      }
      llvm::IRBuilder<> builder(call);
      llvm::CallInst* new_call = builder.CreateCall(newcallee,
                                                    llvm::ArrayRef<llvm::Value*>(NewArgs),
                                                    call->getName());
      call->replaceAllUsesWith(new_call);
      // @todo call->dropAllReferences();?
      call->eraseFromParent();
   }
   
//--------------------------------------------------------------------------------------------------
   
void replace_invoke(llvm::InvokeInst* invoke, llvm::Function* callee,
                    llvm::Function* newcallee, ValueVec NewArgs)
{
   const unsigned nr_args = invoke->getNumArgOperands();
   for (unsigned int i = 0; i < nr_args; ++i)
   {
      NewArgs.push_back(invoke->getArgOperand(i));
   }
   llvm::IRBuilder<> builder(invoke);
   llvm::InvokeInst* new_invoke = builder.CreateInvoke(newcallee,
                                                       invoke->getNormalDest(), 
                                                       invoke->getUnwindDest(),
                                                       llvm::ArrayRef<llvm::Value*>(NewArgs),
                                                       invoke->getName());
   invoke->replaceAllUsesWith(new_invoke);
   // @todo call->dropAllReferences();?
   invoke->eraseFromParent();
}                      
                       
//--------------------------------------------------------------------------------------------------       
    
   std::string outputname()
   {
      return utils::io::text_color("\tinstrumentation_utils", utils::io::Color::CYAN);
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace instrumentation_utils
