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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/png.c $
//
// $Author: Rajashakerg $
//
// $Revision: 2 $
//
// $Date: 6/20/11 1:20p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file png.c
    This file contains code for png image handling

**/

#include "minisetup.h"
#include "png.h"
#include EFI_PROTOCOL_DEFINITION(GraphicsOutput) 
//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define MakeDword(a,b,c,d) ((a)*0x1000000+(b)*0x10000+(c)*0x100+(d))
#define IHDR MakeDword('I','H','D','R')
#define IDAT MakeDword('I','D','A','T')
#define PLTE MakeDword('P','L','T','E')
#define IEND MakeDword('I','E','N','D')
#define pHYs MakeDword('p','H','y','s')
#define tRNS MakeDword('t','R','N','S')
#define gAMA MakeDword('g','A','M','A')
#define PNG_SIGNATURE_LENGTH 8
#define CHUNK_LENGTH 4
#define CHUNK_TYPE 4
#define CRC_LENGTH 4

static unsigned char *ImageBuffer = (unsigned char *)NULL;


/**
    To get decimal values

    @param const unsigned char dat[]

    @retval unsigned int

**/
unsigned int PngGetUnsignedInt(const unsigned char dat[4])
{
	return (unsigned)dat[3] + (unsigned)dat[2] * 0x100 + (unsigned)dat[1] * 0x10000 + (unsigned)dat[0] * 0x1000000;
}

/**
    To find the PNG image size

    @param VOID

    @retval unsigned int

**/
unsigned int FindImageSize()
{
	unsigned char *dwBuf = (unsigned char *)NULL;
	UINT32 i = 0;
	unsigned int ImageSize = 0;
	unsigned int length = 0;
	unsigned int chunkType = 0;

	ImageBuffer = ImageBuffer + PNG_SIGNATURE_LENGTH;
	dwBuf = (unsigned char *)EfiLibAllocateZeroPool(CHUNK_LENGTH);
	if (dwBuf == NULL) {
		goto ERREND;
	}
	ImageSize = PNG_SIGNATURE_LENGTH;
	while (chunkType != IEND)										//IEND is end of PNG image file 
	{
		for (i = 0; i < CHUNK_LENGTH; i++) {
			dwBuf[i] = ImageBuffer[i];
		}
		ImageSize = ImageSize + CHUNK_LENGTH;					//Adding 4 byte for Chunk Length to move the pointer to Chunk type field
		ImageBuffer = ImageBuffer + CHUNK_LENGTH;

		length = PngGetUnsignedInt(dwBuf);

		for (i = 0; i < CHUNK_TYPE; i++) {
			dwBuf[i] = ImageBuffer[i];
		}
		ImageSize = ImageSize + CHUNK_TYPE;						//Adding 4 byte for Chunk type to move the pointer to Chunk data field
		ImageBuffer = ImageBuffer + CHUNK_TYPE;

		chunkType = PngGetUnsignedInt(dwBuf);
		ImageSize = ImageSize + length + CRC_LENGTH;			//Adding Chunk data length and CRC length to move the pointer to next Chunk
		ImageBuffer = ImageBuffer + length + CRC_LENGTH;
	}
	if (dwBuf) {
		MemFreePointer((void**)&dwBuf);
	}

	return ImageSize;
ERREND:
	return 0;
}

/**
    To Store the header values in the PngHeader variables

    @param unsigned char dat[]

    @retval VOID

**/
void Header_Decode(unsigned char dat[])
{
	gPng_Info.hdr.width = PngGetUnsignedInt(dat);
	gPng_Info.hdr.height = PngGetUnsignedInt(dat+4);
	gPng_Info.hdr.bitDepth = dat[8];								// Storing IHDR values in PngHeader structure
	gPng_Info.hdr.colorType = dat[9];
	gPng_Info.hdr.compressionMethod = dat[10];
	gPng_Info.hdr.filterMethod = dat[11];
	gPng_Info.hdr.interlaceMethod = dat[12];
}

/**
    To store the PLTE data values in PngPalette variables

    @param unsigned length,unsigned char dat[]

    @retval int

**/
EFI_STATUS Palette_Decode(unsigned length, unsigned char dat[])
{
	if (length%3 != 0)							//PLTE Length should be divisable by three
	{
		return EFI_UNSUPPORTED;
	}

	if (gPng_Info.plt.entry != NULL)
	{
		MemFreePointer((void**)&gPng_Info.plt.entry);
		gPng_Info.plt.nEntry = 0;
	}

	if (length > 0)
	{
		gPng_Info.plt.entry = (unsigned char *)EfiLibAllocateZeroPool(length);
		if (gPng_Info.plt.entry != NULL)
		{
			unsigned int i;
			gPng_Info.plt.nEntry = length/3;
			for (i = 0; i < length; i++)
			{
				gPng_Info.plt.entry[i] = dat[i];
			}
		}
	}

	return EFI_SUCCESS;
}

/**
    To store the tRNS data values in PngPalette variables

    @param unsigned int length,unsigned char dat[],unsigned int colorType

    @retval int

**/
EFI_STATUS Transparency_Decode(unsigned int length, unsigned char dat[], unsigned int colorType)
{
	unsigned int i;
	switch (colorType)						//See PNG specification for the color Types
	{
	case 0:
		if (length >= 2)
		{
			 gPng_Info.trns.col[0] = (unsigned int)dat[0] * 256 + (unsigned int)dat[1];
			return EFI_SUCCESS;
		}
		break;
	case 2:
		if (length >= 6)
		{
			 gPng_Info.trns.col[0] = (unsigned int)dat[0] * 256 + (unsigned int)dat[1];
			 gPng_Info.trns.col[1] = (unsigned int)dat[2] * 256 + (unsigned int)dat[3];
			gPng_Info.trns.col[2] = (unsigned int)dat[4] * 256 + (unsigned int)dat[5];
			return EFI_SUCCESS;
		}
		break;
	case 3:
		for (i=0; i < 3 && i < length; i++)
		{
			gPng_Info.trns.col[i] = dat[i];
		}
		return EFI_SUCCESS;
	}
	return EFI_ABORTED;
}

/**
    to initialize the PngImageDecoder member variables

    @param VOID

    @retval VOID

**/
void Initialize(void)
{
	gPng_Info.gamma = gamma_default;
	gPng_Info.trns.col[0] = 0x7fffffff;
	gPng_Info.trns.col[1] = 0x7fffffff;
	gPng_Info.trns.col[2] = 0x7fffffff;
}

/**
    To Read all the chunk data

    @param unsigned &length,unsigned char *&buf,unsigned &chunkType,unsigned &crc

    @retval int

**/
EFI_STATUS ReadChunk(unsigned *length, unsigned char **buf, unsigned *chunkType, unsigned *crc)
{
	unsigned char *dwBuf = (unsigned char *)NULL;
	UINT32 i = 0;
	EFI_STATUS Status = EFI_SUCCESS;

	dwBuf = (unsigned char *)EfiLibAllocateZeroPool(4);
	if (dwBuf == NULL) {
		Status = EFI_OUT_OF_RESOURCES;
		goto EndRead;
	}
	for (i = 0; i < 4; i++) {
		dwBuf[i] = ImageBuffer[i];
	}
	ImageBuffer = ImageBuffer + CHUNK_LENGTH;					//Adding 4 byte for Chunk Length to move the pointer to Chunk type field

	*length = PngGetUnsignedInt(dwBuf);

	for (i = 0; i < 4; i++) {
		dwBuf[i] = ImageBuffer[i];
	}
	ImageBuffer = ImageBuffer + CHUNK_TYPE;						//Adding 4 byte for Chunk type to move the pointer to Chunk data field

	*chunkType = PngGetUnsignedInt(dwBuf);

	if(*length > 0)
	{
		*buf = (unsigned char *)EfiLibAllocateZeroPool(*length);
		if (*buf == NULL) {
			Status = EFI_OUT_OF_RESOURCES;
			goto EndRead;
		}
		for (i = 0; i < *length; i++) {
			(*buf)[i] = ImageBuffer[i];
		}
	}
	else
	{
		*buf = (unsigned char *)NULL;
	}
	ImageBuffer = ImageBuffer + *length;							//Adding Chunk data length to move the pointer to CRC field

	for (i = 0; i < 4; i++) {
		dwBuf[i] = ImageBuffer[i];
	}
	ImageBuffer = ImageBuffer + CRC_LENGTH;						//Adding 4 byte for CRC length to move the pointer to next Chunk

	*crc = PngGetUnsignedInt(dwBuf);

EndRead:

	if (dwBuf) {												//Fix for Fortify issue - Memory Leak
		MemFreePointer((void**)&dwBuf);
	}

	return Status;
}

/**
    Read all the PNG chunks and do convert the png format to RGB format.

    @param unsigned char *PNGImage,UINT32 PNGImageSize

    @retval int

**/
EFI_STATUS Decode(unsigned char *PNGImage, UINT32 PNGImageSize)
{
	unsigned char *datBuf = (unsigned char *)NULL;
	unsigned datBufUsed;
	unsigned char *buf;
	unsigned length,chunkType,crc;
	
	if (PNGImage)
	{		
		ImageBuffer = PNGImage;
		PNGImageSize = FindImageSize(); //Find the png image size if the PNGImageSize is zero

		datBufUsed = 0;
		datBuf = (unsigned char *)EfiLibAllocateZeroPool(PNGImageSize);
		if (datBuf == NULL) {
			goto ERREND;
		}

		ImageBuffer = PNGImage + PNG_SIGNATURE_LENGTH; //Adding 8 bytes to move the pointer to Chunk length 
		while (ReadChunk(&length,&buf,&chunkType,&crc) == EFI_SUCCESS && chunkType != IEND) //Read the Chunk data upto IEND(End of the PNG image) 
		{
			if(buf == NULL){
				break; //If the buffer is invalid, break from the loop
			}
			switch (chunkType)
			{
				case IHDR:
					if (length >= 13)
					{
						Header_Decode(buf);	//Stores the IHDR values in the PngHeader structure
					}
					MemFreePointer((void**)&buf);
					break;
				case PLTE:
					if (Palette_Decode(length,buf)) //Stores the PLTE values in the PngPalette structure
					{
						MemFreePointer((void**)&buf);
						goto ERREND;
					}
					MemFreePointer((void**)&buf);
					break;
				case tRNS:
					if (Transparency_Decode(length,buf,gPng_Info.hdr.colorType)) //Stores the tRNS values in the PngTransparency structure
					{
						MemFreePointer((void**)&buf);
						goto ERREND;
					}
					MemFreePointer((void**)&buf);
					break;
				case gAMA:
					if(length >= 4)
					{
						gPng_Info.gamma = PngGetUnsignedInt(buf); //Stores the gAMA chunk values
						MemFreePointer((void**)&buf);
					}
					break;
				case IDAT:
					if (buf != NULL)
					{
						unsigned i;
						for (i = 0; i < length; i++)
						{
							datBuf[datBufUsed+i] = buf[i]; //Stores the IDAT chunk value in datbuf[]
						}
						datBufUsed += length; //datBufUsed used to store consecutive IDAT chunk sequentialy
						 MemFreePointer((void**)&buf);
					}
					break;
				default:
					MemFreePointer((void**)&buf); //Modified to remove the fortify warning
					break;
			}
		}

		if (PrepareOutput() == EFI_SUCCESS) // Check the combination of Color type and bit debth
		{
			//PngUncompressor uncompressor;
			//uncompressor.output = this;
			if (Uncompress(datBufUsed,datBuf)) { //Convert the PNG image data into the RGB format in PngImageDecoder.rgba variable
				goto ERREND;
			}
		} else {
			goto ERREND;
		}

		MemFreePointer((void**)&datBuf);
		return EFI_SUCCESS;
	}

ERREND:
	if (datBuf) {
		MemFreePointer((void**)&datBuf);
	}
	return EFI_ABORTED;
}

/**

    @param unsigned int ua,unsigned int ub,unsigned int uc

    @retval unsigned char

**/
unsigned char Paeth(unsigned int ua,unsigned int ub,unsigned int uc)
{
	int a,b,c,p,pa,pb,pc;

	a = (int)ua;
	b = (int)ub;
	c = (int)uc;

	p = a + b - c;
	pa = (p>a ? p-a : a-p);						//Paeth filtering as per the PNG Specification
	pb = (p>b ? p-b : b-p);
	pc = (p>c ? p-c : c-p);

	if (pa <= pb && pa <= pc)
	{
		return (unsigned char)a;				//Fix for Fortify issue - Type Mismatch : Signed to Unsigned
	}
	else if(pb <= pc)
	{
		return (unsigned char)b;				//Fix for Fortify issue - Type Mismatch : Signed to Unsigned
	}
	else
	{
		return (unsigned char)c;				//Fix for Fortify issue - Type Mismatch : Signed to Unsigned
	}
}

/**

    @param unsigned char curLine[],unsigned char prvLine[],int x,int y,int unitLng,int filter

    @retval VOID

**/
void Filter8(unsigned char curLine[],unsigned char prvLine[],int x,int y,int unitLng,int filter)
{
	int i;
	switch (filter)								//filtering algorithm
	{
	case 1:
		if (x > 0)
		{
			for (i = 0; i < unitLng; i++)
			{
				curLine[ x * unitLng + i] = curLine[ x * unitLng + i] + curLine[ x * unitLng + i - unitLng];
			}
		}
		break;
	case 2:
		if (y > 0)
		{
			for (i = 0; i < unitLng; i++)
			{
				curLine[x * unitLng + i] = curLine[x * unitLng + i] + prvLine[ x * unitLng + i];
			}
		}
		break;
	case 3:
		for (i = 0; i < unitLng; i++)
		{
			unsigned int a;
			a = (x > 0 ? curLine[x * unitLng + i - unitLng] : 0);
			a += (y > 0 ? prvLine[x * unitLng + i] : 0);
			curLine[x * unitLng + i] += (unsigned char)a / 2;
		}
		break;
	case 4:
		for (i = 0; i < unitLng; i++)
		{
			unsigned int a,b,c;
			a = (x > 0 ? curLine[x * unitLng + i - unitLng] : 0);
			b = (y > 0 ? prvLine[x * unitLng + i] : 0);
			c = ((x > 0 && y > 0) ? prvLine[x * unitLng - unitLng + i] : 0);
			curLine[x * unitLng + i] = curLine[x * unitLng + i] + Paeth(a,b,c);
		}
		break;
	}
}

/**

    @param VOID

    @retval VOID

**/
void ShiftTwoLineBuf(void)
{
	if (gPng_Info.twoLineBuf8 != NULL)
	{
		unsigned char *swap;
		swap = gPng_Info.curLine8;
		gPng_Info.curLine8 = gPng_Info.prvLine8;
		gPng_Info.prvLine8 = swap;
	}
}

/**
    To validate the colorType and bitDepth combination and prepare the output data

    @param VOID

    @retval int

**/
EFI_STATUS PrepareOutput(void)
{
	int supported;
	unsigned int twoLineBufLngPerLine = 0;

	supported = 0;
	switch ( gPng_Info.hdr.colorType)
	{
	case 0:									// Greyscale
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
		case 1:
			supported = 1;
			break;
		case 2:
		case 4:
		case 16:
			break;
		}
		break;
	case 2:									// Truecolor
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
		case 16:
			supported = 1;
			break;
		}
		break;
	case 3:									// Indexed-color
		switch (gPng_Info.hdr.bitDepth)
		{
		case 4:
		case 8:
			supported = 1;
			break;
		case 1:
		case 2:
			break;
		}
		break;
	case 4:									// Greyscale with alpha
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
			supported = 1;
			break;
		case 16:
			break;
		}
		break;
	case 6:									// Truecolor with alpha
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
			supported = 1;
			break;
		case 16:
			break;
		}
		break;
	}

	if (supported == 0)
	{
		return EFI_UNSUPPORTED;
	}

	gPng_Info.wid = gPng_Info.hdr.width;
	gPng_Info.hei =gPng_Info.hdr.height;
	if(gPng_Info.autoDeleteRgbaBuffer == 1 && gPng_Info.rgba != NULL)
	{
		MemFreePointer((void**)&gPng_Info.rgba);
		gPng_Info.rgba = (unsigned char *)NULL;
	}
	gPng_Info.rgba = (unsigned char *)EfiLibAllocateZeroPool(gPng_Info.wid * gPng_Info.hei * 4);
	gPng_Info.x = -1;
	gPng_Info.y = 0;
	gPng_Info.filter = 0;
	gPng_Info.inLineCount = 0;
	gPng_Info.inPixelCount = 0;
	gPng_Info.firstByte = 1;
	gPng_Info.index = 0;
	gPng_Info.interlacePass = 1;

	if (gPng_Info.twoLineBuf8 != NULL)
	{
		MemFreePointer((void**)&gPng_Info.twoLineBuf8);
	}
	
	
	switch (gPng_Info.hdr.colorType)
	{
	case 0:											// Greyscale
		switch(gPng_Info.hdr.bitDepth)
		{
		case 1:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 7) /8;
			break;
		case 2:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 3) / 4;
			break;
		case 4:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 1) / 2;
			break;
		case 8:
			twoLineBufLngPerLine = gPng_Info.hdr.width;
			break;
		case 16:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 2;
			break;
		}
		break;
	case 2:											// Truecolor
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 3;
			break;
		case 16:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 6;
			break;
		}
		break;
	case 3:											// Indexed-color
		switch (gPng_Info.hdr.bitDepth)
		{
		case 1:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 7) / 8;
			break;
		case 2:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 3) / 4;
			break;
		case 4:
			twoLineBufLngPerLine = (gPng_Info.hdr.width + 1) / 2;
			break;
		case 8:
			twoLineBufLngPerLine = gPng_Info.hdr.width;
			break;
		}
		break;
	case 4:											// Greyscale with alpha
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 2;
			break;
		case 16:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 4;
			break;
		}
		break;
	case 6:											// Truecolor with alpha
		switch (gPng_Info.hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 4;
			break;
		case 16:
			twoLineBufLngPerLine = gPng_Info.hdr.width * 8;
			break;
		}
		break;
	}

	gPng_Info.twoLineBuf8 = (unsigned char *) EfiLibAllocateZeroPool(twoLineBufLngPerLine*2);
	gPng_Info.curLine8 = gPng_Info.twoLineBuf8;
	gPng_Info.prvLine8 = gPng_Info.twoLineBuf8 + twoLineBufLngPerLine;

	return EFI_SUCCESS;
}

/**
    To create the output values in the RGB format.

    @param unsigned char dat

    @retval int

**/
EFI_STATUS Output(unsigned char dat)
{
	unsigned int i;
	unsigned int colIdx;
	unsigned int interlaceWid,interlaceHei,interlaceX,interlaceY;

	if (gPng_Info.y >= gPng_Info.hei || (gPng_Info.rgba == NULL))
	{
		return EFI_UNSUPPORTED;
	}

	if (gPng_Info.x == -1)					// First byte is filter type for the line.  
	{
		gPng_Info.filter = dat;				// See PNG Specification for filtering
		gPng_Info.inLineCount = 0;
		gPng_Info.inPixelCount = 0;
		gPng_Info.x++;
		return EFI_SUCCESS;
	}
	else 
	{
		switch (gPng_Info.hdr.interlaceMethod)
		{
		case 0:										// Non-Interlace
			switch (gPng_Info.hdr.colorType)					// See PNG Specification for colortype and values
			{
			case 0:									// Grayscale
				switch (gPng_Info.hdr.bitDepth)
				{
				case 1:
					gPng_Info.curLine8[gPng_Info.x/8] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x/8, gPng_Info.y, 1, gPng_Info.filter);

					for (i = 0; i < 8 && gPng_Info.x < gPng_Info.wid; i++)
					{
						colIdx = (gPng_Info.curLine8[gPng_Info.x/8] >> (7 - i))&1;
						colIdx = (colIdx << 1) + colIdx;
						colIdx = (colIdx << 2) + colIdx;
						colIdx = (colIdx << 4) + colIdx;

						gPng_Info.rgba[gPng_Info.index] = (char)colIdx;
						gPng_Info.rgba[gPng_Info.index+1] = (char)colIdx;
						gPng_Info.rgba[gPng_Info.index+2] = (char)colIdx;
						gPng_Info.rgba[gPng_Info.index+3] = 0;
						gPng_Info.x++;
						gPng_Info.index += 4;
					}
					break;

				case 8:
					gPng_Info.curLine8[gPng_Info.x] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 1, gPng_Info.filter);

					gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.x];
					gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.x];
					gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.x];
					if (gPng_Info.curLine8[gPng_Info.x] == gPng_Info.trns.col[0] || gPng_Info.curLine8[gPng_Info.x] == gPng_Info.trns.col[1] || gPng_Info.curLine8[gPng_Info.x] == gPng_Info.trns.col[2])
					{
						gPng_Info.rgba[gPng_Info.index+3] = 0;
					}
					else
					{
						gPng_Info.rgba[gPng_Info.index+3] = 255;
					}

					gPng_Info.x++;
					gPng_Info.index += 4;
					break;
				}
				break;

			case 2:													//True color
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 3)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x,gPng_Info.y, 3, gPng_Info.filter);
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+2];

						if (gPng_Info.curLine8[gPng_Info.inLineCount] == gPng_Info.trns.col[0] && 
						   gPng_Info.curLine8[gPng_Info.inLineCount+1] == gPng_Info.trns.col[1] && 
						   gPng_Info.curLine8[gPng_Info.inLineCount+2] == gPng_Info.trns.col[2])
						{
							gPng_Info.rgba[gPng_Info.index+3] = 0;
						}
						else
						{
							gPng_Info.rgba[gPng_Info.index+3] = 255;
						}

						gPng_Info.x++;
						gPng_Info.index += 4;
						gPng_Info.inLineCount += 3;
						gPng_Info.inPixelCount = 0;
					}
					break;
				case 16:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 6)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 6, gPng_Info.filter);
						gPng_Info.rgba[gPng_Info.index] =gPng_Info. curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+2];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+4];

						gPng_Info.r = gPng_Info.curLine8[gPng_Info.inLineCount] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.g = gPng_Info.curLine8[gPng_Info.inLineCount+2] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+3];
						gPng_Info.b = gPng_Info.curLine8[gPng_Info.inLineCount+4] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+5];
						if (gPng_Info.r == gPng_Info.trns.col[0] && gPng_Info.g == gPng_Info.trns.col[1] && gPng_Info.b == gPng_Info.trns.col[2])
						{
							gPng_Info.rgba[gPng_Info.index+3] = 0;
						}
						else
						{
							gPng_Info.rgba[gPng_Info.index+3] = 255;
						}
						gPng_Info.x++;
						gPng_Info.index += 4;
						gPng_Info.inLineCount += 6;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
				break;

			case 3:													// Indexed color
				switch (gPng_Info.hdr.bitDepth)
				{
				case 4:
					gPng_Info.curLine8[gPng_Info.x/2] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x/2, gPng_Info.y, 1, gPng_Info.filter);

					for (i = 0; i<2 && gPng_Info.x < gPng_Info.wid; i++)
					{
						colIdx = (gPng_Info.curLine8[gPng_Info.x/2] >> ((1 - i) * 4))&0x0f;

						if (colIdx < gPng_Info.plt.nEntry)
						{
							gPng_Info.rgba[gPng_Info.index] = gPng_Info.plt.entry[colIdx*3];
							gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.plt.entry[colIdx*3+1];
							gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.plt.entry[colIdx*3+2];
							if (colIdx == gPng_Info.trns.col[0] || colIdx == gPng_Info.trns.col[1] || colIdx == gPng_Info.trns.col[2])
							{
								gPng_Info.rgba[gPng_Info.index+3] = 0;
							}
							else
							{
								gPng_Info.rgba[gPng_Info.index+3] = 255;
							}
						}
						gPng_Info.x++;
						gPng_Info.index += 4;
					}
					break;

				case 8:
					gPng_Info.curLine8[gPng_Info.x] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 1, gPng_Info.filter);
					colIdx = gPng_Info.curLine8[gPng_Info.x];

					if (colIdx < gPng_Info.plt.nEntry)
					{
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.plt.entry[colIdx*3];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.plt.entry[colIdx*3+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.plt.entry[colIdx*3+2];
						if (colIdx == gPng_Info.trns.col[0] || colIdx == gPng_Info.trns.col[1] || colIdx == gPng_Info.trns.col[2])
						{
							gPng_Info.rgba[gPng_Info.index+3] = 0;
						}
						else
						{
							gPng_Info.rgba[gPng_Info.index+3] = 255;
						}
					}
					gPng_Info.x++;
					gPng_Info.index += 4;
					break;
				}
				break;

			case 4:												// Greyscale with alpha
				switch(gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount + gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 2)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 2, gPng_Info.filter);
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+3] = gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.index += 4;
						gPng_Info.x++;
						gPng_Info.inLineCount += 2;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
				break;

			case 6:										// Truecolor with alpha
				switch(gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 4)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8,gPng_Info.x, gPng_Info.y, 4, gPng_Info.filter);
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+2];
						gPng_Info.rgba[gPng_Info.index+3] = gPng_Info.curLine8[gPng_Info.inLineCount+3];
						gPng_Info.index += 4;
						gPng_Info.x++;
						gPng_Info.inLineCount += 4;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
				break;
			}  // switch(hdr.colorType)

			if (gPng_Info.x >= gPng_Info.wid)
			{
				gPng_Info.y++;
				gPng_Info.x = -1;
				ShiftTwoLineBuf();
			}

			return EFI_SUCCESS;

		case 1:											// Interlace
			//   1 6 4 6 2 6 4 6
			//   7 7 7 7 7 7 7 7
			//   5 6 5 6 5 6 5 6
			//   7 7 7 7 7 7 7 7
			//   3 6 4 6 3 6 4 6
			//   7 7 7 7 7 7 7 7
			//   5 6 5 6 5 6 5 6
			//   7 7 7 7 7 7 7 7
			switch (gPng_Info.interlacePass)
			{
			case 1:
				interlaceWid = (gPng_Info.wid + 7) / 8;
				interlaceHei = (gPng_Info.hei + 7) / 8;
				interlaceX = gPng_Info.x * 8;
				interlaceY = gPng_Info.y * 8;
				break;
			case 2:
				interlaceWid = (gPng_Info.wid + 3) / 8;
				interlaceHei = (gPng_Info.hei + 7) / 8;
				interlaceX = 4 + gPng_Info.x * 8;
				interlaceY = gPng_Info.y * 8;
				break;
			case 3:
				interlaceWid = (gPng_Info.wid + 3) / 4;
				interlaceHei = (gPng_Info.hei + 3) / 8;
				interlaceX = gPng_Info.x * 4;
				interlaceY = 4 + gPng_Info.y * 8;
				break;
			case 4:
				interlaceWid = (gPng_Info.wid + 1) / 4;
				interlaceHei = (gPng_Info.hei + 3) / 4;
				interlaceX = 2 + gPng_Info.x * 4;
				interlaceY = gPng_Info.y * 4;
				break;
			case 5:
				interlaceWid = (gPng_Info.wid + 1) / 2;
				interlaceHei = (gPng_Info.hei + 1) / 4;
				interlaceX = gPng_Info.x * 2;
				interlaceY = 2 + gPng_Info.y * 4;
				break;
			case 6:
				interlaceWid = (gPng_Info.wid) / 2;
				interlaceHei = (gPng_Info.hei + 1) / 2;
				interlaceX = 1 + gPng_Info.x * 2;
				interlaceY = gPng_Info.y * 2;
				break;
			case 7:
				interlaceWid = gPng_Info.wid;
				interlaceHei = gPng_Info.hei / 2;
				interlaceX = gPng_Info.x;
				interlaceY = 1 + gPng_Info.y * 2;
				break;
			default:
				return EFI_UNSUPPORTED;
			} // switch(interlacePass)

			switch (gPng_Info.hdr.colorType)				// See PNG Specification for Colour types and values
			{
			case 0:								// Grayscale
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 1, gPng_Info.filter);

					gPng_Info.index = interlaceX * 4 + interlaceY * gPng_Info.wid * 4;
					gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
					gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount];
					gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount];

					if (gPng_Info.curLine8[gPng_Info.inLineCount] == gPng_Info.trns.col[0] || 
					   gPng_Info.curLine8[gPng_Info.inLineCount] == gPng_Info.trns.col[1] || 
					   gPng_Info.curLine8[gPng_Info.inLineCount] == gPng_Info.trns.col[2])
					{
						gPng_Info.rgba[gPng_Info.index+3] = 0;
					}
					else
					{
						gPng_Info.rgba[gPng_Info.index+3] = 255;
					}

					gPng_Info.x++;
					gPng_Info.inLineCount++;
					break;
				}
				break;

			case 2:													// True color
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 3)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 3, gPng_Info.filter);

						gPng_Info.index = interlaceX * 4 + interlaceY * gPng_Info.wid * 4;
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+2];

						if ( gPng_Info.curLine8[ gPng_Info.inLineCount] ==  gPng_Info.trns.col[0] && 
						    gPng_Info.curLine8[ gPng_Info.inLineCount+1] ==  gPng_Info.trns.col[1] && 
						    gPng_Info.curLine8[ gPng_Info.inLineCount+2] ==  gPng_Info.trns.col[2])
						{
							 gPng_Info.rgba[ gPng_Info.index+3] = 0;
						}
						else
						{
							 gPng_Info.rgba[ gPng_Info.index+3] = 255;
						}

						 gPng_Info.x++;
						 gPng_Info.inLineCount += 3;
						 gPng_Info.inPixelCount = 0;
					}
					break;

				case 16:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 6)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 6, gPng_Info.filter);
						gPng_Info.index = interlaceX * 4 + interlaceY * gPng_Info.wid * 4;
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+2];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+4];

						gPng_Info.r = gPng_Info.curLine8[gPng_Info.inLineCount] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.g = gPng_Info.curLine8[gPng_Info.inLineCount+2] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+3];
						gPng_Info.b = gPng_Info.curLine8[gPng_Info.inLineCount+4] * 256 + gPng_Info.curLine8[gPng_Info.inLineCount+5];

						if(gPng_Info.r==gPng_Info.trns.col[0] && gPng_Info.g==gPng_Info.trns.col[1] && gPng_Info.b==gPng_Info.trns.col[2])
						{
							gPng_Info.rgba[gPng_Info.index+3] = 0;
						}
						else
						{
							gPng_Info.rgba[gPng_Info.index+3] = 255;
						}

						gPng_Info.x++;
						gPng_Info.inLineCount += 6;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
				break;

			case 3:															// Indexed color
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount] = dat;
					Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 1, gPng_Info.filter);

					gPng_Info.index = interlaceX * 4 + interlaceY *gPng_Info.wid * 4;
					colIdx = gPng_Info.curLine8[gPng_Info.inLineCount];
					if (colIdx < gPng_Info.plt.nEntry)
					{
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.plt.entry[colIdx*3];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.plt.entry[colIdx*3+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.plt.entry[colIdx*3+2];
						if (colIdx == gPng_Info.trns.col[0] || colIdx == gPng_Info.trns.col[1] || colIdx == gPng_Info.trns.col[2])
						{
							gPng_Info.rgba[gPng_Info.index+3] = 0;
						}
						else
						{
							gPng_Info.rgba[gPng_Info.index+3] = 255;
						}
					}

					gPng_Info.x++;
					gPng_Info.inLineCount++;
					break;
				}
				break;

			case 4:													// Greyscale with alpha
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 2)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 2, gPng_Info.filter);

						gPng_Info.index = interlaceX * 4 + interlaceY * gPng_Info.wid * 4;
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+3] = gPng_Info.curLine8[gPng_Info.inLineCount+1];

						gPng_Info.x++;
						gPng_Info.inLineCount += 2;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
				break;

			case 6:														// Truecolor with alpha
				switch (gPng_Info.hdr.bitDepth)
				{
				case 8:
					gPng_Info.curLine8[gPng_Info.inLineCount+gPng_Info.inPixelCount] = dat;
					gPng_Info.inPixelCount++;
					if (gPng_Info.inPixelCount == 4)
					{
						Filter8(gPng_Info.curLine8, gPng_Info.prvLine8, gPng_Info.x, gPng_Info.y, 4, gPng_Info.filter);

						gPng_Info.index = interlaceX * 4 + interlaceY * gPng_Info.wid * 4;
						gPng_Info.rgba[gPng_Info.index] = gPng_Info.curLine8[gPng_Info.inLineCount];
						gPng_Info.rgba[gPng_Info.index+1] = gPng_Info.curLine8[gPng_Info.inLineCount+1];
						gPng_Info.rgba[gPng_Info.index+2] = gPng_Info.curLine8[gPng_Info.inLineCount+2];
						gPng_Info.rgba[gPng_Info.index+3] = gPng_Info.curLine8[gPng_Info.inLineCount+3];

						gPng_Info.x++;
						gPng_Info.inLineCount += 4;
						gPng_Info.inPixelCount = 0;
					}
					break;
				}
			break;
			} // switch(hdr.colorType)

			if (gPng_Info.x >= (int)interlaceWid)
			{
				gPng_Info.y++;
				gPng_Info.x = -1;
				ShiftTwoLineBuf();
				if (gPng_Info.y >= (int)interlaceHei)
				{
					gPng_Info.y = 0;
					gPng_Info.interlacePass++;
				}
			}

			return EFI_SUCCESS;
		default:
			return EFI_UNSUPPORTED;
		}
	}
}

/**
    Decode the PNG and store the output in RGB format

    @param PNGImage IN UINT32 PNGImageSize, IN OUT VOID **Blt, 
    @param BltSize OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth

    @retval EFI_STATUS

**/
EFI_STATUS ConvertPNGToUgaBlt( IN VOID *PNGImage, IN UINT32 PNGImageSize, IN OUT VOID **Blt, IN OUT UINTN *BltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth )
{
	//PngImageDecoder pngDecoder;
	//pngDecoder.Initialize();
	EFI_STATUS Status = EFI_SUCCESS;
	unsigned int x,y;
	EFI_UGA_PIXEL *buf = (EFI_UGA_PIXEL *)NULL;
	unsigned char *tempRGBA;
	PNGImageSize = 0;
	Initialize();
	if (Decode((unsigned char*)PNGImage, PNGImageSize) == EFI_SUCCESS)        //Decodes the PNG image to convert PNG image data to RGB format
	{
		

		*PixelHeight = gPng_Info.hei;
		*PixelWidth  = gPng_Info.wid;
		*BltSize = (*PixelHeight) * (*PixelWidth) * 4;

		*Blt = EfiLibAllocateZeroPool(*BltSize);
		if (*Blt == NULL) 
		{
			Status = EFI_OUT_OF_RESOURCES;
			return Status;
		}
		
		buf = (EFI_UGA_PIXEL *)*Blt;

		for(y = 0; y < *PixelHeight; y++) {
			for(x = 0; x < *PixelWidth; x++) {
				
				if(x < (unsigned int)gPng_Info.wid)
				{
					tempRGBA = gPng_Info.rgba + (y * gPng_Info.wid + x) * 4;
				}
				else
				{
					tempRGBA = gPng_Info.rgba + (y * gPng_Info.wid + gPng_Info.wid - 1) * 4;
				}
				
				buf->Blue = tempRGBA[2];
				buf->Green = tempRGBA[1];						//Stores the converted image data(RGB format)
				buf->Red = tempRGBA[0];
				buf++;
			}
		}
	} else {
		Status = EFI_ABORTED;
	}
	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**           (C)Copyright 2011, American Megatrends, Inc.           **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
