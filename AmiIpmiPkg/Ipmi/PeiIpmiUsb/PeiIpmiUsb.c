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

/** @file PeiIpmiUsb.c
    File contains implementation for PeiIpmiUsb module.

**/

//----------------------------------------------------------------------

#include "PeiIpmiUsb.h"

//----------------------------------------------------------------------

/**
    If following conditions are met, returns TRUE else FALSE.
    1. Checks for PEI_BOT_DEVICE_SIGNATURE
    2. Checks for Vendor ID and Product ID of BMC USB Interface.

    @param PeiServices Pointer to the PEI Core data Structure
    @param PeiBotDevice Pointer to PEI BOT device structure.

    @retval TRUE BMC USB Interface is found.
    @retval FALSE BMC USB Interface is not found.

**/

BOOLEAN
IsBmcUsbInterface (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  PEI_BOT_DEVICE    *PeiBotDevice )
{
    EFI_STATUS                  Status;
    PEI_USB_IO_PPI              *UsbIo = NULL;
    EFI_USB_DEVICE_REQUEST      DevReq;
    EFI_USB_DEVICE_DESCRIPTOR   DevDesc;
    UINTN                       DescriptorLength;
    UINT32                      UsbStatus;

    //
    // Verify if this Block IO PPI is installed by USB PEI driver.
    // Use signature for this purpose. Return FALSE, if
    // Signature does not match with PEI_BOT_DEVICE_SIGNATURE
    //
    if ( PeiBotDevice->Signature != PEI_BOT_DEVICE_SIGNATURE ) {
        return FALSE;
    }

    //
    // Get the pointer to UsbIo from PEI_BOT_DEVICE structure
    //
    UsbIo = PeiBotDevice->UsbIoPpi;

    //
    // GetDeviceDescriptor API is not available in UsbIoPpi.
    // So Get Device descriptor details using UsbIo->ControlTransfer API.
    // Fill the Device request structure.
    //
    DevReq.RequestType = USB_DEV_GET_DESCRIPTOR_REQ_TYPE;
    DevReq.Request = USB_DEV_GET_DESCRIPTOR;
    DevReq.Value = DESCRIPTOR_TYPE_INDEX_VALUE( USB_DT_DEVICE );
    DevReq.Index = 0x00;
    DevReq.Length = DescriptorLength = sizeof (DevDesc);

    //
    // Issue ControlTranfer transaction
    //
    Status = UsbIo->UsbControlTransfer (
                PeiServices,
                UsbIo,
                &DevReq,
                EfiUsbDataIn,
                THREE_THOUSAND_MS,
                &DevDesc,
                DescriptorLength,
                &UsbStatus
                );
    //
    // Return FALSE, For Error case
    //
    if ( EFI_ERROR(Status) ) {
        return FALSE;
    }

    //
    // Check for Vendor ID and Product ID
    // Return TRUE, If Vendor ID and Product ID matches
    // with BMC Usb Interface. Else, return FALSE.
    //
    if ( (DevDesc.IdVendor == BMC_USB_INTERFACE_VENDOR_ID) &&\
         (DevDesc.IdProduct == BMC_USB_INTERFACE_PRODUCT_ID) ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "BMC USB interface is found.\n"));
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
    Following things are done in entry point.
    1. Check for presence of BMC USB Interface.
    2. If interface is available, send self test command to get the status of BMC.
    3. Install the PeiIpmiUsb PPI if BMC is not in Hard fail state.

    @param FileHandle Pointer to image file handle.
    @param PeiServices Pointer to the PEI Core data Structure

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializePeiIpmiUsb (
  IN        EFI_PEI_FILE_HANDLE     FileHandle,
  IN  CONST EFI_PEI_SERVICES        **PeiServices )
{
    EFI_STATUS                      Status;
    UINTN                           BlockIoPpiInstance = 0;
    EFI_PEI_RECOVERY_BLOCK_IO_PPI   *BlockIoPpi = NULL;
    PEI_BOT_DEVICE                  *PeiBotDevice = NULL;
    EFI_IPMI_USB_INSTANCE_DATA      *IpmiUsbInstance;
    EFI_PEI_PPI_DESCRIPTOR          *PeiIpmiUsbPpiDesc;
    UINT8                           Index;
    UINT8                           DataSize;
    IPMI_SELF_TEST_RESULT_RESPONSE  UsbBstStatus;

    //
    // Locate all the instances of BLOCK IO PPI.
    //
    do {
        Status = (*PeiServices)->LocatePpi(
                    PeiServices,
                    &gEfiPeiVirtualBlockIoPpiGuid,
                    BlockIoPpiInstance,
                    NULL,
                    (VOID **)&BlockIoPpi );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiPeiVirtualBlockIoPpiGuid Status %r\n", Status));
        //
        // For error case, print the error status and return
        //
        if (EFI_ERROR (Status)) {
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "BMC USB interface is not found.\n"));
            return EFI_UNSUPPORTED;
        }

        //
        // Get the pointer to PEI_BOT_DEVICE structure from BlockIoPpi
        //
        PeiBotDevice = PEI_BOT_DEVICE_FROM_THIS( BlockIoPpi );

        //
        // Check for Bmc Usb Interface
        //
        if (IsBmcUsbInterface ((EFI_PEI_SERVICES **)PeiServices, PeiBotDevice)) {
            break;
        }

        //
        // Increment the BlockIo PPI instance
        //
        BlockIoPpiInstance++;

    } while(TRUE);

    //
    // Allocate memory for IPMI Instance
    //   
    IpmiUsbInstance = AllocateZeroPool (sizeof (EFI_IPMI_USB_INSTANCE_DATA));
    if (IpmiUsbInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for IpmiUsbInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Allocate memory for PPI Descriptor
    //
    PeiIpmiUsbPpiDesc = AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    if (PeiIpmiUsbPpiDesc == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for PeiIpmiUsbPpiDesc! \n"));
        //
        // AllocateZeroPool has failed, so free the allocated memory for IpmiUsbInstance and return EFI_OUT_OF_RESOURCES.
        //
        FreePool (IpmiUsbInstance);
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Fill the IPMI USB BMC Instance private data
    //
    IpmiUsbInstance->Signature = IPMI_USB_SIGNATURE;
    IpmiUsbInstance->BOTCommandTag = 0; //Initial value is zero.
    IpmiUsbInstance->BmcStatus = EFI_BMC_OK;

    //
    // Fill Bulk In and Out end point addresses
    //
    IpmiUsbInstance->BulkInEndpointAddr = PeiBotDevice->BulkInEndpoint->EndpointAddress;
    IpmiUsbInstance->BulkOutEndpointAddr = PeiBotDevice->BulkOutEndpoint->EndpointAddress;

    //
    // Save pointers to PeiServices and UsbIo PPI
    //
    IpmiUsbInstance->UsbIo = PeiBotDevice->UsbIoPpi;
    IpmiUsbInstance->Context = (VOID *)PeiServices;

    //
    // Fill the IPMI USB Transport PPI
    //
    IpmiUsbInstance->IpmiUsbTransport.SendIpmiCommand = EfiSendIpmiUsbCommand;
    IpmiUsbInstance->IpmiUsbTransport.SendOemIpmiCommand = EfiSendOemIpmiUsbCommand;

    //
    // Get the SELF TEST Results.
    //
    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {
        DataSize = sizeof (UsbBstStatus);
        Status = IpmiUsbInstance->IpmiUsbTransport.SendIpmiCommand (
                    &IpmiUsbInstance->IpmiUsbTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&UsbBstStatus,
                    &DataSize );
        DEBUG ((EFI_D_INFO, "Get Self test results Index: %x Status %r\n", Index, Status));
        if (!EFI_ERROR(Status)) {
                SERVER_IPMI_DEBUG((EFI_D_INFO, "ResponseData[0]: %x \n", UsbBstStatus.Result));
                SERVER_IPMI_DEBUG((EFI_D_INFO, "ResponseData[1]: %x \n", UsbBstStatus.Param));
            break;
        }
    }
    DEBUG ((EFI_D_INFO, "Self test result Status: %r \n", Status));

    //
    // If Status indicates a Device error, then the BMC is not responding, so set BMC Status accordingly.
    //
    if (EFI_ERROR (Status)) {
        IpmiUsbInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {
        //
        // Check the self test results. Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (UsbBstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                IpmiUsbInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                IpmiUsbInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                IpmiUsbInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        }
    } //Else case end
 
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BMC Status %x\n", IpmiUsbInstance->BmcStatus));

    //
    // Now install IPMI USB PPI if the BMC is not in a Hard Fail State
    //
    if (IpmiUsbInstance->BmcStatus != EFI_BMC_HARDFAIL) {
        PeiIpmiUsbPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        PeiIpmiUsbPpiDesc->Guid = &gEfiPeiIpmiUsbTransportPpiGuid;
        PeiIpmiUsbPpiDesc->Ppi = &IpmiUsbInstance->IpmiUsbTransport;

        Status = (*PeiServices)->InstallPpi (
                    PeiServices,
                    PeiIpmiUsbPpiDesc );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Install IpmiUsbTransportPpi - Status: %r\n", Status));
    } else {
        //
        // Selftest has failed. So report this error and return EFI_UNSUPPORTED to unload driver from memory.
        //
        ReportStatusCode (
            EFI_ERROR_CODE | EFI_ERROR_MAJOR,
            (PEI_USB_BMC_SELFTEST_FAILED) );
        Status = EFI_UNSUPPORTED;
    }
    //
    // Frees the allocated memory for IpmiUsbInstance & PeiIpmiUsbPpiDesc and return Status.
    //
    FreePool (IpmiUsbInstance);
    FreePool (PeiIpmiUsbPpiDesc);
    return Status;
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
