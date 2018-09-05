//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************

/** @file
  PciAccessCspLib library class
*/
#ifndef __AMI_PCI_ACCESS_CSP_BASE_LIB_H__
#define __AMI_PCI_ACCESS_CSP_BASE_LIB_H__

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Protocol/AcpiTable.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
//Keep MCFG table key and ACPI Table Protocol as global
//in case somebody will like to update MCFG table.
//NULL check MUST BE DONE BEFORE USING IT!!!!
extern UINTN           				gMcfgTableKey;
extern EFI_ACPI_TABLE_PROTOCOL     	*gAcpiTableProtocol;


//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspBeforeEnumeration
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeBeginEnumeration.
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspBeforeEnumeration (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspBeginBusAllocation
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeBeginBusAllocation.
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspBeginBusAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                   **RbIoProtocolBuffer,
    IN UINTN                                             RbCount );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspEndBusAllocation
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeEndBusAllocation
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspEndBusAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspBeginResourceAllocation
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeBeginResourceAllocation.
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspBeginResourceAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspAllocateResources
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeAllocateResources.
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspAllocateResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspSetResources
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeSetResources.
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspSetResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbNotifyCspEndResourceAllocation
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              when NotifyPhase function is called with phase
//              EfiPciHostBridgeEndResourceAllocation
//
// Input:       ResAllocProtocol   - Pointer to Host Bridge Resource
//                                   Allocation Protocol.
//              RbIoProtocolBuffer - Pointer to Root Bridge I/O Protocol.
//              RbCount            - Root Bridge counter.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbNotifyCspEndResourceAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspStartBusEnumeration
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              StartBusEnumeration function, it must prepare initial Bus
//              ACPI Resource
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//              RootBrgData  - Pointer to Root Bridge private structure data.
//              RootBrgIndex - Root Bridge index (0 Based).
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspStartBusEnumeration (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspSetBusNnumbers
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              SubmitBusNumbers function.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//              RootBrgData  - Pointer to Root Bridge private structure data.
//              RootBrgIndex - Root Bridge index (0 Based).
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspSetBusNnumbers (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspSubmitResources
//
// Description: This procedure will be invoked in PCI Host Bridge Protocol
//              SubmitResources function.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//              RootBrgData  - Pointer to Root Bridge private structure data.
//              RootBrgIndex - Root Bridge index (0 Based).
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspSubmitResources (
								IN PCI_HOST_BRG_DATA    *HostBrgData,
								IN PCI_ROOT_BRG_DATA    *RootBrgData,
								IN UINTN                RootBrgIndex );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspAdjustMemoryMmioOverlap
//
// Description: This procedure will be invoked during PCI bus enumeration,
//              it determines the PCI memory base address below 4GB whether
//              it is overlapping the main memory, if it is overlapped, then
//              updates MemoryCeiling variable and reboot.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//              RootBrgData  - Pointer to Root Bridge private structure data.
//              RootBrgIndex - Root Bridge index (0 Based).
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspAdjustMemoryMmioOverlap (
									IN PCI_HOST_BRG_DATA    *HostBrgData,
									IN PCI_ROOT_BRG_DATA    *RootBrgData,
									IN UINTN                RootBrgIndex );

/**
    This function will be invoked in PCI Host Bridge Driver
    before converting all Non Existant MMIO into PCI MMIO.
    In order to allow CSP code do aome none standard conversion.

    @param ImageHandle - this image Handle
    @param ControllerHandle - Controller(RB) Handle (Optional).

    @retval EFI_STATUS
            EFI_UNSUPPORTED - means use default MMIO convertion.
            EFI_SUCCESS - CSP code has been converted MMIO itself.
            ANYTHING else - ERROR.
            
    @note  Porting required if needed.
**/
EFI_STATUS HbCspConvertMemoryMapIo(
    IN EFI_HANDLE      ImageHandle,
    IN EFI_HANDLE      ControllerHandle);

EFI_STATUS HbCspConvertMemoryMapMmio(
    IN EFI_HANDLE      ImageHandle,
    IN EFI_HANDLE      ControllerHandle);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspBasicChipsetInit
//
// Description: This function will be invoked after Initialization of generic
//              part of the Host and Root Bridges.
//              All Handles for PCIHostBrg and PciRootBrg has been created
//              and Protocol Intergaces installed.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspBasicChipsetInit (
    IN PCI_HOST_BRG_DATA    *HostBrg0, UINTN	HbCount);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspGetProposedResources
//
// Description: This function will be invoked when Pci Host Bridge driver runs  
//              out of resources.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspGetProposedResources (
    IN PCI_HOST_BRG_DATA                            *HostBrgData,
    IN PCI_ROOT_BRG_DATA                            *RootBrgData,
    IN UINTN                                        RootBrgNumber );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspPreprocessController
//
// Description: This function is called for all the PCI controllers that
//              the PCI bus driver finds.
//              It can be used to Preprogram the controller.
//
// Input:       HostBrgData   - Pointer to Host Bridge private structure data.
//              RootBrgData   - Pointer to Root Bridge private structure data.
//              RootBrgNumber - Root Bridge number (0 Based).
//              PciAddress    - Address of the controller on the PCI bus.
//              Phase         - The phase during enumeration
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS HbCspPreprocessController (
    IN PCI_HOST_BRG_DATA                            *HostBrgData,
    IN PCI_ROOT_BRG_DATA                            *RootBrgData,
    IN UINTN                                        RootBrgNumber,
    IN EFI_PCI_CONFIGURATION_ADDRESS                PciAddress,
    IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HbCspAllocateResources
//
// Description: This function is invoked in PCI Host Bridge Driver when time
//              to ask GCD for resources. You can overwrite a default
//              algorithm used to allocate resources for the Root Bridge.
//
// Input:       HostBrgData  - Pointer to Host Bridge private structure data.
//              RootBrgData  - Pointer to Root Bridge private structure data.
//              RootBrgIndex - Root Bridge index (0 Based).
//
// Output:      EFI_STATUS
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HbCspAllocateResources (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RbCspIoPciMap
//
// Description: Chipset Specific function to Map Internal Device address
//              residing ABOVE 4G to the BELOW 4G address space for DMA.
//              MUST BE IMPLEMENTED if CHIPSET supports address space
//              decoding ABOVE 4G.
//
// Input:       *RbData        - Root Bridge private structure data
//              Operation      - Operation to provide Mapping for
//              HostAddress    - HostAddress of the Device
//              *NumberOfBytes - Number of Byte in Mapped Buffer.
//              *DeviceAddress - Mapped Device Address.
//              **Mapping      - Mapping Info Structure this function must
//                               allocate and fill.
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Successful.
//                  EFI_UNSUPPORTED - The Map function is not supported.
//                  EFI_INVALID_PARAMETER - One of the parameters has an
//                                          invalid value.
//
// Notes:       Porting is required for chipsets that supports Decoding
//              of the PCI Address Space ABOVE 4G.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                         HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RbCspIoPciUnmap
//
// Description: Chipset Specific function to Unmap previousely Mapped
//              buffer for DMA.
//              MUST BE IMPLEMENTED if CHIPSET supports address space
//              decoding ABOVE 4G.
//
// Input:       *RbData  - Root Bridge private structure data
//              *Mapping - Mapping Info Structure this function must free.
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Successful.
//                  EFI_UNSUPPORTED - The Unmap function is not supported.
//                  EFI_INVALID_PARAMETER - One of the parameters has an
//                                          invalid value.
//
// Notes:       Porting required if needed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RbCspIoPciUnmap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    OUT PCI_ROOT_BRIDGE_MAPPING                     *Mapping );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RbCspIoPciAttributes
//
// Description: Chipset Specific function to do PCI RB Attributes releated
//              programming.
//
// Input:       RbData         - Pointer to Root Bridge private structure.
//              Attributes     - The Root Bridge attributes to be programming.
//              ResourceBase   - Pointer to the resource base. (OPTIONAL)
//              ResourceLength - Pointer to the resource Length. (OPTIONAL)
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Successful.
//                  EFI_INVALID_PARAMETER - One of the parameters has an
//                                          invalid value.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RbCspIoPciAttributes (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN UINT64                                       Attributes,
    IN OUT UINT64                                   *ResourceBase OPTIONAL,
    IN OUT UINT64                                   *ResourceLength OPTIONAL );


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RootBridgeIoPciRW
//
// Description: Read Pci Registers into buffer.
//              Csp Function which actualy access PCI Config Space.
//              Chipsets that capable of having PCI EXPRESS Ext Cfg Space
//              transactions.
//              Must Implement this access routine here.
//
// Input:       *RbData - Root Bridge private structure.
//              Width   - PCI Width.
//              Address - PCI Address.
//              Count   - Number of width reads/writes.
//              *Buffer - Buffer where read/written.
//              Write   - Set if write.
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Successful read.
//                  EFI_INVALID_PARAMETER - One of the parameters has an
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RootBridgeIoPciRW (
    IN VOID                                     *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
    IN UINT64                                   Address,
    IN UINTN                                    Count,
    IN OUT VOID                                 *Buffer,
    IN BOOLEAN                                  Write );




#endif //__AMI_PCI_ACCESS_CSP_BASE_LIB_H__
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

