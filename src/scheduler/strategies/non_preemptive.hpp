#pragma once

#include "task_pool.hpp"

#include <execution.hpp>
#include <state.hpp>

//--------------------------------------------------------------------------------------------------
/// @file non_preemptive.hpp
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace scheduler {

class NonPreemptive
{
public:
   using Status = program_model::Execution::Status;
   using result_t = std::pair<Status, program_model::Thread::tid_t>;

   NonPreemptive() = default;

   result_t select(const TaskPool& pool, const program_model::Tids& selection,
                   const unsigned int task_nr) const;

}; // end class NonPreemptive

} // end namespace scheduler
