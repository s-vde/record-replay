# record-replay
LLVM Instrumentation Pass and Scheduler for recording the execution of multithreaded programs and
replaying them under a given thread interleaving.

### General
In what follows, `[project-root]` refers to the root directory of *record-replay* when used stand-alone, 
or the root directory of the project including *record-replay*. In both cases `RECORD_REPLAY` refers to
the root directory of *record-replay*.

### Instalation

###### Build LLVM 3.6.2
The Instrumentation Pass needs to be built in the context of the LLVM Pass Framework and is compliant 
with LLVM and Clang version 3.6.2. Sources for the LLVM suite, the Clang frontend, and libc++ can
be downloaded from [the LLVM download page](http://llvm.org/releases/download.html) and a quick setup
is described at [Getting Started](http://llvm.org/releases/3.6.2/docs/GettingStarted.html). After building
LLVM and Clang, variables in `Makefile.config` need to be overridden depending on the build location and
settings.

###### Build Instrumentation Pass
In order to make the Instrumentation Pass run

`make pass`

from `[project-root]`. This will build the pass inside the LLVM source tree.

### Instrumenting a program
In order to instrument an input program, run

`make instrument DIR=[path_to_dir_containing_source_file] PROGRAM=[source_file_without_ext]`

from `[project-root]`. The instrumented program will be outut as `DIR/instrumented/PROGRAM`.
