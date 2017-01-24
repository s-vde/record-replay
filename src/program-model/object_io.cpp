
#include "object_io.hpp"

// STL
#include <iomanip>
#include <iostream>
#include <sstream>

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const Object::Op& op)
   {
      switch(op)
      {
         case Object::Op::READ   : return "READ";
         case Object::Op::WRITE  : return "WRITE";
         case Object::Op::RMW    : return "RMW";
         case Object::Op::LOCK   : return "LOCK";
         case Object::Op::UNLOCK : return "UNLOCK";
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream& os, const Object::Op& op)
   {
      os << to_string(op);
      return os;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::istream& operator>>(std::istream& is, Object::Op& op)
   {
      std::string str("");
      is >> str;
      if (str == "READ")          { op = Object::Op::READ;    }
      else if (str == "WRITE")    { op = Object::Op::WRITE;   }
      else if (str == "RMW")      { op = Object::Op::RMW;     }
      else if (str == "LOCK")     { op = Object::Op::LOCK;    }
      else if (str == "UNLOCK")   { op = Object::Op::UNLOCK;  }
      else { is.setstate(std::ios::failbit); }
      return is;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const Object& obj)
   {
      return to_pretty_string(obj);
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string to_pretty_string(const Object& obj)
   {
      std::stringstream stream;
      stream << std::hex << obj.m_address;
      return stream.str();
   }
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream& os, const Object& obj)
   {
      os << std::hex << obj.m_address << std::dec;
      return os;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::istream& operator>>(std::istream& is, Object& object)
   {
      std::uintptr_t ptr;
      is >> std::hex >> ptr >> std::dec;
      object.m_address = (void*)ptr;
      return is;
   }
   
   //-------------------------------------------------------------------------------------
    
} // end namespace program_model
