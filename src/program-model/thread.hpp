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
      CONTROL,
      ENABLED,
      DISABLED,
      FINISHED
   };

   /// @brief Constructor.

   explicit Thread(const tid_t&);

   /// @brief Getter.

   tid_t tid() const;

   /// @brief Getter.

   const Status& status() const;

   /// @brief Setter.

   void set_status(const Status&);

private:
   tid_t mTid;
   Status mStatus;

   friend Thread read_Thread(std::istream&);

}; // end class Thread


// Type definitions
using Tids = std::set<Thread::tid_t>;

} // end namespace program_model
