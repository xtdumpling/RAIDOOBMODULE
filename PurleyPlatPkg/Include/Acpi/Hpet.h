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


  @file Hpet.h

  This file describes the contents of the ACPI High Precision Event Timer Description Table
  (HPET).  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h
  All changes to the HPET contents should be done in this file.

**/

#ifndef _HPET_H_
#define _HPET_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

//
// HPET Definitions
//
#define EFI_ACPI_OEM_HPET_REVISION    0x00000001

#define EFI_ACPI_EVENT_TIMER_BLOCK_ID 0x8086A701

//
// Event Timer Block Base Address Information
//
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS_SPACE_ID EFI_ACPI_3_0_SYSTEM_MEMORY
// APTIOV_SERVER_OVERRIDE_RC_START : Register Bit Width is 64b
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_WIDTH        0x40
// APTIOV_SERVER_OVERRIDE_RC_END : Register Bit Width is 64b
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_OFFSET       0x00
#define EFI_ACPI_EVENT_TIMER_ACCESS_SIZE            0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS          0x00000000FED00000

#define EFI_ACPI_HPET_NUMBER                        0x00

// APTIOV_SERVER_OVERRIDE_RC_START : HPET timer accuracy, the main counter is clocked by the 14.31818 MHz clock.
#define EFI_ACPI_MIN_CLOCK_TICK                     14318
// APTIOV_SERVER_OVERRIDE_RC_END : HPET timer accuracy, the main counter is clocked by the 14.31818 MHz clock.

#define EFI_ACPI_HPET_ATTRIBUTES                    0x00

#endif
