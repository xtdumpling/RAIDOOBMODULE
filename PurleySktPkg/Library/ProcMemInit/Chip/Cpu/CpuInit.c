//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix : Fixed CPU cores still be disabled after change Core Disable Bitmap back to 0 problem.
//    Reason  : RC coding issue.
//    Auditor : Jacker Yeh
//    Date    : Aug/18/2017
//
//****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005-2017, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file CpuInit.c

  Cpu Initialization Module.

**/

#include "ProcMemInit.h"
#include "KtiHost.h"
#include "RcRegs.h"
#include "SysHostChip.h"

//
// Function Declarations
//
//
// Function Implementations
//
/**

    Get bitmap of available max cores for the given CPU package

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID

    @retval AvailCoresMask - UINT32  bitmap of available max cores for the CPU package

**/
UINT32
GetAvailableCoresMask (
  USRA_ADDRESS  *RegisterId
  )
{

  FUSED_CORES_PCU_FUN3_STRUCT FusedCoresMaskPcuFunc3;

  RegisterId->Csr.Offset = FUSED_CORES_PCU_FUN3_REG;
  RegisterRead (RegisterId, &FusedCoresMaskPcuFunc3.Data);
  return (UINT32)FusedCoresMaskPcuFunc3.Bits.fused_core_mask;
}

/**

    Get smt capability for the given CPU package

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID

    @retval SmtCapability - UINT8      if non-zero, then CPU is smt-capable

**/
UINT8
GetSmtCapability (
  USRA_ADDRESS  *RegisterId
  )
{
  SMT_CONTROL_PCU_FUN3_STRUCT  SmtControlPcuFunc3;

  RegisterId->Csr.Offset = SMT_CONTROL_PCU_FUN3_REG;
  RegisterRead (RegisterId, &SmtControlPcuFunc3.Data);
  return (UINT8) SmtControlPcuFunc3.Bits.fused_smt_capability;
}

/**

    Get bitmap of resolved cores for the given CPU package

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID

    @retval ResolvedCoreMask  UINT32     - bitmap of resolved cores

**/
UINT32
GetResolvedCores (
  USRA_ADDRESS  *RegisterId
  )
{

  RESOLVED_CORES_PCU_FUN3_STRUCT ResovedCoresMaskPcuFunc3;

  RegisterId->Csr.Offset = RESOLVED_CORES_PCU_FUN3_REG;
  RegisterRead (RegisterId, &ResovedCoresMaskPcuFunc3.Data);
  return (UINT32)ResovedCoresMaskPcuFunc3.Bits.core_mask;

}



VOID Set_FlexRatio_Msr(
    struct sysHost     *host,
    UINT8              socket,
      UINT32             FlexRatioCsrData
    )

  /**

    Program socket-scope MSR0x194.FlexRatio field based on input  FlexRatioCsrData for the given CPU socket

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID
    @param FlexRatioCsrData:   bit[15:8]= FlexRatio,   bit[16] = Flex_Ratio_En,   all other bits are 0's

    @retval MSR 0x194.flex_ratio field  programmed on the given socket
  **/
{
    UINT64_STRUCT msrReg;

      msrReg = ReadMsrPipe(host, socket, MSR_CLOCK_FLEX_MAX);
      msrReg.lo |= FlexRatioCsrData;
      WriteMsrPipe(host, socket, MSR_CLOCK_FLEX_MAX, msrReg);
}




UINT32 Get_FlexRatio_Msr(
      struct sysHost     *host,
      UINT8              socket
      )

    /**

      Reads socket-scope MSR0x194 [31:0] of a given CPU socket

      @param host:    Pointer to sysHost structure on stack
      @param socket:  CPU socket ID

        @retval MSR0x194 bit[31:0]
    **/
{
    UINT64_STRUCT msrReg;

      msrReg = ReadMsrPipe(host, socket, MSR_CLOCK_FLEX_MAX);
      return msrReg.lo;
}




/**

  Executed by SBSP only. Program FlexRatio for all CPUs
  @param host:  pointer to sysHost structure on stack

  @retval FLEX_RATIO CSRs programmed
  @retval host->var.common.resetRequired updated (if reset is needed)

**/
CPU_STATUS
ProgramProcessorFlexRatio (
  struct sysHost             *host
  )
{
  UINT8   socket;
  UINT8   resetNeeded = 0;
  UINT32  csrReg;
  UINT32  targetRatio = 0;
  UINT32  commonMaxRatio;
  UINT32  commonMinRatio;
  UINT32  RatioChangeFlag;
  UINT32  MaxNonTurbo;
  UINT8   sbspSktId = GetSbspSktId(host);
  UINT32  ConfigurableTDP;
  UINT32  ConfigTDPCtrl = 0;
  UINT32  ConfigTDPCtrlVal = 0;
  USRA_ADDRESS  RegisterId;
  UINT32        TempData32;


  USRA_CSR_OFFSET_ADDRESS (RegisterId, sbspSktId, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  rcPrintf ((host, "\n:: ProgramProcessorFlexRatio()\n"));

  //
  //  For Intel PPV test only: Do not touch power-on default ratio of all CPU sockets if selected in BIOS Setup.
  //  This allows each CPU socket to run at different default ratio/freq as-is.
  //  CAUTION:  This usage is out-of-spec, so use at your own risk.
  //
  if (host->setup.cpu.AllowMixedPowerOnCpuRatio) {
      rcPrintf ((host, "  :: setup.cpu.AllowMixedPowerOnCpuRatio = Enabled. Skip.\n"));
    return CPU_SUCCESS;
  }

  // -------------------------------------------------------------------------------------------
  //  Configure all CPU sockets to a common Max Non-turbo Ratio by programming FLEX_RATIO CSR
  // -------------------------------------------------------------------------------------------

  //
  // Calculate the common Target_Ratio among all CPU sockets to ensure that
  //  Common_Min_Ratio <= Target_Ratio <= Common_Max_Ratio.
  //
  if (host->setup.cpu.flexRatioEn) {
    targetRatio = (UINT32) host->setup.cpu.flexRatioNext;
  }

  RegisterId.Csr.Offset = PLATFORM_INFO_N0_PCU_FUN0_REG;
  RegisterRead (&RegisterId, &TempData32);
  commonMaxRatio = (TempData32 >> 8) & 0x000000ff;
  MaxNonTurbo = commonMaxRatio;

  RegisterId.Csr.Offset = PLATFORM_INFO_N1_PCU_FUN0_REG;
  RegisterRead (&RegisterId, &TempData32);
  commonMinRatio = (TempData32 >> 8) & 0x000000ff;

  // get BIOS knob setting
  ConfigTDPCtrl = host->setup.cpu.ConfigTDPLevel;

  // Check Config TDP feature

  RegisterId.Csr.Offset = CONFIG_TDP_CONTROL_PCU_FUN3_REG;
  RegisterRead (&RegisterId, &TempData32);
  csrReg = TempData32 & 0x3;
  RegisterId.Csr.Offset = PLATFORM_INFO_N1_PCU_FUN0_REG;
  RegisterRead (&RegisterId, &TempData32);
  ConfigurableTDP = (TempData32 >> 1) & 0x00000003;

  if ((ConfigurableTDP > 0) && (ConfigTDPCtrl >= 0)) {
      // Support Config TDP, program MSR_CONFIG_TDP_CONTROL
      //
      RegisterId.Csr.Offset = CONFIG_TDP_CONTROL_PCU_FUN3_REG;
      RegisterRead (&RegisterId, &TempData32);
      csrReg = TempData32 & ~(0x00000003);
      if (ConfigTDPCtrl == 1) {
          //commonMaxRatio = (ReadCpuPciCfgEx (host, sbspSktId, 0, CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG) & B_CONFIG_TDP_LVL_RATIO) >> N_CONFIG_TDP_LVL_RATIO;
          RegisterId.Csr.Offset = CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG;
          RegisterRead (&RegisterId, &TempData32);
          commonMaxRatio = (TempData32 >> 16) & 0x000000ff;
          csrReg |= 1;

  rcPrintf ((host, "::   SBSP - MAX_NON_TURBO_LIM_RATIO = %d [Config TDP level = 1]\n", commonMaxRatio));

      }

      if ((ConfigTDPCtrl == 2) && (ConfigurableTDP == 2)) {
          //commonMaxRatio = (ReadCpuPciCfgEx (host, sbspSktId, 0, CONFIG_TDP_LEVEL2_N0_PCU_FUN3_REG) & B_CONFIG_TDP_LVL_RATIO) >> N_CONFIG_TDP_LVL_RATIO;
          RegisterId.Csr.Offset = CONFIG_TDP_LEVEL2_N0_PCU_FUN3_REG;
          RegisterRead (&RegisterId, &TempData32);
          commonMaxRatio = (TempData32 >> 16) & 0x000000ff;
          csrReg |= 2;

  rcPrintf ((host, "::   SBSP - MAX_NON_TURBO_LIM_RATIO = %d\n [Config TDP level = 2]\n", commonMaxRatio));

      }

      RegisterId.Csr.Offset = CONFIG_TDP_CONTROL_PCU_FUN3_REG;
      RegisterWrite (&RegisterId, &csrReg);
      RegisterRead (&RegisterId, &TempData32);
      ConfigTDPCtrlVal = TempData32 & 0x00000003;
  }
  
  //
  // Collect CPU: Max Non-turbo Ratio and Maximum Efficiency Ratio
  //
  host->var.common.MaxCoreToBusRatio[sbspSktId] = (UINT8) commonMaxRatio;
  host->var.common.MinCoreToBusRatio[sbspSktId] = (UINT8) commonMinRatio;
  
//  rcPrintf ((host, "::   SBSP - MAX_NON_TURBO_LIM_RATIO = %d,  MAX_EFFICIENCY_RATIO = %d\n", commonMaxRatio, commonMinRatio));

  RatioChangeFlag = 0;

  if   (MaxNonTurbo != commonMaxRatio) {
    RatioChangeFlag = 1;
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (socket == sbspSktId)   continue;  // SBSP already done above
    if ((host->var.kti.RasInProgress == FALSE && (host->var.common.socketPresentBitMap & (BIT0 << socket))) ||
      (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == socket)) {
         RegisterId.Csr.SocketId = socket;
         RegisterId.Csr.Offset = PLATFORM_INFO_N0_PCU_FUN0_REG;
         RegisterRead (&RegisterId, &TempData32);
         MaxNonTurbo = (TempData32 >> 8) & 0x000000ff;
         csrReg = MaxNonTurbo;


         RegisterId.Csr.Offset = PLATFORM_INFO_N1_PCU_FUN0_REG;
         RegisterRead (&RegisterId, &TempData32);

         if ( ConfigurableTDP == ((TempData32 >> 1) & 0x00000003) ) {
         ConfigurableTDP = (TempData32 >> 1) & 0x00000003;
           if ( (ConfigurableTDP > 0) && (ConfigTDPCtrlVal >= 0) ) {

             RegisterId.Csr.Offset = CONFIG_TDP_CONTROL_PCU_FUN3_REG;
             RegisterRead (&RegisterId, &TempData32);

             csrReg = (TempData32 & ~(0x00000003)) + ConfigTDPCtrlVal;
             RegisterWrite (&RegisterId, &csrReg);
              csrReg = MaxNonTurbo;

              if (ConfigTDPCtrlVal == 1) {

                  RegisterId.Csr.Offset = CONFIG_TDP_LEVEL1_N0_PCU_FUN3_REG;
                  RegisterRead (&RegisterId, &TempData32);
                  csrReg = (TempData32 >> 16) & 0xff;
              }

              if (ConfigTDPCtrlVal == 2) {

                  RegisterId.Csr.Offset = CONFIG_TDP_LEVEL2_N0_PCU_FUN3_REG;
                  RegisterRead (&RegisterId, &TempData32);
                  csrReg = (TempData32 >> 16) & 0xff;
              }
           }
         }
         // APTIOV_SERVER_OVERRIDE_RC_START
         host->var.common.MaxCoreToBusRatio[socket] = (UINT8) csrReg; // max core to bus ratio for the particular socket
         // APTIOV_SERVER_OVERRIDE_RC_END

         //
         // CSR bit[15:8] = MAX_NON_TURBO_LIM_RATIO, i.e., Max non-turbo ratio
         //
         if (commonMaxRatio != csrReg) {
           RatioChangeFlag = 1;

           if (csrReg < commonMaxRatio) {
             commonMaxRatio = csrReg;

             if (host->var.kti.RasInProgress == TRUE)
              return CPU_FAILURE;   // New socket's max ratio should be greater than or equal to that of sbsp
           }
         }
/*
         rcPrintf ((host, "::   Socket %x - MAX_NON_TURBO_LIM_RATIO = %x\n", socket, csrReg));
*/
         RegisterId.Csr.Offset = PLATFORM_INFO_N1_PCU_FUN0_REG;
         RegisterRead (&RegisterId, &TempData32);
         csrReg = (TempData32 >> 8) & 0x000000ff;
         // APTIOV_SERVER_OVERRIDE_RC_START
          host->var.common.MinCoreToBusRatio[socket] = (UINT8) csrReg; // min core to bus ratio for the particular socket
         // APTIOV_SERVER_OVERRIDE_RC_END

         //
         // CSR bit[15:8] = MAX_EFFICIENCY_RATIO, i.e., Min ratio
         //
         if (csrReg > commonMinRatio) {
           RatioChangeFlag = 1;
           commonMinRatio = csrReg;

           if (host->var.kti.RasInProgress == TRUE)
            return CPU_FAILURE; // New socket's min ratio should be less than or equal to that of sbsp
         }
      // APTIOV_SERVER_OVERRIDE_RC_START
      //
      // Collect CPU: Max Non-turbo Ratio and Maximum Efficiency Ratio
      //
      //host->var.common.MaxCoreToBusRatio[socket] = (UINT8) commonMaxRatio;
      //host->var.common.MinCoreToBusRatio[socket] = (UINT8) commonMinRatio;
      // APTIOV_SERVER_OVERRIDE_RC_END
/*
         rcPrintf ((host, "::   Socket %x - MAX_EFFICIENCY_RATIO = %x\n", socket, csrReg));
*/
   }
  }

  //
  // Now commonMaxRatio, commonMinRatio  contain the common ratio values for all CPU sockets.
  //
  if ((host->setup.cpu.flexRatioEn == 0) & RatioChangeFlag)  {
      //
      // BIOS enforces common targetRatio regardless of Setup option
      //
      targetRatio = commonMaxRatio;
  }

  if (host->setup.cpu.flexRatioEn && (targetRatio < commonMaxRatio) && (targetRatio > commonMinRatio))  {
      RatioChangeFlag = 1;
  }

  //HSD 3614888: Disabling EIST and CSTATES should leave CPUs running at P1
  if (host->setup.cpu.EnableGv == 0)  {
    targetRatio = commonMaxRatio;
    if (targetRatio != MaxNonTurbo) {
      RatioChangeFlag = 1;
    }
  }

  //
  // Validata  that targetRatio is in the right range
  //
  if (host->setup.cpu.flexRatioEn && (targetRatio > commonMaxRatio)) {
     rcPrintf ((host, "\n::   Given targetRatio out-of-range. Forcing it to commonMaxRatio %d\n", commonMaxRatio));
     if (MaxNonTurbo != commonMaxRatio) {
       RatioChangeFlag = 1;
     }
     targetRatio = commonMaxRatio;
  }

  if (host->setup.cpu.flexRatioEn && (targetRatio < commonMinRatio)) {
     rcPrintf ((host, "\n::   Given targetRatio out-of-range. Forcing it to commonMinRatio %d\n", commonMinRatio));
     RatioChangeFlag = 1;
     targetRatio = commonMinRatio;
  }
/*
         rcPrintf ((host, "::   RatioChangeFlag = %x\n", RatioChangeFlag));
         rcPrintf ((host, "::   Cpu Ratio Override Enable = %x\n", host->setup.cpu.flexRatioEn));
         rcPrintf ((host, "::   targetRatio = %x\n", targetRatio));
         rcPrintf ((host, "::   commonMaxRatio = %x\n\n", commonMaxRatio));
         rcPrintf ((host, "::   commonMinRatio = %x\n\n", commonMinRatio));
*/

    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if ((host->var.kti.RasInProgress == FALSE && (host->var.common.socketPresentBitMap & (BIT0 << socket)) && RatioChangeFlag ) ||
      (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == socket && RatioChangeFlag)) {

         // csrReg contains current FlexRatio
         if (RatioChangeFlag) {
           //
           // read FLEX_RATIO CSR
           //
           RegisterId.Csr.SocketId = socket;
           RegisterId.Csr.Offset = FLEX_RATIO_N0_PCU_FUN3_REG;
           RegisterRead (&RegisterId, &csrReg);
           //
           // Check to see if the target ratio is already set.
           //
           if ((csrReg & 0x0001FF00) == ((targetRatio << 8) | BIT16)) {
             rcPrintf ((host, "::  targetRatio previously set:  S%x FLEX_RATIO (MSR 0x194) = %x\n", socket, csrReg));
             continue;
           }
           csrReg = (csrReg & 0xfffe00ff) | (targetRatio << 8) | BIT16;
           //
           // CSR bit[15:8] = desired FLEX_RATIO, bit[16]=enable
           // program target flex ratio
           //
           RegisterWrite (&RegisterId, &csrReg);
           //
           // need a reset
           //
           resetNeeded |= POST_RESET_WARM;
           rcPrintf ((host, "::    S%x FLEX_RATIO (MSR 0x194) = %x\n", socket, csrReg));

      }
    }
  }

  //
  // save current FLEX_RATIO CSR for S3 resume
  //
  RegisterId.Csr.SocketId = sbspSktId;
  RegisterId.Csr.Offset = FLEX_RATIO_N0_PCU_FUN3_REG;
  RegisterRead (&RegisterId, &(host->nvram.cpu.flexRatioCsr));

  if (resetNeeded) {
    host->var.common.resetRequired |= resetNeeded;
  }

  return CPU_SUCCESS;
}

/**

  Executed by SBSP only. Program DesiredCores, SMTDisable for all CPUs
  @param host:  pointer to sysHost structure on stack

  @retval CORE_OFF_MASK CSRs programmed
  @retval host->var.common.resetRequired updated (if reset is needed)

**/
VOID
SetActiveCoresAndSmtEnableDisable (
  struct sysHost             *host
  )

{
  UINT8   socket;
  UINT8   resetNeeded = 0;
  UINT8   smtCapable  = 0;
  UINT8   smtEnabled  = 0;
  UINT32  availCoresMask;
  UINT32  resolvedCoresMask;
  UINT32  bistResultMask;
  UINT32  activeCoreOption;
  CSR_DESIRED_CORES_PCU_FUN1_STRUCT       csrDesiredCoresCurrent, csrDesiredCoresNew;
  CSR_DESIRED_CORES_CFG2_PCU_FUN1_STRUCT  csrDesiredCoresCfg2Current, csrDesiredCoresCfg2New;
  UINT8   PoisonEnStatus;
  UINT64_STRUCT msrData;

  USRA_ADDRESS  RegisterId;
  UINT32        TempData32;

  USRA_CSR_OFFSET_ADDRESS(RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  //
  // -----------------------------------------------------------------------------------------------------
  // Configure number of active(enabled) cores and SMT(HT) enable/disable based on input parameters
  //   - Assume that this is executed on a cold boot path (after a power good reset)
  //   - Read CSR RESOLVED_CORES_MASK[7:0] and RESOLVED_CORES_MASK2[7:0] (for IVT CPU) bitmap to determine
  //      how many/which cores are available in a CPU pacakge.
  //     (example, a bitmap of 00111111b indicates that 6 cores are available)
  //   - Use input parameter host->setup.cpu.CoreDisableMask[]  for each socket independently
  //   - Use input parameter host->setup.cpu.smtEnable and RESOLVED_CORES_MASK[24] SMT_CAPABILITY for setting smtDisable bit.
  //      Note that smt enable/disable symetry is maintained among all CPU sockets.
  // -----------------------------------------------------------------------------------------------------
  //

  //
  // skip if Simics
  //

  if (host->var.common.emulation)  { return; }

  rcPrintf ((host, "\n:: SetActiveCoresAndSmtEnableDisable()\n"));

  //
  // Allow this CPU BIST error exception to avoid being promoted to fatal
  //
  SetPromoteWarningException (host, ERROR_CPU_BIST, ERROR_CPU_BIST_MINOR_SOME_BISTRESULTMASK);

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->var.kti.RasInProgress == FALSE && (host->var.common.socketPresentBitMap & (BIT0 << socket))) ||
      (host->var.kti.RasInProgress == TRUE && host->var.kti.RasSocId == socket)) {

      bistResultMask    = 0;
      csrDesiredCoresNew.Data = 0;
      resetNeeded       = 0;
      RegisterId.Csr.SocketId = socket;

      availCoresMask    = GetAvailableCoresMask(&RegisterId);
      smtCapable        = GetSmtCapability(&RegisterId);
      resolvedCoresMask = GetResolvedCores(&RegisterId) & availCoresMask;
      RegisterId.Csr.Offset = CSR_DESIRED_CORES_PCU_FUN1_REG;
      RegisterRead (&RegisterId,  &csrDesiredCoresCurrent.Data);
      RegisterId.Csr.Offset = CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG;
      RegisterRead (&RegisterId, &csrDesiredCoresCfg2Current.Data);
       
      smtEnabled = csrDesiredCoresCfg2Current.Bits.smt_disable ? 0 : 1;
      //
      // Read BIST_RESULT CSR to get BIST failure info
      //
      if(host->setup.cpu.CheckCpuBist){
        RegisterId.Csr.Offset = BIST_RESULTS_UBOX_DFX_REG;
        RegisterRead (&RegisterId, &TempData32);
        bistResultMask = TempData32 & resolvedCoresMask;
        //convert Bist fail to 1 and mask out bits that due to core already disabled through Desired_core
        bistResultMask ^= resolvedCoresMask;
      }
      activeCoreOption = host->setup.cpu.CoreDisableMask[socket] & availCoresMask;

      rcPrintf ((host, ":: S%d availCoresMask=%x\n", socket,availCoresMask));
      rcPrintf ((host, ":: S%d resolvedCoresMask=%x\n", socket,resolvedCoresMask));
      rcPrintf ((host, ":: S%d Desired_Cores CSR=%x\n", socket,csrDesiredCoresCurrent.Data));
      rcPrintf ((host, ":: S%d bistResultMask=%x\n", socket,bistResultMask));
      rcPrintf ((host, ":: S%d activeCoreOption=%x\n", socket,activeCoreOption));

      PoisonEnStatus = 1;
      // Check Poison Capability, if yes, check poison enable bit in MCG_CONTAIN MSR
      if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
        // Check Poison Capable
        msrData = ReadMsrPipe(host, socket, MSR_IA32_MCG_CAP);
        if (msrData.lo & B_MSR_MCG_CAP_GCM) {
          // Check Poison Enable
          msrData = ReadMsrPipe(host, socket, MSR_MCG_CONTAIN);
          if (msrData.lo & B_MSR_MCG_CONTAIN_PE) {
            PoisonEnStatus = 0;
          }
        }
      } 

      rcPrintf ((host, ":: S%d PoisonEnStatus=%x\n", socket,PoisonEnStatus));

      //
      // If BIST failure is found, create error log entry
      //
      if ( bistResultMask && (bistResultMask != resolvedCoresMask)) {
        rcPrintf ((host, "::  #S%d BIST error found and logged, bistResultMask = %x\n", socket, bistResultMask));
        LogCpuError (host, ERROR_CPU_BIST, ERROR_CPU_BIST_MINOR_SOME_BISTRESULTMASK, socket, bistResultMask);
      }

      //
      // Check if all cores of the socket failed BIST
      //
      if ( bistResultMask == resolvedCoresMask ) {
        rcPrintf ((host, "::CPU on #S%d BIST errors cause all cores disabled!\n", socket));
        if (PoisonEnStatus == 0) {
          rcPrintf ((host, ":: Poison is enabled, skip BIST deadloop now!\n"));
          bistResultMask = 0;
        }
        if (PoisonEnStatus) {
          FatalError (host, ERROR_CPU_BIST, ERROR_CPU_BIST_MINOR_ALL);
          while (bistResultMask);
        }
      }

      csrDesiredCoresNew.Data = 0;
      csrDesiredCoresCfg2New.Data = 0;

      if(((activeCoreOption & csrDesiredCoresCurrent.Bits.core_off_mask) != activeCoreOption) ||  // If core disable option is not subset of desired cores
          (bistResultMask != 0) 
#if SMCPKG_SUPPORT
          || ((activeCoreOption == 0) && (csrDesiredCoresCurrent.Data != 0))
#endif
          ) {                                                                // Or active core BIST failed
        if(csrDesiredCoresCfg2Current.Bits.lock) { //no need to continue
          host->var.common.resetRequired |= POST_RESET_POWERGOOD;
          break;
        }

        csrDesiredCoresNew.Bits.core_off_mask = (activeCoreOption | bistResultMask);
        resetNeeded |= POST_RESET_WARM;
      }

      if((smtEnabled != host->setup.cpu.smtEnable) && smtCapable){  //if reg setting differs with option and HW capable
        if(csrDesiredCoresCfg2Current.Bits.lock) { //no need to continue
          host->var.common.resetRequired |= POST_RESET_POWERGOOD;
          break;
        }
        csrDesiredCoresCfg2New.Bits.smt_disable = (~host->setup.cpu.smtEnable & 1);
        resetNeeded |= POST_RESET_WARM;
      }

      if(resetNeeded) {
        host->var.common.resetRequired |= resetNeeded;
      } else { // not change on core_off_mask
        csrDesiredCoresNew.Data = csrDesiredCoresCurrent.Data;
        csrDesiredCoresCfg2New.Data = csrDesiredCoresCfg2Current.Data;
      }

      if ((csrDesiredCoresCfg2Current.Bits.lock == 0) &&                // CSR is not locked (will defer locking this CSR before hand off to OS)
          (csrDesiredCoresCurrent.Data != csrDesiredCoresNew.Data)) {   // new data != current data
        //
        // Write new data to CSR CSR_DESIRED_CORES
        //
        RegisterId.Csr.Offset = CSR_DESIRED_CORES_PCU_FUN1_REG;
        RegisterWrite (&RegisterId, &csrDesiredCoresNew.Data);
        rcPrintf ((host, "::Writing DesiredCores CSR =%x S%x\n", csrDesiredCoresNew.Data, socket));
      }

      if ((csrDesiredCoresCfg2Current.Bits.lock == 0) &&                  // CSR is not locked (will defer locking this CSR before hand off to OS)
         (csrDesiredCoresCfg2Current.Data != csrDesiredCoresCfg2New.Data)) {   // new data != current data
        //
        // Write new data to CSR CSR_DESIRED_CORES_CFG2
        //
        RegisterId.Csr.Offset = CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG;
        RegisterWrite (&RegisterId, &csrDesiredCoresCfg2New.Data);
        rcPrintf ((host, "::Writing DesiredCoresCfg2 CSR =%x S%x\n", csrDesiredCoresCfg2New.Data, socket));
      }


      // save current DESIRED_CORES CSR for S3 resume
      host->nvram.cpu.desiredCoresCsr[socket] = csrDesiredCoresNew.Data;
      host->nvram.cpu.desiredCoresCfg2Csr[socket] = csrDesiredCoresCfg2New.Data;
    }  //end if socket present

  } //end for socket loop

  return;
}

/**

  Executed by SBSP only. Program MSR_TRACE_HUB_STH_ACPIBAR_BASE
  @param host:  pointer to sysHost structure on stack

**/
VOID
CpuWriteTraceHubAcpiBase (
  struct sysHost             *host,
  IN UINT64_STRUCT TraceHubAcpiBase
  )
{
  UINT64_STRUCT msrData;

  if (host->setup.cpu.PchTraceHubEn) {

    //
    // Check the pass in Trace Hub ACPI base if 256KB alignment.
    //
    if ((TraceHubAcpiBase.lo & (UINT32) V_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK) != 0) {
      RC_ASSERT((TraceHubAcpiBase.lo & (UINT32) V_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK) == 0, ERR_RC_INTERNAL, 0);

      return;
    }

    ///
    /// Set MSR TRACE_HUB_STH_ACPIBAR_BASE[0] LOCK bit for the AET packets to be directed to NPK MMIO.
    ///
    msrData = ReadMsrLocal(MSR_TRACE_HUB_STH_ACPIBAR_BASE);
    if ((msrData.lo & B_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK) == 0) {
      msrData.hi = TraceHubAcpiBase.hi;
      msrData.lo |= (TraceHubAcpiBase.lo + (UINT32) B_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK);
      WriteMsrLocal(MSR_TRACE_HUB_STH_ACPIBAR_BASE, msrData);
      rcPrintf ((host, "\nSet MSR TRACE_HUB_STH_ACPIBAR_BASE[0] in CpuWriteTraceHubAcpiBase as 0x%x %x.\n", msrData.hi, msrData.lo));
    }
  }

  return;
}

#ifdef IA32




/**
  Executed by SBSP only. Init misc uncore CSRs of all CPU sockets

  @param host:  pointer to sysHost structure on stack

  @retval VOID
**/
VOID
UncoreInitMisc (
  struct sysHost     *host
  )

{
    UINT8         socket;
    GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_STRUCT    csrReg;
    UINT64_STRUCT msrData;
    UINT8         MaxClmRatio;

    USRA_ADDRESS  RegisterId;
    USRA_CSR_OFFSET_ADDRESS(RegisterId, 0, 0, 0, CsrBoxInst);
    RegisterId.Attribute.HostPtr = (UINT32)host;
    RegisterId.Csr.Offset = GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_REG;

    rcPrintf ((host, "UncoreInitMisc()\n"));
    //
    // Loop thru all CPU sockets
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
        if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0)  continue;
        //
        //   Program  GLOBAL_PKG_C_S_CONTROL_REGISTER in PCU
        //
        RegisterId.Csr.SocketId = socket;
        RegisterRead (&RegisterId, &(csrReg.Data));
 
        csrReg.Bits.my_nid = socket;
        csrReg.Bits.master_nid = host->nvram.common.sbspSocketId;
        if (socket == host->nvram.common.sbspSocketId) {
            csrReg.Bits.am_i_master = 1;
        }
        RegisterWrite (&RegisterId, &csrReg.Data);
        rcPrintf ((host, " Write Socket 0x%2d GLOBAL_PKG_C_S_CONTROL_REGISTER = %X\n", socket, csrReg.Data));

        //
        //   5331664: BIOS needs to set MIN_CLM_RATIO field in UNCORE_RATIO_LIMIT MSR to a value of 12
        //
        msrData = ReadMsrPipe(host, socket, MSR_UNCORE_RATIO_LIMIT);
        MaxClmRatio = (UINT8)(msrData.lo & MAX_CLM_RATIO_MASK);
        msrData.lo &= 0xFFFF80FF;    // clear Bit[14:8]
        if (!host->setup.cpu.UFSDisable) {
          msrData.lo |= (MIN_CLM_RATIO_DEFAULT << B_MIN_CLM_RATIO_SHIFT);
        } else {
          //
          // To disable UFS, set min clm ratio to equal max clm ratio
          //
          msrData.lo |= (MaxClmRatio << B_MIN_CLM_RATIO_SHIFT);
        }
        WriteMsrPipe(host, socket, MSR_UNCORE_RATIO_LIMIT, msrData);
        rcPrintf ((host, " Write Socket 0x%2d MSR_UNCORE_RATIO_LIMIT.MIN_CLM_RATIO [14:8] = %X\n", socket, (msrData.lo & 0x7F00)));

        //
        //   Program  other uncore CSRs here if needed...
        //
    }
}

/**

  Executed by SBSP only. Check WFR for all socket CPUs
  @param host:  pointer to sysHost structure on stack

  BIOS SETUP option:
     0 = Disabled;  1 = Enabled;  2 = AUTO
**/
VOID
WFRSocketWA(
  struct sysHost             *host
  )
{
  UINT8  socket;
  UINT8  Stack;
  UINT32 Status = 0;
  UINT64_STRUCT msrData;
  UINT32 TempData = 0;
  UINT32 PcodeMailboxStatus = 0;
  PCU_BIOS_SPARE2_PCU_FUN3_STRUCT     pcuBiosSpare2Reg;
  UINT8   MaxClamRatio[MAX_SOCKET];   
  UINT8   MeshMaxRatio[MAX_SOCKET];   
  USRA_ADDRESS RegisterId;

  if (host->setup.WFRWAEnable == 0)  {
    goto WFRWAExit;
  }

  rcPrintf ((host, "\n:: Start WFRSocketWA()...\n"));

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        if (host->var.kti.CpuInfo[socket].CpuRes.StackRes[Stack].Personality == TYPE_MCP) {
          Status = 1;
        }
      }  //Stack for loop
    }  
  }  //socket for loop

  if  (!Status && (host->setup.WFRWAEnable == 2)) {
    goto WFRWAExit;
  }

  Status = 0;

  // Check WFR socket, and prepare MaxClamRatio and MeshMaxRatio
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    MaxClamRatio[socket] = 0; // Init entire array to zero
    MeshMaxRatio[socket] = 0;
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      // populate elements of valid socket with actual values
      rcPrintf ((host, "\nDo WFR WA in current S%2d: \n", socket));

      msrData = ReadMsrPipe(host, socket, MSR_UNCORE_RATIO_LIMIT);
      MaxClamRatio[socket] = (UINT8) (msrData.lo & MAX_CLM_RATIO_MASK) ;

      TempData = (msrData.lo & MAX_CLM_RATIO_MASK);
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_SET_CORE_MESH_RATIO, TempData);
      if (PcodeMailboxStatus) {
         rcPrintf ((host, "\nBIOS failed to request pcode to set UNCORE_RATIO_LIMIT in current S%2d: \n", socket));
         Status = 1;
      } else {
        msrData = ReadMsrPipe(host, socket, MSR_UNCORE_PERF_STATUS);
        MeshMaxRatio[socket] = (UINT8) (msrData.lo & CURRENT_CLM_RATIO_MASK) ;
      }
    }
  }  // for loop

  if (Status) {
    goto WFRWAExit;
  }

  // set UNCORE_RATIO_LIMIT min = max = clm_p1
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      msrData = ReadMsrPipe(host, socket, MSR_UNCORE_RATIO_LIMIT);
      msrData.lo &= 0xFFFF0000;    // clear Bit[15:0]
      msrData.lo |= (MaxClamRatio[socket] + (MaxClamRatio[socket] << B_MIN_CLM_RATIO_SHIFT));
      WriteMsrPipe(host, socket, MSR_UNCORE_RATIO_LIMIT, msrData);
    }  
  } // for loop

  // Program P1 for core and mesh using B2P
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {

      TempData = (UINT32) ((MeshMaxRatio[socket] << 8)  + MaxClamRatio[socket]);
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_SET_CORE_MESH_RATIO, TempData);
      if (PcodeMailboxStatus) {
         rcPrintf ((host, "\nBIOS failed to request pcode to set Core and Mesh ratio in current S%2d: \n", socket));
         Status = 1;
      } 
    }  
  } // for loop

  if (Status) {
    goto WFRWAExit;
  }
  USRA_CSR_OFFSET_ADDRESS(RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  // BIOS sets BIOS_SPARE2.bit 20 
  RegisterId.Csr.Offset = PCU_BIOS_SPARE2_PCU_FUN3_REG;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      RegisterId.Csr.SocketId = socket;
      RegisterRead (&RegisterId, &pcuBiosSpare2Reg.Data);
      pcuBiosSpare2Reg.Data |= BIT20;     //when set, pcode will decouple CLM frequency from core PBM
      RegisterWrite (&RegisterId, &pcuBiosSpare2Reg.Data);
    }  
  } // for loop

  rcPrintf ((host, "\nBIOS completed WFR WA.\n"));

WFRWAExit:
  return;
}

/**

    Executed by SBSP only. Collect package BSP data that is needed to be passed along via sysHost structure

  @param host:  pointer to sysHost structure on stack

  @retval VOID

**/
VOID
CollectPbspData (
  struct sysHost  *host
  )
{
  UINT8   socket;
  UINT32  csrReg;

  USRA_ADDRESS  RegisterId;
  USRA_CSR_OFFSET_ADDRESS(RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;
  RegisterId.Csr.Offset = SR_PBSP_CHECKIN_CSR;
  //
  // Read APIC ID  and stepping from scratch pad register of each PBSP and store them into sysHost struct
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
     if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      RegisterId.Csr.SocketId = socket;
      RegisterRead (&RegisterId, &csrReg);

      //
      // CSR bit[23:8] = APIC ID at reset
      //
      host->var.cpu.packageBspApicID[socket] = (csrReg >> 8) & 0xffff;


      //
      // Clear the APIC ID and Stepping data in scratch pad CSR
      //
      csrReg &= 0x870000ff;
      RegisterWrite (&RegisterId, &csrReg);
     }
  }
}

/**

  Check if VT/TXT is supported, and if a power-good reset is needed
  to unlock the MSR to allow changing the current setting.

  @param pointer to sysHost struct

  @retval host->var.common.resetRequired updated (if reset is needed)

**/
VOID
CheckVtIfNeedReset (
  struct sysHost      *host
  )
{
    UINT32  RegEax;
    UINT32  RegEbx;
    UINT32  RegEcx;
    UINT32  RegEdx;
    UINT64_STRUCT  MsrIa32FeatureControl;
    BOOLEAN CurrentVmxState;

    //
    // Note all logical threads' VT/TXT feature MSRs are configured identically on cold boot path.
    // Here we check if the current config needs to be changed (i.e.,mismatch with BIOS Setup),
    // and if a power-good reset is needed to "unlock" the MSR for that change.
    //

    //
    // Check if VT is supported via CPUID Function 1, which returns
    //    ECX[6] Intel TXT support feature flag
    //    ECX[5] Intel VT  support feature flag
    //

    RegEax = 1; RegEcx = 0;
    GetCpuID (&RegEax, &RegEbx, &RegEcx, &RegEdx);
    if (!(RegEcx & BIT5)) {
      return;
    }

    MsrIa32FeatureControl = ReadMsrLocal (MSR_IA32_FEATURE_CONTROL);

    //
    // MSR_IA32_FEATURE_CONTROL Definition:
    //    Bit 2   Enable VMX outside SMX operation (R/WL): This bit enables Intelr
    //            Virtualization Technology in an environment that may not include
    //            Intelr Trusted Execution Technology support.
    //
    //    Bit 1   Enable VMX inside SMX operation (R/WL): This bit enables Intelr
    //            Virtualization Technology in an environment that includes Intelr
    //            Trusted Execution Technology support.
    //
    //    Bit 0   Lock (R/WO) (1 = locked) When set, locks this MSR from being witten,
    //            writes to this bit will result in GP(0) until an S5 reset occurs.
    //            BIOS must ensure this lock bit is set before boot to OS.
    //


    //
    // If unlocked, no reset needed.
    //
    if ((MsrIa32FeatureControl.lo & BIT0) == 0) {
       return;
    }

    CurrentVmxState = FALSE;
    if ( MsrIa32FeatureControl.lo & BIT2 ) {
      CurrentVmxState = TRUE;
    }

    //
    // Need a reset only if this MSR is locked and VMX state has to be changed.
    //

    if (CurrentVmxState != (BOOLEAN)host->setup.cpu.vtEnable) {
       //
       // We need a power good reset to unlock MSR_IA32_FEATURE_CONTROL.
       //
       host->var.common.resetRequired |= POST_RESET_POWERGOOD;
    }
}


/**

  Executed by SBSP only.
  Restore FLEX_RATIO, DESIRED_CORES CSRS for all CPUs on S3 resume path.

  @param host:  pointer to sysHost structure on stack
                host->nvram.cpu contains valid data to be restored to CSRs.

  @retval FLEX_RATIO, DESIRED_CORES CSRs restored from sysHost->nvram.cpu structure
  @retval host->var.common.resetRequired updated (if reset is needed)

**/
VOID
RestoreProcessorCsrsOnS3Resume (
  struct sysHost             *host
  )
{
  UINT8   socket;
  UINT8   resetNeeded =0;
  UINT32  flexRatioN0PcuFun3;
  UINT32  csrDesiredCoresPcuFun1;
  USRA_ADDRESS  RegisterId;
  USRA_CSR_OFFSET_ADDRESS (RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  //
  //  Intel PPV requirement:  Do not touch power-on default ratio of all CPU sockets if selected in BIOS Setup.
  //  This allows each CPU socket to run at different default ratio/freq as-is.
  //  Desired_CORES CSR is also left alone, since this PPV use case does not involve S3 resume
  //  CAUTION:  This usage is out-of-spec, so use at your own risk.
  //
  if (host->setup.cpu.AllowMixedPowerOnCpuRatio) return;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
     if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
       //
       // restore FLEX_RATIO CSR
       //
       RegisterId.Csr.SocketId = socket;
       RegisterId.Csr.Offset = FLEX_RATIO_N0_PCU_FUN3_REG;
       RegisterRead (&RegisterId, &flexRatioN0PcuFun3);

       if (flexRatioN0PcuFun3 != host->nvram.cpu.flexRatioCsr) {
          RegisterWrite (&RegisterId, &host->nvram.cpu.flexRatioCsr);
          resetNeeded |= POST_RESET_WARM;
       }
       //
       // restore DESIRED_CORES CSR
       //

       RegisterId.Csr.Offset = CSR_DESIRED_CORES_PCU_FUN1_REG;
       RegisterRead (&RegisterId, &csrDesiredCoresPcuFun1);

       if (csrDesiredCoresPcuFun1 != host->nvram.cpu.desiredCoresCsr[socket]) {

          RegisterWrite (&RegisterId, &(host->nvram.cpu.desiredCoresCsr[socket]));
          resetNeeded |= POST_RESET_WARM;
       }

       RegisterId.Csr.Offset = CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG;
       RegisterRead (&RegisterId, &csrDesiredCoresPcuFun1);

       if (csrDesiredCoresPcuFun1 != host->nvram.cpu.desiredCoresCfg2Csr[socket]) {
          RegisterWrite (&RegisterId, &(host->nvram.cpu.desiredCoresCfg2Csr[socket]));
          resetNeeded |= POST_RESET_WARM;
       }
     }
  }
  if (resetNeeded) {
    host->var.common.resetRequired |= resetNeeded;
  }
}

/**

  Executed by System BSP only.
  Common software programming steps needed before warm reset and CPU-only reset.

  @param host - pointer to sysHost structure on stack

Note:

Do not add anything to this routine.  It is called twice for two different purposes.  It is called once prior to
doing the first rest to lock in our frequencies & ratio changes.  BIOS must provide PCODE enough information to
process any RESET_WARN flows.  The seconded time this is called is before we exit PEI.  At this point there is a
separate set of registers that must be programmed prior to BIOS_INIT_Done being set.  This second time we program
this bit Pcode is now ready to start processing PM flows.  Please note that these steps cannot be combined because
when BIOS_INIT_DONE bit is set PCODE locks the BIOS_2_PCODE MBX.

  @retval VOID
**/
VOID
SetBIOSInitDone (
  struct sysHost  *host
  )
{

  UINT8   socket;
  BIOSSCRATCHPAD1_UBOX_MISC_STRUCT biosScratchPad1;
  UINT8 sbspSktId = GetSbspSktId(host);

  USRA_ADDRESS RegisterId;
  USRA_CSR_OFFSET_ADDRESS (RegisterId, sbspSktId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;
  RegisterRead (&RegisterId, &biosScratchPad1.Data);

  if (biosScratchPad1.Data & BIT4) {

     //
     // Set "BIOS_RESET_CPL" flag bit of all PBSP sockets
     //
     for (socket = 0; socket < MAX_SOCKET; socket++) {
        if (socket == sbspSktId) continue;  // skip SBSP
        if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      SetRstCpl(host, socket);
        }
     }

     //
     //  Lastly the SBSP socket
     //
     SetRstCpl(host, sbspSktId);
     //
     // Program DMICTRL...We only want to do this for SBSP... clear bits 0:1 of DMICTRL on Legacy socket only
     //
   rcPrintf ((host, "Clear DMICtrlAutoPmCmplt...\n"));
   ClearDmictrlAutoPmCplt(host, sbspSktId);
  } else {
    rcPrintf ((host, "called early during cold reset...\n"));
    biosScratchPad1.Data |= BIT4;
    RegisterWrite (&RegisterId, &biosScratchPad1.Data);
    return;
  }

} // end routine


UINT8
GetAvailableLlcWays(
  USRA_ADDRESS  *RegisterId  
)
/**


    Get number of available LLC ways for the given CPU package

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID

    @retval data8 -  number of available LLC ways

**/
{
  UINT8 data8;
  CAPID0_PCU_FUN3_STRUCT  pcu_capid0;

  //
  // Read cachesz of CAPID0 of the socket to determine number of LLC ways per Cbo slice
  //
  //  Fuse value   LLC Size per slice (Enabled ways per slice)
  //  000:        11 ways;
  //  001:        12 ways;
  //  010:        2 ways;
  //  011:        8 ways;
  //  100:        20 ways
  //

  data8 = 0;
  RegisterId->Csr.Offset = CAPID0_PCU_FUN3_REG;
  RegisterRead (RegisterId, &pcu_capid0.Data);

  switch (pcu_capid0.Bits.llc_way_en) {

  case 1:
    data8 = (UINT8) 12;  //12 ways
    break;

  case 2:
    data8 = (UINT8) 2;   //2 ways
    break;

  case 3:
    data8 = (UINT8) 8;   //8 ways
    break;

  case 4:
    data8 = (UINT8) 20;  //20 ways
    break;

  default:
    data8 = (UINT8) 11;  //11 ways
    break;
  }

  return data8;
}

UINT8
GetAvailableSlices(
  USRA_ADDRESS  *RegisterId
)
/**

    Get number of availble/enabled LLC slices for the given CPU package

    @param host:     Pointer to sysHost structure on stack
    @param socket:  CPU socket ID

    @retval data8 - number of availble/enabled LLC slices

**/
{
  UINT8 data8,i;
  UINT32 data32;
  CAPID6_PCU_FUN3_STRUCT pcu_capid;

     data8=0;
     RegisterId->Csr.Offset = CAPID6_PCU_FUN3_REG;
     RegisterRead (RegisterId, &pcu_capid.Data);
     data32 = (UINT32)pcu_capid.Bits.llc_slice_en;

     //
     // if a bit=0 then the slice is enabled
     // count the number of '0's in the bitmask
     //
       for (i=0; i<((struct sysHost *)(RegisterId->Attribute.HostPtr))->var.common.MaxCoreCount; i++) {
           if (data32 & 1) {
              data8 += 1;
       }
           data32 >>= 1;
     }

     return data8;
}



UINT8
GetLlcWaysTaken (
  struct sysHost   *host,
  UINT8 NumOfSlices
  )
/*
The maximum allowable value for ocla_max_way = Number_Available_Ways minus [(Two Ways for Isochronous usage if enabled) plus (One Way for non-Isochronous usage) plus (Number_Ways_NEM)].

For SKX:
    i.   The maximum NEM Way usage is dependent on Slice Count (i.e., number of LLC ways available)  and determined by a lookup table

*/

{

  //  Table of SKX Slices v.s. NEM Ways ( Note: a 0 element means n/a)
  //  Fused Slice Count   0   1   2   3   4   5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  UINT8 SKX_NEM_Ways[] = {0, 48, 24, 16, 12, 10, 8, 7, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2};
  UINT8 NumOfWaysTaken;

  NumOfWaysTaken = 1 + SKX_NEM_Ways[NumOfSlices];  // LLC ways used by non-isoch and NEM

  //
  // Reserve 2 LLC ways if Isoc enabled
  //
  if (host->var.kti.OutIsocEn) {
    NumOfWaysTaken += 2;
  }

  return NumOfWaysTaken;
}






UINT64_STRUCT
SetIotLlcConfig (
  struct sysHost  *host,
         UINT8     Socket,
       UINT8     OclaMaxTorEntry,
         UINT8     OclaLlcWay
)
  /**

   Executed by SBSP only. Call BIOS/VCU Mailbox interface to perform IOT LLC configuration setup.

          @param host:  pointer to sysHost structure on stack
          @param Socket:   CPU socketID
                OclaMaxtorEntry, OclaLlcWay :   IOT LLC config parameters for the given  socket

     @retval 64-bit return code from the VCU mailbox:
    @retval Lo Dword
       @retval [15:0] =  Command Response Code (success, timeout, etc)
       @retval [31:16] = rsvd
    @retval Hi Dword
       @retval [31:0] = Return data if applicable

  */

{
    UINT32           cmd32, data32;
  UINT64_STRUCT    RetCode;
  UINT64_STRUCT    RetCode_save;



    //Note: Command CSR [23:16] = Index.  Set Index=0   for all the following mailbox commands


    //
    // Open Sequence
    //
    cmd32 =  VCODE_API_OPCODE_OPEN_SEQUENCE;
    data32 = VCODE_API_SEQ_ID_IOT_LLC_SETUP;
    RetCode = WriteBios2VcuMailboxCommand(host, Socket, cmd32, data32);
    if (RetCode.lo != VCODE_MAILBOX_CC_SUCCESS)  {
         goto VcuApiError;
    }

    //
    // Set Parameters      bit[15:8] = MaxTorEntries, bit[7:0] = Number of LLC ways
    //
    cmd32 =  VCODE_API_OPCODE_SET_PARAMETER;
    data32 = (OclaMaxTorEntry << 8) | OclaLlcWay;
    RetCode = WriteBios2VcuMailboxCommand(host, Socket, cmd32, data32);
    if (RetCode.lo != VCODE_MAILBOX_CC_SUCCESS)  {
         goto VcuApiError;
    }

    //
    // Send IOT_LLC_SETUP command
    //
    cmd32 =  VCODE_API_OPCODE_IOT_LLC_SETUP;
    data32 = 0;
    RetCode = WriteBios2VcuMailboxCommand(host, Socket, cmd32, data32);
    if (RetCode.lo != VCODE_MAILBOX_CC_SUCCESS)  {
         goto VcuApiError;
    }

    RetCode_save = RetCode;     // save return code of IOT_LLC_SETUP API call

    //
    // Close Sequence
    //
    cmd32 =  VCODE_API_OPCODE_CLOSE_SEQUENCE;
    data32 = VCODE_API_SEQ_ID_IOT_LLC_SETUP;
    RetCode = WriteBios2VcuMailboxCommand(host, Socket, cmd32, data32);

    RetCode = RetCode_save;     // prepare return code
    return RetCode;


VcuApiError:

  rcPrintf ((host, "\n::VCU API Error. VCU_MAILBOX_DATA[31:0] = 0x%x, VCU_MAILBOX_INTERFACE[15:0] = 0x%x \n", RetCode.hi ,RetCode.lo ));

    return RetCode;

}

/**

  Check if socket has Isoch/MESEG enabled and return the max number of
  reservable TOR entries accordingly.

  @param host   - pointer to sysHost structure on stack
  @param socket - socket being checked

  @retval maximum number of reservable TOR entries

**/
UINT8
GetMaxReservableTorEntries (
  struct sysHost *host
  )
{
  UINT8 retval;

  if (host->var.kti.OutIsocEn == TRUE) {
    retval = MAX_TOR_ENTRIES_ISOC;
  } else {
    retval = MAX_TOR_ENTRIES_NORMAL;
  }

  return retval;
}

/**

 Executed by SBSP only. Program IOT/OCLA config based on Setup data.

 This function  reserves some of the LLC ways for the IOT while still in NEM mode which also uses LLC for CAR.
 All CBOs of a CPU socket will be programmed the same way.

 It's possible that the IOT ways and the NEM ways will end up overlapping because the LLC may have allocated
 one of the NEM cache lines into the way that you reserved for the IOT since some of those allocations will have
 occurred before the Cbo knew which ways were reserved for the IOT.

 But hits to those LLC ways will still be handled correctly even after those ways have been reserved for the IOT.
 So the data that NEM cached will not be lost, and NEM will still function. We just have to make sure that IOT
 doesn't start debug trace fill into the IOT reserved ways until after NEM mode is completed. Validation team
 agrees to this.

 Since these OCLA config registers are sticky. we program them only on power-good reset path, assuming  no
 IOT/OCLA trace will occur until a warm reset takes place. We leave these registers alone on the warm reset path.
 Every time the related Setup option get changed and saved, a Power-Good reset is expected.

 @param host:  pointer to sysHost structure on stack

 @retval host->var.common.resetRequired updated (if reset is needed)

**/
VOID
Config_IOT_LLC (
  struct sysHost             *host
  )

{
  UINT8   socket;
  UINT8   NumOfLlcWays, NumOfSlices, LlcWaysTaken, OclaMaxTorEntry, OclaLlcWay, MaxReservableTor;
  UINT8   resetNeeded = 0;
  UINT64_STRUCT   RetCode;
  USRA_ADDRESS  RegisterId;
  USRA_CSR_OFFSET_ADDRESS (RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  //
  // Do nothing if not SKX CPU
  //
  if (host->var.common.cpuType != CPU_SKX)  { return; }

  rcPrintf ((host, "\nConfig IOT/LLC via VCU Mailbox Start: resetRequested=%x\n", host->var.common.resetRequired));

  //
  // Loop  thru all sockets
  //
  for ( socket=0; socket<MAX_SOCKET; socket++ )   {
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0)
      continue;

    // skip if IOT config is disabled in Setup
    if (host->setup.cpu.IotEn[socket] == 0) {
      continue;
    }
    RegisterId.Csr.SocketId = socket;
    NumOfLlcWays = GetAvailableLlcWays(&RegisterId);
    NumOfSlices  = GetAvailableSlices(&RegisterId);
    LlcWaysTaken = GetLlcWaysTaken(host, NumOfSlices);
    MaxReservableTor = GetMaxReservableTorEntries (host);

    rcPrintf ((host, "::Skt%2d NumSlices=0x%x NumOfLlcWays=0x%x LlcWaysTaken=0x%x\n", socket,  NumOfSlices, NumOfLlcWays, LlcWaysTaken));
    rcPrintf ((host, "::Skt%2d SetupData: IotEn = 0x%x OclaMaxTorEntry=0x%x OclaWays=0x%x\n", socket, host->setup.cpu.IotEn[socket], host->setup.cpu.OclaTorEntry[socket], host->setup.cpu.OclaWay[socket]));

    OclaMaxTorEntry = host->setup.cpu.OclaTorEntry[socket];
    OclaLlcWay = host->setup.cpu.OclaWay[socket];

    //
    // Validate the Setup data, skip if data is invalid
    //
    if (OclaMaxTorEntry > MaxReservableTor) {
      rcPrintf ((host, "::ERROR: Skt%2d Setupdata OclaMaxTorEntry invalid (0x%2x)\n", socket, OclaMaxTorEntry));
      continue;
    }

    if ((OclaLlcWay > (UINT8)(NumOfLlcWays - LlcWaysTaken)) || (LlcWaysTaken > NumOfLlcWays)) {
      rcPrintf ((host, "::ERROR: Skt%2d Setup data OclaWay invalid (0x%2x)\n", socket, OclaLlcWay));
      rcPrintf ((host, "::Out of %2d LLC Ways, %2d are taken, %2d are available for IOT\n", NumOfLlcWays, LlcWaysTaken, NumOfLlcWays - LlcWaysTaken));
      continue;
    }

    rcPrintf ((host, "::Socket%2d calling IOT_LLC_SETUP VCU API with MaxTorEntry = 0x%x LLC-ways = 0x%x\n", socket, OclaMaxTorEntry, OclaLlcWay  ));

    //
    // Skip if running on simulation
    //
    if (host->var.common.emulation & SIMICS_FLAG) {
      continue;
    }

    //
    // Call VCU Mailbox API to set up IOT LLC config
    //
    RetCode = SetIotLlcConfig(host, socket, OclaMaxTorEntry, OclaLlcWay);

    rcPrintf ((host, "::Socket%2d IOT_LLC_SETUP VCU API VCU_MAILBOX_DATA[31:0] = 0x%x, VCU_MAILBOX_INTERFACE[15:0] = 0x%x\n", socket, RetCode.hi ,RetCode.lo ));

    if (RetCode.lo != VCODE_MAILBOX_CC_SUCCESS){
      rcPrintf ((host, "::VCU API return-code indicates error occured\n"));
    }

    if ((RetCode.lo == VCODE_MAILBOX_CC_SUCCESS) && (RetCode.hi == 1)) {
      if (host->var.common.resetRequired) {  // Cold boot or Fisrt time IOT config is enabled in Setup
        //
        // IOT/LLC config setting has changed. A warm reset is needed
        //
        resetNeeded |= POST_RESET_WARM;
      }
      rcPrintf ((host, "::VCU API return-code indicates a warm reset is needed\n", socket, RetCode.hi ,RetCode.lo ));
    }

  }

  if (resetNeeded)  {
    host->var.common.resetRequired |= resetNeeded;
  }

  rcPrintf ((host, "\nConfig IOT/OCLA end. Reset requested by VCU = %x, resetRequired = %x \n", resetNeeded, host->var.common.resetRequired));

}

/**
  s5331840: Program the KTi IRQ Threshold for 4S ring or 8S

  @param host:  pointer to sysHost structure on stack

  @retval VOID
**/
VOID
Program_IRQ_Threshold (
  struct sysHost     *host
  )

{
  UINT8               socket;
  UINT64_STRUCT       msrData;

  //
  // Loop thru all CPU sockets
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) {
        continue;
      }

      if (!(host->var.common.emulation & SIMICS_FLAG)) {

        msrData = ReadMsrPipe(host, socket, MSR_IRQ_VMSR_64H);
        msrData.lo &= ~MASK_IRQ_VMSR_IRQ_TH;    // clear Bit[4:0]
        msrData.lo |= host->var.kti.OutIRQThreshold;
        WriteMsrPipe(host, socket, MSR_IRQ_VMSR_64H, msrData);
      }
    }
  }
}


/**

  Executed by SBSP only. Configure CPU features that require a reset to take effect

  @param host:  pointer to sysHost structure on stack

  @retval VOID
  @retval Related CPU CSRs are programmed as needed.
  @retval host->var.common.resetRequired is updated as needed

**/
VOID
CpuInit (
  struct sysHost             *host
  )
{
  UINT64_STRUCT TraceHubAcpiBase;

  rcPrintf ((host, "\n\nCpuInit Start\n\n"));

  TraceHubAcpiBase.hi = 0;
  TraceHubAcpiBase.lo = (UINT32) PCH_TRACE_HUB_FW_BASE_ADDRESS;

  // set NPK_STH_ACPIBAR_BASE
  CpuWriteTraceHubAcpiBase (host, TraceHubAcpiBase);

  //
  // read package BSP data into sysHost structure
  // (can only be done after CPU topology has been discovered by KTI RC )
  //
  CollectPbspData (host);

  //
  // Init uncore misc CSRs
  //
  UncoreInitMisc (host);

  //
  // WFR buffering issue WA
  // HSD 5330798: WFR WA should be disabled on A0
  //
  WFRSocketWA (host);


  if (host->var.common.bootMode == S3Resume) {    //S3 resume path
    RestoreProcessorCsrsOnS3Resume(host);
  }

  else {                            // normal boot path

    CheckVtIfNeedReset(host);
    if (host->var.common.resetRequired & POST_RESET_POWERGOOD) {
    return;
    }

    ProgramProcessorFlexRatio(host);

    SetActiveCoresAndSmtEnableDisable(host);
  }

  Config_IOT_LLC (host);

  //
  // CPU MISC configuration
  //
  CpuInitMISC (host);
  
  //
  // s5331840: Init the KTI IRQ Threshold for 4S ring or 8S
  //
  Program_IRQ_Threshold (host);
}


/**

  Configure PPIN MSR on all sockets

  @param host:  pointer to sysHost structure on stack

  @retval VOID
**/
VOID
PpinInit (
  struct sysHost     *host
  )

  {
  UINT64_STRUCT msrReg;
  UINT8         PpinEnable = 0;
  UINT8         socket;

  //
  // To enable PPIN, Bit1 of PPIN_CTRL_MSR needs to be set to 1
  //
  if(host->setup.cpu.PpinControl) {
    PpinEnable = 0x02;
  }
  else {
    PpinEnable = 0;
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {

      //
      // Check if CPU supports PPIN
      //
      msrReg = ReadMsrPipe(host, socket, MSR_PLATFORM_INFO);
      if((msrReg.lo & BIT23) == 0)  continue; // CPU is not PPIN capable

      //
      // Check if PPIN_CTRL MSR is locked
      //
      msrReg = ReadMsrPipe(host, socket, MSR_PPIN_CTL);
      if (msrReg.lo & BIT0)  continue;  // PPIN_CTRL MSR is locked

      //
      // Program PPIN_CTRL MSR
      //
      msrReg.lo = (msrReg.lo & 0xFFFFFFFC) | PpinEnable;
      WriteMsrPipe(host, socket, MSR_PPIN_CTL, msrReg);
      rcPrintf ((host, "::S%2d PPIN_CTRL_MSR[1:0] set to 0x%2x\n", socket, PpinEnable));
    }
  }
}

/**

  Executed by SBSP only. Configure CPU registers that are required to be configured for PCODE usage prior to setting BIOS_INIT_Done.

  @param host:  pointer to sysHost structure on stack

  @retval VOID
  @retval Related CPU CSRs are programmed as needed.
  @retval host->var.common.resetRequired is updated as needed

**/
VOID
CpuInitMISC (
  struct sysHost             *host
  )
{
  UINT8   socket;
  UINT8   sbspSktId = GetSbspSktId(host);
  DMICTRL_N0_IIO_PCIEDMI_STRUCT DMICTRL;
  UINT32 TempData = 0;
  UINT32 PcodeMailboxStatus = 0;
  BOOLEAN WriteRequired = FALSE;
  USRA_ADDRESS  RegisterId;

  if (host->var.common.emulation==0)  {  // skitp for emulation
    //
    // Enable or disable PPIN
    //
    PpinInit (host);
  }
  USRA_CSR_OFFSET_ADDRESS (RegisterId, 0, 0, 0, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {

      rcPrintf ((host, "CPUMISC Current S%2d: \n", socket));

      RegisterId.Csr.SocketId = socket;

      //
      // Program DMICTRL...We only want to do this for SBSP... clear bits 0:1 of DMICTRL on Legacy socket only
      //
      if (socket == sbspSktId) {
        RegisterId.Csr.Offset = DMICTRL_N0_IIO_PCIEDMI_REG;
        RegisterRead (&RegisterId, &DMICTRL.Data);
        DMICTRL.Bits.abort_inbound_requests = 0;
        //BIOS Done-Fix DMICTRL.Bits.auto_complete_pm = 0;
        RegisterWrite (&RegisterId, &DMICTRL.Data);
      } // if socket 0

      if (!(host->var.common.emulation & SIMICS_FLAG)) {
        OemPostCpuInit (host, socket);
      }

      if (!(host->var.common.emulation & SIMICS_FLAG)) {
        PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);
        if (PcodeMailboxStatus == 0) {
          RegisterId.Csr.Offset = BIOS_MAILBOX_DATA_PCU_FUN1_REG;
          RegisterRead (&RegisterId, &TempData);
          rcPrintf ((host, "MBoxStatus=%x,  PCU_MISC_CONFIG=%x\n", PcodeMailboxStatus, TempData));
        }
        if (host->setup.PmaxDisable) {
          //
          // Program MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG (0x6), Bit[23] = Pmax Disable
          //
          WriteRequired = TRUE;
          TempData &= ~BIT23;
          TempData |= (host->setup.PmaxDisable << 23);
        }
        if (WriteRequired) {
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG, TempData);
          rcPrintf ((host, "\nSetting MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG = %x\n", TempData));
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);
          if (PcodeMailboxStatus == 0) {
            RegisterId.Csr.Offset = BIOS_MAILBOX_DATA_PCU_FUN1_REG;
            RegisterRead (&RegisterId, &TempData);
            rcPrintf ((host, "Read back MBoxStatus=%x,  MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG=%x\n", PcodeMailboxStatus, TempData));
          }
        }
      }
    } // end check if socket present
  } // end for{} loop
}


/**

  Executed by SBSP only. Program BIOS_RESET_CPL register.

  @param host:  pointer to sysHost structure on stack

  @retval VOID
  @retval Related CPU CSRs are programmed as needed.
**/
VOID
SetRstCpl (
  struct sysHost     *host,
  UINT8              socket
  )
{

  BIOS_RESET_CPL_PCU_FUN1_STRUCT BIOSRESETCPL;
  USRA_ADDRESS RegisterId;
  USRA_CSR_OFFSET_ADDRESS (RegisterId, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  RegisterRead (&RegisterId, &BIOSRESETCPL.Data);
  BIOSRESETCPL.Bits.rst_cpl1 = 1;
  rcPrintf ((host, "\n:INIT - BIOS_RESET_CPL: Set CPL1 on #S%d\n", socket));
  RegisterWrite (&RegisterId, &BIOSRESETCPL.Data);
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    while (BIOSRESETCPL.Bits.pcode_init_done1 != 1) {
      RegisterId.Csr.Offset = BIOS_RESET_CPL_PCU_FUN1_REG;
      RegisterRead (&RegisterId, &BIOSRESETCPL.Data);
    }  //wait for PCU acknowledgement
  }


}


/**

  Executed by SBSP only. cLEAR dmictrl.AUTO_COMPLETE_PM

  @param host:  pointer to sysHost structure on stack

  @retval VOID
  @retval Related CPU CSRs are programmed as needed.
**/
VOID
ClearDmictrlAutoPmCplt (
  struct sysHost      *host,
  UINT8               socket
  )
{
  DMICTRL_N0_IIO_PCIEDMI_STRUCT DMICTRL;
  USRA_ADDRESS  RegisterId;
  USRA_CSR_OFFSET_ADDRESS (RegisterId, socket, 0, DMICTRL_N0_IIO_PCIEDMI_REG, CsrBoxInst);
  RegisterId.Attribute.HostPtr = (UINT32)host;

  RegisterRead (&RegisterId, &DMICTRL.Data);
  DMICTRL.Bits.auto_complete_pm = 0;
  RegisterWrite (&RegisterId, &DMICTRL.Data);
  rcPrintf ((host, "DMICTRL.auto_complete_pm cleared on S%x\n", RegisterId.Csr.SocketId));
}

#endif // IA32
