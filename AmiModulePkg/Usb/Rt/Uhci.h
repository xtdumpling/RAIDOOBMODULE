//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Uhci.h
    AMI USB UHCI header file

**/

// Avoid including multiple instance of this file
#ifndef     __UHCI_H
#define     __UHCI_H

#define UHCI_DATA_AREA_SIZE         0x1000
#define UHCI_FRAME_LIST_SIZE        1024

#define USB_UHCI_REG_LEGSUP         0xC0

//---------------------------------------------------------------------------
//      UHCI I/O register read access definitions
//---------------------------------------------------------------------------

// Equates for UHCI I/O mapped registers (offsets from base address)

#define UHCI_COMMAND_REG        0x00
#define UHCI_STATUS_REG         0x02
#define UHCI_INTERRUPT_ENABLE   0x04
#define UHCI_FRAME_NUM          0x06
#define UHCI_FRAME_LIST_BASE    0x08
#define UHCI_SOF_MODIFY         0x0C
#define UHCI_PORT1_CONTROL      0x10
#define UHCI_PORT2_CONTROL      0x12

// Bit definitions for UHCI command register

#define UHC_HOST_CONTROLLER_RUN     BIT0
#define UHC_HOST_CONTROLLER_RESET   BIT1
#define UHC_GLOBAL_RESET            BIT2
#define UHC_ENTER_SUSPEND           BIT3
#define UHC_FORCE_RESUME            BIT4
#define UHC_CONFIGURE_FLAG          BIT6
#define UHC_MAX_PACKET_64_BYTE      BIT7

// Bit definitions for UHCI status register

#define UHC_USB_INTERRUPT           BIT0
#define UHC_USB_ERROR_INTERRUPT     BIT1
#define UHC_RESUME_RECEIVED         BIT2
#define UHC_PCI_BUS_ERROR           BIT3
#define UHC_HC_PROCESS_ERROR        BIT4
#define UHC_HC_HALTED               BIT5

// Bit definitions for USB interrupt enable register

#define UHC_TIMEOUT_CRC_ENABLE      BIT0
#define UHC_RESUME_ENABLE           BIT1
#define UHC_IOC_ENABLE              BIT2
#define UHC_SHORT_PACKET_ENABLE     BIT3

// Bit definitions for Port Status and Control

#define UHC_CONNECT_STATUS          BIT0
#define UHC_CONNECT_STATUS_CHANGE   BIT1
#define UHC_PORT_ENABLE             BIT2
#define UHC_PORT_ENABLE_CHANGE      BIT3
#define UHC_LINE_STATUS             0x30
#define UHC_LINE_STATUS_CHANGE      BIT6
#define UHC_LOW_SPEED_ATTACHED      BIT8
#define UHC_PORT_RESET              BIT9
#define UHC_PORT_SUSPEND            BIT12

//---------------------------------------------------------------------------
//      UHCI Controller Port 60/64h trapping flags
//---------------------------------------------------------------------------
#define UHCI_TRAP_PORT_60h_READ     BIT0
#define UHCI_TRAP_PORT_60h_WRITE    BIT1
#define UHCI_TRAP_PORT_64h_READ     BIT2
#define UHCI_TRAP_PORT_64h_WRITE    BIT3
#define UHCI_SMI_ON_USB_INTERRUPT   BIT4
#define UHCI_TRAP_FLAG              (UHCI_SMI_ON_USB_INTERRUPT)

//---------------------------------------------------------------------------
//      Bit definitions for a generic pointer
//---------------------------------------------------------------------------
#define UHCI_TERMINATE              BIT0
#define UHCI_QUEUE_HEAD             BIT1
#define UHCI_VERTICAL_FLAG          BIT2
#define UHCI_POINTER_MASK           0xFFFFFFF0

//---------------------------------------------------------------------------
//      Bit definitions for transfer descriptor control and status
//---------------------------------------------------------------------------

#define UHCI_TD_ACTUAL_LENGTH           0x7FF
#define UHCI_TD_STATUS_FIELD            0xFE0000
#define UHCI_TD_BITSTUFF_ERROR          0x20000
#define UHCI_TD_CRC_TIMEOUT_ERROR       0x40000
#define UHCI_TD_NAK_RECEIVED            0x80000
#define UHCI_TD_BABBLE_DETECTED         0x100000
#define UHCI_TD_DATA_BUFFER_ERROR       0x200000
#define UHCI_TD_STALLED                 0x400000
#define UHCI_TD_ACTIVE                  0x800000
#define UHCI_TD_INTERRUPT_ON_COMPLETE   0x1000000
#define UHCI_TD_ISOCHRONOUS_SELECT      0x2000000
#define UHCI_TD_LOW_SPEED_DEVICE        0x4000000
#define UHCI_TD_ERROR_COUNTER           0x18000000
#define UHCI_TD_ONE_ERROR               0x8000000
#define UHCI_TD_TWO_ERRORS              0x10000000
#define UHCI_TD_THREE_ERRORS            0x18000000
#define UHCI_TD_SHORT_PACKET_DETECT     0x20000000

//---------------------------------------------------------------------------
//      Bit definitions for transfer descriptor token
//---------------------------------------------------------------------------
#define UHCI_TD_PACKET_ID                   0xFF
#define UHCI_TD_IN_PACKET                   0x69
#define UHCI_TD_OUT_PACKET                  0xE1
#define UHCI_TD_SETUP_PACKET                0x2D
#define UHCI_TD_DEVICE_ADDRESS              0x7F00
#define UHCI_TD_ENDPOINT                    0x78000
#define UHCI_TD_DATA_TOGGLE                 0x80000
#define UHCI_TD_MAX_LENGTH                  0xFFE00000

typedef enum {
    Control = 1,
    Bulk = 0,
    Interrupt = 2,
    Isochronous = 3
} XFER_TYPE;

#pragma pack(push, 1)

// UHCI TD structure
typedef struct {
    UINT32          pLinkPtr;
    UINT32          dControlStatus;
    UINT32          dToken;
    UINT32          pBufferPtr;

// AMI defined fields
    UINT32          dCSReload;  // Control status reload value
    UINT8           bCallBackIndex;
    UINT8           bActiveFlag;
    UINT16          wReserved;
    UINT8           aDataArea[8];
} UHCI_TD;

typedef struct {
    UINT32          pLinkPtr;
    UINT32          pElementPtr;
    UHCI_TD         *CurrentTd;
    UINT8           DataToggle;
    UINT32          BytesTransferred;
    BOOLEAN         ShortPacketDetected;
    XFER_TYPE       Type;
    UHCI_TD         *FirstTd;
    UINT8           Interval;
    UINT8           CallBackIndex;
    BOOLEAN         ActiveFlag;
    VOID            *DevInfoPtr;
    UINT8           aReserved[47 - 3 * sizeof(VOID*) - sizeof(XFER_TYPE)];
} UHCI_QH;

#pragma pack(pop)

typedef struct {
    UHCI_QH         *StaticQh;
  	UHCI_QH     	*RootHubQh;
    UHCI_QH         *RepeatQh;
} UHCI_DESC_PTRS;

#endif      // __UHCI_H

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
