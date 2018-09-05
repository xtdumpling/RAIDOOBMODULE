//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Implement check password during POST or enter Setup feature.
//    Reason:
//    Auditor:  Jacker Yeh (Refer from Grantley)
//    Date:     Oct/27/2016
//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/CommonHelper.c $
//
// $Author: Arunsb $
//
// $Revision: 97 $
//
// $Date: 5/29/12 3:15a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file CommonHelper.c
    This file is Helper file for Boot only TSE module
    This functions may be overridden by Other Advanced TSE module.

**/

#ifndef __GNUC__
#pragma warning( disable : 4028 )
#endif
#include "TimeStamp.h"
#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"  
#include <Efi.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/EfiOemBadging.h>
#include <Protocol/AMIPostMgr.h>
#include "AMITSEStrTokens.h"
#include <Protocol/FirmwareVolume.h>
#include "AmiLib.h"				//Added for using CONVERT_TO_STRING macro
#include "AmiDxeLib.h"
#include "AMITSEElinks.h"
#include "AutoId.h" 
#include "TseElinks.h" 

VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

#else //#ifdef TSE_FOR_APTIO_4_50

#include "minisetup.h"

#endif //#ifdef TSE_FOR_APTIO_4_50

#include "commonoem.h"
#include "LogoLib.h"
#include "mem.h"
#include "HiiLib.h"
#include "PwdLib.h"
#include "boot.h"
#include "bootflow.h"
#include "variable.h"
#include "TseCommon.h"
// Build time file generated from AMITSE_OEM_HEADER_LIST elink.
#include "AMITSEOem.h"	
#include "setupdata.h"	
#if SUPPORT_ESRT
#include "Protocol/AmiReflashProtocol.h"
#endif
#if SMCPKG_SUPPORT
#include <Setup.h>
#endif	
#if ACPI_SUPPORT    
#include "AcpiOemElinks.h"
#endif

#ifdef ACPI_OEM_ID_MAK
UINT8   OemIdMak [6] = ACPI_OEM_ID_MAK;
#endif

#ifdef ACPI_OEM_TBL_ID_MAK
UINT8   OemTblIdMak [8] = ACPI_OEM_TBL_ID_MAK;
#endif

/////////////OEM_SPECIAL_CONTROL////////////////
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
//#include "SetupData.h"

VOID *VarGetVariable( UINT32 variable, UINTN *size );
#define	VARIABLE_ID_OEM_TSE_VAR		17

#endif //SETUP_OEM_SPECIAL_CONTROL_SUPPORT

#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
UINT16 OEMSpecialGetControlCount(CONTROL_INFO *controlInfo);
VOID OEMSpecialOneOfFixup( CONTROL_INFO *control , UINT16 value );
VOID OEMSpecialGotoFixup(CONTROL_INFO *control, UINT16 value );
#endif //#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT

//#include EFI_PROTOCOL_DEFINITION(LegacyBios)

#if !TSE_APTIO_5_SUPPORT
#if !TSE_CSM_SUPPORT
EFI_GUID gEfiLegacyBiosProtocolGuid = EFI_LEGACY_BIOS_PROTOCOL_GUID;
#endif
#endif

#define VARIABLE_NAME_LENGTH	40
#define _CharIsUpper(c) ((c >= L'A') && (c <= L'Z'))
#define _CharIsLower(c) ((c >= L'a') && (c <= L'z'))
#define _CharIsAlpha(c) (_CharIsUpper(c) || _CharIsLower(c)) 
#define _CharIsNumeric(c) ((c >= L'0') && (c <= L'9'))
#define _CharIsAlphaNumeric(c) (_CharIsAlpha(c) || _CharIsNumeric(c))
#define _CharIsAlphaNumericSpecial(c) ((c >= (CHAR16)0x20) && (c <= (CHAR16)0x7E))
#define Str2No(A)	(A - '0')
#define	TSE_STRUCT_OFFSET(type, field)	(UINTN)&(((type *)0)->field)

#define	NOTIFY_MASK_UGA					(0x00000001)
#define	NOTIFY_MASK_CONSOLE_CONTROL	(0x00000002)
#define	NOTIFY_MASK_CONSOLE_IN			(0x00000004)
#define	NOTIFY_MASK_CONSOLE_OUT			(0x00000008)
#define	NOTIFY_MASK_MOUSE_DRIVER		(0x00000010)

#if SETUP_DELAY_POST_TILL_GOP
	#if SETUP_DELAY_LOGO_TILL_INPUT
		#define	SOMETHING	(NOTIFY_MASK_UGA | NOTIFY_MASK_CONSOLE_CONTROL | NOTIFY_MASK_CONSOLE_IN | NOTIFY_MASK_CONSOLE_OUT /*| NOTIFY_MASK_MOUSE_DRIVER*/)
	
	#else
		#define	SOMETHING	(NOTIFY_MASK_UGA | NOTIFY_MASK_CONSOLE_CONTROL | NOTIFY_MASK_CONSOLE_OUT /*| NOTIFY_MASK_MOUSE_DRIVER*/)	
	#endif
#else
	#if SETUP_DELAY_LOGO_TILL_INPUT
		#define	SOMETHING	( NOTIFY_MASK_CONSOLE_CONTROL | NOTIFY_MASK_CONSOLE_IN | NOTIFY_MASK_CONSOLE_OUT /*| NOTIFY_MASK_MOUSE_DRIVER*/)	
	#else
		#define	SOMETHING	( NOTIFY_MASK_CONSOLE_CONTROL | NOTIFY_MASK_CONSOLE_OUT /*| NOTIFY_MASK_MOUSE_DRIVER*/)	
	#endif
#endif

#ifndef SCAN_F12
#define SCAN_F12        EFI_SCAN_F12
#endif

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;
extern BOOLEAN gQuietBoot;
VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
VOID GetTseBuildVersion(UINTN *TseMajor, UINTN *TseMinor, UINTN *TseBuild);

#if !TSE_APTIO_5_SUPPORT
	#if TSE_FOR_64BIT
		CHAR16 gBootFileName[]=L"\\EFI\\BOOT\\BOOTX64.EFI";
	#else
		CHAR16 gBootFileName[]=L"\\EFI\\BOOT\\BOOTIA32.EFI";
	#endif //TSE_FOR_64BIT
#else
	CHAR16 gBootFileName[]=EFI_REMOVABLE_MEDIA_FILE_NAME;
#endif

#if TSE_CAPITAL_BOOT_OPTION
CHAR16 gBootFormarSpecifier[]=L"Boot%04X";
CHAR16 gDriverFormarSpecifier[]=L"Driver%04X";
#else
CHAR16 gBootFormarSpecifier[]=L"Boot%04x";
CHAR16 gDriverFormarSpecifier[]=L"Driver%04x";
#endif

BOOLEAN gPopupMenuShowAllBbsDev = POPUP_MENU_SHOW_ALL_BBS_DEVICES;
BOOLEAN gShowAllBbsDev = SETUP_SHOW_ALL_BBS_DEVICES;
BOOLEAN gIsSaveDisabledBBSDevicePath = TSE_SAVE_DISABLED_BBS_DEVICEPATH;
BOOLEAN gSetupHideDisableBootOptions = SETUP_HIDE_DISABLE_BOOT_OPTIONS;
BOOLEAN gPopupMenuHideDisableBootOptions = POPUP_MENU_HIDE_DISABLE_BOOT_OPTIONS;
#ifdef TSE_LOAD_OPTION_HIDDEN
BOOLEAN gLoadOptionHidden = TSE_LOAD_OPTION_HIDDEN;
#else
BOOLEAN gLoadOptionHidden = 0;
#endif

const UINTN TsePasswordLength = SETUP_PASSWORD_LENGTH;




EFI_STATUS GetGifNextFrame( IN OUT VOID **UgaBlt, OUT UINTN	*GifDelay );
extern VOID DrawBltBuffer( EFI_UGA_PIXEL *UgaBlt, CO_ORD_ATTRIBUTE Attribute, UINTN Width,  UINTN Height,
    		INTN DestX, INTN DestY,  UINTN BufferWidth);


extern EFI_STATUS ConvertBmpToUgaBlt (IN VOID *BmpImage, IN UINTN BmpImageSize, IN OUT VOID **UgaBlt,
		IN OUT UINTN  *UgaBltSize, OUT UINTN *PixelHeight, OUT UINTN *PixelWidth );

extern EFI_STATUS ConvertGifToUgaBlt ( IN VOID *GifImage, IN UINTN GifImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINTN *UgaBltSize, OUT UINTN *PixelHeight, OUT UINTN *PixelWidth, OUT UINTN *GifDelay );

extern EFI_STATUS ConvertJPEGToUgaBlt ( IN VOID *JPEGImage, IN UINT32 JPEGImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINT32 *UgaBltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth, OUT UINT32 *punBufferWidth);

extern EFI_STATUS ConvertPCXToUgaBlt ( IN VOID *PCXImage, IN UINT32 PCXImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINT32 *UgaBltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth);

extern EFI_STATUS ConvertPNGToUgaBlt ( IN VOID *PCXImage, IN UINT32 PCXImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINT32 *UgaBltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth);
//Functions
VOID DoAddBootOptionFixup (VOID *ControlInfo);
VOID SpecialFixupDelBootOption( VOID *ControlInfo);
VOID BootAddBootOption();
BOOLEAN DoBootDelBootOption(VOID *popupSel);
void TseUpdateAddDeleteBootVar (void);
VOID DiscardAddDelBootOptions(VOID);
VOID SaveAddDelBootOptions();
UINT16 * TseBootNowinBootOrderInit(VOID);
EFI_STATUS TseVarBuildAMIDefaults(VOID);
VOID TSEMouseInit(VOID);
VOID TSEMouseStop(VOID);
EFI_STATUS TSEGetCoordinates(INT32 *x, INT32 *y, INT32 *z);
BOOLEAN TSEIsMouseClickedonSoftkbd(VOID);
VOID TSEMouseRefresh(VOID);
VOID TSEMouseFreeze(VOID);
VOID TSEMouseStart(VOID);
VOID TSEMouseDestroy(VOID);
INT32 TSEGetactualScreentop(VOID);
INT32 TSEGetactualScreenLeft(VOID);
VOID TSENumericSoftKbdExit(VOID);
VOID TSENumericSoftKbdInit(VOID);
VOID TSESetPwdKeyboardLayout(VOID);
VOID TSEResetPwdKeyboardLayout(VOID);
VOID TSEPrintableKeysSoftKbdInit(VOID);
VOID TSEPrintableKeysSoftKbdExit(VOID);
EFI_STATUS TSEMouseReadInfo(VOID *MouseInfo);
EFI_STATUS TSEMouseScrollBarMove(VOID *frame, BOOLEAN bScrollUp, UINT32 Size); 
EFI_STATUS TSEMouseListBoxScrollBarMove(VOID *listbox, BOOLEAN bScrollUp, UINT32 Size);
EFI_STATUS TSEMouseFrameHandleAction( VOID *frame, VOID *action,VOID *control );
EFI_STATUS TSEMouseSubMenuHandleAction( VOID *submenu, VOID *Data);
EFI_STATUS TSEMouseMsgBoxHandleAction( VOID *msgbox, VOID *Data,BOOLEAN * pRedraw);
EFI_STATUS TSEMousePopupSelHandleAction( VOID *popupSel, VOID *Data);
EFI_STATUS TSEMousePopupEditHandleAction( VOID *PopupEdit, VOID *Data, BOOLEAN * pRedraw );
EFI_STATUS TSEMouseMenuHandleAction( VOID *menu, VOID *Data );
EFI_STATUS TSEMouseListBoxHandleAction( VOID *listbox, VOID *Data);
VOID ClearUserPasswordPolicy(VOID *callbackData,VOID *saveData);
EFI_STATUS TseFramePwdCallbackIdePasswordUpdate ( VOID *control,VOID *saveData);
EFI_STATUS TsePopupPwdAuthenticateIDEPwd(VOID *popuppassword, BOOLEAN *AbortUpdate,VOID *data);
EFI_STATUS TseCheckForIDEPasswordInstalled(popuppassword);
VOID TsePopupPwdUpdateIDEPwd (VOID);
VOID RTIfrProcessExit(VOID);
VOID RTIfrProcessAddVarListAndPageIDList(VOID);
BOOLEAN RTIfrProcessFormIfUpdated(UINT16 link);
VOID RTIfrProcessRunTimeForms(VOID*ref);
LOGO_TYPE GetBmpLogoType(UINT8 *ImageData);
LOGO_TYPE GetGifLogoType(UINT8 *ImageData);
LOGO_TYPE GetJPEGLogoType(UINT8 *ImageData);
LOGO_TYPE GetPCXLogoType(UINT8 *ImageData);
LOGO_TYPE GetOEMLogoType(UINT8 *ImageData);
VOID CleanUpGif(VOID);
VOID DoGifAnimate(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY);
UINTN HiiTestPrintLength ( IN CHAR16   *String );
UINTN UefiHiiTestPrintLength ( IN CHAR16   *String );
UINTN EfiTestPrintLength ( IN CHAR16   *String );
VOID BBSSetBootPriorities( BOOT_DATA *pBootData, UINT16 *pOrder, UINTN u16OrderCount);
BOOLEAN IsBBSDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath );
CHAR16 * GetBBSBootOptionName( BOOT_DATA *bootData);
EFI_STATUS BBSSetBootNowPriority( BOOT_DATA *BootData,UINTN uiPrefferedDevice,BOOLEAN ShowAllBbsDev);
VOID BBSGetDeviceList( VOID );
VOID BBSBuildDefaultName( CHAR16 *String, VOID *info, UINTN index, VOID *HDDInfo );
VOID BBSBuildDevicePathName( CHAR16 *String, VOID *info, UINTN index, VOID *HDDInfo );
VOID SaveBBSOrder(VOID*);
VOID BBSSetDisabled(UINT16 Index, VOID **DisDPs, UINTN *DPSize);
VOID RearrangeBBSOrderVariable(VOID *popupSel, UINT8 bIncrease,UINT16 *newOption);
VOID LoadDefaultLegDevOrder(VOID);
VOID BBSUpdateOrder(UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer);
VOID BBSSetBootPriorities_BootOrder(UINT16);
UINTN GetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background);
CHAR16 *StrDup(CHAR16 *String);
CHAR16 *SkipEscCode(CHAR16 *String);
BOOLEAN CheckShiftState(UINT32 ActionKey, UINT32 HotkeyState);
BOOLEAN SupportPrintScreenEvent();
VOID SupportPrintScreen();
VOID GetBootLanguages( VOID );
VOID GetPlatformBootLanguages( VOID );
VOID RTIfrUpdateVariableInfo( UINT32 ControlVariable, EFI_HANDLE Handle );
void ClearGrphxScreen (void);
VOID ConvertBmpandAddBGRT (
	BOOLEAN GifImagePresence
    );
VOID InvalidateStatusInBgrt (VOID);
UINT32 GetMsgboxWidth(VOID);
INTN  gHotKeyBootOption = -1;
extern UINT32 gBootFlow;
VOID InvalidateStatusInBgrtWrapper (VOID);

VOID SpecialFixupDelDriverOption (VOID *);
BOOLEAN DoDriverDelDriverOption (VOID *);
void TseUpdateAddDeleteDriverVar (VOID );
VOID DiscardAddDelDriverOptions (VOID);
VOID SaveAddDelDriverOptions ();
VOID UninstallBGRT (VOID);
LOGO_TYPE GetExtendedLogoType(UINT8 *ImageData);

VOID *SavePostScreen( UINTN *SizeOfX, UINTN *SizeOfY );
VOID RestorePostScreen( VOID *UgaBlt, UINTN SizeOfX, UINTN SizeOfY );
void SaveActiveBuffer(void **TempScreenBuffer);
void RestoreActiveBuffer(void **TempScreenBuffer);

extern BOOT_DATA	*gBootData;						
extern UINTN		gBootOptionCount;
extern EFI_GUID _gBootFlowGuid;

VOID SetupDebugPrint(IN CONST CHAR8  *Format, ...) ;
#if SUPPRESS_PRINT
    #define SETUP_DEBUG_TSE(format,...)
#else //Else of SUPPRESS_PRINT
#if BUILD_OS == BUILD_OS_LINUX
    #define SETUP_DEBUG_TSE(format,...) SetupDebugPrint(format, ##__VA_ARGS__)
#else //Else of BUILD_OS == BUILD_OS_LINUX
    #define SETUP_DEBUG_TSE(format,...) SetupDebugPrint(format, __VA_ARGS__)
#endif //End of BUILD_OS == BUILD_OS_LINUX
#endif

//Controlling from binary
PASSWORD_ENOCDE_LIST_TEMPLATE gPasswordEncodeList [] = {PASSWORD_ENOCDE_LIST {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0, 0}};	//Last structure for avoiding build error.
UINT32 gEncodeListCount = sizeof (gPasswordEncodeList)/ sizeof (PASSWORD_ENOCDE_LIST_TEMPLATE);

/**
    Function to return the SETUP_SUPPORT_ADD_BOOT_OPTION token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN CheckForAddDelBootOption ()
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	return 1;
#else
	return 0;
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

/**
    Function to return the SETUP_SUPPORT_ADD_DRIVER_OPTION token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN CheckForAddDelDriverOption (VOID)
{
#ifdef SETUP_SUPPORT_ADD_DRIVER_OPTION
	return SETUP_SUPPORT_ADD_DRIVER_OPTION;		//Since there is no token dependency for this, returning as such
#else
	return 0;
#endif
}

VOID TseDoAddBootOptionFixup(VOID *ControlInfo )
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	DoAddBootOptionFixup(ControlInfo);
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}


VOID TSESpecialFixupDelBootOption(VOID *ControlInfo)
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	SpecialFixupDelBootOption(ControlInfo);
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

VOID TSESpecialFixupDelDriverOption (VOID *ControlInfo)
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	SpecialFixupDelDriverOption (ControlInfo);
#endif
}

VOID TseBootAddBootOption()
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	BootAddBootOption();
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

VOID TseDriverAddDriverOption ()
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	DriverAddDriverOption ();
#endif
}

EFI_STATUS FileBrowserLaunchFileSystem (UINT32 Variable);
EFI_STATUS FileBrowserLaunchFilePath (UINT32 Variable);
//----------------------------------------------------------------------------
// Procedure:    TseLaunchFileSystem
//
// Description: Override function. OEM can override this function to change the 
//				default behaviour of function
//----------------------------------------------------------------------------
#if !OVERRIDE_TseLaunchFileSystem
EFI_STATUS TseLaunchFileSystem (UINT32 Variable)
{
	EFI_STATUS Status = EFI_SUCCESS;
#if SETUP_SUPPORT_ADD_BOOT_OPTION || SETUP_SUPPORT_ADD_DRIVER_OPTION			
	Status = FileBrowserLaunchFileSystem (Variable);
	if(!EFI_ERROR(Status))
		Status = FileBrowserLaunchFilePath (Variable);
#endif
	return Status;
}
#endif

BOOLEAN TseDoBootDelBootOption(VOID *popupSel)
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	return DoBootDelBootOption(popupSel);
#else
	return 0;
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

BOOLEAN TseDoDriverDelDriverOption (VOID *popupSel)
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	return DoDriverDelDriverOption (popupSel);
#else
	return 0;
#endif
}

void UpdateAddDeleteBootVar(void)
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	TseUpdateAddDeleteBootVar();
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

void UpdateAddDeleteDriverVar (void)
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	TseUpdateAddDeleteDriverVar ();
#endif
}

VOID TseDiscardAddDelBootOptions()
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	DiscardAddDelBootOptions();
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

VOID TseDiscardAddDelDriverOptions ()
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	DiscardAddDelDriverOptions ();
#endif
}

VOID TseSaveAddDelBootOptions()
{
#if SETUP_SUPPORT_ADD_BOOT_OPTION
	SaveAddDelBootOptions();
#endif //#if SETUP_SUPPORT_ADD_BOOT_OPTION
}

VOID TseSaveAddDelDriverOptions ()
{
#if SETUP_SUPPORT_ADD_DRIVER_OPTION
	SaveAddDelDriverOptions ();
#endif
}

BOOLEAN IsLinkHistorySupport()
{
#if SETUP_LINK_HISTORY_SUPPORT
	return 1;
#else
	return 0;
#endif  
}

BOOLEAN IsSubMenuDisplayTitle()
{
#if SETUP_DISPLAY_SUBMENU_PAGETITLE
	return 1;
#else
	return 0;
#endif  
}

BOOLEAN IsReadOnlyGrayout()
{
#if SETUP_GRAYOUT_READONLY_CONTROL
	return 1;
#else
	return 0;
#endif  
}


BOOLEAN IsGroupDynamicPages()
{
#if SETUP_GROUP_DYNAMIC_PAGES
	return 1;
#else
	return 0;
#endif  
}

BOOLEAN IsOrphanPagesAsRootPage()
{
#if SETUP_ORPHAN_PAGES_AS_ROOT_PAGE
	return 1;
#else
	return 0;
#endif  
}

BOOLEAN IsUpdateBootOrderCursor()
{
#if SETUP_UPDATE_BOOT_ORDER_CURSOR
	return 1;
#else
	return 0;
#endif
}

BOOLEAN IsPreservedDisabledBootOptionOrder()
{
#if TSE_PRESERVE_DISABLED_BBS_DEVICE_ORDER
	return 1;
#else
	return 0;
#endif  
}

BOOLEAN IsReservedBootOptionNamesEnable()
{
#if SETUP_SUPPORT_RES_ADD_DEL_BOOT_OPTION_NAME
	return 1;
#else
	return 0;
#endif  //#if SETUP_SUPPORT_RES_ADD_DEL_BOOT_OPTION_NAME
}

/**
    function to return  TSE_BOOT_TIME_OUT_AS_ZERO value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsBootTimeOutValueZero()
{
#ifdef TSE_BOOT_TIME_OUT_AS_ZERO
	return TSE_BOOT_TIME_OUT_AS_ZERO;
#else
	return 0;
#endif  //#if TSE_BOOT_TIME_OUT_AS_ZERO
}

BOOT_DATA * BootGetBootNowBootData(BOOT_DATA *bootData, UINT16 *BootOrder, UINTN i )
{
#if TSE_BOOT_NOW_IN_BOOT_ORDER
	return BootGetBootData(BootOrder[i]);
#else
	return (bootData + i);
#endif
}


UINT16 * BootNowinBootOrderInit(VOID)
{
#if TSE_BOOT_NOW_IN_BOOT_ORDER
	return TseBootNowinBootOrderInit();
#else
	return NULL;
#endif
}


EFI_STATUS VarBuildAMIDefaults(VOID)
{
#if SETUP_USE_AMI_DEFAULTS
	return TseVarBuildAMIDefaults();
#else
    return EFI_UNSUPPORTED;
#endif // SETUP_USE_AMI_DEFAULTS
}

///
/// Mouse Functions
///

VOID MouseInit(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseInit();	
#endif	
}

BOOLEAN IsMouseSupported(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	return TRUE;
#else
	return FALSE;
#endif	
}

BOOLEAN IsSoftKbdSupported(VOID)
{
#if AMITSE_SOFTKBD_SUPPORT
	return TRUE;
#else
	return FALSE;
#endif	
}


VOID MouseStop(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseStop();	
#endif	
}

VOID GetCoordinates(INT32 *x, INT32 *y, INT32 *z)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEGetCoordinates(x,y,z);	
#endif	
}

VOID MouseRefresh(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseRefresh();	
#endif	
}
BOOLEAN IsMouseClickedonSoftkbd(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEIsMouseClickedonSoftkbd();	
#endif
return FALSE;	
}

VOID MouseFreeze(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseFreeze();	
#endif	
}

VOID MouseStart(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseStart();	
#endif	
}

VOID NumericSoftKbdInit(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSENumericSoftKbdInit();
#endif

}

VOID NumericSoftKbdExit(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSENumericSoftKbdExit();
#endif

}

/**
    function to initialize SetKeyboardLayout

    @param VOID

    @retval VOID

**/
VOID SetPwdKeyboardLayout(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSESetPwdKeyboardLayout();
#endif

}
/**
    function to exit ResetKeyboardLayout and reset with default keyboard layout

    @param VOID

    @retval VOID

**/
VOID ResetPwdKeyboardLayout(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSEResetPwdKeyboardLayout();
#endif

}

/**
    function to launch PrintableKeysSoftKbd layout

    @param VOID

    @retval VOID

**/
VOID PrintableKeysSoftKbdInit(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSEPrintableKeysSoftKbdInit();
#endif

}
/**
    function to exit PrintableKeysSoftKbd and reset with default keyboard layout

    @param VOID

    @retval VOID

**/
VOID PrintableKeysSoftKbdExit(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	TSEPrintableKeysSoftKbdExit();
#endif

}


EFI_STATUS MouseReadInfo(VOID *MouseInfo)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEMouseReadInfo(MouseInfo);	
#else
    return EFI_UNSUPPORTED;
#endif	
}

VOID MouseDestroy(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	TSEMouseDestroy();	
#endif	
}

/**
    function to get the actual screen top value for mouse

    @param VOID

    @retval INT32

**/
INT32 GetactualScreentop(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEGetactualScreentop();
#endif	

return 0;
}
/**
    Function to retrun the actual screen left(X) coordinates

    @param VOID

    @retval INT32

**/
INT32 GetactualScreenLeft(VOID)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEGetactualScreenLeft();
#endif
	return 0;
}

EFI_STATUS MouseFrameHandleAction( VOID *frame, VOID *action,VOID *control )
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEMouseFrameHandleAction(frame, action, control);	
#else
    return EFI_UNSUPPORTED;
#endif
}

EFI_STATUS MouseMsgBoxHandleAction( VOID *msgbox, VOID *Data,BOOLEAN * pRedraw)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEMouseMsgBoxHandleAction(msgbox, Data, pRedraw);	
#else
    return EFI_UNSUPPORTED;
#endif
}

EFI_STATUS MouseMenuHandleAction( VOID *menu, VOID *Data )
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEMouseMenuHandleAction(menu, Data);	
#else
    return EFI_UNSUPPORTED;
#endif	
}


EFI_STATUS MouseListBoxHandleAction( VOID *listbox, VOID *Data)
{
#if MINISETUP_MOUSE_SUPPORT
	return TSEMouseListBoxHandleAction(listbox, Data);	
#else
    return EFI_UNSUPPORTED;
#endif	
}


//----------------------------------------------------------------------------
// Procedure:    FramePasswordAdvancedCallback
//
// Description: Override function. OEM can override this function to change the 
//				default behaviour of function
//----------------------------------------------------------------------------
#if !OVERRIDE_FramePasswordAdvancedCallback 
VOID FramePasswordAdvancedCallback(VOID *callbackData,VOID *saveData)
{
#if TSE_CLEAR_USERPW_WITH_ADMINPW
	ClearUserPasswordPolicy(callbackData,saveData);
#endif //#if TSE_CLEAR_USERPW_WITH_ADMINPW
}
#endif


#if !OVERRIDE_FramePwdCallbackIdePasswordUpdate
EFI_STATUS FramePwdCallbackIdePasswordUpdate ( VOID *control,VOID *saveData)
{
#if SETUP_IDE_SECURITY_SUPPORT
	return TseFramePwdCallbackIdePasswordUpdate ( control,saveData);
#else
    return EFI_UNSUPPORTED;
#endif
}
#endif

#if !OVERRIDE_PopupPwdAuthenticateIDEPwd
EFI_STATUS PopupPwdAuthenticateIDEPwd(VOID *popuppassword, BOOLEAN *AbortUpdate,VOID *data)
{
#if SETUP_IDE_SECURITY_SUPPORT
	return TsePopupPwdAuthenticateIDEPwd(popuppassword, AbortUpdate,data);
#else
    return EFI_UNSUPPORTED;
#endif
}
#endif

#if !OVERRIDE_CheckForIDEPasswordInstalled
EFI_STATUS CheckForIDEPasswordInstalled(VOID *popuppassword)
{
#if SETUP_IDE_SECURITY_SUPPORT 
	return TseCheckForIDEPasswordInstalled(popuppassword);
#else
    return EFI_UNSUPPORTED;
#endif
}
#endif

#if !OVERRIDE_PopupPwdUpdateIDEPwd
VOID PopupPwdUpdateIDEPwd (VOID)
{
#if SETUP_IDE_SECURITY_SUPPORT
	TsePopupPwdUpdateIDEPwd ();
#endif
}
#endif

LOGO_TYPE GetBmpLogoType(UINT8 *ImageData)
{
#if SETUP_BMP_LOGO_SUPPORT
//	if ( ((BMP_IMAGE_HEADER *)ImageData)->CharB == 'B' && ((BMP_IMAGE_HEADER *)ImageData)->CharM == 'M' ) 
	if ( ImageData[0] == 'B' && ImageData[1] == 'M' ) 
  		return BMP_Logo;
#endif
	return Unsupported_Logo;
}


LOGO_TYPE GetGifLogoType(UINT8 *ImageData)
{
#if SETUP_GIF_LOGO_SUPPORT
	if ( 0 == EfiCompareMem(ImageData, "GIF87a", 6) || 0 == EfiCompareMem(ImageData, "GIF89a", 6) )
		return GIF_Logo;
#endif

	return Unsupported_Logo;
}



LOGO_TYPE GetJPEGLogoType(UINT8 *ImageData)
{

#if SETUP_JPEG_LOGO_SUPPORT
    // Start of Image
    if(0x0D8FF == *((UINT16*)ImageData))
		return JPEG_Logo;
#endif

	return Unsupported_Logo;
}

LOGO_TYPE GetPCXLogoType(UINT8 *ImageData)
{

#if SETUP_PCX_LOGO_SUPPORT
	 if (((*ImageData) == 0x0A) && (*(ImageData+2) == 0x01))
		return PCX_Logo;
#endif

	return Unsupported_Logo;
}

LOGO_TYPE GetPNGLogoType(UINT8 *ImageData)
{

#if SETUP_PNG_LOGO_SUPPORT
	 if ((ImageData[0] == 0x89) && (ImageData[1] == 0x50) && (ImageData[2] == 0x4E ) && (ImageData[3] == 0x47 ) && (ImageData[4] == 0x0D ) && (ImageData[5] == 0x0A ) && (ImageData[6] == 0x1A ) && (ImageData[7] == 0x0A ))
		return PNG_Logo;
#endif

	return Unsupported_Logo;
}

LOGO_TYPE GetOEMLogoType(UINT8 *ImageData)
{

#if SETUP_OEM_FORMAT_LOGO_SUPPORT
	if(IsOEMLogoType(ImageData))
		return OEM_Format_LOGO;
#endif

	return Unsupported_Logo;
}
/**
    Wrapper function to process ConvertOEMFormatToUgaBlt 
    functionallity based on SDL token.


    @param OEMImage Pointer to GIF file
    @param OEMImageSize Number of bytes in BmpImage
    @param UgaBlt Buffer containing UGA version of GifImage.
    @param UgaBltSize Size of UgaBlt in bytes.
    @param Height Height of UgaBlt/BmpImage in pixels
    @param Width Width of UgaBlt/BmpImage in pixels
    @param Animate 

    @retval EFI_SUCCESS UgaBlt and UgaBltSize are returned. 
    @retval EFI_UNSUPPORTED GifImage is not a valid *.GIF image
    @retval EFI_BUFFER_TOO_SMALL The passed in UgaBlt buffer is not big enough.
        UgaBltSize will contain the required size.
    @retval EFI_OUT_OF_RESOURCES No enough buffer to allocate

**/
EFI_STATUS ConvertOEMFormatToUgaBltWrapper ( IN VOID *OEMImage, IN UINT32 OEMImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINT32 *UgaBltSize, OUT UINT32 *Height, OUT UINT32 *Width, BOOLEAN * Animate)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

#if SETUP_OEM_FORMAT_LOGO_SUPPORT
            Status = ConvertOEMFormatToUgaBlt(
                                ImageData,
            			(UINT32)ImageSize,
            			UgaBlt,
            			(UINT32*)UgaBltSize,
            			(UINT32*)Height,
            			(UINT32*)Width,
				Animate
				);
#endif
	return Status ;
}



/**
    Wrapper function to process ConvertBmpToUgaBlt 
    functionallity based on SDL token.

    @param BmpImage Pointer to BMP file
    @param BmpImageSize Number of bytes in BmpImage
    @param UgaBlt Buffer containing UGA version of BmpImage.
    @param UgaBltSize Size of UgaBlt in bytes.
    @param PixelHeight Height of UgaBlt/BmpImage in pixels
    @param PixelWidth Width of UgaBlt/BmpImage in pixels

    @retval EFI_SUCCESS UgaBlt and UgaBltSize are returned. 
    @retval EFI_UNSUPPORTED BmpImage is not a valid *.BMP image
    @retval EFI_BUFFER_TOO_SMALL The passed in UgaBlt buffer is not big enough.
        UgaBltSize will contain the required size.
    @retval EFI_OUT_OF_RESOURCES No enough buffer to allocate

**/
EFI_STATUS ConvertBmpToUgaBltWrapper(
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **UgaBlt,
  IN OUT UINTN  *UgaBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

#if SETUP_BMP_LOGO_SUPPORT
	Status = ConvertBmpToUgaBlt(
	        BmpImage,
	        BmpImageSize,
	        UgaBlt,
	        UgaBltSize,
	        PixelHeight,
	        PixelWidth
	        );
#endif
	return Status ;
}

/**
    function to clear the logo

    @param VOID

    @retval VOID

**/
VOID CleanUpExtendedLogoWrapper()
{
#if SETUP_GIF_LOGO_SUPPORT
	CleanUpGif();
#endif
#if SETUP_OEM_FORMAT_LOGO_SUPPORT
	CleanUpOEMLogo();
#endif
   ClearGrphxScreen ();
}

/**
    function to Handle logo animation

    @param Attribute 
    @param CoordinateX INTN CoordinateY

    @retval VOID

**/
VOID DoLogoAnimateWrapper(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY)
{

#if SETUP_GIF_LOGO_SUPPORT
	DoGifAnimate(Attribute,CoordinateX,CoordinateY);
#endif

#if SETUP_OEM_FORMAT_LOGO_SUPPORT
	DoOEMLogoAnimate(Attribute,CoordinateX,CoordinateY);
#endif
}
/**
    Wrapper function to process ConvertGifToUgaBlt 
    functionallity based on SDL token.


    @param GifImage Pointer to GIF file
    @param GifImageSize Number of bytes in BmpImage
    @param UgaBlt Buffer containing UGA version of GifImage.
    @param UgaBltSize Size of UgaBlt in bytes.
    @param PixelHeight Height of UgaBlt/BmpImage in pixels
    @param PixelWidth Width of UgaBlt/BmpImage in pixels
    @param GifDelay 

    @retval EFI_SUCCESS UgaBlt and UgaBltSize are returned. 
    @retval EFI_UNSUPPORTED GifImage is not a valid *.GIF image
    @retval EFI_BUFFER_TOO_SMALL The passed in UgaBlt buffer is not big enough.
        UgaBltSize will contain the required size.
    @retval EFI_OUT_OF_RESOURCES No enough buffer to allocate

**/
EFI_STATUS
ConvertGifToUgaBltWrapper (
  IN  VOID      *GifImage,
  IN  UINTN     GifImageSize,
  IN OUT VOID   **UgaBlt,
  IN OUT UINTN  *UgaBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth,
  OUT BOOLEAN 	*Animate
  )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
#if SETUP_GIF_LOGO_SUPPORT 
	UINTN 		GifDelay;	

	GifDelay = 0;
	
	Status = ConvertGifToUgaBlt(
                GifImage,
                GifImageSize,
                UgaBlt,
                UgaBltSize,
                PixelHeight,
                PixelWidth,
                &GifDelay
                );
	//the first frame usually doesn't have valid data so we get the
	//second one and set the timer
	if(!EFI_ERROR(Status) )
	{
	    if(GifDelay)
	    {
			*Animate = TRUE;
	    }
	}

#endif
	return Status ;
}
/**
    Wrapper function to process ConvertJPEGToUgaBlt 
    functionallity based on SDL token.

    @param JPEGImage Pointer to the memory that has the 
    @param Image that need to be decoded.
    @param JPEGImageSize Size of the JPEG Image
    @param UgaBlt Two cases
        Case 1: Memory is expected to be allocated by the callee.
        Address provided as input should be NULL.
        pointer that will recieve the address of the UGA buffer.
        The buffer is allocated by callee.
        This buffer need to be freed by the caller.
        Case 2: Memory is allocated by the caller
    @param UgaBltSize Pointer that stores Size of UgaBlt 
        in bytes. This will be treated as an input. This should have 
        value ZERO for Case 2 above.
    @param PixelHeight Height of UgaBlt/JPEG Image in pixels
    @param PixelWidth Width of UgaBlt/JPEG Image in pixels
    @param pun32BufferWidth Width of UgaBlt/JPEG Buffer

    @retval EFI_SUCCESS The image was successfully decoded and placed in the buffer
    @retval EFI_UNSUPPORTED
    @retval EFI_BUFFER_TOO_SMALL
    @retval EFI_OUT_OF_RESOURCES

**/
EFI_STATUS ConvertJPEGToUgaBltWrapper ( 
	IN VOID *JPEGImage, 
	IN UINT32 JPEGImageSize, 
	IN OUT VOID **UgaBlt, 
	IN OUT UINT32 *UgaBltSize, 
	OUT UINT32 *PixelHeight, 
	OUT UINT32 *PixelWidth, 
	OUT UINT32 *pun32BufferWidth)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

#if SETUP_JPEG_LOGO_SUPPORT
	Status = ConvertJPEGToUgaBlt(
                JPEGImage,
                (UINT32)JPEGImageSize,
                UgaBlt,
                (UINT32*)UgaBltSize,
                (UINT32*)PixelHeight,
                (UINT32*)PixelWidth,
                (UINT32*)pun32BufferWidth
                );
#endif 
	
	return Status;
}
/**
    Wrapper function to process ConvertPCXToUgaBlt 
    functionallity based on SDL token.

    @param PCXImage Pointer to the memory that has the PCX 
        Image that need to be decoded.
    @param PCXImageSize Size of the PCX Image
    @param UgaBlt Two cases
        Case 1: Memory is expected to be allocated by the callee.
        Address provided as input should be NULL.
        pointer that will receive the address of the UGA buffer. 
        The buffer is allocated by callee.	
        This buffer need to be freed by the caller. 		
        Case 2: Memory is allocated by the caller
    @param UgaBltSize Pointer that stores Size of UgaBlt in bytes. 
        This will be treated as an input. This should have value ZERO for Case 2 above.
    @param PixelHeight Height of UgaBlt/PCX Image in pixels
    @param PixelWidth Width of UgaBlt/PCX Image in pixels

    @retval EFI_STATUS

**/
EFI_STATUS ConvertPCXToUgaBltWrapper ( 
	IN VOID *PCXImage, 
	IN UINT32 PCXImageSize, 
	IN OUT VOID **UgaBlt, 
	IN OUT UINT32 *UgaBltSize, 
	OUT UINT32 *PixelHeight, 
	OUT UINT32 *PixelWidth)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

#if SETUP_PCX_LOGO_SUPPORT
	Status = ConvertPCXToUgaBlt ( 
		PCXImage, 
		PCXImageSize, 
		UgaBlt, 
		UgaBltSize, 
		PixelHeight, 
		PixelWidth);
#endif

	return Status;
}

/**
    Wrapper function to process ConvertPNGToUgaBlt 
    functionallity based on SDL token.

    @param PNGImage Pointer to the memory that has the PNG
        Image that need to be decoded.
    @param PNGImageSize Size of the PNG Image
    @param UgaBlt Two cases
        Case 1: Memory is expected to be allocated by the callee.
        Address provided as input should be NULL.
        pointer that will receive the address of the UGA buffer. 
        The buffer is allocated by callee.	
        This buffer need to be freed by the caller. 		
        Case 2: Memory is allocated by the caller
    @param UgaBltSize Pointer that stores Size of UgaBlt in bytes. 
        This will be treated as an input. This should have value ZERO for Case 2 above.
    @param PixelHeight Height of UgaBlt/PNG Image in pixels
    @param PixelWidth Width of UgaBlt/PNG Image in pixels

    @retval EFI_STATUS

**/
EFI_STATUS ConvertPNGToUgaBltWrapper ( 
	IN VOID *PNGImage, 
	IN UINT32 PNGImageSize, 
	IN OUT VOID **UgaBlt, 
	IN OUT UINT32 *UgaBltSize, 
	OUT UINT32 *PixelHeight, 
	OUT UINT32 *PixelWidth)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

#if SETUP_PNG_LOGO_SUPPORT
	Status = ConvertPNGToUgaBlt ( 
		PNGImage, 
		PNGImageSize, 
		UgaBlt, 
		UgaBltSize, 
		PixelHeight, 
		PixelWidth);
#endif

	return Status;
}

/**
    Wrapper function to process the RuntimeParsing Exit 
    functionallity based on SDL token.

    @param VOID

    @retval VOID

**/
VOID RTIfrProcessExitWrapper(VOID)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessExit();
#endif
}

/**
    Wrapper function to add runtime variable and Pages 
    based on SDL token.

    @param VOID

    @retval VOID

**/
VOID RTIfrProcessAddVarListAndPageIDListWrapper(VOID)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessAddVarListAndPageIDList();
#endif
}


/**
    Wrapper function to include RTIfrProcessFormIfUpdated
    based on SDL token.

    @param Link

    @retval Bool

**/
BOOLEAN RTIfrProcessFormIfUpdatedWrapper(UINT16 link)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	return RTIfrProcessFormIfUpdated(link);
#else 
	return FALSE;
#endif
}

/**
    Wrapper function to include RTIfrUpdateVariableInfo
    based on SDL token.

    @param ControlVariable, Handle

    @retval Bool

**/
#if UEFI_2_1_SUPPORT
BOOLEAN RTIfrUpdateVariableInfoWrapper(UINT32 ControlVariable, EFI_HANDLE Handle)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrUpdateVariableInfo(ControlVariable, Handle);
  return TRUE;
#else 
	return FALSE;
#endif
}
#endif

/**
    Wrapper function to process runtime Ifrs 
    based on SDL token.

    @param Link

    @retval Bool

**/
VOID RTIfrProcessRunTimeFormsWrapper(VOID*ref)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessRunTimeForms(ref);
#endif
}

EFI_STATUS Uefi20HiiInitializeProtocol( VOID );
EFI_STATUS Uefi21HiiInitializeProtocol(VOID);
EFI_STATUS HiiInitializeProtocol( VOID )
{
#if UEFI_2_1_SUPPORT
	return Uefi21HiiInitializeProtocol();
#else
	return Uefi20HiiInitializeProtocol();
#endif
}

CHAR16 *Uefi20HiiGetStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang );
CHAR16 *Uefi21HiiGetStringLanguage(VOID * handle, UINT16 token, CHAR16 *lang);
CHAR16 *HiiGetStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang )
{
#if UEFI_2_1_SUPPORT
	return Uefi21HiiGetStringLanguage(handle,token,lang);
#else
	return Uefi20HiiGetStringLanguage(handle,token,lang);
#endif
}

UINT16 Uefi20HiiChangeStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string );
UINT16 Uefi21HiiChangeStringLanguage(VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string);
UINT16 HiiChangeStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string )
{
#if UEFI_2_1_SUPPORT
	return Uefi21HiiChangeStringLanguage( handle, token, lang, string );
#else
	return Uefi20HiiChangeStringLanguage( handle, token, lang, string);
#endif
}

UINTN Uefi20HiiGetGlyphWidth(VOID);
UINTN Uefi20HiiGetGlyphHeight(VOID);
UINTN Uefi21HiiGetGlyphWidth(VOID);
UINTN Uefi21HiiGetGlyphHeight(VOID);
UINTN HiiGetGlyphWidth(VOID)
{
#if UEFI_2_1_SUPPORT
	return Uefi21HiiGetGlyphWidth();
#else
	return Uefi20HiiGetGlyphWidth();
#endif
}

UINTN HiiGetGlyphHeight(VOID)
{
#if UEFI_2_1_SUPPORT
	return Uefi21HiiGetGlyphHeight();
#else
	return Uefi20HiiGetGlyphHeight();
#endif
}


EFI_STATUS GetUnicodeCollection2Protocol(VOID **Protocol);
EFI_STATUS GetUnicodeCollectionProtocol(VOID **Protocol);
/**
        Procedure	:	InitUnicodeCollectionProtocol

        Description	:	function to add a string

        @param		:	. 

        @retval		:	status and VOID** UnicodeCollectionProtocol

**/
EFI_STATUS InitUnicodeCollectionProtocol(VOID **Protocol)
{
#if UEFI_2_1_SUPPORT
	return GetUnicodeCollection2Protocol(Protocol);
#else
	return GetUnicodeCollectionProtocol(Protocol);
#endif 
}

//----------------------------------------------------------------------------
// In Aptio UnicodeCollation2 protocol is supported in the same file as
// UnicodeCollation Protocol depending on EFI_SPECIFICATION_VERSION
//----------------------------------------------------------------------------
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation)
#if UEFI_2_1_SUPPORT 
#ifndef TSE_FOR_APTIO_4_50
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation2)
#endif
#endif

/**
        Procedure	:	MetaiMatch

        Description	:	Calles MetaiMatch of EFI_UNICODE_COLLATION2_PROTOCOL or EFI_UNICODE_COLLATION_PROTOCOL

        Input		:	

        Output		:	Boolean

**/
BOOLEAN MetaiMatch(VOID *Protocol,IN CHAR16 *String,IN CHAR16 *Pattern)
{
#if UEFI_2_1_SUPPORT
	return ((EFI_UNICODE_COLLATION2_PROTOCOL*)Protocol)->MetaiMatch((EFI_UNICODE_COLLATION2_PROTOCOL*)Protocol,String,Pattern);
#else
	return ((EFI_UNICODE_COLLATION_PROTOCOL*)Protocol)->MetaiMatch((EFI_UNICODE_COLLATION_PROTOCOL*)Protocol,String,Pattern);
#endif 
}

/**
        Procedure	:	StringColl

        Description	:	Calles StriColl of EFI_UNICODE_COLLATION2_PROTOCOL or EFI_UNICODE_COLLATION_PROTOCOL

        Input	:	VOID *Protocol,IN CHAR16 *String1,IN CHAR16 *String2

        Output	:	INTN

**/
INTN StringColl(VOID *Protocol,IN CHAR16 *String1,IN CHAR16 *String2)
{
#if UEFI_2_1_SUPPORT
	return ((EFI_UNICODE_COLLATION2_PROTOCOL*)Protocol)->StriColl((EFI_UNICODE_COLLATION2_PROTOCOL*)Protocol, String1, String2 );
#else
	return ((EFI_UNICODE_COLLATION_PROTOCOL*)Protocol)->StriColl((EFI_UNICODE_COLLATION_PROTOCOL*)Protocol, String1, String2 );
#endif 
}

UINTN TestPrintLength ( IN CHAR16   *String )
{
#if WIDE_GLYPH_SUPPORT
#if !UEFI_2_1_SUPPORT
	return HiiTestPrintLength ( String );
#else
	return UefiHiiTestPrintLength ( String );			
#endif /* #if !UEFI_2_1_SUPPORT */
#else
	return EfiTestPrintLength ( String );
#endif //WIDE_GLYPH_SUPPORT
}

//#undef TSE_CSM_SUPPORT
//#define TSE_CSM_SUPPORT 0


VOID CsmBBSSetBootPriorities( BOOT_DATA *pBootData, UINT16 *pOrder, UINTN u16OrderCount)
{
#if TSE_CSM_SUPPORT
	BBSSetBootPriorities( pBootData, pOrder, u16OrderCount);
#endif
}

BOOLEAN BBSValidDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath )
{
#if TSE_CSM_SUPPORT
	return IsBBSDevicePath(DevicePath);
#else
	return FALSE;
#endif
}

//----------------------------------------------------------------------------
// Procedure:    CsmBBSBootOptionName
//
// Description: Override function. OEM can override this function to change the 
//				default behaviour of function
//----------------------------------------------------------------------------
#if !OVERRIDE_CsmBBSBootOptionName
CHAR16 * CsmBBSBootOptionName( BOOT_DATA *bootData)
{
#if TSE_CSM_SUPPORT
	return GetBBSBootOptionName( bootData);
#else
	return NULL;
#endif
}
#endif

EFI_STATUS CsmBBSSetBootNowPriority( BOOT_DATA *BootData,UINTN uiPrefferedDevice,BOOLEAN ShowAllBbsDev)
{
#if TSE_CSM_SUPPORT
	return BBSSetBootNowPriority( BootData, uiPrefferedDevice, ShowAllBbsDev);
#else
	return EFI_UNSUPPORTED;
#endif
}


VOID CsmBBSGetDeviceList( VOID )
{
#if TSE_CSM_SUPPORT
	BBSGetDeviceList();
#endif
}
EFI_STATUS BBSLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath );
EFI_STATUS CsmBBSLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath )
{
#if TSE_CSM_SUPPORT
	return BBSLaunchDevicePath( DevicePath);
#else
	return EFI_UNSUPPORTED;
#endif
}


VOID CsmSaveBBSOrder( VOID *nvPtr )
{
#if TSE_CSM_SUPPORT
	SaveBBSOrder(nvPtr);
#endif
}

/**
        Procedure	:	CsmRearrangeBBSOrder

        Description	:	Calls RearrangeBBSOrder function

        Input	:	VOID * -> Cache buffer, VOID * -> Default buffer

        Output	:	VOID

**/
VOID RearrangeBBSOrder (NVRAM_VARIABLE *nvPtr, NVRAM_VARIABLE *DefaultBuffer);
VOID CsmRearrangeBBSOrder (VOID *nvPtr, VOID *DefaultBuffer)
{
#if TSE_CSM_SUPPORT
	RearrangeBBSOrder (nvPtr, DefaultBuffer);
#endif
}

VOID CsmBBSSetDisabled(UINT16 Index, VOID **DisDPs, UINTN *DPSize)
{
#if TSE_CSM_SUPPORT
	BBSSetDisabled(Index, DisDPs, DPSize);
#endif
}


VOID CsmRearrangeBBSOrderVariable(VOID *popupSel, UINT8 bIncrease,UINT16 *newOption)
{
#if TSE_CSM_SUPPORT
	RearrangeBBSOrderVariable(popupSel, bIncrease,newOption);
#endif
}

VOID CsmLoadDefaultLegDevOrder(VOID)
{
#if TSE_CSM_SUPPORT
	LoadDefaultLegDevOrder();
#endif
}

EFI_STATUS CsmBBSUpdateOrder(UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer)
{
#if TSE_CSM_SUPPORT
	BBSUpdateOrder(newOption,offset,size, buffer);
	return EFI_SUCCESS;
#else
	return EFI_INVALID_PARAMETER;
#endif
}

VOID TseBBSSetBootPriorities_BootOrder(UINT16 Priority)
{
#if TSE_CONTINUE_BOOT_NOW_ON_FAIL
	BBSSetBootPriorities_BootOrder(Priority);
#endif
}

VOID GetProgressColor(EFI_UGA_PIXEL * BGColor, EFI_UGA_PIXEL * BDRColor, EFI_UGA_PIXEL * FillColor)
{
	EFI_UGA_PIXEL	backgroundColor = PROGRESSBAR_BACKGROUNDCOLOR;
	EFI_UGA_PIXEL	borderColor = PROGRESSBAR_BORDERCOLOR;
	EFI_UGA_PIXEL	fillColor = PROGRESSBAR_FILLCOLOR;

	MemCopy( BGColor, &backgroundColor, sizeof(EFI_UGA_PIXEL) );
	MemCopy( BDRColor, &borderColor, sizeof(EFI_UGA_PIXEL) );
	MemCopy( FillColor, &fillColor, sizeof(EFI_UGA_PIXEL) );
}

///IDE Password Hooks...
UINTN   gCurrIDESecPage;

#if !OVERRIDE_TSEIDEPasswordGetName
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT  

    @param Index 

    @retval UINT16

**/
UINT16 TSEIDEPasswordGetName(UINT16 Index)
{	
#if SETUP_IDE_SECURITY_SUPPORT
	 return IDEPasswordGetName(Index);
#else
	return 0;
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordAuthenticate
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param Password VOID*  DataPtr, BOOLEAN bCheckUser

    @retval EFI_STATUS

**/
EFI_STATUS TSEIDEPasswordAuthenticate(CHAR16 *Password,  VOID*  DataPtr, BOOLEAN bCheckUser)
{
#if SETUP_IDE_SECURITY_SUPPORT
	 return IDEPasswordAuthenticate(Password, DataPtr, bCheckUser);
#else
	return FALSE;
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordUpdate
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param DeviceIndex CHAR16 *Password, BOOLEAN bCheckUser

    @retval BOOLEAN

**/
BOOLEAN TSEIDEPasswordUpdate( UINT32 DeviceIndex, CHAR16 *Password, BOOLEAN bCheckUser)
{
#if SETUP_IDE_SECURITY_SUPPORT
	 return IDEPasswordUpdate(DeviceIndex, Password, bCheckUser);
#else
	return FALSE;
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordGetDataPtr
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param Index 

    @retval VOID*

**/
VOID* TSEIDEPasswordGetDataPtr(UINTN Index)
{
#if SETUP_IDE_SECURITY_SUPPORT
	 return IDEPasswordGetDataPtr(Index);
#else
	return NULL;
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordGetLocked
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param Index 

    @retval BOOLEAN

**/
BOOLEAN TSEIDEPasswordGetLocked(UINTN Index)
{
#if SETUP_IDE_SECURITY_SUPPORT
	 return IDEPasswordGetLocked(Index);
#else
	return FALSE;
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordCheck
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSEIDEPasswordCheck()
{
	VOID *UgaBlt = NULL;
	UINTN SizeOfX=0, SizeOfY=0;
	VOID *TempScreenBuffer = (VOID*)NULL;

#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
	UgaBlt = SavePostScreen(&SizeOfX, &SizeOfY);
	//Save the contents of Active buffer so password code can use it
	SaveActiveBuffer(&TempScreenBuffer);
	IDEPasswordCheck();
	//Restore the original active buffer contents
	RestoreActiveBuffer(&TempScreenBuffer);
	RestorePostScreen( UgaBlt, SizeOfX, SizeOfY);
#endif
}
#endif

#if !OVERRIDE_TSEIDEPasswordFreezeDevices
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSEIDEPasswordFreezeDevices()
{
#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
	 IDEPasswordFreezeDevices();
#endif
}
#endif

#if !OVERRIDE_TSEUnlockHDD
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSEUnlockHDD()
{
#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
	 UnlockHDD();
#endif
}
#endif

#if !OVERRIDE_TSESetHDDPassword
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSESetHDDPassword()
{
#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
	 SetHDDPassword();
#endif
}
#endif

#if !OVERRIDE_TSEIDEUpdateConfig
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSEIDEUpdateConfig(VOID *ideSecConfig, UINTN value)
{
#if SETUP_IDE_SECURITY_SUPPORT
	 IDEUpdateConfig(ideSecConfig, value);
#endif
}
#endif

UINTN TseGetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background)
{
#if TSE_ANSI_ESC_CODE_SUPPORT
	return GetANSIEscapeCode(String, Bold, Foreground, Background);
#else
	return 0;
#endif
}

CHAR16 *TseSkipEscCode(CHAR16 *String)
{
#if TSE_ANSI_ESC_CODE_SUPPORT
	return (CHAR16 *)SkipEscCode(String);
#else
	return StrDup(String);
#endif
}

BOOLEAN TseCheckShiftState(AMI_EFI_KEY_DATA ActionKey, UINT32 HotkeyState)
{
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	return (TSE_CHECK_SHIFTSTATE(ActionKey.KeyState.KeyShiftState, HotkeyState));
#else
	return 1;
#endif
}

/**
    Function to return print screen support feature

    @param ScanCode 

    @retval TRUE/FALSE

**/
BOOLEAN TsePrintScreenEventSupport(UINT16 ScanCode)
{
#if TSE_PRN_SCRN_EVENT_SUPPORT
	if( ScanCode == TSE_PRN_SCRN_KEY_SCAN )
		return 1;
	else 
		return 0;
#else
	return 0;
#endif
}


VOID TseHotkeyPrintScreenSupport()
{
#if TSE_PRN_SCRN_EVENT_SUPPORT
	SupportPrintScreen();
#endif
}

BOOLEAN CheckSystemPasswordPolicy(UINT32 PasswordInstalled)
{
#if SMCPKG_SUPPORT
        AMITSESETUP AmiTseData;
        EFI_GUID gAmiTseSetupGuid = AMITSESETUP_GUID;
        UINTN VariableSize;
        EFI_STATUS Status;
        VariableSize = sizeof(AMITSESETUP);

    Status = gRT->GetVariable ( L"AMITSESetup", \
                                    &gAmiTseSetupGuid, \
                                    NULL, \
                                    &VariableSize, \
                                    &AmiTseData );

if (AmiTseData.CheckPassword == 0)//check password always or only setup?
        return (PasswordInstalled & AMI_PASSWORD_NONE)?TRUE:FALSE;
else
        return (PasswordInstalled & AMI_PASSWORD_ANY)?TRUE:FALSE;
#endif
#if SETUP_USER_PASSWORD_POLICY
	return (PasswordInstalled & AMI_PASSWORD_USER)?TRUE:FALSE;
#else
	return (PasswordInstalled & AMI_PASSWORD_ANY)?TRUE:FALSE;
#endif
}



extern VOID EfiStrCpy(IN CHAR16 *Destination, IN CHAR16 *Source);

/**
        Procedure	:	GetVariableNameByID

        Description	:	function to get Variable Name based on ID

        Input		:	UINT32 VariableID

        Output		:	CHAR16 *

**/
CHAR16 *GetVariableNameByID( UINT32 VariableID )
{
	CHAR16 *varName = (CHAR16 *)EfiLibAllocateZeroPool(VARIABLE_NAME_LENGTH);
	if(varName != NULL)
	{
		switch(VariableID)
		{
			case VARIABLE_ID_LANGUAGE:
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
				EfiStrCpy(varName, L"PlatformLang");
#else
				EfiStrCpy(varName, L"Lang");
#endif
				break;

			default:
				break;
		}
	}	

	return varName;
}

/**
        Procedure	:	GetGUIDNameByID

        Description	:	function to get GUID Name based on Variable ID

        Input		:	UINT32 VariableID

        Output		:	CHAR16 *

**/
CHAR16 *GetGUIDNameByID( UINT32 VariableID )
{
	CHAR16 *guidName = (CHAR16 *)EfiLibAllocateZeroPool(VARIABLE_NAME_LENGTH);
	if(guidName != NULL)
	{
		switch(VariableID)
		{
			case VARIABLE_ID_LANGUAGE:
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
				EfiStrCpy(guidName, L"PlatformLangCodes");
#else
				EfiStrCpy(guidName, L"LangCodes");
#endif
				break;
			default:
				break;
		}
	}
  return guidName;
}

VOID BootGetLanguages()
{
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
	GetPlatformBootLanguages();
#else
	GetBootLanguages();
#endif
}


BOOLEAN PlatformLangVerSupport()
{
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
	return 1;
#else
	return 0;
#endif
}

BOOLEAN BootNowInBootOrderSupport()
{
#if TSE_BOOT_NOW_IN_BOOT_ORDER
	return 1;
#else
	return 0;
#endif
}

BOOLEAN IsTseLoadPasswordOnDefaults()
{
	return TSE_LOAD_PASSWORD_ON_DEFAULTS;
}

UINT16 GetNoVarStoreBootCountOffset()
{
#if NO_VARSTORE_SUPPORT
	return (UINT16)(TSE_STRUCT_OFFSET(TSE_SETUP_DATA, BootCount));
#else
	return 0;
#endif
}


VOID SetPostScreenScrollArea(UINTN TopRow, UINTN TopCol, UINTN BottomRow, UINTN BottomCol);
VOID ConfigPostScrollArea(VOID)
{
	UINTN X[]= TSE_POSTSCREEN_SCROLL_AREA;
	SetPostScreenScrollArea(X[0],X[1],X[2],X[3]);
}


////////// BOARD MODULE FUNCTIONS /////////////

#if !OVERRIDE_CheckIsAllowedPasswordChar
/**
    Check is the char allowed for password control 
    based on SDL token.

    @param Input character

    @retval Bool

**/
BOOLEAN CheckIsAllowedPasswordChar(CHAR16 Char)
{
#if PASSWORD_WITH_SPECIAL_CHAR_SUPPORT
	return (BOOLEAN)_CharIsAlphaNumericSpecial(Char);
#else
	return (BOOLEAN)_CharIsAlphaNumeric(Char);
#endif
}
#endif

#if !OVERRIDE_PopupPasswordFormCallback
EFI_STATUS PopupPasswordFormCallback(/*CONTROL_INFO*/VOID * pControlData,UINT16 Key,UINT8 Flags)
{
	return EFI_UNSUPPORTED;
}
#endif

#if !OVERRIDE_PopupPwdHandleActionOverRide
EFI_STATUS _PopupPasswordHandleAction( VOID *popuppassword, VOID *Data);
EFI_STATUS 	PopupPwdHandleActionOverRide(VOID *popuppassword, VOID *Data)
{
	return _PopupPasswordHandleAction(popuppassword, Data);
}
#endif

#if !OVERRIDE_PasswordCheckInstalled
UINT32 PasswordCheckInstalledLocal(VOID);
UINT32 PasswordCheckInstalled( VOID )
{
	UINT32	Installed = AMI_PASSWORD_NONE;

	Installed = PasswordCheckInstalledLocal();

	return Installed;
}
#endif

#if !OVERRIDE_PasswordAuthenticate
UINT32 PasswordAuthenticateLocal( CHAR16 *Password );
UINT32 PasswordAuthenticate( CHAR16 *Password )
{
	UINT32 PasswordType = AMI_PASSWORD_NONE;

	PasswordType = PasswordAuthenticateLocal(Password);

	return PasswordType;
}
#endif

#if !OVERRIDE_PasswordUpdate
CHAR16 *PasswordUpdateLocal( CHAR16 *Password, UINTN Size );
CHAR16 *PasswordUpdate( CHAR16 *Password, UINTN Size )
{
	CHAR16 *Buffer = NULL;

	Buffer = PasswordUpdateLocal(Password, Size);

	return Buffer;
}
#endif

#if !OVERRIDE_PasswordCommitChanges
VOID PasswordCommitChanges( BOOLEAN SaveChanges )
{
}
#endif

#if !OVERRIDE_PopupPasswordCheckInstalled
UINT32 PopupPasswordCheckInstalledLocal(VOID *popuppassword);
UINT32 PopupPasswordCheckInstalled(VOID *popuppassword)
{
	return PopupPasswordCheckInstalledLocal(popuppassword);
}
#endif

#if !OVERRIDE_PopupPasswordAuthenticate
BOOLEAN PopupPasswordAuthenticateLocal( VOID *popuppassword, CHAR16 *Password );
BOOLEAN PopupPasswordAuthenticate( VOID *popuppassword, CHAR16 *Password )
{
	return PopupPasswordAuthenticateLocal( popuppassword, Password );
}
#endif

/*
BOOLEAN IsPasswordSupportNonCaseSensitive()
{
#if SETUP_PASSWORD_NON_CASE_SENSITIVE
	return TRUE;
#endif
	return FALSE;
}
*/

#if !OVERRIDE_NoVarStoreSupport
BOOLEAN NoVarStoreSupport(VOID)
{
	return NO_VARSTORE_SUPPORT;
}
#endif

#if !OVERRIDE_NoVarStoreUpdateSystemAccess
VOID NoVarStoreUpdateSystemAccess(UINT8 sysAccessValue)
{
}
#endif

#if !OVERRIDE_ItkSupport
BOOLEAN ItkSupport(VOID)
{
	return SETUP_ITK_COMPATIBILITY;
}
#endif


#if !OVERRIDE_BbsItkBoot
EFI_STATUS BbsItkBoot()
{
	return EFI_UNSUPPORTED;
}
#endif

#if !OVERRIDE_GetAMITSEVariable
VOID GetAMITSEVariableLocal(VOID **mSysConf, UINT8 **setup, UINTN *VarSize);
VOID GetAMITSEVariable(VOID **mSysConf, UINT8 **setup, UINTN *VarSize)
{
	GetAMITSEVariableLocal(mSysConf, setup, VarSize);
}
#endif

#if !OVERRIDE_GetBootTimeOut
UINT16 GetBootTimeOutLocal(UINT16 DefaultValue);
UINT16 GetBootTimeOut(UINT16 DefaultValue)
{
	UINT16 Val;

	Val = GetBootTimeOutLocal(SETUP_DEFAULT_TIMEOUT);

	if(Val == 0xFFFF)
		return Val;
	if(SETUP_TIMEOUT_IN_TENTHOFSEC)
		return Val; //timeout Value Calcualted in 1/0 Sec. Value is already in 1/10 Sec.
	else
	{
		//timeout Value Calcualted in 1/0 Sec. Value is in Sec and Multiply by 10 to make 1/10.
		if((65530/10) > Val) // To avoid overflow
			return Val*10; 
		else
			return 0xFFFE; // return Max timeout possible.
	}

}
#endif

#if !OVERRIDE_VarBuildItkDefaults
VOID VarBuildItkDefaults(VOID)
{
}
#endif

#if !OVERRIDE_SetSystemAccessValueItk
VOID SetSystemAccessValueItk(UINT8 sysAccessValue)
{
}
#endif

#if !OVERRIDE_FindVarFromITKQuestionId
UINT32 FindVarFromITKQuestionId(UINT16 QuestionId)
{
    return 0;
}
#endif

#if !OVERRIDE_IsOEMLogoType
BOOLEAN IsOEMLogoType(UINT8 *ImageData)
{
	// Return TRUE if OEM format.
	return FALSE;
}
#endif

#if !OVERRIDE_ConvertOEMFormatToUgaBlt
EFI_STATUS ConvertOEMFormatToUgaBlt ( IN VOID *OEMImage, IN UINT32 OEMImageSize, IN OUT VOID **UgaBlt, 
		IN OUT UINT32 *UgaBltSize, OUT UINT32 *Height, OUT UINT32 *Width, BOOLEAN * Animate)
{
	// Use this override function to convert the oem image to UGABlt.
	return EFI_UNSUPPORTED;
}
#endif

#if !OVERRIDE_CleanUpOEMLogo
VOID CleanUpOEMLogo(VOID)
{
	//Use this override function to free up memory If needed
}
#endif

#if !OVERRIDE_DoOEMLogoAnimate
VOID DoOEMLogoAnimate(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY)
{
	//Takecare to init the timer to animate.
}
#endif

#if !OVERRIDE_BBSBuildName
VOID BBSBuildName( CHAR16 *String, VOID *info, UINTN index, VOID *HDDInfo )
{
#if TSE_CSM_SUPPORT
#if TSE_DEVICE_PATH_NAME
	BBSBuildDevicePathName(String, info, index, HDDInfo);
#else
	BBSBuildDefaultName(String, info, index, HDDInfo );
#endif
#endif
}
#endif

VOID StopClickEvent(VOID);

/**
    Set OVERRIDE_MainSetupLoopHook to 1, if OEM wants to override the MainSetupLoopHook function from Board module.

    @param VOID

    @retval EFI_STATUS

**/
#if !OVERRIDE_MainSetupLoopHook
EFI_STATUS MainSetupLoop( VOID );
EFI_STATUS MainSetupLoopHook( VOID )
{
	StopClickEvent();
	return MainSetupLoop();
}
#endif
#if EFI_SPECIFICATION_VERSION>=0x2000A   
/**
    Set OVERRIDE_ProcessBrowserActionRequestHook to 1, if OEM wants to override the ProcessBrowserActionRequestHook function from Board module.

    @param ActionReq 

    @retval EFI_STATUS

**/
#if !OVERRIDE_ProcessBrowserActionRequestHook
EFI_STATUS ProcessBrowserActionRequest(EFI_BROWSER_ACTION_REQUEST ActionReq);
EFI_STATUS ProcessBrowserActionRequestHook(EFI_BROWSER_ACTION_REQUEST ActionReq)
{
	return ProcessBrowserActionRequest(ActionReq);
}
#endif
#endif
/**
    Set OVERRIDE_BBSGetNonStandardGroupType to 1, then add the OEM 
    specific function to override this function to provide a non standard 
    group type for the given Device Type.

    @param Device Type

    @retval String Token

**/
#if !OVERRIDE_BBSGetNonStandardGroupType
UINT16 BBSGetNonStandardGroupType(UINT16 DeviceType)
{
	// By default return the Unknown order
	return STRING_TOKEN(STR_UNKNOWN_ORDER);
}
#endif


/**
    Hook function which is called when Mouse init is done. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEMouseInitHook
VOID TSEMouseInitHook(VOID)
{
}
#endif

/**
    Hook function which is called when Mouse Stop is done. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEMouseStopHook
VOID TSEMouseStopHook(VOID)
{
}
#endif

/**
    Hook function which is called when Mouse Refresh action is done. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEMouseRefreshHook
VOID TSEMouseRefreshHook(VOID)
{
}
#endif

/**
    Hook function which is called when Mouse start action is done. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEMouseStartHook
VOID TSEMouseStartHook(VOID)
{
}
#endif

/**
    Hook function which is called when Mouse Destory is done. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEMouseDestoryHook
VOID TSEMouseDestroyHook(VOID)
{
}
#endif


/**
    Hook function which can make TSE to Ignore the mouse action.
    This function is called whenever TSE read the mouse action 
    This hook need to be used carefully. It can used in the cases when Mouse 
    actions consumes by modules like Softkeyboard and not to be used by TSE 

    @param VOID

        BOOLEAN
    @retval TRUE Mouse actions will be ignored by TSE
    @retval FALSE Mouse action

**/
#if !OVERRIDE_TSEMouseIgnoreMouseActionHook
BOOLEAN TSEMouseIgnoreMouseActionHook(VOID)
{
    return FALSE;    
}
#endif

/**
    Hook function which is called when Strings/Password is going to be read. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEStringReadLoopEntryHook
VOID TSEStringReadLoopEntryHook(VOID)
{
}
#endif

/**
    Hook function which is called after Strings/Password read. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_TSEStringReadLoopExitHook
VOID TSEStringReadLoopExitHook(VOID)
{
}
#endif

EFI_STATUS GetAdvanceDeviceName( CHAR16 *String, VOID *info, UINTN index, VOID *HDDInfo )
{
#if TSE_ADVANCED_BIN_SUPPORT
	BBSBuildDevicePathName(String, info, index, HDDInfo);
	return EFI_SUCCESS;
#endif
	return EFI_UNSUPPORTED;
}

CHAR16 *GetUefiDevPathString(EFI_DEVICE_PATH_PROTOCOL *DevPath);
CHAR16 *TseGetUefiDevPathString(EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
#if TSE_DEVICE_PATH_UEFI_NAME
	return GetUefiDevPathString(DevPath);
#endif
	return NULL;
}

/////////////OEM_SPECIAL_CONTROL////////////////
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT

/**
    OEM Hook Function can allows exploding into multiple controls.
    Based on the OEM policies it returns number controls need to exploded.

				
    @param controlInfo 

    @retval UINT16 Control Count.

**/
#if !OVERRIDE_OEMSpecialGetControlCount
UINT16 OEMSpecialGetControlCount(CONTROL_INFO *controlInfo)
{
// SAMPLE CODE: START

// SAMPLE Code : for One of Special control returns 1 and for Other(Submenu) it returns the OemTseVar
/*
	if(controlInfo->ControlType == CONTROL_TYPE_POPUPSEL)
		return 1;
	else
	{
		UINT16 * pValue=NULL,Val;
		UINTN size;

		pValue = VarGetVariable( VARIABLE_ID_OEM_TSE_VAR, &size );
		if(pValue)
		{
			Val = *pValue;
			MemFreePointer((VOID **) &pValue);
			return  Val;
		}
		else
			return 1;
	}
*/
// SAMPLE CODE: END

	//Add the OEM code like above for customization
	
	return 1;
}
#endif
/**
    If the OEM Special control is One of Control the members of the one of control
    Can be fixed up in this function. If OEM needs multiple control,
    for each one of it will be called.
    This function has to create dynamic IFR Oneof Options

    @param control ControlInfo that need to be patched.
    @param value Index of the control that need to be pathched.

    @retval VOID

**/
#if !OVERRIDE_OEMSpecialOneOfFixup
VOID OEMSpecialOneOfFixup( CONTROL_INFO *control , UINT16 value )
{
// SAMPLE CODE: START

// Sample Code: To create the One of options based in the OemTSEVar count.
/*
	EFI_IFR_ONE_OF *ifrPtr;
	EFI_IFR_ONE_OF_OPTION *templatePtr, *oneOf;

	VOID *tempPtr;
	UINTN i, size;
	CHAR16 Number[]=L"0";
	UINT16 * pValue=NULL,Val=0;

	pValue = VarGetVariable( VARIABLE_ID_OEM_TSE_VAR, &size );
	if(pValue)
	{
		Val = *pValue;
		MemFreePointer((VOID **) &pValue);
	}

    //Create one one_of_option for each OEM option 
	size = sizeof(EFI_IFR_ONE_OF) + sizeof(EFI_IFR_END_ONE_OF) + (Val) * sizeof(EFI_IFR_ONE_OF_OPTION);
	tempPtr = EfiLibAllocateZeroPool( size );
	if ( tempPtr == NULL )
		return;

	ifrPtr = (VOID *)control->ControlPtr;
	MemCopy( tempPtr, ifrPtr, ifrPtr->Header.Length );
	control->ControlPtr = (UINTN)tempPtr;

	templatePtr = (EFI_IFR_ONE_OF_OPTION *)((UINTN)ifrPtr + ifrPtr->Header.Length);
	oneOf = (EFI_IFR_ONE_OF_OPTION *)((UINTN)tempPtr + ifrPtr->Header.Length);
	for ( i = 0; i < Val; i++, oneOf++ )
	{
		MemCopy( oneOf, templatePtr, sizeof(EFI_IFR_ONE_OF_OPTION) );
		oneOf->Value = (UINT16)i+1;
		Number[0]=L'0'+(UINT16)i+1;
		oneOf->Option = HiiAddString( control->ControlHandle, Number );
	}

	oneOf->Header.OpCode = EFI_IFR_END_ONE_OF_OP;
	oneOf->Header.Length = sizeof(EFI_IFR_END_ONE_OF);

	ifrPtr = (EFI_IFR_ONE_OF *)control->ControlPtr;
	control->ControlHelp = ifrPtr->Help;
*/
// SAMPLE CODE: END

	//Add the OEM code like above for customization

	return;
}
#endif
/**
    Hook to update the OEM Variable based on Selection.
    TSE updates the varible buffer in the normal way (updates with option value selected)
    If any OEM Special updatation required OEM may use this function and return EFI_SUCCESS.
				

    @param newOption Option value selected.
    @param offset Variable offset
    @param Size size of the variable
    @param buffer OutBuffer Allocate and fill this if OEM Updated the buffer. (Return Success)

    @retval EFI_SUCCESS If OEM Updates the Buffer with OEM policy
        EFI_ERROR (EFI_INVALID_PARAMETER) - Standard TSE Update is enough. No Updates done for OEM

**/
#if !OVERRIDE_OEMSpecialUpdateOneOf
EFI_STATUS OEMSpecialUpdateOneOf(UINT16 newOption,UINT32 *offset,UINTN *size, VOID OUT **buffer)
{

	return EFI_INVALID_PARAMETER;
}
#endif
/**
    If the OEM Special control is Submenu Control. Based on the OEMSpecialGetControlCount
    The submenus are added with same promt/help. If OEM want to Fix the prompt and help fields
    of the submenu control. It can be done here.
    For  each submenu of that OEM special control it is called.

    @param ctrlData Control data.
    @param value Index of the control that need to be pathched.

    @retval VOID

**/
#if !OVERRIDE_OEMSpecialGotoFixup
VOID OEMSpecialGotoFixup(CONTROL_INFO *control, UINT16 value )
{
// SAMPLE CODE: START
	// Sample Code: To Fix the OEM One of controls prompt based on Value.
/*
UINTN
SPrint (
  OUT CHAR16        *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *Format,
  ...
  );

	CHAR16 str[100];
	SPrint(str,200,L"OEM Goto Control %d",value);
	((EFI_IFR_REF *)control->ControlPtr)->Prompt = HiiAddString( control->ControlHandle,str);
*/
// SAMPLE CODE: END

	//Add the OEM code like above for customization

	return;
}
#endif
/**
    Hook for OEM Goto control Action Handler.
    On Selection of the OEM Goto control it is called.

    @param value Index of the control that is selected

    @retval VOID

**/
#if !OVERRIDE_OEMSpecialGotoSelect
VOID OEMSpecialGotoSelect(UINT16 value)
{
// SAMPLE CODE: START
/*
	// Sample Code: To display the message box on what is selected.

	CHAR16 str[100];
	UINT8 ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8   MsgBoxType);

	SPrint(str,200,L"OEM Goto Control %d Selected",value);
	ShowPostMsgBox(L"OEM Goto Select",str,MSGBOX_TYPE_OK);
*/
// SAMPLE CODE: END

	//Add the OEM code like above for customization

	return;
}
#endif

#endif // SETUP_OEM_SPECIAL_CONTROL_SUPPORT

/**
    Set OVERRIDE_OemCheckControlCondition to 1, if OEM wants to override the OemCheckControlCondition function from Board module.

    @param FinalCond CONTROL_INFO *controlInfo

    @retval UINT8

**/
#if !OVERRIDE_OEMCheckControlCondition
UINT8 OEMCheckControlCondition(UINT8 FinalCond, CONTROL_INFO *controlInfo)
{
	return FinalCond;
}
#endif

#if !OVERRIDE_FastBootLaunch
EFI_STATUS FastBootLaunch()
{
    return EFI_UNSUPPORTED;
}
#endif

/**
    Hook function to set Messagebox Color. 

    @param Color 

    @retval VOID

**/
#if !OVERRIDE_GetMessageboxColorHook
VOID GetMessageboxColorHook(UINT8 **Color)
{
//	**Color = EFI_BACKGROUND_LIGHTGRAY | EFI_BLUE;	
}
#endif

/**
    Hook function to set GOPScreen Resolution. 

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_GOPSetScreenResolutionHook
EFI_STATUS GOPSetScreenResolutionHook(UINTN *Width, UINTN *Height, UINT32 Index)
{
	return EFI_SUCCESS;
}
#endif

/**
    Board module hook for LaunchHotKeyBootOption function

    @param BOOT_FLOW *

    @retval EFI_STATUS

**/
#if !OVERRIDE_LaunchHotKeyBootOption
EFI_STATUS LaunchHotKeyBootOption (BOOT_FLOW *Bflow)
{
    if (-1 != gHotKeyBootOption)
    {
        return (BootLaunchBootOption ((UINT16)gHotKeyBootOption, NULL, 0) );
    }
    return EFI_NOT_FOUND;
}
#endif

/**
    Board module hook for DriverHealthSystemReset function

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_DriverHealthSystemReset
VOID DriverHealthSystemReset (VOID)
{
    CHAR16 *Text = NULL;
    Text = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_REBOOT_POST));
    if(Text != NULL)
   	 PostManagerDisplayPostMessage (Text);
    MemFreePointer( (VOID **)&Text );
    gBS->Stall (2000000);
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
}
#endif

void SetDesiredTextMode();
/**
    Board module hook for LegacyBootFailHook function
    This function invoked when LoadImage fails

    @param EFI_STATUS => Status returned from CsmBBSLaunchDevicePath

    @retval VOID

**/
void CheckandDeactivateSoftkbd(); 
#if !OVERRIDE_LegacyBootFailHook
VOID LegacyBootFailHook (EFI_STATUS Status)
{
	UINT8 	Sel;
	UINT16 	OptionToken = STR_CTRL_OK;
    CHAR16	*Title = NULL;
    CHAR16	*Message = NULL;
	AMI_POST_MGR_KEY Key;

	if(EFI_SECURITY_VIOLATION == Status)
	{
		
		CheckandDeactivateSoftkbd ();
#if (!(ESA_BINARY_SUPPORT && defined(ESA_TSE_FULL_SUPPORT )&& (ESA_TSE_FULL_SUPPORT ==0)) && !(defined(BUILD_FOR_ESA) && (ESA_TSE_FULL_SUPPORT == 1)))
		MouseInit();
		SetDesiredTextMode();
		ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
#endif		
	   Title = HiiGetString (gHiiHandle, STRING_TOKEN (STR_SECBOOT_VIOLATION));
		Message = HiiGetString (gHiiHandle, STRING_TOKEN (STR_SECBOOT_INV_SIGN));
		
		PostManagerDisplayMsgBoxEx (
			Title,
			Message,
			NULL,
			MSGBOX_EX_CATAGORY_ERROR,
			MSGBOX_TYPE_CUSTOM,
			&OptionToken,
			1,
			NULL,
			0,
			&Sel,
			&Key
			);
		MemFreePointer( (VOID **)&Title );
		MemFreePointer( (VOID **)&Message );
#if (!(ESA_BINARY_SUPPORT && defined(ESA_TSE_FULL_SUPPORT )&& (ESA_TSE_FULL_SUPPORT ==0)) && !(defined(BUILD_FOR_ESA) && (ESA_TSE_FULL_SUPPORT == 1)))
		MouseDestroy(); 
#endif		
	} 
}
#endif

/**
    Board module hook for UefiBootFailHook function
    This function invoked when LoadImage fails

    @param EFI_STATUS => Status returned from LoadImage

    @retval VOID

**/
#if !OVERRIDE_UefiBootFailHook
VOID UefiBootFailHook (EFI_STATUS Status)
{
	UINT8 	Sel;
	UINT16 	OptionToken = STR_CTRL_OK;
    CHAR16	*Title = NULL;
    CHAR16	*Message = NULL;
	AMI_POST_MGR_KEY Key;

	if ((EFI_SECURITY_VIOLATION == Status) || (EFI_ACCESS_DENIED == Status))
	{
		CheckandDeactivateSoftkbd ();

#if (!(ESA_BINARY_SUPPORT && defined(ESA_TSE_FULL_SUPPORT )&& (ESA_TSE_FULL_SUPPORT ==0)) && !(defined(BUILD_FOR_ESA) && (ESA_TSE_FULL_SUPPORT == 1)))
		MouseInit();
		SetDesiredTextMode();	
		ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
		ClearFlushBuffer(EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY);
#endif
		
		Title = HiiGetString (gHiiHandle, STRING_TOKEN (STR_SECBOOT_VIOLATION));
		Message = HiiGetString (gHiiHandle, STRING_TOKEN (STR_SECBOOT_INV_SIGN));
		PostManagerDisplayMsgBoxEx (
			Title,
			Message,
			NULL,
			MSGBOX_EX_CATAGORY_ERROR,
			MSGBOX_TYPE_CUSTOM,
			&OptionToken,
			1,
			NULL,
			0,
			&Sel,
			&Key
			);
		MemFreePointer( (VOID **)&Title );
		MemFreePointer( (VOID **)&Message );
		InvalidateStatusInBgrtWrapper ();
#if (!(ESA_BINARY_SUPPORT && defined(ESA_TSE_FULL_SUPPORT )&& (ESA_TSE_FULL_SUPPORT ==0)) && !(defined(BUILD_FOR_ESA) && (ESA_TSE_FULL_SUPPORT == 1)))
		MouseDestroy(); 
#endif
	} 
}
#endif

/**
    Board module hook for UpdateNumericDisplayString function
    This function invoked before displaying the numeric control
    Can use this function to change the display style of numeric control

    @param CHAR16 ** => Numeric string to be formatted
        INT64 => Numeric data to be formatted
        UINT8 => Base of the numeric control, might be AMI_BASE_HEX, AMI_BASE_INT_DEC, AMI_BASE_DEC
        UINT16 => Width of the numeric control
        UINT16 => Size of the CHAR16 **Text

    @retval VOID

**/
#if !OVERRIDE_UpdateNumericDisplayString
VOID UpdateNumericDisplayString (CHAR16 **Text, INT64 Num, UINT8 Base, UINT16 ControlDataWidth, UINT16 SizeofText)
{
#if 0			//Replace 0 with 1
	#define AMI_BASE_HEX 16
//Example for showing the numeric hex variables in uppercase and with leading zeroes
	if (AMI_BASE_HEX == Base)
	{
		SPrint (*Text, SizeofText, L"%0*X", (ControlDataWidth * 2), Num);
	}
#endif
}
#endif

/**
    Returns the Specification version assigned to TSE_CALLBACK_SPEC_VERSION

    @param VOID

    @retval UINT32 => Callback Version

**/
#if !OVERRIDE_GetUefiSpecVersion
UINT32 GetUefiSpecVersion (VOID)
{
#if TSE_CALLBACK_SPEC_VERSION
	return TSE_CALLBACK_SPEC_VERSION;
#else
	return gST->Hdr.Revision;
#endif
}
#endif

/**

    @param BOOT_FLOW *

    @retval EFI_STATUS

**/
EFI_STATUS	FBBootFlow ( BOOT_FLOW * Bflow)
{
	gBootFlow = BOOT_FLOW_CONDITION_FAST_BOOT;
	return EFI_SUCCESS;
}

/**
    Returns the value for grouping the boot options

    @param void

    @retval UINT8 Value for grouping boot options

**/

UINT8   IsBootOptionsGroupingEnabled (void)
{
#ifdef GROUP_BOOT_OPTIONS_BY_TAG
    return ((UINT8)GROUP_BOOT_OPTIONS_BY_TAG);
#else              
    return 1;        //In older version grouping is defaultly enabled
#endif
}

/**
    Returns the value for hiding the bios signon msg

    @param void

    @retval UINT8 Value for hiding bios signon msg

**/

UINT8   IsSetupHideBiosSignOnMsg (void)
{
#ifdef SETUP_HIDE_BIOS_SIGNON_MESSAGE2
    return ((UINT8)SETUP_HIDE_BIOS_SIGNON_MESSAGE2);
#else              
    return 0;        //In older version no chance for hiding sign on msg
#endif
}
/**
    To Return OEMLogo Draw Support token value

    @param VOID

    @retval BOOLEAN

**/

BOOLEAN IsOEMPOSTLogoSupport()
{
#ifdef TSE_OEM_POSTLOGO_SUPPORT
	return TSE_OEM_POSTLOGO_SUPPORT;
#else
	return 0;
#endif
}

/**
        This is Override function. OEM can override the default behaviour

    Return the graphics image file named FileNameGuid into Image
    and return it's size in ImageSize. All Firmware Volumes (FV)
    in the system are searched for the file name.

    @param FileNameGuid File Name of graphics file in the FV(s).
    @param Image Pointer to pointer to return graphics image.
        If NULL, a buffer will be allocated.
    @param ImageSize Size of the graphics Image in bytes. Zero if no image found.

    @retval EFI_SUCCESS Image and ImageSize are valid.
    @retval EFI_BUFFER_TOO_SMALL Image not big enough. ImageSize has required size
    @retval EFI_NOT_FOUND FileNameGuid not found

**/
#if !OVERRIDE_GetGraphicsBitMapFromFV
EFI_STATUS
GetGraphicsBitMapFromFV ( IN EFI_GUID *FileNameGuid, OUT VOID **Image, OUT UINTN *ImageSize )
{
	EFI_STATUS                    Status;
	UINTN                         FvProtocolCount;
	EFI_HANDLE                    *FvHandles;
	UINTN                         Index;
	UINT32                        AuthenticationStatus;

#if (TSE_ROMHOLE_SUPPORT)	
	EFI_FV_FILE_ATTRIBUTES		Attrib 			= (EFI_FV_FILE_ATTRIBUTES)NULL;
	EFI_FIRMWARE_VOLUME_HEADER  *FvVolume 		= (EFI_FIRMWARE_VOLUME_HEADER  *)NULL;
	EFI_HANDLE					processFvHandle	= NULL;	
	EFI_FV_FILETYPE				FvType;
	DXE_SERVICES *DXE_Service = NULL;
	EFI_TPL CurrentTpl = 0;
	EFI_GUID DxeServicesTableGuid = DXE_SERVICES_TABLE_GUID;
	EFI_GUID AMIROMHOLEGuid = TSE_ROMHOLE_HEADER_GUID;
#endif

#if (PI_SPECIFICATION_VERSION < 0x00010000)
	EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
#else
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
#endif

	Status = gBS->LocateHandleBuffer (
                  ByProtocol,
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                  &gEfiFirmwareVolumeProtocolGuid,
#else
                  &gEfiFirmwareVolume2ProtocolGuid,
#endif
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );
	if (EFI_ERROR (Status)) 
		return EFI_NOT_FOUND;

	for (Index = 0; Index < FvProtocolCount; Index++)
	{
		Status = gBS->HandleProtocol (
                   FvHandles[Index],
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                   &gEfiFirmwareVolumeProtocolGuid,
#else
                   &gEfiFirmwareVolume2ProtocolGuid,
#endif
                   (VOID **) &Fv
                   );

		// Assuming Image and ImageSize are correct on input.
		Status = Fv->ReadSection (
		          Fv,
                  FileNameGuid,
                  EFI_SECTION_RAW,
                  0,
                  Image,
                  ImageSize,
                  &AuthenticationStatus
                  );

		if (!EFI_ERROR (Status))
		{
		//81617 : Avoiding memory leaks in TSE
			gBS->FreePool(FvHandles);
			return EFI_SUCCESS;
		}
		else if (Status == EFI_BUFFER_TOO_SMALL) // ImageSize updated to needed size so return
		{
		//81617 : Avoiding memory leaks in TSE
			gBS->FreePool(FvHandles);
            return EFI_BUFFER_TOO_SMALL;
		}
	}

#if (TSE_ROMHOLE_SUPPORT)

	for (Index = 0; Index < FvProtocolCount; Index++)
	{
		Status = gBS->HandleProtocol (
                   FvHandles[Index],
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                   &gEfiFirmwareVolumeProtocolGuid,
#else
                   &gEfiFirmwareVolume2ProtocolGuid,
#endif
                   (VOID **) &Fv
                   );
		if (EFI_ERROR (Status)) {
			continue;
		}

		// Get FV buffer and FV_ImageSize from RomHole FV
		Status = Fv->ReadFile (
		          Fv,
                  &AMIROMHOLEGuid,
                  Image,
                  ImageSize,
				  &FvType,
				  &Attrib,
                  &AuthenticationStatus
                  );

		if (!EFI_ERROR (Status))//If it found before end of iteration then break the loop
		{
			if (*ImageSize)
				*ImageSize = 0;
			break;
		}

	}

	MemFreePointer ((VOID **)&FvHandles);
	if (EFI_ERROR (Status)){		
		return EFI_NOT_FOUND;
	}

	DXE_Service = (DXE_SERVICES *)GetEfiConfigurationTable(gST,&DxeServicesTableGuid);	
	FvVolume = (EFI_FIRMWARE_VOLUME_HEADER  *)*Image;

	//Raise TPL level to Install FV protocol for ROMHole
	CurrentTpl = gBS->RaiseTPL( TPL_HIGH_LEVEL );
	gBS->RestoreTPL( TPL_APPLICATION);

	//Install new FV protocol for ROMHole
	Status = DXE_Service->ProcessFirmwareVolume (FvVolume, FvVolume->HeaderLength, &processFvHandle);

	//Free Image data after ProcessFirmwareVolume
	MemFreePointer((VOID **)Image);
		
	if (EFI_ERROR(Status)){
		return Status;
	}

	gBS->RaiseTPL( TPL_HIGH_LEVEL );
	gBS->RestoreTPL( CurrentTpl );	

	Status = gBS->LocateHandleBuffer (
                  ByProtocol,
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                  &gEfiFirmwareVolumeProtocolGuid,
#else
                  &gEfiFirmwareVolume2ProtocolGuid,
#endif
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );

	if (EFI_ERROR (Status)){		
		return EFI_NOT_FOUND;
	}

	for (Index = 0; Index < FvProtocolCount; Index++)
	{
		Status = gBS->HandleProtocol (
                   FvHandles[Index],
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                   &gEfiFirmwareVolumeProtocolGuid,
#else
                   &gEfiFirmwareVolume2ProtocolGuid,
#endif
                   (VOID **) &Fv
                   );
				   
		if (EFI_ERROR (Status)) {
			continue;
		}			   

		// Assuming Image and ImageSize are correct on input.
		Status = Fv->ReadSection (
		          Fv,
                  FileNameGuid,
                  EFI_SECTION_RAW,
                  0,
                  Image,
                  ImageSize,
                  &AuthenticationStatus
                  );

		if (!EFI_ERROR (Status))
		{
			MemFreePointer ((VOID **)&FvHandles);
			return EFI_SUCCESS;
		}
		else if (EFI_BUFFER_TOO_SMALL == Status) // ImageSize updated to needed size so return
		{
			MemFreePointer ((VOID **)&FvHandles);
            return EFI_BUFFER_TOO_SMALL;
		}
	}
#endif

	gBS->FreePool(FvHandles);
	return EFI_NOT_FOUND;
}
#endif
/**
    Returns the ACPI OEM ID

    @param VOID

    @retval UINT8 * => OEM ID String

**/
UINT8 *GetACPIOEMID (VOID)
{
#ifdef T_ACPI_OEM_ID
#ifdef ACPI_OEM_ID_MAK
	return (UINT8 *)&OemIdMak;
#else   
    return CONVERT_TO_STRING (T_ACPI_OEM_ID);
#endif //ACPI_OEM_ID_MAK
#else
    return 0;
#endif
}

/**
    Returns the ACPI OEM Table ID

    @param VOID

    @retval UINT8 * => OEM Table ID String

**/
UINT8 *GetACPIOEMTableID (VOID)
{
#ifdef T_ACPI_OEM_TBL_ID
#ifdef ACPI_OEM_TBL_ID_MAK
    return (UINT8 *)&OemTblIdMak;
#else   
    return CONVERT_TO_STRING (T_ACPI_OEM_TBL_ID);
#endif //ACPI_OEM_TBL_ID_MAK
#else
    return 0;
#endif
}

/**
    Returns the ACPI OEM Revision

    @param VOID

    @retval UINT32 => ACPI OEM Revision	

**/
UINT32 GetACPIOEMRevision (VOID)
{
#ifdef ACPI_OEM_REV
    return ACPI_OEM_REV;
#else
    return 0;
#endif
}

/**
    Wrapper function to call ConvertBmpandAddBGRT

    @param BOOLEAN

    @retval VOID

**/
VOID ContribBGRTTableToAcpi (
	BOOLEAN	GifImagePresence
    )
{
#if CONTRIB_BGRT_TABLE_TO_ACPI
    ConvertBmpandAddBGRT (GifImagePresence);
#endif
}


/**
    Wrapper function to call InvalidateStatusInBgrt

    @param VOID

    @retval VOID

**/
VOID InvalidateStatusInBgrtWrapper (
    )
{
#if CONTRIB_BGRT_TABLE_TO_ACPI
    InvalidateStatusInBgrt ();
#endif
}

/**
    Wrapper function to provide TSE_LOAD_DEFAULTS_FROM_DEFAULTS_BUFFER

    @param BOOLEAN

    @retval VOID

**/
BOOLEAN LoadDefaultsFromDefaultBuffer( VOID )
{

#ifdef TSE_LOAD_DEFAULTS_FROM_DEFAULTS_BUFFER
	return TSE_LOAD_DEFAULTS_FROM_DEFAULTS_BUFFER;
#else 
	return FALSE;
#endif

}


/**
    Wrapper function to call UninstallBgrtWrapper

    @param BOOLEAN

    @retval VOID

**/
VOID UninstallBgrtWrapper (
    )
{
#if CONTRIB_BGRT_TABLE_TO_ACPI
    UninstallBGRT ();
#endif
}

/**
    Function to fill the TSE build information

    @param TseMajor UINTN *TseMinor, UINTN *TseBuild

    @retval VOID

**/
VOID GetTseBuildVersion(UINTN *TseMajor, UINTN *TseMinor, UINTN *TseBuild)
{
	*TseMajor = TSE_MAJOR;
	*TseMinor = TSE_MINOR;
	*TseBuild = TSE_BUILD;
}
/**
    Function to fill the TSE build Year

    @param TseBuildYear 


    @retval VOID

**/
VOID GetTseBuildYear(UINT16 *TseBuildYear)
{
	CHAR8 BuildYear[5]=FOUR_DIGIT_YEAR;
	*TseBuildYear=(Str2No(BuildYear[0])*0x1000+Str2No(BuildYear[1])*0x100+Str2No(BuildYear[2])*0x10+Str2No(BuildYear[3]));
}
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
/**
        This is override function. OEM can override default behaviour of this function

    Checks the input shift state with the condition shift state

    @param UINT32 = Key shift state obtained from console input devices
        UINT32 = Shift state condition to match with input key shift state

    @retval BOOLEAN = TRUE => When input and condition shift states matches
        FALSE => When input and condition shift states not matches  

**/
#if !OVERRIDE_CheckAdvShiftState
BOOLEAN CheckAdvShiftState (UINT32 AmiKeyShiftState, UINT32 CondShiftState)
{
	BOOLEAN 	Result = TRUE;

	AmiKeyShiftState =  AmiKeyShiftState & (~SHIFT_STATE_VALID);
	CondShiftState = CondShiftState & (~SHIFT_STATE_VALID);

	if (AmiKeyShiftState == CondShiftState)		
	{
		goto DONE;
	}
	else
	{
		Result = FALSE;
		//If condition shift state has both the shift key set then checking for any one pressed
		if ((CondShiftState & RIGHT_LEFT_SHIFT_PRESSED) == RIGHT_LEFT_SHIFT_PRESSED)
		{	
			if (!(AmiKeyShiftState & RIGHT_LEFT_SHIFT_PRESSED))
			{
				goto DONE;
			}
			AmiKeyShiftState &= ~RIGHT_LEFT_SHIFT_PRESSED;			//Clearing the left right bits
			CondShiftState &= ~RIGHT_LEFT_SHIFT_PRESSED;
		}
		//If condition shift state has both the Ctrl key set then checking for any one pressed
		if ((CondShiftState & RIGHT_LEFT_CONTROL_PRESSED) == RIGHT_LEFT_CONTROL_PRESSED)
		{
			if (!(AmiKeyShiftState & RIGHT_LEFT_CONTROL_PRESSED))
			{
				goto DONE;
			}
			AmiKeyShiftState &= ~RIGHT_LEFT_CONTROL_PRESSED;
			CondShiftState &= ~RIGHT_LEFT_CONTROL_PRESSED;
		}
		//If condition shift state has both the Alt key set then checking for any one pressed
		if ((CondShiftState & RIGHT_LEFT_ALT_PRESSED) == RIGHT_LEFT_ALT_PRESSED)
		{
			if (!(AmiKeyShiftState & RIGHT_LEFT_ALT_PRESSED))
			{
				goto DONE;
			}
			AmiKeyShiftState &= ~RIGHT_LEFT_ALT_PRESSED;
			CondShiftState &= ~RIGHT_LEFT_ALT_PRESSED;
		}
		//If condition shift state has both the logo key set then checking for any one pressed
		if ((CondShiftState & RIGHT_LEFT_LOGO_PRESSED) == RIGHT_LEFT_LOGO_PRESSED)
		{
			if (!(AmiKeyShiftState & RIGHT_LEFT_LOGO_PRESSED))
			{
				goto DONE;
			}
			AmiKeyShiftState &= ~RIGHT_LEFT_LOGO_PRESSED;
			CondShiftState &= ~RIGHT_LEFT_LOGO_PRESSED;
		}
		if (CondShiftState == AmiKeyShiftState)
		{
			Result = TRUE;
		}					
	}
DONE:
	return Result;
}
#endif
#endif

/**
    Retrurns DRIVER_HEALTH_SUPPORT token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN  IsDriverHealthSupported (VOID)
{
#ifdef DRIVER_HEALTH_SUPPORT
    return DRIVER_HEALTH_SUPPORT;
#else
    return 0;					//Defaultly disabled.
#endif
}

/**
    Returns the SHOW_DRV_HEALTH_BOOT_TIME_SENDFORM token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN AdvancedRepairSupported (VOID)
{
#ifdef HONOR_DRVHLTH_CONFIGREQD_ON_BOOTFAIL
	return HONOR_DRVHLTH_CONFIGREQD_ON_BOOTFAIL;
#else
	return 0;
#endif
}

/**
    Returns the EDK_1_05_RETRIEVE_DATA token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN EDKVersion_1_05_RetrieveData (VOID)
{
#ifdef EDK_1_05_RETRIEVE_DATA
	return EDK_1_05_RETRIEVE_DATA;
#else
	return 1;
#endif
}

/**
    Returns the SINGLE_CLICK_ACTIVATION token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN SingleClickActivation (VOID)
{
	return SINGLE_CLICK_ACTIVATION;
}
/**
    Returns value as enable or disable STYLE_HELP_AREA_SCROLLBAR support

    @param void

    @retval UINT8 Value for grouping boot options

**/
BOOLEAN   IsHelpAreaScrollBarSupport (void)
{
#if STYLE_HELP_AREA_SCROLLBAR
    return STYLE_HELP_AREA_SCROLLBAR;
#else
    return 0;
#endif
}
/**
    Function to Get PasswordPopupTextBox Color

    @param VOID

    @retval 

**/
UINT8 GetPasswordPopupTextBoxColor(VOID)
{
#if TSE_STYLE_GTSE_BIN_SUPPORT
	return (EFI_BACKGROUND_BLACK | EFI_WHITE);//For GTSE
#else
	return (EFI_BACKGROUND_LIGHTGRAY | EFI_BLUE);//For TSE
#endif
}

/**
    Function to Get PasswordPopupWindow Color

    @param VOID

    @retval 

**/
UINT8 GetPasswordPopupWindowColor(VOID)
{
#if TSE_STYLE_GTSE_BIN_SUPPORT
	return (EFI_BACKGROUND_LIGHTGRAY | EFI_WHITE);//For GTSE
#else
	return (EFI_BACKGROUND_BLUE | EFI_WHITE);//For TSE
#endif
}

/**
    Function to Get PasswordReportInBox Color

    @param VOID

    @retval 

**/
UINT8 GetPasswordReportInboxcolor(VOID)
{
#if TSE_STYLE_GTSE_BIN_SUPPORT 
	return (EFI_BACKGROUND_LIGHTGRAY | EFI_WHITE);
#else
	return (EFI_BACKGROUND_BLUE | EFI_WHITE);//For TSE
#endif	
}

/**
    Retrurns TSE_MULTILINE_CONTROLS token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN  IsTSEMultilineControlSupported (VOID)
{
#ifdef TSE_MULTILINE_CONTROLS
    return TSE_MULTILINE_CONTROLS;
#endif
    return FALSE;					//Defaultly disabled.
}
/**
    Returns the message box width 

    @param VOID

    @retval UINT32 

**/
UINT32 GetMsgboxWidth(VOID)
{
	return MAX_MSGBOX_WIDTH;
}

/**
    GetFormSetTitleAndHelp

    @param Handle EFI_STRING_ID *Title, EFI_STRING_ID *Help

    @retval status
**/
EFI_STATUS GetFormSetTitleAndHelp (EFI_HII_HANDLE Handle, UINT16 *Title, UINT16 *Help);
EFI_STATUS GetFormSetTitleAndHelpWrapper (VOID *Handle, UINT16 *Title, UINT16 *Help)
{
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
	return GetFormSetTitleAndHelp (Handle, Title, Help);
#else
	return EFI_UNSUPPORTED;
#endif	
}
/**
    Retrurns CONTRIB_BGRT_TABLE_TO_ACPI token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN  IsBGRTSupported (VOID)
{
#ifdef CONTRIB_BGRT_TABLE_TO_ACPI
    return CONTRIB_BGRT_TABLE_TO_ACPI;	//Default is ENABLE
#endif
    return FALSE;					
}

/**

    @param VOID*

    @retval 

**/
VOID ClearNumericShiftState (AMI_EFI_KEY_DATA *Data)
{
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	 Data->KeyState.KeyShiftState = 0;	 
#endif 
}

/**
    Find the length of the complete device path	

    @param EFI_DEVICE_PATH_PROTOCOL *

    @retval UINTN	

**/
UINTN FindDevPathTotLength (EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
	EFI_DEVICE_PATH_PROTOCOL *TempDevPath = DevPath;
	UINTN TotalLength = sizeof (EFI_DEVICE_PATH_PROTOCOL);

	while (!isEndNode (TempDevPath))
	{
		TotalLength += NODE_LENGTH (TempDevPath);
		TempDevPath = NEXT_NODE (TempDevPath);
	}
	return TotalLength;
}

/**
    Checks whether the input device path matches exactly with any of the boot options
    device path. This will be useful for USB, Floppy....

    @param EFI_DEVICE_PATH_PROTOCOL *

    @retval BOOLEAN TRUE - Device path matched and the corresponding boot option is enabled
    @retval FALSE Device path matched and the corresponding boot option is disbled

**/
BOOLEAN BootDevPathMatchEnabled (EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	UINTN iIndex = 0;
//	UINTN CmpLength = 0; Not used
	EFI_DEVICE_PATH_PROTOCOL *TempDevPath = DevicePath;
	
	while (!isEndNode (TempDevPath))
	{
		if ((MEDIA_DEVICE_PATH == TempDevPath->Type) && (MEDIA_FILEPATH_DP == TempDevPath->SubType))
		{
			for (iIndex = 0; iIndex < gBootOptionCount; iIndex ++)
			{
				if (((UINTN)TempDevPath - (UINTN)DevicePath) <= FindDevPathTotLength (gBootData [iIndex].DevicePath))
				{
					if (!MemCmp (DevicePath, gBootData [iIndex].DevicePath, (UINTN)TempDevPath - (UINTN)DevicePath))
					{
						if (!gBootData [iIndex].Active)
						{
							return FALSE;
						}
					}
				}
			}
			break;
		}
		TempDevPath = NEXT_NODE (TempDevPath);
	}
	return TRUE;					//If nothing matched then boot from that device path
}

/**
    Checks whether the input device path matches with any of the boot options
    media device path. This will be useful for drives with partition.

    @param EFI_DEVICE_PATH_PROTOCOL *

    @retval BOOLEAN TRUE - Device path matched and the corresponding boot option is enabled
    @retval FALSE Device path matched and the corresponding boot option is disbled

**/
BOOLEAN MediaDevPathMatchEnabled (EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	UINTN 	iIndex = 0;
	UINTN 	CmpLength = 0;
	EFI_DEVICE_PATH_PROTOCOL *TempDevPath = DevicePath;

	while (!isEndNode (TempDevPath))
	{
		if ((MEDIA_DEVICE_PATH == TempDevPath->Type) && (MEDIA_HARDDRIVE_DP == TempDevPath->SubType))
		{
			CmpLength = NODE_LENGTH (TempDevPath);
			break;
		}
		TempDevPath = NEXT_NODE (TempDevPath);
	}
	if (CmpLength)
	{	
		for (iIndex = 0; iIndex < gBootOptionCount; iIndex ++)
		{
			if (CmpLength <= FindDevPathTotLength (gBootData [iIndex].DevicePath))
			{
				if (!MemCmp (TempDevPath, gBootData [iIndex].DevicePath, CmpLength))
				{
					if (!gBootData [iIndex].Active)
					{
						return FALSE;
					}
				}
			}
		}		
	}	
	return TRUE;			//If nothing matched then boot from that device path
}

/**
        This is override function. OEM can override default behaviour of this function

    Launches default image from the path \EFI\BOOT\BOOT{machine type short-name}.EFI
    from the enabled devices in setup.

    @param CHAR16 *

    @retval EFI_STATUS EFI_SUCCESS -> If default image loaded successfully
        !EFI_SUCCESS -> If default image not loaded successfully

**/
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFileDevicePath( UINT32 *, CHAR16 *);
EFI_STATUS _BootLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *, VOID *, UINTN, BOOLEAN);
#if !OVERRIDE_LaunchSecBootPathFromEnabledDevs
EFI_STATUS LaunchSecBootPathFromEnabledDevs (CHAR16 *FileName)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT32     index = 0;
	
	EFI_DEVICE_PATH_PROTOCOL *devicePath;
	do
	{
		devicePath = _BootBuildFileDevicePath (&index, FileName);
		if (index != (UINT32)-1)
		{
			if ((BootDevPathMatchEnabled (devicePath)) && (MediaDevPathMatchEnabled (devicePath)))
			{
				Status = _BootLaunchDevicePath (devicePath, NULL, 0,FALSE);
			}
		}
		MemFreePointer ((VOID **)&devicePath);
	} while ((EFI_ERROR (Status)) && (index != (UINT32)-1));
	return Status;
}
#endif
/**
    Launches default image from the path \EFI\BOOT\BOOT{machine type short-name}.EFI

    @param CHAR16 *

    @retval EFI_STATUS EFI_SUCCESS -> If default image loaded successfully
        !EFI_SUCCESS -> If default image not loaded successfully

**/
#if !OVERRIDE_LaunchSecondaryBootPath
EFI_STATUS LaunchSecondaryBootPath (CHAR16 *FileName)
{
#if SECONDARY_BOOT_FROM_ENABLED_DEVICES
	return LaunchSecBootPathFromEnabledDevs (FileName);		//If user need to boot the default image from only enabled devices
#else																			//Default case
	return BootLaunchFilename (FileName);	            		//loads the default image from any of the file system
#endif
}
#endif

/**
    Retrurns SETUP_DELAY_LOGO_TILL_INPUT token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN  IsDelayLogoTillInputSupported (VOID)
{
#ifdef SETUP_DELAY_LOGO_TILL_INPUT
	return SETUP_DELAY_LOGO_TILL_INPUT;
#else
	return 0;
#endif
}

/**
    Retrurns notify mask vaule

    @param VOID

    @retval BOOLEAN

**/
UINT32  GetNotifyMaskValue (VOID)
{
//#if AMITSE_SOFTKBD_SUPPORT
	return SOMETHING;
/*#else
	return (SOMETHING & (~NOTIFY_MASK_MOUSE_DRIVER));
#endif*/
}

/**
    Function to move scrollbar in Frame using mouse

    @param VOID *, BOOLEAN, UINT32

    @retval status
**/
EFI_STATUS MouseScrollBarMove(VOID *frame, BOOLEAN bScrollUp, UINT32 Size)
{
#if MINISETUP_MOUSE_SUPPORT
	 return TSEMouseScrollBarMove( frame, bScrollUp, Size );	
#else
    return EFI_UNSUPPORTED;
#endif
}

/**
    Function to move scrollbar in ListBox using mouse

    @param VOID *, BOOLEAN, UINT32

    @retval status
**/
EFI_STATUS MouseListBoxScrollBarMove(VOID *listbox, BOOLEAN bScrollUp, UINT32 Size)
{
#if MINISETUP_MOUSE_SUPPORT
	 return TSEMouseListBoxScrollBarMove( listbox, bScrollUp, Size );	
#else
    return EFI_UNSUPPORTED;
#endif	
}

/**
    Function to return FastBoot is ENABLED or DISABLED

    @param VOID

    @retval BOOLEAN
**/

BOOLEAN TseIgnoreKeyForFastBoot()
{
#if TSE_IGNORE_KEY_FOR_FASTBOOT
	UINT32 *currentBootFlow;
	UINTN size = 0;

	currentBootFlow = VarGetNvramName( L"BootFlow", &_gBootFlowGuid, NULL, &size );	
	if (currentBootFlow != NULL  && *currentBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT)
	{
		MemFreePointer( (VOID **)&currentBootFlow );
		return TRUE;
	}	
	else
	{
		if (currentBootFlow)
		{
			MemFreePointer ((VOID **)&currentBootFlow );
		}
		return FALSE;
	}
#else
 	return FALSE;
#endif	
}

/**
    Function to return TSE_CURSOR_SUPPORT token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN  IsTSECursorSupport (VOID)
{
#ifdef TSE_CURSOR_SUPPORT
	return TSE_CURSOR_SUPPORT;
#else
	return 0;
#endif
}


/**
    Dispalys the image obtained form OS Update and calls the capsule to update

    @param BOOT_FLOW *

    @retval EFI_STATUS EFI_SUCCESS -> Capsule update is success
        !EFI_SUCCESS -> Fault happenend in capsule update

**/
#if !OVERRIDE_OsUpdateCapsuleWrap
TSE_POST_STATUS	PostManagerGetPostStatus(VOID);
LOGO_TYPE GetLogoType(UINT8 *ImageData);
EFI_STATUS OsUpdateCapsuleWrap (BOOT_FLOW *bootFlowPtr)
{
	EFI_STATUS  Status = EFI_SUCCESS;
#if SUPPORT_ESRT
	EFI_GUID    ReflashGuid = EFI_REFLASH_PROTOCOL_GUID;
	UINT8 		*ImageData = NULL;
	UINTN 		ImageSize       = 0;
	UINTN 		CoordinateX     = 0;
	UINTN 		CoordinateY     = 0;
	UINTN 		Width           = 0;
	UINTN       Height          = 0;
	UINTN			LogoType        = 0;
	UINTN			UgaBltSize      = 0;
	UINTN			unBufferWidth   = 0;
	EFI_BADGING_DISPLAY_ATTRIBUTE Attribute;
	EFI_UGA_PIXEL	            	*UgaBlt = NULL;
	EFI_REFLASH_PROTOCOL        	*ReFlash = NULL;

   SETUP_DEBUG_TSE ("\nEntering OsUpdateCapsuleWrap");
	Status = gBS->LocateProtocol (&ReflashGuid, NULL, &ReFlash);
	if (!EFI_ERROR (Status))
	{
		Status = ReFlash->GetDisplayImage (ReFlash, &CoordinateX,  &CoordinateY, &ImageData);		//Recovery module have to allocate and send the image data
		SETUP_DEBUG_TSE ("\nGetDisplayImage status is %x", Status);
		if (!EFI_ERROR (Status))																						//TSE wont free the ImageData
		{																														//Recovery module have to provide the X,Y coordinates too
      	Attribute = EfiBadgingDisplayAttributeCustomized;	//and wont free the memory
      	Height = 0;
      	Width = 0;
      	LogoType = GetLogoType (ImageData);
        
      	if (BMP_Logo ==  LogoType)			//Draw only for BMP image
			{
				Status = ConvertBmpToUgaBltWrapper(
					ImageData,
					ImageSize,
					&UgaBlt,
					&UgaBltSize,
					&Height,
					&Width );
				SETUP_DEBUG_TSE ("\nStatus for blt wrap is %x", Status);
				unBufferWidth = Width;
				SETUP_DEBUG_TSE ("\nCoordinateX  is %x CoordinateY is %x", CoordinateX, CoordinateY);
				SETUP_DEBUG_TSE ("\nDrawing the string");
				DrawBltBuffer (UgaBlt, Attribute, Width, Height, CoordinateX, CoordinateY, unBufferWidth);
			}
			SETUP_DEBUG_TSE ("\nCalling CapUpdProgress");
			Status = ReFlash->CapUpdProgress (ReFlash);
      }
	}
#endif
   SETUP_DEBUG_TSE ("\nExiting OsUpdateCapsuleWrap");
	return Status;
}
#endif

/**
    Function to return BOOT_OVERRIDE_BOOTNEXT_VARIABLE_FEATURE value

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN  IsBootOverrideBootNextSupport (VOID)
{
#if BOOT_OVERRIDE_BOOTNEXT_VARIABLE_FEATURE
	return BOOT_OVERRIDE_BOOTNEXT_VARIABLE_FEATURE;
#else
	return 0;
#endif
}

/**
    Function to return TSE_SUPPRESS_MULTILINE_FOR_ONEOFLABEL value

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN TseSuppressOneofMultilineSupport(VOID)
{

#ifdef TSE_SUPPRESS_MULTILINE_FOR_ONEOFLABEL
	return TSE_SUPPRESS_MULTILINE_FOR_ONEOFLABEL;
#else
	return FALSE;
#endif

}

/**
    Wrapper function for RegFormNotification

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS RegFormNotification (VOID);
EFI_STATUS RTIfrRegFormNotificationWrapper (VOID)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	return RegFormNotification ();
#else
	return EFI_SUCCESS;	
#endif
}

/**
    Wrapper function for UnRegFormNotification

    @param VOID

    @retval VOID

**/
VOID UnRegFormNotification (VOID);
VOID RTIfrUnRegFormNotificationWrapper (VOID)
{
#if SETUP_RUNTIME_IFR_PROCESSING
	UnRegFormNotification ();
#endif
}

/**		
    Retrurns DRAW_POST_LOGO_AT_0POS token value

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN DrawPostLogoAt0Pos (VOID)
{
#ifdef DRAW_POST_LOGO_AT_0POS
	 return DRAW_POST_LOGO_AT_0POS;	
#else
    return FALSE;
#endif	
}

//Save pwd to nvram and not loading empty pwd on loading defaults.
/**
    Returns the token value for SETUP_SAVE_PSWD_TO_NVRAM

    @param VOID

    @retval UINT32 

**/
BOOLEAN TSEPwdSavetoNvram(VOID)
{

#ifdef SETUP_SAVE_PSWD_TO_NVRAM
    return SETUP_SAVE_PSWD_TO_NVRAM;
#endif
    return FALSE;					//Defaultly disabled.

}

/**
    Returns the token value for TSE_DONOT_LOAD_PASSWORD_ON_DEFAULTS

    @param VOID

    @retval UINT32 

**/
BOOLEAN TSEDonotLoadPasswordOnDefaults(VOID)
{

#ifdef TSE_DONOT_LOAD_PASSWORD_ON_DEFAULTS
    return TSE_DONOT_LOAD_PASSWORD_ON_DEFAULTS;
#endif
    return FALSE;					//Defaultly disabled.

}
/**
    Returns type of shell text mode

    @param VOID

    @retval UINT32

**/
UINT32  ShellTextMode (VOID)
{
#ifdef SHELL_TEXT_MODE
	return SHELL_TEXT_MODE;
#else
	return 0xFF;
#endif
}

/**
    Returns value of disable FLUSH_KEYS_AFTER_READ

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   FlushKeysAfterRead(VOID)
{
#ifdef FLUSH_KEYS_AFTER_READ
	return FLUSH_KEYS_AFTER_READ;
#else
	return 0;
#endif
}

/**
    Returns token value TSE_SETUP_GRAYOUT_SELECTABLE

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsGrayoutSelectable()
{
#if TSE_SETUP_GRAYOUT_SELECTABLE
    return 1;
#else
    return 0;
#endif
}

/**
    Returns value of disable SETUP_GO_TO_EXIT_PAGE_ON_EXIT_KEY

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   gotoExitOnEscKey(EFI_GUID *exitPageGuid, UINT16 *pageClass, UINT16 *pageSubclass, UINT16 *pageFormID)
{
#if SETUP_GO_TO_EXIT_PAGE_ON_EXIT_KEY

	EFI_GUID ExitPageGuid = EXIT_PAGE_GUID;
	MemCopy(exitPageGuid, &ExitPageGuid, sizeof(EFI_GUID));
	*pageClass 		= (UINT16)EXIT_PAGE_CLASS;
	*pageSubclass	= (UINT16)EXIT_PAGE_SUB_CLASS;
	*pageFormID		= (UINT16)EXIT_PAGE_FORM_ID;

	return TRUE;
#else
	return FALSE;
#endif
}

/**
    Returns PI_SPECIFICATION_VERSION value

    @param VOID

    @retval UINTN

**/
UINTN   PISpecVersion(VOID)
{
#ifdef PI_SPECIFICATION_VERSION
	return PI_SPECIFICATION_VERSION;
#else
	return 0;
#endif
}


/**
    Returns token value TSE_SHOW_PROMPT_STRING_AS_TITLE

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsShowPromptStringAsTitle(VOID)
{
#if TSE_SHOW_PROMPT_STRING_AS_TITLE
	return TSE_SHOW_PROMPT_STRING_AS_TITLE;
#else
	return 0;
#endif
}

/**
    Wrapper function to call PrntScrnKeyHandleaction

    @param VOID

    @retval BOOLEAN

**/
EFI_STATUS WriteDataToFile(CHAR16 *filename, VOID *Data, UINTN length, UINT32 index);
EFI_STATUS WriteDataToFileWrapper (CHAR16 *filename, VOID *Data, UINTN length, UINT32 index)
{
   EFI_STATUS Status = EFI_SUCCESS;
#if EFI_SPECIFICATION_VERSION>=0x2000A   
   Status = WriteDataToFile (filename, Data, length, index);
#endif
   return Status;
}

/**
    Set OVERRIDE_UIUpdateCallbackHook to 1, if OEM wants to override the UIUpdateCallbackHook function from Board module.

    @param VOID

    @retval EFI_STATUS

**/
#if !OVERRIDE_UIUpdateCallbackHook
VOID UIUpdateCallback( VOID * Handle, UINT32 OldVariableCount );
VOID UIUpdateCallbackHook( VOID * Handle, UINT32 OldVariableCount )
{
    UIUpdateCallback((VOID*)Handle, OldVariableCount);
}
#endif


/**
    Set OVERRIDE_FixMergePagesExtraHook to 1, if OEM wants to override the UIUpdateCallbackHook function from Board module.

    @param VOID

    @retval EFI_STATUS

**/
#if !OVERRIDE_FixMergePagesExtraHook
VOID FixMergePagesExtraHook (VOID *gPages, VOID *tempPages)
{
}
#endif

/**
    Set OVERRIDE_PageRemoveHook to 1, if OEM wants to override the PageRemoveHook function from Board module.

    @param VOID

    @retval EFI_STATUS

**/
#if !OVERRIDE_PageRemoveHook
void PageRemoveHook (UINTN PageIndex, void *pageInfo, void *PageHandle, UINT16 PageFormID)
{
	
}
#endif

/**
    Set OVERRIDE_ProcessPackNotificationHook to 1, if OEM wants to override the ProcessPackNotificationHook function from Board module.

    @param VOID

    @retval EFI_STATUS

**/
#if !OVERRIDE_ProcessPackNotificationHook
EFI_STATUS ProcessPackNotification (void);
EFI_STATUS ProcessPackNotificationHook (void)
{
	return ProcessPackNotification ();
}
#endif

/**
    Set OVERRIDE_MainSetupLoopInitHook to 1, if OEM wants to override the MainSetupLoopInitHook function from Board module.

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_MainSetupLoopInitHook
VOID MainSetupLoopInit(VOID);
VOID MainSetupLoopInitHook ( VOID )
{
	MainSetupLoopInit();
}
#endif

/**
    Set OVERRIDE_ESAInitFvHook to 1, if OEM wants to override the ESAInitFvHook function from Board module.

    @param VOID

    @retval VOID

**/
#if !OVERRIDE_ESAInitFvHook
EFI_STATUS InstallEsaInterfacesForTse (void);
EFI_STATUS ESAInitFvHook (void)
{
//	InstallEsaInterfacesForTse ();
	return EFI_SUCCESS;
}
#endif
 
/**
    Returns token value TSE_DONOT_LOAD_SYSTEM_ACCESS_FOR_USER_DEFAULT

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsLoadSysteAccessOnUserDefault(VOID)
{
#if TSE_DONOT_LOAD_SYSTEM_ACCESS_FOR_USER_DEFAULT
	return TSE_DONOT_LOAD_SYSTEM_ACCESS_FOR_USER_DEFAULT;
#else
	return 0;
#endif
}

/**
    Returns token value TSE_ANSI_ESC_CODE_SUPPORT

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsANSIEscapeCodeSupported(VOID)
{
#if TSE_ANSI_ESC_CODE_SUPPORT
	return TSE_ANSI_ESC_CODE_SUPPORT;
#else
	return 0;
#endif
}

/**
    Returns token value TSE_DBG_DISABLE_APPDESTRUCTOR

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN TseDebugDisableApplicationDestructor (VOID)
{
#ifdef TSE_DBG_DISABLE_APPDESTRUCTOR
	return TSE_DBG_DISABLE_APPDESTRUCTOR;
#else
	return FALSE;
#endif
}

/**
    Returns token value TSE_DEFAULT_SETUP_PASSWORD_SUPPORT

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN TseDefaultSetupPasswordSupported (void)
{
#ifdef TSE_DEFAULT_SETUP_PASSWORD_SUPPORT
	return TSE_DEFAULT_SETUP_PASSWORD_SUPPORT;
#else
	return FALSE;
#endif
}

/**
    Returns token value DEFAULT_SETUP_PWD_AT_FIRSTBOOT_ONLY

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN DefaultSetupPwdAtFirstBootOnly (void)
{
#ifdef DEFAULT_SETUP_PWD_AT_FIRSTBOOT_ONLY
	return DEFAULT_SETUP_PWD_AT_FIRSTBOOT_ONLY;
#else
	return FALSE;
#endif
}

/**
    Returns token DISABLE_ESC_IN_BBS value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsDisableESCinBBSMenu (VOID)
{
#if DISABLE_ESC_IN_BBS
	return DISABLE_ESC_IN_BBS;
#else
	return 0;
#endif
}

/**
    Set OVERRIDE_SetMessageBoxProgressType to 1, if OEM wants to override the GetMessageBoxProgressType function from Board module.
    This function get control while setting type to message box progress bar
    @param VOID

    @retval VOID

**/
#if !OVERRIDE_SetMessageBoxProgressType
MSGBOX_EX_CATAGORY SetMessageBoxProgressType (void)
{
	return MSGBOX_EX_CATAGORY_NORMAL;
}
#endif


/**
	Description:
		Function to set Desired TEXTMODE before entering setup to initialize
		TSE buffer based on TEXTMODE.
	
	Usage:
    Set OVERRIDE_SetResAndScreenBufferHook to 1, if OEM wants to override the SetDesiredTextModeHook function.
    OEM can provide custom TEXT mode or use set desired mode using SetMode()
    
    @param 
		- UINTN *Rows 
		- UINTN *cols 
    @retval
		- EFI_SUCCESS : To use the text mode based on updated Rows and Cols.
		- EFI_UNSUPPORTED : To use default supported TextMode.

**/
#if !OVERRIDE_SetResAndScreenBufferHook
EFI_STATUS SetResAndScreenBufferHook (UINTN *Rows, UINTN *Cols )
{

#if TSE_RECALCULATE_LAYOUT_TO_MAX_RES
//Set the maximum resolution from supported modes
    
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINTN ModeRows = 0, ModeCols = 0;
        Status = gST->ConOut->QueryMode( gST->ConOut, (gST->ConOut->Mode->MaxMode)-1, &ModeCols, &ModeRows );
        if(!EFI_ERROR(Status))
        {
            *Rows = ModeRows;
            *Cols = ModeCols;
        }
        
        return Status;
#endif   
    
   
/*
	//(e.g) To Set desired TEXT Mode
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINTN i,ModeCols = 0,ModeRows = 0, ReqCols = 160, ReqRows = 53; //where ReqCols and ReqRows are Row and Column value of required text mode.

	for ( i = 0; i < (UINTN)gST->ConOut->Mode->MaxMode; i++ )
	{
		Status = gST->ConOut->QueryMode( gST->ConOut, i, &ModeCols, &ModeRows );
		if ( EFI_ERROR( Status ) )
			continue;
		if ( ( ReqCols == ModeCols ) && ( ReqRows == ModeRows ) )
		{
			*Rows = ModeRows;  // Update the details only if Supported the required text mode
			*Cols = ModeCols;
			return EFI_SUCCESS;
		}
	}
*/
return EFI_UNSUPPORTED;
}
#endif

#if ((ESA_BINARY_SUPPORT && defined(ESA_TSE_FULL_SUPPORT )&& (ESA_TSE_FULL_SUPPORT ==0) && (ESA_GUI_DRIVER_SUPPORT == 0)) || (defined(BUILD_FOR_ESA) && (ESA_TSE_FULL_SUPPORT == 1)))
EFI_STATUS SaveTseLiteGlobalData(VOID *temp)
{
#ifndef ESA_SETUP_FORM_BROWSER_SUPPORT 
	return EFI_UNSUPPORTED;
#else
	return EFI_SUCCESS;
#endif
	//Dummy function 
}

VOID RestoreTseLiteGlobalData(VOID *temp)
{
	//Dummy function
}
EFI_STATUS SaveTseAdvancedGlobalData(VOID *temp)
{
#ifndef ESA_SETUP_FORM_BROWSER_SUPPORT 
	return EFI_UNSUPPORTED;
#else
	return EFI_SUCCESS;
#endif
	//Dummy function
}
VOID RestoreTseAdvancedGlobalData(VOID *temp)
{
	//Dummy function
}

BOOLEAN IsTSEBuild()
{
	return FALSE; // For ESA build
}
#else
BOOLEAN IsTSEBuild()
{
	return TRUE;// For TSE Build
}
#endif

/**
    Description:
        Save the Global Variable
   
    @param 
        VOID
    @retval
        VOID
**/
#if !OVERRIDE_SaveSetupGobalDataHook
VOID SaveSetupGobalDataHook(VOID)
{
    //Save The Setup Global Data    
}
#endif

/**
    Description:
        Restore the Global Variable
   
    @param 
        VOID
    @retval
        VOID
**/
#if !OVERRIDE_RestoreSetupGobalDataHook
VOID RestoreSetupGobalDataHook(VOID)
{
    //Restore the Setup Global Data
}
#endif
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
