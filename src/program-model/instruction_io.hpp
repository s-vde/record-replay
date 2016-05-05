
#ifndef INSTRUCTION_IO_HPP_INCLUDED
#define INSTRUCTION_IO_HPP_INCLUDED

#include <iosfwd> 		// forward declaration of std::ostream and std::istream

/*---------------------------------------------------------------------------75*/
/**
 @file instruction_io.hpp
 @brief Declaration of input/output stream operator overloads for Instruction.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
	// forward declarations
	class Instruction;
	
    std::ostream& operator<<(std::ostream&, const Instruction&);
    
    std::istream& operator>>(std::istream&, Instruction&);

} // end namespace program_model

#endif
