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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Memo.c $
//
// $Author: Rajashakerg $
//
// $Revision: 13 $
//
// $Date: 4/04/12 12:34a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#include "minisetup.h"

MEMO_METHODS gMemo =
{
	(OBJECT_METHOD_CREATE)MemoCreate,
	(OBJECT_METHOD_DESTROY)MemoDestroy,
	(OBJECT_METHOD_INITIALIZE)MemoInitialize,
	(OBJECT_METHOD_DRAW)MemoDraw,
	(OBJECT_METHOD_HANDLE_ACTION)MemoHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)MemoSetCallback,
	(CONTROL_METHOD_SET_FOCUS)MemoSetFocus,
	(CONTROL_METHOD_SET_POSITION)MemoSetPosition,
	(CONTROL_METHOD_SET_DIMENSIONS)MemoSetDimensions,
	(CONTROL_METHOD_SET_ATTRIBUTES)MemoSetAttributes,
	(CONTROL_METHOD_GET_CONTROL_HIGHT)MemoGetControlHeight,
	MemoSetJustify
};

/**
    Function to create a Memo, which uses the Control Functions.

    @param object 

    @retval status

**/
EFI_STATUS MemoCreate( MEMO_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(MEMO_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gControl.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gMemo;

	return Status;
}

/**
    Function to Destroy a Memo, which uses the Control Functions.

    @param memo BOOLEAN freeMem

    @retval status

**/
EFI_STATUS MemoDestroy( MEMO_DATA *memo, BOOLEAN freeMem )
{
	if(NULL == memo)
	  return EFI_SUCCESS;

	gControl.Destroy( memo, FALSE );

    if( freeMem ){
        MemFreePointer( (VOID **)&memo->EmptyString ); 
		MemFreePointer( (VOID **)&memo );
    }

	return EFI_SUCCESS;
}

/**
    Function to Initialize a Memo, which uses the Control Functions.

    @param memo VOID *data

    @retval status

**/
EFI_STATUS MemoInitialize( MEMO_DATA *memo, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	Status = gControl.Initialize( memo, data );
	if (EFI_ERROR(Status))
		return Status;

	// add extra initialization here...
	SetControlColorsHook( &(memo->BGColor), &(memo->FGColor), NULL, NULL ,NULL, NULL ,
	                      NULL , NULL,
						  NULL, NULL ,
						  NULL,NULL ,NULL,NULL, NULL );
	StyleGetSpecialColor( CONTROL_TYPE_MEMO, &(memo->FGColor)) ;
	memo->Interval = (UINT8)(memo->ControlData.ControlFlags.ControlRefresh);

	memo->Justify = JUSTIFY_LEFT;

	// initialization of ScrollBarPosition
	memo->ScrollBarPosition = 0;


	return Status;
}
/**
    Function to draw a Memo.

    @param memo 

    @retval status

**/
EFI_STATUS MemoDraw( MEMO_DATA *memo )
{
	CHAR16 *text = NULL, *newText = NULL;
	CHAR16 *line;
	UINT16 pos, height = 0;

	UINT16  TempPos = 0;
	BOOLEAN bScrollBar = FALSE;

	// check conditional ptr if necessary
	// Fix - Prevent memo draw if supressed condition
    
	//if( memo->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &memo->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				//Status = EFI_WARN_WRITE_FAILURE;
				memo->FGColor = CONTROL_GRAYOUT_COLOR;
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}
	
	text = HiiGetString( memo->ControlData.ControlHandle,UefiGetTitleField( (VOID *)memo->ControlData.ControlPtr));

	if ( text == NULL )
		return EFI_OUT_OF_RESOURCES;

	if ( EfiStrLen(text) == 0)
    {
        // String is empty but memory is allocated.
		MemFreePointer( (VOID **)&text );
		return EFI_OUT_OF_RESOURCES;
    }

	// clear out old wrapped string
	for ( pos = 0; pos < memo->Height; pos++ )
		DrawStringWithAttribute( memo->Left, memo->Top + pos, memo->EmptyString, memo->FGColor | memo->BGColor );

	// XXX: this wrapping code is probably not compatible with all languages
	newText = StringWrapText( text, memo->Width, &height );
	if(height == 0)
		return EFI_OUT_OF_RESOURCES;

	if(!IsTSEMultilineControlSupported() && memo->ParentFrameType == MAIN_FRAME){
		UINTN SLength = EfiStrLen(L"...");
	    height = 1;
	    if((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(memo->Width))
	    {
	       EfiStrCpy(newText,text);
	       EfiStrCpy( &newText[HiiFindStrPrintBoundary(newText,(UINTN)(memo->Width-SLength))],L"...");
	    }
	}

	// where height is the total number of Lines if it is greater than memo->Height(14)
	// then scroll bar need or not needed.

	bScrollBar = ( (height > memo->Height) && (memo->ParentFrameType == HELP_FRAME /*memo->Top == MAIN_TOP*/)  ) ? 1 : 0;

	if( memo->ScrollBarPosition > (height - memo->Height) )
		memo->ScrollBarPosition = (height - memo->Height);
	
	if ( newText != text )
		MemFreePointer( (VOID **)&text );

	line = newText;

	if((bScrollBar == FALSE) && (height >= memo->Height))
	{
			height = memo->Height;
	}

	// Draw complete strings in a help frame. Limits upto height of the string.
	for ( pos = 0; pos < height; pos++ )
	{
		CHAR16 save;

		text = line;
		if ( *line == L'\0' )
			break;

		while ( ( *line != L'\n' ) &&( *line != L'\r' ) && ( *line != L'\0' ) )
			line++;

		save = *line;
		*line = L'\0';

		if( (memo->ScrollBarPosition > pos) && ((height - memo->Height) > pos) )
		{
			TempPos++; 
		}
		else
		{
			// If height exceeds memo->Height 
			// Drawing alternative 14 lines in help frame.
			if( bScrollBar )
			{	
				if( (pos-TempPos) < memo->Height )
				{
					DrawStringJustifiedWithAttribute( memo->Left, memo->Left + memo->Width, memo->Top + (pos-TempPos),
						memo->Justify, text, memo->FGColor | memo->BGColor );
					
				}
			}
			// If No of Lines is less than Memo->Height
			// Drawing a string in a help frame
			else
			{
				memo->ScrollBarPosition = 0;
				DrawStringJustifiedWithAttribute( memo->Left, memo->Left + memo->Width, memo->Top + pos,
						memo->Justify, text, memo->FGColor | memo->BGColor );
			}
			
		}

		if ( ( *line = save ) != L'\0' )
		{
			line++;
			if ( ( *line == L'\r' ) || ( *line == L'\n' ) )
				line++;
		}
	}


	// Drawing Scrollbar For Help Frame.
	if( memo->ParentFrameType == HELP_FRAME )
	{
		// If scrollbar needed draw the scroll bar
		if( bScrollBar )
			StyleDrawHelpScrollBar(memo, height) ;
		else
			MemoEraseScrollBar(memo) ;	
	}

	if ( pos < height )
		height = pos;

	memo->DisplayLines = height;
	
	FlushLines( memo->Top, memo->Top + memo->Height -1 );
	if( ( memo->ParentFrameType == HELP_FRAME ) && ( bScrollBar ))
		DoRealFlushLines();
	if(newText)
		MemFreePointer( (VOID **)&newText );

	return EFI_SUCCESS;
}
/**
    Function to draw memo scrollbar

    @param memo UINT16 height

    @retval VOID

**/
VOID MemoDrawScrollBar( MEMO_DATA *memo, UINT16 height)
{
	#define VISIBLE_AREA_HEIGHT (memo->Height - 1)
	#define SCROLLBAR_AREA_HEIGHT (memo->Height)

	UINT16 pos=0 ;
	UINT32	FirstLine=0,LastLine=0,ScrollbarHeight = 0,j = 0;
	CHAR16	ArrUp[2]={GEOMETRICSHAPE_UP_TRIANGLE,0x0000},ArrDown[2]={GEOMETRICSHAPE_DOWN_TRIANGLE,0x0000},ScrlSel[2]={BLOCKELEMENT_FULL_BLOCK/*L'N'*/,0x0000},ScrlNSel[2]={BLOCKELEMENT_LIGHT_SHADE/*L'S'*/,0x0000};
		
	// Calculated scrollbar length based upon the height of the Lines.
	ScrollbarHeight = ( (SCROLLBAR_AREA_HEIGHT - 2) * VISIBLE_AREA_HEIGHT ) / height;
	
	// Calculated the First line of the scroll bar and last line of the scrollbar.		
	FirstLine = memo->ScrollBarPosition * ( (SCROLLBAR_AREA_HEIGHT - 2) - ScrollbarHeight ) / ( height - VISIBLE_AREA_HEIGHT ) + memo->Top + 1;
	
	LastLine  = FirstLine + ScrollbarHeight;

	for ( pos = 1,j = memo->Top; pos <= (UINT32)SCROLLBAR_AREA_HEIGHT ; pos++, j++ )
	{
		// Drawing Up Arrow
		if (pos == 1){
			    DrawStringWithAttribute( memo->Left + memo->Width , memo->Top, ArrUp, 
                                                StyleGetScrollBarUpArrowColor() );
		}
		// Drawing Down Arrow
                else if ( pos == (UINT32 )( SCROLLBAR_AREA_HEIGHT )){
                            DrawStringWithAttribute( memo->Left + memo->Width , memo->Top + (memo->Height-1), ArrDown, 
                                                 StyleGetScrollBarDownArrowColor() );	
		}
		// Drawing Scroll Bar(Scroll selector)
	        else if ( (j >= FirstLine) && (j <= LastLine) ){
		            DrawStringWithAttribute( memo->Left + memo->Width, j, ScrlSel, 
                                                 StyleGetScrollBarColor() );
		}
		// Drawing scroll Not Selector bar
                else{
			DrawStringWithAttribute( memo->Left + memo->Width, j, ScrlNSel, 
                                                 StyleGetScrollBarColor() );
		}
                       
	}
	
} 
/**
    Function to erase memo scrollbar

    @param memo 

    @retval VOID

**/
VOID MemoEraseScrollBar( MEMO_DATA *memo)
{
	UINT16 pos=0 ;
	UINT32 j=0 ;
	
	for ( pos = 0,j = memo->Top; pos < memo->Height; pos++,j++ )
		DrawStringWithAttribute(  memo->Left + memo->Width, j , L" ", 
					memo->FGColor | memo->BGColor );
}

/**
    Function to set callback.

    @param memo ACTION_DATA *Data

    @retval STATUS

**/
EFI_STATUS MemoHandleAction( MEMO_DATA *memo, ACTION_DATA *Data)
{
	// Handling action for help frame.

    CONTROL_ACTION Action;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	{
		if (memo->Interval == 0)
			return Status;
		if ( --(memo->Interval) == 0 )
		{
			// initialize the interval
			memo->Interval = (UINT8)(memo->ControlData.ControlFlags.ControlRefresh);
			return EFI_SUCCESS;
		}
		else
			return Status;
	}

	if ( Data->Input.Type != ACTION_TYPE_KEY )
	   return EFI_UNSUPPORTED;

 	//Get Key       
	Action = MapControlKeysHook(Data->Input.Data.AmiKey);

	
    switch ( Action )
    {
        case ControlActionHelpPageDown:

	// Identify the help frame by identifying the height of the help frame
	if( memo->ParentFrameType == HELP_FRAME)
	{
		// Incrementing the ScrollBarPosition.
		memo->ScrollBarPosition++;
		// Based upon ScrollBarPosition drawing the Memo again.
		gMemo.Draw(memo);
	}
            break;

        case ControlActionHelpPageUp:
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
	   Status = EFI_UNSUPPORTED;
    }


	return EFI_UNSUPPORTED;
}
/**
    Function to set callback.

    @param memo OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval STATUS

**/
EFI_STATUS MemoSetCallback( MEMO_DATA *memo, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set focus.

    @param memo BOOLEAN focus

    @retval STATUS

**/
EFI_STATUS MemoSetFocus(MEMO_DATA *memo, BOOLEAN focus)
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set position.

    @param memo UINT16 Left, UINT16 Top 

    @retval STATUS

**/
EFI_STATUS MemoSetPosition(MEMO_DATA *memo, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition((CONTROL_DATA*)memo, Left, Top );
}
/**
    Function to set attributes.

    @param memo UINT8 FGColor, UINT8 BGColor

    @retval STATUS

**/
EFI_STATUS MemoSetAttributes(MEMO_DATA *memo, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes( (CONTROL_DATA*)memo, FGColor, BGColor );
}
/**
    Function to set dimension.

    @param memo UINT16 width, UINT16 height

    @retval STATUS

**/
EFI_STATUS MemoSetDimensions( MEMO_DATA *memo, UINT16 width, UINT16 height )
{
	memo->Width = width;
	memo->Height = height;

	MemFreePointer( (VOID **)&memo->EmptyString );
	memo->EmptyString = EfiLibAllocateZeroPool( (width + 1) * sizeof(CHAR16) );
	if ( memo->EmptyString == NULL )
		return EFI_OUT_OF_RESOURCES;

	MemFillUINT16Buffer( memo->EmptyString, width, L' ' );
	return EFI_SUCCESS;
}
/**
    Function to set justify.

    @param memo UINT8 justify

    @retval STATUS

**/
EFI_STATUS MemoSetJustify( MEMO_DATA *memo, UINT8 justify )
{
	if ( justify <= JUSTIFY_MAX )
	{
		memo->Justify = justify;
		return EFI_SUCCESS;
	}

	return EFI_INVALID_PARAMETER;
}

/**
    Function to get the memo text height.

    @param memo UINT16 *height

    @retval status

**/
EFI_STATUS MemoGetControlHeight( MEMO_DATA *memo, VOID *frame, UINT16 *height )
{
	CHAR16 *text = NULL, *newText = NULL;

	if(frame != NULL)
	{ // Memo control from Frame
		memo->Width = ((FRAME_DATA*)frame)->FrameData.Width - (UINT8)gLabelLeftMargin - 2;
	}
	// else messagebox memo

	text = HiiGetString( memo->ControlData.ControlHandle,UefiGetTitleField( (VOID *)memo->ControlData.ControlPtr));

	if ( text == NULL )
		return EFI_OUT_OF_RESOURCES;

	// XXX: this wrapping code is probably not compatible with all languages
	newText = StringWrapText( text, memo->Width, height );
	if ( newText != text )
		MemFreePointer( (VOID **)&text );

	MemFreePointer( (VOID **)&newText );

	return EFI_SUCCESS;
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

