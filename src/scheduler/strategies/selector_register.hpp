#pragma once

// SCHEDULER
#include "selector.hpp"

//--------------------------------------------------------------------------------------90
/// @file non_preemptive.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   using SelectorUniquePtr = std::unique_ptr<SelectorBase>;
   
   //-------------------------------------------------------------------------------------
    
   SelectorUniquePtr selector_factory(const std::string& tag);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
