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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/mem.h $
//
// $Author: Arunsb $
//
// $Revision: 4 $
//
// $Date: 11/10/11 12:46a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file mem.h
    memory utility functions

**/

#ifndef _MEM_H_
#define _MEM_H_

VOID MemFreePointer (VOID **ptr);
VOID StringZeroFreeMemory (VOID **ptr);
VOID MemCopy( VOID *dest, VOID *src, UINTN size );
VOID MemSet( VOID *buffer, UINTN size, UINT8 value );
#if !TSE_USE_EDK_LIBRARY
INTN MemCmp( VOID *dest, VOID *src, UINTN size );
#endif
VOID *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );
VOID MemFillUINT16Buffer( VOID *buffer, UINTN bufferSize, UINT16 value );

#endif /* _MEM_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2011, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
