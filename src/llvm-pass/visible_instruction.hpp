#pragma once

// PROGRAM_MODEL
#include "object.hpp"

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
    
} // end namespace record_replay
