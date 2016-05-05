
#ifndef SCHEDULER_HPP_INCLUDED
#define SCHEDULER_HPP_INCLUDED

#include <thread>
#include <unordered_map>
#include "control.hpp"
#include "execution.hpp"
#include "scheduler_settings.hpp"
#include "selector_register.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file scheduler.hpp
 @brief Definition class Scheduler.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace scheduler
{
    /**
     @details The Scheduler class provides a start routine for a scheduler 
     thread that has access to an encapsulated Control object that it can use 
     to control a multithreaded input program's thread interleaving. It 
     further encapsulates the scheduler's state, which includes a TaskPool 
     object. The class provides an interface for the input program's threads.
     */
    class Scheduler
    {
    public:
        
        using TidMap = std::unordered_map<pthread_t,const Thread::tid_t>;
        
        // CTORS / DTOR
        
        Scheduler();
        ~Scheduler();
        
        Scheduler(const Scheduler&) = delete;
        Scheduler(Scheduler&&) = delete;
        
        // OPERATORS
        
        Scheduler& operator=(const Scheduler&) = delete;
        Scheduler& operator=(Scheduler&&) = delete;
        
        // WRAPPERS

        /**
         @brief Calls pthread_create(pid, attr, start_routine, args) and
         registers the created thread with the Scheduler.
         @details Associates pid with a unique Scheduler-internal id in 
         mThreads and registers the created thread with mPool and mControl. 
         Signals mRegCond when the registration is finished. Only a single 
         thread at a time can use this functionality. This is important when
         threads can spawn other threads and hence multiple threads can be in 
         spawn_thread at the same time.
         @returns The return value of the pthread_create call.
         */
        int spawn_thread(
            pthread_t* pid,
            const pthread_attr_t* attr,
            void* (*start_routine)(void*),
            void* args
        );

        /**
         @brief Lets the calling thread wait until its registration with the
         Scheduler is complete.
         */
        void wait_registered();
        
        /**
         @brief If the program runs Scheduler-controlled, this function
         constructs an Instruction object from the the input, posts this 
         Instruction in mPool and calls Scheduler::wait_for_turn.
         */
        void post_task(const int op, const Object& obj);
        
        /**
         @brief If the program runs Scheduler-controlled, this function
         calls mPool.yield with the calling thread's Scheduler-internal id.
         */
        void yield();
        
        /**
         @brief If the program runs Scheduler-controlled, this function
         updates the status of the calling thread in mPool.
         */
        void finish();
        
        /**
         @brief Lets the main thread of the input program join the
         Scheduler thread.
         */
        void join();
        
    private:
        
        // FORWARD DECLARATIONS
        
        class LocalVars;
        
        // DATA MEMBERS

        std::unique_ptr<LocalVars> mLocVars;
        TaskPool mPool;
        Control mControl;
        
        // >>>>> mRegMutex protected
        /**
         @brief Protects mThreads, mNrRegistered and mRegCond.
         */
        // #todo Look at possibility of using shared_mutex
        std::mutex mRegMutex;
        TidMap mThreads;
        int mNrRegistered;
        std::condition_variable mRegCond;
        // <<<<<
        
        // >>>>> mStatusMutex protected
        Execution<State>::Status mStatus;
        std::mutex mStatusMutex;
        // <<<<<
        
        SchedulerSettings mSettings;
        SelectorUniquePtr mSelector;
        
        std::thread mThread;
        
        // SCHEDULER INTERNAL
        
        Thread::tid_t find_tid(const pthread_t& pid);
        
        bool runs_controlled();
        
        Execution<State>::Status status();
        
        void set_status(const Execution<State>::Status&);
        
        std::string thread_str(const Thread::tid_t&);
        std::string thread_str(const pthread_t&);
        
        // THREAD
        
        /**
         @brief Start routine of mThread.
         */
        void run();
        
        /**
         @brief Waits until mNrRegistered == LV.nr_threads.
         */
        void wait_all_registered();

        /**
         @brief Schedule the next task of given tid.
         @returns true iff scheduling the thread succeeded (i.e. tid
         is ENABLED.
         */
        bool schedule_thread(const Thread::tid_t& tid);
        
        void report_error(const std::string& what);
        
        void close(Execution<State>& E);
        
        void dump_execution(const Execution<State>& E) const;
   
    }; // end class Scheduler

    /**
     @brief Encapsulates the data that is modified by the Scheduler thread 
     mThread only.
     */
    class Scheduler::LocalVars
    {
    public:
        
        // LIFETIME
        
        LocalVars();
        
        LocalVars(const LocalVars&) = delete;
        LocalVars(LocalVars&&) = delete;
        ~LocalVars() = default;
        
        // OPERATORS
        
        LocalVars& operator=(const LocalVars&) = delete;
        LocalVars& operator=(LocalVars&&) = delete;
        
        //
        
        int nr_threads() const;
        const schedule_t& schedule() const;
        int task_nr() const;
        void increase_task_nr();
        
    private:
        
        /**
         @brief The (fixed) number of threads in the program.
         */
        int mNrThreads;
        
        /**
         @brief The schedule under which the scheduler is driving
         the program.
         */
        schedule_t mSchedule;
        
        /**
         @brief Counter of already scheduled tasks.
         */
        int mTaskNr;
        
    }; // end class Scheduler::LocalVars
} // end namespace scheduler

#endif
