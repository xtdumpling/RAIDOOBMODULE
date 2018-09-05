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

#ifndef __CONSOLE_CONTROL_PROTOCOL_H__
#define __CONSOLE_CONTROL_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

#define EFI_CONSOLE_CONTROL_PROTOCOL_GUID \
	{ 0xf42f7782, 0x12e, 0x4c12, {0x99, 0x56, 0x49, 0xf9, 0x43, 0x4, 0xf7, 0x21} }

GUID_VARIABLE_DECLARATION(gEfiConsoleControlProtocolGuid,EFI_CONSOLE_CONTROL_PROTOCOL_GUID);

// {EF9A3971-C1A0-4a93-BD40-5AA165F2DC3A}
#define CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID \
	{ 0xef9a3971, 0xc1a0, 0x4a93, {0xbd, 0x40, 0x5a, 0xa1, 0x65, 0xf2, 0xdc, 0x3a} }

GUID_VARIABLE_DECLARATION(gAmiConOutStartedProtocolGuid,CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID);

// {2DF1E051-906D-4eff-869D-24E65378FB9E}
#define CONSOLE_IN_DEVICES_STARTED_PROTOCOL_GUID \
	{ 0x2df1e051, 0x906d, 0x4eff, {0x86, 0x9d, 0x24, 0xe6, 0x53, 0x78, 0xfb, 0x9e} }

GUID_VARIABLE_DECLARATION(gAmiConInStartedProtocolGuid,CONSOLE_IN_DEVICES_STARTED_PROTOCOL_GUID);

typedef struct _EFI_CONSOLE_CONTROL_PROTOCOL EFI_CONSOLE_CONTROL_PROTOCOL;

typedef enum {
	EfiConsoleControlScreenText,
	EfiConsoleControlScreenGraphics,
	EfiConsoleControlScreenMaxValue
} EFI_CONSOLE_CONTROL_SCREEN_MODE;

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE)(
	IN EFI_CONSOLE_CONTROL_PROTOCOL *This,
	OUT EFI_CONSOLE_CONTROL_SCREEN_MODE *Mode,
	OUT BOOLEAN *UgaExists OPTIONAL, 
	OUT BOOLEAN *StdInLocked OPTIONAL
);

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE)(
	IN EFI_CONSOLE_CONTROL_PROTOCOL *This,
	OUT EFI_CONSOLE_CONTROL_SCREEN_MODE Mode
);

typedef EFI_STATUS (EFIAPI *EFI_CONSOLE_CONTROL_PROTOCOL_LOCK_STD_IN)(
	IN EFI_CONSOLE_CONTROL_PROTOCOL *This, IN CHAR16 *Password
);

struct _EFI_CONSOLE_CONTROL_PROTOCOL {
	EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE GetMode;
	EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE SetMode;
	EFI_CONSOLE_CONTROL_PROTOCOL_LOCK_STD_IN LockStdIn;
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
