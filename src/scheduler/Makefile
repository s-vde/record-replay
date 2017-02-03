.PHONY: clean_scheduler scheduler

#===== Makefile ==============================================================80
#
#                             Record-Replay/Scheduler
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# This file is to be included by the Makefile of projects using Scheduler.
#
#===============================================================================

#======Variables =============================================================80
#
# Defined here:
#	scheduler_src     : The directory of Scheduler source files.
#	strategies        : The directory containing selection strategy files.
#	scheduler_srcs    : List of Scheduler source files.
# 	scheduler_file    : The name of the file produced by target scheduler.
#   	scheduler_objs_bc : List of object files to be linked into $(scheduler_file)
#
#===============================================================================

SCHEDULER           	   ?= .
scheduler_src		   := $(SCHEDULER)
strategies          	   := $(scheduler_src)/strategies
CUSTOM_SELECT       	   ?= $(strategies)/custom
scheduler_srcs      	   := $(foreach subdir, $(scheduler_src) $(strategies), $(notdir $(wildcard $(subdir)/*.cpp)))
scheduler_file		   := instrumentation.bc

SUFFIXES            	   := .bc .cpp .hpp .o
VPATH               	   += $(scheduler_src)

#===== Include =====

include $(UTILS)/Makefile
include $(PROGRAM_MODEL)/Makefile
include $(strategies)/Makefile
include $(CUSTOM_SELECT)/Makefile
include $(SCHEDULER)/Makefile.replay

objs                	   = $(addprefix $(BUILD)/,$(subst .cpp,$(2),$(1)))
scheduler_objs	 	   := $(call objs,$(program_model_srcs) $(scheduler_srcs) $(custom_select_srcs) $(utils_srcs),.bc)
scheduler_deps		   := $(scheduler_objs:.bc=.d)

-include $(scheduler_deps)

#===== Patterns =====

$(BUILD)/%.bc: %.cpp
	@echo Compiling $(notdir $(basename $@)).cpp
	@$(CLANGXX) \
	-I $(program_model_src) \
	-I $(scheduler_src) \
	-I $(strategies_src) \
	-I $(custom_select_src) \
	-I $(utils_src) \
	$(custom_select_include_flag) \
	-emit-llvm \
	-std=c++14 \
	-c -MMD $< -o $@

#===== Targets =====

create_build_dir:
	test -d $(BUILD) || mkdir $(BUILD)

scheduler: create_build_dir $(scheduler_objs)
	rm -rf $(scheduler_file)
	$(LLVMBIN)/llvm-link $(LDFLAGS) -o $(BUILD)/$(scheduler_file) $(scheduler_objs)
	ln -s $(BUILD)/$(scheduler_file) $(scheduler_file)

clean_scheduler:
	rm -rf $(BUILD)/*.bc
	rm -rf $(scheduler_file)
