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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/pcxc.c $
//
// $Author: Madhans $
//
// $Revision: 4 $
//
// $Date: 2/19/10 1:02p $
//
//*****************************************************************//
//*****************************************************************//

/** @file pcxc.c
    This file contains code for pcx image handling

**/

#include "pcx.h"


AMI_TRUECOLOR_PIXEL * ScreenBuffer = (AMI_TRUECOLOR_PIXEL *)NULL;

//Functions
BOOL PCX_Init(BYTE * pbyPCXImage, PCX_INFO *pPCX_INFO, DWORD dwPCXImageSize);
BOOL GetPCXDimensions( BYTE * pbyPCXImageData, UINT32 * pn32Width, UINT32 * pn32Height);
BOOL DecodePCX(BYTE * pbyPCXInputData, BYTE * pbyPCXOutputData, DWORD dwPCXImageSize);
void Decode_Line(DWORD *wCountX, PCX_INFO *pPCX_INFO, BYTE *pbOneLineBuffer);
void Output_Line(DWORD wTop, PCX_INFO *pPCX_INFO, BYTE *pbOneLineBuffer);

/**
    This routine is used to fill the PCX_INFO structure.

    @param pbyPCXImage Pointer that gives the memory location 
        where the PCX input data is available.
    @param pPCX_INFO Pointer to PCX_INFO structure
    @param dwPCXImageSize Size of the PCX Image

    @retval BOOL TRUE/FALSE

**/
BOOL PCX_Init(BYTE * pbyPCXImage, PCX_INFO *pPCX_INFO, DWORD dwPCXImageSize)
{
	PCX_HEADER *pPCX_HEADER = (PCX_HEADER *)pbyPCXImage;

	if(pPCX_HEADER->bVersion != 5 || pPCX_HEADER->bBitsPerPixel == 1 || pPCX_HEADER->bBitsPerPixel == 2 || pPCX_HEADER->bColorPlanes != 1)
		return FALSE;
	
	pPCX_INFO->wImageWidth		= pPCX_HEADER->wXmax - pPCX_HEADER->wXmin + 1;
	pPCX_INFO->wImageHeight		= pPCX_HEADER->wYmax - pPCX_HEADER->wYmin + 1;
	pPCX_INFO->wBytesPerLine	= pPCX_HEADER->wBytesPerLine;
	pPCX_INFO->bBitsPerPixel	= pPCX_HEADER->bBitsPerPixel;	
	pPCX_INFO->dwPCXImageSize	= dwPCXImageSize;
	pPCX_INFO->bpPCXData		= pbyPCXImage + 128;
	if(pPCX_HEADER->bBitsPerPixel == 8)			//256 color image palette at the end
		pPCX_INFO->bpPalette	= pbyPCXImage + dwPCXImageSize - 768;
	else if(pPCX_HEADER->bBitsPerPixel == 4)	//16 color image palette in the header
		pPCX_INFO->bpPalette	= pPCX_HEADER->bPalette;
	return TRUE;

}

/**
    Get the dimensions (Width and Height of the PCX Image).

    @param pbyPCXImageData Pointer to the buffer that contains the PCX data
    @param pun32Width Pointer to the UINT32 variable that will 
        receive the Width of the PCX Image
    @param pun32Height Pointer to the UINT32 variable that 
        will receive the Height of the PCX Image

    @retval BOOL TRUE/FALSE

**/
BOOL GetPCXDimensions( BYTE * pbyPCXImageData, UINT32 * pun32Width, UINT32 * pun32Height){
	BOOL bReturn = FALSE;
	WORD wWidth = 0;
	WORD wHeight= 0;
	wWidth  = *((WORD*)(pbyPCXImageData+8)) - *((WORD*)(pbyPCXImageData+4)) + 1;
	wHeight = *((WORD*)(pbyPCXImageData+10)) - *((WORD*)(pbyPCXImageData+6)) + 1;
	if( (wWidth > 0) && (wHeight > 0))
	{
		*pun32Width = (UINT32)wWidth;
		*pun32Height = (UINT32)wHeight;
		bReturn = TRUE;
	}
	return bReturn;

}

/**
    This routine will first check whether the PCX image is 16 
    color or 256 color and then decode the PCX image data one 
    line at a time and put the decoded data in a One Line Buffer.

    @param wCountX Pointer to the index of the image data after 128 byte header
    @param pPCX_INFO Pointer to PCX_INFO structure
    @param pbOneLineBuffer Pointer to the buffer that holds one line of decoded data.

    @retval VOID

**/
void Decode_Line(DWORD *wCountX, PCX_INFO *pPCX_INFO, BYTE *pbOneLineBuffer)
{
	WORD wOffset_of_buffer = 0;
	int nDataCount = 0;
	WORD wLength_of_buffer = 0;
	while(wLength_of_buffer < (pPCX_INFO->wImageWidth))
	{
		if((pPCX_INFO->bpPCXData[*wCountX] & 0xC0) >= 0xC0)
			nDataCount = (pPCX_INFO->bpPCXData[(*wCountX)++] & 0x3f);
		else
			nDataCount = 1;

		while(nDataCount--)
		{
			if(pPCX_INFO->bBitsPerPixel == 8)
			{
				pbOneLineBuffer[wOffset_of_buffer++] = pPCX_INFO->bpPCXData[*wCountX];
				(wLength_of_buffer)++;
			}
			else if(pPCX_INFO->bBitsPerPixel == 4)
			{
				pbOneLineBuffer[wOffset_of_buffer++] = ((pPCX_INFO->bpPCXData[*wCountX]) >> 4);
				pbOneLineBuffer[wOffset_of_buffer++] = ((pPCX_INFO->bpPCXData[*wCountX]) & 0x0f);
				(wLength_of_buffer) = (wLength_of_buffer)+2;
			}
		}
		(*wCountX)++;
	}


	if(pPCX_INFO->bBitsPerPixel == 8)
	{
		if((pPCX_INFO->wImageWidth) != (pPCX_INFO->wBytesPerLine))
			(*wCountX)++;
	}
	else if(pPCX_INFO->bBitsPerPixel == 4)
	{
		if(wLength_of_buffer < ((pPCX_INFO->wBytesPerLine)<<1))
			(*wCountX)++;
	}
}

/**
    This routine is used to map the decoded data to the palette 
    and fill the global Screen Buffer. 

    @param wTop This variable gives the position from where a 
        line should start in the Screen Buffer
    @param pPCX_INFO Pointer to PCX_INFO structure
    @param pbOneLineBuffer Pointer to the buffer that holds one line of decoded data.

    @retval VOID

**/
void Output_Line(DWORD wTop, PCX_INFO *pPCX_INFO, BYTE *pbOneLineBuffer)
{
	WORD wCountX;
	int nIndex;
	AMI_TRUECOLOR_PIXEL * pStartofLineInScreenBuffer = (AMI_TRUECOLOR_PIXEL *)ScreenBuffer + (wTop*(pPCX_INFO->wImageWidth));

		for(wCountX = 0; wCountX< pPCX_INFO->wImageWidth; wCountX++)		
		{
			nIndex = (3 * (int)(pbOneLineBuffer[wCountX]));
			pStartofLineInScreenBuffer->Red = pPCX_INFO->bpPalette[nIndex];
			pStartofLineInScreenBuffer->Green = pPCX_INFO->bpPalette[nIndex + 1];
			pStartofLineInScreenBuffer->Blue = pPCX_INFO->bpPalette[nIndex + 2];
			pStartofLineInScreenBuffer++;
		}


}

/**
    This routine will call functions to decode both 16 and 256 
    color PCX images of version 5 (with palette information) and 
    fill the Screen Buffer.

    @param pbyPCXInputData Pointer that gives the memory location 
        where the PCX input data is available.
    @param pbyPCXOutputData Pointer to the buffer that will 
        receive the decoded PCX image
    @param dwPCXImageSize Size of the PCX Image

    @retval BOOL TRUE/FALSE

**/
BOOL DecodePCX(BYTE * pbyPCXInputData, BYTE * pbyPCXOutputData, DWORD dwPCXImageSize)
{
	PCX_INFO pcx_info;
	BYTE  * bOneLineBuffer = NULL;
	DWORD wCountY = 0;
	DWORD wCountX = 0;

	bOneLineBuffer = EfiLibAllocatePool(sizeof(BYTE)*1280);
	if(bOneLineBuffer == NULL)
		return FALSE;

	ScreenBuffer = (AMI_TRUECOLOR_PIXEL *)pbyPCXOutputData;
	if(PCX_Init(pbyPCXInputData, &pcx_info, dwPCXImageSize))
	{
		for(wCountY = 0; wCountY < pcx_info.wImageHeight; wCountY++)
		{
			Decode_Line(&wCountX, &pcx_info, bOneLineBuffer);
			Output_Line(wCountY, &pcx_info, bOneLineBuffer);
		}
		MemFreePointer( (VOID **)&bOneLineBuffer);
		return TRUE;
	}
	else
	{
		MemFreePointer( (VOID **)&bOneLineBuffer);
		return FALSE;
	}
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
