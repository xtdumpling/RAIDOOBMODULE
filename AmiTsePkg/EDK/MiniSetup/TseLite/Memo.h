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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Memo.h $
//
// $Author: Madhans $
//
// $Revision: 7 $
//
// $Date: 3/09/11 7:23p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file Memo.h
    Header file for Memo controls.

**/

#ifndef _MEMO_H_
#define	_MEMO_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"
#endif

#include "control.h"

#define	MEMO_MEMBER_VARIABLES	\
	UINT8	Justify;			\
	UINT16	DisplayLines;		\
	CHAR16	*EmptyString;		\
	UINT16 	ScrollBarPosition;	\
	UINT8   Interval;


typedef struct _MEMO_METHODS	MEMO_METHODS;

typedef struct _MEMO_DATA
{
	MEMO_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	MEMO_MEMBER_VARIABLES

}
MEMO_DATA;

typedef	EFI_STATUS	(*MEMO_METHOD_SET_JUSTIFY)		( MEMO_DATA *object, UINT8 justify );
typedef EFI_STATUS	(*MEMO_METHOD_GET_TEXT_HEIGHT)	( MEMO_DATA *memo, UINT16 *height );

#define	MEMO_METHOD_FUNCTIONS					\
	MEMO_METHOD_SET_JUSTIFY		SetJustify;		


struct _MEMO_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	MEMO_METHOD_FUNCTIONS
};

extern MEMO_METHODS gMemo;

// Object Methods
EFI_STATUS MemoCreate( MEMO_DATA **object );
EFI_STATUS MemoDestroy( MEMO_DATA *object, BOOLEAN freeMem );
EFI_STATUS MemoInitialize( MEMO_DATA *object, VOID *data );
EFI_STATUS MemoDraw( MEMO_DATA *object );
EFI_STATUS MemoHandleAction(MEMO_DATA *object, ACTION_DATA *Data);
EFI_STATUS MemoSetCallback( MEMO_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS MemoSetFocus(MEMO_DATA *object, BOOLEAN focus);
EFI_STATUS MemoSetPosition(MEMO_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS MemoSetAttributes(MEMO_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS MemoGetControlHeight( MEMO_DATA *object,VOID *frame, UINT16 *height );

// Memo Methods
EFI_STATUS MemoSetDimensions(MEMO_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS MemoSetJustify( MEMO_DATA *object, UINT8 justify );
VOID MemoDrawScrollBar( MEMO_DATA *memo, UINT16 height);
VOID MemoEraseScrollBar( MEMO_DATA *memo);
#endif /* _MEMO_H_ */


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
