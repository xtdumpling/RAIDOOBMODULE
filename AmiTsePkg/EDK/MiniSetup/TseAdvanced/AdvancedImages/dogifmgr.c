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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/dogifmgr.c $
//
// $Author: Madhans $
//
// $Revision: 9 $
//
// $Date: 3/29/11 6:59p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file dogifmgr.c
    This file contains code for gif image handling

**/

#include "minisetup.h"

//#if SETUP_GIF_LOGO_SUPPORT
//static EFI_EVENT	NextFrameEvent;
//static EFI_UGA_PIXEL	*UgaBltForGif = NULL;
//static UINT8		*ImageDataForGif = NULL;
//static CO_ORD_ATTRIBUTE GifAttribute;
//static UINTN            GifWidth, GifHeight, GifX, GifY, GifBufferWidth;
static UINTN 		    gGifDelay, animateDelay;
//#endif

extern unsigned long dofirst( unsigned char *, unsigned long );
extern unsigned long donext( unsigned char * );
extern BOOLEAN GifImageFlag;
UINT64 EFIAPI RShiftU64 (
  	IN UINT64   Operand,
  	IN UINTN    Count
  	);

typedef struct                  //Structure to preserve the various gif images data
{
    UINTN           GifWidth;
    UINTN           GifHeight;
    UINTN           GifX;
    UINTN           GifY;
    UINTN           GifBufferWidth;
    EFI_EVENT       GifEvent;
    EFI_UGA_PIXEL   *UgaBltForGif;
    CO_ORD_ATTRIBUTE GifAttribute;
    unsigned char   *GifCurBufLocation;
    unsigned char 	*ImageBuffer;
    UINTN           ImageSize;
    unsigned long 	FistFrameAddress;
} GIF_ANIMATE_DATA;

struct GIF_IMAGE_DATAS          //Structure used to cleanup the logo buffers
{
	GIF_ANIMATE_DATA        *GifAnimateData;
	struct GIF_IMAGE_DATAS	*Next;
};

struct GIF_IMAGE_DATAS  *GifEntries;
GIF_ANIMATE_DATA        *GifAnimateData;
extern unsigned char    *pcurbuf;
extern unsigned long firstframeadd;     
void SaveCleanUpGifDatas (GIF_ANIMATE_DATA **GifDatas);

/**

    @param UgaBlt UINTN *GifDelay

    @retval EFI_STATUS

**/

EFI_STATUS
GetGifNextFrame(IN OUT VOID **GifImageData, OUT UINTN *GifDelay)
{
	GIF_ANIMATE_DATA    **GifData;
	EFI_TPL 		  OldTpl;

	GifData = (GIF_ANIMATE_DATA    **)GifImageData;

	OldTpl = gBS->RaiseTPL(EFI_TPL_NOTIFY);
	dofirst ((*GifData)->ImageBuffer, (unsigned long)(*GifData)->ImageSize);
	while (pcurbuf != (*GifData)->GifCurBufLocation)
	{
		*GifDelay = donext ((unsigned char *)((*GifData)->UgaBltForGif));
	}
	*GifDelay = donext ((unsigned char *)((*GifData)->UgaBltForGif));
	(*GifData)->GifCurBufLocation = pcurbuf;
	gBS->RestoreTPL (OldTpl);
	return EFI_SUCCESS;
}

/**
    function to get the next frame.

    @param Event VOID *Context

    @retval void

**/

VOID NextFrame(IN EFI_EVENT Event, VOID *Context)
{
	EFI_STATUS      Status = EFI_UNSUPPORTED;
    GIF_ANIMATE_DATA      *GifContextData = (GIF_ANIMATE_DATA *)Context;

    if (NULL == GifContextData)
	{                               //Add clean up function
	   return;
    }
	if (gPostStatus > TSE_POST_STATUS_IN_BOOT_TIME_OUT)// if boot timeout is occured then we have to clean the logo and return
	{
		CleanUpLogo ();
		return;
	}
	if (!GifImageFlag) 
	{
		TimerStopTimer (&(GifAnimateData->GifEvent));
		return;
	}

	GetGifNextFrame ((void**)&GifContextData, &gGifDelay); 
	if (gGifDelay >= 0)
	{
        DrawBltBuffer (GifContextData->UgaBltForGif, GifContextData->GifAttribute, GifContextData->GifWidth, GifContextData->GifHeight, GifContextData->GifX, GifContextData->GifY, GifContextData->GifBufferWidth);
        //DrawBltBuffer(UgaBltForGif, GifAttribute, GifWidth, GifHeight, GifX, GifY, GifBufferWidth);
        Status = gBS->SetTimer (GifContextData->GifEvent, TimerRelative, gGifDelay*10000);
        if (EFI_ERROR (Status))
            TimerStopTimer (&(GifAnimateData->GifEvent));
	}
	else
	{
		CleanUpLogo ();
	}

	if ((0 == gGifDelay) && (gPostStatus <= TSE_POST_STATUS_IN_BOOT_TIME_OUT))// checking gGifDelay and boot timeout to assing the first frame address to the current buffer pointer
	{
		GifContextData->GifCurBufLocation = (unsigned char *)GifContextData->FistFrameAddress;
	}
	return;
}

/**
    function to Handle gif logo animation

    @param Attribute 
    @param CoordinateX INTN CoordinateY

    @retval VOID

**/

VOID DoGifAnimate(CO_ORD_ATTRIBUTE Attribute,INTN CoordinateX,INTN CoordinateY)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;

    if (animateDelay)
    {
        GifAnimateData->GifX = CoordinateX;
        GifAnimateData->GifY = CoordinateY;
        GifAnimateData->GifAttribute = Attribute;
        
        Status = gBS -> CreateEvent (
                            EFI_EVENT_TIMER|EFI_EVENT_NOTIFY_SIGNAL,
                            EFI_TPL_NOTIFY,
                            &NextFrame,
                            GifAnimateData,                 //NULL
                            &GifAnimateData->GifEvent);     //NextFrameEvent
        if (gEnterSetup == TRUE && !(EFI_ERROR (Status))) 
        {
            CleanUpLogo();
            return;
        }
        Status = gBS->SetTimer (GifAnimateData->GifEvent, TimerRelative, animateDelay*10000);
        if (EFI_ERROR (Status)) {
            TimerStopTimer (&(GifAnimateData->GifEvent));
        }
    }
}

/**
    Convert a *.GIF graphics image to a UGA blt buffer. If a 
    NULL UgaBlt buffer is passed in a UgaBlt buffer will be 
    allocated by this routine. If a UgaBlt buffer is passed in 
    it will be used if it is big enough.

    @param GifImage Pointer to GIF file
    @param GifImageSize Number of bytes in BmpImage
    @param UgaBlt Buffer containing UGA version of GifImage.
    @param UgaBltSize Size of UgaBlt in bytes.
    @param PixelHeight Height of UgaBlt/BmpImage in pixels
    @param PixelWidth Width of UgaBlt/BmpImage in pixels
    @param GifDelay 

    @retval EFI_SUCCESS UgaBlt and UgaBltSize are returned. 
    @retval EFI_UNSUPPORTED GifImage is not a valid *.GIF image
    @retval EFI_BUFFER_TOO_SMALL The passed in UgaBlt buffer is not big enough.
        UgaBltSize will contain the required size.
    @retval EFI_OUT_OF_RESOURCES No enough buffer to allocate

**/

EFI_STATUS
ConvertGifToUgaBlt (
  IN  VOID      *GifImage,
  IN  UINTN     GifImageSize,
  IN OUT VOID   **UgaBlt,
  IN OUT UINTN  *UgaBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth,
  OUT UINTN     *GifDelay
  )
{
	UINT64      BltBufferSize;
	UINTN       Height;
	UINTN       Width;
    EFI_TPL     OldTpl;
    unsigned char   *TempCurBuf = NULL;
    unsigned long 	tempfirstframeadd;

//	unsigned long 	tempstartunreadbuf;

    OldTpl = gBS->RaiseTPL (EFI_TPL_NOTIFY);            // prepare BLT buffer for the GIF image
	BltBufferSize = dofirst (GifImage, (unsigned long)GifImageSize);
	gBS->RestoreTPL (OldTpl);

	Width = (UINTN)RShiftU64(BltBufferSize,16);
	Height = (UINTN)(BltBufferSize & 0xffff);

	// Make sure to do x64 Mul
	BltBufferSize = MultU64x32((UINT64)Width, (UINT32)Height);

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
		*UgaBlt = EfiLibAllocatePool (*UgaBltSize);
    		if (*UgaBlt == NULL) return EFI_OUT_OF_RESOURCES;
    } 
	else 
	{
		if (*UgaBltSize < (UINTN)BltBufferSize) 
		{
			*UgaBltSize = (UINTN)BltBufferSize;
				return EFI_BUFFER_TOO_SMALL;
		}    
	}

 	//corrected the assignment.	
 	*PixelWidth   = Width;
	*PixelHeight  = Height;
    OldTpl = gBS->RaiseTPL(EFI_TPL_NOTIFY);
    BltBufferSize = dofirst (GifImage, (unsigned long)GifImageSize);
    tempfirstframeadd = firstframeadd;                                  //Using temp variable because call to allocatepool 
    gGifDelay = *GifDelay = donext ((unsigned char *)(*UgaBlt));        //may lead to trigger the previous gif events so pcurbuf value might change
    TempCurBuf = pcurbuf;
    gBS->RestoreTPL (OldTpl);
    if (*GifDelay)
	{
        GifAnimateData = (GIF_ANIMATE_DATA *)EfiLibAllocateZeroPool (sizeof (GIF_ANIMATE_DATA));
	    if (NULL == GifAnimateData)
	    {
		    return EFI_OUT_OF_RESOURCES;
	    }
        GifAnimateData->ImageBuffer = GifImage;
        GifAnimateData->ImageSize = GifImageSize;
        GifAnimateData->FistFrameAddress = tempfirstframeadd;
        GifAnimateData->GifWidth = Width;
        GifAnimateData->GifHeight = Height;
        GifAnimateData->GifBufferWidth = Width;
        GifAnimateData->UgaBltForGif = *UgaBlt;
        GifAnimateData->GifCurBufLocation = TempCurBuf;
	    GetGifNextFrame ((void **)&GifAnimateData, &gGifDelay);
        animateDelay = gGifDelay;
        SaveCleanUpGifDatas (&GifAnimateData);
	}
	return EFI_SUCCESS;
}

/**
    Cleans the gif datas

    @param void

    @retval void

**/

VOID CleanUpGif (VOID)
{
	GifImageFlag= FALSE;
    while (NULL != GifEntries)
    {
        gBS->CloseEvent (GifEntries->GifAnimateData->GifEvent);
    	MemFreePointer( (VOID **)&GifEntries->GifAnimateData->UgaBltForGif);
    	MemFreePointer( (VOID **)&GifEntries->GifAnimateData->ImageBuffer);   
        GifEntries = GifEntries->Next;    
    }
}
/**
    Saves the structures used for preserving the gif images
					

    @param GifDatas Pointer to a pointer for GIF image structure

    @retval void

**/

void SaveCleanUpGifDatas (GIF_ANIMATE_DATA **GifDatas)
{
    struct GIF_IMAGE_DATAS *HoldGifData = NULL, *tempGifEntries = NULL;
    
    HoldGifData = (struct GIF_IMAGE_DATAS *)EfiLibAllocateZeroPool (sizeof (struct GIF_IMAGE_DATAS));
    if (NULL == HoldGifData)
    {
        return;
    }
    HoldGifData->GifAnimateData = *GifDatas;
    HoldGifData->Next = NULL;
    tempGifEntries = GifEntries;
    if (NULL == tempGifEntries)
    {
        GifEntries = HoldGifData; 
    }
    else
    {
        while (NULL != tempGifEntries->Next)
        {
            tempGifEntries = tempGifEntries->Next;
        }
        tempGifEntries->Next = HoldGifData;
    }                
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
