//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Include/Protocol/AMIPostMgr.h $
//
// $Author: Blaines $
//
// $Revision: 9 $
//
// $Date: 11/10/11 7:17p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AMIPostMgr.h
//
// Description:	AMI post manager protocol related code
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "bootflow.h"
#ifndef _ESA_TSE_INTERFACES_H_
#define _ESA_TSE_INTERFACES_H_

//#define ESA_INTERFACES_FOR_TSE_PROTOCOL_GUID \
//   { 0x113fd31a, 0xbe8b, 0x418b, 0xb0, 0x34, 0x7e, 0xaf, 0xe5, 0xc6, 0xc, 0x99 }


typedef enum {
	TSEHANDLE,
	BOOTDATA,
	DRIVERDATA,
	LANGDATA,
	GOP,
	PostStatus,
	QuietBoot,
	TSEDEBUG,
	BOOTDISABLEDOPTION,
	DRIVERDISABLEDOPTION,
	CURRSCREENRES_X,
	CURRSCREENRES_Y,
	BgrtSafeBuffer,
	TableKey
}BUFFER_IDENTIFIER;

typedef enum {
	ACTIVEPAGE,
	ACTIVECONTROL,
	MESSAGEBOX
}BOOTFLOW_OPERATION;

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_MSG_BOX) (
    IN CHAR16  *MsgBoxTitle,
    IN CHAR16  *Message,
    IN UINT8   MsgBoxType,
    OUT UINT8  *MsgBoxSel
	);

typedef 
EFI_STATUS 
(EFIAPI *ESA_POST_MANAGER_INIT_PROGRESSBAR) (
	UINTN x,
	UINTN y,
	UINTN w,
	UINTN h,
	UINTN delta
	);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_SET_PROGRESSBAR_POSITION) (
	);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_INFO_BOX) (
    IN CHAR16  *InfoBoxTitle,
    IN CHAR16  *InfoString,
    IN UINTN   Timeout,
    OUT EFI_EVENT  *Event
	);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_MENU) (
    
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16 *pSelection
);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_MSG_BOX_EX) (
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,
    IN MSGBOX_EX_CATAGORY	 MsgBoxExCatagory,
    IN UINT8	 		MsgBoxType,
    IN UINT16			*OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
    IN UINT16	 		OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
    IN AMI_POST_MGR_KEY		*HotKeyList, 		// NULL - AnyKeyPress closes
    IN UINT16 			HotKeyListCount, 
    OUT UINT8			*MsgBoxSel,
    OUT AMI_POST_MGR_KEY	*OutKey
);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DRAW_PROGRESS_BOX) (
	IN UINT8	ProgressBoxState, 
	IN CHAR16	*Title,
	IN CHAR16	*Message,
	IN CHAR16	*Legend,
	IN UINTN 	Percent,	// 0 - 100
	IN OUT VOID	**Handle,	//Out HANDLE. Valid Handle for update and close
	OUT AMI_POST_MGR_KEY	*OutKey	//Out Key
);

typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_TEXT_BOX) (
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
	);


typedef
EFI_STATUS
(EFIAPI *ESA_POST_MANAGER_DISPLAY_PASSWORD_WINDOW) (
		    CHAR16 *PasswordEntered,
		        UINTN PasswordLength,
		        UINTN CurrXPos,
		        UINTN CurrYPos,
		        UINTN *pTimeOut
	);

typedef
VOID
(EFIAPI *ESA_POST_MANAGER_DRAW_WINDOW) (
             UINT16 PromptToken,
			 UINTN PasswordLength,
			 UINTN *CurrXPos,
			 UINTN *CurrYPos
	);
	
typedef
VOID
(EFIAPI *ESA_POST_MANAGER_REPORT_IN_BOX) (
		   UINTN PasswordLength,
		   UINT16 BoxToken,
           UINTN CurrXPos,
           UINTN CurrYPos,
           BOOLEAN bWaitForReturn
	);
		
typedef
EFI_STATUS 
(EFIAPI *ESA_INIT_GRAPHICS_LIB_ENTRY) (
		EFI_HANDLE ImageHandle,
		EFI_SYSTEM_TABLE * ptrST
		);

typedef
EFI_STATUS 
(EFIAPI *ESA_TSE_SET_GET_VARIABLES) (
		BUFFER_IDENTIFIER DataType,
		BOOLEAN 	SetData,
		void	**Buffer,
		UINTN 	*BufferSize
		);

typedef
EFI_STATUS 
(EFIAPI *ESA_ENTER_SETUP) ();
typedef
UINT32
(EFIAPI *ESA_POST_CHECK_SYSTEM_PASSWORD) (
	IN UINT32 EmptyPasswordType,
	IN UINTN *NoOfRetries,
	IN UINTN *TimeOut
	);

typedef
TSE_POST_STATUS
(EFIAPI *ESA_POST_MANAGER_POST_STATUS) (
	);

typedef
TSE_POST_STATUS
(EFIAPI *ESA_POST_MANAGER_POST_STATUS) (
	);

typedef
EFI_STATUS 
(EFIAPI *ESA_BOOTFLOW_OPERATIONS) (
		BOOTFLOW_OPERATION Operation,
		BOOT_FLOW 				*bootFlowPtr
		);

typedef
EFI_STATUS 
(EFIAPI *ESA_RESERVED_OPERATION1) (
		VOID  *Buffer,
		UINTN Size
		);

typedef
EFI_STATUS 
(EFIAPI *ESA_RESERVED_OPERATION2) (
		VOID   *Buffer,
		UINTN   Size,
		UINT8	Type
		);

typedef
EFI_STATUS 
(EFIAPI *ESA_RESERVED_OPERATION3) (
		VOID   *Buffer1,
		VOID   *Buffer2,
		VOID   *Buffer3,
		UINTN   Size1,
		UINTN   Size2,
		UINT8	Type1,
		UINT8	Type2
		);

typedef struct
{
	ESA_POST_MANAGER_DISPLAY_MSG_BOX            			DisplayMsgBox;
	ESA_POST_MANAGER_INIT_PROGRESSBAR				InitProgressBar;
	ESA_POST_MANAGER_SET_PROGRESSBAR_POSITION	SetProgressBarPosition;
	ESA_POST_MANAGER_POST_STATUS						GetPostStatus;		
	ESA_POST_MANAGER_DISPLAY_INFO_BOX					DisplayInfoBox;
	ESA_POST_MANAGER_DISPLAY_MENU						DisplayPostMenu;
	ESA_POST_MANAGER_DISPLAY_MSG_BOX_EX				DisplayMsgBoxEx;
	ESA_POST_MANAGER_DRAW_PROGRESS_BOX			DisplayProgress;
	ESA_POST_MANAGER_DISPLAY_TEXT_BOX       				DisplayTextBox;
	ESA_POST_MANAGER_DISPLAY_PASSWORD_WINDOW	GetPassword;
	ESA_POST_MANAGER_DRAW_WINDOW						DrawPasswordWindow;
	ESA_POST_MANAGER_REPORT_IN_BOX					ReportInBox;
	ESA_INIT_GRAPHICS_LIB_ENTRY								InitGraphicsLibEntry;
	ESA_ENTER_SETUP											LaunchEsaSetup;
	ESA_TSE_SET_GET_VARIABLES								EsaTseSetGetVariables;
	ESA_POST_CHECK_SYSTEM_PASSWORD					CheckSystemPassword;
	ESA_BOOTFLOW_OPERATIONS								BootFlowOperations;
	ESA_RESERVED_OPERATION1								Reserved1;
	ESA_RESERVED_OPERATION2								Reserved2;
	ESA_RESERVED_OPERATION3								Reserved3;
	
    //uninstalling the protocol
}ESA_INTERFACES_FOR_TSE;

extern EFI_GUID	gEsaInterfacesForTseProtocolGuid;
extern ESA_INTERFACES_FOR_TSE	*gEsaInterfaceForTSE;

#endif /* _ESA_TSE_INTERFACES_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
