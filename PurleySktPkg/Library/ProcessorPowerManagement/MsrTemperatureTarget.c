//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code for Haswell Debug Interface Feature

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  MsrTemperatureTarget.c

**/

#include "CpuPpmIncludes.h"


/**

  This routine is called to get MSR_TEMPERATURE_TARGET value. It will be call multiple time passing any the node to be programmed.

  @param Current processor

  @retval EFI_SUCCESS

**/
VOID
GetMsrTemperatureTarget (
  EFI_CPU_PM_STRUCT *ppm
  )
{

    UINT64 MsrValue;

    if (ppm->Setup->FuseTjMaxOffset) return;

    MsrValue = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    MsrValue &= B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK;
    ppm->Setup->FuseTjMaxOffset = (UINT8) LShiftU64 (MsrValue, N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET);

    return;
}

/**

  This routine is called to program MSR_TEMPERATURE_TARGET not programed 
  elsewhere. It will be call multiple time passing any the node to be programmed.

  @param PPMPolicy       - Pointer to PPM Policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
ProgramMsrTemperatureTarget (
  EFI_CPU_PM_STRUCT *ppm,
  UINTN             ProcessorNumber
  )
{

    UINT64 MsrValue;
    UINT8  Value = 0;

    MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
    Value = (ppm->Setup->TCCActivationOffset & 0x0F);
    if (Value > ppm->Setup->FuseTjMaxOffset) {
      Value = ppm->Setup->FuseTjMaxOffset;
    }

    if (MsrValue & B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET) {
      if(Value) {

        MsrValue = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
        MsrValue &= ~B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT;
        MsrValue |= LShiftU64 ((UINT64) Value, N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT);
 
        AsmWriteMsr64(MSR_TEMPERATURE_TARGET, MsrValue);
        //save for S3
        WriteRegisterTable(ProcessorNumber, Msr, MSR_TEMPERATURE_TARGET, 0, 64, MsrValue);
      }
    }

    return;
}
