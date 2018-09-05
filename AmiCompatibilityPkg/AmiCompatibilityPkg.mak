#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
#/** @file AmiCompatibilityPkg.mak
#
# Description:	
#
#*/
#**********************************************************************
Prepare : $(BUILD_DIR)/SetupDefinitions.h

# Make sure that SETUP_DATA_LAYOUT_OVERRIDE_HEADER file exists, if it is defined
ifneq ("$(SETUP_DATA_LAYOUT_OVERRIDE_HEADER)", "")
 ifeq ("$(wildcard $(SETUP_DATA_LAYOUT_OVERRIDE_HEADER))", "")
 $(error Setup Data Layout Override header file "$(SETUP_DATA_LAYOUT_OVERRIDE_HEADER)" defined by the SETUP_DATA_LAYOUT_OVERRIDE_HEADER SDL token is not found.)
 endif
 ifdef __ge
  ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 23),yes)
   export EXTERNAL_VFR_FLAGS := -t $(WORKSPACE)/$(SETUP_DATA_LAYOUT_OVERRIDE_HEADER)
  endif
 endif
endif

# Make sure that PROJECT_SETUP_HEADER file exists, if it is defined
ifneq ("$(PROJECT_SETUP_HEADER)", "")
 ifeq ($(wildcard $(PROJECT_SETUP_HEADER)), "")
 $(error Project specific setup header file "$(PROJECT_SETUP_HEADER)" defined by the PROJECT_SETUP_HEADER SDL token is not found.)
 endif
endif

# Generate SETUP_DATA_LAYOUT_OVERRIDE_SECTION to be used in SetupDataDefinition.h
SETUP_DATA_LAYOUT_OVERRIDE_SECTION = $(if $(SETUP_DATA_LAYOUT_OVERRIDE_HEADER),\
\#ifndef VFRCOMPILE$(EOL)\
\#include<$(SETUP_DATA_LAYOUT_OVERRIDE_HEADER)>$(EOL)\
\#else$(EOL)\
\#include<GenericSetupDataDefinition.h>$(EOL)\
\#endif$(EOL),\
\#include<GenericSetupDataDefinition.h>$(EOL) )

# Change backslashes to slashes (for better OS compatibility)
SETUP_DEFINITIONS := $(subst \,/,$(SETUP_DEFINITIONS))
PROJECT_SETUP_HEADER := $(subst \,/,$(PROJECT_SETUP_HEADER))
SETUP_DATA_LAYOUT_OVERRIDE_HEADER := $(subst \,/,$(SETUP_DATA_LAYOUT_OVERRIDE_HEADER))

$(BUILD_DIR)/SetupDefinitions.h : $(SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak AmiCompatibilityPkg/AmiCompatibilityPkg.mak
	$(ECHO) \
"$(if $(SETUP_DEFINITIONS), $(foreach S_DEF, $(SETUP_DEFINITIONS),#include<$(S_DEF)>$(EOL)))"\
>$(BUILD_DIR)/SetupDefinitions.h

	$(ECHO) \
"$(if $(SETUP_DEFINITIONS), \
#define CONTROL_DEFINITION$(EOL)\
#include<SetupDefinitions.h>$(EOL)\
#undef CONTROL_DEFINITION$(EOL)\
#define CONTROLS_ARE_DEFINED)$(EOL)"\
>$(BUILD_DIR)/ControlDefinitions.h

	$(ECHO) \
"#pragma pack(1)$(EOL)\
typedef struct{$(EOL)\
  UINT8 Numlock;$(EOL)\
  #define SETUP_DATA_DEFINITION$(EOL)\
  #include <SetupDefinitions.h>$(EOL)\
  #undef SETUP_DATA_DEFINITION$(EOL)\
} SETUP_DATA;$(EOL)\
#pragma pack()$(EOL)"\
>$(BUILD_DIR)/GenericSetupDataDefinition.h

	$(ECHO) \
"$(SETUP_DATA_LAYOUT_OVERRIDE_SECTION)\
$(if $(PROJECT_SETUP_HEADER), #include<$(PROJECT_SETUP_HEADER)>)$(EOL)"\
>$(BUILD_DIR)/SetupDataDefinition.h

ifndef FONT_INI_FILE
# We are dealing with the old version of AmiPkg
# Keep old-style FonTool command
# In the newer versions of AmiPkg, font tool processing is moved from
# AmiCompatibilityPkg (this file) to AmiPkg (Main.mak).
Prepare: BeforeGenFds
BeforeGenFds : FontTool 
.PHONY : BeforeGenFds FontTool 

ABS_BUILD_DIR:=$(WORKSPACE)$(PATH_SLASH)$(BUILD_DIR)
ABS_OUTPUT_DIR:=$(WORKSPACE)$(PATH_SLASH)$(OUTPUT_DIRECTORY)

FONT_TOOL_TEMPORARY_FILE:=$(WORKSPACE)$(PATH_SLASH)Build$(PATH_SLASH)font.tmp
FontTool :
	FontTool /S /T $(FONT_TOOL_TEMPORARY_FILE)
	$(ECHO_NO_ESC) FontTool /IL $(subst ;,$(SPACE),$(LANGUAGE_FONT_LIST)) /T $(FONT_TOOL_TEMPORARY_FILE) > $(BUILD_DIR)/BeforeGenFds.bat
	$(ECHO_NO_ESC) FontTool /F 2.1 /O $(ABS_OUTPUT_DIR)$(PATH_SLASH)font.out /T $(FONT_TOOL_TEMPORARY_FILE) >> $(BUILD_DIR)/BeforeGenFds.bat
else
.PHONY : DeleteBeforeGenFdBatchFile
InitBeforeGenFds : DeleteBeforeGenFdsBatchFile
DeleteBeforeGenFdsBatchFile :
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)BeforeGenFds.bat
endif

Prepare : LanguagesUni

AMI_COMPATIBILITY_PKG_USE_NEW_LANGUAGE_DEFINITIONS:=0
ifneq ("$(wildcard AmiPkg/Include/AmiLanguageDefinitions.uni)", "")
ifdef __ge
ifeq ($(call __ge, $(BUILD_TOOLS_VERSION), 21),yes)
AMI_COMPATIBILITY_PKG_USE_NEW_LANGUAGE_DEFINITIONS:=1
endif
endif
endif

LanguagesUni :
ifeq ($(AMI_COMPATIBILITY_PKG_USE_NEW_LANGUAGE_DEFINITIONS), 1)
	$(CP) AmiPkg$(PATH_SLASH)Include$(PATH_SLASH)AmiLanguageDefinitions.uni $(BUILD_DIR)$(PATH_SLASH)Languages.uni
else
	$(CP) AmiCompatibilityPkg$(PATH_SLASH)Include$(PATH_SLASH)LegacyLanguageDefinitions.uni $(BUILD_DIR)$(PATH_SLASH)Languages.uni	
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************