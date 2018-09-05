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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/screen.c $
//
// $Author: Madhans $
//
// $Revision: 5 $
//
// $Date: 9/16/10 8:38p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file screen.c
    This file contains code for screen operations.

**/

#include "minisetup.h"

VOID MouseStop(VOID);
VOID MouseRefresh(VOID);
BOOLEAN IsMouseSupported(VOID);
BOOLEAN IsSoftKbdSupported(VOID);

/**
    function to clear screen without flushing.

    @param Attrib 

    @retval void

**/
VOID ClearScreenWithoutFlush( UINT8 Attrib )
{
	if ( NULL != gActiveBuffer )
	{
		gActiveBuffer->ScreenTag = ACTIVE_SCREEN_TAG;
		MemSet( gActiveBuffer->Attribs, sizeof(gActiveBuffer->Attribs), Attrib );

		MemFillUINT16Buffer( gActiveBuffer->Chars, MAX_DIMENSIONS, (UINT16)L' ' );
	}
}

/**
    function to clear lines without flushing.

    @param LineStart UINT8 NumLines, UINT8 Attrib

    @retval void

**/
VOID ClearLinesWithoutFlush( UINT16 LineStart, UINT16 NumLines, UINT8 Attrib )
{

	gActiveBuffer->ScreenTag = ACTIVE_SCREEN_TAG;
	MemSet(&( gActiveBuffer->Attribs[LineStart*gMaxCols]), NumLines*gMaxCols /*sizeof(gActiveBuffer->Attribs)*/, Attrib );

	MemFillUINT16Buffer( &(gActiveBuffer->Chars[LineStart*gMaxCols]), NumLines*gMaxCols/*MAX_ROWS * MAX_COLS*/, (UINT16)L' ' );
}

/**
    function to clear the screen.

    @param Attrib 

    @retval void

**/
VOID ClearScreen( UINT8 Attrib )
{
	ClearScreenWithoutFlush( Attrib );

	if(gST->ConOut!=NULL)
	{
	    if(!IsWriteCharLastRowColSupport())
	    {    
	        gST->ConOut->EnableCursor( gST->ConOut, FALSE );
	        gST->ConOut->SetAttribute( gST->ConOut, Attrib );
	        gST->ConOut->ClearScreen( gST->ConOut);
	    }
	}

	FlushLines( 0, gMaxRows - 1 );

}  

/**
    function to clear the FlushBuffer.

    @param Attrib 

    @retval void

**/
VOID ClearFlushBuffer(UINT8 Attrib)
{
	if(gFlushBuffer)
		MemSet(gFlushBuffer, sizeof(SCREEN_BUFFER), Attrib);
}

EFI_UGA_PIXEL *gPostscreenwithlogo;
/**
    function to get the backup of the screen.

    @param void

    @retval void

**/
VOID *SaveScreen( VOID )
{
	SCREEN_BUFFER *Buffer = (SCREEN_BUFFER*)NULL;

	Buffer = EfiLibAllocatePool( sizeof(SCREEN_BUFFER) );

	if ( Buffer != NULL )
	{
		MemCopy( Buffer, gActiveBuffer, sizeof(SCREEN_BUFFER) );
		Buffer->ScreenTag = SAVE_SCREEN_TAG;
	}

	return (VOID *)Buffer;
}

/**
    function to restore the screen from the saved screen buffer.

    @param ScreenBuffer 

    @retval void

**/
VOID RestoreScreen( VOID *ScreenBuffer )
{
	SCREEN_BUFFER *Buffer = (SCREEN_BUFFER *)ScreenBuffer;

	MouseStop();

	if ( ( Buffer == NULL ) || ( Buffer->ScreenTag != SAVE_SCREEN_TAG ) )
		return;

	///Clear Original data in the buffers before restoring with saved screen buffer.
	if (NULL != gActiveBuffer)
		MemSet(gActiveBuffer, sizeof(SCREEN_BUFFER), 0);
	if (NULL != gFlushBuffer)
		MemSet(gFlushBuffer, sizeof(SCREEN_BUFFER), 0);
	
	if (NULL != gFlushBuffer)
		MemCopy( gActiveBuffer, Buffer, sizeof(SCREEN_BUFFER) );

		gActiveBuffer->ScreenTag = ACTIVE_SCREEN_TAG;

	FlushLines( 0, gMaxRows - 1 );
	DoRealFlushLines();

	MemFreePointer( (VOID **)&Buffer );

	MouseRefresh();
}

/**
    Saves post graphics screen

    @param VOID

    @retval VOID

**/
VOID SaveGraphicsScreen(VOID)
{
	   UINTN HorizontalResolution = 0;
 	   UINTN VerticalResolution = 0;
 	   SETUP_DEBUG_TSE("\n[TSE] SaveGraphicsScreen Entering :\n");
		GetScreenResolution(&HorizontalResolution, &VerticalResolution);
		SETUP_DEBUG_TSE("\n[TSE] HorizontalResolution = %d  VerticalResolution = %d\n",HorizontalResolution, VerticalResolution);
		
		if(NULL != gPostscreenwithlogo)
		{
			MemFreePointer( (VOID **)&gPostscreenwithlogo );
		}
		
		gPostscreenwithlogo = EfiLibAllocateZeroPool( HorizontalResolution * VerticalResolution * sizeof(EFI_UGA_PIXEL)); 

		if (NULL == gPostscreenwithlogo)
		{
		 	SETUP_DEBUG_TSE("\n[TSE] Not able to save screen since gPostscreenwithlogo = NULL\n");
			return;
		}
		 MouseStop();
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
    if ( gGOP){
	    gGOP->Blt (
	        gGOP,
	        gPostscreenwithlogo,
	        EfiBltVideoToBltBuffer,
	       (UINTN)0, (UINTN)0,
			 (UINTN)0, (UINTN)0,
	       HorizontalResolution, VerticalResolution,
	       0
	        );
	SETUP_DEBUG_TSE("\n[TSE] Screen Saved\n");
	}
#else
    if ( gUgaDraw){
	    gUgaDraw->Blt (
	        gUgaDraw,
	        gPostscreenwithlogo,
	        EfiUgaVideoToBltBuffer,
	       (UINTN)0, (UINTN)0,
			 (UINTN)0, (UINTN)0,
	       HorizontalResolution, VerticalResolution,
	       0
	        );
	SETUP_DEBUG_TSE("\n[TSE] Screen Saved\n");
	}
#endif
      MouseRefresh();
 	  SETUP_DEBUG_TSE("\n[TSE] SaveGraphicsScreen Exiting :\n");
}

/**
    Restores the saved post graphics screen

    @param VOID

    @retval VOID

**/
VOID RestoreGraphicsScreen (VOID)
{
	SETUP_DEBUG_TSE ("\n[TSE] RestoreGraphicsScreen Entering :\n");
   if( gPostscreenwithlogo != NULL )
   {
      UINTN HorizontalResolution = 0;
      UINTN VerticalResolution = 0;
      
      GetScreenResolution(&HorizontalResolution, &VerticalResolution);
      SETUP_DEBUG_TSE ("\n[TSE] HorizontalResolution = %d  VerticalResolution = %d \n",HorizontalResolution, VerticalResolution);
      MouseStop();
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
      if ( gGOP){
         gGOP->Blt (
         gGOP,
         gPostscreenwithlogo,
         EfiBltBufferToVideo,
         (UINTN)0, (UINTN)0,
         (UINTN)0, (UINTN)0,
         HorizontalResolution, VerticalResolution,
         0
      );
	SETUP_DEBUG_TSE ("\n[TSE] Screen Restored\n",gGOP);
	}
#else
      if ( gUgaDraw){
         gUgaDraw->Blt (
         gUgaDraw,
         gPostscreenwithlogo,
         EfiUgaBltBufferToVideo,
         (UINTN)0, (UINTN)0,
         (UINTN)0, (UINTN)0,
         HorizontalResolution, VerticalResolution,
         0
      );
	SETUP_DEBUG_TSE ("\n[TSE] Screen Restored\n",gGOP);
	}
#endif
      MouseRefresh();
//  Memory will be freed during CleanUpLogo()
//      MemFreePointer( (VOID **)&gPostscreenwithlogo );
   }
	else
		SETUP_DEBUG_TSE ("\n[TSE] Not able to restore screen\n",gGOP);
   
   SETUP_DEBUG_TSE ("\n[TSE] RestoreGraphicsScreen Exiting :\n");
}

/**
    function to update the TSE global variables and screen.

    @param void

    @retval void

**/
VOID UpdateScreen( VOID )
{
	EFI_STATUS Status;
	UINTN ConCols,ConRows;
	UINT32 HorizontalResolution = 0;
	UINT32 VerticalResolution = 0;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL	
	if(gGOP && ((TSE_BEST_HORIZONTAL_RESOLUTION > gGOP->Mode->Info->HorizontalResolution) || (TSE_BEST_VERTICAL_RESOLUTION > gGOP->Mode->Info->VerticalResolution)))
	{
		SetScreenResolution(TSE_BEST_HORIZONTAL_RESOLUTION,TSE_BEST_VERTICAL_RESOLUTION);
	}
#else
	if ( gUgaDraw != NULL )
	{
		gUgaDraw->GetMode (gUgaDraw, &HorizontalResolution, &VerticalResolution, &ColorDepth, &RefreshRate);
		if((TSE_BEST_HORIZONTAL_RESOLUTION > HorizontalResolution)||(TSE_BEST_VERTICAL_RESOLUTION > VerticalResolution))
		{
			SetScreenResolution(TSE_BEST_HORIZONTAL_RESOLUTION,TSE_BEST_VERTICAL_RESOLUTION);
		}
	}
#endif	
	
	if(gConsoleControl != NULL)
	{
		if(TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
		{
			gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenGraphics );
		}
		else
		{
			gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenText );
		}
		
	}	
	
	if(IsMouseSupported())
	{
		if(((gPostStatus < TSE_POST_STATUS_IN_TSE)&&IsSoftKbdSupported())||
				(gPostStatus >= TSE_POST_STATUS_IN_TSE))
		{
			MouseInit();
		}
	}
	
	if(gST && gST->ConOut)
	{
		Status = gST->ConOut->QueryMode( gST->ConOut, gST->ConOut->Mode->Mode, &ConCols, &ConRows);
		if (EFI_ERROR(Status))
			return ;

		if ((STYLE_STD_MAX_ROWS > ConRows )  || (STYLE_STD_MAX_COLS > ConCols))
		{
			gMaxRows = StyleGetStdMaxRows();
			gMaxCols = StyleGetStdMaxCols(); 
			SetDesiredTextMode ();
		}
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
