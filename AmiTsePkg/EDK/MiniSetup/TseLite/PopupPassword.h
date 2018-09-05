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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/PopupPassword.h $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 5/18/12 6:56a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file PopupPassword.h
    Header file for Popup Passwords

**/

#ifndef _POPUP_PASSWORD_H_
#define	_POPUP_PASSWORD_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "control.h"
#include "action.h"
#include "Label.h"
#include "PopupEdit.h" 


#define POPUP_PASSWORD_MEMBER_VARIABLES \
UINT16 TextMargin; \
UINT16 TextWidth; \
UINT16 TextAreaWidth;


typedef struct _POPUP_PASSWORD_METHODS	POPUP_PASSWORD_METHODS;

typedef struct _POPUP_PASSWORD_DATA
{
	POPUP_PASSWORD_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	LABEL_MEMBER_VARIABLES
	POPUP_PASSWORD_MEMBER_VARIABLES

}
POPUP_PASSWORD_DATA;

#define	POPUP_PASSWORD_METHOD_FUNCTIONS


struct _POPUP_PASSWORD_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	POPUP_PASSWORD_METHOD_FUNCTIONS

};

extern POPUP_PASSWORD_METHODS gPopupPassword;

// Object Methods
EFI_STATUS PopupPasswordCreate( POPUP_PASSWORD_DATA **object );
EFI_STATUS PopupPasswordDestroy( POPUP_PASSWORD_DATA *object, BOOLEAN freeMem );
EFI_STATUS PopupPasswordInitialize( POPUP_PASSWORD_DATA *object, VOID *data );
EFI_STATUS PopupPasswordDraw( POPUP_PASSWORD_DATA *object );
EFI_STATUS PopupPasswordHandleAction(POPUP_PASSWORD_DATA *object, ACTION_DATA *Data);
EFI_STATUS PopupPasswordSetCallback( POPUP_PASSWORD_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS PopupPasswordSetFocus( POPUP_PASSWORD_DATA *object, BOOLEAN focus);
EFI_STATUS PopupPasswordSetPosition(POPUP_PASSWORD_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS PopupPasswordSetDimensions(POPUP_PASSWORD_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS PopupPasswordSetAttributes(POPUP_PASSWORD_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS PopupPasswordGetControlHeight(POPUP_PASSWORD_DATA *object, VOID *frame, UINT16 *height);

EFI_STATUS _DoPopupEdit( POPUP_PASSWORD_DATA *PopupPassword, UINT16 Title, CHAR16 **Text);

#endif /* _POPUP_PASSWORD_H_ */


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
