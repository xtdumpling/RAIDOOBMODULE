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
  HotKeys protocol definition
*/

#ifndef __HOT_KEYS_PROTOCOL_H__
#define __HOT_KEYS_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

#define EFI_HOT_KEYS_PROTOCOL_GUID \
	{0xf1e48287, 0x3fe1, 0x4535, 0x89, 0xab, 0x48, 0xd6, 0xc3, 0xda, 0x27, 0x59}

GUID_VARIABLE_DECLARATION(gEfiHotKeysProtocolGuid,EFI_HOT_KEYS_PROTOCOL_GUID);

//
// Keyboard extension functions definitions
//
typedef EFI_STATUS (*KEY_EXTENDED_FUNC) (VOID* Context);

typedef struct _KEY_ASSOCIATION {
	UINT8				KeyCode;
	UINT8				KeyAttrib;
	BOOLEAN				ReportKey;
	KEY_EXTENDED_FUNC	KeyExtendedFunc;
	VOID*				FunctionContext;
} KEY_ASSOCIATION;

//
// Forward declaration
//

typedef EFI_STATUS (EFIAPI *REGISTER_HOT_KEY) (
	IN KEY_ASSOCIATION *HotKey,
	IN BOOLEAN ReplaceExisting
);

typedef EFI_STATUS (EFIAPI *UNREGISTER_HOT_KEYS) ();

typedef struct _HOT_KEYS_PROTOCOL {
	REGISTER_HOT_KEY RegisterHotKey;
	UNREGISTER_HOT_KEYS UnregisterHotKeys;
} HOT_KEYS_PROTOCOL;

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
