
#include "data_race.hpp"

// PROGRAM_MODEL
#include "instruction_io.hpp"
#include "visible_instruction_io.hpp"

// STL
#include <ostream>

namespace scheduler
{
//--------------------------------------------------------------------------------------------------

std::ostream& write_to_stream(std::ostream& os, const data_race_t& data_race)
{
   os << "Datarace:\n";
   os << "-----\n";
   os << data_race.first << "\n" << data_race.first.meta_data() << "\n";
   os << "-----\n";
   os << data_race.second << "\n" << data_race.second.meta_data() << "\n";
   os << "-----\n";
   return os;
}

//--------------------------------------------------------------------------------------------------
} // end namespace scheduler
