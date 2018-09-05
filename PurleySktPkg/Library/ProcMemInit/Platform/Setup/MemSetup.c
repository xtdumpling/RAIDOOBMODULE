//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
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
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"

/**

  Programs input parameters for the MRC

  @param host - Pointer to the host structure

  @retval N/A

**/
void
GetMemSetupOptions (
  PSYSHOST host
  )
{
  UINT8 socket;
  UINT8 ch;
  UINT8 i,j;
  struct sysSetup *setup;
  setup = (struct sysSetup *) &host->setup;

  //
  // RASmode
  //  CH_INDEPENDENT
  //  FULL_MIRROR_1LM
  //  FULL_MIRROR_2LM
  //  CH_MIRROR - this will go away soon
  //  STAT_VIRT_LOCKSTEP
  //  RK_SPARE
  //
  setup->mem.RASmode = CH_INDEPENDENT;
  setup->mem.spareRanks = TWO_RANK_SPARE;

  //
  //  RASmodeEx
  //    DMNDSCRB_EN
  //    PTRLSCRB_EN
  //    SDDC_EN
  //
  setup->mem.RASmodeEx = 0;
  setup->mem.RASmodeEx |= PTRLSCRB_EN;
  setup->mem.RASmodeEx |= DMNDSCRB_EN;
  setup->mem.RASmodeEx &= ~SDDC_EN;

  setup->mem.patrolScrubAddrMode |= PATROL_SCRUB_SPA;


  //
  //  options
  //    TEMPHIGH_EN
  //    PAGE_POLICY
  //    ALLOW2XREF_EN
  //    ADAPTIVE_PAGE_EN
  //    SCRAMBLE_EN
  //    NUMA_AWARE
  //    ECC_CHECK_EN
  //    DISABLE_WMM_OPP_RD
  //    BALANCED_4WAY_EN
  //    CLTT_EN
  //    OLTT_EN
  //    SPLIT_BELOW_4GB_EN
  //    RAS_TO_INDP_EN
  //    CLTT_TEST_EN
  //    MARGIN_RANKS_EN
  //
  //
  //    MEM_OVERRIDE_EN
  //
  setup->mem.options &= ~MEM_OVERRIDE_EN;
  setup->mem.inputMemTime.nCL = 0;
  setup->mem.inputMemTime.nRP = 0;
  setup->mem.inputMemTime.nRCD = 0;
  setup->mem.inputMemTime.nRRD = 0;
  setup->mem.inputMemTime.nRRD_L = 0;
  setup->mem.inputMemTime.nWTR = 0;
  setup->mem.inputMemTime.nRAS = 0;
  setup->mem.inputMemTime.nRTP = 0;
  setup->mem.inputMemTime.nWR = 0;
  setup->mem.inputMemTime.nFAW = 0;
  setup->mem.inputMemTime.nCWL = 0;
  setup->mem.inputMemTime.nRC = 0;
  setup->mem.inputMemTime.nCMDRate = 0;
  setup->mem.inputMemTime.tREFI = 0;
  setup->mem.inputMemTime.nRFC = 0;
  //
  //    DRAMDLL_OFF_PD_EN
  //
  setup->mem.options = 0;
  setup->mem.thermalThrottlingOptions |= CLTT_EN;
  setup->mem.thermalThrottlingOptions &= ~(MH_OUTPUT_EN | MH_SENSE_EN);
  setup->mem.electricalThrottling = ET_AUTO;
  //setup->mem.altitude = UNKNOWN;
  //
  //    DRAM RAPL Init
  //
  setup->mem.DimmTempStatValue = DIMMTEMP_DEFAULT;
  setup->mem.dramraplen = 2;   // enable DRAM RAPL baselone in mode 1
  setup->mem.dramraplbwlimittf = 1;
  setup->mem.DramRaplExtendedRange = DRAM_RAPL_EXTENDED_RANGE_DEFAULT;
  setup->mem.dramraplRefreshBase = DRAM_RAPL_REFRESH_BASE_DEFAULT;
  setup->mem.customRefreshRate = 0;       // Disable by default
  setup->mem.CmsEnableDramPm = 0;
  //
  // Enhanced Log parsing support
  //
  setup->mem.logParsing = LOG_PARSING_DIS;
  //
  // Data Scrambling
  //
  if (SCRAMBLE_DEFAULT == SCRAMBLE_ENABLE) {
     setup->mem.options |= SCRAMBLE_EN_DDRT;
     setup->mem.options |= SCRAMBLE_EN; // ddr4
     setup->mem.scrambleSeedLow = SCRAMBLE_SEED_LOW;
  }

  // ECC Support
  if (ECC_DEFAULT) setup->mem.dfxMemSetup.dfxOptions |= ECC_CHECK_EN;

  // ECC-NonECC Mix dimms
  setup->mem.options &= ~ECC_MIX_EN;

  // Enable WMM Opportunistic Reads
  if (OPP_RD_WMM_DEFAULT == OPP_RD_WMM_EN) {
    setup->mem.options &= ~DISABLE_WMM_OPP_READ;
  }

  // Setup option for memory test
  if (MEM_TEST_DEFAULT == MEM_TEST_EN) {
    setup->mem.options |= MEMORY_TEST_EN;
  }

  // Setup option for memory test when fast boot
  if (MEM_TEST_FAST_BOOT_DEFAULT == MEM_TEST_FAST_BOOT_EN) {
    setup->mem.options |= MEMORY_TEST_FAST_BOOT_EN;
  }

  // Setup option for fast boot
  if (FAST_BOOT_DEFAULT == FAST_BOOT_EN) {
    setup->mem.options |= ATTEMPT_FAST_BOOT;
  }

  // Setup option for fast cold boot
  if (FAST_BOOT_COLD_DEFAULT == FAST_BOOT_COLD_EN) {
    setup->mem.options |= ATTEMPT_FAST_BOOT_COLD;
  }

  // Advanced Memory Test
  //
  setup->mem.advMemTestEn = ADV_MEM_TEST_DEFAULT;
  setup->mem.advMemTestOptions = \
//    ADV_MT_XMATS8 |
//    ADV_MT_XMATS16 |
    ADV_MT_XMATS32 |
//    ADV_MT_XMATS64 |
//    ADV_MT_WCMATS8 |
//    ADV_MT_WCMCH8 |
//    ADV_MT_GNDB64 |
    ADV_MT_MARCHCM64;

  //
  //
  // Memory test loops
  //
  setup->mem.memTestLoops = MEM_TEST_LOOPS_DEFAULT;

  //
  // Phase Shedding
  //
  setup->mem.phaseShedding = PHASE_SHEDDING_AUTO;


  // Page mode
  if (CLOSED_PAGE_DEFAULT == CLOSED_PAGE_EN) {
    // Closed
    setup->mem.options |= PAGE_POLICY;
    setup->mem.options &= ~ADAPTIVE_PAGE_EN;
  } else {
    // Open Adaptive
    setup->mem.options &= ~PAGE_POLICY;
    setup->mem.options |= ADAPTIVE_PAGE_EN;
  }

#ifdef MARGIN_CHECK
  //
  // Rank Margin Tool
  //
  if (RMT_DEFAULT == RMT_EN) {
    setup->mem.options |= MARGIN_RANKS_EN;
  }
#endif  // MARGIN_CHECK

  //
  // MDLL Shut Down Enable
  //

  if (MDLL_SDEN_DEFAULT == MDLL_SDEN_DIS) {
    setup->mem.options &= ~MDLL_SHUT_DOWN_EN;
  }else {
    setup->mem.options |= MDLL_SHUT_DOWN_EN;
  }

  //
  // Normal operation interval.
  //
  setup->mem.normOppIntvl = NORMAL_OPPERATION_INTERVAL;

  //
  // High Temp Enable
  //
  if (HTE_DEFAULT) {
     setup->mem.options |= TEMPHIGH_EN;
  } else {
     setup->mem.options &= ~TEMPHIGH_EN;
  }

  //
  // CMD Normalization
  //
  if (CMD_NORMAL_DEFAULT == CMD_NORMAL_ENABLE) {
    setup->mem.optionsExt |= CMD_NORMAL_EN;
  } else {
    setup->mem.optionsExt &= ~CMD_NORMAL_EN;
  }

  //
  // Allow 2x Refresh
  //
  if (A2R_DEFAULT) {
     setup->mem.options |= ALLOW2XREF_EN;
  } else {
     setup->mem.options &= ~ALLOW2XREF_EN;
  }

  //
  // C/A Parity
  //
  if (CA_PARITY_AUTO == CA_PARITY_ENABLE){
    setup->mem.options |= CA_PARITY_EN;
  } else {
    setup->mem.options &= ~CA_PARITY_EN;
  }


  //
  // Write CRC
  //
  if (WR_CRC_AUTO == WR_CRC_ENABLE) {
    setup->mem.optionsExt |= WR_CRC;
  }else {
    setup->mem.optionsExt &= ~WR_CRC;
  }

  //
  // DIMM ISOLATION
  //
  if (DIMM_ISOLATION_AUTO == DIMM_ISOLATION_ENABLE) {
    setup->mem.optionsExt |= DIMM_ISOLATION_EN;
  }else {
    setup->mem.optionsExt &= ~DIMM_ISOLATION_EN;
  }

  //
  // DRAM RON Training
  //
  if (DRAM_RON_DEFAULT == DRAM_RON_ENABLE) {
    setup->mem.optionsExt |= DRAM_RON_EN;
  } else {
    setup->mem.optionsExt &= ~DRAM_RON_EN;
  }

  //
  // RX ODT Training
  //
  if (RX_ODT_DEFAULT == RX_ODT_ENABLE) {
    setup->mem.optionsExt |= RX_ODT_EN;
  } else {
    setup->mem.optionsExt &= ~RX_ODT_EN;
  }

  //
  // MC RON Training
  //
  if (MC_RON_DEFAULT == MC_RON_ENABLE) {
    setup->mem.optionsExt |= MC_RON_EN;
  } else {
    setup->mem.optionsExt &= ~MC_RON_EN;
  }
  //
  // TxEq Calibration
  //
  if (TX_EQ_DEFAULT== TX_EQ_ENABLE) {
    setup->mem.optionsExt |= TX_EQ_EN;
  } else {
    setup->mem.optionsExt &= ~TX_EQ_EN;
  }

  //
  // IMode training
  //
   if (IMODE_DEFAULT == IMODE_ENABLE) {
    setup->mem.optionsExt |= IMODE_EN;
  } else {
    setup->mem.optionsExt &= ~IMODE_EN;
  }

  //
  // RX CTLE Training
  //

  if (RX_CTLE_DEFAULT == RX_CTLE_ENABLE) {
    setup->mem.optionsExt |= RX_CTLE_TRN_EN;
  } else {
    setup->mem.optionsExt &= ~RX_CTLE_TRN_EN;
  }

  //
  //  ddrFreqLimit
  //    0:Auto
  //    1:Force DDR-800
  //    2:Force DDR-1066
  //    3:Force DDR-1333
  //    4:Force DDR-1600
  //    5:Force DDR-1866
  //    6:Force DDR-2133
  //
  setup->mem.ddrFreqLimit = DDR3_FREQ_DEFAULT;
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  setup->mem.MemMapOut = 0;
#endif  //#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  setup->mem.enforcePOR = ENFORCE_POR_DEFAULT;

  // read/write 2TCLK preamble
  setup->mem.readPreamble = READ_PREAMBLE_DEFAULT;
  setup->mem.writePreamble = WRITE_PREAMBLE_DEFAULT;

  //
  // Force socket interleave to 1 if NUMA enabled
  //
  if (setup->common.numaEn == 1) {
    setup->mem.socketInter = 1;
    setup->mem.options |= NUMA_AWARE;
  } else {
    // Maximum of 2 way interleaving is supported
    setup->mem.socketInter = 2;
    setup->mem.options &= ~NUMA_AWARE;
  }

  setup->mem.setSecureEraseAllDIMMs = DISABLE_SECURE_ERASE;
  for(i = 0; i < MAX_SOCKET; i++) {
    for(j = 0; j < MAX_CH; j++) {
       setup->mem.setSecureEraseSktCh[i][j] = DISABLE_SECURE_ERASE;
    }
  }
  setup->mem.volMemMode = VOL_MEM_MODE_1LM;
  setup->mem.setTDPDIMMPower = POWER_DEFAULT;

  setup->mem.memInterleaveGran1LM = MEM_INT_GRAN_1LM_DEFAULT;

  setup->mem.dfxMemSetup.dfxPerMemMode = NON_PER_MEM_MODE;

  setup->mem.memInterleaveGran2LM = MEM_INT_GRAN_2LM_DEFAULT;
  setup->mem.dfxMemSetup.dfxMemInterleaveGranPMemNUMA = MEM_INT_GRAN_PMEM_NUMA_DEFAULT;
  setup->mem.dfxMemSetup.dfxCfgMask2LM = CFG_MASK_2LM_NORMAL;
  setup->mem.dfxMemSetup.dfxDimmManagement = DIMM_MGMT_AUTO;
  setup->mem.dfxMemSetup.dfxLoadDimmMgmtDriver = LOAD_NGN_DIMM_MGMT_DRIVER_AUTO;
  setup->mem.dfxMemSetup.dfxPartitionDDRTDimm = 0;
  for(i = 0; i < MAX_SOCKET * MAX_IMC; i++)
    setup->mem.dfxMemSetup.dfxPartitionRatio[i] = RATIO_40_10_50;
  //IMC interleave
  //    0:Auto
  //    1:1-way IMC interleave
  //    2:2-way IMC interleave
  setup->mem.imcInter = IMC_INTER_DEFAULT;
  //
  //  chInter
  //    0:Auto
  //    1:1-way channel interleave
  //    2:2-way channel interleave
  //    3:3-way channel interleave
  //    4:4-way channel interleave
  //
  setup->mem.chInter = CH_INTER_DEFAULT;

  //
  //  chInter
  //    0:Auto
  //    1:1-way rank interleave
  //    2:2-way rank interleave
  //    4:4-way rank interleave
  //    8:8-way rank interleave
  //
  setup->mem.rankInter = RANK_INTER_DEFAULT;

  //
  // Multi-Threaded MRC
  //
  if (MULTI_THREADED_DEFAULT == MULTI_THREADED_EN) {
     setup->mem.options |= MULTI_THREAD_MRC_EN;
  } else {
     setup->mem.options &= ~MULTI_THREAD_MRC_EN;
  }

  //
  // Allow SBE during memory training
  //
  if ( ALLOW_CORRECTABLE_ERROR_DEFAULT == ALLOW_CORRECTABLE_ERROR_ENABLE) {
     setup->mem.optionsExt |= ALLOW_CORRECTABLE_ERROR;
  } else {
     setup->mem.optionsExt &= ~ALLOW_CORRECTABLE_ERROR;
  }

  //
  // Duty Cycle Training
  //
  if (DUTY_CYCLE_AUTO == DUTY_CYCLE_ENABLE) {
    setup->mem.optionsExt |= DUTY_CYCLE_EN;
  } else {
    setup->mem.optionsExt &= ~DUTY_CYCLE_EN;
  }

  //
  // Read Vref Training
  //
  if (RX_VREF_DEFAULT == RX_VREF_ENABLE) {
     setup->mem.optionsExt |= RD_VREF_EN;
  } else {
     setup->mem.optionsExt &= ~RD_VREF_EN;
  }

  //
  // PDA
  //
  if (PDA_DEFAULT == PDA_ENABLE) {
     setup->mem.optionsExt |= PDA_EN;
  } else {
     setup->mem.optionsExt &= ~PDA_EN;
  }

  //
  // Turnaround Time Optimization
  //
  if (TURNAROUND_DEFAULT == TURNAROUND_ENABLE) {
     setup->mem.optionsExt |= TURNAROUND_OPT_EN;
  } else {
     setup->mem.optionsExt &= ~TURNAROUND_OPT_EN;
  }

#ifdef SSA_FLAG
  //
  // Enabling the BIOS SSA loader
  //
  setup->mem.enableBiosSsaLoader = BIOS_SSA_LOADER_DEFAULT;

  //
  // Enabling the BIOS SSA Stitched Mode (RMT)
  //
  setup->mem.enableBiosSsaRMT = BIOS_SSA_RMT_DEFAULT;

  //
  // Enabling the BIOS SSA Stitched Mode (RMT) on FCB
  //
  setup->mem.enableBiosSsaRMTonFCB = BIOS_SSA_RMT_FCB_DEFAULT;

  //
  // Enabling BiosSsaPerBitMargining
  //
  setup->mem.biosSsaPerBitMargining = BIOS_SSA_PER_BIT_MARGINING_DEFAULT;

  //
  // Enabling BiosSsaDisplayTables
  //
  setup->mem.biosSsaDisplayTables = BIOS_SSA_DISPLAY_TABLE_DEFAULT;

  //
  // Enabling BiosSsaPerDisplayPlots
  //
  setup->mem.biosSsaPerDisplayPlots = BIOS_SSA_DISPLAY_PLOTS_DEFAULT;

  //
  // Enabling BiosSsaBacksideMargining;
  //
  setup->mem.biosSsaBacksideMargining = BIOS_SSA_BACKSIDE_MARGINING_DEFAULT;


  //
  // Enabling BiosSsaEarlyReadIdMargining
  //
  setup->mem.biosSsaEarlyReadIdMargining = BIOS_SSA_EARLY_READ_ID_DEFAULT;

  //
  // Enabling BiosSsaStepSizeOverride;
  //
  setup->mem.biosSsaStepSizeOverride = BIOS_SSA_STEP_SIZE_OVERRIDE_DEFAULT;


  //
  // Enabling BiosSsaDebugMessages
  //
  setup->mem.biosSsaDebugMessages = BIOS_SSA_RMT_DEBUG_MSGS_DEFAULT;

  //
  //Exponential loop count for single rank test
  //
  setup->mem.biosSsaLoopCount = BIOS_SSA_LOOP_CNT_DEFAULT;

  //
  //  Step size of RxDqs
  //
  setup->mem.biosSsaRxDqs = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of RxVref
  //
  setup->mem.biosSsaRxVref = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of TxDq
  //
  setup->mem.biosSsaTxDq = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of TxVref
  //
  setup->mem.biosSsaTxVref = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of CmdAll
  //
  setup->mem.biosSsaCmdAll = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of CmdVref
  //
  setup->mem.biosSsaCmdVref = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of CtlVref
  //
  setup->mem.biosSsaCtlAll = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of EridDelay
  //
  setup->mem.biosSsaEridDelay = BIOS_SSA_RMT_DEFAULT_VALUE;
  //
  //  Step size of EridVref
  //
  setup->mem.biosSsaEridVref = BIOS_SSA_RMT_DEFAULT_VALUE;
#endif //SSA_FLAG

#ifdef LRDIMM_SUPPORT
  //
  // Disable Backside RMT
  //
  setup->mem.enableBacksideRMT = BACKSIDE_RMT_DEFAULT;

  //
  // Disable Backside CMD RMT
  //
  setup->mem.enableBacksideCMDRMT = BACKSIDE_CMD_RMT;
#endif

  //
  // NVMDIMM BCOM Margining
  //
  setup->mem.enableNgnBcomMargining = NGN_BCOM_MARGINING;

  //
  // Write Vref Training
  //
  if (TX_VREF_DEFAULT == TX_VREF_ENABLE) {
     setup->mem.optionsExt |= WR_VREF_EN;
  } else {
     setup->mem.optionsExt &= ~WR_VREF_EN;
  }

  //
  // Command Vref Training
  //

  if (CMD_VREF_DEFAULT == CMD_VREF_ENABLE) {
    setup->mem.optionsExt |= CMD_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~CMD_VREF_EN;
  }

  //
  // LRDIMM Backside Vref Training
  //
  if (LRDIMM_BACKSIDE_VREF_DEFAULT == LRDIMM_BACKSIDE_VREF_ENABLE) {
    setup->mem.optionsExt |= LRDIMM_BACKSIDE_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~LRDIMM_BACKSIDE_VREF_EN;
  }

  //
  // LRDIMM Backside Read DQ Centering
  //
  if (LRDIMM_RX_DQ_CENTERING_DEFAULT == LRDIMM_RX_DQ_CENTERING_ENABLE) {
    setup->mem.optionsExt |= LRDIMM_RX_DQ_CENTERING;
  } else {
    setup->mem.optionsExt &= ~LRDIMM_RX_DQ_CENTERING;
  }  

  //
  //  LRDIMM WR VREF
  //
  if (LRDIMM_WR_VREF_DEFAULT == LRDIMM_WR_VREF_ENABLE) {
    setup->mem.optionsExt |= LRDIMM_WR_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~LRDIMM_WR_VREF_EN;
  }

  //
  //  LRDIMM RD VREF
  //
  if (LRDIMM_RD_VREF_DEFAULT == LRDIMM_RD_VREF_ENABLE) {
    setup->mem.optionsExt |= LRDIMM_RD_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~LRDIMM_RD_VREF_EN;
  }

  //
  // Per bit deskew
  //
  setup->mem.perBitDeSkew = BIT_DESKEW_DEFAULT;

  //
  // DDR4 SMBus Speed
  //
  setup->mem.SpdSmbSpeed = SMB_CLK_DEFAULT;

  //
  // Round Trip Latency
  //
  if (ROUND_TRIP_LATENCY_DEFAULT == ROUND_TRIP_LATENCY_ENABLE) {
     setup->mem.optionsExt |= ROUND_TRIP_LATENCY_EN;
  } else {
     setup->mem.optionsExt &= ~ROUND_TRIP_LATENCY_EN;
  }

  //
  // Display Eye Diagrams
  //
  if (EYE_DIAGRAM_AUTO == EYE_DIAGRAM_EN) {
    setup->mem.options |= DISPLAY_EYE_EN;
  } else {
    setup->mem.options &= ~DISPLAY_EYE_EN;
  }


  //
  // High Address Enable: default to disabled
  //
  setup->mem.dfxMemSetup.dfxOptions &= ~HIGH_ADDR_EN;

  //
  // CR Mixed SKU
  //
  if (CR_MIXED_SKU_AUTO == CR_MIXED_SKU_EN) {
    setup->mem.dfxMemSetup.dfxOptions |= CR_MIXED_SKU;
  } else {
    setup->mem.dfxMemSetup.dfxOptions &= ~CR_MIXED_SKU;
  }

  //
  //  HighAddrBitStart
  //    Address bit to start mapping memory above 4GB
  //
  setup->mem.dfxMemSetup.dfxHighAddrBitStart = 0;

  //
  //  lowMemChannel
  //    Channel to use for low memory, if chInterleveave = 1
  //
  setup->mem.dfxMemSetup.dfxLowMemChannel = 0;

  setup->mem.dfxMemSetup.dfxMaxNodeInterleave = 0;

  //
  //  Per Bit Margin Data.
  //
  if (PER_BIT_MARGIN_DEFAULT == PER_BIT_MARGIN_ENABLE) {
     setup->mem.optionsExt |= PER_BIT_MARGINS;
  } else {
     setup->mem.optionsExt &= ~PER_BIT_MARGINS;
  }

  //
  //  dimmTypeSupport
  //    0:RDIMM
  //    1:UDIMM
  //    2:RDIMM and UDIMM
  //
  setup->mem.dimmTypeSupport = MEM_TYPE_AUTO;

  //
  //  lrdimmModuleDelay
  //    0:Auto
  //    1:Disable
  //
  setup->mem.lrdimmModuleDelay = MODULE_DELAY_AUTO;

  //
  //  spareErrTh
  //
  setup->mem.spareErrTh = 0x7FFF;
  //
  // NGN Die Sparing status and Aggressiveness
  //
  setup->mem.DieSparing = NGN_DIE_SPARING_EN;
  setup->mem.DieSparingAggressivenessLevel = NGN_DIE_SPARING_AGGRESSIVENESS_LEVEL_DEFAULT;
  //
  // Address range scrub setup, disabled by default
  //
  setup->mem.NgnAddressRangeScrub = NGN_ADDRESS_RANGE_SCRUB_DIS;
  //
   // NGN Host Alerts for Address Indirection Table Error, configured to signal Poison by default
  //
  setup->mem.NgnHostAlertAit = NGN_HOST_ALERT_POISON_EN;
  //
  // NGN Host Alerts for DIMM Physical Address Error, configured to signal Poison by default
  //
  setup->mem.NgnHostAlertDpa = NGN_HOST_ALERT_POISON_EN;
  //
  //  patrolScrubDuration
  //    Patrol Scrub duration defaults to 24 hours
  //
  setup->mem.patrolScrubDuration = PATROL_SCRUB_DURATION_DEFAULT;

  //
  // Leaky bucket values (1 leak per 10 days for 1GHz DCLK)
  //
  setup->mem.leakyBktLo = LEAKY_BUCKET_LO;
  setup->mem.leakyBktHi = LEAKY_BUCKET_HI;

  //
  // DLL Reset Test
  //
  setup->mem.dllResetTestLoops = DLL_RESET_TEST_LOOPS;


#ifdef MARGIN_CHECK
  //
  //  rmtPatternLength
  //    Pattern length for the rank margin tool.  One pattern = the size of the Write Data Buffer
  //
  setup->mem.rmtPatternLength = RMT_PATTERN_LENGTH;
  setup->mem.rmtPatternLengthExt = RMT_PATTERN_LENGTH_CMD_CTL_VREF;
#endif  // MARGIN_CHECK

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Enable the memory controller
    //
    setup->mem.socket[socket].enabled = 1;

    //
    // Set options
    //
    setup->mem.socket[socket].options = 0;

    for (ch = 0; ch < MAX_CH; ch++) {
      //
      // Enable this channel
      //
      setup->mem.socket[socket].ddrCh[ch].enabled = 1;
      //
      // MAX DIMM slots available per channel
      //
      setup->mem.socket[socket].ddrCh[ch].numDimmSlots = MAX_DIMM;
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.compId = NO_DEV;
      setup->mem.socket[socket].ddrCh[ch].rankmask = 0xFF;

    } // ch loop

  } // socket loop


  setup->mem.SrefProgramming = 0;
  setup->mem.CkeProgramming = 0;

  //
  // pkgc self refresh enabled by default
  //
  setup->mem.PkgcSrefEn = 1;

  //
  // Initialize PPR options
  //
  setup->mem.pprType        = PPR_TYPE_AUTO;
  setup->mem.pprErrInjTest  = PPR_DISABLED;

  //
  // mem flows
  //
  setup->mem.memFlows = MEM_FLOWS;
  setup->mem.memFlowsExt  = MEM_FLOWS;

  // NVMDIMM transactor
  setup->common.ddrtXactor = DDRT_XACTOR_DEFAULT;

  //
  // NGN options
  //
  if (LOCK_NGN_CSR_AUTO == LOCK_NGN_CSR_ENABLE) {
    setup->mem.optionsNgn |= LOCK_NGN_CSR;
  }else {
    setup->mem.optionsNgn &= ~LOCK_NGN_CSR;
  }

  if (NGN_CMD_TIME_AUTO == NGN_CMD_2N) {
    setup->mem.optionsNgn |= NGN_CMD_TIME;
  }else {
    setup->mem.optionsNgn &= ~NGN_CMD_TIME;
  }

  if (NGN_ECC_CORR_AUTO == NGN_ECC_CORR_ENABLE) {
    setup->mem.optionsNgn |= NGN_ECC_CORR;
  }else {
    setup->mem.optionsNgn &= ~NGN_ECC_CORR;
  }

  if (NGN_ECC_WR_CHK_AUTO == NGN_ECC_WR_CHK_ENABLE) {
    setup->mem.optionsNgn |= NGN_ECC_WR_CHK;
  }else {
    setup->mem.optionsNgn &= ~NGN_ECC_WR_CHK;
  }

  if (NGN_ECC_RD_CHK_AUTO == NGN_ECC_RD_CHK_ENABLE) {
    setup->mem.optionsNgn |= NGN_ECC_RD_CHK;
  }else {
    setup->mem.optionsNgn &= ~NGN_ECC_RD_CHK;
  }

  if (CR_MONETIZATION_AUTO == CR_MONETIZATION_ENABLE) {
     setup->mem.optionsNgn |= CR_MONETIZATION ;
  } else {
     setup->mem.optionsNgn &= ~CR_MONETIZATION;
  }

  if (NGN_DEBUG_LOCK_AUTO == NGN_DEBUG_LOCK_ENABLE) {
    setup->mem.optionsNgn |= NGN_DEBUG_LOCK;
  } else {
    setup->mem.optionsNgn &= ~NGN_DEBUG_LOCK;
  }

  if (NGN_ARS_ON_BOOT_AUTO == NGN_ARS_ON_BOOT_EN) {
    setup->mem.optionsNgn |= NGN_ARS_ON_BOOT;
  } else {
    setup->mem.optionsNgn &= ~NGN_ARS_ON_BOOT;
  }

  //
  // CR QoS Configuration
  //
  setup->mem.crQosConfig = CR_QOS_DEFAULT;

  // POR values
  setup->mem.Disddrtopprd = 1;
  setup->mem.Blockgnt2cmd1cyc = 1;

  //
  // One Rank Timing Mode Option
  //
  setup->mem.oneRankTimingModeEn = ONE_RANK_TIMING_MODE_DEFAULT;

  //
  // IMC BCLK
  //
  setup->mem.imcBclk = IMC_BCLK_AUTO;

  //
  // SPD CRC Check
  //
  if (SPD_CRC_CHECK_DEFAULT == SPD_CRC_CHECK_EN) {
     setup->mem.optionsExt |= SPD_CRC_CHECK;
  } else {
     setup->mem.optionsExt &= ~SPD_CRC_CHECK;
  }
#ifdef MEM_NVDIMM_EN
  setup->mem.ADREn = 1;
  setup->mem.LegacyADRModeEn = 0;
  setup->mem.ADRDataSaveMode = ADR_NVDIMM;
  setup->mem.interNVDIMMS = 0;
  setup->mem.eraseArmNVDIMMS = 1;
  setup->mem.restoreNVDIMMS = 1;
#endif
}
