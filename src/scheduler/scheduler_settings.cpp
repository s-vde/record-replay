
#include "scheduler_settings.hpp"
#include <fstream>
#include <iostream>
#include "error.hpp"

namespace scheduler
{
    SchedulerSettings::SchedulerSettings(const std::string& strategy_tag)
    : mStrategyTag(strategy_tag) { }
    
    const std::string& SchedulerSettings::strategy_tag() const
    {
        return mStrategyTag;
    }
    
    SchedulerSettings SchedulerSettings::read_from_file(const std::string& filename)
    {
        std::string strategy_tag = "Random";
        std::ifstream ifs(filename);
        if (ifs >> strategy_tag) {}
        else { ERROR("SchedulerSettings", "reading settings from " << filename); }
        ifs.close();
        return SchedulerSettings(strategy_tag);
    }
    
    std::ostream& operator<<(std::ostream& os, const SchedulerSettings& settings)
    {
        os << settings.strategy_tag();
        return os;
    }
} // end namespace scheduler
