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

  Module Name:  MsrTurboPowerLimit.c

**/

#include "CpuPpmIncludes.h"

#define PCD_CPU_TURBO_MODE_BIT                   0x80000000

/**

    - Enables Turbo Mode at package level
    - Based on user configuration
    - Configures Turbo Mode TDP power level and TDP power level time window
    - Configures Dynamic Turbo Power level and Dynamic Turbo Power time window
    - Enables Critical Power Clamp 1 and Critical Power Clamp 2

  @param PPMPolicy       - Pointer to policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
ProgramMsrTurboPowerLimit (
    EFI_CPU_PM_STRUCT *ppm,
    UINTN             ProcessorNumber
    )
{
//  UINT64 Ia32MiscEnableMsr;
  MSR_REGISTER TurboPowerLimitMsr;
  MSR_REGISTER PlatformInfoMsr;
  UINT16 ConvertedPowerLimit1;
  UINT16 ConvertedPowerLimit1Time;
  UINT16 ConvertedPowerLimit2Time;
  UINT16 ConvertedShortDurationPowerLimit;
  UINT16 Index;
  UINT16 Mutliplier;
  UINT16 GetPcdTemp = 0;
  UINT16 PackageTdp;                           
  UINT8  ProcessorPowerUnit;                 
  UINT32 RegEbx;
  UINT32 CurrentSocket;

  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, &RegEbx, NULL, NULL);
  CurrentSocket = RegEbx >> (24 + ppm->Info->mNumOfBitShift) ;

  ConvertedPowerLimit1Time = 0;
  Mutliplier = 0;
  PackageTdp          = ppm->Info->PackageTdp[CurrentSocket];
  ProcessorPowerUnit  = ppm->Info->ProcessorPowerUnit[CurrentSocket];

  TurboPowerLimitMsr.Qword = AsmReadMsr64 (MSR_TURBO_POWER_LIMIT);
  
  PlatformInfoMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  //
  // Check if TDP limits are programmable
  // Platform Info MSR (0xCE) [29]
  //
  if ((PlatformInfoMsr.Dwords.Low & PROG_TDP_LIMITS)) {
    //
    // Initialize the Power Limit 1 and Power Limit 1 enable bit
    // Power Limit 1: Turbo Power Limit MSR [14:0]
    // Power Limit 1 Enable: Turbo Power Limit MSR [15]
    //
    //
    // By default, program Power Limit 1 to Package TDP limit
    //
    ConvertedPowerLimit1 = (PackageTdp * ProcessorPowerUnit);
  
    GetPcdTemp = ppm->Setup->TurboPowerLimit.Dwords.Low & POWER_LIMIT_MASK;

    if (GetPcdTemp) {
      //
      // PPMPolicy->pTurboSettings->PowerLimit1Power is in watts. We need to convert it to
      // CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
      // Since we are converting from Watts to CPU power units, multiply by
      // PACKAGE_POWER_SKU_UNIT_MSR[3:0]. New TDP limit cannot be higher than
      // the fused TDP limit.
      //
      if (GetPcdTemp <= PackageTdp) {
        ConvertedPowerLimit1 = (GetPcdTemp * ProcessorPowerUnit);
      }

      //
      // OverClocking unit is detected
      //
      if (ppm->Info->CapId4 & B_OC_ENABLED) {
        ConvertedPowerLimit1 = (GetPcdTemp * ProcessorPowerUnit);
      }
    }
  
    TurboPowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
    TurboPowerLimitMsr.Dwords.Low |= (UINT32) (ConvertedPowerLimit1);
  
    //
    // Force Power Limit 1 override to be enabled
    //
    TurboPowerLimitMsr.Dwords.Low |= POWER_LIMIT_ENABLE;

    //
    // Convert seconds to MSR value. Since not all values are programmable, we'll select
    // the entry from mapping table which is either equal to the user selected value. OR to a value in the mapping table
    // which is closest (but less than) to the user-selected value.
    //
    ConvertedPowerLimit2Time = (UINT16) ((ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_1_TIME_MASK) >> POWER_LIMIT_1_TIME_SHIFT);
 
    GetPcdTemp = (UINT16) ((ppm->Setup->TurboPowerLimit.Dwords.Low & POWER_LIMIT_1_TIME_MASK) >> POWER_LIMIT_1_TIME_SHIFT);
  
    ConvertedPowerLimit1Time = mSecondsToMsrValueMapTable[0][1];
    for(Index = 0; mSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
      if (GetPcdTemp >= mSecondsToMsrValueMapTable[Index][0]) {
        ConvertedPowerLimit1Time = mSecondsToMsrValueMapTable[Index][1];
      }
    }

    //
    // Configure Power Limit 1 (Long Duration Turbo) time windows: Turbo Power Limit MSR [23:17]
    //
    TurboPowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_1_TIME_MASK;
    TurboPowerLimitMsr.Dwords.Low |= (UINT32)ConvertedPowerLimit1Time << 17;
    TurboPowerLimitMsr.Dwords.High &= ~POWER_LIMIT_1_TIME_MASK;
    TurboPowerLimitMsr.Dwords.High |= (UINT32)ConvertedPowerLimit2Time << 17;

    //
    // PPMPolicy->pTurboSettings->PowerLimit2Power value is in watts. We need to convert it to
    // CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
    // Since we are converting from Watts to CPU power units, multiply by
    // PACKAGE_POWER_SKU_UNIT_MSR[3:0]
    //
    Mutliplier = 120;
    ConvertedShortDurationPowerLimit = EFI_IDIV_ROUND ((Mutliplier * PackageTdp * ProcessorPowerUnit), 100);
  
    GetPcdTemp = ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_MASK;
  
    if ((GetPcdTemp > 0) & (GetPcdTemp < EFI_IDIV_ROUND ((Mutliplier * PackageTdp), 100))) {
      ConvertedShortDurationPowerLimit = (GetPcdTemp * ProcessorPowerUnit);
    }

    //
    // OverClocking unit is detected
    //
    if (ppm->Info->CapId4 & B_OC_ENABLED) {
      if(GetPcdTemp > 0) {
        ConvertedShortDurationPowerLimit = (GetPcdTemp * ProcessorPowerUnit);
      }
    }

    TurboPowerLimitMsr.Dwords.High &= ~POWER_LIMIT_MASK;  
    TurboPowerLimitMsr.Dwords.High |= (UINT32)ConvertedShortDurationPowerLimit;

    if (ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_ENABLE) {
      TurboPowerLimitMsr.Dwords.High |= POWER_LIMIT_ENABLE;
    }
  }
  
  AsmWriteMsr64 (MSR_TURBO_POWER_LIMIT, TurboPowerLimitMsr.Qword);

  //
  // Save for S3 if we are not locking MSR, otherwise we capture S3 save when we lock
  //
  if(!(ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_LOCK)) {
    WriteRegisterTable(ProcessorNumber, Msr, MSR_TURBO_POWER_LIMIT, 0, 64, TurboPowerLimitMsr.Qword);
  }
  
  return;
}
