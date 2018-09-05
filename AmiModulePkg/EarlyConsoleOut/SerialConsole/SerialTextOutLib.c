//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SerialTextOutLib.c
    Library functions to be used for Serial Console.

**/

#include <Include/AmiLib.h>
#include <Library/PrintLib.h>
#include <AmiSerialTextOutLib.h>
#include <Library/SerialPortLib.h>

UINT8 gForegroundColors[] = {
    30,         //Black
    34,         //Blue
    32,         //Green
    36,         //Cyan
    31,         //Red
    35,         //Magenta
    33,         //Brown (code is for Yellow)
    37,         //Light Gray (code is for White)
};

UINT8 gBackgroundColors[] = {
    40, //Black
    44, //Blue
    42, //Green
    46, //Cyan
    41, //Red
    45, //Magenta
    43, //Brown (code is for Yellow)
    47  //Light Gray (code is for White)
};

/**

    This is the function which writes a string to the screen.  
  
    @param  String String to be printed in serial port.

    @return  EFI_STATUS Status of printing.

**/
EFI_STATUS
TerminalOutputStringHelper (
    IN UINT8    *String 
)
{
    UINTN       StringLength = 0;
    UINTN       WritenLength = 0;

    // Find the String Length
    while(String[StringLength] != 0) {
        StringLength++;
    }

    // Use the Serial Port Lib to write the Data to Serial Port
    WritenLength=SerialPortWrite(String, StringLength);
    
    // If the number of data written to serial port is less than 
    // requested return with error.
    if(WritenLength != StringLength) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
    Writes an escape sequence to the serial port
    to set the cursor position.
    The given "Column" and "Row" parameters must conform
    to MODE_0_MAX_COLUMNS and MODE_0_MAX_ROWS respectively.
 
    @param                            Column
    @param                            Row

    @return EFI_STATUS
**/

EFI_STATUS
TerminalSetCursorPosition(
  IN UINT8                            Column,
  IN UINT8                            Row 
 )
{
    EFI_STATUS Status = EFI_SUCCESS; 

    //SetCursorPositionEscapeSequence: ESC '[' Row ';' Column 'H' NULL
    //-up to 2 characters each for the row and column numbers 
    CHAR8   SetCursorPositionEscapeSequence[9];

    if (Column >= MAX_COLUMNS || Row >= MAX_ROWS) {
        return EFI_UNSUPPORTED;
    }

    AsciiSPrint (SetCursorPositionEscapeSequence, 8, "%c[%d;%dH", 
                                ASCII_ESC, (UINT8)Row+1,(UINT8)Column+1);
    
    Status = TerminalOutputStringHelper (SetCursorPositionEscapeSequence);

    return Status; 
}

/**
    Writes an escape sequence to the serial port
    to set the attribute(Fore Ground, Back Ground)

    @param  TextAttribute

    @return EFI_STATUS
**/

EFI_STATUS
TerminalSetAttribute(
  IN UINT8                            TextAttribute 
)
{
    EFI_STATUS Status; 
    UINT8 ForegroundColor = gForegroundColors[TextAttribute & 0x7];
    UINT8 BackgroundColor = gBackgroundColors[(TextAttribute >> 4) & 0x7];
    
    //  Escape '[' Bold ';' ForegroundColor ';' BackgroundColor 'm' NULL
    //1 char for Bold and up to 2 each for ForegroundColor and
    //BackgroundColor
    CHAR8 SetAttributeEscapeSequence[12];

    AsciiSPrint (SetAttributeEscapeSequence, 11, "%c[%d;%2d;%2dm", 
                                ASCII_ESC, 0, ForegroundColor, BackgroundColor);
    
    Status = TerminalOutputStringHelper (SetAttributeEscapeSequence);

    return Status; 
}

/**
    Writes an escape sequence to the serial port to clear the screen 

    @param  None

    @return EFI_STATUS
**/
EFI_STATUS
TerminalClearScreen()
{
    EFI_STATUS Status; 
    CHAR8 SetAttributeEscapeSequence[5];

    AsciiSPrint (SetAttributeEscapeSequence, 11, "%c[2J", ASCII_ESC);
    Status = TerminalOutputStringHelper (SetAttributeEscapeSequence);

    return Status; 
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
