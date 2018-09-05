//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 1985-2012, American Megatrends, Inc.   **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/LogoLib.h $
//
// $Author: Premkumara $
//
// $Revision: 9 $
//
// $Date: 2/02/12 12:40a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file LogoLib.h
    logo related header

**/

#ifndef _LOGOLIB_H_
#define _LOGOLIB_H_

//OEM_POST Logo GUID. Don't change the GUID. ChangeLogo expects this GUID.
#define AMI_OEM_LOGO_GUID \
	{ 0x294b1cef, 0x9beb, 0x42d5, 0x99, 0x71, 0x0c, 0x89, 0x63, 0xcd, 0xaf, 0x02}

typedef enum { 
	BMP_Logo, 
	GIF_Logo, 
	JPEG_Logo,
	PCX_Logo,
	PNG_Logo,
	OEM_Format_LOGO,
    Unsupported_Logo
} LOGO_TYPE;

EFI_STATUS SetScreenResolution(UINTN ResX, UINTN ResY);
EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY);
EFI_STATUS GOPSetScreenResolution(UINTN *Width, UINTN *Height);
EFI_STATUS GOPSetScreenResolutionHook(UINTN *Width, UINTN *Height, UINT32 Index);

EFI_STATUS
GetGraphicsBitMapFromFV ( 
IN EFI_GUID *FileNameGuid,
IN OUT VOID **Image,
IN OUT UINTN *ImageSize
);

EFI_STATUS
DrawImage( 
IN UINT8 *ImageData,
IN UINTN ImageSize,
IN CO_ORD_ATTRIBUTE Attribute,
IN INTN CoOrdX,
IN INTN CoOrdY,
IN BOOLEAN AdjustScreenResolution,
OUT UINTN *Width,
OUT UINTN *Height
);

EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message );
EFI_STATUS PostManagerDisplayPostMessageEx( CHAR16 *message, UINTN Attribute );
EFI_STATUS PostManagerSwitchToPostScreen( VOID );

EFI_STATUS
PostManagerDisplayMsgBox(
    IN CHAR16  *MsgBoxTitle,
    IN CHAR16  *Message,
    IN UINT8   MsgBoxType,
    OUT UINT8  *MsgBoxSel
);

EFI_STATUS
PostManagerDisplayTextBox(
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
);

EFI_STATUS
PostManagerDisplayProgress(
IN UINT8	ProgressBoxState, 
IN CHAR16	*Title,
IN CHAR16	*Message,
IN CHAR16	*Legend,
IN UINTN 	Percent,	// 0 - 100
IN OUT VOID	**Handle,	//Out HANDLE. Valid Handle for update and close
OUT AMI_POST_MGR_KEY	*OutKey	//Out Key    
);

EFI_STATUS
PostManagerDisplayMsgBoxEx(
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

EFI_STATUS
PostManagerDisplayQuietBootMessage(
    CHAR16 *Message,
    INTN CoOrdX,
    INTN CoOrdY,
    CO_ORD_ATTRIBUTE Attribute,
    EFI_UGA_PIXEL Foreground,
    EFI_UGA_PIXEL Background
);

EFI_STATUS PostManagerDisplayInfoBox(	CHAR16		*InfoBoxTitle,
										CHAR16		*InfoString,
										UINTN		Timeout,
										EFI_EVENT	*Event
						 			);

EFI_STATUS PostManagerSetCursorPosition(UINTN X, UINTN Y);
EFI_STATUS PostManagerGetCursorPosition(UINTN *pX, UINTN *pY);
EFI_STATUS PostManagerSetAttribute(UINT8 ATTRIB);
VOID  CleanUpLogo( VOID );

VOID InitPostScreen( VOID );

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2012, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
