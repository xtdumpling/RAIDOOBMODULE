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

/** @file UsbBmc.c
    Implementation of BMC USB Interface

**/

//----------------------------------------------------------------------

#include "DxeIpmiUsb.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//
// Global variables
//
UINT32  gResponseDataSize = 0;
UINT16  gBulkTransferTimeOut = 0;  // Timeout variable to get timeout provided in PCD
BOOLEAN gUsbProtocolError = FALSE;  // Variable to check if Usb protocol instance is still valid.

//----------------------------------------------------------------------

/**
    This function swaps the bytes in dword: 0-3,1-2,2-1,3-0. Can be used in
    little endian->big endian conversions.

    @param dData DWORD to swap

    @return UINT32 Value after swapping the bytes.

**/

UINT32
dabc_to_abcd (
  IN  UINT32    dData )
{

    return ((( dData & 0x000000FF) << 24 )
            | (( dData & 0x0000FF00) << 8 )
            | (( dData & 0x00FF0000) >> 8 )
            | (( dData & 0xFF000000) >> 24 ));

}

/**
    Sends Clear Feature command for specified Bulk-In/Bulk-out Endpoint

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param EndpointAddr    - Bulk Endpoint Address

    @return EFI_STATUS

**/
EFI_STATUS
BotClearEndpoint (
    IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
    IN  UINT16 EndpointAddr ) {

    EFI_USB_DEVICE_REQUEST  DevReq;
    EFI_STATUS              Status;
    UINT32                  UsbStatus;
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;

    ZeroMem(&DevReq,sizeof(EFI_USB_DEVICE_REQUEST));
    //
    // Fill the Device Request structure to Clear specific device feature
    //
    DevReq.RequestType = USB_REQ_TYPE_ENDPOINT;
    DevReq.Request = USB_DEV_CLEAR_FEATURE;
    DevReq.Value = EfiUsbEndpointHalt;
    DevReq.Length = 0x00;

    //
    // Clear Feature Halt for provided Bulk-In/Bulk-out Endpoint
    //
    DevReq.Index = (UINT8)EndpointAddr;

    //
    // Issue ControlTranfer transaction
    //
    Status = UsbIo->UsbControlTransfer (
                UsbIo,
                &DevReq,
                EfiUsbNoData,
                THREE_THOUSAND_MS,
                NULL,
                0,
                &UsbStatus );

    if( EFI_ERROR(Status) ) {
       SERVER_IPMI_DEBUG((EFI_D_ERROR, "%a Error : UsbStatus = %x\n", __FUNCTION__, UsbStatus));
    }
    return Status;

}

/**
    Performs BOT Recovery Reset in case of certain Error conditions.

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.

    @return EFI_STATUS

**/

EFI_STATUS
BotRecoveryReset (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance )
{
    EFI_STATUS              Status;
    EFI_USB_DEVICE_REQUEST  DevReq;
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                  UsbStatus;

    ZeroMem(&DevReq,sizeof(EFI_USB_DEVICE_REQUEST));

    //
    // Fill the Device Request structure to issue the Bulk-Only Mass Storage Reset
    //
    DevReq.RequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
    DevReq.Request = 0xFF;
    DevReq.Value = 0x00;
    DevReq.Index = 0x00;
    DevReq.Length = 0x00;

    //
    // Issue ControlTranfer transaction
    //
    Status = UsbIo->UsbControlTransfer (
                UsbIo,
                &DevReq,
                EfiUsbNoData,
                THREE_THOUSAND_MS,
                NULL,
                0,
                &UsbStatus );

    if ( EFI_ERROR(Status) ) {
        SERVER_IPMI_DEBUG((EFI_D_ERROR, "%a Error : UsbStatus = %x\n", __FUNCTION__, UsbStatus));
        return Status;
    }

 
    //
    // Clear Feature Halt for Bulk-In Endpoint
    //
    BotClearEndpoint(IpmiUsbInstance, IpmiUsbInstance->BulkInEndpointAddr);

    //
    // Clear Feature Halt for Bulk-Out Endpoint
    //
    Status = BotClearEndpoint(IpmiUsbInstance, IpmiUsbInstance->BulkOutEndpointAddr);
    return Status;
}

/**
    Gets the BOT status sequence

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.

    @retval EFI_STATUS

**/

EFI_STATUS
BotStatusPhase (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance )
{
    EFI_STATUS             Status;
    CSW                    Csw ;
    UINTN                  DataSize = sizeof(CSW);
    EFI_USB_IO_PROTOCOL    *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                 UsbStatus = 0;

    SetMem( &Csw, sizeof(CSW), 0x00);

    //
    // Get the status field from bulk transfer
    //
    Status = UsbIo->UsbBulkTransfer (
                UsbIo,
                IpmiUsbInstance->BulkInEndpointAddr,
                &Csw,
                &DataSize,
                gBulkTransferTimeOut,
                &UsbStatus );

    if( EFI_ERROR(Status) ) {
        SERVER_IPMI_DEBUG((EFI_D_ERROR, "Dxe BotStatusPhase Error : Status = %r UsbStatus = %x\n",Status,UsbStatus));
        if ( UsbStatus == EFI_USB_ERR_STALL ) {

          //
          // Clear stall in Bulk-In Endpoint
          //
          BotClearEndpoint(IpmiUsbInstance, IpmiUsbInstance->BulkInEndpointAddr);

          //
          // Send bulk transfer command again 
          //
          SetMem( &Csw, sizeof(CSW), 0x00);
          DataSize = sizeof(CSW);
          UsbStatus = 0;
          Status = UsbIo->UsbBulkTransfer (
                    UsbIo,
                    IpmiUsbInstance->BulkInEndpointAddr,
                    &Csw,
                    &DataSize,
                    gBulkTransferTimeOut,
                    &UsbStatus );
        }
        if( EFI_ERROR(Status) ) {
            SERVER_IPMI_DEBUG((EFI_D_ERROR, "BotStatusPhase Error : Performing RecoveryReset\n"));
            BotRecoveryReset( IpmiUsbInstance );
            return EFI_DEVICE_ERROR;
        }
    }

    //
    // Check for valid CSW
    //
    if ( (DataSize != sizeof (CSW)) || \
         (Csw.dCSWSignature != BOT_CSW_SIGNATURE) || \
         (Csw.dCSWTag != IpmiUsbInstance->BOTCommandTag) ) {
        BotRecoveryReset (IpmiUsbInstance);
        return EFI_DEVICE_ERROR;
    }
    //
    // Check for meaningful CSW
    //
    if (Csw.bCSWStatus) {
        if (Csw.bCSWStatus == CSW_PHASE_ERROR) {
            BotRecoveryReset (IpmiUsbInstance);
        }
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
    Sends the BOT command sequence

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param Opcode - Opcode of the command
    @param Sector - DATA /Command sector
    @param DataTransferSize - Size of data to be transferred from/to host
              in the data phase

    @return EFI_STATUS

**/

EFI_STATUS
BotCommandPhase(
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         Opcode,
  IN  UINT8                         Sector,
  IN  UINT32                        DataTransferSize )
{
    EFI_STATUS              Status;
    COMN_RWV_CMD            fpCmdBuffer;
    BOT_CMD_BLK             Cbw;
    UINTN                   DataSize = sizeof(BOT_CMD_BLK);
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                  UsbStatus;

    SetMem (&Cbw, sizeof(BOT_CMD_BLK), 0x00);
    SetMem (&fpCmdBuffer, sizeof(COMN_RWV_CMD), 0x00);

    //
    // Fill the command block
    //
    Cbw.dCbwSignature = BOT_CBW_SIGNATURE;
    Cbw.dCbwTag = ++IpmiUsbInstance->BOTCommandTag;
    Cbw.dCbwDataLength = DataTransferSize;
    Cbw.bCbwLun = LOGICAL_UNIT_NUMBER;
    Cbw.bCbwLength = sizeof(COMN_RWV_CMD);

    //
    // Fill OEM defined command for read/write and fill the command buffer
    //
    if( Opcode == COMMON_WRITE_10_OPCODE ){
        Cbw.bmCbwFlags = EfiUsbDataIn << CBW_FLAG_SHIFT;
        fpCmdBuffer.bOpCode = SCSI_AMICMD_CURI_WRITE;
    } else {
        Cbw.bmCbwFlags = EfiUsbDataOut << CBW_FLAG_SHIFT;
        fpCmdBuffer.bOpCode = SCSI_AMICMD_CURI_READ;  
    }

    fpCmdBuffer.dLba = dabc_to_abcd(Sector);
    fpCmdBuffer.wTransferLength = TRANSFER_LENGTH;
    fpCmdBuffer.bLun = LOGICAL_UNIT_NUMBER;

    //
    // Copy the buffer to command block's command buffer
    // 
    CopyMem (Cbw.aCBWCB, &fpCmdBuffer, sizeof(COMN_RWV_CMD));

    //
    // Send the command to host using bulk transfer
    //
    Status = UsbIo->UsbBulkTransfer (
                UsbIo,
                IpmiUsbInstance->BulkOutEndpointAddr,
                &Cbw,
                &DataSize,
                gBulkTransferTimeOut,
                &UsbStatus );
    if ( EFI_ERROR( Status ) ) {
        //
        // Command phase fail, we need to recovery reset this device
        //
        BotRecoveryReset( IpmiUsbInstance );
        return EFI_DEVICE_ERROR;
    }

    return Status;
}

/**
    Read Response data from BMC

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param Opcode - Opcode of the command
    @param Sector - DATA /Command sector
    @param ResponseDataSize - Size of data to be read from BMC
    @param ResponseData - Pointer to Response data.

    @return EFI_STATUS

**/

EFI_STATUS
ReadResponseData (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         Opcode,
  IN  UINT8                         Sector,
  IN  UINT8                         EndpointAddr,
  IN  UINT32                        ResponseDataSize,
  IN  UINT8                         *ResponseData )
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                  UsbStatus;
    UINT8                   *TempData = &IpmiUsbInstance->TempData[0];
    UINT32                  DataBytesRead = 0;
    UINT32                  MaxTempDataSize;

    //
    // Read all the data from BMC in a loop.
    //
    while ( ( DataBytesRead < ( ResponseDataSize - 1 ) ) || ( ResponseDataSize == 1 ) ) {

        //
        // Issue Bulk Transfer to read the data from BMC.
        //
        MaxTempDataSize = MAX_TEMP_DATA;
        Status = UsbIo->UsbBulkTransfer (
                    UsbIo,
                    EndpointAddr,
                    TempData,
                    (UINTN*)&MaxTempDataSize,
                    gBulkTransferTimeOut,
                    &UsbStatus );
        //
        // For error case, return the Status.
        //
        if ( EFI_ERROR(Status) ) {
            return Status;
        }

        //
        // Control comes here only when UsbBulkTransfer is success and for the first time.
        // Based on the DataBytesRead value, copy the data into ResponseData pointer.
        //
        if ( DataBytesRead == 0 ) {
            //
            // When DataBytesRead is zero, check for error completion code.
            //
            IpmiUsbInstance->IpmiUsbTransport.CommandCompletionCode = TempData[0];

            if ( !ERROR_COMPLETION_CODE(TempData[0]) ) {

                //
                // If there is no response data other than  successful completion code, then return success
                //
                if ( ResponseDataSize == 1 ) {
                  return EFI_SUCCESS;
                }

                //
                // For successful completion code, exclude the completion code,  
                // update the remaining data in ResponseData pointer and 
                // update DataBytesRead value.
                //
                CopyMem (ResponseData, &TempData[1], MaxTempDataSize - 1);
                DataBytesRead = MaxTempDataSize - EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE;
            } else {

                //
                // For error complete code, return EFI_SUCCESS.
                // Error completion code is handled in EfiSendIpmiCommand() function.
                //
                return EFI_SUCCESS;
            }
        } else {
            //
            // When DataBytesRead is not zero, update the complete data in 
            // ResponseData pointer and update DataBytesRead value.
            //
            CopyMem (&ResponseData[DataBytesRead], &TempData[0], MaxTempDataSize);
            DataBytesRead += MaxTempDataSize;
        } //if ( DataBytesRead == 0 )

    } //while ( DataBytesRead < *ResponseDataSize )

    return EFI_SUCCESS;
}

/**
    Write command data to BMC

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param Opcode - Opcode of the command
    @param Sector - DATA /Command sector
    @param CommandRequestDataSize - Size of Command, NetFnLun and command data
    @param RequestData - Pointer to Request data.

    @return EFI_STATUS

**/

EFI_STATUS
WriteCommandData (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         Opcode,
  IN  UINT8                         Sector,
  IN  UINT8                         EndpointAddr,
  IN  UINT32                        CommandRequestDataSize,
  IN  UINT8                         *RequestData )
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                  UsbStatus;
    UINT32                  RemainingDataBytes = CommandRequestDataSize;
    UINT32                  IpmiRequestSize;
    UINT32                  IpmiRequestOffset;
    UINT32                  RequestDataOffset = 0;

    //
    // IPMI Command and NetFnLun is already filled in TempData by EfiSendIpmiCommand().
    //
    UINT8                   *IpmiRequest = &IpmiUsbInstance->TempData[0];

    //
    // Write all the data into BMC in a loop.
    //
    while ( RemainingDataBytes != 0 ) {

        //
        // Remaining data bytes will be Request data + command header size
        //
        if ( RemainingDataBytes <= MAX_TEMP_DATA ) {
            IpmiRequestSize = RemainingDataBytes;
        } else {
            IpmiRequestSize = MAX_TEMP_DATA;
        }

        //
        // Fill command data in IpmiRequest.
        //
        if ( RemainingDataBytes == CommandRequestDataSize ) {

            IpmiRequestOffset = EFI_IPMI_USB_COMMAND_DATA_HEADER_SIZE;

        } else {

            IpmiRequestOffset = 0;
        } //if ( RemainingDataBytes == CommandRequestDataSize )

        SERVER_IPMI_DEBUG((EFI_D_INFO, "IpmiRequestSize: %x \n", IpmiRequestSize));
        SERVER_IPMI_DEBUG((EFI_D_INFO, "IpmiRequestOffset: %x \n", IpmiRequestOffset));
        SERVER_IPMI_DEBUG((EFI_D_INFO, "RequestDataOffset: %x \n", RequestDataOffset));

        //
        // Copy the data into IpmiRequest and 
        // update RemainingDataBytes
        //
        CopyMem (
            &IpmiRequest[IpmiRequestOffset],
            &RequestData[RequestDataOffset],
            (IpmiRequestSize -IpmiRequestOffset) );
        RemainingDataBytes -= IpmiRequestSize;

        //
        // RequestDataOffset is calculated based on IpmiRequestSize as a check to avoid
        // sending command data size more than specified BMC data size in MAX_TEMP_DATA.
        //
        RequestDataOffset += (IpmiRequestSize - IpmiRequestOffset);

        SERVER_IPMI_DEBUG((EFI_D_INFO, "RemainingDataBytes: %x \n", RemainingDataBytes));

        //
        // Issue Bulk Transfer to write the data into BMC.
        //
        Status = UsbIo->UsbBulkTransfer (
                    UsbIo,
                    EndpointAddr,
                    (UINT8*)&IpmiRequest[0],
                    (UINTN*)&IpmiRequestSize,
                    gBulkTransferTimeOut,
                    &UsbStatus );
        //
        // For error case, return the Status.
        //
        if ( EFI_ERROR(Status) ) {
            return Status;
        }

    } //while ( RemainingDataBytes != 0 )

    return EFI_SUCCESS;
}

/**
    Issues USB transaction to BMC USB interface.

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param Opcode - Opcode of the command
    @param Sector - DATA /Command sector
    @param EndpointAddr - USB End point address
    @param Data - Pointer to input DATA
    @param DataSize - Size of input data

    @return EFI_STATUS

**/

EFI_STATUS
IssueBmcUsbTransaction (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         Opcode,
  IN  UINT8                         Sector,
  IN  UINT8                         EndpointAddr,
  IN  UINT8                         *Data,
  IN  UINT32                        DataSize )
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo = IpmiUsbInstance->UsbIo;
    UINT32                  UsbStatus;

    SERVER_IPMI_DEBUG((EFI_D_INFO, "DataSize: %x \n", DataSize));

    //
    // Command Phase
    //
    Status = BotCommandPhase (
                IpmiUsbInstance,
                Opcode,
                Sector,
                DataSize );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "BotCommandPhase: Status: %r \n", Status));
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // Data Phase
    //
    if ( (Opcode == COMMON_READ_10_OPCODE ) && \
         (Sector == AMI_DATA_SECTOR) ) {
        //
        // Read the Response data
        //
        Status = ReadResponseData (
                    IpmiUsbInstance,
                    Opcode,
                    Sector,
                    EndpointAddr,
                    DataSize,
                    Data );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "ReadResponseData: Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            return Status;
        }
    } else if ( (Opcode == COMMON_WRITE_10_OPCODE ) && \
                (Sector == AMI_DATA_SECTOR) ) {
        //
        // Write the Command data
        //
        Status = WriteCommandData (
                    IpmiUsbInstance,
                    Opcode,
                    Sector,
                    EndpointAddr,
                    DataSize,
                    Data );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "WriteCommandData: Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            return Status;
        }
    } else {

        Status = UsbIo->UsbBulkTransfer (
                    UsbIo,
                    EndpointAddr,
                    Data,
                    (UINTN*)&DataSize,
                    gBulkTransferTimeOut,
                    &UsbStatus );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "UsbIo->UsbBulkTransfer: Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            return Status;
        }
    }

    //
    // Status Phase
    //
    Status = BotStatusPhase (IpmiUsbInstance);
    SERVER_IPMI_DEBUG((EFI_D_INFO, "BotStatusPhase: Status: %r \n", Status));
    if ( EFI_ERROR(Status)) {
        return Status;
    }

    return Status;
}

/**
    Sends command data to BMC USB interface.

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param IpmiRequest - Pointer to input command DATA
    @param DataSize - Size of input command data

    @return EFI_STATUS

**/

EFI_STATUS
SendDataToUsbBmc (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         *IpmiRequest,
  IN  UINT32                        DataSize )
{
    EFI_STATUS          Status;
    UINT32              Timeout = TWO_THOUSAND_MS;
    UINT32              DataTransferSize;
    CONFIG_CMD          CmdHeader = { BEGIN_SIG, IPMI_USB_CMD_RESERVED, 0, 0, 0, 0, 0 };

    //
    // Size of input command data
    //
    CmdHeader.DataInLen = DataSize;

    //
    // Send the Command header
    //
    DataTransferSize = sizeof(CONFIG_CMD);
    Status = IssueBmcUsbTransaction (
                IpmiUsbInstance,
                COMMON_WRITE_10_OPCODE,
                AMI_CMD_SECTOR,
                IpmiUsbInstance->BulkOutEndpointAddr,
                (UINT8*)&CmdHeader,
                DataTransferSize );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "Send the Command header: IssueBmcUsbTransaction: Status: %r \n", Status));
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // Send the IPMI packets 
    //
    DataTransferSize = DataSize;
    Status = IssueBmcUsbTransaction (
                IpmiUsbInstance,
                COMMON_WRITE_10_OPCODE,
                AMI_DATA_SECTOR,
                IpmiUsbInstance->BulkOutEndpointAddr,
                IpmiRequest,
                DataTransferSize );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "Send the IPMI packets : IssueBmcUsbTransaction: Status: %r \n", Status));
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    // 
    // Wait for command completion, Read Status now
    //
    do {
        DataTransferSize = sizeof (CONFIG_CMD);
        Status = IssueBmcUsbTransaction (
                    IpmiUsbInstance,
                    COMMON_READ_10_OPCODE,
                    AMI_CMD_SECTOR,
                    IpmiUsbInstance->BulkInEndpointAddr,
                    (UINT8*)&CmdHeader,
                    DataTransferSize );
        SERVER_IPMI_DEBUG((EFI_D_INFO, "Wait for command completion : IssueBmcUsbTransaction: Status: %r \n", Status));
        if ( EFI_ERROR(Status) ) {
            return Status;
        }
        //
        // Stall for 1ms
        //
        if ( CmdHeader.Status & IN_PROCESS ) {
            MicroSecondDelay( ONE_THOUSAND_MICRO_SECOND );
        } else {
            break;
        }
    } while( --Timeout );

    //
    // Check for the status of command completion
    //
    switch ( CmdHeader.Status ) {
        case ERR_SUCCESS:
            Status = EFI_SUCCESS;
            //
            // If command is successful, update the gResponseDataSize variable
            //
            gResponseDataSize = CmdHeader.DataOutLen;
            SERVER_IPMI_DEBUG((EFI_D_INFO,"gResponseDataSize: %lx \n", gResponseDataSize));
            break;
        case ERR_BIG_DATA:
            SERVER_IPMI_DEBUG((EFI_D_ERROR,"Too much data send to IPMI over USB\n"));
            Status = EFI_INVALID_PARAMETER;
            break;
        case ERR_NO_DATA:
            SERVER_IPMI_DEBUG((EFI_D_ERROR,"Too little data send to IPMI over USB\n"));
            Status = EFI_INVALID_PARAMETER;
            break;
        case ERR_UNSUPPORTED:
            SERVER_IPMI_DEBUG((EFI_D_ERROR,"Unsupported command\n"));
            Status = EFI_UNSUPPORTED;
            break;
        default:
            SERVER_IPMI_DEBUG((EFI_D_ERROR,"Unknown status\n"));
            Status = EFI_DEVICE_ERROR;
            break;
    }

    return Status;
}

/**
    Receives response data from BMC USB interface.

    @param IpmiUsbInstance - Pointer to IpmiUsb instance.
    @param IpmiRequest - Pointer to response DATA
    @param IpmiResponseDataSize - Size of response DATA

    @return EFI_STATUS

**/

EFI_STATUS
ReceiveDataFromUsbBmc (
  IN  EFI_IPMI_USB_INSTANCE_DATA    *IpmiUsbInstance,
  IN  UINT8                         *ResponseData,
  IN  UINT32                         *IpmiResponseDataSize )
{
    EFI_STATUS          Status;
    UINT32              DataTransferSize = *IpmiResponseDataSize;
    CONFIG_CMD          CmdHeader = { BEGIN_SIG, IPMI_USB_CMD_RESERVED, 0, 0, 0, 0, 0 };

    SERVER_IPMI_DEBUG((EFI_D_INFO, "%a: DataTransferSize: %x \n", __FUNCTION__, DataTransferSize));

    //
    // Get the IPMI response data
    //
    Status = IssueBmcUsbTransaction (
                IpmiUsbInstance,
                COMMON_READ_10_OPCODE,
                AMI_DATA_SECTOR,
                IpmiUsbInstance->BulkInEndpointAddr,
                ResponseData,
                DataTransferSize );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "Get the IPMI response: IssueBmcUsbTransaction: Status: %r \n", Status));
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // Send configuration command to see if read of data itself had any problems
    // This read verifies that the previous read above was okay
    //
    DataTransferSize = sizeof(CONFIG_CMD);
    Status = IssueBmcUsbTransaction (
                IpmiUsbInstance,
                COMMON_READ_10_OPCODE,
                AMI_CMD_SECTOR,
                IpmiUsbInstance->BulkInEndpointAddr,
                (UINT8*)&CmdHeader,
                DataTransferSize );
    SERVER_IPMI_DEBUG((EFI_D_INFO, "Send configuration command: IssueBmcUsbTransaction: Status: %r \n", Status));
    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // Check the status of transaction to read response data.
    // For success case, update response data size and return EFI_SUCCESS
    // For error case, return EFI_DEVICE_ERROR
    //
    if ( CmdHeader.Status  == ERR_SUCCESS ) {
        *IpmiResponseDataSize = CmdHeader.DataOutLen;
        Status = EFI_SUCCESS;
    } else {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}

/**
    USB interface IPMI send command Implementation

    @param This - Pointer to IPMI USB transport PPI/Protocol
    @param NetFunction - Net function of the command
    @param Lun - Logical Unit number of the command
    @param Command - Command to be sent to BMC
    @param CommandData - Command data to be sent along with Command
    @param CommandDataSize - Command Data size
    @param ResponseData - Response Data from BMC
    @param ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiSendIpmiUsbCommand (
  IN  IPMI_USB_TRANSPORT        *This,
  IN  UINT8                     NetFunction,
  IN  UINT8                     Lun,
  IN  UINT8                     Command,
  IN  UINT8                     *CommandData,
  IN  UINT8                     CommandDataSize,
  OUT UINT8                     *ResponseData,
  OUT UINT8                     *ResponseDataSize )
{
    UINT32                      ResponseDataSize32;
    UINT32                      CommandDataSize32;
    EFI_STATUS                  Status;

    ResponseDataSize32 = *ResponseDataSize;
    CommandDataSize32 = CommandDataSize;

    Status = EfiSendOemIpmiUsbCommand (
                This,
                NetFunction,
                Lun,
                Command,
                CommandData,
                CommandDataSize32,
                ResponseData,
                &ResponseDataSize32 );

    *ResponseDataSize = (UINT8)ResponseDataSize32;

    return Status;
}

/**
    USB interface IPMI send command Implementation

    @param This - Pointer to IPMI USB transport PPI/Protocol
    @param NetFunction - Net function of the command
    @param Lun - Logical Unit number of the command
    @param Command - Command to be sent to BMC
    @param CommandData - Command data to be sent along with Command
    @param CommandDataSize - Command Data size
    @param ResponseData - Response Data from BMC
    @param ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiSendOemIpmiUsbCommand (
  IN  IPMI_USB_TRANSPORT        *This,
  IN  UINT8                     NetFunction,
  IN  UINT8                     Lun,
  IN  UINT8                     Command,
  IN  UINT8                     *CommandData,
  IN  UINT32                    CommandDataSize,
  OUT UINT8                     *ResponseData,
  OUT UINT32                    *ResponseDataSize )
{
    EFI_STATUS                      Status;
    EFI_IPMI_USB_INSTANCE_DATA      *IpmiUsbInstance;
    EFI_IPMI_USB_COMMAND_DATA       *IpmiRequest;
    UINT32                          DataSize;
    EFI_USB_IO_PROTOCOL             *UsbIo = NULL;

    //
    // Clear Response data size global variable.
    //
    gResponseDataSize = 0;

    //
    // Assign Timeout value to global variable
    //
    gBulkTransferTimeOut = PcdGet16 (BulkDataTransferTimeout);

    //
    // Get the pointer to IpmiUsbInstance from PPI
    //
    IpmiUsbInstance = INSTANCE_DATA_FROM_IPMI_USB_TRANSPORT_THIS (This);

    //
    //  Check if error condition(USB protocol is not available in USB handle) is already
    //  found to be invalid. If so, return protocol error immediately.
    //
    if ( gUsbProtocolError == TRUE ) {
        return EFI_PROTOCOL_ERROR;
    }

    //
    //  Check if USB IO protocol installed for BMC device is still available.
    //
    Status = gBS->HandleProtocol(IpmiUsbInstance->UsbIoHandle, &gEfiUsbIoProtocolGuid, (VOID **)&UsbIo);

    if (EFI_ERROR(Status)) {

       gUsbProtocolError = TRUE;

       //
       //  Bmc Usb device is disconnected, return Protocol Error
       //
       return EFI_PROTOCOL_ERROR;
    }

    //
    // Validate ResponseDataSize and ResponseData input parameters
    //
    if ( !ResponseDataSize || (!ResponseData && *ResponseDataSize) ) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Fill the IPMI Command and NetFnLun in TempData.
    //
    IpmiRequest = (EFI_IPMI_USB_COMMAND_DATA*)&IpmiUsbInstance->TempData[0];
    IpmiRequest->NetFnLUN = COMBINE_NETFUN_LUN(NetFunction, Lun);
    IpmiRequest->Command = Command;

    //
    // Validate command data size and command data pointer
    // and copy the command data in IPMI request
    //
    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            return EFI_INVALID_PARAMETER;
        }
    }

    //
    // Send Data to UsbBmc
    // Data size is calculated as command data size + command header size
    //
    DataSize = EFI_IPMI_USB_COMMAND_DATA_HEADER_SIZE + CommandDataSize;
    if ( (Status = SendDataToUsbBmc (IpmiUsbInstance, (UINT8*)CommandData,
                DataSize)) != EFI_SUCCESS ) {
        IpmiUsbInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        return Status;
    }

    //
    // SendDataToUsbBmc() function updates the gResponseDataSize value.
    // Check if the ResponseData input variable is capable of holding complete data.
    // Exclude size of completion code and validate ResponseDataSize
    // If it is lesser than required response data size, update
    // required data size in it and return EFI_BUFFER_TOO_SMALL.
    //
    if ( (gResponseDataSize - EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE) > *ResponseDataSize ) {
        *ResponseDataSize = gResponseDataSize - EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE;
        return EFI_BUFFER_TOO_SMALL;
    }

    //
    // Read response from UsbBmc
    //
    DataSize = gResponseDataSize;
    if ( (Status = ReceiveDataFromUsbBmc (IpmiUsbInstance, (UINT8*)ResponseData,
                 &DataSize)) != EFI_SUCCESS ) {
        IpmiUsbInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        return Status;
    }

    //
    // Check for command completion code
    //
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {

        //
        // Write completion code into return buffer and return device error
        // if an IPMI command returns an error
        //
        if ( *ResponseDataSize ) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        return EFI_DEVICE_ERROR;
    } else {

        //
        // Update the ResponseDataSize with output data.
        //
        *ResponseDataSize = DataSize - EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE;
    }

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
