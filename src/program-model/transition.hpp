#pragma once

// PROGRAM_MODEL
#include "instruction.hpp"

// STL
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file transition.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   template<typename State>
   class Transition
   {
   public:

      //----------------------------------------------------------------------------------
      
      using StatePtr = typename State::SharedPtr;
        
      //----------------------------------------------------------------------------------
      
      /// @brief Constructor
      
      Transition(const int index, StatePtr pre, const Instruction& instr, StatePtr post)
      : mIndex(index)
      , mPre(pre)
      , mInstr(instr)
      , mPost(post) { }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      int index() const
      {
         return mIndex;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      const Instruction& instr() const
      {
         return mInstr;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      StatePtr pre_ptr()
      {
         return mPre;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      const State& pre() const
      {
         return *mPre;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      State& pre()
      {
         return *mPre;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      StatePtr post_ptr()
      {
         return mPost;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      const State& post() const
      {
         return *mPost;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      State& post()
      {
         return *mPost;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Setter.
        
      void set_pre(const StatePtr& pre)
      {
         mPre = pre;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Setter.
        
      void set_post(const StatePtr& post)
      {
         mPost = post;
      }
      
      //----------------------------------------------------------------------------------
        
   private:
        
      //----------------------------------------------------------------------------------
        
      int mIndex;
      StatePtr mPre;
      const Instruction mInstr;
      StatePtr mPost;
      
      //----------------------------------------------------------------------------------
        
   }; // end class template Transition<State>
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
