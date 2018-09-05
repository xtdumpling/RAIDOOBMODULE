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

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        NbCspLib.h
//
// Description: This file contains North Bridge chipset porting functions
//              and data structures definition for both PEI & DXE stage.
//
//<AMI_FHDR_END>
//*************************************************************************

#ifndef __NBLIB_H__
#define __NBLIB_H__

#include <efi.h>
#include <PEI.h>
#include <Protocol\BootScriptSave.h>
#include <Protocol\PciHostBridgeResourceAllocation.h>
#include <Protocol\PciRootBridgeIo.h>
#include <PciHostBridge.h>
#include <AmiDxeLib.h>
//#include <PciBus.h>


#ifdef __cplusplus
extern "C" {
#endif

#if (PCIBUS_VERSION >= 20100) && (PCIBUS_VERSION < 20200)
#define CORE4634_SUPPORT 1
#endif

#if (PCIBUS_VERSION >= 20200)
#define CORE4636_SUPPORT 1
#endif

#if 0
#define LEGACY_REGION_LOCK          0                // Read only Read to RAM, Write to ROM
#define LEGACY_REGION_BOOT_LOCK     1
#define LEGACY_REGION_UNLOCK        2                // Read/Write to RAM
#define LEGACY_REGION_DECODE_ROM    3                // Read/Write to ROM

EFI_STATUS
NBProgramPAMRegisters (
    EFI_BOOT_SERVICES        *gBS,
    EFI_RUNTIME_SERVICES    *gRS,
    UINT32                    StartAddress,
    UINT32                    Length,
    UINT8                    Setting,
    UINT32                    *Granularity OPTIONAL
);

EFI_STATUS NBPeiProgramPAMRegisters(
    IN EFI_PEI_SERVICES **PeiServices,
    UINT32                StartAddress,
    UINT32                Length,
    UINT8                Setting,
    UINT32                *Granularity OPTIONAL
);

EFI_STATUS NBPAMWriteBootScript(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL *BootScriptSave
);
#endif
//To Exclude AMI Native Root Bridge Driver HOOKS from CSP LIB
#include <Token.h>
#if AMI_ROOT_BRIDGE_SUPPORT == 1

//***********************************************************************
//-----------------------------------------------------------------------
//Function Prototypes for PciRootBridgeIo
//-----------------------------------------------------------------------
//***********************************************************************
EFI_STATUS HbResAllocNotifyPhase(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL         *This,
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE            Phase );

EFI_STATUS HbResAllocGetNextRootBridge(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
    IN OUT EFI_HANDLE                                           *RootBridgeHandle);

EFI_STATUS HbResAllocGetAllocAttributes(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
    IN EFI_HANDLE                                                RootBridgeHandle,
      OUT UINT64                                                  *Attributes);

EFI_STATUS HbResAllocStartBusEnumeration(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL            *This,
    IN EFI_HANDLE                                                  RootBridgeHandle,
    OUT VOID                                                    **Configuration);

EFI_STATUS HbResAllocSetBusNumbers(
        IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL        *This,
          IN EFI_HANDLE                                           RootBridgeHandle,
          IN VOID                                                 *Configuration);

EFI_STATUS HbResAllocSubmitResources(
        IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
          IN EFI_HANDLE                                               RootBridgeHandle,
          IN VOID                                                     *Configuration);
EFI_STATUS HbResAllocGetProposedResources(
          IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL            *This,
        IN EFI_HANDLE                                                  RootBridgeHandle,
        OUT VOID                                                    **Configuration);

EFI_STATUS HbResAllocPreprocessController(
        IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
        IN EFI_HANDLE                                               RootBridgeHandle,
          IN EFI_PCI_CONFIGURATION_ADDRESS                              PciAddress,
        IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE             Phase);



//***********************************************************************
//Chipset hooks function Prototypes

//***********************************************************************
//  This hooks will be called from Notify Phase Host Bridge Resource
//  Allocation Protocol member function
//***********************************************************************

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginEnumeration
EFI_STATUS HbNotifyCspBeforeEnumeration(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginBusAllocation
EFI_STATUS HbNotifyCspBeginBusAllocation(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeEndBusAllocation
EFI_STATUS HbNotifyCspEndBusAllocation(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginResourceAllocation
EFI_STATUS HbNotifyCspBeginResourceAllocation(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeAllocateResources
EFI_STATUS HbNotifyCspAllocateResources(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeSetResources
EFI_STATUS HbNotifyCspSetResources(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeEndResourceAllocation
EFI_STATUS HbNotifyCspEndResourceAllocation(
        EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                        **RbIoProtocolBuffer,
        UINTN                                                RbCount    );

//***********************************************************************
//  This hooks will be called from Other Host Bridge Resource
//  Allocation Protocol member functions
//***********************************************************************

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol StartBusEnumeration function
EFI_STATUS HbCspStartBusEnumeration(
        PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SetBusNumbers function
EFI_STATUS HbCspSetBusNnumbers(
        PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SubmitResources function
EFI_STATUS HbCspSubmitResources(
        PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SubmitResources function
EFI_STATUS HbCspAdjustMemoryMmioOverlap(
        PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//BASIC Chipset initialization function calle after Initialization of generic part of the
//Host and Root Bridges. All Handles for PCIHostBrg and PciRootBrg has been created and
//Protocol Intergaces installed
EFI_STATUS HbCspBasicChipsetInit(PCI_HOST_BRG_DATA *HostBrg0);


//-----------------------------------------------------------------------
//Csp Function which will return PCI Segment Number for
//Chipsets that capable of having multiple PCI segments
//Parameters UINTN    HostNumber, UINTN RootNumber is ZERO based
UINTN HbCspGetPciSegment(UINTN    HostBridgeNumber, UINTN RootBridgeNumber);

//-----------------------------------------------------------------------
//Csp Function which will be called each time PciBus Enumerator finds Pci Device
//on the bus. This Function will allow to program particular device registers before
//resource requirements are collected from that device.
EFI_STATUS HbCspPreprocessController(PCI_HOST_BRG_DATA *HostBrgData, PCI_ROOT_BRG_DATA *RootBrgData,
                                UINTN RootBrgNumber, EFI_PCI_CONFIGURATION_ADDRESS PciAddress,
                                EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase);



//-----------------------------------------------------------------------
//Chipset Specific function to Map Internal Device address
//residing ABOVE 4G to the BELOW 4G address space for DMA.
//MUST BE IMPLEMENTED if CHIPSET supports address space
//decoding ABOVE 4G.
EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                            HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping);

//-----------------------------------------------------------------------
//Chipset Specific function to UnMap result of previouse PciRbIo->Map
//operation
EFI_STATUS RbCspIoPciUnmap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN PCI_ROOT_BRIDGE_MAPPING                        *Mapping);

//-----------------------------------------------------------------------
//Chipset Specific function to Sets attributes for a resource range
//on a PCI root bridge.
EFI_STATUS RbCspIoPciAttributes(PCI_ROOT_BRG_DATA *RbData,
                                IN     UINT64    Attributes,
                                IN OUT UINT64    *ResourceBase OPTIONAL,
                                IN OUT UINT64    *ResourceLength OPTIONAL);

//-----------------------------------------------------------------------
//Csp Function which actualy access PCI Config Space
//Chipsets that capable of having PCI EXPRESS Ext Cfg Space transactions
//Must Implement this access routine here
EFI_STATUS RootBridgeIoPciRW (
    IN PCI_ROOT_BRG_DATA                        *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
    IN UINT64                                    Address,
    IN UINTN                                    Count,
    IN OUT VOID                                    *Buffer,
    IN BOOLEAN                                    Write);


//Pci RB CSP function prototypes
EFI_STATUS RbCspIoPciUnmap (
    IN    PCI_ROOT_BRG_DATA                             *RbData,
    OUT PCI_ROOT_BRIDGE_MAPPING                     *Mapping);

EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                             *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                            HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping);

#if defined CORE4634_SUPPORT && CORE4634_SUPPORT == 1
//-----------------------------------------------------------------------
//Chipset function that returns Host Bridge Number for the corresponding Root Bridge
UINTN HbCspMapRootBrgToHost(PCI_BUS_XLAT_HDR *RootBrgXlatHdr);
#endif

#if defined CORE4636_SUPPORT && CORE4636_SUPPORT == 1
//-----------------------------------------------------------------------
//Chipset function that returns Host Bridge Number for the corresponding Root Bridge
UINTN HbCspMapRootBrgToHost(PCI_BUS_XLAT_HDR *RootBrgXlatHdr, UINT64 *AllocationAttr, UINT64 *RbSuportedAttr);

EFI_STATUS HbCspAllocateResources( PCI_HOST_BRG_DATA *HostBrgData,
                                PCI_ROOT_BRG_DATA *RootBrgData,
                                UINTN RootBrgIndex);
#endif


//--------------------------------------
//#if AMI_ROOT_BRIDGE_SUPPORT == 1
#endif

#ifdef __cplusplus
}
#endif

//-----------------------------------------------------------------------
// PCI & PCIE Function declaration
//-----------------------------------------------------------------------
UINT32 FindCapPtr(UINT64 PciAddress, UINT8 CapID);

EFI_STATUS PciExpressVideoCardInstalled(
    IN EFI_PEI_SERVICES        **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI         *PciCfg,
    IN UINT64            Address,
    IN UINT8            StartBus,
    IN UINT8            EndBus
);

EFI_STATUS PciExpressDeviceInstalled(
    IN UINT64            Address,
    IN UINT8            StartBus,
    IN UINT8            EndBus
);

typedef EFI_STATUS (*NB_PCIE_CALLBACK_FUNC)(UINT64 Address);

EFI_STATUS PciExpressDeviceInitialize(
    IN UINT64           Address,
    IN UINT8            StartBus,
    IN UINT8            EndBus,
    IN NB_PCIE_CALLBACK_FUNC CallBackFunction
);

EFI_STATUS PciVideoCardInstalled(
    IN EFI_PEI_SERVICES        **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI         *PciCfg
);
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// NB Porting Hooks
//-----------------------------------------------------------------------
UINT32 NBGetTsegBase(VOID);
VOID NBEnableEmrr(UINT32 IedStart, UINT32 IedSize);
VOID NbRuntimeShadowRamWrite(IN BOOLEAN Enable);                    // [ EIP33862 ]
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Lynnfield Porting Hooks
//-----------------------------------------------------------------------
EFI_STATUS LfdGetCoreDevNum(UINT8* CoreDevNum);
EFI_STATUS LfdCpuOnlyReset(VOID);
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Havendale/Clarkdale Porting Hooks
//-----------------------------------------------------------------------
EFI_STATUS HvdCpuOnlyReset(VOID);
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------

#endif

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
