
#include "object_io.hpp"
#include <iostream>

namespace program_model
{
    std::string to_string(const Object::Op& op)
    {
        switch(op) {
            case Object::Op::READ   : return "READ";
            case Object::Op::WRITE  : return "WRITE";
            case Object::Op::LOCK   : return "LOCK";
            case Object::Op::UNLOCK : return "UNLOCK";
        }
    }
    
    std::ostream& operator<<(std::ostream& os, const Object::Op& op)
    {
        os << to_string(op);
        return os;
    }
    
    std::istream& operator>>(std::istream& is, Object::Op& op)
    {
        std::string str("");
        is >> str;
        if (str == "READ")          { op = Object::Op::READ;    }
        else if (str == "WRITE")    { op = Object::Op::WRITE;   }
        else if (str == "LOCK")     { op = Object::Op::LOCK;    }
        else if (str == "UNLOCK")   { op = Object::Op::UNLOCK;  }
        else { is.setstate(std::ios::failbit); }
        return is;
    }
    
    std::string to_pretty_string(const Object& obj)
    {
        std::string str = obj.var();
        str += "[";
        str += std::to_string(obj.index());
        str += "]";
        return str;
    }
    
    std::ostream& operator<<(std::ostream& os, const Object& obj)
    {
        os << obj.var() << " " << obj.index();
        return os;
    }
    
    std::istream& operator>>(std::istream& is, Object& obj)
    {
        is >> obj.mVar >> obj.mIndex;
        return is;
    }
    
} // end namespace program_model