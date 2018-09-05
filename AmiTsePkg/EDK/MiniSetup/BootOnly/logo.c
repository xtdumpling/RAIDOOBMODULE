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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/logo.c $
//
// $Author: Rajashakerg $
//
// $Revision: 26 $
//
// $Date: 9/18/12 12:50a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file logo.c
    file contains code to handle the logo operations.
    Taken from graphics.c to eliminnate library dependancy

**/

#include "minisetup.h"
#include "Protocol/EdidActive.h" 

// to retrieve the frame coordinates in case different frames in a image
UINT16 gGifSrcX=0;
UINT16 gGifSrcY=0;
UINT16 gGifDestX=0;
UINT16 gGifDestY=0;
BOOLEAN GifImageFlag = 0;
extern BOOLEAN gDrawQuietBootMessage;

#define TSE_ABS(a,b)             (((a) < (b)) ? ((b) - (a)) : ((a) - (b)))

extern LOGO_TYPE GetBmpLogoType(UINT8 *ImageData);

extern EFI_STATUS ConvertBmpToUgaBltWrapper (IN VOID *BmpImage, IN UINTN BmpImageSize, IN OUT VOID **UgaBlt,
		IN OUT UINTN  *UgaBltSize, OUT UINTN *PixelHeight, OUT UINTN *PixelWidth );

VOID *ConvertBlt2Bmp ();
VOID AddImageDetailToACPI (UINT8 *, INTN, INTN, BOOLEAN);
UINT8 gAddBgrtResolutions = 0;
VOID SetGetBgrtCoordinates (
	UINTN	*DestX,
	UINTN	*DestY,
	UINTN   *Width,
	UINTN   *Height,
	BOOLEAN Command
	);
VOID InvalidateStatusInBgrtWrapper (VOID);
BOOLEAN IsBGRTSupported(VOID); 
extern void PrintGopAndConInDetails();

LOGO_TYPE GetLogoType(UINT8 *ImageData)
{
	if ( BMP_Logo == GetBmpLogoType(ImageData))
		return BMP_Logo;
	
	return Unsupported_Logo;
}

/**
    function to Clear all the logo buffers.

    @param nil

    @retval void

**/
VOID CleanUpLogo()
{
	if(NULL != gPostscreenwithlogo)
	{
		MemFreePointer( (VOID **)&gPostscreenwithlogo );
	}
	CleanUpExtendedLogoWrapper();
	InvalidateStatusInBgrtWrapper ();			//Setup key detected so clearing the status field
}

/**
    function to set the screen resolutions.

    @param ResX UINTN ResY

    @retval status

**/
EFI_STATUS SetScreenResolution(UINTN ResX, UINTN ResY)
{
    EFI_STATUS Status;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    UINT32  i;
    UINTN   SizeOfInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *pModeInfo;

    if( gGOP == NULL )
        return EFI_UNSUPPORTED;

    for(i=0;i<gGOP->Mode->MaxMode;i++)
    {
        Status = gGOP->QueryMode(gGOP,i,&SizeOfInfo,&pModeInfo);
        if((EFI_SUCCESS == Status) && SizeOfInfo)
        {
            if((ResX == pModeInfo->HorizontalResolution) &&
               (ResY == pModeInfo->VerticalResolution))
            {
                //Set to this mode
                Status = gGOP->SetMode(gGOP,i);
                return Status;
            }
        }
    }

    return EFI_UNSUPPORTED;
#else
	UINT32 ColorDepth, RefreshRate;
    UINTN CurrResX, CurrResY;

    if ( gUgaDraw == NULL )
        return EFI_UNSUPPORTED;

    Status = gUgaDraw->GetMode( gUgaDraw, (UINT32 *)(&CurrResX), (UINT32 *)(&CurrResY), &ColorDepth, &RefreshRate );

    if(EFI_SUCCESS == Status)
        Status = gUgaDraw->SetMode( gUgaDraw, (UINT32)ResX, (UINT32)ResY, ColorDepth, RefreshRate );

    return Status;
#endif
}

/**
    Sets the mode which approximately fits the current image resolution

    @param UINTN *, UINTN * -> Pointer to the width and height of the image to be drawn

    @retval EFI_STATUS -> Status for the operation

**/
EFI_STATUS GOPSetScreenResolution (UINTN *Width, UINTN *Height)
{
	UINT32  i=0, Index = 0;
	UINTN 	DiffXY = 0, ResXY = 0;
	UINTN   SizeOfInfo;
	BOOLEAN ResXYSet = FALSE;
	EFI_STATUS	Status;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *pModeInfo;

	if( NULL == gGOP )
	  return EFI_UNSUPPORTED;//Validate the Gop before usage in all the possible cases and also get instance of Gop through notification
	SETUP_DEBUG_TSE("\n[TSE] GOPSetScreenResolution Entering \n");
	SETUP_DEBUG_TSE("\n[TSE] At Entering : \n");
	PrintGopAndConInDetails();
	//Find proper mode to fit image
	SETUP_DEBUG_TSE("\n[TSE] gGOP->Mode->MaxMode ");
	for (i=0;i<gGOP->Mode->MaxMode;i++)
	{
		Status = gGOP->QueryMode (gGOP,i,&SizeOfInfo,&pModeInfo);
		if ((EFI_SUCCESS == Status) && SizeOfInfo)
		{
			DiffXY = TSE_ABS(*Width , pModeInfo->HorizontalResolution) + TSE_ABS(*Height , pModeInfo->VerticalResolution);
			if ((ResXY || ResXYSet) ? (DiffXY <= ResXY) : (ResXY <= DiffXY))
			{
				Index = i;
				ResXY = DiffXY;
				ResXYSet = TRUE;
			}
		}
	}	
	gGOP->SetMode (gGOP, Index);
	Status = GOPSetScreenResolutionHook (Width, Height, Index);
	SETUP_DEBUG_TSE("\n[TSE] At Exiting : \n");
	PrintGopAndConInDetails();
	SETUP_DEBUG_TSE("\n[TSE] GOPSetScreenResolution Exiting \n");
	return Status;
}

/**
    function to get the screen resolutions.

    @param ResX UINTN *ResY

    @retval status

**/
EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY)
{
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    if( gGOP == NULL )
        return EFI_UNSUPPORTED;
    *ResX = gGOP->Mode->Info->HorizontalResolution;
    *ResY = gGOP->Mode->Info->VerticalResolution;
    return EFI_SUCCESS;
#else
	UINT32 ColorDepth, RefreshRate;

    if ( gUgaDraw == NULL )
        return EFI_UNSUPPORTED;
    return gUgaDraw->GetMode( gUgaDraw, (UINT32 *)ResX, (UINT32 *) ResY, &ColorDepth, &RefreshRate );
#endif
}

/**
    function to draw a image

    @param ImageData UINTN ImageSize, CO_ORD_ATTRIBUTE Attribute,
    @param CoordinateX INTN CoordinateY, BOOLEAN AdjustScreenResolution,
    @param Width UINTN *Height

    @retval status

**/
EFI_STATUS
DrawImage( 
    IN UINT8 *ImageData,
    IN UINTN ImageSize,
    IN CO_ORD_ATTRIBUTE Attribute,
    IN INTN CoordinateX,
    IN INTN CoordinateY,
    IN BOOLEAN AdjustScreenResolution,
    OUT UINTN *Width,
    OUT UINTN *Height
)
{

    EFI_STATUS	Status;
    UINTN		SizeOfX, SizeOfY;
    UINTN		UgaBltSize=0;
    UINTN		unBufferWidth=0;
    UINTN		LogoType;
    BOOLEAN		Animate = FALSE;

    EFI_UGA_PIXEL	*UgaBlt = NULL;

    *Height = *Width = 0;
    
    SETUP_DEBUG_TSE("\n[TSE] DrawImage() Entering :\n");
    SETUP_DEBUG_TSE("\n[TSE] CoordinateX = %d  CoordinateY = %d  AdjustScreenResolution = %d\n",CoordinateX,CoordinateY,AdjustScreenResolution);
    UpdateGoPUgaDraw();
    Status = GetScreenResolution(&SizeOfX, &SizeOfY);
    SETUP_DEBUG_TSE("\n[TSE] GetScreenResolution() : SizeOfX = %d SizeOfY = %d  Status = %r\n",SizeOfX,SizeOfY, Status);
    if ( EFI_ERROR(Status) )
        return Status;

    LogoType = GetLogoType(ImageData);
    SETUP_DEBUG_TSE("\n[TSE] LogoType = %d\n",LogoType);
    switch(LogoType)
    {

	case BMP_Logo:
		Status = ConvertBmpToUgaBltWrapper(
                                ImageData,
                                ImageSize,
                                (void**)&UgaBlt,
                                &UgaBltSize,
                                Height,
                                Width );
            	unBufferWidth = *Width;
        	break;

        case Unsupported_Logo:
        	default:

		Status = ConvertAdvancedImageToUgaBlt(
                                ImageData,
                                ImageSize,
                               (void**) &UgaBlt,
                                &UgaBltSize,
                                Height,
                                Width,
				&Animate);
		unBufferWidth = *Width;


            break;
    }

    if( EFI_ERROR(Status) )
    {
		SETUP_DEBUG_TSE("\n[TSE] DrawImage() Exiting since conversion of image to Blt fails\n");
//        CleanUpLogo();    //If Jpeg off bmp on, jpeg image unsuccess clearing the drawn bmp image so commented.
        return Status;
    }
    SETUP_DEBUG_TSE("\n[TSE] *Width = %d *Height = %d \n",*Width,*Height);
    if(AdjustScreenResolution)
    {
        //If the image is bigger than the current resolution
        //then try to set resolution to fit the image
        if((*Width > SizeOfX) || (*Height > SizeOfY))
        {
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
			Status = GOPSetScreenResolution (Width, Height);			//Always check to suite the best resolution

#else
            UINT32 NewSizeOfX = 0;
            UINT32 NewSizeOfY = 0;

            if((*Width > 1024) || (*Height > 768))
            {
                // Set to max resolution available ie 1024 x 768
                NewSizeOfX = 1024; NewSizeOfY = 768;
                SETUP_DEBUG_TSE("\n[TSE] NewSizeOfX = 1024; NewSizeOfY = 768\n");
            }
            else if((*Width > 800) || (*Height > 600))
            {
                //Set to 1024 x 768
                NewSizeOfX = 1024; NewSizeOfY = 768;
                SETUP_DEBUG_TSE("\n[TSE] NewSizeOfX = 1024; NewSizeOfY = 768\n");
            }
            else if((*Width > 640) || (*Height > 480))
            {
                //Set to 800 x 600
                NewSizeOfX = 800; NewSizeOfY = 600;
                SETUP_DEBUG_TSE("\n[TSE] NewSizeOfX = 800; NewSizeOfY = 600\n");
            }

            if(NewSizeOfX)
            {
                SetScreenResolution(NewSizeOfX,NewSizeOfY);
            }
#endif
			GetScreenResolution (&SizeOfX,&SizeOfY);
		}
    }
    SETUP_DEBUG_TSE("\n[TSE] GetScreenResolution() after updated : SizeOfX = %d SizeOfY = %d \n",SizeOfX,SizeOfY);
    DrawBltBuffer (UgaBlt, Attribute, *Width, *Height, CoordinateX, CoordinateY, unBufferWidth);
	if(Animate)
	{
		DoLogoAnimate(Attribute,CoordinateX,CoordinateY);
	}
	else
	{
		//MemFreePointer( (VOID **)&ImageData ); //image data will be freed in the calling function.
		MemFreePointer( (VOID **)&UgaBlt );
    }
	SETUP_DEBUG_TSE("\n[TSE] DrawImage() Exiting :\n");
	return EFI_SUCCESS;
}

/**
    function to draw a progress bar

    @param nil

    @retval void

**/
VOID DrawBltProgressBar()
{
#ifndef STANDALONE_APPLICATION

	UINTN x = gProgress->x ;
    UINTN y = gProgress->y ;
	UINTN w = gProgress->w ;
    UINTN h = gProgress->h ;
	UINTN delta = gProgress->delta ;
	gProgress->completed+=delta ;

	if(gProgress->quiteBootActive && (gProgress->completed <= w))
	{
		DrawBlock(x, y, w, h, gProgress->backgroundColor) ;					//draw background
		DrawBlock(x, y, gProgress->completed, h, gProgress->fillColor) ;	//draw progression
		DrawProgressBarBorder(x, y, w, h, gProgress->borderColor, 2) ; 		//draw border
	}
#endif
}

/**
    function to draw the border of a progress bar

    @param DestX UINTN DestY, UINTN Width, UINTN Height,
    @param BltBuffer UINTN	bw

    @retval void

**/
VOID DrawProgressBarBorder(
    UINTN	x,
    UINTN	y,
	UINTN	w,
    UINTN	h,
	EFI_UGA_PIXEL *BltBuffer,
	UINTN	bw
    )
{
#ifndef STANDALONE_APPLICATION
 	//Top
	DrawBlock(x, y, w, bw, BltBuffer) ;
	//Bottom
	DrawBlock(x, y+h-bw, w, bw, BltBuffer) ;
	//Left
	DrawBlock(x, y, bw, h, BltBuffer) ;
	//Right
	DrawBlock(x+w-bw, y, bw, h, BltBuffer) ;
#endif
}

/**
    function to draw draw a block

    @param DestX UINTN DestY, UINTN	Width, UINTN Height, EFI_UGA_PIXEL *BltBuffer

    @retval void

**/
VOID DrawBlock(
    UINTN	x,
    UINTN	y,
	UINTN	w,
    UINTN	h,
	EFI_UGA_PIXEL *BltBuffer
    )
{
#ifndef STANDALONE_APPLICATION

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    if ( gQuietBoot && gGOP)
#else
    if ( gQuietBoot && gUgaDraw)
#endif
	{
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    gGOP->Blt (
        gGOP,
        BltBuffer,
        EfiBltVideoFill,
        0, 0,
        x, y,
        w, h,
        0 /*BufferWidth * sizeof (EFI_UGA_PIXEL)*/
        );
#else
    gUgaDraw->Blt (
        gUgaDraw,
        BltBuffer,
        EfiBltVideoFill,
        0, 0,
        x, y,
        w, h,
        0 /*BufferWidth * sizeof (EFI_UGA_PIXEL)*/
        );
#endif
	}
#endif
}

/**
    function to draw draw a block

    @param UgaBlt CO_ORD_ATTRIBUTE Attribute,
    @param Width UINTN Height, INTN DestX, INTN DestY,
    @param BufferWidth 

    @retval void

**/
VOID DrawBltBuffer(
    EFI_UGA_PIXEL *UgaBlt,
    CO_ORD_ATTRIBUTE Attribute,
    UINTN   Width,
    UINTN   Height,
    INTN    DestX,
    INTN    DestY,
    UINTN   BufferWidth
    )
{
    UINTN SizeOfX=0, SizeOfY=0;
    UINTN SourceX=0, SourceY=0;

    GetScreenResolution(&SizeOfX,&SizeOfY);
    // In case of gif set the frame position
    if(GifImageFlag) {
    	SourceX = gGifSrcX; 
    	SourceY = gGifSrcY;
    }
    switch(Attribute)
    {
        case CA_AttributeLeftTop:
            //Need to alter DestX and DestY based on the image descriptor.
            if(GifImageFlag) {
            	DestX = SourceX;
            	DestY = SourceY;
            }
            //No need to alter DestX and DestY
        break;

        case CA_AttributeCenterTop:
            DestX = ((INTN)(SizeOfX - Width)) / 2;
            //DestY need not be altered
        break;

        case CA_AttributeRightTop:
            DestX = (INTN)(SizeOfX - Width - DestX);
            //DestY need not be altered
        break;

        case CA_AttributeRightCenter:
            DestX = (INTN)(SizeOfX - Width - DestX);
            DestY = ((INTN)(SizeOfY - Height))/2;
        break;

        case CA_AttributeRightBottom:
            DestX = (INTN)(SizeOfX - Width - DestX);
            DestY = (INTN)(SizeOfY - Height - DestY);
        break;

        case CA_AttributeCenterBottom:
            DestX = ((INTN)(SizeOfX - Width))/2;
            DestY = (INTN)(SizeOfY - Height - DestY);
        break;

        case CA_AttributeLeftBottom:
            //DestX need not be altered
            DestY = (INTN)(SizeOfY - Height - DestY);
        break;

        case CA_AttributeLeftCenter:
            //DestX need not be altered
            DestY = ((INTN)(SizeOfY - Height))/2;
        break;

        case CA_AttributeCenter:
            // in case of gif, set the video display positions based on the image frame position.
            if(GifImageFlag) {
	           DestX = ((INTN)(SizeOfX - Width))/2 + SourceX;
               DestY = ((INTN)(SizeOfY - Height))/2 + SourceY;
            } 
				// Logo should be 40% of sreen dimension
				else if (IsBGRTSupported() && ( Width <= (((UINTN)(40 *SizeOfX))/100) ) && ( Height <= (((UINTN)(40 *SizeOfY))/100) ))
				{
					DestX = ((INTN)(SizeOfX - Width))/2; //Exact center of x-axis
               DestY = (((INTN)(SizeOfY*382))/1000) - ((INTN)Height/2);//Center of logo is 38.2% from the top of screen
				}
				else 
				{
	         	DestX = ((INTN)(SizeOfX - Width))/2;
               DestY = ((INTN)(SizeOfY - Height))/2;
            }

        break;

        default:
            //DestX and DestY remains as it is
        break;
	}

    //Fill the video based on image frame descriptor values
    if( (GifImageFlag)&& (FALSE == gDrawQuietBootMessage)){
        Width = gGifDestX;
        Height = gGifDestY;
    }

    if( DestX < 0 )
    {
        SourceX = (-(DestX));
        DestX = 0;
    }

    if( DestY < 0 )
    {
        SourceY = (-(DestY));
        DestY = 0;
    }

    // Fix to show Logo images correctly when it is bigger then screen resolution.
    if((DestX + (Width - SourceX)) > SizeOfX)
        Width = (SizeOfX - DestX);

    if((DestY + (Height - SourceY)) > SizeOfY)
        Height = (SizeOfY - DestY);

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    if ( gGOP)
	    gGOP->Blt (
	        gGOP,
	        UgaBlt,
	        EfiBltBufferToVideo,
	        SourceX, SourceY,
	        (UINTN)DestX, (UINTN)DestY,
	        Width, Height,
	        BufferWidth * sizeof (EFI_UGA_PIXEL)
	        );
#else
    if ( gUgaDraw)
	    gUgaDraw->Blt (
	        gUgaDraw,
	        UgaBlt,
	        EfiUgaBltBufferToVideo,
	        SourceX, SourceY,
	        (UINTN)DestX, (UINTN)DestY,
	        Width, Height,
	        BufferWidth * sizeof (EFI_UGA_PIXEL)
	        );
#endif
	if (gAddBgrtResolutions)
	{
		SetGetBgrtCoordinates ((UINTN*)&DestX, (UINTN*)&DestY, &Width, &Height, TRUE);	//1 for set the value, Cast for GCC Build 
	}
}

/**
    Converts the Blt in the screen to Bmp buffer

    @param VOID

    @retval VOID *

**/
VOID *ConvertBlt2Bmp (VOID)
{
    BMP_IMAGE_HEADER BmpHeaderStruct = {
		'B',	//  CHAR8         CharB;
		'M',	//  CHAR8         CharM;
		0,		//  UINT32        Size; // Dynamic
		{0,0},	//  UINT16        Reserved[2];
		0x36,	//  UINT32        ImageOffset;
		0x28,	//  UINT32        HeaderSize;
		0,		//  UINT32        PixelWidth;	// Dynamic
		0,		//  UINT32        PixelHeight;	// Dynamic
		1,		//  UINT16        Planes;       // Must be 1
		0x18,	//  UINT16        BitPerPixel;  // 1, 4, 8, or 24
		0,		//  UINT32        CompressionType;
		0,		//  UINT32        ImageSize;    // // Dynamic
		0,		//  UINT32        XPixelsPerMeter;
		0,		//  UINT32        YPixelsPerMeter;
		0,		//  UINT32        NumberOfColors;
		0,		//  UINT32        ImportantColors;

    };
    BMP_IMAGE_HEADER  *BmpBuffer=NULL;
	UINT32 	i=0,j=0;
    UINT8   *Image;
    UINTN   ImageIndex;
    UINTN   AllignWidth = 0;
	UINTN 	SourceX = 0, SourceY = 0;
	UINTN 	Width = 0, Height = 0;
	EFI_STATUS 		Status = EFI_SUCCESS;
	EFI_UGA_PIXEL 	*BltBuffer;

	SetGetBgrtCoordinates (&SourceX, &SourceY, &Width, &Height, FALSE);		//Get the BGRT logo resolutions
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	if(gGOP)
	{
		BltBuffer = EfiLibAllocateZeroPool (Width * Height * sizeof (EFI_UGA_PIXEL));
		if (NULL == BltBuffer)
		{
			return NULL;
		}
	    Status = gGOP->Blt (
	    	gGOP,
	     	BltBuffer,
	      	EfiBltVideoToBltBuffer,
	       	SourceX, SourceY,
	        0, 0,
	        Width, Height,
	        0);
		if (EFI_SUCCESS != Status)
		{
			MemFreePointer ((VOID **)&BltBuffer);
			return NULL;
		}
	}
#else
	if (gUgaDraw)
	{
		BltBuffer = EfiLibAllocateZeroPool (Width * Height * sizeof (EFI_UGA_PIXEL));
		if (NULL == BltBuffer)
		{
			return NULL;
		}
	    Status = gUgaDraw->Blt (
	    	gUgaDraw,
	     	BltBuffer,
	      	EfiUgaVideoToBltBuffer,           
	       	SourceX, SourceY,
	        0, 0,
	        Width, Height,
	        0);
		if (EFI_SUCCESS != Status)
		{
			MemFreePointer ((VOID **)&BltBuffer);
			return NULL;
		}
	}
#endif
	else
	{
		return NULL;
	}
	AllignWidth = Width;
    if (Width%4 != 0)
    {
        AllignWidth += Width%4;					//Width should be 4 bytes alligned
    }
	BmpHeaderStruct.ImageSize = (UINT32)(3*AllignWidth*Height);
	BmpHeaderStruct.Size = sizeof (BmpHeaderStruct)+ BmpHeaderStruct.ImageSize;
	BmpBuffer = EfiLibAllocateZeroPool (BmpHeaderStruct.Size);
	if (BmpBuffer)
	{
		BmpHeaderStruct.PixelWidth = (UINT32)Width;
		BmpHeaderStruct.PixelHeight = (UINT32)Height;

		gBS->CopyMem(BmpBuffer,&BmpHeaderStruct,sizeof(BmpHeaderStruct));
		Image = (UINT8*)BmpBuffer;
		Image += BmpBuffer->ImageOffset;

		for(i=(UINT32)(Width*Height);i;i-=BmpHeaderStruct.PixelWidth)
		{
			for(j=BmpHeaderStruct.PixelWidth;j;j--)
			{
				*Image++ = BltBuffer[i-j].Blue;
				*Image++ = BltBuffer[i-j].Green;
				*Image++ = BltBuffer[i-j].Red;
			}
            ImageIndex = (UINTN) (Image - BmpBuffer->ImageOffset);
            if ((ImageIndex % 4) != 0) // Bmp Image starts each row on a 32-bit boundary!
                Image = Image + (4 - (ImageIndex % 4));
		}
		MemFreePointer ((VOID **)&BltBuffer);
        return (VOID*)BmpBuffer;
	}
	MemFreePointer ((VOID **)&BltBuffer);
	return NULL;
}

/**
    Converts Blt to Bmp buffer and form BGRT table and add to ACPI.

    @param BOOLEAN  = Indicates whether to set the BGRT status field or not.

    @retval VOID

**/
VOID ConvertBmpandAddBGRT (
	BOOLEAN GifImagePresence
    )
{
    VOID *BmpData = NULL;
    BmpData = ConvertBlt2Bmp ();
    if (BmpData)
    {
		UINTN	DestX = 0, DestY = 0, Width = 0, Height = 0;
		SetGetBgrtCoordinates (&DestX, &DestY, &Width, &Height, FALSE); //Get the Bgrt logo resolution
   	    AddImageDetailToACPI (BmpData, DestX, DestY, GifImagePresence);
	}
}

/**
    Sets the BGRT logo coordinates

    @param DestX = X coordinate
    @param DestY = Y coordinate
    @param Width = Width of the logo
    @param Height = Heigth of the logo
    @param Command = To set or get the Bgrt logo resolutions

    @retval VOID

**/
VOID SetGetBgrtCoordinates (
	UINTN	*DestX,
	UINTN	*DestY,
	UINTN   *Width,
	UINTN   *Height,
	BOOLEAN Command
	)
{
	static UINTN gBgrtStartResX = 0;
	static UINTN gBgrtStartResY = 0;
	static UINTN gBgrtEndResX = 0;
	static UINTN gBgrtEndResY = 0;

	if (FALSE == Command)			//To get the Bgrt logo resolution
	{
		*DestX = gBgrtStartResX;
		*DestY = gBgrtStartResY;
		*Width = gBgrtEndResX - gBgrtStartResX;
		*Height = gBgrtEndResY - gBgrtStartResY;
	}
	else
	{
		if (1 == gAddBgrtResolutions)			//At first time initialize the values
		{
			gBgrtStartResX = *DestX;
			gBgrtStartResY = *DestY;
			gBgrtEndResX = *DestX + *Width;
			gBgrtEndResY = *DestY + *Height;
		}
		else
		{
			if (*DestX < gBgrtStartResX)
			{
				gBgrtStartResX = *DestX;
			}
			if (*DestY < gBgrtStartResY)
			{
				gBgrtStartResY = *DestY;
			}
			if (gBgrtEndResX < (*DestX + *Width))
			{
				gBgrtEndResX = (*DestX + *Width);
			}
			if (gBgrtEndResY < (*DestY + *Height))
			{
				gBgrtEndResY = (*DestY + *Height);
			}
		}
		gAddBgrtResolutions ++;
	}
}

/**
    Sets the graphical resolution corresponding to the input parameter

    @param UINT32 - Horizontal resolution of the mode to set
        UINT32 - Vertical resolution of the mode to set

    @retval EFI_STATUS EFI_SUCCESS - If input resolutions are set successfully
        !EFI_SUCCESS - If input resolutions are not set successfully

**/
EFI_STATUS SetGraphicsResolution (UINT32 HorResolution, UINT32 VerResolution)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT32  		iIndex = 0;
	UINTN   		SizeOfInfo = 0;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *pModeInfo = NULL;

	for (iIndex = 0; iIndex < gGOP->Mode->MaxMode; iIndex ++)
	{
		Status = gGOP->QueryMode (gGOP, iIndex, &SizeOfInfo, &pModeInfo);
		if (!EFI_ERROR (Status))
		{
			SETUP_DEBUG_TSE ("\n[TSE] pModeInfo HorizontalResolution=%d VerticalResolution=%d\n", pModeInfo->HorizontalResolution, pModeInfo->VerticalResolution);
			if ((HorResolution == pModeInfo->HorizontalResolution) && (VerResolution == pModeInfo->VerticalResolution))
			{
				if (gGOP->Mode->Mode != iIndex)			//If Current mode not equals the found mode then call SetMode otherwise return success
				{
					SETUP_DEBUG_TSE ("\n[TSE] Setting %d Mode\n", iIndex);
					Status = gGOP->SetMode (gGOP, iIndex);
					if (!EFI_ERROR (Status))				//Loop for all the cases. Not success also loop for another without returning from here
					{
						return EFI_SUCCESS;
					}
				}
				else
				{
					SETUP_DEBUG_TSE ("\n[TSE] Without Setting %d Mode\n", iIndex);
					return EFI_SUCCESS;
				}
			}
		}
	}
	return EFI_NOT_FOUND;
}

/**
    Finds and sets the resolution from EDID

    @param VOID

    @retval VOID

**/
EFI_STATUS SetNativeResFromEdid (VOID)
{
	EFI_STATUS 	Status;
	UINT32 			HorRes;
	UINT32 			VerRes;
	EFI_EDID_ACTIVE_PROTOCOL *EdidActive;
	EFI_GUID EdidActiveProtocolGuid = EFI_EDID_ACTIVE_PROTOCOL_GUID;
	SETUP_DEBUG_TSE ("\n[TSE] SetNativeResFromEdid() Entering :\n");
	Status = gBS->LocateProtocol (&EdidActiveProtocolGuid, NULL, (void**)&EdidActive);
	SETUP_DEBUG_TSE ("\n[TSE] LocateProtocol (&EdidActiveProtocolGuid) : Status = %r\n",Status);
	if(EFI_ERROR(Status) || EdidActive->SizeOfEdid == 0)
	{
		SETUP_DEBUG_TSE ("\n[TSE] Edid active protocol fails \n");
		Status = SetGraphicsResolution (1024, 768);			//If EDID not found set 1024*768 mode
		SETUP_DEBUG_TSE ("\n[TSE] SetGraphicsResolution (1024, 768) : Status = %r\n",Status);
		SETUP_DEBUG_TSE ("\n[TSE] SetNativeResFromEdid() Exiting :\n");
		return Status;
	}

	if(!((EdidActive->Edid)[24] & BIT01))
	{
	/* native resolution not supported */
		SETUP_DEBUG_TSE ("\n[TSE] native resolution not supported\n");
		SETUP_DEBUG_TSE ("\n[TSE] SetNativeResFromEdid() Exiting :\n");
		return EFI_UNSUPPORTED;
	}
	
	HorRes = ((((EdidActive->Edid)[0x36 + 4]) & 0xF0) << 4) + (EdidActive->Edid)[0x36 + 2];
	VerRes = ((((EdidActive->Edid)[0x36 + 7]) & 0xF0) << 4) + (EdidActive->Edid)[0x36 + 5];

	SETUP_DEBUG_TSE ("\n[TSE] %02d, ", EdidActive->Edid[0x36 + 4]);
	SETUP_DEBUG_TSE ("\n[TSE] %02d, ", EdidActive->Edid[0x36 + 2]);
	SETUP_DEBUG_TSE ("\n[TSE] %02d, ", EdidActive->Edid[0x36 + 7]);
	SETUP_DEBUG_TSE ("\n[TSE] %02d, ", EdidActive->Edid[0x36 + 5]);
	SETUP_DEBUG_TSE ("\n[TSE] EDID HorizontalResolution=%d VerticalResolution=%d\n", HorRes, VerRes);

	Status = SetGraphicsResolution (HorRes, VerRes);
	SETUP_DEBUG_TSE ("\n[TSE] After SetGraphicsResolution : Status = %r HorRes = %d  VerRes = %d ", Status,  HorRes, VerRes );
	if (EFI_ERROR(Status))
	{
		SETUP_DEBUG_TSE ("\n[TSE] Setting 1024 * 768 Mode \n");
		Status = SetGraphicsResolution (1024, 768);			//If EDID not found set 1024*768 mode
#if OVERRIDE_MainSetupLoopHook
		if (EFI_ERROR(Status)) {
			SETUP_DEBUG_TSE ("\n[TSE] Setting 800x600 Mode \n");
			Status = SetGraphicsResolution (800, 600);		//If setting to 1024x768 fails
		}
#endif
	}
	SETUP_DEBUG_TSE ("\n[TSE] SetNativeResFromEdid() Exiting :\n");
	return Status;
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

