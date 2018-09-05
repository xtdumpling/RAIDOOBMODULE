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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/png.h $
//
// $Author: Rajashakerg $
//
// $Revision: 1 $
//
// $Date: 6/13/11 12:21p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file png.h
    Header file for png image format related code

**/


#include "MyDefs.h"

#include EFI_PROTOCOL_DEFINITION(GraphicsOutput) 
#ifndef _PNG_H_
#define _PNG_H_

EFI_STATUS ConvertPNGToBlt ( IN VOID *PNGImage, IN UINT32 PNGImageSize, IN OUT VOID **Blt, IN OUT UINTN *BltSize, OUT UINT32 *PixelHeight, OUT UINT32 *PixelWidth);

typedef struct _PngHeader //information about the PNG image
{
	unsigned int width;				//width of the image in pixels 
	unsigned int height;			//height of the image in pixels
	unsigned int bitDepth;
	unsigned int colorType;
	unsigned int compressionMethod;	//compression method of the original file
	unsigned int filterMethod;		//filter method of the original file
	unsigned int interlaceMethod;	//interlace method of the original file
} PngHeader;

typedef struct _PngPalette
{
	unsigned int nEntry;
	unsigned char *entry;
} PngPalette;

typedef struct _PngTransparency
{
	unsigned int col[3];
} PngTransparency;

enum{
	gamma_default=100000
    };

typedef struct _PngImageDecoder
{
	PngHeader hdr;
	PngPalette plt;
	PngTransparency trns;
	int wid,hei;
	unsigned char *rgba;  // Raw data of R,G,B,A
	int autoDeleteRgbaBuffer;
	int filter,x,y,firstByte;
	int inLineCount;
	int inPixelCount;
	unsigned int r,g,b,msb;  // msb for reading 16 bit depth
	unsigned int index;
	unsigned int interlacePass;
	unsigned char *twoLineBuf8,*curLine8,*prvLine8;		// For filtering
	unsigned int gamma;
}PngImageDecoder;

PngImageDecoder gPng_Info;

void Initialize(void);
	int CheckSignature(CHAR8 *PNGImage);
	EFI_STATUS ReadChunk(unsigned *length,unsigned char **buf,unsigned *chunkType,unsigned *crc);
	void Header_Decode(unsigned char dat[]);
	EFI_STATUS Palette_Decode(unsigned length,unsigned char dat[]);
	EFI_STATUS Transparency_Decode(unsigned length,unsigned char dat[],unsigned int colorType);
	EFI_STATUS Decode(unsigned char *PNGImage,UINT32 PNGImageSize);
	void ShiftTwoLineBuf(void);
	EFI_STATUS PrepareOutput(void);
	EFI_STATUS Output(unsigned char dat);
	EFI_STATUS EndOutput(void);
EFI_STATUS Uncompress(unsigned length, unsigned char dat[]);


#endif //#ifndef _PNG_H_

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
