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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/PopupString.c $
//
// $Author: Rajashakerg $
//
// $Revision: 28 $
//
// $Date: 9/17/12 6:20a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file PopupString.c
    This file contains code to handle Popup Stirngs

**/

#include "minisetup.h"

UINTN gCursorLeft,gCursorTop ; 
POPUP_STRING_METHODS gPopupString =
{
		(OBJECT_METHOD_CREATE)PopupStringCreate,
		(OBJECT_METHOD_DESTROY)PopupStringDestroy,
		(OBJECT_METHOD_INITIALIZE)PopupStringInitialize,
		(OBJECT_METHOD_DRAW)PopupStringDraw,
		(OBJECT_METHOD_HANDLE_ACTION)PopupStringHandleAction,
		(OBJECT_METHOD_SET_CALLBACK)PopupStringSetCallback,
		(CONTROL_METHOD_SET_FOCUS)PopupStringSetFocus,
		(CONTROL_METHOD_SET_POSITION)PopupStringSetPosition,
		(CONTROL_METHOD_SET_DIMENSIONS)PopupStringSetDimensions,
		(CONTROL_METHOD_SET_ATTRIBUTES)PopupStringSetAttributes,
		(CONTROL_METHOD_GET_CONTROL_HIGHT)PopupStringGetControlHeight		
};
extern UINTN gInvalidRangeFailMsgBox;
extern UINTN gAddBootOptionReserved;
extern CALLBACK_VARIABLE  gPopupEditCb ;
extern UINT8 res ;
extern BOOLEAN SoftKbdRefresh;
extern BOOLEAN CompleteReDrawFlag;

VOID BbsStrnCpy ( CHAR16  *Dst, CHAR16  *Src, UINTN   Length );
BOOLEAN IsTSECursorSupport(); 
BOOLEAN IsMouseOnSoftkbd(VOID); 
BOOLEAN IsMouseSupported(VOID);
INT32 GetactualScreentop(VOID);
INT32 GetactualScreenLeft(VOID);
VOID 	GetCoordinates(INT32 *x, INT32 *y, INT32 *z); 
VOID SetSoftKbdPosition(UINT32 Height, UINT32 Top);
BOOLEAN IsCharacteronSoftKbd(UINT32 Left,UINT32 TOP);
/**
    this function uses the create function of control
    and creates popup strings

    @param object 

    @retval status

**/
EFI_STATUS PopupStringCreate( POPUP_STRING_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(POPUP_STRING_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gControl.Create((void**) object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gPopupString;

	return Status;
}

/**
    this function uses the destroy function of control
    and destroys popup strings

    @param popupstring BOOLEAN freeMem

    @retval status

**/
EFI_STATUS PopupStringDestroy( POPUP_STRING_DATA *popupstring, BOOLEAN freeMem )
{
	if(NULL == popupstring)
	  return EFI_SUCCESS;

	gControl.Destroy( popupstring, FALSE );

	MemFreePointer( (VOID **)&popupstring->Text );
	MemFreePointer( (VOID **)&popupstring->TempText );
	if( freeMem )
		MemFreePointer( (VOID **)&popupstring );

	return EFI_SUCCESS;
}

/**
    this function uses the Initialize function of control
    and initializes popup strings

    @param popupstring VOID *data

    @retval status

**/
EFI_STATUS PopupStringInitialize( POPUP_STRING_DATA *popupstring, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	Status = gControl.Initialize( popupstring, data );
	if (EFI_ERROR(Status))
		return Status;

	// add extra initialization here...
	SetControlColorsHook(NULL, NULL,  
						 NULL, NULL,
						 &(popupstring->SelBGColor), &(popupstring->SelFGColor),
			             &(popupstring->BGColor), &(popupstring->FGColor),
						 NULL,&(popupstring->LabelFGColor),
					     &(popupstring->NSelLabelFGColor),
						 NULL,NULL,
						 NULL,NULL );

	popupstring->ControlData.ControlHelp = UefiGetHelpField(popupstring->ControlData.ControlPtr);
	popupstring->Interval = (UINT8)popupstring->ControlData.ControlFlags.ControlRefresh;

	popupstring->TextMargin = (UINT8)gControlLeftMargin;
	if ( popupstring->TextWidth == 0 )
	    popupstring->TextWidth = UefiGetWidth(((CONTROL_INFO *)data)->ControlPtr) /2;

    	popupstring->TextAreaWidth = (popupstring->TextWidth </*10*/(UINT16)(popupstring->Width - popupstring->TextMargin -2) ) ? popupstring->TextWidth:/*10*/(UINT16)(popupstring->Width - popupstring->TextMargin -2);

	popupstring->Text = EfiLibAllocateZeroPool((popupstring->TextWidth +1)* sizeof(CHAR16));
	MemSet( popupstring->Text, (popupstring->TextWidth +1)* sizeof(CHAR16), 0 );


	popupstring->TempText = EfiLibAllocateZeroPool((popupstring->TextWidth +1)* sizeof(CHAR16));
	return EFI_SUCCESS;
}

/**
    this function is to draw the popup strings

    @param popupstring 

    @retval status

**/
EFI_STATUS PopupStringDraw(POPUP_STRING_DATA *popupstring )
{
	CHAR16 *text, *text1=NULL;
    UINT16 tmpHeight=0;
	UINTN Len = 0;
	UINT8 BGClr,FGClr;
	EFI_STATUS Status = EFI_SUCCESS;
	//UINT8 ColorNSel = popupstring->FGColor;
	UINT8 ColorLabel = (popupstring->ControlFocus) ? popupstring->LabelFGColor : popupstring->NSelLabelFGColor ;
	VOID *ifrData = popupstring->ControlData.ControlPtr;

	if(popupstring->ControlFocus)
	{  
		BGClr = popupstring->SelBGColor;
		FGClr =  popupstring->SelFGColor; 
	}
	else
	{ 
		BGClr =  popupstring->BGColor;
		FGClr = popupstring->FGColor;  
	}
	// check conditional ptr if necessary
    //Support grayout condition for readonly controls
	//if( popupstring->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &popupstring->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				Status = EFI_WARN_WRITE_FAILURE;
				FGClr = ColorLabel = CONTROL_GRAYOUT_COLOR;		// to display the control value in grayout color
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}

	text = HiiGetString( popupstring->ControlData.ControlHandle, UefiGetPromptField(popupstring->ControlData.ControlPtr));
		
	if ( text != NULL )
	{
		CHAR16 *temptxt = NULL;

		temptxt = (CHAR16 *)EfiLibAllocateZeroPool( (UefiGetWidth(ifrData)+1) * sizeof(CHAR16) );

		//get the string value to check for string length
		VarGetValue( popupstring->ControlData.ControlVariable, UefiGetQuestionOffset(ifrData), UefiGetWidth(ifrData), (VOID *)temptxt );
		if (temptxt){
				Len = TestPrintLength( temptxt ) / (NG_SIZE);
				MemFreePointer( (VOID **)&temptxt );
		}

		//TSE_MULTILINE_CONTROLS moved to binary
		if(popupstring->Height>1 && IsTSEMultilineControlSupported())
		{
			DrawMultiLineStringWithAttribute(	popupstring->Left , popupstring->Top,
															(UINTN)(popupstring->TextMargin - popupstring->Left),(UINTN) popupstring->Height, text,
															(UINT8)( (!Len && popupstring->ControlFocus) ? (popupstring->SelBGColor  | popupstring->SelFGColor) : (popupstring->BGColor | ColorLabel) ) //make it to focus when no string is given
														);
		}
		else
		{
	        // use frame width minus margins as available space
		    // boundary overflow  check
		    if (( TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(popupstring->TextMargin - popupstring->Left) )
				//EfiStrCpy(&text[HiiFindStrPrintBoundary(text,(UINTN)(popupstring->Width - 5))],L"...");
				text[HiiFindStrPrintBoundary(text,(UINTN)(popupstring->TextMargin - popupstring->Left  ))] = L'\0';

		    DrawStringWithAttribute(	popupstring->Left  ,
												popupstring->Top, text, 
												(UINT8)( (!Len && popupstring->ControlFocus) ? (popupstring->SelBGColor  | popupstring->SelFGColor) : (popupstring->BGColor | ColorLabel) ) //Make it to focus when no string is given
											);
		}
		//PopUpString has to display the string besides the title; need not fill with space here
		//for(i=(UINT16)(popupstring->Left + EfiStrLen( text )/*popupstring->TextWidth*/); i < popupstring->Width; i++)
		//	DrawStringWithAttribute( i, popupstring->Top, L" ", BGClr | FGClr );

	    MemFreePointer( (VOID **)&text );
	}

	if(ifrData)
	{
		//get the string value
		VarGetValue( popupstring->ControlData.ControlVariable, UefiGetQuestionOffset(ifrData), UefiGetWidth(ifrData), (VOID *)popupstring->Text );

		Len = TestPrintLength( popupstring->Text ) / (NG_SIZE);

		if(Len)
		{
			text = EfiLibAllocateZeroPool((Len+1)*2);
			EfiStrCpy(text, popupstring->Text);

			//TSE_MULTILINE_CONTROLS moved to binary
            if(popupstring->Height>1 && IsTSEMultilineControlSupported())
	        {
                text1 = StringWrapText( (CHAR16*)text, (popupstring->Width - popupstring->TextMargin ), &tmpHeight );

                if(tmpHeight > 1)
		        {
                    DrawMultiLineStringWithAttribute( (UINTN)popupstring->Left + (UINTN)popupstring->TextMargin, 
																		(UINTN) popupstring->Top,
																		(UINTN)(popupstring->Width - popupstring->TextMargin ),
																		(UINTN)popupstring->Height, (CHAR16*)text,
																		(UINT8) (popupstring->ControlFocus ? (popupstring->SelBGColor  | popupstring->SelFGColor) : (popupstring->BGColor | ColorLabel) )
																	);
                }
                else
                {
                    DrawStringWithAttribute( popupstring->Left + popupstring->TextMargin,
															popupstring->Top, text, 
															(UINT8) (popupstring->ControlFocus ? (popupstring->SelBGColor  | popupstring->SelFGColor) : (popupstring->BGColor | ColorLabel) )
														);
                }
                
                MemFreePointer( (VOID **)&text1 );
                MemFreePointer( (VOID **)&text );
                
            }
            else
            {			
    			if ( Len  > (UINTN)(popupstring->Width - popupstring->TextMargin) )
    			{
    				Len = popupstring->Width - popupstring->TextMargin;
    				EfiStrCpy( &(text[Len-3 /*3 for "..."*/]), L"..." );
    			}
    
    			DrawStringWithAttribute	( popupstring->Left + popupstring->TextMargin  , popupstring->Top, text, 
    											(UINT8) (popupstring->ControlFocus ? (popupstring->SelBGColor  | popupstring->SelFGColor) : (popupstring->BGColor | ColorLabel) )
												);
    
    			MemFreePointer( (VOID **)&text );
            }

		}


		// erase extra spaces if neccessary
		//for(;(UINT16)(popupstring->Left + popupstring->TextMargin + Len) < (popupstring->Width);Len++)
		//	DrawStringWithAttribute( popupstring->Left + popupstring->TextMargin +Len , popupstring->Top, L" ", 
		//				 BGClr | FGClr);
	}

	FlushLines( popupstring->Top  , popupstring->Top );

	return Status;
}

/**
    function to handle the PopupString actions

    @param popupstring OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval status

**/
EFI_STATUS PopupStringSetCallback( POPUP_STRING_DATA *popupstring, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( popupstring, container, callback, cookie );
}

/**
    function to handle the Popup string actions.

    @param popupstring ACTION_DATA *Data

    @retval status

**/
EFI_STATUS PopupStringHandleAction( POPUP_STRING_DATA *popupstring, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
 	CONTROL_ACTION Action=ControlActionUnknown;
 	VOID * UnicodeInterface;
	CHAR16 *TmpStr = NULL;	
	CHAR16 *ResStr = NULL;
	CHAR16 *TmpPopupStringText = NULL;
	UINT16 TmpPopupStringTextWidth = 0;
    UINT8 u8ChkResult;

	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	{
		if (popupstring->Interval == 0)
			return Status;

		if(IsActiveControlPresent(gApp->PageList[gApp->CurrentPage]))
			return Status;

		if ( --(popupstring->Interval) == 0 )
		{
			// initialize the interval
			popupstring->Interval = (UINT8)(popupstring->ControlData.ControlFlags.ControlRefresh);
			return UefiRefershQuestionValueNvRAM(&(popupstring->ControlData));
		}
		else
			return Status;
	}

    u8ChkResult = CheckControlCondition( &popupstring->ControlData );
	 
    //Not to perform action for grayoutif-control when token TSE_SETUP_GRAYOUT_SELECTABLE is enable to set focus for GrayoutIf control
    if ( IsGrayoutSelectable() && (u8ChkResult == COND_GRAYOUT) ) {
        return EFI_UNSUPPORTED;
    }

    if(popupstring->ControlData.ControlFlags.ControlReadOnly)
		return EFI_UNSUPPORTED;

	if ( Data->Input.Type == ACTION_TYPE_MOUSE )
	{
		//Get mapping
	
		//check whether MouseTop is within the Height and Width of popupstring or not
		if((Data->Input.Data.MouseInfo.Top >= (UINT32)popupstring->Top) && (Data->Input.Data.MouseInfo.Top < (UINT32)(popupstring->Top+popupstring->Height)) &&
			(Data->Input.Data.MouseInfo.Left >= (UINT32)popupstring->Left) && (Data->Input.Data.MouseInfo.Left < (UINT32)(popupstring->Left+popupstring->Width)))
		Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);
	}

	if ( Data->Input.Type == ACTION_TYPE_KEY )
    {
        //Get mapping
        Action = MapControlKeysHook(Data->Input.Data.AmiKey);
	}

        if(ControlActionSelect == Action)
        {
		//Taking backup of original string.
		TmpPopupStringText = EfiLibAllocateZeroPool((popupstring->TextWidth + 1) * sizeof(CHAR16));			
		if(NULL == TmpPopupStringText){
			return EFI_OUT_OF_RESOURCES;
		}
		EfiStrCpy(TmpPopupStringText, popupstring->Text);

		//Taking backup of initial string length  
		TmpPopupStringTextWidth = popupstring->TextWidth; 

		// Reading string
		if(_DoPopupStringEdit( popupstring,UefiGetPromptField(popupstring->ControlData.ControlPtr), &(popupstring->Text)) == EFI_SUCCESS)
		{
			UINTN MinVal=0, MaxVal=0;
			MinVal = (UINTN)UefiGetMinValue(popupstring->ControlData.ControlPtr);
			MaxVal = (UINTN)UefiGetMaxValue(popupstring->ControlData.ControlPtr);

			//Find length of entered string  
			popupstring->TextWidth = (UINT16)EfiStrLen(popupstring->Text);

			// Check for Range. If its a valid string annd not between min and max report invalid error msg
			if((popupstring->Text) &&((popupstring->TextWidth < MinVal) || (popupstring->TextWidth > MaxVal)))
			{
				//Reset popupstring->Text with initial string  
				if(TmpPopupStringText) {
					MemFreePointer((VOID **)&popupstring->Text);					
					popupstring->Text = EfiLibAllocateZeroPool((MaxVal + 1) * sizeof(CHAR16)); //Allocating PopupString->Text based on max size of control
					if(NULL == popupstring->Text){
						MemFreePointer((VOID **)&TmpPopupStringText);
						return EFI_OUT_OF_RESOURCES;
					}
					EfiStrCpy(popupstring->Text, TmpPopupStringText);
					MemFreePointer((VOID **)&TmpPopupStringText);
					popupstring->TextWidth = TmpPopupStringTextWidth; ////Resetting width with old one
				}
				//Report Message box  
				CallbackShowMessageBox( (UINTN)gInvalidRangeFailMsgBox, MSGBOX_TYPE_OK );
				return EFI_ABORTED;
			}

			// not to allow Reserved boot options to delete 
			if( (popupstring->Text) && (IsReservedBootOptionNamesEnable()) ) 
			{
				ResStr = CONVERT_TO_WSTRING(RESERVED_ADD_DEL_BOOT_OPTION_NAME);
				TmpStr = EfiLibAllocateZeroPool( EfiStrLen(ResStr)+1); 
				BbsStrnCpy(TmpStr, popupstring->Text, (EfiStrLen(ResStr)+1));
				TmpStr[EfiStrLen(ResStr)]= L'\0';

				Status = InitUnicodeCollectionProtocol(&UnicodeInterface);
				if(!EFI_ERROR(Status)) 
				{
					if( StringColl( UnicodeInterface, TmpStr, ResStr) == 0 )
					{
						CallbackShowMessageBox( (UINTN)gAddBootOptionReserved, MSGBOX_TYPE_OK );
						return EFI_ABORTED;

					}
				}
			}

            if(popupstring->Callback != NULL)
            {
                if ( popupstring->Cookie != NULL )
                {
                    VOID *ifrData = popupstring->ControlData.ControlPtr;
                    CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)popupstring->Cookie;

                    callbackData->Variable = popupstring->ControlData.ControlVariable;
                    callbackData->Offset = UefiGetQuestionOffset(ifrData);
                    callbackData->Length = UefiGetWidth(ifrData);
                    callbackData->Data = (VOID *)popupstring->Text;

					// To support UEFI 2.1.C spec to Not to update the control when Callback fails.
					UefiPreControlUpdate(&(popupstring->ControlData));
                    popupstring->Callback( popupstring->Container,popupstring, popupstring->Cookie );	
                }
            }
            if ( UefiIsInteractive(&popupstring->ControlData) )
            {
    			VOID * Handle=popupstring->ControlData.ControlHandle;
    			UINT16 Key = UefiGetControlKey(&(popupstring->ControlData));
    			CONTROL_DATA *Control;
    
    			Status = CallFormCallBack(&(popupstring->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE);// Updated the action to AMI_CALLBACK_CONTROL_UPDATE
    			Control = GetUpdatedControlData((CONTROL_DATA*)popupstring,CONTROL_TYPE_POPUP_STRING,Handle,Key);

    			if(Control == NULL) // Control deleted ?
    				return EFI_SUCCESS;
    			if(Control != (CONTROL_DATA*)popupstring)
    				popupstring = (POPUP_STRING_DATA*)Control; //control Updated

				//If error status returned by CallFormCallBack(), reset popupstring->Text with the old string.
				if(EFI_ERROR(Status))
				{
					if(popupstring->Text)
						MemFreePointer((VOID **)&popupstring->Text);
					popupstring->Text = EfiLibAllocateZeroPool((TmpPopupStringTextWidth + 1) * sizeof(CHAR16));
					if(popupstring->Text == NULL){
						return EFI_OUT_OF_RESOURCES;
					}
					if(TmpPopupStringText) {
						//Resetting old PopupstringText with TmpPopupStringText 
						EfiStrCpy(popupstring->Text, TmpPopupStringText);
					}

					//Reset string length to initial string TmpPopupStringTextWidth length 
					popupstring->TextWidth = TmpPopupStringTextWidth; 

					//Report Message box and return without proceeding further. 
					//CallbackShowMessageBox( (UINTN)gInvalidRangeFailMsgBox, MSGBOX_TYPE_OK );   //All other callback not poping up msg box so commented
					MemFreePointer((VOID **)&TmpPopupStringText);
					return EFI_ABORTED;

				}

            }
        }
		Status = EFI_SUCCESS;
    }

	return Status;
}
/**
    Function to handle the PopupString actions

    @param popuppassword BOOLEAN focus

    @retval Status

**/
EFI_STATUS PopupStringSetFocus(POPUP_STRING_DATA *popuppassword, BOOLEAN focus)
{
	UINT8 u8ChkResult;
	
	if(focus != FALSE)
	{
        u8ChkResult = CheckControlCondition( &popuppassword->ControlData );
        
		//Setting focus to control which has no control-condtion and token TSE_SETUP_GRAYOUT_SELECTABLE is enable to make GrayoutIf control to focus and selectable
		if ( ((u8ChkResult != COND_NONE) && (u8ChkResult != COND_GRAYOUT)) ||
             (!IsGrayoutSelectable() && (u8ChkResult == COND_GRAYOUT))
			){
			return EFI_UNSUPPORTED;
		}
	}

	if( !(popuppassword->ControlFocus && focus) )
		popuppassword->ControlFocus = focus;
	return EFI_SUCCESS;
}
/**
    Function to set position.

    @param popuppassword UINT16 Left, UINT16 Top

    @retval STATUS

**/
EFI_STATUS PopupStringSetPosition(POPUP_STRING_DATA *popuppassword, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition( (CONTROL_DATA*)popuppassword, Left, Top );
}
/**
    Function to set dimension.

    @param popuppassword UINT16 Width, UINT16 Height

    @retval STATUS

**/
EFI_STATUS PopupStringSetDimensions(POPUP_STRING_DATA *popuppassword, UINT16 Width, UINT16 Height )
{
	return gControl.SetDimensions((CONTROL_DATA*) popuppassword, Width, Height );
}
/**
    Function to set attributes.

    @param popuppassword UINT8 FGColor, UINT8 BGColor

    @retval STATUS

**/
EFI_STATUS PopupStringSetAttributes(POPUP_STRING_DATA *popuppassword, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes((CONTROL_DATA*) popuppassword, FGColor, BGColor );
}

/**
    Function to get the lable height of the PopupString

    @param popupstring frame, UINT16 *height

    @retval Status

**/
EFI_STATUS PopupStringGetControlHeight(POPUP_STRING_DATA *popupstring, VOID * frame, UINT16 *height)
{
	//TSE_MULTILINE_CONTROLS moved to binary
	if(IsTSEMultilineControlSupported())
	{
		CHAR16 *newText = NULL,*text=NULL;
	    UINT16 Width=0, tmpHeight=0;
		VOID *ifrData = popupstring->ControlData.ControlPtr;
		
	    if(ifrData)
		{
			//get the string value
			VarGetValue( popupstring->ControlData.ControlVariable, UefiGetQuestionOffset(ifrData), UefiGetWidth(ifrData), (VOID *)popupstring->Text );
	        newText = StringWrapText( popupstring->Text, 
	                    (UINT16)(((FRAME_DATA *)frame)->FrameData.Width - gLabelLeftMargin - popupstring->TextMargin - 2 ), &tmpHeight );
	                                
		    *height = tmpHeight;                        
		    MemFreePointer( (VOID **)&newText );
		}
			
		Width = (UINT16)(popupstring->TextMargin - (((FRAME_DATA*)frame)->FrameData.Left + (UINT8)gControlLeftPad));
		text = HiiGetString( popupstring->ControlData.ControlHandle, UefiGetPromptField(popupstring->ControlData.ControlPtr));
	
		newText = StringWrapText( text, Width, &tmpHeight );
	
	    if(tmpHeight > *height)
			*height = tmpHeight;
	
	    (*height) = (*height) ? (*height):1;
	
	    MemFreePointer( (VOID **)&newText );
		MemFreePointer( (VOID **)&text );
	}
	else
	{
		*height = 1 ;
	}
	return EFI_SUCCESS;
}



/**
    Function for PopupStringEdit callback.

    @param container VOID *object, VOID *cookie

    @retval STATUS

**/
EFI_STATUS _CBPopupStringEdit(VOID *container, VOID *object, VOID *cookie)
{
	EFI_STATUS Status = EFI_SUCCESS;
	if(cookie!=NULL)
	{

		res=0;
	}
	else
 	   res=1; // exit with no changes to string

	return Status;
}

/**
    function to get values for popup menu using edit functions.

    @param popupedit 

    @retval UINTN

**/
UINTN _PopupStringEditGetValue( POPUPEDIT_DATA *popupedit )
{
	ACTION_DATA *action = gApp->Action;
	BOOLEAN		DrawCursor = TRUE;
	EFI_STATUS  Status = EFI_SUCCESS; 
	INT32		OldMousePointerX=1,OldMousePointerY=1,Screen_Left,Screen_Top;
	INT32  	MousePointerX = 0,MousePointerY = 0,MousePointerZ = 0;
	UINTN glyphWidth = 0, glyphHeight = 0;
	
	gPopupEdit.SetCallback( popupedit, NULL,(OBJECT_CALLBACK) _CBPopupStringEdit, &gPopupEditCb );
   	gPopupEdit.SetDimensions( (CONTROL_DATA*)popupedit, popupedit->Width , popupedit->Height );
    res = (UINT8)-1;

 	if(action->Input.Type == ACTION_TYPE_MOUSE)
 	{
 		SetSoftKbdPosition(popupedit->Height+1, (UINT32)((gMaxRows - popupedit->Height)/2));
 		TSEStringReadLoopEntryHook();
 	}
  	Screen_Top=GetactualScreentop();
 	Screen_Left=GetactualScreenLeft();
	
	glyphWidth = HiiGetGlyphWidth();
	glyphHeight = HiiGetGlyphHeight();
	
    while ( res == (UINT8)-1 )
	{
		if ( action->Input.Type != ACTION_TYPE_NULL )
			gAction.ClearAction( action );
		
		GetCoordinates (&MousePointerX, &MousePointerY, &MousePointerZ);
		if (gST->ConOut)
		{
			//Stop mouse when mouse pointer is over edit textbox cursor
			if (
					( ((MousePointerX/glyphWidth) <= (UINT32)gST->ConOut->Mode->CursorColumn+(Screen_Left/glyphWidth)+1) && ((MousePointerX/glyphWidth) >= (UINT32)gST->ConOut->Mode->CursorColumn+(Screen_Left/glyphWidth)-2) ) && 
					( ((MousePointerY/glyphHeight) <= (UINT32)gST->ConOut->Mode->CursorRow+(Screen_Top/glyphHeight)+1) && ((MousePointerY/glyphHeight) >= (UINT32)gST->ConOut->Mode->CursorRow+(Screen_Top/glyphHeight)-1) )			//Stop the mouse if mouse poiner is over the text cursor useful for avoiding corruption
				)
			{
					MouseStop ();
					OldMousePointerX=MousePointerX;
					OldMousePointerY=MousePointerY;
			}
			else
			{
				if ( !IsMouseOnSoftkbd() )
					MouseFreeze ();
			}
		}
	
		gPopupEdit.Draw( popupedit );
		if((FALSE == DrawCursor) && CompleteReDrawFlag)
			DrawCursor =TRUE;
		DoRealFlushLines();

		if (IsTSECursorSupport()) 
		{
			if (TRUE == DrawCursor)							// Enabling at valid times
			{
				gST->ConOut->SetAttribute (gST->ConOut, popupedit->FGColor | popupedit->BGColor);
	        	gST->ConOut->SetCursorPosition (gST->ConOut, gCursorLeft, gCursorTop);//Providing the calculated Cursor position values.
	        	if( !IsCharacteronSoftKbd((UINT32)gCursorLeft,(UINT32)gCursorTop))
	        		gST->ConOut->EnableCursor (gST->ConOut, TRUE);
				DrawCursor = FALSE;
			}
		}
		//Do mouserefresh only when mouse pointer is out side softkbd
		if ( !IsMouseOnSoftkbd() )
		{
			SoftKbdRefresh = FALSE;
			MouseRefresh();
		}
			
		

Action:
		if ( gAction.GetAction( action ) != EFI_SUCCESS )
			continue;

		if( ( action->Input.Type == ACTION_TYPE_KEY ) &&
			(TsePrintScreenEventSupport(action->Input.Data.AmiKey.Key.ScanCode) ) ) 
		{
			gApplication.HandleAction( gApp, action );
			DrawCursor = TRUE;     // If print screen is taken then Drawcursor is made true.
			continue;
		}
		
		Status = gPopupEdit.HandleAction( popupedit, action );
		
		if(IsMouseSupported())
		{
			if((OldMousePointerX ==MousePointerX) && (OldMousePointerY ==MousePointerY))
			{
				GetCoordinates (&MousePointerX, &MousePointerY, &MousePointerZ);
				if(action->Input.Type != ACTION_TYPE_KEY)
				{
					if ( action->Input.Type != ACTION_TYPE_NULL )
						gAction.ClearAction( action );
					goto Action;
				}
			}
		}

		if (IsTSECursorSupport() && !((EFI_ERROR (Status))))
		{
			DrawCursor = TRUE;			//If any valid action performed then draw the cursor, useful for blinking
		}
	}
	    TSEStringReadLoopExitHook();

	return res;
}
/**
    Function to initiate PopupString editing

    @param PopupString UINT16 Title, CHAR16 **Text

    @retval STATUS

**/
EFI_STATUS _DoPopupStringEdit( POPUP_STRING_DATA *PopupString, UINT16 Title, CHAR16 **Text)
{
	static UINT8 gIFRPopupEdit[50];
	EFI_STATUS Status= EFI_SUCCESS;

	UINT8 retValue = (UINT8)-1;

	POPUPEDIT_DATA *popupedit = NULL;

	CONTROL_INFO dummy;
    BOOLEAN 		PreviousCursorState = gST->ConOut->Mode->CursorVisible; 

	Status = gPopupEdit.Create( (void**)&popupedit );
	if ( EFI_ERROR( Status ) )
		return Status;

	MemSet( &dummy, sizeof(dummy), 0 );
	dummy.ControlHandle = PopupString->ControlData.ControlHandle;
    MemCopy( &gIFRPopupEdit, (VOID*)PopupString->ControlData.ControlPtr, UefiGetIfrLength(PopupString->ControlData.ControlPtr));

//    gIFRPopupEdit.Prompt = Title;
   	UefiSetPromptField ((VOID*)&gIFRPopupEdit,Title);

	dummy.ControlPtr = (VOID*)&gIFRPopupEdit;
	dummy.ControlFlags.ControlVisible = TRUE;
   
	//gPopupEdit.SetTitle(popupedit,Title);
    gPopupEdit.SetText(popupedit,*Text);
	gPopupEdit.SetType( popupedit, POPUPEDIT_TYPE_STRING );
	Status = gPopupEdit.Initialize( popupedit, &dummy );
	if ( EFI_ERROR( Status ) )
		goto Done;

	//gPopupEdit.SetType( popupedit, POPUPEDIT_TYPE_STRING );
	retValue = (UINT8)_PopupStringEditGetValue( popupedit );
	if (IsTSECursorSupport()) 
	{
		gST->ConOut->EnableCursor(gST->ConOut, PreviousCursorState); 
	}
	if(retValue ==0)
	{
		// string changed, save to nvram or do whatever needs to be done
	    MemFreePointer( (VOID **)Text );
	    *Text = EfiLibAllocateZeroPool( (gPopupEditCb.Length+1) * sizeof(CHAR16) );
	     if ( *Text == NULL )
		return EFI_OUT_OF_RESOURCES;

	    EfiStrCpy( *Text, gPopupEditCb.Data );			

	}
Done:
	gPopupEdit.Destroy( popupedit, TRUE );
	return Status;
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
