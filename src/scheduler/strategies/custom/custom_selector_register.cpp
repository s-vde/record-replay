
#include "custom_selector_register.hpp"

#include <error.hpp>

#include "non_preemptive.hpp"


namespace scheduler {

//-------------------------------------------------------------------------------------

SelectorUniquePtr custom_selector_factory(const std::string& tag)
{
   ERROR("custom_selector_factory", "no selector registered under " << tag);
   return std::make_unique<Selector<NonPreemptive>>();
}

//-------------------------------------------------------------------------------------

} // end namespace scheduler
