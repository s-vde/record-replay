.PHONY: clean_record_replay instrument instrument_dir pass

#===== Makefile ==============================================================80
#
#                               Record-Replay
#
# author: 	Susanne van den Elsen
# date: 	2015-2017
#
# This file is to be included by the Makefile of projects using Record-
# Replay. It includes the Makefiles of the subprojects program_model,
# and scheduler and defines a number of targets.
#
#===============================================================================

#======Variables =============================================================80
#
# The root directory of Record-Replay 
# (absolute path or relative path from including project).
RECORD_REPLAY         	 ?= $(shell pwd)
#
# Directory of the program to instrument.
#   DIR
#
# Name of the program to instrument (without extension).
#   PROGRAM
#
#===============================================================================

record_replay_src     	 := $(RECORD_REPLAY)/src
PROGRAM_MODEL         	 := $(record_replay_src)/program-model
SCHEDULER             	 := $(record_replay_src)/scheduler
passfile 		 := $(RECORD_REPLAY)/build/llvm-pass/src/llvm-pass/LLVMRecordReplay.dylib
passname              	 := -instrument-record-replay-lw
instrument_dir		 := $(DIR)/instrumented

#===== Include =====

include $(RECORD_REPLAY)/Makefile.config
include $(PROGRAM_MODEL)/Makefile
include $(SCHEDULER)/Makefile

#===== Targets =====

pass:
	test -d $(RECORD_REPLAY)/build/llvm-pass || mkdir $(RECORD_REPLAY)/build/llvm-pass
	cd $(RECORD_REPLAY)/build/llvm-pass; \
	cmake	-DLLVM_DIR=$(LLVMBASE)/build/lib/cmake/llvm \
		-DUTILS=$(UTILS)/src \
		-DPROGRAM_MODEL=$(PROGRAM_MODEL) ../../; \
	make

instrument: instrument_dir scheduler
	$(CLANG) -pthread -emit-llvm -c $(DIR)/$(PROGRAM).c -o $(instrument_dir)/$(PROGRAM).bc;
	$(LLVMBIN)/llvm-link $(BUILD)/$(scheduler_file) $(instrument_dir)/$(PROGRAM).bc -o $(instrument_dir)/$(PROGRAM).linked.bc;
	$(LLVMBIN)/opt -S -load $(passfile) $(passname) < $(instrument_dir)/$(PROGRAM).linked.bc > $(instrument_dir)/$(PROGRAM).instrumented.bc
	$(CLANGXX) $(instrument_dir)/$(PROGRAM).instrumented.bc -o $(instrument_dir)/$(PROGRAM) -pthread -std=c++14 

instrument_cpp: instrument_dir scheduler
	$(CLANGXX) -pthread -emit-llvm -std=c++14 -c $(DIR)/$(PROGRAM).cpp -o $(instrument_dir)/$(PROGRAM).bc;
	$(LLVMBIN)/llvm-link $(BUILD)/$(scheduler_file) $(instrument_dir)/$(PROGRAM).bc -o $(instrument_dir)/$(PROGRAM).linked.bc;
	$(LLVMBIN)/opt -S -load $(passlib)/$(passfile) $(passname) < $(instrument_dir)/$(PROGRAM).linked.bc > $(instrument_dir)/$(PROGRAM).instrumented.bc
	$(CLANGXX) $(instrument_dir)/$(PROGRAM).instrumented.bc -o $(instrument_dir)/$(PROGRAM) -pthread -std=c++14 

instrument_dir:
	test -d $(instrument_dir) || mkdir $(instrument_dir)

clean_record_replay:
	make clean_scheduler
	rm -rf ${BUILD)
