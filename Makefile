.PHONY: clean_record_replay instrument instrument_dir

#===== Makefile ==============================================================80
#
#                               Record-Replay
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# This file is to be included by the Makefile of projects using Record-
# Replay. It includes the Makefiles of the subprojects program_model,
# llvm-pass and scheduler and defines a number of targets.
#
#===============================================================================

#======Variables =============================================================80
#
# User defined:
#   RECORD_REPLAY       : The root directory of Record-Replay (absolute path
#                         or relative path from including project.
#   DIR                 : Directory of the program to instrument.
#   PROGRAM             : Name of the program to instrument.
#
# Defined here:
#   record_replay_src   : The directory of Record-Replay source files.
#   LLVM_PASS           : The root directory of the LLVM-PASS subproject.
#   SCHEDULER           : The root directory of the SCHEDULER subproject.
#   PROGRAM_MODEL       : The root directory of the PROGRAM-MODEL subproject.
#   passname            : The name of the instrumentation pass.
#
#===============================================================================

RECORD_REPLAY         	 ?= .
record_replay_src     	 := $(RECORD_REPLAY)/src
PROGRAM_MODEL         	 := $(record_replay_src)/program-model
LLVM_PASS             	 := $(record_replay_src)/llvm-pass
SCHEDULER             	 := $(record_replay_src)/scheduler
passname              	 := -instrument-record-replay-lw
instrument_dir		 := $(DIR)/instrumented

#===== Include =====

include $(RECORD_REPLAY)/Makefile.config
include $(PROGRAM_MODEL)/Makefile
include $(LLVM_PASS)/Makefile
include $(SCHEDULER)/Makefile

#===== Targets =====

instrument: instrument_dir scheduler
	$(CLANG) -pthread -emit-llvm -c $(DIR)/$(PROGRAM).c -o $(instrument_dir)/$(PROGRAM).bc;
	$(LLVMBIN)/llvm-link $(BUILD)/$(scheduler_file) $(instrument_dir)/$(PROGRAM).bc -o $(instrument_dir)/$(PROGRAM).linked.bc;
	$(LLVMBIN)/opt -S -load $(PASSLIB)/$(passfile) $(passname) < $(instrument_dir)/$(PROGRAM).linked.bc > $(instrument_dir)/$(PROGRAM).instrumented.bc
	$(CLANGXX) $(instrument_dir)/$(PROGRAM).instrumented.bc -o $(instrument_dir)/$(PROGRAM) -pthread -std=c++14 

instrument_cpp: instrument_dir scheduler
	$(CLANGXX) -pthread -emit-llvm -std=c++14 $(CLANGPPFLAGS) -c $(DIR)/$(PROGRAM).cpp -o $(instrument_dir)/$(PROGRAM).bc;
	$(LLVMBIN)/llvm-link $(BUILD)/$(scheduler_file) $(instrument_dir)/$(PROGRAM).bc -o $(instrument_dir)/$(PROGRAM).linked.bc;
	$(LLVMBIN)/opt -S -load $(PASSLIB)/$(passfile) $(passname) < $(instrument_dir)/$(PROGRAM).linked.bc > $(instrument_dir)/$(PROGRAM).instrumented.bc
	$(CLANGXX) $(instrument_dir)/$(PROGRAM).instrumented.bc -o $(instrument_dir)/$(PROGRAM) -pthread -std=c++14 

instrument_dir:
	test -d $(instrument_dir) || mkdir $(instrument_dir)

clean_record_replay:
	make clean_scheduler
	make clean_pass
