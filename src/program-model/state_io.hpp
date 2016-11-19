#pragma once

// STL
#include <iosfwd>	// forward declaration of std::ostream and std::istream
#include <memory>

//--------------------------------------------------------------------------------------90
/// @file state_io.hpp
/// @brief Input/output stream operator overloads for State.
/// @author Susanne van den Elsen
/// @date 2015
//----------------------------------------------------------------------------------------

namespace program_model
{
   //-------------------------------------------------------------------------------------
   
	// forward declarations
	struct next_t;
	class State;
   
   //-------------------------------------------------------------------------------------
	
   std::istream& operator>>(std::istream&, next_t&);
   std::ostream& operator<<(std::ostream&, const next_t&);
   
   //-------------------------------------------------------------------------------------
    
   std::istream& operator>>(std::istream&, State&);
   std::istream& operator>>(std::istream&, std::shared_ptr<State>&);
   std::ostream& operator<<(std::ostream&, const State&);
   
   //-------------------------------------------------------------------------------------
    
} // end namespace program_model
