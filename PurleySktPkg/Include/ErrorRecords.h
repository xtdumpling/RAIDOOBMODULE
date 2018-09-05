/** @file
  Contains definitions for the RAS Error Record Structures.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _ERROR_RECORD_STRUCTS_H_
#define _ERROR_RECORD_STRUCTS_H_

#define MAX_NUM_MCA_BANKS       32

// Error Record Header Flags values
#define HW_ERROR_FLAGS_RECOVERED  1
#define HW_ERROR_FLAGS_PREVERR    2
#define HW_ERROR_FLAGS_SIMULATED  3

// Processor Error Types
#define UEFI_PROC_UNKNOWN_ERR_TYPE    00
#define UEFI_PROC_CACHE_ERR_TYPE      01
#define UEFI_PROC_TLB_ERR_TYPE        02
#define UEFI_PROC_BUS_ERR_TYPE        04
#define UEFI_PROC_MICRO_ARCH_ERR_TYPE 0x8

#ifndef PLATFORM_MEM_EXTENDED_ROW_VALID
#define PLATFORM_MEM_EXTENDED_ROW_VALID             0x000040000 // UEFI 2.4a specific
#define PLATFORM_MEM_BANK_GROUP_VALID               0x000080000 // UEFI 2.4a specific
#define PLATFORM_MEM_BANK_ADDRESS_VALID             0x000100000 // UEFI 2.4a specific
#define PLATFORM_MEM_CHIP_IDENTIFICATION_VALID      0x000200000 // UEFI 2.4a specific
#endif

// Descriptor definitions
#define UEFI_DESC_FRU_ID_VALID        0x1
#define UEFI_DESC_FRU_STRING_VALID    0x2

// Notification Type Guid definitions

// Corrected Machine Check
#define UEFI_ERR_RECORD_NOTIFICATION_CMC_GUID \
  { \
    0x2dce8bb1, 0xbdd7, 0x450e, 0xb9, 0xad, 0x9c, 0xf4, 0xeb, 0xd4, 0xf8, 0x90 \
  }

// Corrected Platform Error
#define UEFI_ERR_RECORD_NOTIFICATION_CPE_GUID \
  { \
    0x4e292f96, 0xd843, 0x4a55, 0xa8, 0xc2, 0xd4, 0x81, 0xf2, 0x7e, 0xbe, 0xee \
  }

// Machine Check Exception
#define UEFI_ERR_RECORD_NOTIFICATION_MCE_GUID \
  { \
    0xe8f56ffe, 0x919c, 0x4cc5, 0xba, 0x88, 0x65, 0xab, 0xe1, 0x49, 0x13, 0xbb \
  }

// PCI Express Error
#define UEFI_ERR_RECORD_NOTIFICATION_PCIE_GUID \
  { \
    0xcf93c01f, 0x1a16, 0x4dfc, 0xb8, 0xbc, 0x9c, 0x4d, 0xaf, 0x67, 0xc1, 0x04 \
  }

// Init record
#define UEFI_ERR_RECORD_NOTIFICATION_INIT_GUID \
  { \
    0xcc5263e8, 0x9308, 0x454a, 0x89, 0xd0, 0x34, 0x0b, 0xd3, 0x9b, 0xc9, 0x8e \
  }

// Non maskable interrupt record
#define UEFI_ERR_RECORD_NOTIFICATION_NMI_GUID \
  { \
    0x5bad89ff, 0xb7e6, 0x42c9, 0x81, 0x4a, 0xcf, 0x24, 0x85, 0xd6, 0xe9, 0x8a \
  }
 
// Boot error record
#define UEFI_ERR_RECORD_NOTIFICATION_BOOT_GUID \
  { \
    0x3d61a466, 0xab40, 0x409a, 0xa6, 0x98, 0xf3, 0x62, 0xd4, 0x64, 0xb3, 0x8f \
  }

// DMA remapping error record
#define UEFI_ERR_RECORD_NOTIFICATION_DMAR_GUID \
  { \
    0x667dd791, 0xc6b3, 0x4c27, 0x8a, 0x6b, 0x0f, 0x8e, 0x72, 0x2d, 0xeb, 0x41 \
  }

// Error Record Section Type Guid defintions

#define UEFI_ERR_RECORD_SECTION_PROCESSOR_GENERIC_GUID \
  { \
    0x9876ccad, 0x47b4, 0x4bdb, 0xb6, 0x5e, 0x16, 0xf1, 0x93, 0xc4, 0xf3, 0xdb \
  }

#define UEFI_ERR_RECORD_SECTION_PROCESSOR_SPECIFIC_GUID \
  { \
    0xdc3ea0b0, 0xa144, 0x4797, 0xb5, 0x5b, 0x53, 0xfa, 0x24, 0x2b, 0x6e, 0x1d \
  }

#define UEFI_ERR_RECORD_SECTION_PLATFORM_MEM_GUID \
  { \
    0xa5bc1114, 0x6f64, 0x4ede, 0xb8, 0x63, 0x3e, 0x83, 0xed, 0x7c, 0x83, 0xb1 \
  }

#define UEFI_ERR_RECORD_SECTION_PCIE_GUID \
  { \
    0xd995e954, 0xbbc1, 0x430f, 0xad, 0x91, 0xb4, 0x4d, 0xcb, 0x3c, 0x6f, 0x35 \
  }

#define UEFI_ERR_RECORD_SECTION_FIRMWARE_GUID \
  { \
    0x81212a96, 0x09ed, 0x4996, 0x94, 0x71, 0x8d, 0x72, 0x9c, 0x8e, 0x69, 0xed \
  }

#define UEFI_ERR_RECORD_SECTION_PCIBUS_GUID \
  { \
    0xc5753963, 0x3b84, 0x4095, 0xbf, 0x78, 0xed, 0xda, 0xd3, 0xf9, 0xc9, 0xdd \
  }

#define UEFI_ERR_RECORD_SECTION_PCIDEV_GUID \
  { \
    0xeb5e4685, 0xca66, 0x4769, 0xb6, 0xa2, 0x26, 0x06, 0x8b, 0x00, 0x13, 0x26 \
  }

#define UEFI_ERR_RECORD_SECTION_DMAR_GUID \
  { \
    0x5b51fef7, 0xc79d, 0x4434, 0x8f, 0x1b, 0xaa, 0x62, 0xde, 0x3e, 0x2c, 0x64 \
  }

#define UEFI_ERR_RECORD_SECTION_VTD_GUID \
  { \
    0x71761d37, 0x32b2, 0x45cd, 0xa7, 0xd0, 0xb0, 0xfe, 0xdd, 0x93, 0xe8, 0xcf \
  }

#define UEFI_ERR_RECORD_SECTION_IOMMU_GUID \
  { \
    0x036f84e1, 0x7f37, 0x428c, 0xa7, 0x9e, 0x57, 0x5f, 0xdf, 0xaa, 0x84, 0xec \
  }

#pragma pack(1)
// This error record header follow the UEFI 2.4.0 Specification
// Refer to Appendix N.2.1
typedef struct _UEFI_ERROR_RECORD_HEADER {
  UINT32                    SignatureStart;
  UINT16                    Revision;
  UINT32                    SignatureEnd;
  UINT16                    SectionCount;
  UINT32                    Severity;
  UINT32                    ValidBits;
  UINT32                    RecordLength;
  UINT64                    Timestamp;
  UINT8                     PlatformId[16];
  UINT8                     PartitionId[16];
  UINT8                     CreatorId[16];
  EFI_GUID                  NotificationType;
  UINT64                    RecordId;
  UINT32                    Flags;
  UINT64                    PersistentInfo;
  UINT8                     Reserved[12];
} UEFI_ERROR_RECORD_HEADER;

typedef struct _UEFI_ERROR_SECTION_DESC {
  UINT32                    Offset;
  UINT32                    Length;
  UINT16                    Revision;
  UINT8                     ValidBits;
  UINT8                     Reserved;
  UINT32                    Flags;
  EFI_GUID                  SectionType;
  EFI_GUID                  FruId;
  UINT32                    Severity;
  UINT8                     FruText[20];
} UEFI_ERROR_SECT_DESC;

typedef union {
  struct  {
    UINT16                  TranactionType:2;
    UINT16                  Operation:4;
    UINT16                  Level:3;
    UINT16                  Pcc:1;
    UINT16                  Uncorrected:1;
    UINT16                  PreciseIp:1;
    UINT16                  RestartIp:1;
    UINT16                  Overflow:1;
    UINT16                  Reseved:2;
  }Bits;
  UINT16                    CacheData;
} CACHE_CHECK;

typedef union {
  struct  {
    UINT16                  TranactionType:2;
    UINT16                  Operation:4;
    UINT16                  Level:3;
    UINT16                  Pcc:1;
    UINT16                  Uncorrected:1;
    UINT16                  PreciseIp:1;
    UINT16                  RestartIp:1;
    UINT16                  Overflow:1;
    UINT16                  Reseved:2;
  }Bits;
  UINT16                    TlbData;
} TLB_CHECK;

typedef union {
  struct  {
    UINT16                  ErrorType:3;
    UINT16                  Pcc:1;
    UINT16                  Uncorrected:1;
    UINT16                  PreciseIp:1;
    UINT16                  RestartIp:1;
    UINT16                  Overflow:1;
    UINT16                  Reseved:8;
  }Bits;
  UINT16                    MsCheckData;
} MS_CHECK;

typedef struct {
  BOOLEAN                   Valid;
  UINT32                    BankNum;
  UINT32                    ApicId;
  UINT32                    BankType;
  UINT32                    BankScope;
  UINT64                    McaStatus;
  UINT64                    McaAddress;
  UINT64                    McaMisc;
} MCA_BANK_INFO;

typedef struct _PROCESSOR_ERROR_RECORD {
  // Standard fields
  UINT64                    ValidBits;
  UINT8                     Type;
  UINT8                     Isa;
  UINT8                     ErrorType;
  UINT8                     Operation;
  UINT8                     Flags;
  UINT8                     Level;
  UINT16                    Resv1;
  UINT64                    VersionInfo;
  UINT8                     BrandString[128];
  UINT64                    ApicId;
  UINT64                    TargetAddr;
  UINT64                    RequestorID;
  UINT64                    ResponderID;
  UINT64                    InstuctionIP;
  MCA_BANK_INFO             McaBankInfo;
  // APTIOV_SERVER_OVERRIDE_RC_START : Extra information providing fields
  UINT8                     OtherMcBankCount;  // Total valid MCBanks
  MCA_BANK_INFO             OtherMcBankInfo[MAX_NUM_MCA_BANKS];  // Total valid MC Banks data
  // APTIOV_SERVER_OVERRIDE_RC_END : Extra information providing fields
} PROCESSOR_GENERIC_ERROR_SECTION;

typedef struct {
  UINT16                    SocketId;
  UINT16                    UboxErrStatus;
} UNCORE_UBO_ERROR_RECORD;

typedef struct {
  // Standard fields
  UINT64                    ValidBits;
  UINT64                    ErrorStatus;
  UINT64                    PhysAddr;
  UINT64                    PhysAddrMask;
  UINT16                    Node;
  UINT16                    Card;
  UINT16                    Module;
  UINT16                    Bank;
  UINT16                    Device;
  UINT16                    Row;
  UINT16                    Col;
  UINT16                    BitPosition;
  UINT64                    RequesterId;
  UINT64                    ResponderId;
  UINT64                    TargetId;
  UINT8                     ErrorType;
  UINT8                     Extended;
  UINT16                    RankNumber;
  UINT16                    CardHandle;
  UINT16                    ModuleHandle;
  MCA_BANK_INFO             McaBankInfo;
  // APTIOV_SERVER_OVERRIDE_RC_START : Extra information providing fields
  UINT8                     ErrorEvent;  // OEM Hook Support (ErrorEvent - 0 SDDC 1 ADDDC 2 RANK SPARING)
  UINT64                    RankAddress;
  UINT8                     OtherMcBankCount;  // Total valid MCBanks
  MCA_BANK_INFO             OtherMcBankInfo[MAX_NUM_MCA_BANKS];  // Total valid MC Banks data
  // APTIOV_SERVER_OVERRIDE_RC_END : Extra information providing fields
} MEMORY_ERROR_SECTION;

typedef struct {
  UINT32                    GFatalErrorStatus;
  UINT32                    GNonFatalErrorStatus;
  UINT32                    GCorrFirstErrorStatus;
  UINT32                    GSysStatus;
  UINT32                    ErrPinStatus;
} IIOGLOBAL_ERROR_STATUS;

typedef struct {
  UINT32                    SocketId;
  IIOGLOBAL_ERROR_STATUS    GlobalStatus;
  UINT32                    IioErrorStatus;
  UINT32                    IioErroCount;
  UINT32                    IioFatalFirstErrStatus;
  UINT32                    IioFatalNextErrStatus;
  UINT32                    IioFatalFirstErrHeader0;
  UINT32                    IioFatalFirstErrHeader1;
  UINT32                    IioFatalFirstErrHeader2;
  UINT32                    IioFatalFirstErrHeader3;
  UINT32                    IioNonFatalFirstErrStatus;
  UINT32                    IioNonFatalNextErrStatus;
  UINT32                    IioNonFatalErrHeader0;
  UINT32                    IioNonFatalErrHeader1;
  UINT32                    IioNonFatalErrHeader2;
  UINT32                    IioNonFatalErrHeader3;
} IIO_CORE_ERROR_RECORD;

typedef struct {
  UINT32                    SocketId;
  IIOGLOBAL_ERROR_STATUS    GlobalStatus;
  UINT32                    Irp0ErrorStatus;
  UINT32                    Irp0FatalFirstErrorStatus;
  UINT32                    Irp0FatalNextErrorStatus;
  UINT32                    Irp0FatalFirstErrorHeader0;
  UINT32                    Irp0FatalFirstErrorHeader1;
  UINT32                    Irp0FatalFirstErrorHeader2;
  UINT32                    Irp0FatalFirstErrorHeader3;
  UINT32                    Irp0NonFatalFirstErrorStatus;
  UINT32                    Irp0NonFatalNextErrorStatus;
  UINT32                    Irp0NonFatalFirstErrorHeader0;
  UINT32                    Irp0NonFatalFirstErrorHeader1;
  UINT32                    Irp0NonFatalFirstErrorHeader2;
  UINT32                    Irp0NonFatalFirstErrorHeader3;
  UINT32                    Irp1ErrorStatus;
  UINT32                    Irp1FatalFirstErrorStatus;
  UINT32                    Irp1FatalNextErrorStatus;
  UINT32                    Irp1FatalFirstErrorHeader0;
  UINT32                    Irp1FatalFirstErrorHeader1;
  UINT32                    Irp1FatalFirstErrorHeader2;
  UINT32                    Irp1FatalFirstErrorHeader3;
  UINT32                    Irp1NonFatalFirstErrorStatus;
  UINT32                    Irp1NonFatalNextErrorStatus;
  UINT32                    Irp1NonFatalFirstErrorHeader0;
  UINT32                    Irp1NonFatalFirstErrorHeader1;
  UINT32                    Irp1NonFatalFirstErrorHeader2;
  UINT32                    Irp1NonFatalFirstErrorHeader3;
} IIO_COH_ERROR_RECORD;

typedef struct {
  UINT32                    SocketId;
  IIOGLOBAL_ERROR_STATUS    GlobalStatus;
  UINT32                    MiscErrorStatus;
  UINT32                    MiscErrorCount;
  UINT32                    MiscFatalFirstErrorStatus;
  UINT32                    MiscFatalNextErrorStatus;
  UINT32                    MiscFatalFirstErrorHeader0;
  UINT32                    MiscFatalFirstErrorHeader1;
  UINT32                    MiscFatalFirstErrorHeader2;
  UINT32                    MiscFatalFirstErrorHeader3;
  UINT32                    MiscNonFatalFirstErrorStatus;
  UINT32                    MiscNonFatalNextErrorStatus;
  UINT32                    MiscNonFatalFirstErrorHeader0;
  UINT32                    MiscNonFatalFirstErrorHeader1;
  UINT32                    MiscNonFatalFirstErrorHeader2;
  UINT32                    MiscNonFatalFirstErrorHeader3;
} IIO_MISC_ERROR_RECORD;

typedef struct {
  UINT32                    SocketId;
  IIOGLOBAL_ERROR_STATUS    GlobalStatus;
  UINT32                    DmaDevFunc;
  UINT32                    ChanErrorStatus;
  UINT32                    DmaChanIntErrorStatus;
  UINT32                    DmaChanErrPtr;
} IIO_DMA_ERROR_RECORD;

typedef struct {
  UINT32                    SocketId;
  IIOGLOBAL_ERROR_STATUS    GlobalStatus;
  UINT32                    VtdDevFunc;
  UINT32                    VtdUncErrorStatus;
  UINT32                    VtdUncErrorPtr;
} IIO_VTD_ERROR_RECORD;


typedef union {
  struct {
    UINT16                  Reserved:3;
    UINT16                  SlotNum:13;
  } Bits;
  UINT16                    SlotNum;
} SLOT_NUM;

typedef struct _PCIE_ERROR_SECTION_DEV_ID {
  UINT16                    VendorId;
  UINT16                    DeviceId;
  UINT8                     ClassCode[3];
  UINT8                     FuncNum;
  UINT8                     DevNum;
  UINT16                    SegNum;
  UINT8                     PriBus;
  UINT8                     SecBus;
  SLOT_NUM                  SlotNumber;
  UINT8                     Reserved;
} PCIE_ERROR_SECTION_DEV_ID;

typedef union {
  struct {
    UINT64                  UpperDw:32;
    UINT64                  LowerDw:32;
  } Bits;
  UINT64                    SerialNum;
} PCIE_ERROR_SECTION_DEV_SER;

typedef union {
  struct {
    UINT32                  BrdgSecondarySts:16;
    UINT32                  BrdgCntl:16;
  } Bits;
  UINT32                    BridgeCntl;
} PCIE_ERROR_BRIDGE_STS_CNTL;

typedef struct {
  UINT64                      ValidBits;
  UINT32                      PortType;
  UINT32                      Version;
  UINT32                      CommandStatus;
  UINT32                      Reserved;
  PCIE_ERROR_SECTION_DEV_ID   DeviceId;
  PCIE_ERROR_SECTION_DEV_SER  DeviceSerial;
  PCIE_ERROR_BRIDGE_STS_CNTL  BridgeControlSts;
  UINT8                       CapStruct[60];
  UINT8                       AerInfo[96];
  UINT32                      Uncerrsts;
  UINT32                      Corerrsts;
  UINT32                      Rperrsts;
} PCIE_ERROR_SECTION;


typedef struct {
  UINT32                    NumMcaBanks;
  MCA_BANK_INFO             McaBankInfo[MAX_NUM_MCA_BANKS];  
} MCABANK_ERROR_RECORD;

//
//  RAS Error Record Structure – Contains all information needed to log each error type
//
typedef struct _UEFI_PROCESSOR_ERROR_RECORD {
  UEFI_ERROR_RECORD_HEADER          Header;
  UEFI_ERROR_SECT_DESC              Descriptor;
  PROCESSOR_GENERIC_ERROR_SECTION   Section;
} UEFI_PROCESSOR_ERROR_RECORD;

typedef struct _UEFI_MEM_ERROR_RECORD {
  UEFI_ERROR_RECORD_HEADER          Header;
  UEFI_ERROR_SECT_DESC              Descriptor;
  MEMORY_ERROR_SECTION              Section;
} UEFI_MEM_ERROR_RECORD;

typedef struct _UEFI_PCIE_ERROR_RECORD {
  UEFI_ERROR_RECORD_HEADER          Header;
  UEFI_ERROR_SECT_DESC              Descriptor;
  PCIE_ERROR_SECTION                Section;
} UEFI_PCIE_ERROR_RECORD;

typedef struct _EFI_RAS_ERROR_RECORDS {
  // Core errors
  UEFI_PROCESSOR_ERROR_RECORD   ProcessorErrorRecord;
  UEFI_MEM_ERROR_RECORD         MemoryErrorRecord;
  // Pcie Errors
  UEFI_PCIE_ERROR_RECORD        PcieErrorRecord;
} EFI_RAS_ERROR_RECORDS;

typedef struct _COMMON_PLATFORM_ERROR_RECORD {
  UEFI_ERROR_RECORD_HEADER  Header;
  UEFI_ERROR_SECT_DESC      Descriptor;
  union _SECTION {
    PROCESSOR_GENERIC_ERROR_SECTION   Proc;
    MEMORY_ERROR_SECTION              Mem;
    UEFI_PCIE_ERROR_RECORD            Pcie;
  } Section;
} COMMON_PLATFORM_ERROR_RECORD;

#pragma pack()

#endif
