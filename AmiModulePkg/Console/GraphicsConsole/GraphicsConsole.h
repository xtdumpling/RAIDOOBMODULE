//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
    Graphics console driver that produces the Simple Text Out interface

*/
//**********************************************************************

#ifndef _GRAPHCICS_CONSOLE_H_
#define _GRAPHCICS_CONSOLE_H_

#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Guid/MdeModuleHii.h>

#include <AmiDxeLib.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiFont.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/AmiTextOut.h>
#include <Library/PcdLib.h>
#include <Token.h>

//-----------------------------------------------------------------------------

#define	NARROW_GLYPH_WIDTH	EFI_GLYPH_WIDTH ///< width of a narrow glyph in this project
#define WIDE_GLYPH_WIDTH	NARROW_GLYPH_WIDTH * 2 ///< width of a width Glyph in this project

#define MODE_ZERO_MIN_HOR_RES NARROW_GLYPH_WIDTH * 80 ///< width of the page, in pixels
#define MODE_ZERO_MIN_VER_RES EFI_GLYPH_HEIGHT * 25 ///< height of the page, in pixels

#define CURSOR_THICKNESS	3	///< thickness of the cursor
#define CURSOR_OFFSET   	15  ///< base line of simple narrow font

#define MAX_RES             0
#define MIN_RES             -1

#define	NULL_CHAR			0x0000 ///< value for a unicode null character
#define	BACKSPACE			0x0008 ///< value for a unicode backspace character
#define	LINE_FEED			0x000A ///< value for a unicode Line Feed character
#define	CARRIAGE_RETURN		0x000D ///< value for a unicode Carriage return character

//-----------------------------------------------------------------------------
// Data Structures

#pragma pack(1)

/**
    This structure represents text mode internal information structure
*/
typedef struct _TEXT_MODE  {
	INT32	ModeNum; ///< Mode number
	INT32	Col; ///< Max number of columns
	INT32	Row; ///< Max number of rows
	UINT32	VideoCol; ///< Horizontal screen resolution in pixels
	UINT32	VideoRow; ///< Vertical screen resolution in pixels
} TEXT_MODE;

/**
    This structure represents text mode extended internal information structure
*/
typedef struct _GC_TEXT_MODE  {
	INT32	ModeNum; ///< Mode number
	INT32	Col; ///< Max number of columns
	INT32	Row; ///< Max number of rows
	UINT32	VideoCol; ///< Horizontal screen resolution in pixels
	UINT32	VideoRow; ///< Vertical screen resolution in pixels
    BOOLEAN Supported; ///< Flag if this mode supported
    UINT32  GraphicsMode; ///< Correspondent graphics mode
} GC_TEXT_MODE;

typedef struct _GC_DATA GC_DATA; ///< forward type definition of GC_DATA

typedef VOID (* AGC_UPDATE_BLT_BUFFER ) (
	IN     GC_DATA 			             *This,
	IN     UINT32			             Width,
    IN     UINT32                        Height,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer
	);

typedef VOID (* AGC_CLEAR_SCREEN) (
	IN OUT GC_DATA *This
	);

typedef VOID (* AGC_SCROLL_UP) (
	IN GC_DATA *This
	);

/**
    This structure represents internal information structure for Graphics console
    driver
*/
struct _GC_DATA{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	SimpleTextOut; ///< Pointer to the SimpleText Output Protocol
	UINT32				            Signature; ///< Signature (must be 0x54445348 ('GRCS') )
    UINT32                          Version; ///< Current version of the Graphics Console Driver
	EFI_GRAPHICS_OUTPUT_PROTOCOL	*GraphicsOutput; ///< Pointer to Gop driver
	EFI_HII_FONT_PROTOCOL		    *HiiFont; ///< Pointer to HII driver
    GC_TEXT_MODE                    *SupportedModes; ///< Pointer to supported modes arra
    UINT32                          MaxRows; ///< Max number of rows in current mode
    UINT32                          MaxColumns; ///< Max number of columns in current mode
    UINT32                          DeltaX; ///< Horizontal offset in pixels for current text mode
    UINT32                          DeltaY; ///< Vertical offset in pixels for current text mode
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL   Cursor[NARROW_GLYPH_WIDTH * 3]; ///< Array for saving current cursor image
	BOOLEAN				            BlinkVisible; ///< If true cursor is visible, otherwise - invisible
	EFI_EVENT			            CursorEvent; ///< This event makes the cursor blink
	AGC_UPDATE_BLT_BUFFER		    OemUpdateBltBuffer; ///< Pointer to custom hook before Updating the BLT Buffer
	AGC_CLEAR_SCREEN		        OemClearScreen; ///< Pointer to custom hook before clear screen is called
	AGC_SCROLL_UP			        OemScrollUp; ///< Pointer to custom hook before Scroll up is called
	AMI_TEXT_OUT_PROTOCOL           AmiTextOut; ///< Instance of AMI TextOut protocol
};
#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
