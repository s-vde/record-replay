
#include "state.hpp"

// UTILS
#include "container_input.hpp"

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   const std::string State::mTag = "State";
   
   //-------------------------------------------------------------------------------------
   
   State::State(const Tids& enabled, const NextSet& next)
   : mEnabled(enabled)
   , mNext(next) { }
   
   //-------------------------------------------------------------------------------------
   
   std::string State::tag() const
   {
      return mTag;
   }
   
   //-------------------------------------------------------------------------------------
   
   const Tids& State::enabled() const
   {
      return mEnabled;
   }
	
   //-------------------------------------------------------------------------------------
   
   bool State::is_enabled(const Thread::tid_t& tid) const
   {
      return mEnabled.find(tid) != mEnabled.end();
   }
	
   //-------------------------------------------------------------------------------------
   
   const NextSet::const_iterator State::next(const Thread::tid_t& tid) const
   {
      return mNext.find(tid);
   }
   
   //-------------------------------------------------------------------------------------
    
   NextSet::const_iterator State::next_cbegin() const
   {
      return mNext.begin();
   }
   
   //-------------------------------------------------------------------------------------
    
   NextSet::const_iterator State::next_cend() const
   {
      return mNext.end();
   }
   
   //-------------------------------------------------------------------------------------
    
   bool State::has_next(const Thread::tid_t& tid) const
   {
      return mNext.find(tid) != mNext.end();
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
