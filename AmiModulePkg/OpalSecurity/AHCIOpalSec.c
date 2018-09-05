//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AHCIOpalSec.c
    Opal Support under Ahci mode

**/

//---------------------------------------------------------------------------

#include "OpalSecurity.h"

//---------------------------------------------------------------------------

#define ZeroMemory(Buffer,Size) pBS->SetMem(Buffer,Size,0)

/**
    Send a security protocol command to a device.

    @param  This - Indicates a pointer to the calling context. Type
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL is defined in the
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL description.
    @param  MediaId - ID of the medium to send data to.
    @param  Timeout - The timeout, in 100ns units, to use for the execution of the 
        security protocol command. A Timeout value of 0 means that this function 
        will wait indefinitely for the security protocol command to execute.
    @param  If Timeout is greater than zero, then this function will return 
        EFI_TIMEOUT if the time required to execute the receive data command 
        is greater than Timeout.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize    - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a buffer containing the security protocol 
        command specific payload data for the security protocol command.

         
    @retval EFI_SUCCESS The security protocol command completed successfully.
    @retval EFI_UNSUPPORTED The given MediaId does not support security protocol 
        commands.
    @retval EFI_DEVICE_ERROR The security protocol command completed with an error.
    @retval EFI_INVALID_PARAMETER The PayloadBuffer or PayloadTransferSize is 
        NULL and PayloadBufferSize is non-zero.

**/

EFI_STATUS 
AhciSendData (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN UINT32   MediaId,
    IN UINT64   Timeout,
    IN UINT8    SecurityProtocolId,
    IN UINT16   SecurityProtocolSpecificData,
    IN UINTN    PayloadBufferSize,
    IN VOID *PayloadBuffer 
)
{
    EFI_STATUS  Status;
    UINT8   SectorCountH = 0;
    COMMAND_STRUCTURE   CommandStructure;
    AMI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityProtocol = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This;
    SATA_DEVICE_INTERFACE   *SataDevInterface = (SATA_DEVICE_INTERFACE *)StorageSecurityProtocol->BusInterface;
    SATA_BLOCK_IO   *SataBlkIo = SataDevInterface->SataBlkIo;

    //
    // Check for BlkIo presence
    //
    if (SataBlkIo == NULL) {
        return EFI_UNSUPPORTED;
    }

    //
    // Check for Media change
    //
    if (SataBlkIo->BlkIo.Media->MediaId != MediaId) {
        return EFI_MEDIA_CHANGED;
    }
    
    SectorCountH = (UINT8)((PayloadBufferSize / SataBlkIo->BlkIo.Media->BlockSize)/0x100);

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    if (PayloadBufferSize != 0) {
		CommandStructure.LBALow     = (UINT8)SectorCountH;
        CommandStructure.LBAMid     = (UINT8)(SecurityProtocolSpecificData >> 8);
        CommandStructure.LBAHigh    = (UINT8)SecurityProtocolSpecificData;
        CommandStructure.Command    = TRUSTED_SEND;
        CommandStructure.Features   = SecurityProtocolId;
        CommandStructure.Buffer     = PayloadBuffer;
        CommandStructure.ByteCount  = (UINT32)PayloadBufferSize;

        if (PayloadBuffer == NULL) {
            return EFI_INVALID_PARAMETER;
        }

        Status = SataDevInterface->AhciBusInterface->SataPioDataOut(
                        SataDevInterface,
                        CommandStructure,
                        TRUE);

    } else {
        //
        // For PayloadBufferSize zero, Trusted Non Data command should be used
        // BIT 24 indicates Trusted Receive/Send
        //
        CommandStructure.LBAMid     = (UINT8)(SecurityProtocolSpecificData >> 8);
        CommandStructure.LBAHigh    = (UINT8)SecurityProtocolSpecificData;
        CommandStructure.Command    = TRUSTED_NON_DATA;
        CommandStructure.Features   = SecurityProtocolId;
        CommandStructure.Buffer     = PayloadBuffer;
        CommandStructure.ByteCount  = (UINT32)PayloadBufferSize;
        
        Status = SataDevInterface->AhciBusInterface->ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    }

    return Status;
}

/**
    Send a security protocol command to a device that receives 
    data and/or the result of one or more commands sent by 
    SendData.

    @param  This -Indicates a pointer to the calling context. Type 
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL is defined in the
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL description.
    @param  MediaId - ID of the medium to receive data from. 
    @param  Timeout - The timeout, in 100ns units, to use for the execution of the
        security protocol command. A Timeout value of 0 means that this 
        function will wait indefinitely for the security protocol command
        to execute. If Timeout is greater than zero, then this function 
        will return.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a destination buffer to store the 
        security protocol command specific payload data for the 
        security protocol command. The caller is responsible 
        for either having implicit or explicit ownership of 
        the buffer.
    @param  PayloadTransferSize - A pointer to a buffer to store the size in bytes
        of the data written to the payload data buffer.

         
    @retval EFI_SUCCESS The security protocol command completed successfully.
    @retval EFI_UNSUPPORTED The given MediaId does not support security 
        protocol commands.
    @retval EFI_DEVICE_ERROR The security protocol command completed with an error.
    @retval EFI_INVALID_PARAMETER The PayloadBuffer or PayloadTransferSize is 
        NULL and PayloadBufferSize is non-zero.
**/

EFI_STATUS
AhciReceiveData (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN UINT32    MediaId,
    IN UINT64   Timeout,
    IN UINT8     SecurityProtocolId,
    IN UINT16   SecurityProtocolSpecificData,
    IN UINTN    PayloadBufferSize,
    OUT VOID    *PayloadBuffer,
    OUT UINTN   *PayloadTransferSize
)
{
    EFI_STATUS  Status;
    UINT8   SectorCountL = 0;
    UINT8   SectorCountH = 0;
    COMMAND_STRUCTURE   CommandStructure;
    AMI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityProtocol = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This;
    SATA_DEVICE_INTERFACE   *SataDevInterface = (SATA_DEVICE_INTERFACE *)StorageSecurityProtocol->BusInterface;
    SATA_BLOCK_IO   *SataBlkIo = SataDevInterface->SataBlkIo;

    //
    // Check for BlkIo presence
    //
    if (SataBlkIo==NULL) {
        return EFI_UNSUPPORTED;
    }

    //
    // Check for Media change
    //
    if (SataBlkIo->BlkIo.Media->MediaId != MediaId) {
        return EFI_MEDIA_CHANGED;
    }

    SectorCountL = (UINT8)(PayloadBufferSize / SataBlkIo->BlkIo.Media->BlockSize);
    SectorCountH = (UINT8)((PayloadBufferSize / SataBlkIo->BlkIo.Media->BlockSize)/0x100);

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    if (PayloadBufferSize != 0) {
        //
        // For PayloadBufferSize non zero, Trusted Receive command should be used
        //
        if (PayloadTransferSize == NULL || PayloadBuffer == NULL) {
            return EFI_INVALID_PARAMETER;
        }

        CommandStructure.SectorCount    = (UINT16)SectorCountL;
        CommandStructure.LBALow         = (UINT8)SectorCountH;
        CommandStructure.LBAMid         = (UINT8)(SecurityProtocolSpecificData >> 8);
        CommandStructure.LBAHigh        = (UINT8)SecurityProtocolSpecificData;
        CommandStructure.Command        = TRUSTED_RECEIVE;
        CommandStructure.Features       = SecurityProtocolId;
        CommandStructure.Buffer         = PayloadBuffer;
        CommandStructure.ByteCount      = (UINT32)PayloadBufferSize ;
        
        Status = SataDevInterface->AhciBusInterface->ExecutePioDataCommand (SataDevInterface, &CommandStructure, FALSE);
    } else {
        //
        // For PayloadBufferSize zero, Trusted Non Data command should be used
        // BIT 24 indicates Trusted Receive/Send
        //
        CommandStructure.LBAMid     = (UINT8)(SecurityProtocolSpecificData >> 8);
        CommandStructure.LBAHigh    = (UINT8)SecurityProtocolSpecificData;
        CommandStructure.LBALowExp  = 0x01; // Trusted Receive
        CommandStructure.Command    = TRUSTED_NON_DATA;
        CommandStructure.Features   = SecurityProtocolId;
        CommandStructure.Buffer     = PayloadBuffer;
        CommandStructure.ByteCount  = (UINT32)PayloadBufferSize;
        
        Status = SataDevInterface->AhciBusInterface->ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    }

    if (!EFI_ERROR( Status) ) {
        *PayloadTransferSize = CommandStructure.ByteCount;
    } else {
        *PayloadTransferSize = 0;
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

