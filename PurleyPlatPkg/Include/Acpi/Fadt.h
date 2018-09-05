//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**
Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file Fadt.h

  This file describes the contents of the ACPI Fixed ACPI Description Table
  (FADT).  Some additional ACPI values are defined in Acpi1_0.h and Acpi2_0.h.
  All changes to the FADT contents should be done in this file.

**/

#ifndef _FADT_H_
#define _FADT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <PchAccess.h>
#include "Platform.h"

//
// FADT Definitions
//
#define EFI_ACPI_OEM_FADT_REVISION  0x00000000
//
// For ACPI 1.0 supprt
// Harwich sets this to #define EFI_ACPI_INT_MODEL 0x01
//

/*
INT_MODEL Field
The FADT defines the fixed ACPI information that is needed by the operating system as 
defined in section 5.2.5 of the ACPI 1.0 specification. Currently, this table provides a field 
for INT_MODEL, which was intended to be used to report the interrupt mode of the machine. 
This field is ignored by the Microsoft?Windows?2000 and later operating system because the 
Microsoft Windows 98 operating system only supports programmable interrupt controller (PIC) mode. 
This forces system manufacturers (OEMs) who want to support Windows 98 to enter a "0" in the INT_MODEL field. 
This would then preclude using multiple processors on Windows 2000 and later versions. 
Because the INT_MODEL field in the FADT cannot be dynamically updated while the machine is running, 
it cannot be used to report the interrupt mode, and therefore it is ignored by Windows 98, 
and Windows 2000 and later versions.
http://www.microsoft.com/whdc/system/CEC/ACPI-MP.mspx
For Watson Creek we set this to 0 and then dynamically update this to 1 in the DSDT tables _PIC method and GPIC field. 
*/
#define EFI_ACPI_INT_MODEL            0x0

#define EFI_ACPI_PREFERRED_PM_PROFILE 0x04

#define EFI_ACPI_SCI_INT              0x0009
#define EFI_ACPI_SMI_CMD              0x000000B2

#define EFI_ACPI_ACPI_ENABLE          0xA0
#define EFI_ACPI_ACPI_DISABLE         0xA1
#define EFI_ACPI_S4_BIOS_REQ          0x00
#define EFI_ACPI_CST_CNT              0x00

#define EFI_ACPI_PSTATE_CNT           0x00
#define EFI_ACPI_GPE1_BASE            (EFI_ACPI_GPE1_BLK_BIT_WIDTH / 2)
#define EFI_ACPI_P_LVL2_LAT           0x0065
#define EFI_ACPI_P_LVL3_LAT           0x03E9
//
// #define EFI_ACPI_FLUSH_SIZE           0x0400
//
#define EFI_ACPI_FLUSH_SIZE 0x0000
//
// #define EFI_ACPI_FLUSH_STRIDE         0x0010
//
#define EFI_ACPI_FLUSH_STRIDE 0x0000
#define EFI_ACPI_DUTY_OFFSET  0x01
//
// #define EFI_ACPI_DUTY_OFFSET          0x00
//
#define EFI_ACPI_DUTY_WIDTH 0x00

#define EFI_ACPI_DAY_ALRM   0x0D
#define EFI_ACPI_MON_ALRM   0x00
#define EFI_ACPI_CENTURY    0x32

//
// IA-PC Boot Architecture Flags, see ACPI 2.0 table specification and Acpi2_0.h
// No legacy KBC and legcay devices other reported in ASL code
//
// #define EFI_ACPI_IAPC_BOOT_ARCH (EFI_ACPI_2_0_LEGACY_DEVICES | EFI_ACPI_2_0_8042)
//
//#define EFI_ACPI_IAPC_BOOT_ARCH EFI_ACPI_2_0_8042
#define EFI_ACPI_OS_ASPM_DISABLE        (1 << 4)

#define EFI_ACPI_IAPC_BOOT_ARCH         EFI_ACPI_2_0_8042 | EFI_ACPI_OS_ASPM_DISABLE

//
// Fixed Feature Flags, see ACPI 2.0 table specification and Acpi2_0.h
//
#define EFI_ACPI_FIXED_FEATURE_FLAGS  (EFI_ACPI_2_0_RESET_REG_SUP | \
                                      EFI_ACPI_2_0_RTC_S4 | \
                                      EFI_ACPI_2_0_SLP_BUTTON | \
                                      EFI_ACPI_2_0_WBINVD | \
                                      EFI_ACPI_2_0_PROC_C1 \
          )

//
// PM1A Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM1A_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS           0x00000500

//
// PM1B Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS           0x00000000

//
// PM1A Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH         0x10
#define EFI_ACPI_PM1A_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x04)

//
// PM1B Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS           0x00

//
// PM2 Control Register Block Generic Address Information
//
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS_SPACE_ID EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH        0x08
#define EFI_ACPI_PM2_CNT_BLK_BIT_OFFSET       0x00
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS          (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x50)

//
// Power Management Timer Control Register Block Generic Address
// Information
//
#define EFI_ACPI_PM_TMR_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_PM_TMR_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM_TMR_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM_TMR_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x08)

//
// General Purpose Event 0 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE0_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_GPE0_BLK_BIT_WIDTH         (2 * 0x80)  // size of R_PCH_ACPI_GPE0_STS_127_96 + R_PCH_ACPI_GPE0_EN_127_96
#define EFI_ACPI_GPE0_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_GPE0_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x80)

//
// General Purpose Event 1 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE1_BLK_ADDRESS_SPACE_ID  EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_GPE1_BLK_BIT_WIDTH         0x0
#define EFI_ACPI_GPE1_BLK_BIT_OFFSET        0x0
#define EFI_ACPI_GPE1_BLK_ADDRESS           0x0
//
// Reset Register Generic Address Information
//
#define EFI_ACPI_RESET_REG_ADDRESS_SPACE_ID EFI_ACPI_2_0_SYSTEM_IO
#define EFI_ACPI_RESET_REG_BIT_WIDTH        0x08
#define EFI_ACPI_RESET_REG_BIT_OFFSET       0x00
#define EFI_ACPI_RESET_REG_ADDRESS          0x00000CF9
#define EFI_ACPI_RESET_VALUE                0x06

//
// Number of bytes decoded by PM1 event blocks (a and b)
//
#define EFI_ACPI_PM1_EVT_LEN  ((EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM1 control blocks (a and b)
//
#define EFI_ACPI_PM1_CNT_LEN  ((EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM2 control block
//
#define EFI_ACPI_PM2_CNT_LEN  (EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by PM timer block
//
#define EFI_ACPI_PM_TMR_LEN (EFI_ACPI_PM_TMR_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE0 block
//
#define EFI_ACPI_GPE0_BLK_LEN (EFI_ACPI_GPE0_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE1 block
//
#define EFI_ACPI_GPE1_BLK_LEN (EFI_ACPI_GPE1_BLK_BIT_WIDTH / 8)

#endif
