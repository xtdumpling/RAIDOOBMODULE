//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
AMI BlockIO Write Protection protocol definition.
 **/

#ifndef __AMI_BLOCK_IO_WRITE_PROTECTION_PROTOCOL_
#define __AMI_BLOCK_IO_WRITE_PROTECTION_PROTOCOL_

#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/BlockIo.h>

#define AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL_GUID \
    { 0x7f955a3e, 0xafb5, 0x4122, { 0xb9, 0x25, 0x4b, 0x11, 0x71, 0xf6, 0x93, 0xf5 } }

typedef struct _AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL;

/**
Protects the Block if it has MBR or GPT partition information.

@param This         BlockIo Protection Protocol address
@param BlockIo      BlockIo Protocol Address.
@param Lba          LBA number to be verified. 
@param BufferSize   Number of bytes to be Write by BlockIO Protocol.

@retval EFI_SUCCESS     LBA doesn't contain GPT or MBR partition record 
                        or user accepted the write operation.
@retval EFI_NOT_FOUND   LBA contains the GPT or MBR partition record 
                        and user did not accept the write operation.
**/
typedef
EFI_STATUS
(EFIAPI *AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL_CHECK) (
    IN AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL *This,
    IN EFI_BLOCK_IO_PROTOCOL                 *BlockIo,
    IN EFI_LBA                               Lba,
    IN UINTN                                 BufferSize
 );

typedef struct _AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL {
    AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL_CHECK   BlockIoWriteProtectionCheck;
} AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL;

extern EFI_GUID gAmiBlockIoWriteProtectionProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
