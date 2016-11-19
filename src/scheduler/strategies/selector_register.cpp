
#include "selector_register.hpp"

// SCHEDULER
#include "custom_selector_register.hpp"
#include "non_preemptive.hpp"
#include "random.hpp"

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   SelectorUniquePtr selector_factory(const std::string& tag)
   {
      if (tag == "NonPreemptive") { return std::make_unique<Selector<NonPreemptive>>();   }
      else if (tag == "Random")   { return std::make_unique<Selector<Random>>();          }
      else                        { return custom_selector_factory(tag);                  }
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
