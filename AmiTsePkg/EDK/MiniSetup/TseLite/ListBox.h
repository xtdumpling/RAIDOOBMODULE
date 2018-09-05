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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ListBox.h $
//
// $Author: Madhans $
//
// $Revision: 7 $
//
// $Date: 4/16/10 5:13p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file ListBox.h
    Header file for Listbox controls.

**/

#ifndef _LISTBOX_H_
#define	_LISTBOX_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "popup.h"

#define MAX_VISIBLE_SELECTIONS     (gMaxRows -10)

#define	LISTBOX_MEMBER_VARIABLES	\
    VOID	*ScreenBuf;				\
	/*EFI_HII_HANDLE*/VOID	*ListHandle;				\
	UINT16	Sel;                    \
	UINT16  FirstVisibleSel;		\
	UINT16  LastVisibleSel ;		\
	UINT16	ItemCount;				\
    	UINT16  *PtrTokens;		\
	POSTMENU_TEMPLATE  *PtrItems;	\
	BOOLEAN	 UseScrollbar;  \
	UINT16  QuitPopupLoop ;		\
	UINT16	HiddenItemCount;			

typedef struct _LISTBOX_METHODS	LISTBOX_METHODS;

typedef struct _LISTBOX_DATA
{
	LISTBOX_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	POPUP_MEMBER_VARIABLES
    LISTBOX_MEMBER_VARIABLES

}
LISTBOX_DATA;


typedef struct _LISTBOX_BSS_POPUP_DATA
{
	UINT16 TitleToken;
	UINT16	ItemCount;
	POSTMENU_TEMPLATE  *PtrItems;
	UINT16 	Help1Token;
	UINT16 	Help2Token;
	UINT16 	Help3Token;
	CONTROL_INFO *pControlData;
	UINT16	HiddenItemCount;
}
LISTBOX_BSS_POPUP_DATA;

typedef EFI_STATUS	(*LISTBOX_METHOD_INITIALIZE_BBS_POPUP)	( LISTBOX_DATA *object, LISTBOX_BSS_POPUP_DATA *data );

// Don't Call Initialize if you call InitializeBBSPopup...
#define	LISTBOX_METHOD_FUNCTIONS 	\
	LISTBOX_METHOD_INITIALIZE_BBS_POPUP 	InitializeBBSPopup;

struct _LISTBOX_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	LISTBOX_METHOD_FUNCTIONS
};

extern LISTBOX_METHODS gListBox;

// Object Methods
EFI_STATUS ListBoxCreate( LISTBOX_DATA **object );
EFI_STATUS ListBoxDestroy( LISTBOX_DATA *object, BOOLEAN freeMem );
EFI_STATUS ListBoxInitialize( LISTBOX_DATA *object, VOID *data );
EFI_STATUS ListBoxDraw( LISTBOX_DATA *object );
EFI_STATUS ListBoxHandleAction(LISTBOX_DATA *object, ACTION_DATA *Data);
EFI_STATUS ListBoxSetCallback( LISTBOX_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS ListBoxSetFocus(LISTBOX_DATA *object, BOOLEAN focus);
EFI_STATUS ListBoxSetPosition(LISTBOX_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS ListBoxSetDimensions(LISTBOX_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS ListBoxSetAttributes(LISTBOX_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS ListBoxGetControlHeight( LISTBOX_DATA *object,VOID *frame, UINT16 *height );

//Listbox Methods
EFI_STATUS ListBoxInitializeBBSPopup( LISTBOX_DATA *listbox, LISTBOX_BSS_POPUP_DATA *listboxBBSPopupData );

#endif /* _LISTBOX_H_ */


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
