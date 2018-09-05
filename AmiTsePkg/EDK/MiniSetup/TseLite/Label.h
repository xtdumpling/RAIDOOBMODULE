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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Label.h $
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
/** @file Label.h
    Header file for Label controls.

**/

#ifndef _LABEL_H_
#define	_LABEL_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h" 
#endif

#include "control.h"
#include "action.h"
#include "MessageBox.h"

typedef struct _IFR_LABEL
{
	// this structure is, in some way, following EFI_IFR structures
	UINT8	Opcode, Length;
	UINT16	Text;
	UINT16	Help;
}
AMI_IFR_LABEL;

#define	LABEL_MEMBER_VARIABLES	\
	UINT8		SelFGColor;		\
	UINT8		SelBGColor;		\
    CHAR16		*Text;

typedef struct _LABEL_METHODS	LABEL_METHODS;

typedef struct _LABEL_DATA
{
	LABEL_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	LABEL_MEMBER_VARIABLES

}
LABEL_DATA;

#define	LABEL_METHOD_FUNCTIONS

struct _LABEL_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	LABEL_METHOD_FUNCTIONS

};

extern LABEL_METHODS gLabel;

// Object Methods
EFI_STATUS LabelCreate( LABEL_DATA **object );
EFI_STATUS LabelDestroy( LABEL_DATA *object, BOOLEAN freeMem );
EFI_STATUS LabelInitialize( LABEL_DATA *object, VOID *data );
EFI_STATUS LabelDraw( LABEL_DATA *object );
EFI_STATUS LabelHandleAction(LABEL_DATA *object, ACTION_DATA *Data);
EFI_STATUS LabelSetCallback( LABEL_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS LabelSetFocus( LABEL_DATA *object, BOOLEAN focus);
EFI_STATUS LabelSetPosition(LABEL_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS LabelSetDimensions(LABEL_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS LabelSetAttributes(LABEL_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS LabelGetControlHeight(LABEL_DATA *object, VOID *frame, UINT16 *height);
#endif /* _LABEL_H_ */


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
