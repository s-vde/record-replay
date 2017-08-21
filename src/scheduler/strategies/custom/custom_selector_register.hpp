#pragma once

#include "selector.hpp"


namespace scheduler {

using SelectorUniquePtr = std::unique_ptr<SelectorBase>;

SelectorUniquePtr custom_selector_factory(const std::string& tag);

} // end namespace scheduler
