//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"  
#include <Efi.h>
#include <Protocol/SimpleTextIn.h>

#else //#ifdef TSE_FOR_APTIO_4_50

#include "minisetup.h"

#endif //#ifdef TSE_FOR_APTIO_4_50

#include "commonoem.h"		
#include "HotkeyBin.h"
#include "HotKeyElinks.h"

// Build time file generated from AMITSE_OEM_HEADER_LIST elink.
#include "AMITSEOem.h"		

#ifdef TSE_FOR_APTIO_4_50
#include "AMITSEStrTokens.h"
#else
#include STRING_DEFINES_FILE
#endif

#ifdef TSE_FOR_APTIO_4_50
#ifndef SCAN_F1
#define SCAN_F1         EFI_SCAN_F1
#endif
#ifndef SCAN_F2
#define SCAN_F2         EFI_SCAN_F2
#endif
#ifndef SCAN_F3
#define SCAN_F3         EFI_SCAN_F3
#endif
#ifndef SCAN_F4
#define SCAN_F4         EFI_SCAN_F4
#endif
#ifndef SCAN_F5
#define SCAN_F5         EFI_SCAN_F5
#endif
#ifndef SCAN_F6
#define SCAN_F6         EFI_SCAN_F6
#endif
#ifndef SCAN_F7
#define SCAN_F7         EFI_SCAN_F7
#endif
#ifndef SCAN_F8
#define SCAN_F8         EFI_SCAN_F8
#endif
#ifndef SCAN_F9
#define SCAN_F9         EFI_SCAN_F9
#endif
#ifndef SCAN_F10
#define SCAN_F10        EFI_SCAN_F10
#endif
#ifndef SCAN_F11
#define SCAN_F11        EFI_SCAN_F11
#endif
#ifndef SCAN_F12
#define SCAN_F12        EFI_SCAN_F12
#endif
#ifndef SCAN_ESC
#define SCAN_ESC        EFI_SCAN_ESC
#endif
#endif

HOTKEY_TEMPLATE gHotKeyInfo[] = { HOTKEY_LIST };
UINT32 gHotKeyCount = sizeof(gHotKeyInfo) / sizeof(HOTKEY_TEMPLATE);

#if MINISETUP_MOUSE_SUPPORT
UINT16 gStrNavToken = STRING_TOKEN(STR_NAV_STRINGS_3);
#else
UINT16 gStrNavToken = STRING_TOKEN(STR_NAV_STRINGS_2);
#endif	


//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
