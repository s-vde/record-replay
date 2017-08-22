
#include "thread_io.hpp"

#include <iostream>

namespace program_model {

//--------------------------------------------------------------------------------------------------

std::string to_string(const Thread::Status& status)
{
   switch (status)
   {
      case Thread::Status::START:
         return "START";
      case Thread::Status::ENABLED:
         return "ENABLED";
      case Thread::Status::DISABLED:
         return "DISABLED";
      case Thread::Status::FINISHED:
         return "FINISHED";
   }
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Thread::Status& status)
{
   os << to_string(status);
   return os;
}

//--------------------------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, Thread::Status& status)
{
   std::string str;
   is >> str;
   if (str == "START")
   {
      status = Thread::Status::START;
   }
   else if (str == "ENABLED")
   {
      status = Thread::Status::ENABLED;
   }
   else if (str == "DISABLED")
   {
      status = Thread::Status::DISABLED;
   }
   else if (str == "FINISHED")
   {
      status = Thread::Status::FINISHED;
   }
   else
      is.setstate(std::ios::failbit);
   return is;
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Thread& thread)
{
   os << thread.tid() << " " << thread.status();
   return os;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
