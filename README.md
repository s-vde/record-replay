# record-replay
LLVM Instrumentation Pass and Scheduler for recording the execution of multithreaded programs and
replaying them under a given thread interleaving.

### General
In what follows, `[project-root]` refers to the root directory of *record-replay* when used stand-alone, 
or the root directory of the project including *record-replay*. In both cases `RECORD_REPLAY` refers to
the root directory of *record-replay*.

### Instalation

###### Build LLVM
The Instrumentation Pass needs to be built against the LLVM Pass Framework.
It is tested with LLVM and Clang version 4.0.
Scripts for downloading/cloning and building the LLVM Suite, the Clang frontend 
and required tools with the right configuration can be found in the repository
[llvm-pass-development-scripts](https://github.com/s-vde/llvm-pass-development-scripts).
After building LLVM, Clang and the required tools, variables in `Makefile.config` need to be 
overridden depending on the build location and settings.

###### Build Instrumentation Pass
In order to make the Instrumentation Pass run

`make pass`

from `[project-root]`. This will build the pass outside of the LLVM source tree.

### Instrumenting a program
In order to instrument an input program, run

`make instrument DIR=[path_to_dir_containing_source_file] PROGRAM=[source_file_without_ext]`

from `[project-root]`. The instrumented program will be outut as `DIR/instrumented/PROGRAM`.
