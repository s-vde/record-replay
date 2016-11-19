
#include "execution_io.hpp"

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const ExecutionBase::Status& status)
   {
      switch(status)
      {
         case ExecutionBase::Status::RUNNING  : return "RUNNING";
         case ExecutionBase::Status::DONE     : return "DONE";
         case ExecutionBase::Status::DEADLOCK : return "DEADLOCK";
         case ExecutionBase::Status::BLOCKED  : return "BLOCKED";
         case ExecutionBase::Status::ERROR    : return "ERROR";
         default                              : return "UNDEFINED";
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream& os, const ExecutionBase::Status& status)
   {
      std::string str = to_string(status);
      os << to_string(status);
      return os;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::istream& operator>>(std::istream& is, ExecutionBase::Status& status)
   {
      std::string str = "";
      if (is >> str)
      {
         if (str == "RUNNING")       { status = ExecutionBase::Status::RUNNING;  }
         else if (str == "DONE")     { status = ExecutionBase::Status::DONE;     }
         else if (str == "DEADLOCK") { status = ExecutionBase::Status::DEADLOCK; }
         else if (str == "BLOCKED")  { status = ExecutionBase::Status::BLOCKED;  }
         else if (str == "ERROR")    { status = ExecutionBase::Status::ERROR;    }
         else                        { is.setstate(std::ios::failbit);           }
      }
      return is;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
