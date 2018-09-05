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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/ipcx.c $
//
// $Author: Madhans $
//
// $Revision: 6 $
//
// $Date: 2/19/10 1:02p $
//
//*****************************************************************//
//*****************************************************************//

/** @file ipcx.c
    This file contains code related to pcx image handling

**/

#include "minisetup.h"

#include "MyDefs.h"


BOOL DecodePCX(BYTE * pbyPCXInputData, BYTE * pbyPCXOutputData, DWORD dwPCXImageSize); //Function implemented in PCX_ADV.c
BOOL GetPCXDimensions( BYTE * pbyPCXImageData, UINT32 * pun32Width, UINT32 * pun32Height);	//Function implemented in PCX_ADV.c

/**
    Decode the PCX Image and store the output in UGA Format

    @param PCXImage Pointer to the memory that has the PCX 
        Image that need to be decoded.
    @param PCXImageSize Size of the PCX Image
    @param UgaBlt Two cases
        Case 1: Memory is expected to be allocated by the callee.
        Address provided as input should be NULL.
        pointer that will receive the address of the UGA buffer. 
        The buffer is allocated by callee.	
        This buffer need to be freed by the caller. 		
        Case 2: Memory is allocated by the caller
    @param UgaBltSize Pointer that stores Size of UgaBlt in bytes. 
        This will be treated as an input. This should have value ZERO for Case 2 above.
    @param PixelHeight Height of UgaBlt/PCX Image in pixels
    @param PixelWidth Width of UgaBlt/PCX Image in pixels

    @retval EFI_STATUS

**/
EFI_STATUS ConvertPCXToUgaBlt ( IN VOID *PCXImage, IN UINT32 PCXImageSize, IN OUT VOID **UgaBlt, IN OUT UINT32 *UgaBltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth){
	BOOL bStatus = FALSE;
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINT64 unBltBufferSize = 0;
	bStatus = GetPCXDimensions(PCXImage, PixelWidth, PixelHeight);
	if(bStatus)
	{
		// Make sure to do x64 Mul
		unBltBufferSize = MultU64x32((UINT64)(*PixelWidth), (*PixelHeight));

		/// Check whether the Size exceeds the limit...
		//  4GB/PIXEL_SIZE becoz we need Resolution*PIXEL_SIZE for bltbuffer
		if ( unBltBufferSize >= (FOUR_GB_LIMIT/sizeof (EFI_UGA_PIXEL))) {
			// The buffer size extends the limitation
			return EFI_UNSUPPORTED;
		}

		unBltBufferSize = MultU64x32( unBltBufferSize ,sizeof (EFI_UGA_PIXEL));

		if(NULL!=*UgaBlt)
		{
			if(*UgaBltSize<(UINT32)unBltBufferSize)
			{
				*UgaBltSize = (UINT32)unBltBufferSize;
				Status = EFI_BUFFER_TOO_SMALL;
				bStatus = FALSE;
			}
		}
		else
		{
			*UgaBltSize = (UINT32)unBltBufferSize;
			*UgaBlt = EfiLibAllocatePool((UINT32)unBltBufferSize);
			if(NULL == *UgaBlt)
			{
				Status = EFI_OUT_OF_RESOURCES;
				bStatus = FALSE;
			}
		}
		if(bStatus)
		{
			MemSet(*UgaBlt,*UgaBltSize,0);
			bStatus = DecodePCX(PCXImage, *UgaBlt, PCXImageSize);
			if(bStatus)
				Status = EFI_SUCCESS;
		}
	}
	return Status;
}
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
