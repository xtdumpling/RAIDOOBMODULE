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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/minisetupext.h $
//
// $Author: Premkumara $
//
// $Revision: 22 $
//
// $Date: 5/28/12 12:32p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file minisetupext.h
    Header file for FULL TSE specific minisetup entery and exit	funtionallies

**/

#ifndef _MINISETUPEXT_H_
#define	_MINISETUPEXT_H_


#define	MINI_SETUP_HPK_DATA_GUID	\
	{ 0xDCFBBCA4, 0x2498, 0x48DB, 0x91, 0x22, 0xFF, 0x7E, 0x96, 0x4B, 0xC5, 0x69 }

#define AMI_UEFI_LIB_POPUP_PROTOCOL_GUID \
		{ 0xf31fcbb5, 0xb53d, 0x4215, { 0x81, 0xdb, 0x61, 0x33, 0xe4, 0xd4, 0x98, 0x6a } }
#define AMI_CALLBACK_CONTROL_UPDATE 1
#define AMI_CALLBACK_RETRIEVE 2
#define AMI_CALLBACK_FORM_OPEN 3
#define AMI_CALLBACK_FORM_CLOSE 4
#define AMI_CALLBACK_SUBMITTED 5
#define AMI_CALLBACK_FORM_DEFAULT_STANDARD 		0x1000 
#define AMI_CALLBACK_FORM_DEFAULT_MANUFACTURING 0x1001

#include "setupdata.h"
#if EFI_SPECIFICATION_VERSION <= 0x20000
#include "hii.h"
#else
#include "TseUefiHii.h"
#endif /* #if EFI_SPECIFICATION_VERSION <= 0x20000 */
#include "StyleHook/StyleHook.h" 
//#if TSE_STYLE_SOURCES_SUPPORT
//#include "style.h"
//#endif
#include "application.h"
#include "action.h"
#include "MessageBox.h" 
#include "hotkey.h"
#include "Date.h" 
#include "Text.h" 
#include "edit.h"
#include "Label.h" 
#include "ListBox.h" 
#include "Memo.h" 
#include "Menu.h"  //
#include "numeric.h"
#include "popup.h"
#include "PopupEdit.h"  //
#include "PopupPassword.h"
#include "PopupSel.h"
#include "PopupString.h"
#include "SubMenu.h"
#include "Time.h"
#include "password.h"
#include "ordlistbox.h"
#include "UefiAction.h"
#include "ResetButton.h"

/*
#include "special.h"
*/

extern SETUP_PKG					*gSetupPkg;
extern SCREEN_INFO					*gScreen;
extern PAGE_LIST					*gPages;
extern PAGE_LIST					*gToolPages;
extern CONTROL_INFO					*gToolControlInfo;
extern CONTROL_INFO					*gControlInfo;
extern VARIABLE_LIST				*gToolVariables;
extern VARIABLE_LIST				*gVariables;
extern PAGE_ID_LIST					*gPageIdList;
extern PAGE_ID_LIST					*gToolPageIdList;
extern PAGE_ID_INFO					*gPageIdInfo;
extern PAGE_ID_INFO					*gToolPageIdInfo;

extern NVRAM_VARIABLE				*gVariableList;
extern NVRAM_VARIABLE				*gFailsafeDefaults;
extern NVRAM_VARIABLE				*gOptimalDefaults;

extern HPK_INFO						*gHpkInfo;

extern GUID_LIST					*gGuidList;

extern FRAME_DATA					*gActiveFrame;

extern ACTION_DATA                  *gMsgBoxAction;
extern UINT16 gDynamicParentPage;
// callback.c
VOID LoadPreviousValues( BOOLEAN showMsgBox );
VOID LoadOptimalDefaults( VOID );
VOID LoadFailsafeDefaults( VOID );
BOOLEAN SaveAndExit( VOID );
VOID SaveWithoutExit( VOID );
BOOLEAN SaveAndReset( VOID );
BOOLEAN ResetSys( VOID );
BOOLEAN ExitApplication( VOID );
VOID ShowHelpMessageBox( VOID );
UINT8 CallbackShowMessageBox( UINTN data, UINT8 type );
EFI_STATUS CallFormCallBack(CONTROL_INFO * pControlData, UINT16 Key, UINT8 Flags, UINTN Action);
VOID UEFICallSetupFormCallBack(UINTN Action );

// Helper
extern CHAR8						*gPrevLanguage;
extern UINTN						gPrevLangSize;

VARIABLE_INFO *VarGetVariableInfoIndex( UINT32 index );
VARIABLE_INFO *VarGetVariableInfoId( UINT32 varId, UINT32 *index );
// variable.h
extern UINTN gSetupCount;
VOID *BBSReOrderDefultLegacyDevOrder(BBS_ORDER_TABLE *DefaultLegDevOrder,UINTN DefaultLegDevOrderSize);
extern EFI_GUID EfiDefaultLegacyDevOrderGuid;
extern EFI_GUID EfiDefaultBootOrderGuid;
extern EFI_GUID EfiDefaultDriverOrderGuid;		
//#define SETUP_PASSWORD_LENGTH	20
extern const UINTN TsePasswordLength;
VOID InitgProgress( VOID );
VOID SetHDDPassword(VOID);
VOID BBSUpdateOrder(UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer);
VOID BootUpdateOrder (UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer);
VOID DriverUpdateOrder (UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer);	

extern VOID MainSetupLoopInit(VOID) ;
EFI_STATUS DoBbsPopupInit(VOID) ;
EFI_STATUS DrawBbsPopupMenu( VOID );
VOID DrawBootOnlyBbsPopupMenu( VOID ) ;
VOID BbsBoot(VOID) ;

EFI_STATUS VarBuildAMIDefaults(VOID);

// Mouse Functions
VOID NumericSoftKbdExit(VOID);
VOID NumericSoftKbdInit(VOID);
VOID PrintableKeysSoftKbdInit(VOID);
VOID PrintableKeysSoftKbdExit(VOID);
VOID MouseInit(VOID);
VOID MouseStop(VOID);
VOID MouseFreeze(VOID);
VOID MouseRefresh(VOID);
BOOLEAN IsMouseClickedonSoftkbd(VOID);
VOID MouseStart(VOID);
VOID MouseDestroy(VOID);
EFI_STATUS MouseReadInfo(VOID *MouseInfo);
EFI_STATUS MouseScrollBarMove(VOID *frame, BOOLEAN bScrollUp, UINT32 Size); 
EFI_STATUS MouseListBoxScrollBarMove(VOID *listbox, BOOLEAN bScrollUp, UINT32 Size); 
EFI_STATUS MouseFrameHandleAction( VOID *frame, VOID *action, VOID *control );
EFI_STATUS MouseSubMenuHandleAction( VOID *submenu, VOID *Data);
EFI_STATUS MouseMsgBoxHandleAction( VOID *msgbox, VOID *Data,BOOLEAN * pRedraw );
EFI_STATUS MouseListBoxHandleAction( VOID *listbox, VOID *Data);
EFI_STATUS MousePopupSelHandleAction( VOID *popupSel, VOID *Data);
EFI_STATUS MousePopupEditHandleAction( VOID *PopupEdit, VOID *Data,BOOLEAN * pRedraw );
EFI_STATUS MouseMenuHandleAction( VOID *menu, VOID *Data );


VOID ReGroupOptions(UINT16 *optionList,UINT16 *OrgOptionList);
EFI_STATUS PopupEditHandlePassword( VOID *PopupEdit, VOID *Data);
EFI_STATUS ActionReadKey( AMI_EFI_KEY_DATA *pAmiKey, UINT64 Timeout );
EFI_STATUS AMIReadKeyStroke(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData);
EFI_STATUS HelperIsPasswordCharValid(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData,UINTN StrIndex,UINTN PasswordLength, CHAR16 *OutChar);
VOID FramePasswordAdvancedCallback(CALLBACK_PASSWORD *callbackData,CHAR16 *saveData);
EFI_STATUS FramePwdCallbackIdePasswordUpdate ( CONTROL_DATA *control,CHAR16 *saveData);
EFI_STATUS PopupPwdAuthenticateIDEPwd(POPUP_PASSWORD_DATA *popuppassword, BOOLEAN *AbortUpdate,VOID *data);
VOID PopupPwdUpdateIDEPwd (VOID);
EFI_STATUS 	PopupPwdHandleActionOverRide(POPUP_PASSWORD_DATA *popuppassword, ACTION_DATA *Data);
UINT32 PopupPasswordCheckInstalled(POPUP_PASSWORD_DATA *popuppassword);
BOOLEAN PopupPasswordAuthenticate( POPUP_PASSWORD_DATA *popuppassword, CHAR16 *Password );
EFI_STATUS FormBrowserLocateSetupHandles(/*EFI_HII_HANDLE*/VOID*	**handleBuffer,UINT16 *count);
BOOLEAN FormBrowserHandleValid(VOID);
VOID GetAMITSEVariable(AMITSESETUP **mSysConf,UINT8 **setup,UINTN *VarSize);
VOID NoVarStoreUpdateSystemAccess(UINT8 sysAccessValue);
VOID SetSystemAccessValueItk(UINT8 sysAccessValue);
VOID VarBuildItkDefaults(VOID);
VOID SpecialUpdatePageControls(UINT32 CurrentPage);
EFI_STATUS PopupPasswordFormCallback(CONTROL_INFO * pControlData,UINT16 Key,UINT8 Flags);
BOOLEAN CheckSystemPasswordPolicy(UINT32 PasswordInstalled);
EFI_STATUS CheckForIDEPasswordInstalled(VOID *popuppassword);
extern UINTN                        gHpkInfoLength;
EFI_STATUS WriteCharLastRowCol(CHAR16 *String, UINT32 Attribute, UINT32 Row, UINT32 Alignment, BOOLEAN UpdateBackground);
BOOLEAN IsWriteCharLastRowColSupport();
BOOLEAN IsReadOnlyGrayout();
BOOLEAN IsGrayoutSelectable();
BOOLEAN IsGroupDynamicPages();
BOOLEAN IsGroupDynamicPagesByClass();
BOOLEAN IsOrphanPagesAsRootPage();
BOOLEAN IsLinkHistorySupport();
BOOLEAN IsSubMenuDisplayTitle();
BOOLEAN IsTseWatchDogTimer();

VOID CsmSaveBBSOrder( VOID *);
VOID CsmBBSSetDisabled(UINT16 Index, VOID **DisDPs, UINTN *DPSize);
VOID CsmRearrangeBBSOrderVariable(VOID *popupSel, UINT8 bIncrease,UINT16 *newOption);
VOID CsmLoadDefaultLegDevOrder(VOID);
EFI_STATUS CsmBBSUpdateOrder(UINT16 *newOption,UINT32 *offset,UINTN *size, VOID **buffer);

BOOLEAN TseLiteIsSpecialOptionList(CONTROL_DATA *);


///IDE Password Hooks
UINT16 TSEIDEPasswordGetName(UINT16 Index);
EFI_STATUS TSEIDEPasswordAuthenticate(CHAR16 *Password,  VOID*  DataPtr, BOOLEAN bCheckUser);
BOOLEAN TSEIDEPasswordUpdate( UINT32 DeviceIndex, CHAR16 *Password, BOOLEAN bCheckUser);
VOID* TSEIDEPasswordGetDataPtr( UINTN Index);
BOOLEAN TSEIDEPasswordGetLocked(UINTN Index);
VOID TSEIDEPasswordCheck();
VOID TSEIDEPasswordFreezeDevices();
VOID TSEUnlockHDD(VOID);
VOID TSESetHDDPassword(VOID);
VOID TSEIDEUpdateConfig(VOID *ideSecConfig, UINTN value);


UINTN TseGetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background);
CHAR16 *TseSkipEscCode(CHAR16 *String);

BOOLEAN TseCheckShiftState(AMI_EFI_KEY_DATA ActionKey, UINT32 HotkeyState);

UINT32 _GetControlVariable(CONTROL_INFO *control);

VOID TseHotkeyPrintScreenSupport();
BOOLEAN TsePrintScreenEventSupport(UINT16 ScanCode);
BOOLEAN IsToggleStateKey(ACTION_DATA *Data);
BOOLEAN IsActiveControlPresent(PAGE_DATA *page);
VOID TSEStringReadLoopEntryHook(VOID);
VOID TSEStringReadLoopExitHook(VOID);
BOOLEAN TSEMouseIgnoreMouseActionHook(VOID);
VOID TSENumericSoftKbdExit(VOID);
VOID TSENumericSoftKbdInit(VOID);
typedef 
VOID
(EFIAPI *AMI_UEFI_LIB_CREATE_POPUP_VA_LIST) (
  IN  UINTN          Attribute,
  OUT EFI_INPUT_KEY  *Key,      OPTIONAL
  IN VA_LIST         Args
  );

typedef struct _AMI_UEFI_LIB_POPUP_PROTOCOL
{
    AMI_UEFI_LIB_CREATE_POPUP_VA_LIST CreatePopUpVaList;
}
AMI_UEFI_LIB_POPUP_PROTOCOL;

EFI_STATUS InstallPopupProtocol();
VOID UninstallPopupProtocol();
VOID CreatePopUpVaList(
                        IN UINTN Attribute,
                        OUT EFI_INPUT_KEY  *Key, OPTIONAL
                        IN VA_LIST Args
                        );
#endif /* _MINISETUP_H_ */


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
