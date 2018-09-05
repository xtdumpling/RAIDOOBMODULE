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


  @file Svos.h

  This file describes the contents of the SVOS ACPI.  

**/

#ifndef _SVOS_H_
#define _SVOS_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include "Platform.h"

//
// SVOS ACPI structure
//
typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER             Header;
  //SVOS Speicific Entries
  UINT8                                   SmiDoorBell;
  UINT8                                   Reserved1;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  SvSmmGas;

} EFI_SVOS_ACPI_DESCRIPTION_TABLE;


//
// SVOS ACPI Definitions
//
#define SVOS_SMI_SERVICE_ID 0xFE        //Door Bell

#define EFI_SVOS_ACPI_TABLE_SIGNATURE             SIGNATURE_32('S','V','O','S')

#define EFI_SVOS_ACPI_DESCRIPTION_TABLE_REVISION  0x01

#define EFI_SVOS_ACPI_OEM_REVISION                0x00000000

// SVOS SMM Parameter Region Generic Address
// Information
//
#define EFI_SVOS_ACPI_ADDRESS_SPACE_ID      EFI_ACPI_2_0_SYSTEM_MEMORY
#define EFI_SVOS_ACPI_REGISTER_BIT_WIDTH    0x0
#define EFI_SVOS_ACPI_REGISTER_BIT_OFFSET   0x0
#define EFI_SVOS_ACPI_SMI_ADDRESS           0x0


#endif

