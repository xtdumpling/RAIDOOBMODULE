#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************

#**********************************************************************
## @file
#  First makefile to be called during build, calls ParseVeb and AMISDL 
#  to generate files needed by Main.mak, then launches Main.mak.
#**********************************************************************
export CONFIGURATION_DIR:=AmiPkg/Configuration/
export UTILITIES_MAK:=$(CONFIGURATION_DIR)utilities.mak
include $(UTILITIES_MAK)
ifeq ($(wildcard $(TOOLS_DIR)/BuildToolsVersion.mak),$(TOOLS_DIR)/BuildToolsVersion.mak)
include $(TOOLS_DIR)/BuildToolsVersion.mak
else
export BUILD_TOOLS_VERSION=0
endif

export MAIN_MAK:=$(CONFIGURATION_DIR)Main.mak
export TOKEN_MAK:=Build/Token.mak
export MODULE_MAK:=Build/module.mak
export APTIO_MAKE:=$(MAKE) --no-print-directory

# Script that runs AmiSdl and other AMI preprocessing tools can be either part of the build tools (higher priority)
# or part of the project (lower priority).
ifeq ($(wildcard $(TOOLS_DIR)/RunAmiTools.mak),$(TOOLS_DIR)/RunAmiTools.mak)
RUN_AMI_TOOLS_MAK:=$(TOOLS_DIR)/RunAmiTools.mak
else
RUN_AMI_TOOLS_MAK:=$(CONFIGURATION_DIR)RunAmiSdl.mak
endif

.PHONY : all clean rebuild run RunAmiSdl AptioV sdl

all: $(BUILD_DIR) RunAmiSdl AptioV

# Clean out the old files
clean:
	@$(ECHO) Deleting temporary files...
  ifeq ($(wildcard $(BUILD_DIR)), $(BUILD_DIR))
	-$(RMDIR) $(BUILD_DIR)
  endif	
  ifeq ($(wildcard Conf), Conf) 	
	-$(RMDIR) Conf
  endif	
	@$(APTIO_MAKE) -s -f $(RUN_AMI_TOOLS_MAK) clean
	@$(ECHO) Done.
	
rebuild: clean all

run:
	$(APTIO_MAKE) -f $(MAIN_MAK) run

sdl: $(BUILD_DIR) RunAmiSdl

# Check if Build exists, if not, then make it
$(BUILD_DIR): 
	mkdir $(BUILD_DIR)

RunAmiSdl:
	@$(APTIO_MAKE) -f $(RUN_AMI_TOOLS_MAK) all

# If TODAY/NOW are not defined on the command line, generate them automatically.
# These auto-generated values may be overridden with TODAY/NOW SDL tokens.
# TODAY/NOW defined on the command line have the highest priority (SDL tokens do not override them).
# TODAY format: mm/dd/yyyy 
# NOW format: hh:mm:ss
ifeq ($(TODAY),) 
export TODAY := $(shell $(DATE) +'%m/%d/%Y')
endif
ifeq ($(NOW),) 
export NOW := $(shell $(DATE) +%T)
endif

AptioV:
	$(APTIO_MAKE) -f $(MAIN_MAK) $(target)
ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 29),yes)
	@$(APTIO_MAKE) -s -f $(RUN_AMI_TOOLS_MAK) End
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************