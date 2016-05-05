
#ifndef PROGRAM_HPP_INCLUDED
#define PROGRAM_HPP_INCLUDED

#include <string>

/*---------------------------------------------------------------------------75*/
/**
 @file program.hpp
 @brief Definition of class Program.
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace scheduler
{
    /**
     @brief Model of an input program to the Scheduler.
     */
    class Program
    {
    public:
        
        // CTOR
        
        Program(const std::string& dir, const std::string& file, const unsigned int n);
        
        //
        
        const std::string& file() const;
        
        unsigned int nr_threads() const;
        
        std::string to_string() const;
        
    private:
		
        /// @brief The directory containing the program.
        std::string mDir;
		
        /// @brief The filename of the program.
        std::string mFile;
		
        /// @brief The number of threads in the program.
        unsigned int mNrThreads;
        
    }; // end class Program
} // end namespace scheduler

#endif
