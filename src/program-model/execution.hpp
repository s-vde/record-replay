#pragma once

// PROGRAM_MODEL
#include "execution_base.hpp"
#include "transition.hpp"

// STL
#include <assert.h>
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file execution.hpp
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
   template<typename State> class Execution;
   template<typename State> std::istream& operator>>(std::istream&, Execution<State>&);
   
   //-------------------------------------------------------------------------------------
   
   template<typename State> class Execution : public ExecutionBase
   {
   public:
      
      //----------------------------------------------------------------------------------
		
      using StatePtr = typename Transition<State>::StatePtr;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Constructs an empty Execution object with given number of threads.
      
      explicit Execution(const unsigned int nr_threads, const StatePtr& s0=nullptr)
      : ExecutionBase(nr_threads)
      , mExecution()
      , mS0(s0) { }
      
      //----------------------------------------------------------------------------------

      /// @brief Subscript operator.
      /// @note Indexing starts at 1.

      Transition<State>& operator[](const index_t index)
      {
         return mExecution[index-1];
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Subscript operator.
      /// @note Indexing starts at 1.
      
      const Transition<State>& operator[](const index_t index) const
      {
         return mExecution[index-1];
      }
      
      //----------------------------------------------------------------------------------
        
      auto begin()
      {
         return mExecution.begin();
      }
      
      //----------------------------------------------------------------------------------
        
      const auto begin() const
      {
         return mExecution.begin();
      }
      
      //----------------------------------------------------------------------------------
        
      auto end()
      {
         return mExecution.end();
      }
      
      //----------------------------------------------------------------------------------
        
      const auto end() const
      {
         return mExecution.end();
      }
      
      //----------------------------------------------------------------------------------
        
      /// @brief Getter.
        
      const State& s0() const
      {
         /// @pre initialized()
         assert(initialized());
         return *mS0;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Setter.
        
      void set_s0(const StatePtr& s0)
      {
         mS0 = s0;
      }
      
      //----------------------------------------------------------------------------------
      
      /// @brief Getter.
      
      size_t size() const
      {
         return mExecution.size();
      }
      
      //----------------------------------------------------------------------------------
        
      bool empty() const
      {
         return mExecution.empty();
      }
      
      //----------------------------------------------------------------------------------

      bool initialized() const
      {
         return mS0 != nullptr;
      }
      
      //----------------------------------------------------------------------------------
        
      /// @note Like std::vector::back calling last() on an empty Execution is undefined
      /// and does not throw an exception. In the current implementation there is an
      /// assertion in place.

      Transition<State>& last()
      {
         /// @pre !empty()
         assert(!empty());
         return mExecution.back();
      }
      
      //----------------------------------------------------------------------------------
      
      const Transition<State>& last() const
      {
         /// @pre !empty()
         assert(!empty());
         return mExecution.back();
      }
      
      //----------------------------------------------------------------------------------
        
      // #todo Assertion should be trivial when s0 is a required argument to
      // Execution's constructor.
      State& final()
      {
         if (!empty())
         {
            return last().post();
         }
         /// @pre !empty() || initialized()
         assert(initialized());
         return *mS0;
      }
      
      //----------------------------------------------------------------------------------
        
      const State& final() const
      {
         if (!empty())
         {
            return last().post();
         }
         /// @pre !empty() || initialized()
         assert(initialized());
         return *mS0;
      }
      
      //----------------------------------------------------------------------------------
        
      // #todo Assertion should be trivial when s0 is a required argument to
      // Execution's constructor.
      void push_back(const Instruction& instr, const StatePtr& post)
      {
         /// @pre !empty() || initialized()
         assert(!empty() || initialized());
         mExecution.emplace_back(size()+1, final_ptr(), instr, post);
			if (is_lock_access(instr))
         {
            set_contains_locks();
         }
      }
      
      //----------------------------------------------------------------------------------

      /// @note The popped Transition t still has (valid) pointers to t.pre and t.post.

      Transition<State> pop_last()
      {
         /// @pre !empty()
         assert(!empty());
         Transition<State> t = mExecution.back();
         mExecution.pop_back();
         return t;
      }
      
      //----------------------------------------------------------------------------------
        
   private:

      //----------------------------------------------------------------------------------
      
      /// @brief Modeling a sequence of Transition objects.

      std::vector<Transition<State>> mExecution;
      
      //----------------------------------------------------------------------------------
        
      /// @brief Pointer to the initial State of the Execution.

      StatePtr mS0;
        
      //----------------------------------------------------------------------------------
        
      StatePtr final_ptr()
      {
         /// @pre !empty() || initialized()
         assert(!empty() || initialized());
         if (empty())
         {
            return mS0;
         }
         return last().post_ptr();
      }
      
      //----------------------------------------------------------------------------------
        
      /// @brief Template specialization for State of
      /// operator>>(istream, Execution<State>) is friend.
      
      friend std::istream& operator>><>(std::istream&, Execution<State>&);
      
      //----------------------------------------------------------------------------------
        
   }; // end class template Execution<State>
   
   //-------------------------------------------------------------------------------------
   
} // end namespace program_model
