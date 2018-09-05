//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
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
  This file contains Reflash driver structures definitions

**/
#ifndef __REFLASH__H__
#define __REFLASH__H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <ReFlashStrDefs.h>
#include <Token.h>
#ifndef VFRCOMPILE
#include <AmiDxeLib.h>
#include <Protocol/FlashProtocol.h>
#endif

#define RECOVERY_FORM_SET_CLASS             0x40
// {80E1202E-2697-4264-9CC9-80762C3E5863}
#define RECOVERY_FORM_SET_GUID { 0x80e1202e, 0x2697, 0x4264, 0x9c, 0xc9, 0x80, 0x76, 0x2c, 0x3e, 0x58, 0x63 }

#if REFLASH_INTERACTIVE
#define RECOVERY_MAIN 1
#define RECOVERY_FLASH 2
#define RECOVERY_USER 3
#else
#define RECOVERY_FLASH 1
#endif

#define FLASH_PROGRESS_KEY 100
#define FLASH_START_KEY 101
#define FLASH_ENABLE_KEY 102

#pragma pack(push, 1)
typedef struct {
        UINT8 UserOverride;                 //!< User override flag
        UINT8 VerificationStatus;           //!< Recovery capsule verification status
        UINT8 VerificationError;            //!< Recovery capsule verification error flag
        UINT8 UpdateNv;                     //!< Update NVRAM flag
        UINT8 UpdateBb;                     //!< Update boot block flag
        UINT8 UpdateMain;                   //!< Update main block flag
} AUTOFLASH;
#pragma pack(pop)
    
#include <Setup.h>

#ifndef VFRCOMPILE

#define FW_VERSION_VARIABLE L"AmiFwVersion"

typedef enum {
    FvTypeMain,
    FvTypeBootBlock,
    FvTypeNvRam,
    FvTypeBootBlockBackup,
    FvTypeCustom,
    FvTypeMax
} FLASH_FV_TYPE;

typedef struct _FLASH_AREA_EX FLASH_AREA_EX;

typedef EFI_STATUS (REFLASH_FUNCTION_EX)(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);

struct _FLASH_AREA_EX
{
    UINT8 *BlockAddress;                //!< Address of the area in flash
    UINT8 *BackUpAddress;               //!< Backup address of the area in flash
    UINT32 RomFileOffset;               //!< Offset of the area in ROM file
    UINTN Size;                         //!< Size of the area
    UINT32 BlockSize;                   //!< Size of the area block
    FLASH_FV_TYPE Type;                 //!< Type of the area
    BOOLEAN Update;                     //!< Update area flag
    BOOLEAN TopSwapTrigger;             //!< TOP_SWAP trigger flag
    REFLASH_FUNCTION_EX *BackUp;        //!< Area backup function
    REFLASH_FUNCTION_EX *Program;       //!< Area flash programm function
    EFI_STRING_ID BackUpStringId;       //!< Area backup message ID
    EFI_STRING_ID ProgramStringId;      //!< Area flash message ID
};

extern FLASH_AREA_EX *BlocksToUpdate;
extern FLASH_PROTOCOL *Flash;
extern EFI_HII_HANDLE ReflashHiiHandle;
extern UINT8 *RecoveryBuffer;


EFI_STATUS GetHiiString(
    IN     EFI_HII_HANDLE HiiHandle,
    IN     STRING_REF Token,
    IN OUT UINTN *DataSize, 
    OUT    EFI_STRING *Data
);

EFI_STATUS AmiFlashBackUp(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);
EFI_STATUS AmiFlashProgram(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);
EFI_STATUS AmiFlashProgramTopSwap(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);

VOID ApplyUserSelection(
    IN BOOLEAN Interactive
);

VOID OemBeforeFlashCallback(
    VOID
);

VOID OemAfterFlashCallback(
    VOID
);

EFI_STATUS FlashProgressEx(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

EFI_STATUS IsWin8Update(
    BOOLEAN RecoveryFailed
);

EFI_STATUS FlashWalker(
    IN BOOLEAN BackUp
);

EFI_STATUS Epilogue(
    VOID
);

EFI_STATUS Prologue(
    IN BOOLEAN Interactive,
    IN BOOLEAN Win8StyleUpdate
);

EFI_STATUS InstallEsrtTable(
    VOID
);

EFI_STATUS CreateFlashUpdateArea(
    VOID
);

EFI_STATUS FtControlFlow(
    VOID
);

#endif
    /****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
