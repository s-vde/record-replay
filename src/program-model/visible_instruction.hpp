#pragma once

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

//--------------------------------------------------------------------------------------------------

template <typename operation_type, typename memory_location_t>
class visible_instruction
{
public:
   using operation_t = operation_type;

   visible_instruction(const operation_t& operation, const memory_location_t& operand)
   : m_operation(operation)
   , m_operand(operand)
   , m_meta_data({"unknown", 0})
   {
   }

   const operation_t& operation() const { return m_operation; }
   const memory_location_t& operand() const { return m_operand; }
   void add_meta_data(const meta_data_t& meta_data) { m_meta_data = meta_data; }
   const meta_data_t& meta_data() const { return m_meta_data; }
private:
   operation_t m_operation;
   memory_location_t m_operand;
   meta_data_t m_meta_data;

}; // end class template visible_instruction

//--------------------------------------------------------------------------------------------------

enum class memory_operation
{
   Load = 0,
   Store = 1,
   ReadModifyWrite = 5
};

template <typename memory_location_t>
class memory_instruction : public visible_instruction<memory_operation, memory_location_t>
{
public:
   memory_instruction(const memory_operation& operation, const memory_location_t& operand,
                      bool is_atomic)
   : visible_instruction<memory_operation, memory_location_t>(operation, operand)
   , m_is_atomic(is_atomic)
   {
   }

   bool is_atomic() const { return m_is_atomic; }
private:
   bool m_is_atomic;

}; // end class memory_instruction

//--------------------------------------------------------------------------------------------------

enum class lock_operation
{
   Lock = 3,
   Unlock = 2
};

template <typename memory_location_t>
class lock_instruction : public visible_instruction<lock_operation, memory_location_t>
{
public:
   lock_instruction(const lock_operation& operation, const memory_location_t& operand)
   : visible_instruction<lock_operation, memory_location_t>(operation, operand)
   {
   }

}; // end class lock_instruction

//--------------------------------------------------------------------------------------------------

template <typename memory_location_t>
using visible_instruction_t =
   boost::variant<memory_instruction<memory_location_t>, lock_instruction<memory_location_t>>;

//--------------------------------------------------------------------------------------------------

template <typename memory_location_t>
struct operation_as_int : public boost::static_visitor<int>
{
   template <typename operation_t>
   int operator()(const visible_instruction<operation_t, memory_location_t>& instruction) const
   {
      return static_cast<int>(instruction.operation());
   }
}; // end struct operation_as_int

//--------------------------------------------------------------------------------------------------

template <typename memory_location_t>
struct get_meta_data : public boost::static_visitor<meta_data_t>
{
   template <typename operation_t>
   const meta_data_t& operator()(
      const visible_instruction<operation_t, memory_location_t>& instruction) const
   {
      return instruction.meta_data();
   }
}; // end struct get_meta_data

} // end namespace program_model
