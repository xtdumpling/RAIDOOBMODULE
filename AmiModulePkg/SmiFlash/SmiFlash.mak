##****************************************************************************
##****************************************************************************
##**                                                                        **
##**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
##**                                                                        **
##****************************************************************************
##****************************************************************************
#
#  File History
#
#  Rev. 1.00
#    Bug Fix : N/A.
#    Reason  : Defended SMCI OOB in-band SW SMI 0xE6 with SMM communication 
#              mail box mechanism.
#    Auditor : Joe Jhang
#    Date    : Jun/06/2017
#
##****************************************************************************
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

#**********************************************************************
# $Header: $
#
# $Revision: $
#
# $Date: $
#**********************************************************************
# Revision History
# ----------------
# $Log: $
# 
# 
#**********************************************************************


#<AMI_FHDR_START>
#-----------------------------------------------------------------------
#
# Name:			SMIFlash.mak
#
# Description:	
#				
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/SMIFlashELinks.h $(BUILD_DIR)/SmiFlash.inf

$(BUILD_DIR)/SMIFlashELinks.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define SMIFLASH_IN_SMM_LIST $(SMIFlashInSmmList)$(EOL)\
#define SMIFLASH_NOT_IN_SMM_LIST $(SMIFlashNotInSmmList)$(EOL)\
#define SMIFLASH_END_UPDATE_LIST $(SMIFlashEndUpdateList)$(EOL)\
#define SMIFLASH_PRE_UPDATE_LIST $(SMIFlashPreUpdateList)$(EOL)\
#define SMIFLASH_PRE_HANDLER_LIST $(SMIFlashPreHandlerList)$(EOL)\
#define SMIFLASH_END_HANDLER_LIST $(SMIFlashEndHandlerList)$(EOL)\
#define SMIFLASH_SAVE_VARS_LIST $(SMIFlashSaveVarsList)$(EOL)\
#define SMIFLASH_SAVE_GUIDS_LIST $(SMIFlashSaveGuidsList)$(EOL)\
#define SMIFLASH_OEM_FUNC_HOOKS_LIST $(SMIFlashOemFuncHooksList)$(EOL)"\
> $(BUILD_DIR)/SMIFlashELinks.h

#----------------------------------------------------------------------------
#           Create SmiFlash module inf file
#----------------------------------------------------------------------------
$(BUILD_DIR)/SmiFlash.inf : $(BUILD_DIR)/Token.h $(SMI_FLASH_DIR)/SmiFlash.mak 
	$(ECHO)  \
" #**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
#**                                                                  **$(EOL)\
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **$(EOL)\
#**                                                                  **$(EOL)\
#**                       All Rights Reserved.                       **$(EOL)\
#**                                                                  **$(EOL)\
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **$(EOL)\
#**                                                                  **$(EOL)\
#**                       Phone: (770)-246-8600                      **$(EOL)\
#**                                                                  **$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
$(EOL)\
$(EOL)\
 [Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = SmiFlash$(EOL)\
  FILE_GUID                      = BC327DBD-B982-4f55-9F79-056AD7E987C5$(EOL)\
  MODULE_TYPE                    = DXE_SMM_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x0001000A$(EOL)\
  ENTRY_POINT                    = SMIFlashDriverEntryPoint$(EOL)\
$(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(SMI_FLASH_DIR)/SmiFlash.c$(EOL)\
  ../$(SMI_FLASH_DIR)/SmiFlashLinks.c$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(AMI_SMI_FLASH_OEM_SOURCES_FILES)))$(EOL)\
$(EOL)\
[Packages] $(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_SMI_FLASH_OEM_PACKAGES))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  FlashLib$(EOL)\
  AmiBufferValidationLib$(EOL)\
  MemoryAllocationLib$(EOL)\
  DebugLib$(EOL)\
  HobLib$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_SMI_FLASH_OEM_LIBRARIES))$(EOL)\
  $(EOL)\
[Guids]$(EOL)\
  gEfiFirmwareFileSystem2Guid$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_SMI_FLASH_OEM_GUIDS))$(EOL)\
  $(EOL)\
[Protocols]$(EOL)\
  gEfiSmmBase2ProtocolGuid$(EOL)\
  gEfiSmmSwDispatch2ProtocolGuid$(EOL)\
  gFlashSmmProtocolGuid$(EOL)\
  gEfiFirmwareVolume2ProtocolGuid$(EOL)\
  gEfiSmmAccess2ProtocolGuid$(EOL)\
  gAmiUsbSmmProtocolGuid$(EOL)\
  gEmul6064TrapProtocolGuid$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_SMI_FLASH_OEM_PROTOCOLS))$(EOL)\
  $(EOL)\
[Depex.common.DXE_SMM_DRIVER]$(EOL)\
  gEfiSmmBase2ProtocolGuid AND$(EOL)\
  gEfiSmmSwDispatch2ProtocolGuid AND$(EOL)\
  gFlashSmmProtocolGuid AND$(EOL)\
  gEfiFirmwareVolume2ProtocolGuid AND$(EOL)\
  gEfiSmmAccess2ProtocolGuid$(EOL)\
  $(subst $(SPACE),$(SPACE)AND$(EOL),$(AMI_SMI_FLASH_OEM_DEPEX))\
$(EOL)\
$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
#**                                                                  **$(EOL)\
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **$(EOL)\
#**                                                                  **$(EOL)\
#**                       All Rights Reserved.                       **$(EOL)\
#**                                                                  **$(EOL)\
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **$(EOL)\
#**                                                                  **$(EOL)\
#**                       Phone: (770)-246-8600                      **$(EOL)\
#**                                                                  **$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
$(EOL)"\
> $(BUILD_DIR)/SmiFlash.inf

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
