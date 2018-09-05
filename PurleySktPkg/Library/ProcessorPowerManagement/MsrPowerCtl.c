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

  Module Name:  MsrPowerCtl.c

**/

#include "CpuPpmIncludes.h"

// PCU_CR_CAPID4_CFG bit Description (FUSE value)
#define B_UFS_DISABLE                 (1 << 24) // Bit 24


/**

  This routine is called to program Power_CTL not programed elsewhere. It will be call multiple time passing any the node to be programmed.

  @param PPMPolicy       - Pointer to PPM Policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
ProgramMsrPowerCtl (
  EFI_CPU_PM_STRUCT   *ppm,
  UINTN               ProcessorNumber
  )
{
  MSR_REGISTER MsrValue;

  //
  // Program MSR_POWER_CTL
  //
  MsrValue.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  MsrValue.Dwords.Low &= ~POWER_CTL_MASK;
  MsrValue.Dwords.Low |= ppm->Setup->PowerCtl.Dwords.Low; 

  MsrValue.Dwords.Low &= ~SAPM_IMC_C2_POLICY_EN;
  MsrValue.Dwords.Low &= ~PROCHOT_LOCK;
 
  if (ppm->Info->CapId4 & B_UFS_DISABLE) {
    MsrValue.Dwords.Low |= ( 1 << PWR_PERF_TUNING_DISABLE_SAPM_SHIFT );
  }

  AsmWriteMsr64 (MSR_POWER_CTL, MsrValue.Qword);
  //save for S3
  WriteRegisterTable(ProcessorNumber, Msr, MSR_POWER_CTL, 0, 64, MsrValue.Qword);

  if (ppm->Setup->PowerCtl.Dwords.Low & PWR_PERF_TUNING_CFG_MODE) {
    MSR_REGISTER MsrData;

    MsrData.Qword = AsmReadMsr64 (MSR_ENERGY_PERFORMANCE_BIAS);
    MsrData.Dwords.Low = ALTENERGYPERFIAS_DEFAULT;
    AsmWriteMsr64 (MSR_ENERGY_PERFORMANCE_BIAS, MsrData.Qword);
    //save for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_ENERGY_PERFORMANCE_BIAS, 0, 64, MsrData.Qword);
  }

  // Enables decay mode on package C-state entry
  MsrValue.Qword = AsmReadMsr64 (MSR_VR_MISC_CONFIG);
  MsrValue.Dwords.High &= ~B_IDLE_ENTRY_DECAY_ENABLE;
  if ((ppm->Setup->SapmCtl[0] & SETVID_DECAY_DISABLE) == 0) 
  {
    MsrValue.Dwords.High |= B_IDLE_ENTRY_DECAY_ENABLE;
  }
  AsmWriteMsr64 (MSR_VR_MISC_CONFIG, MsrValue.Qword);

  //save for S3
  WriteRegisterTable(ProcessorNumber, Msr, MSR_VR_MISC_CONFIG, 0, 64, MsrValue.Qword);

  return;
}
