#pragma once

#include "thread.hpp"

#include <iosfwd> // forward declaration of std::ostream and std::istream

//--------------------------------------------------------------------------------------------------
/// @file thread_io.hpp
/// @brief Declaration of input/output stream operator overloads for Thread.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace program_model {

std::string to_string(const Thread::Status&);
std::ostream& operator<<(std::ostream&, const Thread::Status&);
std::istream& operator>>(std::istream&, Thread::Status&);

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream&, const Thread&);
std::istream& operator>>(std::istream&, Thread&);

} // end namespace program_model
