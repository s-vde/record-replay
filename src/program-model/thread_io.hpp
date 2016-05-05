
#ifndef THREAD_IO_HPP_INCLUDED
#define THREAD_IO_HPP_INCLUDED

#include <iosfwd>       // forward declaration of std::ostream and std::istream
#include "thread.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file thread_io.hpp
 @brief Declaration of input/output stream operator overloads for Thread.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    std::string to_string(const Thread::Status&);
    std::ostream& operator<<(std::ostream&, const Thread::Status&);
    std::istream& operator>>(std::istream&, Thread::Status&);
    
    std::ostream& operator<<(std::ostream&, const Thread&);
    
} // end namespace program_model

#endif
