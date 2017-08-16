#pragma once

#include <iosfwd> // forward declaration of std::ostream and std::istream

//--------------------------------------------------------------------------------------------------
/// @file instruction_io.hpp
/// @brief Input/output stream operator overloads for Instruction.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace program_model {

// Forward declarations
class Instruction;

std::ostream& operator<<(std::ostream&, const Instruction&);
std::istream& operator>>(std::istream&, Instruction&);

} // end namespace program_model
