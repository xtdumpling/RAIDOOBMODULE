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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/mem.c $
//
// $Author: Arunsb $
//
// $Revision: 4 $
//
// $Date: 11/10/11 12:52a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file mem.c
    file contains code to handle the memory operations. 

**/

#include "minisetup.h"

/**
    function to free the pointers

    @param ptr 

    @retval void

**/
VOID MemFreePointer( VOID **ptr )
{
	if ( ( ptr == NULL ) || ( *ptr == NULL ) )
		return;

	gBS->FreePool( *ptr );
	*ptr = NULL;
}

/**
    Function to free the string pointers and zeroing its memory, used for strings
    Care should be taken String Length identification function is used

    @param ptr 

    @retval void

**/
VOID StringZeroFreeMemory (VOID **ptr)
{
	if ( (ptr == NULL) || (*ptr == NULL))
		return;

	MemSet (*ptr, (EfiStrLen ((CHAR16 *)*ptr) * sizeof (CHAR16)), 0);
	gBS->FreePool (*ptr);
	*ptr = NULL;
}
/**
    function to copy a memory

    @param dest VOID *src, UINTN size

    @retval void

**/
VOID MemCopy( VOID *dest, VOID *src, UINTN size )
{
	gBS->CopyMem( dest, src, size );
}

/**
    function to set the memory with the specified value

    @param buffer UINTN size, UINT8 value

    @retval void

**/
#if TSE_USE_EDK_LIBRARY
VOID MemSet( VOID *buffer, UINTN size, UINT8 value )
{
	gBS->SetMem( buffer, size, value );
}
#endif //#if TSE_USE_EDK_LIBRARY

/**
    function to reuse a allocated buffer

    @param oldBuffer UINTN oldSize, UINTN newSize

    @retval void

**/
VOID *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize )
{
	VOID *buffer;

	buffer = EfiLibAllocateZeroPool( newSize );
	if ( buffer == NULL )
		return buffer;

	if ( newSize < oldSize )
		oldSize = newSize;

	if ( oldBuffer != NULL )
		MemCopy( buffer, oldBuffer, oldSize );

	MemFreePointer( (VOID **)&oldBuffer );

	return buffer;
}

/**
    function to fill a UINT16 Buffer

    @param buffer UINTN bufferSize, UINT16 value

    @retval void

**/
VOID MemFillUINT16Buffer( VOID *buffer, UINTN bufferSize, UINT16 value )
{
	UINTN	Index;
	UINT16	*bufPtr = (UINT16 *)buffer;

	for ( Index = 0; Index < bufferSize; Index++ )
		*bufPtr++ = value;
}

/**
    function to compare the memory of two buffers.

    @param dest UINT8 *src, UINTN size

    @retval 0/1/-1

**/
#if TSE_USE_EDK_LIBRARY
INTN MemCmp( UINT8 *dest, UINT8 *src, UINTN size )
{
	UINTN i;

	for( i=0; i<size; i++)
	{
		if(dest[i] < src[i])
			return -1;
        if(dest[i] > src[i])
            return 1;
	}
	return 0;
}
#endif //#if TSE_USE_EDK_LIBRARY

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
