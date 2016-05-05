
#ifndef TYPES_HPP_INCLUDED
#define TYPES_HPP_INCLUDED

#include <set>
#include <unordered_map>
#include "llvm/IR/Module.h"

/*---------------------------------------------------------------------------75*/
/**
 @file functions.hpp
 @brief Definition of class Functions.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

namespace record_replay
{
    class Functions
    {
    public:
        
        // TYPES
		
        using FunctionMap = std::unordered_map<std::string,llvm::Function*>;
        
        // CTOR
        
        Functions();
        
        //
        
        bool initialize(const llvm::Module& M);
        
        llvm::Function* Wrapper_finish() const;
        llvm::Function* Wrapper_post_task() const;
        llvm::Function* Wrapper_spawn_thread() const;
        llvm::Function* Wrapper_wait_registered() const;
        llvm::Function* Wrapper_yield() const;
        llvm::Function* Function_pthread_create() const;
		llvm::Function* Scheduler_ctor() const;
		llvm::Function* Scheduler_dtor() const;
		
        bool blacklisted(llvm::Function* F) const;
        
    private:
        
        // DATA MEMBERS
        
        FunctionMap mWrappers;
        FunctionMap mStandard;
        std::set<std::string> mBlackListed;
		bool mSucceeded;

        // HELPER FUNCTIONS
        
		void register_wrapper(const llvm::Module& M, const std::string& name);
		
		void register_standard(const llvm::Module& M, const std::string& name);
		
    }; // end class Functions
} // end namespace record_replay

#endif
