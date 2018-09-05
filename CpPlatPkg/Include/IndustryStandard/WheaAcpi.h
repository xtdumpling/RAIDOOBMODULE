//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  All WHEA ACPI related structure, data and table definitions.

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_ACPI_H_
#define _WHEA_ACPI_H_

//
// Statements that include other files
//

#include <Uefi.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi30.h>

#include <Protocol/WheaSupport.h>
//
// Ensure proper structure formats
//
#pragma pack(1)


#define EFI_ACPI_WHEA_HEST_TABLE_SIGNATURE            SIGNATURE_32('H','E','S','T')
#define EFI_ACPI_WHEA_ERST_TABLE_SIGNATURE            SIGNATURE_32('E','R','S','T')
#define EFI_ACPI_WHEA_EINJ_TABLE_SIGNATURE            SIGNATURE_32('E','I','N','J')
#define EFI_ACPI_WHEA_BERT_TABLE_SIGNATURE            SIGNATURE_32('B','E','R','T')

#define EFI_WHEA_TABLE_REVISION                       1

//
// WHEA HW Error Source table definitions
//

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  ErrorEntryCount;
// Error source structures followed by
} EFI_ACPI_WHEA_ERROR_SOURCE_TABLE;


typedef struct {
    UINT16                  Type;
    UINT16                  SourceId;
    UINT16                  CfgWriteEnable;
    UINT8                   Flags;
    UINT8                   IsEnable;
    UINT32                  NoOfRecords;
    UINT32                  MaxSecPerRecord;
} PCIE_AER_SOURCE_COMMON;

//
// PCIe Root port error source structure definition
//
typedef struct {
    PCIE_AER_SOURCE_COMMON  PcieCommon;
    PCIE_ROOT_AER_SOURCE    PcieRoot;
} EFI_ACPI_WHEA_PCIE_ROOT_AER_SOURCE_ENTRY;

//
// PCIe Device error source structure definition
//
typedef struct {
    PCIE_AER_SOURCE_COMMON  PcieCommon;
    PCIE_DEVICE_AER_SOURCE  PcieDev;
} EFI_ACPI_WHEA_PCIE_DEVICE_AER_SOURCE_ENTRY;

//
// PCIe/PCI-x bridge error source structure definition
//
typedef struct {
    PCIE_AER_SOURCE_COMMON  PcieCommon;
    PCIE_BRIDGE_AER_SOURCE  PcieBridge;
} EFI_ACPI_WHEA_PCIE_BRIDGE_AER_SOURCE_ENTRY;


typedef struct {
    UINT16                  Type;
    UINT16                  SourceId;
    UINT16                  RelatedSourceId;
    UINT8                   Flags;
    UINT8                   IsEnable;
    UINT32                  NoOfRecords;
    UINT32                  MaxSecPerRecord;
    UINT32                  MaxErrorDataSize;
    EFI_ACPI_GAS            ErrorStatusAddress;
    HW_NOTIFY_METHOD        NotifyMethod;
    UINT32                  ErrorStatusSize;  
} EFI_ACPI_WHEA_GENERIC_ERROR_SOURCE_ENTRY;

//
// IA-32 Architecture Corrected Machine Check
//
typedef struct {
  UINT16      Type;
  UINT16      SourceId;
  UINT16      CfgWriteEnable;
  UINT8       Flags;
//
// SMSD_OVERRIDE_START
//
  UINT8       IsEnable;
//
// SMSD_OVERRIDE_END
//
  UINT32      NoOfRecords;
  UINT32      MaxSecPerRecord;
  XPF_MCE_SOURCE MceSource;
}EFI_ACPI_WHEA_XPF_MCE_SOURCE_ENTRY;

typedef struct {
  UINT16          Type;
  UINT16          SourceId;
  UINT16          CfgWriteEnable;
  UINT8           Flags;
  UINT8           IsEnable;
  UINT32          NoOfRecords;
  UINT32          MaxSecPerRecord;
  XPF_CMC_SOURCE  CmcSource;
}EFI_ACPI_WHEA_XPF_CMC_SOURCE_ENTRY;
//
// WHEA Error Record serialization ACPI table definitions
//
#define WHEA_ERROR_END_OP                                       0
#define	WHEA_ERROR_GET_RECORD                                   1
#define	WHEA_ERROR_SET_RECORD                                   2
#define	WHEA_ERROR_CLEAR_RECORD                                 3
#define WHEA_ERROR_DUMMY_WRITE                                  15

#define ERST_STATUS_NOT_ENOUGH_SPACE        1
#define ERST_STATUS_HARDWARE_NOT_AVAILABLE  2
#define ERST_STATUS_FAILED                  3
#define ERST_STATUS_RECORD_STORE_EMPTY      4
#define ERST_STATUS_RECORD_NOT_FOUND        5

#define ACTION_BEGIN_WRITE_OPERATION                    0x00
#define ACTION_BEGIN_READ_OPERATION                     0x01
#define ACTION_BEGIN_CLEAR_OPERATION                    0x02
#define ACTION_END_OPERATION                            0x03
#define ACTION_SET_RECORD_OFFSET                        0x04
#define ACTION_EXECUTE_OPEARTION                        0x05
#define ACTION_CHECK_BUSY_STATUS                        0x06
#define ACTION_GET_COMMAND_STATUS                       0x07
#define ACTION_GET_RECORD_IDENTIFIER                    0x08
#define ACTION_SET_RECORD_IDENTIFIER                    0x09
#define ACTION_GET_RECORD_COUNT                         0x0A
#define ACTION_BEGIN_DUMMY_WRITE_OPERATION              0x0B
#define ACTION_GET_NEXT_RECORD_IDENTIFIER               0x0C
#define ACTION_GET_ERROR_LOG_ADDRESS_RANGE              0x0D
#define ACTION_GET_ERROR_LOG_ADDRESS_RANGE_LENGTH       0x0E
#define ACTION_GET_ERROR_LOG_ADDRESS_RANGE_ATTRIBUTES   0x0F

#define INSTRUCTION_READ_REGISTER                       0x00
#define INSTRUCTION_READ_REGISTER_VALUE                 0x01
#define INSTRUCTION_WRITE_REGISTER                      0x02
#define INSTRUCTION_WRITE_REGISTER_VALUE                0x03
#define INSTRUCTION_NO_OPERATION                        0x04

#define FLAG_NOTHING                                    0x00
#define FLAG_PRESERVE_REGISTER                          0x01

typedef struct {
  UINT8                                   Operation;
  UINT8                                   Instruction;
  UINT8                                   Flags;
  UINT8                                   Reserved8;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  Register;
  UINT64                                  Value;
  UINT64                                  Mask;
} EFI_ACPI_WHEA_SERIALIZATION_ACTION;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  SerializationHeaderSize;
  UINT32                                  Resv1;
  UINT32                                  InstructionEntryCount;
} EFI_ACPI_WHEA_ERROR_SERIALIZATION_HEADER;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  SerializationHeaderSize;
  UINT32                                  Resv1;
  UINT32                                  InstructionEntryCount;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION      InstructionEntry[16];
} EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE;

//
// Definition of the Boot Error Record Table (BERT)
//

// Error region field valid bitmask
#define BOOT_ERROR_FRU_ID_VALID         0x0001
#define BOOT_ERROR_FRU_STRING_VALID     0x0002
#define BOOT_ERROR_DATA_VALID           0x0004

// Error region Error description
#define BOOT_ERROR_UNKNOWN              0x0000
#define BOOT_ERROR_RESET_BY_BIOS        0x0001
#define BOOT_ERROR_RESET_BY_SP          0x0002
#define BOOT_ERROR_SYSTEM_HUNG          0x0003

#define ACTION_COUNT                    9

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  BootErrorRegionLen;              
  UINT64                                  BootErrorRegion;
} EFI_ACPI_WHEA_BOOT_ERROR_TABLE;

//
// WHEA Error Injection table definitions
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  InjectionHeaderSize;
  UINT32                                  Resv1;
  UINT32                                  InstructionEntryCount;
} EFI_ACPI_WHEA_ERROR_INJECTION_HEADER;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  InjectionHeaderSize;
  UINT32                                  Resv1;
  UINT32                                  InstructionEntryCount;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION      InstructionEntry[ACTION_COUNT];
} EFI_ACPI_WHEA_ERROR_INJECTION_TABLE;

#pragma pack()

#endif    // _WHEA_ACPI_H_


