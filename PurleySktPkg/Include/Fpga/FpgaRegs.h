/** @file
  Register names for FPGA Device


@copyright
 Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _FPGA_REGS_H_
#define _FPGA_REGS_H_

//
// FPGA Device PCIe0 (0xBCC0) PCI registes
//
#define R_FPGA_FME_BAR                         0x10
#define R_FPGA_VTD_BAR                         0x180

//
//  FPGA Thermal threshold setting
//
#define V_THRESHOLD_1_POLICY_90_P              0
#define V_THRESHOLD_1_POLICY_50_P              1

#define FME_ERROR_MASK   0x4008
#define FME_ERROR        0x4010
#define FME_FIRST_ERROR  0x4018
#define KTI_ERROR_STAT0  0x0390
#define KTI_ERROR_EN0    0x0394
#define KTI_ERROR_STAT1  0x0398
#define KTI_ERROR_EN1    0x039c
#define KTI_ERROR_STAT2  0x03a0
#define KTI_ERROR_EN2    0x03a4
#define KTI_ERROR_STAT3  0x03a8
#define KTI_ERROR_EN3    0x03ac
#define KTI_ERROR_STAT4  0x03b0
#define KTI_ERROR_EN4    0x03b4
#define KTI_ERROR_STAT5  0x03b8
#define KTI_ERROR_EN5    0x03bc
#define KTI_FERR         0x03c8
#define KTI_NERR         0x03cc
#endif
