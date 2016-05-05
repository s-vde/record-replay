# record-replay
LLVM Instrumentation Pass and Scheduler for recording the execution of multithreaded programs and replaying them under a given thread interleaving.

### General
In what follows, `[project-root]` refers to the root directory of *record-replay* when used stand-alone, 
or the root directory of the project including *record-replay*. In both cases `RECORD_REPLAY` refers to
the root directory of *record-replay*.

### Instalation

###### Build LLVM 3.6.2
The Instrumentation Pass needs to be build in the context of the LLVM Pass Framework and is compliant with LLVM version 3.6.2.
If you don't have a full build of LLVM 3.6.2 (with Clang 3.6.2), 
please download the sources for both LLVM and Clang from [the LLVM download page](http://llvm.org/releases/download.html), 
move the downloaded files to `RECORD_REPLAY`. 
From `[project-root]` run

`make build_llvm`

This will build LLVM in `RECORD_REPLAY/llvm` by default, but the destination can be overridden in Makefile.config (variable `LLVMBASE`).

###### Build Instrumentation Pass
In order to build the Instrumentation Pass run

`make pass`

from `[project-root]`. This will also compile the Scheduler.

### Instrumenting a program
In order to instrument an input program, run

`make instrument DIR=[path_to_dir_containing_source_file] PROGRAM=[source_file_without_ext]`

from `[record-replay-root]`. The instrumented program will be outut as `DIR/instrumented/[source_file_without_ext]`.
