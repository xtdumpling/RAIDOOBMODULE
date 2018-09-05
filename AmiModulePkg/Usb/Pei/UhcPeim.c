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

/** @file UhcPeim.c
    This file contains generic routines needed for USB recovery PEIM

**/


#include "UsbPei.h"
#include "UhcPeim.h"
#include <Library/DebugLib.h>
#include <Ppi/UsbController.h>

#define PAGESIZE  4096
#include "Ppi/LoadFile.h"

#if UHCI_PEI_SUPPORT

//Uhci PCI Device
extern UINTN gUhciPciReg[];

// PCI_REG
#define PCI_REG_UHCI_COMMAND_REGISTER         0x04
#define PCI_REG_UHCI_BASE_ADDRESS_REGISTER    0x20

UINT32
USBReadPortDW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port );

//extern VOID ZeroMem (
//    IN VOID  *Buffer,
//    IN UINTN Size );


EFI_STATUS UhcPeimEntry (
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES    **PeiServices )
{
    EFI_PEI_CPU_IO_PPI *CpuIoPpi;
    EFI_PEI_STALL_PPI  *StallPpi;
    PEI_USB_CONTROLLER_PPI *ChipSetUsbControllerPpi;
    EFI_STATUS     Status;
    UINT8       Index;
    UINTN       ControllerType;
    UINTN       BaseAddress;
    USB_UHC_DEV *UhcDev;

    // Locate the PEI_CPU_IO_PPI interface
    if ( EFI_ERROR( (**PeiServices).LocatePpi(
                PeiServices, &gEfiPeiCpuIoPpiInstalledGuid, 0, NULL,
                &CpuIoPpi ) ) ) {
        return EFI_UNSUPPORTED;
    }

    // Locate the PEI_STALL_PPI interface
    if ( EFI_ERROR( (**PeiServices).LocatePpi(
                PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &StallPpi ) ) ) {
        return EFI_UNSUPPORTED;
    }

    // Locate the EFI_PEI_USB_CONTROLLER_PPI interface
    if ( EFI_ERROR( (**PeiServices).LocatePpi(
                PeiServices, &gPeiUsbControllerPpiGuid, 0, NULL,
                &ChipSetUsbControllerPpi ) ) ) {
        return EFI_UNSUPPORTED;
    }

    //--------------------------------------------------
    // For each UHCI controller found:
    //
    // 1) Verify it is a UHCI controller
    // 2) Allocate and initialize a USB_UHC_DEV structure
    //    to manage the controller.
    // 3) Initialize the controller.
    // 4) Install the PEI_USB_HOST_CONTROLLER_PPI interface.
    //
    //--------------------------------------------------
    Index = 0;
    while (TRUE) {

        // GetUhciControllerPpi() returns error if no more controllers
        if ( EFI_ERROR(ChipSetUsbControllerPpi->GetUsbController(
                    PeiServices, ChipSetUsbControllerPpi, Index,
                    &ControllerType,
                    &BaseAddress ) ) ) {
            break;
        }

        // This PEIM is for UHC type controller.
        if (ControllerType != PEI_UHCI_CONTROLLER) {
            Index++;
            continue;
        }

        // Allocate and initialize USB_UHC_DEV members
        if ( EFI_ERROR( (**PeiServices).AllocatePages(
                    PeiServices, EfiBootServicesData,
                    sizeof(USB_UHC_DEV) / PAGESIZE + 1,
                    (EFI_PHYSICAL_ADDRESS *) &UhcDev ) ) ) {
            return EFI_OUT_OF_RESOURCES;
        }
        UhcDev->Signature = USB_UHC_DEV_SIGNATURE;
        UhcDev->PeiServices = PeiServices;
        UhcDev->CpuIoPpi = (*PeiServices)->CpuIo;
        UhcDev->StallPpi = StallPpi;
        UhcDev->UsbHostControllerBaseAddress = (UINT32) BaseAddress;
        UhcDev->ControllerIndex = Index;

        // Initialize Uhc's hardware
        Status = InitializeUsbHC(UhcDev);
        if (EFI_ERROR(Status)) {
            DEBUG((DEBUG_ERROR,
                        "UHCPEIM: BaseAddress %x\n", BaseAddress) );
            DEBUG((DEBUG_ERROR,
                        "UHCPEIM: InitializeUSbHC Status %r\n", Status) );
            // error in initilizing the controller , so try for next controller 
            Index++;
            continue;
        }

        // Init local memory management service
        Status = InitializeMemoryManagement( UhcDev );
        if ( EFI_ERROR( Status ) ) {
            return Status;
        }

        // Update PEI_USB_HOST_CONTROLLER_PPI members
        UhcDev->UsbHostControllerPpi.ControlTransfer = UhcControlTransfer;
        UhcDev->UsbHostControllerPpi.BulkTransfer = UhcBulkTransfer;
        UhcDev->UsbHostControllerPpi.SyncInterruptTransfer = UhcSyncInterruptTransfer;
        UhcDev->UsbHostControllerPpi.GetRootHubPortNumber =
            UhcGetRootHubPortNumber;
        UhcDev->UsbHostControllerPpi.GetRootHubPortStatus =
            UhcGetRootHubPortStatus;
        UhcDev->UsbHostControllerPpi.SetRootHubPortFeature =
            UhcSetRootHubPortFeature;
        UhcDev->UsbHostControllerPpi.ClearRootHubPortFeature =
            UhcClearRootHubPortFeature;
        UhcDev->UsbHostControllerPpi.Reset = UhciHcReset;        
        UhcDev->UsbHostControllerPpi.PreConfigureDevice = NULL;
        UhcDev->UsbHostControllerPpi.EnableEndpoints = NULL;
        UhcDev->UsbHostControllerPpi.ActivatePolling = UhciHcActivatePolling; 
        UhcDev->UsbHostControllerPpi.CurrentAddress = 0;

        // Install the PEI_USB_HOST_CONTROLLER_PPI interface
        // for this controller
        UhcDev->PpiDescriptor.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI
                                       |
                                       EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
        UhcDev->PpiDescriptor.Guid = &gAmiPeiUsbHostControllerPpiGuid;
        UhcDev->PpiDescriptor.Ppi = &UhcDev->UsbHostControllerPpi;
        if ( EFI_ERROR( (**PeiServices).InstallPpi(
                    PeiServices, &UhcDev->PpiDescriptor ) ) ) {
            return EFI_UNSUPPORTED;
        }

        Index++;
    }

    DEBUG((DEBUG_INFO, "UHCPEIM: Exit() \n"));

    return EFI_SUCCESS;
}


/**
    This function starts the periodic schedule for the interrupt endpoint-based
    devices (keyboard).

**/

EFI_STATUS
EFIAPI
UhciHcActivatePolling(
    IN EFI_PEI_SERVICES **PeiServices,
    IN VOID *UsbDev
)
{
    PEI_USB_DEVICE  *PeiUsbDev = (PEI_USB_DEVICE*)UsbDev;
    USB_UHC_DEV *UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS ( PeiUsbDev->UsbHcPpi );
	EFI_STATUS  Status;

    BOOLEAN     IsFirstTD;	
	UINT8       PktID;
    UINT8       *Ptr;
    UINT8       MaximumPacketLength;
    UINT8       DataToggle;	
    UINT16      LoadFrameListIndex;
    UINT32      i;    
    UINT32      StatusReg;
    UINT32      FrameNumReg;
    UINT32      DataLen;

    QH_STRUCT   *PtrQH;
    TD_STRUCT   *PtrFirstTD;
    TD_STRUCT   *PtrTD;
    TD_STRUCT   *PtrPreTD;

    static UhciPollingStarted = FALSE;

    if (UhciPollingStarted) return EFI_ALREADY_STARTED;

    UhciPollingStarted = TRUE;

    StatusReg = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameNumReg = UhcDev->UsbHostControllerBaseAddress + USBFRNUM;

    // if errors exist that cause host controller halt,
    // then return EFI_DEVICE_ERROR.
    if (IsHCHalted (UhcDev, StatusReg ) ||
         IsHCProcessErr(UhcDev, StatusReg) ||
         IsHostSysErr (UhcDev, StatusReg)) {

        ClearStatusReg(UhcDev, StatusReg);
        DEBUG((DEBUG_ERROR, "UHCIPEIM: UHCI Controller is Halted\n") );
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg ( UhcDev, StatusReg );

    PktID = PeiUsbDev->IntEndpoint & BIT7 ? INPUT_PACKET_ID: OUTPUT_PACKET_ID;
    DataLen = (UINT32) PeiUsbDev->IntMaxPkt;
    Ptr = UhcDev->PollingData;

    //  create QH structure and init
    Status = CreateQH ( UhcDev, &PtrQH );
	if ( EFI_ERROR ( Status ) ) return Status;   
   
    for (i = 0; i < PeiUsbDev->InterfaceDesc->NumEndpoints; i++)
    {
        if (PeiUsbDev->EndpointDesc[i]->EndpointAddress == PeiUsbDev->IntEndpoint) 
        {
            break;
        }
    }

    if ( ( PeiUsbDev->DataToggle & (1 << i) ) != 0 ) {
        DataToggle = 1;
    } else {
        DataToggle = 0;
    }
    
    MaximumPacketLength = (UINT8) PeiUsbDev->IntMaxPkt;

	IsFirstTD = TRUE;
    while (DataLen > 0) {
	
        // create TD structures and link together
        UINT16 PktSize;
	
        PktSize = (UINT16) DataLen;
        if (DataLen > MaximumPacketLength) {
            PktSize = MaximumPacketLength;
        }
        
        GenDataTD ( UhcDev, PeiUsbDev->DeviceAddress, PeiUsbDev->IntEndpoint, Ptr,
            PktSize, PktID, DataToggle, PeiUsbDev->DeviceSpeed, &PtrTD );
    
        if (IsFirstTD) {
            PtrFirstTD = PtrTD;
            PtrFirstTD->ptrNextTD = NULL;
            IsFirstTD = FALSE;
        } else {
            // Link two TDs in vertical depth
            LinkTDToTD( PtrPreTD, PtrTD );
        }
        
        PtrPreTD = PtrTD;
	
        DataToggle ^= 1;
        Ptr += PktSize;
        DataLen -= PktSize;
    }

    // link TD structures to QH structure
    LinkTDToQH ( PtrQH, PtrFirstTD );

    LoadFrameListIndex = (UINT16)
                         ( ( GetCurrentFrameNumber( UhcDev,
                                FrameNumReg ) ) % 1024 );

    for (i = 0; i <= 1024; i++) {

		if (i % 8 == 0)
	    {
        	LinkQHToFrameList( UhcDev->FrameListEntry,
            	(UINT16) ( (LoadFrameListIndex + i) % 1024 ), PtrQH);
	    }

        LoadFrameListIndex += 1;
        LoadFrameListIndex %= 1024;
    }

    UhcDev->IntQH = PtrQH; 
    UhcDev->IntTD = PtrFirstTD;

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UhcControlTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT8                       MaximumPacketLength,
    IN UINT16                      TransactionTranslator OPTIONAL,
    IN EFI_USB_DEVICE_REQUEST      *Request,
    IN EFI_USB_DATA_DIRECTION      TransferDirection,
    IN OUT VOID *Data              OPTIONAL,
    IN OUT UINTN *DataLength       OPTIONAL,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      StatusReg;
    UINT32      FrameNumReg;
    UINT8       PktID;
    QH_STRUCT   *ptrQH;
    TD_STRUCT   *ptrTD;
    TD_STRUCT   *ptrPreTD;
    TD_STRUCT   *ptrSetupTD;
    TD_STRUCT   *ptrStatusTD;
    EFI_STATUS  Status;
    UINTN       i;
    UINT32      DataLen;
    UINT8       *ptrDataSource;
    UINT8       *ptr;
    UINT8       DataToggle;
    UINT16      LoadFrameListIndex;

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );

    StatusReg = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameNumReg = UhcDev->UsbHostControllerBaseAddress + USBFRNUM;

    PktID = INPUT_PACKET_ID;

    if (Request == NULL || TransferResult == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // if errors exist that cause host controller halt,
    // then return EFI_DEVICE_ERROR.
    //
    if (IsHCHalted( UhcDev, StatusReg) ||
        IsHCProcessErr( UhcDev, StatusReg) ||
        IsHostSysErr( UhcDev, StatusReg)) {

        ClearStatusReg(UhcDev, StatusReg);
        *TransferResult = EFI_USB_ERR_SYSTEM;
        DEBUG((DEBUG_ERROR, "UHCIPEIM: Ctrler halted\n") );
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg( UhcDev, StatusReg );

    //
    // Setup Stage of Control Transfer
    //

    //
    // create QH structure and init
    //
    Status = CreateQH( UhcDev, &ptrQH );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    //
    // generate Setup Stage TD
    //

    GenSetupStageTD( UhcDev, DeviceAddress, 0,
        DeviceSpeed, (UINT8 *) Request,
        sizeof(EFI_USB_DEVICE_REQUEST), &ptrSetupTD );

    //
    // link setup TD structures to QH structure
    //
    LinkTDToQH( ptrQH, ptrSetupTD );

    ptrPreTD = ptrSetupTD;

    //
    //  Data Stage of Control Transfer
    //
    switch (TransferDirection)
    {

    case EfiUsbDataIn:
        PktID = INPUT_PACKET_ID;
        ptrDataSource = Data;
        DataLen = (UINT32) *DataLength;
        ptr = ptrDataSource;
        break;

    case EfiUsbDataOut:
        PktID = OUTPUT_PACKET_ID;
        ptrDataSource = Data;
        DataLen = (UINT32) *DataLength;
        ptr = ptrDataSource;
        break;

        //
        // no data stage
        //
    case EfiUsbNoData:
        if (*DataLength != 0) {
            return EFI_INVALID_PARAMETER;
        }
        PktID = OUTPUT_PACKET_ID;
        ptrDataSource = NULL;
        DataLen = 0;
        ptr = NULL;
        break;

    default:
        return EFI_INVALID_PARAMETER;
    }

    DataToggle = 1;

    ptrTD = ptrSetupTD;
    while (DataLen > 0) {

        //
        // create TD structures and link together
        //

        UINT8 pktsize;

        //
        // pktsize is the data load size of each TD carries.
        //
        pktsize = (UINT8) DataLen;
        if (DataLen > MaximumPacketLength) {
            pktsize = MaximumPacketLength;
        }
        GenDataTD( UhcDev, DeviceAddress, 0, ptr, pktsize, PktID, DataToggle,
            DeviceSpeed, &ptrTD );

        //
        // Link two TDs in vertical depth
        //
        LinkTDToTD( ptrPreTD, ptrTD );
        ptrPreTD = ptrTD;

        DataToggle ^= 1;
        ptr += pktsize;
        DataLen -= pktsize;
    }

    //
    // ptrPreTD points to the last TD before the Setup-Stage TD.
    //
    ptrPreTD = ptrTD;

    //
    // Status Stage of Control Transfer
    //
    if (PktID == OUTPUT_PACKET_ID) {
        PktID = INPUT_PACKET_ID;
    }
    else {
        PktID = OUTPUT_PACKET_ID;
    }
    //
    // create Status Stage TD structure
    //
    CreateStatusTD( UhcDev,
        DeviceAddress,
        0,
        PktID,
        DeviceSpeed,
        &ptrStatusTD );

    LinkTDToTD( ptrPreTD, ptrStatusTD );

    //
    // get the frame list index that the QH-TDs will be linked to.
    //
    LoadFrameListIndex = (UINT16) ( ( GetCurrentFrameNumber
                                     ( UhcDev, FrameNumReg ) ) % 1024 );

    //
    // link QH-TDs to total 500 frame list entry to speed up the execution.
    //
    for (i = 0; i < 500; i++) {

        //
        // put the QH-TDs directly or indirectly into the proper place in
        // the Frame List
        //
        LinkQHToFrameList( UhcDev->FrameListEntry,
            (UINT16) ( (LoadFrameListIndex + i) % 1024 ), ptrQH );
    }

    //
    // Poll QH-TDs execution and get result.
    // detail status is returned
    //
    //TimeOut = 1000; //bala

    Status = ExecuteControlTransfer( UhcDev, ptrSetupTD,
        LoadFrameListIndex, DataLength,
        TimeOut, TransferResult );
    //
    // TRUE means must search other framelistindex
    //
    DeleteSingleQH( UhcDev, ptrQH, LoadFrameListIndex, TRUE );
    DeleteQueuedTDs( UhcDev, ptrSetupTD );

    //
    // if has errors that cause host controller halt, then return
    // EFI_DEVICE_ERROR directly.
    //
    if (IsHCHalted(UhcDev, StatusReg) ||
        IsHCProcessErr(UhcDev, StatusReg) ||
        IsHostSysErr(UhcDev, StatusReg)) {

        ClearStatusReg(UhcDev, StatusReg);
        *TransferResult |= EFI_USB_ERR_SYSTEM;
        DEBUG((DEBUG_ERROR,
                    "UHCIPEIM: Ctrl transfer failed\n") );
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg(UhcDev, StatusReg);

    return Status;
}

EFI_STATUS
EFIAPI
UhcBulkTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       EndPointAddress,
	IN UINT8                       DeviceSpeed,
    IN UINT16                      MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN OUT VOID                    *Data,
    IN OUT UINTN                   *DataLength,
    IN OUT UINT8                   *DataToggle,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      StatusReg;
    UINT32      FrameNumReg;

    UINT32      DataLen;

    QH_STRUCT   *ptrQH;
    TD_STRUCT   *ptrFirstTD;
    TD_STRUCT   *ptrTD;
    TD_STRUCT   *ptrPreTD;

    UINT16      LoadFrameListIndex;
    UINT16      SavedFrameListIndex;

    UINT8       PktID;
    UINT8       *ptrDataSource;
    UINT8       *ptr;

    BOOLEAN     IsFirstTD;

    EFI_STATUS  Status;
    UINT32      i;

    EFI_USB_DATA_DIRECTION TransferDirection;
    //
    //  Used to calculate how many entries are linked to the specified bulk
    //  transfer QH-TDs
    //
    UINT32  LinkTimes;

    BOOLEAN ShortPacketEnable;

    UINT16  CommandContent;

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );

    //
    // Enable the maximum packet size (64bytes)
    // that can be used for full speed bandwidth reclamation
    // at the end of a frame.
    //
    CommandContent = USBReadPortW( UhcDev,
        UhcDev->UsbHostControllerBaseAddress + USBCMD );

    if ( (CommandContent & USBCMD_MAXP) != USBCMD_MAXP ) {
        CommandContent |= USBCMD_MAXP;
        USBWritePortW( UhcDev, UhcDev->UsbHostControllerBaseAddress + USBCMD,
            CommandContent );

    }

    StatusReg = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameNumReg = UhcDev->UsbHostControllerBaseAddress + USBFRNUM;

    //
    // these code lines are added here per complier's strict demand
    //
    PktID = INPUT_PACKET_ID;
    ptrTD = NULL;
    ptrFirstTD = NULL;
    ptrPreTD = NULL;
    LinkTimes = 1;
    DataLen = 0;
    ptr = NULL;

    ShortPacketEnable = FALSE;

    if ( (DataLength == 0) || (Data == NULL)
        || (TransferResult == NULL) ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( (*DataToggle != 1) && (*DataToggle != 0) ) {
        return EFI_INVALID_PARAMETER;
    }

    if (MaximumPacketLength != 8 && MaximumPacketLength != 16
        && MaximumPacketLength != 32 && MaximumPacketLength != 64) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // if has errors that cause host controller halt, then return
    // EFI_DEVICE_ERROR directly.
    //
    if ( IsHCHalted( UhcDev, StatusReg ) ||
        IsHCProcessErr( UhcDev, StatusReg ) ||
        IsHostSysErr( UhcDev, StatusReg ) ) {

        ClearStatusReg( UhcDev, StatusReg );
        *TransferResult = EFI_USB_ERR_SYSTEM;
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg( UhcDev, StatusReg );

    //
    // construct QH and TD data structures,
    // and link them together
    //
    if (EndPointAddress & 0x80) {
        TransferDirection = EfiUsbDataIn;
    }
    else {
        TransferDirection = EfiUsbDataOut;
    }

    switch (TransferDirection)
    {

    case EfiUsbDataIn:
        ShortPacketEnable = TRUE;
        PktID = INPUT_PACKET_ID;
        ptrDataSource = Data;
        DataLen = (UINT32) *DataLength;
        ptr = ptrDataSource;
        break;

    case EfiUsbDataOut:
        PktID = OUTPUT_PACKET_ID;
        ptrDataSource = Data;
        DataLen = (UINT32) *DataLength;
        ptr = ptrDataSource;
        break;
    }

    //
    //  create QH structure and init
    //
    Status = CreateQH( UhcDev, &ptrQH );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    //
    // Self link the QH, to make NAK TD retried in one frame.
    //
    //SelfLinkBulkTransferQH (ptrQH);

    //
    // i is used to calculate the total number of TDs.
    //
    i = 0;

    IsFirstTD = TRUE;
    while (DataLen > 0) {

        //
        // create TD structures and link together
        //
        UINT16 pktsize;

        pktsize = (UINT16) DataLen;
        if (DataLen > MaximumPacketLength) {
            pktsize = MaximumPacketLength;
        }
        GenDataTD( UhcDev, DeviceAddress, EndPointAddress, ptr,
            pktsize, PktID, *DataToggle, USB_FULL_SPEED_DEVICE, &ptrTD );

        //
        // Enable short packet detection.
        // (default action is disabling short packet detection)
        //
        if (ShortPacketEnable) {
            EnableorDisableTDShortPacket( ptrTD, TRUE );
        }

        if (IsFirstTD) {
            ptrFirstTD = ptrTD;
            ptrFirstTD->ptrNextTD = NULL;
            IsFirstTD = FALSE;
        }
        else {
            //
            // Link two TDs in vertical depth
            //
            LinkTDToTD( ptrPreTD, ptrTD );
        }

        i++;

        ptrPreTD = ptrTD;

        *DataToggle ^= 1;
        ptr += pktsize;
        DataLen -= pktsize;
    }

    //
    // link TD structures to QH structure
    //
    LinkTDToQH( ptrQH, ptrFirstTD );

    //
    // calculate how many entries are linked to the specified bulk
    // transfer QH-TDs the below values are referred to the USB spec
    // revision1.1.
    //
    switch (MaximumPacketLength)
    {
    case 8:
        LinkTimes = i / 71 + 1;
        break;

    case 16:
        LinkTimes = i / 51 + 1;
        break;

    case 32:
        LinkTimes = i / 33 + 1;
        break;

    case 64:
        LinkTimes = i / 19 + 1;
        break;
    }
    LinkTimes += 200; // redundant

    //
    // put QH-TDs into  Frame list
    //

    LoadFrameListIndex = (UINT16)
                         ( ( GetCurrentFrameNumber( UhcDev,
                                FrameNumReg ) ) % 1024 );
    SavedFrameListIndex = LoadFrameListIndex;

    for (i = 0; i <= LinkTimes; i++) {

        //
        // put the QH-TD directly or indirectly into the proper
        // place in the Frame List
        //
        LinkQHToFrameList( UhcDev->FrameListEntry, LoadFrameListIndex, ptrQH );

        LoadFrameListIndex += 1;
        LoadFrameListIndex %= 1024;
    }

    LoadFrameListIndex = SavedFrameListIndex;

    //
    // Execute QH-TD and get result
    //
    //
    // detail status is put into the Result field in the pIRP
    // the Data Toggle value is also re-updated to the value
    // of the last successful TD
    //
    Status = ExecBulkTransfer( UhcDev, ptrFirstTD,
        LoadFrameListIndex, DataLength,
        DataToggle, TimeOut, TransferResult );

    //
    // Delete Bulk transfer QH-TD structure
    // and maitain the pointers in the Frame List
    // and other pointers in related QH structure
    //


    // TRUE means must search other framelistindex
    DeleteSingleQH( UhcDev, ptrQH, LoadFrameListIndex, TRUE );
    DeleteQueuedTDs( UhcDev, ptrFirstTD );

    //
    // if has errors that cause host controller halt, then
    // return EFI_DEVICE_ERROR directly.
    //
    if ( IsHCHalted( UhcDev, StatusReg ) ||
        IsHCProcessErr( UhcDev, StatusReg ) ||
        IsHostSysErr( UhcDev, StatusReg ) ) {

        ClearStatusReg( UhcDev, StatusReg );
        *TransferResult |= EFI_USB_ERR_SYSTEM;
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg( UhcDev, StatusReg );

    return Status;
}

EFI_STATUS
EFIAPI
UhcSyncInterruptTransfer(
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
    USB_UHC_DEV *UhcDev;
    UINT8       *Src;
    UINT8       *Dst;    
    UINT32      StatusReg;
    UINT32      FrameNumReg;	
    UINTN       ErrTDPos;
    UINTN       ScrollNum;
    UINTN       Delay;
    UINTN       Count;
    QH_STRUCT   *PtrQH;	
    TD_STRUCT   *PtrTD; 		
    
    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );

    StatusReg = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameNumReg = UhcDev->UsbHostControllerBaseAddress + USBFRNUM;

    if ( (DataLength == 0) || (Data == NULL)
        || (TransferResult == NULL) ) 
	{
        return EFI_INVALID_PARAMETER;
    }

    if ( (*DataToggle != 1) && (*DataToggle != 0) ) 
	{
        return EFI_INVALID_PARAMETER;
    }

    if (MaximumPacketLength != 8 && MaximumPacketLength != 16
        && MaximumPacketLength != 32 && MaximumPacketLength != 64) 
	{
        return EFI_INVALID_PARAMETER;
    }

    if (IsHCHalted(UhcDev, StatusReg) ||
         IsHCProcessErr(UhcDev, StatusReg) ||
         IsHostSysErr(UhcDev, StatusReg)) {
        ClearStatusReg (UhcDev, StatusReg);
        *TransferResult = EFI_USB_ERR_SYSTEM;
        DEBUG((DEBUG_ERROR, "UHCIPEIM: UHCI Controller is halted\n") );
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg(UhcDev, StatusReg);

    PtrQH = UhcDev->IntQH; 
    PtrTD = UhcDev->IntTD; 

    ErrTDPos = 0;
    *TransferResult = EFI_USB_NOERROR;

    Delay = (TimeOut * STALL_1_MILLI_SECOND / 50) + 1;

    do {
        CheckTDsResults ( PtrTD, TransferResult, &ErrTDPos, DataLength );

        if ( (*TransferResult & EFI_USB_ERR_NOTEXECUTE) !=
            EFI_USB_ERR_NOTEXECUTE ) 
        {
            for (Src = PtrTD->pTDBuffer,
                 Dst = (UINT8*)Data,
                 Count = 0; Count < 8; Count++)
            {
                *Dst++ = *Src++;
            }
            break;    
        }
        
        UhcDev->StallPpi->Stall ( UhcDev->PeiServices, UhcDev->StallPpi, 50 );
    } while (Delay--);

    // has error
    if (*TransferResult != EFI_USB_NOERROR) 
    {
        ScrollNum = CountTDsNumber( PtrTD ) - ErrTDPos;
        if (ScrollNum % 2) 
        {
            *DataToggle ^= 1;
        }
        return EFI_DEVICE_ERROR;
    }


    //
    // if has errors that cause host controller halt, then return
    // EFI_DEVICE_ERROR directly.
    //
    if (IsHCHalted(UhcDev, StatusReg) ||
         IsHCProcessErr(UhcDev, StatusReg) ||
         IsHostSysErr(UhcDev, StatusReg)) {

        ClearStatusReg(UhcDev, StatusReg);
        *TransferResult |= EFI_USB_ERR_SYSTEM;
        DEBUG((DEBUG_ERROR,
                    "UHCIPEIM: Interrupt transfer failed\n"));
        return EFI_DEVICE_ERROR;
    }

    ClearStatusReg( UhcDev, StatusReg );

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UhcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      PSAddr;
    UINT16      RHPortControl;
    UINT32      i;

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );

    if (PortNumber == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    *PortNumber = 0;

    for (i = 0; i < 2; i++) {
        PSAddr = UhcDev->UsbHostControllerBaseAddress + USBPORTSC1 + i * 2;
        //RHPortControl = ReadRootPortReg(UhcDev->CpuIoPpi,PSAddr);
        RHPortControl = USBReadPortW( UhcDev, PSAddr );
        //
        // Port Register content is valid
        //
        if (RHPortControl != 0xff) {
            (*PortNumber)++;
        }
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UhcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      PSAddr;
    UINT16      RHPortStatus;    // root hub port status
    UINT8       TotalPortNumber;

    if (PortStatus == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    UhcGetRootHubPortNumber( PeiServices, This, &TotalPortNumber );
    if (PortNumber > TotalPortNumber) {
        return EFI_INVALID_PARAMETER;
    }

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );
    PSAddr = UhcDev->UsbHostControllerBaseAddress +
             USBPORTSC1 + (PortNumber - 1) * 2;

    PortStatus->PortStatus = 0;
    PortStatus->PortChangeStatus = 0;

    //RHPortStatus = ReadRootPortReg (UhcDev->CpuIoPpi,PSAddr) ;
    RHPortStatus = USBReadPortW( UhcDev, PSAddr );

    //
    //    Fill Port Status bits
    //

    //
    // Current Connect Status
    //
    if (RHPortStatus & USBPORTSC_CCS) {
        PortStatus->PortStatus |= USB_PORT_STAT_CONNECTION;
    }

    //
    // Port Enabled/Disabled
    //
    if (RHPortStatus & USBPORTSC_PED) {
        PortStatus->PortStatus |= USB_PORT_STAT_ENABLE;
    }

    //
    // Port Suspend
    //
    if (RHPortStatus & USBPORTSC_SUSP) {
        PortStatus->PortStatus |= USB_PORT_STAT_SUSPEND;
    }

    //
    // Port Reset
    //
    if (RHPortStatus & USBPORTSC_PR) {
        PortStatus->PortStatus |= USB_PORT_STAT_RESET;
    }

    //
    // Low Speed Device Attached
    //
    if (RHPortStatus & USBPORTSC_LSDA) {
        PortStatus->PortStatus |= USB_PORT_STAT_LOW_SPEED;
    }

    //
    //   Fill Port Status Change bits
    //

    //
    // Connect Status Change
    //
    if (RHPortStatus & USBPORTSC_CSC) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_CONNECTION;
    }

    //
    // Port Enabled/Disabled Change
    //
    if (RHPortStatus & USBPORTSC_PEDC) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_ENABLE;
    }

	// Port Reset Change
	if (UhcDev->PortResetStatusChangeMap & (1 << (PortNumber - 1))) {
		PortStatus->PortChangeStatus |= USB_PORT_STAT_C_RESET;
	}
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UhcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      PSAddr;
    UINT32      CommandRegAddr;
    UINT16      RHPortControl;    // root hub port status
    UINT8       TotalPortNumber;

    UhcGetRootHubPortNumber( PeiServices, This, &TotalPortNumber );
    if (PortNumber > TotalPortNumber) {
        return EFI_INVALID_PARAMETER;
    }

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );
    PSAddr = UhcDev->UsbHostControllerBaseAddress +
             USBPORTSC1 + (PortNumber - 1) * 2;
    CommandRegAddr = UhcDev->UsbHostControllerBaseAddress + USBCMD;

    RHPortControl = USBReadPortW( UhcDev, PSAddr );

    switch (PortFeature)
    {

    case EfiUsbPortSuspend:
        if ( !(USBReadPortW( UhcDev, CommandRegAddr ) & USBCMD_EGSM) ) {
            //
            // if global suspend is not active, can set port suspend
            //
            RHPortControl &= 0xfff5;
            RHPortControl |= USBPORTSC_SUSP;
        }
        break;

    case EfiUsbPortReset:
        RHPortControl &= 0xfff5;
        RHPortControl |= USBPORTSC_PR;      // Set the reset bit
        USBWritePortW( UhcDev, PSAddr, RHPortControl );
        UhcDev->StallPpi->Stall (UhcDev->PeiServices,UhcDev->StallPpi,10 * 1000);
        RHPortControl = USBReadPortW( UhcDev, PSAddr );
        RHPortControl &= 0xfff5;
        RHPortControl &= ~USBPORTSC_PR;
        USBWritePortW( UhcDev, PSAddr, RHPortControl );
		UhcDev->StallPpi->Stall (UhcDev->PeiServices,UhcDev->StallPpi, 300); // 300 us delay
		RHPortControl = USBReadPortW( UhcDev, PSAddr );
		RHPortControl &= 0xfff5;
		RHPortControl |= USBPORTSC_CSC;
		USBWritePortW( UhcDev, PSAddr, RHPortControl );
		RHPortControl = USBReadPortW( UhcDev, PSAddr );
		RHPortControl &= 0xfff5;
		RHPortControl |= USBPORTSC_PED;
		USBWritePortW( UhcDev, PSAddr, RHPortControl );
        UhcDev->PortResetStatusChangeMap |= (1 << (PortNumber - 1));
	    return EFI_SUCCESS;

    case EfiUsbPortPower:
        break;

    case EfiUsbPortEnable:
        RHPortControl &= 0xfff5;
        RHPortControl |= USBPORTSC_PED;
        break;

    default:
        return EFI_INVALID_PARAMETER;
    }

    //WriteRootPortReg(UhcDev->CpuIoPpi,PSAddr,RHPortControl) ;
    USBWritePortW( UhcDev, PSAddr, RHPortControl );

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UhcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    USB_UHC_DEV *UhcDev;
    UINT32      PSAddr;
    UINT16      RHPortControl;
    UINT8       TotalPortNumber;

    UhcGetRootHubPortNumber( PeiServices, This, &TotalPortNumber );

    if (PortNumber > TotalPortNumber) {
        return EFI_INVALID_PARAMETER;
    }

    UhcDev = PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( This );
    PSAddr = UhcDev->UsbHostControllerBaseAddress +
             USBPORTSC1 + (PortNumber - 1) * 2;

    //RHPortControl = ReadRootPortReg (UhcDev->CpuIoPpi,PSAddr) ;
    RHPortControl = USBReadPortW( UhcDev, PSAddr );

    switch (PortFeature)
    {

        //
        // clear PORT_ENABLE feature means disable port.
        //
    case EfiUsbPortEnable:
        RHPortControl &= 0xfff5;
        RHPortControl &= ~USBPORTSC_PED;
        break;

        //
        // clear PORT_SUSPEND feature means resume the port.
        // (cause a resume on the specified port if in suspend mode)
        //
    case EfiUsbPortSuspend:
        RHPortControl &= 0xfff5;
        RHPortControl &= ~USBPORTSC_SUSP;
        break;

        //
        // no operation
        //
    case EfiUsbPortPower:
        break;

        //
        // clear PORT_RESET means clear the reset signal.
        //
    case EfiUsbPortReset:
        RHPortControl &= 0xfff5;
        RHPortControl &= ~USBPORTSC_PR;
        break;

        //
        // clear connect status change
        //
    case EfiUsbPortConnectChange:
        RHPortControl &= 0xfff5;
        RHPortControl |= USBPORTSC_CSC;
        break;

        //
        // clear enable/disable status change
        //
    case EfiUsbPortEnableChange:
        RHPortControl &= 0xfff5;
        RHPortControl |= USBPORTSC_PEDC;
        break;

        //
        // root hub does not support this request
        //
    case EfiUsbPortSuspendChange:
        break;

        //
        // root hub does not support this request
        //
    case EfiUsbPortOverCurrentChange:
        break;

        //
        // root hub does not support this request
        //
    case EfiUsbPortResetChange:
		UhcDev->PortResetStatusChangeMap &= ~(1 << (PortNumber - 1));
	    return EFI_SUCCESS;

    default:
        return EFI_INVALID_PARAMETER;
    }

    //WriteRootPortReg(UhcDev->CpuIoPpi,PSAddr,RHPortControl) ;
    USBWritePortW( UhcDev, PSAddr, RHPortControl );

    return EFI_SUCCESS;
}


/**
    This routine resets the Host Controler and its USB bus 
    according to the attributes
    @param EFI_PEI_SERVICES            	**PeiServices,
        PEI_USB_HOST_CONTROLLER_PPI		*This,
        UINT16                      	Attributes

    @retval 
        EFI_SUCCESS     Successful reset

**/

EFI_STATUS
EFIAPI
UhciHcReset(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT16                      Attributes
)
{
    UINT16      		 Delay;
    UINT16  			 WordData;
    USB_UHC_DEV			 *UhcDev = 
							PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS (This);
    UINT64               Val64;
    EFI_PEI_PCI_CFG2_PPI *PciCfgPpi = (*PeiServices)->PciCfg;
    
    switch (Attributes) {
        case EFI_USB_HC_RESET_GLOBAL:

			WordData = USBReadPortW( UhcDev,
							UhcDev->UsbHostControllerBaseAddress + USBSTS );
		    if (!(WordData & USBSTS_HCH)){
    	    	// HC is still running. Stop it by programming HC run bit
	    	    USBWritePortW (UhcDev, UhcDev->UsbHostControllerBaseAddress + USBCMD,
						WordData & ~USBCMD_RS);

    	        // Check whether the host controller is halted
	    	    for (Delay = 0; Delay < 1000; Delay++) {
					UhcDev->StallPpi->Stall (UhcDev->PeiServices, UhcDev->StallPpi, 100);
			        WordData = USBReadPortW (UhcDev,
						UhcDev->UsbHostControllerBaseAddress + USBSTS );
		            if (WordData & USBSTS_HCH) {
						break;
	        	    }
        		}
	    	}

		    // Reset the host controller
	 		USBWritePortW (UhcDev, UhcDev->UsbHostControllerBaseAddress + USBCMD,
				USBCMD_GRESET );

			// Recommended 10msec delay, UHCI Spec, p.12, GRESET description 			UhcDev->StallPpi->Stall (UhcDev->PeiServices, UhcDev->StallPpi, 10 * 1000);

			USBWritePortW( UhcDev, UhcDev->UsbHostControllerBaseAddress + USBCMD,
						   0x0000);

            // Clear PCI Resource

            Val64 = 0;

            // Clear UHCI Base Address Register
            PciCfgPpi->Write(PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint32,
                gUhciPciReg[UhcDev->ControllerIndex] |
                PCI_REG_UHCI_BASE_ADDRESS_REGISTER, &Val64);
			
			// Clear UHCI Command Register
            PciCfgPpi->Write(PeiServices, PciCfgPpi, EfiPeiPciCfgWidthUint16,
                gUhciPciReg[UhcDev->ControllerIndex] |
                PCI_REG_UHCI_COMMAND_REGISTER, &Val64);

            break;
		default:
			return EFI_UNSUPPORTED;
    }
    
    return EFI_SUCCESS;  
}


EFI_STATUS InitializeUsbHC (
    USB_UHC_DEV *UhcDev )
{
    EFI_STATUS  Status;
    UINT32      FrameListBaseAddrReg;
    UINT32      StatusReg;
    UINT32      CommandReg;
    UINT16      Command;
    UINTN       Delay;

    // Create and Initialize Frame List For the Host Controller.
    Status = CreateFrameList( UhcDev );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    FrameListBaseAddrReg = UhcDev->UsbHostControllerBaseAddress +
                           USBFLBASEADD;
    StatusReg = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    CommandReg = UhcDev->UsbHostControllerBaseAddress + USBCMD;

    //Issue a Global reset 
    Command = USBReadPortW (UhcDev,CommandReg);
    Command |= USBCMD_GRESET ;
    USBWritePortW (UhcDev,CommandReg,Command);
    
    Delay = (100 * STALL_1_MILLI_SECOND / 50) + 1;
    
    do {
        UhcDev->StallPpi->Stall (UhcDev->PeiServices,UhcDev->StallPpi,50);
    } while(Delay--);
    
    Command = USBReadPortW (UhcDev,CommandReg);
    Command &= !USBCMD_GRESET ;
    USBWritePortW (UhcDev,CommandReg,Command);
    
    Delay = (100 * STALL_1_MILLI_SECOND / 50) + 1;
    
    do {
        UhcDev->StallPpi->Stall (UhcDev->PeiServices,UhcDev->StallPpi,50);
    } while(Delay--);

    // Set Frame List Base Address to the specific register to
    // inform the hardware.
    SetFrameListBaseAddress( UhcDev, FrameListBaseAddrReg,
        (UINT32) (UINTN) (UhcDev->FrameListEntry) );

    // Make the Host Controller in Run state.
    if ( IsHostSysErr( UhcDev, StatusReg )
        || IsHCProcessErr( UhcDev, StatusReg ) ) {
        return EFI_DEVICE_ERROR;
    }

    Command = USBReadPortW( UhcDev, CommandReg );
    Command |= USBCMD_RS;

    USBWritePortW( UhcDev, CommandReg, Command );
    return EFI_SUCCESS;
}


EFI_STATUS CreateFrameList (
    USB_UHC_DEV *UhcDev )
{
    EFI_STATUS Status;
    EFI_PHYSICAL_ADDRESS FrameListBaseAddr;

    //
    // The Frame List ocupies 4K bytes,
    // and must be aligned on 4-Kbyte boundaries.
    //
    Status = ( **(UhcDev->PeiServices) ).AllocatePages(
        UhcDev->PeiServices,
        EfiBootServicesData,
        1,
        &FrameListBaseAddr );

    if (Status != EFI_SUCCESS) {
        return EFI_OUT_OF_RESOURCES;
    }

    UhcDev->FrameListEntry = (FRAMELIST_ENTRY *) ( (UINTN) FrameListBaseAddr );

    InitFrameList( UhcDev );

    return EFI_SUCCESS;
}


VOID InitFrameList (
    USB_UHC_DEV *UhcDev )
{
    FRAMELIST_ENTRY *FrameListPtr;
    UINTN i;

    //
    // Validate each Frame List Entry
    //
    FrameListPtr = UhcDev->FrameListEntry;
    for (i = 0; i < 1024; i++) {
        FrameListPtr->FrameListPtrTerminate = 1;
        FrameListPtr->FrameListPtr = 0;
        FrameListPtr->FrameListPtrQSelect = 0;
        FrameListPtr->FrameListRsvd = 0;
        FrameListPtr++;
    }
}


UINT16 USBReadPortW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port )
{
    UINT16 Data;

    //
    // Perform 16bit Read
    //
    UhcDev->CpuIoPpi->Io.Read(
        UhcDev->PeiServices,
        UhcDev->CpuIoPpi,
        EfiPeiCpuIoWidthUint16,
        (UINT64) Port,
        1,
        &Data
    );

    return Data;
}


VOID USBWritePortW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port,
    IN UINT16      Data )
{
    //
    // Perform 16bit Write
    //
    UhcDev->CpuIoPpi->Io.Write(
        UhcDev->PeiServices,
        UhcDev->CpuIoPpi,
        EfiPeiCpuIoWidthUint16,
        (UINT64) Port,
        1,
        &Data
    );
}


VOID USBWritePortDW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port,
    IN UINT32      Data )
{
    //
    // Perform 32bit Write
    //

    UhcDev->CpuIoPpi->Io.Write(
        UhcDev->PeiServices,
        UhcDev->CpuIoPpi,
        EfiPeiCpuIoWidthUint32,
        (UINT64) Port,
        1,
        &Data
    );
}


UINT32 USBReadPortDW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port )
{
    UINT32 Data;

    //
    // Perform 16bit Read
    //
    UhcDev->CpuIoPpi->Io.Read(
        UhcDev->PeiServices,
        UhcDev->CpuIoPpi,
        EfiPeiCpuIoWidthUint32,
        (UINT64) Port,
        1,
        &Data
    );
    return Data;
}


//
//  USB register-base helper functions
//

VOID ClearStatusReg (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      STSAddr )
{
    UINT16 UsbSts;

    //
    // Clear the content of UHC's Status Register
    //
    UsbSts = 0x003f;
    USBWritePortW( UhcDev, STSAddr, UsbSts );
    //WriteUHCStatusReg (PciIo,STSAddr,UsbSts);
}


BOOLEAN IsHostSysErr (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr )
{
    UINT16 StatusValue;

    //
    // Detect whether the interrupt is caused by serious error.
    // see "UHCI Design Guid".
    //
    StatusValue = USBReadPortW( UhcDev, StatusRegAddr );

    if (StatusValue & USBSTS_HSE) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


BOOLEAN IsHCProcessErr (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr )
{
    UINT16 StatusValue;

    //
    // Detect whether the interrupt is caused by fatal error.
    // see "UHCI Design Guid".
    //
    StatusValue = USBReadPortW( UhcDev, StatusRegAddr );

    if (StatusValue & USBSTS_HCPE) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


BOOLEAN IsHCHalted (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr )
{
    UINT16 StatusValue;

    //
    // Detect whether the the Host Controller is halted.
    //
    StatusValue = USBReadPortW( UhcDev, StatusRegAddr );

    if (StatusValue & USBSTS_HCH) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


UINT16 GetCurrentFrameNumber (
    USB_UHC_DEV *UhcDev,
    UINT32      FRNUMAddr )
{
    //
    // Gets value in the USB frame number register.
    //

    return (UINT16) (USBReadPortW( UhcDev, FRNUMAddr ) & 0x03ff);
}


VOID SetFrameListBaseAddress (
    USB_UHC_DEV *UhcDev,
    UINT32      FLBASEADDRReg,
    UINT32      Addr )
{
    //
    // Sets value in the USB Frame List Base Address register.
    //
    USBWritePortDW( UhcDev, FLBASEADDRReg, (UINT32) (Addr & 0xfffff000) );
}


////////////////////////////////////////////////////////////////
//
// QH TD related Helper Functions
//
////////////////////////////////////////////////////////////////

//
// functions for QH
//
EFI_STATUS AllocateQHStruct (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   **ppQHStruct )
{
    EFI_STATUS Status;

    *ppQHStruct = NULL;

    Status = UhcAllocatePool( UhcDev, MEM_QH_TD_TYPE,
        (UINT8 **) ppQHStruct, sizeof(QH_STRUCT) );

    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

//    ZeroMem( *ppQHStruct, sizeof(QH_STRUCT) );
    (*UhcDev->PeiServices)->SetMem(*ppQHStruct, sizeof(QH_STRUCT), 0);

    return EFI_SUCCESS;
}


VOID InitQH (
    IN QH_STRUCT *ptrQH )
{
    //
    // Make QH ready
    //
    SetQHHorizontalValidorInvalid( ptrQH, FALSE );
    SetQHVerticalValidorInvalid( ptrQH, FALSE );
}


EFI_STATUS CreateQH (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   **pptrQH )
{
    EFI_STATUS Status;

    //
    // allocate align memory for QH_STRUCT
    //
    Status = AllocateQHStruct( UhcDev, pptrQH );
    if ( EFI_ERROR( Status ) ) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // init each field of the QH_STRUCT
    //
    InitQH( *pptrQH );

    return EFI_SUCCESS;
}


VOID SetQHHorizontalLinkPtr (
    IN QH_STRUCT *ptrQH,
    IN VOID      *ptrNext )
{
    //
    // Since the QH_STRUCT is aligned on 16-byte boundaries,
    // Only the highest 28bit of the address is valid
    // (take 32bit address as an example).
    //
    ptrQH->QH.QHHorizontalPtr = (UINT32) (UINTN) ptrNext >> 4;
}


VOID *GetQHHorizontalLinkPtr (
    IN QH_STRUCT *ptrQH )
{
    //
    // Restore the 28bit address to 32bit address
    //(take 32bit address as an example)
    //
    return ( (VOID *) (UINTN) ( (ptrQH->QH.QHHorizontalPtr) << 4 ) );
}


VOID SetQHHorizontalQHorTDSelect (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bQH )
{
    //
    // if QH is connected, the specified bit is set,
    // if TD is connected, the specified bit is cleared.
    //
    ptrQH->QH.QHHorizontalQSelect = bQH ? 1 : 0;
}


VOID SetQHHorizontalValidorInvalid (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bValid )
{
    //
    // Valid means the horizontal link pointer is valid,
    // else, it's invalid.
    //
    ptrQH->QH.QHHorizontalTerminate = bValid ? 0 : 1;
}


VOID SetQHVerticalLinkPtr (
    IN QH_STRUCT *ptrQH,
    IN VOID      *ptrNext )
{
    //
    // Since the QH_STRUCT is aligned on 16-byte boundaries,
    // Only the highest 28bit of the address is valid
    // (take 32bit address as an example).
    //
    ptrQH->QH.QHVerticalPtr = (UINT32) (UINTN) ptrNext >> 4;
}


VOID SetQHVerticalQHorTDSelect (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bQH )
{
    //
    // Set the specified bit if the Vertical Link Pointer pointing to a QH,
    // Clear the specified bit if the Vertical Link Pointer pointing to a TD.
    //
    ptrQH->QH.QHVerticalQSelect = bQH ? 1 : 0;
}


VOID SetQHVerticalValidorInvalid (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bValid )
{
    //
    // If TRUE, meaning the Vertical Link Pointer field is valid,
    // else, the field is invalid.
    //
    ptrQH->QH.QHVerticalTerminate = bValid ? 0 : 1;
}


BOOLEAN GetQHHorizontalValidorInvalid (
    IN QH_STRUCT *ptrQH )
{
    //
    // If TRUE, meaning the Horizontal Link Pointer field is valid,
    // else, the field is invalid.
    //
    return (BOOLEAN) ( !(ptrQH->QH.QHHorizontalTerminate) );
}


//
// functions for TD
//

EFI_STATUS AllocateTDStruct (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   **ppTDStruct )
{
    EFI_STATUS Status = EFI_SUCCESS;

    *ppTDStruct = NULL;

    Status = UhcAllocatePool( UhcDev, MEM_QH_TD_TYPE,
        (UINT8 **) ppTDStruct, sizeof(TD_STRUCT) );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    (*UhcDev->PeiServices)->SetMem(*ppTDStruct, sizeof(TD_STRUCT), 0);
//    ZeroMem( *ppTDStruct, sizeof (TD_STRUCT) );

    return Status;
}


VOID InitTD (
    IN TD_STRUCT *ptrTD )
{
    //
    // Make TD ready.
    //
    SetTDLinkPtrValidorInvalid( ptrTD, FALSE );
}


EFI_STATUS CreateTD (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   **pptrTD )
{
    EFI_STATUS Status;

    //
    // create memory for TD_STRUCT, and align the memory.
    //
    Status = AllocateTDStruct( UhcDev, pptrTD );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    InitTD( *pptrTD );

    return EFI_SUCCESS;
}


EFI_STATUS GenSetupStageTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       DeviceSpeed,
    UINT8       *pDevReq,
    UINT8       RequestLen,
    TD_STRUCT   **ppTD )
{
    TD_STRUCT  *pTDStruct;
    EFI_STATUS Status;

    Status = CreateTD( UhcDev, &pTDStruct );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    SetTDLinkPtr( pTDStruct, NULL );

    //
    // Depth first fashion
    //
    SetTDLinkPtrDepthorBreadth( pTDStruct, TRUE );

    //SetTDLinkPtrQHorTDSelect(pTDStruct,FALSE) ;

    //
    // initialize as the last TD in the QH context,
    // this field will be updated in the TD linkage process.
    //
    SetTDLinkPtrValidorInvalid( pTDStruct, FALSE );

    //
    // Disable Short Packet Detection by default
    //
    EnableorDisableTDShortPacket( pTDStruct, FALSE );

    //
    // Max error counter is 3, retry 3 times when error encountered.
    //
    SetTDControlErrorCounter( pTDStruct, 3 );

    //
    // set device speed attribute
    // (TRUE - Slow Device; FALSE - Full Speed Device)
    //
    switch (DeviceSpeed)
    {
    case USB_SLOW_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( pTDStruct, TRUE );
        break;

    case USB_FULL_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( pTDStruct, FALSE );
        break;
    }

    //
    // Non isochronous transfer TD
    //
    SetTDControlIsochronousorNot( pTDStruct, FALSE );

    //
    // Interrupt On Complete bit be set to zero,
    // Disable IOC interrupt.
    //
    SetorClearTDControlIOC( pTDStruct, FALSE );

    //
    // Set TD Active bit
    //
    SetTDStatusActiveorInactive( pTDStruct, TRUE );

    SetTDTokenMaxLength( pTDStruct, RequestLen );

    SetTDTokenDataToggle0( pTDStruct );

    SetTDTokenEndPoint( pTDStruct, Endpoint );

    SetTDTokenDeviceAddress( pTDStruct, DevAddr );

    SetTDTokenPacketID( pTDStruct, SETUP_PACKET_ID );

    pTDStruct->pTDBuffer = (UINT8 *) pDevReq;
    pTDStruct->TDBufferLength = RequestLen;
    SetTDDataBuffer( pTDStruct );

    *ppTD = pTDStruct;

    return EFI_SUCCESS;
}


EFI_STATUS GenDataTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       *pData,
    UINT16      Len,
    UINT8       PktID,
    UINT8       Toggle,
    UINT8       DeviceSpeed,
    TD_STRUCT   **ppTD )
{
    TD_STRUCT  *pTDStruct;
    EFI_STATUS Status;

    Status = CreateTD( UhcDev, &pTDStruct );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    SetTDLinkPtr( pTDStruct, NULL );

    //
    // Depth first fashion
    //
    SetTDLinkPtrDepthorBreadth( pTDStruct, TRUE );

    //
    // Link pointer pointing to TD struct
    //
    SetTDLinkPtrQHorTDSelect( pTDStruct, FALSE );

    //
    // initialize as the last TD in the QH context,
    // this field will be updated in the TD linkage process.
    //
    SetTDLinkPtrValidorInvalid( pTDStruct, FALSE );

    //
    // Disable short packet detect
    //
    EnableorDisableTDShortPacket( pTDStruct, FALSE );
    //
    // Max error counter is 3
    //
    SetTDControlErrorCounter( pTDStruct, 3 );

    //
    // set device speed attribute
    // (TRUE - Slow Device; FALSE - Full Speed Device)
    //
    switch (DeviceSpeed)
    {
    case USB_SLOW_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( pTDStruct, TRUE );
        break;

    case USB_FULL_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( pTDStruct, FALSE );
        break;
    }
    //
    // Non isochronous transfer TD
    //
    SetTDControlIsochronousorNot( pTDStruct, FALSE );

    //
    // Disable Interrupt On Complete
    // Disable IOC interrupt.
    //
    SetorClearTDControlIOC( pTDStruct, FALSE );

    //
    // Set Active bit
    //
    SetTDStatusActiveorInactive( pTDStruct, TRUE );

    SetTDTokenMaxLength( pTDStruct, Len );

    if (Toggle) {
        SetTDTokenDataToggle1( pTDStruct );
    }
    else {
        SetTDTokenDataToggle0( pTDStruct );
    }

    SetTDTokenEndPoint( pTDStruct, Endpoint );

    SetTDTokenDeviceAddress( pTDStruct, DevAddr );

    SetTDTokenPacketID( pTDStruct, PktID );

    pTDStruct->pTDBuffer = (UINT8 *) pData;
    pTDStruct->TDBufferLength = Len;
    SetTDDataBuffer( pTDStruct );

    *ppTD = pTDStruct;

    return EFI_SUCCESS;
}


EFI_STATUS CreateStatusTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       PktID,
    UINT8       DeviceSpeed,
    TD_STRUCT   **ppTD )
{
    TD_STRUCT  *ptrTDStruct;
    EFI_STATUS Status;

    Status = CreateTD( UhcDev, &ptrTDStruct );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    SetTDLinkPtr( ptrTDStruct, NULL );

    // Depth first fashion
    SetTDLinkPtrDepthorBreadth( ptrTDStruct, TRUE );

    //SetTDLinkPtrQHorTDSelect(pTDStruct,FALSE) ;

    //
    // initialize as the last TD in the QH context,
    // this field will be updated in the TD linkage process.
    //
    SetTDLinkPtrValidorInvalid( ptrTDStruct, FALSE );

    //
    // Disable short packet detect
    //
    EnableorDisableTDShortPacket( ptrTDStruct, FALSE );

    //
    // Max error counter is 3
    //
    SetTDControlErrorCounter( ptrTDStruct, 3 );

    //
    // set device speed attribute
    // (TRUE - Slow Device; FALSE - Full Speed Device)
    //
    switch (DeviceSpeed)
    {
    case USB_SLOW_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( ptrTDStruct, TRUE );
        break;

    case USB_FULL_SPEED_DEVICE:
        SetTDLoworFullSpeedDevice( ptrTDStruct, FALSE );
        break;
    }

    //
    // Non isochronous transfer TD
    //
    SetTDControlIsochronousorNot( ptrTDStruct, FALSE );

    //
    // Disable Interrupt On Complete
    // Disable IOC interrupt.
    //
    SetorClearTDControlIOC( ptrTDStruct, FALSE );

    //
    // Set TD Active bit
    //
    SetTDStatusActiveorInactive( ptrTDStruct, TRUE );

    SetTDTokenMaxLength( ptrTDStruct, 0 );

    SetTDTokenDataToggle1( ptrTDStruct );

    SetTDTokenEndPoint( ptrTDStruct, Endpoint );

    SetTDTokenDeviceAddress( ptrTDStruct, DevAddr );

    SetTDTokenPacketID( ptrTDStruct, PktID );

    ptrTDStruct->pTDBuffer = NULL;
    ptrTDStruct->TDBufferLength = 0;
    SetTDDataBuffer( ptrTDStruct );

    *ppTD = ptrTDStruct;

    return EFI_SUCCESS;
}


VOID SetTDLinkPtrValidorInvalid (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bValid )
{
    //
    // Valid means the link pointer is valid,
    // else, it's invalid.
    //
    ptrTDStruct->TDData.TDLinkPtrTerminate = (bValid ? 0 : 1);
}


VOID SetTDLinkPtrQHorTDSelect (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bQH )
{
    //
    // Indicate whether the Link Pointer pointing to a QH or TD
    //
    ptrTDStruct->TDData.TDLinkPtrQSelect = (bQH ? 1 : 0);
}


VOID SetTDLinkPtrDepthorBreadth (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bDepth )
{
    //
    // If TRUE, indicating the host controller should process in
    // depth first fashion, else, the host controller should process
    // in breadth first fashion
    //
    ptrTDStruct->TDData.TDLinkPtrDepthSelect = (bDepth ? 1 : 0);
}


VOID SetTDLinkPtr (
    IN TD_STRUCT *ptrTDStruct,
    IN VOID      *ptrNext )
{
    //
    // Set TD Link Pointer. Since QH,TD align on 16-byte boundaries,
    // only the highest 28 bits are valid. (if take 32bit address as an example)
    //
    ptrTDStruct->TDData.TDLinkPtr = (UINT32) (UINTN) ptrNext >> 4;
}


VOID *GetTDLinkPtr (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Get TD Link Pointer. Restore it back to 32bit
    // (if take 32bit address as an example)
    //
    return ( (VOID *) (UINTN) ( (ptrTDStruct->TDData.TDLinkPtr) << 4 ) );
}


BOOLEAN IsTDLinkPtrQHOrTD (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Get the information about whether the Link Pointer field pointing to
    // a QH or a TD.
    //
    return (BOOLEAN) (ptrTDStruct->TDData.TDLinkPtrQSelect);
}


VOID EnableorDisableTDShortPacket (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bEnable )
{
    //
    // TRUE means enable short packet detection mechanism.
    //
    ptrTDStruct->TDData.TDStatusSPD = (bEnable ? 1 : 0);
}


VOID SetTDControlErrorCounter (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT8     nMaxErrors )
{
    //
    // valid value of nMaxErrors is 0,1,2,3
    //
    if (nMaxErrors > 3) {
        nMaxErrors = 3;
    }
    ptrTDStruct->TDData.TDStatusErr = nMaxErrors;
}


VOID SetTDLoworFullSpeedDevice (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bLowSpeedDevice )
{
    //
    // TRUE means the TD is targeting at a Low-speed device
    //
    ptrTDStruct->TDData.TDStatusLS = (bLowSpeedDevice ? 1 : 0);
}


VOID SetTDControlIsochronousorNot (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bIsochronous )
{
    //
    // TRUE means the TD belongs to Isochronous transfer type.
    //
    ptrTDStruct->TDData.TDStatusIOS = (bIsochronous ? 1 : 0);
}


VOID SetorClearTDControlIOC (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bSet )
{
    //
    // If this bit is set, it indicates that the host controller should issue
    // an interrupt on completion of the frame in which this TD is executed.
    //
    ptrTDStruct->TDData.TDStatusIOC = bSet ? 1 : 0;
}


VOID SetTDStatusActiveorInactive (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bActive )
{
    //
    // If this bit is set, it indicates that the TD is active and can be
    // executed.
    //
    if (bActive) {
        ptrTDStruct->TDData.TDStatus |= 0x80;
    }
    else {
        ptrTDStruct->TDData.TDStatus &= 0x7f;
    }
}


UINT16 SetTDTokenMaxLength (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT16    nMaxLen )
{
    //
    // Specifies the maximum number of data bytes allowed for the transfer.
    // the legal value extent is 0 ~ 0x500.
    //
    if (nMaxLen > 0x500) {
        nMaxLen = 0x500;
    }
    ptrTDStruct->TDData.TDTokenMaxLen = nMaxLen - 1;

    return nMaxLen;
}


VOID SetTDTokenDataToggle1 (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Set the data toggle bit to DATA1
    //
    ptrTDStruct->TDData.TDTokenDataToggle = 1;
}


VOID SetTDTokenDataToggle0 (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Set the data toggle bit to DATA0
    //
    ptrTDStruct->TDData.TDTokenDataToggle = 0;
}


VOID SetTDTokenEndPoint (
    IN TD_STRUCT *ptrTDStruct,
    IN UINTN     nEndPoint )
{
    //
    // Set EndPoint Number the TD is targeting at.
    //
    ptrTDStruct->TDData.TDTokenEndPt = (UINT8) nEndPoint;
}


VOID SetTDTokenDeviceAddress (
    IN TD_STRUCT *ptrTDStruct,
    IN UINTN     nDevAddr )
{
    //
    // Set Device Address the TD is targeting at.
    //
    ptrTDStruct->TDData.TDTokenDevAddr = (UINT8) nDevAddr;
}


VOID SetTDTokenPacketID (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT8     nPID )
{
    //
    // Set the Packet Identification to be used for this transaction.
    //
    ptrTDStruct->TDData.TDTokenPID = nPID;
}


VOID SetTDDataBuffer (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Set the beginning address of the data buffer that will be used
    // during the transaction.
    //
    ptrTDStruct->TDData.TDBufferPtr = (UINT32) (UINTN) (ptrTDStruct->
                                                            pTDBuffer);
}


BOOLEAN IsTDStatusActive (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether the TD is active.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x80);
}


BOOLEAN IsTDStatusStalled (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether the device/endpoint addressed by this TD is stalled.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x40);
}


BOOLEAN IsTDStatusBufferError (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether Data Buffer Error is happened.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x20);
}


BOOLEAN IsTDStatusBabbleError (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether Babble Error is happened.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x10);
}


BOOLEAN IsTDStatusNAKReceived (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether NAK is received.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x08);
}


BOOLEAN IsTDStatusCRCTimeOutError (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether CRC/Time Out Error is encountered.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x04);
}


BOOLEAN IsTDStatusBitStuffError (
    IN TD_STRUCT *ptrTDStruct )
{
    UINT8 TDStatus;

    //
    // Detect whether Bitstuff Error is received.
    //
    TDStatus = (UINT8) (ptrTDStruct->TDData.TDStatus);
    return (BOOLEAN) (TDStatus & 0x02);
}


UINT16 GetTDStatusActualLength (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Retrieve the actual number of bytes that were tansferred.
    // the value is encoded as n-1. so return the decoded value.
    //
    return (UINT16) ( (ptrTDStruct->TDData.TDStatusActualLength) + 1 );
}


BOOLEAN GetTDLinkPtrValidorInvalid (
    IN TD_STRUCT *ptrTDStruct )
{
    //
    // Retrieve the information of whether the Link Pointer field
    // is valid or not.
    //
    if (ptrTDStruct->TDData.TDLinkPtrTerminate) {
        return FALSE;
    }
    else {
        return TRUE;
    }

}


UINTN CountTDsNumber (
    IN TD_STRUCT *ptrFirstTD )
{
    UINTN     Number;
    TD_STRUCT *ptr;

    //
    // Count the queued TDs number.
    //
    Number = 0;
    ptr = ptrFirstTD;
    while (ptr) {
        ptr = (TD_STRUCT *) ptr->ptrNextTD;
        Number++;
    }

    return Number;
}


VOID LinkTDToQH (
    IN QH_STRUCT *ptrQH,
    IN TD_STRUCT *ptrTD )
{
    if (ptrQH == NULL || ptrTD == NULL) {
        return;
    }
    //
    //  Validate QH Vertical Ptr field
    //
    SetQHVerticalValidorInvalid( ptrQH, TRUE );

    //
    //  Vertical Ptr pointing to TD structure
    //
    SetQHVerticalQHorTDSelect( ptrQH, FALSE );

    SetQHVerticalLinkPtr( ptrQH, (VOID *) ptrTD );

    ptrQH->ptrDown = (VOID *) ptrTD;
}


VOID LinkTDToTD (
    IN TD_STRUCT *ptrPreTD,
    IN TD_STRUCT *ptrTD )
{
    if (ptrPreTD == NULL || ptrTD == NULL) {
        return;
    }
    //
    // Depth first fashion
    //
    SetTDLinkPtrDepthorBreadth( ptrPreTD, TRUE );

    //
    // Link pointer pointing to TD struct
    //
    SetTDLinkPtrQHorTDSelect( ptrPreTD, FALSE );

    //
    // Validate the link pointer valid bit
    //
    SetTDLinkPtrValidorInvalid( ptrPreTD, TRUE );

    SetTDLinkPtr( ptrPreTD, ptrTD );

    ptrPreTD->ptrNextTD = (VOID *) ptrTD;

    ptrTD->ptrNextTD = NULL;
}


//
// Transfer Schedule related Helper Functions
//
VOID SetorClearCurFrameListTerminate (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN BOOLEAN         bSet )
{
    //
    // If TRUE, empty the frame. If FALSE, indicate the Pointer field is valid.
    //
    pCurEntry->FrameListPtrTerminate = (bSet ? 1 : 0);
}


VOID SetCurFrameListQHorTD (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN BOOLEAN         bQH )
{
    //
    // This bit indicates to the hardware whether the item referenced by the
    // link pointer is a TD or a QH.
    //
    pCurEntry->FrameListPtrQSelect = (bQH ? 1 : 0);
}


BOOLEAN IsCurFrameListQHorTD (
    IN FRAMELIST_ENTRY *pCurEntry )
{
    //
    //TRUE is QH
    //FALSE is TD
    //
    return (BOOLEAN) (pCurEntry->FrameListPtrQSelect);
}


BOOLEAN GetCurFrameListTerminate (
    IN FRAMELIST_ENTRY *pCurEntry )
{
    //
    // TRUE means the frame is empty,
    // FALSE means the link pointer field is valid.
    //
    return (BOOLEAN) (pCurEntry->FrameListPtrTerminate);
}


VOID SetCurFrameListPointer (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN UINT8           *ptr )
{
    //
    // Set the pointer field of the frame.
    //
    pCurEntry->FrameListPtr = (UINT32) (UINTN) ptr >> 4;
}


VOID *GetCurFrameListPointer (
    IN FRAMELIST_ENTRY *pCurEntry )
{
    VOID *ptr;

    //
    // Get the link pointer of the frame.
    //
    ptr = (VOID *) (UINTN) ( (pCurEntry->FrameListPtr) << 4 );
    return ptr;
}


VOID LinkQHToFrameList (
    IN FRAMELIST_ENTRY *pEntry,
    IN UINT16          FrameListIndex,
    IN QH_STRUCT       *ptrQH )
{
    FRAMELIST_ENTRY *pCurFrame;
    QH_STRUCT       *TempQH, *NextTempQH;
    TD_STRUCT       *TempTD;
    BOOLEAN         LINK;

    //
    // Get frame list entry that the link process will begin from.
    //
    pCurFrame = pEntry + FrameListIndex;

    //
    // if current frame is empty
    // then link the specified QH directly to the Frame List.
    //
    if ( GetCurFrameListTerminate( pCurFrame ) ) {

        //
        // Link new QH to the frame list entry.
        //
        SetCurFrameListQHorTD( pCurFrame, TRUE );

        SetCurFrameListPointer( pCurFrame, (UINT8 *) ptrQH );

        //
        // clear T bit in the Frame List, means that the frame list
        // entry is no longer empty.
        //
        SetorClearCurFrameListTerminate( pCurFrame, FALSE );

        return;

    }
    else {
        //
        // current frame list has link pointer
        //
        if ( !IsCurFrameListQHorTD( pCurFrame ) ) {
            //
            //  a TD is linked to the framelist entry
            //
            TempTD = (TD_STRUCT *) GetCurFrameListPointer( pCurFrame );

            while ( GetTDLinkPtrValidorInvalid( TempTD ) ) {

                if ( IsTDLinkPtrQHOrTD( TempTD ) ) { // QH linked next to the TD
                    break;
                }

                TempTD = (TD_STRUCT *) GetTDLinkPtr( TempTD );
            }

            //
            // either no ptr linked next to the TD or QH is linked next to the TD
            //
            if ( !GetTDLinkPtrValidorInvalid( TempTD ) ) {

                //
                // no ptr linked next to the TD
                //
                TempTD->ptrNextQH = ptrQH;
                SetTDLinkPtrQHorTDSelect( TempTD, TRUE );
                SetTDLinkPtr( TempTD, ptrQH );
                SetTDLinkPtrValidorInvalid( TempTD, TRUE );
                //ptrQH->ptrNext = NULL;

                return;

            }
            else {
                //
                //  QH is linked next to the TD
                //
                TempQH = (QH_STRUCT *) GetTDLinkPtr( TempTD );
            }

        }
        else {
            //
            // a QH is linked to the framelist entry
            //
            TempQH = (QH_STRUCT *) GetCurFrameListPointer( pCurFrame );
        }

        //
        // Set up Flag
        //
        LINK = TRUE;

        //
        // aVOID the same qh repeated linking in one frame entry
        //
        if (TempQH == ptrQH) {
            LINK = FALSE;
        }
        //
        // if current QH has next QH connected
        //
        while ( GetQHHorizontalValidorInvalid( TempQH ) ) {

            //
            // Get next QH pointer
            //
            NextTempQH = (QH_STRUCT *) GetQHHorizontalLinkPtr( TempQH );

            //
            // Bulk transfer qh may be self-linked,
            // so, the code below is to aVOID dead-loop when meeting self-linked qh
            //
            if (NextTempQH == TempQH) {
                LINK = FALSE;
                break;
            }

            TempQH = NextTempQH;

            //
            // aVOID the same qh repeated linking in one frame entry
            //
            if (TempQH == ptrQH) {
                LINK = FALSE;
            }
        }

        if (LINK) {
            // Link
            TempQH->ptrNext = ptrQH;
            SetQHHorizontalQHorTDSelect( TempQH, TRUE );
            SetQHHorizontalLinkPtr( TempQH, ptrQH );
            SetQHHorizontalValidorInvalid( TempQH, TRUE );
            //ptrQH->ptrNext = NULL;
        }

        return;

    }

}


EFI_STATUS ExecuteControlTransfer (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrTD,
    UINT32      wIndex,
    UINTN       *ActualLen,
    UINTN       TimeOut,
    UINT32      *TransferResult )
{
    UINT32 StatusAddr;
    UINT32 FrameListBaseReg;
    UINTN  ErrTDPos;
    UINTN  Delay;
    CHAR8  *ErrorStr;

    //
    // Retrieve the Registers' address
    //
    StatusAddr = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameListBaseReg = UhcDev->UsbHostControllerBaseAddress + USBFLBASEADD;

    ErrTDPos = 0;
    *TransferResult = EFI_USB_NOERROR;
    *ActualLen = 0;
    Delay = (TimeOut * STALL_1_MILLI_SECOND / 50) + 1;

    do {

        CheckTDsResults( ptrTD, TransferResult, &ErrTDPos, ActualLen );

        //
        // TD is inactive, means the control transfer is end.
        //
        if ( (*TransferResult & EFI_USB_ERR_NOTEXECUTE) !=
            EFI_USB_ERR_NOTEXECUTE ) {
            break;
        }
        UhcDev->StallPpi->Stall( UhcDev->PeiServices, UhcDev->StallPpi, 50 );

    } while (Delay--);


    if (*TransferResult != EFI_USB_NOERROR) {
        switch (*TransferResult)
        {
        case EFI_USB_ERR_NOTEXECUTE:  ErrorStr = gUsbErrorStrings[0];
            break;

        case EFI_USB_ERR_STALL:  ErrorStr = gUsbErrorStrings[1];
            break;

        case EFI_USB_ERR_BUFFER:  ErrorStr = gUsbErrorStrings[2];
            break;

        case EFI_USB_ERR_BABBLE:  ErrorStr = gUsbErrorStrings[3];
            break;

        case EFI_USB_ERR_NAK:  ErrorStr = gUsbErrorStrings[4];
            break;

        case EFI_USB_ERR_CRC:  ErrorStr = gUsbErrorStrings[5];
            break;

        case EFI_USB_ERR_TIMEOUT:  ErrorStr = gUsbErrorStrings[6];
            break;

        case EFI_USB_ERR_BITSTUFF:  ErrorStr = gUsbErrorStrings[7];
            break;

        case EFI_USB_ERR_SYSTEM:  ErrorStr = gUsbErrorStrings[8];
            break;
        }
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


EFI_STATUS ExecBulkTransfer (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrTD,
    UINT32      wIndex,
    UINTN       *ActualLen,
    UINT8       *DataToggle,
    UINTN       TimeOut,
    UINT32      *TransferResult )
{
    UINT32 StatusAddr;
    UINT32 FrameListBaseReg;
    UINTN  ErrTDPos;
    UINTN  ScrollNum;
    UINTN  Delay;

    StatusAddr = UhcDev->UsbHostControllerBaseAddress + USBSTS;
    FrameListBaseReg = UhcDev->UsbHostControllerBaseAddress + USBFLBASEADD;

    ErrTDPos = 0;
    *TransferResult = EFI_USB_NOERROR;
    *ActualLen = 0;

    Delay = (TimeOut * STALL_1_MILLI_SECOND / 50) + 1;

    do {

        CheckTDsResults( ptrTD, TransferResult, &ErrTDPos, ActualLen );
        //
        // TD is inactive, thus meaning bulk transfer's end.
        //
        if ( (*TransferResult & EFI_USB_ERR_NOTEXECUTE) !=
            EFI_USB_ERR_NOTEXECUTE ) {
            break;
        }
        UhcDev->StallPpi->Stall( UhcDev->PeiServices, UhcDev->StallPpi, 50 );

    } while (Delay--);

    //
    // has error
    //
    if (*TransferResult != EFI_USB_NOERROR) {

        //
        // scroll the Data Toggle back to the last success TD
        //
        ScrollNum = CountTDsNumber( ptrTD ) - ErrTDPos;
        if (ScrollNum % 2) {
            *DataToggle ^= 1;
        }

        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


VOID DeleteSingleQH (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   *ptrQH,
    UINT16      FrameListIndex,
    BOOLEAN     SearchOther )
{
    FRAMELIST_ENTRY *pCurFrame;
    UINTN     i;
    UINTN     BeginFrame;
    UINTN     EndFrame;
    QH_STRUCT *TempQH;
    TD_STRUCT *TempTD;
    VOID      *PtrPreQH = NULL;
    BOOLEAN   Found = FALSE;

    if (ptrQH == NULL) {
        return;
    }

    if (SearchOther) {
        BeginFrame = 0;
        EndFrame = 1024;
    }
    else {
        BeginFrame = FrameListIndex % 1024;
        EndFrame = (FrameListIndex + 1) % 1024;
    }

    for (i = BeginFrame; i < EndFrame; i++) {

        pCurFrame = UhcDev->FrameListEntry + i;

        if ( GetCurFrameListTerminate( pCurFrame ) ) {
            //
            // current frame list is empty,search next frame list entry
            //
            continue;
        }

        if ( !IsCurFrameListQHorTD( pCurFrame ) ) {
            //
            // TD linked to current framelist
            //
            TempTD = (TD_STRUCT *) GetCurFrameListPointer( pCurFrame );

            while ( GetTDLinkPtrValidorInvalid( TempTD ) ) {

                if ( IsTDLinkPtrQHOrTD( TempTD ) ) {
                    //
                    // QH linked next to the TD,break while()
                    //
                    break;
                }

                TempTD = (TD_STRUCT *) GetTDLinkPtr( TempTD );
            }

            if ( !GetTDLinkPtrValidorInvalid( TempTD ) ) {
                //
                // no QH linked next to the last TD,
                // search next frame list
                //
                continue;
            }

            //
            // a QH linked next to the last TD
            //
            TempQH = (QH_STRUCT *) GetTDLinkPtr( TempTD );

            PtrPreQH = TempTD;

        }
        else {
            //
            // a QH linked to current framelist
            //
            TempQH = (QH_STRUCT *) GetCurFrameListPointer( pCurFrame );

            PtrPreQH = NULL;
        }

        if (TempQH == ptrQH) {

            if (PtrPreQH) { // QH linked to a TD struct

                TempTD = (TD_STRUCT *) PtrPreQH;
                SetTDLinkPtrValidorInvalid( TempTD, FALSE );
                SetTDLinkPtr( TempTD, NULL );
                TempTD->ptrNextQH = NULL;

            }
            else { // QH linked directly to current framelist entry

                SetorClearCurFrameListTerminate( pCurFrame, TRUE );
                SetCurFrameListPointer( pCurFrame, NULL );
            }

            Found = TRUE;
            //
            // search next framelist entry
            //
            continue;
        }

        while (TempQH != ptrQH) {

            PtrPreQH = TempQH;

            //
            // Get next horizontal linked QH
            //
            TempQH = (QH_STRUCT *) GetQHHorizontalLinkPtr( TempQH );

            //
            // if has no valid QH linked,break the while()
            //
            if (TempQH == NULL) {
                break;
            }

            //
            // detect self-linked QH, then break the loop
            //
            if (TempQH == PtrPreQH) {
                break;
            }
        }

        if (TempQH != ptrQH) {
            //
            // search next frame list entry
            //
            continue;
        }

        //
        // TempQH == ptrQH,
        // Update the related QHs
        //
        if ( GetQHHorizontalValidorInvalid( ptrQH ) ) {
            //
            // if has QH connected after the deleted QH
            //
            //
            // get next QH of the deleted QH
            //
            TempQH = GetQHHorizontalLinkPtr( ptrQH );

            //----------------------------------------------
            //
            // Bulk transfer qh may be self link to itself to enhance performance
            // so, only if the qh is not self linked, can we link the next qh to
            // the previous qh
            //
            if (TempQH != ptrQH) {
                //
                // link the next QH to the previous QH
                //
                SetQHHorizontalLinkPtr( (QH_STRUCT *) PtrPreQH, TempQH );

                ( (QH_STRUCT *) PtrPreQH )->ptrNext = TempQH;
            }
            //----------------------------------------------

            //
            // if the qh is self linked, also need to update the previous qh,
            // the situation just the same as that there is no qh connected after
            // the deleted qh
            //
        }
        //
        // has no QH connected after the deleted QH,
        // or the deleted QH is self-linked,
        //
        //
        // NULL the previous QH's link ptr and set Terminate field.
        //
        SetQHHorizontalValidorInvalid( (QH_STRUCT *) PtrPreQH, FALSE );
        SetQHHorizontalLinkPtr( (QH_STRUCT *) PtrPreQH, NULL );
        ( (QH_STRUCT *) PtrPreQH )->ptrNext = NULL;

        Found = TRUE;
    }

    if (Found) {
        //
        // free memory once used by deleted QH
        //
        UhcFreePool( UhcDev, MEM_QH_TD_TYPE, (UINT8 *) ptrQH,
            sizeof(QH_STRUCT) );
    }

    return;
}


VOID DeleteQueuedTDs (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrFirstTD )
{
    TD_STRUCT *Tptr1, *Tptr2;

    Tptr1 = ptrFirstTD;
    //
    // Delete all the TDs in a queue.
    //
    while (Tptr1) {

        Tptr2 = Tptr1;

        if ( !GetTDLinkPtrValidorInvalid( Tptr2 ) ) {
            Tptr1 = NULL;
        }
        else {
            //
            // has more than one TD in the queue.
            //
            Tptr1 = GetTDLinkPtr( Tptr2 );
        }

        UhcFreePool( UhcDev, MEM_QH_TD_TYPE, (UINT8 *) Tptr2,
            sizeof(TD_STRUCT) );
    }

    return;
}


BOOLEAN CheckTDsResults (
    IN TD_STRUCT *ptrTD,
    OUT UINT32   *Result,
    OUT UINTN    *ErrTDPos,
    OUT UINTN    *ActualTransferSize )
{
    UINTN Len;

    *Result = EFI_USB_NOERROR;
    *ErrTDPos = 0;

    //
    // Init to zero.
    //
    *ActualTransferSize = 0;

    while (ptrTD) {

        if ( IsTDStatusActive( ptrTD ) ) {
            *Result |= EFI_USB_ERR_NOTEXECUTE;
        }

        if ( IsTDStatusStalled( ptrTD ) ) {
            *Result |= EFI_USB_ERR_STALL;
        }


        if ( IsTDStatusBufferError( ptrTD ) ) {
            *Result |= EFI_USB_ERR_BUFFER;
        }

        if ( IsTDStatusBabbleError( ptrTD ) ) {
            *Result |= EFI_USB_ERR_BABBLE;
        }

        if ( IsTDStatusNAKReceived( ptrTD ) ) {
            *Result |= EFI_USB_ERR_NAK;
        }

        if ( IsTDStatusCRCTimeOutError( ptrTD ) ) {
            *Result |= EFI_USB_ERR_TIMEOUT;
        }

        if ( IsTDStatusBitStuffError( ptrTD ) ) {
            *Result |= EFI_USB_ERR_BITSTUFF;
        }

        //
        // Accumulate actual transferred data length in each TD.
        //
        Len = GetTDStatusActualLength( ptrTD ) % 0x800;
        *ActualTransferSize += Len;

        //
        // if any error encountered, stop processing the left TDs.
        //
        if (*Result) {
            return FALSE;
        }

        ptrTD = (TD_STRUCT *) (ptrTD->ptrNextTD);
        //
        // Record the first Error TD's position in the queue,
        // this value is zero-based.
        //
        (*ErrTDPos)++;
    }

    return TRUE;
}


/*
   VOID
   SelfLinkBulkTransferQH (
   IN  QH_STRUCT *ptrQH
   )
   {
   if (ptrQH == NULL) {
    return;
   }

   //
   // Make the QH's horizontal link pointer pointing to itself.
   //
   ptrQH->ptrNext = ptrQH;
   SetQHHorizontalQHorTDSelect(ptrQH,TRUE);
   SetQHHorizontalLinkPtr(ptrQH,ptrQH);
   SetQHHorizontalValidorInvalid(ptrQH,TRUE);
   }
 */

EFI_STATUS CreateMemoryBlock (
    USB_UHC_DEV          *UhcDev,
    MEMORY_MANAGE_HEADER **MemoryHeader,
    UINTN                MemoryBlockSizeInPages )
{
    EFI_STATUS Status;
    EFI_PHYSICAL_ADDRESS TempPtr;
    UINTN      MemPages;
    UINT8      *Ptr;

    //
    // Memory Block uses MemoryBlockSizeInPages pages,
    // memory management header and bit array use 1 page
    //
    MemPages = MemoryBlockSizeInPages + 1;
    Status = ( **(UhcDev->PeiServices) ).AllocatePages(
        UhcDev->PeiServices,
        EfiBootServicesData,
        MemPages,
        &TempPtr
             );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Ptr = (UINT8 *) ( (UINTN) TempPtr );

    (*UhcDev->PeiServices)->SetMem(Ptr, MemPages * PAGESIZE, 0);
//    ZeroMem( Ptr, MemPages * PAGESIZE );

    *MemoryHeader = (MEMORY_MANAGE_HEADER *) Ptr;
    //
    // adjust Ptr pointer to the next empty memory
    //
    Ptr += sizeof(MEMORY_MANAGE_HEADER);
    //
    // Set Bit Array initial address
    //
    (*MemoryHeader)->BitArrayPtr = Ptr;

    (*MemoryHeader)->Next = NULL;

    //
    // Memory block initial address
    //
    Ptr = (UINT8 *) ( (UINTN) TempPtr );
    Ptr += PAGESIZE;
    (*MemoryHeader)->MemoryBlockPtr = Ptr;
    //
    // set Memory block size
    //
    (*MemoryHeader)->MemoryBlockSizeInBytes = MemoryBlockSizeInPages *
                                              PAGESIZE;
    //
    // each bit in Bit Array will manage 32byte memory in memory block
    //
    (*MemoryHeader)->BitArraySizeInBytes =
        ( (*MemoryHeader)->MemoryBlockSizeInBytes / 32 ) / 8;

    return EFI_SUCCESS;
}


EFI_STATUS InitializeMemoryManagement (
    USB_UHC_DEV *UhcDev )
{
    MEMORY_MANAGE_HEADER *MemoryHeader;
    EFI_STATUS Status;
    UINTN      MemPages;

    MemPages = NORMAL_MEMORY_BLOCK_UNIT_IN_PAGES;
    Status = CreateMemoryBlock( UhcDev, &MemoryHeader, MemPages );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    UhcDev->Header1 = MemoryHeader;

    Status = CreateMemoryBlock( UhcDev, &MemoryHeader, MemPages );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    UhcDev->Header2 = MemoryHeader;

    return EFI_SUCCESS;
}


EFI_STATUS UhcAllocatePool (
    USB_UHC_DEV *UhcDev,
    UINT8       Type,
    UINT8       **Pool,
    UINTN       AllocSize )
{
    MEMORY_MANAGE_HEADER *MemoryHeader;
    MEMORY_MANAGE_HEADER *TempHeaderPtr;
    MEMORY_MANAGE_HEADER *NewMemoryHeader;
    UINTN      RealAllocSize;
    UINTN      MemoryBlockSizeInPages;
    EFI_STATUS Status;

    *Pool = NULL;

    switch (Type)
    {
    case MEM_QH_TD_TYPE:
        MemoryHeader = UhcDev->Header1;
        break;

    case MEM_DATA_BUFFER_TYPE:
        MemoryHeader = UhcDev->Header2;
        break;

    default:
        return EFI_INVALID_PARAMETER;
    }

    //
    // allocate unit is 32 byte (align on 32 byte)
    //
    if (AllocSize % 32) {
        RealAllocSize = (AllocSize / 32 + 1) * 32;
    }
    else {
        RealAllocSize = AllocSize;
    }
    //RealAllocSize = (AllocSize / 32 + 1) * 32;

    Status = EFI_NOT_FOUND;
    for (TempHeaderPtr = MemoryHeader; TempHeaderPtr != NULL;
         TempHeaderPtr = TempHeaderPtr->Next) {

        Status = AllocMemInMemoryBlock(
            TempHeaderPtr,
            Pool,
            RealAllocSize / 32
                 );
        if ( !EFI_ERROR( Status ) ) {
            return EFI_SUCCESS;
        }
    }

    //
    // There is no enough memory,
    // Create a new Memory Block
    //

    //
    // if pool size is larger than NORMAL_MEMORY_BLOCK_UNIT_IN_PAGES,
    // just allocate a large enough memory block.
    //
    if ( RealAllocSize > (NORMAL_MEMORY_BLOCK_UNIT_IN_PAGES * PAGESIZE) ) {
        MemoryBlockSizeInPages = RealAllocSize / PAGESIZE + 1;
    }
    else {
        MemoryBlockSizeInPages = NORMAL_MEMORY_BLOCK_UNIT_IN_PAGES;
    }
    Status = CreateMemoryBlock( UhcDev,
        &NewMemoryHeader,
        MemoryBlockSizeInPages );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }
    //
    // Link the new Memory Block to the Memory Header list
    //
    InsertMemoryHeaderToList( MemoryHeader, NewMemoryHeader );

    Status = AllocMemInMemoryBlock( NewMemoryHeader,
        Pool,
        RealAllocSize / 32
             );
    return Status;
}


EFI_STATUS AllocMemInMemoryBlock (
    MEMORY_MANAGE_HEADER *MemoryHeader,
    VOID                 **Pool,
    UINTN                NumberOfMemoryUnit )
{
    UINTN TempBytePos;
    UINTN FoundBytePos = 0;
    UINT8 i;
    UINT8 FoundBitPos = 0;
    UINT8 ByteValue;
    UINT8 BitValue;
    UINTN NumberOfZeros;
    UINTN Count;

    ByteValue = MemoryHeader->BitArrayPtr[0];
    NumberOfZeros = 0;
    i = 0;
    for (TempBytePos = 0; TempBytePos < MemoryHeader->BitArraySizeInBytes;
    ) {

        //
        // Pop out BitValue from a byte in TempBytePos.
        //
        BitValue = (UINT8) (ByteValue % 2);

        if (BitValue == 0) {
            //
            // Found a free bit, the NumberOfZeros only record the number
            // of those consecutive zeros
            //
            NumberOfZeros++;
            //
            // Found enough consecutive free space, break the loop
            //
            if (NumberOfZeros >= NumberOfMemoryUnit) {
                break;
            }
        }
        else {
            //
            // Encountering a '1', meant the bit is ocupied.
            //
            if (NumberOfZeros >= NumberOfMemoryUnit) {
                //
                // Found enough consecutive free space,break the loop
                //
                break;
            }
            else {
                //
                // the NumberOfZeros only record the number of those consecutive zeros,
                // so reset the NumberOfZeros to 0 when encountering '1' before finding
                // enough consecutive '0's
                //
                NumberOfZeros = 0;
                //
                // reset the (FoundBytePos,FoundBitPos) to the position of '1'
                //
                FoundBytePos = TempBytePos;
                FoundBitPos = i;
            }
        }

        //
        // right shift the byte
        //
        ByteValue /= 2;

        //
        // step forward a bit
        //
        i++;
        if (i == 8) {
            //
            // step forward a byte, getting the byte value,
            // and reset the bit pos.
            //
            TempBytePos += 1;
            ByteValue = MemoryHeader->BitArrayPtr[TempBytePos];
            i = 0;
        }
    }

    if (NumberOfZeros < NumberOfMemoryUnit) {
        return EFI_NOT_FOUND;
    }

    //
    // Found enough free space.
    //

    //
    // The values recorded in (FoundBytePos,FoundBitPos) have two conditions:
    //  1)(FoundBytePos,FoundBitPos) record the position
    //    of the last '1' before the consecutive '0's, it must
    //    be adjusted to the start position of the consecutive '0's.
    //  2)the start address of the consecutive '0's is just the start of
    //    the bitarray. so no need to adjust the values of
    //    (FoundBytePos,FoundBitPos).
    //
    if ( ( MemoryHeader->BitArrayPtr[0] & bit( 0 ) ) != 0 ) {
        FoundBitPos += 1;
    }

    //
    // Have the (FoundBytePos,FoundBitPos) make sense.
    //
    if (FoundBitPos > 7) {
        FoundBytePos += 1;
        FoundBitPos -= 8;
    }

    //
    // Set the memory as allocated
    //
    for (TempBytePos = FoundBytePos, i = FoundBitPos, Count = 0;
         Count < NumberOfMemoryUnit; Count++) {

        MemoryHeader->BitArrayPtr[TempBytePos] |= bit( i );
        i++;
        if (i == 8) {
            TempBytePos += 1;
            i = 0;
        }
    }

    *Pool = MemoryHeader->MemoryBlockPtr +
            (FoundBytePos * 8 + FoundBitPos) * 32;

    return EFI_SUCCESS;
}


VOID UhcFreePool (
    USB_UHC_DEV *UhcDev,
    UINT8       Type,
    UINT8       *Pool,
    UINTN       AllocSize )
{
    MEMORY_MANAGE_HEADER *MemoryHeader;
    MEMORY_MANAGE_HEADER *TempHeaderPtr;
    UINTN StartBytePos;
    UINTN i;
    UINT8 StartBitPos;
    UINT8 j;
    UINTN Count;
    UINTN RealAllocSize;

    //  UINTN                   MemPages;
    //  EFI_PHYSICAL_ADDRESS    TempPtr;

    switch (Type)
    {
    case MEM_QH_TD_TYPE:
        MemoryHeader = UhcDev->Header1;
        break;

    case MEM_DATA_BUFFER_TYPE:
        MemoryHeader = UhcDev->Header2;
        break;

    default:
        return;
    }

    //
    // allocate unit is 32 byte (align on 32 byte)
    //
    if (AllocSize % 32) {
        RealAllocSize = (AllocSize / 32 + 1) * 32;
    }
    else {
        RealAllocSize = AllocSize;
    }
    //RealAllocSize = (AllocSize / 32 + 1) * 32;

    for (TempHeaderPtr = MemoryHeader; TempHeaderPtr != NULL;
         TempHeaderPtr = TempHeaderPtr->Next) {

        if ( (Pool >= TempHeaderPtr->MemoryBlockPtr) &&
            ( (Pool + RealAllocSize) <= (TempHeaderPtr->MemoryBlockPtr +
                                         TempHeaderPtr->
                                             MemoryBlockSizeInBytes) ) ) {

            //
            // Pool is in the Memory Block area,
            // find the start byte and bit in the bit array
            //
            StartBytePos = ( (Pool -
                              TempHeaderPtr->MemoryBlockPtr) / 32 ) / 8;
            StartBitPos = (UINT8)
                          ( ( (Pool -
                               TempHeaderPtr->MemoryBlockPtr) / 32 ) % 8 );

            //
            // reset associated bits in bit arry
            //
            for (i = StartBytePos, j = StartBitPos, Count = 0;
                 Count < (RealAllocSize / 32); Count++) {

                TempHeaderPtr->BitArrayPtr[i] ^= (UINT8) ( bit( j ) );
                j++;
                if (j == 8) {
                    i += 1;
                    j = 0;
                }
            }
            //
            // break the loop
            //
            break;
        }
    }

}


VOID InsertMemoryHeaderToList (
    MEMORY_MANAGE_HEADER *MemoryHeader,
    MEMORY_MANAGE_HEADER *NewMemoryHeader )
{
    MEMORY_MANAGE_HEADER *TempHeaderPtr;

    for (TempHeaderPtr = MemoryHeader; TempHeaderPtr != NULL;
         TempHeaderPtr = TempHeaderPtr->Next) {
        if (TempHeaderPtr->Next == NULL) {
            TempHeaderPtr->Next = NewMemoryHeader;
            break;
        }
    }
}


BOOLEAN IsMemoryBlockEmptied (
    MEMORY_MANAGE_HEADER *MemoryHeaderPtr )
{
    UINTN i;

    for (i = 0; i < MemoryHeaderPtr->BitArraySizeInBytes; i++) {
        if (MemoryHeaderPtr->BitArrayPtr[i] != 0) {
            return FALSE;
        }
    }

    return TRUE;
}


VOID DelinkMemoryBlock (
    MEMORY_MANAGE_HEADER *FirstMemoryHeader,
    MEMORY_MANAGE_HEADER *FreeMemoryHeader )
{
    MEMORY_MANAGE_HEADER *TempHeaderPtr;

    if ( (FirstMemoryHeader == NULL) || (FreeMemoryHeader == NULL) ) {
        return;
    }
    for (TempHeaderPtr = FirstMemoryHeader; TempHeaderPtr != NULL;
         TempHeaderPtr = TempHeaderPtr->Next) {

        if (TempHeaderPtr->Next == FreeMemoryHeader) {
            //
            // Link the before and after
            //
            TempHeaderPtr->Next = FreeMemoryHeader->Next;
            break;
        }
    }
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
