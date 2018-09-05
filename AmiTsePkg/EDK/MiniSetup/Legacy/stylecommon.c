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
//**                       Phone: (770)-246-8600                 **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Legacy/stylecommon.c $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 9/17/12 6:26a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file stylecommon.c
    Contains generic or default style functions.

**/
//**********************************************************************
#include "minisetup.h"
#include "style.h"

extern UINTN gMaxBufX;
extern UINTN gMaxBufY;

#if SETUP_STYLE_LEGACY


VOID _StyleControlSubmenu( CONTROL_INFO *control, UINT16 pageID, VOID *ref );
VOID UpdatePageFocusedItemHelp(PAGE_DATA *page);
/**
    Adds controls to the frame 

    @param frame Pointer to the frame data
    @param Colors Color scheme 

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/
EFI_STATUS StyleControlColor(FRAME_DATA *frame, STYLECOLORS *Colors)
{
	//The goal here is to allow controls to match the BGColor of their frame.
	//Normally this is handled in StyleInitializeFrame(), before controls are added to the frame.
	//But this function is called dynamically during runtime (after pages/frames/controls are styled)
	//Since controls derive their colors from [STYLECOLORS Colors], 
	//we'll temporarly modify it before controls are added to this frame, then restore it.

	EFI_STATUS Status = EFI_SUCCESS;	

	#ifdef STYLE_OEM_CONTROL_COLOR				
		Status = OEMControlColor(frame, Colors);			
	#else
		#ifdef STYLE_CONTROL_COLOR	
			Status = OverRideControlColor(frame, Colors);	
		#else
					

				if(LABEL_FOCUS_COLOR != FOCUS_COLOR)
					Colors->LabelFGColor = LABEL_FOCUS_COLOR ;

				if(CONTROL_FOCUS_COLOR != FOCUS_COLOR)	
					Colors->SelFGColor = CONTROL_FOCUS_COLOR;

				if(LABEL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
					Colors->NSelLabelFGColor = LABEL_NON_FOCUS_COLOR ;

				if(CONTROL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
					Colors->NSelFGColor = CONTROL_NON_FOCUS_COLOR ;

				Colors->SelBGColor = CONTROL_FOCUS_BGCOLOR ;
				Colors->EditBGColor =	CONTROL_FOCUS_BGCOLOR ;
			
			
		#endif	

	#endif	
	
	return Status ;
}
/**
    Function to get control special color 

    @param ControlType UINT8 *Color

    @retval status

**/
EFI_STATUS StyleGetSpecialColor( UINT16 ControlType, UINT8 *Color)
{
	EFI_STATUS Status = EFI_SUCCESS;;	

	#ifdef STYLE_OEM_CONTROL_COLOR				
		Status = OEMGetSpecialColor(ControlType, Color);	
	#else
		
		
	switch ( ControlType )
	{
		case CONTROL_TYPE_MEMO:
			if (Color !=NULL) *Color = MEMO_COLOR ;	
			break;

		case CONTROL_TYPE_TEXT:
			if (Color !=NULL) *Color = TEXT_COLOR ;	
			break;
		
		default:
			Status = EFI_UNSUPPORTED;;
		break;
	}
	#endif		

	return Status ;
}

VOID GetMessageboxColorHook(UINT8 **Color);
/**
    Function to get control Messagebox color 

    @param ControlType UINT8 *Color

    @retval status

**/
EFI_STATUS StyleGetMessageboxColor( UINT8 MessageBoxType, UINT8 *Color)
{
	EFI_STATUS Status = EFI_SUCCESS;;	

	if(Color == NULL)
		return Status ;
	
	
	switch ( MessageBoxType )
	{
		case MSGBOX_EX_CATAGORY_HELP:
			*Color = POPUP_BGCOLOR | POPUP_FGCOLOR ;	
			break;

		case MSGBOX_EX_CATAGORY_QUERY:
			 *Color = EFI_BACKGROUND_BLUE | EFI_WHITE ;	
			 break;

		case MSGBOX_EX_CATAGORY_SUCCESS:
			 *Color = EFI_BACKGROUND_GREEN | EFI_WHITE ;	
			 break;

		case MSGBOX_EX_CATAGORY_WARNING:
			 *Color = EFI_BACKGROUND_RED | EFI_WHITE ;	
			 break;

		case MSGBOX_EX_CATAGORY_ERROR:
			 *Color = EFI_BACKGROUND_RED | EFI_WHITE ;	
			 break;

		case MSGBOX_EX_CATAGORY_PROGRESS:
			 *Color = EFI_BACKGROUND_LIGHTGRAY | EFI_WHITE ;	
			 break;

		case MSGBOX_EX_CATAGORY_HALT:
			 *Color = EFI_BACKGROUND_LIGHTGRAY | EFI_WHITE ;	
			 break;
		
		case MSGBOX_EX_CATAGORY_NORMAL:
		
		default:
			*Color = EFI_BACKGROUND_BLUE | EFI_WHITE ;
			
			break;
	}
	
	GetMessageboxColorHook(&Color);
	return Status ;
}
/**
    Function to alter frame styling before frame is drawn

    @param frame 

    @retval VOID

**/
VOID StyleBeforeFrame( FRAME_DATA *frame )
{
	#ifdef STYLE_OEM_BEFORE_FRAME				
		OEMStyleBeforeFrame(frame);			
	#else
		#ifdef STYLE_OVERRIDE_BEFORE_FRAME		// from alternate1.h
			OverRideStyleBeforeFrame(frame );	// From alternate1.c
		#endif
		
	#endif	
}
/**
    Function to update frame strings

    @param page 

    @retval VOID

**/
VOID StyleUpdateFrameStrings( PAGE_DATA *page )
{
	#ifdef STYLE_OEM_FRAME_TOKEN				// SDL
		OEMStyleUpdateFrameStrings(page);		// OEM
	#else
		#ifdef STYLE_OVERRIDE_FRAME_TOKEN		// from Ezport.h
			OverRideStyleUpdateFrameStrings(page );	// From ezport.c
		#endif
	#endif		
}
/**
    Function to determine if frame is drawable

    @param frame 

    @retval EFI_STATUS

**/
EFI_STATUS StyleFrameDrawable( FRAME_DATA *frame )
{
	EFI_STATUS Status = EFI_SUCCESS;

	#ifdef STYLE_OEM_FRAME_DRAWABLE					// SDL
		Status = OEMStyleFrameDrawable(frame);			// OEM
	#else
		#ifdef STYLE_OVERRIDE_FRAME_DRAWABLE			// from Ezport.h
			Status = OverRideStyleFrameDrawable(frame );	// From ezport.c
		#endif
	#endif		

	return Status ;
}
/**
    Function to initialize style  

    @param VOID

    @retval VOID

**/
VOID	StyleInit( VOID )
{
    
    if((gMaxBufX != 0) && (gMaxBufY != 0)) // Reload the original Rows and Cols data after returning from boot option.
    {
        gMaxRows = gMaxBufX;
        gMaxCols = gMaxBufY;
    }
    
	SetDesiredTextMode();
	OverRideStyleInit();		// From ezport.c	
	
#ifdef STYLE_OEM_INIT			// SDL
	OEMStyleInit();			// OEM
#endif
	MouseInit(); 
}
/**
    Function to do style initialization on exit

    @param VOID

    @retval VOID

**/
VOID	StyleExit( VOID )
{
	#ifdef STYLE_OEM_STYLE_EXIT			// SDL
		OEMStyleExit();				// OEM
	#else
		#ifdef STYLE_OVERRIDE_EXIT		// from Ezport.h
		OverRideStyleExit();			// From ezport.c
		#endif
	#endif
	MouseDestroy();  
}
/**
    Function to draw the page border

    @param page number

    @retval VOID

**/
VOID StyleDrawPageBorder( UINT32 page )
{
	#ifdef STYLE_OEM_PAGE_BORDER				// SDL
		OEMStyleDrawPageBorder(page);			// OEM
	#else
		#ifdef STYLE_OVERRIDE_PAGE_BORDER		// from Ezport.h
			OverRideStyleDrawPageBorder(page);	// From ezport.c
		#else
			ClearScreen( EFI_BACKGROUND_BLUE | EFI_WHITE );
		#endif
	#endif	
	
}
/**
    Function to draw the frame scrollbar

    @param frame UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, UINT32 sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar

    @retval VOID

**/
VOID StyleDrawScrollBar(FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32 modVal, 
				UINT32 sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar)
{
	#ifdef STYLE_OEM_SCROLLBAR					
		OEMDrawScrollBar(frame, FirstLine, LastLine, modVal, sizeOfBar, numOfBlocks, bEraseScrollBar);			
	#else
		#ifdef STYLE_OVERRIDE_SCROLLBAR			
			LegacyFrameDrawScrollBar(frame, FirstLine, LastLine, modVal, sizeOfBar, numOfBlocks, bEraseScrollBar);
		#else
			FrameDrawScrollBar(frame, FirstLine, LastLine, modVal, sizeOfBar, numOfBlocks, bEraseScrollBar);
		#endif
	#endif	
	
}
/**
    Function to draw the help frame scrollbar

    @param memo UINT16 height

    @retval VOID

**/
VOID StyleDrawHelpScrollBar( MEMO_DATA *memo, UINT16 height)
{
	#ifdef STYLE_OEM_DRAW_HELP_SCROLLBAR					
		OEMDrawHelpScrollBar(memo, height);			
	#else
		#ifdef STYLE_OVERRIDE_HELP_SCROLLBAR			
			OverRideDrawHelpScrollBar(memo, height);
		#else
			if(IsHelpAreaScrollBarSupport())
				MemoDrawScrollBar(memo, height); 
		#endif
	#endif	
	
}
/**
    Function to get frame initialization data

    @param page number, UINT32 frame number

    @retval VOID

**/
VOID *StyleGetFrameInitData( UINT32 page, UINT32 frame )
{
	#ifdef STYLE_OEM_FRAME_INIT_DATA					
		return OEMStyleGetFrameInitData(page, frame);		
	#else
		#ifdef STYLE_OVERRIDE_FRAME_INIT_DATA				// from Ezport.h
		 	return	OverRideStyleGetFrameInitData(page, frame);	// From ezport.c
		#else
			return NULL;
		#endif
	#endif	
	
}
/**
    Function to handle frame action keys

    @param frame EFI_INPUT_KEY Key

    @retval EFI_STATUS

**/
EFI_STATUS StyleFrameHandleKey( FRAME_DATA *frame, EFI_INPUT_KEY Key )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;;

	#ifdef STYLE_OEM_HANDLE_KEY				// SDL
		Status = OEMStyleFrameHandleKey(frame, Key);			// OEM
	#else
		#ifdef STYLE_OVERRIDE_HANDLE_KEY		// from Ezport.h
		 	Status = OverRideStyleFrameHandleKey(frame, Key);	// From ezport.c
		#endif
	#endif		
	
	return Status ;
}
/**
    Function to handle frame using mouse

    @param frame 
    @param MouseInfo 

    @retval EFI_STATUS

**/
EFI_STATUS StyleFrameHandleMouse( FRAME_DATA *frame, MOUSE_INFO MouseInfo)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;;

	#ifdef STYLE_OEM_HANDLE_MOUSE				
		Status = OEMStyleFrameHandleMouse(frame, MouseInfo);			
	#else
		#ifdef STYLE_OVERRIDE_HANDLE_MOUSE		
		 	Status = OverRideStyleFrameHandleMouse(frame, MouseInfo);	
		#endif
	#endif		
	
	return Status ;
}
/**
    Function to handle frame timer action

    @param frame ACTION_DATA *action

    @retval EFI_STATUS

**/
EFI_STATUS StyleFrameHandleTimer( FRAME_DATA *frame, ACTION_DATA *action )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;;

	#ifdef STYLE_OEM_HANDLE_TIMER				
		Status = OEMStyleFrameHandleTimer(frame, action);			
	#else
		#ifdef STYLE_OVERRIDE_HANDLE_TIMER		
		 	Status = OverRideStyleFrameHandleTimer(frame, action);	
		#endif
	#endif		
	
	return Status ;
}
/**
    Function to handle control overflow

    @param frame CONTROL_DATA *control, UINT16 count 

    @retval EFI_STATUS

**/
EFI_STATUS StyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	#ifdef STYLE_OEM_CONTROL_OVERFLOW						
		Status = OEMStyleHandleControlOverflow(frame, control, count );
	#else
		#ifdef STYLE_OVERRIDE_CONTROL_OVERFLOW
		 	Status = OverRideStyleHandleControlOverflow(frame, control, count );
		#endif
	#endif		
	
	return Status ;	

}
/**
    Function to Initialize frame

    @param frame UINT32 frameType, PAGE_INFO *data

    @retval EFI_STATUS

**/
EFI_STATUS StyleInitializeFrame( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data )
{
	EFI_STATUS Status = EFI_SUCCESS;;

	#ifdef STYLE_OEM_INITIALIZE_FRAME						
		Status = OEMStyleInitializeFrame(frame, frameType, data );
	#else
		#ifdef STYLE_OVERRIDE_INITIALIZE_FRAME
		 	Status = OverRideStyleInitializeFrame(frame, frameType, data);	//style.c
		#endif
	#endif		
	
	return Status ;	
	
}
/**
    Function to get navigation token 

    @param page 

    @retval UINT16 token number

**/
UINT16 StyleGetNavToken( UINT32 page )
{
	UINT16 i=0 ;	

	#ifdef STYLE_OEM_NAV_TOKEN						
		i = OEMStyleGetNavToken(page );
	#else
		#ifdef STYLE_OVERRIDE_NAV_TOKEN
		 	i = OverRideStyleGetNavToken(page);
		#endif
	#endif		
	
	return i ;
}
/**
    Function to add additinal controls

    @param frame UINT32 controlNumber, BOOLEAN focus

    @retval EFI_STATUS

**/
EFI_STATUS StyleAddAdditionalControls( FRAME_DATA *frame, UINT32 controlNumber, BOOLEAN focus )
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	#ifdef STYLE_OEM_ADD_CONTROLS
		Status = OEMStyleAddAdditionalControls(frame, controlNumber, focus );
	#else
		#ifdef STYLE_OVERRIDE_ADD_CONTROLS
		 	Status = OverRideStyleAddAdditionalControls(frame, controlNumber, focus);
		#endif
	#endif		
	
	return Status ;
}
/**
    Function to set Language page

    @param submenu 

    @retval UINT16 page number

**/
UINT16 StyleLanguagePage( SUBMENU_DATA *submenu )
{
	UINT16 i = 0 ;
	
	#ifdef STYLE_OEM_LANGUAGE_PAGE						
		i = OEMStyleLanguagePage(submenu );
	#else
		#ifdef STYLE_OVERRIDE_LANGUAGE_PAGE
		 	i = OverRideStyleLanguagePage(submenu);
		#endif
	#endif			

	return i ;
}



static UINT16 _gBootManagerPageID = 0;
/*static*/ UINT16 _gHDOrderPageID = 0;
/*static*/ UINT16 _gCDOrderPageID = 0;
/*static*/ UINT16 _gFDOrderPageID = 0;
/*static*/ UINT16 _gNetOrderPageID = 0;
/*static*/ UINT16 _gBevOrderPageID = 0;

/**
    Function to set boot manager page

    @param submenu 

    @retval UINT16 page number

**/
UINT16 StyleBootManagerPage( SUBMENU_DATA *submenu )
{
	UINT16 i = 0 ;
	
	#ifdef STYLE_OEM_BM_PAGE						
		i = OEMStyleBootManagerPage(submenu );
	#else
		#ifdef STYLE_OVERRIDE_BM_PAGE
		 	i = OverRideStyleBootManagerPage(submenu);
		#endif
	#endif			
	
	return i ;
}
/**
    Function to set label callback

    @param frame LABEL_DATA *label, VOID *cookie

    @retval VOID

**/
VOID StyleLabelCallback( FRAME_DATA *frame, LABEL_DATA *label, VOID *cookie )
{
	#ifdef STYLE_OEM_LABEL_CALLBACK						
		OEMStyleLabelCallback(frame, label, cookie );
	#else
		#ifdef STYLE_OVERRIDE_LABEL_CALLBACK
		 	OverRideStyleLabelCallback(frame, label, cookie );
		#endif
	#endif		
}
/**
    Function to set the first display page

    @param VOID

    @retval VOID

**/
VOID StyleSelectFirstDisplayPage( VOID )
{
	#ifdef STYLE_OEM_FIRST_PAGE						
		OEMStyleSelectFirstDisplayPage();
	#else
		#ifdef STYLE_OVERRIDE_FIRST_PAGE
		 	OverRideStyleSelectFirstDisplayPage();
		#endif
	#endif	
}
/**
    Function to add extra pages

    @param VOID

    @retval VOID

**/
VOID StyleAddExtraPagesData(VOID)
{
	#ifdef STYLE_OEM_ADD_EXTRA_PAGES
		OEMStyleAddExtraPagesData();
	#else
		#ifdef STYLE_OVERRIDE_ADD_EXTRA_PAGES
			OverRideStyleAddExtraPagesData();
		#endif
	#endif	
}
/**
    Function to get text mode

    @param Rows UINTN Cols 

    @retval UNNTN mode

**/
UINTN StyleGetTextMode( UINTN Rows, UINTN Cols )
{
	EFI_STATUS Status = EFI_UNSUPPORTED ;
	UINTN i = 0;
	UINTN ModeRows = 0, ModeCols = 0;

	#ifdef STYLE_OEM_GET_TEXT_MODE
		i = OEMStyleGetTextMode( Rows, Cols );
	#else
		#ifdef STYLE_OVERRIDE_GET_TEXT_MODE
		    i =	OverRideStyleGetTextMode( Rows, Cols);
		#else
		// Default Implementation
		for ( i = 0; i < (UINTN)gST->ConOut->Mode->MaxMode; i++ )
		{
			Status = gST->ConOut->QueryMode( gST->ConOut, i, &ModeCols, &ModeRows );
			if ( EFI_ERROR( Status ) )
				continue;

			if ( ( Cols == ModeCols ) && ( Rows == ModeRows ) )
				return i;
		}

		// return MaxMode if the mode wasn't found
		#endif
	#endif	
	
	return i;
}
/**
    Function to set the version string 

    @param VOID

    @retval VOID

**/
VOID StyleUpdateVersionString( VOID )
{
	#ifdef STYLE_OEM_VERSION_STRING
		OEMStyleUpdateVersionString();
	#else
		#ifdef STYLE_OVERRIDE_VERSION_STRING
		 	OverRideStyleUpdateVersionString();
		#endif
	#endif	
}
/**
    Function to set control positioning in a frame

    @param frame UINT32 *pOtherEnd

    @retval VOID

**/
VOID StyleFrameSetControlPositions(FRAME_DATA *frame, UINT32 *pOtherEnd)
{
	UINT32 u32Height = 0;
	UINT32 i;
	CONTROL_DATA *control;
  	UINT8 u8ChkResult;
	
	#ifdef STYLE_OEM_FRAME_CONTROL_POSITION
		OEMStyleFrameSetControlPositions(frame, pOtherEnd);
	#else
		#ifdef STYLE_OVERRIDE_FRAME_CONTROL_POSITION
		 	OverRideStyleFrameSetControlPositions(frame, pOtherEnd);
		#else
			if(frame->PageID != 0 )
	{
		//Move reference end variable based on action
		if(pOtherEnd == &(frame->LastVisibleCtrl))
		{
			i = frame->FirstVisibleCtrl;
		}
		else
		{
			i = frame->LastVisibleCtrl;
		}
		
		//Calculate other end variable
		while( (pOtherEnd == &(frame->LastVisibleCtrl)) ? (i < frame->ControlCount) : TRUE )
		{
			control = frame->ControlList[i];

			//Support grayout condition for readonly controls
            u8ChkResult = CheckControlCondition( &(control->ControlData) );
            	        
			if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
			{
        		if( (u32Height + control->Height) < frame->FrameData.Height )
        		{
        			*pOtherEnd = i;
        			u32Height += control->Height;
        		}
        		else
        			break; //We cant accommodate
			}
			
			if(pOtherEnd == &(frame->LastVisibleCtrl))
				i++;
			else
			{
				// i>=0 in while loop will result in infinite loop
				// break statement should be introduced inside
				if (i == 0)
					break;
				i--;
			}
		}
	}
	else
	{
		// frame is double sided in alternate1, let it be drawn
        frame->FirstVisibleCtrl=0;
		frame->LastVisibleCtrl = frame->ControlCount -1;
	}		
			
		#endif
	#endif	

}
/**
    Function to override component or control functionality

    @param VOID

    @retval VOID

**/
VOID StyleOverRideComponent(VOID)
{
	#ifdef STYLE_OEM_COMPONENT_OVERRIDE				// SDL
		OEMOverRideComponent();					// OEM
	#else
		#ifdef STYLE_COMPONENT_OVERRIDE				// from Ezport.h
			OverRideComponent();				// From ezport.c
		#endif
	#endif	
}
/**
    Function to initialize submenu control

    @param control UINT16 pageID, VOID *ref 

    @retval VOID

**/
VOID _StyleControlSubmenu( CONTROL_INFO *control, UINT16 pageID, VOID *ref )
{
	control->ControlHandle = gHiiHandle;
	control->ControlType = CONTROL_TYPE_SUBMENU;
	control->ControlFlags.ControlVisible = TRUE;
	control->ControlPtr = (VOID*)ref;
	control->ControlPageID = pageID;
	control->ControlDestPageID = 0;
}
/**
    Function to get Help area scroll support

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN StyleHelpAreaScrollable(VOID)
{
	if(IsHelpAreaScrollBarSupport())		
		return TRUE ;
	else
		return FALSE;
}	
/**
    Function to get shadow support

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN StyleGetShadowSupport(VOID)
{
	if(STYLE_SHADOW_SUPPORT)		
		return TRUE ;
	else
		return FALSE;
}
/**
    Function to get scroll behavior 

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN StyleGetScrollBehavior(VOID)
{
	return STYLE_SCROLLBAR_ROLLOVER ; // 0 - Item Limit, 1 - Rollover		

}		
/**
    Function to get date format

    @param VOID

    @retval BOOLEAN 

**/
UINTN StyleGetDateFormat(VOID)
{
	#ifdef STYLE_OEM_DATE_FOMAT
   		return OEMStyleGetDateFormat();
	#else
   		return STYLE_DATE_FORMAT;
	#endif
}
/**
    Function to show day in date format

    @param VOID

    @retval BOOLEAN 

**/
BOOLEAN StyleShowDay(VOID)
{
	return STYLE_SHOW_DAY ;		

}			
/**
    Function to max number of columns

    @param VOID

    @retval STYLE_STD_MAX_ROWS

**/
UINTN StyleGetStdMaxRows(VOID)
{
	return STYLE_STD_MAX_ROWS;
}
/**
    Function to get max number of rows

    @param VOID

    @retval STYLE_STD_MAX_COLS

**/
UINTN StyleGetStdMaxCols(VOID)
{
	return STYLE_STD_MAX_COLS;
}
/**
    Function to Get Clear Screen Color

    @param VOID

    @retval STYLE_CLEAR_SCREEN_COLOR

**/
UINTN StyleGetClearScreenColor(VOID)
{
	return STYLE_CLEAR_SCREEN_COLOR;
}
/**
    Function to PageLink (Submenu) color

    @param VOID

    @retval PAGE_LINK_FGCOLOR

**/
UINT8 StyleGetPageLinkColor(VOID)
{
	UINT32 PageNum = gApp->CurrentPage ;

	if( PageNum == 0 )
		return EFI_YELLOW;
	else
		return PAGE_LINK_COLOR;
}
/**
    Function to get scrollbar color

    @param VOID

    @retval FGCOLOR|BGCOLOR 

**/
UINT8 StyleGetScrollBarColor(VOID)
{
	return  (SCROLLBAR_FGCOLOR | SCROLLBAR_BGCOLOR) ;
}
/**
    Function to get scrollbar UpArrow Color

    @param VOID

    @retval FGCOLOR|BGCOLOR 

**/
UINT8 StyleGetScrollBarUpArrowColor(VOID)
{
	return  (SCROLLBAR_UPARROW_FGCOLOR | SCROLLBAR_UPARROW_BGCOLOR) ;
}
/**
    Function to get scrollbar downarrow color

    @param VOID

    @retval FGCOLOR|BGCOLOR

**/
UINT8 StyleGetScrollBarDownArrowColor(VOID)
{
	return  (SCROLLBAR_DOWNARROW_FGCOLOR | SCROLLBAR_DOWNARROW_BGCOLOR) ;
}
/**
    This function is called whenever a user navigates to a new page. 
    OEMs may use this hook to set item focus. 
    The default behavior sets focus to the previously focused page item. 

    @param page FRAME_DATA *frame
				
    @retval VOID

**/
VOID StylePageItemFocus(PAGE_DATA *page, FRAME_DATA *frame )
{
	
	#ifdef STYLE_OEM_PAGE_ITEM_FOCUS				
		OEMStylePageItemFocus(page, frame);
	#else
		#ifdef STYLE_OVERRIDE_PAGE_ITEM_FOCUS		
			OverRideStylePageItemFocus(page, frame);
		#else
		if(STYLE_PAGE_FIRSTITEM_FOCUS)
		{
			UINT32 i;
			for ( i = 0; i < frame->ControlCount; i++ )
			{
				
				if ( _FrameSetControlFocus( frame, i ) )
				{
					frame->FirstVisibleCtrl = 0;
					break;
				}
			}
			///fix - Update the Help based on the Focussed Item
			UpdatePageFocusedItemHelp(page);			
		}
		#endif
	#endif	
}
/**
    This function is called whenever a user returns (or navigates back) 
    from a submenu page causing a complete page redraw. OEMs may use this hook to set item focus. 
    The default behavior sets focus to the previously focused page item. 

    @param page FRAME_DATA *frame 
				
    @retval VOID

**/
VOID StyleSubPageItemFocus(PAGE_DATA *page, FRAME_DATA *frame )
{
	#ifdef STYLE_OEM_SUBPAGE_ITEM_FOCUS				// SDL
		OEMStyleSubPageItemFocus(page, frame);		// OEM
	#else
		#ifdef STYLE_OVERRIDE_SUBPAGE_ITEM_FOCUS		// from Ezport.h
			OverRideStyleSubPageItemFocus(page, frame);	// From ezport.c
		#else
		if(STYLE_SUBPAGE_FIRSTITEM_FOCUS)
		{
			UINT32 i;

			for ( i = 0; i < frame->ControlCount; i++ )
			{
				if ( _FrameSetControlFocus( frame, i ) )
				{
					frame->FirstVisibleCtrl = 0;
					break;
				}
			}
		}
		#endif
	#endif	 	
}
#endif
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
