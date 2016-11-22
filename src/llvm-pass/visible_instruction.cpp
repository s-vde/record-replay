
#include "visible_instruction.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// PROGRAM_MODEL
#include "object_io.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include "llvm/IR/IRBuilder.h"

// STL
#include <algorithm>

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   shared_object::shared_object(llvm::GlobalVariable* gvar, const indices_t& indices)
   : m_gvar(gvar)
   , m_indices(indices) { }
   
   //-------------------------------------------------------------------------------------
   
   llvm::Value* integer(llvm::Module& module, llvm::Instruction* before, llvm::Value* value)
   {
      using namespace llvm;
      Type* int32_type = IntegerType::getInt32Ty(module.getContext());
      if (ConstantInt* const_int = dyn_cast<ConstantInt>(value))
      {
         return AddrSpaceCastInst::CreateIntegerCast(const_int, int32_type, false, "", before);
      }
      else if (llvm::SExtInst* sext_inst = llvm::dyn_cast<llvm::SExtInst>(value))
      {
         return sext_inst->getOperand(0);
      }
      using namespace utils::io;
      PRINT(text_color("unhandled index case\n", Color::RED));
   }
   
   //-------------------------------------------------------------------------------------
   
   llvm::Value* shared_object::construct_model(llvm::Module& module, llvm::Instruction* before) const
   {
      using namespace utils::io;
      PRINT(text_color("shared_object::construct_model()", Color::YELLOW) << "\n");
      
      using instrumentation_utils::get_mangled_name;
      const static auto mangled_name = get_mangled_name(module, "program_model", "", "llvm_object");
      if (mangled_name)
      {
         using namespace llvm;
         static Type* object_type = module.getTypeByName("class.program_model::Object");
         static Function* add_index = cast<Function>(module.getFunction("_ZN13program_model6Object9add_indexEi"));
         static Function* llvm_object = cast<Function>(module.getFunction(*mangled_name));
         
         AllocaInst* obj = new AllocaInst(object_type, "obj", before);
         CallInst::Create(llvm_object,
                          { obj, instrumentation_utils::create_global_cstring_const(module, "", m_gvar->getName().str()) },
                          "",
                          before);
         
         for (const auto& index : m_indices)
         {
            CallInst::Create(add_index, { obj, integer(module, before, index) }, "", before);
         }
         return obj;
      }
      return nullptr;
   }
   
   //-------------------------------------------------------------------------------------
    
   boost::optional<visible_operation_t> get_visible_operation(llvm::Instruction* I)
   {
      using Op = program_model::Object::Op;
      
      // READ
      if (llvm::LoadInst* LI = llvm::dyn_cast<llvm::LoadInst>(I))
      {
         return boost::make_optional(visible_operation_t(Op::READ, LI->getPointerOperand()));
      }
      
      // WRITE
      if (llvm::StoreInst* SI = llvm::dyn_cast<llvm::StoreInst>(I))
      {
         return boost::make_optional(visible_operation_t(Op::WRITE, SI->getPointerOperand()));
      }
      
      if (llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(I))
      {
         llvm::Function* callee = CI->getCalledFunction();
         
         // LOCK
         if (callee->getName() == "pthread_mutex_lock")
         {
            return boost::make_optional(visible_operation_t(Op::LOCK, CI->getArgOperand(0)));
         }
         
         // UNLOCK
         if(callee->getName() == "pthread_mutex_unlock")
         {
            return boost::make_optional(visible_operation_t(Op::UNLOCK, CI->getArgOperand(0)));
         }
      }
      
      if (!llvm::isa<llvm::AllocaInst>(I))
      {
         using namespace utils::io;
         PRINT(text_color("unhandled", Color::RED));
      }
      I->dump();
      return boost::optional<visible_operation_t>();
   }
   
   //-------------------------------------------------------------------------------------
   
   opt_visible_instruction_t get_visible_instruction(llvm::Instruction* instr)
   {
      auto visible_operation = get_visible_operation(instr);
      if (visible_operation)
      {
         using namespace utils::io;
         PRINT("-----\n" << text_color(to_string(visible_operation->first), Color::GREEN) << "\n");
         auto shared_object = get_shared_object(visible_operation->second);
         PRINT("-----\n");
         if (shared_object)
         {
            return boost::make_optional(visible_instruction_t(visible_operation->first, *shared_object));
         }
      }
      return boost::optional<visible_instruction_t>();
   }
   
   //-------------------------------------------------------------------------------------
    
   boost::optional<shared_object> get_shared_object(llvm::Value* operand)
   {
      using namespace utils::io;
      
      std::deque<llvm::Value*> indices;
      
      while (true)
      {
         // GlobalVariable
         if (llvm::GlobalVariable* gvar = llvm::dyn_cast<llvm::GlobalVariable>(operand))
         {
            PRINT(text_color("GlobalVariable=", Color::GREEN) << gvar->getName() << "\n");
            for (const auto& index : indices)
            {
               index->dump();
            }
            return boost::make_optional(shared_object(gvar, indices));
         }
         
         llvm::User* user = nullptr;
         
         if (llvm::ConstantExpr* const_expr = llvm::dyn_cast<llvm::ConstantExpr>(operand))
         {
            if (const_expr->isGEPWithNoNotionalOverIndexing())
            {
               user = const_expr;
               operand = const_expr->getOperand(0);
            }
         }
         else if (llvm::GetElementPtrInst* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(operand))
         {
            if (!gep->getPointerOperand()->getType()->isStructTy())
            {
               user = gep;
               operand = gep->getPointerOperand();
            }
         }
         
         // Add indices to the deque
         if (user && user->getNumOperands() > 1)
         {
            PRINT(text_color("GEP pointer_op=", Color::GREEN)); operand->dump();
            auto begin = user->op_begin();
            ++begin;
            std::reverse_copy(begin, user->op_end(), std::front_inserter(indices));
         }
         else
         {
            PRINT(text_color("unhandled", Color::RED) << "\n");
            return boost::optional<shared_object>();
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
