
include_directories(${UTILS})
include_directories(${PROGRAM_MODEL})

add_llvm_loadable_module(LLVMRecordReplay
  ${UTILS}/color_output.cpp
  ${UTILS}/utils_io.cpp
  ${PROGRAM_MODEL}/object_io.cpp
  ${PROGRAM_MODEL}/object.cpp
  ${PROGRAM_MODEL}/visible_instruction.hpp
  ${PROGRAM_MODEL}/visible_instruction_io.cpp
  functions.cpp
  instrumentation_utils.cpp
  llvm_visible_instruction.cpp
  pass.cpp
)
