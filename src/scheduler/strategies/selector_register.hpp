
#ifndef SELECTOR_REGISTER_HPP_INCLUDED
#define SELECTOR_REGISTER_HPP_INCLUDED

#include "selector.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file non_preemptive.hpp
 @brief Declaration of function selector_factory.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace scheduler
{
    using SelectorUniquePtr = std::unique_ptr<SelectorBase>;
    
    SelectorUniquePtr selector_factory(const std::string& tag);
    
} // end namespace scheduler

#endif
