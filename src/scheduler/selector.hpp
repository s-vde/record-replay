#pragma once

// SCHEDULER
#include "schedule.hpp"
#include "task_pool.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "state.hpp"

// UTILS
#include "debug.hpp"

using namespace program_model;

//--------------------------------------------------------------------------------------90
/// @file selector.hpp
/// @brief Definition of class SelectorBase and class template Selector<Strategy>.
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   class SelectorBase
   {
   public:
      
      //----------------------------------------------------------------------------------
      
      using Status = Execution::Status;
      using result_t = std::pair<Execution::Status,Thread::tid_t>;
      
      //----------------------------------------------------------------------------------
      
      virtual ~SelectorBase() {};
      
      //----------------------------------------------------------------------------------
        
      virtual result_t select(TaskPool&, const schedule_t&, const unsigned int task_nr) = 0;
      
      //----------------------------------------------------------------------------------
        
   }; // end class SelectorBase
   
   //-------------------------------------------------------------------------------------

   /// @brief Class template implementing the selection of a next thread given a TaskPool,
   /// a schedule and a task_nr, based on the argument to the Strategy parameter.
   /// @details Implemented as a class template with template parameter Strategy instead
   /// of a base class that Strategy inherits from, because Selector<Strategy>::select can
   /// first run Strategy independent code before potentially delegating the selection to
   /// Strategy. Even if the user has access to Selector<Strategy> through a SelectorBase
   /// pointer, it still can access the Strategy only through the Selector interface.

   template <class Strategy>
   class Selector : public SelectorBase, private Strategy
   {
   public:
      
      //----------------------------------------------------------------------------------
      
      /// @brief Constructor.
        
      Selector()
      : SelectorBase()
      , Strategy() { }
      
      //----------------------------------------------------------------------------------
		
		/// @details First tries to select the next Thread::tid_t based on the given
      /// schedule. If (task_nr > schedule.size()) and there is no deadlock, the selection
      /// task is delegated to Strategy.

      result_t select(TaskPool& pool, const schedule_t& schedule, const unsigned int task_nr) override
      {
         if (task_nr < schedule.size())
         {
            DEBUGFNL("Selector", "select", "", "schedule[" << task_nr << "] = " << schedule[task_nr]);
            return result_t(Status::RUNNING, schedule[task_nr]);
         }
         std::lock_guard<std::mutex> guard(pool.mMutex);
         if (pool.size() > 0)
         {
            const Tids enabled = pool.enabled_set();
            if (!enabled.empty())
            {
               return Strategy::select(pool, enabled, task_nr);
            }
            return result_t(Status::DEADLOCK, -1);
         }
         return result_t(Status::DONE, -1);
      }
      
      //----------------------------------------------------------------------------------
      
   }; // end class Selector
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
