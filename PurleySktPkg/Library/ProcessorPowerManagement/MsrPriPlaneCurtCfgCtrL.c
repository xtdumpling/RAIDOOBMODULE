//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  MsrPriPlaneCurtCftCtrl.c

**/

#include "CpuPpmIncludes.h"

#define   Current_Limit 0x1FFF

/**

    This routine is called to program MSR PRIMARY_PLANE_CURRENT_CONFIG_CONTROL.

    @param PPMPolicy       - Pointer to PPM Policy protocol instance
    @param ProcessorNumber - Processor index that is executing

    @retval None

**/
VOID
ProgMsrPriPlaneCurtCfgCtrL (
  EFI_CPU_PM_STRUCT *ppm,
  UINTN             ProcessorNumber
  )
{
  MSR_REGISTER MsrValue;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  CpuCsrAccess = ppm->CpuCsrAccess;

  //
  // Read PRIMARY_PLANE_CURRENT_CONFIG_CONTROL
  //
  MsrValue.Qword = AsmReadMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL);

  if (ppm->Setup->AdvPwrMgtCtlFlags & PCD_CPU_CURRENT_CONFIG) {
    if ((ppm->Setup->PP0CurrentCfg.Dwords.Low & CURRENT_LIMIT) < Current_Limit) {
      MsrValue.Dwords.Low &= ~(CURRENT_LIMIT);
      MsrValue.Dwords.Low |= (ppm->Setup->PP0CurrentCfg.Dwords.Low & CURRENT_LIMIT);
      //
      // Write PRIMARY_PLANE_CURRENT_CONFIG_CONTROL
      //
      AsmWriteMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL, 0, 64, MsrValue.Qword);
    }
  }
}
