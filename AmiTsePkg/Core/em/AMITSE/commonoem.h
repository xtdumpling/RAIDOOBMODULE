//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/commonoem.h $
//
// $Author: Arunsb $
//
// $Revision: 15 $
//
// $Date: 2/01/12 1:53a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file commonoem.h
    header of commonoem.h

**/

#ifndef _COMMONOEM_H_
#define _COMMONOEM_H_

typedef enum
{
	ControlActionUnknown,
	ControlActionNextLeft,
	ControlActionNextRight,
	ControlActionNextUp,
	ControlActionNextDown,
	ControlActionPageUp,
	ControlActionPageDown,
	ControlActionNextSelection,
	ControlActionPrevSelection,
	ControlActionSelect,
	ControlActionIncreament,
	ControlActionDecreament,
	ControlActionBackSpace,
	ControlActionAbort,
	ControlActionAlpha,
	ControlActionNumeric,
	ControlActionHelpPageUp,
	ControlActionHelpPageDown,

	ControlActionHome,
	ControlActionEnd,
	ControlActionChoose,
	ControlMouseActionLeftDown,
	ControlMouseActionLeftUp,
	ControlActionTimer
}CONTROL_ACTION;

typedef struct _STYLECOLORS
{
	UINT8 FGColor;
	UINT8 BGColor;
	UINT8 SecFGColor;
	UINT8 SecBGColor;
	UINT8 SelFGColor;
	UINT8 SelBGColor;
	UINT8 NSelFGColor;
	UINT8 NSelBGColor;
	UINT8 LabelBGColor;
	UINT8 LabelFGColor;
	UINT8 NSelLabelFGColor;
	UINT8 EditBGColor;
	UINT8 EditFGColor;
	UINT8 PopupFGColor;
	UINT8 PopupBGColor;
}STYLECOLORS;


//For CLICK Event
#define     TSEMOUSE_NULL_CLICK         0
#define     TSEMOUSE_RIGHT_CLICK        1
#define     TSEMOUSE_LEFT_CLICK         2
//#define     TSEMOUSE_MIDDLE_CLICK       3
#define     TSEMOUSE_LEFT_DCLICK        4
#define     TSEMOUSE_RIGHT_DOWN         5
#define     TSEMOUSE_LEFT_DOWN          6
//#define     TSEMOUSE_MIDDLE_DOWN        7
#define     TSEMOUSE_RIGHT_UP           8
#define     TSEMOUSE_LEFT_UP            9
//#define     TSEMOUSE_MIDDLE_UP         10

//Data Structure for Storing Mouse action
typedef struct _MOUSE_INFO
{
    UINT32	Top;
    UINT32	Left;
    UINT32  ButtonStatus;
}MOUSE_INFO;

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#include <Protocol/AmiKeycode.h> 



BOOLEAN CheckAdvShiftState (UINT32 AmiKeyShiftState, UINT32 CondShiftState);
#define TSE_CHECK_SHIFTSTATE  CheckAdvShiftState
 
/*( (!(AmiKeyShiftSate &(~SHIFT_STATE_VALID)) && (!CondShiftState)) || \
(((AmiKeyShiftSate &(~SHIFT_STATE_VALID)) & CondShiftState) == (CondShiftState & (~SHIFT_STATE_VALID))) || \
((AmiKeyShiftSate == SHIFT_STATE_VALID) && (CondShiftState == SHIFT_STATE_VALID)) )*/

#else
#ifndef _AMI_EFI_KEY_DATA_
#define _AMI_EFI_KEY_DATA_
typedef struct {
  EFI_INPUT_KEY  Key;
} AMI_EFI_KEY_DATA;
#endif
#endif

VOID DrawQuietBootLogo(VOID);
BOOLEAN ProcessConInAvailability(VOID);
VOID ProcessEnterSetup(VOID);
VOID ProcessProceedToBoot(VOID);
VOID MinisetupDriverEntryHook(VOID);
VOID PostManagerHandShakeHook(VOID);
CONTROL_ACTION MapControlKeys(AMI_EFI_KEY_DATA key);
VOID CheckForKey( EFI_EVENT Event, VOID *Context );
VOID CheckForClick( EFI_EVENT Event, VOID *Context );
VOID BeforeLegacyBootLaunch(VOID);
VOID AfterLegacyBootLaunch(VOID);
VOID BeforeEfiBootLaunch(VOID);
VOID AfterEfiBootLaunch(VOID);
VOID SavedConfigChanges(VOID);
VOID LoadedConfigDefaults(VOID);
VOID TimeOutLoopHook(VOID);
VOID PasswordEncode( CHAR16 *Password, UINTN MaxSize);
EFI_STATUS SetControlColors(UINT8 *BGColor, UINT8 *FGColor, UINT8 *SecBGColor, UINT8 *SecFGColor, 
								 UINT8 *SelBGColor, UINT8 *SelFGColor, UINT8 *NSelBGColor, UINT8 *NSelFGColor,
								 UINT8 *LabelBGColor, UINT8 *LabelFGColor,UINT8 *NSelLabelFGColor, UINT8 *EditBGColor, UINT8 *EditFGColor,
								 UINT8 *PopupFGColor, UINT8 *PopupBGColor);

VOID InvalidActionHook(VOID);
VOID LoadedUserDefaults(VOID);
VOID LoadedBuildDefaults(VOID);
VOID LoadedPreviousValues(VOID);
VOID SetupConfigModified(VOID);
VOID LoadSetupDefaults (VOID *defaults, UINTN data );
VOID PreSystemResetHook(VOID);
CONTROL_ACTION MapControlMouseAction(VOID *pMouseInfo);
VOID ProcessProceedToBootNow(VOID);
VOID ProcessConOutAvailable(VOID);
VOID ProcessUIInitHook(VOID);
VOID AfterInitPostScreen(VOID);
VOID SwitchToPostScreen(VOID);
BOOLEAN DefaultEntryStatus();
EFI_STATUS BootFlowExitHook(VOID);

#define MODIFIER_DUAL_KEYS 			0x000000FF          //For handling any oTf the two dual(ctrl, alt, shift and logo) keys pressed
#define RIGHT_LEFT_SHIFT_PRESSED	0x00000003
#define RIGHT_LEFT_CONTROL_PRESSED	0x0000000C
#define RIGHT_LEFT_ALT_PRESSED		0x00000030
#define RIGHT_LEFT_LOGO_PRESSED		0x000000C0


#define EFI_MOUSE_DRIVER_PROTOCOL_GUID \
{0xc7a7030c, 0xc3d8, 0x45ee, 0xbe, 0xd9, 0x5d, 0x9e, 0x76, 0x76, 0x29, 0x53}

typedef	VOID	(*HOTKEY_CALLBACK)	( VOID *container, VOID *object, VOID *cookie );

typedef struct _HOTKEY_TEMPLATE
{
	EFI_INPUT_KEY	Key;
	UINT32			KeySftSte;
	HOTKEY_CALLBACK	Callback;
	UINT32			BasePage;
}
HOTKEY_TEMPLATE;

typedef	VOID	(*HOTCLICK_CALLBACK)	( VOID *container, VOID *object, VOID *cookie );

typedef struct _HOTCLICK_TEMPLATE
{
	UINT32 			Xo, Yo, Xn, Yn;
	HOTCLICK_CALLBACK	Callback;
	UINT32			BasePage;
}
HOTCLICK_TEMPLATE;

extern HOTKEY_TEMPLATE gHotKeyInfo[];
extern UINT32 gHotKeyCount;
extern UINT32 gHotClickCount;
extern HOTCLICK_TEMPLATE gHotClickInfo[];

typedef struct
{
	EFI_GUID			VariableGuid;
	CHAR8				VariableName [50];
}VAR_DYNAMICPARSING_HANDLESUPPRESS;

BOOLEAN IsTseBuild();

#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI	0x0000000000000001
#define EFI_OS_INDICATIONS_START_OS_RECOVERY 0x0000000000000020
#define EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY  0x0000000000000040
VOID SaveGraphicsScreen(VOID);
#endif

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
