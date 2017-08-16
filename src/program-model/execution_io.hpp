#pragma once

#include "execution.hpp"

#include <iostream>

//--------------------------------------------------------------------------------------------------
/// @file execution_io.hpp
/// @brief Input/output functions for Execution.
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

std::string to_string(const Execution::Status&);
std::ostream& operator<<(std::ostream&, const Execution::Status&);
std::istream& operator>>(std::istream&, Execution::Status&);

std::string to_short_string(const Execution& E);
std::ostream& operator<<(std::ostream& os, const Execution& E);

/// @brief Extends E with Transitions read from the given std::istream is, skipping the
/// first E.size() read Transitions.

std::istream& operator>>(std::istream& is, Execution& E);

} // end namespace program_model
