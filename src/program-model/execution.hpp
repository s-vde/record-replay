
#ifndef EXECUTION_HPP_INCLUDED
#define EXECUTION_HPP_INCLUDED

#include <assert.h>
#include <vector>
#include "execution_base.hpp"
#include "transition.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file execution.hpp
 @brief Definition of class template Execution<State>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    template<typename State> class Execution;
    template<typename State> std::istream& operator>>(std::istream&, Execution<State>&);
    
    template<typename State> class Execution : public ExecutionBase
    {
    public:
		
        using StatePtr = typename Transition<State>::StatePtr;
        
        // CTORS / DTOR
        
        /**
         @brief Constructs an empty Execution object with given number
         of threads.
         */
        explicit Execution(const unsigned int nr_threads, const StatePtr& s0=nullptr)
        : ExecutionBase(nr_threads)
        , mExecution()
        , mS0(s0) { }
        
        Execution(const Execution&) = default;
        Execution(Execution&&) = default;
        ~Execution() = default;

        // OPERATORS
        
        Execution& operator=(const Execution&) = default;
        Execution& operator=(Execution&&) = default;
        
        /**
         @brief Subscript operator.
         @note Indexing starts at 1.
         */
        Transition<State>& operator[](const index_t index)
        {
            return mExecution[index-1];
        }
        
        const Transition<State>& operator[](const index_t index) const
        {
            return mExecution[index-1];
        }
        
        // ITERATORS
    
        auto begin()
        {
            return mExecution.begin();
        }
        
        const auto begin() const
        {
            return mExecution.begin();
        }
        
        auto end()
        {
            return mExecution.end();
        }
        
        const auto end() const
        {
            return mExecution.end();
        }
        
        // GETTERS / SETTERS
        
        const State& s0() const
        {
            /// @pre initialized()
            assert(initialized());
            return *mS0;
        }
        
        void set_s0(const StatePtr& s0)
        {
            mS0 = s0;
        }
        
        //
        
        size_t size() const
        {
            return mExecution.size();
        }
        
        bool empty() const
        {
            return mExecution.empty();
        }

        bool initialized() const
        {
            return mS0 != nullptr;
        }
        
        // LAST

        /**
         @note Like std::vector::back calling last() on an empty Execution
         is undefined and does not throw an exception. In the current 
         implementation there is an assertion in place.
         */
        Transition<State>& last()
        {
            /// @pre !empty()
            assert(!empty());
            return mExecution.back();
        }
        
        const Transition<State>& last() const
        {
            /// @pre !empty()
            assert(!empty());
            return mExecution.back();
        }
        
        // FINAL
        
        // #todo Assertion should be trivial when s0 is a required argument to
        // Execution's constructor.
        State& final()
        {
            if (!empty()) { return last().post(); }
            else {
                /// @pre !empty() || initialized()
                assert(initialized());
                return *mS0;
            }
        }
        
        const State& final() const
        {
            if (!empty()) { return last().post(); }
            else {
                /// @pre !empty() || initialized()
                assert(initialized());
                return *mS0;
            }
        }
        
        // PUSH / POP
        
        // #todo Assertion should be trivial when s0 is a required argument to
        // Execution's constructor.
        void push_back(const Instruction& instr, const StatePtr& post)
        {
            /// @pre !empty() || initialized()
            assert(!empty() || initialized());
            mExecution.emplace_back(
                size()+1,
                final_ptr(),
                instr,
                post
            );
			if (is_lock_access(instr)) { set_contains_locks(); }
        }

        /**
         @note The popped Transition t still has (valid) pointers to t.pre 
         and t.post.
         */
        Transition<State> pop_last()
        {
            /// @pre !empty()
            assert(!empty());
            Transition<State> t = mExecution.back();
            mExecution.pop_back();
            return t;
        }
        
    private:
        
        // DATA MEMBERS
        
        /**
         @brief Modeling a sequence of Transition objects.
         */
        typename std::vector<Transition<State>> mExecution;
        
        /**
         @brief Pointer to the initial State of the Execution.
         */
        StatePtr mS0;
        
        // HELPER FUNCTIONS
        
        StatePtr final_ptr()
        {
            /// @pre !empty() || initialized()
            assert(!empty() || initialized());
            if (!empty())   { return last().post_ptr(); }
            else            { return mS0;               }
        }
        
    //friendly:
        
        /**
         @brief Template specialization for State of 
         operator>>(istream, Execution<State>) is friend.
         */
        friend std::istream& operator>><>(std::istream&, Execution<State>&);
        
    }; // end class template Execution<State>
} // end namespace program_model

#endif
