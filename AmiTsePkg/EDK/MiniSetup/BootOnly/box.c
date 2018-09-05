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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/box.c $
//
// $Author: Madhans $
//
// $Revision: 4 $
//
// $Date: 2/19/10 1:01p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#include "minisetup.h"

/**
    function to draw the lines with attributes

    @param Col UINTN Row, UINTN Length, CHAR16 Type, UINT8 Attrib

    @retval void

**/
VOID DrawLineWithAttribute( UINTN Col, UINTN Row, UINTN Length, CHAR16 Type, UINT8 Attrib )
{
	UINTN	Index;
	CHAR16	*Line;

	Line = EfiLibAllocatePool( sizeof(CHAR16) * (Length + 1) );
	if ( Line == NULL )
		return;

	for ( Index = 0; Index < Length; Index++ )
		Line[Index] = Type;

	Line[Index] = 0;

	DrawStringWithAttribute( Col, Row, Line, Attrib );

	MemFreePointer( (VOID **)&Line );
}

VOID DrawLine( UINTN Col, UINTN Row, UINTN Length, CHAR16 Type )
{
	DrawLineWithAttribute( Col, Row, Length, Type, 0 );
}

/**
    function to draw the border

    @param Left UINTN Top, UINTN Width, UINTN Height

    @retval void

**/
VOID DrawBorder( UINTN Left, UINTN Top, UINTN Width, UINTN Height )
{
	UINTN	Index;
	CHAR16	*Line;
	UINTN	Right, Bottom;

	if( Width == 0 || Height == 0 )
		return;

	Line = EfiLibAllocateZeroPool( (Width + 6) * sizeof(CHAR16) );
	if ( Line == NULL )
		return;

	Right = Left + Width - 1;
	Bottom = Top + Height - 1;

	for ( Index = 1; Index < Width - 1; Index++ )
		Line[Index] = BOXDRAW_HORIZONTAL;

	Line[0] = BOXDRAW_DOWN_RIGHT;
	Line[Index] = BOXDRAW_DOWN_LEFT;
	DrawString( Left, Top, Line );
	Line[0] = BOXDRAW_UP_RIGHT;
	Line[Index] = BOXDRAW_UP_LEFT;
	DrawString( Left, Bottom, Line );

	Line[0] = BOXDRAW_VERTICAL;
	Line[1] = 0;

	for ( Index = Top + 1; Index < Bottom; Index++ )
	{
		DrawString( Left, Index, Line );
		DrawString( Right, Index, Line );
	}

	MemFreePointer( (VOID **)&Line );
}

/**
    function to draw a window

    @param Left UINTN Top, UINTN Width, UINTN Height,
    @param Attrib BOOLEAN Border, BOOLEAN Shadow

    @retval void

**/
VOID DrawWindow( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib, BOOLEAN Border, BOOLEAN Shadow )
{
	if ( Shadow )
		DrawBox( Left + 2, Top + 1, Width, Height, 0 );

	DrawBox( Left, Top, Width, Height, Attrib );

	if ( Border )
		DrawBorder( Left, Top, Width, Height );
}

/**
    function to draw a box

    @param Left UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib

    @retval void

**/
VOID DrawBox( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib )
{
	UINTN	Bottom, Right;
	UINTN	Index;
	UINTN	Offset;
	CHAR16	*WindowLine = NULL;
	UINT8	*AttribLine = NULL;

	
	WindowLine = (CHAR16*)EfiLibAllocateZeroPool(sizeof(CHAR16)*gMaxCols);
	if (NULL == WindowLine)
		goto DONE;
	
	AttribLine = (UINT8*)EfiLibAllocateZeroPool(sizeof(UINT8)*gMaxCols);
	if ( NULL == AttribLine )
		goto DONE;
	
	Bottom = Top + Height;
	if ( Bottom >= gMaxRows )
	{
		Bottom = gMaxRows - 1;
		Height = Bottom - Top;
	}

	Right = Left + Width;
	if ( Right >= gMaxCols )
	{
		Right = gMaxCols - 1;
		Width = Right - Left;
	}

	MemSet( AttribLine, Width, Attrib );
	for ( Index = 0; Index < Width; Index++ )
		WindowLine[Index] = L' ';

	Offset = Top * gMaxCols + Left;
	for ( Index = Top; Index < Bottom; Index++, Offset += gMaxCols )
	{
		// to resolve the wide char corruption when box is drawn
		if((Left) && ( IsCharWide( gFlushBuffer->Chars[Offset-1]) == TRUE ))
				gActiveBuffer->Chars[Offset-1]=L' ';

		MemCopy( &gActiveBuffer->Chars[Offset], WindowLine, sizeof(CHAR16) * Width );
		MemCopy( &gActiveBuffer->Attribs[Offset], AttribLine, Width );
	}
DONE:
	if (NULL != WindowLine)
		MemFreePointer((VOID**)&WindowLine);
	
	if (NULL != AttribLine)
		MemFreePointer((VOID**)&AttribLine);
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
