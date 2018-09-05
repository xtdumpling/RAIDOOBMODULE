//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Ezport/style.h $
//
// $Author: Rajashakerg $
//
// $Revision: 9 $
//
// $Date: 6/29/11 12:24p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file style.h
    Style override prototypes.

**/
//**********************************************************************
//#define SETUP_STYLE_FULL_SCREEN 1
//#define SETUP_GO_TO_EXIT_PAGE_ON_EXIT_KEY 0 

#ifndef _STYLE_H_
#define _STYLE_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h" 
#endif

//#define SETUP_STYLE_EZPORT 	1
#include "ezport.h"
#include "page.h"
#include "frame.h"

INTN GetWindowLevel( UINT16	PageID);
EFI_STATUS StyleGetWindowColor(UINT8 *Color, INTN Level, UINT8 WindowType) ;
EFI_STATUS StyleGetMessageboxColor( UINT8 MessageBoxType, UINT8 *Color);
EFI_STATUS StyleControlColor(FRAME_DATA *frame, STYLECOLORS *Colors);
EFI_STATUS OverRideControlColor(FRAME_DATA *frame, STYLECOLORS *Colors);
VOID	StyleInit( VOID );
VOID	StyleExit( VOID );
VOID	StyleDrawPageBorder( UINT32 page );
UINT32	StyleGetPageFrames( UINT32 page );
UINT32		StyleFrameIndexOf( UINT32 frameType );
VOID		StyleBeforeFrame( FRAME_DATA *frame ) ;
EFI_STATUS	StyleFrameDrawable( FRAME_DATA *frame );
VOID	StyleUpdateFrameStrings(PAGE_DATA *page);
VOID *	StyleGetFrameInitData( UINT32 page, UINT32 frame );

EFI_STATUS StyleFrameHandleMouse( FRAME_DATA *frame, MOUSE_INFO MouseInfo);
EFI_STATUS StyleFrameHandleTimer( FRAME_DATA *frame, ACTION_DATA *action );
EFI_STATUS StyleFrameHandleKey( FRAME_DATA *frame, EFI_INPUT_KEY Key );
EFI_STATUS StyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count );
EFI_STATUS StyleInitializeFrame( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data );
UINT16 StyleGetNavToken( UINT32 page );
EFI_STATUS StyleAddAdditionalControls( FRAME_DATA *frame, UINT32 controlNumber, BOOLEAN focus );

//EFI_STATUS StyleMenuCallback( FRAME_DATA *frame, MENU_DATA *menu, CALLBACK_MENU *data );
VOID StyleLabelCallback( FRAME_DATA *frame, LABEL_DATA *label, VOID *cookie );

EFI_STATUS _StyleAddControl( FRAME_DATA *frame, UINT16 type, VOID *data, UINT32 *number, BOOLEAN *focus );
UINT16 StyleLanguagePage( SUBMENU_DATA *submenu );
UINT16 StyleBootManagerPage( SUBMENU_DATA *submenu );
BOOLEAN StyleHelpAreaScrollable(VOID);
VOID StylePageItemFocus(PAGE_DATA *page, FRAME_DATA *frame );
VOID StyleSubPageItemFocus(PAGE_DATA *page, FRAME_DATA *frame );
BOOLEAN StyleGetShadowSupport(VOID);
VOID StyleDrawScrollBar(FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, 
				UINT32 sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar);

VOID StyleDrawHelpScrollBar( MEMO_DATA *memo, UINT16 height);


VOID StyleSelectFirstDisplayPage( VOID );
VOID StyleAddExtraPagesData( VOID );
EFI_STATUS StyleGetSpecialColor( UINT16 ControlType, UINT8 *Color);
UINTN StyleGetDateFormat(VOID);
BOOLEAN StyleShowDay(VOID);
BOOLEAN StyleGetScrollBehavior(VOID) ;

VOID StyleFrameSetControlPositions(FRAME_DATA *frame,  UINT32 *pOtherEnd);
extern HOTKEY_TEMPLATE gHotKeyInfo[];
extern HOTCLICK_TEMPLATE gHotClickInfo[];
extern UINT32 gHotKeyCount;
extern UINT32 gHotClickCount;
//extern UINT8 gLabelLeftMargin ;
//extern UINT8 gControlLeftMargin ;
//extern FRAME_INFO _gStyleStandardSubFrames[] ;
extern UINT32 gSubFrameCount ;
extern EFI_STATUS FrameDrawScrollBar (FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, UINT32 sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar);
EFI_STATUS StyleFrameAddTitle( FRAME_DATA *frame, UINT32 frameType,CONTROL_INFO * dummy );


//Common Overrides
EFI_STATUS 	OverRideStyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count );
EFI_STATUS 	OverRideStyleInitializeFrame( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data );
UINT16 		OverRideStyleGetNavToken( UINT32 page );
EFI_STATUS 	OverRideStyleAddAdditionalControls( FRAME_DATA *frame, UINT32 controlNumber, BOOLEAN focus );
VOID 		OverRideStyleSelectFirstDisplayPage( VOID );
VOID 		OverRideStyleUpdateVersionString( VOID );
EFI_STATUS 	OverRideStyleFrameDrawable( FRAME_DATA *frame );

//Oem Overrides
UINTN		OEMStyleGetDateFormat(VOID);
EFI_STATUS 	OEMGetSpecialColor(UINT16 ControlType, UINT8 *Color);
VOID 		OEMStyleBeforeFrame( FRAME_DATA *frame );
VOID 		OEMStyleUpdateFrameStrings( PAGE_DATA *page );
EFI_STATUS 	OEMStyleFrameDrawable( FRAME_DATA *frame );
VOID		OEMStyleInit( VOID );
VOID		OEMStyleExit( VOID );
VOID 		OEMStyleDrawPageBorder( UINT32 page );
VOID 		OEMDrawScrollBar(FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, UINT32 sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar);
VOID 		OEMDrawHelpScrollBar( MEMO_DATA *memo, UINT16 height);
VOID 		*OEMStyleGetFrameInitData( UINT32 page, UINT32 frame );
EFI_STATUS 	OEMStyleFrameHandleKey( FRAME_DATA *frame, EFI_INPUT_KEY Key );
EFI_STATUS 	OEMStyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count );
EFI_STATUS 	OEMStyleInitializeFrame( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data );
UINT16 		OEMStyleGetNavToken( UINT32 page );
EFI_STATUS 	OEMStyleAddAdditionalControls( FRAME_DATA *frame, UINT32 controlNumber, BOOLEAN focus );
UINT16 		OEMStyleLanguagePage( SUBMENU_DATA *submenu );
UINT16 		OEMStyleBootManagerPage( SUBMENU_DATA *submenu );
VOID 		OEMStyleLabelCallback( FRAME_DATA *frame, LABEL_DATA *label, VOID *cookie );
VOID 		OEMStyleSelectFirstDisplayPage( VOID );
VOID 		OEMStyleAddExtraPagesData(VOID);
UINTN 		OEMStyleGetTextMode( UINTN Rows, UINTN Cols );
VOID 		OEMStyleUpdateVersionString( VOID );
VOID 		OEMStyleFrameSetControlPositions(FRAME_DATA *frame, UINT32 *pOtherEnd);
VOID 		OEMOverRideComponent(VOID);
VOID 		OEMStylePageItemFocus(PAGE_DATA *page, FRAME_DATA *frame );
VOID 		OEMStyleSubPageItemFocus(PAGE_DATA *page, FRAME_DATA *frame );

#endif /* _STYLE_H_ */
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                       All Rights Reserved.                  **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                       Phone: (770)-246-8600                 **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
