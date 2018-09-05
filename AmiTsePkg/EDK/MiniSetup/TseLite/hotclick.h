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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/hotclick.h $
//
// $Author: Rajashakerg $
//
// $Revision: 1 $
//
// $Date: 6/29/11 5:01a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file hotclick.h
    Header file for hotclick code to handling

**/

#ifndef _HOTCLICK_H_
#define _HOTCLICK_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "object.h"
#include "action.h"


typedef struct _HOTCLICK_INFO
{
	UINT32 Xo, Yo, Xn, Yn;
}
HOTCLICK_INFO;

#define	HOTCLICK_MEMBER_VARIABLES	\
	HOTCLICK_INFO		HotClickData;	\
	UINT32			BasePage;

typedef struct _HOTCLICK_METHODS HOTCLICK_METHODS;

typedef struct _HOTCLICK_DATA
{
	HOTCLICK_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	HOTCLICK_MEMBER_VARIABLES

}
HOTCLICK_DATA;

#define	HOTCLICK_METHOD_FUNCTIONS

struct _HOTCLICK_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	HOTCLICK_METHOD_FUNCTIONS
};

extern HOTCLICK_METHODS gHotClick;

// Object Methods
EFI_STATUS HotClickCreate( HOTCLICK_DATA **object );
EFI_STATUS HotClickDestroy( HOTCLICK_DATA *object, BOOLEAN freeMem );
EFI_STATUS HotClickInitialize( HOTCLICK_DATA *object, VOID *data );
EFI_STATUS HotClickEmptyMethod( HOTCLICK_DATA *object );
EFI_STATUS HotClickHandleAction( HOTCLICK_DATA *object, ACTION_DATA *data );
EFI_STATUS HotClickSetCallback( HOTCLICK_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

#endif /* _HOTCLICK_H_ */


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
