
#include "execution_base.hpp"

namespace program_model
{
    ExecutionBase::ExecutionBase(const unsigned int nr_threads)
    : mNrThreads(nr_threads)
    , mStatus(Status::RUNNING) { }
    
    unsigned int ExecutionBase::nr_threads() const
    {
        return mNrThreads;
    }
    
    const ExecutionBase::Status& ExecutionBase::status() const
    {
        return mStatus;
    }
        
    void ExecutionBase::set_status(const Status& status)
    {
        mStatus = status;
    }
	
	bool ExecutionBase::contains_locks() const
	{
		return mContainsLocks;
	}
	
	void ExecutionBase::set_contains_locks()
	{
		mContainsLocks = true;
	}
} // end namespace program_model
