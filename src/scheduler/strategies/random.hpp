#pragma once

#include "task_pool.hpp"

#include <execution.hpp>
#include <state.hpp>

//--------------------------------------------------------------------------------------------------
/// @file random.hpp
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace scheduler {

class Random
{
public:
   using Status = program_model::Execution::Status;
   using result_t = std::pair<Status, program_model::Thread::tid_t>;

   Random() = default;

   result_t select(const TaskPool& pool, const program_model::Tids& selection,
                   const unsigned int task_nr) const;

}; // end class Random

} // end namespace scheduler
