
#include "program.hpp"

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   Program::Program(const std::string& dir, const std::string& file)
   : mDir(dir + "/instrumented")
   , mFile(file)
   { }
   
   //-------------------------------------------------------------------------------------
    
   const std::string& Program::file() const
   {
      return mFile;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string Program::to_string() const
   {
      std::string str = mDir;
      str += "/";
      str += mFile;
      return str;
   }
   
   //-------------------------------------------------------------------------------------

} // end namespace scheduler
