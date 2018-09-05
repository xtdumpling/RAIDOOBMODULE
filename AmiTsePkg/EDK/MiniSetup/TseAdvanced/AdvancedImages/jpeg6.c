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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/jpeg6.c $
//
// $Author: Arunsb $
//
// $Revision: 9 $
//
// $Date: 4/26/12 3:25a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file jpeg6.c
    Support for JPEG decoding

**/
//*****************************************************************//

#include "MyDefs.h"
#include "Jpeg6.h"


//Globals_START

UINTN unBitStreamOFFSET;
BYTE bNumBitsRead;
BYTE btempBitStreamByte;

UINTN unSOF0;
WORD wDCLumtable[13*3];
WORD wACLumtable[170*3];
WORD wDCChrtable[13*3];
WORD wACChrtable[170*3];

WORD space_image[256*4];
AMI_TRUECOLOR_PIXEL_JPEG * ScreenBufferJPEG = (AMI_TRUECOLOR_PIXEL_JPEG *)NULL;

WORD image_Y[64];
WORD image_Y1[64];
WORD image_Y2[64];
WORD image_Y3[64];
WORD image_Cr[64];
WORD image_Cb[64];
UINTN QT[2];
UINTN HT[4];
BYTE bQTLumTable[64];
BYTE bQTChrTable[64];
WORD jpeg_zigzag_order[] = {								\
							0,  1,  5,  6, 14, 15, 27, 28,	\
							2,  4,  7, 13, 16, 26, 29, 42,	\
							3,  8, 12, 17, 25, 30, 41, 43,	\
							9, 11, 18, 24, 31, 40, 44, 53,	\
							10, 19, 23, 32, 39, 45, 52, 54,	\
							20, 22, 33, 38, 46, 51, 55, 60,	\
							21, 34, 37, 47, 50, 56, 59, 61,	\
							35, 36, 48, 49, 57, 58, 62, 63	\
							};

WORD jpeg_zigzag_order_temp[64];


WORD diff_Y	;
WORD diff_Cr;
WORD diff_Cb;
WORD gRsi = 0;
UINT32 g_un32BufferWidth ;
UINT32 g_un32BufferHeight;

//Globals_End


//Function Declarations
void BuildHuffmanCodeTable(WORD * pwHuffCodeTable ,BYTE * pbySrcHT);
void HuffmanDecoding(WORD * pwDCCoeffTable, WORD * pwACCoeffTable, WORD * pwImage);
void GetDCValue(WORD * pwHuffmanTable, WORD * pwValue, WORD * pwLengthDC_RunSizeAC);
BOOL SearchHuffmanCode(WORD * pwSearchTablePos, WORD * pwHuffmanCodeValue, WORD wDc_value_length);
BOOL GetEncodedBit();
void ReadJPEGImage(WORD * pwDCCoeffTable, WORD * pwACCoeffTable, WORD * pwImage, WORD * diff, BYTE * pbySrcQT, int nLumFlag);
void ReadJPEGImage_YUV(DWORD * pdwBuffer, WORD wSampling);
void OutputJPEGImage_YUV(WORD wXPos, WORD wYPos, WORD wXLength, WORD wYLength, WORD	wSampling);
void Output8x8Image(WORD wLeft, WORD wTop, BYTE * pspace_image);
BOOL InitJpegParams(BYTE * pbyImage);
DWORD ConvertYCbCrToRGB(short Y, short Cb, short Cr);
void InvertDCT(WORD * pwSource);
void InvertDCT_xy(WORD * pwSource, WORD wVal);
void GetTables(BYTE bNumQT, BYTE bNumHT);


/**
    Initializes required global variables

    @param VOID

    @retval VOID

**/
void InitGlobals(){
	diff_Y		= 0;
	diff_Cr	= 0;
	diff_Cb	= 0;
	bNumBitsRead = 8;
	btempBitStreamByte = 0;
}

/**
    Decode the JPEG image into the given buffer at given position, 
    subject to the limit of the Buffer Width and Height

    @param wXPos X Start Postion with in the Buffer 
        where the image need to be decoded
    @param wYPos Y Start Postion with in the Buffer 
        where the image need to be decoded
    @param pbyJPEGInputData Pointer that gives the memory location 
        where the Input data is available
    @param pbyJPEGOutputData Pointer that gives the pointer 
        for the output buffer
    @param un32BufferWidth Width of the output Buffer
    @param un32BufferHeight Height of the output Buffer

    @retval BOOL TRUE/FALSE Indicate whether the Decoding was successful

**/
BOOL DecodeJPEG(WORD wXPos, WORD wYPos, BYTE * pbyJPEGInputData, BYTE * pbyJPEGOutputData, UINT32 un32BufferWidth, UINT32 un32BufferHeight){
	WORD wX_Length=0, wY_Length=0; 
	ScreenBufferJPEG = (AMI_TRUECOLOR_PIXEL_JPEG *)pbyJPEGOutputData;
	g_un32BufferWidth	= un32BufferWidth ; 
	g_un32BufferHeight	= un32BufferHeight;

    gRsi = 0;
	InitGlobals();

	if(InitJpegParams(pbyJPEGInputData)){
		wX_Length = (*(WORD*)((BYTE *)unSOF0+5)<<8)|*((BYTE *)unSOF0+6);
		wY_Length = (*(WORD*)((BYTE *)unSOF0+3)<<8)|*((BYTE *)unSOF0+4);
		 
if((0x11==(*(BYTE *)(unSOF0+3+4+1+1+3)))&&(0x11==((*(BYTE *)(unSOF0+3+4+1+1+3+3)))))
		{
#if MKF_JPEG_YUV111_SUPPORT
			if(0x11==(*(BYTE *)(unSOF0+3+4+1+1)))
				OutputJPEGImage_YUV(wXPos, wYPos, wX_Length, wY_Length, 0x111);
			else
#endif
				if(0x22==(*(BYTE *)(unSOF0+3+4+1+1)))
					OutputJPEGImage_YUV(wXPos, wYPos, wX_Length, wY_Length, 0x122);
		}
		return TRUE;
	}
	else
		return FALSE;
	
}

/**
    Get the dimensions (Width and Height) for the JPEG image

    @param pbyJPEGInputData Pointer that gives the memory 
        location where the Input data is available
    @param pun32Width Pointer to the UINT32 variable that 
        will recieve the Width of the Image
    @param pun32Height Pointer to the UINT32 variable that 
        will recieve the Height of the Image

    @retval BOOL TRUE/FALSE.

**/
BOOL GetJPEGDimensions(BYTE * pbyJPEGInputData, UINT32 * pun32Width, UINT32 * pun32Height){
	if(InitJpegParams(pbyJPEGInputData)){
		*pun32Width =  (WORD)((*(WORD*)((BYTE *)unSOF0+5)<<8)|*((BYTE *)unSOF0+6));
		*pun32Height=  (WORD)((*(WORD*)((BYTE *)unSOF0+3)<<8)|*((BYTE *)unSOF0+4));
		return TRUE;
	}
	else return FALSE;
}

/**
    Builds the Huffman codes for the particular JPEG Huffman table.
    By incrementing the offset of the Huffman table by 3, 
    the offset of the actual data is obtained. The first 
    16 bytes represent the no: of code words for a paricular 
    bit-size. Having the bit-size and the no: of code words 
    for each bit-size, the Huffman code table is built.

    @param pwHuffCodeTable Pointer to the memory location 
        that contains the Huffman codes for the particular JPEG Huffman table.
    @param pbySrcHT Pointer to the memory location that contains 
        the offset of the JPEG Huffman Table.

    @retval VOID

**/
void BuildHuffmanCodeTable(WORD * pwHuffCodeTable ,BYTE * pbySrcHT){

	BYTE * pbySrcHTCopy = NULL;
	int nCounter=0;
	int nOffSet=0;			
	BYTE wCodeSize = 1;		
	WORD wCodeWord = 0;		

	pbySrcHT			+= 1;	
	pbySrcHTCopy = pbySrcHT;
	do{
		for(nCounter=*pbySrcHT;nCounter>0; nCounter--){
			pwHuffCodeTable[nOffSet++]	= wCodeSize;
			pwHuffCodeTable[nOffSet++]	= wCodeWord++;
			pwHuffCodeTable[nOffSet++] = *(pbySrcHTCopy+16);
			pbySrcHTCopy++;
		}
		wCodeWord<<=1;
		pbySrcHT++; 
		wCodeSize++;
	}while(wCodeSize<=16);
	pwHuffCodeTable[nOffSet]	= 17;
}

/**
    Reads the 8x8 matrix data for each of Y_0,Y_1,Y_2,Y_3,Cb and Cr. 
    The final output will be image data after inverse DCT is applied.

    @param pwDCCoeffTable DC Luminance/ Chrominance Huffman Table.
    @param pwACCoeffTable AC Luminance/ Chrominance Huffman Table.
    @param pwImage Pointer to the 8x8 matrix data for each of Y_0,Y_1,Y_2,Y_3,Cb and Cr.
    @param pwDiff Pointer to the variable that hold the differences of the DC coefficients. 
        Adds the difference to the Prev Dc coefficient value to get the present DC coefficient value.
    @param pbySrcQT Pointer to the Quantization table.
        int nLumFlag - Flag if it is set, indicates that the image component is luminance.

    @retval VOID

**/
void ReadJPEGImage(WORD * pwDCCoeffTable, WORD * pwACCoeffTable, WORD * pwImage, WORD * diff, BYTE * pbySrcQT, int nLumFlag)
{
	int nCounter;
	HuffmanDecoding(pwDCCoeffTable, pwACCoeffTable, pwImage);
	*diff = *diff + pwImage[0]; 
	pwImage[0] = *diff;
	
	//DeQuantization
	for(nCounter=0; nCounter<64; nCounter++)
	{
		pwImage[nCounter] = pwImage[nCounter] * (WORD)pbySrcQT[nCounter]; 
	}
	
	//InvertZigZagOrder
	for(nCounter=0; nCounter<64; nCounter++)
	{
		jpeg_zigzag_order_temp[nCounter] = *((WORD *)((BYTE *)pwImage+(jpeg_zigzag_order[nCounter]<<1)));
	}
	memcpy(pwImage,jpeg_zigzag_order_temp, 128);
	InvertDCT(pwImage);

	if(nLumFlag)
	{
	//Add128
	for(nCounter=0; nCounter<64; nCounter++)
		pwImage[nCounter] +=128; 
	}
}

/**
    Reads the 8x8 matrix data for each of Y_0,Y_1,Y_2,Y_3,Cb and Cr. 
    The final output will be RGB pixel data.

    @param pdwBuffer RGB format of image data.
    @param wSampling Value tells whether image is YUV111(luminance 
        as well as chrominance is taken for every pixel) or 
        YUV122(luminance is taken for every pixel while chrominance 
        is taken as medium value for 2x2 block of pixels).
        Since the sampling factor for Luminance is 2, all the pixel 
        values are stored, where as for Cb and Cr, only the average 
        of 2x2 pixels is stored.

    @retval VOID

**/
void ReadJPEGImage_YUV(DWORD * pdwBuffer, WORD wSampling){
	int nCounter = 0;
	int nCbCrCounter = 0;					
	ReadJPEGImage(wDCLumtable, wACLumtable, image_Y, &diff_Y, bQTLumTable, 1);
	
	if(0x122 ==wSampling)
	{
		ReadJPEGImage(wDCLumtable, wACLumtable, image_Y1,&diff_Y, bQTLumTable, 1);
		ReadJPEGImage(wDCLumtable, wACLumtable, image_Y2,&diff_Y, bQTLumTable, 1);
		ReadJPEGImage(wDCLumtable, wACLumtable, image_Y3,&diff_Y, bQTLumTable, 1);
	}

	ReadJPEGImage(wDCChrtable, wACChrtable, image_Cr,&diff_Cr, bQTChrTable, 0);
	ReadJPEGImage(wDCChrtable, wACChrtable, image_Cb,&diff_Cb, bQTChrTable, 0);

	for(nCounter=0; nCounter< 64; nCounter++){
#if MKF_JPEG_YUV111_SUPPORT
		if(0x111 ==wSampling)
		pdwBuffer[nCounter] = ConvertYCbCrToRGB(image_Y[nCounter], image_Cr[nCounter], image_Cb[nCounter]);
		else
#endif
		if(0x122 ==wSampling)
		{
			nCbCrCounter = (nCounter&0xfff7)/2;
			pdwBuffer[nCounter]		= ConvertYCbCrToRGB(image_Y[nCounter], image_Cr[nCbCrCounter], image_Cb[nCbCrCounter]);
			pdwBuffer[64+nCounter]	= ConvertYCbCrToRGB(image_Y1[nCounter], image_Cr[nCbCrCounter + 4], image_Cb[nCbCrCounter + 4]);
			pdwBuffer[128+nCounter] = ConvertYCbCrToRGB(image_Y2[nCounter], image_Cr[nCbCrCounter + 32], image_Cb[nCbCrCounter + 32]);
			pdwBuffer[192+nCounter] = ConvertYCbCrToRGB(image_Y3[nCounter], image_Cr[nCbCrCounter + 32 + 4], image_Cb[nCbCrCounter + 32 + 4]);
		}
	}

}

/**
    This routine decodes the AC and DC coefficients using the 
    Huffman tables stored in the JPEG image.

    @param pwDCCoeffTable DC Luminance/ Chrominance Huffman Table.
    @param pwACCoeffTable AC Luminance/ Chrominance Huffman Table.
    @param pwImage Pointer to the memory location that contains the decoded data.

    @retval VOID

**/
void HuffmanDecoding(WORD * pwDCCoeffTable, WORD * pwACCoeffTable, WORD * pwImage)
{
	int nBlockCount=0;

	WORD wLengthDC_RunSizeAC	= 0;
	WORD wCoeffVal		= 0;

	memset(pwImage,(64*sizeof(WORD)),0);

	GetDCValue(pwDCCoeffTable, &wCoeffVal, &wLengthDC_RunSizeAC);
	pwImage[nBlockCount++] = wCoeffVal;	// DC coefficient Value
	
	for(;nBlockCount<64;nBlockCount++)
	{
			GetDCValue(pwACCoeffTable, &wCoeffVal, &wLengthDC_RunSizeAC);
			if(0==wLengthDC_RunSizeAC){
				break;
			}
		wLengthDC_RunSizeAC&=0x0F0;
		wLengthDC_RunSizeAC>>=4;

		for(;wLengthDC_RunSizeAC>0;wLengthDC_RunSizeAC--){
			pwImage[nBlockCount++] = 0;
		}

		pwImage[nBlockCount] = wCoeffVal;	// AC coefficient Value
	}

	return;
}

/**
    This routine returns the differences(DCCoefficientpresent - 
    DCCoefficientprev) for the DC coefficient. For AC coefficients, 
    it returns the actual AC coefficient value and Run/Size.

    @param pwHuffmanTable Pointer to the huffman table ?DC Luminance, 
    @param Chrominance AC Luminance, AC Chrominance.
    @param pwValue Code Word (Actual Bit representation).
    @param pwLengthDC_RunSizeAC Indicates the no: of zeroes.

    @retval VOID

**/
void GetDCValue(WORD * pwHuffmanTable, WORD * pwValue, WORD * pwLengthDC_RunSizeAC)
{
	WORD dc_value_length=0;
	WORD wCoeffLen=0; 
	DWORD dwCoeffVal=0;
	int i=0;

	while(1)
	{
		wCoeffLen<<=1;
		if(GetEncodedBit())
			wCoeffLen|=0x1;

		dc_value_length++;
		
		if( 0xffff == dc_value_length ) 
			return;

		if(SearchHuffmanCode(pwHuffmanTable, (WORD*)&wCoeffLen,dc_value_length)){
			break;
		}
	};


	*pwLengthDC_RunSizeAC	= wCoeffLen;
		
	wCoeffLen&=0x0f;
	
	for(i=0;i<wCoeffLen;i++)
	{
		dwCoeffVal<<=1;
		if(GetEncodedBit())
			dwCoeffVal|=0x1;
	}


	if(0!=wCoeffLen--)
	{
		if(!( dwCoeffVal & (0x1<<(wCoeffLen%16)) )   )
		{
			wCoeffLen = (14+16) - wCoeffLen;
	
			dwCoeffVal=(dwCoeffVal<<(wCoeffLen&0xff)) | 0x80000000;
			
			*((signed int *)(&dwCoeffVal))	>>= (wCoeffLen&0xff);
			dwCoeffVal++;
		}
	}
		
	*pwValue				= (WORD)dwCoeffVal;
	return;
}

/**
    Searches the Huffman code of the DC/AC Luminance/Chrominace 
    values in Huffman code tables and find the appropriate HuffmanCodeValue.

    @param pwSearchTablePos Pointer to the huffman table ?DC Luminance, 
    @param Chrominance AC Luminance, AC Chrominance.
    @param pwHuffmanCodeValue Pointer to the variable that contains 
        the Huffman code value.
    @param wDc_value_length No: of bits in the Huffman code.

    @retval BOOL TRUE/FALSE.

**/
BOOL SearchHuffmanCode(WORD * pwSearchTablePos, WORD * pwHuffmanCodeValue, WORD wDc_value_length)
{
	BOOL bReturn = FALSE;

	while(wDc_value_length>=(*pwSearchTablePos))
	{
		if(wDc_value_length==(*pwSearchTablePos))
		{
			if(*pwHuffmanCodeValue==*(pwSearchTablePos+1))
			{ 
				*pwHuffmanCodeValue=*(pwSearchTablePos+2);
				bReturn = TRUE;
				break;
			}
		}
		pwSearchTablePos+=3;
	}
	return bReturn;
}

/**
    Reads bits from the stream and returns whether the carry 
    flag is set or not.

    @param VOID

    @retval BOOL TRUE/FALSE depending on whether the Carry flag is set or not.

**/
BOOL GetEncodedBit()
{	
	
	UINTN unBitStreamOFFSETCopy;
	BYTE bStreamData;

	unBitStreamOFFSETCopy = unBitStreamOFFSET;
	if(8==bNumBitsRead){
		bStreamData=*(BYTE *)(unBitStreamOFFSETCopy++);
		if(0xff==bStreamData){
			if(0!=(*(BYTE *)(unBitStreamOFFSETCopy++))){
					bStreamData=*(BYTE *)(unBitStreamOFFSETCopy++);
				}
		}
		unBitStreamOFFSET = unBitStreamOFFSETCopy;
		btempBitStreamByte = bStreamData;
		bNumBitsRead=0;
	}
	bNumBitsRead++;
	bStreamData					= btempBitStreamByte;

	if( (bStreamData&(0x1<<(8-bNumBitsRead))) ){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

/**
    This functions writes to the Global Screen Buffer by calling 
    Output8x8Image function.

    @param wX_position X Start Position within the output buffer 
        from where the image needs to be decoded.
    @param wY_position Y Start Position within the output buffer 
        from where the image needs to be decoded.
    @param wX_length Width of the image.
    @param wY_length Height of the image.
    @param wSampling Value tells whether image is YUV111(luminance 
        as well as chrominance is taken for every pixel) or YUV122
        (luminance is taken for every pixel while chrominance is 
        taken as medium value for 2x2 block of pixels).

    @retval VOID

**/
void OutputJPEGImage_YUV(WORD wX_position, WORD wY_position, WORD wX_length, WORD wY_length, WORD	wSampling)
{
	WORD Temp_X_Pos= wX_position;
	WORD Temp_X_Len= wX_length;
	UINTN BlkCount=0;

	if(wSampling==0x122)
	{
		wY_length+= 0x0f;
		wY_length >>=4;
	}
#if MKF_JPEG_YUV111_SUPPORT
	else if(wSampling==0x111)
	{
		wY_length+= 0x07;
		wY_length >>=3;
	}
#endif	
		for(;wY_length>0;wY_length--)
		{
			wX_position			= Temp_X_Pos;
			wX_length			= Temp_X_Len; 

			if(wSampling==0x122)
			{
				wX_length+= 0x0f;
				wX_length >>=4;
			
			for(;wX_length>0;wX_length--)
			{
//				memset(space_image,sizeof(space_image),0);
				{
					ReadJPEGImage_YUV((DWORD *)space_image,wSampling);
					Output8x8Image(wX_position, wY_position, (BYTE *)space_image);
					Output8x8Image(wX_position+8, wY_position, (BYTE *)space_image+256);
					Output8x8Image(wX_position, wY_position+8, (BYTE *)space_image+(256*2));
					Output8x8Image(wX_position+8, wY_position+8, (BYTE *)space_image+(256*3));
					wX_position+=16;
					BlkCount++;
                    // if Restart Interval valid Support for YUV122
					if(gRsi && ((BlkCount%gRsi) == 0))
					{   
                        UINT8 *jpgScan = (UINT8*)unBitStreamOFFSET;
                        // It is going to be 0xFFD0 .. 0xFFD7
                        if((jpgScan[0] == 0xFF) && ((jpgScan[1]& 0xF8 ) == 0xD0))
                        {
                            // Restart the scan params
 							InitGlobals();
                            // Skip the Rsi maraker
							unBitStreamOFFSET+=2;
                        }
					}
				}
			}
			wY_position+=16;
			}
#if MKF_JPEG_YUV111_SUPPORT
			else if(wSampling==0x111)
			{
				wX_length+= 0x07;
				wX_length >>=3;
				for(;wX_length>0;wX_length--)
				{
						//memset(space_image,sizeof(space_image),wSampling);
						ReadJPEGImage_YUV((DWORD *)space_image,wSampling);
						Output8x8Image(wX_position, wY_position, (BYTE *)space_image);
						wX_position+=8;
						BlkCount++;
                        // if Restart Interval valid
						if(gRsi && ((BlkCount%gRsi) == 0))
						{   
                            UINT8 *jpgScan = (UINT8*)unBitStreamOFFSET;
                            // It is going to be 0xFFD0 .. 0xFFD7
                            if((jpgScan[0] == 0xFF) && ((jpgScan[1]& 0xF8 ) == 0xD0))
                            {
                                // Restart the scan params
 							    InitGlobals();
                                // Skip the Rsi maraker
							    unBitStreamOFFSET+=2;
                            }
						}
				}
				wY_position+=8;
			}
#endif
		}
}

/**
    This functions writes to the Global Screen Buffer.

    @param wLeft Left coordinate from where the Screen buffer 
        is to be filled.
    @param wTop Top coordinate from where the Screen buffer is to be filled.
    @param pspace_image Pointer to 8x8 block of image.

    @retval VOID

**/  
void Output8x8Image(WORD wLeft, WORD wTop, BYTE * pspace_image){
	int nCounterY=0;
	int nCounterX=0;
	AMI_TRUECOLOR_PIXEL_JPEG * pStartofLineInScreenBuffer = (AMI_TRUECOLOR_PIXEL_JPEG *)ScreenBufferJPEG + wTop * g_un32BufferWidth + wLeft;

	for(;nCounterY<8;nCounterY++){
		if((UINT32)(wTop + nCounterY) >= g_un32BufferHeight){
			break;
		}
		for(nCounterX=0;nCounterX<8; nCounterX++){
			if( (UINT32)(wLeft + nCounterX) < g_un32BufferWidth){
				(pStartofLineInScreenBuffer + nCounterX)->Red	= *pspace_image;
				(pStartofLineInScreenBuffer + nCounterX)->Green	= *(pspace_image+1);
				(pStartofLineInScreenBuffer + nCounterX)->Blue	= *(pspace_image+2);
				}
			pspace_image+=4;
		}
		pStartofLineInScreenBuffer+=g_un32BufferWidth;
	}
}

/**
    Gets the 2 Quantization Tables and 4 Huffman Tables.
    Checks the type of table and then builds the corresponding table.

    @param bNumQT Number of Quantization Tables.
    @param bNumHT Number of Huffman Tables.

    @retval VOID

**/
void GetTables(BYTE bNumQT, BYTE bNumHT)
{
	int i,j;
	
	for(j=0;j<bNumQT;j++)
	{

		switch(*((BYTE*)QT[j]))
		{
		case 0x00:
				for(i=0;i<64;i++)
				{
					bQTLumTable[i] = *((BYTE*)QT[j]+i+1);
				}
			break;
		case 0x01:
				for(i=0;i<64;i++)
				{
					bQTChrTable[i] = *((BYTE*)QT[j]+i+1);
				}
			break;
		}
	}
	for(i=0;i<bNumHT;i++)
	{
		switch((*(BYTE*)(HT[i])))
		{
			case 0x00:
				BuildHuffmanCodeTable(wDCLumtable,(BYTE *)(HT[i]));
				break;
			case 0x10:
				BuildHuffmanCodeTable(wACLumtable,(BYTE *)(HT[i]));
				break;
			case 0x01:
				BuildHuffmanCodeTable(wDCChrtable,(BYTE *)(HT[i]));
				break;
			case 0x11:
				BuildHuffmanCodeTable(wACChrtable,(BYTE *)(HT[i]));
				break;
		}
	}

}

/**
    Reads the image parameters and checks whether the image 
    is supported or not.
    TSE Jpeg Algorithm Supports following Markers
    ==================================================
    0C0h - Start Of Frame (Baseline DCT) (REQUIRED)
    0C4h - Define Huffman Table (REQUIRED)
    0D0h - RSI0 Marker (OPTIONAL)
    0D1h - RSI1 Marker (OPTIONAL)
    0D2h - RSI2 Marker (OPTIONAL)
    0D3h - RSI3 Marker (OPTIONAL)
    0D4h - RSI4 Marker (OPTIONAL)
    0D5h - RSI5 Marker (OPTIONAL)
    0D6h - RSI6 Marker (OPTIONAL)
    0D7h - RSI7 Marker (OPTIONAL)
    0D8h - Start of Image (REQUIRED)
    0D9h - End of Image (REQUIRED)
    0DAh - Start Of Scan (REQUIRED)
    0DBh - Define Quantization Table (REQUIRED)
    0DDh - Define Restart Interval (OPTIONAL)
    0E0h - 0xEF - APPx Markers (Does Depend on and just Skips marker) (DON'T CARE)
    0F0h - 0xFD - Reserved for JPEG extensions (Does Depend on and just Skips marker) (DON'T CARE)
    0FEh - Jpeg Comment (Does Depend on and just Skips marker) (DON'T CARE)

    TSE Jpeg Algorithom Does not Support following Markers
    ======================================================
    000h-0BFh - RESERVED
    0C1h - Start Of Frame (Extended sequential DCT)
    0C2h - Start Of Frame (Progressive DCT)
    0C3h - Start Of Frame (Lossless (sequential))
    0C5h - Start Of Frame (Differential sequential DCT)
    0C6h - Start Of Frame (Differential progressive DCT)
    0C7h - Start Of Frame (Differential lossless (sequential))
    0C8h - Start Of Frame (Reserved for JPEG extensions)
    0C9h - Start Of Frame (Extended sequential DCT)
    0CAh - Start Of Frame (Progressive DCT)
    0CBh - Start Of Frame (Lossless (sequential))
    0CDh - Start Of Frame (Differential sequential DCT)
    0CEh - Start Of Frame (Differential progressive DCT)
    0CFh - Start Of Frame (Differential lossless (sequential))
    0CCh - Arithmetic coding conditioning
    0DCh - Define number of lines
    0DEh - Define hierarchical progression
    0DFh - Expand reference component(s)
    0FFh - RESERVED

    @param pbyImage Pointer to the JPEG image.

    @retval BOOL TRUE/FALSE.

**/
BOOL InitJpegParams(BYTE * pbyImage)
{
	
	BYTE bJpegImageFlag	 = 0;
	BYTE bCount = 0;
	int i = 0;
	BYTE bNumHT=0;
	BYTE bNumQT=0;
	WORD wLen = 0;

	if ( ( *pbyImage != 0xFF ) || ( *(pbyImage+1) != 0xD8 ) )	// if marker is not Start Of Image
		return FALSE;
	do{
		if(0x0FF== *pbyImage){
			pbyImage++;
			//; start of image
			if ( 0xD8 == *pbyImage )
			{
				pbyImage++;
				continue;
			}
			//; end of image
			if(0x0D9== *pbyImage){
				break;
			}
			if(*pbyImage){
                // APPx Marks No information needed for us
                if((0x0F0 & *pbyImage) == 0x0E0){
				}
				//	; if maker is Start Of Frame
				else if(0x0C0== *pbyImage){
					bJpegImageFlag|=2;
					unSOF0 = (UINTN)(pbyImage+1);
				}
				//; if maker is Define Huffman Table
				else if(0x0C4== *pbyImage){
					HT[bNumHT] = (UINTN)(pbyImage+3);
					bNumHT++;
				}
				else if((0x0F0 & *pbyImage) == 0xC0){
				//	; if Frame markers
                    switch(*pbyImage) 
                    {
                    //Unsupported Frame Markers
                    case 0x0C1: //Start Of Frame (Extended sequential DCT)
                    case 0x0C2: //Start Of Frame (Progressive DCT)
                    case 0x0C3: //Start Of Frame (Lossless (sequential))
                    case 0x0C5: //Start Of Frame (Differential sequential DCT)
                    case 0x0C6: //Start Of Frame (Differential progressive DCT)
                    case 0x0C7: //Start Of Frame (Differential lossless (sequential))
                    case 0x0C8: //Start Of Frame (Reserved for JPEG extensions)
                    case 0x0C9: //Start Of Frame (Extended sequential DCT)
                    case 0x0CA: //Start Of Frame (Progressive DCT)
                    case 0x0CB: //Start Of Frame (Lossless (sequential))
                    case 0x0CD: //Start Of Frame (Differential sequential DCT)
                    case 0x0CE: //Start Of Frame (Differential progressive DCT)
                    case 0x0CF: //Start Of Frame (Differential lossless (sequential))
                    //Arithmetic coding conditioning
                    case 0x0CC:
    					return FALSE;
                    default:
                        // Never comes here as C4 and C0 is handled above
                        break;
                    }
				}
				//; if maker is Start Of Scan
				else if(0x0DA== *pbyImage){
					//SOS = (UINTN)(pbyImage+1);
					unBitStreamOFFSET = (UINTN)(pbyImage+13);
					break;
				}
				//; if maker is Define Quantization Table
				else if(0x0DB== *pbyImage){
						QT[bNumQT] = (UINTN)(pbyImage+3);
						bNumQT++;
				}
				//; if marker is Define Restart Interval Start
				else if(0x0DD== *pbyImage){
					*((BYTE *)&gRsi) = *(pbyImage+4);
					*((BYTE *)&gRsi+1) = *(pbyImage+3);
				}
                // if Restart RSIx Marker
				else if((*pbyImage & 0xF8 ) == 0xD0){
                }
				//; if marker is Define number of lines
				else if(0x0DC== *pbyImage){
   					return FALSE;
				}
				//; if marker is Define hierarchical progression
				else if(0x0DE== *pbyImage){
   					return FALSE;
				}

				//; if marker is Expand reference component(s)
				else if(0x0DF== *pbyImage){
   					return FALSE;
				}
                // Reserved for JPEG extensions and Comment. Skip it
                else if((*pbyImage >= 0xF0) && (*pbyImage <= 0xFE)){
                }
                // if the Marker is Less then 0xC0 Or 0xFF then they are reserved marker. No Valid
                // JPEG file may use it. We don't handle such files
                // that is ((*pbyImage < 0xC0) || (*pbyImage == 0xFF))
                else 
                {
   					return FALSE;
                }
    			pbyImage++;
				*((BYTE *)&wLen) = *(pbyImage+1);
				*((BYTE *)&wLen+1) = *pbyImage;
				pbyImage += wLen;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}while(1);
	
	//Support for 1 QT
	if(1 == bNumQT)
	{
			QT[bNumQT++] = (UINTN)(QT[0]+65);
	}

	//Support for 1 HT
	if(1 == bNumHT)
	{
		while(bNumHT<4)
		{
			bCount = 0;
		for(i=0;i<16;i++)
		{
			bCount = bCount + *(BYTE*)(HT[bNumHT-1]+i+1);
		}
		HT[bNumHT] = (UINTN)(HT[bNumHT-1]+bCount+17);
		bNumHT++;
		}
	}
// Check for 4 HT and 2 QT and SOF0
	if((4==bNumHT)&&(2==bNumQT)&&(0x02 & bJpegImageFlag)) 
	{
		GetTables(bNumQT, bNumHT);
		return TRUE;
	}
else
return FALSE;

}

/**
    Internal function for doing a check for each color component

    @param n32ColorComp Color Component (R, G or B).

    @retval The color component value after the check.

**/
INT32 Check(INT32 n32ColorComp)
{
	n32ColorComp>>=14;
	if(0x8000==(n32ColorComp&0x8000))
		n32ColorComp=0;

	if((n32ColorComp&0xffff)>255)
		n32ColorComp=0x00ff;//mov CX,255

	return n32ColorComp;
}

/**
    Converts the luminance(Y) and chrominance(Cb and Cr) values
    of the image to R, G, B values.	The R,G, B values are stored 
    in a DWORD variable.

    @param short Y - Y is the luminance value.
        short Cb - Cb is the chrominance Hue value.
        short Cr - Cr is the chrominance Saturation value.

    @retval Variable having the RGB value, which is a DWORD.

**/
DWORD ConvertYCbCrToRGB(short Y, short Cb, short Cr)
{
	DWORD dwRGB    =0;
	INT32 n32ColorComp	=0;
	INT32 YComp = (((INT32)(Y))<<14);
	INT32 CrComp = (((INT32)(Cr))*c_g_cr);
	INT32 CbComp =	((INT32)(Cb) * c_g_cb);
	//;;;cal r
	n32ColorComp = Check((YComp + (CrComp<<1)));
	dwRGB = ((n32ColorComp<<16)|((unsigned int)n32ColorComp>>16));

	//;;;cal g
	n32ColorComp = Check((YComp-CrComp-CbComp));
	*(((char *)(&dwRGB))+1) = *(char *)(&n32ColorComp);

	//;;;cal b
	n32ColorComp = Check((YComp + CbComp + (CbComp<<2) + (CbComp>>3)));
	*(char *)(&dwRGB) = *(char *)(&n32ColorComp);

	return dwRGB;

}

/**
    Performs inverse discrete cosine transform on dequantized data.

    @param pwSource Input is dequantized array of 8x8 pixels and 
        output is data after  applying inverse discrete cosine transform.

    @retval VOID

**/
void InvertDCT(WORD * pwSource){
	WORD * pwSourceCopy=pwSource;
	int nCounter=0;

	for(nCounter=0; nCounter<8; nCounter++){
		InvertDCT_xy(pwSource,1);
		pwSource+=8;		
	}
	pwSource = pwSourceCopy;
	for(nCounter=0; nCounter<8; nCounter++){
		InvertDCT_xy(pwSource,8);
		pwSource++;		
	}
}

/**
    Performs inverse discrete cosine transform on dequantized 
    data in x and y direction.

    @param pwSource Input is dequantized array of 8x8 pixels 
        and output is data after applying inverse discrete cosine transform.
    @param wVal 
        wVal = 1, then the routine performs inverse discrete 
        cosine transform on dequantized data in x direction.
        wVal = 8, then the routine performs inverse discrete cosine 
        transform on dequantized data in y direction.

    @retval VOID

**/
void InvertDCT_xy(WORD * pwSource, WORD wVal)
{
INT32 buf1[8];
INT32 buf2[8];
int Index[8];
int i=0;
for(;i<8;i++)
Index[i] = i*wVal;

//;;;stage 1, 2 ,3

//	;;o0=x0 and normalize
	buf2[0] = (((INT32)(INT16)(pwSource[0])*c1_sqrt2))>>14;


//	;;o1=x4*c1_4
	buf2[1] = (((INT32)(INT16)(pwSource[Index[4]]))*c1_4)>>14;

//	;;o2=x2

	buf2[2]	= ((INT32)(INT16)(pwSource[Index[2]]));
//	;;;o3=(x2+x6)*c1_4

	buf2[3] = ((((buf2[2] + (INT32)(INT16)(pwSource[Index[6]]))*c1_4))>>14);

//	;;o4=x1

	buf2[4]	= (INT32)(INT16)(pwSource[Index[1]]);

//	;;;o6=x1+x3

	buf2[6]	= buf2[4]	+ (INT32)(INT16)(pwSource[Index[3]]);

//	;;;o7=(x1+x3+x5+x7)*c1_4

	buf2[7] = (((buf2[6]+((INT32)(INT16)(pwSource[Index[5]]))+((INT32)(INT16)(pwSource[Index[7]])))*c1_4)>>14);

//	;;;o5=(x3+x5)*c1_4

	buf2[5] = (((((INT32)(INT16)(pwSource[Index[3]]))+((INT32)(INT16)(pwSource[Index[5]])))*c1_4)>>14);

//;;;stage 4, 5

//	;;b0=o0+o1
	buf1[0] = buf2[0]+buf2[1];

//	;;b1=o0-o1
	buf1[1] = buf2[0]-buf2[1];

//	;;b2=(o2+o3)*c1_8

	buf1[2]=(((buf2[2]+buf2[3])*c1_8)>>14);

//	;;b3=(o2-o3)*c3_8

	buf1[3]=(((buf2[2]-buf2[3])*c3_8)>>14);

//	;;b4=o4+o5
	buf1[4] = buf2[4]+buf2[5];

//	;;b5=o4-o5
	buf1[5] = buf2[4]-buf2[5];

//	;;b6=(o6+o7)*c1_8

	buf1[6]=(((buf2[6]+buf2[7])*c1_8)>>14);

//	;;b7=(o6-o7)*c3_8

	buf1[7]=(((buf2[6]-buf2[7])*c3_8)>>14);

//;;;stage 6, 7

//	;;o0=b0+b2
	buf2[0] = buf1[0]+buf1[2];

//	;;o2=b0-b2
	buf2[2] = buf1[0]-buf1[2];

//	;;o1=b1+b3
	buf2[1] = buf1[1]+buf1[3];

//	;;o3=b1-b3
	buf2[3] = buf1[1]-buf1[3];

//	;;o4=(b4+b6)*c1_16

	buf2[4] = (((buf1[4]+buf1[6])*c1_16)>>14);

//	;;o6=(b4-b6)*c7_16

	buf2[6] = (((buf1[4]-buf1[6])*c7_16)>>14);

//	;;o5=(b5+b7)*c3_16

	buf2[5] = (((buf1[5]+buf1[7])*c3_16)>>14);

//	;;o7=(b5-b7)*c5_16

	buf2[7] = (((buf1[5]-buf1[7])*c5_16)>>14);

//;;;stage 8

//	;;b0=o0+o4

	pwSource[0] = (WORD)((buf2[0]+buf2[4])>>1); 

//	;;b7=o0-o4

	pwSource[Index[7]] = (WORD)((buf2[0]-buf2[4])>>1);

//	;;b1=o1+o5

	pwSource[Index[1]] = (WORD)((buf2[1]+buf2[5])>>1);

//	;;b6=o1-o5

	pwSource[Index[6]] = (WORD)((buf2[1]-buf2[5])>>1);

//	;;b2=o3+o7

	pwSource[Index[2]] = (WORD)((buf2[3]+buf2[7])>>1);

//	;;b5=o3-o7

	pwSource[Index[5]] = (WORD)((buf2[3]-buf2[7])>>1);

//	;;b3=o2+o6

	pwSource[Index[3]] = (WORD)((buf2[2]+buf2[6])>>1);

//	;;b4=o2-o6

	pwSource[Index[4]] = (WORD)((buf2[2]-buf2[6])>>1);
	}
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
