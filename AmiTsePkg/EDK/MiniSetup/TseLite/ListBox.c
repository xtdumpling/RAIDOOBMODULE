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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ListBox.c $
//
// $Author: Arunsb $
//
// $Revision: 24 $
//
// $Date: 9/21/12 8:40a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file ListBox.c
    This file contains code to handle Listbox controls

**/

#include "minisetup.h"

static INT32 lButtonVaryingPosition = 0;
static INT32 lButtonInitialPosition = 0;
BOOLEAN lButtonOnListBoxScroll = FALSE;
BOOLEAN ScrollBarMoveAction = FALSE;
static UINT32 gListBoxScrollBarTop = 0, gListBoxScrollBarBottom = 0;
LISTBOX_METHODS gListBox =
{
	(OBJECT_METHOD_CREATE)ListBoxCreate,
	(OBJECT_METHOD_DESTROY)ListBoxDestroy,
	(OBJECT_METHOD_INITIALIZE)ListBoxInitialize,
	(OBJECT_METHOD_DRAW)ListBoxDraw,
	(OBJECT_METHOD_HANDLE_ACTION)ListBoxHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)ListBoxSetCallback,
	(CONTROL_METHOD_SET_FOCUS)ListBoxSetFocus,
	(CONTROL_METHOD_SET_POSITION)ListBoxSetPosition,
	(CONTROL_METHOD_SET_DIMENSIONS)ListBoxSetDimensions,
	(CONTROL_METHOD_SET_ATTRIBUTES)ListBoxSetAttributes,
	(CONTROL_METHOD_GET_CONTROL_HIGHT)ListBoxGetControlHeight,
	ListBoxInitializeBBSPopup
};
BOOLEAN SingleClickActivation (VOID); 
EFI_STATUS TSEMouseListBoxScrollBarMove(  LISTBOX_DATA *listbox, BOOLEAN bScrollUp, UINT16 Size );
extern BOOLEAN IsGrayoutOneOfOptionsSupport(void);

/**
    Gets the number of focusable items.

    @param listbox 

    @retval UINT16 

**/
UINT16 GetFocusItemCount( LISTBOX_DATA *listbox )
{
    UINT16 i, Count = 0 ;
    if(listbox->Style == POPUP_STYLE_BBS)   
    {
        for ( i = 0; i < listbox->ItemCount; i++ )
        {
            if((listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_NON_FOCUS)&&
            (listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_EXIT_KEY)&&
                (listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_HIDDEN)) 
            {
                Count++;
            }
        }
    }

    return Count ;
}
/**
    Function to create a List Box, which uses the popup controls.

    @param object 

    @retval status

**/
EFI_STATUS ListBoxCreate( LISTBOX_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(LISTBOX_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gPopup.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gListBox;

	return Status;
}

/**
    Function to Destroy a List Box, which uses the popup controls.

    @param listbox BOOLEAN freeMem

    @retval status

**/
EFI_STATUS ListBoxDestroy( LISTBOX_DATA *listbox, BOOLEAN freeMem )
{
    if(listbox == NULL)
        return EFI_SUCCESS;

	gPopup.Destroy( listbox, FALSE );

	RestoreScreen( listbox->ScreenBuf );

	if( freeMem )
	{
		MemFreePointer((VOID **)&listbox->Title);
		MemFreePointer((VOID **)&listbox->PtrTokens);
		MemFreePointer((VOID **)&listbox->PtrItems);
		MemFreePointer((VOID **)&listbox->Help1);
		MemFreePointer((VOID **)&listbox->Help2);
		MemFreePointer((VOID **)&listbox->Help3);
		MemFreePointer( (VOID **)&listbox );
	}

	return EFI_SUCCESS;
}

/**
    Function to set a string in List Box.

    @param listbox CHAR16 **pStr,UINT16 Toke

    @retval void

**/
VOID SetListBoxString(LISTBOX_DATA *listbox, CHAR16 **pStr,UINT16 Token)
{
	CHAR16 *TempStr=NULL;
	UINT16 TitleLength;

	TempStr = HiiGetString( listbox->ControlData.ControlHandle, Token );

	if ( TempStr != NULL )
	{
		TitleLength = (UINT16)((TestPrintLength( TempStr ) / (NG_SIZE)) + 3);

		// Truncate the list box title if it exceeds the limit
		if( TitleLength > gMaxCols-9 ){
			TempStr[gMaxCols-9] = L'\0';
			TitleLength = (UINT16)((TestPrintLength( TempStr ) / (NG_SIZE)) + 3);
		}

		if( listbox->Width < TitleLength + 4 )
			listbox->Width = TitleLength + 4;
	}
	*pStr = TempStr;
}
/**
    function to get line count and max width.

    @param line UINT8 *Width 
					

    @retval UINTN

**/
UINTN GetLineCount(CHAR16 *line, UINT16 *Width )
{
	CHAR16 * text;
	UINTN i=1;
	CHAR16 * String=line;
	UINT16 length;

	while(1)
	{
		CHAR16 save;

		text = String;
		if ( *String == L'\0' )
			break;

		while ( ( *String != L'\n' ) &&( *String != L'\r' ) && ( *String != L'\0' ) )
			String++;

		save = *String;
		*String = L'\0';

		length = 0 ;
		length = (UINT16)((TestPrintLength(text) / (NG_SIZE))) ;
		if(length > *Width)
			*Width = length ;

		if ( ( *String = save ) != L'\0' )
		{
			if ( *String == L'\r' )
			{	String++;
				i--;
			}
			if ( *String == L'\n' )
			{
				String++;

				if ( *(String - sizeof(CHAR16)) == L'\r' )
					i++;
			}
		}
		else
			break;
		i++;
	}
	return i;
}
/**
    Function to set a string in List Box.

    @param listbox CHAR16 **pStr,UINT16 Toke

    @retval void

**/
VOID SetListBoxHelpString(LISTBOX_DATA *listbox, CHAR16 **pStr,UINT16 Token)
{
	CHAR16 *TempStr=NULL;
	UINT16 Width=0 ;

	TempStr = HiiGetString( listbox->ControlData.ControlHandle, Token );

	if ( TempStr != NULL )
	{
		GetLineCount(TempStr, &Width);	
				
		if( listbox->Width < Width + 4 )
			listbox->Width = Width + 4;
	}
	*pStr = TempStr;
}
/**
    Function to set the common fields in a List Box.

    @param listbox 

    @retval void

**/
VOID SetListBoxCommonFields(LISTBOX_DATA *listbox)
{
	
	listbox->Width += 4;
	listbox->Sel = 0;

	// check if we need to use do srollbar
	if(listbox->Height > MAX_VISIBLE_SELECTIONS )
	{
		listbox->Height = (UINT16)MAX_VISIBLE_SELECTIONS;
      		listbox->Width++;   // ad space for the scrollbar.
		listbox->FirstVisibleSel = 0;
		listbox->LastVisibleSel = listbox->Height-3; // 2 for borders and 1 more because is zero based.
         
	}
	else
	{
		listbox->FirstVisibleSel = 0;
		listbox->LastVisibleSel =  listbox->Height-3;
	}
	SetControlColorsHook(NULL, NULL, NULL, NULL ,NULL, NULL,
		              NULL , NULL,NULL, NULL,	NULL,NULL ,NULL,
			 	&(listbox->FGColor),&(listbox->BGColor));

	listbox->Border =TRUE;
	listbox->Shadow = TRUE;
	if(listbox->ScreenBuf == NULL)
	listbox->ScreenBuf = SaveScreen();
}
/**
    Function to initialize a List Box, which uses the Popup controls.

    @param listbox VOID *data

    @retval status

**/
EFI_STATUS ListBoxInitialize( LISTBOX_DATA *listbox, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_STATUS CondStatus = EFI_UNSUPPORTED;
	UINT16 TempLength=0;
	UINT16 TitleToken;
	UINT16 Index= 0 ;

	Status = gPopup.Initialize( listbox, data );
	if (EFI_ERROR(Status))
		return Status;
	// add extra initialization here...
	listbox->ControlData.ControlHelp = UefiGetHelpField(listbox->ControlData.ControlPtr);

	listbox->ListHandle = listbox->ControlData.ControlHandle;
	listbox->HiddenItemCount = 0;
	Status = UefiGetOneOfOptions(&listbox->ControlData,&listbox->ListHandle,&listbox->PtrTokens, NULL, &listbox->ItemCount,NULL,NULL);

	// check if there are grayed out options
	listbox->PtrItems = EfiLibAllocatePool( listbox->ItemCount * sizeof(POSTMENU_TEMPLATE) ); 
	if ( listbox->PtrItems == NULL )
		Status = EFI_OUT_OF_RESOURCES;
	else
		CondStatus = UefiGetValidOptionsCondition(&listbox->ControlData, listbox->PtrItems, listbox->ItemCount);
	// grayed out options are not found -> PtrItems are not needed
	if ( EFI_ERROR(CondStatus))
		MemFreePointer( (VOID **)&listbox->PtrItems );
	
	if ( ! EFI_ERROR(Status) )
	{

		for ( Index = 0; Index < listbox->ItemCount; Index++ )
		{
			TempLength = (UINT16)HiiMyGetStringLength( listbox->ListHandle, listbox->PtrTokens[Index] );
	
			if ( TempLength > listbox->Width )
				listbox->Width = TempLength;
		}
		if (listbox->Width > (gMaxCols-6))
		    listbox->Width = (UINT16)(gMaxCols-6);

		listbox->ItemCount = Index;
		listbox->Height = Index + 2;

		//pad title 
		TitleToken = UefiGetPromptField(listbox->ControlData.ControlPtr);
		SetListBoxString(listbox,&listbox->Title ,TitleToken);
		SetListBoxCommonFields(listbox);
	}

	return Status;
}

/**
    Function to initialize the BBS Popup in a List Box.

    @param listbox LISTBOX_BSS_POPUP_DATA *listboxBBSPopupData

    @retval status

**/
EFI_STATUS ListBoxInitializeBBSPopup( LISTBOX_DATA *listbox, LISTBOX_BSS_POPUP_DATA *listboxBBSPopupData )
{
	UINT16 Index= 0 ;
	UINT16 TempLength=0;
	EFI_STATUS Status = EFI_SUCCESS;

	listbox->Style = POPUP_STYLE_BBS;
	
	if(listboxBBSPopupData->pControlData != NULL )
		MemCopy( &listbox->ControlData, listboxBBSPopupData->pControlData, sizeof(CONTROL_INFO) );

	listbox->ListHandle = listbox->ControlData.ControlHandle;
	listbox->ItemCount = listboxBBSPopupData->ItemCount;
	listbox->HiddenItemCount = listboxBBSPopupData->HiddenItemCount;
	listbox->Height = listbox->ItemCount - listbox->HiddenItemCount + 2;

	listbox->PtrItems = EfiLibAllocatePool( listbox->ItemCount * sizeof(POSTMENU_TEMPLATE) ); 
	if ( listbox->PtrItems == NULL )
		Status = EFI_OUT_OF_RESOURCES;
	else
	{
		for ( Index = 0; Index < listbox->ItemCount; Index++ )
		{
			listbox->PtrItems[Index] = listboxBBSPopupData->PtrItems[Index] ;

			if((listbox->PtrItems[Index].Attribute != AMI_POSTMENU_ATTRIB_HIDDEN)&&
				(listbox->PtrItems[Index].Attribute != AMI_POSTMENU_ATTRIB_EXIT_KEY))
			{
				TempLength = (UINT16)HiiMyGetStringLength( listbox->ListHandle, listbox->PtrItems[Index].ItemToken );
	
				if ( TempLength > listbox->Width )
					listbox->Width = TempLength;
			}
		}
		if (listbox->Width > (gMaxCols-6))
		    listbox->Width = (UINT16)(gMaxCols-6);
		    
		if(listboxBBSPopupData->TitleToken)
			SetListBoxString(listbox,&listbox->Title ,listboxBBSPopupData->TitleToken);
		if(listboxBBSPopupData->Help1Token)
			SetListBoxHelpString(listbox,&listbox->Help1 ,listboxBBSPopupData->Help1Token);
		//SetListBoxString(listbox,&listbox->Help2 ,listboxBBSPopupData->Help2Token);
		//SetListBoxString(listbox,&listbox->Help3 ,listboxBBSPopupData->Help3Token);
		SetListBoxCommonFields(listbox);
	}

	return Status;
}
/**
        Procedure	:	ListBoxDrawSeperator

        Description	:	function to draw the border

        Input		:	UINTN Left, UINTN Top, UINTN Width, UINTN Height

        Output		:	void

**/
VOID ListBoxDrawSeperator( LISTBOX_DATA *listbox, UINTN Left, UINTN Top, UINTN Width, UINTN Height )
{
	UINTN  Index;
	CHAR16 *Line;

//	UINTN Right, Bottom; //set but not used

	Line = EfiLibAllocateZeroPool( (Width + 6) * sizeof(CHAR16) );

	if ( Line == NULL )
		return;

//	Right = Left + Width - 1; Set But not used
//	Bottom = Top + Height - 1;

	for ( Index = 1; Index < Width - 1; Index++ )
		Line[Index] = BOXDRAW_HORIZONTAL;

	if(listbox->Border)
	{
		Line[0] = BOXDRAW_VERTICAL_RIGHT;

		if(!listbox->UseScrollbar)
			Line[Index] = BOXDRAW_VERTICAL_LEFT;

		DrawString( Left, Top, Line );
	}
	
	

	MemFreePointer( (VOID **)&Line );
}
/**
    Function to draw a List Box.

    @param listbox 

    @retval status

**/
EFI_STATUS ListBoxDraw( LISTBOX_DATA *listbox )
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN Index;
	CHAR16 *text = NULL;
	UINT32  FirstLine = 0, LastLine = 0;
	CHAR16	ArrUp[2]={GEOMETRICSHAPE_UP_TRIANGLE,0x0000},ArrDown[2]={GEOMETRICSHAPE_DOWN_TRIANGLE,0x0000},ScrlSel[2]={BLOCKELEMENT_FULL_BLOCK/*L'N'*/,0x0000},ScrlNSel[2]={BLOCKELEMENT_LIGHT_SHADE/*L'S'*/,0x0000};
    UINT16 BorderWidth = 4;
	UINT8 FGColor = listbox->FGColor;
	UINT8 BGColor = listbox->BGColor;
	

	Status =  gPopup.Draw( listbox );
	if (Status != EFI_SUCCESS)
		return Status;

	listbox->UseScrollbar = FALSE;

	
	// check if we need to use do scrollbar
	if((listbox->ItemCount-listbox->HiddenItemCount) > (UINT16)(MAX_VISIBLE_SELECTIONS - 2))
	{
  		listbox->UseScrollbar = TRUE ;      	
		BorderWidth = 5;
		if (!ScrollBarMoveAction)
		{
			if( listbox->Sel < listbox->FirstVisibleSel)
			{
				if((listbox->Sel != 0) && (listbox->Sel < listbox->LastVisibleSel -1))
				{
					listbox->FirstVisibleSel = listbox->Sel;
					listbox->LastVisibleSel = listbox->Sel + listbox->Height -3 ;
				}
				else
				{
					if( listbox->Sel !=0)
					{
						listbox->FirstVisibleSel = listbox->Sel /*--*/;
						listbox->LastVisibleSel = listbox->FirstVisibleSel + listbox->Height -3;
					}
					else
					{
						listbox->FirstVisibleSel = 0 ;
						listbox->LastVisibleSel = listbox->Height -3;
					}
				}
			}

			if(listbox->Sel > listbox->LastVisibleSel )
			{
				if( (( listbox->Sel +listbox->Height-3) <= (listbox->ItemCount-listbox->HiddenItemCount-1) ) && (listbox->Sel > listbox->LastVisibleSel +1))
				{
					listbox->FirstVisibleSel = listbox->Sel;
					listbox->LastVisibleSel = listbox->Sel + listbox->Height -3 ;
				}
				else
				{
					if( (listbox->Sel ) < (listbox->ItemCount-listbox->HiddenItemCount-1))
					{
						listbox->FirstVisibleSel = listbox->Sel - listbox->Height +3 /*--*/;
						listbox->LastVisibleSel = listbox->Sel;
					}
					else
					{
						listbox->FirstVisibleSel = listbox->ItemCount-listbox->HiddenItemCount-1  - listbox->Height +3 ;
						listbox->LastVisibleSel = listbox->ItemCount-listbox->HiddenItemCount-1;
					}
				}
			}
		}

		// draw the scrollbar
       		DrawStringWithAttribute( listbox->Left + listbox->Width -2 , listbox->Top+1, ArrUp, 
		                            listbox->FGColor | listbox->BGColor );
		
		DrawStringWithAttribute( listbox->Left+ listbox->Width -2, listbox->Top + listbox->Height-2, ArrDown, 
					                             listbox->FGColor | listbox->BGColor );

		FirstLine = ((listbox->Height-2)* listbox->FirstVisibleSel/(listbox->ItemCount-listbox->HiddenItemCount) )+ listbox->Top+2;
	    LastLine  = ((listbox->Height-2)* listbox->LastVisibleSel/(listbox->ItemCount-listbox->HiddenItemCount) )+ listbox->Top+2;
	
		gListBoxScrollBarTop = FirstLine; //To store Scrollbar Top positon 
		gListBoxScrollBarBottom = LastLine; //To store Scrollbar Bottom positon 

		for(Index = (listbox->Top + 2); Index < (UINTN)(listbox->Top + listbox->Height -2) ;Index++)
		{
                      if( (Index >= FirstLine) && (Index < LastLine))
						  DrawStringWithAttribute( listbox->Left+ listbox->Width-2, Index, ScrlSel, 
						                            listbox->FGColor | listbox->BGColor );
					  else
                        DrawStringWithAttribute( listbox->Left+ listbox->Width-2, Index, ScrlNSel, 
						                         listbox->FGColor | listbox->BGColor );
		}

	}


	// highlight selected item
	for ( Index = listbox->FirstVisibleSel/*0*/; Index <= listbox->LastVisibleSel/*listbox->ItemCount*/; Index++ )
	{
		FGColor = listbox->FGColor ;
		BGColor = listbox->BGColor;

		if ( Index == listbox->Sel )
		{
			FGColor = EFI_WHITE ;
			BGColor = EFI_BACKGROUND_BLACK ;

			// Check condition if item is non-focusable
			// This may occur if all menu items are set non-focusable
			if (listbox->PtrItems)
			{
				if( listbox->PtrItems[Index].Attribute == AMI_POSTMENU_ATTRIB_NON_FOCUS)
				{
					FGColor = EFI_LIGHTGRAY;
					BGColor = listbox->BGColor;
				}	
			}
			if(listbox->Style == POPUP_STYLE_BBS)
			{
				text = HiiGetString( listbox->ListHandle, listbox->PtrItems[Index].ItemToken );
			
			}
			else
				text =  HiiGetString( listbox->ListHandle, listbox->PtrTokens[Index]);

			if(!text)
				text = EfiLibAllocateZeroPool(2);
			if((TestPrintLength( text) / (NG_SIZE)) >(UINTN) (listbox->Width - BorderWidth) )
				text[HiiFindStrPrintBoundary(text ,listbox->Width - BorderWidth)] = L'\0';
			DrawStringWithAttribute( listbox->Left + 2, listbox->Top + Index - listbox->FirstVisibleSel  + 1, 
					text, BGColor | FGColor);
		}	
		else
		{
			if(listbox->Style == POPUP_STYLE_BBS)
				text = HiiGetString( listbox->ListHandle, listbox->PtrItems[Index].ItemToken );
			else
				text = HiiGetString( listbox->ListHandle, listbox->PtrTokens[Index] );
			
			
			if (listbox->PtrItems)
			{
				if( listbox->PtrItems[Index].Attribute == AMI_POSTMENU_ATTRIB_NON_FOCUS)
				{
					FGColor = EFI_LIGHTGRAY;
					BGColor = listbox->BGColor;
				}	
			}
			
			if(listbox->Style == POPUP_STYLE_BBS)
			{
				if (listbox->PtrItems[Index].ItemToken)
				{

					//if(!text)
					//	text = EfiLibAllocateZeroPool(2);
					if((TestPrintLength( text) / (NG_SIZE)) > (UINTN) (listbox->Width - BorderWidth) )
						text[HiiFindStrPrintBoundary(text ,listbox->Width - BorderWidth)] = L'\0';
		
					DrawStringWithAttribute( listbox->Left + 2, listbox->Top + Index - listbox->FirstVisibleSel + 1, 
						text, BGColor | FGColor /*popupmenu->FGColor*/ );
				}else
					ListBoxDrawSeperator( listbox, listbox->Left , listbox->Top + Index - listbox->FirstVisibleSel + 1, 
						listbox->UseScrollbar?(listbox->Width-2):listbox->Width, 1);

			}			
			else
			{

				if(!text)
					text = EfiLibAllocateZeroPool(2);
				if((TestPrintLength( text) / (NG_SIZE)) > (UINTN) (listbox->Width - BorderWidth) )
					text[HiiFindStrPrintBoundary(text ,listbox->Width - BorderWidth)] = L'\0';
				DrawStringWithAttribute( listbox->Left + 2, listbox->Top + Index - listbox->FirstVisibleSel + 1, 
					text, BGColor | FGColor );
			}
		}
		MemFreePointer( (VOID **)&text );
	}

	FlushLines( listbox->Top-2, listbox->Top+listbox->Height/*PtrTokens[0]*/ + 1+2);
	return EFI_SUCCESS;
}

/**
    Function to handle the List Box actions.

    @param listbox ACTION_DATA *Data

    @retval status

**/
EFI_STATUS ListBoxHandleAction( LISTBOX_DATA *listbox, ACTION_DATA *Data)
{
	EFI_STATUS Status=EFI_SUCCESS; 
	ACTION_DATA TempData;
	UINT16	 start, end ;
	INT16 i=0;
//	VOID	*ScreenBuf = NULL;				Unused var

	if(Data->Input.Type == ACTION_TYPE_TIMER)
		Status = EFI_UNSUPPORTED;

    if(Data->Input.Type == ACTION_TYPE_MOUSE)
    {
		Status = MouseListBoxHandleAction( listbox, Data);
    }

	if(Data->Input.Type == ACTION_TYPE_KEY)
	{
        CONTROL_ACTION Action;

		ScrollBarMoveAction = FALSE; 

        //Get mapping
        Action = MapControlKeysHook(Data->Input.Data.AmiKey);

        switch(Action)
        {
            // Add Page up and Page down key in List box
            case ControlActionPageUp:
                if(listbox->Style == POPUP_STYLE_NORMAL && !listbox->PtrItems)
                {
                    if (listbox->Sel < (listbox->Height-3))
                        listbox->Sel = 0;
                    else
                        listbox->Sel= listbox->Sel - (listbox->Height-3);
                }
                break;
            case ControlActionPageDown:
                if(listbox->Style == POPUP_STYLE_NORMAL && !listbox->PtrItems )
                {
                    if ((listbox->Sel+(listbox->Height-3)) > (listbox->ItemCount-1))
                        listbox->Sel = (listbox->ItemCount-1);
                    else
                        listbox->Sel= listbox->Sel + (listbox->Height-3);
                }
                break;
            case ControlActionNextUp:
    		if ( listbox->Sel > 0 )
			{
                if(listbox->Sel > (listbox->ItemCount-1))   // Safety check
                {
                    listbox->Sel = listbox->ItemCount ;     
                }
                                
				listbox->Sel--;
			
				if ((listbox->Style == POPUP_STYLE_BBS)	|| (listbox->Style == POPUP_STYLE_NORMAL && listbox->PtrItems))	
				{
				   					
				    if ( (listbox->PtrItems[listbox->Sel].ItemToken==0) || (listbox->PtrItems[listbox->Sel].Attribute == AMI_POSTMENU_ATTRIB_NON_FOCUS))
				    {	
				        
                        if(listbox->Sel < listbox->FirstVisibleSel)
                        { 
                            TSEMouseListBoxScrollBarMove(listbox, TRUE, 1 ) ;

                            if(!GetFocusItemCount(listbox)) // 
                                break ;
                        }                   
				        
						//Send Fake Action...
						MemCopy(&TempData,Data,sizeof(ACTION_DATA));
						TempData.Input.Data.AmiKey.Key.ScanCode = SCAN_UP;
						TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_NULL ;
						gListBox.HandleAction(listbox, &TempData);
				    }
				}
			}
			else
			{
				if( (listbox->Style == POPUP_STYLE_BBS)|| (listbox->Style == POPUP_STYLE_NORMAL && listbox->PtrItems))	
				{
					//Rollover, find the first focusable item
					for ( i = listbox->ItemCount-1; i >= 0 ; i-- )
					{
						if((listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_NON_FOCUS)&&
					(listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_EXIT_KEY)&&
						   (listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_HIDDEN)) 
						{
						 	listbox->Sel=i;
							break;
						}
					}
				}
				else
					listbox->Sel = listbox->ItemCount - 1;
			}
            break;

            case ControlActionNextDown:
    		if ( (INTN)(listbox->Sel) < listbox->ItemCount-listbox->HiddenItemCount - 1 )
			{
		    	listbox->Sel++;
		    
   		    	if((listbox->Style == POPUP_STYLE_BBS)	|| (listbox->Style == POPUP_STYLE_NORMAL && listbox->PtrItems))		 
  		    	{
								
					if ( (listbox->PtrItems[listbox->Sel].ItemToken==0) || (listbox->PtrItems[listbox->Sel].Attribute == AMI_POSTMENU_ATTRIB_NON_FOCUS))
					{
                        if(listbox->Sel > listbox->LastVisibleSel)
                        {
                            TSEMouseListBoxScrollBarMove(listbox, FALSE, 1 ) ;
                            
                            if(!GetFocusItemCount(listbox))
                                break ;
                        }                       
						
						//Send Fake Action...
						MemCopy(&TempData,Data,sizeof(ACTION_DATA));
						TempData.Input.Data.AmiKey.Key.ScanCode = SCAN_DOWN;
						TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_NULL ;
						gListBox.HandleAction(listbox, &TempData);
					}
		    	}
		
			}
			else
			{	//Rollover, find the first focusable item
				listbox->Sel=0;
				if((listbox->Style == POPUP_STYLE_BBS)	|| (listbox->Style == POPUP_STYLE_NORMAL && listbox->PtrItems))		
				{
					for ( i = 0; i < listbox->ItemCount; i++ )
					{
						if((listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_NON_FOCUS)&&
						(listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_EXIT_KEY)&&
					   		(listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_HIDDEN)) 
						{
					 		listbox->Sel=i;
							break;
						}
					}
				}

			}
            break;

        	case ControlActionAbort:
//				ScreenBuf = SaveScreen();	
				listbox->Callback( listbox->Container, listbox, NULL );
//				RestoreScreen( ScreenBuf );	
			break;

	    	case ControlActionSelect:
//				ScreenBuf = SaveScreen();
	    		if(listbox->Style == POPUP_STYLE_NORMAL && listbox->PtrItems)
	    		{
	    			if(listbox->PtrItems[listbox->Sel].Attribute == AMI_POSTMENU_ATTRIB_NON_FOCUS)
	    				break;
	    		}
		  		listbox->Callback( listbox->Container, listbox, &(listbox->Sel) );
//				RestoreScreen( ScreenBuf );
			break;

	    	default:
			if(listbox->Style == POPUP_STYLE_BBS)	 
	  		{		
				//Start from current selection
				start = listbox->Sel ;
				end = listbox->ItemCount ;
				
				Search:
				for ( i = start; i < end; i++ )
				{
					if(	(listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_NON_FOCUS)&&
						(listbox->PtrItems[i].Attribute != AMI_POSTMENU_ATTRIB_EXIT_KEY))
					{
						
						
						if( (listbox->PtrItems[i].Key.Key.ScanCode != 0) 	||
							(listbox->PtrItems[i].Key.Key.UnicodeChar != 0) ||
							(listbox->PtrItems[i].Key.KeyShiftState != 0) )
						{						
							if( (listbox->PtrItems[i].Key.Key.ScanCode == Data->Input.Data.AmiKey.Key.ScanCode) &&
								 (listbox->PtrItems[i].Key.Key.UnicodeChar == Data->Input.Data.AmiKey.Key.UnicodeChar) &&
								 TseCheckShiftState((Data->Input.Data.AmiKey), listbox->PtrItems[i].Key.KeyShiftState) )
							{
															
								if(listbox->PtrItems[i].Attribute == AMI_POSTMENU_ATTRIB_HIDDEN)
								{
									listbox->Callback( listbox->Container, listbox, &i );
								}
								else
								{
									listbox->Sel = i ;
									listbox->Callback( listbox->Container, listbox, &(listbox->Sel) );
								}	
								
								//Send Fake Action...
								/*
								MemCopy(&TempData,Data,sizeof(ACTION_DATA));
								TempData.Input.Data.AmiKey.Key.ScanCode = 0;
								TempData.Input.Data.AmiKey.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
								gListBox.HandleAction(listbox, &TempData);
								*/
	
								break;
							}
						}
					}
				}
				
				//Wrap and Search remaining items for match
				if(i==listbox->ItemCount && (start != 0))
				{
					start = 0 ;
					end = listbox->Sel ;
					goto Search ;	
				}
			}
        }
    }
	return Status;
}

/**
    Function to set callback.

    @param listbox OBJECT_DATA *container,   OBJECT_CALLBACK callback, VOID *cookie

    @retval STATUS

**/
EFI_STATUS ListBoxSetCallback( LISTBOX_DATA *listbox, OBJECT_DATA *container,   OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( listbox, container, callback, cookie );
}
/**
    Function to set focus.

    @param listbox BOOLEAN focus

    @retval STATUS

**/
EFI_STATUS ListBoxSetFocus(LISTBOX_DATA *listbox, BOOLEAN focus)
{
	listbox->ControlFocus =focus;
	return EFI_SUCCESS;
}
/**
    Function to set position.

    @param listbox UINT16 Left, UINT16 Top

    @retval STATUS

**/
EFI_STATUS ListBoxSetPosition(LISTBOX_DATA *listbox, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition((CONTROL_DATA*) listbox, Left, Top );
}
/**
    Function to set dimension.

    @param listbox UINT16 Width, UINT16 Height

    @retval STATUS

**/
EFI_STATUS ListBoxSetDimensions(LISTBOX_DATA *listbox, UINT16 Width, UINT16 Height )
{
	return gControl.SetDimensions((CONTROL_DATA*) listbox, Width, Height );
}
/**
    Function to set attributes.

    @param listbox UINT8 FGColor, UINT8 BGColor

    @retval STATUS

**/
EFI_STATUS ListBoxSetAttributes(LISTBOX_DATA *listbox, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes( (CONTROL_DATA*)listbox, FGColor, BGColor );
}

/**
    Function to hadnle List Box using mouse

    @param listbox 
    @param action 

    @retval EFI_STATUS

**/
EFI_STATUS TSEMouseListBoxHandleAction( LISTBOX_DATA *listbox, ACTION_DATA *Data)
{
	UINT16	i;	
	UINT32 Action;
	
	Action = MapControlMouseActionHook(&Data->Input.Data.MouseInfo);
	//Exit listbox if clicked outside listbox
	if( (
        (Data->Input.Data.MouseInfo.Top < listbox->Top) ||
        (Data->Input.Data.MouseInfo.Top > (UINT32)(listbox->Top + listbox->Height)) ||
        (Data->Input.Data.MouseInfo.Left < listbox->Left) ||
        (Data->Input.Data.MouseInfo.Left > (UINT32)(listbox->Left + listbox->Width))
		 ) && (!lButtonOnListBoxScroll) // Check for lButtonDown on ScrollBar
		 && ( (SingleClickActivation() && (ControlActionSelect ==Action)) || //If singleClickActivation is enabled/Disable
		 	  (ControlActionAbort == Action) || (ControlActionChoose ==Action ) //Closing listbox whn mouse action happens outside listbox.
		 	)
	    )
	{
        listbox->Callback( listbox->Container, listbox, NULL );
	}
	//Reset lbutton positions
	if (ControlMouseActionLeftUp == Action
		|| ControlActionChoose == Action
		|| ControlActionSelect == Action)
	{
		lButtonOnListBoxScroll = FALSE;
		lButtonInitialPosition = 0;
		lButtonVaryingPosition = 0;
	}
	//If scrollbar present
	if ( listbox->UseScrollbar )
	{
		//Check if clicked on scroll bar area
		if( ( Data->Input.Data.MouseInfo.Top > (UINT32)listbox->Top && (Data->Input.Data.MouseInfo.Top < (UINT32)(listbox->Top + listbox->Height)) ) //Clicked inside listbox area
			&& (Data->Input.Data.MouseInfo.Left == (UINT32)(listbox->Left + listbox->Width - 2)) //Clicked on scrollbar area
			)
		{
			//If clicked on UP_ARROW
			if(((Data->Input.Data.MouseInfo.Top == (UINT32)(listbox->Top + 1)) &&(Data->Input.Data.MouseInfo.Left == (UINT32) (listbox->Left + listbox->Width -2))) //Clicked on UP_ARROW
				 &&((TSEMOUSE_LEFT_CLICK == Data->Input.Data.MouseInfo.ButtonStatus)||(TSEMOUSE_LEFT_DCLICK == Data->Input.Data.MouseInfo.ButtonStatus))// Check for left click and left double click to scroll the frame if click on arrow buttons of scroll bar.
				)
			{
				ScrollBarMoveAction = TRUE;
				MouseListBoxScrollBarMove( listbox, TRUE, 1 );
				return EFI_SUCCESS;//Returning by consuming the click when the clicked on the arrow buttons of scroll bar 
			}
			//If clicked on DOWN_ARROW
			else if(((Data->Input.Data.MouseInfo.Top == (UINT32) (listbox->Top + listbox->Height-2)) &&(Data->Input.Data.MouseInfo.Left == (UINT32) (listbox->Left + listbox->Width-2))) //Clicked on DOWN_ARROW
						&&((TSEMOUSE_LEFT_CLICK == Data->Input.Data.MouseInfo.ButtonStatus)||(TSEMOUSE_LEFT_DCLICK == Data->Input.Data.MouseInfo.ButtonStatus))// Check for left click and left double click to scroll the frame if click on arrow buttons of scroll bar.
					)
			{
				ScrollBarMoveAction = TRUE;
				MouseListBoxScrollBarMove( listbox, FALSE, 1 );
				return EFI_SUCCESS;// Returning by consuming the click when the clicked on the arrow buttons of scroll bar 
			}
			//If clicked on scrollbar
			else if( ((Data->Input.Data.MouseInfo.Top < gListBoxScrollBarBottom) && (Data->Input.Data.MouseInfo.Top > gListBoxScrollBarTop)) //Scrollbar area
						&& (ControlMouseActionLeftDown == Action) //Mouse lbutton down
						&&	((ControlActionChoose != Action) || (ControlActionSelect != Action)) //Neglecting LEFT_DCLICK and LEFT_CLICK on scrollbar area
					)
			{
				lButtonOnListBoxScroll = TRUE;
				ScrollBarMoveAction = TRUE;
				if ( lButtonInitialPosition == 0 )//To get initial lButtonDown position
				{
					lButtonInitialPosition = Data->Input.Data.MouseInfo.Top;
				}
				if ( lButtonInitialPosition != Data->Input.Data.MouseInfo.Top )
				{
					lButtonVaryingPosition = Data->Input.Data.MouseInfo.Top;
					//Move scrollbar upwards
					if( lButtonInitialPosition > lButtonVaryingPosition )
						MouseListBoxScrollBarMove( listbox, TRUE, lButtonInitialPosition - lButtonVaryingPosition );

					//Move scrollbar downwards
					else
						MouseListBoxScrollBarMove( listbox, FALSE, lButtonVaryingPosition - lButtonInitialPosition );
					lButtonInitialPosition = lButtonVaryingPosition;
				}
				return EFI_SUCCESS;
			}
			//If mouse clicked above DOWN_ARROW and below ScrollBar
			else if( (Data->Input.Data.MouseInfo.Top < (UINT32) (listbox->Top + listbox->Height-3)) //Clicked above DOWN_ARROW
						&& (Data->Input.Data.MouseInfo.Top >= gListBoxScrollBarBottom)  //Clicked below ScrollBar							
						&& (ControlMouseActionLeftDown == Action) //Action is LEFT_DOWN
					)
			{
				ScrollBarMoveAction = TRUE;
				MouseListBoxScrollBarMove( listbox, FALSE, Data->Input.Data.MouseInfo.Top - gListBoxScrollBarBottom );
				return EFI_SUCCESS;
			}
			//If mouse clicked below UP_ARROW and above ScrollBar
			else if( (Data->Input.Data.MouseInfo.Top > (UINT32)(listbox->Top + 1)) //Clicked below UP_ARROW
						&& (Data->Input.Data.MouseInfo.Top <= gListBoxScrollBarTop) //Clicked above ScrollBar
						&& (ControlMouseActionLeftDown == Action) //Action is LEFT_DOWN
					)
			{
				ScrollBarMoveAction = TRUE;
				MouseListBoxScrollBarMove( listbox, TRUE, gListBoxScrollBarTop - Data->Input.Data.MouseInfo.Top);
				return EFI_SUCCESS;
			}
			return EFI_SUCCESS;
		}
		//If mouse button clicked on scrollbar and moved outside the scroll area
		else if( ControlMouseActionLeftDown == Action //Action is LEFT_DOWN
					&& (lButtonInitialPosition != 0)
					&& ((ControlActionChoose != Action) || (ControlActionSelect != Action)) //Neglecting LEFT_CLICK and LEFT_DCLICK
				)
		{
			lButtonVaryingPosition = Data->Input.Data.MouseInfo.Top;
			//Move scrollbar upwards
			if( lButtonInitialPosition > lButtonVaryingPosition )
				MouseListBoxScrollBarMove( listbox, TRUE, lButtonInitialPosition - lButtonVaryingPosition );

			//Move scrollbar downwards
			else
				MouseListBoxScrollBarMove( listbox, FALSE, lButtonVaryingPosition - lButtonInitialPosition );
			lButtonInitialPosition = lButtonVaryingPosition;
			
			//Reset lButtonDown position when lButton releases
			if (ControlMouseActionLeftUp == Action)
			{
				lButtonInitialPosition = lButtonVaryingPosition = 0;
			}
			return EFI_SUCCESS;
		}
	}
	//If clicked Inside listbox area other than scroll area to handle highlight or selection of the option
	if( ( Data->Input.Data.MouseInfo.Top > (UINT32)listbox->Top && (Data->Input.Data.MouseInfo.Top < (UINT32)(listbox->Top + listbox->Height)) ) //Clicked inside ListBox area
		&& ( (Data->Input.Data.MouseInfo.Left > listbox->Left)  && (Data->Input.Data.MouseInfo.Left < (listbox->Left + (UINT32)(listbox->Width - 2))) ) //Width calculation			
		&& (ControlActionChoose == Action || ControlActionSelect == Action) //check LEFT_DCLICK or LEFT_CLICK
		)
	{		
		for (i = listbox->FirstVisibleSel; i <= listbox->LastVisibleSel; i++ )
		{
			if(Data->Input.Data.MouseInfo.Top == (UINT32) (listbox->Top + i - listbox->FirstVisibleSel + 1))
			{
				switch( MapControlMouseActionHook(&Data->Input.Data.MouseInfo) )
				{
					case ControlActionChoose:
						listbox->Sel=i;
					break;

					case ControlActionSelect:
						if (SingleClickActivation ())		// providing select option for single click too
						{
							listbox->Sel=i;
						}
						listbox->Callback( listbox->Container, listbox, &(listbox->Sel) );
					break;
					default: // Lot of Not handled switch 
						break;
				}
			}
		}
	}
	return EFI_SUCCESS;
}

/**
    Function unsupported.

    @param object VOID *frame, UINT16 *height

    @retval EFI_STATUS

**/
EFI_STATUS ListBoxGetControlHeight( LISTBOX_DATA *object,VOID *frame, UINT16 *height )
{
	return EFI_UNSUPPORTED;
}

/**
    Adds scroll bar functionality for frames with many controls

    @param frame Pointer to the frame data
    @param bScrollUp Go up is TRUE, else to down one control

    @retval EFI_STATUS Status - EFI_SUCCESS if successful, else
        EFI_UNSUPPORTED

**/

EFI_STATUS TSEMouseListBoxScrollBarMove(  LISTBOX_DATA *listbox, BOOLEAN bScrollUp, UINT16 Size )
{	
	if ( listbox->ItemCount == 0 )
		return EFI_UNSUPPORTED;

	switch ( bScrollUp )
	{
		case TRUE:

			if( Size >= listbox->FirstVisibleSel )
				Size = listbox->FirstVisibleSel;

			//If FirstVisibleCtrl is not equal to first control then move scrollbar till it reaches top
			if(listbox->FirstVisibleSel > 0)
			{
				listbox->FirstVisibleSel = listbox->FirstVisibleSel - Size;
				listbox->LastVisibleSel = listbox->LastVisibleSel - Size;
			}
			break;

		case FALSE:
			//If the size of scroll to move exceeds the remaining control count then change the size value based on the remaining control count
			if( Size >= (listbox->ItemCount - listbox->LastVisibleSel) )
				Size = listbox->ItemCount - listbox->LastVisibleSel - 1;

			//If LastVisibleCtrl is not equal to last control then move scrollbar till it reaches bottom
			if(listbox->LastVisibleSel < listbox->ItemCount - 1)
			{
				listbox->FirstVisibleSel = listbox->FirstVisibleSel + Size;
				listbox->LastVisibleSel = listbox->LastVisibleSel + Size;
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
