#pragma once

// STL
#include <string>

//--------------------------------------------------------------------------------------90
/// @file program.hpp
/// @brief Definition of class Program.
/// @date 2015
//----------------------------------------------------------------------------------------

namespace scheduler
{
   
   //-------------------------------------------------------------------------------------
   
   /// @brief Model of an input program to the Scheduler.

   class Program
   {
   public:
      
      //----------------------------------------------------------------------------------
      
      /// @brief Constructor.
      
      Program(const std::string& dir, const std::string& file);
        
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      const std::string& file() const;
      
      //----------------------------------------------------------------------------------
        
      std::string to_string() const;
      
      //----------------------------------------------------------------------------------
        
   private:
      
      //----------------------------------------------------------------------------------
		
      /// @brief The directory containing the program.
      std::string mDir;
		
      /// @brief The filename of the program.
      std::string mFile;
        
   }; // end class Program
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
