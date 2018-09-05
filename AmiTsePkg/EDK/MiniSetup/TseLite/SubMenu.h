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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/SubMenu.h $
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
/** @file SubMenu.h
    Header file for submenu related functionalities

**/

#ifndef _SUBMENU_H_
#define	_SUBMENU_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "control.h"

#define	SUBMENU_MEMBER_VARIABLES	\
	UINT8	LabelFGColor;			\
	UINT8	NSelLabelFGColor;		\
	UINT8  	LabelMargin;			\
    UINT8	SubMenuType;			\
	UINT8	SelFGColor;				\
	UINT8	SelBGColor;				\
	UINT8	NSelFGColor;			\
    UINT8	NSelBGColor;			\
	UINT8   Interval;

typedef struct _SUBMENU_METHODS	SUBMENU_METHODS;

typedef struct _SUBMENU_DATA
{
	SUBMENU_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	SUBMENU_MEMBER_VARIABLES

}
SUBMENU_DATA;

#define	SUBMENU_METHOD_FUNCTIONS

struct _SUBMENU_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	SUBMENU_METHOD_FUNCTIONS
};

extern SUBMENU_METHODS gSubMenu;

// Object Methods
EFI_STATUS SubMenuCreate( SUBMENU_DATA **object );
EFI_STATUS SubMenuDestroy( SUBMENU_DATA *object, BOOLEAN freeMem );
EFI_STATUS SubMenuInitialize( SUBMENU_DATA *object, VOID *data );
EFI_STATUS SubMenuDraw( SUBMENU_DATA *object );
EFI_STATUS SubMenuHandleAction(SUBMENU_DATA *object, ACTION_DATA *Data);
EFI_STATUS SubMenuSetCallback(  SUBMENU_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS SubMenuSetFocus(SUBMENU_DATA *object, BOOLEAN focus);
EFI_STATUS SubMenuSetPosition(SUBMENU_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS SubMenuSetDimensions(SUBMENU_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS SubMenuSetAttributes(SUBMENU_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS SubMenuGetControlHeight(SUBMENU_DATA *submenu, VOID *frame, UINT16 *height);

UINT16 _SubMenuGetSpecialValue( SUBMENU_DATA *submenu );
BOOLEAN _SubMenuHandleSpecialOp( SUBMENU_DATA *submenu );
BOOLEAN _SubMenuAmiCallback( UINT16 value );

// find a better place for these
#define	SAVE_AND_EXIT_VALUE		1
#define	DISCARD_AND_EXIT_VALUE		2
#define	SAVE_VALUE				3
#define	DISCARD_VALUE			4
#define	RESTORE_DEFAULTS_VALUE		5
#define	SAVE_USER_DEFAULTS_VALUE	6
#define	RESTORE_USER_DEFAULTS_VALUE	7
#define PASSWORD_CLEAR              18
#define PASSWORD_INVALID            19
#define NO_VALID_FILES                      21

#endif /* _SUBMENU_H_ */


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
