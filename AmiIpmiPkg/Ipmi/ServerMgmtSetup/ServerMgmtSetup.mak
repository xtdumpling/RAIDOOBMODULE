#****************************************************************************
#**                                                                        **
#**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
#**                                                                        **
#****************************************************************************
#  File History
#  
#  Rev. 1.00
#    Bug Fix : Add IPMI function support item.
#    Reason  : Customer require feature to reduce boot time, skip IPMI related function.
#              Refer from Grantley
#    Auditor : Refer from Grantley
#    Date    : Jun/07/2016
#  
#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#
# @file  ServerMgmtSetup.mak
#
#  Make file for ServerMgmtSetup Modular part
#

Prepare : ServerMgmtSetupFiles
Prepare : $(BUILD_DIR)/ServerMgmtSetupDefinitions.h

ServerMgmtSetupFiles : $(BUILD_DIR)/ServerMgmtSetup.inf $(BUILD_DIR)/ServerAutoId.h $(BUILD_DIR)/ServerMgmtSetupCallbackList.h

# Make sure that SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER file exists, if it is defined
ifneq ("$(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER)", "")
 ifeq ("$(wildcard $(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER))", "")
 $(error Setup Data Layout Override header file "$(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER)" defined by the SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER SDL token is not found.)
endif
endif

# Generate SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_SECTION to be used in ServerMgmtSetupDataDefinition.h
SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_SECTION = $(if $(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER),\
\#ifndef VFRCOMPILE$(EOL)\
\#include<$(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER)>$(EOL)\
\#else$(EOL)\
\#include<ServerMgmtGenericSetupDataDefinition.h>$(EOL)\
\#endif$(EOL),\
\#include<ServerMgmtGenericSetupDataDefinition.h>$(EOL) )

# Change backslashes to slashes (for better OS compatibility)
SERVER_MGMT_SETUP_DEFINITIONS := $(subst \,/,$(SERVER_MGMT_SETUP_DEFINITIONS))
SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER := $(subst \,/,$(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_HEADER))

$(BUILD_DIR)/ServerMgmtSetupCallbackList.h : $(BUILD_DIR)/Token.mak
	@$(ECHO) \
"#define SERVER_MGMT_SETUP_ITEM_CALLBACK_LIST $(ServerMgmtSetupItemCallbacks)$(EOL)\
#define SERSVER_MGMT_STRING_INIT_LIST $(ServerMgmtStringInit)$(EOL)" > $(BUILD_DIR)/ServerMgmtSetupCallbackList.h

$(BUILD_DIR)/ServerAutoId.h : $(ServerMgmtSetup_DIR)/ServerMgmtSetup.vfr $(SERVER_MGMT_SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak
	$(VFRID) /s2049 /o$(BUILD_DIR)/ServerAutoId.h $(ServerMgmtSetup_DIR)/ServerMgmtSetup.vfr $(SERVER_MGMT_SETUP_DEFINITIONS)
	@$(ECHO) \
"#ifndef AUTO_ID$(EOL)\
#define AUTO_ID(x) x$(EOL)\
#endif$(EOL)" >> $(BUILD_DIR)/ServerAutoId.h

$(BUILD_DIR)/ServerMgmtSetupDefinitions.h : $(SERVER_MGMT_SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak $(ServerMgmtSetup_DIR)/ServerMgmtSetup.mak
	$(ECHO) \
"$(if $(SERVER_MGMT_SETUP_DEFINITIONS), $(foreach S_DEF, $(SERVER_MGMT_SETUP_DEFINITIONS),#include<$(S_DEF)>$(EOL)))"\
>$(BUILD_DIR)/ServerMgmtSetupDefinitions.h

	$(ECHO) \
"$(if $(SERVER_MGMT_SETUP_DEFINITIONS), \
#define CONTROL_DEFINITION$(EOL)\
#include<ServerMgmtSetupDefinitions.h>$(EOL)\
#undef CONTROL_DEFINITION$(EOL)\
#define CONTROLS_ARE_DEFINED)$(EOL)"\
>$(BUILD_DIR)/ServerMgmtControlDefinitions.h

	$(ECHO) \
"#include<Token.h>$(EOL)\
#pragma pack(1)$(EOL)\
typedef struct{$(EOL)\
  UINT8        BmcSupport;$(EOL)\
  UINT8        IpmiFunctionSupport;$(EOL)\
#if BMC_INIT_DELAY$(EOL)\
  UINT8        WaitForBmc;$(EOL)\
#endif$(EOL)\
#if BMC_TIMEZONE_SUPPORT$(EOL)\
  CHAR16    TimeZone[6];$(EOL)\
#endif$(EOL)\
  #define SERVER_MGMT_CONFIGURATION_DATA_DEFINITION$(EOL)\
  #include <ServerMgmtSetupDefinitions.h>$(EOL)\
  #undef SERVER_MGMT_CONFIGURATION_DATA_DEFINITION$(EOL)\
} SERVER_MGMT_CONFIGURATION_DATA;$(EOL)\
#pragma pack()$(EOL)"\
>$(BUILD_DIR)/ServerMgmtGenericSetupDataDefinition.h

	$(ECHO) \
"$(SERVER_MGMT_SETUP_DATA_LAYOUT_OVERRIDE_SECTION)$(EOL)"\
>$(BUILD_DIR)/ServerMgmtSetupDataDefinition.h

SERVER_MGMT_SETUP_DEFINITIONS := $(subst \,/,$(SERVER_MGMT_SETUP_DEFINITIONS))
ServerMgmtSetupStringFiles := $(subst \,/,$(ServerMgmtSetupStringFiles))
ServerMgmtSetupCallbackFiles := $(subst \,/,$(ServerMgmtSetupCallbackFiles))

ifneq ($(SERVER_MGMT_SETUP_DEFINITIONS),"")
SERVER_MGMT_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(SERVER_MGMT_SETUP_DEFINITIONS)))$(EOL)
endif
ifneq ($(ServerMgmtSetupStringFiles),"")
SERVER_MGMT_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(ServerMgmtSetupStringFiles)))$(EOL)
endif
ifneq ($(ServerMgmtSetupCallbackFiles),"")
SERVER_MGMT_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(ServerMgmtSetupCallbackFiles)))$(EOL)
endif


$(BUILD_DIR)/ServerMgmtSetup.inf : $(BUILD_DIR)/Token.mak $(ServerMgmtSetup_DIR)/ServerMgmtSetup.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   SERVER_MGMT_SETUP_DEFINITIONS, ServerMgmtSetupStringFiles and ServerMgmtSetupCallbackFiles$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = ServerMgmtSetup$(EOL)\
  FILE_GUID                      = 1B08A1DB-F91A-4fa1-A911-255C417F1CF7$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = InstallServerMgmtSetupScreen$(EOL)\
#$(EOL)\
#  This flag specifies whether HII resource section is generated into PE image.$(EOL)\
#$(EOL)\
  UEFI_HII_RESOURCE_SECTION      = TRUE$(EOL)\
  $(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(SERVER_MGMT_SETUP_FILES)$(EOL)\
  ../$(ServerMgmtSetup_DIR)/ServerMgmtSetup.c$(EOL)\
  ../$(ServerMgmtSetup_DIR)/ServerMgmtSetup.h$(EOL)\
  ../$(ServerMgmtSetup_DIR)/ServerMgmtSetup.vfr$(EOL)\
  ../$(ServerMgmtSetup_DIR)/ServerMgmtSetupStrings.uni$(EOL)\
  $(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiIpmiPkg/AmiIpmiPkg.dec$(EOL)\
  AmiTsePkg/AmiTsePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalServerMgmtSetupPackages))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  BaseLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiHiiServicesLib$(EOL)\
  HiiLib$(EOL)\
  UefiLib$(EOL)\
  PcdLib$(EOL)\
  MemoryAllocationLib$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalServerMgmtSetupLibraryClasses))$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalServerMgmtSetupProtocols))$(EOL)\
$(EOL)\
[Guids]$(EOL)\
  gEfiServerMgmtSetupVariableGuid$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalServerMgmtSetupGuids))$(EOL)\
$(EOL)\
[Pcd]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalServerMgmtSetupPcds))$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)\
$(EOL)"\
> $(BUILD_DIR)/ServerMgmtSetup.inf

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
