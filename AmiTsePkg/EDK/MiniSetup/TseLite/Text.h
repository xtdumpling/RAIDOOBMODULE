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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Text.h $
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
/** @file Text.h
    Header file for code to handle text operations

**/
#ifndef _TEXT_H_
#define	_TEXT_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h" 
#endif

#include "control.h"

#define	TEXT_MEMBER_VARIABLES	\
	CHAR16	*LabelText;			\
	UINT16	LabelMargin;		\
	CHAR16	*Text;				\
	UINT8	Interval;

typedef struct _TEXT_METHODS	TEXT_METHODS;

typedef struct _TEXT_DATA
{
	TEXT_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	TEXT_MEMBER_VARIABLES

}
TEXT_DATA;

typedef EFI_STATUS	(*TEXT_METHOD_SET_TEXT) ( TEXT_DATA *object, CHAR16 *label, CHAR16 *value );
#define	TEXT_METHOD_FUNCTIONS                   \
	TEXT_METHOD_SET_TEXT        SetText;

struct _TEXT_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	TEXT_METHOD_FUNCTIONS
};

extern TEXT_METHODS gText;

// Object Methods
EFI_STATUS TextCreate( TEXT_DATA **object );
EFI_STATUS TextDestroy( TEXT_DATA *object, BOOLEAN freeMem );
EFI_STATUS TextInitialize( TEXT_DATA *object, VOID *data );
EFI_STATUS TextDraw( TEXT_DATA *object );
EFI_STATUS TextHandleAction(TEXT_DATA *object, ACTION_DATA *Data);
EFI_STATUS TextSetCallback( TEXT_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS TextSetFocus(TEXT_DATA *object, BOOLEAN focus);
EFI_STATUS TextSetPosition(TEXT_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS TextSetDimensions(TEXT_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS TextSetAttributes(TEXT_DATA *object, UINT8 FGColor, UINT8 BGColor);

// Text Methods
EFI_STATUS TextSetText( TEXT_DATA *object, CHAR16 *label, CHAR16 *value );
EFI_STATUS TextGetControlHeight( TEXT_DATA *text,VOID *frame, UINT16 *height );

#endif /* _TEXT_H_ */


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
