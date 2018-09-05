//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/frame.c $
//
// $Author: Premkumara $
//
// $Revision: 47 $
//
// $Date: 5/28/12 12:31p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file frame.c
    This file contains code to handle frame operations

**/

#include "minisetup.h"

EFI_STATUS _FrameHandleKey( FRAME_DATA *frame, AMI_EFI_KEY_DATA Key );
EFI_STATUS _FrameScroll( FRAME_DATA *frame, BOOLEAN bScrollUp );
BOOLEAN _FrameSetControlFocus( FRAME_DATA *frame, UINT32 index );
EFI_STATUS _PreviousFrame (FRAME_DATA *frame);
EFI_STATUS _NextFrame (FRAME_DATA *frame);
EFI_STATUS _FocusFirst (FRAME_DATA *frame);
EFI_STATUS _FocusLast (FRAME_DATA *frame);
EFI_STATUS FrameDrawScrollBar (FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32	modVal, UINT32	sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar);
EFI_STATUS UpdateDestiantionQuestion (UINT32, UINT32, UINT32 *);
EFI_STATUS TSEMouseHelpFrameHandleAction( FRAME_DATA *frame, ACTION_DATA *action,CONTROL_DATA **control);
VOID _HelpFrameScroll(MEMO_DATA *memo,BOOLEAN bScrollUp);
BOOLEAN IsMouseSupported(VOID);
VOID CollectReconnectHandles();
INT32 lButtonDownVaringPosition = 0;
INT32 lButtonDownInitialPosition = 0;
static UINT32 gFrameScrollBarTop = 0, gFrameScrollBarBottom = 0;
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
FRAME_METHODS gFrame =
{
	(OBJECT_METHOD_CREATE)FrameCreate,
	(OBJECT_METHOD_DESTROY)FrameDestroy,
	(OBJECT_METHOD_INITIALIZE)FrameInitialize,
	(OBJECT_METHOD_DRAW)FrameDraw,
	(OBJECT_METHOD_HANDLE_ACTION)FrameHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)FrameSetCallback,
	FrameAddControl
};
//--------------------------------------------------------------------------

/**
    Allocated memory size and creates frame 

    @param object 

        EFI_STATUS status - 
        EFI_SUCCESS - 
    @retval EFI_OUT_OF_RESOURCES if the object is NULL
    @retval EFI_ERROR If there are any the error in the object
        creating process

**/
EFI_STATUS FrameCreate( FRAME_DATA **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(FRAME_DATA) );

		if ( *object == NULL )
			return EFI_OUT_OF_RESOURCES;
	}

	Status = gObject.Create( (void ** )object );

	if (EFI_ERROR(Status))
	{
		MemFreePointer( (VOID **)object );
		return Status;
	}

	(*object)->Methods = &gFrame;

	return Status;
}

/**
    Destroys an existing frame and frees memory

    @param frame Frame to be destroyed
    @param freeMem Frees frame memory if TRUE, ignores
        otherwise

    @retval EFI_STATUS EFI_SUCCESS

**/

EFI_STATUS FrameDestroy( FRAME_DATA *frame, BOOLEAN freeMem )
{
	CONTROL_DATA **control;
	UINT32	i;

	if(NULL == frame)
	  return EFI_SUCCESS;
	control = frame->ControlList;
	for ( i = 0; i < frame->ControlCount; i++, control++ )
	{
		if((*control)->ParentFrameType == HELPTITLE_FRAME)
		{
			MemFreePointer( (VOID **)&((*control)->ControlData.ControlPtr) );
		}
		if (*control)			//Destroy the controls if it is not NULL
			(*control)->Methods->Destroy( *control, TRUE );
	}

	frame->ControlCount = 0;

	MemFreePointer( (VOID **)&frame->ControlList );
	MemFreePointer( (VOID **)&frame->BlankLine );
	MemFreePointer( (VOID **)&frame->OrigPosition );
	//MemFreePointer( (VOID **)&frame->CurrPosition );

	if ( freeMem )
		MemFreePointer( (VOID **)&frame );

	return EFI_SUCCESS;
}

/**
    Initializes the frame data

    @param frame Frame to be intialized
    @param data Related frame data 

    @retval EFI_STATUS EFI_SUCCESS

**/
EFI_STATUS FrameInitialize( FRAME_DATA *frame, FRAME_INFO *data )
{
	if ( data != NULL )
	{
		MemCopy( &frame->FrameData, data, sizeof(FRAME_INFO) );
		if ( frame->PageID != 0 )
		{
			frame->BlankLine = EfiLibAllocateZeroPool( sizeof(CHAR16) * (data->Width + 1) );
			if ( frame->BlankLine != NULL )
				MemFillUINT16Buffer( frame->BlankLine, data->Width, L' ' );
		}
	}

	frame->FirstVisibleCtrl = 0;
	return EFI_SUCCESS;
}

/**
    To know whether the scroll supprot required befor drawing the frame

    @param frame Frame information to get frame info

    @retval BOOLEAN TRUE/FALSE

**/
BOOLEAN	IsScrollSupportRequired(FRAME_DATA *frame )
{
	CONTROL_DATA **control;
	UINT16 i, Height;
	UINT8 u8ChkResult;

	control = frame->ControlList;
	Height = frame->FrameData.Top;

	for ( i = 0; i < frame->ControlCount; i++, control++ )
	{

		u8ChkResult = CheckControlCondition( &((*control)->ControlData) ); //Check conditional

		if(u8ChkResult == COND_NONE || u8ChkResult == COND_GRAYOUT) {
		//TSE_MULTILINE_CONTROLS moved to binary
		if(IsTSEMultilineControlSupported())
		{
			if(frame->FrameData.FrameType == MAIN_FRAME)
			{
				UINT16 height=0;
				(*control)->Methods->GetControlHeight( *control, (VOID*)frame,&height);
				height = height ? height:1;
				(*control)->Methods->SetDimensions( *control, 
										frame->FrameData.Width - (UINT8)gLabelLeftMargin - 2, 
										height );
			}
		}         
			Height = Height + (*control)->Height;
		}
	}
	
	//check whether scroll support required.
	if((frame->UseScrollbar) && ((frame->FrameData.Top + frame->FrameData.Height) >= Height+1))
	{
		return FALSE;
	}

	return TRUE;
}

/**
    Draws the controls for the given frame 

    @param frame Frame information to draw controls

    @retval EFI_STATUS EFI_SUCCESS if successful, else EFI_ERROR

**/
EFI_STATUS FrameDraw( FRAME_DATA *frame )
{
	CONTROL_DATA **control;
	UINT32	i, j,FirstLine=0, LastLine=0, topPos=0, lastPos=0, count;
	UINT32 visibleControlHeight=0, visibleControlCount = 0;
	UINT32 modVal=0, sizeOfBar = 0, numOfBlocks = 0;
	UINT32 CurrentvisibleControlCount = 0, CurrentFirstvisibleControlCount = 0, MltCnt=0 ,FrameLastVisibleControl=0;
	UINT16 CurrHeight, ControlTop;
    UINT8 u8ChkResult;
    UINT16 Height = 0;

    BOOLEAN bCurrNotFocused = FALSE,bEraseScrollBar=FALSE;

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
		
		u8ChkResult = CheckControlCondition( &((*control)->ControlData) );
		
		//Setting focus to control if no control-condition is present and token TSE_SETUP_GRAYOUT_SELECTABLE is enable to make GrayoutIf control to focus and selectable
		if ( (COND_NONE == u8ChkResult) || (IsGrayoutSelectable() && (COND_GRAYOUT == u8ChkResult)) )
		{
          // Control might be dynamically updated. Get the actual control
          if(*control == NULL)
          {
            gApp->CompleteRedraw = TRUE;
            return EFI_UNSUPPORTED;
          }

			// Set the Focus to current control
			if((*control)->Methods->SetFocus(*control,TRUE) !=  EFI_SUCCESS)
			{
				
				//Fix to allow Help Frame scroll support
				if(frame->FrameData.FrameType != HELP_FRAME)
				{ 
					frame->CurrentControl = FRAME_NONE_FOCUSED;
					bCurrNotFocused = TRUE; // Non is focused
				}
			}
		}
	}

    //There is no active control we have to draw all in the view.

   	// First, clear the frame.
	if ( frame->BlankLine != NULL )
	{
		j = frame->FrameData.Top;
		Height = (frame->FrameData.FrameType ==  HELP_FRAME) ? frame->FrameData.Height : ((frame->FrameData.Height !=  0) ? frame->FrameData.Height-1 :frame->FrameData.Height); // Decrement the height only if frame height is non-zero.
		if(Height)
		{	
			for ( i = 1; i <= Height ; i++, j++ )
			{
				DrawStringWithAttribute( frame->FrameData.Left, j, frame->BlankLine, frame->FrameData.FGColor | frame->FrameData.BGColor );
			}	
		}
	}
	
	//Check whether the scroll support required before proceeding.
	if(!IsScrollSupportRequired( frame )) {
		//If the scroll is not required, then reset the first visible control.
		frame->FirstVisibleCtrl = 0;
	}

    //Set position for all controls begining from FirstVisibleControl.
    //In the process calculate the correct LastVisibleControl.
    control = frame->ControlList;
    ControlTop = CurrHeight = 0;
    control = control + frame->FirstVisibleCtrl;
    CurrHeight = frame->FrameData.Top;
    frame->LastVisibleCtrl = frame->FirstVisibleCtrl;

	for ( i = frame->FirstVisibleCtrl; i < frame->ControlCount; i++, control++ )
	{
        ControlTop = CurrHeight;

        //Check conditional
        u8ChkResult = CheckControlCondition( &((*control)->ControlData) );

        if(*control == NULL)
        {
          gApp->CompleteRedraw = TRUE;
           return EFI_UNSUPPORTED;
        }

        if(u8ChkResult == COND_NONE || u8ChkResult == COND_GRAYOUT)
        {
			// TSE_MULTILINE_CONTROLS moved to binary
			if(IsTSEMultilineControlSupported())
			{
				if(frame->FrameData.FrameType == MAIN_FRAME)
				{
					UINT16 height=0;
					(*control)->Methods->GetControlHeight( *control, (VOID*)frame,&height);
					height = height ? height:1;
					if(height >= frame->FrameData.Height)
					{
						height=frame->FrameData.Height -1;
					}
					(*control)->Methods->SetDimensions( *control, 
											frame->FrameData.Width - (UINT8)gLabelLeftMargin - 2, 
											height );
				}
			}

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
        
        if(!((u8ChkResult == COND_NONE) || ((u8ChkResult == COND_GRAYOUT) && IsGrayoutSelectable())))
        {
      	  //Control grayed out or suppressed
      	  if(frame->CurrentControl == i)
      	  {
      		  //Current control cant be focused
      		  (*control)->Methods->SetFocus(*control,FALSE);
      		  bCurrNotFocused = TRUE;
      	  }
        }

        (*control)->Methods->SetPosition( *control, (*control)->Left, ControlTop);
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
    control = control + frame->FirstVisibleCtrl;
    for ( i = frame->FirstVisibleCtrl; i <= frame->LastVisibleCtrl; i++, control++ )
    {
        (*control)->Methods->Draw( *control );
    }

    CurrHeight = frame->FrameData.Top;
	for (count = 0; count < frame->ControlCount; count++)
	{
		u8ChkResult = COND_NONE;

        u8ChkResult = CheckControlCondition( &(frame->ControlList[count]->ControlData) );
        
        if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
        {
			visibleControlCount++ ; 
            CurrHeight = CurrHeight + frame->ControlList[count]->Height;

			if(count <= frame->LastVisibleCtrl)// Calculating the actual vlaue of CurrentvisibleControl when suppressed controls are present
   			         CurrentvisibleControlCount++;	
			if(count < frame->FirstVisibleCtrl)//Calculating the actual vlaue of FirstvisibleControl when suppressed controls are present
            			CurrentFirstvisibleControlCount++;
			FrameLastVisibleControl = count;
	  }
	}
    
    if(frame->FrameData.FrameType == MAIN_FRAME)
    {
        // Total height of all drawable controls, takes care of multi-line issues
        visibleControlHeight = (UINT32)(CurrHeight - frame->FrameData.Top);
    
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
    
    	//Scroll bar coordinates are updated based on the controls in the frame
       	numOfBlocks = frame->FrameData.Height - 2;
        
        // Find the top position, considering multi-line controls may be present     
        for(count=0; count < frame->FirstVisibleCtrl; count++)
        {
            u8ChkResult = CheckControlCondition( &(frame->ControlList[count]->ControlData) );
            if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
            {
                topPos += frame->ControlList[count]->Height;
            }
        }
        
        // Find the last position, considering multi-line controls may be present
        for(count=0; count < frame->LastVisibleCtrl; count++)
        {
            u8ChkResult = CheckControlCondition( &(frame->ControlList[count]->ControlData) );
            if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
            {
                lastPos += frame->ControlList[count]->Height;
            }
        }

        //Special case, check if total lines is 1 greater than a filled frame (visible controls)
        if( (visibleControlHeight > (UINT32)(frame->FrameData.Height-1)) && 
            ((visibleControlHeight - (UINT32)(frame->FrameData.Height-1)) < 2)  )
        {
            visibleControlHeight += 1 ;
            MltCnt = 1 ;
        }

		if (visibleControlHeight)
		{
			FirstLine = (((frame->FrameData.Height - 1) * topPos)/visibleControlHeight )+ frame->FrameData.Top;
			LastLine  = (((frame->FrameData.Height - 1) * lastPos)/visibleControlHeight )+ frame->FrameData.Top;
			sizeOfBar = ((frame->FrameData.Height - 1) * numOfBlocks) / visibleControlHeight;
			modVal = ((((frame->FrameData.Height - 1) * numOfBlocks) % visibleControlHeight)* numOfBlocks) > (numOfBlocks/2) ? 1 : 0;
		}
    
        if(((frame->LastVisibleCtrl < frame->ControlCount-1 )&&(FirstLine > (UINT32)(frame->FrameData.Height-2) )) || (frame->LastVisibleCtrl == FrameLastVisibleControl) || 
			((frame->FirstVisibleCtrl > 0) && (FirstLine == frame->FrameData.Top)) )
		{
		    if( (frame->FrameData.Top+topPos) < (UINT16)(frame->FrameData.Height-2))
			{
                if(MltCnt)
	            {
				    FirstLine = frame->FrameData.Top+topPos; //Providing the top postion value for immediate scroll after the (frame->FrameData.Height-2) reached
					LastLine = FirstLine + sizeOfBar+1;//Providing the appopriate lastline
                }
			}
		}

        // Minimum scrollbar size is 1
        if( ((LastLine - FirstLine)< 2) || (sizeOfBar == 0) )
        {
           sizeOfBar = 1 ;
        }
    
        // Scrollbar size must remain constant
        if((LastLine - FirstLine - 1) != sizeOfBar)
        {
            //Check the maximum scrollbar boundry
            if((FirstLine + sizeOfBar + 1) > (UINT32)(frame->FrameData.Top + frame->FrameData.Height - 2))
            {
                //Set the maximum scrollbar boundry
                FirstLine = frame->FrameData.Top + frame->FrameData.Height - 2 - sizeOfBar - 1 ;
            }
               
            LastLine = FirstLine + sizeOfBar + 1 ;  
        }
		
		if(MAIN_FRAME == frame->FrameData.FrameType)//To store Scrollbar Top and Bottom positon 
		{
			gFrameScrollBarTop = FirstLine;
			gFrameScrollBarBottom = LastLine;
		}
    
       	// draw scrollbar if necessary
    	StyleDrawScrollBar(frame, FirstLine, LastLine, modVal, sizeOfBar, numOfBlocks, bEraseScrollBar);

    }	
			
	FlushLines( frame->FrameData.Top, frame->FrameData.Top + frame->FrameData.Height );

//	if((!TSEMouseIgnoreMouseActionHook())&&((frame->ControlList[frame->CurrentControl]->ControlData.ControlType == CONTROL_TYPE_DATE)||(frame->ControlList[frame->CurrentControl]->ControlData.ControlType == CONTROL_TYPE_TIME)))
//	 if(MAIN_FRAME == frame->FrameData.FrameType)	
//		NumericSoftKbdInit();

	
	return EFI_SUCCESS;
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

EFI_STATUS FrameDrawScrollBar (FRAME_DATA *frame, UINT32 FirstLine, UINT32 LastLine, UINT32	modVal, UINT32	sizeOfBar, UINT32 numOfBlocks, BOOLEAN bEraseScrollBar)
{
	UINT32	i, j ; 
	CHAR16	ArrUp[2]={GEOMETRICSHAPE_UP_TRIANGLE,0x0000} ;
    CHAR16	ArrDown[2]={GEOMETRICSHAPE_DOWN_TRIANGLE,0x0000};
    CHAR16	ScrlSel[2]={BLOCKELEMENT_FULL_BLOCK/*L'N'*/,0x0000};
    CHAR16	ScrlNSel[2]={BLOCKELEMENT_LIGHT_SHADE/*L'S'*/,0x0000};


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
					DrawStringWithAttribute( frame->FrameData.Left + frame->FrameData.Width , j, ArrUp, 
				                            StyleGetScrollBarUpArrowColor() );
				else if(i==(UINT16)(frame->FrameData.Height - 1))
					DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width, j, ArrDown, 
					                             StyleGetScrollBarDownArrowColor() );
				else
				{
					if( (j > FirstLine) && (j < LastLine) &&  ((sizeOfBar + modVal) != 0))
					{	
						DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width, j, ScrlSel, 
													StyleGetScrollBarColor() );
						sizeOfBar--;
					}
					else
					{
						DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width, j, ScrlNSel, 
							                         StyleGetScrollBarColor() ); 
					}
				}
			}
			else if (bEraseScrollBar == TRUE)
				DrawStringWithAttribute( frame->FrameData.Left+ frame->FrameData.Width, j, L" ", 
					                         StyleGetScrollBarColor() ); 

		}
		bEraseScrollBar = FALSE;
	}
			

	return EFI_SUCCESS;
}

/**
    For Inconsistence condition checking for Hotkeys pressed to show the popup

    @param Key 

    @retval BOOLEAN 

**/
BOOLEAN CheckKeyinHotKeysList(AMI_EFI_KEY_DATA Key)
{
  UINT16 index=0;
	for(index=0; index <= gHotKeyCount; index++)
	{
		if( (!EfiCompareMem(&Key, &gHotKeyInfo[index], sizeof(EFI_INPUT_KEY)) ) &&
		( TseCheckShiftState( Key, gHotKeyInfo[index].KeySftSte ) )	)
			return TRUE;
	}
  return FALSE;
}

/**
    Action handler for specific frame 

    @param frame Pointer to the frame data
    @param action Specific action for the frame

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/

EFI_STATUS FrameHandleAction( FRAME_DATA *frame, ACTION_DATA *action )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CONTROL_DATA **control;
	ACTION_DATA *TempAction=NULL;
	CONTROL_ACTION ControlAction;
	
	if ( frame->ControlCount == 0 )
		return Status;

	if(frame->CurrentControl == FRAME_NONE_FOCUSED)
		control = NULL;
	else
		control = &frame->ControlList[frame->CurrentControl];

	switch ( action->Input.Type )
	{

        case ACTION_TYPE_MOUSE:
			Status = MouseFrameHandleAction(frame,action,control);
            break;

        case ACTION_TYPE_KEY:
            if ( control != NULL )
                Status = (*control)->Methods->HandleAction( *control, action );

            if (EFI_ERROR(Status))
                Status = StyleFrameHandleKey( frame, action->Input.Data.AmiKey.Key );

			//Checking Inconsistence for the current control
			if (EFI_ERROR(Status))
			{
				if(( ControlActionUnknown != MapControlKeysHook(action->Input.Data.AmiKey))||(CheckKeyinHotKeysList(action->Input.Data.AmiKey)))
				{//If the control action is known and if its hot key only we are popuping up the ERROR popup
					TempAction =(ACTION_DATA *) EfiLibAllocateZeroPool(sizeof(ACTION_DATA));
					if(TempAction != NULL)
					{
						MemCpy(TempAction,action,sizeof(ACTION_DATA));
					}
					ControlAction =  MapControlKeysHook(action->Input.Data.AmiKey);
					if( CheckInconsistence((PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[gApp->CurrentPage])) )
					{
						if ( (action->Input.Type != ACTION_TYPE_NULL)&&((ControlAction == ControlActionNextRight) || (ControlAction== ControlActionNextLeft)|| (ControlAction == ControlActionAbort)) )
						{
							gAction.ClearAction( action );//Clearing the action if it is valid key pressed
						}
						else if(TempAction != NULL)
						{
							MemCpy(action,TempAction,sizeof(ACTION_DATA));
						}
							
					}
					if(TempAction != NULL)
					{
						MemFreePointer((VOID **)&TempAction);
					}
					
				}
			}
		
            if (EFI_ERROR(Status))
                Status = _FrameHandleKey( frame, action->Input.Data.AmiKey );
            break;

		case ACTION_TYPE_TIMER:
			{
				UINT32 i;
				control = frame->ControlList;
				for ( i = 0; i < frame->ControlCount; i++, control++ )
				{
					EFI_STATUS TempStatus = (*control)->Methods->HandleAction( *control, action );
					if ( Status != EFI_SUCCESS )
						Status = TempStatus;
				}
				// To kill the timer action. Pass it to all the controls and frame.
				//	gAction.ClearAction( action );
			}

            if (EFI_ERROR(Status))
	           	Status = StyleFrameHandleTimer( frame, action);
			break;
		default:
			break;
	}

	return Status;
}

/**
    Function to set callback.

    @param frame OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval STATUS

**/
EFI_STATUS FrameSetCallback( FRAME_DATA *frame, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gObject.SetCallback( frame, container, callback, cookie );
}
/**
    Adds controls to the frame 

    @param frame Pointer to the frame data
    @param controlData Control date to be added

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/
EFI_STATUS FrameAddControl( FRAME_DATA *frame, CONTROL_INFO *controlData )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CONTROL_DATA **control;
	POSITION_INFO *pos;
	UINT16 count = (UINT16)frame->ControlCount;
	UINT16 i;
	BOOLEAN		IsValidControl = FALSE;
	gActiveFrame = frame;
	control = MemReallocateZeroPool( frame->ControlList, count * sizeof(CONTROL_INFO *), (count + 1) * sizeof(CONTROL_INFO *) );

	if ( control == NULL )
		return EFI_OUT_OF_RESOURCES;

	frame->ControlList = control;
	control = &frame->ControlList[ count ];

	for(i=0;gTseControlMap[i].ControlType != CONTROL_TYPE_NULL;i++)
	{
		if(gTseControlMap[i].ControlType == controlData->ControlType)
		{
			Status = gTseControlMap[i].ControlMethods->Create((void**)control);
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
            
            //commented in order to remove the delay before entering Setup.
            // For Multi-line support, GetControlHeight is called again during the draw phase.
            /* 
			if(IsTSEMultilineControlSupported())
			{
				Status = (*control)->Methods->GetControlHeight( *control, (VOID*)frame,&height);
	   			height = height ? height:1;
			}
			*/
            

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

		//	MemFreePointer( (VOID **)&frame->CurrPosition );
		//	frame->CurrPosition = EfiLibAllocateZeroPool( count * sizeof(POSITION_INFO) );
			
		}

	}
	return Status;
}
/**
    Controls Keyboard action for each frame 

    @param frame Pointer to the frame data
    @param key Key pressed in the keyboard

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/

EFI_STATUS _FrameHandleKey( FRAME_DATA *frame, AMI_EFI_KEY_DATA key )
{
    EFI_STATUS Status = EFI_SUCCESS;
    CONTROL_ACTION Action;


    //Get mapping
    Action = MapControlKeysHook(key);

    switch (Action)
    {
        case ControlActionNextUp:
            Status = _FrameScroll( frame, TRUE);
            break;

        case ControlActionNextDown:
            Status = _FrameScroll( frame, FALSE);
            break;

		case ControlActionPageUp:
			Status = _PreviousFrame(frame);
			break;

		case ControlActionPageDown:
			Status = _NextFrame(frame);
			break;

		case ControlActionHome:
				Status = _FocusFirst(frame);
			break;

		case ControlActionEnd:
				Status = _FocusLast(frame);
			break;

		default:
			Status = EFI_UNSUPPORTED;
		break;
    }

//	if ( ! EFI_ERROR( Status ) )
//	{
//		if((TSEMouseIgnoreMouseActionHook())&&((frame->ControlList[frame->CurrentControl]->ControlData.ControlType != CONTROL_TYPE_DATE)&&(frame->ControlList[frame->CurrentControl]->ControlData.ControlType != CONTROL_TYPE_TIME)))
//			NumericSoftKbdExit();
//		if((!TSEMouseIgnoreMouseActionHook())&&((frame->ControlList[frame->CurrentControl]->ControlData.ControlType == CONTROL_TYPE_DATE)||(frame->ControlList[frame->CurrentControl]->ControlData.ControlType == CONTROL_TYPE_TIME)))
//			NumericSoftKbdInit();	
//	}
    return Status;
}



/**
    Adds scroll bar functionality for frames with many controls

    @param frame Pointer to the frame data
    @param bScrollUp Go up is TRUE, else to down one control

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/

EFI_STATUS _FrameScroll( FRAME_DATA *frame, BOOLEAN bScrollUp )
{
	UINT32 thisControl = frame->CurrentControl;
	BOOLEAN bDone = FALSE;
	UINT32 *pOtherEnd = NULL;
    UINT8 u8ChkResult;

	if ( frame->ControlCount == 0 )
		return EFI_UNSUPPORTED;

    switch ( bScrollUp )
    {
        case TRUE:
            if (thisControl == FRAME_NONE_FOCUSED)
				thisControl = frame->FirstVisibleCtrl;

            if(thisControl < frame->FirstVisibleCtrl)
                thisControl = frame->FirstVisibleCtrl;

			//See if there are some controls that can be focused in the screen
			while (thisControl != frame->FirstVisibleCtrl)
			{
				thisControl--;
				if ( _FrameSetControlFocus( frame, thisControl ) )
				{
					bDone = TRUE;
					break;
				}
			}
			
			if (!bDone)
			{
				// No controls that can be focused is left move up til we find a unsuppressed control
                while(thisControl)
                {
                    thisControl--;

                    u8ChkResult = CheckControlCondition( &(frame->ControlList[thisControl]->ControlData) );
                    
                    if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
                    {
                        frame->FirstVisibleCtrl = thisControl;
  				    	pOtherEnd = &(frame->LastVisibleCtrl);
       					_FrameSetControlFocus( frame, thisControl);
                        bDone = TRUE;
                        break;
                    }
                }

                // If no control can be found roll to last visible page
				if (!bDone && StyleGetScrollBehavior())
                {
                    thisControl = frame->LastVisibleCtrl = frame->ControlCount;
    				do
                    {
                        thisControl--;

                        u8ChkResult = CheckControlCondition( &(frame->ControlList[thisControl]->ControlData) );
                        
                        if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
                        {
                            frame->LastVisibleCtrl = thisControl;
      				    	pOtherEnd = &(frame->FirstVisibleCtrl);
                            bDone = TRUE;
          					break;
                        }
    				}while(thisControl);
                }

                if(!bDone)
                {
                    //No control can be shown in this whole frame
					
			if(StyleGetScrollBehavior())
                    		frame->FirstVisibleCtrl = frame->LastVisibleCtrl = 0;
                    return EFI_UNSUPPORTED;
                }
				
				// Calculate other end
				StyleFrameSetControlPositions(frame, pOtherEnd);

				if(pOtherEnd == &(frame->FirstVisibleCtrl))
				{
					//We rolled to last page; find if we can focus one
					while (thisControl >= frame->FirstVisibleCtrl)
					{
						if ( _FrameSetControlFocus( frame, thisControl ) )
						{
							break;
						}
                        if(thisControl == 0)
                            break;
						thisControl--;
					}
				}
					
				//De-focus if we have no control that can be focused in the screen
				if(frame->CurrentControl != FRAME_NONE_FOCUSED)
				{
					if((frame->CurrentControl < frame->FirstVisibleCtrl) || (frame->CurrentControl > frame->LastVisibleCtrl))
					{
						(frame->ControlList[frame->CurrentControl])->Methods->SetFocus(frame->ControlList[frame->CurrentControl],FALSE);
						frame->CurrentControl = FRAME_NONE_FOCUSED;
					}
				}				
			}
		break;

		case FALSE:
			if (thisControl == FRAME_NONE_FOCUSED)
				thisControl = frame->LastVisibleCtrl;

            if(thisControl > frame->LastVisibleCtrl)
                thisControl = frame->LastVisibleCtrl;

			//See if there are some controls that can be focused in the screen
			while (thisControl != frame->LastVisibleCtrl)
			{
				thisControl++;
				if ( _FrameSetControlFocus( frame, thisControl ) )
				{
					bDone = TRUE;
					break;
				}
			}
			
			if (!bDone)
			{
                // No controls that can be focused is left move down til we find an unsuppressed control
                while((frame->ControlCount - 1) != thisControl)
                {
                    thisControl++;

                    //Support grayout condition for readonly controls
                    u8ChkResult = CheckControlCondition( &(frame->ControlList[thisControl]->ControlData) );
                    
                    if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
                    {
                        frame->LastVisibleCtrl = thisControl;
   				    	pOtherEnd = &(frame->FirstVisibleCtrl);
       					_FrameSetControlFocus( frame, thisControl);
                        bDone = TRUE;
                        break;
                    }
                }

                // If no control can be found roll to First visible page
				
		        if (!bDone && StyleGetScrollBehavior())
       		    {
               		thisControl = frame->FirstVisibleCtrl = 0;
   			        while(thisControl < frame->ControlCount)
                    {
                        u8ChkResult = CheckControlCondition( &(frame->ControlList[thisControl]->ControlData) );

                        if((COND_NONE == u8ChkResult) || (COND_GRAYOUT == u8ChkResult))
                        {
                            //frame->FirstVisibleCtrl = thisControl;
      				    	pOtherEnd = &(frame->LastVisibleCtrl);
                            bDone = TRUE;
          					break;
                        }

                        if((thisControl + 1) == frame->ControlCount) //this while loop breaks here before while statement
                            break;
                        thisControl++;
    				}
                }

                if(!bDone)
                {
                    //No control can be shown in this whole frame
					if(StyleGetScrollBehavior())
						frame->FirstVisibleCtrl = frame->LastVisibleCtrl = 0;
                    return EFI_UNSUPPORTED;
                }
					
				//Calculate other end
				StyleFrameSetControlPositions(frame, pOtherEnd);

				if(pOtherEnd == &(frame->LastVisibleCtrl))
				{
					//We rolled to First page; find if we can focus one
					while (thisControl <= frame->LastVisibleCtrl)
					{
						if ( _FrameSetControlFocus( frame, thisControl ) )
						{
							break;
						}
						thisControl++;
					}
				}
                    
				//De-focus if we have no control that can be focused in the screen
				if(frame->CurrentControl != FRAME_NONE_FOCUSED)
				{
					if((frame->CurrentControl < frame->FirstVisibleCtrl) || (frame->CurrentControl > frame->LastVisibleCtrl))
					{
						(frame->ControlList[frame->CurrentControl])->Methods->SetFocus(frame->ControlList[frame->CurrentControl],FALSE);
						frame->CurrentControl = FRAME_NONE_FOCUSED;
					}
				}
			}
		break;
	}

	return EFI_SUCCESS;
}

/**
    Function to set frame submenu callback.

    @param frame SUBMENU_DATA *submenu, VOID *cookie

    @retval VOID

**/
VOID _FrameSubmenuCallback( FRAME_DATA *frame, SUBMENU_DATA *submenu, VOID *cookie )
{
	EFI_STATUS			Status = EFI_SUCCESS;
	CALLBACK_SUBMENU	*callbackData = (CALLBACK_SUBMENU *)cookie;

	if ( ( callbackData == NULL ) || ( callbackData->Header.Type != CALLBACK_TYPE_SUBMENU ) )
		return;

	if (NULL == gApp->PageList [callbackData->DestPage])
	{
		return;		//If destination page is not valid then return. Useful for dynamically forming pages always
	}
		
	if ( ! EFI_ERROR( Status ) )
	{
		gApp->CurrentPage = callbackData->DestPage;

		if(submenu->ControlData.DestQuestionID != 0){
			UINT32 FocusControlIndex = 0;
        
			Status = UpdateDestiantionQuestion(gApp->CurrentPage, submenu->ControlData.DestQuestionID, &FocusControlIndex);
			if(Status == EFI_SUCCESS){
				UINT32 MainFrameIndex = 0;		
				FRAME_DATA *MainFrame = NULL;
				PAGE_DATA *pageData = gApp->PageList[gApp->CurrentPage];
				MainFrameIndex = StyleFrameIndexOf(MAIN_FRAME); 
				MainFrame = pageData->FrameList[MainFrameIndex];
				_FrameSetControlFocus(MainFrame, FocusControlIndex);
			}
		}
	}
	gApp->CompleteRedraw = TRUE;
	gApp->OnRedraw = SUBMENU_COMPLETE_REDRAW;

}
/**
    Function to set frame menu callback.

    @param frame MENU_DATA *menu, VOID *cookie

    @retval VOID

**/
VOID _FrameMenuCallback( FRAME_DATA *frame, MENU_DATA *menu, VOID *cookie )
{
	EFI_STATUS			Status = EFI_SUCCESS;
	CALLBACK_MENU	*callbackData = (CALLBACK_MENU *)cookie;

	if ( ( callbackData == NULL ) || ( callbackData->Header.Type != CALLBACK_TYPE_MENU ) )
		return;

	if ( ! EFI_ERROR( Status ) )
		gApp->CurrentPage = callbackData->DestPage;
	gApp->CompleteRedraw = TRUE;
	gApp->OnRedraw = MENU_COMPLETE_REDRAW;
    if(IsLinkHistorySupport())
    {            
	    ResetNavStack();
    }
}

/**
    Function to set frame variable callback.

    @param frame CONTROL_DATA *control, VOID *cookie

    @retval VOID

**/
VOID _FrameVariableCallback( FRAME_DATA *frame, CONTROL_DATA *control, VOID *cookie )
{
	CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)cookie;

	if ( ( callbackData == NULL ) || ( callbackData->Header.Type != CALLBACK_TYPE_VARIABLE ) )
		return;

	if ( callbackData->Variable >= gVariables->VariableCount )
		return;

	VarSetValue( callbackData->Variable, callbackData->Offset, callbackData->Length, callbackData->Data );

	if ( control->ControlData.ControlFlags.ControlReset )
		gResetRequired = TRUE;

    gUserTseCacheUpdated = TRUE;		//Setting TRUE to know whether setup question is changed or not
	
	if ( control->ControlData.ControlFlags.ControlReconnect )
		CollectReconnectHandles();
}
BOOLEAN TSEPwdSavetoNvram(VOID);
/**
    Function to set frame password callback.

    @param frame CONTROL_DATA *control, VOID *cookie

    @retval VOID

**/
VOID _FramePasswordCallback( FRAME_DATA *frame, CONTROL_DATA *control, VOID *cookie )
{
	CALLBACK_PASSWORD *callbackData = (CALLBACK_PASSWORD *)cookie;
	CHAR16 *saveData = NULL;

	if ( ( callbackData == NULL ) || ( callbackData->Header.Type != CALLBACK_TYPE_PASSWORD ) )
		return;

	if ( callbackData->Variable >= gVariables->VariableCount )
		return;

	saveData = (CHAR16 *)callbackData->Data;

	if(FramePwdCallbackIdePasswordUpdate ( control,saveData) != EFI_SUCCESS)
	{
        if( IsPasswordEncodeEnabled(&control->ControlData) )
    		saveData = PasswordUpdate( callbackData->Data, callbackData->Length);
    
        if ( saveData )
        {
//Save pwd to nvram and not loading empty pwd on loading defaults.
				if((TSEPwdSavetoNvram())&&( VARIABLE_ID_AMITSESETUP == callbackData->Variable ))
				{
				   AMITSESETUP *TempSaveData = (AMITSESETUP *)NULL;
					UINTN size=0;
					
					//TempSaveData = (AMITSESETUP *) EfiLibAllocateZeroPool(sizeof(AMITSESETUP));
					size = sizeof(AMITSESETUP);
	
					TempSaveData = VarGetNvram(VARIABLE_ID_AMITSESETUP, &size);
					MemCopy( ((UINT8*)(TempSaveData))+(callbackData->Offset), saveData, callbackData->Length );
					VarSetNvram( VARIABLE_ID_AMITSESETUP, TempSaveData, sizeof(AMITSESETUP));
					MemFreePointer( (VOID **)&TempSaveData );	       	
				}
				else
        	 		VarSetValue( callbackData->Variable, callbackData->Offset, callbackData->Length, saveData );

			FramePasswordAdvancedCallback(callbackData,saveData);
        }

    	if ( saveData != callbackData->Data )
    		MemFreePointer( (VOID **)&saveData );
	}

	if ( control->ControlData.ControlFlags.ControlReset )
		gResetRequired = TRUE;

    gUserTseCacheUpdated = TRUE;	//Setting TRUE to know whether setup question is changed or not

}

/**
    Sets focus on a defined control in the frame 

    @param frame Date for a specific frame
    @param index Index of the control

    @retval BOOLEAN focusSet - TRUE if focus is set, FALSE otherwise

**/

BOOLEAN _FrameSetControlFocus( FRAME_DATA *frame, UINT32 index )
{
	CONTROL_DATA *control;
	BOOLEAN focusSet = FALSE;

	if ( frame->ControlCount <= index )
		return focusSet;

	control = frame->ControlList[index];
	focusSet = (BOOLEAN)(control->Methods->SetFocus( control, TRUE ) == EFI_SUCCESS);

	// de-focus all other controls
	if ( focusSet )
	{
		UINT32 i;

		for ( i = 0; i < frame->ControlCount; i++ )
		{
			if ( i == index )
				continue;
			control = frame->ControlList[i];
			control->Methods->SetFocus( control, FALSE );
		}

		frame->CurrentControl = index;
	}

	return focusSet;
}

/**
    Function to add controls in the frame 

    @param frame PAGE_INFO *data
				 

    @retval STATUS

**/
EFI_STATUS _FrameAddControls( FRAME_DATA *frame, PAGE_INFO *data )
{
	EFI_STATUS	Status = EFI_INVALID_PARAMETER;
	UINT32 i, controlNumber = 0;
	BOOLEAN focusSet = FALSE;

	for ( i = 0; i < data->PageControls.ControlCount; i++ )
	{
		Status = gFrame.AddControl( frame,  (CONTROL_INFO *) ((UINT8 *)gControlInfo + data->PageControls.ControlList[i]) );
		if ( EFI_ERROR(Status) )
			continue;

		if ( ! focusSet )
		{
			if ( _FrameSetControlFocus( frame, controlNumber ) )
			{
				frame->CurrentControl = controlNumber;
				focusSet = TRUE;
			}
		}
		controlNumber++;
	}

	Status = StyleAddAdditionalControls( frame, controlNumber, focusSet );

    return Status;
}

/**
    Function to add tittle to a frame.

    @param frame UINT32 frameNumber, PAGE_INFO *data

    @retval status

**/
EFI_STATUS _FrameAddTitle( FRAME_DATA *frame, UINT32 frameType, PAGE_INFO *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CONTROL_DATA **control;
	CONTROL_INFO dummy;
	UINT8 BORDER = (frame->FrameData.Border==TRUE)?1:0 ;

	UINT8 SPACER = 0 ;


	control = EfiLibAllocateZeroPool( sizeof(CONTROL_INFO *) );
	if ( control == NULL )
		return EFI_OUT_OF_RESOURCES;

	frame->ControlList = control;
	Status = gMemo.Create((void **) control );

	if ( EFI_ERROR( Status ) )
		return Status;

	MemSet( &dummy, sizeof(dummy), 0 );
	dummy.ControlHandle = (VOID*)(UINTN)INVALID_HANDLE;
	dummy.ControlFlags.ControlVisible = TRUE;
    dummy.ControlType = CONTROL_TYPE_MEMO;

	switch ( frameType )
	{
		case TITLE_FRAME:
			dummy.ControlPtr = (VOID*)gTitle;
			break;
		case HELP_FRAME:
			dummy.ControlPtr = (VOID*)gHelp;
			SPACER = 1 ;
			break;
		case SUBTITLE_FRAME:
			dummy.ControlPtr = (VOID*)gSubTitle;
			break;
		case HELPTITLE_FRAME:
			dummy.ControlPtr = (VOID*)gHelpTitle;
			break;
		case NAV_FRAME:
			dummy.ControlPtr = (VOID*)gNavStrings;
			break;
		default:
			if(StyleFrameAddTitle(frame, frameType,&dummy) != EFI_SUCCESS)
			{
				gMemo.Destroy( *control, TRUE );
				return EFI_UNSUPPORTED;
			}
	}

	(*control)->ParentFrameType = frame->FrameData.FrameType ;
	gMemo.Initialize( *control, &dummy );
	gMemo.SetPosition( *control, frame->FrameData.Left+BORDER, frame->FrameData.Top+BORDER );
	gMemo.SetDimensions( *control, frame->FrameData.Width-SPACER-2*BORDER, frame->FrameData.Height-2*BORDER );
	gMemo.SetAttributes( *control, frame->FrameData.FGColor, frame->FrameData.BGColor );
	if((IsMouseSupported()) && (frameType == NAV_FRAME))
	{	
		(*control)->FGColor = FOCUS_COLOR;
	}
	frame->ControlCount++;
	

	return Status;
}

/**
    Loads the controls from the previous frame into the screen.

    @param frame = Pointer to the current frame

        EFI_STATUS status - 
    @retval EFI_SUCCESS If there exists a previous page and the 
        function successfully loads it				
    @retval EFI_OUT_PF_RESOURCES If there are know previous page

**/

EFI_STATUS _PreviousFrame (FRAME_DATA *frame)
{
	UINT32 thisControl;
	BOOLEAN bFocused;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	if(frame->FirstVisibleCtrl > 0)
	{
		//return EFI_UNSUPPORTED;

		frame->LastVisibleCtrl = frame->FirstVisibleCtrl-1;
		StyleFrameSetControlPositions(frame, &(frame->FirstVisibleCtrl));


		if(frame->FirstVisibleCtrl<=0)
		{
			StyleFrameSetControlPositions(frame, &(frame->LastVisibleCtrl));
		}
	}

	
	//Focus a control if possible
	thisControl = frame->FirstVisibleCtrl;
	bFocused = FALSE;
	while (thisControl <= frame->LastVisibleCtrl)
	{
		if ( _FrameSetControlFocus( frame, thisControl ) )
		{
			bFocused = TRUE;
			break;
		}
		thisControl++;
	}

	if (!bFocused)
		frame->CurrentControl = FRAME_NONE_FOCUSED;

	Status = EFI_SUCCESS ;

	return Status;
}


/**
    Loads the controls from the next frame into the screen.

    @param frame = Pointer to the current frame

        EFI_STATUS status - 
    @retval EFI_SUCCESS If there exists a next page and the 
        function successfully loads it				
    @retval EFI_OUT_PF_RESOURCES If there are know previous page

**/
EFI_STATUS _NextFrame (FRAME_DATA *frame)
{
	UINT32 thisControl;
	BOOLEAN bFocused;

    if(frame->LastVisibleCtrl < (frame->ControlCount-1))
	{
		//return EFI_UNSUPPORTED;
		frame->FirstVisibleCtrl = frame->LastVisibleCtrl + 1;

	StyleFrameSetControlPositions(frame, &(frame->LastVisibleCtrl));

		if(frame->LastVisibleCtrl>=(frame->ControlCount-1))
		{
			StyleFrameSetControlPositions(frame, &(frame->FirstVisibleCtrl));
		}
	}
	

	//Focus a control if possible
	/*
	thisControl = frame->FirstVisibleCtrl;
	bFocused = FALSE;
	while (thisControl <= frame->LastVisibleCtrl)
	{
		if ( _FrameSetControlFocus( frame, thisControl ) )
		{
			bFocused = TRUE;
			break;
		}
		thisControl++;
	}
	*/

	//Set Focus to bottom of page on PageDown 
	thisControl = frame->LastVisibleCtrl;
	bFocused = FALSE;
	while (thisControl >= frame->FirstVisibleCtrl)
	{
		if ( _FrameSetControlFocus( frame, thisControl ) )
		{
			bFocused = TRUE;
			break;
		}
		//Fix for hanging issue in case of no active controls in a frame
		if (thisControl == 0)
			break;

		thisControl--;
	}

	if (!bFocused)
		frame->CurrentControl = FRAME_NONE_FOCUSED;

	return EFI_SUCCESS;
}

/**
    Set focus to the first control in the frame (in the first page). 

    @param frame = Pointer to the current frame

        EFI_STATUS status - 
    @retval EFI_SUCCESS If there exists a previous page and the 
        function successfully loads it				
    @retval EFI_OUT_PF_RESOURCES If there are know previous page

**/

EFI_STATUS _FocusFirst (FRAME_DATA *frame)
{
	EFI_STATUS Status = EFI_SUCCESS;

	do
	{	//At least once in case there's less than a page
		Status = _PreviousFrame(frame);

	}while(frame->FirstVisibleCtrl > 0);
	
	
	return Status;
}

/**
    Set focus to the last control in the frame. 

    @param frame = Pointer to the current frame

        EFI_STATUS status - 
    @retval EFI_SUCCESS If there exists a previous page and the 
        function successfully loads it				
    @retval EFI_OUT_PF_RESOURCES If there are know previous page

**/

EFI_STATUS _FocusLast (FRAME_DATA *frame)
{
	
	EFI_STATUS Status = EFI_SUCCESS;

	do
	{   //At least once in case there's less than a page
		Status = _NextFrame(frame);

	}while(frame->LastVisibleCtrl < (frame->ControlCount-1));

	return Status;
}

/**
    Function to hadnle frame using mouse

    @param frame 
    @param MouseInfo 

    @retval EFI_STATUS

**/
EFI_STATUS _FrameHandleMouse( FRAME_DATA *frame,MOUSE_INFO MouseInfo)
{	
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT32 i, Action;
	
	Action = MapControlMouseActionHook(&MouseInfo);
	//if(TSEMOUSE_RIGHT_CLICK == MouseInfo.ButtonStatus)
	if(ControlActionAbort == Action)
		return Status;	

   if ( frame->ControlCount == 0 )
		return Status;
	
	//Reset lbutton positions
	if (ControlMouseActionLeftUp == Action || ControlActionChoose == Action || ControlActionSelect == Action)
	{
		lButtonDownInitialPosition = 0;
		lButtonDownVaringPosition = 0;
	}	

   //For Handling Frame Scroll Bar using Mouse
	if(frame->UseScrollbar)
	{
		//If clicked on scrollbar area
		if(MouseInfo.Left == (UINT32) frame->FrameData.Left + frame->FrameData.Width)
		{
			//If clicked on UP_ARROW
			if( (MouseInfo.Top == frame->FrameData.Top) /*Arrow Up*/ 
				  &&((TSEMOUSE_LEFT_CLICK == MouseInfo.ButtonStatus)||(TSEMOUSE_LEFT_DCLICK == MouseInfo.ButtonStatus))// Check for left click and left double click to scroll the frame if click on arrow buttons of scroll bar.   			
				)
				MouseScrollBarMove( frame, TRUE, 1 );

			//If clicked on DOWN_ARROW			
			else if( ( MouseInfo.Top == (UINT32) (frame->FrameData.Top + frame->FrameData.Height -2) || MouseInfo.Top == (UINT32) (frame->FrameData.Top + frame->FrameData.Height -3) ) //Coordinates of Mouse click down arrow operation are updated to work for GTSE and AMITSE. 
						&&((TSEMOUSE_LEFT_CLICK == MouseInfo.ButtonStatus)||(TSEMOUSE_LEFT_DCLICK == MouseInfo.ButtonStatus))//Check for left click and left double click to scroll the frame if click on arrow buttons of scroll bar.
					)
				 MouseScrollBarMove( frame, FALSE, 1 );
				
			//If mouse clicked below UP_ARROW and above ScrollBar
			else if( (MouseInfo.Top <= gFrameScrollBarTop) //Clicked  above scrollbar
						&& (MouseInfo.Top > frame->FrameData.Top) //Clicked below UP_ARROW
						&& (ControlMouseActionLeftDown == Action)
					)
			{
				MouseScrollBarMove( frame, TRUE, gFrameScrollBarTop-MouseInfo.Top+1 );				
			}

			//If mouse clicked on scrollbar
			else if( (MouseInfo.Top < gFrameScrollBarBottom) && (MouseInfo.Top > gFrameScrollBarTop) //Clicked on ScrollBar area
						&& (ControlMouseActionLeftDown == Action) // Clicked lbutton down
						&&	((ControlActionChoose != Action) || (ControlActionSelect != Action)) //Neglecting LEFT_DCLICK and LEFT_CLICK on scrollbar area
					)
			{
				if ( lButtonDownInitialPosition == 0 )//To get initial lButtonDown position
				{
					lButtonDownInitialPosition = MouseInfo.Top;
				}
				if ( lButtonDownInitialPosition != MouseInfo.Top )
				{
					lButtonDownVaringPosition = MouseInfo.Top;
					//Move scrollbar upwards
					if( lButtonDownInitialPosition > lButtonDownVaringPosition )
						MouseScrollBarMove( frame, TRUE, lButtonDownInitialPosition - lButtonDownVaringPosition );

					//Move scrollbar downwards
					else
						MouseScrollBarMove( frame, FALSE, lButtonDownVaringPosition - lButtonDownInitialPosition );
					lButtonDownInitialPosition = lButtonDownVaringPosition;
				}
				return EFI_SUCCESS;
			}
			
			//If mouse clicked above DOWN_ARROW and below ScrollBar
			else if( (MouseInfo.Top >= gFrameScrollBarBottom)
						&& (MouseInfo.Top < (UINT32) (frame->FrameData.Top + frame->FrameData.Height -2))
						&& (ControlMouseActionLeftDown == Action)
					)
			{
				MouseScrollBarMove( frame, FALSE, MouseInfo.Top-gFrameScrollBarBottom + 1 );
			}
			return EFI_SUCCESS;
		}
		//If mouse button clicked on scrollbar and moved outside the scroll area
		else if( ControlMouseActionLeftDown == Action
					&& (lButtonDownInitialPosition != 0)
					&& ((ControlActionChoose != Action) || (ControlActionSelect != Action)) 
		)
		{
			lButtonDownVaringPosition = MouseInfo.Top;
			//Move scrollbar upwards
			if( lButtonDownInitialPosition > lButtonDownVaringPosition )
				MouseScrollBarMove( frame, TRUE, lButtonDownInitialPosition - lButtonDownVaringPosition );

			//Move scrollbar downwards
			else
				MouseScrollBarMove( frame, FALSE, lButtonDownVaringPosition - lButtonDownInitialPosition );
			lButtonDownInitialPosition = lButtonDownVaringPosition;
			
			//Reset lButtonDown position when lButton releases
			if (ControlMouseActionLeftUp == Action)
			{
				lButtonDownInitialPosition = lButtonDownVaringPosition = 0;
			}
			return EFI_SUCCESS;
		}
   }
		
	//If mouse clicked on frame without scrollbar support
   if(!((MouseInfo.Top >= (UINT32)frame->FrameData.Top) && (MouseInfo.Left > (UINT32)(frame->FrameData.Width+1) )) //Clicked other than scrollbar area
		&& (ControlMouseActionLeftUp != Action) //Neglecting LEFT_UP action
		)
   {
		//Find control that was clicked
	   for(i=frame->FirstVisibleCtrl; i<=frame->LastVisibleCtrl; i++)
	   {
			if( (MouseInfo.Top >= frame->ControlList[i]->Top) && (MouseInfo.Top < (UINT32)(frame->ControlList[i]->Top + frame->ControlList[i]->Height)) )
	      {
	         //This is the control; select it.
				if ( _FrameSetControlFocus( frame, i ) )
				{
				//	if((TSEMouseIgnoreMouseActionHook())&&((frame->ControlList[i]->ControlData.ControlType != CONTROL_TYPE_DATE)&&(frame->ControlList[i]->ControlData.ControlType != CONTROL_TYPE_TIME)))
				//		NumericSoftKbdExit();
				//	if((!TSEMouseIgnoreMouseActionHook())&&((frame->ControlList[i]->ControlData.ControlType == CONTROL_TYPE_DATE)||(frame->ControlList[i]->ControlData.ControlType == CONTROL_TYPE_TIME)))
				//		 NumericSoftKbdInit();				
						 Status = EFI_SUCCESS;
					break;
				}
	      }
	   }
   }
	return Status;
}

/**
    Function to hadnle frame using mouse

    @param frame 
    @param action 
    @param control 

    @retval EFI_STATUS

**/
EFI_STATUS TSEMouseFrameHandleAction( FRAME_DATA *frame, ACTION_DATA *action,CONTROL_DATA **control )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	if( (action->Input.Data.MouseInfo.Top >= (UINT32)frame->FrameData.Top) && (action->Input.Data.MouseInfo.Left > (UINT32)(frame->FrameData.Width+1) ) )
	{
		 TSEMouseHelpFrameHandleAction(frame, action, control);
	}
	 
		//check whether Current Control is with in the Visible Area or not.
		if ( control != NULL  && (frame->CurrentControl >= frame->FirstVisibleCtrl && frame->CurrentControl <= frame->LastVisibleCtrl))
			Status = (*control)->Methods->HandleAction( *control, action );


    if (EFI_ERROR(Status))
    	Status = StyleFrameHandleMouse( frame, action->Input.Data.MouseInfo );

	//Checking Inconsistence for the current control
	if (EFI_ERROR(Status))
	{
		if( CheckInconsistence((PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[gApp->CurrentPage])) )
			if ( action->Input.Type != ACTION_TYPE_NULL )
				gAction.ClearAction( action );

	}

    if (EFI_ERROR(Status))
        Status = _FrameHandleMouse( frame, action->Input.Data.MouseInfo);

    return Status;
}

/**
    Function to hadnle help frame using mouse

    @param frame 
    @param action 
    @param control 

    @retval EFI_STATUS

**/
EFI_STATUS TSEMouseHelpFrameHandleAction( FRAME_DATA *frame, ACTION_DATA *action,CONTROL_DATA **control )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	MEMO_DATA *memo;
	BOOLEAN bScrollBar = FALSE;
	CHAR16 *text = NULL;
	UINT16 height = 0;
	
	control = frame->ControlList;
	control = control+ frame->FirstVisibleCtrl;
	memo = (MEMO_DATA *) *control;
	
	if( memo->ParentFrameType != HELP_FRAME )
	 return Status;
	else
	{	
		text = HiiGetString( memo->ControlData.ControlHandle,UefiGetTitleField( (VOID *)memo->ControlData.ControlPtr));

		if ( text == NULL )
		return Status;

		if ( EfiStrLen(text) == 0)
    		{
       			 // String is empty but memory is allocated.
			MemFreePointer( (VOID **)&text );
			return Status;
    		}

		// clear out old wrapped string
		//Here gActiveBuffer will update with EmptyString and clear HelpFrame characters. This causes help area flickers when mouse action happens.
		//for ( pos = 0; pos < memo->Height; pos++ )		
		//DrawStringWithAttribute( memo->Left, memo->Top + pos, memo->EmptyString, memo->FGColor | memo->BGColor );
		
		// XXX: this wrapping code is probably not compatible with all languages
		StringWrapText( text, memo->Width, &height );
		MemFreePointer( (VOID **)&text );
		if(height == 0)
			return Status;
		
		bScrollBar = ( (height > memo->Height) && (memo->ParentFrameType == HELP_FRAME /*memo->Top == MAIN_TOP*/)  ) ? 1 : 0;
		
		if(bScrollBar && (action->Input.Data.MouseInfo.ButtonStatus == TSEMOUSE_LEFT_CLICK))
		{
		
			if((action->Input.Data.MouseInfo.Top == (frame->FrameData.Top + frame->FrameData.Height -1))&&(action->Input.Data.MouseInfo.Left == frame->FrameData.Width + frame->FrameData.Left -1 ))
				 	_HelpFrameScroll( memo, FALSE);
			
			if(((action->Input.Data.MouseInfo.Top == frame->FrameData.Top) ||(action->Input.Data.MouseInfo.Top == frame->FrameData.Top -1))
			   && (action->Input.Data.MouseInfo.Left == frame->FrameData.Width + frame->FrameData.Left -1) )
					_HelpFrameScroll( memo, TRUE);
					

		}
		else
		return Status;
	}

	return Status;
	
}

/**
    Function to hadnle help frame scroll using mouse

    @param frame 
    @param action 
    @param control 

    @retval EFI_STATUS

**/
VOID _HelpFrameScroll(MEMO_DATA *memo,BOOLEAN bScrollUp)
{
	switch(bScrollUp)
	{
		
	case FALSE:
		if( memo->ParentFrameType == HELP_FRAME)
		{
			// Incrementing the ScrollBarPosition.
			memo->ScrollBarPosition++;
			// Based upon ScrollBarPosition drawing the Memo again.
			gMemo.Draw(memo);
		}
            break;

	case TRUE:
		if( memo->ParentFrameType == HELP_FRAME)
		{
			// Decrementing ScrollBarPosition
			if( memo->ScrollBarPosition != 0 )
				memo->ScrollBarPosition--;
			else
				memo->ScrollBarPosition = 0;
				gMemo.Draw(memo);
		}
            break;
	default:
		return ;			
	}

}

/**
    Adds scroll bar functionality for frames with many controls

    @param frame Pointer to the frame data
    @param bScrollUp Go up is TRUE, else to down one control

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/

EFI_STATUS TSEMouseScrollBarMove( FRAME_DATA *frame, BOOLEAN bScrollUp, UINT32 Size )
{	
	CONTROL_DATA *control=NULL;
	UINTN  i=0,IndexOfLastNonSupressctrl=0;
	UINT8 u8ChkResult;
	 
	if ( frame->ControlCount == 0 )
		return EFI_UNSUPPORTED;

	switch ( bScrollUp )
	{
		case TRUE:

			if( Size >= frame->FirstVisibleCtrl )
				Size = frame->FirstVisibleCtrl;
			//If FirstVisibleCtrl is not equal to first control then move scrollbar till it reaches top
			if(frame->FirstVisibleCtrl > 0)
			{
				frame->FirstVisibleCtrl = frame->FirstVisibleCtrl - Size;
				frame->LastVisibleCtrl = frame->LastVisibleCtrl - Size;
			}
			break;

		case FALSE:
			//If the size of scroll to move exceeds the remaining control count then change the size value based on the remaining control count
			if( Size >= (frame->ControlCount - frame->LastVisibleCtrl) )
				Size = frame->ControlCount - frame->LastVisibleCtrl - 1;
			//get the Index of the Last Non Supress Ctrl in the Frame
			for ( i = 0; i < frame->ControlCount; i++ )
				{
					
					control = frame->ControlList[i];
					if(control != NULL)
						{
						//Check conditional
						u8ChkResult = CheckControlCondition(&(control->ControlData ));
						if(COND_SUPPRESS != u8ChkResult )
							IndexOfLastNonSupressctrl=i;
						else
							continue;
						}
				}
			//If LastVisibleCtrl is not equal to last control then move scrollbar till it reaches bottom
			if(frame->LastVisibleCtrl < frame->ControlCount-1 && IndexOfLastNonSupressctrl != frame->LastVisibleCtrl)
			{
				frame->FirstVisibleCtrl = frame->FirstVisibleCtrl + Size;
				frame->LastVisibleCtrl = frame->LastVisibleCtrl + Size;
			}
			break;
	}
	return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
