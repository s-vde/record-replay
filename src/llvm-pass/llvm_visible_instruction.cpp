
#include "llvm_visible_instruction.hpp"

// LLVM_PASS
#include "instrumentation_utils.hpp"
#include "print.hpp"

// PROGRAM_MODEL
#include "visible_instruction_io.hpp"

// UTILS
#include "color_output.hpp"

// LLVM
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

// STL
#include <algorithm>
#include <assert.h>

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   shared_object::shared_object(llvm::Value* value, llvm::GlobalVariable* gvar, const indices_t& indices)
   : m_value(value)
   , m_gvar(gvar)
   , m_indices(indices)
   {
   }
   
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
      return nullptr;
   }
   
   //-------------------------------------------------------------------------------------
   
   llvm::Value* shared_object::construct_model(llvm::Module& module, llvm::Instruction* before) const
   {
      using namespace utils::io;
      using namespace instrumentation_utils;
      const static auto mangled_name = get_mangled_name(module, "program_model", "", "llvm_object");
      if (mangled_name)
      {
         using namespace llvm;
         static Type* object_type = module.getTypeByName("class.program_model::Object");
         static Function* llvm_object = cast<Function>(module.getFunction(*mangled_name));
         
         // Construct program_model::Object
         IRBuilder<> builder(before);
         auto* obj = builder.CreateAlloca(object_type);
         auto* gvar_name = builder.CreateGlobalStringPtr(m_gvar ? m_gvar->getName().str() : "");
         auto* address = builder.CreatePointerCast(m_value, builder.getInt8PtrTy());
         CallInst::Create(llvm_object, { obj, address, gvar_name }, "", before);
         
         // Add m_indices to constructed program_model::Object
         static Function* add_index = cast<Function>(module.getFunction("_ZN13program_model6Object9add_indexEi"));
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
   
   namespace
   {
      template <typename operation_t>
      void dump_base(const visible_instruction<operation_t>& instruction)
      {
         using namespace utils::io;
         llvm::errs() << text_color(to_string(instruction.operation()), Color::GREEN) << "\n";
         instruction.operand().dump();
      }
      
   } // end namespace
   
   //-------------------------------------------------------------------------------------
   
   void dump::operator()(const memory_instruction& instruction) const
   {
      using namespace utils::io;
      if (instruction.is_atomic())
      {
         llvm::errs() << text_color("atomic ", Color::GREEN);
      }
      dump_base(instruction);
   }
   
   //-------------------------------------------------------------------------------------
   
   void dump::operator()(const lock_instruction& instruction) const
   {
      dump_base(instruction);
   }
   
   //-------------------------------------------------------------------------------------
   
   boost::optional<shared_object> get_shared_object(llvm::Value* operand)
   {
      llvm::Value* original_operand = operand;
      std::deque<llvm::Value*> indices;
      
      while (true)
      {
         // GlobalVariable
         if (llvm::GlobalVariable* gvar = llvm::dyn_cast<llvm::GlobalVariable>(operand))
         {
            return boost::make_optional(shared_object(original_operand, gvar, indices));
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

   namespace llvm_visible_instruction
   {
      //----------------------------------------------------------------------------------
      
      template <typename instruction_t, typename ... args_t>
      creator::return_type create(const typename instruction_t::operation_t& operation,
                                  llvm::Value* value,
                                  args_t&& ... args)
      {
         auto shared_object = get_shared_object(value);
         if (shared_object)
         {
            return creator::return_type(instruction_t(operation, *shared_object, std::forward<args_t>(args) ...));
         }
         return creator::return_type();
      }
      
      //----------------------------------------------------------------------------------
      // creator
      //----------------------------------------------------------------------------------
      
      using memory_operation = program_model::memory_operation;
      using lock_operation = program_model::lock_operation;
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitLoadInst(llvm::LoadInst& instr) -> return_type
      {
         instr.dump();
         return create<memory_instruction>(memory_operation::Load, instr.getPointerOperand(), instr.isAtomic());
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitStoreInst(llvm::StoreInst& instr) -> return_type
      {
         instr.dump();
         return create<memory_instruction>(memory_operation::Store, instr.getPointerOperand(), instr.isAtomic());
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitAtomicRMWInst(llvm::AtomicRMWInst& instr) -> return_type
      {
         instr.dump();
         assert(instr.isAtomic());
         return create<memory_instruction>(memory_operation::ReadModifyWrite, instr.getPointerOperand(), true);
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitCallInst(llvm::CallInst& instr) -> return_type
      {
         instr.dump();
         
         llvm::Function* callee = instr.getCalledFunction();
         if (callee->getName() == "pthread_mutex_lock")
         {
            return create<lock_instruction>(lock_operation::Lock, instr.getArgOperand(0));
         }
         else if (callee->getName() == "pthread_mutex_unlock")
         {
            return create<lock_instruction>(lock_operation::Unlock, instr.getArgOperand(0));
         }
         return return_type();
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitInstruction(llvm::Instruction& instr) -> return_type
      {
         instr.dump();
         return return_type();
      }
      
      //----------------------------------------------------------------------------------
      
   } // end namespace llvm_visible_instruction
   
} // end namespace record_replay
