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

/** @file DxeIpmiUsb.c
    This file produces a protocol to send IPMI commands
    over USB interface

**/

//----------------------------------------------------------------------

#include "DxeIpmiUsb.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------

EFI_USB_IO_PROTOCOL             *gUsbIo = NULL;
VOID                            *gIpmiUsbRegistration;
VOID                            *gDxeIpmiTransportRegistration;
EFI_IPMI_USB_INSTANCE_DATA      *gIpmiUsbInstance = NULL;
EFI_HANDLE                       gUsbIoHandle = NULL;
EFI_IPMI_TRANSPORT              *gIpmiTransportHandle=NULL;

//----------------------------------------------------------------------

/**
    If following conditions are met, returns TRUE else FALSE.
    1. Checks for USB IO protocol in given handle matching registration value
    2. Checks for Vendor ID and Product ID of BMC USB Interface.

    @param VOID

    @retval TRUE BMC USB Interface is found.
    @retval FALSE BMC USB Interface is not found.

**/

BOOLEAN
IsBmcUsbInterface (
  VOID
  )
{
    EFI_STATUS                  Status;
    EFI_USB_DEVICE_DESCRIPTOR   DevDesc;
    EFI_HANDLE                  Handle;
    UINTN                       HandleSize = sizeof(EFI_HANDLE);

    SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));

    //
    // Get list of available USB I/O devices
    //
    Status = gBS->LocateHandle (
                ByRegisterNotify,
                NULL,
                gIpmiUsbRegistration,
                &HandleSize,
                &Handle
                );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "LocateHandle.... Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        return FALSE;
    }

    Status = gBS->HandleProtocol (
                Handle,
                &gEfiUsbIoProtocolGuid,
                (VOID **)&gUsbIo
                );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "HandleProtocol gEfiUsbIoProtocolGuid .... Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        return FALSE;
    }

    //
    // GetDeviceDescriptor API is not available in UsbIoPpi.
    // So Get Device descriptor details using gUsbIo->ControlTransfer API.
    // Fill the Device request structure.

    //
    // Issue ControlTranfer transaction
    //
    Status = gUsbIo->UsbGetDeviceDescriptor (
                gUsbIo,
                &DevDesc
                );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "UsbGetDeviceDescriptor .... Status: %r \n", Status));

    //
    // Return FALSE, For Error case
    //
    if (EFI_ERROR(Status)) {
        return FALSE;
    }

    SERVER_IPMI_DEBUG((EFI_D_INFO, "DevDesc.IdVendor: %X \n", DevDesc.IdVendor));
    SERVER_IPMI_DEBUG((EFI_D_INFO, "DevDesc.IdProduct: %X \n", DevDesc.IdProduct));

    //
    // Check for Vendor ID and Product ID
    // Return TRUE, If Vendor ID and Product ID matches
    // with BMC USB Interface. Else, return FALSE.
    //
    if ((DevDesc.IdVendor == BMC_USB_INTERFACE_VENDOR_ID) &&\
         (DevDesc.IdProduct == BMC_USB_INTERFACE_PRODUCT_ID)) {
         SERVER_IPMI_DEBUG((EFI_D_INFO, "BMC USB interface is found.\n"));
         //
         //  Saving handle for further use
         //
         gUsbIoHandle = Handle;
         return TRUE;
    } else {
        return FALSE;
    }
}

/**
    GetBulkEndPoints for USB.

    @param VOID

    @return EFI_STATUS

**/

EFI_STATUS
GetBulkEndPoints (
  VOID
  )
{
    EFI_STATUS                      Status;
    UINT8                           EndPoint = 0;
    EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;
    EFI_USB_ENDPOINT_DESCRIPTOR     EndpointDescriptor;
    BOOLEAN                         BulkInEndpointFound = FALSE;
    BOOLEAN                         BulkOutEndpointFound = FALSE;

    SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));

    Status = gUsbIo->UsbGetInterfaceDescriptor (
                gUsbIo,
                &InterfaceDescriptor
                );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "UsbGetInterfaceDescriptor: Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    SERVER_IPMI_DEBUG((EFI_D_INFO, "InterfaceDescriptor.NumEndpoints: %X \n", InterfaceDescriptor.NumEndpoints));

    //
    // Get Bulk-in/Bulk-out end point address
    //
    do {
        Status = gUsbIo->UsbGetEndpointDescriptor (
                    gUsbIo,
                    EndPoint,
                    &EndpointDescriptor
                    );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "UsbGetInterfaceDescriptor: Status: %r EndPoint: %x \n", Status, EndPoint));
        if (EFI_ERROR(Status)) {
            return Status;
        }
        SERVER_IPMI_DEBUG((EFI_D_INFO, "EndpointDescriptor.Attributes: %x \n", EndpointDescriptor.Attributes));
        SERVER_IPMI_DEBUG((EFI_D_INFO, "EndpointDescriptor.EndpointAddress: %x \n", EndpointDescriptor.EndpointAddress));
        //
        // Check for Bulk EndPoint Attributes
        //
        if (EndpointDescriptor.Attributes != USB_ENDPOINT_BULK) {
            ++EndPoint;
            continue;
        }

        if ((BulkInEndpointFound != TRUE) && \
                IsBulkInEndPoint (EndpointDescriptor.EndpointAddress)) {
            gIpmiUsbInstance->BulkInEndpointAddr = EndpointDescriptor.EndpointAddress;
            BulkInEndpointFound = TRUE;
            SERVER_IPMI_DEBUG((EFI_D_INFO, "BulkInEndpointAddr: %x \n", gIpmiUsbInstance->BulkInEndpointAddr));
        } else if ((BulkOutEndpointFound != TRUE) &&\
                IsBulkOutEndPoint (EndpointDescriptor.EndpointAddress)) {
            gIpmiUsbInstance->BulkOutEndpointAddr = EndpointDescriptor.EndpointAddress;
            BulkOutEndpointFound = TRUE;
            SERVER_IPMI_DEBUG((EFI_D_INFO, "BulkOutEndpointAddr: %x \n", gIpmiUsbInstance->BulkOutEndpointAddr));
        }

        SERVER_IPMI_DEBUG((EFI_D_INFO, "BulkInEndpointFound: %x \n", BulkInEndpointFound));
        SERVER_IPMI_DEBUG((EFI_D_INFO, "BulkOutEndpointFound: %x \n", BulkOutEndpointFound));

        if ((BulkInEndpointFound == TRUE) && (BulkOutEndpointFound == TRUE)) {
            return EFI_SUCCESS;
        }
    } while (++EndPoint < InterfaceDescriptor.NumEndpoints);

    return EFI_NOT_FOUND;
}

/**
    Callback function in which IpmiUsb Protocol is installed if device is found

    @param Event Callback event
    @param Context Pointer to calling context

    @retval VOID

**/

VOID
EFIAPI
UsbIoProtocolCallback (
  IN EFI_EVENT      Event,
  IN VOID           *Context
  )
{
    EFI_STATUS                        Status;
    UINT8                             Index;
    UINT8                             DataSize;
    IPMI_SELF_TEST_RESULT_RESPONSE    UsbBstStatus;

    SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));

    //
    // Check for BMC USB Interface
    //
    if (TRUE != IsBmcUsbInterface()) {
        return;
    }

    //
    // Allocate and clear memory for IPMI Instance
    //
    gIpmiUsbInstance = AllocateRuntimeZeroPool (sizeof (EFI_IPMI_USB_INSTANCE_DATA));
    if (gIpmiUsbInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, " Failed to allocate memory for gIpmiUsbInstance \n"));
        return;
    }

    //
    // Fill the IPMI USB BMC Instance private data
    //
    gIpmiUsbInstance->Signature = IPMI_USB_SIGNATURE;
    gIpmiUsbInstance->BOTCommandTag = 0; //Initial value is zero.
    gIpmiUsbInstance->BmcStatus = EFI_BMC_OK;

    //
    // Save pointers to PeiServices and gUsbIo PPI
    //
    gIpmiUsbInstance->UsbIoHandle = gUsbIoHandle;
    gIpmiUsbInstance->UsbIo = gUsbIo;

    //
    // Get Bulk In and Out end point addresses and fill in private data
    //
    Status = GetBulkEndPoints();
    SERVER_IPMI_DEBUG((EFI_D_INFO, "GetBulkEndPoints: Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        SERVER_IPMI_DEBUG((EFI_D_ERROR, "Failed to get Bulk In and Out end point addresses! \n"));
        goto Error;
    }

    //
    // Fill the IPMI USB Transport Protocol
    //
    gIpmiUsbInstance->IpmiUsbTransport.SendIpmiCommand = EfiSendIpmiUsbCommand;
    gIpmiUsbInstance->IpmiUsbTransport.SendOemIpmiCommand = EfiSendOemIpmiUsbCommand;

    //
    // Get the SELF TEST Results.
    //
    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {
        DataSize = sizeof(UsbBstStatus);
        Status = gIpmiUsbInstance->IpmiUsbTransport.SendIpmiCommand (
                    &gIpmiUsbInstance->IpmiUsbTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&UsbBstStatus,
                    &DataSize
                    );
        DEBUG((EFI_D_INFO, "Self test result Status: %r Index: %x \n", Status, Index));
        if (!EFI_ERROR(Status)) {
            break;
        }
    }

    SERVER_IPMI_DEBUG((EFI_D_INFO, "Self test result Status: %r \n", Status));

    if (!EFI_ERROR(Status)) {
            SERVER_IPMI_DEBUG((EFI_D_INFO, "ResponseData[0]: %x \n", UsbBstStatus.Result));
            SERVER_IPMI_DEBUG((EFI_D_INFO, "ResponseData[1]: %x \n", UsbBstStatus.Param));
    }

    //
    // If Status indicates a Device error, then the BMC is not responding, so set BMC Status accordingly.
    //
    if (EFI_ERROR (Status)) {
        gIpmiUsbInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {
        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (UsbBstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                gIpmiUsbInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                gIpmiUsbInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                gIpmiUsbInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        }
    } //Else case end

    SERVER_IPMI_DEBUG((EFI_D_INFO, "BMC Status %x\n", gIpmiUsbInstance->BmcStatus));

    //
    // Now install IPMI USB Protocol if the BMC is not in a Hard Fail State
    //
    if (gIpmiUsbInstance->BmcStatus != EFI_BMC_HARDFAIL) {

        //
        //  Install Ipmi Usb transport protocol on same handle as BMC Usb IO protocol
        //
        Status = gBS->InstallProtocolInterface (
                    &gIpmiUsbInstance->UsbIoHandle,
                    &gEfiDxeIpmiUsbTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gIpmiUsbInstance->IpmiUsbTransport
                    );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "Install IpmiUsbTransportProtocol - Status: %r\n", Status));
    } else {
        //
        // Selftest has failed. So report this error and return EFI_UNSUPPORTED.
        //
        ReportStatusCode (
            EFI_ERROR_CODE|EFI_ERROR_MAJOR,
            (DXE_USB_BMC_SELFTEST_FAILED)
            );

        Status = EFI_UNSUPPORTED;
    }

Error:
    if (EFI_ERROR (Status)) {
        //
        // Control comes here only when BMC interface is found
        // But error happened in later stage. So free the allocated memory.
        //
        gBS->FreePool (gIpmiUsbInstance);
    }

    //
    // Close the registered event.
    //
    gBS->CloseEvent(Event);

    return;
}

/**
    Entry point for the IpmiUsb driver.

    @param ImageHandle Handle of this driver image
    @param SystemTable Table containing standard EFI services

    @return EFI_STATUS Status
    @retval EFI_SUCCESS Success
    @retval EFI_ERROR Failure

**/

EFI_STATUS
DxeIpmiUsbEntryPoint (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_EVENT       Event = NULL;
    VOID            *Context = NULL;

    // To Enable and Disable BMC USB Interface at exit boot via IPMI command"
#if VIRTUAL_BMC_USB_INTERFACE
    Status = gBS->LocateProtocol (
                     &gEfiDxeIpmiTransportProtocolGuid,
                     NULL,
                     &gIpmiTransportHandle
                     );

    if (Status == EFI_SUCCESS) {
       SERVER_IPMI_DEBUG((EFI_D_INFO, "Dxe Ipmi Transport Protocol Success\n"));
       BmcUsbInterfaceHook (Event, Context);
    } else {
        SERVER_IPMI_DEBUG((EFI_D_ERROR, "Dxe Ipmi Transport Protocol Fail\n"));
        EfiCreateProtocolNotifyEvent (
               &gEfiDxeIpmiTransportProtocolGuid,
               TPL_NOTIFY,
               BmcUsbInterfaceHook,
               NULL,
               &gDxeIpmiTransportRegistration
               );
    }
#endif

    EfiCreateProtocolNotifyEvent (
           &gEfiUsbIoProtocolGuid,
           TPL_NOTIFY,
           UsbIoProtocolCallback,
           NULL,
           &gIpmiUsbRegistration
           );

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
