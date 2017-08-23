#pragma once

#include "visible_instruction.hpp"

#include <llvm/IR/InstIterator.h>
#include <llvm/IR/InstVisitor.h>

#include <boost/optional.hpp>

#include <vector>

//--------------------------------------------------------------------------------------------------
/// @file llvm_visible_instruction.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//--------------------------------------------------------------------------------------------------


namespace llvm {
class Instruction;
class Module;
class Value;
} // end namespace llvm


namespace concurrency_passes {

//--------------------------------------------------------------------------------------------------

// Forward declarations
class Functions;

//--------------------------------------------------------------------------------------------------

using operand_t = llvm::Value*;

template <typename operation_t>
using visible_instruction = program_model::visible_instruction<operation_t, operand_t>;

using memory_instruction = program_model::memory_instruction<operand_t>;

using lock_instruction = program_model::lock_instruction<operand_t>;

using visible_instruction_t = program_model::visible_instruction_t<operand_t>;

//--------------------------------------------------------------------------------------------------

struct wrap : public boost::static_visitor<void>
{
   using arguments_t = std::vector<llvm::Value*>;

   wrap(llvm::Module& module, Functions& functions, llvm::inst_iterator& instruction_it);

   void operator()(const memory_instruction& instruction);
   void operator()(const lock_instruction& instruction);

private:
   arguments_t construct_arguments(const memory_instruction& instruction);
   arguments_t construct_arguments(const lock_instruction& instruction);

   llvm::Value* construct_operand(const operand_t& operand);
   llvm::Value* construct_file_name(const std::string& file_name);
   llvm::Value* construct_line_number(unsigned int line_number);

   llvm::Module& m_module;
   Functions& m_functions;
   llvm::inst_iterator& m_instruction_it;

}; // end struct construct_instruction

//--------------------------------------------------------------------------------------------------

struct dump : public boost::static_visitor<void>
{
   void operator()(const memory_instruction& instruction) const;
   void operator()(const lock_instruction& instruction) const;

}; // end struct dump

//--------------------------------------------------------------------------------------------------

namespace llvm_visible_instruction {
struct creator : public llvm::InstVisitor<creator, boost::optional<visible_instruction_t>>
{
   using return_type = boost::optional<visible_instruction_t>;

   // Potential Visible Instructions
   return_type visitLoadInst(llvm::LoadInst& instr);
   return_type visitStoreInst(llvm::StoreInst& instr);
   return_type visitAtomicRMWInst(llvm::AtomicRMWInst& instr);
   return_type visitCallInst(llvm::CallInst& instr);
   return_type visitInvokeInst(llvm::InvokeInst& instr);

   // Default
   return_type visitInstruction(llvm::Instruction& instr);

private:
   return_type handle_call_and_invoke_instr(
      llvm::Instruction& instr, const llvm::Function* callee,
      const llvm::iterator_range<llvm::User::const_op_iterator>& arg_operands);

}; // end struct creator

} // end namesapce llvm_visible_instruction

//--------------------------------------------------------------------------------------------------
} // end namespace concurrency_passes
