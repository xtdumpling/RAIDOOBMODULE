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

/** @file UsbHub.c
    AMI USB Hub support implementation

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"

extern  USB_GLOBAL_DATA *gUsbData;

VOID*   USB_MemAlloc(UINT16);
UINT8   USB_MemFree(void _FAR_*, UINT16);
DEV_INFO*   USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);
UINT8   USB_StopDevice (HC_STRUC*,  UINT8, UINT8);
VOID    FixedDelay(UINTN);
UINT8   USB_ProcessPortChange (HC_STRUC*, UINT8, UINT8, UINT8);
UINT8   USB_InstallCallBackFunction (CALLBACK_FUNC  pfnCallBackFunction);
UINT8   USBCheckPortChange (HC_STRUC*, UINT8, UINT8);
UINT8	UsbControlTransfer(HC_STRUC*, DEV_INFO*, DEV_REQ, UINT16, VOID*);

UINT8   USBHUBDisconnectDevice (DEV_INFO*);
UINT8   USBHub_EnablePort(HC_STRUC*, UINT8, UINT8);
UINT8   USBHub_DisablePort(HC_STRUC*, UINT8, UINT8);
UINT8   USBHub_ResetPort(HC_STRUC*, UINT8, UINT8);

VOID    UsbHubDeviceInit(VOID);
UINT8   USBHubCheckDeviceType (DEV_INFO*, UINT8, UINT8, UINT8);
UINT8   USBHub_ProcessHubData(HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);
DEV_INFO*   USBHUBConfigureDevice (HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT16);
UINT8	UsbHubResetPort(HC_STRUC*, DEV_INFO*, UINT8, BOOLEAN);

UINT8	UsbHubCearHubFeature(HC_STRUC*, DEV_INFO*, HUB_FEATURE);
UINT8	UsbHubClearPortFeature(HC_STRUC*, DEV_INFO*, UINT8, HUB_FEATURE);
UINT8	UsbHubGetHubDescriptor(HC_STRUC*, DEV_INFO*, VOID*, UINT16);
UINT8	UsbHubGetHubStatus(HC_STRUC*, DEV_INFO*, UINT32*);
UINT8	UsbHubGetPortStatus(HC_STRUC*, DEV_INFO*, UINT8, UINT32*);
UINT8	UsbHubGetErrorCount(HC_STRUC*, DEV_INFO*, UINT8, UINT16*);
UINT8	UsbHubSetHubDescriptor(HC_STRUC*, DEV_INFO*, VOID*, UINT16);
UINT8	UsbHubSetHubFeature(HC_STRUC*, DEV_INFO*, HUB_FEATURE);
UINT8	UsbHubSetHubDepth(HC_STRUC*, DEV_INFO*, UINT16);
UINT8	UsbHubSetPortFeature(HC_STRUC*, DEV_INFO*, UINT8, HUB_FEATURE);

PUBLIC
void
USBHubFillDriverEntries(
    DEV_DRIVER *DevDriver
)
{
    DevDriver->bDevType               = BIOS_DEV_TYPE_HUB;
//  DevDriver->bBaseClass             = BASE_CLASS_HUB;
//  DevDriver->bSubClass              = SUB_CLASS_HUB;
    DevDriver->bProtocol              = 0;
    DevDriver->pfnDeviceInit          = UsbHubDeviceInit;
    DevDriver->pfnCheckDeviceType     = USBHubCheckDeviceType;
    DevDriver->pfnConfigureDevice     = USBHUBConfigureDevice;
    DevDriver->pfnDisconnectDevice    = USBHUBDisconnectDevice;
}



/**
    This function checks an interface descriptor of a device
    to see if it describes a USB hub.  If the device is a hub,
    then it is configured and initialized.

    @param HcStruc    HcStruc pointer
        DevInfo    Device information structure pointer
        Desc       Pointer to the descriptor structure
        supported by the device
        Start      Start offset of the device descriptor
        End        End offset of the device descriptor

    @retval New device info structure, 0 on error

**/

DEV_INFO*
USBHUBConfigureDevice(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*      Desc,
    UINT16      Start,
    UINT16      End
)
{
    UINT8           PortNum;
    UINTN           DelayValue;
    UINT8*          Buffer;
    HUB_DESC        *HubDesc;
    UINT8           Status;
	DEV_INFO*		ParentHub;
	BOOLEAN			SetPortPower = FALSE;
    ENDP_DESC       *EndpDesc;
    INTRF_DESC      *IntrfDesc;
    INTRF_DESC      *AltIntrfDesc;
    UINT16          DescLength;
    UINT16          TotalLength;

	USB3_HUB_PORT_STATUS*	Usb3HubPortSts = (USB3_HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;	

    //
    // Set the BiosDeviceType field in DeviceTableEntry[0].  This serves as a flag
    // that indicates a usable interface has been found in the current
    // configuration. This is needed so we can check for other usable interfaces
    // in the current configuration (i.e. composite device), but not try to search
    // in other configurations.
    //
    DevInfo->bDeviceType = BIOS_DEV_TYPE_HUB;
    DevInfo->bCallBackIndex = USB_InstallCallBackFunction(USBHub_ProcessHubData);

    IntrfDesc = (INTRF_DESC*)(Desc + Start);

    // Check if the hub supports multiple TTs.
    if (DevInfo->Flag & DEV_INFO_ALT_SETTING_IF) {
        DescLength = Start;
        TotalLength = ((CNFG_DESC*)Desc)->wTotalLength;
        for (;DescLength < TotalLength;) {
            AltIntrfDesc = (INTRF_DESC*)(Desc + DescLength);
            if ((AltIntrfDesc->bDescLength == 0) || 
                ((AltIntrfDesc->bDescLength + DescLength) > TotalLength)) {
                break;
            }
            if ((AltIntrfDesc->bDescType == DESC_TYPE_INTERFACE) && (AltIntrfDesc->bAltSettingNum != 0)) {
                if ((AltIntrfDesc->bBaseClass == BASE_CLASS_HUB) && 
                    (AltIntrfDesc->bSubClass == SUB_CLASS_HUB) && 
                    (AltIntrfDesc->bProtocol == PROTOCOL_HUB_MULTIPLE_TTS)) {
                    DevInfo->bProtocol = AltIntrfDesc->bProtocol;
                    DevInfo->bAltSettingNum = AltIntrfDesc->bAltSettingNum;
                    IntrfDesc = AltIntrfDesc;
                    break;
                }
            }
            if (AltIntrfDesc->bDescLength) {
                DescLength += (UINT16)AltIntrfDesc->bDescLength;
            } else {
                break;
            }
        }
    }

    Desc+=((CNFG_DESC*)Desc)->wTotalLength; // Calculate the end of descriptor block
    EndpDesc = (ENDP_DESC*)((char*)IntrfDesc + IntrfDesc->bDescLength);

    for( ;(EndpDesc->bDescType != DESC_TYPE_INTERFACE) && ((UINT8*)EndpDesc < Desc);
        EndpDesc = (ENDP_DESC*)((UINT8 *)EndpDesc + EndpDesc->bDescLength)){

        if(!(EndpDesc->bDescLength)) {
            break;  // Br if 0 length desc (should never happen, but...)
        }

        if( EndpDesc->bDescType != DESC_TYPE_ENDPOINT ) {
            continue;
        }

        //
        // Check for and configure Interrupt endpoint if present
        //
        if ((EndpDesc->bEndpointFlags & EP_DESC_FLAG_TYPE_BITS) !=
                EP_DESC_FLAG_TYPE_INT) {    // Bit 1-0: 10 = Endpoint does interrupt transfers
			continue;
        }

		if (EndpDesc->bEndpointAddr & EP_DESC_ADDR_DIR_BIT) {
			DevInfo->IntInEndpoint = EndpDesc->bEndpointAddr;
			DevInfo->IntInMaxPkt = EndpDesc->wMaxPacketSize;
			DevInfo->bPollInterval = EndpDesc->bPollInterval;
			break;
		}
    }

    if ((HcStruc->dHCFlag & HC_STATE_CONTROLLER_WITH_RMH) &&
        (DevInfo->bHubDeviceNumber & BIT7)) {
        DevInfo->wIncompatFlags |= USB_INCMPT_RMH_DEVICE;
    }

	DevInfo->HubDepth = 0;
	ParentHub = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, 
						NULL, DevInfo->bHubDeviceNumber, NULL);
	if(ParentHub) {
		DevInfo->HubDepth = ParentHub->HubDepth + 1;
	}
	
	if(DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
		UsbHubSetHubDepth(HcStruc, DevInfo, DevInfo->HubDepth);
	}

    //
    // Allocate memory for getting hub descriptor
    //
    Buffer    = USB_MemAlloc(sizeof(MEM_BLK));
    if (!Buffer) {
		//USB_AbortConnectDev(fpDevInfo);   //(EIP59579-)
		return NULL;
    }

	Status = UsbHubGetHubDescriptor(HcStruc, DevInfo, Buffer, sizeof(MEM_BLK));
    if(Status != USB_SUCCESS) {    // Error
        USB_MemFree(Buffer, sizeof(MEM_BLK));
        //USB_AbortConnectDev(fpDevInfo);   //(EIP59579-)
		return NULL;
    }
    HubDesc                   = (HUB_DESC*)Buffer;
    DevInfo->bHubNumPorts     = HubDesc->bNumPorts;
    DevInfo->bHubPowerOnDelay = HubDesc->bPowerOnDelay; // Hub's ports have not been enumerated

    if (DevInfo->Flag & DEV_INFO_ALT_SETTING_IF) {
        if (DevInfo->bAltSettingNum != 0) {
            // Select this alternate setting for multiple TTs.
            UsbSetInterface(HcStruc, DevInfo, DevInfo->bAltSettingNum);
        }
    }

    //
    // Turn on power to all of the hub's ports by setting its port power features.
    // This is needed because hubs cannot detect a device attach until port power
    // is turned on.
    //
    for (PortNum = 1; PortNum <= DevInfo->bHubNumPorts; PortNum++)
    {
        if (DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE &&
            PortNum == HcStruc->DebugPort)
        {
            continue;
        }

		if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
			UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
			if (Usb3HubPortSts->PortStatus.Power == 1) {
				continue;
			}
		}

		UsbHubSetPortFeature(HcStruc, DevInfo, PortNum, PortPower);
		SetPortPower = TRUE;
    }

    //
    // Delay the amount of time specified in the PowerOnDelay field of
    // the hub descriptor: in ms, add 30 ms to the normal time and multiply
    // by 64 (in 15us).
    //
    if(SetPortPower) {
        if (!(DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE)) {
	        if (gUsbData->PowerGoodDeviceDelay == 0) {
	            DelayValue = (UINTN)DevInfo->bHubPowerOnDelay * 2 * 1000;  // "Auto"
	        } else {
	            DelayValue = (UINTN)gUsbData->PowerGoodDeviceDelay * 1000* 1000;  // convert sec->15 mcs units
	        }
	        FixedDelay(DelayValue);
        }
    }

    DevInfo->fpPollTDPtr  = 0;
    DevInfo->fpPollEDPtr  = 0;

    //
    // Free the allocated buffer
    //
    USB_MemFree(Buffer, sizeof(MEM_BLK));

    DevInfo->HubPortConnectMap = 0;

    //
    // Check for new devices behind the hub
    //
    for (PortNum = 1; PortNum <= DevInfo->bHubNumPorts; PortNum++) {
        USBCheckPortChange(HcStruc, DevInfo->bDeviceAddress, PortNum);
    }

	DevInfo->PollingLength = DevInfo->IntInMaxPkt;

    // Start polling the new device's interrupt endpoint.
    (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDActivatePolling)
                (HcStruc, DevInfo);

    return DevInfo;
}


/**
    This routine disconnects the hub by disconnecting all the
    devices behind it

    @param pDevInfo    Device info structure pointer

    @retval USB_SUCCESS

**/

UINT8
USBHUBDisconnectDevice(
    DEV_INFO*   DevInfo
)
{
    HC_STRUC* HcStruc = gUsbData->HcTable[DevInfo->bHCNumber - 1];
    UINT8 Port;

	// Stop polling the endpoint
	(*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDDeactivatePolling)(HcStruc,DevInfo);
	DevInfo->IntInEndpoint = 0;

    //
    // A hub device is being disconnected.  For each of the hub's ports disconnect
    // any child device connected.
    //
	HcStruc = gUsbData->HcTable[DevInfo->bHCNumber - 1];

    for (Port = 1; Port <= (UINT8)DevInfo->bHubNumPorts; Port++)
    {
        if (DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE &&
            Port == HcStruc->DebugPort)
        {
            continue;
        }

        USB_StopDevice (HcStruc,  DevInfo->bDeviceAddress, Port);
    }

    return USB_SUCCESS;

}

VOID
UsbHubDeviceInit(
    VOID
)
{
    USB_InstallCallBackFunction(USBHub_ProcessHubData);
    return;
}

/**
    This routine checks for hub type device from the
    interface data provided

    @param bBaseClass  USB base class code
        bSubClass   USB sub-class code
        bProtocol   USB protocol code

    @retval BIOS_DEV_TYPE_HUB type on success or 0FFH on error

**/

UINT8
USBHubCheckDeviceType(
    DEV_INFO    *DevInfo,
    UINT8       BaseClass,
    UINT8       SubClass,
    UINT8       Protocol
)
{
    if (BaseClass == BASE_CLASS_HUB) {
        return BIOS_DEV_TYPE_HUB;
    } else {
        return USB_ERROR;
    }
}


/**
    This routine returns the hub port status

    @param fpDevInfo   USB device - the hub whose status has changed
        bit 7   : 1 - Root hub, 0 for other hubs
        bit 6-0 : Device address of the hub
        bPortNum    Port number
        pHCStruc    HCStruc of the host controller

    @retval Port status flags (Refer USB_PORT_STAT_XX equates)

**/

UINT32
USBHub_GetPortStatus(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum,
    BOOLEAN     ClearChangeBits
)
{
    UINT32      PortSts = USB_PORT_STAT_DEV_OWNER;
    UINT8		Status;
    DEV_INFO    *DevInfo;
    HUB_FEATURE	Feature;
    UINT16		PortChange;
    UINT8		i = 0;

    HUB_PORT_STATUS*		HubPortSts = (HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;
    USB3_HUB_PORT_STATUS*	Usb3HubPortSts = (USB3_HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;	

    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, NULL, HubAddr, HcStruc);
    ASSERT(DevInfo);
    if (DevInfo == NULL) {
        return 0;
    }

    if (DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE &&
        PortNum == HcStruc->DebugPort){
        return 0;
    }

    Status = UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
    if (Status == USB_ERROR) {
        return USB_ERROR;
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);

    if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
        for (i = 0; i < 20; i++) {
            if (Usb3HubPortSts->PortStatus.Reset == 0) {
                break;
            }
            FixedDelay(10 * 1000);	   // 10ms
            UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
        }
        
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);

        switch (Usb3HubPortSts->PortStatus.LinkState) {
            case USB3_HUB_PORT_LINK_U0:
            case USB3_HUB_PORT_LINK_RXDETECT:
                break;
            case USB3_HUB_PORT_LINK_U3:
                PortSts |= USB_PORT_STAT_DEV_SUSPEND;
                break;
            case USB3_HUB_PORT_LINK_RECOVERY:
                for (i = 0; i < 20; i++) {
                    FixedDelay(10 * 1000);
                    UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
                    if (Usb3HubPortSts->PortStatus.LinkState != USB3_HUB_PORT_LINK_RECOVERY) {
                        break;
                    }
                }
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);
                break;
            case USB3_HUB_PORT_LINK_POLLING:
                for (i = 0; i < 50; i++) {
                    FixedDelay(10 * 1000);
                    UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
                    if (Usb3HubPortSts->PortStatus.LinkState != USB3_HUB_PORT_LINK_POLLING) {
                        break;
                    }
                }
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);
                if (Usb3HubPortSts->PortStatus.LinkState == USB3_HUB_PORT_LINK_U0 || 
                    Usb3HubPortSts->PortStatus.LinkState == USB3_HUB_PORT_LINK_RXDETECT) {
                    break;
                }
            case USB3_HUB_PORT_LINK_INACTIVE:
                // A downstream port can only exit from this state when directed, 
                // or upon detection of an absence of a far-end receiver termination 
                // or upon a Warm Reset.
                // The Timeout of SS.Inactive.Quiet is 12 ms.
                FixedDelay(12 * 1000);
                UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);
                if (Usb3HubPortSts->PortStatus.LinkState == USB3_HUB_PORT_LINK_RXDETECT) {
                    break;
                }
            case USB3_HUB_PORT_LINK_COMPLIANCE_MODE:
                UsbHubResetPort(HcStruc, DevInfo, PortNum, TRUE);
                UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Hub port[%d] status: %08x\n", PortNum, gUsbData->dHubPortStatus);
                break;
            default:
                PortSts |= USB_PORT_STAT_DEV_CONNECTED;
                break;
        }
        if (Usb3HubPortSts->PortChange.ConnectChange) {
            PortSts |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
        }
        if (Usb3HubPortSts->PortStatus.Connected) {
            if (!(DevInfo->HubPortConnectMap & (UINT16) (1 << PortNum))) {
                PortSts |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
                DevInfo->HubPortConnectMap |= (UINT16) (1 << PortNum);
            }
            PortSts |= USB_PORT_STAT_DEV_CONNECTED | USB_PORT_STAT_DEV_SUPERSPEED;

			// USB 3.0 hub may not set Connect Status Change bit,
			// set the connect change flag if the BH Reset change or Reset change is set.
            if (Usb3HubPortSts->PortChange.BhResetChange || Usb3HubPortSts->PortChange.ResetChange) {
                PortSts |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
                PortSts |= USB_PORT_STAT_DEV_RESET_CHANGED;
            }
            if (Usb3HubPortSts->PortStatus.Enabled) {
                PortSts |= USB_PORT_STAT_DEV_ENABLED;
            }
        } else {
            DevInfo->HubPortConnectMap &= (UINT16) (~(1 << PortNum));
        }
        if (Usb3HubPortSts->PortChange.OverCurrentChange) {
            PortSts |= USB_PORT_STAT_DEV_OVERCURRENT_CHANGED;
            if ((Usb3HubPortSts->PortStatus.OverCurrent == 0) && 
                (Usb3HubPortSts->PortStatus.Power == 0)) {
                UsbHubSetPortFeature(HcStruc, DevInfo, PortNum, PortPower);
                FixedDelay((UINTN)(DevInfo->bHubPowerOnDelay * 2 * 1000));
            }
        }
        if (Usb3HubPortSts->PortStatus.OverCurrent) {
            PortSts |= USB_PORT_STAT_DEV_OVERCURRENT;
        }
        if (Usb3HubPortSts->PortStatus.Reset) {
            PortSts |= USB_PORT_STAT_DEV_RESET;
        }
        if (Usb3HubPortSts->PortStatus.Power) {
            PortSts |= USB_PORT_STAT_DEV_POWER;
        }
	} else {
	    if (HubPortSts->PortChange.ConnectChange) {
	        PortSts |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
	    }
	    if (HubPortSts->PortStatus.Connected) {
            if (!(DevInfo->HubPortConnectMap & (UINT16) (1 << PortNum))) {
                PortSts |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
                DevInfo->HubPortConnectMap |= (UINT16) (1 << PortNum);
            }
            PortSts |= USB_PORT_STAT_DEV_CONNECTED;
            if (HubPortSts->PortStatus.LowSpeed) {
                PortSts |= USB_PORT_STAT_DEV_LOWSPEED;
            } else if (HubPortSts->PortStatus.HighSpeed) {
                PortSts |= USB_PORT_STAT_DEV_HISPEED;
            } else {
                PortSts |= USB_PORT_STAT_DEV_FULLSPEED;
            }
            if (HubPortSts->PortStatus.Enabled) {
                PortSts |= USB_PORT_STAT_DEV_ENABLED;
            }
	    } else {
	        DevInfo->HubPortConnectMap &= (UINT16) (~(1 << PortNum));
        }
	    if (HubPortSts->PortStatus.Suspend) {
            PortSts |= USB_PORT_STAT_DEV_SUSPEND;
        }
        if (HubPortSts->PortStatus.OverCurrent) {
            PortSts |= USB_PORT_STAT_DEV_OVERCURRENT;
        }
        if (HubPortSts->PortStatus.Reset) {
            PortSts |= USB_PORT_STAT_DEV_RESET;
        }
        if (HubPortSts->PortStatus.Power) {
            PortSts |= USB_PORT_STAT_DEV_POWER;
        }
        if (HubPortSts->PortChange.OverCurrentChange) {
            if ((HubPortSts->PortStatus.OverCurrent == 0) && 
                (HubPortSts->PortStatus.Power == 0)) {
                UsbHubSetPortFeature(HcStruc, DevInfo, PortNum, PortPower);
                FixedDelay((UINTN)(DevInfo->bHubPowerOnDelay * 2 * 1000));
            }
        }
        if (HubPortSts->PortChange.EnableChange) {
            PortSts |= USB_PORT_STAT_DEV_ENABLE_CHANGED;
        }
        if (HubPortSts->PortChange.SuspendChange) {
            PortSts |= USB_PORT_STAT_DEV_SUSPEND_CHANGED;
        }
        if (HubPortSts->PortChange.ResetChange) {
            PortSts |= USB_PORT_STAT_DEV_RESET_CHANGED;
        }
	}

	// Clear all port status change
	//UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);
    if (ClearChangeBits == TRUE) {
    	PortChange = (*((UINT16*)&HubPortSts->PortChange));
    	for (Feature = PortConnectChange; Feature <= PortResetChange; Feature++) {
    	    if (PortChange & 1) {
    	        UsbHubClearPortFeature(HcStruc, DevInfo, PortNum, Feature);
    	    }
    	    PortChange >>= 1;
    	}

    	if (DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
    	    if (Usb3HubPortSts->PortChange.LinkStateChange) {
    	        UsbHubClearPortFeature(HcStruc, DevInfo, PortNum, PortLinkStateChange);
    	    }
    	    if (Usb3HubPortSts->PortChange.ConfigErrorChange) {
    	        UsbHubClearPortFeature(HcStruc, DevInfo, PortNum, PortConfigErrorChange);
    	    }
    	    if (Usb3HubPortSts->PortChange.BhResetChange) {
    	        UsbHubClearPortFeature(HcStruc, DevInfo, PortNum, BhPortResetChange);
    	    }
    	}
    }

    return PortSts;
}


/**
    This routine disables the hub port

    @param bHubAddr    USB device address of the hub whose status
        has changed
        bit 7   : 1 - Root hub, 0 for other hubs
        bit 6-0 : Device address of the hub
        bPortNum    Port number
        pHCStruc    HCStruc of the host controller

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
USBHub_DisablePort(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum
)
{
    DEV_INFO*   DevInfo;

	HUB_PORT_STATUS*	HubPortSts = (HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;
	
    //
    // Get DeviceInfo pointer
    //
    DevInfo   = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR,
                            (DEV_INFO*)0,
                            HubAddr,
                            HcStruc);
//
// Disable the hub/port by clearing its Enable feature
//
    if (DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE &&
            PortNum == HcStruc->DebugPort)
    {
        return USB_SUCCESS;
    }

	if(DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) return USB_SUCCESS;

	UsbHubGetPortStatus(HcStruc, DevInfo, PortNum, &gUsbData->dHubPortStatus);

    // Perform control transfer with device request as HUB_RQ_CLEAR_PORT_FEATURE,
    // wIndex = Port number, wValue = HUB_FEATURE_PORT_ENABLE,
    // fpBuffer = 0 and wlength = 0
    //
    if(HubPortSts->PortStatus.Enabled) {
	    UsbHubClearPortFeature(HcStruc, DevInfo, PortNum, PortEnable);
    }

    return USB_SUCCESS;

}


/**
    This routine enables the hub port

    @param bHubAddr    USB device address of the hub whose status
        has changed
        bit 7   : 1 - Root hub, 0 for other hubs
        bit 6-0 : Device address of the hub
        bPortNum    Port number
        pHCStruc    HCStruc of the host controller

    @retval USB_SUCCESS if the hub port is enabled. USB_ERROR otherwise

**/

UINT8
USBHub_EnablePort(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum
)
{
    return USB_SUCCESS;
}

/**
    This routine resets the hub port

    @param HCStruc    HCStruc of the host controller
        HubAddr    USB device address of the hub whose status
        has changed
        bit 7   : 1 - Root hub, 0 for other hubs
        bit 6-0 : Device address of the hub
        PortNum    Port number

    @retval USB_SUCCESS if the hub port is enabled. USB_ERROR otherwise

**/

UINT8
USBHub_ResetPort(
    HC_STRUC*   HcStruc,
    UINT8       HubAddr,
    UINT8       PortNum)
{
	UINT8		Status;
    DEV_INFO*   DevInfo;

    //
    // Get DeviceInfo pointer
    //
    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, 
    						(DEV_INFO*)0, HubAddr, HcStruc);
    if (DevInfo == NULL) return USB_ERROR;

    if ((DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE) && 
		(PortNum == HcStruc->DebugPort)) {
        return USB_SUCCESS;
    }
	Status = UsbHubResetPort(HcStruc, DevInfo, PortNum, FALSE);

    return Status;
}


/**
    This routine is called with USB hub status change
    report data

    @param pHCStruc    Pointer to HCStruc
        pDevInfo    Pointer to device information structure
        pTD         Pointer to the polling TD
        pBuffer     Pointer to the data buffer


    @note  The status change data is an array of bit flags:
          Bit     Description
      ----------------------------------------------------------
          0   Indicate connect change status for all ports
          1   Indicate connect change status for port 1
          2   Indicate connect change status for port 2
          ...     ..............
          n   Indicate connect change status for port n
      -----------------------------------------------------------

**/

UINT8
USBHub_ProcessHubData (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Td,
    UINT8       *Buffer,
    UINT16      DataLength
 )
{
    UINT8       PortNum;
    UINT16      PortMap;
    BOOLEAN     ConnectDelay = FALSE;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBHub_ProcessHubData, gUsbData->bEnumFlag = %d\n", gUsbData->bEnumFlag);
    //
    // Check for enum flag and avoid hub port enumeration if needed
    //
    if (gUsbData->bEnumFlag == TRUE) return USB_SUCCESS;
    
    for (PortNum = 1; PortNum <= DevInfo->bHubNumPorts; PortNum++) {
        PortMap = (UINT16)(1 << PortNum);
        if (*(UINT16*)Buffer & PortMap) {
            if (!ConnectDelay && ((~DevInfo->HubPortConnectMap) & PortMap)) {
                // The interval with a minimum duration of 100 ms when 
                // the USB System Software is notified of a connection detection.
                FixedDelay(USB_PORT_CONNECT_STABLE_DELAY_MS * 1000);
                ConnectDelay = TRUE;
            }
            //
            // Set enumeration flag so that another device will not get enabled
            //
            gUsbData->bEnumFlag = TRUE;

            USBCheckPortChange(HcStruc, DevInfo->bDeviceAddress, PortNum);

            //
            // Reset enumeration flag so that other devices can be enumerated
            //
            gUsbData->bEnumFlag = FALSE;
        }
    }
	return USB_SUCCESS;
}


/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubResetPort(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8		Port,
    BOOLEAN		WarmReset
)
{
    UINT8       Status;
    UINT32      i;
    BOOLEAN     IsResetChange;
    HUB_PORT_STATUS*    HubPortSts = (HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;
    USB3_HUB_PORT_STATUS*   Usb3HubPortSts = (USB3_HUB_PORT_STATUS*)&gUsbData->dHubPortStatus;	

    if (WarmReset && DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
        Status = UsbHubSetPortFeature(HcStruc, DevInfo, Port, BhPortReset);
        if (Status != USB_SUCCESS) {
            return USB_ERROR;
        }
		
		for (i = 0; i < (USB_HUB_WARM_RESET_PORT_TIMEOUT_MS / 10); i++) {
			FixedDelay(10 * 1000);
			Status = UsbHubGetPortStatus(HcStruc, DevInfo, Port, &gUsbData->dHubPortStatus);
            if (Status != USB_SUCCESS) {
                return USB_ERROR;
            }
            if (Usb3HubPortSts->PortChange.BhResetChange) {
                break;
            }
		}
		if (!Usb3HubPortSts->PortChange.BhResetChange) {
            USB_DEBUG(DEBUG_ERROR, 3, "Usb super speed hub port[%d] status: %08x: warm reset port is failed\n",
                Port, gUsbData->dHubPortStatus);
			return USB_ERROR;
		}
        Status = UsbHubClearPortFeature(HcStruc, DevInfo, Port, BhPortResetChange);
        if (Status != USB_SUCCESS) {
            return USB_ERROR;
        }
		
        Status = UsbHubClearPortFeature(HcStruc, DevInfo, Port, PortResetChange);
        if (Status != USB_SUCCESS) {
            return USB_ERROR;
        }

	} else {
		Status = UsbHubSetPortFeature(HcStruc, DevInfo, Port, PortReset);
		if(Status != USB_SUCCESS) return USB_ERROR;

        // The duration of the Resetting state is nominally 10 ms to 20 ms
        FixedDelay(20 * 1000);      // 20 ms delay

		for(i = 0; i < 10; i++) {
			Status = UsbHubGetPortStatus(HcStruc, DevInfo, Port, &gUsbData->dHubPortStatus);
			if(Status != USB_SUCCESS) return USB_ERROR;

			if(DevInfo->bEndpointSpeed == USB_DEV_SPEED_SUPER) {
				IsResetChange = Usb3HubPortSts->PortChange.ResetChange ? TRUE : FALSE;
			} else {
				IsResetChange = HubPortSts->PortChange.ResetChange ? TRUE : FALSE;
			}

			if(IsResetChange) break;

            FixedDelay(5 * 1000);      // 5 ms delay
		}
		if (!IsResetChange) {
			return USB_ERROR;
		}

		Status = UsbHubClearPortFeature(HcStruc, DevInfo, Port, PortResetChange);
		if(Status != USB_SUCCESS) return USB_ERROR;

		if (DevInfo->bEndpointSpeed != USB_DEV_SPEED_SUPER) {
			if (!(DevInfo->wIncompatFlags & USB_INCMPT_RMH_DEVICE)) {
				FixedDelay(20 * 1000);	   // 20 ms
			} else if (HubPortSts->PortStatus.LowSpeed == 1) {
				// 1 ms delay for Low-Speed device
				FixedDelay(1 * 1000);
			}
		}
	}
	return USB_SUCCESS;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubCearHubFeature(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    HUB_FEATURE	HubFeature)
{
	return USB_ERROR;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubClearPortFeature(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8		Port,
    HUB_FEATURE	PortFeature)
{
	DEV_REQ	DevReq;

	DevReq.wRequestType = HUB_RQ_CLEAR_PORT_FEATURE;
	DevReq.wValue = PortFeature;
	DevReq.wIndex = Port;
	DevReq.wDataLength = 0;

	return UsbControlTransfer(HcStruc, DevInfo, DevReq, 50, NULL);
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubGetHubDescriptor(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    VOID*		Buffer,
    UINT16		Length)
{
	DEV_REQ	DevReq;

	DevReq.wRequestType = USB_RQ_GET_CLASS_DESCRIPTOR;
	DevReq.wValue = DevInfo->bEndpointSpeed == 
						USB_DEV_SPEED_SUPER ? DESC_TYPE_SS_HUB << 8 : DESC_TYPE_HUB << 8;
	DevReq.wIndex = 0;
	DevReq.wDataLength = Length;

	return UsbControlTransfer(HcStruc, DevInfo, DevReq, 150, Buffer);
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubGetHubStatus(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT32*		HubStatus)
{
	return USB_ERROR;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubGetPortStatus(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
	UINT8		Port,
    UINT32*		PortStatus)
{
	DEV_REQ	DevReq;

	DevReq.wRequestType = HUB_RQ_GET_PORT_STATUS;
	DevReq.wValue = 0;
	DevReq.wIndex = Port;
	DevReq.wDataLength = 4;

	return UsbControlTransfer(HcStruc, DevInfo, DevReq, 150, PortStatus);
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubGetErrorCount(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8		Port,
    UINT16*		ErrorCount)
{
	return USB_ERROR;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubSetHubDescriptor(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
	VOID*		Buffer,
    UINT16		Length)
{
	return USB_ERROR;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubSetHubFeature(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    HUB_FEATURE	HubFeature)
{
	return USB_ERROR;
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubSetHubDepth(
	HC_STRUC*	HcStruc,
	DEV_INFO*	DevInfo,
	UINT16		HubDepth)
{
	DEV_REQ	DevReq;

	DevReq.wRequestType = HUB_RQ_SET_HUB_DEPTH;
	DevReq.wValue = HubDepth;
	DevReq.wIndex = 0;
	DevReq.wDataLength = 0;

	return UsbControlTransfer(HcStruc, DevInfo, DevReq, 50, NULL);
}

/**

    @param 

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
UsbHubSetPortFeature(
	HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8		Port,
    HUB_FEATURE	PortFeature)
{
	DEV_REQ	DevReq;

	DevReq.wRequestType = HUB_RQ_SET_PORT_FEATURE;
	DevReq.wValue = PortFeature;
	DevReq.wIndex = Port;
	DevReq.wDataLength = 0;

	return UsbControlTransfer(HcStruc, DevInfo, DevReq, 100, NULL);		//(EIP77526)		 
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
