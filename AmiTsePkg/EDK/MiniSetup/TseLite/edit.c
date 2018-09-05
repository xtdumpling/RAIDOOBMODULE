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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/edit.c $
//
// $Author: Rajashakerg $
//
// $Revision: 11 $
//
// $Date: 4/05/12 7:14a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file edit.c
    This file contains code to handle Edit control operations

**/

#include "minisetup.h"


EDIT_METHODS gEdit =
{
	(OBJECT_METHOD_CREATE)EditCreate,
	(OBJECT_METHOD_DESTROY)EditDestroy,
	(OBJECT_METHOD_INITIALIZE)EditInitialize,
	(OBJECT_METHOD_DRAW)EditDraw,
	(OBJECT_METHOD_HANDLE_ACTION)EditHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)EditSetCallback,
	(CONTROL_METHOD_SET_FOCUS)EditSetFocus,
	(CONTROL_METHOD_SET_POSITION)EditSetPosition,
	(CONTROL_METHOD_SET_DIMENSIONS)EditSetDimensions,
	(CONTROL_METHOD_SET_ATTRIBUTES)EditSetAttributes,
	(CONTROL_METHOD_GET_CONTROL_HIGHT)EditGetControlHeight
	

};

EFI_STATUS _EditHandleActionKey(EDIT_DATA *edit, AMI_EFI_KEY_DATA key);
/**
    Function to create Edit Controls, which uses the control functions.

    @param object 

    @retval status

**/
EFI_STATUS EditCreate( EDIT_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(EDIT_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gControl.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gEdit;

	return Status;
}

/**
    Function to Destroy Edit Controls, which uses the control functions.

    @param edit BOOLEAN freeMem

    @retval status

**/
EFI_STATUS EditDestroy( EDIT_DATA *edit, BOOLEAN freeMem )
{
	if(NULL == edit)
	  return EFI_SUCCESS;

	gControl.Destroy( edit, FALSE );

	MemFreePointer( (VOID **)&edit->Text );
	MemFreePointer( (VOID **)&edit->TempText );

	if( freeMem )
		MemFreePointer( (VOID **)&edit );

	return EFI_SUCCESS;
}

/**
    Function to Initialize Edit Controls, which uses the control functions.

    @param edit VOID *data

    @retval status

**/
EFI_STATUS EditInitialize( EDIT_DATA *edit, VOID *data )
{
	EFI_STATUS Status =  EFI_UNSUPPORTED;

	Status = gControl.Initialize( edit, data );
	if (EFI_ERROR(Status))
		return Status;

	SetControlColorsHook(NULL, NULL,
						 NULL,NULL, 
						 NULL /*&(edit->SelBGColor )*/, &(edit->SelFGColor),
	                     			&(edit->BGColor), &(edit->FGColor),
						 NULL /*UINT8 *LabelBGColor*/,&(edit->LabelFGColor) /*UINT8 *LabelFGColor*/,
						 &(edit->NSelLabelFGColor)/*UINT8 *NSelLabelFGColor*/,
						 &(edit->SelBGColor )/*EditBGColor*/,&(edit->EditFGColor),
						 NULL, NULL );


	// add extra initialization here...
	edit->TextMargin = (UINT8)gControlLeftMargin;
	if ( edit->TextWidth == 0 )
	{
	    edit->TextWidth = UefiGetWidth(((CONTROL_INFO *)data)->ControlPtr);
		edit->TextWidth = (edit->TextWidth>1)?(3 * (UINT16)edit->TextWidth):4;
	}

	edit->TextAreaWidth = (edit->TextWidth </*10*/(UINT16)(edit->Width - edit->TextMargin - 2)) ? edit->TextWidth:/*10*/(UINT16)(edit->Width - edit->TextMargin - 2);
	edit->Text = EfiLibAllocateZeroPool((edit->TextWidth +1)* sizeof(CHAR16));
	MemSet( edit->Text, (edit->TextWidth +1)* sizeof(CHAR16), 0 );

	if(  *((UINT8*)((CONTROL_INFO *)data)->ControlPtr) == IFR_PASSWORD_OP)
		edit->Chr = L'*';

	if ( edit->Text != NULL )
	{
        VarGetValue( edit->ControlData.ControlVariable, 
		         UefiGetQuestionOffset(((CONTROL_INFO *)data)->ControlPtr), 
				 UefiGetWidth(((CONTROL_INFO *)data)->ControlPtr), edit->Text );
	  
	}
	edit->TempText = EfiLibAllocateZeroPool((edit->TextWidth +1)* sizeof(CHAR16));
	return EFI_SUCCESS;
}

/**
    Function to draw Edit Controls.

    @param edit 

    @retval status

**/
EFI_STATUS EditDraw(EDIT_DATA *edit )
{
	CHAR16 *text,*text2;
	UINT16 i;
	UINT8 BGClr,FGClr;
    
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ColorNSel = edit->FGColor;
	UINT8 ColorLabel = (edit->ControlFocus) ? edit->LabelFGColor : edit->NSelLabelFGColor ;
		
	
	if(edit->ControlFocus)
	{  
		BGClr = edit->SelBGColor;
		if(edit->ControlActive)
			FGClr =  edit->EditFGColor;  
		else
			FGClr =  edit->SelFGColor; 
	}
	else
	{ 
		BGClr =  edit->BGColor;
		FGClr = /*ColorLabel;*/ edit->FGColor;  
	}

	// check conditional ptr if necessary
		//if( edit->ControlData.ControlConditionalPtr != 0x0)
		//{
			switch( CheckControlCondition( &edit->ControlData ) )
			{
				case COND_NONE:
					break;
				case COND_GRAYOUT:
					Status = EFI_WARN_WRITE_FAILURE;
					FGClr = ColorNSel =  ColorLabel = CONTROL_GRAYOUT_COLOR;
					break;
				default:
					return EFI_UNSUPPORTED;
					break;
			}
		//}
 
	
	text = HiiGetString( edit->ControlData.ControlHandle, UefiGetPromptField(edit->ControlData.ControlPtr));
	if (NULL == text){
		text = EfiLibAllocateZeroPool(2*sizeof(CHAR16));
		if(!text)
			return EFI_NOT_FOUND;
		EfiStrCpy(text,L" ");
	}
	if(edit->Height>1 && IsTSEMultilineControlSupported())
	{
		DrawMultiLineStringWithAttribute( edit->Left , edit->Top, 
				(UINTN)(edit->TextMargin - edit->Left),(UINTN) edit->Height,
				text, edit->BGColor | ColorLabel );
	}
	else
	{
		// boundary overflow  check
		if ((TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(edit->TextMargin - edit->Left  )/*21*/ )
				text[HiiFindStrPrintBoundary(text,(UINTN)(edit->TextMargin - edit->Left  ))] = L'\0';
	
		DrawStringWithAttribute( edit->Left  , edit->Top, (CHAR16*)text, 
			edit->BGColor | ColorLabel/*((edit->ControlFocus)? edit->LabelFGColor : edit->NSelLabelFGColor )*/ );

	}

	for(i=0; i < edit->TextAreaWidth; i++)
		DrawStringWithAttribute( edit->Left + edit->TextMargin + i  , edit->Top, L" ", 
				BGClr | FGClr );

	if(edit->Chr != 0x0)
	{
		text2 = EfiLibAllocateZeroPool( ((TestPrintLength( edit->Text ) / (NG_SIZE))+1) * sizeof(CHAR16));
		for(i = 0; (i < (TestPrintLength( edit->Text ) / (NG_SIZE))) && (i < edit->TextAreaWidth); i++ )
			text2[i] = edit->Chr;

		DrawStringWithAttribute( edit->Left + edit->TextMargin, edit->Top, text2, 
				BGClr | FGClr );
		MemFreePointer((VOID **)&text2);
	}
	else
		DrawStringWithAttribute( edit->Left + edit->TextMargin  , edit->Top , edit->Text, 
				BGClr | FGClr );

    MemFreePointer((VOID **)&text);

    FlushLines( edit->Top, edit->Top );
    DoRealFlushLines();
		MouseRefresh();
    return Status;

}
/**
    Function to set callback.

    @param edit OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie 

    @retval STATUS

**/
EFI_STATUS EditSetCallback( EDIT_DATA *edit, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( edit, container, callback, cookie );
}

/**
    Function to handle Edit actions.

    @param edit ACTION_DATA *Data

    @retval status

**/
EFI_STATUS EditHandleAction( EDIT_DATA *edit, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;


    if ( Data->Input.Type == ACTION_TYPE_KEY )
        return _EditHandleActionKey(edit, Data->Input.Data.AmiKey);

	return Status;
}

/**
    Function to handle Edit actions.

    @param edit AMI_EFI_KEY_DATA key

    @retval EFI_STATUS

**/
EFI_STATUS _EditHandleActionKey(EDIT_DATA *edit, AMI_EFI_KEY_DATA key)
{
	UINT8 Redraw =0;
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINTN i;
    CONTROL_ACTION Action;


    //Get mapping
    Action = MapControlKeysHook(key);

    switch(Action)
    {
        case ControlActionAbort:
            if(edit->ControlActive == TRUE)
                Status = EFI_SUCCESS;
        case ControlActionNextUp:
        case ControlActionNextDown:
            if(edit->ControlActive == TRUE)
            {
                MemCopy(edit->Text,edit->TempText,edit->TextWidth*sizeof(UINT16)); //UNICODE CHARS
                edit->ControlActive = FALSE;
                Redraw = 1;
            }
        break;

        case ControlActionNextLeft:
        case ControlActionNextRight:
            if(edit->ControlActive == TRUE)
			   Status = EFI_SUCCESS; // cosume the action if editing
		break;

        case ControlActionSelect:
    		if(edit->Callback != NULL)
			{
				if ( edit->Cookie != NULL )
				{
					CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)edit->Cookie;

					callbackData->Variable = edit->ControlData.ControlVariable;
					callbackData->Offset = UefiGetQuestionOffset(edit->ControlData.ControlPtr);
					callbackData->Length = UefiGetWidth(edit->ControlData.ControlPtr);
					callbackData->Data = (VOID *)edit->Text;
                
					edit->Callback( edit->Container,edit, edit->Cookie );	
				}
			}

			Redraw =1;
			edit->ControlActive = FALSE;
			Status = EFI_SUCCESS;
        break;

        case ControlActionBackSpace:
			if(EfiStrLen(edit->Text)>0)
			{
				if(edit->ControlActive == FALSE)
				{
					MemCopy(edit->TempText,edit->Text,edit->TextWidth * sizeof(UINT16)); //UNICODE CHARS
					edit->ControlActive = TRUE; 
				}
				edit->Text[EfiStrLen(edit->Text)-1] = 0x0;
				Redraw =1;
			}
        break;

        default:
            if(SCAN_NULL == key.Key.ScanCode)
            {
    			if((EfiStrLen(edit->Text) < edit->TextWidth) || 
    				( (edit->ControlActive == FALSE) ) && (edit->ControlData.ControlType == CONTROL_TYPE_NUMERIC) )
    			{
    				if(edit->ControlActive ==FALSE)
    				{
                        MemCopy(edit->TempText,edit->Text,edit->TextWidth* sizeof(UINT16));
    					if(edit->ControlData.ControlType == CONTROL_TYPE_NUMERIC)
    					{
                            i = edit->TextWidth;
    						do	{
    						   edit->Text[i] = 0x0;
    						}while(i-- > 0 );
    					}
    					edit->ControlActive = TRUE; 
    				}
    
    				i = EfiStrLen(edit->Text);
    				edit->Text[i] = key.Key.UnicodeChar ; 
                    edit->Text[i+1] = 0x0 ; 
    				Redraw =1;
    			}
            }
       break;
    }

	if(Redraw)
		Status = gEdit.Draw(edit);

	return Status;
}
/**
    Function to set the edit focus.

    @param edit BOOLEAN focus

    @retval status

**/
EFI_STATUS EditSetFocus(EDIT_DATA *edit, BOOLEAN focus)
{
	if(focus != FALSE)
	{
		if ( CheckControlCondition( &edit->ControlData ) )
			return EFI_UNSUPPORTED;
	}

	if( !(edit->ControlFocus && focus) )
		edit->ControlFocus = focus;

	if(!focus) 
		edit->ControlActive = 0;
	return EFI_SUCCESS;
}
/**
    Function to set the edit position.

    @param edit UINT16 Left, UINT16 Top

    @retval status

**/
EFI_STATUS EditSetPosition(EDIT_DATA *edit, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition( (CONTROL_DATA*)edit, Left, Top );
}
/**
    Function to set the edit dimensions.

    @param edit UINT16 Width, UINT16 Height 

    @retval status

**/
EFI_STATUS EditSetDimensions(EDIT_DATA *edit, UINT16 Width, UINT16 Height )
{
	return gControl.SetDimensions((CONTROL_DATA*) edit, Width, Height );
}
/**
    Function to set the edit attributes.

    @param edit UINT8 FGColor, UINT8 BGColor

    @retval status

**/
EFI_STATUS EditSetAttributes(EDIT_DATA *edit, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes( (CONTROL_DATA*)edit, FGColor, BGColor );
}

/**
    Function unsuppored.

    @param object VOID *frame, UINT16 *height

    @retval status

**/
EFI_STATUS EditGetControlHeight( EDIT_DATA *object,VOID *frame, UINT16 *height )
{
	return EFI_UNSUPPORTED;
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
