#pragma once

// PROGRAM_MODEL
#include "object.hpp"

// BOOST
#include <boost/optional.hpp>

// LLVM
#include "llvm/IR/Module.h"

//--------------------------------------------------------------------------------------90
/// @file visible_instruction.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace record_replay
{
   //-------------------------------------------------------------------------------------
   
   using Op = program_model::Object::Op;
   
   //-------------------------------------------------------------------------------------
   
   class VisibleInstruction
   {
   public:

      //----------------------------------------------------------------------------------
      
      /// @brief Constructor.
      
      explicit VisibleInstruction(const Op& op=Op::READ,
                                  llvm::GlobalVariable* gvar=nullptr,
                                  llvm::Value* index=nullptr);
      
      //----------------------------------------------------------------------------------
        
      /// @brief Getter.
      
      const Op& op() const;
      
      //----------------------------------------------------------------------------------
      
      /// @brief Adds the construction of a program_model::Object corresponding to this
      /// VisibleInstruction after I.
      
      llvm::Value* object(llvm::Module& M, llvm::Instruction* I) const;
      
      //----------------------------------------------------------------------------------
      
   private:

      //----------------------------------------------------------------------------------
        
      Op mOp;
      llvm::GlobalVariable* mGVar;
      llvm::Value* mIndex;
      
      //----------------------------------------------------------------------------------
        
   }; // end class VisibleInstruction
   
   //-------------------------------------------------------------------------------------
    
   boost::optional<VisibleInstruction> get_visible(llvm::Instruction* I);
    
   boost::optional<VisibleInstruction> get_visible(const Op& op, llvm::Value* operand);
    
   boost::optional<VisibleInstruction> get_visible(const Op& op,
                                                   llvm::Value* ptrop,
                                                   llvm::Value* indexop1,
                                                   llvm::Value* indexop2);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace record_replay
