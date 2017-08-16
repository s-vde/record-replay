#pragma once

#include "object.hpp"
#include "thread.hpp"
#include "visible_instruction.hpp"

//--------------------------------------------------------------------------------------------------
/// @file instruction.hpp
/// @author Susanne van den Elsen
/// @date 2015-2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

class Instruction
{
public:
   /// @brief Constructor.

   Instruction(const Thread::tid_t tid, const Object::Op&, const Object&, bool is_atomic);

   /// @brief Default constructor.

   Instruction();

   bool operator==(const Instruction&) const;

   /// @brief Getter.

   Thread::tid_t tid() const;

   /// @brief Getter.

   const Object::Op& op() const;

   /// @brief Getter.

   const Object& obj() const;

   bool is_atomic() const;

   void add_meta_data(const meta_data_t& meta_data);
   const meta_data_t& meta_data() const;

private:
   Thread::tid_t mTid;
   Object::Op mOp;
   Object mObj;
   bool m_is_atomic;
   meta_data_t m_meta_data;


   friend std::istream& operator>>(std::istream&, Instruction&);

}; // end class Instruction


bool is_lock_access(const Instruction&);

} // end namespace program_model
