
#include "object.hpp"

//--------------------------------------------------------------------------------------90

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   Object::Object(ptr_t address, std::string var, const indices_t& indices)
   : m_address(address)
   , mVar(var)
   , mIndices(indices)
   {
   }
   
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
	
   Object llvm_object(Object::ptr_t address, const char* gvar)
   {
      return Object(address, gvar);
   }
   
   //-------------------------------------------------------------------------------------
	
} // end namespace program_model
