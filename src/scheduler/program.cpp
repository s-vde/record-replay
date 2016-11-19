
#include "program.hpp"

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   Program::Program(const std::string& dir, const std::string& file, const unsigned int n)
   : mDir(dir + "/instrumented")
   , mFile(file)
   , mNrThreads(n) { }
   
   //-------------------------------------------------------------------------------------
    
   const std::string& Program::file() const
   {
      return mFile;
   }
   
   //-------------------------------------------------------------------------------------
    
   unsigned int Program::nr_threads() const
   {
      return mNrThreads;
   }
   
   //-------------------------------------------------------------------------------------
    
   std::string Program::to_string() const
   {
      std::string str = mDir;
      str += "/";
      str += mFile;
      str += std::to_string(mNrThreads);
      return str;
   }
   
   //-------------------------------------------------------------------------------------

} // end namespace scheduler
