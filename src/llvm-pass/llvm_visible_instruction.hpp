#pragma once

// PROGRAM_MODEL
#include "visible_instruction.hpp"

// LLVM
#include <llvm/IR/InstVisitor.h>

// BOOST
#include <boost/optional.hpp>

//--------------------------------------------------------------------------------------90
/// @file llvm_visible_instruction.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace llvm
{
   class Instruction;
   class Module;
   class Value;
}

//----------------------------------------------------------------------------------------

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   using operand_t = llvm::Value*;
   
   template <typename operation_t>
   using visible_instruction = program_model::visible_instruction<operation_t, operand_t>;
   
   using memory_instruction = program_model::memory_instruction<operand_t>;
   
   using lock_instruction = program_model::lock_instruction<operand_t>;
   
   using visible_instruction_t = program_model::visible_instruction_t<operand_t>;
   
   //-------------------------------------------------------------------------------------
   
   struct construct_operand : public boost::static_visitor<llvm::Value*>
   {
      construct_operand(llvm::Module& module, llvm::Instruction* before)
      : m_module(module)
      , m_before(before)
      {
      }
      
      template <typename operation_t>
      llvm::Value* operator()(const visible_instruction<operation_t>& instruction) const
      {
         return construct_operand_impl(m_module, instruction.operand(), m_before);
      }
      
   private:
      
      llvm::Value* construct_operand_impl(llvm::Module& module,
                                          operand_t operand,
                                          llvm::Instruction* before) const;
      
      llvm::Module& m_module;
      llvm::Instruction* m_before;
      
   }; // end struct construct_operand

   //-------------------------------------------------------------------------------------
   
   struct dump : public boost::static_visitor<void>
   {
      void operator()(const memory_instruction& instruction) const;
      void operator()(const lock_instruction& instruction) const;
      
   }; // end struct dump
   
   //-------------------------------------------------------------------------------------
   
   namespace llvm_visible_instruction
   {
      struct creator
      : public llvm::InstVisitor<creator, boost::optional<visible_instruction_t>>
      {
         using return_type = boost::optional<visible_instruction_t>;
         
         // Potential Visible Instructions
         return_type visitLoadInst(llvm::LoadInst& instr);
         return_type visitStoreInst(llvm::StoreInst& instr);
         return_type visitAtomicRMWInst(llvm::AtomicRMWInst& instr);
         return_type visitCallInst(llvm::CallInst& instr);
         
         // Default
         return_type visitInstruction(llvm::Instruction& instr);
         
      }; // end struct creator
      
   } // end namesapce llvm_visible_instruction
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
