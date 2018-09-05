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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ordlistbox.c $
//
// $Author: Premkumara $
//
// $Revision: 17 $
//
// $Date: 5/28/12 12:35p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file ordlistbox.c
    This file contains code to handle ordered list box controls

**/

#include "minisetup.h"

VOID _PopupSelGetSelection( POPUPSEL_DATA *popupSel );
EFI_STATUS _OrdListMultiLIneDraw(ORD_LISTBOX_DATA *OrdList);
UINT16 gOrderlistcount;
extern BOOLEAN lButtonOnListBoxScroll;

ORD_LISTBOX_METHODS gOrdListBox =
{
		(OBJECT_METHOD_CREATE)OrdListBoxCreate,
		(OBJECT_METHOD_DESTROY)OrdListBoxDestroy,
		(OBJECT_METHOD_INITIALIZE)OrdListBoxInitialize,
		(OBJECT_METHOD_DRAW)OrdListBoxDraw,
		(OBJECT_METHOD_HANDLE_ACTION)OrdListBoxHandleAction,
		(OBJECT_METHOD_SET_CALLBACK)OrdListBoxSetCallback,
		(CONTROL_METHOD_SET_FOCUS)OrdListBoxSetFocus,
		(CONTROL_METHOD_SET_POSITION)OrdListBoxSetPosition,
		(CONTROL_METHOD_SET_DIMENSIONS)OrdListBoxSetDimensions,
		(CONTROL_METHOD_SET_ATTRIBUTES)OrdListBoxSetAttributes,
		(CONTROL_METHOD_GET_CONTROL_HIGHT)OrdListBoxGetControlHeight	
	};
VOID UefiGetValidOptionType(CONTROL_INFO *CtrlInfo, UINTN *Type, UINT32 *SizeOfData);
BOOLEAN FirstDownEvent = FALSE;		
/**
    function to Create a Order List Box, which uses the PopupSel Fucntions

    @param object 

    @retval status

**/
EFI_STATUS OrdListBoxCreate( ORD_LISTBOX_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(ORD_LISTBOX_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gPopupSel.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gOrdListBox;

	return Status;
}

/**
    function to Destroy a Order List Box, which uses the PopupSel Fucntions

    @param listbox BOOLEAN freeMem

    @retval status

**/
EFI_STATUS OrdListBoxDestroy( ORD_LISTBOX_DATA *listbox, BOOLEAN freeMem )
{
	if(NULL == listbox)
	  return EFI_SUCCESS;
	
	gPopupSel.Destroy( listbox, FALSE );

	if( freeMem )
	{
		MemFreePointer( (VOID **)&(listbox->PtrTokens) );
		MemFreePointer( (VOID **)&listbox );
	}
	return EFI_SUCCESS;
}

/**
    function to initialize a Order List Box, which uses the PopupSel Fucntions

    @param listbox VOID *data

    @retval status

**/
EFI_STATUS OrdListBoxInitialize( ORD_LISTBOX_DATA *listbox, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	Status = gPopupSel.Initialize( listbox, data );
//	_OrdListGetSelection(listbox); // make sure data is correct.
	listbox->ControlData.ControlHelp = UefiGetHelpField(listbox->ControlData.ControlPtr);// Updated the Help field for order list control.

	return Status;
}

/**
    function to draw a ordered List Box

    @param listbox 

    @retval status

**/
EFI_STATUS OrdListBoxDraw( ORD_LISTBOX_DATA *listbox )
{
	EFI_STATUS Status = EFI_SUCCESS;

	Status = _OrdListMultiLIneDraw(listbox); 

	return Status;
}

/**
    Function to update the orderlist count.

    @param ItemCount 

    @retval VOID
**/
VOID _UpdateOrderlistcount(UINT16 ItemCount)
{
	gOrderlistcount = ItemCount;
}
/**
    Call back function of ordered List Box

    @param listbox 

    @retval status

**/
VOID DoOrdListCallBack(ORD_LISTBOX_DATA *listbox)
{
	UINT8 			i=0;
	VOID 				*Data = (VOID *)NULL; 

	if ( listbox->Cookie != NULL )
	{
		VOID *ifrData = listbox->ControlData.ControlPtr;
		CALLBACK_VARIABLE *callbackData = (CALLBACK_VARIABLE *)listbox->Cookie;

		callbackData->Variable = listbox->ControlData.ControlVariable;
		callbackData->Offset = UefiGetQuestionOffset(ifrData);
		callbackData->Length = UefiGetMaxEntries(ifrData);

		if( (listbox->ControlData.ControlVariable == VARIABLE_ID_BOOT_ORDER) || 
			(listbox->ControlData.ControlVariable == VARIABLE_ID_BBS_ORDER) )
		{
			// Special Handling for Boot Order variable change
	        Data = (UINT8*)EfiLibAllocateZeroPool( listbox->ItemCount * sizeof(UINT16) );
			for(i=0;i<listbox->ItemCount;i++)
				((UINT16*)Data)[i] = (UINT16)(listbox->PtrTokens[i].Value);
			callbackData->Length = listbox->ItemCount * sizeof(UINT16);
		}
		else
		{
			UINT32 sizeOfData = 0;
			UINTN type = 0;
			UINT8 *tempData = (UINT8 *)NULL;

			//To find the Type and Size of OrderList based on EFI_IFR_TYPE
			UefiGetValidOptionType(&listbox->ControlData, &type, &sizeOfData);

			Data = (UINT8*)EfiLibAllocateZeroPool( listbox->ItemCount * sizeOfData);
			tempData = Data;
			
			for(i=0;i<listbox->ItemCount;i++)
			{
				//Data[i] = (UINT8)(listbox->PtrTokens[i].Value);
				//Copy Data based on EFI_IFR_TYPE and Size
				MemCopy(tempData, &listbox->PtrTokens[i].Value, sizeOfData);
				tempData = (UINT8 *)tempData + sizeOfData;
			}
			//Fix the length of Data based on EFI_IFR_TYPE
			callbackData->Length = UefiGetMaxEntries(ifrData) * sizeOfData;
			if (callbackData->Length > (listbox->ItemCount * sizeOfData))			//Filling all the dynamic forming datas with 0's
			{
				Data = MemReallocateZeroPool (Data, (listbox->ItemCount * sizeOfData), callbackData->Length);
			}
		}
		
		callbackData->Data = Data; //must send actual array of UINT8 values 
	}
	_UpdateOrderlistcount(listbox->ItemCount);										//Initializing gOrderlistcount
	UefiPreControlUpdate(&(listbox->ControlData));
	listbox->Callback( listbox->Container, listbox, listbox->Cookie );

		//For interactive orderedlist control invoke the callback functions
	if(UefiIsInteractive(&listbox->ControlData)){
		//EFI_STATUS Status = EFI_SUCCESS; not used 
		CallFormCallBack(&listbox->ControlData, UefiGetControlKey(&listbox->ControlData), FALSE, AMI_CALLBACK_CONTROL_UPDATE);
	}
	//_OrdListGetSelection(listbox);

	if(Data!=NULL)
		MemFreePointer((VOID **)&Data);

}

/**
    Function to handle the ordered List Box actions

    @param listbox ACTION_DATA *Data

    @retval status

**/
EFI_STATUS OrdListBoxHandleAction( ORD_LISTBOX_DATA *listbox, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
    UINT8 i=0;
    CONTROL_ACTION Action = ControlActionUnknown;
    UINT8 u8ChkResult;


	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	{
		// List box is active don't refresh
		if ( listbox->ListBoxCtrl != NULL )
			return Status;

		if (listbox->Interval == 0)
			return Status;

		if(IsActiveControlPresent(gApp->PageList[gApp->CurrentPage]))
			return Status;
		
		if ( --(listbox->Interval) == 0 )
		{
			// initialize the interval
			listbox->Interval = (UINT8)(listbox->ControlData.ControlFlags.ControlRefresh);
			return UefiRefershQuestionValueNvRAM(&(listbox->ControlData));
		}
		else
			return Status;
	}

    u8ChkResult = CheckControlCondition( &listbox->ControlData );

    //Not to perform action when token TSE_SETUP_GRAYOUT_SELECTABLE is enable OR controlflag is READ_ONLY OR control has GRAYOUTIF condition
    if(	( IsGrayoutSelectable() && (COND_GRAYOUT == u8ChkResult) ) || 
	 		( listbox->ControlData.ControlFlags.ControlReadOnly )
	 )
    {
        return EFI_UNSUPPORTED;
    }
	
	
	if(!listbox->ControlFocus)
		return Status;

	if(Data->Input.Type == ACTION_TYPE_MOUSE) //Change Order in Orderlist using Mouse drag
	{
		 if((listbox->Top <= (UINT16)Data->Input.Data.MouseInfo.Top ) &&  
			((listbox->Top + listbox->Height) > (UINT16)Data->Input.Data.MouseInfo.Top )&&
			(listbox->Left <= Data->Input.Data.MouseInfo.Left) &&
			((listbox->Left + listbox->Width) > (UINT16)Data->Input.Data.MouseInfo.Left) )
			Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);
	}

	if(Data->Input.Type == ACTION_TYPE_KEY)
    	Action = MapControlKeysHook(Data->Input.Data.AmiKey);

	if ( listbox->ListBoxCtrl != NULL )
	{
		Status = gListBox.HandleAction(listbox->ListBoxCtrl,Data);
		// special case, we need ot get the Sel from the listbox and put it into ordlistbox
        listbox->Sel = listbox->ListBoxCtrl->Sel;
		if ( listbox->ListBoxEnd )
		{
			gListBox.Destroy(listbox->ListBoxCtrl,TRUE);
			listbox->ListBoxCtrl = NULL;
			listbox->ListBoxEnd = FALSE;
			listbox->ControlActive = FALSE;
			DoOrdListCallBack(listbox);
		}
	}
	else
	{
		if(ControlActionSelect == Action)
        {
			if ( gListBox.Create( (void**)&(listbox->ListBoxCtrl) ) == EFI_SUCCESS )
			{
				UINT16 TempLength=0;

				listbox->ControlActive = TRUE; 
				gListBox.Initialize( listbox->ListBoxCtrl, &(listbox->ControlData) );
				//The next for loop should not be necessary after the callback
				//updates NVRAM properly
				//if(TseLiteIsSpecialOptionList((CONTROL_DATA *)listbox) == TRUE)
				{
					for( i = 0 ; i < listbox->ItemCount; i++ )
					{
						listbox->ListBoxCtrl->PtrTokens[i] = listbox->PtrTokens[i].Option;
						TempLength = (UINT16)HiiMyGetStringLength( listbox->ListBoxCtrl->ListHandle, listbox->ListBoxCtrl->PtrTokens[i] );
						TempLength+=5; // to included barders
						if ( TempLength > listbox->ListBoxCtrl->Width )
							listbox->ListBoxCtrl->Width = TempLength;

					}
					if(listbox->ListBoxCtrl->Width > (UINT16)(gMaxCols-5))
						listbox->ListBoxCtrl->Width = (UINT16)(gMaxCols-5);
				}
				if ( listbox->Sel > (listbox->ItemCount-1) ) //If selected item is suppresed then reset the focus to first item
					listbox->ListBoxCtrl->Sel = listbox->Sel = 0;
				else
					listbox->ListBoxCtrl->Sel = listbox->Sel;
				gListBox.SetCallback(listbox->ListBoxCtrl, listbox,(OBJECT_CALLBACK)  _OrdListBoxCallback, NULL);
				gListBox.Draw( listbox->ListBoxCtrl );
			} 

			Status = EFI_SUCCESS; 
		} 
	}

	if(Data->Input.Type == ACTION_TYPE_MOUSE && !lButtonOnListBoxScroll)//Change Order in Orderlist using Mouse drag 
	{
		if ( listbox->ListBoxCtrl != NULL )
		{
			Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);
		// Check if the Mouse action is on top of list box.
 		    if(
				(Data->Input.Data.MouseInfo.Top >= (UINT32)(listbox->ListBoxCtrl->Top - 1)) &&
				(ControlActionSelect != Action) &&
				(Data->Input.Data.MouseInfo.Top <= (UINT32)(listbox->ListBoxCtrl->Top + listbox->ListBoxCtrl->Height)) &&
				(Data->Input.Data.MouseInfo.Left >= listbox->ListBoxCtrl->Left) &&
				(Data->Input.Data.MouseInfo.Left <= (UINT32)(listbox->ListBoxCtrl->Left + listbox->ListBoxCtrl->Width - 1))
				) 
			{	
				//Note MOUSE_LEFT_DOWN position
				if(Action==ControlMouseActionLeftDown)
				{
					if(FirstDownEvent == FALSE )
					{
						if(Data->Input.Data.MouseInfo.Top == (UINT32)(listbox->ListBoxCtrl->Top + listbox->Sel - listbox->ListBoxCtrl->FirstVisibleSel + 1))	
							FirstDownEvent = TRUE;
					}
					else
					{
						if(Data->Input.Data.MouseInfo.Top > (UINT32)(listbox->ListBoxCtrl->Top + listbox->Sel - listbox->ListBoxCtrl->FirstVisibleSel + 1))	
							Action = ControlActionDecreament;
					
						if(Data->Input.Data.MouseInfo.Top < (UINT32)(listbox->ListBoxCtrl->Top + listbox->Sel - listbox->ListBoxCtrl->FirstVisibleSel + 1))	
							Action = ControlActionIncreament;
					}				
				}
				else
				{
					FirstDownEvent = FALSE;
				}
			}
		}
	}
    switch ( Action )
	{
        case ControlActionDecreament:
			//  Don't allow +/- until it is selected.
			if ( listbox->ListBoxCtrl != NULL ) 
			{
	        	if(!EFI_ERROR(_OrdListBoxShift(listbox, FALSE)))
	        	{
	        		if ( (INTN)(listbox->Sel) < listbox->ItemCount - 1 )
	        			listbox->Sel++;
	        	}
	        	Status = EFI_SUCCESS;
			}
        break;
        
        case ControlActionIncreament:
			//  Don't allow +/- until it is selected.
			if ( listbox->ListBoxCtrl != NULL )
			{
	        	if(!EFI_ERROR(_OrdListBoxShift(listbox, TRUE)))
	        	{
	        		if ( listbox->Sel > 0 )
	        			listbox->Sel--;
	        	}
	        	Status = EFI_SUCCESS;
			}
        break;
        default: //Not handled in Switch - Error
        	break;
	}

	return Status;
}

/**
    Function to set callback for ordered List Box 

    @param listbox OBJECT_DATA *container,   OBJECT_CALLBACK callback, VOID *cookie

    @retval status

**/
EFI_STATUS OrdListBoxSetCallback( ORD_LISTBOX_DATA *listbox, OBJECT_DATA *container,   OBJECT_CALLBACK callback, VOID *cookie )
{
	return gPopupSel.SetCallback( listbox, container, callback, cookie );
}
/**
    Function to set focus for ordered List Box 

    @param listbox BOOLEAN focus

    @retval status

**/
EFI_STATUS OrdListBoxSetFocus(ORD_LISTBOX_DATA *listbox, BOOLEAN focus)
{
	return gPopupSel.SetFocus((CONTROL_DATA*)listbox,focus);
}
/**
    Function to set position for ordered List Box 

    @param listbox UINT16 Left, UINT16 Top

    @retval status

**/
EFI_STATUS OrdListBoxSetPosition(ORD_LISTBOX_DATA *listbox, UINT16 Left, UINT16 Top )
{
	return gPopupSel.SetPosition( (CONTROL_DATA*)listbox, Left, Top );
}
/**
    Function to set dimension for ordered List Box 

    @param listbox UINT16 Width, UINT16 Height

    @retval status

**/
EFI_STATUS OrdListBoxSetDimensions(ORD_LISTBOX_DATA *listbox, UINT16 Width, UINT16 Height )
{
	return gPopupSel.SetDimensions((CONTROL_DATA*) listbox, Width, Height );
}
/**
    Function to set atributes for ordered List Box 

    @param listbox UINT8 FGColor, UINT8 BGColor

    @retval status

**/
EFI_STATUS OrdListBoxSetAttributes(ORD_LISTBOX_DATA *listbox, UINT8 FGColor, UINT8 BGColor )
{
	return gPopupSel.SetAttributes((CONTROL_DATA*) listbox, FGColor, BGColor );
}
/**
    Ordered List Box callback function

    @param container CONTROL_DATA *listbox, VOID *cookie

    @retval VOID

**/
VOID _OrdListBoxCallback( ORD_LISTBOX_DATA *container, CONTROL_DATA *listbox, VOID *cookie )
{
	container->ListBoxEnd = TRUE;
	if ( cookie != NULL )
		container->Sel = *((UINT16*)cookie);
}

/*
	Please note that the following function is non-rotatary or non-circular
	This is an internal function to ordered list control,so we need to:
	   1-add underscore to the name 
	   2-do not put it in control_methods
	This function is not used by any one else outside the control.
*/

/**
    Function used in OrdListBoxHandleAction to handle actions

    @param listbox UINT8 bShiftUp

    @retval status

**/
EFI_STATUS _OrdListBoxShift(ORD_LISTBOX_DATA *listbox, UINT8 bShiftUp)
{	
	PTRTOKENS  unSwap;
	UINT8 i=0;

	if(TRUE == bShiftUp)
	{
		//move up
		if ( listbox->Sel > 0 )
		{
			unSwap = listbox->PtrTokens[listbox->Sel];
			listbox->PtrTokens[listbox->Sel] = listbox->PtrTokens[listbox->Sel-1];
			listbox->PtrTokens[listbox->Sel-1] = unSwap;
			if( listbox->ListBoxCtrl !=NULL)
			{
				// update string tokens into listbox area
				for( i = 0 ; i < listbox->ItemCount; i++ )
					listbox->ListBoxCtrl->PtrTokens[i] = listbox->PtrTokens[i].Option;
				listbox->ListBoxCtrl->Sel--;
				gPopupSel.Draw(listbox);
		    }
			return EFI_SUCCESS;
		}
	}
	else
	{
		//move dn
		if ( (INTN)(listbox->Sel) < listbox->ItemCount - 1 )
		{
			unSwap = listbox->PtrTokens[listbox->Sel];
			listbox->PtrTokens[listbox->Sel] = listbox->PtrTokens[listbox->Sel+1];
			listbox->PtrTokens[listbox->Sel+1] = unSwap;
			if( listbox->ListBoxCtrl !=NULL)
			{	// update string tokens into listbox area
				for( i = 0 ; i < listbox->ItemCount; i++ )
					listbox->ListBoxCtrl->PtrTokens[i] = listbox->PtrTokens[i].Option;
				listbox->ListBoxCtrl->Sel++;
				gPopupSel.Draw(listbox);
			} 

			return EFI_SUCCESS;
		}
	}

	return EFI_NOT_FOUND;
}

/**
    Function used get the hight of control

    @param OrdList VOID* frame, UINT16 *height

    @retval status

**/
EFI_STATUS OrdListBoxGetControlHeight( ORD_LISTBOX_DATA *OrdList, VOID* frame, UINT16 *height )
{
	if(IsTSEMultilineControlSupported())
	{
		CHAR16 *newText = NULL,*text=NULL;
		UINT16 Width;
	
		Width = (UINT16)(OrdList->LabelMargin - (((FRAME_DATA*)frame)->FrameData.Left + (UINT8)gLabelLeftMargin));
	
		text = HiiGetString( OrdList->ControlData.ControlHandle, OrdList->Title );
		if ( text == NULL )
			return EFI_OUT_OF_RESOURCES;
	
		newText = StringWrapText( text, Width, height );
	
	    (*height) = (*height) ? (*height):1;
	
	    MemFreePointer( (VOID **)&newText );
		MemFreePointer( (VOID **)&text );

    	*height = (*height < OrdList->ItemCount)? OrdList->ItemCount : *height ;
	}
	else
	{
		*height = 1;
	}
	return EFI_SUCCESS;
}

/**
    Function used to draw teh Ordered list box

    @param OrdList 

    @retval status

**/
EFI_STATUS _OrdListMultiLIneDraw(ORD_LISTBOX_DATA *OrdList)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ColorNSel = OrdList->NSelFGColor;
	UINT8 ColorLabel = (OrdList->ControlFocus) ? OrdList->LabelFGColor : OrdList->NSelLabelFGColor ;
  	CHAR16 *text1, *text;
	UINTN i=0,Len, oldSize = 0;
	BOOLEAN MultilineSupport = FALSE;
	UINT16 *OptionList=NULL, Index = 0;
	UINT64 *ValueList=NULL;

    //Draw the list box alone if the control is active
	if(  OrdList->ListBoxCtrl != NULL)
	{
		gListBox.Draw( OrdList->ListBoxCtrl );
	}
	else
	{
		// check conditional
		//if( OrdList->ControlData.ControlConditionalPtr != 0x0)
		//{
			switch( CheckControlCondition( &OrdList->ControlData ) )
			{
				case COND_NONE:
					break;
				case COND_GRAYOUT:
					Status = EFI_WARN_WRITE_FAILURE;
					ColorNSel =  ColorLabel = CONTROL_GRAYOUT_COLOR;
					break;
				default:
					return EFI_UNSUPPORTED;
					break;
			}
		//}
			if(TseLiteIsSpecialOptionList((CONTROL_DATA *)OrdList) != TRUE) //Fix options and option count in listbox if any item is suppressed
			{
				oldSize = OrdList->ItemCount;

				Status = UefiGetOneOfOptions(&OrdList->ControlData,&OrdList->PopupSelHandle,
					&OptionList, &ValueList, &OrdList->ItemCount,NULL,NULL);

				if (OrdList->ItemCount!=oldSize)
				{
					OrdList->PtrTokens = MemReallocateZeroPool(OrdList->PtrTokens, oldSize*sizeof(PTRTOKENS), OrdList->ItemCount * sizeof(PTRTOKENS));
					if (NULL == OrdList->PtrTokens)
						return EFI_OUT_OF_RESOURCES;
				}

				if (EFI_ERROR(Status))
					return Status;
				for( Index = 0; Index < OrdList->ItemCount; Index++ )
				{
					OrdList->PtrTokens[Index].Option = OptionList[Index];
					OrdList->PtrTokens[Index].Value = ValueList[Index];
				}
				MemFreePointer( (VOID **)&(OptionList));
				MemFreePointer( (VOID **)&(ValueList));
			}
        _OrdListGetSelection(OrdList); //To handle OrderList Items
		//_PopupSelGetSelection((POPUPSEL_DATA *)OrdList);

        //Draw label in left column
		text1 = HiiGetString( OrdList->ControlData.ControlHandle, OrdList->Title );
		if ( text1 != NULL )
		{
			//TSE_MULTILINE_CONTROLS moved to binary
			if(OrdList->Height>1 && IsTSEMultilineControlSupported())
			{
				DrawMultiLineStringWithAttribute( OrdList->Left, OrdList->Top, 
						(UINTN)(OrdList->LabelMargin - OrdList->Left),(UINTN) OrdList->Height,
						text1, OrdList->BGColor |  ColorLabel );
			}
			else
			{
	            // boundary overflow  check
				if((TestPrintLength( text1) / (NG_SIZE))> (UINTN)(OrdList->LabelMargin - OrdList->Left  ))
					text1[HiiFindStrPrintBoundary(text1 ,(UINTN)(OrdList->LabelMargin - OrdList->Left  ))] = L'\0';
	
				DrawStringWithAttribute( OrdList->Left , OrdList->Top, (CHAR16*)text1, 
						OrdList->BGColor |  ColorLabel  );
			}
			MemFreePointer( (VOID **)&text1 );
		}
		if(IsTSEMultilineControlSupported()) //TSE_MULTILINE_CONTROLS moved to binary
			MultilineSupport = TRUE;

        do
		{
            text1 = HiiGetString( OrdList->PopupSelHandle, OrdList->PtrTokens[i].Option);

			if(!text1) //To handle NULL items in orderlistbox
				text1 = EfiLibAllocateZeroPool(2);

            Len = TestPrintLength( text1 ) / (NG_SIZE);
            if ( Len > (UINTN)(OrdList->Width - OrdList->Left - OrdList->LabelMargin -2) )
			    EfiStrCpy(&text1[HiiFindStrPrintBoundary(text1 ,(UINTN)(OrdList->Width - OrdList->Left - OrdList->LabelMargin -4))],L"..." );

            //Enclose the string with [ ]
            text = EfiLibAllocateZeroPool( (Len+3)*2 );
            SPrint( text,(UINTN)( (Len+3)*2 ), L"[%s]", text1 );
            MemFreePointer( (VOID **)&text1 );

            DrawStringWithAttribute(
                        OrdList->Left + OrdList->LabelMargin,
                        OrdList->Top+i,
                        (CHAR16*)text,
                        (UINT8)(OrdList->ControlFocus ? OrdList->SelBGColor|OrdList->SelFGColor : OrdList->BGColor|ColorNSel)
                        );
			i++;
			MemFreePointer( (VOID **)&text );
        }while(i<OrdList->ItemCount && MultilineSupport);

  		/*
        i=TestPrintLength(text) / (NG_SIZE);
        MemFreePointer( (VOID **)&text );

        // erase extra spaces if neccessary
		for(;(UINT16)(popupSel->Left + popupSel->LabelMargin + i+2) <= (popupSel->Width-1);i++)
			DrawStringWithAttribute( popupSel->Left + popupSel->LabelMargin +i +2 , popupSel->Top, L" ", 
						 popupSel->BGColor  |  ColorNSel );
        */

		FlushLines( OrdList->Top, OrdList->Top+i-1 );
	}

	return Status;
}

/**
    Function to ordering the option in OrderList

    @param ordlist 

    @retval 
**/
VOID _OrdListGetSelection( ORD_LISTBOX_DATA *ordlist )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT8 *temp=NULL;
	VOID *ifrData = ordlist->ControlData.ControlPtr;
	int i=0,j=0;
    PTRTOKENS		*OrdPtrTokens=NULL, TempOrdPtrTokens;



	UINT32 sizeOfData = 0;
	UINTN type = 0;
	UINTN dataLen = 0;
	//Find the size of OrderList based on EFI_IFR_TYPE of EFI_IFR_ONE_OF_OPTION
//	UefiGetValidOptionSize(&ordlist->ControlData, &sizeOfData);
//	dataLen = ordlist->ItemCount * sizeOfData;
    
	UefiGetValidOptionType(&ordlist->ControlData, &type, &sizeOfData);
	dataLen = UefiGetMaxEntries(ifrData) * sizeOfData;

	temp = EfiLibAllocateZeroPool( dataLen  );
	if ( temp == NULL )
		return;

    OrdPtrTokens = ordlist->PtrTokens;
    
    Status = VarGetValue( ordlist->ControlData.ControlVariable, UefiGetQuestionOffset(ifrData), dataLen, temp );

    // Check variable data and adjust item order.
	if ( Status == EFI_SUCCESS )
	{
        for (i=0 ; i<ordlist->ItemCount-1 ; i++)
        {
            for (j=i+1 ; j<ordlist->ItemCount ; j++)
            {
                if ( MemCmp( (VOID *)&(OrdPtrTokens[j].Value),(VOID *)(temp + (i * sizeOfData)), sizeOfData ) == 0)
                {
                    TempOrdPtrTokens = OrdPtrTokens[i];
                    OrdPtrTokens[i] = OrdPtrTokens[j];
                    OrdPtrTokens[j] = TempOrdPtrTokens;
                    break;
                }
            }
        }
	}
	MemFreePointer( (VOID **)&temp );

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
