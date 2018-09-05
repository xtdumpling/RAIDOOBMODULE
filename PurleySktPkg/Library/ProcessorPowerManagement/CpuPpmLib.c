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

  Module Name:  CpuPpmLib.c

**/

#include "CpuPpmIncludes.h"
#include <Library/PlatformHooksLib.h> //for IsSimicsPlatform() will remove when Simic fixIsSimicsPlatform

extern EFI_MP_SERVICES_PROTOCOL     *mMpService;

/**

    Initializes the platform power management library. This must be called
    prior to any of the library functions being used.

    Arguments: None

    Returns: None

**/
VOID
InitializeCpuPPMLib (
    EFI_CPU_PM_STRUCT   *ppm
    )
{
  PACKAGE_POWER_SKU_UNIT_PCU_FUN0_STRUCT      PackagePowerSKUUnit;  //pwr_unit, time_unit
  PACKAGE_POWER_SKU_N0_PCU_FUN0_STRUCT        PackagePowerSKU; //pkg_tdp
  UINT8     ProcessorPowerUnit = 0;
  UINT8     ProcessorTimeUnit = 0;
  UINT16    PackageTdp = 0;
  UINT8     SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  if (SocketNumber == 0) {
    ppm->Version.Major = (UINT8)PPM_MAJOR;
    ppm->Version.Minor = (UINT8)PPM_MINOR;
    ppm->Version.Rev   = (UINT8)PPM_REV;
    ppm->Version.Build = (UINT8)PPM_BUILD;
  }

  //
  // Get Processor TDP
  // Get Maximum Power from Turbo Power Limit CSR Bits[14:0]
  // and convert it to units specified by Package Power SKU
  // Unit CSR [3:0]
  //
  PackagePowerSKUUnit.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PACKAGE_POWER_SKU_UNIT_PCU_FUN0_REG);
  PackagePowerSKU.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PACKAGE_POWER_SKU_N0_PCU_FUN0_REG);

  ProcessorPowerUnit = (UINT8) PackagePowerSKUUnit.Bits.pwr_unit;
  if( ProcessorPowerUnit == 0 ) {
    ProcessorPowerUnit = 1;
  } else {
    ProcessorPowerUnit = (UINT8) LShiftU64 (2, (ProcessorPowerUnit - 1));
  }

  PackageTdp = (UINT16) DivU64x32 ( (UINT64)PackagePowerSKU.Bits.pkg_tdp, ProcessorPowerUnit);

  ProcessorTimeUnit = (UINT8) PackagePowerSKUUnit.Bits.time_unit;
  if ( ProcessorTimeUnit == 0 ) {
    ProcessorTimeUnit = 1;
  } else {
    ProcessorTimeUnit = (UINT8) LShiftU64 (2, (ProcessorTimeUnit - 1));
  }

  ppm->Info->PackageTdp[SocketNumber] = PackageTdp;
  ppm->Info->ProcessorPowerUnit[SocketNumber] = ProcessorPowerUnit;
  ppm->Info->ProcessorTimeUnit[SocketNumber]  = ProcessorTimeUnit;

  return;
}

/**

    GC_TODO: add routine description

    @param ppm - GC_TODO: add arg description

    @retval None

**/
VOID
PStateTransition (
  EFI_CPU_PM_STRUCT   *ppm
  )
{

  MSR_REGISTER  PerformanceControl;
  MSR_REGISTER  PlatformInfo;
  UINT32        RegEcx;
  UINTN         ProcessorNumber = 0;

  mMpService->WhoAmI(mMpService, &ProcessorNumber);

  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, &RegEcx, NULL);
  if (!(RegEcx & B_EFI_CPUID_VERSION_INFO_ECX_EIST))  return;

  PerformanceControl.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTL);
  PlatformInfo.Qword =  AsmReadMsr64 (MSR_PLATFORM_INFO);

  PerformanceControl.Dwords.Low &= (UINT32)~(B_IA32_PERF_CTRLP_STATE_TARGET);  //clear P target
  // set P-target to performance (max ratio) as default
  PerformanceControl.Dwords.Low |= (PlatformInfo.Dwords.Low & (UINT32)B_IA32_PERF_CTRLP_STATE_TARGET);

  if(ppm->Setup->AdvPwrMgtCtlFlags & PCU_CPU_EFFICIENT_BOOT) { //max efficent
    PerformanceControl.Dwords.Low &= (UINT32)~(B_IA32_PERF_CTRLP_STATE_TARGET);  //clear P target
    // set P target to max efficient (min Ratio)
    PerformanceControl.Dwords.Low |= (PlatformInfo.Dwords.High & (UINT32)B_IA32_PERF_CTRLP_STATE_TARGET);
  }

  AsmWriteMsr64 (MSR_IA32_PERF_CTL, PerformanceControl.Qword);
  WriteRegisterTable(ProcessorNumber, Msr, MSR_IA32_PERF_CTL, 0, 64, PerformanceControl.Qword);
  return;
}


/**

    Executed by System BSP only.
    Common software programming steps needed before warm reset and CPU-only reset.


    @param PPMPolicy Pointer to PPM Policy protocol instance

    @retval EFI_SUCCESS

**/
VOID
PpmSetBiosInitDone (
    EFI_CPU_PM_STRUCT   *ppm
    )
{

  UINT32    data = 0;
  UINT8     SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 Tempdata = 0;
  UINT64    RegAddr = 0;
  UINT8     Size = 4;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);

  PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
  Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
  if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
    data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);

    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG;
    PM_MBX_DATA = (UINT32) data;

    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
  }

  PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
  PM_MBX_DATA = 0;
  Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
  if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
    data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);
  }

  //BIOS_RESET_CPL_PCU_FUN1_REG
  ppm->Info->Bios_Reset_Cpl_Phase= 3; //set BIOS_RESET_CPL3
  Program_Bios_Reset_Cpl(ppm);

  if (/*PPMPolicy->HybridMode == PPM_DISABLE*/ 1) {
    // if DRAM RAPL Hybrid mode is disabled then lock Mailbox interface, else leave it clear to be be set later
    ppm->Info->Bios_Reset_Cpl_Phase= 4; //set BIOS_RESET_CPL4
    Program_Bios_Reset_Cpl(ppm);
    //Note: This is for DRAM RAPL, may move tie this to a
    //setup option later which is why this is on a seperate line
  }


  //CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG.lock
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG);
  data |= BIT31;
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, CSR_DESIRED_CORES_PCU_FUN1_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );

  return;
}

/**

    Executed by System BSP only.
    Common software programming steps needed before warm reset and CPU-only reset.


    @param PPMPolicy Pointer to PPM Policy protocol instance

    @retval EFI_SUCCESS

**/
VOID
PpmSetCsrLockBit (
    EFI_CPU_PM_STRUCT   *ppm
    )
{
  UINT32    data;
  UINT64    RegAddr = 0;
  UINT8     Size = 4;
  UINT8     SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG);
  data |= PCU_CR_PMAX_CFG_LOCK;
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PMAX_CONFIG_PCU_FUN0_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );

  if (ppm->Setup->SapmCtl[SocketNumber] & SAPMCTL_LOCK) {
    data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, SAPMCTL_PCU_FUN1_REG);
    data |= SAPMCTL_LOCK;
    CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, SAPMCTL_PCU_FUN1_REG, data);
    RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, SAPMCTL_PCU_FUN1_REG, &Size);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  }

  //DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG);
  data |= PPCCC_LOCK;
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );

  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, P_STATE_LIMITS_PCU_FUN0_REG);
  data |= PPCCC_LOCK;
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, P_STATE_LIMITS_PCU_FUN0_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, P_STATE_LIMITS_PCU_FUN0_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );

  // PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG
  if (ppm->Setup->AdvPwrMgtCtlFlags & TURBO_LIMIT_CSR_LOCK) {
    data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG);
    data |= POWER_LIMIT_LOCK;
    CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG, data);
    RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG, &Size);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  }

  // CONFIG_TDP_CONTROL_PCU_FUN3_REG
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, CONFIG_TDP_CONTROL_PCU_FUN3_REG);
  data |= BIT31; // LOCK BIT
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, CONFIG_TDP_CONTROL_PCU_FUN3_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, CONFIG_TDP_CONTROL_PCU_FUN3_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data);

  return;
}

/**

    Set LOCK bit in MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL, MSR_DRAM_PLANE_POWER_LIMIT, and MSR_PRIMARY_PLANE_TURBO_POWER_LIMIT.

    NOTE: This routine should be specifically used to program MSR. There has been a separate routine created for programming CSR's.

    @param PPMPolicy Pointer to PPM Policy protocol instance

    @retval EFI_SUCCESS

**/
VOID
PpmSetMsrLockBit (
    EFI_CPU_PM_STRUCT   *ppm
    )
{

  MSR_REGISTER  TempMsr;
  UINTN         ProcessorNumber = 0;

  mMpService->WhoAmI(mMpService, &ProcessorNumber);

  // Program MSR_TURBO_POWER_LIMIT LOCK bit
  TempMsr.Qword = AsmReadMsr64 (MSR_TURBO_POWER_LIMIT);
  if (ppm->Setup->TurboPowerLimit.Dwords.High & POWER_LIMIT_LOCK) {
    TempMsr.Dwords.High |= POWER_LIMIT_LOCK;
    AsmWriteMsr64 (MSR_TURBO_POWER_LIMIT, TempMsr.Qword);
    WriteRegisterTable(ProcessorNumber, Msr, MSR_TURBO_POWER_LIMIT, 0, 64, TempMsr.Qword);
  }


  if (ppm->Setup->OverclockingLock) {
    TempMsr.Qword = AsmReadMsr64 (MSR_FLEX_RATIO);
    if (TempMsr.Qword & OC_SUPPORTED) {              // if non-zero then the part support Overclocking
      TempMsr.Dwords.Low |= OC_LOCK;                 // cannot set the lock bit if OC is not supported (gp fault)
      AsmWriteMsr64 (MSR_FLEX_RATIO, TempMsr.Qword);
      WriteRegisterTable(ProcessorNumber, Msr, MSR_FLEX_RATIO, OC_LOCK_SHIFT, 1, TRUE);
    }
  }

  // Program MSR_POWER_CTL LOCK bit
  TempMsr.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  if (ppm->Setup->PowerCtl.Dwords.Low & PROCHOT_LOCK) {
    TempMsr.Dwords.Low |= PROCHOT_LOCK;
    AsmWriteMsr64 (MSR_POWER_CTL, TempMsr.Qword);
    WriteRegisterTable(ProcessorNumber, Msr, MSR_POWER_CTL, PROCHOT_LOCK_SHIFT, 1, TRUE);
  }


  // Program MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL LOCK bit
  TempMsr.Qword = AsmReadMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL);
  if (ppm->Setup->PP0CurrentCfg.Dwords.Low & PPCCC_LOCK) {
    TempMsr.Dwords.Low |= PPCCC_LOCK;
    AsmWriteMsr64 (MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL, TempMsr.Qword);

    // Save Lock Bit for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL, PPCCC_LOCK_SHIFT, 1, TRUE);
  }

  // Program MSR_MISC_PWR_MGMT LOCK bit
  TempMsr.Qword = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
  if(!(TempMsr.Dwords.Low & B_EFI_MSR_MISC_PWR_MGMT_LOCK)) {
    TempMsr.Dwords.Low |= B_EFI_MSR_MISC_PWR_MGMT_LOCK;
    AsmWriteMsr64 (MSR_MISC_PWR_MGMT, TempMsr.Qword);  // MSR 1AA:Bit13 write
  }

  return;
}

/**

    Set LOCK bit in MSR_PMG_CST_CONFIG_CONTROL

    NOTE: This routine should be specifically used to program MSR.

    @param PPMPolicy Pointer to PPM Policy protocol instance

    @retval EFI_SUCCESS

**/
VOID
PpmSetMsrCstConfigCtrlLockBit (
    EFI_CPU_PM_STRUCT   *ppm
    )
{
  MSR_REGISTER TempMsr;

  if(ppm->Setup->AdvPwrMgtCtlFlags & CPU_MSR_LOCK) {
    //
    // Program MSR_PMG_CST_CONFIG_CONTROL LOCK bit if not set
    //
    TempMsr.Qword = AsmReadMsr64 (MSR_PMG_CST_CONFIG_CONTROL);
    if(!(TempMsr.Dwords.Low & B_EFI_CST_CONTROL_LOCK)) {
      TempMsr.Dwords.Low |= B_EFI_CST_CONTROL_LOCK;
      AsmWriteMsr64 (MSR_PMG_CST_CONFIG_CONTROL, TempMsr.Qword);
    }
  }

}

/**

    This routine will program BIOS_REST_CPL CSR bits per phase input.

    @param EFI_CPU_PM_STRUCT

    @retval EFI_SUCCESS

**/
VOID
Program_Bios_Reset_Cpl (
  EFI_CPU_PM_STRUCT *ppm
   )
{
  UINT32 data;
  UINT64 RegAddr = 0;
  UINT8  Size = 4;
  UINT8  phase;
  UINT8  SocketNumber;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  UINT8  TimeoutCounter;

  phase = ppm->Info->Bios_Reset_Cpl_Phase;
  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  //BIOS_RESET_CPL_PCU_FUN1_REG
  data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_RESET_CPL_PCU_FUN1_REG);

  switch(phase) {
    case 0:
      data |= BIOS_RSTCPL0_DONE;
      break;
    case 1:
      data |= BIOS_RSTCPL1_MCALDONE;
      break;
    case 2:
      data |= BIOS_RSTCPL2_RCOMPDONE;
      break;
    case 3:
      //
      // Set BIOS Init Done in BIOS, in Reset_CPl (CSR), to commuicate to Pcode to start PM flows
      //
      data |= BIOS_RSTCPL3_PMCFGDONE; // PM Init Done
      break;
    case 4:
      data |= BIOS_RSTCPL4_MEMCFGDONE;// MemCFG Done,
      break;
    default:
      ASSERT(0);
      break;
  }

  DEBUG ( (EFI_D_ERROR, " \n:PM - BIOS_RESET_CPL: write CPL%d on #S%d into BIOS_RESET_CPL_PCU_FUN1_REG\n\n", phase, SocketNumber) );
  CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, BIOS_RESET_CPL_PCU_FUN1_REG, data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, BIOS_RESET_CPL_PCU_FUN1_REG, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data );
  TimeoutCounter = 5;
  if (IsSimicsPlatform() == FALSE) {
    
    // spin here if pcode fails to ACK or timeout of 5ms
    do {
      data = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_RESET_CPL_PCU_FUN1_REG);
          MicroSecondDelay(1000);
          TimeoutCounter--;
    }while (((data & (1 << (phase + 8))) == 0) && (TimeoutCounter > 0));
  }
    if (TimeoutCounter == 0) {
      DEBUG ( (EFI_D_ERROR, " \n:PM - Pcode fails to ACK on Socket%d\n\n", SocketNumber) );
    }
}

/**

  This routine is called to program MAILBOX_BIOS_CMD_EADR. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramB2PForceUncoreAndMeshRatio (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 Tempdata = 0;
  UINT8  SocketNumber;
  UINT8  ch;
  UINT8  mc;
  BOOLEAN ddrtDimmFound = FALSE;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;
  MCDDRTCFG_MC_MAIN_STRUCT      mcDdrtCfg;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  //
  // SKX A0 sequence workaround for WFR buffer
  //
  if (ppm->Info->CpuStepping < B0_REV_SKX) {
    PM_MBX_DATA |= UNCORE_MESH_RATIO_HARDCODE_PN; //hardcode ratio: P1 for Core, and 700MHz for Mesh
    PM_MBX_CMD = (UINT32)MAILBOX_BIOS_CMD_SET_CORE_MESH_RATIO;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);

    if (Tempdata > 0) {
      DEBUG ( (EFI_D_ERROR, " \n\n :: !!! SKX L0 WFR WA failed to force Core & Mesh ratio using B2P CMD in current socket (%d) !!!. \n\n", SocketNumber) );
    }

    PM_MBX_DATA |= UNCORE_MESH_RATIO_HARDCODE_P1; //hardcode ratio: P1 for Core, and P1 for Mesh
    PM_MBX_CMD = (UINT32)MAILBOX_BIOS_CMD_SET_CORE_MESH_RATIO;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);

    if (Tempdata > 0) {
      DEBUG ( (EFI_D_ERROR, " \n\n :: !!! SKX L0 WFR WA failed to force Core & Mesh ratio using B2P CMD in current socket (%d) !!!. \n\n", SocketNumber) );
    }
  }

  //
  // 2LM uclk < dclk WA not needed for SKX H0 or later
  //
  if (ppm->Info->CpuStepping < H0_CPU_STEP) {
    //
    // find out if there is at least 1 ddrt dimm populated in the current socket
    //
    for (mc = 0; mc < MAX_MC; mc++) {
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        mcDdrtCfg.Data = CpuCsrAccess->ReadCpuCsr(SocketNumber, NODECH_TO_SKTCH(SKT_TO_NODE(SocketNumber, mc), ch), MCDDRTCFG_MC_MAIN_REG); 
        DEBUG ( (EFI_D_ERROR, " Socket:%d; MC:%d; Ch:%d; mcDdrtCfg.Data = %d\n", SocketNumber, mc, ch, mcDdrtCfg.Data) );
        if ((mcDdrtCfg.Bits.slot0 == 1) || (mcDdrtCfg.Bits.slot1 == 1)) {
          ddrtDimmFound = TRUE;
          break;
        }
      } // ch loop
      if (ddrtDimmFound == TRUE) {
        break;
      }
    } // mc loop

    // atleast 1 ddrt dimm found; send it to pcode
    // 5370322 CLONE from skylake_server: Uclk < dclk WA only happening for systems configured for 2LM 
    if (ddrtDimmFound) {
      PM_MBX_CMD = (UINT32)MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE;
      PM_MBX_DATA = ddrtDimmFound;

      Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
      if (Tempdata == MAILBOX_BIOS_ERROR_CODE_INVALID_WORKAROUND) {
        DEBUG ( (EFI_D_ERROR, " \n\n :: !!! SKX L0 WA failed to enforce uclk >= dclk in a DDRT dimm in current socket (%d) !!!. \n\n", SocketNumber) );
      } else {
        DEBUG ( (EFI_D_ERROR, " \n\n :: !!! SKX L0 WA enforced uclk >= dclk in a DDRT dimm in current socket (%d) !!!. \n\n", SocketNumber) );
      }
    } // ddrtDimmFound
  }
}

/**

  This routine is called to program MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE;. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramB2PMiscWorkaroundEnable (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 Tempdata = 0;
  UINT8  SocketNumber;

  SocketNumber = ppm->Info->SocketNumber;

  PM_MBX_CMD = (UINT32)MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE;

  if (ppm->Setup->AdvPwrMgtCtlFlags & SPT_PCH_WORKAROUND) {
  }

  if (ppm->Setup->AdvPwrMgtCtlFlags & VCCSA_VCCIO_DISABLE) {
    PM_MBX_DATA = MAILBOX_BIOS_VCCSA_VCCIO_DISABLE;

    DEBUG((EFI_D_INFO, " \n:PM: Set B2P MISC_WORKAROUND_ENABLE(0x%x) for VCCSA and VCCIO Disable, Value = %x in S#%d\n", MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE, PM_MBX_DATA, SocketNumber));

    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if (Tempdata == MAILBOX_BIOS_ERROR_CODE_INVALID_WORKAROUND) {
      DEBUG((EFI_D_ERROR, " \n :: B2P command not supported.  Failed to program VCCSA and VCCIO Disable in current socket (%d) !!!  Error code: %d \n\n", SocketNumber, Tempdata));
    }
  }
}
