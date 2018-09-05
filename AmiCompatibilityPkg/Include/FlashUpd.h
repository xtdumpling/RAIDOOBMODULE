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
/** @file

	Secure Flash Update Data structures

*/
#ifndef _EFI_FLASH_UPD_H_
#define _EFI_FLASH_UPD_H_

#include "Pei.h"

//----------------------------------------------------------------------------
// GUID used to identify FW Capsule Hdr FFS file within the Firmware Volume. Aptio Tools must support this GUID
//----------------------------------------------------------------------------
#define AMI_FW_CAPSULE_FFS_GUID \
    {0x414D94AD,0x998D,0x47D2,0xBF,0xCD,0x4E,0x88,0x22,0x41,0xDE,0x32}

//----------------------------------------------------------------------------
// Section GUID used to identify FW Capsule Hdr section within FwCap FFS file. Aptio Tools must support this GUID
//----------------------------------------------------------------------------
#define AMI_FW_CAPSULE_SECTION_GUID \
	{0x5A88641B,0xBBB9,0x4AA6,0x80,0xF7,0x49,0x8A,0xE4,0x07,0xC3,0x1F}

//----------------------------------------------------------------------------
// EFI FW Capsule Recovery PPI
//----------------------------------------------------------------------------

//2A6E902B-F1F3-4275-BC7B-40FD4B5481E7
#define EFI_PEI_BOOT_IN_FLASH_UPDATE_MODE_PEIM_PPI \
    { 0x2A6E902B, 0xF1F3, 0x4275, 0xBC, 0x7B, 0x40, 0xFD, 0x4B, 0x54, 0x81, 0xE7 }

GUID_VARIABLE_DECLARATION(gFlashUpdBootModePpiGuid,EFI_PEI_BOOT_IN_FLASH_UPDATE_MODE_PEIM_PPI);

// FW Capsule Recovery
// {5E794317-A07E-45df-94BB-1C997D6232CA}
#define  AMI_FW_RECOVERY_CAPSULE_GUID \
    { 0x5e794317, 0xa07e, 0x45df, 0x94, 0xbb, 0x1c, 0x99, 0x7d, 0x62, 0x32, 0xca }

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        FLASH_UPD_POLICY
//
// Description:    This structure is returned by Flash Update Policy protocol
//
// Fields: Name     Type        Description
//----------------------------------------------------------------------------
// FlashUpdate      FLASH_OP    Bit map of Enabled Flash update methods (online, recovery, capsule)
// BBUpdate         FLASH_OP    Bit map of Enabled CRTM block update methods
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct { 
    UINT8           FlashUpdate; //FLASH_OP
    UINT8           BBUpdate;
} FLASH_UPD_POLICY;

//----------------------------------------------------------------------------
// IsRecovery prototypes
//----------------------------------------------------------------------------
// Bit Mask of checks to perform on Aptio FW Image
// 1- Capsule integrity
// 2- Verify Signature
// 3- Verify FW Key
// 4- Verify FW Version compatibility. 
//    To prevent possible re-play attack:
//    update current FW with older version with lower security.
typedef enum {
    Cap=1,
    Sig,
    Key,
    Ver
} VerifyTask;

BOOLEAN IsFlashUpdate (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_BOOT_MODE       *BootMode
);
// next 2 are left for backward compatibility with pre-ported SbPei.c
BOOLEAN IsFlashUpdateRecovery (
    EFI_PEI_SERVICES    **PeiServices
);

BOOLEAN IsFlashUpdateS3Capsule (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_BOOT_MODE       *BootMode
);

EFI_STATUS FwCapsuleInfo (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsuleName,
  IN OUT UINTN         *pCapsuleSize,
  OUT    BOOLEAN       *ExtendedVerification
);

EFI_STATUS VerifyFwImage(
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID           **pCapsule,
  IN OUT UINT32         *pCapsuleSize,
  IN OUT UINT32         *FailedVTask
);

//----------------------------------------------------------------------------
// EFI Variable defines
//----------------------------------------------------------------------------
// {974231D5-ED4B-44d1-8870-CE515CC14D68}
#define FLASH_UPDATE_GUID \
    {0x974231d5, 0xed4b, 0x44d1, 0x88, 0x70, 0xce, 0x51, 0x5c, 0xc1, 0x4d, 0x68}

#define FLASH_UPDATE_VAR  L"AmiFlashUpd"

//Enumerated Flash update methods
typedef enum {
    FlDisabled = 0,
    FlRecovery = 1,        // power can be lost, FW Capsule image read from file on the recovery media    
    FlCapsule  = 2,        // S3/SW reset with memory intact. FW Capsule resides in Memory
    FlRuntime  = 4,        // runtime update using secured SMM Flash protocol
} FLASH_OP;

// UINT32 ROMSection 
// Bit mask of enumerated main Aptio Flash blocks.
// FLASH_BLOCK_TYPE = 0-BB, 1-MAIN, 2-NV, 3-EC, etc.
// TBD. Need to be replaced by the flash map with guided types and offsets within new flash image
// Order a complete re-flash if new flash map is different(map hash don't compare)
typedef enum {
     FV_BB
    ,FV_MAIN
    ,FV_NV
    ,ROM_EC
    ,ROM_NC = 0x80 //Types from NC_BLOCK to 0xFF are reserved for non critical blocks
} FLASH_ROM_SECTIONS;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        AMI_FLASH_UPDATE_BLOCK
//
// Description: This structure represents NVRAM variable stored to 
//              indicate pending Flash Update operation
//
// Fields: Name     Type        Description
//----------------------------------------------------------------------------
// FlashOpType      FLASH_OP    Pending Flash update method
// ROMSection       UINT32      Bit map of FW image blocks to be flashed.
//                              Can be 32bit map of the mem ranges within ROM Map data structure.
// FwImage
//  AmiRomFileName   CHAR8[16]  Null terminated file name on recovery media. FlashOpType=Recovery
//  CapsuleMailboxPtr UINT64[2] Capsule Mailbox Ptr. FlashOpType=Capsule 
// ImageSize        UINT32      Size of FW image including the Capsule's header
// MonotonicCounter UINT32      Current value of EFI Hi Monotonic Counter. 
//                              Prevents FW update re-play attacks - no stale FlashUpd requests allowed
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
#pragma pack(1)
typedef struct {
    UINT8               FlashOpType;
    UINT32              ROMSection;
    union {
      CHAR8             AmiRomFileName[16]; // can be a file path
      EFI_PHYSICAL_ADDRESS   CapsuleMailboxPtr[2];
    } FwImage;
    UINT32              ImageSize;
    UINT32              MonotonicCounter;
} AMI_FLASH_UPDATE_BLOCK;

#pragma pack()

#endif

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
