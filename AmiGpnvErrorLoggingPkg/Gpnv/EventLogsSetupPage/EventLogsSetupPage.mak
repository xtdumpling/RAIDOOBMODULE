#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

## @file EventLogsSetupPage.mak
#    Make file for EventLogsSetupPage Modular part
##

Prepare : EventLogsSetupFiles
Prepare : $(BUILD_DIR)/EventLogsSetupDefinitions.h

  ifeq ($(DXE_ARCHITECTURE), AARCH64)
    EVENTLOGSSETUPPAGE_TARGET_ARCH = AARCH64
  else
    ifeq ($(DXE_ARCHITECTURE), X64)
      EVENTLOGSSETUPPAGE_TARGET_ARCH = X64
    else
      EVENTLOGSSETUPPAGE_TARGET_ARCH = IA32
    endif
  endif

EventLogsSetupFiles : $(BUILD_DIR)/EventLogsSetupPage.inf $(BUILD_DIR)/EventLogsAutoId.h $(BUILD_DIR)/EventLogsSetupPageStrTokens.h

$(BUILD_DIR)/EventLogsAutoId.h : $(EventLogsSetupPage_DIR)/EventLogsSetupPage.vfr $(EVENT_LOGS_SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak
	$(VFRID) /s3000 /o$(BUILD_DIR)/EventLogsAutoId.h $(EventLogsSetupPage_DIR)/EventLogsSetupPage.vfr $(EVENT_LOGS_SETUP_DEFINITIONS)
	@$(ECHO) \
"#ifndef AUTO_ID$(EOL)\
#define AUTO_ID(x) x$(EOL)\
#endif$(EOL)" >> $(BUILD_DIR)/EventLogsAutoId.h

$(BUILD_DIR)/EventLogsSetupDefinitions.h : $(EVENT_LOGS_SETUP_DEFINITIONS) $(BUILD_DIR)/Token.mak $(EventLogsSetupPage_DIR)/EventLogsSetupPage.mak
	$(ECHO) \
"$(if $(EVENT_LOGS_SETUP_DEFINITIONS), $(foreach S_DEF, $(EVENT_LOGS_SETUP_DEFINITIONS),#include<$(S_DEF)>$(EOL)))"\
>$(BUILD_DIR)/EventLogsSetupDefinitions.h

$(BUILD_DIR)/EventLogsSetupPageStrTokens.h : $(EventLogsSetupPage_DIR)/EventLogsSetupPage.mak
	$(ECHO) \
"#include <$(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(EVENTLOGSSETUPPAGE_TARGET_ARCH)/Build/EventLogsSetupPage/DEBUG/EventLogsSetupPageStrDefs.h>$(EOL)"\
 >$(BUILD_DIR)/EventLogsSetupPageStrTokens.h

EVENT_LOGS_SETUP_DEFINITIONS := $(subst \,/,$(EVENT_LOGS_SETUP_DEFINITIONS))
EventLogsSetupStringFiles := $(subst \,/,$(EventLogsSetupStringFiles))


ifneq ($(EVENT_LOGS_SETUP_DEFINITIONS),"")
EVENT_LOGS_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(EVENT_LOGS_SETUP_DEFINITIONS)))$(EOL)
endif
ifneq ($(EventLogsSetupStringFiles),"")
EVENT_LOGS_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(EventLogsSetupStringFiles)))$(EOL)
endif
ifneq ($(EventLogsSetupCallbackFiles),"")
EVENT_LOGS_SETUP_FILES += $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(EventLogsSetupCallbackFiles)))$(EOL)
endif


$(BUILD_DIR)/EventLogsSetupPage.inf : $(BUILD_DIR)/Token.mak $(EventLogsSetupPage_DIR)/EventLogsSetupPage.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   SERVER_MGMT_SETUP_DEFINITIONS, ServerMgmtSetupStringFiles and ServerMgmtSetupCallbackFiles$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = EventLogsSetupPage$(EOL)\
  FILE_GUID                      = FFE0FCDC-809A-4d48-A060-6A8A6A269176$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = EventLogsSetupPageEntry$(EOL)\
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
 $(EVENT_LOGS_SETUP_FILES)$(EOL)\
  ../$(EventLogsSetupPage_DIR)/EventLogsSetupPage.c$(EOL)\
  ../$(EventLogsSetupPage_DIR)/EventLogsSetupPage.h$(EOL)\
  ../$(EventLogsSetupPage_DIR)/EventLogsSetupPage.vfr$(EOL)\
  ../$(EventLogsSetupPage_DIR)/EventLogsSetupPage.uni$(EOL)\
  $(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiGpnvErrorLoggingPkg/AmiGpnvErrorLoggingPkg.dec$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  UefiHiiServicesLib$(EOL)\
  HiiLib$(EOL)\
  PcdLib$(EOL)\
  MemoryAllocationLib$(EOL)\
  $(EOL)\
[Protocols]$(EOL)\
  gAmiEventLogsHiiHandleGuid$(EOL)\
  gAmiEventLogsDynamicGuid$(EOL)\
 $(EOL)\
[Guids]$(EOL)\
  gAmiEventLogsFormsetGuid$(EOL)\
  $(EOL)\
$(EOL)\
[Pcd]$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  gAmiEventLogsDynamicGuid$(EOL)\
$(EOL)"\
> $(BUILD_DIR)/EventLogsSetupPage.inf

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
