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
//**********************************************************************

/** @file KbcOhci.c
    Handles the SMI events for port 60/64

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "KbcOhci.h"
#include "KbcEmulLib.h"
#include "Kbc.h"
#include <Library/AmiBufferValidationLib.h>

//---------------------------------------------------------------------------

extern KBC*     gVirtualKBC;
extern BOOLEAN  KbcPresent;

//Carries the LEGKEY status information saved before enable/disable trapping from outside trap handler is
//performed; The saving of the status allows recovering legacy I/O accesss event in case when multiple
//sources are serviced at signle SMI# ( legacy I/O and USB keyboard interupt, for example)

static BOOLEAN  gInTrapHandler = FALSE;
BOOLEAN         KBDDisabled = FALSE;

OHCI_EMUL_DEVICE Ohci_Hc_Array[] = {
#if defined(OHCI_EMUL_PCI_DEVICES)
        OHCI_EMUL_PCI_DEVICES,
#endif
        { PciDevice ,0xFFFF,0xFFFFFFFF }
};

UINTN Ohci_Hc_Array_Size = sizeof(Ohci_Hc_Array) / sizeof(Ohci_Hc_Array[0]);

volatile OHCI_LEG_SUP_REGS	*gLegSupRegs = NULL;

VOID DisableLegKeyRegs();
VOID trap64w();
VOID trap60w();
VOID trap64r();
VOID trap60r();
UINT8 GetHceInput();
VOID SetHceOutput (UINT8 Data);
BOOLEAN KBC_WaitForOutputBufferToBeFilled();
BOOLEAN CheckPS2KBDMouseIRQEnabled();
VOID CheckNumberofResponseBytes ();
VOID WaitForOBFSMM();
VOID SerialOutput (UINT8 Data);
BOOLEAN gClearCharacterPending = FALSE;
BOOLEAN MouseCmdInProgress = FALSE;

// cmd, response, cmd, response ... 0
UINT8 KBDCmdResponse[] =   {0xF2, 3, 0xFF, 2, 0xF0, 1, 0xF3, 1, 0xF4, 1, 0xF5, 1, 0xF6, 1, 0xF7, 1, 0xF8, 1, 0xF9, 1, 0xFA, 1, 0xFB, 1, 0xFC, 1, 0xFD, 1, 0xED ,1, 0};
UINT8 MouseCmdResponse[] = {0xFF, 3, 0xF2, 2, 0xE9, 4, 0xEb, 4, 0xE6, 1, 0xE7, 1, 0xE8, 1, 0xEA, 1, 0xEE, 1, 0xF0, 1, 0xF3, 1, 0xF4, 1, 0xF5, 1, 0};
UINT8 ResponseBytes = 0;

//status bits for SMI#, parenthesis is needed for ~ or other conversion
#define OHCI_TRAPBY_MASK        (HCE_CNTRL_EMULATION_INTERRUPT)

//enable bits for traps
#define OHCI_TRAPEN_MASK        (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EXT_IRQ_ENABLE)

VOID
EFIAPI
Trap6064_Handler (
    IN  EFI_HANDLE                    DispatchHandle,
    IN  EFI_SMM_USB_REGISTER_CONTEXT  * DispatchContext
);

VOID
EFIAPI
Trap6064_Handler_LegFree (
    IN  EFI_HANDLE                    DispatchHandle,
    IN  EFI_SMM_USB_REGISTER_CONTEXT  * DispatchContext
);

/**
    Enable the SMI for port 6064 access.

    @param  None

    @retval VOID

    Note :  Called inside SMM

**/

VOID
Smm_Register ()
{
    EFI_SMM_USB_REGISTER_CONTEXT    context;
    EFI_SMM_USB_DISPATCH2_PROTOCOL  *pDispatch;
    EFI_HANDLE hDisp;
    EFI_STATUS Status;
    static FULL_USB_DEVICE_PATH hc_dp =  USB1_1_DEVICE_PATH;
    UINTN OhciCount;

    //
    // Register the USB HW SMI handler
    //
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = gSmst1->SmmLocateProtocol(
            &gEfiSmmUsbDispatch2ProtocolGuid,
            NULL,
            &pDispatch);
#else
    Status = gBS->LocateProtocol(
            &gEfiSmmUsbDispatchProtocolGuid,
            NULL,
            &pDispatch);
#endif
    ASSERT_EFI_ERROR(Status);

    //
    // SMI registration routines will install the handlers, set enable bit
    // and clear status in PM IO space.
    //
    for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
        // Check for Invalid Device	

        if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
            continue;
        }

        hc_dp.pci.Device=(Ohci_Hc_Array[OhciCount].BusDevFunc >> 3);
        hc_dp.pci.Function=(Ohci_Hc_Array[OhciCount].BusDevFunc & 07);
        context.Type = UsbLegacy;
        context.Device = (EFI_DEVICE_PATH_PROTOCOL *)&hc_dp;

        if (!KbcPresent) {
            Status = pDispatch->Register(pDispatch,(EFI_SMM_HANDLER_ENTRY_POINT2)Trap6064_Handler_LegFree, &context, &hDisp);
            ASSERT_EFI_ERROR(Status);
        } else {
            Status = pDispatch->Register(pDispatch,(EFI_SMM_HANDLER_ENTRY_POINT2)Trap6064_Handler, &context, &hDisp);
            ASSERT_EFI_ERROR(Status);
        }
    }

    //The SMI source for the port6064 is disabled. it will be enabled when we are in legacy enviorment.
    //EFI enviorment Emulation is Disabled.
    DisableLegKeyRegs();
}

/**
    Disable the port6064 SMI source based on the Trap enable mask.

    @param None

    @retval VOID

        Note :       Called inside SMM

**/

VOID
DisableLegKeyRegs ()
{

    if (gLegSupRegs != NULL) {
        gLegSupRegs->HceControl = 0;
    }

    return;

}

/**
    SMI triggered becauase of GA20 sequence change

    @param None

    @retval VOID

    Note :  Called inside SMM

**/

VOID
HandleGA20Sequence ()
{
    UINT8   Data;
    
    if (!KbcPresent) {
        //
        //Legacy Free system. Handle it using PORT 92
        //
        Data = GetHceInput();
        IoWrite8(0x92, (IoRead8(0x92) & 0xFD) | (Data & 0x2));
        SetHceControl(GetHceControl() & 0xFFFFFE3F | ((Data & 0x2) << 7));
    }
    else {
        //
        //Legacy Free system. Handle it using PORT 92. Maybe need to be chnaged if KBC needs to be used.
        //
        Data = GetHceInput();
        IoWrite8(0x92, (IoRead8(0x92) & 0xFD) | (Data & 0x2));
        SetHceControl(GetHceControl() & 0xFFFFFE3F | ((Data & 0x2) << 7));
    }
    ((VIRTKBC* )gVirtualKBC)->st_ = 0x1c;

    return;
}

/**
    SMI handler to handle the 64write, 64read, 60 write and 60 read SMI.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval void

    Note :  Called inside SMM
    Used only when KBC is NOT present on the system

**/

VOID
EFIAPI
Trap6064_Handler_LegFree (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_USB_REGISTER_CONTEXT  *DispatchContext
)
{
    UINT8 LegacyStatus;
    UINTN OhciCount;
    volatile EFI_PHYSICAL_ADDRESS   OHCIBar = 0;
    VIRTKBD* kbd = (VIRTKBD*)gVirtualKBC->kbd_dev;
    VIRTMOUSE* mouse = (VIRTMOUSE*)gVirtualKBC->mouse_dev;

    ((VIRTKBC* )gVirtualKBC)->DelaySendingDataCmd = FALSE;

    //
    // check if gOHCIBar has a valid address
    //
    if (gLegSupRegs == NULL) {
        for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
            // Check for Invalid Device	
            if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
                continue;
            }
            
            if (Ohci_Hc_Array[OhciCount].Type == PciDevice) {
                OHCIBar = ReadPCIConfig( Ohci_Hc_Array[OhciCount].BusDevFunc, 0x10) ; 
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
            return;
        } else {
            // Check if MMIO address space of Legacy Support registers resides in SMRAM region. If yes, don't proceed.
            if( AmiValidateMmioBuffer( (VOID*)gLegSupRegs, sizeof(OHCI_LEG_SUP_REGS) ) ) {
                return;
            }
        }

    }

    //
    // Check emulation is enabled if not exit.
    //
    if ((GetHceControl() & (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EMULATION_INTERRUPT)) != 
            (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EMULATION_INTERRUPT)) {
        return;
    }

    LegacyStatus = GetHceStatus();

    //
    // Set the variable that we are processing the Trap
    //
    gInTrapHandler = TRUE;

    //
    // Disable Emulation ( in responce to i/o handler can try to access a real KBC)
    //
    SetHceControl(GetHceControl() & (~(HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_IRQ_ENABLE)));

    //
    // if Character Pending interrupt is enabled and OBF is not set, handle Port read 60
    //
    if (GetHceControl() & HCE_CNTRL_CHARACTER_PENDING && !(LegacyStatus & HCE_STS_OUTPUTFULL)){
        trap60r();
    } else {
        //
        //Clear OBF in internal structure
        //
        ((VIRTKBC*)gVirtualKBC)->st_ &= (~(KBC_STATUS_OBF | KBC_STATUS_AUXB));
        // OBF is not full, clear AUX BUF also
        if (!(LegacyStatus & KBC_STATUS_OBF)) {
            LegacyStatus &=  ~KBC_STATUS_AUXB;
        }
        //
        // Update the Internal stucture with the correct value
        //
        ((VIRTKBC*)gVirtualKBC)->st_ |= LegacyStatus & (KBC_STATUS_OBF | KBC_STATUS_AUXB);
    }

    //
    //Check for GA20 Sequence change
    //
    if ((LegacyStatus & HCE_STS_INPUTFULL) && GetHceControl() & HCE_CNTRL_GA20_SEQ){
        HandleGA20Sequence();
    } else {
        // Dispatch the interrupt depending on saved status
        if (LegacyStatus & HCE_STS_INPUTFULL) {
            if (LegacyStatus & HCE_STS_CMDDATA) {
                trap64w();
            } else {
                trap60w();
            }
        }
    }

    // Update Status
    SetHceStatus(((VIRTKBC* )gVirtualKBC)->st_);

    // If more data needs to be sent set Character Pending interrupt
    if (kbd->qhead != kbd->qtail || mouse->qhead != mouse->qtail) {
        SetHceControl(GetHceControl() | HCE_CNTRL_CHARACTER_PENDING);
        gClearCharacterPending = FALSE;
    } else {
        if (gClearCharacterPending == TRUE){
            SetHceControl(GetHceControl() & (~HCE_CNTRL_CHARACTER_PENDING));
            gClearCharacterPending = FALSE;
        } else {
            // Clear Character Pending bit and OBF in next SMI
            gClearCharacterPending = TRUE;
        }
    }

    //
    // Enable Traps
    //
    SetHceControl(GetHceControl() | HCE_CNTRL_EMULATION_ENABLE);
    gInTrapHandler = FALSE;

    return;
}

/**
    SMI handler to handle the 64write, 64read, 60 write and 60 read SMI.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval VOID

    Note :  Called inside SMM
    Used only when KBC is present on the system

**/

VOID
EFIAPI
Trap6064_Handler (
    IN  EFI_HANDLE                    DispatchHandle,
    IN  EFI_SMM_USB_REGISTER_CONTEXT  *DispatchContext
)
{
    UINT8 EmulationStatus;
    UINTN OhciCount;
    volatile EFI_PHYSICAL_ADDRESS   OHCIBar = 0;
    UINT8   Data;
    VIRTMOUSE* mouse = (VIRTMOUSE*)gVirtualKBC->mouse_dev;
    VIRTKBD* kbd = (VIRTKBD*)gVirtualKBC->kbd_dev;
    //
    // check if gOHCIBar has a valid address
    //
    if (gLegSupRegs == NULL) {
        for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
            // Check for Invalid Device	
            if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
                continue;
            }
            if (Ohci_Hc_Array[OhciCount].Type == PciDevice) {
                OHCIBar = ReadPCIConfig( Ohci_Hc_Array[OhciCount].BusDevFunc, 0x10) ; 
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
            return;
        } else {
            // Check if MMIO address space of Legacy Support registers resides in SMRAM region. If yes, don't proceed.
            if( AmiValidateMmioBuffer( (VOID*)gLegSupRegs, sizeof(OHCI_LEG_SUP_REGS) ) ) {
                return;
            }
        }
    }

    //
    // Check emulation is enabled if not exit.
    //
    if ((GetHceControl() & (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EMULATION_INTERRUPT)) != 
                  (HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_EMULATION_INTERRUPT)) {
            return;
    }

    ((VIRTKBC*)gVirtualKBC)->st_ = EmulationStatus = GetHceStatus();

    // Set the variable that we are processing the Trap
    gInTrapHandler = TRUE;

    //
    // Disable Emulation ( in response to i/o handler can try to access a real KBC)
    //
    SetHceControl(GetHceControl() & (~(HCE_CNTRL_EMULATION_ENABLE | HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE)));

    //
    // Check if IRQ1 and IRQ12 is active
    //
    if (GetHceControl() & (HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE )){

        // Update Status
        Data = ByteReadIO(KBC_STATUS_REG);

        if (Data & (KBC_STATUS_OBF | KBC_STATUS_AUXB)) {
            // Update Data
            SetHceOutput(ByteReadIO(KBC_DATA_REG));
            ((VIRTKBC* )gVirtualKBC)->DelaySendingDataCmd = TRUE;
            SerialOutput(GetHceOutput());
            if (ResponseBytes) {
                ResponseBytes--;
            }
        } else {
            // No data but only IRQ, then use emulation status
            Data = EmulationStatus;
        }

        //
        // Clear the IRQ12/IRQ1 Active bit. Enable IRQEn only if data is pending
        //
        if ((Data & (KBC_STATUS_OBF | KBC_STATUS_AUXB)) == (KBC_STATUS_OBF | KBC_STATUS_AUXB)) {
            SetHceControl (GetHceControl() | HCE_CNTRL_IRQ12_ACTIVE | HCE_CNTRL_IRQ_ENABLE);
        } else if ((Data & (KBC_STATUS_OBF | KBC_STATUS_AUXB)) == KBC_STATUS_OBF ) {
            SetHceControl (GetHceControl() | HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ_ENABLE);
        }

        //
        // This is a catch all condition. If IRQ generatd with out data just clear the status.
        //
        if (!(Data & KBC_STATUS_OBF)) {
            SetHceControl (GetHceControl() | HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE | HCE_CNTRL_IRQ_ENABLE);
        }

        SetHceStatus(Data);
        SerialOutput(Data);
    } else {

        //
        // Enable HCE_CNTRL_EXT_IRQ_ENABLE
        //
        SetHceControl((GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE |HCE_CNTRL_IRQ_ENABLE)) | HCE_CNTRL_EXT_IRQ_ENABLE);
        ((VIRTKBC* )gVirtualKBC)->DelaySendingDataCmd = FALSE;

        //
        // if Character Pending interrupt is enabled and OBF is not set, handle Port read 60
        //
        if (GetHceControl() & HCE_CNTRL_CHARACTER_PENDING && !(EmulationStatus & HCE_STS_OUTPUTFULL)) {
            if (ResponseBytes) {
                if (kbd->qhead != kbd->qtail || mouse->qhead != mouse->qtail) {
                    trap60r();
                } else {
                    WaitForOBFSMM();
                }
            }
            if(!CheckPS2KBDMouseIRQEnabled()) {
               Data=ByteReadIO(KBC_STATUS_REG);
               if (Data & KBC_STATUS_OBF) {
                   ResponseBytes--;
                   SetHceOutput(ByteReadIO(KBC_DATA_REG));
                   SerialOutput(GetHceOutput());
                   ((VIRTKBC* )gVirtualKBC)->st_ = Data;
               }
            }
        }

        //Check for GA20 Sequence change
        if ((EmulationStatus & HCE_STS_INPUTFULL) && GetHceControl() & HCE_CNTRL_GA20_SEQ){
            HandleGA20Sequence();
            // Clear the i/p full bit once the cmd/data is processed
            ((VIRTKBC*)gVirtualKBC)->st_ = ((VIRTKBC*)gVirtualKBC)->st_ & ~(HCE_STS_INPUTFULL);
        } else {
            // Dispatch the interrupt depending on saved status
            if (EmulationStatus & HCE_STS_INPUTFULL) {
                if (EmulationStatus & HCE_STS_CMDDATA) {
                    MouseCmdInProgress = FALSE;
                    if (GetHceInput() == 0xD4) {
                        MouseCmdInProgress = TRUE;
                    }
                    SerialOutput(GetHceInput());
                    trap64w();
                    // CCB Response Pending
                    if(GetHceInput() == 0x20) {
                        if (!CheckPS2KBDMouseIRQEnabled()) {
                            WaitForOBFSMM();
                            Data=ByteReadIO(KBC_STATUS_REG);
                            if (Data & KBC_STATUS_OBF) {
                                SetHceOutput(ByteReadIO(KBC_DATA_REG));
                                ((VIRTKBC* )gVirtualKBC)->st_ = Data;
                                SerialOutput(GetHceOutput());
                            }
                        }
                    }
                } else {
                    SerialOutput(GetHceInput());
                    trap60w();
                    CheckNumberofResponseBytes();
                    if (!CheckPS2KBDMouseIRQEnabled()) {
                    	if(ResponseBytes) {
                    		WaitForOBFSMM();
                    	}
                        Data=ByteReadIO(KBC_STATUS_REG);
                        if (!(Data & KBC_STATUS_OBF)) {
                            ResponseBytes = 0;
                        }
                        //
                        // if no data in emulation o/p register then update it
                        //
                        if (!(GetHceStatus() & KBC_STATUS_OBF) && (Data & KBC_STATUS_OBF)){
                            // Update Data
                            SetHceOutput(ByteReadIO(KBC_DATA_REG));
                            SerialOutput(GetHceOutput());
                            ((VIRTKBC* )gVirtualKBC)->st_ = Data;
                            if (ResponseBytes) {
                                ResponseBytes--;
                            }
                        }
                    }
                    MouseCmdInProgress = FALSE;
                }
                // Clear the i/p full bit once the cmd/data is processed
                ((VIRTKBC*)gVirtualKBC)->st_ = ((VIRTKBC*)gVirtualKBC)->st_ & ~(HCE_STS_INPUTFULL);
            }
        }

        // Update Status
        SetHceStatus(((VIRTKBC* )gVirtualKBC)->st_);

        if (ResponseBytes) {
            SetHceControl(GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE) | HCE_CNTRL_CHARACTER_PENDING);
        } else {
            SetHceControl(GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE) & (~HCE_CNTRL_CHARACTER_PENDING));
        }
    }

    //
    // Enable Traps
    //
    SetHceControl(GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE) | HCE_CNTRL_EMULATION_ENABLE);
    gInTrapHandler = FALSE;

    return;
}

/**
    Enable/Disable traping in OHCI HC.

    @param TrapEnable

    @retval BOOLEAN

    Note :  Called inside SMM
**/

BOOLEAN
Ohci_TrapEnable (
    BOOLEAN  TrapEnable
)
{
    UINTN OhciCount;
    volatile EFI_PHYSICAL_ADDRESS   OHCIBar = 0;

    if(gInTrapHandler)
        return FALSE;

    // The idea is, since data has not been pushed, don't let USB module to send more data. By keeping emulation enabled
    // emulation status will be sent, which will return O/P full so next byte will not be sent from USB
    if (GetHceStatus() & KBC_STATUS_OBF) {
        return FALSE;
    }

    if (gLegSupRegs == NULL) {
        for( OhciCount = 0; OhciCount < Ohci_Hc_Array_Size; ++OhciCount ){
            // Check for Invalid Device	
            if( Ohci_Hc_Array[OhciCount].Type == PciDevice && Ohci_Hc_Array[OhciCount].BusDevFunc == 0xFFFF) {
                continue;
            }
            if (Ohci_Hc_Array[OhciCount].Type == PciDevice) {
                OHCIBar = ReadPCIConfig( Ohci_Hc_Array[OhciCount].BusDevFunc, 0x10) ; 
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
        } else {
            // Check if MMIO address space of Legacy Support registers resides in SMRAM region. If yes, don't proceed.
            if( AmiValidateMmioBuffer( (VOID*)gLegSupRegs, sizeof(OHCI_LEG_SUP_REGS) ) ) {
                return FALSE;
            }
        }
    }

    if(TrapEnable) {
        //
        // Enable Traps
        //
    	if(!KbcPresent) {
            SetHceControl(GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE) | HCE_CNTRL_EMULATION_ENABLE);
    	} else {
    		SetHceControl(GetHceControl() & ~(HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE) | OHCI_TRAPEN_MASK);
    	}
    } else {
        //
        // Disable Traps;
        //
    	SetHceControl((GetHceControl() & ~OHCI_TRAPEN_MASK) | HCE_CNTRL_IRQ1_ACTIVE | HCE_CNTRL_IRQ12_ACTIVE);
    }
    return TRUE;
}

/**
    Return whether Emulation Interrupt has been generated or not

    @param None

    @retval BOOLEAN

        Note :       Called inside SMM. No usage for now. Present for compatible reason with OHCI.
**/
BOOLEAN
Ohci_HasTrapStatus ()
{
    if(gLegSupRegs == NULL ) {
        return FALSE;
    }
    
    //
    // Check emulation is enabled if not exit.
    //
    if ( GetHceControl() & HCE_CNTRL_EMULATION_ENABLE ) {
        return TRUE;
    }
    
    return FALSE;
}

/**
    Reads the HceInput register which holds the data written to port 60/64

    @param None

    @retval UINT8

    Note :  Called only inside SMM
**/

UINT8
GetHceInput ()
{
    return (UINT8)(gLegSupRegs->HceInput & 0xFF);

}

/**
    Reads the HceOutPut register in OHCI controller

    @param None

    @retval UINT8

    Note :  Called only inside SMM
**/

UINT8
GetHceOutput ()
{
    return (UINT8)(gLegSupRegs->HceOutput & 0xFF);
}

/**
    Reads the HceStatus register in OHCI controller

    @param None

    @retval UINT8

    Note :  Called only inside SMM
**/

UINT8
GetHceStatus ()
{
    return (UINT8)(gLegSupRegs->HceStatus & 0xFF);
}

/**
    Writes to HceStatus in OHCI controller

    @param Data

    @retval VOID

        Note :       Called only inside SMM
**/

VOID
SetHceStatus (
    UINT8  Data
)
{
    gLegSupRegs->HceStatus = (UINT32)Data;
    return;
}

/**
    Writes to HceOutput which is returned on IO port 60h read

    @param Data

    @retval VOID

    Note :  Called only inside SMM
**/

VOID
SetHceOutput (
    UINT8  Data
)
{
    gLegSupRegs->HceOutput = (UINT32)Data;
    return;
}

/**
    Writes to HceControl in OHCI controller

    @param UINT8

    @retval VOID

    Note :  Called only inside SMM
**/

VOID
SetHceControl (
    UINT16 Data
)
{
    gLegSupRegs->HceControl = (UINT32)Data;
    return;
}

/**
    Reads the HceStatus register in OHCI controller

    @param None

    @retval UINT16

    Note :  Called only inside SMM
**/

UINT16
GetHceControl ()
{
    return (UINT16)(gLegSupRegs->HceControl & 0x1FF);
}

/**
    Wait till O/P buffer is full

    @param None

    @retval VOID

    Note :  Called only inside SMM
**/

VOID
WaitForOBFSMM ()
{
    UINT8   bCount = 8;
    UINT8   bStatus;
    UINT32  wCount;
    do {
        wCount = 0xffff;
        bStatus = (UINT8)(ByteReadIO(KBC_STATUS_REG) & BIT0);
        while (!bStatus) {
            if(wCount == 0) break;
                bStatus = (UINT8)(ByteReadIO(KBC_STATUS_REG) & BIT0);
                wCount--;
            }
            bCount--;
    } while (bCount && (!bStatus));

    return;
}

/**
    Check whether PS2 KBD/Mouse IRQ has been enabled or not

    @param None

    @retval BOOLEAN

    Note :  Called only inside SMM
**/

BOOLEAN
CheckPS2KBDMouseIRQEnabled ()
{

    LEGACYKBC* legkbc = _CR(gVirtualKBC,LEGACYKBC,kbc_);

    // If the device and IRQ is enabled in CCB return TRUE else return FALSE
    // The idea being, we don't have to wait for the response after the cmd is issued to the device.
    // IRQ will be generated when the response is ready.

    if (!legkbc->fCcb_loaded) {
        return FALSE;
    }

    if (MouseCmdInProgress){
        if (!(legkbc->ccb_ & KBC_CCB_EN2) && (legkbc->ccb_ & KBC_CCB_INT2)) {
            return TRUE;
        }
    } else {
        if (!(legkbc->ccb_ & KBC_CCB_EN) && (legkbc->ccb_ & KBC_CCB_INT)) {
            return TRUE;
        }
    }

    return FALSE;

}

/**
    Based on KBD/Mouse cmd, determine how many response bytes should be provided

    @param None

    @retval VOID

    Note :  Called only inside SMM
**/

VOID
CheckNumberofResponseBytes ()
{
    UINT8   bData;
    UINT8*  CmdResponseArray;

    ResponseBytes = 0;
    bData = GetHceInput();
    if (MouseCmdInProgress) {
        ResponseBytes = 1;
        CmdResponseArray = MouseCmdResponse;
    } else  {
        CmdResponseArray = KBDCmdResponse;
    }

    do {
        if (*CmdResponseArray == bData) {
            ResponseBytes = *(CmdResponseArray +1);
            break;
        }
        CmdResponseArray+=2;
    } while (*CmdResponseArray);

}

/**
    Directly writes to 3F8 COM port. Using this routine will be faster and
    debug output delay will have no impact on debugging time sensitive routines
    O/P the byte and give a space after that;

    @param Data -Byte to output

    @retval VOID

**/

//
VOID
SerialOutput (
    UINT8   Data
)
{
/*
    UINT8 HigherNibble = Data >> 4;
    UINT8 LowerBibble = Data & 0xF;

    HigherNibble = HigherNibble > 9 ? HigherNibble + 0x37 : HigherNibble + 0x30;
    IoWrite8 (0x3f8, HigherNibble);

    LowerBibble = LowerBibble > 9 ? LowerBibble + 0x37 : LowerBibble + 0x30;
    IoWrite8 (0x3f8, LowerBibble);

    // Space
    IoWrite8 (0x3f8, 0x20);
*/
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
//**********************************************************************

