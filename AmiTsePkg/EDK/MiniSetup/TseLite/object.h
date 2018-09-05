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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/object.h $
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
/** @file object.h
    Header file for code to handle Objects

**/

#ifndef _OBJECT_H_
#define	_OBJECT_H_

//#include "Tiano.h" 
//#include "EfiDriverLib.h"
#include "minisetup.h"
#include "action.h"
#include "callback.h"

typedef	VOID	(*OBJECT_CALLBACK)	( VOID *container, VOID *object, VOID *cookie );

#define	OBJECT_MEMBER_VARIABLES	\
	OBJECT_CALLBACK	Callback;	\
	VOID			*Container;	\
	VOID			*Cookie;
	
typedef struct _OBJECT_METHODS OBJECT_METHODS;

typedef struct _OBJECT_DATA
{
	OBJECT_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
}
OBJECT_DATA;

typedef EFI_STATUS	(*OBJECT_METHOD_CREATE)			( VOID **object );
typedef EFI_STATUS	(*OBJECT_METHOD_DESTROY)		( VOID *object, BOOLEAN freeMem );
typedef EFI_STATUS	(*OBJECT_METHOD_INITIALIZE)		( VOID *object, VOID *data );
typedef	EFI_STATUS	(*OBJECT_METHOD_DRAW)			( VOID *object );
typedef EFI_STATUS	(*OBJECT_METHOD_HANDLE_ACTION)	( VOID *object, ACTION_DATA *Data );
typedef	EFI_STATUS	(*OBJECT_METHOD_SET_CALLBACK)	( VOID *object, VOID *container, OBJECT_CALLBACK callback, VOID *cookie );

#define	OBJECT_METHOD_FUNCTIONS					\
	OBJECT_METHOD_CREATE		Create;			\
	OBJECT_METHOD_DESTROY		Destroy;		\
	OBJECT_METHOD_INITIALIZE	Initialize;		\
	OBJECT_METHOD_DRAW			Draw;			\
	OBJECT_METHOD_HANDLE_ACTION	HandleAction;	\
	OBJECT_METHOD_SET_CALLBACK	SetCallback;

struct _OBJECT_METHODS
{
	OBJECT_METHOD_FUNCTIONS
};

extern OBJECT_METHODS gObject;

EFI_STATUS ObjectCreate( VOID **object );
EFI_STATUS ObjectDestroy( VOID *object, BOOLEAN freeMem );

EFI_STATUS ObjectInitialize( VOID *object, VOID *data );
EFI_STATUS ObjectDraw( VOID *object );
EFI_STATUS ObjectHandleAction( VOID *object, ACTION_DATA *Data );
EFI_STATUS ObjectSetCallback( VOID *object, VOID *container, OBJECT_CALLBACK callback, VOID *cookie );

#endif /* _OBJECT_H_ */


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
