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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/bootflow.h $
//
// $Author: Rajashakerg $
//
// $Revision: 13 $
//
// $Date: 9/17/12 5:59a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file bootflow.h
    bootflow header

**/

#ifndef _BOOTFLOW_H_
#define	_BOOTFLOW_H_

/* EF152FB4-7B2F-427D-BDB4-7E0A05826E64 */
#define	BOOT_FLOW_VARIABLE_GUID \
	{ 0xEF152FB4, 0x7B2F, 0x427D, 0xBD, 0xB4, 0x7E, 0x0A, 0x05, 0x82, 0x6E, 0x64 }

#define	TPM_BOOT_FLOW_VARIABLE_GUID \
	{ 0xb6460a49, 0x0ac1, 0x484f, 0xae, 0x58, 0xf1, 0x6e, 0xb2, 0x39, 0xdb, 0x3d }

#define	BOOT_FLOW_CONDITION_NORMAL	0
#define	BOOT_FLOW_CONDITION_ERROR	1
#define	BOOT_FLOW_CONDITION_RECOVERY 2
#define	BOOT_FLOW_CONDITION_FIRST_BOOT 4

#define	BOOT_FLOW_CONDITION_OEM_KEY1	0x00001001
#define	BOOT_FLOW_CONDITION_OEM_KEY2	0x00001002
#define	BOOT_FLOW_CONDITION_OEM_KEY3	0x00001003
#define	BOOT_FLOW_CONDITION_OEM_KEY4	0x00001004
#define	BOOT_FLOW_CONDITION_OEM_KEY_CALLBACK	0x00001005
#define BOOT_FLOW_CONDITION_NO_SETUP	        5
#define	BOOT_FLOW_CONDITION_BBS_POPUP	        6
#define	BOOT_FLOW_CONDITION_FAST_BOOT	        7
#define BOOT_FLOW_CONDITION_PCI_OUT_OF_RESOURCE 9
#define	BOOT_FLOW_HOTKEY_BOOT                   0xA
#define	BOOT_FLOW_CONDITION_OS_UPD_CAP 			0xB			
#define	BOOT_FLOW_CONDITION_NULL	0xFFFFFFFF

typedef struct _BOOT_FLOW	BOOT_FLOW;

typedef EFI_STATUS	(*BOOT_FLOW_CALLBACK)( BOOT_FLOW * );

struct _BOOT_FLOW
{
	UINT32				Condition;
	UINT16				PageClass;
	UINT16				PageSubClass;
	UINT16				PageFormID;
	UINT32				ControlNumber;
	UINT16				MessageBoxToken;
	UINT16				MessageBoxTimeout;
	BOOLEAN				GotoPageOnEntry;
	BOOLEAN				ProceedBooting;
	BOOLEAN				InfiniteLoop;
	BOOLEAN				LaunchShell;
	BOOLEAN				DoNotEnterSetup;
	BOOT_FLOW_CALLBACK	CallbackFunction;
};

EFI_STATUS BootFlowManageEntry( VOID );
EFI_STATUS BootFlowManageExit( VOID );
VOID _BootFlowShowMessageBox( BOOT_FLOW *bootFlowPtr );
VOID _BootFlowSetActivePage( BOOT_FLOW *bootFlowPtr );
VOID _BootFlowSetActiveControl( BOOT_FLOW *bootFlowPtr );

EFI_STATUS	OemKey1Hook ( BOOT_FLOW * );
EFI_STATUS	OemKey2Hook ( BOOT_FLOW * );
EFI_STATUS	OemKey3Hook ( BOOT_FLOW * );
EFI_STATUS	OemKey4Hook ( BOOT_FLOW * );
EFI_STATUS	DoPopup( BOOT_FLOW * );
//EFI_STATUS	DoPopupMenu( BOOT_FLOW * );
//EFI_STATUS	DoBootOnlyPopupMenu( BOOT_FLOW * );
EFI_STATUS	FBBootFlow (BOOT_FLOW *);
EFI_STATUS	LaunchHotKeyBootOption (BOOT_FLOW *);
EFI_STATUS  OsUpdateCapsuleWrap (BOOT_FLOW *BootFlow);
BOOLEAN IsBootFlowInfinite();

#endif /* _BOOTFLOW_H_ */

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
