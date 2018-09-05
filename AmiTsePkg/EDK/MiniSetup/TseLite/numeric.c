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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/numeric.c $
//
// $Author: Premkumara $
//
// $Revision: 26 $
//
// $Date: 5/28/12 11:52a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file numeric.c
    This file contains code to handle Label operations

**/

#include "minisetup.h"

NUMERIC_METHODS gNumeric =
{ 
		(OBJECT_METHOD_CREATE)NumericCreate,
		(OBJECT_METHOD_DESTROY)NumericDestroy,
		(OBJECT_METHOD_INITIALIZE)NumericInitialize,
		(OBJECT_METHOD_DRAW)NumericDraw,
		(OBJECT_METHOD_HANDLE_ACTION)NumericHandleAction,
		(OBJECT_METHOD_SET_CALLBACK)NumericSetCallback,
		(CONTROL_METHOD_SET_FOCUS)NumericSetFocus,
		(CONTROL_METHOD_SET_POSITION)NumericSetPosition,
		(CONTROL_METHOD_SET_DIMENSIONS)NumericSetDimensions,
		(CONTROL_METHOD_SET_ATTRIBUTES)NumericSetAttributes,
		(CONTROL_METHOD_GET_CONTROL_HIGHT)NumericGetControlHeight,
	
};
BOOLEAN SingleClickActivation (VOID); 
extern UINTN gInvalidRangeFailMsgBox;
static BOOLEAN NumericIncDec = TRUE;
BOOLEAN CheckKeyinHotKeysList(AMI_EFI_KEY_DATA Key);
EFI_STATUS NumericHandleActionKeyMouse(NUMERIC_DATA *numeric, ACTION_DATA *Data);
VOID NumSPrint(NUMERIC_DATA *numeric,NUMERIC_MIN_MAX_BASE Num);
VOID UpdateNumericDisplayString (CHAR16 **, INT64, UINT8, UINT16, UINT16);
VOID ClearNumericShiftState (AMI_EFI_KEY_DATA *);
VOID SetSoftKbdPosition(UINT32 Height, UINT32 Top);

/**
    function used for Numeric control Create, which uses the Edit functions.

    @param object 

    @retval status

**/
EFI_STATUS NumericCreate( NUMERIC_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(NUMERIC_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gEdit.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gNumeric;

	return Status;
}

/**
    function used to destroy Numeric controls, which uses the Edit functions.

    @param object BOOLEAN freeMem

    @retval status

**/
EFI_STATUS NumericDestroy( NUMERIC_DATA *numeric, BOOLEAN freeMem )
{
	if(NULL == numeric	)
	  return EFI_SUCCESS;
	
	gEdit.Destroy( numeric, FALSE );

	MemFreePointer( (VOID **)&numeric->Text );

	if( freeMem )
		MemFreePointer( (VOID **)&numeric );

	return EFI_SUCCESS;
}

/**
    function used to check the range for the negative numbers and positive nubmers.

    @param numeric NUMERIC_VALUE_BASE *Num

    @retval BOOLEAN

**/
BOOLEAN CheckForRange(NUMERIC_DATA *numeric, NUMERIC_VALUE_BASE *Num)
{
	if(numeric->Base == 32)
	{
		if(numeric->ControlData.ControlDataWidth == 1)
		{
			if(((INT8)*Num < (INT8)numeric->MinValue) ||((INT8)*Num > (INT8)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 2)
		{
			if(((INT16)*Num < (INT16)numeric->MinValue) ||((INT16)*Num > (INT16)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 4)
		{
			if(((INT32)*Num < (INT32)numeric->MinValue) ||((INT32)*Num > (INT32)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 8)
		{
			if(((INT64)*Num < (INT64)numeric->MinValue) ||((INT64)*Num > (INT64)numeric->MaxValue))
				return TRUE;
		}
		else 
			return FALSE;
	}
	else
	{
		if(((*Num) < numeric->MinValue) ||(*Num > numeric->MaxValue))
			return TRUE;
		else
			return FALSE;
	}
return FALSE;

}

/**
    function used to check the range for the negative numbers and positive nubmers from setup.

    @param numeric NUMERIC_VALUE_BASE *Num

    @retval BOOLEAN

**/
BOOLEAN CheckForRangefromsetup(NUMERIC_DATA *numeric, NUMERIC_VALUE_BASE *Num)
{
	if(numeric->Base == 32)
	{
		if(numeric->ControlData.ControlDataWidth == 1)
		{
			if(((INT8)*Num >= (INT8)numeric->MinValue) &&((INT8)*Num <= (INT8)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 2)
		{
			if(((INT16)*Num >= (INT16)numeric->MinValue) &&((INT16)*Num <= (INT16)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 4)
		{
			if(((INT32)*Num >= (INT32)numeric->MinValue) &&((INT32)*Num <= (INT32)numeric->MaxValue))
				return TRUE;
		}
		else if(numeric->ControlData.ControlDataWidth == 8)
		{
			if(((INT64)*Num >= (INT64)numeric->MinValue) &&((INT64)*Num <= (INT64)numeric->MaxValue))
				return TRUE;
		}
		else 
			return FALSE;
	}
	else
	{
		if(((*Num) >= numeric->MinValue) &&(*Num <= numeric->MaxValue))
			return TRUE;
		else
			return FALSE;
	}
	
	return FALSE;
}
/**
    function used to Initialize the Numeric controls, which uses the Edit functions.

    @param object VOID *data

    @retval status

**/
EFI_STATUS NumericInitialize( NUMERIC_DATA *numeric, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	NUMERIC_VALUE_BASE *Num;
	VOID * ControlPtr = ((CONTROL_INFO *)data)->ControlPtr;
	UINT16 Size = UefiGetWidth(ControlPtr);

	// If the size 1 (it can hold upto 3 Decimal data +1 for null, is more then one the size * 3 is ok
	numeric->TextWidth = (Size>1)?(3 * (UINT16)Size):4;

	Status = gEdit.Initialize( numeric, data );
	if (EFI_ERROR(Status))
		return Status;

	// add extra initialization here...
	//numeric->FGColor = EFI_YELLOW;
	//numeric->BGColor = EFI_BACKGROUND_BLUE;
	//numeric->TextWidth = numeric->TextAreaWidth;
	numeric->Chr = 0x0;
	numeric->MinValue = UefiGetMinValue(ControlPtr);
	numeric->MaxValue = UefiGetMaxValue(ControlPtr);
	numeric->Step = UefiGetStepValue(ControlPtr);
    numeric->ControlData.ControlDataWidth = Size;
	numeric->Base = UefiGetBaseValue(ControlPtr); // default base decimal number
	numeric->Interval = (UINT8)(numeric->ControlData.ControlFlags.ControlRefresh);

	Num = EfiLibAllocateZeroPool( (Size <sizeof(NUMERIC_MIN_MAX_BASE))? sizeof(NUMERIC_MIN_MAX_BASE):Size );

	if ( Num != NULL )
	{
		VarGetValue( numeric->ControlData.ControlVariable, 
				UefiGetQuestionOffset(ControlPtr), Size, Num );

		// check that value is in range
		if(CheckForRange( numeric, Num))
		{
			// try to use the default
			if(numeric->ControlData.ControlDataWidth != 0)
			{
			    VarGetDefaults( numeric->ControlData.ControlVariable, 
								UefiGetQuestionOffset(ControlPtr),
								Size, Num );
			}
		}

        if(*Num < numeric->MinValue)
			*Num = numeric->MinValue;//revert to minimum value 

        if(*Num > numeric->MaxValue)
			*Num =numeric->MaxValue;//revert to maximum value 

		numeric->Value = *Num;

		MemFreePointer( (VOID **)&Num );
	}

	NumSPrint(numeric,numeric->Value);

	return EFI_SUCCESS;
}

/**
    function used to draw the numeric controls

    @param object 

    @retval status

**/
EFI_STATUS NumericDraw(NUMERIC_DATA *numeric )
{
	NUMERIC_MIN_MAX_BASE *Num;
	VOID * ControlPtr = numeric->ControlData.ControlPtr;

	if(numeric->ControlActive == TRUE)
		return gEdit.Draw( numeric );

	// check conditional ptr if necessary
	//if( numeric->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &numeric->ControlData ) )
		{
			case COND_NONE:
			case COND_GRAYOUT:
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}

	Num = EfiLibAllocateZeroPool((numeric->ControlData.ControlDataWidth<sizeof(NUMERIC_MIN_MAX_BASE))?sizeof(NUMERIC_MIN_MAX_BASE):numeric->ControlData.ControlDataWidth);

	if ( Num != NULL )
	{
		VarGetValue( numeric->ControlData.ControlVariable, 
				UefiGetQuestionOffset(ControlPtr),
				numeric->ControlData.ControlDataWidth, Num );

		// check that value is in range
		if(CheckForRange(numeric, Num))
		{
			//Report Message box
//	  		CallbackShowMessageBox( (UINTN)gInvalidRangeFailMsgBox, MSGBOX_TYPE_OK );

			// try to use the default
			if(numeric->ControlData.ControlDataWidth != 0)
			{
			    VarGetDefaults( numeric->ControlData.ControlVariable, 
								UefiGetQuestionOffset(ControlPtr),
								numeric->ControlData.ControlDataWidth, Num );
			}
		 if(*Num < numeric->MinValue)
			*Num = numeric->MinValue;//revert to minimum value 

        if(*Num > numeric->MaxValue)
			*Num =numeric->MaxValue;//revert to maximum value 
		}

		numeric->Value = *Num;

		MemFreePointer( (VOID **)&Num );
	}

	NumSPrint(numeric,numeric->Value);
#if 0
	/*To eliminate leading zeroes*/
	Num = atoi_base( numeric->Text, numeric->Base );
	SPrint( numeric->Text, numeric->TextAreaWidth, L"%d", Num);
#endif
	return gEdit.Draw( numeric );
}
/**
    function used for setting Numeric control callback.

    @param numeric OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval status

**/
EFI_STATUS NumericSetCallback( NUMERIC_DATA *numeric, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gEdit.SetCallback( numeric, container, callback, cookie );
}

/**
    function used handle the Numeric controls

    @param numeric ACTION_DATA *Data

    @retval status

**/
EFI_STATUS NumericHandleAction( NUMERIC_DATA *numeric, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;


	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	{
		if(numeric->ControlActive)
			return Status;

		if (numeric->Interval == 0)
			return Status;

		if(IsActiveControlPresent(gApp->PageList[gApp->CurrentPage]))
			return Status;

		if ( --(numeric->Interval) == 0 )
		{
			// initialize the interval
			numeric->Interval = (UINT8)(numeric->ControlData.ControlFlags.ControlRefresh);
			return UefiRefershQuestionValueNvRAM(&(numeric->ControlData));
		}
		else
			return Status;
	}

     if (( Data->Input.Type == ACTION_TYPE_KEY )||(Data->Input.Type == ACTION_TYPE_MOUSE))
		return NumericHandleActionKeyMouse(numeric, Data);

    return Status;
}

/**
    function used handle the Numeric controls Increment and Decreament operation

    @param Num NUMERIC_DATA *numeric,  CONTROL_ACTION Action,UINT8 *bValueChanged 

    @retval VOID

**/
VOID CheckforIncrementandDecreament(NUMERIC_MIN_MAX_BASE *Num, NUMERIC_DATA *numeric,  CONTROL_ACTION Action,UINT8 *bValueChanged )
{
	if(ControlActionIncreament == Action)
	{
		if(AMI_BASE_INT_DEC == numeric->Base)
		{
				if(numeric->ControlData.ControlDataWidth == 1)
				{
					if ( ((INT8)*Num + (INT8)numeric->Step) <= (INT8)numeric->MaxValue )
					{
						*Num = *Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;	
					}
				}
				else if(numeric->ControlData.ControlDataWidth == 2)
				{
					if ( ((INT16)*Num + (INT16)numeric->Step) <= (INT16)numeric->MaxValue )
					{
						*Num = *Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
				else if(numeric->ControlData.ControlDataWidth == 4)
				{
					if ( ((INT32)*Num + (INT32)numeric->Step) <= (INT32)numeric->MaxValue )
					{
						*Num = *Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
				else
				{
					if ( ((INT64)*Num + (INT64)numeric->Step) <= (INT64)numeric->MaxValue )
					{
						*Num = *Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
			
		}
		else
		{
			if ( *Num + numeric->Step <= numeric->MaxValue )
			{
							*Num = *Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
							*bValueChanged = 1;
			}
		}
	}
	else
	{
		if(AMI_BASE_INT_DEC == numeric->Base)
		{
				if(numeric->ControlData.ControlDataWidth == 1)
				{		
					if ( ((INT8)*Num - (INT8)numeric->Step) >= (INT8)numeric->MinValue)
					{
						*Num = *Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
				else if(numeric->ControlData.ControlDataWidth == 2)
				{
					if ( ((INT16)*Num - (INT16)numeric->Step) >= (INT16)numeric->MinValue )
					{
						*Num = *Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
				else if(numeric->ControlData.ControlDataWidth == 4)
				{
					if ( ((INT32)*Num - (INT32)numeric->Step) >= (INT32)numeric->MinValue)
					{
						*Num = *Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}
				else
				{
					if ( ((INT64)*Num - (INT64)numeric->Step) >= (INT64)numeric->MinValue )
					{
						*Num = *Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
						*bValueChanged = 1;
						
					}
				}

			
		}
		else
		{
			if( *Num - numeric->Step >= numeric->MinValue )
			 {
				*Num = *Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
				*bValueChanged = 1;
			 }
		}
			
	}


}


/**
    function used handle the Numeric control keys

    @param numeric ACTION_DATA *Data

    @retval status

**/
EFI_STATUS NumericHandleActionKeyMouse(NUMERIC_DATA *numeric, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CHAR16 Chr;
	BOOLEAN DataOk = FALSE;
	NUMERIC_MIN_MAX_BASE Num;
	ACTION_DATA TempData,*ClearActionData = NULL;
    CONTROL_ACTION Action;
    AMI_EFI_KEY_DATA key;
	VOID * ControlPtr = numeric->ControlData.ControlPtr;
    UINT8 u8ChkResult;

    u8ChkResult = CheckControlCondition( &numeric->ControlData );
	 
	 //Not to perform action for control when token TSE_SETUP_GRAYOUT_SELECTABLE is enable to set focus for GrayoutIf control
    if ( IsGrayoutSelectable() && (COND_GRAYOUT == u8ChkResult) ) {
        return EFI_UNSUPPORTED;
    }

    if(numeric->ControlData.ControlFlags.ControlReadOnly)
		return EFI_UNSUPPORTED;

    key  = Data->Input.Data.AmiKey;
    //Get mapping
	if(Data->Input.Type == ACTION_TYPE_KEY)
	{	
      Action = MapControlKeysHook(key);
	}
	else if(Data->Input.Type == ACTION_TYPE_MOUSE)
	{
		Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);
		//check whether MouseTop is within the Height and Width of Neumeric Control or not
		if	(	(ControlActionSelect == Action) &&
			 	(	(Data->Input.Data.MouseInfo.Top >= (UINT32)numeric->Top) && 
					(Data->Input.Data.MouseInfo.Top < (UINT32)(numeric->Top+numeric->Height)) &&
					(Data->Input.Data.MouseInfo.Left >= (UINT32)numeric->Left) && 
					(Data->Input.Data.MouseInfo.Left < (UINT32)(numeric->Left+numeric->Width))
				)//when mouse is clicked on numeric control and action is ControlActionSelect
		)
		{
			Action = ControlActionSelect;		
			if(!TSEMouseIgnoreMouseActionHook())
			{
				if (numeric->Base == AMI_BASE_INT_DEC || numeric->Base == AMI_BASE_DEC)
				{
					NumericSoftKbdInit();
					SetSoftKbdPosition(numeric->Height, numeric->Top);
					TSEStringReadLoopEntryHook();
				}
				else if (numeric->Base == AMI_BASE_HEX || numeric->Base == AMI_BASE_OCT)
				{
					PrintableKeysSoftKbdInit();
					SetSoftKbdPosition(numeric->Height, numeric->Top);
					TSEStringReadLoopEntryHook();
				}
			}
		}
		//Close softkbd if activatd when clicked/touch outside numeric
		else if ( ((!((Data->Input.Data.MouseInfo.Top >= (UINT32)numeric->Top) && (Data->Input.Data.MouseInfo.Top < (UINT32)(numeric->Top+numeric->Height))&& (Data->Input.Data.MouseInfo.Left >= (UINT32)numeric->Left) && (Data->Input.Data.MouseInfo.Left < (UINT32)(numeric->Left+numeric->Width)))))
					&&(TSEMouseIgnoreMouseActionHook())&&(( ControlMouseActionLeftDown== Action)||(ControlMouseActionLeftUp == Action))
		)
		{
			if (numeric->Base == AMI_BASE_INT_DEC || numeric->Base == AMI_BASE_DEC)
				NumericSoftKbdExit();
			else if (numeric->Base == AMI_BASE_HEX || numeric->Base == AMI_BASE_OCT)
			{
				PrintableKeysSoftKbdExit();
			}		
		}

		//If mouse clicked outside numeric control area
		else if	( ((Data->Input.Data.MouseInfo.Top < (UINT32)numeric->Top) ||
						(Data->Input.Data.MouseInfo.Top >= (UINT32)(numeric->Top+numeric->Height)) ||
						(Data->Input.Data.MouseInfo.Left < (UINT32)numeric->Left) ||
						(Data->Input.Data.MouseInfo.Left > (UINT32)(numeric->Left+numeric->Width))
						)&& ( (ControlActionSelect == Action) && SingleClickActivation() ) 
		)
		{
			return EFI_UNSUPPORTED;	
		}

		//If mouse clicked in numeric control area including multi-line string also don't consume mouse action in numeric side.
		else if ((Data->Input.Data.MouseInfo.Top >= (UINT32)numeric->Top) && 
					(Data->Input.Data.MouseInfo.Top < (UINT32)(numeric->Top+numeric->Height)) && 
					(Data->Input.Data.MouseInfo.Left >= (UINT32)numeric->Left) && 
					(Data->Input.Data.MouseInfo.Left < (UINT32)(numeric->Left+numeric->Width))&& 
					(numeric->ControlActive)
		)
		{
			return EFI_UNSUPPORTED;	
		}

	}
	
    switch(Action)
    {
        case ControlActionSelect:      		
			// check that the data is proper.
			if((numeric->Text[0] ==0x0)||((numeric->Base == AMI_BASE_INT_DEC)&&(L'-' == numeric->Text[0])&&( EfiStrStr(numeric->Text+1, L"-"))))
					 NumSPrint(numeric,numeric->MinValue);
			
			Num = (NUMERIC_MIN_MAX_BASE)atoi_base( numeric->Text, numeric->Base );

			// to support signed integers for NUMERIC opcode
			if ( CheckForRangefromsetup( numeric, &Num) )
			{
				if ( ( numeric->Callback != NULL ) && ( numeric->Cookie != NULL ) )
				{
					CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)numeric->Cookie;

					callbackData->Variable = numeric->ControlData.ControlVariable;
					callbackData->Offset = UefiGetQuestionOffset(ControlPtr);
					callbackData->Length = UefiGetWidth(ControlPtr);
					callbackData->Data = (VOID *)&Num;
					// To support UEFI 2.1.C spec to Not to update the control when Callback fails.
					UefiPreControlUpdate(&(numeric->ControlData));
					numeric->Callback( numeric->Container,numeric, numeric->Cookie );
				}

                {
					if(UefiIsInteractive(&numeric->ControlData))
                    {
            			VOID * Handle=numeric->ControlData.ControlHandle;
            			UINT16 Key = UefiGetControlKey(&(numeric->ControlData));
            			CONTROL_DATA *Control;
            
            			Status = CallFormCallBack(&(numeric->ControlData), Key, 0, AMI_CALLBACK_CONTROL_UPDATE);// Updated the action to AMI_CALLBACK_CONTROL_UPDATE
            			Control = GetUpdatedControlData((CONTROL_DATA*)numeric,CONTROL_TYPE_NUMERIC,Handle,Key);

            			if(Control == NULL) // Control deleted ?
            				return EFI_SUCCESS;
            			if(Control != (CONTROL_DATA*)numeric)
            				numeric = (NUMERIC_DATA*)Control; //control Updated

                    }
                }

				Status = EFI_SUCCESS;				
				numeric->ControlActive = FALSE;
				if(((Data->Input.Type == ACTION_TYPE_KEY))&&(TSEMouseIgnoreMouseActionHook() == TRUE)&& (TRUE == NumericIncDec))
				{
					if (numeric->Base == AMI_BASE_INT_DEC || numeric->Base == AMI_BASE_DEC)
						NumericSoftKbdExit();
					else if (numeric->Base == AMI_BASE_HEX || numeric->Base == AMI_BASE_OCT)
					{
						PrintableKeysSoftKbdExit();
					}
				}
				else if(!NumericIncDec)
				{
					NumericIncDec = TRUE;	
				}
			}
			else
			{
				ClearActionData = EfiLibAllocateZeroPool(sizeof(ACTION_DATA));
				if(ClearActionData)
				{
					while(gAction.GetAction( ClearActionData ) == EFI_SUCCESS)
					{ 
						if((ClearActionData->Input.Type == ACTION_TYPE_TIMER) || (ClearActionData->Input.Type == ACTION_TYPE_NULL))
							break;
						gAction.ClearAction( ClearActionData );
					}
					MemFreePointer((void**)&ClearActionData);
				}
		  		CallbackShowMessageBox( (UINTN)gInvalidRangeFailMsgBox, MSGBOX_TYPE_OK );
				Data->Input.Type = ACTION_TYPE_KEY;// Providing the input type as key such that it can be consumed in edithandleaction 
				Data->Input.Data.AmiKey.Key.ScanCode = SCAN_ESC;
				Data->Input.Data.AmiKey.Key.UnicodeChar = L'\0';
				ClearNumericShiftState(&(Data->Input.Data.AmiKey));
				//Data->Input.Data.AmiKey.KeyState.KeyShiftState = 0;//providing the key shiftstate
				DataOk = TRUE;
			}
        break;

        case ControlActionBackSpace:
    		DataOk = TRUE;
        break;

        case ControlActionIncreament:
        case ControlActionDecreament:
			if ( ( numeric->Step != 0 ) && ( ! numeric->ControlActive ))
			{
				UINT8 bValueChanged = 0;

				Num = (NUMERIC_MIN_MAX_BASE)atoi_base( numeric->Text, numeric->Base );
				if ( ControlActionIncreament == Action )
				{
					if(Num == numeric->MaxValue);
					else
					 CheckforIncrementandDecreament(&Num,numeric,Action,&bValueChanged);
                   /*     if ( Num + numeric->Step <= numeric->MaxValue )
						{
							Num = Num + (NUMERIC_MIN_MAX_BASE)numeric->Step;
							bValueChanged = 1;
						}*/
				}
				else if ( ControlActionDecreament == Action )
				{
					if(Num == numeric->MinValue);
				    else
					CheckforIncrementandDecreament(&Num,numeric,Action,&bValueChanged);
                   /* if( Num - numeric->Step >= numeric->MinValue )
					{
						Num = Num - (NUMERIC_MIN_MAX_BASE)numeric->Step;
						bValueChanged = 1;
					}*/
				}

                NumSPrint(numeric,Num);
				NumericIncDec = FALSE;
				if((bValueChanged)&& CheckForRangefromsetup( numeric, &Num))
				{
					MemCopy(&TempData,Data,sizeof(ACTION_DATA));
					TempData.Input.Data.AmiKey.Key.ScanCode = 0;
					TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
					ClearNumericShiftState(&(TempData.Input.Data.AmiKey));//Clearing the Shiftstate before invoking numerichandleaction with CHAR_CARRIAGE_RETURN
					gNumeric.HandleAction(numeric,&TempData);
				}
				// Data updated to variables Now draw it in the screen.
				Status = gNumeric.Draw( numeric );
			}
			else if( ( numeric->ControlActive )&&(numeric->Base == AMI_BASE_INT_DEC))
			{
				DataOk = TRUE;
			}
			
        break;

        case ControlActionAlpha:
        case ControlActionNumeric:
            Chr = key.Key.UnicodeChar;
            switch ( numeric->Base )
			{
					case AMI_BASE_BIN:
						if ( ( Chr == L'0' ) || ( Chr == L'1' ) )
							DataOk = TRUE;
						break;

					case AMI_BASE_OCT:
						if ( ( Chr <= L'7' ) && ( Chr >= L'0' ) )
							DataOk = TRUE;
						break;

					case AMI_BASE_DEC:
                    case AMI_BASE_INT_DEC:
						if ( ( Chr <= L'9' ) && ( Chr >= L'0' ) )
							DataOk = TRUE;
						break;

					case AMI_BASE_HEX:
						// To support uppercase Hex numbers
						if ( ( ( Chr <= L'9' ) && ( Chr >= L'0' ) ) || ( ( Chr >= L'a' ) && ( Chr <= L'f' ) ) || ( ( Chr >= L'A' ) && ( Chr <= L'F' ) ))

							DataOk = TRUE;
						break;
			}
        break;

        case ControlActionNextUp:
        case ControlActionNextDown:
        case ControlActionNextLeft:
        case ControlActionNextRight:
			if ( numeric->ControlActive )
			{
				MemCopy(&TempData,Data,sizeof(ACTION_DATA));
				TempData.Input.Data.AmiKey.Key.ScanCode = 0;
			    TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
				ClearNumericShiftState(&(TempData.Input.Data.AmiKey));//Clearing the Shiftstate before invoking numerichandleaction with CHAR_CARRIAGE_RETURN
				gNumeric.HandleAction(numeric,&TempData);
			} 
        break;

        default:
			if ( numeric->ControlActive )
			{
				MemCopy(&TempData,Data,sizeof(ACTION_DATA));
				TempData.Input.Type = ACTION_TYPE_KEY;
				TempData.Input.Data.AmiKey.Key.ScanCode = 0;
			    TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
				ClearNumericShiftState(&(TempData.Input.Data.AmiKey));
				//TempData.Input.Data.AmiKey.KeyState.KeyShiftState = 0;
				gNumeric.HandleAction(numeric,&TempData);
			} 
			// Only Alpha Numeric chars are allowed. Other Special printable chars not allowed.
			if(Data->Input.Data.AmiKey.Key.ScanCode)
	            DataOk = TRUE;
        break;
    }

	if ( DataOk )	
		Status = gEdit.HandleAction( numeric, Data );

	if((((!numeric->ControlActive)||(!IsMouseClickedonSoftkbd())) && (TSEMouseIgnoreMouseActionHook() == TRUE) )
		&&	(((Data->Input.Type == ACTION_TYPE_KEY) && ((Action == ControlActionNextLeft) ||(Action == ControlActionNextRight) ||(Action == ControlActionNextUp) ||(Action == ControlActionNextDown) ||(Action == ControlActionPageUp) ||(Action == ControlActionPageDown) ||(Action == ControlActionAbort) ||(Action == ControlActionHome) ||(Action == ControlActionEnd) ||(CheckKeyinHotKeysList(key))) ) 
				|| ((Data->Input.Type == ACTION_TYPE_MOUSE)&&((!((Data->Input.Data.MouseInfo.Top >= (UINT32)numeric->Top) && (Data->Input.Data.MouseInfo.Top < (UINT32)(numeric->Top+numeric->Height))&& (Data->Input.Data.MouseInfo.Left >= (UINT32)numeric->Left) && (Data->Input.Data.MouseInfo.Left < (UINT32)(numeric->Left+numeric->Width))))||(Action == ControlActionAbort))))
			)
	{
		if (numeric->Base == AMI_BASE_INT_DEC || numeric->Base == AMI_BASE_DEC)
				NumericSoftKbdExit();
		else if (numeric->Base == AMI_BASE_HEX || numeric->Base == AMI_BASE_OCT)
		{
			PrintableKeysSoftKbdExit();
		}
	}
	return Status;
}
/**
    Function to set focus.

    @param numeric BOOLEAN focus

    @retval status

**/
EFI_STATUS NumericSetFocus( NUMERIC_DATA *numeric, BOOLEAN focus )
{
	 UINT8 u8ChkResult;
	 
	if(focus != FALSE)
	{
		u8ChkResult = CheckControlCondition( &numeric->ControlData );
		
		//Setting focus to control which has no control-condtion and token TSE_SETUP_GRAYOUT_SELECTABLE is enable to set focus for GrayoutIf control
		if ( ((u8ChkResult != COND_NONE) && (u8ChkResult != COND_GRAYOUT)) ||
		     (!IsGrayoutSelectable() && (u8ChkResult == COND_GRAYOUT))
		   ){
			return EFI_UNSUPPORTED;
		}
	}


	
	if( !(numeric->ControlFocus && focus) )
		numeric->ControlFocus = focus;
	return EFI_SUCCESS;
}
/**
    Function to set position.

    @param numeric UINT16 Left, UINT16 Top 

    @retval status

**/
EFI_STATUS NumericSetPosition(NUMERIC_DATA *numeric, UINT16 Left, UINT16 Top )
{
	return gEdit.SetPosition( (CONTROL_DATA*)numeric, Left, Top );
}
/**
    Function to set dimensions.

    @param numeric UINT16 Width, UINT16 Height

    @retval status

**/
EFI_STATUS NumericSetDimensions(NUMERIC_DATA *numeric, UINT16 Width, UINT16 Height )
{
	return gEdit.SetDimensions((CONTROL_DATA*) numeric, Width, Height );
}
/**
    Function to set attributes.

    @param numeric UINT8 FGColor, UINT8 BGColor

    @retval status

**/
EFI_STATUS NumericSetAttributes(NUMERIC_DATA *numeric, UINT8 FGColor, UINT8 BGColor )
{
	return gEdit.SetAttributes( (CONTROL_DATA*)numeric, FGColor, BGColor );
}
/**
    Function to get label height.

    @param numeric UINT16 Width, UINT16 *height

    @retval status

**/
EFI_STATUS NumericGetControlHeight(NUMERIC_DATA *numeric, VOID* frame , UINT16 *height)
{
	//TSE_MULTILINE_CONTROLS moved to binary
	if(IsTSEMultilineControlSupported())
	{
		CHAR16 *newText = NULL,*text=NULL;
		UINT16 Width;
	
		Width = (UINT16)(((EDIT_DATA *)numeric)->TextMargin - (((FRAME_DATA*)frame)->FrameData.Left + (UINT8)gControlLeftPad));
		text = HiiGetString( numeric->ControlData.ControlHandle, UefiGetPromptField((VOID *)numeric->ControlData.ControlPtr));
		if ( text == NULL )
			return EFI_OUT_OF_RESOURCES;
	
		newText = StringWrapText( text, Width, height );
	
	    (*height) = (*height) ? (*height):1;
	
	    MemFreePointer( (VOID **)&newText );
		MemFreePointer( (VOID **)&text );
	}
	else
	{
		*height =1;
	}
	return EFI_SUCCESS;
}

/**
    Function to print nums.

    @param numeric NUMERIC_MIN_MAX_BASE Num

    @retval VOID

**/
VOID NumSPrint(NUMERIC_DATA *numeric,NUMERIC_MIN_MAX_BASE Num)
{
	switch(numeric->Base)
	{
		case AMI_BASE_HEX:
			SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%lx", Num );
			break;
        case AMI_BASE_INT_DEC:
			if(numeric->ControlData.ControlDataWidth == 1)
			{
				if((INT8)Num < 0)
				{
					SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%d", (INT8)Num );
					break;
				}
			}
			else if(numeric->ControlData.ControlDataWidth == 2)
			{
				if((INT16)Num < 0)
				{
					SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%d", (INT16)Num );	
					break;
				}
			}
			else if(numeric->ControlData.ControlDataWidth == 4)
			{
				if((INT32)Num < 0)
				{
					SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%d", (INT32)Num );	
					break;
				}
			}
			else if(numeric->ControlData.ControlDataWidth == 8)
			{
				if((INT64)Num < 0)
				{
					SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%ld", (INT64)Num );	
					break;
				}
			}
			SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%d", Num );
            break;
		case AMI_BASE_DEC:
		default:
			SPrint( numeric->Text, numeric->TextWidth*sizeof(CHAR16), L"%ld", Num );
			break;
	}
	UpdateNumericDisplayString (&(numeric->Text), Num, numeric->Base, numeric->ControlData.ControlDataWidth, numeric->TextWidth*sizeof(CHAR16)); //Provision to change the Numeric string format
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
