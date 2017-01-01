#pragma once

// PROGRAM_MODEL
#include "object.hpp"

// LLVM
#include <llvm/IR/InstVisitor.h>

// BOOST
#include <boost/optional.hpp>

// STL
#include <deque>

//--------------------------------------------------------------------------------------90
/// @file visible_instruction.hpp
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
      
      //----------------------------------------------------------------------------------
      
      using indices_t = std::deque<llvm::Value*>;
      
      //----------------------------------------------------------------------------------
      
      /// @brief Constructor.
      
      shared_object(llvm::GlobalVariable* gvar, const indices_t& indices);
      
      //----------------------------------------------------------------------------------
      
      /// @brief Constructs a program_model::Object from this shared_object before
      /// llvm::Instruction before and returns a pointer to it.
      
      llvm::Value* construct_model(llvm::Module& module, llvm::Instruction* before) const;
      
      //----------------------------------------------------------------------------------
      
   private:
      
      //----------------------------------------------------------------------------------
      
      llvm::GlobalVariable* m_gvar;
      indices_t m_indices;
      
      //----------------------------------------------------------------------------------
      
   }; // end class shared_object
   
   //-------------------------------------------------------------------------------------
   
   using visible_operation_t = std::pair<program_model::Object::Op, llvm::Value*>;
   using visible_instruction_t = std::pair<program_model::Object::Op, shared_object>;
   using opt_visible_instruction_t = boost::optional<visible_instruction_t>;
   
   //-------------------------------------------------------------------------------------
    
   opt_visible_instruction_t get_visible_instruction(llvm::Instruction* instr);
   
   boost::optional<shared_object> get_shared_object(llvm::Value* operand);
   
   //-------------------------------------------------------------------------------------
   
   struct visible_operation_creator
   : public llvm::InstVisitor<visible_operation_creator, boost::optional<visible_operation_t>>
   {
      using return_type = boost::optional<visible_operation_t>;
      
      return_type visitLoadInst(llvm::LoadInst& instr);
      return_type visitStoreInst(llvm::StoreInst& instr);
      return_type visitAtomicCmpXchgInst(llvm::AtomicCmpXchgInst& instr);
      return_type visitCallInst(llvm::CallInst& instr);
      return_type visitInstruction(llvm::Instruction& instr);
   };
   
   //-------------------------------------------------------------------------------------

#define DELEGATE(CLASS_TO_VISIT) \
   return static_cast<SubClass*>(this)-> \
      visit##CLASS_TO_VISIT(static_cast<CLASS_TO_VISIT&>(value))
   
   template <typename SubClass, typename RetTy=void>
   struct ValueVisitor
   {
      RetTy visit(llvm::Value& value)
      {
         using namespace llvm;
         if (User* user = dyn_cast<User>(&value))
         {
            if (Constant* constant = dyn_cast<Constant>(&user))
            {
               if (GlobalValue* global_value = dyn_cast<GlobalValue>(&constant))
               {
                  if (GlobalObject* global_object = dyn_cast<GlobalObject>(&global_value))
                  {
                     if (GlobalVariable* global_variable = dyn_cast<GlobalVariable>(&global_object))
                     {
                        visitGlobalVariable(global_variable);
                     }
                     // > llvm::Function
                  }
                  // >llvm::GlobalAlias
               }
               // > ...
            }
            // > llvm::Instruction
            // > llvm::Operator
         }
      }
      
      RetTy visitGlobalVariable(llvm::GlobalVariable* value)
      {
         DELEGATE(llvm::GlobalObject);
      }
      
      // > llvm::Function
      
      //-----
      
      RetTy visitGlobalObject(llvm::GlobalObject* value)
      {
         
         DELEGATE(llvm::GlobalValue);
      }
      
      // > llvm::GlobalAlias
      
      //-----
      
      RetTy visitGlobalValue(llvm::GlobalValue* value)
      {
         DELEGATE(llvm::Constant);
      }
      
      // > ...
      
      //-----
      
      RetTy visitConstant(llvm::Constant* value)
      {
         DELEGATE(llvm::User);
      }
      
      RetTy visitInstruction(llvm::Instruction* value)
      {
         DELEGATE(llvm::User);
      }
      
      RetTy visitOperator(llvm::Operator* value)
      {
         DELEGATE(llvm::User);
      }
      
      //-----
      
      RetTy visitUser(llvm::User* value)
      {
         DELEGATE(llvm::Value);
      }
      
      // > llvm::Argument
      // > llvm::BasicBlock
      // > llvm::InlineAsm
      // > llvm::MetadataAsValue
   };
   
   //-------------------------------------------------------------------------------------
   
} // end namespace record_replay
