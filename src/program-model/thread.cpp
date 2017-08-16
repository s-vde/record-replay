
#include "thread.hpp"

namespace program_model {

//--------------------------------------------------------------------------------------------------

Thread::Thread(const tid_t& tid)
: mTid(tid)
, mStatus(Status::START)
{
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
