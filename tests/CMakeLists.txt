cmake_minimum_required(VERSION 3.5)

project(record_replay_scheduler_test)

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

set(PROGRAM_MODEL   ${CMAKE_CURRENT_SOURCE_DIR}/../src/program-model)
include_directories(${PROGRAM_MODEL})

include_directories(${CPP_UTILS}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/program_model)


####################
# LIBRARY

add_executable(RecordReplayTest
  ${CPP_UTILS}/src/utils_io.cpp
  ${PROGRAM_MODEL}/execution.cpp
  ${PROGRAM_MODEL}/execution_io.cpp
  ${PROGRAM_MODEL}/object.cpp
  ${PROGRAM_MODEL}/object_io.cpp
  ${PROGRAM_MODEL}/state.cpp
  ${PROGRAM_MODEL}/state_io.cpp
  ${PROGRAM_MODEL}/thread.cpp
  ${PROGRAM_MODEL}/thread_io.cpp
  ${PROGRAM_MODEL}/transition.cpp
  ${PROGRAM_MODEL}/transition_io.cpp
  ${PROGRAM_MODEL}/visible_instruction_io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/program_model/execution_io_TEST.cpp
)
