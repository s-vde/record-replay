
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
#include <assert.h>

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   llvm::Value* construct_operand::construct_operand_impl(llvm::Module& module,
                                                          operand_t operand,
                                                          llvm::Instruction* before) const
   {
      llvm::IRBuilder<> builder(before);
      return builder.CreatePointerCast(operand, builder.getInt8PtrTy());
   }
   
   //-------------------------------------------------------------------------------------
   
   namespace
   {
      template <typename operation_t>
      void dump_base(const visible_instruction<operation_t>& instruction)
      {
         using namespace utils::io;
         llvm::errs() << text_color(to_string(instruction.operation()), Color::GREEN) << "\n";
         instruction.operand()->dump();
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

   namespace llvm_visible_instruction
   {
      //----------------------------------------------------------------------------------
      
      bool is_thread_local(const llvm::Value& operand)
      {
         const llvm::Value* address_start = operand.stripInBoundsOffsets();
         if (llvm::isa<llvm::GlobalVariable>(address_start))
         {
            return false;
         }
         return false;
      }
      
      //----------------------------------------------------------------------------------
      
      template <typename instruction_t, typename ... args_t>
      creator::return_type create(const typename instruction_t::operation_t& operation,
                                  llvm::Value* operand,
                                  args_t&& ... args)
      {
         if (!is_thread_local(*operand))
         {
            return creator::return_type(instruction_t(operation, operand,
                                                      std::forward<args_t>(args) ...));
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
         return create<memory_instruction>(memory_operation::Load,
                                           instr.getPointerOperand(),
                                           instr.isAtomic());
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitStoreInst(llvm::StoreInst& instr) -> return_type
      {
         return create<memory_instruction>(memory_operation::Store,
                                           instr.getPointerOperand(),
                                           instr.isAtomic());
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitAtomicRMWInst(llvm::AtomicRMWInst& instr) -> return_type
      {
         assert(instr.isAtomic());
         return create<memory_instruction>(memory_operation::ReadModifyWrite,
                                           instr.getPointerOperand(),
                                           true);
      }
      
      //----------------------------------------------------------------------------------
      
      auto creator::visitCallInst(llvm::CallInst& instr) -> return_type
      {
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
         return return_type();
      }
      
      //----------------------------------------------------------------------------------
      
   } // end namespace llvm_visible_instruction
   
} // end namespace record_replay
