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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/numeric.h $
//
// $Author: Rajashakerg $
//
// $Revision: 11 $
//
// $Date: 4/05/12 7:18a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file numeric.h
    Header file for numeric control

**/
#ifndef _NUMERIC_H_
#define	_NUMERIC_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "control.h"
#include "action.h"
#include "MessageBox.h"

#define AMI_BASE_BIN 2
#define AMI_BASE_OCT 8
#define AMI_BASE_DEC 10
#define AMI_BASE_HEX 16
#define AMI_BASE_INT_DEC 32

#define NUMERIC_MIN_MAX_BASE	UINT64 
#define NUMERIC_VALUE_BASE		UINT64
#define	NUMERIC_MEMBER_VARIABLES	\
     UINT8	Base;					\
     NUMERIC_MIN_MAX_BASE MinValue;				\
	 NUMERIC_MIN_MAX_BASE MaxValue;				\
	 NUMERIC_VALUE_BASE	Value;					\
     NUMERIC_MIN_MAX_BASE Step;					\
 	 UINT8   Interval;


typedef struct _NUMERIC_METHODS	NUMERIC_METHODS;

typedef struct _NUMERIC_DATA
{
	NUMERIC_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	EDIT_MEMBER_VARIABLES
	NUMERIC_MEMBER_VARIABLES
}
NUMERIC_DATA;
#define	NUMERIC_METHOD_FUNCTIONS


struct _NUMERIC_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
	EDIT_METHOD_FUNCTIONS
	NUMERIC_METHOD_FUNCTIONS
};

extern NUMERIC_METHODS gNumeric;

// Object Methods
EFI_STATUS NumericCreate( NUMERIC_DATA **object );
EFI_STATUS NumericDestroy( NUMERIC_DATA *object, BOOLEAN freeMem );
EFI_STATUS NumericInitialize( NUMERIC_DATA *object, VOID *data );
EFI_STATUS NumericDraw( NUMERIC_DATA *object );
EFI_STATUS NumericHandleAction(NUMERIC_DATA *object, ACTION_DATA *Data);
EFI_STATUS NumericSetCallback( NUMERIC_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS NumericSetFocus( NUMERIC_DATA *object, BOOLEAN focus);
EFI_STATUS NumericSetPosition(NUMERIC_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS NumericSetDimensions(NUMERIC_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS NumericSetAttributes(NUMERIC_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS NumericGetControlHeight(NUMERIC_DATA *object, VOID *frame, UINT16 *height);
#endif /* _NUMERIC_H_ */


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
