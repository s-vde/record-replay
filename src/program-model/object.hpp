
#pragma once

// STL
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file object.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   class Object
   {
   public:
      
      using indices_t = std::vector<int>;
      
      //----------------------------------------------------------------------------------
      
      enum class Op { READ = 0, WRITE = 1, LOCK = 2, UNLOCK = 3 };
      
      //----------------------------------------------------------------------------------
        
      /// @brief Constructor.
      /// @note Default arguments construct dummy Object.
      
      explicit Object(std::string gvar="", const indices_t& indices={});
      
      //----------------------------------------------------------------------------------
        
      bool operator==(const Object&) const;
        
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      const std::string& var() const;
      
      //----------------------------------------------------------------------------------
      
      void add_index(int index);
      
      //----------------------------------------------------------------------------------
        
   private:
        
      //----------------------------------------------------------------------------------
      
      std::string mVar;
      indices_t mIndices;
      
      //----------------------------------------------------------------------------------
        
      friend std::string to_pretty_string(const Object&);
      friend std::ostream& operator<<(std::ostream& os, const Object& obj);
      friend std::istream& operator>>(std::istream&, Object&);
      
      //----------------------------------------------------------------------------------
        
   }; // end class Object
   
   //-------------------------------------------------------------------------------------
	
	Object llvm_object(const char* gvar, const int index=0);
   
   //-------------------------------------------------------------------------------------
	
} // end namespace program_model
