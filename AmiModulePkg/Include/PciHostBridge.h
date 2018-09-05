//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//**********************************************************************
// $Header: /Alaska/BIN/Core/Modules/PciBus/PciHostBridge.h 15    8/16/10 1:14p Yakovlevs $Revision:$
//
// $Date: 8/16/10 1:14p $
//**********************************************************************
// Revision History
// ----------------
//$Log: $
// 
//**********************************************************************
/** @file PciHostBridge.h
    This header file contains Data Type Definitions for 
    the PCI Host/Root Bridge Driver Private Data 

 Tabsize:		4
**/

#ifndef _AMI_PCI_HOST_BRIDGE_H_
#define _AMI_PCI_HOST_BRIDGE_H_
#ifdef __cplusplus
extern "C" {
#endif


//**********************************************************************
//Includes goes here
#include <Efi.h>
#include <Dxe.h>
#include <AcpiRes.h>
#include <PciSetup.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/PciPlatform.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DeviceIo.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Protocol/AmiBoardInitPolicy.h>


/**
    Macro defining PCI Config Spase access UCING PCI Express Base

**/
#ifndef EFI_PCI_CFG_ADDRESS
#define	EFI_PCI_CFG_ADDRESS(bus,dev,func,reg)	\
 ((UINT64) ( (((UINTN)bus) << 24)+(((UINTN)dev)	<< 16) + (((UINTN)func)	<< 8) + ((UINTN)reg)))& 0x00000000ffffffff
#endif


//Defining ROOT BRIDGE Compatibility  Attributes group
#define PCI_ROOT_COMPATIBILITY_ATTRIBUTES 	(EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO |	EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO| \
	 EFI_PCI_ATTRIBUTE_ISA_IO | EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO | EFI_PCI_ATTRIBUTE_ISA_IO_16)

#define PCI_ROOT_VGA_ATTRIBUTES 	(EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO  | EFI_PCI_ATTRIBUTE_VGA_MEMORY | \
     EFI_PCI_ATTRIBUTE_VGA_IO | EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16 | EFI_PCI_ATTRIBUTE_VGA_IO_16)

#define MAX_PCI_BUSES   0xFF
#define MIN_PCI_IO      0x1000
#define MAX_PCI_IO      0xFFFF
#define MIN_PCI_MMIO32  0x100000    
#define MAX_PCI_MMIO32  0xFFFFFFFF
#define MIN_PCI_MMIO64  0x100000000    
#define MAX_PCI_MMIO64  0xFFFFFFFFFFFFFFFF

//Pci Host and Hoot Bridge Data Signature UINT64
#define AMI_RB_SIG          0x24524252494d4124 //'$AMIRBR$'
#define AMI_HB_SIG          0x24494350494d4124 //'$AMIHST$'

//**********************************************************************
//Private Data Definitions
//**********************************************************************

//forward declaration
typedef struct _PCI_HOST_BRG_DATA PCI_HOST_BRG_DATA;

/**
    Structure used to specify RootBridge Specific Mapping 
    Information for DMA transfere.

 Fields:		
	Name			Type					Description
   ------------------------------------------------------------------
	Operation	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION	PciRoot Bridge IO Operation Description. 
	NumBytes	UINTN		Number Of Bytes in Mapping Buffer.
	HostAddr	EFI_PHYSICAL_ADDRESS	Buffer Address of the device who initiated BM transfere.
	DeviceAddr	EFI_PHYSICAL_ADDRESS	Buffer Address of the device who receives BM transfere.
**/
typedef struct _PCI_ROOT_BRIDGE_MAPPING {
  	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION	Operation;
  	UINTN                                      	NumPages;
  	EFI_PHYSICAL_ADDRESS                       	HostAddr;
  	EFI_PHYSICAL_ADDRESS                       	DeviceAddr;
}	PCI_ROOT_BRIDGE_MAPPING;


typedef enum {
    raBus=0,
    raIo,
    raMmio32,
    raMmio64,
    raMaxRes
} ACPI_RES_TYPE;


typedef struct _ACPI_RES_DATA{
    UINT64                  Min;
    UINT64                  Len;
    UINT64                  Max;
    UINT64                  Gra;
    UINT64                  AddrUsed;
    EFI_GCD_ALLOCATE_TYPE   AllocType;
} ACPI_RES_DATA;

/**
    Private RootBridge Data Structure

 Fields:		
	Name			Type					Description
   ------------------------------------------------------------------
	RbIoProtocol	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	This PCI Root Bridge IO Protocol Instance. 
  RbHandle		EFI_HANDLE					This PCI Root Bridge EFI_HANDLE.
	DevPath			*EFI_DEVICE_PATH_PROTOCOL	This PCI Root Bridge Device Path Protocol instance.
	Owner			*PCI_HOST_BRG_DATA			Pointer on the Host Bridge Private Data Structure who owns this Root Bridge.
	DevIoProtocol	*EFI_DEVICE_IO_INTERFACE	This PCI Root Bridge Device IO Protocol instance.
	Supports		UINT64						Set of Attirbutes Supported by this PCI Root Bridge.
	Attributes		UINT64						Set of currently supported Attirbutes by this PCI Root Bridge.
	ResInitCnt		UINTN						To make this and following 2 fields look like T_ITEM_LIST type.
  ResCount		UINTN						Number of ACPI QWord Resource descriptors of this PCI Root Bridge.
	RbRes			**ASLR_QWORD_ASD			Pointer to the array of pointers at the resource descriptors of this PCI Root Bridge. 
	Mapping			*PCI_ROOT_BRIDGE_MAPPING	When Mapping is set this pointer points on Private RB Mapping Structure.
	BusesSet		BOOLEAN						Flag to indicate that Bus Allocation Phase is over.
	ResSubmited 	BOOLEAN						Flag to indicate that Resources were submited by the PciBus Driver.
	ResAsquired		BOOLEAN						Flag to indicate that Resource request was sutisfied by the GCD.
	Reserved		UINT8						Reserved field to make this structure end at 4 byte alignment.
  BusXlatEntry    *PCI_BUS_XLAT_HDR           Pointer at AMI SDL Generated Bus Translate table corresponded to THIS Root Bridge. 
  RbAslName       *CHAR8                      Pointer to the String of THIS Root Bridge Device in ACPI Name Space.
  AcpiRbRes[]     ACPI_RES_DATA               Array holding THIS Root Bridge Resource Information for ACPI 
**/
typedef struct _PCI_ROOT_BRG_DATA {
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL		RbIoProtocol;
	EFI_HANDLE							RbHandle;
	EFI_HANDLE                          ImageHandle;
	EFI_DEVICE_PATH_PROTOCOL			*DevPath;
	PCI_HOST_BRG_DATA					*Owner;
	EFI_DEVICE_IO_INTERFACE				*DevIoProtocol;
	UINT64								Supports;
	UINT64								Attributes;
	//make it as T_ITEM_LIST type
	UINTN								ResInitCnt;
	UINTN								ResCount;
	ASLR_QWORD_ASD						**RbRes;
	PCI_ROOT_BRIDGE_MAPPING				*Mapping;
	BOOLEAN								BusesSet;
	BOOLEAN								ResSubmited;
	BOOLEAN								ResAsquired;
	BOOLEAN								NotPresent;
    CHAR8                               *RbAslName;
    ACPI_RES_DATA                      	AcpiRbRes[raMaxRes];//BUS; IO; MMIO32; MMIO64; 
    AMI_SDL_PCI_DEV_INFO				*RbSdlData;
    UINTN								RbSdlIndex;
    AMI_BOARD_INIT_PROTOCOL 			*PciInitProtocol;
    UINT64                              Signature;
} PCI_ROOT_BRG_DATA;

typedef enum  {
    ptBus=0,    //0
    ptIo,       //1
    ptIo32,		//2
    ptMmio32,   //3
    ptMmio32pf, //4
    ptMmio64,   //5
    ptMmio64pf, //6
    ptMaxType
} HP_PADD_RES_TYPE;



/**
    Private HostBridge Data Structure. It holds all information 
    related to HPC and HPB Location in the System.

 Fields:		
	Name			Type					Description
   ------------------------------------------------------------------
	HpcLocation     EFI_HPC_LOCATION        HPC Location Information in terms of Device Path.
	Owner           *PCI_HOST_BRG_DATA      Back Pointer to Locate this Data structure Owner.
  SlotHdr		    PCI_BUS_XLAT_HDR		AMI SDL Generated SLOT Bus Xlat Information (AmiBoardInfoProtocol).
	Padding[]		UINTN64					Array, holding Padding Information for particular Resource type.
**/
typedef struct _HPC_LOCATION_DATA {
    EFI_HPC_LOCATION    	HpcLocation;
    PCI_ROOT_BRG_DATA   	*Owner;
    AMI_SDL_PCI_DEV_INFO    *SdlHpBrgData;
    UINTN					SdlHpBrgIndex;
    PCI_HOTPLUG_SETUP_DATA	HpSetup;
} HPC_LOCATION_DATA;


/**
    Private HostBridge Data Structure. It holds all Hot Plug 
    related Information. Used to Produce EFI_PCI_HOT_PLUG_INIT_PROTOCOL.

 Fields:		
	Name			Type					Description
   ------------------------------------------------------------------
	RootHotplugProtocol EFI_PCI_HOT_PLUG_INIT_PROTOCOL	This PCI Root Hot Plug Controller Initialization Protocol Instance.
  HbHandle		EFI_HANDLE					This PCI Host Bridge EFI_HANDLE.
	HpclInitCnt		UINTN						To make this and following 2 fields look like T_ITEM_LIST type.
	HpcLocCount     UINTN						Number of Root Bridges Hot Plug Location Data Structures Associated with THIS HOST.
	HpcLocData		**HPC_LOCATION_DATA			Pointer to the array of pointers at the private PCI Hot Plug Controller Location Data. 
	Owner           *PCI_HOST_BRG_DATA	        Back Pointer to Locate this Data structure Owner.
**/
typedef struct _PCI_HPC_DATA {
    //The Protocol coming first...
    EFI_PCI_HOT_PLUG_INIT_PROTOCOL  RootHotplugProtocol;
    //HPC Location data... to make it look like T_ITEM_LIST
    UINTN               HpclInitCnt;
    UINTN               HpcLocCount;
    HPC_LOCATION_DATA   **HpcLocData;
    PCI_HOST_BRG_DATA   *Owner;
} PCI_HPC_DATA;



/**
    Private HostBridge Data Structure. This is the Starting 
    point for PCI Subsystem.

 Fields:		
	Name			Type					Description
   ------------------------------------------------------------------
	ResAllocProtocol EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	This PCI Host Bridge Resource Allocation Protocol Instance.
  HbHandle		EFI_HANDLE					This PCI Host Bridge EFI_HANDLE.
	RbInitCnt		UINTN						To make this and following 2 fields look like T_ITEM_LIST type.
	RootBridgeCount UINTN						Number of PCI_ROOT_BRG_DATA structures (Root Bridges) this host will group.
	RootBridges		**PCI_ROOT_BRG_DATA			Pointer to the array of pointers at the private PCI Root Bridge Data Structure. 
	AllocPhase		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE	Current Phase of PCI Bus Enumeration.
	AllocAttrib		UINT64						This PCI Host Bridge Allocation Attributes.
	EnumStarted		BOOLEAN						Flag to indicate that PCI Bus Enumeration has started.
	Reserved[3]		UINT8						Reserved fields to make this structure end at 4 byte alignment.
**/
typedef struct _PCI_HOST_BRG_DATA {
	EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	ResAllocProtocol;
	EFI_HANDLE											HbHandle;
	EFI_HANDLE											ImageHandle;
	//make it as T_ITEM_LIST type
	UINTN												RbInitCnt;
	UINTN												RootBridgeCount;
	PCI_ROOT_BRG_DATA									**RootBridges;
	//----------------------------
	EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE		AllocPhase;
	UINT64												AllocAttrib;
	BOOLEAN												EnumStarted;
	UINT8												Reserved[3];
    //Optional Protocols - one instance per HOST
    PCI_HPC_DATA                                        *HpcData;
    AMI_SDL_PCI_DEV_INFO								*HbSdlData;
    UINTN												HbSdlIndex;
    AMI_BOARD_INIT_PROTOCOL 							*PciInitProtocol;
    UINT64                                              Signature;
}PCI_HOST_BRG_DATA;

//**********************************************************************//
//Some Helper Functions
EFI_STATUS GetResources(PCI_ROOT_BRG_DATA *RootBrg, ASLR_QWORD_ASD **Resources, ASLR_TYPE_ENUM ResType);


/****** DO NOT WRITE BELOW THIS LINE *******/

#ifdef __cplusplus
}
#endif
#endif//_AMI_PCI_HOST_BRIDGE_H_
//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
