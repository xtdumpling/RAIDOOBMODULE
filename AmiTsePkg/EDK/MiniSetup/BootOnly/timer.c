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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/timer.c $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:02p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file timer.c
    This file contains code to handle the timer.

**/

#include "minisetup.h"

/**
    function to create a timer event and to set the timer

    @param Event EFI_EVENT_NOTIFY Callback,
    @param Context EFI_TIMER_DELAY Delay, UINT64 Trigger,
    @param CallBackTPL 

    @retval status

**/
EFI_STATUS TimerCreateTimer( EFI_EVENT *Event, EFI_EVENT_NOTIFY Callback,
		VOID *Context, EFI_TIMER_DELAY Delay, UINT64 Trigger, EFI_TPL CallBackTPL )
{
	EFI_STATUS Status;
	UINT32 EventType = EFI_EVENT_TIMER;

	if ( Callback != NULL )
		EventType |= EFI_EVENT_NOTIFY_SIGNAL;
	
	Status = gBS->CreateEvent(
			EventType,
			CallBackTPL,
			Callback,
			Context,
			Event
			);

	if ( EFI_ERROR( Status ) )
		return Status;

	Status = gBS->SetTimer( *Event, Delay, Trigger );
	if ( EFI_ERROR( Status ) )
		TimerStopTimer( Event );

	return Status;
}

/**
    function to stop the timer event.

    @param Event 

    @retval status

**/
EFI_STATUS TimerStopTimer( EFI_EVENT *Event )
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( ( Event == NULL ) || ( *Event == NULL ) )
		return Status;

	gBS->CloseEvent( *Event );
	*Event = NULL;

	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2007, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
