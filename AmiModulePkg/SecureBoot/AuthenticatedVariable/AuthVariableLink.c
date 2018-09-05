//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Setup.h>
#include "AuthVariable.h"
//
// Global defines and external variables
// 
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   PhysicalUserPresent
//
//  Description: Default implementation for Physical User Presence detection.
//               Confirmation the Administrative User has signed in;
//
//  Input:  NONE
//
//  Output: BOOLEAN
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN PhysicalUserPresent ( VOID  ) 
{
    EFI_STATUS    Status;
static EFI_GUID  SystemAccessGuid  = SYSTEM_ACCESS_GUID;
    SYSTEM_ACCESS SystemAccess = {SYSTEM_PASSWORD_USER};
    UINTN         Size;
    UINT32        Attributes;

    Size = sizeof(SYSTEM_ACCESS);
    Status = DxeGetVariable(L"SystemAccess", &SystemAccessGuid, &Attributes, &Size, &SystemAccess);
//AVAR_TRACE((TRACE_ALWAYS,"Admin=%x, %r, sz=%x, attr=%x\n", (SystemAccess.Access)?0:1, Status, Size, Attributes));
    if (!EFI_ERROR(Status) && (Attributes == EFI_VARIABLE_BOOTSERVICE_ACCESS)
         // Confirm the hook is called from inside of TSE Setup: Access==SYSTEM_PASSWORD_ADMIN
        && SystemAccess.Access==SYSTEM_PASSWORD_ADMIN)
    {
       return TRUE;
    }
    return FALSE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
