cmake_minimum_required(VERSION 3.5)


####################
# FLAGS

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-undefined dynamic_lookup")
endif()


####################
# LLVM ENVIRONMENT

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


####################
# DEPENDENCIES

set(PROGRAM_MODEL   ${CMAKE_CURRENT_SOURCE_DIR}/../program-model)
include_directories(${PROGRAM_MODEL})

include_directories(${CPP_UTILS}/src)


####################
# LOADABLE MODULE

add_llvm_loadable_module(LLVMRecordReplayPass
  ${CPP_UTILS}/src/color_output.cpp
  ${CPP_UTILS}/src/utils_io.cpp
  ${PROGRAM_MODEL}/object_io.cpp
  ${PROGRAM_MODEL}/object.cpp
  ${PROGRAM_MODEL}/visible_instruction.hpp
  ${PROGRAM_MODEL}/visible_instruction_io.cpp
  functions.cpp
  instrumentation_utils.cpp
  llvm_visible_instruction.cpp
  RecordReplayPass.cpp
  VisibleInstructionPass.cpp
)
