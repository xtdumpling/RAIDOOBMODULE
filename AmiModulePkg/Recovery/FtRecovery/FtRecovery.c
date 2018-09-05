//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  This file contains FV before publishing hook

**/
#include <Token.h>
#include <AmiCspLib.h>
#include <AmiRomLayout.h>

/**
  This function checks whether TopSwap is ON and updates 
  FV_RECOVERY base address accordingly
  
  @param PeiServices Pointer to the Pointer to the Pei Services Table
  @param Area        Pointer to the RomArea being checked
  @param FvType      The attributes of the RomArea that is being checked

  @retval TRUE       Volume should be published

**/
BOOLEAN FtFvBeforePublishing(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN OUT AMI_ROM_AREA *Area, 
    IN UINT32 FvType
)
{
    const UINTN FvRecoveryBase = AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS;
    if(!IsTopSwapOn())  //TopSwap if OFF - do nothing
        return TRUE;

//update address of recovery FV to its backup copy
    if(Area->Address == FvRecoveryBase) {
        if(BUILD_TIME_BACKUP)
            Area->Address = AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS;
        else
            Area->Address = AMI_ROM_LAYOUT_FV_MAIN_ADDRESS + 0x10000;
    }
    return TRUE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

