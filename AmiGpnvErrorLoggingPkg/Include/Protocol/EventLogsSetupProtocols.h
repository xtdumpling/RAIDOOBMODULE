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


/** @file EventLogsSetupProtocols.h
    EventLogsSetupProtocols definitions

**/

#ifndef _EFI_EVENT_LOGS_SETUP_PROTOCOLS_H_
#define _EFI_EVENT_LOGS_SETUP_PROTOCOLS_H_

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE ABOVE THIS LINE *******/

//---------------------------------------------------------------------------

#include <Token.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>

//---------------------------------------------------------------------------

#define EVENT_LOGS_SETUP_HII_HANDLE_GUID { 0x55cf7cce, 0x051f, 0x42c3, { 0x9c, 0xa9, 0x20, 0xdb, 0x5c, 0xad, 0x9a, 0xe4 } }

// Protocol and GUID for providing HII handle for event logs page

#ifndef GUID_VARIABLE_DEFINITION

typedef struct _EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL;

typedef struct _EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL {
  EFI_HII_HANDLE    HiiHandle;
  ERROR_MANAGER*    BufferPtr;
} EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL;

// Protocol and GUID for providing links to dynamic information logs

typedef struct _EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL	EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL;

typedef struct _EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL {
  EFI_HII_ACCESS_FORM_CALLBACK CallbackFunction;
  UINT8                        NumberInDynamicList;
  STRING_REF                   TitleStringReference;
} EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL;

EFI_GUID gAmiEventLogsHiiHandleGuid;
EFI_GUID gAmiEventLogsDynamicGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#endif // #ifndef GUID_VARIABLE_DEFINITION
#ifdef __cplusplus
}
#endif
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
