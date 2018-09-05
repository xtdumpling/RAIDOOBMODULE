//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file AmiTseHddSecurity.h
    Header file for the HddPassword

**/

#ifndef _HDD_SECURITY_AMITSE_H_
#define _HDD_SECURITY_AMITSE_H_

//---------------------------------------------------------------------------

#include "AmiStatusCodes.h"
#include "AMITSEStrTokens.h"

//---------------------------------------------------------------------------

#define HDD_UNLOCKED_GUID \
    { 0x1fd29be6, 0x70d0, 0x42a4, 0xa6, 0xe7, 0xe5, 0xd1, 0xe, 0x6a, 0xc3, 0x76};

#define HDD_PASSWORD_PROMPT_ENTER_GUID \
    { 0x8e8d584a, 0x6e32, 0x44bf, { 0xb9, 0x6e, 0x1d, 0x27, 0x7, 0xc4, 0xeb, 0x5c } }

#define HDD_PASSWORD_PROMPT_EXIT_GUID \
    { 0xe22af903, 0xfd6f, 0x4d22, { 0x94, 0xce, 0xf8, 0x49, 0xc6, 0x14, 0xc3, 0x45 } }

#define     SECURITY_SET_PASSWORD           0xF1
#define     SECURITY_UNLOCK                 0xF2
#define     SECURITY_ERASE_PREPARE          0xF3
#define     SECURITY_ERASE_UNIT             0xF4
#define     SECURITY_FREEZE_LOCK            0xF5
#define     SECURITY_DISABLE_PASSWORD       0xF6
#define     SECURITY_BUFFER_LENGTH          512     // Bytes
#define     HDD_MODEL_NUMBER_LENGTH         39     // Model number is 40 (0-39) bytes
#define     SPACE_ASCII_VALUE               0x20

#ifndef AMI_DXE_BS_EC_INVALID_IDE_PASSWORD
  #define AMI_DXE_BS_EC_INVALID_IDE_PASSWORD (AMI_STATUS_CODE_CLASS\
                                              | 0x00000005)
#endif

#ifndef DXE_INVALID_IDE_PASSWORD
  #define DXE_INVALID_IDE_PASSWORD (EFI_SOFTWARE_DXE_BS_DRIVER\
                                    | AMI_DXE_BS_EC_INVALID_IDE_PASSWORD)
#endif

#define NG_SIZE                     19
#define VARIABLE_ID_AMITSESETUP     5
#if !defined(SECURITY_SETUP_ON_SAME_PAGE) || SECURITY_SETUP_ON_SAME_PAGE == 0
#define INVALID_HANDLE  ((VOID*)-1)
#endif
extern VOID * gHiiHandle;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;

extern VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
extern EFI_STATUS VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size );
extern VOID MemFreePointer (VOID **ptr);
extern VOID MemCopy( VOID *dest, VOID *src, UINTN size );
extern UINT16 HiiAddString( /*EFI_HII_HANDLE*/VOID* handle, CHAR16 *string );
extern CHAR16 *HiiGetString( VOID* handle, UINT16 token );
extern VOID    CheckForKeyHook( EFI_EVENT Event, VOID *Context );
extern UINTN TestPrintLength ( IN CHAR16   *String );
extern VOID _DrawPasswordWindow(UINT16 PromptToken, UINTN PasswordLength, UINTN *CurrXPos, UINTN *CurrYPos);
extern VOID _ReportInBox(UINTN PasswordLength, UINT16 BoxToken, UINTN CurrXPos, UINTN CurrYPos, BOOLEAN bWaitForReturn);
extern EFI_STATUS _GetPassword(CHAR16 *PasswordEntered, UINTN PasswordLength, UINTN CurrXPos, UINTN CurrYPos, UINTN *TimeOut);
extern VOID    ClearScreen( UINT8 Attrib );
extern EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection);
extern VOID    *SaveScreen( VOID );

#if TSE_BUILD > 0x1206
BOOLEAN 
IsPasswordSupportNonCaseSensitive(
);
VOID 
UpdatePasswordToNonCaseSensitive(
        CHAR16 *Password, 
        UINTN PwdLength
);
#endif

VOID
EFIAPI
HddNotificationFunction (
    EFI_EVENT   Event,
    VOID *HddRegContext
);

EFI_STATUS
IDEPasswordUpdateAllHddWithValidatedPsw (
    UINT8   *Password,
    VOID    *Ptr,
    BOOLEAN bCheckUser
);

VOID
IDEPasswordCheck (
    VOID
);

UINT16
InitHddSecurityInternalDataPtr(
);

VOID* 
IDEPasswordGetDataPtr( 
        UINTN Index
);

BOOLEAN
HddPasswordGetDeviceName (
    EFI_HANDLE Controller,
    CHAR16 **wsName
);

BOOLEAN
CheckSecurityStatus (
    AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol,
    BOOLEAN                     *Locked,
    UINT16                      Mask
);

EFI_STATUS
IDEPasswordAuthenticateHdd (
    CHAR16      *Password,
    VOID        * Ptr,
    BOOLEAN     bCheckUser
);

VOID
IDEUpdateConfig (
    VOID  *TempideSecConfig,
    UINTN value
);

VOID IDEUpdateConfigAllHdd(
    VOID  *TempideSecConfig,
    UINTN value 
);

VOID
SearchTseHardDiskField (
    IN  OUT BOOLEAN *pbCheckUser,
    IN  OUT BOOLEAN *pEnabledBit,
    IN  OUT UINT8   *pHardDiskNumber,
    IN  VOID        *data
);

VOID
EfiStrCpy (
    IN CHAR16   *Destination,
    IN CHAR16   *Source
);

UINTN
EfiStrLen (
    IN CHAR16 *String
);

extern  VOID    
TSEIDEPasswordCheck (
);

VOID
HddSecuritySignalProtocolEvent (
    IN  EFI_GUID    *ProtocolGuid
);

EFI_STATUS
IDEPasswordAuthenticate (
    CHAR16  *Password,
    VOID*   Ptr,
    BOOLEAN bCheckUser
);

EFI_STATUS
EfiLibReportStatusCode (
    IN EFI_STATUS_CODE_TYPE     Type,
    IN EFI_STATUS_CODE_VALUE    Value,
    IN UINT32                   Instance,
    IN EFI_GUID                 *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

VOID *
EfiLibAllocateZeroPool (
    IN  UINTN   AllocationSize
);

UINTN
SPrint (
    OUT CHAR16        *Buffer,
    IN  UINTN         BufferSize,
    IN  CONST CHAR16  *Format,
  ...
  );

#endif 

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************