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

/** @file PeiUhci.c
    This file is the main source file for the UHCI PEI USB
    recovery module

**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include "UsbPei.h"
#include <Token.h>

#if UHCI_PEI_SUPPORT


//----------------------------------------------------------------------------
// Global Constants
//----------------------------------------------------------------------------

#define PCI_REG_UHCI_COMMAND_REGISTER         0x04
#define PCI_REG_UHCI_BASE_ADDRESS_REGISTER    0x20
#define PCI_REG_UHCI_LEGACY_CONTROL_REGISTER  0xc0
#define PCI_REG_UHCI_RESUME_ENABLE_REGISTER   0xc4

#define PEI_UHCI_CONTROLLER                   0x01

#define PCI_REG_EHCI_MEMORY_BAR               0x10

#define EHCI_HC_OPERATIONAL_CONFIG_FLAG_REG     0x40

UINTN gUhciPciReg[] = PEI_UHCI_PCI_DEVICES;
#define MAX_USB_CTRLERS  sizeof(gUhciPciReg) / sizeof(UINTN)


//----------------------------------------------------------------------------
// Type definitions
//----------------------------------------------------------------------------

typedef struct
{
    //  UINTN                   Signature;
    //  PEI_USB_CONTROLLER_PPI  UsbControllerPpi;
    //  EFI_PEI_PPI_DESCRIPTOR      PpiList;
    //  PEI_PCI_CFG_PPI         *PciCfgPpi;
    //  UINTN                   TotalUsbControllers;
    UINTN IoBase[MAX_USB_CTRLERS];
} PCI_UHCI_DEVICE;

//----------------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------------

EFI_STATUS GetUhciController (
    IN EFI_PEI_SERVICES           **PeiServices,
    IN PEI_USB_CONTROLLER_PPI     *This,
    IN UINT8                      UsbControllerId,
    IN UINTN                      *ControllerType,
    IN UINTN                      *IoBaseAddress );

EFI_STATUS
EnableUhciController (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfgPpi,
    IN UINTN                BaseAddress,
    IN UINT8                UsbControllerId );

VOID
ClearEhciConfigFlagReg (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfgPpi );

//----------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------

#ifdef PEI_EHCI_MEM_BASE_ADDRESSES
UINTN  mEhciPciBDFs[] = PEI_EHCI_PCI_BDFS;
UINT32 mEhciMemBaseAddresses[] = PEI_EHCI_MEM_BASE_ADDRESSES;
#endif

PCI_UHCI_DEVICE gPeiUhciDev;

PEI_USB_CONTROLLER_PPI GetUhciControllerPpi = {
    GetUhciController
};

// PPI to be installed
static
EFI_PEI_PPI_DESCRIPTOR PpiList[] = {
    {EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
     &gPeiUsbControllerPpiGuid, &GetUhciControllerPpi}
};

//----------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------


/**
    This function is the entry point for this PEI. This function
    initializes the UHCI Controller

    @param FfsHeader   Pointer to the FFS file header
        PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/

EFI_STATUS UhciPeiUsbEntryPoint (
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES    **PeiServices )
{

    UINTN      i;
    EFI_STATUS Status;
    EFI_PEI_PCI_CFG2_PPI *PciCfgPpi;
    UINT8       ClassCode[4];

    // Set PCI Config pointer
    PciCfgPpi = (*PeiServices)->PciCfg;

  #ifdef PEI_EHCI_MEM_BASE_ADDRESSES
    ClearEhciConfigFlagReg( PeiServices, PciCfgPpi );
  #endif

    // Assign resources and enable UHCI controllers
    for (i = 0; i < MAX_USB_CTRLERS; i++) {
        // PEI_UHCI_IOBASE = 0x4000
        gPeiUhciDev.IoBase[i] = PEI_UHCI_IOBASE + 0x40 * i;
        
        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint32, 
            gUhciPciReg[i] + PCI_REG_REVID, ClassCode);
        
        DEBUG((DEBUG_INFO, "USB UHCI #%d PI %x SubClassCode %x BaseCode %x\n", 
            i, ClassCode[1], ClassCode[2], ClassCode[3]));

        if ((ClassCode[1] != PCI_CLASSC_PI_UHCI) || 
            (ClassCode[2] != PCI_CLASSC_SUBCLASS_SERIAL_USB) ||
            (ClassCode[3] != PCI_CLASSC_BASE_CLASS_SERIAL)) {
            continue;   //This is not an uhci controller.
        }
        
        Status = EnableUhciController( PeiServices, PciCfgPpi,
            gPeiUhciDev.IoBase[i], (UINT8) i );
    }

    // Install USB Controller PPI
    return (*PeiServices)->InstallPpi(PeiServices, PpiList);
}


/**
    This function enables the UHCI controller

    @param **PeiServices       Pointer to the PEI services table
        *PciCfgPpi          Pointer to the PCI Configuration PPI
        BaseAddress         I/O base Address to be programmed for the
        UHCI controller
        UsbControllerId     USB Controller ID

    @retval EFI_STATUS

**/

EFI_STATUS EnableUhciController (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfgPpi,
    IN UINTN                BaseAddress,
    IN UINT8                UsbControllerId )
{
    UINT16 CmdReg;


    // Validate USB Controller ID
    if (UsbControllerId >= MAX_USB_CTRLERS) {
        return EFI_INVALID_PARAMETER;
    }

    // Assign base address register
    PciCfgPpi->Write( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint32,
        gUhciPciReg[UsbControllerId] |
        PCI_REG_UHCI_BASE_ADDRESS_REGISTER, &BaseAddress );

    // Enable UHCI PCI Command register
    PciCfgPpi->Read( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
        gUhciPciReg[UsbControllerId] |
        PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );

    CmdReg = (UINT16) (CmdReg | 0x05);

    PciCfgPpi->Write( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
        gUhciPciReg[UsbControllerId] |
        PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );

    return EFI_SUCCESS;
}


/**

    @param **PeiServices       Pointer to the PEI services table
        *PciCfgPpi          Pointer to the PCI Configuration PPI

**/
#ifdef PEI_EHCI_MEM_BASE_ADDRESSES
VOID ClearEhciConfigFlagReg (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfgPpi )
{
    UINT32 i;
    UINT16 CmdReg;
    UINT32  ConfigureFlagAddress;
    

    for (i = 0; i < sizeof(mEhciPciBDFs)/sizeof(UINTN); i++) {
        // Assign Memory Base Address
        PciCfgPpi->Write( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint32,
            mEhciPciBDFs[i] |
            PCI_REG_EHCI_MEMORY_BAR,
            &mEhciMemBaseAddresses[i] );

        // Set the Memory Space Enable and Bus Master Enable bits in the
        // PCI Command register.
        PciCfgPpi->Read( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
            mEhciPciBDFs[i] |
            PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );

        CmdReg = (UINT16) (CmdReg | 0x06);

        PciCfgPpi->Write( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
            mEhciPciBDFs[i] |
            PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );

        //Clear Config Flag Register
        ConfigureFlagAddress=mEhciMemBaseAddresses[i]+(UINT32)MmioRead8(mEhciMemBaseAddresses[i])+EHCI_HC_OPERATIONAL_CONFIG_FLAG_REG;
        MmioWrite32(ConfigureFlagAddress, 0);

        // Clear the Memory Space Enable and Bus Master Enable bits in the
        // PCI Command register.
        PciCfgPpi->Read( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
            mEhciPciBDFs[i] |
            PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );

        CmdReg = (UINT16) ( CmdReg &= ~(0x6) );

        PciCfgPpi->Write( PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
            mEhciPciBDFs[i] |
            PCI_REG_UHCI_COMMAND_REGISTER, &CmdReg );
    }
}


#endif

/**
    This function returns controller type and I/O base address
    value for the controller specified

    @param **PeiServices       Pointer to the PEI services table
        *PciCfgPpi          Pointer to the PCI Configuration PPI
        UsbControllerId     USB Controller ID

    @retval EFI_STATUS
        *ControllerType     Type of the USB controller
        *IoBaseAddress      I/O base Address programmed for this
        controller

**/

EFI_STATUS
GetUhciController(
    IN EFI_PEI_SERVICES           **PeiServices,
    IN PEI_USB_CONTROLLER_PPI     *This,
    IN UINT8                      UsbControllerId,
    OUT UINTN                     *ControllerType,
    OUT UINTN                     *IoBaseAddress
)
{
    UINT8   ClassCode[4];

    if (UsbControllerId >= MAX_USB_CTRLERS) {
        return EFI_INVALID_PARAMETER;
    }

    (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint32, 
            gUhciPciReg[UsbControllerId] + PCI_REG_REVID, ClassCode);
        
    DEBUG((DEBUG_INFO, "USB UHCI #%d PI %x SubClassCode %x BaseCode %x\n",
            UsbControllerId, ClassCode[1], ClassCode[2], ClassCode[3]));

    if ((ClassCode[1] != PCI_CLASSC_PI_UHCI) || 
        (ClassCode[2] != PCI_CLASSC_SUBCLASS_SERIAL_USB) ||
        (ClassCode[3] != PCI_CLASSC_BASE_CLASS_SERIAL)) {
        *ControllerType = 0; //This is not an uhci controller.
        return EFI_SUCCESS;
    }

    *ControllerType = PEI_UHCI_CONTROLLER;
    *IoBaseAddress = gPeiUhciDev.IoBase[UsbControllerId];

    return EFI_SUCCESS;
}

#endif

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
