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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/protocol.h $
//
// $Author: Blaines $
//
// $Revision: 7 $
//
// $Date: 11/10/11 7:21p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#ifndef _PROTOCOL_H_
#define	_PROTOCOL_H_

EFI_STATUS InstallProtocol( VOID );
VOID UninstallProtocol( VOID );
EFI_STATUS PostManagerHandshake( VOID );
EFI_STATUS PostManagerSetCursorPosition(UINTN X, UINTN Y);
EFI_STATUS PostManagerGetCursorPosition(UINTN *pX, UINTN *pY);
EFI_STATUS PostManagerSetProgressBarPosition();
EFI_STATUS PostManagerInitProgressBar(
	UINTN x,
	UINTN y,
	UINTN w,
	UINTN h,
	UINTN delta);

TSE_POST_STATUS	PostManagerGetPostStatus(VOID);

EFI_STATUS PostManagerDisplayInfoBox(	CHAR16		*InfoBoxTitle,
										CHAR16		*InfoString,
										UINTN		Timeout,
										EFI_EVENT	*Event
						 			);


EFI_STATUS PostManagerSetAttribute(UINT8 ATTRIB);
EFI_STATUS PostManagerGetAttribute(UINT8 *ATTRIB);

EFI_STATUS
PostManagerDisplayMenu(
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,	  
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16  *pSelection
);

typedef 
BOOLEAN
(EFIAPI *DISPLAY_TEXT_KEY_VALIDATE) (
	UINT16 ItemIndex,
    UINT16 Unicode,
    CHAR16	*Value
);


EFI_STATUS
PostManagerDisplayTextBox(
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
);

EFI_STATUS PostManagerDisplayProgress (
IN UINT8		ProgressBoxState, 
IN CHAR16		*Title,
IN CHAR16		*Message,
IN CHAR16		*Legend,
IN UINTN 		Percent,	// 0 - 100
IN OUT VOID		**Handle,	//Out HANDLE. Valid Handle for update and close
OUT AMI_POST_MGR_KEY	*OutKey		//Out Key	   
);

EFI_STATUS InstallScreenMgmtProtocol( EFI_HANDLE Handle);
VOID UninstallScreenMgmtProtocol( EFI_HANDLE Handle);

EFI_STATUS ScreenMgmtSaveTseScreen ();

EFI_STATUS ScreenMgmtRestoreTseScreen ();

EFI_STATUS ScreenMgmtClearScreen (
		IN UINT8 Attribute
		);

#endif /* _PROTOCOL_H_ */


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
