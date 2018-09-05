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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/PopupEdit.c $
//
// $Author: Arunsb $
//
// $Revision: 21 $
//
// $Date: 4/27/12 2:02p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file PopupEdit.c
    This file contains code to handle Popup Edit operations

**/

#include "minisetup.h"
extern UINTN gCursorLeft,gCursorTop ; 
extern BOOLEAN SoftKbdRefresh;

POPUPEDIT_METHODS gPopupEdit =
{
		(OBJECT_METHOD_CREATE)PopupEditCreate,
		(OBJECT_METHOD_DESTROY)PopupEditDestroy,
		(OBJECT_METHOD_INITIALIZE)PopupEditInitialize,
		(OBJECT_METHOD_DRAW)PopupEditDraw,
		(OBJECT_METHOD_HANDLE_ACTION)PopupEditHandleAction,
		(OBJECT_METHOD_SET_CALLBACK)PopupEditSetCallback,
		(CONTROL_METHOD_SET_FOCUS)PopupEditSetFocus,
		(CONTROL_METHOD_SET_POSITION)PopupEditSetPosition,
		(CONTROL_METHOD_SET_DIMENSIONS)PopupEditSetDimensions,
		(CONTROL_METHOD_SET_ATTRIBUTES)PopupEditSetAttributes,
		(CONTROL_METHOD_GET_CONTROL_HIGHT)PopupEditGetControlHeight,
		PopupEditSetType,
	    PopupEditSetText		
	
};

BOOLEAN CheckIsAllowedPasswordChar(CHAR16 Char);
/**
    this function uses the create function of control
    and creates the Popup Edit

    @param object 

    @retval status

**/
EFI_STATUS PopupEditCreate( POPUPEDIT_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(POPUPEDIT_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gPopup.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gPopupEdit;

	return Status;
}

/**
    this function uses the destroy function of control
    and destroys the Popup Edit

    @param object BOOLEAN freeMem

    @retval status

**/
EFI_STATUS PopupEditDestroy( POPUPEDIT_DATA *PopupEdit, BOOLEAN freeMem )
{
	//EFI_STATUS Status; Set but not used

	if(NULL == PopupEdit)
	  return EFI_SUCCESS;
	
//	Status = gPopup.Destroy( PopupEdit, FALSE );
	gPopup.Destroy( PopupEdit, FALSE );

	if(PopupEdit->Title)
		MemFreePointer( (VOID **)&PopupEdit->Title);

	RestoreScreen( PopupEdit->ScreenBuf );
	if (freeMem)
	{
		if (POPUPEDIT_TYPE_PASSWORD == PopupEdit->PopupEditType)
		{
			gBS->SetMem (PopupEdit->Text, EfiStrLen (PopupEdit->Text) * sizeof (CHAR16), 0);	
		}
        MemFreePointer( (VOID **)&PopupEdit->Text );
        MemFreePointer( (VOID **)&PopupEdit->TempText );
        MemFreePointer( (VOID **)&PopupEdit->EmptyString ); //Free the allocated memory for the EmptyString field
		MemFreePointer( (VOID **)&PopupEdit );
	}

	return EFI_SUCCESS;
}

/**
    this function uses the Initialize function of control
    and initializes the Popup Edit

    @param PopupEdit VOID *data

    @retval status

**/
EFI_STATUS PopupEditInitialize( POPUPEDIT_DATA *PopupEdit, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
//	UINT16 Length =0/*,height=0*/, Size =0;  Unused Size
	UINT16 Length =0/*,height=0*/; 
	CHAR16 *TempText;
	UINT16 TempHeight = 0;

	// initializes the heigth and width
 	Status = gPopup.Initialize( PopupEdit, data );
	if (EFI_ERROR(Status))
		return Status;

	// add extra initialization here...
	SetControlColorsHook( NULL, NULL , NULL, NULL , NULL, NULL ,
	                      NULL , NULL,  NULL,NULL ,
						  NULL,NULL ,NULL,&(PopupEdit->FGColor),&(PopupEdit->BGColor)  );


	PopupEdit->Container = NULL;
	
	PopupEdit->Border =TRUE;
	PopupEdit->Shadow = TRUE;
	PopupEdit->Title = HiiGetString(PopupEdit->ControlData.ControlHandle, UefiGetPromptField(PopupEdit->ControlData.ControlPtr));

	if ( Length < (TestPrintLength( PopupEdit->Title ) / (NG_SIZE)))
		Length = (UINT16)TestPrintLength( PopupEdit->Title ) / (NG_SIZE);

	if ( Length < (UINT16)UefiGetMaxValue(PopupEdit->ControlData.ControlPtr) )
		Length = (UINT16)UefiGetMaxValue(PopupEdit->ControlData.ControlPtr);

	PopupEdit->Width = (UINT16)(Length + 6);		//6 = 4 + 2; 4 for 2 * (border and hyphen) + 2 space in title
	if ( PopupEdit->Width > MAX_POPUPEDIT_WIDTH )
	{
//		if ( IsTSEMultilineControlSupported() ) //have to increase editable width area for both multiline enable and disable case
		if (((UINT16)UefiGetMaxValue(PopupEdit->ControlData.ControlPtr)) >= (MAX_POPUPEDIT_WIDTH - 2))
		{
			PopupEdit->Height += (UINT16)(PopupEdit->Width / (MAX_POPUPEDIT_WIDTH-2))+(((PopupEdit->Width % MAX_POPUPEDIT_WIDTH) > 0)?1:0)-1;
		}
		PopupEdit->Width = (UINT16)MAX_POPUPEDIT_WIDTH;	
	}
	PopupEdit->TextWidth = (UINT16)UefiGetMaxValue(((CONTROL_INFO *)data)->ControlPtr);

     
	// create text memory area with max available size
	TempText = EfiLibAllocateZeroPool(PopupEdit->TextWidth*2+2);
    EfiStrCpy(TempText,PopupEdit->Text);
//	MemFreePointer((CHAR16**)&PopupEdit->Text);
	MemFreePointer((VOID**)&PopupEdit->Text);
	PopupEdit->Text =TempText;

	PopupEdit->ScreenBuf = SaveScreen();

    PopupEdit->TempText =  EfiLibAllocateZeroPool( PopupEdit->TextWidth*2 +2 );
	 
	//Obtain the exact height after text wrap
	//PopupEdit->Height += 2+ (PopupEdit->TextWidth/PopupEdit->Width +1)/*height*/;
    //If multiline on draw title in multiline else title height will be 1
    if ( IsTSEMultilineControlSupported() )
    {
    	StringWrapText(PopupEdit->Title, PopupEdit->Width - 4, &TempHeight);
    	PopupEdit->Height += TempHeight;
    }
    else
    {
    	PopupEdit->Height += 1;
    }
	
	// Height +1 for editing space, +1 for border 
	gPopupEdit.SetDimensions( (CONTROL_DATA*)PopupEdit, PopupEdit->Width , PopupEdit->Height+2 );

	return Status;
}

/**
    function to draw the popup to edit with attributes

    @param PopupEdit 

    @retval status

**/
EFI_STATUS PopupEditDraw( POPUPEDIT_DATA *PopupEdit )
{
	EFI_STATUS Status;
	CHAR16 *text = NULL, *newText = NULL;
	CHAR16 *line;
	UINT16 pos, height=1,i;
	UINTN Wrappedtextlen= 0;
	
    text = StrDup(PopupEdit->Text);

	Status = gPopup.Draw( PopupEdit );
	if(EFI_ERROR(Status))
	{
		MemFreePointer( (VOID **)&text );
		return Status;
	}

	// clear out old wrapped string
	for ( pos = PopupEdit->Height- (2+ (PopupEdit->TextWidth/PopupEdit->Width +1))+1; pos < PopupEdit->Height-2; pos++ )
		DrawStringWithAttribute( PopupEdit->Left+1, PopupEdit->Top + pos, PopupEdit->EmptyString, PopupEdit->FGColor | PopupEdit->BGColor );

//String and password behaves same no wrapping
/*	if(PopupEdit->PopupEditType != POPUPEDIT_TYPE_PASSWORD ) {
		//Updated the width to write the string with in the boundary.
		newText = StringWrapText( text, PopupEdit->Width-2, &height );	
		
	}
	else {*/
		newText = StringWrapTextWithoutJustification (text, PopupEdit->Width-2, &height);
//	}
		
	if (newText == NULL)
	{
		MemFreePointer( (VOID **)&text );
		return EFI_NOT_FOUND;
	}

	if ( newText != text )
		MemFreePointer( (VOID **)&text );

	line = newText;

	//Just for clearance
	pos =  PopupEdit->Height - (PopupEdit->TextWidth/(PopupEdit->Width-2))-2;
	
			
	for (; pos < PopupEdit->Height; pos++ )
	{
		CHAR16 save;

		text = line;
		if ( *line == L'\0' )
			break;

		while ( ( *line != L'\n' ) &&( *line != L'\r' ) && ( *line != L'\0' ) )
			line++;


		save = *line;
		*line = L'\0';
		i=0;
		while( (text[i] != L'\0') && (PopupEdit->Chr !=0x0 ) )
		{		
//			if(text[i] != L' ') //all chars should be treated the same
				text[i] = PopupEdit->Chr;
			i++;
		}

		DrawStringWithAttribute( PopupEdit->Left+1, PopupEdit->Top + pos, text, PopupEdit->FGColor | PopupEdit->BGColor );

		if ( ( *line = save ) != L'\0' )
		{
			line++;
			if ( ( *line == L'\r' ) || ( *line == L'\n' ) )
				line++;
		}
		
		if(L'\0' == *line)
			Wrappedtextlen = EfiStrLen(text);// Getting the wrapped text length
			 
	}
	MemFreePointer( (VOID **)&newText );

	if ( pos < height )
		height = pos;

	gCursorLeft = PopupEdit->Left + Wrappedtextlen + 1;

	gCursorTop = PopupEdit->Top + (pos-1);//Providing the Cursor Postion depending on the lenght of string
	if((Wrappedtextlen == 0)||(((UINT16)Wrappedtextlen) >= (PopupEdit->Width-2)))		//To show cursor in next line
	{
		gCursorLeft = PopupEdit->Left + 1;
		gCursorTop++;
	}
	PopupEdit->DisplayLines = height;
	FlushLines( PopupEdit->Top, PopupEdit->Top + PopupEdit->Height  );
	return  EFI_SUCCESS;

}


/**
    function to handle the Popup Edit actions

    @param PopupEdit ACTION_DATA *Data

    @retval status

**/
EFI_STATUS PopupEditHandleAction( POPUPEDIT_DATA *PopupEdit, ACTION_DATA *Data)
{
	UINT8 Redraw =0;
	//Always return success to consume the message, the popupedit is always modal.
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CONTROL_ACTION Action=ControlActionUnknown;
	
	if ( (Data->Input.Type == ACTION_TYPE_TIMER) || (Data->Input.Type == ACTION_TYPE_NULL) )	
		return Status;

	if ( Data->Input.Type == ACTION_TYPE_MOUSE )
	{
		Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);

	   if( (
	        (Data->Input.Data.MouseInfo.Top < PopupEdit->Top) ||
	        (Data->Input.Data.MouseInfo.Top > (UINT32)(PopupEdit->Top + PopupEdit->Height)) ||
	        (Data->Input.Data.MouseInfo.Left < PopupEdit->Left) ||
	        (Data->Input.Data.MouseInfo.Left > (UINT32)(PopupEdit->Left + PopupEdit->Width)) )
			&& ( (ControlActionChoose == Action) || (ControlActionSelect == Action) ) // To close popup window when mouse clicked outside(when SINGLE_CLICK_ACTIVATION is enabled)
	      )
		{	// Any action outside the popupedit about
			Action = ControlActionAbort;
		}

	}

	if (Data->Input.Type == ACTION_TYPE_KEY)
    {
        //Get mapping
        Action = MapControlKeysHook(Data->Input.Data.AmiKey);
	}

        switch(Action)
        {
            case ControlActionNextUp:
            case ControlActionNextDown:
            case ControlActionNextLeft:
            case ControlActionNextRight:
            case ControlActionNextSelection: 
    			Redraw = TRUE;
            break;

            case ControlActionAbort:
    			if(PopupEdit->ControlActive == TRUE)
    			{
    				MemCopy(PopupEdit->Text,PopupEdit->TempText,PopupEdit->TextWidth*sizeof(UINT16)); //UNICODE CHARS
    			}
                if(PopupEdit->Callback != NULL)
	            {
                    if ( PopupEdit->Cookie != NULL )
		            {
			            VOID *ifrData = (VOID *)PopupEdit->ControlData.ControlPtr;
			            CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)PopupEdit->Cookie;

			            callbackData->Variable = PopupEdit->ControlData.ControlVariable;
			            callbackData->Offset = UefiGetQuestionOffset(ifrData);
			            callbackData->Length = UefiGetWidth(ifrData);
			            callbackData->Data = (VOID *)PopupEdit->Text;
        
			            PopupEdit->Callback( PopupEdit->Container,PopupEdit, NULL );	
					}
				}

				PopupEdit->ControlActive = FALSE;
				Redraw = 1;
				Status = EFI_SUCCESS;
            break;

            case ControlActionSelect:
    			if(PopupEdit->Callback != NULL)
    			{
    				if ( PopupEdit->Cookie != NULL )
    				{
    					VOID *ifrData = PopupEdit->ControlData.ControlPtr;
    					CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)PopupEdit->Cookie;
    
    					callbackData->Variable = PopupEdit->ControlData.ControlVariable;
    					callbackData->Offset = UefiGetQuestionOffset(ifrData);
    					callbackData->Length = UefiGetWidth(ifrData);
    					callbackData->Data = (VOID *)PopupEdit->Text;
                    
    					PopupEdit->Callback( PopupEdit->Container,PopupEdit, PopupEdit->Cookie );	
    				}
    
    			}
    
    			Redraw =1;
    			PopupEdit->ControlActive =FALSE;
    			Status = EFI_SUCCESS;
            break;

            case ControlActionBackSpace:
    			if(EfiStrLen(PopupEdit->Text)>0)
    			{
    				if(PopupEdit->ControlActive ==FALSE)
    				{
    					MemCopy(PopupEdit->TempText,PopupEdit->Text,PopupEdit->TextWidth * sizeof(UINT16)); //UNICODE CHARS
    					PopupEdit->ControlActive = TRUE; 
    				}
    				PopupEdit->Text[EfiStrLen(PopupEdit->Text)-1] = 0x0;
				Redraw =1;
			}
        break;

        default:
			if ( Data->Input.Type == ACTION_TYPE_MOUSE )
				break;
	
			if(PopupEditHandlePassword( PopupEdit, Data) == EFI_SUCCESS)
    			Redraw =1;
				else
				{
	                if ((Data->Input.Data.AmiKey.Key.UnicodeChar) && (CHAR_CARRIAGE_RETURN != Data->Input.Data.AmiKey.Key.UnicodeChar)) // Avoid carriage return when press CTRL+ENTER in popupedit controls.
	                {
	                    if ( ( CheckIsAllowedPasswordChar(Data->Input.Data.AmiKey.Key.UnicodeChar) || ( PopupEdit->PopupEditType != POPUPEDIT_TYPE_PASSWORD ) ) &&
	                		( EfiStrLen(PopupEdit->Text) < PopupEdit->TextWidth ) )
	        			{
	        				if(PopupEdit->ControlActive ==FALSE)
	        				{
	                            MemCopy(PopupEdit->TempText,PopupEdit->Text,PopupEdit->TextWidth* sizeof(UINT16));
	        					PopupEdit->ControlActive = TRUE; 
	        				}
	        
	        				PopupEdit->Text[EfiStrLen(PopupEdit->Text)] = Data->Input.Data.AmiKey.Key.UnicodeChar ; 
	                        PopupEdit->Text[EfiStrLen(PopupEdit->Text)] = 0x0 ; 
	        				Redraw =1;
	        			}
						else {
							InvalidActionHookHook();		///
						}
	                }
					else if(!IsToggleStateKey(Data))	
					{
						InvalidActionHookHook();
					}

				}
            break;
        }


	
	
	if ( Redraw == TRUE )
	{	
		MouseFreeze();

		gPopupEdit.Draw( PopupEdit );
		SoftKbdRefresh = FALSE;
		MouseRefresh();	
	}

	return EFI_SUCCESS;
}

/**
    Function to set the PopupSel callback

    @param PopupEdit OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval STATUS

**/
EFI_STATUS PopupEditSetCallback( POPUPEDIT_DATA *PopupEdit, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( PopupEdit, container, callback, cookie );
}

/**
    Function to set the PopupEdit type

    @param PopupEdit UINT8 Type

    @retval STATUS

**/
EFI_STATUS PopupEditSetType( POPUPEDIT_DATA *PopupEdit, UINT8 Type )
{
	PopupEdit->PopupEditType = Type;
	if ( Type == POPUPEDIT_TYPE_PASSWORD )
		PopupEdit->Chr = PASSWORD_CHAR;

	return EFI_SUCCESS;
}

/**
    Function to set the PopupEdit text

    @param PopupEdit CHAR16 *String 

    @retval STATUS

**/
EFI_STATUS PopupEditSetText(POPUPEDIT_DATA *PopupEdit, CHAR16 *String )
{
    if(PopupEdit->Text==NULL)
    {
        if(String != NULL)
		{
            PopupEdit->Text = EfiLibAllocateZeroPool( (EfiStrLen(String )+1) * sizeof(CHAR16) );
		    EfiStrCpy(PopupEdit->Text, String);
		}
	    else
		{
			PopupEdit->Text = EfiLibAllocateZeroPool( (2) * sizeof(CHAR16) );
            EfiStrCpy(PopupEdit->Text, L"");
		}
	}
    else
	{
        if(String != NULL)
		  EfiStrCpy(PopupEdit->Text,String);
	    else
           EfiStrCpy(PopupEdit->Text, L"");

	}

	return EFI_SUCCESS;
}
/**
    Function to set focus

    @param PopupEdit BOOLEAN focus

    @retval Status

**/
EFI_STATUS PopupEditSetFocus(POPUPEDIT_DATA *PopupEdit, BOOLEAN focus)
{
	PopupEdit->ControlFocus = focus;
	return EFI_SUCCESS;
}
/**
    Function to set focus

    @param PopupEdit UINT16 Left, UINT16 Top

    @retval Status

**/
EFI_STATUS PopupEditSetPosition(POPUPEDIT_DATA *PopupEdit, UINT16 Left, UINT16 Top )
{
	return gPopup.SetPosition( (CONTROL_DATA*)PopupEdit, Left, Top );
}

/**
    function to set the dimentions of the editable popup menu

    @param PopupEdit UINT16 Width, UINT16 Height

    @retval status

**/
EFI_STATUS PopupEditSetDimensions(POPUPEDIT_DATA *PopupEdit, UINT16 Width, UINT16 Height )
{ 
	EFI_STATUS Status= EFI_SUCCESS;
    
	// dimensions include top, bottom, left and right lines for popup
	Status = gPopup.SetDimensions( (CONTROL_DATA*)PopupEdit, Width, Height ); 

	MemFreePointer( (VOID **)&PopupEdit->EmptyString );
	PopupEdit->EmptyString = EfiLibAllocateZeroPool( (Width-1 ) * sizeof(CHAR16) );
	if ( PopupEdit->EmptyString == NULL )
		return EFI_OUT_OF_RESOURCES;

	MemFillUINT16Buffer( PopupEdit->EmptyString, Width-2, L' ' );

	return(Status);
}
/**
    Function to set the dimentions of the editable popup menu

    @param PopupEdit UINT8 FGColor, UINT8 BGColor

    @retval status

**/
EFI_STATUS PopupEditSetAttributes(POPUPEDIT_DATA *PopupEdit, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes( (CONTROL_DATA*)PopupEdit, FGColor, BGColor );
}

/**
    Function unsuppored.

    @param object VOID *frame, UINT16 *height

    @retval status

**/
EFI_STATUS PopupEditGetControlHeight( POPUPEDIT_DATA *object,VOID *frame, UINT16 *height )
{
	return EFI_UNSUPPORTED;
}

/**
    function to create a new text without any justification.

    @param OrgText UINT16 width, UINT16 *height

    @retval text

**/
CHAR16 *StringWrapTextWithoutJustification ( CHAR16 *OrgText, UINT16 width, UINT16 *height )
{
    CHAR16 		*text;
    UINTN 		i = 0, j = 0;
    UINTN		NumOfExtraChars = 0;

	if(OrgText == NULL)
		return NULL;

    text = StrDup (OrgText);
    if (NULL == text)
    {
    	return OrgText;
    }
    NumOfExtraChars = (EfiStrLen (text))/width;
    if (0 == NumOfExtraChars)
    {
    	return OrgText;
    }
    else
    {
    	text = EfiLibAllocateZeroPool ((EfiStrLen (text) + NumOfExtraChars + 1) * sizeof (CHAR16));	//1 is for NULL char
    	if (NULL == text)
    	{
    		return OrgText;
    	}
    }
    //reformat input string by replacing \n and \r with \0
    i=0;
    j=0;
    while (OrgText [i])
    {
    	if (i >= width)
    	{
    		if (0 == (i  % width))
    		{
    			text [j] = L'\n';
    			j ++;
    			*height = (*height) + 1;
    		}
    	}
    	text [j] = OrgText [i];
    	j++;
    	i++;
    }
    return text;
}

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**       (C)Copyright 1985-2015, American Megatrends, Inc.     **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

