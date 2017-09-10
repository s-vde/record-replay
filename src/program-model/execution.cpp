
#include "execution.hpp"


namespace program_model {

//--------------------------------------------------------------------------------------------------

Execution::Execution(const unsigned int nr_threads, const StatePtr& s0)
: mExecution()
, mS0(s0)
, mNrThreads(nr_threads)
, mStatus(Status::RUNNING)
{
}

//--------------------------------------------------------------------------------------------------

bool Execution::operator==(const Execution& other)
{
   return mExecution == other.mExecution && *mS0 == *(other.mS0) &&
          mNrThreads == other.mNrThreads && mStatus == other.mStatus;
}

//--------------------------------------------------------------------------------------------------

auto Execution::operator[](const index_t index) -> transition_t&
{
   return mExecution[index - 1];
}

//--------------------------------------------------------------------------------------------------

auto Execution::operator[](const index_t index) const -> const transition_t&
{
   return mExecution[index - 1];
}

//--------------------------------------------------------------------------------------------------

auto Execution::begin() -> execution_t::iterator
{
   return mExecution.begin();
}

//--------------------------------------------------------------------------------------------------

auto Execution::begin() const -> execution_t::const_iterator
{
   return mExecution.begin();
}

//--------------------------------------------------------------------------------------------------

auto Execution::end() -> execution_t::iterator
{
   return mExecution.end();
}

//--------------------------------------------------------------------------------------------------

auto Execution::end() const -> execution_t::const_iterator
{
   return mExecution.end();
}

//--------------------------------------------------------------------------------------------------

const State& Execution::s0() const
{
   /// @pre initialized()
   assert(initialized());
   return *mS0;
}

//--------------------------------------------------------------------------------------------------

void Execution::set_s0(const StatePtr& s0)
{
   mS0 = s0;
}

//--------------------------------------------------------------------------------------------------

size_t Execution::size() const
{
   return mExecution.size();
}

//--------------------------------------------------------------------------------------------------

bool Execution::empty() const
{
   return mExecution.empty();
}

//--------------------------------------------------------------------------------------------------

bool Execution::initialized() const
{
   return mS0 != nullptr;
}

//--------------------------------------------------------------------------------------------------

auto Execution::last() -> transition_t&
{
   /// @pre !empty()
   assert(!empty());
   return mExecution.back();
}

//--------------------------------------------------------------------------------------------------

auto Execution::last() const -> const transition_t&
{
   /// @pre !empty()
   assert(!empty());
   return mExecution.back();
}

//--------------------------------------------------------------------------------------------------

State& Execution::final()
{
   if (!empty())
   {
      return last().post();
   }
   /// @pre !empty() || initialized()
   assert(initialized());
   return *mS0;
}

//--------------------------------------------------------------------------------------------------

const State& Execution::final() const
{
   if (!empty())
   {
      return last().post();
   }
   /// @pre !empty() || initialized()
   assert(initialized());
   return *mS0;
}

//--------------------------------------------------------------------------------------------------

void Execution::push_back(const instruction_t& instr, const StatePtr& post)
{
   /// @pre !empty() || initialized()
   assert(!empty() || initialized());
   mExecution.emplace_back(size() + 1, final_ptr(), instr, post);
   if (boost::get<lock_instruction>(&instr))
   {
      set_contains_locks();
   }
}

//--------------------------------------------------------------------------------------------------

auto Execution::pop_last() -> transition_t
{
   /// @pre !empty()
   assert(!empty());
   transition_t transition = mExecution.back();
   mExecution.pop_back();
   return transition;
}

//--------------------------------------------------------------------------------------------------

auto Execution::final_ptr() -> StatePtr
{
   /// @pre !empty() || initialized()
   assert(!empty() || initialized());
   if (empty())
   {
      return mS0;
   }
   return last().post_ptr();
}

//--------------------------------------------------------------------------------------------------

unsigned int Execution::nr_threads() const
{
   return mNrThreads;
}

//--------------------------------------------------------------------------------------------------

const Execution::Status& Execution::status() const
{
   return mStatus;
}

//--------------------------------------------------------------------------------------------------

void Execution::set_status(const Status& status)
{
   mStatus = status;
}

//--------------------------------------------------------------------------------------------------

bool Execution::contains_locks() const
{
   return mContainsLocks;
}

//--------------------------------------------------------------------------------------------------

void Execution::set_contains_locks()
{
   mContainsLocks = true;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
