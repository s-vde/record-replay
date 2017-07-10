cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-undefined dynamic_lookup")
endif()


####################
# DEPENDENCIES

if(NOT DEFINED CPP_UTILS)
	set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/libs/cpp-utils)
endif(NOT DEFINED CPP_UTILS)
message(STATUS "Using CPP_UTILS ${CPP_UTILS}")
	

add_subdirectory(src/llvm-pass)
add_subdirectory(src/scheduler)
