
#ifndef THREAD_HPP_INCLUDED
#define THREAD_HPP_INCLUDED

#include <set>

/*---------------------------------------------------------------------------75*/
/**
 @file thread.hpp
 @brief Definition of class Thread and type alias Tids.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    class Thread
    {
    public:
        
        // TYPES
        
        // #todo type is not unsigned int because of Instruction's default ctors
        using tid_t = int;
        
        enum class Status { START, CONTROL, ENABLED, DISABLED, FINISHED };
        
        // CTORS / DTOR
        
        explicit Thread(const tid_t&);
        
        Thread(const Thread&) = default;
        Thread(Thread&&) = default;
        ~Thread() = default;
        
        // OPERATORS
        
        Thread& operator=(const Thread&) = default;
        Thread& operator=(Thread&&) = default;
        
        //
        
        tid_t tid() const;
        const Status& status() const;
        void set_status(const Status&);
        
    private:
        
        // DATA MEMBERS
        
        tid_t mTid;
        Status mStatus;
        
    //friendly:
        
        friend Thread read_Thread(std::istream&);
        
    }; // end class Thread
    
    using Tids = std::set<Thread::tid_t>;
    
} // end namespace program_model

#endif
