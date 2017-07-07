cmake_minimum_required(VERSION 3.5)


####################
# LLVM ENVIRONMENT

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${UTILS_SRC})
include_directories(${PROGRAM_MODEL_SRC})


####################
# LOADABLE MODULE

add_llvm_loadable_module(LLVMRecordReplayPass
  ${UTILS_SRC}/color_output.cpp
  ${UTILS_SRC}/utils_io.cpp
  ${PROGRAM_MODEL_SRC}/object_io.cpp
  ${PROGRAM_MODEL_SRC}/object.cpp
  ${PROGRAM_MODEL_SRC}/visible_instruction.hpp
  ${PROGRAM_MODEL_SRC}/visible_instruction_io.cpp
  functions.cpp
  instrumentation_utils.cpp
  llvm_visible_instruction.cpp
  RecordReplayPass.cpp
  VisibleInstructionPass.cpp
)
