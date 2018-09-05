//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SerialIo.h
    Contains Serial Io related definitions. 

**/

#ifndef __SERIAL_IO_H__
#define __SERIAL_IO_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <Efi.h>
#include <AmiDxeLib.h>
#include <Protocol/SerialIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>

#define UART_DEFAULT_TYPE   UART16550A

//
// 115200 baud with rounding errors
//
#define UART_PORT_MAX_BAUD_RATE         115400
#define UART_MIN_BAUD_RATE              50

#ifdef EFI_DEBUG
#define UART_DEFAULT_RECEIVE_FIFO_DEPTH 16
#else
#define UART_DEFAULT_RECEIVE_FIFO_DEPTH 1
#endif

#define UART_MAX_RECEIVE_FIFO_DEPTH     16
#define UART_MIN_TIMEOUT                1           // 1 uS
#define UART_MAX_TIMEOUT                100000000   // 100 seconds

#define UART_DEFAULT_TIMEOUT            1000000
#define TIMEOUT_INTERVAL                10

//
// UART Register and bit definitions
//
#define SERIAL_REGISTER_THR 0       // WO   Transmit Holding Register
#define SERIAL_REGISTER_RBR 0       // RO   Receive Buffer Register
#define SERIAL_REGISTER_DLL 0       // R/W  Divisor Latch LSB
#define SERIAL_REGISTER_DLM 1       // R/W  Divisor Latch MSB
#define SERIAL_REGISTER_IER 1       // R/W  Interrupt Enable Register
    #define RAVIE 0x1               //      Bit0: Receiver Data Available
                                    //            Interrupt Enable
    #define THEIE 0x2               //      Bit1: Transmitter Holding
                                    //      Register Empty Interrupt Enable
    #define RIE 0x4                 //      Bit2: Receiver Interrupt Enable
    #define MIE 0x8                 //      Bit3: Modem Interrupt Enable
#define SERIAL_REGISTER_IIR 2       // RO   Interrupt Identification Register
    #define IPS 0x1                 //      Bit0: Interrupt Pending Status
    #define IIB 0x0e                //      Bit1-3: Interrupt ID Bits
    #define FIFO_ENABLE_STATUS_64B 0x20 //  Bit5: 64 Byte FIFO Enable Status
    #define FIFOES 0xc0             //      Bit6-7: FIFO Mode Enable Status
    #define FIFO_ENABLED 0xc0       //      Bit6 and 7: FIFO enabled, usuable
    #define FIFO_ENABLED_UNUSABLE 0x80 //   Bit7: FIFO enabled but unusable
    #define FIFO_DISABLED 0x0       //      FIFO not enabled
#define SERIAL_REGISTER_FCR 2       // WO   FIFO Cotrol Register
    #define TRFIFOE 0x1             //      Bit0: Transmit and Receive 
                                    //            FIFO Enable
    #define RESETRF 0x2             //      Bit1: Reset Reciever FIFO
    #define RESETTF 0x4             //      Bit2: Reset Transmistter FIFO
    #define DMS 0x8                 //      Bit3: DMA Mode Select
    #define FIFO_ENABLE_64B 0x20    //      Bit5: 64 Byte FIFO Enable   
    #define RTB 0xc0                //      Bit6-7: Receive Trigger Bits
#define SERIAL_REGISTER_LCR 3       // R/W  Line Control Register
    #define SERIALDB 0x3            //      Bit0-1: Number of Serial 
                                    //                 Data Bits
    #define STOPB 0x4               //      Bit2: Number of Stop Bits
    #define PAREN 0x8               //      Bit3: Parity Enable
    #define EVENPAR 0x10            //      Bit4: Even Parity Select
    #define STICPAR 0x20            //      Bit5: Sticky Parity
    #define BRCON 0x40              //      Bit6: Break Control
    #define DLAB 0x80               //      Bit7: Divisor Latch Access Bit
#define SERIAL_REGISTER_MCR 4       // R/W  Modem Control Register
    #define DTRC 0x1                //      Bit0: Data Terminal Ready Control
    #define RTS  0x2                //      Bit1: Request To Send Control
    #define OUT1 0x4                //      Bit2: Output1
    #define OUT2 0x8                //      Bit3: Output2, used to disable 
                                    //            interrupts
    #define LME 0x10                //      Bit4: Loopback Mode Enable
#define SERIAL_REGISTER_LSR 5       // R/W  Line Status Register
    #define DR 0x1                  //      Bit0: Receiver Data Ready Status
    #define OE 0x2                  //      Bit1: Overrun Error Status
    #define PE 0x4                  //      Bit2: Parity Error Status
    #define FE 0x8                  //      Bit3: Framing Error Status
    #define BI 0x10                 //      Bit4: Break Interrupt Status
    #define THRE 0x20               //      Bit5: Transmistter Holding 
                                    //            Register Status
    #define TEMT 0x40               //      Bit6: Transmitter Empty Status
    #define FIFOE 0x80              //      Bit7: FIFO Error Status
#define SERIAL_REGISTER_MSR 6       // R/W  Modem Status Register
    #define DeltaCTS 0x1            //      Bit0: Delta Clear To Send Status
    #define DeltaDSR 0x2            //      Bit1: Delta Data Set Ready Status
    #define TrailingEdgeRI 0x4      //      Bit2: Trailing Edge of Ring 
                                    //            Indicator Status
    #define DeltaDCD 0x8            //      Bit3: Delta Data Carrier 
                                    //            Detect Status
    #define CTS 0x10                //      Bit4: Clear To Send Status
    #define DSR 0x20                //      Bit5: Data Set Ready Status
    #define RI 0x40                 //      Bit6: Ring Indicator Status
    #define DCD 0x80                //      Bit7: Data Carrier Detect Status
#define SERIAL_REGISTER_SCR 7       // R/W  Scratch Pad Register

#define XON 17
#define XOFF 19
//
// Macro definitions for Serial I/O driver signature
//
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define EFI_SERIAL_DRIVER_SIGNATURE    EFI_SIGNATURE_32 ('$', 'S', 'I', 'O')


//
// UART types
//
typedef enum {
    UART8250   = 0,
    UART16450  = 1,
    UART16550  = 2,
    UART16550A = 3
} UART_TYPE;


typedef struct _SERIAL_PARAMETERS{
    EFI_SERIAL_IO_PROTOCOL      SerialIo;
    UINT32                      Signature;
#if EFI_SIO_PROTOCOL_SUPPORT 
    EFI_SIO_PROTOCOL            *EfiSio;
#else
    AMI_SIO_PROTOCOL            *AmiSio;
#endif
    EFI_HANDLE                  Handle;
    SERIAL_IO_MODE              SerialMode;
    EFI_DEVICE_PATH_PROTOCOL    *DevPathProtocol;
    EFI_DEVICE_PATH_PROTOCOL    *ParentDevicePath;
    UART_DEVICE_PATH            UartDevicePath;
    UART_FLOW_CONTROL_DEVICE_PATH UartFlowControlDevicePath;
    UINT64                      BaseAddress;
    UART_TYPE                   UartType;
    BOOLEAN                     WaitForXon;
    UINT32                      ErrorCount;
    UINTN                       Fifohead;
    UINTN                       Fifotail;
    UINTN                       DataCount;
    UINT8                       LoopbackBuffer[UART_MAX_RECEIVE_FIFO_DEPTH];
    BOOLEAN                     Started;
    BOOLEAN                     LineStatusRegOverrunErrorBit;
    BOOLEAN                     SerialDevicePresentOnPort;
    CHAR16                      *ControllerName; 
    EFI_PCI_IO_PROTOCOL         *PciIo;
    UINT8                       BarIndex;
    UINT8                       ReadWriteSemaphore; 
    BOOLEAN                     MMIODevice;
    UINT32                      SerialPortErrorCount;
    EFI_EVENT                   SerialErrorReEnableEvent;
    BOOLEAN                     TimerEventActive;
    BOOLEAN                     AmiSerialDetected;
    BOOLEAN                     IsPciAmiSerial;
    BOOLEAN                     FlowCtrlCTSnotSet;
    UINTN                       UartClock;
    UINT8                       MmioWidth;
} SERIAL_PARAMETERS;


//
// LOCAL FUNCTION PROTOTYPES
//

BOOLEAN
LookupHID (
    UINT32                          hid
);

EFI_STATUS
GetSerialIo_DP (
    EFI_DRIVER_BINDING_PROTOCOL     *This,
    EFI_HANDLE                      Controller,
    ACPI_HID_DEVICE_PATH**          SerialIodp,
    UINT32                          Attributes,
    BOOLEAN                         Test
);

UINT8 
SerialReadPort (
    IN SERIAL_PARAMETERS            *Parameters,
    IN UINT32                       Offset
);

VOID 
SerialWritePort (
    IN SERIAL_PARAMETERS            *Parameters,
    IN UINT32                       Offset,
    IN UINT8                        Data
);

VOID AddSerialFifo(
    IN SERIAL_PARAMETERS            *Parameters,
    IN UINT8                        Data
);

UINT8 RemoveSerialFifo(
    IN SERIAL_PARAMETERS            *Parameters
);

BOOLEAN CheckSerialFifoEmpty(
    IN SERIAL_PARAMETERS            *Parameters
);

BOOLEAN CheckSerialFifoFull(
    IN SERIAL_PARAMETERS            *Parameters
);

VOID
CheckThreBit (
    EFI_EVENT Event, 
    VOID *Context
);

EFI_STATUS
EFIAPI
SerialIoSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS
EFIAPI
SerialIoStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS
EFIAPI
SerialIoStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     ControllerHandle,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
);


EFI_STATUS
EFIAPI
EfiSerialReset (
    IN EFI_SERIAL_IO_PROTOCOL      *This
);

EFI_STATUS
EFIAPI
EfiSerialSetAttributes (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN UINT64                   BaudRate,
    IN UINT32                   ReceiveFifoDepth,
    IN UINT32                   Timeout,
    IN EFI_PARITY_TYPE          Parity,
    IN UINT8                    DataBits,
    IN EFI_STOP_BITS_TYPE       StopBits
);

EFI_STATUS
EFIAPI
EfiSerialSetControl (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN UINT32                   Control
);

EFI_STATUS
EFIAPI
EfiSerialGetControl (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    OUT UINT32                  *Control
);

EFI_STATUS
EFIAPI
EfiSerialRead (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN OUT UINTN                *BufferSize,
    OUT VOID                    *Buffer
);

EFI_STATUS
EFIAPI
EfiSerialWrite (
    IN EFI_SERIAL_IO_PROTOCOL   *This,
    IN OUT UINTN                *BufferSize,
    IN VOID                     *Buffer
);

VOID
SetUartType (
    IN SERIAL_PARAMETERS        *Parameters
);

VOID
EnableFifoBuffers (
    IN SERIAL_PARAMETERS        *Parameters,
    UINT32                      NewFifoDepth
);

BOOLEAN
CheckForLoopbackDevice (
    IN SERIAL_PARAMETERS        *SerialIo
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
