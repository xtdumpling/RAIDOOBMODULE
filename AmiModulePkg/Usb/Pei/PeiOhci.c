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

/** @file PeiOhci.c
    This file is the main source file for the OHCI PEI USB
    recovery module.  Its entry point at OhciPeiUsbEntryPoint
    will be executed from the UsbRecoveryInitialize INIT_LIST.

**/

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Token.h>
#include <Ppi/Stall.h>
#include "UsbPei.h"
#include "PeiOhci.h"

#if OHCI_PEI_SUPPORT

// List of PCI addresses for OHCI controllers
extern PCI_BUS_DEV_FUNCTION gOhciControllerPciTable[];
extern UINT16   gOhciControllerCount;

// List of OHCI PCI register table
extern OHCI_PCI_PROGRAMMING_TABLE gOhciPciRegisterTable[];
extern UINT16 gOhciPciRegisterTableSize;

extern EFI_STATUS OhciPeiBoardInit (
    IN EFI_PEI_SERVICES **PeiServices,
    EFI_PEI_PCI_CFG2_PPI *PciCfgPpi,
    EFI_PEI_STALL_PPI   *StallPpi );

/**
    This function uses ControllerIndex and the global PCI_BUS_DEV_FUNCTION
    array to access a particular controller's PCI configuration space in 
    order to obtain the Operational Register base address.

        
    @param PeiServices 
        --  PEI Sevices table pointer
    @param ControllerIndex 
        --  Index of the controller in the global
        PCI_BUS_DEV_FUNCTION array

    @retval 
        UINT32 (Return Value)
        = Base address for this controller's operational
        registers.

**/

UINT32 OhciGetOperationalRegisterBase (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN UINT16               ControllerIndex )
{
    UINT32 OhciBaseAddress;

    (*PeiServices)->PciCfg->Read(
        PeiServices,
        (*PeiServices)->PciCfg,
        EfiPeiPciCfgWidthUint32,
        OHCI_PCI_ADDRESS(ControllerIndex, OHCI_BASE_ADDR_REG),
        &OhciBaseAddress
    );


    return OhciBaseAddress &= 0xfffffff0;    // Mask lower bits
}


/**
    This is the entry point into the OHCI controller initialization
    subsystem.

        
    @param FfsHeader 
        --  EFI_FFS_FILE_HEADER pointer
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS OhciPeiUsbEntryPoint (
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES    **PeiServices )
{
    UINT8                Index;
    UINTN                MemPages;
    EFI_STATUS           Status;
    EFI_PHYSICAL_ADDRESS TempPtr;
    PEI_OHCI_DEV         *OhciDevPtr;
    EFI_PEI_STALL_PPI    *StallPpi;
    UINT8                ClassCode[4];

    //-------------------------------------------
    // Initialize the EFI_PEI_STALL_PPI interface
    //-------------------------------------------
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid,
        0, NULL, &StallPpi );
    if ( EFI_ERROR( Status ) ) {
        return EFI_UNSUPPORTED;
    }

    //-----------------------------------------
    // board specific initialization to program
    // PCI bridges and enable MMIO
    //-----------------------------------------

    Status = OhciPeiBoardInit(
        PeiServices,
        (*PeiServices)->PciCfg,
        StallPpi );
    if ( EFI_ERROR( Status ) ) {
        return EFI_UNSUPPORTED;
    }

    //----------------------------------------------------------
    // Allocate OHCI DEVICE OBJECT that holds all necessary
    // information for the Host Controller operational registers
    // for each controller.  Initialze the controller and setup
    // data structures for ready for operation
    //----------------------------------------------------------

    for (Index = 0; Index < gOhciControllerCount; Index++) {

        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint32, 
            OHCI_PCI_ADDRESS(Index, PCI_REG_REVID), ClassCode);
        
        DEBUG((DEBUG_INFO, "USB OHCI#%d PI %x SubClassCode %x BaseCode %x\n", 
            Index, ClassCode[1], ClassCode[2], ClassCode[3]));

        if ((ClassCode[1] != PCI_CLASSC_PI_OHCI) || 
            (ClassCode[2] != PCI_CLASSC_SUBCLASS_SERIAL_USB) ||
            (ClassCode[3] != PCI_CLASSC_BASE_CLASS_SERIAL)) {
            continue;   //This is not an ohci controller.
        }

        // PAGESIZE = 0x1000
        MemPages = sizeof (PEI_OHCI_DEV) / 0x1000 + 1;
        Status = (**PeiServices).AllocatePages(
            PeiServices,
            EfiBootServicesData,
            MemPages,
            &TempPtr
                 );
        if ( EFI_ERROR( Status ) )
        {
            return EFI_OUT_OF_RESOURCES;
        }

        OhciDevPtr = (PEI_OHCI_DEV *) ( (UINTN) TempPtr );
        OhciDevPtr->Signature = PEI_OHCI_DEV_SIGNATURE;
        OhciDevPtr->PeiServices = PeiServices;
        OhciDevPtr->CpuIoPpi = (*PeiServices)->CpuIo;
        OhciDevPtr->StallPpi = StallPpi;
        OhciDevPtr->PciCfgPpi = (*PeiServices)->PciCfg;
        ;
        OhciDevPtr->UsbHostControllerBaseAddress =
            OhciGetOperationalRegisterBase( PeiServices, Index );

        //Initialize the OHCI Controller for operation

        OhciInitHC( PeiServices, OhciDevPtr, Index );  // 0xff03

        //Setup PPI entry point
        OhciDevPtr->UsbHostControllerPpi.ControlTransfer =
            OhciHcControlTransfer;
        OhciDevPtr->UsbHostControllerPpi.BulkTransfer =
            OhciHcBulkTransfer;
        OhciDevPtr->UsbHostControllerPpi.SyncInterruptTransfer =
            OhciHcSyncInterruptTransfer;
        OhciDevPtr->UsbHostControllerPpi.GetRootHubPortNumber =
            OhciHcGetRootHubPortNumber;
        OhciDevPtr->UsbHostControllerPpi.GetRootHubPortStatus =
            OhciHcGetRootHubPortStatus;
        OhciDevPtr->UsbHostControllerPpi.SetRootHubPortFeature =
            OhciHcSetRootHubPortFeature;
        OhciDevPtr->UsbHostControllerPpi.ClearRootHubPortFeature =
            OhciHcClearRootHubPortFeature;
        OhciDevPtr->UsbHostControllerPpi.PreConfigureDevice = NULL;
        OhciDevPtr->UsbHostControllerPpi.EnableEndpoints = NULL;
        OhciDevPtr->UsbHostControllerPpi.ActivatePolling = OhciHcActivatePolling;
        OhciDevPtr->UsbHostControllerPpi.Reset = OhciHcReset;
        OhciDevPtr->UsbHostControllerPpi.CurrentAddress = 0;

        OhciDevPtr->PpiDescriptor.Flags =
            (EFI_PEI_PPI_DESCRIPTOR_PPI |
             EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
        OhciDevPtr->PpiDescriptor.Guid = &gAmiPeiUsbHostControllerPpiGuid;
        OhciDevPtr->PpiDescriptor.Ppi = &OhciDevPtr->UsbHostControllerPpi;
        OhciDevPtr->ControllerIndex = Index;

        //Now is the time to install the PPI
        Status = (**PeiServices).InstallPpi(
            PeiServices, &OhciDevPtr->PpiDescriptor );
        if ( EFI_ERROR( Status ) )
        {
            return EFI_NOT_FOUND;
        }

    }
    
    return EFI_SUCCESS;

}


/**
    This function starts the periodic schedule for the interrupt endpoint-based
    devices (keyboard).

**/

EFI_STATUS
EFIAPI
OhciHcActivatePolling(
    IN EFI_PEI_SERVICES **PeiServices,
    IN VOID *UsbDev
)
{
    PEI_USB_DEVICE  *PeiUsbDev = (PEI_USB_DEVICE*)UsbDev;
    PEI_OHCI_DEV    *OhciDev = PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS(PeiUsbDev->UsbHcPpi);

    EFI_STATUS  Status;
    UINT8       *Ptr;
    UINT16      wData;
    UINT32      dData;

    static OhciPollingStarted = FALSE;

    if (OhciPollingStarted) return EFI_ALREADY_STARTED;
    OhciPollingStarted = TRUE;

    // Allocate ED and TD, making allocation 64-Bytes aligned
    Status = (**PeiServices).AllocatePool( PeiServices,
        sizeof(OHCI_ED)+sizeof(OHCI_TD)+64, &Ptr );

    ASSERT(Status == EFI_SUCCESS);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    // 64-Bytes alignment, assume no deallocation will be necessary,
    // because the original pointer is lost here
    Ptr = (UINT8*)((UINTN)Ptr + 0x40);
    Ptr = (UINT8*)((UINTN)Ptr & ~0x3f);
    OhciDev->stOHCIDescPtrs.pstEDInterrupt = (POHCI_ED)Ptr;
    OhciDev->stOHCIDescPtrs.pstTDInterruptData = (POHCI_TD)(Ptr + sizeof(OHCI_ED));

    dData = (UINT32)PeiUsbDev->IntMaxPkt;
    dData <<= 16;                   // dData[26:16] = device's packet size
    switch (PeiUsbDev->DeviceSpeed) {
        case USB_SLOW_SPEED_DEVICE: wData = 1; break;
        case USB_FULL_SPEED_DEVICE: wData = 2; break;
        default:
            ASSERT(FALSE); // Invalid device speed
    }
    wData = (wData & 1) << 13;      // wData[13] = full/low speed flag
    dData |= (ED_SKIP_TDQ | ED_IN_PACKET |(DEFAULT_PACKET_LENGTH  << 16));
    OhciDev->stOHCIDescPtrs.pstEDInterrupt->dControl = dData | wData;

    // Update the endpoint characteristcs field
    dData = ((UINT32)PeiUsbDev->DeviceAddress)  |
                    ((UINT32)PeiUsbDev->IntEndpoint << 7);

    OhciDev->stOHCIDescPtrs.pstEDInterrupt->dControl |= dData;
    OhciDev->stOHCIDescPtrs.pstEDInterrupt->fpHeadPointer =
        OhciDev->stOHCIDescPtrs.pstTDInterruptData;
    OhciDev->stOHCIDescPtrs.pstEDInterrupt->fpEDLinkPointer = OHCI_TERMINATE;
    OhciDev->stOHCIDescPtrs.pstEDInterrupt->fpTailPointer = OHCI_TERMINATE;

    OhciDev->stOHCIDescPtrs.pstTDInterruptData->dControlStatus =
                (UINT32)(GTD_BUFFER_ROUNDING | GTD_IN_PACKET | GTD_INTD |
                GTD_NO_ERRORS | (GTD_NOT_ACCESSED << 28));
    OhciDev->stOHCIDescPtrs.pstTDInterruptData->dCSReloadValue =
                (UINT32)(GTD_BUFFER_ROUNDING | GTD_IN_PACKET | GTD_INTD |
                GTD_NO_ERRORS | (GTD_NOT_ACCESSED << 28));
    OhciDev->stOHCIDescPtrs.pstTDInterruptData->fpCurrentBufferPointer =
                (UINT8*)(UINTN)OhciDev->stOHCIDescPtrs.pstTDInterruptData->aSetupData;
    OhciDev->stOHCIDescPtrs.pstTDInterruptData->fpBufferEnd =
                (UINT8*)(UINTN)OhciDev->stOHCIDescPtrs.pstTDInterruptData->aSetupData + 7;
    OhciDev->stOHCIDescPtrs.pstTDInterruptData->fpLinkPointer = OHCI_TERMINATE;
    OhciDev->stOHCIDescPtrs.pstTDInterruptData->bActiveFlag = TRUE;

    OhciScheduleED(OhciDev,
        OhciDev->stOHCIDescPtrs.pstEDInterrupt,
        OhciDev->stOHCIDescPtrs.pstED8ms);

    OhciDev->stOHCIDescPtrs.pstEDInterrupt->dControl &= ~ED_SKIP_TDQ;

    return EFI_SUCCESS;
}


/**
    This function adds an ED to the frame list.

    @param 
        pED  - Pointer to the ED to be added
        pPtr - Pointer to the frame list

**/

VOID
OhciAddED(
    POHCI_ED    pED,
    UINTN       *pPtr
)
{
    UINTN CurrentED = *pPtr;
    *pPtr = (UINTN)pED;
    pED->fpEDLinkPointer = (POHCI_ED)(UINTN)CurrentED;
}


/**
    This routine allocates memory for the the Host Controller 
    Communications Area (HCCA), aligned on a 256 byte boundary,
    and updates the HcHcca operational register pointer, as
    well as the corresponding PEI_OHCI_DEV.pFrameList
    pointer.

            
    @param OhciDevPtr 
        --  This is a pointer to a PEI_OHCI_DEV structure
        for an OHCI controller.

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
OhciInitHcca (
    IN PEI_OHCI_DEV *OhciDevPtr
)
{
    OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    POHCI_DESC_PTRS      pstOHCIDescPtrs = &(OhciDevPtr->stOHCIDescPtrs);

    UINT16  Index;
    UINT8   *pPtr;


    // Allocate 256 byte aligned Communication Channel area
    // to the PEI_OHCI_DEV.pFrameList pointer.
    ABORT_ON_ERROR( 
        (*OhciDevPtr->PeiServices)->AllocatePages(
            OhciDevPtr->PeiServices,
            EfiBootServicesData,
            (UINTN) (256 / 0x1000) + 1,
            (EFI_PHYSICAL_ADDRESS *) &OhciDevPtr->pFrameList )
    );
        
    // Zero the memory and update the HcHCCA Host Controller
    // Operational Register.
    (*OhciDevPtr->PeiServices)->SetMem(OhciDevPtr->pFrameList, sizeof(HC_HCCA), 0);

    // The TD schedule should be added in proper order otherwise the
    // links will get garbled. The proper order is to add all the 1ms TDs,
    // followed by 2ms, 8ms and finally 32ms TD.
    //
    for (Index = 0; Index < OhciDevPtr->wAsyncListSize; Index++) {
        pPtr = (UINT8*) &OhciDevPtr->pFrameList[Index];

        pstOHCIDescPtrs->pstED1ms->dControl |= ED_SKIP_TDQ;           // Skip this ED
        OhciAddED(pstOHCIDescPtrs->pstED1ms, (UINTN*)pPtr);          // Schedule 1ms ED
        if (!(Index % 2)) {
            pstOHCIDescPtrs->pstED2ms->dControl |= ED_SKIP_TDQ;
            OhciAddED (pstOHCIDescPtrs->pstED2ms, (UINTN*)pPtr);      // Schedule 2ms ED
        }
        if (!(Index % 8)) {
            pstOHCIDescPtrs->pstED8ms->dControl |= ED_SKIP_TDQ;
            OhciAddED (pstOHCIDescPtrs->pstED8ms, (UINTN*)pPtr);      // Schedule 8ms ED
        }
        if (!(Index % 32)) {
            pstOHCIDescPtrs->pstED32ms->dControl |= ED_SKIP_TDQ;
            OhciAddED (pstOHCIDescPtrs->pstED32ms, (UINTN*)pPtr);    // Schedule 32ms TD
        }
    }

    OhciHcReg->HcHcca = (HC_HCCA*) OhciDevPtr->pFrameList;
    
    return EFI_SUCCESS;
}


/**
    This function adds the ED provided to the particular ED schedule

    @param 
        OhciDev - PEI_OHCI_DEV* structure
        pX - TD/QH to be scheduled (X)
        pY - TD pointer where the new TD/QH (Y) has to be scheduled

    @note  
  This routine adds a new ED (called 'X') to the 'Y' ED. This is accomplished
  by moving the next field in 'Y' (called 'Z') to the next field of 'X' and
  storing address of 'X' into next field of 'Y'.

**/

VOID
OhciScheduleED(
    PEI_OHCI_DEV* OhciDev,
    POHCI_ED    pX,
    POHCI_ED    pY
)
{
    POHCI_ED pED;

    pED = (POHCI_ED)(UINTN)pY->fpEDLinkPointer;   // Get the Z ED pointer from Y ED
    pY->fpEDLinkPointer = (POHCI_ED)(UINTN)pX;      // Store the address of X ED address into next field of Y ED
    pX->fpEDLinkPointer = (POHCI_ED)(UINTN)pED;

}


/**
    OHCI Host Controller initialization.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param OhciDevPtr 
        --  PEI_OHCI_DEV pointer
    @param Index 
        --  Index of this controller in the global 
        PCI_BUS_DEV_FUNCTION array

    @retval 
        VOID (Return Value)

**/

VOID OhciInitHC (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_OHCI_DEV     *OhciDevPtr,
    IN UINT8            Index )
{
    UINT8      bPortNum;
    UINT8      *pPtr;
    UINTN      MemPages;
    EFI_STATUS Status;
    EFI_PHYSICAL_ADDRESS TempPtr;
    EFI_PEI_PCI_CFG2_PPI  *PciCfgPpi = OhciDevPtr->PciCfgPpi;
    POHCI_DESC_PTRS      pstOHCIDescPtrs = &(OhciDevPtr->stOHCIDescPtrs);
    OHCI_HC_REGISTERS    *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;

    OhciDevPtr->wAsyncListSize = OHCI_FRAME_LIST_SIZE;
    OhciDevPtr->dMaxBulkDataSize = MAX_OHCI_BULK_DATA_SIZE;
    //OhciDevPtr->wEndp0MaxPacket = 0x40;
    //OhciDevPtr->bEndpointSpeed = USBSPEED_FULL;


    // Do a Host Controller reset first
    OhciReset( PeiServices, OhciDevPtr );


    //-----------------------------------------------------------------
    // Allocate and initialize the Host Controller Communication
    // area aligned on a 256 byte boundary.
    //
    // This is needed only to poll the HccaDoneHead register
    // in the HCCA, as the periodic list is not implemented.
    //-----------------------------------------------------------------

    // Store number of downstream ports into PEI_OHCI_DEV struct
    OhciDevPtr->bNumPorts =
        OhciHcReg->HcRhDescriptorA.Field.NumberDownstreamPorts;

    // Program root hub characteristics:
    OhciHcReg->HcRhDescriptorA.AllBits = 
        (NO_OVERCURRENT_PROTECTION |    // No over current protection
        POWERED_INDIVIDUALLY |          // Ports powered individually
        POWER_SWITCHED);                // Ports individually power switched
    OhciHcReg->HcRhDescriptorB.AllBits =
        (ALL_REMOVEABLE |               // All devices are removbale
        ALL_PER_PORT_POWER_SWITCHED);   // Power control is per-port
    
    OhciHcReg->HcRhStatus.Field.LocalPowerStatusChange = 
        SET_GLOBAL_POWER;               // Turn on power to all ports
        
    // Initialize the frame interval register
    OhciHcReg->HcFmInterval.Field.FrameInterval = 
        FM_INTERVAL_DEFAULT;            // Between Start of Frames (SOFs)
    OhciHcReg->HcFmInterval.Field.FsLargestDataPacket = 
        FS_LARGEST_PACKET_DEFAULT;      // Largest for single transaction

    // Enable the host controller list processing
    OhciHcReg->HcControl.Field.BulkListEnable = 1;
    OhciHcReg->HcControl.Field.ControlListEnable = 1;
    OhciHcReg->HcControl.Field.PeriodicListEnable = 1;

    // Allocate a block of memory and define/initialize 
    // Setup Control and Bulk EDs/TDs
    pstOHCIDescPtrs = &(OhciDevPtr->stOHCIDescPtrs);

    MemPages = ( 7 *
                sizeof (OHCI_ED) ) + ( 6 * sizeof(OHCI_TD) ) / 0x1000 + 1;
    Status = (**PeiServices).AllocatePages(
        PeiServices,
        EfiBootServicesData,
        MemPages,
        &TempPtr
             );

    pPtr = (UINT8 *) ( (UINTN) TempPtr );
    ZeroMem(pPtr, 7 * sizeof (OHCI_ED) + 6 * sizeof(OHCI_TD));
    if (!pPtr) {
        return;
    }
    pstOHCIDescPtrs->pstED1ms = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstED2ms = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstED8ms = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstED32ms = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstTD32ms = (POHCI_TD) pPtr;
    pPtr += sizeof (OHCI_TD);
    pstOHCIDescPtrs->pstEDControl = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstTDControlSetup = (POHCI_TD) pPtr;
    pPtr += sizeof (OHCI_TD);
    pstOHCIDescPtrs->pstTDControlData = (POHCI_TD) pPtr;
    pPtr += sizeof (OHCI_TD);
    pstOHCIDescPtrs->pstTDControlStatus = (POHCI_TD) pPtr;
    pPtr += sizeof (OHCI_TD);
    pstOHCIDescPtrs->pstEDBulk = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstTDBulkData = (POHCI_TD) pPtr;
    pPtr += sizeof (OHCI_TD);
    pstOHCIDescPtrs->pstEDInterrupt = (POHCI_ED) pPtr;
    pPtr += sizeof (OHCI_ED);
    pstOHCIDescPtrs->pstTDInterruptData = (POHCI_TD) pPtr;

    OhciInitHcca(OhciDevPtr);

    // Initialize EDInterrupt
    pstOHCIDescPtrs->pstEDInterrupt->dControl = ED_SKIP_TDQ;  // Inactive
    OhciScheduleED(OhciDevPtr, pstOHCIDescPtrs->pstEDInterrupt, pstOHCIDescPtrs->pstED1ms);

    // Set the operational bit in the host controller so that power
    // can be applied to the ports.
    OhciHcReg->HcControl.Field.HostControllerFunctionalState = 
        SET_USB_OPERATIONAL;
        
    // Enable all the ports.
    for ( bPortNum = 0; bPortNum < OhciDevPtr->bNumPorts; bPortNum++ ) {
        OhciHcReg->HcRhPortStatus[bPortNum].AllBits = SET_PORT_POWER;
    }
}


/**
    This function performs a software reset of the host controller.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param OhciDevPtr 
        --  PEI_OHCI_DEV pointer

    @retval 
        VOID (Return Value)

    @note  
      It is assumed that all necessary operational register data has been 
      saved prior to calling this function.

**/

VOID OhciReset (
    EFI_PEI_SERVICES **PeiServices,
    PEI_OHCI_DEV     *OhciDevPtr )
{
    OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;

    // Initiate a software reset 
    OhciHcReg->HcCommandStatus.AllBits = SET_CONTROLLER_RESET;

    //Wait for 2ms
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 2);

    //Do USBRESET to reset roothub and downstream port
    OhciHcReg->HcControl.Field.HostControllerFunctionalState = SET_USB_RESET;

    //wait for 10ms
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 10);

    // Note:    HcInterruptStatus.Field.RootHubStatusChange bit should 
    //          now be set if any devices are connected to a port
    //          on this controller. 
    
    return;
}


/**
    This function obtains the port status and port change status for
    a port specified by PortNumber and updates the EFI_USB_PORT_STATUS 
    data structure as specified the the PortStatus pointer parameter.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param PortNumber 
        --  Port number for which status is requested
    @param PortStatus 
        --  EFI_USB_PORT_STATUS pointer's data is updated

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
OhciHcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
)
{
    PEI_OHCI_DEV      *OhciDevPtr =
        PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    HC_RH_PORT_STATUS HcPortStatus;

    HcPortStatus = ( (OHCI_HC_REGISTERS *)
                    OhciDevPtr->UsbHostControllerBaseAddress )->
                       HcRhPortStatus[PortNumber - 1];

    PortStatus->PortStatus = 0;
    PortStatus->PortChangeStatus = 0;

    if (HcPortStatus.Field.CurrentConnectStatus) {
        PortStatus->PortStatus |= USB_PORT_STAT_CONNECTION;
    }
    if (HcPortStatus.Field.PortEnableStatus) {
        PortStatus->PortStatus |= USB_PORT_STAT_ENABLE;
    }
    if (HcPortStatus.Field.PortSuspendStatus) {
        PortStatus->PortStatus |= USB_PORT_STAT_SUSPEND;
    }
    if (HcPortStatus.Field.PortResetStatus) {
        PortStatus->PortStatus |= USB_PORT_STAT_RESET;
    }
    if (HcPortStatus.Field.LowSpeedDeviceAttached) {
        PortStatus->PortStatus |= USB_PORT_STAT_LOW_SPEED;
    }
    if (HcPortStatus.Field.ConnectStatusChange) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_CONNECTION;
    }
    if (HcPortStatus.Field.PortEnableStatusChange) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_ENABLE;
    }
    if (HcPortStatus.Field.PortResetStatusChange) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_RESET;
    }
    if (HcPortStatus.Field.CurrentConnectStatus) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_CONNECTION;
    }

    return EFI_SUCCESS;

}


/**
    This function returns the number of downstream ports as specified 
    in the HcRhDescriptorA operational register.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param PortNumber 
        --  Number of downstream ports

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
OhciHcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
)
{

    PEI_OHCI_DEV        *OhciDevPtr = PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
        
    if (PortNumber == NULL) {
        return EFI_INVALID_PARAMETER;
    }
    
    *PortNumber = OhciHcReg->HcRhDescriptorA.Field.NumberDownstreamPorts;

    return EFI_SUCCESS;
}


/**
    This function sets an OHCI specification port feature as specified by
    PortFeature for the port specified by PortNumber.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param PortNumber 
        --  Port number whose feature is to be set
    @param PortFeature 
        --  Feature to set

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
OhciHcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    PEI_OHCI_DEV        *OhciDevPtr = 
        PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    OHCI_HC_REGISTERS    *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;

    if (PortNumber > OhciHcReg->HcRhDescriptorA.Field.NumberDownstreamPorts){
        return EFI_INVALID_PARAMETER;
    }

    switch (PortFeature)
    {

    case EfiUsbPortSuspend:
        OhciHcReg->HcRhPortStatus[PortNumber - 1].AllBits = SET_PORT_SUSPEND;
        break;

    case EfiUsbPortReset:
        OhciHcReg->HcRhPortStatus[PortNumber - 1].AllBits = SET_PORT_RESET;
        break;

    case EfiUsbPortPower:
        break;

    case EfiUsbPortEnable:
        OhciHcReg->HcRhPortStatus[PortNumber - 1].AllBits = SET_PORT_ENABLE;
        break;

    default:
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}


/**
    This function clears an OHCI specification port feature as specified
    by PortFeature for the port specified by PortNumber.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param PortNumber 
        --  Port number whose feature is to be set
    @param PortFeature 
        --  Feature to set

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
OhciHcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    PEI_OHCI_DEV        *OhciDevPtr = 
        PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    HC_RH_PORT_STATUS   *Pstatus;

    if (PortNumber > OhciHcReg->HcRhDescriptorA.Field.NumberDownstreamPorts){
        return EFI_INVALID_PARAMETER;
    }

    Pstatus = &OhciHcReg->HcRhPortStatus[PortNumber - 1];
    
    switch (PortFeature)
    {
        // clear PORT_ENABLE feature means disable port.
        // This is accomplished by writing a 1 to the CurrentConnectStatus
        // field.
    case EfiUsbPortEnable:
        Pstatus->AllBits = CLEAR_PORT_ENABLE;
        break;

        // clear PORT_SUSPEND feature means resume the port.
        // (cause a resume on the specified port if in suspend mode)
        // This is accomplished by writing a 1 to the PortOverCurrentIndicator
        // field.     
    case EfiUsbPortSuspend:
        Pstatus->AllBits = CLEAR_SUSPEND_STATUS;
        break;

    case EfiUsbPortPower:
        break;


        // clear PORT_RESET means clear the reset signal.
        // This is accomplished by writing a 1 to the PortResetStatusChange
        // field.  No assumption is made that the appropriate delay
        // was implemented.
    case EfiUsbPortReset:

        break;

        // clear connect status change by writing
        // a 1 to the ConnectStatusChange field
    case EfiUsbPortConnectChange:
        Pstatus->AllBits = CLEAR_CONNECT_STATUS_CHANGE;
        break;

        // clear enable/disable status change by writing
        // a 1 to the PortEnableStatusChange field
    case EfiUsbPortEnableChange:
        Pstatus->AllBits = CLEAR_PORT_ENABLE_STATUS_CHANGE;
        break;

        // root hub does not support these requests
    case EfiUsbPortSuspendChange:
    case EfiUsbPortOverCurrentChange:
    case EfiUsbPortResetChange:
        Pstatus->AllBits = CLEAR_PORT_RESET_STATUS_CHANGE;
        break;

    default:
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}


/**
    This routine resets the Host Controler 

    @param EFI_PEI_SERVICES             **PeiServices,
        PEI_USB_HOST_CONTROLLER_PPI  *This,
        UINT16                       Attributes         

    @retval 
        EFI_SUCCESS     Successfully reset controller.

**/

EFI_STATUS
EFIAPI
OhciHcReset(
     IN EFI_PEI_SERVICES             **PeiServices,
     IN PEI_USB_HOST_CONTROLLER_PPI  *This,
     IN UINT16                       Attributes
)
{
	UINT8       			   PortIndex = 0;
    PEI_OHCI_DEV    		   *OhciDev = 
								   PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS (This);
  	volatile OHCI_HC_REGISTERS *OhciHcReg;
    UINT16                     RegisterIndex;
    UINT64                     Val64;
    EFI_PEI_PCI_CFG2_PPI       *Pci = (*PeiServices)->PciCfg;

    switch (Attributes) {
        case EFI_USB_HC_RESET_GLOBAL:

			OhciHcReg = 
				(OHCI_HC_REGISTERS*)OhciDev->UsbHostControllerBaseAddress;

			// Stop periodic schedule
			OhciHcReg->HcControl.Field.PeriodicListEnable = 0;				

			// Reset host controller
			OhciReset (PeiServices, OhciDev);
			
			// Clear PCI resource
			Val64 = 0;

			// Clear BAR
			Pci->Write( PeiServices, Pci,
				EfiPeiPciCfgWidthUint32,
				OHCI_PCI_ADDRESS(OhciDev->ControllerIndex, OHCI_BASE_ADDR_REG),
				&Val64);

			// Clear all other registers using gOhciPciRegisterTable[]
			for (RegisterIndex = 0;
				RegisterIndex < gOhciPciRegisterTableSize;
				RegisterIndex++) {
				EFI_PEI_PCI_CFG_PPI_WIDTH Width;
			
				switch (gOhciPciRegisterTable[RegisterIndex].Size) {
					case 8:	
						Width = EfiPeiPciCfgWidthUint8; 
						break;
					case 16:	
						Width = EfiPeiPciCfgWidthUint16; 
						break;
					case 32:	
						Width = EfiPeiPciCfgWidthUint32; 
						break;
					case 64:	
						Width = EfiPeiPciCfgWidthUint64; 
						break;
					default:
						continue;
				}

				Pci->Write(PeiServices, Pci,
					Width,
					OHCI_PCI_ADDRESS(OhciDev->ControllerIndex, gOhciPciRegisterTable[RegisterIndex].Register),
					&Val64);

			}

    		break;                    
		default:
			return EFI_UNSUPPORTED;
    }
    
    return EFI_SUCCESS;  
}

/**
    This function enables a root hub port as specified by PortNumber.

        
    @param OhciDevPtr 
        --  PEI_OHCI_DEV pointer
    @param PortNumber 
        --  Root hub port number to be enabled

    @retval 
        VOID (Return Value)

**/

VOID OhciHcEnableRootHub (
    IN PEI_OHCI_DEV *OhciDevPtr,
    IN UINT8        PortNumber )
{
    OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    volatile HC_RH_PORT_STATUS *Pstatus = &OhciHcReg->HcRhPortStatus[PortNumber - 1];
    
    // Enable the port
    Pstatus->AllBits = SET_PORT_ENABLE;

    // Wait for port enable to stabilize (100ms delay)
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 100 );

    // Reset the port
    Pstatus->AllBits = SET_PORT_RESET;
    
    // Wait for reset to complete by polling the PortResetStatus field
    // as it is cleared at the end of the reset sequence
    while (Pstatus->Field.PortResetStatus == RH_RESET_ACTIVE )
        ;

    // Wait an additional delay
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 500 ); 

    // Clear the PortResetStatus and PortResetStatusChange fields
    // by writing a 1 to the PortResetStatusChange field
    Pstatus->AllBits = CLEAR_PORT_RESET_STATUS_CHANGE;

    // 1/2 second Wait for devices connected to the port to stabilize
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 500 );

}


/**
    This function intiates a USB control transfer and waits on it to 
    complete.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param bDeviceAddress 
        --  USB address of the device for which the control 
        transfer is to be issued
    @param DeviceType 
        --  Not used
    @param MaximumPacketLength 
        --  Maximum number of bytes that can be sent to or 
        received from the endpoint in a single data packet
    @param Request 
        --  EFI_USB_DEVICE_REQUEST pointer
    @param TransferDirection 
        --  Direction of transfer
    @param DataBuffer 
        --  Pointer to source or destination buffer
    @param DataLength 
        --  Length of buffer
    @param TimeOut 
        --  Not used
    @param TransferResult 
        --  Not used

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

typedef struct _D_REQUEST
{
    UINT8  RequestType;
    UINT8  Request;
    UINT16 Value;
    UINT16 Index;
    UINT16 Length;
} D_REQUEST;

EFI_STATUS
EFIAPI
OhciHcControlTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       bDeviceAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT8                       MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN EFI_USB_DEVICE_REQUEST      *Request,
    IN EFI_USB_DATA_DIRECTION      TransferDirection,
    IN OUT VOID *DataBuffer        OPTIONAL,
    IN OUT UINTN *DataLength       OPTIONAL,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
)
{
    UINT16            *fpData;
    POHCI_ED          fpED;
    POHCI_TD          fpTD;
    UINT32            DwordData;
    UINT16            WordData;
    UINT16            WordRequest;
    UINT16            WordIndex;
    UINT16            WordValue;
    EFI_STATUS        Status = EFI_DEVICE_ERROR;
    PEI_OHCI_DEV      *OhciDevPtr =
        PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    POHCI_DESC_PTRS   fpDescPtrs =
        &(OhciDevPtr->stOHCIDescPtrs);
    OHCI_HC_REGISTERS *HcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    UINT8               CompletionCode;

    // 5 ms delay is necessary for OHCI host controllers
    OHCI_FIXED_DELAY_MS( OhciDevPtr, 5 );

    WordRequest = (Request->Request << 8) | Request->RequestType;
    WordValue = Request->Value;
    WordIndex = Request->Index;

    //
    // Build the device request in the data area of the control setup qTD
    //
    fpData = (UINT16 *) fpDescPtrs->pstTDControlSetup->aSetupData;
    *fpData++ = WordRequest;
    *fpData++ = WordValue;
    *fpData++ = WordIndex;
    *fpData++ = (UINT16) *DataLength;
    *(UINT32 *) fpData = (UINT32) DataBuffer;

    //
    // Prepare some registers that will be used in building the TDs below.
    // wLength  contains the data length.
    // fpBuffer contains the absolute address of the data buffer.
    // WordRequest contains the request type (bit 7 = 0/1 for Out/In).
    // fpDevInfo will contain a pointer to the DeviceInfo structure for
    // the given device.
    //
    // Ready the EDControl for the control transfer.
    //
    fpED = fpDescPtrs->pstEDControl;
    //
    // The ED control field will be set so
    //   Function address & Endpoint number = ESI,
    //   Direction = From TD,
    //   Speed = DeviceInfo.bEndpointSpeed,
    //   Skip = 1, Format = 0,
    //   Max packet size  = DeviceInfo.wEndp0MaxPacket
    // The HeadPointer field will be set to TDControlSetup
    // The TailPointer field will be set to OHCI_TERMINATE
    // The LinkPointer field will be set to OHCI_TERMINATE
    //

    // if wEndp0MaxPacket is NULL, then use default
    // packet size
    DwordData = MaximumPacketLength;

    // Force the max packet size to 64 bytes
    if (DwordData > 0x40 || DwordData == 0) {
        DwordData = 0x40;
    }
    DwordData <<= 16;                                       // DwordData[26:16] = device's packet size
    WordData = (UINT16) DeviceSpeed;       					// 00/01/10 for HI/LO/FULL
    WordData = (WordData & 1) << 13;                          // WordData[13] = full/low speed flag
    WordData |= bDeviceAddress | ED_SKIP_TDQ;
    fpED->dControl = DwordData | WordData;
    fpED->fpTailPointer = 0;
    fpED->fpEDLinkPointer = 0;

    fpTD = fpDescPtrs->pstTDControlSetup;
    //
    // The ControlStatus field will be set so
    //   Buffer Rounding = 1,
    //   Direction PID = GTD_SETUP_PACKET,
    //   Delay Interrupt = GTD_INTD,
    //   Data Toggle = GTD_SETUP_TOGGLE,
    //   Error Count = GTD_NO_ERRORS,
    //   Condition Code = GTD_NOT_ACCESSED
    // The CurrentBufferPointer field will point to the TD's SetupData buffer
    //   which was before initialized to contain a DeviceRequest struc.
    // The BufferEnd field will point to the last byte of the TD's SetupData
    //   buffer.
    // The LinkPointer field will point to the TDControlData if data will
    //   be sent/received or to the TDControlStatus if no data is expected.
    // The CSReloadValue field will contain 0 because this is a "one shot"
    //   packet.
    // The pCallback will be set to point to the OHCI_ControlTDCallback
    //   routine.
    // The ActiveFlag field will be set to TRUE.
    // The DeviceAddress field does not need to be set since the Control TDs
    //  do not need rebinding to the EDControl.
    //
    fpTD->dControlStatus = (UINT32) ( GTD_BUFFER_ROUNDING |
                                     GTD_SETUP_PACKET | GTD_SETUP_TOGGLE |
                                     GTD_NO_ERRORS | (GTD_NOT_ACCESSED << 28) );

    fpTD->fpCurrentBufferPointer = fpTD->aSetupData;
    fpTD->fpBufferEnd = fpTD->aSetupData + 7; // size of aSetupData - 1

    if (*DataLength) {  // some data to transfer

        // Fill in various fields in the TDControlData.
        fpTD = fpDescPtrs->pstTDControlData;
        //
        // The ControlStatus field will be set so
        //   Buffer Rounding = 1,
        //   Direction PID = GTD_OUT_PACKET/GTD_IN_PACKET,
        //   Delay Interrupt = GTD_INTD,
        //   Data Toggle = GTD_DATA1_TOGGLE,
        //   Error Count = GTD_NO_ERRORS,
        //   Condition Code = GTD_NOT_ACCESSED
        // The CurrentBufferPointer field will point to the caller's buffer
        //   which is now in EBP.
        // The BufferEnd field will point to the last byte of the caller's
        //   buffer.
        // The LinkPointer field will point to the TDControlStatus.
        // The CSReloadValue field will contain 0 because this is a
        //   "one shot" packet.
        // The pCallback will be set to point to the OHCI_ControlTDCallback
        //   routine.
        // The ActiveFlag field will be set to TRUE.
        // The DeviceAddress field does not need to be set since the Control
        //   TDs do not need rebinding to the EDControl.
        // The CSReloadValue field will contain 0 because this is a
        //   "one shot" packet.
        // The pCallback will be set to point to the OHCI_ControlTDCallback
        //   routine.
        // The ActiveFlag field will be set to TRUE.    return  USB_SUCCESS;
        // The DeviceAddress field does not need to be set since the Control
        //   TDs do not need rebinding to the EDControl.
        //
        DwordData = (UINT32) ( GTD_BUFFER_ROUNDING | GTD_DATA1_TOGGLE
                          | GTD_NO_ERRORS | (GTD_NOT_ACCESSED << 28) );
        DwordData = (WordRequest & BIT7)
                ? (DwordData | GTD_IN_PACKET | GTD_INTD)
                : (DwordData | GTD_OUT_PACKET);
        fpTD->dControlStatus = DwordData;
        fpTD->fpCurrentBufferPointer = (UINT8 *) DataBuffer;

        WordData = (*DataLength < MAX_CONTROL_DATA_SIZE)
                ? *DataLength
                : MAX_CONTROL_DATA_SIZE;
        fpTD->fpBufferEnd = (UINT8 *) DataBuffer + WordData - 1;
    }

    // Fill in various fields in the TDControlStatus.
    fpTD = fpDescPtrs->pstTDControlStatus;
    //
    // The ControlStaus field will be set so
    //   Buffer Rounding = 1,
    //   Direction PID = GTD_OUT_PACKET/GTD_IN_PACKET,
    //   Delay Interrupt = GTD_INTD,
    //   Data Toggle = GTD_DATA1_TOGGLE,
    //   Error Count = GTD_NO_ERRORS,
    //   Condition Code = GTD_NOT_ACCESSED
    // The CurrentBufferPointer field will point to NULL
    // The BufferEnd field will point to NULL.
    // The LinkPointer field will point to OHCI_TERMINATE.
    // The CSReloadValue field will contain 0 because this is a
    //   "one shot" packet.
    // The pCallback will be set to point to the OHCI_ControlTdCallback
    //   routine.
    // The ActiveFlag field will be set to TRUE.
    // The DeviceAddress field does not need to be set since the Control
    //   TDs do not need rebinding to the EdControl.
    //
    // Note: For OUT control transfer status should be IN and
    //       for IN cotrol transfer, status should be OUT.
    //
    DwordData = (UINT32) ( GTD_BUFFER_ROUNDING | GTD_DATA1_TOGGLE
                      | GTD_NO_ERRORS | (GTD_NOT_ACCESSED << 28) );
    DwordData = (WordRequest & BIT7)
            ? (DwordData | GTD_OUT_PACKET)
            : (DwordData | GTD_IN_PACKET | GTD_INTD);
    fpTD->dControlStatus = DwordData;
    fpTD->fpCurrentBufferPointer = NULL;
    fpTD->fpBufferEnd = NULL;
    fpTD->fpLinkPointer = NULL;

    //
    // Link all the pointers together
    //
    fpTD = fpDescPtrs->pstTDControlSetup;
    fpED->fpHeadPointer = fpTD;
    if (*DataLength) {  // chain in data TD
        fpTD->fpLinkPointer = (UINT8 *) fpDescPtrs->pstTDControlData;
        fpTD = fpDescPtrs->pstTDControlData;
    }
    fpTD->fpLinkPointer = (UINT8 *) fpDescPtrs->pstTDControlStatus;

    fpDescPtrs->pstTDControlStatus->fpLinkPointer = NULL;

    fpTD = fpDescPtrs->pstTDControlSetup;
    do {
        fpTD->dCSReloadValue = 0;
        fpTD->bActiveFlag = TRUE;
        fpTD = (POHCI_TD) fpTD->fpLinkPointer;
    } while (fpTD);

    // clear the WriteBackDoneHead status bit in
    // the HcInterruptStatus register
    HcReg->HcInterruptStatus.Field.WriteBackDoneHead = HCINT_STATUS_CLEAR;

    //
    // Now control queue is complete, so set ED_SKIP_TDQ=0
    //
    fpED->dControl &= ~ED_SKIP_TDQ;
    //
    // Set the HcControlHeadED register to point to the EDControl.
    //
    HcReg->HcControlHeadEd = (UINTN) fpED;

    //
    // Now put the control setup, data and status into the HC's schedule by
    // setting the ControllListFilled field of HcCommandStatus reg.
    // This will cause the HC to execute the transaction in the next
    // active frame.
    //
    HcReg->HcCommandStatus.AllBits = SET_CONTROL_LIST_FILLED;

    //
    // Now wait for the control status TD to complete.  When it has completed,
    // the OHCI_ControlTDCallback will set its active flag to FALSE.
    Status = OhciHcWaitForTransferComplete( PeiServices, OhciDevPtr,
        fpDescPtrs->pstTDControlStatus );

    //
    // Stop the HC from processing the EDControl by setting its Skip bit.
    //
    fpED->dControl |= ED_SKIP_TDQ;

    //
    // Finally check for any error bits set in both the TDControlStatus.
    // If the TD did not complete successfully, return STC.
    //

    // DwordData[3:0] = Completion status
    CompletionCode = (UINT8)(fpDescPtrs->pstTDControlStatus->dControlStatus >> 28);

	switch (CompletionCode) {
        case GTD_NO_ERROR:
            *TransferResult = EFI_USB_NOERROR;
            Status = EFI_SUCCESS;
			break;
		case GTD_STALL:
            *TransferResult = EFI_USB_ERR_STALL;
			break;
        case GTD_NOT_ACCESSED:
            *TransferResult = EFI_USB_ERR_TIMEOUT;
            Status = EFI_TIMEOUT;
            break;
		default:
			break;
	}

    return Status;

}

/**
    This function intiates a USB bulk transfer and waits on it to 
    complete.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param DeviceAddress 
        --  USB address of the device for which the control 
        transfer is to be issued
    @param EndPointAddress 
        --  Particular endpoint for the device
    @param MaximumPacketLength 
        --  Maximum number of bytes that can be sent to or 
        received from the endpoint in a single data packet
    @param DataBuffer 
        --  Pointer to source or destination buffer
    @param DataLength 
        --  Length of buffer
    @param DataToggle 
        --  Used to update the control/status DataToggle field
        of the Transfer Descriptor
    @param TimeOut 
        --  Not used
    @param TransferResult 
        --  Not used

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
OhciHcBulkTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       EndPointAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT16                      MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN OUT VOID                    *DataBuffer,
    IN OUT UINTN                   *DataLength,
    IN OUT UINT8                   *DataToggle,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
)
{
    UINT32          DwordData;
    UINT16          wMaxPkt;
    EFI_STATUS      Status = EFI_DEVICE_ERROR;
    UINT8           bEndp;
    UINT8           Toggle;
    PEI_OHCI_DEV        *OhciDevPtr = 
        PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( This );
    POHCI_DESC_PTRS     fpDescPtrs = &(OhciDevPtr->stOHCIDescPtrs);
    OHCI_HC_REGISTERS   *HcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    UINT8               CompletionCode;

    wMaxPkt = MaximumPacketLength;

    if (wMaxPkt == 0) {
        *DataLength = 0;
        return EFI_NOT_FOUND;
    }

    //Clear Bulk ED and TD
    ZeroMem(fpDescPtrs->pstEDBulk, sizeof(OHCI_ED));
    ZeroMem(fpDescPtrs->pstTDBulkData, sizeof(OHCI_TD));
    //
    //  Set the SKIP bit in the EdBulk to avoid accidental scheduling
    //
    fpDescPtrs->pstEDBulk->dControl = ED_SKIP_TDQ;
    //
    // Set the ED's head pointer field to bulk data TD and tail pointer
    // field to OHCI_TERMINATE. Also set ED's link pointer to
    // OHCI_TERMINATE.
    //
    fpDescPtrs->pstEDBulk->fpHeadPointer = fpDescPtrs->pstTDBulkData;
    fpDescPtrs->pstEDBulk->fpTailPointer = OHCI_TERMINATE;
    fpDescPtrs->pstEDBulk->fpEDLinkPointer = OHCI_TERMINATE;
    //
    // Form the data needed for ED's control field with the available
    // information
    //
    bEndp = EndPointAddress & 0x7f;
    DwordData = (EndPointAddress & 0x80) ? ED_IN_PACKET : ED_OUT_PACKET;
    DwordData |= DeviceAddress;
    DwordData |= (UINT16) bEndp << 7;
    DwordData |= (UINT32) wMaxPkt << 16;

    //
    // Update the ED's control field with the data formed
    // ASSUME ALL MASS DEVICES ARE FULL SPEED DEVICES.
    //
    fpDescPtrs->pstEDBulk->dControl = DwordData;
    //
    // Fill the general bulk data TD with relevant information.  Set the
    // TD's control field with buffer rounding set to 1, direction PID to
    // don't care, delay interrupt to INTD, data toggle to the latest data
    // toggle value, error count to no errors and condition code to not
    // accessed.
    //
    // Set the data toggle to DATA0 (SETUP_TOGGLE)
    fpDescPtrs->pstTDBulkData->dControlStatus = (UINT32) (
        GTD_BUFFER_ROUNDING |
        GTD_IN_PACKET |
        GTD_INTD |
        GTD_SETUP_TOGGLE |
        GTD_NO_ERRORS |
        (GTD_NOT_ACCESSED << 28)
                                                );
    fpDescPtrs->pstTDBulkData->dControlStatus |= (UINT32) *DataToggle << 24;

    //
    // GTD current buffer pointer field will point to the caller's buffer which
    // now in the variable fpBuffer
    //
    fpDescPtrs->pstTDBulkData->fpCurrentBufferPointer = (UINT8 *) DataBuffer;
    fpDescPtrs->pstTDBulkData->fpBufferEnd = (UINT8 *) DataBuffer +
                                             *DataLength - 1;
    fpDescPtrs->pstTDBulkData->fpLinkPointer = OHCI_TERMINATE;
    //
    // GTD's CSReloadValue field will contain 0 because this is a
    // "one shot" packet
    //
    fpDescPtrs->pstTDBulkData->dCSReloadValue = 0;
    fpDescPtrs->pstTDBulkData->bActiveFlag = TRUE;

    // Set the HCBulkHeadED register to point to the bulk ED
    fpDescPtrs->pstEDBulk->dControl &= ~ED_SKIP_TDQ;
    HcReg->HcBulkHeadEd = (UINT32) fpDescPtrs->pstEDBulk;
    
    // Enable the bulk list processing
    HcReg->HcCommandStatus.AllBits = SET_BULK_LIST_FILLED;

    Status = OhciHcWaitForTransferComplete( PeiServices, OhciDevPtr,
        fpDescPtrs->pstTDBulkData );
    //
    // Stop the HC from processing the EDBulk by setting its Skip bit.
    //
    fpDescPtrs->pstEDBulk->dControl |= ED_SKIP_TDQ;
    //
    // Update the data toggle value into the mass info structure
    //

    *DataToggle = (UINT8) ( ( (fpDescPtrs->pstTDBulkData->dControlStatus &
                               GTD_DATA_TOGGLE) >> 24 ) & 1 );
    Toggle = (UINT8) ( (fpDescPtrs->pstTDBulkData->dControlStatus &
                        GTD_DATA_TOGGLE) >> 24 );
    if ( (Toggle & 0x02) == 0 )
    {
        //Use the Carry
        DwordData = (UINT32) fpDescPtrs->pstEDBulk->fpHeadPointer;
        DwordData &= 0x00000002;
        DwordData = DwordData >> 1;
        DwordData &= 0x00000001;
        *DataToggle = DwordData;
    }
    //
    // Check for the error conditions - if possible recover from them
    //
    CompletionCode = (UINT8)(fpDescPtrs->pstTDBulkData->dControlStatus >> 28);
    
	switch (CompletionCode) {
        case GTD_NO_ERROR:
            *TransferResult = EFI_USB_NOERROR;
            Status = EFI_SUCCESS;
			break;
		case GTD_STALL:
            *TransferResult = EFI_USB_ERR_STALL;
            *DataLength = 0;
			break;
        case GTD_NOT_ACCESSED:
            *TransferResult = EFI_USB_ERR_TIMEOUT;
            *DataLength = 0;
            Status = EFI_TIMEOUT;
            break;
		default:
            *DataLength = 0;
			break;
	}

    if (Status != EFI_SUCCESS) {
        return Status;
    }
    
    //
    // Get the size of data transferred
    //
    // Size transferred is calculated by subtracting end address with current
    // buffer pointer and subtracting that value from the total size
    //
    if (!fpDescPtrs->pstTDBulkData->fpCurrentBufferPointer) {
        //All data are transfered.
        return EFI_SUCCESS;
    }
    DwordData = (UINT32)fpDescPtrs->pstTDBulkData->fpBufferEnd;
    DwordData = DwordData -
            (UINT32)fpDescPtrs->pstTDBulkData->fpCurrentBufferPointer;
    
    if (DwordData != 0) {
        *DataLength = *DataLength - DwordData - 1;
    }
    
    return Status;

}

/**
    This function processes a USB interrupt transfer within a given timeout.

    @retval 
        EFI_INVALID_PARAMETER   Invalid data, datalength, datatoggle
        EFI_NOT_FOUND   No endpoint found that matches the given parameters.
        EFI_TIMEOUT     No transfer during the given timeout.
        EFI_SUCCESS     Successful transfer, data is copied to the given data buffer.

**/

EFI_STATUS
EFIAPI
OhciHcSyncInterruptTransfer(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI  *This,
    IN     UINT8                    DeviceAddress,
    IN     UINT8                    EndPointAddress,
    IN     UINT8                    DeviceSpeed,
    IN     UINT8                    MaximumPacketLength,
    IN UINT16                       TransactionTranslator,
    IN OUT VOID                     *Data,
    IN OUT UINTN                    *DataLength,
    IN OUT UINT8                    *DataToggle,
    IN     UINTN                    TimeOut,
    OUT    UINT32                   *TransferResult
)
{
    PEI_OHCI_DEV    *OhciDev = PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS(This);
    volatile OHCI_HC_REGISTERS   *OhciHcReg =
        (OHCI_HC_REGISTERS *) OhciDev->UsbHostControllerBaseAddress;
    POHCI_DESC_PTRS pDescPtrs = &OhciDev->stOHCIDescPtrs;

    UINTN           Count;
    UINT8           *Src;
    UINT8           *Dst;

    if (Data == NULL || DataLength == NULL) {
        ASSERT(FALSE);
        return EFI_INVALID_PARAMETER;
    }

    for (Count = 0; Count < TimeOut; Count++) {
        // Check if the memory dword at HCCADONEHEAD has been updated to contain the head
        // pointer of the linked list of TDs that have completed.
        if (OhciHcReg->HcHcca->HccaDoneHead != 0) {
            break;
        }

        OHCI_FIXED_DELAY_MS(OhciDev, 1);  // 1 msec
    }

    // Clear the link pointer. It may point to some other TD
    pDescPtrs->pstTDInterruptData->fpLinkPointer = OHCI_TERMINATE;
    pDescPtrs->pstTDInterruptData->dControlStatus = pDescPtrs->pstTDInterruptData->dCSReloadValue;
    pDescPtrs->pstTDInterruptData->fpCurrentBufferPointer = pDescPtrs->pstTDInterruptData->aSetupData;

    pDescPtrs->pstEDInterrupt->fpHeadPointer = (POHCI_TD)((UINTN)pDescPtrs->pstEDInterrupt->fpHeadPointer & ED_TOGGLE_CARRY);
    pDescPtrs->pstEDInterrupt->fpHeadPointer = (POHCI_TD)((UINTN)pDescPtrs->pstEDInterrupt->fpHeadPointer | (UINTN)pDescPtrs->pstTDInterruptData);
    pDescPtrs->pstTDInterruptData->bActiveFlag = TRUE;

    if (Count == TimeOut) return EFI_TIMEOUT;

    // Get the data and return success
    for (Src = pDescPtrs->pstTDInterruptData->aSetupData,
         Dst = (UINT8*)Data,
         Count = 0; Count < 8; Count++)
    {
        *Dst++ = *Src++;
    }

    return EFI_SUCCESS;
}


/**
    This function waits for a tranfer to complete by polling the 
    HcInterruptStatus register's WritebackDoneHead field.
      
        
    @param OhciDevPtr 
        --  PEI_OHCI_DEV pointer
    @param Td 
        --  Not used

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS OhciHcWaitForTransferComplete (
	IN EFI_PEI_SERVICES	**PeiServices,
    IN PEI_OHCI_DEV *OhciDevPtr,
    IN POHCI_TD     Td )
{
    UINT32              Count;
    OHCI_HC_REGISTERS   *HcReg =
        (OHCI_HC_REGISTERS *) OhciDevPtr->UsbHostControllerBaseAddress;
    HC_INTERRUPT_STATUS *HcIntStatus = &HcReg->HcInterruptStatus;
	UINT32				DoneHead = 0;

    // Poll (up to 4 seconds) or until the
    // WriteBackDoneHead status bit is set in
    // the HcInterruptStatus operational register.

    // if the WriteBackDoneHead is set, then clear 
    // HccaDoneHead in the HCCA and WriteBackDoneHead
    // and then return EFI_SUCCESS to continue execution.

    for (Count = 0; Count < 1000; Count++) {
        if (HcIntStatus->Field.WriteBackDoneHead == HCINT_WB_DONE ) {
			DoneHead = HcReg->HcHcca->HccaDoneHead;
			HcReg->HcHcca->HccaDoneHead = 0;
            HcIntStatus->Field.WriteBackDoneHead = HCINT_STATUS_CLEAR;
			if (DoneHead == (UINT32)Td) {
            	return EFI_SUCCESS;
			}
        }
        OHCI_FIXED_DELAY_MS( OhciDevPtr, 4 ); // 4 ms

    }
	
	DEBUG((DEBUG_ERROR, "OHCI Time-Out:\n"));
    return EFI_NOT_FOUND;

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
