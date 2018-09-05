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

/** @file AmiPciBusLib.h
    Library Class for AMI PCI Bus And AMI Root Bridge common routines.

**/
//*************************************************************************
#ifndef _AMI_PCI_BUS_COMMON_LIB_H_
#define _AMI_PCI_BUS_COMMON_LIB_H_

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Token.h>
#include <PciBus.h>
#include <Pci.h> 
#include <PciE.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiBoardInfo2.h>

//-------------------------------------------------------------------------
/// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
//PCI Bus Porting Constants definitions
const UINT16 NbDmiL0ExitLatency;
const UINT16 NbDmiL1ExitLatency;
const UINT16 SbDmiL0ExitLatency;
const UINT16 SbDmiL1ExitLatency;
const UINT16 NbDmiAspmSupport;
const UINT16 SbDmiAspmSupport;
const UINT16 SbInternalDelay;

//----------------------------------------------------------------------------------
//PCI Bus Configuration Constants definitions
//const BOOLEAN S3VideoRepost;
const BOOLEAN FixedBusAssign;
const BOOLEAN DecodeFullBusRanges;
const UINT8	PciRserveUncoreBuses;
const BOOLEAN HotPlugSupport;
const BOOLEAN ApplyPaddingAnyway;
const BOOLEAN CombineMemPfMem32;
const BOOLEAN CombineMem64PfMem;
const BOOLEAN IgnoreOptionRom;
const BOOLEAN Io32support;
const BOOLEAN UsePciIoForOptRom;

const UINT32 CrdBusPaddingMmioLen;
const UINT32 CrdBusPaddingMmioGra;
const UINT32 CrdBusPaddingIoLen;
const UINT32 CrdBusPaddingIoGra;

const BOOLEAN PcieCrsSupport;
const BOOLEAN PciRbCombineMemPmemReset;
const BOOLEAN DeviceClassPolicyOverride;
const UINT8   PolicyOverrideMode;
const BOOLEAN LaunchCommonRoutineFirst;
const BOOLEAN DontUpdateRbResAsl;
const BOOLEAN DontUpdatePebs;
const BOOLEAN PciWriteVidDid;
// APTIOV_SERVER_OVERRIDE_START
const BOOLEAN DontUpdateRbUidAsl;
// APTIOV_SERVER_OVERRIDE_END

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

BOOLEAN     gNoSdlDataMode;


/**
    Fills gPciBusDb Array in ascending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/

EFI_STATUS  AmiPciAddBusDbEntry(AMI_SDL_PCI_DEV_INFO *SdlData, T_ITEM_LIST *BusDb);

/**
    Fills gPciBusDb Array in ascending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS  AmiPciAddDevDbEntry(AMI_SDL_PCI_DEV_INFO *SdlData, T_ITEM_LIST *BusDb);

/**
    Fills gPciBusDb Array in ascending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS  AmiPciAddBarDbEntry(PCI_BAR *Bar, T_ITEM_LIST *BarDb, BOOLEAN Fixed);

/**
    Will do PCI Configuration Space Access 8;16;32;64 bit width

        
    @param RbIo Pointer to PciRootBridgeIO Protocol.
    @param addr PCI_CFG_ADDR filled by caller
    @param wr TRUE = Write FALSE = Read
    @param buff Pointer Data Buffer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

 Referals:PCI_CFG_ADDR

**/
EFI_STATUS PciCfg8(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT8 *buff);

EFI_STATUS PciCfg16(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT16 *buff);

EFI_STATUS PciCfg32(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT32 *buff);

EFI_STATUS PciCfg64(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT64 *buff);


/**
    Checks if PCI_DEV_INFO is Function 0 of PCI device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsFunc0(PCI_DEV_INFO *Device );

/**
    Checks if PCI_DEV_INFO data passed belongs to Function 0 of
    Multy-Functional device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsFunc0OfMfDev(PCI_DEV_INFO *Device );

/**
    Checks if PCI_DEV_INFO data passed belongs to other than Func0 of
    Multy-Functional device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsNotFunc0OfMfDev(PCI_DEV_INFO *Device );

/**
    Creates a copy of T_ITEM_LST structure.

        
    @param Lst Pointer to the structure to copy.
    @param NewLstPtr Double Pointer to the copied data (Memory allocation is done by this function).

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.

**/
EFI_STATUS CpyItemLst(T_ITEM_LIST *Lst, T_ITEM_LIST **NewLstPtr);

/**
    Programm SubordinateBusNumber Register of PCI Bridge.

        
    @param Brg Pointer to PCI Bridge Private Data structure.
    @param SubBusNo Number to programm.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS SetSubBus(PCI_DEV_INFO *Brg, UINT8 SubBusNo);


/**
    This function will Reprogram Primary Secondary and Subordinate
    bus numbers for the downsteram bridges after SEC BUS reset signal assertion.

        
    @param DnPort Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS RestoreBridgeBuses(PCI_DEV_INFO *DnPort);


/**
    Maps Bridge's Primary Secondary Subordinate Bus Numbers
    according information stored in PCI_DEV_INFO and PCI_BRG_EXT structures
    of the PCI Bridge.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BRG_INFO; PCI_BRG_EXT.

    @note  
  BaseBus         =   PCI_DEV_INFO->Address.Addr.Bus;
  SecondaryBus    =   PCI_BRG_EXT->Res[rtBus].Base;
  SubordinateBus  =   PCI_BRG_EXT->Res[rtBus].Base + PCI_BRG_EXT->Res[rtBus].Length-1;

**/
EFI_STATUS MapBridgeBuses(PCI_DEV_INFO *Brg);


/**
    Checks if value passed makes "POWER OF TWO"

        
    @param Value Value to check.

    @retval BOOLEAN
        TRUE or FALSE based on value passed.
**/
BOOLEAN IsPowerOfTwo(UINT64 Value);


/**
    Clears/zeros contents of PCI_BAR structure.

        
    @param Bar Pointer to PCI_BAR structure to clear.

    @retval Nothing

**/
VOID ClearBar(PCI_BAR *Bar);


/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/

EFI_STATUS SortRbSdlData(AMI_SDL_PCI_DEV_INFO ***RbSdlData, UINTN *SdlDataCount);


UINT8 FindRootBus(PCI_DEV_INFO *Device);

#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
EFI_STATUS AmiPciGetSetupData(	PCI_SETUP_DATA 			*PciSetupData, 
								PCI_COMMON_SETUP_DATA 	*PciCommon,
								PCI_HOTPLUG_SETUP_DATA 	*HpSetup);
#else
EFI_STATUS AmiPciGetSetupData(	PCI_COMMON_SETUP_DATA 	*PciCommon,
								PCI_DEVICE_SETUP_DATA	*PciDev, 
								PCIE1_SETUP_DATA		*Pcie1,
								PCIE2_SETUP_DATA		*Pcie2,
								AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,  	//OPTIONAL if ==NULL get defaults...
								UINTN					DevIdx);		//OPTIONAL 
#endif

EFI_STATUS AmiPciLibLaunchInitRoutine(VOID *RoutineOwner, PCI_INIT_STEP InitStep, PCI_INIT_TYPE InitType,
		VOID *Param1, VOID *Param2, VOID *Param3, VOID *Param4);

BOOLEAN PciBusCheckIfPresent(PCI_DEV_INFO *Dev, AMI_SDL_PCI_DEV_INFO *DevFnData);

EFI_STATUS PciBusReadNextEmbeddedRom(	PCI_DEV_INFO *Device, UINTN OpRomNumber, //IN
									    AMI_SDL_PCI_DEV_INFO **ThisRomSdlData, VOID **RomFile, UINTN *RomSize); //OUT


EFI_STATUS AmiPciGetCommonSetupData(PCI_COMMON_SETUP_DATA 	*PciCommon);

EFI_STATUS AmiPciGetPciDevSetupData(PCI_DEVICE_SETUP_DATA 	*PciDev,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,
									BOOLEAN 				Set);

EFI_STATUS AmiPciGetPcie1SetupData( PCIE1_SETUP_DATA 		*Pcie1,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set);


EFI_STATUS AmiPciGetPcie2SetupData( PCIE2_SETUP_DATA 		*Pcie2,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set);

EFI_STATUS AmiPciGetPciHpSetupData( PCI_HP_SETUP_DATA 		*PciHp,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set);

EFI_STATUS AmiPciBusShift(			AMI_SDL_PCI_DEV_INFO 	*DevSdlInfo,
						  	  	  	INT16					*ShiftValue,	
						  	  	  	UINTN					HostIdx,
									UINTN					RootIdx,
						  	  	  	BOOLEAN					Set);

//Compatibility routine from AmiCompatibilityPkg
EFI_STATUS LaunchPortingRoutine(PCI_DEV_INFO *Device, PCI_INIT_STEP InitStep, 
								VOID *Param1, VOID *Param2, VOID *Param3, VOID *Param4);

//Check if System SDL Data has Fixed resource allocation information.
EFI_STATUS AmiPciCheckFixedResources();

//Tests if region1(s-e) belongs to the region2(S-E)
//returns EFI_ACCESS_DENIED if Belongs=FALSE and region 1 belong to region 2
//returns EFI_ACCESS_DENIED if Belongs=TRUE and region 1 don't belong to region 2
//returns EFI_SUCCESS in all other cases.
EFI_STATUS AmiPciTestRegions(UINT64 s, UINT64 e, UINT64 S, UINT64 E, BOOLEAN Belongs);


EFI_STATUS  AmiPciUpdateBarFixedRes(PCI_BAR *FixedBar);


#endif //_AMI_PCI_BUS_COMMON_LIB_H_
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

