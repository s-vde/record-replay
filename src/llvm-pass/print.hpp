#pragma once

#include <llvm/Support/raw_ostream.h>

//--------------------------------------------------------------------------------------------------
/// @file print.hpp
/// @brief Print macros using llvm::errs.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


#define _PRINT
#ifdef _PRINT
#   define PRINT(x) do { llvm::errs() << x; } while (false)
#   define PRINTF(class, method, args, other) do { PRINT(class << "::" << method << "(" << args << ")\t" << other); } while (false)
#else
#   define PRINT(x) do { } while (false)
#   define PRINTF(class, method, args, other) do { } while (false)
#endif
