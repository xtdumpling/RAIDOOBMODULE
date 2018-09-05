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
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
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
#include "SysFuncChip.h"
#include "SysHostChip.h"
#include "RcRegs.h"
#include "Cpgc.h"
#include "MemHostChip.h"
#include "MemFuncChip.h"
#include "MemApiSkx.h"


#define   X_OVER_STEP_SIZE  1
#define   X_OVER_PI_START   0
#define   X_OVER_MAX_SAMPLE 64

#define   MAX_CMDPI       6
#define   MAX_CLKPI       8
#define   CMDN_INDEX      0
#define   CMDS_INDEX      1
#define   CTL_INDEX       2
#define   CKE_INDEX       3
#define   MAX_CMD_FUB     4

// B0 offsets
UINT8  B0_CLK_OFFSET[MAX_CH] = {60,60,60,60,60,60};
UINT8  B0_CMD_OFFSET[MAX_CH] = {47,48,48,47,48,48};
UINT8  B0_DATA_OFFSET[MAX_CH] = {53,53,53,53,53,53};
//B0 slope
UINT8  B0_CLK_SLOPE[MAX_CH] = { 5,10,10, 5,10,10};
UINT8  B0_CMD_SLOPE[MAX_CH] = {15,10,10,15,10,10};
UINT8  B0_DATA_SLOPE[MAX_CH] = {10,10,10,10,10,10};

 //ch 1866, 2133, 2400, 2666, 2933, 3200
INT16 CLK_X_OVER_OFFSET[MAX_CH][6] = {
{52,56,60, 0, 4, 8},
{52,56,60, 0, 4, 8},
{52,56,60, 0, 4, 8},
{52,56,60, 0, 4, 8},
{52,56,60, 0, 4, 8},
{52,56,60, 0, 4, 8}};

INT16 CMD_X_OVER_OFFSET[MAX_CH][6] = {
{45,47,49,51,53,55},
{45,47,49,51,53,55},
{45,47,49,51,53,55},
{45,47,49,51,53,55},
{45,47,49,51,53,55},
{45,47,49,51,53,55}};

INT16 DATA_X_OVER_OFFSET[MAX_CH][6] = {
{44,46,48,50,52,54},
{44,46,48,50,52,54},
{44,46,48,50,52,54},
{44,46,48,50,52,54},
{44,46,48,50,52,54},
{44,46,48,50,52,54}};



/**

  Peform crossover calibration

  @param host  - Pointer to sysHost

  @retval Status

**/
UINT32
CrossoverCalib (
               PSYSHOST  host
               )
{
  UINT8                                 socket;
  UINT8                                 ch;
  UINT8                                 strobe;
  UINT16							                	dataOffset;
  UINT16							                	cmdOffset;
  UINT16							                	clkOffset;
  UINT16                                piDelay;
  UINT32                                feedBack;
  UINT8                                 sampleCounter;
  UINT8                                 previousStatusData[MAX_CH][MAX_STROBE];
  UINT8                                 previousStatusCmdN[MAX_CH][MAX_CMDPI];
  UINT8                                 previousStatusCmdS[MAX_CH][MAX_CMDPI];
  UINT8                                 previousStatusCtl[MAX_CH][MAX_CMDPI];
  UINT8                                 previousStatusCke[MAX_CH][MAX_CMDPI];
  UINT8                                 previousStatusClk[MAX_CH][MAX_CLKPI];
  UINT16                                offset[MAX_CH][MAX_STROBE];
  UINT16                                offsetCmdN[MAX_CH][MAX_CMDPI];
  UINT16                                offsetCmdS[MAX_CH][MAX_CMDPI];
  UINT16                                offsetCtl[MAX_CH][MAX_CMDPI];
  UINT16                                offsetCke[MAX_CH][MAX_CMDPI];
  UINT16                                offsetClk[MAX_CH][MAX_CLKPI];
  UINT8                                 cmdLoop;
  UINT8                                 cmdnCounter[MAX_CMDPI];
  UINT8                                 cmdsCounter[MAX_CMDPI];
  UINT8                                 ckeCounter[MAX_CMDPI];
  UINT8                                 ctlCounter[MAX_CMDPI];
  UINT8                                 clkCounter[MAX_CLKPI];
  UINT8                                 cmdnResult[MAX_CMDPI];
  UINT8                                 cmdsResult[MAX_CMDPI];
  UINT8                                 ckeResult[MAX_CMDPI];
  UINT8                                 ctlResult[MAX_CMDPI];
  UINT8                                 clkResult[MAX_CLKPI];
  UINT8                                 dataCntr[MAX_STROBE];
  UINT8                                 dataResult[MAX_STROBE];
  struct channelNvram                   (*channelNvList)[MAX_CH];
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT              dataControl0;
  DATAOFFSETTRAINN0_0_MCIO_DDRIO_STRUCT           dataOffsetTrain;
  DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_STRUCT      ddrCRCmdControls;
  DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT           ddrCRClkControls;
  DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_STRUCT      ddrCRCmdNTraining;
  DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_STRUCT      ddrCRCmdSTraining;
  DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_STRUCT       ddrCRCtlTraining;
  DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_STRUCT       ddrCRCkeTraining;
  DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_STRUCT     ddrCRCmdControls3;
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT          ddrCRClkRanksUsed;
  DATACONTROL4N0_0_MCIO_DDRIO_STRUCT              dataControl4;
  DDRCRCLKTRAINING_MCIO_DDRIOEXT_STRUCT           ddrCRClkTraining;
  UINT8     i;
  UINT16    minOffset[MAX_CMD_FUB];
  UINT16    maxOffset[MAX_CMD_FUB];
  UINT16    midOffset[MAX_CMD_FUB];
  UINT16    minClkOffset;
  UINT16    maxClkOffset;
  UINT16    midClkOffset;
  UINT8     count1cmdn[MAX_CMDPI];
  UINT8     count1cmds[MAX_CMDPI];
  UINT8     count1ctl[MAX_CMDPI];
  UINT8     count1cke[MAX_CMDPI];
  UINT8     count1clk[MAX_CLKPI];
  UINT8     count1strobe[MAX_STROBE];
  // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
  if (~host->memFlows & MF_X_OVER_EN) return SUCCESS;

#ifdef SERIAL_DBG_MSG
  getPrintFControl(host);
#endif
  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nCrossoverCalib Starts\n"));
#endif

  channelNvList = GetChannelNvList(host, socket);

  dataControl0.Data = 0;

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {

#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
    SetMemPhaseCP (host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION);
#endif  // SERIAL_DBG_MSG
#endif  // DEBUG_PERFORMANCE_STATS

    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Enable xovercal\n"));
    OutputCheckpoint(host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // List of SKX registers containing xovercal
      //2_22_0
      //DDRDATA0N0CH0_CR_DATACONTROL0N0_0_REG
      //DDRDATA0N1CH0_CR_DATACONTROL0N1_0_REG
      //DDRDATA1N0CH0_CR_DATACONTROL0N0_1_REG
      //DDRDATA1N1CH0_CR_DATACONTROL0N1_1_REG
      //DDRDATA2N0CH0_CR_DATACONTROL0N0_2_REG
      //DDRDATA2N1CH0_CR_DATACONTROL0N1_2_REG
      //DDRDATA3N0CH0_CR_DATACONTROL0N0_3_REG
      //DDRDATA3N1CH0_CR_DATACONTROL0N1_3_REG
      //DDRDATA8N0CH0_CR_DATACONTROL0N0_8_REG
      //DDRDATA8N1CH0_CR_DATACONTROL0N1_8_REG
      //DDRDATA4N0CH0_CR_DATACONTROL0N0_4_REG
      //DDRDATA4N1CH0_CR_DATACONTROL0N1_4_REG
      //DDRDATA5N0CH0_CR_DATACONTROL0N0_5_REG
      //DDRDATA5N1CH0_CR_DATACONTROL0N1_5_REG
      //2_22_1
      //DDRDATA6N0CH0_CR_DATACONTROL0N0_6_REG
      //DDRDATA6N1CH0_CR_DATACONTROL0N1_6_REG
      //DDRDATA7N0CH0_CR_DATACONTROL0N0_7_REG
      //DDRDATA7N1CH0_CR_DATACONTROL0N1_7_REG
      //DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CMDN_REG
      //DDRCKECH0_CR_DDRCRCMDCONTROLS_CKE_REG
      //DDRCLKCH0_CR_DDRCRCLKCONTROLS_REG
      //DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CMDS_REG
      //DDRCTLCH0_CR_DDRCRCMDCONTROLS_CTL_REG

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRClkControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);
      ddrCRClkControls.Bits.xovercal = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, ddrCRClkControls.Data | (*channelNvList)[ch].ddrCRClkControls);

      dataControl0.Bits.xovercal = 1;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } // strobe loop


      for (i = 0; i < MAX_CMDPI; i++) {
         count1cmdn[i] = 0;
         count1cmds[i] = 0;
         count1ctl[i] = 0;
         count1cke[i] = 0;

        offsetCmdN[ch][i] = 0;
        offsetCmdS[ch][i] = 0;
        offsetCtl[ch][i] = 0;
        offsetCke[ch][i] = 0;

        previousStatusCmdN[ch][i] = 0;
        previousStatusCmdS[ch][i] = 0;
        previousStatusCtl[ch][i] = 0;
        previousStatusCke[ch][i] = 0;
      }
      // Special case for SKX phase detector not implemented
      //trainingDoneCtl[ch][0] = 1;
      //trainingDoneCke[ch][0] = 1;

      for (i = 0; i < MAX_CLKPI; i++) {
        offsetClk[ch][i] = 0;
        previousStatusClk[ch][i] = 0;
        count1clk[i] = 0;
      }

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        previousStatusData[ch][strobe] = 0;
        offset[ch][strobe] = 0;
        count1strobe[strobe] = 0;
      } // strobe loop

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "piDelay CMDN[0:5] CMDS[0:5] CTL[0:5]  CKE[0:5]   CLK[0:7]    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17\n"));


      //SKX training algorithm: remains the same as HSX x-over training, except:
      //  BIOS will sweep RefPiClkDelay (instead of DdrTxDqPiDelayQnnnL)
      //  Resulting offset is programmed into RcvEnOffset, TxDqOffset, TxDqsOffset AND RefPiClkDelay (last one is new)

      // List of SKX registers containing RefPiClkDelay
      //2_22_0
      //DDRDATA0N0CH0_CR_DATACONTROL4N0_0_REG
      //DDRDATA0N1CH0_CR_DATACONTROL4N1_0_REG
      //DDRDATA1N0CH0_CR_DATACONTROL4N0_1_REG
      //DDRDATA1N1CH0_CR_DATACONTROL4N1_1_REG
      //DDRDATA2N0CH0_CR_DATACONTROL4N0_2_REG
      //DDRDATA2N1CH0_CR_DATACONTROL4N1_2_REG
      //DDRDATA3N0CH0_CR_DATACONTROL4N0_3_REG
      //DDRDATA3N1CH0_CR_DATACONTROL4N1_3_REG
      //DDRDATA8N0CH0_CR_DATACONTROL4N0_8_REG
      //DDRDATA8N1CH0_CR_DATACONTROL4N1_8_REG
      //DDRDATA4N0CH0_CR_DATACONTROL4N0_4_REG
      //DDRDATA4N1CH0_CR_DATACONTROL4N1_4_REG
      //DDRDATA5N0CH0_CR_DATACONTROL4N0_5_REG
      //DDRDATA5N1CH0_CR_DATACONTROL4N1_5_REG
      //2_22_1
      //DDRDATA6N0CH0_CR_DATACONTROL4N0_6_REG
      //DDRDATA6N1CH0_CR_DATACONTROL4N1_6_REG
      //DDRDATA7N0CH0_CR_DATACONTROL4N0_7_REG
      //DDRDATA7N1CH0_CR_DATACONTROL4N1_7_REG
      //DDRCMDNCH0_CR_DDRCRCMDCONTROLS3_CMDN_REG - cmdrefpiclkdelay
      //DDRCKECH0_CR_DDRCRCMDCONTROLS3_CKE_REG - cmdrefpiclkdelay
      //DDRCLKCH0_CR_DDRCRCLKRANKSUSED_REG - refpiclk
      //DDRCMDSCH0_CR_DDRCRCMDCONTROLS3_CMDS_REG - cmdrefpiclkdelay
      //DDRCTLCH0_CR_DDRCRCMDCONTROLS3_CTL_REG - cmdrefpiclkdelay

      // A-Stepping Cmd/CTL DivBy2AlignCtl Xover Workaround
      if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
        ddrCRCmdControls3.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.divby2alignctl = 1;
        MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.divby2alignctl = 1;
        MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.divby2alignctl = 1;
        MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.divby2alignctl = 1;
        MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);
      }

      for (piDelay = X_OVER_PI_START; piDelay < X_OVER_MAX_SAMPLE; piDelay += X_OVER_STEP_SIZE) {

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "         %2d     ", piDelay));

        for (i = 0; i < MAX_CMDPI; i++) {
          cmdnCounter[i] = 0;
          cmdsCounter[i] = 0;
          ckeCounter[i] = 0;
          ctlCounter[i] = 0;
        }
        for (i = 0; i < MAX_CLKPI; i++) {
          clkCounter[i] = 0;
        }
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          dataCntr[strobe]  = 0;
        }

        ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
        MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
        MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
        MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG);
        ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
        MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);

        ddrCRClkRanksUsed.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
        ddrCRClkRanksUsed.Bits.refpiclk = piDelay;
        MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksUsed.Data);

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          dataControl4.Data = (*channelNvList)[ch].dataControl4[strobe];
          dataControl4.Bits.refpiclkdelay = piDelay;
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), dataControl4.Data);
        } // strobe loop

        // HSD4165676: Multiple sample during crossover calibration
        for (sampleCounter = 0; sampleCounter < XOVER_CALIB_SAMPLE_COUNT; sampleCounter++) {
          FixedDelay(host, 1); // might want to fine tune this delay with real silicon

          //list of SKX regs containing xoverphasedet
          //2_22_1
          //DDRCMDNCH0_CR_DDRCRCMDTRAINING_CMDN_REG
          //DDRCKECH0_CR_DDRCRCMDTRAINING_CKE_REG
          //DDRCLKCH0_CR_DDRCRCLKTRAINING_REG - pclkxoverphasedet
          //DDRCLKCH0_CR_DDRCRCLKTRAINING_REG - nclkxoverphasedet
          //DDRCMDSCH0_CR_DDRCRCMDTRAINING_CMDS_REG
          //DDRCTLCH0_CR_DDRCRCMDTRAINING_CTL_REG

          ddrCRCmdNTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG);
          ddrCRCmdSTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG);
          ddrCRCtlTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG);
          ddrCRCkeTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG);


          for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if ((ddrCRCmdNTraining.Bits.xoverphasedet >> cmdLoop) & 1) {
              cmdnCounter[cmdLoop]++;
            }
            if ((ddrCRCmdSTraining.Bits.xoverphasedet >> cmdLoop) & 1) {
              cmdsCounter[cmdLoop]++;
            }
            if ((ddrCRCtlTraining.Bits.xoverphasedet >> cmdLoop) & 1) {
              ctlCounter[cmdLoop]++;
            }
            if ((ddrCRCkeTraining.Bits.xoverphasedet >> cmdLoop) & 1) {
              ckeCounter[cmdLoop]++;
            }
          }
          ddrCRClkTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG);
          for (cmdLoop = 0; cmdLoop < MAX_CLKPI; cmdLoop++) {
            if (cmdLoop < (MAX_CLKPI / 2)) {
              if ((ddrCRClkTraining.Bits.pclkxoverphasedet >> cmdLoop) & 1) {
                clkCounter[cmdLoop]++;
              }
            } else {
              if ((ddrCRClkTraining.Bits.nclkxoverphasedet >> (cmdLoop % (MAX_CLKPI / 2))) & 1) {
                clkCounter[cmdLoop]++;
              }
            }
          }

          //DDRDATA0N0CH0_CR_DATATRAINFEEDBACKN0_0_REG
          //DDRDATA0N1CH0_CR_DATATRAINFEEDBACKN1_0_REG
          //DDRDATA1N0CH0_CR_DATATRAINFEEDBACKN0_1_REG
          //DDRDATA1N1CH0_CR_DATATRAINFEEDBACKN1_1_REG
          //DDRDATA2N0CH0_CR_DATATRAINFEEDBACKN0_2_REG
          //DDRDATA2N1CH0_CR_DATATRAINFEEDBACKN1_2_REG
          //DDRDATA3N0CH0_CR_DATATRAINFEEDBACKN0_3_REG
          //DDRDATA3N1CH0_CR_DATATRAINFEEDBACKN1_3_REG
          //DDRDATA8N0CH0_CR_DATATRAINFEEDBACKN0_8_REG
          //DDRDATA8N1CH0_CR_DATATRAINFEEDBACKN1_8_REG
          //DDRDATA4N0CH0_CR_DATATRAINFEEDBACKN0_4_REG
          //DDRDATA4N1CH0_CR_DATATRAINFEEDBACKN1_4_REG
          //DDRDATA5N0CH0_CR_DATATRAINFEEDBACKN0_5_REG
          //DDRDATA5N1CH0_CR_DATATRAINFEEDBACKN1_5_REG

          /* datatrainfeedback - Bits[17:0], RW_L, default = 1'b0 
             Data Training Feedback Storage
             
                     DataTrainFeedback Description:
                      Training Step    Bits    Nibble    Description
                     --------------    ----    -------  
             -------------------------------------------------------------------------------------------------------------------------------------------------------- 
              - SaOffset Calibration: during SA offset calibration, RX outputs are stored in bits 3:0.
              - Xover Calibration: during x-over calibration, X-over phase detector output is stored in bit 0
              - Read Leveling: during Rcven/Read leveling training, 9-bit counter output is stored in bits 8:0
              - Write Leveling: during Write leveling training, 9-bit counter output is stored in bits 8:0
          */

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            feedBack = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));
            if (feedBack & 1) {
              dataCntr[strobe]++;
            }
          } // strobe loop        
        }


        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
          if ((cmdnCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            cmdnResult[cmdLoop] = 1;
          } else {
            cmdnResult[cmdLoop] = 0;
          }
          if ((cmdsCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            cmdsResult[cmdLoop] = 1;
          } else {
            cmdsResult[cmdLoop]= 0;
          }
          if ((ckeCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            ckeResult[cmdLoop] = 1;
          } else {
            ckeResult[cmdLoop] = 0;
          }
          if ((ctlCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            ctlResult[cmdLoop] = 1;
          } else {
            ctlResult[cmdLoop] = 0;
          }
        }

        for (cmdLoop = 0; cmdLoop < MAX_CLKPI; cmdLoop++) {
          if ((clkCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            clkResult[cmdLoop] = 1;
          } else {
            clkResult[cmdLoop] = 0;
          }
        }

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((dataCntr[strobe] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
            dataResult[strobe] = 1;
          } else {
            dataResult[strobe] = 0;
          }
        }

        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if (cmdnResult[cmdLoop]) {
              count1cmdn[cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0"));
            }
            if ((previousStatusCmdN[ch][cmdLoop] != 0) & (cmdnResult[cmdLoop] == 0)) {
              if (piDelay > offsetCmdN[ch][cmdLoop]) {
                offsetCmdN[ch][cmdLoop] = piDelay;
              }

            } 
              previousStatusCmdN[ch][cmdLoop] = cmdnResult[cmdLoop];

        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    "));

        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if (cmdsResult[cmdLoop]) {
              count1cmds[cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0"));
            }
            if ((previousStatusCmdS[ch][cmdLoop] != 0) & (cmdsResult[cmdLoop] == 0)) {
              if (piDelay > offsetCmdS[ch][cmdLoop]) {
                offsetCmdS[ch][cmdLoop] = piDelay;
              }

            } 
              previousStatusCmdS[ch][cmdLoop] = cmdsResult[cmdLoop];

        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    "));

        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if (ctlResult[cmdLoop]) {
              if(cmdLoop >= 1) count1ctl[cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0"));
            }
            if ((previousStatusCtl[ch][cmdLoop] != 0) & (ctlResult[cmdLoop] == 0)) {
              if (piDelay > offsetCtl[ch][cmdLoop]) {
                offsetCtl[ch][cmdLoop] = piDelay;
              }
            } 
              previousStatusCtl[ch][cmdLoop] = ctlResult[cmdLoop];

        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    "));

        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if (ckeResult[cmdLoop]) {
              if (cmdLoop >= 1) count1cke[cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0"));
            }
            if ((previousStatusCke[ch][cmdLoop] != 0) & (ckeResult[cmdLoop] == 0)) {
              if (piDelay > offsetCke[ch][cmdLoop]) {
                offsetCke[ch][cmdLoop] = piDelay;
              }

            } 
              previousStatusCke[ch][cmdLoop] = ckeResult[cmdLoop];
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    "));

        for (cmdLoop = 0; cmdLoop < MAX_CLKPI; cmdLoop++) {
            if (clkResult[cmdLoop]) {
              count1clk[cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0"));
            }
            if ((previousStatusClk[ch][cmdLoop] != 0) & (clkResult[cmdLoop] == 0)) {
              if (piDelay > offsetClk[ch][cmdLoop]) {
                offsetClk[ch][cmdLoop] = piDelay;
              }
            }
              previousStatusClk[ch][cmdLoop] = clkResult[cmdLoop];
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    "));

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {

            if (dataResult[strobe]) {
              count1strobe[strobe]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "1  "));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "0  "));
            }
            // Check if previous was one and current is 0
            if (previousStatusData[ch][strobe] && (dataResult[strobe] == 0)) {
              if (piDelay > offset[ch][strobe]) {
                offset[ch][strobe] = piDelay;
              }
            } 
              previousStatusData[ch][strobe] = dataResult[strobe];
        } 

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n"));
      } // piDelay loop

      //Print Number of 1s from all phase detectors
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\n           CMDN [0][1][2][3][4][5] CMDS[0][1][2][3][4][5] CTL[1][2][3][4][5] CKE[1][2][3][4][5] CLK[0][1][2][3][4][5][6][7]   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17\n"));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "COUNT NON-ZERO: "));
      for (i = 0; i < MAX_CMDPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1cmdn[i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "     "));
      for (i = 0; i < MAX_CMDPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1cmds[i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "    "));
      for (i = 1; i < MAX_CMDPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1ctl[i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "    "));
      for (i = 1; i < MAX_CMDPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1cke[i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "    "));
      for (i = 0; i < MAX_CLKPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1clk[i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "   "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "%d ", count1strobe[strobe]));
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\n"));

      //
      // Disable xover training mode
      //
      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls.Bits.xovercal = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);

      ddrCRClkControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);
      ddrCRClkControls.Bits.xovercal = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkControls);

      dataControl0.Bits.xovercal = 0;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } // strobe loop

      // List of SKX registers containing RefPiClkDelay
      //2_22_0
      //DDRDATA0N0CH0_CR_DATACONTROL4N0_0_REG
      //DDRDATA0N1CH0_CR_DATACONTROL4N1_0_REG
      //DDRDATA1N0CH0_CR_DATACONTROL4N0_1_REG
      //DDRDATA1N1CH0_CR_DATACONTROL4N1_1_REG
      //DDRDATA2N0CH0_CR_DATACONTROL4N0_2_REG
      //DDRDATA2N1CH0_CR_DATACONTROL4N1_2_REG
      //DDRDATA3N0CH0_CR_DATACONTROL4N0_3_REG
      //DDRDATA3N1CH0_CR_DATACONTROL4N1_3_REG
      //DDRDATA8N0CH0_CR_DATACONTROL4N0_8_REG
      //DDRDATA8N1CH0_CR_DATACONTROL4N1_8_REG
      //DDRDATA4N0CH0_CR_DATACONTROL4N0_4_REG
      //DDRDATA4N1CH0_CR_DATACONTROL4N1_4_REG
      //DDRDATA5N0CH0_CR_DATACONTROL4N0_5_REG
      //DDRDATA5N1CH0_CR_DATACONTROL4N1_5_REG
      //2_22_1
      //DDRDATA6N0CH0_CR_DATACONTROL4N0_6_REG
      //DDRDATA6N1CH0_CR_DATACONTROL4N1_6_REG
      //DDRDATA7N0CH0_CR_DATACONTROL4N0_7_REG
      //DDRDATA7N1CH0_CR_DATACONTROL4N1_7_REG
      //DDRCMDNCH0_CR_DDRCRCMDCONTROLS3_CMDN_REG - cmdrefpiclkdelay
      //DDRCKECH0_CR_DDRCRCMDCONTROLS3_CKE_REG - cmdrefpiclkdelay
      //DDRCLKCH0_CR_DDRCRCLKRANKSUSED_REG - refpiclk
      //DDRCMDSCH0_CR_DDRCRCMDCONTROLS3_CMDS_REG - cmdrefpiclkdelay
      //DDRCTLCH0_CR_DDRCRCMDCONTROLS3_CTL_REG - cmdrefpiclkdelay

      //
      // Determine best CMD offsets
      //
      for (i = 0; i < MAX_CMD_FUB; i++) {
        minOffset[i] = 0xFF;
        maxOffset[i] = 0;
      }

      for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {

        // Get min/max values for each CMD fub
        if (offsetCmdN[ch][cmdLoop] < minOffset[CMDN_INDEX]) {
          minOffset[CMDN_INDEX] = offsetCmdN[ch][cmdLoop];
        }
        if (offsetCmdN[ch][cmdLoop] > maxOffset[CMDN_INDEX]) {
          maxOffset[CMDN_INDEX] = offsetCmdN[ch][cmdLoop];
        }

        if (offsetCmdS[ch][cmdLoop] < minOffset[CMDS_INDEX]) {
          minOffset[CMDS_INDEX] = offsetCmdS[ch][cmdLoop];
        }
        if (offsetCmdS[ch][cmdLoop] > maxOffset[CMDS_INDEX]) {
          maxOffset[CMDS_INDEX] = offsetCmdS[ch][cmdLoop];
        }

        // Special case for SKX phase detector not implemented
        if (cmdLoop == 0) {
          continue;
        }
        if (offsetCtl[ch][cmdLoop] < minOffset[CTL_INDEX]) {
          minOffset[CTL_INDEX] = offsetCtl[ch][cmdLoop];
        }
        if (offsetCtl[ch][cmdLoop] > maxOffset[CTL_INDEX]) {
          maxOffset[CTL_INDEX] = offsetCtl[ch][cmdLoop];
        }

        if (offsetCke[ch][cmdLoop] < minOffset[CKE_INDEX]) {
          minOffset[CKE_INDEX] = offsetCke[ch][cmdLoop];
        }
        if (offsetCke[ch][cmdLoop] > maxOffset[CKE_INDEX]) {
          maxOffset[CKE_INDEX] = offsetCke[ch][cmdLoop];
        }
      }
      for (i = 0; i < MAX_CMD_FUB; i++) { 
        if (maxOffset[i] - minOffset[i] > X_OVER_MAX_SAMPLE/2){ 
            midOffset[i] =((minOffset[i] + maxOffset[i] + X_OVER_MAX_SAMPLE) / 2)%X_OVER_MAX_SAMPLE;  
        } else { 
        midOffset[i] = (minOffset[i] + maxOffset[i]) / 2; 
        } 
      } 

      minClkOffset = 0xFF;
      maxClkOffset = 0;
      for (cmdLoop = 0; cmdLoop < MAX_CLKPI; cmdLoop++) {

        // Get min/max values for each CMD fub
        if (offsetClk[ch][cmdLoop] < minClkOffset) {
          minClkOffset = offsetClk[ch][cmdLoop];
        }
        if (offsetClk[ch][cmdLoop] > maxClkOffset) {
          maxClkOffset = offsetClk[ch][cmdLoop];
        }
      }  
      if (maxClkOffset - minClkOffset > X_OVER_MAX_SAMPLE/2){ 
        midClkOffset = ((minClkOffset + maxClkOffset + X_OVER_MAX_SAMPLE) / 2)%X_OVER_MAX_SAMPLE;  
      } else { 
        midClkOffset = (minClkOffset + maxClkOffset) / 2; 
      } 
 
      //
      // Program offsets
      //
      if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
        if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
          cmdOffset = CMD_X_OVER_OFFSET[ch][0];
          clkOffset = CLK_X_OVER_OFFSET[ch][0];
          dataOffset = DATA_X_OVER_OFFSET[ch][0];
        } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
          cmdOffset = CMD_X_OVER_OFFSET[ch][1];
          clkOffset = CLK_X_OVER_OFFSET[ch][1];
          dataOffset = DATA_X_OVER_OFFSET[ch][1];
        } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
          cmdOffset = CMD_X_OVER_OFFSET[ch][2];
          clkOffset = CLK_X_OVER_OFFSET[ch][2];
          dataOffset = DATA_X_OVER_OFFSET[ch][2];
        } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666) {
          cmdOffset = CMD_X_OVER_OFFSET[ch][3];
          clkOffset = CLK_X_OVER_OFFSET[ch][3];
          dataOffset = DATA_X_OVER_OFFSET[ch][3];
        } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2933) {
          cmdOffset = CMD_X_OVER_OFFSET[ch][4];
          clkOffset = CLK_X_OVER_OFFSET[ch][4];
          dataOffset = DATA_X_OVER_OFFSET[ch][4];        
        } else {
          cmdOffset = CMD_X_OVER_OFFSET[ch][5];
          clkOffset = CLK_X_OVER_OFFSET[ch][5];
          dataOffset = DATA_X_OVER_OFFSET[ch][5];
        }
      } else {
        // offsets based on 1866
        cmdOffset = B0_CMD_OFFSET[ch]+((host->nvram.mem.socket[socket].ddrFreq - DDR_1866)*B0_CMD_SLOPE[ch])/10;
        clkOffset = B0_CLK_OFFSET[ch]+((host->nvram.mem.socket[socket].ddrFreq - DDR_1866)*B0_CLK_SLOPE[ch])/10;
        dataOffset = B0_DATA_OFFSET[ch]+((host->nvram.mem.socket[socket].ddrFreq - DDR_1866)*B0_DATA_SLOPE[ch])/10;
      }
      piDelay = (midOffset[CMDN_INDEX]+cmdOffset)%64;
      ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);
      (*channelNvList)[ch].ddrCRCmdControls3CmdN = ddrCRCmdControls3.Data;

      ddrCRCmdNTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG);
      ddrCRCmdNTraining.Bits.trainingoffset = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdNTraining.Data);
      (*channelNvList)[ch].ddrCRCmdTrainingCmdN = ddrCRCmdNTraining.Data;

      piDelay = (midOffset[CMDS_INDEX]+cmdOffset)%64;
	    ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);
      (*channelNvList)[ch].ddrCRCmdControls3CmdS = ddrCRCmdControls3.Data;

      ddrCRCmdSTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdSTraining.Bits.trainingoffset = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdSTraining.Data);
      (*channelNvList)[ch].ddrCRCmdTrainingCmdS = ddrCRCmdSTraining.Data;

      piDelay = (midOffset[CTL_INDEX]+cmdOffset)%64;
      ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);
      (*channelNvList)[ch].ddrCRCmdControls3Ctl = ddrCRCmdControls3.Data;

      ddrCRCtlTraining.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG);
      ddrCRCtlTraining.Bits.trainingoffset = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlTraining.Data);
      (*channelNvList)[ch].ddrCRCtlTraining = ddrCRCtlTraining.Data;

      piDelay = (midOffset[CKE_INDEX]+cmdOffset)%64;
      ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls3.Data);
      (*channelNvList)[ch].ddrCRCmdControls3Cke = ddrCRCmdControls3.Data;

      ddrCRCkeTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG);
      ddrCRCkeTraining.Bits.trainingoffset = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG, ddrCRCkeTraining.Data);
      (*channelNvList)[ch].ddrCRCkeTraining = ddrCRCkeTraining.Data;

      piDelay = (midClkOffset+clkOffset)%64;
      ddrCRClkRanksUsed.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
      ddrCRClkRanksUsed.Bits.refpiclk = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksUsed.Data);
      (*channelNvList)[ch].ddrCRClkRanksUsed = ddrCRClkRanksUsed.Data;

      ddrCRClkTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG);
      ddrCRClkTraining.Bits.trainingoffset = piDelay;
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, ddrCRClkTraining.Data);
      (*channelNvList)[ch].ddrCRClkTraining = ddrCRClkTraining.Data;

      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        piDelay = (offset[ch][strobe] + dataOffset)%64;

        dataControl4.Data = (*channelNvList)[ch].dataControl4[strobe];
        dataControl4.Bits.refpiclkdelay = piDelay;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), dataControl4.Data);
        (*channelNvList)[ch].dataControl4[strobe] = dataControl4.Data;

        dataOffsetTrain.Data = 0;
        dataOffsetTrain.Bits.rcvenoffset = piDelay;
        dataOffsetTrain.Bits.txdqoffset = piDelay;
        dataOffsetTrain.Bits.txdqsoffset = piDelay;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATAOFFSETTRAINN0_0_MCIO_DDRIO_REG), dataOffsetTrain.Data);
        (*channelNvList)[ch].dataOffsetTrain[strobe] = dataOffsetTrain.Data;

      } // strobe loop

    } // ch loop

#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
    ClearMemPhaseCP(host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION);
#endif  // SERIAL_DBG_MSG
#endif  // DEBUG_PERFORMANCE_STATS
  } else if ((host->var.mem.subBootMode == WarmBootFast) || (host->var.mem.subBootMode == ColdBootFast)) {

    //SKX: TODO handle S3 path
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdTrainingCmdN);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdTrainingCmdS);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCtlTraining);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCkeTraining);
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkTraining);

      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3CmdN);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3CmdS);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3Ctl);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3Cke);
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkRanksUsed);

      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {

        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATAOFFSETTRAINN0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataOffsetTrain[strobe]);
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl4[strobe]);
      } // strobe loop
    }
  }
#ifdef SERIAL_DBG_MSG
  DisplayXoverResults(host, socket);
#endif  
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nCrossoverCalib Ends\n"));
#endif
#ifdef SERIAL_DBG_MSG
  releasePrintFControl(host);
#endif
  return SUCCESS;
} // CrossoverCalib

/**

Peform crossover calibration for FNV

@param host  - Pointer to sysHost

@retval Status

**/
UINT32
CrossoverCalibFnv(
                  PSYSHOST  host
)
{
  UINT8                                 socket;
  UINT8                                 ch;
  UINT8                                 dimm;
  UINT16                                piDelay;
  UINT8                                 sampleCounter;
  UINT8                                 previousStatusCmdS[MAX_CH][MAX_CMDPI];
  UINT16                                offsetCmdS[MAX_CH][MAX_CMDPI];
  UINT8                                 cmdLoop;
  UINT8                                 cmdsCounter[MAX_CMDPI];
  UINT8                                 cmdsResult[MAX_CH][MAX_CMDPI];
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct dimmNvram                      (*dimmNvList)[MAX_DIMM];
  DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_STRUCT    ddrCRCmdControls;
  DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_STRUCT    ddrCRCmdSTraining;
  DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_STRUCT   ddrCRCmdControls3;
  DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_STRUCT   ddrCRCmdControls2;
  MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_STRUCT     fnvIoDdrtDll;
  DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_STRUCT   ddrCRCtlRanksUsed;
  UINT8     i;
  UINT16    minOffset;
  UINT16    maxOffset;
  UINT16    midOffset;
  UINT8     count1cmds[MAX_CH][MAX_CMDPI];
  
  if (~host->memFlows & MF_X_OVER_EN) return SUCCESS;

#ifdef SERIAL_DBG_MSG
  getPrintFControl(host);
#endif
  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "\nCrossoverCalibFnv Starts\n"));
#endif

  channelNvList = GetChannelNvList(host, socket);

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {

#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
    //SetMemPhaseCP(host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION);
#endif  // SERIAL_DBG_MSG
#endif  // DEBUG_PERFORMANCE_STATS

    OutputCheckpoint(host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if (!(*dimmNvList)[dimm].aepDimmPresent) {
          continue;
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Enable xovercal\n"));

        fnvIoDdrtDll.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG);
        fnvIoDdrtDll.Bits.ddrtdllpowerdown4qnnnh_val = 0;
        WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG, fnvIoDdrtDll.Data);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls2.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls2.Bits.fnvddrcrdllpibypassen = 0;
        ddrCRCmdControls2.Bits.fnvcmdcrxoverbypassen = 0;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls2.Data, 0, FNV_IO_WRITE, 0xF);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls.Bits.txon = 0;
        ddrCRCmdControls.Bits.xovercal = 1;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_WRITE, 0xF);
      }
    }

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\nCMDS[0:5]:\n"));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "PI "));

    for (ch = 0; ch < MAX_CH; ch++) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "       Ch%d", ch));
    } // ch loop
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
      for (i = 0; i < MAX_CMDPI; i++) {
        count1cmds[ch][i] = 0;
        offsetCmdS[ch][i] = 0;
        previousStatusCmdS[ch][i] = 0;
      }
    }

    for (piDelay = X_OVER_PI_START; piDelay < X_OVER_MAX_SAMPLE; piDelay += X_OVER_STEP_SIZE) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "%2d     ", piDelay));

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].ddrtEnabled == 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "          "));
          continue;
        }

        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if (!(*dimmNvList)[dimm].aepDimmPresent) {
            continue;
          }

          for (i = 0; i < MAX_CMDPI; i++) {
            cmdsCounter[i] = 0;
          }

          GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls3.Data, 0, FNV_IO_READ, 0xF);
          ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
          GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls3.Data, 0, FNV_IO_WRITE, 0xF);

          // HSD4165676: Multiple sample during crossover calibration
          for (sampleCounter = 0; sampleCounter < XOVER_CALIB_SAMPLE_COUNT; sampleCounter++) {
            FixedDelay(host, 1); // might want to fine tune this delay with real silicon

            GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdSTraining.Data, 0, FNV_IO_READ, 0xF);


            for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
              if ((ddrCRCmdSTraining.Bits.xoverphasedet >> cmdLoop) & 1) {
                cmdsCounter[cmdLoop]++;
              }
            }
          } // sampleCounter

          for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if ((cmdsCounter[cmdLoop] * 10) >= ((XOVER_CALIB_SAMPLE_COUNT * 10) / 2)) {
              cmdsResult[ch][cmdLoop] = 1;
            } else {
              cmdsResult[ch][cmdLoop] = 0;
            }
          }

          for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {
            if (cmdsResult[ch][cmdLoop]) {
              count1cmds[ch][cmdLoop]++;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "1"));
            } else {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "0"));
            }
            if ((previousStatusCmdS[ch][cmdLoop] != 0) & (cmdsResult[ch][cmdLoop] == 0)) {
              if (piDelay > offsetCmdS[ch][cmdLoop]) {
                offsetCmdS[ch][cmdLoop] = piDelay;
              }

            }
            previousStatusCmdS[ch][cmdLoop] = cmdsResult[ch][cmdLoop];
          }
        } // dimm
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "    "));
      } // ch
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\n"));
    } // piDelay

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nCOUNT NON-ZERO:\n"));

    //Print Number of 1s from all phase detectors
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "  CMDS [0][1][2][3][4][5]\n"));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, ""));

      for (i = 0; i < MAX_CMDPI; i++) MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%d ", count1cmds[ch][i]));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\n"));
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].ddrtEnabled == 0) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "      "));
        continue;
      }
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if (!(*dimmNvList)[dimm].aepDimmPresent) {
          continue;
        }

        //
        // Disable xover training mode
        //
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls.Bits.xovercal = 0;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_WRITE, 0xF);

        //
        // Determine best CMD offsets
        //
        minOffset = 0xFF;
        maxOffset = 0;

        for (cmdLoop = 0; cmdLoop < MAX_CMDPI; cmdLoop++) {

          // Get min/max values for each CMD fub
          if (offsetCmdS[ch][cmdLoop] < minOffset) {
            minOffset = offsetCmdS[ch][cmdLoop];
          }
          if (offsetCmdS[ch][cmdLoop] > maxOffset) {
            maxOffset = offsetCmdS[ch][cmdLoop];
          }
        }
        if (maxOffset - minOffset > X_OVER_MAX_SAMPLE / 2) {
          midOffset = ((minOffset + maxOffset + X_OVER_MAX_SAMPLE) / 2) % X_OVER_MAX_SAMPLE;
        } else {
          midOffset = (minOffset + maxOffset) / 2;
        }

        //
        // Program offsets
        //
        piDelay = midOffset % 64;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls3.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls3.Bits.cmdrefpiclkdelay = piDelay;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls3.Data, 0, FNV_IO_WRITE, 0xF);
        (*channelNvList)[ch].ddrCRCmdControls3CmdSFnv = ddrCRCmdControls3.Data;

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdSTraining.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdSTraining.Bits.trainingoffset = piDelay;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdSTraining.Data, 0, FNV_IO_WRITE, 0xF);
        (*channelNvList)[ch].ddrCRCmdTrainingCmdSFnv = ddrCRCmdSTraining.Data;

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCtlRanksUsed.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCtlRanksUsed.Bits.ranken = 0x3f;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCtlRanksUsed.Data, 0, FNV_IO_WRITE, 0xF);

      } // dimm loop
    } // ch loop

#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
    //ClearMemPhaseCP(host, STS_CHANNEL_TRAINING, CROSSOVER_CALIBRATION);
#endif  // SERIAL_DBG_MSG
#endif  // DEBUG_PERFORMANCE_STATS
  } else if (host->var.mem.subBootMode == ColdBootFast) {

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].ddrtEnabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if (!(*dimmNvList)[dimm].aepDimmPresent) {
          continue;
        }
        fnvIoDdrtDll.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG);
        fnvIoDdrtDll.Bits.ddrtdllpowerdown4qnnnh_val = 0;
        WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG, fnvIoDdrtDll.Data);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls2.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls2.Bits.fnvddrcrdllpibypassen = 0;
        ddrCRCmdControls2.Bits.fnvcmdcrxoverbypassen = 0;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls2.Data, 0, FNV_IO_WRITE, 0xF);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCmdControls.Bits.txon = 0;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCmdControls.Data, 0, FNV_IO_WRITE, 0xF);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG, &(*channelNvList)[ch].ddrCRCmdTrainingCmdSFnv, 0, FNV_IO_WRITE, 0xF);
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG, &(*channelNvList)[ch].ddrCRCmdControls3CmdSFnv, 0, FNV_IO_WRITE, 0xF);

        GetSetFnvIO(host, socket, ch, dimm, DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCtlRanksUsed.Data, 0, FNV_IO_READ, 0xF);
        ddrCRCtlRanksUsed.Bits.ranken = 0x3f;
        GetSetFnvIO(host, socket, ch, dimm, DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG, &ddrCRCtlRanksUsed.Data, 0, FNV_IO_WRITE, 0xF);
      }
    }
  }
#ifdef SERIAL_DBG_MSG
  DisplayXoverResultsFnv(host, socket);
#endif  
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "\nCrossoverCalibFnv Ends\n"));
#endif
#ifdef SERIAL_DBG_MSG
  releasePrintFControl(host);
#endif
  return SUCCESS;
} // CrossoverCalibFnv
