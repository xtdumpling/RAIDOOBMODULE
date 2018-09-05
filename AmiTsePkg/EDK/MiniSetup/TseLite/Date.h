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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/Date.h $
//
// $Author: Premkumara $
//
// $Revision: 8 $
//
// $Date: 11/21/11 12:10p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file Date.h
    Header file for Date controls

**/
#ifndef _DATE_H_
#define	_DATE_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"
#endif

#include "control.h"

#define	DATE_MEMBER_VARIABLES	\
	UINT8	LabelFGColor;		\
	UINT8	NSelLabelFGColor;	\
	UINT8   LabelMargin;		\
	UINT8   Sel;				\
	UINT8	SelFGColor;			\
	UINT8	SelBGColor;			\
	UINT8	NSelFGColor;		\
	UINT16  Min;				\
    UINT16  Max;				\
	UINT8   Interval;

typedef struct _DATE_METHODS	DATE_METHODS;

typedef struct _DATE_DATA
{
	DATE_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	DATE_MEMBER_VARIABLES

}
DATE_DATA;


struct _DATE_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
};

extern DATE_METHODS gDate;

extern EFI_STATUS UefiGetTime(CONTROL_INFO *control, EFI_TIME *Tm);
extern EFI_STATUS UefiSetTime(CONTROL_INFO *control, EFI_TIME *Tm);

// Object Methods
EFI_STATUS DateCreate( DATE_DATA **object );
EFI_STATUS DateDestroy( DATE_DATA *object, BOOLEAN freeMem );
EFI_STATUS DateInitialize( DATE_DATA *object, VOID *data );
EFI_STATUS DateDraw( DATE_DATA *object );
EFI_STATUS DateHandleAction(DATE_DATA *object, ACTION_DATA *Data);
EFI_STATUS DateSetCallback( DATE_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );

// Control Methods
EFI_STATUS DateSetFocus( DATE_DATA *object, BOOLEAN focus);
EFI_STATUS DateSetPosition( DATE_DATA *object, UINT16 Left, UINT16 Top );
EFI_STATUS DateSetDimensions( DATE_DATA *object, UINT16 Width, UINT16 Height );
EFI_STATUS DateSetAttributes( DATE_DATA *object, UINT8 FGColor, UINT8 BGColor );
EFI_STATUS DateGetControlHeight(DATE_DATA *object, VOID *frame, UINT16 *height);

UINTN _DateGetWeekDay( EFI_TIME *tm );
VOID _DateDecrease( DATE_DATA *date );
VOID _DateIncrease( DATE_DATA *date );
VOID _DateUpdateHelp( DATE_DATA *date );

#define DATE_STYLE_MMDDYYYY		0
#define DATE_STYLE_YYYYDDMM		1
#endif /* _DATE_H_ */


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
