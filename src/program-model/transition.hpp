#pragma once

#include "instruction.hpp"
#include "state.hpp"

#include <vector>

//--------------------------------------------------------------------------------------------------
/// @file transition.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

class Transition
{
public:
   using StatePtr = typename State::SharedPtr;

   /// @brief Constructor

   Transition(const int index, StatePtr pre, const Instruction& instr, StatePtr post);

   /// @brief Getter.

   int index() const;

   /// @brief Getter.

   const Instruction& instr() const;

   /// @brief Getter.

   StatePtr pre_ptr();

   /// @brief Getter.

   const State& pre() const;

   /// @brief Getter.

   State& pre();

   /// @brief Getter.

   StatePtr post_ptr();

   /// @brief Getter.

   const State& post() const;

   /// @brief Getter.

   State& post();

   /// @brief Setter.

   void set_pre(const StatePtr& pre);

   /// @brief Setter.

   void set_post(const StatePtr& post);

private:
   int mIndex;
   StatePtr mPre;
   const Instruction mInstr;
   StatePtr mPost;

}; // end class Transition

} // end namespace program_model
