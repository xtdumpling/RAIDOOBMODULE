//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TerminalSimpleTextOut.c
    Provides AHCI Block IO protocol

**/

//---------------------------------------------------------------------------
#include "Terminal.h"

//---------------------------------------------------------------------------
extern  TERMINAL_DEV    **gTerminalDev;
extern  UINTN           TotalTerminalDev;

extern  BOOLEAN         Dont_Send_Ascii_Control_Printable_Characters;

CHAR16 CurrentText[MODE_2_MAX_ROWS][MODE_2_MAX_COLUMNS];
UINT8  CurrentAttribute[MODE_2_MAX_ROWS][MODE_2_MAX_COLUMNS];
INT32 LineLength[MODE_2_MAX_ROWS];

typedef struct {
    CHAR16  Unicode;
    CHAR8   ExtendedAscii;
    CHAR8   Ascii;
} UNICODE_TO_ASCII;

UNICODE_TO_ASCII gUnicodeToAscii[] =
{
//*******************************************************
// UNICODE DRAWING CHARACTERS
//*******************************************************
    BOXDRAW_HORIZONTAL,                 0xc4, '-', 
    BOXDRAW_VERTICAL,                   0xb3, '|',
    BOXDRAW_DOWN_RIGHT,                 0xda, '/',
    BOXDRAW_DOWN_LEFT,                  0xbf, '\\',
    BOXDRAW_UP_RIGHT,                   0xc0, '\\',
    BOXDRAW_UP_LEFT,                    0xd9, '/',
    BOXDRAW_VERTICAL_RIGHT,             0xc3, '|',
    BOXDRAW_VERTICAL_LEFT,              0xb4, '|',
    BOXDRAW_DOWN_HORIZONTAL,            0xc2, '+',
    BOXDRAW_UP_HORIZONTAL,              0xc1, '+',
    BOXDRAW_VERTICAL_HORIZONTAL,        0xc5, '+',
    BOXDRAW_DOUBLE_HORIZONTAL,          0xcd, '-',
    BOXDRAW_DOUBLE_VERTICAL,            0xba, '|',
    BOXDRAW_DOWN_RIGHT_DOUBLE,          0xd5, '/',
    BOXDRAW_DOWN_DOUBLE_RIGHT,          0xd6, '/',
    BOXDRAW_DOUBLE_DOWN_RIGHT,          0xc9, '/',
    BOXDRAW_DOWN_LEFT_DOUBLE,           0xb8, '\\',
    BOXDRAW_DOWN_DOUBLE_LEFT,           0xb7, '\\',
    BOXDRAW_DOUBLE_DOWN_LEFT,           0xbb, '\\',
    BOXDRAW_UP_RIGHT_DOUBLE,            0xd4, '\\',
    BOXDRAW_UP_DOUBLE_RIGHT,            0xd3, '\\',
    BOXDRAW_DOUBLE_UP_RIGHT,            0xc8, '\\',
    BOXDRAW_UP_LEFT_DOUBLE,             0xbe, '/',
    BOXDRAW_UP_DOUBLE_LEFT,             0xbd, '/',
    BOXDRAW_DOUBLE_UP_LEFT,             0xbc, '/',
    BOXDRAW_VERTICAL_RIGHT_DOUBLE,      0xc6, '|',
    BOXDRAW_VERTICAL_DOUBLE_RIGHT,      0xc7, '|',
    BOXDRAW_DOUBLE_VERTICAL_RIGHT,      0xcc, '|',
    BOXDRAW_VERTICAL_LEFT_DOUBLE,       0xb5, '|',
    BOXDRAW_VERTICAL_DOUBLE_LEFT,       0xb6, '|',
    BOXDRAW_DOUBLE_VERTICAL_LEFT,       0xb9, '|',
    BOXDRAW_DOWN_HORIZONTAL_DOUBLE,     0xd1, '+',
    BOXDRAW_DOWN_DOUBLE_HORIZONTAL,     0xd2, '+',
    BOXDRAW_DOUBLE_DOWN_HORIZONTAL,     0xcb, '+',
    BOXDRAW_UP_HORIZONTAL_DOUBLE,       0xcf, '+',
    BOXDRAW_UP_DOUBLE_HORIZONTAL,       0xd0, '+',
    BOXDRAW_DOUBLE_UP_HORIZONTAL,       0xca, '+',
    BOXDRAW_VERTICAL_HORIZONTAL_DOUBLE, 0xd8, '+',
    BOXDRAW_VERTICAL_DOUBLE_HORIZONTAL, 0xd7, '+',
    BOXDRAW_DOUBLE_VERTICAL_HORIZONTAL, 0xce, '+',
//*******************************************************
// EFI Required Block Elements Code Chart
//*******************************************************
    BLOCKELEMENT_FULL_BLOCK,            0xdb, '*',
    BLOCKELEMENT_LIGHT_SHADE,           0xb0, '+',
//*******************************************************
// EFI Required Geometric Shapes Code Chart
//*******************************************************
    GEOMETRICSHAPE_UP_TRIANGLE,         0x1e, '^',
    GEOMETRICSHAPE_RIGHT_TRIANGLE,      0x10, '>',
    GEOMETRICSHAPE_DOWN_TRIANGLE,       0x1f, 'v',
    GEOMETRICSHAPE_LEFT_TRIANGLE,       0x11, '<',
//*******************************************************
// EFI Required Arrow shapes
//*******************************************************
    ARROW_UP,                           0x18, '^',
    ARROW_DOWN,                         0x19, 'v',
    ARROW_LEFT,                         0x3c, '<',
    ARROW_RIGHT,                        0x3e, '>'
};

CHAR16 gClearScreenEscapeSequence[] = {ASCII_ESC, '[', '2', 'J', '\0'};

//ANSI display mode 3: 80 columns by 25 rows, color on
CHAR16 gSetModeEscapeSequence[] = {ASCII_ESC, '[', '=', '3', 'h'};

/*
Ansi Color Codes: 
    30    Black
    31    Red
    32    Green
    33    Yellow
    34    Blue
    35    Magenta
    36    Cyan
    37    White

Array indeces for array gForegroundColors, from SimpleTextOut.h: 
#define EFI_BLACK                   0x00
#define EFI_BLUE                    0x01
#define EFI_GREEN                   0x02
#define EFI_CYAN                    0x03
#define EFI_RED                     0x04
#define EFI_MAGENTA                 0x05
#define EFI_BROWN                   0x06
#define EFI_LIGHTGRAY               0x07
#define EFI_DARKGRAY                0x08
#define EFI_LIGHTBLUE               0x09
#define EFI_LIGHTGREEN              0x0A
#define EFI_LIGHTCYAN               0x0B
#define EFI_LIGHTRED                0x0C
#define EFI_LIGHTMAGENTA            0x0D
#define EFI_YELLOW                  0x0E
#define EFI_WHITE                   0x0F

Array index << 4 for array gBackgroundColors, from SimpleTextOut.h: 
#define EFI_BACKGROUND_BLACK        0x00
#define EFI_BACKGROUND_BLUE         0x10
#define EFI_BACKGROUND_GREEN        0x20
#define EFI_BACKGROUND_CYAN         0x30
#define EFI_BACKGROUND_RED          0x40
#define EFI_BACKGROUND_MAGENTA      0x50
#define EFI_BACKGROUND_BROWN        0x60
#define EFI_BACKGROUND_LIGHTGRAY    0x70
*/

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
    Return TRUE is the given character value is less than 256  
    (both ASCII and extended ASCII characters). 

    @param  c

    @retval BOOLEAN

**/

BOOLEAN
isValidAnsi (
    CHAR16 c
)
{
    return (c < 256);
}

/**
    Return TRUE is the given character value is less than 128.  

    @param  c

    @retval  BOOLEAN

**/

BOOLEAN
isValidAscii (
    CHAR16 c
)
{
    return (c < 128);
}

/**
    This function translates the Unicode drawing 
    characters required by EFI and defined in SimpleTextOut.h 
    (e.g. for drawing boxes) to extended ASCII.  
    (The extended ASCII drawing characters look nicer than the 
    regular ASCII drawing characters).
    The Unicode drawing characters have the form 0x25XX or 0x21XX.  
    We look up the Unicode character in the gUnicodeToAscii table.  
    If the given character does not have the correct form or is 
    not in the table, we return 0.  

    @param  c

    @retval  CHAR8

**/

CHAR8
TranslateToAnsi (
    CHAR16 c
)
{
    UINTN i;

  if ((((c & 0xff00) != 0x2500) && ((c & 0xff00) != 0x2100))) {
    return 0;
  }

    for(i = 0; i < sizeof(gUnicodeToAscii)/sizeof(UNICODE_TO_ASCII); i++)
    {
        if (gUnicodeToAscii[i].Unicode == c) {
            //
            // Send the Ascii value instead of the extended Ascii if
            // DONT_SEND_ASCII_CONTROL_PRINTABLE_CHARACTERS token is enabled.
            //
            if ((Dont_Send_Ascii_Control_Printable_Characters == 1) && \
                   (gUnicodeToAscii[i].ExtendedAscii <= 0x1f) ) {
                return gUnicodeToAscii[i].Ascii;
            }
            return gUnicodeToAscii[i].ExtendedAscii;
        }
    }
    return 0;
}

/**
    This function translates the Unicode drawing
    characters required by EFI and defined in SimpleTextOut.h
    (e.g. for drawing boxes) to ASCII.  
    The Unicode drawing characters have the form 0x25XX or 0x21XX.
    We look up the Unicode character in the gUnicodeToAscii table.
    If the given character does not have the correct form or is
    not in the table, we return 0.

    @param  c

    @retval  CHAR8

**/

CHAR8
TranslateToAscii (
    CHAR16 c
)
{
    UINTN i;

    if ((((c & 0xff00) != 0x2500) && ((c & 0xff00) != 0x2100))) {
        return 0;
    }

    for(i = 0; i < sizeof(gUnicodeToAscii)/sizeof(UNICODE_TO_ASCII); i++)
    {
        if (gUnicodeToAscii[i].Unicode == c)
            return gUnicodeToAscii[i].Ascii;
    }
    return 0;
}

/**
    Uses the SerialIo protocol's Write function.

    @param  SerialIo
    @param  Buffer
    @param  Length

    @retval  EFI_STATUS

**/

EFI_STATUS 
WriteToSerialPort (
    EFI_SERIAL_IO_PROTOCOL *SerialIo,
    UINT8 *Buffer,
    UINTN Length
)
{
    EFI_STATUS Status;
    UINTN BufferSize;

    while (Length)
    {
        BufferSize = Length;

        Status = SerialIo->Write(
                                SerialIo,
                                &BufferSize,
                                Buffer
                                );
        if (EFI_ERROR(Status) && Status != EFI_TIMEOUT) break;

        Buffer += BufferSize;
        Length -= BufferSize;
    }

    return EFI_SUCCESS;
}

/**
    Writes escape sequences to the seral port which
    clear the screen and set the background color to white and
    the foreground color to black.

    @param  This
    @param  ExtendedVerification

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalReset (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN BOOLEAN                          ExtendedVerification
)
{
    EFI_STATUS Status;
    UINTN      i,j;
    UINTN      MaxColumns,MaxRows;

    if (This->Mode->Mode == 2) {
        MaxColumns = MODE_2_MAX_COLUMNS;
        MaxRows = MODE_2_MAX_ROWS;
    } else if (This->Mode->Mode == 0){
        MaxColumns = MODE_0_MAX_COLUMNS;
        MaxRows = MODE_0_MAX_ROWS;
    } else {
        return EFI_UNSUPPORTED;
    }

    Status = TerminalClearScreen(This);

    for(i=0; i<MaxRows;i++) {
        for(j=0; j<MaxColumns;j++) {
            CurrentAttribute[i][j] = 0xff;
        }
    }
    if (EFI_ERROR(Status)) return Status;

    return TerminalSetAttribute(This, EFI_TEXT_ATTR(EFI_WHITE,EFI_BLACK));
}

/**
    Utf8 is a character encoding for Unicode characters.

    For Unicode characters with values 0x00-0x7F,
    (the basic ASCII characters), 
    the Utf8 encoding has one byte which is comprised of '0b'
    plus the ASCII character.

    For Unicode characters with values 0x80-0x7FF,
    the Utf8 encoding has two bytes.
    The first byte begins with "110b" and the second byte "10b".

    For Unicode characters with values 0x800-0xFFFF,
    the Utf8 encoding has three bytes.
    The first byte begins with "1110b,"
    and the second and third bytes start with "10b".

    @param  Unicode
    @param  Utf8Char
    @param  NumBytes

    @retval  VOID

**/

VOID
UnicodeToUtf8 (
    CHAR16 Unicode, 
    UTF8_CHAR *Utf8Char,
    UINT8 *NumBytes
)
{
    UINT8 UnicodeByte0  = (UINT8) Unicode;
    UINT8 UnicodeByte1 = (UINT8) (Unicode >> 8);

    *NumBytes = 0;

    if (Unicode < 0x0080) {   //7 bits

        Utf8Char->UTF8_1  = (UINT8) (UnicodeByte0 & 0x7f);
        *NumBytes       = 1;

    } else if (Unicode < 0x0800) {    //8-11 bits
        //
        // byte sequence: high -> low
        //                UTF8_2[0], UTF8_2[1]
        //
        Utf8Char->UTF8_2[1] = (UINT8) ((UnicodeByte0 & 0x3f) + 0x80);
        Utf8Char->UTF8_2[0] = (UINT8) 
            ((((UnicodeByte1 << 2) + (UnicodeByte0 >> 6)) & 0x1f) + 0xc0);

        *NumBytes         = 2;

    } else {
        //
        // byte sequence: high -> low
        //                UTF8_3[0], UTF8_3[1], UTF8_3[2]
        //
        Utf8Char->UTF8_3[2] = (UINT8) ((UnicodeByte0 & 0x3f) + 0x80);
        Utf8Char->UTF8_3[1] = (UINT8)
            ((((UnicodeByte1 << 2) + (UnicodeByte0 >> 6)) & 0x3f) + 0x80);
        Utf8Char->UTF8_3[0] = (UINT8) (((UnicodeByte1 >> 4) & 0x0f) + 0xe0);

        *NumBytes         = 3;
    }
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
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *String,
    IN BOOLEAN                          UpdateCursorPosition
)
{
    EFI_STATUS   Status = EFI_SUCCESS;
    UINT8        *Buffer = NULL;
    UINTN        StringLength = 0;
    UINTN        i = 0;
    UINTN        j = 0;
    CHAR16       c = 0;
    BOOLEAN      Warning = FALSE;
    TERMINAL_DEV *TerminalDev = 
                            OUTTER(This, SimpleTextOutput, TERMINAL_DEV);
    UTF8_CHAR     Utf8Char; 
    UINT8         NumBytes = 0;
    INT32         MaxColumns = MODE_0_MAX_COLUMNS;
    INT32         MaxRows = MODE_0_MAX_ROWS; 
    BOOLEAN       TransalteAscii=FALSE;
    INT32         k,l;

    while(String[StringLength] != 0) {
        StringLength++;
    }

    if (TerminalDev->TerminalType == VT_UTF8) {
        Buffer = Malloc(StringLength*3);
    } else { 
        Buffer = Malloc(StringLength);
    }

    if (!Buffer) {
        return EFI_OUT_OF_RESOURCES;
    }

    if (This->Mode->Mode == 2) {
        MaxColumns = MODE_2_MAX_COLUMNS;
        MaxRows = MODE_2_MAX_ROWS;
    } else if (This->Mode->Mode == 0) {
        MaxColumns = MODE_0_MAX_COLUMNS;
        MaxRows = MODE_0_MAX_ROWS;
    }

    for(i = 0, j = 0; i < StringLength; i++, j++)
    {
        c = String[i];
        if(UpdateCursorPosition) {
            //
            // Line break,newline characters need not be stored inside
            // CurrentText buffer. Only printable characters are stored
            // inside CurrentText buffer
            //
            if( (c != '\b') && (c != '\r') && (c != '\n') ){
                CurrentText[This->Mode->CursorRow][This->Mode->CursorColumn]= c;
                CurrentAttribute[This->Mode->CursorRow][This->Mode->CursorColumn]=This->Mode->Attribute;
                LineLength[This->Mode->CursorRow] = This->Mode->CursorColumn+1;
            }
        }

        TransalteAscii=FALSE;
        switch (TerminalDev->TerminalType) {
            case ANSI:
                if (c) {
                    if (!isValidAnsi(c))
                    {
                        c = TranslateToAnsi(c);
                        TransalteAscii=TRUE;
                        if (!c) {c = '.'; Warning = TRUE;}
                    }
                    if(!(TerminalDev->RecorderMode && TransalteAscii)) {
                        Buffer[i] = (UINT8) c;
                    } else {
                        Buffer[i]=0x20;
                    }
                }
                break;

            case VT100:
            case VT100Plus:
                if (c) {
                    if (!isValidAscii(c))
                    {
                        c = TranslateToAscii(c);
                        TransalteAscii=TRUE;
                        if (!c) {c = '.'; Warning = TRUE;}
                    }
                    if(!(TerminalDev->RecorderMode && TransalteAscii)) {
                        Buffer[i] = (UINT8) c;
                    } else {
                        Buffer[i]=0x20;
                    }
                }
                break;

            case VT_UTF8:
                NumBytes = 0; 
                UnicodeToUtf8 (c, &Utf8Char, &NumBytes);
                switch (NumBytes) {
                    case 0:
                        Buffer[j] = '.';
                        Warning = TRUE;
                        break;
                    case 1:
                        Buffer[j] = Utf8Char.UTF8_1;
                        break;
                    case 2: 
                        if(!TerminalDev->RecorderMode) {
                            Buffer[j] = Utf8Char.UTF8_2[0];
                            Buffer[j+1] = Utf8Char.UTF8_2[1];
                            j++;
                        }
                        break;
                    case 3: 
                        if(!TerminalDev->RecorderMode) {
                            Buffer[j] = Utf8Char.UTF8_3[0];
                            Buffer[j+1] = Utf8Char.UTF8_3[1];
                            Buffer[j+2] = Utf8Char.UTF8_3[2];
                            j+=2;
                        }
                }
                break;
        }

        if (UpdateCursorPosition) {
            switch(c)
            {
                case '\b': case ASCII_DEL:  //backspace
                    This->Mode->CursorColumn--;
                    break;
                case '\r':      //carriage return
                    LineLength[This->Mode->CursorRow] = This->Mode->CursorColumn;
                    This->Mode->CursorColumn = 0;
                    break;
                case '\n':      //new line/line feed
                    if( This->Mode->CursorRow == (MaxRows - 1) ) {
                        for( k = 0; k < (MaxRows-1); k++ ) {
                            for( l = 0; l < LineLength[k+1]; l++ ) {
                                CurrentText[k][l] = CurrentText[k+1][l];
                                CurrentAttribute[k][l] = CurrentAttribute[k+1][l];
                            }
                            if( l < MaxColumns ) {
                                for( ; l < MaxColumns; l++ ) CurrentText[k][l] = 0x20;
                            }
                            LineLength[k] = LineLength[k+1];
                        }
                        for( l = 0; l < MaxColumns; l++ ) CurrentText[k][l] = 0x20;
                    }
                    This->Mode->CursorRow++;
                    break;
                default:
                    This->Mode->CursorColumn++;
                break;
            }

            if (This->Mode->CursorColumn < 0) {
                This->Mode->CursorColumn = MaxColumns-1;
                --This->Mode->CursorRow;
            } else if (This->Mode->CursorColumn >= MaxColumns) {
                This->Mode->CursorColumn = 0; 
                LineLength[This->Mode->CursorRow] = MaxColumns;
                if( This->Mode->CursorRow == (MaxRows - 1) ) {
                    for( k = 0; k < (MaxRows-1); k++ ) {
                        for( l = 0; l < LineLength[k+1]; l++ ) {
                            CurrentText[k][l] = CurrentText[k+1][l];
                            CurrentAttribute[k][l] = CurrentAttribute[k+1][l];
                        }
                        if( l < MaxColumns ) {
                            for( ; l < MaxColumns; l++ ) CurrentText[k][l] = 0x20;
                        }
                        LineLength[k] = LineLength[k+1];
                    }
                    for( l = 0; l < MaxColumns; l++ ) CurrentText[k][l] = 0x20;
                }
                ++This->Mode->CursorRow;
            }
  
            if (This->Mode->CursorRow < 0) {
                This->Mode->CursorRow = 0;
            } else if (This->Mode->CursorRow >= MaxRows) {
                This->Mode->CursorRow = MaxRows-1;
            }
        }
    }

    if (TerminalDev->TerminalType == VT_UTF8) {
        Status = WriteToSerialPort(TerminalDev->SerialIo, Buffer, j);
    } else {
        Status = WriteToSerialPort(TerminalDev->SerialIo,
                                    Buffer, StringLength);
    }

    pBS->FreePool(Buffer);

    if (!EFI_ERROR(Status) && Warning == TRUE) {
        Status = EFI_WARN_UNKNOWN_GLYPH;
    }

    return Status;
}
/**
    Refresh the whole screen 
    
	@param  VOID
	
    @retval  VOID

**/

VOID
RefreshScreen ()
{
    EFI_STATUS  Status;
    UINTN       i,j;
    CHAR16      String[]={0,0};
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This;
    UINTN       Index;
    UINTN       MaxColumns = MODE_0_MAX_COLUMNS;
    UINTN       MaxRows = MODE_0_MAX_ROWS;
    UINT32      CurrentRow,CurrentColumn;

    for(Index=0; Index<TotalTerminalDev;Index++) {
        if(gTerminalDev[Index] == NULL) {
            continue;
        }
        This=&(gTerminalDev[Index]->SimpleTextOutput);
        CurrentRow = This->Mode->CursorRow;
        CurrentColumn = This->Mode->CursorColumn;
        if (This->Mode->Mode == 2) {
            MaxColumns = MODE_2_MAX_COLUMNS;
            MaxRows = MODE_2_MAX_ROWS;
        } else if (This->Mode->Mode == 0) {
            MaxColumns = MODE_0_MAX_COLUMNS;
            MaxRows = MODE_0_MAX_ROWS;
        }


        // Clear screen and set cursor to home.
        Status = TerminalOutputEscapeSequence(This, gClearScreenEscapeSequence);
        
        if(!EFI_ERROR(Status)){
            This->Mode->CursorColumn = 0;
            This->Mode->CursorRow = 0;
        }

        for(i=0; i<MaxRows;i++) {
            for(j=0; j<MaxColumns;j++) {
                TerminalSetCursorPosition(This,(UINTN)j,(UINTN)i);
                if(CurrentAttribute[i][j] != 0xff) {
                    TerminalSetAttribute(This,CurrentAttribute[i][j]);
                }
                // If the character is the very last one on the screen,
                // don't print it. It causes an unwanted wrap.
                if( (i == (MaxRows-1)) && (j == (MaxColumns-1)) ) {
                } else {
                    String[0]=CurrentText[i][j];   
                    TerminalOutputStringHelper(This,&String[0], FALSE);
                }

             }
        }

        Status = TerminalSetCursorPosition(This, CurrentColumn, CurrentRow);
    }
}

/**
    Wrapper function for TerminalOutputStringHelper.
    TerminalOutputStringHelper is called with the
    UpdateCursorPosition parameter set to TRUE.

    @param  This
    @param  String

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalOutputString (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *String
)
{
    return TerminalOutputStringHelper(This, String, TRUE);
}

/**
    Calls TerminalOutputStringHelper with the
    UpdateCursorPosition parameter set to FALSE.

    @param  This
    @param  EscapeSequence

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalOutputEscapeSequence (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *EscapeSequence
)
{
    TERMINAL_DEV *TerminalDev =
                            OUTTER(This, SimpleTextOutput, TERMINAL_DEV);

    if(TerminalDev->RecorderMode) {
        return EFI_SUCCESS;
    }

    return TerminalOutputStringHelper(This, EscapeSequence, FALSE);
}

/**
    Writes the given string to the screen.
    Returns an error if the characters could not be encoded
    properly if there was some problem writing them to the screen.

    @param  This
    @param  String

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalTestString (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *String
)
{
  EFI_STATUS     Status = EFI_SUCCESS;
    UINT8        i = 0;
    CHAR16       c = 0;
    UTF8_CHAR    Utf8Char; 
    UINT8        NumBytes = 0; 
    TERMINAL_DEV *TerminalDev = 
                            OUTTER(This, SimpleTextOutput, TERMINAL_DEV);

    while((c = String[i++]) != 0)
    {
        switch(TerminalDev->TerminalType) {
            case ANSI:
                if (!isValidAnsi(c) && !TranslateToAnsi(c))
                Status = EFI_UNSUPPORTED;
                break;
            case VT100:
            case VT100Plus:
                if (!isValidAscii(c) && !TranslateToAscii(c))
                Status = EFI_UNSUPPORTED;
                break;
            case VT_UTF8:
                UnicodeToUtf8(c, &Utf8Char, &NumBytes);
                if (NumBytes == 0)
                Status = EFI_UNSUPPORTED;
                break;
        }
    }

    return Status;
}

/**
    We only support whatever mode is specified by the setup
    question "Resolution."
    Current possible values for the setup question are
    modes 0 and 2.
    If the mode is supported, that if the parameter "ModeNumber"
    matches the setup question value the function returns the
    max number of rows and columns for the given ModeNumber.


    @param  This
    @param  ModeNumber
    @param  Columns
    @param  Rows

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalQueryMode (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            ModeNumber,
    OUT UINTN                           *Columns,
    OUT UINTN                           *Rows
)
{
    if ((INT32)ModeNumber >= This->Mode->MaxMode || ModeNumber == 1) return EFI_UNSUPPORTED;

    if (ModeNumber == 2) {
	    *Columns = MODE_2_MAX_COLUMNS;
	    *Rows = MODE_2_MAX_ROWS;
    } else if (ModeNumber == 0) {
	    *Columns = MODE_0_MAX_COLUMNS;
	    *Rows = MODE_0_MAX_ROWS;
    }

    return EFI_SUCCESS;
}

/**
    In our implementation, we only support mode 0 and Mode 1,
    which is MODE_0_MAX_ROWS or MODE_2_MAX_ROWS and  MODE_0_MAX_COLUMNS
    or MODE_2_MAX_COLUMNS respectively.

    @param  This
    @param  ModeNumber

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalSetMode (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            ModeNumber
)
{
    UINTN       i,j;
    UINTN       MaxColumns,MaxRows;

    if ((INT32)ModeNumber >= This->Mode->MaxMode || ModeNumber == 1) return EFI_UNSUPPORTED;

    if(ModeNumber == 2) {
        MaxColumns = MODE_2_MAX_COLUMNS;
        MaxRows = MODE_2_MAX_ROWS;
    } else {
        MaxColumns = MODE_0_MAX_COLUMNS;
        MaxRows = MODE_0_MAX_ROWS;
    }

    This->Mode->Mode = (INT32)ModeNumber;

    for(i=0; i<MaxRows;i++) {
        for(j=0; j<MaxColumns;j++) {
            CurrentAttribute[i][j] = 0xff;
        }
    }
    return TerminalClearScreen(This);
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
EFIAPI
TerminalSetAttribute (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            Attribute
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    //Attribute: [7]: Blinking mode if set to 1.
    //Attribute: [6-4]: BackgroundColor
    //Attribute: [3]: Is foreground color light or dark?
    //                  If so, make the text bold. 
    //Attrubute: [2-0]: Foreground Color
    UINT8 ForegroundColor = gForegroundColors[Attribute & 0x7];
    UINT8 Bold = (UINT8)((Attribute >> 3) & 0x1);
    UINT8 Blink = (UINT8)((Attribute >> 7) & 0x1);
    UINT8 BackgroundColor = gBackgroundColors[(Attribute >> 4) & 0x7];
    //SetAttributeEscapeSequence for ANSI & others: 
    //  Escape '[' Bold ';' ForegroundColor ';' BackgroundColor 'm' NULL
    //1 char for Bold and up to 2 each for ForegroundColor and
    //BackgroundColor
    CHAR16 SetAttributeEscapeSequence[11];
    UINT8 BlinkSequence = 0x5;
    //SetAttributeEscapeSequence for VT100:
    //  Escape '[' Bold or Blink 'm' NULL
    //1 char for Bold and 1 char for Blink

    TERMINAL_DEV *TerminalDev =
                            OUTTER(This, SimpleTextOutput, TERMINAL_DEV);

    if (This->Mode->Attribute != (INT32)Attribute) {
        //
        //Check for VT100 Terminal Type.if so send the escape sequence for bold & blink.
        //
        if (TerminalDev->TerminalType == VT100) {
            if (Blink) {
                Swprintf(SetAttributeEscapeSequence, L"%c[%dm",
                            ASCII_ESC, BlinkSequence);

                Status = TerminalOutputEscapeSequence(This,
                                                        SetAttributeEscapeSequence);
                    if (EFI_ERROR(Status)) {
                        return Status;
                    }
            }
            //
            //Send the escape sequence for bold
            //
            Swprintf(SetAttributeEscapeSequence, L"%c[%dm",
                        ASCII_ESC, Bold);

            Status = TerminalOutputEscapeSequence(This,
                                                    SetAttributeEscapeSequence);
            if (EFI_ERROR(Status)) {
                return Status;
            }
        //
        //it is not VT100 type,send escape sequence for ANSI Type.
        //
        } else {
            Swprintf(SetAttributeEscapeSequence, L"%c[%d;%d;%dm",
                        ASCII_ESC, Bold, ForegroundColor, BackgroundColor);
            Status = TerminalOutputEscapeSequence(This,
                                                    SetAttributeEscapeSequence);
            if (EFI_ERROR(Status)) {
                return Status;
            }
        }
    }

    This->Mode->Attribute = (INT32)Attribute;

    return EFI_SUCCESS;
}

/**
    Writes an escape sequence to the serial port
    to clear the screen.

    @param  This

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalClearScreen (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This
)
{
  EFI_STATUS Status;
  UINT8       i,j;
  UINT8       MaxColumns = MODE_0_MAX_COLUMNS;
  UINT8       MaxRows = MODE_0_MAX_ROWS;

    //Clear screen and set cursor to home.
    Status = TerminalOutputEscapeSequence(This, gClearScreenEscapeSequence);

    if (!EFI_ERROR(Status)) {
        if (This->Mode->Mode == 2) {
            MaxColumns = MODE_2_MAX_COLUMNS;
            MaxRows = MODE_2_MAX_ROWS;
        } else if (This->Mode->Mode == 0) {
            MaxColumns = MODE_0_MAX_COLUMNS;
            MaxRows = MODE_0_MAX_ROWS;
        }

        //
        // Clear CurrentText buffer and its attributes when clearing the
        // Terminal screen
        //
        for(i = 0; i < MaxRows; i++){
            for(j = 0; j < MaxColumns; j++){
                CurrentText[i][j]= 0;
                CurrentAttribute[i][j]= 0xff;
            }
        }

        This->Mode->CursorColumn = 0;
        This->Mode->CursorRow = 0;
        TerminalSetCursorPosition( This, 0, 0); 
    }

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
EFIAPI
TerminalSetCursorPosition (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            Column,
    IN UINTN                            Row
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    //SetCursorPositionEscapeSequence: ESC '[' Row ';' Column 'H' NULL
    //-up to 2 characters each for the row and column numbers 
    CHAR16 SetCursorPositionEscapeSequence[9];

    if(This->Mode->Mode == 2) {
	    if (Column >= MODE_2_MAX_COLUMNS || Row >= MODE_2_MAX_ROWS) {
            return EFI_UNSUPPORTED;
        }
    } else if (This->Mode->Mode == 0) {
	    if (Column >= MODE_0_MAX_COLUMNS || Row >= MODE_0_MAX_ROWS) {
            return EFI_UNSUPPORTED;
        }
    }

    Swprintf(SetCursorPositionEscapeSequence, L"%c[%d;%dH",
                                ASCII_ESC, (UINT8)Row+1,(UINT8)Column+1);
    Status = TerminalOutputEscapeSequence(This, 
                                        SetCursorPositionEscapeSequence);

    if (!EFI_ERROR(Status)) {
        This->Mode->CursorColumn = (INT32)Column;
        This->Mode->CursorRow = (INT32)Row;
    }

    return Status;
}

/**
    In our implementation, the parameter Visible should be set
    to TRUE.  We cannot make the cursor invisible.

    @param  This
    @param  Visible

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalEnableCursor (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN BOOLEAN                          Visible
)
{
    //
    //Terminal doesn't support invisible cursor
    //    
    if (!Visible) return EFI_UNSUPPORTED;
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
