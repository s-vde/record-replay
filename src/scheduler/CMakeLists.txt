cmake_minimum_required(VERSION 3.5)

project(record_replay_scheduler)

set(CMAKE_CXX_FLAGS "-install_name @rpath/libRecordReplayScheduler.dylib")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MACOSX_RPATH 1)


####################
# DEPENDENCIES

set(PROGRAM_MODEL   ${CMAKE_CURRENT_SOURCE_DIR}/../program-model)
include_directories(${PROGRAM_MODEL})

include_directories(${CPP_UTILS}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/strategies)

if(NOT DEFINED CUSTOM_STRATEGIES)
	set(CUSTOM_STRATEGIES   ${CMAKE_CURRENT_SOURCE_DIR}/strategies/custom)
endif(NOT DEFINED CUSTOM_STRATEGIES)
message(STATUS "Using CUSTOM_STRATEGIES ${CUSTOM_STRATEGIES}")

add_subdirectory(${CUSTOM_STRATEGIES} ${CMAKE_CURRENT_BINARY_DIR}/selection-strategies)
include_directories(${CUSTOM_STRATEGIES})


####################
# LIBRARY

add_library(RecordReplayScheduler SHARED
  ${CPP_UTILS}/src/binary_sem.cpp
  ${CPP_UTILS}/src/utils_io.cpp
  ${PROGRAM_MODEL}/execution.cpp
  ${PROGRAM_MODEL}/execution_io.cpp
  ${PROGRAM_MODEL}/instruction.cpp
  ${PROGRAM_MODEL}/instruction_io.cpp
  ${PROGRAM_MODEL}/object.cpp
  ${PROGRAM_MODEL}/object_io.cpp
  ${PROGRAM_MODEL}/state.cpp
  ${PROGRAM_MODEL}/state_io.cpp
  ${PROGRAM_MODEL}/thread.cpp
  ${PROGRAM_MODEL}/thread_io.cpp
  ${PROGRAM_MODEL}/transition.cpp
  ${PROGRAM_MODEL}/transition_io.cpp
  ${PROGRAM_MODEL}/visible_instruction_io.cpp
  concurrency_error.cpp
  control.cpp
  object_state.cpp
  program.cpp
  replay.cpp
  schedule.cpp
  scheduler_settings.cpp
  scheduler.cpp
  task_pool.cpp
  strategies/non_preemptive.cpp
  strategies/random.cpp
  strategies/selector_register.cpp
)


####################
# LINKING

target_link_libraries(RecordReplayScheduler CustomSelectionStrategies)
