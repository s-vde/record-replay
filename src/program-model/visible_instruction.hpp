#pragma once

#include "object.hpp"
#include "thread.hpp"

#include <boost/variant.hpp>

//--------------------------------------------------------------------------------------------------
/// @file visible_instruction.hpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------


namespace program_model {

struct meta_data_t
{
   std::string file_name;
   unsigned int line_number;

}; // end struct meta_data_t

inline bool operator==(const meta_data_t& lhs, const meta_data_t& rhs)
{
   return lhs.file_name == rhs.file_name && lhs.line_number == rhs.line_number;
}

//--------------------------------------------------------------------------------------------------


namespace detail {

template <typename thread_id_t, typename operation_type, typename memory_location_t>
class visible_instruction
{
public:
   using operation_t = operation_type;

   /// @brief Default constructor.
   /// @note Required for istream right shift operator.

   visible_instruction()
   : m_tid(-1)
   , m_meta_data({"unknown", 0})
   {
   }

   visible_instruction(const thread_id_t& tid, const operation_t& operation,
                       const memory_location_t& operand,
                       const meta_data_t& meta_data = {"unknown", 0})
   : m_tid(tid)
   , m_operation(operation)
   , m_operand(operand)
   , m_meta_data(meta_data)
   {
   }

   const thread_id_t& tid() const { return m_tid; }
   const operation_t& operation() const { return m_operation; }
   const memory_location_t& operand() const { return m_operand; }
   void add_meta_data(const meta_data_t& meta_data) { m_meta_data = meta_data; }
   const meta_data_t& meta_data() const { return m_meta_data; }

private:
   thread_id_t m_tid;
   operation_t m_operation;
   memory_location_t m_operand;
   meta_data_t m_meta_data;

}; // end class template visible_instruction

} // end namespace detail

//--------------------------------------------------------------------------------------------------


enum class memory_operation
{
   Load = 0,
   Store = 1,
   ReadModifyWrite = 5
};

//--------------------------------------------------------------------------------------------------


namespace detail {

template <typename thread_id_t, typename memory_location_t>
class memory_instruction
: public visible_instruction<thread_id_t, memory_operation, memory_location_t>
{
public:
   using base_type = visible_instruction<thread_id_t, memory_operation, memory_location_t>;

   /// @brief Default constructor.
   /// @note Required for istream right shift operator.

   memory_instruction()
   : base_type()
   , m_is_atomic(false)
   {
   }

   memory_instruction(const thread_id_t& tid, const memory_operation& operation,
                      const memory_location_t& operand, bool is_atomic,
                      const meta_data_t& meta_data = {"unknown", 0})
   : base_type(tid, operation, operand, meta_data)
   , m_is_atomic(is_atomic)
   {
   }

   bool is_atomic() const { return m_is_atomic; }

private:
   bool m_is_atomic;

}; // end class memory_instruction

} // end namespace detail

//--------------------------------------------------------------------------------------------------


enum class lock_operation
{
   Lock = 3,
   Unlock = 2
};

//--------------------------------------------------------------------------------------------------


namespace detail {

template <typename thread_id_t, typename memory_location_t>
class lock_instruction : public visible_instruction<thread_id_t, lock_operation, memory_location_t>
{
public:
   using base_type = visible_instruction<thread_id_t, lock_operation, memory_location_t>;

   /// @brief Default constructor.
   /// @note Required for istream right shift operator.

   lock_instruction()
   : base_type()
   {
   }

   lock_instruction(const thread_id_t& tid, const lock_operation& operation,
                    const memory_location_t& operand, const meta_data_t& meta_data = {"unknown", 0})
   : base_type(tid, operation, operand, meta_data)
   {
   }

}; // end class lock_instruction

//--------------------------------------------------------------------------------------------------


template <typename thread_id_t, typename memory_location_t>
using visible_instruction_t = boost::variant<memory_instruction<thread_id_t, memory_location_t>,
                                             lock_instruction<thread_id_t, memory_location_t>>;

//--------------------------------------------------------------------------------------------------

template <typename thread_id_t, typename memory_location_t>
struct get_tid : public boost::static_visitor<thread_id_t>
{
   template <typename operation_t>
   using visible_instruction = visible_instruction<thread_id_t, operation_t, memory_location_t>;

   template <typename operation_t>
   const thread_id_t& operator()(const visible_instruction<operation_t>& instruction) const
   {
      return instruction.tid();
   }
}; // end struct get_tid

//--------------------------------------------------------------------------------------------------

template <typename thread_id_t, typename memory_location_t>
struct operation_as_int : public boost::static_visitor<int>
{
   template <typename operation_t>
   using visible_instruction = visible_instruction<thread_id_t, operation_t, memory_location_t>;

   template <typename operation_t>
   int operator()(const visible_instruction<operation_t>& instruction) const
   {
      return static_cast<int>(instruction.operation());
   }
}; // end struct operation_as_int

//--------------------------------------------------------------------------------------------------

template <typename thread_id_t, typename memory_location_t>
struct get_operand : public boost::static_visitor<memory_location_t>
{
   template <typename operation_t>
   using visible_instruction = visible_instruction<thread_id_t, operation_t, memory_location_t>;

   template <typename operation_t>
   memory_location_t operator()(const visible_instruction<operation_t>& instruction) const
   {
      return instruction.operand();
   }
}; // end struct get_operand

//--------------------------------------------------------------------------------------------------

template <typename thread_id_t, typename memory_location_t>
struct get_meta_data : public boost::static_visitor<meta_data_t>
{
   template <typename operation_t>
   using visible_instruction = visible_instruction<thread_id_t, operation_t, memory_location_t>;

   template <typename operation_t>
   const meta_data_t& operator()(const visible_instruction<operation_t>& instruction) const
   {
      return instruction.meta_data();
   }
}; // end struct get_meta_data

} // end namespace detail

//--------------------------------------------------------------------------------------------------


template <typename thread_id_t, typename memory_location_t>
bool operator==(const detail::memory_instruction<thread_id_t, memory_location_t>& lhs,
                const detail::memory_instruction<thread_id_t, memory_location_t>& rhs)
{
   return lhs.tid() == rhs.tid() && lhs.operation() == rhs.operation() &&
          lhs.operand() == rhs.operand() && lhs.is_atomic() == rhs.is_atomic() &&
          lhs.meta_data() == rhs.meta_data();
}

//--------------------------------------------------------------------------------------------------

template <typename thread_id_t, typename memory_location_t>
bool operator==(const detail::lock_instruction<thread_id_t, memory_location_t>& lhs,
                const detail::lock_instruction<thread_id_t, memory_location_t>& rhs)
{
   return lhs.tid() == rhs.tid() && lhs.operation() == rhs.operation() &&
          lhs.operand() == rhs.operand() && lhs.meta_data() == rhs.meta_data();
}

//--------------------------------------------------------------------------------------------------


// Type definitions
using memory_instruction = detail::memory_instruction<Thread::tid_t, Object>;
using lock_instruction = detail::lock_instruction<Thread::tid_t, Object>;

using visible_instruction_t = detail::visible_instruction_t<Thread::tid_t, Object>;

using get_tid = detail::get_tid<Thread::tid_t, Object>;
using operation_as_int = detail::operation_as_int<Thread::tid_t, Object>;
using get_operand = detail::get_operand<Thread::tid_t, Object>;
using get_meta_data = detail::get_meta_data<Thread::tid_t, Object>;

//--------------------------------------------------------------------------------------------------

} // end namespace program_model
