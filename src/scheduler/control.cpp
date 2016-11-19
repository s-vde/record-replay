
#include "control.hpp"

// STL
#include <assert.h>

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   Control::Control()
   : mHandles(Handles())
   , mMutex()
   , mOwner() { }

   //-------------------------------------------------------------------------------------
   
   void Control::register_thread(const Thread::tid_t& tid)
   {
      std::lock_guard<std::mutex> guard(mMutex);
      /// @pre mHandles.find(tid) == mHandles.end() (i.e. tid not yet registered)
      assert(mHandles.find(tid) == mHandles.end());
      // Implementation uses emplace rather than insert because copy
      // constructor of BinarySem is implicitly deleted due to deleted copy
      // constructor of std::mutex. If rehashing occurs due to emplacement,
      // iterators are invalidated, but references are not.
      mHandles.emplace(std::piecewise_construct,
                       std::forward_as_tuple(tid),
                       std::forward_as_tuple(tid));
   }
   
   //-------------------------------------------------------------------------------------
   
   void Control::wait_for_turn(const Thread::tid_t& tid)
   {
      handle(tid).wait();
   }
   
   //-------------------------------------------------------------------------------------
    
   void Control::grant_execution_right(const Thread::tid_t& tid)
   {
      if (is_owner())
      {
         handle(tid).post(true, BinarySem::BroadcastMode::NOTIFY_ONE);
      }
   }
   
   //-------------------------------------------------------------------------------------

   void Control::grant_execution_right_all()
   {
      if (is_owner())
      {
         std::lock_guard<std::mutex> guard(mMutex);
         for (auto& handle : mHandles)
         {
            handle.second.post(true, BinarySem::BroadcastMode::NOTIFY_ONE);
         }
      }
   }
   
   //-------------------------------------------------------------------------------------
    
   void Control::set_owner(const std::thread::id& tid)
   {
      mOwner = tid;
   }
   
   //-------------------------------------------------------------------------------------
    
   BinarySem& Control::handle(const Thread::tid_t& tid)
   {
      std::unique_lock<std::mutex> ul(mMutex);
      auto handle(mHandles.find(tid));
      /// @pre mHandles.find(tid) != mHandles.end()
      assert(handle != mHandles.end());
      return handle->second;
   }
   
   //-------------------------------------------------------------------------------------
    
   bool Control::is_owner()
   {
      return mOwner == std::this_thread::get_id();
   }
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler
