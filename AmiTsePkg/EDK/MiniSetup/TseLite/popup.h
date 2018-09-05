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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/popup.h $
//
// $Author: Madhans $
//
// $Revision: 6 $
//
// $Date: 4/16/10 5:13p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file popup.h
    Header file for code to handle popup operations

**/

#ifndef _POPUP_H_
#define	_POPUP_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"
#endif

#include "control.h"

// popup constants
#define POPUP_SINGLE_BORDER	1
#define POPUP_DOUBLE_BORDER	2

#define POPUP_STYLE_NORMAL	0
#define POPUP_STYLE_BBS		1

#define	POPUP_MEMBER_VARIABLES	\
	CHAR16	*Title;				\
	UINT16  PopupBorder;		\
	BOOLEAN	Border;				\
	BOOLEAN	Shadow;				\
	UINT16	Style;				\
	CHAR16	*Help1;				\
	CHAR16	*Help2;				\
	CHAR16	*Help3;				

typedef struct _POPUP_METHODS	POPUP_METHODS;

typedef struct _POPUP_DATA
{
	POPUP_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	POPUP_MEMBER_VARIABLES
    
}
POPUP_DATA;

#define	POPUP_METHOD_FUNCTIONS

struct _POPUP_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	POPUP_METHOD_FUNCTIONS
};

extern POPUP_METHODS gPopup;

// Object Methods
EFI_STATUS PopupCreate( POPUP_DATA **object );
EFI_STATUS PopupDestroy( POPUP_DATA *object, BOOLEAN freeMem );
EFI_STATUS PopupInitialize( POPUP_DATA *object, VOID *data );
EFI_STATUS PopupDraw( POPUP_DATA *object );
EFI_STATUS PopupHandleAction(POPUP_DATA *object, ACTION_DATA *Data);
EFI_STATUS PopupSetCallback(  POPUP_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS PopupSetFocus(POPUP_DATA *object, BOOLEAN focus);
EFI_STATUS PopupSetPosition(POPUP_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS PopupSetDimensions(POPUP_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS PopupSetAttributes(POPUP_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS PopupGetControlHeight( POPUP_DATA *object,VOID *frame, UINT16 *height );

#endif /* _POPUP_H_ */


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
