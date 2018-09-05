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

/** @file UsbPort.c
    AMI USB Porting file

**/

#include <Token.h>
#include <Setup.h>
#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"

#if defined(CSM_SUPPORT) && CSM_SUPPORT		//(EIP69136+)
#include <Protocol/LegacyBiosExt.h>
#endif										//(EIP69136+)

#if USB_DIFFERENTIATE_IDENTICAL_DEVICE_NAME
static UINT8   DeviceNumber=1;
#endif

#if  USB_RT_DRIVER
//-----------------------------------------------------------------------------
// This part is linked with USBRT and located in SMI

/**
    This table contains list of vendor & device IDs of USB
    devices that are non-compliant. This is currently used
    only for USB mass storage devices but can be extended to
    other type of non-compliant devices also.

**/

USB_BADDEV_STRUC    gUsbBadDeviceTable[] = {
// Intel, Lacie hard disk
    {0x059f, 0xa601,
        0, 0, PROTOCOL_CBI_NO_INT,
        0},

// In-systems ATA bridge
    {0x05ab, 0x0060,
        0, 0, 0,
        USB_INCMPT_START_UNIT_NOT_SUPPORTED},

// Data Store Technologies, USB 2 ATA bridge
    {0x04e6, 0x0001,
        BASE_CLASS_MASS_STORAGE, 0, PROTOCOL_CBI_NO_INT,
        USB_INCMPT_BOT_STATUS_FAILED},

// NEC, Floppy drive
    {0x0409, 0x0040,
        0, 0, PROTOCOL_CBI_NO_INT,
        0},

// Hana flash drive
    {0x090a, 0x1001,
        0, 0, 0, \
        USB_INCMPT_SINGLE_LUN_DEVICE +
        USB_INCMPT_MODE_SENSE_NOT_SUPPORTED},

// Compact Flash reader
    {0x04e6, 0x000a,
        0, 0, 0,
        USB_INCMPT_MODE_SENSE_NOT_SUPPORTED},

// ScanLogic SL11R-IDE and Ennyah RW4420U
    {0x04ce, 0x0002,
        0, SUB_CLASS_SL11R, PROTOCOL_BOT,
        0},

// BAFO Slim CDR-W BF-2100
    {0x09cc, 0x0404,
        0, 0, PROTOCOL_CBI,
        0},

//Panasonic USB CD/RW Model:KXL-RW21AN
    {0x04da, 0x0d06,
        BASE_CLASS_MASS_STORAGE, 0, PROTOCOL_CBI,
        0},

    {0x04da, 0x0d07,
        BASE_CLASS_MASS_STORAGE, 0, PROTOCOL_CBI,
        0},

//TaiDen Technology:CoolFlash
    {0x0ea0, 0x6803,
        0, 0, 0,
        USB_INCMPT_MODE_SENSE_NOT_SUPPORTED},

// A-Bit USB Mouse(Model:97M32U)
    {0x0605, 0x0001,
        0, 0, 0,
        USB_INCMPT_SET_BOOT_PROTOCOL_NOT_SUPPORTED},

// The problem with this card reader is fixed elsewhere
// HP 4-in-1 Card reader (does not support Read Format Capacity command)
//  {0x6050, 0x0034,
//      0, 0, 0,
//      USB_INCMPT_FORMAT_CAPACITY_NOT_SUPPORTED},

// Silicon Motion Inc., Taiwan: USB Flash Disk
    {0x090c, 0x1000,
        0, 0, 0,
        USB_INCMPT_GETMAXLUN_NOT_SUPPORTED},

//Intel KVM HID
    {0x8086, 0x2b,
        0, 0, 0,
        USB_INCMPT_HID_DATA_OVERFLOW},

// SANWA Supply MA-LS11DS USB Mouse
    {0x04fc, 0x0801,
        0, 0, 0,
        USB_INCMPT_BOOT_PROTOCOL_IGNORED},
										//(EIP75441+)>
// SanDisk 2GB Sansa Clip+ MP3 Player
	{0x0781, 0x74D0,
		0xFF, 0xFF, 0xFF,
		0},

// SanDisk Sansa Fuze 4GB Flash Portable Media Player
	{0x0781, 0x74C2,
		0xFF, 0xFF, 0xFF,
		0},
										//<(EIP75441+)
// Genovation USB Mini-Terminal Model #904-RJ
	{0x16C0, 0x0604,
		0, 0, 0,
		USB_INCMPT_DISABLE_DEVICE},
#ifdef USB_BAD_DEVICES
	USB_BAD_DEVICES
#endif
// End of incompatible device list
    {0, 0, 0, 0, 0, 0}
};

UINT16 gKbcSetTypeRate11CharsSec=KEY_REPEAT_DELAY;
UINT16 gKbcSetTypeDelay500MSec=KEY_REPEAT_INITIAL_DELAY;
extern  USB_GLOBAL_DATA  *gUsbData;  //EIP58029

DEV_INFO*   USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);

/**
    This function intends to skip port.
  
**/ 

BOOLEAN 
OEMSkipList (
    UINT8       HubAddr,
    UINT8       PortNum, 	
	UINT16      Bdf,
	UINT8       BaseClass,
    UINT8       VaildBaseClass
)
{
                                        //(EIP88776+)>
    USB_SKIP_LIST   *UsbSkipListTable;
    DEV_INFO        *TmpDevInfo;
    UINT8           TablePortNumber; 
    UINT8           ConnectDeviceNumber;
	UINT8           ConnectPortNumber;	
    UINT16          i; 
    UINT16          TableLevel; 
    UINT16          ConnectLevel;
    UINT32          TablePath; 
    UINT32          ConnectPath;
    UINT32          ConnectPathMask;
 
    UsbSkipListTable = (USB_SKIP_LIST*) gUsbData->gUsbSkipListTable;

    if (UsbSkipListTable == NULL) {
        return FALSE;
    }
	
    for ( ; (UsbSkipListTable->wBDF != 0); UsbSkipListTable++) {
        TablePortNumber = UsbSkipListTable->bRootPort;
        TablePath = UsbSkipListTable->dRoutePath;
        for (i = 1; i < 5; i++, TablePath >>= 4) {
            if (TablePath == 0) {
                break;
            }
		}

        TablePath = UsbSkipListTable->dRoutePath;
        TableLevel = i;
        USB_DEBUG(DEBUG_INFO, 4, "TableLevel = %x TablePath = %x TablePortNumber = %x BDF = %x\n",
                TableLevel,TablePath,TablePortNumber, UsbSkipListTable->wBDF);

        //get connect path and level
        ConnectDeviceNumber = HubAddr; 
        ConnectPortNumber = PortNum;		
        for (i = 1, ConnectPath = 0; i < 5; i++) {	
            if (ConnectDeviceNumber & BIT7) {
                break;
            }
            ConnectPath = ConnectPath << 4; 
            ConnectPath |= ConnectPortNumber;

            // Get the device info structure for the matching device address
            TmpDevInfo = USB_GetDeviceInfoStruc(
                                        USB_SRCH_DEV_ADDR, 
                                        0, 
                                        ConnectDeviceNumber, 
                                        0);
            if (TmpDevInfo == NULL) {
                break;
            }
            ConnectDeviceNumber = TmpDevInfo->bHubDeviceNumber;
            ConnectPortNumber   = TmpDevInfo->bHubPortNumber;             
        }
        ConnectLevel = i; 
        USB_DEBUG(DEBUG_INFO, 4, "ConnectLevel = %x ConnectPath = %x ConnectPortNumber = %x BDF = %x\n",
                    ConnectLevel, ConnectPath, ConnectPortNumber, Bdf);

        //Skip by all 
        if (UsbSkipListTable->bSkipType == SKIP_FOR_ALLCONTROLLER) {
            if (UsbSkipListTable->bFlag == SKIP_FLAG_SKIP_LEVEL) {
                if (TableLevel != ConnectLevel) {
                    continue;
                }
            }
			
            if (UsbSkipListTable->bBaseClass == 0) {
                return TRUE;
            }
			
            if (UsbSkipListTable->bBaseClass == BaseClass) {
                return TRUE;
            }
			
			continue;
		}

        if (UsbSkipListTable->wBDF != Bdf) {
            continue;
        }

        //Skip by controller
        if (UsbSkipListTable->bRootPort == 0) {
            if (UsbSkipListTable->bBaseClass != 0) {         
                if (UsbSkipListTable->bBaseClass != BaseClass) {
                    continue;
                } else {
                    return TRUE;                    
                }
            }
            return TRUE;                                
        }
        //Skip usb ports which include down stream ports.
        if (UsbSkipListTable->bFlag == SKIP_FLAG_SKIP_MULTI_LEVEL) {
            ConnectPathMask = 0xFFFFFFFF >> (4 * (8 - (TableLevel)));
            ConnectPathMask = ConnectPathMask >> 4;
            if (((ConnectPath & ConnectPathMask) == TablePath) && (ConnectPortNumber == TablePortNumber)){
                if (UsbSkipListTable->bBaseClass != 0) {
                    if (UsbSkipListTable->bBaseClass == BaseClass) {
                        return TRUE;
                    }
                    continue;
                }
                return TRUE;
            }
            continue;
        }
        
        if (TableLevel != ConnectLevel) {
            continue;
        }
		//Skip usb ports on the same level.
        if (UsbSkipListTable->bFlag == SKIP_FLAG_SKIP_LEVEL) {
            if (UsbSkipListTable->bBaseClass == 0) {
                return TRUE;
            }
			
            if (UsbSkipListTable->bBaseClass == BaseClass) {
                return TRUE;
            }
        }
        for (i = 0; i < TableLevel; i++, ConnectPath >>= 4, TablePath >>= 4) {
            if (i == (TableLevel - 1)) { 
                switch (UsbSkipListTable->bFlag) {
                    case SKIP_FLAG_SKIP_PORT:
                        if ((ConnectPath == TablePath) && (ConnectPortNumber == TablePortNumber)){
                            if (UsbSkipListTable->bBaseClass != 0) {
                                if (UsbSkipListTable->bBaseClass == BaseClass) {
                                    return TRUE;
                                }
                                break;
                            }
                            return TRUE;
                        }
                        break;						
                    case SKIP_FLAG_KEEP_PORT:
                        if (ConnectPortNumber == TablePortNumber) {
                            if (ConnectPath == TablePath) {
                                if (UsbSkipListTable->bBaseClass != 0) {
                                    if (VaildBaseClass == 1) {
                                        if (UsbSkipListTable->bBaseClass != BaseClass) {
                                            return TRUE;
                                        }
                                    }
                                }
                                break;                                
                            }
                            return TRUE;
                        }
                        break;
                    default:
                        break;
                }                
            } else {
                if ((ConnectPath & 0xf) != (TablePath & 0xf)) {
                    break;
                }
            }
        }
    }
	
    return FALSE;
                                        //<(EIP88776+)
}

/**
    This function provides Oem to check for non-compliant 
    USB devices and updates the data structures appropriately 
    to support the device.

    @param  HCStruc - HCStruc pointer
            DevInfo - Device information structure pointer
            Desc    - Pointer to the descriptor structure
            DescLength - End offset of the device descriptor

    @retval Updated DevInfo->wIncompatFlags field

**/

VOID
UsbOemCheckNonCompliantDevice(
	HC_STRUC*	HCStruc,
	DEV_INFO*	DevInfo,
	UINT8*		Desc,
	UINT16		Length,
	UINT16		DescLength
)
{
}

// End of SMI part of USBPORT.C
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
// This part is linked with UHCD and located outside SMI
extern  USB_GLOBAL_DATA  *gUsbData;  //EIP58029

EFI_GUID gEfiSetupGuid = SETUP_GUID;

UINT8 gFddHotplugSupport = USB_HOTPLUG_FDD;
UINT8 gHddHotplugSupport = USB_HOTPLUG_HDD;
UINT8 gCdromHotplugSupport = USB_HOTPLUG_CDROM;
UINT8 gUsbMassNativeEmulation = USB_MASS_EMULATION_NATIVE;
UINT8 UsbMassEmulationForNoMedia = USB_MASS_EMULATION_FOR_NO_MEDIA;	//(EIP86793+)
                                        //(EIP80382+)>
#ifndef USB_MASS_EMULATION_BY_SIZE
#define USB_MASS_EMULATION_BY_SIZE 0
#endif
UINT8 UsbMassSizeEmulation = USB_MASS_EMULATION_BY_SIZE;
                                        //<(EIP80382+)
#ifndef REMOVE_CHECK_FOR_USB_FLOPPY_DRIVE
#define REMOVE_CHECK_FOR_USB_FLOPPY_DRIVE 0
#endif
UINT8 gUsbMassMediaCheck = REMOVE_USB_STORAGE_FROM_BBS_IF_NO_MEDIA;
UINT8 gUsbMassSkipFddMediaCheck = REMOVE_CHECK_FOR_USB_FLOPPY_DRIVE;
UINT8 gUsbEfiMsDirectAccess = USB_EFIMS_DIRECT_ACCESS;
UINT8 SkipCardReaderConnectBeep = SKIP_CARD_READER_CONNECT_BEEP_IF_NO_MEDIA;    //(EIP64781+)

//USB_SUPPORT_SETUP           gSetupData;					//(EIP99882-)

//(EIP51653+)>
/**
    This function returns a name string of connected mass storage
    device.

    @param SkipStruc - Pointer to a skip list to be filled
        TotalStruc - Size of the data to copy to a buffer
    @retval VOID

**/

VOID
EFIAPI
UsbGetSkipList(
    USB_SKIP_LIST	*SkipStruc,
    UINT8			TotalStruc
)
{
	EFI_STATUS  Status;

	Status = gBS->AllocatePool(
			EfiRuntimeServicesData,
			(TotalStruc + 1) * sizeof(USB_SKIP_LIST),
			&(gUsbData->gUsbSkipListTable));
    
    if (EFI_ERROR(Status)) {
        return;
    }
    
    gBS->SetMem(gUsbData->gUsbSkipListTable, (TotalStruc + 1) * sizeof(USB_SKIP_LIST), 0);
	gBS->CopyMem(gUsbData->gUsbSkipListTable, SkipStruc, TotalStruc * sizeof(USB_SKIP_LIST));

	return;
}
//<(EIP51653+) 

/**
    This function is called from the UHCD entry point, HcPciInfo
    can be updated here depending on the platform and/or chipset
    requirements.

    @param VOID

    @retval EFI_STATUS Updating succeeded / failed

**/

EFI_STATUS
UpdateHcPciInfo()
{
    return EFI_UNSUPPORTED;
}


/**
    This procedure return specific USB host controller index and
    port number for BIOS to give specific mass storage device
    have highest boot priority.

    @param VOID

    @retval 
        EFI_SUCCESS         USB boot device assigned
        UsbHcIndx       USB host index (1-based)
        UsbHubPortIndx  USB hub port index (1-based)
        EFI_UNSUPPORTED     No USB boot device assigned
        EFI_INVALID_PARAMETER   UsbHcIndx or UsbHubPortIndx are NULL

**/

EFI_STATUS
EFIAPI
OemGetAssignUsbBootPort(
    UINT8   *UsbHcIndx,
    UINT8   *UsbHubPortIndx
)
{
    if (UsbHcIndx == NULL || UsbHubPortIndx == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
/*
    // The code below is the sample implementation that reports Port#3 of HC#7
    // to be a port of boot priority device
    *UsbHcIndx = 7;
    *UsbHubPortIndx = 3;

    return EFI_SUCCESS;
*/
    return EFI_UNSUPPORTED;
}


/**
    This is porting function that fills in USB related fields in
    gSetupData variable according to the setup settings and OEM
    policy.

    @param Pointers to USB data, Boot Services and Runtime Services

    @retval The status of gSetupData initialization

**/

EFI_STATUS
InitUsbSetupVars (
    USB_GLOBAL_DATA         *UsbData
)
{

    UINTN               VariableSize;
    USB_MASS_DEV_NUM    MassDevNum;
    EFI_STATUS          Status;
    UINT8               Index;
    USB_MASS_DEV_VALID  MassDevValid= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    UINT32              VariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    USB_CONTROLLER_NUM  UsbControllerNum;

#if USB_SETUP_VARIABLE_RUNTIME_ACCESS
    VariableAttributes |= EFI_VARIABLE_RUNTIME_ACCESS;
#endif

    UsbControllerNum.UhciNum = 0;
    UsbControllerNum.OhciNum = 0;
    UsbControllerNum.EhciNum = 0;
    UsbControllerNum.XhciNum = 0;
    VariableSize = sizeof(UsbControllerNum);
    
    Status = gRT->SetVariable(L"UsbControllerNum",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &UsbControllerNum);

    MassDevNum.UsbMassDevNum = 0;
    MassDevNum.IsInteractionAllowed = 1;
    VariableSize = sizeof(MassDevNum);

    Status = gRT->SetVariable(L"UsbMassDevNum",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &MassDevNum);

    //
    // Initilize the Variable to 0
    //        
    VariableSize = sizeof(MassDevValid);
    Status = gRT->SetVariable( L"UsbMassDevValid",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &MassDevValid );

    if (UsbData == NULL) {
        return EFI_SUCCESS;
    }

    VariableSize = sizeof(USB_SUPPORT_SETUP);
    Status = gRT->GetVariable( L"UsbSupport", &gEfiSetupGuid, NULL,
                        &VariableSize, &UsbData->UsbSetupData);
    if (EFI_ERROR(Status)) {
        // Set default values and save "UsbSupport" variable.
        gBS->SetMem(&UsbData->UsbSetupData, sizeof(UsbData->UsbSetupData), 0);

		UsbData->UsbSetupData.UsbMainSupport = 1;

        if (gFddHotplugSupport) {
            UsbData->UsbSetupData.UsbHotplugFddSupport = SETUP_DATA_HOTPLUG_AUTO;
        }
        if (gHddHotplugSupport) {
            UsbData->UsbSetupData.UsbHotplugHddSupport = SETUP_DATA_HOTPLUG_AUTO;
        }
        if (gCdromHotplugSupport) {
            UsbData->UsbSetupData.UsbHotplugCdromSupport = SETUP_DATA_HOTPLUG_AUTO;
        }

        UsbData->UsbSetupData.UsbMassResetDelay = SETUP_DATA_RESETDELAY_20S;
        UsbData->UsbSetupData.UsbControlTimeOut = 20;      //(EIP30079+)
		UsbData->UsbSetupData.UsbXhciSupport = 1;
		UsbData->UsbSetupData.UsbHiSpeedSupport = 1;
        UsbData->UsbSetupData.UsbMassDriverSupport = 1;

        VariableAttributes |= EFI_VARIABLE_NON_VOLATILE;

        Status = gRT->SetVariable(
                    L"UsbSupport",
                    &gEfiSetupGuid,
                    VariableAttributes,
                    sizeof(UsbData->UsbSetupData),
                    &UsbData->UsbSetupData );
    }

    if (gFddHotplugSupport) {
        UsbData->fdd_hotplug_support = UsbData->UsbSetupData.UsbHotplugFddSupport;
    }
    if (gHddHotplugSupport) {
        UsbData->hdd_hotplug_support = UsbData->UsbSetupData.UsbHotplugHddSupport;
    }
    if (gCdromHotplugSupport) {
        UsbData->cdrom_hotplug_support = UsbData->UsbSetupData.UsbHotplugCdromSupport;
    }
	UsbData->UsbXhciHandoff = UsbData->UsbSetupData.UsbXhciHandoff;
    UsbData->UsbEhciHandoff = UsbData->UsbSetupData.UsbEhciHandoff;
    UsbData->UsbOhciHandoff = UsbData->UsbSetupData.UsbOhciHandoff;
    UsbData->UsbEmul6064 = UsbData->UsbSetupData.UsbEmul6064;
    UsbData->UsbMassResetDelay = UsbData->UsbSetupData.UsbMassResetDelay;
    for (Index=0; Index<16; Index++) {
        UsbData->USBMassEmulationOptionTable[Index] =
                        *((UINT8*)&UsbData->UsbSetupData.UsbEmu1+Index);
    }
    if (UsbData->UsbSetupData.UsbLegacySupport == 1) {
        UsbData->dUSBStateFlag |= USB_FLAG_DISABLE_LEGACY_SUPPORT;
    }
    UsbData->wTimeOutValue = UsbData->UsbSetupData.UsbControlTimeOut*1000 ;   //(EIP30079+)
	UsbData->UsbXhciSupport = UsbData->UsbSetupData.UsbXhciSupport;
	UsbData->UsbHiSpeedSupport = UsbData->UsbSetupData.UsbHiSpeedSupport;

    UsbData->PowerGoodDeviceDelay =
        (UsbData->UsbSetupData.PowerGoodDeviceDelay == 0)? 0 : UsbData->UsbSetupData.PowerGoodDeviceNumDelay;

    return EFI_SUCCESS;
}


/**
    Returns the status of "USB legacy support" question from Setup.

    @param VOID

    @retval 0 Enable, 1 - Disable, 2 - Auto

**/

UINT8 UsbSetupGetLegacySupport()
{
    return gUsbData->UsbSetupData.UsbLegacySupport;			//(EIP99882)
}


/**
    Updates "UsbMassDevNum" setup variable according to the number
    of installed mass storage devices.

**/

EFI_STATUS
UpdateMassDevicesForSetup()
{
    EFI_STATUS  Status;
    UINTN       VariableSize;
    CONNECTED_USB_DEVICES_NUM   devs;
    USB_MASS_DEV_NUM    SetupData;
    UINT8       MassDevValid[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    UINT8       i;
    UINT32      VariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;

#if USB_SETUP_VARIABLE_RUNTIME_ACCESS
    VariableAttributes |= EFI_VARIABLE_RUNTIME_ACCESS;
#endif

    VariableSize = sizeof(SetupData);
    Status = gRT->GetVariable( L"UsbMassDevNum",
        &gEfiSetupGuid,
        NULL,
        &VariableSize,
        &SetupData );

    if (Status == EFI_SUCCESS) {
        if (!SetupData.IsInteractionAllowed) return EFI_SUCCESS;
    }

    ReportDevices(&devs);

    SetupData.UsbMassDevNum = devs.NumUsbMass;

    Status = gRT->SetVariable( L"UsbMassDevNum",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &SetupData );

    //
    // Based on avilable USB mass storage device, set the device avilable flag
    //
    for(i=0;i<devs.NumUsbMass;i++) {
        MassDevValid[i]=1;
    }

    VariableSize = sizeof(USB_MASS_DEV_VALID);

    Status = gRT->SetVariable( L"UsbMassDevValid",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &MassDevValid );

    return Status;
}

/**
    Returns maximum device slots to be enabled and programmed
    in MaxSlotsEn field of XHCI CONFIG register. Valid range
    is 1...HCPARAMS1.MaxSlots (see xhci.h for details)

    @retval EFI_SUCCESS Valid value is reported in MaxSlots
    @retval EFI_UNSUPPORTED Function is not ported; MaxSlots will
        be used from HCPARAMS1 field.
    @note  Porting is optional

**/

EFI_STATUS
Usb3OemGetMaxDeviceSlots(
	IN	    HC_STRUC    *HcStruc,
    IN	OUT UINT8       *MaxSlots
)
{
    return EFI_UNSUPPORTED;
}


/*
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name:        XHCI_InterruptHandler
//
// Description:
//  Hardware interrupt handler
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
XHCI_InterruptHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
    EFI_TPL OriginalTPL;
    
    OriginalTPL = pBS->RaiseTPL (TPL_HIGH_LEVEL);
    
    gPic->EndOfInterrupt (gPic, gVector);
    
    pBS->RestoreTPL (OriginalTPL);
    USB_DEBUG(DEBUG_INFO, 3, "xhci interrupt..\n");
}
*/

VOID
EFIAPI
XhciTimerCallback(
    EFI_EVENT   Event,
    VOID        *Context
)
{
	EFI_TPL OriginalTPL;
    
    OriginalTPL = gBS->RaiseTPL (TPL_HIGH_LEVEL);

    // Execute XHCI_ProcessInterrupt using SW SMI, Context points to HC_STRUC
    UsbSmiHc(opHC_ProcessInterrupt, USB_HC_XHCI, Context);

	gBS->RestoreTPL (OriginalTPL);
}


/**
    Purpose of this function is to install event handlers for different
    USB host controllers.

    @param 
        Pointer to a host controller data structure

    @retval 
        EFI_SUCCESS on a successful handler installation, otherwise EFI_NOT_READY
  
    @note  
  1. Currently implemented for XHCI only. UHCI, OHCI and EHCI currently have the
  HW SMI registration routines in their HC Start functions.

  2. This function is a part of UHCD, not a part of USBRT (SMI). It may only have
  a code that installs a non-SMI event handler, e.h. timer callback function for
  HC event polling, or HW interrupt handler. SMI handlers are installed in the
  USBRT entry point (amiusb.c).

**/

EFI_STATUS
USBPort_InstallEventHandler(
    HC_STRUC *HcStruc
)
{
#if XHCI_SUPPORT
    EFI_STATUS  Status = EFI_SUCCESS;

	if (HcStruc->bHCType != USB_HC_XHCI) return EFI_UNSUPPORTED;
/*
#if XHCI_EVENT_SERVICE_MODE == 0
{
    EFI_EVENT   XhciTimerEvent;

    // Current HW does not support INTx method of interrupt, hold on to the handler
    // installation until HW matures. In the meantime, use timer interrupt callback.
    Status = pBS->CreateEvent(
            EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
            EFI_TPL_NOTIFY,
            XhciTimerCallback, HcStruc, &XhciTimerEvent
    );
    ASSERT_EFI_ERROR(Status);

    pBS->SetTimer (XhciTimerEvent, TimerPeriodic, USB_INTERRUPT_POLLING_PERIOD);
}
#endif
*/
/*
#if USBInt1C_SUPPORT
#if defined(CSM_SUPPORT) && CSM_SUPPORT		//(EIP69136+)

    // Update INT1C timer interrupt handler with the HcStruc data
    {
        UINT32  *ivt = (UINT32*)0;
        UINT32  Int1cVector;
        UINT32  Addr;
        UINT32  HcStrucAddr;
        UINT8   Count;
        EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions = NULL;

        // Find the vector hooked up by "USB Int1C". At this time only AMI OEM interrupts
        // might have trapped the original INT1C. So the trapped vector is located at EP-4
        // (EP is the Entry Point).
        //
        // For the details refer the INT1C data area in usbint1c.asm
		Status = pBS->LocateProtocol(
			&gEfiLegacyBiosExtProtocolGuid, NULL, &BiosExtensions);

		if (!EFI_ERROR(Status)) {
	        for (Count = 0, Int1cVector = ivt[0x1c];
	            ((Int1cVector >> 16) != 0xf000) && Count < 5;
	            Count++)
	        {
	            Addr = ((Int1cVector >> 12) & 0xffff0) + (Int1cVector & 0xffff);
	            // Check "$AMIUSB$" signature
	            if ((*(UINT32*)(UINTN)(Addr+2) == 0x494d4124) &&
	                (*(UINT32*)(UINTN)(Addr+6) == 0x24425355)) break;
	    
	            Int1cVector = *(UINT32*)(UINTN)(Addr - 4); // Trapped address
	        }
	        ASSERT(Count < 5);
	        if ((Count < 5) && ((Int1cVector >> 16) != 0xf000)) {
	            // Update HcStruc information
	            HcStrucAddr = Addr + 12;
	            Addr = (UINT32)(*(UINT16*)(UINTN)(Addr + 10));  // offset of the Params in URP
	            Addr += ((Int1cVector >> 12) & 0xffff0);        // added segment to create 32-bit address

	            BiosExtensions->UnlockShadow(0, 0, 0, 0);

	            *(UINTN*)(UINTN)HcStrucAddr = (UINTN)HcStruc;
	            *(UINTN*)(UINTN)Addr = (UINTN)HcStrucAddr;

	            BiosExtensions->LockShadow(0, 0);
	        }
		}
    }

#endif										//(EIP69136+)
#endif
*/

/*
    // Install HW interrupt handler on HcStruc->Irq IRQ level

    // Find the Legacy8259 protocol.  ASSERT if not found.
    Status = pBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, (VOID **) &gPic);
    if (!EFI_ERROR(Status)) {
        EFI_CPU_ARCH_PROTOCOL   *Cpu;

        // Get the interrupt vector number corresponding to IRQ0 from the 8259 driver
        Status = gPic->GetVector (gPic, HcStruc->Irq, (UINT8*)&gVector);
        ASSERT_EFI_ERROR (Status);
    
        // Find the CPU architectural protocol.  ASSERT if not found.
        Status = pBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &Cpu);
        ASSERT_EFI_ERROR (Status);

        // Install interrupt handler for XHCI controller
        Status = Cpu->RegisterInterruptHandler (Cpu, gVector, XHCI_InterruptHandler);
        ASSERT_EFI_ERROR (Status);
        gPic->EnableIrq (gPic, HcStruc->Irq, TRUE);

    }
*/

#endif
    return EFI_SUCCESS;
}

#endif

#if USB_DIFFERENTIATE_IDENTICAL_DEVICE_NAME
/**
    This function will insert the USB device number into the devicename string.
    Format----> "U(DeviceNumber)-DevNameString"

    @param 
        Pointer to a device data structure

         
    @retval VOID
  
**/
VOID
AddPortNumbertoDeviceString(
    DEV_INFO        *Device
)
{
    UINT8   i;
    UINT8   j;
    UINT8   TempArray[50];
    UINT8   Appendarray1[5] = {'U',NULL,'-',NULL};
    UINT8   Appendarray2[6] = {'U',NULL,NULL,'-',NULL};

    for (i = 0 ; Device->DevNameString[i]; i++) {
        TempArray[i] = Device->DevNameString[i];
    }
    TempArray[i] = 0;

    //
    // Check for device number, if <= 9 then convert to ASCII and insert in array,
    // else split the number and convert to ASCII and then insert in array.  
    //
    if (DeviceNumber <= 9) {
        Appendarray1[1] = DeviceNumber + 0x30;
        
        for (i = 0; Appendarray1[i]; i++) {
            Device->DevNameString[i] = Appendarray1[i];
        }
    } else {
        i = (DeviceNumber / 10) + 0x30;
        j = (DeviceNumber % 10) + 0x30;

        Appendarray2[1] = i;
        Appendarray2[2] = j;

        for (i = 0; Appendarray2[i] ; i++) {
            Device->DevNameString[i] = Appendarray2[i];
        }
    }

    for (j=0; TempArray[j]; j++, i++) {
         Device->DevNameString[i] = TempArray[j];
    }

    Device->DevNameString[i] = 0;

    DeviceNumber++;
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

