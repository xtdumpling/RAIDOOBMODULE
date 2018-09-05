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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/dobmpmgr.c $
//
// $Author: Rajashakerg $
//
// $Revision: 11 $
//
// $Date: 9/17/12 6:10a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file dobmpmgr.c
    file contains code that is  taken from graphics.c 
    to eliminnate library dependancy

**/

#include "minisetup.h"


//#if SETUP_BMP_LOGO_SUPPORT

/**
    Convert a *.BMP graphics image to a UGA blt buffer. 
    If a NULL UgaBlt buffer is passed in a UgaBlt buffer will 
    be allocated by this routine. If a UgaBlt buffer is passed 
    in it will be used if it is big enough.

    @param BmpImage Pointer to BMP file
    @param BmpImageSize Number of bytes in BmpImage
    @param UgaBlt Buffer containing UGA version of BmpImage.
    @param UgaBltSize Size of UgaBlt in bytes.
    @param PixelHeight Height of UgaBlt/BmpImage in pixels
    @param PixelWidth Width of UgaBlt/BmpImage in pixels

    @retval EFI_SUCCESS UgaBlt and UgaBltSize are returned. 
    @retval EFI_UNSUPPORTED BmpImage is not a valid *.BMP image
    @retval EFI_BUFFER_TOO_SMALL The passed in UgaBlt buffer is not big enough.
        UgaBltSize will contain the required size.
    @retval EFI_OUT_OF_RESOURCES No enough buffer to allocate

**/
EFI_STATUS
ConvertBmpToUgaBlt (
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **UgaBlt,
  IN OUT UINTN  *UgaBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth
  )
{
	UINT8             *Image;
	BMP_IMAGE_HEADER  *BmpHeader;
	BMP_COLOR_MAP     *BmpColorMap;
	EFI_UGA_PIXEL     *BltBuffer;
	EFI_UGA_PIXEL     *Blt;
	UINT64			  BltBufferSize;
	UINTN             Index;
	UINTN             Height;
	UINTN             Width;
	UINTN             ImageIndex;

	BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;
	if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') 
  		return EFI_UNSUPPORTED;
  
	if (BmpHeader->CompressionType != 0) 
		return EFI_UNSUPPORTED;
  
	// Calculate Color Map offset in the image.
	Image       = BmpImage;
	BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));

	// Calculate graphics image data address in the image
	Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;

	// Make sure to do x64 Mul
	BltBufferSize = MultU64x32((UINT64)BmpHeader->PixelWidth ,BmpHeader->PixelHeight);

	/// Check whether the Size exceeds the limit...
	//  4GB/PIXEL_SIZE becoz we need Resolution*PIXEL_SIZE for bltbuffer
	if ( BltBufferSize >= (FOUR_GB_LIMIT/sizeof (EFI_UGA_PIXEL))) {
		// The buffer size extends the limitation
		return EFI_UNSUPPORTED;
	}

	BltBufferSize = MultU64x32( BltBufferSize ,sizeof (EFI_UGA_PIXEL));

	if (*UgaBlt == NULL) 
	{
		*UgaBltSize = (UINTN)BltBufferSize;
		*UgaBlt     = EfiLibAllocatePool (*UgaBltSize);
		if (*UgaBlt == NULL) 
			return EFI_OUT_OF_RESOURCES;
    }
	else 
	{
		if (*UgaBltSize < (UINTN)BltBufferSize) 
		{
			*UgaBltSize = (UINTN)BltBufferSize;
			return EFI_BUFFER_TOO_SMALL;
		}
	}

	*PixelWidth   = BmpHeader->PixelWidth;
	*PixelHeight  = BmpHeader->PixelHeight;

    // Convert image from BMP to Blt buffer format
  
	BltBuffer = *UgaBlt;
	for (Height = 0; Height < BmpHeader->PixelHeight; Height++) 
	{
		Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
		for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) 
		{
			switch (BmpHeader->BitPerPixel) 
			{
				case 1: //the bitmap is monochrome		
					//for every bit
					for(Index = 8; Index ; Index--)
					{
						if(1<<(Index-1) & *Image)
						{
							Blt->Red    = BmpColorMap[1].Red;
							Blt->Green  = BmpColorMap[1].Green;
							Blt->Blue   = BmpColorMap[1].Blue;
						}
						else
						{
							Blt->Red    = BmpColorMap[0].Red;
							Blt->Green  = BmpColorMap[0].Green;
							Blt->Blue   = BmpColorMap[0].Blue;
						}
						if (Width < (BmpHeader->PixelWidth))
						{
							Blt++;
							Width++;
						}
					}
					Blt--;
					Width--;
				break;

				case 4:   // Convert BMP Palette to 24-bit color
        			Index       = (*Image) >> 4;
					Blt->Red    = BmpColorMap[Index].Red;
					Blt->Green  = BmpColorMap[Index].Green;
					Blt->Blue   = BmpColorMap[Index].Blue;
					if (Width < (BmpHeader->PixelWidth - 1)) 
					{
						Blt++;
						Width++;
						Index       = (*Image) & 0x0f;
						Blt->Red    = BmpColorMap[Index].Red;
						Blt->Green  = BmpColorMap[Index].Green;
						Blt->Blue   = BmpColorMap[Index].Blue;
					}
					break;

				case 8:  // Convert BMP Palette to 24-bit color
					Blt->Red    = BmpColorMap[*Image].Red;
					Blt->Green  = BmpColorMap[*Image].Green;
					Blt->Blue   = BmpColorMap[*Image].Blue;
					break;

				case 24:
					Blt->Blue   = *Image++;
					Blt->Green  = *Image++;
					Blt->Red    = *Image;
					break;
				case 32:									{
					if((BmpHeader->CompressionType)==0)
					{
						Blt->Blue   = *Image++;
						Blt->Green  = *Image++;
						Blt->Red    = *Image++;
					}
					else
					{
						return EFI_UNSUPPORTED;
					}
				}
				break;
				default:
					return EFI_UNSUPPORTED;
					break;
			};
		}
		ImageIndex = (UINTN) (Image - BmpHeader->ImageOffset);
		if ((ImageIndex % 4) != 0) // Bmp Image starts each row on a 32-bit boundary!
			Image = Image + (4 - (ImageIndex % 4));
	}
 	return EFI_SUCCESS;
}

//#endif //SETUP_BMP_LOGO_SUPPORT

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
