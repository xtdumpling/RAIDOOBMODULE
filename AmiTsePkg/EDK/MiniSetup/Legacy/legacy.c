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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Legacy/legacy.c $
//
// $Author: Rajashakerg $
//
// $Revision: 7 $
//
// $Date: 12/01/11 5:40a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file legacy.c
    Main file for alternate1 style module.

**/
//**********************************************************************
#include "minisetup.h"
#include "legacy.h"
#include <Protocol/AMIPostMgr.h>

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

EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames, FRAME_INFO * StyleFullMainFrames);

BOOLEAN   gotoExitOnEscKey(EFI_GUID *exitFormGuid, UINT16 *pageClass, UINT16 *pageSubclass, UINT16 *pageFormID);

#if SETUP_STYLE_LEGACY

static FRAME_INFO _gStyleLegacyMainFrames[] =
{
//	{FrameType		Drawable	Border, BorderType, Width, Height, Top, Left, FGColor, BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE, 0, 80, 1, 0, 0, TITLE_FGCOLOR, TITLE_BGCOLOR },			// Title
	{ MAIN_FRAME,		TRUE,		FALSE, 0, 78, 15, 2, 1, PAGE_FGCOLOR, PAGE_BGCOLOR },			// Main
	{ HELP_FRAME,		TRUE,		FALSE, 0, 76, 1, 21, 1, HELP_FGCOLOR, PAGE_BGCOLOR },			// Help text
	{ NAV_FRAME,		TRUE,		FALSE, 0, 76, 2, 18, 2, NAV_FGCOLOR, PAGE_BGCOLOR }			// Navigation
};

static FRAME_INFO _gStyleLegacySubFrames[] =
{
//	{ FrameType		Drawable	Border, BorderType, Width, Height, Top, Left, FGColor, BGColor }	
	
	{ TITLE_FRAME,		TRUE,		FALSE, 0, 80,  1,  0, 0,  TITLE_FGCOLOR, TITLE_BGCOLOR},		// Title
    	{ SUBTITLE_FRAME,	TRUE,		FALSE, 0, 80,  1,  1, 0,  MENU_FGCOLOR, MENU_BGCOLOR },		// Subtitle
	{ MAIN_FRAME,		TRUE,		FALSE, 0, 50, 19,  3, 1,  PAGE_FGCOLOR, PAGE_BGCOLOR },		// Main
	{ HELPTITLE_FRAME,	TRUE,		FALSE, 0, 23,  1,  3, 56, HELPTITLE_FGCOLOR, PAGE_BGCOLOR },	// Help title
	{ HELP_FRAME,		TRUE,		FALSE, 0, 23, 16,  5, 56, HELP_FGCOLOR, PAGE_BGCOLOR },		// Help text
	{ NAV_FRAME,		TRUE,		FALSE, 0, 80,  2, 22, 0,  NAV_FGCOLOR, PAGE_BGCOLOR },		// Navigation
};

#if SETUP_STYLE_FULL_SCREEN
static FRAME_INFO _gStyleLegacyFullMainFrames[] =
{
//	{FrameType			Drawable		Border, BorderType, Width, Height, Top, Left, FGColor, BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE, 0, 100, 1, 0, 0, TITLE_FGCOLOR, TITLE_BGCOLOR },			// Title
	{ MAIN_FRAME,		TRUE,		FALSE, 0, 98, 19, 2, 1, PAGE_FGCOLOR, PAGE_BGCOLOR },			// Main
	{ HELP_FRAME,		TRUE,		FALSE, 0, 95, 1, 28, 1, HELP_FGCOLOR, PAGE_BGCOLOR },			// Help text
	{ NAV_FRAME,		TRUE,		FALSE, 0, 95, 2, 25, 2, NAV_FGCOLOR, PAGE_BGCOLOR }			// Navigation
};

static FRAME_INFO _gStyleLegacyFullSubFrames[] =
{
//	{ FrameType		Drawable	Border, BorderType, Width, Height, Top, Left, FGColor, BGColor }	
	{ TITLE_FRAME,		TRUE,		FALSE, 0, 100,  1,  0,  0,  TITLE_FGCOLOR, TITLE_BGCOLOR},		// Title
    { SUBTITLE_FRAME,	TRUE,		FALSE, 0, 100,  1,  1,  0,  MENU_FGCOLOR, MENU_BGCOLOR },		// Subtitle
	{ MAIN_FRAME,		TRUE,		FALSE, 0,  63, 25,  3,  1,  PAGE_FGCOLOR, PAGE_BGCOLOR },		// Main
	{ HELPTITLE_FRAME,	TRUE,		FALSE, 0,  29,  1,  3, 70,  HELPTITLE_FGCOLOR, PAGE_BGCOLOR },	// Help title
	{ HELP_FRAME,		TRUE,		FALSE, 0,  29, 21,  5, 70,  HELP_FGCOLOR, PAGE_BGCOLOR },		// Help text
	{ NAV_FRAME,		TRUE,		FALSE, 0, 100,  2, 29,  0,  NAV_FGCOLOR, PAGE_BGCOLOR },		// Navigation
};
#endif

FRAME_INFO *_gStyleMainFrames = _gStyleLegacyMainFrames ;
FRAME_INFO *_gStyleSubFrames = _gStyleLegacySubFrames ;

INT16 gTabIndex = 0 ;

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



/**
    function to handle the page actions

    @param page ACTION_DATA *action

    @retval status

**/
EFI_STATUS LegacyPageHandleAction( PAGE_DATA *page, ACTION_DATA *action )
{
	UINT32	i;
    EFI_STATUS Status = EFI_UNSUPPORTED;


    UINT32 stackIndex = 0xFFFFFFFF;
    UINT16 parentPgID = 0;

	// On printScreen HotKey, Just return Unsupported and It will handed by Application Hotkey Handler.
	if( ( action->Input.Type == ACTION_TYPE_KEY ) &&
		(TsePrintScreenEventSupport(action->Input.Data.AmiKey.Key.ScanCode) ) ) 
	{
		Status = EFI_UNSUPPORTED;
		goto DONE;
        
	}
/*
#if TSE_PRN_SCRN_EVENT_SUPPORT
	if (( action->Input.Type == ACTION_TYPE_KEY ) && ( action->Input.Data.AmiKey.Key.ScanCode == TSE_PRN_SCRN_KEY_SCAN ))
            return EFI_UNSUPPORTED;
#endif
*/
	for ( i = 0; i < page->FrameCount; i++ )
	{
		if ( gFrame.HandleAction( page->FrameList[StyleFrameIndexOf(i)], action ) == EFI_SUCCESS )
		{	
            Status = EFI_SUCCESS;
		    goto DONE;
        }
	}

	for ( i = 0; i < page->HotKeyCount; i++ )
	{
		if ( gHotKey.HandleAction( page->HotKeyList[i], action ) == EFI_SUCCESS )
        {
			Status = EFI_SUCCESS;
		    goto DONE;
        }
	}

	if(
        (( action->Input.Type == ACTION_TYPE_KEY ) && (ControlActionAbort == MapControlKeysHook(action->Input.Data.AmiKey)))
        || ((action->Input.Type == ACTION_TYPE_MOUSE) && (ControlActionAbort == MapControlMouseActionHook(&(action->Input.Data.MouseInfo))) /*(TSEMOUSE_RIGHT_CLICK == action->Input.Data.MouseInfo.ButtonStatus)*/)
      )
	{
	
	//Commented for Legacy Style 07/07/10
	//if ( page->PageData.PageParentID == 0 )
        //    return EFI_UNSUPPORTED;

        
        if ( page->PageData.PageID != page->PageData.PageParentID )
        {
            // check for inconsistency here too.
            if (CheckInconsistence((PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[page->PageData.PageID]))!= TRUE )
            {
                // reset focus to first control
                FRAME_DATA *frame = page->FrameList[StyleFrameIndexOf(MAIN_FRAME)];
                UINT32 i;

                for ( i = 0; i < frame->ControlCount; i++ )
                {
                    if ( _FrameSetControlFocus( frame, i ) )
                    {
                        frame->FirstVisibleCtrl = 0;
                        break;
                    }
                }

                if(IsLinkHistorySupport())
                {
                    // tell application that we need to change pages
                    Status = GetParentFormID((UINT16)gApp->CurrentPage, &parentPgID, &stackIndex); //Gets the parent page of the current page
    				if(Status != EFI_SUCCESS)
    					gApp->CurrentPage = page->PageData.PageParentID;
    				 else
                        gApp->CurrentPage = parentPgID; //Else set the current page to the parent page
                }
                else
                {
                    gApp->CurrentPage = page->PageData.PageParentID;
                }

                gApp->CompleteRedraw = TRUE;
				gApp->OnRedraw = SUBMENU_COMPLETE_REDRAW;
            }

            Status = EFI_SUCCESS;
        }
    }
DONE:
   
    if(IsLinkHistorySupport())
    {
        if(Status == EFI_SUCCESS){
       		SetParentFormID(page->PageData.PageID, (UINT16)gApp->CurrentPage);
    	}
    }

    return Status ;
    
}
/**
    Adds controls to the frame 

    @param frame Pointer to the frame data
    @param controlData Control date to be added

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/
EFI_STATUS LegacyFrameAddControl( FRAME_DATA *frame, CONTROL_INFO *controlData )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CONTROL_DATA **control;
	POSITION_INFO *pos;
	UINT16 count = (UINT16)frame->ControlCount;
	UINT16 i;
	BOOLEAN		IsValidControl = FALSE;
	PAGE_INFO *pageInfo=NULL;

	gActiveFrame = frame;


	if(controlData->ControlType == CONTROL_TYPE_SUBMENU)
	{
		pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[controlData->ControlDestPageID]);
		
		if(pageInfo->PageHandle == NULL)
		{
			frame->NullCount--;
			return EFI_OUT_OF_RESOURCES;
		}
	}
		
	control = MemReallocateZeroPool( frame->ControlList, count * sizeof(CONTROL_INFO *), (count + 1) * sizeof(CONTROL_INFO *) );

	if ( control == NULL )
		return EFI_OUT_OF_RESOURCES;

	frame->ControlList = control;
	control = &frame->ControlList[ count ];

	for(i=0;gTseControlMap[i].ControlType != CONTROL_TYPE_NULL;i++)
	{
		if(gTseControlMap[i].ControlType == controlData->ControlType)
		{
			Status = gTseControlMap[i].ControlMethods->Create(control);
			if ( ! EFI_ERROR(Status) )
			{
				if(gTseControlMap[i].CallbackSuppored )
					gTseControlMap[i].ControlMethods->SetCallback(*control, frame, 
							(OBJECT_CALLBACK)(UINTN)gTseControlMap[i].CallbackFunction, 
							(VOID*)gTseControlMap[i].CallbackContext);
			}
			IsValidControl = TRUE;
		}
	}

	if(!IsValidControl)
	{
		if(controlData->ControlType == CONTROL_TYPE_NULL )
			frame->NullCount++;
		return Status;
	}
	
	if ( ! EFI_ERROR( Status ) )
	{
		Status = (*control)->Methods->Initialize( *control, controlData );

		if ( ! EFI_ERROR( Status ) )
		{
            UINT16 height = 1;
			// TSE_MULTILINE_CONTROLS moved to binary
			if(IsTSEMultilineControlSupported())
			{
				Status = (*control)->Methods->GetControlHeight( *control, (VOID*)frame,&height);
	   			height = height ? height:1;
			}

			if(frame->FrameData.FrameType == SUBTITLE_FRAME)
				(*control)->Methods->SetDimensions( *control, frame->FrameData.Width , height );
			else
				(*control)->Methods->SetDimensions( *control, frame->FrameData.Width - (UINT8)gLabelLeftMargin - 2 , height );

			(*control)->ParentFrameType = frame->FrameData.FrameType ;

			frame->ControlCount++;
			count++;

			(*control)->Methods->SetPosition( *control, frame->FrameData.Left + (UINT8)gLabelLeftMargin, frame->FrameData.Top + count -1 );

		 	
		 	if( EFI_ERROR( StyleHandleControlOverflow( frame, frame->ControlList[ count -1 ], (UINT16)frame->ControlCount )) )
			{
				frame->LastVisibleCtrl = frame->ControlCount-1;
			}

			// set position information
			pos = MemReallocateZeroPool( frame->OrigPosition, (count-1) * sizeof(POSITION_INFO), (count ) * sizeof(POSITION_INFO) );
			if ( pos != NULL )
			{
				pos[count-1].Left = (*control)->Left;
				pos[count-1].Top = (*control)->Top;
				frame->OrigPosition = pos;
			}

			MemFreePointer( (VOID **)&frame->CurrPosition );
			frame->CurrPosition = EfiLibAllocateZeroPool( count * sizeof(POSITION_INFO) );
			
		}

	}
	return Status;
}
/**
    Draws the scrollbar controls for the frame..

    @param frame Pointer to the current frame
    @param FirstLine FirstLine of the scrollbar
    @param LastLine LastLine	 
    @param modVal modVal 
    @param sizeOfBar Size of the Scrollbar
    @param numOfBlocks Number of blocks 
    @param bEraseScrollBar flag to erase the scrollbar

        EFI_STATUS status - 
    @retval EFI_SUCCESS If there exists a next page and the 
        function successfully loads it				
    @retval EFI_OUT_PF_RESOURCES If there are know previous page

**/
EFI_STATUS LegacyFrameDrawScrollBar (FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, UINT32	sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar)
{
	UINT32	i, j ; //,FirstLine=0,LastLine=0, count,visibleControlCount = 0, modVal=0, sizeOfBar, numOfBlocks = 0;
	CHAR16	ArrUp[2]={{GEOMETRICSHAPE_UP_TRIANGLE},{0x0000}},ArrDown[2]={{GEOMETRICSHAPE_DOWN_TRIANGLE},{0x0000}},ScrlSel[2]={{BLOCKELEMENT_FULL_BLOCK/*L'N'*/},{0x0000}},ScrlNSel[2]={{BLOCKELEMENT_LIGHT_SHADE/*L'S'*/},{0x0000}};


	// draw scrollbar if necessary
	if ( frame->BlankLine != NULL )
	{
		j = frame->FrameData.Top;
		for ( i = 1; i < frame->FrameData.Height; i++, j++ )
		{
            // use this loop to draw the scrollbar
		    if(frame->UseScrollbar)
		    {
				if(i==1)
					DrawStringWithAttribute( frame->FrameData.Left + frame->FrameData.Width+2 , j, ArrUp, 
				                            StyleGetScrollBarUpArrowColor() );
				else if(i==(UINT16)(frame->FrameData.Height - 1))
					DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width+2, j, ArrDown, 
					                             StyleGetScrollBarDownArrowColor() );
				else
				{
					if( (j > FirstLine) && (j < LastLine) &&  ((sizeOfBar + modVal) != 0))
					{	
						DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width+2, j, ScrlSel, 
													StyleGetScrollBarColor() );
						sizeOfBar--;
					}
					else
					{
						DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width+2, j, ScrlNSel, 
							                         StyleGetScrollBarColor() ); 
					}
				}
			}
			else if (bEraseScrollBar == TRUE)
				DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width+2, j, L" ", 
					                         StyleGetScrollBarColor() ); 

		}
		bEraseScrollBar = FALSE;
	}
			

	return EFI_SUCCESS;
}
/**
    Draws the controls for the given frame 

    @param frame Frame information to draw controls

    @retval EFI_STATUS EFI_SUCCESS if successful, else EFI_ERROR

**/
EFI_STATUS LegacyFrameDraw( FRAME_DATA *frame )
{
	CONTROL_DATA **control;
	UINT32	i, j,FirstLine=0,LastLine=0, count,visibleControlCount = 0, modVal=0, sizeOfBar, numOfBlocks = 0;
	UINT16 CurrHeight, ControlTop;
    	UINT8 u8ChkResult;
	EFI_STATUS Status = EFI_UNSUPPORTED;

    	BOOLEAN bCurrNotFocused = FALSE,bEraseScrollBar=FALSE;

	if((frame->FrameData.FrameType != MAIN_FRAME) || ( frame->PageID != 0 ))
    	{
        Status = FrameDraw(frame );
        return Status;
    	}


	if(StyleFrameDrawable(frame)) 
		return EFI_UNSUPPORTED;
	
	if ( frame->ControlCount == 0 )
		return EFI_UNSUPPORTED;

    //If there is an active control send draw only for that
   	if(frame->CurrentControl == FRAME_NONE_FOCUSED)
	{
		control = NULL;
		bCurrNotFocused = TRUE; // Non is focused
	}
	else
		control = &frame->ControlList[frame->CurrentControl];

	if ( control != NULL )
	{
        //If a control is active only draw that
		if( (*control)->ControlActive == TRUE )
		{
			(*control)->Methods->Draw( *control );
			return EFI_SUCCESS;
		}
		
		if(COND_NONE == CheckControlCondition( &((*control)->ControlData) ))
		{
			// Set the Focus to current control
			if((*control)->Methods->SetFocus(*control,TRUE) !=  EFI_SUCCESS)
			{
				frame->CurrentControl = FRAME_NONE_FOCUSED;
				bCurrNotFocused = TRUE; // Non is focused
			}
		}
	}

    //There is no active control we have to draw all in the view.

   	// First, clear the frame.
	if ( frame->BlankLine != NULL )
	{
		j = frame->FrameData.Top;
		for ( i = 1; i < frame->FrameData.Height; i++, j++ )
		{
			DrawStringWithAttribute( frame->FrameData.Left, j, frame->BlankLine, frame->FrameData.FGColor | frame->FrameData.BGColor );
		}
	}

	//Set position for all controls begining from FirstVisibleControl.
    //In the process calculate the correct LastVisibleControl.
    control = frame->ControlList;

    control = control+ frame->FirstVisibleCtrl;
    CurrHeight = frame->FrameData.Top;
    frame->LastVisibleCtrl = frame->FirstVisibleCtrl;

	for ( i = frame->FirstVisibleCtrl; i < frame->ControlCount; i++, control++ )
	{
        ControlTop = CurrHeight;

        //Check conditional
        u8ChkResult = CheckControlCondition( &((*control)->ControlData) );

        if(u8ChkResult == COND_NONE || u8ChkResult == COND_GRAYOUT)
        {
            //Control is drawn increament height
            CurrHeight = CurrHeight + (*control)->Height;
            if((frame->ControlCount > 1) && ((frame->FrameData.Top +frame->FrameData.Height) < CurrHeight+1 ))
            {
                break;
            }
            else
            {
                frame->LastVisibleCtrl = i;
            }
        }

        if(u8ChkResult != COND_NONE)
        {
            //Control grayed out or suppressed
            if(frame->CurrentControl == i)
            {
                //Current control cant be focused
                (*control)->Methods->SetFocus(*control,FALSE);
                bCurrNotFocused = TRUE;
            }
        }

        //(*control)->Methods->SetPosition( *control, (*control)->Left, ControlTop);
    }

    //Current control can not be focused? Find another one to focus.
    if(bCurrNotFocused)
    {
        control = frame->ControlList;
        control = control+ frame->FirstVisibleCtrl;

        for ( i = frame->FirstVisibleCtrl; i <= frame->LastVisibleCtrl; i++, control++ )
        {
            if ( (*control)->Methods->SetFocus( *control, TRUE ) == EFI_SUCCESS )
            {
                frame->CurrentControl = i;
                break;
            }
        }

        if(i > frame->LastVisibleCtrl)
            frame->CurrentControl = FRAME_NONE_FOCUSED;
    }

	//Draw controls in the set positions
    control = frame->ControlList;
    control = control+ frame->FirstVisibleCtrl;
    for ( i = frame->FirstVisibleCtrl; i <= frame->LastVisibleCtrl; i++, control++ )
    {
        (*control)->Methods->Draw( *control );
    }

    CurrHeight = frame->FrameData.Top;
	for (count = 0; count < frame->ControlCount; count++)
	{
		u8ChkResult = COND_NONE;

		// Support grayout condition for readonly controls
        u8ChkResult = CheckControlCondition( &(frame->ControlList[count]->ControlData) );
        
        if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
        {
			visibleControlCount++ ; 
            CurrHeight = CurrHeight + frame->ControlList[count]->Height;
	    }
	}

	// if already have a Scroll bar and due some changes (some controls may bedisabled) we don't need a scroll bar in this frame.
	if((frame->UseScrollbar) && ((frame->FrameData.Top + frame->FrameData.Height) >= CurrHeight+1))
	{
		bEraseScrollBar = TRUE;
		frame->UseScrollbar = FALSE;
	}

	// if we don't have a Scroll bar and due some changes (some controls may be enabled) we  need a scroll bar in this frame.
	if((frame->ControlCount > 1) && (!frame->UseScrollbar) && ((frame->FrameData.Top + frame->FrameData.Height) < CurrHeight+1))
	{
		frame->UseScrollbar = TRUE;
	}

	if(visibleControlCount==0)visibleControlCount=1;
    	FirstLine = (frame->FrameData.Height* frame->FirstVisibleCtrl/visibleControlCount )+ frame->FrameData.Top;
	LastLine  = (frame->FrameData.Height* frame->LastVisibleCtrl/visibleControlCount )+ frame->FrameData.Top;

	numOfBlocks = frame->FrameData.Height - 2;
	sizeOfBar = ((frame->FrameData.Height -1) * numOfBlocks) / visibleControlCount;
	modVal = ((((frame->FrameData.Height -1) * numOfBlocks) % visibleControlCount)* numOfBlocks) > (numOfBlocks/2) ? 1 : 0;

	// draw scrollbar if necessary
	StyleDrawScrollBar(frame, FirstLine, LastLine, modVal, sizeOfBar, numOfBlocks, bEraseScrollBar);	
			
	FlushLines( frame->FrameData.Top, frame->FrameData.Top + frame->FrameData.Height );

	return EFI_SUCCESS;
}
/**
    Function to Initialize Message Box, which uses the Popup functions.

    @param msgbox VOID *data

    @retval status

**/
EFI_STATUS LegacyMsgBoxInitialize( MSGBOX_DATA *msgbox, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT16 Length =0 ;
	VOID *buff;
	CONTROL_INFO MemoCtrlData;
	MEMO_DATA *memo;
	UINT8 Color ;
	extern MSGBOX_EX_CATAGORY gMsgBoxType;

	UINT16 titleToken;
  	VOID* TextHandle =((AMI_IFR_MSGBOX*)(((CONTROL_INFO*)data)->ControlPtr))->TextHandle;

	// initializes the heigth and width
	Status = gPopup.Initialize( msgbox, data );
	if (EFI_ERROR(Status))
		return Status;

	gMsgBoxType = msgbox->MsgBoxCatagory;
	Status = StyleGetMessageboxColor(msgbox->MsgBoxCatagory, &Color) ; 
	
	if ( EFI_ERROR( Status ) == EFI_SUCCESS)
	{
		msgbox->FGColor = Color & 0x0f ; 
		msgbox->BGColor = Color & 0xf0 ; 
	}else
	// add extra initialization here...
	SetControlColorsHook( NULL, NULL, NULL , NULL, NULL , NULL, NULL ,
	                      NULL , NULL,  NULL,NULL ,
						  NULL,NULL ,&(msgbox->FGColor),&(msgbox->BGColor)  );
	
	msgbox->FGColor = Colors.PopupFGColor ;
	msgbox->BGColor = Colors.PopupBGColor ;

	msgbox->Container = NULL;
	
	msgbox->Border =TRUE;

	titleToken = (UINT16)(((AMI_IFR_MSGBOX*)(msgbox->ControlData.ControlPtr))->Title);
	if ( titleToken != 0 ) {
		msgbox->Title = HiiGetString(msgbox->ControlData.ControlHandle, titleToken);
		if ( msgbox->Title == NULL )
			return EFI_OUT_OF_RESOURCES;
	}

	Length = (UINT16)HiiMyGetMultiLineStringLength(msgbox->ControlData.ControlHandle, (UINT16)(((AMI_IFR_MSGBOX*)(msgbox->ControlData.ControlPtr))->Text));

	if ( ( msgbox->Title != NULL) && ( Length < (TestPrintLength( msgbox->Title )/ (NG_SIZE)) ) )
		Length = (UINT16)(TestPrintLength( msgbox->Title ) / (NG_SIZE));

	msgbox->Width = (UINT16)(Length + 4);

	if(( msgbox->MsgBoxType == MSGBOX_TYPE_NULL ) && ( msgbox->Legend != NULL ))
	{ 
		if ( msgbox->Width < (UINT16)(TestPrintLength( msgbox->Legend )/ (NG_SIZE)) )
			msgbox->Width = (UINT16)(TestPrintLength( msgbox->Legend ) / (NG_SIZE)) + 8;
	}	
// MAX_MSGBOX_WIDTH macro changed as token and handled from binary
	if ( msgbox->Width > GetMsgboxWidth())
		msgbox->Width = GetMsgboxWidth();

	msgbox->ScreenBuf = SaveScreen();

	// initialize memo control 
	memo = msgbox->Memo;

	buff = UefiCreateSubTitleTemplate(((AMI_IFR_MSGBOX*)(msgbox->ControlData.ControlPtr))->Text );

	
	MemSet( &MemoCtrlData, sizeof(MemoCtrlData), 0 );
	MemoCtrlData.ControlPtr = (VOID*)buff;
	MemoCtrlData.ControlHandle = TextHandle;
	MemoCtrlData.ControlFlags.ControlVisible = TRUE;
	gMemo.Initialize(memo,(VOID *)&(MemoCtrlData));
	memo->Width = msgbox->Width - 2;
	gMemo.GetControlHeight((CONTROL_DATA *)memo, NULL, &msgbox->Height );
	msgbox->Height += 4;

	gMemo.SetAttributes((CONTROL_DATA *)memo,msgbox->FGColor ,msgbox->BGColor);
	gMemo.SetDimensions((CONTROL_DATA *)memo, msgbox->Width-2 , msgbox->Height-3 );
	gMemo.SetJustify( memo, JUSTIFY_CENTER );

	return Status;
}
/**
    this function uses the initialize function of control
    and initializes the submenu.

    @param submenu VOID *data

    @retval status

**/
EFI_STATUS LegacySubMenuInitialize( SUBMENU_DATA *submenu, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	//PAGE_INFO *pageInfo=NULL;
	//PAGE_DATA *Page ;
	
	Status = gControl.Initialize( submenu, data );
	if (EFI_ERROR(Status))
		return Status;

	

	// add extra initialization here...
	submenu->ControlData.ControlHelp = UefiGetHelpField((VOID *)submenu->ControlData.ControlPtr); //refPtr->Help;
	submenu->ControlFocus = FALSE;
	submenu->LabelMargin = (UINT8)gControlLeftMargin;

	// initialize default colors
	SetControlColorsHook(&(submenu->BGColor), &(submenu->FGColor),  
						NULL, NULL,
						&(submenu->SelBGColor), &(submenu->SelFGColor),
			            		&(submenu->NSelBGColor), &(submenu->NSelFGColor),  
						NULL,&(submenu->LabelFGColor) ,
					    	&(submenu->NSelLabelFGColor),
						NULL,NULL,
						NULL,NULL );


	//if((submenu->SubMenuType==0) && (submenu->ControlData.ControlPageID!=0))
	//	submenu->SelFGColor = FOCUS_COLOR ;

	

/*        

	if(submenu->SubMenuType==0)
	{
		Page = gApp->PageList[gApp->CurrentPage];
		
		if(Page->PageData.PageTitle == STRING_TOKEN(STR_EXIT))
		{
			submenu->SelFGColor = FOCUS_COLOR ;

		}
	}
*/        

/*						
	if((submenu->SubMenuType==0) && (submenu->ControlData.ControlPageID!=0))
	{
		pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[submenu->ControlData.ControlPageID]);
		
		if(pageInfo->PageTitle == STRING_TOKEN(STR_EXIT))
		{
			submenu->SelFGColor = FOCUS_COLOR ;

		}
	}	
*/			
	
	

#if SETUP_STYLE_LEGACY
	submenu->SubMenuType = (UINT8)(submenu->ControlData.ControlPageID != 0);
#endif

	return Status;
}
/**
    function to draw a sub-menu.

    @param submenu 

    @retval status

**/
EFI_STATUS LegacySubMenuDraw( SUBMENU_DATA *submenu )
{
	CHAR16 *text=NULL,*text1=NULL;
	UINTN length;
	EFI_STATUS Status = EFI_SUCCESS;
//	UINT8 ColorSubMenu = submenu->NSelFGColor;
	UINT8 ColorLabel = (submenu->ControlFocus) ? submenu->LabelFGColor : submenu->NSelLabelFGColor ;
	UINT32 PageNum ;

	PageNum = gApp->CurrentPage ;

	// check conditional ptr if necessary
    // Support grayout condition for readonly controls
	//if( submenu->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &submenu->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				Status = EFI_WARN_WRITE_FAILURE;
				ColorLabel = CONTROL_GRAYOUT_COLOR;
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}

	text1 = HiiGetString( submenu->ControlData.ControlHandle,  UefiGetPromptField((VOID *)(submenu->ControlData.ControlPtr)));
	if ( text1 == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
	// XXX check to see if text1 is not NULL
	length = (3 + (TestPrintLength( text1 ) / (NG_SIZE))) * sizeof(CHAR16);
	text = EfiLibAllocateZeroPool( length );
	if ( text == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	if((submenu->SubMenuType == 0)&&( PageNum == 0 ))
		SPrint( text, length,  L"%s", text1 );
	else if(submenu->SubMenuType == 2)
		SPrint( text, length, L"%s", text1 );
	else
		SPrint( text, length, L"%c %s", GEOMETRICSHAPE_RIGHT_TRIANGLE, text1 );		


	//SPrint( text, length, L"%c %s", (submenu->SubMenuType == 2)?L' ':GEOMETRICSHAPE_RIGHT_TRIANGLE, text1 );

	//SPrint( text, length,  ((submenu->SubMenuType == 0)&&( PageNum == 0 ))?L"%c%s":L"%c %s", ((submenu->SubMenuType == 2)||( PageNum == 0 ))?L' ':GEOMETRICSHAPE_RIGHT_TRIANGLE, text1 );

	//SPrint( text, length,  ((submenu->SubMenuType == 0)&&( PageNum == 0 ))?L"%c%s":L"%c %s", ((submenu->SubMenuType == 2)||( PageNum == 0 ))?L' ':GEOMETRICSHAPE_RIGHT_TRIANGLE, text1 );
	MemFreePointer( (VOID **)&text1 );

	if ( submenu->SubMenuType != 1 )
	{
		//TSE_MULTILINE_CONTROLS moved to binary
		if(submenu->Height>1 && IsTSEMultilineControlSupported())
		{
			DrawMultiLineStringWithAttribute( submenu->Left , submenu->Top, 
					(UINTN)(submenu->Width-2),(UINTN) submenu->Height,
					&text[2], (UINT8)(  (submenu->ControlFocus) ? 
							  submenu->SelBGColor  | ColorLabel /*submenu->SelFGColor*/ :
							  submenu->NSelBGColor | ColorLabel /*ColorSubMenu*/ )  );
			if(submenu->SubMenuType != 2)
			{
				text[2]=0;
				DrawStringWithAttribute( submenu->Left - 2, submenu->Top, (CHAR16*)text, 
						(UINT8)(  (submenu->ControlFocus) ? 
								  submenu->SelBGColor  | ColorLabel /*submenu->SelFGColor*/ :
								  submenu->NSelBGColor | ColorLabel /*ColorSubMenu*/ ) );
			}
		}
		else	
		{
			
			if(submenu->SubMenuType==0 && !submenu->ControlFocus && ColorLabel != CONTROL_GRAYOUT_COLOR)
				ColorLabel = StyleGetPageLinkColor();


            if( PageNum == 0 )
            {
                if((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)((submenu->Width/2)-2))
				    EfiStrCpy( &text[HiiFindStrPrintBoundary(text,(UINTN)((submenu->Width/2)-5))],L"...");
                
            }else{
						
			
			if((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(submenu->Width-2))
				EfiStrCpy( &text[HiiFindStrPrintBoundary(text,(UINTN)(submenu->Width-5))],L"...");

            }
	
			DrawStringWithAttribute( submenu->Left, submenu->Top, (CHAR16*)text, 
					(UINT8)(  (submenu->ControlFocus) ? 
							  submenu->SelBGColor  | submenu->SelFGColor :
							  submenu->NSelBGColor | ColorLabel ) );
		}
	}
	else
	{
		if((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(submenu->Width-2))
			EfiStrCpy( &text[HiiFindStrPrintBoundary(text,(UINTN)(submenu->Width-5))],L"...");

		text1 = HiiGetString( gHiiHandle, STRING_TOKEN(STR_SUBMENU_OPTION) );
		if ( text1 != NULL )
		{
			DrawStringWithAttribute( submenu->Left - 2, submenu->Top, (CHAR16*)text, 
					EFI_BACKGROUND_BLUE | EFI_WHITE );
			MemFreePointer( (VOID **)&text );

			length = (3 + (TestPrintLength( text1 ) / (NG_SIZE))) * sizeof(CHAR16);
			text = EfiLibAllocatePool( length );
			if ( text != NULL )
			{
				SPrint( text, length, L"[%s]", text1 );
		
			 	if ((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(submenu->Width-2))
					EfiStrCpy( &text[HiiFindStrPrintBoundary(text,(UINTN)(submenu->Width-5))],L"...");

				DrawStringWithAttribute( submenu->Left  + submenu->LabelMargin , submenu->Top, text,
					(UINT8)(  (submenu->ControlFocus) ? 
						  	submenu->SelBGColor  | submenu->SelFGColor :
							submenu->NSelBGColor | submenu->NSelFGColor ) );

				//if ( submenu->ControlFocus )
				//	DrawStringWithAttribute( submenu->Left + submenu->LabelMargin + 1,
				//			submenu->Top, text1, (UINT8)(submenu->SelBGColor | submenu->SelFGColor) );
			}
		}
	}
	FlushLines( submenu->Top, submenu->Top );
DONE:
	if(text)
		MemFreePointer( (VOID **)&text );
	if(text1)
		MemFreePointer( (VOID **)&text1 );
	return Status;
}

/**
    Function to initialize style

    @param VOID

    @retval VOID

**/
VOID	OverRideStyleInit( VOID )
{
	VOID *data = NULL ;
	data = (UINT8 *)gApplicationData + gPages->PageList[0] ;

	if( ((PAGE_INFO*)data)->PageHandle == 0)
	       	((PAGE_INFO*)data)->PageHandle = gHiiHandle ;

	
	gLabelLeftMargin = STYLE_LABEL_LEFT_MARGIN ;
	gControlLeftMargin = STYLE_CONTROL_LEFT_MARGIN ;
	gControlLeftPad = STYLE_CONTROL_LEFT_PAD ;
	

	#if SETUP_STYLE_FULL_SCREEN
	if ( gMaxCols == STYLE_FULL_MAX_COLS )
	{
		_gStyleMainFrames = _gStyleLegacyFullMainFrames ;
		_gStyleSubFrames = _gStyleLegacyFullSubFrames ;

		gLabelLeftMargin = FULL_STYLE_LABEL_LEFT_MARGIN ;
		gControlLeftMargin = FULL_STYLE_CONTROL_LEFT_MARGIN ;
		gControlLeftPad = FULL_STYLE_CONTROL_LEFT_PAD ;
	
	}
	#endif

	//
    //When Style FrameData is overridden/modified by OEM or other modules, TSE will update again to reflect the changes.
    //
    if ( EFI_SUCCESS == StyleInitRecalculateFrames(_gStyleLegacyFullSubFrames, _gStyleLegacyFullMainFrames) )
    {
        _gStyleMainFrames = _gStyleLegacyFullMainFrames ;
        _gStyleSubFrames = _gStyleLegacyFullSubFrames ;
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
	UINT32 i, PageNum ;

	PageNum = gApp->CurrentPage ;


	if ( PageNum == 0 )
		FrameCount = sizeof(_gStyleLegacyMainFrames) / sizeof(FRAME_INFO);
	else
		FrameCount = sizeof(_gStyleLegacySubFrames) / sizeof(FRAME_INFO);


	for ( i = 0; i < FrameCount; i++ )
	{
		if ( PageNum == 0 )
		{
			if( _gStyleMainFrames[i].FrameType == frameType )
			{
				return i ;
				break ;
			}
		}else{

			if( _gStyleSubFrames[i].FrameType == frameType )
			{
				return i ;
				break ;
			}

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
	if ( page == 0 )
		return sizeof(_gStyleLegacyMainFrames) / sizeof(FRAME_INFO);

	return sizeof(_gStyleLegacySubFrames) / sizeof(FRAME_INFO);
}
/**
    Function to draw page border

    @param page 

    @retval VOID

**/
VOID OverRideStyleDrawPageBorder( UINT32 page )
{
	CHAR16 *line;
	UINT16 lineNumber;

	UINT16 start, end, col;

	PAGE_DATA *Page=NULL ;
	FRAME_DATA *fHelptitle=NULL, *fHelp=NULL ;

	Page = gApp->PageList[page];
	
	fHelptitle = Page->FrameList[StyleFrameIndexOf(HELPTITLE_FRAME)];
	fHelp = Page->FrameList[StyleFrameIndexOf(HELP_FRAME)];
	




	line = EfiLibAllocateZeroPool( (gMaxCols + 1) * sizeof(CHAR16) );
	if ( line == NULL )
		return;

	ClearScreenWithoutFlush( EFI_BACKGROUND_BLUE | EFI_WHITE );

	MemFillUINT16Buffer( &line[1], gMaxCols - 2, L' ' );
	line[0] = line[gMaxCols - 1] = BOXDRAW_DOUBLE_VERTICAL;

	if ( page == 0 )
	{
		start = 2;
		end = (UINT8)(gMaxRows - 2);
	}
	else
	{
		start = 3;
		end = (UINT8)(gMaxRows - 3);
	}

	for ( lineNumber = start; lineNumber < end; lineNumber++ )
		DrawString( 0, lineNumber, line );

	MemFillUINT16Buffer( &line[1], gMaxCols - 2, BOXDRAW_DOUBLE_HORIZONTAL );
	// top line
	line[0] = BOXDRAW_DOUBLE_DOWN_RIGHT;
	line[gMaxCols - 1] = BOXDRAW_DOUBLE_DOWN_LEFT;
	DrawString( 0, start - 1, line );

	// bottom line
	line[0] = BOXDRAW_DOUBLE_UP_RIGHT;
	line[gMaxCols - 1] = BOXDRAW_DOUBLE_UP_LEFT;
	DrawString( 0, end, line );

	MemFillUINT16Buffer( &line[1], gMaxCols - 2, BOXDRAW_HORIZONTAL );

	if ( page == 0 )
	{
		line[0] = BOXDRAW_VERTICAL_DOUBLE_RIGHT;
		line[gMaxCols - 1] = BOXDRAW_VERTICAL_DOUBLE_LEFT;
		DrawString( 0, gMaxRows - 4, line );
		line[gMaxCols / 2] = BOXDRAW_UP_HORIZONTAL;
		DrawString( 0, gMaxRows - 7, line );

		line[0] = BOXDRAW_DOWN_HORIZONTAL_DOUBLE;
		line[1] = L'\0';
		DrawString( gMaxCols / 2, 1, line );
	}
	else
	{
		// dividing line
		line[0] = BOXDRAW_VERTICAL_RIGHT;
		line[fHelp->FrameData.Width + 2] = BOXDRAW_VERTICAL_DOUBLE_LEFT;
		line[fHelp->FrameData.Width + 3] = L'\0';
		DrawString( fHelp->FrameData.Left - 2, 4, line );
		line[0] = BOXDRAW_DOWN_HORIZONTAL_DOUBLE;
		line[1] = L'\0';
		DrawString( fHelp->FrameData.Left - 2, 2, line );
		line[0] = BOXDRAW_UP_HORIZONTAL_DOUBLE;
		DrawString( fHelp->FrameData.Left - 2, gMaxRows - 3, line );

	}

	// center divider
	line[0] = BOXDRAW_VERTICAL;

	if ( page == 0 )
	{
		start = 2;
		end = (UINT8)(gMaxRows - 7);
		col = (UINT8)(gMaxCols / 2);
	}
	else
	{
		start = 5;
		end = (UINT8)(gMaxRows - 3);
		col = fHelp->FrameData.Left - 2;
	}
	for ( lineNumber = start; lineNumber < end; lineNumber++ )
		DrawString( col, lineNumber, line );

	if ( page != 0 )
		DrawString( fHelp->FrameData.Left - 2, 3, line );

	FlushLines( 0, gMaxRows - 1 );

	gBS->FreePool( line );
	
}
/**
    Function to get frame initialization data

    @param page UINT32 frame

    @retval VOID* 

**/
VOID *OverRideStyleGetFrameInitData( UINT32 page, UINT32 frame )
{
	if ( page == 0 )
				return &_gStyleMainFrames[frame];

	return &_gStyleSubFrames[frame];

}
/**
    Function to override component or control functionality

    @param VOID

    @retval VOID

**/
VOID OverRideComponent(VOID)
{
    gMsgBox.Initialize = LegacyMsgBoxInitialize ;
    gSubMenu.Initialize = LegacySubMenuInitialize ;
    gSubMenu.Draw = LegacySubMenuDraw ;
    gFrame.Draw = LegacyFrameDraw ;    		        // Override framedraw
	gFrame.AddControl = LegacyFrameAddControl ;     // Override FrameAddControl
    gPage.HandleAction = LegacyPageHandleAction ;   // Override FrameAddControl
    
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

#endif /* SETUP_STYLE_LEGACY */

/**
    Function to handle ESC Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
extern BOOLEAN gTseCacheUpdated;
VOID LegacyHandleEscKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
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
	else if ( gotoExitOnEscKey (&ExitPageGuid, &pageClass, &pageSubclass, &pageFormID) )//GotoExitPageOnEscKey
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
VOID LegacyHandleSaveExitKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	app->Quit = SaveAndExit();
}
/**
    Function to handle Load Previous value Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID LegacyHandlePrevValuesKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	//if ( app->CurrentPage != 0 )
		LoadPreviousValues( TRUE );
		gUserTseCacheUpdated = FALSE;	//Modifications reverted back to previous values, No need to keep the flag true
}
/**
    Function to handle Load Failsafe values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID LegacyHandleFailsafeKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	//if ( app->CurrentPage != 0 )
		LoadFailsafeDefaults();
}
/**
    Function to handle Load Optimal values Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID LegacyHandleOptimalKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	//if ( app->CurrentPage != 0 )
		LoadOptimalDefaults();
}
/**
    Function to handle Help Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie

    @retval VOID

**/
VOID LegacyHandleHelpKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
{
	//if ( app->CurrentPage != 0 )
		ShowHelpMessageBox();
}
/**
    Function to handle print screen Hotkey action

    @param app HOTKEY_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID LegacyHandlePrnScrnKey( APPLICATION_DATA *app, HOTKEY_DATA *hotkey, VOID *cookie )
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
VOID LegacyMainFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{
	if(app->Action->Input.Data.MouseInfo.ButtonStatus == TSEMOUSE_RIGHT_CLICK)
		app->Quit = ExitApplication();
}

/**
    Function to handle Help Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID LegacyHelpFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{
	
}

/**
    Function to handle Navigation Frame with Mouse

    @param app HOTCLICK_DATA *hotkey, VOID *cookie 

    @retval VOID

**/
VOID LegacyNavFrameHandlewithMouse( APPLICATION_DATA *app, HOTCLICK_DATA *hotclick, VOID *cookie )
{

}

/**
    Function to recaltulate the frame datas based on new gMaxRows and gMaxCols, if updated. 
 
    @param app FRAME_INFO *StyleFullSubFrames , FRAME_INFO *StyleFullMainFrames
 
    @retval EFI_STATUS
 
**/
EFI_STATUS StyleInitRecalculateFrames (FRAME_INFO *StyleFullSubFrames , FRAME_INFO *StyleFullMainFrames)
{
    if (( gMaxRows != 31 && gMaxRows != 25  )||(gMaxCols != 100 && gMaxCols != 80 ))
   {               
        //
        //Main frame
        //
        
        //TITLE
        StyleFullMainFrames[0].Width     = (UINT16)(gMaxCols);
        StyleFullMainFrames[0].Height    = 1;
        StyleFullMainFrames[0].Top       = 0;
        StyleFullMainFrames[0].Left      = 0;
        //MAIN
        StyleFullMainFrames[1].Width     = (UINT16)(gMaxCols-2);
        StyleFullMainFrames[1].Height    = (UINT16)(gMaxRows-8);
        StyleFullMainFrames[1].Top       = 2;
        StyleFullMainFrames[1].Left      = 1;      
        //HELP
        StyleFullMainFrames[2].Width     = (UINT16)(gMaxCols-4);
        StyleFullMainFrames[2].Height    = 1; 
        StyleFullMainFrames[2].Top       = (UINT16)(gMaxRows-3);
        StyleFullMainFrames[2].Left      = 1;
        //NAV
        StyleFullMainFrames[3].Width     = (UINT16)(gMaxCols-4);
        StyleFullMainFrames[3].Height    = 2 ;
        StyleFullMainFrames[3].Top       = (UINT16)(gMaxRows-6);
        StyleFullMainFrames[3].Left      = 2;       
     
        //
        //Sub frame
        //
        
        //TITLE
        StyleFullSubFrames[0].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[0].Height    = 1;
        StyleFullSubFrames[0].Top       = 0;
        StyleFullSubFrames[0].Left      = 0;
        //SUBTITLE
        StyleFullSubFrames[1].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[1].Height    = 1;
        StyleFullSubFrames[1].Top       = 1;
        StyleFullSubFrames[1].Left      = 0;      
        //MAIN
        StyleFullSubFrames[2].Width     = (UINT16)((gMaxCols*2/3)-3);
        StyleFullSubFrames[2].Height    = (UINT16)(gMaxRows-6); 
        StyleFullSubFrames[2].Top       = 3;
        StyleFullSubFrames[2].Left      = 1;
        //HELPTITLE
        StyleFullSubFrames[3].Width     = (UINT16)((gMaxCols*1/3)-3);
        StyleFullSubFrames[3].Height    = 1;
        StyleFullSubFrames[3].Top       = 3;
        StyleFullSubFrames[3].Left      = (UINT16)((gMaxCols*2/3)+3);       
        //HELP
        StyleFullSubFrames[4].Width     = (UINT16)((gMaxCols*1/3)-3);
        StyleFullSubFrames[4].Height    = (UINT16)(gMaxRows-9);
        StyleFullSubFrames[4].Top       = 5;
        StyleFullSubFrames[4].Left      = (UINT16)((gMaxCols*2/3)+3);       
        //NAV
        StyleFullSubFrames[5].Width     = (UINT16)(gMaxCols);
        StyleFullSubFrames[5].Height    = 2 ;
        StyleFullSubFrames[5].Top       = (UINT16)(gMaxRows-2);
        StyleFullSubFrames[5].Left      = 0 ;
            
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
