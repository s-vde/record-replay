
#include "thread.hpp"

namespace program_model {

//--------------------------------------------------------------------------------------------------

Thread::Thread() : Thread(-1) {};

//--------------------------------------------------------------------------------------------------

Thread::Thread(const tid_t& tid, const Status status)
: mTid(tid)
, mStatus(status)
{
}

//--------------------------------------------------------------------------------------------------

bool Thread::operator==(const Thread& other) const
{
   return mTid == other.mTid && mStatus == other.mStatus;
}

//--------------------------------------------------------------------------------------------------

Thread::tid_t Thread::tid() const
{
   return mTid;
}

//--------------------------------------------------------------------------------------------------

const Thread::Status& Thread::status() const
{
   return mStatus;
}

//--------------------------------------------------------------------------------------------------

void Thread::set_status(const Status& status)
{
   mStatus = status;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
