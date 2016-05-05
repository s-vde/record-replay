.PHONY:

#===== Makefile ==============================================================80
#
#                      Record-Replay/Scheduler/Strategies
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# Defines paths and targets for Scheduler Selection Strategy files, specifying
# their dependencies.
#
#===============================================================================

strategies          ?= .
strategies_src      := $(strategies)

VPATH               += $(strategies_src)

#===== Targets =====

selector_register.*:\
	selector.hpp

non_preemptive.*:\
    execution.hpp\
    state.hpp\
	task_pool.hpp

random.*:\
    execution.hpp\
    state.hpp\
	task_pool.hpp
