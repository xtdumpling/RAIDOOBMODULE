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


/** @file SmbiosElogSetupScreen.h
    SmbiosElogSetupScreen Header file - function declarations, structures,
    constants

**/

#ifndef _SMBIOS_ELOG_SETUP_SCREEN_H_
#define _SMBIOS_ELOG_SETUP_SCREEN_H_

//---------------------------------------------------------------------------

#include "StatusCodes.h"

#include <Setup.h>
#include "EventLogsSetupPageStrTokens.h"
#include "Token.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <EventLogsAutoId.h>


#include <UefiHii.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>

#include <Protocol/SmbiosElogSupport.h>
#include <Protocol/GpnvRedirProtocol.h>
#include <Protocol/EventLogsSetupProtocols.h>
#include <Protocol/AmiSmbios.h>

//---------------------------------------------------------------------------

// Global declarations

#define BLANK L"                    "
#define ERROR_COLUMN_WIDTH          15
#define ERROR_CODE_COLUMN           21
#define ERROR_SEVERITY_COLUMN       36

#define MAX_ERRORS                  500
#define STRING_BUFFER_LENGTH        4000
#define MAX_TOKENS_TO_SAVE          0x9ff

typedef struct _EFI_ERROR_SETUP_SCREEN_TOKEN_LOOKUP {
  BOOLEAN                       IsDetail;
  STRING_REF                    Token;
  IN EFI_GPNV_ERROR_EVENT       Event;
  IN EFI_STATUS_CODE_TYPE       CodeType;
  IN EFI_STATUS_CODE_VALUE      Value;
} EFI_ERROR_SETUP_SCREEN_TOKEN_LOOKUP;

typedef struct _EFI_ERROR_MANAGER_INSTANCE {
  EFI_HII_HANDLE                        HiiHandle;
  EFI_HII_STRING_PROTOCOL               *HiiString;
  EFI_HII_DATABASE_PROTOCOL             *HiiDatabase;
  VOID                                  *SetupScreenBuffer;
  EFI_ERROR_SETUP_SCREEN_TOKEN_LOOKUP   SavedTokens[MAX_TOKENS_TO_SAVE];
  UINT32                                TokenCount;
} EFI_ERROR_SETUP_SCREEN_INSTANCE;

typedef struct {
  UINT32                  Value;
  STRING_REF              Token;
} STATUS_CODE_LOOKUP_TABLE;

typedef struct {
  UINT32                  Value;
  BOOLEAN                 IsCustomToken;
  STRING_REF              Token;
} OPERATION_LOOKUP_TABLE;

// Internal function declarations

EFI_STATUS
InstallErrorSetupScreen (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
);

EFI_STATUS
UpdateFormSetWithDataHub ( VOID );

EFI_STATUS
ErrorScreenCallbackRoutine(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);


EFI_STATUS
StatusCodeMatchString (
  IN  STATUS_CODE_LOOKUP_TABLE  *Table,
  IN  UINT32                    Value,
  OUT STRING_REF                *Token
);

EFI_STATUS
StatusCodeMatchOperation (
  IN  UINT32              Value,
  OUT BOOLEAN             *IsCustom,
  OUT STRING_REF          *OperationToken
);

EFI_STATUS
BuildErrorDetails (
  IN EFI_GPNV_ERROR_EVENT        *EventPtr,
  IN UINT8                       *ExtendedData,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  OUT STRING_REF                 *Token
);

EFI_STATUS
BuildErrorDescription (
  IN EFI_GPNV_ERROR_EVENT        *EventPtr,
  IN UINT8                       *ExtendedData,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  OUT STRING_REF                 *Token
);

EFI_STATUS
EFIAPI
GpnvHiiGetString (
  IN        EFI_STRING_ID  StringId, 
  OUT       EFI_STRING     String,
  IN        UINTN          *StringSize,
  IN  CONST CHAR8          *Language  OPTIONAL
);

#endif

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
