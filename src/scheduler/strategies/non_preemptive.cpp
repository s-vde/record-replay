
#include "non_preemptive.hpp"

#include <assert.h>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

NonPreemptive::result_t NonPreemptive::select(const TaskPool& pool,
                                              const program_model::Tids& selection,
                                              const unsigned int task_nr) const
{
   /// @pre !selection.empty
   assert(!selection.empty());
   program_model::Thread::tid_t next = *(selection.begin());
   if (task_nr > 0)
   {
      const auto current = pool.current_task();
      const auto tid = boost::apply_visitor(program_model::get_tid(), *current);
      /// @pre task_nr > 0 -> pool.current_task != nullptr
      assert(current != nullptr);
      if (selection.find(tid) != selection.end())
      {
         next = tid;
      }
   }
   return result_t(Status::RUNNING, next);
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
