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
#include <SmcSerialTextOut.h>
#include <Library\PrintLib.h>

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

EFI_STATUS 
WriteToSerialPort (
    UINT16 BaseAddress,
    UINT8  *Buffer,
    UINTN  Length
)
{
    UINTN Result; 
    UINT8 Data;
    
    if (NULL == Buffer) {
       return 0;
    }

    Result = Length;

    while (Length--) {
       //
       // Wait for the serail port to be ready.
       //
       do {
          Data = IoRead8 (BaseAddress + LSR_OFFSET);
       } while ((Data & LSR_TXRDY) == 0);
       IoWrite8 (BaseAddress, *Buffer++);
     }

     return EFI_SUCCESS;
}


/**
    This is the function which writes a string to the screen.
    The characters of the string are translated or encoded as
    necessary according to the terminal type.
    The parameter UpdateCursorPosition is set to FALSE if an
    escape sequence is being written to the screen.
    For all other strings, the parameter should be set to TRUE.

    @param  This
    @param  String
    @param  UpdateCursorPosition

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalOutputStringHelper (
    IN SERIAL_CONSOLE_CONFIGURATION     *Configuration,
    IN UINT8                            *String 
)
{
    EFI_STATUS   Status = EFI_SUCCESS;
   
    UINTN       StringLength = 0;

    // Find the String Length
    while(String[StringLength] != 0) {
        StringLength++;
    }

    Status = WriteToSerialPort(Configuration->BaseAddress, String, StringLength);
   
    return Status;
}





/**
    Writes an escape sequence to the serial port
    to set the cursor position.
    The given "Column" and "Row" parameters must conform
    to MODE_0_MAX_COLUMNS and MODE_0_MAX_ROWS respectively.

    @param This 
    @param  Column
    @param  Row

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalSetCursorPosition (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration,
  IN UINT8                            Column,
  IN UINT8                            Row 
)
{
     EFI_STATUS Status = EFI_SUCCESS; 
 
    //SetCursorPositionEscapeSequence: ESC '[' Row ';' Column 'H' NULL
    //-up to 2 characters each for the row and column numbers 
    CHAR8   SetCursorPositionEscapeSequence[9];
    UINT8 gSetCursor24_60_Sequence[] = {0x1B, 0x5B, 0x32, 0x34, 0x3B, 0x36, 0x30, 0x48};
    if (Column >= MAX_COLUMNS || Row >= MAX_ROWS) {
        return EFI_UNSUPPORTED;
    }

    AsciiSPrint (SetCursorPositionEscapeSequence, 9, "%c[%d;%dH", 
                                ASCII_ESC, (UINT8)Row,(UINT8)Column);
    Status = TerminalOutputStringHelper(Configuration, 
                                        SetCursorPositionEscapeSequence);


    return Status;
}


/**
    Writes an escape sequence to the serial port
    to set various attributes (foreground and background colors
    and bold/normal text).

    @param  This
    @param  Attribute

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalSetAttribute (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration,
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

    AsciiSPrint (SetAttributeEscapeSequence, 12, "%c[%d;%2d;%2dm", 
                                ASCII_ESC, 0, ForegroundColor, BackgroundColor);
    
    Status = TerminalOutputStringHelper (Configuration, SetAttributeEscapeSequence);

    return Status; 
}

/**
    Writes an escape sequence to the serial port
    to clear the screen.

    @param  This

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalClearScreen (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration
)
{
    CHAR8 SetAttributeEscapeSequence[11];

    AsciiSPrint (SetAttributeEscapeSequence, 11, "%c[2J", ASCII_ESC);
    return TerminalOutputStringHelper (Configuration, SetAttributeEscapeSequence);
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
