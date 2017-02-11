cmake_minimum_required(VERSION 3.5)

project(record_replay_scheduler)

set(CMAKE_CXX_FLAGS "-install_name @rpath/libRecordReplayScheduler.dylib")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MACOSX_RPATH 1)

include_directories(${UTILS_SRC})
include_directories(${PROGRAM_MODEL_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/strategies)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/strategies/custom)	
 
add_library(RecordReplayScheduler SHARED 
  ${UTILS_SRC}/binary_sem.cpp
  ${UTILS_SRC}/utils_io.cpp
  ${PROGRAM_MODEL_SRC}/execution.cpp
  ${PROGRAM_MODEL_SRC}/execution_io.cpp
  ${PROGRAM_MODEL_SRC}/instruction.cpp
  ${PROGRAM_MODEL_SRC}/instruction_io.cpp
  ${PROGRAM_MODEL_SRC}/object.cpp
  ${PROGRAM_MODEL_SRC}/object_io.cpp
  ${PROGRAM_MODEL_SRC}/state.cpp
  ${PROGRAM_MODEL_SRC}/state_io.cpp
  ${PROGRAM_MODEL_SRC}/thread.cpp
  ${PROGRAM_MODEL_SRC}/thread_io.cpp
  ${PROGRAM_MODEL_SRC}/transition.cpp
  ${PROGRAM_MODEL_SRC}/transition_io.cpp
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
  strategies/custom/custom_selector_register.cpp
)
