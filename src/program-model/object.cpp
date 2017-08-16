
#include "object.hpp"


namespace program_model {

//--------------------------------------------------------------------------------------------------

Object::Object(ptr_t address)
: m_address(address)
{
}

//--------------------------------------------------------------------------------------------------

bool Object::operator==(const Object& other) const
{
   return m_address == other.m_address;
}

//--------------------------------------------------------------------------------------------------

auto Object::address() const -> ptr_t
{
   return m_address;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
