/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014-15 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  Pcat.h

@brief:

  Definitions of ACPI PCAT (Platform Configuration Attributes Table)
  structures and values

**/

#ifndef _ACPI_PCAT_H_
#define _ACPI_PCAT_H_

#include <Uefi/UefiBaseType.h>

/// @brief PCAT Signature to put in table header
#define NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE  SIGNATURE_32('P', 'C', 'A', 'T')
#define NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_REVISION   0x01
/// @brief Table type for SSKU Attribute Extension Table
#define SSKU_ATTRIBUTE_EXTENSION_TABLE_TYPE         0x03
/// @brief Vendor ID for SSKU Attribute Extension Table
#define SSKU_ATTRIBUTE_EXTENSION_TABLE_VENDOR_ID    0x8086
// Allocated Max size for Platform Config Attribute Table
#define MAX_PCAT_SIZE                               0x200
// Maximum Interleave list seen so far is as below
#define MAX_INT_LIST                                4
// Interleave Capability Header Size
#define INTERLEAVE_CAP_HEADER_SIZE                  12
// Maximum size of all the PCAT Tables = MAX_PCAT_SIZE - PCAT Header size (40 bytes)
#define PCAT_MAX_TBL_SIZE                           472
/// Platform Capabilities Info Table type
#define PLAT_CAP_INFO_TABLE_TYPE                    0

#define PLAT_CAP_MEM_MODE_CAP_1LM       BIT0
#define PLAT_CAP_MEM_MODE_CAP_2LM       BIT1
#define PLAT_CAP_MEM_MODE_CAP_PMEM      BIT2
#define PLAT_CAP_MEM_MODE_CAP_BLK       BIT4
#define PLAT_CAP_MEM_MODE_CAP_SNC       BIT5

#define PLAT_CAP_CURRENT_VOL_MEM_MODE_1LM        0
#define PLAT_CAP_CURRENT_VOL_MEM_MODE_2LM        1
#define PLAT_CAP_CURRENT_VOL_MEM_MODE_AUTO       2
#define PLAT_CAP_CURRENT_VOL_MEM_MODE_RESERVED   3

#define PLAT_CAP_CURRENT_PER_MEM_MODE_DISABLED   0
#define PLAT_CAP_CURRENT_PER_MEM_MODE_PMEM       1
#define PLAT_CAP_CURRNET_PER_MEM_MODE_RESERVED   2
#define PLAT_CAP_CURRENT_PER_MEM_MODE_RESERVED1  3 
#define PLAT_CAP_CURRENT_MEM_MODE_SNC            BIT7

#define CH_INT_SIZE_64B             BIT0
#define CH_INT_SIZE_128B            BIT1
#define CH_INT_SIZE_256B            BIT2
#define CH_INT_SIZE_4KB             BIT6
#define IMC_INT_SIZE_64B            BIT8
#define IMC_INT_SIZE_128B           BIT9
#define IMC_INT_SIZE_256B           BIT10
#define IMC_INT_SIZE_4KB            BIT14
#define CH_INT_1CH_WAYS             BIT16                 
#define CH_INT_2CH_WAYS             BIT17                 
#define CH_INT_3CH_WAYS             BIT18                 
#define CH_INT_4CH_WAYS             BIT19                 
#define CH_INT_6CH_WAYS             BIT20                 
#define CH_INT_8CH_WAYS             BIT21                 
#define CH_INT_12CH_WAYS            BIT22                
#define CH_INT_16CH_WAYS            BIT23                 
#define CH_INT_24CH_WAYS            BIT24                 
#define SOC_INT_6_WAY               CH_INT_1CH_WAYS | CH_INT_2CH_WAYS | CH_INT_3CH_WAYS | CH_INT_4CH_WAYS | CH_INT_6CH_WAYS
#define SOC_INT_4_WAY               CH_INT_1CH_WAYS | CH_INT_2CH_WAYS | CH_INT_4CH_WAYS 
// Bit field for Recommeded Interleave Format
#define RECOMMENDED_INTERLEAVE_FORMAT               BIT31
#define TYPE_INTERLEAVE_CAPABILITY_DESCRIPTOR       1


#pragma pack(push, 1)

//
// Memory Interleave Capability Information struct - Per Aep Dimm
// Note:
//  IntAlignSize: Interleave alignment size in 2^n bytes, n = 26 for 64MB, n = 27 for 128MB
//  IntFormats:
//  Byte0: Channel Interleave Size
//    Bits[7:0] - Bit0 - 64B, Bit1 - 128B, Bit2 - 256B, Bit6 - 4KB , Bit7 - 1GB, others bits are reserved
//  Byte1: iMC Interleave Size
//    Bits[7:0] - Bit0 - 64B, Bit1 -128B, Bit2 - 256B, Bit6 - 4KB, Bit7 - 1Gb, other bits are reserved
//  Byte2-3: Number of channel ways, Recommended Interleave Format
//    Bits[14:0] - Bit0 - 1way, Bit1 - 2way, Bit2 - 3way, Bit3 - 4way, Bit4 - 6way, Bit5 - 8way, Bit6 - 12way, Bit7 - 16way, Bit8 - 24way
//    Bits[14:9] - Reserved
//    Bit15 - If clear, the interleave format is supported but not recommended
// 
//               - certain interleave size combinations
//    Bit[15] - If clear, the interleave format is supported but not recommended. If set, the interleave format is recommended.
//
typedef struct _INT_CAP {
  UINT16       Type;                    // Should be 1 for Interleave Capability
  UINT16       Length;                  // Length in bytes of this entire table
  UINT8        MemMode;                 // 0 = 1LM, 1 = 2LM, 3 = PMEM, 4 = PMEM_CACHE, Other values are reserved
  UINT8        Rsvrd[3];
  UINT16       IntAlignSize;            // Described as above:
  UINT16       NumIntFormats;           // Number of Interleave formats supported by BIOS for the above memory mode.
  UINT32       IntFormatList[MAX_INT_LIST]; // Interleave Format List
} INTERLEAVE_CAPABILITIES, *PINTERLEAVE_CAPABILITIES; 

//
// Platform Capability Information Table 
// CRMgmtSWConfigInput
//   Bit0:If set BIOS supports changing configuration thru mgmt software, else otherwise
//   Bit1:If set, BIOS supports runtine interface to validate mgmt configuration change requests
// MemModeCap
//   Bit0:1LM
//   Bit1:2LM
//   Bit2:PMEM
//   Bit3:PMEM_CACHE
//   Bit4:Blk
//   Bit5:SNC
// CurrentMemMode
//   Bits[1:0] - Volatile Memory Mode
//     00b:1LM 
//     01b:2LM
//     10b:Auto (2LM if DDR4+AEP DIMM with volatile mode present, 1LM Otherwise)
//
//   Bits[3:2] - Persistent Memory Mode
//     00b:Disabled
//     01b:App Direct PM Mode
//     10b:App Direct Cached PM Mode (NOT SUPPORTED)
//
// PMEMMirrorCap
//   Bit0:If set, Intel NVM DIMM PMEM mirroring is supported
//   Bit1:If set, Intel NVM DIMM PMEM sparing is supported
//   Bit2:If set, Intel NVM DIMM PMEM migration is supported
//
typedef struct _PLATFORM_CAP {
  UINT16       Type;                    // Should be 0 for Plat Cap Info table
  UINT16       Length;                  // Length in bytes of this entire table
  UINT8        CRMgmtSWConfigInput;     // Defined above
  UINT8        MemModeCap;              // Memory Mode capabilities defined above
  UINT8        CurrentMemMode;          // Current Memory enabled in BIOS
  UINT8        PMEMMirrorCap;           // PMEM Mirroring Capability, defined above
  UINT8        Reserved[8];              // Reserved
} PLAT_CAPABILITIES_INFO, *PPLAT_CAPABILITIES_INFO; 

// A BIG NOTE: When ever the NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE defined either here or 
// what is in Pcat.h in PlatPkg/Include/Acpi changes, the same changes need to relect in both the files.
typedef struct {
  UINT32                      Signature;      // 'PCAT' should be the signature for this table
  UINT32                      Length;         // Length in bytes for the entire table
  UINT8                       Revision;       // Revision # of this table, initial is '1'
  UINT8                       Checksum;       // Entire Table Checksum must sum to 0
  UINT8                       OemID[6];       // OemID
  UINT8                       OemTblID[8];    // Should be Manufacturer's Model #
  UINT32                      OemRevision;    // Oem Revision of for Supplied OEM Table ID
  UINT32                      CreatorID;      // Vendor ID of the utility that is creating this table
  UINT32                      CreatorRev;     // Revision of utility that is creating this table
  UINT32                      Reserved;       // Alignement for size modulo 8 = 0
} NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE, *PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE;
#pragma pack(pop)

/// @brief Layout of SSKU Attribute Extension Table header
#pragma pack(push, 1)
typedef struct {
  UINT16       Type;                          // Type for Mgmt Info Struct, type should be 3
  UINT16       Length;                        // Length in Bytes for Entire SSKU Attribute Extension Table size.
  UINT16       Rsvd; 
  UINT16       VendorID;                      // Vendor ID
  EFI_GUID     Guid;                          // Intel defined SSKU Attribute Extension Table. guid: F93032E5-B045-40ef-91C8-F02B06AD948D
} NVDIMM_SSKU_ATTR_EXT_TABLE, *PNVDIMM_SSKU_ATTR_EXT_TABLE; 
#pragma pack(pop)

/**
  @brief Install protocol to access ACPI PCAT supporting functions
*/
EFI_STATUS
InstallAcpiPcatProtocol (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );


#endif //_ACPI_PCAT_H_

