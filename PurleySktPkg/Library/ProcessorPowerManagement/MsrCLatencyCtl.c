/**
This file contains an 'Intel Peripheral Driver' and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor. This file may
be modified by the user, subject to additional terms of the
license agreement
**/
/** @file

Code for Enhanced Intel Speedstep Technology

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name: MsrCLatencyCtl.c

**/

#include "CpuPpmIncludes.h" 

static const UINT32 C_LATENCY_MSR[] = {
  MSR_C_STATE_LATENCY_CONTROL_0,
  MSR_C_STATE_LATENCY_CONTROL_1,
  MSR_C_STATE_LATENCY_CONTROL_2,
};

VOID
ProgramMsrCLatency (
  EFI_CPU_PM_STRUCT *ppm,
  UINTN             ProcessorNumber
  )
  /**

  Program C-State Latency MSRs.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

  **/
{
  MSR_REGISTER  MsrData;
  UINT8         i;

  for(i = 0; i < NUM_CST_LAT_MSR; i++) {
    
    if(ppm->Setup->Cst.LatencyCtrl[i].Dwords.Low & (1 << VALID_SHIFT)) {
      MsrData.Qword = AsmReadMsr64 (C_LATENCY_MSR[i]);
      MsrData.Dwords.Low = (MsrData.Dwords.Low & 0xFFFF0000);                         //clear bit15:0;      

      MsrData.Qword |= ppm->Setup->Cst.LatencyCtrl[i].Qword;      
      AsmWriteMsr64(MSR_C_STATE_LATENCY_CONTROL_0, MsrData.Qword);   //program bit15:0 per user option

      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_C_STATE_LATENCY_CONTROL_0, 0, 64, MsrData.Qword);
    }
  }
  return;
}
 