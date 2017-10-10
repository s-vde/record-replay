
#include "transition_io.hpp"

#include "state_io.hpp"
#include "transition.hpp"
#include "visible_instruction_io.hpp"


namespace program_model {

//--------------------------------------------------------------------------------------------------

std::string to_string_pre(const Transition& trans)
{
   std::stringstream stream;
   stream << trans.pre() << "\n" << trans.index() << " " << trans.instr();
   return stream.str();
}

//--------------------------------------------------------------------------------------------------

std::string to_string_post(const Transition& trans)
{
   std::stringstream stream;
   stream << trans.index() << " " << trans.instr() << "\n" << trans.post();
   return stream.str();
}

//--------------------------------------------------------------------------------------------------

std::string to_string_pre_post(const Transition& trans)
{
   std::stringstream stream;
   stream << to_string_pre(trans) << "\n" << trans.post();
   return stream.str();
}

//--------------------------------------------------------------------------------------------------

std::string to_short_string(const Transition& trans)
{
   std::stringstream stream;
   stream << trans.index() << " "
          << boost::apply_visitor(instruction_to_short_string(), trans.instr());
   return stream.str();
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
