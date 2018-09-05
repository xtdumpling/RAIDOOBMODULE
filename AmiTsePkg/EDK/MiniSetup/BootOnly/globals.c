//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/globals.c $
//
// $Author: Arunsb $
//
// $Revision: 6 $
//
// $Date: 1/24/12 4:24a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file globals.c
    file contains code to take care of the globals in boot only

**/

#include "minisetup.h"

PROGRESSBAR_INFO	*gProgress;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
EFI_GRAPHICS_OUTPUT_PROTOCOL    *gGOP = NULL;
#else
EFI_UGA_DRAW_PROTOCOL           *gUgaDraw = NULL;
#endif //SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL

#ifndef STANDALONE_APPLICATION
EFI_CONSOLE_CONTROL_PROTOCOL	*gConsoleControl = NULL;
#endif //STANDALONE_APPLICATION

SCREEN_BUFFER	*gActiveBuffer;
SCREEN_BUFFER	*gFlushBuffer;

EFI_HANDLE	gImageHandle;
VOID*		gHiiHandle;

BOOLEAN		gPostMsgProtocolActive = TRUE;
BOOLEAN		gSetupContextActive = FALSE;
UINT8		gPostMgrAttribute = 0;

#if 0
// This adds no code space but allows the StrGather utility to know that these tokens are used
STRING_TOKEN(STR_MAIN_TITLE);
STRING_TOKEN(STR_MAIN_COPYRIGHT);
STRING_TOKEN(STR_HELP_TITLE);
STRING_TOKEN(STR_EVAL_MSG);
#endif // 0

UINT8	*gApplicationData;

#ifdef USE_DEPRICATED_INTERFACE
UINT8	*STRING_ARRAY_NAME;
#endif //USE_DEPRICATED_INTERFACE

NVRAM_VARIABLE	*gVariableList = NULL;
NVRAM_VARIABLE	*gFailsafeDefaults = NULL;
NVRAM_VARIABLE	*gOptimalDefaults = NULL;

BOOLEAN gEnterSetup = FALSE;
BOOLEAN gEnterBoot = FALSE;
EFI_EVENT gKeyTimer = NULL;
EFI_EVENT gClickTimer = NULL;
BOOLEAN CompleteReDrawFlag = FALSE; 
UINT32 gPasswordType = AMI_PASSWORD_NONE;
BOOLEAN gPostPasswordFlag = FALSE; 
BOOLEAN ActivateInputDone = FALSE;
UINTN gPostStatus = TSE_POST_STATUS_BEFORE_POST_SCREEN;

UINTN gMaxRows = MAX_ROWS;
UINTN gMaxCols = MAX_COLS;

UINTN gMaxBufX;
UINTN gMaxBufY;

UINTN gPostManagerHandshakeCallIndex = 0;

#define EFI_DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID  \
  { 0x3c4ead08, 0x45ae, 0x4315, 0x8d, 0x15, 0xa6, 0x0e, 0xaa, 0x8c, 0xaf, 0x69 }

#define EFI_DEFAULT_BOOT_ORDER_VARIABLE_GUID  \
  { 0x45cf35f6, 0x0d6e, 0x4d04, 0x85, 0x6a, 0x03, 0x70, 0xa5, 0xb1, 0x6f, 0x53 }

#define EFI_DEFAULT_DRIVER_ORDER_VARIABLE_GUID  \
  { 0xde788bed, 0xb6e2, 0x4290, 0xbd, 0xc8, 0x2a, 0xbb, 0x65, 0xd6, 0x21, 0x78 }

EFI_GUID EfiDefaultLegacyDevOrderGuid = EFI_DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
EFI_GUID EfiDefaultBootOrderGuid = EFI_DEFAULT_BOOT_ORDER_VARIABLE_GUID;
EFI_GUID EfiDefaultDriverOrderGuid = EFI_DEFAULT_DRIVER_ORDER_VARIABLE_GUID;

CHAR16 gPostStatusArray[12][100] = {
					  L"TSE_POST_STATUS_BEFORE_POST_SCREEN",
					  L"TSE_POST_STATUS_IN_POST_SCREEN",
					  L"TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN",
					  L"TSE_POST_STATUS_IN_BOOT_TIME_OUT",
					  L"TSE_POST_STATUS_ENTERING_TSE",
					  L"TSE_POST_STATUS_IN_TSE",
					  L"TSE_POST_STATUS_IN_BBS_POPUP",
					  L"TSE_POST_STATUS_PROCEED_TO_BOOT",
					  L"TSE_POST_STATUS_IN_FRONT_PAGE",
					  L"TSE_POST_STATUS_NO_BOOT_OPTION_FOUND",
					  L"TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED",
					  L"TSE_POST_STATUS_ALL_PLATFORM_RECOVERY_OPTIONS_FAILED"
					};

//Globals for Debug prints
UINT16 gDbgPrint = TSE_DEBUG_MESSAGES;
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
