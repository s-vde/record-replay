
#ifndef STATE_HPP_INCLUDED
#define STATE_HPP_INCLUDED

#include <unordered_map>
#include "instruction.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file state.hpp
 @brief Definition of class State and struct next_t and type alias NextSet.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    struct next_t
    {
        Instruction instr;
        bool enabled;
        
    }; // end struct next_t
    
    using NextSet = std::unordered_map<Thread::tid_t,next_t>;
    
    class State
    {
    public:
        
        // TYPES
        
        using SharedPtr = std::shared_ptr<State>;
        
        // CTORS / DTOR
        
        State(const Tids& enabled, const NextSet& next);
        
        State(const State&) = default;
        State(State&&) = default;
        ~State() = default;
        
        // OPERATORS
        
        State& operator=(const State&) = default;
        State& operator=(State&&) = default;

        //
        
        std::string tag() const;
        
        const Tids& enabled() const;
		bool is_enabled(const Thread::tid_t& tid) const;
        
        const NextSet::const_iterator next(const Thread::tid_t& tid) const;
        NextSet::const_iterator next_cbegin() const;
        NextSet::const_iterator next_cend() const;
		bool has_next(const Thread::tid_t& tid) const;

    private:
        
        // DATA MEMBERS
        
        static const std::string mTag;
        
        Tids mEnabled;
        
        /**
         @brief <code>{(next_{this}(p),enabled(this,p)) | p in Tids }</code>.
         */
        NextSet mNext;
    
    //friendly:
        
        friend std::ostream& operator<<(std::ostream&, const State&);
        friend std::istream& operator>>(std::istream&, State&);
        
    }; // end class State
} // end namespace program_model

#endif
