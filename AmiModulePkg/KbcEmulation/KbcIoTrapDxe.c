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

/** @file KbcIoTrapDxe.c
    Functions for IoTrap in Non-Smm mode .
**/

//---------------------------------------------------------------------------

#include "KbcIoTrap.h"
#include "KbcSbIoTrap.h"

//---------------------------------------------------------------------------

/**
    Check if trap status is set in SB.

    @param None

    @retval BOOLEAN

**/

BOOLEAN
NonSmmIoTrap_HasTrapStatus ()
{
    //
    // Check IoTrap SMI Enable or not
    //
    return NonSmmGetIoTrapRegisterStatus();
}

/**
    Enable/Disable traping in UHCI HC.

    @param TrapEnable

    @retval BOOLEAN

**/

BOOLEAN
NonSmmIoTrapEnable (
    BOOLEAN  TrapEnable
)
{
    //
    // Call NonSmmClearIoTrapStatusReg
    //
    NonSmmClearIoTrapStatusReg();

    //
    // Call NonSmmWriteIoTrapCfg function
    //
    NonSmmWriteIoTrapCfg(TrapEnable);
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

