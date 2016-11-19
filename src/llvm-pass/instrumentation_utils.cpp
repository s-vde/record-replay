
#include "instrumentation_utils.hpp"

// LLVM_PASS
#include "print.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"

// STL
#include <string>

//--------------------------------------------------------------------------------------90

namespace instrumentation_utils
{
   //-------------------------------------------------------------------------------------
   
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
      PRINTF(outputname(), "create_function", F->getName().str() << ", " << name, "\n");
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
            NA->takeName(A);
            A->replaceAllUsesWith(NA);
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
      PRINTF(outputname(), "add_call_begin", F->getName() << ", " << callee->getName(), "\n");
      return llvm::CallInst::Create(callee, args, call_name, F->begin()->begin());
   }
   
   //-------------------------------------------------------------------------------------
    
   llvm::CallInst* add_call_end(llvm::Function* F,
                                llvm::Function* callee,
                                const llvm::ArrayRef<llvm::Value*>& args,
                                const std::string& call_name)
   {
      PRINTF(outputname(), "add_call_end", F->getName() << ", " << callee->getName(), "\n");
      return llvm::CallInst::Create(callee, args, call_name, --((--(F->end()))->end()));
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
    
   void replace_call(llvm::CallInst* call,
                     llvm::Function* callee,
                     llvm::Function* newcallee,
                     ValueVec NewArgs)           // by value
   {
      PRINTF(outputname(), "replace_call", callee->getName() << "," << newcallee->getName(), "\n");
      const unsigned nr_args = call->getNumArgOperands();
      for (unsigned int i = 0; i < nr_args; ++i)
      {
         NewArgs.push_back(call->getArgOperand(i));
      }
      call->replaceAllUsesWith(
         llvm::CallInst::Create(
            newcallee,                              // Func
            llvm::ArrayRef<llvm::Value*>(NewArgs),  // Args
            call->getName(),                        // NameStr
            call                                    // InsertBefore
         )
      );
      // @todo call->dropAllReferences();?
      call->eraseFromParent();
   }
   
   //-------------------------------------------------------------------------------------

   /// @brief Creates a global C-string constant with given name and initialized with a
   /// constant array corresponding to given string str.
   /// @details The type of the llvm::GlobalVariable is an array type [str.size()+1 x i8].

   llvm::Constant* create_global_cstring_const(llvm::Module& M,
                                               const std::string& name,
                                               const std::string& str)
   {
      llvm::GlobalVariable* gvar = new llvm::GlobalVariable(
         M,                                                                                  // Module
         llvm::ArrayType::get(llvm::IntegerType::getInt8Ty(M.getContext()), str.size()+1),   // Type
         true,                                                                               // isConstant
         llvm::GlobalValue::PrivateLinkage,                                                  // Linkage
         llvm::ConstantDataArray::getString(M.getContext(), str, true),                      // Initializer
         name                                                                                // Name
      );
      gvar->setAlignment(1);
      llvm::ConstantInt* zero = llvm::ConstantInt::get(
         M.getContext(),
         llvm::APInt(64, llvm::StringRef("0"), 10)
      );
      return llvm::ConstantExpr::getGetElementPtr(gvar, ConstVec{ zero, zero });
   }
   
   //-------------------------------------------------------------------------------------
	
   boost::optional<std::string> get_mangled_name(const llvm::Module& M,
                                                 const std::string& namespc,
                                                 const std::string& class_name,
                                                 const std::string& name)
   {
      std::size_t pos = 0;
      for (auto fun = M.getFunctionList().begin(); fun != M.getFunctionList().end(); ++fun)
      {
         const std::string& mangled = fun->getName().str();
         pos = 0;
         if ((pos = mangled.find(namespc, pos)) != std::string::npos)
         {
            if ((pos = mangled.find(class_name, pos)) != std::string::npos)
            {
               if ((pos = mangled.find(name, pos)) != std::string::npos)
               {
                  PRINT(name << " found with mangled name " << mangled << "\n");
                  return boost::make_optional(mangled);
               }
            }
         }
      }
      return boost::optional<std::string>();
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string outputname()
   {
      return utils::io::text_color("\tinstrumentation_utils", utils::io::Color::CYAN);
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace instrumentation_utils
