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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/edit.h $
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
/** @file edit.h
    Header file for Edit controls

**/

#ifndef _EDIT_H_
#define	_EDIT_H_


#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "control.h"
#include "action.h"

#define	EDIT_MEMBER_VARIABLES	\
    CHAR16	*Text;			\
	CHAR16	*TempText;		\
	UINT16	TextMargin;		\
    UINT16	MinSize;		\
	UINT16	MaxSize;		\
	UINT16	TextWidth;		\
	UINT16  TextAreaWidth;	\
    UINT8	LabelFGColor;	\
	UINT8	NSelLabelFGColor;		\
	UINT8   LabelMargin;	\
	UINT8	SelFGColor;		\
	UINT8	SelBGColor;		\
	UINT8	EditFGColor;	\
	CHAR16	Chr;			 /* character used to display instead of real chars from keyboard.*/
								 /* mainly used for password entry.*/

typedef struct _EDIT_METHODS	EDIT_METHODS;

typedef struct _EDIT_DATA
{
	EDIT_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	EDIT_MEMBER_VARIABLES

}
EDIT_DATA;

#define	EDIT_METHOD_FUNCTIONS

struct _EDIT_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	EDIT_METHOD_FUNCTIONS

};

extern EDIT_METHODS gEdit;

// Object Methods
EFI_STATUS EditCreate( EDIT_DATA **object );
EFI_STATUS EditDestroy( EDIT_DATA *object, BOOLEAN freeMem );
EFI_STATUS EditInitialize( EDIT_DATA *object, VOID *data );
EFI_STATUS EditDraw( EDIT_DATA *object );
EFI_STATUS EditHandleAction(EDIT_DATA *object, ACTION_DATA *Data);
EFI_STATUS EditSetCallback( EDIT_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS EditSetFocus( EDIT_DATA *object, BOOLEAN focus);
EFI_STATUS EditSetPosition(EDIT_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS EditSetDimensions(EDIT_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS EditSetAttributes(EDIT_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS EditGetControlHeight( EDIT_DATA *object,VOID *frame, UINT16 *height );

#endif /* _EDIT_H_ */


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
