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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ResetButton.c $
//
// $Author: Premkumara $
//
// $Revision: 4 $
//
// $Date: 11/21/11 11:06a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file ResetButton.c
    This file contains code to handle the uefi actions

**/

#include "minisetup.h"

RESET_BUTTON_METHODS gResetButton =
{
	(OBJECT_METHOD_CREATE)ResetButtonCreate,
	(OBJECT_METHOD_DESTROY)LabelDestroy,
	(OBJECT_METHOD_INITIALIZE)ResetButtonInitialize,
	(OBJECT_METHOD_DRAW)ResetButtonDraw,
	(OBJECT_METHOD_HANDLE_ACTION)ResetButtonHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)LabelSetCallback,
	(CONTROL_METHOD_SET_FOCUS)LabelSetFocus,
	(CONTROL_METHOD_SET_POSITION)LabelSetPosition,
	(CONTROL_METHOD_SET_DIMENSIONS)LabelSetDimensions,
	(CONTROL_METHOD_SET_ATTRIBUTES)LabelSetAttributes,
	(CONTROL_METHOD_GET_CONTROL_HIGHT) LabelGetControlHeight
};

/**
    Function to create an ResetButton Control.

    @param object 

    @retval EFI_STATUS

**/
EFI_STATUS ResetButtonCreate( VOID **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(RESET_BUTTON_DATA) );

		if ( *object == NULL )
    {
			Status = EFI_OUT_OF_RESOURCES;
      goto DONE;
    }
	}

	Status = gControl.Create((void**) object );
	if ( ! EFI_ERROR(Status) )
		((RESET_BUTTON_DATA*)(*object))->Methods = &gResetButton;

DONE:
	return Status;
}

/**
    Function to Initialize an Action Control.

    @param object VOID *data

    @retval EFI_STATUS

**/
EFI_STATUS ResetButtonInitialize( VOID *object, VOID *data )
{
  EFI_STATUS Status = EFI_SUCCESS;
  RESET_BUTTON_DATA *ResetButton = (RESET_BUTTON_DATA*) object;

	Status = gControl.Initialize( ResetButton, data );
	if (EFI_ERROR(Status))
  {
    goto DONE;
  }

	// add extra initialization here...
  ResetButton->ControlData.ControlHelp = ResetButton->ControlData.ControlHelp? ResetButton->ControlData.ControlHelp :
                                      UefiGetHelpField(ResetButton->ControlData.ControlPtr);

	SetControlColorsHook(NULL, NULL, NULL, NULL, &(ResetButton->SelBGColor), &(ResetButton->SelFGColor),
			               &(ResetButton->BGColor), &(ResetButton->FGColor), NULL, NULL, NULL, NULL, NULL, NULL, NULL );

DONE:
  return Status;
}

/**
    Function to draw ResetButton.

    @param object 

    @retval EFI_STATUS

**/
EFI_STATUS ResetButtonDraw( VOID *object )
{
  EFI_STATUS Status = EFI_SUCCESS;
  RESET_BUTTON_DATA *ResetButton = (RESET_BUTTON_DATA*) object;

	CHAR16 *text;
  UINT16 token = 0;
	UINT8 ColorLabel = ResetButton->FGColor;

  // check conditional ptr if necessary
    //Support grayout condition for readonly controls
	//if( ResetButton->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &ResetButton->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				Status = EFI_WARN_WRITE_FAILURE;
				ColorLabel = CONTROL_GRAYOUT_COLOR;
				break;
			default:
				Status = EFI_UNSUPPORTED;
        goto DONE;
				break;
		}
	//}

  token = UefiGetPromptField(ResetButton->ControlData.ControlPtr);
	text = HiiGetString( ResetButton->ControlData.ControlHandle, token);

	//TSE_MULTILINE_CONTROLS moved to binary
	if(ResetButton->Height>1 && IsTSEMultilineControlSupported())
	{
		DrawMultiLineStringWithAttribute( ResetButton->Left , ResetButton->Top,
				(UINTN)(ResetButton->Width),(UINTN) ResetButton->Height,
				text,(UINT8)( (ResetButton->ControlFocus) ?
					 ResetButton->SelBGColor  | ResetButton->SelFGColor :
					 ResetButton->BGColor | ColorLabel ));


	}
	else
	{
		// use frame width minus margins as available space
		// boundary overflow  check
		if ( (TestPrintLength( text) / (NG_SIZE)) > (UINTN)(ResetButton->Width))
				text[HiiFindStrPrintBoundary(text,(UINTN)(ResetButton->Width))] = L'\0';

		DrawStringWithAttribute( ResetButton->Left  , ResetButton->Top, text,
				(UINT8)( (ResetButton->ControlFocus) ?
						 ResetButton->SelBGColor  | ResetButton->SelFGColor :
						 ResetButton->BGColor | ColorLabel /*Action->FGColor*/  ));
	}

	MemFreePointer( (VOID **)&text );
	FlushLines( ResetButton->Top  , ResetButton->Top );

DONE:
  return Status;
}

/**
    Function to handle the ResetButton.

    @param object ACTION_DATA *data

    @retval EFI_STATUS

**/
EFI_STATUS ResetButtonHandleAction(VOID *object, ACTION_DATA *data)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  RESET_BUTTON_DATA *ResetButton = (RESET_BUTTON_DATA*) object;
  CONTROL_ACTION CtrlAction;
  UINT16 DefaultId = 0;

  if (( data->Input.Type == ACTION_TYPE_KEY )|| (data->Input.Type == ACTION_TYPE_MOUSE))
  {
	if( data->Input.Type == ACTION_TYPE_KEY )
    CtrlAction = MapControlKeysHook(data->Input.Data.AmiKey);
	else
	{
		 CtrlAction = MapControlMouseActionHook(&data->Input.Data.MouseInfo);
  	}
	    switch(CtrlAction)
	    {
	    case ControlActionSelect:
	      //call a wapper function to update the Default vaule
			DefaultId = UefiGetResetButtonDefaultid( ResetButton->ControlData.ControlPtr );

			if(0xFFFF == DefaultId)
			    return EFI_UNSUPPORTED;

	      Status = UefiupdateResetButtonDefault(ResetButton->ControlData,DefaultId);
	       break;

	    default:
	      break;
	    }
  }
  return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 2015, American Megatrends, Inc.         	  **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
