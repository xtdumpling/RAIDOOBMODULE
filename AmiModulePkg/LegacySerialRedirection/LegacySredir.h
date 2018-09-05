//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************


/** @file LegacySredir.h
    Legacy console redirection Protocol header file
*/


#ifndef __LEGACY_SREDIR_H__
#define __LEGACY_SREDIR_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>

#define SREDIR_VENDORID 0x7
#define SREDIR_DEVICEID 0x9
#define SREDIR_MODULEID 0x3


#ifndef     SERIAL_WRITE_ERROR_CHECK
#define     SERIAL_WRITE_ERROR_CHECK 1
#ifndef     MAXIMUM_SERIAL_WRITE_ERROR_COUNT
#define     MAXIMUM_SERIAL_WRITE_ERROR_COUNT 10
#endif
#endif

#ifndef     MAX_FAILURES_ALLOWED
#define     MAX_FAILURES_ALLOWED 5
#endif

#define     CONVENTIONAL_MEMORY_TOP     0xA0000
#define     SREDIR_BIN_SIZE             0x3C00


//Parameter passed to sredir.bin
#define     DISPLAY_SCREEN              0x01
#define     INT10_WORKAROUND            0x02
#define     MMIO_DEVICE                 0x04
#define     SERIAL_READWRITE_CALLBACK   0x08
#define     CTRLI_MAPPING               0x10

#define     SERIAL_ERROR_CHECK          0x80
#define     ESC_SEQUENCES               0x0100
#define     CTRLH_MAPPING               0x0200

// UART Register Related defines
#define     DLAB_BIT                    1   
#define     LSR_OFFSET                  0x05
#define     LCR_OFFSET                  0x03
#define     DIV_MSB                     0x01
#define     DIV_LSB                     0

#define     STOPB   0x4                 //      Bit2: Number of Stop Bits
#define     PAREN   0x8                 //      Bit3: Parity Enable
#define     EVENPAR 0x10                //      Bit4: Even Parity Select
#define     STICPAR 0x20                //      Bit5: Sticky Parity
#define     SERIALDB 0x3                //      Bit0-1: Number of Serial 
                                        //                 Data Bits

#define     SERIAL_REGISTER_THR 0       // WO   Transmit Holding Register
#define     SERIAL_REGISTER_RBR 0       // RO   Receive Buffer Register

#define     SERIAL_REGISTER_FCR 2       // WO   FIFO Control Register
#define     TRFIFOE 0x1                 //          Bit0: Transmit and Receive 
                                        //                 FIFO Enable
#define     RESETRF 0x2                 //          Bit1: Reset Receiver FIFO
#define     RESETTF 0x4                 //          Bit2: Reset Transmitter FIFO

#define     SERIAL_REGISTER_MCR 4       // R/W  Modem Control Register
                                        //            interrupts
#define     LME 0x10                    //           Bit4: Loop back Mode Enable

#define     SERIAL_REGISTER_LSR 5       // R/W  Line Status Register
#define     DR 0x1                      //           Bit0: Receiver Data Ready Status

#define     SERIAL_REGISTER_MSR 6       // R/W  Modem Status Register
#define     CTS 0x10                    //           Bit4: Clear To Send Status
#define     DSR 0x20                    //           Bit5: Data Set Ready Status
#define     RI 0x40                     //           Bit6: Ring Indicator Status
#define     DCD 0x80                    //           Bit7: Data Carrier Detect Status

#define     SERIAL_REGISTER_SCR 7       // R/W  Scratch Pad Register

#define     BYTE_WIDTH      1
#define     WORD_WIDTH      2
#define     DWORD_WIDTH     4


// ACPI PM timer signature,for 'TM' the ASCII code is 0x544D
// the MSB bit of the signature denotes 24bit/32bit timer
#if ACPI_SUPPORT
#if FACP_FLAG_TMR_VAL_EXT
#define TIMER_SIGNATURE_32BIT 0xD44D0000
#else
#define TIMER_SIGNATURE_24BIT 0x544D0000
#endif
#endif


/**
    This structure consists of functions which are used for Legacy Redirection

 Fields:

 LEGACY_SreDirInitializeSerialPort : Initializes the Serial Port
 LEGACY_SerialRedirection          : Starts the Legacy Serial Redirection by hooking the required interrupts
 LEGACY_ReleaseSerialRedirection   : Stops the Legacy Serial Redirection by Releasing the corresponding interrupts
 LEGACY_InvalidFunction_FAR        : Invalid function

*/


typedef enum {
    LEGACY_SreDirInitializeSerialPort,  // 0000,
    LEGACY_SerialRedirection,           // 0001,
    LEGACY_ReleaseSerialRedirection,    // 0002,
    LEGACY_GetInterruptAddress,         // 0003,
    LEGACY_ClearKbCharBuffer,           // 0004,
    Legacy_FindDataAreaAndInitSetup,    // 0005,
    LEGACY_InvalidFunction_FAR,         // 0006
} AMI_LEGACYSREDIR_FUNCTIONS;

/**
 Description:  There is a table located within the traditional BIOS.It is located on a 16-byte
              boundary and provides the physical address of the entry point for the Legacy Redirection
              functions. These functions provide the platform-specific information that is 
              required by the generic EfiCompatibility code. The functions are invoked via 
              thunking by using EFI_LEGACY_BIOS_PROTOCOL.FarCall86() with the 32-bit physical 
              entry point defined below. 

*/
#pragma pack(1)
typedef struct _AMI_LEGACYSREDIR_TABLE {
    UINT32      Signature;
    UINT8       Version;
    UINT8       TableChecksum;
    UINT8       TableLength;
    UINT16      SreDirSegment;
    UINT16      SreDirOffset;
    UINT16      SreDirEfiToLegacyOffset;
}AMI_LEGACYSREDIR_TABLE;

/**
    This structure consists of COM parameters and setup values
    which are used to transfer the data from EFI to Legacy for
    Legacy Redirection.

    @note  Don't Change this structure,as the same structure is defined in
           CSM16 Serial Redirection. 
*/
typedef struct _AMI_COM_PARAMETERS {
    UINT32  SdlParameters;
    UINT16  BaseAddress;
    UINT32  MMIOBaseAddress;
    UINT8   SerialIRQ;
    UINT32  Baudrate;
    UINT8   TerminalType;
    UINT8   FlowControl;
    UINT8   DataParityStop;
    UINT8   LegacyOsResolution;
    UINT8   RecorderMode;
    UINT8   VtUtf8;
    UINT8   PuttyKeyPad;
    UINT8   SwSMIValue;
    UINT8   InstallLegacyOSthroughRemote;
    UINT16  SredirBinSize; 

    UINT8   RedirectionAfterBiosPost;
    UINT8   Flag;                    //This is not a setup variable rather than used as 
                                     // a flag to start or stop Serial Redirection
    UINT8   RefreshScreenKey;
    UINT8   SwSMIPort;               // Sw SMI port Address
    UINT8   UartPollingRedirection;
    UINT8   FlowControlFailureCount;
    UINT8   CheckForLoopBackRetryCount;
    UINT16  MaximumErrorCount;
    UINT32  AcpiTimerAddress;
    UINT8   MmioWidth;
}AMI_COM_PARAMETERS;

/**
  This structure consists of Set Color ESC Sequence 

  @Notes:       Don't Change this structure,as the same structure is defined in
                CSM16 Serial Redirection. 
*/
typedef struct _SET_COLOR_ESC_SEQ {
    UINT32  reset_Attribute;
    UINT8   esc_char;
    UINT8   sq_bracket;
    UINT8   attrib;
    UINT8   semicolon1;
    UINT16  foreground;
    UINT8   semicolon2;
    UINT16  background;
    UINT16  m_char ;
}SET_COLOR_ESC_SEQ; 

typedef struct LEGACY_SREDIR_SMM {
    UINT32  MMIOAddress;
    UINT8   FuncNo;
    UINT8   ReadWrite;    
    UINT8   PortOffset;
    UINT8   Value;
    UINT8   Count;
    UINT32  BufferAddress;
} LEGACY_SREDIR_SMM;

/**
  This structure consists of Data parameter of Legacy serial module

  @Notes:       Don't Change this structure,as the same structure is defined in
                CSM16 Serial Redirection. 
*/

typedef struct _AMI_SREDIR_DATA {

    UINT32      Signature;
    UINT8       Flag;
    UINT8       InsideInt10;
    UINT8       Exception_flag;
    UINT8       REL_INT08_FLAG;
    UINT8       TRACK_REL_INT08_FLAG;
    UINT8       software_fc_flag;
    UINT8       TimeInterrupt_UnHooked;
    UINT8       SeriaInterrupt_UnHooked;
    UINT8       Our40_17;
    UINT8       cTemp;
    UINT8       bESCSequenceBufferHead;
    UINT8       bESCSequenceBufferTail;
    UINT8       color_save;
    UINT8       bColumnOffset;
    UINT8       DisableRedirection;
    UINT8       FlowCtrlCTSnotSet;
    UINT8       Flow_Control_Failure_Count;
    UINT8       LineNum;
    UINT8       PendingLine;
    UINT8       cntr;
    UINT8       Vga_mode_80_x_30;
    UINT16      OrgFS;
    UINT16      GraphredirFlags;
    UINT16      sKBCCharacterBufferHead;
    UINT16      sKBCCharacterBufferTail;
    UINT16      CursorPosition;
    UINT16      CursorType;
    UINT16      LineSize;
    UINT16      color_data;
    UINT16      video_ptr;
    UINT16      buffer_counter;
    UINT16      active_screenSegment;
    UINT16      PrevCursorPos;
    UINT16      Flow_Control_Count;
    UINT16      Error_count;
    UINT16      active_screenOffset;
    UINT16      active_inc;

    UINT32      OldIntHandler;
    UINT32      OldInt10hHandler;
    UINT32      prev_cksum;
    UINT32      SavedStackForFifo;
    UINT32      SavedStackForTimer;
    UINT32      cur_cksum;
    UINT32      TmpCkSum;
    UINT32      old_com_handler;
    UINT32      Int10hAddress;
    UINT32      Int0ChAddress;
    UINT32      Int14hAddress;
    UINT32      old_Int14h_handler;
    UINT32      OldInt08hHandler;
    UINT32      VideoCtrlInt10hHandler;

    UINT8       VT100_attrib_[8];
    UINT8       VT100_CursorPosition[8];
    SET_COLOR_ESC_SEQ  ESC_Seq;
    UINT8       interrupt_buffer[15];
    UINT8       sESCSequenceBuffer[6];
    UINT8       sKBCCharacterBufferStart[0x80];
    UINT8       BufferEnd;
    UINT8       Timer_Stack[0x200];
    UINT8       Timer_Stack_Label;
    UINT8       Fifo_Stack[0x200];
    UINT8       Fifo_Stack_Label;
    UINT16      CurrentLineData[80];
    UINT32      LineCksmSREDIR[25];
    LEGACY_SREDIR_SMM LegacySreDirSmm ;

}AMI_SREDIR_DATA;


typedef struct { 
    UINT16 VendorId; 
    UINT16 DeviceId; 
} INVALID_PCICOM;

#pragma pack()

EFI_STATUS 
InitilizeNonCommonSerialRegsiters(
    IN AMI_COM_PARAMETERS *AmiComParameters
);

EFI_STATUS
CopyAmiSredirDataIntoBaseMemory (
    IN AMI_LEGACY_SREDIR_PROTOCOL   *This
);

EFI_STATUS
EFIAPI
EnableLegacySredir (
    IN AMI_LEGACY_SREDIR_PROTOCOL   * This
);

EFI_STATUS
EFIAPI
DisableLegacySredir (
    IN AMI_LEGACY_SREDIR_PROTOCOL   * This
);

EFI_STATUS
LegacySreDirInSmmFunction(
    IN EFI_HANDLE                ImageHandle,
    IN EFI_SYSTEM_TABLE          *SystemTable
 );

EFI_STATUS 
DataCopiedToCsm16Function(
    IN  UINTN                  StructStartAddress,
    IN  AMI_COM_PARAMETERS     *AmiComParameters
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
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
