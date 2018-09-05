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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ordlistbox.h $
//
// $Author: Premkumara $
//
// $Revision: 7 $
//
// $Date: 11/03/11 4:33a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file ordlistbox.h
    Header file for code to handle ordered list box controls

**/

#ifndef _ORD_LISTBOX_H_
#define	_ORD_LISTBOX_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "PopupSel.h"

//TBD// May be removed if not needed.
#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif

#define SCAN_PLUS  '+'
#define SCAN_MINUS '-'

#define	ORD_LISTBOX_MEMBER_VARIABLES	\
    VOID	*ScreenBuf;				\
	/*EFI_HII_HANDLE*/VOID*  ListHandle;				

/*  These variables are defined in PopupSel control
	UINT16	Sel;                    \
	UINT16	ItemCount;				\
    UINT16  *PtrTokens;*/

typedef struct _ORD_LISTBOX_METHODS	ORD_LISTBOX_METHODS;

typedef struct _ORD_LISTBOX_DATA
{
	ORD_LISTBOX_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	POPUPSEL_MEMBER_VARIABLES
    ORD_LISTBOX_MEMBER_VARIABLES

}
ORD_LISTBOX_DATA;


#define	ORD_LISTBOX_METHOD_FUNCTIONS


struct _ORD_LISTBOX_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	ORD_LISTBOX_METHOD_FUNCTIONS
};

extern ORD_LISTBOX_METHODS gOrdListBox;

// Object Methods
EFI_STATUS OrdListBoxCreate( ORD_LISTBOX_DATA **object );
EFI_STATUS OrdListBoxDestroy( ORD_LISTBOX_DATA *object, BOOLEAN freeMem );
EFI_STATUS OrdListBoxInitialize( ORD_LISTBOX_DATA *object, VOID *data );
EFI_STATUS OrdListBoxDraw( ORD_LISTBOX_DATA *object );
EFI_STATUS OrdListBoxHandleAction(ORD_LISTBOX_DATA *object, ACTION_DATA *Data);
EFI_STATUS OrdListBoxSetCallback( ORD_LISTBOX_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS OrdListBoxSetFocus(ORD_LISTBOX_DATA *object, BOOLEAN focus);
EFI_STATUS OrdListBoxSetPosition(ORD_LISTBOX_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS OrdListBoxSetDimensions(ORD_LISTBOX_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS OrdListBoxSetAttributes(ORD_LISTBOX_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS OrdListBoxGetControlHeight( ORD_LISTBOX_DATA *OrdList,VOID* frame, UINT16 *height );
EFI_STATUS _OrdListBoxShift(ORD_LISTBOX_DATA *listbox, UINT8 bShiftUp);

VOID	_OrdListBoxCallback( ORD_LISTBOX_DATA *container, CONTROL_DATA *listbox, VOID *cookie );
VOID _OrdListGetSelection( ORD_LISTBOX_DATA *ordlist );

#endif /* _ORD_LISTBOX_H_ */


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
