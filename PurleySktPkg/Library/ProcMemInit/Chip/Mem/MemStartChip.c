/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2017 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "RcRegs.h"
#include "MemApiSkx.h"

VOID
SetSetupOptionsChip (
  PSYSHOST host
  )
{


  // HSD 5332613 - DISABLED CMD VREF for issue
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    *(UINT8 *)&host->setup.mem.optionsExt &= ~CMD_VREF_EN;
  }

  return;
} // SetSetupOptionsChip


#ifdef SERIAL_DBG_MSG
/**

    Print Memory Setup Options

    @param host   - Pointer to SysHost
    @param socket - Current socket

    @retval N/A

**/
VOID
PrintMemSetupOptionsChip (
  PSYSHOST host,
  UINT8    socket
  )
{
  UINT8           i ;
  rcPrintf ((host, "  lowGap:          %02X\n", host->setup.common.lowGap));
  rcPrintf ((host, "  highGap:         %02X\n", host->setup.common.highGap));
  rcPrintf ((host, "  mmiohSize:       %08X\n", host->setup.common.mmiohSize));
  rcPrintf ((host, "  isocEn:          %02X\n", host->setup.common.isocEn));
  rcPrintf ((host, "  dcaEn:           %02X\n", host->setup.common.dcaEn));
  rcPrintf ((host, "  options (Chip):  %08X\n", host->setup.mem.options));
  rcPrintf ((host, "      SCRAMBLE_EN_DDRT         "));
  if (host->setup.mem.options & SCRAMBLE_EN_DDRT) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      RAS_TO_INDP_EN      "));
  if (host->setup.mem.options & RAS_TO_INDP_EN) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      DISABLE_CHANNEL_EN "));
  if (host->setup.mem.options & DISABLE_CHANNEL_EN) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  RASmode:               %02X\n", host->setup.mem.RASmode));
  rcPrintf ((host, "      RK_SPARE           "));
  if (host->setup.mem.RASmode & RK_SPARE) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      STAT_VIRT_LOCKSTEP "));
  if (host->setup.mem.RASmode & STAT_VIRT_LOCKSTEP) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      CH_MIRROR_1LM          "));
  if (host->setup.mem.RASmode & FULL_MIRROR_1LM) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      CH_MIRROR_2LM          "));
  if (host->setup.mem.RASmode & FULL_MIRROR_2LM) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "      ADDDC_EN      "));
  if (host->setup.mem.RASmodeEx & ADDDC_EN) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, " optionsNgn: %08X\n", host->setup.mem.optionsNgn));
  rcPrintf ((host, "  LOCK_NGN_CSR "));
  if (host->setup.mem.optionsNgn & LOCK_NGN_CSR) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_CMD_TIME "));
  if (host->setup.mem.optionsNgn & NGN_CMD_TIME) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_ECC_CORR "));
  if (host->setup.mem.optionsNgn & NGN_ECC_CORR) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_ECC_WR_CHK "));
  if (host->setup.mem.optionsNgn & NGN_ECC_WR_CHK) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_ECC_RD_CHK "));
  if (host->setup.mem.optionsNgn & NGN_ECC_RD_CHK) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  CR_MONETIZATION "));
  if (host->setup.mem.optionsNgn & CR_MONETIZATION) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_DEBUG_LOCK "));
  if (host->setup.mem.optionsNgn & NGN_DEBUG_LOCK) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }
  rcPrintf ((host, "  NGN_ARS_ON_BOOT "));
  if (host->setup.mem.optionsNgn & NGN_ARS_ON_BOOT) {
    rcPrintf ((host, "1\n"));
  } else {
    rcPrintf ((host, "0\n"));
  }

  rcPrintf ((host, "crQosConfig: %x\n", host->setup.mem.crQosConfig));
  rcPrintf ((host, "\nstruct sysHost.setup.mem (Chip) {\n"));
  rcPrintf ((host, "  socketInter:     %02X\n", host->setup.mem.socketInter));
  rcPrintf ((host, "  imcInter:        %02X\n", host->setup.mem.imcInter));
  rcPrintf ((host, "  rankInter:       %02X\n", host->setup.mem.rankInter));
  rcPrintf ((host, "  DdrtMemPwrSave:  %02X\n", host->setup.mem.DdrtMemPwrSave));
  rcPrintf ((host, "  spareErrTh:      %02X\n", host->setup.mem.spareErrTh));
  rcPrintf ((host, "  DieSparing:      %02X\n", host->setup.mem.DieSparing));
  rcPrintf ((host, "  DieSparingAggressivenessLevel:      %02X\n", host->setup.mem.DieSparingAggressivenessLevel));
  rcPrintf ((host, "  ADDDCEn:      %02X\n", host->setup.mem.ADDDCEn));
#ifdef SSA_FLAG
  rcPrintf ((host, "  enableBiosSsaRMT: %02X\n", host->setup.mem.enableBiosSsaRMT));
  rcPrintf ((host, "  enableBiosSsaRMTonFCB: %02X\n", host->setup.mem.enableBiosSsaRMTonFCB));
  rcPrintf ((host, "  biosSsaPerBitMargining: %02X\n", host->setup.mem.biosSsaPerBitMargining));
  rcPrintf ((host, "  biosSsaDisplayTables: %02X\n", host->setup.mem.biosSsaDisplayTables));
  rcPrintf ((host, "  biosSsaPerDisplayPlots: %02X\n", host->setup.mem.biosSsaPerDisplayPlots));
  rcPrintf ((host, "  biosSsaLoopCount: %02X\n", host->setup.mem.biosSsaLoopCount));
  rcPrintf ((host, "  biosSsaBacksideMargining: %02X\n", host->setup.mem.biosSsaBacksideMargining));
  rcPrintf ((host, "  biosSsaEarlyReadIdMargining: %02X\n", host->setup.mem.biosSsaEarlyReadIdMargining));
  rcPrintf ((host, "  biosSsaStepSizeOverride: %02X\n", host->setup.mem.biosSsaStepSizeOverride));
  rcPrintf ((host, "  biosSsaRxDqs: %02X\n", host->setup.mem.biosSsaRxDqs));
  rcPrintf ((host, "  biosSsaRxVref: %02X\n", host->setup.mem.biosSsaRxVref));
  rcPrintf ((host, "  biosSsaTxDq: %02X\n", host->setup.mem.biosSsaTxDq));
  rcPrintf ((host, "  biosSsaTxVref: %02X\n", host->setup.mem.biosSsaTxVref));
  rcPrintf ((host, "  biosSsaCmdAll: %02X\n", host->setup.mem.biosSsaCmdAll));
  rcPrintf ((host, "  biosSsaCmdVref: %02X\n", host->setup.mem.biosSsaCmdVref));
  rcPrintf ((host, "  biosSsaCtlAll: %02X\n", host->setup.mem.biosSsaCtlAll));
  rcPrintf ((host, "  biosSsaEridDelay: %02X\n", host->setup.mem.biosSsaEridDelay));
  rcPrintf ((host, "  biosSsaEridVref: %02X\n", host->setup.mem.biosSsaEridVref));
  rcPrintf ((host, "  biosSsaDebugMessages: %02X\n", host->setup.mem.biosSsaDebugMessages));
#endif // SSA_FLAG
  rcPrintf ((host, "  NgnAddressRangeScrub:      %02X\n", host->setup.mem.NgnAddressRangeScrub));
  rcPrintf ((host, "  NgnHostAlertAit:      %02X\n", host->setup.mem.NgnHostAlertAit));
  rcPrintf ((host, "  NgnHostAlertDpa:      %02X\n", host->setup.mem.NgnHostAlertDpa));
  rcPrintf ((host, "  leakyBktLo:      %02X\n", host->setup.mem.leakyBktLo));
  rcPrintf ((host, "  leakyBktHi:      %02X\n", host->setup.mem.leakyBktHi));
  rcPrintf ((host, "  numSparTrans:  %04X\n", host->setup.mem.numSparTrans));
  rcPrintf ((host, "  };\n"));
  //
  //**Memmap related options**
  //
  rcPrintf ((host, "  volMemMode = %d\n", host->setup.mem.volMemMode )) ;
  rcPrintf ((host, "  memInterleaveGran1LM = %d\n", host->setup.mem.memInterleaveGran1LM )) ;
  rcPrintf ((host, "  memInterleaveGran2LM  = %d\n", host->setup.mem.memInterleaveGran2LM )) ;
  //
  //**Memmap related options for DFX
  //
  rcPrintf ((host, "  dfxMemInterleaveGranPMemNUMA = %d\n", host->setup.mem.dfxMemSetup.dfxMemInterleaveGranPMemNUMA )) ;
  rcPrintf ((host, "  dfxCfgMask2LM = %d\n", host->setup.mem.dfxMemSetup.dfxCfgMask2LM )) ;
  rcPrintf ((host, "  dfxDimmManagement = %02X\n", host->setup.mem.dfxMemSetup.dfxDimmManagement )) ;
  rcPrintf ((host, "  dfxLoadDimmMgmtDriver = %02X\n", host->setup.mem.dfxMemSetup.dfxLoadDimmMgmtDriver )) ;
  rcPrintf ((host, "  dfxPerMemMode = %d\n", host->setup.mem.dfxMemSetup.dfxPerMemMode )) ;
  rcPrintf ((host, "  dfxPartitionDDRTDimm = %02X\n", host->setup.mem.dfxMemSetup.dfxPartitionDDRTDimm )) ;
  rcPrintf ((host, "  dfxOptions(BIT0->HIGH_ADDR_EN) = %02X\n", (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN) )) ;
  rcPrintf ((host, "  dfxOptions(BIT2->CR_MIXED_SKU) = %02X\n", (host->setup.mem.dfxMemSetup.dfxOptions & CR_MIXED_SKU) )) ;
  for( i = 0 ; i < ( MAX_SOCKET * MAX_IMC ) ; i++ )  {
    rcPrintf ((host, "  dfxPartitionRatio[%02X] = %02X\n", i, host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] )) ;
  }

  //
  // ** struct ddrIMCSetup **
  //
  rcPrintf ((host, "  struct ddrIMCSetup[%02X] {\n", socket));
  rcPrintf ((host, "    enabled:        %08X\n", host->setup.mem.socket[socket].enabled));
  rcPrintf ((host, "  }\n"));

  return;
}
#endif // SERIAL_DBG_MSG


VOID
InitDdrFreqChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  NA

--*/
{
  return;
} // InitDdrFreqChip

UINT32
SavedSetupDataChangedChip (
  PSYSHOST host
  )
/*++

Routine Description:

  Initialize internal data structures

Arguments:

  host  - Pointer to the system host (root) structure

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;

  result = FALSE;

  if (
   host->nvram.mem.savedSetupData.RASmode != host->setup.mem.RASmode ||
   host->nvram.mem.savedSetupData.memFlows != host->setup.mem.memFlows ||
   host->nvram.mem.savedSetupData.memFlowsExt != host->setup.mem.memFlowsExt ||
     host->nvram.mem.savedSetupData.socketInter != host->setup.mem.socketInter ||
     host->nvram.mem.savedSetupData.rankInter != host->setup.mem.rankInter ||
     host->nvram.mem.savedSetupData.DdrtMemPwrSave != host->setup.mem.DdrtMemPwrSave ||
     host->nvram.mem.savedSetupData.logParsing != host->setup.mem.logParsing ||
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
     host->nvram.mem.savedSetupData.meRequestedSize != host->setup.mem.meRequestedSize ||
#endif // ME_SUPPORT_FLAG
     host->nvram.mem.savedSetupData.spareErrTh != host->setup.mem.spareErrTh ||
     host->nvram.mem.savedSetupData.DieSparing != host->setup.mem.DieSparing ||
     host->nvram.mem.savedSetupData.DieSparingAggressivenessLevel != host->setup.mem.DieSparingAggressivenessLevel ||
#ifdef SSA_FLAG
     host->nvram.mem.savedSetupData.enableBiosSsaRMT != host->setup.mem.enableBiosSsaRMT ||
     host->nvram.mem.savedSetupData.enableBiosSsaRMTonFCB != host->setup.mem.enableBiosSsaRMTonFCB ||
     host->nvram.mem.savedSetupData.biosSsaPerBitMargining != host->setup.mem.biosSsaPerBitMargining ||
     host->nvram.mem.savedSetupData.biosSsaDisplayTables != host->setup.mem.biosSsaDisplayTables ||
     host->nvram.mem.savedSetupData.biosSsaPerDisplayPlots != host->setup.mem.biosSsaPerDisplayPlots ||
     host->nvram.mem.savedSetupData.biosSsaLoopCount != host->setup.mem.biosSsaLoopCount ||
     host->nvram.mem.savedSetupData.biosSsaBacksideMargining != host->setup.mem.biosSsaBacksideMargining ||
     host->nvram.mem.savedSetupData.biosSsaEarlyReadIdMargining != host->setup.mem.biosSsaEarlyReadIdMargining ||
     host->nvram.mem.savedSetupData.biosSsaStepSizeOverride != host->setup.mem.biosSsaStepSizeOverride ||
     host->nvram.mem.savedSetupData.biosSsaRxDqs != host->setup.mem.biosSsaRxDqs ||
     host->nvram.mem.savedSetupData.biosSsaRxVref != host->setup.mem.biosSsaRxVref ||
     host->nvram.mem.savedSetupData.biosSsaTxDq != host->setup.mem.biosSsaTxDq ||
     host->nvram.mem.savedSetupData.biosSsaTxVref != host->setup.mem.biosSsaTxVref ||
     host->nvram.mem.savedSetupData.biosSsaCmdAll != host->setup.mem.biosSsaCmdAll ||
     host->nvram.mem.savedSetupData.biosSsaCmdVref != host->setup.mem.biosSsaCmdVref ||
     host->nvram.mem.savedSetupData.biosSsaCtlAll != host->setup.mem.biosSsaCtlAll ||
     host->nvram.mem.savedSetupData.biosSsaEridDelay != host->setup.mem.biosSsaEridDelay ||
     host->nvram.mem.savedSetupData.biosSsaEridVref != host->setup.mem.biosSsaEridVref ||
     host->nvram.mem.savedSetupData.biosSsaDebugMessages != host->setup.mem.biosSsaDebugMessages ||
#endif // SSA_FLAG
     host->nvram.mem.savedSetupData.NgnAddressRangeScrub != host->setup.mem.NgnAddressRangeScrub
    ) {
      result = TRUE;
  }
  return result;
} // SavedSetupDataChangedChip

UINT32
DetectIMCEnabledChangeChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;

  result = FALSE;

  return result;
} // DetectIMCEnabledChangeChip


UINT32
SavedSetupDataChangedIMCChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:

  Initialize internal data structures

Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;

  result = FALSE;

  if (host->nvram.mem.savedSetupData.socket[socket].options != host->setup.mem.socket[socket].options) {
    result = TRUE;
  }

  return result;
} // SavedSetupDataChangedIMCChip


UINT32
DetectSocketPopulatedChangedChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
    UINT32 result;
    UINT16 bootDate;

    result = FALSE;

    //
    // Check if training data is old and needs re-training
    // Only check on First pass and if we are attempting Fast Boot.
    // Check for both Warm and Cold Boot
    //
    if ((host->var.common.bootMode == NormalBoot) &&
        ((host->setup.mem.options & ATTEMPT_FAST_BOOT) || (host->setup.mem.options & ATTEMPT_FAST_BOOT_COLD)) &&
        (host->var.mem.firstPass != 0) && (host->var.mem.dateCheckedOnFirstSocket != 1)) {

     host->var.mem.dateCheckedOnFirstSocket = 1;  //Indicates date to be checked only for the first socket;
      //If setupChanged == 1, then do a cold boot through memory from socket0; do not check the date for Socket1/2/3

      bootDate = MemGetCmosDate (host);
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Current Linear Date = %u days - Last Boot Date = %u days\n", bootDate, host->nvram.mem.lastBootDate));
      if (bootDate > (host->nvram.mem.lastBootDate + MEM_MAX_COLD_BOOT_DAYS)) {
        MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Training Data older than %u days. Forcing cold boot\n", MEM_MAX_COLD_BOOT_DAYS));
        result = TRUE;
      }
    }

  return result;
} // DetectSocketPopulatedChangedChip

UINT32
DetectMCAErrorsChip (
  PSYSHOST host,
  UINT8 socket,
  UINT32 *logData
  )
/*++

Routine Description:



Arguments:

  host    - Pointer to the system host (root) structure
  socket  - socket id
  logData - pointer to 32-bit lag data

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;
  MCA_ERR_SRC_LOG_PCU_FUN2_STRUCT errSrcLog;

  result = FALSE;

  //
  // Disable fast boot if there were any core errors on any socket
  //

  //errSrcLog.Data = ReadCpuPciCfgEx (host, socket, 0, MCA_ERR_SRC_LOG_PCU_FUN2_REG);
  errSrcLog.Data = 0;
  *logData = errSrcLog.Data;
  if (errSrcLog.Bits.mcerr || errSrcLog.Bits.ierr || errSrcLog.Bits.caterr) {
    result = TRUE;
  }

  return result;
} // DetectMCAErrorsChip


UINT32
DetectBootDateOldChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;

  result = FALSE;

  return result;
} // DetectBootDateOldChip


/**

  Gets Processor PPINs

  @param host:  pointer to sysHost structure on stack

  @retval PPIN
**/
UINT64_STRUCT
GetPpinChip (
       PSYSHOST host,
       UINT8    socket
       )

{
  UINT64_STRUCT msrReg;
  UINT64_STRUCT tmpReg;
  UINT64_STRUCT procPpin;

  procPpin.hi = 0x0;
  procPpin.lo = 0x0;
  rcPrintf ((host, "Get socket PPIN\n"));
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
      msrReg = ReadMsrPipe(host, socket, MSR_PLATFORM_INFO);
      if((msrReg.lo & BIT23) == 0) {
#ifdef SERIAL_DBG_MSG
        rcPrintf ((host, "Socket not PPIN Capable\n"));
#endif
        return procPpin; // CPU is not PPIN capable
      }
      //
      // Save PPIN_CTRL state
      //
      msrReg = ReadMsrPipe(host, socket, MSR_PPIN_CTL);
      tmpReg = msrReg;

      //
      // Unlock and enable PPIN_CTRL
      //
      msrReg.lo = (msrReg.lo & 0xFFFFFFFC) | 0x2;
      WriteMsrPipe(host, socket, MSR_PPIN_CTL, msrReg);

      //
      // Get PPIN
      //
      procPpin = ReadMsrPipe(host, socket, MSR_PPIN);

      //
      // Restore PPIN_CTRL
      //
      WriteMsrPipe(host, socket, MSR_PPIN_CTL, tmpReg);
    }
  } else {
    // Use hardcoded PPIN if running under Simics
    rcPrintf ((host, "Running in Simics\n"));
    procPpin.hi = 0xFFFFFFFF;
    procPpin.lo = 0x12345678;
  }

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "PPIN Hi = 0x%x, PPIN Lo = 0x%x\n", procPpin.hi, procPpin.lo));

  return procPpin;
} // GetPpinChip

UINT32
DetectProcessorRemovedChip (
  PSYSHOST host
  )
/*++

Routine Description:

  Initialize internal data structures

Arguments:

  host  - Pointer to the system host (root) structure

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;

  result = FALSE;
  if  (host->nvram.mem.socketBitMap != host->var.common.socketPresentBitMap) {
    result = TRUE;
  }

  return result;
} // DetectProcessorRemovedChip


UINT32
AttemptColdBootFastChip (
  PSYSHOST host
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure

Returns:

  TRUE / FALSE

--*/
{
  UINT32 result;
  result = FALSE;
  if ((host->var.mem.subBootMode == ColdBoot || host->var.mem.subBootMode == WarmBoot) && (host->setup.mem.options & ATTEMPT_FAST_BOOT_COLD)) {
    result = TRUE;
  }
  return result;
} // AttemptColdBootFastChip


UINT32
GetDisableQRDimmChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32                     result;
  CAPID3_PCU_FUN3_STRUCT     capId3;

  result = FALSE;

  //
  // Get IMC Fuse data
  //
  capId3.Data = host->var.common.procCom[socket].capid3;

  //
  // Check if QR DIMM is disabled in HW
  //
  if (capId3.Bits.disable_qr_dimm) {
    result = TRUE;
  }

  return result;
} // GetDisableQRDimmChip


VOID
SetIMCEnabledChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  host->nvram.mem.socket[socket].enabled = host->setup.mem.socket[socket].enabled;
  return;
} // SetIMCEnabledChip


UINT32
GetChnDisableChip (
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id
  ch     - ch id

Returns:

  TRUE / FALSE

--*/
{
  UINT32                          result;

  result = FALSE;

  return result;
} // GetChnDisableChip


UINT32
GetDisable2dpcChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32                     result = FALSE;

  return result;
} // GetDisable2dpcChip


UINT32
GetDisable3dpcChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  TRUE / FALSE

--*/
{
  UINT32                     result;

  result = TRUE;

  return result;
} // GetDisable3dpcChip


VOID
SetSteppingsChip (
  PSYSHOST host,
  UINT8 socket
  )
/*++

Routine Description:



Arguments:

  host   - Pointer to the system host (root) structure
  socket - socket id

Returns:

  NA

--*/
{


  return;
} // SetSteppingsChip
