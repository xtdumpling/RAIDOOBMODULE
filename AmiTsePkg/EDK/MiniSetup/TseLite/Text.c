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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Text.c $
//
// $Author: Blaines $
//
// $Revision: 12 $
//
// $Date: 2/01/12 5:54p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file Text.c
    This file contains code to handle text operations

**/

#include "minisetup.h"

BOOLEAN Flag = FALSE;	

TEXT_METHODS gText =
{
	(OBJECT_METHOD_CREATE)TextCreate,
	(OBJECT_METHOD_DESTROY)TextDestroy,
	(OBJECT_METHOD_INITIALIZE)TextInitialize,
	(OBJECT_METHOD_DRAW)TextDraw,
	(OBJECT_METHOD_HANDLE_ACTION)TextHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)TextSetCallback,
	(CONTROL_METHOD_SET_FOCUS)TextSetFocus,
	(CONTROL_METHOD_SET_POSITION)TextSetPosition,
	(CONTROL_METHOD_SET_DIMENSIONS)TextSetDimensions,
	(CONTROL_METHOD_SET_ATTRIBUTES)TextSetAttributes,
	(CONTROL_METHOD_GET_CONTROL_HIGHT)TextGetControlHeight,
	TextSetText
};

/**
    this function uses the create function of control
    and creates text

    @param object 

    @retval status

**/
EFI_STATUS TextCreate( TEXT_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(TEXT_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gControl.Create((void**) object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gText;

	return Status;
}

/**
    this function uses the destroy function of control
    and destroys the text

    @param text BOOLEAN freeMem

    @retval status

**/
EFI_STATUS TextDestroy( TEXT_DATA *text, BOOLEAN freeMem )
{
	if(NULL == text)
	  return EFI_SUCCESS;

	gControl.Destroy( text, FALSE );

	MemFreePointer( (VOID **)&text->LabelText );
	MemFreePointer( (VOID **)&text->Text );

	if ( freeMem )
		MemFreePointer( (VOID **)&text );

	return EFI_SUCCESS;
}

/**
    this function uses the initialize function of control
    and initializes the text

    @param text VOID *data

    @retval status

**/
EFI_STATUS TextInitialize( TEXT_DATA *text, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	Status = gControl.Initialize( text, data );
	if (EFI_ERROR(Status))
		return Status;

	SetControlColorsHook(&(text->BGColor), &(text->FGColor), 
						NULL, NULL, 
						NULL, NULL,
	                    			NULL, NULL,
						NULL, NULL,
						NULL, 
						NULL, NULL,
						NULL,NULL);

	// add extra initialization here...
	text->ControlData.ControlHelp = UefiGetHelpField ((VOID*)text->ControlData.ControlPtr);
	//text->FGColor= EFI_DARKGRAY;
	StyleGetSpecialColor( CONTROL_TYPE_TEXT, &(text->FGColor)) ;

	text->LabelMargin = (UINT8)gControlLeftMargin;


	text->Interval = (UINT8)text->ControlData.ControlFlags.ControlRefresh;
	text->LabelText = HiiGetString( text->ControlData.ControlHandle, UefiGetTextField((VOID*)text->ControlData.ControlPtr));
//	if ( text->LabelText == NULL )						//In some case in init hiigetstring is failing but in draw its passing so commented this check.
//		return EFI_OUT_OF_RESOURCES;

	text->Text = HiiGetString( text->ControlData.ControlHandle, UefiGetTextTwoField((VOID *)text->ControlData.ControlPtr));
//	if ( text->Text == NULL )
//		return EFI_OUT_OF_RESOURCES;

	return Status;
}

/**
    function to draw the text with attributes.

    @param text 

    @retval status

**/
EFI_STATUS TextDraw( TEXT_DATA *text )
{
	CHAR16 *txt =NULL, *txt2 = NULL;
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ColorText = text->FGColor ;

	UINT16 height =0;
	CHAR16 *newtext = NULL;
	UINTN temp_width = 0;

	// check conditional ptr if necessary
    //Support grayout condition for readonly controls
	//if( text->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &text->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				Status = EFI_WARN_WRITE_FAILURE;
				ColorText = CONTROL_GRAYOUT_COLOR;
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}

	txt = HiiGetString( text->ControlData.ControlHandle, UefiGetTextField((VOID*)text->ControlData.ControlPtr));
	if ( txt == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

 	txt2 = HiiGetString( text->ControlData.ControlHandle, UefiGetTextTwoField((VOID*)text->ControlData.ControlPtr));
	if ( txt2 == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

/*
	gText.SetText(text,txt,txt2);
	MemFreePointer ((VOID **)&txt);
	MemFreePointer ((VOID **)&txt2);
*/

	if ( text->LabelText == NULL )
	{ 
		Status = EFI_UNSUPPORTED;
		goto DONE;
	}


	if(IsTSETextExcessSupport())
	{
		if((StrLen(txt2) == 0 &&  (text->ControlData.ControlFlags.ControlInteractive) == 0) && IsTSEMultilineControlSupported()) // check txt2 have strings or not in mutlicontrolSupported case
		{
			
			newtext = StringWrapText( text->LabelText,(UINTN)(text->LabelMargin - text->Left), &height ); // Wrap strings and get height of strings
			
					
			if(height > (UINTN)text->Height) //  compare height of whole text and upto which height we are printing strings (previously).
			{
				temp_width = (text->LabelMargin - text->Left)+ ((text->Width + text->Left)  - text->LabelMargin -gLabelLeftMargin); //get width of txt+txt2
				DrawMultiLineStringWithAttribute( (UINTN)text->Left, (UINTN) text->Top, 
									(UINTN)temp_width,(UINTN)text->Height,
									text->LabelText, text->BGColor |ColorText ); //print string on width of txt+txt2
				Flag = TRUE;
			
			}
			MemFreePointer( (VOID **)&newtext );
		}
	}
	gText.SetText(text,txt,txt2);
	// labeltext boundary overflow  check
	if(temp_width == 0)	 // check whether already string printed or need to print.
	{

//	if(text->Height>1 && IsTSEMultilineControlSupported())//EIP-72610 TSE_MULTILINE_CONTROLS moved to binary
	if(IsTSEMultilineControlSupported())//TSE_MULTILINE_CONTROLS moved to binary	//[EIP219275+]
	{
		DrawMultiLineStringWithAttribute( (UINTN)text->Left, (UINTN) text->Top, 
					(UINTN)(text->LabelMargin - text->Left),(UINTN)text->Height,
					text->LabelText, text->BGColor |ColorText );
	}
	else
	{
		if ( (TestPrintLength( text->LabelText ) / (NG_SIZE)) > (UINTN)(text->LabelMargin - text->Left  )/*21*/ )
				text->LabelText[HiiFindStrPrintBoundary(text->LabelText ,(UINTN)(text->LabelMargin - text->Left  ))] = L'\0';
	
		DrawStringWithAttribute( text->Left , text->Top, text->LabelText, text->BGColor |  ColorText /*text->FGColor*/  );
	}
	}	

	if ( text->Text != NULL )
	{
		//TSE_MULTILINE_CONTROLS moved to binary
		//in case of TSE_MULTILINE_CONTROLS is enabled and we will get text height is >= 1.if height is 1 and text characters is 24,we won't truncate the characters.   
		if( IsTSEMultilineControlSupported())
		{
				DrawMultiLineStringWithAttribute( (UINTN)text->Left + text->LabelMargin, (UINTN) text->Top, 
						(UINTN)((text->Width + text->Left)  - text->LabelMargin -gLabelLeftMargin),(UINTN)text->Height,
						text->Text, text->BGColor |ColorText );//Adjusted the cordinates to show maximum string in control left area

						
		}
		else
		{
			// text boundary overflow check
			//Adjusted the cordinates to show maximum string in control left area
	        //if text characters is more than 24,we will truncate the characters and display ... in last 3characters
		    if ( (TestPrintLength( text->Text ) / (NG_SIZE)) >= (UINTN)((text->Width + text->Left) - text->LabelMargin -gLabelLeftMargin) )
	      	  EfiStrCpy(&text->Text[HiiFindStrPrintBoundary(text->Text ,(UINTN)((text->Width + text->Left) - text->LabelMargin -gLabelLeftMargin-3))],L"..." ); 
	        DrawStringWithAttribute( text->Left + text->LabelMargin , text->Top, text->Text, text->BGColor |ColorText );
		}
	}

	//if TSE_MULTILINE_CONTROLS moved to binary
	if(IsTSEMultilineControlSupported())
	{
		FlushLines(text->Top, text->Top + text->Height - 1);
	}
	else
	{
		FlushLines(text->Top, text->Top);
	}
DONE:
   if(txt)
	   MemFreePointer ((VOID **)&txt);
   if(txt2)
		MemFreePointer ((VOID **)&txt2);
	return Status;
}

/**
    function to handle the text actions

    @param text ACTION_DATA *Data

    @retval status

**/
EFI_STATUS TextHandleAction( TEXT_DATA *text, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	if ( (Data->Input.Type == ACTION_TYPE_TIMER) && ((UINT8)text->ControlData.ControlFlags.ControlRefresh != 0))
	{
		Status = CallTextCallBack(text, Data);
	}

	return Status;
}

/**
    Function to set callback.

    @param text OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie 

    @retval STATUS

**/
EFI_STATUS TextSetCallback( TEXT_DATA *text, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( text, container, callback, cookie );
}
/**
    Function to set focus.

    @param text BOOLEAN focus

    @retval STATUS

**/
EFI_STATUS TextSetFocus(TEXT_DATA *text, BOOLEAN focus)
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set position.

    @param text UINT16 Left, UINT16 Top

    @retval STATUS

**/
EFI_STATUS TextSetPosition(TEXT_DATA *text, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition((CONTROL_DATA*) text, Left, Top );
}
/**
    Function to set text dimension.

    @param text UINT16 Width, UINT16 Height

    @retval STATUS

**/
EFI_STATUS TextSetDimensions(TEXT_DATA *text, UINT16 Width, UINT16 Height )
{
	return gControl.SetDimensions( (CONTROL_DATA*)text, Width, Height );
}
/**
    Function to set text attributes.

    @param text UINT8 FGColor, UINT8 BGColor

    @retval STATUS

**/
EFI_STATUS TextSetAttributes(TEXT_DATA *text, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes((CONTROL_DATA*) text, FGColor, BGColor );
}

/**
    function to set the text

    @param text CHAR16 *label, CHAR16 *value

    @retval status

**/
EFI_STATUS TextSetText( TEXT_DATA *text, CHAR16 *label, CHAR16 *value )
{
	if ( label != NULL )
	{
		MemFreePointer( (void**)&text->LabelText );
		text->LabelText = StrDup( label );
	}

	if ( value != NULL )
	{
		MemFreePointer( (void**)&text->Text );
		text->Text = StrDup( value );
	}

	return EFI_SUCCESS;
}


/**
    function to get the text height

    @param text Frame,  UINT16 *height

    @retval status

**/
EFI_STATUS TextGetControlHeight( TEXT_DATA *text, VOID* frame,  UINT16 *height )
{
	//if TSE_MULTILINE_CONTROLS moved to binary
	if(IsTSEMultilineControlSupported())
	{
		CHAR16 *newText = NULL;
		UINT16 tmpHeight=0;
		UINT16 Width;
		CHAR16 *Text1=NULL, *Text2=NULL;
	    
		//EIP-147142  
		//Make sure text->Label and text->text are updated before computing the control height 		
		Text1 = HiiGetString( text->ControlData.ControlHandle, UefiGetTextField((VOID*)text->ControlData.ControlPtr));
		Text2 = HiiGetString( text->ControlData.ControlHandle, UefiGetTextTwoField((VOID*)text->ControlData.ControlPtr));
		gText.SetText(text, Text1, Text2);
    
		Width = (UINT16)((text->LabelMargin - (((FRAME_DATA*)frame)->FrameData.Left + (UINT8)gLabelLeftMargin)));
		//
		//Frame width is the total page width - LabelMargin - 3(gLabelLeftMargin+ border line)
		//
		newText = StringWrapText( text->Text, (UINT16)(((FRAME_DATA *)frame)->FrameData.Width - text->LabelMargin -gLabelLeftMargin-1), &tmpHeight );
		*height = tmpHeight;
		MemFreePointer( (VOID **)&Text1 );
		MemFreePointer( (VOID **)&Text2 );
		MemFreePointer( (VOID **)&newText );

		if(IsTSETextExcessSupport())
		{
			if(Flag) // check printing the string on both txt and txt2 width
			{
				Width = (UINT16)((text->LabelMargin - text->Left)+ ((text->Width + text->Left)  - text->LabelMargin -gLabelLeftMargin)); // height will be adjusted according to width
			}
		}

		newText = StringWrapText( text->LabelText, Width, &tmpHeight );
	
		if(tmpHeight > *height)
			*height = tmpHeight;
	
		(*height) = (*height) ? (*height):1;
	
		MemFreePointer( (VOID **)&newText );
	}
	else
	{
		*height = 1;
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
