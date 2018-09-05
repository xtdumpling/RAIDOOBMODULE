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
  
*/

#ifndef _EFI_SMI_FLASH_H_
#define _EFI_SMI_FLASH_H_

#include <Token.h>

// {6F2C06A8-D4B4-4a93-9F78-2B480BE80E0F}
#define EFI_SMI_FLASH_GUID \
    {0x6f2c06a8,0xd4b4,0x4a93,0x9f,0x78,0x2b,0x48,0xb,0xe8,0xe,0xf}

#define NUMBER_OF_BLOCKS (FLASH_SIZE/FLASH_BLOCK_SIZE)

#define SMIFLASH_ENABLE_FLASH      0x20
#define SMIFLASH_READ_FLASH        0x21
#define SMIFLASH_ERASE_FLASH       0x22
#define SMIFLASH_WRITE_FLASH       0x23
#define SMIFLASH_DISABLE_FLASH     0x24
#define SMIFLASH_GET_FLASH_INFO    0x25

typedef struct {
    UINT64      BufAddr;
    UINT32      BlockAddr;      //0 starts at 0xfff0_0000
    UINT32      BlockSize;
    UINT8       ErrorCode;
} FUNC_BLOCK;

#pragma pack(1)
typedef struct {
    UINT32      StartAddress;
    UINT32      BlockSize;
    UINT8       Type;
} BLOCK_DESC;

typedef enum {
    BOOT_BLOCK
    ,MAIN_BLOCK
    ,NV_BLOCK
#if SMI_FLASH_INTERFACE_VERSION > 10
    ,EC_BLOCK
#endif
    ,OA3_FLASH_BLOCK_DESC_TYPE = 0x4A
    ,NC_BLOCK = 0x80 //Types from NC_BLOCK to 0xFF are reserved for non critical blocks
} FLASH_BLOCK_TYPE;

typedef struct  {
    UINT32      Length;
    UINT8       Implemented;
    UINT8       Version;
    UINT16      TotalBlocks;
#if SMI_FLASH_INTERFACE_VERSION > 10
    UINT32      ECVersionOffset;
    UINT32      ECVersionMask;
#endif
// APTIOV_SERVER_OVERRIDE_START: Support AFU flash when "FPGA_SUPPORT" token is enabled.
#if SMI_FLASH_INTERFACE_VERSION >= 14
	UINT32		BiosRomSize;
	UINT16		BaseBlockSize;
	UINT8		Reserved[4];
#endif
// APTIOV_SERVER_OVERRIDE_END: Support AFU flash when "FPGA_SUPPORT" token is enabled.
    BLOCK_DESC  Blocks[NUMBER_OF_BLOCKS];
} INFO_BLOCK;
#pragma pack()

typedef struct _EFI_SMI_FLASH_PROTOCOL EFI_SMI_FLASH_PROTOCOL;

typedef EFI_STATUS (EFIAPI *GET_FLASH_INFO) (
    IN OUT INFO_BLOCK           *InfoBlock
);

typedef EFI_STATUS (EFIAPI *ENABLE_FLASH) (
    IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *DISABLE_FLASH) (
    IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *READ_FLASH) (
    IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *WRITE_FLASH) (
    IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *ERASE_FLASH) (
    IN OUT FUNC_BLOCK           *FuncBlock
);

struct _EFI_SMI_FLASH_PROTOCOL {
    GET_FLASH_INFO  GetFlashInfo;
    ENABLE_FLASH    EnableFlashWrite;
    DISABLE_FLASH   DisableFlashWrite;
    READ_FLASH      ReadFlash;
    WRITE_FLASH     WriteFlash;
    ERASE_FLASH     EraseFlash;
    UINT32          FlashCapacity;
};

extern EFI_GUID gEfiSmiFlashProtocolGuid;
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
