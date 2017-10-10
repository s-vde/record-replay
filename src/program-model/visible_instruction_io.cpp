
#include "visible_instruction_io.hpp"


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


std::ostream& operator<<(std::ostream& os, const memory_operation& operation)
{
   switch (operation)
   {
      case memory_operation::Load:
         os << "Load";
         break;
      case memory_operation::Store:
         os << "Store";
         break;
      case memory_operation::ReadModifyWrite:
         os << "RMW";
         break;
   }
   return os;
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


std::ostream& operator<<(std::ostream& os, const lock_operation& operation)
{
   switch (operation)
   {
      case lock_operation::Lock:
         os << "Lock";
         break;
      case lock_operation::Unlock:
         os << "Unlock";
         break;
   }
   return os;
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


std::ostream& operator<<(std::ostream& os, const thread_management_operation& operation)
{
   switch (operation)
   {
      case thread_management_operation::Spawn:
         os << "Spawn";
         break;
      case thread_management_operation::Join:
         os << "Join";
         break;
   }
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, thread_management_operation& operation)
{
   std::string str;
   is >> str;
   if (str == "Spawn")
      operation = thread_management_operation::Spawn;
   else if (str == "Join")
      operation = thread_management_operation::Join;
   else
      is.setstate(std::ios::failbit);
   return is;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
