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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/EzportPlus/style.c $
//
// $Author: Rajashakerg $
//
// $Revision: 2 $
//
// $Date: 6/23/11 5:46a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file style.c
    Contains style override functions.

**/
//**********************************************************************
#include "minisetup.h"

extern UINT16 gVerticalMainDivider ;
extern UINT16 gHorizontalHelpDivider ;
extern UINT16 gStrNavToken;

VOID _StyleAddSpacing( FRAME_DATA *frame );
VOID GetTseBuildVersion(UINTN *TseMajor, UINTN *TseMinor, UINTN *TseBuild);
VOID GetTseBuildYear(UINT16 *TseBuildYear);

#if SETUP_STYLE_EZPORTPLUS



STYLECOLORS  Colors =
{
	TEXT_COLOR, 			//EFI_WHITE,	//FGColor
	PAGE_BGCOLOR,					//BGColor
	NON_FOCUS_COLOR, 		//EFI_BLUE,	//secondary FGcolor
	PAGE_BGCOLOR,					//secondary BGColor
	CONTROL_FOCUS_COLOR, 		//EFI_WHITE,	//SelFGColor
	PAGE_BGCOLOR,					//SelBGColor
	CONTROL_NON_FOCUS_COLOR,	//EFI_BLUE,	//NSelFGColor
	PAGE_BGCOLOR,					//NSelBGColor
	PAGE_BGCOLOR,					//LabelBGColor
	LABEL_FOCUS_COLOR,		//EFI_WHITE,	//LabelFGColor
	LABEL_NON_FOCUS_COLOR,		//EFI_BLUE,	//NSelLabelFGColor
	EDIT_BGCOLOR,					//EditBGColor
	EDIT_FGCOLOR,		 	//EFI_WHITE,	//EditFGColor
	POPUP_FGCOLOR,					//popupFGColor
	POPUP_BGCOLOR					//popupBGColor
};
/**
    Function to handle contro overflow

    @param frame CONTROL_DATA *control, UINT16 count

    @retval VOID

**/
EFI_STATUS OverRideStyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
  	CONTROL_DATA *tempcontrol;
  	UINT16 i=0,Height=0;
  	UINT8	BORDER=(frame->FrameData.Border==TRUE)?1:0 ;

    	// show scrollbar if neccessary
    	frame->UseScrollbar = FALSE;

	for(i = 0; i < count; i++)
	{
		tempcontrol = (CONTROL_DATA*)(frame->ControlList[i]);
	        if( (Height + tempcontrol->Height ) <= frame->FrameData.Height-2*BORDER )
	        {
	            Height = Height + tempcontrol->Height;
	            frame->LastVisibleCtrl = i;
	        }
	        else
	        {
	            Status = EFI_SUCCESS;
	            break;
	        }
	}

	return Status;
}

static AMI_IFR_MENU gMenuData = {CONTROL_TYPE_MENU,2};
/**
    Function to initialize frame

    @param frame UINT32 frameType, PAGE_INFO *data

    @retval VOID

**/
EFI_STATUS OverRideStyleInitializeFrame( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data )
{
	EFI_STATUS	Status = EFI_INVALID_PARAMETER;
	UINT8	justify = JUSTIFY_LEFT /*, BORDER=(frame->FrameData.Border==TRUE)?1:0*/ ;
	STYLECOLORS saveColors ;
	
	CONTROL_INFO dummy;

	UINT32 FrameType = frame->FrameData.FrameType ;
	
	saveColors = Colors ;

	//Allows controls to match the Background Color of the Frame
	Colors.BGColor		=	frame->FrameData.BGColor ;	
	Colors.SecBGColor	=	frame->FrameData.BGColor ;
	Colors.SelBGColor	=	frame->FrameData.BGColor ;
	Colors.NSelBGColor	=	frame->FrameData.BGColor ;
	Colors.LabelBGColor	=	frame->FrameData.BGColor ;
	Colors.PopupBGColor =	frame->FrameData.BGColor ;

	switch ( FrameType )
	{
		case MAIN_FRAME:
			if(LABEL_FOCUS_COLOR != FOCUS_COLOR)
				Colors.LabelFGColor = LABEL_FOCUS_COLOR ;

			if(CONTROL_FOCUS_COLOR != FOCUS_COLOR)
				Colors.SelFGColor = CONTROL_FOCUS_COLOR;

			if(LABEL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
				Colors.NSelLabelFGColor = LABEL_NON_FOCUS_COLOR ;

			if(CONTROL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
				Colors.NSelFGColor = CONTROL_NON_FOCUS_COLOR ;
			
            if (frame->PageID == FIRST_PAGE_ID_INDEX)
            {
                // Reset frame width
                frame->FrameData.Width = (UINT16)(gMaxCols - gLabelLeftMargin - 1);

                // Reset clear frame area width
                gBS->FreePool(frame->BlankLine);
                frame->BlankLine = EfiLibAllocateZeroPool( sizeof(CHAR16) * (frame->FrameData.Width + 1) );
                if ( frame->BlankLine != NULL )
                    MemFillUINT16Buffer( frame->BlankLine, frame->FrameData.Width, L' ' );
            }
			Status = _FrameAddControls( frame, data );
			break;

		case SUBTITLE_FRAME:
			Colors.SecBGColor	=	SELECTED_MENU_BGCOLOR ;
			Colors.SecFGColor	=	SELECTED_MENU_FGCOLOR ;
			Colors.PopupBGColor =	MENU_BGCOLOR ;
			Colors.PopupFGColor =	MENU_FGCOLOR ;

	        MemSet( &dummy, sizeof(dummy), 0 );
	        dummy.ControlHandle = gHiiHandle;
			dummy.ControlType = CONTROL_TYPE_MENU;
	        dummy.ControlPtr = (VOID*)&gMenuData;
			dummy.ControlPageID = data->PageID; 
			dummy.ControlFlags.ControlVisible = TRUE;
	        Status=gFrame.AddControl(frame,&dummy);
			break;

		case TITLE_FRAME:

		
        case NAV_FRAME:
             
			 justify = JUSTIFY_CENTER;
			Status = _FrameAddTitle( frame, FrameType, data );
			if ( ! EFI_ERROR( Status ) )
			{
				MEMO_DATA *memo = (MEMO_DATA *)frame->ControlList[0];
				
				if(FrameType == NAV_FRAME )
				{
                    memo->ControlData.ControlPtr= (VOID*) UefiCreateSubTitleTemplate(STRING_TOKEN(STR_NAV_STRINGS_5));
					if ( gMaxCols <= 80 )
					{
                    	memo->ControlData.ControlPtr= (VOID*) UefiCreateSubTitleTemplate(STRING_TOKEN(STR_NAV_STRINGS_6));
					}
				}
				if (FrameType == HELPTITLE_FRAME)
					justify = JUSTIFY_CENTER;   
				gMemo.SetJustify( memo, justify );
			}

			 break;
		case HELP_FRAME:

			 justify = JUSTIFY_LEFT;
		case HELPTITLE_FRAME:
			Status = _FrameAddTitle( frame, FrameType, data );
			if ( ! EFI_ERROR( Status ) )
			{
				MEMO_DATA *memo = (MEMO_DATA *)frame->ControlList[0];
				
				if(FrameType == NAV_FRAME )
				{
                    memo->ControlData.ControlPtr= (VOID*) UefiCreateSubTitleTemplate(STRING_TOKEN(STR_NAV_STRINGS_5));
					if ( gMaxCols <= 80 )
					{
                    	memo->ControlData.ControlPtr= (VOID*) UefiCreateSubTitleTemplate(STRING_TOKEN(STR_NAV_STRINGS_6));
					}
				}
				//To maintain same functionality in all flavours,We have modified below code to handle memory Leak.
				if (FrameType == HELPTITLE_FRAME)
				{
					void *HelpTitle=NULL;
					HelpTitle=EfiLibAllocateZeroPool(sizeof(EFI_IFR_SUBTITLE));
					MemCpy(HelpTitle, memo->ControlData.ControlPtr, sizeof(EFI_IFR_SUBTITLE));
					memo->ControlData.ControlPtr= HelpTitle;
					justify = JUSTIFY_CENTER;   
				}  
				gMemo.SetJustify( memo, justify );
			}
			break;

		case SCROLLBAR_FRAME:
			// not implemented at this time
			Status = EFI_SUCCESS;
			break;

		default:
			//Status = StyleInitializeOemFrame( frame, frame->FrameData.FrameType, data ) ;
			break;
	}


	Colors = saveColors;

	// initialize frame scroll colors
	frame->FrameData.ScrlFGColor = SCROLLBAR_FGCOLOR ; 
	frame->FrameData.ScrlBGColor = SCROLLBAR_BGCOLOR ; 
	frame->FrameData.ScrlUpFGColor = SCROLLBAR_UPARROW_FGCOLOR ; 
	frame->FrameData.ScrlUpBGColor = SCROLLBAR_UPARROW_BGCOLOR ;
	frame->FrameData.ScrlDnFGColor = SCROLLBAR_DOWNARROW_FGCOLOR ; 
	frame->FrameData.ScrlDnBGColor = SCROLLBAR_DOWNARROW_BGCOLOR ;

	return Status;

}
/**
    Function to get navigation frame token

    @param page number

    @retval UINT16 token 

**/
UINT16 OverRideStyleGetNavToken( UINT32 page )
{
    UINT16 Token=0; 
	
	Token = STRING_TOKEN(STR_NAV_STRINGS_5);
	
    if ( gMaxCols <= 80 )
	{
	    	Token = STRING_TOKEN(STR_NAV_STRINGS_6);
	}
    
    return Token ;//    return gStrNavToken;
}



static AMI_IFR_LABEL gFailSafeLabel = { 0, 0, STRING_TOKEN(STR_LOAD_FAILSAFE), STRING_TOKEN(STR_LOAD_FAILSAFE_HELP) };
static AMI_IFR_LABEL gOptimalLabel = { 0, 0, STRING_TOKEN(STR_LOAD_OPTIMAL), STRING_TOKEN(STR_LOAD_OPTIMAL_HELP) };
static AMI_IFR_LABEL gSaveExitLabel = { 0, 0, STRING_TOKEN(STR_SAVE_EXIT), STRING_TOKEN(STR_SAVE_EXIT_HELP) };
static AMI_IFR_LABEL gExitLabel = { 0, 0, STRING_TOKEN(STR_EXIT), STRING_TOKEN(STR_EXIT_HELP) };
static AMI_IFR_LABEL gSaveResetLabel = { 0, 0, STRING_TOKEN(STR_SAVE_RESET), STRING_TOKEN(STR_SAVE_RESET_HELP) };
static AMI_IFR_LABEL gResetLabel = { 0, 0, STRING_TOKEN(STR_RESET), STRING_TOKEN(STR_RESET_HELP) };

/**
    Function to add additional controls

    @param frame UINT32 controlNumber, BOOLEAN focus 

    @retval EFI_STATUS

**/
EFI_STATUS OverRideStyleAddAdditionalControls( FRAME_DATA *frame, UINT32 controlNumber, BOOLEAN focus )
{
	EFI_STATUS Status = EFI_SUCCESS;
	CONTROL_DATA **control;
	UINT32	i;

	if ( frame->PageID != 0 ) 
		return Status;

	control = frame->ControlList;
	  
	for ( i = 0; i < frame->ControlCount; i++, control++ )
	{
		(*control)->Methods->Destroy( *control, TRUE );
	}

	// initialize counter of control in frame and set focus to false (at this point the are no controls in the frame)
	frame->ControlCount =0;
	frame->NullCount=0;
	focus=0;
	controlNumber=0;
	
	// add controls for boot manager, language, failsafe, optimal, save & exit, exit without saving
/*	Status = _StyleAddControl( frame, CONTROL_TYPE_SUBMENU, &gBootManagerRef, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	Status = _StyleAddControl( frame, CONTROL_TYPE_SUBMENU, &gLanguageRef, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;
*/
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gFailSafeLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gOptimalLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gSaveExitLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gExitLabel, &controlNumber, &focus );

	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gSaveResetLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gResetLabel, &controlNumber, &focus );

	return Status;
}
/**
    Function to set first page display

    @param VOID

    @retval VOID

**/
VOID OverRideStyleSelectFirstDisplayPage( VOID )
{
	gApp->CurrentPage = 1;
}
/**
    Function to update version string

    @param VOID

    @retval VOID

**/
VOID OverRideStyleUpdateVersionString( VOID )
{
	CHAR16 *string = NULL, *newString = NULL;
	UINTN length, TseMajor,TseMinor, TseBuild;
	UINT16 TseBuildYear=0;
	UINTN i;

	for ( i = 0; i < gLangCount; i++ )
	{
		string = HiiGetStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_COPYRIGHT), gLanguages[i].Unicode );
		if ( string == NULL )
			return;

		length = EfiStrLen(string) + 10;
		newString = EfiLibAllocateZeroPool( length * sizeof(CHAR16) );
		if ( newString != NULL )
		{
			// Source modules in TSE should get the version details from binary in run time not at build time
			GetTseBuildVersion(&TseMajor,&TseMinor,&TseBuild);
			// Use BIOS build year for copyright message
			GetTseBuildYear(&TseBuildYear);
			SPrint( newString, length * sizeof(CHAR16), string, TseMajor, TseMinor, TseBuild,TseBuildYear );
			HiiChangeStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_COPYRIGHT), gLanguages[i].Unicode, newString );
		}

		MemFreePointer( (VOID **)&newString );
		MemFreePointer( (VOID **)&string );
		// Use BIOS build year for copyright message
		string = HiiGetStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_TITLE), gLanguages[i].Unicode );
		if ( string == NULL )
			return;
		length = EfiStrLen(string) + 10;
		newString = EfiLibAllocateZeroPool( length * sizeof(CHAR16) );
		if ( newString != NULL )
		{
			//Use BIOS build year for copyright message
			GetTseBuildYear(&TseBuildYear);
			SPrint( newString, length * sizeof(CHAR16), string, TseBuildYear );
			HiiChangeStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_TITLE), gLanguages[i].Unicode, newString );
		}
		MemFreePointer( (VOID **)&newString );
		MemFreePointer( (VOID **)&string );
	}
}
/**
    Function to determine if frame is drawable

    @param frame 

    @retval EFI_STATUS

**/
EFI_STATUS OverRideStyleFrameDrawable( FRAME_DATA *frame )
{
	EFI_STATUS Status = EFI_SUCCESS;
    static UINTN TempRightAreaWidth = 0;
    
    if (frame->PageID == FIRST_PAGE_ID_INDEX)
    {
        
        switch ( frame->FrameData.FrameType )
        {            
            // Hidden NAV and HELP frame
            case HELP_FRAME:
            case HELPTITLE_FRAME:
                Status = EFI_UNSUPPORTED;
                break;

            // Set right area width
            case MAIN_FRAME:
                if (TempRightAreaWidth == 0)
                    {
                        // Set right area width
                        TempRightAreaWidth = gControlRightAreaWidth;
                        gControlRightAreaWidth = gMaxCols - gControlLeftMargin - gLabelLeftMargin - 2;
                    }
                break;
        }

        return Status;
    }
    if (TempRightAreaWidth != 0)
    {
        gControlRightAreaWidth = TempRightAreaWidth;
        TempRightAreaWidth = 0;
    }
    return Status;
}
/**
    Function to add spacing

    @param frame 

    @retval VOID

**/
VOID _StyleAddSpacing( FRAME_DATA *frame )
{
	_StyleAddControl( frame, CONTROL_TYPE_NULL, NULL, NULL, NULL );
	if ( frame->ControlCount == 7 )
		_StyleAddControl( frame, CONTROL_TYPE_NULL, NULL, NULL, NULL );
}
/**
    Function to add controls

    @param frame UINT16 type, VOID *data, UINT32 *controlNumber, BOOLEAN *focus

    @retval EFI_STATUS

**/
EFI_STATUS _StyleAddControl( FRAME_DATA *frame, UINT16 type, VOID *data, UINT32 *controlNumber, BOOLEAN *focus )
{
	EFI_STATUS Status;
	CONTROL_INFO dummy;

	MemSet( &dummy, sizeof(dummy), 0 );
	dummy.ControlHandle = (VOID*)(UINTN)INVALID_HANDLE;

	dummy.ControlType = type;
	dummy.ControlPtr = (VOID*)data;
	dummy.ControlFlags.ControlVisible = TRUE;

	Status = gFrame.AddControl( frame, &dummy );
	if ( EFI_ERROR( Status ) )
		return Status;

	if ( type != CONTROL_TYPE_NULL )
	{
		if ( ! *focus )
		{
			if ( _FrameSetControlFocus( frame, *controlNumber ) )
			{
				frame->CurrentControl = *controlNumber;
				*focus = TRUE;
			}
		}

		(*controlNumber)++;
	}

	return Status;
}
#endif
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
