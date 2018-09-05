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
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Ezport/ezport.c $
//
// $Author: Rajashakerg $
//
// $Revision: 12 $
//
// $Date: 5/24/12 7:29a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file ezport.c
    Main file for ezport style module.

**/
//**********************************************************************
#include "minisetup.h"
#include "ezport.h"

extern	STYLECOLORS  Colors ;
extern 	AMI_IFR_MENU gMenuData ;

extern UINTN gLabelLeftMargin ;
extern UINTN gControlLeftMargin ;
extern UINTN gControlLeftPad ;
extern UINTN gControlRightAreaWidth ;
static CALLBACK_SUBMENU gMenuCallbackData = { { CALLBACK_TYPE_MENU, sizeof(CALLBACK_MENU) }, 0 };
static CALLBACK_SUBMENU gSubmenuCallbackData = { { CALLBACK_TYPE_SUBMENU, sizeof(CALLBACK_SUBMENU) }, 0 };
static CALLBACK_VARIABLE gVariableCallbackData = { { CALLBACK_TYPE_VARIABLE, sizeof(CALLBACK_VARIABLE) }, 0, 0, 0, NULL };
static CALLBACK_PASSWORD gPasswordCallbackData = { { CALLBACK_TYPE_PASSWORD, sizeof(CALLBACK_PASSWORD) }, 0, 0, 0, NULL, TRUE };

VOID _FrameSubmenuCallback( FRAME_DATA *frame, SUBMENU_DATA *submenu, VOID *cookie );
VOID _FrameMenuCallback( FRAME_DATA *frame, MENU_DATA *menu, VOID *cookie );
VOID HotclickESCaction(VOID);
EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames);

BOOLEAN   gotoExitOnEscKey(EFI_GUID *exitFormGuid, UINT16 *pageClass, UINT16 *pageSubclass, UINT16 *pageFormID); 

#if SETUP_STYLE_EZPORT

#if SETUP_STYLE_FULL_SCREEN
static FRAME_INFO _gStyleFullSubFrames[] =
{
//	{ FrameType		Drawable	Border,		BorderType,	Width,			Height,			Top,			Left,			FGColor,		BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE,		0,		TITLE_FULL_W,		TITLE_FULL_H,		TITLE_FULL_Y,		TITLE_FULL_X,		TITLE_FGCOLOR,		TITLE_BGCOLOR		},// TITLE
	{ SUBTITLE_FRAME,	TRUE,		FALSE,		0,		MENU_FULL_W,		MENU_FULL_H,		MENU_FULL_Y,		MENU_FULL_X,		MENU_FGCOLOR,		MENU_BGCOLOR		},// SUBTITLE (MENU)
	{ MAIN_FRAME,		TRUE,		FALSE,		0,		MAIN_FULL_W,		MAIN_FULL_H,		MAIN_FULL_Y,		MAIN_FULL_X,		PAGE_FGCOLOR,		MAIN_BGCOLOR		},// Main
	{ HELP_FRAME,		TRUE,		FALSE,		0,		HELP_FULL_W,		HELP_FULL_H,		HELP_FULL_Y,		HELP_FULL_X,		HELP_FGCOLOR,		HELP_BGCOLOR		},// HELP
	{ NAV_FRAME,		TRUE,		FALSE,		0,		NAV_FULL_W,		NAV_FULL_H,		NAV_FULL_Y,		NAV_FULL_X,		NAV_FGCOLOR,		NAV_BGCOLOR		},// NAVIGATION
	{ HELPTITLE_FRAME,	TRUE,		FALSE,		0,		HELPTITLE_FULL_W,	HELPTITLE_FULL_H,	HELPTITLE_FULL_Y,	HELPTITLE_FULL_X,	HELPTITLE_FGCOLOR,	HELPTITLE_BGCOLOR	} // HELP TITLE (Copyright)
};
#endif

static FRAME_INFO _gStyleStandardSubFrames[] =
{
//	{ FrameType		Drawable	Border,		BorderType,	Width,		Height,		Top,		Left,		FGColor,		BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE,		0,		TITLE_W,	TITLE_H,	TITLE_Y,	TITLE_X,	TITLE_FGCOLOR,		TITLE_BGCOLOR		},// TITLE
	{ SUBTITLE_FRAME,	TRUE,		FALSE,		0,		MENU_W,		MENU_H,		MENU_Y,		MENU_X,		MENU_FGCOLOR,		MENU_BGCOLOR		},// SUBTITLE (MENU)
	{ MAIN_FRAME,		TRUE,		FALSE,		0,		MAIN_W,		MAIN_H,		MAIN_Y,		MAIN_X,		PAGE_FGCOLOR,		MAIN_BGCOLOR		},// Main
	{ HELP_FRAME,		TRUE,		FALSE,		0,		HELP_W,		HELP_H,		HELP_Y,		HELP_X,		HELP_FGCOLOR,		HELP_BGCOLOR		},// HELP
	{ NAV_FRAME,		TRUE,		FALSE,		0,		NAV_W,		NAV_H,		NAV_Y,		NAV_X,		NAV_FGCOLOR,		NAV_BGCOLOR		},// NAVIGATION
	{ HELPTITLE_FRAME,	TRUE,		FALSE,		0,		HELPTITLE_W,	HELPTITLE_H,	HELPTITLE_Y,	HELPTITLE_X,	HELPTITLE_FGCOLOR,	HELPTITLE_BGCOLOR	} // HELP TITLE (Copyright)
};

AMITSE_CONTROL_MAP gTseControlMap[] =
{
//	ControlType			 ControlMethods		 	CallbackSuppored 	CallbackFunction		CallbackContext		
 { CONTROL_TYPE_MENU, 	 (CONTROL_METHODS*)&gMenu, 	TRUE, (VOID*)(UINTN)&_FrameMenuCallback, (VOID*)(UINTN)&gMenuCallbackData},
 { CONTROL_TYPE_SUBMENU, (CONTROL_METHODS*)&gSubMenu,TRUE, (VOID*)(UINTN)&_FrameSubmenuCallback, (VOID*) &gSubmenuCallbackData},
 { CONTROL_TYPE_LABEL, 	 (CONTROL_METHODS*)&gLabel, 	TRUE, (VOID*)(UINTN)&StyleLabelCallback, (VOID*) NULL},
 { CONTROL_TYPE_TEXT, 	 (CONTROL_METHODS*)&gText, 	FALSE,(VOID*)NULL, (VOID*) NULL},
 { CONTROL_TYPE_POPUP_STRING, (CONTROL_METHODS*)&gPopupString, TRUE, (VOID*)(UINTN)&_FrameVariableCallback, (VOID*) &gVariableCallbackData},
 { CONTROL_TYPE_DATE, (CONTROL_METHODS*)&gDate, FALSE, (VOID*)NULL, (VOID*) NULL},
 { CONTROL_TYPE_TIME, (CONTROL_METHODS*)&gTime, FALSE, (VOID*)NULL, (VOID*) NULL},
 { CONTROL_TYPE_POPUPSEL, (CONTROL_METHODS*)&gPopupSel, TRUE, (VOID*)(UINTN)&_FrameVariableCallback, (VOID*) &gVariableCallbackData},
	// Checkbox is just popup sel
 { CONTROL_TYPE_CHECKBOX, (CONTROL_METHODS*)&gPopupSel, TRUE, (VOID*)(UINTN)&_FrameVariableCallback, (VOID*) &gVariableCallbackData},
 { CONTROL_TYPE_MEMO, (CONTROL_METHODS*)&gMemo, FALSE, (VOID*)NULL, (VOID*)NULL},
 { CONTROL_TYPE_NUMERIC, (CONTROL_METHODS*)&gNumeric, TRUE, (VOID*)(UINTN)&_FrameVariableCallback, (VOID*) &gVariableCallbackData},
 { CONTROL_TYPE_ORDERED_LIST, (CONTROL_METHODS*)&gOrdListBox, TRUE, (VOID*)(UINTN)&_FrameVariableCallback, (VOID*) &gVariableCallbackData},
 { CONTROL_TYPE_PASSWORD, (CONTROL_METHODS*)&gPopupPassword, TRUE, (VOID*)(UINTN)&_FramePasswordCallback, (VOID*)&gPasswordCallbackData},
 { CONTROL_TYPE_ACTION, (CONTROL_METHODS*)&gUefiAction, TRUE, (VOID*)(UINTN) &StyleLabelCallback, (VOID*) NULL},
 { CONTROL_TYPE_RESET, (CONTROL_METHODS*)&gResetButton, TRUE, (VOID*)(UINTN) &StyleLabelCallback, (VOID*) NULL},
	// CONTROL_TYPE_NULL need to be last member 
 { CONTROL_TYPE_NULL, (CONTROL_METHODS*)NULL, FALSE, (VOID*)NULL, (VOID*) NULL}
};

static FRAME_INFO *_gStyleSubFrames = _gStyleStandardSubFrames ;

UINT16 gVerticalMainDivider = VERTICAL_MAIN_DIVIDER ;
UINT16 gHorizontalHelpDivider = HORIZONTAL_HELP_DIVIDER ;
/**
    Function to initialize style

    @param VOID

    @retval VOID

**/
VOID	OverRideStyleInit( VOID )
{

	gLabelLeftMargin = STYLE_LABEL_LEFT_MARGIN ;
	gControlLeftMargin = STYLE_CONTROL_LEFT_MARGIN ;
	gControlLeftPad = STYLE_CONTROL_LEFT_PAD ;
	gControlRightAreaWidth 	= STYLE_CONTROL_RIGHT_AREA_WIDTH ;
		

#if SETUP_STYLE_FULL_SCREEN
	if ( gMaxCols == STYLE_FULL_MAX_COLS )
	{
		_gStyleSubFrames 	= _gStyleFullSubFrames;

		gVerticalMainDivider 	= FULL_VERTICAL_MAIN_DIVIDER ;
		gHorizontalHelpDivider 	= FULL_HORIZONTAL_HELP_DIVIDER ;

		gLabelLeftMargin 	= FULL_STYLE_LABEL_LEFT_MARGIN ;
		gControlLeftMargin 	= FULL_STYLE_CONTROL_LEFT_MARGIN ;
		gControlLeftPad 	= FULL_STYLE_CONTROL_LEFT_PAD ;
		gControlRightAreaWidth 	= FULL_STYLE_CONTROL_RIGHT_AREA_WIDTH ;
	}
#endif

	if(gLabelLeftMargin < 2)
		gLabelLeftMargin = 2 ;
	
	//
	//When Style FrameData is overridden/modified by OEM or other modules, TSE will update again to reflect the changes.
	//
	if ( EFI_SUCCESS == StyleInitRecalculateFrames(_gStyleFullSubFrames) )
	{
		_gStyleSubFrames 	= _gStyleFullSubFrames;
	}
	
}
/**
    Function to get frame index

    @param frameType 

    @retval UINT32 index

**/
UINT32	StyleFrameIndexOf( UINT32 frameType )
{
	UINT32 FrameCount ;
	UINT32 i;
	
	FrameCount = sizeof(_gStyleStandardSubFrames) / sizeof(FRAME_INFO);
	

	for ( i = 0; i < FrameCount; i++ )
	{
		if( _gStyleSubFrames[i].FrameType == frameType ){
			return i ;
			break ;
		}
	}
	
	return i ;
	
}
/**
    Function to number of frames

    @param page 

    @retval UINT32 number of frames

**/
UINT32	StyleGetPageFrames( UINT32 page )
{
	return sizeof(_gStyleStandardSubFrames) / sizeof(FRAME_INFO);
}
/**
    Function to draw page border

    @param page 

    @retval VOID

**/
VOID OverRideStyleDrawPageBorder( UINT32 page )
{
	PAGE_DATA *Page ;
	FRAME_DATA *fTitle, *fMenu, *fHelptitle, *fMain;

	CHAR16 *line;
	UINT16 lineNumber;
	UINT16 start, end, col;
	UINT16 pbx,pby,pbw,pbh;
	UINT16 vertDivider = VERTICAL_MAIN_DIVIDER ;
	UINT16 horzDivider = HORIZONTAL_HELP_DIVIDER;

	Page = gApp->PageList[page];
	
	fTitle = Page->FrameList[StyleFrameIndexOf(TITLE_FRAME)];
	fHelptitle = Page->FrameList[StyleFrameIndexOf(HELPTITLE_FRAME)];
	fMenu = Page->FrameList[StyleFrameIndexOf(SUBTITLE_FRAME)];
	fMain = Page->FrameList[StyleFrameIndexOf(MAIN_FRAME)];
	pbx = PAGEBORDER_X;
	pby = PAGEBORDER_Y;
	pbw = PAGEBORDER_W;
	pbh = PAGEBORDER_H;
	
	//
	// If textmode is greater than 100 OR 31 or lesser than 80 OR 25
	//
	if (( gMaxRows != 31 && gMaxRows != 25  )||(gMaxCols != 100 && gMaxCols != 80 ))
	{
		vertDivider = (UINT16)(gMaxCols*2/3);
		horzDivider = (UINT16)(gMaxRows/2);
		pbx = PAGEBORDER_X;
		pby = PAGEBORDER_Y;
		pbw = (UINT16)gMaxCols;
		pbh = (UINT16)gMaxRows-3;
	}
	else if ( gMaxCols == STYLE_FULL_MAX_COLS )
	{
		vertDivider = FULL_VERTICAL_MAIN_DIVIDER ;
		horzDivider = FULL_HORIZONTAL_HELP_DIVIDER;
		pbx = PAGEBORDER_FULL_X;
		pby = PAGEBORDER_FULL_Y;
		pbw = PAGEBORDER_FULL_W;
		pbh = PAGEBORDER_FULL_H;
	}
		

	line = EfiLibAllocateZeroPool( (gMaxCols + 1) * sizeof(CHAR16) );
	if ( line == NULL )
		return;

	ClearLinesWithoutFlush( fTitle->FrameData.Top, fTitle->FrameData.Height, fTitle->FrameData.BGColor | fTitle->FrameData.FGColor );
	ClearLinesWithoutFlush( fHelptitle->FrameData.Top, fHelptitle->FrameData.Height, fHelptitle->FrameData.BGColor | fHelptitle->FrameData.FGColor );
	ClearLinesWithoutFlush( fMenu->FrameData.Top, fMenu->FrameData.Height, fMenu->FrameData.BGColor | fMenu->FrameData.FGColor );

	//Clearing gFlushBuffer with attribute(0) and chars(' ')
	if (NULL != gFlushBuffer)
	{
		MemSet( &(gFlushBuffer->Attribs[fMain->FrameData.Top*gMaxCols]), fMain->FrameData.Height*gMaxCols, 0 );
		MemFillUINT16Buffer( &(gFlushBuffer->Chars[fMain->FrameData.Top*gMaxCols]), fMain->FrameData.Height*gMaxCols, (UINT16)L' ' );
	}
	ClearLinesWithoutFlush( pby, pbh, PAGE_BGCOLOR | PAGE_FGCOLOR );	
	DrawWindow( pbx, pby, pbw, pbh, PAGE_BGCOLOR| PAGE_FGCOLOR, TRUE, FALSE );



	MemFillUINT16Buffer( &line[1], gMaxCols - 1, BOXDRAW_HORIZONTAL );

	line[0] = BOXDRAW_DOWN_HORIZONTAL;
	line[1] = L'\0';
	DrawString( vertDivider, pby, line );

	line[0] = BOXDRAW_UP_HORIZONTAL;
	DrawString( vertDivider, pby+pbh-1, line );
		
	// center divider
	line[0] = BOXDRAW_VERTICAL;

	col = vertDivider ;
	start = pby+1; 
	end = pby+pbh-1 ;

	for ( lineNumber = start; lineNumber < end ; lineNumber++ )
		DrawString( col, lineNumber, line );

	// Horizontal Help divider 
	MemFillUINT16Buffer( &line[1], gMaxCols - 1, BOXDRAW_HORIZONTAL );
	line[0] = BOXDRAW_VERTICAL;
	//line[1] = BOXDRAW_HORIZONTAL;
	line[gMaxCols - vertDivider-1] = L'\0';
	DrawString(col, horzDivider, line );

	FlushLines( 0, end-1 );

	gBS->FreePool( line );
	
}
/**
    Function to get frame initialization data

    @param page UINT32 frame

    @retval VOID* 

**/
VOID *OverRideStyleGetFrameInitData( UINT32 page, UINT32 frame )
{
	return &_gStyleSubFrames[frame];
}
/**
    Function to add frame title 

    @param frame UINT32 frameType, CONTROL_INFO * dummy

    @retval EFI_STATUS

**/
EFI_STATUS StyleFrameAddTitle( FRAME_DATA *frame, UINT32 frameType,CONTROL_INFO * dummy )
{
	return EFI_UNSUPPORTED;
}

#endif /* SETUP_STYLE_EZPORT */

/**
    Function to handle ESC Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandleEscKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
extern BOOLEAN gTseCacheUpdated;
VOID HandleEscKey( VOID *vapp, VOID  *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	UINT16 pageClass = 0, pageSubclass = 0, pageFormID = 0;
	EFI_GUID ExitPageGuid;
	UINT8 	Sel = 0;
	CHAR16 	*Title = NULL;
	CHAR16  *Text = NULL;

	if(FormBrowserHandleValid())
	{
#if TSE_FOR_EDKII_SUPPORT
		if (gTseCacheUpdated)
		{
		
			Title = HiiGetString (gHiiHandle, STRING_TOKEN (STR_WARNING));
			Text = HiiGetString (gHiiHandle, STRING_TOKEN (STR_TSE_CACHE_CHANGE_WARNING));
			PostManagerDisplayMsgBox (Title, Text, MSGBOX_TYPE_OK, &Sel);
			MemFreePointer ((VOID **)&Title);
			MemFreePointer ((VOID **)&Text);
		}
		else
#endif
		{
			app->Quit = TRUE;
		}
	}

	else if ( gotoExitOnEscKey (&ExitPageGuid, &pageClass, &pageSubclass, &pageFormID) )
	{
    	UINT32 i=0;
    	PAGE_INFO *pageInfo;

		for ( i = 0; i < gPages->PageCount; i++ )
		{
			pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
			if (
	         	( EfiCompareGuid(&ExitPageGuid,&(gPageIdInfo[pageInfo->PageIdIndex].PageGuid)) ) &&
					( pageClass == gPageIdInfo[pageInfo->PageIdIndex].PageClass ) 							&&
					( pageSubclass == gPageIdInfo[pageInfo->PageIdIndex].PageSubClass ) 					&&
					( pageFormID == pageInfo->PageFormID ) 
			   )
			{
				gApp->CurrentPage = i; // Page found go to exit page
	            gApp->CompleteRedraw = TRUE;
	            gApp->Fixed = FALSE;
				break;
			}
		}
		if(i >= gPages->PageCount) //Page not found exit application
			app->Quit = ExitApplication();
	}

	else
		app->Quit = ExitApplication();
}
/**
    Function to handle Save and Exit Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandleSaveExitKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandleSaveExitKey( VOID *vapp, VOID *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	app->Quit = SaveAndExit();
}
/**
    Function to handle Load Previous value Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandlePrevValuesKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandlePrevValuesKey( VOID *vapp, VOID *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	if ( app->CurrentPage != 0 )
		LoadPreviousValues( TRUE );
	gUserTseCacheUpdated = FALSE;	//Modifications reverted back to previous values, No need to keep the flag true
}
/**
    Function to handle Load Failsafe values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandleFailsafeKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandleFailsafeKey( VOID *vapp, VOID *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	if ( app->CurrentPage != 0 )
		LoadFailsafeDefaults();
}
/**
    Function to handle Load Optimal values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandleOptimalKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandleOptimalKey( VOID *vapp, VOID *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	if ( app->CurrentPage != 0 )
		LoadOptimalDefaults();
}
/**
    Function to handle Help Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
//VOID HandleHelpKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandleHelpKey( VOID *vapp, VOID *hotkey, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	if ( app->CurrentPage != 0 )
		ShowHelpMessageBox();
}
/**
    Function to handle print screen Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
//VOID HandlePrnScrnKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
VOID HandlePrnScrnKey( VOID  *app, VOID *hotkey, VOID *cookie )
{
	TseHotkeyPrintScreenSupport();
}

/**
    Function to handle debug infrastructure Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID SetupShowDebugScreenWrapper (VOID *app, VOID *hotkey, VOID *cookie );
VOID SetupShowDebugScreen( VOID  *app, VOID *hotkey, VOID *cookie )
{
	SetupShowDebugScreenWrapper (app, hotkey, cookie);
}


/**
    Function to handle Main Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
//VOID MainFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
VOID MainFrameHandlewithMouse( VOID *vapp, VOID *hotclick, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *)vapp;
	NumericSoftKbdExit();
	if(app->Action->Input.Data.MouseInfo.ButtonStatus == TSEMOUSE_RIGHT_CLICK)
		app->Quit = ExitApplication();
}

/**
    Function to handle Help Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
//VOID HelpFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
VOID HelpFrameHandlewithMouse( VOID *app, VOID *hotclick, VOID *cookie )
{
	NumericSoftKbdExit();
	
}

/**
    Function to handle Navigation Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
//VOID NavFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
VOID NavFrameHandlewithMouse( VOID *vapp, VOID *vhotclick, VOID *cookie )
{
	APPLICATION_DATA *app = (APPLICATION_DATA *) vapp;
	HOTCLICK_DATA *hotclick = ( HOTCLICK_DATA *)vhotclick;
	UINT32 Action;

	// Handling mouse hot click operations
	NumericSoftKbdExit();
	Action = MapControlMouseActionHook(&app->Action->Input.Data.MouseInfo);
	//When pop-up window is invoked in setup and Click on ESC icon or ESC/Right Click, it will respond as two times ESC is clicked
	if(( ControlActionChoose == Action )||( ControlActionSelect == Action ))// In Setup click\Touch from navigation window, option are not displayed properly
	{
		if(app->Action->Input.Data.MouseInfo.Top == hotclick->HotClickData.Yo + 4)
			if ( app->CurrentPage != 0 )
			{
				ShowHelpMessageBox();
				return;
			}
		
		if(app->Action->Input.Data.MouseInfo.Top == hotclick->HotClickData.Yo + 5)
			if ( app->CurrentPage != 0 )
			{	
				LoadPreviousValues( TRUE );
				gUserTseCacheUpdated = FALSE;	//Modifications reverted back to previous values, No need to keep the flag true
				return;
			}
		if(app->Action->Input.Data.MouseInfo.Top == hotclick->HotClickData.Yo + 6)
			if ( app->CurrentPage != 0 )
			{	
				LoadOptimalDefaults();
				return;
			}
		if(app->Action->Input.Data.MouseInfo.Top == hotclick->HotClickData.Yo + 7)
			{
				app->Quit = SaveAndExit();
				return;
			}
		
		if(app->Action->Input.Data.MouseInfo.Top == hotclick->HotClickData.Yo + 8)		
			HotclickESCaction();	
		//app->Quit = ExitApplication();	
	
	}	
}
/**
    Function to recalculate the frame datas based on the new gMaxRows and gMaxCols if updated.
 
    @param FRAME_INFO *StyleFullSubFrames

    @retval EF_STATUS

**/
EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames)
{
    if (( gMaxRows != 31 && gMaxRows != 25  )||(gMaxCols != 100 && gMaxCols != 80 ))
	{		
		//
		//Update below data based on TextMode used
		//
		StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Width 	= (UINT16)(gMaxCols);
		StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Height 	= TITLE_FULL_H;
		StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Top 		= TITLE_FULL_Y;
		StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Left 		= TITLE_FULL_X;

		StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Width 	= (UINT16)(gMaxCols);
		StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Height 	= MENU_FULL_H;
		StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Top 		= MENU_FULL_Y;
		StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Left 		= MENU_FULL_X;		

		StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Width 	= (UINT16)((gMaxCols*2/3)-2);
		StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Height 	= (UINT16)((gMaxRows-3)-2); 
		StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Top 		= MAIN_FULL_Y;
		StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Left 		= MAIN_FULL_X;

		StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Width 	= (UINT16)(gMaxCols-(gMaxCols*2/3)-2);
		StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Height 	= (UINT16)(gMaxRows/2)-4 ;
		StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Top 		=  HELP_FULL_Y;
		StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Left 		= (UINT16)((gMaxCols*2/3)+1);		

		StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Width 	= (UINT16)(gMaxCols-(gMaxCols*2/3)-2);
		StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Height 	= (UINT16)((gMaxRows-3)-(gMaxRows/2));
		StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Top 		= (UINT16)((gMaxRows/2)+1);
		StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Left 		= (UINT16)((gMaxCols*2/3)+1);		

		StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Width 	= (UINT16)(gMaxCols);
		StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Height 	= HELPTITLE_FULL_H ;
		StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Top 		= (UINT16)(2 + (gMaxRows-3));
		StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Left 		= HELPTITLE_FULL_X ;
			
		return EFI_SUCCESS;
	}
	return EFI_UNSUPPORTED;       //Return EFI_SUCCESS to reflect the changes of StyleFullSubFrames in TSE. otherwise return EFI_UNSUPPORTED
}

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
