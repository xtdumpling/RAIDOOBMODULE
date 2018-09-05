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
/** @file AMIPostMgr.h
    AMI post manager protocol related code

**/

#ifndef _AMI_POSTMGR_H_
#define	_AMI_POSTMGR_H_

#define AMI_POST_MANAGER_PROTOCOL_GUID \
   { 0x8A91B1E1, 0x56C7, 0x4ADC, 0xAB, 0xEB, 0x1C, 0x2C, 0xA1, 0x72, 0x9E, 0xFF }

#ifndef EFI_PROTOCOL_DEFINITION
#define TSE_STRINGIZE(a) #a
#define EFI_PROTOCOL_DEFINITION(a) TSE_STRINGIZE(Protocol/a.h)
#endif

#include EFI_PROTOCOL_DEFINITION(GraphicsOutput)
#include EFI_PROTOCOL_DEFINITION(SimpleTextIn)

#define AMI_POSTMENU_ATTRIB_FOCUS	0
#define AMI_POSTMENU_ATTRIB_NON_FOCUS	1
#define AMI_POSTMENU_ATTRIB_HIDDEN	2
#define AMI_POSTMENU_ATTRIB_EXIT_KEY	3

#define TSE_WINDOW_TYPE_SETUP			0
#define TSE_WINDOW_TYPE_POSTMENU		1

#define TSE_FILTER_KEY_NULL	        0
#define TSE_FILTER_KEY_NUMERIC	    1
#define TSE_FILTER_KEY_ALPHA	    2
#define TSE_FILTER_KEY_ALPHANUMERIC	3
#define TSE_FILTER_KEY_CUSTOM   	4


typedef struct {
	EFI_INPUT_KEY 	Key;
	UINT32 		KeyShiftState;
} AMI_POST_MGR_KEY;

typedef struct _TEXT_INPUT_TEMPLATE
{
	UINT16			ItemToken;
	UINT8	        MaxSize;
    UINT8			FilterKey;
    BOOLEAN         Hidden;
	CHAR16			*Value; 
}
TEXT_INPUT_TEMPLATE;

typedef VOID (EFIAPI *POSTMENU_FUNCTION) (VOID *CallbackContext);

typedef struct _POSTMENU_TEMPLATE
{
	UINT16			ItemToken;
	AMI_POST_MGR_KEY	Key;
	POSTMENU_FUNCTION	Callback;
	VOID			*CallbackContext; 
	UINT8			Attribute;
}
POSTMENU_TEMPLATE;

typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_UGA_PIXEL;

typedef enum _CO_ORD_ATTRIBUTE {
CA_AttributeLeftTop,
CA_AttributeCenterTop,
CA_AttributeRightTop,
CA_AttributeRightCenter,
CA_AttributeRightBottom,
CA_AttributeCenterBottom,
CA_AttributeLeftBottom,
CA_AttributeLeftCenter,
CA_AttributeCenter,
CA_AttributeCustomised
}CO_ORD_ATTRIBUTE;

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_HANDSHAKE) (
	VOID
	);

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_MESSAGE) (
	CHAR16	*Message
	);

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_MESSAGE_EX) (
	CHAR16	*Message,
    UINTN   Attribute
	);

typedef 
BOOLEAN
(EFIAPI *DISPLAY_TEXT_KEY_VALIDATE) (
	UINT16 ItemIndex,
    UINT16 Unicode,
    CHAR16	*Value
);

//Post message ex attributes
#define PM_EX_DONT_ADVANCE_TO_NEXT_LINE     0x00000001

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_QUIETBOOT_MESSAGE) (
    CHAR16 *Message,
    INTN CoOrdX,
    INTN CoOrdY,
    CO_ORD_ATTRIBUTE Attribute,
    EFI_UGA_PIXEL Foreground,
    EFI_UGA_PIXEL Background
    );

//Message box types
#define MSGBOX_TYPE_NULL		0
#define MSGBOX_TYPE_OK			1
#define MSGBOX_TYPE_OKCANCEL	2
#define MSGBOX_TYPE_YESNO		3
#define MSGBOX_TYPE_CUSTOM		4
//Message box result buttons
#define MSGBOX_OK				0
#define MSGBOX_CANCEL			1
#define MSGBOX_YES				0	
#define MSGBOX_NO				1
typedef enum _MSGBOX_EX_CATAGORY {
MSGBOX_EX_CATAGORY_NORMAL,
MSGBOX_EX_CATAGORY_HELP,
MSGBOX_EX_CATAGORY_QUERY,
MSGBOX_EX_CATAGORY_SUCCESS,
MSGBOX_EX_CATAGORY_WARNING,
MSGBOX_EX_CATAGORY_ERROR,
MSGBOX_EX_CATAGORY_HALT,
MSGBOX_EX_CATAGORY_PROGRESS,
MSGBOX_EX_ATTRIB_CLEARSCREEN=0x1000,
} MSGBOX_EX_CATAGORY;

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_MSG_BOX) (
    IN CHAR16  *MsgBoxTitle,
    IN CHAR16  *Message,
    IN UINT8   MsgBoxType,
    OUT UINT8  *MsgBoxSel
	);

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_TEXT_BOX) (
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
	);



typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_SWITCH_TO_POST_SCREEN) (VOID);

typedef 
EFI_STATUS 
(EFIAPI *AMI_POST_MANAGER_SET_CURSOR_POSITION) (
	UINTN X,
	UINTN Y
	);

typedef 
EFI_STATUS 
(EFIAPI *AMI_POST_MANAGER_GET_CURSOR_POSITION) (
	UINTN *pX,
	UINTN *pY
	);

typedef 
EFI_STATUS 
(EFIAPI *AMI_POST_MANAGER_INIT_PROGRESSBAR) (
	UINTN x,
	UINTN y,
	UINTN w,
	UINTN h,
	UINTN delta
	);


typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_SET_PROGRESSBAR_POSITION) (
	);

typedef 
EFI_STATUS 
(EFIAPI *AMI_POST_MANAGER_SET_ATTRIBUTE) (
	UINT8 Attrib
	);

typedef 
EFI_STATUS 
(EFIAPI *AMI_POST_MANAGER_GET_ATTRIBUTE) (
	UINT8 *Attrib
	);


typedef enum _TSE_POST_STATUS {
TSE_POST_STATUS_BEFORE_POST_SCREEN,		// TSE Loaded and yet goto post screen
TSE_POST_STATUS_IN_POST_SCREEN,			// In the post screen
TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN,	// In the quite boot screen
TSE_POST_STATUS_IN_BOOT_TIME_OUT,		// Witing for Boot timeout
TSE_POST_STATUS_ENTERING_TSE,			// Entering TSE
TSE_POST_STATUS_IN_TSE,					// Inside TSE
TSE_POST_STATUS_IN_BBS_POPUP,			// Inside BBS Poupup
TSE_POST_STATUS_PROCEED_TO_BOOT,		// Outside TSE and Booting or in Shell
TSE_POST_STATUS_IN_FRONT_PAGE,    		//Inside Front Page
TSE_POST_STATUS_NO_BOOT_OPTION_FOUND,   	//No boot options found
TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED,    	//All Boot Options failed
TSE_POST_STATUS_ALL_PLATFORM_RECOVERY_OPTIONS_FAILED  //All Platform Recovey Options failed
}TSE_POST_STATUS;

typedef
TSE_POST_STATUS
(EFIAPI *AMI_POST_MANAGER_POST_STATUS) (
	);

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_INFO_BOX) (
    IN CHAR16  *InfoBoxTitle,
    IN CHAR16  *InfoString,
    IN UINTN   Timeout,
    OUT EFI_EVENT  *Event
	);

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DISPLAY_MSG_BOX_EX) (
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

// ProgressBoxState
#define AMI_PROGRESS_BOX_INIT		1
#define AMI_PROGRESS_BOX_UPDATE		2
#define AMI_PROGRESS_BOX_CLOSE		3

typedef
EFI_STATUS
(EFIAPI *AMI_POST_MANAGER_DRAW_PROGRESS_BOX) (
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
(EFIAPI *AMI_POST_MANAGER_DISPLAY_MENU) (
    
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16 *pSelection
);


typedef struct _AMI_POST_MANAGER_PROTOCOL
{
    AMI_POST_MANAGER_HANDSHAKE                  Handshake;
    AMI_POST_MANAGER_DISPLAY_MESSAGE            DisplayPostMessage;
    AMI_POST_MANAGER_DISPLAY_MESSAGE_EX         DisplayPostMessageEx;
    AMI_POST_MANAGER_DISPLAY_QUIETBOOT_MESSAGE  DisplayQuietBootMessage;
    AMI_POST_MANAGER_DISPLAY_MSG_BOX            DisplayMsgBox;
    AMI_POST_MANAGER_SWITCH_TO_POST_SCREEN      SwitchToPostScreen;
	AMI_POST_MANAGER_SET_CURSOR_POSITION		SetCurPos;
	AMI_POST_MANAGER_GET_CURSOR_POSITION		GetCurPos;
	AMI_POST_MANAGER_INIT_PROGRESSBAR			InitProgressBar;
	AMI_POST_MANAGER_SET_PROGRESSBAR_POSITION	SetProgressBarPosition;
	AMI_POST_MANAGER_POST_STATUS				GetPostStatus;
	AMI_POST_MANAGER_DISPLAY_INFO_BOX			DisplayInfoBox;
    AMI_POST_MANAGER_SET_ATTRIBUTE		SetAttribute;
    AMI_POST_MANAGER_DISPLAY_MENU		DisplayPostMenu;
    AMI_POST_MANAGER_DISPLAY_MSG_BOX_EX		DisplayMsgBoxEx;
    AMI_POST_MANAGER_DRAW_PROGRESS_BOX		DisplayProgress;
    AMI_POST_MANAGER_GET_ATTRIBUTE		GetAttribute;	
    AMI_POST_MANAGER_DISPLAY_TEXT_BOX       DisplayTextBox;
}
AMI_POST_MANAGER_PROTOCOL;

extern EFI_GUID	gAmiPostManagerProtocolGuid;

#define TSE_INVALIDATE_BGRT_STATUS_PROTOCOL_GUID \
   { 0x73905351, 0xeb4d, 0x4637, 0xa8, 0x3b, 0xd1, 0xbf, 0x6c, 0x1c, 0x48, 0xeb }

typedef EFI_STATUS (EFIAPI *INVALIDATE_BGRT_STATUS) (void);
typedef struct _TSE_INVALIDATE_BGRT_STATUS_PROTOCOL
{
	INVALIDATE_BGRT_STATUS 	InvalidateBgrtStatusByProtocol;
}TSE_INVALIDATE_BGRT_STATUS_PROTOCOL;

extern EFI_GUID	gTSEInvalidateBgrtStatusProtocolGuid;

#endif /* _AMI_POSTMGR_H_ */

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
