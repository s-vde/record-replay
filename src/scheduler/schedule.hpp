#pragma once

// PROGRAM_MODEL
#include "execution.hpp"

// STL
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file schedule.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   using schedule_t = std::vector<program_model::Thread::tid_t>;
   
   //-------------------------------------------------------------------------------------
    
   template<typename State>
   schedule_t schedule(const program_model::Execution<State>& E)
   {
      schedule_t s{};
      std::transform(E.begin(), E.end(), std::back_inserter(s),
                     [] (const auto& trans) { return trans.instr().tid(); });
      return s;
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
