//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fix:    NVMe LED behavior abnormal.
//      Reason:     AMI remove after label update but issue happen.
//      Auditor:    Kasber Chen
//      Date:       Jun/16/2017
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

//**********************************************************************
// $Header:  $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PciInitPolicy.c
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Token.h>
#include <Include/PchAccess.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/IioUds.h>
#include <Protocol/IioSystem.h>

#include <Library/PciAccessCspLib.h>
#include <Library/AmiPciBusLib.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Library/PcdLib.h>
#include <AmiLib.h>
///Multy-Segment support
UINT32      gAmiSegmentsSupported=AMI_MAX_NUMBER_OF_PCI_SEGMENTS;
UINT64      *gAmiPcieBaseAddressArray=NULL;

UINTN SizeOfBuffer=AMI_MAX_NUMBER_OF_PCI_SEGMENTS*sizeof(UINT64);
UINT64 Buffer[AMI_MAX_NUMBER_OF_PCI_SEGMENTS]; 
UINT8 NumberofStacksPerCpu = 0xFF;
UINT8 BufferIndex = 0;
UINT8 TempBufferIndex = 0 ;
EFI_IIO_UDS_PROTOCOL  		*IIoUds;
EFI_IIO_SYSTEM_PROTOCOL     *gIioSystemProtocol;
UINT8						NumberOfSegments = AMI_MAX_NUMBER_OF_PCI_SEGMENTS;
UINT8 NumberOfAvailableSegments = 1;
BOOLEAN FoundNoofSegments = FALSE;
// APTIOV_SERVER_OVERRIDE_START : Include AmiPciBusSetupOverrideLib
#include <Library/AmiPciBusSetupOverrideLib.h>
// APTIOV_SERVER_OVERRIDE_END : Include AmiPciBusSetupOverrideLib

#include <Include/Register/PchRegsSata.h> 
#include <Include/Register/PchRegsEva.h> 
#include <PchPolicyCommon.h> 
#include <Include/Guid/PchRcVariable.h>

#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DriverBinding.h>
#include <Protocol\BlockIo.h>
#include <Protocol/AmiAhciBus.h>
#include <SataRaidOverrideInitElink.h>

#include <Library/SetupLib.h>

#if defined SECURITY_SUPPORT && SECURITY_SUPPORT
#include <Library/PciLib.h>
#include <Library/S3PciLib.h>
#endif

#if FPGA_SUPPORT
#include <Library/FpgaConfigurationLib.h>
#endif

#ifndef PCH_PCI_DEVICES_SEGMENT_NO
#define PCH_PCI_DEVICES_SEGMENT_NO  0x00
#endif

#if CRB_OUT_OF_RESOURCE_SUPPORT
#include <Guid/SetupVariable.h>
#include <Guid/SocketCommonRcVariable.h>

#define	MAX_NUMBER_OFFBOARD_VGA 4  

#define LAST_BOOT_FAILED_GUID \
{0x8163179a, 0xf31a, 0x4132, 0xae, 0xde, 0x89, 0xf3, 0x6a, 0xee, 0x43, 0xda}
EFI_GUID LastBootFailedIohGuid          = LAST_BOOT_FAILED_GUID;
EFI_IIO_UDS_PROTOCOL      *mIohUds = NULL;
SYSTEM_CONFIGURATION      *SystemConfiguration = NULL;
SOCKET_MP_LINK_CONFIGURATION  SocketMpLinkConfiguration;
//BOOLEAN     			  gPciOutOfResHit;
BOOLEAN                 AtleastOneVideoFound = FALSE;
BOOLEAN             	OnboardVideo = FALSE;
BOOLEAN             	OffboardVideo = FALSE;
BOOLEAN             	FirstCallFLAG = FALSE;
BOOLEAN                 StackSocketMisMatch = FALSE;
BOOLEAN                 MultiFunctionVGA = FALSE;

typedef struct _OFFBOARDVGADEVICESTRIED {
    UINT16      VenId;
    UINT16	DevId;
    BOOLEAN	Enabled;
    BOOLEAN BarDisabled;
    BOOLEAN VGATriedFlag;
    BOOLEAN MultiFunc;
    UINTN   ChildFuncCount;
    PCI_DEV_INFO  VGADevice;
    PCI_DEV_INFO  ChildFunc[8];
//    UINT64	Length;
} OFFBOARDVGADEVICESTRIED;

//  LAST_BOOT_FAILED_IOH Structure
typedef struct _LAST_BOOT_FAILED_IOH {
    UINT8     MmmioOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
    UINT8     IoOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
} LAST_BOOT_FAILED_IOH;

UINT8	MinimumResourceRequestedOffboarVGA;
UINT8	NumOffboardVideoPresent;
UINT8	ResourceTypeIndex;
UINTN	FailedResType;

OFFBOARDVGADEVICESTRIED OffboardVideoDevices[MAX_NUMBER_OFFBOARD_VGA];

EFI_STATUS
EnableDisableDevices (
  IN  PCI_DEV_INFO *dev,
  BOOLEAN	*PciOutOfResHit
);

typedef VOID (*DEVICE_PARSER)(
    PCI_DEV_INFO    *Device,
    BOOLEAN	    *PciOutOfResHit
    
);
#endif

BOOLEAN     gDxeSmmReadyToLock = FALSE;
EFI_GUID   	gSataGuidListCheckForRaid[] = { SATA_BUS_OVERRIDE_GUIDS_FOR_RAID {0} };
EFI_GUID   	gsSataGuidListCheckForRaid[] = { SSATA_BUS_OVERRIDE_GUIDS_FOR_RAID {0} };
BOOLEAN    	gLoadedImageDone = FALSE;
EFI_HANDLE 	gSataImageHandleArray[SataRaidBusOverrideMaxHandles] = {NULL};
EFI_HANDLE 	gsSataImageHandleArray[sSataRaidBusOverrideMaxHandles] = {NULL};
UINT8    	gSataGuidCount = 0;
UINT8    	gsSataGuidCount = 0;

// Instantiate AHCI_PLATFORM_POLICY_PROTOCOL with raid values 
// These values are taken from the gDefaultAhciPlatformPolicy in AhciBus.c
// The current Intel RAID driver can handle CD-ROM drives. So  AhciBusAtapiSupport
// is changed to FALSE to prevent the device from being installed twice.
AHCI_PLATFORM_POLICY_PROTOCOL        gRAIDAhciPlatformPolicy = {
    FALSE,                              // Legacy Raid option selected 
    FALSE,                               // AhciBus driver handles the ATAPI devices
    FALSE,                              // Drive LED on ATAPI Enable (DLAE)
#ifdef POWERUP_IN_STANDBY_SUPPORT
    POWERUP_IN_STANDBY_SUPPORT,         // PowerUpInStandby feature is supported or not
#else
    FALSE,
#endif
#ifdef POWERUP_IN_STANDBY_MODE
    POWERUP_IN_STANDBY_MODE,            // PowerUpInStandby mode
#else
    FALSE,
#endif
#ifdef DiPM_SUPPORT
    DiPM_SUPPORT,                        // Device Initiated power management
#else
    FALSE,
#endif
#ifdef ENABLE_DIPM
    ENABLE_DIPM,
#else
    FALSE,
#endif
#ifdef DEVICE_SLEEP_SUPPORT
    DEVICE_SLEEP_SUPPORT,
#else
    FALSE,
#endif
#ifdef ENABLE_DEVICE_SLEEP
    ENABLE_DEVICE_SLEEP,
#else
    FALSE,
#endif
#ifdef USE_PCIIO_MAP_ADDRESS_FOR_DATA_TRANSFER
    USE_PCIIO_MAP_ADDRESS_FOR_DATA_TRANSFER
#else
    FALSE
#endif
};

//-------------------------------------------------------------------------

typedef struct _RBINDEXLOCATOR {
	CHAR8 *RbAslDevName;
	UINT8 Index;
}RBINDEXLOCATOR;

RBINDEXLOCATOR  rbindexloc[] = {
		
		{"PC00",0}, {"PC01",1}, {"PC02",2}, {"PC03",3}, {"PC04",4}, {"PC05",5},
		{"PC06",6}, {"PC07",7}, {"PC08",8}, {"PC09",9}, {"PC10",10}, {"PC11",11},
		{"PC12",12}, {"PC13",13}, {"PC14",14}, {"PC15",15}, {"PC16",16}, {"PC17",17},
		{"PC18",18}, {"PC19",19}, {"PC20",20}, {"PC21",21}, {"PC22",22}, {"PC23",23}
#if MAX_SOCKET > 4		
		,{"PC24",24}, {"PC25",25}, {"PC26",26}, {"PC27",27}, {"PC28",28}, {"PC29",29}
#endif
#if MAX_SOCKET > 5
		,{"PC30",30}, {"PC31",31}, {"PC32",32}, {"PC33",33}, {"PC34",34}, {"PC35",35}
#endif
#if MAX_SOCKET > 6
		,{"PC36",36}, {"PC37",37}, {"PC38",38}, {"PC39",39}, {"PC40",40}, {"PC41",41}
#endif
#if MAX_SOCKET > 7
		,{"PC42",42}, {"PC43",43}, {"PC44",44}, {"PC45",45}, {"PC46",46}, {"PC47",47}
#endif
};

#if FPGA_SUPPORT
RBINDEXLOCATOR  Fpgarbindexloc[] = {
		{"VFP0",0}, {"VKT0",1}, {"VFP1",2}, {"VKT1",3}, {"VFP2",4}, {"VKT2",5}, {"VFP3",6}, {"VKT3",7}
};
#endif
//-------------------------------------------------------------------------
//!!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!!*
//!!!!!!!!!!!!!!!! must be maintained for PCI devices!!!!!!!!!!!!!!!!!!*
//-------------------------------------------------------------------------

///* APTIOV_SERVER_OVERRIDE_START
//TEMPLATE PCI DEVICE INIT FUNCTION START
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PciDevXXX_Init(
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)

{
//Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO   					*dev=(PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS  					Status=EFI_UNSUPPORTED;
//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;
    // APTIOV_SERVER_OVERRIDE_START : AmiPciBus Setup Override changes.
    if (dev == NULL) return Status;
    // APTIOV_SERVER_OVERRIDE_END : AmiPciBus Setup Override changes.
    switch (InitStep)
    {
	//-------------------------------------------------------------------------
		case isPciGetSetupConfig:
			PCI_TRACE((TRACE_PCI," (isPciGetSetupConfig); " ));
            // APTIOV_SERVER_OVERRIDE_START : AmiPciBus Setup Override changes.
            if (dev->PciExpress != NULL) {
               if (dev->PciExpress->Pcie2 != NULL) {
                   Status = AmiPciGetPcie2SetupDataOverrideHook (dev, &dev->PciExpress->Pcie2->Pcie2Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
               }
               Status = AmiPciGetPcie1SetupDataOverrideHook (dev, &dev->PciExpress->Pcie1Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
            }
            Status = AmiPciGetPciDevSetupDataOverrideHook (dev, &dev->DevSetup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
           // APTIOV_SERVER_OVERRIDE_END : AmiPciBus Setup Override changes.

		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciSetAttributes:
			PCI_TRACE((TRACE_PCI," (isPciSetAttributes); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciProgramDevice:
			PCI_TRACE((TRACE_PCI," (isPciProgramDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetAspm:
			PCI_TRACE((TRACE_PCI," (isPcieSetAspm); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetLinkSpeed:
			PCI_TRACE((TRACE_PCI," (isPcieSetLinkSpeed); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciGetOptionRom:
			PCI_TRACE((TRACE_PCI," (isPciGetOptionRom); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciOutOfResourcesCheck:
			PCI_TRACE((TRACE_PCI," (isPciOutOfResourcesCheck); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciReadyToBoot:
			PCI_TRACE((TRACE_PCI," (isPciReadyToBoot); " ));

		break;
	//-------------------------------------------------------------------------
    }//switch

    return Status;
}
//TEMPLATE PCI DEVICE INIT FUNCTION END
//*/ APTIOV_SERVER_OVERRIDE_END

#if CRB_OUT_OF_RESOURCE_SUPPORT
//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  DoesDeviceUseMmio
//
// Description: Check to see if this device uses this resource type
//
// Input: PCI_DEV_INFO          *Device        - Device to check
//
// Output:  BOOLEAN
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------

BOOLEAN DoesDeviceUseMmio(PCI_DEV_INFO *Device, PCI_BAR_TYPE Type)
{
    UINT8   BarIndex;

    for (BarIndex = 0;
        BarIndex <= PCI_MAX_BAR_NO;
        BarIndex++)
    {
        if (Device->Bar[BarIndex].Type == Type) return TRUE;
    }
    return FALSE;

}
//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  DisableDevResourceType
//
// Description: Disable this device's Resources of the given type
//
// Input: PCI_DEV_INFO          *Device        - Device to check
//
// Output:  None
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------

VOID DisableDevResourceType(PCI_DEV_INFO *Device, PCI_BAR_TYPE Type)
{
    UINT8   BarIndex;

    for (BarIndex = 0;
        BarIndex <= PCI_MAX_BAR_NO;
        BarIndex++)
    {
        if (Device->Bar[BarIndex].Type == Type)
        {
            Device->Bar[BarIndex].Type = tBarUnused;
            Device->Bar[BarIndex].DiscoveredType = tBarUnused;
            Device->Bar[BarIndex].Base = 0;
            Device->Bar[BarIndex].Length = 0;
        }
    }
}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  DisableDevOprom
//
// Description: Disable this device's OpROM BAR
//
// Input: PCI_DEV_INFO          *Device        - Device to check
//
// Output:  None
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------

VOID DisableDevOprom(PCI_DEV_INFO *Device)
{
    UINT8   BarIndex;

    for (BarIndex = 0;
        BarIndex <= PCI_MAX_BAR_NO;
        BarIndex++)
    {
        if (Device->Bar[BarIndex].Offset == 0x30)
        {
            Device->Bar[BarIndex].Type = tBarUnused;
            Device->Bar[BarIndex].DiscoveredType = tBarUnused;
            Device->Bar[BarIndex].Base = 0;
            Device->Bar[BarIndex].Length = 0;
        }
    }
}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  ParseAllDevices
//
// Description: Execute passed in function on all devices behind
//              this bridge.
//
// Input: PCI_BRG_INFO          *Bridge        - Bridge
//        DEVICE_PARSER          Parser         - Parsing Function
//        VOID                  *Context       - any data the parsing
//                                               function needs
//
// Output:  None
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
VOID ParseAllDevices(
    PCI_BRG_INFO            *Bridge,
    BOOLEAN		    *PciOutOfResHit,
    DEVICE_PARSER            Parser
    )
{
    UINTN   i;

    for (i = 0 ; i < Bridge->Bridge.ChildCount ; i++)
    {
        switch (Bridge->Bridge.ChildList[i]->Type)
        {
            case    tPci2PciBrg:
            case    tPci2CrdBrg:
                //call recursively on next-level bridges
                    ParseAllDevices(
                    (PCI_BRG_INFO*)(Bridge->Bridge.ChildList[i]),
                    PciOutOfResHit,
                    Parser
                    );
           default:
                //call parse function on this device
                Parser(Bridge->Bridge.ChildList[i],PciOutOfResHit);
        }
    }

}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  GetRootBridge
//
// Description: Find Root Bridge this device is on
//
// Input: PCI_DEV_INFO          Devoce         - Concerned Device
//
// Output:  None
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
PCI_DEV_INFO *GetRootBridge(
    PCI_DEV_INFO    *Device
    )
{
    if (Device->Type == tPciRootBrg) return Device;

    return GetRootBridge(Device->ParentBrg);

}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  CheckNumberOfOffBoardVGADevices
//
// Description: Returns the Number of Offboard video devices
//
// Input: PCI_DEV_INFO          Devoce         - Concerned Device
//
// Output:  None
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
VOID CheckNumberOfOffBoardVGADevices(
    PCI_DEV_INFO    *Device,
    BOOLEAN	    *PciOutOfResHit
)
{
      EFI_STATUS                Status = EFI_SUCCESS;
      UINT8                     i=0;
      PCI_DEV_INFO              *ChildFunc;
      UINT8                     ChildFuncCnt;

      static UINT8		Count = 0 ;
      
      if ((Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x00) ||
          (Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x80) ||
          (Device->Class.BaseClassCode == 0x04 && Device->Class.SubClassCode == 0x00))
      	      {
	      // Offboard Video devices
	      	      if (!(Device->Attrib & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE))
	      	      {
	      		      OffboardVideoDevices[Count].VenId = Device->DevVenId.VenId;
	      		      OffboardVideoDevices[Count].DevId = Device->DevVenId.DevId;
                      PCI_TRACE ((TRACE_PCI, "\n Offboard Video device information : VENDDORID=%x DEVICEID=%x\n", OffboardVideoDevices[Count].VenId, OffboardVideoDevices[Count].DevId));
	      		      pBS->CopyMem(&OffboardVideoDevices[Count].VGADevice, Device, sizeof(PCI_DEV_INFO));

	      		      // If This is a Multi-Function Device. Need to Disable all functions of this device.
	      		      if(Device->Func0!=NULL && (Device->FuncCount!=0 || Device->FuncInitCnt!=0)){
	      		        MultiFunctionVGA = TRUE;
	      		        OffboardVideoDevices[Count].MultiFunc = TRUE; 
	      		        OffboardVideoDevices[Count].ChildFuncCount = Device->FuncCount;
	      		          PCI_TRACE ((TRACE_PCI, "A Multi-Function Device with FuncCount=%xh\n",Device->FuncCount));
	      		          for(ChildFuncCnt=0; ChildFuncCnt< Device->FuncCount ; ChildFuncCnt++){
	      		                ChildFunc = Device->DevFunc[ChildFuncCnt];
	                            pBS->CopyMem(&OffboardVideoDevices[Count].ChildFunc[ChildFuncCnt], ChildFunc, sizeof(PCI_DEV_INFO));
	                        }// Child Functions of VGA Device
	                    }// Multi-Function Device
	      		      
	      		      Count = Count + 1;
	      
	      	      }
      	      }
      
      NumOffboardVideoPresent = Count;

}


//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  checkVideoDevice
//
// Description: Find VideoDevice on the corresponding IOH
//
// Input: PCI_DEV_INFO          Devoce         - Concerned Device
//
// Output:  SUCCESS - SKIP THIS VIDEO DEVICE,UNSUPPORTED - DON"T SKIP THIS VIDEO
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
EFI_STATUS
checkVideoDevice (
IN  PCI_DEV_INFO *dev
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN               SocketNum;
  UINTN               StackNum;
  
  SocketNum = SocketMpLinkConfiguration.LegacyVgaSoc;
  StackNum = SocketMpLinkConfiguration.LegacyVgaStack;

  if ((dev->Address.Addr.Bus >= mIohUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusBase) && (dev->Address.Addr.Bus <= mIohUds->IioUdsPtr->PlatformData.IIO_resource[SocketNum].StackRes[StackNum].BusLimit))
  {
      if (dev->Attrib & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)
      {
          PCI_TRACE((TRACE_PCI,"\n:: Onboard Device\n"));
          OnboardVideo = TRUE;
      }
      else
      {
          PCI_TRACE((TRACE_PCI,"\n:: Offboard Device\n"));
          OffboardVideo = TRUE;
      }
      
      if (((SystemConfiguration->VideoSelect == 1) && OnboardVideo) || ((SystemConfiguration->VideoSelect == AUTO) && (NumOffboardVideoPresent == 0))) // Auto/Onboard
      {
          AtleastOneVideoFound = TRUE;
          PCI_TRACE((TRACE_PCI,"\n VGA Selection in Setup is :: Onboard\n"));
          return EFI_UNSUPPORTED; // Do not skip the device
      }

      if ((SystemConfiguration->VideoSelect == AUTO || SystemConfiguration->VideoSelect == 2) && OffboardVideo) // Offboard
      {
          AtleastOneVideoFound = TRUE;
          PCI_TRACE((TRACE_PCI,"\n VGA Selection in Setup :: Offboard\n"));
          return EFI_UNSUPPORTED; // Do not skip the device
      }
      
  }
  else{
      StackSocketMisMatch = TRUE ;
      PCI_TRACE((TRACE_PCI,"\nVGA is not on the Selected Stack. Disabling it\n"));
      return EFI_SUCCESS; // SKIP the device
  }


  return Status;
}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  EnableDisableResources
//
// Description: Disable the current device's MMIO/IO if it is present and
// on a ROotBridge .
//
// Input:           Device          - current device (not necessarily problematic device)
//                  Context         - Device path of current slot
//
// Output:  EFI_STATUS
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
VOID EnableDisableResources(
    PCI_DEV_INFO    *Device,
    BOOLEAN	    *PciOutOfResHit
)
{

      EFI_STATUS                Status = EFI_SUCCESS;
      EFI_STATUS                PciReadStatus;
      UINT8		                OffboardVideoIndex = 0;
      PCI_DEV_INFO              *ChildFunc;
      UINT8                     ChildFuncCnt;
      UINT8                     ChildFns;
      UINT8                     i=0, j=0;
      PCI_BAR                   *VfBar;
      PCI_CFG_ADDR              addr;
      PCIE_EXT_CAP_HDR          extcap;
      BOOLEAN                   IsDeviceVGA = FALSE;
      BOOLEAN                   SRIOVDevice = FALSE; 

      if ((Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x00) ||
          (Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x80) ||
          (Device->Class.BaseClassCode == 0x04 && Device->Class.SubClassCode == 0x00))
              {
          IsDeviceVGA = TRUE;
              }
    if ((Device->Address.Addr.Bus == mIohUds->IioUdsPtr->PlatformData.IIO_resource[0].StackRes[0].BusBase) && (!IsDeviceVGA))
    {
        PCI_TRACE((TRACE_PCI,"\nCPU0 Stack0 onboard Devices won't be SKIPPED\n"));
        return;   // Do not skip the device
    }

    else
    {

      if (Device->Class.BaseClassCode == 0x06 ) // this is the bridge device
      {
          PCI_TRACE((TRACE_PCI,"\nBridge Device won't be SKIPPED\n"));
          return ;    // Do not skip the device
      }


      if (AtleastOneVideoFound == TRUE)
      {
        if ((Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x00) ||
            (Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x80) ||
            (Device->Class.BaseClassCode == 0x04 && Device->Class.SubClassCode == 0x00))
        {
              Status = EFI_SUCCESS; // FLAG VGA Device for Disabling and SKIP THE VIDEO
              for (OffboardVideoIndex = 0 ; OffboardVideoIndex < NumOffboardVideoPresent ; OffboardVideoIndex++)
                {
                  if ((Device->DevVenId.VenId == OffboardVideoDevices[OffboardVideoIndex].VenId) && (Device->DevVenId.DevId == OffboardVideoDevices[OffboardVideoIndex].DevId && (OffboardVideoDevices[OffboardVideoIndex].BarDisabled == FALSE))){
                      OffboardVideoDevices[OffboardVideoIndex].BarDisabled = TRUE;
                      PCI_TRACE ((TRACE_PCI, "\nDisabling the Video device : VENDDORID=%x DEVICEID=%x BarDisabled:%d\n",Device->DevVenId.VenId,Device->DevVenId.DevId,OffboardVideoDevices[OffboardVideoIndex].BarDisabled));
                  }
                }
        }
      }

      else
      {
        if ((Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x00) ||
            (Device->Class.BaseClassCode == 0x03 && Device->Class.SubClassCode == 0x80) ||
            (Device->Class.BaseClassCode == 0x04 && Device->Class.SubClassCode == 0x00))
        {
            Status = checkVideoDevice(Device);
            // If VGA selection is Offboard and even after enabling one offboard VGA card and disabled the other Offboard VGA cards
            // System may not boot because the Enabled VGA card requests more resource.
            // Skip VGAs if they are not from Chosen Stack
        	if(!StackSocketMisMatch){
        	      if (OffboardVideo && (NumOffboardVideoPresent > 1))
        	      {
        			  for (OffboardVideoIndex = 0 ; OffboardVideoIndex < NumOffboardVideoPresent ; OffboardVideoIndex++)
        			  {
        		  		  if ((Device->DevVenId.VenId == OffboardVideoDevices[OffboardVideoIndex].VenId) && (Device->DevVenId.DevId == OffboardVideoDevices[OffboardVideoIndex].DevId))
        				  {
        		              PCI_TRACE ((TRACE_PCI, "\nVideo device : VENDDORID=%x DEVICEID=%x\n",Device->DevVenId.VenId,Device->DevVenId.DevId));
        		  		      if(OffboardVideoDevices[OffboardVideoIndex].Enabled == FALSE){
        		  		          
        		  		          OffboardVideoDevices[OffboardVideoIndex].Enabled = TRUE;
        		  		          
        		  		          if( OffboardVideoDevices[OffboardVideoIndex].BarDisabled == TRUE){
        		  		              pBS->CopyMem(Device, &OffboardVideoDevices[OffboardVideoIndex].VGADevice, sizeof(PCI_DEV_INFO));
        		  		              OffboardVideoDevices[OffboardVideoIndex].BarDisabled = FALSE;
        		                        if(OffboardVideoDevices[OffboardVideoIndex].MultiFunc == TRUE){
        		                            // If Multi-Function: Need to enable Child Functions as well. For now only 2 child functions are considered.
        		                            PCI_TRACE ((TRACE_PCI, "A Multi-Function Device with FuncCount=%xh\n",Device->FuncCount));
        		                            for(ChildFuncCnt=0; ChildFuncCnt< Device->FuncCount ; ChildFuncCnt++){
        		                                ChildFunc = Device->DevFunc[ChildFuncCnt];
        		                                PCI_TRACE ((TRACE_PCI, "Child Fn: OffboardVideoDevice: Address:%x DevFunc%x\n",OffboardVideoDevices[OffboardVideoIndex].VGADevice.DevFunc[ChildFuncCnt],ChildFunc));
        		                                pBS->CopyMem(Device->DevFunc[ChildFuncCnt], &OffboardVideoDevices[OffboardVideoIndex].ChildFunc[ChildFuncCnt], sizeof(PCI_DEV_INFO));   
        		                            }
        		                        }
        		                          break;
        		  		          }
                                  PCI_TRACE ((TRACE_PCI, "\nExit 1 : VGA[%d] VISIT 1",OffboardVideoIndex));
        		  		          break;
        		  		      }
							  if ((OffboardVideoDevices[OffboardVideoIndex].Enabled == TRUE) && (!OffboardVideoDevices[OffboardVideoIndex].VGATriedFlag)){
        		  		        AtleastOneVideoFound = FALSE;
        		  		        OffboardVideoDevices[OffboardVideoIndex].VGATriedFlag = TRUE;
        		  		        Status = EFI_SUCCESS; 
        		  		        PCI_TRACE ((TRACE_PCI, "\nExit 2: VGA[%d] Tried and Disabled AtleastOneVideoFound:%d\n",OffboardVideoIndex, AtleastOneVideoFound));
        		  		        break;
        		  		      }
	                            PCI_TRACE ((TRACE_PCI, "\nExit 3: VGA[%d] Tried and Disabled VISIT 3 AtleastOneVideoFound:%d\n",OffboardVideoIndex, AtleastOneVideoFound));
        				  }
        			  }
        			  if (OffboardVideoIndex >= NumOffboardVideoPresent) 
        			  {
                          TRACE ((TRACE_ALWAYS, "Multiple VGAs: Not Able to Disable VIDEO CARDs which requested more resources VENDDORID=%x DEVICEID=%x\n",Device->DevVenId.VenId,Device->DevVenId.DevId));
        			      ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
        			      EFI_DEADLOOP();// NOT ABLE TO DISABLE VIDEO CARDs which requested more IO. 
        			  }

        	      }else{
        	          if(OffboardVideoDevices[0].Enabled == FALSE){
        	              PCI_TRACE ((TRACE_PCI, "ONE VGA: VENDDORID=%x DEVICEID=%x; Enabling  and Trying  \n",Device->DevVenId.VenId,Device->DevVenId.DevId));
        	              OffboardVideoDevices[0].Enabled = TRUE; // Enable this VGA once and see if System Boots
        	          }else{
                          TRACE ((TRACE_ALWAYS, "ONE VGA: Not Able to Disable VIDEO CARD which requested more resources VENDDORID=%x DEVICEID=%x\n",Device->DevVenId.VenId,Device->DevVenId.DevId));
                        #if (defined(AMI_CRB_OOR_FORCE_DISABLE_VGA) && (AMI_CRB_OOR_FORCE_DISABLE_VGA == 1))
                          Status = EFI_SUCCESS;
                          goto  DeviceDisable;
                       #endif
                          ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
                          EFI_DEADLOOP();// NOT ABLE TO DISABLE VIDEO CARDs which requested more IO.
                          
        	          }
        	      }
        	}
        }
      }
    }

#if (defined(AMI_CRB_OOR_FORCE_DISABLE_VGA) && (AMI_CRB_OOR_FORCE_DISABLE_VGA == 1))
DeviceDisable:
#endif
      if (Status != EFI_SUCCESS)
      {
        PCI_TRACE ((TRACE_PCI, "\nEnabling the PCI device : VENDDORID=%x DEVICEID=%x\n",Device->DevVenId.VenId,Device->DevVenId.DevId));
        return ;
      }
      
      // In Case of Multi Function VGA, Once VGA is enabled, all its child function devices should be 
      // skipped from Disabling.
      if(MultiFunctionVGA){
          for (OffboardVideoIndex = 0 ; OffboardVideoIndex < NumOffboardVideoPresent ; OffboardVideoIndex++)
            {
              if(OffboardVideoDevices[OffboardVideoIndex].MultiFunc){
                  for (ChildFns = 0 ; ChildFns < OffboardVideoDevices[OffboardVideoIndex].ChildFuncCount; ChildFns++){
                      if ((Device->DevVenId.VenId == OffboardVideoDevices[OffboardVideoIndex].ChildFunc[ChildFns].DevVenId.VenId) && (Device->DevVenId.DevId == OffboardVideoDevices[OffboardVideoIndex].ChildFunc[ChildFns].DevVenId.DevId))
                      {
                          if (!OffboardVideoDevices[OffboardVideoIndex].BarDisabled){
                              PCI_TRACE ((TRACE_PCI, "\nSkipping the PCI device :\nBUS=%x DEVICE=%x FUNCTION:%x",Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function));
                              return ;
                          }
                          
                        }
                  }
              }
            }
      }
      
      // Does this device use  MMIO/IO?
      if (DoesDeviceUseMmio(Device, tBarMmio32) ||
          DoesDeviceUseMmio(Device, tBarMmio32pf)||
          DoesDeviceUseMmio(Device, tBarIo16)|| 
          DoesDeviceUseMmio(Device, tBarIo32))
      {
        // yes, disable its  MMIO/IO
    	  PCI_TRACE ((TRACE_PCI, "\nDisabling the PCI device :\nBUS=%x DEVICE=%x FUNCTION:%x",Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function));
    	  PCI_TRACE ((TRACE_PCI, " VENDDORID=%x DEVICEID=%x\n",Device->DevVenId.VenId,Device->DevVenId.DevId));
    	  
          for(i=0; i<=PCI_MAX_BAR_NO; i++){
              if((Device->Bar[i].Type>0) && (Device->Bar[i].Length>0)){
                  PCI_TRACE((TRACE_PCI,"BarTypes Fn0: BAR Index=%d;\tType=%d;\tGRA=0x%lX;\tLEN=0x%lX;\tOffset=0x%X;\n", 
                      i, Device->Bar[i].Type, Device->Bar[i].Gran, 
                      Device->Bar[i].Length, Device->Bar[i].Offset));
              }
          }
          DisableDevResourceType(Device,tBarIo16);
          DisableDevResourceType(Device,tBarIo32);
    	  DisableDevResourceType(Device,tBarMmio32);
          DisableDevResourceType(Device,tBarMmio32pf);
          
          //
          // handling SRIOV Devices.
          //
            if( SystemConfiguration->SRIOVEnable ){
                addr.ADDR=Device->Address.ADDR;
                //Check Pci Express Extended Capability header
                addr.Addr.ExtendedRegister=0x100;
                
                while(addr.Addr.ExtendedRegister){
                    PciReadStatus=PciCfg32(Device->RbIo,addr,FALSE,&extcap.EXT_CAP_HDR);
                    ASSERT_EFI_ERROR(PciReadStatus);
                    
                    if (extcap.ExtCapId==PCIE_CAP_ID_SRIOV ){
                        SRIOVDevice = TRUE;
                        PCI_TRACE((TRACE_PCI,"It is SRIOV Device \n"));
                        break;
                    }
                    addr.Addr.ExtendedRegister=extcap.NextItemPtr;
                }
                
                if(SRIOVDevice){
                    
                    VfBar=&Device->PciExpress->SriovData->Bar[0];
                    
                    for( j=0; j<PCI_MAX_BAR_NO; j++){
                       
                        if ((VfBar[j].Type >= tBarIo16 ) && VfBar[j].Length){
                          PCI_TRACE((TRACE_PCI,"SRIOVHandling BarType:%d\n",j));
                          Device->Bar[j].Type = tBarUnused;
                          Device->Bar[j].DiscoveredType = tBarUnused;
                          Device->Bar[j].Base = 0;
                          Device->Bar[j].Length = 0;
                          
                          }
                        }// bar Loop
                        
                        pBS->SetMem(&Device->PciExpress->SriovData->Bar[0], (PCI_MAX_BAR_NO * sizeof(PCI_BAR)),0);

                        for(i=0; i<=PCI_MAX_BAR_NO; i++){
                        PCI_TRACE((TRACE_PCI," BarTypes After SRIOVHandling BAR Index=%d;\tType=%d;\tGRA=0x%lX;\tLEN=0x%lX;\tOffset=0x%X;\n", 
                        i, Device->Bar[i].Type, Device->Bar[i].Gran, 
                        Device->Bar[i].Length, Device->Bar[i].Offset));
                        }
                        
                    }
                SRIOVDevice = FALSE;
            }

          // If This is a Multi-Function Device. Need to Disable all functions of this device.
          if(Device->Func0!=NULL && (Device->FuncCount!=0 || Device->FuncInitCnt!=0)){
              PCI_TRACE ((TRACE_PCI, "A Multi-Function Device with FuncCount=%xh\n",Device->FuncCount));
              for(ChildFuncCnt=0; ChildFuncCnt< Device->FuncCount ; ChildFuncCnt++){
                  ChildFunc = Device->DevFunc[ChildFuncCnt];
                  
                  for(i=0; i<=PCI_MAX_BAR_NO; i++){
                      if((Device->Bar[i].Type >= tBarIo16) && (Device->Bar[i].Length>0)){
                          PCI_TRACE((TRACE_PCI,"BarTypes: BAR Index=%d;\tType=%d;\tGRA=0x%lX;\tLEN=0x%lX;\tOffset=0x%X;\n", 
                              i, Device->Bar[i].Type, Device->Bar[i].Gran, 
                              Device->Bar[i].Length, Device->Bar[i].Offset));
                      }
                  }
                  
                  // Does this device use  MMIO/IO?
                  if (DoesDeviceUseMmio(ChildFunc, tBarMmio32) ||
                      DoesDeviceUseMmio(ChildFunc, tBarMmio32pf)||
                      DoesDeviceUseMmio(ChildFunc, tBarIo16)|| 
                      DoesDeviceUseMmio(ChildFunc, tBarIo32)){
                      
                    // yes, disable its  MMIO/IO
                      PCI_TRACE ((TRACE_PCI, "Disabling the Child Function :\nBUS=%x DEVICE=%x FUNCTION:%x",ChildFunc->Address.Addr.Bus,ChildFunc->Address.Addr.Device,ChildFunc->Address.Addr.Function));
                      PCI_TRACE ((TRACE_PCI, " VENDDORID=%x DEVICEID=%x\n",ChildFunc->DevVenId.VenId,ChildFunc->DevVenId.DevId));
                      DisableDevResourceType(ChildFunc,tBarIo16);
                      DisableDevResourceType(ChildFunc,tBarIo32);
                      DisableDevResourceType(ChildFunc,tBarMmio32);
                      DisableDevResourceType(ChildFunc,tBarMmio32pf);
                      
                      
                     // handling SRIOV Devices.
                      if( SystemConfiguration->SRIOVEnable ){
                          
                          addr.ADDR=Device->Address.ADDR;
                          //Check Pci Express Extended Capability header
                          addr.Addr.ExtendedRegister=0x100;
                          
                          while(addr.Addr.ExtendedRegister){
                              PciReadStatus=PciCfg32(Device->RbIo,addr,FALSE,&extcap.EXT_CAP_HDR);
                              ASSERT_EFI_ERROR(PciReadStatus);
                              
                              if (extcap.ExtCapId==PCIE_CAP_ID_SRIOV ){
                                  SRIOVDevice = TRUE;
                                  PCI_TRACE((TRACE_PCI,"It is SRIOV Child Devices \n"));
                                  break;
                              }
                              addr.Addr.ExtendedRegister=extcap.NextItemPtr;
                          }
                          if(SRIOVDevice){
                              
                              VfBar=&ChildFunc->PciExpress->SriovData->Bar[0];
                              for( j=0; j<PCI_MAX_BAR_NO; j++){
                                  if ((VfBar[j].Type >= tBarIo16 ) && VfBar[j].Length){
                                      PCI_TRACE((TRACE_PCI,"SRIOVHandling BarType:%d\n",j));
                                      ChildFunc->Bar[j].Type = tBarUnused;
                                      ChildFunc->Bar[j].DiscoveredType = tBarUnused;
                                      ChildFunc->Bar[j].Base = 0;
                                      ChildFunc->Bar[j].Length = 0;
                                  }
                              }// bar Loop
                              pBS->SetMem(&ChildFunc->PciExpress->SriovData->Bar[0], (PCI_MAX_BAR_NO * sizeof(PCI_BAR)),0);
                          }
                          SRIOVDevice = FALSE;
                      }
                  }
              }
          }
          *PciOutOfResHit = TRUE;
      }

}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  EnableDisableDevices
//
// Description: If there was a resource overflow, implement the following
//
// Input:           PCI_DEV_INFO *Device - current device (not necessarily problematic device)
//                  UINTN Count          - count of already disabled devices
//                  UINTN LowResType     - MRES_TYPE - type of resource causing overflow
//
// Output:  Enable all devices on BUS0.In other BUS,enable bridge devices and  one video device.
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
EFI_STATUS EnableDisableDevices(
  IN  PCI_DEV_INFO *Device,
  IN  BOOLEAN      *PciOutOfResHit
)
{
    PCI_DEV_INFO                *RootBridge;
    EFI_HANDLE                  *Handle = NULL;

    UINTN                       SocketVariableSize = sizeof(SOCKET_MP_LINK_CONFIGURATION);
    UINT32                      SocketVariableAttr;
    EFI_STATUS                  Status;
    RootBridge = GetRootBridge(Device);
	Status = pRS->GetVariable( SOCKET_MP_LINK_CONFIGURATION_NAME,
	                                &gEfiSocketMpLinkVariableGuid,
	                                &SocketVariableAttr,
	                                &SocketVariableSize,
	                                &SocketMpLinkConfiguration
	                                );
	if (EFI_ERROR(Status)) {
        TRACE((TRACE_ALWAYS,"\n PciBoardDxeInit.c SOCKET_MP_LINK_CONFIGURATION_NAME : Getvariable failed\n" ));
        EFI_DEADLOOP();
	}
		
	ASSERT_EFI_ERROR(Status);	
    // execute MMIO/IO function on all devices behind this root bridge.
// if more than One Offboard VGA cards . Try One VGA Card at a time and see if System Boots.
// Halt the System if Enabled VGA is requesting more resources.
	if(!FirstCallFLAG){
	    // Number of VGA Devices wont change over differnt calls of CrbDefinedTable. So, Checking Number of VGAs only on First Call to CrbDefinedTable
        PCI_TRACE((TRACE_PCI,"CheckNumberOfOffBoardVGADevices: START\n" ));
	    ParseAllDevices ((PCI_BRG_INFO*)RootBridge,PciOutOfResHit,CheckNumberOfOffBoardVGADevices);
	    FirstCallFLAG = TRUE;
        PCI_TRACE((TRACE_PCI,"CheckNumberOfOffBoardVGADevices: NumOffboardVideoPresent:%d END\n",NumOffboardVideoPresent ));
	}
    ParseAllDevices ((PCI_BRG_INFO*)RootBridge,PciOutOfResHit,EnableDisableResources);
    return EFI_UNSUPPORTED;

}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  GetSetupData
//
// Description: Reading the Setup Data
//
// Input:   None
//
// Output:  EFI_STATUS
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
EFI_STATUS
GetSetupData ()
{
//  EFI_GUID            SetupGuid = SETUP_GUID;
  UINTN               SetupDataVarSize = sizeof(SYSTEM_CONFIGURATION);
  EFI_STATUS          GetSetStatus=EFI_SUCCESS;

  GetSetStatus = GetEfiVariable(
                                L"IntelSetup",
                                &gEfiSetupVariableGuid,
                                NULL,
                                &SetupDataVarSize,
                                &SystemConfiguration
                               );

   ASSERT_EFI_ERROR (GetSetStatus);
   if (EFI_ERROR(GetSetStatus))
      return GetSetStatus;

    return GetSetStatus;

}

//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:  GetIohUdsStrcuture
//
// Description: Reading the IohUdsStrcuture
// Input :  None
// Output:  EFI_STATUS
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
EFI_STATUS
GetIohUdsStrcuture ()
{
//  EFI_GUID            gEfiIohUdsProtocolGuid = EFI_IOH_UDS_PROTOCOL_GUID;
  EFI_STATUS          GetSetStatus=EFI_SUCCESS;

  GetSetStatus = pBS->LocateProtocol(
                                    &gEfiIioUdsProtocolGuid,
                                    NULL,
                                    &mIohUds
                                    );

  ASSERT_EFI_ERROR (GetSetStatus);
  if (EFI_ERROR(GetSetStatus))
    return GetSetStatus;

  return  GetSetStatus;

}


//---------------------------------------------------------------------------
//<AMI_PHDR_START>
//
// Name:	CrbDefinedPciDeviceTable
//
// Description:	If there was a resource overflow, implement the following
//
// Input:           PCI_DEV_INFO *Device - current device (not necessarily problematic device)
//                  UINTN Count          - count of already disabled devices
//                  UINTN LowResType     - MRES_TYPE - type of resource causing overflow
//
// Output:	Enable all devices on CPU 0 Stack 0.In other Stacks,enable bridge devices and one video device.
//
// Referrals:
//
//<AMI_PHDR_END>
//---------------------------------------------------------------------------
EFI_STATUS CrbDefinedPciDeviceTable(
    PCI_DEV_INFO *Device,
    UINTN Count,
    UINTN LowResType,
    BOOLEAN **PciOutOfResHit
    )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    
    FailedResType = LowResType;

    if (SystemConfiguration == NULL)
    {
        GetSetupData();
    }

    if (mIohUds == NULL)
    {
        GetIohUdsStrcuture();
    }
    PCI_TRACE((TRACE_PCI,"\n  In CrbDefinedPciDeviceTable(): LowResType%lx FailedResType=%lx\n",LowResType,FailedResType));
    Status =  EnableDisableDevices(Device,*PciOutOfResHit);

    AtleastOneVideoFound = FALSE;
    return Status;
}
#endif

EFI_STATUS
EFIAPI
GetIioRootPortIndex (
  IN PCI_DEV_INFO   *Dev,
  IN OUT UINT8      *SocketIndex,
  IN OUT UINTN      *RootPortIndex
  )
{
    PCI_ROOT_BRG_DATA       *RootBridge=(PCI_ROOT_BRG_DATA*)(Dev->ParentBrg);
    UINTN                   PortIndex = 0;
    UINT8                   Socket = 0;

    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (!IIoUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
        {
            continue;
        }

        if (RootBridge->RbIoProtocol.SegmentNumber != IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].PcieSegment) {
            continue;
            
        }
        for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
            if ((Dev->Address.Addr.Bus == gIioSystemProtocol->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[Socket][PortIndex]) &&
                    (Dev->Address.Addr.Device == gIioSystemProtocol->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device) &&
                    (Dev->Address.Addr.Function == gIioSystemProtocol->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function)) {
                *SocketIndex = Socket;
				*RootPortIndex = PortIndex;
                return EFI_SUCCESS;
            }
        }
    }

    return EFI_NOT_FOUND;
}


EFI_STATUS PcieRootPortInitCallback (
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)
{
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
	PCI_DEV_INFO   					*dev=(PCI_DEV_INFO*)Args->Param1;
	EFI_PCI_IO_PROTOCOL		*PciIo = &dev->PciIo ;		//PCI IO Protocol Instance
	EFI_STATUS            Status;
    UINT8   SocketIndex;
    UINTN   IioRootPortIndex;
#if SMCPKG_SUPPORT
	UINT16                       SltSts;
	UINT16                       SltCnt;	
#endif
#if CRB_OUT_OF_RESOURCE_SUPPORT		
	UINTN			     *Count;
	UINTN                        *LowResType;
	BOOLEAN     		      *PciOutOfResHit;
#endif
	
	if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
	if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;
	
    if ( gIioSystemProtocol == NULL ) {
        Status = pBS->LocateProtocol( \
                &gEfiIioSystemProtocolGuid, \
                NULL, \
                &gIioSystemProtocol);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;
    }

	    switch (InitStep)
	    {
		//-------------------------------------------------------------------------
#if SMCPKG_SUPPORT
			case isPciProgramDevice:
				PCI_TRACE((TRACE_PCI," (isPciProgramDevice); " ));
				if ((dev->PciExpress->PcieCap.SlotImpl) && (dev->PciExpress->SlotCap.HpCapable))  { // check whether slot is implemented and hot plug capable.

				     // enable interrupt sources
				     //
				     // Get slot control information and update interrupt sources
				     //
				     // Enable all Hot Plug Interrupts except MRL sensor bit 2 and  Hot Plug Interrupt HPINTEN bit-5. Command completion interrupt is enabled in ASL.
				     //
				     Status = PciIo->Pci.Read(
				         PciIo,
				         EfiPciIoWidthUint16,
				         0xA8,  // offset
				         1,  // width
				         &SltCnt);
				     ASSERT_EFI_ERROR(Status);     
				     PCI_TRACE((TRACE_PCI," slot control value read %x \n ", SltCnt ));           
				     SltCnt |= (BIT3 | BIT1 | BIT0);  // Presence detect change | Power fault detected | attention button pressed
				     SltCnt |=   (BIT6 | BIT7);       // turn of attention led
				     //           
				     // if no device connected, then turn off power and power indicator bits         
				     //
 				     Status = PciIo->Pci.Read(
				         PciIo,
				         EfiPciIoWidthUint16,
				         0xAA,  // offset
				         1,  // width
				         &SltSts);
				     ASSERT_EFI_ERROR(Status);       
				     PCI_TRACE((TRACE_PCI," slot status value read %x \n ", SltSts ));                      
				     if ((SltSts & BIT6) == 0) { // if no device connected, then turn off power and power indicator bits 
					    PCI_TRACE((TRACE_PCI," No device connected, remove power to HP slot \n "));     
				           SltCnt |= (BIT8 | BIT9 | BIT10); //Bit 8:9: Power indicator control (11: Led Off,   01: LEd On)      Bit 10: Power controller control (1: Power off)
				     }
				     Status = PciIo->Pci.Write(
				         PciIo,
 				         EfiPciIoWidthUint16,
				         0xA8,  // offset
				         1,  // width
				         &SltCnt);   
           
				     Status = PciIo->Pci.Read(
   				         PciIo,
				         EfiPciIoWidthUint16,
				         0xA8,  // offset
				         1,  // width
				         &SltCnt);
				     ASSERT_EFI_ERROR(Status);
				     PCI_TRACE((TRACE_PCI," slot control value after write %x \n ", SltCnt ));               
           
				     Status =  EFI_SUCCESS;       
				     break;	
				}
				Status=EFI_UNSUPPORTED;				
				break;	
#endif
			case isPciOutOfResourcesCheck:
				PCI_TRACE((TRACE_PCI," (isPciOutOfResourcesCheck); " ));
#if CRB_OUT_OF_RESOURCE_SUPPORT	
				Count        = (UINTN*)Args->Param2;
				LowResType   = (UINTN*)Args->Param3;
				PciOutOfResHit = (BOOLEAN*)Args->Param4;
                if( *LowResType > 4 ){
                    PCI_TRACE((TRACE_PCI,"\nMMIOH OOR happened and is not Handled in CRB \n"));
                    PCI_TRACE((TRACE_PCI,"\n SYSTEM is HALTED \n"));
                    ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
                    EFI_DEADLOOP();
                }
                
				Status = CrbDefinedPciDeviceTable(dev,*Count,*LowResType,&PciOutOfResHit);
				
# else	
				Status=EFI_UNSUPPORTED;
#endif						
				break;

            case isPciGetSetupConfig:
                PCI_TRACE((TRACE_PCI," (isPciGetSetupConfig); " ));

				Status = GetIioRootPortIndex (dev, &SocketIndex, &IioRootPortIndex);
				if (!EFI_ERROR(Status) && (dev->PciExpress != NULL) ) {
					UINT8   MaxPayload = gIioSystemProtocol->IioGlobalData->SetupData.PcieMaxPayload[(SocketIndex * NUMBER_PORTS_PER_SOCKET) + IioRootPortIndex];

					// Map RC MaxPayload to AMI MaxPayload Setup question
					// IIO RC Options: 0 - 128, 1 - 256, 2 - Auto
					// AMI RC Options: 0 - 128, 1 - 256, 55 - Auto
					if ( MaxPayload == 2 ) {
						MaxPayload = PCI_SETUP_AUTO_VALUE;
					}
					dev->PciExpress->Pcie1Setup.MaxPayload = MaxPayload;
					Status = EFI_SUCCESS;
				} else {
	                Status=EFI_UNSUPPORTED;
				}
                break;
				
			default:
				Status=EFI_UNSUPPORTED;
				PCI_TRACE((TRACE_PCI," (!!!isPciMaxStep!!!); " ));
	    }//switch
	    
	    return Status;
}

VOID GetNumberofSegements()
{
	
	UINT8 SocketIndex = 0 ;
	
	for (SocketIndex = 1 ; SocketIndex < IIoUds->IioUdsPtr->SystemStatus.numCpus ; SocketIndex++)
	{
		if (IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].PcieSegment != IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex -1].PcieSegment)
		{
			NumberOfAvailableSegments = NumberOfAvailableSegments + 1;
		}
	}
	
}


EFI_STATUS RootBrgInit (
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)
{
//Update Standard parameter block
    UINTN                           Index;
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_INIT_STEP                   InitStep=(PCI_INIT_STEP)Args->InitStep;
    PCI_ROOT_BRG_DATA               *rb=(PCI_ROOT_BRG_DATA*)Args->Param1;
    EFI_STATUS                      Status=EFI_UNSUPPORTED;
    UINTN                           StackIndex = 0, SocketIndex = 0;
    UINTN                           StackResIndex = 0, SocketResIndex = 0;
    BOOLEAN                         RbAslNameMatchFound = FALSE;
#if FPGA_SUPPORT
    UINTN							FpgaSocketIndex = 0;
    BOOLEAN							FpgaRbAslNameMatchFound = FALSE;
    FPGA_CONFIGURATION  			FpgaConfiguration;
#endif

//---------------------------------
 	static UINTN 				NumberofRb = 0;
 //	static UINT8 				BufferIndex = 0;
 	
 	
 	Status = pBS->LocateProtocol(
 	 					&gEfiIioUdsProtocolGuid,
 	 					NULL,
 	 					&IIoUds);
 	 	ASSERT_EFI_ERROR (Status);
 	 	if (EFI_ERROR(Status))
 	 		return Status;
 	
 	

 	if (NumberofRb == 0)
 	{
 		for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) 
 		{
 			if (!IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
 			{
 				continue;
 			}
 			for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
 			{
 	           if (!((IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
 	           {
 	                continue;
 	           }
 					
 	          NumberofRb++;
 			} // for loop for Stack
 		} // for loop for Socket
 		
 		NumberofStacksPerCpu = (UINT8)(NumberofRb / IIoUds->IioUdsPtr->SystemStatus.numCpus);
 	}
 	
 	 if (!FoundNoofSegments)
 	 {
 		 GetNumberofSegements();
 		FoundNoofSegments = TRUE;
 	 }


 	for (Index =0; Index < MAX_SOCKET*MAX_IIO_STACK; Index++) //Purley has six root bridges under each Socket/IIO
 	{
	    if (Strcmp(rb->RbAslName,rbindexloc[Index].RbAslDevName) == 0 )
	    {
	        StackResIndex = rbindexloc[Index].Index%MAX_IIO_STACK; //within each IIO socket ID, the stack resources are indexed 0 to 5.
	        SocketResIndex = rbindexloc[Index].Index/MAX_IIO_STACK;
	        RbAslNameMatchFound = TRUE;
	        break;
	    }
 	}

#if FPGA_SUPPORT
 	for (Index = 0; Index < (MAX_SOCKET*2); Index++)
 	{
	    if (Strcmp(rb->RbAslName, Fpgarbindexloc[Index].RbAslDevName) == 0 )
	    {
	    	SocketResIndex = (Fpgarbindexloc[Index].Index/2);
	    	StackResIndex = rbindexloc[Index].Index%2;
	    	FpgaSocketIndex = (Fpgarbindexloc[Index].Index/2) + 2;
	        FpgaRbAslNameMatchFound = TRUE;
	        break;
	    }
 	}
#endif
#if FPGA_SUPPORT
 	Status = FpgaConfigurationGetValues (&FpgaConfiguration);
#endif

 	switch (InitStep)
 	{
	//-------------------------------------------------------------------------
		case isRbCheckPresence:
	                  PCI_TRACE((TRACE_PCI," (isRbCheckPresence); " ));
	                  if ( RbAslNameMatchFound && 
	                       (IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].Valid) && 
	                       ((IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketResIndex].stackPresentBitmap) & (1 << StackResIndex)) ) {
	                      rb->NotPresent = FALSE;
                      
	                      if (((IIoUds->IioUdsPtr->PlatformData.PciExpressSize/1024/1024)) > 256 )
	                      {
	                    	 
	                    	  BufferIndex = TempBufferIndex / NumberofStacksPerCpu;
	                    	  BufferIndex = BufferIndex/ (IIoUds->IioUdsPtr->SystemStatus.numCpus /NumberOfAvailableSegments);
                   			  Buffer[BufferIndex] = (UINTN)PcdGet64 (PcdPciExpressBaseAddress) + IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketResIndex].PcieSegment * PCIEX_LENGTH;
	                      }
	                      
	                      else
	                      {
	                    	  Buffer[0] = (UINTN)PcdGet64 (PcdPciExpressBaseAddress);
	                      }
	                      TempBufferIndex ++ ;

	                      if (TempBufferIndex == NumberofRb )
	                      {
	                    	  
	                    		  NumberOfSegments = NumberOfAvailableSegments;
	                    		  PcdSet32(AmiPcdMaxSegmentSupported,NumberOfSegments);
	                    		  SizeOfBuffer = NumberOfSegments*sizeof(UINT64);
	                    		  PcdSetPtr(AmiPcdPcieBaseAddressBuffer,&SizeOfBuffer, (VOID*)&Buffer);
	                    		  gAmiPcieBaseAddressArray= (UINT64*)PcdGetPtr(AmiPcdPcieBaseAddressBuffer);
	                      }
	                      
	                  } else {
	                  PCI_TRACE((TRACE_PCI,"RB not present\n "));
	                      rb->NotPresent = TRUE;
	                  }
#if FPGA_SUPPORT
	                  if ( FpgaRbAslNameMatchFound && 
	                       (IIoUds->IioUdsPtr->SystemStatus.FpgaPresentBitMap) & (1 << FpgaSocketIndex)) {
	                	  if ((!Strcmp(rb->RbAslName, "VKT0")) || (!Strcmp(rb->RbAslName, "VKT1")) || (!Strcmp(rb->RbAslName, "VKT2")) || (!Strcmp(rb->RbAslName, "VKT3")))
	                	  {
	                		  PCI_TRACE((TRACE_PCI,"FPGA RB Skipped\n "));
	                		  rb->NotPresent = TRUE;		
	                	  }
	                	  else
	                	  {
	                		  rb->NotPresent = FALSE;
	                	  }
	                  }
#endif
	            break;
			
		break;
	//-------------------------------------------------------------------------
		case isRbBusUpdate:
		{
			ASLR_QWORD_ASD                    *res = (ASLR_QWORD_ASD*)Args->Param2;
			PCI_TRACE((TRACE_PCI," (isRbBusUpdate); " ));
			res->_MIN = IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].StackRes[StackResIndex].BusBase;
			res->_MAX =IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].StackRes[StackResIndex].BusLimit;
#if FPGA_SUPPORT
			if (FpgaConfiguration.FpgaSktActive & (1 << SocketResIndex)) {
				if (StackResIndex == IIO_PSTACK3) {
					res->_MAX = IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].StackRes[StackResIndex].BusLimit - FPGA_PREAllOCATE_BUS_NUM;
				}
			}
#endif
			res->_LEN = (res->_MAX - res->_MIN) + 1;				
			rb->RbIoProtocol.SegmentNumber = IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketResIndex].PcieSegment;
			PCI_TRACE((TRACE_PCI,"\n(SegmentNumber = %d); \n",rb->RbIoProtocol.SegmentNumber ));

#if FPGA_SUPPORT
			if (FpgaRbAslNameMatchFound) {
				if (StackResIndex == 0) {
					res->_MIN = IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].StackRes[IIO_PSTACK3].BusLimit - FPGA_PREAllOCATE_BUS_NUM + 0x01;
					res->_MAX = IIoUds->IioUdsPtr->PlatformData.IIO_resource[SocketResIndex].StackRes[IIO_PSTACK3].BusLimit;
				}
				else {
					res->_MIN = IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[FpgaSocketIndex].SocketFirstBus;
					res->_MAX = IIoUds->IioUdsPtr->PlatformData.CpuQpiInfo[FpgaSocketIndex].SocketLastBus;	
				}
				res->_LEN = (res->_MAX - res->_MIN) + 1;
			}
#endif
		}
		break;
	//-------------------------------------------------------------------------
 	}//switch
//---------------------------------
 	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS OnBoardVgaInit(
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)

{
	//Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO   					*dev=(PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS  					Status=EFI_UNSUPPORTED;
//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

    //INTEL IGD GOP driver fails if it uses address above 4G.
    //So force IGD to use only Below 4G Address Space...
    if(InitStep==isPciGetSetupConfig){
//    	dev->DevSetup.Decode4gDisable=TRUE;
//    	Status=EFI_SUCCESS;
    }
    
    return Status;	
    
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: PciPortSkipDeviceInit
//
// Description: 
//  Skips the specified device from PCI enumeration. Bus, device and 
//  function numbers of the device are used to skip device.
//
// Input:
//  IN       AMI_BOARD_INIT_PROTOCOL  *This           - Pointer to an instance
//                                                      of AMI_BOARD_INIT_PROTOCOL.
//  IN       UINTN                    *Function       - Pointer to the function
//                                                      number from which it is
//                                                      called.
//  IN  OUT  VOID                     *ParameterBlock - Contains a block of
//                                                      parameters of routine.
//
// Output:  
//     EFI_STATUS
//       EFI_SUCCESS             When device is called with skip device and matches
//                               specific bus, device and function numbers of device.
//       EFI_UNSUPPORTED         When device is not called with skip device or is
//                               not matched with specific bus, device and function
//                               numbers of device.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:   This function will be called from LaunchInitRoutine in PciBus.c with
//          arguments for the device. Currently this function skips SMBUS and SPI.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS PciPortSkipDeviceInit (
  IN      AMI_BOARD_INIT_PROTOCOL     *This,
  IN      UINTN                       *Function,
  IN OUT  VOID                        *ParameterBlock
)
{
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args = (AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_DEV_INFO                    *Dev = (PCI_DEV_INFO*)Args->Param1;
    
    if(Args->InitStep == isPciSkipDevice) {

        //
        // Skip SMBUS device from PCI Bus enumeration.
        //
        if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
            (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SMBUS) && \
            (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SMBUS)) {
            return EFI_SUCCESS;
        }
        
        //
        // Skip SPI device from PCI Bus enumeration.
        //
        if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
            (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SPI) && \
            (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SPI)) {
            return EFI_SUCCESS;
        }

        //
        // Skip P2SB device from PCI Bus enumeration.
        //
        if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
            (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_P2SB) && \
            (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_P2SB)) {
            return EFI_SUCCESS;
        }
    }
    return EFI_UNSUPPORTED;
}

EFI_STATUS RaidGetDriver(
	IN EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL *This,
	IN OUT EFI_HANDLE *DriverImageHandle
)
{
	EFI_STATUS			Status = EFI_UNSUPPORTED;
    UINTN       		Index, RaidBusOverrideMaxHandles;
    UINTN			PciRead8=0;
    BOOLEAN			ArrayFound = FALSE;
    PCI_BUS_OVERRIDE_DATA	*ovr=(PCI_BUS_OVERRIDE_DATA*)This;
    PCI_DEV_INFO                *Dev=ovr->Owner;
    EFI_HANDLE 			*ImageHandleArray = NULL;

    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Start \n"));
    
    //
    // Validate the input parameters
    //
    if (DriverImageHandle == NULL) {
        return EFI_INVALID_PARAMETER;
    } 
    
    Status = Dev->PciIo.Pci.Read(&Dev->PciIo, EfiPciIoWidthUint8, 0x0A, 1, &PciRead8);	
    if(EFI_ERROR(Status)){
	    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": No PciIo \n"));
	    return EFI_UNSUPPORTED;
    }
    
    if (PciRead8 != PCI_CLASS_MASS_STORAGE_RAID){
	    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Not Mass Storage \n"));
	    return EFI_UNSUPPORTED;
    }
    
    
    
    if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && 
		    (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SATA) && 
		    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SATA)) {
	    if(gSataGuidCount != 0) ArrayFound = TRUE;
	    ImageHandleArray = gSataImageHandleArray;
	    RaidBusOverrideMaxHandles = gSataGuidCount;
    }
    
    if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && 
		    (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SSATA) && 
		    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SSATA)) {
	    if(gsSataGuidCount != 0) ArrayFound = TRUE;
	    ImageHandleArray = gsSataImageHandleArray;
	    RaidBusOverrideMaxHandles = gsSataGuidCount;
    }
    
    if(ArrayFound) {
	    //
	    // Image handle found
	    //    
	    
	    //
	    //If the *DriverImageHandle is NULL , return the first Imagehandle
	    //    
	    if( *DriverImageHandle == NULL ) {
		    if(ImageHandleArray[0] != NULL) {
			    *DriverImageHandle = ImageHandleArray[0];
			    return EFI_SUCCESS;
		    }
	    } else {
		    // If *DriverImageHandle not NULL , return the next Imagehandle 
		    // from the available image handle list 
		    //
		    for (Index = 0; Index < RaidBusOverrideMaxHandles - 1; Index++) {
			    if( *DriverImageHandle == ImageHandleArray[Index] && (ImageHandleArray[Index+1] != NULL) ) {
				    *DriverImageHandle = ImageHandleArray[Index+1];
				    return EFI_SUCCESS;
			    }
		    }
	    }
    }
    //Finished with checking handle array
    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Finish \n"));
    return EFI_INVALID_PARAMETER;
 
}

EFI_STATUS GetSataAhciHandles(PCI_DEV_INFO *dev,
		EFI_GUID	*GuidList,
		EFI_HANDLE	*ImageHandleArray,
		UINT8		*GuidCount){
	EFI_STATUS			Status = EFI_UNSUPPORTED;
	UINTN       			HandleCount;
	EFI_HANDLE  			*HandleBuffer=NULL;
	UINTN       			Index;
	EFI_DRIVER_BINDING_PROTOCOL 	*DriverBindingProtocol=NULL;
	EFI_GUID        		gEfiLoadedImageGuid  = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_LOADED_IMAGE_PROTOCOL   	*LoadedImage;
	
	gLoadedImageDone = TRUE;
	
	//
	// Locate all the driver binding protocols
	//
	Status = pBS->LocateHandleBuffer (
			ByProtocol,
			&gEfiDriverBindingProtocolGuid,
			NULL,
			&HandleCount,
			&HandleBuffer
			);
	
	if (EFI_ERROR(Status)) {
		return Status;
	}
	
	for (Index = 0; Index < HandleCount; Index++) {
		
		//
		// Get the Driver Binding Protocol Interface
		//
		Status = pBS->HandleProtocol( HandleBuffer[Index], 
				&gEfiDriverBindingProtocolGuid, 
				&DriverBindingProtocol );
		
		if(EFI_ERROR(Status) || DriverBindingProtocol == NULL){
			continue;
		}
		
		//
		// Get the LoadedImage Protocol from ImageHandle
		//
		Status = pBS->HandleProtocol( DriverBindingProtocol->ImageHandle, 
				&gEfiLoadedImageGuid, 
				&LoadedImage );
		
		if(EFI_ERROR(Status)){
			continue;
		}
		
		//
		//Compare the File guid with driver's needs to launched first
		//
		if(guidcmp(&(((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(LoadedImage->FilePath))->FvFileName), \
				&GuidList[*GuidCount]) != 0) {
			continue;
		}
		
		//
		// Driver Image handle found. Add it in the Array
		//    
		ImageHandleArray[*GuidCount] = DriverBindingProtocol->ImageHandle;
		(*GuidCount)++;
		
		//
		// Start from the beginning
		//
		Index = -1;
		
		//
		// Check All the Guid's are found. If found break the loop
		//
		
		if(*GuidCount >= (sizeof(GuidList)/sizeof(EFI_GUID) -1 )) {
			break;
		}   
	}
	
	//Finally add embedded ROM
	if(dev->EfiRomCount>0){
		ImageHandleArray[*GuidCount] = dev->EfiRomImages[0]->RomImgHandle;
		(*GuidCount)++;
	}
		
	//
	// Free the HandleBuffer Memory.
	//
	if (HandleBuffer) {
		pBS->FreePool (HandleBuffer);
	}
	
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: DxeSmmReadyToLockEventCallBack
//
// Description: 
//  This is the Event call back function to notify the library functions that the 
//	system is entering the SmmLockd phase.
//
// Input:
//  IN       EFI_EVENT  Event           - Pointer to this event
//  IN       VOID                    *Context       - Event handler private data

// Output:  
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI
DxeSmmReadyToLockEventCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS   Status;
  VOID         *Interface;

  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }
  
  // Set global variable
  gDxeSmmReadyToLock = TRUE;
}

#if defined SECURITY_SUPPORT && SECURITY_SUPPORT
EFI_STATUS SatasSataBootScriptSave(PCI_DEV_INFO *Dev){
//    EFI_STATUS		Status;
    UINT8             	PciRead8=0;
    UINT16             	PciRead16=0;
    UINT32             	PciRead32=0;
    UINT8		i;
    
    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Saving Boot Scripts on Device: %X \n", Dev->Address.Addr.Device));
    
    //
    // SATA/sSATA, Reg#74h PMCS
    //
    S3PciRead16(PCI_LIB_ADDRESS(Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, 0x74));
    
    //
    // SATA/sSATA, Reg#90h MAP
    //
    S3PciRead8(PCI_LIB_ADDRESS(Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, 0x90));
    
    //
    // SATA/sSATA, Reg#94h PCS
    //
    S3PciRead8(PCI_LIB_ADDRESS(Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, 0x94));
    
    //
    // SATA/sSATA, Reg#10h~24h BARs
    //
    for (i = 0x10; i <= 0x24; i += 4) {
	    S3PciRead32(PCI_LIB_ADDRESS(Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, i));
    }
    
    //
    // SATA/sSATA, Reg#04h CMD
    //
    S3PciOr8(PCI_LIB_ADDRESS(Dev->Address.Addr.Bus, Dev->Address.Addr.Device, Dev->Address.Addr.Function, 0x04), 0x7);

    return EFI_SUCCESS;    
}
#endif		

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: SataDeviceInit
//
// Description: 
//  Saves the boot scripts for AHCI devices and loads the Bus Override Protocol for RAID devices.
//
// Input:
//  IN       AMI_BOARD_INIT_PROTOCOL  *This           - Pointer to an instance
//                                                      of AMI_BOARD_INIT_PROTOCOL.
//  IN       UINTN                    *Function       - Pointer to the function
//                                                      number from which it is
//                                                      called.
//  IN  OUT  VOID                     *ParameterBlock - Contains a block of
//                                                      parameters of routine.
//
// Output:  
//     EFI_STATUS
//       EFI_SUCCESS             
//       EFI_UNSUPPORTED         When device is not called with set attribute device or is
//                               not matched with specific bus, device and function
//                               numbers of RAID device.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:   This function will be called from LaunchInitRoutine in PciBus.c with
//          arguments for the device.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SataDeviceInit (
  IN      AMI_BOARD_INIT_PROTOCOL     *This,
  IN      UINTN                       *Function,
  IN OUT  VOID                        *ParameterBlock
)
{
    EFI_STATUS				Status;
    AMI_BOARD_INIT_PARAMETER_BLOCK  	*Args = (AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_DEV_INFO                    	*Dev = (PCI_DEV_INFO*)Args->Param1;
    UINT8				PciRead8=0;
    AHCI_PLATFORM_POLICY_PROTOCOL       *AhciPlatformPolicy = NULL;
    EFI_HANDLE                     	Handle        = NULL;
        
    if(Args->InitStep == isPciProgramDevice) {

	    if (((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && 
			    (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SATA) && 
			    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SATA)) ||
			    ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && 
			    (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SSATA) &&
			    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SSATA))){
		    
		    Status = Dev->PciIo.Pci.Read(&Dev->PciIo, EfiPciIoWidthUint8, 0x0A, 1, &PciRead8);	
		    if(EFI_ERROR(Status)){
			    return EFI_UNSUPPORTED;
		    } 
		    
		    //
		    //Check for AHCI and RAID modes.
		    //		    
			#if A_S3 && (defined SECURITY_SUPPORT && SECURITY_SUPPORT)
			if (gDxeSmmReadyToLock == FALSE){
				SatasSataBootScriptSave(Dev);
			}
			#endif	
		    if (PciRead8 == PCI_CLASS_MASS_STORAGE_RAID) {
			    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Installing OnboardRaidGuid on Device: %X \n", Dev->Address.Addr.Device));
			    Status = pBS->InstallProtocolInterface(
					    &Dev->Handle,
					    &gAmiOnboardRaidControllerGuid,
					    EFI_NATIVE_INTERFACE,
					    NULL);
			    ASSERT_EFI_ERROR(Status);
			    
			    //Check for UEFI
			    if(Dev->BusOvrData.BusOverride.GetDriver!=NULL){
				    if ((Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SATA) && 
						    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SATA)){
					    //Get SATA Handle Array
					    Status = GetSataAhciHandles(Dev, gSataGuidListCheckForRaid, gSataImageHandleArray, &gSataGuidCount);
				    }
				    if ((Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SSATA) && 
						    (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SSATA)){
					    //Get SATA Handle Array
					    Status = GetSataAhciHandles(Dev, gsSataGuidListCheckForRaid, gsSataImageHandleArray, &gsSataGuidCount);
				    }
				    
				    // Get the Ahci Platform Policy Protocol
				    Status=pBS->LocateProtocol( &gAmiAhciPlatformPolicyProtocolGuid,
				                                NULL,
				                                (VOID **)&AhciPlatformPolicy );

				    if(EFI_ERROR(Status)) {
				        // If the Ahci Platform policy protocol not found, initialize with RAID value
					    Status=pBS->InstallProtocolInterface(
							    &Handle,
							    &gAmiAhciPlatformPolicyProtocolGuid,
							    EFI_NATIVE_INTERFACE,
							    &gRAIDAhciPlatformPolicy);
				    }
				    
				    //Replace Bus Specific Override Get Driver		    
				    Dev->BusOvrData.BusOverride.GetDriver=RaidGetDriver;
			    }
		    }
		    return EFI_SUCCESS;                       
	    }
    }
    return EFI_UNSUPPORTED;
}

EFI_STATUS PciPortDegradeUsbBar (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
)
{
	
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status   = EFI_UNSUPPORTED;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo    = dev->RbIo;
    UINTN                           BarCount;
    UINT8                           DegradeUsbBar = 0;
    

    if (InitStep == isPciQueryDevice) {

        if ( (RbIo->SegmentNumber == PCH_PCI_DEVICES_SEGMENT_NO) && (dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
                (dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_XHCI) && (dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_XHCI) ) {

            Status = GetOptionData(&gEfiPchRcVariableGuid, OFFSET_OF (PCH_RC_CONFIGURATION, PchUsbDegradeBar), &DegradeUsbBar, sizeof (DegradeUsbBar));
            if ( !EFI_ERROR(Status) && (DegradeUsbBar == 1) ) {

                for(BarCount = 0; BarCount < PCI_MAX_BAR_NO; BarCount++) {
                    if ( dev->Bar[BarCount].Type == tBarMmio64 ) { // Checking the allocate type.

                        TRACE((TRACE_ALWAYS,"\n Assigning resources below 4 GB for USB as per RC request \n"));
                        dev->Bar[BarCount].Type = tBarMmio32;
                    } else if ( dev->Bar[BarCount].Type == tBarMmio64pf) {

                        TRACE((TRACE_ALWAYS,"\n Assigning resources below 4 GB for USB as per RC request \n"));
                        dev->Bar[BarCount].Type = tBarMmio32pf;
                    }
                }
            }
        }
    } // isPciQueryDevice
    
    return Status;
}

//APTIOV_SERVER_OVERRIDE_RC_START - To fix issue with issue in booting 10 GbE legacy PXE
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: PciPortDegrade10GbEBar
//
// Description: 
//  Forces 10 GbE Bar into 32 bit.
//
// Input:
//  IN       AMI_BOARD_INIT_PROTOCOL  *This           - Pointer to an instance
//                                                      of AMI_BOARD_INIT_PROTOCOL.
//  IN       UINTN                    *Function       - Pointer to the function
//                                                      number from which it is
//                                                      called.
//  IN  OUT  VOID                     *ParameterBlock - Contains a block of
//                                                      parameters of routine.
//
// Output:  
//     EFI_STATUS
//       EFI_SUCCESS             
//       EFI_UNSUPPORTED         When device is not called with set attribute device, does not 
//				 match with specific bus, device and function
//                               numbers of 10 GbE device, or Force10GbE32BitResource token is disabled.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:   This function will be called from LaunchInitRoutine in PciBus.c with
//          arguments for the device.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PciPortDegrade10GbEBar (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
)
{
	
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status   = EFI_UNSUPPORTED;
    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Start \n"));
    
    if (InitStep == isPciGetSetupConfig) {
        if (FORCE_32BIT_RESOURCES_TO_ONBOARD_10GBE == 1){
        	PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Disable 4g Decode \n"));
        	dev->DevSetup.Decode4gDisable = TRUE;
        }
    } // isPciGetSetupConfig
    
    return Status;
}
//APTIOV_SERVER_OVERRIDE_RC_END - To fix issue with issue in booting 10 GbE legacy PXE

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: PciBoardDxeInitInitialize
//
// Description: 
//  Constructor to initialize library.
//
// Input:
//  IN       EFI_HANDLE  		ImageHandle
//  IN       EFI_SYSTEM_TABLE           *SystemTable

// Output:  
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
RETURN_STATUS
EFIAPI
PciBoardDxeInitInitialize (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  VOID                           *Registration;
  EFI_EVENT                      Event;
  
  //
  // Create event to notify the library system entered the SmmLocked phase.
  //
  Event = EfiCreateProtocolNotifyEvent  (
		  &gEfiDxeSmmReadyToLockProtocolGuid,
		  TPL_CALLBACK,
		  DxeSmmReadyToLockEventCallBack,
		  NULL,
		  &Registration
		  );
  ASSERT (Event != NULL);
  
  return EFI_SUCCESS;
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
