
#ifndef EXECUTION_IO_HPP_INCLUDED
#define EXECUTION_IO_HPP_INCLUDED

#include <iostream>
#include "execution.hpp"
#include "transition_io.hpp"

/*---------------------------------------------------------------------------75*/
/**
 @file execution_io.hpp
 @brief Declarations and definitions of input/output functions for Execution<State>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace program_model
{
    std::string to_string(const ExecutionBase::Status&);
    std::ostream& operator<<(std::ostream&, const ExecutionBase::Status&);
    std::istream& operator>>(std::istream&, ExecutionBase::Status&);
    
    template<typename State>
    std::string to_short_string(const Execution<State>& E)
    {
        std::string str("");
        for (const auto& trans : E) {
            str += program_model::to_short_string(trans);
            str += "\n";
        }
        str += to_string(E.status());
        return str;
    }
    
    template<typename State>
    std::ostream& operator<<(std::ostream& os, const Execution<State>& E)
    {
        if (E.initialized()) {
            os << E.s0() << std::endl;
            for (const auto& trans : E) {
                os << to_string_post(trans) << std::endl;
            }
            os << "=====" << std::endl << E.status();
        }
        return os;
    }
    
    /**
     @brief Extends E with Transitions read from the given std::istream is,
     skipping the first E.size() read Transitions.
     */
    template<typename State>
    std::istream& operator>>(std::istream& is, Execution<State>& E)
    {
        int i = 0;
        while (true)
        {
            // try read E.status
            if ((is >> std::ws).peek() == std::char_traits<char>::to_int_type('=')) {
                utils::io::skip(is, '\n', 1);
                ExecutionBase::Status status;
                is >> status;
                E.set_status(status);
                break;
            }
            // read State s0
            if (i == 0) {
                // set E.s0
                if (!E.initialized()) {
                    if (!(is >> E.mS0)) { break; }
                // ignore
                } else if (!utils::io::skip(is, '\n', 1)) { break; }
            }
            // read Transition
            if (i == E.size()) {
                int index;
                Instruction instr;
                is >> index >> instr;
                std::shared_ptr<State> post{};
                if (is >> post) { E.push_back(instr, post); }
                else { break; }
            // skip Transition
            } else if (!utils::io::skip(is, '\n', 2)) { break; }
            ++i;
        }
        return is;
    }
} // end namespace program_model

#endif
