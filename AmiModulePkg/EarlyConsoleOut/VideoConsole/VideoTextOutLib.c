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

/** @file VideoTextOutLib.c
    AMI Video TextOut Library functions

**/

//---------------------------------------------------------------------------

#include "AmiVideoTextOutLib.h"
#include <Library/DebugLib.h>

//---------------------------------------------------------------------------

// Video Memory
#define VIDEO_MEMORY             0xB8000

// Use memory outside the screen area to hold current cursor location and color.
#define VIDEO_PRIVATE_DATA_MEMORY   VIDEO_MEMORY + MAX_ROWS * MAX_COLS * BYTES_PER_CHAR

typedef struct {
    UINT8 Ascii;
    UINT8 Attribute;
} TEXT_MODE_CHAR;

typedef struct {
    UINT32 Cursor;
    UINT8  Color;
} VIDEO_PRIVATE_DATA;


/**
    Clears the screen (writes spaces) to the text mode frame buffer.

    @param VOID

    @retval EFI_SUCCESS
**/

EFI_STATUS
AmiVideoClear(
    VOID
) 
{
    TEXT_MODE_CHAR   *VideoMemory = (TEXT_MODE_CHAR *)VIDEO_MEMORY;
    VIDEO_PRIVATE_DATA  *ScreenInfo = (VIDEO_PRIVATE_DATA *)VIDEO_PRIVATE_DATA_MEMORY;
    UINT32  Index = 0;

    // Clear the Video Memory Area
    for (Index = 0; Index < MAX_ROWS * MAX_COLS ; Index+=1) {
        // Fill with Spaces
        VideoMemory[Index].Ascii = ' ';
        VideoMemory[Index].Attribute = VIDEO_COLOR_WHITE;
    }

    // Reset current screen cursor position and color values
    ScreenInfo->Cursor = 0;
    ScreenInfo->Color = VIDEO_COLOR_WHITE;            
   
    return EFI_SUCCESS;
}

/**
    Scroll the screen up by one line

    @param Side - Top/Bottom screen to scroll

    @retval EFI_SUCCESS
**/

EFI_STATUS 
AmiScrollUp(
    IN UINT8    Side
) 
{
    TEXT_MODE_CHAR   *VideoMemory = (TEXT_MODE_CHAR *)VIDEO_MEMORY;
    UINT32  Index;

    if(Side > 1) {
        return EFI_NOT_FOUND;
    }
    
    //Top Screen
    if (Side == 0) { 
        
        // Move the next Row value to current row and keep doing until it reaches the end.   
        for (Index = FIRST_SCREEN_START; Index < ((ROWS_SPLIT)-1)* MAX_COLS ; Index++) {
            VideoMemory[Index].Ascii = VideoMemory[Index + MAX_COLS ].Ascii;
            VideoMemory[Index].Attribute =  VideoMemory[Index + MAX_COLS ].Attribute;
        }
        
        //Clear the Last Row
        for (Index = ((ROWS_SPLIT)-1)* MAX_COLS ; Index < (ROWS_SPLIT) * MAX_COLS ; Index++) {
            VideoMemory[Index].Ascii = ' '; // Fill with Spaces
            VideoMemory[Index].Attribute = VIDEO_COLOR_WHITE;
        }
        
    } else {        
        // Bottom Screen 
        // Start scroll after display "System boot status" in Side 1
        for (Index = ((SECOND_SCREEN_START)) * MAX_COLS ; Index < (MAX_ROWS-1) * MAX_COLS ; Index++) {
            VideoMemory[Index].Ascii = VideoMemory[Index + MAX_COLS ].Ascii;
            VideoMemory[Index].Attribute = VideoMemory[Index + MAX_COLS ].Attribute;
        }
        
        // Clear the Last row
        for (Index = (MAX_ROWS-1) * MAX_COLS ; Index < MAX_ROWS * MAX_COLS ; Index++) {
            VideoMemory[Index].Ascii = ' '; 
            VideoMemory[Index].Attribute = VIDEO_COLOR_WHITE;
        }    
    }
    
    return EFI_SUCCESS;
}


/**
    Function to print the input string and scroll the line if needed

    @param Side - Top/Bottom screen to print on
    @param String - String to print

    @retval EFI_SUCCESS
**/

EFI_STATUS 
AmiVideoPrintWorker(
    IN UINT8        Side, 
    IN const char   *String
) 
{
    
    TEXT_MODE_CHAR      *VideoMemory = (TEXT_MODE_CHAR *)VIDEO_MEMORY;
    VIDEO_PRIVATE_DATA  *ScreenInfo = (VIDEO_PRIVATE_DATA *)VIDEO_PRIVATE_DATA_MEMORY;
    CHAR8               Char;

    while (*String != '\0') {
        Char = *String++;
        
        switch (Char) {
            case 0xa: 
                // Go to the next line
                ScreenInfo->Cursor = ScreenInfo->Cursor + (MAX_COLS);
                break;
                
            case 0xd: 
                // Bring the cursor to the beginning of the line
                ScreenInfo->Cursor = ScreenInfo->Cursor / (MAX_COLS);
                ScreenInfo->Cursor = ScreenInfo->Cursor * (MAX_COLS);
                break;
                
            default:    
                //Write date to Video Memory
                VideoMemory[ScreenInfo->Cursor].Ascii = Char;
                VideoMemory[ScreenInfo->Cursor].Attribute = ScreenInfo->Color;
                ScreenInfo->Cursor = ScreenInfo->Cursor + 1;
        }
        
        if (Side == 0){
            // If the Cursor position is above the Side 0 Screen 
            // Scroll up the one line
            if (ScreenInfo->Cursor >= (ROWS_SPLIT)*MAX_COLS) {
                AmiScrollUp(Side);
                ScreenInfo->Cursor = (ROWS_SPLIT-1)*MAX_COLS;
            }       
        } else {
            // If the Cursor position is above the Side 1 Screen 
            // Scroll up the one line
            if (ScreenInfo->Cursor >= MAX_ROWS*MAX_COLS) {
                AmiScrollUp(Side);
                ScreenInfo->Cursor = (MAX_ROWS-1)*MAX_COLS;
            }
        }
    }
    
    return EFI_SUCCESS;
}

/**
    Prints input string

    @param Side - Top/Bottom screen to print on
    @param String - String to print

    @retval EFI_SUCCESS
**/

EFI_STATUS
AmiVideoPrint(
    IN UINT8    Side, 
    IN const char* String
)
{

    
    AmiVideoPrintWorker(Side, String);
    return EFI_SUCCESS;
}

/**
    Prints input string at the specified position (X,Y).

    @param ColX - Column number
    @param RowY - Row number    
    @param Side - Top/Bottom screen to print on
    @param String - String to print

    @retval EFI_SUCCESS
**/

EFI_STATUS
AmiVideoGetCursorPosition(
    IN UINT8    *Col, 
    IN UINT8    *Row 
)
{

    VIDEO_PRIVATE_DATA  *ScreenInfo = (VIDEO_PRIVATE_DATA *)VIDEO_PRIVATE_DATA_MEMORY;

    // Bring the cursor to the beginning of the line
    *Row = (UINT8)( ScreenInfo->Cursor / (MAX_COLS));
    *Col = (UINT8)( ScreenInfo->Cursor - (ScreenInfo->Cursor / MAX_COLS ) * MAX_COLS);

    return EFI_SUCCESS;
}

/**
    Sets the cursor to the row and column specified by x and y.

    @param ColX - Column number
    @param RowY - Row number    

    @retval EFI_SUCCESS - Cursor position set as input X, Y
**/

EFI_STATUS
AmiVideoGotoXY(
    IN UINT8    ColX, 
    IN UINT8    RowY
)
{
    VIDEO_PRIVATE_DATA  *ScreenInfo = 
                    (VIDEO_PRIVATE_DATA *)VIDEO_PRIVATE_DATA_MEMORY;

    ScreenInfo->Cursor = RowY*MAX_COLS + ColX;

    return EFI_SUCCESS;
}

/**
    Sets the current screen foreground/background color to the specified value

    @param Color - Color attributes to be set
           Foreground is specified by bits 2:0,
           Background is specified by bits 6:4
           Bit 3 increases the intensity of the color selected.
           Bit 7 causes the text to blink.
 
       For example, intense white text on a blue background would be specified 
       as ((VIDEO_COLOR_BLUE << 4) | VIDEO_COLOR_WHITE | VIDEO_INTENSITY).

    @retval EFI_SUCCESS
**/

EFI_STATUS
AmiVideoSetColor(
    IN UINT8    Color
)
{
    VIDEO_PRIVATE_DATA  *ScreenInfo = (VIDEO_PRIVATE_DATA *)VIDEO_PRIVATE_DATA_MEMORY;
    
    ScreenInfo->Color = Color;

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
