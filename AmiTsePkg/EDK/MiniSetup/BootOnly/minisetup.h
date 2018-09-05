//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/minisetup.h $
//
// $Author: Premkumara $
//
// $Revision: 44 $
//
// $Date: 5/28/12 5:39a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file minisetup.h
    Main header file takes care of TSE includes.

**/

#ifndef _MINISETUP_H_
#define	_MINISETUP_H_

#ifdef TSE_FOR_APTIO_4_50
#include <Token.h>
#else
#include "tokens.h"
#endif


#ifndef SETUP_USE_GUIDED_SECTION
#define SETUP_USE_GUIDED_SECTION 0
#endif

#ifndef SETUP_JPEG_LOGO_SUPPORT
#define SETUP_JPEG_LOGO_SUPPORT 0
#endif

#ifndef SETUP_PCX_LOGO_SUPPORT
#define SETUP_PCX_LOGO_SUPPORT 0
#endif

#ifndef SETUP_GIF_LOGO_SUPPORT
#define SETUP_GIF_LOGO_SUPPORT 0
#endif

#ifndef TSE_BOOT_NOW_IN_BOOT_ORDER
#define TSE_BOOT_NOW_IN_BOOT_ORDER 0
#endif

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 

#include "EfiDriverLib.h"
#include "EfiPrintLib.h"

#include EFI_PROTOCOL_DEFINITION(FirmwareVolume)
#include EFI_PROTOCOL_DEFINITION(SimpleFileSystem)
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
#include EFI_PROTOCOL_DEFINITION(GraphicsOutput)
// Between GOP and UGA draw protocols blt buffer structure remains
// the same. The difference is only in the name so we can safely
// typedef the new structure to old structure's name.
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_UGA_PIXEL;
#else
#include EFI_PROTOCOL_DEFINITION(UgaDraw)
#endif
#include EFI_PROTOCOL_DEFINITION(ConsoleControl)
#include EFI_PROTOCOL_DEFINITION(DevicePath)
#include EFI_PROTOCOL_DEFINITION(ComponentName)
#include EFI_PROTOCOL_DEFINITION(LegacyBios)
#include EFI_PROTOCOL_DEFINITION(LoadedImage)
#include EFI_PROTOCOL_DEFINITION(FileInfo)
#if EFI_SPECIFICATION_VERSION<=0x20000
#include EFI_PROTOCOL_DEFINITION(Hii)
#endif

#if SETUP_ITK_COMPATIBILITY
#include "..\ITK\OemBadgingSupport\EfiOEMBadging.h"
#else
#include EFI_PROTOCOL_DEFINITION(EfiOEMBadging)
#endif
#include EFI_GUID_DEFINITION(Bmp)
INTN MemCmp( UINT8 *dest, UINT8 *src, UINTN size );

#define MSG_USB_WWID_CLASS_DP           MSG_USB_WWID_DP
#define MSG_USB_LOGICAL_UNIT_CLASS_DP   MSG_DEVICE_LOGICAL_UNIT_DP
#define MSG_USB_SATA_DP			        MSG_SATA_DP
#define MSG_USB_ISCSI_DP		        MSG_ISCSI_DP

#else //TSE_USE_EDK_LIBRARY

#include "Efi.h" 

#include "Protocol/FirmwareVolume.h"
#include "Protocol/SimpleFileSystem.h"
#include "Protocol/SimpleTextOut.h"
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
#include "Protocol/GraphicsOutput.h"
	// Between GOP and UGA draw protocols blt buffer structure remains
	// the same. The difference is only in the name so we can safely
	// typedef the new structure to old structure's name.
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_UGA_PIXEL;
#else
#include "UgaDraw.h"
#endif

#include "Protocol/ConsoleControl.h"
#include "Protocol/DevicePath.h"
#include "Protocol/ComponentName.h"
//#include "LegacyBios.h"
#include "Protocol/LoadedImage.h"


#if SETUP_ITK_COMPATIBILITY
#include "..\ITK\OemBadgingSupport\EfiOEMBadging.h"
#else
#include "Protocol/EfiOemBadging.h" 
#endif

#include "Protocol/SimpleTextInEx.h"
//#include "LegacyBios.h"
#include "AmiDxeLib.h"  
#define EFI_DRIVER_ENTRY_POINT(a)

#include "EDKhelper.h"  

// EfiDriverlib
VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

VOID
EfiStrCpy (
  IN CHAR16   *Destination,
  IN CHAR16   *Source
  );
UINTN
EfiDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  );
VOID *
EfiLibAllocatePool (
  IN  UINTN   AllocationSize
  );
EFI_STATUS
EFIAPI
TseEfiCreateEventReadyToBoot (
  IN EFI_TPL                      NotifyTpl,
  IN EFI_EVENT_NOTIFY             NotifyFunction,
  IN VOID                         *NotifyContext,
  OUT EFI_EVENT                   *ReadyToBootEvent
  );
EFI_STATUS
EfiLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );
EFI_DEVICE_PATH_PROTOCOL *
EfiFileDevicePath (
  IN EFI_HANDLE               Device  OPTIONAL,
  IN CHAR16                   *FileName
  );
EFI_STATUS
EfiLibNamedEventSignal (
  IN EFI_GUID            *Name
  );
EFI_DEVICE_PATH_PROTOCOL *
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );

EFI_DEVICE_PATH_PROTOCOL        *
EfiAppendDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );

VOID
EFIAPI
TseEfiInitializeFwVolDevicepathNode (
  IN  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH     *FvDevicePathNode,
  IN EFI_GUID                               *NameGuid
  );

UINTN
SPrint (
  OUT CHAR16        *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *Format,
  ...
  );
EFI_STATUS
EfiInitializeDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

VOID SetupDebugPrint(IN CONST CHAR8  *Format, ...) ;

extern EFI_SYSTEM_TABLE 	*gST;
extern EFI_BOOT_SERVICES 	*gBS;
extern EFI_RUNTIME_SERVICES 	*gRT;

#define EFI_TPL_APPLICATION 	TPL_APPLICATION
#define EFI_TPL_CALLBACK    	TPL_CALLBACK
#define EFI_TPL_NOTIFY      	TPL_NOTIFY
#define EFI_TPL_HIGH_LEVEL  	TPL_HIGH_LEVEL

#define	DYNAMIC_PAGE_GROUPS_SIZE	8

#ifndef BBS_TYPE_FLOPPY
#define BBS_TYPE_FLOPPY           	BBS_FLOPPY
#endif
#ifndef BBS_TYPE_HARDDRIVE
#define BBS_TYPE_HARDDRIVE        	BBS_HARDDISK
#endif
#ifndef BBS_TYPE_CDROM
#define BBS_TYPE_CDROM            	BBS_CDROM
#endif
#ifndef BBS_TYPE_PCMCIA
#define BBS_TYPE_PCMCIA           	BBS_PCMCIA
#endif
#ifndef BBS_TYPE_USB
#define BBS_TYPE_USB              	BBS_USB
#endif
#ifndef BBS_TYPE_EMBEDDED_NETWORK
#define BBS_TYPE_EMBEDDED_NETWORK 	BBS_EMBED_NETWORK
#endif
#ifndef BBS_TYPE_BEV
#define BBS_TYPE_BEV              	BBS_BEV_DEVICE
#endif
#ifndef BBS_TYPE_UNKNOWN
#define BBS_TYPE_UNKNOWN          	BBS_UNKNOWN
#endif

#ifndef SCAN_NULL
#define SCAN_NULL       EFI_SCAN_NULL
#endif
#ifndef SCAN_UP
#define SCAN_UP         EFI_SCAN_UP
#endif
#ifndef SCAN_DOWN
#define SCAN_DOWN       EFI_SCAN_DN
#endif
#ifndef SCAN_RIGHT
#define SCAN_RIGHT      EFI_SCAN_RIGHT
#endif
#ifndef SCAN_LEFT
#define SCAN_LEFT       EFI_SCAN_LEFT
#endif
#ifndef SCAN_HOME
#define SCAN_HOME       EFI_SCAN_HOME
#endif
#ifndef SCAN_END
#define SCAN_END        EFI_SCAN_END
#endif
#ifndef SCAN_INSERT
#define SCAN_INSERT     EFI_SCAN_INS
#endif
#ifndef SCAN_DELETE
#define SCAN_DELETE     EFI_SCAN_DEL
#endif
#ifndef SCAN_PAGE_UP
#define SCAN_PAGE_UP    EFI_SCAN_PGUP
#endif
#ifndef SCAN_PAGE_DOWN
#define SCAN_PAGE_DOWN  EFI_SCAN_PGDN
#endif
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

#define CHAR_NULL             0x0000
#define CHAR_BACKSPACE        0x0008
#define CHAR_TAB              0x0009
#define CHAR_LINEFEED         0x000A
#define CHAR_CARRIAGE_RETURN  0x000D
#define GLYPH_WIDTH         8
#define GLYPH_HEIGHT        19
#define NARROW_CHAR         0xFFF0
#define WIDE_CHAR           0xFFF1
#define NON_BREAKING_CHAR   0xFFF2

// BMP
//
// Definitions for BMP files
//
#pragma pack(1)

typedef struct {
  UINT8   Blue;
  UINT8   Green;
  UINT8   Red;
  UINT8   Reserved;
} BMP_COLOR_MAP;

typedef struct {
  CHAR8         CharB;
  CHAR8         CharM;
  UINT32        Size;
  UINT16        Reserved[2];
  UINT32        ImageOffset;
  UINT32        HeaderSize;
  UINT32        PixelWidth;
  UINT32        PixelHeight;
  UINT16        Planes;       // Must be 1
  UINT16        BitPerPixel;  // 1, 4, 8, or 24
  UINT32        CompressionType;
  UINT32        ImageSize;    // Compressed image size in bytes
  UINT32        XPixelsPerMeter;
  UINT32        YPixelsPerMeter;
  UINT32        NumberOfColors;
  UINT32        ImportantColors;
} BMP_IMAGE_HEADER;

#pragma pack()

#define EFI_DEFAULT_BMP_LOGO_GUID \
  {0x7BB28B99,0x61BB,0x11d5,0x9A,0x5D,0x00,0x90,0x27,0x3F,0xC1,0x4D}

extern EFI_GUID gEfiDefaultBmpLogoGuid;

///DEVICE PATH Definitions
#ifndef MSG_USB_WWID_CLASS_DP
#define MSG_USB_WWID_CLASS_DP		0x10
#endif
#ifndef MSG_USB_LOGICAL_UNIT_CLASS_DP
#define MSG_USB_LOGICAL_UNIT_CLASS_DP	0x11
#endif
#ifndef MSG_USB_SATA_DP
#define MSG_USB_SATA_DP			0x12
#endif
#ifndef MSG_USB_ISCSI_DP
#define MSG_USB_ISCSI_DP		0x13
#endif
#endif //TSE_USE_EDK_LIBRARY

//USB Class devices - Device path..
#define USB_PHY_DEV_CLASS 		0x05
#define USB_MASS_DEV_CLASS 		0x08
#ifndef MSG_URI_DP
#define MSG_URI_DP		0x18
#endif

#ifndef MSG_UFS_DP
#define MSG_UFS_DP                0x19
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT8                           Pun;
  UINT8                           Lun;
} UFS_DEVICE_PATH;
#endif

#ifndef MSG_SD_DP
#define MSG_SD_DP                 0x1A
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT8                           SlotNumber;
} SD_DEVICE_PATH;
#endif


#ifndef MEDIA_RAM_DISK_DP
#define MEDIA_RAM_DISK_DP         0x09
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT32                          StartingAddr[2];
  UINT32                          EndingAddr[2];
  EFI_GUID                        TypeGuid;
  UINT16                          Instance;
} MEDIA_RAM_DISK_DEVICE_PATH;
#endif

extern EFI_GUID  gEfiGlobalVariableGuid;

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#if TSE_USE_EDK_LIBRARY
#if EFI_SPECIFICATION_VERSION>=0x2000A
#define __UEFI_HII__H__
#include<EfiHii.h>
#else
#define __HII_PROTOCOL_H__
#endif
#endif //TSE_USE_EDK_LIBRARY
#include "Protocol/AmiKeycode.h"  
#else
#ifndef _AMI_EFI_KEY_DATA_
#define _AMI_EFI_KEY_DATA_
typedef struct {
  EFI_INPUT_KEY  Key;
} AMI_EFI_KEY_DATA;
#endif
#endif

#ifdef TSE_FOR_APTIO_4_50
#include "Include/Protocol/AMIPostMgr.h"
#include "Include/Protocol/EsaTseInterfaces.h"
#else
#include "AMIPostMgr.h"
#endif

#include "amiver.h"

//handling version macros as tokens
//#define	TSE_MAJOR	0x02
//#define	TSE_MINOR	0x00
//#define	TSE_BUILD	0x1201

#define	MINI_SETUP_DATA_GUID		\
	{ 0xFE612B72, 0x203C, 0x47B1, 0x85, 0x60, 0xA6, 0x6D, 0x94, 0x6E, 0xB3, 0x71 }

#include "hiistring.h"
#include "string.h"
#include "protocol.h"
#include "variable.h"
#include "boot.h"
#include "AMILogo/AMILogo.h"  
#include "bootflow.h"
#include "Timer.h"  

#ifdef TSE_FOR_APTIO_4_50
#include "AMITSEStrTokens.h"
#else
#include STRING_DEFINES_FILE
#endif

#ifdef TSE_FOR_APTIO_4_50
#include "Include/AMIVfr.h" 
#else
#include "AMIVFR.h"
#endif

#if TSE_APTIO_5_SUPPORT
#define AMITSE_TEXT(a) 	a
#else
#define AMITSE_TEXT(a) 	L##a
#endif

#include "bbs.h"

///EIP 115082 : Referance to Style module is removed. Managed from Style hooks
//#if TSE_STYLE_SOURCES_SUPPORT
//#include "HotkeyBin.h"
//#include "HotclickBin.h" //EIP:47086 - Right clicking from the Main page is not exiting from BIOS setup.
//#endif

#if TSE_LITE_SOURCES_SUPPORT
#include "frame.h"
#include "Label.h"  
#include "hotkey.h"
#include "hotclick.h"//EIP:47086 - Right clicking from the Main page is not exiting from BIOS setup.
#include "minisetupext.h"
#endif 

#ifndef STRING_TOKEN
#define STRING_TOKEN(t) t
#endif

UINTN StyleGetTextMode( UINTN Rows, UINTN Cols );
VOID StyleUpdateVersionString( VOID );
UINTN StyleGetClearScreenColor(VOID);
UINT8 StyleGetPageLinkColor(VOID);
UINT8 StyleGetScrollBarColor(VOID);
UINT8 StyleGetScrollBarUpArrowColor(VOID);
UINT8 StyleGetScrollBarDownArrowColor(VOID);

UINTN StyleGetStdMaxRows(VOID);
UINTN StyleGetStdMaxCols(VOID);
VOID GetProgressColor(EFI_UGA_PIXEL * BGColor, EFI_UGA_PIXEL * BDRColor, EFI_UGA_PIXEL * FillColor);

#ifndef STYLE_FULL_MAX_ROWS
#define	STYLE_FULL_MAX_ROWS	31
#endif

#ifndef STYLE_FULL_MAX_COLS
#define	STYLE_FULL_MAX_COLS	100
#endif

#ifndef STYLE_STD_MAX_ROWS
#define	STYLE_STD_MAX_ROWS	24
#endif

#ifndef STYLE_STD_MAX_COLS
#define	STYLE_STD_MAX_COLS	80
#endif

#ifndef STYLE_MAX_COLS
#define	STYLE_MAX_COLS	STYLE_FULL_MAX_COLS
#endif

#ifndef STYLE_MAX_ROWS
#define	STYLE_MAX_ROWS	STYLE_FULL_MAX_ROWS
#endif

#ifndef STYLE_USER_MAX_ROWS
#define STYLE_USER_MAX_ROWS 2000
#endif

#ifndef STYLE_USER_MAX_COLS
#define STYLE_USER_MAX_COLS 2000
#endif

#define	MAX_ROWS		STYLE_MAX_ROWS
#define	MAX_COLS		STYLE_MAX_COLS
#define	MAX_DIMENSIONS	(STYLE_USER_MAX_ROWS * STYLE_USER_MAX_COLS)

#include "screen.h"

#include "commonoem.h"
#include "LogoLib.h"
#include "PwdLib.h"
#include "HiiLib.h"
#include "mem.h"
#include "HookAnchor.h"
#if TSE_ADVANCED_SUPPORT
#include "TseElinks.h"
#endif 
#ifdef TSE_FOR_APTIO_4_50
#include "AmiStatusCodes.h"
#else
#include "EfiStatusCode.h"
#endif

#ifndef BBS_TYPE_DEV
#ifdef BBS_TYPE_BEV
#define BBS_TYPE_DEV BBS_TYPE_BEV
#else
#define BBS_TYPE_DEV 0x80
#endif
#endif
/* B1DA0ADF-4F77-4070-A88E-BFFE1C60529A */
#define	MINI_SETUP_GUID	\
	{ 0xB1DA0ADF, 0x4F77, 0x4070, { 0xA8, 0x8E, 0xBF, 0xFE, 0x1C, 0x60, 0x52, 0x9A } }

#define	SETUP_VARIABLE_GUID	\
	{ 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }

/* 47C7B224-C42A-11D2-8E57-00A0C969723B */
#define ENVIRONMENT_VARIABLE_ID  \
  { 0x47c7b224, 0xc42a, 0x11d2, 0x8e, 0x57, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

// End Notifications


// Status code reporting - Defines >> Start
// Defined in AmiStatusCodes.h
// Redefining for Aptio 3.x 
#ifndef AMI_STATUS_CODE_CLASS
#define AMI_STATUS_CODE_CLASS 	EFI_OEM_SPECIFIC //0x8000
#endif

#ifndef AMI_DXE_BS_EC_INVALID_PASSWORD
#define AMI_DXE_BS_EC_INVALID_PASSWORD (AMI_STATUS_CODE_CLASS | 0x00000002)
#endif

#ifndef AMI_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR
#define AMI_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR (AMI_STATUS_CODE_CLASS | 0x00000003)
#endif

#ifndef AMI_DXE_BS_EC_BOOT_OPTION_FAILED
#define AMI_DXE_BS_EC_BOOT_OPTION_FAILED (AMI_STATUS_CODE_CLASS | 0x00000004)
#endif


#ifndef DXE_SETUP_VERIFYING_PASSWORD
#define DXE_SETUP_VERIFYING_PASSWORD (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_VERIFYING_PASSWORD)
#endif

#ifndef DXE_SETUP_START
#define DXE_SETUP_START (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP)
#endif

#ifndef DXE_SETUP_INPUT_WAIT
#define DXE_SETUP_INPUT_WAIT (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_INPUT_WAIT)
#endif

#ifndef DXE_READY_TO_BOOT
#define DXE_READY_TO_BOOT (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT)
#endif

#ifndef DXE_INVALID_PASSWORD
#define DXE_INVALID_PASSWORD (EFI_SOFTWARE_DXE_BS_DRIVER | AMI_DXE_BS_EC_INVALID_PASSWORD)
#endif

#ifndef DXE_BOOT_OPTION_LOAD_ERROR
#define DXE_BOOT_OPTION_LOAD_ERROR (EFI_SOFTWARE_DXE_BS_DRIVER | AMI_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR)
#endif

#ifndef DXE_BOOT_OPTION_FAILED
#define DXE_BOOT_OPTION_FAILED (EFI_SOFTWARE_DXE_BS_DRIVER | AMI_DXE_BS_EC_BOOT_OPTION_FAILED)
#endif
// Status code reporting -Define >> End

#define NG_SIZE 	19
#define WG_SIZE		38

#ifdef EFI_NT_EMULATOR
#define	RUNTIME_DEBUG_SUPPORT
#define	RUNTIME_DEBUG(str)	{ if ( __RuntimeCheckDebugMode( str ) ) _asm int 3 }
#else
#undef RUNTIME_DEBUG_SUPPORT
#define	RUNTIME_DEBUG(str)
#endif

#ifndef STANDALONE_APPLICATION
#if APTIO_4_00 != 1 && SETUP_USE_GUIDED_SECTION !=1
extern SETUP_PKG					_SetupPackage;
#endif
#else
extern UINT8 DummySetupData[];
#endif


extern SCREEN_BUFFER				*gActiveBuffer;
extern SCREEN_BUFFER				*gFlushBuffer;
#ifdef USE_DEPRICATED_INTERFACE
extern UINT8						*STRING_ARRAY_NAME;
#else
#if APTIO_4_00
#else
extern UINT8						STRING_ARRAY_NAME[];
#endif
#endif
extern UINT8						*gApplicationData;
extern EFI_HANDLE					gImageHandle;
extern EFI_COMPONENT_NAME_PROTOCOL	gComponentName;
extern UINT16						gCheckboxTokens[];

extern BOOLEAN						gVariableChanged;
extern BOOLEAN						gResetRequired;
extern BOOLEAN                      gUserTseCacheUpdated;
extern EFI_HANDLE 					*gReconnectPageHandles;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
extern EFI_GRAPHICS_OUTPUT_PROTOCOL	*gGOP;
#else
extern EFI_UGA_DRAW_PROTOCOL		*gUgaDraw;
#endif

#ifndef STANDALONE_APPLICATION
extern EFI_CONSOLE_CONTROL_PROTOCOL	*gConsoleControl;
#endif

//EIP75481  Support TSE debug print infrastructure

#define PRINT_UEFI		        1
#define PRINT_UEFI_CALLBACK	    2
#define PRINT_UEFI_NOTIFICATION 4
#define PRINT_UEFI_PARSE	    8

#if SUPPRESS_PRINT
    #define SETUP_DEBUG_TSE(format,...)
    #define SETUP_DEBUG_UEFI(format,...)
    #define SETUP_DEBUG_UEFI_PARSE(format,...)
    #define SETUP_DEBUG_UEFI_NOTIFICATION(format,...)
    #define SETUP_DEBUG_UEFI_CALLBACK(format,...)
    #define SETUP_DEBUG_VAR(format,...)
#else //Else of SUPPRESS_PRINT
#if BUILD_OS == BUILD_OS_LINUX
    #define SETUP_DEBUG_TSE(format,...) SetupDebugPrint(format, ##__VA_ARGS__)
    #define SETUP_DEBUG_UEFI(format,...) if((gDbgPrint & PRINT_UEFI)== PRINT_UEFI)SetupDebugPrint(format, ##__VA_ARGS__)
    #define SETUP_DEBUG_UEFI_PARSE(format,...) if((gDbgPrint & PRINT_UEFI_PARSE)== PRINT_UEFI_PARSE)SetupDebugPrint(format, ##__VA_ARGS__)
    #define SETUP_DEBUG_UEFI_NOTIFICATION(format,...) if((gDbgPrint & PRINT_UEFI_NOTIFICATION)== PRINT_UEFI_NOTIFICATION)SetupDebugPrint(format, ##__VA_ARGS__)
    #define SETUP_DEBUG_UEFI_CALLBACK(format,...) if((gDbgPrint & PRINT_UEFI_CALLBACK)== PRINT_UEFI_CALLBACK)SetupDebugPrint(format, ##__VA_ARGS__)
    #define SETUP_DEBUG_VAR(format,...) SetupDebugPrint(format, ##__VA_ARGS__)
#else //Else of BUILD_OS == BUILD_OS_LINUX
    #define SETUP_DEBUG_TSE(format,...) SetupDebugPrint(format, __VA_ARGS__)
    #define SETUP_DEBUG_UEFI(format,...) if((gDbgPrint & PRINT_UEFI)== PRINT_UEFI)SetupDebugPrint(format, __VA_ARGS__)
    #define SETUP_DEBUG_UEFI_PARSE(format,...) if((gDbgPrint & PRINT_UEFI_PARSE)== PRINT_UEFI_PARSE)SetupDebugPrint(format, __VA_ARGS__)
    #define SETUP_DEBUG_UEFI_NOTIFICATION(format,...) if((gDbgPrint & PRINT_UEFI_NOTIFICATION)== PRINT_UEFI_NOTIFICATION)SetupDebugPrint(format, __VA_ARGS__)
    #define SETUP_DEBUG_UEFI_CALLBACK(format,...) if((gDbgPrint & PRINT_UEFI_CALLBACK)== PRINT_UEFI_CALLBACK)SetupDebugPrint(format, __VA_ARGS__)
    #define SETUP_DEBUG_VAR(format,...) SetupDebugPrint(format, __VA_ARGS__)
#endif //End of BUILD_OS == BUILD_OS_LINUX
#endif //End of SUPPRESS_PRINT
//EIP75481  


extern BOOLEAN						gQuietBoot;
extern BOOLEAN						gEnterSetup;
extern EFI_EVENT					gKeyTimer;
extern EFI_EVENT					gClickTimer;
extern UINT32						gPasswordType;

extern UINTN 						gPostStatus;

extern BOOT_DATA					*gBootData;

extern BOOT_DATA                    *gCurrLegacyBootData;

extern UINTN						gLangCount;
extern LANGUAGE_DATA				*gLanguages;

extern BOOLEAN						gSetupContextActive;
extern BOOLEAN						gPostMsgProtocolActive;
extern UINT8						gPostMgrAttribute;
extern AMI_VERSION					gVersion_TSE;
extern UINT32						gStartPage;

extern UINTN 						gLabelLeftMargin ;
extern UINTN 						gControlLeftPad ;
extern UINTN						gControlLeftMargin;
extern UINTN						gControlRightAreaWidth;

extern UINTN						gMaxRows;
extern UINTN						gMaxCols;
extern UINTN 						gPostManagerHandshakeCallIndex;
extern UINT16                       gDbgPrint ;

extern EFI_GUID _gBootFlowGuid;
extern BOOLEAN gDoNotBoot;

extern UINTN   gCurrIDESecPage;


#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
extern EFI_GUID gAmiEfiKeycodeProtocolGuid;
#endif

//EIP-28501: To support SETUP_STORE_KEYCODE_PASSWORD options
#define PW_EFI_KEY		1
#define PW_SCAN_CODE	2

#define DATE_STYLE_MMDDYYYY		0
#define DATE_STYLE_YYYYDDMM		1
#define DATE_STYLE_DDMMYYYY		2
#define DATE_STYLE_YYYYMMDD		3

#define	TIMER_ONE_SECOND	(10 * 1000 * 1000)
#define	TIMER_HALF_SECOND	(TIMER_ONE_SECOND / 2)
#define TIMER_TENTH_SECOND  (TIMER_ONE_SECOND / 10)

#define	STRUCT_OFFSET(type, field)	(UINTN)&(((type *)0)->field)

typedef struct _PROGRESSBAR_INFO		//Structure containing global information used by progressbar handler
{
	UINTN			x;					//	ProgressBar Left Position
	UINTN			y;					//	ProgressBar Top Position
	UINTN			w;					//	ProgressBar width
	UINTN			h;					//	ProgressBar Height
	EFI_UGA_PIXEL	*backgroundColor;	//	ProgressBar Background Color
	EFI_UGA_PIXEL	*borderColor;		//	ProgressBar Border Color
	EFI_UGA_PIXEL	*fillColor;			//	ProgressBar Fill Color
	UINTN			delta;				//	Varies	increment
	UINTN			completed;			//	Varies	increment
	BOOLEAN			quiteBootActive;	//  True if quiteBoot is active	
	BOOLEAN			active;				//  True if progressbar has been initialized
}PROGRESSBAR_INFO;

extern PROGRESSBAR_INFO	*gProgress;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// Function Number (0 = First Function).
  ///
  UINT8                           FunctionNumber;
} AMITSE_PCCARD_DEVICE_PATH;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// Firmware file name
  ///
  EFI_GUID                        FvFileName;
} AMITSE_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT32                          Tid;
} AMITSE_I2O_DEVICE_PATH;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// The MAC address for a network interface padded with 0s.
  ///
  EFI_MAC_ADDRESS                 MacAddress;
  ///
  /// Network interface type(i.e. 802.3, FDDI).
  ///
  UINT8                           IfType;
} AMITSE_MAC_ADDR_DEVICE_PATH;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// The HBA port number that facilitates the connection to the
  /// device or a port multiplier. The value 0xFFFF is reserved.
  ///
  UINT16                          HBAPortNumber;
  ///
  /// The Port multiplier port number that facilitates the connection
  /// to the device. Bit 15 should be set if the device is directly
  /// connected to the HBA.
  ///
  UINT16                          PortMultiplierPortNumber;
  ///
  /// Logical Unit Number.
  ///
  UINT16                          Lun;
} AMITSE_SATA_DEVICE_PATH;
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// Network Protocol (0 = TCP, 1+ = reserved).
  ///
  UINT16                          NetworkProtocol;
  ///
  /// iSCSI Login Options.
  ///
  UINT16                          LoginOption;
  ///
  /// iSCSI Logical Unit Number.
  ///
  UINT64                          Lun;
  ///
  /// iSCSI Target Portal group tag the initiator intends
  /// to establish a session with.
  ///
  UINT16                          TargetPortalGroupTag;
  ///
  /// iSCSI NodeTarget Name. The length of the name
  /// is determined by subtracting the offset of this field from Length.
  ///
  /// CHAR8                        iSCSI Target Name.
} AMITSE_ISCSI_DEVICE_PATH;

// box.c 
VOID DrawLineWithAttribute( UINTN Col, UINTN Row, UINTN Length, CHAR16 Type, UINT8 Attrib );
VOID DrawLine( UINTN Col, UINTN Row, UINTN Length, CHAR16 Type );
VOID DrawBorder( UINTN Left, UINTN Top, UINTN Width, UINTN Height );
VOID DrawWindow( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib, BOOLEAN Border, BOOLEAN Shadow );
VOID DrawBox( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib );

// buffer.c
EFI_STATUS InitializeScreenBuffer( UINT8 attrib );
VOID SlowFlushLines( UINTN Top, UINTN Bottom );
VOID FlushLines( UINTN Top, UINTN Bottom );
VOID DoRealFlushLines();
void SetDesiredTextMode();


// logo.c
VOID CleanUpExtendedLogoWrapper();
VOID DoLogoAnimateWrapper(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY);
VOID DrawBltProgressBar(/*UINTN completed*/ );

VOID DrawProgressBarBorder(
    UINTN	x,
    UINTN	y,
	UINTN	w,
    UINTN	h,
	EFI_UGA_PIXEL *BltBuffer,
	UINTN	bw
    );

VOID DrawBlock(
    UINTN	x,
    UINTN	y,
	UINTN	w,
    UINTN	h,
	EFI_UGA_PIXEL *BltBuffer
    );


VOID DrawBltBuffer(
    EFI_UGA_PIXEL *UgaBlt,
    CO_ORD_ATTRIBUTE Attribute,
    UINTN Width,
    UINTN Height,
    INTN DestX,
    INTN DestY,
    UINTN BufferWidth
    );
EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY);

typedef struct RefreshIdInfo{
    UINT8 *pEvent;
    UINT8 *pNotifyContext;
} REFRESH_ID_INFO;

// dobmpmgr.c
#if SETUP_BMP_LOGO_SUPPORT
EFI_STATUS ConvertBmpToUgaBlt (IN VOID *BmpImage, IN UINTN BmpImageSize, IN OUT VOID **UgaBlt,
		IN OUT UINTN  *UgaBltSize, OUT UINTN *PixelHeight, OUT UINTN *PixelWidth );
#endif

EFI_STATUS
ConvertAdvancedImageToUgaBlt (
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **UgaBlt,
  IN OUT UINTN  *UgaBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth,
  OUT BOOLEAN *Animate
);

VOID DoLogoAnimate(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY);
VOID CleanUpExtendedLogo(VOID);


// minisetup.c
BOOLEAN __RuntimeCheckDebugMode( CHAR16 *string );
VOID UIUpdateCallbackHook( VOID * Handle, UINT32 OldVariableCount  );//EIP 129750: UIUpdateCallback as board module hook
EFI_STATUS MainSetupLoopHook( VOID );//EIP74591: MainSetupLoop as board module hook
EFI_STATUS Handshake( VOID );
EFI_STATUS MiniSetupExit( EFI_STATUS Status );
EFI_STATUS MiniSetupEntry( VOID );

//EIP# 58925
extern UINT32 gNavStartPage ;
extern EFI_STATUS GetParentFormID(UINT16 ChildID, UINT16 *ParentID, UINT32 *StackIndex);
extern EFI_STATUS SetParentFormID(UINT16 ParentID, UINT16 TargetID);
extern VOID ResetNavStack();
//EIP# 58925

// notify.c
EFI_STATUS RegisterNotification( VOID );
VOID NotificationFunction( EFI_EVENT Event, VOID *Context );
VOID ActivateApplication( VOID );

//EIP-75236 Starts
VOID ActivateInput( VOID );
BOOLEAN  GetNotifyMaskValue (VOID);
BOOLEAN  IsDelayLogoTillInputSupported (VOID);
//EIP-75236 Ends

// postmgmt.c
VOID EncodePassword( CHAR16 *Password, UINTN MaxSize );
VOID CheckEnableQuietBoot( VOID );
//UINTN DrawAMILogo( VOID );
VOID InstallKeyHandlers( VOID );
VOID InstallClickHandlers( VOID );
VOID _DrawPasswordWindow(UINT16 PromptToken, UINTN PasswordLength, UINTN *CurrXPos, UINTN *CurrYPos);
VOID _ReportInBox(UINTN PasswordLength, UINT16 BoxToken, UINTN CurrXPos, UINTN CurrYPos, BOOLEAN bWaitForReturn);
EFI_STATUS _GetPassword(CHAR16 *PasswordEntered, UINTN PasswordLength, UINTN CurrXPos, UINTN CurrYPos, UINTN *TimeOut);

// helper functions
VOID * HelperGetVariable( UINT32 variable, CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
EFI_STATUS	InitApplicationData(EFI_HANDLE ImageHandle);
VOID MiniSetupUIExit(VOID);
VOID TSEIDEPasswordCheck();
VOID TSEIDEPasswordFreezeDevices();
VOID TSEUnlockHDD(VOID);
extern NVRAM_VARIABLE				*gVariableList;

// EIP 76381 : Performance Improving of variable data load and usage
extern NVRAM_VARIABLE *gCurrNvramVarList;
extern UINTN gCurrNvramVarCount;

#define	VARIABLE_ID_SETUP			0
#define	VARIABLE_ID_LANGUAGE		1
#define	VARIABLE_ID_BOOT_TIMEOUT	2
#define	VARIABLE_ID_USER_DEFAULTS	3
#define	VARIABLE_ID_ERROR_MANAGER	4
#define	VARIABLE_ID_AMITSESETUP     5
#define	VARIABLE_ID_IDE_SECURITY    6
#define VARIABLE_ID_BOOT_ORDER      7
#define VARIABLE_ID_BBS_ORDER       8
#define VARIABLE_ID_DEL_BOOT_OPTION 9
#define VARIABLE_ID_ADD_BOOT_OPTION 10
#define VARIABLE_ID_BOOT_MANAGER    11
#define VARIABLE_ID_BOOT_NOW        12
#define	VARIABLE_ID_LEGACY_DEV_INFO 13
#define	VARIABLE_ID_AMI_CALLBACK    14
#define	VARIABLE_ID_LEGACY_GROUP_INFO 15
#define	VARIABLE_ID_OEM_TSE_VAR		17
#define	VARIABLE_ID_DYNAMIC_PAGE_COUNT	18
#define VARIABLE_ID_ADD_BOOT_PATH                    31
#define VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS	57

UINT16 GetBootTimeOut(UINT16 DefaultValue);
EFI_STATUS UefiFormCallbackNVRead(CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size, VOID **buffer);
EFI_STATUS UefiFormCallbackNVWrite(CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size);
void UpdateAddDeleteBootVar(void);
UINT16 * BootNowinBootOrderInit(VOID);
BOOT_DATA * BootGetBootNowBootData(BOOT_DATA *bootData, UINT16 *BootOrder, UINTN i );
BOOLEAN NoVarStoreSupport(VOID);
BOOLEAN ItkSupport(VOID);
VOID UpdateLegacyDevVariable(UINT16 NoOfLegacyGroups);
VOID CsmBBSSetBootPriorities( BOOT_DATA *pBootData, UINT16 *pOrder, UINTN u16OrderCount);
CHAR16 *CsmBBSBootOptionName( BOOT_DATA *bootData);
EFI_STATUS CsmBBSSetBootNowPriority( BOOT_DATA *BootData,UINTN uiPrefferedDevice,BOOLEAN ShowAllBbsDev);
VOID CsmBBSGetDeviceList( VOID );
EFI_STATUS CsmBBSLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath );
EFI_STATUS 	InstallFormBrowserProtocol(EFI_HANDLE Handle);
VOID UnInstallFormBrowserProtocol(EFI_HANDLE Handle);
EFI_STATUS DoBbsPopupInit(VOID);
VOID MainSetupLoopInit(VOID);
UINTN TseGetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background);
CHAR16 *TseSkipEscCode(CHAR16 *String);
VOID DrawBootOnlyBbsPopupMenu( VOID );
VOID BbsBoot(VOID);
VOID BootGetLanguages();

extern BOOLEAN PlatformLangVerSupport();
extern BOOLEAN BootNowInBootOrderSupport();

VOID TseBootAddBootOption();
BOOLEAN CheckForAddDelBootOption();
BOOLEAN CheckForAddDelDriverOption (VOID);
VOID TSESpecialFixupDelBootOption(VOID *ControlInfo);
VOID TseDoAddBootOptionFixup(VOID *ControlInfo );
BOOLEAN TseDoBootDelBootOption(VOID *popupSel);
VOID TseDiscardAddDelBootOptions();
VOID TseSaveAddDelBootOptions();

// EIP-41615: Functions for the file browser support in add boot option
VOID TseLaunchFileSystem();

VOID TseBBSSetBootPriorities_BootOrder(UINT16 Priority); //EIP-24971: moved TSE_CONTINUE_BOOT_NOW_ON_FAIL support to Binary.

///Extended checks for Add/Delete Boot Option support
BOOLEAN IsUpdateBootOrderCursor();
BOOLEAN IsReservedBootOptionNamesEnable();
BOOLEAN IsPreservedDisabledBootOptionOrder();
EFI_STATUS FastBootLaunch(VOID);
extern BOOLEAN gIsSaveDisabledBBSDevicePath;
VOID UpdateGoPUgaDraw( VOID );//EIP:50479 : Function to Update gGop before using it in AMITSE.
//EIP:51671 Start
//Function declarations 
BOOLEAN IsBootDeviceEnabled( UINT16 value, BOOLEAN ShowAllBBSDev, BOOLEAN TseBootNowInBootOrde, BOOLEAN FromSetup);
BOOLEAN BootGetOptionStatus(BOOT_DATA *bootData, BOOLEAN FromSetup );
BOOLEAN BootGetBBSOptionStatus(BOOT_DATA *bootData, UINT16 value, BOOLEAN FromSetup,  BOOLEAN ShowAllBBSDev);
//EIP:51671 End
VOID RedrawGif(VOID);//EIP:53740 : Function to Redraw the Gif animated image.
BOOLEAN LoadOptionhidden(UINT16, BOOLEAN);	    //EIP:59417 - Fucntion Decleration to check the LOAD_OPTION_HIDDEN attribute for a boot option
EFI_STATUS TSEGetCoordinates(INT32 *x, INT32 *y, INT32 *z);
BOOLEAN CheckHiddenforBootDriverOption (UINT16, BOOLEAN);

//EIP70421 & 70422  Support for driver order starts
extern BOOT_DATA					*gDriverData;
void UpdateAddDeleteDriverVar (void);
VOID TseDriverAddDriverOption();
VOID TSESpecialFixupDelDriverOption (VOID *);
BOOLEAN TseDoDriverDelDriverOption(VOID *popupSel);
VOID TseDiscardAddDelDriverOptions();
VOID TseSaveAddDelDriverOptions();
//EIP70421 & 70422  Support for driver order ends
BOOLEAN  IsTSEMultilineControlSupported (VOID); //EIP-72610 Moved TSE_MULTILINE_CONTROLS to binary
UINT32 GetMsgboxWidth(VOID);//EIP74963 : MAX_MSGBOX_WIDTH macro changed as token and handled from binary
extern const UINTN TsePasswordLength;
VOID GetAMITSEVariable(AMITSESETUP **mSysConf,UINT8 **setup,UINTN *VarSize);
VOID NoVarStoreUpdateSystemAccess(UINT8 sysAccessValue);
VOID SetSystemAccessValueItk(UINT8 sysAccessValue);
VOID TSEStringReadLoopEntryHook(VOID);
VOID TSEStringReadLoopExitHook(VOID);
UINT32 PasswordAuthenticate( CHAR16 *Password );
EFI_STATUS AMIReadKeyStroke(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData);
EFI_STATUS HelperIsPasswordCharValid(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData,UINTN StrIndex,UINTN PasswordLength, CHAR16 *OutChar);
VOID TSEIDEUpdateConfig(VOID *ideSecConfig, UINTN value);
UINT8  IsTSETextExcessSupport (VOID);
BOOLEAN IsPopupMenuEnterSetupEnabled(VOID);
//EIP 77400
#define EFI_SHELL_PROTOCOL_GUID \
  { \
    0x47C7B223, 0xC42A, 0x11D2, 0x8E, 0x57, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B \
  }

EFI_STATUS InitEsaTseInterfaces (void);		//EIP162708
extern ESA_INTERFACES_FOR_TSE	*gEsaInterfaceForTSE;

MSGBOX_EX_CATAGORY SetMessageBoxProgressType (void);

VOID OverrideTitleString(void);			//EIP233428
CHAR8* _GetSupportedLanguages (EFI_HII_HANDLE HiiHandle);
VOID _GetNextLanguage(CHAR8 **LangCode, CHAR8 *Lang);
CHAR16* StrDup8to16( CHAR8 *string );

VOID DefaultsEvaluateExpression(EFI_EVENT Event, VOID* Context);
BOOLEAN IsDefaultConditionalExpression(VOID);
BOOLEAN IsTSEDisplayFormsetParsingError(VOID);
EFI_STATUS CreateReadyToBootEventForEvaluateDefault();
BOOLEAN   IsRecoverySupported(VOID);
extern EFI_UGA_PIXEL *gPostscreenwithlogo;
VOID RestoreGraphicsScreen (VOID);
extern BOOLEAN IsSetupFormBrowserNestedSendFormSupport(void);
#pragma pack(1)
typedef struct _APPS_DATA
{
	UINT8  *ApplicationData;
	UINT32 PageListSize;
	UINT32 PageListOffset;
	UINT32 PageInfoSize;
	UINT32 PageInfoOffset;
	UINT32 AllocatedFirstPageSize;
	UINT32 FirstPageOffset;	
}APPS_DATA;

typedef struct _SETUPDATA
{
	EFI_HII_HANDLE *SetupHandles;
	VOID/*SETUP_LINK*/ *SetupData;
	UINTN 			SetupCount;
	
}SETUPDATA;

typedef struct _SENDFORM_DATA
{
	VOID    **SfHandles;
	UINTN   SfHandleCount;
	UINT8 *SfNvMap;
	EFI_GUID *FSetGuid;
}SENDFORM_DATA;

typedef struct _GUID_DATA
{
	EFI_GUID *GuidDump;
	UINTN    GuidDumpCount;
}GUID_DATA;

typedef struct _CONTROLS_DATA
{
	UINT32 		ControlListSize;
	UINT32 		ControlListOffset;
}CONTROLS_DATA;

typedef struct _VARIABLE_DATA
{
	NVRAM_VARIABLE	*VariableList;
	UINT32 	VariableListSize;
	UINT32 	VariableListOffset;
	UINT32 	VariableInfoSize;
	UINT32 	VariableInfoOffset;	
}VARIABLE_DATA;


typedef struct _DEFAULT_DATA
{
	NVRAM_VARIABLE	*FailsafeDefaults;
	NVRAM_VARIABLE	*OptimalDefaults;
}DEFAULT_DATA;

typedef struct _PAGEID_DATA
{
	UINT32 			PageIdListSize;
	UINT32 			PageIdListOffset;
	UINT32 			PageIdInfoSize;
	UINT32 			PageIdInfoOffset;
}PAGEID_DATA;

typedef struct _DYNAMIC_DATA
{
	UINTN DynamicPageCount;
	VOID/*DYNAMIC_PAGE_GROUP*/ *DynamicPageGroup;
	UINTN DynamicPageGroupCount;
	UINT16 CurrDynamicPageGroupClass;
	UINT16 CurrDynamicPageGroup; 
}DYNAMIC_DATA;


typedef struct _BOOT_OPTION_DATA
{
	BOOT_DATA	*BootData;
	UINTN	BootOptionCount;
	BOOT_DATA *CurrLegacyBootData;
	BOOT_DATA *DriverData;
	UINTN	DriverOptionCount;
/*Actual global variables corresponds to below four members are defined in TseAdvanced. 
Still not grouped separatly to suppress in ESA build since same variables are used in ESA side which are defined in EsaSrcHelper.c*/ 
	UINT16 	*BootOptionTokenArray;
	UINTN 	BootOptionTokenCount;
	UINT16 	*DriverOptionTokenArray;
	UINTN 	DriverOptionTokenCount;
}BOOT_OPTION_DATA;

////Variables Available only for TSE Starts
typedef struct _FLAG_DATA
{
	BOOLEAN SetupUpdated;
	BOOLEAN TseCacheUpdated;
	BOOLEAN UserTseCacheUpdated;
	BOOLEAN VariableChanged;
	BOOLEAN ResetRequired;
}FLAG_DATA;

typedef struct _NAV_STACK
{
	VOID *NavPageStack;
	UINT32 NavStartPage;
	UINT32 PageNavCount;
	UINT32 NavPageStackSize;
}NAV_STACK;

typedef struct _NOTIFICATION_HANDLER
{
	BOOLEAN EnableDrvNotification;
	BOOLEAN EnableProcessPack;
	BOOLEAN PackUpdatePending;
	BOOLEAN BrowserCallbackEnabled;
}NOTIFICATION_HANDLER;

typedef struct _TSELITE_GLOBALS
{
	VOID	*App;
	CONTROL_INFO *ControlInfo;
	VARIABLE_LIST	*Variables;
	NVRAM_VARIABLE *CurrNvramVarList;
	UINTN CurrNvramVarCount;
	PAGE_ID_LIST	*PageIdList;
	FLAG_DATA FlagData;
	NAV_STACK NavigationStack;	
	CHAR8 *PrevLanguage;
	UINTN PrevLangSize;
	ACTION_DATA *MsgBoxAction;
}TSELITE_GLOBALS;
////Variables Available only for TSE Ends

typedef struct _TSE_SETUP_GLOBAL_DATA TSE_SETUP_GLOBAL_DATA;

struct _TSE_SETUP_GLOBAL_DATA
{
	APPS_DATA AppsData;
	SETUPDATA SetupData;
	SENDFORM_DATA SfData;
	GUID_DATA GuidData;
	CONTROLS_DATA ControlData;
	VARIABLE_DATA VariableData;
	DEFAULT_DATA DefaultData;
	PAGEID_DATA PageIdData;
	DYNAMIC_DATA DynamicData;
	BOOT_OPTION_DATA BootOptionData;
	NOTIFICATION_HANDLER NotifHandler;
	UINTN TotalRootPages;
	EFI_IFR_REF *RefData;
	UINTN PostStatus;
	UINT32 tempCurrentPage;
	TSELITE_GLOBALS TseLite;
	TSE_SETUP_GLOBAL_DATA *Next;
};
#pragma pack()


#endif /* _MINISETUP_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2012, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
