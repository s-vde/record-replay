
#ifndef OBJECT_HPP_INCLUDED
#define OBJECT_HPP_INCLUDED

#include <string>

/*---------------------------------------------------------------------------75*/
/**
 @file object.hpp
 @brief Definition of class Object.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    class Object
    {
    public:
        
        enum class Op {
            READ = 0,
            WRITE = 1,
            LOCK = 2,
            UNLOCK = 3
        };
        
        // CTORS / DTOR
        
        /**
         @note Default arguments construct dummy Object.
         */
        explicit Object(std::string gvar="", const int index=0);
        
        Object(const Object&) = default;
        Object(Object&&) = default;
        ~Object() = default;
        
        // OPERATORS
        
        Object& operator=(const Object&) = default;
        Object& operator=(Object&&) = default;
        
        bool operator==(const Object&) const;
        
        //
        
        const std::string& var() const;
        int index() const;
        
    private:
        
        // DATA MEMBERS
        
        std::string mVar;
        int mIndex;
        
    //friendly:
        
        friend std::istream& operator>>(std::istream&, Object&);
        
    }; // end class Object
	
	Object llvm_object(const char* gvar, const int index=0);
	
} // end namespace program_model

#endif
