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

/** @file PeiUsbLib.c
    PEI USB functions library

**/

#include <Token.h>
#include "UsbPei.h"
#include <Library/DebugLib.h>

#if USB_PEI_KEYBOARD_SUPPORT  
#include <Ppi/AmiKeyCodePpi.h>

extern EFI_PEI_AMIKEYCODE_DATA gKeys[PEI_MAX_USB_KEYS];
extern EFI_PEI_AMIKEYCODE_DATA *gKeysHead;
extern EFI_PEI_AMIKEYCODE_DATA *gKeysTail;
extern UINT8 gCapsLockOn;
extern UINT8 gNumLockOn;
extern UINT8 gScrlLockOn;

#define USB_KEYCODE_MAX_MAKE    0x64

//
// USB Key Code to Efi key mapping table
// Format:
// <efi scan code>, <unicode w/o shift>, <unicode w shift>, <PS2 scan code>
//
static
UINT8 KeyConversionTable[USB_KEYCODE_MAX_MAKE][4] = {
    EFI_SCAN_NULL,      'a',    'A',    0x1e,   // 0x04
    EFI_SCAN_NULL,      'b',    'B',    0x30,   // 0x05
    EFI_SCAN_NULL,      'c',    'C',    0x2e,   // 0x06
    EFI_SCAN_NULL,      'd',    'D',    0x20,   // 0x07
    EFI_SCAN_NULL,      'e',    'E',    0x12,   // 0x08
    EFI_SCAN_NULL,      'f',    'F',    0x21,   // 0x09
    EFI_SCAN_NULL,      'g',    'G',    0x22,   // 0x0A
    EFI_SCAN_NULL,      'h',    'H',    0x23,   // 0x0B
    EFI_SCAN_NULL,      'i',    'I',    0x17,   // 0x0C
    EFI_SCAN_NULL,      'j',    'J',    0x24,   // 0x0D
    EFI_SCAN_NULL,      'k',    'K',    0x25,   // 0x0E
    EFI_SCAN_NULL,      'l',    'L',    0x26,   // 0x0F
    EFI_SCAN_NULL,      'm',    'M',    0x32,   // 0x10
    EFI_SCAN_NULL,      'n',    'N',    0x31,   // 0x11
    EFI_SCAN_NULL,      'o',    'O',    0x18,   // 0x12
    EFI_SCAN_NULL,      'p',    'P',    0x19,   // 0x13
    EFI_SCAN_NULL,      'q',    'Q',    0x10,   // 0x14
    EFI_SCAN_NULL,      'r',    'R',    0x13,   // 0x15
    EFI_SCAN_NULL,      's',    'S',    0x1f,   // 0x16
    EFI_SCAN_NULL,      't',    'T',    0x14,   // 0x17
    EFI_SCAN_NULL,      'u',    'U',    0x16,   // 0x18
    EFI_SCAN_NULL,      'v',    'V',    0x2f,   // 0x19
    EFI_SCAN_NULL,      'w',    'W',    0x11,   // 0x1A
    EFI_SCAN_NULL,      'x',    'X',    0x2d,   // 0x1B
    EFI_SCAN_NULL,      'y',    'Y',    0x15,   // 0x1C
    EFI_SCAN_NULL,      'z',    'Z',    0x2c,   // 0x1D
    EFI_SCAN_NULL,      '1',    '!',    0x02,   // 0x1E
    EFI_SCAN_NULL,      '2',    '@',    0x03,   // 0x1F
    EFI_SCAN_NULL,      '3',    '#',    0x04,   // 0x20
    EFI_SCAN_NULL,      '4',    '$',    0x05,   // 0x21
    EFI_SCAN_NULL,      '5',    '%',    0x06,   // 0x22
    EFI_SCAN_NULL,      '6',    '^',    0x07,   // 0x23
    EFI_SCAN_NULL,      '7',    '&',    0x08,   // 0x24
    EFI_SCAN_NULL,      '8',    '*',    0x09,   // 0x25
    EFI_SCAN_NULL,      '9',    '(',    0x0a,   // 0x26
    EFI_SCAN_NULL,      '0',    ')',    0x0b,   // 0x27
    EFI_SCAN_NULL,      0x0d,   0x0d,   0x1c,   // 0x28   Enter
    EFI_SCAN_ESC,       0x00,   0x00,   0x01,   // 0x29   Esc
    EFI_SCAN_NULL,      0x08,   0x08,   0x0e,   // 0x2A   Backspace
    EFI_SCAN_NULL,      0x09,   0x09,   0x0f,   // 0x2B   Tab
    EFI_SCAN_NULL,      ' ',    ' ',    0x39,   // 0x2C   Spacebar
    EFI_SCAN_NULL,      '-',    '_',    0x0c,   // 0x2D
    EFI_SCAN_NULL,      '=',    '+',    0x0d,   // 0x2E
    EFI_SCAN_NULL,      '[',    '{',    0x1a,   // 0x2F
    EFI_SCAN_NULL,      ']',    '}',    0x1b,   // 0x30
    EFI_SCAN_NULL,      '\\',   '|',    0x2b,   // 0x31
    EFI_SCAN_NULL,      '\\',   '|',    0x2b,   // 0x32  Keyboard US \ and |
    EFI_SCAN_NULL,      ';',    ':',    0x27,   // 0x33
    EFI_SCAN_NULL,      '\'',   '"',    0x28,   // 0x34
    EFI_SCAN_NULL,      '`',    '~',    0x29,   // 0x35  Keyboard Grave Accent and Tilde
    EFI_SCAN_NULL,      ',',    '<',    0x33,   // 0x36
    EFI_SCAN_NULL,      '.',    '>',    0x34,   // 0x37
    EFI_SCAN_NULL,      '/',    '?',    0x35,   // 0x38
    EFI_SCAN_NULL,      0x00,   0x00,   0x3a,   // 0x39   CapsLock
    EFI_SCAN_F1,        0x00,   0x00,   0x3b,   // 0x3A
    EFI_SCAN_F2,        0x00,   0x00,   0x3c,   // 0x3B
    EFI_SCAN_F3,        0x00,   0x00,   0x3d,   // 0x3C
    EFI_SCAN_F4,        0x00,   0x00,   0x3e,   // 0x3D
    EFI_SCAN_F5,        0x00,   0x00,   0x3f,   // 0x3E
    EFI_SCAN_F6,        0x00,   0x00,   0x40,   // 0x3F
    EFI_SCAN_F7,        0x00,   0x00,   0x41,   // 0x40
    EFI_SCAN_F8,        0x00,   0x00,   0x42,   // 0x41
    EFI_SCAN_F9,        0x00,   0x00,   0x43,   // 0x42
    EFI_SCAN_F10,       0x00,   0x00,   0x44,   // 0x43
    EFI_SCAN_F11,       0x00,   0x00,   0x85,   // 0x44   F11
    EFI_SCAN_F12,       0x00,   0x00,   0x86,   // 0x45   F12
    EFI_SCAN_NULL,      0x00,   0x00,   0x00,   // 0x46   PrintScreen
    EFI_SCAN_NULL,      0x00,   0x00,   0x46,   // 0x47   Scroll Lock
    EFI_SCAN_NULL,      0x00,   0x00,   0x00,   // 0x48   Pause
    EFI_SCAN_INS,       0x00,   0x00,   0x52,   // 0x49
    EFI_SCAN_HOME,      0x00,   0x00,   0x47,   // 0x4A
    EFI_SCAN_PGUP,      0x00,   0x00,   0x49,   // 0x4B
    EFI_SCAN_DEL,       0x00,   0x00,   0x53,   // 0x4C
    EFI_SCAN_END,       0x00,   0x00,   0x4f,   // 0x4D
    EFI_SCAN_PGDN,      0x00,   0x00,   0x51,   // 0x4E
    EFI_SCAN_RIGHT,     0x00,   0x00,   0x4d,   // 0x4F
    EFI_SCAN_LEFT,      0x00,   0x00,   0x4b,   // 0x50
    EFI_SCAN_DN,        0x00,   0x00,   0x50,   // 0x51
    EFI_SCAN_UP,        0x00,   0x00,   0x48,   // 0x52
    EFI_SCAN_NULL,      0x00,   0x00,   0x45,   // 0x53   NumLock
    EFI_SCAN_NULL,      '/',    '/',    0x35,   //  0x54
    EFI_SCAN_NULL,      '*',    '*',    0x37,   // 0x55
    EFI_SCAN_NULL,      '-',    '-',    0x4a,   // 0x56
    EFI_SCAN_NULL,      '+',    '+',    0x4e,   // 0x57
    EFI_SCAN_NULL,      0x0d,   0x0d,   0x1c,   // 0x58
    EFI_SCAN_END,       '1',    '1',    0x4f,   // 0x59
    EFI_SCAN_DN,        '2',    '2',    0x50,   // 0x5A
    EFI_SCAN_PGDN,      '3',    '3',    0x51,   // 0x5B
    EFI_SCAN_LEFT,      '4',    '4',    0x4b,   // 0x5C
    EFI_SCAN_NULL,      '5',    '5',    0x4c,   // 0x5D
    EFI_SCAN_RIGHT,     '6',    '6',    0x4d,   // 0x5E
    EFI_SCAN_HOME,      '7',    '7',    0x47,   // 0x5F
    EFI_SCAN_UP,        '8',    '8',    0x48,   // 0x60
    EFI_SCAN_PGUP,      '9',    '9',    0x49,   // 0x61
    EFI_SCAN_INS,       '0',    '0',    0x52,   // 0x62
    EFI_SCAN_DEL,       '.',    '.',    0x53,   // 0x63
    EFI_SCAN_NULL,      '\\',   '|',    0x2b,   // 0x64 Keyboard Non-US \ and |
    EFI_SCAN_NULL,      0x00,   0x00,   0x00,   // 0x65 Keyboard Application
    EFI_SCAN_NULL,      0x00,   0x00,   0x00,   // 0x66 Keyboard Power
    EFI_SCAN_NULL,      '=' ,   '=',    0x0d    // 0x67 Keypad =
 };

UINT8 UsbToEfiKeyTable[USB_KEYCODE_MAX_MAKE] = {
    EfiKeyC1, //      'a',    'A',    0x1e,   // 0x04
    EfiKeyB5, //      'b',    'B',    0x30,   // 0x05
    EfiKeyB3, //      'c',    'C',    0x2e,   // 0x06
    EfiKeyC3, //      'd',    'D',    0x20,   // 0x07
    EfiKeyD3, //      'e',    'E',    0x12,   // 0x08
    EfiKeyC4, //      'f',    'F',    0x21,   // 0x09
    EfiKeyC5, //      'g',    'G',    0x22,   // 0x0A
    EfiKeyC6, //      'h',    'H',    0x23,   // 0x0B
    EfiKeyD8, //      'i',    'I',    0x17,   // 0x0C
    EfiKeyC7, //      'j',    'J',    0x24,   // 0x0D
    EfiKeyC8, //      'k',    'K',    0x25,   // 0x0E
    EfiKeyC9, //      'l',    'L',    0x26,   // 0x0F
    EfiKeyB7, //      'm',    'M',    0x32,   // 0x10
    EfiKeyB6, //      'n',    'N',    0x31,   // 0x11
    EfiKeyD9, //      'o',    'O',    0x18,   // 0x12
    EfiKeyD10,//       'p',    'P',    0x19,   // 0x13
    EfiKeyD1, //      'q',    'Q',    0x10,   // 0x14
    EfiKeyD4, //      'r',    'R',    0x13,   // 0x15
    EfiKeyC2, //      's',    'S',    0x1f,   // 0x16
    EfiKeyD5, //      't',    'T',    0x14,   // 0x17
    EfiKeyD7, //      'u',    'U',    0x16,   // 0x18
    EfiKeyB4, //      'v',    'V',    0x2f,   // 0x19
    EfiKeyD2, //      'w',    'W',    0x11,   // 0x1A
    EfiKeyB2, //      'x',    'X',    0x2d,   // 0x1B
    EfiKeyD6, //      'y',    'Y',    0x15,   // 0x1C
    EfiKeyB1, //      'z',    'Z',    0x2c,   // 0x1D
    EfiKeyE1, //      '1',    '!',    0x02,   // 0x1E
    EfiKeyE2, //      '2',    '@',    0x03,   // 0x1F
    EfiKeyE3, //      '3',    '#',    0x04,   // 0x20
    EfiKeyE4, //      '4',    '$',    0x05,   // 0x21
    EfiKeyE5, //      '5',    '%',    0x06,   // 0x22
    EfiKeyE6, //      '6',    '^',    0x07,   // 0x23
    EfiKeyE7, //      '7',    '&',    0x08,   // 0x24
    EfiKeyE8, //      '8',    '*',    0x09,   // 0x25
    EfiKeyE9, //      '9',    '(',    0x0a,   // 0x26
    EfiKeyE10,  //       '0',    ')',    0x0b,   // 0x27
    EfiKeyEnter,//       0x0d,   0x0d,   0x1c,   // 0x28   Enter
    EfiKeyEsc,  //      0x00,   0x00,   0x01,   // 0x29   Esc
    EfiKeyBackSpace,    //       0x08,   0x08,   0x0e,   // 0x2A   Backspace
    EfiKeyTab,      //   0x09,   0x09,   0x0f,   // 0x2B   Tab
    EfiKeySpaceBar, //   ' ',    ' ',    0x39,   // 0x2C   Spacebar
    EfiKeyE11,      //   '-',    '_',    0x0c,   // 0x2D
    EfiKeyE12,      //   '=',    '+',    0x0d,   // 0x2E
    EfiKeyD11,      //   '[',    '{',    0x1a,   // 0x2F
    EfiKeyD12,      //   ']',    '}',    0x1b,   // 0x30
    EfiKeyD13,      //   '\\',   '|',    0x2b,   // 0x31
    EfiKeyC12,      //   '\\',   '|',    0x2b,   // 0x32  Keyboard Non-US # and ~
    EfiKeyC10,      //   ';',    ':',    0x27,   // 0x33
    EfiKeyC11,      //   '\'',   '"',    0x28,   // 0x34
    EfiKeyE0,       //   '`',    '~',    0x29,   // 0x35  Keyboard Grave Accent and Tlide
    EfiKeyB8,       //   ',',    '<',    0x33,   // 0x36
    EfiKeyB9,       //   '.',    '>',    0x34,   // 0x37
    EfiKeyB10,      //   '/',    '?',    0x35,   // 0x38
    EfiKeyCapsLock, //   0x00,   0x00,   0x00,   // 0x39   CapsLock
    EfiKeyF1,       //   0x00,   0x00,   0x3b,   // 0x3A
    EfiKeyF2,       //   0x00,   0x00,   0x3c,   // 0x3B
    EfiKeyF3,       //   0x00,   0x00,   0x3d,   // 0x3C
    EfiKeyF4,       //   0x00,   0x00,   0x3e,   // 0x3D
    EfiKeyF5,       //   0x00,   0x00,   0x3f,   // 0x3E
    EfiKeyF6,       //   0x00,   0x00,   0x40,   // 0x3F
    EfiKeyF7,       //   0x00,   0x00,   0x41,   // 0x40
    EfiKeyF8,       //   0x00,   0x00,   0x42,   // 0x41
    EfiKeyF9,       //   0x00,   0x00,   0x43,   // 0x42
    EfiKeyF10,      //   0x00,   0x00,   0x44,   // 0x43
    EfiKeyF11,      //   0x00,   0x00,   0x85,   // 0x44   F11
    EfiKeyF12,      //   0x00,   0x00,   0x86,   // 0x45   F12
    EfiKeyPrint,    //   0x00,   0x00,   0x00,   // 0x46   PrintScreen
    EfiKeySLck,     //   0x00,   0x00,   0x00,   // 0x47   Scroll Lock
    EfiKeyPause,    //   0x00,   0x00,   0x00,   // 0x48   Pause
    EfiKeyIns,      //   0x00,   0x00,   0x52,   // 0x49
    EfiKeyHome,     //   0x00,   0x00,   0x47,   // 0x4A
    EfiKeyPgUp,     //   0x00,   0x00,   0x49,   // 0x4B
    EfiKeyDel,      //   0x00,   0x00,   0x53,   // 0x4C
    EfiKeyEnd,      //   0x00,   0x00,   0x4f,   // 0x4D
    EfiKeyPgDn,     //   0x00,   0x00,   0x51,   // 0x4E
    EfiKeyRightArrow, // 0x00,   0x00,   0x4d,   // 0x4F
    EfiKeyLeftArrow,  // 0x00,   0x00,   0x4b,   // 0x50
    EfiKeyDownArrow,  // 0x00,   0x00,   0x50,   // 0x51
    EfiKeyUpArrow,    // 0x00,   0x00,   0x48,   // 0x52
    EfiKeyNLck,       // 0x00,   0x00,   0x00,   // 0x53   NumLock
    EfiKeySlash,      // '/',    '/',    0x35,   // 0x54
    EfiKeyAsterisk,   // '*',    '*',    0x37,   // 0x55
    EfiKeyMinus,      // '-',    '-',    0x4a,   // 0x56
    EfiKeyPlus,       // '+',    '+',    0x4e,   // 0x57
    EfiKeyEnter,      // 0x0d,   0x0d,   0x1c,   // 0x58
    EfiKeyOne,        // '1',    '1',    0x02,   // 0x59
    EfiKeyTwo,        // '2',    '2',    0x03,   // 0x5A
    EfiKeyThree,      // '3',    '3',    0x04,   // 0x5B
    EfiKeyFour,       // '4',    '4',    0x05,   // 0x5C
    EfiKeyFive,       // '5',    '5',    0x06,   // 0x5D
    EfiKeySix,        // '6',    '6',    0x07,   // 0x5E
    EfiKeySeven,      // '7',    '7',    0x08,   // 0x5F
    EfiKeyEight,      // '8',    '8',    0x09,   // 0x60
    EfiKeyNine,       // '9',    '9',    0x0a,   // 0x61
    EfiKeyZero,       // '0',    '0',    0x0b,   // 0x62
    EfiKeyPeriod,     // '.',    '.',    0x53,   // 0x63
    EfiKeyD13,        // '\\',   '|',    0x2b,   // 0x64 Keyboard Non-US \ and |
    0,               // 0x00,    0x00,   0x00,   // 0x65 Keyboard Application
    0,               // 0x00,    0x00,   0x00,   // 0x66 Keyboard Power
    0                // '=' ,    '=',    0x0d    // 0x67 Keypad =
};

#endif


/**
    This library function copies bytes specified by Length from the memory
    location specified by Source to the memory location specified by 
    Destination.

        
    @param Destination 
        --  Target memory location of copy
    @param Source 
        --  Source memory location
    @param Length 
        --  Number of bytes to copy

    @retval 
        VOID (No Return Value)

**/

VOID PeiCopyMem (
    OUT VOID *Destination,
    IN VOID  *Source,
    IN UINTN Length )
{
    CHAR8 *Destination8;
    CHAR8 *Source8;

    Destination8 = Destination;
    Source8 = Source;
    while (Length--) {
        *(Destination8++) = *(Source8++);
    }
}


/**
    This function uses a device's PEI_USB_IO_PPI interface to execute a 
    control transfer on the default control pipe to obtain a device 
    descriptor.

         
    @param PeiServices 
        --  PEI Sevices table pointer
    @param UsbIoPpi 
        --  PEI_USB_IO_PPI interface pointer for the device
        that is being accessed
    @param Value 
        --  The upper byte of Value specifies the type of 
        descriptor and the lower byte of Value specifies
        the index (for configuration and string descriptors)
    @param Index 
        --  Specifies the Language ID for string descriptors
        or zero for all other descriptors
    @param DescriptorLength 
        --  Specifies the length of the descriptor to be
        retrieved
    @param Descriptor 
        --  Allocated buffer in which the descriptor will be 
        returned

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

    @note  
      The lower byte of Value is typically zero and the upper byte of 
      Value specifies the type of descriptor:

                  READ_BITS(Value,15,8)   Descriptor
                  -------------------------------------------------
                          1               DEVICE
                          2               CONFIGURATION
                          3               STRING
                          4               INTERFACE
                          5               ENDPOINT
                          6               DEVICE_QUALIFIER
                          7               OTHER_SPEED_CONFIGURATION
                          8               INTERFACE_POWER

      For additional reference, read the USB Device Framework chapter in 
      the USB Specification (Revision 2.0)

**/

EFI_STATUS PeiUsbGetDescriptor (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16           Value,
    IN UINT16           Index,
    IN UINT16           DescriptorLength,
    IN VOID             *Descriptor )
{
    EFI_USB_DEVICE_REQUEST      DevReq;
    UINT32                      Timeout;
    UINT32                      UsbStatus;

    if (UsbIoPpi == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    DevReq.RequestType = USB_DEV_GET_DESCRIPTOR_REQ_TYPE;
    DevReq.Request = USB_DEV_GET_DESCRIPTOR;
    DevReq.Value = Value;
    DevReq.Index = Index;
    DevReq.Length = DescriptorLength;

    Timeout = 3000;

    return UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
		EfiUsbDataIn,
        Timeout,
        Descriptor,
        DescriptorLength,
        &UsbStatus
    );

}


/**
    This function uses a device's PEI_USB_IO_PPI interface to execute a 
    control transfer on the default control pipe to set the device's 
    address on the USB bus for all future accesses.

        
    @param PeiServices 
        --  PEI Sevices table pointer
    @param UsbIoPpi 
        --  PEI_USB_IO_PPI interface pointer for the device
        that is being accessed
    @param AddressValue 
        --  The device address to be set

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS PeiUsbSetDeviceAddress (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16           AddressValue )
{
    EFI_USB_DEVICE_REQUEST      DevReq;
    UINT32                      Timeout;
    UINT32                      UsbStatus;

    if (UsbIoPpi == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    DevReq.RequestType = USB_DEV_SET_ADDRESS_REQ_TYPE;
    DevReq.Request = USB_DEV_SET_ADDRESS;
    DevReq.Value = AddressValue;
    DevReq.Index = 0;
    DevReq.Length = 0;

    Timeout = 3000;

    return UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
        EfiUsbNoData,
        Timeout,
        NULL,
        0,
        &UsbStatus
    );

}


/**
    This function uses a device's PEI_USB_IO_PPI interface to execute a 
    control transfer on the default control pipe to clear or disable a
    specific feature.

         
    @param PeiServices 
        --  PEI Sevices table pointer
    @param UsbIoPpi 
        --  PEI_USB_IO_PPI interface pointer for the device
        that is being accessed
    @param Recipient 
        --  The recipient of the request can be a device,
        an interface or an endpoint respectively specified 
        by EfiUsbDevice, EfiUsbInterface or EfiUsbEndpoint.
    @param Value 
        --  The feature selector to be cleared or disabled
    @param Target 
        --  This value specifies an index for a specific 
        interface/endpoint or zero for device recipients.

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS PeiUsbClearDeviceFeature (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN PEI_USB_IO_PPI    *UsbIoPpi,
    IN EFI_USB_RECIPIENT Recipient,
    IN UINT16            Value,
    IN UINT16            Target )
{
    EFI_USB_DEVICE_REQUEST      DevReq;
    UINT32                      Timeout;
    UINT32                      UsbStatus;

    if (UsbIoPpi == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    switch (Recipient)
    {
    case EfiUsbDevice:
        DevReq.RequestType = 0x00;
        break;

    case EfiUsbInterface:
        DevReq.RequestType = 0x01;
        break;

    case EfiUsbEndpoint:
        DevReq.RequestType = 0x02;
        break;
    }

    DevReq.Request = USB_DEV_CLEAR_FEATURE;
    DevReq.Value = Value;
    DevReq.Index = Target;
    DevReq.Length = 0;

    Timeout = 3000;

    return UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
        EfiUsbNoData,
        Timeout,
        NULL,
        0,
        &UsbStatus
    );

}


/**
    This function uses a device's PEI_USB_IO_PPI interface to execute a 
    control transfer on the default control pipe to set the device's 
    default configuration index of 1.

        
    @param PeiServices 
        --  PEI Sevices table pointer
    @param UsbIoPpi 
        --  PEI_USB_IO_PPI interface pointer for the device
        that is being accessed

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS PeiUsbSetConfiguration (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi )
{
    EFI_USB_DEVICE_REQUEST      DevReq;
    UINT32                      Timeout;
    UINT32                      UsbStatus;

    //ZeroMem( &DevReq, sizeof(EFI_USB_DEVICE_REQUEST) );
    (*PeiServices)->SetMem( &DevReq, sizeof(EFI_USB_DEVICE_REQUEST), 0 );

    DevReq.RequestType = USB_DEV_SET_CONFIGURATION_REQ_TYPE;
    DevReq.Request = USB_DEV_SET_CONFIGURATION;
    DevReq.Value = 1;  // default
    DevReq.Index = 0;
    DevReq.Length = 0;

    Timeout = 3000;

    return UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
        EfiUsbNoData,
        Timeout,
        NULL,
        0,
        &UsbStatus
    );
}


/**
    This function uses a device's PEI_USB_IO_PPI interface to execute a 
    control transfer on the default control pipe to clear a bulk Endpoint
    halt condition (and resetting the Halt status bit) for a specified 
    Endpoint.

        
    @param PeiServices 
        --  PEI Sevices table pointer
    @param UsbIoPpi 
        --  PEI_USB_IO_PPI interface pointer for the device
        that is being accessed
    @param EndpointAddress 
        --  The endpoint for which the Halt condition is to be 
        cleared

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS PeiUsbClearEndpointHalt (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            EndpointAddress )
{
    EFI_STATUS     Status;
    PEI_USB_DEVICE *PeiUsbDev;
    EFI_USB_ENDPOINT_DESCRIPTOR *EndpointDescriptor;
    UINT8 EndpointIndex = 0;

    PeiUsbDev = PEI_USB_DEVICE_FROM_THIS( UsbIoPpi );

    while (EndpointIndex < MAX_ENDPOINT) {
        Status = UsbIoPpi->UsbGetEndpointDescriptor( PeiServices, UsbIoPpi,
            EndpointIndex, &EndpointDescriptor );
        if ( EFI_ERROR( Status ) ) {
            return EFI_INVALID_PARAMETER;
        }

        if (EndpointDescriptor->EndpointAddress == EndpointAddress) {
            break;
        }

        EndpointIndex++;
    }

    if (EndpointIndex == MAX_ENDPOINT) {
        return EFI_INVALID_PARAMETER;
    }

    Status = PeiUsbClearDeviceFeature(
        PeiServices,
        UsbIoPpi,
        EfiUsbEndpoint,
        EfiUsbEndpointHalt,
        EndpointAddress
             );

    //
    // set data toggle to zero.
    //
    if ( ( PeiUsbDev->DataToggle & (1 << EndpointIndex) ) != 0 ) {
        PeiUsbDev->DataToggle =
            (UINT8) ( PeiUsbDev->DataToggle ^ (1 << EndpointIndex) );
    }

    return Status;
}


EFI_STATUS
PeiUsbIoControlTransfer(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16           Request,
    IN UINT16           Index,
    IN UINT16           Value,
    IN OUT UINT8        *Buffer,
    IN UINT16           Length,
    OUT UINT32          *UsbStatus
)
{
    EFI_USB_DEVICE_REQUEST DevReq;
    EFI_USB_DATA_DIRECTION Direction;

    (*PeiServices)->SetMem( &DevReq, sizeof(EFI_USB_DEVICE_REQUEST), 0 );

    *(UINT16*)&DevReq.RequestType = Request;
    DevReq.Value = Value;
    DevReq.Index = Index;
    DevReq.Length = Length;

    if (Length != 0)
    {
        Direction = (Request & USB_REQ_TYPE_INPUT)? EfiUsbDataIn : EfiUsbDataOut;
    }
    else
    {
        Direction = EfiUsbNoData;
    }

    return UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
        Direction,
        3000,   //Timeout
        Buffer,
        (UINTN)Length,
        UsbStatus
    );
}


/**
    This function returns true if the Hub Class device's Current Connect 
    Status bit is set in the port status value specified by PortStatus.

        
    @param PortStatus 
        --  This value is the USB Specification (Revision 2.0) 
        Hub Port Status Field value as returned by the Get 
        Port Status Hub Class device standard request.

    @retval 
        BOOLEAN (Return Value)
        = TRUE if a device is present or FALSE if a device is not
        present

**/

BOOLEAN IsPortConnect (
    IN UINT16 PortStatus )
{
    //
    // return the bit 0 value of PortStatus
    //
    if ( (PortStatus & USB_PORT_STAT_CONNECTION) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**

    @param 

    @retval 

**/

BOOLEAN IsPortEnable (
    IN UINT16 PortStatus )
{
    //
    // return the bit 0 value of PortStatus
    //
    if ( (PortStatus & USB_PORT_STAT_ENABLE) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
    This function returns true if the Hub Class device's Low-Speed 
    Device Attached bit is set in the port status value specified by 
    PortStatus.

        
    @param PortStatus 
        --  This value is the USB Specification (Revision 2.0) 
        Hub Port Status Field value as returned by the Get 
        Port Status Hub Class device standard request.

    @retval 
        BOOLEAN (Return Value)
        = TRUE if a low-speed device is present or FALSE otherwise

**/

BOOLEAN IsPortLowSpeedDeviceAttached (
    UINT16 PortStatus )
{
    //
    // return the bit 9 value of PortStatus
    //
    if ( (PortStatus & USB_PORT_STAT_LOW_SPEED) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/**
    This function returns true if the Hub Class device's High-Speed 
    Device Attached bit is set in the port status value specified by 
    PortStatus.

        
    @param PortStatus 
        --  This value is the USB Specification (Revision 2.0) 
        Hub Port Status Field value as returned by the Get 
        Port Status Hub Class device standard request.

    @retval 
        BOOLEAN (Return Value)
        = TRUE if a high-speed device is present or FALSE otherwise

**/

BOOLEAN IsPortHighSpeedDeviceAttached (
    UINT16 PortStatus )
{
    //
    // return the bit 10 value of PortStatus
    //
    if ( (PortStatus & USB_PORT_STAT_HIGH_SPEED) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/**
    This function returns true if connect status indicates the SuperSpeed
    device.

    @retval 
        BOOLEAN (Return Value)
        = TRUE if a super-speed device is present or FALSE otherwise

**/

BOOLEAN IsPortSuperSpeedDeviceAttached (
    UINT16 PortStatus )
{
    //
    // return the bit 10 value of PortStatus
    //
    if ( (PortStatus & USB_PORT_STAT_SUPER_SPEED) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
    This function returns true if connect status indicates the SuperSpeedPlus
    device.

    @retval BOOLEAN (Return Value)= TRUE 
    if a super-speed-plus device is present or FALSE otherwise

**/

BOOLEAN
IsPortSuperSpeedPlusDeviceAttached (
    UINT16  PortStatus
)
{
    //
    // return the bit 10 value of PortStatus
    //
    if ((PortStatus & USB_PORT_STAT_SUPER_SPEED_PLUS) != 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/**
    This function returns true if the Hub Class device's Connect Status 
    Change bit is set in the port change status value specified 
    by PortChangeStatus.

        
    @param PortStatus 
        --  This value is the USB Specification (Revision 2.0) 
        Hub Port Change Field value as returned by the Get 
        Port Status Hub Class device standard request.

    @retval 
        BOOLEAN (Return Value)
        = TRUE if Current Connect status has changed or FALSE
        if no change has occurred to Current Connect status

**/

BOOLEAN IsPortConnectChange (
    UINT16 PortChangeStatus )
{
    //
    // return the bit 0 value of PortChangeStatus
    //
    if ( (PortChangeStatus & USB_PORT_STAT_C_CONNECTION) != 0 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

#if USB_PEI_KEYBOARD_SUPPORT  

/**
    This function updates USB keyboard LEDs according to the values of
    global variables: gNumLockOn, gCapsLockOn, and gScrlLockOn.

    @param 
        EFI_PEI_SERVICES  **PeiServices
        PEI_USB_IO_PPI    *UsbIoPpi

**/

EFI_STATUS PeiUsbLibLightenKbLeds(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi
)
{
    UINT8   ModifierReportByte = 0;
    UINT32  UsbStatus;

    if (gNumLockOn) ModifierReportByte |= 1;
    if (gCapsLockOn) ModifierReportByte |= 2;
    if (gScrlLockOn) ModifierReportByte |= 4;

    return PeiUsbIoControlTransfer(PeiServices, UsbIoPpi,
        (UINT16)HID_RQ_SET_REPORT, 0, 0x200, &ModifierReportByte, 1, &UsbStatus);
}


/**
    This function takes the USB keyboard 8-Bytes buffer and translates it
    into a data represented by EFI_PEI_AMIKEYCODE_PPI. Then it places
    the data into a global keyboard data buffer and updates its head and
    the tail.

    @param 
        EFI_PEI_SERVICES  **PeiServices
        PEI_USB_IO_PPI    *UsbIoPpi

        UINT8   *UsbKeys
        --  USB keyboard data buffer pointer

**/

VOID PeiUsbLibProcessKeypress(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8   *UsbKeys
)
{
    UINT8 i;
    EFI_PEI_AMIKEYCODE_DATA *Head = gKeysHead;
    EFI_PEI_AMIKEYCODE_DATA KeyData;
    UINT8 Index;
    UINT8 Modifier;
    BOOLEAN ModifierToggled;

    static UINT8 ModifierBits[8] = {
        LEFT_CONTROL_PRESSED, LEFT_SHIFT_PRESSED, LEFT_ALT_PRESSED, LEFT_LOGO_PRESSED,
        RIGHT_CONTROL_PRESSED, RIGHT_SHIFT_PRESSED, RIGHT_ALT_PRESSED, RIGHT_LOGO_PRESSED
    };
    
    // First check if there is any data in the buffer
    for (i = 2; (i < 8) && (UsbKeys[i] == 0); i++) {}
    if (i == 8) {
        return;
    }

    (**PeiServices).SetMem((VOID*)&KeyData, sizeof(EFI_PEI_AMIKEYCODE_DATA), 0);

    ModifierToggled = TRUE;
    switch (UsbKeys[2])
    {
        case 0x39:  gCapsLockOn ^= 1; break;
        case 0x53:  gNumLockOn ^= 1; break;
        case 0x47:  gScrlLockOn ^= 1; break;
        default:    ModifierToggled = FALSE;
    }
    if (ModifierToggled) {
        DEBUG((DEBUG_INFO, "NumLk: %d CapsLk: %d ScrLk: %d\n", gNumLockOn, gCapsLockOn, gScrlLockOn));
        PeiUsbLibLightenKbLeds(PeiServices, UsbIoPpi);
    }

    // Prepare KeyData.KeyState.KeyShiftState
    for (i = 0, Modifier = UsbKeys[0]; i < 8; i++) {
        if ((1 << i) & Modifier) {
            KeyData.KeyState.KeyShiftState |= ModifierBits[i];
        }
    }

    // Process up to six USB keys pressed at the same time
    for (i = 2; i < 8; i++) {
        if (UsbKeys[i] == 0) continue;

        // Before processing check if there is any room in the buffer
        if (((Head + 1) == gKeysTail) ||
            ((Head + 1) == &gKeys[PEI_MAX_USB_KEYS]) && (gKeysTail == gKeys)) {
            return; // Buffer is full
        }

        Index = UsbKeys[i] - 4;
        KeyData.Key.ScanCode = KeyConversionTable[Index][0];
        KeyData.Key.UnicodeChar = KeyConversionTable[Index][1];
        if (KeyData.KeyState.KeyShiftState & (RIGHT_SHIFT_PRESSED | LEFT_SHIFT_PRESSED)) {
            KeyData.Key.UnicodeChar = KeyConversionTable[Index][2];
        }

        // Keypad '5' with NumLock off
        if (gNumLockOn == 0
            && (KeyData.KeyState.KeyShiftState & (RIGHT_SHIFT_PRESSED | LEFT_SHIFT_PRESSED)) == 0
            && Index == (0x5d - 4)) {
            KeyData.Key.UnicodeChar = 0;
        }

        // Process CapsLock
        if (gCapsLockOn == 1) {
            if (KeyData.Key.UnicodeChar >= 'a' && KeyData.Key.UnicodeChar <= 'z') {
                KeyData.Key.UnicodeChar = KeyConversionTable[Index][2];
            } else if (KeyData.Key.UnicodeChar >= 'A' && KeyData.Key.UnicodeChar <= 'Z') {
                    KeyData.Key.UnicodeChar = KeyConversionTable[Index][1];
                }
        }

        // Process Keypad
        if (UsbKeys[i] >= 0x59 && UsbKeys[i] <=0x63 && KeyData.Key.ScanCode != EFI_SCAN_NULL) {
            if (gNumLockOn == 1
                && (KeyData.KeyState.KeyShiftState & (RIGHT_SHIFT_PRESSED | LEFT_SHIFT_PRESSED)) == 0) {
                KeyData.Key.ScanCode = EFI_SCAN_NULL;
            } else {
                KeyData.Key.UnicodeChar = 0x00;
            }
        }

        KeyData.EfiKey = UsbToEfiKeyTable[Index];
        KeyData.PS2ScanCode = KeyConversionTable[Index][3];

        // Adjust the head and place the key information
        *Head = KeyData;
        gKeysHead = ((Head+1) == &gKeys[PEI_MAX_USB_KEYS])? gKeys : Head+1;
    }

    DEBUG((DEBUG_INFO, "buffer: "));
    for (i = 0; i < 8; i++) {
        DEBUG((DEBUG_INFO, "%x ", UsbKeys[i]));
    }
    DEBUG((DEBUG_INFO, "\n"));
}

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
