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

/** @file KbcEmul.c
    KBC emulation driver entry point and initilize the SMI for port 6064
**/

//---------------------------------------------------------------------------
#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "Token.h"
#include "Protocol/UsbPolicy.h"
#include <Setup.h>


//---------------------------------------------------------------------------

KBC*    gVirtualKBC = NULL;
VOID    Emulation_Init();
VOID    Smm_Register();

BOOLEAN     KbcPresent = FALSE;
EFI_GUID    gEfiSetupGuid= SETUP_GUID;
BOOLEAN                   gTrackPs2Mouse4ByteCheck = TRACK_PS2MOUSE_4BYTE_PACKET;
USB_GLOBAL_DATA           *gUsbDataPtr;
EFI_USB_PROTOCOL          *USBCfg = NULL;

//
//KBC Emulation TRAP protocol.
//
BOOLEAN EFIAPI    Emulation6064TrapEnable(EFI_EMUL6064TRAP_PROTOCOL *This);
BOOLEAN EFIAPI    Emulation6064TrapDisable(EFI_EMUL6064TRAP_PROTOCOL *This);
UINT32 EFIAPI     KbcEmulationFeature (EFI_EMUL6064TRAP_PROTOCOL * This);
BOOLEAN EFIAPI    Get6064SmiTrapStatus (EFI_EMUL6064TRAP_PROTOCOL * This);

EFI_EMUL6064TRAP_PROTOCOL       Emulation6064Trap= {
    Emulation6064TrapEnable,
    Emulation6064TrapDisable,
    KbcEmulationFeature,
    Get6064SmiTrapStatus
};


EFI_HANDLE      EmulationTrapHandle=NULL;

#if PI_SPECIFICATION_VERSION >= 0x0001000A
EFI_SMM_CPU_PROTOCOL            *SmmCpuProtocol=NULL;
#endif

EFI_SMM_BASE2_PROTOCOL*         gSMM = NULL;
EFI_SMM_SYSTEM_TABLE2*          gSmst1 = NULL;


/**
    Entry point for Emulation driver. If the port 6064 emulation enabled by setup 
    it enables the SMI event for port 6064 and install the Trap handle protocol.    

    @param Standard  ImageHandle
    @param Standard  SystemTable

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
EmulationEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                  Status      = 0;
    UINTN                       VariableSize;
    USB_SUPPORT_SETUP           SetupData;


    Status = InitAmiSmmLib( ImageHandle, SystemTable );

    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    //
    //Check the setup option, if the emulation disabled in setup return with EFI_SUCCESS
    //
    VariableSize = sizeof(USB_SUPPORT_SETUP);
    Status = pRS->GetVariable( L"UsbSupport", &gEfiSetupGuid, NULL, &VariableSize, &SetupData );

    if (EFI_ERROR(Status) || (SetupData.UsbEmul6064 == 0)) {
        return EFI_SUCCESS;
    }

    Status = pBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **) &gSMM);
    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    gSMM->GetSmstLocation (gSMM, &gSmst1);


    //
    // We're now in SMM, registering all USB SMM callback functions
    //
    Emulation_Init();

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = gSmst1->SmmInstallProtocolInterface(&EmulationTrapHandle,
                   &gEmul6064TrapProtocolGuid,EFI_NATIVE_INTERFACE,&Emulation6064Trap);

    Status= gSmst1->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, (VOID**) &SmmCpuProtocol);

#else
    Status = gBS->InstallProtocolInterface(&EmulationTrapHandle,
                    &gEmul6064TrapProtocolGuid,EFI_NATIVE_INTERFACE,&Emulation6064Trap);
    ASSERT_EFI_ERROR(Status);

    Status= gBS->LocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &SmmCpuProtocol);
    ASSERT_EFI_ERROR(Status);
#endif

#if EMULATION_ACPI_ENABLE_DISPATCH
    RegisterAcpiEnableCallBack();
#endif
    
    if (KbcPresent & gTrackPs2Mouse4ByteCheck &  !USBCfg) {
        //
        // Retrieve global data from AMIUHCD
        //
        Status  = pBS->LocateProtocol (&gEfiUsbProtocolGuid, NULL, &USBCfg);
        ASSERT_EFI_ERROR(Status);
        if (!EFI_ERROR(Status)) {
            gUsbDataPtr = USBCfg->USBDataPtr;
        }
    }

    return EFI_SUCCESS;
}

/**
    Initilize the Keyboard Controller, Keyboard and Mouse device.Regsiter the SMI for port 6064
    access.

    @param VOID

    @retval VOID

**/

VOID
Emulation_Init ()
{
    static      VIRTKBC     VirtualKBC;
    static      VIRTMOUSE   VirtualMouse;
    static      VIRTKBD     VirtualKbd;
    static      LEGACYKBC       LegacyKbc;
    static      LEGACYKBD       LegacyKbd;
    static      LEGACYMOUSE     LegacyMouse;
    UINT16      IrqMask;
    EFI_STATUS  Status;

    //
    // Detect the KBC presence based on KBC_SUPPORT SDL token value if defined
    // else read port 0x64.
    //
#ifdef KBC_SUPPORT
    KbcPresent = KBC_SUPPORT;
#else
    KbcPresent = ((IoRead8(0x64)== 0xFF)) ? 0 : 1; 
#endif
    if(KbcPresent) {

        gVirtualKBC = &LegacyKbc.kbc_;

        //
        //Initialize the Legacy Mouse Device
        //
        InitLegacyMouse(gVirtualKBC,&LegacyMouse);

        //
        //Initialize the Legacy Keyboard Device
        //
        InitLegacyKbd(gVirtualKBC,&LegacyKbd);

        //
        //Initialize the Legacy Keyboard Controller
        //
        InitLegacyKBC(&LegacyKbc,&LegacyKbd.sink,&LegacyMouse.sink);


    } else {

        gVirtualKBC = &VirtualKBC.kbc;

        //
        //Initialize the Virtual Mouse Device
        //
        InitVirtualMouse(gVirtualKBC,&VirtualMouse);

        //
        //Initialize the Virtual Keyboard Device
        //
        InitVirtualKbd(gVirtualKBC,&VirtualKbd);

        //
        //Initialize the Virtual Keyboard Controller
        //
        InitVirtualKBC(&VirtualKBC,&VirtualKbd.sink,&VirtualMouse.sink);

        //
        // Reserve IRQ1 if the Emulation enabled
        //
        Status=AmiIsaIrqMask(&IrqMask, TRUE);

        if(Status==EFI_NOT_FOUND){
            IrqMask=ISA_IRQ_MASK;
            IrqMask |= 2;
            Status = AmiIsaIrqMask(&IrqMask, FALSE);
        } else {
            IrqMask |= 2;
            Status=AmiIsaIrqMask(&IrqMask, FALSE);
            ASSERT_EFI_ERROR(Status);
        }

    }

    //
    // Hardware specific SMM registration
    Smm_Register();

    return;
}

/**
    Get the Port 60/64 SMI enable status

    @param  This

    @retval  BOOLEAN

**/

BOOLEAN
EFIAPI
Get6064SmiTrapStatus (
    EFI_EMUL6064TRAP_PROTOCOL           *This
)
{
    return SmmGetTrapStatus();
}

/**
    Enable the SMI source for port 6064

    @param This

    @retval BOOLEAN

**/

BOOLEAN
EFIAPI
Emulation6064TrapEnable (
    EFI_EMUL6064TRAP_PROTOCOL           *This
)
{
    return TrapEnable(TRUE);
}

/**
    Disable the SMI source for port 6064

    @param This

    @retval BOOLEAN

**/

BOOLEAN
EFIAPI
Emulation6064TrapDisable (
    EFI_EMUL6064TRAP_PROTOCOL           *This
)
{
    return TrapEnable(FALSE);
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
