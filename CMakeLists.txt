cmake_minimum_required(VERSION 3.5)

####################
# FLAGS

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

if(NOT DEFINED CPP_UTILS)
	set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/libs/cpp-utils)
endif(NOT DEFINED CPP_UTILS)
message(STATUS "Using CPP_UTILS ${CPP_UTILS}")
	

####################
# SUBDIRECTORIES

add_subdirectory(src/llvm-pass)
add_subdirectory(src/scheduler)
add_subdirectory(tests)
