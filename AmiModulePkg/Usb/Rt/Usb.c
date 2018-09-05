#pragma warning(disable: 4001)
#pragma warning(disable: 4127)
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

/** @file Usb.c
    AMI USB main wrapper

**/

#include <Token.h>
#include "AmiDef.h"
#include "UsbDef.h"

#include <Pci.h>
#include <Library/BaseMemoryLib.h>

#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#include <Library/AmiUsbSmmGlobalDataValidationLib.h>
#endif

BOOLEAN     gKeyRepeatStatus = FALSE;
extern BOOLEAN  gCheckUsbApiParameter;

BOOLEAN	OEMSkipList(UINT8,UINT8,UINT16,UINT8,UINT8);		//(EIP74609+)
VOID    UsbOemCheckNonCompliantDevice(HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT16);

#if USB_DEV_HUB
UINT8   USBHub_EnablePort(HC_STRUC*, UINT8, UINT8);
UINT8   USBHub_DisablePort(HC_STRUC*, UINT8, UINT8);
UINT8   USBHub_ResetPort(HC_STRUC*, UINT8, UINT8);
//VOID    USBHubFillDriverEntries (DEV_DRIVER*);    //(EIP71750-)
UINT32  USBHub_GetPortStatus(HC_STRUC*, UINT8, UINT8, BOOLEAN);
#endif

VOID    SpeakerBeep (UINT8, UINT16, HC_STRUC*);
VOID    FixedDelay(UINTN);
//VOID    BusFillDriverEntries(DEV_DRIVER*);    //(EIP71750-)
										//(EIP38434+)>
//#if USB_DEV_KBD
//VOID    USBKBDFillDriverEntries (DEV_DRIVER*);
//#endif
//#if USB_DEV_MOUSE
//VOID    USBMSFillDriverEntries (DEV_DRIVER*);
//#endif
                                        //(EIP71750-)>                                       
//VOID    USBHIDFillDriverEntries (DEV_DRIVER*);
										//<(EIP38434+)
//#if USB_DEV_MASS
//VOID    USBMassFillDriverEntries (DEV_DRIVER*);
//#endif
//#if USB_DEV_CCID
//VOID    USBCCIDFillDriverEntries (DEV_DRIVER*);
//#endif
                                        //<(EIP71750-)

VOID USBAPI_CheckDevicePresence (URP_STRUC*);

extern  USB_BADDEV_STRUC gUsbBadDeviceTable[];				//(EIP60706)

DEV_INFO*    USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);    //(EIP98145)
VOID*       USB_MemAlloc(UINT16);
UINT8       USB_MemFree (VOID*, UINT16);
UINT8*      USB_GetDescriptor (HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT8 , UINT8);
UINT8       USB_SetAddress(HC_STRUC*,DEV_INFO*, UINT8);
DEV_INFO*   USBIdentifyAndConfigureDevice (HC_STRUC* , DEV_INFO* , UINT8* , UINT16 ,UINT16 );
UINT8       USB_DisconnectDevice (HC_STRUC*, UINT8, UINT8 );
VOID        USB_InitFrameList (HC_STRUC*, UINT32);
DEV_DRIVER* UsbFindDeviceDriverEntry(DEV_DRIVER*);

UINT8   USB_MemInit (VOID);
UINT8   USBInitHostController(UINT16 *, UINT8);
UINT8   USB_EnumerateRootHubPorts(UINT8);
UINT8   USBLogError(UINT16);
BOOLEAN CheckDeviceLimit(UINT8);
VOID	USB_SmiQueuePutMsg(QUEUE_T*, VOID*, int);

extern  USB_GLOBAL_DATA  *gUsbData;  // Defined in AMIUHCD

#ifdef USB_CONTROLLERS_INITIAL_DELAY_LIST
typedef struct {
    UINT16  Vid;
    UINT16  Did;
    UINT16  DelayTime;
} CONTROLLERS_INITIAL_DELAY_LIST;

CONTROLLERS_INITIAL_DELAY_LIST gControllersInitialDelayList[] = {USB_CONTROLLERS_INITIAL_DELAY_LIST};
#endif

/**
    This function initializes the USB host controllers and
    enumerate the root hub ports for possible USB devices.

    @param GlobalDataArea    Far pointer to the global data area

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_StartHostControllers(
    UINT8* GlobalDataArea
)
{
    return USB_SUCCESS;
}

/**
    This function stops all USB host controllers of a given type

    @param HC type

**/

VOID
StopControllerType(
    UINT8 HcType
)
{
    UINT8 i;
    HC_STRUC*   HcStruc;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "stopping all HC type %x:", HcType);
    for (i = 0; i < gUsbData->HcTableCount; i++) {
        HcStruc = gUsbData->HcTable[i];
        if (HcStruc == NULL) {
            continue;
        }
        if ((HcStruc->bHCType == HcType) &&
            (HcStruc->dHCFlag & HC_STATE_RUNNING)) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStop)(HcStruc);
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, ".");
        }
    }
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\n");
}

/**
    This function start all USB host controllers of a given type

    @param HC type

**/

VOID
StartControllerType(
    UINT8 HcType
)
{
    UINT8 i;
    HC_STRUC*   HcStruc;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "starting all HC type %x:", HcType);
    for (i = 0; i < gUsbData->HcTableCount; i++) {
        HcStruc = gUsbData->HcTable[i];
        if (HcStruc == NULL) {
            continue;
        }
        if (!(HcStruc->dHCFlag & HC_STATE_USED)) {
        	continue;
        }
        if ((HcStruc->bHCType == HcType)) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStart)(HcStruc);
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, ".");
        }
    }
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\n");
}

										//(EIP74876+)>
/**
    This function stops the USB host controllers of a given Bus Dev Function

    @param BusDevFuncNum

    @retval VOID
**/

VOID
StopControllerBdf(
    UINT16 BusDevFuncNum
)
{
    UINT8 i;
    HC_STRUC*   HcStruc;

    for (i = 0; i < gUsbData->HcTableCount; i++) {
        HcStruc = gUsbData->HcTable[i];
        if (HcStruc == NULL) {
            continue;
        }
        if ((HcStruc->wBusDevFuncNum == BusDevFuncNum) &&
            (HcStruc->dHCFlag & HC_STATE_RUNNING)) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStop)(HcStruc);
            break;
        }
    }
}
										//<(EIP74876+)

/**
    This function stops the USB host controllers and
    frees the data structures associated with the host controllers
    In case of USB2.0 first stop USB1.1 controllers, then USB2.0.

    @param VOID

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_StopHostControllers (VOID)
{
    // Suppress disconnect beeps as they might be confusing
    gUsbData->dUSBStateFlag  &= (~USB_FLAG_ENABLE_BEEP_MESSAGE);

    StopControllerType(USB_HC_XHCI);    				//(EIP52339+)
    StopControllerType(USB_HC_UHCI);
    StopControllerType(USB_HC_OHCI);
    StopControllerType(USB_HC_EHCI);

    return USB_SUCCESS;
}

/**
    This function initializes the USB host controller and
    enumerate the root hub ports for possible USB devices.

    @param HcStruc		HC struc pointer

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
UsbHcStart(HC_STRUC* HcStruc)
{
    UINT8   Index = 0;
    
	USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Starting HC %X, HCNum %d, type %x\n",
                    HcStruc->wBusDevFuncNum, HcStruc->bHCNumber, HcStruc->bHCType);

	// Execute start routine of the host controller driver
	(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStart)(HcStruc);

	// Check if the HC is running
	if ((HcStruc->dHCFlag & HC_STATE_RUNNING) == 0) {
		return USB_ERROR;
	}

    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        if (gUsbData->HcTable[Index] == NULL) {
            continue;
        }
        if (HcStruc == gUsbData->HcTable[Index]) {
            continue;
        }
        if (HcStruc->BaseAddress == gUsbData->HcTable[Index]->BaseAddress) {
            break;
        }
        if (HcStruc->fpFrameList != NULL) {
            if (HcStruc->fpFrameList == gUsbData->HcTable[Index]->fpFrameList) {
                break;
            }
        }
        if (HcStruc->usbbus_data != NULL) {
            if (HcStruc->usbbus_data == gUsbData->HcTable[Index]->usbbus_data) {
                break;
            }
        }
#if USB_RUNTIME_DRIVER_IN_SMM
        if (HcStruc->wBusDevFuncNum == gUsbData->HcTable[Index]->wBusDevFuncNum) {
            break;
        }
#endif
    }

    if (Index != gUsbData->HcTableCount) {
        HcStruc->dHCFlag &= ~HC_STATE_RUNNING;
        return USB_ERROR;
    }
    
	HcStruc->dHCFlag |= HC_STATE_INITIALIZED;
	
#if !USB_RT_DXE_DRIVER
    UpdateAmiUsbSmmGlobalDataCrc32(gUsbData);
#endif

#ifdef USB_CONTROLLERS_INITIAL_DELAY_LIST
    for (Index = 0; Index < COUNTOF(gControllersInitialDelayList); Index++) {
        if ((gControllersInitialDelayList[Index].Vid == HcStruc->Vid) &&
            (gControllersInitialDelayList[Index].Did == HcStruc->Did)) {
        	if (gControllersInitialDelayList[Index].DelayTime) {
        		FixedDelay(gControllersInitialDelayList[Index].DelayTime * 1000);
        	}
            break;
        }
    }
#endif

	USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Enumerating HC#%d, type 0x%x\n", HcStruc->bHCNumber, HcStruc->bHCType);
	// Issue enumerate call for this HC
	(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDEnumeratePorts)(HcStruc);

    if (USB_RUNTIME_DRIVER_IN_SMM == 2) {
	    (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDSmiControl)(HcStruc, FALSE);
    }

	return USB_SUCCESS;
}

/**
    This function stops the USB host controller.

    @param HcStruc		HC struc pointer

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
UsbHcStop(HC_STRUC* HcStruc)
{
	if ((HcStruc->dHCFlag & HC_STATE_RUNNING) == 0) {
		return USB_ERROR;
	}

	USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Stopping HC %X, HCNum %d, type %x\n", 
		 		HcStruc->wBusDevFuncNum, HcStruc->bHCNumber, HcStruc->bHCType);
	(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStop)(HcStruc);

	return USB_SUCCESS;
}

/**
    This function checks bios owned hc. Clear USB_FLAG_DRIVER_STARTED
    if we don't start any host controller.

    @param VOID

    @retval VOID

**/

VOID
CheckBiosOwnedHc(
    VOID
)
{
    UINT8       Index;
    HC_STRUC    *HcStruc;

    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
            continue;
        }
        if (HcStruc->dHCFlag & HC_STATE_RUNNING) {
            return;
        }
    }
    
    gUsbData->dUSBStateFlag &= ~(USB_FLAG_DRIVER_STARTED);

    return;
}


/**
    This function enumerates the root hub ports of the all
    selected type HCs


    @param bType   - HC type

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_EnumerateRootHubPorts(
    UINT8 Type
)
{
    UINT8		Index;
    HC_STRUC*   HcStruc;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "Enumerating HC Ports.\n");
    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        //
        // Get the HCStruc pointer associated with this controller
        //
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
              continue;
        }

        if((HcStruc->bHCType) == Type && (HcStruc->dHCFlag & HC_STATE_RUNNING)) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Enumerating HC#%d, type 0x%x\n", HcStruc->bHCNumber, Type);
            //
            // Issue enumerate call for this HC
            //
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(Type)].pfnHCDEnumeratePorts)(HcStruc);
        }
    }
    return USB_SUCCESS;
}


/**
    This function initializes the specified type of the HC
    from the provided list of host controller PCI addresses

    @param pHCPCIAddrList  Pointer to table of HC PCI addresses in the system
        bHCType         Type of HC to be initialized (EHCI, OHCI etc)

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USBInitHostController(
    UINT16  *HcPciAddrList,
    UINT8   HcType
)
{
    return USB_SUCCESS;
}


/**
    This function returns the hub port status

    @param HcStruc   HC struc pointer
        HubAddr   USB device address of the hub or HC number
        BIT7 = 1/0  Roothub/Hub
        PortNum   Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT32
USB_GetHubPortStatus (
    HC_STRUC*	HcStruc,
    UINT8       HubAddr,
    UINT8		PortNum,
    BOOLEAN     ClearChangeBits
)
{
    //
    // Check whether the request is for root hub or generic hub
    //
    if (HubAddr & BIT7) {
		// Root hub
        return (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDGetRootHubStatus)(
                                HcStruc, PortNum, ClearChangeBits);
    } else {
    #if USB_DEV_HUB
        return USBHub_GetPortStatus(HcStruc, HubAddr, PortNum, ClearChangeBits);
    #else
        return 0;
    #endif
    }
}

/**
    This function disables the hub port

    @param fpHCStruc   HC struc pointer
        bHubAddr    USB device address of the hub or HC number
        BIT7 = 1/0  Roothub/Hub
        bPortNum    Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_DisableHubPort(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum
)
{
    //
    // Check whether the request is for root hub or generic hub
    //
    if (HubAddr & BIT7) {
        //
        // Issue the disable root hub call to disable the hub port
        //
        (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDisableRootHub)(
                        HcStruc,
                        PortNum);
#if USB_DEV_HUB
    }
    else {
        USBHub_DisablePort(HcStruc, HubAddr, PortNum);
#endif
    }
    return USB_SUCCESS;
}


/**
    This function enables the hub port

    @param fpHCStruc   HC struc pointer
        bHubAddr    USB device address of the hub or HC number
        BIT7 = 1/0  Roothub/Hub
        bPortNum    Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_EnableHubPort (
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum
)
{
    //
    // Check whether the request is for root hub or generic hub
    //
    if (HubAddr & BIT7) {
        //
        // Root hub
        // Issue the disable root hub call to disable the hub port
        //
        return (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDEnableRootHub)(HcStruc, PortNum);
    } else {
    #if USB_DEV_HUB
        return USBHub_EnablePort(HcStruc, HubAddr, PortNum);
    #else
        return USB_ERROR;   // Only root hub could be successfully enabled
    #endif
    }
}

/**
    This function resets the hub port

    @param HcStruc   HC struc pointer
        HubAddr   USB device address of the hub or HC number
        BIT7 = 1/0  Roothub/Hub
        PortNum   Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_ResetHubPort (
    HC_STRUC*	HcStruc,
    UINT8       HubAddr,
    UINT8		PortNum)
{
    //
    // Check whether the request is for root hub or generic hub
    //
    if (HubAddr & BIT7) {
        //
        // Root hub
        // Issue the reset root hub call to reset the hub port
        //
        return (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDResetRootHub)(HcStruc, PortNum);
    } else {
    #if USB_DEV_HUB
        return USBHub_ResetPort (HcStruc, HubAddr, PortNum);
    #else
        return USB_ERROR;   // Only root hub could be successfully reset
    #endif
    }
}

/**
    This function checks the port status provided and depending
    on the status it invokes device connect/disconnect routine

    @param fpHCStruc   Pointer to HCStruc
        bHubAddr    For root port this is the host controller index
        in gUsbData->aHCStrucTable combined with BIT7;
        For devices connected to a hub this is parent
        hub USB address
        bHubPort    Parent hub port number
        bPortStatus Port status read

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

DEV_INFO*
USB_DetectNewDevice(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       HubPort,
    UINT32      PortStatus
)
{
    UINT8           ErrorFlag  = 0;
    UINT16          DescLength;
    UINT8           DevConfigured;
    UINTN           ConfigLevel = 0;
    UINT16          TotalLength;
    UINT8           *Buffer = NULL;
    DEV_INFO        *DevInfo;
    DEV_INFO        *Pointer;
    DEV_DESC        *DevDesc;
    CNFG_DESC       *CnfgDesc;
    INTRF_DESC      *IntrfDesc;
    UINT8           *DevMiscInfo;
	UINT8			Status;
    BOOLEAN         SkipConnectBeep = FALSE;   //(EIP64781+)
    UINT16			OrgTimeOutValue;			//(EIP75441+)
    UINT8           DeviceAddress;
    DLINK                    *Link = NULL;
    DLINK                    *NextLink = NULL;
    USB_DEV_CONFIG_LINK      *DevConfigLink = NULL;
    //
    // Get the temporary device info structure pointer (index 0)
    //
    DevInfo = gUsbData->aDevInfoTable;

    //
    // Fill the necessary entries in the device info
    //
    DevInfo->Flag             = DEV_INFO_VALID_STRUC;
    DevInfo->bDeviceAddress   = 0;
    //fpDevInfo->wEndp0MaxPacket  = 0x40;   //(EIP98230-)
    DevInfo->bDeviceType      = 0;
    DevInfo->wIncompatFlags   = 0;
	DevInfo->DevMiscInfo		= NULL;

    //
    // Fill the hub/host controller information
    //
    DevInfo->bHubDeviceNumber = HubAddr;
    DevInfo->bHubPortNumber   = HubPort;

    //
    // Fill the device speed
    //
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_DetectNewDevice: wPS = %x\n", PortStatus);
    DevInfo->bEndpointSpeed = (PortStatus & USB_PORT_STAT_DEV_SPEED_MASK) >>
                        USB_PORT_STAT_DEV_SPEED_MASK_SHIFT;

                                        //(EIP98145+)>
#if SHOW_SKIP_PORT_INFORMATION
{
    UINT8   i;
    DEV_INFO   						*TmpDevInfo; 
    TmpDevInfo = DevInfo; 	
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "==== SHOW_SKIP_PORT_INFORMATION ==== \n"); 				  
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "BDF %x \nRoutePath = ",HcStruc->wBusDevFuncNum);  
    for(i = 0;i < 5;i++) {
        if(TmpDevInfo->bHubDeviceNumber & BIT7) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\nRootPort %x \n",TmpDevInfo->bHubPortNumber); 					  
            break;
        }
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "%x ",TmpDevInfo->bHubPortNumber); 				  
        TmpDevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, 0, TmpDevInfo->bHubDeviceNumber, 0);     
        if (TmpDevInfo == NULL) {
            break;
        }     
    }
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "==== SHOW_SKIP_PORT_INFORMATION ==== \n"); 				  
}
#endif
                                        //<(EIP98145+)
                                        //(EIP98230+)>    
    switch (DevInfo->bEndpointSpeed) {
        case USB_DEV_SPEED_SUPER_PLUS:
        case USB_DEV_SPEED_SUPER:
            DevInfo->wEndp0MaxPacket = 0x200;
            break;
        case USB_DEV_SPEED_HIGH:
            DevInfo->wEndp0MaxPacket = 0x40;
            break;
        case USB_DEV_SPEED_FULL:
        case USB_DEV_SPEED_LOW:
            DevInfo->wEndp0MaxPacket = 0x08;
            break;
    }
                                        //<(EIP98230+)
    
    //
    // Fill the HC struc index value
    //
    DevInfo->bHCNumber = HcStruc->bHCNumber;

    ErrorFlag = TRUE;      // Assume as error
    DevConfigured = FALSE;     // No device configured

    //
    // Allocate memory for device requests
    //
    ConfigLevel = USB_ERR_DEV_INIT_MEM_ALLOC;   // For proper error handling
    Buffer = USB_MemAlloc (GET_MEM_BLK_COUNT(MAX_CONTROL_DATA_SIZE));
    if (Buffer == NULL) {
		SpeakerBeep(8, 0x2000, HcStruc);
		return (DEV_INFO*)ConfigLevel;
    }
    ConfigLevel = USB_ERR_DEV_INIT_GET_DESC_8;

    // Initialize HC specific data before device configuration
    Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDInitDeviceData)(
                        HcStruc, DevInfo, PortStatus, &DevMiscInfo);
	if(Status != USB_SUCCESS) {
		USB_MemFree(Buffer, (UINT8)(MAX_CONTROL_DATA_SIZE / sizeof(MEM_BLK)));
		SpeakerBeep(8, 0x2000, HcStruc);
		return (DEV_INFO*)ConfigLevel;
	}
    DevInfo->DevMiscInfo = (VOID*)DevMiscInfo;

//
// Next send a GetDescriptor command to the device to get its Device
// Descriptor. Assume a MaxPacket size of 64 bytes (the device will use 8,
// 16, 32, or 64). Regardless of the packet size used by te device we can
// always get the real MaxPacket size that the device is using, because
// this piece of information is at offset 7 in the device descriptor.
//
	OrgTimeOutValue = gUsbData->wTimeOutValue;
	gUsbData->wTimeOutValue = 1000;

    DevDesc = (DEV_DESC*)USB_GetDescriptor(
                            HcStruc,
                            DevInfo,
                            Buffer,
                            8,
                            DESC_TYPE_DEVICE,
                            0);

	gUsbData->wTimeOutValue = OrgTimeOutValue;
    if (DevDesc == NULL) {
        goto detection_complete;
    }

    //
    // Get and store the endpoint 0 max packet size
    //
    ConfigLevel = USB_ERR_DEV_INIT_SET_ADDR;
    //
    // Endpoint 0 max packet size check.
    // CyQ've USB modem(Model:MQ4UFM560) return invalid device descriptor after 
    // warm reset.
    //
                                        //(EIP81612)>
    if (DevDesc->BcdUsb >= 0x0300) {
        DevInfo->wEndp0MaxPacket = (UINT16)1 << DevDesc->MaxPacketSize0;
    } else {
        DevInfo->wEndp0MaxPacket = (DevDesc->MaxPacketSize0)?
                (UINT16)DevDesc->MaxPacketSize0 : 0x40;
    }
                                        //<(EIP81612)
                                        //(EIP73803)>
    if((DevInfo->bEndpointSpeed == USB_DEV_SPEED_LOW) ||
        (DevInfo->bEndpointSpeed == USB_DEV_SPEED_FULL) ||
        (DevInfo->bEndpointSpeed == USB_DEV_SPEED_HIGH)){
        FixedDelay(10 * 1000);     // 10msec delay
    }
                                        //<(EIP73803)

    //To assign an address to a USB device, the USB device transitions the state 
    //from the Default to the Address state.
    for (DeviceAddress = 1; DeviceAddress < 64; DeviceAddress++) {
        if (gUsbData->DeviceAddressMap & LShiftU64(1, DeviceAddress)) {
            break;
        }
    }

    if (DeviceAddress == 64) {
        goto detection_complete;
    }
    Status = USB_SetAddress(HcStruc, DevInfo, DeviceAddress);
    if (Status == USB_ERROR) {
		goto detection_complete;
    }
    gUsbData->DeviceAddressMap &= ~(LShiftU64(1, DeviceAddress));
    DevInfo->bDeviceAddress = DeviceAddress;
    FixedDelay(2 * 1000);
    
    //
    // Now send a GetDescriptor command to the device to get its device descriptor.
    //
    DevDesc = (DEV_DESC*)USB_GetDescriptor(
                            HcStruc,
                            DevInfo,
                            Buffer,
                            18,
                            DESC_TYPE_DEVICE,
                            0);

    //ASSERT(fpDevDesc != NULL);
    if (DevDesc == NULL) {
		goto detection_complete;
    }
    // If a descriptor returns with a value in its length field that is 
    // less than defined by USB specification, the descriptor is invalid.
    if (DevDesc->DescLength < 18) {
        goto detection_complete;
    }
    if (DevDesc->NumConfigs == 0) {
        DevDesc->NumConfigs = 1;
    }

    CopyMem((UINT8*)&DevInfo->DevDesc, DevDesc, sizeof(DEV_DESC));

    ConfigLevel = USB_ERR_DEV_INIT_GET_DESC_200;
    //
    // Get the relevant information from the descriptor and store it in
    // device information struture
    //
    DevInfo->wVendorId    = DevDesc->VendorId;
    DevInfo->wDeviceId    = DevDesc->DeviceId;
//
// Look at each of the device's ConfigDescriptors and InterfaceDescriptors
// until an InterfaceDescriptor is found with BaseClass, SubClass, and
// Protocol fields indicating boot keyboard, mouse, hub or storage support.
//
    DevInfo->bConfigNum   = 0;

    for (Link = gUsbData->DevConfigInfoList.pHead; Link != NULL; Link = NextLink) {
        NextLink = Link->pNext;
        DevConfigLink = OUTTER(Link, Link, USB_DEV_CONFIG_LINK);
    
        if (DevConfigLink->DevConfigInfo.Vid == DevInfo->wVendorId) {
            if (DevConfigLink->DevConfigInfo.Did == DevInfo->wDeviceId) {
                DevInfo->bConfigNum = DevConfigLink->DevConfigInfo.Config - 1;  // zero base
                break;
            }
        }
    }

    if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_FULL) {
        FixedDelay(100);
    }

    do {    // For processing multiple configurations
								//(EIP70933+)>
        CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(
                        HcStruc,
                        DevInfo,
                        Buffer,
                        0xFF,
                        DESC_TYPE_CONFIG,
                        DevInfo->bConfigNum);
		if (CnfgDesc == NULL) {
			break;
		}
		TotalLength = CnfgDesc->wTotalLength;
        if (TotalLength > 0xFF) {
            if(TotalLength > (MAX_CONTROL_DATA_SIZE - 1)) {
                TotalLength = MAX_CONTROL_DATA_SIZE - 1;
            }
            CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(
                        HcStruc,
                        DevInfo,
                        Buffer,
                        TotalLength,
                        DESC_TYPE_CONFIG,
                        DevInfo->bConfigNum);
								//<(EIP70933+)
            if (CnfgDesc == NULL) {
                break;
            }
        }
  
        if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_FULL) {
            FixedDelay(100);
        }
//
// fpCnfgDesc should now point to a ConfigDescriptor.  Verify this and
// then get some fields out of it.  Then point to the next descriptor.
//
        if (CnfgDesc->bDescType == DESC_TYPE_CONFIG) {
            // Some devices might return the wrong length of the configuration 
            // descriptor, we force the length filed to 0x09. 
            if (CnfgDesc->bDescLength == 0) {
                USB_DEBUG(DEBUG_WARN, 3, "The length of the configuration descriptor is 0.\n");
                CnfgDesc->bDescLength = 0x09;
            }
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDEnableEndpoints)(
                        HcStruc, DevInfo, (UINT8*)CnfgDesc);

            //wTotalLength = fpCnfgDesc->wTotalLength; //(EIP70933-)
            DescLength = (UINT8)CnfgDesc->bDescLength;
            DevInfo->bConfigNum = CnfgDesc->bConfigValue;

										//(EIP70933-)>
/*
            if(wTotalLength > (MAX_CONTROL_DATA_SIZE - 1)) {
                wTotalLength = MAX_CONTROL_DATA_SIZE - 1;
            }
*/
										//<(EIP70933-)

            // Check if the device has alternate setting for the interface.
            for (;DescLength < TotalLength;) {
                //
                // fpIntrfDesc should now point to an InterfaceDescriptor.  Verify this
                // and then check its BaseClass, SubClass, and Protocol fields for
                // usable devices.
                //
                IntrfDesc = (INTRF_DESC*)((UINT8*)CnfgDesc + DescLength);
										//(EIP59601+)>
				if ((IntrfDesc->bDescLength == 0) || 
					((IntrfDesc->bDescLength + DescLength) > TotalLength)) {
					break;
				}
                if ((IntrfDesc->bDescType == DESC_TYPE_INTERFACE) && (IntrfDesc->bAltSettingNum != 0)) {
                    DevInfo->Flag |= DEV_INFO_ALT_SETTING_IF;
                    break;
                }
                if (IntrfDesc->bDescLength) {
                    DescLength += (UINT16)IntrfDesc->bDescLength;
                } else {
                    break;
                }
            }

            DescLength = (UINT8)CnfgDesc->bDescLength;

            for (;DescLength < TotalLength;) {
                //
                // fpIntrfDesc should now point to an InterfaceDescriptor.  Verify this
                // and then check its BaseClass, SubClass, and Protocol fields for
                // usable devices.
                //
                IntrfDesc = (INTRF_DESC*)((UINT8*)CnfgDesc + DescLength);
										//(EIP59601+)>
				if ((IntrfDesc->bDescLength == 0) || 
					((IntrfDesc->bDescLength + DescLength) > TotalLength)) {
					break;
				}
										//<(EIP59601+)
                if ((IntrfDesc->bDescType == DESC_TYPE_INTERFACE) && (IntrfDesc->bAltSettingNum == 0)) {
                    DevInfo->bInterfaceNum    = IntrfDesc->bInterfaceNum;
                    DevInfo->bAltSettingNum   = 0;
                    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_6, "USBIdentifyAndConfigureDevice::fpIntrfDesc %lx\n",fpIntrfDesc);
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBIdentifyAndConfigureDevice:: %04x/%04x Intrf %d, AltSetting %d\n",
                            DevInfo->wVendorId, DevInfo->wDeviceId, IntrfDesc->bInterfaceNum, IntrfDesc->bAltSettingNum);
                    USB_DEBUG(DEBUG_INFO, 3, "fpCnfgDesc %x, wDescLength 0x%x, wTotalLength 0x%x\n", CnfgDesc, DescLength, TotalLength);
                    Pointer = USBIdentifyAndConfigureDevice(
                                    HcStruc,
                                    DevInfo,
                                    (UINT8*)CnfgDesc,
                                    DescLength,
                                    TotalLength);
                    if (Pointer != NULL) {
                        DevInfo = Pointer;
                        DevConfigured = TRUE;  // At-least one device is configured
                                        //(EIP64781+)>
                        if (gUsbData->dUSBStateFlag & USB_FLAG_SKIP_CARD_READER_CONNECT_BEEP) {
                            if (DevInfo->bBaseClass == BASE_CLASS_MASS_STORAGE) {
                                SkipConnectBeep = TRUE;
                            }
                        }
                                        //<(EIP64781+)
                    }
															//(EIP22046+)>
					//
                    // There is one more config. Set device info structure entry 0 for it
                    //
                    if ((CnfgDesc->bNumInterfaces > 1) && DevConfigured) {
						gUsbData->aDevInfoTable[0].Flag   |= DEV_INFO_MULTI_IF;
                    }
															//<(EIP22046+)
                }
                if (IntrfDesc->bDescLength && 
                    !(IntrfDesc->bDescType == DESC_TYPE_INTERFACE &&
                            IntrfDesc->bBaseClass == BASE_CLASS_HUB)) {
                    DescLength += (UINT16)IntrfDesc->bDescLength;
                    if (DescLength < TotalLength) {
															//(EIP22046-)>
                        //
                        // There is one more config. Set device info structure entry 0 for it
                        //
                        /*
                        if (fpDevInfo->bInterfaceNum > 0) {
                            fpDevInfo->Flag |= DEV_INFO_MULTI_IF;
                        }
                        gUsbData->aDevInfoTable[0].Flag   |= DEV_INFO_MULTI_IF;
                        */
															//<(EIP22046-)
                        DevInfo = gUsbData->aDevInfoTable;
                    }
                } else {
                    break;   // fpIntrfDesc->bDescLength == 0
                }
            }   // while ()
        }   // if
        //
        // Check if we have at least one usable device
        //
        if (DevConfigured) {
            ErrorFlag = FALSE; // Device successfully configured
            ConfigLevel = (UINTN)gUsbData->aDevInfoTable;
            goto detection_complete;
        }
        else {
            DevInfo->bConfigNum++;
        }
    } while (DevInfo->bConfigNum < DevInfo->DevDesc.NumConfigs);  // while

detection_complete:
    //
    // At this point, if bErrorFlag is FALSE then we successfully configured
    // atleast a device.
    // If bErrorFlag is TRUE then there is error in configuring the device
    //
    if (ErrorFlag) {
        USBLogError((UINT16)ConfigLevel); // Log configuration level

        SpeakerBeep(8, 0x2000, HcStruc);

		(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeinitDeviceData)
 						(HcStruc, DevInfo);
        if(ConfigLevel != USB_ERR_DEV_INIT_GET_DESC_8) {
            //
            // Disable the hub port
            //
            USB_DisableHubPort(
                HcStruc,
                DevInfo->bHubDeviceNumber,
                DevInfo->bHubPortNumber);
            ConfigLevel = 0;
        }
        if (DevInfo->bDeviceAddress) {
            gUsbData->DeviceAddressMap |= LShiftU64(1, DevInfo->bDeviceAddress);
        }
    }
    else {
                                        //(EIP64781+)>
        if(!SkipConnectBeep) {
            SpeakerBeep(4, 0x1000, HcStruc);
        }
                                        //<(EIP64781+)
    }
    USB_MemFree(Buffer, (UINT8)(MAX_CONTROL_DATA_SIZE / sizeof(MEM_BLK)));

    return (DEV_INFO*)ConfigLevel;

}


/**
    This function stops the device:
    - calls its disconnect function if available
    - stops polling the device's interrupt endpoint
    - updates device address memory map


    @param  HcStruc   Pointer to HCStruc
            HcHubAddr   address
            HcPort     Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_StopDevice(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       HcPort
)
{
    UINT8           Count;
    DEV_DRIVER      *DevDriver;
    DEV_INFO        *DevInfo;
    UINT8           Status;

    Status = USB_ERROR;
    
    //
    // Find the device entry that would match the input.
    //
    for (Count = 1; Count < MAX_DEVICES; Count++) {
        DevInfo = &gUsbData->aDevInfoTable[Count];
        if ((DevInfo->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_DUMMY)) ==
            DEV_INFO_VALID_STRUC) {
            if ((DevInfo->bHubDeviceNumber == HubAddr) && 
                (DevInfo->bHubPortNumber == HcPort)) {
                //
                // Device found - issue disconnect call for the device
                //
                if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
                    if ((!(DevInfo->Flag & DEV_INFO_IN_QUEUE)) && 
                        (DevInfo->Flag & DEV_INFO_DEV_BUS)){
                        USB_SmiQueuePut(DevInfo);
                        DevInfo->Flag |= DEV_INFO_IN_QUEUE;
                    }
                }

                DevDriver = UsbFindDeviceDriverEntry(DevInfo->fpDeviceDriver);
                //
                // Check disconnect function is valid, if yes - execute it
                //
                if (DevDriver && DevDriver->pfnDisconnectDevice) {
                    DevDriver->pfnDisconnectDevice(DevInfo);
					DevInfo->fpDeviceDriver = NULL;
                } else {
                    //
                    // Stop polling the device's interrupt endpoint
                    //
                    if (DevInfo->IntInEndpoint) {
                        Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeactivatePolling)
                                    (HcStruc, DevInfo);
                        DevInfo->IntInEndpoint = 0;
                    }
                }

                // HC device removal call
                Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeinitDeviceData)
                                (HcStruc, DevInfo);

                // Reset the disconnecting flag
                DevInfo->Flag &= ~DEV_INFO_DEV_DISCONNECTING;

                //
                // Update Device Address Map, preserving the address for registered devices
                //
                if (DevInfo->bDeviceAddress != 0) {
                    gUsbData->DeviceAddressMap |= LShiftU64(1, DevInfo->bDeviceAddress);
                    DevInfo->bDeviceAddress = 0;
                }
                DevInfo->Flag &= ~DEV_INFO_DEV_PRESENT;
                if (!(DevInfo->Flag & (DEV_INFO_DEV_BUS | DEV_INFO_MASS_DEV_REGD))) {
                    // Reset the device info structure validity ~flag
                    DevInfo->Flag &= ~DEV_INFO_VALID_STRUC;
                }
                USB_DEBUG(DEBUG_INFO, 3, "Release Dev[%d]: %x, flag %x\n", Count, DevInfo, DevInfo->Flag);
            }
        }
    }
    return Status;
}

/**
    This function is called when a device disconnect is
    detected. This routine disables the hub port and stops the
    device and its children by calling another routine.

    @param fpHCStruc   Far pointer to HCStruc of the host controller
        bHubAddr    USB device address of the hub whose status
        has changed
        bit 7   : 1 - Root hub, 0 for other hubs
        bit 6-0 : Device address of the hub
        bPortNum    Port number

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
USB_DisconnectDevice(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       HCPort
)
{
    //
    // A device has been disconnected from the USB.  First disable the hub port
    // that the device was plugged into.  Then free up the device's entry in the
    // DeviceTable.  If there an error occurs while disabling the port, assume
    // that the device is still present an leave its DeviceTable entry in place.
    //
    USB_DisableHubPort(HcStruc, HubAddr, HCPort);

    USB_StopDevice(HcStruc, HubAddr, HCPort);

    return USB_SUCCESS;

}


/**
    This routine processes the port status change (like connect,
    disconnect, etc.) for the root hub and external hubs.

    @param HcStruc   Pointer to Host Controller structure
        HubAddr     Device address of the hub whose status
        has changed:
        bit 7	: 1 - Root hub, 0 for other hubs
        bit 6-0	: Device address of the hub
        PortNum     Hub port number

**/

UINT8
USBCheckPortChange (
    HC_STRUC    *HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum
)
{
    UINT32      PortStatus;
    DEV_INFO    *Dev;
    UINT8       Count;

    for (Count = 0; Count < USB_DEVICE_ENUMERATE_RETRY_COUNT; Count++) {
		PortStatus = USB_GetHubPortStatus(HcStruc, HubAddr, PortNum, TRUE);

        //
        // Check the obtained port status
        //
        if (PortStatus == USB_ERROR) {
			return USB_ERROR;
        }
        if (!(PortStatus & USB_PORT_STAT_DEV_OWNER)) {
            return USB_SUCCESS;
        }
        if (OEMSkipList(HubAddr,PortNum,HcStruc->wBusDevFuncNum,0,0)) {
            USB_DEBUG(DEBUG_INFO, 3, "Match the skip table ; skipping this device.\n");   //(EIP98145)
            return USB_SUCCESS;
        }
        if (!Count && !(PortStatus & USB_PORT_STAT_DEV_CONNECT_CHANGED)) {
			return USB_SUCCESS;
        }
    
        if (PortStatus & USB_PORT_STAT_DEV_CONNECTED) {
            if (gUsbData->bHandOverInProgress) {
                USB_DisableHubPort(HcStruc, HubAddr, PortNum);
                return USB_SUCCESS;
            }
			if ((Count != 0) || !(PortStatus & USB_PORT_STAT_DEV_ENABLED) || USB_RESET_PORT_POLICY) {
				// Reset and enable the port
				USB_ResetHubPort(HcStruc, HubAddr, PortNum);
				USB_EnableHubPort(HcStruc, HubAddr, PortNum);
				PortStatus = USB_GetHubPortStatus(HcStruc, HubAddr, PortNum, TRUE);

                if (PortStatus == USB_ERROR) {
			        return USB_ERROR;
                }
                if (!(PortStatus & USB_PORT_STAT_DEV_OWNER)) {
                    return USB_SUCCESS;
                }
				if (!(PortStatus & USB_PORT_STAT_DEV_CONNECTED)) {
					// Some device will be disconnected after 
					// port reset, and reconnected for a while.
					FixedDelay(100 * 1000);
					continue;
				}
				// Check whether port is enabled
				if (!(PortStatus & USB_PORT_STAT_DEV_ENABLED)) {
					FixedDelay(100 * 1000);	 // 100msec delay
					continue;
				}
			}
            Dev = USB_DetectNewDevice(HcStruc, HubAddr, PortNum, PortStatus);
            if ((UINTN)Dev == USB_ERR_DEV_INIT_GET_DESC_8) {
                // The device might be configured to the wrong speed and doesn't work.
                // Try to disable and reset the port, it might be reconfigured to the right speed.
                if (Count >= (USB_DEVICE_ENUMERATE_RETRY_COUNT / 2)) {
                    USB_DisableHubPort(HcStruc, HubAddr, PortNum);
                }
                FixedDelay(100 * 1000);     // 100msec delay
                continue;
            }
			if ((UINTN)Dev == 0) {
				return USB_ERROR;
			}
            if ((UINTN)Dev > USB_ERR_DEV_INIT_GET_DESC_200) {
                return USB_SUCCESS;
            }
            SpeakerBeep(16, 0x4000, HcStruc);  // Issue error beep
            return USB_ERROR;
        } else {  // Disconnect
            USB_DisconnectDevice(HcStruc, HubAddr, PortNum);
            SpeakerBeep(8, 0x1000, HcStruc);
			return USB_SUCCESS;
        }
    }
    if (Count == USB_DEVICE_ENUMERATE_RETRY_COUNT) {
        USB_DEBUG(DEBUG_ERROR, 3, "Usb Device enumeration is failed.\n");
        USB_DisableHubPort(HcStruc, HubAddr, PortNum);
        USB_GetHubPortStatus(HcStruc, HubAddr, PortNum, TRUE);
        return USB_ERROR;
    }

    return USB_SUCCESS;
}


/**
    This routine allocates blocks of memory from the global
    memory pool

    @param bNumBlocks  Number of 32 byte blocks needed

    @retval Start offset to the allocated block (NULL on error)

    @note  This routine allocates continuous 32 byte memory blocks.

**/

VOID _FAR_ *
USB_MemAlloc(
    UINT16  NumBlk
)
{
    UINT8       MemIsFound     = FALSE;
    UINT8       BitCount       = 0;
    UINT8       Start          = 0;
    UINT16      Num;
    UINT16      Count          = 0;    // Contiguous blocks counter

    UINT16      BlkOffset      = 0;
    UINT16      MapDwordPtr;
										//(EIP89641)>
	UINT16		PageCount = 0;
	UINT16		MapDwordCount = 0;
	UINT32		BlksStsDwordsPerPage = 0;

    UINT32      Mask;
    UINT32      Temp;

    if (NumBlk == 0) {
        return NULL;
    }
    
#if USB_FORCE_64BIT_ALIGNMENT
    if (NumBlk % 2) {
        NumBlk++;
    }
#endif
    Num = NumBlk;

	BlksStsDwordsPerPage = (gUsbData->MemBlkStsBytes >> 2) / gUsbData->MemPages;

    //
    // Locate wNumBlk contiguous blocks from each memory page
    //
    for(PageCount = 0; (PageCount < gUsbData->MemPages) && !MemIsFound; PageCount++) {

        // Do not reset the counter if the allocated blocks greater than a page.
        if (NumBlk <= (0x1000 / sizeof(MEM_BLK))) {
		    Count = 0;  // Reset contiguous blocks counter
        }

	    for (MapDwordCount = 0; MapDwordCount < BlksStsDwordsPerPage; MapDwordCount++) {
	        //
	        // Read the next DWORD memory map data
	        //
	        MapDwordPtr = (PageCount * BlksStsDwordsPerPage) + MapDwordCount;
        	Temp = gUsbData->aMemBlkSts[MapDwordPtr];

	        for (BitCount = 0; BitCount < 32; BitCount++)  {
	            BlkOffset++;
	            if (Temp & (UINT32)(1 << BitCount))  {
	                Count++;    // Found another free block
	                if(Count == Num) {
	                    BlkOffset = (UINT16)(BlkOffset-Count);
	                    MemIsFound = TRUE;
	                    break;  // Found the requested number of free blocks
	                }
	            }
	            else
	            {
	                Count = 0;  // Reset contiguous blocks counter
	            }
	        }
	        if (MemIsFound) break;
	    }
    }
										//<(EIP89641)
    if (!MemIsFound) {
        ASSERT(FALSE);
        return NULL;
    }

//
// Change the appropriate bits in the memory map to indicate that some memory
// is being allocated
//
// At this point,
//  bBitCount points to the end of the block within DWORD
//  wMapDwordPtr points to the status dword in question

// We have to reset bCount number of bits starting from
// wMapDwordPtr[bBitCount] to wStsX[BitPosY]
// where wStsX is the status double word of the starting block,
// BitPosY is the bit position of the starting block.
//
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "wMapDwordPtr = %d\n", MapDwordPtr);
//
// Let us have a do loop to do the trick
//
    do {
        //
        // Find out how many bits we can reset in current (pointed by wMapDwordPtr)
        // double word
        //
        Count = (UINT16)((BitCount >= (Num-1)) ? Num : BitCount+1);
        //
        // Find out the starting bit offset
        //
        Start = (UINT8)(BitCount + 1 - Count);
        //
        // Form the 32bit mask for the AND operation
        // First prepare the bits left on the left
        //
        // Note: FFFFFFFF << 32 treated differently by different compilers; it
        // results as 0 for 16 bit compiler and FFFFFFFF for 32 bit. That's why
        // we use caution while preparing the AND mask for the memory map update.
        //
        Mask = ((Count + Start) < 32) ? (0xFFFFFFFF << (Count + Start)) : 0;

        //
        // Second, prepare the bits on the right
        //
        if (Start) {
            Mask = Mask | ~(0xFFFFFFFF << Start);
        }

        //
        // Reset the specified number of bits
        //
        gUsbData->aMemBlkSts[MapDwordPtr] &= Mask;

        //
        // Update the bCount, StsWordCount & BitCount
        //
        BitCount = 31;     // End of previous double word where we have to start
        MapDwordPtr--;     // Previous double word
        Num = Num - Count;
    } while (Num);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "MemAlloc: %d block(s) at %x %x %x\n",
            NumBlk,
            gUsbData->fpMemBlockStart + BlkOffset * sizeof(MEM_BLK),
            gUsbData->aMemBlkSts[0],
            gUsbData->aMemBlkSts[1]);

    return  ((VOID _FAR_ *)
        (gUsbData->fpMemBlockStart + (UINT32)BlkOffset * sizeof(MEM_BLK)));
}


/**
    This routine frees the chunk of memory allocated using
    the USBMem_Alloc call

    @param fpPtr       Pointer to the memory block to be freed
        bNumBlocks  Number of 32 byte blocks to be freed

    @retval Start offset to the allocated block (NULL on error)

    @note  This routine frees continuous memory blocks starting
              from the fpPtr.

**/

UINT8
USB_MemFree (
    VOID _FAR_ * Ptr,
    UINT16    NumBlk
)
{
    UINT8   Offset;
    UINT8   Count;
    UINT16  BlkCount;
    UINT16  BlkOffset;
    UINT16  StsWord;

#if USB_FORCE_64BIT_ALIGNMENT
    if (NumBlk % 2) {
        NumBlk++;
    }
#endif
    BlkCount = NumBlk;
    BlkOffset = 0;

    //
    // Check for pointer validity
    //
    if (Ptr == NULL) return USB_ERROR;

    if ((Ptr < (VOID *)gUsbData->fpMemBlockStart) ||
            (Ptr > (VOID *)(gUsbData->fpMemBlockStart +
                            (MEM_BLK_COUNT+1)*sizeof(MEM_BLK)))) {
        return  USB_ERROR;
    }

    BlkOffset = (UINT16)((UINTN)Ptr - (UINTN)gUsbData->fpMemBlockStart) / sizeof (MEM_BLK);

    if (BlkOffset >= MEM_BLK_COUNT) {
        return USB_ERROR;
    }

    StsWord = (UINT16)(BlkOffset >> 5);   // Divide by 32
    Offset = (UINT8)(BlkOffset & 0x1F);   // Mod 32
    Count = 0;

    do {
        gUsbData->aMemBlkSts[StsWord] |= ((UINT32)1 << (Count + Offset));
        BlkCount--;
        Count++;

        if ((Count + Offset) && (!((Count + Offset) & 0x1F))) {
            StsWord ++;
            Count = Offset = 0;
        }
    } while (BlkCount);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "MemFree: %d block(s) at %x %x %x\n",
                NumBlk, Ptr,
                gUsbData->aMemBlkSts[0], gUsbData->aMemBlkSts[1]);
    //
    // Pointer is valid. Fill the memory with 0's
    //
    ZeroMem(Ptr, (UINT32)(NumBlk * sizeof (MEM_BLK)));

    return USB_SUCCESS;
}

/**
    This function adds a new callback function to the globall
    callback function list and returns the index of it.

    @param pfnCallBackFunction     Callback function address

    @retval Callback function index

**/

UINT8
USB_InstallCallBackFunction (
    CALLBACK_FUNC      CallBackFunction
)
{
    UINT8   Index;
    UINT8   MaxIndex;

#if !USB_RT_DXE_DRIVER
    Index = 0;
    MaxIndex = MAX_CALLBACK_FUNCTION;
#else
    Index = MAX_CALLBACK_FUNCTION;
    MaxIndex = MAX_CALLBACK_FUNCTION * 2;
#endif

    //
    // Check whether this function is already installed or none found
    //
    for (; Index < MaxIndex; Index++) {
        //
        // Check for null entry
        //
        if (gUsbData->aCallBackFunctionTable[Index] == 0) {
            break;  // No entry found
        }

        if (gUsbData->aCallBackFunctionTable[Index] == CallBackFunction) {
#if USB_RT_DXE_DRIVER
            Index -= MAX_CALLBACK_FUNCTION;
#endif
            return (UINT8)(Index+1);
        }
    }

    ASSERT(Index != MaxIndex);
    if (Index == MaxIndex) {
        EFI_DEADLOOP(); // Exceeding max # of callback function is illegal
    } else {
        //
        // Store the call back function
        //
        gUsbData->aCallBackFunctionTable[Index] = CallBackFunction;
    }

#if USB_RT_DXE_DRIVER
    Index -= MAX_CALLBACK_FUNCTION;
#endif

    return (UINT8)(Index + 1);
}


DEV_DRIVER*
UsbFindDeviceDriverEntry(
    DEV_DRIVER* DevDriver
)
{
    UINTN   Index;

    if (DevDriver == NULL) {
        return DevDriver;
    }

    for (Index = 0; Index < MAX_DEVICE_TYPES * 2; Index++) {
        if (DevDriver == &gUsbData->aDevDriverTable[Index]) {
#if !USB_RT_DXE_DRIVER
            if (Index >= MAX_DEVICE_TYPES) {
                Index -= MAX_DEVICE_TYPES;
            }
#else
            if (Index < MAX_DEVICE_TYPES) {
                Index += MAX_DEVICE_TYPES;
            }
#endif
            return &gUsbData->aDevDriverTable[Index];
        }
        if (DevDriver == &gUsbData->aDelayedDrivers[Index]) {
#if !USB_RT_DXE_DRIVER
            if (Index >= MAX_DEVICE_TYPES) {
                Index -= MAX_DEVICE_TYPES;
            }
#else
            if (Index < MAX_DEVICE_TYPES) {
                Index += MAX_DEVICE_TYPES;
            }
#endif
            return &gUsbData->aDelayedDrivers[Index];
        }
    }

    return NULL;
    
}

/**
    This function executes a get descriptor command to the
    given USB device and endpoint

    @param fpHCStruc   HCStruc pointer
        fpDevInfo   Device info pointer
        fpBuffer    Buffer to be used for the transfer
        wLength     Size of the requested descriptor
        bDescType   Requested descriptor type
        bDescIndex  Descriptor index

    @retval Pointer to memory buffer containing the descriptor
        NULL on error

**/

UINT8*
USB_GetDescriptor(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*  Buffer,
    UINT16  Length,
    UINT8   DescType,
    UINT8   DescIndex
)
{
    UINT8           GetDescIteration;
    UINT16          Reg;
    UINT16          Status;
    EFI_STATUS      EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(EfiStatus)) {
        return NULL;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return NULL;
    }

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        EfiStatus = AmiValidateMemoryBuffer((VOID*)Buffer, Length);
        if (EFI_ERROR(EfiStatus)) {
            return NULL;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    for (GetDescIteration = 0; GetDescIteration < 5; GetDescIteration++) {
        Reg = (UINT16)((DescType << 8) + DescIndex);
        Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDControlTransfer)(
                        HcStruc,
                        DevInfo,
                        (UINT16)USB_RQ_GET_DESCRIPTOR,
                        (UINT16)0,
                        Reg,
                        Buffer,
                        Length);
        if (Status) {
            return Buffer;
        }
        if (gUsbData->dLastCommandStatusExtended & USB_TRNSFR_TIMEOUT) {
            break;
        }
        FixedDelay(10 * 1000);
    }

    return NULL;
}


/**
    This function sets the USB device address of device 0 to
    the given value. After this call the USB device will respond
    at its new address.

    @param fpHCStruc   Pointer to HCStruc structure
        fpDevInfo   Pointer to device info structure
        bNewDevAddr New device address to set

    @retval USB_SUCCESS or USB_ERROR

    @note  Skip SET_ADDRESS request for XHCI controllers

**/

UINT8
USB_SetAddress(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8       NewDevAddr
)
{
    UINT8           SetAddressIteration;

    for (SetAddressIteration = 0; SetAddressIteration < 5; SetAddressIteration++) {
        (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDControlTransfer)(
                            HcStruc,
                            DevInfo,
                            (UINT16)USB_RQ_SET_ADDRESS,
                            0,
                            (UINT16)NewDevAddr,
                            0,
                            0);
        if (!(gUsbData->bLastCommandStatus & USB_CONTROL_STALLED )) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USB_SetAddress#%d\n",NewDevAddr);
            return USB_SUCCESS;
        }
    }
    return USB_ERROR;

}

/**
    This function sets the device configuration.

    @param HcStruc   Pointer to HCStruc structure
        DevInfo   Pointer to device info structure
        ConfigNum Configuration Value

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
USB_SetConfig(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8       ConfigNum)
{
     (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDControlTransfer)(
                    HcStruc,
                    DevInfo,
                    USB_RQ_SET_CONFIGURATION,
                    0,
                    (UINT16)ConfigNum,
                    0,
                    0);

    return USB_SUCCESS;
}

/**
    This function sets the device interface.

    @param HcStruc   Pointer to HCStruc structure
        DevInfo   Pointer to device info structure
        InterfaceNum Interface Value

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
UsbSetInterface(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8       InterfaceNum
)
{
    USB_DEBUG(DEBUG_INFO, 3, "UsbSetInterface %d\n", InterfaceNum);
    (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDControlTransfer)(
                    HcStruc,
                    DevInfo,
                    USB_RQ_SET_INTERFACE,
                    0,
                    (UINT16)InterfaceNum,
                    0,
                    0);

    return USB_SUCCESS;
}

/**
    This routine logs the USB error in the data area. This
    logged errors will be displayed during the POST.

    @param wErrorCode  Error code to log

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
USBLogError(
    UINT16  ErrorCode
)
{
    //
    // First check for end of the buffer
    //
    if(gUsbData->bErrorLogIndex < MAX_USB_ERRORS_NUM) {
        //
        // Still have space to log errors
        //
        gUsbData->aErrorLogBuffer[gUsbData->bErrorLogIndex] = ErrorCode;
        gUsbData->bErrorLogIndex++;
    }
    return USB_ERROR;
}


/**
    This function is used to retrieve the device info structure
    for the particular device address & HCStruc

    @param bSearchFlag  Flag indicating search type
        = USB_SRCH_DEV_ADDR to search by device address and
        HCStruc pointer
        = USB_SRCH_DEV_TYPE to search by device type
        = USB_SRCH_HC_STRUC to search by HC struc pointer
        = USB_SRCH_DEV_NUM to count the number of devices connected:
        if fpHCStruc is not NULL - count only devices connected to
        certain controller, otherwise - all devices of requested
        type.
        = USB_SERCH_DEV_INDX to search by device location in the DEV_INFO:
        a) if fpDevInfo <> 0 return index or the fpDevInfo
        b) if bDevAddr <> 0 return the corresponding fpDevInfo
        c) if both bDevAddr <> 0 and fpDevInfo <> 0, consider a)

        fpDevInfoPtr    Pointer to the device info structure from where the
        search begins (if 0 start from first entry)
        bDev    Device address/drive number/device type
        pHCStruc    Pointer to the HCStruc structure

    @retval Depending on bSearchFlag this function returns:
        - pointer to DEV_INFO structure
        - table index
        - number of devices
        Function returns NULL if device is not found.

**/

DEV_INFO*
USB_GetDeviceInfoStruc(
    UINT8       SearchFlag,
    DEV_INFO*   DevInfo,
    UINT8       Dev,
    HC_STRUC*   HcStruc
)
{
    UINT8       Index;
    BOOLEAN     TerminateSearch = FALSE;
    UINT32      DeviceCounter  = 0;

    if (SearchFlag == USB_SRCH_DEV_INDX) {
        if (DevInfo) {
            for (Index=1; Index < MAX_DEVICES; Index++) {
                if (&gUsbData->aDevInfoTable[Index] == DevInfo) {
                    return (DEV_INFO*)(UINTN)Index;
                }
            }
            return NULL;    // Device address not found in the table
        }
        if (Dev == USB_HOTPLUG_FDD_ADDRESS) return &gUsbData->FddHotplugDev;
        if (Dev == USB_HOTPLUG_HDD_ADDRESS) return &gUsbData->HddHotplugDev;
        if (Dev == USB_HOTPLUG_CDROM_ADDRESS) return &gUsbData->CdromHotplugDev;

        if (Dev) return &gUsbData->aDevInfoTable[Dev];
        return NULL;        // Invalid input - both bDev and fpDevInfo are zeroes.
    }

    for (Index = 1; Index < MAX_DEVICES; Index ++) {
        //
        // if fpDev_Info is not null then position the search at the correct
        // index that matches the fpDev_Info
        //
        if (DevInfo) {
            if (&gUsbData->aDevInfoTable[Index] != DevInfo)
                continue;
            else {
                DevInfo = 0;
                continue;
            }
        }
        //
        // For USB_SRCH_DEVBASECLASS_NUM devices are counted regardless of their
        // DEV_INFO_VALID_STRUC flag
        //
        if (SearchFlag == USB_SRCH_DEVBASECLASS_NUM)
        {
            if(gUsbData->aDevInfoTable[Index].bBaseClass == Dev) {
                if (HcStruc) {
                    //
                    // Check if device belongs to the specified HC
                    //
                    if (gUsbData->aDevInfoTable[Index].bHCNumber != HcStruc->bHCNumber)
                    {
                        continue;
                    }
                }
                if (gUsbData->aDevInfoTable[Index].Flag & DEV_INFO_DEV_PRESENT)
                {
                    DeviceCounter++;
                }
            }
            continue;
        }

        if ((gUsbData->aDevInfoTable[Index].Flag & DEV_INFO_VALIDPRESENT) ==
            DEV_INFO_VALIDPRESENT){
            switch(SearchFlag) {
                case  USB_SRCH_HC_STRUC:
                    if (HcStruc == NULL) return NULL;
                    if (gUsbData->HcTable
                        [gUsbData->aDevInfoTable[Index].bHCNumber-1] == HcStruc) {
                        TerminateSearch = TRUE;
                    }
                    break;

                case  USB_SRCH_DEV_TYPE:
                    if (gUsbData->aDevInfoTable[Index].bDeviceType == Dev) {
                        TerminateSearch = TRUE;
                    }
                    break;
                case  USB_SRCH_DEV_NUM:
                    if (gUsbData->aDevInfoTable[Index].bDeviceType == Dev) {
                        if (HcStruc) {
                            //
                            // Check if device belongs to the specified HC
                            //
                            if (gUsbData->aDevInfoTable[Index].bHCNumber != HcStruc->bHCNumber)
                            {
                                break;
                            }
                        }
                        DeviceCounter++;
                    }
                    break;  // Do not change TerminateSearch so loop continues
                case  USB_SRCH_DEV_ADDR:
                    if (gUsbData->aDevInfoTable[Index].bDeviceAddress == Dev) {
                        if ((HcStruc == NULL) ||
                            (gUsbData->HcTable
                                [gUsbData->aDevInfoTable[Index].bHCNumber-1] == HcStruc)) {
                            TerminateSearch = TRUE;
                        }
                    }
                    break;

                default:
                    return NULL;
            }
        }
        if (TerminateSearch) return ((DEV_INFO*)&gUsbData->aDevInfoTable[Index]);
    }
    if ( (SearchFlag == USB_SRCH_DEV_NUM) || (SearchFlag == USB_SRCH_DEVBASECLASS_NUM) )
         return (DEV_INFO*)(UINTN)DeviceCounter;

    return NULL;
}


/**
    Finds a non-used DEV_INFO record in aDevInfoTable and marks it
    reserved. To free the user need to clear DEV_INFO_VALID_STRUC
    bit in bFlag of DEV_INFO

    @retval Pointer to new device info. struc. 0 on error

**/
DEV_INFO* UsbAllocDevInfo()
{
    UINT8       Index;
    DEV_INFO    *NewDevInfo;

    for (Index = 1; Index < MAX_DEVICES; Index ++){
        NewDevInfo = gUsbData->aDevInfoTable + Index;
        if ((NewDevInfo->Flag &
        ( DEV_INFO_VALID_STRUC | DEV_INFO_DEV_BUS)) == 0 ){
            //
            // Free device info structure. Save it if not.
            //
            NewDevInfo->Flag |= DEV_INFO_VALID_STRUC |  DEV_INFO_DEV_PRESENT;
            return  NewDevInfo;
        }
    }
    return NULL;
}


/**
    This routine searches for a device info structure that
    matches the vendor and device id, and LUN of the device
    found. If such a device info structure not found, then it
    will return a free device info structure

    @param Vendor, Device ID, Current LUN

    @retval Pointer to new device info. struc. NULL on error

**/

DEV_INFO*
USBGetProperDeviceInfoStructure(
    DEV_INFO*	Dev,
    UINT8   	Lun
)
{
    UINT8       Count;
    DEV_INFO    *DevInfo;
    DEV_INFO    *FreeDevInfo;

    FreeDevInfo = NULL;

//
// Scan through the device info table for a free entry. Also if the device
// connected is a mass storage device look for a device info structure whose
// device is disconnected and its vendor & device id matches the one of
// current device. If such a structure found that means this device may be
// reconnected - use the same structure
//
    for (Count = 1; Count < MAX_DEVICES; Count++)
    {
        DevInfo = (DEV_INFO*)&gUsbData->aDevInfoTable[Count];

        if (DevInfo->Flag & DEV_INFO_DEV_DUMMY) {
            continue;
        }

        // Check whether the structure is valid
        if (!(DevInfo->Flag & DEV_INFO_VALID_STRUC)) {    
            if (FreeDevInfo == NULL) {
                FreeDevInfo = DevInfo;    // Store the value of the free device info
            }
        } else {
            //
            // Yes, structure is valid. Check for device presence
            //
			if (DevInfo->Flag & DEV_INFO_DEV_PRESENT) {
				if ((DevInfo->bHubDeviceNumber != Dev->bHubDeviceNumber) ||
					(DevInfo->bHubPortNumber != Dev->bHubPortNumber)) {
					continue;
				}
			}
            //
            // Device is not present. Match the vendor, device id  and LUN with
            // current device info
            //
            if ((DevInfo->wVendorId == Dev->wVendorId) &&
                (DevInfo->wDeviceId == Dev->wDeviceId) &&
                (DevInfo->bInterfaceNum == Dev->bInterfaceNum) &&
                (DevInfo->bEndpointSpeed == Dev->bEndpointSpeed) &&
                (DevInfo->bLUN == Lun)) {
                return DevInfo;   // "Abandoned" device entry found
            }
        }
    }
    return FreeDevInfo;
}


/**
    This routine completes the USB device configuration for
    the devices supported by USB BIOS. This routine
    handles the generic configuration for the devices.

    @param 
        HcStruc    HCStruc pointer
        DevInfo    Device information structure pointer
        Desc       Pointer to the descriptor structure
        Start      Offset within interface descriptor supported by the device
        End        End offset of the device descriptor

    @retval Pointer to new device info. struc. 0 on error

**/

DEV_INFO*
USB_ConfigureDevice (
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*      Desc,
    UINT16      Start,
    UINT16      End
)
{
    DEV_INFO    *NewDevInfo;
    HC_STRUC    *NewHcStruc;
    DEV_DRIVER  *NewDevDriver;

    NewDevInfo = USBGetProperDeviceInfoStructure(DevInfo, 0);

    if (NewDevInfo == NULL) {
        return NULL;
    }
    //
    // Check whether this device is reconnected by checking the valid
    // structure flag
    //
    if (NewDevInfo->Flag & DEV_INFO_VALID_STRUC) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_ConfigureDevice: Existing device.\n");
        //
        // This device is reconnected. Reuse the old device address so that
        // INT13h can identify this drive properly
        //
        DevInfo->Flag |= NewDevInfo->Flag;
        DevInfo->HidDevType = NewDevInfo->HidDevType;
		NewDevInfo->wDataInSync = 0;
		NewDevInfo->wDataOutSync = 0;
        
        NewHcStruc = gUsbData->HcTable[NewDevInfo->bHCNumber - 1];
        NewDevDriver = UsbFindDeviceDriverEntry(NewDevInfo->fpDeviceDriver);
        if (NewDevDriver && NewDevDriver->pfnDisconnectDevice) {
            if (NewDevInfo->bDeviceType == BIOS_DEV_TYPE_HUB) {
                if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
                    if (!(NewDevInfo->Flag & DEV_INFO_IN_QUEUE)) {
                        USB_SmiQueuePut(NewDevInfo);
                        DevInfo->Flag |= DEV_INFO_IN_QUEUE;
                    }
                }
            }
            NewDevDriver->pfnDisconnectDevice(NewDevInfo);
            NewDevInfo->fpDeviceDriver = NULL;
            } else {
            //
            // Stop polling the device's interrupt endpoint
            //
            if (NewDevInfo->IntInEndpoint) {
                (*gUsbData->aHCDriverTable[GET_HCD_INDEX(NewHcStruc->bHCType)].pfnHCDDeactivatePolling)
										(NewHcStruc, NewDevInfo);
                NewDevInfo->IntInEndpoint = 0;
			}
        }
        if ((NewDevInfo->DevMiscInfo != NULL) && (NewDevInfo->DevMiscInfo != DevInfo->DevMiscInfo)) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(NewHcStruc->bHCType)].pfnHCDDeinitDeviceData)
                                (NewHcStruc, NewDevInfo);
        }
        //
        // Update Device Address Map, preserving the address for registered devices
        //
        if (NewDevInfo->Flag & DEV_INFO_DEV_PRESENT) {
            if (NewDevInfo->bDeviceAddress != 0) {
                gUsbData->DeviceAddressMap |= LShiftU64(1, NewDevInfo->bDeviceAddress);
                NewDevInfo->bDeviceAddress = 0;
            }
        }
    }
    else {
        //
        // Check whether we reached the limit of devices of this type
        //
        if (CheckDeviceLimit(DevInfo->bBaseClass) == TRUE) {
            return NULL;
        }
    }

    //
    // For registered devices skip updating bFlag
    //
    if (!(NewDevInfo->Flag & DEV_INFO_MASS_DEV_REGD)) {
        //
        // Since DeviceInfo[0] already has many fields filled in, the new entry
        // should be initialized with a copy of DeviceInfo[0].  But, the new
        // DeviceInfo should not be  marked as "present" until the device
        // is successfully initialized.
        //
        // Copy old DeviceInfo struc to new DeviceInfo struc and zero device[0]
        //
        CopyMem((UINT8*)NewDevInfo, (UINT8*)DevInfo, sizeof (DEV_INFO));
		NewDevInfo->Flag &= DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT |
								DEV_INFO_MASS_DEV_REGD | DEV_INFO_DEV_BUS |
								DEV_INFO_IN_QUEUE | DEV_INFO_ALT_SETTING_IF;
    } else {
        // Change the parent HC number and port number in the existing DEV_INFO
        NewDevInfo->bHCNumber = DevInfo->bHCNumber;
    	NewDevInfo->bHubDeviceNumber = DevInfo->bHubDeviceNumber;
		NewDevInfo->bHubPortNumber = DevInfo->bHubPortNumber;
		NewDevInfo->bEndpointSpeed = DevInfo->bEndpointSpeed;
		NewDevInfo->wEndp0MaxPacket = DevInfo->wEndp0MaxPacket;
    	NewDevInfo->DevMiscInfo = DevInfo->DevMiscInfo;
        NewDevInfo->bDeviceAddress = DevInfo->bDeviceAddress;
    }

    //
    // Do a SetConfiguration command to the device to set it to its
    // HID/Boot configuration.
    //
    NewDevInfo->Flag |= DEV_INFO_VALIDPRESENT;
    if (!(DevInfo->Flag & DEV_INFO_MULTI_IF)) {
	    USB_SetConfig(HcStruc, NewDevInfo, NewDevInfo->bConfigNum);
        if (DevInfo->Flag & DEV_INFO_ALT_SETTING_IF) {
            UsbSetInterface(HcStruc, NewDevInfo, NewDevInfo->bAltSettingNum);
        }
    }

    USB_DEBUG(DEBUG_INFO, 3, "new dev: %x, flag: %x, addr %d\n",
                NewDevInfo, NewDevInfo->Flag, NewDevInfo->bDeviceAddress);

    return NewDevInfo;

}


/**
    This function checks for non-compliant USB devices by
    by comparing the device's vendor and device id with
    the non-compliant device table list and updates the
    data structures appropriately to support the device.

    @param fpHCStruc - HCStruc pointer
        fpDevInfo - Device information structure pointer
        fpDesc    - Pointer to the descriptor structure
        wDescLength - End offset of the device descriptor

    @retval Updated fpDevInfo->wIncompatFlags field

**/
VOID
USBCheckNonCompliantDevice(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*      Desc,
    UINT16      Length,
    UINT16      DescLength
)
{
    USB_BADDEV_STRUC    *BadDevice;
    INTRF_DESC          *IntrfDesc;

    IntrfDesc = (INTRF_DESC*)((UINT8*)Desc + Length);

    //
    // Search the bad device table to get the structure for this device
    //
    for (BadDevice = gUsbBadDeviceTable;
         BadDevice->wDID | BadDevice->wVID; BadDevice++) {

        if ((BadDevice->wDID != DevInfo->wDeviceId) ||
                (BadDevice->wVID != DevInfo->wVendorId)) {
            continue;
        }
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Found non-compatible device: DID=%x, VID=%x\n", BadDevice->wDID, BadDevice->wVID);
        //
        // Save the incompatibility flag into device info structure
        //
        DevInfo->wIncompatFlags = BadDevice->wFlags;

        //
        // Check which fields to update in the interface descriptor
        //
        // Check for base class field
        //
        if (BadDevice->bBaseClass) {
            //
            // Update base class field in the interface descriptor
            //
            IntrfDesc->bBaseClass = BadDevice->bBaseClass;
        }
        //
        // Check for base sub class field
        //
        if (BadDevice->bSubClass) {
            //
            // Update sub class field in the interface descriptor
            //
            IntrfDesc->bSubClass = BadDevice->bSubClass;
        }
        //
        // Check for protocol field
        //
        if (BadDevice->bProtocol) {
            //
            // Update protocol field in the interface descriptor
            //
            IntrfDesc->bProtocol = BadDevice->bProtocol;
        }
        break;
    }
}


/**
    This routine invokes the device drivers 'check device type'
    routine and identifies the device type.

    @param pHCStruc    HCStruc pointer
        pDevInfo    Device information structure pointer
        pDesc       Pointer to the descriptor structure
        wStart      Offset within interface descriptor
        supported by the device
        wEnd        End offset of the device descriptor

    @retval Pointer to new device info. struc, NULL on error


**/

DEV_INFO*
USBIdentifyAndConfigureDevice (
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*      Desc,
    UINT16      Length,
    UINT16      DescLength)
{
    UINT8           BaseClass;
    UINT8           SubClass;
    UINT8           Protocol;
    UINT8           Index;
    UINT8           RetValue;
    UINT8			MaxIndex;
    DEV_INFO*       DevInfoLocal;
    DEV_DRIVER*     DevDriver;
    INTRF_DESC*     IntrfDesc;

    //
    // Check for non-compliant device. If non-compliant device found then
    // the descriptor values will get updated depending on the need.
    //
    USBCheckNonCompliantDevice (
            HcStruc,
            DevInfo,
            Desc,
            Length,
            DescLength);
    
	UsbOemCheckNonCompliantDevice(
	    HcStruc,
		DevInfo,
		Desc,
		Length,
		DescLength);
	
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBIdentifyAndConfigureDevice...");

    //
    // Check whether device needs to be disable
    //
    if (DevInfo->wIncompatFlags & USB_INCMPT_DISABLE_DEVICE) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "not compatible device.\n");
        return NULL;
    }

    IntrfDesc = (INTRF_DESC*)(Desc + Length);

//(EIP74609+)>
	if(OEMSkipList(DevInfo->bHubDeviceNumber,DevInfo->bHubPortNumber,HcStruc->wBusDevFuncNum,IntrfDesc->bBaseClass,1)) {
        USB_DEBUG(DEBUG_INFO, 3, "Match the skip table ; skipping this device.\n");   //(EIP98145) 
		return NULL;
	}	
//<(EIP74609+) 
    //
    // Get the base, sub class & protocol values
    //
    BaseClass  = IntrfDesc->bBaseClass;
    SubClass   = IntrfDesc->bSubClass;
    Protocol   = IntrfDesc->bProtocol;

    //
    // Check for matching device driver
    //
    DevInfoLocal = NULL;
    RetValue = USB_ERROR;
    
#if !USB_RT_DXE_DRIVER
    Index = 0;
    MaxIndex = MAX_DEVICE_TYPES;
#else
    Index = MAX_DEVICE_TYPES;
    MaxIndex = MAX_DEVICE_TYPES * 2;
#endif
                                        
                                        //(EIP96616+)>
    for (; Index < MaxIndex; Index ++) {
        DevDriver = &gUsbData->aDevDriverTable[Index];
        //
        // Check structure validity
        //
        if (!DevDriver->bDevType) {
            continue;       // Driver table not valid
        }
        //
        // Verify presence of Check Device routine
        //
        if (DevDriver->pfnCheckDeviceType) {
            //
            // Check device type is implemented. Execute it!
            //
            RetValue = (*DevDriver->pfnCheckDeviceType)(
                           DevInfo,BaseClass,
                           SubClass,Protocol);
                if (RetValue != USB_ERROR)
                    break;
            }  else {
            //
            // Check device type is not implemented. Compare the class codes
            //
            if((DevDriver->bBaseClass == BaseClass) ||
                (DevDriver->bSubClass == SubClass) ||
                (DevDriver->bProtocol == Protocol)) {
                //
                // If the class codes match set bRetValue with the bDevType from the Device Driver
                //
                RetValue = DevDriver->bDevType;
                break;
            }
        }
    }
    if (RetValue != USB_ERROR) {
        //
        // Check whether we reached the limit of devices of this type
        //
        //if (CheckDeviceLimit(bBaseClass) == TRUE) continue;   //(EIP81761-)

        //
        // Set the device type in the Device Info structure
        //
        DevInfo->bDeviceType  = RetValue;

        //
        // Set Base Class, Subclass and Protocol information
        //
        DevInfo->bBaseClass = BaseClass;
        DevInfo->bProtocol = Protocol;
        DevInfo->bSubClass = SubClass;

        //
        // Device identified. Issue common configure call
        // Call a common routine to handle the remaining initialization that is done
        // for all devices.
        //
        DevInfoLocal = USB_ConfigureDevice(
                            HcStruc,
                            DevInfo,
                            Desc,
                            Length,
                            DescLength);

        if (DevInfoLocal == NULL) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "USB: Common configure failed.\n");
            return DevInfoLocal;
        }

        if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
            if (!(DevInfoLocal->Flag & DEV_INFO_IN_QUEUE)) {
                USB_SmiQueuePut(DevInfoLocal);
                DevInfoLocal->Flag |= DEV_INFO_IN_QUEUE;
            }
        }

        DevInfoLocal->fpDeviceDriver = DevDriver;
        DevInfoLocal = (*DevDriver->pfnConfigureDevice)(
                                HcStruc,
                                DevInfoLocal,
                                Desc,
                                Length,
                                DescLength);
        if (!DevInfoLocal ||
        !(DevInfoLocal->Flag & DEV_INFO_VALID_STRUC)) {
            DevInfoLocal = 0;
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "USB: Device specific configure failed.\n");
            return DevInfoLocal;
        }

                                        //<(EIP96616+)
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "%x\n", DevInfoLocal);

    return DevInfoLocal;
}

/**
    This routine initializes the frame list pointed by fpPtr
    with the dValue provided

    @param fpHCStruc   Pointer to the Host Controller structure
        dValue  Value to be initialized with

    @retval Nothing

**/


VOID
USB_InitFrameList(
    HC_STRUC*   HcStruc,
    UINT32      Value)
{
    UINT16  Index;
    UINT32  *Ptr = (UINT32*)HcStruc->fpFrameList;

    for (Index = 0; Index < HcStruc->wAsyncListSize; Index ++) {
        Ptr[Index] = Value;
    }
    return;
}


/**
    This function handles different key repeat related functions
    depending on the input

    @param fpHCStruc - pointer for the HC that implements the key repeat function
        bAction   - sub-function index:
        0  Install key repeat HCStruc
        1  Disable key repeat
        2  Enable key repeat
        3  Uninstall key repeat HCStruc

    @retval VOID

    @note  fpHCStruc is only relevant for sub-function 0.

**/

VOID
USBKeyRepeat(
    HC_STRUC*   HcStruc,
    UINT8       Action
)
{
	UINT8		i;

    switch (Action) {
        case 0:     // Sub-function 0: Save the HCStruc value for later use
            if (gKeyRepeatStatus == FALSE) {
                gUsbData->fpKeyRepeatHCStruc = HcStruc;
        	}
            break;
        case 1:     // Sub-function 0: Disable key repeat
			if (gKeyRepeatStatus) {
#if USB_HID_KEYREPEAT_USE_SETIDLE == 1 
	            if(gUsbData->fpKeyRepeatDevInfo != NULL) {
	            //
	            // Set the HID SET_IDLE request to 0
	            //
	            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable[gUsbData->fpKeyRepeatDevInfo->bHCNumber - 1]->bHCType)].pfnHCDControlTransfer)
	                    (gUsbData->HcTable[gUsbData->fpKeyRepeatDevInfo->bHCNumber - 1],
	                    gUsbData->fpKeyRepeatDevInfo,(UINT16)HID_RQ_SET_IDLE, gUsbData->fpKeyRepeatDevInfo->bInterfaceNum, 0, 0, 0);	//(EIP54782)
	            }
#else
	            if (gUsbData->fpKeyRepeatHCStruc) {
	                (*gUsbData->aHCDriverTable[GET_HCD_INDEX(
	                    gUsbData->fpKeyRepeatHCStruc->bHCType)].pfnHCDDisableKeyRepeat)(
	                        gUsbData->fpKeyRepeatHCStruc);
	            }
#endif
				gKeyRepeatStatus = FALSE;
			}
            break;
        case 2:     // Sub-function 0: Enable key repeat
        	if (!gKeyRepeatStatus) {
#if USB_HID_KEYREPEAT_USE_SETIDLE == 1 
	            if(gUsbData->fpKeyRepeatDevInfo != NULL) {
	                //
	                // Set the HID SET_IDLE request to 0x200 (8ms)
	                //
	                (*gUsbData->aHCDriverTable[GET_HCD_INDEX(gUsbData->HcTable[gUsbData->fpKeyRepeatDevInfo->bHCNumber - 1]->bHCType)].pfnHCDControlTransfer)
	                        (gUsbData->HcTable[gUsbData->fpKeyRepeatDevInfo->bHCNumber - 1],
	                        gUsbData->fpKeyRepeatDevInfo,(UINT16)HID_RQ_SET_IDLE, gUsbData->fpKeyRepeatDevInfo->bInterfaceNum, 0x400, 0, 0);	//(EIP54782)
	            }
#else 
	            if (gUsbData->fpKeyRepeatHCStruc) {
	                (*gUsbData->aHCDriverTable[GET_HCD_INDEX(
	                    gUsbData->fpKeyRepeatHCStruc->bHCType)].pfnHCDEnableKeyRepeat)(
	                        gUsbData->fpKeyRepeatHCStruc);
	            }
#endif
				gKeyRepeatStatus = TRUE;
        	}
            break;
		case 3:
        	if (gUsbData->fpKeyRepeatHCStruc == HcStruc) {
            	gUsbData->fpKeyRepeatHCStruc = NULL;
				for (i = 0; i < gUsbData->HcTableCount; i++) {
                    if (gUsbData->HcTable[i] == NULL) {
                        continue;
                    }
                    if (gUsbData->HcTable[i] == HcStruc) {
                        continue;
                    }
                    if (gUsbData->HcTable[i]->HwSmiHandle == NULL) {
                        continue;
                    }
					if (gUsbData->HcTable[i]->dHCFlag & HC_STATE_RUNNING) {
						gUsbData->fpKeyRepeatHCStruc = gUsbData->HcTable[i];
                        if (gKeyRepeatStatus) {
                            gKeyRepeatStatus = FALSE;
                            USBKeyRepeat(NULL, 2);
                        }
                        break;
					}
				}
        	}
			break;
    }
}

/**
    Install drivers that redirects ...????

    @param fpDevDriver - record that the routine can use to install the drive
    @retval VOID

**/

UINT8
USB_bus_interrupt_handler (
    HC_STRUC	*HcStruc,
    DEV_INFO	*DevInfo,
    UINT8		*Td,
    UINT8		*Buffer,
    UINT16      DataLength
)
{
	USBHC_INTERRUPT_DEVNINFO_T *Idi = (USBHC_INTERRUPT_DEVNINFO_T *)DevInfo->pExtra;
	EFI_STATUS  Status = EFI_SUCCESS;
//	ASSERT(Idi);
	if (Idi == NULL) {
		return USB_SUCCESS;
	}
	
#if !USB_RT_DXE_DRIVER	
	Status = AmiValidateMemoryBuffer((VOID*)Idi, sizeof(USBHC_INTERRUPT_DEVNINFO_T));
    if (EFI_ERROR(Status)) {
        return USB_ERROR;
    }
#endif
	
	USB_SmiQueuePutMsg(&Idi->QCompleted, Buffer, (int)Idi->DataLength);
    return USB_SUCCESS;
}

VOID
UsbBusDeviceInit(
    VOID
)
{
    USB_InstallCallBackFunction(USB_bus_interrupt_handler);
    return;
}

DEV_INFO*
USB_on_configDev(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Desc,
    UINT16      Start,
    UINT16      End
)
{
    DevInfo->bDeviceType      = (UINT8)BIOS_DEV_TYPE_USBBUS;
    DevInfo->bCallBackIndex   = USB_InstallCallBackFunction(USB_bus_interrupt_handler);
    return(DevInfo);
}

UINT8
USB_on_identifyDev(
    DEV_INFO*   DevInfo,
    UINT8       BaseClass,
    UINT8       SubClass,
    UINT8       Protocol
)
{
                                        //(EIP96616+)>
    if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)
        return  BIOS_DEV_TYPE_USBBUS;
    else
        return  USB_ERROR;
                                        //<(EIP96616+)
}

UINT8
USB_on_disconnectDev(
    DEV_INFO* DevInfo
)
{
    return  USB_SUCCESS;
}

VOID
BusFillDriverEntries(
    DEV_DRIVER  *DevDriver
)
{
    DevDriver->bDevType               = BIOS_DEV_TYPE_USBBUS;
    DevDriver->bBaseClass             = 0;
    DevDriver->bSubClass              = 0;
    DevDriver->bProtocol              = 0;
    DevDriver->pfnDeviceInit          = UsbBusDeviceInit;
    DevDriver->pfnCheckDeviceType     = USB_on_identifyDev;
    DevDriver->pfnConfigureDevice     = USB_on_configDev;
    DevDriver->pfnDisconnectDevice    = USB_on_disconnectDev;
}


/**
    EFI code will call this function to give a chance for
    SMI dev driver to complete the configuration of device

    Before call, USB device is connected, address is assigned
    and configuration is set. DEV_INFO structure is initalized
    from information parsed from descriptors and linked
    to USBBUS dev driver. Device driver specific to the type
    of USB device wasn't called on this device

    After the call returns, a specific device driver
    initialization was  performed by calling pfnCheckDeviceType
    and pfnConfigureDevice functions of device driver. Parameters
    to those functions are taken from descriptors downloaded from
    the device. Device preserve old address and active configuration
**/
int USB_ReConfigDevice2( HC_STRUC* HcStruc, DEV_INFO* DevInfo,
                        CNFG_DESC   *CnfgDesc,
                        INTRF_DESC * IntrfDesc );

UINT32 
USB_ReConfigDevice(
    HC_STRUC    *HcStruc, 
    DEV_INFO    *DevInfo
)
{
    INTRF_DESC  *IntrfDesc = NULL;
    UINT8       Config;
    UINT32      Status = USB_SUCCESS;
    UINT8       *Buffer;
    CNFG_DESC   *CnfgDesc = NULL;
    UINT16      OrgTimeOutValue;
    UINT16      TotalLength;
    UINT16      Offset;
    UINT16      DescLength;
    INTRF_DESC  *Intrf;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    gCheckUsbApiParameter = FALSE;

    if (DevInfo->bDeviceType != 0 && 
        DevInfo->bDeviceType != BIOS_DEV_TYPE_USBBUS) {
        return USB_SUCCESS;
    }

    Buffer = USB_MemAlloc(GET_MEM_BLK_COUNT(MAX_CONTROL_DATA_SIZE));
    if (Buffer == NULL) {
        return USB_ERROR;
    }
    //
    // Find configuration desc
    //
    for (Config = 0; Config < DevInfo->DevDesc.NumConfigs; ++Config){
        
        OrgTimeOutValue = gUsbData->wTimeOutValue;
        gUsbData->wTimeOutValue = USB_GET_CONFIG_DESC_TIMEOUT_MS;
        
        CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(
            HcStruc,
            DevInfo,
            Buffer,
            0xFF,
            DESC_TYPE_CONFIG,
            Config);

        if (CnfgDesc == NULL) {
            break;
        }
        
        TotalLength = CnfgDesc->wTotalLength;

        if (TotalLength > 0xFF) {
            if (TotalLength > (MAX_CONTROL_DATA_SIZE - 1)) {
                TotalLength = MAX_CONTROL_DATA_SIZE - 1;
            }
            CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(
                        HcStruc,
                        DevInfo,
                        Buffer,
                        TotalLength,
                        DESC_TYPE_CONFIG,
                        Config);
            if (CnfgDesc == NULL) {
                break;
            }
        }
        
        gUsbData->wTimeOutValue = OrgTimeOutValue;
        
        if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_FULL) {
            FixedDelay(1000);           
        }
        if (CnfgDesc != NULL && CnfgDesc->bDescType == DESC_TYPE_CONFIG &&
            DevInfo->bConfigNum == CnfgDesc->bConfigValue){
            break;
        }

        CnfgDesc = NULL;
    }

    if (CnfgDesc) {
        if (CnfgDesc->wTotalLength > MAX_CONTROL_DATA_SIZE - 1) {
            CnfgDesc->wTotalLength = MAX_CONTROL_DATA_SIZE - 1;
        }
        DescLength = CnfgDesc->wTotalLength;
        for (Offset = (UINT16)CnfgDesc->bDescLength; Offset <DescLength; Offset = Offset + (UINT16)Intrf->bDescLength) {
            Intrf = (INTRF_DESC*)((UINT8*)CnfgDesc + Offset);
            if (Intrf->bDescLength == 0) {
                break;
            }
            if (Intrf->bDescType == DESC_TYPE_INTERFACE &&
                DevInfo->bInterfaceNum == Intrf->bInterfaceNum &&
                DevInfo->bAltSettingNum == Intrf->bAltSettingNum ) {
                IntrfDesc = Intrf;
                break;
            }
        }
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3,
        "USB_reConfigDev:: CfgDsc=%x; IntrfDsc=%x\n",
        CnfgDesc, IntrfDesc);

    if (IntrfDesc && CnfgDesc) {
        Status = USB_ReConfigDevice2(HcStruc, DevInfo,
                                    CnfgDesc, IntrfDesc);
    } else {
        Status = USB_ERROR;
    }

    USB_MemFree(Buffer, (UINT8)(MAX_CONTROL_DATA_SIZE / sizeof(MEM_BLK)));

    return Status;
}

//----------------------------------------------------------------------------
//  USB_ReConfigDevice2
//----------------------------------------------------------------------------
int
USB_ReConfigDevice2(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    CNFG_DESC   *CnfgDesc,
    INTRF_DESC  *IntrfDesc
)
{
//  int             abort=0;
    UINTN           Index;
    UINTN           MaxIndex;
    UINT8               RetValue = USB_ERROR;
    DEV_DRIVER          *DevDriver = NULL;
    DEV_INFO            *DevInfoLocal;
    UINT8               BaseClass, SubClass, Protocol;
    EFI_STATUS      EfiStatus;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_ReConfigDevice2.\n");

    EfiStatus = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    
#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        EfiStatus = AmiValidateMemoryBuffer((VOID*)CnfgDesc, sizeof(CNFG_DESC));
        if (EFI_ERROR(EfiStatus)) {
            return USB_ERROR;
        }
        EfiStatus = AmiValidateMemoryBuffer((VOID*)CnfgDesc, CnfgDesc->wTotalLength);
        if (EFI_ERROR(EfiStatus)) {
            return USB_ERROR;
        }
        EfiStatus = AmiValidateMemoryBuffer((VOID*)IntrfDesc, sizeof(INTRF_DESC));
        if (EFI_ERROR(EfiStatus)) {
            return USB_ERROR;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    //
    // Check for non-compliant device. If non-compliant device found then
    // the descriptor values will get updated depending on the need.
    //
    USBCheckNonCompliantDevice (
        HcStruc,
        DevInfo,
        (UINT8*)CnfgDesc,
        CnfgDesc->bDescLength,
        CnfgDesc->wTotalLength);

    //
    // Check whether device needs to be disable
    //
    if (DevInfo->wIncompatFlags & USB_INCMPT_DISABLE_DEVICE)
    {
        return USB_ERROR;
    }

    //
    // Get the base, sub class & protocol values
    //
    BaseClass  = IntrfDesc->bBaseClass;
    SubClass   = IntrfDesc->bSubClass;
    Protocol   = IntrfDesc->bProtocol;

    //
    // Check for matching device driver
    //
    DevInfoLocal = NULL;
#if !USB_RT_DXE_DRIVER
    Index = 0;
    MaxIndex = MAX_DEVICE_TYPES;
#else
    Index = MAX_DEVICE_TYPES;
    MaxIndex = MAX_DEVICE_TYPES * 2;
#endif
    for (RetValue = USB_ERROR;
        Index < MaxIndex && RetValue == USB_ERROR; Index++) {
        DevDriver = &gUsbData->aDelayedDrivers[Index];
        if (!DevDriver->bDevType)
            continue;
        if (DevDriver->pfnCheckDeviceType){
            RetValue = (*DevDriver->pfnCheckDeviceType)(
                DevInfo,BaseClass,SubClass,Protocol);
        }else if((DevDriver->bBaseClass == BaseClass) &&
                (DevDriver->bSubClass == SubClass) &&
                (DevDriver->bProtocol == Protocol)){
            RetValue = DevDriver->bDevType;
        }
    }
    if(RetValue == USB_ERROR)
        return RetValue;

    //driver was found

    DevInfo->bDeviceType = RetValue;
    DevInfo->fpDeviceDriver = DevDriver;
    DevInfoLocal = (*DevDriver->pfnConfigureDevice)(
        HcStruc,DevInfo,(UINT8*)CnfgDesc,
        (UINT16)(UINTN)((char*)IntrfDesc - (char*)CnfgDesc),CnfgDesc->wTotalLength);

    if ((!DevInfoLocal) ||(DevInfo->fpDeviceDriver != DevDriver)) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_0, "USB_ReConfigDevice2: Device specific configure failed.\n");
        return USB_ERROR;
    }
    return USB_SUCCESS;
}

/**
    Search for the HC_STRUC with specified bHCNumber
**/

HC_STRUC*
hcnum2hcstruc(
    UINT8 HCNumber
)
{
    return gUsbData->HcTable[HCNumber - 1];
}

/**
    Changes global state of USBSMI module to function properly
    in non-EFI OS - without support from EFI drivers

    Before call USB BUS is a driver that handles all devices (
    except hub) and rest of the drivers are delayed. Number of
    devices are supported by SUBBUS driver and custom EFI driver

    After call returns, USBBUS driver is removed and all drivers
    that where
    delayed became active. All USBBUS devices are reconfigured.
    Devices that are not supported by now active drivers are decon-
    figured.
**/

VOID
PrepareForLegacyOs(
    VOID
)
{
    UINTN       Index;
    DEV_INFO    *DevInfo;
    HC_STRUC    *HcStruc;

    gCheckUsbApiParameter = FALSE;

    //
    //First Reconfigure all USBBUS device (while drivers are in delayed array)
    //
    for (Index = 1; Index < MAX_DEVICES; Index++) {
		DevInfo = &gUsbData->aDevInfoTable[Index];
        if (((DevInfo->Flag & DEV_INFO_VALIDPRESENT) == DEV_INFO_VALIDPRESENT) &&
            (DevInfo->bDeviceType == BIOS_DEV_TYPE_USBBUS)) {
            HcStruc = hcnum2hcstruc(DevInfo->bHCNumber);
            USB_ReConfigDevice(HcStruc, DevInfo);
        }
    }
 
    USBKeyRepeat(NULL, 1);  // Disable key repeat
    
}

/**
    This routine resets and reconfigures the device.

**/

UINT32
USB_ResetAndReconfigDev(
	HC_STRUC    *HcStruc,
	DEV_INFO    *DevInfo
)
{
	DEV_INFO	*Dev;
	UINT32		Status;
	UINT8		DevAddr;
	UINT8		*Buffer;
    DEV_DESC	*DevDesc;
    CNFG_DESC	*CnfgDesc;
    INTRF_DESC	*IntrfDesc;
	UINT8		ConfigIndx;
	UINT8		IntrfIndx;
	DEV_DRIVER	*DevDriver;
	UINT8		i;
    UINT32      PortStatus;
    UINT8       *DevMiscInfo;
    UINT16      TotalLength;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    gCheckUsbApiParameter = FALSE;
    
	for (i = 1; i < MAX_DEVICES; i++) {
		Dev = &gUsbData->aDevInfoTable[i];
		if ((Dev->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT | 
			DEV_INFO_DEV_DUMMY)) != (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT)) {
			continue;
		}
		if ((Dev->bHubDeviceNumber == DevInfo->bHubDeviceNumber) && 
			(Dev->bHubPortNumber == DevInfo->bHubPortNumber) &&
			(Dev->bDeviceType != BIOS_DEV_TYPE_USBBUS)) {
			(*gUsbData->aHCDriverTable[GET_HCD_INDEX(
					HcStruc->bHCType)].pfnHCDDeactivatePolling)(HcStruc, Dev);
		}
	}

    Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeinitDeviceData)
                                (HcStruc, DevInfo);
	if (Status != USB_SUCCESS) {
		return Status;
	}
	
	Status = USB_ResetHubPort(HcStruc, DevInfo->bHubDeviceNumber, DevInfo->bHubPortNumber);
	if (Status != USB_SUCCESS) {
		return Status;
	}

	Status = USB_EnableHubPort(HcStruc, DevInfo->bHubDeviceNumber, DevInfo->bHubPortNumber);
	if (Status != USB_SUCCESS) {
		return Status;
	}
    
    PortStatus = USB_GetHubPortStatus(HcStruc, DevInfo->bHubDeviceNumber, DevInfo->bHubPortNumber, TRUE);

    if (PortStatus == USB_ERROR) {
        return USB_ERROR;
    }

    if (!(PortStatus & USB_PORT_STAT_DEV_ENABLED)) {
        return USB_ERROR;
    }

    // Initialize HC specific data before device configuration
    Status = (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDInitDeviceData)(
                        HcStruc, DevInfo, PortStatus, &DevMiscInfo);
    if (Status != USB_SUCCESS) {
        return Status;
    }

    DevInfo->DevMiscInfo = (VOID*)DevMiscInfo;

    Buffer = USB_MemAlloc(GET_MEM_BLK_COUNT(sizeof(DEV_DESC)));
    if (Buffer == NULL) {
        return USB_ERROR;
    }

	DevAddr = DevInfo->bDeviceAddress;
	DevInfo->bDeviceAddress = 0;

	DevDesc = (DEV_DESC*)USB_GetDescriptor(HcStruc, DevInfo, Buffer, sizeof(DEV_DESC), 
				DESC_TYPE_DEVICE, 0);
	if(DevDesc == NULL) {
		USB_MemFree(Buffer, GET_MEM_BLK_COUNT(sizeof(DEV_DESC)));
		return USB_ERROR;
	}

	Status = USB_SetAddress(HcStruc, DevInfo, DevAddr);	
	if (Status != USB_SUCCESS) {
		USB_MemFree(DevDesc, GET_MEM_BLK_COUNT(sizeof(DEV_DESC)));
		return Status;
	}	

	DevInfo->bDeviceAddress = DevAddr;

    Buffer = USB_MemAlloc(GET_MEM_BLK_COUNT(MAX_CONTROL_DATA_SIZE));
    if (Buffer == NULL) {
		USB_MemFree(DevDesc, GET_MEM_BLK_COUNT(sizeof(DEV_DESC)));
        return USB_ERROR;
    }

    for (ConfigIndx = 0; ConfigIndx < DevDesc->NumConfigs; ConfigIndx++) {
        CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(HcStruc, DevInfo, Buffer, 
						0xFF, DESC_TYPE_CONFIG, ConfigIndx);
		if (CnfgDesc == NULL) {
			continue;
		}
		TotalLength = CnfgDesc->wTotalLength;
        if (TotalLength > 0xFF) {
            if (TotalLength > (MAX_CONTROL_DATA_SIZE - 1)) {
                TotalLength = MAX_CONTROL_DATA_SIZE - 1;
            }
            CnfgDesc = (CNFG_DESC*)USB_GetDescriptor(HcStruc, DevInfo, Buffer, 
						TotalLength, DESC_TYPE_CONFIG, ConfigIndx);
            if (CnfgDesc == NULL) {
                continue;
            }
        }

        if (CnfgDesc->bDescType == DESC_TYPE_CONFIG) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDEnableEndpoints)(
                        HcStruc, DevInfo, (UINT8*)CnfgDesc);
        }

		USB_SetConfig(HcStruc, DevInfo, CnfgDesc->bConfigValue);
	
		IntrfDesc = (INTRF_DESC*)CnfgDesc;
		for (IntrfIndx = 0; IntrfIndx < CnfgDesc->bNumInterfaces; IntrfIndx++) {
			do {
				IntrfDesc = (INTRF_DESC*)((UINTN)IntrfDesc + IntrfDesc->bDescLength);
				if ((UINTN)IntrfDesc > ((UINTN)CnfgDesc + CnfgDesc->wTotalLength) ||
					(UINTN)IntrfDesc > ((UINTN)CnfgDesc + MAX_CONTROL_DATA_SIZE)) {
					break;
				}
			} while (IntrfDesc->bDescType != DESC_TYPE_INTERFACE);

			if (IntrfDesc->bDescType != DESC_TYPE_INTERFACE) {
				break;
			}

			for (i = 1; i < MAX_DEVICES; i++) {
				Dev = &gUsbData->aDevInfoTable[i];
				if ((Dev->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT | 
					DEV_INFO_DEV_DUMMY)) != (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT)) {
					continue;
				}
				if ((Dev->bHubDeviceNumber == DevInfo->bHubDeviceNumber) && 
					(Dev->bHubPortNumber == DevInfo->bHubPortNumber) &&
					(Dev->bConfigNum == CnfgDesc->bConfigValue) &&
					(Dev->bInterfaceNum == IntrfDesc->bInterfaceNum) &&
					(Dev->bAltSettingNum == IntrfDesc->bAltSettingNum)) {
					break;
				}
			}
			if (i == MAX_DEVICES) {
				continue;
			}

			Dev->wVendorId = DevDesc->VendorId;
			Dev->wDeviceId = DevDesc->DeviceId;

			if (Dev->bDeviceType != BIOS_DEV_TYPE_USBBUS) {
				DevDriver = UsbFindDeviceDriverEntry(DevInfo->fpDeviceDriver);
				if (DevDriver != NULL) {
                    (*DevDriver->pfnConfigureDevice)(HcStruc, Dev, (UINT8*)CnfgDesc,
                        (UINT16)((UINTN)IntrfDesc - (UINTN)CnfgDesc), CnfgDesc->wTotalLength);
				}
			}
		}
	}

	USB_MemFree(DevDesc, GET_MEM_BLK_COUNT(sizeof(DEV_DESC)));
	USB_MemFree(Buffer, GET_MEM_BLK_COUNT(MAX_CONTROL_DATA_SIZE));

	return USB_SUCCESS;
}

/**
**/

UINT32
USB_DevDriverDisconnect(
	HC_STRUC    *HcStruc,
	DEV_INFO    *DevInfo
)
{
	DEV_DRIVER* DevDriver;
	UINT8		Index;
    UINT8       MaxIndex;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    gCheckUsbApiParameter = FALSE;

	DevDriver = UsbFindDeviceDriverEntry(DevInfo->fpDeviceDriver);
    
	if (DevDriver && DevDriver->pfnDisconnectDevice) {
		DevDriver->pfnDisconnectDevice(DevInfo);

		DevInfo->bDeviceType = 0;
		DevInfo->fpDeviceDriver = NULL;

#if !USB_RT_DXE_DRIVER
    Index = 0;
    MaxIndex = MAX_DEVICE_TYPES;
#else
    Index = MAX_DEVICE_TYPES;
    MaxIndex = MAX_DEVICE_TYPES * 2;
#endif

		for (; Index < MaxIndex; Index++) {
			DevDriver = &gUsbData->aDevDriverTable[Index];

			if (DevDriver->bDevType == BIOS_DEV_TYPE_USBBUS) {
				DevInfo->bDeviceType = DevDriver->bDevType;
				DevDriver->pfnConfigureDevice(HcStruc, DevInfo, NULL, 0, 0);
				break;
			}
		}
	} else {
		if (DevInfo->IntInEndpoint) {
			// Stop polling the device's interrupt endpoint
			(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeactivatePolling)
							(HcStruc, DevInfo);
			DevInfo->IntInEndpoint = 0;
		}
	}

	return USB_SUCCESS;
}

/**
    Checks if DEV_INFO is a valid connected device info
    Due to hot-plug a DEV_INFO can become invalid in the
    midle of configuration
**/
int VALID_DEVINFO(DEV_INFO* DevInfo)
{
    return (DevInfo->Flag & DEV_INFO_VALIDPRESENT)!=0;
}


/**
    Mark DEV_INFO not valid for all the devices connected to a
    given hub.

**/

VOID
USB_AbortConnectHubChildren(
    UINT8 HubAddr
)
{
    UINT8       i;
    DEV_INFO    *Dev = &gUsbData->aDevInfoTable[1];

    for (i=1;  i<MAX_DEVICES; i++, Dev++) {
        if ((Dev->bHubDeviceNumber == HubAddr) && (Dev->Flag & DEV_INFO_VALIDPRESENT)) {
			Dev->Flag &= ~DEV_INFO_DEV_PRESENT;
			if (!(Dev->Flag & DEV_INFO_MASS_DEV_REGD)) {
            	Dev->Flag &= ~DEV_INFO_VALID_STRUC;
			}

            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB: abort device [%x] connected to hub[%x]\n",
                Dev->bDeviceAddress, HubAddr);

            if (Dev->bDeviceType == BIOS_DEV_TYPE_HUB) {
                USB_AbortConnectHubChildren(Dev->bDeviceAddress);
            }
        }
    }
}


/**
    This routine releases the given device's address by
    updating gUsbData->dDeviceAddressMap.

**/

VOID
USB_FreeDeviceAddress(
    DEV_INFO    *DevInfo
)
{
    UINT8   i;
    UINT8   Found = 0;

    if (DevInfo->bDeviceAddress) {
        for (i = 1; i<MAX_DEVICES; i++) {
            if (gUsbData->aDevInfoTable+i != DevInfo &&
                gUsbData->aDevInfoTable[i].bDeviceAddress == DevInfo->bDeviceAddress) {
                Found++;
            }
        }
        if (Found == 0){
            //The DevInfo was the only function with allocated address -
            // return the address to the pool
            if (DevInfo->bDeviceAddress != 0) {
                gUsbData->DeviceAddressMap |= LShiftU64(1, DevInfo->bDeviceAddress);
                DevInfo->bDeviceAddress = 0;
            }
        }
    }
}


/**
    Mark DEV_INFO not valid and release its device address

**/

VOID
USB_AbortConnectDev(
    DEV_INFO* DevInfo
)
{
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB: abort connect [%x].flag = %x\n",
          DevInfo, DevInfo->Flag);

	DevInfo->Flag &= ~DEV_INFO_DEV_PRESENT;

	if (!(DevInfo->Flag & DEV_INFO_MASS_DEV_REGD)) {
	    DevInfo->Flag &= ~DEV_INFO_VALID_STRUC;
	    if (DevInfo->bDeviceAddress == 0) return;

	    USB_FreeDeviceAddress(DevInfo);
	}

    // Remove children (if any) from aborted parent hub device.
    // Assume the child device has not been connected since
    // the hub has to be connected first.
    if (DevInfo->bDeviceType == BIOS_DEV_TYPE_HUB) {
        USB_AbortConnectHubChildren(DevInfo->bDeviceAddress);
    }
}


/**
    Puts the pointer pointer into the queue for processing,
    updates queue head and tail.

**/

VOID
USB_SmiQueuePut(VOID * d)
{
    QUEUE_T* q = &gUsbData->QueueCnnctDisc;

    while (q->head >= q->maxsize) {
        q->head -= q->maxsize;
    }

    q->data[q->head++] = d;
    if (q->head == q->maxsize) {
        q->head -= q->maxsize;
    }
    if (q->head == q->tail) {
        //Drop data from queue
        q->tail++;
        while (q->tail >= q->maxsize) {
            q->tail -= q->maxsize;
        }
    }
}

/**
    Add a variable size item to the queue

**/

VOID
USB_SmiQueuePutMsg( QUEUE_T* q, VOID * d, int sz )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if (q->head + sz > q->maxsize) {
        q->head = 0;
    }
#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)((UINTN)q->data + q->head), sz);
    if (EFI_ERROR(Status)) {
        return;
    }
#endif
    CopyMem((UINT8*)((UINTN)q->data + q->head), (UINT8*)d, sz);
    q->head += sz;
    if(q->head==q->maxsize) q->head = 0;
    if(q->head==q->tail){
        //Drop data from queue
        q->tail+=sz;
        if( q->tail >= q->maxsize ) q->tail = 0;
    }
}

/**
    Verifies whether the number of initialized devices of a given
    class has reached the limit.

**/

BOOLEAN
CheckDeviceLimit(
    UINT8   BaseClass
)
{
    URP_STRUC Urp;
    UINT8 DevNumber;

    Urp.bFuncNumber = USB_API_CHECK_DEVICE_PRESENCE;
    Urp.bSubFunc = 1;
    Urp.ApiData.ChkDevPrsnc.fpHCStruc = NULL;
    Urp.ApiData.ChkDevPrsnc.bDevType = BaseClass;

    USBAPI_CheckDevicePresence(&Urp);

    if (Urp.bRetValue == USB_SUCCESS)
    {
        DevNumber = Urp.ApiData.ChkDevPrsnc.bNumber;
        if ((BaseClass == BASE_CLASS_HID)
              && ((USB_DEV_HID_COUNT == 0) || (DevNumber == USB_DEV_HID_COUNT)))
        {
            USB_DEBUG(DEBUG_INFO, 3, "Reached the limit of supported HIDs (%d); skipping this device.\n", USB_DEV_HID_COUNT);
            return TRUE;
        }

        if ((BaseClass == BASE_CLASS_HUB)
              && ((USB_DEV_HUB_COUNT == 0) || (DevNumber == USB_DEV_HUB_COUNT)))
        {
            USB_DEBUG(DEBUG_INFO, 3, "Reached the limit of supported HUBs (%d); skipping this device.\n", USB_DEV_HUB_COUNT);
            return TRUE;
        }

        if ((BaseClass == BASE_CLASS_MASS_STORAGE)
              && ((USB_DEV_MASS_COUNT == 0) || (DevNumber == USB_DEV_MASS_COUNT)))
        {
            USB_DEBUG(DEBUG_INFO, 3, "Reached the limit of supported Mass Storage Devices (%d); skipping this device.\n", USB_DEV_MASS_COUNT);
            return TRUE;
        }
        if ((BaseClass == BASE_CLASS_CCID_STORAGE)
              && ((USB_DEV_CCID_COUNT == 0) || (DevNumber == USB_DEV_CCID_COUNT+1)))
        {
            USB_DEBUG(DEBUG_INFO, 3, "Reached the limit of supported CCID Devices (%d); skipping this device.\n", USB_DEV_CCID_COUNT);
            return TRUE;
        }
    }
    return FALSE;
}


/**

    @param 

    @retval 

**/

UINT8
UsbControlTransfer(
	HC_STRUC*   HcStruc,
    DEV_INFO*	DevInfo,
	DEV_REQ		DevReq,
	UINT16		Timeout,
	VOID*		Buffer)
{
	UINT16	Status;
	UINT16  SavedTimeout;

	SavedTimeout = gUsbData->wTimeOutValue;
	gUsbData->wTimeOutValue = Timeout;

    Status = (*gUsbData->aHCDriverTable[
				GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDControlTransfer)(
                HcStruc,
                DevInfo,
                DevReq.wRequestType,
                DevReq.wIndex,
                DevReq.wValue,
                Buffer,
                DevReq.wDataLength);

	gUsbData->wTimeOutValue = SavedTimeout;

	return DevReq.wDataLength && (Status == 0)? USB_ERROR : USB_SUCCESS;
}

/**
    This function executes an interrupt transaction on the USB.
    @param 
        HcStruc     Pointer to HCStruc of the host controller.
        DevInfo     DeviceInfo structure (if available else 0).
        EndpointAddress The destination USB device endpoint to which the device request 
                    is being sent.
        MaxPktSize  Indicates the maximum packet size the target endpoint is capable 
        of sending or receiving.
        Buffer      Buffer containing data to be sent to the device or buffer to be
        used to receive data.
        Length      Length request parameter, number of bytes of data to be transferred.
        Timeout     Indicates the maximum time, in milliseconds, which the transfer 
                    is allowed to complete.
    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
UsbInterruptTransfer (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       EndpointAddress,
    UINT16      MaxPktSize,
    VOID        *Buffer,
    UINT16      Length,
    UINT16		Timeout
)
{
    UINT16  SavedTimeout;
    UINT16  BytesTransferred;

    SavedTimeout = gUsbData->wTimeOutValue;
    gUsbData->wTimeOutValue = Timeout;

    BytesTransferred = (*gUsbData->aHCDriverTable[
                GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDInterruptTransfer)(
                HcStruc,
                DevInfo,
                EndpointAddress,
                MaxPktSize,
                Buffer,
                Length);

    gUsbData->wTimeOutValue = SavedTimeout;

    if (BytesTransferred == 0) {
        return USB_ERROR;
    } else {
        return USB_SUCCESS;
    }
    
}

/**
    Dummy HC API function used by the HC drivers that do not need
    to implement enable endpoint function.

**/

UINT8
USB_EnableEndpointsDummy (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Desc
)
{
    return USB_SUCCESS;
}

UINT8
USB_InitDeviceDataDummy (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT32      PortStatus,
    UINT8       **DeviceData
)
{
    *DeviceData = NULL;
    return USB_SUCCESS;
}

UINT8
USB_DeinitDeviceDataDummy (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo
)
{
    return USB_SUCCESS;
}

UINT8
UsbGetDataToggle(
	DEV_INFO	*DevInfo,
	UINT8		EndpointAddr
)
{
    DEV_INFO    *DevInfoToToggle;
    UINT8       ToggleBit = (EndpointAddr & 0xF) - 1;
    UINT16      *DataSync;
    EFI_STATUS  Status;

    if (DevInfo->fpLUN0DevInfoPtr) {
        Status = UsbDevInfoValidation(DevInfo->fpLUN0DevInfoPtr);
        if (EFI_ERROR(Status)) {
            return 0;
        }  
        DevInfoToToggle = DevInfo->fpLUN0DevInfoPtr;
    } else {
        DevInfoToToggle = DevInfo;
    }

    if (EndpointAddr & BIT7) {
        DataSync = &DevInfoToToggle->wDataInSync;
    } else {
        DataSync = &DevInfoToToggle->wDataOutSync;
    }

    return (UINT8)((*DataSync) >> ToggleBit) & 0x1;
}

VOID
UsbUpdateDataToggle(
	DEV_INFO	*DevInfo,
	UINT8		EndpointAddr,
	UINT8		DataToggle
)
{
    DEV_INFO    *DevInfoToToggle;
    UINT8       ToggleBit = (EndpointAddr & 0xF) - 1;
    UINT16      *DataSync;
    EFI_STATUS  Status;

    if (DevInfo->fpLUN0DevInfoPtr) {
        Status = UsbDevInfoValidation(DevInfo->fpLUN0DevInfoPtr);
        if (EFI_ERROR(Status)) {
            return;
        }  
        DevInfoToToggle = DevInfo->fpLUN0DevInfoPtr;
    } else {
        DevInfoToToggle = DevInfo;
    }

    if (EndpointAddr & BIT7) {
        DataSync = &DevInfoToToggle->wDataInSync;
    } else {
        DataSync = &DevInfoToToggle->wDataOutSync;
    }
    
    *DataSync &= (UINT16)~(1 << ToggleBit);
    *DataSync |= (UINT16)(DataToggle << ToggleBit);
    return;
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
