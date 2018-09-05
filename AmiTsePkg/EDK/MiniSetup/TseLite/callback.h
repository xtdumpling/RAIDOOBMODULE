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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/callback.h $
//
// $Author: Madhans $
//
// $Revision: 5 $
//
// $Date: 2/19/10 1:04p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file callback.h
    Header file for code to handle callbacks

**/

#ifndef _CALLBACK_H_
#define	_CALLBACK_H_


#include "minisetup.h"  

#define	CALLBACK_TYPE_UNKNOWN		0
#define	CALLBACK_TYPE_SUBMENU		1
#define	CALLBACK_TYPE_VARIABLE		2
#define	CALLBACK_TYPE_MESSAGEBOX	3
#define	CALLBACK_TYPE_NVRAM			4
#define	CALLBACK_TYPE_DEFAULTS		5
#define	CALLBACK_TYPE_LABEL			6
#define	CALLBACK_TYPE_PASSWORD		7
#define	CALLBACK_TYPE_MENU		    8
#define	CALLBACK_TYPE_USER			-1

typedef struct _CALLBACK_HEADER
{
	UINT32	Type;
	UINT32	Length;
}
CALLBACK_HEADER;

typedef struct _CALLBACK_SUBMENU
{
	CALLBACK_HEADER	Header;
	UINT16			DestPage;
}
CALLBACK_SUBMENU;

typedef struct _CALLBACK_MENU
{
	CALLBACK_HEADER	Header;
	UINT16			DestPage;
}
CALLBACK_MENU;

typedef struct _CALLBACK_VARIABLE
{
	CALLBACK_HEADER	Header;
	UINT32			Variable;
	UINT32			Offset;
	UINT32			Length;
	VOID			*Data;
}
CALLBACK_VARIABLE;

typedef struct _CALLBACK_PASSWORD
{
	CALLBACK_HEADER	Header;
	UINT32			Variable;
	UINT32			Offset;
	UINT32			Length;
	VOID			*Data;
	BOOLEAN			Encoded;
}
CALLBACK_PASSWORD;

typedef struct _CALLBACK_MESSAGEBOX
{
	CALLBACK_HEADER	Header;
	UINTN			Result;
}
CALLBACK_MESSAGEBOX;

typedef struct _CALLBACK_NVRAM
{
	CALLBACK_HEADER	Header;
	// I can't remember what this type was for
}
CALLBACK_NVRAM;

#define	DEFAULT_TYPE_UNKNOWN	0
#define	DEFAULT_TYPE_FAILSAFE	1
#define	DEFAULT_TYPE_OPTIMAL	2

typedef struct _CALLBACK_DEFAULTS
{
	CALLBACK_HEADER	Header;
	UINT32			Type;
}
CALLBACK_DEFAULTS;

typedef struct _CALLBACK_LABEL
{
	CALLBACK_HEADER	Header;
	UINT32			Key;
}
CALLBACK_LABEL;

typedef struct _CALLBACK_USER
{
	CALLBACK_HEADER	Header;
	VOID			*UserData;
}
CALLBACK_USER;

#endif /* _CALLBACK_H_ */


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
