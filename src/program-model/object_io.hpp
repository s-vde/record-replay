#pragma once

#include "object.hpp"

// STL
#include <iosfwd> 	// forward declaration of std::ostream and std::istream

//--------------------------------------------------------------------------------------90
/// @file object_io.hpp
/// @brief Input/output stream operator and to_string overloads for Object.
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const Object::Op&);
   std::ostream& operator<<(std::ostream&, const Object::Op&);
   std::istream& operator>>(std::istream&, Object::Op&);
   
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const Object&);
   std::string to_pretty_string(const Object&);
   std::ostream& operator<<(std::ostream&, const Object&);
   std::istream& operator>>(std::istream&, Object&);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace program_model
