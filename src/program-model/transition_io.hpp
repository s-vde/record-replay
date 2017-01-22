#pragma once

#include <string>

//--------------------------------------------------------------------------------------90
/// @file transition_io.hpp
/// @brief Input/output functions for Transition.
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   class Transition; // forward declaration
   
   //-------------------------------------------------------------------------------------
   
   std::string to_string_pre(const Transition& trans);
   std::string to_string_post(const Transition& trans);
   std::string to_string_pre_post(const Transition& trans);
   
   std::string to_short_string(const Transition& trans);
   
   std::string to_string_pre(const unsigned int t_i);
   std::string to_string_post(const unsigned int t_i);
   
   std::string to_string_pre_state(const Transition& trans);
   std::string to_string_post_state(const Transition& trans);
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
