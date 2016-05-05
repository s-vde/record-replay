
#ifndef TRANSITION_HPP_INCLUDED
#define TRANSITION_HPP_INCLUDED

#include <vector>
#include "instruction.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file transition.hpp
 @brief Definition of class template Transition<State>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    template<typename State>
    class Transition
    {
    public:
        
        // TYPES
        
        using StatePtr = typename State::SharedPtr;
        
        // CTORS / DTOR
        
        Transition(
            const int index,
            StatePtr pre,
            const Instruction& instr,
            StatePtr post)
        : mIndex(index)
        , mPre(pre)
        , mInstr(instr)
        , mPost(post) { }
        
        Transition(const Transition&) = default;
        Transition(Transition&&) = default;
        ~Transition() = default;
        
        // OPERATORS
        
        Transition& operator=(const Transition&) = default;
        Transition& operator=(Transition&&) = default;
        
        //
        
        int index() const
        {
            return mIndex;
        }
        
        const Instruction& instr() const
        {
            return mInstr;
        }
        
        StatePtr pre_ptr()
        {
            return mPre;
        }
        
        const State& pre() const
        {
            return *mPre;
        }
        
        State& pre()
        {
            return *mPre;
        }
        
        StatePtr post_ptr()
        {
            return mPost;
        }
        
        const State& post() const
        {
            return *mPost;
        }
        
        State& post()
        {
            return *mPost;
        }
        
        void set_pre(const StatePtr& pre)
        {
            mPre = pre;
        }
        
        void set_post(const StatePtr& post)
        {
            mPost = post;
        }
        
    private:
        
        // DATA MEMBERS
        
        int mIndex;
        StatePtr mPre;
        const Instruction mInstr;
        StatePtr mPost;
        
    }; // end class template Transition<State>
} // end namespace program_model

#endif
