
#include "visible_instruction.hpp"

#include <iostream>


namespace program_model {

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const meta_data_t& meta_data)
{
   os << meta_data.file_name << " " << meta_data.line_number;
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, meta_data_t& meta_data)
{
   is >> meta_data.file_name >> meta_data.line_number;
   return is;
}

//--------------------------------------------------------------------------------------------------

std::string to_string(const memory_operation& operation)
{
   switch (operation)
   {
      case memory_operation::Load:
         return "Load";
      case memory_operation::Store:
         return "Store";
      case memory_operation::ReadModifyWrite:
         return "RMW";
   }
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, memory_operation& operation)
{
   std::string str;
   is >> str;
   if (str == "Load")
      operation = memory_operation::Load;
   else if (str == "Store")
      operation = memory_operation::Store;
   else if (str == "RMW")
      operation = memory_operation::ReadModifyWrite;
   else
      is.setstate(std::ios::failbit);
   return is;
}

//--------------------------------------------------------------------------------------------------

std::string to_string(const lock_operation& operation)
{
   switch (operation)
   {
      case lock_operation::Lock:
         return "Lock";
      case lock_operation::Unlock:
         return "Unlock";
   }
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, lock_operation& operation)
{
   std::string str;
   is >> str;
   if (str == "Lock")
      operation = lock_operation::Lock;
   else if (str == "Unlock")
      operation = lock_operation::Unlock;
   else
      is.setstate(std::ios::failbit);
   return is;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
