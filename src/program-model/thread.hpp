#pragma once

#include <set>

//--------------------------------------------------------------------------------------------------
/// @file thread.hpp
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace program_model {

class Thread
{
public:
   // tid_t is not unsigned int because of Instruction's default ctors
   using tid_t = int;

   enum class Status
   {
      START,
      ENABLED,
      DISABLED,
      FINISHED
   };
   
   /// @brief Default constructor.
   /// @note Required for istream right shift operator.
   
   Thread();
   
   bool operator==(const Thread&) const;
   
   /// @brief Constructor.

   explicit Thread(const tid_t&, Status status = Status::START);

   /// @brief Getter.

   tid_t tid() const;

   /// @brief Getter.

   const Status& status() const;

   /// @brief Setter.

   void set_status(const Status&);

private:
   tid_t mTid;
   Status mStatus;
   
   friend std::istream& operator>>(std::istream&, Thread&);

}; // end class Thread


// Type definitions
using Tids = std::set<Thread::tid_t>;

} // end namespace program_model
