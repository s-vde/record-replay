
#include "random.hpp"
#include <assert.h>
#include "debug.hpp"

namespace scheduler
{
    Random::result_t Random::select(
        const TaskPool& pool,
        const Tids& selection,
        const unsigned int task_nr) const
    {
        /// @pre !selection.empty
        assert(!selection.empty());
        srand(time(NULL));
        Tids::iterator it = selection.begin();
        int index = rand() % selection.size();
        std::advance(it, index);
        DEBUGFNL("Random", "select", "", "selection[" << index << "] = " << *it);
        return result_t(Status::RUNNING, *it);
    }
} // end namespace scheduler
