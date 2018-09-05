//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file KbcSbIoTrapSmm.c
    Handles the SMI events for port 60/64

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "KbcSbIoTrap.h"

//---------------------------------------------------------------------------

/**
    Get IoTrap base address.

    @param None

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
    Get IoTrap base address.

    @param RegAddr
    @param Type

    @retval UINT32

**/

VOID
CheckTrappedCycleRegister (
    UINT32  *RegAddr,
    UINT8   *Type
)
{
    //
    // Get IoTrap Cycle Register
    //
    *RegAddr = 0;
    *Type = 0;
}

/**
    Get IoTrap Register Status.

    @param None

    @retval BOOLEAN

**/
BOOLEAN
GetIoTrapRegisterStatus ()
{
    //
    // Get the IoTrap SMI Enable or not
    //
    return 0;
}

/**
    Get IoTrap Register Address.

    @param IoPort

    @retval UINT32

**/

UINT32
GetIoTrapRegister(
    UINT8  IoPort
)
{
    //
    // Get information from Io Trap handler register
    //
    return 0;
}

/**
    Write the Value in to all the UHCI controller Legacy Regsiters.

    @param  Value

    @retval VOID

**/

VOID
WriteIoTrapCfg (
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
    Clear the Port6064 SMI Status Reg

    @param None

    @retval VOID

**/

VOID
ClearIoTrapStatusReg ()
{
    //
    // Clear the IoTrap SMI status
    //
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

