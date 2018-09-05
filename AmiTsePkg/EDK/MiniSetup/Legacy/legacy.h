//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2010, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Legacy/legacy.h $
//
// $Author: Madhans $
//
// $Revision: 1 $
//
// $Date: 3/28/11 11:52p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file legacy.h
    Main header file for alternate1 style module.

**/
//**********************************************************************
#ifndef _LEGACY_H_
#define _LEGACY_H_

#if SETUP_STYLE_LEGACY

VOID 		OverRideStyleInit( VOID );
VOID		OverRideStyleDrawPageBorder( UINT32 page );
VOID 		*OverRideStyleGetFrameInitData( UINT32 page, UINT32 frame );

#if SETUP_STYLE_FULL_SCREEN
	#define	STYLE_MAX_COLS	STYLE_FULL_MAX_COLS
	#define	STYLE_MAX_ROWS	STYLE_FULL_MAX_ROWS
#else
	#define	STYLE_MAX_COLS	STYLE_STD_MAX_COLS
	#define	STYLE_MAX_ROWS	STYLE_STD_MAX_ROWS
#endif

#define MAIN_FULL_Y     3
#define MAIN_Y          3
#define MAIN_FULL_W     98
#define MAIN_W          78


//#define	STYLE_CONTROL_LEFT_PAD	((UINT8)4)
//#define STYLE_CONTROL_RIGHT_AREA_WIDTH 28
//#define	STYLE_CONTROL_LEFT_MARGIN	((UINT8)28)
//#define FULL_STYLE_CONTROL_LEFT_MARGIN	37
//#define FULL_STYLE_CONTROL_LEFT_PAD	2
#define FULL_STYLE_CONTROL_RIGHT_AREA_WIDTH	((UINT8)FULL_VERTICAL_MAIN_DIVIDER - FULL_STYLE_CONTROL_LEFT_MARGIN - FULL_STYLE_CONTROL_LEFT_PAD)
#define FULL_STYLE_LABEL_LEFT_MARGIN	2
#define STYLE_LABEL_LEFT_MARGIN	2
//#define	STYLE_CLEAR_SCREEN_COLOR	(HELPTITLE_BGCOLOR | EFI_WHITE)


// Style Overrides...
#define STYLE_OVERRIDE_INIT
#define STYLE_OVERRIDE_PAGE_BORDER
#define STYLE_OVERRIDE_FRAME_INIT_DATA
#define STYLE_OVERRIDE_CONTROL_OVERFLOW
#define STYLE_OVERRIDE_INITIALIZE_FRAME
#define STYLE_OVERRIDE_INITIALIZE_OEM_FRAME
#define STYLE_OVERRIDE_NAV_TOKEN
#define STYLE_OVERRIDE_ADD_CONTROLS
#define STYLE_OVERRIDE_FIRST_PAGE
#define	STYLE_OVERRIDE_VERSION_STRING
#define STYLE_OVERRIDE_FRAME_DRAWABLE
#define STYLE_COMPONENT_OVERRIDE
#define STYLE_OVERRIDE_HANDLE_KEY
#define STYLE_OVERRIDE_LABEL_CALLBACK
#define MENU_COMPLETE_REDRAW		1
#define SUBMENU_COMPLETE_REDRAW		2
#define STYLE_OVERRIDE_SCROLLBAR
#endif /* SETUP_STYLE_LEGACY */

#endif /* _LEGACY_H_ */
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2010, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
