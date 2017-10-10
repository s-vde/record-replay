#pragma once

#include "object.hpp"

#include <iosfwd> // forward declaration of std::ostream and std::istream

//--------------------------------------------------------------------------------------------------
/// @file object_io.hpp
/// @brief Input/output stream operator and to_string overloads for Object.
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

std::ostream& operator<<(std::ostream&, const Object&);
std::istream& operator>>(std::istream&, Object&);

} // end namespace program_model
