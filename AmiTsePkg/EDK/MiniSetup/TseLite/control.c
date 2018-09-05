//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/control.c $
//
// $Author: Rajashakerg $
//
// $Revision: 4 $
//
// $Date: 11/21/11 8:49a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file control.c
    This file contains code to handle controls

**/

#include "minisetup.h"

CONTROL_METHODS gControl =
{
	(OBJECT_METHOD_CREATE)ControlCreate,
	(OBJECT_METHOD_DESTROY)ControlDestroy,
	(OBJECT_METHOD_INITIALIZE)ControlInitialize,
	(OBJECT_METHOD_DRAW)ControlEmptyMethod,
	(OBJECT_METHOD_HANDLE_ACTION)ControlHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)ControlSetCallback,
	ControlSetFocus,
	ControlSetPosition,
	ControlSetDimensions,
	ControlSetAttributes
};

/**
    Function to create the cotrol options using object functions.

    @param object 

    @retval status

**/
EFI_STATUS ControlCreate( CONTROL_DATA **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(CONTROL_DATA) );

		if ( *object == NULL )
			return EFI_OUT_OF_RESOURCES;
	}

	Status = gObject.Create((void**) object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gControl;

	return Status;
}

/**
    Function to destroy the cotrol options using object functions.

    @param control BOOLEAN freeMem

    @retval status

**/
EFI_STATUS ControlDestroy( CONTROL_DATA *control, BOOLEAN freeMem )
{
	if(NULL == control)
		return EFI_SUCCESS;
		
	gObject.Destroy( control, FALSE );

	if ( freeMem )
		MemFreePointer( (VOID **)&control );

	return EFI_SUCCESS;
}

/**
    Function to Initialize the cotrol options using object functions.

    @param control VOID *data

    @retval status

**/
EFI_STATUS ControlInitialize( CONTROL_DATA *control, VOID *data )
{
	EFI_STATUS Status;

	Status = gObject.Initialize( control, data );
	if (EFI_ERROR(Status))
		return Status;

//	if ( ((CONTROL_INFO *)data)->ControlPtr == (UINTN)NULL )
	if ( ((CONTROL_INFO *)data)->ControlPtr == NULL )
		return EFI_UNSUPPORTED;

	if ( ! ((CONTROL_INFO *)data)->ControlFlags.ControlVisible )
		Status = EFI_UNSUPPORTED;
	else
		MemCopy( &control->ControlData, data, sizeof(CONTROL_INFO) );

	return Status;
}
/**
    Function to draw control.

    @param control 

    @retval status

**/
EFI_STATUS ControlDraw( CONTROL_DATA *control )
{
	return gObject.Draw( control );
}
/**
    Empty control method.

    @param control 

    @retval status

**/
EFI_STATUS ControlEmptyMethod( CONTROL_DATA *control )
{
	return EFI_SUCCESS;
}
/**
    Function to set callback.

    @param control OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie 

    @retval STATUS

**/
EFI_STATUS ControlSetCallback( CONTROL_DATA *control, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gObject.SetCallback( control, container, callback, cookie );
}
/**
    Function to handle action.

    @param control ACTION_DATA *Data

    @retval STATUS

**/
EFI_STATUS ControlHandleAction( CONTROL_DATA *control, ACTION_DATA *Data )
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set focus.

    @param control BOOLEAN focus

    @retval status

**/
EFI_STATUS ControlSetFocus( CONTROL_DATA *control, BOOLEAN focus )
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set the position.

    @param control UINT16 Left, UINT16 Top

    @retval status

**/
EFI_STATUS ControlSetPosition( CONTROL_DATA *control, UINT16 Left, UINT16 Top )
{
	control->Left = Left;
	control->Top = Top;

	return EFI_SUCCESS;
}
/**
    Function to set dimensions.

    @param control UINT16 Width, UINT16 Height

    @retval status

**/
EFI_STATUS ControlSetDimensions(CONTROL_DATA *control, UINT16 Width, UINT16 Height)
{
	control->Width = Width;
	control->Height = Height;

	return EFI_SUCCESS;
}

/**
    Function to set the control attributes

    @param control UINT8 FGColor, UINT8 BGColor

    @retval status

**/
EFI_STATUS ControlSetAttributes( CONTROL_DATA *control, UINT8 FGColor, UINT8 BGColor )
{
	if ( FGColor != 0 )
		control->FGColor = FGColor;

	if ( BGColor != 0 )
		control->BGColor = BGColor;

	return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
