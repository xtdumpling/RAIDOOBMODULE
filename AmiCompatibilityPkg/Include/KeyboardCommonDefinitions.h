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
  This file contains definitions common for HiiKeyboard, SimpleTextIn protocol 
  and Ami keyboard PPI
*/

#ifndef __KEYBOARD_COMMON_DEFINITIONS__H__
#define __KEYBOARD_COMMON_DEFINITIONS__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// Include files from EDKII
// MdePkg:
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Uefi/UefiInternalFormRepresentation.h>

//
// EFI Scan codes 
//
#define EFI_SCAN_NULL		    0x00
#define EFI_SCAN_UP			    0x01
#define EFI_SCAN_DN			    0x02
#define EFI_SCAN_RIGHT		    0x03
#define EFI_SCAN_LEFT		    0x04
#define EFI_SCAN_HOME		    0x05
#define EFI_SCAN_END		    0x06
#define EFI_SCAN_INS		    0x07
#define EFI_SCAN_DEL		    0x08
#define EFI_SCAN_PGUP		    0x09
#define EFI_SCAN_PGDN		    0x0A
#define EFI_SCAN_F1			    0x0B
#define EFI_SCAN_F2			    0x0C
#define EFI_SCAN_F3			    0x0D
#define EFI_SCAN_F4			    0x0E
#define EFI_SCAN_F5			    0x0F
#define EFI_SCAN_F6			    0x10
#define EFI_SCAN_F7			    0x11
#define EFI_SCAN_F8			    0x12
#define EFI_SCAN_F9			    0x13
#define EFI_SCAN_F10		    0x14
#define EFI_SCAN_F11		    0x15
#define EFI_SCAN_F12		    0x16
#define EFI_SCAN_ESC		    0x17

#define SHIFT_STATE_VALID       0x80000000
#define RIGHT_SHIFT_PRESSED     0x00000001
#define LEFT_SHIFT_PRESSED      0x00000002
#define RIGHT_CONTROL_PRESSED   0x00000004
#define LEFT_CONTROL_PRESSED    0x00000008
#define RIGHT_ALT_PRESSED       0x00000010
#define LEFT_ALT_PRESSED        0x00000020
#define RIGHT_LOGO_PRESSED      0x00000040
#define LEFT_LOGO_PRESSED       0x00000080
#define MENU_KEY_PRESSED        0x00000100
#define SYS_REQ_PRESSED         0x00000200

#define TOGGLE_STATE_VALID      0x80
#define SCROLL_LOCK_ACTIVE      0x01
#define NUM_LOCK_ACTIVE         0x02
#define CAPS_LOCK_ACTIVE        0x04
#define KEY_STATE_EXPOSED       0x40

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
