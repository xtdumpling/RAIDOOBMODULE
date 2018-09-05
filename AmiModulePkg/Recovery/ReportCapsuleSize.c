//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2015, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file FsRecovery.c
    

**/

#include <AmiPeiLib.h>
#include <AmiHobs.h>

VOID ReportCapsuleSize (
        EFI_PEI_SERVICES **PeiServices,
        UINT32 Size
        )
{
    static EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_STATUS Status;
    RECOVERY_IMAGE_HOB *RecoveryHob;
    
    Status = (*PeiServices)->GetHobList(PeiServices, (VOID **)&RecoveryHob);
    if(!EFI_ERROR(Status)) {
        Status = FindNextHobByGuid(&RecoveryHobGuid, (VOID **)&RecoveryHob);
        if(EFI_ERROR(Status))
            return;
    }
    
    RecoveryHob->ImageSize = Size;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2015, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
