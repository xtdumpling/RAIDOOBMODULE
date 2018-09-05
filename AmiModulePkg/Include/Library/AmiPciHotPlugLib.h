//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
/** @file AmiPciHotPlugLib.h
    Library Class for AMI Hot Plug Support Functions. 


**/
//*************************************************************************
#ifndef _AMI_PCI_HOT_PLUG_LIB_H_
#define _AMI_PCI_HOT_PLUG_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------

#include <Pci.h>  
#include <PciSetup.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <AmiDxeLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHotPlugInit.h> 

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
//extern EFI_PCI_HOT_PLUG_INIT_PROTOCOL	*gRhpcInitProtocol;

/**
    This function will update pointer to PCI_RHPC_INFO of
    Bridge Type device which creates a hot plug bus. Also if "Device"
    creates the 'home bus' for Root HPC it will initialize Root HPC and
    record the HPC state;

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpCheckRootHotplug(PCI_DEV_INFO *Device, UINT8 MaxBusFound);


/**
    This function will check if "Device" passed is the Bridge
    Type Device with hotplug support.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    If "Device" is a Bridge with HPC on it.
        FALSE   Otherwice.

**/
BOOLEAN HpIsHpb(PCI_DEV_INFO *Device);

/**
    This function will get and apply resource padding
    requirements for the PCI to PCI Bridge or Card Bus Bridge, if this
    Bridge is supports hot plug.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpGetHpbResPadding(PCI_DEV_INFO *Device);


/**

    @param 

    @retval 

**/
EFI_STATUS HpAllocateInitHpData(PCI_DEV_INFO *Device, UINT16 HpCapOffset);

/**

    @param 

    @retval 

**/
BOOLEAN HpCheckHpCompatible(PCI_DEV_INFO *Device);

/**

    @param 

    @retval 

**/
EFI_STATUS HpLocateProtocolSortRhpc(PCI_HOST_INFO *PciHpcHost, PCI_DEV_INFO	*RootBridge);


/**

    @param 

    @retval 

**/

EFI_STATUS HpApplyBusPadding(PCI_DEV_INFO *Brg, UINT8 OldMaxBus, UINT8 *MaxBusFound);


/**

    @param 

    @retval 

**/
EFI_STATUS HpApplyResPadding(PCI_BAR *PaddingBar, PCI_BAR *BridgeBar);


/**
    This function will collect all information HP related
    information and initialize an instance of PCI Root Hotplug Controller
    Initialization Protocol.

    @param HostBrg Pointer on Private Data
        structure for which PCI Hot Plug Init Protocol going to
        be initialized

    @retval EFI_SUCCESS is OK
        EFI_NOT_FOUND no Hotplug slots where found.

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS HpcInstallHpProtocol (PCI_HOST_BRG_DATA *HostBrg	);

EFI_STATUS HpcFindSlots(PCI_HOST_BRG_DATA *HostBrg, PCI_ROOT_BRG_DATA *RootBrg);



EFI_STATUS HpcGetResourcePadding(IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
                                 IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
                                 IN  UINT64                         HpcPciAddress,
                                 OUT EFI_HPC_STATE                  *HpcState,
                                 OUT VOID                           **Padding,
                                 OUT EFI_HPC_PADDING_ATTRIBUTES     *Attributes);




VOID HpcFillDescriptor(ASLR_QWORD_ASD *Descriptor, HP_PADD_RES_TYPE PaddType, UINT64  Length);


EFI_STATUS HpcInitInitializeRootHpc(IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL       *This,
                                    IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
                                    IN  UINT64                              HpcPciAddress,
                                    IN  EFI_EVENT                         	Event, OPTIONAL
                                    OUT EFI_HPC_STATE                    	*HpcState);




BOOLEAN HpcCheckHpCompatibleHost(PCI_HOST_BRG_DATA *HostBrg);



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif //_AMI_PCI_BUS_COMMON_LIB_H_


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

