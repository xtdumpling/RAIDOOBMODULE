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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/object.c $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:04p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file object.c
    This file contains code to handle Objects

**/

#include "minisetup.h"

OBJECT_METHODS gObject = 
{
	ObjectCreate,
	ObjectDestroy,
	ObjectInitialize,
	ObjectDraw,
	ObjectHandleAction,
	ObjectSetCallback
};

/**
    function to create an object

    @param object 

    @retval status

**/
EFI_STATUS ObjectCreate( VOID **AnyObject )
{
	OBJECT_DATA **object = (OBJECT_DATA **)AnyObject;
	if ( *object == NULL )
		*object = EfiLibAllocateZeroPool( sizeof(OBJECT_DATA) );

	if ( *object == NULL )
		return EFI_OUT_OF_RESOURCES;

	(*object)->Methods = &gObject;

	return EFI_SUCCESS;
}

/**
    function to Destroy an object

    @param object BOOLEAN freeMem

    @retval status

**/
//EFI_STATUS ObjectDestroy( OBJECT_DATA *object, BOOLEAN freeMem )
EFI_STATUS ObjectDestroy( VOID *object, BOOLEAN freeMem )
{
	if ( freeMem )
		MemFreePointer( (VOID **)&object );

	return EFI_SUCCESS;
}
/**
    Function to initialize an object

    @param object VOID *data

    @retval status

**/
//EFI_STATUS ObjectInitialize( OBJECT_DATA *object, VOID *data )
EFI_STATUS ObjectInitialize( VOID *object, VOID *data )
{
	return EFI_SUCCESS;
}
/**
    Function to draw an object

    @param object 

    @retval status

**/
//EFI_STATUS ObjectDraw( OBJECT_DATA *object )
EFI_STATUS ObjectDraw( VOID *object )
{
	return EFI_SUCCESS;
}
/**
    Function to handle object action

    @param object ACTION_DATA *Data

    @retval status

**/
//EFI_STATUS ObjectHandleAction( OBJECT_DATA *object, ACTION_DATA *Data )
EFI_STATUS ObjectHandleAction( VOID *object, ACTION_DATA *Data )
{
	return EFI_UNSUPPORTED;
}
/**
    Function to set  object callback

    @param object OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie

    @retval status

**/
//EFI_STATUS ObjectSetCallback( VOID *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
EFI_STATUS ObjectSetCallback( VOID *AnyObject, VOID * container, OBJECT_CALLBACK callback, VOID *cookie )
{
	 OBJECT_DATA *object = (OBJECT_DATA *)AnyObject;
	object->Callback = callback;
	object->Container = container,
	object->Cookie = cookie;

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
