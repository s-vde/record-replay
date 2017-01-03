#pragma once

// PROGRAM_MODEL
#include "visible_instruction.hpp"

// LLVM
#include <llvm/IR/InstVisitor.h>

// BOOST
#include <boost/optional.hpp>

// STL
#include <deque>

//--------------------------------------------------------------------------------------90
/// @file llvm_visible_instruction.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace llvm
{
   class GlobalVariable;
   class Instruction;
   class Module;
   class Value;
}

//----------------------------------------------------------------------------------------

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   class shared_object
   {
   public:
      
      using indices_t = std::deque<llvm::Value*>;
      
      /// @brief Constructor.
      
      shared_object(llvm::GlobalVariable* gvar, const indices_t& indices);
      
      /// @brief Constructs a program_model::Object from this shared_object before
      /// llvm::Instruction before and returns a pointer to it.
      
      llvm::Value* construct_model(llvm::Module& module, llvm::Instruction* before) const;
      
      void dump() const;
      
   private:
      
      llvm::GlobalVariable* m_gvar;
      indices_t m_indices;
      
   }; // end class shared_object

   //-------------------------------------------------------------------------------------
   
   template <typename operation_t>
   using visible_instruction = program_model::visible_instruction<operation_t, shared_object>;
   
   using memory_instruction = program_model::memory_instruction<shared_object>;
   
   using lock_instruction = program_model::lock_instruction<shared_object>;
   
   using visible_instruction_t = program_model::visible_instruction_t<shared_object>;
   
   //-------------------------------------------------------------------------------------
   
   struct construct_model : public boost::static_visitor<llvm::Value*>
   {
      construct_model(llvm::Module& module, llvm::Instruction* before)
      : m_module(module)
      , m_before(before)
      {
      }
      
      template <typename operation_t>
      llvm::Value* operator()(const visible_instruction<operation_t>& instruction) const
      {
         return instruction.operand().construct_model(m_module, m_before);
      }
      
      llvm::Module& m_module;
      llvm::Instruction* m_before;
      
   }; // end struct construct_model

   //-------------------------------------------------------------------------------------
   
   struct dump : public boost::static_visitor<void>
   {
      void operator()(const memory_instruction& instruction) const;
      void operator()(const lock_instruction& instruction) const;
      
   }; // end struct dump
   
   //-------------------------------------------------------------------------------------
   
   boost::optional<shared_object> get_shared_object(llvm::Value* operand);
   
   //-------------------------------------------------------------------------------------
   
   struct visible_instruction_creator
   : public llvm::InstVisitor<visible_instruction_creator, boost::optional<visible_instruction_t>>
   {
      using return_type = boost::optional<visible_instruction_t>;

      return_type visitLoadInst(llvm::LoadInst& instr);
      return_type visitStoreInst(llvm::StoreInst& instr);
      return_type visitAtomicRMWInst(llvm::AtomicRMWInst& instr);
      return_type visitCallInst(llvm::CallInst& instr);
      return_type visitInstruction(llvm::Instruction& instr);
      
   }; // end struct visible_instruction_creator;
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
