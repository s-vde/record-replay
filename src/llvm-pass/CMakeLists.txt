
include_directories(${UTILS_SRC})
include_directories(${PROGRAM_MODEL_SRC})

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
