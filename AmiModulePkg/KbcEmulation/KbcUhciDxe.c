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

/** @file KbcUhciDxe.c
    Functions for UHCI in Non-Smm mode.

**/

//---------------------------------------------------------------------------

#include "KbcEmulDxe.h"
#include "KbcUhci.h"

//---------------------------------------------------------------------------
extern BOOLEAN  KbcPresent;
static UINT16 SavedLegcyStatus = 0;

static UINT16 UhciTrapByMask = UHCI_TRAPBY_MASK_WITHOUTKBC;
static UINT16 UhciTrapEnMask = UHCI_TRAPEN_MASK_WITHOUTKBC;

UHCI_EMUL_DEVICE Uhci_Hc_Array[] = {
#if defined(UHCI_EMUL_PCI_DEVICES)
        UHCI_EMUL_PCI_DEVICES, 
#endif
#if defined(INTEL_LEGACY_USB_KBC_EMUL_CONTROL_REG_PCI_DEVICES)
        INTEL_LEGACY_USB_KBC_EMUL_CONTROL_REG_PCI_DEVICES,
#endif        
        { 0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

UINTN Uhci_Hc_Array_Size = sizeof(Uhci_Hc_Array) / sizeof(Uhci_Hc_Array[0]);

/**
    Validate the UHCI controller.

    @param Uhc

    @retval Boolean

**/

BOOLEAN
NonSmmValidateUhc (
    UHCI_EMUL_DEVICE  *Uhc
)
{
    UINT32  Data32 = 0;

    // End of the Device List.
    if(Uhc->BusDevFunc == 0xFFFF) {
        return FALSE;
    }

    if (NonSmmReadPCIConfig (Uhc->BusDevFunc, 0) == 0xFFFFFFFF) {
        return FALSE;
    }

    Data32 = NonSmmReadPCIConfig (Uhc->BusDevFunc, 8) >> 8;
    if (Data32 != (*((UINT32*)&Uhc->InterfaceType) & 0x00FFFFFF)) {
        return FALSE;
    }

    return TRUE;
}

/**
    Write the Value in to all the UHCI controller Legacy Regsiters.

    @param  Value

    @retval VOID

**/

VOID
NonSmmWriteLegKeyReg (
   UINT16  Value
)
{
    UINTN UhciCount;

    for( UhciCount = 0; UhciCount < Uhci_Hc_Array_Size; ++UhciCount ){
        if (!NonSmmValidateUhc(&Uhci_Hc_Array[UhciCount])) {
            continue;
        }
        NonSmmWordWritePCIConfig( Uhci_Hc_Array[UhciCount].BusDevFunc, 
                        Uhci_Hc_Array[UhciCount].LegacyRegOffset, Value);
    }
    return;
}

/**
    Clear the Port6064 SMI Status 

    @param  Value

    @retval VOID

**/

VOID
NonSmmClearLegKeyStatusReg (
    UINT16  Value
)
{

    UINTN UhciCount;

#if ICH10_WORKAROUND
    //
    // Enable all the UCHI controller
    // In ICH10 chipset, we need to clear the Port 6064 SMI status in disabled controller
    // also. Otherwise it's keep on generating SMI
    //
    EnableAllUhciController();
#endif

    for( UhciCount = 0; UhciCount < Uhci_Hc_Array_Size; ++UhciCount ){
        if (!NonSmmValidateUhc(&Uhci_Hc_Array[UhciCount])) {
             continue;
        }
        NonSmmWordWritePCIConfig( Uhci_Hc_Array[UhciCount].BusDevFunc,
                        Uhci_Hc_Array[UhciCount].LegacyRegOffset, Value);
    }

#if ICH10_WORKAROUND
    //
    // Restore the UCHI controller's in RCBA Reg
    //
    RestoreUhciControllerStatus();
#endif

    return;
}

/**
    Check if trap status is set in UHCI HC.

    @param None

    @retval BOOLEAN

**/

BOOLEAN
NonSmmUhci_HasTrapStatus ()
{
    UINT16 LegcyKeyStatus = 0;
    UINTN UhciCount;

    for( UhciCount = 0; UhciCount < Uhci_Hc_Array_Size; ++UhciCount ){
        if (!NonSmmValidateUhc(&Uhci_Hc_Array[UhciCount])) {
             continue;
        }

        LegcyKeyStatus =(UINT16) NonSmmReadPCIConfig( Uhci_Hc_Array[UhciCount].BusDevFunc ,
                                        Uhci_Hc_Array[UhciCount].LegacyRegOffset );
        break;
    }
    return ((LegcyKeyStatus & UhciTrapEnMask) != 0 );
}

/**
    Enable/Disable trapping in UHCI HC.

    @param TrapEnable

    @retval BOOLEAN

**/

BOOLEAN
NonSmmUhci_TrapEnable (
    BOOLEAN  TrapEnable
)
{

    UINT16    LegcyKeyStatus;
    EFI_TPL     OldTpl;
    UINT16      UhciCount;
    
    for( UhciCount = 0; UhciCount < Uhci_Hc_Array_Size; ++UhciCount ){
        if (!NonSmmValidateUhc(&Uhci_Hc_Array[UhciCount])) {
             continue;
        }

        LegcyKeyStatus =(UINT16) NonSmmReadPCIConfig( Uhci_Hc_Array[UhciCount].BusDevFunc ,
                                        Uhci_Hc_Array[UhciCount].LegacyRegOffset );
        //
        //Record first time that trapping is disabled. Record only trap status bits handled by Trap handler
        //
        if( (LegcyKeyStatus & UhciTrapEnMask)== UhciTrapEnMask &&
            ( LegcyKeyStatus & UhciTrapByMask) != 0 ){
            //If legacy I/O caused SMI# and this is first time we are in uhci_trapEnable
            //then trapping in LEGKEY reg must have been enabled and one trap status is set.
            //Any port 60/64 operation within SMI# must be wrapped into
            //enable/ disable&clear status. So subsequent trapEnable will not produce any
            //trap statuses
            SavedLegcyStatus |= LegcyKeyStatus;
        }

        OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

        if(TrapEnable){
            //
            // Clear the status
            //
            NonSmmClearLegKeyStatusReg(LegcyKeyStatus | UhciTrapByMask);
            //
            // Enable Traps 
            //
            NonSmmWriteLegKeyReg( LegcyKeyStatus | UhciTrapEnMask);
        } else {
            //
            // Clear the status
            //
            NonSmmClearLegKeyStatusReg(LegcyKeyStatus | UhciTrapByMask);

            //
            // Disable the Trap
            //
            NonSmmWriteLegKeyReg( (LegcyKeyStatus & ~UhciTrapEnMask) );
        }
        pBS->RestoreTPL(OldTpl);
    }
    return TRUE;
}
/**
 *  Sets the UHCI Trap Enable Mask value by checking the presence of 
 *  KBC on the platform by reading port 64h.If KBC controller is present
 *  loads the global variables UhciTrapEnMask and UhciTrapByMask.
 *  If the platform has KBC then for Port 64h Read SMI will not be generated
 *  and handled by the KBC Emulation driver.
 *  
 *  @param  none
 *  @param  none
 *  
 */
void
NonSmmUhci_SetTrapEnMask()
{
    if(KbcPresent){
        UhciTrapByMask = UHCI_TRAPBY_MASK_WITHKBC;
        UhciTrapEnMask = UHCI_TRAPEN_MASK_WITHKBC;
    }
    return;
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

