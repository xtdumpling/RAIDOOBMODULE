//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//
//****************************************************************************

/** @file KbcSb.c
    Handles the SMI events for port 60/64

**/

//---------------------------------------------------------------------------
#include "Token.h"
#include "KbcEmul.h"
#include "KbcSb.h"
#include "KbcEmulLib.h"
#include "Kbc.h"

//---------------------------------------------------------------------------

extern KBC* gVirtualKBC;


//Carries the LEGKEY status information saved before enable/disable trapping from outside trap handler is
//performed; The saving of the status allows recovering legacy I/O accesss event in case when multiple
//sources are serviced at signle SMI# ( legacy I/O and USB keyboard interupt, for example)

static UINT16 SavedLegcyStatus = 0;
static BOOLEAN gInTrapHandler = FALSE;

VOID trap64w ();
VOID trap60w ();
VOID trap64r ();
VOID trap60r ();

TRAPDISPATCH trap_dispatcher[] = {
    { SB_TRAPBY64W,  trap64w },
    { SB_TRAPBY64R,  trap64r },
    { SB_TRAPBY60W,  trap60w },
    { SB_TRAPBY60R,  trap60r },
};

#define trap_dispatcher_size  (sizeof(trap_dispatcher)/sizeof(trap_dispatcher[0]))

VOID EFIAPI Trap6064_Handler (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_USB_REGISTER_CONTEXT  * DispatchContext
);

/**
    Enable the SMI for port 6064 access.

    @param None

    @retval VOID

**/
VOID
Smm_Register ()
{

    /// PORTING Required.
    /// Regsiter the SMI handler for the Emulation SMI
    /// By default Disable the Port 60/64 SMI's.

    return;

}

/**
    Write the Value in to all the Legacy USB Control Regsiter in SB.

    @param  Value

    @retval VOID

**/

VOID
WriteLegKeyReg (
    UINT16  Value
)
{
    /// PORTING Required
    /// Write the Trap Regsiter value 

    return;
}

/**
    Clear the Port6064 SMI Status Reg 

    @param Value

    @retval VOID

**/
VOID
ClearLegKeyStatusReg (
    UINT16  Value
)
{

    /// PORTING Required
    /// Clear the TRAP SMI status.

    return;
}

/**
    SMI handler to handle the 64write, 64read, 60 write and 60 read SMI.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval VOID

**/
VOID
EFIAPI
Trap6064_Handler (
    IN  EFI_HANDLE                    DispatchHandle,
    IN  EFI_SMM_USB_REGISTER_CONTEXT  *DispatchContext
)
{
    UINT16 LegcyKeyStatus=0;
    UINT32 TrapFunCount;

    /// Porting Required.
    /// Based on the TRAP SMI status call the dispatch handler function.
    /// Dispatch functions are listed in trap_dispatcher array.

    /// LegacyKeyStatus should be having the TRAP SMI value.
    /// !!!!Porting Required!!!.

    //
    // Set the variable that we are processing the Trap
    //
    gInTrapHandler = TRUE;

    //
    // Clear the Trap SMI status
    //
    ClearLegKeyStatusReg( LegcyKeyStatus | SB_TRAPBY_MASK);

    //
    // Disable Traps ( in responce to i/o handler can try to access a real KBC)
    //
    WriteLegKeyReg( (LegcyKeyStatus & ~SB_TRAPEN_MASK));

    //
    // Dispatch the interrupt depending on saved status
    //
    for( TrapFunCount = 0; TrapFunCount < trap_dispatcher_size; ++TrapFunCount ){
        if( (LegcyKeyStatus & trap_dispatcher[TrapFunCount].status_bit  ) != 0){
            (*trap_dispatcher[TrapFunCount].trap_function)();
        }
    }

    //
    // Clear the Status
    //
    ClearLegKeyStatusReg( LegcyKeyStatus | SB_TRAPBY_MASK);

    //
    // Enable Traps 
    //
    WriteLegKeyReg( LegcyKeyStatus | SB_TRAPEN_MASK);

    SavedLegcyStatus = 0;
    gInTrapHandler = FALSE;

    return;
}

/**
    Check if trap status is set in SB Legacy USB Control Register.

    @param None

    @retval BOOLEAN

**/

BOOLEAN
Sb_HasTrapStatus ()
{

    /// PORTING Needed depend on the SB 
    /// return TRUE/FALSE based on the Trap SMI status Regsiter 
    
    return FALSE;
}

/**
    Enable/Disable traping in UHCI HC.

    @param TrapEnable

    @retval BOOLEAN

**/

BOOLEAN
Sb_TrapEnable (
    BOOLEAN  TrapEnable
)
{

    if(gInTrapHandler)
        return FALSE;

    /// PORTING Needed depend on the SB
    /// Clear the Trap SMI status Regsiters
    /// Based on the Input Argument enable/Disable the Port 60/64 SMI's.

    // Enabled Successfully.
    return TRUE;
}

/**
    Generate IRQ12

    @param None

    @retval VOID

**/

VOID
GenerateSbIRQ12 ()
{

    /// PORTING REQUIRED.
    /// Port the SB regsiter to Generate the IRQ12
    
    return;
    
}

/**
    Generate IRQ1

    @param None

    @retval VOID

**/
VOID
GenerateSbIRQ1()
{

    /// PORTING REQUIRED.
    /// Port the SB regsiter to Generate the IRQ12
    
    return;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

