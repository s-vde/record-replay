# record-replay
LLVM Instrumentation Pass and Scheduler for recording the execution of multithreaded programs and
replaying them under a given thread interleaving.

### Dependencies
Both the Instrumentation Pass and the Scheduler use c++ utility functions found in the repository
[cpp-utils](https://github.com/s-vde/cpp-utils).

### Build LLVM
The Instrumentation Pass needs to be built against the LLVM Pass Framework.
It is currently tested with LLVM and Clang version 4.0.
Scripts for downloading/cloning and building the LLVM Suite, the Clang frontend
and required tools with the right configuration can be found in the repository
[llvm-pass-development-scripts](https://github.com/s-vde/llvm-pass-development-scripts).

### Instrumenting a program
In order to instrument an input program, run

`source <path_to_record_replay>/instrument.sh <abs_path_to_record_replay> <path_to_llvm> <path_to_cpp_utils> <path_to_input_program> <path_to_output_dir>`

### Running the instrumented program
When the instrumented program `<output_dir>/<program_name>` is run, it expects the following files (relative to the place from where it is run):
- `schedules/schedule.txt`: containing the schedule under which the program is to be run (e.g. `<0,0,1,1>`)
- `schedules/threads.txt`: containing the number of threads in the program
- `schedules/settings.txt`: containing the name of the strategy for selecting the next thread, if not by schedule. The builtin strategies are `Random` and `NonPreemptive`.
