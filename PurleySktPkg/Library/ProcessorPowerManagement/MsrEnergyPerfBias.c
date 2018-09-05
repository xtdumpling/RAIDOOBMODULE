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

  Module Name:  MsrEnergyPerfBias.c

**/

#include "CpuPpmIncludes.h"

/**

  This routine is called to program ENERGY_PERF_BIAS_CONFIG.

  @param PPMPolicy       - Pointer to PPM Policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
ProgramEnergyPerfBiasConfigMsr (
  EFI_CPU_PM_STRUCT   *ppm,
  UINTN               ProcessorNumber
  )

{
  
  MSR_REGISTER  MsrData;

  //
  // Program ENERGY_PERF_BIAS_CONFIG(0xA01)
  //
  MsrData.Qword = AsmReadMsr64 (MSR_ENERGY_PERF_BIAS_CONFIG);
  MsrData.Dwords.Low &= ~ENERGY_PERF_BIAS_CTRL_MASK ;
  MsrData.Dwords.Low |= ppm->Setup->PerfBiasConfig.Dwords.Low;
  AsmWriteMsr64 (MSR_ENERGY_PERF_BIAS_CONFIG, MsrData.Qword);

  //save for S3
  WriteRegisterTable(ProcessorNumber, Msr, MSR_ENERGY_PERF_BIAS_CONFIG, 0, 64, MsrData.Qword);
}
