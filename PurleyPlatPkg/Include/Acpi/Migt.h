//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
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


  @file Migt.h

  This file describes the contents of the MIGT ACPI table.

**/

#ifndef _MIGT_H_
#define _MIGT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>

//
// MIGT ACPI structure
//
typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER             Header;
  // MIGT Specific Entries
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  ControlRegister;
  UINT32                                  ControlRegisterValue;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  ActionRegion;

} EFI_MIGT_ACPI_DESCRIPTION_TABLE;


//
// MIGT ACPI Definitions
//
#define MIGT_SMI_SERVICE_ID                       0xFD        // Door Bell

#define EFI_MIGT_ACPI_TABLE_SIGNATURE             SIGNATURE_32('M','I','G','T')
#define EFI_MIGT_ACPI_DESCRIPTION_TABLE_REVISION  0x01
#define EFI_MIGT_ACPI_OEM_REVISION                0x00000000

//
// MIGT Control Register Generic Address Information
//
#define EFI_MIGT_CR_ACPI_ADDRESS_SPACE_ID         EFI_ACPI_2_0_SYSTEM_IO
#define EFI_MIGT_CR_ACPI_REGISTER_BIT_WIDTH       0x8
#define EFI_MIGT_CR_ACPI_REGISTER_BIT_OFFSET      0x0
#define EFI_MIGT_CR_ACPI_SMI_ADDRESS              0xB2

//
// MIGT Action Region Generic Address Information
//
#define EFI_MIGT_AR_ACPI_ADDRESS_SPACE_ID         EFI_ACPI_2_0_SYSTEM_MEMORY
#define EFI_MIGT_AR_ACPI_REGISTER_BIT_WIDTH       64
#define EFI_MIGT_AR_ACPI_REGISTER_BIT_OFFSET      0x0
#define EFI_MIGT_AR_ACPI_MEMORY_ADDRESS           0x0

#endif

