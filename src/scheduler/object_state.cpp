
#include "object_state.hpp"

// PROGRAM_MODEL
#include "instruction_io.hpp"

// UTILS
#include "debug.hpp"
#include "utils_io.hpp"

// STL
#include <algorithm>
#include <exception>

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   namespace data_race
   {
      //----------------------------------------------------------------------------------
      
      bool is_a_memory_operation(const program_model::Object::Op& operation)
      {
         return
            static_cast<int>(operation) == 0 ||
            static_cast<int>(operation) == 1 ||
            static_cast<int>(operation) == 5;
      }
      
      //----------------------------------------------------------------------------------
      
      vector_t get_data_races(const object_state& object, const instruction_t& instr)
      {
         auto convert_to_race = [&instr] (const auto& entry)
         {
            return data_race::type{entry.second, instr};
         };
         vector_t data_races;
         if (is_a_memory_operation(instr.op()))
         {
            if (instr.op() == program_model::Object::Op::WRITE ||
                instr.op() == program_model::Object::Op::RMW)
            {
               std::transform(object.begin(0), object.end(0), std::back_inserter(data_races),
                              convert_to_race);
            }
            std::transform(object.begin(1), object.end(1), std::back_inserter(data_races),
                            convert_to_race);
         }
         return data_races;
      }
      
      //----------------------------------------------------------------------------------
      
   } // end namespace data_race
   
   //-------------------------------------------------------------------------------------
   
   object_state::object_state(const object_t& object)
   : m_object(object)
   , m_waiting{{{},{}}}
   , m_current(0)
   {}

   //-------------------------------------------------------------------------------------
   
   unsigned int object_state::request(const instruction_t& instr)
   {
      DEBUGF_SYNC("object", "request", instr, "\n");
      const auto op_type = static_cast<unsigned int>(instr.op()) % 2;
      auto& waitset = m_waiting[op_type];
      if (waitset.find(instr.tid()) == waitset.end())
      {
         waitset.insert({instr.tid(), instr});
         DEBUG_SYNC(*this << "\n");
         return m_current;
      }
      throw std::logic_error("requesting thread already has instruction waiting");
   }
   
   //-------------------------------------------------------------------------------------
   
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
   
   //-------------------------------------------------------------------------------------
   
   auto object_state::begin(std::size_t index) const -> waitset_t::const_iterator
   {
      return m_waiting[index].begin();
   }
   
   //-------------------------------------------------------------------------------------
   
   auto object_state::end(std::size_t index) const -> waitset_t::const_iterator
   {
      return m_waiting[index].end();
   }
   
   //-------------------------------------------------------------------------------------
   
   std::string object_state::str() const
   {
      return to_pretty_string(m_object);
   }
   
   //-------------------------------------------------------------------------------------
   
   std::ostream& operator<<(std::ostream& os, const object_state& object)
   {
      os << "object(object=" << to_pretty_string(object.m_object) << " waiting[0]={";
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
   
   //-------------------------------------------------------------------------------------
   
} // end namespace scheduler