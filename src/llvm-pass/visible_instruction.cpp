
#include "visible_instruction.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// PROGRAM_MODEL
#include "object_io.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

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
   
   void shared_object::dump() const
   {
      llvm::errs() << m_gvar->getName() << "\n";
      for (const auto& index : m_indices)
      {
         index->dump();
      }
   }
   
   //-------------------------------------------------------------------------------------
   
   opt_visible_instruction_t get_visible_instruction(llvm::Instruction* instr)
   {
      instr->dump();
      visible_operation_creator operation_creator;
      auto visible_operation = operation_creator.visit(instr);
      if (visible_operation)
      {
         using namespace utils::io;
         auto shared_object = get_shared_object(visible_operation->second);
         if (shared_object)
         {
            PRINT("-----\n");
            PRINT(text_color(to_string(visible_operation->first), Color::GREEN) << "\n");
            shared_object->dump();
            PRINT("-----\n");
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
            auto begin = user->op_begin();
            ++begin;
            std::reverse_copy(begin, user->op_end(), std::front_inserter(indices));
         }
         else
         {
            return boost::optional<shared_object>();
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
   
   auto visible_operation_creator::visitLoadInst(llvm::LoadInst& instr) -> return_type
   {
      return boost::make_optional(visible_operation_t(program_model::Object::Op::READ,
                                                      instr.getPointerOperand()));
   }
   
   //-------------------------------------------------------------------------------------
   
   auto visible_operation_creator::visitStoreInst(llvm::StoreInst& instr) -> return_type
   {
      return boost::make_optional(visible_operation_t(program_model::Object::Op::WRITE,
                                                      instr.getPointerOperand()));
   }
   
   //-------------------------------------------------------------------------------------
   
   auto visible_operation_creator::visitAtomicRMWInst(llvm::AtomicRMWInst& instr) -> return_type
   {
//      return boost::make_optional(visible_operation_t(program_model::Object::Op::AtomicRMW,
//                                                      instr.getPointerOperand()));
      return return_type();
   }
   
   //-------------------------------------------------------------------------------------
   
   auto visible_operation_creator::visitCallInst(llvm::CallInst& instr) -> return_type
   {
      using Op = program_model::Object::Op;
      llvm::Function* callee = instr.getCalledFunction();
   
      if (callee->getName() == "pthread_mutex_lock")
      {
         return boost::make_optional(visible_operation_t(Op::LOCK, instr.getArgOperand(0)));
      }
      else if (callee->getName() == "pthread_mutex_unlock")
      {
         return boost::make_optional(visible_operation_t(Op::UNLOCK, instr.getArgOperand(0)));
      }
      return return_type();
   }
   
   //-------------------------------------------------------------------------------------
   
   auto visible_operation_creator::visitInstruction(llvm::Instruction& instr) -> return_type
   {
      return return_type();
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
