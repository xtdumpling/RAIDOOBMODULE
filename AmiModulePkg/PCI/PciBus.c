//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fixed: Mask all device relate to NVMe, root port and devices.
//    Reason:    
//    Auditor:   Kasber Chen
//    Date:      Aug/21/2017
//
//  Rev. 1.10
//    Bug Fixed: Add XXV710 base address 0x10 into bad list.
//    Reason:    Fixed XXV710 PXE boot fail when enabled Above 4G issue.
//    Auditor:   Jacker Yeh
//    Date:      Jul/28/2017
//
//  Rev. 1.09
//    Bug Fixed: Add 24NVMe patch
//    Reason:    1. Hide 10 of 12 NVMe to speed up POST
//				 2. Prelocate MMIO for empty NVMe slot
//    Auditor:   Donald Han
//    Date:      May/16/2017
//
//  Rev. 1.08
//    Bug Fixed: Disable NVMe ports SERR/PERR.
//    Reason:    From grantley
//    Auditor:   Kasber Chen
//    Date:      Apr/13/2017
//
//  Rev. 1.07
//    Bug Fixed: Add XL710 base address 0x10 into bad list.
//    Reason:
//    Auditor:   Kasber Chen
//    Date:      Feb/18/2017
//
//  Rev. 1.06
//    Bug Fixed: Add X557 for PHY CS4227(0x37D3) base address 0x10 into bad list.
//    Reason:    Function improve.
//    Auditor:   William Hsu
//    Date:      Dec/20/2016
//  
//  Rev. 1.05
//    Bug Fixed: Add a function that limit the primary VGA's resources under
//               4G.(Refer to Grantley)
//    Reason:    To prevent form Display have no output when enabled Above 4G
//               and set VGA Priority to offboard.
//    Auditor:   Isaac Hsu
//    Date:      Dec/14/2016
//
//  Rev. 1.04
//    Bug Fixed: Add X722(0x37D1) base address 0x10 into bad list.
//    Reason:    Function improve.
//    Auditor:   Jimmy Chiu
//    Date:      Dec/12/2016
//  
//  Rev. 1.03
//    Bug Fixed: Add X540, 82599, 82599EB base address 0x10 into bad list.
//    Reason:    Function improve.
//    Auditor:   Jacker Yeh
//    Date:      Oct/27/2016
//
//  Rev. 1.02
//    Bug Fixed: Add X550, X710 and XL710 base address 0x10 into bad list.
//    Reason:    To avoid get wrong MAC address work when enable above 4G decoce problem.
//    Auditor:   Jacker Yeh
//    Date:      Nov/16/2016
//
//  Rev. 1.01
//    Bug fixed: Fix system assert error when PCI device plugged and debug_mode enabled.
//    Reason:    From Greenlow
//    Auditor:   Kasber Chen
//    Date:      Jul/05/2016
//
//  Rev. 1.00
//    Bug fixed: Refer Grantley to add SMC LAN OPROM selective function.
//    Reason:    
//    Auditor:   Kasber Chen
//    Date:      Jul/07/2016
// 
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


/** @file PciBus.c
    EFI PCI Bus Generic Driver.
**/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <Efi.h>
#include <Token.h>
//---------------------------------------------------------------------------
//Protocols used
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/Decompress.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/PciPlatform.h>
#include <Protocol/IncompatiblePciDeviceSupport.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/PciBusEx.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/AmiCsmOpromPolicy.h>

//---------------------------------------------------------------------------
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <PciBus.h>
#include <AcpiRes.h>

//---------------------------------------------------------------------------
//AmiPackage Libraries used
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressLib.h>
#include <Library/AmiPciExpressGen2Lib.h>
#include <Library/AmiPciExpressGen3Lib.h>
#include <Library/AmiPciHotPlugLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/AmiSriovLib.h>
//APTIOV_SERVER_OVERRIDE_START
#include <Library/PcdLib.h>
//APTIOV_SERVER_OVERRIDE_END
#if PCI_OUT_OF_RESOURCE_SETUP_SUPPORT
#include <Setup.h>
#endif


#if EFI_DEBUG || USB_DEBUG_TRANSPORT
#include <AmiDebugPort.h>
#endif

#if SMCPKG_SUPPORT
#include <SmcLib.h>
UINT16  BDF_Tbl[25];
#endif
///
/// Function Prototypes for Driver Binding Protocol Interface
///
EFI_STATUS EFIAPI PciBusSupported(IN EFI_DRIVER_BINDING_PROTOCOL	*This,
						   IN EFI_HANDLE                    Controller,
						   IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath);

EFI_STATUS EFIAPI PciBusStart(IN EFI_DRIVER_BINDING_PROTOCOL		*This,
					   IN EFI_HANDLE						Controller,
					   IN EFI_DEVICE_PATH_PROTOCOL			*RemainingDevicePath );

EFI_STATUS EFIAPI PciBusStop(IN EFI_DRIVER_BINDING_PROTOCOL			*This,
					   IN EFI_HANDLE						Controller,
					   IN UINTN								NumberOfChildren,
					   IN EFI_HANDLE						*ChildHandleBuffer);

EFI_STATUS EFIAPI PciEnumerateAll(EFI_HANDLE Controller);

EFI_STATUS EFIAPI StartPciDevices(IN EFI_HANDLE ControllerHandle, 
					   IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath);

//-------------------------------------------------------
//Forward declarations for Extended PCI Bus Protocol
EFI_STATUS EFIAPI PciExtIsPciExpresss(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIE_DATA                                       **PciExpData    OPTIONAL
);

EFI_STATUS EFIAPI PciExtIsPciX(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIX_DATA                                       **PciXData      OPTIONAL
);

EFI_STATUS EFIAPI PciExtIsPciBrg(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
);


EFI_STATUS EFIAPI PciExtIsCrdBrg(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
);


EFI_STATUS EFIAPI PciExtIsDevice(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo          OPTIONAL
);


EFI_STATUS EFIAPI PciExtClassCodes(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL,
	OUT PCI_DEV_CLASS									*CassCodes
);


EFI_STATUS EFIAPI PciExtPicIrqRout (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL						 		*PciIo 		OPTIONAL,
    OUT PCI_IRQ_PIC_ROUTE                               **PicIrqTblEntry,
    OUT EFI_PCI_CONFIGURATION_ADDRESS                   **ParentDevices,
    OUT UINTN                                           *EntryCount
);

EFI_STATUS EFIAPI PciExtApicIrqRout (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL						 		*PciIo 		OPTIONAL,
    OUT PCI_IRQ_APIC_ROUTE                              **ApicIrqTblEntry,
    OUT EFI_PCI_CONFIGURATION_ADDRESS                   **ParentDevices,
    OUT UINTN                                           *EntryCount
);

EFI_STATUS EFIAPI ReadEfiRom(PCI_DEV_INFO	*Dev, PCI_ROM_IMAGE_DATA *RomData, VOID **ImgBase, UINT32 *ImgSize);

//VOID EFIAPI CheckEmptySetupSlot(PCI_DEV_INFO *Device);

VOID EFIAPI PciSmmReadyToLockCallback (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

//==================================================================================
//Some GUIDs variables if needed here
//==================================================================================
T_ITEM_LIST gBadPciDevList={0,0,NULL};

BOOLEAN 	gPciFixedRes	        = FALSE;
BOOLEAN     gPciHotPlug             = FALSE;
BOOLEAN     gReadyToLock            = FALSE;

T_ITEM_LIST gNotFixedResBarList={0,0,NULL};

T_ITEM_LIST gFixedResBarList={0,0,NULL};

T_ITEM_LIST gThirdPartyRomList={0,0,NULL};

EFI_PCI_PLATFORM_POLICY gPlatformPolicy=EFI_RESERVE_NONE_IO_ALIAS;

CSM_SETUP_DATA      *gCsmSetupPtr=NULL;  

ROM_SETUP_DATA      *gRomSetupPtr=NULL;

UINT32              gRomAttributes=(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS);
//=============================================================================
/**
    Incompatible PCI Devices List known so far.

    @note  
  See PCI_BAD_BAR for field names and usage details.
  If BarType field == tBarMaxType then BarOffset field should be used. 
  For "BarType" field only tBarIo, tBarMem and tBarMaxType allowed!
**/
PCI_BAD_BAR gAmiBadPciDevList[]={ 
//-----------------------------------------------------------------------------------------------------------------------
//enum			UINT16		UINT16		UINT16		UINT16	  PCI_BAR_TYPE 	UINTN		UINT64		UINT64
//IncompType;	VendorId;	DeviceId;	DevIdMask;	Reserved;	BarType;	BarOffset;	BarBase;	BarLength;
//-----------------------------------------------------------------------------------------------------------------------
//Device Adaptec 9004
{	icBarBad,	0x9004,		0xFFFF,		0x0000,		0x0000,		tBarIo,		0x0000, 	0, 			0 			},
//Device Adaptec 9005
{	icBarBad,	0x9005,		0xFFFF,		0x0000,		0x0000,		tBarIo,		0x0000, 	0, 			0 			},
//Device QLogic  1007
{	icBarBad,	0x1007,		0xFFFF,		0x0000,		0x0000,		tBarIo,		0x0000, 	0,			0 			},
//Device Agilent 103C
{	icBarBad,	0x103C,		0xFFFF,		0x0000,		0x0000,		tBarIo,		0x0000,		0,			0 			},
//Device Agilent 15BC
{	icBarBad,	0x15BC,		0xFFFF,		0x0000,		0x0000,		tBarIo,		0x0000, 	0,			0			},
//Device AMI Mega RAID 493
{icBarFixedSize,0x1077,		0x1216,		0xFFFF,		0x0000,	 	tBarMaxType,0x0014, 	0,			0x400000	},
//ICH8 smbus controller 
{icNotBar,		0x8086,		0x283E,		0xFFFF,		0x0000,	 	tBarMaxType,0x0014, 	0,			0			},
#if RT8111_ON_BOARD_BAD_ROM_BAR
//RTL8111E
{   icNotBar,   0x10EC,     0x8168,     0xFFFF,     0x0000,     tBarMaxType,0x0030,     0,          0           },
#endif
//NVIDIA 7200 GS
{icBad64BitBar,	0x10de,		0x01d3,		0xFFFF,		0x0000,	 	tBarMaxType,0x0014, 	0,			0			},
//RealTek 8111DP
{icBarFixedSize,0x10EC,     0x8168,     0xFFF0,     0x0000,     tBarIo,     0x0010,     0,          0x400       },
//APM Root Complex Root Port
{icNotBar,      0x19aa,     0xE008,     0xFFF0,     0x0000,     tBarMaxType,0x0010,     0,          0           },
//new APM's RC 1.13.28 changes V/D ID
{icNotBar,      0x10E8,     0xE004,     0xFFF0,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
//Device IPC 940 very old PCI Card
{icNotBar,      0x1498,     0x33AC,     0xFFFF,     0x0000,     tBarMaxType,0x0030,     0,          0           },
//Intel EMMC Interface  B0 D30 F4
{icBad64BitBar, 0x8086,     0x9D2B,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
//Intel SDIO  B0 D30 F5
{icBad64BitBar, 0x8086,     0x9D2C,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
//Intel SDXC  B0 D30 F6
{icBad64BitBar, 0x8086,     0x9D2D,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
#if SMCPKG_SUPPORT
//X540
{icBad64BitBar,	0x8086,	0x1528,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//82599EB
{icBad64BitBar,	0x8086,	0x10FB,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//82599
{icBad64BitBar,	0x8086,	0x1557,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x8D3A,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x8D3B,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x14E4,	0x16A1,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x8D31,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//X550
{icBad64BitBar,	0x8086,	0x1563,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//X710
{icBad64BitBar,	0x8086,	0x1572,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//XL710, 40GbE backplane
{icBad64BitBar,	0x8086,	0x1580,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//XL710, 40GbE QSFP+
{icBad64BitBar,	0x8086,	0x1583,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x1589,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//X722
{icBad64BitBar,	0x8086,	0x37D0,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x37D1,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x37D2,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
{icBad64BitBar,	0x8086,	0x37D3,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
//XXV710
{icBad64BitBar,	0x8086,	0x158B,	0xFFFF,	0x0000,	tBarMaxType, 0x0010, 0,	0},
#endif
};

/**
    Number or records in gBadPciDevList table.
    @note  UINTN

**/
UINTN gAmiBadPciDevCount=sizeof(gAmiBadPciDevList)/sizeof(PCI_BAD_BAR);


///
/// PCI Bus Driver Global Variables
///

/**
    Root Bridge Allocation Attributes
    @note  UINT64

**/
UINT64	gAllocationAttributes=0;

/**
    Indicates Out Of Resources Condition.
    @note  BOOLEAN

**/
BOOLEAN	gPciOutOfRes=FALSE;

BOOLEAN	gPciOutOfResHit=FALSE;


/**
    Indicates what type of resource has Out Of Resources Condition.
    @note  MRES_TYPE

**/
MRES_TYPE gLowResType=rtMaxRes;

AMI_OUT_OF_RES_VAR	gPciOutOfResData={0};


//----------------------------------------------------------------------------------
//EfiDBE Key field definition used to sort resources by size 
//Function prototypes used for Compare keys we will use a custom way.
//We will be using combination UINT64 Length (LS64) + UINT64 Granularity (MS64).
INTN EFIAPI Cmp128IntRR(IN VOID *pContext, VOID *pRecord1, VOID *pRecord2);
INTN EFIAPI Cmp128IntKR(IN DBE_OFFSET_KEY_CONTEXT *pContext, VOID *pKey, VOID *pRecord);
//Initialize BAR DataBase KeyField Structure  
DBE_OFFSET_KEY_CONTEXT  BarKeyInfo = { EFI_FIELD_OFFSET(PCI_BAR,Length), 
                                    EFI_FIELD_SIZE(PCI_BAR,Length)+EFI_FIELD_SIZE(PCI_BAR,Gran)};
DBE_KEY_FIELD           gBarKey = {(DBE_CMP)&Cmp128IntRR, (DBE_CMP)&Cmp128IntKR, &BarKeyInfo}; 

/**
    Bus order Data Base if fixed bus allocation selected we need 
    to know how many buses does this bridge suppose to decode
    @note  T_ITEM_LIST

**/

/**
    Global Setup Variable to get the setup settings pointer.

    @note  PCI_SETUP_DATA

**/
PCI_SETUP_DATA		    *gPciDefaultSetup=NULL;

PCI_COMMON_SETUP_DATA	*gPciCommonSetup=NULL;

T_ITEM_LIST				gSdlSetup={0,0,NULL};

/**
    Global counter of buses found.

    @note  UINT8

**/
UINT8				mMaxBusFound;
UINTN               mMaxBusScan;
UINT8               mMaxBusReport=0;

/**
    Initial Global Variable to store Host Bridge Count.

    @note  UINTN

**/
UINTN							gHostCnt;

/**
    Initial Global Variable to store pointer on PCI Subsystem
    Host (This is very ROOT of the PCI Bus Driver Data).  

    @note  PCI_HOST_INFO

**/
PCI_HOST_INFO					**gPciHostList=NULL;
PCI_HOST_INFO					*gPciHostArray=NULL;

/// Bot script save protocol interface
EFI_S3_SAVE_STATE_PROTOCOL			*gS3SaveState=NULL;

/// PCI Devices Init Protocol Instance.
AMI_BOARD_INIT_PROTOCOL		*gPciInitProtocol=NULL;

/// PCI Compatibility Porting Protocol
AMI_PCI_PORT_COMPATIBILITY_PROTOCOL	*gPciPortProtocol=NULL;

//----------------------------------------------------------------------------------
#if S3_VIDEO_REPOST_SUPPORT == 1
EFI_EVENT           gVgaS3Event=NULL;
#endif

//----------------------------------------------------------------------------------
/// Initial Global Variable to store CPU Cache Info
UINT16				gCpuCaheLineSize=0x40;
UINT8				gPciCaheLineSize=0x10;		

//----------------------------------------------------------------------------------
/// To Avoid Enumerating AmiDebug Port Usb Device 
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
AMI_DEBUGPORT_INFORMATION_HOB 	*gDbgPortHob=NULL;
DEBUG_PORT_INFO					gDbgPortInfo={0,0,0,0,0,0,0,0};
#endif

/**
    Extended PCI Bus Protocol instance for PciBus Driver

    @note  AMI_PCI_EXT_PROTOCOL

**/

AMI_PCI_EXT_PROTOCOL gAmiExtPciBusProtocol = {
    PciExtIsPciExpresss,
    PciExtIsPciX,
    PciExtIsPciBrg,
    PciExtIsCrdBrg,
    PciExtIsDevice,
    PciExtClassCodes,    
    PciExtPicIrqRout,
    PciExtApicIrqRout,
    NULL
};



/**
    Driver binding protocol instance for PciBus Driver

    @note  EFI_DRIVER_BINDING_PROTOCOL

**/
EFI_DRIVER_BINDING_PROTOCOL gPciBusDriverBinding = {
	PciBusSupported,	///Supported
	PciBusStart,		///PciBusDrvStart,
	PciBusStop,			///PciBusDrvStop,
	((PCI_BUS_MAJOR_VER<<16)|(PCI_BUS_MINOR_VER<<8)|(PCI_BUS_REVISION)),///Version 					
	NULL,				///Image Handle
	NULL				///DriverBindingHandle
};

#ifdef EFI_DEBUG
#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
#ifndef LANGUAGE_CODE_ENGLISH
#define LANGUAGE_CODE_ENGLISH "eng"
#endif
static BOOLEAN LanguageCodesEqual(
    CONST CHAR8* LangCode1, CONST CHAR8* LangCode2
){
    return    LangCode1[0]==LangCode2[0] 
           && LangCode1[1]==LangCode2[1]
           && LangCode1[2]==LangCode2[2];
}
static EFI_GUID gEfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
#endif
///---------------------------------------------------------------------------
/// Driver Name Interface of the PCI Bus Driver
///---------------------------------------------------------------------------
static UINT16 *gDriverName=L"AMI PCI Bus Driver";


///---------------------------------------------------------------------------
/// Function Definitions
///---------------------------------------------------------------------------

/**
    This function is the one of interface functions of the
    OPTIONAL, EFI_COMPONENT_NAME_PROTOCOL. Suppose to retun Controller
    Name String. Currently returning EFI_UNSUPPORTED.

        
        EFI_COMPONENT_NAME_PROTOCOL 
        *This               Pointer to EFI_COMPONENT_NAME_PROTOCOL.
    @param ControllerHandle EFI_HANDLE of the device which to return.
    @param ChildHandle OPTIONAL, an EFI_HANDLE of child device.
    @param Language Pointer to 3 char Language name.
    @param ControllerName Pointer to the data buffer for Name Sring.

    @retval EFI_STATUS
        EFI_SUCCESS         When everything is going on fine!
        EFI_UNSUPPORTED     When feature not supported by the Driver.

 Reference: EFI_COMPONENT_NAME2_PROTOCOL

**/
static EFI_STATUS EFIAPI ComponentNameGetControllerName (
		IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
		IN  EFI_HANDLE                   ControllerHandle,
		IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
		IN  CHAR8                        *Language,
		OUT CHAR16                       **ControllerName )
{
	return EFI_UNSUPPORTED;
}

/**
    This function is the one of interface functions of the OPTIONAL
    EFI_COMPONENT_NAME_PROTOCOL.	Retuning Driver Name String of the PCI BUS Driver.

        
        EFI_COMPONENT_NAME_PROTOCOL 
        *This               Pointer to EFI_COMPONENT_NAME_PROTOCOL.
    @param Language Pointer to 3 char Language name.
    @param ControllerName Pointer to the data buffer for Name Sring.

    @retval EFI_STATUS
        EFI_SUCCESS         When everything is going on fine!
        EFI_UNSUPPORTED     When feature not supported by the Driver.

 Reference:	EFI_COMPONENT_NAME_PROTOCOL
**/
static EFI_STATUS EFIAPI ComponentNameGetDriverName(IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
						IN  CHAR8                        *Language,
						OUT CHAR16                       **DriverName)
{
	//Supports only English
	if(!Language || !DriverName) return EFI_INVALID_PARAMETER;

	if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH))
        return EFI_UNSUPPORTED;
	else 
        *DriverName=gDriverName;
	
	return EFI_SUCCESS;
}

/**
    Declaration of the EFI_COMPONENT_NAME_PROTOCOL

 Fields:		Name				Type					Description
          ------------------------------------------------------------------
  ComponentNameGetDriverName,		F_PTR 		DriverName function pointer.
  ComponentNameGetControllerName, F_PTR		ControllerName function pointer.
  "eng"							CHAR8 		Language list Buffer.
**/
static EFI_COMPONENT_NAME2_PROTOCOL gComponentName = {
  ComponentNameGetDriverName,
  ComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

#endif

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// PCI BUS Driver Entry Point
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    This function is the entry point for PCI BUS Driver. 
    Since PCI BUS Driver follows EFI 1.1 driver model in it's entry point
    it will initialize some global data and install
    EFI_DRIVER_BINDING_PROTOCOL. 

        
    @param ImageHandle Image handle.
    @param SystemTable Pointer to the EFI system table.

    @retval EFI_STATUS
        EFI_SUCCESS         When everything is going on fine!
        EFI_NOT_FOUND       When something required is not found!
        EFI_DEVICE_ERROR    When the device is not responding!

    @note  
  Entry Points are used to locate or install protocol interfaces and
 notification events. 

**/
EFI_STATUS EFIAPI PciBusEntryPoint(IN EFI_HANDLE			ImageHandle,
							IN EFI_SYSTEM_TABLE   *SystemTable )
{
	static EFI_GUID CpuInfoHobGuid = AMI_CPUINFO_HOB_GUID;
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
	static EFI_GUID DbgPortHobGuid = AMI_DEBUGPORT_HOB_GUID;
#endif
	static EFI_GUID HobListGuid	= HOB_LIST_GUID;
	CPUINFO_HOB					*CpuInfoHob;
	EFI_STATUS					Status;	
	EFI_EVENT Event;
	VOID *Registration;
//--------------------------------------------------------------------

	//Init some Global Data
	InitAmiLib(ImageHandle,SystemTable);
	gPciBusDriverBinding.DriverBindingHandle=NULL;
	gPciBusDriverBinding.ImageHandle=ImageHandle;

	//Get CPU Cache Line Size 
	//this is by design - reusing CpuInfoHob variable to get first hob in the list and than
	//go forward and find needed hob by the GUID
	CpuInfoHob=(CPUINFO_HOB*)GetEfiConfigurationTable(pST,&HobListGuid);
	if(CpuInfoHob==NULL) Status=EFI_UNSUPPORTED;
	else Status=FindNextHobByGuid(&CpuInfoHobGuid,(VOID**)&CpuInfoHob);
	
	if(EFI_ERROR(Status)){
        PCI_TRACE((TRACE_PCI,"PciBus: Unable to find CpuInfo HOB! Setting default CacheLineSize -> 0x%X\n", gCpuCaheLineSize));
	} else {
		gCpuCaheLineSize=CpuInfoHob->CacheLineSize; //in bytes
  		gPciCaheLineSize=(UINT8)gCpuCaheLineSize/4; //in DWORDs
		PCI_TRACE((TRACE_PCI,"PciBus: Find CpuInfo HOB! gCpuCaheLineSize=0x%X; gPciCaheLineSize=0x%X;\n", gCpuCaheLineSize, gPciCaheLineSize));
    }
		
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
//Get Ami Debug Port Settings
	gDbgPortHob=(AMI_DEBUGPORT_INFORMATION_HOB*)GetEfiConfigurationTable(pST,&HobListGuid);
	if(gDbgPortHob) Status=FindNextHobByGuid(&DbgPortHobGuid,(VOID**)&gDbgPortHob);
	if(EFI_ERROR(Status)) gDbgPortHob=NULL;
	else {
		Status=gDbgPortHob->GetDebugPortProperties(gDbgPortHob, &gDbgPortInfo);
		ASSERT_EFI_ERROR(Status);
	}
#endif

	//Install Multiple Prot Drv. Binding and Comp. Name
	Status = pBS->InstallMultipleProtocolInterfaces(
					&gPciBusDriverBinding.DriverBindingHandle,
					&gEfiDriverBindingProtocolGuid,&gPciBusDriverBinding,
#ifdef EFI_DEBUG
					&gEfiComponentName2ProtocolGuid,&gComponentName,
#endif
					NULL);
	//Here we can set up notification events if needed
	RegisterProtocolCallback(
	                &gEfiDxeSmmReadyToLockProtocolGuid,
	                PciSmmReadyToLockCallback,
	                NULL, &Event, &Registration);

	//------------------------------------
	return Status;
}


//#if HOTPLUG_SUPPORT
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Following function to initialize PCI Root Hotplug Controller
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//==============================================================================
//#endif

//==============================================================================
// Device Handle Helper Functions
//==============================================================================

/**
    This function will take Parent DevPath and extand it with
    Current ""Dev", Device Path and update pointer of the "Dev->DevPath"

        
    @param Dev Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS EFIAPI MakePciDevicePath(PCI_DEV_INFO *Dev)
{
	static PCI_DEVICE_PATH			pcidp;	
//-------------------------------------------
	//we have a wonderful Lib function DPAddNode - use it!
	pcidp.Header.SubType=HW_PCI_DP;
	pcidp.Header.Type=HARDWARE_DEVICE_PATH;
	SET_NODE_LENGTH(&pcidp.Header,HW_PCI_DEVICE_PATH_LENGTH);

	pcidp.Function=Dev->Address.Addr.Function;
	pcidp.Device=Dev->Address.Addr.Device;
	
	Dev->DevicePath=DPAddNode(Dev->ParentBrg->DevicePath, &pcidp.Header);
	if(Dev->DevicePath)return EFI_SUCCESS;	
	else return EFI_OUT_OF_RESOURCES;
}


/**
    This function will finds Root Bridge Device Handle for
    the "Device".

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_HANDLE
        !=NULL          When everything is going on fine!

**/
EFI_HANDLE EFIAPI FindRbHandle(PCI_DEV_INFO *Device){
	EFI_HANDLE  	rbh=NULL;
	PCI_DEV_INFO	*dev=Device;
//---------------------
	while(dev->Type!=tPciRootBrg)dev=dev->ParentBrg; 
	rbh=dev->Handle;
	return rbh; 
}

/**
    This function Searches PCI Bridge Subsystem Database to find
    device which have passed "Handle"

        
    @param Brg Pointer to PCI Bridge Device Private Data structure.
    @param Handle Device Handle to search for.

    @retval PCI_DEV_INFO
        !=NULL          When everything is going on fine!
        ==NULL          Not Found.

    @note  This is a subordinate function of FindPciDeviceByHandle()

**/
PCI_DEV_INFO* FindPciDeviceByHandleBrg(PCI_BRG_INFO *Brg, EFI_HANDLE Handle)
{
	PCI_DEV_INFO	*dev, *res=NULL;	
	INTN			i;
//--------------------------------------------------------------------
	if(Brg->Common.Handle==Handle) return &Brg->Common;

	for(i=0; i<(INTN)Brg->Bridge.ChildCount; i++){
		dev=Brg->Bridge.ChildList[i];
		res=NULL;
		if(dev->Handle==Handle) {
			res=dev;
			break;
		}
	
		if(dev->Type==tPci2PciBrg) res=FindPciDeviceByHandleBrg((PCI_BRG_INFO*)dev,Handle);
		
		if(res) break;
	}
	return res;
}

/**
    This function finds Pci Device by "Handle" passed

        
    @param Handle PCI Device Handle.

    @retval PCI_DEV_INFO
        !=NULL      When everything is going on fine!
        ==NULL      Not Found.

**/
PCI_DEV_INFO *FindPciDeviceByHandle(EFI_HANDLE Handle)
{
	PCI_BRG_INFO	*brg;
	UINTN			i,j;
	PCI_HOST_INFO	*lhst;
	PCI_DEV_INFO	*res;
//--------------------------------------------------------------------
	for(j=0; j<gHostCnt; j++){
		lhst=gPciHostList[j];
		for(i=0; i<lhst->RbCount; i++){ 
			brg=(PCI_BRG_INFO*)lhst->RootBridges[i];
			res=FindPciDeviceByHandleBrg(brg, Handle);
			if(res) return res;
		}
	}
	return NULL;
}

/**
    This function will follow the rules of calling seqence of
    Platform Preprocess Controller it will call:
    1.Platform PreprocessController with ChipsetEntery
    2.Host Brg Preprocess Controller 
    3.Platform PreprocessController with ChipsetExit

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS EFIAPI DoPrepController(PCI_DEV_INFO* Device)
{
	EFI_STATUS										Status=EFI_NOT_FOUND;	
	EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE 	ph;
	EFI_HANDLE  									rbh=FindRbHandle(Device);
//-----------
	if(Device->Type==tPci2PciBrg) ph=EfiPciBeforeChildBusEnumeration;
	else ph=EfiPciBeforeResourceCollection;

	//If System has PciPlatform protocol installed
	if(Device->HostData->PlatformProt!=NULL){
		Status=Device->HostData->PlatformProt->PlatformPrepController(
  				Device->HostData->PlatformProt, Device->HostData->HostHandle,
				rbh,Device->Address.Addr, ph, ChipsetEntry);
        PCI_TRACE((TRACE_PCI,"PciPlt: PrepController Dev[B%X|D%X|F%X]; Phase=%d; ExecP=0; Status=%r.\n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, 
        		Device->Address.Addr.Function, ph, Status));
	}

	//if PciPlatform returned an ERROR or not present...
	//but PlatformOverride protocol exists we must call it
	if( EFI_ERROR(Status) && (Device->HostData->PciOverrideProt!=NULL) ){
		Status=Device->HostData->PciOverrideProt->PlatformPrepController(
  				Device->HostData->PciOverrideProt, Device->HostData->HostHandle,
				rbh,Device->Address.Addr, ph, ChipsetEntry);
        PCI_TRACE((TRACE_PCI,"PltOvr: PrepController Dev[B%X|D%X|F%X]; Phase=%d; ExecP=0; Status=%r.\n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, 
        		Device->Address.Addr.Function, ph, Status));
	}
	
	//This one must exist..
	Status=Device->HostData->ResAllocProt->PreprocessController(
				Device->HostData->ResAllocProt,rbh,Device->Address.Addr, ph);
	if(EFI_ERROR(Status) && Status!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(Status);

	Status=EFI_NOT_FOUND;
	
	//Chipset Exit ExecPhase:
	//If System has PciPlatform protocol installed
	if(Device->HostData->PlatformProt!=NULL){
		Status=Device->HostData->PlatformProt->PlatformPrepController(
  				Device->HostData->PlatformProt, Device->HostData->HostHandle,
				rbh, Device->Address.Addr, ph, ChipsetExit);
        PCI_TRACE((TRACE_PCI,"PciPlt: PrepController Dev[B%X|D%X|F%X]; Phase=%d; ExecP=1; Status=%r.\n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, 
        		Device->Address.Addr.Function, ph, Status));
	}	

	//if PciPlatform returned an ERROR or not present...
	//but PlatformOverride protocol exists we must call it...
	if( EFI_ERROR(Status) && (Device->HostData->PciOverrideProt!=NULL) ){
		Status=Device->HostData->PciOverrideProt->PlatformPrepController(
  				Device->HostData->PciOverrideProt, Device->HostData->HostHandle,
				rbh,Device->Address.Addr, ph, ChipsetExit);
        PCI_TRACE((TRACE_PCI,"PltOvr: PrepController Dev[B%X|D%X|F%X]; Phase=%d; ExecP=1; Status=%r.\n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, 
        		Device->Address.Addr.Function, ph, Status));
	}
	
    if(Status==EFI_NOT_FOUND || Status==EFI_UNSUPPORTED) Status=EFI_SUCCESS;
	return Status;
}


/**
    This function will follow the rules of calling seqence of
    Platform Notify it will call: 
    1.Platform NotifyPhase with ChipsetEntery
    2.Host Brg NotifyPhase 
    3.Platform NotifyPhase with ChipsetExit

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS EFIAPI DoPciNotify(PCI_HOST_INFO *Host, EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE Phase)
{
	EFI_STATUS	Status=EFI_SUCCESS;	
//-----------
	if(Host->PlatformProt){
		//Status=Host->PlatformProt->PhaseNotify(Host->PlatformProt,Host->HostHandle,Phase,ChipsetEntry);
		Status=Host->PlatformProt->PlatformNotify(Host->PlatformProt,
		        Host->HostHandle,Phase,ChipsetEntry);
        PCI_TRACE((TRACE_PCI,"PciPlt: PciNotify HostHandle=0x%X; Phase=%d; ExecP=0; Status=%r.\n",
        		Host->HostHandle, Phase, Status));
	}

	//if PciPlatform returned an ERROR or not present...
	//but PlatformOverride protocol exists we must call it
	if( EFI_ERROR(Status) && (Host->PciOverrideProt!=NULL) ){
		Status=Host->PciOverrideProt->PlatformNotify(Host->PciOverrideProt,
		        Host->HostHandle,Phase,ChipsetEntry);
        PCI_TRACE((TRACE_PCI,"PciOvr: PciNotify HostHandle=0x%X; Phase=%d; ExecP=0; Status=%r.\n",
        		Host->HostHandle, Phase, Status));
	}

	//This one must exist...it comes from ResAllocationProtocol
	Status=Host->ResAllocProt->NotifyPhase(Host->ResAllocProt,Phase);
    PCI_TRACE((TRACE_PCI,"PciOvr: PciNotify HostHandle=0x%X; Phase=%d; ExecP=0; Status=%r.\n",
            Host->HostHandle, Phase, Status));
	if(EFI_ERROR(Status) && Status!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(Status);

	Status=EFI_NOT_FOUND;
	
	//Chipset Exit ExecPhase:
	//If System has PciPlatform protocol installed
	if(Host->PlatformProt){
		//Status=Host->PlatformProt->PhaseNotify(Host->PlatformProt,Host->HostHandle,Phase,ChipsetExit);
		Status=Host->PlatformProt->PlatformNotify(Host->PlatformProt,
		        Host->HostHandle,Phase,ChipsetExit);
        PCI_TRACE((TRACE_PCI,"PciPlt: PciNotify HostHandle=0x%X; Phase=%d; ExecP=1; Status=%r.\n",
        		Host->HostHandle, Phase, Status));
	}
	
	//if PciPlatform returned an ERROR or not present...
	//but PlatformOverride protocol exists we must call it
	if( EFI_ERROR(Status) && (Host->PciOverrideProt!=NULL) ){
		Status=Host->PciOverrideProt->PlatformNotify(Host->PciOverrideProt,
		        Host->HostHandle,Phase,ChipsetExit);
        PCI_TRACE((TRACE_PCI,"PciOvr: PciNotify HostHandle=0x%X; Phase=%d; ExecP=1; Status=%r.\n",
        		Host->HostHandle, Phase, Status));
	}
	
	if(Status==EFI_NOT_FOUND || Status==EFI_UNSUPPORTED) Status=EFI_SUCCESS;
	return Status;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	PCI IO Protocol Functions Implementation Protocol 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//==================================================================================
// PciIoProtocol Supporting Functions
//==================================================================================

/**
    This helper function will check if Base Address Register(BAR) selected
    for PciIo operation is of valid type and has been initialized;

        
    @param Dev Pointer to PciDevice Info structure.
    @param BarIndex Index of the BAR within PCI device 
    @param BarType Indicating what to check - IO or Memory.

    @retval BOOLEAN
        TRUE    BAR supports selected transaction.
        FALSE   BAR does not support selected transaction.

 Referals: PCI_DEV_INFO; PCI_BAR_TYPE

**/
BOOLEAN EFIAPI BarCheckType(PCI_DEV_INFO  *Dev, UINT8 BarIndex, PCI_BAR_TYPE BarType)
{
	PCI_BAR *bar=&Dev->Bar[BarIndex];
//-------------------------------------
	switch(BarType){
    	case tBarMem:
			if (bar->Type != tBarMmio32 && 
				bar->Type != tBarMmio32pf &&
				bar->Type != tBarMmio64 &&
				bar->Type != tBarMmio64pf ) return FALSE;
			break;

		case tBarIo:
			if (bar->Type != tBarIo32 && bar->Type != tBarIo16) 
				return FALSE;
			break;
		default: return FALSE; 
	}
 
  return TRUE;
}


/**
    This helper function will check if parameters passed to the 
    PCI IO Protocol to read or write a PCI resources are correct.

        
    @param Dev Pointer to PciDevice Info structure.
    @param BarInd Index of the BAR within PCI device.
    @param BarType Indicating what to check - IO or Memory.
    @param Width Width of transaction.
    @param Count Number of bytes been transfered.
    @param Offset Offset within BAR address space to start from.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BAR_TYPE;EFI_PCI_IO_PROTOCOL_WIDTH

**/
EFI_STATUS EFIAPI PciIoCheckBar(PCI_DEV_INFO *Dev, UINT8 BarInd, PCI_BAR_TYPE Type,
						EFI_PCI_IO_PROTOCOL_WIDTH Width, UINTN Count, UINT64 *Offset)
{
	if ( Width<0 || Width>=EfiPciIoWidthMaximum ) return EFI_INVALID_PARAMETER;

	if (BarInd==0xFF) return EFI_SUCCESS;
	
	if (BarInd>=MAX_PCI_BAR) return EFI_INVALID_PARAMETER;

	if (!BarCheckType(Dev,BarInd,Type))return EFI_INVALID_PARAMETER;

	// If Width is EfiPciIoWidthFillUintX then convert to EfiPciIoWidthUintX
	if (Width >=(EFI_PCI_IO_PROTOCOL_WIDTH) EfiPciWidthFifoUint8 && Width <= (EFI_PCI_IO_PROTOCOL_WIDTH)EfiPciWidthFifoUint64) Count = 1; 
	Width &= 0x03;

	if((*Offset+Count*((UINTN)1<<Width))-1 >= Dev->Bar[BarInd].Length) return EFI_INVALID_PARAMETER;

	*Offset = *Offset+(UINTN)Dev->Bar[BarInd].Base;

	return EFI_SUCCESS;
}


/**
    This helper function will check if parameters passed to the
    PCI IO Protocol to made a PCI Config Space Access are correct

        
    @param Dev Pointer to PciDevice Info structure.
    @param Width Width of transaction been performed.
    @param Count Number of bytes been transfered.
    @param Offset Offset to start from.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BAR_TYPE; EFI_PCI_IO_PROTOCOL_WIDTH

**/
//**********************************************************************
EFI_STATUS EFIAPI PciIoCheckConfig(PCI_DEV_INFO  *Dev, EFI_PCI_IO_PROTOCOL_WIDTH Width,
							UINTN Count, UINT64 *Offset)
{
    PCI_CFG_ADDR  PciAddr;
//------------------------------    
    
	if(Width<0 || Width>= EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;

	// If Width is EfiPciIoWidthFillUintX then convert to EfiPciIoWidthUintX
	Width &= 0x03;
	if(PcieCheckPcieCompatible(Dev)){
        if((*Offset + Count * ((UINTN)1 << Width)) - 1 > 0xfff) return EFI_UNSUPPORTED;
	} else {
	    if((*Offset + Count * ((UINTN)1 << Width)) - 1 > 0xff) return EFI_UNSUPPORTED;
	}
	
	PciAddr.ADDR=0;
    PciAddr.ADDR=PCI_ASSEMBLE_ADDR(Dev->Address.Addr.Bus,
            Dev->Address.Addr.Device,Dev->Address.Addr.Function,0);

    if(*Offset>0xff)PciAddr.Addr.ExtendedRegister=(UINT32)*Offset;
    else PciAddr.Addr.Register=(UINT8)*Offset;
        
    *Offset=PciAddr.ADDR;
    
    return EFI_SUCCESS;
}

/**
    This function will Change IDE Device Mode LEGACY/NATIVE

        
    @param Dev Pointer to PCI Device Private Data structure.
    @param Primary Indicate Primary/Secondary controller 
    @param Legacy Indicate Mode to programm
    @param Override If not NULL value to programm in PI register 

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When device does not support mode programming.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; 

    @note  
	This function does not check if SecIde/PriIde Property of the Dev->HostData
  is tacken already caller must do that before call;
  This function does not update SecIde/PriIde Property of the Dev->HostData
  object. Caller must do that upon successfull return.
	
**/
EFI_STATUS EFIAPI SetIdeDevMode(PCI_DEV_INFO *Dev, BOOLEAN Primary, BOOLEAN Legacy, UINT8 *Override){

	PCI_CFG_ADDR	addr;
	EFI_STATUS		Status=EFI_SUCCESS;
	UINT8			newpi=0, oldpi=Dev->Class.ProgInterface, tmp=oldpi=Dev->Class.ProgInterface; 
	BOOLEAN 		canprog=FALSE, needprog=FALSE;
//--------        
	if(Dev->Class.BaseClassCode!=PCI_CL_MASS_STOR && Dev->Class.SubClassCode!=PCI_CL_MASS_STOR_SCL_IDE)	
		return EFI_INVALID_PARAMETER;
	
	addr.ADDR=Dev->Address.ADDR;
	addr.Addr.Register=PCI_PI_OFFSET;


	if(Override!=NULL){
		Status=PciCfg8(Dev->RbIo,addr,TRUE,Override);
		if(!EFI_ERROR(Status)) {
			UINT8	b=0;
		//----------------
			//read back what we have write there
			Status=PciCfg8(Dev->RbIo,addr,FALSE,&b);
			if(b==(*Override))Dev->Class.ProgInterface=(*Override);
			else Status=EFI_UNSUPPORTED;
			PCI_TRACE((TRACE_PCI, "\nPciBus: SetIdeMode(Pri=N/A, Legacy=N/A, PciPI=0x%X) @ B%X|D%X|F%X = %r,\n",*Override,
			Dev->Address.Addr.Bus,Dev->Address.Addr.Device,Dev->Address.Addr.Function, Status));
		}
	} else {
		if(Primary){
			canprog=(Dev->Class.ProgInterface & 0x02);
			oldpi&=(~0x01);
			tmp&=0x01;
			if(!Legacy) newpi|=0x01;
		} else {
			canprog=(Dev->Class.ProgInterface & 0x08);
			oldpi&=(~0x04);
			tmp&=0x04;
			if(!Legacy) newpi|=0x04;
		}
		needprog = (tmp ^ newpi); 
		if(needprog){
			if(canprog){	
				newpi|=oldpi;			
				Status=PciCfg8(Dev->RbIo,addr,TRUE,&newpi);
				if(!EFI_ERROR(Status)) Dev->Class.ProgInterface=newpi;	
			} else Status=EFI_UNSUPPORTED;
		} 
		PCI_TRACE((TRACE_PCI, "\nPciBus: SetIdeMode(Pri=%d, Legacy=%d, PciPI=0x%X)  @ B%X|D%X|F%X = %r,\n", Primary, Legacy, 
		newpi,Dev->Address.Addr.Bus,Dev->Address.Addr.Device,Dev->Address.Addr.Function, Status));
	}
	
	return Status;
}

/**
    Helper function to check if device referenced as "Dev" sits
    within Current VGA device parents bridges.

        
    @param Dev Pointer to PciDevice Info structure.

    @retval BOOLEAN
        TRUE    "Dev" in Current VGA device parents bridges.
        FALSE   "Dev" is not in Current VGA device parents bridges.

**/
BOOLEAN EFIAPI InCurVgaChain(PCI_DEV_INFO *Dev)
{
	PCI_DEV_INFO *d=Dev->HostData->VgaDev;
//------------------------------------
	if(!d) return FALSE;
	while (d->Type!=tPciRootBrg){
		if(d==Dev) return TRUE;
		d=d->ParentBrg;
	}
	return FALSE;
}


/**
    Protocol Function Sets or resets PCI Device Arttributs.

        
    @param Dev Pointer to PciDevice Info structure.
    @param Attributes Attributes bitmask which caller whants to change.
    @param Set Specifies weathere to set or reset given "Attributes".

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When some of the "Attributes" not supported by the "Dev".

**/
EFI_STATUS EFIAPI DeviceAttributes(PCI_DEV_INFO *Dev, UINT64 Attributes, BOOLEAN Set)
{
	EFI_STATUS		Status=0;
	UINT16			tmp=0;
    PCI_CMD_REG     cmd;
    PCI_BRG_CNT_REG bc;
	PCI_CFG_ADDR	addr;
	UINT64			newattr=0, capab= Dev->Capab;
//------------------------------

	addr.ADDR=Dev->Address.ADDR;
    bc.BRG_CNT_REG=0;
    cmd.CMD_REG=0;

	//If Devices for special attributes has been selected already we can't set it for different device
	if(Dev->HostData->VgaDev != NULL){
        if(Dev != Dev->HostData->VgaDev){
    		if(!InCurVgaChain(Dev) ) capab&=(~PCI_VGA_ATTRIBUTES);
        }
    }

	if((Dev->HostData->PriIde!=NULL)&& (Dev!=Dev->HostData->PriIde))
		capab&=(~EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO);
	if((Dev->HostData->SecIde!=NULL)&& (Dev!=Dev->HostData->SecIde))
		capab&=(~EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO);

    //If OEMs want to add some more filters...
    //Status=PciPortOemAttributes(Dev, &Attributes, capab, Set);
	//if(Dev->AmiSdlPciDevData!=NULL && Dev->AmiSdlPciDevData->InitRoutine)
	{
		UINT64	oldattr=Attributes;
	//-----------------------------
		Status=AmiPciLibLaunchInitRoutine(Dev, isPciSetAttributes, itDevice, Dev, &Attributes, &capab, &Set);
		if(EFI_ERROR(Status)){
			if(Status==EFI_UNSUPPORTED){
				Status=EFI_SUCCESS;
			} else ASSERT_EFI_ERROR(Status);
		} else {
	        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X Overrides Attributes from %X to %X .\n\n",
	            Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function,
	            Dev->DevVenId.VenId, Dev->DevVenId.DevId, oldattr, Attributes));
		}
	}

	//if Attributes requested NOT SUPPORTED by the device...
	if((capab & Attributes)!=Attributes) {
		Status=EFI_UNSUPPORTED;
		goto Exit;
	}
 
	//if requested Attributes already match Device current Attributes
	if( (((Dev->Attrib & Attributes)==Attributes) && Set ) || (((Dev->Attrib & Attributes)==0) && (!Set)) )
	{
		Status=EFI_SUCCESS;
		goto Exit;
	}

	//So far so good - apply attributes
	
	//For IDE controller it is a special case
	if( Attributes & (EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO | EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO)){
		BOOLEAN Primary=(BOOLEAN)(Attributes & EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO);
	//--------------
		Status=SetIdeDevMode(Dev, Primary, Set, NULL);
		if(EFI_ERROR(Status))goto Exit;

		if(Set){
			if(Primary) Dev->HostData->PriIde=Dev;
			else  Dev->HostData->SecIde=Dev;
		} else {
			if(Primary) Dev->HostData->PriIde=NULL;
			else  Dev->HostData->SecIde=NULL;
		}
	}

	//the 
        if(Attributes & EFI_PCI_IO_ATTRIBUTE_ISA_MOTHERBOARD_IO ||
            Attributes & EFI_PCI_IO_ATTRIBUTE_ISA_IO) bc.IsaEnable=1;

        if(Attributes & EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY ||
            Attributes & EFI_PCI_IO_ATTRIBUTE_VGA_IO)
		{
            	DXE_SERVICES		    *dxe;	
                EFI_PHYSICAL_ADDRESS    addr=0x3C0;
            //-----------------------------
               	Status=LibGetDxeSvcTbl(&dxe);
	            if(EFI_ERROR(Status)) return Status;
            
                
                bc.VgaEnable=1;
                if (Dev->DevSetup.VgaPallete && Dev->DevSetup.VgaPallete!=PCI_SETUP_DONT_TOUCH) cmd.VgaPaletteSnoop=1;
                
                if(Set) Status=dxe->AllocateIoSpace(EfiGcdAllocateAddress,
							EfiGcdIoTypeIo,
                            0,
							0x20,
							&addr,
							gPciBusDriverBinding.ImageHandle, 
                            gPciBusDriverBinding.DriverBindingHandle);
                else Status=dxe->FreeIoSpace(addr,0x20);
                if(EFI_ERROR(Status)){
	                PCI_TRACE((TRACE_PCI, "\nPciBus: Fail to allocate/free IO 0x3C0 ~ 0x3DF; Set=%d; Status=%r,\n", Set, Status));
                    //If it was allocated - fine just keep going.
                    Status=EFI_SUCCESS;
                }
        }

        if(Attributes & EFI_PCI_IO_ATTRIBUTE_IO) cmd.IoSpace=1;

        if(Attributes & EFI_PCI_IO_ATTRIBUTE_MEMORY) cmd.MemSpace=1;

        if(Attributes & EFI_PCI_IO_ATTRIBUTE_BUS_MASTER) cmd.BusMaster=1;
	
	//Recoursevely Set attributes to all parents
	//if Parent bridge is the RootBrg use it native function to set Attributes
    //keep in mind that per UIEFI 2.1 Spec Bridges are suppose to be always open,
    //so we will filter Device Enable attributes from setting/resetting them for bridges.
	newattr=(Attributes & (~(EFI_PCI_IO_ATTRIBUTE_IO | EFI_PCI_IO_ATTRIBUTE_MEMORY | EFI_PCI_IO_ATTRIBUTE_BUS_MASTER)));
	newattr&=(~(EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE | EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM ));
	if(Dev->ParentBrg->Type == tPciRootBrg ){
		//remove not supported by rb attributes
		Status=Dev->RbIo->SetAttributes(Dev->RbIo,newattr,NULL,NULL);
	} else 	Status=DeviceAttributes(Dev->ParentBrg,newattr,Set);

	if(EFI_ERROR(Status))goto Exit;

	//bridge control register for P2P and P2C at the same place
	if((Dev->Type==tPci2PciBrg || Dev->Type==tPci2CrdBrg) && bc.BRG_CNT_REG){
		//update bridge control reg if we have to
		addr.Addr.Register=PCI_BRIDGE_CNTL; //brg controll reg
		
		//read what it has
		Status=PciCfg16(Dev->RbIo,addr,FALSE,&tmp);
		if(EFI_ERROR(Status)) goto Exit;

		if(Set) bc.BRG_CNT_REG=bc.BRG_CNT_REG|tmp;
		else bc.BRG_CNT_REG=tmp&(~bc.BRG_CNT_REG);

		//write updated value
		Status=PciCfg16(Dev->RbIo,addr,TRUE,&bc.BRG_CNT_REG);
		if(EFI_ERROR(Status)) goto Exit;
	}

	if(cmd.CMD_REG){
		//update PCI command reg if we have to
		addr.Addr.Register=PCI_CMD; //command reg
		
		//read what it has
		Status=PciCfg16(Dev->RbIo,addr,FALSE,&tmp);
		if(EFI_ERROR(Status)) goto Exit;

		if(Set) cmd.CMD_REG=cmd.CMD_REG|tmp;
		else cmd.CMD_REG=tmp&(~cmd.CMD_REG);

		//write updated value
		Status=PciCfg16(Dev->RbIo,addr,TRUE,&cmd.CMD_REG);
		if(EFI_ERROR(Status)) goto Exit;
	}
	//Update Recorded Attributes
	if(Set)Dev->Attrib|=Attributes;	
	else Dev->Attrib&=(~Attributes);

Exit:
	PCI_TRACE((TRACE_PCI, "\nPciBus: SetAttributes(%d) @ B%X|D%X|F%X Attr=0x%lX; Capab=0x%lX; Status=%r,\n", Set,
	Dev->Address.Addr.Bus,Dev->Address.Addr.Device,Dev->Address.Addr.Function, Attributes, capab, Status));

	return Status;
}


EFI_STATUS EFIAPI UpdateRomVar(PCI_DEV_INFO *Device, ROM_POLICY_VAR  **RomVar){
    ROM_POLICY_VAR  *rp;
    EFI_GUID        RomSetupGuid=ROM_FORM_SET_GUID;
    EFI_STATUS      Status;
//-------------------------    
    if(gRomSetupPtr==NULL) return EFI_NOT_FOUND;
    if( (gRomSetupPtr->OptRomCount + gRomSetupPtr->ExtraRomCount+1 ) > (MAX_NUMBER_OF_ROM_ITEMS)){
        PCI_TRACE((TRACE_PCI, "PciBus: OptROM Policy Var buffer too small - %d Items\n Please update token MAX_NUMBER_OF_ROM_ITEMS\n", MAX_NUMBER_OF_ROM_ITEMS));
        ASSERT_EFI_ERROR(EFI_BUFFER_TOO_SMALL);
        return EFI_BUFFER_TOO_SMALL;
    }
    
    //Found next empty ROM_POLICY_VAR array item
    rp=&gRomSetupPtr->PolicyOption[gRomSetupPtr->OptRomCount+gRomSetupPtr->ExtraRomCount];

    //initialize it with default value...
    rp->Option=AMI_OPTION_ROM_POLICY_UEFI;
    rp->SdlIdx=(UINT32)Device->SdlDevIndex;
    if(RomVar!=NULL) *RomVar=rp;
    
    gRomSetupPtr->ExtraRomCount++;
    
    Status=pRS->SetVariable(L"ROM_CMN",&RomSetupGuid,
        gRomAttributes, 
        sizeof(ROM_SETUP_DATA), gRomSetupPtr);

    return Status;
}

VOID EFIAPI GetRomPolicyVars(VOID){
    UINTN       sz=0;
    EFI_GUID    RomSetupGuid=ROM_FORM_SET_GUID;
    EFI_STATUS  Status;
//-----------------------------    
    
    //Variable MUST be there if OptionRom Dispatch Module is there.
    Status=pRS->GetVariable(L"CSM_CMN",&RomSetupGuid,NULL, &sz, gCsmSetupPtr);
    if(EFI_ERROR(Status)){
        if(Status==EFI_NOT_FOUND) return;
        if(Status==EFI_BUFFER_TOO_SMALL){
            gCsmSetupPtr=MallocZ(sz);
            if(gCsmSetupPtr==NULL) {
                ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
                return;
            }
            
            Status=pRS->GetVariable(L"CSM_CMN",&RomSetupGuid,NULL, &sz, gCsmSetupPtr);
            if(EFI_ERROR(Status)){
                ASSERT_EFI_ERROR(Status);
                return;
            }
        } else {
            ASSERT_EFI_ERROR(Status);
            return;
        }
    }

    //OptRom Module present, get the rest of data;
    sz=sizeof(ROM_SETUP_DATA);
    gRomSetupPtr=MallocZ(sz);
    if(gRomSetupPtr==NULL){
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return;
    }
    
    //Get ROM Policy NVRAM Variable
    Status=pRS->GetVariable(L"ROM_CMN",&RomSetupGuid, &gRomAttributes, &sz, gRomSetupPtr);
    if(EFI_ERROR(Status)){
        if(Status=EFI_NOT_FOUND){
            //OptRom Policy Module is Not Present.. free RomVar Buffer and NULL it down
            if(gRomSetupPtr!=NULL) pBS->FreePool(gRomSetupPtr);
            gRomSetupPtr=NULL;
        } else {
            ASSERT_EFI_ERROR(Status);
        }
    }
    return;
}

EFI_STATUS EFIAPI AmiOptRomGetPolicy(
    IN      AMI_OPTION_ROM_POLICY_PROTOCOL          *This,
    OUT     AMI_OPTION_ROM_POLICY_TYPE              *Policy,
    OUT OPTIONAL BOOLEAN                            *RomStatus
    )
{
    PCI_DEV_INFO    *dev=((AMI_ROM_POLICY_DATA*)This)->Owner;
//------------------------------------    
    if(This==NULL || Policy==NULL || dev->Signature!=AMI_PCI_SIG) return EFI_INVALID_PARAMETER;
    
    *Policy=((AMI_ROM_POLICY_DATA*)This)->CurrentPolicy;
    
    if(RomStatus!=NULL) *RomStatus=((AMI_ROM_POLICY_DATA*)This)->RomStatus;
    
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI AmiOptRomSetPolicy(
    IN      AMI_OPTION_ROM_POLICY_PROTOCOL           *This,
    IN      AMI_OPTION_ROM_POLICY_TYPE               NewPolicy )
{
    PCI_DEV_INFO    *dev=((AMI_ROM_POLICY_DATA*)This)->Owner;
//------------------------------------    
    if(This==NULL || dev->Signature!=AMI_PCI_SIG) return EFI_INVALID_PARAMETER;
    
    ((AMI_ROM_POLICY_DATA*)This)->CurrentPolicy=NewPolicy;

    return EFI_SUCCESS;
}

EFI_STATUS CreateRomPolicyData(PCI_DEV_INFO   *Dev, ROM_POLICY_VAR  *RomVar){
    
    Dev->AmiRomPolicy=MallocZ(sizeof(AMI_ROM_POLICY_DATA));
    if(Dev->AmiRomPolicy==NULL){
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return EFI_OUT_OF_RESOURCES;
    }
    
    Dev->AmiRomPolicy->CurrentPolicy=RomVar->Option;
    Dev->AmiRomPolicy->OptRomProtocol.SetOptionRomPolicy=AmiOptRomSetPolicy;
    Dev->AmiRomPolicy->OptRomProtocol.GetOptionRomPolicy=AmiOptRomGetPolicy;
    Dev->AmiRomPolicy->Owner=Dev;
    Dev->AmiRomPolicy->BaseClass=Dev->Class.BaseClassCode;
    
    if(Dev->AmiSdlPciDevData!=NULL && Dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard){
        if(Dev->AmiSdlPciDevData->PciDevFlags.Bits.StdPciRom)Dev->AmiRomPolicy->RomStatus|=(AMI_OPTION_ROM_POLICY_LEGACY);
        if(Dev->AmiSdlPciDevData->PciDevFlags.Bits.UefiDriverBin)Dev->AmiRomPolicy->RomStatus|=(AMI_OPTION_ROM_POLICY_UEFI);
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI CheckInitRomPolicy(PCI_DEV_INFO   *Dev){
    UINTN           i;
    EFI_STATUS      Status=EFI_NOT_FOUND;
//----------------
    //1. No policy in place - bussines as usual...
    if(gRomSetupPtr==NULL) return EFI_SUCCESS;
    
    //2. We coming here only if Device has SDL Data
    //3. Check if Device SdlIndex recorded in Rom Policy Nvram VAR.
    for(i=0;i<gRomSetupPtr->OptRomCount; i++){
            ROM_POLICY_VAR  *rp = &gRomSetupPtr->PolicyOption[i];
    //-----------------------------------
            if(rp->SdlIdx==Dev->SdlDevIndex){
                Status=CreateRomPolicyData(Dev, rp);
                ASSERT_EFI_ERROR(Status);
                break;
            }
    }
    
    return Status;
}


/**
    Protocol Function Poll PCI Memmory

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoPollMem(IN  EFI_PCI_IO_PROTOCOL        *This,
						IN  EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						IN  UINT8                      BarIndex,
						IN  UINT64                     Offset,
						IN  UINT64                     Mask,
						IN  UINT64                     Value,
						IN  UINT64                     Delay,
						OUT UINT64                     *Result)
{
	EFI_STATUS    Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//----------------------------------------------------------------------

	if (((UINT32)Width) > ((UINT32)EfiPciIoWidthUint64)) return EFI_INVALID_PARAMETER;

	Status = PciIoCheckBar(dev, BarIndex, tBarMem, Width, 1, &Offset);
	if(EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	Status=dev->RbIo->PollMem(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,Offset,
		Mask,Value,Delay,Result);
	return Status;
}


/**
    Protocol Function Poll PCI IO

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoPollIo (IN  EFI_PCI_IO_PROTOCOL        *This,
						IN  EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						IN  UINT8                      BarIndex,
						IN  UINT64                     Offset,
						IN  UINT64                     Mask,
						IN  UINT64                     Value,
						IN  UINT64                     Delay,
						OUT UINT64                     *Result )
{
	EFI_STATUS		Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//----------------------------------
	if (((UINT32)Width) > ((UINT32)EfiPciIoWidthUint64)) return EFI_INVALID_PARAMETER;
 
	Status=PciIoCheckBar(dev, BarIndex, tBarIo, Width, 1, &Offset);
	if(EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	if (Width > EfiPciIoWidthUint64) return EFI_INVALID_PARAMETER;

	Status=dev->RbIo->PollIo(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                             Offset, Mask, Value, Delay, Result);
	return Status;
}

/**
    Protocol Function Performs a PCI Memory Read Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoMemRead(IN EFI_PCI_IO_PROTOCOL        *This,
						IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						IN UINT8                      BarIndex,
						IN UINT64                     Offset,
						IN UINTN                      Count,
						IN OUT VOID                   *Buffer)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------

	if (Width < 0 || Width >= EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;

	Status=PciIoCheckBar(dev, BarIndex, tBarMem, Width, Count, &Offset);
	if(EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	Status=dev->RbIo->Mem.Read(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
							   Offset, Count, Buffer);
	
	return Status;
}


/**
    Protocol Function Performs a PCI Memory Write Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoMemWrite(IN EFI_PCI_IO_PROTOCOL        *This,
						 IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						 IN UINT8                      BarIndex,
						 IN UINT64                     Offset,
						 IN UINTN                      Count,
						 IN OUT VOID                   *Buffer)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//-------------------------------------------
  
	if(Width<0 || Width>=EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;

	Status = PciIoCheckBar(dev, BarIndex, tBarMem, Width, Count, &Offset);
	if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
  
	Status=dev->RbIo->Mem.Write(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                Offset, Count, Buffer);

	return Status;
}


/**
    Protocol Function Performs a PCI I/O Read Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoIoRead(IN EFI_PCI_IO_PROTOCOL        *This,
					   IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
					   IN UINT8                      BarIndex,
					   IN UINT64                     Offset,
					   IN UINTN                      Count,
					   IN OUT VOID                   *Buffer)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//----------------------------------------------------

	if (Width < 0 || Width >= EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;

	Status=PciIoCheckBar(dev, BarIndex, tBarIo, Width, Count, &Offset);
	if (EFI_ERROR(Status))return EFI_UNSUPPORTED;
  
	Status=dev->RbIo->Io.Read(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                              Offset, Count, Buffer);
	
	return Status;
}


/**
    Protocol Function Performs a PCI I/O Write Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoIoWrite(IN EFI_PCI_IO_PROTOCOL        *This,
						IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						IN UINT8                      BarIndex,
						IN UINT64                     Offset,
						IN UINTN                      Count,
						IN OUT VOID                   *Buffer)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//----------------------------------------------------

	if (Width < 0 || Width >= EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;
  
	Status=PciIoCheckBar(dev, BarIndex, tBarIo, Width, Count, &Offset);
	if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	Status=dev->RbIo->Io.Write(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                               Offset, Count, Buffer);

	return Status;
}


/**
    Protocol Function Performs a PCI Configuration Read Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoConfigRead(IN EFI_PCI_IO_PROTOCOL        *This,
						   IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
						   IN UINT32                     Offset,
						   IN UINTN                      Count,
						   IN OUT VOID                   *Buffer)
{
  
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
	UINT64         Address;
//---------------------------------------------------
	if(Buffer==NULL) return EFI_INVALID_PARAMETER;
	Address = Offset;
	Status = PciIoCheckConfig(dev, Width, Count, &Address);
	if(EFI_ERROR(Status))return Status;

	Status=dev->RbIo->Pci.Read(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
							   Address, Count, Buffer);
	return Status;
}


/**
    Protocol Function Performs a PCI Configuration Write Cycle

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoConfigWrite(IN EFI_PCI_IO_PROTOCOL        *This,
							IN EFI_PCI_IO_PROTOCOL_WIDTH  Width,
							IN UINT32                     Offset,
							IN UINTN                      Count,
							IN OUT VOID                   *Buffer)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
	UINT64         Address;
//---------------------------------------------------
    if(Buffer==NULL) return EFI_INVALID_PARAMETER;
	Address = Offset;
	Status = PciIoCheckConfig(dev, Width, Count, &Address);
	if (EFI_ERROR(Status)) return Status;
	
	Status=dev->RbIo->Pci.Write(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
							   Address, Count, Buffer);
	return Status;
}


/**
    Protocol Function Copyes PCI Memory

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoCopyMem(IN EFI_PCI_IO_PROTOCOL			*This,
						IN EFI_PCI_IO_PROTOCOL_WIDTH    Width,
						IN UINT8                        DestBarIndex,
						IN UINT64                       DestOffset,
						IN UINT8                        SrcBarIndex,
						IN UINT64                       SrcOffset,
						IN UINTN                        Count)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------------

	if (Width < 0 || Width >= EfiPciIoWidthMaximum) return EFI_INVALID_PARAMETER;

	if(Width == EfiPciIoWidthFifoUint8     ||
	   Width == EfiPciIoWidthFifoUint16    ||
	   Width == EfiPciIoWidthFifoUint32    ||
	   Width == EfiPciIoWidthFifoUint64    ||
       Width == EfiPciIoWidthFillUint8     ||
       Width == EfiPciIoWidthFillUint16    ||
       Width == EfiPciIoWidthFillUint32    ||
       Width == EfiPciIoWidthFillUint64) return EFI_INVALID_PARAMETER;

	Status=PciIoCheckBar(dev, DestBarIndex, tBarMem, Width, Count, &DestOffset);
	if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
  
	Status=PciIoCheckBar(dev, SrcBarIndex, tBarMem, Width, Count, &SrcOffset);
	if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
  
	Status=dev->RbIo->CopyMem(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                            DestOffset, SrcOffset, Count);
                                           
	return Status;
}


/**
    Protocol Function Maps a memory region for DMA

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoMap(IN EFI_PCI_IO_PROTOCOL            *This,
					IN EFI_PCI_IO_PROTOCOL_OPERATION  Operation,
					IN VOID                           *HostAddress,
					IN OUT UINTN                      *NumberOfBytes,
					OUT EFI_PHYSICAL_ADDRESS          *DeviceAddress,
					OUT VOID                          **Mapping)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------------

	if(Operation<0 || Operation>=EfiPciIoOperationMaximum) return EFI_INVALID_PARAMETER;

    if(HostAddress==NULL || NumberOfBytes==NULL || DeviceAddress==NULL || Mapping==NULL)
		return EFI_INVALID_PARAMETER;

	if (dev->Attrib & EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE) 
		Operation = Operation + EfiPciOperationBusMasterRead64;
  
	Status=dev->RbIo->Map(dev->RbIo,(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION)Operation,
                          HostAddress, NumberOfBytes, DeviceAddress, Mapping);

    return Status;
}


/**
    Protocol Function Unmaps a memory region for DMA

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoUnmap(IN EFI_PCI_IO_PROTOCOL  *This,
					  IN VOID                 *Mapping)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------------

	Status=dev->RbIo->Unmap(dev->RbIo,Mapping);
                                        
	return Status;
}


/**
    Protocol Function Allocates a common buffer for DMA

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoAllocateBuffer(IN  EFI_PCI_IO_PROTOCOL   *This,
							   IN  EFI_ALLOCATE_TYPE     Type,
							   IN  EFI_MEMORY_TYPE       MemoryType,
							   IN  UINTN                 Pages,
							   OUT VOID                  **HostAddress,
							   IN  UINT64                Attributes)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------------
  
	if (Attributes & (~(EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE | EFI_PCI_ATTRIBUTE_MEMORY_CACHED))) 
		return EFI_UNSUPPORTED;
  
	if (dev->Attrib & EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE) 
		Attributes|=EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE;
  
	Status=dev->RbIo->AllocateBuffer(dev->RbIo,Type,MemoryType,Pages,HostAddress,Attributes);

	return Status;
}


/**
    Protocol Function Frees a common buffer 

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoFreeBuffer(IN  EFI_PCI_IO_PROTOCOL   *This,
						   IN  UINTN                 Pages,
						   IN  VOID                  *HostAddress)
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//---------------------------------------------------
	
	Status=dev->RbIo->FreeBuffer(dev->RbIo,Pages,HostAddress);
	return Status;
}


/**
    Protocol Function Flushes a DMA buffer 

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoFlush (IN  EFI_PCI_IO_PROTOCOL  *This)
{
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//--------------------------------------------
	return dev->RbIo->Flush(dev->RbIo);
}


/**
    Protocol Function Proides Device Address on PCI BUS like
    Bus#, Dev#, Fun# for the device who has PciIoProtocol == "This".

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoGetLocation(IN  EFI_PCI_IO_PROTOCOL  *This,
							OUT UINTN                *Segment,
							OUT UINTN                *Bus,
							OUT UINTN                *Device,
							OUT UINTN                *Function)
{
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
//--------------------------------------------

	if(Segment==NULL || Bus==NULL || Device==NULL || Function==NULL)
		return EFI_INVALID_PARAMETER;
  
	*Segment  = dev->RbIo->SegmentNumber;
	*Bus      = dev->Address.Addr.Bus;
	*Device   = dev->Address.Addr.Device;
	*Function = dev->Address.Addr.Function;

	return EFI_SUCCESS;
}


/**
    Protocol Function Provides Arttribute operation for the PCI device

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoAttributes(IN EFI_PCI_IO_PROTOCOL						*This,
						   IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION  Operation,
						   IN  UINT64                                   Attributes,
						   OUT UINT64                                   *Result   OPTIONAL )
{
	EFI_STATUS     Status;
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
	BOOLEAN 		f=FALSE;	
	UINT64			capab=dev->Capab;
//--------------------------------------------

	switch (Operation) {
		case EfiPciIoAttributeOperationGet:
		case EfiPciIoAttributeOperationSupported:
			if (Result == NULL) return EFI_INVALID_PARAMETER;
			if(Operation==EfiPciIoAttributeOperationGet)*Result = dev->Attrib;
			else {

            	if(dev->HostData->VgaDev != NULL){
                    if(dev != dev->HostData->VgaDev){
    		            if(!InCurVgaChain(dev) ) capab&=(~PCI_VGA_ATTRIBUTES);
                    }
                }
				*Result=capab; 
			}
			return EFI_SUCCESS;
			break;
		case EfiPciIoAttributeOperationSet:
		case EfiPciIoAttributeOperationEnable:
			//only one VGA legacy resources may be decoded
			if(Attributes&PCI_VGA_ATTRIBUTES){ //special case for VGA attr
				if(dev->HostData->VgaDev==NULL)f=TRUE;
            }
				//else if (dev!=dev->HostData->VgaDev) return EFI_UNSUPPORTED;
			Status=DeviceAttributes(dev, Attributes,TRUE);
			if((!EFI_ERROR(Status))&& f) dev->HostData->VgaDev=dev; 
			break;
		case EfiPciIoAttributeOperationDisable:      
			if(Attributes&PCI_VGA_ATTRIBUTES) {//special case for VGA attr
                if(dev->HostData->VgaDev==dev)f=TRUE;       
            }
			Status=DeviceAttributes(dev, Attributes,FALSE);
			if((!EFI_ERROR(Status))&& f ) dev->HostData->VgaDev=NULL; //clear default VGA Device ptr somebody reset it's status 
			break;
		default : return EFI_INVALID_PARAMETER;
	}//switch
  return Status;
}

/**
    Helper function that returns Translation Offset for selected 
    PCI_BAR_TYPE register.

    @note  This Function Assumes that PCI_RB_IO protocol ->Configuration()
    function returns correctly Filled _TRA field for selected resource type.

**/
UINT64  GetTranslationOffset(PCI_DEV_INFO   *Dev, UINT64 Gra, UINT8 AslType, UINT8 AslTFlags){
    ASLR_QWORD_ASD  *res;
    EFI_STATUS      Status;
//------------------
    Status=Dev->RbIo->Configuration(Dev->RbIo, (VOID*)&res);
    ASSERT_EFI_ERROR(Status);
    
    while(res->Hdr.HDR!=ASLV_END_TAG_HDR){
        
        if((res->Type==AslType) && ((res->TFlags.TFLAGS & AslTFlags) == AslTFlags))
            if(AslType==ASLRV_SPC_TYPE_IO){
                return res->_TRA;
            } else {
                if (res->_GRA == Gra) return res->_TRA;                
            }    
        res++;
    }
    
    return 0;
}


/**
    Protocol Function Gets respective BAR Attributes

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoGetBarAttributes(IN EFI_PCI_IO_PROTOCOL		*This,
								 IN UINT8                   BarIndex,
								 OUT UINT64                 *Supports,   OPTIONAL
								 OUT VOID                   **Resources  OPTIONAL )
{
	PCI_DEV_INFO	*dev=(PCI_DEV_INFO*)This;
	ASLR_QWORD_ASD	*resdsc;
//--------------------------------------------
 
	if(Supports==NULL && Resources == NULL) return EFI_INVALID_PARAMETER;
 	if(BarIndex>=MAX_PCI_BAR) return EFI_UNSUPPORTED;
  
	if(Supports!=NULL) {
		*Supports = (dev->Capab & (BAR_ATTR));
	}
	if(Resources != NULL) {
		ASLR_EndTag	*enddsc;
		UINTN		sz=sizeof(ASLR_EndTag);
	//------------------------------------------------	
		if(dev->Bar[BarIndex].Type!=tBarUnused)sz+=sizeof(ASLR_QWORD_ASD);

		resdsc=MallocZ(sz);
		if(!resdsc) return EFI_OUT_OF_RESOURCES;


		if(dev->Bar[BarIndex].Type!=tBarUnused){

			enddsc=(ASLR_EndTag*)(resdsc+1);
	
			//fill BAR descriptor
			resdsc->Hdr.Name=ASLV_RT_QWORD_ASD;
			resdsc->Hdr.Type=ASLV_LARGE_RES;
			resdsc->Hdr.Length=sizeof(ASLR_QWORD_ASD)-sizeof(ASLRF_L_HDR);
			resdsc->_MIN=dev->Bar[BarIndex].Base;
			resdsc->_LEN=dev->Bar[BarIndex].Length;
			resdsc->_MAX=resdsc->_MIN+resdsc->_LEN-1;
			
			switch (dev->Bar[BarIndex].DiscoveredType) {
				case tBarIo16:
                    resdsc->Type=ASLRV_SPC_TYPE_IO;         // Io             
                    resdsc->_GRA=16;                        // 32 bit
                    break;
				case tBarIo32:       
					resdsc->Type=ASLRV_SPC_TYPE_IO;			// Io             
                    resdsc->_GRA=32;                        // 32 bit
					break;
				case tBarMmio32:
					resdsc->Type=ASLRV_SPC_TYPE_MEM;		// Mem
					resdsc->_GRA=32;						// 32 bit
					break;
				case tBarMmio32pf:
	   				resdsc->Type=ASLRV_SPC_TYPE_MEM;		// Mem             
					resdsc->TFlags.TFLAGS=0x6;				// prefechable
					resdsc->_GRA=32;						// 32 bit
					break;
				case tBarMmio64:
					resdsc->Type=ASLRV_SPC_TYPE_MEM;		// Mem             
	   		        resdsc->_GRA=64;						// 64 bit
		    	    break;
				case tBarMmio64pf:
					resdsc->Type=ASLRV_SPC_TYPE_MEM;		// Mem             
					resdsc->TFlags.TFLAGS=0x6;				// prefechable
					resdsc->_GRA=64;						// 64 bit
					break;
				default:
					break ; 
			}//switch
			resdsc->_TRA=GetTranslationOffset(dev, resdsc->_GRA, resdsc->Type, resdsc->TFlags.TFLAGS);
		} else enddsc=(ASLR_EndTag*)(resdsc);
		//fix End tag
		enddsc->Hdr.Name=ASLV_SR_EndTag;
		enddsc->Hdr.Type=ASLV_SMALL_RES;
		enddsc->Hdr.Length=sizeof(ASLR_EndTag)-sizeof(ASLRF_S_HDR);
		enddsc->Chsum=0;
		*Resources = resdsc;
	}//if(Resources!=NULL) 

	return EFI_SUCCESS;
}


/**
    Protocol Function Sets respective BAR Attributes

    @note  See EFI Specification for detail description

**/
EFI_STATUS EFIAPI PciIoSetBarAttributes(IN EFI_PCI_IO_PROTOCOL		*This,
								 IN UINT64                  Attributes,
								 IN UINT8                   BarIndex,
								 IN OUT UINT64              *Offset,
								 IN OUT UINT64              *Length)
{
	EFI_STATUS			Status;
	PCI_DEV_INFO		*dev=(PCI_DEV_INFO*)This;
	UINT64				offs, attr;
//--------------------------------------------------

	if(Offset==NULL || Length==NULL) return EFI_INVALID_PARAMETER;
  
	if(dev->Bar[BarIndex].Type==tBarUnused) return EFI_UNSUPPORTED;
 
	// This driver does not support setting the WRITE_COMBINE or the CACHED attributes.
	// If Attributes is not 0, then return EFI_UNSUPPORTED.
	attr = Attributes & (~BAR_ATTR);
	if (attr)return EFI_UNSUPPORTED;
 
	//Make sure the BAR range describd by BarIndex, Offset, and Length are valid for this PCI device.
	offs = *Offset; 
	Status=PciIoCheckBar(dev, BarIndex, tBarMem, EfiPciIoWidthUint8, (UINT32)*Length, &offs);

	if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	return Status;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//============================================================================
// Pci Io Protocol Interface
//============================================================================
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    PCI IO Protocol Instance for Child Devices of the PciBus Driver.

    @note  EFI_PCI_IO_PROTOCOL

**/
static EFI_PCI_IO_PROTOCOL gPciIoInstance = {
    PciIoPollMem,
    PciIoPollIo, 
    {
        PciIoMemRead,  
        PciIoMemWrite
    },
    {
        PciIoIoRead,   
        PciIoIoWrite
    },
    {
        PciIoConfigRead, 
        PciIoConfigWrite
    },
    PciIoCopyMem,
    PciIoMap,
    PciIoUnmap,
    PciIoAllocateBuffer,
    PciIoFreeBuffer,
    PciIoFlush,
    PciIoGetLocation,
    PciIoAttributes,
    PciIoGetBarAttributes,
    PciIoSetBarAttributes,
    0,                      //RomSize;
    NULL                    //RomImage
};


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//======================================================================
// Here follows worker functions used for 
// PCI Bus Enumeration and Resource Allocation
//======================================================================
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    Enables PCI Device Decoding.

        
    @param Dev Pointer to PCI Device Private Data structure.
    @param WhatSpace Type of PCI Device Space for Action.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_SPACE_TYPE.

**/
EFI_STATUS EFIAPI EnableDeviceDecoding(PCI_DEV_INFO *Dev, PCI_SPACE_TYPE WhatSpace)
{
	EFI_STATUS		Status;
	UINT16			b16;
	PCI_CFG_ADDR	addr;
//------------------
	
	if(Dev->Type<tPci2PciBrg) return EFI_SUCCESS;
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
	if(Dev->DebugPort) return EFI_SUCCESS;
#endif

	addr.ADDR=Dev->Address.ADDR;
	//first read Command reg contents
	addr.Addr.Register=PCI_COMMAND_REGISTER_OFFSET;
	Status=PciCfg16(Dev->RbIo,addr,FALSE,&b16);
	if(EFI_ERROR(Status)) return Status;

	switch (WhatSpace){
		case stOptRomSpace:
		case stMemSpace: b16|=(PCI_CMD_MEMORY_SPACE); break;
		case stIoSpace:  b16|=(PCI_CMD_IO_SPACE); break;
		case stMemIoSpace: b16|=(PCI_CMD_IO_SPACE | PCI_CMD_MEMORY_SPACE); break;
		default: return EFI_INVALID_PARAMETER;
	} //switch
	//if Enabling PCI ROM space
	if(WhatSpace==stOptRomSpace && Dev->Bar[PCI_MAX_BAR_NO].Type!=tBarUnused){
		UINT32		b32;
	//----------------------	
		addr.Addr.Register=Dev->Bar[PCI_MAX_BAR_NO].Offset;

		Status=PciCfg32(Dev->RbIo,addr,FALSE,&b32);
		if(EFI_ERROR(Status)) return Status;

		b32|=1;
		Status=PciCfg32(Dev->RbIo,addr,TRUE,&b32);
		if(EFI_ERROR(Status)) return Status;
	}

	//Write Data to the Command Register
	addr.Addr.Register=PCI_COMMAND_REGISTER_OFFSET;
	return PciCfg16(Dev->RbIo,addr,TRUE,&b16);
}


/**
    Disables PCI Device Decoding.

        
    @param Dev Pointer to PCI Device Private Data structure.
    @param WhatSpace Type of PCI Device Space for Action.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_SPACE_TYPE.

**/
EFI_STATUS EFIAPI DisableDeviceDecoding(PCI_DEV_INFO *Dev, PCI_SPACE_TYPE WhatSpace)
{
	EFI_STATUS		Status;
	UINT16			b16;
	PCI_CFG_ADDR	addr;
//------------------
	//Don't toch Host brg device as a PCI Device
	if(Dev->Type==tPciHostDev || Dev->Type==tUncompatibleDevice) return EFI_SUCCESS;
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
	if(Dev->DebugPort) return EFI_SUCCESS;
#endif
 
	addr.ADDR=Dev->Address.ADDR;
	//first read Command reg contents
	addr.Addr.Register=PCI_COMMAND_REGISTER_OFFSET;
	Status=PciCfg16(Dev->RbIo,addr,FALSE,&b16);
	if(EFI_ERROR(Status)) return Status;

	switch (WhatSpace){
		case stOptRomSpace:
		case stMemSpace: 
            b16 &= (~PCI_CMD_MEMORY_SPACE); 
            break;
		case stIoSpace:  
            b16 &= (~PCI_CMD_IO_SPACE); 
            break;
		case stMemIoSpace: 
            b16 &= (~(PCI_CMD_IO_SPACE | PCI_CMD_MEMORY_SPACE)); 
            break;
		case stDisableAll: 
            b16 &= (~(PCI_CMD_IO_SPACE | PCI_CMD_MEMORY_SPACE | PCI_CMD_BUS_MASTER)); 
            break;
		default: return EFI_INVALID_PARAMETER;
	} //switch

	//if Disabling PCI ROM space
	if( (WhatSpace == stOptRomSpace ) && (Dev->Bar[PCI_MAX_BAR_NO].Type!=tBarUnused)){
		UINT32		b32;
	//----------------------	
		addr.Addr.Register=Dev->Bar[PCI_MAX_BAR_NO].Offset;

		Status=PciCfg32(Dev->RbIo,addr,FALSE,&b32);
		if(EFI_ERROR(Status)) return Status;

		b32&=(~1);
		Status=PciCfg32(Dev->RbIo,addr,TRUE,&b32);
		if(EFI_ERROR(Status)) return Status;
	}
	
	//Write Data to the Command Register
	addr.Addr.Register=PCI_COMMAND_REGISTER_OFFSET;
	Status=PciCfg16(Dev->RbIo,addr,TRUE,&b16);
	
	return Status;
}


/**
    Enables PCI Bridge I/O Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.
    @param Base Base Address of the Bridge I/O Window.
    @param Length Length of the Bridge I/O Window.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO.

**/
EFI_STATUS EFIAPI EnableBridgeIoDecoding(PCI_DEV_INFO *Brg, UINT64 Base, UINT64 Length)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT64			buff;
	EFI_STATUS		Status;
//----------------------------------------
    PCI_TRACE((TRACE_PCI,"PciBus: Enabling Brg I/O  @ [B%X|D%X|F%X] --> Offs 0x1C\tB=%lX; L=%lX\n", 
	Brg->Address.Addr.Bus, Brg->Address.Addr.Device,Brg->Address.Addr.Function,
    Base, Length));

    if (Base==0 && Length==0){
        PCI_TRACE((TRACE_PCI,"  IO     Base == 0 keep This Bridge closed!\n")); 
        return EFI_SUCCESS;
    }

    BrgDevAddr.ADDR=Brg->Address.ADDR;
	//Set IObase to 0xFF and IO limit to 0
	BrgDevAddr.Addr.Register=PCI_IOBASE; 
	buff=Shr64(Base,8);

	Status = PciCfg8(Brg->RbIo, BrgDevAddr,TRUE,(UINT8*)&buff);
	if(EFI_ERROR(Status))return Status;

	buff=Shr64(Base,16); 
	BrgDevAddr.Addr.Register=PCI_IOBASE_U; //Upper 16 Base Reg 

	Status = PciCfg16(Brg->RbIo, BrgDevAddr,TRUE,(UINT16*)&buff);
	if(EFI_ERROR(Status))return Status;

	//Set IO limit 
	buff=Shr64(Base+(Length-1),8);
	BrgDevAddr.Addr.Register=PCI_IOLIMIT; 

	Status = PciCfg8(Brg->RbIo, BrgDevAddr,TRUE,(UINT8*)&buff);
	if(EFI_ERROR(Status))return Status;

	//Set IO limit  Upper 16
	buff=Shr64(Base+(Length-1),16);
	BrgDevAddr.Addr.Register=PCI_IOLIMIT_U; 

	return PciCfg16(Brg->RbIo, BrgDevAddr,TRUE,(UINT16*)&buff);
}


/**
    Disables PCI Bridge I/O Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS EFIAPI DisableBridgeIoDecoding(PCI_BRG_INFO *Brg)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT32			buff=0x00FF;
	EFI_STATUS		Status;
//----------------------------------------
	BrgDevAddr.ADDR=Brg->Common.Address.ADDR;
	//Set IObase to 0xFF and IO limit to 0
	BrgDevAddr.Addr.Register=PCI_IOBASE; 
	Status = PciCfg16(Brg->Common.RbIo, BrgDevAddr,TRUE,(UINT16*)&buff);
	if(EFI_ERROR(Status))return Status;

	//Set IO limit upper 32 base and limit to 0
	BrgDevAddr.Addr.Register=PCI_IOBASE_U; 
	return PciCfg32(Brg->Common.RbIo, BrgDevAddr,TRUE,&buff);
}


/**
    Enables PCI Bridge MMIO Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.
    @param Base Base Address of the Bridge I/O Window.
    @param Length Length of the Bridge I/O Window.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO.

**/
EFI_STATUS EFIAPI EnableBridgeMmioDecoding(PCI_DEV_INFO *Brg, UINT64 Base, UINT64 Length)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT64			buff;
	EFI_STATUS		Status;
//----------------------------------------
    PCI_TRACE((TRACE_PCI,"PciBus: Enabling Brg MMIO @ [B%X|D%X|F%X] --> Offs 0x20;\tB=%lX; L=%lX\n", 
	Brg->Address.Addr.Bus, Brg->Address.Addr.Device,Brg->Address.Addr.Function,
    Base, Length));

    if (Base==0 && Length==0){
        PCI_TRACE((TRACE_PCI,"  MMIO   Base == 0 keep This Bridge closed!\n")); 
        return EFI_SUCCESS;
    }
    
	BrgDevAddr.ADDR=Brg->Address.ADDR;
	//Set Memory Base 
	BrgDevAddr.Addr.Register=PCI_MEMBASE; 
	//buff=Base>>16;
	buff=Shr64(Base,16);	

	Status=PciCfg16(Brg->RbIo, BrgDevAddr,TRUE,(UINT16*)&buff);
	if(EFI_ERROR(Status))return Status;

	//Set Memory limt
	BrgDevAddr.Addr.Register=PCI_MEMLIMIT;
	buff=Shr64(Base+(Length-1),16);
	return 	PciCfg16(Brg->RbIo, BrgDevAddr,TRUE,(UINT16*)&buff);
}


/**
    Disables PCI Bridge MMIO Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS EFIAPI DisableBridgeMmioDecoding(PCI_BRG_INFO *Brg)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT32			buff=0x0000FFFF;
//----------------------------------------
	BrgDevAddr.ADDR=Brg->Common.Address.ADDR;
	//Set Memory Base to FFFF and Limit to 0;
	BrgDevAddr.Addr.Register=PCI_MEMBASE; 
	return PciCfg32(Brg->Common.RbIo, BrgDevAddr,TRUE,&buff);
}


/**
    Enables PCI Bridge Prefetchable MMIO Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.
    @param Base Base Address of the Bridge I/O Window.
    @param Length Length of the Bridge I/O Window.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO.

**/
EFI_STATUS EFIAPI EnableBridgePfmmDecoding(PCI_DEV_INFO *Brg, UINT64 Base, UINT64 Length)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT64			buff;
	EFI_STATUS		Status;
//----------------------------------------
    PCI_TRACE((TRACE_PCI,"PciBus: Enabling Brg PFMM @ [B%X|D%X|F%X] --> Offs 0x24;\tB=%lX; L=%lX\n", 
	Brg->Address.Addr.Bus, Brg->Address.Addr.Device,Brg->Address.Addr.Function,
    Base, Length));

    if (Base==0 && Length==0){
        PCI_TRACE((TRACE_PCI,"  PFMMIO Base == 0 keep This Bridge closed!\n")); 
        return EFI_SUCCESS;
    }

	BrgDevAddr.ADDR=Brg->Address.ADDR;

	//Set Pf Memory Base 
	BrgDevAddr.Addr.Register=PCI_PRE_MEMBASE; 
	buff=Shr64(Base,16);
	
	Status = PciCfg16(Brg->RbIo, BrgDevAddr, TRUE,(UINT16*)&buff);
	if(EFI_ERROR(Status))return Status;

	//Set PF Memory Limit
	BrgDevAddr.Addr.Register=PCI_PRE_MEMLIMIT; 
	buff=Shr64(Base+(Length-1),16);
	
	Status = PciCfg16(Brg->RbIo, BrgDevAddr, TRUE, (UINT16*)&buff);
	if(EFI_ERROR(Status))return Status;

	buff=Shr64(Base,32);
	//Set Pf Memory Upper 32 Base
	BrgDevAddr.Addr.Register=PCI_PRE_MEMBASE_U; 
	Status = PciCfg32(Brg->RbIo, BrgDevAddr,TRUE,(UINT32*)&buff);
	if(EFI_ERROR(Status))return Status;

	buff=Shr64(Base+(Length-1),32);
	//Set Pf Memory Upper 32 Limit 
    BrgDevAddr.Addr.Register=PCI_PRE_MEMLIMIT_U; 
	return PciCfg32(Brg->RbIo, BrgDevAddr,TRUE,(UINT32*)&buff);
}


/**
    Disables PCI Bridge Prefetchable MMIO Space Decoding.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS EFIAPI DisableBridgePfmmDecoding(PCI_BRG_INFO *Brg)
{
	PCI_CFG_ADDR	BrgDevAddr;
	UINT32			buff=0x0000FFFF;
	EFI_STATUS		Status;
//----------------------------------------
	BrgDevAddr.ADDR=Brg->Common.Address.ADDR;

	//Set Pf Memory Base to FFFF Limit to 0;
	BrgDevAddr.Addr.Register=PCI_PRE_MEMBASE; 
	Status = PciCfg32(Brg->Common.RbIo, BrgDevAddr,TRUE,&buff);
	if(EFI_ERROR(Status))return Status;

	buff=0;
	//Set Pf Memory Upper 32 Base to 0;
	BrgDevAddr.Addr.Register=PCI_PRE_MEMBASE_U; 
	Status = PciCfg32(Brg->Common.RbIo, BrgDevAddr,TRUE,&buff);
	if(EFI_ERROR(Status))return Status;

	//Set Pf Memory Upper 32 Limit to 0; 
    BrgDevAddr.Addr.Register=PCI_PRE_MEMLIMIT_U; 
	return PciCfg32(Brg->Common.RbIo, BrgDevAddr,TRUE,&buff);
}


/**
    Disables PCI Bridge Decoding of ALL resources.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS EFIAPI DisableBridgeDecoding(PCI_BRG_INFO *Brg)
{
	EFI_STATUS		Status;
//---------------------------------------
	
    PCI_TRACE((TRACE_PCI,"PciBus: Disabling Brg @ B%X|D%X|F%X\n", 
	Brg->Common.Address.Addr.Bus, Brg->Common.Address.Addr.Device,Brg->Common.Address.Addr.Function));
	Status=DisableBridgeIoDecoding(Brg);
	if(EFI_ERROR(Status))return Status;

	Status=DisableBridgeMmioDecoding(Brg);
	if(EFI_ERROR(Status))return Status;

	return DisableBridgePfmmDecoding(Brg);
}

/**
    Finds corresponded BusXlatTable entry for the bridge.

        
    @param Dev Pointer to PCI Bridge Private Data structure.
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

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
EFI_STATUS EFIAPI FindSdlEntry(PCI_DEV_INFO *Dev){
	EFI_STATUS					Status=EFI_SUCCESS;
	PCI_DEV_INFO				*parent=Dev->ParentBrg;
	AMI_SDL_PCI_DEV_INFO		**chldbuff=NULL;
	AMI_SDL_PCI_DEV_INFO		*tmp;
	UINTN						cnt=0,i,hit;
	UINTN						*idx;
//-----------------------------

    if(gNoSdlDataMode) return EFI_NOT_FOUND;

	PCI_TRACE((TRACE_PCI,"\nPciBus: Locating SDL Data for [B%X|D%X|F%X]\n",
			Dev->Address.Addr.Bus,Dev->Address.Addr.Device, Dev->Address.Addr.Function));

	//Check first if we have Bridged device on a slot it should not have SDL record.
	if(Dev->Type > tPciRootBrg){
		if( parent->AmiSdlPciDevData==NULL || (parent->AmiSdlPciDevData)->PciDevFlags.Bits.OnBoard == 0 ){
			PCI_TRACE((TRACE_PCI,"  Parent is a SLOT or Device Behind SLOT ... Status=EFI_NOT_FOUND\n"));
			return EFI_NOT_FOUND;
		}
	}

	//Only tPciHostBrg device Type can have parent as NULL. 
	if(parent!=NULL) Status=AmiSdlPciGetChildsOfParentIndex(&chldbuff, &cnt, parent->SdlDevIndex);
	else {
		if (Dev->Type == tPciRootBrg)Status=AmiSdlPciGetChildsOfParentIndex(&chldbuff, &cnt, Dev->HostData->HbSdlIndex);
		else {
			ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
			return EFI_INVALID_PARAMETER;
		}
	}
	
    PCI_TRACE((TRACE_PCI,"  Parent has %d children ... Status=%r\n",cnt, Status));
//    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    idx=MallocZ(sizeof(UINTN)*cnt);
    if(idx==NULL){
    	Status=EFI_OUT_OF_RESOURCES;
    	ASSERT_EFI_ERROR(Status);
    	return Status;
    }

	for(i=0, hit=0; i<cnt; i++){
		tmp=chldbuff[i];
		//For On board devices - function # must provided.
		if(tmp->PciDevFlags.Bits.OnBoard){
			if(tmp->Device==Dev->Address.Addr.Device && tmp->Function==Dev->Address.Addr.Function){
				if (Dev->Type == tPciRootBrg &&  tmp->Bus != Dev->Address.Addr.Bus)
					continue;
				Status=AmiSdlFindRecordIndex(tmp, &idx[hit]);
				PCI_TRACE((TRACE_PCI,"  Found Matching #%d OnBoard Device; Idx=%d; @0x%X Status=%r.\n", hit+1,idx[hit],tmp, Status));
			    ASSERT_EFI_ERROR(Status);
			    //Now Update Bus Number
			    if(tmp->PciDevFlags.Bits.FixedBus) Dev->FixedBusNo=tmp->Bus;
			    //Now AmiSdlPciData record will have actual Bus Dev Func #
			    tmp->Bus=Dev->Address.Addr.Bus;
				hit++;
			}
		} else {
			if(tmp->Device==Dev->Address.Addr.Device) {
				Status=AmiSdlFindRecordIndex(tmp, &idx[hit]);
				PCI_TRACE((TRACE_PCI,"  Found Matching #%d Slot #%d(0x%X); Idx=%d; @0x%X Status=%r.\n",
							hit+1, tmp->Slot, tmp->Slot, idx[hit],tmp, Status));
			    ASSERT_EFI_ERROR(Status);
			    //Now Update Bus Number and Fixed Bus Number;
			    if(tmp->PciDevFlags.Bits.FixedBus) Dev->FixedBusNo=tmp->Bus;
			    //Now AmiSdlPciData record will have actual Bus Dev Func #
			    tmp->Bus=Dev->Address.Addr.Bus;
				hit++;
			}
		}
	}

	if(hit){
		//Update SDL Entry Count
		Dev->SdlDevCount=hit;

		//Get memory as we need it for SdlIndexArray
		Dev->SdlIdxArray=MallocZ(sizeof(UINTN)*hit);
		if(Dev->SdlIdxArray==NULL){
		   	Status=EFI_OUT_OF_RESOURCES;
		   	ASSERT_EFI_ERROR(Status);
		   	return Status;
		}
		//Copy Matching indexes...
		MemCpy(&Dev->SdlIdxArray[0], &idx[0], sizeof(UINTN)*hit);

		//for now initialize SDL Data Pointer with first Matching index Pointer 
		//that Points on not a container type object. It suppose to be only one.
		for(i=0; i<Dev->SdlDevCount; i++){
			tmp=&gSdlPciData->PciDevices[Dev->SdlIdxArray[i]];
			if(tmp->PciDevFlags.Bits.ContainerType);
			else {
				Dev->AmiSdlPciDevData=tmp;
				Dev->SdlDevIndex=Dev->SdlIdxArray[i];
				Dev->SdlMatchFound=TRUE;
				break;
			}
		}
	
		PCI_TRACE((TRACE_PCI,"  Updating: Count=%d; SdlPciData @0x%X; RecIdx=%d; MatchFound=%d; InitRoutine=%d.\n",
				Dev->SdlDevCount, Dev->AmiSdlPciDevData, Dev->SdlDevIndex, Dev->SdlMatchFound,Dev->AmiSdlPciDevData->InitRoutine));
		if(!Dev->SdlMatchFound) {
			PCI_TRACE((TRACE_PCI,"WARNING!!! MUST be at least 1 NOT(CONTAINER or VIRTUAL) SDL Object!\n"));
		    Dev->AmiSdlPciDevData=&gSdlPciData->PciDevices[Dev->SdlIdxArray[0]];
		    //Init SdlDevIndex field with the first index in array for now...
		    Dev->SdlDevIndex=Dev->SdlIdxArray[0];
		}
	} else {
		UINT8	rb=FindRootBus(Dev);
		//-------------------------------
		//here we fail to found matching SDL structure...
		Status=EFI_NOT_FOUND;
		if(Dev->Type != tPciHostDev){
			PCI_TRACE((TRACE_PCI,"  !!!WARNING!!! NO Matching SDL Object Was Found! Status=%r.\n",Status));
		} else {
			PCI_TRACE((TRACE_PCI,"  SDL Object not present for tPciHostDev. Will use it's Parent. %r.\n",Status));
		}
		//It is not a big deal if this is device on the slot but if this is a device on the root bus...
		//Than we got a problem since IRQ routing for this device may not be provided... if root don't have
		//routing information filled in then round-robin will not work.
		if(Dev->Type != tPciHostDev && Dev->Address.Addr.Bus==rb && Dev->IrqPinReg ) {
			PCI_TRACE((TRACE_PCI,"  !!!ERROR!!! This Device on the ROOT_BUS #0x%X; INT_PIN = 0x%X !!!\n  !!! Missing SDL IRQ Info - CHECK YOUR PORTING !!!\n", rb, Dev->IrqPinReg));
		}
	}
	
	if(Dev->AmiSdlPciDevData!=NULL){
		//Update Device's Attributes and Capabilities with EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE for OnBoard devices
		if(Dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard){
			Dev->Capab |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE);
			Dev->Attrib  |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE);
		}
		//Update fixed Resource Flag...
		Dev->FixedRes=Dev->AmiSdlPciDevData->PciDevFlags.Bits.FixedResources;
		CheckInitRomPolicy(Dev);
	}
	
	//free temp memory..
	if(idx!=NULL) pBS->FreePool(idx);
	if(chldbuff!=NULL) pBS->FreePool(chldbuff);

    return Status;
}

/**
    Finds corresponded BusXlatTable entry for the next bridge
    of the same level (same number of nodes/chain-counts).

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval UINT8   Number of buses decoded by the Bridge which "Ext" passed.

    @note  
  BaseBus         =   PCI_DEV_INFO->Address.Addr.Bus;
  SecondaryBus    =   PCI_BRG_EXT->Res[rtBus].Base;
  SubordinateBus  =   PCI_BRG_EXT->Res[rtBus].Base + PCI_BRG_EXT->Res[rtBus].Length-1;

**/
EFI_STATUS EFIAPI FindNextSameLevelBrg(PCI_DEV_INFO *Dev, AMI_SDL_PCI_DEV_INFO **NextBrg){
	AMI_SDL_PCI_DEV_INFO    *temp, *this=Dev->AmiSdlPciDevData;
	AMI_SDL_PCI_DEV_INFO    **buffer;
	EFI_STATUS				Status;
	UINTN					cnt, i;
	T_ITEM_LIST				brglst={0,0,NULL};
//-----------------------------
	
    if(gNoSdlDataMode) return EFI_NOT_FOUND;

	//This function should call only if Fixed Bus Numbering is selected..
	//So all Bus fields must reflect valid bus numbering...
	PCI_TRACE((TRACE_PCI, "FindNextSameLevelBrg: Searching for device [B%X|D%X|F%X]\n", 
			Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function));
	Status=AmiSdlPciGetChildsOfParentIndex(&buffer, &cnt, this->ParentIndex);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return EFI_NOT_FOUND;

	//Filter PCI devices and sort Root, P2P, and Crd Bridges..
	for(i=0; i<cnt; i++){
		temp=buffer[i];
		if(temp->PciDevFlags.Bits.Pci2PciBridgeType ||
		        temp->PciDevFlags.Bits.Pci2CrdBridgeType ||
		        temp->PciDevFlags.Bits.RootBridgeType){
			PCI_TRACE((TRACE_PCI, "\tFindNextSameLevelBrg: Found child [B%X|D%X|F%X]\n",
					Dev->Type == tPciRootBrg ? temp->Bus : Dev->ParentBrg->Address.Addr.Bus,
					temp->Device,
					temp->Function));
			Status=AmiPciAddDevDbEntry(temp, &brglst);
			if(EFI_ERROR(Status)) return Status;
		}
	}

	//now find next Dev in this filtered list P2P bridge...
	//if list has only 1 item that means we got this bridge only..
	if(brglst.ItemCount>1){
		//here we got devices sitting on the same bus
		for(i=0;i<brglst.ItemCount;i++){
			temp=brglst.Items[i];
			//Found this bridge in the list we looking for Next element.
			//list ids already sorted in ascending order so next or pev item will be ours..
			if(temp==this){
				//index of this bridge
				if(i<brglst.ItemCount-1) {
					*NextBrg=brglst.Items[i+1];

					if( DecodeFullBusRanges && (!PciBusCheckIfPresent(Dev, brglst.Items[i+1]))){
					    // Found next bridge, but it's not present.
					    // Start from here and continue to find next after that.
					    this = brglst.Items[i+1];
						PCI_TRACE((TRACE_PCI, "\t\tFindNextSameLevelBrg: [B%X|D%X|F%X] not present. Skipping.\n",
							Dev->Type == tPciRootBrg ? this->Bus : Dev->ParentBrg->Address.Addr.Bus,
							this->Device,
							this->Function));
						continue;
					} else {
						ClearItemLst(&brglst, FALSE);
						return EFI_SUCCESS;
					}
				} else break; //our bridge the last one in the hierarchy
			}
		}
	}

	//if no same level bridge found - return Max bus in range
	PCI_TRACE((TRACE_PCI, "\tFindNextSameLevelBrg: [B%X|D%X|F%X] is last bridge on this bus. Returning EFI_NOT_FOUND\n",
			Dev->Address.Addr.Bus,
			Dev->Address.Addr.Device,
			Dev->Address.Addr.Function));
	*NextBrg=NULL;
	return EFI_NOT_FOUND;
}


/**
    Finds last bus number this bridge should have when fixed bus enabled

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval UINT8   Number of buses decoded by the Bridge which "Ext" passed.

    @note  
  BaseBus         =   PCI_DEV_INFO->Address.Addr.Bus;
  SecondaryBus    =   PCI_BRG_EXT->Res[rtBus].Base;
  SubordinateBus  =   PCI_BRG_EXT->Res[rtBus].Base + PCI_BRG_EXT->Res[rtBus].Length-1;

**/
EFI_STATUS EFIAPI FindLastBusOfThisBridge(PCI_DEV_INFO *Dev, UINT8 *Bus){
    EFI_STATUS    Status;
    static	AMI_SDL_PCI_DEV_INFO	*nextbrgsdldata=NULL;
//---------------------------------

    if(gNoSdlDataMode) return EFI_NOT_FOUND;
    
    // Assume next bridge has dynamic bus, so no padding
	*Bus = (UINT8)mMaxBusFound;

    Status = FindNextSameLevelBrg(Dev, &nextbrgsdldata);
    // if this is the last child of this parent, go up a level
    if (Status == EFI_NOT_FOUND)
    {
    	// Did we hit the last root bridge?
    	if (Dev->Type == tPciRootBrg)
    	{
    		PCI_TRACE((TRACE_PCI, "\nFindLastBusOfThisBridge: Found last root bridge. returning 0xFF\n"));
    		// return last system bus
    		*Bus = 0xFF;
    	}
    	// if normal bridge, call recursively
		else
		{
			PCI_TRACE((TRACE_PCI, "\nFindLastBusOfThisBridge: Try Parent: [B%X|D%X|F%X]\n",
					Dev->Type == tPciRootBrg ? Dev->ParentBrg->AmiSdlPciDevData->Bus : Dev->ParentBrg->Address.Addr.Bus,
							Dev->ParentBrg->Address.Addr.Device,
							Dev->ParentBrg->Address.Addr.Function));
			return FindLastBusOfThisBridge(Dev->ParentBrg, Bus);
		}
    }
    else if (!EFI_ERROR(Status))
    {
    	// if we found a fixed bus bridge, return the fixed bus
    	if (nextbrgsdldata!=NULL && nextbrgsdldata->PciDevFlags.Bits.FixedBus==1)
    	{
    		// don't shift end bus for root bridges
    		if (Dev->Type == tPciRootBrg && mMaxBusFound <= (UINT8)((UINT16)(nextbrgsdldata->Bus - 1)))
    		{
    			*Bus = (UINT8)(nextbrgsdldata->Bus - 1);
    		} else {
				if(mMaxBusFound <= (UINT8)((UINT16)(nextbrgsdldata->Bus - 1)+Dev->FixedBusShift) )
				{
					*Bus = (UINT8)((UINT16)(nextbrgsdldata->Bus - 1)+Dev->FixedBusShift);
				}
				else
				{
					//With Fixed Bus allocation this condition signifies an error if we here
					//that means that we have found more buses than allowed by fixed bus layout
					//So scream about it.
					PCI_TRACE((TRACE_PCI,"\nPciBus: Can't apply Fixed Buses for the Next Bridge @ [B%X|D%X|F%X]:\n  Actual MAX Bus Discovered =%X; Proposed START Bus for That Bridge = %X\n    !!!WARNING!!! Will use Dynamic Assignment for the next Bridge on This Bridge level!!!\n\n",
						Dev->Address.Addr.Bus, nextbrgsdldata->Device, nextbrgsdldata->Function,
						mMaxBusFound, ((UINT16)(nextbrgsdldata->Bus - 1)+Dev->FixedBusShift)));
					nextbrgsdldata->Bus=0;
					nextbrgsdldata->PciDevFlags.Bits.FixedBus=0;
				}
    		}
    	}
    }
    else
    {
    	PCI_TRACE((TRACE_PCI, "\nFindLastBusOfThisBridge: [B%X|D%X|F%X] Status=%r\n", Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, Status ));
    	return Status;
    }

	if (Dev->Type == tPciRootBrg)
	{
		// Adjust for chipset bus reservations
		*Bus -= PciRserveUncoreBuses;
	}

	return EFI_SUCCESS;
}


/**
    Creates IRQ routing table entry for PCI Device.

        
    @param Dev Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BRG_INFO; PCI_BRG_EXT.

**/
EFI_STATUS EFIAPI CreateDevIrqEntry(PCI_DEV_INFO *Dev){
    UINTN               i;
    PCI_IRQ_PIC_ROUTE   *pic;
    PCI_IRQ_APIC_ROUTE  *apic;
    EFI_STATUS			Status;
    UINT16 				PciIrq;
    PCI_DEV_INFO 		*dev;
//-----------------------------------
    //If device don't use IRQ - just return...
    if((!Dev->IrqPinReg) && (Dev->Type==tPciDevice || Dev->Type==tPciHostDev)) return EFI_SUCCESS;
    
    if(IsFunc0(Dev)) dev=Dev;
    else dev=Dev->Func0;
    
    //if( Dev->AmiSdlPciDevData != NULL){
    if(dev->PicIrqEntry==NULL){
    	pic=MallocZ(sizeof(PCI_IRQ_PIC_ROUTE));
    	dev->PicIrqEntry=pic;
    } else pic=dev->PicIrqEntry;
    
    if(dev->ApicIrqEntry==NULL){
    	apic=MallocZ(sizeof(PCI_IRQ_APIC_ROUTE));
    	dev->ApicIrqEntry=apic;
    } else apic=dev->ApicIrqEntry;
    
	if(pic==NULL || apic==NULL) return EFI_OUT_OF_RESOURCES;

    //Get Already updated ISA_IRQ MASK
	Status=AmiIsaIrqMask(&PciIrq, TRUE);
	if (EFI_ERROR(Status)) {
		//if Var does not exist create it. 
		if(Status==EFI_NOT_FOUND){
		    PciIrq=ISA_IRQ_MASK;
		    Status=AmiIsaIrqMask(&PciIrq, FALSE);
		}
	}
	ASSERT_EFI_ERROR(Status);
	
    PciIrq=(~PciIrq); //Invert it, this is going to be most fresh PCI IRQ MASK

    
    //now fill out irq info...
    //PIC Irqinfo ...APIC ones...
    pic->PciBusNumber=Dev->Address.Addr.Bus;
    pic->DevFun.Dev=Dev->Address.Addr.Device;
    apic->PciBusNumber=Dev->Address.Addr.Bus;
    apic->DeviceNumber=Dev->Address.Addr.Device;

    
    if( Dev->AmiSdlPciDevData != NULL){
        for(i=0; i<4; i++){
        	if((pic->PciIrq[i].ChipsetReg !=0) && (Dev->AmiSdlPciDevData->PicIrq[i].ChipsetReg != 0) &&
        	   (pic->PciIrq[i].ChipsetReg != Dev->AmiSdlPciDevData->PicIrq[i].ChipsetReg))
        	{
        		PCI_TRACE((TRACE_PCI,"PciBus: WARNING!!!PCI IRQ reg does not MUCH for MF Device!\n     IRQ pin=%d; Old RR=0x%X; Update RR=0x%X\n",
        				i, pic->PciIrq[i].ChipsetReg, Dev->AmiSdlPciDevData->PicIrq[i].ChipsetReg ));
        		Status=EFI_DEVICE_ERROR;
        		ASSERT_EFI_ERROR(Status);
        	} else { 
        		if(Dev->AmiSdlPciDevData->PicIrq[i].ChipsetReg!=0){
        			pic->PciIrq[i].ChipsetReg=Dev->AmiSdlPciDevData->PicIrq[i].ChipsetReg;
        			pic->PciIrq[i].IrqMask=Dev->AmiSdlPciDevData->PicIrq[i].IrqMask;
        			pic->PciIrq[i].IrqMask&=PciIrq;
        		}
                
        		if(Dev->AmiSdlPciDevData->ApicIrq[i].IoApicItin!=0){
        			apic->Intn[i].IoApicItin=Dev->AmiSdlPciDevData->ApicIrq[i].IoApicItin;
        			apic->Intn[i].IoApicId=Dev->AmiSdlPciDevData->ApicIrq[i].IoApicId;
        		}
        	}
        }
        
        if(Dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard) pic->SlotNum=0;
        else pic->SlotNum=(UINT8)Dev->AmiSdlPciDevData->Slot;
    } 

    return Status;
}

EFI_STATUS EFIAPI GetAllPciDevices(PCI_BRG_EXT *BrgExt, T_ITEM_LIST *List){
	EFI_STATUS			Status=EFI_SUCCESS;
	UINTN				i;//,j;
	PCI_BRG_EXT			*ext;
    PCI_DEV_INFO		*dev;
//---------------------
	for(i=0; i<BrgExt->ChildCount; i++){
		dev=BrgExt->ChildList[i];
		if(dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg){
			//if it is slot and we have Bridged card on it...
			//we don't need dive behind the bridge looking for devices
			//since this is bridged slot...
			if(dev->AmiSdlPciDevData!=NULL){
				if(!dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard){
					Status=AppendItemLst(List, dev);
				} else {
					ext=(PCI_BRG_EXT*)(dev+1);
					Status=GetAllPciDevices(ext, List);
					//Some Bridges might use an interrupts at their primary interface needs
					if(dev->IrqPinReg && IsFunc0(dev))Status=AppendItemLst(List, dev);
				}
			}
		} else {
			// Add current device to list, only if its a end-point device, function 0 
			if(dev->Type==tPciDevice && IsFunc0(dev) && dev->PicIrqEntry!=NULL && dev->ApicIrqEntry!=NULL ) {
				Status=AppendItemLst(List, dev);
			} 
		}
		if(EFI_ERROR(Status)) return Status;
	}
	return EFI_SUCCESS;
}

VOID EFIAPI ApplyRoundRobin(PCI_DEV_INFO *Dev, PCI_IRQ_PIC_ROUTE *Pirq, PCI_IRQ_APIC_ROUTE *Airq){
	PCI_DEV_INFO 	*parent;
	UINTN			i;
	UINTN			shift=0;
	BOOLEAN			upd=FALSE;
//------------------------------
	parent=Dev->ParentBrg;
	
	if(parent==NULL || gNoSdlDataMode)return;
	do{
    	if( !( (parent->PicIrqEntry==NULL || parent->ApicIrqEntry==NULL) ||
    	    ( !((parent->PicIrqEntry->PciIrq[0].IrqMask | parent->PicIrqEntry->PciIrq[1].IrqMask |
			    parent->PicIrqEntry->PciIrq[2].IrqMask | parent->PicIrqEntry->PciIrq[3].IrqMask) &&
			  (parent->ApicIrqEntry->Intn[0].IoApicItin | parent->ApicIrqEntry->Intn[1].IoApicItin |
			    parent->ApicIrqEntry->Intn[2].IoApicItin | parent->ApicIrqEntry->Intn[3].IoApicItin))
			) )	)
		{
			shift=Dev->Address.Addr.Device%4;

			for(i=0; i<4; i++){
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
				if((i+shift)<4){
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
					MemCpy(&Pirq->PciIrq[i], &parent->PicIrqEntry->PciIrq[i+shift], sizeof(PCI_PIC_IRQ_DATA));
					MemCpy(&Airq->Intn[i], &parent->ApicIrqEntry->Intn[i+shift], sizeof(PCI_APIC_IRQ_DATA));
				} else {
					MemCpy(&Pirq->PciIrq[i], &parent->PicIrqEntry->PciIrq[i+shift-4], sizeof(PCI_PIC_IRQ_DATA));
					MemCpy(&Airq->Intn[i], &parent->ApicIrqEntry->Intn[i+shift-4], sizeof(PCI_APIC_IRQ_DATA));
				}
			}
			//Since we passing here pointer to the $PIR table entry, not a device
			//internal buffer, need to update Device Addres with Actual One...
		    Pirq->PciBusNumber=Dev->Address.Addr.Bus;
    		Pirq->DevFun.Dev=Dev->Address.Addr.Device;
    		Airq->PciBusNumber=Dev->Address.Addr.Bus;
    		Airq->DeviceNumber=Dev->Address.Addr.Device;
	 
	 		//And Slot Number, if any from SDL...
	        if(Dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard) Pirq->SlotNum=0;
        	else Pirq->SlotNum=(UINT8)Dev->AmiSdlPciDevData->Slot;

			upd=TRUE;
			break;
		}

    	if( parent->Type==tPciRootBrg ) break;
		parent=parent->ParentBrg;

	} while(parent!=NULL);
	
	if(!upd) {
		PCI_TRACE((TRACE_PCI,"ERROR!!! Can NOT ROUT IRQ using RR for Dev @ [B%X|D%X|F%X] Type %d, IntPin=0x%X\n",
			Dev->Address.Addr.Bus, Dev->Address.Addr.Device,
			Dev->Address.Addr.Function, Dev->Type, Dev->IrqPinReg));
 	}
}


/**
    Removes IRQ routing entries which got there by porting mistake
    in order not to confuse other consumers of AmiBoardInfoProtocol...

    @param NOTHING

    @retval NOTHING
**/
EFI_STATUS EFIAPI CreateIrqTables(){
	EFI_STATUS			Status;
    UINTN   			i, j;//, k;
    PCI_HOST_INFO		*lhst;
	PCI_BRG_INFO		*brg;
    PCI_DEV_INFO		*dev;//,*func;
    PCI_IRQ_PIC_ROUTE   *picr;
    PCI_IRQ_APIC_ROUTE  *apicr;
    T_ITEM_LIST			devlst={0,0,NULL};
//------------------------------
    for(j=0; j<gHostCnt; j++){
    	lhst=gPciHostList[j];
		for(i=0; i<lhst->RbCount; i++){
			brg=(PCI_BRG_INFO*)lhst->RootBridges[i];
			//This function will dump in a list all Function 0 of all devices...
			//this pretty much how many entries we need for
			Status=GetAllPciDevices(&brg->Bridge, &devlst);
			//the only status can come from there is OUT_OF_RESOURCES...
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
		}
    } //host loop

    //Now we got our devices in the "devlst"...
    //get memory for PIC and APIC IRQ routing tables...
    gAmiBoardInfo2Protocol->PicRoutLength=(sizeof(PCI_IRQ_PIC_ROUTE)*devlst.ItemCount);
    gAmiBoardInfo2Protocol->PicRoutTable=MallocZ(gAmiBoardInfo2Protocol->PicRoutLength);
    gAmiBoardInfo2Protocol->ApicRoutLength=sizeof(PCI_IRQ_APIC_ROUTE)*devlst.ItemCount;
    gAmiBoardInfo2Protocol->ApicRoutTable=MallocZ(gAmiBoardInfo2Protocol->ApicRoutLength);

    if(gAmiBoardInfo2Protocol->PicRoutTable==NULL || gAmiBoardInfo2Protocol->ApicRoutTable==NULL) Status=EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;


	//Pritn table header...
	PCI_TRACE((TRACE_PCI,"PciBus: Printing PIC $PIR/$AIR table, %d entries \n",devlst.ItemCount  ));
	PCI_TRACE((TRACE_PCI,"============================================================\n"));
	PCI_TRACE((TRACE_PCI," #  Bus  D|F  PIRQ A    PIRQ B    PIRQ C    PIRQ D    Slt#\n"));
	PCI_TRACE((TRACE_PCI,"============================================================\n"));

    //IRQ applys to DEVICES not FUNCTIONS so now we should filter Functions and
    //group them into corresponded device if they use different PCI_INT...
    for (i=0; i<devlst.ItemCount; i++){
    	dev=devlst.Items[i];
    	picr=&gAmiBoardInfo2Protocol->PicRoutTable[i];
    	apicr=&gAmiBoardInfo2Protocol->ApicRoutTable[i];

    	//Copy this Device IRQ routing info....
    	if( (dev->PicIrqEntry==NULL || dev->ApicIrqEntry==NULL) ||
    	    ( !((dev->PicIrqEntry->PciIrq[0].IrqMask | dev->PicIrqEntry->PciIrq[1].IrqMask |
    	    	dev->PicIrqEntry->PciIrq[2].IrqMask | dev->PicIrqEntry->PciIrq[3].IrqMask) &&
			  (dev->ApicIrqEntry->Intn[0].IoApicItin | dev->ApicIrqEntry->Intn[1].IoApicItin |
				dev->ApicIrqEntry->Intn[2].IoApicItin | dev->ApicIrqEntry->Intn[3].IoApicItin))
			)
    	)
    	{
    		ApplyRoundRobin(dev, picr, apicr);
    	}
    	else 
    	{
    		MemCpy(picr, dev->PicIrqEntry, sizeof(PCI_IRQ_PIC_ROUTE));
    		MemCpy(apicr, dev->ApicIrqEntry, sizeof(PCI_IRQ_APIC_ROUTE));
    	} 
    	
    	//Print Current Entry.
    	PCI_TRACE((TRACE_PCI,"%02d| %02X  %02X/%X  %02X->%04X  %02X->%04X  %02X->%04X  %02X->%04X  %02X <- PIC_IRQ\n", i,
				picr->PciBusNumber, picr->DevFun.Dev,picr->DevFun.Fun,
				picr->PciIrq[0].ChipsetReg, picr->PciIrq[0].IrqMask,
				picr->PciIrq[1].ChipsetReg, picr->PciIrq[1].IrqMask,
				picr->PciIrq[2].ChipsetReg, picr->PciIrq[2].IrqMask,
				picr->PciIrq[3].ChipsetReg, picr->PciIrq[3].IrqMask,
				picr->SlotNum));

		PCI_TRACE((TRACE_PCI,"  | %02X  %02X    %02X ID=%02X  %02X ID=%02X  %02X ID=%02X  %02X ID=%02X     <-APIC_INT\n",
                        apicr->PciBusNumber, apicr->DeviceNumber,
                        apicr->Intn[0].IoApicItin, apicr->Intn[0].IoApicId,
                        apicr->Intn[1].IoApicItin, apicr->Intn[1].IoApicId,
                        apicr->Intn[2].IoApicItin, apicr->Intn[2].IoApicId,
                        apicr->Intn[3].IoApicItin, apicr->Intn[3].IoApicId));

		PCI_TRACE((TRACE_PCI,"------------------------------------------------------------\n"));
    }//for(i...
	PCI_TRACE((TRACE_PCI,"============================================================\n"));

    //Now Irq Tables are Valid set the Flag.
    gAmiBoardInfo2Protocol->DataValid=TRUE;

    if(devlst.ItemCount) ClearItemLst(&devlst, FALSE);
    return Status;
}

/**
    This function will check if PCI "Device" incompatible by calling 
    Bad PCI Device Table - gBadPciDevList if needed.

        
    @param Device Pointer to PCI Device Private Data structure.
    @retval 
        N/A

 Referals: PCI_DEV_INFO; PCI_BAR_TYPE.
**/
VOID  EFIAPI UpdateBadBarList(PCI_DEV_INFO *Device){
	UINT32 			ssid=0; 
	PCI_CFG_ADDR	addr;
	UINTN			cnt=0, len=0;
	UINTN			i=0, j=0, rev=0;
	VOID			*Configuration=NULL;
//-------------------------------------------
	if(Device->HostData->IncompDevProt==NULL) return;
	
	//if Incompatible Device Protocol present...
	//1.Get SSVDID value.
	addr.ADDR=Device->Address.ADDR;
	
	addr.Addr.Register=0;
	//SS VID DID not available for bridge devices with header type 1.
	//CardBUs Bridge has it at offset 40h
	if(Device->Type==tPci2CrdBrg ) addr.Addr.Register=0x40;
	if(Device->Type==tPciDevice || Device->Type==tPciHostDev )addr.Addr.Register=PCI_SVID;
		
	if(addr.Addr.Register!=0) {
		EFI_STATUS	Status;
	//------------------
		//Read what we have there 
		Status=PciCfg32(Device->RbIo,addr,FALSE,&ssid);
		if(EFI_ERROR (Status)) return;
	}

	//now initalize parameters..
	cnt=(UINTN)(ssid & 0xFFFF);
	len=(UINTN)(ssid >> 16);
	//if Subsystem VID or ID was not filled 
	//have it in ignored state (-1) for the CheckDevice() function
	if(!cnt) cnt=-1;
	if(!len) len=-1;

	i=(UINTN)Device->DevVenId.VenId;
	j=(UINTN)Device->DevVenId.DevId;
	rev=(UINTN)Device->Class.RevisionId;
	
	//Quote from PI 1.2 vol 5 "This function will only return EFI_SUCCESS."
	Device->HostData->IncompDevProt->CheckDevice(
			Device->HostData->IncompDevProt,
			i, j, rev, cnt, len, &Configuration);
			
	//nothing to check Incompatible Device Protocol did not return configuration buffer!
	if(Configuration==NULL) return;
	
	//if we got configuration - check/count descriptors...
	len=ValidateDescriptorBlock(Configuration, tResAll, FALSE);
	if(!len) return;

	cnt=(len-sizeof(ASLR_EndTag))/sizeof(ASLR_QWORD_ASD);
	
	//Check if we got empty descriptor block - just the END_TAG
	//so, lucky we are ... it is a "cnt" of valid ACPI QW Resource descriptor(s)!
	if(cnt) {
		ASLR_QWORD_ASD	*dsc;
		PCI_BAD_BAR		*badbar;

	//------------------	
		dsc=(ASLR_QWORD_ASD*)Configuration;
		//fields to be used ifn resource descriptor...
		//1.Resource type:	0=Memory range;1=I/O range	
		//2._MIN = Fixed resource base. If the device does not request a fixed base, it must be 0.
		//3._MAX = Alignment information. If no special alignment for the BAR, it must be	0
		//4._TRA = Used to indicate the BAR Index from 0 to 5. 
		//         Specially, (UINT64)-1 in this field means all the PCI BARs on the device.
		//5._LEN = Length of the resource. If the device has no special length request, it must be 0.
		//============================================================================================
		// here we are going to map this stuff above to the stuff below
		// ------------------------------------------------------------------
		//	icNone			ENUM		Device is Compatible with PCI Spec 
		//	icBarBad		ENUM		Device doesn't set all implemented bits as "1" after writing there all "1"s. Setting icBadBar will notify PCI bus Driver that it could be a sizing problems with this BAR and it should treat this BAR differently.
		//	icBarFixedSize	ENUM		Device could hang the system if all "1s" will be written in corresponded BAR register. Setting icBarFixedSize will	notify PCI bus Driver to use Value provided in Incompatible Devices table intead of querying the Device.
		//	icNotBar 		ENUM		Device has a read/write register implemented but this register must not be enumerated. So the PCI Bus Driver will not consider this BAR as valid and will skip it. 
		//  icBad64BitBar   ENUM		Device has a bar that is 64bit capable, but the card does not function properly when it is allocated with an address above 4G.
		//	icMaxIncompatibility ENUM	Last valid value of this ENUM 
		//  If BarType field == tBarMaxType then BarOffset field should be used. 
		//  For "BarType" field only tBarIo, tBarMem and tBarMaxType allowed!
		
		for(i=0; i<cnt;i++, dsc++){
			EFI_STATUS	Status;
		//-----------------------	
			//Check Unsupported case...
			//we are not supporting Devices with fixed base.
			if(dsc->_MIN) continue;
			
			badbar=MallocZ(sizeof(PCI_BAD_BAR));
			if(badbar==NULL) return; //no memory left...
			

			badbar->VendorId = Device->DevVenId.VenId;
			badbar->DeviceId = Device->DevVenId.DevId;
			badbar->DevIdMask= 0xFFFF; 
			
			//Check if all BARs affected by this Descriptor or certain offset...
			if(dsc->_TRA ==((UINT64)-1)) {
				//Check if it is MMIO or IO BAR
				if(dsc->Type==ASLRV_SPC_TYPE_IO) badbar->BarType=tBarIo;
				else badbar->BarType=tBarMem;
			} else {
				badbar->BarType=tBarMaxType;
				badbar->BarOffset=(UINTN) Mul64(dsc->_TRA,4)+0x10;
			}
			
			badbar->IncompType=icBarFixedSize;
			
			if(dsc->_MAX) badbar->BarLength=(UINTN)dsc->_MAX+1;
			else badbar->BarLength = (UINTN)dsc->_LEN;
			
			Status=AppendItemLst(&gBadPciDevList,(VOID*)badbar);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return;
		}
		
		pBS->FreePool(Configuration);
	}
}

/**
    This function will check if PCI "Device" listed in Bad PCI 
    Device Table - gBadPciDevList[].

        
    @param Device Pointer to PCI Device Private Data structure.
    @param BarType Type of Bar Register.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BAR_TYPE.
**/
PCI_BAD_BAR* CheckPciCompatibility(PCI_DEV_INFO *Device, PCI_BAR *Bar, PCI_BAR_TYPE BarType) //(EIP41687)
{
	UINTN				i;
	PCI_BAD_BAR			*bbp;
	PCI_DEV_ID			d,t;
//-----------------------
	d.DEV_VEN_ID=Device->DevVenId.DEV_VEN_ID;
	for(i=0; i<gBadPciDevList.ItemCount; i++){
		bbp=(PCI_BAD_BAR*)gBadPciDevList.Items[i];
        //(EIP41687)>
        if(Bar!=NULL) {
            if (bbp->BarOffset) {
                if (Bar->Offset != bbp->BarOffset) continue;
            }
        }
        //<(EIP41687)
		t.VenId=bbp->VendorId;
		t.DevId=bbp->DeviceId & bbp->DevIdMask;
		d.DevId=Device->DevVenId.DevId & bbp->DevIdMask;
		if(d.DEV_VEN_ID==t.DEV_VEN_ID){
			if(BarType) {
				if(BarType==bbp->BarType) return bbp;
			} else return bbp;
		}
	}
	return NULL;
}


/**
    This function will Check and Adjust PCI Device's BAR
    Granularity according information found in gBadPciDevList[].

        
    @param Bar Type of Bar Register.

    @retval NOTHING

 Referals: PCI_BAR; PCI_BAR_TYPE; gBadPciDevList.

    @note  
  This function will be invoked only if PCI Device owning this BAR is in
 gBadPciDevList.
**/
VOID AdjustBarGra(PCI_BAR *Bar){
	UINTN			i,s,e;
	PCI_BAR_TYPE	bt;
	PCI_BAD_BAR		*bbp;
	UINT64			t=0,g=~(Bar->Gran);	
//----------------------------	
	switch(Bar->Type){
		case 	tBarIo16:       //2
		case 	tBarIo32:       //3
			bt=tBarIo;
			s=2;
			break;
		case tBarMmio32:        //4
		case tBarMmio32pf:      //5
		case tBarMmio64:        //6
		case tBarMmio64pf:      //7
			bt=tBarMem;
			s=4;
            break;
        default: 
            bt=tBarUnused;
		    s=0;
	}	
	
	bbp=CheckPciCompatibility(Bar->Owner,Bar,bt); //(EIP41687)
	if(!bbp)bbp=CheckPciCompatibility(Bar->Owner,Bar,tBarMaxType); //(EIP41687)
	//this function will be called only if compatibility issue exists with the device
	//so bbp must be valid or this is not the BAR which needs Adjustment;
	if(!bbp) return; 
	if(bbp->BarType==tBarMaxType && Bar->Offset!=bbp->BarOffset) return;
	
	//Here let's do the adjustment...
	switch(bbp->IncompType){
		case 	icBarBad:
			for(i=s; i<64; i++){
				//find the very first bit set as 1;
				t=Shl64(1,(UINT8)i);
				if(g&t)break;
			}
			//Assume that all other bits must be 1
			e=i;
			t=0;
			//Generate Granularity value for this BAR
			for(i=0;i<e;i++)t|=Shl64(1,(UINT8)i);
			if(Bar->Type!= tBarMmio64pf	&& Bar->Type!=tBarMmio64) t&=0xFFFFFFFF;
			Bar->Gran=t;
		break;

		case 	icBarFixedSize:
			Bar->Length=bbp->BarLength;
			Bar->Gran=bbp->BarLength-1;
		break;
	
		case 	icNotBar	:
			Bar->Type=tBarUnused;
			Bar->Length=0;
			Bar->Gran=0;
		break;

		case 	icBad64BitBar	:
			if(Bar->Type == tBarMmio64pf) Bar->Type=tBarMmio32pf;
			else if (Bar->Type == tBarMmio64) Bar->Type=tBarMmio32;
		break;
		default :
			break;
	}//switch
	
}

/**
    This function will detect PCI Option ROMs BAR requirements.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO.
**/
EFI_STATUS  EFIAPI GetOptRomRequirements(PCI_DEV_INFO *Device)
{
	PCI_CFG_ADDR	devaddr;
	EFI_STATUS		Status=0;
	UINT32			b32, o32;
	PCI_BAR			*rombar;
//---------------------------------
	//Our Device still in disable decoding mode which it entered in   
	//Now Query Expansion ROM reg 
	// the bit0 is 0 to prevent the enabling of the Rom address decoder
	devaddr.ADDR=Device->Address.ADDR;
	switch (Device->Type){
		case tPci2PciBrg : devaddr.Addr.Register=PCI_P2P_ROM_BAR;
			break;
		case tPciDevice	 : 
		case tPciHostDev :	
			devaddr.Addr.Register=PCI_DEV_ROM_BAR;
			break;
		case tPci2CrdBrg : //this one doesnot have ROM BAR
		default	:	return EFI_SUCCESS; //other devices not suppose to be examined
	}
	
	rombar=&Device->Bar[PCI_MAX_BAR_NO];
	rombar->Offset=devaddr.Addr.Register;

	//Read what we have there currently (suppose to be Zeros...)
	Status=PciCfg32(Device->RbIo,devaddr,FALSE,&o32);
	if(EFI_ERROR (Status)) return Status;
	
	b32=0xFFFFFFFE;

	//Write query pattern to PCI Rom Bar Register
	Status=PciCfg32(Device->RbIo,devaddr,TRUE,&b32);
	if(EFI_ERROR (Status)) return Status;
    
	Status=PciCfg32(Device->RbIo,devaddr,FALSE,&b32);
	if(EFI_ERROR (Status)) return Status;
   
	b32&=0xFFFFFFFE;
	if ((b32 != 0) && (b32 != 0xFFFFFFFE)){
		rombar->Type=tBarMmio32;
		rombar->Gran=~b32;
		rombar->Length=(~b32)+1;
	}

	if(Device->Incompatible) AdjustBarGra(rombar);
	
	if(rombar->Type!=tBarUnused){
	    if(gPciFixedRes && Device->FixedRes && rombar->Type!=tBarUnused ) {
		    Status=AmiPciUpdateBarFixedRes(rombar);
		    //this function can return NOT_FOUND or SUCCESS 
		    //if SUCCESS add Updated BAR to the ordered Bar list...
		    if(EFI_ERROR(Status)) {
		        Status=AmiPciAddBarDbEntry(rombar, &gNotFixedResBarList, FALSE);
		    } else {
		        Status=AmiPciAddBarDbEntry(rombar, &gFixedResBarList, TRUE);
		    } 
		} else {
		    Status=AmiPciAddBarDbEntry(rombar, &gNotFixedResBarList, FALSE);	
		}
        ASSERT_EFI_ERROR(Status);
	}

	//Restore  old value...
	if (rombar->Type!=tBarUnused){
		Status=PciCfg32(Device->RbIo,devaddr,TRUE,&o32);
		
		//Check here Embedded devices with Option ROMs exposed through ROM BAR
		if(Device->AmiSdlPciDevData!=NULL && Device->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard && Device->AmiRomPolicy==NULL){
		    //This Device must be in included in RomVar....
		    Status=CheckInitRomPolicy(Device);
		    //If status is NOT_FOUND 
		    if(EFI_ERROR(Status)){
		       ROM_POLICY_VAR  *rp;
		        
	        //If not - Update RomVar with tis device data and create AmiRomPolicyData record...
		        
		       Status=UpdateRomVar(Device, &rp);
		       if(!EFI_ERROR(Status))CreateRomPolicyData(Device, rp);
		    
		    }
		}
	}
    
	//Update ROM Status field...
    if(Device->AmiRomPolicy==NULL) {
        //If Device on Slot is bridged try to find AmiRomPolicy instance of it's parent...
        if((Device->AmiSdlPciDevData==NULL) && (rombar->Type!=tBarUnused)){
            PCI_DEV_INFO *dev=Device;
        //--------------------------
            //EFI_DEADLOOP();
            //Try to find first parent with SDL data.
            while(TRUE){
               dev=dev->ParentBrg;
               if(dev->AmiSdlPciDevData!=NULL)break;
               if(dev==NULL)break;
            }
            
            //must be slot only, all on-board devices with option ROMs must have SDL data by itselfs
            if(dev!=NULL){
                if( dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard || dev->AmiRomPolicy==NULL);
                else {
                    Device->AmiRomPolicy=dev->AmiRomPolicy;
                    Device->AmiRomPolicy->BaseClass=Device->Class.BaseClassCode;
                }
            } else {
                Status=EFI_DEVICE_ERROR;
                ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
            }
        }
    }
	return Status;
}


/**
    This function will detect PCI Device Capabilities of the Device's
    Command Cegister and Bridge Command register (if any).

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO.
**/
EFI_STATUS EFIAPI GetDeviceCapabilities(PCI_DEV_INFO *Dev)
{
	PCI_CFG_ADDR	addr;
	EFI_STATUS		Status;
	UINT16			oldcmd, tstcmd, oldbc, tstbc=0;
	UINT8			tcls=0;
//--------------------------------
    //Use | operator to preserve EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE Capabilities, set earlier. 
	if(Dev->ParentBrg)Dev->Capab|=Dev->ParentBrg->Capab;
	
	addr.ADDR=Dev->Address.ADDR;
	tstcmd=(PCI_CMD_IO_SPACE | PCI_CMD_MEMORY_SPACE | PCI_CMD_MEMORY_WRITE_AND_INVALIDATE |
			PCI_CMD_BUS_MASTER | PCI_CMD_VGA_PALETTE_SNOOP);

	//Set Latency Timer to 20 and Cacheline size to 0 - just in case
	addr.Addr.Register=0x0c;//Cacheline size & Latency Timer
	Status=PciCfg16(Dev->RbIo,addr,FALSE,&oldcmd);
	//PciX devices upon reset initialize Latency Timer reg with value of 0x40
	//regular PCI devices initialize it with 0
	if(!(oldcmd&0xff00)){//so we will program it only for Regular PCI devices
		oldcmd=0x2000;
		Status=PciCfg16(Dev->RbIo,addr,TRUE,&oldcmd);
		if(EFI_ERROR(Status))return Status;
	}
	addr.Addr.Register=0x04;//Command Reg

	//Read initial value
	Status=PciCfg16(Dev->RbIo,addr,FALSE,&oldcmd);
	if(EFI_ERROR(Status))return Status;

	//Write cmdval there and see if it supports it;
	Status=PciCfg16(Dev->RbIo,addr,TRUE,&tstcmd);
	if(EFI_ERROR(Status))return Status;

	//Read it back and see which bits remains set
	Status=PciCfg16(Dev->RbIo,addr,FALSE,&tstcmd);
	if(EFI_ERROR(Status))return Status;
	
	//Restore what was there
	Status=PciCfg16(Dev->RbIo,addr,TRUE,&oldcmd);
	if(EFI_ERROR(Status))return Status;

	if(Dev->Type==tPci2PciBrg){
	//Set Latency Timer to 20 and Cacheline size to 0 - just in case
		addr.Addr.Register=0x1b;//Secondary Latency Timer
		oldcmd=0x20;
		Status=PciCfg8(Dev->RbIo,addr,TRUE,(UINT8*)&oldcmd);
		if(EFI_ERROR(Status))return Status;

		addr.Addr.Register=0x3E; //Bridge controll reg
		tstbc=(P2P_BRG_CONTROL_ISA | P2P_BRG_CONTROL_VGA | P2P_BRG_CONTROL_VGA_16);
        
		//Read initial value
		Status=PciCfg16(Dev->RbIo,addr,FALSE,&oldbc);
		if(EFI_ERROR(Status))return Status;

		//Write cmdval there and see if it supports it;
		Status=PciCfg16(Dev->RbIo,addr,TRUE,&tstbc);
		if(EFI_ERROR(Status))return Status;

		//Read it back and see which bits remains set
		Status=PciCfg16(Dev->RbIo,addr,FALSE,&tstbc);
		if(EFI_ERROR(Status))return Status;
	
		//Restore what was there
		Status=PciCfg16(Dev->RbIo,addr,TRUE,&oldbc);
		if(EFI_ERROR(Status))return Status;
	}

	//Analize tstcmd Value and update Device->Capab;
	if(tstcmd & PCI_CMD_IO_SPACE)Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_IO;
	else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_IO);

	if(tstcmd & PCI_CMD_MEMORY_SPACE)Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_MEMORY;
	else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_MEMORY);

	if(tstcmd & PCI_CMD_BUS_MASTER)Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_BUS_MASTER;
	else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_BUS_MASTER);

	if(tstcmd & PCI_CMD_VGA_PALETTE_SNOOP)Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO;
	else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO);

	//clear CLS register we will program it later when resources will be allocated
	addr.Addr.Register=PCI_CLS; // Cache Line Size
	Status=PciCfg8(Dev->RbIo,addr,TRUE,&tcls);
	if(EFI_ERROR(Status))return Status;

	//we have additional work to do if Device is P2P bridge
	if(Dev->Type==tPci2PciBrg){ 
		if(tstbc & P2P_BRG_CONTROL_ISA)Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_ISA_IO;
		else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_ISA_IO);

        if(tstbc & P2P_BRG_CONTROL_VGA)Dev->Capab|=( EFI_PCI_IO_ATTRIBUTE_VGA_IO | 
					EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY|EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO);
		else Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_VGA_IO | 
					EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY|EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO));
        if(tstbc & P2P_BRG_CONTROL_VGA_16) Dev->Capab|=( EFI_PCI_IO_ATTRIBUTE_VGA_IO_16 | 
					EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO_16);
		else Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_VGA_IO_16 | EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO_16));

		//bridge should support IDE attributes
		Dev->Capab|=(EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO | EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO);
	} else {
		//Some special atributes could be supported by PCI devices based on class code
		switch (Dev->Class.BaseClassCode){
			case PCI_CL_OLD :
				if (Dev->Class.SubClassCode==PCI_CL_OLD_SCL_VGA)     
					Dev->Capab|=(EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO);
				else 
					Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO));
				break;
			case PCI_CL_DISPLAY:
				if (Dev->Class.SubClassCode==PCI_CL_DISPLAY_SCL_VGA)     
					Dev->Capab|=(EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO);
				else 
					Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO));

				if (Dev->Class.SubClassCode==PCI_CL_DISPLAY_SCL_OTHER) //GFX device can snoop pallete
					Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO;
				else Dev->Capab&=(~EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO);
				break;
			case PCI_CL_MASS_STOR :
				if(Dev->Class.SubClassCode==PCI_CL_MASS_STOR_SCL_IDE){
					UINT8	pib=Dev->Class.ProgInterface, pif=0;
				//---------------
					Dev->Capab|=(EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO|EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO);
					//Now if device is IDE controller check PI on order determine how it was programmed
					//Just in case overwrite bus master capab if IDE device sets bit 7 in PI Reg
					if(Dev->Class.ProgInterface & 0x80) Dev->Capab|=EFI_PCI_IO_ATTRIBUTE_BUS_MASTER;

					//primary Controller in LEGACY mode?
					if( !(Dev->Class.ProgInterface & 0x01)){
						if(Dev->HostData->PriIde==NULL){
							//Set the current PCI Device attributes to LEGACY 
							Dev->Attrib|=EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO;
							Dev->HostData->PriIde=Dev;
						} else {
							//set native mode bit since this Legacy IO space has been taken already
							pif++;
							pib |= 0x01;
						}
					} 
					
					//secondary Controller in LEGACY mode?
					if( !(Dev->Class.ProgInterface & 0x04) ){
						if(Dev->HostData->SecIde==NULL){
							//Set the current PCI Device attributes to LEGACY 
							Dev->Attrib|=EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO;
							Dev->HostData->SecIde=Dev;
						} else {
							//set native mode bit since this Legacy IO space has been taken already
							pif++;
							pib |= 0x04;
						}
					} 
					if(pif){
						Status=SetIdeDevMode(Dev, 0,0, &pib);
						if(!EFI_ERROR(Status)) Dev->Class.ProgInterface = pib; 
 					}
				} else Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO|EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO));

				break;
		}//switch;
	}
	
	//device caps should not exsceed parent;
	if(Dev->Capab & EFI_PCI_IO_ATTRIBUTE_IO)
		Dev->Capab|=(EFI_PCI_IO_ATTRIBUTE_ISA_MOTHERBOARD_IO | EFI_PCI_IO_ATTRIBUTE_ISA_IO);
	else 
		Dev->Capab&=(~(EFI_PCI_IO_ATTRIBUTE_ISA_MOTHERBOARD_IO | EFI_PCI_IO_ATTRIBUTE_ISA_IO));

	PCI_TRACE((TRACE_PCI, "\n Device Data: -> Supported Attributes -> 0x%lX\n", Dev->Capab));
	return Status;
}

/**
    This function will find first bit Set or Reset 
    going Left to Right.

        
    @param Value Value to scan.
    @param Set What condition to test
    @retval 
        UINT8                   First bit set/reset
**/
UINT8 EFIAPI FindFirstBit(UINT64 Value, BOOLEAN Set){
	UINT64	msk=1, iv=~Value;
	UINT8	bit;
//---------------------
	for(bit=0;bit<64;bit++){
		msk=Shl64(1,bit);		
		if(Set) {
			if( (Value & msk) == msk ) break;
		} else {
			if(( iv & msk ) == msk ) break;
		}
	}
	if(bit==64)	bit=0xFF;
	return bit;
}

///////////////////////////////////////////////////////////////
//   Capability pointers operation Including PCI Express...
///////////////////////////////////////////////////////////////

/**
    This function will collect information about PCI "Device"
    Capability Headers (Including PCI Express, Hot Plug and PCI-X/X2) and
    record them.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters - invalid.

**/
EFI_STATUS EFIAPI GetPciCapPtrs(PCI_DEV_INFO *Device)
{
	PCI_CFG_ADDR		devaddr;		
	EFI_STATUS			Status;
	EFI_PCI_CAPABILITY_HDR	cp;	
	UINT16				sr;
//-------------------------------
	devaddr.ADDR=Device->Address.ADDR;
	devaddr.Addr.Register=0x06; //Status Register;
	//bit 4 in status register is set if device supports capabilities pointer
	Status=PciCfg16(Device->RbIo,devaddr,FALSE,&sr);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
	if(!(sr&0x10)) return Status; //no reason to continue - capabilities is not supported!
		
	//if device Supports Capabilities
	cp.CAP_HDR=0;
	if(Device->Type==tPci2CrdBrg) devaddr.Addr.Register=EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR;	
	else  devaddr.Addr.Register=EFI_PCI_CAPABILITY_PTR;

	Status=PciCfg8(Device->RbIo,devaddr,FALSE,&cp.NextItemPtr);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	//we got something here. 
	while(cp.NextItemPtr)
	{
		devaddr.Addr.Register=cp.NextItemPtr;

		Status=PciCfg16(Device->RbIo,devaddr,FALSE,&cp.CAP_HDR);
		ASSERT_EFI_ERROR(Status);
		if(cp.CapabilityID==PCI_CAP_ID_PMI) Device->PmiCapOffs=devaddr.Addr.Register;

//Enables PCI Express Handling only if PCI Express Base is Defined and !=0
//#if PCI_EXPRESS_SUPPORT

		if(cp.CapabilityID==PCI_CAP_ID_PCIEXP){
			Status= PcieAllocateInitPcieData(Device,devaddr.Addr.Register);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
			//Get Setup Data for PciExpress Gen 1
			if(PcieCheckPcieCompatible(Device)){
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
				//get Setup Settings if any 
				if(Device->AmiSdlPciDevData!=NULL && Device->AmiSdlPciDevData->PciDevFlags.Bits.HasSetup){
					Status=AmiPciGetPcie1SetupData(&Device->PciExpress->Pcie1Setup,Device->AmiSdlPciDevData,
							Device->SdlDevIndex, FALSE);
				} else {
#endif				
					Device->PciExpress->Pcie1Setup=gPciDefaultSetup->Pcie1Settings;
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
				}	
#endif
				
			}
		}
//#endif

//#if HOTPLUG_SUPPORT
		if(cp.CapabilityID==PCI_CAP_ID_HOTPLUG){
			//we may process this controller as root HPC so check that.
			PCI_TRACE((TRACE_PCI,"PciBus: PCI-HP Caps OFFS=0x%X;\n",devaddr.Addr.Register ));
			Status=HpAllocateInitHpData(Device, devaddr.Addr.Register);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
		}
//#endif

//#if PCI_X_SUPPORT
		if(cp.CapabilityID==PCI_CAP_ID_PCIX){
			PCI_TRACE((TRACE_PCI,"PciBus: PCI-X Caps OFFS=0x%X;\n",devaddr.Addr.Register ));

			Device->PciX=MallocZ(sizeof(PCIX_DATA));
			ASSERT(Device->PciX);
			if(!Device->PciX) return EFI_OUT_OF_RESOURCES;

			//fill out fields within PciX structure
			Device->PciX->PcixOffs=devaddr.Addr.Register;
			devaddr.Addr.Register=Device->PciX->PcixOffs+PCIX_DEV_STA_OFFSET;
			Status=PciCfg32(Device->RbIo, devaddr, FALSE, &Device->PciX->PcixDevStatus.DEV_STA);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

			devaddr.Addr.Register=Device->PciX->PcixOffs+PCIX_SEC_STA_OFFSET;
			Status=PciCfg16(Device->RbIo, devaddr, FALSE, &Device->PciX->PcixSecStatus.SEC_STA);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

			Device->PciX->PcixOffs=devaddr.Addr.Register;
			Device->PciX->Owner=Device;

		}
//#endif
	}
	return Status;
}


//----------------------------------------------------------------------------
//Enable PCI Express Handling only if PCI Express Base is Defined and !=0
//#if PCI_EXPRESS_SUPPORT
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
// PCI Express Helper Functions
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//Enable PCI Express Handling only if PCI Express Base is Defined and !=0
//#endif //PCI_EXPRESS_SUPPORT
//----------------------------------------------------------------------------

#if S3_VIDEO_REPOST_SUPPORT == 1
/**
    This function will create S3 Resume Boot Script
    for Device's BAR Registers.

        
    @param Dev Pointer to PCI Device Private Data structure.

    @retval 
        Nothing.

**/

VOID SaveBars(PCI_DEV_INFO *Dev){
    UINTN           i;
    PCI_BAR         *bar;
	PCI_CFG_ADDR	addr;		
	EFI_STATUS		Status;
	UINT64			v=0;
	PCI_BRG_EXT		*ext;
//----------------------------

    for( i=0; i < PCI_MAX_BAR_NO+1; i++){
        bar=&Dev->Bar[i];
		addr.ADDR=Dev->Address.ADDR;
		addr.Addr.ExtendedRegister=0;
        
        if( (bar->Type != tBarUnused) && (bar->Length != 0)){
            addr.Addr.Register=bar->Offset;
            if( (Dev->Bar[0].DiscoveredType == tBarMmio64) || (Dev->Bar[0].DiscoveredType == tBarMmio64pf)){ 
                Status=PciCfg64(Dev->RbIo,addr,FALSE,&v);
       			BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                                gS3SaveState,				//This
                				EfiBootScriptWidthUint64,	//Width
                				addr.ADDR,1,&v);			//Address, Data
            } else {
                Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
       			BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
       							gS3SaveState, 			//This
                				EfiBootScriptWidthUint32,	//Width
                				addr.ADDR,1,&v);			//Address, Data
            } 
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return;
        }
    } //for
    
    //if device is p2p bridge
    if( Dev->Type == tPci2PciBrg){
        //get bridge extension structure pointer
        ext=(PCI_BRG_EXT*)(Dev+1);
        
        //For IO 16 Resources Decoded by the bridge
        addr.Addr.Register = ext->Res[rtIo16].Offset;
        Status=PciCfg16(Dev->RbIo,addr,FALSE,(UINT16*)&v);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;
		
		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
							gS3SaveState, 			//This
               				EfiBootScriptWidthUint16,	//Width
               				addr.ADDR,1,&v);			//Address, Data
        //Upper 16bits for IO window
        addr.Addr.Register = 0x30;
        Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;

		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
							gS3SaveState, 			//This
               				EfiBootScriptWidthUint32,	//Width
               				addr.ADDR,1,&v);			//Address, Data
   		
        
		//For MMIO Resources Decoded by the bridge
        addr.Addr.Register = ext->Res[rtMmio32].Offset;
        Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;

		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
							gS3SaveState, 			//This
               				EfiBootScriptWidthUint32,	//Width
               				addr.ADDR,1,&v);			//Address, Data

		//For MMIO_PF 
        addr.Addr.Register = ext->Res[rtMmio32p].Offset;
        Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;

		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
							gS3SaveState, 			//This
               				EfiBootScriptWidthUint32,	//Width
               				addr.ADDR,1,&v);			//Address, Data

		//For MMIO_PF Upper 32 bit
        addr.Addr.Register = 0x28;
        Status=PciCfg64(Dev->RbIo,addr,FALSE,&v);
		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
							gS3SaveState, 			//This
               				EfiBootScriptWidthUint64,	//Width
               				addr.ADDR,1,&v);			//Address, Data
		ASSERT_EFI_ERROR(Status);
    }

}

/**
    This function will create S3 Resume Boot Script
    for Device's other than BAR Registers.

        
    @param Dev Pointer to PCI Device Private Data structure.

    @retval 
        Nothing.

**/

VOID SaveDevice(PCI_DEV_INFO *Dev){
	PCI_CFG_ADDR	addr;		
	EFI_STATUS		Status;
	UINT64			v=0;
//----------------------------
	addr.ADDR=Dev->Address.ADDR;
	addr.Addr.ExtendedRegister=0;
        
    //get Cache line size and Latency Timer + ProgInterface 
	addr.Addr.Register=PCI_CLS;
    Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return;

    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
    			gS3SaveState, 			//This
				EfiBootScriptWidthUint32,	//Width
				addr.ADDR,1,&v);			//Address, Data

    //Save content of BARs 
    SaveBars(Dev);    

    //get IntLine; IntPin; MaxLat; MinGnt
    addr.Addr.Register=PCI_INTLINE;
    Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return;

    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
    			gS3SaveState, 			//This
				EfiBootScriptWidthUint32,	//Width
				addr.ADDR,1,&v);			//Address, Data

    //if device is Pci2PciBrg
	if( Dev->Type == tPci2PciBrg){
			
        //get Base bus Subordinste bus Secondary bus registers + Secondary Latency Timer
        addr.Addr.Register=PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;

    	Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
	    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
	    			gS3SaveState, 			//This
			        EfiBootScriptWidthUint32,	//Width
			        addr.ADDR,1,&v);			//Address, Data


	} else {
        //Get Subsystem VID; DID Just In case...
		addr.Addr.Register=PCI_SVID;
        Status=PciCfg32(Dev->RbIo,addr,FALSE,(UINT32*)&v);
		BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
					gS3SaveState, 			//This
    				EfiBootScriptWidthUint32,	//Width
				    addr.ADDR,1,&v);			//Address, Data

    }
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return;


    //Now get Device Control Reg
	addr.Addr.Register=PCI_COMMAND_REGISTER_OFFSET;
	Status=PciCfg16(Dev->RbIo,addr,FALSE,(UINT16*)&v);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return;

	BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
			gS3SaveState, 			//This
		    EfiBootScriptWidthUint16,	//Width
		    addr.ADDR,1,&v);			//Address, Data

}

/**
    This function will create S3 Resume Boot Script
    for Device's on the path of primary VGA Device inclusive.

    @param 
        Nothing

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
VOID RecordPriVgaBootScript(IN EFI_EVENT Event, IN VOID *Context){
	PCI_DEV_INFO	*vga=NULL;
	PCI_DEV_INFO	*dev;
	PCI_HOST_INFO	*hst;
    PCI_BRG_EXT     *ext;
    T_ITEM_LIST     VgaPath={0,0,NULL};
	INTN			i;
    UINTN           j;
	EFI_STATUS		Status;
//------------------------------
    //If we were not found gS3SaveState - nothing to save here
    if(gS3SaveState==NULL) return;

    for( i=0; (UINTN)i<gHostCnt; i++){
        hst=gPciHostList[i];
        //There might be only one legacy VGA device in the system
        if( hst->VgaDev!=0){
            vga=hst->VgaDev;
            break;//there will be only one Primary VGA
        }
    }
    
    //If we can't find Primery VGA device just exit 
    //Most likely we are dealing here with headless system.
    if( vga == NULL){
        return;
    }

    dev = vga;
    //Save Device pointers in Current Primary Vga Path
    while (dev!=NULL){
        Status=AppendItemLst(&VgaPath,dev);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;

        dev=dev->ParentBrg;
    }

    //Now write the boot script in opposite order for the devices stored @ VgaPath object.
    //So the script will go from Host Bridge down to the Pri VGA device 
	for(i=VgaPath.ItemCount-1; i>=0;i--){
		dev=(PCI_DEV_INFO*)VgaPath.Items[i];
                
        //Save device content into BOOT SCRIPT
        SaveDevice(dev);

        //Now check if device is a multifunction device
        //if so it's other functions needs to be saved as well
        if( ( dev->ParentBrg !=NULL) && ( dev->Type != tPci2PciBrg)){
            
            ext = (PCI_BRG_EXT*)(dev->ParentBrg+1);
            //check for devices with the same Dev# but different Function# among device's ParentBridge childs...
            for(j=0; j<ext->ChildCount; j++){           
                vga=ext->ChildList[j];    
                //Since this is one bridge childs they have to have same bus
                if(vga->Address.Addr.Device == dev->Address.Addr.Device &&
                   vga->Address.Addr.Function != dev->Address.Addr.Function) {
                    SaveDevice(vga);
                }

            }//for j
        }
    }//for i

    pBS->CloseEvent(Event);
}
#endif //S3_VIDEO_REPOST

/**
    This function will turn off SMI generation on PCI PME
    and put device in to D0 state, if device has PM capability. 

        
    @param Brg Pointer to PCI Device Private Data structure.

    @retval Nothing

**/
VOID EFIAPI ResetDevicePm(PCI_DEV_INFO *Device)
{
	EFI_STATUS		Status;
	PCI_CFG_ADDR	devaddr;		
	UINT16			pmcsrv=0x8000;//Turn off the PME assertion and set D0 PM State
//-------------------------------
	
	if(Device->PmiCapOffs){
		devaddr.ADDR=Device->Address.ADDR;
		devaddr.Addr.Register=(UINT8)(Device->PmiCapOffs+4); //
		Status=PciCfg16(Device->RbIo,devaddr,TRUE,&pmcsrv);
		ASSERT_EFI_ERROR(Status);
	}
	return; 
}

/**
    This function will collect all possible data about PCI Device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS EFIAPI QueryPciDevice(PCI_DEV_INFO *Device)
{
	EFI_STATUS		Status;
	UINT8			maxbar,	i, incr=0;	
	UINT64			buff=0xff, oldv;
	PCI_CFG_ADDR	devaddr;
	UINT32			*b32=(UINT32*)&buff, *o32=(UINT32*)&oldv;
//------------------------------------------

	//for accuracy
	Device->Address.Addr.Register=0; 
	Device->Address.Addr.ExtendedRegister=0;
	devaddr.ADDR=Device->Address.ADDR;
	
	PCI_TRACE((TRACE_PCI,"PciBus: Discovered PCI Device @ [B%X|D%X|F%X]; PCI_DEV_INFO.Type=%d.\n", 
			Device->Address.Addr.Bus, Device->Address.Addr.Device,
			Device->Address.Addr.Function, Device->Type));

	//Clear Interrupt line register
	devaddr.Addr.Register=PCI_INTLINE;
    //P2P Bridge Spec. v1.1 requires to set INT Line Register to 0xFF
    //PCI spec 3.0 require to set it to 0xFF - means not connected for x86 architecture.
    //Values of 0..0x0F indicating IRQ0..15 values 0x10..0xFE reserved.
	Status=PciCfg8(Device->RbIo,devaddr,TRUE,(UINT8*)&buff);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	//We must call Preprocess Controller 
	Status=DoPrepController(Device);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	Status=GetDeviceCapabilities(Device);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	Status=GetPciCapPtrs(Device);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
	
	ResetDevicePm(Device);	

	if(PcieCheckPcieCompatible(Device)){
		Status=PcieProbeDevice(Device, gPciCommonSetup->SriovSupport);
	    ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		//Update Setup Data for PCIe gen 2
		if(Pcie2CheckPcie2Compatible(Device)){
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
			//get Setup Settings if any 
			if(Device->AmiSdlPciDevData!=NULL && Device->AmiSdlPciDevData->PciDevFlags.Bits.HasSetup){
				Status=AmiPciGetPcie2SetupData(&Device->PciExpress->Pcie2->Pcie2Setup,
						Device->AmiSdlPciDevData, Device->SdlDevIndex, FALSE);
			} else {
#endif
				Device->PciExpress->Pcie2->Pcie2Setup=gPciDefaultSetup->Pcie2Settings;
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
			}	
#endif
		}
	}

	//Now time to get Setup settings override
	Status=AmiPciLibLaunchInitRoutine(Device, isPciGetSetupConfig, itDevice, Device, NULL, NULL, NULL);
	if(EFI_ERROR(Status)){
		if(Status==EFI_UNSUPPORTED){
			Status=EFI_SUCCESS;
		} else ASSERT_EFI_ERROR(Status);
	} else {
        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X Overrides SetupData...\n\n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, Device->Address.Addr.Function,
        		Device->DevVenId.VenId, Device->DevVenId.DevId));
	}

	//After overriding settings check if override does not have any conflicts with inherited settings from ParentBridge
	// from the parent bridge. If i.e. parent does not decode 4G this should be the for all it children down stream.
	//Now we have PCI_SETUP_DONT_TOUCH option == 0xFE - so copy entire value there... it it is not 0 either TRUE or DONT_TOUCH
	//It is not possible set this option through Setup, only through Initialization Routine
	if(Device->ParentBrg->DevSetup.Decode4gDisable)Device->DevSetup.Decode4gDisable=Device->ParentBrg->DevSetup.Decode4gDisable;
	if(Device->ParentBrg->DevSetup.Pcie1Disable)Device->DevSetup.Pcie1Disable=Device->ParentBrg->DevSetup.Pcie1Disable;
	if(Device->ParentBrg->DevSetup.Pcie2Disable)Device->DevSetup.Pcie2Disable=Device->ParentBrg->DevSetup.Pcie2Disable;
	if(Device->ParentBrg->DevSetup.HpDisable)Device->DevSetup.HpDisable=Device->ParentBrg->DevSetup.HpDisable;
	
    //Now when we got setup preferences try to Init PCIe device if allowed...
	Status=PcieInitDevice(Device, &mMaxBusFound);
	
	
	//Set Pci Latency Timer register according to the Setup settings;
	if(Device->DevSetup.PciLatency != PCI_SETUP_DONT_TOUCH){
        devaddr.Addr.Register = PCI_PMLT;
        Status=PciCfg8(Device->RbIo,devaddr,TRUE,(UINT8*)&Device->DevSetup.PciLatency);
        if((Device->Type == tPci2PciBrg) || (Device->Type == tPci2CrdBrg)){
            devaddr.Addr.Register = PCI_SMLT;
            Status=PciCfg8(Device->RbIo,devaddr,TRUE,(UINT8*)&Device->DevSetup.PciLatency);
        }
	}
	
	//Set PCI-X Latency Timer register according to the Setup settings;
	if((Device->PciX != NULL) && (Device->DevSetup.PciXLatency != PCI_SETUP_DONT_TOUCH)){
		devaddr.Addr.Register = PCI_SMLT;
		Status=PciCfg8(Device->RbIo,devaddr,TRUE,(UINT8*)&Device->DevSetup.PciXLatency);
		if((Device->Type == tPci2PciBrg) || (Device->Type == tPci2CrdBrg)){
			devaddr.Addr.Register = PCI_SMLT;
			Status=PciCfg8(Device->RbIo,devaddr,TRUE,(UINT8*)&Device->DevSetup.PciXLatency);
		}
	}

	//If PciIncompatibleDeviceProtocol exists will check device and update local database with it.
	UpdateBadBarList(Device);

	//Check Compatibility 
	if(CheckPciCompatibility(Device,NULL,tBarUnused)!=NULL)Device->Incompatible=TRUE; //(EIP41687)
	switch (Device->Type){
		case tPci2PciBrg :
			maxbar=2;
			devaddr.Addr.Register=PCI_BAR0; //first BAR starts here
			break;
		case tPciDevice	:
		case tPciHostDev:
			maxbar=PCI_MAX_BAR_NO;
			devaddr.Addr.Register=PCI_BAR0; //first BAR starts here
			break;
		case tPci2CrdBrg :
			maxbar=1;	
			devaddr.Addr.Register=PCI_BAR0; //first BAR starts here
			break;
		default	:	return EFI_SUCCESS; //other devices not suppose to be examined????
	}
	
	for(i=0; i<maxbar; i++){
		buff=0;
		oldv=0;		
		devaddr.Addr.Register=devaddr.Addr.Register+incr;
		Status=PciCfg32(Device->RbIo,devaddr,FALSE,b32);
		if(EFI_ERROR(Status)) return Status;
		
		//check if what it is...
		if((*b32) & 1) {
			Device->Bar[i].Type=tBarIo;
			incr=4;
		} else {
			switch ((*b32) & 0x0F) {
				case 0x0 : 
					Device->Bar[i].Type=tBarMmio32;
					Device->Bar[i].DiscoveredType=tBarMmio32;
					incr=4;
					break;
				case 0x2: //this takes care about old PCI Bar type - MMIO below 1M
				    Device->Bar[i].DiscoveredType=tBarMmio32;
				    Device->Bar[i].Type=tBarUnused;
				    incr=4;
				    break;
				case 0x4 : 
				    if (CombineMem64PfMem)Device->Bar[i].Type=tBarMmio64pf;
				    else Device->Bar[i].Type=tBarMmio64;
					Device->Bar[i].DiscoveredType=tBarMmio64;
					incr=8;
					maxbar--;
					break;
				case 0x8 :
				    if (CombineMemPfMem32) Device->Bar[i].Type=tBarMmio32;
				    else Device->Bar[i].Type=tBarMmio32pf;
					Device->Bar[i].DiscoveredType=tBarMmio32pf;
					incr=4;
					break;
				case 0xc :
					Device->Bar[i].Type=tBarMmio64pf;
					Device->Bar[i].DiscoveredType=tBarMmio64pf;
					incr=8;
					maxbar--;
					break;
				default : return EFI_UNSUPPORTED;
			} //switch			
		}// else for memory BAR
		Device->Bar[i].Offset=devaddr.Addr.Register;
		
		buff=(~0ULL);
		
#if SMCPKG_SUPPORT
        if((gPciDefaultSetup->SmcPriorVgaBus != 0) &&
                (gPciDefaultSetup->SmcPriorVgaBus == Device->Address.Addr.Bus ) &&
                (gPciCommonSetup->Above4gDecode)){
            if(Device->Bar[i].Type == tBarMmio64pf){
                Device->Bar[i].Type = tBarMmio32pf;
            }
            if(Device->Bar[i].Type == tBarMmio64){
                Device->Bar[i].Type = tBarMmio32;
            }
        }
#endif // #if SMCPKG_SUPPORT

		switch (Device->Bar[i].Type){
		
			case tBarMmio64pf	:
			case tBarMmio64		:
				Status=PciCfg64(Device->RbIo,devaddr,FALSE,&oldv);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg64(Device->RbIo,devaddr,TRUE,&buff);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg64(Device->RbIo,devaddr,FALSE,&buff);
				if(EFI_ERROR(Status)) return Status;
				buff&=(~0x0F); //Mask don't care bits
				if(buff){
					//This workaround done for PCI Compliance Test... 
					//It could be the BAR that clames - "I'm a 64bit BAR", 
                    //but implemented as 32bit register. This BAR will not hold 
                    //64bit address and must be converted to 32bit BAR.
					if(buff<=0xFFFFFFFF) {
						buff|=0xFFFFFFFF00000000;
						Device->Bar[i].Type-=2; //reduce tBarType to 32 bit BAR
					}
					Device->Bar[i].Gran=(UINTN)(~buff);
					if(Device->Incompatible) AdjustBarGra(&Device->Bar[i]);
					if(Device->Bar[i].Type==tBarUnused) {
						Status=PciCfg64(Device->RbIo,devaddr,TRUE,&oldv);
						if(EFI_ERROR(Status)) return Status;
					}
					Device->Bar[i].Length=Device->Bar[i].Gran+1;
				} else Device->Bar[i].Type=tBarUnused;
				break;

			case tBarMmio32pf	:
			case tBarMmio32		:
			case tBarIo			:
				Status=PciCfg32(Device->RbIo,devaddr,FALSE,o32);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg32(Device->RbIo,devaddr,TRUE,b32);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg32(Device->RbIo,devaddr,FALSE,b32);
				if(EFI_ERROR(Status)) return Status;
				if(Device->Bar[i].Type==tBarIo){
					(*b32)&=(~0x03);
					if(*b32){
						//We got something here try to determine is it 32 bit addressing IO
						//of 16 bit addressing
						if(*b32&0xFFFF0000) {
							Device->Bar[i].Type=tBarIo32;
							Device->Bar[i].DiscoveredType=tBarIo32;
						} else { 
							(*b32)|=(0xffff0000);
							Device->Bar[i].Type=tBarIo16;
							Device->Bar[i].DiscoveredType=tBarIo16;
						}
					}
				} else (*b32)&=(~0x0F);
				if(*b32){
					Device->Bar[i].Gran=(~(*b32));
					if(Device->Incompatible) AdjustBarGra(&Device->Bar[i]);
					if(Device->Bar[i].Type==tBarUnused) {
						Status=PciCfg32(Device->RbIo,devaddr,TRUE,o32);
						if(EFI_ERROR(Status)) return Status;
					}
					Device->Bar[i].Length=Device->Bar[i].Gran+1;
					//Doing work around for resource requirements for I/O
					//where request is lesser than 16 bytes. I'll just make it 16
					//it might be some compatibility issues if I/O resourtce alignment will be 
					// lesser than 8 byte
					if((Device->Bar[i].Type==tBarIo32 || Device->Bar[i].Type==tBarIo16)&&
						Device->Bar[i].Length<0x10)
					{
						Device->Bar[i].Length=0x10;
						Device->Bar[i].Gran=0x0F;
					}

				} else Device->Bar[i].Type=tBarUnused;
				break;
			
			default : Device->Bar[i].Type=tBarUnused;
		}//switch 
		

        //Add BAR into Fixed BARs Database
		if(Device->Bar[i].Type!=tBarUnused){
	        if(gPciFixedRes && Device->FixedRes) {
	            Status=AmiPciUpdateBarFixedRes(&Device->Bar[i]);
	            //this function can return NOT_FOUND or SUCCESS 
	            //if SUCCESS add Updated BAR to the ordered Bar list...
	            if(EFI_ERROR(Status)) {
	                Status=AmiPciAddBarDbEntry(&Device->Bar[i], &gNotFixedResBarList, FALSE);
	            } else {
	                Status=AmiPciAddBarDbEntry(&Device->Bar[i], &gFixedResBarList, TRUE);
	            }
	        } else {
			    Status=AmiPciAddBarDbEntry(&Device->Bar[i], &gNotFixedResBarList, FALSE);
			}
	        ASSERT_EFI_ERROR(Status);
        }

        //Restore Original Value value Zeroing out Incompatible BARs
		if((Device->Bar[i].DiscoveredType!=tBarUnused) && (Device->Bar[i].Type==tBarUnused)) oldv=0;
		if(Device->Bar[i].DiscoveredType !=tBarUnused){
			if(Device->Bar[i].Type==tBarMmio64pf || Device->Bar[i].Type==tBarMmio64) Status=PciCfg64(Device->RbIo,devaddr,TRUE,&oldv);
			else Status=PciCfg32(Device->RbIo,devaddr,TRUE,o32);
			if(EFI_ERROR(Status)) return Status;
		}

//Check that all MMIO is allocated in 4k aligned chanks IVT-d requirements   
#if( (defined iVTd_SUPPORT && iVTd_SUPPORT == 1) || ( PCI_4K_RESOURCE_ALIGNMENT == 1 ) || (SRIOV_SUPPORT == 1) )
//---------------------------------------------
        if((Device->Bar[i].Type==tBarMmio64pf || Device->Bar[i].Type==tBarMmio64 ||
            Device->Bar[i].Type==tBarMmio32pf || Device->Bar[i].Type==tBarMmio32) &&
            Device->Bar[i].Length<0x1000 ) {
                Device->Bar[i].Length=0x1000;
                Device->Bar[i].Gran=0xFFF;
            }               
//---------------------------------------------
#endif
        
        //now apply new feature of the pci bus driver - selective 4G decode....
        if((gAllocationAttributes & EFI_PCI_HOST_BRIDGE_MEM64_DECODE)&&
        	Device->DevSetup.Decode4gDisable &&
        	(Device->Bar[i].Type==tBarMmio64pf))Device->Bar[i].Type=tBarMmio32pf;
        if((gAllocationAttributes & EFI_PCI_HOST_BRIDGE_MEM64_DECODE)&&
        	Device->DevSetup.Decode4gDisable &&
        	(Device->Bar[i].Type==tBarMmio64)) Device->Bar[i].Type=tBarMmio32;

	} // Bar enumeration loop

	if(Device->Type==tPci2CrdBrg){
		//Clear Brg Control Reg bits 8 & 9 to signify that Mem Window 1&2 is not PF
		devaddr.Addr.Register=PCI_BRIDGE_CONTROL_REGISTER_OFFSET;
		Status=PciCfg16(Device->RbIo,devaddr,FALSE,(UINT16*)&buff);
		ASSERT_EFI_ERROR(Status);
		buff&=(~(BIT09 | BIT08));	//Memory Window 2 is nonPF
		Status=PciCfg16(Device->RbIo,devaddr,TRUE,(UINT16*)&buff);
		ASSERT_EFI_ERROR(Status);
	}

	//now we're going to check if any Option ROM present
	Status=GetOptRomRequirements(Device);
#if EFI_DEBUG
	for(i=0; i<=PCI_MAX_BAR_NO; i++){

		if((Device->Bar[i].Type>0) && (Device->Bar[i].Length>0))
		{
			PCI_TRACE((TRACE_PCI,"BAR Index=%d;\tType=%d;\tGRA=0x%lX;\tLEN=0x%lX;\tOffset=0x%X;\n", 
				i, Device->Bar[i].Type, Device->Bar[i].Gran, 
				Device->Bar[i].Length, Device->Bar[i].Offset));	
		}
	}
	PCI_TRACE((TRACE_PCI,"\n")); 
#endif
	if (EFI_ERROR(Status)) return Status;
	
	// Launch porting hook
  	Status=AmiPciLibLaunchInitRoutine(Device, isPciQueryDevice, itDevice, Device, NULL, NULL, NULL);	
	if(EFI_ERROR(Status)){
		if(Status==EFI_UNSUPPORTED){
			Status=EFI_SUCCESS;
		} else ASSERT_EFI_ERROR(Status);
	} else {
		PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X QUERY DEVICE overriden.\n\n",
				Device->Address.Addr.Bus, Device->Address.Addr.Device, Device->Address.Addr.Function,
				Device->DevVenId.VenId, Device->DevVenId.DevId));
	}
  	
	Device->Discovered=TRUE;
	return Status;
}


/**
    This function applys default Card Bus Bridge Padding. 

        
    @param Device Pointer to PCI Device Private Data structure.
    @param ResType Type of resource padding is applied.

    @retval Nothing

**/
VOID EFIAPI ApplyCrdPadding(PCI_DEV_INFO *Device, MRES_TYPE ResType){
	PCI_BRG_EXT		*ext=(PCI_BRG_EXT*)(Device+1);
	PCI_BAR			*bbar=&ext->Res[ResType];
//---------------

	bbar->Type=ResType+1;
	switch(ResType){
		case rtIo16:
			bbar->Length+=CrdBusPaddingIoLen; 
			if(bbar->Gran < CrdBusPaddingIoGra) bbar->Gran=CrdBusPaddingIoGra;
			break;
		
		case rtIo32:
			if(CPU_MAX_IO_SIZE <= 0x10000) {
				bbar->Type=tBarUnused;
				bbar=&ext->Res[ResType-1];
			}
			bbar->Length+=CrdBusPaddingIoLen;
			if(bbar->Gran < CrdBusPaddingIoGra) bbar->Gran=CrdBusPaddingIoGra;
			break;		

		case rtMmio32:
			bbar->Length+=CrdBusPaddingMmioLen; 
			if(bbar->Gran < CrdBusPaddingMmioGra) bbar->Gran=CrdBusPaddingMmioGra;
			break;

		case rtMmio32p:
			if (gAllocationAttributes & EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM) {
				bbar->Type=tBarUnused;
				bbar=&ext->Res[ResType-1];
			}
			bbar->Length+=CrdBusPaddingMmioLen;
			if(bbar->Gran < CrdBusPaddingMmioGra) bbar->Gran=CrdBusPaddingMmioGra; 
			break;
		case rtMmio64:
		case rtMmio64p:
			bbar->Type=tBarUnused;
			break;
		 default: 
			break;
	}
	
	ext->Padded=TRUE;
}


/**
    This function compares 2 128 bit integers. 

        
    @param pDestination *Pointer to the 128bit Integer to compare.
    @param pSource *Pointer to the 128bit Integer to compare.

    @retval INTN
        == 0 Destination and are Source equal; 
        >  0 Destination is bigger than Source;
        <  0 Destination is lesser than Source;
**/
INTN Cmp128Int(VOID* pDestination, VOID* pSource){
	INT64 r;
	UINT64 *d=(UINT64*)pDestination;
    UINT64 *s=(UINT64*)pSource;
//----------------------------------
    r = d[1] - s[1]; 
    if( r == 0){
        r = d[0] - s[0];
	}
	return (r>0) ? 1 : (r<0) ? -1 : 0;
}


/**
    This function compares 2 128 bit integers with respect to EFI 
    Database Engine Record to Record comparation Routine.
    When position of record gets determined. 

        
    @param pContext Pointer to the DBE_OFFSET_KEY_CONTEXT structure
    @param pRecord1 Pointer to the 128bit Integer to compare.
    @param pRecord2 Pointer to the 128bit Integer to compare.

    @retval INTN
        == 0 pRecord1 and are pRecord2 equal; 
        >  0 pRecord1 is bigger than pRecord2;
        <  0 pRecord2 is lesser than pRecord2;
**/
INTN EFIAPI Cmp128IntRR(IN VOID *pContext, VOID *pRecord1, VOID *pRecord2)
{
	DBE_OFFSET_KEY_CONTEXT *pOffsetKey = (DBE_OFFSET_KEY_CONTEXT*)pContext;
//--------------------------
    return Cmp128Int((VOID*)((INT8*)pRecord1+pOffsetKey->Offset),(VOID*)((INT8*)pRecord2+pOffsetKey->Offset));
}

/**
    This function compares 2 128 bit integers with respect to EFI 
    Database Engine Key to Record comparation Routine.
    When search for record is conducted. 

        
    @param pContext Pointer to the DBE_OFFSET_KEY_CONTEXT structure
    @param pRecord1 Pointer to the 128bit Integer to compare.
    @param pRecord2 Pointer to the 128bit Integer to compare.

    @retval INTN
        == 0 pRecord1 and are pRecord2 equal; 
        >  0 pRecord1 is bigger than pRecord2;
        <  0 pRecord2 is lesser than pRecord2;
**/
INTN EFIAPI Cmp128IntKR(IN DBE_OFFSET_KEY_CONTEXT *pContext, VOID *pKey, VOID *pRecord)
{
	DBE_OFFSET_KEY_CONTEXT *pOffsetKey = (DBE_OFFSET_KEY_CONTEXT*)pContext;
//------------------------------
	return Cmp128Int((VOID*)pKey,(VOID*)((INT8*)pRecord+pOffsetKey->Offset));
}

/**
    This function suppore to free memory allocated for index array 
    pool of the EFI Database Engine DATABASE.

        
    @param Db Pointer to the EFI Database Engine DATABASE.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI FreeResDb(DBE_DATABASE *Db){
	if(Db->IndexArray!=NULL)return pBS->FreePool(Db->IndexArray);
	else return EFI_SUCCESS;
}

/**
    This function suppore to initialize EFI Database Engine DATABASE
    with PCI_BAR pointers in decending order.

        
    @param Db Pointer to the EFI Database Engine DATABASE.
    @param Brg Bridge whose BARs must be stuffed in DATABASE.
    @param ResType Resource type of the BARs.
    @param Dev We are filling DATABASE with Bridges or Devices.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI InitResDb(DBE_DATABASE *Db, PCI_DEV_INFO *Brg, MRES_TYPE ResType, BOOLEAN Dev){
	EFI_STATUS			Status=0;
	PCI_DEV_INFO		*dev; 
	PCI_BRG_EXT			*Ext=(PCI_BRG_EXT*)(Brg+1);
	UINTN				i,j;
//------------------------------------	

	//Initialize Optimization Database
	Db->KeyCount=1;
	Db->KeyField=&gBarKey;
	Db->MemoryType=EfiBootServicesData;
	Db->RecordCount=0;

	if(Dev)Db->InitialCount=0x30; //least likely it would be more than 48 BARs of the same type
	else Db->InitialCount=0x10; //same 16 for the bridges, but anyway it would be realocated...

	Status=pBS->AllocatePool(Db->MemoryType,sizeof(VOID*)*Db->InitialCount,&Db->IndexArray);
	if(EFI_ERROR(Status)) return Status;
	
	//Now we will stuff the database with resource requirements sorted in assending order
	for(i=0; i<Ext->ChildCount; i++){
		dev=Ext->ChildList[i];

        if(dev->OutOfResRemove) continue;

		if(Dev){
			for( j=0; j<PCI_MAX_BAR_NO+1; j++){
				//Use ResType+1 Since CountBars() takes diferent type of parameter - 
				//PCI_BAR_TYPE. It mapps to MRES_TYPE as MRES_TYPE+1
				if ((dev->Bar[j].Type==ResType+1) && dev->Bar[j].Length){
					Status=DbeInsert(Db,&dev->Bar[j]);
					if(EFI_ERROR(Status)) return Status;
				}
			}//bar loop
//#if SRIOV_SUPPORT
			if(SriovCheckSriovCompatible(dev)) {
				Status=SriovDbAddVirtualBar(dev, Db, ResType);
			    if(EFI_ERROR(Status)) return Status;
			}
//#endif
		} else {
			//take care of the bridge
			if(dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg){
				PCI_BRG_EXT *ext=(PCI_BRG_EXT*)(dev+1);
			//-----------------------
				//Don't add empry bridge BARs
				if((ext->Res[ResType].Type==ResType+1) && ext->Res[ResType].Length ){
					Status=DbeInsert(Db,&ext->Res[ResType]);
					if(EFI_ERROR(Status)) return Status; 
				}
			}
		}
	} //child loop

//	if(!Db->RecordCount)Status=EFI_NOT_FOUND;

	return Status;	
}


/**
    This function suppore to fill array of BAR ORDER with actual 
    order of PCI_BARs behind the Bridge and remove added BAR structure from
    the BAR DATABASE.

        
    @param Db Pointer to the EFI Database Engine DATABASE.
    @param BrgResOrder Pointer to the Bridge Resource order structure.
    @param Bar Pointer to the BAR structure to be added.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI AppendBarOrder(DBE_DATABASE *Db, BRG_RES_ORDER *BrgResOrder, PCI_BAR *Bar){
	EFI_STATUS			Status;
//--------------------------
   	Status=AppendItemLst((T_ITEM_LIST*)BrgResOrder, Bar);
    if(EFI_ERROR(Status))return Status;

	//Remove copied record from Database it not suppose to clear the *bar variable;
	if (Db!=NULL){
	    PCI_TRACE((TRACE_PCI,"    "));
	    Status=DbeDelete(Db,Bar,FALSE);
	} else PCI_TRACE((TRACE_PCI," Fx "));
	
	PCI_TRACE((TRACE_PCI,"BAR: Bas=0x%lX; Len=0x%lX;\tGra=0x%lX;\tOfs=0x%X;Owner->[B%X|D%X|F%X] DEV_TYPE=%d;\n",
	    Bar->Base, Bar->Length, Bar->Gran, Bar->Offset, Bar->Owner->Address.Addr.Bus, 
		Bar->Owner->Address.Addr.Device, Bar->Owner->Address.Addr.Function, Bar->Owner->Type));
	
	return Status;
}

/**
    This function Checks if Bridge has fixed resources e resource request in a way it will 
    consume a optimal amount of resources.

        
    @param Brg Bridge whose BARs must be stuffed in DATABASE.
    @param ResType Resource type we are doing optimization for.

    @retval BOOLEAN
        TRUE  if Brg has rixed resource settings; 
        FALSE if NOT
**/

BOOLEAN EFIAPI CheckIfFixedBridge(PCI_DEV_INFO *Brg, MRES_TYPE ResType){
    PCI_BRG_EXT         *Ext=(PCI_BRG_EXT*)(Brg+1); //This Bridge Bridge Extension Data.
    UINTN               i;
    PCI_BAR             *bbar=&Ext->Res[ResType];
//	BRG_ALIGN_INFO      *Align=&Ext->Align[ResType];
    AMI_SDL_PCI_DEV_INFO *sdldata=Brg->AmiSdlPciDevData;
    BOOLEAN             found=FALSE;
//--------------------

    if(gNoSdlDataMode) return FALSE;

    //If no sdl data associated with this bridge this is mostly not fixed bridge or bridge behind the slot
    if(sdldata==NULL) return found;
    
    //Here check if Bridge has Fixed resources by itself with some children... or w/o
    for(i=0; i<sResMaxResType; i++){
        if(sdldata->FixedResources[i].BarType==ResType && sdldata->FixedResources[i].BarOffset==bbar->Offset){
            if(bbar->Base != sdldata->FixedResources[i].BarBase){
                PCI_TRACE((TRACE_PCI,"  BrgFixedRes Base mismatch SDL 0x%lX != 0x%lX H/W ",
                        sdldata->FixedResources[i].BarBase, bbar->Base));
                if(bbar->Base == 0 || bbar->Base > sdldata->FixedResources[i].BarBase ) bbar->Base=sdldata->FixedResources[i].BarBase;
                PCI_TRACE((TRACE_PCI,"(0x%lX used)\n",bbar->Base));
            }
            
            if(sdldata->FixedResources[i].BarLength != bbar->Length ){
                PCI_TRACE((TRACE_PCI,"  BrgFixedRes Length mismatch SDL 0x%lX != 0x%lX H/W ",
                        sdldata->FixedResources[i].BarLength, bbar->Length));
                if( bbar->Length == 0 || sdldata->FixedResources[i].BarLength > bbar->Length )bbar->Length=sdldata->FixedResources[i].BarLength;
                PCI_TRACE((TRACE_PCI,"(0x%lX used)\n",bbar->Length));
            }
            bbar->Type=sdldata->FixedResources[i].BarType;
            found=TRUE;
            //Only one bridge bar suppose to be of this "ResType"
            break;
        }
    }

    return found;
    
}


//returns 
//0 = All resources dynamic 
//1 = All resources fixed. 
//2 = Mixed mode not supported...
UINT8 EFIAPI CheckIfFixedDevices(PCI_DEV_INFO *Brg, MRES_TYPE ResType){
	PCI_BRG_EXT         *Ext=(PCI_BRG_EXT*)(Brg+1); //This Bridge Extension Data.
	PCI_DEV_INFO        *dev; 
	PCI_BAR             *bar;
    UINT8               found=0;
    UINTN               i;
//---------------------------------------------

    //1. Find all BARs whose Owners has this Brg as a Parent... and add it to the 
    if(Ext->ChildCount==0) return found;
    
    //Get all device BARs from fixed BARs Database (Sorted already)
    for(i=0; i<gFixedResBarList.ItemCount; i++){
        bar=(PCI_BAR*)(gFixedResBarList.Items[i]); 
        
        if(bar->Type != ResType+1) continue;
           
        //Get PCI Device this bar belongs to and compare to the passed Brg
        dev=bar->Owner;
        if(dev->ParentBrg==Brg){
           found=1;
        }
    }
        
    //Now do the same for NOT fixed BARs Database using AmiPciAddBarDbEntry() 
    //function that sorts data...
    for(i=0; i<gNotFixedResBarList.ItemCount; i++){
        bar=(PCI_BAR*)(gNotFixedResBarList.Items[i]); 
           
        if(bar->Type != ResType+1) continue;
           
        //Get PCI Device this bar belongs to and compare to the passed Brg
        dev=bar->Owner;
        if(dev->ParentBrg==Brg){
            if( found == 1 ) found=2;
        }
    }
 
    return found;
}

/**
    This function checks Bridge "Dynamic" Children Unevenly Aligned 

    against arrange Bridge resource request in a way it will 
    consume a optimal amount of resources.

        
    @param Brg Bridge whose BARs must be stuffed in DATABASE.
    @param ResType Resource type we are doing optimization for.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI UpdateUnevenBarOrder(PCI_DEV_INFO *Brg, MRES_TYPE ResType){
    EFI_STATUS          Status=EFI_SUCCESS;
    PCI_BRG_EXT         *Ext=(PCI_BRG_EXT*)(Brg+1); //This Bridge Bridge Extension Data.
    UINTN               i;
    BRG_ALIGN_INFO      *Align=&Ext->Align[ResType];
    PCI_BAR             *bar, *ebar, *bbar=&Ext->Res[ResType];
    BRG_RES_ORDER       *ResOrd=&Ext->BarOrder[ResType];
    BRG_RES_ORDER       no={0,0,NULL};
    BRG_RES_ORDER       *neword=NULL;
    UINT64              base;
    BOOLEAN             alignFound;  
    UINT8               bit;
//-------------------------------------------------------
    if(!ResOrd->BarCount) return Status;
    
    //get the biggest BAR.
    bar=ResOrd->BarList[0];
    base=bbar->Base;
    
    //If fixed window don't comply with alignment requirements...
    //try to find some smaller BAR that does, and put it at the first place.
    if( (base & bar->Gran) !=0 ){
        //first we need to remove EBAR from ResOrder if it was initialized.
        ebar=&Align->ExtraRes;
        if(ebar->Length!=0){
            for(i=0;i<ResOrd->BarCount; i++){
                
                bar=ResOrd->BarList[i];
                if(ebar==bar){
                    Status=DeleteItemLst((T_ITEM_LIST*)ResOrd, i, FALSE);
                    ebar->Length=0;
                }
            }    
        }

        //Currently ResOrd->BarList items arranged By Granularity and size in descending order.
        //we can check if ResOrd->BarList items may fit into bottom of the bridge fixed resource window 
        while(ResOrd->BarCount){
            //try to find among BARs in BAR order BAR with lesser Gran...
            //and feed it in the beginning of the window. 
            for(i=0,alignFound=FALSE; i<ResOrd->BarCount; i++){
                bar=ResOrd->BarList[i];
                if( (bar->Gran & base) == 0 ){
                    //if we have not done so allocate memory for new resource order information
                    if(neword==NULL) neword=&no;
                    
                    
                    base= base + bar->Length;
                    Status=AppendItemLst((T_ITEM_LIST*)neword,bar);
                    ASSERT_EFI_ERROR(Status);
					
                    alignFound=TRUE;
                    Status=DeleteItemLst((T_ITEM_LIST*)ResOrd, i, FALSE);
                    break; //now start
                }
            }
            
            //here we failed to find (any more) Alignment matching BAR(s) among bridge children. 
            //the only thing left for us is to add EBAR to compensate alignment issues issue.
            if(!alignFound || i >= ResOrd->BarCount) break;
            
        }
        
    
        //it might be that once we have resorted everything we have nothing left!
        //this is the perfect 
        if(ResOrd->BarCount){
            ebar=&Align->ExtraRes;
            bar=ResOrd->BarList[0];
         
            ebar->Length = (bar->Gran+1)-(base & bar->Gran);
             
    
            if(ebar->Length!=0){
                bit=FindFirstBit(ebar->Length-1,FALSE);
                ebar->Gran=Shr64((~0ULL),64-bit);
    
                ebar->Offset=0xFF;
                ebar->Type=ResType+1;
                ebar->DiscoveredType=ebar->Type;
    
                if(neword==NULL){ 
                    //neword==NULL: here we can't find at all initial window fitting BARs
                    //need to initialize EBAR to fill uneven alignment...and insert it before ResOrd 
                    Status=InsertItemLst((T_ITEM_LIST*)ResOrd, ebar, 0);
                } else {
                    //in this case we have some BAR(s) that can be put before the biggest one in ResOrd BAR list and they are already 
                    //moved to the neword... 
                    Status=AppendItemLst((T_ITEM_LIST*)neword, ebar);
                    ASSERT_EFI_ERROR(Status);
                    
                    //now we have to keep moving remaining BARs into neworder they are sorted already.
                    for(i=0; i>ResOrd->BarCount; i++){
                        bar=ResOrd->BarList[i];
                        Status=AppendItemLst((T_ITEM_LIST*)neword, bar);
                        ASSERT_EFI_ERROR(Status);
                    }
                }    
            }  
        }
        
        //Now some house keeping...
        if(neword!=NULL){
            ClearItemLst((T_ITEM_LIST*)ResOrd, FALSE);
            ResOrd->InitCount=neword->InitCount;
            ResOrd->BarCount=neword->BarCount;
            ResOrd->BarList=neword->BarList;
        }
    }
    
    return Status;
}


/**
    This function arrange Bridge resource request in a way it will 
    consume a optimal amount of resources.

        
    @param Brg Bridge whose BARs must be stuffed in DATABASE.
    @param ResType Resource type we are doing optimization for.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI ArrangeFixedResources(PCI_DEV_INFO *Brg, MRES_TYPE ResType){
    EFI_STATUS          Status=EFI_SUCCESS;
	PCI_DEV_INFO        *dev; //Bridge Device used to identify Bridge resources with uneven Granularity. 
	PCI_BRG_EXT         *Ext=(PCI_BRG_EXT*)(Brg+1); //This Bridge Bridge Extension Data.
	UINTN               i;
	BRG_RES_ORDER       *ResOrd=&Ext->BarOrder[ResType];
	BRG_ALIGN_INFO      *Align=&Ext->Align[ResType];
	PCI_BAR             *bar, *bbar=&Ext->Res[ResType];
	PCI_BAR             *minbar, *maxbar;
//---------------------------------------------
    if(!Ext->ChildCount && !Brg->AmiSdlPciDevData->PciDevFlags.Bits.FixedResources)return EFI_SUCCESS;

    
    PCI_TRACE((TRACE_PCI,"PciBus: ArrangeFixRes() Bridge->[B%X|D%X|F%X] PCI_BRG_EXT.Res[%d] :\n",
        				Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function, ResType));
	//1. Find all bars whose Owners has this Brg as a Parent... and add it to the 
    if(Ext->ChildCount){
        minbar=NULL;
        maxbar=NULL;
        
        //Add Bridges Resources to the list if present
        for(i=0; i<Ext->ChildCount; i++){
            PCI_BRG_EXT *ext;
        //------------------------     
            dev=Ext->ChildList[i];
                    
            if(dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg){
                ext=(PCI_BRG_EXT*)(dev+1);
                bar=&ext->Res[ResType];

                if(bar->Base!=0){
                    if(minbar==NULL){
                        minbar=bar;
                        Align->MinGran=bar->Gran;
                    } else {
                        if(bar->Base<minbar->Base){
                            minbar=bar; 
                        }
                    }
                               
                    if(maxbar==NULL){
                        maxbar=bar;
                        Align->MinGran=bar->Gran;
                    } else {
                        if(bar->Base>maxbar->Base){
                            maxbar=bar; 
                        }
                    }
                    
                    Status=AppendBarOrder(NULL, ResOrd, bar);
                    ASSERT_EFI_ERROR(Status);
					
                    if(bar->Gran>Align->MaxGran) Align->MaxGran=bar->Gran;
                    if(Align->MinGran>bar->Gran) Align->MinGran=bar->Gran;

                    
                }

            }
        }    
        
        for(i=0; i<gFixedResBarList.ItemCount; i++){
           bar=(PCI_BAR*)(gFixedResBarList.Items[i]); 
           if(bar->Type != ResType+1) continue;
           
           //Get PCI Device this bar belongs to and compare to the passed Brg
           dev=bar->Owner;
           if(dev->ParentBrg==Brg){
               //we have to monitor bottom and top Fixed resource window...  
               if(minbar==NULL){
                   minbar=bar;
                   Align->MinGran=bar->Gran;
               } else {
                   if(bar->Base<minbar->Base){
                       minbar=bar; 
                   }
               }
                          
               if(maxbar==NULL){
                   maxbar=bar;
                   Align->MinGran=bar->Gran;
               } else {
                   if(bar->Base>maxbar->Base){
                       maxbar=bar; 
                   }
               }
               
               if(bar->Gran>Align->MaxGran) Align->MaxGran=bar->Gran;
               if(Align->MinGran>bar->Gran) Align->MinGran=bar->Gran;
               
               //This is our BAR add it to the ResOrder... 
               //Bar Db already has resources arranged in descending order... 
               Status=AppendBarOrder(NULL, ResOrd, bar);
               //ASSERT_EFI_ERROR(Status);
               
               //Remove it ... from bar DB...
               Status=DeleteItemLst(&gFixedResBarList, i, FALSE);
               //since ItemsCount changed due to delete operation change "i" 
               //it will point to Items[i+1] before the delete operation
               i--;
               if(gFixedResBarList.ItemCount==0)break;
           }
        }
        //If we here all BARs that belongs to this bridge went to ResOrder[ResType] array.
        //2. Update Brg BAR Properties
        if(minbar!=NULL){
            UINT64 len;
        //-----------------
            bbar->Base=minbar->Base;
            len = maxbar->Base+maxbar->Length - minbar->Base;
            if(len>bbar->Length) bbar->Length=len;
        }
    } 
    
    return Status;
}

/**
    This function arrange Bridge resource request in a way it will 
    consume a optimal amount of resources.

        
    @param Brg Bridge whose BARs must be stuffed in DATABASE.
    @param ResType Resource type we are doing optimization for.

    @retval EFI_STATUS
        EFI_SUCCESS if everything OK; 
        an EFI_ERROR if an ERROR
**/
EFI_STATUS EFIAPI OptimizeBrgResource(PCI_DEV_INFO *Brg, MRES_TYPE ResType){
	EFI_STATUS			Status=0;
//	PCI_DEV_INFO		*dev; //Device Dtata used for iteration,
	PCI_DEV_INFO		*brg; //Bridge Device used to identify Bridge resources with uneven Granularity. 
	PCI_BRG_EXT			*Ext=(PCI_BRG_EXT*)(Brg+1),*ext; //This Bridge Bridge Extension Data.
	UINTN				i;
	PCI_BAR				*bar,*bbar,*nbbar,*nextbar; //temp BAR used in iterations, This Bridge child who is the bridge.  
	BRG_ALIGN_INFO		*balign;
	PCI_BAR				*ebar; 
	BRG_RES_ORDER		*ResOrd=&Ext->BarOrder[ResType]; //This Bridge this bridge Resource Alignment information
	DBE_DATABASE		DevDb={0,0,0,0,NULL,NULL},BrgDb={0,0,0,0,NULL,NULL};
	INT8				v;
	UINT64				val;
	UINT8				bit;
//------------------------------------	

	//No childs behind the bridge, no optimization...
	if(!Ext->ChildCount)return EFI_SUCCESS;
	PCI_TRACE((TRACE_PCI,"PciBus: OptimizeBrgRes() Bridge->[B%X|D%X|F%X] PCI_BRG_EXT.Res[%d] :\n",
			Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function, ResType));


	//Start the optimization
	//1. Create Ascending Sorted Database of all PCI Devices BARs behind the Brg of ResType.
	Status=InitResDb(&DevDb, Brg, ResType, TRUE);	
	if(EFI_ERROR(Status)) goto ExitLbl;
	
	Status=InitResDb(&BrgDb, Brg, ResType, FALSE);	
	if(EFI_ERROR(Status)) goto ExitLbl;
	
	PCI_TRACE((TRACE_PCI," BAR(s) Order for %d Device BAR(s); %d Bridge BAR(s) of BAR_TYPE=%d\n",
			DevDb.RecordCount, BrgDb.RecordCount, ResType+1));
	
	//2. Start Populating Resource Array Brg->BarOrder[ResType]
	while(DevDb.RecordCount || BrgDb.RecordCount){
	
		if(BrgDb.RecordCount){
			Status=DbeGoToIndex(&BrgDb,0,BrgDb.RecordCount-1,(VOID**)&bbar); 
			if(EFI_ERROR(Status)){
				if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
					ASSERT_EFI_ERROR(Status);
					goto ExitLbl;
				}
			}
		} else bbar=NULL;
		
		if(DevDb.RecordCount){
			Status=DbeGoToIndex(&DevDb,0,DevDb.RecordCount-1,(VOID**)&bar); 
			if(EFI_ERROR(Status)){
				if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
					ASSERT_EFI_ERROR(Status);
					goto ExitLbl;
				}
			}
		} else bar=NULL;

		//Check if THIS Bridge has a bridge childs
		if((bar!=NULL) && (bbar!=NULL)){
			//This case when we have bridge resourses among regular device resources
			brg=bbar->Owner;
			//dev=bar->Owner;
			ext=(PCI_BRG_EXT*)(brg+1);
			balign=&ext->Align[ResType];
			
			//1. Check first if Biggest resource in "BrgDb" - "bbar" has the same or lesser 
			//Alignment Requirements as a Biggest one in "DevDb" - "bar". balign->MaxGran 
			if(bar->Gran >= balign->MaxGran){
				Status=AppendBarOrder(&DevDb,ResOrd, bar);
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status)) goto ExitLbl;
			} else {
				//This is the case when we should use PCI_BRG_EXT.ExtraRes[ResType] information to accommodate 
				//resources with lower granularity to utilize space used to adjust uneven bridge granularity
				Status=AppendBarOrder(&BrgDb,ResOrd, bbar);
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status)) goto ExitLbl;
				
				//Determine how much Extra resources we will use here
				ebar=&balign->ExtraRes;
				if(bbar->Length & bbar->Gran){
					//Here we have add the biggest bridge resource to the BarOrdedr DB.
					//We have yet Biggrest Dev Resource. Let's see if there are any Bridge resources left
					if(BrgDb.RecordCount){
						Status=DbeGoToIndex(&BrgDb,0,BrgDb.RecordCount-1,(VOID**)&nbbar); 
						if(EFI_ERROR(Status)){
							if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
								ASSERT_EFI_ERROR(Status);
								goto ExitLbl;
							}
						}
					} else nbbar=NULL;
					//Now see which bar (nbbar-NextBridgeBar) if any, or bar-DeviceBar will be the next 
					//member in BarOrder Database.
					if(nbbar!=NULL){
						if(bar->Gran>=nbbar->Gran)nextbar=bar;
						else nextbar=nbbar;
					} else nextbar=bar;
					
					//Determine how much Extra resources we will use here
					//if we got here - the Bridge resource request, we just added to the BarOrder DB, has uneven alignment
					//Here we have:
					//bbar 		- already added to the BarOrder DB;
					//nextbar	- biggest resource following by the bbar;
					//bar		- biggest Device Bar evenly aligned ;
					if(nextbar->Gran <= balign->ResGran) {
						//if next biggest bar has Alignment requirements lesser or equal to the bbar just added
						//GREAT! We will just add it and forget it;
						if(nextbar==bar)Status=AppendBarOrder(&DevDb,ResOrd, nextbar);
						else Status=AppendBarOrder(&BrgDb,ResOrd, nextbar);
						
						ASSERT_EFI_ERROR(Status);
						if(EFI_ERROR(Status)) goto ExitLbl;
							
					} else {
						ebar->Length=(( bbar->Length | nextbar->Gran )+1)-bbar->Length;
						bit=FindFirstBit(ebar->Length-1,FALSE);
						ebar->Gran=Shr64((~0ULL),64 - bit);
					}
				} 

				//bar->Gran here < "bbar" MaxGran, so "bar" holds next biggest Gran after the bridge
				//we will try to adjust Extra Space requested by the bridge, analizing bar->Gran information
				if(ebar->Length) {
					while( ebar->Length >= (ebar->Gran+1) ){
						VOID*	p;
					//-----------------------

						if(!DevDb.RecordCount)break;

						bar=NULL;
						val=ebar->Gran+1;
						Status=DbeLocateKey(&DevDb,0,&val,&p,&v, &i);
						

						if(EFI_ERROR(Status)){
							if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF) || (Status==EFI_NOT_FOUND))){
								ASSERT_EFI_ERROR(Status);
								goto ExitLbl;
							}
						}
								
						Status=DbeGoToIndex(&DevDb,0,i,(VOID**)&bar); 
						if(EFI_ERROR(Status)){
							if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
								ASSERT_EFI_ERROR(Status);
								goto ExitLbl;
							}
						}

						//if DevDb (who has even sizes) has resource that smaller or equal
						//we will try to fill Extra Gap... 
						if((v == -1) && (bar!=NULL)){
							//Database sorted in ascending order. DbeLocateKey() function, parameter "v" tells how close the result is.
							//if DBE couldn't find exact match, it will return pointer at first element bigger than Parameter Passed.
							//There fore, if this is the case, we need to go down one index to get a smaller element than Parameter Passed.
							if(i>0){ 
								bar=NULL;
								Status=DbeGoToIndex(&DevDb,0,i-1,(VOID**)&bar); 
								if(EFI_ERROR(Status)){
									if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
										ASSERT_EFI_ERROR(Status);
										goto ExitLbl;
									}
								}
							} else break; //this will break "while(DevDb.RecordCount || BrgDb.RecordCount)" loop
						} //if(v==-1 && bar!=NULL)
						
						if(bar!=NULL && bar->Gran<=ebar->Gran){ 
							Status=AppendBarOrder(&DevDb,ResOrd,bar);
							ASSERT_EFI_ERROR(Status);
							if(EFI_ERROR(Status)) goto ExitLbl;
							
							//Adjust ebar->Length and Gran since we have filled out some extra space...
							ebar->Length-=bar->Length;
                            if(ebar->Length!=0){
    							bit=FindFirstBit(ebar->Length-1,FALSE);
				    			ebar->Gran=Shr64((~0ULL),64-bit);
                            } else break; //this will break "while( ebar->Length >= (ebar->Gran+1) )" loop
						} else break; //this will break "while( ebar->Length >= (ebar->Gran+1) )" loop

					} //while( ebar->Length >= (ebar->Gran+1)

					//if after all efforts we still have extra resources left
					//Mark ebar->Offset as 0xFF to notify routine which will programm the BARs
					//Not to toughch this one, but just add ebar->Length before next valid BAR. 

				    PCI_TRACE((TRACE_PCI," EBAR: Len=0x%lX;\t Gra=0x%lX;\t Ofs=0x%X; Owner [B%X|D%X|F%X] Type=%d\n",
				    ebar->Length, ebar->Gran, ebar->Offset, ebar->Owner->Address.Addr.Bus, 
				    ebar->Owner->Address.Addr.Device, ebar->Owner->Address.Addr.Function, ebar->Owner->Type));
                    if(ebar->Length!=0){
    					ebar->Offset=0xFF;
				    	ebar->Type=ResType+1;
					    ebar->DiscoveredType=ebar->Type;

    					Status=AppendItemLst((T_ITEM_LIST*)ResOrd, ebar);
				    	ASSERT_EFI_ERROR(Status);
					    if(EFI_ERROR(Status)) goto ExitLbl;
                    }  
				}//if(ebar->Length)
			}//else of "if(bar->Gran >= balign->MaxGran)"
		} else { //"if((bar!=NULL) && (bbar!=NULL))"
			//This case when we have only one type of resources most likely it would be regular device resources
			//But just in case do the check
			if(bbar==NULL) Status=AppendBarOrder(&DevDb,ResOrd,bar);
			else {
				ebar=NULL;
				//If we got only bridges (more than one) behind parent bridge we might got uneven alignment!
				//So we have to check if Granuilarity requirements satisfactory for them and padd resources if needed.
				if(BrgDb.RecordCount>1){
					UINT64  resgran;
					//---------------------
					PCI_TRACE((TRACE_PCI,"     Bridges ONLY found!!! \n"));
					brg=bbar->Owner;
					ext=(PCI_BRG_EXT*)(brg+1);
					balign=&ext->Align[ResType]; 

					bit=FindFirstBit(bbar->Length-1,FALSE);
					resgran=Shr64((~0ULL),64-bit);

					if(bbar->Gran>resgran){
						//get next record
						Status=DbeGoToIndex(&BrgDb,0,BrgDb.RecordCount-2,(VOID**)&nextbar); 
						if(EFI_ERROR(Status)){ 
							if(!((Status==EFI_DBE_EOF) || (Status==EFI_DBE_BOF))){
								ASSERT_EFI_ERROR(Status);
								goto ExitLbl;
							}
						}
						//determine extra resource size... 
						ebar=&ext->Align[ResType].ExtraRes;
						ebar->Length=(( bbar->Length | nextbar->Gran )+1)-bbar->Length;
						if(ebar->Length!=0){
							bit=FindFirstBit(ebar->Length-1,FALSE);
							ebar->Gran=Shr64((~0ULL),64-bit);				
						}
					}//if(balign->MaxGran>balign->ResGran)
				}//if(BrgDb.RecordCount>1)
				Status=AppendBarOrder(&BrgDb,ResOrd,bbar);
				if(ebar!=NULL && ebar->Length){
					ebar->Offset=0xFF;
					ebar->Type=ResType+1;
					ebar->DiscoveredType=ebar->Type;
					PCI_TRACE((TRACE_PCI," EBAR: Len=0x%lX;\t Gra=0x%lX;\t Ofs=0x%X; Owner [B%X|D%X|F%X] Type=%d\n",
					ebar->Length, ebar->Gran, ebar->Offset, ebar->Owner->Address.Addr.Bus, 
					ebar->Owner->Address.Addr.Device, ebar->Owner->Address.Addr.Function, ebar->Owner->Type));

					Status=AppendItemLst((T_ITEM_LIST*)ResOrd, ebar);
					ASSERT_EFI_ERROR(Status);
					if(EFI_ERROR(Status)) goto ExitLbl;
				}				

				ASSERT_EFI_ERROR(Status);
			}
			if(EFI_ERROR(Status)) goto ExitLbl;
		}

	}//while(DevDb.RecordCount || BrgDb.RecordCount)

ExitLbl:	
	//Free Memory allocated... 
	FreeResDb(&DevDb);	
	FreeResDb(&BrgDb);

	//if there was an ERROR clear Bridge Resource Ortder Array.	
	if(EFI_ERROR(Status)){
		ClearItemLst((T_ITEM_LIST*)ResOrd,FALSE);
		PCI_TRACE((TRACE_PCI,"PciBus: OptimizeBrgResource() returning ERROR=%r\n", Status));
	}

	return Status;

}

VOID EFIAPI ClearBrgResources(PCI_DEV_INFO *Brg){
//	EFI_STATUS			Status=0;
	PCI_DEV_INFO		*dev;
	PCI_BRG_EXT			*ext=(PCI_BRG_EXT*)(Brg+1);
	UINTN				i;
//-------------------------------

	for (i=0; i<ext->ChildCount; i++){
		dev=ext->ChildList[i];

		if((dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg) )
			ClearBrgResources(dev);
	} //child loop
        
    //Once here that means all recourcive calls are done and 
    //all BridgeChildren Bridges - clean....      
    //Clear ThisBridge Res[], Pad[], Align[]...
    for(i=rtIo16; i<rtMaxRes; i++) {
        PCI_DEV_INFO    *owner;
        //----------------------
        owner=ext->Res[i].Owner;
        pBS->SetMem(&ext->Res[i], sizeof(PCI_BAR),0);
		ext->Res[i].Owner=owner;

        owner=ext->Align[i].ExtraRes.Owner; 
        pBS->SetMem(&ext->Align[i], sizeof(BRG_ALIGN_INFO),0);
		ext->Align[i].ExtraRes.Owner=owner;
		
        if(ext->BarOrder[i].BarCount) ClearItemLst((T_ITEM_LIST*)&ext->BarOrder[i], FALSE);
    }    
}




/**
    The objective of this routine is to select the biggest
    possible Granularity/Alignment for the Bridge by adding all Child's
    resources of the same type together. 

        
    @param Brg Pointer to PCI Device Private Data structure.

    @retval Nothing

**/
VOID EFIAPI CalculateBrgResources(PCI_DEV_INFO *Brg)
{
	EFI_STATUS			Status=0;
	PCI_DEV_INFO		*dev;
	PCI_BRG_EXT			*ext=(PCI_BRG_EXT*)(Brg+1);
	UINTN				i,j,k;
	PCI_BAR				*bar, *bbar;
	UINT8 				bit;
	BRG_RES_ORDER		*resord;
	BRG_ALIGN_INFO		*balign;
	BOOLEAN				paddingappl=FALSE;
    MRES_TYPE           LowResType=rtMaxRes;
	BOOLEAN				Padd64Pf;
//-----------------------------------------------
	//Tell what we are doing
	PCI_TRACE((TRACE_PCI,"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    PCI_TRACE((TRACE_PCI,"PciBus: "));
    if(gPciOutOfRes) PCI_TRACE((TRACE_PCI," RE_"));
	PCI_TRACE((TRACE_PCI,"CalculateBrgResources() Bridge->[B%X|D%X|F%X] ---> BEGIN \n",
			Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function));
	PCI_TRACE((TRACE_PCI,"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));

    	//Init initial Alignment fields	for the special Bridge BARs
    	//every P2P brg has 4k IO and 1M MMMIO alignment
    	//Init Initial Granularity array
    	if(Brg->Type==tPciRootBrg){
    		//in case of Host Bridge Granularity Values should be provided by Root Brg
    		for(i=1; i<rtMaxRes; i++)	ext->Align[i].MinGran=1; //just don't care now
    	} else {
    		//in case of P2P Brg use default settings 
    		ext->Align[1].MinGran=P2P_BRG_IO_GRA;
    		ext->Align[2].MinGran=P2P_BRG_IO_GRA;
    		ext->Align[3].MinGran=P2P_BRG_MEM_GRA;
    		ext->Align[4].MinGran=P2P_BRG_MEM_GRA;
    		ext->Align[5].MinGran=P2P_BRG_MEM_GRA;
    		ext->Align[6].MinGran=P2P_BRG_MEM_GRA;
    	}
    
    	//Initialize fields in Bridge info Structure
    	ext->Res[rtIo16].Gran=P2P_BRG_IO_GRA;
    	ext->Res[rtIo32].Gran=P2P_BRG_IO_GRA;
    	ext->Res[rtMmio32].Gran=P2P_BRG_MEM_GRA;
    	ext->Res[rtMmio32p].Gran=P2P_BRG_MEM_GRA;
    	ext->Res[rtMmio64].Gran=P2P_BRG_MEM_GRA;
    	ext->Res[rtMmio64p].Gran=P2P_BRG_MEM_GRA;
    
    	//Init Bridge Bar offset fields
    	if(Brg->Type==tPci2PciBrg){
   		//For IO 16 Resources Decoded by the bridge
   		ext->Res[rtIo16].Offset=0x1C;
   		//For IO 32 Resources Decoded by the bridge
   		ext->Res[rtIo32].Offset=0x1C;
   		//For MMIO32 
   		ext->Res[rtMmio32].Offset=0x20;
   		//For MMIO32 PF 	
   		ext->Res[rtMmio32p].Offset=0x24;
   		//For MMIO64
   		ext->Res[rtMmio64].Offset=0x20; //P2PBridge doesnot have any MMIO64
   		//For MMIO64 PF
   		ext->Res[rtMmio64p].Offset=0x24;
   	} 
    
   	//Init Card Bus Bridge Bar offset fields
   	if(Brg->Type==tPci2CrdBrg){
   		//For IO 16 Resources Decoded by the bridge
   		ext->Res[rtIo16].Offset=0x2C;
   		//For IO 32 Resources Decoded by the bridge
   		ext->Res[rtIo32].Offset=0x34;
   		//For MMIO32 
   		ext->Res[rtMmio32].Offset=0x1C;
   		//For MMIO32 PF 	
   		ext->Res[rtMmio32p].Offset=0x24;
   		//For MMIO64
   		ext->Res[rtMmio64].Offset=0x1C; //P2Crd Bridge doesnot have any MMIO64
   		//For MMIO64 PF
   		ext->Res[rtMmio64p].Offset=0x24;//P2Crd Bridge doesnot have any MMIO64PF
   	} 

    if(gPciOutOfRes){	
//OUT_OF_RES!!                 
        //Get AmiOutOfResVar (suppose to be created by ROOT BRG Generic code or Custom HbCspAllocateResources())
        Status=AmiPciOutOfRes(&gPciOutOfResData, TRUE);
        ASSERT_EFI_ERROR(Status); //should not fail at that point!
       
        //gPciOutOfResData.Resource.Type can't be ASLRV_SPC_TYPE_BUS
        //Update resource type 
        if(gPciOutOfResData.Resource.Type==ASLRV_SPC_TYPE_IO) LowResType=rtIo16;

        if(gPciOutOfResData.Resource.Type==ASLRV_SPC_TYPE_MEM){
            if(gPciOutOfResData.Resource._GRA==32){
                LowResType=rtMmio32;
                if(gPciOutOfResData.Resource.TFlags.MEM_FLAGS._MEM!=ASLRV_MEM_UC)LowResType=rtMmio32p;
            }
            if(gPciOutOfResData.Resource._GRA==64){
                LowResType=rtMmio64;
                if(gPciOutOfResData.Resource.TFlags.MEM_FLAGS._MEM!=ASLRV_MEM_UC)LowResType=rtMmio64p;
            }
        }
    }

	//Check/Determine Out of RESOURCES remove status here and traverce to the very last bridge in the list...
	for (i=0; i<ext->ChildCount; i++){
		dev=ext->ChildList[i];

        //if we got Out Of resources condition call Platform function to determine
        //priority list for devices Platform wants to IGNORE.
        //Pick just one device at a time.
        if(gPciOutOfRes && !gPciOutOfResHit && !dev->OutOfResRemove){ 

            //Status=PciPortOutOfResourcesRemoveDevice(dev, gPciOutOfResData.Count, LowResType);
        	//Instead of line above launch init routine...
        	Status=AmiPciLibLaunchInitRoutine(dev, isPciOutOfResourcesCheck, itDevice, dev, &gPciOutOfResData.Count, &LowResType, &gPciOutOfResHit);
       		if(EFI_ERROR(Status)){
       			if(Status==EFI_UNSUPPORTED){
       				Status=EFI_SUCCESS;
       			} else ASSERT_EFI_ERROR(Status);
       		} else {
				gPciOutOfResHit=TRUE;
				dev->OutOfResRemove=TRUE;

				PCI_TRACE((TRACE_PCI,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
				PCI_TRACE((TRACE_PCI,"\nPciBus: OutOfRes Hit #%d!!! Removing Device [B%X|D%X|F%X]->",
				gPciOutOfResData.Count, dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function));

				if(dev->AmiSdlPciDevData!=NULL && (dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard==0) )
					PCI_TRACE((TRACE_PCI,"Slt #%d\n", dev->AmiSdlPciDevData->Slot));
				else PCI_TRACE((TRACE_PCI,"Slt N/A\n"));
				PCI_TRACE((TRACE_PCI,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));

				//We must remove all it's functions also since f0 defines persence of the rest.
				if(IsFunc0OfMfDev(dev)&& dev->Type!=tPci2PciBrg) {
					PCI_TRACE((TRACE_PCI,"       AND ALL IT's %d FUNCTIONS!!!\n",dev->FuncCount));
					for(j=0;j<dev->FuncCount;j++){
						dev->DevFunc[j]->OutOfResRemove=TRUE;
						DisableDeviceDecoding(dev->DevFunc[j], stDisableAll);
					}
				}
				//if we hit device from remove list - shut it down...
				DisableDeviceDecoding(dev, stDisableAll);
				if(dev->Type==tPci2PciBrg) DisableBridgeDecoding((PCI_BRG_INFO*)dev);

				//save OUT_OF_RES_VAR
				Status=AmiPciOutOfRes(&gPciOutOfResData, FALSE);
        	}

        }

        if(dev->OutOfResRemove) continue;
		//here we have to check if one of the Child devices is P2P brg - 
		//it requires a special handling
		if(dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg)
			CalculateBrgResources(dev);
	} //child loop

	PCI_TRACE((TRACE_PCI,"=>>>Resource Requirements for Bridge->[B%X|D%X|F%X] :<<<=\n",
			Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function));
	PCI_TRACE((TRACE_PCI,"ResType: 1=rtIo16; 2=rtIo32; 3=rtMmio32; 4=rtMmio32p; 5=rtMmio64; 6=rtMmio64p\n"));
	PCI_TRACE((TRACE_PCI,"------------------------------------------------------------------------\n"));
	
	
	//So, now in gFixedResBarList and gNotFixedResBarList we have all device resources sorted already by,
	//descending "Length" order in gNotFixedResBarList and acending "Base" oreder in gFixedResBarList...   
	for(j=rtIo16; j<rtMaxRes; j++){
	    BOOLEAN     bf;
	    UINT8       df;
	//    PCI_BAR     fdresbar;
	//---------------------    
        //Init variables.
        bbar=&ext->Res[j];
		balign=&ext->Align[j];
        resord=&ext->BarOrder[j];
		balign->MaxGran=bbar->Gran;

	    bf=CheckIfFixedBridge(Brg,j);
	    
        //df=0 No Fixed Devices among bridge childs...
	    //df=1 All fixed devices among bridge childs.
	    //df=2 there are fixed and dynamic devices among bridge childs.
	    //IF df==2 condition NOT SUPPORTED!!!!
	    df=CheckIfFixedDevices(Brg,j);
        if(df==2){
            PCI_TRACE((TRACE_PCI," !!!ERROR!!!: Bridge @ [B%X|D%X|F%X] has FIXED and NOT_FIXED BARs behind it!!!\n",
                        bbar->Owner->Address.Addr.Bus, bbar->Owner->Address.Addr.Device, bbar->Owner->Address.Addr.Function));
            PCI_TRACE((TRACE_PCI," !!!ERROR!!!: This mode is not supported by Software. Please Update SDL Data!!!\n"));
            ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
            EFI_DEADLOOP();        
			return;// EFI_INVALID_PARAMETER;
        } else {
            UINT64      len=0;
        //-------------------------------    
            if(df==1){
                Status=ArrangeFixedResources(Brg, j);
            } else {
                Status=OptimizeBrgResource(Brg, j);
            }
            ASSERT_EFI_ERROR(Status);       

            //Check bottom alignmend of the fixed bridge window...
            //if bridge childrens are not fixed.
            if(bf && df==0 && resord->BarCount) UpdateUnevenBarOrder(Brg,j);

            bbar->Type=j+1;
            
            for(k=0; k<resord->BarCount; k++){
                bar=resord->BarList[k];

                //Pick the biggest granularity of the bridge window
                if( bbar->Gran < bar->Gran) bbar->Gran=bar->Gran;	

                //add children's resources to the bridge resource window...
                len+=bar->Length;
                bbar->Type=bar->Type;
            }
            
            //if bridge has fixed resources bbar->Length has resource window length delivered from sdl data
            if(bf){
                //here we have to check if HW Requested resource window bigger than SDL one...
                if(bbar->Length < len){
                    PCI_TRACE((TRACE_PCI," !!!ERROR!!!: Bridge @ [B%X|D%X|F%X] has HW Resource Request 0x%lX > 0x%lXthan FIXED Resource Window!!!\n",
                                bbar->Owner->Address.Addr.Bus, bbar->Owner->Address.Addr.Device, bbar->Owner->Address.Addr.Function, len, bbar->Length));
                    PCI_TRACE((TRACE_PCI," !!!ERROR!!!: This mode is not supported by Software. Please Update SDL Data!!!\n"));
                    ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
                    bbar->Length = len;
                } 
            } else {
			    bbar->Length = len;			
			}
        }
        
		//Report Bridge Parameters acquired so far...
        PCI_TRACE((TRACE_PCI,"->ResType=%X; Bas=0x%lX\tLen=0x%lX;\tGran=%lX; MaxGran=%lX; ResGran=%lX; MinGran=%lX\n",
        j, bbar->Base, bbar->Length, bbar->Gran, balign->MaxGran, balign->ResGran, balign->MinGran));
		PCI_TRACE((TRACE_PCI,"------------------------------------------------------------------------\n"));

	} //for(j=rtIo16; j<rtMaxRes; j++)..#1 brg actual resources
	
//#if HOTPLUG_SUPPORT
	if(gPciHotPlug && (Brg->DevSetup.HpDisable==FALSE)){
	//If we have to make Padding for Hot plug lets keep in mind following things:
	//1. Bridge does not support  at all64bit MMIO - so 64bit MMIO padding makes no sense
	//2. Bridge can not support both 32 bit and 64 bit PFMMIO it can support only one of them
	//3. If bridge has some card behind it preference in PFMMIO should be given to the 
	//WIDTH(32 vs 64) of PFMMIO present in the card.
		//PCI_TRACE((TRACE_PCI,"\n"));
		Padd64Pf=TRUE; 
		if(Brg->Type==tPci2PciBrg){
			//if we have card with Mmio32pf
			if(ext->Res[rtMmio32p].Length!=0){
				Padd64Pf=FALSE; 
				PCI_TRACE((TRACE_PCI,"PciHp: WARNING! Bridge decodes 32bit PFMMIO. 64bit padding will be ignored!\n"));
			} else {
				if(ext->Res[rtMmio64p].Length!=0){
					Padd64Pf=TRUE; 
					PCI_TRACE((TRACE_PCI,"PciHp: WARNING! Bridge decodes 64bit PFMMIO. 32bit padding will be ignored!\n"));
				} else {
					//if Mmio32pf padding specified.
					if(ext->Pad[rtMmio32p].Length){
						Padd64Pf=FALSE;
						PCI_TRACE((TRACE_PCI,"PciHp: WARNING! Bridge requests 32bit PFMMIO padding.  64bit padding will be ignored!\n"));
					}
				}
			}
		}
		
		if(Brg->DevSetup.Decode4gDisable && Padd64Pf){
			Padd64Pf=FALSE;
			PCI_TRACE((TRACE_PCI,"PciHp: WARNING! Bridge does not support 64bit (Setup). 64bit padding will be ignored!\n"));
		}
		//Card Bus does not have 64bit resources
		if(Brg->Type==tPci2CrdBrg && Padd64Pf){
			Padd64Pf=FALSE;
			PCI_TRACE((TRACE_PCI,"PciHp: WARNING! CrdBus does not support 64bit. 64bit padding will be ignored!\n"));
		}

		//If customer opted use padding even when OUT OF RESOURCE condition occur... 
		if(!gPciOutOfRes || ApplyPaddingAnyway ){
			for(j=rtIo16, paddingappl=FALSE; j<rtMaxRes; j++){
				bbar=&ext->Res[j];
				switch(j){
					case rtMmio32p:
						if(!Padd64Pf) Status=HpApplyResPadding(&ext->Pad[j], bbar);
						break;
					case rtMmio64p:
						if(Padd64Pf) Status=HpApplyResPadding(&ext->Pad[j], bbar);
						break;
					case rtMmio64: break;
					default:	Status=HpApplyResPadding(&ext->Pad[j], bbar);
				}
				if(!EFI_ERROR(Status))paddingappl=TRUE;
			} //for(j=rtIo16; j<rtMaxRes; j++)..#2 brg padding Applied.
		} else PCI_TRACE((TRACE_PCI,"PciHp: !!!System in OUT OF RESOURCES mode - Padding IGNORED!!!\n"));
	}
//#endif
	//if we have a card bus brg and hotplug is disabled we will APPLY default padding
	if(Brg->Type==tPci2CrdBrg && !paddingappl) {
	    for(j=rtIo16; j<rtMaxRes; j++){ // [EIP186073+]
	        PCI_TRACE((TRACE_PCI,"->>>Applying CRD BUS Default Padding for Pci2CrdBrg [B%X|D%X|F%X]\n",
	                Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function));
            ApplyCrdPadding(Brg, j);
	    } // [EIP186073+]
	}

	//And finally calculate resource alignment of the bridge
	//PCI_TRACE((TRACE_PCI,"\n"));
	PCI_TRACE((TRACE_PCI,"------------------------------------------------------------------------\n"));
	PCI_TRACE((TRACE_PCI,"PciBus: Update Bridge Alignment and Granularity\n"));
	PCI_TRACE((TRACE_PCI,"ResType: 1=rtIo16; 2=rtIo32; 3=rtMmio32; 4=rtMmio32p; 5=rtMmio64; 6=rtMmio64p\n"));
	PCI_TRACE((TRACE_PCI,"------------------------------------------------------------------------\n"));
	
	for(j=rtIo16; j<rtMaxRes; j++){
		//Init variables.
		bbar=&ext->Res[j];
		balign=&ext->Align[j];
		resord=&ext->BarOrder[j];
		balign->MaxGran=bbar->Gran;

		//if Brg have resource request
		if(bbar->Length){
			if( (bbar->Length < (balign->MinGran + 1)) || ((bbar->Length & balign->MinGran)) )
				bbar->Length=(bbar->Length | balign->MinGran)+1;

			//Figure out Resource Delivered Granularity
			bit=FindFirstBit(bbar->Length-1,FALSE);
			balign->ResGran=Shr64((~0ULL),64-bit);
		}
		//Report Bridge Parameters
		PCI_TRACE((TRACE_PCI,"->ResType=%X; Bas=0x%lX\tLen=0x%lX;\tGran=%lX; MaxGran=%lX; ResGran=%lX; MinGran=%lX\n",
		j, bbar->Base, bbar->Length, bbar->Gran, balign->MaxGran, balign->ResGran, balign->MinGran));

	} //for(j=rtIo16; j<rtMaxRes; j++)..#3 calculate alignment.

	//Tell what we are doing
	PCI_TRACE((TRACE_PCI,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
	PCI_TRACE((TRACE_PCI,"PciBus: CalculateBrgResources() Bridge->[B%X|D%X|F%X] ---> END \n",
			Brg->Address.Addr.Bus, Brg->Address.Addr.Device, Brg->Address.Addr.Function));
	PCI_TRACE((TRACE_PCI,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n"));

	//We did it ... EXIT!
}

//================================================
/**
    Allocats Space nedded for New PCI_Device Instance and inits
    some of it's Fields

        
    @param BrgDevice Indicator that New Device is a BRIDGE Device.  
    @param DevArrd Device Address on PCI Bus.

    @retval PCI_DEV_INFO*
        Pointer to the Created PCI Device Private Data structure.
        NULL, if there was an ERROR.
**/

PCI_DEV_INFO* EFIAPI NewDevice(BOOLEAN BrgDevice, PCI_CFG_ADDR *DevAddr)
{
	UINTN			i;
	PCI_DEV_INFO	*dev;
	PCI_BRG_EXT		*ext;
//------------------------------------
	if(BrgDevice)i=sizeof(PCI_BRG_INFO);
	else i=sizeof(PCI_DEV_INFO);

	dev=MallocZ(i);
	if(!dev) return dev;

	//Specify Owner for BusSpecific Override Protocol
	dev->BusOvrData.Owner=dev;	
    dev->LoadFileData.Owner=dev;	
    dev->Signature=AMI_PCI_SIG;
    dev->MajorVersion = PCI_BUS_MAJOR_VER;
	dev->MinorVersion = PCI_BUS_MINOR_VER;
	dev->Revision = PCI_BUS_REVISION;
	dev->PciInitProtocol=gPciInitProtocol;
	dev->PciPortProtocol=gPciPortProtocol;

	dev->DevSetup=gPciDefaultSetup->PciDevSettings;

	//init some fields which must not be ZERO
	for(i=0; i<PCI_MAX_BAR_NO+1; i++)dev->Bar[i].Owner=dev;
	if(DevAddr){
		dev->Address.ADDR=DevAddr->ADDR;	
		dev->Address.Addr.Register=0;
	} else dev->Address.ADDR=0;	

    //Initialize some static fields
	if(BrgDevice){
		dev->Type=tPci2PciBrg;
		ext=(PCI_BRG_EXT*)(dev+1);	
		for(i=0; i<rtMaxRes; i++) {
			ext->Res[i].Owner=dev;
			ext->Align[i].ExtraRes.Owner=dev;
		}
	} else dev->Type=tPciDevice;

	return dev;
}

/**
    Adds Pointer at the child PCI Device to the "Bridge" 
    T_ITEM_LIST Child List Structure of the BRG_EXT.
    some of it's Fields

        
    @param Brgidge Pointer to PCI Device Private Data structure
        of the Parent Bridge.
    @param Child Pointer to PCI Device Private Data structure.
        of the Child Device.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
 
**/

EFI_STATUS EFIAPI BridgeAddChild(PCI_DEV_INFO *Bridge,PCI_DEV_INFO *Child)
{
	PCI_BRG_EXT		*ext=(PCI_BRG_EXT*)(Bridge+1);
//-------------------------------------------------
	return AppendItemLst((T_ITEM_LIST*)&ext->InitialCount,Child);
}


/**
    Initializes missing fields in PCI_DEV_INFO structure.

        
    @param Dev Pointer to PCI Device Private Data structure
        to initialize.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS EFIAPI InitDevData(PCI_DEV_INFO *Dev, PCI_DEV_INFO *Parent, UINT32 VenDevId, UINT32 ClassCodes, PCI_DEV_INFO *Func0){
    EFI_STATUS      Status;
	PCI_CFG_ADDR	addr;
//-------------------------
    //Get CMD Register's safe value per UEFI 2.1 spec
    addr.ADDR=Dev->Address.ADDR;

	Dev->ParentBrg=Parent;
    Dev->DevVenId.DEV_VEN_ID=VenDevId;
    Dev->Class.DEV_CLASS=ClassCodes;				
    Dev->HostData=Parent->HostData;
    Dev->RbIo=Parent->RbIo;
    Dev->Func0=Func0;

	addr.Addr.Register=PCI_INTPIN;
	Status=PciCfg8(Dev->RbIo,addr,FALSE,&Dev->IrqPinReg);
	ASSERT_EFI_ERROR(Status);
	Dev->IrqPinReg&=0x0F;
    
    //If (Func0==NULL && FuncCount==0) function is a single function device, following fields are not used and reserved;
    //If (Func0!=NULL && FuncCount==0) function is one of the Func1..Func7 of multi-func device, Func0 points on DevFunc0;
    //If (Func0!=NULL && (FuncCount!=0 || FuncInitCnt!=0)) function is Func0 of multi-func device DevFunc holds pointers at all other Func1..7 found yet

    //If (Func0==NULL && FuncCount!=0) Illegal combination - reserved!
    if(Func0!=NULL){
        if (Func0!=Dev){
            //This is func 1..7 of the device... add it to the list.
            Status=AppendItemLst((T_ITEM_LIST*)&Func0->FuncInitCnt,Dev);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) return Status;
        } else {
            //This is Func 0 if the device.. init Func0->FuncInitCnt with 8
			//this NOT zero value will be used in IsFunc0OfMfDev() and IsFunc0() 
            Func0->FuncInitCnt=8;
        }
    }

    addr.Addr.Register=PCI_CMD;
    Status=PciCfg16( Dev->RbIo, addr, FALSE, &Dev->CmdReg.CMD_REG);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

	Status=MakePciDevicePath(Dev);

    return Status;
}

/**
    Enumerate the PCI BUSes behind the "ParentBrg" and determine 
    how much resources needed for all its Child devices.

        
    @param ParentBrg Pointer to PCI Device Private Data structure
        of the Parent Bridge.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
 
**/

EFI_STATUS EFIAPI EnumerateBus(PCI_DEV_INFO *ParentBrg)
{
	EFI_STATUS		Status, Status1=EFI_NOT_FOUND;
	PCI_CFG_ADDR	pcidev;
	BOOLEAN			mf,ari; //multi-function flag; ARI Flag
	UINT32			id, cc; //Device ID Vendor ID reg
	UINT8			ht; //Header type reg
	PCI_BRG_EXT		*ext, *parext;
	PCI_DEV_INFO	*dev, *func0, *func0ari;
	UINT8			omb=0;
#if PCI_DEV_REVERSE_SCAN_ORDER
	UINT8            DevBuff;
#endif
//----------------------------------	
    PROGRESS_CODE(DXE_PCI_BUS_ENUM);
	//Root Bridges Will have Same Number for Primary and  Secondary bus
	pcidev.ADDR=PCI_VID;
	parext=(PCI_BRG_EXT*)(ParentBrg+1);
	pcidev.Addr.Bus=(UINT8)parext->Res[rtBus].Base;
    func0ari=NULL;
    ari=FALSE;

	for(pcidev.Addr.Device=0; pcidev.Addr.Device<=PCI_MAX_DEVICE; pcidev.Addr.Device++){
#if PCI_DEV_REVERSE_SCAN_ORDER
        DevBuff = pcidev.Addr.Device;
        pcidev.Addr.Device = PCI_MAX_DEVICE - pcidev.Addr.Device;
#endif
        //if ARI Detected and Enabled we should scan all devices and functions 
        //but in ARI Mode Dev0..31 and Func0..7 will be translated for PCIe as Dev0 Func 0...255 
        //So Func0 must be the same as we have detected when started from Actual Function 0

        mf=FALSE;  
        func0=NULL;

		for(pcidev.Addr.Function=0;  pcidev.Addr.Function<=PCI_MAX_FUNC; pcidev.Addr.Function++){
			//read devid-vendid register pare,
			id=0;
			pcidev.Addr.Register=PCI_VID;
			
			//Root Complexes that implement CRS Software Visibility have the ability to report the receipt of CRS 
			//Completion Status to software, enabling software to attend to other tasks rather than being stalled 
			//while the device completes its self-initialization. Software that intends to take advantage of this 
			//mechanism must ensure that the first access made to a device following a valid reset condition is a 
			//Configuration Read Request accessing both bytes of the Vendor ID field in the device's Configuration 
			//Space header. For this case only, the Root Complex, if enabled, will synthesize a special read-data 
			//value for the Vendor ID field to indicate to software that CRS Completion Status has been returned 
			//by the device. For other Configuration Requests, or when CRS Software Visibility is not enabled, the 
			//Root Complex will generally re-issue the Configuration Request until it completes with a status other 
			//than CRS as described in Section 2.3.2.
			
			//Section 2.3.2 PCIe Base 4.0 spec.
			//For a Configuration Read Request that includes both bytes of the Vendor ID field of a device Function's 
			//Configuration Space Header, the Root Complex must complete the Request to the host by returning a 
			//read-data value of 0001h for the Vendor ID field and all "1"s for any additional bytes included in 
			//the request. This read-data value has been reserved specifically for this use by the PCI-SIG and does 
			//not correspond to any assigned Vendor ID.
			
			Status=PciCfg32(ParentBrg->RbIo,pcidev,FALSE,&id); 
			if(PcieCrsSupport){
				//Check if we got CRS Status....
				while(id==0xFFFF0001){
					pBS->Stall(PCI_T_RST_RECOVERY);
					Status=PciCfg32(ParentBrg->RbIo,pcidev,FALSE,&id); 
				}
			}
			// A config write is required in order for the device to re-capture the Bus number,
            // according to PCI Express Base Specification, 2.2.6.2
            // Write to a read-only register VendorID to not cause any side effects.
            //
			if(PciWriteVidDid)Status=PciCfg32(ParentBrg->RbIo,pcidev,TRUE,&id); 
            // let's tell what we are doing...
            PCI_TRACE((TRACE_PCI,"\n==========================================================================\n"));
            PCI_TRACE((TRACE_PCI,"PciBus: Probing PCI Device @ [B%X|D%X|F%X]...", 
                    pcidev.Addr.Bus, pcidev.Addr.Device, pcidev.Addr.Function));
            Status=PciCfg32(ParentBrg->RbIo,pcidev,FALSE,&id);
            
            //ARM SYSTEMS MAY RETURN AN ERROR on config access to not existing devices
            if(EFI_ERROR(Status)){
                PCI_TRACE((TRACE_PCI,"...%r\n",Status));
                if(mf) continue;
                else break;
            }

            //Check if all Ones  
			if( (id == 0xFFFFFFFF)&& //only if first attemt of CFG read did not find anything
				PcieCheckPcieCompatible(ParentBrg) && //and device is PCIe capable
#if PCI_DEV_REVERSE_SCAN_ORDER
                (DevBuff==0) && (pcidev.Addr.Function==0) && //for PCIe dev0 func0 is only option
#else
                (pcidev.Addr.Device==0) && (pcidev.Addr.Function==0) && //for PCIe dev0 func0 is only option
#endif
                (ParentBrg->PciExpress->PcieCap.SlotImpl)) //and we have a slot there...
            {
                //Some cards does not reply on configuration transactions 
                //for some time after opening config space behind the bridge.
                //If slot is implemented behind the bridge -
                //doiblecheck if card present using PCIe facilities (SLOT_STATUS reg)
                //we will save time on skipping DEV 1..31 though
                Status=PcieDoubleCheckCard(ParentBrg,&pcidev,&id);
                ASSERT_EFI_ERROR(Status);     
            } 

			if(id == 0xFFFFFFFF || id==0){
	            PCI_TRACE((TRACE_PCI,"...%r\n",Status));
				if (!mf)break;	//devX.fun0.reg=0 returns 0xFFFFFFFF nothing is there 
				else continue;  //if device was identified as multifunc keep scanning
			}	
			//here we got something alive there
            Status1=EFI_SUCCESS;
			ht=0;
			//read the HeaderType Reg
			pcidev.Addr.Register=EFI_FIELD_OFFSET(PCI_COMMON_REG,HeaderType);
			Status=PciCfg8(ParentBrg->RbIo,pcidev,FALSE,&ht);	
			if(EFI_ERROR(Status)) return Status; //Error happend cant continue
            
    		if(pcidev.Addr.Function==0)	{
                if(ht & HDR_TYPE_MULTIFUNC)mf=TRUE; //this is multifunc device 
            }
    
    	    ht&=3;
            PCI_TRACE((TRACE_PCI,"...%r, mf=%d; ht=%X\n",Status, mf, ht));

			//Get the class code and revision Id
			pcidev.Addr.Register=PCI_RID;  //Rev ID and Class  Code
			Status=PciCfg32(ParentBrg->RbIo,pcidev,FALSE,&cc);
			if(EFI_ERROR(Status)) return Status; //Error happend can't continue

			switch (ht){
				case HDR_TYPE_DEVICE	: 
					//Allocate Space to accomodate found device
					dev=NewDevice(FALSE,&pcidev);
					if(!dev) return EFI_OUT_OF_RESOURCES;
					dev->FixedBusShift=ParentBrg->FixedBusShift;
					
					//First time when we come here even if ARI capable ari will be FALSE due to initialization
					//Mark function 0 now if mf and ari will remain FALSE we will clear it at the end of each case when ARI Check is done.
                    if((ari==FALSE) && (pcidev.Addr.Function==0)){
						if(mf)func0=dev;
						if(pcidev.Addr.Device==0) func0ari=dev;
                    }
					
                    //InitDevData() will initialize dev->Func0 and Dev->Func[]
					if(ari) Status=InitDevData(dev, ParentBrg, id, cc, func0ari);
					else Status=InitDevData(dev, ParentBrg, id, cc, func0);
					if(EFI_ERROR(Status)) goto EEXIT;

					//Test the class code to mark host brg device
					if( dev->Class.SubClassCode==0 && 
                        dev->Class.BaseClassCode==6 &&
						dev->Class.ProgInterface==0 )
                    {
                        dev->Type=tPciHostDev;
                    }
                    //Find PCI Bus SDL Data corresponded to this bridge
					FindSdlEntry(dev);

#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
					//get Setup Settings if any (was updated with Default settings at NewDev() function)
					if(dev->AmiSdlPciDevData!=NULL && dev->AmiSdlPciDevData->PciDevFlags.Bits.HasSetup){
						Status=AmiPciGetPciDevSetupData(&dev->DevSetup,
								dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
						ASSERT_EFI_ERROR(Status);
					}
#endif					
					
					//Check if OEM wants this device to be skipped from enumeration and probing...
					Status=AmiPciLibLaunchInitRoutine(dev, isPciSkipDevice, itDevice, dev, NULL, NULL, NULL);
					if(EFI_ERROR(Status)){
						if(Status==EFI_UNSUPPORTED){
							Status=EFI_SUCCESS;
						} else ASSERT_EFI_ERROR(Status);
					} else {
				        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X SKIPPED from enumeration.\n\n",
				        		dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,
				        		dev->DevVenId.VenId, dev->DevVenId.DevId));
				        dev->SkipDevice=TRUE;
					}

					//Check if we got a debug port here
#if EFI_DEBUG || USB_DEBUG_TRANSPORT
					if( 
                        (gDbgPortHob && (
							(gDbgPortInfo.DebugPortType==USB2_EHCI) ||
							(gDbgPortInfo.DebugPortType==USB2_UHCI) ||		
							(gDbgPortInfo.DebugPortType==USB2_OHCI)	) 
                        ) && (
					    	(pcidev.Addr.Function==gDbgPortInfo.Address.Addr.Function) && 
					   		(pcidev.Addr.Device==gDbgPortInfo.Address.Addr.Device) && 
							(pcidev.Addr.Bus==gDbgPortInfo.Address.Addr.Bus)
						)
					)
					{
						dev->Bar[gDbgPortInfo.BarIndex].Length	=gDbgPortInfo.Length;
						dev->Bar[gDbgPortInfo.BarIndex].Gran	=gDbgPortInfo.Length-1;
						dev->Bar[gDbgPortInfo.BarIndex].Offset	=gDbgPortInfo.BarOffset;
						dev->Bar[gDbgPortInfo.BarIndex].Type	=gDbgPortInfo.BarType;
						dev->DebugPort=TRUE;
						dev->Enumerated=TRUE;
						dev->Discovered=TRUE;
					} 
                    else 
#endif
					{
                        if(dev->SkipDevice){
                            //if porting hook tells us to skip this device.
                            //We will just start it 
                            dev->Assigned=TRUE;
                            dev->Discovered=TRUE; 
                            dev->Enumerated=TRUE;
                            dev->Incompatible=TRUE;
                        } else {                                            
                            //Vars we need here
                            UINTN   i;
                            UINT64  tl32, tl64, tlIO;
                            //-------------
    						//make sure - Device is disabled
				    		Status=DisableDeviceDecoding(dev, stMemIoSpace);
    						if(EFI_ERROR(Status)) goto EEXIT;

    						//Try to determine how mutch resources Device consumes
				    		Status=QueryPciDevice(dev);
						    if(EFI_ERROR(Status)) goto EEXIT;

                            //This workaround done for PCI Compliance Test... 
                            //32 bit, 64bit and IO resource request excides certain length. 
                            for(i=0,tlIO=0,tl32=0,tl64=0; i<PCI_MAX_BAR_NO+1; i++){
                                if((dev->Bar[i].Type!=tBarUnused) && (dev->Bar[i].Length!=0) ){
                                    switch (dev->Bar[i].Type){
                                        case tBarIo16:
                                        case tBarIo32:
                                            tlIO+=dev->Bar[i].Length;
                                            break;
                                        case tBarMmio32:
                                        case tBarMmio32pf:
                                            tl32+=dev->Bar[i].Length;
                                            break;
                                        case tBarMmio64:
                                        case tBarMmio64pf:
                                            tl64+=dev->Bar[i].Length;
                                        default: 
                                        	break;
                                    }//switch
                                }//if not empty
                            } //for

                            //Check if Calculated total resource request falls in acceptable range
                            if( (tlIO>=PCI_DEVICE_IO_RESOURCE_THRESHOLD) ||
                                (tl32>=PCI_DEVICE_32BIT_RESOURCE_THRESHOLD) ||
                                (tl64>=PCI_DEVICE_64BIT_RESOURCE_THRESHOLD)
                                )
                            {
                                PCI_TRACE((TRACE_PCI,"\nPciBus: !!!RES TOO BIG WARNING!!! Device @[B%X|D%X|F%X], VID=%X, DID=%X\n",
                                        dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,
                                        dev->DevVenId.VenId, dev->DevVenId.DevId));
                                PCI_TRACE((TRACE_PCI,"        !!!Adjust xx_RESOURCE_THRESHOLD tokens or remove the card!!!\n"));
                                //Zero out all BARs and ROM BAR.
                                for(i=PCI_BAR0, id=0; i<PCI_CAPP; i+=4){
                               	    pcidev.Addr.Register=(UINT8)i;
                                    Status=PciCfg32(ParentBrg->RbIo,pcidev,TRUE,&id);
            	                    if(EFI_ERROR(Status)) goto EEXIT;
                                }
                                //Clear CMD_REG
                                pcidev.Addr.Register=(UINT8)PCI_CMD;
                                Status=PciCfg16(ParentBrg->RbIo,pcidev,TRUE,(UINT16*)(&id));
      	                       if(EFI_ERROR(Status)) goto EEXIT;

                                //Clear All collected resource information for that device
                                //to avoid them to be added to the system's resource request.
                                    for(i=0; i<PCI_MAX_BAR_NO+1; i++){
                                    dev->Bar[i].Type=tBarUnused;
                                    dev->Bar[i].DiscoveredType=tBarUnused;    
                                    dev->Bar[i].Length=0;
                                    dev->Bar[i].Gran=0;
                                }
                                //Setting Flags telling not to install PciIo protocol instance
                                //on this device and don't touch device at all! 
                                dev->Started=TRUE;
                                dev->Assigned=TRUE;
                                dev->Discovered=TRUE; 
                                dev->Enumerated=TRUE;
                                dev->RomBarError=TRUE;
                                dev->Incompatible=TRUE;
                            } //if .. Device requests too much resources.

                            //Check if we have to enable ARI here.
                            if(Pcie2CheckPcie2Compatible(dev) && dev->Address.Addr.Device==0 && dev->Address.Addr.Function==0){
                                Status=Pcie2CheckAri(dev,&mf,&ari);
        					    if(EFI_ERROR(Status)) goto EEXIT;
                            }

							//If nor ARI no MultiFunction feature detected clear Func0 to have IsFunc0() work correctly
                            if(ari && func0ari==NULL) ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
                            if(!(mf||ari)){
                                dev->Func0=NULL;
                                func0=NULL;
                            }
                            
                            //Find IRQ Routing Entry for discovered device
                            Status = CreateDevIrqEntry(dev);

                            //Set the "Enumerated" flag to not do it again
                            dev->Enumerated=TRUE;
                    
                        }
					}
					break;

				case HDR_TYPE_P2P_BRG	: 
				case HDR_TYPE_P2C_BRG	: 
					//Allocate space for new bridge struct
					dev=NewDevice(TRUE,&pcidev);
                    if(!dev) return EFI_OUT_OF_RESOURCES;
					dev->FixedBusShift=ParentBrg->FixedBusShift;
					ext=(PCI_BRG_EXT*)(dev+1);	
					if(ht==HDR_TYPE_P2C_BRG)dev->Type=tPci2CrdBrg;
					
					//Record some properties of the discovered device
                    //First time when we come here even if ARI capable ari will be FALSE due to initialization
                    //Mark function 0 now if mf and ari will remain FALSE we will clear it at the end of each case when ARI Check is done.
                    if((ari==FALSE) && (pcidev.Addr.Function==0)){
						if(mf)func0=dev;
						if(pcidev.Addr.Device==0) func0ari=dev;
                    }
					
                    //InitDevData() will initialize dev->Func0 and Dev->Func[]
					if(ari) Status=InitDevData(dev, ParentBrg, id, cc, func0ari);
					else Status=InitDevData(dev, ParentBrg, id, cc, func0);
					if(EFI_ERROR(Status)) goto EEXIT;

                    //Find PCI Bus SDL Data corresponded to this bridge
                    FindSdlEntry(dev);
	
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
					//get Setup Settings if any (was updated with Default settings at NewDev function)
					if(dev->AmiSdlPciDevData!=NULL && dev->AmiSdlPciDevData->PciDevFlags.Bits.HasSetup){
						Status=AmiPciGetPciDevSetupData(&dev->DevSetup,
								dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
					}
#endif					
					Status=AmiPciLibLaunchInitRoutine(dev, isPciSkipDevice, itDevice, dev, NULL, NULL, NULL);
					if(EFI_ERROR(Status)){
						if(Status==EFI_UNSUPPORTED){
							Status=EFI_SUCCESS;
						} else ASSERT_EFI_ERROR(Status);
					} else {
				        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X SKIPPED from enumeration.\n\n",
				        		dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,
				        		dev->DevVenId.VenId, dev->DevVenId.DevId));
				        dev->SkipDevice=TRUE;
					}

					if(dev->SkipDevice){
                        //if porting hook tells us to skip this device.
                        //We will just start it 
                        dev->Assigned=TRUE;
                        dev->Discovered=TRUE; 
                        dev->Enumerated=TRUE;
                        dev->Incompatible=TRUE;
                    } else {
                        //make sure - Device is disabled
                        if(dev->Type == tPci2PciBrg) DisableBridgeDecoding((PCI_BRG_INFO*)dev);
				    	Status=DisableDeviceDecoding(dev, stMemIoSpace);
    					if(EFI_ERROR(Status)) goto EEXIT;
					
					    //Try to determine how mutch resources Device consumes
					    Status=QueryPciDevice(dev);
					    //this call must fill Bar[0..1] 
					    if(EFI_ERROR(Status)) goto EEXIT;
                    }
                    

                    //Check if we have to enable Ari in case of 
                    if( Pcie2CheckPcie2Compatible(dev) && (PcieIsDownStreamPort(dev)==FALSE) 
                        && (dev->Address.Addr.Device==0) && (dev->Address.Addr.Function==0)){
#if PCI_BUS_SKIP_BRG_RECURSIVELY
                        if(dev->SkipDevice==0){
#endif
                            Status=Pcie2CheckAri(dev,&mf,&ari);
                            if(EFI_ERROR(Status)) goto EEXIT;

#if PCI_BUS_SKIP_BRG_RECURSIVELY
                        }
#endif
                    }

                    //If nor ARI no MultiFunction feature detected clear Func0 to have IsFunc0() work correctly
                    if(ari && func0ari==NULL) ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
                    if(!(mf||ari)){
                        dev->Func0=NULL;
                        func0=NULL;
                    }
                    
                    //IRQ Routing Entry for discovered device
                    Status = CreateDevIrqEntry(dev);

                    //KeepUpdating NewBrg
                    //Programm Primary and Secondary I/F Bus Numbers
                    //To store Bridge SecondaryBusNo and SubordinateBusNo we will use PCI_BAR structure
                    //Device.Address.Addr.Bus will have PrimaryBusNo 
                    //Bar.Base will be used for SecondaryBusNo and 
                    //Bar.Base +Bar.Length-1 will give us SubordinateBusNo. 
                    //So Record the Secondary Bus we will use for this Bridge..  
                    
                    //Set Bridge Bus register Proprties
                    ext->Res[rtBus].Type=tBarBus;
                    ext->Res[rtBus].Offset=PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET; //SecondaryBusOffset

                    //If fixed bus assignment we should handle situation differently
                    //get secondary bus number from SDL Data
                   	if(dev->AmiSdlPciDevData != NULL && dev->AmiSdlPciDevData->PciDevFlags.Bits.FixedBus && FixedBusAssign) {
                    	if( mMaxBusFound >= (UINT8)((UINT16)dev->FixedBusNo + dev->FixedBusShift)) {
                            //With Fixed Bus allocation this condition signifies an error if we here
                            //that means that we have found more buses than allowed buy fixed bus layout
                            //So scream about it.
                            PCI_TRACE((TRACE_PCI,"\nPciBus: Can't apply Fixed Buses for the Next Bridge @ [B%X|D%X|F%X]:\n  Actual MAX Bus Discovered =%X; Proposed START Bus for That Bridge = %X\n    !!!WARNING!!! Will use Dynamic Assignment for the next Bridge on This Bridge level!!!\n\n",
                                    dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,
                                    mMaxBusFound, dev->FixedBusNo + dev->FixedBusShift));
                            dev->AmiSdlPciDevData->Bus=0;
                            dev->AmiSdlPciDevData->PciDevFlags.Bits.FixedBus=0;
                            mMaxBusFound++;
                    	} else {
                    	    mMaxBusFound =(UINT8)((UINT16)dev->FixedBusNo + dev->FixedBusShift);
                    	}
                    } else {
    					//Bridge has been found just increase bus counter
				    	mMaxBusFound++;
					}

                    ext->Res[rtBus].Base = mMaxBusFound;					
                    //Set Sub Bus # for this bridge to max # available
				    ext->Res[rtBus].Length=mMaxBusScan-ext->Res[rtBus].Base;
                    // if(ext->XlatTblEntry){
                    //    ext->XlatTblEntry->BusRun=mMaxBusFound;
                    //}
        
                    if(mMaxBusReport < mMaxBusFound) mMaxBusReport=mMaxBusFound;

#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    if(dev->SkipDevice==0){
#endif
					Status=MapBridgeBuses(dev);
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    }
#endif
					ASSERT_EFI_ERROR(Status);
					if(EFI_ERROR(Status)) goto EEXIT;

//Exclude Hotplug support 
//#if HOTPLUG_SUPPORT
					omb=mMaxBusFound; //save the old number of buses found only in case of hotplug
					//this will try to init Root HPC siting behind this bridge if any...
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    if(dev->SkipDevice==0){
#endif
					Status=HpCheckRootHotplug(dev,mMaxBusFound);
					ASSERT_EFI_ERROR(Status);

#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    }
#endif
					//if root hpc init fail for any reason we just must keep going
//#endif//HOTPLUG_SUPPORT

					//Enter a Recursive call in both cases 
                    //Only in case Device is identifyes itself as Bridge
					if(dev->Class.BaseClassCode == PCI_CL_BRIDGE){
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                        if(dev->SkipDevice==0){
#endif
                            Status = EnumerateBus(dev);
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                        }
#endif

                        if(Status!=EFI_NOT_FOUND){
    					    ASSERT_EFI_ERROR(Status);
				    	    if(EFI_ERROR(Status)) goto EEXIT;
                        } else Status=EFI_SUCCESS;
                        //After we come back from enumeration update possible Empty Slots with Setup Falag Set...
//#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
                        //SetupItem Database must have reference to the parenting bridge as a ParentIndex...
//                        if(dev->AmiSdlPciDevData!=NULL) CheckEmptySetupSlot(dev);
//#endif
					}

//-------------------------------------------------------------------------
//Exclude Hotplug support 
//#if HOTPLUG_SUPPORT
					//here if this is the bridge who has hotplugable slots
					//we must appy resource padding to it
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    if(dev->SkipDevice==0){
#endif
                    	Status=HpApplyBusPadding(dev, omb, &mMaxBusFound);

#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    }
#endif
//#endif //HOTPLUG_SUPPORT
//-------------------------------------------------------------------------
					//when we come back from EnumerateBus and have applyed padding for Bus Resources 
					// - mMaxBusFound will effectively reflect subordinate bus number for this Bridge so...

                    //If fixed bus assignment we should handle situation differenty
                    if( FixedBusAssign &&
                        (dev->AmiSdlPciDevData != NULL) &&
                        (dev->AmiSdlPciDevData->PciDevFlags.Bits.FixedBus==1))
                    {
                        UINT8   				busnum;
					//-----------------------------------
						if(DecodeFullBusRanges)
						{
	                    	//-------------------------                        
	                        Status=FindLastBusOfThisBridge(dev, &busnum);
	                        if(EFI_ERROR(Status))
                        	{
	                        	PCI_TRACE((TRACE_PCI, "***DecodeFullBusRanges ERROR. Using fallback bus 0x%X***\n", mMaxBusScan));
	                        	busnum=(UINT8)mMaxBusScan;
                        	}
                        	mMaxBusFound=busnum;
						}
                      	PCI_TRACE((TRACE_PCI,"\nPciBus: Applying Fixed Buses Bridge @ [B%X|D%X|F%X]: SecBus# = %X; SubBus# = %X\n",
                        			dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,
                        			ext->Res[rtBus].Base, mMaxBusFound));
					}
					ext->Res[rtBus].Length=mMaxBusFound-ext->Res[rtBus].Base+1;
    
                    if(mMaxBusReport < mMaxBusFound) mMaxBusReport=mMaxBusFound;
 						
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    if(dev->SkipDevice==0){
#endif
					Status=SetSubBus(dev,mMaxBusFound);
#if PCI_BUS_SKIP_BRG_RECURSIVELY 
                    }
#endif

					ASSERT_EFI_ERROR(Status);
					if(EFI_ERROR(Status)) goto EEXIT;

//-------------------------------------------------------------------------
//Enables PCI Express Handling only if PCI Express Base is Defined and !=0
//#if PCI_EXPRESS_SUPPORT
					//Now when we come back we can power off PCIExpress Empty slot to make it 
					//cpable for Hot Plugging

                    if(PcieCheckPcieCompatible(dev)){//dev->PciExpress!=NULL){
                        //Check if we hit a DOWN STREAM type device,
                        //and Initialize PciE link properties if so.
                        if( PcieIsDownStreamPort(dev) ){
                            Status=PcieInitLink(dev);
                            ASSERT_EFI_ERROR(Status);
						    if(EFI_ERROR(Status)) goto EEXIT;
                        } else {
                            //Add to the gPcieEpLst Orphan Upstream Ports of Switches and PCIe2PCI bridgers.
        					if((dev->PciExpress->PcieCap.PortType==PCIE_TYPE_UPS_PORT) ||
                               (dev->PciExpress->PcieCap.PortType==PCIE_TYPE_PCIE_PCI)){

                                Status=AppendItemLst(&gPcieEpList, dev);
                                PCI_TRACE((TRACE_PCI,"PciE: Adding Device [B%X|D%X|F%X] to gPcieEpList[%d]=%r\n\n",
                                dev->Address.Addr.Bus,dev->Address.Addr.Device, dev->Address.Addr.Function, gPcieEpList.ItemCount, Status));
                                ASSERT_EFI_ERROR(Status);
						        if (EFI_ERROR(Status)) goto EEXIT;
                            }
                        }
                    }
//#endif
//-------------------------------------------------------------------------
					//Finally we got here this bridge is done for now!!! we will use 
					//information collected, when we will programm the resources 
                    dev->Enumerated=TRUE;
				break;

				default	: 
					Status=EFI_UNSUPPORTED;
					ASSERT_EFI_ERROR(Status);
					return Status;
			} //switch;
			//Now update ParentBridge ChildList and Count
			Status=BridgeAddChild(ParentBrg,dev);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) goto EEXIT;
			//and so on
			if( !mf ) break;
		} //function loop
#if PCI_DEV_REVERSE_SCAN_ORDER
        pcidev.Addr.Device=DevBuff;
#endif
	} //device loop
    return Status1;
///////////////////////////////////////////
//Emergency Exit Label
EEXIT:
	if(dev){
		if(dev->PciExpress){
            if(dev->PciExpress->Pcie2) pBS->FreePool(dev->PciExpress->Pcie2);
            if(dev->PciExpress->VcData) {
                if(dev->PciExpress->VcData->VcCount)ClearItemLst((T_ITEM_LIST*)&dev->PciExpress->VcData->InitCnt, TRUE);
                pBS->FreePool(dev->PciExpress->VcData);
            }
            if(dev->PciExpress->SriovData) pBS->FreePool(dev->PciExpress->SriovData);
            if(dev->PciExpress->AriData) pBS->FreePool(dev->PciExpress->AriData);
            if(dev->PciExpress->AcsData) pBS->FreePool(dev->PciExpress->AcsData);
            if(dev->PciExpress->AtsData) pBS->FreePool(dev->PciExpress->AtsData);
            if(dev->PciExpress->RcLnkData) pBS->FreePool(dev->PciExpress->RcLnkData);
            if(Pcie3CheckPcie3Compatible(dev)) pBS->FreePool(dev->PciExpress->Pcie3);
            
            pBS->FreePool(dev->PciExpress);
        }

		if(dev->PciX) pBS->FreePool(dev->PciX);
		if(dev->HotPlug)pBS->FreePool(dev->HotPlug);
		pBS->FreePool(dev);
	}
	return Status;

}


/**
    Programm PCI BAR Register with the Resource Address 
    provided by the "bar" parameter.

        
    @param bar Pointer at PCI BAR register information

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
 
**/
EFI_STATUS EFIAPI ProgramBar(PCI_BAR	*bar)
{
	EFI_STATUS	Status=0;
	PCI_CFG_ADDR	addr;
	PCI_DEV_INFO	*owner=bar->Owner;
//------------------------------------
	addr.ADDR=bar->Owner->Address.ADDR;
	addr.Addr.Register=bar->Offset;
    addr.Addr.ExtendedRegister=bar->ExtOffset;

#if EFI_DEBUG || USB_DEBUG_TRANSPORT
	if(owner->DebugPort){
		gDbgPortInfo.BaseAddress=bar->Base;		
		Status=gDbgPortHob->SetDebugPortResources(gDbgPortHob, pBS, &gDbgPortInfo);
		ASSERT_EFI_ERROR(Status);
		owner->Assigned=TRUE;
		return Status;
	}	
#endif

	PCI_TRACE((TRACE_PCI,"PciBus: Device [B%X|D%X|F%X] Type=%X Bar.Offs=%Xh, Bar.Type=%d \n  Assigned Base=%lX, Size=%lX \n", 
	owner->Address.Addr.Bus, owner->Address.Addr.Device,
	    owner->Address.Addr.Function, owner->Type, 
        ((bar->ExtOffset==0) ? bar->Offset : bar->ExtOffset), 
        bar->Type, bar->Base, bar->Length));
    //EIP 26787 +
    //Forgot to exclude P2P Bridge ROM BAR, it belongs to primary interface!
	//if(owner->Type==tPci2PciBrg && (bar->Offset > 0x14 ){
    if(owner->Type==tPci2PciBrg && ( (bar->Offset > PCI_BAR1) && (bar->Offset < PCI_EROM) ) ){
    //EIP 26787 -

		//we got special P2P bridge BAR 
		switch (bar->Type){
			case tBarIo16 :
			case tBarIo32 :
				Status=EnableBridgeIoDecoding(owner, (UINT64)bar->Base, (UINT64)bar->Length);
				//if(EFI_ERROR(Status)) return Status;
				break;

			case tBarMmio32:
			case tBarMmio64:
				Status=EnableBridgeMmioDecoding(owner, (UINT64)bar->Base, (UINT64)bar->Length);
				//if(EFI_ERROR(Status)) return Status;
				break;
			
			case tBarMmio32pf:
			case tBarMmio64pf:
				Status=EnableBridgePfmmDecoding(owner, (UINT64)bar->Base, (UINT64)bar->Length);
				//if(EFI_ERROR(Status)) return Status;
				break;
			default: 
				break; 
		}//switch
	
	} else {
		if(owner->Type==tPci2CrdBrg && bar->Offset > PCI_BAR0 ){
			//here goes special Memory and IO windows of CardBus Bridge
			UINT32 	buffer=(UINT32)bar->Base;
		//--------------------
			Status=PciCfg32(owner->RbIo,addr,TRUE,(UINT32*)&buffer); //Base 
			if(EFI_ERROR(Status)) return Status;

			addr.Addr.Register+=4; //Limit
			buffer=(UINT32)(bar->Base+bar->Length-1);
			Status=PciCfg32(owner->RbIo,addr,TRUE,(UINT32*)&buffer); 
			if(EFI_ERROR(Status)) return Status;

			//Program Brg Control Reg to notify that Bridge Mem Window is PF
			if(bar->Type==tBarMmio32pf || bar->Type==tBarMmio64pf){

				addr.Addr.Register=PCI_BRIDGE_CONTROL_REGISTER_OFFSET;
				Status=PciCfg16(owner->RbIo,addr,FALSE,(UINT16*)&buffer);
				if(EFI_ERROR(Status)) return Status;
				
				if(bar->Offset==0x1C) buffer|=BIT08;	//Memory Window 1 is PF		
				if(bar->Offset==0x24) buffer|=BIT09;	//Memory Window 2 is PF		

				Status=PciCfg16(owner->RbIo,addr,TRUE,(UINT16*)&buffer);
				if(EFI_ERROR(Status)) return Status;
			}
		} else {
			//do regular device BAR programming
			UINT64	buffer=bar->Base;
		//--------------------
			if(bar->DiscoveredType == tBarMmio64 || bar->DiscoveredType == tBarMmio64pf)
				Status=PciCfg64(owner->RbIo,addr,TRUE,&buffer);
			else Status=PciCfg32(owner->RbIo,addr,TRUE,(UINT32*)&buffer);
		}
	}
	return Status;
}

/**
    Sorts, Aligns, and Programms Resources requested by devices 
    residing behind its Parent PCI Bridge - "Brg".

        
    @param Brg Pointer to PCI Device Private Data structure
        of the Parent Bridge.
    @param rt Type of PCI Bus resources to work with. 
        See definition of MRES_TYPE for detail.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
 
**/
EFI_STATUS EFIAPI AssignBridgeResources(PCI_BRG_INFO *Brg, MRES_TYPE rt)
{
	EFI_STATUS		Status=EFI_NOT_FOUND;
	UINT64			min=Brg->Bridge.Res[rt].Base;
	UINT64			max=(min+Brg->Bridge.Res[rt].Length);
	PCI_BAR			*bar=NULL;
	BRG_RES_ORDER	*ord=&Brg->Bridge.BarOrder[rt];
	//APTIOV_SERVER_OVERRIDE_START
	INTN			i;//,j;
	//APTIOV_SERVER_OVERRIDE_END
	BOOLEAN         dynord;
//---------------------------------------

    if(ord->BarCount == 0) return EFI_SUCCESS;

//APTIOV_SERVER_OVERRIDE_START
	//Assign Io resources to all of Bridge childs
#if PCI_MMIO_RES_TOP_ALLIGN == 1    
    for(i = ord->BarCount-1; i >= 0; i--){
#else        
	for(i = 0; i < (INTN)ord->BarCount; i++){
#endif	    
//APTIOV_SERVER_OVERRIDE_END

        if(min>=max) {
            Status=EFI_UNSUPPORTED;
            PCI_TRACE((TRACE_PCI,"PciBus: ERROR Assign Res: MIN_ADDR(0x%lX) >= MAX_ADDR(0x%lX)\n", min, max));
            break;
        }
		bar=ord->BarList[i];
		
        if(!gPciFixedRes || bar->Base==0) dynord=TRUE;
        else dynord=FALSE;

//(EIP45278)>
		if( dynord ) {
#if PCI_MMIO_RES_TOP_ALLIGN == 1
		    if(rt==rtIo16)
#endif
		    bar->Base=min;
#if PCI_MMIO_RES_TOP_ALLIGN == 1
		    else
		    bar->Base=max-bar->Length;
#endif
        }
//<(EIP45278)
		if(bar->Offset!=0xFF){
			Status=ProgramBar(bar);
			if(EFI_ERROR(Status)) break;
		}
//(EIP45278)>
		if( dynord ) {
#if PCI_MMIO_RES_TOP_ALLIGN == 1
		    if(rt==rtIo16)
#endif
		    min+=(bar->Length);
#if PCI_MMIO_RES_TOP_ALLIGN == 1
		    else
		    max-=(bar->Length);
#endif
        }
//<(EIP45278)
	}	

	if(EFI_ERROR(Status)&& bar!=NULL){
		PCI_TRACE((TRACE_PCI,"PciBus: ERROR Assign Res for BAR(T=%X O=%X) @ [ B%X|D%X|F%X ]",
					bar->Type, bar->Offset,	bar->Owner->Address.Addr.Bus,
					bar->Owner->Address.Addr.Device,bar->Owner->Address.Addr.Function));
        ASSERT_EFI_ERROR(Status);
    } 

	return Status;
}

/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ClearBar()
//
// Description: Clears/zeros contents of PCI_BAR structure.
//
// Input:
//  PCI_BAR         *Bar    Pointer to PCI_BAR structure to clear.
//
// Output:	Nothing
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ClearBar(PCI_BAR *Bar)
{
	Bar->Type=tBarUnused;
	Bar->Gran=0;
	Bar->Length=0;
	Bar->Base=0;
}
*/

/**
    Checks if PCI BAR register of a particular "BarType" present 
    behind the PCI Bridge. 

        
    @param Brg Pointer to PCI Device Private Data structure
        of the Parent Bridge.
    @param BarType Bar Type to search for. 

    @retval BOOLEAN
        TRUE    if BAR of "BarType" present Behind the "Brg".
        FALSE   if otherwice.
 
**/
BOOLEAN EFIAPI FindBarByType(PCI_BRG_INFO* Brg, PCI_BAR_TYPE BarType)
{
	UINTN			i,j;
	BOOLEAN 		res;	
	PCI_DEV_INFO	*dev;
//---------------------
	for(i=0; i<Brg->Bridge.ChildCount; i++){
		dev=Brg->Bridge.ChildList[i];
		if(dev->Type<tPciHostDev && dev->Type>=tPciRootBrg){
			//go inside the bridge...
			res=FindBarByType((PCI_BRG_INFO*)dev,BarType);
			if(res) return res;
		}
			
		for(j=0; j<PCI_MAX_BAR_NO+1; j++) if(dev->Bar[j].Type==BarType) return TRUE;
//#if SRIOV_SUPPORT
		//SRIOV Support adds some more bars ito the picture
		if( SriovCheckSriovCompatible(dev)){
			if(SriovCheckBarType(dev, BarType)) return TRUE;
		}
//#endif
	}
	return FALSE;
}

/**
    Converts Resources Requested by the Devices behind the 
    PCI Bridge of "ResType" using "ConvType" method of conversion 

        
    @param Brg Pointer to PCI Device Private Data structure
        of the Parent Bridge.
    @param ResType Resource Type to convert. Accepted values are
        tBarIo, tBarMem, tBarMemPf, 
        all other values are invalid.
    @param ConvType Type of Conversion to be performed.

    @param Convert request to convert.

    @retval Nothing
**/
VOID EFIAPI CheckFixedBridgeResConvertion(PCI_BRG_INFO *Brg, PCI_BAR_TYPE ResType, RES_CONV_TYPE *ConvType, BOOLEAN *Convert){
    UINTN i;
    AMI_SDL_PCI_DEV_INFO    *SdlInfo=Brg->Common.AmiSdlPciDevData;
    AMI_SDL_PCI_FIX_BAR     *fixedbar;
//------------------    
    
    if(gNoSdlDataMode) return;

    //This is the bridge device ....
    //so bar offsets must be 
    //for IO16 & 32         -  0x1c 
    //    MMIO32 Only       -  0x20
    //    PFMMIO 32 & 64    -  0x24
    PCI_TRACE((TRACE_PCI,"PciBus: CheckFixedBridgeResConvertion: "));

    //number of elements in SdlInfo->FixedResources[7] array == 7
    for(i=0; i<PCI_MAX_BAR_NO+1; i++){ 
        fixedbar=&SdlInfo->FixedResources[i];
        //got a valid BAR here...
        if(fixedbar->BarLength!=0){
            switch (ResType) {
                case tBarIo:
                    //IO Base and Limit reg
                    if(fixedbar->BarOffset==0x1c){
                        //check if fixed base address can accommodate narrow resources.
                        //If proposed base address lesser than
                        PCI_TRACE((TRACE_PCI,"fB=0x%lX; fL=0x%lX; ",fixedbar->BarBase,fixedbar->BarLength));
                        if(fixedbar->BarBase >= 0x10000){
                            PCI_TRACE((TRACE_PCI,"rcWideOnly;"));
                            *ConvType=rcWideOnly;
                            *Convert=TRUE;
                        } else {
                            PCI_TRACE((TRACE_PCI,"rcOneOf;"));
                            *ConvType=rcOneOf;
                            *Convert=TRUE;
                        }
                        //we are done here no need to continue
                        PCI_TRACE((TRACE_PCI," Convert=%d;\n", *Convert));
                        return;
                    }
                    break;

                case tBarMem:
                    if(fixedbar->BarOffset==0x20){
                        //this is only p2p bridge here
                        PCI_TRACE((TRACE_PCI,"fB=0x%lX; fL=0x%lX; ",fixedbar->BarBase,fixedbar->BarLength));
                        if(fixedbar->BarBase >= 0x100000000){
                            //This condition should not happened
                            //if ASSERT - means porting error.
                            PCI_TRACE((TRACE_PCI,"rcNone;"));
                            ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
                            *ConvType=rcNone;
                            *Convert=TRUE;
                        } else {
                            PCI_TRACE((TRACE_PCI,"rcNarrow;"));
                            *ConvType=rcNarrow;
                            *Convert=TRUE;
                        }
                        //we are done here no need to continue
                        PCI_TRACE((TRACE_PCI," Convert=%d;\n", *Convert));
                        return;
                        
                    }
                    break;           

                case tBarMemPf:
                    if(fixedbar->BarOffset==0x24){
                        //this is only p2p bridge here
                        PCI_TRACE((TRACE_PCI,"fB=0x%lX; fL=0x%lX; ",fixedbar->BarBase,fixedbar->BarLength));
                        if(fixedbar->BarBase >= 0x100000000){
                            PCI_TRACE((TRACE_PCI,"rcWideOnly;"));
                            *ConvType=rcWideOnly;
                            *Convert=TRUE;
                        } else {
                            PCI_TRACE((TRACE_PCI,"rcOneOf;"));
                            *ConvType=rcOneOf;
                            *Convert=TRUE;
                        }
                        PCI_TRACE((TRACE_PCI," Convert=%d;\n", *Convert));
                        //we are done here no need to continue
                        return;
                    }
                    break;           

                default : return; //no other combinations supported
            }//switch
        } //fixedbar->BarLength!=0
    } //for loop
    
    //if we here that means - resources requested for conversion not supported by this P2P Bridge.
    //in other words - Some of Bridge BARs at offset 0x1c; 0x20 or 0x24 in VeB UI left uninitialized 
    *Convert = TRUE;
    *ConvType=rcNone;
    PCI_TRACE((TRACE_PCI,"NO VALID BAR(s) found; rcNone; Convert=%d;\n", *Convert));
   
}


/**
    Converts Resources Requested by the Devices behind the 
    PCI Bridge of "ResType" using "ConvType" method of conversion 

        
    @param Brg Pointer to PCI Device Private Data structure
        of the Parent Bridge.
    @param ResType Resource Type to convert. Accepted values are
        tBarIo, tBarMem, tBarMemPf, 
        all other values are invalid.
    @param ConvType Type of Conversion to be performed.

    @retval Nothing
**/
VOID EFIAPI ConvertResources(PCI_BRG_INFO	*Brg, PCI_BAR_TYPE ResType, RES_CONV_TYPE ConvType, BOOLEAN CombineMemPmem)
{
	PCI_BRG_EXT	 	*ext=&Brg->Bridge;
	PCI_DEV_INFO 	*dev=&Brg->Common;
	UINTN			i,j;
	BOOLEAN			cnv=FALSE;
	PCI_BAR_TYPE	nt, wt;	
	RES_CONV_TYPE	p2pct;
	BOOLEAN         virt=FALSE;
//----------------------------
	//Virtual flag Initialized to FALSE if AmiSdlPciDevData record is not present in PCI Database.
	//Since only existing in Database record may have Virtual flag set to true.
	if(dev->AmiSdlPciDevData!=NULL) virt=dev->AmiSdlPciDevData->PciDevFlags.Bits.Virtual;

	PCI_TRACE((TRACE_PCI,"PciBus: ConvertResources for [B%X|D%X|F%X] Virt=%d:",
                dev->Address.Addr.Bus, dev->Address.Addr.Device,dev->Address.Addr.Function, virt));
    
	//Pick Brg Bars to convert
	switch (ResType) {
		case tBarIo:
		    PCI_TRACE((TRACE_PCI," tBarIo"));
			nt=tBarIo16;
			wt=tBarIo32;
			p2pct=rcOneOf;	
			if(Brg->Common.Type==tPci2CrdBrg)p2pct=rcBoth;
			break;

		case tBarMem:
            PCI_TRACE((TRACE_PCI," tBarMem"));
			nt=tBarMmio32;
			wt=tBarMmio64;
			p2pct=rcNarrow;
			if(Brg->Common.Type==tPci2CrdBrg)p2pct=rcNarrow;
			break;			 

		case tBarMemPf:
            PCI_TRACE((TRACE_PCI," tBarMemPf"));
			nt=tBarMmio32pf;
			wt=tBarMmio64pf;
			p2pct=rcOneOf;
			if(Brg->Common.Type==tPci2CrdBrg)p2pct=rcNarrow;
			break;			 

		default : return; //no other combinations supported
	}//switch

	switch (ConvType){
		case rcNone:
            PCI_TRACE((TRACE_PCI,"; rcNone"));
			cnv=TRUE;
			break;
		case rcOneOf:
            PCI_TRACE((TRACE_PCI,"; rcOneOf"));
			cnv=FindBarByType(Brg,nt);
			break;
		case rcNarrow: 
            PCI_TRACE((TRACE_PCI,"; rcNarrow"));
			cnv=FindBarByType(Brg,wt);
			break;
		case rcBoth: 
		    cnv=FALSE;
		    PCI_TRACE((TRACE_PCI,"; rcBoth"));
		    break;
	}//switch
	PCI_TRACE((TRACE_PCI,"; Convert=%d;\n", cnv));
	
	//Take care about Padding behind the bridge if CombineMemPmem Attribute is set
	if(ext->Pad[rtMmio32p].Length && CombineMemPmem){
		 ext->Pad[rtMmio32].Length+=ext->Pad[rtMmio32p].Length;
		 ext->Pad[rtMmio32p].Length=0;
	}
	if(ext->Pad[rtMmio64p].Length && CombineMemPmem){
		 ext->Pad[rtMmio64].Length+=ext->Pad[rtMmio64p].Length;
		 ext->Pad[rtMmio64p].Length=0;
	}

	//At this point we determined if conversion needed as if it was standard resources
	if(Brg->Common.AmiSdlPciDevData!=NULL && Brg->Common.FixedRes){
	    CheckFixedBridgeResConvertion(Brg, ResType,&ConvType, &cnv);
	}

	for(i=0; i<ext->ChildCount; i++) {
		dev=ext->ChildList[i];			
		for(j=0; j<PCI_MAX_BAR_NO; j++){
		
		    PCI_TRACE((TRACE_PCI,"  "));
	        if(CombineMemPmem){
	            if( dev->Bar[j].Type==tBarMmio32pf ) {
	                dev->Bar[j].Type=tBarMmio32;
	                PCI_TRACE((TRACE_PCI,"PFMMIO32 -> MMIO32; "));
                }
                if( dev->Bar[j].Type==tBarMmio64pf ) {
                    dev->Bar[j].Type=tBarMmio64;
                    PCI_TRACE((TRACE_PCI,"PFMMIO64 -> MMIO64; "));
                }
            }

			if(cnv){
		        switch(ConvType){
					case rcNone:
						if(dev->Bar[j].Type==wt || dev->Bar[j].Type==nt) {
						    ClearBar(&dev->Bar[j]);
	                        PCI_TRACE((TRACE_PCI,"rcNone ClearBar[%d]", j));
						}
						break;
					case rcOneOf:
					case rcNarrow: 
						if(dev->Bar[j].Type==wt){
						    dev->Bar[j].Type=nt;
                            PCI_TRACE((TRACE_PCI,"Convert to Narrow[%d]", j));
						}
						break;
                    case rcWideOnly: 
                        if(dev->Bar[j].Type==nt){
                            ClearBar(&dev->Bar[j]);
                            PCI_TRACE((TRACE_PCI,"rcWideOnly ClearBar[%d]", j));
                        }
                        break;
					default:
						break;
				}
                PCI_TRACE((TRACE_PCI,"\n"));
			} //if cnv 
		}//for j

//#if SRIOV_SUPPORT
		if(SriovCheckSriovCompatible(dev)){
			SriovConvertResources(dev, nt, wt, ConvType,cnv, CombineMemPmem);
		}
//#endif

		//recoursively call to convert child resources behind other brg
		if(dev->Type==tPci2PciBrg || dev->Type==tPci2CrdBrg){
			if(cnv){
				//Unconditionally convert all resources to narrow because parent requested resource conversion
				//if(ConvType)ConvertResources((PCI_BRG_INFO*)dev, ResType, rcNarrow, CombineMemPmem);
				//else ConvertResources((PCI_BRG_INFO*)dev, ResType, ConvType, CombineMemPmem);
			    ConvertResources((PCI_BRG_INFO*)dev, ResType, ConvType, CombineMemPmem);
			} else  ConvertResources((PCI_BRG_INFO*)dev, ResType, p2pct, CombineMemPmem);
		}
	} //Child loop
	//we did it!!!
}


/**
    Sorts, Aligns, and Programms Resources requested by devices 
    on the PCI Bus created by the PCI Bridge - "Brg".

        
    @param Brg Pointer to PCI Device Private Data structure
        of the Bridge who creates the bus.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
 
    @note  
  Assigns Resources by filling Device Bar structure
  and programm all Bridge Childs Bars
  and Enables Bridge devices to decode their resources
**/
EFI_STATUS EFIAPI AssignBusResources(PCI_BRG_INFO *Brg)
{
	EFI_STATUS		Status=0;
	UINT8			i;
	PCI_DEV_INFO	*dev;
#if SMCPKG_SUPPORT
	UINT8   j, BusS, BusE;
	UINT32  TempData;
	PCI_CFG_ADDR    TempDev;
#endif
//----------------------------------
	//First Check if Resources Assigned don't do it multiple times
	if(Brg->Common.Assigned) return EFI_SUCCESS;
	
	if(Brg->Common.Type == tPci2PciBrg || Brg->Common.Type == tPci2CrdBrg){
		Status=DisableDeviceDecoding(&Brg->Common,stDisableAll);
		Brg->Common.Attrib=0;				
	}	

	//Start with IO 
	if (Brg->Bridge.Res[rtIo32].Length) Status=AssignBridgeResources(Brg,rtIo32);
	if(EFI_ERROR(Status)) return Status;

	if (Brg->Bridge.Res[rtIo16].Length) Status=AssignBridgeResources(Brg,rtIo16);
	if(EFI_ERROR(Status)) return Status;

	
	//Now MMIO 64 PF
	if(Brg->Bridge.Res[rtMmio64p].Length)Status=AssignBridgeResources(Brg,rtMmio64p);
	if(EFI_ERROR(Status)) return Status;

	//Now MMIO 64
	if(Brg->Bridge.Res[rtMmio64].Length)Status=AssignBridgeResources(Brg, rtMmio64);
	if(EFI_ERROR(Status)) return Status;

	//Now MMIO 32 PF
	if(Brg->Bridge.Res[rtMmio32p].Length)Status=AssignBridgeResources(Brg,rtMmio32p);
	if(EFI_ERROR(Status)) return Status;

	//And MMIO 32	
	if(Brg->Bridge.Res[rtMmio32].Length)Status=AssignBridgeResources(Brg, rtMmio32);
	if(EFI_ERROR(Status)) return Status;

	
	//So far so good - resources has been assigned on this Bridge level
	//now check if we have any p2p brg among this brg childs... 
	//if so we will call this function recoursively...
	for(i=0; i<Brg->Bridge.ChildCount; i++ ){
        PCI_CFG_ADDR	a;
        PCI_CMD_REG     cmdreg;
    //-----------------
		Status=EFI_SUCCESS;
   
		dev=Brg->Bridge.ChildList[i];
		a.ADDR=dev->Address.ADDR;

		//Now take care of CacheLine Size register 
		a.Addr.Register=PCI_CLS; // Cache Line Size
			
		Status=PciCfg8(dev->RbIo,a,TRUE,&gPciCaheLineSize);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;


		if(dev->Type==tPci2PciBrg ||dev->Type==tPci2CrdBrg) Status=AssignBusResources((PCI_BRG_INFO*)dev);

		//disable decoding fo the PCI  devices unles PciPortSkipThisDevice() 
		//says to Skip it. The Device Specific driver should enable it using Attributes() function.
		if(!dev->SkipDevice){
            Status=DisableDeviceDecoding(dev,stDisableAll);
    		dev->Attrib&=((EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE | EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM));
        }    

		if (EFI_ERROR(Status)) return Status;
		dev->Assigned=TRUE;
	    //UEFI 2.1 - we must leave BRIDGES open (decoding it's ranges)
		if(dev->Type==tPci2PciBrg ||dev->Type==tPci2CrdBrg){
            PCI_BRG_CNT_REG bc;
    		//---------------------------		
            Status=DeviceAttributes(dev, dev->Capab & (EFI_PCI_DEVICE_ENABLE), TRUE);
            ASSERT_EFI_ERROR(Status);
            
            a.Addr.Register=PCI_BRIDGE_CNTL; //Bridge Control Reg 

		    Status=PciCfg16(dev->RbIo,a,FALSE,&bc.BRG_CNT_REG);
		    ASSERT_EFI_ERROR(Status);
   			if(EFI_ERROR(Status)) return Status;

            //Set VGA16 enable to avoid forwarding ISA VGA Aliases
            bc.Vga16Enable=1;             

            //Programm device's BRG_CTL_REG to forward #SERR and #PERR to the primary Interface.
            if(dev->DevSetup.SerrEnable != PCI_SETUP_DONT_TOUCH){
                if(dev->DevSetup.SerrEnable) bc.SerrEnable=1;
                else bc.SerrEnable=0;
            }
            
            if(dev->DevSetup.PerrEnable != PCI_SETUP_DONT_TOUCH){
                if(dev->DevSetup.PerrEnable == 1) bc.PerrEnable=1;
                else bc.PerrEnable=0;
            }
#if SMCPKG_SUPPORT
            for(j = 0; j < sizeof(BDF_Tbl)/sizeof(BDF_Tbl[0]); j++){
                if(!BDF_Tbl[j]) continue;

                if((dev->Address.Addr.Bus == (UINT8)(BDF_Tbl[j] >> 8)) &&
                        (dev->Address.Addr.Device == (UINT8)((BDF_Tbl[j] >> 3) & 0x1f))){
                    bc.SerrEnable=0;
                    bc.PerrEnable=0;
                    break;
                }
                TempDev.ADDR = 0;
                TempDev.Addr.Bus = (UINT8)(BDF_Tbl[j] >> 8);
                TempDev.Addr.Device = (UINT8)((BDF_Tbl[j] >> 3) & 0x1f);
                TempDev.Addr.Register = 0x18;
                PciCfg32(dev->RbIo, TempDev, 0, &TempData);
                if(TempData == 0xffffffff)      continue;
                BusS = (UINT8)(TempData >> 8);
                BusE = (UINT8)(TempData >> 16);
                if((dev->Address.Addr.Bus >= BusS) && (dev->Address.Addr.Bus <= BusE)){
                    bc.SerrEnable=0;
                    bc.PerrEnable=0;
                    break;
                }
            }
#endif
		    Status=PciCfg16(dev->RbIo,a,TRUE,&bc.BRG_CNT_REG);
		    ASSERT_EFI_ERROR(Status);
   			if(EFI_ERROR(Status)) return Status;
        }
    
        //Programm device's PCI_CMD_REG to generate #SERR and #PERR according to Setup.
        a.Addr.Register=PCI_CMD; //PCI Command Reg
        
	    Status=PciCfg16(dev->RbIo,a,FALSE,&cmdreg.CMD_REG);
	    ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;

		if(dev->DevSetup.SerrEnable != PCI_SETUP_DONT_TOUCH){
		    if(dev->DevSetup.SerrEnable) cmdreg.SerrEnable=1;
		    else cmdreg.SerrEnable=0;
		}
            

		if(dev->DevSetup.PerrEnable != PCI_SETUP_DONT_TOUCH){
		    if(dev->DevSetup.PerrEnable) cmdreg.ParityErrorResp=1;
            else cmdreg.ParityErrorResp=0;
		}
#if SMCPKG_SUPPORT
	    if((dev->Address.Addr.Bus >= 0) && (dev->Address.Addr.Bus <= 0xFF)){
	        cmdreg.SerrEnable=0;
	        cmdreg.ParityErrorResp=0;
	    }
	    for(j = 0; j < sizeof(BDF_Tbl)/sizeof(BDF_Tbl[0]); j++){
	        if(!BDF_Tbl[j]) continue;

	        TempDev.ADDR = 0;
	        TempDev.Addr.Bus = (UINT8)(BDF_Tbl[j] >> 8);
	        TempDev.Addr.Device = (UINT8)((BDF_Tbl[j] >> 3) & 0x1f);
	        TempDev.Addr.Register = 0x18;
	        PciCfg32(dev->RbIo, TempDev, 0, &TempData);
	        if(TempData == 0xffffffff)      continue;
	        BusS = (UINT8)(TempData >> 8);
	        BusE = (UINT8)(TempData >> 16);
	        if((dev->Address.Addr.Bus >= BusS) && (dev->Address.Addr.Bus <= BusE)){
	            cmdreg.SerrEnable=0;
	            cmdreg.ParityErrorResp=0;
	            break;
	        }
	    }
#endif
	    Status=PciCfg16(dev->RbIo,a,TRUE,&cmdreg.CMD_REG);
	    ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;

	} //for 
	Brg->Common.Assigned=TRUE;

	return Status;
}


/**
    Creates an ACPI QWORD Resource Descriptors set to reflect 
    this "RootBrg" resource request

        
    @param RootBrg Pointer to PCI Device Private Data structure
        of the Bridge who creates the bus.
    @param ResDsc ACPI QWORD Resource Descriptors set.
        See definition of ASLR_QWORD_ASD for details.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
 
    @note  
  Meaning of QWORD Resource Descriptor Fields for this function
  _LEN => Set to the size of the aperture that is requested.
  _GRA => Used to differentiate between a 32-bit memory request and a 
          64-bit memory request. For a 32-bit memory request, 
          this field should be set to 32. For a 64-bit memory request, 
          this field should be set to 64. All other values result in 
          this function returning the error code of EFI_INVALID_PARAMETER.
  _MAX => Used to specify the alignment requirement. 
  All other fields are ignored.
**/
EFI_STATUS EFIAPI CreateRootResDsc(PCI_DEV_INFO *RootBrg, VOID **ResDsc){
	UINTN				i, rc=0;
	ASLR_QWORD_ASD		*rd;
	PCI_BRG_EXT			*ext=(PCI_BRG_EXT*)(RootBrg+1);
//-----------------------------------------------------
	//First count how many rd suppose to be 
	for(i=rtIo16; i<rtMaxRes; i++){
		if(ext->Res[i].Type && ext->Res[i].Length) rc++;
	}

    //If RB don't have any resource requests fill out END_TAG_DESCRIPTOR and return.
    if(!rc){
        rd=MallocZ(sizeof(ASLR_EndTag));
        rd->Hdr.HDR=ASLV_END_TAG_HDR;        
    	*ResDsc=rd;
        return EFI_SUCCESS;
    }        

	//get some memory 
	rd=MallocZ(sizeof(ASLR_QWORD_ASD)*rc+sizeof(ASLR_EndTag));
	ASSERT(rd);
	if(!rd)	return EFI_OUT_OF_RESOURCES;
	
	*ResDsc=rd;
	for(i=rtIo16; i<rtMaxRes; i++){
		if(!(ext->Res[i].Type && ext->Res[i].Length)) continue ;
		switch(ext->Res[i].Type){
			case tBarIo16:
				rd->Type=ASLRV_SPC_TYPE_IO;
				rd->_GRA=16;
			break;
			case tBarIo32:
				rd->Type=ASLRV_SPC_TYPE_IO;
				rd->_GRA=32;
			break;
			case tBarMmio32pf:
				rd->TFlags.MEM_FLAGS._MEM=ASLRV_MEM_CEPF;
				//no break	intentionally!! DON'T CHANGE 
			case tBarMmio32:
				rd->Type=ASLRV_SPC_TYPE_MEM;
				rd->_GRA=32;
				rd->TFlags.MEM_FLAGS._RW=1;
			break;
			case tBarMmio64pf:
				rd->TFlags.MEM_FLAGS._MEM=ASLRV_MEM_CEPF;
				//no break	intentionally!! DON'T CHANGE 
			case tBarMmio64:
				rd->Type=ASLRV_SPC_TYPE_MEM;
				rd->_GRA=64;
				rd->TFlags.MEM_FLAGS._RW=1;
			break;
			default : continue;
		}
		rd->Hdr.HDR=0x8A;
		rd->GFlags.GFLAGS=0x0C; //Means _MIN and _MAX is fixed
		rd->Hdr.Length=0x2B; 	
		rd->_MAX=ext->Res[i].Gran;
		rd->_LEN=ext->Res[i].Length + ext->Align[i].ExtraRes.Length;

		if(rd->Type==ASLRV_SPC_TYPE_MEM){
			//Check to Memory Resource Size Align on 4K 
			if(rd->_LEN & (EFI_PAGE_SIZE-1))rd->_LEN=(rd->_LEN | (EFI_PAGE_SIZE-1))+1;
		}
		
       PCI_TRACE((TRACE_PCI,"CreateRD QWD: T=%X; GF=%X; TF=%X; _MN=%lX; _MX=%lX; _LN=%lX;_GR=%lX;\n",
					rd->Type, rd->GFlags.GFLAGS, rd->TFlags.TFLAGS, rd->_MIN, rd->_MAX, rd->_LEN, rd->_GRA));

		rd++;
	}
	rd->Hdr.HDR=ASLV_END_TAG_HDR;
	return EFI_SUCCESS;
}

/**
    Converts Resource information from  QWORD ACPI ResDesc
    to internal format and store it within PCI_DEV_INFO structure.  

        
    @param RbExt Pointer to PCI Device Private Data structure
        of the Root Bridge. (PCI_BRG_EXT part)
    @param Resources Pointer to the set 

    @param ConvType Type of Conversion to be prformed.

    @retval Nothing
**/
VOID EFIAPI ApplyAcpiResources(PCI_BRG_EXT	*RbExt, ASLR_QWORD_ASD *Resources){
	ASLR_QWORD_ASD 	*res=Resources;
	PCI_BAR			*bar;
	UINTN			i, s, e;
//------------------------------------	
	while(res->Hdr.HDR!=ASLV_END_TAG_HDR) {
		bar=NULL;
       	PCI_TRACE((TRACE_PCI,"ApplyRD QWD: T=%X; GF=%X; TF=%X; _MN=%lX; _MX=%lX; _LN=%lX;_GR=%lX;\n",
					res->Type, res->GFlags.GFLAGS, res->TFlags.TFLAGS, res->_MIN, res->_MAX, res->_LEN, res->_GRA));

		if(res->Type==ASLRV_SPC_TYPE_IO){
			s=rtIo16;
			e=rtMmio32;
		} else {
			if(res->Type==ASLRV_SPC_TYPE_MEM){
				s=rtMmio32;
				e=rtMaxRes;
			} else {  
				res++;
				continue;	
			}
		}

		for(i=s; i<e; i++){
			UINT64	len;
		//--------------------
			bar=&RbExt->Res[i];
			len=bar->Length+RbExt->Align[i].ExtraRes.Length;
			
			if( (res->Type==ASLRV_SPC_TYPE_MEM) && (len&(EFI_PAGE_SIZE-1)) ) 
					len=(len|(EFI_PAGE_SIZE-1))+1;

			//It might be an empty descriptor (i.e. System has MMIO32, but does not have MMIO32PF)
			//This might cause to break early. w/o searching through 64bit resources
			//take care of this condition by adding check (bar->Length != 0)
			if((bar->Base == 0) && (bar->Length != 0) && (len <= res->_LEN)) break;
			else bar=NULL;
		}
		
		if(bar)bar->Base=res->_MIN;
		res++;
	}
}

/**
    Updates global Platform Policy Data variable if PCI_PLATFORM or
    PCI_PLATFORM_OVERRIDE protocol,instance present.

        
    @param Controller Controller Handle passed to the PCI Bus Driver
        Start Function.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
**/
VOID EFIAPI GetPlatformPolicyData(PCI_HOST_INFO *HostData){
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    EFI_STATUS  Status = EFI_UNSUPPORTED;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
//-----------------
    
    //function returns only SUCCESS or UNSUPPORTED. 
    if(HostData->PlatformProt!=NULL){
        Status=HostData->PlatformProt->GetPlatformPolicy(
                                HostData->PlatformProt,
                                &gPlatformPolicy);
    }   
        
    //Platform protocol has priority over Override protocol
    //Override protocol must called if Platform - not present 
    //or returned an ERROR!
    if( EFI_ERROR(Status) && (HostData->PciOverrideProt!=NULL) ){
        Status=HostData->PciOverrideProt->GetPlatformPolicy(
                                HostData->PciOverrideProt,
                                &gPlatformPolicy);
    
    }
}

/**
    Creates all anckor structures and Initialize PCI Database.

        
    @param Controller Controller Handle passed to the PCI Bus Driver
        Start Function.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
**/
EFI_STATUS EFIAPI InitPciDb(EFI_HANDLE Controller){
	EFI_STATUS							Status;
	EFI_HANDLE							*hBuff=NULL, htmp=NULL;
	EFI_DEVICE_PATH_PROTOCOL			*rbdp;
	UINTN								i, cnt=0, rbcnt,j;
	PCI_DEV_INFO						*rb;
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL		*rbio;
	AMI_SDL_PCI_DEV_INFO 				**hostSdlLst=NULL;
	PCI_HOST_INFO						*lhst;
//------------------------
	//Print PCI Bus Driver Version
    PCI_TRACE((TRACE_PCI,"PciBus: Initializing PCI DataBase... PCI Bus Driver Version: %X.%d.%d\n", PCI_BUS_MAJOR_VER, PCI_BUS_MINOR_VER, PCI_BUS_REVISION));

	Status=pBS->HandleProtocol(Controller,&gEfiPciRootBridgeIoProtocolGuid,(VOID**)&rbio); 
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return EFI_INVALID_PARAMETER;
	
    if(gPciHostArray!=NULL && gPciHostArray[0].Updated)	return EFI_SUCCESS;


	//Try to figure out how many instances of PCI HostBridge Resource Allocation Protocol
	//exists, this will give us an idea how many PCI_ROOT_INFO instances to create...
	Status=pBS->LocateHandleBuffer(
							ByProtocol,						//	SearchType,
							&gEfiPciHostBridgeResourceAllocationProtocolGuid,	//*Protocol OPTIONAL,
							NULL,							//*SearchKey OPTIONAL,
							&gHostCnt,							//*NoHandles,	
							&hBuff);						//**Buffer

    PCI_TRACE((TRACE_PCI,"PciBusInit: ResAllocProt HandleCnt=%d; %r;",gHostCnt, Status));
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

    //Double checkinfo based on ResAllocProt with SDL info stored...
    if(gNoSdlDataMode==FALSE){
        Status=AmiSdlPciGetHostBridges(&hostSdlLst, &cnt);
        PCI_TRACE((TRACE_PCI,"SDL HostCnt=%d; %r;\n",cnt,Status));
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;
    } else {
        PCI_TRACE((TRACE_PCI," NoSdlDataMode==TRUE; %r;\n",Status));
    }
    
	//Now we know how many Host Bridges the System has
	gPciHostArray=MallocZ(sizeof(PCI_HOST_INFO)*gHostCnt);
	gPciHostList=MallocZ(sizeof(PCI_HOST_INFO*)*gHostCnt);
	
	//record Host Bridge Properties
	for(i=0; i<gHostCnt; i++){
		//Handle of resource allocation protocol should corresponds to HB Handle.
	    gPciHostArray[i].HostHandle=hBuff[i];

		//Get Resource Alloc Protocol and save it... It must be only one instance per HostBridge Handle.
		Status=pBS->HandleProtocol(hBuff[i],&gEfiPciHostBridgeResourceAllocationProtocolGuid,(VOID**)&gPciHostArray[i].ResAllocProt); 
		//it has to be there or platform is not Framework complaint
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		//now will locate some optional protocols which must be used if present during PCI enumeration
		//as per spec - only one instance of this protocols should be present
		if(i==0){
			pBS->LocateProtocol(&gEfiPciPlatformProtocolGuid, NULL, (VOID**)&gPciHostArray[i].PlatformProt); 
			pBS->LocateProtocol(&gEfiPciOverrideProtocolGuid, NULL, (VOID**)&gPciHostArray[i].PciOverrideProt); 
			pBS->LocateProtocol(&gEfiIncompatiblePciDeviceSupportProtocolGuid, NULL, (VOID**)&gPciHostArray[i].IncompDevProt); 
		} else {
		    gPciHostArray[i].PlatformProt=gPciHostArray[0].PlatformProt;
		    gPciHostArray[i].PciOverrideProt=gPciHostArray[0].PciOverrideProt;
		    gPciHostArray[i].IncompDevProt=gPciHostArray[0].IncompDevProt;
		}
		gPciHostArray[i].Updated=TRUE;
	}

	//Now Get platform Policy FLAG...
	GetPlatformPolicyData(&gPciHostArray[0]);
	
	//Now init BAD_PCI/Incompatible Device DATABASE... To support PI 1.2 
	//EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT_PROTOCOL along with AMI's own 
	//BAD PCI DEVICE LIST we need have ability to add to our local DATABASE 
	//new records from Installed EFI_INCOMPATIBLE_PCI_DEVICE_SUPPORT_PROTOCOL 
	for(i=0; i<gAmiBadPciDevCount; i++){
		PCI_BAD_BAR	*badbar;
	//----------------------------	
		badbar=&gAmiBadPciDevList[i];
		Status=AppendItemLst(&gBadPciDevList,(VOID*)badbar);
		ASSERT_EFI_ERROR(Status);
	}
	
	
	//Now get all RbIo Protocol Handlers and find the right place for it Protocol Instance.
	Status=pBS->HandleProtocol(Controller,&gEfiPciRootBridgeIoProtocolGuid,(VOID**)&rbio); 
	//RBIoProtocol has to be installed on Controller Handle
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	//Select Right Host Brg for this RootBrg Io Protocol Handler
	for(i=0; i<gHostCnt; i++){
	    j=0; //reset Root Bridge index since scanning new Host.
		htmp=NULL;
		rbcnt=0;
		//Now we will fill Pci Root infrastructure in the right order 
		while(!Status){
			//get Root Bridge Handle of this Host
			Status=gPciHostArray[i].ResAllocProt->GetNextRootBridge(gPciHostArray[i].ResAllocProt, &htmp);
			//EFI_NOT_FOUND is anticipated ERROR Status value - that means we are done...
			if(EFI_ERROR(Status) && Status==EFI_NOT_FOUND) {
                //restore good status since while loop will break w/o even trying to scan next host
                Status=EFI_SUCCESS;
                break;
            }
			//All other ERROR Statuses are bad and must ASSERT.
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

			//Create New PCI_BRG_INFO structure;
			rb=NewDevice(TRUE,NULL);
			if(!rb) return EFI_OUT_OF_RESOURCES;
				
			//Add it to the Root Bridge Item List
			Status=AppendItemLst((T_ITEM_LIST*)&gPciHostArray[i].RbInitCnt, rb);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
		
			//now I'll get DevicePath and RootBridgeIo protocols Associated with this Handle   
			//Status=pBS->HandleProtocol(htmp,&gEfiPciRootBridgeIoProtocolGuid,&rb->RbIo);
			//This Protocol will remain opened untill PciBusStop is Called. 
			Status=pBS->OpenProtocol(htmp,
				&gEfiPciRootBridgeIoProtocolGuid,(VOID **)&rb->RbIo, 
				gPciBusDriverBinding.DriverBindingHandle,htmp,
				EFI_OPEN_PROTOCOL_BY_DRIVER );

			ASSERT_EFI_ERROR(Status); //it MUST have this
			if(EFI_ERROR(Status)) return Status;

			Status=pBS->HandleProtocol(htmp,&gEfiDevicePathProtocolGuid,(VOID**)&rbdp); 
			ASSERT_EFI_ERROR(Status); //it MUST have this
			if(EFI_ERROR(Status)) return Status;

			//Fill out some missing and incorrectly filled fields
			rb->DevicePath=DPCopy(rbdp);
			rb->Type=tPciRootBrg;
			rb->HostData=&gPciHostArray[i];
			rb->Handle=htmp;
			rb->ParentBrg=NULL;
//---------------------------------------------------------------------------------------
			//now map RbIo opened to the corresponded host using HostHandle.
			for(cnt=0; cnt<gHostCnt; cnt++){
				lhst=&gPciHostArray[cnt];
				if(lhst->HostHandle==rb->RbIo->ParentHandle) break;
			}

		    //Init Hot Plug
			Status=HpLocateProtocolSortRhpc(&gPciHostArray[i],rb);
			
			j++;
		} //while
        PCI_TRACE((TRACE_PCI,"PciBusInit: HB [%d] Has %d RootBridge(s);\n",i, j));

	} //for gHostCnt..

	//We have collected all Hb->Rb data and have arrange Root bridges inside each Host.
	
	
	//Check If we have a FIXED_RESOURCES in sdl data...
    if(gNoSdlDataMode==FALSE){
        Status=AmiPciCheckFixedResources();
        if(!EFI_ERROR(Status)) gPciFixedRes=TRUE;
    } else {
        gPciFixedRes=FALSE;
    }
    
	//Fixed Resources and Hotplug support are mutually exclusive features.
	//if system has Hotplug Settings in SDL - disable Fixed Resources feature
    if (gPciHostArray[0].HpInitProt!=NULL){
	    gPciHotPlug=TRUE;
	 	if(gPciFixedRes) {
            PCI_TRACE((TRACE_PCI,"\n  !!!Fixed Resources and Hot-Plug are NOT SUPPORTED TOGETHER !!!\n  !!!Fixed Resources Support will be Disabled in this project!!!\n"));
            ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
            gPciFixedRes=FALSE;
        }
	}	
	//Don't forget to free handle Buffer;
	if(hBuff!=NULL)pBS->FreePool(hBuff);
	if(hostSdlLst!=NULL) pBS->FreePool(hostSdlLst);

	return EFI_SUCCESS;
}

/**
    This routine will update try to find RB by type casting RbIoProtocol
    to internal PCI_ROOT_BRG_DATA Data Structure.

 
    
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI AmiSdlFindRbBySignature(IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIoProtocol, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index)
{
    PCI_ROOT_BRG_DATA* rbdata=(PCI_ROOT_BRG_DATA*)RbIoProtocol;
//------------------------
    if(rbdata->Signature==AMI_RB_SIG){
        *Record=rbdata->RbSdlData;
        *Index=rbdata->RbSdlIndex;
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;   
}


/**
    This is Main function to start PCI Bus Enumeration Process.

        
    @param Controller Controller Handle passed to the PCI Bus Driver
        Start Function.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
**/
EFI_STATUS EFIAPI PciEnumerateAll(EFI_HANDLE Controller){
	EFI_STATUS			Status=EFI_SUCCESS;
	UINTN				i, j;
	UINT8				StartBus, EndBus;
	RES_CONV_TYPE		ct;
	PCI_HOST_INFO		*lhst;
	PCI_DEV_INFO		*rb;
	PCI_BRG_EXT			*ext;
	ASLR_QWORD_ASD		*rres;
#if SMCPKG_SUPPORT
        UINT64  NVMe_Port_Flag, NVMe_Extend_Flag;
        UINT16  Bus_Tbl[8] = {0x0011, 0x0012, 0x0013, 0, 0x0021, 0x0022, 0x0023, 0};
        UINT8   IIO_Flag, IIO_Extend_Flag, Index, BDF_Tbl_Index = 0, TempBus, TempDev;
#endif
//-------------------------------
	//Init Pci Local Data if it wasn't initialized yet
    if(gPciHostArray==NULL){
    	Status=InitPciDb(Controller);
	    ASSERT_EFI_ERROR(Status);	
    }
    //Check if we have enumerated VERY FIRST ROOT we done.
    //That means all roots were enumerated alredy, don't spend eny more time here... 
	if(gPciHostArray[0].Enumerated) return EFI_SUCCESS; 

	//We will do enumeration for all PCI infrastructure, as soon as we got very first 
	//Root Bridge Handle. and then will start only devices belonging to the RootBridge   
	//who's handle we have received as a Controller Handle.
	mMaxBusFound=0;		
    mMaxBusReport=0;

	for(j=0; j<gHostCnt; j++){
	
        PCI_TRACE((TRACE_PCI,"\n====================================================================\n"));
	
		lhst=&gPciHostArray[j]; //init local host pointer
		//will circle trough Root bridges system has starting from _UID=0
		for(i=0; i<lhst->RbCount; i++){
			rb=lhst->RootBridges[i];			
			ext=(PCI_BRG_EXT*)(rb+1);

#if BoardPciRes_SUPPORT
            gPciOutOfRes=FALSE;
#endif
			//check if we have this Root Bridge Enumerated already
			if(rb->Enumerated) continue;		

			//6. Notify the host bridge driver that PCI enumeration is about to begin by calling
			//NotifyPhase (EfiPciHostBridgeBeginEnumeration). This member function
			//must be the first one that gets called. PCI enumeration has two steps: bus enumeration and
			//resource enumeration.
			Status=DoPciNotify(lhst, EfiPciHostBridgeBeginEnumeration);
			ASSERT_EFI_ERROR(Status);
		
			//7. Notify the host bridge driver that bus enumeration is about to begin by calling
			//NotifyPhase (EfiPciHostBridgeBeginBusAllocation).
			Status=DoPciNotify(lhst, EfiPciHostBridgeBeginBusAllocation);
			ASSERT_EFI_ERROR(Status);
				
			//8. Do the following for every PCI root bridge handle:
			//	a. Call StartBusEnumeration (This,RootBridgeHandle).
			Status=lhst->ResAllocProt->StartBusEnumeration(lhst->ResAllocProt, rb->Handle, (VOID**)&rres); 
			ASSERT_EFI_ERROR(Status);
			
            //get allocation attributes.
            Status=lhst->ResAllocProt->GetAllocAttributes(lhst->ResAllocProt,rb->Handle,&gAllocationAttributes);
            ASSERT_EFI_ERROR(Status);

            //some RB RC set this attribute w/o intent to down grade 64bit resource request.
            if(PciRbCombineMemPmemReset) gAllocationAttributes &= (~EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM);
            
			//Check if it is NULL descriptor..
			if(rres->_MIN==0 && rres->_MAX==0 && rres->_LEN==0) {
				Status=DoPciNotify(lhst, EfiPciHostBridgeEndBusAllocation);
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status))

    			Status=DoPciNotify(lhst, EfiPciHostBridgeBeginResourceAllocation);
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status))

				continue;
			}

			//Check if we've received correct descriptors
			if(!ValidateDescriptorBlock(rres,tResBus,FALSE)) {
				ASSERT(0);
				return EFI_DEVICE_ERROR;
			}

			//we have received BUS configuration information in "rres"
			//_MIN Bus# has to be Updated with respect of previous pass 
			StartBus=(UINT8)rres->_MIN;
        	mMaxBusFound=StartBus;		

            mMaxBusScan=(UINTN)(StartBus+rres->_LEN-1);
            //Maximum bus number must not exceed 0xFF buses!
            if(mMaxBusScan > 0xFF) ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
           
            PCI_TRACE((TRACE_PCI,"PciBus: RB [%d] of HB[%d]: _BBN=0x%X; _SEG=0x%X; ",
                    i,j,StartBus,rb->RbIo->SegmentNumber));

            if(gNoSdlDataMode==FALSE){
                //Here we can effectively map HB->Rb to it's sdl data. 
                //1. Find Rb SDL Entry of the current Root Bridge      
                //If HB and RB driver is AMI's we just type cast PCI_RBIO Protocol pointer to get RbSdlData and its Index 
                Status=AmiSdlFindRbBySignature(rb->RbIo,&rb->AmiSdlPciDevData, &rb->SdlDevIndex);
                
                //in case we failed to find a signature inside type casted PCI_RBIO - third party HB/RB drivers were used...
                if(EFI_ERROR(Status)){
                    //try to find RB SDL Data by matching RB actual StartBus and Segment to SDL Data 
                    Status=AmiSdlFindRbBySegBus(rb->RbIo->SegmentNumber, StartBus, &rb->AmiSdlPciDevData, &rb->SdlDevIndex);
                    if(EFI_ERROR(Status)){
                        UINT8 AslName[5]={0,0,0,0,0};
                        //Looks like a bus shift is i place and SDL Bus Out get shifted relative to rres descriptor..
                        //and we need to use yet another alternative way of mapping RB SDL record.
                    	//Try to use initialization routine. Try to get RB ASL name and then find corresponded SDL record.
                        Status=AmiPciLibLaunchInitRoutine(rb, isPciGetRbAslName, itDevice, rb, &j, &i, AslName);
                        
                        //if we here it MUST be an Init Routine to identify RB, so only EFI_SUCCESS status accepted
                        if(!EFI_ERROR(Status)){
                            AslName[4]=0; //just in case...
                            Status=AmiSdlFindRbByAslName(AslName, &rb->AmiSdlPciDevData, &rb->SdlDevIndex);
                        }  
                        //Not found... pretty bad!
                        if (EFI_ERROR(Status)) PCI_TRACE((TRACE_PCI,"can't find SDL record! "));
                    }
                } 
                
                if (!EFI_ERROR(Status))PCI_TRACE((TRACE_PCI,"AslName=%a; SdlIndex=%d; ",rb->AmiSdlPciDevData->AslName,rb->SdlDevIndex));   
                
                PCI_TRACE((TRACE_PCI,"%r\n",Status));
                
                //It's a serious error if it returns NOT_FOUND need to ASSERT here.
                ASSERT_EFI_ERROR(Status); 
                
                //2.Find it's parent It must be a "HostBridgeType" device
                if(rb->HostData->HbSdlData==NULL){
                    PCI_TRACE((TRACE_PCI,"PciBus: Looking for SDL data of HB [%d] ",j));
                    Status=AmiSdlFindIndexRecord(rb->AmiSdlPciDevData->ParentIndex,&rb->HostData->HbSdlData);
                    if(EFI_ERROR(Status)){
                        PCI_TRACE((TRACE_PCI,"can't locate  SDL record! "));
                    } else {
                        rb->HostData->HbSdlIndex=rb->AmiSdlPciDevData->ParentIndex;
                        PCI_TRACE((TRACE_PCI,"AslName=%a; SdlIndex=%d; ", rb->HostData->HbSdlData->AslName, rb->HostData->HbSdlIndex));
                        //Update gPciHostList....
                        gPciHostList[j]=rb->HostData;
                    }
                } else {
                    //Here to verify that RB belongs to correct HB 
                    PCI_TRACE((TRACE_PCI,"PciBus: Checking...  SDL data of HB [%d] ",j));
                    if(rb->AmiSdlPciDevData->ParentIndex != rb->HostData->HbSdlIndex){
                        PCI_TRACE((TRACE_PCI,"can't confirm SDL record! "));
                    }
                    PCI_TRACE((TRACE_PCI,"AslName=%a; SdlIndex=%d; ", rb->HostData->HbSdlData->AslName, rb->HostData->HbSdlIndex));
                }
                PCI_TRACE((TRACE_PCI,"%r\n",Status));
                ASSERT_EFI_ERROR(Status);

            } else {
                gPciHostList[j]=rb->HostData;
                PCI_TRACE((TRACE_PCI,"No SDL Data Mode;\n"));
            }
            
            //Update PCI Address of the RB Device...
			rb->Address.Addr.Bus=StartBus;
	        if(gNoSdlDataMode==FALSE){
	            rb->Address.Addr.Device=rb->AmiSdlPciDevData->Device;
	            rb->Address.Addr.Function=rb->AmiSdlPciDevData->Function;
	        }
	        
            //Now when we got correct HB->RB mapping...Update things that we skipped in InitPciDb.
            //3. Get Root IRQ ROUTING for this root bridge if exists...
            Status=CreateDevIrqEntry(rb);
            ASSERT_EFI_ERROR(Status);

            //4. Get BusShift Value..
            if(gNoSdlDataMode==FALSE){
                Status=AmiPciBusShift(rb->AmiSdlPciDevData,&rb->FixedBusShift,j,i,FALSE);
            
                PCI_TRACE((TRACE_PCI," BusShiftValue=%d; %r.\n", rb->FixedBusShift, Status));
                if(EFI_ERROR(Status)){
                    //If project has RC RootBridge driver this VAR might not exist...
                    //So EFI_NOT_FOUND status is OK. All other status indicates an ERROR!
                    if(Status!=EFI_NOT_FOUND){
                        ASSERT_EFI_ERROR(Status);
                    } else {
                        rb->FixedBusShift=0;
                    }
                }
            } else {
                rb->FixedBusShift=0;
            }
                
            
			ext->Res[rtBus].Type=tBarBus;
			ext->Res[rtBus].Base=StartBus;
			ext->Res[rtBus].Length=rres->_LEN;
			//Get Attributes to see what this RB could support
			Status=rb->RbIo->GetAttributes(rb->RbIo,&rb->Capab,&rb->Attrib);		
			ASSERT_EFI_ERROR(Status);

//Exclude Hotplug support 
//#if HOTPLUG_SUPPORT
			//this will try to init Root HPC siting behind this bridge if any...
			Status=HpCheckRootHotplug(rb, mMaxBusFound);
			ASSERT_EFI_ERROR(Status);
			//if root hpc init fail for any reason we just must keep going
//#endif
			//	b. Make sure each PCI root bridge handle supports the
			//	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
			//		c. Allocate memory to hold resource requirements. These resources can be two resource trees:
			//		one to hold bus requirements and another to hold the I/O and memory requirements.
			//
			//		e. Scan all the devices in the specified bus range and on the specified segment. If it is a PCIto-
			//		PCI bridge, update the bus numbers and program the bus number registers in the PCI-to-
			//		PCI bridge hardware. If it is an ordinary device, collect the resource request and add up all
			//		of these requests in multiple pools (e.g., I/O, 32-bit prefetchable memory). Combine
			//		different types of memory requests at an appropriate level based on the PCI root bridge
			//		attributes. Update the resource requirement information accordingly. On every PCI root
			//		bridge, reserve space to cover the largest expansion ROMs on that bus, which will allow
			//		the PCI bus driver to retrieve expansion ROMs from the PCI card or device without having
			//		to reprogram the PCI host bridge. Because the memory and I/O resource collection step
			//		does not call any member function of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL, 
			//		it can be performed at a later time.
			//		f. Once the number of PCI buses under this PCI root bridge is known, call
			//		SetBusNumbers() with this information.			

			//Go and do the job! 
            mMaxBusReport=StartBus;
            PCI_TRACE((TRACE_PCI,"PciBus: Root[%d]; Scanning ",i));

            EndBus=StartBus;
        	PCI_TRACE((TRACE_PCI,"Buses Starting from 0x%X \n",  StartBus));

        
            do{
    			Status=EnumerateBus((PCI_DEV_INFO*)rb);
			    if(EFI_ERROR(Status)){
                    if(Status != EFI_NOT_FOUND) return Status;
                } else {
                    if(mMaxBusReport < rb->Address.Addr.Bus) mMaxBusReport = rb->Address.Addr.Bus;
                    rb->Address.Addr.Bus = mMaxBusReport;
        			ext->Res[rtBus].Base = mMaxBusReport;
                }
                
                //Bus is UINT8 to avoid deadloop here check 
                //for condition to berak the loop.
                if(rb->Address.Addr.Bus==0xFF) break;
                rb->Address.Addr.Bus++;
    			ext->Res[rtBus].Base++;
            } while (rb->Address.Addr.Bus <= EndBus);

//#if PCI_EXPRESS_SUPPORT
            //We have collected all Upstream Ports information 
            //Now init it from EndPoint up to RootPort
            PCI_TRACE((TRACE_PCI,"============================================================\n"));
            PCI_TRACE((TRACE_PCI,"PciBus: InitDevice Chain ... Processing gPcieEpList[%d] Items\n",gPcieEpList.ItemCount));
            PCI_TRACE((TRACE_PCI,"------------------------------------------------------------\n"));
            while(gPcieEpList.ItemCount!=0){
                PCI_DEV_INFO *epdev;
                BOOLEAN 	 NoPcie;
            //---------------------
                NoPcie=FALSE;
                epdev=(PCI_DEV_INFO*)gPcieEpList.Items[0];

            	//if PCIe capabilities get overridden by isGetSetupData IntRoutine or PciPort->PciPortCheckBadPcieDevice,
            	//we should remove device from endpoint list...
                if(!PcieCheckPcieCompatible(epdev)) NoPcie=TRUE;
                if(!NoPcie) {
                	Status=PcieCheckIntegratedDevice(epdev);
                	if(EFI_ERROR(Status)){
                		if(Status!=EFI_NOT_FOUND){
                			ASSERT_EFI_ERROR(Status);
                			return Status;
                		}
                	} else {
                		Status=PcieRemoveEpFromLst(epdev, FALSE);
                        PCI_TRACE((TRACE_PCI,"------------------------------------------------------------\n"));
                		continue;
                	}
                }
                if(epdev->ParentBrg!=NULL && !PcieCheckPcieCompatible(epdev->ParentBrg)) NoPcie=TRUE;
                
                if(NoPcie){
                	Status=PcieRemoveEpFromLst(epdev, TRUE);
                    PCI_TRACE((TRACE_PCI,"PciE: Link Members are NOT PCIe (Dev->DevSetup.Pcie1Disable==0x%X)! %r\n",
                            epdev->ParentBrg->DevSetup.Pcie1Disable, Status));
                    continue;
                }
                
                PCI_TRACE((TRACE_PCI,"O=O=O=O= GOING UP from EP Device @ [B%X|D%X|F%X] =O=O=O=O:\n", epdev->Address.Addr.Bus,epdev->Address.Addr.Device, epdev->Address.Addr.Function));                
                PCI_TRACE((TRACE_PCI,"------------------------------------------------------------\n"));
                PCI_TRACE((TRACE_PCI,"PciBus: InitDeviceChain():\n"));
                Status=PcieInitDevChain(epdev);
                
                PCI_TRACE((TRACE_PCI,"------------------------------------------------------------\n"));
                ASSERT_EFI_ERROR(Status);
			    if(EFI_ERROR(Status)) return Status;
            }
            PCI_TRACE((TRACE_PCI,"PciBus: InitDevice Chain Done...gPcieEpList[] %d Items Left.\n",gPcieEpList.ItemCount));
            PCI_TRACE((TRACE_PCI,"============================================================\n"));

//#endif
            //Restore old values    
            rb->Address.Addr.Bus = StartBus;    
			ext->Res[rtBus].Base = StartBus;
            
			//we got lucky!!! finally we are here with information about all resources we need
			//in the Root Bridge device the following fields will reflect ACPI Resource descriptor fields
			// rb->Address.Bus = Primary Bus Number _MIN
			// ext->Res[rtBus].Base = Subordinate Bus Number = _MAX
			// ext->Res[rtBus].Len =  _MAX+1
			ext->Res[rtBus].Base=mMaxBusReport; //that how much this root brg has buses 
			ext->Res[rtBus].Length=mMaxBusReport-StartBus+1;

			//Update discovered Bus Numbers and report Bus Resources to the HOST
			rres->_LEN=ext->Res[rtBus].Length;
			rres->_MAX=rres->_MIN+rres->_LEN-1;
			Status=lhst->ResAllocProt->SetBusNumbers(lhst->ResAllocProt, rb->Handle, rres);
			ASSERT_EFI_ERROR(Status);
			//if everything OK free memory given by StartBusEnumeration function
			pBS->FreePool(rres);

			//Here we have to convert some resourcers which we have discovered but RB doesn't supports
			//Get Allocation Attributes for this Root Bridge it will let us know how to convert resources for this Root bridge
			//Notify HB and CSP
			Status=DoPciNotify(lhst, EfiPciHostBridgeEndBusAllocation);
			ASSERT_EFI_ERROR(Status);

            PROGRESS_CODE(DXE_PCI_BUS_REQUEST_RESOURCES);

            PCI_TRACE((TRACE_PCI,"============================================================================\n"));
			//First do IO - Any PCI Root Bridge supports only Io16 
            if(Io32support) ct=rcBoth;
            else ct=rcNarrow;
			ConvertResources((PCI_BRG_INFO*)rb,tBarIo,ct,(BOOLEAN)(gAllocationAttributes & EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM));

			//than do MMIO
			if(gAllocationAttributes & EFI_PCI_HOST_BRIDGE_MEM64_DECODE)ct=rcBoth; 
			else ct=rcNarrow;
			ConvertResources((PCI_BRG_INFO*)rb,tBarMem,ct,(BOOLEAN)(gAllocationAttributes & EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM));

			//than do pfMMIO 
			if(gAllocationAttributes & EFI_PCI_HOST_BRIDGE_MEM64_DECODE)ct=rcBoth; 
			else ct=rcNarrow;
			ConvertResources((PCI_BRG_INFO*)rb,tBarMemPf,ct,(BOOLEAN)(gAllocationAttributes & EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM));
            PCI_TRACE((TRACE_PCI,"============================================================================\n"));

//???????????????????????????????????????????????????????????????????????????????????????????????????
// Attempt to boot with reduced number of PCI Cards if resources will be not enough  
//??has to be called consequently 
//??or create RecalculateBrgResources()...
//////////////////////////////////////////////////////////////////
//???????????????????????????????????????????????????????????????????????????????????????????????????
#if BoardPciRes_SUPPORT
            
            do{ 
//OUT_OF_RES!!  
                //Reset Flag to stop calling Platform function to select PCI device selected 
                // to be removed from enumeration for this pass. It will be SET in CalculateBrgResources().
                gPciOutOfResHit=FALSE;
                ext=(PCI_BRG_EXT*)(rb+1);
                if(gPciOutOfRes) ClearBrgResources(rb);   
#endif
                //Here, determine enumeration mode we are runnimg at and set corresponded flags...
				//1. Dynamic resource allocation gPciFixedRes==FALSE, gPciFrMixedMode==FALSE;
				//2. Fixed Resources Mode gPciFixedRes==TRUE, gPciFrMixedMode==FALSE;
				//3. Fixed Resources MIXED Mode gPciFixedRes==TRUE, gPciFrMixedMode==TRUE; 

                if(gPciFixedRes) {
                    if(gNotFixedResBarList.ItemCount>0){
                        //we are supporting "Partially Fixed Resources!" with some limitations!
                        PCI_TRACE((TRACE_PCI,"\n  !!!There are %d BAR(s) missing FIXED Values!!!\n  !!!WILL TRY TO USE MIXED ALLOCATION MODE...!!!\n\n", gNotFixedResBarList.ItemCount));
                    }
                }

				
                
    			//Calculate resources Consumed by this Root bridge
    			CalculateBrgResources(rb);
    			rb->Discovered=TRUE;
                //APTIOV_SERVER_OVERRIDE_START
    			PcdSetBool(AmiPcdPciOutOfResourcesStatus,gPciOutOfRes);
                //APTIOV_SERVER_OVERRIDE_END
			    //Notify HB and CSP
    			Status=DoPciNotify(lhst, EfiPciHostBridgeBeginResourceAllocation);
    			ASSERT_EFI_ERROR(Status);

			    //Status=AllocateRootResources(gPciRoot.RootBrg[i], &mRootRes);
			    //Create Resource information to report to the HOST BRG
			    Status=CreateRootResDsc(rb,(VOID**)&rres); 
    			if(EFI_ERROR(Status)){
                    PCI_TRACE((TRACE_PCI,"PciBus: CreateRootResDsc() returned %r \n ",Status));
                    return Status;
                }
            
                //Not Found Condition is Normal for RB it does not have enything behind it.
                //Skip Calling SubmitResources() since nothing to Submit
   	    		Status=lhst->ResAllocProt->SubmitResources(lhst->ResAllocProt, rb->Handle, rres);
    		    if(EFI_ERROR(Status)) {
                    VOID *Configuration;    //This is a dummy parameter.
#if BoardPciRes_SUPPORT
#if PCI_OUT_OF_RESOURCE_SETUP_SUPPORT
                //Declare Boot flow variable GUID...
                    EFI_GUID BootFlowVariableGuid = BOOT_FLOW_VARIABLE_GUID;
                    UINT32   BootFlow = BOOT_FLOW_CONDITION_PCI_OUT_OF_RESOURCE;
                //------------------------------
                    Status=pRS->SetVariable(L"BootFlow",&BootFlowVariableGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS,sizeof(BootFlow),&BootFlow);
#endif

                    //Give Platform Policy Driver to handle OUT_OF_RESOURCES situation 
                    //by itself (it just might want to BEEP and HUNG the system)
                    gPciOutOfRes=TRUE;

#endif
                    // if Allocate Resources Notify Phase returns an ERROR 
                    // Here suppose to be the following logic :
                    // a. Make do with the smaller ranges.
                    // b. Call GetProposedResources() to retrieve the proposed settings and examine the
                    // 	differences. Prioritize various requests and drop lower-priority requests. Call
                    //	NotifyPhase (EfiPciHostBridgeFreeResources) to undo the previous
                    //	allocation. Go back to step 11 with reduced requirements, which includes resubmitting
                    //	requests for all the root bridges.
                    lhst->ResAllocProt->GetProposedResources(
  		                lhst->ResAllocProt,
		                rb->Handle,
		                &Configuration);
#if BoardPciRes_SUPPORT
#else
                    ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
                    ASSERT_EFI_ERROR(Status);
#endif
			        pBS->FreePool(rres);
#if BoardPciRes_SUPPORT
                //APTIOV_SERVER_OVERRIDE_START
    		    } else {
				 gPciOutOfRes=FALSE;
				 PcdSetBool(AmiPcdPciOutOfResourcesStatus,gPciOutOfRes);
				 }
                 //APTIOV_SERVER_OVERRIDE_END
            } while(gPciOutOfRes); //while resources found
#else
			    return Status;
		    }
#endif
		} //loop (i) on submiting resources
#if BoardPciRes_SUPPORT
        //Check if we suppose to beep and report "OUT OF_RESOURCES" condition
        Status=AmiPciOutOfRes(NULL, TRUE);
        if(!EFI_ERROR(Status)) ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
#endif  
        //Notify HB and CSP
		Status=DoPciNotify(lhst, EfiPciHostBridgeAllocateResources);
        PROGRESS_CODE(DXE_PCI_BUS_ASSIGN_RESOURCES);
		ASSERT_EFI_ERROR(Status);

		Status=DoPciNotify(lhst, EfiPciHostBridgeSetResources);
		ASSERT_EFI_ERROR(Status);
#if SMCPKG_SUPPORT
            for(Index = 0; Index < sizeof(BDF_Tbl)/sizeof(BDF_Tbl[0]); Index++)
                BDF_Tbl[Index] = 0;

            NVMe_Port_Flag = PcdGet64(PcdSmcAOCLocation);
            NVMe_Extend_Flag = PcdGet64(PcdSmcAOCExtendFlag);
            for(Index = 0; Index < sizeof(NVMe_Port_Flag); Index++){
                IIO_Flag = (UINT8)RShiftU64(NVMe_Port_Flag, (Index * 8));
                IIO_Extend_Flag = (UINT8)RShiftU64(NVMe_Extend_Flag, (Index * 8));
                TempBus = (UINT8)(SmcBDFTransfer((UINT32)Bus_Tbl[Index] << 16) >> 8);
                if(!IIO_Flag && !IIO_Extend_Flag)       continue;

                if((IIO_Flag == 0x30) || (IIO_Flag == 0x03)){   //x16 slot with NVMe
                    for(TempDev = 0; TempDev < 4; TempDev++, BDF_Tbl_Index++)
                        BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                } else {
                    if((IIO_Flag & 0x0f) == 0x02){      //x8 slot with NVMe, IIO_dev 0, 1
                        for(TempDev = 0; TempDev < 2; TempDev++, BDF_Tbl_Index++)
                            BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    }
                    if((IIO_Flag & 0xf0) == 0x20){      //x8 slot with NVMe, IIO_dev 2, 3
                        for(TempDev = 2; TempDev < 4; TempDev++, BDF_Tbl_Index++)
                            BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    }
                    if(IIO_Flag & 0x01){        //x4 slot with NVMe, IIO_dev 0
                        TempDev = 0;
                        BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                        BDF_Tbl_Index++;
                    }
                    if(IIO_Flag & 0x04){        //x4 slot with NVMe, IIO_dev 1
                        TempDev = 1;
                        BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                        BDF_Tbl_Index++;
                    }
                    if(IIO_Flag & 0x10){        //x4 slot with NVMe, IIO_dev 2
                        TempDev = 2;
                        BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                        BDF_Tbl_Index++;
                    }
                    if(IIO_Flag & 0x40){        //x4 slot with NVMe, IIO_dev 3
                        TempDev = 3;
                        BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                        BDF_Tbl_Index++;
                    }
                }
                if((IIO_Extend_Flag & 0x03) == 0x02){
                    TempDev = 0;
                    BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    BDF_Tbl_Index++;
                }
                if((IIO_Extend_Flag & 0x0C) == 0x08){
                    TempDev = 1;
                    BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    BDF_Tbl_Index++;
                }
                if((IIO_Extend_Flag & 0x30) == 0x20){
                    TempDev = 2;
                    BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    BDF_Tbl_Index++;
                }
                if((IIO_Extend_Flag & 0xC0) == 0x80){
                    TempDev = 3;
                    BDF_Tbl[BDF_Tbl_Index] = ((UINT16)TempBus << 8) + (TempDev << 3);
                    BDF_Tbl_Index++;
                }
            }
            for(Index = 0; Index < sizeof(BDF_Tbl)/sizeof(BDF_Tbl[0]); Index++){
                if(!BDF_Tbl[Index])     continue;
                DEBUG((-1, "BDF_Tbl[%d] = %x.\n", Index, BDF_Tbl[Index]));
            }
#endif
		//now enter the loop where we will get resource windows selected by the Root Bridge 
		//and programm it in to PCI subsystem BARs
		for(i=0; i<lhst->RbCount; i++){
			rb=lhst->RootBridges[i];			
			ext=(PCI_BRG_EXT*)(rb+1);

			//Get Copy of updated resources to see if our call worked
			Status=rb->RbIo->Configuration(rb->RbIo,(VOID**)&rres);
			if (Status==EFI_NOT_AVAILABLE_YET) continue;

			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

    		ApplyAcpiResources(ext, rres);
		
			//now we have to assign PCI bus resources, and PciRoot will be the special case.
			//We should not touch the device which adressed as gPciRoot.RootBrg[i].Address
			//because it is controlled by PciRootBridge Driver and we must not directly access it
			//So AssignBusResources routine has a checking of such condition
			Status=AssignBusResources((PCI_BRG_INFO*)rb);
			if(EFI_ERROR(Status)) return Status;

//#if PCI_EXPRESS_SUPPORT
			//Record PciEcpress Boot Script since not all OSs knows about it
			if((gS3SaveState!=NULL) && (gPciCommonSetup->S3PciExpressScripts)){
			    PcieRecordBootScript(rb, gS3SaveState, gPciCommonSetup->S3PciExpressScripts);
			}
//#endif
	
			//Set Started Flag for RootBridge device in order not to install PCI IO on it later
			rb->Started=TRUE;
		}//loop (i) for root bridge
		Status=DoPciNotify(lhst, EfiPciHostBridgeEndResourceAllocation);
		ASSERT_EFI_ERROR(Status);
		
		lhst->Enumerated=TRUE;
	} //loop (j)
#if S3_VIDEO_REPOST_SUPPORT == 1
    if( (gS3SaveState!=NULL) && (gPciCommonSetup->S3ResumeVideoRepost==1) ){
        // Register the event handling function to Record Primary VGA BootScript.
        Status = CreateReadyToBootEvent (
                        TPL_CALLBACK,
                        RecordPriVgaBootScript,
                        NULL,
                        &gVgaS3Event);
    }
#endif

	return Status;
}


/**
    Checks if "DevicePath" is a PCI_DEVICE_PATH.

        
    @param DevicePath Device Path Pointer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When "DevicePath" is not a PCI_DEVICE_PATH.
**/
EFI_STATUS EFIAPI CheckPciDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
    // Check if the RemainingDevicePath is valid
	if (DevicePath->Type != HARDWARE_DEVICE_PATH ||
		DevicePath->SubType != HW_PCI_DP &&
		NODE_LENGTH(DevicePath) != sizeof (PCI_DEVICE_PATH)) 
      return EFI_UNSUPPORTED;
	else return EFI_SUCCESS;
}


/**
    PCI Firmware Spec 3.0 defines some reserved field in PCIR 
    Structure as Device List Pointer. This function checks additional Device IDs  

        
    @param Dev Pointer PCI Devicse Private Data.
    @param PciRomStruct Pointer at PCIR ROM Header
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When ROM is not compatible with device.
**/
EFI_STATUS EFIAPI CheckPciRomDevId(PCI_DEV_INFO    *Dev, PCI_DATA_STRUCTURE *PciRomStruct){

    if(Dev->DevVenId.VenId != PciRomStruct->VendorId) return EFI_NOT_FOUND;
    
    if(Dev->DevVenId.DevId != PciRomStruct->DeviceId){
	    UINT16      *did;
	//--------------------------
        //Dev->DevVenId.DevId ==0x0000 special case - (Service ROM like PXE, iSCIS boot agent)
        if(Dev->DevVenId.DevId==0) return EFI_SUCCESS;

        //Check Device List if any...
        if(PciRomStruct->Reserved0 == 0) return EFI_UNSUPPORTED;

        did=(UINT16*)((UINT8*)PciRomStruct + PciRomStruct->Reserved0);        
        
        while(*did != 0){
            if(Dev->DevVenId.DevId==*did) return EFI_SUCCESS;        
            did++; // next device list entry      
        }
        return EFI_UNSUPPORTED;    
    }

    return EFI_SUCCESS;
}


/**
    This function called when PciRomHdr->InitializationSize and 
    Pcir->ImageSize has different values and Pcir->Indicator bit 7 is not set. 
    It evaluets whch one is correct by Searching for 0x55AA signature that is 
    must present in the beginning of next OptROM image.

        
    @param PciRomHdr Pointer to Legacy Option ROM Header(0x55AA).
    @param Pcir Pointer to PCI ROM header ("PCIR")
  
    @retval EFI_STATUS
        UINTN   Size of the ROM in bytes;
    @note  PCIR and OptROM signatures assumed to be valid. 
**/
UINTN   EFIAPI DoubleCheckOpRom(PCI_DEV_INFO *Dev, VOID *PciRomHdr, PCI_DATA_STRUCTURE *Pcir, BOOLEAN BarAccess){
    UINT8                   *p;
    UINTN                   sz=0;
    UINTN                   hsz=0;
    LEGACY_OPT_ROM_HEADER   rhdr;
    LEGACY_OPT_ROM_HEADER   *rhdrptr;
    EFI_STATUS              Status;
    BOOLEAN                 sigfound=FALSE;
//---------------------------------
    //Get OptRom Header using PCI RB IO instead of direct memory access.
    //it might use Address translation on access
    if(BarAccess){
        Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint8, (UINT64)((UINTN)PciRomHdr),
                            sizeof(LEGACY_OPT_ROM_HEADER), &rhdr);
        ASSERT_EFI_ERROR(Status);
        
        rhdrptr=&rhdr;
    } else rhdrptr=PciRomHdr;

    if ( ((LEGACY_OPT_ROM_HEADER*)rhdrptr)->Signature==PCI_OPT_ROM_SIG ) sigfound=TRUE;
    
    //we should deal with rhdrptr pointer since in ARM systems Direct Memory access may hung the system.
    //EFI Compatible ROM headers have 16 bit Length field... 
    if( ((PCI_EFI_OPT_ROM_HEADER*)rhdrptr)->EfiSignature == PCI_OPT_ROM_EFISIG){
        hsz=((PCI_EFI_OPT_ROM_HEADER*)rhdrptr)->InitializationSize; //16 bit size
        if(Dev->AmiRomPolicy!=NULL) Dev->AmiRomPolicy->RomStatus|=(AMI_OPTION_ROM_POLICY_UEFI);
    } else {
        hsz=((LEGACY_OPT_ROM_HEADER*)rhdrptr)->Size512;
        if(Dev->AmiRomPolicy!=NULL) Dev->AmiRomPolicy->RomStatus|=(AMI_OPTION_ROM_POLICY_LEGACY);
    }

    if(hsz == Pcir->ImageLength ) return (hsz*512);

    p=(UINT8*)PciRomHdr;

    //check both rom sizes which has a valid PCI ROM image at the end...
    sz= Pcir->ImageLength*512;
    
    if(BarAccess){
        //Get OptRom Header using PCI RB IO instead of direct memory access.
        Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint8, (UINT64)((UINTN)(p+sz)),
                            sizeof(LEGACY_OPT_ROM_HEADER), &rhdr);
        ASSERT_EFI_ERROR(Status);

        rhdrptr=&rhdr;
    } else rhdrptr=(LEGACY_OPT_ROM_HEADER*)(p+sz);
    
    if( ((LEGACY_OPT_ROM_HEADER*)rhdrptr)->Signature==PCI_OPT_ROM_SIG) return sz;

    //if not try other size value right next to 0x55aa signature.
    hsz=hsz*512;
    
    if(BarAccess){
        //Get OptRom Header using PCI RB IO instead of direct memory access.
        Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint8, (UINT64)((UINTN)(p+hsz)),
                             sizeof(LEGACY_OPT_ROM_HEADER), &rhdr);
        ASSERT_EFI_ERROR(Status);

        rhdrptr=&rhdr;
    } else rhdrptr=(LEGACY_OPT_ROM_HEADER*)(p+hsz);
    
    if( ((LEGACY_OPT_ROM_HEADER*)rhdrptr)->Signature==PCI_OPT_ROM_SIG) return hsz;

    //If we still here we are in trouble!
    //will return bigger size..
    if(hsz>sz)sz=hsz;

    //if this is a last ROM image...
    if(Pcir->Indicator&0x80) return sz;//(EIP37774+)
    
    //Here try to check... if we got here from adding 512 to the size iteration or 
    //just got image with incorrect Indicator settings.
    //if at the beginning of image we had 0x55AA-ROM_SIG return bigger size though.
    if (sigfound) return sz;
    
    //else try to iterate 512 bytes more, until BAR size runs out or ROM_SIG found...
    return 512;
}

/**
    Validate and copy Embedded PCI Option ROM Image FFS File storage 
    in to the System Memory.

        
    @param Dev Pointer PCI Devicse Private Data.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.
**/
EFI_STATUS EFIAPI GetEmbeddedRom(PCI_DEV_INFO *Device){
	UINTN					i;
	EFI_STATUS				Status, retStatus=EFI_NOT_FOUND; 
	AMI_SDL_PCI_DEV_INFO	*romSdlData;
	VOID					*romFile;
	UINTN					romSize;
	PCI_STD_OPT_ROM_HEADER	*StdRomHdr;
	PCI_DATA_STRUCTURE		*PcirStruct;
	EMB_ROM_INFO			*EmbRomInfo;
#if SMCPKG_SUPPORT && SmcOpromCtrl_SUPPORT
#if defined SMC_ONBOARD_LAN_DID1 && SMC_ONBOARD_LAN_DID1 != 0xFFFF
	UINT8		*TempAddress;
	UINT32		LanOpRomSign = 0;
	SETUP_DATA	SetupData;
	UINTN		SetupSize = sizeof(SETUP_DATA);
	EFI_GUID	SetupGuid = SETUP_GUID;
#endif
#endif // #if SMCPKG_SUPPORT
//-----------------------------------
	
	for(i=0; i<Device->SdlDevCount; i++){
		//for this function EFI_NOT_FOUND and 
		Status=PciBusReadNextEmbeddedRom(Device, i, &romSdlData, &romFile, &romSize);
		if(EFI_ERROR(Status)){
			//Not found is OK Status for this routine.
			//Device mnight not have an option ROM or container object was for ASL 
			if (Status==EFI_NOT_FOUND) continue;
			ASSERT_EFI_ERROR(Status);
			return Status;
		} 
		
		//Get memory to accomodate Rom Image info...
		EmbRomInfo=MallocZ(sizeof(EMB_ROM_INFO));
		if(EmbRomInfo==NULL){
			ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
			return EFI_OUT_OF_RESOURCES;
		}
		
		EmbRomInfo->EmbRom=romFile;
		EmbRomInfo->EmbRomSize=romSize;

		//Here we have read something from FFS file, try to validate it and see what is it...
		//we are going to look at 3 possible options here.
		//1. Multiple ROM Image wrapped in 0x55AA STD PCI ROM Header with $PCIR structure inside which might have an UEFI Image.
		//2. Separate UEFI Driver Binary in format of PE32 not wrapped in 0x55AA ROM Header.
		//3. Combination of both defined in #1 and #2
		//Use Dedicated romSdlData->PciDevFlags for Option ROM type
		StdRomHdr=(PCI_STD_OPT_ROM_HEADER*)romFile;
		if ((StdRomHdr->Signature != PCI_OPT_ROM_SIG) || (StdRomHdr->PcirOffset == 0) || 
				(romSdlData->PciDevFlags.Bits.UefiDriverBin==1) ){
			//This option rom is UEFI Driver...
			EmbRomInfo->UefiDriver=TRUE;
		} else {
			//STD ROM Image... read VID/DID info from PCIR struct.
			PcirStruct=(PCI_DATA_STRUCTURE*)( ((UINT8*)romFile)+StdRomHdr->PcirOffset);
#if SMCPKG_SUPPORT && SmcOpromCtrl_SUPPORT
#if defined SMC_ONBOARD_LAN_DID1 && SMC_ONBOARD_LAN_DID1 != 0xFFFF
			if(PcirStruct->DeviceId == SMC_ONBOARD_LAN_DID1 || PcirStruct->DeviceId == SMC_ONBOARD_LAN_DID2 
#if defined SMC_ONBOARD_LAN_DID3 && SMC_ONBOARD_LAN_DID3 != 0xFFFF
                        || PcirStruct->DeviceId == SMC_ONBOARD_LAN_DID3 || PcirStruct->DeviceId == SMC_ONBOARD_LAN_DID4
#endif
                        ){
			    Status = pRS->GetVariable(
						L"Setup",
						&SetupGuid,
						NULL,
						&SetupSize,
						&SetupData);
			    if(!Status){
				if(SetupData.SmcLanOpRom[0] == 0x01){		// load PXE
				    TempAddress = (UINT8*)romFile + *(UINT16*)((UINT8*)romFile + 0x16);
				    LanOpRomSign = *(UINT32*)(TempAddress);
				    if(!((LanOpRomSign == 0x49444E55) || (LanOpRomSign == 0x24434224)))
					continue;
				} else if(SetupData.SmcLanOpRom[0] == 0x02) {	// load iSCSI
				    TempAddress = (UINT8*)romFile + 0x08;
				    LanOpRomSign = *(UINT32*)(TempAddress);
				    if(!((LanOpRomSign == 0x504C4324) || (LanOpRomSign == 0x45534924)))
					continue;
				} else if(SetupData.SmcLanOpRom[0] == 0x03){	// load FCoE
				    TempAddress = (UINT8*)romFile + 0x08;
				    LanOpRomSign = *(UINT32*)(TempAddress);
				    if(!(LanOpRomSign == 0x43466924))
					continue;
				}
			    }
			}
#endif
#endif // #if SMCPKG_SUPPORT && SmcOpromCtrl_SUPPORT
			EmbRomInfo->EmbRomDevVenId.VenId=PcirStruct->VendorId;
			EmbRomInfo->EmbRomDevVenId.DevId=PcirStruct->DeviceId;

			if(EmbRomInfo->EmbRomDevVenId.DEV_VEN_ID!=Device->DevVenId.DEV_VEN_ID) {
		        //PCI Firmware Spec 3.0 defines some reserved field in PCIR Structure as supported DID List Pointer  
		        Status=CheckPciRomDevId(Device, PcirStruct);
		        if(EFI_ERROR(Status)) {
		        	//Routine returns NOT_FOUND and UNSUPPORTED as well as SUCCESS
		            if(Status==EFI_NOT_FOUND){
		            	//NOT_FOUND if  Vendor ID not mutch.
		                PCI_TRACE((TRACE_PCI," EmbROM Img Dev.VID!=PCIR.VID "));
		            } else //UNSUPPORTED Device ID not mutch
		            	PCI_TRACE((TRACE_PCI," EmbROM Img #%d Dev.DID!=PCIR.DID "));
		        } else EmbRomInfo->EmbRomDevVenId.DEV_VEN_ID=Device->DevVenId.DEV_VEN_ID;
			}
		}

		//Feed Embedded ROM data structure to the PCI_DRV_INFO struct...
		retStatus=AppendItemLst((T_ITEM_LIST*)&Device->EmbRomInitCnt,EmbRomInfo);	
		ASSERT_EFI_ERROR(retStatus);
		if(EFI_ERROR(retStatus)) break;
	}
	
	return retStatus;
}

EFI_STATUS EFIAPI CheckPlatformRom(PCI_DEV_INFO *Dev){
	EFI_STATUS 	Status=EFI_NOT_FOUND;
	UINTN       romsz=(UINTN)Dev->PciIo.RomSize;
//-----------------
	if(Dev->HostData->PlatformProt!=NULL){
		Status=Dev->HostData->PlatformProt->GetPciRom(
								Dev->HostData->PlatformProt,
								Dev->Handle,
								&Dev->PciIo.RomImage,
								//&Dev->PciIo.RomSize
								&romsz);
	
		
	}	
		
	//Platform protocol has priority over Override protocol
	//Override protocol must called if Platform - not present 
	//or returned an ERROR!
	if( EFI_ERROR(Status) && (Dev->HostData->PciOverrideProt!=NULL) ){
		Status=Dev->HostData->PciOverrideProt->GetPciRom(
								Dev->HostData->PciOverrideProt,
								Dev->Handle,
								&Dev->PciIo.RomImage,
								//&Dev->PciIo.RomSize
								&romsz);
	
	}

   	//The only thing has to be done here: setting the Embedded ROM 
   	//attributes and capabilities, since Rom image came from one of 
   	//PLATFORM protocol function members
	if(!EFI_ERROR(Status)){
	    Dev->PciIo.RomSize=romsz;
    	Dev->Attrib|=(Dev->Attrib|EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
    	Dev->Capab|=(Dev->Capab|EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
    }

	return Status;
}

/**
    Validate and copy PCI Option ROM Image From ADD ON CARD to
    the System Memory.

        
    @param Dev Pointer PCI Devicse Private Data.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.
**/
EFI_STATUS EFIAPI GetOptRom(PCI_DEV_INFO	*Dev)
{
	EFI_STATUS				Status, ExitStatus=0; 
	PCI_BAR					*rombar=&Dev->Bar[PCI_MAX_BAR_NO];
	UINTN					OptRomSize = 0;
    PCI_DATA_STRUCTURE      Pcir;
    PCI_STD_OPT_ROM_HEADER  PciRomHdr;
    BOOLEAN                 ff=TRUE;
    UINT64                  base=rombar->Base;
    UINTN                   sz=0;
//-------------------------------------
    if(IgnoreOptionRom) return EFI_NOT_FOUND;
    
    //PCI_PLATFORM and PCI_OVERRIDE have priority over APTIO 5 Embedded and Oncard ROMs!
    //So if it is returned success - don't explore other options.
    Status=CheckPlatformRom(Dev);
    if(!EFI_ERROR(Status)) return Status;		
    
    Status=EFI_NOT_FOUND;
    //we got Opt ROM to check...
	if(Dev->Bar[PCI_MAX_BAR_NO].Type!=tBarUnused){
	    UINT32		trs; //temporary rom size
	//------------------------
	    //If policy tells to skip ROM ... 
	    if(Dev->AmiRomPolicy!=NULL && Dev->AmiRomPolicy->CurrentPolicy==0) return EFI_NOT_FOUND;

	    //Enable RomBar Decoding
		//We will use PciIo->SetAttributes here to enable device Memory Space.
		//If device sits behind some P2P brg Attributes will enable it all the way down   
		//Per UEFI 2.1 spec bridges must be open and decoding it's resources.
		//Status=DeviceAttributes(Dev, EFI_PCI_IO_ATTRIBUTE_MEMORY, TRUE);
		//if(EFI_ERROR(Status))return Status;

		Status=EnableDeviceDecoding(Dev,stOptRomSpace);	
		if(EFI_ERROR(Status))return Status;

	    for(trs=0;;){
		    
 	        if(base + trs + 512 >= rombar->Base+rombar->Length) break;
   
	        // Get the first 20 bytes of the ROM header
	    	Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint8, base+trs,
		    sizeof(PCI_STD_OPT_ROM_HEADER), &PciRomHdr);
		    if(EFI_ERROR(Status))goto ExitLbl;  

	    	// Check the validity of the ROM
		    if ((PciRomHdr.Signature != PCI_OPT_ROM_SIG) || (PciRomHdr.PcirOffset == 0) ){
	            if(ff) {
			        Status=EFI_NOT_FOUND;
			        goto ExitLbl;
		        } else {
	                //base+=512;
	                trs+=512;
	                continue;
	            }
	        } 
	        ff=FALSE;
               
	    	Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint8, base+trs+PciRomHdr.PcirOffset,
								    sizeof(PCI_DATA_STRUCTURE), &Pcir);
		    if(EFI_ERROR(Status))goto ExitLbl;  
        
	        if(Pcir.Signature!=PCI_PCIR_SIG){
            //Status=EFI_NOT_FOUND;
            //goto ExitLbl;
 	           continue;        
	        }        

	        OptRomSize=trs;
			//PciRomHdr should point at the actual rom buffer not a small copy of it.
	        // sz=DoubleCheckOpRom((VOID*)&PciRomHdr, &Pcir);
	        //Last parameter indicates that we accessing PCI BAR to get option ROM
	        sz=DoubleCheckOpRom(Dev,(VOID*)(base+trs), &Pcir, TRUE);
            OptRomSize+=sz;
            
	        trs=(UINT32)OptRomSize;

	        if(Pcir.Indicator & 0x80) break;
		}//for(;;)

		//Allocate Memory For Opt Rom Image Buffer
		Dev->PciIo.RomImage=Malloc(OptRomSize);
		if(Dev->PciIo.RomImage==NULL){
			Status=EFI_OUT_OF_RESOURCES;
			goto ExitLbl;    
		}

	   	//Copy Rom image into memory
		if(UsePciIoForOptRom){
		    Status=Dev->RbIo->Mem.Read(Dev->RbIo, EfiPciWidthUint32,rombar->Base,
									OptRomSize/sizeof(UINT32),Dev->PciIo.RomImage);
		    if(EFI_ERROR(Status)) goto ExitLbl;
		} else {
		    MemCpy32(Dev->PciIo.RomImage, (VOID*)(rombar->Base), OptRomSize);
		}

		//We didnot parse OPT ROM buffer yet so RomSize will have temp value 
		Dev->PciIo.RomSize=OptRomSize;

ExitLbl:
		//Disable decoding option rom again 
		//Per UEFI 2.1 spec bridges must be open and decoding it's resources.
		//ExitStatus=DeviceAttributes(Dev, EFI_PCI_IO_ATTRIBUTE_MEMORY, FALSE);
        if(Dev->Type == tPciDevice) ExitStatus=DisableDeviceDecoding(Dev,stOptRomSpace);
        else ExitStatus=EFI_SUCCESS;
	} //if(Dev->Bar[PCI_MAX_BAR_NO].Type!=tBarUnused) 
	//We tryed to Find PCI Off Board Card Option ROM because it's ROM BAR was valid. 
	//But Some On Board Devices Implements ROM BAR but It does not have a valid ROM
	//So I will check for embeded Option ROM if Status==EFI_NOT_FOUND and ExitStatus==EFI_SUCCES

	//Here we'll try to locate Embeded rom For Onboard devices.
	if((Status==EFI_NOT_FOUND)&&(ExitStatus==EFI_SUCCESS)){
		UINTN	i;
	//------------	
   	    //If policy tells to skip ROM ... 
        if(Dev->AmiRomPolicy!=NULL && Dev->AmiRomPolicy->CurrentPolicy==0) return EFI_NOT_FOUND;

		Status=GetEmbeddedRom(Dev);
		if(!EFI_ERROR(Status)){
			EMB_ROM_INFO	*embrom;
		//---------------	
			//we found Embedded ROM check - if more than one ROM Imnage there try to find a best much.
			if(Dev->EmbRomCnt > 0){
				for(i=0; i<Dev->EmbRomCnt;i++){
					embrom=Dev->EmbRoms[i];
					if(!embrom->UefiDriver){
						if(embrom->EmbRomDevVenId.DEV_VEN_ID==Dev->DevVenId.DEV_VEN_ID){
							Dev->PciIo.RomImage=embrom->EmbRom;
							Dev->PciIo.RomSize=embrom->EmbRomSize;
							//Update Capabilities...
							Dev->Capab |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
							Dev->Attrib  |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
							break;
						}
					}
				}
			}
		}	
	}

	if(EFI_ERROR(ExitStatus)) return ExitStatus;
    //Calling OEM Hook to override Option Rom content...
    //Status=PciPortOemGetOptRom(Dev, &Dev->PciIo.RomImage, &Dev->PciIo.RomSize);
    //if porting hook not implemented, adjust Status, otherwice return whatever we got 
    //if(Status==EFI_UNSUPPORTED) Status=EFI_SUCCESS;
	ExitStatus=AmiPciLibLaunchInitRoutine(Dev, isPciGetOptionRom, itDevice, Dev, &Dev->PciIo.RomImage, &Dev->PciIo.RomSize, NULL);
	if(EFI_ERROR(ExitStatus)){
		if(ExitStatus!=EFI_UNSUPPORTED){
			ASSERT_EFI_ERROR(ExitStatus);
			Status=ExitStatus;
		} 
	} else {
        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X Overrides ROM file @ %X  size=%X .\n",
            Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function,
            Dev->DevVenId.VenId, Dev->DevVenId.DevId, Dev->PciIo.RomImage, Dev->PciIo.RomSize));
            //If Device did not have an Op ROM but OEM hook delivered it we will assume 
            //a SUCCESS status from porting hook invocation.
            Status=ExitStatus;
	}
	return Status;
}


EFI_STATUS EFIAPI LoadRomFile( IN EFI_LOAD_FILE2_PROTOCOL  *This, 
                        IN EFI_DEVICE_PATH_PROTOCOL *FilePath,
                    	IN BOOLEAN                  BootPolicy, 
                        IN OUT UINTN                *BufferSize,
	                    IN VOID                     *Buffer OPTIONAL)
{
    PCI_LOAD_FILE_DATA  *lfdata=(PCI_LOAD_FILE_DATA*)This;   
    PCI_DEV_INFO        *dev=lfdata->Owner;
    EFI_STATUS          Status=EFI_SUCCESS;
    UINTN               i;
    MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH *dp=(MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH*)FilePath;    
    PCI_ROM_IMAGE_DATA  *romdata;
    BOOLEAN             found=FALSE;
	VOID					*imgb=NULL;
	UINT32					imgsz;
	BOOLEAN					emb;
//---------------------

    //Check input parameters first...
    if( (dev->Signature != AMI_PCI_SIG) || 
        (FilePath==NULL) || (BufferSize==NULL)||
        (dev->EfiRomCount==0) ) return EFI_INVALID_PARAMETER;
        
    if( (dp->Header.Type==MEDIA_DEVICE_PATH)||
        (dp->Header.SubType==MEDIA_RELATIVE_OFFSET_RANGE_DP) ){
    		emb=FALSE;
        } else {
            if( (dp->Header.Type==HARDWARE_DEVICE_PATH)||
                (dp->Header.SubType==HW_MEMMAP_DP) ){
            	emb=TRUE;
            } else return EFI_INVALID_PARAMETER;
        }
    
    if(BootPolicy==TRUE) return EFI_UNSUPPORTED;

    for(i=0;i<dev->EfiRomCount; i++){
        romdata=dev->EfiRomImages[i];
    	//verify that relative offset corresponds to the images stored.
        if(emb){
        	if(MemCmp(dp,romdata->RomImageDP,sizeof(MEMMAP_DEVICE_PATH))==0){
        		found=TRUE;
        		break;
        	}                
        } else {
        	if(MemCmp(dp,romdata->RomImageDP,sizeof(MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH))==0){
        		found=TRUE;
        		break;
        	}                
        }
    }
    
    if(!found) return EFI_NOT_FOUND;
    
    if((romdata->ActualSize<*BufferSize)||(Buffer==NULL)){
        *BufferSize=romdata->ActualSize;
        if(Buffer==NULL)return EFI_SUCCESS;
        else return EFI_BUFFER_TOO_SMALL;
    }
    
    //here we have checked everything Time to read file ...
    if(emb){
    	imgb=romdata->RomStart;
		imgsz=(UINT32)romdata->ActualSize;
    } else {
    	Status=ReadEfiRom(dev, romdata,&imgb,&imgsz);
    	if(EFI_ERROR(Status)) return Status;
    }

    MemCpy(Buffer,imgb,imgsz);
    
    if(!emb) pBS->FreePool(imgb);
    return Status;       

}

/**
    PCI Bus Specific Override Protocol Get Override Driver 
    function implementation of the PCI Bus Driver. 

        
    @param This Pointer PCI Devicse's
        Bus Specific Override 
        Protocol Interface.
    @param DriverImageHandle Image Handle of the Option
    @param Driver .
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND		    When there are no more handles to override.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.
**/
EFI_STATUS EFIAPI OvrGetDriver(IN EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL  *This,
						IN OUT EFI_HANDLE                             *DriverImageHandle )
{
	PCI_BUS_OVERRIDE_DATA		*ovr=(PCI_BUS_OVERRIDE_DATA*)This;
	UINTN						i;
	EFI_HANDLE					himg=NULL;
    PCI_DEV_INFO                *dev=ovr->Owner;
    PCI_ROM_IMAGE_DATA          *romdata;
//------------------------------------------------------------------  

    if((DriverImageHandle==NULL)||(dev->EfiRomCount==0))return EFI_INVALID_PARAMETER;

    //Here we know dev->EfiRomCount>0
	if(*DriverImageHandle==NULL) {
        romdata=dev->EfiRomImages[0];
        himg=romdata->RomImgHandle;
	} else {
		for(i=0; i<dev->EfiRomCount; i++){
            romdata=dev->EfiRomImages[i];   
			if(*DriverImageHandle==romdata->RomImgHandle)break;
		}

		if(i==dev->EfiRomCount) return EFI_INVALID_PARAMETER;
//APTIOV_SERVER_OVERRIDE_START	
		if(i<dev->EfiRomCount-1) {
			romdata=dev->EfiRomImages[i+1]; //read next item
			himg=romdata->RomImgHandle;
		}
		else return EFI_NOT_FOUND;
	}
//APTIOV_SERVER_OVERRIDE_END
	*DriverImageHandle=himg;
	return EFI_SUCCESS;
}

/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   AddOvrHandle()
//
// Description: Adds EFI_HANDLE of the Addon Card Option ROM image to the 
// PCI Bus Driver Private Data.
//
// Input:
//  PCI_BUS_OVERRIDE_DATA   *OvrData    Pointer to the PCI Devicse's Bus 
//                                      Override data inside PCI_DEV_INFO
//  EFI_HANDLE              OvrHandle   Handle to add.
//  
// Output:	EFI_STATUS
//  EFI_SUCCESS             When everything is going on fine!
//  EFI_OUT_OF_RESOURCES    When system runs out of resources.
//  EFI_INVALID_PARAMETER   When Parameter passed is invalid.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AddOvrHandle( PCI_DEV_INFO *Device, EFI_HANDLE OvrHandle, UINTN StartOffset, UINTN EndOffset)
{
    PCI_ROM_IMAGE_DATA          *romdata;
//-------------------------------------------------
    if(Device->BusOvrData.BusOverride.GetDriver==NULL)Device->BusOvrData.BusOverride.GetDriver=OvrGetDriver;

		
	//Get the space needed
	romdata=MallocZ(sizeof(PCI_ROM_IMAGE_DATA));
	if (romdata==NULL) return EFI_OUT_OF_RESOURCES;	
	
    romdata->RomImgHandle=OvrHandle;
    romdata->RelOffsStart=StartOffset;
    romdata->RelOffsEnd=EndOffset;    

	return AppendItemLst((T_ITEM_LIST*)&Device->RomInitCnt, romdata);	
}
*/

/**
    Decompresses Option ROM Image if it is Compressed.

        
    @param Image Pointer at the compressed Option ROM Image.
    @param ImageLength Length of the Compressed Image.
    @param DecompImage Place holder to return pointer to the 
        Decopressed Image.
    @param ImageLength Length of the Decompressed Image.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.
**/
EFI_STATUS EFIAPI DecompressOptRom( VOID *Image, UINT32 ImageLength, VOID** DecompImage, UINT32 *DecompSize)
{
	EFI_STATUS 					Status;	
	EFI_DECOMPRESS_PROTOCOL		*dcp;
	UINT32						scrsz, imgsz;
	VOID						*imgb, *scrb;
//--------------------------------
	//Check for parameter first
	if(!DecompImage) return EFI_INVALID_PARAMETER;

	Status = pBS->LocateProtocol(&gEfiDecompressProtocolGuid, NULL,(VOID**)&dcp);
	if(EFI_ERROR(Status))return Status;

	Status = dcp->GetInfo(dcp,Image,ImageLength, &imgsz, &scrsz);
	if (EFI_ERROR (Status)) return Status;

	Status=pBS->AllocatePool(EfiBootServicesData, imgsz,&imgb);
	if(EFI_ERROR(Status)) return Status;

	Status=pBS->AllocatePool(EfiBootServicesData,	scrsz,&scrb);
	if(EFI_ERROR(Status)) return Status;

	Status=dcp->Decompress(dcp,Image,ImageLength,imgb,imgsz,scrb,scrsz);
	pBS->FreePool(scrb);

	if(!EFI_ERROR(Status)){
		*DecompImage=imgb;
		*DecompSize=imgsz;
	}

	return Status;
}


EFI_STATUS EFIAPI ReadEfiRom(PCI_DEV_INFO	*Dev, PCI_ROM_IMAGE_DATA *RomData, VOID **ImgBase, UINT32 *ImgSize){
    EFI_STATUS  Status=EFI_SUCCESS;
	VOID					*imgb;
	UINT32					imgsz;
    MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH	*RoDp=(MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH*)RomData->RomImageDP;
    PCI_EFI_OPT_ROM_HEADER	*RomHeader=(PCI_EFI_OPT_ROM_HEADER*)RomData->RomStart;
//-------------------

    imgb=(VOID*)((UINTN)Dev->PciIo.RomImage+(UINTN)RoDp->StartingOffset);
    imgsz=(UINT32)(RoDp->EndingOffset - RoDp->StartingOffset+1);

    if (RomHeader->CompressionType){
        Status=DecompressOptRom((UINT8*)RomHeader+RomHeader->EfiImageOffset,imgsz,&imgb,&imgsz);
	} 
    
    if(!(EFI_ERROR(Status))){
        *ImgBase=imgb;
        *ImgSize=imgsz;
    }    
    return Status;

}



/**
    Loads and Starts Native EFI PCI Option ROM.

        
    @param Dev Pointer PCI Devicse Private Data.
    @param RomHeader Pointer at PCI Option ROM Header
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.
**/
EFI_STATUS EFIAPI InstallEfiOpRom(	PCI_DEV_INFO			*Dev,
							PCI_EFI_OPT_ROM_HEADER	*P1,
                            PCI_DATA_STRUCTURE      *P2,
                            BOOLEAN					EmbDriver)
{
	EFI_STATUS				Status=0;
	VOID					*imgb;
	UINT32					imgsz;
    PCI_ROM_IMAGE_DATA      *romdata;
    EFI_DEVICE_PATH_PROTOCOL *romdp=NULL;
    MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH	*RoDp=NULL;
    MEMMAP_DEVICE_PATH	 					*MmDp=NULL;
//-------------------------------------------
    //Check Parameters for 0x55AA ROMs
    if(!EmbDriver){
    	if ( (((PCI_EFI_OPT_ROM_HEADER*)P1)->EfiSubsystem != EFI_IMAGE_BS_DRIVER) &&
    			(((PCI_EFI_OPT_ROM_HEADER*)P1)->EfiSubsystem != EFI_IMAGE_RT_DRIVER)) 
    		return EFI_NOT_FOUND;

    	// decompress it if needed
    	if (((PCI_EFI_OPT_ROM_HEADER*)P1)->CompressionType > 1)	return EFI_INVALID_PARAMETER;
	}	
    
	//Get the space needed
    romdata=MallocZ(sizeof(PCI_ROM_IMAGE_DATA));
    if (romdata==NULL) return EFI_OUT_OF_RESOURCES;

    //Fill RomData Parts
    romdata->RomStart=P1;
    
    if(!EmbDriver){
    	PCI_EFI_OPT_ROM_HEADER	*RomHeader=(PCI_EFI_OPT_ROM_HEADER*)P1;
        PCI_DATA_STRUCTURE      *Pcir=(PCI_DATA_STRUCTURE*)P2;
    //-------------------------
    	//Init RELATIVE OFFS DP Header...
    	RoDp=MallocZ(sizeof(MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH));
		if (RoDp==NULL) return EFI_OUT_OF_RESOURCES;
    	
    	RoDp->Header.Type=MEDIA_DEVICE_PATH;
    	RoDp->Header.SubType=MEDIA_RELATIVE_OFFSET_RANGE_DP;
    	SET_NODE_LENGTH(&RoDp->Header, sizeof(MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH));
        //Last parameter indicates that we accessing option ROM in the memory buffer
    	imgsz=(UINT32)DoubleCheckOpRom(Dev, (VOID*)RomHeader, Pcir, FALSE);
    	imgsz-=RomHeader->EfiImageOffset;
        //Get Start..End offsets of Rom File...
        imgb=(VOID*)((UINT8*)RomHeader+RomHeader->EfiImageOffset);
        RoDp->StartingOffset=(UINT64)((UINTN)imgb-(UINTN)Dev->PciIo.RomImage);
        RoDp->EndingOffset=RoDp->StartingOffset+imgsz-1;

        romdata->RomImageDP=(EFI_DEVICE_PATH*)RoDp;
    	
    	//ReadEfiRom Will Uncompress ROM if needed and update imgsz...
        Status=ReadEfiRom(Dev, romdata, &imgb, &imgsz);
        if(EFI_ERROR(Status)) return Status;		
        
    } else {
    	EMB_ROM_INFO	*EmbRomInfo=(EMB_ROM_INFO*)P2;
    //----------	
    	//If we used Embedded UEFI Driver - use Memory Mapped DP
    	MmDp=MallocZ(sizeof(MEMMAP_DEVICE_PATH));
		if (MmDp==NULL) return EFI_OUT_OF_RESOURCES;
   	
		imgb=EmbRomInfo->EmbRom;
		imgsz=(UINT32)EmbRomInfo->EmbRomSize;
		
		MmDp->Header.Type=HARDWARE_DEVICE_PATH;
		MmDp->Header.SubType=HW_MEMMAP_DP;
    	SET_NODE_LENGTH(&MmDp->Header, sizeof(MEMMAP_DEVICE_PATH));

		MmDp->MemoryType=EfiBootServicesCode;
		MmDp->StartingAddress=(EFI_PHYSICAL_ADDRESS)imgb;
		MmDp->EndingAddress=MmDp->StartingAddress+ (EFI_PHYSICAL_ADDRESS)imgsz;

    	romdata->RomImageDP=(EFI_DEVICE_PATH*)MmDp;
    	romdata->EmbDriver=TRUE;
    	
        //Update Capabilities...
        Dev->Capab |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
        Dev->Attrib  |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
    }

    //update Actual File size field..
    romdata->ActualSize=imgsz;

    //Form Rom Image Device Path from Dev->DevicePath + romdata->RomImgDP+DP_END...
    romdp=DPAddNode((VOID*)Dev->DevicePath, (VOID*)romdata->RomImageDP);
    if(romdp==NULL) return EFI_OUT_OF_RESOURCES;
	
    //load image and start image
	Status=pBS->LoadImage(FALSE,gPciBusDriverBinding.ImageHandle,romdp,imgb,imgsz,&romdata->RomImgHandle);
	if (EFI_ERROR(Status)) return Status;

	Status=pBS->StartImage(romdata->RomImgHandle,NULL,NULL);
	if (EFI_ERROR(Status)) return Status;
							
    //Add romdata to the EfiRomList...
    Status=AppendItemLst((T_ITEM_LIST*)&Dev->RomInitCnt, romdata);
    if(EFI_ERROR(Status)) return Status;
    
    //update BusOvr Data...
    if(Dev->BusOvrData.BusOverride.GetDriver==NULL)Dev->BusOvrData.BusOverride.GetDriver=OvrGetDriver;
    
//#if ((defined EFI_SPECIFICATION_VERSION) && (EFI_SPECIFICATION_VERSION >= 0x2001F))
    //Update LoadFile Data
    if(Dev->LoadFileData.LoadFile2.LoadFile==NULL)Dev->LoadFileData.LoadFile2.LoadFile=LoadRomFile;
//#endif

    if(romdp!=NULL)pBS->FreePool(romdp);
    return Status;

}

/**
    This function decide to launch or not UEFI compatible  
    Option ROM

        
    @param Dev PCI Device.
  
    @retval BOOLEAN
        TRUE             		Launch UEFI Option ROM
        FALSE                   NOT Launch UEFI Option ROM
**/
BOOLEAN EFIAPI ExecuteUefiRom(PCI_DEV_INFO *Dev)
{
    EFI_STATUS Status;
    AMI_OPROM_POLICY_PROTOCOL *AmiOpromPolicyProtocol;
    AMI_CSM_OPROM_POLICY_PROTOCOL *AmiOpromPolicyByPciHandleProtocol;

    Status = pBS->LocateProtocol(&gAmiOpromPolicyProtocolGuid, NULL, (VOID**)&AmiOpromPolicyProtocol); 
    if(EFI_ERROR(Status))   //if CSM OptOut is disabled we should always launch UEFI OpROM
        return TRUE;

    Status = pBS->LocateProtocol(&gAmiCsmOpromPolicyProtocolGuid, NULL, (VOID**)&AmiOpromPolicyByPciHandleProtocol);
    if (!EFI_ERROR(Status)) {
        Status = AmiOpromPolicyByPciHandleProtocol->CheckOpromPolicy(AmiOpromPolicyByPciHandleProtocol, Dev->Handle);
        return (EFI_ERROR(Status)) ? FALSE : TRUE;
    }

    Status = AmiOpromPolicyProtocol->CheckUefiOpromPolicy(AmiOpromPolicyProtocol, Dev->Class.BaseClassCode);
    return (EFI_ERROR(Status)) ? FALSE : TRUE;
}


/**
    This function parses Option ROM Image to decide which 
    Image to give control to and updates PciIO->OptioRom and  
    PciIO->OptioRomSize with Correct values.

        
    @param Dev Pointer PCI Devicse Private Data.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_NOT_FOUND           When Device does not have any ROMs.
**/
EFI_STATUS EFIAPI ActivateOptRom(PCI_DEV_INFO *Dev, BOOLEAN Embedded)
{
	UINT8					*cp; 
	UINTN					romc;
	PCI_EFI_OPT_ROM_HEADER	*rh;
	PCI_DATA_STRUCTURE		*pcir; 
	BOOLEAN					ff=TRUE;//, fnd=FALSE;
	BOOLEAN 				efior=FALSE;
	BOOLEAN					notarom=FALSE;
	UINTN					sz=0;  
	INT64					rsz=(INT64)Dev->PciIo.RomSize;
	EFI_STATUS				Status=EFI_SUCCESS;
    BOOLEAN                 RomMatch=FALSE;
//APTIOV_SERVER_OVERRIDE_START
    BOOLEAN                 RomDelayed=FALSE;
//APTIOV_SERVER_OVERRIDE_END
//------------------------
	if( (Dev->PciIo.RomImage == NULL) && !Dev->EmbRomCnt ) return EFI_NOT_FOUND;

	
	if( Dev->PciIo.RomImage != NULL ){
//APTIOV_SERVER_OVERRIDE_START
        if(Embedded){
            if(!((Dev->Capab&EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM) && (Dev->Attrib&EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM))){
                PCI_TRACE((TRACE_PCI,"<Delayed>="));
                Status=AppendItemLst(&gThirdPartyRomList, Dev);
                RomDelayed = TRUE;
            }
        }
        if (RomDelayed == FALSE) {
//APTIOV_SERVER_OVERRIDE_END
		cp=(UINT8*)Dev->PciIo.RomImage;
		for(romc=0;;romc++){
			rsz-=sz;
			if(rsz<=0) return Status;
			cp=cp+sz;
			
			rh=(PCI_EFI_OPT_ROM_HEADER*)cp;
			//Check if a valid signarture here 
			if(ff){//first time here
				if(rh->Signature!=PCI_OPT_ROM_SIG){
					notarom=TRUE;
					break;
				}
				//roms=(UINTN)rh;
			} else {
				//here is the case when we can not find a valid signature for the next image  
				//withing the Option ROM
				if(rh->Signature!=PCI_OPT_ROM_SIG){
					sz=512;
					continue;				
				} 
			}		
			ff=FALSE;
		
			//Check if it is EFI Opt ROM		
			if(rh->EfiSignature==PCI_OPT_ROM_EFISIG) efior=TRUE;
			else efior=FALSE;
	
			//we got what we were looking for...
			pcir=(PCI_DATA_STRUCTURE*)(cp+rh->PcirOffset);
			if(pcir->Signature!=PCI_PCIR_SIG) continue;
	
			//Here is some more conditions to check.
			//some ROMs has Different Sizes filled in 0x55AA ROM Header and in PCIR structure.
			//So check which size is right by looking one 
	
			//PCI Firmware Spec 3.0 defines some reserved field in PCIR Structure as Device List Pointer  
			Status=CheckPciRomDevId(Dev, pcir);
			if(EFI_ERROR(Status)) {
				if(Status==EFI_NOT_FOUND){
					PCI_TRACE((TRACE_PCI," ROM Img #%d Dev.VID!=PCIR.VID ", romc));
				} else PCI_TRACE((TRACE_PCI," ROM Img #%d Dev.DID!=PCIR.DID ", romc));
			} else RomMatch=TRUE;
	 
		   //Check CSM Opt OUT Protocol in what order to starty UEFI OPT ROM.
			if( ExecuteUefiRom(Dev) ) {
				//check conditions under which we must process EFI OptROM
				if( efior && pcir->CodeType == PCI_CODE_TYPE_EFI_IMAGE && RomMatch) 
				{
					PCI_TRACE((TRACE_PCI,"(Install UEFI OpROM="));
					if(Embedded){
					    if((Dev->Capab&EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM) && (Dev->Attrib&EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM)){
					        Status=InstallEfiOpRom(Dev, (VOID*)rh, (VOID*)pcir, FALSE);
//APTIOV_SERVER_OVERRIDE_START
					    } //else {
					      //  PCI_TRACE((TRACE_PCI,"<Delayed>="));
					     //   Status=AppendItemLst(&gThirdPartyRomList, Dev);
					   // }
//APTIOV_SERVER_OVERRIDE_END
					} else {
					    Status=InstallEfiOpRom(Dev, (VOID*)rh, (VOID*)pcir, FALSE);
					}
					PCI_TRACE((TRACE_PCI,"%r)", Status));
				} 
			}
			
			if(pcir->Indicator&0x80) {
				break;
			}
			//else keep circling until we reach the end...
	        //Last parameter indicates that we accessing option ROM in the memory buffer
			sz=DoubleCheckOpRom(Dev,(VOID*)rh, pcir, FALSE);
		}	
//APTIOV_SERVER_OVERRIDE_START        
		}
	} //if Dev->PciIo.RomImage !=0
	if (RomDelayed == FALSE) {
	//Some house keeping...
	    if(notarom || !RomMatch){
	        pBS->FreePool(Dev->PciIo.RomImage);
	        Dev->PciIo.RomImage=NULL;
	        Dev->PciIo.RomSize=0;
	        Status=EFI_NOT_FOUND;
	    } else Status=EFI_SUCCESS;
	}
//APTIOV_SERVER_OVERRIDE_END
	//Now if device has Embedded ROM stored as an UEFI Driver we will launch them as well
	if( Dev->EmbRomCnt){
		UINTN	i;
	//---------------	
		for(i=0;i<Dev->EmbRomCnt; i++){
			EMB_ROM_INFO	*embrom;
		//---------------------	
			embrom=Dev->EmbRoms[i];
			if(embrom->UefiDriver && ExecuteUefiRom(Dev)){
				PCI_TRACE((TRACE_PCI,"(Install UEFI EmbDrv="));
				Status=InstallEfiOpRom(Dev,embrom->EmbRom,(VOID*)embrom, TRUE);
				PCI_TRACE((TRACE_PCI,"%r) ", Status));
			}
		}
	}
	
	return Status;
}

BOOLEAN CheckOptRomStatus(PCI_DEV_INFO *Device, UINTN* LstIndex){
    UINTN           i;
    PCI_DEV_INFO    *dev;
//--------------------
    if(!gReadyToLock) return FALSE;
    for(i=0; i<gThirdPartyRomList.ItemCount; i++){
        dev=(PCI_DEV_INFO*)gThirdPartyRomList.Items[i];
        if(Device==dev) {
            *LstIndex=i;
            return TRUE;
        }
    }
    return FALSE;
}


EFI_STATUS Activate3rdPartyRom(PCI_DEV_INFO *Dev, UINTN Index){
    VOID			*buff[4]={NULL,NULL,NULL,NULL};
    UINTN           offs=0;
    EFI_STATUS      Status;
//------------------    
    PCI_TRACE((TRACE_PCI,"PciBus: Launch3rdPartyRom->Activate="));
    Status=ActivateOptRom(Dev, FALSE);
    PCI_TRACE((TRACE_PCI,"=%r; ", Status));
	if(EFI_ERROR(Status)){
    //filter out conditions where ERROR status does not break an operation.
		if(Status==EFI_NOT_FOUND || Status==EFI_ALREADY_STARTED ) Status=EFI_SUCCESS;
		else if(Status!=EFI_ACCESS_DENIED) return Status;
    }

    //EFI_ACCESS_DENIED status tells that Rom Image failed security check.	
    //Check if Override handle count gets changed..
    //then install Bus Override protocol interface
    if(Status!=EFI_ACCESS_DENIED){
        if(Dev->BusOvrData.BusOverride.GetDriver!=NULL){
            buff[offs] = &gEfiBusSpecificDriverOverrideProtocolGuid;
            buff[offs+1] = &Dev->BusOvrData.BusOverride;
            offs+=2;
        }
    } else Status=EFI_SUCCESS;

    //Install LoadFile2 protocol since we still have image in memory
    //even if Image was not signed correctly...
    if(Dev->LoadFileData.LoadFile2.LoadFile!=NULL){
        buff[offs]=&gEfiLoadFile2ProtocolGuid;
        buff[offs+1]=&Dev->LoadFileData.LoadFile2;
    }

    if(buff[1]!=NULL){
    	Status=pBS->InstallMultipleProtocolInterfaces(
					&Dev->Handle,
					buff[0],buff[1], //BusSpecificDriverOverride GUID - I/F pare
					buff[2],buff[3], //LoadFile2  GUID - I/F pare if present
					NULL, NULL );

	    PCI_TRACE((TRACE_PCI,"->Ins BusOvr LoadFile2=%r\n", Status));

    	if(EFI_ERROR(Status)) return Status;
    }
    
    //Now remove Device from Delayed Rom Launch list...
    DeleteItemLst(&gThirdPartyRomList, Index, FALSE);
    return Status;
}   


/**
    This function performs the following operations
    1. Register Device with CORE by getting a brand new handle
    2. Install PciIo Protocol Interface on This Device
    3. Generates EFI_DEVICE_PPATH protocol instance for "Dev"ice
    4. Installs DevicePath Protocol on Created Handle
    5. If Device has Option ROM copy it to the Memory Buffer
    6. If 5 is TRUE and if Opt ROM has EFI compliant Image 
    installs Bus Override protocol.

        
    @param ControllerHandle Device's Controller Handle.
    @param Dev Pointer PCI Devicse Private Data.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

**/

EFI_STATUS EFIAPI InstallPciDevice(EFI_HANDLE ControllerHandle, PCI_DEV_INFO *Dev)
{
	EFI_STATUS		Status=0;	
    EFI_STATUS      OpRomStatus = EFI_NOT_FOUND;
	BOOLEAN			ndp=FALSE;//NewDevicePath
	VOID			*buff[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	UINTN			offs;
    BOOLEAN         cr=TRUE; //flag to copy ROM image from the card.
//-----------------------------------
	PCI_TRACE((TRACE_PCI, "\n====================================================================\n"));
	PCI_TRACE((TRACE_PCI,"PciBus: Installing PCI IO for B%X|D%X|F%X; ->->->->\n", Dev->Address.Addr.Bus,
			Dev->Address.Addr.Device,Dev->Address.Addr.Function));

	//Check if Device Has Been already started
	if(Dev->Started) {
	    if(CheckOptRomStatus(Dev, &offs)) return Activate3rdPartyRom(Dev,offs);
        Status=EFI_ALREADY_STARTED;
		PCI_TRACE((TRACE_PCI," -> Dev.Started=1; -> %r\n",Status));
        return Status;
	}
    
    offs=0;
	if(Dev->Type==tPciRootBrg) ndp=FALSE;
	else ndp=TRUE;
	
    if(Dev->PciIo.PollMem!=gPciIoInstance.PollMem){
    	pBS->CopyMem(&Dev->PciIo, &gPciIoInstance, sizeof(EFI_PCI_IO_PROTOCOL));
    } else cr=FALSE;
	buff[offs]=&gEfiPciIoProtocolGuid;
	buff[offs+1]=&Dev->PciIo;
	offs+=2;

	//Install Device Path Protocol on newly created Handle
	if(ndp){
		buff[offs]=&gEfiDevicePathProtocolGuid;
		buff[offs+1]=Dev->DevicePath;
		offs+=2;
	}
	
	if(Dev->AmiRomPolicy!=NULL){
        Status=pBS->InstallMultipleProtocolInterfaces(
                        &Dev->Handle,
                        &gAmiOptionRomPolicyProtocolGuid,&Dev->AmiRomPolicy->OptRomProtocol, // GUID - I/F pare
                        NULL, NULL );
        PCI_TRACE((TRACE_PCI,"\nPciBus: Device may have OpROM - ROM Status=%d; Dispatch=%d - %r\n",
                        Dev->AmiRomPolicy->RomStatus, Dev->AmiRomPolicy->CurrentPolicy, Status));
        ASSERT_EFI_ERROR(Status);
	}
	
	Status=pBS->InstallMultipleProtocolInterfaces(
					&Dev->Handle,
					buff[0],buff[1], //PciIO    GUID - I/F pare
					buff[2],buff[3], //DevPath  GUID - I/F pare
					NULL, NULL );
	PCI_TRACE((TRACE_PCI,"\nPciBus: Instaled DP; PCIO Status=%r",Status));
    ASSERT_EFI_ERROR(Status);
   	if(EFI_ERROR(Status)) return Status;
	//Set Started Flag...
	Dev->Started=TRUE;

	//Moved here to support PCI_PLATFORM_PROTOCOL.GetRom functionality...
	//Check if Device has Opton ROM ...
    if(cr){ 
        OpRomStatus=GetOptRom(Dev);
    } else {
        //OpROM was initialized earlier
        if((Dev->PciIo.RomImage != NULL && Dev->PciIo.RomSize != 0)||
           (Dev->BusOvrData.BusOverride.GetDriver != NULL && Dev->LoadFileData.LoadFile2.LoadFile != NULL)){ 
            OpRomStatus = EFI_SUCCESS;
        }        
    }
	
	//update offset it might be ==2 if ndp==FALSE, but we need it as 4 after this point.
	offs=4;

	PCI_TRACE((TRACE_PCI,":GetRom="));

	if(EFI_ERROR(OpRomStatus)){
		Dev->RomBarError=TRUE;
		Dev->PciIo.RomImage=NULL;
		Dev->PciIo.RomSize=0;
       	PCI_TRACE((TRACE_PCI,"%r\n", OpRomStatus));	
	} else {
        //Print Status from GetOptRom()
		PCI_TRACE((TRACE_PCI,"%r :ActivateRom", OpRomStatus));
// APTIOV_SERVER_OVERRIDE_START
		if(cr) {
		    if (!gReadyToLock) {
		        Status=ActivateOptRom(Dev, TRUE);
			}
			else {
			    Status=ActivateOptRom(Dev, FALSE);
			}
		}	
// APTIOV_SERVER_OVERRIDE_END		
		PCI_TRACE((TRACE_PCI,"=%r\n", Status));
		if(EFI_ERROR(Status)){
		    //filter out conditions where ERROR status does not break an operation.
			if(Status==EFI_NOT_FOUND || Status==EFI_ALREADY_STARTED ) Status=EFI_SUCCESS;
			else {
				if(Status!=EFI_ACCESS_DENIED) return Status;
			}
		}
		
        //EFI_ACCESS_DENIED status tells that Rom Image failed security check.	
        //Check if Override handle count gets changed..
		//then install Bus Override protocol interface
		if(Status!=EFI_ACCESS_DENIED){
			if(Dev->BusOvrData.BusOverride.GetDriver!=NULL){
				buff[offs] = &gEfiBusSpecificDriverOverrideProtocolGuid;
				buff[offs+1] = &Dev->BusOvrData.BusOverride;
				offs+=2;
			}
		} else Status=EFI_SUCCESS;

		//Install LoadFile2 protocol since we still have image in memory
		//even if Image was not signed correctly...
		if(Dev->LoadFileData.LoadFile2.LoadFile!=NULL){
			buff[offs]=&gEfiLoadFile2ProtocolGuid;
			buff[offs+1]=&Dev->LoadFileData.LoadFile2;
		}

	}

	//Now Before installing Protocol Interface for THIS device.
    //Call Porting Hook to do OEM Custom Programming of the device.
    //PciIo and all the stuff been updated for this device.
    //But NO notification yet for PciIO installation dispatchetd
    //Status=PciPortOemProgDevice(Dev);
	//PCI_TRACE((TRACE_PCI,":PciBus: Calling PciPortOemProgDevice() >Status=%r\n", Status));
	Status=AmiPciLibLaunchInitRoutine(Dev, isPciProgramDevice, itDevice, Dev, NULL,NULL,NULL);
	if(EFI_ERROR(Status)){
		if(Status==EFI_UNSUPPORTED){
			Status=EFI_SUCCESS;
		} else ASSERT_EFI_ERROR(Status);
	} else {
        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], VID=%X, DID=%X ... OEM PROGRAM DEVICE.\n",
            Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function,
            Dev->DevVenId.VenId, Dev->DevVenId.DevId));
	}

    if(buff[5]!=NULL){
    	Status=pBS->InstallMultipleProtocolInterfaces(
					&Dev->Handle,
					buff[4],buff[5], //BusSpecificDriverOverride GUID - I/F pare
					buff[6],buff[7], //LoadFile2  GUID - I/F pare if present
					NULL, NULL );

	    PCI_TRACE((TRACE_PCI," -> Installing BusOvr and LoadFile2 -> %r\n", Status));

    	if(EFI_ERROR(Status)) return Status;
    }

	if(Dev->Type>tPciRootBrg){//Don't Open Rb Protocol for itself
		Status=pBS->OpenProtocol(
					ControllerHandle, &gEfiPciRootBridgeIoProtocolGuid,
					(VOID**)&Dev->RbIo, gPciBusDriverBinding.DriverBindingHandle,
					Dev->Handle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
	}

	return Status;
}

/**
    Starts PCI Devices Behind the Bridge. (Called recoursively)
    This function performs the following operations
    1. Register Device with CORE by getting a brand new handle
    2. Install PciIo Protocol Interface on This Device
    3. Generates EFI_DEVICE_PPATH protocol instance for "Dev"ice
    4. Installs DevicePath Protocol on Created Handle
    5. If Device has Option ROM copy it to the Memory Buffer
    6. If 5 is TRUE and if Opt ROM has EFI compliant Image 
    installs Bus Override protocol.

        
    @param ControllerHandle Device's Controller Handle.
    @param Brg Pointer at PCI Bridge Private Data.
    @param RemainingDp Pointer at PCI Device Path
        Protocol Instance or NULL.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

**/
EFI_STATUS EFIAPI StartPciChildDevice(EFI_HANDLE ControllerHandle,PCI_BRG_INFO *Brg, PCI_DEVICE_PATH *RemainingDp)
{
	PCI_DEV_INFO				*dev;
	EFI_STATUS					Status = EFI_NOT_FOUND;
	UINTN						i;
	PCI_DEVICE_PATH				*pcidp=RemainingDp;
    BOOLEAN                     Started=FALSE, Success=FALSE;
//-------------------------------
	
	PCI_TRACE((TRACE_PCI, "PciBus: StartPciChildDevice -> Bridge has %d Child Devices \n",Brg->Bridge.ChildCount));

    //Check if Device Path is a valid PCI Device Path
    if (pcidp!=NULL){
		//Check if remaining device path is a valid PCI device path
	    if( EFI_ERROR(CheckPciDevicePath(&pcidp->Header)) ) return EFI_INVALID_PARAMETER;
        //If remaining device path is provided, we must find the device.
    }

	if(!Brg->Bridge.ChildCount)	return Status;

	for(i=0; i<Brg->Bridge.ChildCount; i++){
		dev=Brg->Bridge.ChildList[i];
        if(dev->OutOfResRemove) continue;
		//if Remaining Device Path provided start only specified device 
		if(pcidp) 
		{	
		    if(pcidp->Device != dev->Address.Addr.Device ||
			   pcidp->Function != dev->Address.Addr.Function
            ) continue;
			// The Device Path of this Device matches provided
			Status=InstallPciDevice(ControllerHandle, dev); 
			if (EFI_ERROR(Status)){
                if(Status==EFI_ALREADY_STARTED) Started=TRUE;
                else return Status;
            } else Success=TRUE;

			//if Device is P2P bridge than BDS might like to start next DP node
			//So recousively call this function to find desired Device
            pcidp++;
			if(dev->Type==tPci2PciBrg && !isEndNode(&pcidp->Header))
				Status=StartPciChildDevice(ControllerHandle,(PCI_BRG_INFO*)dev,pcidp);


            //if(!EFI_ERROR(Status) && Status == EFI_NOT_FOUND){
            //    if(Started && !Success) Status=EFI_ALREADY_STARTED;
            //}    
	        return Status;
		} 
		else //if no Remaining Device Path provided start all devices
		{
			Status=InstallPciDevice(ControllerHandle, dev); 
			if (EFI_ERROR(Status)){
                if(Status==EFI_ALREADY_STARTED){
                    Started=TRUE;
                    Status=EFI_SUCCESS;
                } else return Status;
            } else Success=TRUE;

			//if Device is P2P bridge recousively call this function 
			if(dev->Type==tPci2PciBrg){  
				Status=StartPciChildDevice(ControllerHandle,(PCI_BRG_INFO*)dev,pcidp);
				if(EFI_ERROR(Status)){
                    //StartPciChildDevice() could return EFI_ALREADY_STARTED or EFI_NOT_FOUND
                    //this is if we have started this device already, of Bridge don't have any 
                    //childs behind it. This is normal and we shouldn't break the loop.
                    //All other ERROR Statuses suppose to break the loop and raise an ERROR.
                    if(Status==EFI_ALREADY_STARTED || Status==EFI_NOT_FOUND){
                        Started=TRUE;
                        Status=EFI_SUCCESS;
                    } else return Status;
                } else Success=TRUE;
			}  	
		}
	}//for

    if(!EFI_ERROR(Status)){
        if(Started && !Success) Status=EFI_ALREADY_STARTED;
    }    
	return Status;
}

/**
    Installs PCI Io Protocol Instance on all or some PCI Devices
    in the System

        
    @param ControllerHandle Device's Controller Handle.
    @param RemainingDevicePath Pointer at PCI Device Path
        Protocol Instance or NULL.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

**/
EFI_STATUS EFIAPI StartPciDevices(IN EFI_HANDLE ControllerHandle, IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath)
{
	EFI_STATUS			Status = EFI_DEVICE_ERROR;
	UINTN				i, j;
	PCI_DEV_INFO		*rbrg;
	PCI_HOST_INFO		*lhst; //local host
    EFI_DEVICE_PATH_PROTOCOL *rdp=RemainingDevicePath;
//---------------------------------------------	
	PCI_TRACE((TRACE_PCI, "\n====================================================================\n"));
	PCI_TRACE((TRACE_PCI, "PciBus: StartPciDevices Called -> hCtrl=0x%X; RemainingDp=0x%X;\n",ControllerHandle,RemainingDevicePath));

	//Check if RemainingDevicePath is valid
	if(rdp!=NULL){

        if (isEndNode(rdp)) {
            PCI_TRACE((TRACE_PCI, "PciBus: RemainingDp==EndOfDpNode\n"));
            return EFI_SUCCESS;
        } else {
		    Status=CheckPciDevicePath(rdp);
		    PCI_TRACE((TRACE_PCI, "PciBus: START CheckPciDp -> %r\n",Status));
    		if(EFI_ERROR(Status)) goto ExitLbl;
        }
	}
	
	//find Root Bridge first
	for(j=0; j<gHostCnt; j++){
		lhst=gPciHostList[j];
		for(i=0; i<lhst->RbCount; i++){
			rbrg=lhst->RootBridges[i];

			if(rbrg->Handle==ControllerHandle){ 

    			Status=pBS->OpenProtocol(rbrg->Handle,
				    &gEfiPciRootBridgeIoProtocolGuid,(VOID **)&rbrg->RbIo, 
				    gPciBusDriverBinding.DriverBindingHandle,rbrg->Handle,
				    EFI_OPEN_PROTOCOL_BY_DRIVER );

                if(EFI_ERROR(Status)){
                    if( Status!=EFI_ALREADY_STARTED ) return Status;
                    else Status=EFI_SUCCESS;
                }  

				Status=StartPciChildDevice(ControllerHandle,(PCI_BRG_INFO*)rbrg,(PCI_DEVICE_PATH*)rdp);
				break;
			}
		}
	}
ExitLbl:

	PCI_TRACE((TRACE_PCI, "PciBus: StartPciChildDevice -> %r\n",Status));
	PCI_TRACE((TRACE_PCI, "====================================================================\n"));

	return Status;
}

EFI_STATUS EFIAPI LocateDevHandle(PCI_DEV_INFO *Dev, T_ITEM_LIST *PciHnd){
    UINTN       i;  
    EFI_HANDLE  h;
//------------------------
    for(i=0; i<PciHnd->ItemCount; i++){
        h=*(EFI_HANDLE*)(PciHnd->Items[i]);
        if(Dev->Handle==h){
            DeleteItemLst(PciHnd, i, FALSE);
            Dev->Handle=NULL;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
    Stops PCI Device referenced as "Dev". Called From 
    StopPciDeviceBrg() function.

        
    @param Dev Pointer PCI Devicse Private Data.
    @param Controller Device's Controller Handle.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

**/
EFI_STATUS EFIAPI StopPciDevice(PCI_DEV_INFO *Dev,EFI_HANDLE Controller, T_ITEM_LIST *HndDb)
{
	EFI_STATUS 				Status=EFI_SUCCESS;
	UINTN					offs=0;
    BOOLEAN                 ndp;
#if ((defined EFI_SPECIFICATION_VERSION) && (EFI_SPECIFICATION_VERSION >= 0x2001F))
    VOID	*buff[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
#else
    VOID    *buff[6]={NULL,NULL,NULL,NULL,NULL,NULL};
#endif
//--------------------------------------------------------------------

    //Check if Device Has been Stopped already... 
    if(Dev->Started==FALSE) return EFI_SUCCESS;

    if(Dev->Type==tPciRootBrg) {
        Dev->Started=FALSE;
        return EFI_SUCCESS;
    }
	
   	PCI_TRACE((TRACE_PCI,"\nPciBusStop: Hnd=%X [B%X|D%X|F%X]->",Dev->Handle,
            Dev->Address.Addr.Bus,Dev->Address.Addr.Device, Dev->Address.Addr.Function));
	//Close Root Brg Protocol we've opened at the beginning.
    //Avoid Closing Protocol for RootBridge Device - we didn't open it.
    if(Dev->Handle!=Controller){ 
	    Status=pBS->CloseProtocol(Controller,&gEfiPciRootBridgeIoProtocolGuid,
			    gPciBusDriverBinding.DriverBindingHandle, Dev->Handle);
        ndp=TRUE;
        PCI_TRACE((TRACE_PCI,"Close(RbIo)=%r;\n",Status));    
    } else {
        ndp=FALSE;
    }

    ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
    
	buff[offs]=&gEfiPciIoProtocolGuid;
	buff[offs+1]=&Dev->PciIo;
	offs+=2;

    //Same for Device Path: we did'not create device path for RootBridge device
    if(ndp){
    	buff[offs]=&gEfiDevicePathProtocolGuid;
	    buff[offs+1]=Dev->DevicePath;
	    offs+=2;
	}
	
	//Now take care of BusSpecific Override	Protocol
	//If it was installed
	if(Dev->BusOvrData.BusOverride.GetDriver!=NULL){
		buff[offs]=&  gEfiBusSpecificDriverOverrideProtocolGuid;
		buff[offs+1]=&Dev->BusOvrData.BusOverride;
	}

#if ((defined EFI_SPECIFICATION_VERSION) && (EFI_SPECIFICATION_VERSION >= 0x2001F))
    //LoadFile2 Protocol
	offs+=2;
    if(Dev->LoadFileData.LoadFile2.LoadFile!=NULL){
        buff[offs]=&gEfiLoadFile2ProtocolGuid;
        buff[offs+1]=&Dev->LoadFileData.LoadFile2;
    }
#endif
	Status=pBS->UninstallMultipleProtocolInterfaces(
					Dev->Handle,
					buff[0],buff[1], //PciIO    GUID - I/F pare
					buff[2],buff[3], //DevPath  GUID - I/F pare
					buff[4],buff[5], //BusOwerride  GUID - I/F pare if present
#if ((defined EFI_SPECIFICATION_VERSION) && (EFI_SPECIFICATION_VERSION >= 0x2001F))
					buff[6],buff[7], //LoadFile2  GUID - I/F pare if present
#endif
					NULL, NULL );
    
   	PCI_TRACE((TRACE_PCI,"\nPciBusStop: Hnd=%X [B%X|D%X|F%X]->",Dev->Handle,
            Dev->Address.Addr.Bus,Dev->Address.Addr.Device, Dev->Address.Addr.Function));
    PCI_TRACE((TRACE_PCI,"Uninst(PciIo,DP,BusOvr)=%r;\n",Status));    
//DEBUG????
	if(!EFI_ERROR(Status)){
//DEBUG????
        Dev->Started=FALSE;
        if(HndDb!=NULL) Status=LocateDevHandle(Dev, HndDb);
        Dev->Handle=NULL;
//DEBUG????
    }
//DEBUG????
    ASSERT_EFI_ERROR(Status);
	return Status;
}	


/**
    Stops - Uninstalls PCI Io Protocol Instance from PCI Devices
    behind the PCI Bridge referenced as "Brg"

        
    @param Brg Pointer PCI Devicse Private Data.
    @param Controller Device's Controller (Parent) Handle.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

**/
EFI_STATUS EFIAPI StopPciDeviceBrg(PCI_BRG_INFO *Brg, EFI_HANDLE Controller, T_ITEM_LIST   *HndDb)	
{
	PCI_DEV_INFO    *dev;
	UINTN           i;
	EFI_STATUS      Status;
//--------------------------------------------------------------------
	for(i=0; i<Brg->Bridge.ChildCount; i++){
		dev=Brg->Bridge.ChildList[i];
		//recoursive calling ourselfs 
		if(dev->Type==tPci2PciBrg) Status=StopPciDeviceBrg((PCI_BRG_INFO*)dev,Controller, HndDb);
        else Status=StopPciDevice(dev, Controller, HndDb);
        ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
	}
	//After we done with the childs - Stop this Bridge itself
	Status=StopPciDevice(&Brg->Common, Controller, HndDb);
    ASSERT_EFI_ERROR(Status);

	return Status;
}	


//==============================================================================
//
// EFI Drver Binding Protocol Functions
//
//==============================================================================

/**
    Supported Function of the EFI_DRIVER_BINDING_PROTOCOL
    for PCI Bus Driver.

    @note  
  See EFI Specification for detail description
**/
EFI_STATUS EFIAPI PciBusSupported(IN EFI_DRIVER_BINDING_PROTOCOL    *This,
						   IN EFI_HANDLE                     Controller,
						   IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath )
{
	EFI_STATUS                       Status;
	EFI_DEVICE_PATH_PROTOCOL         *ParentDevicePath;
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo;
    EFI_DEVICE_PATH_PROTOCOL         *rdp=RemainingDevicePath;
//-----------------------------------------------
    
    
	//Check if it is valid Device Path 
	if (rdp != NULL){
        if (isEndNode(rdp)) rdp=NULL;
        else {   
		    Status=CheckPciDevicePath(rdp);
		    if (EFI_ERROR (Status))  return Status;
        }
	}

	//Open the IO Abstraction(s) needed to perform the supported test
	Status=pBS->OpenProtocol(Controller,
							&gEfiDevicePathProtocolGuid,
							(VOID **)&ParentDevicePath,
							This->DriverBindingHandle,     
							Controller,   
							EFI_OPEN_PROTOCOL_BY_DRIVER	);

	if (Status == EFI_ALREADY_STARTED)return EFI_SUCCESS;
	if (EFI_ERROR (Status))  return Status;
 
	pBS->CloseProtocol(Controller,&gEfiDevicePathProtocolGuid,This->DriverBindingHandle,
					 Controller);

	Status=pBS->OpenProtocol( Controller,
							  &gEfiPciRootBridgeIoProtocolGuid,
							  (VOID **)&PciRootBridgeIo,
							  This->DriverBindingHandle,
							  Controller,
							  EFI_OPEN_PROTOCOL_BY_DRIVER );

	if(Status==EFI_ALREADY_STARTED) return EFI_SUCCESS;
  
	pBS->CloseProtocol(Controller,&gEfiPciRootBridgeIoProtocolGuid,This->DriverBindingHandle,
						 Controller);

	return Status;
}

#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
/**
    Function collect all device's sdl data that have HasSetup flag set.  

    @note  
  See EFI Specification for detail description
**/
EFI_STATUS EFIAPI UpdateSdlSetupItems(T_ITEM_LIST	*PciSetupItems){
	UINTN					i;
	AMI_SDL_PCI_DEV_INFO	*sdlinfo;
	EFI_STATUS				Status=EFI_SUCCESS;	
//--------------------
	for(i=0;i<gSdlPciData->PciDevCount; i++){
		sdlinfo=(AMI_SDL_PCI_DEV_INFO*)&gSdlPciData->PciDevices[i];
		if(sdlinfo->PciDevFlags.Bits.HasSetup) {
			Status=AppendItemLst(PciSetupItems,sdlinfo);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
		}
	}
	return Status;
}


VOID EFIAPI CreateAllPciVars(T_ITEM_LIST	*PciSetupItems){
	UINTN					i,x;
	EFI_STATUS				Status;
	PCI_SETUP_DATA	  		pcisetup;
	AMI_SDL_PCI_DEV_INFO	*sdl;
//--------------------	
	for(i=0; i<PciSetupItems->ItemCount; i++){
		sdl=PciSetupItems->Items[i];
		Status=AmiSdlFindRecordIndex(sdl,&x);
		ASSERT_EFI_ERROR(Status);
		
		//this will return already existed VARs or create missing one 
		AmiPciGetSetupData(NULL,
				&pcisetup.PciDevSettings,
				&pcisetup.Pcie1Settings,
				&pcisetup.Pcie2Settings,
				sdl,x);
		
		//for HotPlug slightly different approach.
		//it was updated during Host/Root initialization		
	}
}


/**
    If empty slot with setup data found it will check and create if 
    needed corresponded Setup varstor NVRAM variable.

    @note  
  See EFI Specification for detail description
**/
/*
VOID EFIAPI CheckUpdateSetupVar(PCI_DEV_INFO *Bridge, AMI_SDL_PCI_DEV_INFO *SlotSdlData){
	UINTN			slotidx;
	EFI_STATUS		Status;	
	
//----------------------------
	Status=AmiSdlFindRecordIndex(SlotSdlData, &slotidx);
	ASSERT_EFI_ERROR(Status);
	
	Status=AmiPciGetPciDevSetupData(&Bridge->DevSetup,SlotSdlData, slotidx,FALSE);
	
	if(Bridge->PciExpress!=NULL){
		Status=AmiPciGetPcie1SetupData(&Bridge->PciExpress->Pcie1Setup,SlotSdlData, slotidx,FALSE);
		if(Bridge->PciExpress->Pcie2!=NULL){
			Status=AmiPciGetPcie2SetupData(&Bridge->PciExpress->Pcie2->Pcie2Setup,SlotSdlData, slotidx,FALSE);
		}
	}
	ASSERT_EFI_ERROR(Status);

}


VOID EFIAPI EFIAPI CheckEmptySetupSlot(PCI_DEV_INFO *Device){
	UINTN					i;
	AMI_SDL_PCI_DEV_INFO	*sdlinfo;
//------------------------------------
	for(i=0; i<gSdlSetup.ItemCount; i++){
		sdlinfo=gSdlSetup.Items[i];
		if(Device->SdlDevCount==1){
			if(sdlinfo->ParentIndex==Device->SdlDevIndex){
				CheckUpdateSetupVar(Device, sdlinfo);
				break;
			}
		} else {
			UINTN	j;
		//-----------------------	
			for(j=0; j<Device->SdlDevCount; j++){
				if(sdlinfo->ParentIndex==Device->SdlIdxArray[j]){
					CheckUpdateSetupVar(Device, sdlinfo);
					break;
				}
			}//for
		}
		
	}
}
*/
#endif


/**
    Start Function of the EFI_DRIVER_BINDING_PROTOCOL
    for PCI Bus Driver

    @note  
  See EFI Specification for detail description
**/
EFI_STATUS EFIAPI EFIAPI PciBusStart(IN EFI_DRIVER_BINDING_PROTOCOL  *This,
					   IN EFI_HANDLE                   Controller,
					   IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath )
{
	EFI_STATUS          Status;
    BOOLEAN             ft=FALSE;
//------------------------------------------------
    PROGRESS_CODE(DXE_PCI_BUS_BEGIN);

    PCI_TRACE((TRACE_PCI,"\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    PCI_TRACE((TRACE_PCI,"PciBus: ...STARTING... PCI Driver Version %X.%d.%d\n", PCI_BUS_MAJOR_VER, PCI_BUS_MINOR_VER, PCI_BUS_REVISION));
    PCI_TRACE((TRACE_PCI,"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    
    
    
	PCI_TRACE((TRACE_PCI,"\n"));
	//Locate Boot Script protocol we will need it to record Extd PCIExpress Config Space Programing
    if(gS3SaveState==NULL){
    	Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL, (VOID**)&gS3SaveState); 
        //No boot script Could Happened in ARM projects!
	    if(EFI_ERROR(Status)) {
		    PCI_TRACE((TRACE_PCI,"PciBus: Unable to find EfiBootScriptSave Protocol! Status=%r\n       !!!WARNING!!! S3BootSript will NOT record!!!\n", Status));
		    gS3SaveState=NULL;
	    }
    }


    //Locate Ami Board Info 2 Protocol
    if(gAmiBoardInfo2Protocol==NULL){
        Status=AmiSdlInitBoardInfo();
	    if(EFI_ERROR(Status)) {
		    PCI_TRACE((TRACE_PCI,"PciBus: Unable to find AMI Board Info Protocol! Status=%r\n", Status));
		    ASSERT_EFI_ERROR(Status);
		    return Status;
	    }
	    GetRomPolicyVars();
        ft=TRUE;
    }

    if(gPciInitProtocol==NULL){
    	//Get AMI BoardINit Protocol to call initialization functions of PCI Devices
    	Status=pBS->LocateProtocol( &gAmiBoardPciInitProtocolGuid,	//*Protocol,
    								NULL,							//*Registration OPTIONAL,
    								(VOID**)&gPciInitProtocol);		//*Interface 

    	//It must be here otherwise it will NO INIT functions for all Pci Devices.
    	PCI_TRACE((TRACE_PCI,"PciBus: Locate AmiBoardPciInitProtocol............Status=%r.\n", Status));
    	//ASSERT_EFI_ERROR(Status);
    }
    
    if(gPciPortProtocol==NULL){
    	//Get Aptio 4.x Compatinility Protocol to call initialization functions of PCI Devices
    	Status=pBS->LocateProtocol( &gAmiPciPortCompatibilityProtocolGuid,	//*Protocol,
    								NULL,									//*Registration OPTIONAL,
    								(VOID**)&gPciPortProtocol);				//*Interface 

    	//It must be here otherwise it will NO INIT functions for all Pci Devices.
    	PCI_TRACE((TRACE_PCI,"PciBus: Locate AmiPciPortCompatibilityProtocol....Status=%r.\n", Status));
    	//ASSERT_EFI_ERROR(Status);
    }
    
    if(gSdlPciData->PciDevCount==0) gNoSdlDataMode=TRUE;
    
    //Create Pci Setup Data Buffer;
    if(gPciCommonSetup==NULL){
    	
        gPciDefaultSetup=MallocZ(sizeof(PCI_SETUP_DATA));
        if (gPciDefaultSetup==NULL) return EFI_OUT_OF_RESOURCES;

        gPciCommonSetup=MallocZ(sizeof(PCI_COMMON_SETUP_DATA));
        if (gPciCommonSetup==NULL) return EFI_OUT_OF_RESOURCES;

        //Call Library function from PciBusLib to get setup data or Defaults
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)			
        Status=AmiPciGetSetupData(gPciCommonSetup,
        						  &gPciDefaultSetup->PciDevSettings, 
        						  &gPciDefaultSetup->Pcie1Settings,
        						  &gPciDefaultSetup->Pcie2Settings,
        						  NULL, 0);
        if(gNoSdlDataMode==FALSE){
            Status=UpdateSdlSetupItems(&gSdlSetup);
            ASSERT_EFI_ERROR(Status);
            //create missing setup VARs.
            CreateAllPciVars(&gSdlSetup);
        }
#else
        Status=AmiPciGetSetupData(gPciDefaultSetup,
								  gPciCommonSetup,
        						  NULL);

#endif
        PCI_TRACE((TRACE_PCI,"PciBus: Updating Default Setup Data...............Status=%r.\n", Status));
        ASSERT_EFI_ERROR(Status);
        
    }

	// Enumerate the entire host bridge
	Status = PciEnumerateAll(Controller);
    if(ft && (gNoSdlDataMode==FALSE)){
    	Status=CreateIrqTables();
    } 

    ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR (Status)) return Status;
 
    //If Enumeration is OK install Extended PCI BUS Protocol Interface NOW.
    //Only if we have not done it before. It is only single instance on PciBusExt possible.
    if(gAmiExtPciBusProtocol.PciExtHanle==NULL){
        Status=pBS->InstallMultipleProtocolInterfaces( &gAmiExtPciBusProtocol.PciExtHanle,
                               &gAmiExtPciBusProtocolGuid, &gAmiExtPciBusProtocol, 
							   &gEfiPciEnumerationCompleteProtocolGuid, NULL,
							   NULL);
        PCI_TRACE((TRACE_PCI,"PciBus: Installing PciEnumerationCompleteGuid.....Status=%r.\n", Status));
    }

    ASSERT_EFI_ERROR(Status);
 
	// Enable PCI device specified by remaining device path. BDS or other driver can call the
	// start more than once.
    PCI_TRACE((TRACE_PCI,"PciBus: Entering StartPciDevices(Ctlr=0x%X, RemainingDP=0x%X).\n",Controller,RemainingDevicePath));
	Status=StartPciDevices (Controller, RemainingDevicePath);
    PCI_TRACE((TRACE_PCI,"PciBus: Returned from StartPciDevices()...........Status=%r.\n", Status));
    
	return Status;
}

/**
    Stop Function of the EFI_DRIVER_BINDING_PROTOCOL
    for PCI Bus Driver.

    @note  
  See EFI Specification for detail description
**/
EFI_STATUS EFIAPI PciBusStop(IN EFI_DRIVER_BINDING_PROTOCOL  *This,
					  IN EFI_HANDLE                   Controller,
					  IN UINTN                        NumberOfChildren,
					  IN EFI_HANDLE                   *ChildHandleBuffer)
{
	EFI_STATUS         	Status=0;
	PCI_DEV_INFO		*dev=NULL;
	UINTN				i; 
    T_ITEM_LIST         PciHndDb={0,0,NULL};
    EFI_HANDLE          hnd;
//------------------------------------------------
    PCI_TRACE((TRACE_PCI,"\nPciBusStop: NumberOfChildren=0x%X, Controller=0x%X\n",NumberOfChildren, Controller));

	if (NumberOfChildren == 0) {
		// Close the bus driver
		//mST->BootServices->CloseProtocol(Controller,&gEfiDevicePathProtocolGuid,
		//	This->DriverBindingHandle, Controller);
		dev=FindPciDeviceByHandle(Controller);
		if(dev)	Status=StopPciDeviceBrg((PCI_BRG_INFO*)dev, Controller, NULL);
 		else Status=EFI_NOT_FOUND;	
		ASSERT_EFI_ERROR(Status);
        //Close PCI Rooty Bridge Protocol we opened BY_DRIVER
		pBS->CloseProtocol(Controller,&gEfiPciRootBridgeIoProtocolGuid,
			This->DriverBindingHandle,Controller);
	} else {
        //Populate PciHandle Database we need to stop. If we will enter recoursive 
        //call we need to rtemove stopped handles from DB inside recoursive call 
        //to avoid getting EFI_INVALID_PARAMETER trying to Stop already stopped devices.
		for(i=0; i<NumberOfChildren; i++){
            AppendItemLst(&PciHndDb, &ChildHandleBuffer[i]);
            PCI_TRACE((TRACE_PCI,"Hnd #%X: %X \n", i, ChildHandleBuffer[i]));
        }

        //Stopping all the childs 
        while(PciHndDb.ItemCount){
            hnd=*(EFI_HANDLE*)(PciHndDb.Items[0]);
            dev=FindPciDeviceByHandle(hnd);
   		    if(dev){
                //If device a bridge stop all its childs first 
                if(dev->Type==tPci2PciBrg)Status=StopPciDeviceBrg((PCI_BRG_INFO*)dev,Controller, &PciHndDb);		
                else Status=StopPciDevice(dev,Controller,&PciHndDb);
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status))return Status;
            } else {
                Status=EFI_NOT_FOUND;
                break;
            }
            ASSERT_EFI_ERROR(Status);
        }//while...
	}

    if(gAmiExtPciBusProtocol.PciExtHanle!=NULL){
        Status=pBS->UninstallMultipleProtocolInterfaces( gAmiExtPciBusProtocol.PciExtHanle,
                               &gAmiExtPciBusProtocolGuid, &gAmiExtPciBusProtocol, NULL);
        if (!EFI_ERROR(Status)) {
            gAmiExtPciBusProtocol.PciExtHanle=NULL;
        }
    }

    //Cleanup used memory...
	ClearItemLst(&PciHndDb, FALSE);
	return Status;
}


//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
// PCI BUS Extended Protocol Function Implementation

/**
    Locates PCI_DEV_INFO using passed PCI Device Handle or PciIo
    Protocol pointer.
        
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
  
    @retval 
        PCI_DEV_INFO*           When everything is going on fine!
        NULL                    If no such device exists.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
**/
PCI_DEV_INFO*  EFIAPI PciExtGetDevice(EFI_HANDLE PciDeviceHandle, EFI_PCI_IO_PROTOCOL *PciIo){
	PCI_DEV_INFO		*dev;
//------------------------------------        
    if(PciDeviceHandle == NULL){
        if(PciIo!=NULL){
            dev=(PCI_DEV_INFO*)PciIo;
            if(dev->Signature != AMI_PCI_SIG) return NULL;
        } else return NULL;
    } else dev=FindPciDeviceByHandle(PciDeviceHandle);
    
    return dev;
}


/**
    Checks if PciDeviceHandle, or PciIO passed belongs to 
    PCI Express device.
        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param PciExpData Double Pointer to the PCIE_DATA Structure (OPTIONAL)
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If Device is not PCI Express device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
  **PciExpData if not needed must be NULL;
**/
EFI_STATUS EFIAPI PciExtIsPciExpresss(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIE_DATA                                       **PciExpData    OPTIONAL
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev)	{
        if(PcieCheckPcieCompatible(dev)){
            //if we have to update pointer
            if(PciExpData != NULL){
                *PciExpData = dev->PciExpress;
            } 
            return EFI_SUCCESS;
        } else return EFI_NOT_FOUND; 
    } else return EFI_INVALID_PARAMETER;
}

/**
    Checks if PciDeviceHandle, or PciIO passed belongs to 
    PCI Express device.
        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param PciXData Double Pointer to the PCIX_DATA Structure (OPTIONAL)
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If Device is not PCI X device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
  **PciXData if not needed must be NULL;
**/
EFI_STATUS EFIAPI PciExtIsPciX(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCIX_DATA                                       **PciXData      OPTIONAL
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev)	{
        if(dev->PciX != NULL){
            //if we have to update pointer
            if(PciXData != NULL){
                *PciXData = dev->PciX;
            } 
            return EFI_SUCCESS;
        } else return EFI_NOT_FOUND; 
    } else return EFI_INVALID_PARAMETER;
}

/**
    Checks if PciDeviceHandle, or PciIO passed belongs to 
    PCI 2 PCI Bridge device.
        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param PciBrgExt Double Pointer to the PCI_BRG_EXT Structure (OPTIONAL)
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If Device is not PCI 2 PCI Bridge device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
  **PciBrgExt if not needed must be NULL;
**/
EFI_STATUS EFIAPI PciExtIsPciBrg(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev)	{
        if(dev->Type == tPci2PciBrg){
            //if we have to update pointer
            if(BrgData != NULL){
                *BrgData = (PCI_BRG_EXT*)(dev+1);
            } 
            return EFI_SUCCESS;
        } else return EFI_NOT_FOUND; 
    } else return EFI_INVALID_PARAMETER;
}

/**
    Checks if PciDeviceHandle, or PciIO passed belongs to 
    PCI 2 Card Bridge device.
        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param PciBrgExt Double Pointer to the PCI_BRG_EXT Structure (OPTIONAL)
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If Device is not PCI 2 Card Bridge device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
  **PciBrgExt if not needed must be NULL;
**/
EFI_STATUS EFIAPI PciExtIsCrdBrg(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		    OPTIONAL,
    OUT PCI_BRG_EXT                                     **BrgData       OPTIONAL
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev)	{
        if(dev->Type == tPci2CrdBrg){
            //if we have to update pointer
            if(BrgData != NULL){
                *BrgData = (PCI_BRG_EXT*)(dev+1);
            } 
            return EFI_SUCCESS;
        } else return EFI_NOT_FOUND; 
    } else return EFI_INVALID_PARAMETER;
}

/**
    Checks if PciDeviceHandle, or PciIO passed belongs to 
    Regular PCI device.
        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If Device is not Regular PCI device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
**/
EFI_STATUS EFIAPI PciExtIsDevice(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo          OPTIONAL
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev){
        if(dev->Type == tPciDevice) return EFI_SUCCESS;
        else return EFI_NOT_FOUND; 
    } else return EFI_INVALID_PARAMETER;
}


/**
    Returns device's Class code; Sub Class code; Programing Interface
    and Revision Id information.

        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param CassCodes Pointer to the PCI_DEV_CLASS to fill.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
**/
EFI_STATUS EFIAPI PciExtClassCodes(
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL								*PciIo 		OPTIONAL,
	OUT PCI_DEV_CLASS									*CassCodes 
)
{
	PCI_DEV_INFO		*dev;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev){
        *CassCodes=dev->Class;
        return EFI_SUCCESS;
    } else return EFI_INVALID_PARAMETER;
}


/**
    Returns device's corresponded PIC IRQ routing entry from OEMPIR.INC
    table created by the BUILD process.
    If function returns EFI_SUCCESS the "PicIrqTblEntry" updated with corresponded PIC 
    IRQ Table entry pointer. "ParentDevices" == NULL and "EntryCount"==0
    Function will return EFI_NOT_FOUND if there a no entry for the Device HANDLE passed.
 
    But it will return update the array of EFI_PCI_CONFIGURATION_ADDRESS structures with
    PCI Addresses of parenti 

        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param PicIrqTblEntry Double pointer to the corresponded PCI_IRQ_PIC_ROUTE entry
    @param ParentDevices Pointer to an array of parent Devices 
        for which was not found a valid PCI_IRQ_PIC_ROUTE entry.
    @param EntryCount Number of elements in an array above.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If no PIRQ entry was found for this device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
**/
EFI_STATUS EFIAPI PciExtPicIrqRout (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL						 		*PciIo 		OPTIONAL,
    OUT PCI_IRQ_PIC_ROUTE                               **PicIrqTblEntry,
    OUT EFI_PCI_CONFIGURATION_ADDRESS                   **ParentDevices,
    OUT UINTN                                           *EntryCount )
{
    EFI_STATUS          Status;
	PCI_DEV_INFO		*dev;
    PCI_DEV_INFO        *parent;
    T_ITEM_LIST         ParentLst={0,0,NULL};
    INTN                i,j;
    EFI_PCI_CONFIGURATION_ADDRESS   *pa=NULL;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev != NULL ){
        if( dev->PicIrqEntry != NULL ){
            *PicIrqTblEntry = dev->PicIrqEntry;
            ParentDevices = NULL;
            EntryCount = 0;
            return EFI_SUCCESS;
        } else {
            parent=dev;
            do{
                parent=parent->ParentBrg; 
                Status=AppendItemLst(&ParentLst, parent);
				//The only bad Status here could be an EFI_OUT_OF_RESOURCES...
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status))return Status;
            }while( (parent->PicIrqEntry == NULL) || (parent->Type != tPciRootBrg));
            
            if( parent->PicIrqEntry != NULL ){
                *PicIrqTblEntry=parent->PicIrqEntry;
                //Create the array of addresses in order from Device which has IRQ endtry down to this Device parent
                if(ParentLst.ItemCount){    
                    pa=Malloc(sizeof(EFI_PCI_CONFIGURATION_ADDRESS)*(ParentLst.ItemCount));
                    for(i=(INTN)ParentLst.ItemCount-1, j=0; i<=0; i--,j++){
                        parent = (PCI_DEV_INFO*)ParentLst.Items[i];
                        pa[j] = parent->Address.Addr;
                    }
                } 
                *ParentDevices=pa;
                *EntryCount=ParentLst.ItemCount;
            } else {
                *PicIrqTblEntry=NULL;
                *ParentDevices=NULL;
                *EntryCount=0;
            }
            return EFI_NOT_FOUND;
        }
    } 
    else return EFI_INVALID_PARAMETER;
}

/**
    Returns device's corresponded APIC IRQ routing entry from MPPCIIRQ.INC
    table created by the BUILD process.
    If function returns EFI_SUCCESS the "ApicIrqTblEntry" updated with corresponded PIC 
    IRQ Table entry pointer. "ParentDevices" == NULL and "EntryCount"==0
    Function will return EFI_NOT_FOUND if there a no entry for the Device HANDLE passed.
 
    But it will return update the array of EFI_PCI_CONFIGURATION_ADDRESS structures with
    PCI Addresses of parenti 

        
    @param This Pointer to the PciBusExtended protocol.
    @param PciDeviceHandle Handle of the PCI Device to check.
    @param PciIo Pointer to the instance of PCI IO Protocol (OPTIONAL)
    @param ApicIrqTblEntry Double pointer to the corresponded PCI_IRQ_PIC_ROUTE entry
    @param ParentDevices Pointer to an array of parent Devices 
        for which was not found a valid PCI_IRQ_PIC_ROUTE entry.
    @param EntryCount Number of elements in an array above.
  
    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           If no PIRQ entry was found for this device.
        EFI_INVALID_PARAMETER   When Parameter passed is invalid.

    @note  
  If PciDeviceHandle == NULL, PciIo must be valid.
  If *PciIo == NULL PciDeviceHandle must be valid.
**/
EFI_STATUS EFIAPI PciExtApicIrqRout (
	IN  AMI_PCI_EXT_PROTOCOL	              			*This,
	IN  EFI_HANDLE                               		PciDeviceHandle,
	IN  EFI_PCI_IO_PROTOCOL						 		*PciIo 		OPTIONAL,
    OUT PCI_IRQ_APIC_ROUTE                              **ApicIrqTblEntry,
    OUT EFI_PCI_CONFIGURATION_ADDRESS                   **ParentDevices,
    OUT UINTN                                           *EntryCount )
{
    EFI_STATUS          Status;
	PCI_DEV_INFO		*dev;
    PCI_DEV_INFO        *parent;
    T_ITEM_LIST         ParentLst={0,0,NULL};
    INTN                i,j;
    EFI_PCI_CONFIGURATION_ADDRESS   *pa=NULL;
//------------------------------------        
    dev=PciExtGetDevice(PciDeviceHandle, PciIo);

	if(dev != NULL ){
        if( dev->ApicIrqEntry != NULL ){
            *ApicIrqTblEntry = dev->ApicIrqEntry;
            ParentDevices = NULL;
            EntryCount = 0;
            return EFI_SUCCESS;
        } else {
            parent=dev;
            do{
                parent=parent->ParentBrg; 
                Status=AppendItemLst(&ParentLst, parent);
				//The only bad Status here could be an EFI_OUT_OF_RESOURCES...
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status))return Status;
            }while( (parent->ApicIrqEntry == NULL) || (parent->Type != tPciRootBrg));
            
            if( parent->ApicIrqEntry != NULL ){
                *ApicIrqTblEntry=parent->ApicIrqEntry;
                //Create the array of addresses in order from Device which has IRQ endtry down to this Device parent
                if(ParentLst.ItemCount){    
                    pa=Malloc(sizeof(EFI_PCI_CONFIGURATION_ADDRESS)*(ParentLst.ItemCount));
                    for(i=(INTN)ParentLst.ItemCount-1, j=0; i<=0; i--,j++){
                        parent = (PCI_DEV_INFO*)ParentLst.Items[i];
                        pa[j] = parent->Address.Addr;
                    }
                } 
                *ParentDevices=pa;
                *EntryCount=ParentLst.ItemCount;
            } else {
                *ApicIrqTblEntry=NULL;
                *ParentDevices=NULL;
                *EntryCount=0;
            }
            return EFI_NOT_FOUND;
        }
    } 
    else return EFI_INVALID_PARAMETER;
}

VOID EFIAPI PciSmmReadyToLockCallback(IN EFI_EVENT Event, IN VOID *Context ){
	VOID           *rtl;
	EFI_STATUS	   Status;
//--------------------------
	if(!gReadyToLock){
	    Status = pBS->LocateProtocol(&gEfiDxeSmmReadyToLockProtocolGuid, NULL, &rtl );
	    if(!EFI_ERROR (Status))gReadyToLock=TRUE;
	}
}
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
