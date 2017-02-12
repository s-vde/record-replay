#!/bin/bash

here=`pwd .`

record_replay_base=`echo $1`

pass_build=${record_replay_base}/build/src/llvm-pass/LLVMRecordReplayPass.dylib
pass_name="-instrument-record-replay-lw"
scheduler_build=${record_replay_base}/build/src/scheduler/libRecordReplayScheduler.dylib

llvm_base=`echo $2`
llvm_bin=${llvm_base}/build/bin

cpp_utils_base=`echo $3`

input_program=`echo $4`
input_filename=$(basename "${input_program}")
input_extension="${input_filename##*.}"

output_dir=`echo $5`

custom_strategies=`echo $6`
if [ -z "${custom_strategies// }" ]; then
   custom_strategies=${record_replay_base}/src/scheduler/strategies/custom
fi

###
echo Instrumenting ${input_program}
###

compiler_flags="-pthread -emit-llvm "

if [ ${input_extension} == "c" ]; then
   compiler=${llvm_bin}/clang
else
   compiler=${llvm_bin}/clang++
   compiler_flags+="-std=c++14 "
fi

test -d ${record_replay_base}/build || mkdir ${record_replay_base}/build
cd ${record_replay_base}/build/
cmake ../ \
   -DLLVM_DIR=${llvm_base}/build/lib/cmake/llvm \
   -DPROGRAM_MODEL_SRC=${record_replay_base}/src/program-model \
   -DCUSTOM_STRATEGIES=${custom_strategies} \
   -DUTILS_SRC=${cpp_utils_base}/src
make
cd ${here}

test -d ${output_dir} || mkdir -p ${output_dir}
${compiler} ${compiler_flags} -c ${input_program} -o ${output_dir}/${input_filename}.bc
${llvm_bin}/opt -S -load ${pass_build} ${pass_name} < ${output_dir}/${input_filename}.bc > ${output_dir}/${input_filename}.instrumented.bc
${compiler} ${output_dir}/${input_filename}.instrumented.bc \
   ${scheduler_build} -rpath ${record_replay_base}/build/src/scheduler/ \
   -o ${output_dir}/"${input_filename%.*}"
