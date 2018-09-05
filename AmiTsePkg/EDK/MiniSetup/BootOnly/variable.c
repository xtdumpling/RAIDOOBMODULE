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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/variable.c $
//
// $Author: Arunsb $
//
// $Revision: 6 $
//
// $Date: 9/28/11 10:48p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file variable.c
    This file contains code to handle bootonly lever variables operations.

**/

#include "minisetup.h"

extern UINTN gSetupCount;

/**
    function to get the NvRam Variable Name

    @param name EFI_GUID *guid, UINT32 *attributes, UINTN *size

    @retval Name String

**/
VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size )
{
	VOID *buffer = NULL;
	UINT32 attrib = 0;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	// NO GET Functionality is taken care by VarGetNvram()

	if ( attributes != NULL )
    {
        attrib = *attributes;
    }

	Status = UefiFormCallbackNVRead(name, guid, &attrib, size, &buffer);

	if(EFI_ERROR(Status))
	{
	    Status = gRT->GetVariable(
			name,
			guid,
			&attrib,
			size,
			buffer
			);

	    if ( Status != EFI_BUFFER_TOO_SMALL )
		    return buffer;

	    buffer = EfiLibAllocateZeroPool ( *size + 1);
	    if ( buffer == NULL )
		   return buffer;

	    Status = gRT->GetVariable(
			name,
			guid,
			&attrib,
			size,
			buffer
			);
	}
	    
	if ( EFI_ERROR( Status ) )
	    MemFreePointer( (VOID *)&buffer );

	if ( attributes != NULL )
		*attributes = attrib;

	return buffer;
}

/**
    function to Set the NvRam Variable Name

    @param name EFI_GUID *guid, UINT32 *attributes,
    @param buffer UINTN *size

    @retval status

**/
EFI_STATUS VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	// NO SET Functionality is taken care by VarSetNvram()

	Status = UefiFormCallbackNVWrite(name, guid, attributes, buffer, size);

	if(EFI_ERROR(Status ) )
		Status = gRT->SetVariable(
			name,
			guid,
			attributes,
			size,
			buffer
			);

	return(Status);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
