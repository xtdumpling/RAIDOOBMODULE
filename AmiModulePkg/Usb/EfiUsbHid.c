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

/** @file EfiUsbHid.c
    EFI USB HID device Driver

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "EfiUsbKb.h"
#include "UsbKbd.h"

#include "ComponentName.h"
#include "UsbBus.h"
#include <Protocol/AmiUsbHid.h>

extern  USB_GLOBAL_DATA *gUsbData; 

EFI_GUID gAmiHidProtocolGuid = AMI_USB_HID_PROTOCOL_GUID;

/**
    USB EFI keyboard driver driver protocol function that
    returns the keyboard controller name.

**/

CHAR16*
UsbHidGetControllerName(
    EFI_HANDLE Controller,
    EFI_HANDLE Child
)
{
    return NULL;
}
 

/**
    HID EFI driver entry point

**/

EFI_STATUS
UsbHidInit(
    EFI_HANDLE  ImageHandle,
    EFI_HANDLE  ServiceHandle
)
{
    //EFI_STATUS        Status;
    static NAME_SERVICE_T Names;
    static EFI_DRIVER_BINDING_PROTOCOL Binding = {
        SupportedUSBHid,
        InstallUSBHid,
        UninstallUSBHid,
        USBKB_DRIVER_VERSION,
        NULL,
        NULL };

    Binding.DriverBindingHandle = ServiceHandle;
    Binding.ImageHandle = ImageHandle;

    InitUSBMouse(); 

    return gBS->InstallMultipleProtocolInterfaces(
        &Binding.DriverBindingHandle,
        &gEfiDriverBindingProtocolGuid, &Binding,
        &gEfiComponentName2ProtocolGuid, InitNamesProtocol(&Names,	//(EIP69250)
              L"USB Hid driver", UsbHidGetControllerName),
        NULL);
}
 
/**
    Verifies if usb hid support can be installed on a device

**/

EFI_STATUS
EFIAPI
SupportedUSBHid(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath)
{
    EFI_USB_INTERFACE_DESCRIPTOR    Desc;
    EFI_STATUS                      Status;
    EFI_USB_IO_PROTOCOL             *UsbIo;
    DEV_INFO                        *DevInfo;

    Status = gBS->OpenProtocol(Controller, &gEfiUsbIoProtocolGuid,
        &UsbIo, This->DriverBindingHandle,
        Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle, Controller);

    DevInfo = FindDevStruc(Controller);

    if (DevInfo == NULL) {
        return EFI_UNSUPPORTED;
    }
    
    if (DevInfo->Flag & DEV_INFO_DEV_UNSUPPORTED) {
        return EFI_UNSUPPORTED;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor(UsbIo, &Desc);
    
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    if (Desc.InterfaceClass == BASE_CLASS_HID) {
        return EFI_SUCCESS;
    } else {
        return EFI_UNSUPPORTED;
    }
} 

/**
    Installs SimpleTxtIn protocol on a given handle

    @param Controller - controller handle to install protocol on.

    @retval VOID

**/

EFI_STATUS
EFIAPI
InstallUSBHid(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath
)
{
    EFI_STATUS                  Status;
    EFI_USB_IO_PROTOCOL         *UsbIo;
	USBDEV_T                    *HidDev;
	HC_STRUC                    *HcData;
	UINT8                       UsbStatus;
    AMI_USB_HID_PROTOCOL        *AmiUsbHidProtocol;
 
    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL,
        "USB: InstallUSBHid: starting...\n");
    //
    // Open Protocols
    //
    Status = gBS->OpenProtocol ( Controller,  &gEfiUsbIoProtocolGuid,
        &UsbIo, This->DriverBindingHandle,
        Controller, EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR(Status))
        return Status; 
	
	HidDev = UsbIo2Dev(UsbIo); 	
//Find DEV_INFO 
    ASSERT(HidDev);

    HcData = gUsbData->HcTable[HidDev->dev_info->bHCNumber - 1];
    UsbStatus = UsbSmiReConfigDevice(HcData, HidDev->dev_info); 
    if(UsbStatus != USB_SUCCESS) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_USBHC_LEVEL,
            "InstallUSBHid: failed to Reconfigure Hid: %d\n", UsbStatus );
		gBS->CloseProtocol(
			  Controller, &gEfiUsbIoProtocolGuid,
			  This->DriverBindingHandle, Controller);
        return EFI_DEVICE_ERROR;
    }

    if (HidDev->dev_info->HidDevType == 0) {
        HidDev->dev_info->Flag |= DEV_INFO_DEV_UNSUPPORTED;
        gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
                This->DriverBindingHandle, Controller);
        return EFI_UNSUPPORTED;
    }

    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_USB_HID_PROTOCOL),
                    &AmiUsbHidProtocol);

    if (EFI_ERROR(Status)) {
		gBS->CloseProtocol(
			  Controller, &gEfiUsbIoProtocolGuid,
			  This->DriverBindingHandle, Controller);
        return Status;
    }

    AmiUsbHidProtocol->HidDevType = HidDev->dev_info->HidDevType;

    Status = gBS->InstallMultipleProtocolInterfaces(&Controller,
                &gAmiHidProtocolGuid, AmiUsbHidProtocol, 
                NULL);
    
    if (HidDev->dev_info->HidDevType & HID_DEV_TYPE_KEYBOARD) {
	    InstallUsbKeyboard(This,Controller,DevicePath,HidDev->dev_info,UsbIo);
    }

    if (HidDev->dev_info->HidDevType & (HID_DEV_TYPE_MOUSE | HID_DEV_TYPE_POINT)) {
        if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) {
    		if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_RELATIVE_DATA) {
      			InstallUSBMouse(Controller, UsbIo, HidDev->dev_info);
    		}
#if USB_DEV_POINT 
    		if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_ABSOLUTE_DATA) {
    			InstallUSBAbsMouse(Controller, HidDev->dev_info);
    		}
#endif
        } else {
            InstallUSBMouse(Controller, UsbIo, HidDev->dev_info);
        }
    }
    return Status;

} 

/**
    Uninstalls protocol on a given handle

    @param Controller - controller handle.

    @retval VOID

**/

EFI_STATUS
EFIAPI
UninstallUSBHid(
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    UINTN                       NumberOfChildren,
    EFI_HANDLE                  *Children
)
{
	EFI_STATUS				    Status;
	EFI_USB_IO_PROTOCOL 	    *UsbIo;
	AMI_USB_HID_PROTOCOL        *AmiUsbHidProtocol;
	UINT8		                UsbSatus;
	USBDEV_T*	                HidDev;
	HC_STRUC*	                HcData;
 
	USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL,
		"\n USB: UnInstallUSBHid: stoping...\n");

	Status = gBS->OpenProtocol(Controller, &gAmiHidProtocolGuid,
		&AmiUsbHidProtocol, This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if (EFI_ERROR(Status)) {
		return Status; 
	}
	//
	// Open Protocols
	//
	Status = gBS->OpenProtocol(Controller, &gEfiUsbIoProtocolGuid,
		&UsbIo, This->DriverBindingHandle,
		Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL );
	if (EFI_ERROR(Status)) {
		return Status; 
	}

	HidDev = UsbIo2Dev(UsbIo);	
	HcData = gUsbData->HcTable[HidDev->dev_info->bHCNumber - 1];

	UsbSatus = UsbDevDriverDisconnect(HcData, HidDev->dev_info);
	ASSERT(UsbSatus == USB_SUCCESS);

	if (HidDev->dev_info->HidDevType & HID_DEV_TYPE_KEYBOARD) {
		Status = UninstallUsbKeyboard(This,Controller,NumberOfChildren,Children);
        if (EFI_ERROR(Status)) {
            return Status;
        }
	}

	if (HidDev->dev_info->HidDevType & (HID_DEV_TYPE_MOUSE | HID_DEV_TYPE_POINT)) {
        if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) {
    		if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_RELATIVE_DATA) {
    			Status = UninstallUSBMouse(This, Controller, NumberOfChildren, Children);
                if (EFI_ERROR(Status)) {
                    return Status;
                }
    		}
#if USB_DEV_POINT 
    		if (HidDev->dev_info->HidReport.Flag & HID_REPORT_FLAG_ABSOLUTE_DATA) {
    			Status = UninstallUSBAbsMouse(This, Controller, NumberOfChildren, Children);
                if (EFI_ERROR(Status)) {
                    return Status;
                }
    		}
#endif
        } else {
            Status = UninstallUSBMouse(This, Controller, NumberOfChildren, Children);
            if (EFI_ERROR(Status)) {
                return Status;
            }
        }
	}

	Status = gBS->UninstallMultipleProtocolInterfaces(Controller,
				&gAmiHidProtocolGuid, AmiUsbHidProtocol, 
				NULL);

	if (EFI_ERROR(Status)) {
		return Status;
	}
    
    gBS->FreePool(AmiUsbHidProtocol);
    
	//Close usbio protocol
    Status = gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
    			This->DriverBindingHandle, Controller); 

 	return Status;
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
