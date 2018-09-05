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

/** @file DxeIpmiInterfaceLib.c
    Library function definitions for DxeIpmiInterfaceLib

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/IpmiUsbTransportProtocol.h>

//
// Global Variable
//
static  EFI_IPMI_TRANSPORT  *gIpmiTransport = NULL;
static  IPMI_USB_TRANSPORT  *gIpmiUsbTransport = NULL;

/**
    Locates Ipmi USB Transport Protocol. Ipmi USB protocol instance will be
    available in gIpmiUsbTransport.

    @param VOID

    @return VOID

**/
VOID
LocateIpmiUsbProtocol (
  VOID )
{
    EFI_STATUS      Status;

    //
    // Locate the IPMI USB transport protocol.
    //
    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiUsbTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiUsbTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DxeIpmiInterfaceLib: gEfiDxeIpmiUsbTransportProtocolGuid Status: %r \n", Status));
}

/**
    Locates Ipmi Transport Protocol. Ipmi protocol instance will be available
    in gIpmiTransport.

    @param VOID

    @return VOID

**/
VOID
LocateIpmiProtocol (
  VOID )
{
    EFI_STATUS      Status;

    //
    // Locate the IPMI transport protocol.
    //
    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DxeIpmiInterfaceLib: gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
}

/**
    Sends Ipmi command to BMC through Non USB (KCS or BT) Interface

    @param NetFunction       Net function of the command
    @param Lun               Logical Unit number of the command
    @param Command           Command to be sent to BMC
    @param CommandData       Command data to be sent along with Command
    @param CommandDataSize   Command Data size
    @param ResponseData      Response Data from BMC
    @param ResponseDataSize  Size of the Response data
    @param CompletionCode    Completion code returned by Bmc

    @return EFI_STATUS Status from BMC command

    @retval EFI_PROTOCOL_ERROR If Protocol is not found.

**/
EFI_STATUS
NonUsbSendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT8      *ResponseDataSize,
  OUT UINT8      *CompletionCode )
{

    EFI_STATUS Status;

    //
    // If gIpmiTransport is NULL, try to 
    // Locate Ipmi Transport Protocol.
    //
    if ( gIpmiTransport == NULL ) {
        LocateIpmiProtocol ();
        //
        // If gIpmiTransport is still NULL, return EFI_PROTOCOL_ERROR.
        //
        if ( gIpmiTransport == NULL ) {
            return EFI_PROTOCOL_ERROR;
        }
    }

    //
    // Call SendIpmiCommand
    //
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                NetFunction,
                Lun,
                Command,
                CommandData,
                CommandDataSize,
                ResponseData,
                ResponseDataSize );

    //
    //  If Completion Code is assigned to a proper address
    //
    if( (CompletionCode != NULL)  ) {

        //
        // Completion code will be got from a field in transport protocol,
        // If Status is success
        // If CC is error CC after receiving data from BMC, ResponseDataSize will be one and EFI_DEVICE_ERROR will be returned
        // Hence updating CC at that time also
        //
         if ( (Status == EFI_SUCCESS ) || ( ( Status == EFI_DEVICE_ERROR ) && ( *ResponseDataSize == 1 ) ) ) {
            *CompletionCode = gIpmiTransport->CommandCompletionCode;
        }
    }
    return Status;
}

/**
    Sends Ipmi command to BMC through USB Interface

    @param NetFunction      Net function of the command
    @param Lun              Logical Unit number of the command
    @param Command          Command to be sent to BMC
    @param CommandData      Command data to be sent along with Command
    @param CommandDataSize  Command Data size
    @param ResponseData     Response Data from BMC
    @param ResponseDataSize Size of the Response data
    @param CompletionCode   Completion code returned by Bmc

    @return EFI_STATUS Status from BMC command
    @retval EFI_PROTOCOL_ERROR If Protocol is not found.

**/
EFI_STATUS
UsbSendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT8      *ResponseDataSize, 
  OUT UINT8      *CompletionCode )
{
    EFI_STATUS Status;

    //
    // If gIpmiUsbTransport is NULL, try to 
    // Locate Ipmi USB Transport Protocol.
    //
    if ( gIpmiUsbTransport == NULL ) {
        LocateIpmiUsbProtocol ();

        //
        // If gIpmiUsbTransport is still NULL, return EFI_PROTOCOL_ERROR.
        //
        if ( gIpmiUsbTransport == NULL ) {
            return EFI_PROTOCOL_ERROR;
        }
    }

    //
    // Call SendIpmiCommand
    //
    Status = gIpmiUsbTransport->SendIpmiCommand (
                gIpmiUsbTransport,
                NetFunction,
                Lun,
                Command,
                CommandData,
                CommandDataSize,
                ResponseData,
                ResponseDataSize );
    //
    //  If Completion Code is assigned to a proper address
    //
    if( (CompletionCode != NULL)  ) {

        //
        // Completion code will be got from a field in transport protocol,
        // If Status is success
        // If CC is error CC (after receiving data from BMC, ResponseDataSize is made one and EFI_DEVICE_ERROR is returned)
        //
        if ( ( Status == EFI_SUCCESS ) || ( ( Status == EFI_DEVICE_ERROR ) && ( *ResponseDataSize == 1 ) ) ) {
            *CompletionCode = gIpmiUsbTransport->CommandCompletionCode;
        }
    }
    return Status;
}

/**
    Sends OEM Ipmi commands to BMC through USB Interface

    @param NetFunction      Net function of the command
    @param Lun              Logical Unit number of the command
    @param Command          Command to be sent to BMC
    @param CommandData      Command data to be sent along with Command
    @param CommandDataSize  Command Data size
    @param ResponseData     Response Data from BMC
    @param ResponseDataSize Size of the Response data
    @param CompletionCode   Completion code returned by Bmc

    @return EFI_STATUS Status from BMC command
    @retval EFI_PROTOCOL_ERROR If Protocol is not found.

**/
EFI_STATUS
UsbSendOemIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT32     CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT32     *ResponseDataSize,
  OUT UINT8      *CompletionCode )
{

    EFI_STATUS Status;
    //
    // If gIpmiUsbTransport is NULL, try to 
    // Locate Ipmi USB Transport Protocol.
    //
    if ( gIpmiUsbTransport == NULL ) {
        LocateIpmiUsbProtocol ();

        //
        // If gIpmiUsbTransport is still NULL, return EFI_PROTOCOL_ERROR.
        //
        if ( gIpmiUsbTransport == NULL ) {
            return EFI_PROTOCOL_ERROR;
        }
    }

    //
    // Call SendOemIpmiCommand
    //
    Status = gIpmiUsbTransport->SendOemIpmiCommand (
                gIpmiUsbTransport,
                NetFunction,
                Lun,
                Command,
                CommandData,
                CommandDataSize,
                ResponseData,
                ResponseDataSize );

    //
    //  If Completion Code is assigned to a proper address
    //
    if( (CompletionCode != NULL)  ) {

        //
        // Completion code will be got from a field in transport protocol,
        // If Status is success
        // If CC is error CC (after receiving data from BMC, ResponseDataSize is made one and EFI_DEVICE_ERROR is returned)
        //
        if ( ( Status == EFI_SUCCESS ) || ( ( Status == EFI_DEVICE_ERROR ) && ( *ResponseDataSize == 1 ) ) ) {
            *CompletionCode = gIpmiUsbTransport->CommandCompletionCode;
        }
    }
    return Status;
}

/**
    Sends Oem Ipmi commands to BMC based on "BulkDataTransferInterfaceType" PCD.
    This method can be called for OEM commands with datasize exceeding one byte.

    @param NetFunction      Net function of the command
    @param Lun              Logical Unit number of the command
    @param Command          Command to be sent to BMC
    @param CommandData      Command data to be sent along with Command
    @param CommandDataSize  Command Data size
    @param ResponseData     Response Data from BMC
    @param ResponseDataSize Size of the Response data
    @param CompletionCode   Completion code returned by Bmc

    @return EFI_STATUS Status from BMC command
    @retval EFI_PROTOCOL_ERROR If Protocol is not found.
    @retval EFI_UNSUPPORTED    If requested interface(USB/NON USB) does not support current request.

**/
EFI_STATUS
SendOemIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT32      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT32     *ResponseDataSize, 
  OUT UINT8      *CompletionCode )
{

    switch ( PcdGet8 (BulkDataTransferInterfaceType) ) {

        case NonUsbInterface:

            //
            // Return unsupported if interface is not usb as separate OEM method is
            // available only in USB interface
            //
            return  EFI_UNSUPPORTED;

      case UsbInterface:

            //
            // Call UsbSendOemIpmiCommand
            //
            return  UsbSendOemIpmiCommand (
                     NetFunction,
                     Lun,
                     Command,
                     CommandData,
                     CommandDataSize,
                     ResponseData,
                     ResponseDataSize, 
                     CompletionCode );

        //
        // Return Unsupported for default case.
        //
        default:
            return EFI_UNSUPPORTED;
    } // End Switch case 
}

/**
    Sends Ipmi command to BMC using interface type mentioned in
    "BulkDataTransferInterfaceType" PCD.

    @param NetFunction      Net function of the command
    @param Lun              Logical Unit number of the command
    @param Command          Command to be sent to BMC
    @param CommandData      Command data to be sent along with Command
    @param CommandDataSize  Command Data size
    @param ResponseData     Response Data from BMC
    @param ResponseDataSize Size of the Response data
    @param CompletionCode   Completion code returned by Bmc

    @return EFI_STATUS Status from BMC command
    @retval EFI_PROTOCOL_ERROR If Protocol is not found.

**/
EFI_STATUS
SendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8     *ResponseData,
  OUT UINT8     *ResponseDataSize, 
  OUT UINT8     *CompletionCode )
{

    switch ( PcdGet8 (BulkDataTransferInterfaceType) ) {

        case NonUsbInterface:

            //
            // Call NonUsbSendIpmiCommand
            //
            return  NonUsbSendIpmiCommand (
                        NetFunction,
                        Lun,
                        Command,
                        CommandData,
                        CommandDataSize,
                        ResponseData,
                        ResponseDataSize, 
                        CompletionCode );

        case UsbInterface:

            //
            // Call UsbSendIpmiCommand
            //
            return  UsbSendIpmiCommand (
                        NetFunction,
                        Lun,
                        Command,
                        CommandData,
                        CommandDataSize,
                        ResponseData,
                        ResponseDataSize, 
                        CompletionCode );
        //
        // Return Unsupported for default case.
        //
        default:
            return EFI_UNSUPPORTED;
    } // End Switch case 
}

/**
    The constructor function locates IPMI transport protocol.

    @param ImageHandle The firmware allocated handle for the EFI image.
    @param SystemTable A pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS The constructor always returns EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IpmiInterfaceLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable )
{
    //
    // Locate the IPMI transport protocol.
    //
    LocateIpmiProtocol ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DxeIpmiInterfaceLib: gIpmiTransport: %X \n", gIpmiTransport));

    //
    // Locate the IPMI USB transport protocol.
    //
    LocateIpmiUsbProtocol ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DxeIpmiInterfaceLib: gIpmiUsbTransport: %X \n", gIpmiUsbTransport));

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
