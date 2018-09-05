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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Action.c $
//
// $Author: Rajashakerg $
//
// $Revision: 7 $
//
// $Date: 9/21/12 12:34p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file Action.c
    This file contains code to handle different actions.

**/

#include "minisetup.h"

///////////////////////////////////
//VARIABLES DECLARATION
//////////////////////////////////
ACTION_METHODS gAction =
{
	ActionCreate,
	ActionDestroy,
	ActionGetAction,
	ActionClearAction
};
///////////////////////////////////
//FUNCTIONS DECLARATION
//////////////////////////////////
EFI_STATUS 		_ActionReadMouse(MOUSE_INFO *MouseInfo);
BOOLEAN   		FlushKeysAfterRead(void);

/**
    Function to create an action.

    @param object 

    @retval status

**/
EFI_STATUS ActionCreate( ACTION_DATA **object )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( *object == NULL )
		*object = EfiLibAllocateZeroPool( sizeof(ACTION_DATA) );

	if ( *object == NULL )
		return EFI_OUT_OF_RESOURCES;

	(*object)->Methods = &gAction;

	_ActionInstallTimer( *object, ONE_SEC / 10 );

	return Status;
}

/**
    Function to destroy an action.

    @param action BOOLEAN freeMem

    @retval status

**/
EFI_STATUS ActionDestroy( ACTION_DATA *action, BOOLEAN freeMem )
{
	TimerStopTimer( &action->TimerEvent );

	if ( freeMem )
		MemFreePointer( (VOID **)&action );

	return EFI_SUCCESS;
}

/**
    Function to get an action.

    @param action 

    @retval status

**/
EFI_STATUS ActionGetAction( ACTION_DATA *action )
{
	EFI_STATUS Status;
	AMI_EFI_KEY_DATA AmiKey;
	MOUSE_INFO MouseInfo;	  

	if ( action->Input.Type != ACTION_TYPE_NULL )
		return EFI_SUCCESS;

	MemSet(&AmiKey, sizeof(AMI_EFI_KEY_DATA), 0);	//makesure AmiKey does not have any garbage data
	Status = ActionReadKey( &AmiKey, TIMER_ONE_SECOND / 20 );

	if ( ! EFI_ERROR( Status ) )
	{	//If Key press return that first.
		action->Input.Type = ACTION_TYPE_KEY;
		MemCopy( &action->Input.Data.AmiKey, &AmiKey, sizeof(AMI_EFI_KEY_DATA) );
		return Status;
	}

	//Read the Mouse Movement Co-ordinates and the Mouse Click
    Status = _ActionReadMouse(&MouseInfo);
    if ( ! EFI_ERROR( Status ) )
    {
		//Not updating the action data for TSEMOUSE_RIGHT_DOWN and TSEMOUSE_RIGHT_UP
        if((MouseInfo.ButtonStatus!=TSEMOUSE_NULL_CLICK)&&(MouseInfo.ButtonStatus != TSEMOUSE_RIGHT_DOWN)&&(MouseInfo.ButtonStatus != TSEMOUSE_RIGHT_UP))
        {
            action->Input.Type = ACTION_TYPE_MOUSE;
            MemCopy( &action->Input.Data.MouseInfo, &MouseInfo, sizeof(MOUSE_INFO) );
			return Status;
        }
    }

    if ( action->TimerCount > 0 )
   	{
    		action->Input.Type = ACTION_TYPE_TIMER;
    		action->Input.Data.TimerCount = action->TimerCount;
    		action->TimerCount = 0;
    		return EFI_SUCCESS;
   	}

	return Status;
}
/**
    Function to clear actions 

    @param action 

    @retval STATUS

**/
EFI_STATUS ActionClearAction( ACTION_DATA *action )
{
	MemSet( &action->Input, sizeof(ACTION_INPUT), 0 );
	return EFI_SUCCESS;
}

//
// Internal functions
//
/**
    Function to read a key.

    @param pAmiKey UINT64 Timeout

    @retval status

**/
EFI_STATUS _ActionReadKey( AMI_EFI_KEY_DATA *pAmiKey, UINT64 Timeout )
{
	EFI_STATUS Status;
	EFI_STATUS StatusFlush;
	EFI_EVENT EventList[2] = { NULL, NULL };
	UINTN Count = 1, Index = 1;
    AMI_EFI_KEY_DATA    KeyFlush;

	if ( Timeout )
	{
		EventList[0] = gST->ConIn->WaitForKey;

		Status = TimerCreateTimer( &EventList[1], NULL, NULL, TimerRelative, Timeout, EFI_TPL_CALLBACK );
		if ( EFI_ERROR( Status ) )
			return Status;

		Count++;

		gBS->WaitForEvent( Count, EventList, &Index );
	
		TimerStopTimer( &EventList[1] );

		if ( Index == 1 )
			return EFI_NOT_READY;
	}

      Status = gST->ConIn->ReadKeyStroke( gST->ConIn, (EFI_INPUT_KEY*)pAmiKey);

	if ( ! EFI_ERROR( Status ) && FlushKeysAfterRead())//check for the FLUSH_KEYS_AFTER_READ token before flushing the keys
	{
		do
		{
			StatusFlush = gST->ConIn->ReadKeyStroke( gST->ConIn, &KeyFlush.Key );
		} while ( ! EFI_ERROR( StatusFlush ) );

	}

	return Status;
}
/**
    Function action timer

    @param Event VOID *Context

    @retval VOID

**/
VOID _ActionTimer( EFI_EVENT Event, VOID *Context )
{
	(*(UINT32 *)Context)++;
}
/**
    Function to install action timer

    @param action UINT64 TriggerTime

    @retval VOID

**/
VOID _ActionInstallTimer( ACTION_DATA *action, UINT64 TriggerTime )
{
	TimerCreateTimer( &action->TimerEvent, _ActionTimer, &action->TimerCount, TimerPeriodic, TriggerTime, EFI_TPL_CALLBACK );
}
/**
    Function to get mouse action

    @param MouseInfo 

    @retval STATUS

**/
//Function Which gets the Mouse action
EFI_STATUS _ActionReadMouse(MOUSE_INFO *MouseInfo)
{
	return MouseReadInfo(MouseInfo);
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
