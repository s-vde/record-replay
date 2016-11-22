.PHONY: clean_pass pass

#===== Makefile ==============================================================80
#
#                            Record-Replay/LLVM-Pass
#
# author: 	Susanne van den Elsen
# date: 	2015
#
#===============================================================================

#===== Variables =============================================================80
#
# Defined here:
#   llvm_pass_src       : The directory of LLVM-Pass source and header files.
#   llvm_pass_files     : List of LLVM-Pass source and header files.
#   passdir             : The directory name of the pass's files in the llvm
#                         project directories.
#   passfile            : The name of the pass dynamic library.
#
#===============================================================================

LLVM_PASS               ?= .
llvm_pass_src           := $(LLVM_PASS)
llvm_pass_files         := $(llvm_pass_src)/*.hpp $(llvm_pass_src)/*.cpp
passdir                 := record-replay
passfile                := record-replay.$(PASSEXT)

#===== Include =====

include $(UTILS)/Makefile
include $(PROGRAM_MODEL)/Makefile

#===== Targets =====

pass:
	test -d $(PASSBUILD)/$(passdir) || mkdir $(PASSBUILD)/$(passdir)
	test -d $(PASSSRC)/$(passdir) || mkdir $(PASSSRC)/$(passdir)
	cp $(llvm_pass_src)/Makefile.llvm $(PASSBUILD)/$(passdir)/Makefile
	cp $(llvm_pass_src)/Makefile.llvm $(PASSSRC)/$(passdir)/Makefile
	cp $(llvm_pass_files) $(PASSSRC)/$(passdir)
	for file in debug error color_output container_format container_input container_output utils_io; do \
		cp $(utils_src)/$$file.* $(PASSSRC)/$(passdir); \
	done
	for file in object object_io; do \
		cp $(program_model_src)/$$file.* $(PASSSRC)/$(passdir); \
	done
	make --directory=$(PASSBUILD)/$(passdir)

clean_pass:
	rm -rf $(PASSSRC)/$(passdir)
	rm -rf $(PASSBUILD)/$(passdir)
	rm -rf $(PASSLIB)/$(passfile)
