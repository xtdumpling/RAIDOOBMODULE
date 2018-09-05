//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file KbcSbDxe.c
    Functions for UHCI in Non-Smm mode .

**/

//---------------------------------------------------------------------------

#include "KbcEmulDxe.h"
#include "KbcSb.h"

//---------------------------------------------------------------------------

static UINT16 SavedLegcyStatus = 0;

/**
    Check if trap status is set in SB.

    @param None

    @retval BOOLEAN

**/

BOOLEAN
NonSmmSb_HasTrapStatus ()
{
    /// PORTING Needed depend on the SB 
    return FALSE;
}

/**
    Enable/Disable traping in UHCI HC.

    @param TrapEnable

    @retval BOOLEAN

**/

BOOLEAN
NonSmmSb_TrapEnable (
    BOOLEAN TrapEnable
)
{

    EFI_TPL     OldTpl;

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

    /// PORTING Needed depend on the SB 
    /// Clear the Trap SMI status Regsiters
    /// Based on the Input Argument enable/Disable the Port 60/64 SMI's.

    pBS->RestoreTPL(OldTpl);

    // Enabled Successfully.
    return TRUE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

