//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file Terminal.h
    Contains TERMINAL_TYPE and TERMINAL_DEV definitions, among others. 

**/

#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <AmiDxeLib.h>
#include <Acpi30.h>
#include <AcpiRes.h>
#include <Pci.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmiKeycode.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SerialIo.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DevicePath.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/PciIo.h>

typedef enum {
    VT100,
    VT100Plus,
    VT_UTF8,
    ANSI
} TERMINAL_TYPE;

typedef enum {
    BaudRate9600 = 3,
    BaudRate19200 = 4,
    BaudRate57600 = 6,
    BaudRate115200 = 7,
} SPCR_BAUD_RATE;

typedef enum {
    NoFlowControl=0,
    HardwareFlowControl=2,
    SoftwareFlowControl=4
} FLOW_CONTROL;

typedef struct {
    UINT8 PortNumber;
    UINT64 BaseAddress;
    UINT8 Irq;
    TERMINAL_TYPE TerminalType;
    SPCR_BAUD_RATE BaudRate;
    FLOW_CONTROL FlowControl;
    UINT16 DeviceId;
    UINT16 VendorId;
    UINTN Segment;
    UINTN Bus;
    UINTN Device;
    UINTN Function;
} ACPI_SPCR_TABLE_INFO;


typedef struct _STATE_DESCRIPTION STATE_DESCRIPTION;

struct _STATE_DESCRIPTION{
    CHAR16 InputChar;
    STATE_DESCRIPTION *NextEscSeqState;
    UINT16 EfiInputKeyScanCode;
    CHAR16 EfiInputKeyUnicodeChar;
    UINT8  KeyPad;
};

#define BUFFER_SIZE 1000
#define ASCII_DEL   127
#define ASCII_ESC   0x1B
#define KEY_POLLING_INTERVAL    500000

/*
Bit Width       UTF8 Encoding
    7           0x0nnnnnnn
    8-11        0x110nnnnn 0x10nnnnnn 
    12-16       0x1110nnnn 0x10nnnnnn 0x10nnnnnn 
*/
typedef union {
    UINT8 UTF8_1;
    UINT8 UTF8_2[2];
    UINT8 UTF8_3[3];
} UTF8_CHAR;

typedef struct {
    UINT16 Head;
    UINT16 Tail;
    CHAR8 Buffer[BUFFER_SIZE];
} RAW_FIFO_BUFFER;

typedef struct {
    UINT16 Head;
    UINT16 Tail;
    CHAR16 Buffer[BUFFER_SIZE];
} UNICODE_FIFO_BUFFER;

typedef struct _KEY_WAITING_RECORD{
  DLINK                                         Link;
  EFI_KEY_DATA                                  Context;
  EFI_KEY_NOTIFY_FUNCTION                       Callback;
  BOOLEAN                                       CallbackStatus;
} KEY_WAITING_RECORD;

#define MODE_0_MAX_COLUMNS  80
#define MODE_0_MAX_ROWS     25

#define MODE_2_MAX_COLUMNS  100
#define MODE_2_MAX_ROWS     31

#define MAX_MODE            3
#define MAX_KEY_ALLOWED     32

#define PCI_CFG_ADDR(bus,dev,func,reg) \
    ((VOID*)(UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

typedef struct {
    AMI_EFI_KEY_DATA KeyData[MAX_KEY_ALLOWED + 1];
    UINT8   bHead;
    UINT8   bTail;
} TERMINAL_KB_BUFFER;


typedef struct _TERMINAL_DEV {

    EFI_SIMPLE_TEXT_INPUT_PROTOCOL  SimpleTextInput;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  SimpleTextInputEx;
    AMI_EFIKEYCODE_PROTOCOL         EfiKeycodeInput;

    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL SimpleTextOutput;
    SIMPLE_TEXT_OUTPUT_MODE         TerminalTextOutputMode;
    
    EFI_EVENT                       TimeoutEvent;
    EFI_EVENT                       TimeoutEscEvent;
    VOID                            *TerminalEscIn;

    EFI_EVENT                       TerminalKeyEvent;
    VOID                            *TerminalKeyIn;

    TERMINAL_TYPE                   TerminalType;

    EFI_SERIAL_IO_PROTOCOL          *SerialIo;

    EFI_DEVICE_PATH_PROTOCOL        *ChildDevicePath;

    STATE_DESCRIPTION               *TerminalEscSeqState;

    RAW_FIFO_BUFFER                 RawFIFO;
    UNICODE_FIFO_BUFFER             UnicodeFIFO;

    TERMINAL_KB_BUFFER              KeyboardBuffer;
    EFI_KEY_TOGGLE_STATE            KeyToggleState;

    //Variables for processing escape sequences:
    STATE_DESCRIPTION               *EscSeqStatePtr;
    CHAR16                          InputChars[5];
    UINTN                           InputCharIndex;

    BOOLEAN                         RecorderMode;
    UINT8                           PuttyKeyPad;

} TERMINAL_DEV;

EFI_STATUS
TerminalCommonReset (
    IN TERMINAL_DEV    *TerminalDev,
    IN BOOLEAN          ExtendedVerification
);

//Simple Input: 

EFI_STATUS
EFIAPI
TerminalInputReset (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    IN BOOLEAN                          ExtendedVerification
);

EFI_STATUS
EFIAPI
TerminalInputReadKey (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    OUT EFI_INPUT_KEY                   *Key
);

VOID
EFIAPI
WaitForKey (
    EFI_EVENT  Event,
    VOID       *Context
);
VOID
EFIAPI
WaitForEsc (
    EFI_EVENT  Event,
    VOID       *Context
);
VOID
EFIAPI
PollingTerminalKey (
    EFI_EVENT  Event,
    VOID       *Context
);

EFI_STATUS
GetKeyFromSerial (
    IN TERMINAL_DEV     *TerminalDev, 
    IN AMI_EFI_KEY_DATA *TerminalKey
);

EFI_STATUS
EFIAPI
TerminalInputResetEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN BOOLEAN                              ExtendedVerification
);

EFI_STATUS
EFIAPI
TerminalInputReadKeyEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    OUT EFI_KEY_DATA                        *KeyData
);

EFI_STATUS
EFIAPI
TerminalInputSetState (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_KEY_TOGGLE_STATE                 *KeyToggleState
);

EFI_STATUS
EFIAPI
TerminalInputRegisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_KEY_DATA                         *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
    OUT EFI_HANDLE                          *NotifyHandle
);

EFI_STATUS
EFIAPI
TerminalInputUnRegisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_HANDLE                           NotificationHandle
);

// AMI KeyCode:

EFI_STATUS
EFIAPI
TerminalInputReadEfiKey (
    IN AMI_EFIKEYCODE_PROTOCOL              *This,
    OUT AMI_EFI_KEY_DATA                    *KeyData
);

VOID
RefreshScreen();

//Simple Output: 

EFI_STATUS
EFIAPI
TerminalReset (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN BOOLEAN                          ExtendedVerification
);

EFI_STATUS
EFIAPI
TerminalOutputString (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *String
);

EFI_STATUS
EFIAPI
TerminalTestString (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *String
);

EFI_STATUS
EFIAPI
TerminalQueryMode (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            ModeNumber,
    OUT UINTN                           *Columns,
    OUT UINTN                           *Rows
);

EFI_STATUS
EFIAPI
TerminalSetMode (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            ModeNumber
);

EFI_STATUS
EFIAPI
TerminalSetAttribute (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            Attribute
);

EFI_STATUS
EFIAPI
TerminalClearScreen (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This
);

EFI_STATUS
EFIAPI
TerminalSetCursorPosition (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN UINTN                            Column,
    IN UINTN                            Row
);

EFI_STATUS
EFIAPI
TerminalEnableCursor (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN BOOLEAN                          Visible
);

EFI_STATUS
TerminalOutputEscapeSequence (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                           *EscapeSequence
);

VOID
EFIAPI
ClearFiFoBuffers (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

EFI_STATUS
RemoveKeyCode (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer,
    IN AMI_EFI_KEY_DATA   *TerminalKey
);

BOOLEAN
CheckKeyinBuffer (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
);

EFI_STATUS
InsertKeyCode (
    IN TERMINAL_KB_BUFFER   *KeyboardBuffer,
    IN AMI_EFI_KEY_DATA     *Key
);

BOOLEAN
IsTerminalKeyboardBufferFull (
    IN TERMINAL_KB_BUFFER   *KeyboardBuffer
);

BOOLEAN
IsTerminalKeyboardBufferEmpty (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
);

EFI_STATUS
InitTerminalKeyBuffer (
    IN TERMINAL_KB_BUFFER *KeyboardBuffer
);

EFI_STATUS
TerminalCheckKeyNotify (
    IN AMI_EFI_KEY_DATA *Key
);

EFI_STATUS
TerminalCommonReset ( 
    IN TERMINAL_DEV         *TerminalDev, 
    IN BOOLEAN              ExtendedVerification
);

EFI_STATUS
CheckKeyboardDataFromSerial (
    IN  TERMINAL_DEV *TerminalDev
);

EFI_STATUS
GetKeyData (
    IN  TERMINAL_DEV *TerminalDev,
    OUT VOID         *pKey,
    IN  UINT8        KeySize
);

VOID
AcpiSpcrTable (
    IN UINT8 Port,
    IN EFI_DRIVER_BINDING_PROTOCOL *This, 
    IN EFI_HANDLE Controller
);

VOID
EFIAPI
CreateSpcrAcpiTableWrapper (
    IN EFI_EVENT Event, 
    IN VOID *Context
);

VOID
CreateSpcrAcpiTable (
    IN EFI_ACPI_SUPPORT_PROTOCOL *AcpiSupportProtocol,
    IN ACPI_SPCR_TABLE_INFO *SpcrInfo
);

EFI_STATUS
GetComPortNumber (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE Controller,
    IN OUT UINT8 *ComPort,
    IN OUT BOOLEAN *IsPciSerialInterface
);

VOID
GetSetupValuesForTerminal (
    IN UINT8 ComPort,
    IN OUT UINT8 *ConsoleRedirectionEnable,
    IN OUT UINT8 *TerminalType,
    IN OUT BOOLEAN *SupportExRes,
    IN OUT BOOLEAN *VtUtf8,
    IN OUT BOOLEAN *RecorderMode,
    IN OUT BOOLEAN *PuttyKeyPad,
    IN OUT UINT8 *Disable_Terminal_For_SCT_Test
);

VOID
GetSetupValuesForSerialIoMode (
    IN UINT8 ComPort,
    IN OUT SERIAL_IO_MODE *SerialIoMode
);

EFI_STATUS
SetComSuperIoSpcrTableValues (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE Controller
);

EFI_STATUS
SetPciSerialSpcrTableValues (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE Controller
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
