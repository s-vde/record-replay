
#include "concurrency_error.hpp"

// PROGRAM_MODEL
#include "instruction_io.hpp"
#include "visible_instruction_io.hpp"

// STL
#include <ostream>
#include <sstream>

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

std::ostream& write_to_stream(std::ostream& os, const deadlock_t& deadlock)
{
   os << "Deadlock:\n";
   for (const auto& instruction : deadlock)
   {
      os << "-----\n";
      os << instruction << "\n" << instruction.meta_data() << "\n";
   }
   os << "-----\n";
   return os;
}

//--------------------------------------------------------------------------------------------------

deadlock_exception::deadlock_exception(const deadlock_t& deadlocks) 
: m_deadlock(deadlocks) {}

//--------------------------------------------------------------------------------------------------
   
const deadlock_t deadlock_exception::get() const 
{
   return m_deadlock;
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
