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

Module Name: MsrTurboRatioLimit.c

**/

#include "CpuPpmIncludes.h"

BOOLEAN 
RatioLimitCheck (
  UINT32 *RatioLimit,
  UINT8   CoreCount
  );

/**

    This function check the user RatioLimit option inputs

    @param RatioLimit - pointer to user ratiolimit inputs
    @param CoreCount  - socket's active core counts

    @retval RatioLimitState - TRUE if user inputs are valid otherwise FALSE

**/
BOOLEAN 
RatioLimitCheck (
  UINT32 *RatioLimit,
  UINT8   CoreCount
  )
{
  BOOLEAN RatioLimitState = TRUE;
  UINT32   i;
    
  for(i = 1; i < CoreCount; i++) {
    RatioLimitState = (RatioLimit[0] >= RatioLimit[i]);
    if(!RatioLimitState) {
        break;
    }
  }

  return RatioLimitState;

} 

BOOLEAN 
RatioLimitCheckSkx (
  VOID
  )
/**

    This function checks whether capable for TurboLimit update

    @param  none

    @retval Status 

**/
{
  BOOLEAN       Status;
  MSR_REGISTER PlatformInfoMsr;
  MSR_REGISTER FlexRatioMsr;
  UINT8 OverclockingBins;
  UINT8 MaxRatioOverride;
  UINT16 MaxBusRatio, MinBusRatio;

  Status = TRUE;  
  //
  // Check if XE capable
  //
  PlatformInfoMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  FlexRatioMsr.Qword = AsmReadMsr64 (MSR_FLEX_RATIO);

  MaxBusRatio = PlatformInfoMsr.Bytes.SecondByte;
  MinBusRatio = PlatformInfoMsr.Bytes.SixthByte;
  OverclockingBins = (UINT8)((FlexRatioMsr.Dwords.Low & OVERCLOCKING_BINS) >> 17);

  MaxRatioOverride = (UINT8)(MaxBusRatio + OverclockingBins);
  //MaxBusRatio is the MaxTurboratio

  //
  // Check if processor turbo-ratio can be overriden:
  // SandyBridge BWG Section 14.14.2
  // If PLATFORM INFO MSR [28] == 1 and FLEX_RATIO MSR [19:17] != 0
  // Processor with capability to override turbo-ratio detected (either XE or Overclocking part detected)
  //
  if ((PlatformInfoMsr.Dwords.Low & PROG_RATIO_LIMITS) == 0) {
    //
    // Not XE or Over clocking Capable processor.
    //
    Status = FALSE;
  }

  return Status;
}

VOID
ProgramMsrTurboRatioLimit (
  EFI_CPU_PM_STRUCT *ppm,
  UINTN             ProcessorNumber
  )
  /**

  Initializes XE OR Overclocking support in the processor.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

  **/
{
  UINT8 CoreCount;
  MSR_REGISTER TurboRatioLimitRatio;
  MSR_REGISTER TurboRatioLimitCores;  
  MSR_REGISTER CoreThreadCount;
  MSR_REGISTER FlexRatioMsr;
  XE_STRUCT *XePtr = NULL;
  TURBO_RATIO_LIMIT_RATIO_CORES *TurbRatioLimitRatioCores;

  XePtr = (XE_STRUCT *)&(ppm->Setup->Xe);
  TurbRatioLimitRatioCores = (TURBO_RATIO_LIMIT_RATIO_CORES *)&(ppm->Setup->TurboRatioLimitRatioCores);

  //
  // Now initialize turbo ratio limit MSR
  // Find the number of active cores and initialize the ratio limits only if they are available.
  // Also program the VID value for the new max Turbo ratio by programming Flex Ratio MSR.
  //
  CoreThreadCount.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  CoreCount = (UINT8) (CoreThreadCount.Dwords.Low >> CORE_COUNT_OFFSET);

  TurboRatioLimitRatio.Qword = AsmReadMsr64 (EFI_MSR_TURBO_RATIO_LIMIT_RATIOS);
  TurboRatioLimitCores.Qword = AsmReadMsr64 (EFI_MSR_TURBO_RATIO_LIMIT_CORES);
  
  if(!RatioLimitCheckSkx()) {
    return;
  }

  //
  FlexRatioMsr.Qword = AsmReadMsr64 (MSR_FLEX_RATIO);
  if (FlexRatioMsr.Dwords.Low & OC_LOCK) { 
    //
    // Clear OC_LOCK if it is set
    //
    FlexRatioMsr.Dwords.Low &= ~OC_LOCK;
    AsmWriteMsr64 (MSR_FLEX_RATIO, FlexRatioMsr.Qword);
    //save for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_FLEX_RATIO, 0, 64, FlexRatioMsr.Qword);
    //
    // Program VID for the new Max Turbo Ratio
    //
    FlexRatioMsr.Dwords.Low &= ~MAX_EXTRA_VOLTAGE;
    FlexRatioMsr.Dwords.Low |= XePtr->Voltage;
    AsmWriteMsr64 (MSR_FLEX_RATIO, FlexRatioMsr.Qword);
    //save for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_FLEX_RATIO, 0, 64, FlexRatioMsr.Qword);
  }

  //now program TurboRatioLimitRatio and TurboRatioLimitCores Msr per user options
  TurboRatioLimitRatio.Qword &= (UINT64)*(UINT64 *)TurbRatioLimitRatioCores->RatioLimitRatioMask;
  TurboRatioLimitCores.Qword &= (UINT64)*(UINT64 *)TurbRatioLimitRatioCores->RatioLimitCoresMask;
  TurboRatioLimitRatio.Qword |= (UINT64)*(UINT64 *)TurbRatioLimitRatioCores->RatioLimitRatio;
  TurboRatioLimitCores.Qword |= (UINT64)*(UINT64 *)TurbRatioLimitRatioCores->RatioLimitCores;

  AsmWriteMsr64 (EFI_MSR_TURBO_RATIO_LIMIT_RATIOS, TurboRatioLimitRatio.Qword);
  //save for S3
  WriteRegisterTable(ProcessorNumber, Msr, EFI_MSR_TURBO_RATIO_LIMIT_RATIOS, 0, 64, TurboRatioLimitRatio.Qword);

  AsmWriteMsr64 (EFI_MSR_TURBO_RATIO_LIMIT_CORES, TurboRatioLimitCores.Qword);
  //save for S3
  WriteRegisterTable(ProcessorNumber, Msr, EFI_MSR_TURBO_RATIO_LIMIT_CORES, 0, 64, TurboRatioLimitCores.Qword);

  return;
}

