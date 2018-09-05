//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

  Copyright (c) 2016 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval 
  system, or transmitted in any form or by any means without the express 
  written consent of Intel Corporation.
 
Abstract:

  This file describes the contents of the ACPI Memory Power State Description Table
  (MPST).  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h
  All changes to the MPST contents should be done in this file.

--*/

#ifndef _PMTT_FILE_
#define _PMTT_FILE_

//
// Includes
//
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SmBios.h>
#include "Platform.h"

#pragma pack (1)

//
// PMTT Definitions
//
#define EFI_ACPI_2_0_PMTT_TABLE_SIGNATURE               0x54544D50
#define EFI_ACPI_OEM_PMTT_REVISION                      0x00000001
#define SIZE_OF_PMTT_TABLE                              0x2000

#define EFI_ACPI_PMTT_ID                                0x8086A201

//
// PMTT Revision (defined in spec)
//
#define EFI_ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE_REVISION    0x01

#define PROXMITY_DOMAIN_INFO_NOT_PRSENT  00   

#define EFI_ACPI_TOP_LEVEL_MEMORY_TYPE_SOCKET         00
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_TLAD         01
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_PHYELEMENT   02	// BIT1 - set to 1 since socket is a physical element of the topology
// Bit 2 and 3 - If 00, indicates that all components aggregated by this device implements volatile memory
// If 01, indicates that components aggregated by this device implements both volatile and non-volatile memory
// If 10, indicates that all components aggregated by this device implements non-volatile memory
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_VOLATILE     00
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_MIXED        4
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_NONVOLATILE  0x08
#define EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_RSVD         0x0C

#define EFI_ACPI_TOP_LEVEL_MEMOERY_TYPE_MEM_CTRL_HUB  01
#define PHYSICAL_COMPONENT_IDENTIFIER_TYPE_DIMM       02

#define PMTT_TBI       00

//
// This function will patch the PMTT table 
//
#define MEMCTRL_CHAR        {0, 0, 0, 0, 0, 0}
#define PROX_DOMAIN         {0, 0, 0, 0, 0, 0, 0, 0}

#pragma pack (1)

// PMMT Table fields

typedef struct {

  UINT32                                  ReadLatency;
  UINT32                                  WriteLatency;
  UINT32                                  ReadBW;
  UINT32                                  WriteBW;
  UINT16                                  OptimalAccUnit;
  UINT16                                  OptimalAccAlignment;

} EFI_ACPI_MEMCTRL_CHAR_TABLE;

typedef struct {

  UINT8                                     Type;
  UINT8                                     Rsvd;
  UINT16                                    Length;
  UINT16                                    Flag;
  UINT16                                    Rsvd1;
  UINT16                                    PhyCompIdentifier;
  UINT16                                    Rsvd2;
  UINT32                                    Size;
  UINT32                                    SmBiosHandle;

} EFI_ACPI_PHYSICAL_COMP_IDENTIFIER_STRUC;

typedef struct {

  UINT8                                     Type;
  UINT8                                     Rsvd;
  UINT16                                    Length;
  UINT16                                    Flag;
  UINT16                                    Rsvd1;
  EFI_ACPI_MEMCTRL_CHAR_TABLE               MemCtrlCharTable;
  UINT16               	                    Rsvd2;
  UINT16                                    NumProxDomains;  
  UINT32                                    ProxDomain;      //  Proximity domain set to socket ID.
  EFI_ACPI_PHYSICAL_COMP_IDENTIFIER_STRUC   PhyCompIdentStruc[MAX_DIMM];

} EFI_ACPI_MEM_CTRL_STRUC;

typedef struct {

  UINT8                                     Type;
  UINT8                                     Rsvd;
  UINT16                                    Length;
  UINT16                                    Flag;
  UINT16                                    Rsvd1;
  UINT16                                    SckIdent;
  UINT16                                    Rsvd2;
  EFI_ACPI_MEM_CTRL_STRUC                   MemCtrlStruc[MAX_CH];

} EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC;

typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER               Header;
  UINT8                                     rsvd[4];
  EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC          TopLevelmemAggrDevStruc[MAX_SOCKET];

} EFI_ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE;

#pragma pack ()

#endif

