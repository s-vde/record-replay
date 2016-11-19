#pragma once

// PROGRAM_MODEL
#include "execution.hpp"
#include "state.hpp"

// SCHEDULER
#include "task_pool.hpp"

//--------------------------------------------------------------------------------------90
/// @file non_preemptive.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

using namespace program_model;

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   class NonPreemptive
   {
   public:
       
      //----------------------------------------------------------------------------------

      using Status = program_model::Execution<program_model::State>::Status;
      using result_t = std::pair<Execution<State>::Status,Thread::tid_t>;
      
      //----------------------------------------------------------------------------------
      
      NonPreemptive() = default;
      
      //----------------------------------------------------------------------------------
        
      result_t select(const TaskPool& pool,
                      const Tids& selection,
                      const unsigned int task_nr) const;
      
      //----------------------------------------------------------------------------------
        
   }; // end class NonPreemptive
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
