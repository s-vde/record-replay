cmake_minimum_required(VERSION 3.5)

####################
# FLAGS

set(CMAKE_CXX_STANDARD 14)


####################
# VARIABLES

set(LLVM_BIN                    ${LLVM_BUILD_DIR}/bin)
set(LLVM_BIN                    ${LLVM_BIN}                     PARENT_SCOPE)
set(RECORD_REPLAY_BUILD_DIR     ${CMAKE_CURRENT_BINARY_DIR})
set(RECORD_REPLAY_BUILD_DIR     ${RECORD_REPLAY_BUILD_DIR}      PARENT_SCOPE)


####################
# DEPENDENCIES

find_package(Boost COMPONENTS filesystem system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(NOT DEFINED GOOGLE_TEST)
   set(GOOGLE_TEST   ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest)
   add_subdirectory(${GOOGLE_TEST}/googletest)
endif(NOT DEFINED GOOGLE_TEST)

set(LLVM_DIR    ${LLVM_BUILD_DIR}/lib/cmake/llvm)

if(NOT DEFINED CPP_UTILS)
	set(CPP_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/libs/cpp-utils)
   add_subdirectory(${CPP_UTILS}/tests)
endif(NOT DEFINED CPP_UTILS)
message(STATUS "Using CPP_UTILS ${CPP_UTILS}")

add_subdirectory(src/llvm-pass)
add_subdirectory(src/program-model)
add_subdirectory(src/scheduler)
add_subdirectory(tests)
