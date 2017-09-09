#pragma once

#include "visible_instruction.hpp"

#include <unordered_map>

//--------------------------------------------------------------------------------------------------
/// @file state.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

struct next_t
{
   visible_instruction_t instr;
   bool enabled;
};

// Type definitions
using NextSet = std::unordered_map<Thread::tid_t, next_t>;

//--------------------------------------------------------------------------------------------------

class State
{
public:
   using SharedPtr = std::shared_ptr<State>;

   /// @brief Constructor.

   State(const Tids& enabled, const NextSet& next);

   /// @brief Getter.

   std::string tag() const;

   /// @brief Getter.

   const Tids& enabled() const;

   /// @brief Getter.

   bool is_enabled(const Thread::tid_t& tid) const;

   const NextSet::const_iterator next(const Thread::tid_t& tid) const;

   NextSet::const_iterator next_cbegin() const;

   NextSet::const_iterator next_cend() const;

   bool has_next(const Thread::tid_t& tid) const;

private:
   static const std::string mTag;

   Tids mEnabled;

   /// @brief @brief <code>{(next_{this}(p),enabled(this,p)) | p in Tids }</code>.

   NextSet mNext;

   friend std::ostream& operator<<(std::ostream&, const State&);
   friend std::istream& operator>>(std::istream&, State&);

}; // end class State

} // end namespace program_model
