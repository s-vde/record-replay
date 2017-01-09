
#pragma once

// STL
#include <string>

//--------------------------------------------------------------------------------------90
/// @file object.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   class Object
   {
   public:
      
      using ptr_t = void*;
      
      enum class Op { READ = 0, WRITE = 1, RMW = 5, UNLOCK = 2, LOCK = 3 };
        
      /// @brief Constructor.
      /// @note Default arguments construct dummy Object.
      
      Object(ptr_t address=nullptr);
        
      bool operator==(const Object&) const;
      
      /// @brief Getter.
      
      ptr_t address() const;
        
   private:
      
      ptr_t m_address;
      
      //----------------------------------------------------------------------------------
        
      friend std::string to_pretty_string(const Object&);
      friend std::ostream& operator<<(std::ostream& os, const Object& obj);
      friend std::istream& operator>>(std::istream&, Object&);
      
      //----------------------------------------------------------------------------------
        
   }; // end class Object
   
   //-------------------------------------------------------------------------------------
	
} // end namespace program_model
