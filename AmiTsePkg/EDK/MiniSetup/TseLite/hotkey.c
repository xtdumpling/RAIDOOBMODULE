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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/hotkey.c $
//
// $Author: Madhans $
//
// $Revision: 4 $
//
// $Date: 2/19/10 1:04p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file hotkey.c
    This file contains code to handle hotkey operations

**/

#include "minisetup.h"

HOTKEY_METHODS gHotKey =
{
	(OBJECT_METHOD_CREATE)HotKeyCreate,
	(OBJECT_METHOD_DESTROY)HotKeyDestroy,
	(OBJECT_METHOD_INITIALIZE)HotKeyInitialize,
	(OBJECT_METHOD_DRAW)HotKeyEmptyMethod,
	(OBJECT_METHOD_HANDLE_ACTION)HotKeyHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)HotKeySetCallback
};
/**
    Function create a Hotkey, which uses the Object functions.

    @param object 

    @retval status

**/
EFI_STATUS HotKeyCreate( HOTKEY_DATA **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(HOTKEY_DATA) );

		if ( *object == NULL )
			return EFI_OUT_OF_RESOURCES;
	}

	Status = gObject.Create( (void**)object );

	if (EFI_ERROR(Status))
	{
		MemFreePointer( (VOID **)object );
		return Status;
	}

	(*object)->Methods = &gHotKey;

	return Status;
}
/**
    Function destroy a Hotkey, which uses the Object functions.

    @param hotkey BOOLEAN freeMem

    @retval status

**/
EFI_STATUS HotKeyDestroy( HOTKEY_DATA *hotkey, BOOLEAN freeMem )
{

	gObject.Destroy( hotkey, FALSE );

	if ( freeMem )
		MemFreePointer( (VOID **)&hotkey );

	return EFI_SUCCESS;
}
/**
    Function Initialize a Hotkey, which uses the Object functions.

    @param hotkey VOID *data

    @retval status

**/
EFI_STATUS HotKeyInitialize( HOTKEY_DATA *hotkey, VOID *data )
{
	HOTKEY_TEMPLATE *template = (HOTKEY_TEMPLATE *)data;

	MemCopy( &hotkey->HotKeyData.Key, &(template->Key), sizeof(EFI_INPUT_KEY) );

	hotkey->HotKeyData.KeySftSte=template->KeySftSte;

	hotkey->BasePage = template->BasePage;

	return EFI_SUCCESS;
}

EFI_STATUS HotKeyEmptyMethod( HOTKEY_DATA *hotkey )
{
	return EFI_SUCCESS;
}
/**
    Function to handle the HotKey actions.

    @param hotkey ACTION_DATA *action

    @retval status

**/
EFI_STATUS HotKeyHandleAction( HOTKEY_DATA *hotkey, ACTION_DATA *action )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	if ( action->Input.Type != ACTION_TYPE_KEY )
		return Status;
	
	if ( (! EfiCompareMem( &action->Input.Data.AmiKey.Key, &hotkey->HotKeyData.Key, sizeof(EFI_INPUT_KEY) ) )
		&& ( TseCheckShiftState( (action->Input.Data.AmiKey), (hotkey->HotKeyData.KeySftSte) ) ) )
	{
		if ( hotkey->Callback )
			hotkey->Callback( hotkey->Container, hotkey, hotkey->Cookie );

		gAction.ClearAction( action );
		Status = EFI_SUCCESS;
	}

	return Status;
}
/**
    Function to set callback.

    @param hotkey OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval STATUS

**/
EFI_STATUS HotKeySetCallback( HOTKEY_DATA *hotkey, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gObject.SetCallback( hotkey, container, callback, cookie );
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
