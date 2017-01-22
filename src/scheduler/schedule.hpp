#pragma once

// PROGRAM_MODEL
#include <thread.hpp>

// STL
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file schedule.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   class Execution;
   
} // end namespace program_model

//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   using schedule_t = std::vector<program_model::Thread::tid_t>;
   
   schedule_t schedule(const program_model::Execution& E);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
