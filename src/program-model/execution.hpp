#pragma once

#include "transition.hpp"

#include <assert.h>
#include <set>
#include <vector>

//--------------------------------------------------------------------------------------------------
/// @file execution.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

class Execution
{
public:
   using index_t = unsigned int;
   using instruction_t = visible_instruction_t;
   using transition_t = Transition;
   using execution_t = std::vector<transition_t>;
   using StatePtr = typename transition_t::StatePtr;

   enum class Status
   {
      RUNNING = 0,
      DONE = 1,
      DEADLOCK = 2,
      BLOCKED = 3,
      ERROR = 4
   };

   /// @brief Constructs an empty Execution object with given number of threads.

   explicit Execution(const StatePtr& s0 = nullptr);

   bool operator==(const Execution&);

   /// @brief Subscript operator.
   /// @note Indexing starts at 1.

   transition_t& operator[](const index_t index);

   /// @brief Subscript operator.
   /// @note Indexing starts at 1.

   const transition_t& operator[](const index_t index) const;

   execution_t::iterator begin();
   execution_t::const_iterator begin() const;
   execution_t::iterator end();
   execution_t::const_iterator end() const;

   /// @brief Getter.

   const State& s0() const;
   void set_s0(const StatePtr& s0);

   size_t size() const;
   bool empty() const;

   bool initialized() const;

   /// @note Like std::vector::back calling last() on an empty Execution is undefined
   /// and does not throw an exception. In the current implementation there is an
   /// assertion in place.

   transition_t& last();
   const transition_t& last() const;

   // #todo Assertion should be trivial when s0 is a required argument to
   // Execution's constructor.
   State& final();
   const State& final() const;

   // #todo Assertion should be trivial when s0 is a required argument to
   // Execution's constructor.
   void push_back(const instruction_t& instr, const StatePtr& post);

   /// @note The popped Transition t still has (valid) pointers to t.pre and t.post.

   transition_t pop_last();

   /// @brief Getter.

   unsigned nr_threads() const;

   /// @brief Getter.

   const Status& status() const;

   /// @brief Setter.

   void set_status(const Status& status);

   /// @brief Getter.

   bool contains_locks() const;

   /// @brief Setter.

   void set_contains_locks();

private:
   /// @brief Modeling a sequence of Transition objects.
   execution_t mExecution;

   /// @brief Pointer to the initial State of the Execution.
   StatePtr mS0;

   /// @brief Set of Threads being spawn during this Execution.
   std::set<Thread::tid_t> mThreads;

   /// @brief The (current/termination) status of the Execution object.
   Status mStatus;

   bool mContainsLocks;

   StatePtr final_ptr();

   /// @brief Template specialization for State of
   /// operator>>(istream, Execution<State>) is friend.

   friend std::istream& operator>>(std::istream&, Execution&);

}; // end class template Execution<State>

// Type definitions
using thread_execution_t = std::vector<Execution::instruction_t>;

} // end namespace program_model
