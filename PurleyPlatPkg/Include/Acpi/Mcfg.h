//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Mcfg.h

  ACPI Memory mapped configuration space base address Description Table
  implementation, based on PCI Firmware Specification Revision 3.0 final draft,
  downloadable at http://www.pcisig.com/home

**/

#ifndef _MCFG_H_
#define _MCFG_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include "Platform.h"
//
// "MCFG" Static Resource Affinity Table
//
#define EFI_ACPI_3_0_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_SIGNATURE 0x4746434D

//
// MCFG Definitions, see specification for details.
//
#define EFI_ACPI_OEM_MCFG_REVISION  0x00000001

//
// Define the number of each table type.
// This is where the table layout is modified.
//
#define EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_BASE_ADDRESS_STRUCTURE_COUNT MAX_SOCKET
 
//
// MCFG Table definition.  The table must be defined in a platform
// specific manner.
//
//
// Ensure proper structure formats
//
#pragma pack(1)

typedef struct {
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER        Header;

#if EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_BASE_ADDRESS_STRUCTURE_COUNT > 0
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  Segment[
    EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_BASE_ADDRESS_STRUCTURE_COUNT];
#endif

} EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE;

#pragma pack()

#endif // _MCFG_H_
