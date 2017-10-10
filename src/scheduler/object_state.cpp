
#include "object_state.hpp"

#include <visible_instruction_io.hpp>

#include <debug.hpp>
#include <utils_io.hpp>

#include <algorithm>
#include <assert.h>
#include <exception>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

get_data_races::get_data_races(const object_state& object)
: m_object(object)
{
}

//--------------------------------------------------------------------------------------------------

std::vector<data_race_t> get_data_races::operator()(
   const program_model::memory_instruction& instruction)
{
   using namespace program_model;
   std::vector<data_race_t> data_races;
   auto convert_to_race = [&instruction](const auto& entry) {
      const auto* mem_instr = boost::get<memory_instruction>(&(entry.second));
      assert(mem_instr);
      return data_race_t{*mem_instr, instruction};
   };
   const auto operation = instruction.operation();
   if (operation == memory_operation::Store || operation == memory_operation::ReadModifyWrite)
   {
      std::transform(m_object.begin(0), m_object.end(0), std::back_inserter(data_races),
                     convert_to_race);
   }
   std::transform(m_object.begin(1), m_object.end(1), std::back_inserter(data_races),
                  convert_to_race);
   // filter the ones with two atomic operations out
   data_races.erase(std::remove_if(data_races.begin(), data_races.end(),
                                   [](const auto& data_race) {
                                      return data_race.first.is_atomic() &&
                                             data_race.second.is_atomic();
                                   }),
                    data_races.end());
   return data_races;
}

//--------------------------------------------------------------------------------------------------


object_state::object_state(const object_t& object)
: m_object(object)
, m_waiting{{{}, {}}}
, m_current(0)
{
}

//--------------------------------------------------------------------------------------------------

bool object_state::request(const instruction_t& instr)
{
   using namespace program_model;
   DEBUGF_SYNC("object", "request", instr, "\n");
   const auto op_type = boost::apply_visitor(operation_as_int(), instr) % 2;
   auto& waitset = m_waiting[op_type];
   const auto tid = boost::apply_visitor(program_model::get_tid(), instr);
   if (waitset.find(tid) == waitset.end())
   {
      waitset.insert({tid, instr});
      DEBUG_SYNC(*this << "\n");
      try
      {
         const auto& lock_instr = boost::get<lock_instruction>(instr);
         if (lock_instr.operation() == lock_operation::Lock && m_current == 1)
         {
            return false;
         }
      }
      catch (const boost::bad_get&)
      {
      }
      return true;
   }
   throw std::logic_error("requesting thread already has instruction waiting");
}

//--------------------------------------------------------------------------------------------------

void object_state::perform(const thread_t::tid_t& tid)
{
   DEBUGF_SYNC("object", "perform", tid, "\n");
   for (unsigned int i = 0; i < 2; ++i)
   {
      auto& waitset = m_waiting[i];
      const auto it = waitset.find(tid);
      if (it != waitset.end())
      {
         m_current = i;
         waitset.erase(it);
         DEBUG_SYNC(*this << "\n");
         return;
      }
   }
   throw std::invalid_argument("requesting thread has no instruction waiting");
}

//--------------------------------------------------------------------------------------------------

auto object_state::begin(std::size_t index) const -> waitset_t::const_iterator
{
   return m_waiting[index].begin();
}

//--------------------------------------------------------------------------------------------------

auto object_state::end(std::size_t index) const -> waitset_t::const_iterator
{
   return m_waiting[index].end();
}

//--------------------------------------------------------------------------------------------------

std::string object_state::str() const
{
   return utils::io::to_string(m_object);
}

//--------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const object_state& object)
{
   os << "object(object=" << utils::io::to_string(object.m_object) << " waiting[0]={";
   for (const auto& it : object.m_waiting[0])
   {
      os << utils::io::to_string(it.second) << " ";
   }
   os << "} waiting[1]={";
   for (const auto& it : object.m_waiting[1])
   {
      os << utils::io::to_string(it.second) << " ";
   }
   os << "}";
   return os;
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
