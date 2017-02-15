#pragma once

// PROGRAM_MODEL
#include "instruction.hpp"

// STL
#include <iosfwd>

//-----------------------------------------------------------------------------------------------100
/// @file data_race.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

namespace scheduler
{
//--------------------------------------------------------------------------------------------------

using data_race_t = std::pair<program_model::Instruction, program_model::Instruction>;

std::ostream& write_to_stream(std::ostream& os, const data_race_t& data_race);

//--------------------------------------------------------------------------------------------------    
} // end namespace scheduler
