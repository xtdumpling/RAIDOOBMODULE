#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
## @file
# Runs AMI pre-processing tools such as ParseVeb and AMISDL
#**********************************************************************
include $(UTILITIES_MAK)
AMISDL = $(JAVA) -jar $(TOOLS_DIR)/AMISDL.jar
PARSEVEB =  $(JAVA) -jar $(TOOLS_DIR)/ParseVeB.jar
OVERRIDE_PROCESSOR=$(JAVA) -jar $(TOOLS_DIR)/OverrideProcessor.jar
PROJECT_LDL=project.ldl
PROJECT_LFO=project.lfo
RUN_AMI_SDL_MAK:=$(CONFIGURATION_DIR)/RunAmiSdl.mak
PARSE_VEB_FLAGS:=/v$(VEB).veb /s$(PROJECT_LDL) /o$(PROJECT_LFO) /i /b
AMI_SDL_FLAGS:=$(PROJECT_LDL) /S /O$(PROJECT_LFO)

ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 24),no)
$(error This version of AmiPkg requires build tools 24 or newer (you are using tools $(BUILD_TOOLS_VERSION)))
endif

all: RunParseVeb $(TOKEN_MAK) RunOverrideProcessor
.PHONY : all RunParseVeb RunOverrideProcessor

# Parse VeB and generate a list of SDL files in project.ldl
RunParseVeb:
	$(PARSEVEB) $(PARSE_VEB_FLAGS)

RunOverrideProcessor:
	$(OVERRIDE_PROCESSOR) /i$(PROJECT_LFO)

# Generate new token files if project.ldl or any SDL files has changed
ifeq ($(wildcard $(PROJECT_LDL)), $(PROJECT_LDL))
TOKEN_MAK_DEPENDENCIES:=$(PROJECT_LDL) $(subst \,/, $(patsubst %$(PATH_SLASH),%,$(shell $(CAT) $(PROJECT_LDL))))
else
TOKEN_MAK_DEPENDENCIES:=$(PROJECT_LDL)
endif
$(TOKEN_MAK): $(RUN_AMI_SDL_MAK) $(PROJECT_LFO) $(TOKEN_MAK_DEPENDENCIES)
	$(AMISDL) $(AMI_SDL_FLAGS)

clean:
	$(OVERRIDE_PROCESSOR) /i$(PROJECT_LFO) /restore
  ifeq ($(wildcard $(PROJECT_LDL)), $(PROJECT_LDL))
	-$(RM) $(PROJECT_LDL)
  endif
  ifeq ($(wildcard project.lfo), project.lfo)
	-$(RM) project.lfo
  endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************