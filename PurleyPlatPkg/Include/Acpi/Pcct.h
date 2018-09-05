//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

  Copyright (c) 2014 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval 
  system, or transmitted in any form or by any means without the express 
  written consent of Intel Corporation.
 
Abstract:

  This file describes the contents of the ACPI Platform Communications Channel Table
  (PCCT).  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h
  All changes to the PCC contents should be done in this file.

--*/

#ifndef _PCTT_H_
#define _PCTT_H_

//
// Include files
//
// APTIOV_SERVER_OVERRIDE_RC_START : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <AmiProtocol.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SmBios.h>
#include "Platform.h"
#include  <Acpi/Rasf.h>

//
// PCCT Definitions
//
#define EFI_ACPI_3_0_PCCT_SIGNATURE                     0x54434350
#define EFI_ACPI_PCCT_REVISION                          0x00000001

#define EFI_ACPI_PCCT_ID                                0x8086A201      

#define EFI_ACPI_PCC_SIGNATURE                          0x50434300

//
// ACPI table information used to initialize tables.
//
#ifndef EFI_ACPI_OEM_ID
#define EFI_ACPI_OEM_ID           'I', 'N', 'T', 'E', 'L', ' '  // OEMID 6 bytes long
#endif
#ifndef EFI_ACPI_OEM_TABLE_ID
#define EFI_ACPI_OEM_TABLE_ID    SIGNATURE_64 ('P', 'U', 'R', 'L', 'E', 'Y', ' ', ' ') // OEM table id 8 bytes long
#endif
#ifndef EFI_ACPI_OEM_REVISION
#define EFI_ACPI_OEM_REVISION     0x00000003
#endif
#ifndef EFI_ACPI_CREATOR_ID
#define EFI_ACPI_CREATOR_ID       SIGNATURE_32 ('M', 'S', 'F', 'T')
#endif
#ifndef EFI_ACPI_CREATOR_REVISION
#define EFI_ACPI_CREATOR_REVISION 0x0100000D
#endif
#ifndef EFI_ACPI_TABLE_VERSION_ALL
#define EFI_ACPI_TABLE_VERSION_ALL  (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_2_0|EFI_ACPI_TABLE_VERSION_3_0)
#endif
#define PCCT_ACPI_VERSION_SUPPORT   (EFI_ACPI_TABLE_VERSION_ALL)

//
// Memory Power State Comamnd Mailbox Address Information
//
#define EFI_ACPI_COMMAND_MAILBOX_ADDRESS_SPACE_ID     EFI_ACPI_3_0_SYSTEM_IO
#define EFI_ACPI_COMMAND_MAILBOX_BIT_WIDTH            0x20
#define EFI_ACPI_COMMAND_MAILBOX_BIT_OFFSET           0x00
#define EFI_ACPI_COMMAND_MAILBOX_ACCESS_SIZE          EFI_ACPI_3_0_BYTE
#define EFI_ACPI_COMMAND_MAILBOX_ADDRESS              0x81  // Need to change???, Currently using 0x80-0x83 IoPort

//
// ACPI PCC Spec defines 8 instructions to be specified as action entires 
//
#define EFI_ACPI_PCC_SUBSPACE_STRUCTURE_COUNT         1
#define EFI_ACPI_PCC_RASF_SUBSPACE_STRUCTURE_ID       0
// 
// 
#define EFI_ACPI_SCI_DOORBELL_SET               1
#define EFI_ACPI_SCI_DOORBELL_CLEAR             1
// 
#define PCC_SUBSPACE_STRUCTURE_TYPE             00
// 
#define PCC_SHARED_MEM0_CMD_FIELD_GENERATE_SCI              0x8000//BIT15
#define PCC_SHARED_MEM0_STS_FIELD_SCI_DOORBELL              BIT1
#define PCC_SHARED_MEM0_STS_FIELD_ERROR                     BIT2
 
// 
// // To be clarified
#define PCC_SBSTANCE_NOMINAL_LATENCY              00
#define PCC_SBSTANCE_MAX_PERIODIC_ACCESS_RATE     00
#define PCC_SBSTANCE_MIN_REQUEST_TURNAROUND_TIME  00
// 
#pragma pack (1)
 
#define PcctActionItemFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  8,                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0}
 
#define PcctActionItemExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  8,                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}
 
/*
Type	      1	0	0 	PCC Subspace Structure
Length	      1	1	52
Reserved	  6	2	Reserved
Base Address8	8	Base Address of the shared memory range, described below
Length	      8	16	Length of the memory range. Must be > 8.
Doorbell Register	12	24	Contains the processor relative address, represented in Generic Address Structure format, of the PCC doorbell
Note: Only System I/O space and System Memory space are valid for values for Address_Space_ID. 
Doorbell Preserve	8	36	Contains a mask of bits to preserve when writing the doorbell register.
Doorbell Write	8	44	Contains a mask of bits to set when writing the doorbell register.
*/
 
typedef struct {
  UINT8                                   Type;
  UINT8                                   Length;
  UINT8                                   Reserved[6];
  UINT64                                  BaseAddress;
  UINT64                                  MemLength;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  GasDoorbellRegister;
  UINT64                                  DoorbellPreserve;
  UINT64                                  DoorbellWrite;
  UINT32                                  NominalLatency;
  UINT32                                  MaxPeriodicAccrate;
  UINT16                                  MinReqTurnArounfTime;
} EFI_ACPI_PCC_SUBSPACE_STRUCTURE;
 
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT32                                  Flags;
  UINT8                                   Reserved[8];
} EFI_ACPI_PCCT_HEADER;
 
typedef struct {
  EFI_ACPI_PCCT_HEADER                    Header;
  EFI_ACPI_PCC_SUBSPACE_STRUCTURE         PccSubspaceStructure[EFI_ACPI_PCC_SUBSPACE_STRUCTURE_COUNT];
} EFI_ACPI_PLATFORM_COMMUNICATIONS_CHANNEL_TABLE;

#pragma pack ()

#endif

