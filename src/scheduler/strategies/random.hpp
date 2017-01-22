#pragma once

// SCHEDULER
#include "task_pool.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "state.hpp"

//--------------------------------------------------------------------------------------90
/// @file random.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

using namespace program_model;

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   class Random
   {
   public:
      
      //----------------------------------------------------------------------------------
      
      using Status = program_model::Execution::Status;
      using result_t = std::pair<Execution::Status,Thread::tid_t>;
      
      //----------------------------------------------------------------------------------

      Random() = default;
      
      //----------------------------------------------------------------------------------
        
      result_t select(const TaskPool& pool,
                      const Tids& selection,
                      const unsigned int task_nr) const;
      
      //----------------------------------------------------------------------------------
        
   }; // end class Random
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
