
#include "transition.hpp"


namespace program_model {

//--------------------------------------------------------------------------------------------------

Transition::Transition(const int index, StatePtr pre, const instruction_t& instr, StatePtr post)
: mIndex(index)
, mPre(pre)
, mInstr(instr)
, mPost(post)
{
}

//--------------------------------------------------------------------------------------------------

bool Transition::operator==(const Transition& other) const
{
   return mIndex == other.mIndex && *mPre == *(other.mPre) && mInstr == other.mInstr &&
          *mPost == *(other.mPost);
}

//--------------------------------------------------------------------------------------------------

int Transition::index() const
{
   return mIndex;
}

//--------------------------------------------------------------------------------------------------

auto Transition::instr() const -> const instruction_t&
{
   return mInstr;
}

//--------------------------------------------------------------------------------------------------

auto Transition::pre_ptr() -> StatePtr
{
   return mPre;
}

//--------------------------------------------------------------------------------------------------

const State& Transition::pre() const
{
   return *mPre;
}

//--------------------------------------------------------------------------------------------------

State& Transition::pre()
{
   return *mPre;
}

//--------------------------------------------------------------------------------------------------

auto Transition::post_ptr() -> StatePtr
{
   return mPost;
}

//--------------------------------------------------------------------------------------------------

const State& Transition::post() const
{
   return *mPost;
}

//--------------------------------------------------------------------------------------------------

State& Transition::post()
{
   return *mPost;
}

//--------------------------------------------------------------------------------------------------

void Transition::set_pre(const StatePtr& pre)
{
   mPre = pre;
}

//--------------------------------------------------------------------------------------------------

void Transition::set_post(const StatePtr& post)
{
   mPost = post;
}

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
