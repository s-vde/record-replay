
#include "visible_instruction.hpp"

// STL
#include <iostream>

namespace program_model
{
//--------------------------------------------------------------------------------------------------

std::ostream& operator << (std::ostream& os, const meta_data_t& meta_data)
{
   os << "file_name=" << meta_data.file_name 
      << "\nline_number=" << meta_data.line_number;
   return os;
}

//--------------------------------------------------------------------------------------------------

   std::string to_string(const memory_operation& operation)
   {
      switch (operation)
      {
         case memory_operation::Load            : return "Load";
         case memory_operation::Store           : return "Store";
         case memory_operation::ReadModifyWrite : return "RMW";
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
