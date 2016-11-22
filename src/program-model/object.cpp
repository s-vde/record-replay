
#include "object.hpp"

//--------------------------------------------------------------------------------------90

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   Object::Object(std::string var, const indices_t& indices)
   : mVar(var)
   , mIndices(indices) { }
   
   //-------------------------------------------------------------------------------------
    
   bool Object::operator==(const Object& other) const
   {
      return mVar == other.mVar && mIndices == other.mIndices;
   }
   
   //-------------------------------------------------------------------------------------
    
   const std::string& Object::var() const
   {
      return mVar;
   }
   
   //-------------------------------------------------------------------------------------
   
   void Object::add_index(int index)
   {
      mIndices.push_back(index);
   }
   
   //-------------------------------------------------------------------------------------
	
   Object llvm_object(const char* gvar)
   {
      return Object(gvar);
   }
   
   //-------------------------------------------------------------------------------------
	
} // end namespace program_model
