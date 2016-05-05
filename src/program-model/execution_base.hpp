
#ifndef EXECUTION_BASE_HPP_INCLUDED
#define EXECUTION_BASE_HPP_INCLUDED

/*---------------------------------------------------------------------------75*/
/**
 @file execution_base.hpp
 @brief Definition of class ExecutionBase.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    class ExecutionBase
    {
    public:
		
		using index_t = unsigned int;
        
        enum class Status
        {
            RUNNING = 0,
            DONE = 1,
            DEADLOCK = 2,
            BLOCKED = 3,
            ERROR = 4
        };
        
        explicit ExecutionBase(const unsigned int nr_threads);
        
        unsigned nr_threads() const;
        const Status& status() const;
        void set_status(const Status& status);
		
		bool contains_locks() const;
		void set_contains_locks();
        
    private:
        
        /**
         @brief The number of threads in the program to which this Execution
         belongs.
         */
        unsigned int mNrThreads;
        
        /**
         @brief The (current/termination) status of the Execution object.
         */
        Status mStatus;
		
		bool mContainsLocks;
        
    }; // end class ExecutionBase
} // end namespace program_model

#endif
