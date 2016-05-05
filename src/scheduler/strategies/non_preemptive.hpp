
#ifndef NON_PREEMPTIVE_HPP_INCLUDED
#define NON_PREEMPTIVE_HPP_INCLUDED

#include "execution.hpp"
#include "state.hpp"
#include "task_pool.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file non_preemptive.hpp
 @brief Declaration of class NonPreemptive.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;

namespace scheduler
{
    class NonPreemptive
    {
    public:
        
        // TYPES
        
        using Status = program_model::Execution<program_model::State>::Status;
        using result_t = std::pair<Execution<State>::Status,Thread::tid_t>;
        
        // LIFETIME
        
        NonPreemptive() = default;
        
        //
        
        result_t select(
            const TaskPool& pool,
            const Tids& selection,
            const unsigned int task_nr
        ) const;
        
    }; // end class NonPreemptive
} // end namespace scheduler

#endif
