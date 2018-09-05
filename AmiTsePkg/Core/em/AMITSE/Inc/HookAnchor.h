//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/HookAnchor.h $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 11/20/11 6:33a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file HookAnchor.h
    TSE hook functions

**/

#ifndef _HOOKANCHOR_H_
#define	_HOOKANCHOR_H_

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#include <Protocol/AmiKeycode.h>  
#else
#ifndef _AMI_EFI_KEY_DATA_
#define _AMI_EFI_KEY_DATA_
typedef struct {
  EFI_INPUT_KEY  Key;
} AMI_EFI_KEY_DATA;
#endif
#endif

VOID DrawQuietBootLogoHook(VOID);
BOOLEAN ProcessConInAvailabilityHook(VOID);
VOID ProcessEnterSetupHook(VOID);
VOID ProcessProceedToBootHook(VOID);
VOID MinisetupDriverEntryHookHook(VOID);
VOID PostManagerHandShakeHookHook(VOID);
CONTROL_ACTION MapControlKeysHook(AMI_EFI_KEY_DATA key);
VOID CheckForKeyHook( EFI_EVENT Event, VOID *Context );
VOID CheckForClickHook( EFI_EVENT Event, VOID *Context );//EIP 62763 : Utilize the Improvements done from mouse driver in AMITSE 
VOID BeforeLegacyBootLaunchHook(VOID);
VOID AfterLegacyBootLaunchHook(VOID);
VOID BeforeEfiBootLaunchHook(VOID);
VOID AfterEfiBootLaunchHook(VOID);
VOID SavedConfigChangesHook(VOID);
VOID LoadedConfigDefaultsHook(VOID);
VOID TimeOutLoopHookHook(VOID);
EFI_STATUS	OemKey1HookHook ( BOOT_FLOW *bootFlowPtr );
EFI_STATUS	OemKey2HookHook ( BOOT_FLOW *bootFlowPtr );
EFI_STATUS	OemKey3HookHook ( BOOT_FLOW *bootFlowPtr );
EFI_STATUS	OemKey4HookHook ( BOOT_FLOW *bootFlowPtr );
VOID PasswordEncodeHook( CHAR16 *Password, UINTN MaxSize);
EFI_STATUS SetControlColorsHook(UINT8 *BGColor, UINT8 *FGColor, UINT8 *SecBGColor, UINT8 *SecFGColor, 
								 UINT8 *SelBGColor, UINT8 *SelFGColor, UINT8 *NSelBGColor, UINT8 *NSelFGColor,
								 UINT8 *LabelBGColor, UINT8 *LabelFGColor,UINT8 *NSelLabelFGColor, UINT8 *EditBGColor, UINT8 *EditFGColor,
								 UINT8 *PopupFGColor, UINT8 *PopupBGColor);
VOID InvalidActionHookHook(VOID);
VOID LoadedUserDefaultsHook(VOID);
VOID LoadedBuildDefaultsHook(VOID);
VOID LoadedPreviousValuesHook(VOID);
VOID SetupConfigModifiedHook(VOID);
VOID DrawPopupMenuHook();
VOID BbsBootHook();
VOID LoadSetupDefaultsHook( NVRAM_VARIABLE *defaults, UINTN data );
VOID PreSystemResetHookHook(VOID);
CONTROL_ACTION MapControlMouseActionHook(VOID *pMouseInfo);
VOID ProcessProceedToBootNowHook(VOID);
VOID ProcessConOutAvailableHook(VOID);
VOID ProcessUIInitHookHook(VOID);
VOID AfterInitPostScreenHook(VOID);
VOID SwitchToPostScreenHook(VOID);
BOOLEAN SetupEntryHook(VOID);
EFI_STATUS BootFlowExitHookHook(VOID);
VOID LastTSEHook(VOID);

typedef VOID (TSE_HOOK) (VOID);
typedef BOOLEAN (TSE_HOOK_T1) ();
typedef CONTROL_ACTION (TSE_HOOK_T2) (AMI_EFI_KEY_DATA key);
typedef VOID (TSE_HOOK_T3) (EFI_EVENT Event, VOID *Context);
typedef EFI_STATUS (TSE_HOOK_T4) (BOOT_FLOW *bootFlowPtr);
typedef VOID (TSE_HOOK_T5) (CHAR16 *Password, UINTN MaxSize);
typedef EFI_STATUS (TSE_HOOK_T6) (UINT8 *BGColor, UINT8 *FGColor, UINT8 *SecBGColor, UINT8 *SecFGColor, 
								 UINT8 *SelBGColor, UINT8 *SelFGColor, UINT8 *NSelBGColor, UINT8 *NSelFGColor,
								 UINT8 *LabelBGColor, UINT8 *LabelFGColor,UINT8 *NSelLabelFGColor, UINT8 *EditBGColor, UINT8 *EditFGColor,
								 UINT8 *PopupFGColor, UINT8 *PopupBGColor);
typedef CONTROL_ACTION (TSE_HOOK_T7) (VOID *MouseInfo);
typedef EFI_STATUS (TSE_HOOK_T9) ();

extern TSE_HOOK * AMITSE_HookList [];

extern UINTN gHookBase;

#define HOOKID0		(gHookBase)
#define HOOKID1		(gHookBase+1)
#define HOOKID2		(gHookBase+2)
#define HOOKID3		(gHookBase+3)
#define HOOKID4		(gHookBase+4)
#define HOOKID5		(gHookBase+5)
#define HOOKID6		(gHookBase+6)
#define HOOKID7		(gHookBase+7)
#define HOOKID8		(gHookBase+8)
#define HOOKID9		(gHookBase+9)
#define HOOKID10	(gHookBase+10)
#define HOOKID11	(gHookBase+11)
#define HOOKID12	(gHookBase+12)
#define HOOKID13	(gHookBase+13)
#define HOOKID14	(gHookBase+14)
#define HOOKID15	(gHookBase+15)
#define HOOKID16	(gHookBase+16)
#define HOOKID17	(gHookBase+17)
#define HOOKID18	(gHookBase+18)
#define HOOKID19	(gHookBase+19)
#define HOOKID20	(gHookBase+20)
#define HOOKID21	(gHookBase+21)
#define HOOKID22	(gHookBase+22)
#define HOOKID23	(gHookBase+23)
#define HOOKID24	(gHookBase+24)
#define HOOKID25	(gHookBase+25)
#define HOOKID26	(gHookBase+26)
#define HOOKID27	(gHookBase+27)
#define HOOKID28	(gHookBase+28)
#define HOOKID29	(gHookBase+29)
#define HOOKID30	(gHookBase+30)
#define HOOKID31	(gHookBase+31)
#define HOOKID32	(gHookBase+32)
#define HOOKID33	(gHookBase+33)
#define HOOKID34	(gHookBase+34)
#define HOOKID35	(gHookBase+35)
#define HOOKID36	(gHookBase+36)
#define HOOKID37	(gHookBase+37)//EIP-SwitchToPostScreenHook
#define HOOKID38	(gHookBase+38)
#define HOOKID39	(gHookBase+39)

#endif /* _HOOKANCHOR_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2011, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
