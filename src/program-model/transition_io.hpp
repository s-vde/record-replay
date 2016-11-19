#pragma once

// PROGRAM_MODEL
#include "instruction_io.hpp"
#include "transition.hpp"

// UTILS
#include "utils_io.hpp"

//--------------------------------------------------------------------------------------90
/// @file transition_io.hpp
/// @brief Input/output functions for Transition<State>.
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   template<typename State>
   std::string to_string_pre(const Transition<State>& trans)
   {
      std::string str = to_string(trans.pre());
      str += "\n";
      str += to_short_string(trans);
      return str;
   }
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   std::string to_string_post(const Transition<State>& trans)
   {
      std::string str = to_short_string(trans);
      str += "\n";
      str += utils::io::to_string(trans.post());
      return str;
   }
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   std::string to_string_pre_post(const Transition<State>& trans)
   {
      std::string str = to_string_pre(trans);
      str += "\n";
      str += utils::io::to_string(trans.post());
      return str;
   }
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   std::string to_short_string(const Transition<State>& trans)
   {
      std::string str = std::to_string(trans.index());
      str += " ";
      str += utils::io::to_string(trans.instr());
      return str;
   }
   
   //-------------------------------------------------------------------------------------

   std::string to_string_pre(const unsigned int t_i);
   
   //-------------------------------------------------------------------------------------
    
   std::string to_string_post(const unsigned int t_i);
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   std::string to_string_pre_state(const Transition<State>& trans)
   {
      return to_string_pre(trans.index());
   }
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   std::string to_string_post_state(const Transition<State>& trans)
   {
      return to_string_post(trans.index());
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
