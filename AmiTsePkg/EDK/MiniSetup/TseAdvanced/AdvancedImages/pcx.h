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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/pcx.h $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:02p $
//
//*****************************************************************//
//*****************************************************************//
/** @file pcx.h
    Header file for pcx image format related code

**/


#ifndef _PCX_H_13_JUNE_2006
#define _PCX_H_13_JUNE_2006

#include "MyDefs.h"

#pragma pack(1)

typedef struct _AMI_TRUECOLOR_PIXEL{
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Reserved;
}AMI_TRUECOLOR_PIXEL, *PAMI_TRUECOLOR_PIXEL;

#pragma pack()


//PCX File Header (128 bytes)
typedef struct _PCX_HEADER{
	BYTE bManufacturer;
	BYTE bVersion;
	BYTE bEncoding;
	BYTE bBitsPerPixel;
	WORD wXmin;
	WORD wYmin;
	WORD wXmax;
	WORD wYmax;
	WORD wHres;
	WORD wVres;
	BYTE bPalette[48];
	BYTE bReserved;
	BYTE bColorPlanes;
	WORD wBytesPerLine;
	WORD wPaletteType;
	WORD wHorizontalSize;
	WORD wVerticalSize;
	BYTE bFiller[54];
}PCX_HEADER;

typedef struct _PCX_INFO{
	WORD wImageWidth;
	WORD wImageHeight;
	WORD wBytesPerLine;
	BYTE bBitsPerPixel;
	DWORD dwPCXImageSize;
	BYTE *bpPCXData;
	BYTE *bpPalette;
}PCX_INFO;


#endif //#ifndef _PCX_H_13_JUNE_2006

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2007, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
