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
#ifndef _EFI_SEC_SMI_FLASH_H_
#define _EFI_SEC_SMI_FLASH_H_

#include <RomLayout.h>
#include <Protocol/Hash.h>
#include <Protocol/AmiDigitalSignature.h>
#include <BaseCryptLib.h>
#include <FlashUpd.h>
#include <Protocol/SmiFlash.h>

// {3BF4AF16-AB7C-4b43-898D-AB26AC5DDC6C}
#define EFI_SEC_SMI_FLASH_GUID \
{ 0x3bf4af16, 0xab7c, 0x4b43, { 0x89, 0x8d, 0xab, 0x26, 0xac, 0x5d, 0xdc, 0x6c } }

GUID_VARIABLE_DECLARATION(gSecureSMIFlashProtocolGuid, EFI_SEC_SMI_FLASH_GUID);

typedef struct _EFI_SEC_SMI_FLASH_PROTOCOL EFI_SEC_SMI_FLASH_PROTOCOL;

// SMI Flash INFO_BLOCK->Implemented bit mask flags
//typedef enum {SMIflash=0, noSMIflash=1, SecuredSMIflash=2};
#ifndef SwSmi_LoadFwImage 
#define SwSmi_LoadFwImage    0x1D
#define SwSmi_GetFlashPolicy 0x1E
#define SwSmi_SetFlashMethod 0x1F
#endif
// SMI Flash Command API
typedef enum {
 SecSMIflash_Load     =SwSmi_LoadFwImage,
 SecSMIflash_GetPolicy=SwSmi_GetFlashPolicy,
 SecSMIflash_SetFlash =SwSmi_SetFlashMethod,
    SMIflash_Enable  =0x20, 
    SMIflash_Read    =0x21,
    SMIflash_Erase   =0x22,
    SMIflash_Write   =0x23,
    SMIflash_Disable =0x24, 
    SMIflash_Info    =0x25
};

#define SEC_FLASH_HASH_TBL_BLOCK_COUNT  (FLASH_SIZE/FLASH_BLOCK_SIZE)
#define SEC_FLASH_HASH_TBL_SIZE (sizeof(EFI_SHA256_HASH)*SEC_FLASH_HASH_TBL_BLOCK_COUNT)

typedef struct {
// can be part of BIOS_INFO
    FLASH_UPD_POLICY	FlUpdPolicy;
    UINT8				PKpub[DEFAULT_RSA_KEY_MODULUS_LEN];
    UINT8  				ErrorCode;
} FLASH_POLICY_INFO_BLOCK;

typedef struct {
    AMI_FLASH_UPDATE_BLOCK FlUpdBlock;
    UINT8                  ErrorCode;
    UINT32                 FSHandle;
} FUNC_FLASH_SESSION_BLOCK;

typedef EFI_STATUS (EFIAPI *LOAD_FLASH_IMAGE) (
  IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *GET_FLASH_UPD_POLICY) (
  IN OUT FLASH_POLICY_INFO_BLOCK  *InfoBlock
);

typedef EFI_STATUS (EFIAPI *SET_FLASH_UPD_METHOD) (
  IN OUT FUNC_FLASH_SESSION_BLOCK  *InfoBlock
);

typedef EFI_STATUS (EFIAPI *CAPSULE_VALIDATE) (
  IN OUT UINT8     **pFwCapsule,
  IN OUT VOID     **pFWCapsuleHdr
);

struct _EFI_SEC_SMI_FLASH_PROTOCOL {
  LOAD_FLASH_IMAGE        LoadFwImage;  // SW SMI Port 0x1d
  GET_FLASH_UPD_POLICY GetFlUpdPolicy;	// SW SMI Port 0x1e
  SET_FLASH_UPD_METHOD SetFlUpdMethod;	// SW SMI Port 0x1f
  CAPSULE_VALIDATE CapsuleValidate;
  UINT32                  *pFwCapsule;  // Local buffer for FwCapsule
  ROM_AREA                  *RomLayout; // Local copy of Rom Layout table 
  EFI_SHA256_HASH            *HashTbl;  // ptr to Fw image Hash table
  UINT32                     FSHandle;  // SetFlash Session handle 
};

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
