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

/**
  @file  AmiSerialTextOutLib.h
  Declares functions used by SerialTextOutLib.
**/

#ifndef __SERIAL_TEXT_OUT_LIB_H__
#define __SERIAL_TEXT_OUT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_COLUMNS     80
#define MAX_ROWS        25

#define ASCII_ESC       0x1B

EFI_STATUS
TerminalClearScreen (
    VOID
);

EFI_STATUS
TerminalSetCursorPosition (
    IN  UINT8 Column,
    IN  UINT8 Row 
);

EFI_STATUS
TerminalOutputStringHelper (
    IN  UINT8    *String 
);

EFI_STATUS
TerminalSetAttribute (
    IN  UINT8    TextAttribute
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
