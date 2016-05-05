
#ifndef SCHEDULER_SETTINGS_HPP_INCLUDED
#define SCHEDULER_SETTINGS_HPP_INCLUDED

#include <string>

/*---------------------------------------------------------------------------75*/
/**
 @file scheduler_settings.hpp
 @brief Definition of class SchedulerSettings.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace scheduler
{
    class SchedulerSettings
    {
    public:
        
        // CTORS / DTOR
        
        explicit SchedulerSettings(const std::string& strategy_tag="Random");
        
        SchedulerSettings(const SchedulerSettings&) = default;
        SchedulerSettings(SchedulerSettings&&) = default;
        ~SchedulerSettings() = default;
        
        // OPERATORS
        
        SchedulerSettings& operator=(const SchedulerSettings&) = default;
        SchedulerSettings& operator=(SchedulerSettings&&) = default;
        
        //
        
        const std::string& strategy_tag() const;
        
        /**
         @note Function to initialize SchedulerSettings object in the
         initializer list of Scheduler.
         */
        static SchedulerSettings read_from_file(const std::string& filename);
        
    private:
        
        /**
         @brief The tag corresponding to the SelectionStrategy to be 
         used by the Scheduler.
         */
        std::string mStrategyTag;
        
    }; // end class SchedulerSettings
    
    std::ostream& operator<<(std::ostream&, const SchedulerSettings&);
    
} // end namespace scheduler

#endif
