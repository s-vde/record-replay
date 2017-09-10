
#include "object_io.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace program_model {

//--------------------------------------------------------------------------------------------------

std::string to_string(const Object& obj)
{
   return to_pretty_string(obj);
}

//--------------------------------------------------------------------------------------------------

std::string to_pretty_string(const Object& obj)
{
   std::stringstream stream;
   stream << std::hex << obj.m_address;
   return stream.str();
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Object& obj)
{
   os << std::hex << obj.m_address << std::dec;
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, Object& object)
{
   std::uintptr_t ptr;
   is >> std::hex >> ptr >> std::dec;
   object.m_address = (void*)ptr;
   return is;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
