#!/bin/bash

here=`pwd .`

record_replay_base=`echo $1`

pass_build=${record_replay_base}/build/src/llvm-pass/LLVMRecordReplayPass.dylib
pass_name="-instrument-record-replay-lw"
scheduler_build=${record_replay_base}/build/src/scheduler/libRecordReplayScheduler.dylib

llvm_base=`echo $2`
llvm_bin=${llvm_base}/build/bin

input_program=`echo $3`
input_filename=$(basename "${input_program}")
input_extension="${input_filename##*.}"

output_dir=`echo $4`

compiler_flags="-pthread -emit-llvm "

if [ ${input_extension} == "c" ]; then
   compiler=${llvm_bin}/clang
else
   compiler=${llvm_bin}/clang++
   compiler_flags+="-std=c++14 "
fi

cd build/
cmake ../
make
cd ${here}

test -d ${output_dir} || mkdir -p ${output_dir}
${compiler} ${compiler_flags} -c ${input_program} -o ${output_dir}/${input_filename}.bc
${llvm_bin}/opt -S -load ${pass_build} ${pass_name} < ${output_dir}/${input_filename}.bc > ${output_dir}/${input_filename}.instrumented.bc
${compiler} ${output_dir}/${input_filename}.instrumented.bc \
   ${scheduler_build} -rpath ${record_replay_base}/build/src/scheduler/ \
   -o ${output_dir}/"${input_filename%.*}"
