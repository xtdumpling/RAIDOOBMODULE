//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CsrTurboPowerLimit.c

**/

#include "CpuPpmIncludes.h"

#define N_MSR_TURBO_MODE_DISABLE 38

//
// Table to convert Seconds into equivalent MSR values
// This table is used for TDP Time Window programming
//
const UINT16        mSecondsToMsrValueMapTable[][2] = {
        // Seconds       MSR Value
       {   1        ,     0xA },
       {   2        ,     0xB },
       {   3        ,    0x4B },
       {   4        ,     0xC },
       {   5        ,    0x2C },
       {   6        ,    0x4C },
       {   7        ,    0x6C },
       {   8        ,     0xD },
       {  10        ,    0x2D },
       {  12        ,    0x4D },
       {  14        ,    0x6D },
       {  16        ,     0xE },
       {  20        ,    0x2E },
       {  24        ,    0x4E },
       {  28        ,    0x6E },
       {  32        ,     0xF },
       {  40        ,    0x2F },
       {  48        ,    0x4F },
       {  56        ,    0x6F },
       {END_OF_TABLE,END_OF_TABLE}
};

/**

    This routine is called to program CSR_TURBO_POWER_LIMIT. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramCSRTurboPowerLimit (
  EFI_CPU_PM_STRUCT *ppm
  )
{
 
  UINT32   data;
  UINT32   data1;
  UINT64   TurboPowerLimitMsr;
  UINT64   PlatformInfoMsr;
//  UINT64 Ia32MiscEnables;
  UINT16   ConvertedPowerLimit1;
  UINT16   ConvertedPowerLimit1Time;
  UINT16   ConvertedPowerLimit2Time;
  UINT16   ConvertedShortDurationPowerLimit;
  UINT16   Index;
  UINT16   Mutliplier;
  UINT16   GetPcdTemp;
  UINT64   RegAddr = 0; 
  UINT8    Size = 4;
  UINT8    SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  ConvertedPowerLimit1Time = 0;
  Mutliplier = 0;
  
  // Read (CSR)
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, C2C3TT_PCU_FUN1_REG);

  // Program CSR_C2C3TT
  data &= ~PPDN_INIT_MASK;
  data |= ppm->Setup->C2C3TT;   
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, C2C3TT_PCU_FUN1_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, C2C3TT_PCU_FUN1_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );


  // Read (MSR)
  TurboPowerLimitMsr = AsmReadMsr64 (MSR_TURBO_POWER_LIMIT);
  PlatformInfoMsr = AsmReadMsr64 (MSR_PLATFORM_INFO);

  // PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG 
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG);
  // PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG
  data1 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG);
  
  // Program CSR_TURBO_POWER_LIMIT
  //
  // Check if TDP limits are programmable
  // Platform Info MSR (0xCE) [29]
  //
  if ((PlatformInfoMsr & PROG_TDP_LIMITS)) {
    //
    // Initialize the Power Limit 1 and Power Limit 1 enable bit
    // Power Limit 1: Turbo Power Limit MSR [14:0]
    // Power Limit 1 Enable: Turbo Power Limit MSR [15]
    //
    //
    // By default, program Power Limit 1 to Package TDP limit
    //
    ConvertedPowerLimit1 = (ppm->Info->PackageTdp[SocketNumber] * ppm->Info->ProcessorPowerUnit[SocketNumber]);
  
    GetPcdTemp = ppm->Setup->TurboPowerLimit.Dwords.Low & POWER_LIMIT_MASK;
  
    if (GetPcdTemp) {
      //
      // PPMPolicy->pTurboSettings->PowerLimit1Power is in watts. We need to convert it to
      // CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
      // Since we are converting from Watts to CPU power units, multiply by
      // PACKAGE_POWER_SKU_UNIT_MSR[3:0]. New TDP limit cannot be higher than
      // the fused TDP limit.
      //
      if (GetPcdTemp <= ppm->Info->PackageTdp[SocketNumber]) {
        ConvertedPowerLimit1 = (GetPcdTemp * ppm->Info->ProcessorPowerUnit[SocketNumber]);
      }

      //
      // OverClocking unit is detected
      //
      if (ppm->Info->CapId4 & B_OC_ENABLED) {
        ConvertedPowerLimit1 = (GetPcdTemp * ppm->Info->ProcessorPowerUnit[SocketNumber]);
      }
    }

    data &= ~POWER_LIMIT_MASK;
    data |= (UINT32) (ConvertedPowerLimit1);

    //
    // Force Power Limit 1 override to be enabled
    //
    data |= POWER_LIMIT_ENABLE;

    //
    // Convert seconds to MSR value. Since not all values are programmable, we'll select
    // the entry from mapping table which is either equal to the user selected value. OR to a value in the mapping table
    // which is closest (but less than) to the user-selected value.
    //
    ConvertedPowerLimit2Time = (UINT16) ((ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_1_TIME_MASK) >> POWER_LIMIT_1_TIME_SHIFT);
    GetPcdTemp = (UINT16) ((ppm->Setup->TurboPowerLimit.Dwords.Low & POWER_LIMIT_1_TIME_MASK) >> POWER_LIMIT_1_TIME_SHIFT);

    ConvertedPowerLimit1Time = mSecondsToMsrValueMapTable[0][1];
    for(Index = 0; mSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {

      if (GetPcdTemp == mSecondsToMsrValueMapTable[Index][0]) {
        ConvertedPowerLimit1Time = mSecondsToMsrValueMapTable[Index][1];
        continue;
      }

      if (GetPcdTemp > mSecondsToMsrValueMapTable[Index][0]) {
        ConvertedPowerLimit1Time = mSecondsToMsrValueMapTable[Index][1];
      } else {
        continue;
      }
    }

    //
    // Configure Power Limit 1 (Long Duration Turbo) time windows: Turbo Power Limit MSR [23:17]
    //
    data &= ~POWER_LIMIT_1_TIME_MASK;
    data |= (UINT32) (ConvertedPowerLimit1Time << 17);
    data1 &= ~POWER_LIMIT_1_TIME_MASK;
    data1 |= (UINT32) (ConvertedPowerLimit2Time << 17);
  
    //
    // Initialize Short Duration Power limit and enable bit
    // Short duration Power Limit: Turbo Power Limit MSR (0x450h) [46:32]
    // Short duration Power Limit Enable:Turbo Power Limit MSR (0x450h) [47]
    //
    //
    // PPMPolicy->pTurboSettings->PowerLimit2Power value is in watts. We need to convert it to
    // CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
    // Since we are converting from Watts to CPU power units, multiply by
    // PACKAGE_POWER_SKU_UNIT_MSR[3:0]
    //
    Mutliplier = 120;
    ConvertedShortDurationPowerLimit = EFI_IDIV_ROUND ((Mutliplier * ppm->Info->PackageTdp[SocketNumber] * ppm->Info->ProcessorPowerUnit[SocketNumber]), 100);
  
    GetPcdTemp = ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_MASK;
  
    if ((GetPcdTemp > 0) & (GetPcdTemp < EFI_IDIV_ROUND ((Mutliplier * ppm->Info->PackageTdp[SocketNumber]), 100))) {
      ConvertedShortDurationPowerLimit = EFI_IDIV_ROUND ((Mutliplier * (UINT8) GetPcdTemp * ppm->Info->ProcessorPowerUnit[SocketNumber]), 100);
    }

    //
    // OverClocking unit is detected
    //
    if (ppm->Info->CapId4 & B_OC_ENABLED) {
      if(GetPcdTemp > 0) {
        ConvertedShortDurationPowerLimit = EFI_IDIV_ROUND ((Mutliplier * (UINT8) GetPcdTemp * ppm->Info->ProcessorPowerUnit[SocketNumber]), 100);
      }
    }

    data1 &= ~POWER_LIMIT_MASK;
    data1 |= (UINT32) (ConvertedShortDurationPowerLimit);

    if (ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_ENABLE) {
      data1 |= POWER_LIMIT_ENABLE;
    }
  }

  // Program CSR_TURBO_POWER_LIMIT
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG, data);
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG, data1);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N0_PCU_FUN0_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data1 );

  return;
}

/**
  This routine is called to program FAST_RAPL_NSTRIKE_PL2_DUTY_CYCLE. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS
**/
VOID
ProgramB2PFastRaplDutyCycle (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 Tempdata = 0;
  UINT8  SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  if (ppm->Setup->FastRaplDutyCycle > 0) {
    PM_MBX_DATA = ppm->Setup->FastRaplDutyCycle; 
    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_FAST_RAPL_DUTY_CYCLE;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
  }

  return;
}
