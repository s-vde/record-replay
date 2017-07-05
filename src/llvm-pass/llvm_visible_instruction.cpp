
#include "llvm_visible_instruction.hpp"

#include "functions.hpp"
#include "instrumentation_utils.hpp"

#include "visible_instruction_io.hpp"

#include "color_output.hpp"

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <assert.h>


namespace concurrency_passes {

//--------------------------------------------------------------------------------------------------

wrap::wrap(llvm::Module& module, Functions& functions, llvm::inst_iterator& instruction_it)
: m_module(module)
, m_functions(functions)
, m_instruction_it(instruction_it)
{
}

//--------------------------------------------------------------------------------------------------

void wrap::operator()(const memory_instruction& instruction)
{
   auto arguments = construct_arguments(instruction);
   llvm::CallInst::Create(m_functions.Wrapper_post_memory_instruction(), arguments, "",
                          &*m_instruction_it);
}

//--------------------------------------------------------------------------------------------------

void wrap::operator()(const lock_instruction& instruction)
{
   auto arguments = construct_arguments(instruction);
   llvm::CallInst::Create(m_functions.Wrapper_post_instruction(), arguments, "",
                          &*m_instruction_it);
}

//--------------------------------------------------------------------------------------------------

auto wrap::construct_arguments(const memory_instruction& instruction) -> arguments_t
{
   using namespace llvm;
   Value* arg_operation = ConstantInt::get(
      m_module.getContext(), APInt(32, static_cast<int>(instruction.operation()), false));
   Value* arg_operand = construct_operand(instruction.operand());
   Value* arg_is_atomic =
      ConstantInt::get(m_module.getContext(), APInt(8, (instruction.is_atomic() ? 1 : 0), false));
   Value* arg_file_name = construct_file_name(instruction.meta_data().file_name);
   Value* arg_line_number = construct_line_number(instruction.meta_data().line_number);
   return {arg_operation, arg_operand, arg_is_atomic, arg_file_name, arg_line_number};
}

//--------------------------------------------------------------------------------------------------

auto wrap::construct_arguments(const lock_instruction& instruction) -> arguments_t
{
   using namespace llvm;
   Value* arg_operation = ConstantInt::get(
      m_module.getContext(), APInt(32, static_cast<int>(instruction.operation()), false));
   Value* arg_operand = construct_operand(instruction.operand());
   Value* arg_file_name = construct_file_name(instruction.meta_data().file_name);
   Value* arg_line_number = construct_line_number(instruction.meta_data().line_number);
   return {arg_operation, arg_operand, arg_file_name, arg_line_number};
}

//--------------------------------------------------------------------------------------------------

llvm::Value* wrap::construct_operand(const operand_t& operand)
{
   llvm::IRBuilder<> builder(&*m_instruction_it);
   return builder.CreatePointerCast(operand, builder.getInt8PtrTy());
}

//--------------------------------------------------------------------------------------------------

llvm::Value* wrap::construct_file_name(const std::string& file_name)
{
   std::string global_name = "_recrep_file_name_" + file_name;
   return instrumentation_utils::get_or_create_global_string_ptr(m_module, *m_instruction_it,
                                                                 global_name, file_name);
}

//--------------------------------------------------------------------------------------------------

llvm::Value* wrap::construct_line_number(unsigned int line_number)
{
   return llvm::ConstantInt::get(m_module.getContext(), llvm::APInt(32, line_number, false));
}

//--------------------------------------------------------------------------------------------------


namespace {
    
template <typename operation_t>
void dump_base(const visible_instruction<operation_t>& instruction)
{
   using namespace utils::io;
   llvm::errs() << text_color(to_string(instruction.operation()), Color::GREEN) << "\n";
   instruction.operand()->dump();
}

} // end namespace


//--------------------------------------------------------------------------------------------------

void dump::operator()(const memory_instruction& instruction) const
{
   using namespace utils::io;
   if (instruction.is_atomic())
   {
      llvm::errs() << text_color("atomic ", Color::GREEN);
   }
   dump_base(instruction);
}

//--------------------------------------------------------------------------------------------------

void dump::operator()(const lock_instruction& instruction) const
{
   dump_base(instruction);
}

//--------------------------------------------------------------------------------------------------


namespace llvm_visible_instruction {
    
//--------------------------------------------------------------------------------------------------

bool is_thread_local(const llvm::Value& operand)
{
   const llvm::Value* address_start = operand.stripInBoundsOffsets();
   if (llvm::isa<llvm::GlobalVariable>(address_start))
   {
      return false;
   }
   return false;
}

//--------------------------------------------------------------------------------------------------

boost::optional<program_model::meta_data_t> get_meta_data(llvm::Instruction& instruction)
{
   if (llvm::DILocation* location = instruction.getDebugLoc())
   {
      return boost::make_optional<program_model::meta_data_t>(
         {location->getFilename().str(), location->getLine()});
   }
   return boost::optional<program_model::meta_data_t>();
}

//--------------------------------------------------------------------------------------------------

template <typename instruction_t, typename... args_t>
creator::return_type create(llvm::Instruction& instruction,
                            const typename instruction_t::operation_t& operation,
                            llvm::Value* operand, args_t&&... args)
{
   if (!is_thread_local(*operand))
   {
      instruction_t visible_instruction(operation, operand, std::forward<args_t>(args)...);
      auto meta_data = get_meta_data(instruction);
      if (meta_data)
      {
         visible_instruction.add_meta_data(*meta_data);
      }
      return creator::return_type(visible_instruction);
   }
   return creator::return_type();
}

//--------------------------------------------------------------------------------------------------
// creator
//--------------------------------------------------------------------------------------------------

using memory_operation = program_model::memory_operation;
using lock_operation = program_model::lock_operation;

//--------------------------------------------------------------------------------------------------

auto creator::visitLoadInst(llvm::LoadInst& instr) -> return_type
{
   return create<memory_instruction>(instr, memory_operation::Load, instr.getPointerOperand(),
                                     instr.isAtomic());
}

//--------------------------------------------------------------------------------------------------

auto creator::visitStoreInst(llvm::StoreInst& instr) -> return_type
{
   return create<memory_instruction>(instr, memory_operation::Store, instr.getPointerOperand(),
                                     instr.isAtomic());
}

//--------------------------------------------------------------------------------------------------

auto creator::visitAtomicRMWInst(llvm::AtomicRMWInst& instr) -> return_type
{
   assert(instr.isAtomic());
   return create<memory_instruction>(instr, memory_operation::ReadModifyWrite,
                                     instr.getPointerOperand(), true);
}

//--------------------------------------------------------------------------------------------------

auto creator::visitCallInst(llvm::CallInst& instr) -> return_type
{
   llvm::Function* callee = instr.getCalledFunction();
   // Direct function invocation
   if (callee)
   {
      if (callee->getName() == "pthread_mutex_lock")
      {
         return create<lock_instruction>(instr, lock_operation::Lock, instr.getArgOperand(0));
      }
      else if (callee->getName() == "pthread_mutex_unlock")
      {
         return create<lock_instruction>(instr, lock_operation::Unlock, instr.getArgOperand(0));
      }
   }
   /// @todo Case of indirect function invokation
   return return_type();
}

//--------------------------------------------------------------------------------------------------

auto creator::visitInstruction(llvm::Instruction& instr) -> return_type
{
   return return_type();
}

//--------------------------------------------------------------------------------------------------

} // end namespace llvm_visible_instruction
} // end namespace concurrency_passes
