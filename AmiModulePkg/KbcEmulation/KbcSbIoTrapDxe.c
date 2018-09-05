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

/** @file KbcSbIoTrapDxe.c
    Functions for Io Trap in Non-Smm mode .

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "KbcSbIoTrap.h"

//---------------------------------------------------------------------------

/**
    Write the Value in to all the IoTrap configuration Regsiters.

    @param BOOLEAN Value

    @retval VOID

**/
VOID
NonSmmWriteIoTrapCfg (
    BOOLEAN  Value
)
{    
    if(Value == 1) {
    //
    // Open the IoTrap SMI
    //
    } else {
    //
    // Close the IoTrap SMI
    //
    }
}

/**
    Clear the Port6064 SMI Status

    @param UINT8 Value

    @retval VOID

**/
VOID
NonSmmClearIoTrapStatusReg ()
{
    //
    // Clear the IoTrap SMI status
    //
}

/**
    Get IoTrap Register Status.

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN
NonSmmGetIoTrapRegisterStatus ()
{
    //
    // Get the IoTrap SMI Enable or not
    //
    return 0;
}

/**
    Get IoTrap base address.

    @param VOID

    @retval UINT32

**/
UINT32
IoTrapBaseAddr ()
{
    //
    // Get the base address of IoTrap register
    //
    return 0;
}

/**
    Get IoTrap Register Address.

    @param UINT8

    @retval UINT32

**/
UINT32
GetIoTrapRegister (
    UINT8  IoPort
)
{    
    //
    // Get the base address of IoTrap register
    //
    return 0;
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

