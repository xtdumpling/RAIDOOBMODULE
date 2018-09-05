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


  @file Srat.h

  ACPI Static resource definition table implementation, defined at 
  http://microsoft.com/hwdev/design/srat.htm.

**/

#ifndef _SRAT_H_
#define _SRAT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <Acpi/StaticResourceAffinityTable.h>
#include "Platform.h"

//
// SRAT Definitions, see specification for details.
//
#define EFI_ACPI_OEM_SRAT_REVISION  0x00000001  // For Compatibility changed from 2 to 1
#define EFI_ACPI_OEM_SRAT_4_0_REVISION  0x00000002  //

//
// Backward Compatibility per ACPI 3.0. Required by Hyper-V. OS's ok so far as of 5/27/09
//
#define EFI_ACPI_SRAT_RESERVED_FOR_BACKWARD_COMPATIBILITY  0x00000001  
//
// Define the number of each table type.
// This is where the table layout is modified.
//
#define EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT  MAX_CPU_NUM
#define EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT                      MC_MAX_NODE*MAX_CRS_ENTRIES_PER_NODE  
#define EFI_ACPI_PROCESSOR_X2APIC_AFFINITY_STRUCTURE_COUNT						MAX_CPU_NUM
//
// Statis Resource Affinity Table header definition.  The table
// must be defined in a platform specific manner.
//
//
// Ensure proper structure formats
//
#pragma pack(1)

typedef struct {
  EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE_HEADER          Header;

#if EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT > 0
  EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE  Apic[
    EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT];
#endif

#if EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT > 0
  EFI_ACPI_MEMORY_AFFINITY_STRUCTURE                      Memory[EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT];
#endif

#if EFI_ACPI_PROCESSOR_X2APIC_AFFINITY_STRUCTURE_COUNT > 0
  EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE  x2Apic[EFI_ACPI_PROCESSOR_X2APIC_AFFINITY_STRUCTURE_COUNT];
#endif

} EFI_ACPI_4_0_STATIC_RESOURCE_AFFINITY_TABLE;

#pragma pack()

#endif
