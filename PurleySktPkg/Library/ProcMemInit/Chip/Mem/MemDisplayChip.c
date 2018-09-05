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
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "MemProjectSpecific.h"
#include "MemFuncChip.h"


#ifdef SERIAL_DBG_MSG

//
// Local Prototypes
//
void DisplaySwizzleTrainResults(PSYSHOST host, UINT8 socket);
void DisplayCompRegResultsSkx(PSYSHOST host, UINT8 socket);

void
DisplayTcwlAdj(
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch,
    GSM_GT    group
    )
/*++

  Displays the tCOTHP Data

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param ch:           Memory Channel
  @param group:        GSM_GT group

  @retval N/A

--*/
{
  TCOTHP_MCDDC_CTL_STRUCT   tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT  tCOTHP2;

  if(group == TxDqsDelay) {
    tCOTHP.Data  = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
    tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "t_cwl_adj = %d\n", tCOTHP.Bits.t_cwl_adj-tCOTHP2.Bits.t_cwl_adj_neg));
  }
}

/*++

  Displays the training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/

void
DisplayXoverResults(
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8               ch;
  UINT8               strobe;
  DATAOFFSETTRAINN0_0_MCIO_DDRIO_STRUCT           dataOffsetTrain;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_XOVER\n"));

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "       CMDN CMDS  CTL  CKE  CLK    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " %3d", (*channelNvList)[ch].ddrCRCmdTrainingCmdN & 0x3f));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d", (*channelNvList)[ch].ddrCRCmdTrainingCmdS & 0x3f));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d", (*channelNvList)[ch].ddrCRCtlTraining & 0x3f));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d", (*channelNvList)[ch].ddrCRCkeTraining & 0x3f));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d", (*channelNvList)[ch].ddrCRClkTraining & 0x3f));

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      dataOffsetTrain.Data = (*channelNvList)[ch].dataOffsetTrain[strobe];
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d", dataOffsetTrain.Bits.rcvenoffset));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));
  }
} // DisplayXoverResults

/*++

  Displays the training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/

void
DisplayXoverResultsFnv(
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8               ch;
  UINT8               dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_XOVER_FNV\n"));

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "          CMDS\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if (!(*dimmNvList)[dimm].aepDimmPresent) {
        continue;
      }
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, " %3d", (*channelNvList)[ch].ddrCRCmdTrainingCmdSFnv & 0x3f));

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\n"));
    }
  }

} // DisplayXoverResults

/**
  Displays the Sense Amp register settings

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A
**/
void
DisplaySenseAmpTrainingResults(
  PSYSHOST      host,
  UINT8         socket
)
{
  UINT8                                     ch;
  UINT8                                     dimm;
  UINT8                                     rank;
  UINT8                                     nibble;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  struct ddrRank                            (*rankList)[MAX_RANK_DIMM];
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT       rxOffset;

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\nSTART_SENSE_AMP_TRAINING_OFFSETS\n"));
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "Senseamp           bit:   0  1  2  3\n"));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    // only print ranks >0 for debug. 
    dimm = 0;
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);

    rank = 0;
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    for (nibble = 0; nibble < MAX_STROBE; nibble++) {
      if ((!host->nvram.mem.eccEn) && ((nibble == 8) || (nibble == 17))) continue;

      rxOffset.Data = (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][nibble];

      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                    "Nibble %2d: ", nibble));

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %2d", rxOffset.Bits.saoffset0));
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %2d", rxOffset.Bits.saoffset1));
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %2d", rxOffset.Bits.saoffset2));
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %2d", rxOffset.Bits.saoffset3));

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n"));
    } // nibble loop

  }
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n"));
} //DisplaySenseAmpTrainingResults

/**
  Displays the Swizzling register settings

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A
**/
void
DisplaySwizzleTrainResults(
  PSYSHOST      host,
  UINT8         socket
)
{
  UINT8                      ch;
  UINT8                      dimm;
  UINT8                      i;
  struct channelNvram        (*channelNvList)[MAX_CH];
  struct dimmNvram           (*dimmNvList)[MAX_DIMM];

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nSTART_SWIZZLE_TRAINING_RESULTS\n"));

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      // Report results
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    "));
      for (i = 0; i < MAX_BITS; i++) {
        if (i && (i % 8 == 0)) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " "));
        }
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "%d", ((*dimmNvList)[dimm].dqSwz[i / 2] >> ((i % 2) * 4)) & 7));
      } // i loop
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n"));
    } // dimm loop
  } // ch loop
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));

} //DisplaySwizzleTrainResults

void
DisplayCompRegResultsSkx(
  PSYSHOST  host,
  UINT8     socket
)
/*++
  Displays the comp register contents

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/
{
  UINT8                                             ch;
  UINT8                                             strobe;
  struct channelNvram                               (*channelNvList)[MAX_CH];
  COMPDATA0N0_0_MCIO_DDRIO_STRUCT                   compData0;
  COMPDATA1N0_0_MCIO_DDRIO_STRUCT                   compData1;
  DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_STRUCT             ddrCRClkComp;
  DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_STRUCT            ddrCRCmdCompCmdn;
  DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_STRUCT            ddrCRCmdCompCmds;
  DDRCRCMDCOMP_CKE_MCIO_DDRIOEXT_STRUCT            ddrCRCmdCompCke;
  DDRCRCMDCOMP_CTL_MCIO_DDRIOEXT_STRUCT            ddrCRCmdCompCtl;
  DDRCRCTLCOMP_CMDN_MCIO_DDRIOEXT_STRUCT            ddrCRCtlCompCmdn;
  DDRCRCTLCOMP_CMDS_MCIO_DDRIOEXT_STRUCT            ddrCRCtlCompCmds;
  DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_STRUCT            ddrCRCtlCompCke;
  DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_STRUCT            ddrCRCtlCompCtl;

  channelNvList = &host->nvram.mem.socket[socket].channelList;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "START_COMP_REG_DUMP"));


  //DQ dump
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nNibble:\t\t    "));
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", strobe));
  } // strobe loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  DrvUp:\t    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData0.Bits.rcompdrvup));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  DrvDn:\t    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData0.Bits.rcompdrvdown));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  SComp:\t    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData0.Bits.slewratecomp));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX,socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  TCOComp:   "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData0.Bits.tcocomp));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  VTComp:    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData0.Bits.vtcomp));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  PanicDrvUp:"));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA1N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData1.Bits.panicdrvup));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  PanicDrvDn:"));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA1N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData1.Bits.panicdrvdn));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  ODTUp:\t    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA1N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData1.Bits.rcompodtup));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  ODTDn:\t    "));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      compData1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, COMPDATA1N0_0_MCIO_DDRIO_REG));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3u ", compData1.Bits.rcompodtdown));
    }
  } //ch loop

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n\nFub:\t  \t   CLK\t  CMD_CMDN    CMD_CKE    CMD_CTL   CMD_CMDS    CTL_CMDN    CTL_CKE    CTL_CTL    CTL_CMDS\n"));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ddrCRClkComp.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_REG);
    //CMD
    ddrCRCmdCompCmdn.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCmdCompCmds.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCmdCompCke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMP_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCmdCompCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMP_CTL_MCIO_DDRIOEXT_REG);
    //CTL
    ddrCRCtlCompCmdn.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMP_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompCmds.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMP_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompCke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_REG);

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  DrvUp:\t   %3u      %3u        %3u        %3u        %3u        %3u          %3u        %3u       %3u\n",
                   ddrCRClkComp.Bits.rcompdrvup, ddrCRCmdCompCmdn.Bits.rcompdrvup, ddrCRCmdCompCke.Bits.rcompdrvup, ddrCRCmdCompCtl.Bits.rcompdrvup, ddrCRCmdCompCmds.Bits.rcompdrvup,
                   ddrCRCtlCompCmdn.Bits.rcompdrvup, ddrCRCtlCompCke.Bits.rcompdrvup, ddrCRCtlCompCtl.Bits.rcompdrvup, ddrCRCtlCompCmds.Bits.rcompdrvup));

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  DrvDn:\t   %3u      %3u        %3u        %3u        %3u        %3u          %3u        %3u       %3u\n",
                   ddrCRClkComp.Bits.rcompdrvdown, ddrCRCmdCompCmdn.Bits.rcompdrvdown, ddrCRCmdCompCke.Bits.rcompdrvdown, ddrCRCmdCompCtl.Bits.rcompdrvdown, ddrCRCmdCompCmds.Bits.rcompdrvdown,
                   ddrCRCtlCompCmdn.Bits.rcompdrvdown, ddrCRCtlCompCke.Bits.rcompdrvdown, ddrCRCtlCompCtl.Bits.rcompdrvdown, ddrCRCtlCompCmds.Bits.rcompdrvdown));

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  Scomp:\t   %3u      %3u        %3u        %3u        %3u        %3u          %3u        %3u       %3u\n",
                   ddrCRClkComp.Bits.scomp, ddrCRCmdCompCmdn.Bits.scomp, ddrCRCmdCompCke.Bits.scomp, ddrCRCmdCompCtl.Bits.scomp, ddrCRCmdCompCmds.Bits.scomp,
                   ddrCRCtlCompCmdn.Bits.scomp, ddrCRCtlCompCke.Bits.scomp, ddrCRCtlCompCtl.Bits.scomp, ddrCRCtlCompCmds.Bits.scomp));

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  TCOcomp:  %3u      %3u        %3u        %3u        %3u        %3u          %3u        %3u       %3u\n",
                   ddrCRClkComp.Bits.tcocomp, ddrCRCmdCompCmdn.Bits.tcocomp, ddrCRCmdCompCke.Bits.tcocomp, ddrCRCmdCompCtl.Bits.tcocomp, ddrCRCmdCompCmds.Bits.tcocomp,
                   ddrCRCtlCompCmdn.Bits.tcocomp, ddrCRCtlCompCke.Bits.tcocomp, ddrCRCtlCompCtl.Bits.tcocomp, ddrCRCtlCompCmds.Bits.tcocomp));
  }
}

/**
  Displays training results specific to chip/project

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A
**/
void
DisplayResultsChip(
    PSYSHOST  host,
    UINT8     socket
)
{
  DisplaySenseAmpTrainingResults(host, socket);
  DisplayCMDVrefResults(host, socket);
  DisplaySwizzleTrainResults(host, socket);
  MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nSTART_POWER_TRAINING_DUMP\n"));
  DisplayPowerTraingResults(host, socket, TxImode);
  DisplayPowerTraingResults(host, socket, RxEq);
  DisplayPowerTraingResults(host, socket, DramDrvStr);
  DisplayPowerTraingResults(host, socket, TxRon);
  DisplayPowerTraingResults(host, socket, WrOdt);
  DisplayPowerTraingResults(host, socket, RxOdt);
  DisplayPowerTraingResults(host, socket, TxEq);
  DisplayCompRegResultsSkx(host, socket);
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n"));
}

#endif // SERIAL_DBG_MSG

