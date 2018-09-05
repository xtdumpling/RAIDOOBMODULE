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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/PopupEdit.h $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 4/05/12 7:25a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file PopupEdit.h
    This file contains definitions to handle Popup Edit operations

**/

#ifndef _POPUPEDIT_H_
#define	_POPUPEDIT_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "popup.h"
//#include "memo.h"

#define PASSWORD_CHAR   L'*'
// popupedit constants
#define MAX_POPUPEDIT_WIDTH		(gMaxCols - 10)
#define POPUPEDIT_TYPE_STRING	0
#define POPUPEDIT_TYPE_PASSWORD	1
#define POPUPEDIT_CANCEL		1

#define	POPUPEDIT_MEMBER_VARIABLES	\
    VOID	*ScreenBuf;			\
    UINT8	PopupEditType; \
	UINT8	Justify;			\
	UINT16	DisplayLines;		\
    CHAR16  *EmptyString; \
    UINT16	MinSize;		\
	UINT16	MaxSize;		\
	UINT16	TextWidth;		\
    CHAR16  *Text;			\
	CHAR16	*TempText;		\
	CHAR16	Chr;		      /* character used to display instead of real chars */
								 /* mainly used for password entry.*/

typedef struct _POPUPEDIT_METHODS	POPUPEDIT_METHODS;

typedef struct _POPUPEDIT_DATA
{
	POPUPEDIT_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	POPUP_MEMBER_VARIABLES
	POPUPEDIT_MEMBER_VARIABLES
}
POPUPEDIT_DATA;

typedef	EFI_STATUS	(*POPUPEDIT_METHOD_SET_TEXT)		( POPUPEDIT_DATA *object, CHAR16 *String);
typedef	EFI_STATUS	(*POPUPEDIT_METHOD_SET_TYPE)		( POPUPEDIT_DATA *object, UINT8 Type);

#define	POPUPEDIT_METHOD_FUNCTIONS \
	POPUPEDIT_METHOD_SET_TYPE		SetType; \
	POPUPEDIT_METHOD_SET_TEXT		SetText;		

struct _POPUPEDIT_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
    POPUPEDIT_METHOD_FUNCTIONS
};

extern POPUPEDIT_METHODS gPopupEdit;


// Object Methods
EFI_STATUS PopupEditCreate( POPUPEDIT_DATA **object );
EFI_STATUS PopupEditDestroy( POPUPEDIT_DATA *object, BOOLEAN freeMem );
EFI_STATUS PopupEditInitialize( POPUPEDIT_DATA *object, VOID *data );
EFI_STATUS PopupEditDraw( POPUPEDIT_DATA *object );
EFI_STATUS PopupEditHandleAction(POPUPEDIT_DATA *object, ACTION_DATA *Data);
EFI_STATUS PopupEditSetCallback(  POPUPEDIT_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );
EFI_STATUS PopupEditSetType(POPUPEDIT_DATA *object, UINT8 Type );
//EFI_STATUS PopupEditSetTitle(VOID *object, CHAR16 *String );
EFI_STATUS PopupEditSetText(POPUPEDIT_DATA *object, CHAR16 *String );

// Control Methods
EFI_STATUS PopupEditSetFocus(POPUPEDIT_DATA *object, BOOLEAN focus);
EFI_STATUS PopupEditSetPosition(POPUPEDIT_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS PopupEditSetDimensions(POPUPEDIT_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS PopupEditSetAttributes(POPUPEDIT_DATA*object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS PopupEditGetControlHeight( POPUPEDIT_DATA *object,VOID *frame, UINT16 *height );
CHAR16 *StringWrapTextWithoutJustification ( CHAR16 *OrgText, UINT16 width, UINT16 *height );

#endif /* _POPUPEDIT_H_ */


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
