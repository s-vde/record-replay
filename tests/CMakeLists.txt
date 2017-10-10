cmake_minimum_required(VERSION 3.5)

project(record_replay_scheduler_test)

set(CMAKE_CXX_STANDARD 14)


####################
# DEPENDENCIES

set(PROGRAM_MODEL   ${CMAKE_CURRENT_SOURCE_DIR}/../src/program-model)
include_directories(${PROGRAM_MODEL})

set(SCHEDULER   ${CMAKE_CURRENT_SOURCE_DIR}/../src/scheduler)
include_directories(${SCHEDULER})

include_directories(${CPP_UTILS}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/program_model)


####################
# LIBRARY

add_executable(RecordReplayTest
  ${CPP_UTILS}/src/utils_io.cpp
  ${SCHEDULER}/replay.cpp
  ${SCHEDULER}/scheduler_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_TEST.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main_TEST.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/program_model/execution_io_TEST.cpp
)


####################
# COMPILE DEFINITIONS

target_compile_definitions(RecordReplayTest PRIVATE "LLVM_BIN=${LLVM_BUILD_DIR}/bin")
target_compile_definitions(RecordReplayTest PRIVATE "RECORD_REPLAY_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}/..")
target_compile_definitions(RecordReplayTest PRIVATE "TESTS_BUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions(RecordReplayTest PRIVATE "TEST_PROGRAMS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_programs")


####################
# LINKING

target_link_libraries(RecordReplayTest RecordReplayProgramModel ${Boost_LIBRARIES})
