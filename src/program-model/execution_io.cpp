
#include "execution_io.hpp"

// PROGRAM_MODEL
#include "execution.hpp"
#include "state_io.hpp"
#include "transition.hpp"
#include "transition_io.hpp"

// UTILS
#include <utils_io.hpp>

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   std::string to_string(const Execution::Status& status)
   {
      switch(status)
      {
         case Execution::Status::RUNNING  : return "RUNNING";
         case Execution::Status::DONE     : return "DONE";
         case Execution::Status::DEADLOCK : return "DEADLOCK";
         case Execution::Status::BLOCKED  : return "BLOCKED";
         case Execution::Status::ERROR    : return "ERROR";
         default                          : return "UNDEFINED";
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   std::ostream& operator<<(std::ostream& os, const Execution::Status& status)
   {
      std::string str = to_string(status);
      os << to_string(status);
      return os;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::istream& operator>>(std::istream& is, Execution::Status& status)
   {
      std::string str = "";
      if (is >> str)
      {
         if (str == "RUNNING")       { status = Execution::Status::RUNNING;  }
         else if (str == "DONE")     { status = Execution::Status::DONE;     }
         else if (str == "DEADLOCK") { status = Execution::Status::DEADLOCK; }
         else if (str == "BLOCKED")  { status = Execution::Status::BLOCKED;  }
         else if (str == "ERROR")    { status = Execution::Status::ERROR;    }
         else                        { is.setstate(std::ios::failbit);       }
      }
      return is;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::string to_short_string(const Execution& E)
   {
      std::string str("");
      for (const auto& trans : E)
      {
         str += program_model::to_short_string(trans);
         str += "\n";
      }
      str += to_string(E.status());
      return str;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::ostream& operator<<(std::ostream& os, const Execution& E)
   {
      if (E.initialized())
      {
         os << E.s0() << std::endl;
         for (const auto& trans : E)
         {
            os << to_string_post(trans) << std::endl;
         }
         os << "=====" << std::endl << E.status();
      }
      return os;
   }
   
   //-------------------------------------------------------------------------------------
   
   std::istream& operator>>(std::istream& is, Execution& E)
   {
      int i = 0;
      while (true)
      {
         // try read E.status
         if ((is >> std::ws).peek() == std::char_traits<char>::to_int_type('='))
         {
            utils::io::skip(is, '\n', 1);
            Execution::Status status;
            is >> status;
            E.set_status(status);
            break;
         }
         
         // read State s0
         if (i == 0)
         {
            // set E.s0
            if (!E.initialized())
            {
               if (!(is >> E.mS0))
               {
                  break;
               }
               // ignore
            }
            else if (!utils::io::skip(is, '\n', 1))
            {
               break;
            }
         }
         
         // read Transition
         if (i == E.size())
         {
            int index;
            Instruction instr;
            is >> index >> instr;
            std::shared_ptr<State> post{};
            if (is >> post)
            {
               E.push_back(instr, post);
            }
            else
            {
               break;
            }
         }
         // update object in Transition
         else if (!utils::io::skip(is, '\n', 2))
         {
            break;
         }
         ++i;
      }
      return is;
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
