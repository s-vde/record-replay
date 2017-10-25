# Record-Replay
LLVM Instrumentation Pass and Scheduler for recording the execution of multithreaded programs and
replaying them under a given thread interleaving.

---

## Cloning and Building Record-Replay

#### Cloning the Repository

Record-Replay uses [cpp-utils](https://github.com/s-vde/cpp-utils) as a submodule. Therefore, clone recursively:

```
git clone --recursive https://github.com/s-vde/record-replay.git
```

#### Build LLVM
The Instrumentation Pass needs to be built against the LLVM Pass Framework.
It is currently tested with LLVM and Clang version 4.0.
Scripts for downloading/cloning and building the LLVM Suite, the Clang frontend
and required tools with the right configuration can be found in the repository
[llvm-pass-development-scripts](https://github.com/s-vde/llvm-pass-development-scripts).

#### Build Record-Replay

Once you have a build of the LLVM Pass Framework, Record-Replay can be built using cmake. By default the build process will look for and use an installed LLVM. A custom built LLVM can be specified as follows:

```
cmake -DLLVM_BUILD_DIR=<path_to_llvm_build_dir>
```

---

## Instrumenting a Program using the API

The API for instrumenting a program is provided in ```src/scheduler/replay.hpp``` is as follows:

```
boost::filesystem::path instrument(const program_t& program_source,
                                   const boost::filesystem::path& output_dir,
                                   const std::string& optimization_level = "0",
                                   const std::string& compiler_options = "");
```

The returned path is the path to the instrumented executable.

---

## Running the Instrumented Program
When the instrumented program `<output_dir>/<input_program.filename>` is run, it expects the following files (relative to the place from where it is run):
- `schedules/schedule.txt`: containing the schedule under which the program is to be run (e.g. `<0,0,1,1>`)
- `schedules/settings.txt`: containing the name of the strategy for selecting the next thread, if not by schedule. The builtin strategies are `Random` and `NonPreemptive`.
