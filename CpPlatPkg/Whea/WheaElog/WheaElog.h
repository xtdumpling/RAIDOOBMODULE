//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This driver produces the SW SMI handlers that abstracts the WHEA Error 
  Record Storage I/F. The runtime driver also publishes the serialization 
  actions as ERST.

  Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_ELOG_H_
#define _WHEA_ELOG_H_

//
// Statements that include other files
//
#include <Uefi.h>

#include <IndustryStandard/WheaDefs.h>
#include <IndustryStandard/WheaAcpi.h>


#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

// Consumed protocols
#include <Protocol/CpuIo2.h>
#include <Protocol/WheaSupport.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/FirmwareVolumeBlock.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Modified as the value is same as EFI_WHEA_ME_EINJ_CONSUME_SWSMI
#define WHEA_ERROR_RECORD_SWSMI                                 0x90 
// APTIOV_SERVER_OVERRIDE_RC_END : Modified as the value is same as EFI_WHEA_ME_EINJ_CONSUME_SWSMI

//
// Statements that include other files
//

#define ErstRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  8,                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}

#define ErstRegisterFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_3_0_QWORD,         \
  0}

#pragma pack (1)

#define   M_ATTRIBUTE_MASK    0x03  // Only BIT0 & BIT1 to be checked
#define   B_RECORD_FREE       0x03
#define   B_RECORD_INUSE      0x03  // Multiplex FREE & INUSE
#define   B_RECORD_PROCESSED  0x02
#define   B_RECORD_CLEARED    0x00


typedef struct {
  UINT16      Signature;
  UINT32      Resv1;
  UINT8       Resv2;
  // BIT[1:0]=11(free),01(In Use),00(cleared) BIT[7:2] is Reserved
  // THIS IS THE ONLY POSSIBLE STATES DUE TO FLASH CHIP DESIGN
  UINT8       Attributes; 
} EFI_ERROR_RECORD_SERIALIZATION_INFO;

typedef struct {
	UINT32			Signature;
	UINT16			Revision;	
	UINT32			PlatformReserved;
	UINT16			SectionCount;
	UINT32			ErrorSeverity;
	UINT32			ValidationBits;
	UINT32			RecordLength;
	UINT64			TimeStamp;
	EFI_GUID		PlatformID;
	EFI_GUID		PartitionID;
	EFI_GUID		CreatorID;
	EFI_GUID		NotificationType;
	UINT64			RecordID;
	UINT32			Flag;
	UINT64			OSReserved; // EFI_ERROR_RECORD_SERIALIZATION_INFO
	UINT64			Reserved0;
	UINT32			Reserved1;
} ERROR_RECORD_HEADER;

typedef EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  FV_BLOCK;
typedef struct {
  FV_BLOCK              *FvBlock;
  EFI_PHYSICAL_ADDRESS  BaseAddress;          // Physical Address where ELOG Records start
  UINT32                Length;               // ELOG Space for Records in bytes (excluding FV HDR etc)
  UINT32                TotalRecordBytes;     // Bytes occupied by records in ELOG including cleared ones
  UINT32                FreeRecordBytes;      // Bytes occupied by records marked as cleared
  EFI_PHYSICAL_ADDRESS  NextRecordAddress;    // Address where a new record can be appended
  UINT16                HeaderLength;
} EFI_WHEA_ELOG_STATISTICS;

typedef struct {
  UINT64      Command;
  UINT64      RecordID;
  UINT64      NextRecordID;
  UINT32      LogOffset;
  UINT32      Padding0;
  UINT64      Status;
  UINT64      BusyFlag;
  UINT64      RecordCount;
  UINT64      ErrorLogAddressRange;
  UINT64      ErrorLogAddressLength;
  UINT64      ErrorLogAddressAttributes;
  UINT64      Dummy;
  UINT64	    CurRecordID;
} WHEA_PARAMETER_REGION;

#pragma pack()

//
// Module prototypes
//
// APTIOV_SERVER_OVERRIDE_RC_START : Using our ERST implementation as fatal error was not working with Intel WHEA
/*
EFI_STATUS
EFIAPI
InitializeWheaElog (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  )*/
EFI_STATUS
EFIAPI
WheaErstInit (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  )
// APTIOV_SERVER_OVERRIDE_RC_END : Using our ERST implementation as fatal error was not working with Intel WHEA
/*++

Routine Description:

  Initializes the SMM Handler Driver

Arguments:
  ImageHandle - ImageHandle of the loaded driver
  SystemTable - Pointer to the System Table

Returns:
  EFI_SUCCESS - If all services discovered.
  Other       - Failure in constructor.

--*/
;

EFI_STATUS
WheaElogSwSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  );

//
// Local Function Prototypes
//
EFI_STATUS
PrstInit (
  VOID
  );

EFI_STATUS
PrstDefragment (
  VOID
  );

VOID
PrstRead (
  EFI_PHYSICAL_ADDRESS  RecordBuffer,
  UINT64                RecordID,
  UINT64                *Status
  );

VOID
PrstWrite(
  EFI_PHYSICAL_ADDRESS  Record,
  UINT64                *Status
  );

VOID
PrstClear (
  UINT64                RecordID,
  UINT64                *Status
  );

EFI_STATUS
ConvertPhyAddressToLbaOffset (
  IN  EFI_PHYSICAL_ADDRESS   Address,
  OUT EFI_LBA                *Lba,
  OUT UINTN                  *Offset
  );

extern EFI_GUID gEfiWheaElogFvGuid;
extern EFI_GUID gEfiSmmFirmwareVolumeBlockProtocolGuid;
// APTIOV_SERVER_OVERRIDE_RC_START : Using our ERST implementation as fatal error was not working with Intel WHEA
//
// WHEA ERST ACPI TAble
//

#define WHEA_ERST_INSTR_ENTRIES 16

//
// ACPI 3.0 Generic Address Space definition
//
typedef struct _EFI_ACPI_3_0_GAS{
  UINT8	  AddressSpaceId;
  UINT8   RegisterBitWidth;
  UINT8   RegisterBitOffset;
  UINT8   AccessSize;
  UINT64  Address;
} EFI_ACPI_3_0_GAS;

typedef struct _WHEA_SERIALIZATION_HEADER {
  UINT32   InjectionHeaderSize;
  UINT32   Reserved;
  UINT32   InjectionInstEntryCount;
} WHEA_SERIALIZATION_HEADER;


typedef struct _WHEA_SERIALIZATION_INST_ENTRY {
  UINT8			SerializationAction;
  UINT8			Instruction;
  UINT8			Flags;
  UINT8			Reserved;
  EFI_ACPI_3_0_GAS	RegisterRegion;
  UINT64		Value;
  UINT64		Mask;
} WHEA_SERIALIZATION_INST_ENTRY;

typedef struct _WHEA_ERST_ACPI_TABLE{
   EFI_ACPI_DESCRIPTION_HEADER					AcpiHeader;
  WHEA_SERIALIZATION_HEADER		  SerializationHeader;
  WHEA_SERIALIZATION_INST_ENTRY	SerializationInstEntry[WHEA_ERST_INSTR_ENTRIES];
} WHEA_ERST_ACPI_TABLE;

//
// Error Record Serailization Action Definition
//
#define ERST_BEGIN_WRITE_OPERATION		0x0
#define ERST_BEGIN_READ_OPERATION		0x1
#define ERST_BEGIN_CLEAR_OPERATION		0x2
#define ERST_END_OPERATION			0x3
#define ERST_SET_RECORD_OFFSET			0x4
#define ERST_EXECUTE_OPERATION			0x5
#define ERST_CHECK_BUSY_STATUS			0x6
#define ERST_GET_OPERATION_STATUS		0x7
#define ERST_GET_RECORD_IDENTIFIER		0x8
#define ERST_SET_RECORD_IDENTIFIER		0x9
#define ERST_GET_RECORD_COUNT			0xA
#define ERST_BEGIN_DUMMY_WRITE_OPERATION	0xB
#define ERST_RESERVED				0xC
#define ERST_GET_ERROR_LOG_ADDRESS_RANGE	0xD
#define ERST_GET_ERROR_LOG_ADDRESS_LENGTH	0xE
#define ERST_GET_ERROR_LOG_ADDRESS_ATTR		0xF

//
// ERST Command Status Definition
//
#define ERST_SUCCESS				0x0
#define ERST_NOT_ENOUGH_SPACE			0x1
#define ERST_HARDWARE_NOT_AVAILABLE		0x2
#define ERST_FAILED				0x3
#define ERST_RECORD_STORE_EMPTY			0x4
#define ERST_RECORD_NOT_FOUND			0x5

//
//Serialization Instruction Definition
//
#define ERST_READ_REGISTER			0x0
#define ERST_READ_REGISTER_VALUE		0x1
#define ERST_WRITE_REGISTER			0x2
#define ERST_WRITE_REGISTER_VALUE		0x3
#define ERST_NOOP				0x4

//
//ERST Serialization Action Register Region of memory location definition
//
#define ERST_END_OPER_OFFSET		1
#define ERST_SET_REC_OFFSET		2
#define ERST_CHK_BUSY_OFFSET		6
#define ERST_GET_OPER_STATUS_OFFSET	7
#define ERST_GET_REC_IDENT_OFFSET	8
#define ERST_SET_REC_IDENT_OFFSET	8
#define ERST_GET_REC_COUNT_OFFSET	16
#define ERST_GET_MEM_OFFSET		18
#define ERST_GET_MEM_SIZE_OFFSET	26
#define ERST_GET_MEM_ATTRIBUTES_OFFSET	30

//
//FLAGS
//
#define WHEA_CLEAR_FLAG			0
#define WHEA_SET_FLAG 			1

//
//Whea Instruction definition
//

#define WHEA_READ_REG			0x0
#define WHEA_READ_REG_VALUE		0x1
#define WHEA_WRITE_REG			0x2
#define WHEA_WRITE_REG_VALUE		0x3
#define WHEA_NOOP			0x4


#define WHEA_ACCESS_BYTE		0x1
#define WHEA_ACCESS_WORD		0x2
#define WHEA_ACCESS_DWORD		0x3
#define WHEA_ACCESS_QWORD		0x4

#define WHEA_ENTRY_ADDRID_SYSMEM	0x0
#define WHEA_ENTRY_ADDRID_SYSIO		0x1
#define WHEA_ENTRY_ADDRID_RES		0x0

//
//Reserved
//
#define WHEA_RESERVED			0

//
//Register bit width
//
#define REG_BIT_WIDTH_BYTE		8
#define REG_BIT_WIDTH_WORD		16
#define REG_BIT_WIDTH_DWORD		32
#define REG_BIT_WIDTH_QWORD		64

//
//Register bit offset
//
#define REG_BIT_OFFSET0			0


//
//Mask
//

#define WHEA_BYTE_MASK			0xff
#define WHEA_WORD_MASK			0xffff
#define WHEA_DWORD_MASK			0xffffffff
#define WHEA_QWORD_MASK			0xffffffffffffffff
#define WHEA_CHK_BUSY_STS_MASK		0x01
#define WHEA_GET_CMD_STS_MASK		0xfffe

#define SMIPORT 0xB2

#define ERST_PERSISTENT_STORE_GUID \
  { 0x193A3779, 0xA2E3, 0x4f08, 0xB4, 0xAB, 0x43, 0x99, 0x8E, 0x55, 0x02, 0xFF }

enum {
  WHEA_PREPEND_MAGIC_NUMBER = 0x44332211,
  WHEA_APPEND_MAGIC_NUMBER = 0x11223344
};

enum {
  FLASH_ERASE_VALUE = 0xFF,
  MARKED_AS_DELETE = 0x00
};

#pragma pack(1)

//
//Defined in UEFI spec
//
typedef struct _UEFI_ERROR_RECORD_HEADER {
  UINT32	SignatureStart;
  UINT16	Revision;
  UINT32	SignatureEnd;
  UINT16	SectionCount;
  UINT32	ErrSeverity;
  UINT32	ValidBits;
  UINT32	RecLength;
  UINT64	Timestamp;
  EFI_GUID	PlatformID;
  EFI_GUID	PartitionID;
  EFI_GUID	CreatorID;
  EFI_GUID	NotifiType;
  UINT64	RecordID;
  UINT32	Flags;
  UINT64	OSReserved;
  UINT8		Reserved[12];
} UEFI_ERROR_RECORD_HEADER;

//
//Header of Error Record in GPNV area
//
typedef struct _NV_PREPEND_DATA {
  UINT32      MagicNum;
  UINT8       RecordValidity;   //if status = ERASE_VALUE, then valid record
} NV_PREPEND_DATA;


//
//Footer of Error Record in GPNV area
//
typedef struct _NV_APPEND_DATA {
  UINT32      MagicNum;
} NV_APPEND_DATA;


typedef struct {
  UINT16      Signature;
  UINT32      Reserved1;
  UINT8       Reserved2;
  UINT8       Attributes;
} ERROR_RECORD_SERIALIZATION_INFO;


typedef struct _PERSISTENT_ERROR_RECORD {
  NV_PREPEND_DATA NvPrependData;
  UEFI_ERROR_RECORD_HEADER UefiErrorRecord;
} PERSISTENT_ERROR_RECORD;


#define PS_PREPEND_DATA_SIZE sizeof(NV_PREPEND_DATA)
#define PS_APPEND_DATA_SIZE sizeof(NV_APPEND_DATA)

#pragma pack()

//
//These are the registers that will be read/written by windows
//
typedef struct _WHEA_ERST_REGISTERS{
  UINT8         CmdReg;
  UINT8         EndOperation;
  UINT32        RecordOffset;
  UINT8         BusyStatus;
  UINT8         OperationStatus;
  UINT64        ValidRecordIdentifier; //firmware stores a record identifier which is available in persistent store
  UINT64        OSRecordIdentifier; //OS tells to firmware which record identifier to be read
  UINT16        RecordCount;
  UINT64        ElarAddress;
  UINT32        ElarLength;
  UINT8         ElarAttributes;
} WHEA_ERST_REGISTERS;

typedef struct _UEFI_SECTION_DESCRIPTOR {
  UINT32 SectionOffset;
  UINT32 SectionLength;
  UINT16 Revision;
  UINT8 ValidationBits;
  UINT8 Reserved;
  UINT32 Flags;
  EFI_GUID SectionType;
  UINT8 FruId[16];
  UINT32 SectionSeverity;
  UINT8 FruTest[20];
} UEFI_SECTION_DESCRIPTOR;

typedef struct _GENERIC_PROCESSOR_ERROR_SECTION {
   UINT64 ValidationBits;
   UINT8 ProcessorType;
   UINT8 ProcessorIsa;
   UINT8 ProcessorErrorType;
   UINT8 Operation;
   UINT8 Flags;
   UINT8 Level;
   UINT16 Reserved;
   UINT64 CouVerInfo;
   UINT8 CpuBrandString[128];
   UINT64 ProcessorId;
   UINT64 TargetAddr;
   UINT64 RequestorId;
   UINT64 ResponderId;
   UINT64 InstructionIp;
} GENERIC_PROCESSOR_ERROR_SECTION;

typedef struct _DUMMY_ERST_RECORD {
  UEFI_ERROR_RECORD_HEADER UefiErrorRecord;
  UEFI_SECTION_DESCRIPTOR  UefiSectionDescriptor;
  GENERIC_PROCESSOR_ERROR_SECTION GenProcessorErrSec;
} DUMMY_ERST_RECORD;

EFI_STATUS
ErstSWSMIHandler(
  IN EFI_HANDLE                        DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT *RegisterContext,
  IN OUT EFI_SMM_SW_CONTEXT            *SwContext,
  IN OUT UINTN                         *CommBufferSize
);

// APTIOV_SERVER_OVERRIDE_RC_END : Using our ERST implementation as fatal error was not working with Intel WHEA
#endif


