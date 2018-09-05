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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/frame.h $
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
/** @file frame.h
    Header file for Frame control related functions

**/
#ifndef _FRAME_H_
#define _FRAME_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "object.h"
#include "action.h"
#include "control.h"

#include "SubMenu.h" 
#include "Menu.h" 
#include "Text.h"   

#define	MAIN_FRAME	0
#define	TITLE_FRAME	(MAIN_FRAME + 1)
#define	HELP_FRAME	(TITLE_FRAME + 1)
#define	NAV_FRAME	(HELP_FRAME + 1)
#define	SUBTITLE_FRAME	(NAV_FRAME + 1)
#define	HELPTITLE_FRAME	(SUBTITLE_FRAME + 1)
#define	MAINTITLE_FRAME	(HELPTITLE_FRAME + 1)
#define	SCROLLBAR_FRAME	(MAINTITLE_FRAME + 1)
#define MESSAGEBOX_FRAME (SCROLLBAR_FRAME + 1)

typedef struct _FRAME_INFO
{
	UINT32			FrameType;
	BOOLEAN			Drawable;
	BOOLEAN			Border;
	CHAR16			BorderType;
	UINT16			Width;
	UINT16			Height;
	UINT16			Top;
	UINT16			Left;
	UINT8			FGColor;
	UINT8			BGColor;
	UINT8			ScrlFGColor;
	UINT8			ScrlBGColor;
	UINT8			ScrlUpFGColor; 
	UINT8			ScrlUpBGColor;
	UINT8			ScrlDnFGColor; 
	UINT8			ScrlDnBGColor;

}
FRAME_INFO;

typedef struct _AMITSE_CONTROL_MAP
{
	UINT16			ControlType;
	CONTROL_METHODS *ControlMethods;
	BOOLEAN			CallbackSuppored;
	VOID 			*CallbackFunction;	
	VOID 			*CallbackContext;
}AMITSE_CONTROL_MAP;

typedef struct _POSITION_INFO
{
	UINT16	Top;
	UINT16	Left;
}
POSITION_INFO;

#define FRAME_NONE_FOCUSED 0xFFFFFFFF

#define	FRAME_MEMBER_VARIABLES		\
	FRAME_INFO		FrameData;		\
	UINT32			PageID;			\
	UINT32			ControlCount;	\
	UINT32			NullCount;		\
	UINT32			CurrentControl;	\
	CONTROL_DATA	**ControlList;	\
	CHAR16			*BlankLine;		\
	BOOLEAN			UseScrollbar;  \
	UINT32			FirstVisibleCtrl; \
	UINT32			LastVisibleCtrl;  \
	POSITION_INFO	*OrigPosition;	\
	POSITION_INFO	*CurrPosition;

typedef struct _FRAME_METHODS FRAME_METHODS;

typedef struct _FRAME_DATA
{
	FRAME_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	FRAME_MEMBER_VARIABLES

}
FRAME_DATA;

typedef EFI_STATUS	(*FRAME_METHOD_ADD_CONTROL)		( FRAME_DATA *object, CONTROL_INFO *data );


#define	FRAME_METHOD_FUNCTIONS				\
	FRAME_METHOD_ADD_CONTROL	AddControl;	

struct _FRAME_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	FRAME_METHOD_FUNCTIONS
};

extern FRAME_METHODS gFrame;
extern AMITSE_CONTROL_MAP gTseControlMap[];
// Object Methods
EFI_STATUS FrameCreate( FRAME_DATA **object );
EFI_STATUS FrameDestroy( FRAME_DATA *object, BOOLEAN freeMem );
EFI_STATUS FrameInitialize( FRAME_DATA *object, FRAME_INFO *data );
EFI_STATUS FrameDraw( FRAME_DATA *object );
EFI_STATUS FrameHandleAction( FRAME_DATA *object, ACTION_DATA *data );
EFI_STATUS FrameSetCallback( FRAME_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Frame Methods
EFI_STATUS FrameAddControl( FRAME_DATA *object, CONTROL_INFO *data );


VOID _FrameSubmenuCallback( FRAME_DATA *frame, SUBMENU_DATA *submenu, VOID *cookie );
VOID _FrameVariableCallback( FRAME_DATA *frame, CONTROL_DATA *control, VOID *cookie );
VOID _FramePasswordCallback( FRAME_DATA *frame, CONTROL_DATA *control, VOID *cookie );
BOOLEAN _FrameSetControlFocus( FRAME_DATA *frame, UINT32 index );
EFI_STATUS _FrameAddControls( FRAME_DATA *frame, PAGE_INFO *data );
EFI_STATUS _FrameAddTitle( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data );

#endif /* _FRAME_H_ */


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
