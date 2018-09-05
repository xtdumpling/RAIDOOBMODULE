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

/** @file IDEOpalSec.c
    IDE Opal Security Support

**/
//---------------------------------------------------------------------------

#include "OpalSecurity.h"

//---------------------------------------------------------------------------


/**
    Send a security protocol command to a device.

    @param  This - Indicates a pointer to the calling context. Type
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL is defined in the
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL description.
    @param  MediaId - ID of the medium to send data to.
    @param  Timeout - The timeout, in 100ns units, to use for the execution of the 
        security protocol command. A Timeout value of 0 means that this 
        function will wait indefinitely for the security protocol command 
        to execute. If Timeout is greater than zero, then this function 
        will return EFI_TIMEOUT if the time required to execute the 
        receive data command is greater than Timeout.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a buffer containing the security protocol 
        command specific payload data for the security protocol command.
         
    @retval EFI_SUCCESS The security protocol command completed successfully.
    @retval EFI_UNSUPPORTED The given MediaId does not support security 
        protocol commands.
    @retval EFI_DEVICE_ERROR The security protocol command completed with an error.
    @retval EFI_INVALID_PARAMETER The PayloadBuffer or PayloadTransferSize is 
        NULL and PayloadBufferSize is non-zero.

**/

EFI_STATUS 
IdeSendData (
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
    UINT8   SectorCountL = 0;
    UINT8   SectorCountH = 0;
    AMI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityProtocol = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This;
    AMI_IDE_BUS_PROTOCOL    *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)StorageSecurityProtocol->BusInterface;
    EFI_BLOCK_IO_PROTOCOL   *Blockio = &(IdeBusInterface->IdeBlkIo->BlkIo);

    //
    // Check for BlkIo presence
    //
    if (Blockio == NULL) {
        return EFI_UNSUPPORTED;
    }

    //
    // Check for Media change
    //
    if (Blockio->Media->MediaId != MediaId) {
        return EFI_MEDIA_CHANGED;
    }

    SectorCountL = (UINT8)(PayloadBufferSize / Blockio->Media->BlockSize);
    SectorCountH = (UINT8)((PayloadBufferSize / Blockio->Media->BlockSize)/0x100);

    if (PayloadBufferSize != 0) {
        //
        // For PayloadBufferSize non zero, Trusted Send command should be used
        //
        if (PayloadBuffer == NULL) {
            return EFI_INVALID_PARAMETER;
        }
        Status = IdeBusInterface->AtaPioDataOut(
                        IdeBusInterface,
                        PayloadBuffer,
                        (UINT32)PayloadBufferSize,
                        SecurityProtocolId,
                        SectorCountL, // Sector count - 512 bytes multiples
                        SectorCountH,//TransferLength
                        0,
                        (UINT8)(SecurityProtocolSpecificData >> 8) , //Com ID
                        0,
                        (UINT8)SecurityProtocolSpecificData,
                        0,
                        IdeBusInterface->IdeDevice.Device << 4,
                        TRUSTED_SEND,
                        TRUE,
                        FALSE );
    } else {
        //
        // For PayloadBufferSize zero, Trusted Non Data command should be used
        // BIT 24 indicates Trusted Receive/Send
        //
        Status = IdeBusInterface->IdeNonDataCommand(IdeBusInterface,
                SecurityProtocolId,
                0, // Reserved if command is Trusted Non-Data
                0, // Reserved if command is Trusted Non-Data
                0, // Reserved if command is Trusted Non-Data
                0, // Reserved if command is Trusted Non-Data
                (UINT8)(SecurityProtocolSpecificData >> 8),//Com Id
                0,
                (UINT8)SecurityProtocolSpecificData,
                0,
                IdeBusInterface->IdeDevice.Device << 4,
                TRUSTED_NON_DATA
            );
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
        function will wait indefinitely for the security protocol command to
        execute. If Timeout is greater than zero, then this function will 
        return.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a destination buffer to store the security 
        protocol command specific payload data for the security protocol 
        command. The caller is responsible for either having implicit or 
        explicit ownership of the buffer.
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
IdeReceiveData (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN UINT32   MediaId,
    IN UINT64    Timeout,
    IN UINT8     SecurityProtocolId,
    IN UINT16   SecurityProtocolSpecificData,
    IN UINTN    PayloadBufferSize,
    OUT VOID    *PayloadBuffer,
    OUT UINTN   *PayloadTransferSize
)
{
    EFI_STATUS  Status;
    UINT8   Device = 0;
    UINT8   SectorCountL = 0;
    UINT8   SectorCountH = 0;
    AMI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityProtocol = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This;
    AMI_IDE_BUS_PROTOCOL    *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)StorageSecurityProtocol->BusInterface;
    EFI_BLOCK_IO_PROTOCOL   *Blockio = &(IdeBusInterface->IdeBlkIo->BlkIo);

    //
    // Check for BlkIo presence
    //
    if (Blockio == NULL) {
        return EFI_UNSUPPORTED;
    }

    if (Blockio->Media->MediaId != MediaId) {
        return EFI_MEDIA_CHANGED;
    }

    SectorCountL = (UINT8)(PayloadBufferSize / Blockio->Media->BlockSize);
    SectorCountH = (UINT8)((PayloadBufferSize / Blockio->Media->BlockSize)/0x100);

    if (PayloadBufferSize != 0) {
        //
        // For PayloadBufferSize non zero, Trusted Receive command should be used
    	//
        if (PayloadTransferSize == NULL || PayloadBuffer == NULL) {
            return EFI_INVALID_PARAMETER;
        }
        
        Status = IdeBusInterface->AtaPioDataIn(
                        IdeBusInterface,
                        PayloadBuffer,
                        (UINT32)PayloadBufferSize,
                        SecurityProtocolId,
                        SectorCountL, // Sector count - 512 bytes multiples
                        SectorCountH,//TransferLength
                        (UINT8)(SecurityProtocolSpecificData >> 8) , //Com ID
                        (UINT8)SecurityProtocolSpecificData,
                        IdeBusInterface->IdeDevice.Device << 4,
                        TRUSTED_RECEIVE,
                        FALSE );
    } else {
        //
        // For PayloadBufferSize zero, Trusted Non Data command should be used
        // BIT 24 indicates Trusted Receive/Send
    	//
        UINT32 LBA = 0;

        //
        // IdeNonDataCommand supports only for 24 bits of LBA(Low, Mid and High). For 28 bit LBA 
        // we use device to fill remaining 4 bits (24:27).
        // Needed since bit 24 indicates Trusted Receive/Send.
        //
        LBA = (SecurityProtocolSpecificData << 8) | (1 << 24);
        Device = ((UINT8) ((UINT32) LBA >> 24 ) & 0x0f) | (IdeBusInterface->IdeDevice.Device << 4) | 0x40;

        Status = IdeBusInterface->IdeNonDataCommand(
                    IdeBusInterface,
                    SecurityProtocolId,
                    0, // Reserved if command is Trusted Non-Data
                    0, // Reserved if command is Trusted Non-Data
                    0, // Reserved if command is Trusted Non-Data
                    0x01, // Trusted Receive
                    (UINT8)(SecurityProtocolSpecificData >> 8) , //Com ID - Discovery 0
                    0,
                    (UINT8)SecurityProtocolSpecificData,
                    0,
                    Device,
                    TRUSTED_NON_DATA );
    }

    if (!EFI_ERROR( Status) ) {
        *PayloadTransferSize = PayloadBufferSize;
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

