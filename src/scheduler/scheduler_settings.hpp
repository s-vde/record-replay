#pragma once

// STL
#include <string>

//--------------------------------------------------------------------------------------90
/// @file scheduler_settings.hpp
/// @brief Definition of class SchedulerSettings.
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   class SchedulerSettings
   {
   public:
      
      //----------------------------------------------------------------------------------
        
      /// @brief Constructor.
      
      explicit SchedulerSettings(const std::string& strategy_tag="Random");
      
      //----------------------------------------------------------------------------------
        
      /// @brief Getter.
      
      const std::string& strategy_tag() const;
      
      //----------------------------------------------------------------------------------
        
      /// @note Function to initialize SchedulerSettings object in the initializer list of
      /// Scheduler.

      static SchedulerSettings read_from_file(const std::string& filename);
      
      //----------------------------------------------------------------------------------
        
   private:
        
      //----------------------------------------------------------------------------------
      
      /// @brief The tag corresponding to the SelectionStrategy to be used by the Scheduler.

      std::string mStrategyTag;
      
      //----------------------------------------------------------------------------------
        
   }; // end class SchedulerSettings
   
   //-------------------------------------------------------------------------------------
   
   std::ostream& operator<<(std::ostream&, const SchedulerSettings&);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
