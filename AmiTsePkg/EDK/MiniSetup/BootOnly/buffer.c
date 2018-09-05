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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/buffer.c $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 1/18/12 4:55a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file buffer.c
    This file contains code to manage buffers

**/

#include "minisetup.h"
extern BOOLEAN CompleteReDrawFlag; 
VOID GetCoordinates(INT32 *x, INT32 *y, INT32 *z);
INT32 GetactualScreentop(VOID);
BOOLEAN IsMouseSupported(VOID);
BOOLEAN IsMouseOnSoftkbd(VOID);
BOOLEAN IsCharacteronSoftKbd(UINT32 Left,UINT32 TOP);
BOOLEAN IsSoftKbdonCurrentRow(UINT32 TOP);

/**
    function to initialize the screen buffer

    @param attrib 

    @retval status

**/
EFI_STATUS InitializeScreenBuffer( UINT8 attrib )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;
	
	if (NULL != gFlushBuffer)
		MemFreePointer( (VOID **)&gFlushBuffer );

	gFlushBuffer = EfiLibAllocateZeroPool( sizeof(SCREEN_BUFFER) );
	if ( gFlushBuffer == NULL )
		return Status;

	if (NULL != gActiveBuffer)
		MemFreePointer( (VOID **)&gActiveBuffer );

	gActiveBuffer = EfiLibAllocatePool( sizeof(SCREEN_BUFFER) );
	if ( gActiveBuffer != NULL )
	{
		ClearScreenWithoutFlush( attrib );
		Status = EFI_SUCCESS;
	}

	return Status;
}

/**
    function to slow flush lines

    @param Top UINTN Bottom

    @retval void

**/
VOID SlowFlushLines( UINTN Top, UINTN Bottom )
{
	UINTN	Index;
	UINTN	Offset = Top * gMaxCols;
	UINTN	End = (Bottom - Top + 1) * gMaxCols + Offset;
	UINT8	LastAttrib = 0xFF;
	CHAR16	String[2] = { 0, 0 };

	gST->ConOut->SetCursorPosition( gST->ConOut, 0, Top );
	for ( Index = Offset; Index < End; Index++ )
	{
		if ( gActiveBuffer->Attribs[Index] != LastAttrib )
		{
			LastAttrib = gActiveBuffer->Attribs[Index];
			gST->ConOut->SetAttribute( gST->ConOut, LastAttrib );
		}
		*String = gActiveBuffer->Chars[Index];

		gST->ConOut->OutputString( gST->ConOut, String );
	}
}

#define INVALID_FLUSH_LINE 0xFFFF
UINTN	FlushTop=INVALID_FLUSH_LINE;
UINTN	FlushBottom=INVALID_FLUSH_LINE;
/**
    function to set the positions for flush lines

    @param Top UINTN Bottom

    @retval void

**/
VOID FlushLines( UINTN Top, UINTN Bottom )
{
	if( FlushTop==INVALID_FLUSH_LINE) 
		FlushTop = Top;

	if(FlushBottom==INVALID_FLUSH_LINE)
		FlushBottom = Bottom;

	if(FlushTop>Top)
		FlushTop = Top;

	if(FlushBottom < Bottom)
		FlushBottom = Bottom;
}
/**
    function to flush lines

    @param NIL

    @retval void

**/
VOID DoRealFlushLines()
{
	UINTN	Index;
	UINTN	ByteStart, ByteEnd;
	UINTN	Offset = 0;
	UINTN	End = 0;
	UINT8	LastAttrib,LastSetAttrib=0;
	CHAR16	*String;
	UINTN	X=0,Y=0,TempOffSet,PrintLen;
	UINTN	CurrentX=0;
	BOOLEAN	FlushIt;
	UINTN	Top, Bottom;
	INT32  MousePointerX=0,MousePointerY=0,MousePointerZ=0;
	UINTN glyphHeight = HiiGetGlyphHeight();
	UINTN screenTop = GetactualScreentop();

	if( (FlushTop==INVALID_FLUSH_LINE) || (FlushBottom==INVALID_FLUSH_LINE))
		return;

	if(gST->ConOut == NULL)
		return;
	Top = FlushTop;
	Bottom = FlushBottom;

	if ( Top >= gMaxRows )
		return;

	if ( Top > Bottom )
	{
		UINTN Swap = Bottom;
		Bottom = Top;
		Top = Swap;
	}

	if ( Bottom >= gMaxRows )
		Bottom = gMaxRows - 1;

	ByteStart = Top * gMaxCols;
	ByteEnd = (Bottom - Top + 1) * gMaxCols + ByteStart;

	String = EfiLibAllocateZeroPool( (gMaxCols + 1) * sizeof(CHAR16) );

	if ( String == NULL )
	{
		SlowFlushLines( Top, Bottom );
		return;
	}

	// Flush from Top to Bottom
	// inside the loop 'Top' refers current line
	for ( ; Top <= Bottom; Top++ )
	{
		Offset = Top * gMaxCols;
		End = Offset + gMaxCols;

		// If All the Chars and attributes are equal in current line. Go to next line.
		if ( (CompleteReDrawFlag == FALSE)&& (( ! EfiCompareMem( &gActiveBuffer->Attribs[Offset], &gFlushBuffer->Attribs[Offset], gMaxCols ) ) &&
				( ! EfiCompareMem( &gActiveBuffer->Chars[Offset], &gFlushBuffer->Chars[Offset], gMaxCols * sizeof(CHAR16) ) )) )
			continue;

		// Don't Print the last char in MaxRow X MaxCol
		// As it will scroll if we print.
		if (!IsWriteCharLastRowColSupport())
		{    
		    if ( End >= gMaxRows * gMaxCols )
		        End = gMaxRows * gMaxCols - 1;
		}
		// Attribute for current print.
		LastAttrib = gActiveBuffer->Attribs[Offset];

		// To improve the flush lines code.
		CurrentX=0;

		// To support Terminal host with hight resolution.
		gST->ConOut->EnableCursor(gST->ConOut, FALSE);			//Make the cursor invisible since the cursor position is changing and visible outside popup box
		gST->ConOut->SetCursorPosition( gST->ConOut, CurrentX, Top );

		PrintLen = 0;
		FlushIt = TRUE; // decides printing

		if(IsMouseSupported())
		GetCoordinates(&MousePointerX, &MousePointerY, &MousePointerZ);

		while ( Offset < End )
		{
			// Collect the string till the Attribute is differnt as it may need setAttribute
			for ( Index = 0, TempOffSet = Offset; (gActiveBuffer->Attribs[Offset] == LastAttrib) && (Offset < End); Offset++, Index++ )
			{
				String[Index] = gActiveBuffer->Chars[Offset];

				if(TRUE == IsCharWide(String[Index]))
					Offset++;

			}
			String[Index] = 0;
			// Getting the actual Screen top from the binary for stopping the mouse
		  if(
			(IsMouseSupported() && 
         ((0 == Top) || (((MousePointerY/HiiGetGlyphHeight()) >= ((Top + ( GetactualScreentop()/ HiiGetGlyphHeight()))-1)) && ( (MousePointerY/HiiGetGlyphHeight()) <= ((Top + (GetactualScreentop()/ HiiGetGlyphHeight()))+1)))) )
         )		
		  {
		     MouseStop(); 
   		  }

			// We are trying to print the strings with 'LastAttrib'
			// NOW - TempOffSet refers begining of the buffer
			// 		 Offset refers end if the print buffer for currnet print.
			
			// if the String and attribute are equal to flush lines and it is more then 10 then we don't need to Print.
			// The reason for 10 is to avoid sending setposition. IF we skip less then 10 then we will end up calling setposition that takes 9 bytes
			// for serial redirection.
		  	  if(((Offset-TempOffSet)>10)&&
				( ! EfiCompareMem( &gActiveBuffer->Attribs[TempOffSet], &gFlushBuffer->Attribs[TempOffSet], Offset-TempOffSet ) )&&(CompleteReDrawFlag == FALSE))
			{
				// If all the charecters are same between active and flush buffer, let not print it
				if ( ( ! EfiCompareMem( &gActiveBuffer->Chars[TempOffSet], &gFlushBuffer->Chars[TempOffSet], (Offset-TempOffSet) * sizeof(CHAR16) ) ) )
					FlushIt = FALSE;
				else
				{
					UINTN i,j=0;
					UINTN FirstDiff, LastDiff;
					FirstDiff = LastDiff = (Offset-TempOffSet); // Fail case default
					
					// loop to find out that is the first and last different offset in the printable string.
					for(i=0,j=0; i<(Offset-TempOffSet);i++,j++)
					{
						if(gActiveBuffer->Chars[TempOffSet+i] != gFlushBuffer->Chars[TempOffSet+i])
						{
							if(FirstDiff>i) // First Diff set
								FirstDiff = i;
							LastDiff = i;
							Index = j;
						}
						if(TRUE == IsCharWide(String[j]))
							i++;
					}

                    // Modification to erase wide characters correctly.
                    //  Issue:
                    //   If we write "a" over some wide character, the right half of the wide character is sometimes not erased.
                    //   e.g.)
                    //    When we write "aaaa    " over "aaa?   ", the right half of "?" is not erased.
                    if (!IsCharWide(String[Index]) && IsCharWide(gFlushBuffer->Chars[TempOffSet + LastDiff]) && LastDiff+1<Offset-TempOffSet){
                        LastDiff++;
                        Index++;
                    }else if (IsCharWide(String[Index]) && IsCharWide(gFlushBuffer->Chars[TempOffSet + LastDiff + 1]) && LastDiff+2<Offset-TempOffSet){
                        LastDiff += 2;
                        Index++;
                    }
					if(FirstDiff>10) // Is it worth skiping?
					{
						Offset = TempOffSet+FirstDiff;
						String[Index] = 0; // No need. But lets do it anyway
						FlushIt = FALSE;
					}
					else  if(((Offset-TempOffSet)- LastDiff) > 10) // Only print the Different sting up to LastDiff
					{
						// Make the offset to point to next char to be printed for next iteration
						Offset = TempOffSet+LastDiff+1;
						if(TRUE == IsCharWide(String[Index]))
							Offset++;
						String[Index+1] = 0; 
					}
				}
			}

			if(FlushIt)
			{
				// Set of it is different from previous setatribute.
				if(LastSetAttrib!=LastAttrib)
				{
					gST->ConOut->SetAttribute( gST->ConOut, LastAttrib );
					LastSetAttrib = LastAttrib;
				}
				//do the set position if the cursor is not at expected loc. 
				if(((X+PrintLen) != CurrentX) || (Y != Top) || ((CurrentX == 0) && (Top==0)))
				{
					gST->ConOut->SetCursorPosition( gST->ConOut, CurrentX, Top );
				}

				X=CurrentX;
				Y=Top;
				
                if ( End >= gMaxRows * gMaxCols ) 
                {    
                    if(IsWriteCharLastRowColSupport())    
                        WriteCharLastRowCol(String, 0, 0xffff, 0, FALSE);
                }
                else
                {
               	 if(!TSEMouseIgnoreMouseActionHook() || !IsSoftKbdonCurrentRow(gST->ConOut->Mode->CursorRow))
               	 {
               		 gST->ConOut->OutputString( gST->ConOut, String );
               	 }
               	 else
               	 {
               		 UINTN i=0;
               		 CHAR16 Temp[2]={'\0'};
               		 for(i=0;i<EfiStrLen(String);i++)
               		 {
              				 MemCpy(Temp,String+i,sizeof(CHAR16));
               				 if(!IsCharacteronSoftKbd(gST->ConOut->Mode->CursorColumn,gST->ConOut->Mode->CursorRow))
               				 {
               					 gST->ConOut->OutputString( gST->ConOut, Temp);
               				 }
               				 else
               				 {
               					 gST->ConOut->SetCursorPosition(gST->ConOut,++(gST->ConOut->Mode->CursorColumn),gST->ConOut->Mode->CursorRow);
               				 }
               		 }
               	 }
                }

				PrintLen = Offset-TempOffSet;

				// if we print till end of line then increament the y and make x=0.
				if((X+PrintLen)>=gMaxCols)
				{
					Y++;X=0;
				}
			}

			CurrentX += Offset-TempOffSet;
			FlushIt = TRUE;
			LastAttrib = gActiveBuffer->Attribs[Offset];
		}
	}
	
	//Getting the actual Screen top and mouse pointer is on softkbd or not
	if(	IsMouseSupported() &&
			(
				( ( MousePointerY/glyphHeight ) >= ( (Bottom + ( screenTop/glyphHeight))-1) )	&& 	//Check whether MousePointerY is greater than actual Screen top 
				( (MousePointerY/glyphHeight) <= ((Bottom + (screenTop/glyphHeight))+1) ) 		&&		//Check whether MousePointerY is less than actual Screen top 
				( IsMouseOnSoftkbd() )																					//Check whether mouse pointer is on softkbd or not
			)
	)
	{
		MouseRefresh();
	}
	CompleteReDrawFlag = FALSE;
	MemCopy( &gFlushBuffer->Attribs[ByteStart], &gActiveBuffer->Attribs[ByteStart], ByteEnd - ByteStart );
	MemCopy( &gFlushBuffer->Chars[ByteStart], &gActiveBuffer->Chars[ByteStart], (ByteEnd - ByteStart) * sizeof(CHAR16) );
	FlushTop=INVALID_FLUSH_LINE;
	FlushBottom=INVALID_FLUSH_LINE;
	MemFreePointer( (VOID **)&String );
}

/**
    function to set the test mode

    @param nil

    @retval void

**/
void SetDesiredTextMode()
{
	EFI_STATUS Status;

	if(gST->ConOut==NULL)
		return;
    Status = gST->ConOut->Reset( gST->ConOut, FALSE );
	Status = gST->ConOut->SetMode( gST->ConOut, StyleGetTextMode( gMaxRows, gMaxCols ) );

	if ( EFI_ERROR( Status ) )
	{
		Status = gST->ConOut->SetMode( gST->ConOut, 0 );
		if (!EFI_ERROR(Status))
		{
			gMaxRows = StyleGetStdMaxRows(); 
			gMaxCols = StyleGetStdMaxCols();
		} 
	}
}

/**
    function to save the Active buffer Contents

    @param void **TempScreenBuffer

    @retval void

**/
void SaveActiveBuffer(void **TempScreenBuffer)
{
	if(*TempScreenBuffer == NULL)
		*TempScreenBuffer = EfiLibAllocateZeroPool( sizeof(SCREEN_BUFFER));
	if(*TempScreenBuffer != NULL)
		MemCopy(*TempScreenBuffer, gActiveBuffer, sizeof(SCREEN_BUFFER));
}

/**
    function to Restore the Active buffer Contents

    @param void **TempScreenBuffer

    @retval void

**/
void RestoreActiveBuffer(void **TempScreenBuffer)
{
	if(*TempScreenBuffer != NULL)
	{
		MemCopy(gActiveBuffer, *TempScreenBuffer, sizeof(SCREEN_BUFFER));
		MemFreePointer((VOID **)TempScreenBuffer);
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
