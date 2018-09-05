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

/** @file KbcOhciDxe.c
    Functions for OHCI in Non-Smm mode.

**/

//---------------------------------------------------------------------------

#include "KbcOhci.h"
#include "KbcEmulDxe.h"

//---------------------------------------------------------------------------

extern BOOLEAN KbcPresent;
OHCI_EMUL_DEVICE Ohci_Hc_Array[] = {
#if defined(OHCI_EMUL_PCI_DEVICES)
        OHCI_EMUL_PCI_DEVICES,
#endif
        { 0xFF,0xFFFF,0xFFFFFFFF }
};

UINTN Ohci_Hc_Array_Size = sizeof(Ohci_Hc_Array) / sizeof(Ohci_Hc_Array[0]);

//status bits for SMI#, parenthesis is needed for ~ or other conversion
#define OHCI_TRAPBY_MASK        (HCE_CNTRL_EMULATION_INTERRUPT)

//enable bits for traps
#define OHCI_TRAPEN_MASK        (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EXT_IRQ_ENABLE)

volatile OHCI_LEG_SUP_REGS	*gLegSupRegs = NULL;

/**
    Check if trap status is set in OHCI HC.

    @param None

    @retval BOOLEAN

**/

BOOLEAN
NonSmmOhci_HasTrapStatus ()
{
    UINTN OhciCount;
    volatile EFI_PHYSICAL_ADDRESS   OHCIBar = 0;

    if (gLegSupRegs == NULL) {
        for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
            // Check for Invalid Device 
            if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
                continue;
            }
            if (Ohci_Hc_Array[OhciCount].Type == PciDevice) {
                OHCIBar = NonSmmReadPCIConfig( Ohci_Hc_Array[OhciCount].BusDevFunc, 0x10) ;
                if (OHCIBar != 0xFFFFFFFF && OHCIBar != 0) {
                    gLegSupRegs = (OHCI_LEG_SUP_REGS*)((OHCIBar & 0xFFFFFF00) + OHCI_HCE_CONTROL);
                    break;
                }
            } else if (Ohci_Hc_Array[OhciCount].Type == FixedMemory) {
                gLegSupRegs = (OHCI_LEG_SUP_REGS*)Ohci_Hc_Array[OhciCount].MemAddr;
                break;
            }
        }
        if (gLegSupRegs == NULL) {
            return FALSE;
        }
    }

    if(gLegSupRegs->HceControl & HCE_CNTRL_EMULATION_ENABLE) {
        return TRUE;
    }
    
    return FALSE;
}

/**
    Enable/Disable traping in OHCI HC.

    @param TrapEnable

    @retval BOOLEAN

    @note  Called outside SMM

**/

BOOLEAN
NonSmmOhci_TrapEnable (
    BOOLEAN  TrapEnable
)
{
    UINTN OhciCount;
    volatile EFI_PHYSICAL_ADDRESS   OHCIBar = 0;
    EFI_TPL   OldTpl;

    if (gLegSupRegs == NULL) {
        for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
            // Check for Invalid Device	
            if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
                continue;
            }
            if (Ohci_Hc_Array[OhciCount].Type == PciDevice) {
                OHCIBar = NonSmmReadPCIConfig( Ohci_Hc_Array[OhciCount].BusDevFunc, 0x10) ; 
                if (OHCIBar != 0xFFFFFFFF && OHCIBar != 0) {
                    gLegSupRegs = (OHCI_LEG_SUP_REGS*)((OHCIBar & 0xFFFFFF00) + OHCI_HCE_CONTROL);
                    break;
                }
            } else if (Ohci_Hc_Array[OhciCount].Type == FixedMemory) {
                gLegSupRegs = (OHCI_LEG_SUP_REGS*)Ohci_Hc_Array[OhciCount].MemAddr;
                break;
            }
        }
        if (gLegSupRegs == NULL) {
            return FALSE;
        }
    }

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

    if (TrapEnable) {
        // Initialize the status to default value
        gLegSupRegs->HceStatus = 0x1C;

        // Enable Traps and clear status(es) that might be asserted by our handlers
        if (!KbcPresent) {
            gLegSupRegs->HceControl = HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_A20_STATE;			
        } else {
            gLegSupRegs->HceControl = OHCI_TRAPEN_MASK | HCE_CNTRL_A20_STATE;
        }
    } else {
        // Disable Traps; Clear the status
        gLegSupRegs->HceControl &= ~OHCI_TRAPEN_MASK;
    }
    pBS->RestoreTPL(OldTpl);

    return TRUE;
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

