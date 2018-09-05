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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/application.c $
//
// $Author: Rajashakerg $
//
// $Revision: 7 $
//
// $Date: 6/29/11 6:21p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file application.c
    This file contains code to handle Application.

**/

#include "minisetup.h"

APPLICATION_METHODS gApplication =
{
	(OBJECT_METHOD_CREATE)ApplicationCreate,
	(OBJECT_METHOD_DESTROY)ApplicationDestroy,
	(OBJECT_METHOD_INITIALIZE)ApplicationInitialize,
	(OBJECT_METHOD_DRAW)ApplicationEmptyMethod,
	(OBJECT_METHOD_HANDLE_ACTION)ApplicationHandleAction,
	(OBJECT_METHOD_SET_CALLBACK)ApplicationSetCallback,
	ApplicationAddPage
};

/**
    Function to create an application, which uses the Object functions.

    @param object 

    @retval status

**/
EFI_STATUS ApplicationCreate( APPLICATION_DATA **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(APPLICATION_DATA) );

		if ( *object == NULL )
			return EFI_OUT_OF_RESOURCES;
	}

//	Status = gObject.Create( object );
	Status = gObject.Create((void**) object );
	if ( EFI_ERROR( Status ) )
	{
		MemFreePointer( (VOID **)object );
		return Status;
	}

	(*object)->Methods = &gApplication;
	(*object)->CompleteRedraw = TRUE;

	return Status;
}

/**
    Function to destroy an application, which uses the Object functions.

    @param app BOOLEAN freeMem

    @retval status

**/
EFI_STATUS ApplicationDestroy( APPLICATION_DATA *app, BOOLEAN freeMem )
{
	UINT32 i;

	if ( app->Action )
		gAction.Destroy( app->Action, TRUE );

	app->Action = NULL;
	gMsgBoxAction = NULL;			//if not made NULL here, then TSE will try to use the freed memory in above step.

	for ( i = 0; i < app->PageCount; i++ )
		if(app->PageList[i] !=NULL)
			gPage.Destroy( app->PageList[i], TRUE );

	if ( app->PageList )
		MemFreePointer( (VOID **)&app->PageList );

	for ( i = 0; i < app->HotKeyCount; i++ )
		gHotKey.Destroy( app->HotKeyList[i], TRUE );
	
	for ( i = 0; i < app->HotClickCount; i++ )
		gHotClick.Destroy( app->HotClickList[i], TRUE );

	if ( app->HotKeyList )
		MemFreePointer( (VOID **)&app->HotKeyList );
	
	if ( app->HotClickList )
		MemFreePointer( (VOID **)&app->HotClickList );
	

	gObject.Destroy( app, FALSE );

	if ( freeMem )
		MemFreePointer( (VOID **)&app );

	return EFI_SUCCESS;
}

EFI_STATUS ApplicationEmptyMethod( APPLICATION_DATA *app )
{
	return EFI_SUCCESS;
}

/**
    Function to initialize an application, which uses the Object functions.

    @param app VOID *data

    @retval status

**/
extern UINT32	gtempCurrentPage;
EFI_STATUS ApplicationInitialize( APPLICATION_DATA *app, VOID *data )
{
	EFI_STATUS Status;

	UINT32 i;

	Status = gAction.Create( &app->Action );
	if ( EFI_ERROR( Status ) )
		return Status;

	if ( gPages->PageCount == 0 )
		return EFI_OUT_OF_RESOURCES;

	for ( i = 0; i < gPages->PageCount; i++ )
	{
		if (NULL == gApp)
		{
			gtempCurrentPage = i;					//Will be used in UefiGetQuestionOffset. Since gApp will not be filled but we need handle to find name value variable so
		}												//saving current page
		else
		{
			gApp->CurrentPage = i;
		}
		ApplicationAddPage( app, (UINT8 *)data + gPages->PageList[i] );
	}

	app->HotKeyList = EfiLibAllocateZeroPool( sizeof(HOTKEY_DATA *) * gHotKeyCount );
	
	app->HotClickList = EfiLibAllocateZeroPool( sizeof(HOTCLICK_DATA *) * gHotClickCount );

	if ( app->HotKeyList == NULL )
		return Status;
	
	if ( app->HotClickList == NULL )
		return Status;

	app->HotKeyCount = gHotKeyCount;
	
	app->HotClickCount = gHotClickCount;

	for ( i = 0; i < app->HotKeyCount; i++ )
	{
		if ( gHotKey.Create( (void**)&app->HotKeyList[i] ) != EFI_SUCCESS )
			continue;

		gHotKey.Initialize( app->HotKeyList[i], &gHotKeyInfo[i] );
		gHotKey.SetCallback( app->HotKeyList[i], app, (OBJECT_CALLBACK)gHotKeyInfo[i].Callback, NULL );
	}

	for ( i = 0; i < app->HotClickCount; i++ )
	{
		if ( gHotClick.Create( (void**)&app->HotClickList[i] ) != EFI_SUCCESS )
			continue;

		gHotClick.Initialize( app->HotClickList[i], &gHotClickInfo[i] );
		gHotClick.SetCallback( app->HotClickList[i], app, gHotClickInfo[i].Callback, NULL );
	}

	return Status;
}

/**
    Function to handle the application actions.

    @param app ACTION_DATA *action

    @retval status

**/
EFI_STATUS ApplicationHandleAction( APPLICATION_DATA *app, ACTION_DATA *action )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT32 i;

	for ( i = 0; i < app->HotKeyCount; i++ )
	{
		Status = gHotKey.HandleAction( app->HotKeyList[i], action );

		if ( ! EFI_ERROR(Status) )
			goto DONE;
	}
	

	if ((31 == gMaxRows) && (100 == gMaxCols))
	{
		for ( i = 0; i < app->HotClickCount/2; i++ )
		{
			Status = gHotClick.HandleAction( app->HotClickList[i], action );
	
			if ( ! EFI_ERROR(Status) )
				return Status;
		}
	}
	else if ((24 == gMaxRows) && (80 == gMaxCols))
	{
		for ( i = (UINT32)(app->HotClickCount/2); i < (app->HotClickCount-1); i++ )
		{
			Status = gHotClick.HandleAction( app->HotClickList[i], action );
	
			if ( ! EFI_ERROR(Status) )
				return Status;
		}
	}
DONE:
	if(Status!=EFI_SUCCESS && IsToggleStateKey(action))	
		Status = EFI_SUCCESS;

	return Status;
}

EFI_STATUS ApplicationSetCallback( APPLICATION_DATA *app, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gObject.SetCallback( app, container, callback, cookie );
}

/**
    Function to add a page to the application.

    @param app VOID *data

    @retval status

**/
EFI_STATUS ApplicationAddPage( APPLICATION_DATA *app, VOID *data )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;
	PAGE_DATA **page;
	UINT32 count = app->PageCount;

	page = MemReallocateZeroPool( app->PageList, count * sizeof(PAGE_DATA *), (count + 1) * sizeof(PAGE_DATA *) );
	if ( page == NULL )
		return Status;

	app->PageList = page;
	page = &app->PageList[ count ];

	if( ((PAGE_INFO*)data)->PageHandle == 0)
	{
        app->PageCount++;
		//return EFI_SUCCESS , page space left , but no page to create.
		return EFI_SUCCESS;
	}

	Status = gPage.Create( (void**)page );
	if ( ! EFI_ERROR( Status ) )
	{
		Status = gPage.Initialize( *page, data );
		if ( ! EFI_ERROR( Status ) )
			app->PageCount++;
		else
		{
			gPage.Destroy( *page, TRUE );
			*page = NULL;
		}
	}

	return Status;
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
