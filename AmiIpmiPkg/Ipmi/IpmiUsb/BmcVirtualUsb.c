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

/** @file BmcVirtualUsb.c
    This file provides Power Consumption Support feature in BMC
    over USB interface (enable or disable BMC USB interface through KCS 
    using functions provoded in this file).
    
**/

//-------------------------------------------------------------------------

#include "DxeIpmiUsb.h"

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

//NetFn
#define EFI_APP_ENABLE_MESSAGE_CHANNEL_RECEIVE  0x32

// Virtual USB devices State
#define     VIRTUAL_USB_DEVICES_ENABLED     0x00
#define     VIRTUAL_USB_DEVICES_DISABLED    0x01
#define     COMMAND_SUCCESS                 0x01

// LUN definition
#ifndef BMC_LUN
    #define BMC_LUN     0x00
#endif

#if VIRTUAL_BMC_USB_INTERFACE
    EFI_EVENT           gLegacyBootEvent;
    EFI_EVENT           gExitBootServicesEvent;
#endif

//Command for setting
#define     BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND       0xAA

// Command to Get
#define     BMC_OEM_COM_FOR_POWER_SAVING_RECEIVE_COMMAND    0xAB

UINT8       gUsbEnableCommandData = 0;    // 0- Enabling BMC USB Interface
UINT8       gUsbDisableCommandData = 1;   // 1- Disabling BMC USB Interface
UINT8       gResponseData;

EFI_IPMI_TRANSPORT      *gIpmiTransport = NULL;

extern  BMC_USB_ENABLE_AND_DISABLE   BMC_USB_HOOK_LIST  EndOfBmcUsbInterfaceHookListFunctions;
BMC_USB_ENABLE_AND_DISABLE  *BmcUsbInterfaceHookList[]  =   {BMC_USB_HOOK_LIST NULL};

//-------------------------------------------------------------------------

/**
    This routine calls the Bmc Usb Enable related functions.

    @param  Event Callback event
    @param  Context Pointer to calling context

    @retval VOID
**/

VOID
EFIAPI
BmcUsbInterfaceHook (
  IN    EFI_EVENT Event,
  IN    VOID      *Context
  )
{
    UINTN   Index;

    for (Index = 0; BmcUsbInterfaceHookList[Index]; Index++) {
        BmcUsbInterfaceHookList[Index]();
    }
}

/**
    This function checks the status of BMC Virtual USB devices.
        00h = Virtual USB devices enabled
        01h = Virtual USB devices disabled

    @param  VirtualUsbDevicesState Address of Virtual Usb Device State to get the Status
    @return EFI_STATUS Status
**/

EFI_STATUS
BmcUsbStatus (
  UINT8  *VirtualUsbDevicesState
  )
{
    // Checking USB Enabled or not
    EFI_STATUS      Status;
    UINT8           ResponseDataSize;

    ResponseDataSize = sizeof(gResponseData);
    Status = gIpmiTransport->SendIpmiCommand (
                               gIpmiTransport,
                               EFI_APP_ENABLE_MESSAGE_CHANNEL_RECEIVE,
                               BMC_LUN,
                               BMC_OEM_COM_FOR_POWER_SAVING_RECEIVE_COMMAND,
                               NULL,
                               0,
                               &gResponseData,
                               &ResponseDataSize
                               );

    SERVER_IPMI_DEBUG((EFI_D_INFO, "Send Ipmi Command in USB Status Function :: Status - %r and Response data :: %x \n", Status, gResponseData));

    if (!EFI_ERROR(Status)) {  // Returning response data
        if (gResponseData == VIRTUAL_USB_DEVICES_ENABLED || gResponseData == VIRTUAL_USB_DEVICES_DISABLED) {
            *VirtualUsbDevicesState = gResponseData;
            return EFI_SUCCESS;
        }
    }

    //other than response code
    return EFI_DEVICE_ERROR;
}

/**
    This function disables BMC Virtual USB devices if not disabled,
    else exits.

    @param  Event Callback event
    @param  Context Pointer to calling context

    @retval VOID
**/

VOID
EFIAPI
DisableUsbInterface (
  IN    EFI_EVENT Event,
  IN    VOID      *Context
  )
{
    EFI_STATUS      Status;
    UINT8           ResponseDataSize;

    Status = gBS->LocateProtocol (
                    &gEfiDxeIpmiTransportProtocolGuid,
                    NULL,
                    &gIpmiTransport
                    );

    SERVER_IPMI_DEBUG((EFI_D_INFO, "LocateProtocol In Disable Function :: Status - %r\n", Status));

    if (Status == EFI_SUCCESS) {
        ResponseDataSize = sizeof(gResponseData);
        Status = gIpmiTransport->SendIpmiCommand (
                                   gIpmiTransport,
                                   EFI_APP_ENABLE_MESSAGE_CHANNEL_RECEIVE,
                                   BMC_LUN,
                                   BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND,
                                   &gUsbDisableCommandData,
                                   1,
                                   &gResponseData,
                                   &ResponseDataSize
                                   );

        SERVER_IPMI_DEBUG((EFI_D_INFO, "SendIpmiCommand  in Disable Function :: Status- %r and ResponseData :: %x \n", Status, gResponseData));

        if (Status == EFI_SUCCESS) {
            if (gResponseData == COMMAND_SUCCESS)
                SERVER_IPMI_DEBUG((EFI_D_INFO, "Virtual USB devices are Disabled Using Send Ipmi Command\n"));
            else
                SERVER_IPMI_DEBUG((EFI_D_ERROR, "Virtual USB devices Disabling FAILED Using Send Ipmi Command\n"));
        }
    }
}

/**
    This function enables BMC Virtual USB devices if not enabled,
    else exits.

    @param  VOID
    @return EFI_STATUS Return Status
**/

EFI_STATUS
BmcUsbInterface (
  VOID
  )
{
    EFI_STATUS      Status;
    UINT8           VirtualUsbDevicesState;
    UINT8           ResponseDataSize;

    Status = gBS->LocateProtocol (
                    &gEfiDxeIpmiTransportProtocolGuid,
                    NULL,
                    &gIpmiTransport
                    );

    SERVER_IPMI_DEBUG((EFI_D_INFO, "LocateProtocol In Enable Function :: Status - %r\n", Status));

    if (Status == EFI_SUCCESS) {
        // Checking BMC USB Status and getting the Response Data 
        Status = BmcUsbStatus (&VirtualUsbDevicesState);

        if (Status == EFI_SUCCESS) {
            if (VirtualUsbDevicesState == VIRTUAL_USB_DEVICES_DISABLED) { // Virtual USB devices Disabled
                ResponseDataSize = sizeof(gResponseData);
                Status = gIpmiTransport->SendIpmiCommand (
                                        gIpmiTransport,
                                        EFI_APP_ENABLE_MESSAGE_CHANNEL_RECEIVE,
                                        BMC_LUN,
                                        BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND,
                                        &gUsbEnableCommandData,
                                        1,
                                        &gResponseData,
                                        &ResponseDataSize
                                        );

                SERVER_IPMI_DEBUG((EFI_D_INFO, "SendIpmiCommand  in Enable Function :: Status- %r  and ResponseData :: %x\n", Status, gResponseData));

                if (Status == EFI_SUCCESS) {
                    if (gResponseData == COMMAND_SUCCESS) {
                        SERVER_IPMI_DEBUG((EFI_D_INFO, "Virtual USB devices are enabled Using Send Ipmi Command\n"));
                        // To disable BMC Virtual USB devices at exit boot.
                        Status = gBS->CreateEvent (
                                EVT_SIGNAL_EXIT_BOOT_SERVICES,
                                TPL_NOTIFY,
                                DisableUsbInterface,
                                NULL,
                                &gExitBootServicesEvent
                                );
                        ASSERT_EFI_ERROR (Status);

                        Status = EfiCreateEventLegacyBootEx (
                                TPL_NOTIFY,
                                DisableUsbInterface,
                                NULL,
                                &gLegacyBootEvent
                                );
                        ASSERT_EFI_ERROR (Status);
                    } else {
                          SERVER_IPMI_DEBUG((EFI_D_ERROR, "Virtual USB devices enabling FAILED Using Send Ipmi Command\n"));
                      }
                } else if (VirtualUsbDevicesState == VIRTUAL_USB_DEVICES_ENABLED) {
                      SERVER_IPMI_DEBUG((EFI_D_INFO, "Virtual USB devices already enabled \n"));
                }
            }
        }
    }
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
