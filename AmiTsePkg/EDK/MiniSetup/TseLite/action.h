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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/action.h $
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
/** @file action.h
    Header file for code to handle different actions.

**/
#ifndef _ACTION_H_
#define	_ACTION_H_

#include "minisetup.h"
#include "commonoem.h"
#define	ACTION_TYPE_NULL	0
#define	ACTION_TYPE_KEY		1
#define	ACTION_TYPE_TIMER	2
#define ACTION_TYPE_MOUSE	3

// In terms of 100ns units
#define	ONE_SEC				10000000

typedef struct _ACTION_INPUT
{
	UINT16			Type;
	union
	{
		AMI_EFI_KEY_DATA	AmiKey;
		UINT32			TimerCount;
        MOUSE_INFO 		MouseInfo;
	}
	Data;
}
ACTION_INPUT;

#define	ACTION_MEMBER_VARIABLES	\
	ACTION_INPUT	Input;		\
	UINT32			TimerCount;	\
	EFI_EVENT		TimerEvent;

typedef struct _ACTION_METHODS ACTION_METHODS;

typedef struct _ACTION_DATA
{
	ACTION_METHODS	*Methods;

	ACTION_MEMBER_VARIABLES

}
ACTION_DATA;

typedef EFI_STATUS	(*ACTION_METHOD_CREATE)			( ACTION_DATA **object );
typedef EFI_STATUS	(*ACTION_METHOD_DESTROY)		( ACTION_DATA *object, BOOLEAN freeMem );
typedef EFI_STATUS	(*ACTION_METHOD_GET_ACTION)		( ACTION_DATA *object );
typedef EFI_STATUS	(*ACTION_METHOD_CLEAR_ACTION)	( ACTION_DATA *object );

#define	ACTION_METHOD_FUNCTIONS					\
	ACTION_METHOD_CREATE		Create;			\
	ACTION_METHOD_DESTROY		Destroy;		\
	ACTION_METHOD_GET_ACTION	GetAction;		\
	ACTION_METHOD_CLEAR_ACTION	ClearAction;

struct _ACTION_METHODS
{
	ACTION_METHOD_FUNCTIONS
};

extern ACTION_METHODS gAction;

// Action Methods
EFI_STATUS ActionCreate( ACTION_DATA **object );
EFI_STATUS ActionDestroy( ACTION_DATA *object, BOOLEAN freeMem );
EFI_STATUS ActionGetAction( ACTION_DATA *object );
EFI_STATUS ActionClearAction( ACTION_DATA *object );

//
// Internal functions
//

EFI_STATUS _ActionReadKey( AMI_EFI_KEY_DATA *pAmiKey, UINT64 Timeout );
VOID _ActionTimer( EFI_EVENT Event, VOID *Context );
VOID _ActionInstallTimer( ACTION_DATA *object, UINT64 TriggerTime );

#endif /* _ACTION_H_ */

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
