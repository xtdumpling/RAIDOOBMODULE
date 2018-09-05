#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#/** @file FastBoot.mak
# * Make file to build Fastboot library part
# */

#(EIP62683)(EIP63924)(EIP62845)>
#{$(FastBoot_DIR)}.c{$(BUILD_DIR)}.obj::
#	$(CC) $(CFLAGS) /I $(FastBoot_DIR)  \
#    /D\"IS_FAST_BOOT_LIST=$(IsFastBootList)\" \
#    /D\"AFTER_ALL_DRIVER_CONNECT_HOOK=$(FastBootAfterAllDriverConnctHook)\" \
#    /D\"FAST_BOOT_CHECK_MODE_CHANGE_HOOK=$(FastBootCheckModeChangeList)\" \
#    /D\"BEFORE_CONNECT_FAST_BOOT_DEVICE_HOOK=$(BeforeConnectFastBootDeviceHook)\" \
#    /Fo$(BUILD_DIR)\ $<
#<(EIP62683)(EIP63924)(EIP62845)

Prepare : FastBootFile $(BUILD_DIR)/FastBoot.inf

FastBootFile : $(BUILD_DIR)/FastBootLinks.h

FASTBOOT_FLOW_HOOK0 = $(subst $(SPACE),,$(FastBootFlowList)) # remove the spaces
FASTBOOT_FLOW_HOOK1 = $(subst */$(COMMA),*/,$(FASTBOOT_FLOW_HOOK0)) # remove commas after the comments
FASTBOOT_FLOW_HOOK2 = $(subst *//*,,$(FASTBOOT_FLOW_HOOK1)) # merge the adjacent comments
FASTBOOT_FLOW_HOOK3 = $(subst */,*/",$(FASTBOOT_FLOW_HOOK2)) # add quote after the comment to begin the next string
FASTBOOT_FLOW_HOOK4 = "$(subst $(COMMA),"$(COMMA)",$(FASTBOOT_FLOW_HOOK3))", # add quotes before and after the commas
FASTBOOT_FLOW_HOOK5 = $(subst "/*,/*,$(FASTBOOT_FLOW_HOOK4)) # remove quote before the comments
FASTBOOT_FLOW_HOOK_NAMES_STRINGS = $(subst ",\",$(FASTBOOT_FLOW_HOOK5)) # escape the quotes for echo
	
$(BUILD_DIR)/FastBootLinks.h : $(BUILD_DIR)/Token.h $(FastBoot_DIR)/FastBoot.mak
	$(ECHO) \
"#define IS_FAST_BOOT_LIST $(IsFastBootList)$(EOL)\
#define AFTER_ALL_DRIVER_CONNECT_HOOK $(FastBootAfterAllDriverConnctHook)$(EOL)\
#define FAST_BOOT_CHECK_MODE_CHANGE_HOOK $(FastBootCheckModeChangeList)$(EOL)\
#define BEFORE_CONNECT_FAST_BOOT_DEVICE_HOOK $(BeforeConnectFastBootDeviceHook)$(EOL)\
#define RETURN_NORMAL_MODE_HOOK $(ReturnNormalMode)$(EOL)\
$(EOL)\
#define FASTBOOT_FLOW_HOOK_NAMES $(FASTBOOT_FLOW_HOOK_NAMES_STRINGS)$(EOL)\
#define FASTBOOT_FLOW_HOOK $(FastBootFlowList)"\
>$(BUILD_DIR)/FastBootLinks.h

ifeq ($(AmiChipsetModulePkg_SUPPORT), 1)
$(BUILD_DIR)/FastBoot.inf : $(BUILD_DIR)/Token.h $(FastBoot_DIR)/FastBoot.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = FastBoot$(EOL)\
  FILE_GUID                      = aad784d2-87c8-44d0-b82f-edc51f6cb572$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = NULL$(EOL)\
  UEFI_HII_RESOURCE_SECTION      = TRUE$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(FastBoot_DIR)/FastBoot.c$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiChipsetModulePkg/AmiChipsetModulePkg.dec$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  UefiRuntimeServicesTableLib$(EOL)\
  DevicePathLib$(EOL)\
  DebugLib$(EOL)\
$(EOL)\
[Guids]$(EOL)\
  gEfiGlobalVariableGuid$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
  gEfiUsbPolicyProtocolGuid$(EOL)\
  gEfiPciIoProtocolGuid$(EOL)\
  gEfiSioProtocolGuid$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
 MSFT:*_*_*_CC_FLAGS = /DPTT_BUILD$(EOL)\
 GCC:*_*_*_CC_FLAGS = -DPTT_BUILD$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)"\
> $(BUILD_DIR)/FastBoot.inf
else
$(BUILD_DIR)/FastBoot.inf : $(BUILD_DIR)/Token.h $(FastBoot_DIR)/FastBoot.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = FastBoot$(EOL)\
  FILE_GUID                      = aad784d2-87c8-44d0-b82f-edc51f6cb572$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = NULL$(EOL)\
  UEFI_HII_RESOURCE_SECTION      = TRUE$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(FastBoot_DIR)/FastBoot.c$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  DevicePathLib$(EOL)\
$(EOL)\
[Guids]$(EOL)\
  gEfiGlobalVariableGuid$(EOL)\
$(EOL)\
[Protocols]$(EOL)\
  gEfiUsbPolicyProtocolGuid$(EOL)\
  gEfiPciIoProtocolGuid$(EOL)\
  gEfiSioProtocolGuid$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
 MSFT:*_*_*_CC_FLAGS = /DPTT_BUILD$(EOL)\
 GCC:*_*_*_CC_FLAGS = -DPTT_BUILD$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)"\
> $(BUILD_DIR)/FastBoot.inf
endif

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
