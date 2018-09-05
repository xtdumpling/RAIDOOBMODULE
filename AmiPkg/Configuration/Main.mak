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

#**********************************************************************
## @file
# Called by makefile, this includes all of the makefiles
#  from module.mak, generates target.txt, then launches the EDK2 build.exe
#**********************************************************************
include $(UTILITIES_MAK)
include $(TOKEN_MAK)

ifeq ($(SILENT), 1) 
APTIO_MAKE+=-s
.SILENT :
endif

# Set target debug type based on SDL token DEBUG_MODE
ifeq ($(DEBUG_MODE), 1)
 TARGET	= DEBUG
else
 TARGET	= RELEASE
endif	#ifeq ($(DEBUG_MODE), 1)

ifeq ($(RUN_VSVARS_BAT), 1)
EDKII_BUILD_COMMAND = $(TOOLS_DIR)\RunInVsEnv.bat $(EDII_BUILD)
else
EDKII_BUILD_COMMAND = $(EDII_BUILD)
endif	#ifeq (TOOL_CHAIN_TAG, MYTOOLS)

ABS_BUILD_DIR:=$(WORKSPACE)$(PATH_SLASH)$(BUILD_DIR)
ABS_OUTPUT_DIR:=$(WORKSPACE)$(PATH_SLASH)$(OUTPUT_DIRECTORY)

include $(CONFIGURATION_DIR)ToolChainInit.mak

.PHONY : all run Prepare EdkII RomPatch End ModuleEnd BuildTimeStamp

all: Prepare EdkII

Prepare: BuildTimeStamp $(ROM_IMAGE_DIRECTORY) $(BUILD_DIR)/RomImageDirectory.mak

# Generate new timestamp files
BuildTimeStamp:
	$(APTIO_MAKE) -f $(CONFIGURATION_DIR)dater.mak TODAY=$(TODAY) NOW=$(NOW)

$(ROM_IMAGE_DIRECTORY): 
	$(MKDIR) $(ROM_IMAGE_DIRECTORY)

$(BUILD_DIR)/RomImageDirectory.mak : $(TOKEN_MAK) $(MAIN_MAK)
	$(ECHO) ROM_IMAGE_DIRECTORY=$(subst /,$(PATH_SLASH),$(WORKSPACE))$(PATH_SLASH)$(ROM_IMAGE_DIRECTORY) > $(BUILD_DIR)/RomImageDirectory.mak

.PHONY : BeforeGenFds InitBeforeGenFds FontFile 
Prepare : BeforeGenFds
BeforeGenFds : InitBeforeGenFds FontFile IfrDirList

InitBeforeGenFds :
	$(ECHO) \
"include $(UTILITIES_MAK)\
$(EOL).PHONY : all\
$(EOL)all:\
" > $(BUILD_DIR)/BeforeGenFds.mak

FontFile : InitBeforeGenFds
	$(FONT_TOOL) -S -T $(FONT_TOOL_TMP_FILE)
	$(ECHO) \
"$(EOL).PHONY : FontFile$(EOL)all: FontFile\
$(EOL)FontFile : \
$(EOL)$(TAB)$(FONT_TOOL) -IL $(subst ;,$(SPACE),$(LANGUAGE_FONT_LIST)) -T $(FONT_TOOL_TMP_FILE)\
$(EOL)$(TAB)$(FONT_TOOL) -O $(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/font.out -T $(FONT_TOOL_TMP_FILE)\
" >> $(BUILD_DIR)/BeforeGenFds.mak

IfrDirList : InitBeforeGenFds $(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrList
	$(ECHO) \
"$(EOL).PHONY : IfrDirList$(EOL)all: IfrDirList\
$(EOL)IfrDirList : \
$(EOL)ifneq ('$(DOUBLEDOLLAR)(wildcard $(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrList/*.txt)','')\
$(EOL)$(TAB)$(CAT) $(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrList/*.txt > $(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrDirList.txt\
$(EOL)endif\
" >> $(BUILD_DIR)/BeforeGenFds.mak

$(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrList:
	$(MKDIR) $(subst /,$(PATH_SLASH),$(ABS_OUTPUT_DIR)/$(TARGET)_$(TOOL_CHAIN_TAG)/IfrList)

# Clear MAKEFLAGS during execution of the EdkII target. 
# We don't want to pass our options to EDKII build process.
run : MAKEFLAGS=
run : 
	$(EDKII_BUILD_COMMAND) run

# Create Conf/target.txt, based on SDL tokens
Conf/target.txt : $(TOKEN_MAK) $(MAIN_MAK) Conf
	@$(ECHO) \
"ACTIVE_PLATFORM	   = $(subst \,/, $(ACTIVE_PLATFORM))\
$(EOL)\
$(EOL)TARGET				= $(TARGET)\
$(EOL)\
$(EOL)TOOL_CHAIN_CONF	   = $(TOOL_DEFINITION_FILE)\
$(EOL)\
$(EOL)TOOL_CHAIN_TAG		= $(TOOL_CHAIN_TAG)\
$(EOL)\
$(EOL)MAX_CONCURRENT_THREAD_NUMBER = $(NUMBER_OF_BUILD_PROCESS_THREADS)\
$(EOL)\
$(EOL)BUILD_RULE_CONF = $(BUILD_RULE_FILE)" > Conf/target.txt

Conf: 
	$(MKDIR) Conf

# Make sure the variables required to launch EDKII build process are set
ifeq ("$(wildcard $(ACTIVE_PLATFORM))","")
  $(error Project description(.dsc) file "$(ACTIVE_PLATFORM)" defined by the ACTIVE_PLATFORM SDL token is not found.\
  This can happen when PLATFORM_DSC or <Arch-Type>_PLATFORM_DSC SDL output directive does not exist\
  or refers to a different file)
endif
ifeq ("$(PLATFORM_GUID)","INVALID")
  $(error PLATFORM_GUID SDL token must be cloned in every project)
endif
ifeq ("$(SUPPORTED_ARCHITECTURES)","INVALID")
  $(error The value of the SUPPORTED_ARCHITECTURES SDL token is invalid.\
  The token must be cloned by the CPU architecture module)
endif

ifeq ($(SILENT), 1) 
# EDKII_FLAGS can be set with a command line argument.
# If a variable has been set with a command line argument, the ordinary assignments in the makefile are ignored.
# Such a variable can be updated using an override directive.
override EDKII_FLAGS += -s
endif

####################################################
# Single Module Build Support.
# There are two ways to define a module to build:
# 1. Using VeB GUI. VeB will set VEB_BUILD_MODULE environment variable.
# 2. Using EDKII_FLGAS (-m <module-name>)
####################################################
# Extract module name from EDKII_FLAGS (Don't try this at home :)
EDKII_ACTIVE_MODULE_NAME:=$(strip $(subst ~~~~~,,$(filter ~~~~~%,$(subst -m$(SPACE),~~~~~,$(EDKII_FLAGS)))))
ifneq ("$(EDKII_ACTIVE_MODULE_NAME)","")
ifneq ("$(VEB_BUILD_MODULE)","")
$(error Ambiguous single component build parameters. VEB_BUILD_MODULE=$(VEB_BUILD_MODULE). EDKII_FLAGS module = $(EDKII_ACTIVE_MODULE_NAME).)
endif
endif
ifneq ("$(VEB_BUILD_MODULE)","")
override EDKII_FLAGS += -m $(VEB_BUILD_MODULE)
EDKII_ACTIVE_MODULE_NAME:=$(VEB_BUILD_MODULE)
else
override EDKII_FLAGS += --before-fv-command="$(APTIO_MAKE) -f $(BUILD_DIR)/BeforeGenFds.mak"
endif

ifeq ("$(EDKII_ACTIVE_MODULE_NAME)","")
all: End
End: RomPatch
else
all: ModuleEnd
endif

# Clear MAKEFLAGS during execution of the EdkII target. 
# We don't want to pass our options to EDKII build process.
EdkII: MAKEFLAGS=
# If using MYTOOLS, create a batch file that runs VSVARS32.BAT before build.exe
#  so that the build can inherit the newly set up env variables
EdkII: Conf/target.txt
	$(EDKII_BUILD_COMMAND) $(EDKII_FLAGS)
ifeq ($(MULTIPLATFORM),1)
ifeq ($(wildcard $(BUILD_DIR)/MultiPlatform),)
	$(MKDIR) $(BUILD_DIR)$(PATH_SLASH)MultiPlatform
endif
ifeq ($(wildcard $(ROM_FILE_NAME)),)
	$(ECHO) "ROM File $(ROM_FILE_NAME) defined by the ROM_FILE_NAME SDL token is not found"
endif
	$(CP) $(ROM_FILE_NAME) $(BUILD_DIR)$(PATH_SLASH)MultiPlatform$(PATH_SLASH)$(PLATFORM_NAME).fd
endif # ifeq ($(MULTIPLATFORM),1)

# Include all the module makefiles
include $(MODULE_MAK)
ifneq ($(LAST_MAKEFILE),)
 include $(LAST_MAKEFILE)
endif

ModuleEnd:
	$(ECHO) TARGET: $(TARGET). TOOL CHAIN: $(TOOL_CHAIN_TAG). ARCHITECTURE: "$(SUPPORTED_ARCHITECTURES)".
	$(ECHO) Build Tools: $(BUILD_TOOLS_VERSION). Single component build mode. ROM image generation is skipped.
	$(ECHO) Active module: $(EDKII_ACTIVE_MODULE_NAME)

End:
#ifdef FWBUILD
#  ifeq ($(SILENT), 0) 
#	$(FWBUILD) $(ROM_FILE_NAME) $(FWBUILD_OPTION_CHAR)v
#  else 
#	$(FWBUILD) $(ROM_FILE_NAME) $(FWBUILD_OPTION_CHAR)s
#  endif
#endif
	$(ECHO) TARGET: $(TARGET). TOOL CHAIN: $(TOOL_CHAIN_TAG). ARCHITECTURE: "$(SUPPORTED_ARCHITECTURES)".
	$(ECHO) Build Tools: $(BUILD_TOOLS_VERSION). ROM IMAGE: $(ROM_FILE_NAME).
	$(ECHO) All output modules were successfully built.

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