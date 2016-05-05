
#include "object.hpp"

namespace program_model
{
    Object::Object(std::string var, const int index)
    : mVar(var)
    , mIndex(index) { }
    
    bool Object::operator==(const Object& other) const {
        return
            mVar == other.mVar &&
            mIndex == other.mIndex;
    }
    
    const std::string& Object::var() const
    {
        return mVar;
    }
    
    int Object::index() const
    {
        return mIndex;
    }
	
	Object llvm_object(const char* gvar, const int index)
	{
		return Object(gvar, index);
	}
	
} // end namespace program_model
