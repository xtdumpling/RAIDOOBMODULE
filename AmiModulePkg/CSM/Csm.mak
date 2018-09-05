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

#---------------------------------------------------------------------------
#
#   Make file for the CSM. "Prepare" target will make INF files generated
#   to support OEM customization features. Later INF files generation will
#   be performed by AMISDL. When this support is available, current file
#   should be obsolete.
#
#---------------------------------------------------------------------------

# Make sure LEGACY_TO_EFI_BOOT_SUPPORT is not defined or reset, i.e. LegacyToEfiBoot module is off
ifeq ("$(LEGACY_TO_EFI_BOOT_SUPPORT)", "1")
$(error LEGACY_TO_EFI_BOOT_SUPPORT detected: disable or remove LegacyToEfiBoot module. This version of CSM implements all features of LegacyToEfiBoot.)
endif

############################################################################################################
#
# The following steps will make LEGACY_TO_EFI functional in the project that has NVRAM version 7 or greater.
#
############################################################################################################
#
# 1. Modify NVRAM driver: remove the callback installation on the LegacyBoot event.
#
# 2. Set CSM_IGNORE_NVRAM_RT_ACCESS vulnurability flag to 1 (defined in this file below). By setting this flag
#    you assume the responsibility for the system that becomes vulnurable becase of the NVRAM boot variables
#    can be potentially accessed from run-time.
#
# Note: NvramDxe.c and Csm.mak become private files.
#
############################################################################################################

CSM_IGNORE_NVRAM_RT_ACCESS = 0

ifeq ("$(LEGACY_TO_EFI_DEFAULT)", "1")
$(warning LEGACY_TO_EFI_DEFAULT is ON and NVRAM driver version is $(NVRAM_VERSION).)
ifeq ($(call __ge, $(NVRAM_VERSION), 7),yes)
ifneq ("$(CSM_IGNORE_NVRAM_RT_ACCESS)", "1")
$(warning This version of NVRAM requires a patch for LEGACY_TO_EFI to be functional.)
$(error Follow the steps listed in Csm.mak to enable this feature.)
endif
endif
endif

Prepare :  $(BUILD_DIR)/equates.equ $(BUILD_DIR)/OemCsm16.inf MANAGE_TEMP_FILES PACK_LEGACY_ROMS $(BUILD_DIR)/CsmElinkFunctions.h

PACK_LEGACY_ROMS: $(CSM_DIR)/Csm.mak $(CSM_DIR)/OpRomScript.txt $(BUILD_DIR)/Token.mak
	$(ECHO) "$(CSM_OPROM_LIST)" > $(BUILD_DIR)/LegacyRomsELink.txt
	gawk -f $(CSM_DIR)/OpRomScript.txt $(BUILD_DIR)/LegacyRomsELink.txt > $(BUILD_DIR)/LegacyRoms.txt
	PackOpROM /o$(BUILD_DIR)/ROMs.bin $(BUILD_DIR)/LegacyRoms.txt

$(BUILD_DIR)/CsmElinkFunctions.h:  $(BUILD_DIR)/Token.mak $(CSM_DIR)/Csm.mak
	$(ECHO) "\
#define CSM_GET_OPROM_VIDEO_SWITCHING_MODE_FUNCTIONS $(CSM_GET_OPROM_VIDEO_SWITCHING_MODE_FUNCTIONS)$(EOL)\
#define CSM_GET_CUSTOM_PCI_PIRQ_MASK_FUNCTIONS $(CSM_GET_CUSTOM_PCI_PIRQ_MASK_FUNCTIONS)$(EOL)\
#define CSM_GET_GATE_A20_INFORMATION_FUNCTIONS $(CSM_GET_GATE_A20_INFORMATION_FUNCTIONS)$(EOL)\
#define CSM_GET_NMI_INFORMATION_FUNCTIONS $(CSM_GET_NMI_INFORMATION_FUNCTIONS)$(EOL)\
#define CSM_GET_OEM_INT_DATA_FUNCTIONS $(CSM_GET_OEM_INT_DATA_FUNCTIONS)$(EOL)\
#define CSM_GET_PLATFORM_HANDLE_FUNCTIONS $(CSM_GET_PLATFORM_HANDLE_FUNCTIONS)$(EOL)\
#define CSM_GET_PLATFORM_PCI_EMBEDDED_ROM_FUNCTIONS $(CSM_GET_PLATFORM_PCI_EMBEDDED_ROM_FUNCTIONS)$(EOL)\
#define CSM_GET_PLATFORM_EMBEDDED_ROM_FUNCTIONS $(CSM_GET_PLATFORM_EMBEDDED_ROM_FUNCTIONS)$(EOL)\
#define CSM_CHECK_OEM_PCI_SIBLINGS_FUNCTIONS $(CSM_CHECK_OEM_PCI_SIBLINGS_FUNCTIONS)$(EOL)\
#define CSM_ENABLE_OEM_PCI_SIBLINGS_FUNCTIONS $(CSM_ENABLE_OEM_PCI_SIBLINGS_FUNCTIONS)$(EOL)\
#define CSM_GET_ROUTING_TABLE_FUNCTIONS $(CSM_GET_ROUTING_TABLE_FUNCTIONS)$(EOL)\
#define CSM_BSP_UPDATE_PRT_FUNCTIONS $(CSM_BSP_UPDATE_PRT_FUNCTIONS)$(EOL)\
#define CSM_BSP_PREPARE_TO_BOOT_FUNCTIONS $(CSM_BSP_PREPARE_TO_BOOT_FUNCTIONS)$(EOL)\
#define CSM_INIT_LEGACY_MEMORY_FUNCTIONS $(CSM_INIT_LEGACY_MEMORY_FUNCTIONS)$(EOL)\
#define CSM_CHECK_UEFI_OPROM_POLICY $(CSM_CHECK_UEFI_OPROM_POLICY)$(EOL)\
#define CSM_16_CALL_COMPANION_FUNCTIONS $(CSM_16_CALL_COMPANION_FUNCTIONS)$(EOL)\
" > $(BUILD_DIR)/CsmElinkFunctions.h


MANAGE_TEMP_FILES:
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)data.dat
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)*.csm16obj
	$(CP) $(CSM_DIR)$(PATH_SLASH)Oem16End.csm16obj $(BUILD_DIR)$(PATH_SLASH)

# equates.equ is needed to compile csmkrnl.csm16 (outputregister EKERNEL_POST)
$(BUILD_DIR)/equates.equ : $(CSM_DIR)/Csm.mak
	$(ECHO) " "\
>$(BUILD_DIR)/equates.equ

$(BUILD_DIR)/OemCsm16.inf : $(BUILD_DIR)/Token.h $(CSM_DIR)/Csm.mak
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = OemCsm16$(EOL)\
  FILE_GUID                      = E998C6D8-572B-4e18-96CC-031EA3DD558C$(EOL)\
  MODULE_TYPE                    = USER_DEFINED$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  CsmKrnl.csm16$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(CSM_OEMINT_SRC)))$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(CSM_OEM16_SRC)))$(EOL)\
  ../$(CSM_DIR)/Oem16Sig.csm16$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(CSM_OEMSIG_SRC)))$(EOL)\
  $(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)"\
> $(BUILD_DIR)/OemCsm16.inf

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