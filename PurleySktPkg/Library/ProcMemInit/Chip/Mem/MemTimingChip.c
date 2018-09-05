//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.02
//      Bug Fixed:  Add tRWSR w/a setup option for Samsung DIMMs.
//      Reason:     Improve memory RX margin
//      Auditor:    Stephen Chen
//      Date:       Aug/18/2017
//
//  Rev. 1.01
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//  Rev. 1.00
//      Bug Fixed:  Added Samsung STB_OverrideRDtoWRTiming w/a
//      Reason:     It will set max value to all channel tRWSR for better margin
//                  result, comment out at this moment.
//      Auditor:    Stephen Chen
//      Date:       Mar/29/2017
//
//****************************************************************************
/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
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
#include "RcRegs.h"
#include "SysFuncChip.h"
#include "MemProjectSpecific.h"
#include "MemHostChip.h"
#include "MemApiSkx.h"
#include "MemFuncChip.h"
#include "SysHostChip.h"
#define DDRT_TURNAROUND_EN 1

extern  UINT8   tMRD_DDR4;
extern  UINT8   tPARRECOVERY[MAX_SUP_FREQ];
extern  UINT8   maxRefAllowed[MAX_DENSITY];
extern  UINT8   PiTicks;

static memTimingMinMaxType memTimingMinMax[] = {
    { tREFI,      0x0, 0x7FFF, CALL_TABLE_STRING("tREFI")     },
    { tRFC,       0x0, 0x03FF, CALL_TABLE_STRING("tRFC")      },
    { tRAS,       0x0, 0x003F, CALL_TABLE_STRING("tRAS")      },
    { tCWL,       0x0, 0x001F, CALL_TABLE_STRING("tCWL")      },
    { tCL,        0x0, 0x001F, CALL_TABLE_STRING("tCL")       },
    { tRP,        0x0, 0x001F, CALL_TABLE_STRING("tRP")       },
    { tRCD,       0x0, 0x001F, CALL_TABLE_STRING("tRCD")      },
    { tRRD,       0x0, 0x0007, CALL_TABLE_STRING("tRRD")      },
    { tRRD_S,     0x0, 0x0007, CALL_TABLE_STRING("tRRD_S")    },
    { tRRD_L,     0x0, 0x0007, CALL_TABLE_STRING("tRRD_L")    },
    { tWTR,       0x0, 0x000F, CALL_TABLE_STRING("tWTR")      },
    { tWTR_S,     0x0, 0x000F, CALL_TABLE_STRING("tWTR_S")    },
    { tWTR_L,     0x0, 0x000F, CALL_TABLE_STRING("tWTR_L")    },
    { tCCD,       0x0, 0x000B, CALL_TABLE_STRING("tCCD")      },
    { tCCD_S,     0x0, 0x0007, CALL_TABLE_STRING("tCCD_S")    },
    { tCCD_L,     0x0, 0x0007, CALL_TABLE_STRING("tCCD_L")    },
    { tRTP,       0x0, 0x000F, CALL_TABLE_STRING("tRTP")      },
    { tCKE,       0x0, 0x000F, CALL_TABLE_STRING("tCKE")      },
    { tFAW,       0x0, 0x003F, CALL_TABLE_STRING("tFAW")      },
    { tWR,        0x0, 0x007F, CALL_TABLE_STRING("tWR")       },
    { tPRPDEN,    0x0, 0x0003, CALL_TABLE_STRING("tPRPDEN")   },
    { tXP,        0x0, 0x003F, CALL_TABLE_STRING("tXP")       },
    { tZQCS,      0x0, 0x00FF, CALL_TABLE_STRING("tZQCS")     },
    { tSTAB,      0x0, 0xFFFFF, CALL_TABLE_STRING("tSTAB")     },
    { tMOD,       0x0, 0x001F, CALL_TABLE_STRING("tMOD")      },
    { tREFIx9,    0x0, 0x007F, CALL_TABLE_STRING("tREFIx9")   },
    { tXPDLL,     0x0, 0x001F, CALL_TABLE_STRING("tXPDLL")    },
    { tXSOFFSET,  0x0, 0x000F, CALL_TABLE_STRING("tXSOFFSET") },
    { tRRDR,      0x0, 0x0007, CALL_TABLE_STRING("tRRDR")     },
    { tRRDD,      0x0, 0x0007, CALL_TABLE_STRING("tRRDD")     },
    { tRC,        0x0, 0x007F, CALL_TABLE_STRING("tRC")       },
    { tRDA,       0x0, 0x007F, CALL_TABLE_STRING("tRDA")      },
    { tWRA,       0x0, 0x007F, CALL_TABLE_STRING("tWRA")      },
    { tWRPRE,     0x0, 0x007F, CALL_TABLE_STRING("tWRPRE")    },
    { tRDPDEN,    0x0, 0x007F, CALL_TABLE_STRING("tRDPDEN")   },
    { tWRPDEN,    0x0, 0x007F, CALL_TABLE_STRING("tWRPDEN")   },
};

// Local Prototypes
void   InitTurnAroundTimes(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, TCRWP_MCDDC_CTL_STRUCT *tcrwp, TCOTHP2_MCDDC_CTL_STRUCT *tcothp2, TCLRDP_MCDDC_CTL_STRUCT *tclrdp); // LOCAL
void   Sett_zqcs(PSYSHOST host, UINT8 socket, TCZQCAL_MCDDC_CTL_STRUCT *tczqcal);
void   SetRdimmTimingCntl(PSYSHOST host, RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT *RdimmTimingCntl, UINT8 aepPresent);        // LOCAL
void   PartialWrStarvationCounter(PSYSHOST host, UINT8 socket, UINT8 ch, PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_STRUCT *pmmStarvCntr);
#ifdef  LRDIMM_SUPPORT
void   TimeConstraintLrdimm(PSYSHOST host, UINT8 socket, TCRAP_MCDDC_CTL_STRUCT *tcrap, TCRWP_MCDDC_CTL_STRUCT tcrwp, TCDBP_MCDDC_CTL_STRUCT tcdbp);       // LOCAL
#endif // LRDIMM_SUPPORT
void   Ddr3MrsTimingReg(PSYSHOST host, UINT8 socket, UINT8 ch, RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT RdimmTimingCntl);
UINT8  Sett_ckev(PSYSHOST host, RDIMMTIMINGCNTL2_MCDDC_CTL_STRUCT *RdimmTimingCntl2);
void   SaveRefreshRate(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 refreshRate);
#ifdef  LRDIMM_SUPPORT
void   SetEncodeCSMode(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 *lrdimmQrDimmBitMask);
#endif
void   SetStaggerRefEn(PSYSHOST host, UINT8 socket, UINT8 ch);
void   IdtLrbufWA(PSYSHOST host, UINT8 socket, UINT8 ch, WORK_AROUND_TYPE wa, void *data);
void   SetMiscDelay(PSYSHOST host, UINT8 socket, UINT8 ch, void *tcrap);
void   SetMasterDllWakeUpTimer(PSYSHOST host, UINT8 socket, UINT8 ch);
static UINT8  Check2xRefreshWA(PSYSHOST host, UINT8 socket, UINT8 ch);


void
ProgramTimings (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch
               )
/*++

  Programs timing parameters

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param ch          - Channel number
  @param burstLength - Burst length

  @retval N/A

--*/
{
  UINT8                                       fourHighstack = 0;
  UINT8                                       dimm;
  UINT8                                       onlySingleRanks;
  UINT16                                      tREFI;
  UINT16                                      t_stagger_ref = 0;
  UINT16                                      tRfcMtb;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  TCRFTP_MCDDC_CTL_STRUCT                     tcrftp;
  TCDBP_MCDDC_CTL_STRUCT                      tcdbp;
  TCRFP_MCDDC_CTL_STRUCT                      tcrfp;
  TCOTHP_MCDDC_CTL_STRUCT                     tcothp;
  TCSTAGGER_REF_MCDDC_CTL_STRUCT              tcstagger;
  TCRWP_MCDDC_CTL_STRUCT                      tcrwp;
  TCRAP_MCDDC_CTL_STRUCT                      tcrap;
  TCZQCAL_MCDDC_CTL_STRUCT                    tczqcal;
  IDLETIME_MCDDC_CTL_STRUCT                   IdleTime;
  RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT            RdimmTimingCntl;
  RDIMMTIMINGCNTL2_MCDDC_CTL_STRUCT           RdimmTimingCntl2;
  PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_STRUCT  pmmStarvCntr;
  WDBWM_MCDDC_CTL_STRUCT                      wdbwm;
  WDB_INIT_PSN_CTL_MCDDC_DP_STRUCT            wdbInitCtl;
  TCOTHP2_MCDDC_CTL_STRUCT                    tcothp2;
  TDDR4_MCDDC_CTL_STRUCT                      tddr4;
  WMM_READ_CONFIG_MCDDC_CTL_STRUCT            wmmReadConfig;
  UINT8                                       refreshRate;
  UINT8                                       aepPresent;
  TCLRDP_MCDDC_CTL_STRUCT                     tclrdp;
#ifdef  LRDIMM_SUPPORT
  UINT32                                      lrdimmQrDimmBitMask = 0;
#endif

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  if ((host->setup.mem.options & MEM_OVERRIDE_EN) && host->setup.mem.inputMemTime.tREFI) {
    tREFI = host->setup.mem.inputMemTime.tREFI;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Refresh rate = %d\n", host->setup.mem.inputMemTime.tREFI));
  } else
  {
    //
    // Establish Refresh Rate
    // Units in 0.1x of standard 7.8 usec interval
    //
    if ((host->setup.mem.customRefreshRate >= 20) && (host->setup.mem.customRefreshRate <= 40)) {
      //
      // BIOS HSD 3617584: Add MRC option for Custom Refresh Rate between 2x and 4x
      //
      refreshRate = host->setup.mem.customRefreshRate;

    } else if (Check2xRefreshWA(host, socket, ch)){

      refreshRate = 20;

    } else {
      //
      // Default to legacy setting of 1x
      //
      refreshRate = 10;
    }

    //
    // Save Refresh Rate for later use
    //
    SaveRefreshRate(host, socket, ch, refreshRate);

    tREFI = (UINT16)GettREFI(host, socket, ch);
  }
  //
  // Decreasing refresh period by 2% to be safe
  //
  //tREFI                 = tREFI - (tREFI / 50);

  tcrftp.Data           = MemReadPciCfgEp (host, socket, ch, TCRFTP_MCDDC_CTL_REG);
  tcrftp.Bits.t_refi    = tREFI;
  tcrftp.Bits.t_refix9  = GettREFIx9(host, tREFI);
  tcrftp.Bits.t_rfc     = GettRFC(host, socket, ch);
  //MRC HSD 4930526: Need to increase tRFC by 1 cycle at 3200 in order to meet JEDEC spec with t_xsoffset = 0xF
  //Increasing t_rfc by 1 for frequecies 3200 and above. This should be revisited if JEDEC spec is updated for even greater frequencies
  if (host->nvram.mem.socket[socket].ddrFreq >= DDR_3200) {
    tcrftp.Bits.t_rfc = tcrftp.Bits.t_rfc + 1;
  }
  MemWritePciCfgEp (host, socket, ch, TCRFTP_MCDDC_CTL_REG, tcrftp.Data);

  //
  // REF-REF separation
  //
  tcstagger.Data = MemReadPciCfgEp (host, socket, ch, TCSTAGGER_REF_MCDDC_CTL_REG);

  //
  // Set refresh stagger to 1/3 tRFC, rounded up to next cycle
  //
  tRfcMtb = (*channelNvList)[ch].common.tRFC;
  t_stagger_ref = TimeBaseToDCLK(host, socket, 0, 0, (tRfcMtb + 2) / 3, 0);

  //
  // Default to only single rank DIMM's present
  //
  onlySingleRanks = 1;

  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    OutputExtendedCheckpoint((host, STS_GLOBAL_EARLY, SUB_PROG_TIMINGS, (UINT16)((socket << 8)|(ch << 4)|(dimm))));

    //
    // Indicate if any of the DIMMs on this channel has a 4 high stack
    //
    if ((*dimmNvList)[dimm].dieCount == 4) {
      fourHighstack = 1;
    }

    //
    // Set flag if this DIMM has more than 1 rank
    //
    if ((*dimmNvList)[dimm].numDramRanks > 1) {
      onlySingleRanks = 0;
    }

  } // dimm loop

  //
  // Single rank configs get hard coded to 3. All other configs get set to 1/3 tRFC.
  //
  if (onlySingleRanks){
    tcstagger.Bits.t_stagger_ref = 3;
  } else {
    tcstagger.Bits.t_stagger_ref = t_stagger_ref;
  }

  //
  // Program short loop when 3DS DIMM present
  //
  if ((*channelNvList)[ch].encodedCSMode == 2) {

    //
    // Set shrtloop_stagger_ref to (tRFC + 10ns) / 3, rounded up to next cycle
    //
    tcstagger.Bits.t_shrtloop_stagger_ref = TimeBaseToDCLK(host, socket, 0, 0, (tRfcMtb + 80 + 2) / 3, 0);

    //
    // shrtloop_num controls the refresh stagger between subranks
    // For 4H stack this constraint occurs after subrank 0, 1, 2
    // For 2H stack this occurs after subrank 0
    // Mixed case must use the largest stack size
    //
    if (fourHighstack == 1) {
      tcstagger.Bits.t_shrtloop_num = 3;
    } else {
      tcstagger.Bits.t_shrtloop_num = 1;
    }
    tcstagger.Bits.maxnum_active_reffsm = 0;
  }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "t_stagger_ref = 0x%x\n", tcstagger.Bits.t_stagger_ref));

  MemWritePciCfgEp (host, socket, ch, TCSTAGGER_REF_MCDDC_CTL_REG, tcstagger.Data);

  SetStaggerRefEn(host, socket, ch);

  //
  // Timing Contstraints DDR3 Bin Parameters
  //
  tcdbp.Data        = MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.t_ras  = GettRAS(host, socket, ch);
  tcdbp.Bits.t_cwl  = GettCWL(host, socket, ch);
  tcdbp.Bits.t_cl   = GettCL(host, socket, ch);

  tcrwp.Data   = MemReadPciCfgEp (host, socket, ch, TCRWP_MCDDC_CTL_REG);
  tcothp2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
  wdbInitCtl.Data = (UINT8)MemReadPciCfgEp(host, socket, ch, WDB_INIT_PSN_CTL_MCDDC_DP_REG);

  //
  // Initial turnaround times
  //
  tclrdp.Data  = MemReadPciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG);
  InitTurnAroundTimes(host, socket, ch, dimm, &tcrwp, &tcothp2, &tclrdp);

  tcdbp.Bits.t_rp   = GettRP(host, socket, ch);
  tcdbp.Bits.t_rcd  = GettRCD(host, socket, ch);
  MemWritePciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG, tcdbp.Data);


  //
  // DDR4 Timing Parameters
  //
  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
#ifdef  LRDIMM_SUPPORT
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      SetEncodeCSMode(host, socket, ch, dimm, &lrdimmQrDimmBitMask);
    } // dimm loop
#endif

    tddr4.Data         = MemReadPciCfgEp (host, socket, ch, TDDR4_MCDDC_CTL_REG);
    tddr4.Bits.t_rrd_l = GettRRD_L(host, socket, ch);
    tddr4.Bits.t_wtr_l = GettWTR_L(host, socket, ch);
    tddr4.Bits.t_ccd_l = (GetTccd_L(host, socket, ch) - 4);
    tddr4.Bits.t_ccd_wr_l = tddr4.Bits.t_ccd_l;
    if ((host->setup.mem.optionsExt & WR_CRC) && (tddr4.Bits.t_ccd_wr_l < 7)) {
      tddr4.Bits.t_ccd_wr_l += 1;
    }
    //MRC: 5330161 Si WA: For MC with x8 DIMM and static virtual lockstep enabled, in 2S system or when poison is enabled, increase the read to read time(tCCD) to a minimum of 8.
    if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
      if (((*channelNvList)[ch].features & X8_PRESENT) && (host->nvram.mem.RASmode == STAT_VIRT_LOCKSTEP) && ((host->var.common.numCpus == 2) || (wdbInitCtl.Bits.poisonall == 1))) {
        if (tcrwp.Bits.t_ccd < 4) {
          tcrwp.Bits.t_ccd = 4;
        }
      }
    }

    // cache t_ccd values
    (*channelNvList)[ch].common.tCCD      = tcrwp.Bits.t_ccd & 0xff;
    (*channelNvList)[ch].common.tCCD_WR   = tclrdp.Bits.t_ccd_wr & 0xff;
    (*channelNvList)[ch].common.tCCD_L    = tddr4.Bits.t_ccd_l & 0xff;
    (*channelNvList)[ch].common.tCCD_WR_L = tddr4.Bits.t_ccd_wr_l & 0xff;

  #ifdef LRDIMM_SUPPORT
    tddr4.Bits.qrlrdimm2cs = Saturate(0x7, lrdimmQrDimmBitMask);
  #endif
    MemWritePciCfgEp (host, socket, ch, TDDR4_MCDDC_CTL_REG, tddr4.Data);
  }

  //
  // Timing Constraints Regular Access Parameters
  //
  tcrap.Data              = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  tcrap.Bits.t_rrd        = GettRRD(host, socket, ch);
  tcrap.Bits.t_rtp        = GettRTP(host, socket);
  tcrap.Bits.t_cke        = GettCKE(host, socket);
  tcrap.Bits.t_wtr        = GettWTR_S(host, socket);
#ifdef LRDIMM_SUPPORT
  TimeConstraintLrdimm(host, socket, &tcrap, tcrwp, tcdbp);
#endif
  tcrap.Bits.t_faw        = (*channelNvList)[ch].common.nFAW;
  tcrap.Bits.t_wr = GettWR(host, socket);

  // tPRPDen, tACTPDEN, and tREFPDEN needs to be 2 for 2133 and higher
  if (host->nvram.mem.socket[socket].ddrFreq >= DDR_2133) {
    tcrap.Bits.t_prpden = 2;
  }

  if (host->var.common.bootMode == S3Resume ||
      ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode != ColdBoot))) {
    tcrap.Bits.cmd_stretch  = (*channelNvList)[ch].timingMode;
  } else {
    // Set 3N timing for basic training
    (*channelNvList)[ch].trainTimingMode = TIMING_3N;
    tcrap.Bits.cmd_stretch = (*channelNvList)[ch].trainTimingMode;
  }

  IdtLrbufWA(host, socket, ch, t_cke, &tcrap);
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tcrap.Data);

  SetMiscDelay(host, socket, ch, (void*)&tcrap);

  tcothp.Data         = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
  IdtLrbufWA(host, socket, ch, t_xp, &tcothp);
  IdtLrbufWA(host, socket, ch, t_odt_oe, NULL);

  MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, tcothp.Data);

  //
  // Timing Constraints DDR3 Refresh Timing Parameters
  //
  tcrfp.Data              = MemReadPciCfgEp (host, socket, ch, TCRFP_MCDDC_CTL_REG);
  tcrfp.Bits.orefni       = OREFNI;
  tcrfp.Bits.ref_hi_wm    = REF_HI_WM;
  tcrfp.Bits.ref_panic_wm = REF_PANIC_WM;
  MemWritePciCfgEp (host, socket, ch, TCRFP_MCDDC_CTL_REG, tcrfp.Data);

  //
  // Timing Constraints DDR3 Self-Refresh Timing Parameters
  //
  IdtLrbufWA(host, socket, ch, t_xsdll, NULL);

  //
  // Timing Constraints DDR3 ZQ Calibration Timing Parameters
  //
  tczqcal.Data            = MemReadPciCfgEp (host, socket, ch, TCZQCAL_MCDDC_CTL_REG);
  Sett_zqcs(host, socket, &tczqcal);
  tczqcal.Bits.zqcsperiod = ZQCS_PERIOD;
  MemWritePciCfgEp (host, socket, ch, TCZQCAL_MCDDC_CTL_REG, tczqcal.Data);

  //
  // Initial Page Policy and Timing Parameters
  //
  IdleTime.Data = MemReadPciCfgEp (host, socket, ch, IDLETIME_MCDDC_CTL_REG);
  // set max value for idle page reset value
  IdleTime.Bits.idle_page_rst_val = IDLE_PAGE_RST_VAL_MAX;
  IdleTime.Bits.win_size          = WIN_SIZE;
  IdleTime.Bits.ppc_th            = PPC_TH;
  IdleTime.Bits.opc_th            = OPC_TH;
  //
  // Set to open page mode for training
  //
  IdleTime.Bits.adapt_pg_clse = 0;

  MemWritePciCfgEp (host, socket, ch, IDLETIME_MCDDC_CTL_REG, IdleTime.Data);

  RdimmTimingCntl.Data        = MemReadPciCfgEp (host, socket, ch, RDIMMTIMINGCNTL_MCDDC_CTL_REG);

  RdimmTimingCntl.Bits.t_stab = GettCKSRX(host, socket);

  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    aepPresent = 0;
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++){
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        aepPresent = 1;
        break;
      }
    }
    SetRdimmTimingCntl(host, &RdimmTimingCntl, aepPresent);
  } // if RDIMM

  //
  // RDIMM Timing Control 2
  //
  RdimmTimingCntl2.Data         = MemReadPciCfgEp (host, socket, ch, RDIMMTIMINGCNTL2_MCDDC_CTL_REG);
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    RdimmTimingCntl2.Bits.t_ckoff = RDIMM_TIMING_CNTL2_TCKOFF;
  } else {
    RdimmTimingCntl2.Bits.t_ckoff = RdimmTimingCntl.Bits.t_stab;
  }

  if (!Sett_ckev(host, &RdimmTimingCntl2)) {
    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
      RdimmTimingCntl2.Bits.t_ckev = RDIMM_TIMING_CNTL2_TCKEV_DDR_2133;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666){
      RdimmTimingCntl2.Bits.t_ckev = RDIMM_TIMING_CNTL2_TCKEV_DDR_2666;
    } else {
      RdimmTimingCntl2.Bits.t_ckev = RDIMM_TIMING_CNTL2_TCKEV;
    }
  }

  MemWritePciCfgEp (host, socket, ch, RDIMMTIMINGCNTL2_MCDDC_CTL_REG, RdimmTimingCntl2.Data);
  MemWritePciCfgEp (host, socket, ch, RDIMMTIMINGCNTL_MCDDC_CTL_REG, RdimmTimingCntl.Data);

  Ddr3MrsTimingReg(host, socket, ch, RdimmTimingCntl);

  //
  // DDR3 Partial Write Starvation Counter
  //
  PartialWrStarvationCounter(host, socket, ch, &pmmStarvCntr);

  //
  // DDR3 MRS Timing Register
  //
  wdbwm.Data            = MemReadPciCfgEp (host, socket, ch, WDBWM_MCDDC_CTL_REG);
  wdbwm.Bits.wmm_enter  = WMM_ENTRY;
  wdbwm.Bits.wmm_exit   = WMM_EXIT;
  if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX))  wdbwm.Bits.starve_cnt = 0x00;
  else wdbwm.Bits.starve_cnt = STARVATION_COUNTER;
  // Make sure WMMEnter is less than or equal to WPQ_IS - 1 for > A0.
  if (wdbwm.Bits.wmm_enter > (pmmStarvCntr.Bits.wpq_is - 1)) {
    wdbwm.Bits.wmm_enter = pmmStarvCntr.Bits.wpq_is - 1;
  }

  // Make sure WMMEnter is greater than or equal to 2.
  if (wdbwm.Bits.wmm_enter < 2) {
    wdbwm.Bits.wmm_enter = 2;
  }
  // Make sure WMMExit is greater than 0.  We will never exit WMM if it is set to 0.
  if (wdbwm.Bits.wmm_exit == 0) {
    wdbwm.Bits.wmm_exit = 1;
  }
  // Make sure WMMExit is less than WMMEnter.
  if (wdbwm.Bits.wmm_exit >= wdbwm.Bits.wmm_enter) {
    wdbwm.Bits.wmm_exit = wdbwm.Bits.wmm_enter - 1;
  }

  MemWritePciCfgEp (host, socket, ch, WDBWM_MCDDC_CTL_REG, wdbwm.Data);

  //
  // s4929265: Cloned From SKX Si Bug Eco: ww48 & ww51 2socket 1LM model pending IDI WCiL and missing ExtCmp in raccoon regression
  // If SKX A0 and not a 1LM 1S configuration, set: MC_MISC_CRNODE_CHx_MCx_CR_WMM_READ_CONFIG.dis_opp_rd = 1
  // Not the best idea to check host->setup.mem.volMemMode, but should be safe to disable this feature, performance impact only.
  //
  if ((host->setup.mem.options & DISABLE_WMM_OPP_READ) ||
      ((host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) &&
       (host->var.common.numCpus > 1 || host->setup.mem.volMemMode == VOL_MEM_MODE_2LM))) {
    wmmReadConfig.Data = MemReadPciCfgEp (host, socket, ch, WMM_READ_CONFIG_MCDDC_CTL_REG);
    wmmReadConfig.Bits.dis_opp_rd = 1;
    MemWritePciCfgEp (host, socket, ch, WMM_READ_CONFIG_MCDDC_CTL_REG, wmmReadConfig.Data);
  }

  SetMasterDllWakeUpTimer(host, socket, ch);

  return ;
} // ProgramTimings


void
RestoreTimingsChip (
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch
               )
/*++

  Restores other timing parameters

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param ch          - Channel number

  @retval N/A

--*/
{
  struct channelNvram                         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, (*channelNvList)[ch].tcothp);
  MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, (*channelNvList)[ch].tcothp2);
  MemWritePciCfgEp(host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, (*channelNvList)[ch].ddrtDimm0BasicTiming);
  MemWritePciCfgEp(host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG, (*channelNvList)[ch].ddrtDimm1BasicTiming);


} // RestoreTimingsChip

static UINT8
Check2xRefreshWA (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
/*++

  Checks to see if any workarounds require 2x refresh

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param ch          - Channel number

**/
{
   return 0;
}

void
SetMR0Shadow (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT32    MR0Data
  )
{
    //
    // Shadow MR0 but do not set DLL_RESET
    MemWritePciCfgEp (host, socket, ch, TCMR0SHADOW_MCDDC_CTL_REG, MR0Data & ~MR0_DLL);
    //
    // Save the value for S3 resume.
    //
    host->nvram.mem.socket[socket].channelList[ch].TCMr0Shadow = MR0Data & ~MR0_DLL;
}

void
SetMR2Shadow (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    dimm,
             UINT32   MR2Data
             )
/*++

  Programs the MR2 shadow register

  @param host    - Pointer to sysHost
  @param socket    - Socket number
  @param ch      - Channel number
  @param dimm    - DIMM number
  @param MR2Data - MR2 data

  @retval N/A

--*/
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  TCMR2SHADOW_MCDDC_CTL_STRUCT TCMr2Shadow;

  dimmNvList        = GetDimmNvList(host, socket, ch);
  TCMr2Shadow.Data  = MemReadPciCfgEp (host, socket, ch, TCMR2SHADOW_MCDDC_CTL_REG);

  //
  // If inversion required
  //
  if ((host->nvram.mem.dimmTypePresent != UDIMM) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
    TCMr2Shadow.Bits.two_mrs_en = 1;
  } else {
    TCMr2Shadow.Bits.two_mrs_en = 0;
  }

  //
  // Are the address and bank mappings mirrored?
  //
  if ((*dimmNvList)[dimm].SPDAddrMapp) {
    //
    // Set if not swizzled
    //
    TCMr2Shadow.Bits.addr_bit_swizzle |= (1 << dimm);
  } else {
    //
    // Clear if not swizzled
    //
    TCMr2Shadow.Bits.addr_bit_swizzle &= ~(1 << dimm);
  }
  //
  // Shadow MR2 bits 15:8 to bits 23:16
  //
  TCMr2Shadow.Bits.mr2_shdw_a15to8 = (MR2Data >> 8) & 0x0FF;

  //
  // Shadow MR2 bits 5:0 to bits 5:0
  //
  TCMr2Shadow.Bits.mr2_shdw_a5to0 = MR2Data & 0x3F;

  MemWritePciCfgEp (host, socket, ch, TCMR2SHADOW_MCDDC_CTL_REG, TCMr2Shadow.Data);
  //
  // Save it to be restored on S3 resume.
  //
  host->nvram.mem.socket[socket].channelList[ch].TCMr2Shadow = TCMr2Shadow.Data;

} // SetMR2Shadow


void
SetMR4Shadow (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT32    MR4Data
  )
{
  MemWritePciCfgEp (host, socket, ch, TCMR4SHADOW_MCDDC_CTL_REG, MR4Data);
  host->nvram.mem.socket[socket].channelList[ch].TCMr4Shadow = MR4Data;
}

void
SetMR5Shadow (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT32    MR5Data
  )
{
  MemWritePciCfgEp (host, socket, ch, TCMR5SHADOW_MCDDC_CTL_REG, MR5Data);
  host->nvram.mem.socket[socket].channelList[ch].TCMr5Shadow = MR5Data;
}

//@SEC_S override t_rwsr, t_rwdr, t_rwdd value to improve Rx margin
void STB_OverrideRDtoWRTiming(PSYSHOST host, UINT8 socket){
    UINT8   ch;
    UINT8   dimm;
    TCOTHP2_MCDDC_CTL_STRUCT tcothp2;
    struct  channelNvram (*channelNvList)[MAX_CH];
    struct  dimmNvram (*dimmNvList)[MAX_DIMM];
    UINT8   samsungDimmPresent = 0;
    UINT32  max_rwsr;

    channelNvList = &host->nvram.mem.socket[socket].channelList;

    //Check Samsung DIMM
    samsungDimmPresent = 0;
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          if ((*dimmNvList)[dimm].SPDDramMfgId == MFGID_SAMSUNG)
            samsungDimmPresent = 1;
            break;
        }
    }

    if((samsungDimmPresent == 0) || (host->setup.mem.tRWSREqualization == 0))
      return;

    max_rwsr = 0;

    //get maximun value
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      //read current value
      tcothp2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
      if (tcothp2.Bits.t_rwsr > max_rwsr)
        max_rwsr = tcothp2.Bits.t_rwsr;
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      //read current value
      tcothp2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
      tcothp2.Bits.t_rwsr = max_rwsr;
      //write new value
      (*channelNvList)[ch].tcothp2 = tcothp2.Data;
      MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, tcothp2.Data);
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      //read current value
      tcothp2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "T_RWSR = %d\n", tcothp2.Bits.t_rwsr));
    }
}
//_@SEC


UINT32
TurnAroundTimings (
                  PSYSHOST host,
                  UINT8    socket
                  )
/*++

  Programs turnaround times

  @param host  - Point to sysHost
  @param socket  - Socket numer

  @retval SUCCESS

--*/
{
  UINT8     ch;
  UINT8     dimm;
  UINT16    tempvalue;
  UINT32    status;
  TT_VARS   ttVars;

  struct channelNvram           (*channelNvList)[MAX_CH];
  struct dimmNvram              (*dimmNvList)[MAX_DIMM];
  TCOTHP2_MCDDC_CTL_STRUCT      tcothp2;
  TCRWP_MCDDC_CTL_STRUCT        tcrwp;
  TCOTHP_MCDDC_CTL_STRUCT       tcothp;
  TCLRDP_MCDDC_CTL_STRUCT       tclrdp;
#if !defined (HW_EMULATION) && defined (DDRT_TURNAROUND_EN)
  T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_STRUCT      ddrtGnt2gntCnstrnt;
  DDRT_TIMING_MCDDC_CTL_STRUCT              ddrtCtlTiming;
  T_DDRT_WR_CNSTRNT_MC_2LM_STRUCT           ddrtWrCnstrnt;
#endif
  UINT32                        specMin=0;

#ifdef LRDIMM_SUPPORT
  UINT32    tPDM_RD=0;
  UINT32    tPDM_WR=0;
  UINT32    tWRPRE=0;
  UINT32    tRPRE=0;
  UINT32    BL=0;
  UINT32    tCK=0;

  tCK = GettCK(host, socket);
  tPDM_RD = (167000000/tCK) +2500;   // unit is in tCKs
  tPDM_WR = (167000000/tCK) +2500;   // unit is in tCKs
  if (host->setup.mem.writePreamble >0) tWRPRE = 10000; // if enabled +10000
  if (host->setup.mem.readPreamble >0) tRPRE = 10000; // if enabled +10000
  BL = (((host->setup.mem.optionsExt & WR_CRC) / WR_CRC) * 20000) + 80000; // burst length is 8 or 10 (x10)
  specMin = (tPDM_RD +tPDM_WR +tWRPRE +tRPRE/2 +BL/2 + 27000 - 60000)/10000;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "(x10000) tck = %d, tPDM_RD=%d, tPDM_WR=%d, tWRPRE=%d, tRPRE=%d, BL=%d, specMin=%d\n",tCK,tPDM_RD,tPDM_WR,tWRPRE,tRPRE,BL,specMin));

#endif

  channelNvList = GetChannelNvList(host, socket);
  status = SUCCESS;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    // HSD 5331581 - skip for 2DPC LRDIMMs until fix can be root caused
    //if (IsLrdimmPresent(host, socket, ch, 0) && (*channelNvList)[ch].maxDimm == 2) continue;
    // HSD 5331581

    //
    // The following code is used to optimize turnaround times based on training results
    //
    tcrwp.Data = MemReadPciCfgEp (host, socket, ch, TCRWP_MCDDC_CTL_REG);
    tcothp.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);

    tcothp2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
    tclrdp.Data  = MemReadPciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG);

#if !defined (HW_EMULATION) && defined (DDRT_TURNAROUND_EN)
    ddrtGnt2gntCnstrnt.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG);
    ddrtCtlTiming.Data = MemReadPciCfgEp (host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG);
    ddrtWrCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG);
#endif

    if (((*channelNvList)[ch].maxDimm > 2) && ((*channelNvList)[ch].lrDimmPresent) && ((*channelNvList)[ch].encodedCSMode == 2)) {
      tcrwp.Bits.t_ccd += LR_RANK_MULT_TA;
      (*channelNvList)[ch].common.tCCD = tcrwp.Bits.t_ccd & 0xff;
    }

    //
    // Get turnaround time variables
    //
    GetTTVar(host, socket, ch, &ttVars);

    //
    // BIOS HSD 4166843 - Mixing x4 and x8 might cut off last strobe so skip optimizing if mixed
    //
    tcrwp.Bits.t_rrdr = CalctRRDR(host, socket, ch, &ttVars);
    tcrwp.Bits.t_rrdd = CalctRRDD(host, socket, ch, &ttVars);

    //
    // BIOS HSD 4928496
    //
#if !defined (HW_EMULATION) && defined (DDRT_TURNAROUND_EN)
    if ((*channelNvList)[ch].ddrtEnabled) {
      ddrtGnt2gntCnstrnt.Bits.t_ddrt_gntgnt_dd = CalctDdr4toDdrtRRDD (host, socket, ch, &ttVars);
      ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd = CalctDdrttoDdr4RRDD (host, socket, ch, &ttVars);
#if !defined (HW_EMULATION) && !defined (DDRT_TURNAROUND_EN)
      ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd = CalctDdr4toDdrtWWDD (host, socket, ch, &ttVars);
      ddrtCtlTiming.Bits.ddrtwr_to_ddr4wr = CalctDdrttoDdr4WWDD (host, socket, ch, &ttVars);
#endif
    }
#endif

    tcrwp.Bits.t_wwdr = CalctWWDR(host, socket, ch, MIN_WWDR, &ttVars);
    tcrwp.Bits.t_wwdd = CalctWWDD(host, socket, ch, MIN_WWDD, &ttVars);
    tcothp2.Bits.t_rwsr = CalctRWSR(host, socket, ch, specMin, &ttVars);
    tcothp2.Bits.t_rwdr = CalctRWDR(host, socket, ch, specMin, &ttVars);
    if ((((*channelNvList)[ch].encodedCSMode) || ((*channelNvList)[ch].lrRankMultEnabled)) &&
        (tcothp2.Bits.t_rwsr < tcothp2.Bits.t_rwdr)) {
      tcothp2.Bits.t_rwsr = tcothp2.Bits.t_rwdr;
    }

    tcothp2.Bits.t_rwdd = CalctRWDD(host, socket, ch, &ttVars);
    tcrwp.Bits.t_wrdr = CalctWRDR(host, socket, ch, &ttVars);
    tcothp2.Bits.t_wrdd = CalctWRDD(host, socket, ch, &ttVars);

    tclrdp.Bits.t_rrds = tcrwp.Bits.t_rrdr;
    tclrdp.Bits.t_wwds = tcrwp.Bits.t_wwdr;
    tclrdp.Bits.t_rwds = tcothp2.Bits.t_rwdr;
    tclrdp.Bits.t_wrds = tcrwp.Bits.t_wrdr;
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      // t_rwds = CL - CWL + RBL/2 + 1tCK + tWPRE -5(built in clocks)
      tempvalue = (*channelNvList)[ch].common.nCL - (*channelNvList)[ch].common.nWL + 4 + 1 - 5;
      if (tempvalue > tcothp2.Bits.t_rwdr) {
        if (tempvalue < 0x1f){
          tclrdp.Bits.t_rwds = tempvalue;
        } else {
          tclrdp.Bits.t_rwds = 0x1f;
        }
      }

      // t_wrds = CWL + WBL/2 + tWTR_L -5(built in clocks)
      tempvalue = ((*channelNvList)[ch].common.nWL + 4 + (*channelNvList)[ch].common.nWTR_L - 5);
      if (tempvalue > tcrwp.Bits.t_wrdr) {
        if (tempvalue < 0x1f) {
          tclrdp.Bits.t_wrds = tempvalue;
        } else {
          tclrdp.Bits.t_wrds = 0x1f;
        }
      }

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "t_rwds = %d, t_wrds = %d\n", tclrdp.Bits.t_rwds, tclrdp.Bits.t_wrds));
    }
    tclrdp.Bits.t_ccd_wr = tcrwp.Bits.t_ccd;

    // Montage WA
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE)&& ((*dimmNvList)[dimm].lrbufGen == 0x01) && ((*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_GEN2_A0) && ((*channelNvList)[ch].lrDimmPresent)) {
        if (tclrdp.Bits.t_ccd_wr < 2) {
          tclrdp.Bits.t_ccd_wr = 2;
        }
      } // if WA dimm type
    } // dimm loop


    if (host->setup.mem.optionsExt & WR_CRC) {
      tclrdp.Bits.t_ccd_wr = tclrdp.Bits.t_ccd_wr + 2;
    }
    (*channelNvList)[ch].common.tCCD_WR   = tclrdp.Bits.t_ccd_wr & 0xff;


    (*channelNvList)[ch].tcrwp = tcrwp.Data;
    MemWritePciCfgEp (host, socket, ch, TCRWP_MCDDC_CTL_REG, tcrwp.Data);

    (*channelNvList)[ch].tcothp = tcothp.Data;
    MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, tcothp.Data);
    (*channelNvList)[ch].tcothp2 = tcothp2.Data;
    MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, tcothp2.Data);

    (*channelNvList)[ch].tclrdp = tclrdp.Data;
    MemWritePciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG, tclrdp.Data);

    //Write values back to register
#if !defined (HW_EMULATION) && defined (DDRT_TURNAROUND_EN)
    MemWritePciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG, ddrtWrCnstrnt.Data);
    MemWritePciCfgEp (host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG, ddrtCtlTiming.Data);
    MemWritePciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG, ddrtGnt2gntCnstrnt.Data);
#endif

  } // ch loop

  STB_OverrideRDtoWRTiming(host, socket); //@SEC override t_rwsr, t_rwdr, t_rwdd value to improve Rx margin

  return status;
} // TurnAroundTimings

UINT8
CalctRRDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the read to read turnaround time for different ranks on the same DIMM

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRRDR channel value

  --*/
{
  UINT8 odtStretch;
  UINT8 tRRDR = 0;
  struct ddrChannel                 (*channelList)[MAX_CH];

  channelList = &host->var.mem.socket[socket].channelList;
  tRRDR = (*ttVars).cas2RecEnDR;

  //
  // t_RRDR = 1(RTT Change Skew) + max(ODT Gap due to CK PI Difference (for UDIMM), ODT Stretch, 1 (ODT Gap due to CAS2RCVEN))
  //

  if (((*ttVars).odtRdTrailing > 1) || ((*ttVars).odtRdLeading > 0)) {
    odtStretch = (*ttVars).odtRdTrailing - 1 + (*ttVars).odtRdLeading;
  } else {
    odtStretch = 0;
  }

  //
  // RTT Change Skew
  //
  tRRDR += 1;
  tRRDR += (*ttVars).clkDiffDR;
  tRRDR += odtStretch;

  //
  // If LRDIMM then min setting is 2
  //
  if ((host->nvram.mem.socket[socket].lrDimmPresent) && (tRRDR < 2)) {
    if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_ENABLED) {
      tRRDR = 1;
    } else {
      tRRDR = 2;
    }
  }
  //
  // floor trrdr to 2
  //
  if (tRRDR < 2) {
    tRRDR = 2;
  }
  //
  // Make sure we don't overflow
  //
  if (tRRDR > 7) tRRDR = 7;

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tRRDR = %d\n", tRRDR));

  return tRRDR;
} // CalctRRDR

UINT8
CalctRRDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
  )
/*++

  This function calculates the read to read turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRRDD channel value

  --*/
{
  UINT8             tRRDD;
  UINT8             odtStretch;

  //
  // t_RRDR = 1(RTT Change Skew) + max(ODT Gap due to CK PI Difference (for UDIMM), ODT Stretch, 1 (ODT Gap due to CAS2RCVEN))
  //

  if ((ttVars->odtRdTrailing > 1) || (ttVars->odtRdLeading > 0)) {
    odtStretch = (*ttVars).odtRdTrailing - 1 + (*ttVars).odtRdLeading;
  } else {
    odtStretch = 0;
  }
  //
  // RTT Change Skew
  //
  tRRDD = 1;

  tRRDD += ttVars->cas2RecEnDD;
  tRRDD += ttVars->clkDiffDD;
  tRRDD += odtStretch;

  //floor trrdd to 2
  if (tRRDD < 2) tRRDD = 2;
  //
  // Make sure we don't overflow
  //
  if (tRRDD > 7) tRRDD = 7;

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tRRDD = %d\n", tRRDD));
  return tRRDD;
} // CalctRRDD

UINT16
CalctDdr4toDdrtRRDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
  )
/*++
  DDR4-TO-DDRT
  This function calculates the read to read turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval t_DDRT_GNTGNT_DD channel value

  --*/
{
  UINT8     tRRDD;
  UINT16    t_DDRT_GNTGNT_DD;
  UINT8     GNT2Cmpl;
  UINT8     CAS2Cmpl_Late;
  INT16     MCDelay;
  UINT8     dimm;
  UINT8     rank;
  UINT8     max_roundTripLatency[MAX_CH] = {0};
  UINT8     cur_roundTripLatency[MAX_CH] = {0};
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];

  //
  // t_DDRT_GNTGNT_DD =max((DDR4 calculation for t_rrdd with DDR-T Roundtrip accounted for) + 5), (MC Internal Delay)) - 2
  // MC Internal Datapath constraint = max(CAS2Cmpl_Late – GNT2Cmpl + 1, 1)
  // CAS2Cmpl_Late = ceiling(Roundtrip(DDR4)/2 – 13 + 8 + 1), GNT2Cmpl = GNT2ERID + 4 + 1 + 1
  //

  //get maximum roundtrip latency
  dimmNvList    = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;
      cur_roundTripLatency[ch] = GetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank));
      if(max_roundTripLatency[ch] < cur_roundTripLatency[ch]) max_roundTripLatency[ch] = cur_roundTripLatency[ch];
    }
    if ((*dimmNvList)[dimm].aepDimmPresent == 1) {  ttVars->Gnt2Erid = (UINT8)(*dimmNvList)[dimm].gnt2erid;}
  }

  tRRDD = CalctRRDD(host, socket, ch, ttVars);
  //(4) is the number of DCLK’s to accumulate the ERID values
  //(1) additional DCLK to look up DDRTQ
  //(1) DCLK to flop completion
  GNT2Cmpl = ttVars->Gnt2Erid + 4 + 1 + 1;

  //find ceiling value
  //(13) cycles is how many internal cycles prior to Roundtrip Latency that RdValid is asserted
  //(8) is number of cycles delay to shift through programmable Rd Delay in RRD fub
  //(1) more cycle to flop from DP to 2LMCntl
  if ( max_roundTripLatency[ch] % 2 == 0 ) CAS2Cmpl_Late = max_roundTripLatency[ch]/2 -13 + 8 + 1;
  else CAS2Cmpl_Late = max_roundTripLatency[ch]/2 -13 + 8 + 1 + 1; //add 1 for odd
  MCDelay = CAS2Cmpl_Late - GNT2Cmpl + 1;

  //find maximum value for max(CAS2Cmpl_Late – GNT2Cmpl + 1, 1)
  //Value of 5 translates the DDR4 turn-around equation to CMD to CMD spacing.
  if(MCDelay < 1) MCDelay = 1;
  t_DDRT_GNTGNT_DD = tRRDD + 5 ;

  //find maximum value for max((DDR4 calculation for t_rrdd with DDR-T Roundtrip accounted for) + 5), (MC Internal Delay))
  if(t_DDRT_GNTGNT_DD < MCDelay) t_DDRT_GNTGNT_DD = MCDelay;

  t_DDRT_GNTGNT_DD -= 2;  //Subtract 2 for default command spacing
  if (IsLrdimmPresent(host, socket, ch, 0) && (t_DDRT_GNTGNT_DD < 9) && (host->nvram.mem.socket[socket].channelList[ch].maxDimm == 2)) { t_DDRT_GNTGNT_DD = 9; }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "t_DDRT_GNTGNT_DD = %d\n", t_DDRT_GNTGNT_DD));
  return t_DDRT_GNTGNT_DD;
} // CalctDdr4toDdrtRRDD

UINT8
CalctDdrttoDdr4RRDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
  )
/*++
  DDRT-TO-DDR4
  This function calculates the read to read turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval DDRTRD_to_DDR4RD channel value

  --*/
{
  UINT8     tRRDD;
  UINT8     DDRTRD_to_DDR4RD;
  UINT8     GNT2Cmpl;
  UINT8     CAS2Cmpl_Early;
  INT8     MCDelay;
  UINT8     dimm;
  UINT8     rank;
  UINT8     max_roundTripLatency[MAX_CH] = {0};
  UINT8     cur_roundTripLatency[MAX_CH] = {0};
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];

  //
  // DDRTRD_to_DDR4RD =max((DDR4 calculation for t_rrdd with DDR-T Roundtrip accounted for) + 5), (MC Internal Delay)) - 2
  // MC Internal Datapath constraint = max(GNT2Cmpl - CAS2Cmpl_Early + 1, 1)
  // CAS2Cmpl_Late = ceiling(Roundtrip(DDR4)/2 – 13 + 8 + 1), GNT2Cmpl = GNT2ERID + 4 + 1 + 1
  //

  //get maximum roundtrip latency
  dimmNvList    = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;
      cur_roundTripLatency[ch] = GetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank));
      if(max_roundTripLatency[ch] < cur_roundTripLatency[ch]) max_roundTripLatency[ch] = cur_roundTripLatency[ch];
    }
    if ((*dimmNvList)[dimm].aepDimmPresent == 1) {  ttVars->Gnt2Erid = (UINT8)(*dimmNvList)[dimm].gnt2erid;}
  }

  tRRDD = CalctRRDD(host, socket, ch, ttVars);
  //(4) is the number of DCLK’s to accumulate the ERID values
  //(1) additional DCLK to look up DDRTQ
  //(1) DCLK to flop completion
  GNT2Cmpl = ttVars->Gnt2Erid + 4 + 1 + 1;

  //find ceiling value
  //(13) cycles is number of internal cycles prior to Roundtrip Latency that RdValid is asserted
  //(1) is number of cycles delay to shift through programmable Rd Delay in RRD fub
  //(1) cycle to flop from DP to 2LMCntl
  if ( max_roundTripLatency[ch] % 2== 0 ) CAS2Cmpl_Early = max_roundTripLatency[ch]/2 -13 + 1;
  else CAS2Cmpl_Early = max_roundTripLatency[ch]/2 -13 + 1 + 1; //add 1 if odd
  MCDelay = GNT2Cmpl - CAS2Cmpl_Early + 1;

  //find maximum value for max(CAS2Cmpl_Late – GNT2Cmpl + 1, 1)
  //Value of 5 translates the DDR4 turn-around equation to CMD to CMD spacing.
  if(MCDelay < 1) MCDelay = 1;
  DDRTRD_to_DDR4RD = tRRDD + 5 ;

 //find maximum value for max((DDR4 calculation for t_rrdd with DDR-T Roundtrip accounted for) + 5), (MC Internal Delay))
  if(DDRTRD_to_DDR4RD < MCDelay) DDRTRD_to_DDR4RD = MCDelay;

 //Adding 4 for default command spacing
  DDRTRD_to_DDR4RD += 4;
  if (IsLrdimmPresent(host, socket, ch, 0) && (DDRTRD_to_DDR4RD < 8) && (host->nvram.mem.socket[socket].channelList[ch].maxDimm == 2)) { DDRTRD_to_DDR4RD = 8; }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDRTRD_to_DDR4RD = %d\n", DDRTRD_to_DDR4RD));
  return DDRTRD_to_DDR4RD;
} // CalctDdrttoDdr4RRDD

UINT8
CalctDdr4toDdrtWWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
  )
/*++
  DDR4-TO-DDRT
  This function calculates the write to write turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval t_DDRT_WRWR_DD channel value

  --*/
{
  UINT8     t_DDRT_WRWR_DD;
  UINT8     odtStretch;
  //
  //t_DDRT_WRWR_DD = 1(RTT Change Skew) + ODT Gap due to CK PI Difference + ODT Stretch + CAS2DRVEN differences + (5 – 3)
  //
  if ((ttVars->odtWrTrailing > 1) || (ttVars->odtWrLeading > 0)) {
    odtStretch = (*ttVars).odtWrTrailing - 1 + (*ttVars).odtWrLeading;
  } else {
    odtStretch = 0;
  }
  //
  // RTT Change Skew
  //
  t_DDRT_WRWR_DD = 1;

  t_DDRT_WRWR_DD += ttVars->cas2DrvEnDD;
  t_DDRT_WRWR_DD += ttVars->clkDiffDD;
  t_DDRT_WRWR_DD += odtStretch;
  t_DDRT_WRWR_DD += 5-3; // 5-3 term is difference in additional cmd spacing above register field setting (DDR4 – DDR-T)

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "t_DDRT_WRWR_DD = %d\n", t_DDRT_WRWR_DD));
  return t_DDRT_WRWR_DD;
} // CalctDdr4toDdrtWWDD

UINT8
CalctDdrttoDdr4WWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
  )
/*++
  DDR4-TO-DDRT
  This function calculates the write to write turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval DDRTWR_to_DDR4WR channel value

  --*/
{
  UINT8     DDRTWR_to_DDR4WR;
  UINT8     odtStretch;
  //
  //DDRTWR_to_DDR4WR = 1(RTT Change Skew) + ODT Gap due to CK PI Difference + ODT Stretch + CAS2DRVEN differences + (5 + 4)
  //
  if ((ttVars->odtWrTrailing > 1) || (ttVars->odtWrLeading > 0)) {
    odtStretch = (*ttVars).odtWrTrailing - 1 + (*ttVars).odtWrLeading;
  } else {
    odtStretch = 0;
  }
  //
  // RTT Change Skew
  //
  DDRTWR_to_DDR4WR = 1;

  DDRTWR_to_DDR4WR += ttVars->cas2DrvEnDD;
  DDRTWR_to_DDR4WR += ttVars->clkDiffDD;
  DDRTWR_to_DDR4WR += odtStretch;
  DDRTWR_to_DDR4WR += 5 + 4; // 5 + 4 term is difference in additional cmd spacing above register field setting (DDR4 – DDR-T)

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDRTWR_to_DDR4WR = %d\n", DDRTWR_to_DDR4WR));
  return DDRTWR_to_DDR4WR;
} // CalctDdrttoDdr4WWDD

void
DramSpecCalctWWDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tWWDR
  )
{
  UINT8     odtStretch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel   (*channelList)[MAX_CH];

  channelList = &host->var.mem.socket[socket].channelList;
  channelNvList = GetChannelNvList(host, socket);

  //
  // Rtt change skew
  //
  *tWWDR = 1;

  if ((*channelNvList)[ch].maxDimm > 1) {
    if (((*ttVars).odtWrTrailing > 0) || ((*ttVars).odtWrLeading > 1)) {
      odtStretch = (*ttVars).odtWrTrailing + (*ttVars).odtWrLeading - 1;
    } else {
      odtStretch = 0;
    }
  } else {
    odtStretch = 0;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "odtStretch = %d\n", odtStretch));

  *tWWDR += (*ttVars).cas2DrvEnDR;
  *tWWDR += (*ttVars).clkDiffDR;
  *tWWDR += odtStretch;

  if ((host->nvram.mem.socket[socket].lrDimmPresent) && (*tWWDR < 2)) {
    if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_ENABLED) {
      *tWWDR = 1;
    } else {
      *tWWDR = 2;
    }
  }
  *tWWDR += 1;
} // DramSpecCalctWWDR

void
DramSpecCalctWWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tWWDD
  )
{
  UINT8     odtStretch;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  //
  // Get ODT stretch
  //
  if ((*channelNvList)[ch].maxDimm > 1) {
    if (((*ttVars).odtWrTrailing > 0) || ((*ttVars).odtWrLeading > 1)) {
      odtStretch = (*ttVars).odtWrTrailing + (*ttVars).odtWrLeading - 1;
    } else {
      odtStretch = 0;
    }
  } else {
    odtStretch = 0;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "odtStretch = %d\n", odtStretch));

  //
  // Rtt change skew
  //
  *tWWDD += 1;
  *tWWDD += (*ttVars).clkDiffDD;
  *tWWDD += odtStretch;
  *tWWDD += 1;

} // DramSpecCalctWWDD

void
DramSpecCalctRWSR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tRWSR
  )
{
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT      dataControl0;

  struct channelNvram                   (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  //
  // RRT Change Skew
  //
  *tRWSR += 1;

  if (host->nvram.mem.socket[socket].lrDimmPresent) {
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    if (dataControl0.Bits.longpreambleenable && (host->nvram.mem.socket[socket].ddrFreq < 2133) && (*tRWSR < 5)) {
      *tRWSR = 5;
    } else if (dataControl0.Bits.longpreambleenable && (host->nvram.mem.socket[socket].ddrFreq >= 2133) && (*tRWSR < 6)) {
      *tRWSR = 6;
    }
  }

} // DramSpecCalctRWSR

void
DramSpecCalctRWDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tRWDR
  )
{
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct ddrChannel                     (*channelList)[MAX_CH];

  channelList = &host->var.mem.socket[socket].channelList;
  channelNvList = GetChannelNvList(host, socket);

    //
    // RRT Change Skew
    //
    *tRWDR += 1;

    //
    // Get clk diff
    //
    if ((*ttVars).clkDiffDR) {
      *tRWDR += 1;
    }

    //
    // Add an extra clock for now
    //
    *tRWDR += 1;

    if (((*channelNvList)[ch].lrDimmPresent) && (*tRWDR < 7)){
      if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_ENABLED) {
        *tRWDR = 6;
      } else {
        *tRWDR = 7;
      }
    }

} // DramSpecCalctRWDR

void
DramSpecCalctRWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tRWDD
         )
{
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT      dataControl0;

  struct channelNvram                   (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  //
  // RRT Change Skew
  //
  *tRWDD += 1;

  if (((*ttVars).odtRdTrailing > 1) || ((*ttVars).odtWrLeading > 1)) {
    *tRWDD += (*ttVars).odtRdTrailing - 1 + (*ttVars).odtWrLeading - 1;
  }

  //
  // Get clk diff
  //
  if ((*ttVars).clkDiffDD) {
    *tRWDD += 1;
  }

  if (host->nvram.mem.socket[socket].lrDimmPresent) {
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    if (dataControl0.Bits.longpreambleenable && (host->nvram.mem.socket[socket].ddrFreq < 2133) && (*tRWDD < 5)) {
      *tRWDD = 5;
    } else if (dataControl0.Bits.longpreambleenable && (host->nvram.mem.socket[socket].ddrFreq >= 2133) && (*tRWDD < 6)) {
      *tRWDD = 6;
    }
  }

  //
  // Add an extra clock for now
  //
  *tRWDD += 1;

} // DramSpecCalctRWDD

void
DramSpecCalctWRDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tWRDR
         )
{
  UINT8     odtStretch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel   (*channelList)[MAX_CH];

  channelList = &host->var.mem.socket[socket].channelList;
  channelNvList = GetChannelNvList(host, socket);

  //
  // Rtt change skew
  //
  *tWRDR = 1;

  if ((*channelNvList)[ch].maxDimm > 1) {
    if (((*ttVars).odtWrTrailing > 0) || ((*ttVars).odtRdLeading > 0)) {
      odtStretch = (*ttVars).odtWrTrailing + (*ttVars).odtRdLeading;
    } else {
      odtStretch = 0;
    }
  } else {
    odtStretch = 0;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "odtStretch = %d\n", odtStretch));

  *tWRDR += (*ttVars).clkDiffDR;
  *tWRDR += odtStretch;

  if ((host->nvram.mem.socket[socket].lrDimmPresent) && (*tWRDR < 2)) {
    if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_ENABLED) {
      *tWRDR = 1;
    } else {
      *tWRDR = 2;
    }
  }
} // DramSpecCalctWRDR

void
DramSpecCalctWRDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars,
         UINT8     *tWRDD
  )
/*++

  This function calculates the write to read turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval WR to RD different dimm

  --*/
{
  UINT8 odtStretch;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  //
  // Rtt change skew
  //
  *tWRDD = 1;

  if ((*channelNvList)[ch].maxDimm > 1) {
    if (((*ttVars).odtWrTrailing > 0) || ((*ttVars).odtRdLeading > 0)) {
      odtStretch = (*ttVars).odtWrTrailing + (*ttVars).odtRdLeading;
    } else {
      odtStretch = 0;
    }
  } else {
    odtStretch = 0;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "odtStretch = %d\n", odtStretch));

  *tWRDD += (*ttVars).clkDiffDD;
  *tWRDD += odtStretch;

} // DramSpecCalctWRDD


void
EffectiveBusDelayInit(
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 INT16     *CwlAdj,
                 UINT32                              *IOComp,
                 UINT16                              *IntDelta,
                 UINT32                              *longPreambleEnable
                 )
{
  UINT8                   dimm;
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  TCOTHP_MCDDC_CTL_STRUCT  tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT tCOTHP2;
  IOLATENCY1_MCDDC_DP_STRUCT          ioLatency1;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT  dataControl0;


  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  tCOTHP.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
  tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);

  *CwlAdj = (INT16)tCOTHP.Bits.t_cwl_adj - (INT16)tCOTHP2.Bits.t_cwl_adj_neg;
  *CwlAdj = *CwlAdj * (2 * PiTicks);


  dimm = 0;
  dataControl0.Data = (*channelNvList)[ch].dataControl0;
  ioLatency1.Data =((*channelNvList)[ch].ioLatency1);
  *longPreambleEnable = dataControl0.Bits.longpreambleenable;

  *IOComp      = (ioLatency1.Bits.io_lat_io_comp) * PiTicks;

  //SKX change to 8UI
  //IntDelta    = (4 * PiTicks) + 32;        //equates to 4 UI
  *IntDelta    = (8 * PiTicks);        //equates to 8 UI
} // EffectiveBusDelayInit


#ifdef DEBUG_TURNAROUNDS

const INT8  taNameTable  [TA_POR_TYPES][6] = {
  { "nRRDR"},
  { "nRRDD"},
  { "nWWDR"},
  { "nWWDD"},
  { "nWRDR"},
  { "nWRDD"},
  { "nRWSR"},
  { "nRWDR"},
  { "nRWDD"},
  { "nCCD "},
  { "nEODT"}
};

/**

  Compares turnaround times with HAS table of expected values
  reports via scratchpad if worse than expected

  @param host  - Point to sysHost
  @param socket  - Socket numer

  @retval SUCCESS

**/
UINT32
CheckTurnAroundTimings(
                      PSYSHOST host,
                      UINT8    socket
                      )
{
  UINT32  status;
  UINT8   ch;
  UINT8   dimm;
#ifdef   SERIAL_DBG_MSG
  UINT8   rank;
#endif  // SERIAL_DBG_MSG
  UINT8   worse;
  UINT8   i;
  UINT8   taTable[TA_POR_TYPES];
  UINT8   tableUpdate;
  UINT8   allSingle;
  TCRWP_MCDDC_CTL_STRUCT            tcrwp;
  TCOTHP2_MCDDC_CTL_STRUCT          tcothp2;
  BIOSSCRATCHPAD1_UBOX_MISC_STRUCT  biosScratchPad1;

  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
#ifdef   SERIAL_DBG_MSG
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct RankCh       (*rankPerCh)[MAX_RANK_CH];
#endif  // SERIAL_DBG_MSG
  UINT8               taPORTableMin[TA_POR_TYPES] = {
                      // RDIMM/UDIMM
                      /* rrdr */  1,
                      /* rrdd */  1,
                      /* wwdr */  1,
                      /* wwdd */  1,
                      /* rwsr */  1,
                      /* wrdr */  1,
                      /* wrdd */  1,
                      /* rwdr */  1,
                      /* rwdd */  1,
                      /* ccd  */  0,
                      /* eodt */  0
                      };
  UINT8               taPORTableMax[TA_POR_TYPES] = {
                      // RDIMM/UDIMM
                      /* rrdr */  4,
                      /* rrdd */  5,
                      /* wwdr */  4,
                      /* wwdd */  5,
                      /* wrdr */  3,
                      /* wrdd */  3,
                      /* rwsr */  7,
                      /* rwdr */  8,
                      /* rwdd */  9,
                      /* ccd  */  0,
                      /* eodt */  0
                      };

  channelNvList = GetChannelNvList(host, socket);

  status = SUCCESS;
  worse = 0;        // Set worse indicator to "worse timings not detected yet"

  //
  // Until the end of the power on we put the max value
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nCompare Turnaround timings against POR timings\n"));

  tableUpdate = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    allSingle = 1;
    dimmNvList    = GetDimmNvList(host, socket, ch);

    //
    // Timing Constraints DDR3 Read Write Parameters
    //

    tcrwp.Data = (*channelNvList)[ch].tcrwp;
    tcothp2.Data = (*channelNvList)[ch].tcothp2;


    taPORTableMin[TA_POR_T_CCD] = 0;
    taPORTableMax[TA_POR_T_CCD] = 0;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].numRanks >= 2) {
        allSingle = 0;
        break;
      }
    }

    if ((allSingle == 1) && (tableUpdate == 0)) {
      taPORTableMax[TA_POR_T_WWDD] = taPORTableMax[TA_POR_T_WWDD] - 1;
      tableUpdate = 1;
    }

    // Init actual timings table
    taTable[TA_POR_T_CCD]  = (UINT8) tcrwp.Bits.t_ccd;
    taTable[TA_POR_T_RWSR] = (UINT8) tcothp2.Bits.t_rwsr;
    taTable[TA_POR_T_WRDD] = (UINT8) tcothp2.Bits.t_wrdd;
    taTable[TA_POR_T_WRDR] = (UINT8) tcrwp.Bits.t_wrdr;
    taTable[TA_POR_T_RWDD] = (UINT8) tcothp2.Bits.t_rwdd;
    taTable[TA_POR_T_RWDR] = (UINT8) tcothp2.Bits.t_rwdr;
    taTable[TA_POR_T_WWDD] = (UINT8) tcrwp.Bits.t_wwdd;
    taTable[TA_POR_T_WWDR] = (UINT8) tcrwp.Bits.t_wwdr;
    taTable[TA_POR_T_RRDD] = (UINT8) tcrwp.Bits.t_rrdd;
    taTable[TA_POR_T_RRDR] = (UINT8) tcrwp.Bits.t_rrdr;
    taTable[TA_POR_T_EODT] = (UINT8) tcrwp.Bits.t_rwsr;
#ifdef   SERIAL_DBG_MSG
    // Print all this information out
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl(host);
      // Now compare tables and print/post appropriate messages
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "TurnA - Min - Max - Actual\n"));
      for (i = 0; i < TA_POR_TYPES; i++) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "%s - %2d  - %2d  - %2d", taNameTable[i], taPORTableMin[i], taPORTableMax[i], taTable[i]));
        if (taTable[i] > taPORTableMax[i]) {
          rcPrintf ((host, " - Too High"));
        } else if (taTable[i] < taPORTableMin[i]) {
          rcPrintf ((host, " - Too Low"));
        }// >
        rcPrintf ((host, "\n"));
      } // for i
      rcPrintf ((host, "\n"));
      releasePrintFControl(host);
    } // if checkMsgLevel
#endif //  SERIAL_DBG_MSG

    for (i = 0; i < TA_POR_TYPES; i++) {
      if (taPORTableMax[i] != taTable[i]) {
        status = FAILURE;
        if (taTable[i] > taPORTableMax[i]) {
          worse = 1;
        } // >
      } // !=
    } // for i
  } // ch loop

  // Read/Modify/Write ScratchPad1 Bits 2/3
  biosScratchPad1.Data = ReadCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG);
  biosScratchPad1.Data = biosScratchPad1.Data & ~(BIT3+BIT2);

  if (status == FAILURE) {
    biosScratchPad1.Data = biosScratchPad1.Data | BIT2;
  }
  if (worse) {
    biosScratchPad1.Data = biosScratchPad1.Data | BIT3;
  }
  WriteCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG, biosScratchPad1.Data);

#ifdef   SERIAL_DBG_MSG
  getPrintFControl(host);
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "   RoundTrip timings summary\n"));
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "      R0 - R1 - R2 - R3 \n"));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    rankPerCh = &(*channelNvList)[ch].rankPerCh;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                    ""));

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (rank != 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        " - "));
        }
        if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "NA"));
        } else {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "%2d", (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
        }
      } //rank
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n"));
    } //dimm
  } //ch
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));
  releasePrintFControl(host);
#endif //  SERIAL_DBG_MSG

  return status;
}

#endif  // DEBUG_TURNAROUNDS

void
SaveRefreshRate(
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8 refreshRate
               )
{
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  //
  // Save Refresh Rate for later use
  //
  host->nvram.mem.socket[socket].refreshRate = refreshRate;
  (*channelNvList)[ch].twoXRefreshSetPerCh = refreshRate;
}


void
InitTurnAroundTimes(
    PSYSHOST                   host,
    UINT8                      socket,
    UINT8                      ch,
    UINT8                      dimm,
    TCRWP_MCDDC_CTL_STRUCT     *tcrwp,
    TCOTHP2_MCDDC_CTL_STRUCT   *tcothp2,
    TCLRDP_MCDDC_CTL_STRUCT    *tclrdp
                   )
{
  TCLRDP1_MCDDC_CTL_STRUCT                    tclrdp1;
  struct channelNvram                         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tcrwp->Bits.t_ccd = 0;

#ifdef LRDIMM_SUPPORT
  if ((*channelNvList)[ch].lrDimmPresent) {
      tcrwp->Bits.t_rrdr = 4;
      tcrwp->Bits.t_rrdd = 4;
      tcrwp->Bits.t_wwdr = 5;
      tcrwp->Bits.t_wwdd = 5;
      tcrwp->Bits.t_wrdr = 4;
      tcrwp->Bits.t_rwsr = 0;
      // this field is used for ODT Delay
      // tcrwp.Bits.t_rwdr = 0;
      tcothp2->Bits.t_rwdd = 15;
      tcothp2->Bits.t_rwdr = 15;
      tcothp2->Bits.t_rwsr = 15;
      tcothp2->Bits.t_wrdd = 4;
  } else
#endif
  {
      tcrwp->Bits.t_rrdr = 4;
      tcrwp->Bits.t_rrdd = 4;
      tcrwp->Bits.t_wwdr = 5;
      tcrwp->Bits.t_wwdd = 5;
      tcrwp->Bits.t_wrdr = 4;
      tcrwp->Bits.t_rwsr = 0;
      // this field is used for ODT Delay
      // tcrwp.Bits.t_rwdr = 0;
      tcothp2->Bits.t_rwdd = 15;
      tcothp2->Bits.t_rwdr = 15;
      tcothp2->Bits.t_rwsr = 15;
      tcothp2->Bits.t_wrdd = 4;
  }

  tclrdp->Bits.t_ccd_wr = tcrwp->Bits.t_ccd;
  tclrdp->Bits.t_rrds = tcrwp->Bits.t_rrdr;
  tclrdp->Bits.t_wwds = tcrwp->Bits.t_wwdr;
  tclrdp->Bits.t_rwds = tcothp2->Bits.t_rwdr;
  tclrdp->Bits.t_wrds = tcrwp->Bits.t_wrdr;
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    // t_rwds = CL - CWL + RBL/2 + 1tCK + tWPRE
    tclrdp->Bits.t_rwds = 0x1f;
    // setting to max for initial settings

    // t_wrds = CWL + WBL/2 + tWTR_L
    // setting to max for initial settings
    tclrdp->Bits.t_wrds = 0x1f;

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "t_rwds = %d, t_wrds = %d\n", tclrdp->Bits.t_rwds, tclrdp->Bits.t_wrds));
  }
  MemWritePciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG, tclrdp->Data);

  tclrdp1.Data = MemReadPciCfgEp (host, socket, ch, TCLRDP1_MCDDC_CTL_REG);
  tclrdp1.Bits.t_rr_l_ds = tcrwp->Bits.t_rrdr;
  tclrdp1.Bits.t_ww_l_ds = tcrwp->Bits.t_wwdr;
  MemWritePciCfgEp (host, socket, ch, TCLRDP1_MCDDC_CTL_REG, tclrdp1.Data);

  (*channelNvList)[ch].tcrwp = tcrwp->Data;
  MemWritePciCfgEp (host, socket, ch, TCRWP_MCDDC_CTL_REG, tcrwp->Data);

  MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, tcothp2->Data);
}


#ifdef  LRDIMM_SUPPORT
void
SetEncodeCSMode(
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    dimm,
             UINT32   *lrdimmQrDimmBitMask
)
{
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);

  if (IsLrdimmPresent(host, socket, ch, dimm)) {
    if ((*dimmNvList)[dimm].lrRankMult > 1) {
      *lrdimmQrDimmBitMask |= (1 << dimm);
    }
  }
} // SetEncodeCSMode
#endif // LRDIMM_SUPPORT

void
SetStaggerRefEn(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch
)
{
  MCMNT_CHKN_BIT_MCDDC_CTL_STRUCT         chknBitMcddcCtl;

  chknBitMcddcCtl.Data = MemReadPciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG);
  chknBitMcddcCtl.Bits.stagger_ref_en = 1;
  MemWritePciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG, chknBitMcddcCtl.Data);
}

void
IdtLrbufWA(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch,
             WORK_AROUND_TYPE                            wa,
             void                                        *data
)
{
  UINT8  dimm;
  TCOTHP_MCDDC_CTL_STRUCT                     *tcothp;
  TCSRFTP_MCDDC_CTL_STRUCT                    tcsrftp;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  switch(wa){
    case t_cke:
    break; // t_cke
    case t_xp:
      tcothp = (TCOTHP_MCDDC_CTL_STRUCT*)data;
      tcothp->Bits.t_xp = GettXP(host, socket);
    break; // t_xp
    case t_odt_oe:
      tcothp = (TCOTHP_MCDDC_CTL_STRUCT*)data;
      // Workaround for BIOS HSD 3613265
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        // If native QR and 2 registers present
        if (((*dimmNvList)[dimm].numRanks == 4) && (IsLrdimmPresent(host, socket, ch, dimm) == 0) &&
            (((*dimmNvList)[dimm].dimmAttrib & 3) == 2)) {

          tcothp->Bits.t_odt_oe = 0;
        }
      }
    break; // t_odt_oe
    case t_xsdll:
      tcsrftp.Data            = MemReadPciCfgEp (host, socket, ch, TCSRFTP_MCDDC_CTL_REG);
      tcsrftp.Bits.t_zqoper   = tZQOPER_DDR4;
      tcsrftp.Bits.t_mod      = GettMOD(host, socket);

      if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
        tcsrftp.Bits.t_xsdll  = 597;
      } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400){
        tcsrftp.Bits.t_xsdll  = 768;
      } else {
        tcsrftp.Bits.t_xsdll  = 1024;
      }

      tcsrftp.Bits.t_xsoffset = GettXSOFFSET(host, socket);

      MemWritePciCfgEp (host, socket, ch, TCSRFTP_MCDDC_CTL_REG, tcsrftp.Data);
    break; // t_xsdll
  } // switch (wa)
}


void
SetMiscDelay(
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             void     *ptcrap
)
{
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT             ddrtMiscDelay;
  TCRAP_MCDDC_CTL_STRUCT                      *tcrap;

  tcrap = (TCRAP_MCDDC_CTL_STRUCT*)ptcrap;

  ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
  if ((*tcrap).Bits.cmd_stretch == TIMING_2N)
  {
    ddrtMiscDelay.Bits.enable_ddrt_2n_timing = 1;
  } else {
    ddrtMiscDelay.Bits.enable_ddrt_2n_timing = 0;
  }
  MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
}

void
Sett_zqcs(
             PSYSHOST                                    host,
             UINT8                                       socket,
             TCZQCAL_MCDDC_CTL_STRUCT                    *tczqcal
)
{
  tczqcal->Bits.t_zqcs     = tZQCS_DDR4;
}
void
SetRdimmTimingCntl(
             PSYSHOST                                    host,
             RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT            *RdimmTimingCntl,
             UINT8                                       aepPresent
)
{
    if (host->setup.mem.options & CA_PARITY_EN) {
      //
      // RDIMM Timing Control
      //
      RdimmTimingCntl->Bits.t_mrd = 12;
    } else {
      RdimmTimingCntl->Bits.t_mrd = tMRD_DDR4;
    }

    RdimmTimingCntl->Bits.t_stab = RdimmTimingCntl->Bits.t_stab + (UINT16)GettSTAB(host, host->var.mem.currentSocket);
    // Add another 5us for total of 10us if aep is present.
    if (aepPresent) {
      RdimmTimingCntl->Bits.t_stab = RdimmTimingCntl->Bits.t_stab + (UINT16)GettSTAB(host, host->var.mem.currentSocket);
    }
}
UINT8
Sett_ckev(
             PSYSHOST                                    host,
             RDIMMTIMINGCNTL2_MCDDC_CTL_STRUCT           *RdimmTimingCntl2
)
{
  return 0;
}

void
PartialWrStarvationCounter(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch,
             PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_STRUCT  *pmmStarvCntr
)
{
  pmmStarvCntr->Data = MemReadPciCfgEp (host, socket, ch, PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_REG);
  if (host->var.kti.OutIsocEn == TRUE) {
    pmmStarvCntr->Bits.wpq_is = WPQ_IS_ISOC;
  } else {
    pmmStarvCntr->Bits.wpq_is = WPQ_IS;
  }
  MemWritePciCfgEp (host, socket, ch, PWMM_STARV_CNTR_PRESCALER_MCDDC_CTL_REG, pmmStarvCntr->Data);
}

void
SetMasterDllWakeUpTimer(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch
)
{
  SREF_STATIC_MCDDC_CTL_STRUCT                sref_static;
  SREF_LL0_MCDDC_CTL_STRUCT                   sref_ll0;
  struct channelNvram                         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  //
  // PM DLL Control
  // Master DLL wake up timer = 500ns
  //
  sref_static.Data            = MemReadPciCfgEp (host, socket, ch, SREF_STATIC_MCDDC_CTL_REG);
  sref_static.Bits.mdll_timer = Saturate(0xfff, (5000000 + (GettCK(host, socket) - 500)) / GettCK(host, socket));
  sref_ll0.Data    = MemReadPciCfgEp (host, socket, ch, SREF_LL0_MCDDC_CTL_REG);
  if ((host->setup.mem.options & MDLL_SHUT_DOWN_EN) && ((*channelNvList)[ch].lrDimmPresent == 0)) {
    sref_ll0.Bits.mdll_off_en = 1;
  } else {
    sref_ll0.Bits.mdll_off_en = 0;
  }
  MemWritePciCfgEp (host, socket, ch, SREF_STATIC_MCDDC_CTL_REG, sref_static.Data);
  MemWritePciCfgEp (host, socket, ch, SREF_LL0_MCDDC_CTL_REG, sref_ll0.Data);
}


void
CheckTxVrefSupport(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch,
             UINT8                                       dimm,
             UINT8                                       rank,
             UINT8                                       strobe,
             INT16                                       *zeroValue
)
{
}

void
WriteFastBootTimings(
             PSYSHOST                                    host,
             UINT8                                       socket,
             UINT8                                       ch,
             UINT8                                       dimm,
             UINT8                                       rank,
             UINT8                                       strobe
)
{
  struct channelNvram           (*channelNvList)[MAX_CH];
  struct ddrRank                (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  rankList = GetRankNvList(host, socket, ch, dimm);
  //SKX TODO: why is this not covered by GetSet API?
  MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG),
                    (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe]);
}


#ifdef  LRDIMM_SUPPORT
void
TimeConstraintLrdimm(
                    PSYSHOST                      host,
                    UINT8                         socket,
                    TCRAP_MCDDC_CTL_STRUCT        *tcrap,
                    TCRWP_MCDDC_CTL_STRUCT        tcrwp,
                    TCDBP_MCDDC_CTL_STRUCT        tcdbp
                    )
{
  if (host->nvram.mem.socket[socket].lrDimmPresent) {
    // MAX(T_WTR of LRDIMM, (T_WRDR' - T_CL + 2))
    if ((tcrap->Bits.t_wtr < (tcrwp.Bits.t_wrdr - tcdbp.Bits.t_cl + 2)) && (tcrwp.Bits.t_wrdr >= (tcdbp.Bits.t_cl - 2))) {
      tcrap->Bits.t_wtr = tcrwp.Bits.t_wrdr - tcdbp.Bits.t_cl + 2;
    }
  }
}
#endif // LRDIMM_SUPPORT

void
Ddr3MrsTimingReg(
                PSYSHOST                                    host,
                UINT8                                       socket,
                UINT8                                       ch,
                RDIMMTIMINGCNTL_MCDDC_CTL_STRUCT            RdimmTimingCntl
                )
{
  TCMRS_MCDDC_CTL_STRUCT                      tcmrs;

  tcmrs.Data            = MemReadPciCfgEp (host, socket, ch, TCMRS_MCDDC_CTL_REG);
  tcmrs.Bits.tmrd_ddr3  = tMRD;
  MemWritePciCfgEp (host, socket, ch, TCMRS_MCDDC_CTL_REG, tcmrs.Data);
}

void
CalcFinalGap(
            UINT16 *gap,
            UINT16 maxGap,
            UINT16 gapDimm
            )
{
  if (maxGap >= 22) {
    gapDimm = maxGap - 22;
  } else {
    gapDimm = 0;
  }

  if ((gapDimm % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) > 0) {
    *gap = (gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + 1;
  } else {
    *gap = gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2);
  }
}

void
SetRestoreTimings(
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch
                 )
{
  struct channelNvram                         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemWritePciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG, (*channelNvList)[ch].roundtrip0);
  MemWritePciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG, (*channelNvList)[ch].roundtrip1);
  MemWritePciCfgEp (host, socket, ch, IOLATENCY0_MCDDC_DP_REG, (*channelNvList)[ch].ioLatency0);
  MemWritePciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG, (*channelNvList)[ch].ioLatency1);
}

void
GetOdtTiming(
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             ODT_TIMING *odtTiming
             )
{
  RD_ODT_TBL2_MCDDC_CTL_STRUCT  rdOdtTiming;
  WR_ODT_TBL2_MCDDC_CTL_STRUCT  wrOdtTiming;

  rdOdtTiming.Data = MemReadPciCfgEp (host, socket, ch, RD_ODT_TBL2_MCDDC_CTL_REG);
  wrOdtTiming.Data = MemReadPciCfgEp (host, socket, ch, WR_ODT_TBL2_MCDDC_CTL_REG);

  odtTiming->odtRdLeading  = (UINT8)rdOdtTiming.Bits.extraleadingodt;
  odtTiming->odtRdTrailing = (UINT8)rdOdtTiming.Bits.extratrailingodt;
  odtTiming->odtWrLeading  = (UINT8)wrOdtTiming.Bits.extra_leading_odt;
  odtTiming->odtWrTrailing = (UINT8)wrOdtTiming.Bits.extra_trailing_odt;

  if (wrOdtTiming.Bits.extra_leading_odt == 0) {
    odtTiming->odtWrLeading = 1;
  }
}

void
CheckBoundsChip(
PSYSHOST        host,
MEM_TIMING_TYPE timing,
UINT32          *value
)
/*++

This function ensures we do not overflow

@param host    - Point to sysHost
@param timing  - Timing parameter
@param *value  - Pointer to the value to check

@retval void

--*/
{
  if (*value > memTimingMinMax[timing].maxVal) {
    rcPrintf((host, memTimingMinMax[timing].timing_string_ptr));
    rcPrintf((host, " -- Register Overflow! maxVal = 0x%x, value = 0x%x\n", memTimingMinMax[timing].maxVal, *value));
    *value = memTimingMinMax[timing].maxVal;
#if SMCPKG_SUPPORT
    OutputWarning(host, WARN_REGISTER_OVERFLOW, WARN_MINOR_REGISTER_OVERFLOW, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else
    OutputWarning(host, WARN_REGISTER_OVERFLOW, WARN_MINOR_REGISTER_OVERFLOW, 0, 0, 0, 0);
#endif
  }
} // CheckBounds

