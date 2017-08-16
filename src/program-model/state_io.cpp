
#include "state_io.hpp"

#include "instruction_io.hpp"
#include "state.hpp"

#include <container_io.hpp>

namespace program_model {

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, next_t& next)
{
   std::string str_enabled{};
   if (is >> next.instr >> str_enabled)
   {
      if (str_enabled == "enabled")
      {
         next.enabled = true;
      }
      else if (str_enabled == "disabled")
      {
         next.enabled = false;
      }
      else
      {
         is.setstate(std::ios::failbit);
      }
   }
   return is;
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const next_t& next)
{
   os << next.instr << " " << (next.enabled ? "enabled" : "disabled");
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, State& state)
{
   std::string tag{};
   if (is >> tag)
   {
      if (tag == "State")
      {
         is >> state.mEnabled >> state.mNext;
      }
      else
      {
         is.setstate(std::ios::failbit);
      }
   }
   return is;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, std::shared_ptr<State>& state)
{
   std::string tag{};
   if (is >> tag)
   {
      if (tag == "State")
      {
         Tids enabled{};
         NextSet next{};
         is >> enabled >> next;
         state = std::make_shared<State>(enabled, next);
      }
      else
      {
         is.setstate(std::ios::failbit);
      }
   }
   return is;
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const State& state)
{
   os << state.tag() << " " << state.enabled() << " " << state.mNext;
   return os;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
