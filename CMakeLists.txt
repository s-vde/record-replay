cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-undefined dynamic_lookup")
endif()

add_subdirectory(src/llvm-pass)
add_subdirectory(src/scheduler)
