.PHONY:

#===== Makefile ==============================================================80
#
#                          Record-Replay/Program-Model
#
# author: 	Susanne van den Elsen
# date: 	2015
#
# This file is to be included by the Makefile of projects using Program-Model.
#
#===============================================================================

#======Variables =============================================================80
#
# Defined here:
#	program_model_src   : The directory of Program-Model source files.
#	program_model_srcs  : List of Program-Model source files.
#
#===============================================================================

PROGRAM_MODEL           ?= .
program_model_src       := $(PROGRAM_MODEL)
program_model_srcs      := $(notdir $(shell find $(program_model_src) -name "*.cpp"))

VPATH                   += $(program_model_src)
