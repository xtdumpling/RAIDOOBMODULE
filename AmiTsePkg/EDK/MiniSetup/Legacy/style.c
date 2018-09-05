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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Legacy/style.c $
//
// $Author: Rajashakerg $
//
// $Revision: 3 $
//
// $Date: 6/23/11 3:48p $
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
extern INT16 gTabIndex ;

VOID _StyleAddSpacing( FRAME_DATA *frame );
VOID GetTseBuildVersion(UINTN *TseMajor, UINTN *TseMinor, UINTN *TseBuild);
VOID GetTseBuildYear(UINT16 *TseBuildYear);

#if SETUP_STYLE_LEGACY



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
    Function to set label callback

    @param frame LABEL_DATA *label, VOID *cookie

    @retval VOID

**/
VOID OverRideStyleLabelCallback( FRAME_DATA *frame, LABEL_DATA *label, VOID *cookie )
{
	UINT16 token = 0		;

	if ( frame->PageID != 0 ) 
		return  ;

    	token = ((AMI_IFR_LABEL *)label->ControlData.ControlPtr)->Text; 

    	switch(token)
	{
		case STRING_TOKEN(STR_LOAD_FAILSAFE): 
			LoadFailsafeDefaults();
			break;
				
		case STRING_TOKEN(STR_LOAD_OPTIMAL):
			_SubMenuAmiCallback( RESTORE_DEFAULTS_VALUE ); 
			//LoadOptimalDefaults();
			break;	 
		
		case STRING_TOKEN(STR_SAVE_EXIT):
			_SubMenuAmiCallback( SAVE_AND_EXIT_VALUE ); 
			//HandleSaveAndExit();
			break;

		case STRING_TOKEN(STR_EXIT):
			_SubMenuAmiCallback( DISCARD_AND_EXIT_VALUE ); 
			//HandleExitApplication();
			break;	

		case STRING_TOKEN(STR_SAVE_RESET):
			_SubMenuAmiCallback( SAVE_AND_RESET_VALUE ); 
			//HandleSaveAndReset();
			break;	

		case STRING_TOKEN(STR_RESET):
			_SubMenuAmiCallback( DISCARD_AND_RESET_VALUE ); 	
			//HandleResetSys();
			break;	
	}
}
/**
    Function to handle action key

    @param frame EFI_INPUT_KEY Key

    @retval STATUS

**/
EFI_STATUS OverRideStyleFrameHandleKey( FRAME_DATA *frame, EFI_INPUT_KEY Key )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	INT32 prevControl = frame->CurrentControl;

	INT32 thisControl = prevControl;
	INT32 lastControl = frame->ControlCount - 1;
	CONTROL_DATA *control;

	if ( frame->PageID != 0 )
		return Status;

	if ( frame->ControlCount == 0 )
		return Status;

	switch ( Key.ScanCode )
	{
		case SCAN_LEFT:
			if ( thisControl - gTabIndex >= 0 )
				thisControl -= gTabIndex;
			else if ( thisControl + gTabIndex <= lastControl )
				thisControl += gTabIndex;
			break;
		case SCAN_RIGHT:
			if ( thisControl + gTabIndex <= lastControl )
				thisControl += gTabIndex;
			else if ( thisControl - gTabIndex >= 0 )
				thisControl -= gTabIndex;
			break;
		default:
			return Status;
	}

	control = frame->ControlList[prevControl];
	control->Methods->SetFocus( control, FALSE );

	if ( _FrameSetControlFocus( frame, thisControl ) )
	{
		Status = EFI_SUCCESS;
		frame->CurrentControl = thisControl;
	}

	return Status;

}
/**
    Function to handle contro overflow

    @param frame CONTROL_DATA *control, UINT16 count

    @retval VOID

**/
EFI_STATUS OverRideStyleHandleControlOverflow( FRAME_DATA *frame, CONTROL_DATA *control, UINT16 count )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT16 top, left;
	CONTROL_DATA *tempcontrol;
	UINT16 i=0,Height=0;

	if ( frame->PageID != 0 )
	{
		// Add scrollbar if not present linking it with the frame 0 thorugh a callback
		for(i = 0; i < count; i++)
		{
			tempcontrol = (CONTROL_DATA*)(frame->ControlList[i]);
          		if( (Height + tempcontrol->Height ) < frame->FrameData.Height )
		  	{
			  	Height = Height + tempcontrol->Height;
              			frame->LastVisibleCtrl = i;
		  	}
		  	else
		  	{
//		    		frame->UseScrollbar = TRUE;
				Status = EFI_SUCCESS;
            			break;
		  	}
		}
		return Status;
	}
    	
	frame->UseScrollbar = FALSE;
	Status = EFI_SUCCESS;

	//_StyleAddSpacing( frame );	

    //Double Space layout
    count = count*2 ;// + (UINT16)(frame->NullCount) ;
	left = frame->FrameData.Left + (UINT8)gLabelLeftMargin;
	top = frame->FrameData.Top + (UINT16)(count - 2) ;
	
	if(count > frame->FrameData.Height)
	{
		left = frame->FrameData.Left + (UINT8)gControlLeftPad + (UINT16)(gMaxCols / 2);
		top = frame->FrameData.Top + (UINT16)(count - 1 - frame->FrameData.Height)  ;
	}else
		gTabIndex =  frame->ControlCount ;

	control->Methods->SetPosition( control, left, top );
    count = frame->ControlCount ;

    //If the double spaced layout exceeds the left + right columns,
    //reflow the layout using single spacing. 
	if((count*2) > (frame->FrameData.Height*2))
	{
		CONTROL_DATA **Control=NULL;
        Control = frame->ControlList;
        top = frame->FrameData.Top ;
		left = frame->FrameData.Left + (UINT8)gLabelLeftMargin;

		//Update the column tab index
		gTabIndex = (UINT32)((frame->ControlCount)/2) ;

		//Update using single space layout 
        for(i = 0; i< frame->ControlCount; i++, Control++ )
		{
			(*Control)->Methods->SetPosition( *Control, left, top );
			frame->LastVisibleCtrl = i ;
			top++;
			
			if( top > (frame->FrameData.Top + gTabIndex))
			{
				left = frame->FrameData.Left + (UINT8)gControlLeftPad +(UINT16)(gMaxCols / 2);
				top = frame->FrameData.Top ;
			}
		}
	}
	frame->LastVisibleCtrl = frame->ControlCount -1;
	Status = EFI_SUCCESS;
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
	
	//CONTROL_INFO dummy;

	UINT32 FrameType = frame->FrameData.FrameType ;
	
	saveColors = Colors ;

	//Allows controls to match the Background Color of the Frame
	Colors.BGColor		=	frame->FrameData.BGColor ;	
	Colors.SecBGColor	=	frame->FrameData.BGColor ;
	Colors.SelBGColor	=	frame->FrameData.BGColor ;
	Colors.NSelBGColor	=	frame->FrameData.BGColor ;
	Colors.LabelBGColor	=	frame->FrameData.BGColor ;
	//Colors.EditBGColor	=	frame->FrameData.BGColor ;
	Colors.PopupBGColor 	=	frame->FrameData.BGColor ;

	switch ( FrameType )
	{
		case MAIN_FRAME:

			if ( frame->PageID == 0 )
			{
				Colors.SelFGColor	=	EFI_WHITE ;
				Colors.SelBGColor	=	EFI_BACKGROUND_RED ;
				Colors.NSelFGColor	=   	EFI_YELLOW ; 
				Colors.NSelLabelFGColor =	EFI_YELLOW ;
				Colors.LabelFGColor 	=	EFI_WHITE ;
				Colors.PopupFGColor 	=	POPUP_FGCOLOR ;
				Colors.PopupBGColor 	=	POPUP_BGCOLOR ;
			}else{

				if(LABEL_FOCUS_COLOR != FOCUS_COLOR)
					Colors.LabelFGColor = LABEL_FOCUS_COLOR ;

				if(CONTROL_FOCUS_COLOR != FOCUS_COLOR)	
					Colors.SelFGColor = CONTROL_FOCUS_COLOR;

				if(LABEL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
					Colors.NSelLabelFGColor = LABEL_NON_FOCUS_COLOR ;

				if(CONTROL_NON_FOCUS_COLOR != NON_FOCUS_COLOR)
					Colors.NSelFGColor = CONTROL_NON_FOCUS_COLOR ;

				Colors.SelBGColor = CONTROL_FOCUS_BGCOLOR ;
				Colors.EditBGColor = CONTROL_FOCUS_BGCOLOR ;
			}

			Status = _FrameAddControls( frame, data );
			break;

		case TITLE_FRAME:
		case SUBTITLE_FRAME:
		case HELPTITLE_FRAME:
			justify = JUSTIFY_CENTER;
		case HELP_FRAME:
			if ( frame->PageID == 0 )
				justify = JUSTIFY_CENTER;
		case NAV_FRAME:
            if ( FrameType == NAV_FRAME)
				justify = JUSTIFY_CENTER;
			Status = _FrameAddTitle( frame, FrameType, data );
			if ( ! EFI_ERROR( Status ) )
			{
				MEMO_DATA *memo = (MEMO_DATA *)frame->ControlList[0];
				//To maintain same functionality in all flavours,We have modified below code to handle memory Leak.
				if(FrameType == HELPTITLE_FRAME )
				{
					void *HelpTitle=NULL;
					HelpTitle=EfiLibAllocateZeroPool(sizeof(EFI_IFR_SUBTITLE));
					MemCpy(HelpTitle, memo->ControlData.ControlPtr, sizeof(EFI_IFR_SUBTITLE));
					memo->ControlData.ControlPtr= HelpTitle;
				}
				gMemo.SetJustify( memo, justify );
			}
			break;
		case SCROLLBAR_FRAME:
			// not implemented at this time
			Status = EFI_SUCCESS;
			break;
		default:
			break;
	}

	// initialize frame scroll colors
	frame->FrameData.ScrlFGColor = (frame->FrameData.FGColor == 0) ? Colors.SecFGColor:frame->FrameData.FGColor ;
	frame->FrameData.ScrlBGColor = (frame->FrameData.BGColor == 0) ? Colors.SecBGColor : frame->FrameData.BGColor  ;



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
	return gStrNavToken;
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
	UINT8 top=0, left=0;
	UINT16 i=0, count = 0;

	
	if ( frame->PageID != 0 ) 
		return Status;


//	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gFailSafeLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gOptimalLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gSaveExitLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gExitLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;

	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gSaveResetLabel, &controlNumber, &focus );
	if ( EFI_ERROR( Status ) )
		return Status;
	
	_StyleAddSpacing( frame );
	Status = _StyleAddControl( frame, CONTROL_TYPE_LABEL, &gResetLabel, &controlNumber, &focus );

    control = frame->ControlList;
    count = frame->ControlCount	;

	//Note: Initially, the root page layout of controls is from top->bottom left->right column with single spacing.
	//If the initial layout spans more than the left and right column, update the layout using no spacing.

	return Status;

}
/**
    Function to set first page display

    @param VOID

    @retval VOID

**/
VOID OverRideStyleSelectFirstDisplayPage( VOID )
{
	gApp->CurrentPage = 0;
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
			//Source modules in TSE should get the version details from binary in run time not at build time
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
			// Use BIOS build year for copyright message
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
	EFI_STATUS	Status = EFI_SUCCESS;
			
	if(!frame->FrameData.Drawable){
		Status = EFI_UNSUPPORTED;
	}
	return Status ;	
}
/**
    Function to add spacing

    @param frame 

    @retval VOID

**/
VOID _StyleAddSpacing( FRAME_DATA *frame )
{
	_StyleAddControl( frame, CONTROL_TYPE_NULL, NULL, NULL, NULL );
	//if ( frame->ControlCount == 7 )
	//	_StyleAddControl( frame, CONTROL_TYPE_NULL, NULL, NULL, NULL );
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
