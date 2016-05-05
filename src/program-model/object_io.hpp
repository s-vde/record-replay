
#ifndef OBJECT_IO_HPP_INCLUDED
#define OBJECT_IO_HPP_INCLUDED

#include <iosfwd> 	// forward declaration of std::ostream and std::istream
#include "object.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file object_io.hpp
 @brief Declaration of input/output stream operator and to_string overloads 
 for Instruction.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    std::string to_string(const Object::Op&);
    std::ostream& operator<<(std::ostream&, const Object::Op&);
    std::istream& operator>>(std::istream&, Object::Op&);
    
    std::string to_pretty_string(const Object&);
    std::ostream& operator<<(std::ostream&, const Object&);
    std::istream& operator>>(std::istream&, Object&);
    
} // end namespace program_model

#endif
