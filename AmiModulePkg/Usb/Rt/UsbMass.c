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

/** @file UsbMass.c
    AMI USB Mass Storage support implementation

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"
#include "UsbMass.h"
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#endif

extern  USB_GLOBAL_DATA *gUsbData;
extern  BOOLEAN gCheckUsbApiParameter;


VOID        USBMassInitialize(VOID);
UINT8       USBMassCheckForStorageDevice(DEV_INFO*, UINT8, UINT8, UINT8);
DEV_INFO*   USBMassConfigureStorageDevice(HC_STRUC*, DEV_INFO*,
                                        UINT8*, UINT16, UINT16);
UINT8       USBMassDisconnectStorageDevice(DEV_INFO*);
UINT16      USBMassSendCBICommand(DEV_INFO*, MASS_XACT_STRUC*);
UINT32      USBMassProcessBulkData(DEV_INFO*, MASS_XACT_STRUC*);
UINT8       USBMassConsumeBulkData(DEV_INFO*,UINT8,UINT16);
UINT32      USBMassIssueBOTTransaction(DEV_INFO*, MASS_XACT_STRUC*);
VOID        USBMassClearBulkEndpointStall(DEV_INFO*, UINT8);
VOID        USBMassBOTResetRecovery(DEV_INFO*);
UINT16      USBMassSendBOTCommand(DEV_INFO*, MASS_XACT_STRUC*);
UINT8       USBMassGetBOTStatus(DEV_INFO*, MASS_XACT_STRUC*);
UINT16      USBMassCBIGetStatus(DEV_INFO*);
UINT32      USBMassIssueCBITransaction(DEV_INFO*, MASS_XACT_STRUC*);
UINT8       USBMassReadCapacity10Command(DEV_INFO*);
UINT32      USBMassCheckDeviceReady(DEV_INFO*);
UINT32      USBMassRequestSense(DEV_INFO* fpDevInfo);
VOID        USBMassSenseKeyParsing(DEV_INFO* , UINT32);
MASS_INQUIRY        *USBMassInquiryCommand(DEV_INFO*);
UINT8       USBMassUpdateDeviceGeometry( DEV_INFO* fpDevInfo );
UINT16      USBMassBOTGetMaxLUN(DEV_INFO*);
UINT8       USBMassIdentifyDeviceType(DEV_INFO*, UINT8*);
UINT32      USBMassIssueBulkTransfer(DEV_INFO*, UINT8, UINT8*, UINT32);
VOID        iPodShufflePatch(MASS_GET_DEV_INFO*);
VOID        USBMassUpdateCylinderInfo(DEV_INFO*, UINT64);
UINT8       USBMassSetDefaultGeometry(DEV_INFO*, UINT64);
UINT8       USBMassValidatePartitionTable(MASTER_BOOT_RECORD*, UINT64, MBR_PARTITION*);
UINT16      USBMassSetDefaultType(DEV_INFO*, UINT64);
VOID        USBMassGetPhysicalDeviceType(DEV_INFO*, UINT8*);
UINT8       USB_SetAddress(HC_STRUC*, DEV_INFO*, UINT8);
UINT32      dabc_to_abcd(UINT32);
DEV_INFO*   USBGetProperDeviceInfoStructure(DEV_INFO*, UINT8);
UINT32      USBMassTestUnitReady(DEV_INFO*);
VOID        StoreUsbMassDeviceName(DEV_INFO*, UINT8*);
extern      VOID AddPortNumbertoDeviceString(DEV_INFO*);
UINT8       USBMassGetConfiguration(DEV_INFO*);

VOID* USB_MemAlloc (UINT16);
DEV_INFO* USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);
MASS_INQUIRY* USBMassGetDeviceParameters(DEV_INFO*);

UINT8 USB_MemFree  (VOID*,  UINT16);
VOID FixedDelay(UINTN);    
VOID SpeakerBeep (UINT8, UINT16, HC_STRUC*); //(EIP64781+)

static char* IOMegaZIPString        = "IOMEGA  ZIP";
#define IOMegaZIPStringLength       11

static char* MSysDiskOnKeyString    = "M-Sys   DiskOnKey";
#define MSysDiskOnKeyStringLength   17

BOOLEAN CheckDeviceLimit(UINT8);

VOID
USBMassFillDriverEntries (DEV_DRIVER    *fpDevDriver)
{
    fpDevDriver->bDevType               = BIOS_DEV_TYPE_STORAGE;
    fpDevDriver->bProtocol              = 0;
    fpDevDriver->pfnDeviceInit          = USBMassInitialize;
    fpDevDriver->pfnCheckDeviceType     = USBMassCheckForStorageDevice;
    fpDevDriver->pfnConfigureDevice     = USBMassConfigureStorageDevice;
    fpDevDriver->pfnDisconnectDevice    = USBMassDisconnectStorageDevice;
}

/**
    This function initializes mass storage device related data

**/

VOID
USBMassInitialize ()
{
    //
    // Set default value for the delay. Selections are: 20,40,60,80 for 10,20,30,40 sec.
    //
    gUsbData->bUSBStorageDeviceDelayCount = (gUsbData->UsbMassResetDelay + 1)*10;

}

/**
    This routine checks for hub type device from the
    interface data provided

    @param bBaseClass  USB base class code
        bSubClass   USB sub-class code
        bProtocol   USB protocol code

    @retval BIOS_DEV_TYPE_STORAGE type on success or 0FFH on error

**/

UINT8
USBMassCheckForStorageDevice (
    DEV_INFO*   DevInfo,
    UINT8       BaseClass,
    UINT8       SubClass,
    UINT8       Protocol
)
{
    if (BaseClass != BASE_CLASS_MASS_STORAGE) {
        return USB_ERROR;
    }
										//(EIP99882+)>
    if (!gUsbData->UsbSetupData.UsbMassDriverSupport) {
        return USB_ERROR;
    }
										//<(EIP99882+)
//Skip USB mass storage devices enumeration when legacy is disabled
    if (gUsbData->dUSBStateFlag & USB_FLAG_DISABLE_LEGACY_SUPPORT) {
        if (LEGACY_USB_DISABLE_FOR_USB_MASS) {									//(EIP93469)
           return USB_ERROR;
        }
    }
    //
    // Base class is okay. Check the protocol field for supported protocols.
    // Currently we support CBI, CB and BOT protocols.
    //
    if ((Protocol != PROTOCOL_CBI) &&
        (Protocol != PROTOCOL_CBI_NO_INT) &&
        (Protocol != PROTOCOL_BOT)) {
        return USB_ERROR;
    }

    return BIOS_DEV_TYPE_STORAGE;
    
}

/**
    This function finds a free mass device info structure and
    returns the pointer to it

    @param VOID

    @retval Pointer to the Mass Device Info (0 on failure)
        The number mass storage DeviceInfo structure (0-based)

**/

DEV_INFO*
USBMassGetFreeMassDeviceInfoStruc(
	DEV_INFO	*DevInfo,
    UINT8		*Indx
)
{
    DEV_INFO*   Dev = &gUsbData->aDevInfoTable[1];
    UINT8       Count;
    UINT8       MassDevIndx = 0;
    
    for (Count = 0; Count < (MAX_DEVICES-1); Count++, Dev++) {
		if (!(Dev->Flag & DEV_INFO_VALID_STRUC)) {
			continue;
		}
        if (Dev->bDeviceType == BIOS_DEV_TYPE_STORAGE) {
            MassDevIndx++;
        }
        if (Dev == DevInfo) {
			break;
        }
    }
    if (Count == (MAX_DEVICES-1)) {
		return NULL;
    }
    *Indx = MassDevIndx;
    
    return Dev;
}


/**
    This function finds a free mass device info structure and
    copies the current mass device info structure into it

    @param Current mass device info structure

    @retval New mass device info

**/

DEV_INFO*
USBMassFindFreeMassDeviceInfo(
    DEV_INFO* Dev,
    UINT8 *EmulIndex
)
{
    UINT8		Indx = 0;
    DEV_INFO	*NewDev;

    // Get the free mass device info structure pointer
    NewDev = USBMassGetFreeMassDeviceInfoStruc(Dev, &Indx);

    if (NewDev == NULL) {
        return NULL;   // No free entry found.
    }

    // Get the emulation type setup question associated with this device
    ASSERT(Indx>0 && Indx<17);
    if ((Indx == 0) || (Indx > 16)) {
	    return NULL;
    }

    Dev->wEmulationOption = gUsbData->USBMassEmulationOptionTable[Indx-1];
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBMassFindFreeMassDeviceInfo-------- indx %d, emu %d\n", Indx, Dev->wEmulationOption);

    // Set default device type and emulation type to 0
    Dev->bStorageType = 0;
    Dev->fpLUN0DevInfoPtr = 0;
    Dev->Flag |= DEV_INFO_DEV_PRESENT;

    *EmulIndex = Indx-1;

    return Dev;
    
}

/**
    This function verifies the presence of logical units (LUN)
    in the USB mass device and creates appropriate device info
    structures for them

    @param fpDevInfo - Device information structure pointer
        bMaxLun - Maximum number of logical units present (non-ZERO)

    @retval USB_ERROR   On error
        USB_SUCCESS On successfull completion

**/

UINT8
USBMassCreateLogicalUnits(
    DEV_INFO*   DevInfo,
    UINT8       MaxLun,
    UINT8       EmulIndex
)
{
    UINT8           Lun;
    DEV_INFO*       NewDevInfo;
    MASS_INQUIRY    *Inq;

    for (Lun = 1; Lun <= MaxLun; Lun++) {

        if (CheckDeviceLimit(BASE_CLASS_MASS_STORAGE) == TRUE) {
            break;
        }
        //
        // Get the proper device info structure
        //
        NewDevInfo = USBGetProperDeviceInfoStructure(DevInfo, Lun);
        if (!NewDevInfo) {
            return USB_ERROR;
        }
        //
        // Check whether this device is reconnected by checking the
        // valid structure flag
        //
        if ((NewDevInfo->Flag & DEV_INFO_MASS_DEV_REGD)) {
            //
            // Indicate device as connected
            //
            NewDevInfo->Flag |= DEV_INFO_DEV_PRESENT;

			// Change the parent HC number and port number in the existing DEV_INFO
	        NewDevInfo->bHCNumber = DevInfo->bHCNumber;
	    	NewDevInfo->bHubDeviceNumber = DevInfo->bHubDeviceNumber;
			NewDevInfo->bHubPortNumber = DevInfo->bHubPortNumber;
			NewDevInfo->bEndpointSpeed = DevInfo->bEndpointSpeed;
			NewDevInfo->wEndp0MaxPacket = DevInfo->wEndp0MaxPacket;
	    	NewDevInfo->DevMiscInfo = DevInfo->DevMiscInfo;
	        NewDevInfo->bDeviceAddress = DevInfo->bDeviceAddress;
			NewDevInfo->bBulkInEndpoint = DevInfo->bBulkInEndpoint;
			NewDevInfo->wBulkInMaxPkt = DevInfo->wBulkInMaxPkt;
			NewDevInfo->bBulkOutEndpoint = DevInfo->bBulkOutEndpoint;
			NewDevInfo->wBulkOutMaxPkt = DevInfo->wBulkOutMaxPkt;
			NewDevInfo->IntInEndpoint = DevInfo->IntInEndpoint;
			NewDevInfo->IntInMaxPkt = DevInfo->IntInMaxPkt;
			NewDevInfo->bPollInterval = DevInfo->bPollInterval;
			NewDevInfo->fpLUN0DevInfoPtr = DevInfo;
        } else {    // This is different device, it was not reconnected
            //
            // Copy the old device info structure into the new one
            //
            CopyMem((UINT8*)NewDevInfo, (UINT8*)DevInfo, sizeof (DEV_INFO));
            NewDevInfo->bLUN  = Lun; // Change LUN number
            NewDevInfo->wEmulationOption = gUsbData->USBMassEmulationOptionTable[EmulIndex + Lun];
            ZeroMem(NewDevInfo->DevNameString, 64);
            //
            // Save the Lun0 device info pointer in the current LUN
            //
            NewDevInfo->fpLUN0DevInfoPtr  = DevInfo;

			//
			// The Lun0 device might have been already locked by the
			// bus (USBBUS.usbhc_on_timer), clear it for current LUN.
			//
			NewDevInfo->Flag &= ~DEV_INFO_DEV_BUS;

			Inq = USBMassGetDeviceParameters(NewDevInfo);
			ASSERT(Inq);
			StoreUsbMassDeviceName(NewDevInfo, (UINT8*)Inq + 8);
            if (NewDevInfo->bStorageType == USB_MASS_DEV_CDROM) {
                USBMassGetConfiguration(NewDevInfo);
            }
        }

		if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
            if (!(NewDevInfo->Flag & DEV_INFO_IN_QUEUE)) {
			    USB_SmiQueuePut(NewDevInfo);
                NewDevInfo->Flag |= DEV_INFO_IN_QUEUE;
            }
		}
    }

    return USB_SUCCESS;
}

VOID
StoreUsbMassDeviceName(
    DEV_INFO    *Device,
    UINT8       *Str
)
{
    UINT8   i;
    UINT8   j;
    UINT8   StrIndex;
    UINT8   StrIndexMax;

    for (i = 0; i < 64; i++) {
        if (Device->DevNameString[i] != 0) {
            return;
        }
    }

    for (i = 0, j = 0; i < 32; i++) {
        if (*Str == 0) {
            Str++; j++;     // supress leading zeroes
        }
    }

    // INQUIRY Data Format
    // The Vendor Identification field contains 8 bytes of ASCII data identifying the vendor of the product.
    // The Product Identification field contains 16 bytes of ASCII data as defined by the vendor.
    // The Product Revision Level field contains 4 bytes of ASCII data as defined by the vendor.
    
    switch (USB_MASS_STORAGE_DEVICE_NAME) {
        // Vendor Information + Product Identification + Product Revision Level
        case 0:
            StrIndexMax = 28;
            break;
        // Vendor Information + Product Identification
        case 1:
            StrIndexMax = 24;
            break;
        // Vendor Information
        case 2:
            StrIndexMax = 8;
            break;
        default:
            StrIndexMax = 28;
            break;            
    }

    for (i = 0, StrIndex = 0; (i < (32 - j)) && (StrIndex < StrIndexMax); i++, StrIndex++) {
        // supress spaces if more than one
        if ((i > 0) && (Device->DevNameString[i-1] == ' ') && (Str[StrIndex] == ' ')) {
            i--;
            continue;
        }
										//(EIP63706+)>
		// Filter out the character if it is invisible.
		if (((Str[StrIndex] != 0) && (Str[StrIndex] < 0x20)) || (Str[StrIndex] > 0x7E)) {
			i--;
			continue;
		}
										//<(EIP63706+)

        Device->DevNameString[i] = Str[StrIndex];
    }

    //
    // Add Device number to the USB device string
    //
#if USB_DIFFERENTIATE_IDENTICAL_DEVICE_NAME
    AddPortNumbertoDeviceString(Device);
#endif
}

/**
    This function checks an interface descriptor of a device
    to see if it describes a USB mass device.  If the device
    is a mass storage device,  then it is configured
    and initialized.

    @param pHCStruc    HCStruc pointer
        pDevInfo    Device information structure pointer
        pDesc       Pointer to the descriptor structure
        wEnd        End offset of the device descriptor

    @retval New device info structure, NULL on error

**/

DEV_INFO*
USBMassConfigureStorageDevice (
        HC_STRUC*   fpHCStruc,
        DEV_INFO*   fpDevInfo,
        UINT8*      fpDesc,
        UINT16      wStart,
        UINT16      wEnd)
{
    UINT8           bTemp;
    UINT16          wRetValue;
    ENDP_DESC       *fpEndpDesc;
    INTRF_DESC      *fpIntrfDesc;
    UINT8           bMaxLUN;
    DEV_INFO*       newDev;
    MASS_INQUIRY    *inq;
    BOOLEAN         checkFDDhotplug, checkCDROMhotplug, checkHDDhotplug;
    UINT8           EmulIndex;
    UINT8           i;                  //(EIP64781+)      

    wRetValue       = 0;
    bMaxLUN         = 0;

//
// Set fpDevInfo->bDeviceType.  This serves as a flag
// that indicates a usable interface has been found in the current
// configuration. This is needed so we can check for other usable interfaces
// in the current configuration (composite device) without trying to search
// in other configurations.
//
    fpDevInfo->bDeviceType      = BIOS_DEV_TYPE_STORAGE;
    fpDevInfo->fpPollTDPtr      = 0;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBMassConfigureDevice ....\n");

    bTemp = 0x03;       // bit 1 = Bulk In, bit 0 = Bulk Out

	fpDevInfo->bBulkOutEndpoint = 0;
	fpDevInfo->bBulkInEndpoint = 0;
	fpDevInfo->IntInEndpoint  = 0;

    fpIntrfDesc = (INTRF_DESC*)(fpDesc + wStart);
    fpDesc+=((CNFG_DESC*)fpDesc)->wTotalLength; // Calculate the end of descriptor block
    fpEndpDesc = (ENDP_DESC*)((char*)fpIntrfDesc + fpIntrfDesc->bDescLength);
    for( ;(fpEndpDesc->bDescType != DESC_TYPE_INTERFACE) && ((UINT8*)fpEndpDesc < fpDesc);
        fpEndpDesc = (ENDP_DESC*)((char*)fpEndpDesc + fpEndpDesc->bDescLength)){

        if(!(fpEndpDesc->bDescLength)) {
            break;  // Br if 0 length desc (should never happen, but...)
        }

        if( fpEndpDesc->bDescType != DESC_TYPE_ENDPOINT ) {
            continue;
        }

        if ((fpEndpDesc->bEndpointFlags & EP_DESC_FLAG_TYPE_BITS) ==
                EP_DESC_FLAG_TYPE_BULK) {   // Bit 1-0: 10 = Endpoint does bulk transfers
            if(!(fpEndpDesc->bEndpointAddr & EP_DESC_ADDR_DIR_BIT)) {
                //
                // Bit 7: Dir. of the endpoint: 1/0 = In/Out
                // If Bulk-Out endpoint already found then skip subsequent ones
                // on the interface.
                //
                if (bTemp & 1) {
                    fpDevInfo->bBulkOutEndpoint = (UINT8)(fpEndpDesc->bEndpointAddr
                                                        & EP_DESC_ADDR_EP_NUM);
                    fpDevInfo->wBulkOutMaxPkt = fpEndpDesc->wMaxPacketSize;
                    bTemp &= 0xFE;
					USB_DEBUG(DEBUG_INFO, 3, "bulk out endpoint addr: %x, max packet size: %x\n", 
						fpDevInfo->bBulkOutEndpoint, fpDevInfo->wBulkOutMaxPkt);
                }
            } else {
                //
                // If Bulk-In endpoint already found then skip subsequent ones
                // on the interface
                //
                if (bTemp & 2) {
                    fpDevInfo->bBulkInEndpoint  = (UINT8)(fpEndpDesc->bEndpointAddr
                                                        & EP_DESC_ADDR_EP_NUM);
                    fpDevInfo->wBulkInMaxPkt    = fpEndpDesc->wMaxPacketSize;
                    bTemp   &= 0xFD;
					USB_DEBUG(DEBUG_INFO, 3, "bulk in endpoint addr: %x, max packet size: %x\n", 
						fpDevInfo->bBulkInEndpoint, fpDevInfo->wBulkInMaxPkt);
                }
            }
        }

        //
        // Check for and configure Interrupt endpoint if present
        //
        if ((fpEndpDesc->bEndpointFlags & EP_DESC_FLAG_TYPE_BITS) !=
                EP_DESC_FLAG_TYPE_INT) {    // Bit 1-0: 10 = Endpoint does interrupt transfers
			continue;
        }

		if (fpEndpDesc->bEndpointAddr & EP_DESC_ADDR_DIR_BIT ) {
            fpDevInfo->IntInEndpoint = fpEndpDesc->bEndpointAddr;
            fpDevInfo->IntInMaxPkt = fpEndpDesc->wMaxPacketSize;
            fpDevInfo->bPollInterval = fpEndpDesc->bPollInterval;  
			USB_DEBUG(DEBUG_INFO, 3, "interrupt in endpoint addr: %x, max packet size: %x\n", 
				fpDevInfo->IntInEndpoint, fpDevInfo->IntInMaxPkt);
        }
    }

    //
    // Check the compatibility flag for LUN support
    //
    if (!(fpDevInfo->wIncompatFlags & USB_INCMPT_SINGLE_LUN_DEVICE)) {
        //
        // If it is a BOT device, get maximum LUN supported
        //
        if (fpDevInfo->bProtocol == PROTOCOL_BOT) {
            bMaxLUN = (UINT8)USBMassBOTGetMaxLUN(fpDevInfo);
        }
    }

    //
    // Check whether the device is already registered. If so, proceed with current
    // mass info structure
    //
    if (fpDevInfo->Flag & DEV_INFO_MASS_DEV_REGD) {
        newDev = fpDevInfo;
        
        goto UMCM_MassDeviceOkay;
    }

    // Find a new mass device info structure and copy the old one into the new one
    // Note: this is called before GetDeviceParameters because it sets up dev->wEmulationOption
    newDev = USBMassFindFreeMassDeviceInfo(fpDevInfo, &EmulIndex);

    if (newDev == NULL) goto UMCM_Error;
    fpDevInfo = newDev;

    inq = USBMassGetDeviceParameters(fpDevInfo);
    if (inq == NULL) {
        fpDevInfo->bDeviceType = 0;
        goto UMCM_Error;
    }

    //
    // Do not enumerate device if it is not a CD-ROM and has the block size different from 512 Bytes
    // EIP#15595, iPod nano makes POST hang.
    //
                                        //(EIP59738-)>
    //if ( fpDevInfo->bPhyDevType!=USB_MASS_DEV_CDROM ) {
    //    if( fpDevInfo->wBlockSize!=0x200 && fpDevInfo->wBlockSize!=0xFFFF && fpDevInfo->wBlockSize!=0 )
    //        goto UMCM_Error;
    //}
                                        //<(EIP59738-)
    StoreUsbMassDeviceName(fpDevInfo, (UINT8*)inq+8);

    // Check for the hotplug devices current status, install the new one if needed
    if ( !(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) ) {
        // Find out if FDD/HDD/CDROM hotplugging is a valid option
        checkFDDhotplug = ((gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            ((gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
                (gUsbData->NumberOfFDDs == 0))) &&
            !(BOOLEAN)(gUsbData->dUSBStateFlag & USB_HOTPLUG_FDD_ENABLED);

        checkHDDhotplug = ((gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            ((gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
                (gUsbData->NumberOfHDDs == 0))) &&
            !(BOOLEAN)(gUsbData->dUSBStateFlag & USB_HOTPLUG_HDD_ENABLED);

        checkCDROMhotplug = ((gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            ((gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
                (gUsbData->NumberOfCDROMs == 0))) &&
            !(BOOLEAN)(gUsbData->dUSBStateFlag & USB_HOTPLUG_CDROM_ENABLED);

        if ( checkFDDhotplug || checkCDROMhotplug || checkHDDhotplug ) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "connecting hotplug...");
//          inq = USBMassGetDeviceParameters(fpDevInfo);
//          if (inq == NULL) goto UMCM_Error;
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "devtype phy %d, emu %d...", fpDevInfo->bPhyDevType, fpDevInfo->bEmuType);

            if ( checkFDDhotplug &&
                (fpDevInfo->bStorageType == USB_MASS_DEV_ARMD) ) {
                newDev = &gUsbData->FddHotplugDev;
                gUsbData->dUSBStateFlag |= USB_HOTPLUG_FDD_ENABLED;
            }

            if ( checkHDDhotplug && (fpDevInfo->bEmuType == USB_EMU_HDD_ONLY) ) {
                newDev = &gUsbData->HddHotplugDev;
                gUsbData->dUSBStateFlag |= USB_HOTPLUG_HDD_ENABLED;
            }

            if ( checkCDROMhotplug && (fpDevInfo->bPhyDevType == USB_MASS_DEV_CDROM) ) {
                newDev = &gUsbData->CdromHotplugDev;
                gUsbData->dUSBStateFlag |= USB_HOTPLUG_CDROM_ENABLED;
            }

            fpDevInfo->Flag |= DEV_INFO_HOTPLUG;
            *newDev = *fpDevInfo;   // Copy device into DevInfo dedicated to hotplug
            fpDevInfo->Flag &= ~DEV_INFO_VALIDPRESENT; // Release fpDevInfo
            fpDevInfo = newDev;
        }
    }

UMCM_MassDeviceOkay:
    if ( (newDev->bEmuType == USB_EMU_FLOPPY_ONLY) ||
        (newDev->bEmuType == USB_EMU_FORCED_FDD) ) {
        gUsbData->NumberOfFDDs++;
    }

    if ( newDev->bEmuType == USB_EMU_HDD_ONLY ) {
        gUsbData->NumberOfHDDs++;
    }

//    if ( newDev->bPhyDevType == USB_EMU_HDD_OR_FDD ) {
    if ( newDev->bPhyDevType == USB_MASS_DEV_CDROM ) {
        gUsbData->NumberOfCDROMs++;
        USBMassGetConfiguration(newDev);
    }

    if (bMaxLUN) {
        USBMassCreateLogicalUnits(newDev, bMaxLUN, EmulIndex);
    }

    										//(EIP64781+)>
    if (gUsbData->dUSBStateFlag & USB_FLAG_SKIP_CARD_READER_CONNECT_BEEP) {        
        if ((newDev->bLastStatus & USB_MASS_MEDIA_PRESENT) ||
            newDev->bPhyDevType == USB_MASS_DEV_CDROM ||
            newDev->bPhyDevType == USB_MASS_DEV_FDD) {
            SpeakerBeep(4, 0x1000, fpHCStruc);
        } else if (bMaxLUN) {
            for(i = 1; i < MAX_DEVICES; i++) {
                if (gUsbData->aDevInfoTable[i].fpLUN0DevInfoPtr == newDev) {
                    if (gUsbData->aDevInfoTable[i].bLastStatus & USB_MASS_MEDIA_PRESENT) {
                        SpeakerBeep(4, 0x1000, fpHCStruc);
                        break;
                    }
                }
            }
        }
    }
										//<(EIP64781+)

    return  newDev;

UMCM_Error:
    //USB_AbortConnectDev(fpDevInfo);   //(EIP59579-)
    return NULL;
}


/**
    This function disconnects the storage device

    @param pDevInfo    Device info structure pointer

    @retval Nothing

**/

UINT8
USBMassDisconnectStorageDevice (
    DEV_INFO* DevInfo
)
{
//  USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USBMassDisconnectDevice ....  \n");

	DevInfo->bBulkOutEndpoint = 0;
	DevInfo->bBulkInEndpoint = 0;
	DevInfo->IntInEndpoint  = 0;

    if ((DevInfo->bEmuType == USB_EMU_FLOPPY_ONLY) ||
        (DevInfo->bEmuType == USB_EMU_FORCED_FDD)) {
        gUsbData->NumberOfFDDs--;
    }

    if (DevInfo->bEmuType == USB_EMU_HDD_ONLY) {
        gUsbData->NumberOfHDDs--;
    }

//    if ( newDev->bPhyDevType == USB_EMU_HDD_OR_FDD ) {
    if ( DevInfo->bPhyDevType == USB_MASS_DEV_CDROM ) {
        gUsbData->NumberOfCDROMs--;
    }

    if (DevInfo->Flag & DEV_INFO_HOTPLUG) {
        DevInfo->Flag &= ~DEV_INFO_HOTPLUG;
		if (DevInfo == &gUsbData->FddHotplugDev) {
            gUsbData->dUSBStateFlag &= ~USB_HOTPLUG_FDD_ENABLED;
		} else if (DevInfo == &gUsbData->HddHotplugDev) {
			gUsbData->dUSBStateFlag &= ~USB_HOTPLUG_HDD_ENABLED;
		} else if (DevInfo == &gUsbData->CdromHotplugDev) {
			gUsbData->dUSBStateFlag &= ~USB_HOTPLUG_CDROM_ENABLED;
		}
    }

    return USB_SUCCESS;
}

/**
    This function clears the mass transaction structure

**/

VOID
USBMassClearMassXactStruc(
    MASS_XACT_STRUC *MassXactStruc
)
{
    UINT8   i;
    UINT8*  Cleaner = (UINT8*)MassXactStruc;
    
    for (i = 0; i < sizeof (MASS_XACT_STRUC); i++ ) {
        *Cleaner++ = 0;
    }
}


/**
    This function clears the bulk endpoint stall by sending
    CLEAR_FEATURE command to bulk endpoints

    @param fpDevInfo   Pointer to DeviceInfo structure
        bDirec      Endpoint direction

    @retval VOID

**/

VOID
USBMassClearBulkEndpointStall(
    DEV_INFO*   DevInfo,
    UINT8       Direc
)
{
    UINT8           Shift;
    UINT16          EndPoint;
	HC_STRUC	    *HcStruc;
	HCD_HEADER	    *HcdDriver;
	EFI_STATUS      Status;

	HcStruc = gUsbData->HcTable[DevInfo->bHCNumber - 1];
	HcdDriver = &gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)];

    EndPoint = (UINT16)((DevInfo->bBulkInEndpoint) | BIT7);

    if (!(Direc & BIT7)) {
        EndPoint = DevInfo->bBulkOutEndpoint;
    }
    //
    // Issue clear port feature command
    //
	HcdDriver->pfnHCDControlTransfer(HcStruc, DevInfo, (UINT16)ENDPOINT_CLEAR_PORT_FEATURE, 
		EndPoint,(UINT16)ENDPOINT_HALT, 0, 0);

	//if (HcdDriver->pfnHCDClearEndpointState) {
	//	HcdDriver->pfnHCDClearEndpointState(HcStruc, DevInfo, (UINT8)EndPoint);
	//} else {
	
    //
    // Reset the toggle bit
    //
    Shift = (EndPoint & 0xF) - 1;

    if (Direc & BIT7) {
        DevInfo->wDataInSync &= ~((UINT16)(1 << Shift));
    } else {
        DevInfo->wDataOutSync &= ~((UINT16)(1 << Shift));
    }

    if (DevInfo->fpLUN0DevInfoPtr == NULL) {
        return;
    }
    
    Status = UsbDevInfoValidation(DevInfo->fpLUN0DevInfoPtr);
    
    if (EFI_ERROR(Status)) {
        return;
    }

    if (Direc & BIT7) {
        DevInfo->fpLUN0DevInfoPtr->wDataInSync &= ~((UINT16)(1 << Shift));
    } else {
        DevInfo->fpLUN0DevInfoPtr->wDataOutSync &= ~((UINT16)(1 << Shift));
    }
	//}

}

/**
    This function performs a mass storage transaction by
    invoking proper transaction protocol.

    @param Pointer to DeviceInfo structure
        stMassXactStruc
        pCmdBuffer  Pointer to command buffer
        bCmdSize    Size of command block
        bXferDir    Transfer direction
        fpBuffer    Data buffer far pointer
        dwLength    Amount of data to be transferred
        wPreSkip    Number of bytes to skip before data
        wPostSkip   Number of bytes to skip after data

    @retval Amount of data actually transferred

**/
UINT32
USBMassIssueMassTransaction(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
    UINT32      DataLength;
    HC_STRUC    *HcStruc;
    
    if ((DevInfo->bProtocol == PROTOCOL_CBI) ||
        (DevInfo->bProtocol == PROTOCOL_CBI_NO_INT)) {
        return USBMassIssueCBITransaction(DevInfo, MassXactStruc);
    }

    if (DevInfo->bProtocol == PROTOCOL_BOT) {
        
        // Block to process periodic list to prevent that we might send the wrong
        // command sequences to the same device.
        gUsbData->ProcessingPeriodicList = FALSE;
        
        DataLength = USBMassIssueBOTTransaction(DevInfo, MassXactStruc);
        
        // To process any pending periodic list.
        gUsbData->ProcessingPeriodicList = TRUE;
        HcStruc = gUsbData->HcTable[DevInfo->bHCNumber - 1];
        (*gUsbData->aHCDriverTable
            [GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDProcessInterrupt)(HcStruc);
        
        return DataLength;
    }

    return 0;

}

/**
    This function gets the USB mass device parameters such as
    max cylinder, head, sector, block size and

    @param Pointer to DeviceInfo structure

    @retval Pointer to the temp buffer, NULL on error

**/

MASS_INQUIRY*
USBMassGetDeviceParameters(
    DEV_INFO* DevInfo
)
{
    MASS_INQUIRY    *Inq;
    UINT8           i;
    UINT8           Status;

    for (i = 0; i < 2; i++) {
        Inq = USBMassInquiryCommand(DevInfo);
        if (Inq) {
            break;
        }
        if (!(gUsbData->bLastCommandStatus & USB_BULK_STALLED)) {
            break;
        }
    }
    
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "fpMassInquiry = %x\n", Inq);
    
    if (!Inq) {
        return NULL;
    }

    DevInfo->wBlockSize = 0xFFFF; // Clear the cached block size

    //
    // Find the device type and update the device type structure accordingly
    //
    Status = USBMassIdentifyDeviceType(DevInfo, (UINT8*)Inq);

    if (Status == USB_ERROR) {
        Inq = NULL;
    }
    
    return Inq;
    
}

/**
    This procedure check whether device return valid device name
    if no valid device name returned, assign default name for it

    @param Inquiry Data

**/
VOID ValidateDeviceName (
    MASS_INQUIRY *InqData
)
{
    static UINT8 DefaultName[] = "USB     Storage Device";
    UINT8 *Name = ((UINT8*)InqData) + 8;
    UINT8 *DefName = DefaultName;
    UINT8 Count;

    // check for a blank name
    if (*Name) return;

//  for (Count = 0; Count < 28; Count++) {
//      if (*(Name + Count)) return;  // Not blank
//  }

    // copy default name
    for (Count = 0; Count < sizeof(DefaultName); Count++) {
        *(Name + Count) = *(DefName + Count);
    }
}

/**
    This function fills and returns the mass get device info
    structure

    @param fpMassGetDevInfo    Pointer to the mass get info struc
        bDevAddr    USB device address of the device

    @retval USB_SUCCESS or USB_ERROR
        fpMassGetDevInfo    Pointer to the mass get info struc
        dSenseData  Sense data of the last command
        bDevType    Device type byte (HDD, CD, Removable)
        bEmuType    Emulation type used
        fpDevId     Far pointer to the device ID

    @note  Initially the bDevAddr should be set to 0 as input. This
          function returns the information regarding the first mass
          storage device (if no device found it returns bDevAddr as
          0FFh) and also updates bDevAddr to the device address of
          the current mass storage device. If no other mass storage
          device is found then the routine sets the bit7 to 1
          indicating current information is valid but no more mass
          device found in the system. The caller can get the next
          device info if bDevAddr is not 0FFh and bit7 is not set

**/

UINT8
USBMassGetDeviceInfo (
    MASS_GET_DEV_INFO *MassGetDevInfo
)
{
    DEV_INFO        *DevInfo;
    MASS_INQUIRY    *MassInq;
    UINT8           Dev = MassGetDevInfo->bDevAddr;

    //
    // Get the total number of Mass Storage Devices
    //
    MassGetDevInfo->bTotalMassDev = (UINT8)(UINTN)USB_GetDeviceInfoStruc(USB_SRCH_DEV_NUM,
                            0, BIOS_DEV_TYPE_STORAGE, 0);

    if (Dev == 0) {
        iPodShufflePatch(MassGetDevInfo);
    }

    if (Dev & BIT7) {
        return USB_ERROR;  // Check for device address validity
    }

    //
    // If bDev = 0 then get information about first mass storage device
    //
    if (!Dev) {
        DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_TYPE, 0, BIOS_DEV_TYPE_STORAGE, 0);
        //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Get Mass0 info: %x\n", DevInfo);

        if (!DevInfo) {   // Set as no more device found
            MassGetDevInfo->bDevAddr  = 0xFF;
            return USB_SUCCESS;
        }
    } else {  //  Not the first mass device
        //
        // Get the device info structure for the matching device index
        //
        DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, Dev, 0);
        ASSERT(DevInfo);
        if ( (!DevInfo) || (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT)) ) {   // Error
            return USB_ERROR;
        }
        //
        // Get device info structure for next device
        //
        DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_TYPE, DevInfo, BIOS_DEV_TYPE_STORAGE, 0);
        ASSERT(DevInfo);
        if (!DevInfo) {   // Error. Exit !
            return USB_ERROR;
        }
    }
    MassInq = USBMassGetDeviceParameters(DevInfo);

    if (!MassInq) {
        return USB_ERROR;
    }

    MassGetDevInfo->bDevType = DevInfo->bPhyDevType;
//  MassGetDevInfo->bPhyDevType = fpDevInfo->bPhyDevType;
    MassGetDevInfo->bEmuType = DevInfo->bEmuType;
    MassGetDevInfo->wPciInfo =
        gUsbData->HcTable[DevInfo->bHCNumber - 1]->wBusDevFuncNum;
    MassGetDevInfo->fpDevId = (UINT32)(UINTN)((UINT8*)MassInq + 8);
//  MassGetDevInfo->fpDevId = USBMassAdjustIdString((UINT32)MassInq + 8);

    Dev = (UINT8)(UINTN)USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, DevInfo, 0, 0);
    ASSERT(Dev);

    Dev |= BIT7;   // Assume that no more mass device present

    //
    // Check whether more mass device is present
    //
    if (USB_GetDeviceInfoStruc(USB_SRCH_DEV_TYPE, DevInfo, BIOS_DEV_TYPE_STORAGE, 0)) {
        Dev &= ~BIT7;
    }

    DevInfo->Flag |= DEV_INFO_MASS_DEV_REGD;
    MassGetDevInfo->bDevAddr = Dev;

    *(UINTN*)MassGetDevInfo->Handle = *(UINTN*)DevInfo->Handle;

    return USB_SUCCESS;
}

/**
    This check whether iPod shuffle attached to system and move
    iPod shuffle to first initial device.

    @param Pointer to the mass get info struc

    @retval VOID

    @note  Attaching iPod shuffle and iPod mini to system causes BIOS POST
          stop. iPod shuffle must be initialized as early as possible.
          iPod mini cosumes about 2 seconds to complete initialization,
          init iPod shuffle first to fix problem.

**/

VOID
iPodShufflePatch(
    MASS_GET_DEV_INFO *MassGetDevInfo
)
{
    //TO BE IMPLEMENTED
}


/**
    Get USB MassStorage device status. Include Media Informarion.
    Refer to USB_MASS_MEDIA_XXX in USBDEF.H

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR or USB_SUCCESS

**/

UINT8
USBMassGetDeviceStatus  (
    MASS_GET_DEV_STATUS *MassGetDevSts
)
{
    DEV_INFO*   DevInfo;
    UINT8       DevAddr = MassGetDevSts->bDevAddr;
    UINT8       LastStatus;

    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);

    ASSERT(DevInfo != NULL);
    if (DevInfo == NULL) {
        return USB_ERROR;
    }

    LastStatus= DevInfo->bLastStatus;

    USBMassCheckDeviceReady(DevInfo);

    // When the Media is not present in the drive and insert the Media
    // it's just sends the status as Media Present. So check the last status
    // and if the media not present and current stauts is media present
    // report it as Media changed
    if ((LastStatus ^ DevInfo->bLastStatus) & USB_MASS_MEDIA_PRESENT) {
        // Report the Last Status along with Media Changed status
        DevInfo->bLastStatus |= USB_MASS_MEDIA_CHANGED;
    }

    MassGetDevSts->bDeviceStatus = DevInfo->bLastStatus;

    if (DevInfo->bLastStatus & USB_MASS_MEDIA_CHANGED) {
        //
        // Clear Media Change Status.
        //
        DevInfo->bLastStatus &= (UINT8)(~USB_MASS_MEDIA_CHANGED);
    }
    return USB_SUCCESS;
}

/**
    This function issues the command/data sequence provided
    as input.  This function can be used to send raw data
    to the USB mass storage device

    @param fpDevInfo   Pointer to Device Info structure
        fpMassCmdPassThru   Pointer to the mass command pass
        through structure
        bDevAddr        USB device address of the device
        dSenseData      Sense data of the last command
        fpCmdBuffer     Far pointer to the command buffer
        wCmdLength      Command length
        fpDataBuffer    Far pointer for data buffer
        wDataLength     Data length
        bXferDir        Data transfer direction

    @retval USB_SUCCESS or USB_ERROR
        dSenseData      Sense data of the last command
        fpDataBuffer    Updated with returned data if the transfer
        is an IN transaction

**/

UINT8
USBMassCmdPassThru (
    MASS_CMD_PASS_THRU  *MassCmdPassThru
)
{
    UINT8           *CmdBuffer;
    UINT8           *Src;
    UINT8           *Dst;
    DEV_INFO        *DevInfo;
//  UINT8           CommandRetried = FALSE;
    UINT8           CmdBlkSize;
    UINT8           Count;
    UINT16          Data16;
    UINT32          Data32;
    UINT8           DevAddr = MassCmdPassThru->bDevAddr;
    MASS_XACT_STRUC MassXactStruc;

    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);
    if ( (!DevInfo) || (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT)) ) {   // Error
        return USB_ERROR;
    }

    CmdBlkSize = (UINT8)((MassCmdPassThru->wCmdLength +
                USB_MEM_BLK_SIZE - 1) >> USB_MEM_BLK_SIZE_SHIFT);

    //
    // Check whether the drive is ready for read TOC command
    //
    USBMassCheckDeviceReady(DevInfo);

    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = USB_MemAlloc((UINT8)GET_MEM_BLK_COUNT(CmdBlkSize));
    if (!CmdBuffer) {
        return USB_ERROR;
    }

    //
    // Copy the command into (just allocated) mass command buffer
    //
    Src = (UINT8*)(UINTN)MassCmdPassThru->fpCmdBuffer;
    Dst = CmdBuffer;
    for (Count = 0; Count < MassCmdPassThru->wCmdLength; Count++) {
        *Dst++ = *Src++;
    }

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = CmdBuffer;
    MassXactStruc.bCmdSize = (UINT8)MassCmdPassThru->wCmdLength;
    MassXactStruc.bXferDir = MassCmdPassThru->bXferDir;
    MassXactStruc.fpBuffer = (UINT8*)(UINTN)MassCmdPassThru->fpDataBuffer;
    MassXactStruc.dLength = (UINT32)MassCmdPassThru->wDataLength;

    Data16 = (UINT16)USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

    //
    // Update the actual data length processed/returned
    //
    MassCmdPassThru->wDataLength = Data16;

    Data32 = USBMassRequestSense (DevInfo);

    MassCmdPassThru->dSenseData = Data32;

    //
    // Check and free command buffer
    //
    if (!CmdBuffer) {
        return USB_ERROR;
    }

    USB_MemFree(CmdBuffer, (UINT16)GET_MEM_BLK_COUNT(CmdBlkSize));

    return USB_SUCCESS;

}

/**
    This function issues read capacity of the mass storage

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR or USB_SUCCESS

    @note  This routine will update the MassDeviceInfo structure
          with the block size & last LBA values obtained from the
          device

**/

UINT8
USBMassReadCapacity16Command  (
    DEV_INFO* DevInfo
)
{
    UINT32                          Data;
    COMN_READ_CAPACITY_16_CMD       *CmdBuffer;
    MASS_XACT_STRUC                 MassXactStruc;
    
    if (!VALID_DEVINFO(DevInfo)) {
        return USB_ERROR;
    }
    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_16_CMD));
    
    if (!CmdBuffer) {
        return USB_ERROR;
    }
    
    CmdBuffer->OpCode = COMMON_READ_CAPACITY_16_OPCODE;
    CmdBuffer->ServiceAction = 0x10;
    CmdBuffer->AllocLength = 0x0C000000;

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);

    //
    // Change the bulk transfer delay to 10 seconds (For CDROM drives)
    //
    gUsbData->wBulkDataXferDelay = 10000;

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
    MassXactStruc.bCmdSize = sizeof(COMN_RWV_16_CMD);
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = gUsbData->fpUSBTempBuffer;
    MassXactStruc.dLength = 0xC;
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "rcc..");
    Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

    //
    // Reset the delay back
    //
    gUsbData->wBulkDataXferDelay = 0;

    if (!Data) {
        USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_16_CMD));
        
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "err ");
        return  USB_ERROR;
    }

    Data = *((UINT32*)(gUsbData->fpUSBTempBuffer + 8));

    //
    // Change little endian format to big endian(INTEL) format
    //
    Data = dabc_to_abcd(Data);
										 	//(EIP37167+)>
	if (!Data) {
		USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_16_CMD));
		USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "err ");
		return USB_ERROR;
	}

    DevInfo->wBlockSize = (UINT16)Data;

    USB_DEBUG(DEBUG_INFO, 3,"BlockSize %x\n", DevInfo->wBlockSize);

    //
    // Store the last LBA number in the mass info structure
    //
    Data = *((UINT32*)(gUsbData->fpUSBTempBuffer));

    Data = dabc_to_abcd(Data);

    DevInfo->MaxLba = LShiftU64(Data, 32);

    Data = *((UINT32*)(gUsbData->fpUSBTempBuffer + 4));

    Data = dabc_to_abcd(Data);

    DevInfo->MaxLba |= Data;

    USB_DEBUG(DEBUG_INFO, 3,"MaxLba %lx\n", DevInfo->MaxLba);

    return USB_SUCCESS;

}

/**
    This function issues read capacity of the mass storage

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR or USB_SUCCESS

    @note  This routine will update the MassDeviceInfo structure
          with the block size & last LBA values obtained from the
          device

**/

UINT8
USBMassReadCapacity10Command  (
    DEV_INFO* DevInfo
)
{
    UINT32                          Data;
    COMN_READ_CAPACITY_10_CMD       *CmdBuffer;
    MASS_XACT_STRUC                 MassXactStruc;

    if (!VALID_DEVINFO(DevInfo)) {
        return USB_ERROR;
    }
    
    if (RShiftU64(DevInfo->MaxLba, 32)) {
        USBMassReadCapacity16Command(DevInfo);
        return USB_SUCCESS;
    }
    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));

    if (!CmdBuffer) {
        return USB_ERROR;
    }

    CmdBuffer->bOpCode = COMMON_READ_CAPACITY_10_OPCODE;

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);

    //
    // Change the bulk transfer delay to 10 seconds (For CDROM drives)
    //
    gUsbData->wBulkDataXferDelay = 10000;

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
                                        //(EIP51158+)>
    if (DevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x0A;	 //SBC-3_66
    } else {
        MassXactStruc.bCmdSize = sizeof (COMN_READ_CAPACITY_10_CMD);
    }
										//<(EIP51158+)
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = gUsbData->fpUSBTempBuffer;
    MassXactStruc.dLength = 8;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "rcc..");
    Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

    //
    // Reset the delay back
    //
    gUsbData->wBulkDataXferDelay = 0;

    if (!Data) {
        USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "err ");
        return USB_ERROR;
    }

    USB_DEBUG(DEBUG_INFO, 3,"Read Capacity 10 LBA %x\n", *(UINT32*)gUsbData->fpUSBTempBuffer);

    if (*(UINT32*)gUsbData->fpUSBTempBuffer == 0xFFFFFFFF) {
        USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));
        USBMassReadCapacity16Command(DevInfo);
        return USB_SUCCESS;
    }

    Data = *((UINT32*)(gUsbData->fpUSBTempBuffer + 4));

    //
    // Change little endian format to big endian(INTEL) format
    //
    Data = dabc_to_abcd(Data);
										 	//(EIP37167+)>
	if (!Data) {
		USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));
		USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "err ");
		return	USB_ERROR;
	}
											 //<(EIP37167+)

    DevInfo->wBlockSize = (UINT16)Data;
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "succ: %x, %x\n", dData, fpDevInfo);
    //
    // Store the last LBA number in the mass info structure
    //
    Data = *((UINT32*)(gUsbData->fpUSBTempBuffer));

    Data = dabc_to_abcd(Data);

	if (!Data) {
		USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "err ");
		return USB_ERROR;
	}

    DevInfo->MaxLba = Data + 1; // 1-based value

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "%x ", DevInfo->MaxLba);

    USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_CAPACITY_10_CMD));

    return USB_SUCCESS;
}

/**
    This function sends read format capacity command to the USB
    mass storage device

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR or USB_SUCCESS

    @note  This routine will update the MassDeviceInfo structure
              with the block size & last LBA values obtained from the
              device
**/

UINT8
USBMassReadFormatCapacity (DEV_INFO* fpDevInfo)
{
    COMN_READ_FMT_CAPACITY  *fpCmdBuffer;
    UINT32  dData;
    UINT16  wData;
    UINT8*	DataBuffer;
	UINT16	DataBufferSize = 0xFC;
    MASS_XACT_STRUC MassXactStruc;

    //
    // Allocate memory for the command buffer
    //
    fpCmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));

    if(!fpCmdBuffer) {
        return USB_ERROR;
    }

	DataBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT(DataBufferSize));
	if (DataBuffer == NULL) {
		return USB_ERROR;
	}

    fpCmdBuffer->bOpCode = COMMON_READ_FORMAT_CAPACITY_OPCODE;
    fpCmdBuffer->wAllocLength = (UINT16)((DataBufferSize << 8) + (DataBufferSize >> 8));

    USBMassClearMassXactStruc(&MassXactStruc);   // Clear the common bulk transaction structure

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)fpCmdBuffer;
                                        //(EIP51158+)>
    if (fpDevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x0A;
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMN_READ_FMT_CAPACITY);
    }
										//<(EIP51158+)
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = DataBuffer;
//  gUsbData->stMassXactStruc.dLength = MAX_TEMP_BUFFER_SIZE;
//
// Temp buffer 40h-64h was used as device name string buffer.
// Limit Transaction size to 40h to prevent name string display problem.
//
    MassXactStruc.dLength = DataBufferSize;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "Issue ReadFormatCapacityCommand .... \n");

    dData = USBMassIssueMassTransaction(fpDevInfo, &MassXactStruc);

    //
    // The amount of data obtained should be atleast of read format capacity structure size
    //
    if (dData < sizeof (COMN_READ_FMT_CAPACITY)) {
		USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
        USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));
        return USB_ERROR;
    }


    if ((DataBuffer[0] != 0) || (DataBuffer[1] != 0) || (DataBuffer[2] != 0) || (DataBuffer[3] < 0x08)) {
        USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
        USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));
        return USB_ERROR;
    }

    wData = *((UINT16*)(DataBuffer + 10));   // Offset 10
    if (wData == 0) {
        USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
        USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));
        return USB_ERROR;
    }
    fpDevInfo->wBlockSize = (UINT16)((wData << 8) + (wData >> 8));

    dData = *((UINT32*)(DataBuffer + 4));   // Offset 4
    if (dData == 0) {
        USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
        USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));
        return USB_ERROR;
    }
    dData = dabc_to_abcd(dData);
    fpDevInfo->MaxLba = dData;

    if (dData == USB_144MB_FDD_MAX_LBA) {
        //
        // Return parameters for 1.44MB floppy
        //
        fpDevInfo->Heads            = USB_144MB_FDD_MAX_HEADS;
        fpDevInfo->NonLBAHeads      = USB_144MB_FDD_MAX_HEADS;
        fpDevInfo->bSectors         = USB_144MB_FDD_MAX_SECTORS;
        fpDevInfo->bNonLBASectors   = USB_144MB_FDD_MAX_SECTORS;
        fpDevInfo->wCylinders       = USB_144MB_FDD_MAX_CYLINDERS;
        fpDevInfo->wNonLBACylinders = USB_144MB_FDD_MAX_CYLINDERS;
        fpDevInfo->bMediaType       = USB_144MB_FDD_MEDIA_TYPE;
    }

	USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
    USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_READ_FMT_CAPACITY));

    return  USB_SUCCESS;
}

/**
    This function sends get configuration command to the USB
    mass storage device

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR or USB_SUCCESS

**/

UINT8
USBMassGetConfiguration(
    DEV_INFO* DevInfo
)
{
    COMMON_GET_CONFIGURATION        *CmdBuffer;
    UINT32                          Data;
    UINT8	                        *DataBuffer;
	UINT16	                        DataBufferSize = 0x8;
    MASS_XACT_STRUC                 MassXactStruc;

    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMMON_GET_CONFIGURATION));

    if (!CmdBuffer) {
        return USB_ERROR;
    }

	DataBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT(DataBufferSize));
	if (DataBuffer == NULL) {
		return USB_ERROR;
	}

    CmdBuffer->OpCode = COMMON_GET_CONFIGURATION_OPCODE;
    CmdBuffer->AllocLength = (UINT16)((DataBufferSize << 8) + (DataBufferSize >> 8));

    USBMassClearMassXactStruc(&MassXactStruc);  // Clear the common bulk transaction structure

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
    MassXactStruc.bCmdSize = sizeof (COMMON_GET_CONFIGURATION);
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = DataBuffer;
    MassXactStruc.dLength = DataBufferSize;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "Issue GetConfigurationCommand .... \n");

    Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

	USB_MemFree(DataBuffer, GET_MEM_BLK_COUNT(DataBufferSize));
    USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMMON_GET_CONFIGURATION));

    if (Data) {
        return USB_SUCCESS;
    } else {
        return USB_ERROR;
    }

}

/**
    This function a sector at the LBA specified

    @param Pointer to DeviceInfo structure
        LBA to read
          DS:DI   Data buffer pointer

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
USBMassReadSector(
    DEV_INFO*   DevInfo,
    UINT32      Lba,
    UINT8*      Buffer
)
{
    COMN_RWV_10_CMD    *CmdBuffer;
    UINT32              Data;
    UINT8               Counter;
    UINT8               RetValue = USB_ERROR;
    MASS_XACT_STRUC     MassXactStruc;

    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_RWV_10_CMD));

    if (!CmdBuffer) {
        return USB_ERROR;
    }

    USBMassClearMassXactStruc(&MassXactStruc);

    Counter = 10;
    do {
		//
		// Set opcode to read command
		//
		CmdBuffer->bOpCode = COMMON_READ_10_OPCODE;
		CmdBuffer->wTransferLength = 0x100;	// Big endian to little endian: 0x0001 -> 0x0100
		Data = Lba;
		//
		// Change LBA from big endian to little endian
		//
		Data = dabc_to_abcd(Data);
		
		CmdBuffer->dLba = Data;

        //
        // Fill the common bulk transaction structure
        //
        MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
                                        //(EIP51158+)>
        if (DevInfo->bSubClass == SUB_CLASS_SCSI) {
	    	MassXactStruc.bCmdSize = 0x0A;	//SBC-3_60
        } else {
            MassXactStruc.bCmdSize = sizeof (COMN_RWV_10_CMD);
        }
										//<(EIP51158+)
        MassXactStruc.bXferDir = BIT7;     // IN
        MassXactStruc.fpBuffer = Buffer;
        MassXactStruc.dLength = DevInfo->wBlockSize;  //(EIP59738)
        MassXactStruc.wPreSkip = 0;
        MassXactStruc.wPostSkip= 0;

        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "Read Sector .... \n");

        Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);
        if (Data) {
			RetValue = USB_SUCCESS;
            break;  // Success
        }
        //
        // May be drive error. Try to correct from it !
        // Check whether the drive is ready for read/write/verify command
        //
        Data = USBMassCheckDeviceReady(DevInfo);
        if (Data) {    // Device is not ready.
            RetValue = USB_ERROR;
            break;
        }
        ZeroMem((UINT8*)CmdBuffer, sizeof(COMN_RWV_10_CMD));
    } while (Counter--);

    USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_RWV_10_CMD));

    return RetValue;
    
}

/**
    This function parses the boot record and extract the CHS
    information of the formatted media from the boot record.
    This routine checks for DOS & NTFS formats only

    @param Pointer to DeviceInfo structure
        Maximum LBA in the device
        Boot record of the device

    @retval USB_ERROR   If the boot record is un-recognizable and CHS info
        is not extracted
        USB_SUCCESS If the boot record is recognizable and CHS info
        is extracted. CHS information is updated in the
        mass device info structure
**/

UINT8
USBMassUpdateCHSFromBootRecord(
    DEV_INFO        *DevInfo,
    UINT64          MaxLba,
    BOOT_SECTOR     *BootSetor
)
{
	UINT32	OemName = 0;
	UINT32	Fat16SysType = 0;
	UINT32	Fat32SysType = 0;

    if (BootSetor->Signature != 0xAA55) {
		return USB_ERROR;
    }

    //
    // Check for valid MSDOS/MSWIN/NTFS boot record
    //
    CopyMem((UINT8*)&OemName, (UINT8*)BootSetor->OEMName, sizeof(OemName));
    if ((OemName != 0x4F44534D) &&    // "ODSM", MSDO...
        (OemName != 0x4957534D) &&    // "IWSM", MSWI...
        (OemName != 0x5346544E)) {    // "SFTN", NTFS
        //
        // Check for valid FAT,FAT16 or FAT32 boot records
        //
        BootSetor->Fat.Fat16.FilSysType[3] = 0x20;
        CopyMem((UINT8*)&Fat16SysType,
                (UINT8*)BootSetor->Fat.Fat16.FilSysType, sizeof(Fat16SysType));
        CopyMem((UINT8*)&Fat32SysType,
                (UINT8*)BootSetor->Fat.Fat32.FilSysType, sizeof(Fat32SysType));
        if ((Fat16SysType != 0x20544146) &&	// " TAF", FAT
            (Fat32SysType != 0x33544146)) {	// "3TAF", FAT3

            //
            // None of the conditions met - boot record is invalid. Return with error
            //
            return  USB_ERROR;
        }
    }

    // zero check added to prevent invalid sector/head information in BPB
    if (BootSetor->SecPerTrk == 0) {
		return USB_ERROR;
    }

    DevInfo->bSectors = (UINT8)BootSetor->SecPerTrk;
	DevInfo->bNonLBASectors = (UINT8)BootSetor->SecPerTrk;

    // Wrong BPB in MSI MegaStick 128; this is preformat usility issue, wrong BPB
    // information built in device.
    if (BootSetor->NumHeads == 0) {
		return USB_ERROR;
    }

    DevInfo->Heads = BootSetor->NumHeads;
	DevInfo->NonLBAHeads = BootSetor->NumHeads;
    DevInfo->BpbMediaDesc = BootSetor->Media;

    USBMassUpdateCylinderInfo(DevInfo, MaxLba);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "CHS: %x %x %x\n",
                    DevInfo->bSectors,
                    DevInfo->Heads,
                    DevInfo->wCylinders);

    return USB_SUCCESS;
    
}

/**
    This procedure update cylinder parameter for device geometry.
    head and sector paramaters are required before invoke this
    function.

    @param Pointer to DeviceInfo structure
        Maximum LBA in the device
        dev->Heads
        dev->bSectors

    @retval VOID

**/

VOID
USBMassUpdateCylinderInfo(
    DEV_INFO*   Dev,
    UINT64      Lba
)
{
    UINT64 Data;

    if ((Dev->bSectors != 0) && (Dev->Heads != 0)) {
        Data = DivU64x64Remainder(Lba, (Dev->bSectors * Dev->Heads), NULL);
    } else {
        Data = 0;
    }
   
    if (Data <= 1) {
        Data++;
    }
    if (Data > 0xFFFF) {
        Data = 0xFFFF;   // DOS workaround
    }
    Dev->wCylinders = (UINT16)Data;
    Dev->wNonLBACylinders = (UINT16)Data;
    
}

/**
    This function reads the first sector from the mass storage
    device and identifies the formatted type.

    @param Pointer to DeviceInfo structure
        Maximum LBA of the device

    @retval USB_ERROR   If could not identify the formatted type
        USB_SUCCESS If formatted type is identified
        MSB of emu - Emulation type
        LSB of emu - Device type (Floppy, Harddisk or CDROM)

**/

UINT8
USBMassGetFormatType(
    DEV_INFO*   DevInfo,
    UINT64      MaxLba,
    UINT16      *Emu
)
{

	MBR_PARTITION Partition = {0};

    //
    // Read the first sector of the device
    //
    if (USBMassReadSector(DevInfo, 0, gUsbData->fpUSBMassConsumeBuffer) == USB_ERROR) {
        return USB_ERROR;
    }

    DevInfo->bHiddenSectors = 0;

    //
    // Check for validity of the partition table/boot record
    //
    if (*((UINT16*)(gUsbData->fpUSBMassConsumeBuffer + 0x1FE)) != 0xAA55) {
        USBMassSetDefaultGeometry(DevInfo, MaxLba);
        return USB_ERROR;
    }

    if (USBMassValidatePartitionTable((MASTER_BOOT_RECORD*)gUsbData->fpUSBMassConsumeBuffer, 
			MaxLba, &Partition) == USB_SUCCESS) {
        //
        // Only one partition present, check the device size, if the device size
        // is less than 530 MB assume FDD or else assume the emulation as HDD
        //

        //if (((MaxLba >> 11) < MAX_SIZE_FOR_USB_FLOPPY_EMULATION) &&	//(EIP80382)
        //    !(gUsbData->dUSBStateFlag & USB_FLAG_MASS_NATIVE_EMULATION)) {
        //    emu_ = (UINT16)(USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
        //}else {
        //    emu_ = (UINT16)(USB_EMU_HDD_ONLY << 8) + USB_MASS_DEV_HDD;
        //}
        //
        // Read boot sector, set the LBA number to boot record LBA number
        //
        DevInfo->bHiddenSectors = Partition.StartingLba;

        if (USBMassReadSector(DevInfo, Partition.StartingLba,
                gUsbData->fpUSBMassConsumeBuffer) == USB_ERROR) {
            return USB_ERROR;
        }

        if (USBMassUpdateCHSFromBootRecord(DevInfo, MaxLba, 
				(BOOT_SECTOR*)gUsbData->fpUSBMassConsumeBuffer) == USB_SUCCESS) {
            if (((RShiftU64(MaxLba, 11)) < MAX_SIZE_FOR_USB_FLOPPY_EMULATION) &&
                !(gUsbData->dUSBStateFlag & USB_FLAG_MASS_NATIVE_EMULATION)) {
                if (DevInfo->bSubClass != SUB_CLASS_UFI) {
                    *Emu = (UINT16)(USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
                }
            }
            return USB_SUCCESS;
        } else {  // Reset hidden sector value and return HDD emulation
            USBMassSetDefaultGeometry(DevInfo, MaxLba);
            DevInfo->bHiddenSectors = 0;
										//(EIP43711)>
            //don't emulate as HDD for UFI class even media has valid partition like HDD
            if (gUsbData->dUSBStateFlag & USB_FLAG_MASS_SIZE_EMULATION) {
	            if (DevInfo->bSubClass != SUB_CLASS_UFI) {
		            if ((RShiftU64(MaxLba, 11)) < MAX_SIZE_FOR_USB_FLOPPY_EMULATION) {
                        *Emu = (UINT16)(USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
                    }
                }
            }
										//<(EIP43711)
            return USB_SUCCESS;
        }
    }
    
    *Emu = USBMassSetDefaultType(DevInfo, MaxLba);
    
    if (USBMassUpdateCHSFromBootRecord(DevInfo, MaxLba, 
			(BOOT_SECTOR*)gUsbData->fpUSBMassConsumeBuffer) == USB_SUCCESS) {
        //*emu = USBMassSetDefaultType(fpDevInfo, MaxLba);
         if (gUsbData->dUSBStateFlag & USB_FLAG_MASS_SIZE_EMULATION) {
            if (DevInfo->bSubClass != SUB_CLASS_UFI) {
                if ((RShiftU64(MaxLba, 11)) >= MAX_SIZE_FOR_USB_FLOPPY_EMULATION) {
                    *Emu = (UINT16)(USB_EMU_HDD_ONLY << 8) + USB_MASS_DEV_HDD;
                }
            }
        }
        return USB_SUCCESS;
    }
    USBMassSetDefaultGeometry(DevInfo, MaxLba);

    //*emu = USBMassSetDefaultType(fpDevInfo, MaxLba);

    if (((RShiftU64(MaxLba, 11)) >= MAX_SIZE_FOR_USB_FLOPPY_EMULATION) &&
        !(gUsbData->dUSBStateFlag & USB_FLAG_MASS_NATIVE_EMULATION)) {
        if (DevInfo->bSubClass != SUB_CLASS_UFI) {
            *Emu = (UINT16)(USB_EMU_HDD_ONLY << 8) + USB_MASS_DEV_HDD;
        }
        DevInfo->bHiddenSectors = 0;
    }
    //*emu = emu_;

    return USB_SUCCESS;
}

/**
    This procedure set device type depend on device class.

    @param Pointer to DeviceInfo structure
        Maximum LBA in the device (DWORD)

    @retval Device Type (WORD)

**/

UINT16
USBMassSetDefaultType(
    DEV_INFO*   Dev,
    UINT64      Lba
)
{
    UINT16 DevType = (UINT16)(USB_EMU_FLOPPY_ONLY << 8) + USB_MASS_DEV_ARMD;

    if (Dev->bSubClass != SUB_CLASS_UFI) {  // Check whether UFI class device
        // Assume force FDD emulation for non-UFI class device
        DevType = (UINT16)(USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
    }
    return DevType;
}

/**
    This procedure check whether partition table valid.

    @param Partition table content
        Maximum LBA in the device

    @retval USB_SUCCESS partion table is valid:
        Possible valid entry count(1-based)
        Table entry counts(0-based, 4 means all entries scaned)
        Activate entry offset(Absolute offset)
    @retval USB_ERROR Invalid partition table

**/

UINT8
USBMassValidatePartitionTable(
    IN MASTER_BOOT_RECORD	*Mbr,
    IN UINT64 				Lba,
    OUT MBR_PARTITION		*Partition)
{
	UINT8	Index = 0;
	UINT8	ActivateIndex = 0;

    // The partition table area could be all 0's, and it would pass the below tests,
    // So test for that here (test sector count for all partitions).
	if ((Mbr->PartRec[0].SizeInLba == 0) && 
		(Mbr->PartRec[1].SizeInLba == 0) &&
		(Mbr->PartRec[2].SizeInLba == 0) &&
		(Mbr->PartRec[3].SizeInLba == 0)) {
		return USB_ERROR;
	}

    for (Index = 0; Index < 4; Index++) {
        // Boot flag check added to ensure that boot sector will not be treated as
        // a valid partation table.
        if (Mbr->PartRec[Index].BootIndicator & 0x7F) {
			return USB_ERROR;   // BootFlag should be 0x0 or 0x80
        }

        // Check whether beginning LBA is reasonable
        if (Mbr->PartRec[Index].StartingLba > Lba) {
			return USB_ERROR;
        }

        // Check whether the size is reasonable
#if HDD_PART_SIZE_CHECK
        if (Mbr->PartRec[Index].SizeInLba > Lba) {
			return USB_ERROR;
        }
#endif
        // Update activate entry offset
        if (!(Mbr->PartRec[Index].BootIndicator & 0x80)) {
			continue;
        }

		ActivateIndex = Index;
    }

    // If no activate partition table entry found use first entry
    CopyMem((UINT8*)Partition,
            (UINT8*)&Mbr->PartRec[ActivateIndex], sizeof(MBR_PARTITION));

    return USB_SUCCESS;

}

/**
    This procedure set default geometry for mass storage devices.

    @param Pointer to DeviceInfo structure
        Maximum LBA in the device

    @retval USB_ERROR   If could not identify the formatted type
        USB_SUCCESS If formatted type is identified
        Emulation type - bits 8..15
        Device type (Floppy, Harddisk or CDROM) - bits 0..7

**/

UINT8 USBMassSetDefaultGeometry(DEV_INFO* dev, UINT64 lba)
{
    if (dev->bSubClass == SUB_CLASS_UFI) {
        dev->Heads = 0x02;
        dev->NonLBAHeads = 0x02;
        dev->bSectors = 0x12;
        dev->bNonLBASectors = 0x12;
    }
    else {
    	dev->bSectors = 0x3F;
    	dev->bNonLBASectors = 0x3F;
// Use default heads that results in 1023 (3FF) cylinders or less for CHS
    	if (lba <= 0x1F7820) {
            dev->Heads = 0x20;
            dev->NonLBAHeads = 0x20;
        }
        else if ( (lba > 0x1F7820) && (lba <= 0x3EF040) ) {
            dev->Heads = 0x40;
            dev->NonLBAHeads = 0x40;
        }
        else if ( (lba > 0x3EF040) && (lba <= 0x7DE080) ) {
            dev->Heads = 0x80;
            dev->NonLBAHeads = 0x80;
        }
        else if (lba > 0x7DE080) {
            dev->Heads = 0xFF;
            dev->NonLBAHeads = 0xFF;
        }
    }

    USBMassUpdateCylinderInfo(dev, lba);
    return USB_SUCCESS;
}


/**
    This function identifies the type of the USB mass storage
    device attached from the INQUIRY data obtained from the drive

    @param Pointer to DeviceInfo structure
        Pointer to the inquiry data (read from device)

    @retval VOID

**/

UINT8
USBMassIdentifyDeviceType(
    DEV_INFO*   DevInfo,
    UINT8*      InqData
)
{
    UINT16  EmulationType;
    UINT16  ForceEmulationType = 0;
    UINT32  Data = 0;
    UINT8   Count;
    BOOLEAN ReadCapacityTimeout;
    static  UINT16 UsbMassEmulationTypeTable[5] = {
        0,  // Auto
        (USB_EMU_FLOPPY_ONLY << 8) + USB_MASS_DEV_ARMD,     // Floppy
        (USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD,      // Forced floppy
        (USB_EMU_HDD_ONLY << 8) + USB_MASS_DEV_HDD,         // HDD
        (USB_EMU_HDD_OR_FDD << 8) + USB_MASS_DEV_CDROM };   // CDROM

    USBMassGetPhysicalDeviceType(DevInfo, InqData);

    // Note: at this point we assume that dev->wEmulationOption is filled in
    // according to the setup question selection.
    if (!(DevInfo->Flag & DEV_INFO_HOTPLUG) || DevInfo->wEmulationOption) {    // not auto
        EmulationType = UsbMassEmulationTypeTable[DevInfo->wEmulationOption];
        ForceEmulationType = UsbMassEmulationTypeTable[DevInfo->wEmulationOption];
    }
    
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, ">>-- IdentifyDeviceType:: Device #%d, Emul#: %d, Emul: %x\n",
    //                              DevInfo->bDeviceAddress, DevInfo->wEmulationOption, EmulationType);
#if USB_STORAGE_DEVICE_RMB_CHECK
    if (*(InqData + 1) & 0x80) { // Check RMB status
        DevInfo->bLastStatus |= USB_MASS_MEDIA_REMOVEABLE;
    }
#else
    DevInfo->bLastStatus |= USB_MASS_MEDIA_REMOVEABLE;
#endif

    DevInfo->bLastStatus |= USB_MASS_MEDIA_PRESENT; // Assume Media Present

    if (*InqData == 5) {   // CDROM
        // Set the type as CDROM and emulation as HDD or FDD
        DevInfo->wBlockSize = 0x800;
        EmulationType = (UINT16)(USB_EMU_HDD_OR_FDD << 8) + USB_MASS_DEV_CDROM;
        goto UMIDT_DeviceTypeOver;
    }
//					;(EIP25229+)>
#if USB_START_UNIT_BEFORE_MSD_ENUMERATION
//  Start unit command before access it
	USBMassStartUnitCommand(DevInfo);
#endif
//					;<(EIP25229+)
                                        //(EIP80382)>
    if (DevInfo->bSubClass == SUB_CLASS_UFI) { 
        EmulationType = (UINT16)(USB_EMU_FLOPPY_ONLY << 8) + USB_MASS_DEV_ARMD;
    } else {
        EmulationType = (UINT16)(USB_EMU_HDD_ONLY << 8) + USB_MASS_DEV_HDD;
    }
                                        //<(EIP80382)
                                        
    FixedDelay(gUsbData->UsbTimingPolicy.MassDeviceComeUp * 1000);    // Device is coming up give 500ms delay
    //
    // Some USB mass storage devces are not fast enough to accept mass storage
    // commands for parsing geometry, issue read capacity command to make sure device
    // is ready for further access. (USB0089+)>
    //
    if (DevInfo->bSubClass != SUB_CLASS_UFI) {
        for (Count = 0; Count < 30 && VALID_DEVINFO(DevInfo); Count++) {
            if (USBMassReadCapacity10Command(DevInfo) == USB_SUCCESS) {
                break;
            }
            if (gUsbData->bLastCommandStatus & USB_BULK_TIMEDOUT) {
                ReadCapacityTimeout = TRUE;
            } else {
                ReadCapacityTimeout = FALSE;
            }
            if ((UINT16)USBMassRequestSense(DevInfo) == 0x3A02 ) {	//(EIP86793)
                break;  // No media
            }
            if ((gUsbData->bLastCommandStatus & USB_BULK_TIMEDOUT) && (ReadCapacityTimeout == TRUE)) {
                return USB_ERROR;
            }
        }
    }
    //
    // Get the block size & last LBA number
    //
    Data = USBMassCheckDeviceReady(DevInfo);
										//(EIP86793)>
    if ((UINT16)Data == 0x3A02) {  // Check for media presence status
        //
        // Media not present. Try to get disk geometry from Format
        // capacity command
        //
        if (!(DevInfo->wIncompatFlags & USB_INCMPT_FORMAT_CAPACITY_NOT_SUPPORTED)) {
            USBMassReadFormatCapacity(DevInfo);
            if ((DevInfo->MaxLba != 0) && (DevInfo->MaxLba <= USB_144MB_FDD_MAX_LBA)) {
                EmulationType = (UINT16)(USB_EMU_FLOPPY_ONLY << 8) + USB_MASS_DEV_ARMD;
            } else {
                if (!(gUsbData->dUSBStateFlag & USB_FLAG_MASS_EMULATION_FOR_NO_MEDIA)) {
                    EmulationType = (UINT16)(USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
                }
            }
            goto UMIDT_DeviceTypeOver;
        }
    }

    //
    // Proceed with normal checking
    //
    if (!Data) {
                                        //(EIP59738-)>
        //
        // Get the max LBA & block size; if block size is other than
        // 512 bytes assume emulation as CDROM
        //
        //if ( dev->wBlockSize > 0x200 ) {
        //    wEmulationType = (UINT16)(USB_EMU_HDD_OR_FDD << 8) + USB_MASS_DEV_CDROM;
        //    goto UMIDT_DeviceTypeOver;
        //}
                                        //(<EIP59738-)
                                        //(EIP80382)>
        if (USBMassGetFormatType(DevInfo, DevInfo->MaxLba, &EmulationType) == USB_ERROR) {
            //
            // Find the device type by size
            //
            if (((RShiftU64(DevInfo->MaxLba, 11)) < MAX_SIZE_FOR_USB_FLOPPY_EMULATION) || 
                    (gUsbData->dUSBStateFlag & USB_FLAG_MASS_NATIVE_EMULATION)) {
				if (DevInfo->bSubClass != SUB_CLASS_UFI) {
					EmulationType = (USB_EMU_FORCED_FDD << 8) + USB_MASS_DEV_ARMD;
                }
            }
        }
    }
										//<(EIP80382)
										//<(EIP86793)

UMIDT_DeviceTypeOver:

    if (ForceEmulationType) {
        EmulationType = ForceEmulationType;
    }
    DevInfo->bStorageType = (UINT8)EmulationType;
    DevInfo->bEmuType = (UINT8)(EmulationType >> 8);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "<<-- IdentifyDeviceType:: Emul: %x\n", EmulationType);

    return USB_SUCCESS;
}

/**
    This procedure classify USB mass storage devices according to
    inquiry command return data.

    @param Pointer to DeviceInfo structure
        Pointer to the inquiry data (read from device)

    @retval VOID

**/

VOID
USBMassGetPhysicalDeviceType(
    DEV_INFO*   Dev,
    UINT8       *Buf
)
{

    switch (*Buf) {
        case 0x0:
            if (Dev->bSubClass == SUB_CLASS_UFI) {
                Dev->bPhyDevType = USB_MASS_DEV_FDD;
                break;
            }
            Dev->bPhyDevType = (*(Buf+1) & 0x80) ? 
                USB_MASS_DEV_ARMD : USB_MASS_DEV_HDD;
            break;
        case 0x5:
            Dev->bPhyDevType = USB_MASS_DEV_CDROM;
            break;
        case 0x7:
            Dev->bPhyDevType = USB_MASS_DEV_MO;
            break;
        case 0xE:
            Dev->bPhyDevType = USB_MASS_DEV_ARMD;
            break;
        default:
            Dev->bPhyDevType = USB_MASS_DEV_UNKNOWN;
            break;
    }
    
}

/*                                      //(EIP59738-)>
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// PROCEDURE:   USBMassConsumeBulkData
//
// DESCRIPTION: This function reads unwanted amount of data specified in
//              the size
//
// PARAMETERS:  fpDevInfo   Pointer to DeviceInfo structure
//              bXferDir    Transfer direction
//              wLength     Size of data to consume
//
// RETURN:      USB_ERROR or USB_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8
USBMassConsumeBulkData(
    DEV_INFO*   fpDevInfo,
    UINT8       bXferDir,
    UINT16      wLength)
{
    UINT16  wBytesToTransfer, wBytesRemaining;
    UINT32  dData;

//
// Need to process only maximum amount of data that pUSBMassConsumeBuffer can
// handle, i.e. MAX_CONTROL_DATA_SIZE
//
    wBytesRemaining = wLength;
    do {
        wBytesToTransfer = (UINT16)((wBytesRemaining < MAX_CONTROL_DATA_SIZE)?
                    wBytesRemaining : MAX_CONTROL_DATA_SIZE);

        dData = USBMassIssueBulkTransfer(fpDevInfo, bXferDir,
                    gUsbData->fpUSBMassConsumeBuffer, (UINT32)wBytesToTransfer);

        if ((UINT16)dData != wBytesToTransfer) {    // Comparing word should be sufficient
            return  USB_ERROR;
        }
        wBytesRemaining = (UINT16)(wBytesRemaining - dData);

    } while (wBytesRemaining);

    return  USB_SUCCESS;
}
*/                                      //<(EIP59738-)


/**
    This function reads/writes the data to the mass storage
    device using bulk transfer. It also takes care of pre and
    post skip bytes.

    @param fpDevInfo   Pointer to DeviceInfo structure
        stMassXactStruc (given for reference)
        bXferDir    Transfer direction
        fpBuffer    Data buffer far pointer
        dLength Amount of data to be transferred
        wPreSkip    Number of bytes to skip before data
        wPostSkip   Number of bytes to skip after data

    @retval Amount of data actually transferred

**/
										//(EIP70814)>
UINT32
USBMassProcessBulkData(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
    UINT32      dData;
    UINT16      wTemp;
	UINT8		*Buffer;
	UINT8		*SrcBuffer;
	UINT8		*DestBuffer;
	UINT16		PreSkip;
	UINT32		XferSize;
	UINT32		XferedSize;
	UINT32		RemainingSize;

//USB_DEBUG (DEBUG_INFO, DEBUG_LEVEL_3, "Pre,%x;Post,%x\n", gUsbData->stMassXactStruc.wPreSkip,
//                  gUsbData->stMassXactStruc.wPostSkip);
    //
    // Check whether something we have to transfer
    //
    if (!MassXactStruc->dLength) {
        return 0;
    }


    wTemp   = gUsbData->wTimeOutValue;     // Save original value
    if (gUsbData->wBulkDataXferDelay) {    // Check the bulk data delay specified
        gUsbData->wTimeOutValue = gUsbData->wBulkDataXferDelay;
    }

	if ((MassXactStruc->wPreSkip == 0) && (MassXactStruc->wPostSkip == 0)) {
	    dData = USBMassIssueBulkTransfer(
	                    DevInfo,
	                    MassXactStruc->bXferDir,
	                    MassXactStruc->fpBuffer,
	                    MassXactStruc->dLength);
	} else {
		// Allocate a data buffer
		Buffer = USB_MemAlloc((UINT16)GET_MEM_BLK_COUNT(DevInfo->wBlockSize));
		PreSkip = MassXactStruc->wPreSkip;
		RemainingSize = MassXactStruc->dLength - 
						(PreSkip + MassXactStruc->wPostSkip);
		DestBuffer = MassXactStruc->fpBuffer;

		for (XferedSize = 0; XferedSize < MassXactStruc->dLength;) {
			XferSize = MassXactStruc->dLength >= DevInfo->wBlockSize ?
						DevInfo->wBlockSize : MassXactStruc->dLength;

		    dData = USBMassIssueBulkTransfer(
		                DevInfo,
		                MassXactStruc->bXferDir,
		                Buffer,
		                XferSize);
		    if (dData == 0) {
                                        //(EIP83295)>
                //return 0;
                XferedSize = 0;
                break;
                                        //<(EIP83295)
		    }

			XferedSize += XferSize;
			if (RemainingSize == 0) {
				continue;
			}

			SrcBuffer = Buffer;
	
			if (PreSkip != 0) {
				if (PreSkip >= XferSize) {
					PreSkip -= XferSize;
					continue;
				}
	
				SrcBuffer += PreSkip;
				XferSize -= (UINT32)PreSkip;
				PreSkip = 0;
			}

			XferSize = RemainingSize < XferSize ? RemainingSize : XferSize;
			CopyMem(DestBuffer, SrcBuffer, XferSize);

			// Update the destination buffer pointer
			DestBuffer += XferSize;
			RemainingSize -= XferSize;
		}
		
		USB_MemFree(Buffer, (UINT16)GET_MEM_BLK_COUNT(DevInfo->wBlockSize));

		dData = XferedSize;
	}

    gUsbData->wTimeOutValue = wTemp;   // Restore original timeout value
    gUsbData->wBulkDataXferDelay = 0;

	return dData;
}
										//<(EIP70814)

/**
    This function sends inquiry command to the USB mass storage
    device

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Pointer to the inquiry data

**/

MASS_INQUIRY*
USBMassInquiryCommand (DEV_INFO* fpDevInfo)
{
    COMMON_INQ_CMD  *   fpCmdBuffer;
    UINT32              dData;
    MASS_XACT_STRUC     MassXactStruc;

    //
    // Allocate memory for the command buffer
    //
    fpCmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMMON_INQ_CMD));
    if(!fpCmdBuffer) {
        return NULL;
    }

    fpCmdBuffer->bOpCode = COMMON_INQUIRY_OPCODE;
    fpCmdBuffer->bAllocLength = 0x24;

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)fpCmdBuffer;
                                        //(EIP51158+)>
    if (fpDevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x06;	//SPC-4_246	
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMMON_INQ_CMD);
    }
										//<(EIP51158+)
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = gUsbData->fpUSBTempBuffer + 0x40;
    MassXactStruc.dLength = 0x24;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "Issue Inquiry Command .... \n");

    dData = USBMassIssueMassTransaction(fpDevInfo, &MassXactStruc);

    USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMMON_INQ_CMD));


    if (dData) {
        return (MASS_INQUIRY*)(gUsbData->fpUSBTempBuffer + 0x40);
    }
    else {
        return NULL;
    }
}


/**
    This function reads/writes/verifies blocks of data from the
    USB mass device specified by its device address

    @param fpDevInfo   Pointer to DeviceInfo structure
        bOpCode     Read/Write/Verify
        fpReadData  Pointer to the read command structure
        bDevAddr        USB device address of the device
        dStartLBA       Starting LBA address
        wNumBlks        Number of blocks to process
        wPreSkipSize    Number of bytes to skip before
        wPostSkipSize   Number of bytes to skip after
        fpBufferPtr     Far buffer pointer

    @retval Return code (0 - Failure, <>0 - Size read)
        fpReadData  Pointer to the mass read command structure
        dSenseData  Sense data of the last command
        fpBufferPtr Far buffer pointer

**/

UINT16
USBMassRWVCommand(
    DEV_INFO    *DevInfo,
    UINT8       OpCode,
    VOID        *DataStruc
)
{
    EFI_MASS_READ		*MassDataStruc = (EFI_MASS_READ*)DataStruc;
    COMN_RWV_16_CMD	    *CmdBuffer;
    UINT64              StartLba;
    UINT32              BytesToRw;
    UINT32              Data;
    UINT32              SenseData;
    UINT8               Dir;       // BIT7 0/1 - R/W
    UINT8               RetryNum;
    UINT16              RetCode = 0;
    UINT8               CmdSize;
    EFI_STATUS          EfiStatus = EFI_SUCCESS;
    MASS_XACT_STRUC     MassXactStruc;
    

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        EfiStatus = AmiValidateMemoryBuffer((VOID*)MassDataStruc->BufferPtr, 
                (UINT32)MassDataStruc->NumBlks * (UINT32)DevInfo->wBlockSize);
        if (EFI_ERROR(EfiStatus)) {
            USB_DEBUG(DEBUG_ERROR, 3, "UsbMassRWVCommand Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    //
    // Set the sense code as 0
    //
    MassDataStruc->SenseData = 0;

    //
    // Allocate memory for the command buffer
    //
    CmdBuffer = (COMN_RWV_16_CMD*)USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_RWV_16_CMD));
    if (!CmdBuffer) {
        return 0;
    }

	for (RetryNum = 0; RetryNum < 2; RetryNum++) {
	    //
	    // Load command into (just allocated) mass command buffer
	    //
	    CmdBuffer->OpCode = OpCode;
	    StartLba = MassDataStruc->StartLba;
                                        //(EIP60588+)>
        if (StartLba > (DevInfo->MaxLba - MassDataStruc->NumBlks)) {
            StartLba = DevInfo->MaxLba - MassDataStruc->NumBlks;
        }
                                        //<(EIP60588+)
	//
	// If the "Forced FDD" option is selected that means the device has
	// to be emulated as a floppy drive even though it has a HDD emulated
	// image.  This is accomplished by hiding the first cylinder totally.
	// The partition table is in the first cylinder.  LBA value for all
	// the requests to the device will be offset with the number of sectors
	// in the cylinder.
	//

	    //
	    // Check for forced floppy emulated device and change LBA accordingly
	    //
	    if (DevInfo->bEmuType == USB_EMU_FORCED_FDD) {
            if (!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) //(EIP113379+)
    	        //
    	        // Skip first track in case of floppy emulation
    	        //
    	        StartLba += DevInfo->bHiddenSectors;
	    }

        //
        // Check the validity of the block size
        //
        if (DevInfo->wBlockSize != 0xFFFF) {
            //
            // Change big endian format (INTEL) to little endian format
            //
            if ((OpCode == COMMON_READ_10_OPCODE) ||
                (OpCode == COMMON_WRITE_10_OPCODE) ||
                (OpCode == COMMON_VERIFY_10_OPCODE)) {
                ((COMN_RWV_10_CMD*)CmdBuffer)->dLba = dabc_to_abcd((UINT32)StartLba);
                ((COMN_RWV_10_CMD*)CmdBuffer)->wTransferLength =
                    (UINT16)((MassDataStruc->NumBlks << 8) + (MassDataStruc->NumBlks >> 8));
                if (DevInfo->bSubClass == SUB_CLASS_SCSI) {
	        	    CmdSize = 0x0A;	//SBC-3_60
                } else {
                    CmdSize = sizeof (COMN_RWV_10_CMD);
                }
            } else {
            	CmdBuffer->Lba = LShiftU64(dabc_to_abcd((UINT32)StartLba), 32);
                CmdBuffer->Lba |= dabc_to_abcd((UINT32)RShiftU64(StartLba, 32));
                CmdBuffer->TransferLength = dabc_to_abcd(MassDataStruc->NumBlks);
                CmdSize = sizeof(COMN_RWV_16_CMD);
            }
            //
            // Verify command does not need delay
            //
            gUsbData->wBulkDataXferDelay = 0;

            //
            // Calculate number of bytes to transfer (for verify command nothing
            // to read/write.
            //
            BytesToRw = 0;
            if ((OpCode != COMMON_VERIFY_10_OPCODE) && 
                (OpCode != COMMON_VERIFY_16_OPCODE)){
                //
                // Read/write command may need long time delay
                //
                gUsbData->wBulkDataXferDelay = 20000;
                BytesToRw = (UINT32)MassDataStruc->NumBlks * (UINT32)DevInfo->wBlockSize;
            }

            //
            // Set the direction properly
            //
            if ((OpCode == COMMON_WRITE_10_OPCODE) ||
                (OpCode == COMMON_WRITE_16_OPCODE)) {
                Dir = 0;
            } else {
                Dir = BIT7;
            }

            //
            // Fill the common bulk transaction structure
            // Fill Command buffer address & size
            //
            MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
            MassXactStruc.bCmdSize = CmdSize;
            MassXactStruc.bXferDir = Dir;
            MassXactStruc.fpBuffer = (UINT8*)(UINTN)MassDataStruc->BufferPtr;
            MassXactStruc.wPreSkip = MassDataStruc->PreSkipSize;
            MassXactStruc.wPostSkip = MassDataStruc->PostSkipSize;
            MassXactStruc.dLength = BytesToRw;

            Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

            if ((Data) && ((RetryNum != 0) || (Data == BytesToRw))) {    // Some data processed. Set return value
                
                //
                // Bug fix for installing Linux from USB CD-ROM.
                // Linux64Bit Boot
                // If data read is 64K or higher return 0FFFFh
                //
                if (Data >= 0x010000) {
                    Data = 0xFFFF;
                }

                RetCode = (UINT16)Data;
                //
                // Check for forced floppy emulated device
                //
                if ((DevInfo->bEmuType == USB_EMU_FORCED_FDD) &&
                     (OpCode == COMMON_READ_10_OPCODE) &&
                     (MassDataStruc->StartLba == 0) && 
                    !(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) ) {     //(EIP113379+)
                    //
                    // This is a floppy emulated ZIP drive, with read to
                    // first sector. Update the boot record so that floppy
                    // emulation is okay.
                    //
                    // Force #of hidden sectors to 0
                    //
                    *(UINT32*)((UINTN)MassDataStruc->BufferPtr + 0xB + 0x11) = 0;

                    //
                    // FreeDOS workaround
                    //
                    if ((*(UINT32*)((UINTN)MassDataStruc->BufferPtr + 3)==0x65657246) &&  // 'eerF'
                        (*(UINT32*)((UINTN)MassDataStruc->BufferPtr + 7)==0x20534F44) &&  // ' SOD'
                        (*(UINT32*)((UINTN)MassDataStruc->BufferPtr + 0x3A)!=0x20202032)) {				//(EIP61388)
                        *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x42) =
                            *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x42)-(UINT16)DevInfo->bHiddenSectors;
                        *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x46) =
                            *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x46)-(UINT16)DevInfo->bHiddenSectors;
                        *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x4A) =
                            *(UINT16*)((UINTN)MassDataStruc->BufferPtr + 0x4A)-(UINT16)DevInfo->bHiddenSectors;
                    }
                    //
                    // Force physical drive# to 0
                    // For FAT32, physical drive number is present in offset 40h
                    //
                    if ((*(UINT32*)((UINTN)MassDataStruc->BufferPtr + 0x52)) ==
                                        0x33544146) {       // "3TAF", FAT3
                        *(UINT8*)((UINTN)MassDataStruc->BufferPtr + 0x40) = 0;
                    }
                    else {
                        *(UINT8*)((UINTN)MassDataStruc->BufferPtr + 0x24) = 0;
                    }
                }
                break;  // dData ready

            }
            else {  // Error condition: dData = 0, RetCode = 0
                //
                // Check for error
                //
                SenseData = USBMassRequestSense(DevInfo);
                MassDataStruc->SenseData = SenseData;
                Data = SenseData;

                //
                // Check for write protect error code
                //
                if ((UINT8)SenseData == 7) {
                    break;
                }

                if (((OpCode == COMMON_VERIFY_10_OPCODE) ||
                    (OpCode == COMMON_VERIFY_16_OPCODE)) && (!SenseData)) {
                    //
                    // This is verify command so no data to send or read and
                    // also sense data is 0. So set return value to success.
                    //
                    RetCode = 0xFFFF;
                    break;
                }
            }
        }   // fpDevInfo->wBlockSize != 0xFFFF

        //
        // UPRCC_ProceedIfRW
        // May be drive error, try to correct it
        // Check whether the drive is ready for read/write/verify command
        //
        Data = USBMassCheckDeviceReady(DevInfo);
        MassDataStruc->SenseData = Data;

        if (Data) {
            break;  // Return error
        }

        ZeroMem((UINT8*)CmdBuffer, sizeof(COMN_RWV_16_CMD));
    }   // Fof loop

    USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_RWV_16_CMD));

    return RetCode;
}


/**
    This function sends the start unit command to the mass device

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Sense data: 0 - Success, <>0 - Error

**/

UINT16
USBMassStartUnitCommand (DEV_INFO* fpDevInfo)
{
    COMMON_START_STOP_UNIT_CMD  *       fpCmdBuffer;
    MASS_XACT_STRUC                     MassXactStruc;
//  MASS_START_STOP_UNIT        *fpStartData;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USBMProStartUnitCommand ....  \n");

    //
    // Check the compatibility flag for start unit command not supported
    //
    if (fpDevInfo->wIncompatFlags & USB_INCMPT_START_UNIT_NOT_SUPPORTED) {
        return  USB_SUCCESS;
    }

    //
    // Allocate memory for the command buffer
    //
    fpCmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMMON_START_STOP_UNIT_CMD));
    if (!fpCmdBuffer) {
        return  USB_ERROR;
    }

    //
    // Load command into (just allocated) mass command buffer
    //
    fpCmdBuffer->bOpCode = COMMON_START_STOP_UNIT_OPCODE;
    fpCmdBuffer->bStart = 1;

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);
    gUsbData->wBulkDataXferDelay = 10000;  // Start unit command may need long time delay
    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)fpCmdBuffer;
                                        //(EIP51158+)>
    if (fpDevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x06;	//SBC-3_77
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMMON_START_STOP_UNIT_CMD);
    }
										//<(EIP51158+)
    USBMassIssueMassTransaction(fpDevInfo, &MassXactStruc);

    //
    // No data to read/write. So do not process return code.
    // Check and free command buffer
    //
    USB_MemFree(fpCmdBuffer,GET_MEM_BLK_COUNT_STRUC(COMMON_START_STOP_UNIT_CMD));

    return USBMassRequestSense(fpDevInfo);
}


/**
    This function requests the mode sense data page number 5 from
    the USB mass storage device

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval USB_SUCCESS/USB_ERROR on Success/Failure
        fpModeSenseData     Pointer to the mode sense data
        dSenseData  Sense data
        bNumHeads   Number of heads
        wNumCylinders   Number of cylinders
        bNumSectors Number of sectors
        wBytesPerSector Number of bytes per sector
        bMediaType  Media type

**/

UINT8
USBMassModeSense(
    DEV_INFO        *fpDevInfo,
    MASS_MODE_SENSE *fpModeSenseData)
{
    UINT32                  dData;
    UINT8                   bRetCode;
    COMN_MODE_SENSE_10CMD   *fpCmdBuffer;
    MODE_SENSE_10_HEADER    *fpModeSense10_Header;
    PAGE_CODE_5             *fpPageCode5;
    MASS_XACT_STRUC         MassXactStruc;

    dData = 0;
    bRetCode = USB_ERROR;

    fpCmdBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMN_MODE_SENSE_10CMD));
    if (!fpCmdBuffer) {
        return  USB_ERROR;
    }

    //
    // Load command into (just allocated) mass command buffer
    //
    fpCmdBuffer->bOpCode = COMMON_MODE_SENSE_10_OPCODE;
    fpCmdBuffer->wAllocLength = 0x2800; // Allocation Length = 40 bytes (0x28)
    fpCmdBuffer->bPageCode = 5; // Page code

    //
    // Clear the common bulk transaction structure
    //
    USBMassClearMassXactStruc(&MassXactStruc);

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)fpCmdBuffer;
                                        //(EIP51158+)>
    if (fpDevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x0A;	//SPC-4_280
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMN_MODE_SENSE_10CMD);
    }
										//<(EIP51158+)
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = gUsbData->fpUSBTempBuffer;
    MassXactStruc.dLength = 0x28;

    //
    // Bulk in, with temp buffer & 40 bytes of data to read
    //
    dData = USBMassIssueMassTransaction(fpDevInfo, &MassXactStruc);

    if (!dData) {
        USBMassRequestSense( fpDevInfo );
        USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_MODE_SENSE_10CMD));
        return  USB_ERROR;
    }

    //
    // Fill in the output data
    //
    fpModeSense10_Header = (MODE_SENSE_10_HEADER*)gUsbData->fpUSBTempBuffer;

    //
    // Process media type
    //
    fpModeSenseData->bMediaType = fpModeSense10_Header->bMediaType;

    //
    // Position to the correct page code starting location
    //
    fpPageCode5 = (PAGE_CODE_5*)((UINT8*)fpModeSense10_Header +
                                        fpModeSense10_Header->wBlkDescSize +
                                        sizeof (MODE_SENSE_10_HEADER));
//  USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBMassModeSense ....  fpPageCode5->bPageCode %x\n",fpPageCode5->bPageCode);

    bRetCode = USB_ERROR;
    if(fpPageCode5->bPageCode == 5) {
        //
        // Process number of bytes per sector (the block size)
        //
        fpModeSenseData->wBytesPerSector = (UINT16)((fpPageCode5->wBlockSize << 8)
                                            + (fpPageCode5->wBlockSize >>8));
        //
        // Process number of heads and number of sectors/track
        //
        fpModeSenseData->bNumHeads = fpPageCode5->bHeads;
        fpModeSenseData->bNumSectors = fpPageCode5->bSectors;

        //
        // Process number of cylinders
        //
        fpModeSenseData->wNumCylinders  = (UINT16)((fpPageCode5->wCylinders << 8)
                                        + (fpPageCode5->wCylinders >> 8));
        bRetCode = USB_SUCCESS;
    }

    fpModeSenseData->dSenseData = USBMassRequestSense( fpDevInfo );

    USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_MODE_SENSE_10CMD));
// USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USBMProModeSense ....  wRetCode %x\n",wRetCode);

    return bRetCode;

}


/**
    This function sends request sense command and returns
    the sense key information

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Sense data

**/

UINT32
USBMassRequestSense(DEV_INFO* fpDevInfo)
{
    UINT32                      dData;
    UINT8                       *fpDataBuffer;
    COMMON_REQ_SENSE_CMD        *fpCmdBuffer;
    MASS_XACT_STRUC             MassXactStruc;

    //
    // Allocate memory for the command buffer
    //
    fpCmdBuffer = (COMMON_REQ_SENSE_CMD*)USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(COMMON_REQ_SENSE_CMD));
    if(!fpCmdBuffer) {
        return USB_ERROR;   // Error - return no sense data <>0
    }

    fpDataBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT(1));
    if(!fpDataBuffer) {
        return USB_ERROR;   // Error - return no sense data <>0
    }

    //
    // Load command into (just allocated) mass command buffer
    //
    fpCmdBuffer->bOpCode = COMMON_REQUEST_SENSE_OPCODE;
    fpCmdBuffer->bAllocLength = 0x12;   // Length of transfer

    USBMassClearMassXactStruc(&MassXactStruc);    // Clear the common bulk transaction structure

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer  = (UINT8*)fpCmdBuffer;
                                        //(EIP51158+)>
    if (fpDevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x06;	//SPC-4_350
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMMON_REQ_SENSE_CMD);
    }
										//<(EIP51158+)
    MassXactStruc.bXferDir = BIT7;     // IN
    MassXactStruc.fpBuffer = fpDataBuffer;
    MassXactStruc.dLength  = 0x12;

    //
    // Bulk in, with locally allocated temp buffer & 12h bytes of data to read
    //
    dData = USBMassIssueMassTransaction(fpDevInfo, &MassXactStruc);

    if(dData) {
        //
        // Form the return value:
        //      Bit 0..7    - Sense key (offset 002d)
        //      Bit 8..15   - ASC code (offset 012d)
        //      Bit 16..23  - ASCQ code (offset 013d)
        //
        dData = (UINT32)(fpDataBuffer[2] +
                    (fpDataBuffer[12] << 8) +
                    (fpDataBuffer[13] << 16));
        USBMassSenseKeyParsing(fpDevInfo, dData);
    }
							//(EIP20863+)>
	else
		dData = USB_ERROR;
							//<(EIP20863+)

    USB_MemFree(fpCmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMMON_REQ_SENSE_CMD));
    USB_MemFree(fpDataBuffer, GET_MEM_BLK_COUNT(1));

    return dData;
}


/**
    Translate USB sense key to USB MassStorage status.

    @param fpDevInfo   Pointer to DeviceInfo structure
        dCode[23..16]   ASCQ
        dCode[15..08]   ASC
        dCode[07..00]   Sense Code


    @retval Sense data

**/

VOID
USBMassSenseKeyParsing(DEV_INFO* fpDevInfo, UINT32 dCode)
{
    if ((UINT16)dCode == 0x3A02) {		//(EIP86793)
        fpDevInfo->bLastStatus &= ~USB_MASS_MEDIA_PRESENT;
    }
    if((UINT16)dCode == 0x2806) {
        fpDevInfo->bLastStatus |= (USB_MASS_MEDIA_PRESENT | USB_MASS_MEDIA_CHANGED);
    }
										//(EIP86125+)>
    if(dCode == 0) {
        fpDevInfo->bLastStatus |= USB_MASS_MEDIA_PRESENT;
    }
										//<(EIP86125+)
}


/**
    This function sends test unit ready command

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Sense data

**/

UINT32
USBMassTestUnitReady(
    DEV_INFO* DevInfo
)
{
    COMN_TEST_UNIT_READY_CMD        *CmdBuffer;
    UINT32                          Data;
    MASS_XACT_STRUC                 MassXactStruc;

    CmdBuffer = (COMN_TEST_UNIT_READY_CMD*)USB_MemAlloc(
                    GET_MEM_BLK_COUNT_STRUC(COMN_TEST_UNIT_READY_CMD));
    if (!CmdBuffer) {
        return USB_ERROR;       // Error - return no sense data
    }

    CmdBuffer->bOpCode = COMMON_TEST_UNIT_READY_OPCODE;
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USBMassTestUnitReady ....  \n");

    USBMassClearMassXactStruc(&MassXactStruc);    // Clear the common bulk transaction structure

    //
    // Fill the common bulk transaction structure
    //
    MassXactStruc.fpCmdBuffer = (UINT8*)CmdBuffer;
                                        //(EIP51158+)>
    if (DevInfo->bSubClass == SUB_CLASS_SCSI) {
		MassXactStruc.bCmdSize = 0x06;	//SPC-4_368
    } else {
    	MassXactStruc.bCmdSize = sizeof (COMN_TEST_UNIT_READY_CMD);
    }
										//<(EIP51158+)
    Data = USBMassIssueMassTransaction(DevInfo, &MassXactStruc);

    USB_MemFree(CmdBuffer, GET_MEM_BLK_COUNT_STRUC(COMN_TEST_UNIT_READY_CMD));

    if ((Data == USB_ERROR) || (DevInfo->bProtocol == PROTOCOL_CBI) || 
        (DevInfo->bProtocol == PROTOCOL_CBI_NO_INT)) {
        Data = USBMassRequestSense(DevInfo);
    }

    return Data;
}


/**
    This function makes sure the device is ready for next
    command

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Sense code

**/

UINT32
USBMassCheckDeviceReady (DEV_INFO* fpDevInfo)
{
    UINT8   count, nomedia_count;
    UINT8   NotReadyCount; 				//(EIP101623+)
    UINT32  dData = 0;

    count = gUsbData->bUSBStorageDeviceDelayCount;
    nomedia_count = 3;
    NotReadyCount = 3;  				//(EIP101623+)
    while (count) {
        if (fpDevInfo->wIncompatFlags & USB_INCMPT_TEST_UNIT_READY_FAILED) {
            break;  // consider device is ready
        }

        //
        // Issue test unit ready command and check the return value
        //
        dData = USBMassTestUnitReady( fpDevInfo );
//USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "(%d)tur..%x ", fpDevInfo->bDeviceAddress, dData);
        if ((UINT8)dData == 0) { // Device ready
            break;
        }
        //
        // Device is not ready.
        // Check for getting ready/reset command occurence in dData:
        //      Bit 0..7    - Sense Code
        //      Bit 8..15   - Additional Sense Code (ASC)
        //      Bit 16..23  - Additional Sense Code Qualifier (ASCQ)
        //
        if ((UINT16)dData == 0x2806) {
            //
            // Send Start/Stop Unit command to UFI class device only
            //
            if (fpDevInfo->bSubClass == SUB_CLASS_UFI) {
                USBMassStartUnitCommand (fpDevInfo);
            }
            FixedDelay(100 * 1000);        // 100 msec delay
            count--;
            continue;
        }
        if ((UINT16)dData == 0x3A02) {			// Media is not present
            nomedia_count--;
            if (nomedia_count == 0) return  dData;  // No media
            FixedDelay(20 * 1000);        // 20 msec delay
            count--;
            continue;
        }

		if (dData == 0x020402)
		{
			USBMassStartUnitCommand (fpDevInfo);
			FixedDelay(100 * 1000);
			count--;
			continue;
		}

		if ((UINT16)dData == 0x1103) {
			FixedDelay(100 * 1000);
			count--;
			continue;
		}

        //
        // Check whether we can recover from this error condition
        // Currently only recoverable error condition are
        // 1. Device is getting ready (010402)
        // 2. Device reset occurred (002906)
        //
        if (dData != 0x010402) {
            //
            // Check for write protected command
            //
            if ( (UINT8)dData == 7 ) {
                break;
            }
            if (((UINT8)dData != 0x06) && ((UINT8)dData != 0x02)) {
                return  dData;
            }
        }

                                        //(EIP101623+)>
        if (dData == 0x02) {			
            NotReadyCount--;
            if (NotReadyCount == 0) return  dData;  
            FixedDelay(20 * 1000); // 20 msec delay
            count--;
            continue;
        }
                                        //<(EIP101623+)
            
        //
        // Prepare for the next itaration
        // Delay for the device to get ready
        //
        FixedDelay(1000 * 1000);       // 1 sec delay
        count--;
    }   // while
                                        //(EIP53416+)>
    if (count == 0) {
        return dData;
    }
                                        //<(EIP53416+)
    return USBMassUpdateDeviceGeometry(fpDevInfo);
}


/**
    This function obtains the device geometry from the device
    using mode sense command and updates the global variables

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT8
USBMassUpdateParamUsingModeSense(DEV_INFO* fpDevInfo)
{
    MASS_MODE_SENSE ModeSenseData;

    gUsbData->wModeSenseCylinders = gUsbData->bModeSenseHeads =
                                gUsbData->bModeSenseSectors = 0;
    //
    // Check the compatibility flag for mode sense support
    //
    if (fpDevInfo->wIncompatFlags & USB_INCMPT_MODE_SENSE_NOT_SUPPORTED) {
        return  USB_SUCCESS;
    }

    //
    // CDROM devices never support mode sense page code 5 (Flexible disk page)
    // so skip it
    //
    if (fpDevInfo->bStorageType == USB_MASS_DEV_CDROM) {
        return  USB_ERROR;
    }

    //
    // Issue mode sense command
    //
    if (USBMassModeSense(fpDevInfo, &ModeSenseData)) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "ms..err ");
        return  USB_ERROR;
    }

    //
    // Mode sense is supported. Update the local structure.
    //
    gUsbData->wModeSenseCylinders  = ModeSenseData.wNumCylinders;  // Number of cylinders
    gUsbData->bModeSenseHeads      = ModeSenseData.bNumHeads;      // Number of heads
    gUsbData->bModeSenseSectors    = ModeSenseData.bNumSectors;    // Number of sectors
    gUsbData->wModeSenseBlockSize  = ModeSenseData.wBytesPerSector;// Number of bytes per sector
    gUsbData->bDiskMediaType       = ModeSenseData.bMediaType;     // Media type


    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "ms..%x %x %x %x %x ",
            gUsbData->wModeSenseCylinders,
            gUsbData->bModeSenseHeads,
            gUsbData->bModeSenseSectors,
            gUsbData->wModeSenseBlockSize,
            gUsbData->bDiskMediaType);

    if (fpDevInfo->bStorageType == USB_MASS_DEV_HDD) {
        gUsbData->bDiskMediaType = USB_UNKNOWN_MEDIA_TYPE;
    }

    //
    // Calculate and update Max LBA
    //
    gUsbData->dModeSenseMaxLBA =
            (UINT32)(ModeSenseData.wNumCylinders *
            ModeSenseData.bNumHeads * ModeSenseData.bNumSectors);
    //
    // Set the flag indicating mode sense is executed
    //
    gUsbData->bGeometryCommandStatus |= MODE_SENSE_COMMAND_EXECUTED;

    return  USB_SUCCESS;
}


/**
    This function obtains the device geometry from the device
    using read capacity command and updates the global variables

    @param Pointer to DeviceInfo structure

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT8
USBMassUpdateParamUsingReadCapacity(
    DEV_INFO* DevInfo
)
{
    UINT8   Sectors;
    UINT8   Heads;

    //
    // Either mode sense not supported or failed. Try read capacity
    // Issue read capacity command
    //
    if (USBMassReadCapacity10Command(DevInfo)) {
        return  USB_ERROR;
    }

    //
    // Set the flag indicating read capacity is executed
    //
    gUsbData->bGeometryCommandStatus |= READ_CAPACITY_COMMAND_EXECUTED;

    //
    // Max LBA & block size are updated in MassDeviceInfo structure
    //
    if (DevInfo->MaxLba < 0x4000) {    //  last LBA < 16MB
        switch (DevInfo->MaxLba) {
            case USB_144MB_FDD_MAX_LBA:
                gUsbData->bReadCapHeads    = USB_144MB_FDD_MAX_HEADS;
                gUsbData->bReadCapSectors  = USB_144MB_FDD_MAX_SECTORS;
                gUsbData->wReadCapCylinders= USB_144MB_FDD_MAX_CYLINDERS;
                gUsbData->bDiskMediaType   = USB_144MB_FDD_MEDIA_TYPE;
                return  USB_SUCCESS;

            case USB_720KB_FDD_MAX_LBA:
                gUsbData->bReadCapHeads    = USB_720KB_FDD_MAX_HEADS;
                gUsbData->bReadCapSectors  = USB_720KB_FDD_MAX_SECTORS;
                gUsbData->wReadCapCylinders= USB_720KB_FDD_MAX_CYLINDERS;
                gUsbData->bDiskMediaType   = USB_720KB_FDD_MEDIA_TYPE;
                return  USB_SUCCESS;
        }
    }

    //
    // Convert to CHS
    //
    gUsbData->wReadCapBlockSize = DevInfo->wBlockSize;

    //
    // Do CHS conversion
    // Use fixed sectors/track & heads for CHS conversion
    //
    if (DevInfo->MaxLba < 0x400) {   // < 512 KB
        Sectors    = 1;
        Heads      = 1;
    }
    else {
        if (DevInfo->MaxLba < 0x200000) {  // < 1GB
            Sectors    = USB_FIXED_LBA_SPT_BELOW_1GB;
            Heads      = USB_FIXED_LBA_HPT_BELOW_1GB;
        }
        else {                                  // > 1GB
            Sectors    = USB_FIXED_LBA_SPT_ABOVE_1GB;
            Heads      = USB_FIXED_LBA_HPT_ABOVE_1GB;
        }
    }

    gUsbData->bReadCapSectors  = Sectors;
    gUsbData->bReadCapHeads    = Heads;

    //
    // Calculate number of cylinders Cyl = LBA/(Head*Sec)
    //
    if ((Sectors != 0) && (Heads != 0)) {
        gUsbData->wReadCapCylinders = (UINT16)DivU64x64Remainder(DevInfo->MaxLba, (Sectors * Heads), NULL);
    } else {
        gUsbData->wReadCapCylinders = 0;
    }

    return  USB_SUCCESS;

}


/**
    This function updates the device geometry information

    @param Pointer to device info structure

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
USBMassUpdateDeviceGeometry(
    DEV_INFO* DevInfo
)
{
    UINT64          MaxLba;
    UINT8           Heads;
    UINT8           Sectors;
    UINT16          Cylinders;
    UINT8           Status;
    EFI_STATUS      EfiStatus = EFI_SUCCESS;
    USB_MASS_DEV    *MassDev;

    //
    // Try to update geometry if it is not valid
    // "Valid" block size is 1...FFFE
    // Additional check added to ensure the head, sector, and cylinder values are non-zero.
    //
                            //(EIP13457+)>
    if ((DevInfo->Heads != 0) && 
		(DevInfo->bSectors != 0) &&
		(DevInfo->wCylinders != 0) &&
        !(DevInfo->bLastStatus & USB_MASS_MEDIA_CHANGED) &&
        (!((DevInfo->bLastStatus & USB_MASS_GET_MEDIA_FORMAT) &&
        (DevInfo->bSubClass == SUB_CLASS_UFI)))) {

        DevInfo->bLastStatus &= ~USB_MASS_GET_MEDIA_FORMAT;

        if (DevInfo->wBlockSize && (DevInfo->wBlockSize != 0xFFFF)) {
            return USB_SUCCESS;
        }
    }

    DevInfo->bLastStatus &= ~USB_MASS_GET_MEDIA_FORMAT;

    //
    // Set default values for the global variables
    //
    gUsbData->bDiskMediaType = USB_UNKNOWN_MEDIA_TYPE;
    gUsbData->bGeometryCommandStatus &= ~(MODE_SENSE_COMMAND_EXECUTED |
                    READ_CAPACITY_COMMAND_EXECUTED);

    //
    // Get disk geometry using Mode Sense
    //
    if (DevInfo->bSubClass == SUB_CLASS_UFI) {  //(EIP94060)
	    USBMassUpdateParamUsingModeSense(DevInfo);
    }

    //
    // Get disk geometry using Read Capacity
    //
    Status = USBMassUpdateParamUsingReadCapacity(DevInfo);

    //
    // Parameters are obtained and stored in respective global variables;
    // check whether any of the commands executed.
    //
    if (!(gUsbData->bGeometryCommandStatus & (READ_CAPACITY_COMMAND_EXECUTED |
                        MODE_SENSE_COMMAND_EXECUTED)))  {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "-error\n");
        return USB_ERROR;
    }

    //
    // Check whether read capacity is executed. If so, then max LBA & block size
    // are already updated in the MassDeviceInfo structure. If not update it using
    // mode sense parameters
    //
    if (!(gUsbData->bGeometryCommandStatus & READ_CAPACITY_COMMAND_EXECUTED)) {
        //
        // At this point we made sure atleast one of the command (Mode sense or Read
        // Capacity) was executed. So if one command is not executed then other
        // command is surely executed.
        //

        //
        // Update the max LBA & block size using mode sense parameters
        //
        DevInfo->wBlockSize = gUsbData->wModeSenseBlockSize;
        DevInfo->MaxLba = gUsbData->dModeSenseMaxLBA;
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "size %x lba %lx\n", DevInfo->wBlockSize, DevInfo->MaxLba);
    }

    //
    // Update Efi BlockIo device
    //
    if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
        if (DevInfo->MassDev) {
#if !USB_RT_DXE_DRIVER
            EfiStatus = AmiValidateMemoryBuffer((VOID*)DevInfo->MassDev, sizeof(USB_MASS_DEV));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            MassDev = DevInfo->MassDev;

#if !USB_RT_DXE_DRIVER
            EfiStatus = AmiValidateMemoryBuffer((VOID*)MassDev->Media, sizeof(EFI_BLOCK_IO_MEDIA));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            MassDev->Media->BlockSize = DevInfo->wBlockSize;
            if ((DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT) && 
                (DevInfo->MaxLba != 0) && (DevInfo->wBlockSize != 0)) {
                // For SCSI devices, this is reported in the READ CAPACITY (16) parameter 
                // data Returned Logical Block Address field (see SBC-3) minus one.
                MassDev->Media->LastBlock = DevInfo->MaxLba - 1;
            } else {
                MassDev->Media->LastBlock = 0; 
            }
        }
    }

    //Some usb mass storages report media change even if they don't, we already 
    //update CHS from boot record and legacy boot doesn't support dynamic 
    //media insertion, we should not update it from read capacity parameters.

    if ((DevInfo->Heads != 0) && (DevInfo->bSectors != 0) && (DevInfo->wCylinders !=0)) {
            return USB_SUCCESS;
    }

    //
    // Update the media type byte
    //
    DevInfo->bMediaType = gUsbData->bDiskMediaType;

    //
    // Check whether mode sense is executed. If so, then update CHS from mode
    // sense value or else update from read capacity values.
    //

    //
    // Update the CHS values using mode sense parameters
    //
    Heads      = gUsbData->bModeSenseHeads;
    Sectors    = gUsbData->bModeSenseSectors;
    Cylinders  = gUsbData->wModeSenseCylinders;

//  if ((gUsbData->bGeometryCommandStatus & MODE_SENSE_COMMAND_EXECUTED) &&
    if ((Heads * Sectors * Cylinders) == 0) {
        //
        // Update the CHS values using read capacity parameters
        //
        Heads      = gUsbData->bReadCapHeads;
        Sectors    = gUsbData->bReadCapSectors;
        Cylinders  = gUsbData->wReadCapCylinders;
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "Cyl-%x, Hds-%x, Sec-%x", Cylinders, Heads, Sectors);

    DevInfo->Heads       = Heads;
    DevInfo->bSectors     = Sectors;
    DevInfo->wCylinders   = Cylinders;

    //
    // Calculate non-LBA CHS values from max LBA
    //
    MaxLba = DevInfo->MaxLba;

    //
    // Do not translate sectors for non HDD devices
    //
    if ((!DevInfo->bStorageType) || (DevInfo->bStorageType == USB_MASS_DEV_HDD)) {
        //
        // If Total number of sectors < 1032192(0FC000h) CHS translation is not
        // needed
        //
        if (MaxLba >= 0xFC000) {
            Sectors    = 63;
            Heads      = 32;
            //
            // If Total number of sectors < 2064384(01F8000h) then use
            // 63 Sec/track and 32 head for translation
            //
            if (MaxLba >= 0x01F8000) {
                Heads = 64;
                //
                // If Total number of sectors < 4128768(03F0000h) then use
                // 63 Sec/track and 64 head for translation
                //
                if (MaxLba >= 0x03F0000) {
                    Heads = 128;
                    //
                    // If Total number of sectors < 8257536(07E0000h) then use
                    // 63 Sec/track and 128 head for translation else use 255 heads
                    //
                    if (MaxLba >= 0x7E0000) {
                        Heads      = 255;
                        MaxLba = DevInfo->MaxLba;
                    }
                }
            }
        }

        //
        // In any case, check the parameters for maximum values allowed by BIOS and
        // ATA specs (that is, 1024 cylinders, 16 heads and 63 sectors per track)
        //
        for (;;) {
            //
            // Calculate translated number of cylinders
            //
            if ((Sectors != 0) && (Heads != 0)) {
                Cylinders = (UINT16)DivU64x64Remainder(MaxLba, (Heads * Sectors), NULL);
            } else {
                Cylinders = 0;
            }

            //
            // Check whether number of cylinders is less than or equal to 1024
            //
            if (Cylinders <= 1024) break;

            //
            // Cylinders are getting larger than usually supported try increasing
            // head count keeping cylinders within safe limit
            //
            Cylinders = 1024;
            if (Heads == 0xFF) {
                break;  // Heads limit reached
            }
            //
            // Double number of heads
            //
            Heads <<= 1;
            if (!Heads) {
                Heads = 0xFF;
            }
        }
    }

    //
    // Save the parameters
    //
    DevInfo->NonLBAHeads     = Heads;
    DevInfo->bNonLBASectors   = Sectors;
    DevInfo->wNonLBACylinders = Cylinders;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "BPS %d H %d S %d C %d MT %d\n",
        DevInfo->wBlockSize,
        DevInfo->Heads,
        DevInfo->bSectors,
        DevInfo->wCylinders,
        DevInfo->bMediaType);

    return USB_SUCCESS;

}

/**
    This function performs a mass storage transaction using bulk
    only transport (BOT) protocol.

    @param Pointer to DeviceInfo structure
        stMassXactStruc
        pCmdBuffer  Pointer to command buffer
        bCmdSize    Size of command block
        bXferDir    Transfer direction
        fpBuffer    Data buffer far pointer
        dwLength    Amount of data to be transferred
        wPreSkip    Number of bytes to skip before data
        wPostSkip   Number of bytes to skip after data

    @retval Amount of data actually transferred

**/

UINT32
USBMassIssueBOTTransaction(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
    UINT32  Data;
    UINT8   Status;

    Data = USBMassSendBOTCommand(DevInfo, MassXactStruc);   // Send the command control transfer

    if (!Data) {
        //
        // Check for stall/timedout condition
        //
        if (gUsbData->bLastCommandStatus & (USB_BULK_STALLED + USB_BULK_TIMEDOUT)) {
            //
            // Perform USB BOT reset recovery
            //
            USBMassBOTResetRecovery(DevInfo);
            return 0;
        }
        else {
            return 0;  // Unknown error exit
        }
    }

    if (!MassXactStruc->dLength) {  // No data
        if (gUsbData->wBulkDataXferDelay) {
            //
            // Issue some delay
            //
            FixedDelay(100 * 1000);
            gUsbData->wBulkDataXferDelay   = 0;
        }
        //
        // Get the status for the last transfer
        //
        Data = USBMassGetBOTStatus(DevInfo, MassXactStruc);
        return Data;
    }

    //
    // Tranfer the bulk data
    //
    Data = USBMassProcessBulkData(DevInfo, MassXactStruc);  // Actual data size

    //
    // Check for stall/timeout condition
    //
    if (!(gUsbData->bLastCommandStatus & (USB_BULK_STALLED + USB_BULK_TIMEDOUT))) {
        //
        // Get the status for the last transfer
        //
        Status = USBMassGetBOTStatus(DevInfo, MassXactStruc);
		if ((Status == USB_ERROR) || (gUsbData->bLastCommandStatus & USB_BULK_TIMEDOUT)) {
			return 0;
		} else {
        	return Data;
		}
    }

    //
    // Check for time out condition
    //
    if (gUsbData->bLastCommandStatus & USB_BULK_TIMEDOUT) {
        //
        // Perform USB BOT reset recovery
        //
        USBMassBOTResetRecovery(DevInfo);
        return 0;
    }

    //
    // Clear endpoint stall
    //
    USBMassClearBulkEndpointStall(DevInfo, MassXactStruc->bXferDir);

    //
    // Get the status for the last transfer
    //
    USBMassGetBOTStatus(DevInfo, MassXactStruc);

    return Data;

}

/**
    This function performs a mass storage transaction using bulk
    only transport (BOT) protocol.

    @param fpDevInfo   Pointer to DeviceInfo structure
        bXferDir    Transfer direction
        dwDataSize  Amount of data to be transferred
        fpCmdBuffer Pointer to the command buffer
        bCmdSize    Size of command block

    @retval Amount of data actually transferred

**/

UINT16
USBMassSendBOTCommand(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
    UINT8           Count;
    UINT8           *Src;
    UINT8           *Dest;
    BOT_CMD_BLK     *BotCmdBlk;
    UINT8           CmdSize;

    BotCmdBlk = (BOT_CMD_BLK*)MassXactStruc->fpCmdBuffer;

    CmdSize = MassXactStruc->bCmdSize;

    //
    // Make enough space for BOT command block wrapper
    // Move backwards
    //
    Src = MassXactStruc->fpCmdBuffer + CmdSize - 1;

    //
    // BOT_COMMAND_BLOCK + end of command
    //
    Dest = Src + ((UINT8*)BotCmdBlk->aCBWCB - (UINT8*)BotCmdBlk);

    for (Count = 0; Count < CmdSize; Count++) {
        *Dest = *Src;
        --Dest;
        --Src;
    }

    //fpDest = gUsbData->stMassXactStruc.fpCmdBuffer;

    //
    // Clear the BOT command block
    //
    //for (bCount = 0; bCount < bCmdSize; bCount++) {
    //    *fpDest = 0x00;
    //    ++fpDest;
    //}

    BotCmdBlk->dCbwSignature      = BOT_CBW_SIGNATURE;
    BotCmdBlk->dCbwTag            = ++(gUsbData->dBOTCommandTag);
    BotCmdBlk->dCbwDataLength     = MassXactStruc->dLength;
    BotCmdBlk->bmCbwFlags         = MassXactStruc->bXferDir;
    BotCmdBlk->bCbwLun            = DevInfo->bLUN;
    BotCmdBlk->bCbwLength         = CmdSize;

    return (UINT16)USBMassIssueBulkTransfer(
                    DevInfo,
                    0,
                    (UINT8*)BotCmdBlk,
                    sizeof (BOT_CMD_BLK));
    
}

/**
    This function gets the BOT status sequence using
    bulk IN transfer

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval Nothing

**/

UINT8
USBMassGetBOTStatus(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
										//(EIP90503)>
    UINT8*  CmdBuffer;
    UINT16  Data;

    CmdBuffer = MassXactStruc->fpCmdBuffer;

    Data = (UINT16)USBMassIssueBulkTransfer(DevInfo, BIT7,
                    CmdBuffer, sizeof (BOT_STATUS_BLOCK));
    if ((Data != sizeof (BOT_STATUS_BLOCK))) {
        if (gUsbData->bLastCommandStatus & USB_BULK_STALLED) {
            USBMassClearBulkEndpointStall(DevInfo, BIT7);
        }
        Data = (UINT16)USBMassIssueBulkTransfer(DevInfo, BIT7,
                    CmdBuffer, sizeof (BOT_STATUS_BLOCK));
        if (gUsbData->bLastCommandStatus & USB_BULK_STALLED) {
            USBMassBOTResetRecovery(DevInfo);
            return USB_ERROR;
        }
    }

    //
    // Check for valid CSW
    //
    if ((Data != sizeof (BOT_STATUS_BLOCK)) ||
        (((BOT_STATUS_BLOCK*)CmdBuffer)->dCswSignature != BOT_CSW_SIGNATURE) ||
        (((BOT_STATUS_BLOCK*)CmdBuffer)->dCswTag != gUsbData->dBOTCommandTag)) {
        //USBMassClearBulkEndpointStall(fpDevInfo, BIT7);	//(EIP63308-)
        //USBMassClearBulkEndpointStall(fpDevInfo, BIT0);	//(EIP63308-)
        return USB_ERROR;
    }
										//<(EIP90503)
    //
    // Check for meaningful CSW
    //
    if (((BOT_STATUS_BLOCK*)CmdBuffer)->bmCswStatus) {
   		if (((BOT_STATUS_BLOCK*)CmdBuffer)->bmCswStatus > 1) {
	        //
	        // Perform reset recovery if BOT status is phase error
	        //
        	USBMassBOTResetRecovery(DevInfo);
    	}
		return USB_ERROR;
	}

    return USB_SUCCESS;
}

/**
    This function performs the BOT reset recovery

    @param fpDevInfo   Pointer to DeviceInfo structure

**/

VOID
USBMassBOTResetRecovery(DEV_INFO* fpDevInfo)
{
    (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable
                [fpDevInfo->bHCNumber - 1]->bHCType)].pfnHCDControlTransfer)
                (gUsbData->HcTable[fpDevInfo->bHCNumber - 1],
      																			//(EIP20863)>
                //fpDevInfo, ADSC_OUT_REQUEST_TYPE,
                //(UINT16)fpDevInfo->bInterfaceNum,BOT_RESET_REQUEST_CODE, 0, 0);
				fpDevInfo, ADSC_OUT_REQUEST_TYPE + (BOT_RESET_REQUEST_CODE << 8),
                (UINT16)fpDevInfo->bInterfaceNum, 0, 0, 0);
																				//<(EIP20863)
    USBMassClearBulkEndpointStall(fpDevInfo, BIT7);
    USBMassClearBulkEndpointStall(fpDevInfo, BIT0);
}

/**
    This function gets the maximum logical unit number(LUN)
    supported by the device.  It is zero based value.

    @param Pointer to DeviceInfo structure

    @retval Max LUN supported

**/

UINT16
USBMassBOTGetMaxLUN(
    DEV_INFO* DevInfo
)
{
	UINT8	*Buffer = NULL;
	UINT8	MaxLun = 0;
    UINT16  Status;

    if (DevInfo->wIncompatFlags & USB_INCMPT_GETMAXLUN_NOT_SUPPORTED) {
        return 0;
    }

	Buffer = USB_MemAlloc(1);
	ASSERT(Buffer);
	if (Buffer == NULL) {
		return 0;
	}

    Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable
            [DevInfo->bHCNumber - 1]->bHCType)].pfnHCDControlTransfer)
            (gUsbData->HcTable[DevInfo->bHCNumber - 1],
            DevInfo, ADSC_IN_REQUEST_TYPE + (BOT_GET_MAX_LUN_REQUEST_CODE << 8),
            DevInfo->bInterfaceNum, 0, Buffer, 1);
    if (Status) {
	    MaxLun = *Buffer;
    }
	USB_MemFree(Buffer, 1);

    return MaxLun;
}


/**
    This function performs a mass storage transaction using CBI
    or CB protocol.

    @param fpDevInfo   Pointer to DeviceInfo structure
        fpCmdBuffer Pointer to command buffer
        bCmdSize    Size of command block
        bXferDir    Transfer direction
        fpBuffer    Data buffer far pointer
        dwLength    Amount of data to be transferred
        wPreSkip    Number of bytes to skip before data
        wPostSkip   Number of bytes to skip after data

    @retval Amount of data actually transferred

**/

UINT32
USBMassIssueCBITransaction(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc    
)
{
    UINT32  Data = 0;

    if (!(USBMassSendCBICommand(DevInfo, MassXactStruc))) {  // Returns 0 on error
        return  0;
    }

    if (MassXactStruc->dLength) {
        Data = USBMassProcessBulkData(DevInfo, MassXactStruc);
        if (!Data) {
            if(gUsbData->bLastCommandStatus & USB_BULK_STALLED) {
                USBMassClearBulkEndpointStall(DevInfo, MassXactStruc->bXferDir);
                return Data;
            }
        }
    }

    if (DevInfo->bProtocol != PROTOCOL_CBI_NO_INT && DevInfo->IntInEndpoint != 0) {
        //
        // Bypass interrupt transaction if it is CB protocol
        //
        USBMassCBIGetStatus(DevInfo);
    }

    return Data;
}


/**
    This function performs a mass storage transaction using CBI
    or CB protocol.

    @param fpDevInfo   Pointer to DeviceInfo structure
        fpCmdBuffer Pointer to the command buffer
        bCmdSize    Size of command block

    @retval 0xFFFF  SUCCESS
        0x00    ERROR

**/

UINT16
USBMassSendCBICommand(
    DEV_INFO*           DevInfo,
    MASS_XACT_STRUC*    MassXactStruc
)
{
    UINT16  RetValue;

    RetValue = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable
                [DevInfo->bHCNumber - 1]->bHCType)].pfnHCDControlTransfer)
                (gUsbData->HcTable[DevInfo->bHCNumber - 1],
                DevInfo, ADSC_OUT_REQUEST_TYPE,
                (UINT16)DevInfo->bInterfaceNum, 0,
                MassXactStruc->fpCmdBuffer,
                (UINT16)MassXactStruc->bCmdSize);

    return RetValue;

}


/**
    This function gets the status of the mass transaction
    through an interrupt transfer

    @param pDevInfo    Pointer to DeviceInfo structure

    @retval Return value from the interrupt transfer

**/

UINT16
USBMassCBIGetStatus(DEV_INFO*   fpDevInfo)
{
    (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable
        [fpDevInfo->bHCNumber - 1]->bHCType)].pfnHCDInterruptTransfer)
        (gUsbData->HcTable[fpDevInfo->bHCNumber - 1],
         fpDevInfo, fpDevInfo->IntInEndpoint, 
         fpDevInfo->IntInMaxPkt, (UINT8*)&gUsbData->wInterruptStatus, 2);

    return ((UINT16)gUsbData->wInterruptStatus);

}


/**
    This function executes a bulk transaction on the USB. The
    transfer may be either DATA_IN or DATA_OUT packets containing
    data sent from the host to the device or vice-versa. This
    function wil not return until the request either completes
    successfully or completes with error (due to time out, etc.)
    Size of data can be upto 64K

    @param - DeviceInfo structure (if available else 0)
        - Transfer direction
        Bit 7   : Data direction
        0 Host sending data to device
        1 Device sending data to host
        Bit 6-0 : Reserved
        - Buffer containing data to be sent to the device or
        buffer to be used to receive data. Value in
        - Length request parameter, number of bytes of data
        to be transferred in or out of the host controller

    @retval Amount of data transferred

**/

UINT32
USBMassIssueBulkTransfer(DEV_INFO* fpDevInfo, UINT8 bXferDir,
                    UINT8* fpCmdBuffer, UINT32 dSize)
{
    return (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable
                [fpDevInfo->bHCNumber - 1]->bHCType)].pfnHCDBulkTransfer)
                (gUsbData->HcTable[fpDevInfo->bHCNumber -1],
                fpDevInfo, bXferDir,
                fpCmdBuffer, dSize);
}


/**
    This function fills and returns the mass get device geometry
    structure

    @param fpMassGetDevGeo     Pointer to mass get geometry struc

    @retval Return value
        fpMassGetDevGeo     Pointer to mass get geometry struc
        dSenseData  Sense data of the last command
        bNumHeads   Number of heads
        wNumCylinders   Number of cylinders
        bNumSectors Number of sectors
        wBytesPerSector Number of bytes per sector
        bMediaType  Media type

**/

UINT8
USBMassGetDeviceGeometry(
    MASS_GET_DEV_GEO *GetDevGeometry
 )
{
    DEV_INFO    *DevInfo;
    UINT8       DevAddr = GetDevGeometry->bDevAddr;
    BOOLEAN     ValidGeo;
    MASS_GET_DEV_STATUS MassGetDevSts;

    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);

    if ((!DevInfo) || (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT))) {   // Error
        return USB_ERROR;
    }

    MassGetDevSts.bDevAddr = DevAddr;
                        //(EIP13457+)>
    if (GetDevGeometry->bInt13FuncNum == 0x20){
        DevInfo->bLastStatus |= USB_MASS_GET_MEDIA_FORMAT;
    }
    if ((!DevInfo->wBlockSize) || (DevInfo->wBlockSize == 0xFFFF) ||
            (!(DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT) ||
            (GetDevGeometry->bInt13FuncNum == 0x20)) ) {
//      USBMassCheckDeviceReady(fpDevInfo);
        USBMassGetDeviceStatus(&MassGetDevSts);
    }                   //<(EIP13457+)
    ValidGeo = (BOOLEAN)((DevInfo->wBlockSize != 0xFFFF) && (DevInfo->wBlockSize != 0));
    ValidGeo &= (DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT);
                                        //(EIP107198+)>
    GetDevGeometry->wBytesPerSector   = ValidGeo? DevInfo->wBlockSize : 0;
    GetDevGeometry->LBANumHeads       = ValidGeo? DevInfo->Heads : 0;
    GetDevGeometry->bLBANumSectors    = ValidGeo? DevInfo->bSectors : 1;
    GetDevGeometry->wLBANumCyls       = ValidGeo? DevInfo->wCylinders : 0;
    GetDevGeometry->NumHeads          = ValidGeo? DevInfo->NonLBAHeads : 0;
    GetDevGeometry->bNumSectors       = ValidGeo? DevInfo->bNonLBASectors : 1;
    GetDevGeometry->wNumCylinders     = ValidGeo? DevInfo->wNonLBACylinders : 0;
    GetDevGeometry->bMediaType        = DevInfo->bMediaType;
    GetDevGeometry->LastLBA           = ValidGeo? DevInfo->MaxLba : 0;
    GetDevGeometry->BpbMediaDesc      = ValidGeo? DevInfo->BpbMediaDesc : 0;

                                        //<(EIP107198+)

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "BPS %d H %d S %d C %d MT %d\n",
                DevInfo->wBlockSize,
                DevInfo->Heads,
                DevInfo->bSectors,
                DevInfo->wCylinders,
                DevInfo->bMediaType);

    return USB_SUCCESS;

}

/**
    This function issues read capacity command to the mass
    device and returns the value obtained

    @param fpReadCapacity  Pointer to the read capacity structure
        bDevAddr    USB device address of the device

**/

UINT8
USBMassReadCapacity(
    MASS_READ_CAPACITY *ReadCapacity
)
{
    DEV_INFO    *DevInfo;
    UINT8       DevAddr = ReadCapacity->bDevAddr;

    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);

    if ((!DevInfo) || (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT))) {   // Error
        return USB_ERROR;
    }

    return USBMassReadCapacity10Command(DevInfo);
}

/**
    This function swaps the bytes in dword: 0-3,1-2,2-1,3-0. Can be
    used for example in little endian->big endian conversions.

    @param DWORD to swap

    @retval Input value with the swapped bytes in it.

**/

UINT32 dabc_to_abcd(UINT32 dData)
{
    return (((dData & 0x000000FF) << 24)
            | ((dData & 0x0000FF00) << 8)
            | ((dData & 0x00FF0000) >> 8)
            | ((dData & 0xFF000000) >> 24));
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
