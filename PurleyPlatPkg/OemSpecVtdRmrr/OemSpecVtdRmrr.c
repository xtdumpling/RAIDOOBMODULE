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
/** @file OemSpecVtdRmrr.c

    OemSpecVtdRmrr library implementation
*/

//----------------------------------------------------------------------
#pragma optimize( "", off )

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>

#include <Acpi/DMARemappingReportingTable.h>

#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/DmaRemap.h>
#include <Protocol/OemVtdRmrrProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/IioUds.h>

#include "HostMemoryReportProtocol.h"

#include "OemSpecVtdRmrr.h"

//----------------------------------------------------------------------

//
// Module global values
//
static EFI_GUID gEfiHostMemoryReportProtocolGuid = EFI_HOST_MEMORY_REPORT_PROTOCOL_GUID;

DMAR_RMRR           *gRmrr = NULL;
DEVICE_SCOPE        *gDevScope = NULL;
PCI_NODE            *gPciNode = NULL;

UINTN               gRmrrCount = 0;
UINTN               gDevScopeCount = 0;
UINTN               gPciNodeCount = 0;

//
// Function implementations
//
EFI_STATUS
OemSpecVtdRmrrInsertRmrr (
  IN  EFI_DMA_REMAP_PROTOCOL    *DmaRemap )
{
    EFI_STATUS          Status;
    UINTN               Index;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered  \n"));

    //
    // Insert RMRR table
    //
    for ( Index=0; Index < gRmrrCount; Index++ ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "Insert RMRR table..\n"));
        Status = DmaRemap->InsertDmaRemap (DmaRemap, RmrrType, &gRmrr[Index]);
        if (EFI_ERROR(Status)) {
            OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Index: %x  InsertDmaRemap Status: %r \n", Index, Status));
        }
    }
    //
    // Free the memory
    //
    if ( gRmrr ) {
        gBS->FreePool (gRmrr);
    }
    if ( gDevScope ) {
        gBS->FreePool (gDevScope);
    }
    if ( gPciNode ) {
        gBS->FreePool(gPciNode);
    }

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting  \n"));

    return EFI_SUCCESS;
}

EFI_STATUS
GetOemSpecDevicePath (
  IN  UINTN                     ControllerSegmentNumber,
  IN  UINTN                     ControllerBusNumber,
  IN  UINTN                     ControllerDeviceNumber,
  IN  UINTN                     ControllerFunctionNumber,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath )
{
    UINTN                       HandleCount;
    EFI_HANDLE                  *HandleBuffer;
    UINTN                       Index;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    UINTN                       SegmentNumber;
    UINTN                       BusNumber;
    UINTN                       DeviceNumber;
    UINTN                       FunctionNumber;
    EFI_STATUS                  Status;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered  \n"));

    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "ControllerSegmentNumber: %x \n", ControllerSegmentNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "ControllerBusNumber: %x \n", ControllerBusNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "ControllerDeviceNumber: %x \n", ControllerDeviceNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "ControllerFunctionNumber: %x \n", ControllerFunctionNumber));

    //
    // Locate all PciIo handles
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer );
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HandleCount: %X \n", HandleCount));

    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "LocateHandleBuffer gEfiPciIoProtocolGuid. Status: %r \n", Status));
        return Status;
    }

    for (Index = 0; Index < HandleCount; Index ++) {
        //
        // Get the instance of Protocol
        //
        Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo );
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "HandleProtocol gEfiPciIoProtocolGuid. Status: %r \n", Status));
            break;
        }

        //
        // Get PCI Io Location
        //
        Status = PciIo->GetLocation ( 
                            PciIo,
                            &SegmentNumber,
                            &BusNumber,
                            &DeviceNumber,
                            &FunctionNumber );
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "PciIo->GetLocation GetLocation. Status: %r \n", Status));
            break;
        }

        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "SegmentNumber: %x \n", SegmentNumber));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "BusNumber: %x \n", BusNumber));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "DeviceNumber: %x \n", DeviceNumber));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "FunctionNumber: %x \n", FunctionNumber));

        //
        // Get the PciIo handle by comparing Bus, Device and funtion numbers
        //
        if ( (SegmentNumber == ControllerSegmentNumber) && \
                (BusNumber == ControllerBusNumber) && \
                (DeviceNumber == ControllerDeviceNumber) && \
                (FunctionNumber == ControllerFunctionNumber) ) {
            //
            // Get the device path
            //
            Status = gBS->HandleProtocol (
                            HandleBuffer[Index],
                            &gEfiDevicePathProtocolGuid,
                            DevicePath ); 
            if ( EFI_ERROR(Status) ) {
                OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "HandleProtocol gEfiDevicePathProtocolGuid. Status: %r \n", Status));
            }
            break;
        }
    } // end of for loop

    //
    // Free the handle buffer
    //
    gBS->FreePool(HandleBuffer);

    if ( (Index == HandleCount) || EFI_ERROR(Status) ) {
        Status = EFI_NOT_FOUND;
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "Error in getting the Device path. Status: %r \n", Status));
        return Status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
GetBaseBusNumber (
  IN  UINTN    ControllerSegmentNumber,
  IN  UINTN    ControllerBusNumber,
  IN  UINT8    *BaseBusNumber )
{
    EFI_IIO_UDS_PROTOCOL    *IioUds;
    UINT8                   SocketNum;
    UINT8                   StackNum;
    EFI_STATUS              Status;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered  \n"));

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "ControllerBusNumber: %x  \n", ControllerBusNumber));

    //
    // Get the base bus number of the root bridge
    //
    Status = gBS->LocateProtocol(
                    &gEfiIioUdsProtocolGuid,
                    NULL,
                    &IioUds );
    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "LocateProtocol gEfiIioUdsProtocolGuid. Status: %r \n", Status));
        return Status;
    }

    //
     // Loop through all IIO stacks of all sockets that are present
     //
     for (SocketNum = 0; SocketNum < MAX_SOCKET; SocketNum++) {

         if (!IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].Valid) {
           continue;
         }

         OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "ControllerSegmentNumber: %x \n", ControllerSegmentNumber));
         OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].PcieSegment: %x \n", IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].PcieSegment));

         //
         // Verify Socket Segment number
         //
         if( ControllerSegmentNumber != IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].PcieSegment) {
             continue;
         }

         OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketFirstBus: %x \n", IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketFirstBus));
         OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketLastBus: %x \n", IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketLastBus));

         //
         // Verify Socket Bus numbers
         //
         if( (ControllerBusNumber < IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketFirstBus) || \
             (ControllerBusNumber > IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].SocketLastBus) ) {
             continue;
         }

         for (StackNum = 0; StackNum < MAX_IIO_STACK; StackNum++) {

             if (!((IioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNum].stackPresentBitmap) & (1 << StackNum))) {
                 continue;
             }

             OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase: %x \n", IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase));
             OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusLimit: %x \n", IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusLimit));

             //
             // Verify Stack Bus numbers
             //
             if( (ControllerBusNumber >= IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase) && \
                 (ControllerBusNumber <= IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusLimit) ) {
                 *BaseBusNumber = IioUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase;
                 OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "BaseBusNumber: %x  \n", *BaseBusNumber));
                 return EFI_SUCCESS;
             }
         }
    
    }

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting  \n"));

    return EFI_UNSUPPORTED;
}

EFI_STATUS
FillRmrrStructure (
  IN  EFI_HOST_MEMORY_REPORT_INFO   HostMemoryInfo )
{
    EFI_STATUS                      Status;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *Node = NULL;
    UINT8                           StartBusNumber = 0;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" Entering  \n"));

    //
    // Get the Base bus number: #HostMemorySegment: Segment number should come from HostMemoryInfo Structure
    //
    Status = GetBaseBusNumber (SEGMENT0, HostMemoryInfo.Bus, &StartBusNumber);
    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "GetBaseBusNumber. Status: %r \n", Status));
        return Status;
    }

    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "StartBusNumber: %X \n", StartBusNumber));

    //
    // Get Device path
    //
    Status = GetOemSpecDevicePath (
                SEGMENT0,               //#HostMemorySegment: Segment number should come from HostMemoryInfo Structure
                HostMemoryInfo.Bus,
                HostMemoryInfo.Device,
                HostMemoryInfo.Func,
                &DevicePath );
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "GetOemSpecDevicePath. Status: %r DevicePath: %lx \n", Status, DevicePath));
    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "GetOemSpecDevicePath. Status: %r \n", Status));
        return Status;
    }

    //
    // Fill RMRR Structure
    //
    gRmrr[gRmrrCount].Signature         = RMRR_SIGNATURE;
    gRmrr[gRmrrCount].SegmentNumber     = SEGMENT0; //#HostMemorySegment: Segment number should come from HostMemoryInfo Structure
    gRmrr[gRmrrCount].DeviceScopeNumber = 00;
    gRmrr[gRmrrCount].RsvdMemBase       = HostMemoryInfo.HostMemoryAddress;
    gRmrr[gRmrrCount].RsvdMemLimit      = (HostMemoryInfo.HostMemoryAddress+HostMemoryInfo.Size-1);
    gRmrr[gRmrrCount].DeviceScope       = &gDevScope[gDevScopeCount];
    gRmrr[gRmrrCount].DeviceScopeNumber++;

    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gRmrr[%d].Signature: %lX \n", gRmrrCount, gRmrr[gRmrrCount].Signature));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gRmrr[%d].SegmentNumber: %lX \n", gRmrrCount, gRmrr[gRmrrCount].SegmentNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gRmrr[%d].DeviceScopeNumber: %lX \n", gRmrrCount, gRmrr[gRmrrCount].DeviceScopeNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gRmrr[%d].RsvdMemBase: %lX \n", gRmrrCount, gRmrr[gRmrrCount].RsvdMemBase));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gRmrr[%d].RsvdMemLimit: %lX \n", gRmrrCount, gRmrr[gRmrrCount].RsvdMemLimit));

    //
    // Increment RMRR count
    //
    gRmrrCount++;

    //
    // Fill Device scope Structure
    //
    gDevScope[gDevScopeCount].DeviceType       = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
    gDevScope[gDevScopeCount].EnumerationID    = 00;
    gDevScope[gDevScopeCount].StartBusNumber    = StartBusNumber;
    gDevScope[gDevScopeCount].PciNode          = &gPciNode[gPciNodeCount];

    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gDevScope[%d].DeviceType: %X \n", gDevScopeCount, gDevScope[gDevScopeCount].DeviceType));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gDevScope[%d].EnumerationID: %X \n", gDevScopeCount, gDevScope[gDevScopeCount].EnumerationID));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gDevScope[%d].StartBusNumber: %X \n", gDevScopeCount, gDevScope[gDevScopeCount].StartBusNumber));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gDevScope[%d].PciNode: %X \n", gDevScopeCount, gDevScope[gDevScopeCount].PciNode));

    //
    // Increment Device Scope count
    //
    gDevScopeCount++;

    //
    // Parse device path and Fill Pci Nodes
    // First node contains ACPI device path. Skip the first node.
    //
    Node = NextDevicePathNode (DevicePath);

    while ( !IsDevicePathEnd (Node) ) {

        gPciNode[gPciNodeCount].Device          = ((PCI_DEVICE_PATH *)Node)->Device;
        gPciNode[gPciNodeCount].Function        = ((PCI_DEVICE_PATH *)Node)->Function;

        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gPciNode[%d].Device: %X \n", gPciNodeCount, gPciNode[gPciNodeCount].Device));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gPciNode[%d].Function: %X \n", gPciNodeCount, gPciNode[gPciNodeCount].Function));

        //
        // Increment Pci Node count
        //
        gPciNodeCount++;

        //
        // Next device path node
        //
        Node = NextDevicePathNode (Node);
    }

    //
    // Fill Pci End Node Structure
    //
    gPciNode[gPciNodeCount].Device    = (UINT8) -1;
    gPciNode[gPciNodeCount].Function  = (UINT8) -1;

    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gPciNode[%d].Device: %X \n", gPciNodeCount, gPciNode[gPciNodeCount].Device));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "gPciNode[%d].Function: %X \n", gPciNodeCount, gPciNode[gPciNodeCount].Function));

    //
    // Increment Pci Node count
    //
    gPciNodeCount++;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting  \n"));

    return EFI_SUCCESS;
}

EFI_STATUS
OemSpecVtdRmrrCollectLegacyInfo (
  VOID )
{
    EFI_STATUS                              Status;
    EFI_HOST_MEMORY_REPORT_INFO             HostMemoryInfo;
    UINT16                                  HandleCount;
    UINT16                                  Index;
    EFI_LEGACY_BIOS_PROTOCOL                *LegacyBios=NULL;
    BOOLEAN                                 ReturnSuccess;
    UINT8                                   BaseBusNumber=0;
    BOOLEAN                                 Int86Status;
    EFI_IA32_REGISTER_SET                   RegSet;
    UINTN                                   SizeinPages=0;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered  \n"));

    //
    // Locate gEfiLegacyBiosProtocolGuid protocol
    //
    Status = gBS->LocateProtocol (
                &gEfiLegacyBiosProtocolGuid,
                NULL,
                &LegacyBios );
    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "LocateProtocol gEfiLegacyBiosProtocolGuid. Status: %r \n", Status));
        return Status;
    }

    ZeroMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET));

    //
    // Get controller count
    //
    RegSet.H.AH = OEM_SPEC_FUNCTION_CODE;
    RegSet.H.AL = OemSpecGetControllerCount;
    RegSet.E.EDX = OEM_SPEC_FUNCTION_SIGNATURE;

    Int86Status = LegacyBios->Int86 (
                    LegacyBios, 
                    OEM_SPEC_INT_15H, 
                    &RegSet );
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "LegacyBios->Int86. OemSpecGetControllerCount Int86Status: %X \n", Int86Status));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.X.Flags.CF: %X \n", RegSet.X.Flags.CF));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.H.AH: %X \n", RegSet.H.AH));
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.X.CX: %X \n", RegSet.X.CX));

    if ( Int86Status || RegSet.X.Flags.CF ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "OEM SPEC Function OemSpecGetControllerCount Status: %r \n", EFI_UNSUPPORTED));
        return EFI_UNSUPPORTED;
    }

    //
    // Fill Handle Count
    //
    HandleCount = RegSet.X.CX;

    //
    // Allocate memory
    //
    gRmrr = AllocateZeroPool(HandleCount* sizeof (DMAR_RMRR));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gRmrr: %lx \n", gRmrr));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gRmrr! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    gDevScope = AllocateZeroPool(HandleCount*sizeof (DEVICE_SCOPE));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gDevScope: %lx \n", gDevScope));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gDevScope! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    gPciNode = AllocateZeroPool(HandleCount*21*sizeof (PCI_NODE));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gPciNode: %lx \n", gPciNode));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gPciNode! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < HandleCount; Index ++) {

        ZeroMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET));

        //
        // Get Memory Request Size and BDF information
        //
        RegSet.H.AH = OEM_SPEC_FUNCTION_CODE;
        RegSet.H.AL = OemSpecGetMemoryReqBDFInfo;
        RegSet.E.EDX = OEM_SPEC_FUNCTION_SIGNATURE;
        RegSet.X.CX = Index;

        Int86Status = LegacyBios->Int86 (
                        LegacyBios, 
                        OEM_SPEC_INT_15H, 
                        &RegSet );
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "LegacyBios->Int86. OemSpecGetMemoryReqBDFInfo Int86Status: %X \n", Int86Status));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.X.Flags.CF: %X \n", RegSet.X.Flags.CF));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.H.AH: %X \n", RegSet.H.AH));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.X.CX: %X \n", RegSet.X.CX));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.E.EBX: %X \n", RegSet.E.EBX));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.E.ESI: %X \n", RegSet.E.ESI));

        if ( Int86Status || RegSet.X.Flags.CF ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "OEM SPEC Function OemSpecGetMemoryReqBDFInfo Status: %r \n", EFI_DEVICE_ERROR));
            continue;
        }

        ZeroMem (&HostMemoryInfo, sizeof (HostMemoryInfo));

        HostMemoryInfo.Bus = (RegSet.X.CX >>8) & 0xFF;
        HostMemoryInfo.Device = (RegSet.X.CX >>3) & 0x1F;
        HostMemoryInfo.Func = RegSet.X.CX & 0x7;

        HostMemoryInfo.Size =  (RegSet.E.EBX << 4)+ RegSet.E.ESI;
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HostMemoryInfo.Size: %lX \n", HostMemoryInfo.Size));

        //
        // Allocate 32 bit memory for HOST
        //
        SizeinPages = EFI_SIZE_TO_PAGES(HostMemoryInfo.Size);
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "SizeinPages: %lX \n", SizeinPages));

        HostMemoryInfo.HostMemoryAddress = 0xFFFFFFFF;
        Status = gBS->AllocatePages (
                        AllocateMaxAddress,
                        EfiRuntimeServicesData,
                        SizeinPages, 
                        &HostMemoryInfo.HostMemoryAddress);
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "gBS->AllocatePages  Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for Host! \n"));
            continue;
        }

        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HostMemoryInfo.HostMemoryAddress: %lX \n", HostMemoryInfo.HostMemoryAddress));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HostMemoryInfo.Bus: %X \n", HostMemoryInfo.Bus));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HostMemoryInfo.Device: %X \n", HostMemoryInfo.Device));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HostMemoryInfo.Func: %X \n", HostMemoryInfo.Func));

        ZeroMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET));

        //
        // Set Memory
        //
        RegSet.H.AH = OEM_SPEC_FUNCTION_CODE;
        RegSet.H.AL = OemSpecSetMemory;
        RegSet.E.EDX = OEM_SPEC_FUNCTION_SIGNATURE;
        RegSet.X.CX = Index;

        RegSet.E.ESI =  (UINT32)HostMemoryInfo.HostMemoryAddress;
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.E.ESI: %X \n", RegSet.E.ESI));

        Int86Status = LegacyBios->Int86 (
                        LegacyBios, 
                        OEM_SPEC_INT_15H, 
                        &RegSet );
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "LegacyBios->Int86. OemSpecSetMemory Int86Status: %X \n", Int86Status));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.X.Flags.CF: %X \n", RegSet.X.Flags.CF));
        OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "RegSet.H.AH: %X \n", RegSet.H.AH));

        if ( Int86Status || RegSet.X.Flags.CF ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "OEM SPEC Function OemSpecSetMemory Status: %r \n", EFI_DEVICE_ERROR));
            continue;
        }

        //
        // Fill the RMRR structure
        //
        Status = FillRmrrStructure ( HostMemoryInfo );
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "FillRmrrStructure  Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "FillRmrrStructure  Status: %r \n", Status));
            continue;
        }

        //
        // If control comes here, return success
        //
        ReturnSuccess = TRUE;

    }

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "ReturnSuccess: %x \n", ReturnSuccess));

    if ( ReturnSuccess ) {
        Status = EFI_SUCCESS;
    } else {
        //
        // Free the memory
        //
        if ( gRmrr ) {
            gBS->FreePool (gRmrr);
        }
        if ( gDevScope ) {
            gBS->FreePool (gDevScope);
        }
        if ( gPciNode ) {
            gBS->FreePool(gPciNode);
        }
        Status = EFI_UNSUPPORTED;
    }

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting  Status: %r \n", Status));

    return Status;
}

EFI_STATUS
OemSpecVtdRmrrCollectUefiInfo (
  VOID )
{
    EFI_STATUS                              Status;
    EFI_HOST_MEMORY_REPORT_PROTOCOL         *HostMemoryReport = NULL;
    EFI_HOST_MEMORY_REPORT_INFO             HostMemoryInfo;
    UINTN                                   HandleCount;
    EFI_HANDLE                              *HandleBuffer;
    UINTN                                   Index;
    BOOLEAN                                 ReturnSuccess = FALSE;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered  \n"));

    //
    // Locate all instances of HostMemoryReport protocol
    //
    Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiHostMemoryReportProtocolGuid,
                 NULL,
                 &HandleCount,
                 &HandleBuffer );
    OEM_SPEC_RMMR_DEBUG((EFI_D_INFO, "HandleCount: %X \n", HandleCount));

    if ( EFI_ERROR(Status) ) {
        OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "LocateHandleBuffer gEfiHostMemoryReportProtocolGuid. Status: %r \n", Status));
        return Status;
    }

    //
    // Allocate memory
    //
    gRmrr = AllocateZeroPool(HandleCount* sizeof (DMAR_RMRR));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gRmrr: %lx \n", gRmrr));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gRmrr! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    gDevScope = AllocateZeroPool(HandleCount*sizeof (DEVICE_SCOPE));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gDevScope: %lx \n", gDevScope));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gDevScope! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    gPciNode = AllocateZeroPool(HandleCount*21*sizeof (PCI_NODE));
    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "AllocateZeroPool  gPciNode: %lx \n", gPciNode));
    if (gRmrr == NULL) {
        OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gPciNode! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < HandleCount; Index ++) {
        //
        // Get the instance of Protocol
        //
        Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiHostMemoryReportProtocolGuid,
                    &HostMemoryReport );
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "HandleProtocol gEfiHostMemoryReportProtocolGuid. Status: %r \n", Status));
            continue;
        }

        ZeroMem (&HostMemoryInfo, sizeof (HostMemoryInfo));

        //
        // Get Host Information
        //
        Status = HostMemoryReport->GetInfo (
                    HostMemoryReport,
                    &HostMemoryInfo );
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG((EFI_D_ERROR, "HostMemoryReport->GetInfo. Status: %r \n", Status));
            continue;
        }
	
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "HostMemoryInfo.HostMemoryAddress: %lX \n", HostMemoryInfo.HostMemoryAddress));
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "HostMemoryInfo.Size: %lX \n", HostMemoryInfo.Size));
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "HostMemoryInfo.Bus: %X \n", HostMemoryInfo.Bus));
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "HostMemoryInfo.Device: %X \n", HostMemoryInfo.Device));
        OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "HostMemoryInfo.Func: %X \n", HostMemoryInfo.Func));

        //
        // Fill the RMRR structure
        //
        Status = FillRmrrStructure ( HostMemoryInfo );
        if ( EFI_ERROR(Status) ) {
            OEM_SPEC_RMMR_DEBUG ((EFI_D_ERROR, "FillRmrrStructure  Status: %r \n", Status));
            continue;
        }

        //
        // If control comes here, return success
        //
        ReturnSuccess = TRUE;

    }

    //
    // Free the handle buffer
    //
    gBS->FreePool(HandleBuffer);

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, "ReturnSuccess: %x \n", ReturnSuccess));

    if ( ReturnSuccess ) {
        Status = EFI_SUCCESS;
    } else {
        //
        // Free the memory
        //
        if ( gRmrr ) {
            gBS->FreePool (gRmrr);
        }
        if ( gDevScope ) {
            gBS->FreePool (gDevScope);
        }
        if ( gPciNode ) {
            gBS->FreePool(gPciNode);
        }
        Status = EFI_UNSUPPORTED;
    }

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting  Status: %r \n", Status));

    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name : OemSpecVtdRmrr
//
// Description:
//  OemSpecVtdRmrr Hook
//
// Input: 
//  IN OEM_VTD_RMRR_FUNC_NUMBER FuncNumber     - Function number
//  IN VOID *Ptr - NULL - OemVtdRmrrInsertRmrr function
//               - Pointer to DmaRemap protocol - OemVtdRmrrInsertRmrr function
//
// Output:
//   EFI_STATUS
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END> 
EFI_STATUS
OemSpecVtdRmrr (
  IN OEM_VTD_RMRR_FUNC_NUMBER FuncNumber,
  IN VOID                     *Ptr )
{
    EFI_STATUS      Status;

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" entered   FuncNumber: %x \n", FuncNumber));

    switch ( FuncNumber ) {
        case OemVtdRmrrCollectInfo:
            Status = OemSpecVtdRmrrCollectUefiInfo();
            if ( EFI_ERROR(Status) ) {
                Status = OemSpecVtdRmrrCollectLegacyInfo();
            }
            break;

        case OemVtdRmrrInsertRmrr:
            Status = OemSpecVtdRmrrInsertRmrr(Ptr);
            break;

        default:
            Status = EFI_UNSUPPORTED;
            break;
    } // switch case

    OEM_SPEC_RMMR_DEBUG ((EFI_D_INFO, __FUNCTION__" exiting....... Status: %r \n", Status));

    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
