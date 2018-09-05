//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
   WHEA releated definition for Platform drivers and data tables.

  Copyright (c) 2005 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_PLATFORM_DEFS_H_
#define _WHEA_PLATFORM_DEFS_H_

//
// Includes
//
#include <IndustryStandard/WheaDefs.h>
#include <IndustryStandard/WheaAcpi.h>

//
// System Error (Source) types based on type and sevirity
//
#define GEN_ERR_SEV_CPU_GENERIC_RECOVERABLE         0x0000      // General Processor error servirity types
#define GEN_ERR_SEV_CPU_GENERIC_FATAL               0x0001
#define GEN_ERR_SEV_CPU_GENERIC_CORRECTED           0x0002
#define GEN_ERR_SEV_CPU_GENERIC_INFORMATION         0x0003
#define GEN_ERR_SEV_CPU_SPECIFIC_RECOVERABLE        0x0008      // Processor specific error servirity types
#define GEN_ERR_SEV_CPU_SPECIFIC_FATAL              0x0009
#define GEN_ERR_SEV_CPU_SPECIFIC_CORRECTED          0x000A
#define GEN_ERR_SEV_CPU_SPECIFIC_INFORMATION        0x000B
#define GEN_ERR_SEV_PLATFORM_MEMORY_RECOVERABLE     0x0010      // Platform memory error servirity types
#define GEN_ERR_SEV_PLATFORM_MEMORY_FATAL           0x0011
#define GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED       0x0012
#define GEN_ERR_SEV_PLATFORM_MEMORY_INFORMATION     0x0013
#define GEN_ERR_SEV_PCIE_RECOVERABLE                0x0018      // PCIe (root port, device & bridge) error servirity types
#define GEN_ERR_SEV_PCIE_FATAL                      0x0019
#define GEN_ERR_SEV_PCIE_CORRECTED                  0x001A
#define GEN_ERR_SEV_PCIE_INFORMATION                0x001B
#define GEN_ERR_SEV_PCI_BUS_RECOVERABLE             0x0020      // PCI Bus error servirity types
#define GEN_ERR_SEV_PCI_BUS_FATAL                   0x0021
#define GEN_ERR_SEV_PCI_BUS_CORRECTED               0x0022
#define GEN_ERR_SEV_PCI_BUS_INFORMATION             0x0023
#define GEN_ERR_SEV_PCI_DEV_RECOVERABLE             0x0028      // PCI device error servirity types
#define GEN_ERR_SEV_PCI_DEV_FATAL                   0x0029
#define GEN_ERR_SEV_PCI_DEV_CORRECTED               0x002A
#define GEN_ERR_SEV_PCI_DEV_INFORMATION             0x002B
#define GEN_ERR_SEV_IOH_CORRECTED                   0x002C      // IOH corrected error servirity types
#define GEN_ERR_SEV_IOH_RECOVERABLE                 0x002D      // IOH recoverable error servirity types
#define GEN_ERR_SEV_IOH_FATAL                       0x002F      // IOH fatal error servirity types
#define GEN_ERR_SEV_NON_STANDARD_DEV_CORRECTED      0x0030
#define GEN_ERR_SEV_NON_STANDARD_DEV_RECOVERABLE    0x0031
#define GEN_ERR_SEV_NON_STANDARD_DEV_FATAL          0x0032

//
// Protocol Data Definitions
//
//
#define PCIE_SPECIFICATION_SUPPORTED    0x0110      // 1.1
#define EFI_WHEA_ENABLE_SWSMI           0x9E
#define EFI_WHEA_DISABLE_SWSMI          0x9D

#pragma pack(1)

//
// Functions & Data structures
//

typedef struct {
  WHEA_ERROR_TYPE                       Type;
  UINT16                                SourceId;
  UINT32                                Flags;
  UINT32                                NumOfRecords;
  UINT32                                MaxSecPerRecord;
  VOID                                  *SourceData;
} SYSTEM_NATIVE_ERROR_SOURCE;

typedef struct {
  UINT16                                Type;
  UINT16                                SourceId;
  UINT32                                Flags;
  UINT32                                NumOfRecords;
  UINT32                                MaxSecPerRecord;
  VOID                                  *SourceData;
} SYSTEM_GENERIC_ERROR_SOURCE;

//
// Memory Error sub-type definitions
//
//
#define MEM_ERROR                       0xf0
#define MEM_LINK_ERROR                  0x01
#define MEM_LANE_FAILOVER               0x02
#define MEM_DDR_PARITY_ERROR            0x03
#define MEM_CRC_ERROR                   0x04
#define MEM_ECC_ERROR                   0x05
#define MEM_MIRROR_FAILOVER             0x06
#define MEM_PATROL_SCRUB_ERROR          0x07

#define EFI_WHEA_ENABLE_SWSMI           0x9E
#define EFI_WHEA_DISABLE_SWSMI          0x9D

typedef struct {
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to use AMI RuntimeError (RtErrorLogBMC/Whea) Listener code.
    //
    // Header-like information...
    //
    UINT8   Source;
    UINT8	Type;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to use AMI RuntimeError (RtErrorLogBMC/Whea) Listener code.
    UINT64  ValidBits;
    UINT16   UefiErrorRecordRevision;
    UINT16  ErrorType;
    UINT16  Node;
    UINT16  Branch;
    UINT16  Channel;
    UINT16  VmseCh;
    UINT16  Dimm;
    UINT16  Device;
    UINT16  Bank;
    UINT16  Rank;
    UINT32  Row;
// APTIOV_SERVER_OVERRIDE_RC_START
    UINT16  Column;
    UINT64  RankAddress;
    UINT8   Extended;
// APTIOV_SERVER_OVERRIDE_RC_END
    UINT8   BankGroup;
    UINT8   ChipSelect;
    UINT64  PhyAddr;
// APTIOV_SERVER_OVERRIDE_RC_START : Support added for Mode1 Memory Error Reporting
    UINT64  PhyAddrMask;
// APTIOV_SERVER_OVERRIDE_RC_END : Support added for Mode1 Memory Error Reporting
    UINT16  SmBiosCardHandle;    // Type 16 handle;
    UINT16  SmBiosModuleHandle;	 // Type 17 handle;
// APTIOV_SERVER_OVERRIDE_RC_START : Support added for Mode1 Memory Error Reporting
    UINT16  BitPosition;
// APTIOV_SERVER_OVERRIDE_RC_END : Support added for Mode1 Memory Error Reporting
} MEMORY_DEV_INFO;

typedef struct {
    UINT8   Segment;
    UINT8   Bus;
    UINT8   Device;
    UINT8   Function;
} PCIE_PCI_DEV_INFO;

typedef struct {
    UINT8   Valid : 1;
    UINT8   Reserved : 7;
    UINT8   ComponentType;  // Bit Definition : 0-Generic, 1-Internal, 2-Cache, 3-QPI Links
    UINT8   ComponentData;  // Link Number
}ADDITIONAL_CPU_INFO;

typedef struct {
    UINT8                 ProcessorType;
    UINT8                 ProcessorISA;
    UINT8                 ProcessorErrorType;
    UINT8                 Operation;
    UINT8                 Flags;
    UINT8                 Level;
    UINT16                Reserved;
    UINT64                VersionInfo;
    UINT64                BrandString;
    UINT8                 ProcessorApicId;
    UINT64                TargetAddress;
    UINT64                RequestorIdentifier;
    UINT64                ResponderIdentifier;
    UINT64                InstructionIp;
    UINT32                ProcessorId;
    ADDITIONAL_CPU_INFO   AdditionalInfo;
} CPU_DEV_INFO;

typedef enum {
  PSF0 = 1,
  PSF1,
  PSF2,
  PSF3,
  IEH,
  BUNIT
}PORT_TYPE;

typedef struct {
    UINT16          ErrorType;
	PORT_TYPE       PortType;
} NON_STANDARD_DEV_ERROR_INFO;

#define PLATFORM_NON_STANDARD_DEV_ERROR_PSF_CMD_PARITY      0x00
#define PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY      0x01
#define PLATFORM_NON_STANDARD_DEV_ERROR_IEH_DATA_PARITY     0x02
#define PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CFG_PARITY      0x03
#define PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_BRAM_PARITY   0x04
#define PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_PCC           0x05
#define PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_UNC           0x06



#pragma pack()


#endif  // _WHEA_PLATFORM_DEFS_H_

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2007 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

-----------------------------------------------------------------------------*/
