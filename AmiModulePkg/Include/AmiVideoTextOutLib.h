//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file AmiVideoTextOutLib.h
    Video TextOut library defines and equates

**/

#ifndef __AMI_VIDEO_TEXT_OUT_LIB_H__
#define __AMI_VIDEO_TEXT_OUT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <AmiPeiLib.h>
#include <AmiDxeLib.h>


//Dimension Definitions
#define MAX_COLS                80
#define MAX_ROWS                25
#define BYTES_PER_CHAR          2   // 1 - ASCII value, 1 - Attribute value

#define ROWS_SPLIT              11
#define FIRST_SCREEN_START      0  
// Bottom -> Row number after "System boot status" display in Side 1
#define SECOND_SCREEN_START     ROWS_SPLIT + 3 

// Color values
#define VIDEO_COLOR_BLACK       0
#define VIDEO_COLOR_BLUE        1
#define VIDEO_COLOR_GREEN       2
#define VIDEO_COLOR_CYAN        3
#define VIDEO_COLOR_RED         4
#define VIDEO_COLOR_MAGENTA     5
#define VIDEO_COLOR_ORANGE      6
#define VIDEO_COLOR_WHITE       7

#define VIDEO_INTENSITY         BIT3
#define VIDEO_BLINK             BIT7

#define POST_MSG_FOREGROUND     VIDEO_COLOR_WHITE|VIDEO_INTENSITY
#define POST_MSG_BACKGROUND     VIDEO_COLOR_BLACK

#define FG_COLOR_MASK           0x0F
#define BG_COLOR_MASK           0x07

#define CHAR_CR                 0x0D
#define CHAR_LF                 0x0A


EFI_STATUS
AmiVideoClear(
    VOID
);

EFI_STATUS 
AmiScrollUp(
    IN UINT8        Side
);

EFI_STATUS
AmiVideoPrint(
    IN UINT8        Side, 
    IN const char* String
);

EFI_STATUS
AmiVideoPrintXY(
    IN UINT8        ColX, 
    IN UINT8        RowY, 
    IN UINT8        Side, 
    IN const char* String
);

EFI_STATUS
AmiVideoGotoXY(
    IN UINT8        ColX, 
    IN UINT8        RowY
);

EFI_STATUS
AmiVideoSetColor(
    IN UINT8        Color
);

EFI_STATUS 
AmiVideoPrintWorker(
    IN UINT8        Side, 
    IN const char   *String
);

EFI_STATUS
AmiVideoGetCursorPosition(
    IN UINT8    *Col, 
    IN UINT8    *Row 
);

UINT8
AmiVideoGetColor(
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
