//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  2007 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file BackCompatiable.h
   
  Back Compatiable temp header file

**/
#ifndef __BACK_COMPATIABLE_H__
#define __BACK_COMPATIABLE_H__

//
// ACPI and legacy I/O register offsets from PMBASE
//
#define R_ACPI_PM1_STS                      0x00
#define R_ACPI_PM1_EN                       0x02
#define R_ACPI_PM1_CNT                      0x04
#define R_ACPI_PM1_TMR                      0x08
#define R_ACPI_PROC_CNT                     0x10
#define R_ACPI_PM2_CNT                      0x50
#define R_ACPI_GPE0_STS                     0x20
#define R_ACPI_GPE0_EN                      0x28
#define R_ACPI_SMI_EN                       0x30
#define R_ACPI_SMI_STS                      0x34
#define R_ACPI_ALT_GP_SMI_EN                0x38
#define R_ACPI_ALT_GP_SMI_STS               0x3A

#define R_ACPI_LV2                          0x14

#define R_IOPORT_CMOS_STANDARD_INDEX            0x70
#define R_IOPORT_CMOS_STANDARD_DATA             0x71

#define R_IOPORT_CMOS_UPPER_INDEX               0x72
#define R_IOPORT_CMOS_UPPER_DATA                0x73

#define R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS     0x0E

//
// Misc PCI register offsets and sizes
//
#define R_EFI_PCI_SVID                              0x2C
#define   V_EFI_PCI_SVID_SIZE                         0x2
#define R_EFI_PCI_SID                               0x2E
#define   V_EFI_PCI_SID_SIZE                          0x2

#endif

