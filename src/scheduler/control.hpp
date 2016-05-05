
#ifndef CONTROL_HPP_INCLUDED
#define CONTROL_HPP_INCLUDED

#include <unordered_map>
#include "binary_sem.hpp"
#include "thread.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file control.hpp
 @brief Definition of class Control.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

using namespace program_model;
using utils::threads::BinarySem;

namespace scheduler
{
    class Control
    {
    public:
        
        // CTORS / DTOR
        
        Control();
        
        Control(const Control&) = delete;
        Control(Control&&) = delete;
        ~Control() = default;
        
        // OPERATORS
        
        Control& operator=(const Control&) = delete;
        Control& operator=(Control&&) = delete;
        
        //

        /** 
         @brief Registers the thread with given tid in the Control unit by 
         assigning it a handle.
         */
        void register_thread(const Thread::tid_t& tid);

        /**
         @brief Lets the thread with given tid wait until it is granted 
         execution right.
         */
        void wait_for_turn(const Thread::tid_t& tid);
        
        /**
         @brief Grant the thread with given tid execution right.
         @details Execution right is granted to thread tid by setting the value
         of mHandles[tid] to true and notifying the waiting thread.
         @permission mOwner.
         */
        void grant_execution_right(const Thread::tid_t& tid);

        /**
         @brief Grants execution right to all registered threads. Allows the 
         program to continue without the Scheduler's control.
         @permission mOwner.
         */
        void grant_execution_right_all();
        
        void set_owner(const std::thread::id& tid);
        
    private:
        
        using Handles = std::unordered_map<Thread::tid_t,BinarySem>;

        /**
         @brief Maps Thread::tid_t's to their handles.
         @details A handle is a BinarySem object that the associated thread 
         waits on until it is granted execution right.
         */
        Handles mHandles;
        
        /**
         @brief Mutex for protecting mHandles.
         */
        std::mutex mMutex;
        
        /**
         @brief The thread that is in control.
         */
        std::thread::id mOwner;

        /**
         @brief Retreive the handle for the thread with given tid in a
         lock-protected section.
         */
        BinarySem& handle(const Thread::tid_t& tid);

        /**
         @brief Returns whether the calling thread is this Control's owner.
         */
        bool is_owner();
        
    }; // end class Control
} // end namespace scheduler

#endif