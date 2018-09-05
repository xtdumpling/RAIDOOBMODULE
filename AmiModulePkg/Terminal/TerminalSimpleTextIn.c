//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.01
//      Bug Fixed: This driver didn't support "Ctrl+Home".
//      Reason   : Pressing "Ctrl+Home" to do BIOS recovery.
//      Auditor  : Yenhsin Cho
//      Date     : Apr/10/2017
//
//     Rev. 1.0
//      Bug Fix: Patch CTRL+HOME can't work via console
//       Reason:	Function improve.
//       Auditor:   Jacker Yeh (Refer from Grantley)
//       Date:      Oct/27/2016
//
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

/** @file TerminalSimpleTextIn.c
    Implements the SimpleTextIn protocol for the Terminal.
    Interfaces with the SerialIo protocol to get and write 
    characters to the serial port.
    
**/

//---------------------------------------------------------------------------
#include "Token.h" //SMCPKG_SUPPORT
#include "Terminal.h"

//---------------------------------------------------------------------------

extern UINT32 TimeoutForDeterminingLoneEscChar;
extern UINT32 EscSequenceCheckingIntervel;
extern CHAR16  RefreshScreenKey;
extern BOOLEAN ASCIIControlCodeSupport;
EFI_STATUS  TerminalCheckKeyNotify(AMI_EFI_KEY_DATA *Key);

DLIST   mTerminalKeyboardData;
KEY_WAITING_RECORD  *mTerminalKeyboardRecord;

extern STATE_DESCRIPTION *EscSeqStateArrayEndMarkerPtr;

typedef struct _ASCII_CONTROL_CODE {
    CHAR16  UniCode;
    CHAR16  ConvertedUniCode;
    BOOLEAN CtrlKeyPressed;
}ASCII_CONTROL_CODE;

ASCII_CONTROL_CODE ASCIIControlCodeTable[]={
    {0x00, 0x00, FALSE},
    {0x01, 0x61, TRUE}, //a
    {0x02, 0x62, TRUE}, //b
    {0x03, 0x63, TRUE}, //c
    {0x04, 0x64, TRUE}, //d
    {0x05, 0x65, TRUE}, //e
    {0x06, 0x66, TRUE}, //f
    {0x07, 0x67, TRUE}, //g
    {0x08, 0x08, FALSE},//h BackSpace
    {0x09, 0x09, FALSE},//i TAB Key
    {0x0a, 0x0a, FALSE},//j Line Feed
    {0x0b, 0x6b, TRUE}, //k
    {0x0c, 0x6c, TRUE}, //l
    {0x0d, 0x0d, FALSE},//m Enter Key
    {0x0e, 0x6e, TRUE}, //n
    {0x0f, 0x6f, TRUE}, //o
    {0x10, 0x70, TRUE}, //p
    {0x11, 0x71, TRUE}, //q
    {0x12, 0x72, TRUE}, //r
    {0x13, 0x73, TRUE}, //s
    {0x14, 0x74, TRUE}, //t
    {0x15, 0x75, TRUE}, //u
    {0x16, 0x76, TRUE}, //v
    {0x17, 0x77, TRUE}, //w
    {0x18, 0x78, TRUE}, //x
    {0x19, 0x79, TRUE}, //y
    {0x1A, 0x7a, TRUE}, //z
};

#define RESET 0xFFFF
//**********************************************************************
//  Part1.
//  The following code defines Serail I/O Reading and Buffering Logic
//**********************************************************************

/**
    Calls SerialIo->Read to read characters from the serial port.
    The caller must allocate "Buffer."  
    On input, "Size" is the size of "Buffer."
    On output, "Size" is the number of characters read from the 
    serial port.

    @param  SerialIo
    @param  Buffer
    @param  Size

    @retval  VOID

**/

VOID
ReadSerialPort (
    IN EFI_SERIAL_IO_PROTOCOL *SerialIo,
    IN OUT UINT8 *Buffer,
    IN OUT UINTN *Size
)
{
    EFI_STATUS  Status;
    UINT32      Control;
    UINTN       ReadSize = *Size;
    UINTN       ByteSize = 1;

#if MAX_DATA_READ_RETRY_COUNT
	UINTN		RetryCount = 0;
#endif
    *Size = 0;

    while (1) {
        //Exit condition for while(1)
        //1. ReadSize becomes Zero.
        //2. RetryCount becomes Zero.
		//3. Data read fails.        
        if (!ReadSize)
            return;
        //
        //Check if Character.
        //
        Status = SerialIo->GetControl(SerialIo,
                                      &Control);
        
        if (EFI_ERROR(Status) || Control & EFI_SERIAL_INPUT_BUFFER_EMPTY) {
#if MAX_DATA_READ_RETRY_COUNT
			if (RetryCount == 0) {
           		return;
			}         
       		pBS->Stall(DATA_READ_RETRY_DELAY);
			RetryCount--;
			continue;

#else
			return;  
#endif
        }

        //
        //Read single character to avoid timeout.
        //
        Status = SerialIo->Read(SerialIo,
                                &ByteSize,
                                Buffer
        );
               
        if (EFI_ERROR(Status)) {
#if MAX_DATA_READ_RETRY_COUNT
			if (RetryCount == 0) {
				return;
			}
       		pBS->Stall(DATA_READ_RETRY_DELAY);
			RetryCount--;
			continue;

#else
			return;  
#endif
        }

#if MAX_DATA_READ_RETRY_COUNT
        // Set Retry count when first byte is read successfully.
		RetryCount = MAX_DATA_READ_RETRY_COUNT;
#endif
		++Buffer;
		++*Size;
		ReadSize--;
	}
}

/**
    Fills our internal raw buffer TerminalDev->RawFIFO
    with characters read from the serial port.

    @param  SerialIo
    @param  Buffer
    @param  Start
    @param  Size

    @retval VOID

**/

VOID
FillRawBuffer (
    EFI_SERIAL_IO_PROTOCOL *SerialIo,
    CHAR8                  *Buffer,
    UINTN                  Start,
    UINTN                  *Size
)
{
    UINTN   FillSize1;
    UINTN   FillSize2;
    UINTN   TempSize;

    if (Start + *Size > BUFFER_SIZE) {
        //
        //Buffer Tail to end of buffer.
        //
        FillSize1 = BUFFER_SIZE - Start;
        //
        //Wrap around.
        //
        FillSize2 = *Size - FillSize1;
    } else {
        FillSize1 = *Size;
        FillSize2 = 0;
    }

    TempSize = FillSize1;
    //      
    //Fill to end of buffer.
    //  
    ReadSerialPort(SerialIo, (UINT8*)(Buffer + Start), &TempSize);

    *Size = TempSize;
    //
    //Less characters to read then Size.
    //
    if (TempSize < FillSize1 || !FillSize2) {
        return; 
    }

    //Buffer Wrap around.
    //NB: FillSize2 is updated.
    ReadSerialPort(SerialIo, (UINT8*)Buffer, &FillSize2);
    *Size = FillSize1 + FillSize2;
}

/**
    Removes a character from our internal raw buffer 
    TerminalDev->RawFIFO.  

    @param  TerminalDev
    @param  pByte

    @retval  BOOLEAN

**/

BOOLEAN
RemoveCharFromRawBuffer (
    TERMINAL_DEV *TerminalDev,
    UINT8*       pByte
)
{
    if (TerminalDev->RawFIFO.Head == TerminalDev->RawFIFO.Tail) {
        return FALSE;
    }
    *pByte = TerminalDev->RawFIFO.Buffer[TerminalDev->RawFIFO.Head++];
    if (TerminalDev->RawFIFO.Head >= BUFFER_SIZE) {
        TerminalDev->RawFIFO.Head-=BUFFER_SIZE;
    }
    return TRUE;
}

/**
    Adds a character to our internal unicode buffer 
    TerminalDev->UnicodeFIFO.

    @param  TerminalDev
    @param  UnicodeChar

    @retval  VOID

**/

VOID
AddCharToUnicodeBuffer (
    TERMINAL_DEV *TerminalDev,
    CHAR16       UnicodeChar
)
{
    if (((TerminalDev->UnicodeFIFO.Tail + 1) % BUFFER_SIZE) == 
                                        TerminalDev->UnicodeFIFO.Head) {
        return;
    }
    TerminalDev->UnicodeFIFO.Buffer[TerminalDev->UnicodeFIFO.Tail++] = 
                                                            UnicodeChar;
    if (TerminalDev->UnicodeFIFO.Tail >= BUFFER_SIZE) {
        TerminalDev->UnicodeFIFO.Tail -= BUFFER_SIZE;
    }

}

/**
    Creates a Uft8 char from our internal raw buffer of
    characters TerminalDev->RawFIFO.

    @param  TerminalDev
    @param  Utf8Char
    @param  NumBytes

    @retval  BOOLEAN

**/

BOOLEAN
ExtractUtf8CharFromRawBuffer (
    TERMINAL_DEV *TerminalDev,
    UTF8_CHAR*   Utf8Char,
    UINT8        *NumBytes
)
{
    UINT8 Char = 0;
    *NumBytes = 0;
 
    if (RemoveCharFromRawBuffer(TerminalDev, &Char)) {
        if ((Char & 0x80) == 0) {   //1 byte utf8 char
            Utf8Char->UTF8_1 = Char;
            *NumBytes = 1;
            return TRUE;
        }
    } else {
        return FALSE;
    }

    if ((Char & 0xe0) == 0xc0) {    //2 byte utf8 char
        Utf8Char->UTF8_2[1] = Char; //2nd byte
        if (RemoveCharFromRawBuffer(TerminalDev, &Char)) {
            if ((Char & 0xc0) == 0x80) {
                Utf8Char->UTF8_2[0] = Char;     //1st byte
                *NumBytes = 2; 
                return TRUE; 
            } 
        } 
        return FALSE;
    }

    if ((Char & 0xf0) == 0xe0) {    //3 byte utf8 char
        Utf8Char->UTF8_3[2] = Char; //3rd byte
        if (RemoveCharFromRawBuffer(TerminalDev, &Char)) {
            if ((Char & 0xc0) == 0x80) {    //2nd byte
                Utf8Char->UTF8_3[1] = Char;
                if (RemoveCharFromRawBuffer(TerminalDev, &Char)) {
                    if ((Char & 0xc0) == 0x80) {    //1st byte
                        Utf8Char->UTF8_3[0] = Char;
                        *NumBytes = 3;
                        return TRUE;
                    }
                }
            }
        }
    } 
    return FALSE;

}

/**
    Creates a Unicode character from a Utf8 character.

    @param  Utf8Char
    @param  NumBytes
    @param  UnicodeChar

    @retval  VOID

**/

VOID
ConvertUtf8ToUnicode (
    UTF8_CHAR Utf8Char,
    UINT8     NumBytes,
    CHAR16    *UnicodeChar
)
{
    UINT8 UnicodeByte0 = 0;
    UINT8 UnicodeByte1 = 0;
    UINT8 Byte0 = 0;
    UINT8 Byte1 = 0;
    UINT8 Byte2 = 0;

    *UnicodeChar = L'\0';

    switch (NumBytes) {
        case 1:
            *UnicodeChar = (UINT16) Utf8Char.UTF8_1;
            break;

        case 2:
            Byte0         = Utf8Char.UTF8_2[0];
            Byte1         = Utf8Char.UTF8_2[1];

            UnicodeByte0  = (UINT8) ((Byte1 << 6) | (Byte0 & 0x3f));
            UnicodeByte1  = (UINT8) ((Byte1 >> 2) & 0x07);
            *UnicodeChar  = (UINT16) (UnicodeByte0 | (UnicodeByte1 << 8));
            break;

        case 3:
            Byte0         = Utf8Char.UTF8_3[0];
            Byte1         = Utf8Char.UTF8_3[1];
            Byte2         = Utf8Char.UTF8_3[2];

            UnicodeByte0  = (UINT8) ((Byte1 << 6) | (Byte0 & 0x3f));
            UnicodeByte1  = (UINT8) ((Byte2 << 4) | ((Byte1 >> 2) & 0x0f));
            *UnicodeChar  = (UINT16) (UnicodeByte0 | (UnicodeByte1 << 8));

        default:
            break;
  }
}


/**
    Fills our internal Unicode buffer TerminalDev->UnicodeFIFO
    from our internal buffer of raw characters
    TerminalDev->RawFIFO.
    If the terminal type is Utf8, the bytes from the RawFIFO have
    to be decoded to form Unicode characters.
    In the case of all other supported terminal types
    (ANSI, VT100, VT100+), no processing is needed and characters
    are simply transferred from one buffer to the other.

    @param  TerminalDev

    @retval  void

**/

void
FillUnicodeBufferFromSerial (
    TERMINAL_DEV *TerminalDev
)
{
    UINTN FreeBuffer;
    UINTN AdjTail;

    UINT8 Char = 0;
    UINT8 NumBytes = 0;
    UTF8_CHAR Utf8Char;
    CHAR16 UnicodeChar = 0;

#if MAX_DATA_READ_RETRY_COUNT
    // Reset FIFO to avoid data loss
    if (TerminalDev->RawFIFO.Head == TerminalDev->RawFIFO.Tail){ 
        TerminalDev->RawFIFO.Head = TerminalDev->RawFIFO.Tail = 0;
    }
#endif
    
    AdjTail = (TerminalDev->RawFIFO.Tail < TerminalDev->RawFIFO.Head) ?
                TerminalDev->RawFIFO.Tail + BUFFER_SIZE :
                TerminalDev->RawFIFO.Tail;

    //Subtract 1, must have one unused byte for calculations to determine
    //if buffer full.
    FreeBuffer = BUFFER_SIZE - (AdjTail - TerminalDev->RawFIFO.Head) - 1;
    if (!FreeBuffer) {
        return;    //Buffer full
    }

    FillRawBuffer(TerminalDev->SerialIo, TerminalDev->RawFIFO.Buffer,
                    TerminalDev->RawFIFO.Tail, &FreeBuffer);
    TerminalDev->RawFIFO.Tail += (UINT16)FreeBuffer;

    if (TerminalDev->RawFIFO.Tail >= BUFFER_SIZE) {
        TerminalDev->RawFIFO.Tail -= BUFFER_SIZE;
    }

    switch(TerminalDev->TerminalType) {
        case ANSI:
        case VT100:
        case VT100Plus:
            while (RemoveCharFromRawBuffer(TerminalDev, &Char)) {
                AddCharToUnicodeBuffer(TerminalDev, Char);
            }
            break;
        case VT_UTF8:
            while(ExtractUtf8CharFromRawBuffer(TerminalDev,&Utf8Char,&NumBytes)) {
                ConvertUtf8ToUnicode(Utf8Char, NumBytes, &UnicodeChar);
                AddCharToUnicodeBuffer(TerminalDev, UnicodeChar);
            }
            break;
    }
}

//**********************************************************************
//  Part2. 
//  The following code defines Logic to convert input stream into Keys
//**********************************************************************

/**
    Removes a character from our internal 
    character buffer TerminalDev->UnicodeFIFO.

    @param  TerminalDev
    @param  UnicodeChar

    @retval  BOOLEAN

**/

BOOLEAN
RemoveCharFromUnicodeBuffer (
    TERMINAL_DEV *TerminalDev,
    CHAR16*      UnicodeChar
)
{
    if (TerminalDev->UnicodeFIFO.Head >= BUFFER_SIZE) {
        TerminalDev->UnicodeFIFO.Head -= BUFFER_SIZE;
    }
    if (TerminalDev->UnicodeFIFO.Head == TerminalDev->UnicodeFIFO.Tail) {
        return FALSE;
    }
    *UnicodeChar = 
        TerminalDev->UnicodeFIFO.Buffer[TerminalDev->UnicodeFIFO.Head++];
    return TRUE;
}

/**
    Puts the given Unicode character back into our internal
    character buffer TerminalDev->UnicodeFIFO.

    @param  TerminalDev
    @param  UnicodeChar

    @retval  VOID

**/

VOID
PutBack (
    TERMINAL_DEV *TerminalDev,
    UINT16       UnicodeChar
)
{
    //
    //buffer is full; can't put the character back
    //
    if (TerminalDev->UnicodeFIFO.Head==TerminalDev->UnicodeFIFO.Tail+1) {
        return ;
    }

    if (!TerminalDev->UnicodeFIFO.Head) {
        TerminalDev->UnicodeFIFO.Head=BUFFER_SIZE;
    }
    TerminalDev->UnicodeFIFO.Buffer[--TerminalDev->UnicodeFIFO.Head]=UnicodeChar;
}

/**
    Convert the ASCII control codes into CTRL+Unicode(Key)values

    @param  pKey

    @retval  VOID

**/

VOID
ASCIIControlCodeConversion (
    AMI_EFI_KEY_DATA    *pKey
)
{
    UINT8 Ch;
      
    Ch=(UINT8)((AMI_EFI_KEY_DATA*)pKey)->Key.UnicodeChar;
    if(Ch <= 0x1A) {
        ((AMI_EFI_KEY_DATA*)pKey)->Key.UnicodeChar=ASCIIControlCodeTable[Ch].ConvertedUniCode;
        if(ASCIIControlCodeTable[Ch].CtrlKeyPressed) {
            ((AMI_EFI_KEY_DATA*)pKey)->KeyState.KeyShiftState |= LEFT_CONTROL_PRESSED;
        }
    }

   return;
}

/**
    The function polls the serial port to see if a key is 
    available.
    If a key is available, checks for the ESC sequnce until time out happens.
    If ESC sequence found then stops the timer or timeout happens also this 
    event will be closed

    @param  Event
    @param  Context

    @retval  VOID

**/

VOID
EFIAPI
WaitForEsc (
    EFI_EVENT Event,
    VOID      *Context
)
{
    EFI_STATUS   Status;
    TERMINAL_DEV *TerminalDev=(TERMINAL_DEV*)(*(UINTN*)Context);

    if(TerminalDev == NULL) {
        return;
    }

    if(TerminalDev->EscSeqStatePtr == NULL) {
        pBS->SetTimer(TerminalDev->TimeoutEscEvent, TimerCancel, 0);
        return;
    }
    Status=CheckKeyboardDataFromSerial(TerminalDev);
    if(Status==EFI_SUCCESS || TerminalDev->EscSeqStatePtr == NULL) {
        pBS->SetTimer(TerminalDev->TimeoutEscEvent, TimerCancel, 0);
    }
    return;
}

/**
    Reads the serial port and returns a corresponding
    AMI_EFI_KEY_DATA value. 
    In some cases, multiple bytes must be read from the
    serial port to determine an EFI_INPUT_KEY value
    (e.g. escape sequences and Utf8 encoded characters).

    @param  TerminalDev
    @param  TerminalKey

    @retval  EFI_STATUS

**/

EFI_STATUS
GetKeyFromSerial (
    IN TERMINAL_DEV     *TerminalDev, 
    IN AMI_EFI_KEY_DATA *TerminalKey
)
{
    UINT16 InputChar;

    FillUnicodeBufferFromSerial(TerminalDev);

    if (TerminalDev->EscSeqStatePtr == NULL) {
        //
        //If there are no chars in the buffer, return.
        //
        if(!RemoveCharFromUnicodeBuffer(TerminalDev, &InputChar)) {
            return EFI_NOT_READY;
        }

        //
        //We have a char from the buffer...
        //

        //
        //If the char is not an escape char...
        //
        if (InputChar != ASCII_ESC) {
            TerminalKey->Key.ScanCode = EFI_SCAN_NULL;
            TerminalKey->Key.UnicodeChar = InputChar;
            return EFI_SUCCESS;
        }

        //
        //Else the char is an escape char.
        //  
        TerminalDev->EscSeqStatePtr = TerminalDev->TerminalEscSeqState;
        pBS->SetTimer(TerminalDev->TimeoutEvent,
                        TimerRelative,
                        TimeoutForDeterminingLoneEscChar);
    }

    //
    //Do we have just a lone escape char or an escape sequence?
    //
    if (EFI_ERROR(pBS->CheckEvent(TerminalDev->TimeoutEvent)))
    {
        if(!RemoveCharFromUnicodeBuffer(TerminalDev, &InputChar)) {
            return EFI_NOT_READY;
        }

        TerminalDev->InputChars[TerminalDev->InputCharIndex++] = InputChar;
        while (TerminalDev->EscSeqStatePtr->InputChar != EscSeqStateArrayEndMarkerPtr->InputChar) {
            if ((TerminalDev->EscSeqStatePtr->InputChar == InputChar) && (TerminalDev->EscSeqStatePtr->KeyPad & TerminalDev->PuttyKeyPad)) {
                if (TerminalDev->EscSeqStatePtr->NextEscSeqState == NULL) {
                    //The input character matches the current character
                    //in the escape sequence. 
                    //There are no more characters to match in the escape
                    //sequence.
                    //We are done.
                    //Return the Scan Code and Unicode Char for this escape
                    //sequence.

                    if (TerminalDev->EscSeqStatePtr->EfiInputKeyScanCode == RESET) {
                        pRS->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
                    }

                    TerminalKey->Key.ScanCode =
                        TerminalDev->EscSeqStatePtr->EfiInputKeyScanCode;
                    TerminalKey->Key.UnicodeChar =
                        TerminalDev->EscSeqStatePtr->EfiInputKeyUnicodeChar;
                    TerminalDev->InputCharIndex = 0;
                    TerminalDev->EscSeqStatePtr = NULL;
                    break;
                } else {
#if SMCPKG_SUPPORT
                    if(TerminalDev->EscSeqStatePtr->InputChar == 0x3){  // Ctrl
                        TerminalKey->Key.ScanCode = 0;
                        TerminalKey->Key.UnicodeChar = 0;
                        TerminalKey->KeyState.KeyShiftState |= LEFT_CONTROL_PRESSED;
                        TerminalKey->KeyState.KeyToggleState = TerminalDev->KeyToggleState;
                    }
#endif
                    //The input character matches the current character
                    //in the escape sequence.
                    //Move on to the next character in the escape sequence.

                    //Reset the timer to wait for next key.
                    pBS->SetTimer(TerminalDev->TimeoutEvent,
                                    TimerRelative,
                                    TimeoutForDeterminingLoneEscChar);
                    TerminalDev->EscSeqStatePtr =
                            TerminalDev->EscSeqStatePtr->NextEscSeqState;
                    if(!RemoveCharFromUnicodeBuffer(TerminalDev, &InputChar)) {
                        return EFI_NOT_READY;
                    }
                    TerminalDev->InputChars[TerminalDev->InputCharIndex++] =
                                                                InputChar;
                }
            } else {
                //The input character does NOT match the current character
                //in the escape sequence.
                //Move on to the next escape sequence.
                TerminalDev->EscSeqStatePtr++;
            }
        }
        pBS->SetTimer(TerminalDev->TimeoutEvent, TimerCancel, 0);
        if (TerminalDev->EscSeqStatePtr == NULL) {
            return EFI_SUCCESS;
        }
    }
    //What we have is a lone escape character, not an escape sequence.
    //Put the characters after the escape character back into the buffer.
    //Return EFI_SCAN_ESC.
    while(TerminalDev->InputCharIndex) {
        PutBack(TerminalDev, 
                TerminalDev->InputChars[--TerminalDev->InputCharIndex]);
    }
    TerminalKey->Key.ScanCode = EFI_SCAN_ESC;
    TerminalKey->Key.UnicodeChar = EFI_SCAN_NULL;
    TerminalDev->EscSeqStatePtr = NULL;
    return EFI_SUCCESS;
}

typedef struct _CHAR_TO_EFI {
    UINT8   EfiKey;
    UINT8   Ps2ScanCode;
    BOOLEAN IsUpperCase;
} CHAR_TO_EFI;

static CHAR_TO_EFI CharToEfiTable[] = {
// 0x00..0x1f
    EfiKeySpaceBar, 0x39, FALSE, // 0x20
    EfiKeyE1,  0x02, TRUE, // 0x21 33 !
    EfiKeyC11, 0x28, TRUE, // 0x22 34 "
    EfiKeyE3,  0x04, TRUE, // 0x23 35 #
    EfiKeyE4,  0x05, TRUE, // 0x24 36 $
    EfiKeyE5,  0x06, TRUE, // 0x25 37 %
    EfiKeyE7,  0x08, TRUE, // 0x26 38 &
    EfiKeyC11, 0x28, FALSE,// 0x27 39 '
    EfiKeyE9,  0x0a, TRUE, // 0x28 40 (
    EfiKeyE10, 0x0b, TRUE, // 0x29 41 )
    EfiKeyE8,  0x09, TRUE, // 0x2a 42 *
    EfiKeyE12, 0x0d, TRUE, // 0x2b 43 +
    EfiKeyB8,  0x33, FALSE,// 0x2c 44 ,
    EfiKeyE11, 0x0c, FALSE,// 0x2d 45 -
    EfiKeyB9,  0x34, FALSE,// 0x2e 46 .
    EfiKeyB10, 0x35, FALSE,// 0x2f 47 /
    EfiKeyE10, 0x0b, FALSE,// 0x30 48 0
    EfiKeyE1,  0x02, FALSE,// 0x31 49 1
    EfiKeyE2,  0x03, FALSE,// 0x32 50 2
    EfiKeyE3,  0x04, FALSE,// 0x33 51 3
    EfiKeyE4,  0x05, FALSE,// 0x34 52 4
    EfiKeyE5,  0x06, FALSE,// 0x35 53 5
    EfiKeyE6,  0x07, FALSE,// 0x36 54 6
    EfiKeyE7,  0x08, FALSE,// 0x37 55 7
    EfiKeyE8,  0x09, FALSE,// 0x38 56 8
    EfiKeyE9,  0x0a, FALSE,// 0x39 57 9
    EfiKeyC10, 0x27, TRUE, // 0x3a 58 :
    EfiKeyC10, 0x27, FALSE,// 0x3b 59 ;
    EfiKeyB8,  0x33, TRUE, // 0x3c 60 <
    EfiKeyE12, 0x0d, FALSE,// 0x3d 61 =
    EfiKeyB9,  0x34, TRUE, // 0x3e 62 >
    EfiKeyB10, 0x35, TRUE, // 0x3f 63 ?
    EfiKeyE2,  0x03, TRUE, // 0x40 64 @
    EfiKeyC1,  0x1e, TRUE, // 0x41 65 A
    EfiKeyB5,  0x30, TRUE, // 0x42 66 B
    EfiKeyB3,  0x2e, TRUE, // 0x43 67 C
    EfiKeyC3,  0x20, TRUE, // 0x44 68 D
    EfiKeyD3,  0x12, TRUE, // 0x45 69 E
    EfiKeyC4,  0x21, TRUE, // 0x46 70 F
    EfiKeyC5,  0x22, TRUE, // 0x47 71 G
    EfiKeyC6,  0x23, TRUE, // 0x48 72 H
    EfiKeyD8,  0x17, TRUE, // 0x49 73 I
    EfiKeyC7,  0x24, TRUE, // 0x4a 74 J
    EfiKeyC8,  0x25, TRUE, // 0x4b 75 K
    EfiKeyC9,  0x26, TRUE, // 0x4c 76 L
    EfiKeyB7,  0x32, TRUE, // 0x4d 77 M
    EfiKeyB6,  0x31, TRUE, // 0x4e 78 N
    EfiKeyD9,  0x18, TRUE, // 0x4f 79 O
    EfiKeyD10, 0x19, TRUE, // 0x50 80 P
    EfiKeyD1,  0x10, TRUE, // 0x51 81 Q
    EfiKeyD4,  0x13, TRUE, // 0x52 82 R
    EfiKeyC2,  0x1f, TRUE, // 0x53 83 S
    EfiKeyD5,  0x14, TRUE, // 0x54 84 T
    EfiKeyD7,  0x16, TRUE, // 0x55 85 U
    EfiKeyB4,  0x2f, TRUE, // 0x56 86 V
    EfiKeyD2,  0x11, TRUE, // 0x57 87 W
    EfiKeyB2,  0x2d, TRUE, // 0x58 88 X
    EfiKeyD6,  0x15, TRUE, // 0x59 89 Y
    EfiKeyB1,  0x2c, TRUE, // 0x5a 90 Z
    EfiKeyD11, 0x1a, FALSE,// 0x5b 91 [
    EfiKeyD13, 0x2b, FALSE,// 0x5c 92 BackSlash
    EfiKeyD12, 0x1b, FALSE,// 0x5d 93 ]
    EfiKeyE6,  0x07, TRUE, // 0x5e 94 ^
    EfiKeyE11, 0x0c, TRUE, // 0x5f 95 _
    EfiKeyE0,  0x29, FALSE,// 0x60 96 `
    EfiKeyC1,  0x1e, FALSE,// 0x61 97 a
    EfiKeyB5,  0x30, FALSE,// 0x62 98 b
    EfiKeyB3,  0x2e, FALSE,// 0x63 99 c
    EfiKeyC3,  0x20, FALSE,// 0x64 100 d
    EfiKeyD3,  0x12, FALSE,// 0x65 101 e
    EfiKeyC4,  0x21, FALSE,// 0x66 102 f
    EfiKeyC5,  0x22, FALSE,// 0x67 103 g
    EfiKeyC6,  0x23, FALSE,// 0x68 104 h
    EfiKeyD8,  0x17, FALSE,// 0x69 105 i
    EfiKeyC7,  0x24, FALSE,// 0x6a 106 j
    EfiKeyC8,  0x25, FALSE,// 0x6b 107 k
    EfiKeyC9,  0x26, FALSE,// 0x6c 108 l
    EfiKeyB7,  0x32, FALSE,// 0x6d 109 m
    EfiKeyB6,  0x31, FALSE,// 0x6e 110 n
    EfiKeyD9,  0x18, FALSE,// 0x6f 111 o
    EfiKeyD10, 0x19, FALSE,// 0x70 112 p
    EfiKeyD1,  0x10, FALSE,// 0x71 113 q
    EfiKeyD4,  0x13, FALSE,// 0x72 114 r
    EfiKeyC2,  0x1f, FALSE,// 0x73 115 s
    EfiKeyD5,  0x14, FALSE,// 0x74 116 t
    EfiKeyD7,  0x16, FALSE,// 0x75 117 u
    EfiKeyB4,  0x2f, FALSE,// 0x76 118 v
    EfiKeyD2,  0x11, FALSE,// 0x77 119 w
    EfiKeyB2,  0x2d, FALSE,// 0x78 120 x
    EfiKeyD6,  0x15, FALSE,// 0x79 121 y
    EfiKeyB1,  0x2c, FALSE,// 0x7a 122 z
    EfiKeyD11, 0x1a, TRUE, // 0x7b 123 {
    EfiKeyD13, 0x2b, TRUE, // 0x7c 124 |
    EfiKeyD12, 0x1b, TRUE, // 0x7d 125 }
    EfiKeyE0,  0x29, TRUE,  // 0x7e 126 ~
    EfiKeyDel, 0x53, FALSE // 0x7f 127 Del
};

typedef struct _EFISCAN_TO_EFIKEY {
    UINT8   EfiScanCode;
    UINT8   EfiKey;
    UINT8   Ps2ScanCode;
} EFISCAN_TO_EFIKEY;

static EFISCAN_TO_EFIKEY EfiScanToEfiKey[] = {
    EFI_SCAN_UP,    EfiKeyUpArrow,   0x48,
    EFI_SCAN_DN,    EfiKeyDownArrow, 0x50,
    EFI_SCAN_RIGHT, EfiKeyRightArrow,0x4d,
    EFI_SCAN_LEFT,  EfiKeyLeftArrow, 0x4b,
    EFI_SCAN_HOME,  EfiKeyHome,      0x47,
    EFI_SCAN_END,   EfiKeyEnd,       0x4f,
    EFI_SCAN_INS,   EfiKeyIns,       0x52,
    EFI_SCAN_DEL,   EfiKeyDel,       0x53,
    EFI_SCAN_PGUP,  EfiKeyPgUp,      0x49,
    EFI_SCAN_PGDN,  EfiKeyPgDn,      0x51,
    EFI_SCAN_F1,    EfiKeyF1,        0x3b,
    EFI_SCAN_F2,    EfiKeyF2,        0x3c,
    EFI_SCAN_F3,    EfiKeyF3,        0x3d,
    EFI_SCAN_F4,    EfiKeyF4,        0x3e,
    EFI_SCAN_F5,    EfiKeyF5,        0x3f,
    EFI_SCAN_F6,    EfiKeyF6,        0x40,
    EFI_SCAN_F7,    EfiKeyF7,        0x41,
    EFI_SCAN_F8,    EfiKeyF8,        0x42,
    EFI_SCAN_F9,    EfiKeyF9,        0x43,
    EFI_SCAN_F10,   EfiKeyF10,       0x44,
    EFI_SCAN_F11,   EfiKeyF11,       0x85,
    EFI_SCAN_F12,   EfiKeyF12,       0x86,
    EFI_SCAN_ESC,   EfiKeyEsc,       0x01,
    0, 0, 0
};

/**
    Get the Keyboard Data

    @param  TerminalDev
    @param  pKey
    @param  KeySize

    @retval  EFI_STATUS

**/

EFI_STATUS
GetKeyData (
    IN  TERMINAL_DEV    *TerminalDev,
    OUT VOID            *pKey,
    IN  UINT8           KeySize
)
{

    EFI_STATUS          Status;
    AMI_EFI_KEY_DATA    TerminalKey;


    // Check the Keyboard buffer. If it's empty check the data from Serial Port
    if (IsTerminalKeyboardBufferEmpty (&(TerminalDev->KeyboardBuffer))) {
        Status = CheckKeyboardDataFromSerial(TerminalDev);
        
        if(!EFI_ERROR(Status) && TerminalDev->EscSeqStatePtr != NULL) {
            TerminalDev->TerminalEscIn=(VOID*)TerminalDev;
            pBS->SetTimer(TerminalDev->TimeoutEscEvent,
                        TimerPeriodic,
                        EscSequenceCheckingIntervel);
        }
    }

    // Get the Keyboard data from Buffer
    Status=RemoveKeyCode(&(TerminalDev->KeyboardBuffer),&TerminalKey);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Return the Requested Keyboard data
    pBS->CopyMem(pKey, &TerminalKey, KeySize);

    return EFI_SUCCESS;
}

/**
    Check the Keyboard Data from Serial Port and Convert them 
    into proper Keyboard data. Once the Keyboard data is found added into 
    Keyboard Local buffer      

    @param  TerminalDev

    @retval  EFI_STATUS

**/

EFI_STATUS
CheckKeyboardDataFromSerial (
    IN  TERMINAL_DEV *TerminalDev 
)
{
    EFI_STATUS          Status;
    UINT8               Ch;
    EFISCAN_TO_EFIKEY   *LookupTbl;
    UINT8               EfiKey=0;
    UINT8               Ps2ScanCode=0;
    AMI_EFI_KEY_DATA    TerminalKey;

    // Initilize the Local Buffer
    pBS->SetMem ((VOID*)&TerminalKey, sizeof (AMI_EFI_KEY_DATA), 0);

    // Get the Remote Keyboard Data from Serial Port
    Status=GetKeyFromSerial(TerminalDev, &TerminalKey);

    if(EFI_ERROR(Status)) {
        // If no Data Present, return with error.
        return Status;
    }

    // Refresh the Screen if CTRL + R or CTRL+ r key comes
    if((Status== EFI_SUCCESS) && (TerminalKey.Key.UnicodeChar == RefreshScreenKey)) {
        RefreshScreen();
        // Don't report the CTRL+R or CTRL+r data.
        return EFI_NOT_READY;
    }

    // Initilize the KeyState values
#if SMCPKG_SUPPORT
    TerminalKey.KeyState.KeyShiftState |= SHIFT_STATE_VALID;
#else
    TerminalKey.KeyState.KeyShiftState = SHIFT_STATE_VALID;
#endif
    TerminalKey.KeyState.KeyToggleState = TerminalDev->KeyToggleState;

    Ch = (UINT8)TerminalKey.Key.ScanCode;

    if (Ch != EFI_SCAN_NULL) {
        for (LookupTbl = EfiScanToEfiKey; LookupTbl->EfiScanCode; LookupTbl++) {
            if (LookupTbl->EfiScanCode == Ch) {
                break;
            }
        }

        if (LookupTbl->EfiScanCode != 0) {
    
            EfiKey = LookupTbl->EfiKey;
            Ps2ScanCode = LookupTbl->Ps2ScanCode;
        }

    } else {

        if(ASCIIControlCodeSupport) {
            ASCIIControlCodeConversion(&TerminalKey);
        }

        Ch = (UINT8)TerminalKey.Key.UnicodeChar;
      
        //
        // Process BackSpace, Tab and Enter
        //
        if (Ch < 32) {
            switch (Ch) {
                case 0x08:
                    EfiKey = EfiKeyBackSpace;
                    Ps2ScanCode = 0x0e;
                    break;
                case 0x09:
                    EfiKey = EfiKeyTab;
                    Ps2ScanCode = 0x0f;
                    break;
                case 0x0d:
                    EfiKey = EfiKeyC12;
                    Ps2ScanCode = 0x1c;
                    break;
            }
        } else if( Ch <= 126) {
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//            Ch = Ch - 32;
            // Update EfiKey and PS2ScanCode in KeyData
            EfiKey = CharToEfiTable[Ch-32].EfiKey;
            Ps2ScanCode = CharToEfiTable[Ch-32].Ps2ScanCode;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        }
        //Some terminal applications like teraterm reports 0x7F for DEL key(does not follows escape sequence),
        else if(Ch == 127){
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            EfiKey = CharToEfiTable[Ch-32].EfiKey;
            Ps2ScanCode = CharToEfiTable[Ch-32].Ps2ScanCode;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
            TerminalKey.Key.ScanCode=EFI_SCAN_DEL;
            TerminalKey.Key.UnicodeChar=0x00;
        }
    }

    if(EfiKey != 0) {
        TerminalKey.EfiKey = EfiKey;
        TerminalKey.PS2ScanCode = Ps2ScanCode;
        TerminalKey.EfiKeyIsValid = 1;
        TerminalKey.PS2ScanCodeIsValid = 1;
    }

    // Insert the valid Keyboard data into Keyboard Buffer
    Status = InsertKeyCode(&(TerminalDev->KeyboardBuffer),&TerminalKey);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    return EFI_SUCCESS;
}

/**
    Terminal Keyboard Data buffer cleared 

    @param  TerminalDev
    @param  ExtendedVerification

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalCommonReset (
    IN TERMINAL_DEV         *TerminalDev,
    IN BOOLEAN              ExtendedVerification
)
{

    //
    // Non Exhaustive reset: only reset private data structures.
    //
    if(!ExtendedVerification) {
        InitTerminalKeyBuffer(&(TerminalDev->KeyboardBuffer));
    }

    return EFI_SUCCESS;
}

//**********************************************************************
//  Part 3.
//  The following code is implementation of standard SimpleTextIn funcitons
//**********************************************************************

/**
    The function polls the serial port to see if a key is
    available. If a key is available and the WaitForKey event is signaled.

    @param  Event
    @param  Context

    @retval  VOID

**/

VOID
EFIAPI
WaitForKey (
    EFI_EVENT Event,
    VOID      *Context)
{
    TERMINAL_DEV *TerminalDev =
                        OUTTER(Context, SimpleTextInput, TERMINAL_DEV);

    // Check for Key in the Keyboard buffer
    if(CheckKeyinBuffer(&(TerminalDev->KeyboardBuffer))){
        //Key is ready.
        pBS->SignalEvent(Event);
        return;
    }

    // Check the Key from the Serial Port
    CheckKeyboardDataFromSerial(TerminalDev);

    if(TerminalDev->EscSeqStatePtr != NULL) {
        TerminalDev->TerminalEscIn=(VOID*)TerminalDev;
        pBS->SetTimer(TerminalDev->TimeoutEscEvent,
                        TimerPeriodic,
                        EscSequenceCheckingIntervel);
    }

    if(CheckKeyinBuffer(&(TerminalDev->KeyboardBuffer))){
        //Key is ready.
        pBS->SignalEvent(Event);
    }

    return;
}

/**
    In our implementation, this function does nothing.

    @param  This
    @param  ExtendedVerification

    @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
TerminalInputReset (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    IN BOOLEAN                          ExtendedVerification
)
{

    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInput, TERMINAL_DEV);

    return TerminalCommonReset(TerminalDev, ExtendedVerification);
}

/**
    Returns a EFI_INPUT_KEY Key if possible.  

    @param  This
    @param  Key

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputReadKey (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    OUT EFI_INPUT_KEY                   *Key
)
{
    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInput, TERMINAL_DEV);

    if(Key == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    return GetKeyData(TerminalDev, Key, sizeof(EFI_INPUT_KEY));
}

/**
    In our implementation, this function does nothing. 

    @param  This
    @param  ExtendedVerification

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputResetEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN BOOLEAN                              ExtendedVerification
)
{
    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInputEx, TERMINAL_DEV);

    return TerminalCommonReset(TerminalDev, ExtendedVerification);

}

/**
    Reads the next keystroke from the input device and
    returns data in the form of EFI_KEY_DATA structure.
    This routine is a part of SimpleTextInEx protocol 
    implementation. 
    @param  This
    @param  KeyData

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputReadKeyEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA *KeyData
)
{
    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInputEx, TERMINAL_DEV);

    if(KeyData == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    return GetKeyData(TerminalDev, KeyData, sizeof(EFI_KEY_DATA));
}

/**
    Set certain state for the input device.

    @param  This
    @param  KeyToggleState

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputSetState (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_TOGGLE_STATE *KeyToggleState
)
{
    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInputEx, TERMINAL_DEV);

    if(KeyToggleState == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    if (!(*KeyToggleState & TOGGLE_STATE_VALID) ||
        ((*KeyToggleState & (~(TOGGLE_STATE_VALID | SCROLL_LOCK_ACTIVE | NUM_LOCK_ACTIVE | CAPS_LOCK_ACTIVE)))) ) {
        return EFI_UNSUPPORTED;
    }

    TerminalDev->KeyToggleState=*KeyToggleState;

    return EFI_SUCCESS;
}

/**
    Get the keys from the Serial device
    @param  Event
    @param  Context

    @retval  VOID

**/

VOID
EFIAPI
PollingTerminalKey (
    IN EFI_EVENT Event,
    IN VOID      *Context
)
{

    TERMINAL_DEV *TerminalDev=(TERMINAL_DEV*)(*(UINTN*)Context);

    if(TerminalDev == NULL) {
        return;
    }

    CheckKeyboardDataFromSerial(TerminalDev);

    if(TerminalDev->EscSeqStatePtr != NULL) {
        TerminalDev->TerminalEscIn=(VOID*)TerminalDev;
        pBS->SetTimer(TerminalDev->TimeoutEscEvent,
                        TimerPeriodic,
                        EscSequenceCheckingIntervel);
    }

    return;
}

/**
    Check for Record is present or Not

    @param  TerminalKeyIn

    @retval  BOOLEAN

**/

BOOLEAN
ValidateKeyRecord (
    IN KEY_WAITING_RECORD    *TerminalKeyIn
)
{
    KEY_WAITING_RECORD *TempRecord = OUTTER(mTerminalKeyboardData.pHead, Link, KEY_WAITING_RECORD);

    while ( TempRecord != NULL) {
        if(TempRecord == TerminalKeyIn) {
            return TRUE;
        }
        // go to the next element in the list
        TempRecord = OUTTER(TempRecord->Link.pNext, Link, KEY_WAITING_RECORD);
    }

    return FALSE;
}

/**
    Call the notification function based on the key pressed

    @param  Key

    @retval  EFI_STATUS

**/

EFI_STATUS
TerminalCheckKeyNotify (
    IN AMI_EFI_KEY_DATA *Key
)
{

    KEY_WAITING_RECORD *TerminalKeyIn = 
                            OUTTER(mTerminalKeyboardData.pHead,
                                    Link, KEY_WAITING_RECORD);
    BOOLEAN             KeyScanCodeMatch=FALSE;
    BOOLEAN             KeyUniCodeMatch=FALSE;
    BOOLEAN             ShiftKeyMatch=FALSE;
    BOOLEAN             CtrlKeyMatch=FALSE;
    BOOLEAN             AltKeyMatch=FALSE;
    BOOLEAN             LogoKeyMatch=FALSE;
    BOOLEAN             MenuKeyMatch=FALSE;
    BOOLEAN             SysRqKeyMatch=FALSE;
    BOOLEAN             KeyShiftCodeMatch=FALSE;
    BOOLEAN             KeyToggleKeyMatch=FALSE;
    BOOLEAN             CallBackCalled=FALSE;

    // if the list is empty return the status that was passed in
    if (TerminalKeyIn == NULL) {
        return EFI_SUCCESS;
    }

    // check for a handle that was already identified
    while ( TerminalKeyIn != NULL) {
        KeyScanCodeMatch=FALSE;
        KeyUniCodeMatch=FALSE;
        ShiftKeyMatch=FALSE;
        CtrlKeyMatch=FALSE;
        AltKeyMatch=FALSE;
        LogoKeyMatch=FALSE;
        MenuKeyMatch=FALSE;
        SysRqKeyMatch=FALSE;
        KeyShiftCodeMatch=FALSE;
        KeyToggleKeyMatch=FALSE;

        //
        // Check the Scan Code if the Scan code is not 0
        //
        if (TerminalKeyIn->Context.Key.ScanCode != 0) {
            if( TerminalKeyIn->Context.Key.ScanCode == Key->Key.ScanCode) {
                KeyScanCodeMatch=TRUE;
            }
        } else {
            KeyScanCodeMatch=TRUE;
        }

        //
        // Check the Uncide Code Matching
        //

        if(TerminalKeyIn->Context.Key.UnicodeChar == Key->Key.UnicodeChar) {
            KeyUniCodeMatch=TRUE;
        }

        if(TerminalKeyIn->Context.KeyState.KeyShiftState & SHIFT_STATE_VALID){

            //
            // Check the ShiftKey Matching. Left Shift Key is matched with
            // Left or Right Shift Key. Same for Right Shift Key
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & (RIGHT_SHIFT_PRESSED | LEFT_SHIFT_PRESSED)) {
                if(Key->KeyState.KeyShiftState & (RIGHT_SHIFT_PRESSED | LEFT_SHIFT_PRESSED)) {
                    ShiftKeyMatch=TRUE;
                }
            } else {
                ShiftKeyMatch=TRUE;
            }

            //
            // Check the Ctrl Matching. Left Ctrl Key is matched with
            // Left or Right Ctrl Key. Same for Right Ctrl Key
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & (RIGHT_CONTROL_PRESSED | LEFT_CONTROL_PRESSED)) {
                if(Key->KeyState.KeyShiftState & (RIGHT_CONTROL_PRESSED | LEFT_CONTROL_PRESSED)) {
                    CtrlKeyMatch=TRUE;
                }
            } else {
                CtrlKeyMatch=TRUE;
            }

            //
            // Check the Alt Matching. Left Alt Key is matched with
            // Left or Right Alt Key. Same for Right Alt Key
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) {
                if(Key->KeyState.KeyShiftState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) {
                    AltKeyMatch=TRUE;
                }
            } else {
                AltKeyMatch=TRUE;
            }

            //
            // Check the Logo Matching. Left Logo Key is matched with
            // Left or Right Logo Key. Same for Right Logo Key
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & (RIGHT_LOGO_PRESSED | LEFT_LOGO_PRESSED)) {
                if(Key->KeyState.KeyShiftState & (RIGHT_LOGO_PRESSED | LEFT_LOGO_PRESSED)) {
                    LogoKeyMatch=TRUE;
                }
            } else {
                LogoKeyMatch=TRUE;
            }

            //
            // Check the Menu Key Matching
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & MENU_KEY_PRESSED) {
                if(Key->KeyState.KeyShiftState & MENU_KEY_PRESSED) {
                    MenuKeyMatch=TRUE;
                }
            } else {
                MenuKeyMatch=TRUE;
            }

            //
            // Check the SysRq Key Matching
            //
            if (TerminalKeyIn->Context.KeyState.KeyShiftState & SYS_REQ_PRESSED) {
                if(Key->KeyState.KeyShiftState & SYS_REQ_PRESSED) {
                    SysRqKeyMatch=TRUE;
                }
            } else {
                SysRqKeyMatch=TRUE;
            }

            KeyShiftCodeMatch=ShiftKeyMatch & CtrlKeyMatch & AltKeyMatch
                                & LogoKeyMatch & MenuKeyMatch & SysRqKeyMatch;
        }else {
            KeyShiftCodeMatch=TRUE;
        }
        //
        // Check the Key Toggle State
        //
        if(TerminalKeyIn->Context.KeyState.KeyToggleState & TOGGLE_STATE_VALID){
            if(TerminalKeyIn->Context.KeyState.KeyToggleState == Key->KeyState.KeyToggleState) {
                KeyToggleKeyMatch=TRUE;
            }
        } else {
            KeyToggleKeyMatch=TRUE;
        }

        //
        // If everything matched, call the callback function.
        //
        if(KeyScanCodeMatch && KeyUniCodeMatch && KeyShiftCodeMatch && KeyToggleKeyMatch && (!TerminalKeyIn->CallbackStatus)) {

            //Call the notification function
            TerminalKeyIn->CallbackStatus=TRUE;
            CallBackCalled=TRUE;
            TerminalKeyIn->Callback(&TerminalKeyIn->Context);

            if(ValidateKeyRecord(TerminalKeyIn)) {

                // go to the next element in the list
                TerminalKeyIn = OUTTER(TerminalKeyIn->Link.pNext, Link, KEY_WAITING_RECORD);
            } else {

                // Record deleted. So go to the first element in the list
                TerminalKeyIn = OUTTER(mTerminalKeyboardData.pHead, Link, KEY_WAITING_RECORD);
            }
            continue;
        }
        // go to the next element in the list
        TerminalKeyIn = OUTTER(TerminalKeyIn->Link.pNext, Link, KEY_WAITING_RECORD);
    }

    if(CallBackCalled) {
        TerminalKeyIn = OUTTER(mTerminalKeyboardData.pHead, Link, KEY_WAITING_RECORD);
        while ( TerminalKeyIn != NULL) {
            TerminalKeyIn->CallbackStatus=FALSE;
            // go to the next element in the list
            TerminalKeyIn = OUTTER(TerminalKeyIn->Link.pNext, Link, KEY_WAITING_RECORD);
        }
    }

    return EFI_SUCCESS;
}

/**
    Register a notification function for a particular
    keystroke for the input device.

    @param  This
    @param  KeyData
    @param  KeyNotificationFunction
    @param  NotifyHandle

    @retval EFI_STATUS

**/

EFI_STATUS
TerminalInputRegisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_DATA                      *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION           KeyNotificationFunction,
    OUT EFI_HANDLE                       *NotifyHandle
)
{
    EFI_STATUS                  Status;
    TERMINAL_DEV *TerminalDev = OUTTER(This, SimpleTextInputEx, TERMINAL_DEV);


    if(KeyData == NULL || KeyNotificationFunction == NULL || NotifyHandle == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Create database record and add to database
    //
    Status = pBS->AllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (KEY_WAITING_RECORD),
                      (VOID**)&mTerminalKeyboardRecord
                      );

    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Gather information about the registration request
    //

    mTerminalKeyboardRecord->Context   = *KeyData;
    mTerminalKeyboardRecord->Callback  = KeyNotificationFunction;
    mTerminalKeyboardRecord->CallbackStatus = FALSE;

    DListAdd (&mTerminalKeyboardData, &(mTerminalKeyboardRecord->Link));

    //
    // Child's handle will be the address linked list link in the record
    //
    *NotifyHandle = (EFI_HANDLE) (&mTerminalKeyboardRecord->Link);

    TerminalDev->TerminalKeyIn=(VOID *)TerminalDev;
    pBS->SetTimer(TerminalDev->TerminalKeyEvent,
                        TimerPeriodic,
                        KEY_POLLING_INTERVAL);

    return EFI_SUCCESS;
}
/**
    Removes the notification which was previously
    registered by TerminalInputRegisterKeyNotify.

    @param  This
    @param  NotificationHandle

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputUnRegisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_HANDLE                        NotificationHandle
)
{
    DLINK               *ListPtr;
    KEY_WAITING_RECORD  *TerminalKeyIn;
    TERMINAL_DEV        *TerminalDev = OUTTER(This, SimpleTextInputEx, TERMINAL_DEV);


    if(NotificationHandle == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    ListPtr = mTerminalKeyboardData.pHead;
    while ( ListPtr != NULL) {
        TerminalKeyIn = OUTTER(ListPtr, Link, KEY_WAITING_RECORD);
        if ( (&TerminalKeyIn->Link) == NotificationHandle) {
            DListDelete(&mTerminalKeyboardData, ListPtr);
            pBS->FreePool(TerminalKeyIn);
            break;
        }

        ListPtr = ListPtr->pNext;
    }

    if(ListPtr == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    pBS->SetTimer(TerminalDev->TerminalKeyEvent,
                        TimerCancel,
                        KEY_POLLING_INTERVAL);

    return EFI_SUCCESS;
}

/**
    Reads the next keystroke from the input device and
    returns data in the form of AMI_EFI_KEY_DATA structure.
    This routine is a part of AmiEfiKeycode protocol 
    implementation. 

    @param  This
    @param  KeyData

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalInputReadEfiKey (
    IN AMI_EFIKEYCODE_PROTOCOL *This,
    OUT AMI_EFI_KEY_DATA *KeyData
)
{
    TERMINAL_DEV *TerminalDev = OUTTER(This, EfiKeycodeInput, TERMINAL_DEV);

    if(KeyData == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    return GetKeyData(TerminalDev, KeyData, sizeof(AMI_EFI_KEY_DATA));
}

/**
    Initialize Terminal Keyboard Buffer.

    @param  KeyboardBuffer

    @retval EFI_STATUS

**/

EFI_STATUS
InitTerminalKeyBuffer (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
)
{

    pBS->SetMem ((VOID*)KeyboardBuffer, sizeof (TERMINAL_KB_BUFFER), 0);

    KeyboardBuffer->bHead = KeyboardBuffer->bTail;

    return EFI_SUCCESS;
}

/**
    Check whether Terminal Keyboard buffer is empty.

    @param  KeyboardBuffer

    @retval BOOLEAN

**/

BOOLEAN
IsTerminalKeyboardBufferEmpty (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
)
{
    //
    // meet FIFO empty condition
    //
    return (BOOLEAN)(KeyboardBuffer->bHead == KeyboardBuffer->bTail);
}

/**
    Check whether Terminal Keyboard buffer is full.

    @param  KeyboardBuffer

    @retval BOOLEAN

**/

BOOLEAN
IsTerminalKeyboardBufferFull (
    IN TERMINAL_KB_BUFFER   *KeyboardBuffer
)
{
    return (BOOLEAN)(((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1)) ==
                            KeyboardBuffer->bHead);
}

/**
    Inserts a key code into keyboard buffer.

    @param  KeyboardBuffer
    @param  Key

    @retval EFI_STATUS

**/

EFI_STATUS
InsertKeyCode (
    TERMINAL_KB_BUFFER   *KeyboardBuffer,
    AMI_EFI_KEY_DATA     *Key
)
{
    AMI_EFI_KEY_DATA         TerminalKey;
    EFI_STATUS               Status;

    //
    // if keyboard buffer is full, throw the
    // first key out of the keyboard buffer.
    //
    if (IsTerminalKeyboardBufferFull (KeyboardBuffer)) {	
        Status = RemoveKeyCode(KeyboardBuffer,&TerminalKey);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    pBS->CopyMem((VOID*)&(KeyboardBuffer->KeyData[KeyboardBuffer->bTail]),
                (VOID*)Key,
                sizeof(AMI_EFI_KEY_DATA));

    //
    // adjust the tail pointer of the FIFO keyboard buffer.
    //
    KeyboardBuffer->bTail = (UINT8)((KeyboardBuffer->bTail + 1)
                % (MAX_KEY_ALLOWED + 1)) ;

    TerminalCheckKeyNotify(Key);

    return EFI_SUCCESS;
}

/**
    Removes a key code from the keyboard buffer.

    @param  KeyboardBuffer
    @param  TerminalKey

    @retval EFI_STATUS

**/

EFI_STATUS
RemoveKeyCode (
    TERMINAL_KB_BUFFER *KeyboardBuffer,
    AMI_EFI_KEY_DATA   *TerminalKey
)
{
    if (IsTerminalKeyboardBufferEmpty (KeyboardBuffer)) {
        return EFI_NOT_READY;
    }

    pBS->CopyMem((VOID*)TerminalKey,
                (VOID*)&(KeyboardBuffer->KeyData[KeyboardBuffer->bHead]),
                sizeof(AMI_EFI_KEY_DATA));

    //
    // adjust the head pointer of the FIFO keyboard buffer.
    //
    KeyboardBuffer->bHead = (UINT8)((KeyboardBuffer->bHead + 1) %
                        (MAX_KEY_ALLOWED + 1)) ;

    return EFI_SUCCESS;
}

/**
    Checks if any key is present in the buffer

    @param  KeyboardBuffer

    @retval  BOOLEAN

**/

BOOLEAN
CheckKeyinBuffer (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
)
{
    if (KeyboardBuffer->bHead == KeyboardBuffer->bTail) {
        return FALSE;
    }

    return TRUE;
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
