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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Time.h $
//
// $Author: Premkumara $
//
// $Revision: 8 $
//
// $Date: 11/21/11 10:56a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file Time.h
    Header file for code to handle Time control operations

**/
#ifndef _TIME_H_
#define	_TIME_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

#include "control.h"

typedef struct _TIME
{
	UINT8 Hour;
	UINT8 Min;
	UINT8 Sec;

}
TIME;

#define	TIME_MEMBER_VARIABLES	\
	UINT8	LabelFGColor;		\
	UINT8	NSelLabelFGColor;		\
	UINT8   LabelMargin;		\
	UINT8   Sel;				\
	UINT8	SelFGColor;			\
	UINT8	SelBGColor;			\
	UINT8	NSelFGColor;		\
	UINT16  Min;				\
    UINT16  Max;				\
	UINT8   Interval;			\
	EFI_TIME *EditTime;

typedef struct _TIME_METHODS	TIME_METHODS;

typedef struct _TIME_DATA
{
	TIME_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	TIME_MEMBER_VARIABLES

}
TIME_DATA;



struct _TIME_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
};

extern TIME_METHODS gTime;

// Object Methods
EFI_STATUS TimeCreate( TIME_DATA **object );
EFI_STATUS TimeDestroy( TIME_DATA *object, BOOLEAN freeMem );
EFI_STATUS TimeInitialize( TIME_DATA *object, VOID *data );
EFI_STATUS TimeDraw( TIME_DATA *object );
EFI_STATUS TimeHandleAction(TIME_DATA *object, ACTION_DATA *Data);
EFI_STATUS TimeSetCallback(  TIME_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS TimeSetFocus(TIME_DATA *object, BOOLEAN focus);
EFI_STATUS TimeSetPosition(TIME_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS TimeSetDimensions(TIME_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS TimeSetAttributes(TIME_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS TimeGetControlHeight(TIME_DATA *object, VOID *frame, UINT16 *height);
VOID _TimeUpdateHelp( TIME_DATA *time );
#endif /* _TIME_H_ */



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
