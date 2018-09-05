//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file NFIT.h

--*/

#ifndef _NFIT_H_
#define _NFIT_H_
// APTIOV_SERVER_RC_OVERRIDE_START : To resolve build error that occurs due to updation of MdePkg_13
#include <IndustryStandard/Acpi61.h>
// APTIOV_SERVER_RC_OVERRIDE_START : To resolve build error that occurs due to updation of MdePkg_13

#define NFIT_HEADER                         EFI_ACPI_6_0_NVDIMM_FIRMWARE_INTERFACE_TABLE
#define NFIT_TABLE_HDR                      EFI_ACPI_6_0_NFIT_STRUCTURE_HEADER

#define NVDIMM_FW_INTERFACE_TABLE_SIGNATURE EFI_ACPI_6_0_NVDIMM_FIRMWARE_INTERFACE_TABLE_STRUCTURE_SIGNATURE
#define NVDIMM_FW_INTERFACE_TABLE_REVISION  EFI_ACPI_6_0_NVDIMM_FIRMWARE_INTERFACE_TABLE_REVISION

#define VOLATILE_REGION_GUID                EFI_ACPI_6_0_NFIT_GUID_VOLATILE_MEMORY_REGION
#define PMEM_REGION_GUID                    EFI_ACPI_6_0_NFIT_GUID_BYTE_ADDRESSABLE_PERSISTENT_MEMORY_REGION
#define CONTROL_REGION_GUID                 EFI_ACPI_6_0_NFIT_GUID_NVDIMM_CONTROL_REGION
#define BLOCK_DATA_WINDOW_REGION_GUID       EFI_ACPI_6_0_NFIT_GUID_NVDIMM_BLOCK_DATA_WINDOW_REGION
#define VOLATILE_VIRTUAL_DISK_REGION_GUID   EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_VOLATILE
#define VOLATILE_VIRTUAL_CD_REGION_GUID     EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_VOLATILE
#define PMEM_VIRTUAL_DISK_REGION_GUID       EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_PERSISTENT
#define PMEM_VIRTUAL_CD_REGION_GUID         EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_PERSISTENT

#define MAX_LINES                   64
#define MAX_SADS                    64
#define INT_TBL_HDR_SIZE            16
#define BITS_64MB_TO_BYTES          26

// Max Nfit size is set so that the total Nfit is slightly below 4k (as the header takes some).
#define MAX_NFIT_SIZE               0x18000

// Defines for Descriptor 'Type's
#define TYPE_SPA_RANGE_DESCRIPTOR               EFI_ACPI_6_0_NFIT_SYSTEM_PHYSICAL_ADDRESS_RANGE_STRUCTURE_TYPE
#define TYPE_MEMDEV_TO_SPA_RANGE_MAP_DESCRIPTOR EFI_ACPI_6_0_NFIT_MEMORY_DEVICE_TO_SYSTEM_ADDRESS_RANGE_MAP_STRUCTURE_TYPE
#define TYPE_INTERLEAVE_DESCRIPTOR              EFI_ACPI_6_0_NFIT_INTERLEAVE_STRUCTURE_TYPE
#define TYPE_SMBIOS_MGMT_INFO_DESCRIPTOR        EFI_ACPI_6_0_NFIT_SMBIOS_MANAGEMENT_INFORMATION_STRUCTURE_TYPE
#define TYPE_CNTRL_REGION_DESCRIPTOR            EFI_ACPI_6_0_NFIT_NVDIMM_CONTROL_REGION_STRUCTURE_TYPE
#define TYPE_BLK_WINDOW_DESCRIPTOR              EFI_ACPI_6_0_NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE_TYPE
#define TYPE_FLUSH_HINT_DESCRIPTOR              EFI_ACPI_6_0_NFIT_FLUSH_HINT_ADDRESS_STRUCTURE_TYPE

// There could be at most 2 Control Region Description Tables per channel (for BLOCK and PMEM)
#define MAX_CTRL_RGNS_PER_CHANNEL   2

// Control region size difference, when Number of Block Control Windows is 0
#define CONTROL_REGION_SIZE_DIFF_NO_BLOCK_WINDOWS   48

// Control Region's DPA in dimm is 0.
#define CTRL_REG_DPA_OFFSET                     0

#define NUM_BLOCK_CTRL_WINDOWS                  256
// Block Control Window Size is 8kb
#define BLOCK_CTRL_WIN_SIZE                     0x2000
// Offset from the start of the CTRL region to the start of the BIOS mailbox
#define BLK_CONTROL_OFFSET                      0x800000
// Status Register Offset is 4k from the start of the Control Region
#define BLK_CONTROL_STATUS_REG_OFFSET           0x801000
#define STATUS_REG_SIZE                         4
// Start of the Block Data Window (DPA) = 128MB
#define BLK_WINDOW_REGION_OFFSET                0x8000000

//
// Memory Device To System Physical Address Range Mapping Table
// NfitDevHandle:
// Bit [3:0]   - Dimm number,
// Bit [7:4]   - Channel #,
// Bit [11:8]  - iMC iD,
// Bit [15:12] - Socket ID,
// Bit [27:16] - Node ID
// Bit [31:28] - reserved
#define NFIT_DEVICE_HANDLE              EFI_ACPI_6_0_NFIT_DEVICE_HANDLE

//
// System Physical Address Range Description Table struct
//
// Flags field definitions:
// Bit [0] set to 1 indicates that Control region is strictly
//   for management during hot add/online operation.
// Bit [1] set to 1 to indicate that data in Proximity
//   Domain field is valid.
// Bits [15:2] : Reserved
// Flag bit field definitions for SPA table
#define CNTRL_RGN_FOR_HOT_ADD           EFI_ACPI_6_0_NFIT_SYSTEM_PHYSICAL_ADDRESS_RANGE_FLAGS_CONTROL_REGION_FOR_MANAGEMENT
#define PROX_DOMAIN_VALID               EFI_ACPI_6_0_NFIT_SYSTEM_PHYSICAL_ADDRESS_RANGE_FLAGS_PROXIMITY_DOMAIN_VALID

#define SPA_RANGE_DESC_TABLE            EFI_ACPI_6_0_NFIT_SYSTEM_PHYSICAL_ADDRESS_RANGE_STRUCTURE

// Flag bit field definitions used for MEMDEV table
#define PREVIOUS_SAVE_FAIL              EFI_ACPI_6_0_NFIT_MEMORY_DEVICE_STATE_FLAGS_PREVIOUS_SAVE_FAIL
#define PLATFORM_FLUSH_FAIL             EFI_ACPI_6_0_NFIT_MEMORY_DEVICE_STATE_FLAGS_PLATFORM_FLUSH_FAIL
#define SMART_EVENTS_OBSERVED           EFI_ACPI_6_0_NFIT_MEMORY_DEVICE_STATE_FLAGS_SMART_HEALTH_EVENTS_PRIOR_OSPM_HAND_OFF

#define MEMDEV_TO_SPA_RANGE_MAP_TABLE   EFI_ACPI_6_0_NFIT_MEMORY_DEVICE_TO_SYSTEM_ADDRESS_RANGE_MAP_STRUCTURE

#define INTERLEAVE_DESC_TABLE           EFI_ACPI_6_0_NFIT_INTERLEAVE_STRUCTURE
#define SMBIOS_MGMT_INFO_TABLE          EFI_ACPI_6_0_NFIT_SMBIOS_MANAGEMENT_INFORMATION_STRUCTURE
#define BLK_WINDOW_DESC_TABLE           EFI_ACPI_6_0_NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE
#define FLUSH_HINT_ADDR_TABLE           EFI_ACPI_6_0_NFIT_FLUSH_HINT_ADDRESS_STRUCTURE

/*
// APTIOV_SERVER_RC_OVERRIDE_START : Commented to resolve build error that occurs due to updation of MdePkg_13
//
// Definition for NVDIMM Control Region Structure from ACPI6.1 industry standard include
//

// -------------------------------------------------------------
// This will be removed when IndustryStandard/Acpi61.h is available
// -------------------------------------------------------------
#define EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_VALID_FIELDS_MANUFACTURING           BIT0
#define EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_FLAGS_BLOCK_DATA_WINDOWS_BUFFERED    BIT0

#pragma pack(1)

typedef struct {
  UINT16                                      Type;
  UINT16                                      Length;
  UINT16                                      NVDIMMControlRegionStructureIndex;
  UINT16                                      VendorID;
  UINT16                                      DeviceID;
  UINT16                                      RevisionID;
  UINT16                                      SubsystemVendorID;
  UINT16                                      SubsystemDeviceID;
  UINT16                                      SubsystemRevisionID;
  UINT8                                       ValidFields;
  UINT8                                       ManufacturingLocation;
  UINT16                                      ManufacturingDate;
  UINT8                                       Reserved_22[2];
  UINT32                                      SerialNumber;
  UINT16                                      RegionFormatInterfaceCode;
  UINT16                                      NumberOfBlockControlWindows;
  UINT64                                      SizeOfBlockControlWindow;
  UINT64                                      CommandRegisterOffsetInBlockControlWindow;
  UINT64                                      SizeOfCommandRegisterInBlockControlWindows;
  UINT64                                      StatusRegisterOffsetInBlockControlWindow;
  UINT64                                      SizeOfStatusRegisterInBlockControlWindows;
  UINT16                                      NVDIMMControlRegionFlag;
  UINT8                                       Reserved_74[6];
} EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_STRUCTURE;
// -------------------------------------------------------------
// APTIOV_SERVER_RC_OVERRIDE_END : Commented to resolve build error that occurs due to updation of MdePkg_13
*/

#define MANUFACTURING_VALID_FIELDS  EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_VALID_FIELDS_MANUFACTURING
#define BLOCK_DATA_WINDOWS_BUFFERED EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_FLAGS_BLOCK_DATA_WINDOWS_BUFFERED

#define IFC_STORAGE_MODE            0x0201
#define IFC_APPDIRECT_MODE          0x0301
#define IFC_AUTO                    0x0

#define CTRL_REGION_TABLE           EFI_ACPI_6_1_NFIT_NVDIMM_CONTROL_REGION_STRUCTURE


//
// NVDIMM Firmware Interface Table struct
// IMPORTANT NOTE: Num of various sub-table fields of the Nfit has been eliminated in the latest Nfit Spec (0.8k)
// And also since there is NO memory allocation scheme for Nfit (all globals), Adding a buffer NfitTables with the,
// max nfit size currently set to 4k. Will need to watch if we will ever overrun this buffer. It is expected that
// Nfit parser will need to parse the NfitTables field to understand the various other Nfit Tables in this buffer
// based on the Type and Length field of these tables.
// A BIG NOTE: When ever the NVDIMM_FW_INTERFACE_TABLE defined either here or what is in Nfit.h of PlatPkg/Include/Acpi
// changes, the same changes need to relect in both the files.
//
typedef struct _NVDIMM_FW_INT_TBL {
  UINT32       Signature;                    // 'NFIT' should be the signature for this table
  UINT32       Length;                       // Length in bytes fir the entire table
  UINT8        Revision;                     // Revision # of this table, initial is '1'
  UINT8        Checksum;                     // Entire Table Checksum must sum to 0
  UINT8        OemID[6];                     // OemID
  UINT8        OemTblID[8];                  // Should be Manufacturer's Model #
  UINT32       OemRevision;                  // Oem Revision of for Supplied OEM Table ID
  UINT32       CreatorID;                    // Vendor ID of the utility that is creating this table
  UINT32       CreatorRev;                   // Revision of utility that is creating this table
  UINT32       Reserved;
  UINT8        NfitTables[MAX_NFIT_SIZE];    // See Description above (IMPORTANT).
} NVDIMM_FW_INTERFACE_TABLE, *PNVDIMM_FW_INTERFACE_TABLE;

#pragma pack()

NFIT_DEVICE_HANDLE
BuildNfitDevHandle(
  UINT8 Socket,
  UINT8 Ch
  );

EFI_STATUS
BuildNfitTable(
  UINT64 *CrystalRidgeTablePtr,
  UINTN  CrystalRidgeTableSize
  );

EFI_STATUS
UpdateNfitTableChecksum(
  UINT64 *CrystalRidgeTablePtr
  );

#endif  // _NFIT_H_

