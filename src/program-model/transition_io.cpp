
#include "transition_io.hpp"

#include "instruction_io.hpp"
#include "state_io.hpp"
#include "transition.hpp"

#include <utils_io.hpp>


namespace program_model {

//--------------------------------------------------------------------------------------------------

std::string to_string_pre(const Transition& trans)
{
   std::string str = utils::io::to_string(trans.pre());
   str += "\n";
   str += to_short_string(trans);
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string_post(const Transition& trans)
{
   std::string str = to_short_string(trans);
   str += "\n";
   str += utils::io::to_string(trans.post());
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string_pre_post(const Transition& trans)
{
   std::string str = to_string_pre(trans);
   str += "\n";
   str += utils::io::to_string(trans.post());
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_short_string(const Transition& trans)
{
   std::string str = std::to_string(trans.index());
   str += " ";
   str += utils::io::to_string(trans.instr());
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string_pre(const unsigned int t_i)
{
   std::string str = "[s";
   str += std::to_string(t_i - 1);
   str += "]";
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string_post(const unsigned int t_i)
{
   std::string str = "[s";
   str += std::to_string(t_i);
   str += "]";
   return str;
}

//--------------------------------------------------------------------------------------------------

std::string to_string_pre_state(const Transition& trans)
{
   return to_string_pre(trans.index());
}

//--------------------------------------------------------------------------------------------------

std::string to_string_post_state(const Transition& trans)
{
   return to_string_post(trans.index());
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
