#pragma once

// PROGRAM_MODEL
#include "instruction.hpp"

// STL
#include <array>
#include <unordered_map>
#include <vector>

//--------------------------------------------------------------------------------------90
/// @file object_state.hpp
/// @author Susanne van den Elsen
/// @date 2015-2016
//----------------------------------------------------------------------------------------

namespace scheduler
{
   //-------------------------------------------------------------------------------------
   
   class object_state;  // forward declaration
   
   //-------------------------------------------------------------------------------------
   
   namespace data_race
   {
      using instruction_t = program_model::Instruction;
      using type = std::pair<instruction_t, instruction_t>;
      using vector_t = std::vector<type>;
      
      /// @brief Returns the set of instructions posted for the given object that form
      /// a data race with the given instruction.
      
      vector_t get_data_races(const object_state& object, const instruction_t& instr);
      
   } // end namespace data_race
   
   //-------------------------------------------------------------------------------------
   
   class object_state
   {
   public:
      
      using thread_t = program_model::Thread;
      using instruction_t = program_model::Instruction;
      using object_t = program_model::Object;
      using waitset_t = std::unordered_map<thread_t::tid_t, instruction_t>;
      
      /// @brief Constructor.
      
      explicit object_state(const object_t& object);
      
      unsigned int request(const instruction_t& instr);
      void perform(const thread_t::tid_t& tid);
      
      waitset_t::const_iterator begin(std::size_t index) const;
      waitset_t::const_iterator end(std::size_t index) const;
      
      std::string str() const;
      
   private:
      
      object_t m_object;
      std::array<waitset_t,2> m_waiting;
      unsigned int m_current;
      
      friend std::ostream& operator<<(std::ostream&, const object_state&);
      
   }; // end class object
   
   //-------------------------------------------------------------------------------------
   
   std::ostream& operator<<(std::ostream&, const object_state&);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace scheduler
