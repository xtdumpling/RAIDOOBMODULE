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

/** @file KbcEmulDxe.c
    KBC emulation driver entry point and installs
    NonSmmEmul6064TrapProtocol

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include "KbcEmulDxe.h"
#include "Protocol/UsbPolicy.h"
#include <Setup.h>
#if UHCI_EMUL_SUPPORT
#include "KbcUhci.h"
#endif
#if OHCI_EMUL_SUPPORT
#include "KbcOhci.h"
#endif
#if SB_EMUL_SUPPORT
#include "KbcSb.h"
#endif
#if IOTRAP_EMUL_SUPPORT
#include "KbcIoTrap.h"
#endif

//---------------------------------------------------------------------------

extern  EFI_GUID    gEfiLoadedImageProtocolGuid;
extern  EFI_GUID    gEfiDevicePathProtocolGuid;
EFI_GUID    gEfiSetupGuid = SETUP_GUID;
BOOLEAN     KbcPresent = FALSE;
UINT32 EFIAPI    KbcEmulationFeature (EFI_EMUL6064TRAP_PROTOCOL * This);
BOOLEAN EFIAPI   NonSmmEmulation6064TrapEnable(EFI_EMUL6064TRAP_PROTOCOL *This);
BOOLEAN EFIAPI   NonSmmEmulation6064TrapDisable(EFI_EMUL6064TRAP_PROTOCOL *This);
BOOLEAN EFIAPI   Get6064SmiTrapStatus ( EFI_EMUL6064TRAP_PROTOCOL * This);

EFI_EMUL6064TRAP_PROTOCOL       NonSmmEmulation6064Trap= {
    NonSmmEmulation6064TrapEnable,
    NonSmmEmulation6064TrapDisable,
    KbcEmulationFeature,
    Get6064SmiTrapStatus
};

EFI_CPU_IO2_PROTOCOL *CpuIo;

EFI_HANDLE      EmulationTrapHandle=NULL;

/**
    Entry point for Emulation driver. If the port 6064 emulation enabled by setup
    it enables the SMI event for port 6064 and install the Trap handle protocol.

    @param ImageHandle
    @param SystemTable

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
EmulationDxeEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                  Status;
    UINTN                       VariableSize;
    USB_SUPPORT_SETUP           SetupData;

    //
    // Load this driver's image to memory
    //
    InitAmiLib(ImageHandle,SystemTable);

    //
    //Check the setup option, if the emulation disabled in setup return with EFI_SUCCESS
    //
    VariableSize = sizeof(USB_SUPPORT_SETUP);
    Status = pRS->GetVariable( L"UsbSupport", &gEfiSetupGuid, NULL, &VariableSize, &SetupData );

    if (EFI_ERROR(Status) || (SetupData.UsbEmul6064 == 0)) {
        return EFI_SUCCESS;
    }

    Status = pBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Detect the KBC presence based on KBC_SUPPORT SDL token value if defined
    // else read port 0x64.
    //
#ifdef KBC_SUPPORT
    KbcPresent = KBC_SUPPORT;
#else
    KbcPresent = ((IoRead8(0x64)== 0xFF))? 0 : 1; 
#endif

    Status=pBS->InstallProtocolInterface(&EmulationTrapHandle,
                          &gNonSmmEmul6064TrapProtocolGuid,EFI_NATIVE_INTERFACE,&NonSmmEmulation6064Trap);

#if UHCI_EMUL_SUPPORT
    NonSmmUhci_SetTrapEnMask();
#endif
    return Status;
}

/**
    Get the Port 60/64 SMI enable status 

    @param This

    @retval BOOLEAN

**/

BOOLEAN
EFIAPI
Get6064SmiTrapStatus (
    EFI_EMUL6064TRAP_PROTOCOL           * This
)
{
#if UHCI_EMUL_SUPPORT
    return NonSmmUhci_HasTrapStatus();
#endif
#if OHCI_EMUL_SUPPORT
    return NonSmmOhci_HasTrapStatus();
#endif
#if SB_EMUL_SUPPORT
    return NonSmmSb_HasTrapStatus();
#endif
#if IOTRAP_EMUL_SUPPORT
    return NonSmmIoTrap_HasTrapStatus();
#endif
    return FALSE;
}

/**
    Enable the SMI source for port 6064 in Non SMM

    @param This

    @retval BOOLEAN

**/

BOOLEAN
EFIAPI
NonSmmEmulation6064TrapEnable (
    EFI_EMUL6064TRAP_PROTOCOL           * This
)
{
    return NonSmmTrapEnable(TRUE);
}

/**
    Disable the SMI source for port 6064 in Non SMM

    @param This

    @retval BOOLEAN

**/

BOOLEAN
EFIAPI
NonSmmEmulation6064TrapDisable (
    EFI_EMUL6064TRAP_PROTOCOL           * This
)
{
    return NonSmmTrapEnable(FALSE);
}

/**
    Returns all the feature supported by Emulation

    @param This

    @retval UINT32

**/

UINT32
EFIAPI
KbcEmulationFeature (
    EFI_EMUL6064TRAP_PROTOCOL           * This
)
{
    UINT32          KbcEmulFeature=0;

#if IRQ_EMUL_SUPPORT
    KbcEmulFeature |= IRQ_SUPPORTED;
#endif
    return  KbcEmulFeature;
}

/**
    Enable/disable the port6064 trap in Non SMM

    @param b

    @retval BOOLEAN

**/
BOOLEAN
NonSmmTrapEnable (
    BOOLEAN      b
)
{
#if UHCI_EMUL_SUPPORT
    return NonSmmUhci_TrapEnable(b);
#endif
#if OHCI_EMUL_SUPPORT
    return NonSmmOhci_TrapEnable(b);
#endif
#if SB_EMUL_SUPPORT
    return NonSmmSb_TrapEnable(b);
#endif
#if IOTRAP_EMUL_SUPPORT
    return NonSmmIoTrapEnable(b);
#endif
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

