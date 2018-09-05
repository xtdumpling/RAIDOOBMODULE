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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/HiiLib.h $
//
// $Author: Madhans $
//
// $Revision: 5 $
//
// $Date: 9/20/10 6:46p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file HiiLib.h
    Hii related functions 

**/

#ifndef _HIILIB_H_
#define _HIILIB_H_

EFI_STATUS HiiInitializeProtocol( VOID );
CHAR16 *HiiGetString( VOID* handle, UINT16 token );
UINT16 HiiAddString( VOID* handle, CHAR16 *string );
UINTN TestPrintLength ( IN CHAR16   *String );

extern VOID * gHiiHandle;

#ifndef INVALID_HANDLE
#define	INVALID_HANDLE	((VOID*)(UINTN)-1)
#endif

#ifndef INVALID_TOKEN
#define	INVALID_TOKEN	((UINT16)-1)
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2010, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
