//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  Flash Protocol  
*/

#ifndef __FLASH_PROTOCOL__H__
#define __FLASH_PROTOCOL__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// {755B6596-6896-4ba3-B3DD-1C629FD1EA88}
#define FLASH_PROTOCOL_GUID \
    { 0x755b6596, 0x6896, 0x4ba3, 0xb3, 0xdd, 0x1c, 0x62, 0x9f, 0xd1, 0xea, 0x88 }

// {ECB867AB-8DF4-492d-8150-A7FD1B9B5A75}
#define FLASH_SMM_PROTOCOL_GUID \
    { 0xecb867ab, 0x8df4, 0x492d, 0x81, 0x50, 0xa7, 0xfd, 0x1b, 0x9b, 0x5a, 0x75 }

GUID_VARIABLE_DECLARATION(gFlashProtocolGuid,FLASH_PROTOCOL_GUID);
GUID_VARIABLE_DECLARATION(gFlashSmmProtocolGuid,FLASH_SMM_PROTOCOL_GUID);

typedef struct _FLASH_PROTOCOL FLASH_PROTOCOL;

typedef EFI_STATUS (EFIAPI *FLASH_READ_WRITE)(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);
typedef EFI_STATUS (EFIAPI *FLASH_ERASE)(
    VOID* FlashAddress, UINTN Size
);
typedef EFI_STATUS (EFIAPI *FLASH_WRITE_ENABLE_DISABLE)();

struct _FLASH_PROTOCOL {
    FLASH_READ_WRITE Read;
    FLASH_ERASE Erase;
    FLASH_READ_WRITE Write; //Write only
    FLASH_READ_WRITE Update; //Erase if necessary
    FLASH_WRITE_ENABLE_DISABLE DeviceWriteEnable;
    FLASH_WRITE_ENABLE_DISABLE DeviceWriteDisable; 
};

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
