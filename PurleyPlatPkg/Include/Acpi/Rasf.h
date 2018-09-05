//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

  Copyright (c) 2013 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval 
  system, or transmitted in any form or by any means without the express 
  written consent of Intel Corporation.
 
Abstract:

  This file describes the contents of the ACPI RAS Feature Table (RASF)
  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h
  All changes to the RASF contents should be done in this file.

--*/

#ifndef _RASF_H_
#define _RASF_H_

//
// Include files
//
// APTIOV_SERVER_OVERRIDE_RC_START : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files 
#include <AmiProtocol.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Included to avoid EFI_ACPI_TABLE_VERSION_ALL macro inclusion for RC files
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SmBios.h>
#include "Platform.h"


//
// ACPI table information used to initialize tables.
//
#ifndef EFI_ACPI_OEM_ID
#define EFI_ACPI_OEM_ID           'I', 'N', 'T', 'E', 'L', ' '  // OEMID 6 bytes long
#endif
#ifndef EFI_ACPI_OEM_TABLE_ID
#define EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64 ('I', 'N', 'T', 'E', 'L', 'C', 'R', 'B') // OEM table id 8 bytes long
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

//
// RASF definitions
//
#define EFI_ACPI_RAS_FEATURE_TABLE_REVISION             0x00000001  // confirm in the ACPI spec
#define EFI_ACPI_OEM_RASF_REVISION                      0x00000001


#define EFI_ACPI_RASF_PCC_IDENTIFIER                      0x01

#define RASF_SWSMI_VALUE                                  0xFD

#pragma pack (1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                 Header;
  UINT8                                       RasfPccIdentifier[12];
 } EFI_ACPI_RAS_FEATURE_TABLE;


//
// RASF shared memory region 
//

#define EFI_ACPI_RASF_PCC_SIGNATIRE               0x52415346
#define EFI_ACPI_RASF_PCC_COMMAND                 0x1
#define EFI_ACPI_RASF_PCC_VERSION                 0x1
#define EFI_ACPI_RASF_PCC_PARAMETER_BLOCKS_NUM    6

#define RASF_MAX_SPARES                           0x10
// RAS Capabilities
#define HARDWARE_PATROL_SCRUB                       BIT0
#define HARDWARE_PATROL_SCRUB_EXPOSED_TO_SOFTWARE   BIT1
#define GET_DEVICE_CAPABILITY                       BIT2
#define DEVICE_ONLINE                               BIT3
#define DEVICE_OFFLINE                              BIT4
#define MEMORY_MIGRATION                            BIT5
#define GET_SPARES                                  BIT6
#define PARTIAL_MIRROR                              BIT7


// Parameter Block Types
#define RASF_TYPE_PATROL_SCRUB                    0x00
#define RASF_TYPE_PARTIAL_MIRRORING               0x01
#define RASF_TYPE_GET_DEVICE_CAPABILITY           0x02
#define RASF_TYPE_DEVICE_ONLINE                   0x03
#define RASF_TYPE_DEVICE_OFFLINE                  0x04
#define RASF_TYPE_MIGRATE_MEMORY                  0x05
#define RASF_TYPE_GET_SPARES                      0x06


// Device Capabilities
#define DEVICE_ONLINE_OFFLINE_CAPABLE             BIT0
#define DEVICE_HOT_PLUG_CAPABLE                   BIT1
#define DEVICE_POWER_ON_OFF_CAPABLE               BIT2
#define DEVICE_MIG_HARDWARE_NONBLOCKING           BIT4
#define DEVICE_MIG_HARDWARE_BLOCKING              BIT5
#define MODULE_DEVICE_MIGRATION_CAPABLE           BIT4 | BIT5
#define DEVICE_SPARE_ENABLE                       BIT6
#define DEVICE_PARTIAL_MIRROR_CAPABLE             BIT7

//Partial Mirroring Commands
#define GET_CURRENT_MIRROR                        0x01
#define GET_POSSIBLE_MIRROR_SIZES                 0x02
#define SET_MIRROR_ON_REBOOT                      0x03
#define MIRROR_BELOW_4G_RANGE                     0x04

typedef union {
   struct{
	    UINT16 CmdComplete:1;
	    UINT16 SciDoorBell:1;
	    UINT16 Error:1;
	    UINT16 Status:5;
	    UINT16 Rsvd:8;
   } Bits;
   UINT16 Data;
} PCC_STS_FIELD;

typedef union {
   struct{
	    UINT16 Cmd:8;
	    UINT16 Rsvd:7;
	    UINT16 GenerateSci:1;
   } Bits;
   UINT16 Data;
} PCC_CMD_FIELD;

typedef struct {
  UINT32        UId;
  UINT16        Type;
} RESOURCE_ID;

typedef struct {
  UINT16        Type;
  UINT32        CapabilityFlags;
  UINT32        UId;
} DEVICE_CAPABILITY_BLOCK;

typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        ResourceType;
  UINT32        ResourceUId;
  DEVICE_CAPABILITY_BLOCK     DeviceCapabilityBlock;
  } GET_DEVICE_CAPABILITY_PARAMETER_BLOCK;

typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        ResourceType;
  UINT32        ResourceUId;
  UINT32        OutgoingResourceUId;
  UINT32        Rsvd;
  } ONLINE_PARAMETER_BLOCK;

typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        ResourceType;
  UINT32        ResourceUId;
  UINT32        Rsvd;
  } OFFLINE_PARAMETER_BLOCK;

typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        ResourceType;
  UINT32        OutgoingResourceUId;
  UINT32        SpareResourceUId;
  } MIGRATE_MEMORY_PARAMETER_BLOCK;


//
// Mirror Range Structure.
//
typedef struct {
  UINT16        Flags;
  UINT16        Version;
  UINT32        Reserved;
  UINT64        MirrorStartAddress;
  UINT64        MirrorEndAddress;
  } MIRROR_RANGE_STRUCT_BLOCK;

//
// Mirror Size Info Structure.
//
typedef struct {
  UINT16        Flags;
  UINT16        Status;
  UINT16        Version;
  UINT32        Size;
  UINT32        ApicIds;
  } MIRROR_SIZE_STRUCT_BLOCK;

//
// Block Structure for Partial Mirroring.
//
typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        Flags;
  UINT16        MaxMirroringRanges; //Need to find the Maximum number.
  UINT16        PartialAddressMirroringCommand;
  UINT16        Status;
  MIRROR_RANGE_STRUCT_BLOCK     MirrorRangeInfo[ MC_MAX_NODE * (sizeof(MIRROR_RANGE_STRUCT_BLOCK))];
  MIRROR_SIZE_STRUCT_BLOCK     MirrorSizeInfo[MC_MAX_NODE * (sizeof(MIRROR_SIZE_STRUCT_BLOCK))];
  MIRROR_SIZE_STRUCT_BLOCK     OSMirrorRangeRequest[MC_MAX_NODE * (sizeof(MIRROR_SIZE_STRUCT_BLOCK))];
  } PARTIAL_MIRRORING_PARAMETER_BLOCK;
typedef struct {
  UINT16        Type;
  UINT16        Version;
  UINT16        Length;
  UINT16        ResourceType;
  UINT32        ResourceUId;
  UINT32        NumberOfSpares;
  RESOURCE_ID   SparesList[RASF_MAX_SPARES];
  } GET_SPARES_PARAMETER_BLOCK;

typedef struct {
  UINT32                                  Signature;
  PCC_CMD_FIELD                           Command;
  PCC_STS_FIELD                           Status;
  UINT16                                  Version;
  UINT64                                  RasCapabilities[2];
  UINT64                                  SetRasCapabilities[2];
  UINT16                                  NumOfRasfParameterBlocks;
  UINT32                                  RasCapabilitiesStatus;
  GET_DEVICE_CAPABILITY_PARAMETER_BLOCK   GetDeviceCapabilityBlock;
  ONLINE_PARAMETER_BLOCK                  OnlineCapabililtyBlock;
  OFFLINE_PARAMETER_BLOCK                 OfflineCapabilityBlock;
  MIGRATE_MEMORY_PARAMETER_BLOCK          MigrateMemoryCapabilityBlock;
  PARTIAL_MIRRORING_PARAMETER_BLOCK       PartialMirroringParamBlock;
  GET_SPARES_PARAMETER_BLOCK              GetSparesParameterBlock;
  } EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF;

#pragma pack ()

#endif
//End of File

