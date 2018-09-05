//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//
// Name:  SuperMSetup.h
//
// Description:
//
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __SMC_SERIAL_TEXT_OUT_H__
#define __SMC_SERIAL_TEXT_OUT_H__
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Token.h>

typedef struct _SERIAL_CONSOLE_CONFIGURATION_ {
   UINT16     BaseAddress;
   UINT32     BaudRate;
   UINT8      Parity;
   UINT8      StopBits;
   UINT8      DataBits;   
   UINT8      TerminalType;
   UINT8      Resolution;
   UINT8      RecorderMode;
} SERIAL_CONSOLE_CONFIGURATION;

typedef struct _SMC_SERIAL_TEXT_OUT_HOB_ {
  UINT8                         PortNumber;
  SERIAL_CONSOLE_CONFIGURATION  PortInfo[TOTAL_SIO_SERIAL_PORTS]; 		
} SMC_SERIAL_TEXT_OUT_HOB;


typedef struct {
   UINT8        DevEnable;
   UINT8        DevPrsId;
   UINT8        DevMode;
} COM_NV_DATA;   


#define SMC_SERIAL_TEXT_OUT_HOB_GUID \
 { 0xe1b87c6a, 0x700d, 0x4d95, { 0x84, 0x5c, 0x11, 0xf1, 0xd2, 0xab, 0xe1, 0xc3 } }

typedef union {
    UINT8 UTF8_1;
    UINT8 UTF8_2[2];
    UINT8 UTF8_3[3];
} UTF8_CHAR;

typedef enum {
    VT100,
    VT100Plus,
    VT_UTF8,
    ANSI
} TERMINAL_TYPE;

#define MAX_COLUMNS     80
#define MAX_ROWS        25

#define MODE_0_MAX_COLUMNS  80
#define MODE_0_MAX_ROWS     25

#define MODE_2_MAX_COLUMNS  100
#define MODE_2_MAX_ROWS     31

#define RXBUF_OFFSET      0x00
#define TXBUF_OFFSET      0x00
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

#define SERIALDB 0x3            //      Bit0-1: Number of Serial 
                                //                 Data Bits
#define STOPB 0x4               //      Bit2: Number of Stop Bits
#define PAREN 0x8               //      Bit3: Parity Enable
#define EVENPAR 0x10            //      Bit4: Even Parity Select
#define STICPAR 0x20            //      Bit5: Sticky Parity
#define BRCON 0x40              //      Bit6: Break Control
#define FCR_FIFOE    BIT0
#define FCR_FIFO64   BIT5
#define LSR_TXRDY    0x20
#define LSR_RXDA     0x01
#define DLAB         BIT7
#define MCR_RTS      BIT1
#define MSR_CTS      BIT4
#define MSR_DSR      BIT5
//
// Define's for required EFI Unicode Box Draw characters
//
#define BOXDRAW_HORIZONTAL                  0x2500
#define BOXDRAW_VERTICAL                    0x2502
#define BOXDRAW_DOWN_RIGHT                  0x250c
#define BOXDRAW_DOWN_LEFT                   0x2510
#define BOXDRAW_UP_RIGHT                    0x2514
#define BOXDRAW_UP_LEFT                     0x2518
#define BOXDRAW_VERTICAL_RIGHT              0x251c
#define BOXDRAW_VERTICAL_LEFT               0x2524
#define BOXDRAW_DOWN_HORIZONTAL             0x252c
#define BOXDRAW_UP_HORIZONTAL               0x2534
#define BOXDRAW_VERTICAL_HORIZONTAL         0x253c
#define BOXDRAW_DOUBLE_HORIZONTAL           0x2550
#define BOXDRAW_DOUBLE_VERTICAL             0x2551
#define BOXDRAW_DOWN_RIGHT_DOUBLE           0x2552
#define BOXDRAW_DOWN_DOUBLE_RIGHT           0x2553
#define BOXDRAW_DOUBLE_DOWN_RIGHT           0x2554
#define BOXDRAW_DOWN_LEFT_DOUBLE            0x2555
#define BOXDRAW_DOWN_DOUBLE_LEFT            0x2556
#define BOXDRAW_DOUBLE_DOWN_LEFT            0x2557
#define BOXDRAW_UP_RIGHT_DOUBLE             0x2558
#define BOXDRAW_UP_DOUBLE_RIGHT             0x2559
#define BOXDRAW_DOUBLE_UP_RIGHT             0x255a
#define BOXDRAW_UP_LEFT_DOUBLE              0x255b
#define BOXDRAW_UP_DOUBLE_LEFT              0x255c
#define BOXDRAW_DOUBLE_UP_LEFT              0x255d
#define BOXDRAW_VERTICAL_RIGHT_DOUBLE       0x255e
#define BOXDRAW_VERTICAL_DOUBLE_RIGHT       0x255f
#define BOXDRAW_DOUBLE_VERTICAL_RIGHT       0x2560
#define BOXDRAW_VERTICAL_LEFT_DOUBLE        0x2561
#define BOXDRAW_VERTICAL_DOUBLE_LEFT        0x2562
#define BOXDRAW_DOUBLE_VERTICAL_LEFT        0x2563
#define BOXDRAW_DOWN_HORIZONTAL_DOUBLE      0x2564
#define BOXDRAW_DOWN_DOUBLE_HORIZONTAL      0x2565
#define BOXDRAW_DOUBLE_DOWN_HORIZONTAL      0x2566
#define BOXDRAW_UP_HORIZONTAL_DOUBLE        0x2567
#define BOXDRAW_UP_DOUBLE_HORIZONTAL        0x2568
#define BOXDRAW_DOUBLE_UP_HORIZONTAL        0x2569
#define BOXDRAW_VERTICAL_HORIZONTAL_DOUBLE  0x256a
#define BOXDRAW_VERTICAL_DOUBLE_HORIZONTAL  0x256b
#define BOXDRAW_DOUBLE_VERTICAL_HORIZONTAL  0x256c

//
// EFI Required Block Elements Code Chart
//
#define BLOCKELEMENT_FULL_BLOCK   0x2588
#define BLOCKELEMENT_LIGHT_SHADE  0x2591

//
// EFI Required Geometric Shapes Code Chart
//
#define GEOMETRICSHAPE_UP_TRIANGLE    0x25b2
#define GEOMETRICSHAPE_RIGHT_TRIANGLE 0x25ba
#define GEOMETRICSHAPE_DOWN_TRIANGLE  0x25bc
#define GEOMETRICSHAPE_LEFT_TRIANGLE  0x25c4

//
// EFI Required Arrow shapes
//
#define ARROW_LEFT  0x2190
#define ARROW_UP    0x2191
#define ARROW_RIGHT 0x2192
#define ARROW_DOWN  0x2193

//
// EFI Console Colours
//
#define EFI_BLACK                 0x00
#define EFI_BLUE                  0x01
#define EFI_GREEN                 0x02
#define EFI_CYAN                  (EFI_BLUE | EFI_GREEN)
#define EFI_RED                   0x04
#define EFI_MAGENTA               (EFI_BLUE | EFI_RED)
#define EFI_BROWN                 (EFI_GREEN | EFI_RED)
#define EFI_LIGHTGRAY             (EFI_BLUE | EFI_GREEN | EFI_RED)
#define EFI_BRIGHT                0x08
#ifndef EFI_DARKGRAY
#define EFI_DARKGRAY              (EFI_BRIGHT)
#endif
#define EFI_LIGHTBLUE             (EFI_BLUE | EFI_BRIGHT)
#define EFI_LIGHTGREEN            (EFI_GREEN | EFI_BRIGHT)
#define EFI_LIGHTCYAN             (EFI_CYAN | EFI_BRIGHT)
#define EFI_LIGHTRED              (EFI_RED | EFI_BRIGHT)
#define EFI_LIGHTMAGENTA          (EFI_MAGENTA | EFI_BRIGHT)
#define EFI_YELLOW                (EFI_BROWN | EFI_BRIGHT)
#define EFI_WHITE                 (EFI_BLUE | EFI_GREEN | EFI_RED | EFI_BRIGHT)

#define EFI_TEXT_ATTR(f, b)       ((f) | ((b) << 4))

#define EFI_BACKGROUND_BLACK      0x00
#define EFI_BACKGROUND_BLUE       0x10
#define EFI_BACKGROUND_GREEN      0x20
#define EFI_BACKGROUND_CYAN       (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN)
#define EFI_BACKGROUND_RED        0x40
#define EFI_BACKGROUND_MAGENTA    (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_BROWN      (EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_LIGHTGRAY  (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)

//
// We currently define attributes from 0 - 7F for color manipulations
// To internally handle the local display characteristics for a particular character, 
// Bit 7 signifies the local glyph representation for a character.  If turned on, glyphs will be
// pulled from the wide glyph database and will display locally as a wide character (16 X 19 versus 8 X 19)
// If bit 7 is off, the narrow glyph database will be used.  This does NOT affect information that is sent to
// non-local displays, such as serial or LAN consoles.
//
#define EFI_WIDE_ATTRIBUTE  0x80

#define ASCII_DEL   127
#define ASCII_ESC   0x1B

typedef struct {
    CHAR16  Unicode;
    CHAR8   ExtendedAscii;
    CHAR8   Ascii;
} UNICODE_TO_ASCII;

EFI_STATUS
TerminalClearScreen (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration
);

EFI_STATUS
TerminalSetAttribute (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration,
    IN UINT8                           Attribute
);

EFI_STATUS
TerminalSetCursorPosition (
    IN SERIAL_CONSOLE_CONFIGURATION    *Configuration,
    IN UINTN                            Column,
    IN UINTN                            Row
);

EFI_STATUS
TerminalOutputStringHelper (
    IN SERIAL_CONSOLE_CONFIGURATION     *Configuration,
    IN UINT8                            *String
);
#endif
