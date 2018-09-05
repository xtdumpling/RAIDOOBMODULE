//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/ResetButton.h $
//
// $Author: Rajashakerg $
//
// $Revision: 1 $
//
// $Date: 3/28/11 4:02p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file ResetButton.h
    Header file for ResetButton data

**/

#ifndef _RESET_BUTTON_H_
#define _RESET_BUTTON_H_

#include "minisetup.h"

#include "control.h"
#include "action.h"

typedef struct _RESET_BUTTON_METHODS	RESET_BUTTON_METHODS;

#define	RESET_BUTTON_MEMBER_VARIABLES \
  LABEL_MEMBER_VARIABLES

typedef struct _RESET_BUTTON_DATA
{
	RESET_BUTTON_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	RESET_BUTTON_MEMBER_VARIABLES
}
RESET_BUTTON_DATA;

#define RESET_BUTTON_METHOD_FUNCTIONS

struct _RESET_BUTTON_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
  RESET_BUTTON_METHOD_FUNCTIONS
};

extern RESET_BUTTON_METHODS gResetButton;

// Object Methods
EFI_STATUS ResetButtonCreate( VOID **object );
EFI_STATUS ResetButtonInitialize( VOID *object, VOID *data );
EFI_STATUS ResetButtonDraw( VOID *object );
EFI_STATUS ResetButtonHandleAction(VOID *object, ACTION_DATA *Data);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 2011, American Megatrends, Inc.        	  **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
