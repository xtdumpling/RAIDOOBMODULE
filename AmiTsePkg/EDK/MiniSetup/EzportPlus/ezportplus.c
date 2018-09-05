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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/EzportPlus/ezportplus.c $
//
// $Author: Premkumara $
//
// $Revision: 10 $
//
// $Date: 1/18/12 7:37a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file ezportplus.c
    Main file for ezportplus style module.

**/
//**********************************************************************
#include "minisetup.h"
#include "ezportplus.h"

extern	STYLECOLORS  Colors ;
extern 	AMI_IFR_MENU gMenuData ;
extern UINTN gInvalidPasswordFailMsgBox;

extern UINTN gLabelLeftMargin ;
extern UINTN gControlLeftMargin ;
extern UINTN gControlLeftPad ;
extern UINTN gControlRightAreaWidth ;
extern BOOLEAN IsPasswordSupportNonCaseSensitive();

static CALLBACK_SUBMENU gMenuCallbackData = { { CALLBACK_TYPE_MENU, sizeof(CALLBACK_MENU) }, 0 };
static CALLBACK_SUBMENU gSubmenuCallbackData = { { CALLBACK_TYPE_SUBMENU, sizeof(CALLBACK_SUBMENU) }, 0 };
static CALLBACK_VARIABLE gVariableCallbackData = { { CALLBACK_TYPE_VARIABLE, sizeof(CALLBACK_VARIABLE) }, 0, 0, 0, NULL };
static CALLBACK_PASSWORD gPasswordCallbackData = { { CALLBACK_TYPE_PASSWORD, sizeof(CALLBACK_PASSWORD) }, 0, 0, 0, NULL, TRUE };

VOID _FrameSubmenuCallback( FRAME_DATA *frame, SUBMENU_DATA *submenu, VOID *cookie );
VOID _FrameMenuCallback( FRAME_DATA *frame, MENU_DATA *menu, VOID *cookie );
EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames);
BOOLEAN   gotoExitOnEscKey(EFI_GUID *exitFormGuid, UINT16 *pageClass, UINT16 *pageSubclass, UINT16 *pageFormID);
#if SETUP_STYLE_EZPORTPLUS

#if SETUP_STYLE_FULL_SCREEN
static FRAME_INFO _gStyleFullSubFrames[] =
{
//	{ FrameType		Drawable	Border,		BorderType,	Width,			Height,			Top,			Left,			FGColor,		BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE,		0,		TITLE_FULL_W,		TITLE_FULL_H,		TITLE_FULL_Y,		TITLE_FULL_X,		TITLE_FGCOLOR,		TITLE_BGCOLOR		},// TITLE
	{ SUBTITLE_FRAME,	TRUE,		FALSE,		0,		MENU_FULL_W,		MENU_FULL_H,		MENU_FULL_Y,		MENU_FULL_X,		MENU_FGCOLOR,		MENU_BGCOLOR		},// SUBTITLE (MENU)
    { HELPTITLE_FRAME,	TRUE,		FALSE,		0,		HELPTITLE_FULL_W,	HELPTITLE_FULL_H,	HELPTITLE_FULL_Y,	HELPTITLE_FULL_X,	HELPTITLE_FGCOLOR,	HELPTITLE_BGCOLOR	},// HELP TITLE (Copyright)	
    { HELP_FRAME,		TRUE,		FALSE,		0,		HELP_FULL_W,		HELP_FULL_H,		HELP_FULL_Y,		HELP_FULL_X,		HELP_FGCOLOR,		HELP_BGCOLOR		},// HELP    
    { MAIN_FRAME,		TRUE,		FALSE,		0,		MAIN_FULL_W,		MAIN_FULL_H,		MAIN_FULL_Y,		MAIN_FULL_X,		PAGE_FGCOLOR,		MAIN_BGCOLOR		},// Main
    { NAV_FRAME,		TRUE,		FALSE,		0,		NAV_FULL_W,		    NAV_FULL_H,		    NAV_FULL_Y,		    NAV_FULL_X,		    NAV_FGCOLOR,	    NAV_BGCOLOR		    }// NAVIGATION
	
};
#endif

static FRAME_INFO _gStyleStandardSubFrames[] =
{
//	{ FrameType		Drawable	Border,		BorderType,	Width,		Height,		Top,		Left,		FGColor,		BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE,		0,		TITLE_W,	TITLE_H,	TITLE_Y,	TITLE_X,	TITLE_FGCOLOR,		TITLE_BGCOLOR		},// TITLE
	{ SUBTITLE_FRAME,	TRUE,		FALSE,		0,		MENU_W,		MENU_H,		MENU_Y,		MENU_X,		MENU_FGCOLOR,		MENU_BGCOLOR		},// SUBTITLE (MENU)
    { HELPTITLE_FRAME,	TRUE,		FALSE,		0,		HELPTITLE_W,HELPTITLE_H,HELPTITLE_Y,HELPTITLE_X,HELPTITLE_FGCOLOR,	HELPTITLE_BGCOLOR	},// HELP TITLE (Copyright)	
    { HELP_FRAME,		TRUE,		FALSE,		0,		HELP_W,		HELP_H,		HELP_Y,		HELP_X,		HELP_FGCOLOR,		HELP_BGCOLOR		},// HELP
    { MAIN_FRAME,		TRUE,		FALSE,		0,		MAIN_W,		MAIN_H,		MAIN_Y,		MAIN_X,		PAGE_FGCOLOR,		MAIN_BGCOLOR		},// Main
	{ NAV_FRAME,		TRUE,		FALSE,		0,		NAV_W,		NAV_H,		NAV_Y,		NAV_X,		NAV_FGCOLOR,		NAV_BGCOLOR		    }// NAVIGATION
	
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
        _gStyleSubFrames    = _gStyleFullSubFrames;
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
	FRAME_DATA *fTitle, *fMenu, *fHelptitle ;

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
        horzDivider = FULL_HORIZONTAL_HELP_DIVIDER;
        pbx = PAGEBORDER_X;
        pby = PAGEBORDER_Y;
        pbw = (UINT16)gMaxCols;
        pbh = (UINT16)gMaxRows-4;
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
	ClearLinesWithoutFlush( pby, pbh, PAGE_BGCOLOR | PAGE_FGCOLOR );
	
	
	DrawWindow( pbx, pby, pbw, pbh, PAGE_BGCOLOR| PAGE_FGCOLOR, TRUE, FALSE );

    // Hidden line in first page
    if (Page->PageData.PageID == FIRST_PAGE_ID_INDEX)
    {
        gBS->FreePool( line );
        return;
    }

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

#endif /* SETUP_STYLE_EZPORTPLUS */

/**
    Function to handle ESC Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
extern BOOLEAN gTseCacheUpdated;
VOID EzportPlusHandleEscKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
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
VOID EzportPlusHandleSaveExitKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	app->Quit = SaveAndExit();
}
/**
    Function to handle Load Previous value Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID EzportPlusHandlePrevValuesKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	if ( app->CurrentPage != 0 )
		LoadPreviousValues( TRUE );
	gUserTseCacheUpdated = FALSE;	//Modifications reverted back to previous values, No need to keep the flag true
}
/**
    Function to handle Load Failsafe values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID EzportPlusHandleFailsafeKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	if ( app->CurrentPage != 0 )
		LoadFailsafeDefaults();
}
/**
    Function to handle Load Optimal values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID EzportPlusHandleOptimalKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	if ( app->CurrentPage != 0 )
		LoadOptimalDefaults();
}
/**
    Function to handle Help Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID EzportPlusHandleHelpKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	if ( app->CurrentPage != 0 )
		ShowHelpMessageBox();
}
/**
    Function to handle print screen Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID EzportPlusHandlePrnScrnKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
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
VOID EzportPlusMainFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{
	if(app->Action->Input.Data.MouseInfo.ButtonStatus == TSEMOUSE_RIGHT_CLICK)
		app->Quit = ExitApplication();
}

/**
    Function to handle Help Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID EzportPlusHelpFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{

	
}

/**
    Function to handle Navigation Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID EzportPlusNavFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{
}




#if OVERRIDE_PopupPwdHandleActionOverRide
extern VOID UpdatePasswordToNonCaseSensitive(CHAR16 *Password, UINTN PwdLength);
extern UINT8 CallbackShowMessageBox( UINTN data, UINT8 type );
extern UINTN gClearPasswordMsgBox ;

EFI_STATUS _EzportPlusPopupPasswordHandleAction( VOID *popuppassword, VOID *Data);
EFI_STATUS 	PopupPwdHandleActionOverRide(POPUP_PASSWORD_DATA *popuppassword, ACTION_DATA *Data)
{
	return _EzportPlusPopupPasswordHandleAction(popuppassword, Data);
}
/**
    Function to Display the password failed message box

    @param VOID

    @retval VOID

**/
VOID EzportPlusShowPasswordErrorMessageBox( VOID )
{
	UINT8 sel = 0 ;
	AMI_POST_MGR_KEY OutKey ;
	CHAR16	*Title = NULL,*Message = NULL;
	
	Title = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ERROR));
	Message = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ERROR_PSWD));

    PostManagerDisplayMsgBoxEx(
		Title, 
		Message, 
		NULL, MSGBOX_EX_CATAGORY_WARNING, MSGBOX_TYPE_OK,
		NULL, 0, NULL, 0, &sel, &OutKey
	) ;
 	MemFreePointer( (VOID **)&Title );
 	MemFreePointer( (VOID **)&Message );
	TSEStringReadLoopExitHook();
}
/**
    Function to Display the password success message box

    @param VOID

    @retval VOID

**/
VOID EzportPlusShowPasswordSuccessMessageBox( VOID )
{
	UINT8 sel = 0 ;
	AMI_POST_MGR_KEY OutKey ;
	CHAR16	*Title = NULL,*Message = NULL;
	
	Title = HiiGetString( gHiiHandle, STRING_TOKEN(STR_SUCCESS));
	Message = HiiGetString( gHiiHandle, STRING_TOKEN(STR_PASSWORD_SUCCESS_MSG));
    PostManagerDisplayMsgBoxEx(
		Title, 
		Message, 
		NULL, MSGBOX_EX_CATAGORY_SUCCESS, MSGBOX_TYPE_OK,
		NULL, 0, NULL, 0, &sel, &OutKey
	) ;
 	MemFreePointer( (VOID **)&Title );
 	MemFreePointer( (VOID **)&Message );
	TSEStringReadLoopExitHook();
}
/**
    Function to Display the password success message box

    @param VOID

    @retval VOID

**/
VOID EzportPlusShowPasswordClearedMessageBox( VOID )
{
	UINT8 sel = 0 ;
	AMI_POST_MGR_KEY OutKey ;
	CHAR16	*Title = NULL,*Message = NULL;
	
	 Title = HiiGetString( gHiiHandle, STRING_TOKEN(STR_INFO));
	 Message = HiiGetString( gHiiHandle, STRING_TOKEN(STR_PASSWORD_CLEARED_MSG));
	PostManagerDisplayMsgBoxEx(
		Title, 
		Message, 
		NULL, MSGBOX_EX_CATAGORY_QUERY, MSGBOX_TYPE_OK,
		NULL, 0, NULL, 0, &sel, &OutKey
	) ;
 	MemFreePointer( (VOID **)&Title );
 	MemFreePointer( (VOID **)&Message );
	TSEStringReadLoopExitHook();
}
/**
    Function to display password prompts for new, and confirm in the same window.

    @param PopupPassword 
    @param NewPrompt 
    @param NewPwd 
    @param ConfirmPrompt 
    @param ConfirmPwd 

    @retval Status

**/
EFI_STATUS EzportPlusPopupPasswordSetPrompt(POPUP_PASSWORD_DATA *PopupPassword, UINT16 NewPrompt, CHAR16 **NewPwd, UINT16 ConfirmPrompt, CHAR16 **ConfirmPwd)
{
    EFI_STATUS Status = EFI_SUCCESS ;

    UINT8 Length = (UINT8)UefiGetMaxValue(PopupPassword->ControlData.ControlPtr);

    TEXT_INPUT_TEMPLATE SetPasswordPrompt[2] = 
    {
        {NewPrompt, Length, TSE_FILTER_KEY_NULL, TRUE, NULL},
        {ConfirmPrompt, Length, TSE_FILTER_KEY_NULL, TRUE, NULL}
    };

    UINT16 Title = STRING_TOKEN(STR_SET_PASSWORD_PROMPT) ;

    *NewPwd = EfiLibAllocateZeroPool((Length+1) * sizeof(CHAR16) );
    *ConfirmPwd = EfiLibAllocateZeroPool((Length+1) * sizeof(CHAR16) );

    SetPasswordPrompt[0].Value = *NewPwd ;
    SetPasswordPrompt[1].Value = *ConfirmPwd ;
    
    
    if ( *NewPwd == NULL || *ConfirmPwd == NULL)
        return EFI_OUT_OF_RESOURCES ;
	
	TSEStringReadLoopEntryHook();

    Status = PostManagerDisplayTextBox(
			gHiiHandle,
            Title,
            SetPasswordPrompt, 
            2,
            NULL
			) ;	
	
	return Status;
}
/**
    Function to display password prompts for old, new, and confirm in the same window.

    @param PopupPassword 
    @param OldPrompt 
    @param OldPwd 
    @param NewPrompt 
    @param NewPwd 
    @param ConfirmPrompt 
    @param ConfirmPwd 

    @retval Status

**/
EFI_STATUS EzportPlusPopupPasswordChangePrompt(POPUP_PASSWORD_DATA *PopupPassword, UINT16 OldPrompt, CHAR16 **OldPwd, UINT16 NewPrompt, CHAR16 **NewPwd, UINT16 ConfirmPrompt, CHAR16 **ConfirmPwd)
{

    EFI_STATUS Status = EFI_SUCCESS ;

    UINT8 Length = (UINT8)UefiGetMaxValue(PopupPassword->ControlData.ControlPtr);

    TEXT_INPUT_TEMPLATE ChangePasswordPrompt[3] = 
    {
        {OldPrompt, Length, TSE_FILTER_KEY_NULL, TRUE, NULL},
        {NewPrompt, Length, TSE_FILTER_KEY_NULL, TRUE, NULL},
        {ConfirmPrompt, Length, TSE_FILTER_KEY_NULL, TRUE, NULL}
    };

    UINT16 Title = STRING_TOKEN(STR_CHANGE_PASSWORD_PROMPT) ;

    *OldPwd = EfiLibAllocateZeroPool((Length+1) * sizeof(CHAR16) );
    *NewPwd = EfiLibAllocateZeroPool((Length+1) * sizeof(CHAR16) );
    *ConfirmPwd = EfiLibAllocateZeroPool((Length+1) * sizeof(CHAR16) );

    ChangePasswordPrompt[0].Value = *OldPwd ;
    ChangePasswordPrompt[1].Value = *NewPwd ;
    ChangePasswordPrompt[2].Value = *ConfirmPwd ;
    
    
    if ( *OldPwd == NULL || *NewPwd == NULL || *ConfirmPwd == NULL)
    {
    	if(*OldPwd)
    		MemFreePointer((VOID**)OldPwd);
    	if(*NewPwd)
    		MemFreePointer((VOID**)NewPwd);
    	if(*ConfirmPwd)
    		MemFreePointer((VOID**)ConfirmPwd);

        return EFI_OUT_OF_RESOURCES ;
    }
	TSEStringReadLoopEntryHook();

    Status = PostManagerDisplayTextBox(
			gHiiHandle,
            Title,
            ChangePasswordPrompt, 
            3,
            NULL
			) ;	
	
	return Status;
}

BOOLEAN IsPasswordSupportNonCaseSensitive();
/**
    function to set and activate a password

    @param popuppassword 

    @retval status

**/
extern VOID SavePswdString (CONTROL_INFO *ControlData, CHAR16 *String);
extern EFI_STATUS HandleESCOnInteractivePassword(POPUP_PASSWORD_DATA *popuppassword,CHAR16 *Text );
EFI_STATUS _EzportPlusPopupPasswordActivate(POPUP_PASSWORD_DATA *popuppassword)
{
   	EFI_STATUS Status = EFI_UNSUPPORTED;
	BOOLEAN AbortUpdate = FALSE, ChangePwd = FALSE, ClearPwd = FALSE ;
    CHAR16 /* *TempText=NULL,*/ *Text=NULL,*NewPswd=NULL,*ConfirmNewPswd=NULL;
    VOID *data = popuppassword->ControlData.ControlPtr;
	UINTN NewPwLen = 0, ConfirmPwLen = 0,OldPwLen = 0;
	CHAR16 *TempEncoded=NULL;
	UINT16 Key = UefiGetControlKey(&(popuppassword->ControlData));
	if( UefiIsInteractive(&popuppassword->ControlData))
	    {
	    	SavePswdString(&popuppassword->ControlData, NULL);// kept text as NULL, as we are checking for preexisitng password present or not
	    	Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE); // need to pass NULL as we are checking for pre existing password  
	    	if(Status == EFI_SUCCESS)
		{
			// no pre existing password is present
			ChangePwd = FALSE;
			AbortUpdate = FALSE;
		
			goto Password;
			
		}   
	    	else if(Status != EFI_SUCCESS)
	    	{
	    		ChangePwd = TRUE ;
	    		Status = EzportPlusPopupPasswordChangePrompt(popuppassword, STRING_TOKEN(STR_OLD_PSWD), &Text, STRING_TOKEN(STR_NEW_PSWD), &NewPswd,  STRING_TOKEN(STR_CONFIRM_NEW_PSWD), &ConfirmNewPswd) ;
	    	  
				if(Status == EFI_SUCCESS)
				{
					SavePswdString(&popuppassword->ControlData, Text);
					OldPwLen = EfiStrLen(Text);
					if(OldPwLen){
						TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
						if ( TempEncoded == NULL )
							return EFI_NOT_READY;
						
						EfiStrCpy(TempEncoded,Text);
						
						if( IsPasswordEncodeEnabled( &popuppassword->ControlData )){
								PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
							}
						SavePswdString(&popuppassword->ControlData, TempEncoded);
						MemFreePointer( (VOID **)&TempEncoded );
					}
					Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE); // need to pass the old password to get it authenticated
					if(Status == EFI_NOT_READY)
					{
						// wrong old password
						// show error alert message box and exit
						// return;
						EzportPlusShowPasswordErrorMessageBox();
						AbortUpdate = TRUE;
						goto Done;
					}
					else if(Status == EFI_SUCCESS)
					{
						ChangePwd = TRUE;
						AbortUpdate = FALSE;
					}
					else
					{
						AbortUpdate = TRUE;
						EzportPlusShowPasswordErrorMessageBox();
						goto Done;
						//CallbackShowMessageBox( (UINTN)gCannotChangePasswordMsgBox, MSGBOX_TYPE_OK );
					}
				}
	        		
	    	}  
	    }
	else if( PopupPwdAuthenticateIDEPwd(popuppassword,&AbortUpdate,data) != EFI_SUCCESS)
	{

		AbortUpdate = FALSE;

		if(PopupPasswordCheckInstalled(popuppassword))
		{

            ChangePwd = TRUE ;
            Status = EzportPlusPopupPasswordChangePrompt(popuppassword, STRING_TOKEN(STR_OLD_PSWD), &Text, STRING_TOKEN(STR_NEW_PSWD), &NewPswd,  STRING_TOKEN(STR_CONFIRM_NEW_PSWD), &ConfirmNewPswd) ;   

			if( Status )
				AbortUpdate = TRUE;
			else
			{
				if(!PopupPasswordAuthenticate( popuppassword, Text ))
                {   
					// optional message to user: "wrong password" and  exit                   
					//CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
                    EzportPlusShowPasswordErrorMessageBox();
					AbortUpdate = TRUE;
				}
			}
		}
	}
#if OVERRIDE_PopupPwdAuthenticateIDEPwd && SETUP_IDE_SECURITY_SUPPORT     

    else if( !UefiIsInteractive(&popuppassword->ControlData))
    {
        BOOLEAN bCheckUser = FALSE;
        BOOLEAN EnabledBit = FALSE;
        UINTN size = 0;
        IDE_SECURITY_CONFIG *ideSecConfig;
        VOID* DataPtr = TSEIDEPasswordGetDataPtr(gCurrIDESecPage);
        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
        
        if (NULL != ideSecConfig) {
                
            if(ideSecConfig->Enabled) EnabledBit = TRUE;
    
            if(EnabledBit)
            {        
                if(UefiGetQuestionOffset(data) /*data->QuestionId*/ == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword))
                    bCheckUser = TRUE;
        
                if(bCheckUser || ideSecConfig->MasterPasswordStatus)
                {
                   //Status = _DoPopupEdit( *popuppassword, STRING_TOKEN(STR_OLD_PSWD), &Text);
                    ChangePwd = TRUE ;
                    Status = EzportPlusPopupPasswordChangePrompt(popuppassword, STRING_TOKEN(STR_OLD_PSWD), &Text, STRING_TOKEN(STR_NEW_PSWD), &NewPswd,  STRING_TOKEN(STR_CONFIRM_NEW_PSWD), &ConfirmNewPswd) ;
                
                    if(Status )
                    {
                        AbortUpdate = TRUE;
                    }
        			else
        			{
                        Status = TSEIDEPasswordAuthenticate( Text, DataPtr, bCheckUser );
                        if(EFI_ERROR( Status ))
            			{
            				//CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
                            EzportPlusShowPasswordErrorMessageBox();
            				AbortUpdate = TRUE;
            			}
                        else
                        {
                            NewPwLen = EfiStrLen(NewPswd);
                            ConfirmPwLen = EfiStrLen(ConfirmNewPswd);
                           
    				        if(NewPwLen == 0 && ConfirmPwLen == 0) 
    			        	{
    					        ClearPwd = TRUE ; 
    					    }
                        }
        			}
                }
            }
            
            MemFreePointer((VOID **) &ideSecConfig);
        }
    }
#endif

    if(AbortUpdate == FALSE)
	{
Password:

        if(ChangePwd == FALSE)
        {
            Status = EzportPlusPopupPasswordSetPrompt(popuppassword, STRING_TOKEN(STR_NEW_PSWD), &NewPswd,  STRING_TOKEN(STR_CONFIRM_NEW_PSWD), &ConfirmNewPswd) ; 
            if(Status) //If ESC pressed
            {
        	    if(UefiIsInteractive(&popuppassword->ControlData)){//if only interactive we will send old password to save again
        		    HandleESCOnInteractivePassword(popuppassword, Text );
			
		}
        	goto Done;
            }
            NewPwLen = EfiStrLen(NewPswd);
            if(!Status &&  ( (NewPwLen < (UINT8)UefiGetMinValue(popuppassword->ControlData.ControlPtr)) ||
    				(NewPwLen > (UINT8)UefiGetMaxValue(popuppassword->ControlData.ControlPtr))
    				))
            {             
                //Report Message box 
                EzportPlusShowPasswordErrorMessageBox(); 
                Status = EFI_UNSUPPORTED;    
            }
        }
		
        if(!Status)
        {
			NewPwLen = EfiStrLen(NewPswd);
			ConfirmPwLen = EfiStrLen(ConfirmNewPswd);    
			//non case sensitive support during setting password
			if (
							    (IsPasswordSupportNonCaseSensitive ()) && 
							    (NewPwLen != 0) &&
							    ( (VARIABLE_ID_AMITSESETUP == popuppassword->ControlData.ControlVariable) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) )
			) {
				UpdatePasswordToNonCaseSensitive(NewPswd, NewPwLen);
			}

   			if (((VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) && (ChangePwd == TRUE)) || PopupPasswordCheckInstalled(popuppassword))
			{
    			
    				 // Override the minmax validation for password clearing
				if(NewPwLen == 0 && ConfirmPwLen == 0 && (ChangePwd == TRUE)) 
    				{
    					//Report Message box for Clearing Old password
    					if(CallbackShowMessageBox( (UINTN)gClearPasswordMsgBox, MSGBOX_TYPE_YESNO )!= MSGBOX_YES)
    					{
    						ChangePwd = FALSE ;
    						ClearPwd = FALSE ; 
    						goto Password;
    						//Status = EFI_UNSUPPORTED; //Do not clear the password
    					} 
    					else 
    					{
    						ClearPwd = TRUE ; 
    					}
    				}
    			
            }else if( (NewPwLen < (UINT8)UefiGetMinValue(popuppassword->ControlData.ControlPtr)) ||
				(NewPwLen > (UINT8)UefiGetMaxValue(popuppassword->ControlData.ControlPtr))
				)
            {
                //Report Message box
                EzportPlusShowPasswordErrorMessageBox();

                Status = EFI_UNSUPPORTED;
            }
            
            if( ClearPwd == TRUE)
            {
                if(ConfirmNewPswd)
                    StringZeroFreeMemory ((VOID **)&ConfirmNewPswd);

                ConfirmNewPswd = EfiLibAllocateZeroPool( sizeof(CHAR16) );
                    						
    			if(ConfirmNewPswd) 
    			{
    				*ConfirmNewPswd = '\0'; //Set confirmation password to null string
    			} 
    			else 
    			{
    				Status = EFI_OUT_OF_RESOURCES;
    			}

            }
        }

		if ( !Status )
		{
			//non case sensitive support during setting password
			if( IsPasswordSupportNonCaseSensitive() && EfiStrLen(ConfirmNewPswd) 
					&&
			 ( (VARIABLE_ID_AMITSESETUP == popuppassword->ControlData.ControlVariable) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable))
			 ){
				NewPwLen = EfiStrLen(ConfirmNewPswd);
				UpdatePasswordToNonCaseSensitive( ConfirmNewPswd, NewPwLen );
			}
		    if ( EfiStrCmp(NewPswd, ConfirmNewPswd)==0 ) 
		    {
				//Support for Interactive Password control
				if(popuppassword->Text != NULL){
					MemFreePointer( (VOID **)&popuppassword->Text );
				}
				popuppassword->Text = (CHAR16 *) EfiLibAllocateZeroPool((NewPwLen + 1) * sizeof(CHAR16));
				EfiStrCpy(popuppassword->Text, NewPswd);

				//save new password
                if ( ( popuppassword->Callback != NULL ) && ( popuppassword->Cookie != NULL ) )
				{
                    VOID *ifrData = popuppassword->ControlData.ControlPtr;
					CALLBACK_PASSWORD *callbackData = (CALLBACK_PASSWORD *)popuppassword->Cookie;

					callbackData->Variable = popuppassword->ControlData.ControlVariable;
					callbackData->Offset = UefiGetQuestionOffset(ifrData);
					callbackData->Length = (UINT8)UefiGetMaxValue(ifrData) * sizeof(CHAR16);
					callbackData->Data = (VOID *)NewPswd;
					UefiPreControlUpdate(&(popuppassword->ControlData));
					popuppassword->Callback( popuppassword->Container,popuppassword, popuppassword->Cookie );

					PopupPwdUpdateIDEPwd ();
				}


				//For interactive control invoke the formcallback function.
				if(UefiIsInteractive(&popuppassword->ControlData))
				{
					VOID *Handle = popuppassword->ControlData.ControlHandle;
					
					CONTROL_DATA *Control = (CONTROL_DATA *)NULL;
					if(NewPwLen){
						TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
						if ( TempEncoded == NULL )
							return EFI_NOT_READY;
					
						EfiStrCpy(TempEncoded,NewPswd);
						
						SavePswdString(&(popuppassword->ControlData),TempEncoded);
						MemFreePointer( (VOID **)&TempEncoded );
					}
					//Call the callback passing the input value and value size.
					Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE);

					Control = GetUpdatedControlData((CONTROL_DATA*)popuppassword, CONTROL_TYPE_PASSWORD, Handle, Key);
					if(Control == NULL){ //Control deleted.
						return EFI_SUCCESS;
					}
					if(Control != (CONTROL_DATA *)popuppassword){
						popuppassword = (POPUP_PASSWORD_DATA *)Control; //Control Updated.
					}
				}

				popuppassword->ControlActive = FALSE;
                
                if(ClearPwd)
                {
                    EzportPlusShowPasswordClearedMessageBox();
                }
                else if(NewPwLen != 0)
                {
                    EzportPlusShowPasswordSuccessMessageBox();
                }
            }
			else
            {
			    EzportPlusShowPasswordErrorMessageBox();
			    if(UefiIsInteractive(&popuppassword->ControlData)){//if only interactive we will send old password to save again
				    HandleESCOnInteractivePassword(popuppassword, Text );
				
			}
            }
	    }
	}

   Done:
	StringZeroFreeMemory ((VOID **)&Text);		
	StringZeroFreeMemory ((VOID **)&NewPswd);
	StringZeroFreeMemory ((VOID **)&ConfirmNewPswd);

	TSEStringReadLoopExitHook();
    return EFI_SUCCESS;
}

/**
    function to handle the password actions

    @param popuppassword , ACTION_DATA *Data

    @retval status

**/
EFI_STATUS _EzportPlusPopupPasswordHandleAction( POPUP_PASSWORD_DATA *popuppassword, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	   return Status;
	

	if ( Data->Input.Type == ACTION_TYPE_KEY )
    {
        CONTROL_ACTION Action;

        //Get mapping
        Action = MapControlKeysHook(Data->Input.Data.AmiKey);

        if(ControlActionSelect == Action)
        {
			if(UefiIsInteractive(&popuppassword->ControlData))
				if(PopupPasswordFormCallback(&(popuppassword->ControlData),UefiGetControlKey(&(popuppassword->ControlData)),0) == EFI_SUCCESS)
					return EFI_SUCCESS;

            return _EzportPlusPopupPasswordActivate(popuppassword);
        }
    }

    if(Data->Input.Type == ACTION_TYPE_MOUSE)
	{
		if(ControlActionSelect == MapControlMouseActionHook(&Data->Input.Data.MouseInfo))
		{
			if( popuppassword->Top == (Data->Input.Data.MouseInfo.Top) )
	        {
				if(UefiIsInteractive(&popuppassword->ControlData))
					if(PopupPasswordFormCallback(&(popuppassword->ControlData),UefiGetControlKey(&(popuppassword->ControlData)),0) == EFI_SUCCESS)
						return EFI_SUCCESS;

	                return _EzportPlusPopupPasswordActivate(popuppassword);
	        }
		}
	}
	return Status;
}
#endif


#if OVERRIDE_PopupPwdAuthenticateIDEPwd

EFI_STATUS PopupPwdAuthenticateIDEPwd(POPUP_PASSWORD_DATA *popuppassword, BOOLEAN *AbortUpdate,VOID *data)
{
#if SETUP_IDE_SECURITY_SUPPORT
	EFI_STATUS Status = EFI_UNSUPPORTED;
    CHAR16 *Text=NULL;
	if(popuppassword->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY ) 
	{
		//if drive is locked ask for the old password to unlock the drive
		VOID* DataPtr = TSEIDEPasswordGetDataPtr(gCurrIDESecPage);
        BOOLEAN bCheckUser = FALSE;
        BOOLEAN EnabledBit = FALSE;
        UINTN size = 0;
        IDE_SECURITY_CONFIG *ideSecConfig;

        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
        
        if (NULL == ideSecConfig) 
            return EFI_NOT_FOUND;
        
        if(ideSecConfig->Enabled) EnabledBit = TRUE;

        if(EnabledBit)
        {
             return EFI_SUCCESS;
 		}

        MemFreePointer((VOID **) &ideSecConfig);
		return EFI_SUCCESS;
	}
	return EFI_UNSUPPORTED;
#else
    return EFI_UNSUPPORTED;
#endif
}
#endif

/**
    function to recalculate the frame details based on new gMaxRows andgMaxCols if updated

    @param FRAME_INFO *StyleFullSubFrames

    @retval EFI_STATUS

**/
EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames)
{
    if (( gMaxRows != 31 && gMaxRows != 25  )||(gMaxCols != 100 && gMaxCols != 80 ))
    {     
        //
        //Update below data based on TextMode used
        //
        StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Height    = TITLE_FULL_H;
        StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Top       = TITLE_FULL_Y;
        StyleFullSubFrames[StyleFrameIndexOf(TITLE_FRAME)].Left      = TITLE_FULL_X;

        StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Height    = MENU_FULL_H;
        StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Top       = MENU_FULL_Y;
        StyleFullSubFrames[StyleFrameIndexOf(SUBTITLE_FRAME)].Left      = MENU_FULL_X;      
            
        StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Width     = (UINT16)((gMaxCols*1/3)-2);
        StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Height    = HELPTITLE_FULL_H; 
        StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Top       = HELPTITLE_FULL_Y;
        StyleFullSubFrames[StyleFrameIndexOf(HELPTITLE_FRAME)].Left      = (UINT16)((gMaxCols*2/3)+1);
            
        StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Width     = (UINT16)((gMaxCols*1/3)-2);
        StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Height    = (UINT16)(gMaxRows-8) ;
        StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Top       =  HELP_FULL_Y;
        StyleFullSubFrames[StyleFrameIndexOf(HELP_FRAME)].Left      = (UINT16)((gMaxCols*2/3)+1);       
        
        StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Width     = (UINT16)((gMaxCols*2/3)-2);
        StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Height    = (UINT16)((gMaxRows-6));
        StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Top       = MAIN_FULL_Y;
        StyleFullSubFrames[StyleFrameIndexOf(MAIN_FRAME)].Left      = MAIN_FULL_X;       
        
        StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Height    = NAV_FULL_H ;
        StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Top       = (UINT16)((gMaxRows-2));
        StyleFullSubFrames[StyleFrameIndexOf(NAV_FRAME)].Left      = NAV_FULL_X ;
            
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
