
#include "transition_io.hpp"

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string_pre(const unsigned int t_i)
   {
      std::string str = "[s";
      str += std::to_string(t_i-1);
      str += "]";
      return str;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string to_string_post(const unsigned int t_i)
   {
      std::string str = "[s";
      str += std::to_string(t_i);
      str += "]";
      return str;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
