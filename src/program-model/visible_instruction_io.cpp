
#include "visible_instruction.hpp"

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const memory_operation& operation)
   {
      switch (operation)
      {
         case memory_operation::Load            : return "Load";
         case memory_operation::Store           : return "Store";
         case memory_operation::ReadModifyWrite : return "RWMW";
      }
   }
   
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const lock_operation& operation)
   {
      switch (operation)
      {
         case lock_operation::Lock              : return "Lock";
         case lock_operation::Unlock            : return "Unlock";
      }
   }
   
   //-------------------------------------------------------------------------------------

} // end namespace program_model
