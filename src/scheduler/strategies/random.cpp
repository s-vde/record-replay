
#include "random.hpp"

#include <debug.hpp>

#include <assert.h>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

Random::result_t Random::select(const TaskPool& pool, const program_model::Tids& selection,
                                const unsigned int task_nr) const
{
   /// @pre !selection.empty
   assert(!selection.empty());
   srand(time(NULL));
   program_model::Tids::iterator it = selection.begin();
   int index = rand() % selection.size();
   std::advance(it, index);
   DEBUGF_SYNC("Random", "select", "", "selection[" << index << "] = " << *it);
   return result_t(Status::RUNNING, *it);
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
