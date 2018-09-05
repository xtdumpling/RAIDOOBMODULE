//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiUsbCommon.h
    IPMI Usb Common definitions.

**/

#ifndef _IPMI_USB_COMMON_H_
#define _IPMI_USB_COMMON_H_

//
// AMI BMC USB interface definitions
//
#define BEGIN_SIG                   "$G2-CONFIG-HOST$"
#define BEGIN_SIG_LEN               16
#define IPMI_USB_CMD_RESERVED       0x0000
#define AMI_CMD_SECTOR              0x01
#define AMI_DATA_SECTOR             0x02

#define COMMON_READ_10_OPCODE       0x28
#define COMMON_WRITE_10_OPCODE      0x2A

#define SCSI_AMICMD_CURI_WRITE      0xE2
#define SCSI_AMICMD_CURI_READ       0xE3

//
// USB Command Status Wrapper Structure
//
#define BOT_CSW_SIGNATURE           0x53425355  // 0-3h, signature = "USBS"
//
// USB Command Block Wrapper Structure
//
#define BOT_CBW_SIGNATURE           0x43425355  // 0-3h, signature = "USBC"

//
// BMC Usb interface status definitions
//
#define IN_PROCESS                  0x8000  /* Bit 15 of Status */

#define ERR_SUCCESS                 0       /* Success */
#define ERR_BIG_DATA                1       /* Too Much Data */
#define ERR_NO_DATA                 2       /* No/Less Data Available */
#define ERR_UNSUPPORTED             3       /* Unsupported Command */

//
// General macro definitions
//
#define PM1_TIMER                   PM_BASE_ADDRESS + 8
#define ONE_THOUSAND_MICRO_SECOND   1000
#define TWO_THOUSAND_MS             2000
#define THREE_THOUSAND_MS           3000

#define TRANSFER_LENGTH             0x100  // Big Endian to little Endian: 0x0001 -> 0x0100
#define LOGICAL_UNIT_NUMBER         0x0
#define CBW_FLAG_SHIFT              0x07
#define CSW_PHASE_ERROR             0x02

//
// BMC Common Defines
//
#define EFI_BMC_OK                  0
#define EFI_BMC_SOFTFAIL            1
#define EFI_BMC_HARDFAIL            2
#define EFI_BMC_UPDATE_IN_PROGRESS  3


#define MAX_SOFT_COUNT              10
#define BLOCK_SIZE                  512

//
// For Pei Phase, MAX_BLOCK_PER_TRANSACTION is defined in IpmiUsbPei.h file
// For Dxe Phase, MAX_BLOCK_PER_TRANSACTION is defined in IpmiUsbDxe.h file
//
//#define MAX_BLOCK_PER_TRANSACTION   1


//
// Max temp data should always be in multiple of 512 bytes.
// If you want to change the value of it, MAX_BLOCK_PER_TRANSACTION value.
//
#define MAX_IPMI_TEMP_DATA          100
#define MAX_TEMP_DATA               (BLOCK_SIZE * MAX_BLOCK_PER_TRANSACTION)

#define COMBINE_NETFUN_LUN(NetFunction, Lun) (UINT8)((NetFunction << 2) | (Lun & 0xfc))

//
// Structure of IPMI Command buffer
//
#define EFI_IPMI_USB_COMMAND_DATA_HEADER_SIZE  2

//
// IPMI Instance signature
//
#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)          ((A) | (B << 8))
#endif

#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)    (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif

#define IPMI_USB_SIGNATURE              EFI_SIGNATURE_32 ('i', 'p', 'm', 'u')

//
// Completion code related definitions
//
#define COMPLETION_CODE_SUCCESS                     0x00
#define COMPLETION_CODE_DEVICE_SPECIFIC_START       0x01
#define COMPLETION_CODE_DEVICE_SPECIFIC_END         0x7E
#define COMPLETION_CODE_COMMAND_SPECIFIC_START      0x80
#define COMPLETION_CODE_COMMAND_SPECIFIC_END        0xBE

//#define ERROR_COMPLETION_CODE(a) !( (a >= COMPLETION_CODE_SUCCESS) && (a <= COMPLETION_CODE_COMMAND_SPECIFIC_END) )

#define ERROR_COMPLETION_CODE(a)        !( (a == COMPLETION_CODE_SUCCESS) || \
                            ( (a >= COMPLETION_CODE_DEVICE_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_DEVICE_SPECIFIC_END) )||\
                            ( (a >= COMPLETION_CODE_COMMAND_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_COMMAND_SPECIFIC_END) ) )

//
// CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
// IPMI_USB_CR - Returns pointer to Ipmi Usb Instance data structure
//
#define IPMI_USB_CR( Record, TYPE, Field, Signature ) \
    ( (TYPE *) ( (CHAR8 *) (Record) - (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )

#define INSTANCE_DATA_FROM_IPMI_USB_TRANSPORT_THIS(a) \
    IPMI_USB_CR( a, EFI_IPMI_USB_INSTANCE_DATA, IpmiUsbTransport, SM_IPMI_BMC_SIGNATURE )

//
// Structure of IPMI Command response buffer
//
#define EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE 1

/**
    Structure definitions for BMC USB interface
*/

typedef UINT32  EFI_BMC_STATUS;

#pragma pack(1)
typedef struct _CONFIG_CMD {
    UINT8   BeginSig[BEGIN_SIG_LEN];   ///< Begin of signature
    UINT16  Command;                   ///< Command
    UINT16  Status;                    ///< Status
    UINT32  DataInLen;                 ///< Data Length Sent to us 
    UINT32  DataOutLen;                ///< Data Length Sent by us
    UINT32  InternalUseDataIn;
    UINT32  InternalUseDataOut;
} CONFIG_CMD;

typedef struct {
  UINT8           bOpCode;        ///< 00h, Operation Code
  UINT8           bLun;           ///< 01h, Logical Unit Number, etc
  UINT32          dLba;           ///< 02h-05h, Logical Block Address
  UINT8           bReserved;      ///< 06h, Reserved
  UINT16          wTransferLength;///< 07h-08h, Transfer Length(BIG ENDIAN)
  UINT8           bControl;       ///< 09h, Control/Reserved
} COMN_RWV_CMD;

typedef struct {
  UINT32          dCbwSignature;  ///< 0-3h, CBW signature = "USBC"
  UINT32          dCbwTag;        ///< 4-7h, Tag
  UINT32          dCbwDataLength; ///< 8-0Bh, Data transfer length
  UINT8           bmCbwFlags;     ///< 0Ch, Flags
  UINT8           bCbwLun;        ///< 0Dh, Logical unit number
  UINT8           bCbwLength;     ///< 0Eh, Command block length
  UINT8           aCBWCB[16];     ///< 0Fh-1Eh, Command block buffer
} BOT_CMD_BLK;

typedef struct
{
  UINT32    dCSWSignature;
  UINT32    dCSWTag;
  UINT32    dCSWDataResidue;
  UINT8     bCSWStatus;
} CSW;

typedef struct {
  UINT8     NetFnLUN; //NetFunction : 6 + Lun : 2
  UINT8     Command;
  UINT8     CommandData[MAX_TEMP_DATA - EFI_IPMI_USB_COMMAND_DATA_HEADER_SIZE];
} EFI_IPMI_USB_COMMAND_DATA;

typedef struct {
  UINT8     CompletionCode;
  UINT8     ResponseData[MAX_TEMP_DATA - EFI_IPMI_USB_RESPONSE_DATA_HEADER_SIZE];
} EFI_IPMI_USB_RESPONSE_DATA;

//
// Ipmi USB Bmc Instance private data.
//
typedef struct {
  UINTN                     Signature;
  UINT8                     TempData[MAX_TEMP_DATA];
  UINT8                     BulkInEndpointAddr;
  UINT8                     BulkOutEndpointAddr;
  UINT32                    BOTCommandTag;
  EFI_BMC_STATUS            BmcStatus;
  IPMI_USB_TRANSPORT        IpmiUsbTransport;
  EFI_HANDLE                UsbIoHandle;
  VOID                      *UsbIo;
  VOID                      *Context; //In PEI phase, it contains **PeiServices.
} EFI_IPMI_USB_INSTANCE_DATA;

#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
