//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmbBmc.c
    IPMB Transport functions.

**/

//----------------------------------------------------------------------

#include "IpmiBmc.h"
#include "IpmbBmc.h"

//
// Global Variable
//
extern EFI_I2C_MASTER_PROTOCOL *gMasterTransmit;

/**
    Sends the command to BMC port

    @param IpmiInstance Data structure describing BMC
           variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to command data that will be sent to BMC
           along with Command
    @param DataSize Size of the command data

    @return  EFI_STATUS Status of the Sending data to BMC port

**/

EFI_STATUS
SendDataToIpmbBmcPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  IN  UINT8                           *Data,
  IN  UINT8                           DataSize )
{

    EFI_STATUS             Status = EFI_SUCCESS;
    EFI_I2C_REQUEST_PACKET Packet;
    EFI_EVENT Event = NULL;

    //
    // Pack command data.
    // 
    Packet.Operation[0].Buffer = Data;
    Packet.Operation[0].LengthInBytes = DataSize;
    Packet.Operation[0].Flags = WRITE_FLAG;

    //
    // Call the StartRequest function
    //
    Status = gMasterTransmit->StartRequest(
            gMasterTransmit,
            IPMI_BMC_SLAVE_ADDRESS,
            &Packet,
            Event,
            &Status
            );
    return Status;
}

/**
    Receives the Data from BMC port

    @param IpmiInstance Data structure describing BMC
            variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to Response data that is received from BMC
    @param DataSize Size of the Response data

    @return  EFI_STATUS Status of the Receiving data from BMC port

**/

EFI_STATUS
ReceiveBmcDataFromIpmbPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  OUT UINT8                           *Data,
  OUT UINT8                           *DataSize )
{

    EFI_STATUS Status = EFI_SUCCESS;
    EFI_I2C_REQUEST_PACKET Packet;
    EFI_EVENT Event = NULL;

    //
    // Pack command data.
    // 
    Packet.Operation[0].Buffer = Data;
    Packet.Operation[0].Flags = READ_FLAG;

    //
    // Call the StartRequest function
    //
    Status = gMasterTransmit->StartRequest(
            gMasterTransmit,
            IPMI_BMC_SLAVE_ADDRESS,
            &Packet,
            Event,
            &Status
            );
    Data = Packet.Operation[0].Buffer;
    *DataSize = Packet.Operation[0].LengthInBytes;
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
