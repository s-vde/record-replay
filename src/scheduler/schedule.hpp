
#ifndef SCHEDULE_HPP_INCLUDED
#define SCHEDULE_HPP_INCLUDED

#include <vector>
#include "execution.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file schedule.hpp
 @brief Contains type alias schedule_t and definition of a free function
 related to schedules.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace scheduler
{
    using schedule_t = std::vector<program_model::Thread::tid_t>;
    
    template<typename State>
    schedule_t schedule(const program_model::Execution<State>& E)
    {
        schedule_t s{};
        std::transform(
            E.begin(), E.end(), std::back_inserter(s),
            [] (const auto& trans) { return trans.instr().tid(); }
        );
        return s;
    }
    
} // end namespace scheduler

#endif
