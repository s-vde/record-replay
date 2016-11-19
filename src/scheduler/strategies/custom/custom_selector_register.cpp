
#include "custom_selector_register.hpp"

// UTILS
#include "error.hpp"

// SCHEDULER
#include "non_preemptive.hpp"

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   SelectorUniquePtr custom_selector_factory(const std::string& tag)
   {
      ERROR("custom_selector_factory", "no selector registered under " << tag);
      return std::make_unique<Selector<NonPreemptive>>();
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
