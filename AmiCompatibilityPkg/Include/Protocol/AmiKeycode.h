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
  AMI_EFIKEYCODE_PROTOCOL definition

  @note: Currently this file is used both in Aptio and EDK files (MiniSetup.h)
*/

#ifndef __AMI_KEYCODE_PROTOCOL_H__
#define __AMI_KEYCODE_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h> 
#include <KeyboardCommonDefinitions.h>

#ifndef GUID_VARIABLE_DECLARATION
#define GUID_VARIABLE_DECLARATION(Variable, Guid) extern EFI_GUID Variable
#endif

#define AMI_EFIKEYCODE_PROTOCOL_GUID \
    {0x0adfb62d, 0xff74, 0x484c, 0x89, 0x44, 0xf8, 0x5c, 0x4b, 0xea, 0x87, 0xa8}

#define AMI_MULTI_LANG_SUPPORT_PROTOCOL_GUID \
    {0xb295bd1c, 0x63e3, 0x48e3, 0xb2, 0x65, 0xf7, 0xdf, 0xa2, 0x7, 0x1, 0x23}

GUID_VARIABLE_DECLARATION(gAmiEfiKeycodeProtocolGuid,AMI_EFIKEYCODE_PROTOCOL_GUID);
GUID_VARIABLE_DECLARATION(gAmiMultiLangSupportProtocolGuid,AMI_MULTI_LANG_SUPPORT_PROTOCOL_GUID);

#ifndef GUID_VARIABLE_DEFINITION

#include <Protocol/SimpleTextInEx.h>

typedef	struct _AMI_EFIKEYCODE_PROTOCOL AMI_EFIKEYCODE_PROTOCOL;
typedef struct _AMI_MULTI_LANG_SUPPORT_PROTOCOL AMI_MULTI_LANG_SUPPORT_PROTOCOL;

typedef struct {
    EFI_INPUT_KEY Key;
    EFI_KEY_STATE KeyState;
    EFI_KEY EfiKey;
    UINT8 EfiKeyIsValid;
    UINT8 PS2ScanCode;
    UINT8 PS2ScanCodeIsValid;
} AMI_EFI_KEY_DATA;

typedef EFI_STATUS (EFIAPI *AMI_READ_EFI_KEY) (
    IN AMI_EFIKEYCODE_PROTOCOL *This,
    OUT AMI_EFI_KEY_DATA *KeyData
);

struct _AMI_EFIKEYCODE_PROTOCOL {
    EFI_INPUT_RESET_EX Reset;
    AMI_READ_EFI_KEY ReadEfikey;
    EFI_EVENT WaitForKeyEx;
    EFI_SET_STATE SetState;
    EFI_REGISTER_KEYSTROKE_NOTIFY RegisterKeyNotify;
    EFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
}; // AMI_EFIKEYCODE_PROTOCOL;

typedef EFI_STATUS (EFIAPI *EFI_KEYBOARD_LAYOUT_MAP)(
    IN      AMI_MULTI_LANG_SUPPORT_PROTOCOL *This,
    IN  OUT AMI_EFI_KEY_DATA *Keydata );

struct _AMI_MULTI_LANG_SUPPORT_PROTOCOL {
    EFI_KEYBOARD_LAYOUT_MAP     KeyboardLayoutMap;
}; // AMI_MULTI_LANG_SUPPORT_PROTOCOL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#endif // #ifndef GUID_VARIABLE_DEFINITION
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
