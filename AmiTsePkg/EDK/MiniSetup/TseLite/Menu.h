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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Menu.h $
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
/** @file Menu.h
    Header file for Menu controls

**/

#ifndef _MENU_H_
#define	_MENU_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"   
#endif

#include "control.h"

typedef struct _IFR_MENU
{ // this structure is, in some way, following EFI_IFR structures
	UINT8	Opcode, Length;
}AMI_IFR_MENU;

typedef struct _MENU_ENTRY
{
	/*EFI_HII_HANDLE*/VOID* PageHandle;
	UINT16 PageID;
	UINT16 StringToken; // page title to use in menu
}AMI_MENU_ENTRY;


#define	MENU_MEMBER_VARIABLES   \
    UINT8   SelFGColor;         \
    UINT8   SelBGColor;         \
    UINT16   NumEntries;         \
    UINT16   CurrEntry;          \
    UINT8   IsSubMenu;          \
    UINT8   MenuWidth;          \
    UINT16   StartItem;          \
    UINT16   ActualPage;         \
    UINT16   PageNum;            \
    UINT16   ItemsInPage;        \
    AMI_MENU_ENTRY	*Entries;


typedef struct _MENU_METHODS	MENU_METHODS;

typedef struct _MENU_DATA
{
	MENU_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	MENU_MEMBER_VARIABLES

}
MENU_DATA;

typedef	EFI_STATUS	(*MENU_METHOD_SET_WIDTH)		( MENU_DATA *object, UINT8 Width );

#define	MENU_METHOD_FUNCTIONS \
	MENU_METHOD_SET_WIDTH		SetWidth;	

struct _MENU_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	MENU_METHOD_FUNCTIONS
};

extern MENU_METHODS gMenu;

// Object Methods
EFI_STATUS MenuCreate( MENU_DATA **object );
EFI_STATUS MenuDestroy( MENU_DATA *object, BOOLEAN freeMem );
EFI_STATUS MenuInitialize( MENU_DATA *object, VOID *data );
EFI_STATUS MenuDraw( MENU_DATA *object );
EFI_STATUS MenuHandleAction(MENU_DATA *object, ACTION_DATA *Data);
EFI_STATUS MenuSetCallback(  MENU_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS MenuSetFocus(MENU_DATA *object, BOOLEAN focus);
EFI_STATUS MenuSetPosition(MENU_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS MenuSetDimensions(MENU_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS MenuSetAttributes(MENU_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS MenuGetControlHight( MENU_DATA  *object,VOID *frame, UINT16 *height );
//menu methods
EFI_STATUS MenuSetWidth(MENU_DATA *menu,UINT8 Width);

#endif /* _MENU_H_ */


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
