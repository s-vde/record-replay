cmake_minimum_required(VERSION 3.5)

project(record_replay_program_model)

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

include_directories(${CPP_UTILS}/src)


####################
# LIBRARY

add_library(RecordReplayProgramModel STATIC
  ${CPP_UTILS}/src/utils_io.cpp
  execution.cpp
  execution_io.cpp
  object.cpp
  object_io.cpp
  state.cpp
  state_io.cpp
  thread.cpp
  thread_io.cpp
  transition.cpp
  transition_io.cpp
  visible_instruction_io.cpp
)
