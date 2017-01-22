
#include "schedule.hpp"

// PROGRAM_MODEL
#include <execution.hpp>

namespace scheduler
{
   //-------------------------------------------------------------------------------------

   schedule_t schedule(const program_model::Execution& E)
   {
      schedule_t s;
      std::transform(E.begin(), E.end(), std::back_inserter(s),
                     [] (const auto& trans) { return trans.instr().tid(); });
      return s;
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
