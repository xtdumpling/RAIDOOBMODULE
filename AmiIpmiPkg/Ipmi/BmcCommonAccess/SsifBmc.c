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

/** @file SsifBmc.c
    SSIF Transport Hook.

**/

//----------------------------------------------------------------------

#include "IpmiBmc.h"
#include "SsifBmc.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------

extern EFI_SMBUS_HC_PROTOCOL *gEfiSmbusHcProtocol;

//----------------------------------------------------------------------

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
SendDataToSsifBmcPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
  IN  VOID                        *Context,
  IN  UINT8                       *Data,
  IN  UINT8                       DataSize )
{
    EFI_STATUS                 Status = EFI_SUCCESS;
    EFI_SMBUS_DEVICE_ADDRESS   BmcAddress;
    UINTN                      IpmiWriteCommand;
    UINT8                      IpmiData[SMBUS_BLOCK_LENGTH] = {0};
    UINTN                      DataLength = DataSize;
    UINT8                      DataIndex = 0;
    BOOLEAN                    PECSupport;
    UINT8                      Count = 0;

    PECSupport = IpmiInstance->Interface.SSIF.PecSupport;
    BmcAddress.SmbusDeviceAddress = IPMI_SSIF_SLAVE_ADDRESS;

    do {
        if (DataLength == DataSize) {
            if (DataSize  <= SMBUS_BLOCK_LENGTH) {
                //
                // Working single writes start
                //
                IpmiWriteCommand = SMBUS_SINGLE_WRITE_CMD;
                CopyMem (&IpmiData, &Data[DataIndex*SMBUS_BLOCK_LENGTH], DataLength);
            } else {
                //
                // Working multi-part writes start
                //
                IpmiWriteCommand = SMBUS_MULTI_WRITE_START_CMD;
                DataLength = SMBUS_BLOCK_LENGTH;
                CopyMem (&IpmiData, &Data[DataIndex*SMBUS_BLOCK_LENGTH], DataLength);
            }
        } else {
            if (DataSize  > SMBUS_BLOCK_LENGTH) {
                //
                // Working multi-part writes middle
                //
                IpmiWriteCommand = SMBUS_MULTI_WRITE_MIDDLE_CMD;
                DataLength = SMBUS_BLOCK_LENGTH;
                CopyMem (&IpmiData, &Data[DataIndex*SMBUS_BLOCK_LENGTH], DataLength);
            } else {
                //
                // Working multi-part writes end
                //
                IpmiWriteCommand = SMBUS_MULTI_WRITE_END_CMD;
                DataLength = DataSize;
                CopyMem (IpmiData, &Data[DataIndex*SMBUS_BLOCK_LENGTH], DataLength);
            }
        }
        
        Status = gEfiSmbusHcProtocol->Execute(
                        gEfiSmbusHcProtocol,
                        BmcAddress,
                        IpmiWriteCommand,
                        EfiSmbusWriteBlock,
                        PECSupport,
                        &DataLength,
                        IpmiData
                        );

        if (!EFI_ERROR(Status)) {
            if (DataSize >=  SMBUS_BLOCK_LENGTH) {
                Count = 0;
                DataSize -= SMBUS_BLOCK_LENGTH;
                DataIndex++;
            } else {
                DataSize = 0;
            }
        } else {
            if (Count == IpmiInstance->Interface.SSIF.SsifRetryCounter)
                break;
            else
                Count++;
            //
            // Failed retries delay about 60ms to 250ms
            //
            MicroSecondDelay(IPMI_SSIF_TIME_BETWEEN_REQUEST_RETRIES_DELAY);
        }
    } while (DataSize);

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
ReceiveBmcDataFromSsifPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
  IN  VOID                                         *Context,
  OUT UINT8                                     *Data,
  OUT UINT8                                     *DataSize )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_SMBUS_DEVICE_ADDRESS BmcAddress;
    UINTN IpmiReadCommand;
    UINT8 IpmiData[SMBUS_BLOCK_LENGTH];
    UINTN Length = 0;
    UINT8 DataIndex = 0;
    BOOLEAN PECSupport;

    Length = *DataSize;
    
    PECSupport = IpmiInstance->Interface.SSIF.PecSupport;
    BmcAddress.SmbusDeviceAddress = IPMI_SSIF_SLAVE_ADDRESS;
    IpmiReadCommand = SMBUS_SINGLE_READ_CMD;

    while (TRUE) {
        Status = gEfiSmbusHcProtocol->Execute(
                gEfiSmbusHcProtocol,
                BmcAddress,
                IpmiReadCommand,
                EfiSmbusReadBlock,
                PECSupport,
                &Length,
                (VOID *)IpmiData
                );

        if (EFI_ERROR(Status)) return Status;
        if (IpmiReadCommand == SMBUS_SINGLE_READ_CMD) {
            if ((IpmiData[0] == MULTI_READ_ZEROTH_STRT_BIT) && (IpmiData[1] == MULTI_READ_FIRST_STRT_BIT)) {
                //
                // Working multi-part reads start
                //
                CopyMem(Data, &IpmiData[2], Length-2);
                *DataSize = (UINT8)Length-2;
                IpmiReadCommand = SMBUS_MULTI_READ_MIDDLE_CMD;
            } else {
                //
                // Working single reads start
                //
                CopyMem(Data, IpmiData, Length);
                *DataSize = (UINT8)Length;
                break;
            }
        } else {
            if(IpmiData[0] == 0xFF) {
                //
                // Working multi-part reads end
                //
                CopyMem(&Data[*DataSize], &IpmiData[1], Length-1);
                *DataSize += (UINT8)Length-1;
                break;
            } else {
                //
                // Working multi-part reads middle
                //                
                CopyMem(&Data[*DataSize], &IpmiData[1], Length-1);
                *DataSize += (UINT8)Length-1;
                IpmiReadCommand = SMBUS_MULTI_READ_MIDDLE_CMD;
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
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
