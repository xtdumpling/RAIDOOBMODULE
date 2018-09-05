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
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
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
 *      This file contains display routines.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "MemFunc.h"

extern UINT8  tWL_DDR3[MAX_SUP_FREQ];
extern UINT8  tWL_DDR4[MAX_SUP_FREQ];
extern UINT8  primaryWidth[MAX_TECH];   // Primary SDRAM device width
extern UINT8  rowBits[MAX_TECH];        // Number of row address bits
extern UINT8  columnBits[MAX_TECH];     // Number of column address bits
extern UINT8  internalBanks[MAX_TECH];  // Number of internal SDRAM banks
extern UINT8  lrdimmExtraOdtDelay[MAX_SUP_FREQ];
extern const UINT32 rxVrefCtrlTable[MAX_STROBE];
extern const UINT32 rxOffsetTable[MAX_STROBE];
extern const UINT32 txXtalkTable[MAX_STROBE];
extern const UINT8 tPARRECOVERY[MAX_SUP_FREQ];

extern const UINT8   BCLKs[MAX_SUP_FREQ];

// -------------------------------------
// Code section
// -------------------------------------

#ifdef SERIAL_DBG_MSG
//
// Local Prototypes
//
void   DisplaySize (PSYSHOST  host, UINT8 socket, UINT8 dimm);
void   DisplayDimmType (PSYSHOST  host, UINT8 socket, UINT8 dimm);
void   DisplayDramMfgDate (PSYSHOST  host, UINT8 socket, UINT8 dimm);
void   DisplaySpeed (PSYSHOST  host, UINT8 socket, UINT8 dimm);
void   DisplayModulePartNum (PSYSHOST  host, UINT8 socket, UINT8 dimm);
void   DisplayIntelSN(PSYSHOST host, UINT8 socket, UINT8 dimm);
void   DisplayEye(PSYSHOST host, UINT8 socket, UINT8 scope, GSM_LT level, void *results, UINT8 maxVrefOffset);
void   DisplayMRS (PSYSHOST host, UINT8 socket);
#ifdef MEM_NVDIMM_EN
void   DisplayNvdimmN (PSYSHOST  host, UINT8 socket, UINT8 dimm);
#endif

/**

  Displays information about the DIMMs present

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
DIMMInfo (
  PSYSHOST host
  )
{
  UINT8                                 socket;
  UINT8                                 ch;
  UINT8                                 dimm;
  UINT16                                userVdd = 0;
  UINT8                                 maxSPC;
  UINT8                                 channelActive;
  UINT8                                 dimmPresent;
  UINT8                                 reqType = 0;
  UINT8                                 reqData = 0;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct dimmNvram                      (*dimmNvList)[MAX_DIMM];
  UINT32                                RcRevision = 0;


  //
  // Do not display if the message level is not minimum or maximum
  //
  if (checkMsgLevel(host, SDBG_MINMAX) == 0) return SUCCESS;

  rcPrintf ((host, "START_DIMMINFO_TABLE\n"));
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;

    PrintLine(host, 2 + 21 * MAX_CH, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    rcPrintf ((host, "START_SOCKET_%d_TABLE\n", socket));

    CHIP_FUNC_CALL(host, DimmInfoDisplayProc (host, socket));

    PrintLine(host, 2 + 21 * MAX_CH, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    rcPrintf ((host, "S|"));
    maxSPC = 0;
    for (ch = 0; ch < MAX_CH; ch++) {
      rcPrintf ((host, "     Channel %d", ch));
      EmptyBlock(host, 6);

      if(maxSPC < host->var.mem.socket[socket].channelList[ch].numDimmSlots) {
        maxSPC = host->var.mem.socket[socket].channelList[ch].numDimmSlots;
      }
    }
    rcPrintf ((host, "\n"));
    PrintLine(host, 2 + 21 * MAX_CH, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

    for (dimm = 0; dimm < maxSPC; dimm++) {
      rcPrintf ((host, "%d|", dimm));

      //
      // Indicate this DIMM isn't present on any channel. Will be updated in channel loop.
      //
      dimmPresent = 0;

      channelNvList = GetChannelNvList(host, socket);
      for (ch = 0; ch < MAX_CH; ch++) {

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent) {
          rcPrintf ((host, "   DIMM: "));
          DisplayManf (host, (*dimmNvList)[dimm].SPDMMfgId);
          dimmPresent = 1;
        } else {
          rcPrintf ((host, "   Not installed"));
          EmptyBlock(host, 4);
        }
      } // ch loop

      //
      // Skip all this other stuff if this DIMM is not present on this channel
      //
      if (dimmPresent == 0) {
        PrintLine(host, 2 + 21 * MAX_CH, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
        continue;
      }

      // New line
      rcPrintf ((host, "\n |"));

      for (ch = 0; ch < MAX_CH; ch++) {

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent) {
          rcPrintf ((host, "   DRAM: "));
          DisplayManf (host, (*dimmNvList)[dimm].SPDDramMfgId);
        } else {
          EmptyBlock(host, 20);
        }
      } // ch loop

      // New line
      rcPrintf ((host, "\n |"));

      if (host->nvram.mem.dimmTypePresent == RDIMM) {
        for (ch = 0; ch < MAX_CH; ch++) {

          dimmNvList = GetDimmNvList(host, socket, ch);

          if ((*dimmNvList)[dimm].dimmPresent) {
            rcPrintf ((host, "    RCD: "));
            DisplayManf (host, (*dimmNvList)[dimm].SPDRegVen);
          } else {
            EmptyBlock(host, 20);
          }
        } // ch loop

        // New line
        rcPrintf ((host, "\n"));
        EmptyBlock(host, 1);
      } // if RDIMM
      else {
        for (ch = 0; ch < MAX_CH; ch++) {
          EmptyBlock(host, 20);
        } // ch loop
        // New line
        rcPrintf ((host, "\n |"));
      }

      DisplaySize (host, socket, dimm);

      // New line
      rcPrintf ((host, "\n |"));

      DisplayDimmType(host, socket, dimm);

      // New line
      rcPrintf ((host, "\n |"));

      DisplaySpeed(host, socket, dimm);

      // New line
      rcPrintf ((host, "\n |"));

      // DRAM Manufacture Date
      DisplayDramMfgDate(host, socket, dimm);

      // New line
      rcPrintf ((host, "\n |"));

      // Module Part Num
      DisplayModulePartNum(host, socket, dimm);

#ifdef SERIAL_DBG_MSG
      // New line
      rcPrintf ((host, "\n |"));

      // Display if the DIMM is mapped out
      DisplayIntelSN(host, socket, dimm);
#endif  // SERIAL_DBG_MSG

#ifdef MEM_NVDIMM_EN
      // New line
      rcPrintf((host, "\n |"));

      // Display Type 1 (NVDIMM-N) information.
      DisplayNvdimmN(host, socket, dimm);
#endif
      // New line
      rcPrintf ((host, "\n |"));

      // Display if the DIMM is mapped out
      CheckMappedOut(host, socket, dimm);

      PrintLine(host, 2 + 21 * MAX_CH, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
    } // dimm loop
    rcPrintf ((host, "STOP_SOCKET_%d_TABLE\n", socket));
    PrintLine(host, 2 + 21 * MAX_CH, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
  } // socket loop

  PrintLine(host, 86, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
  EmptyBlock(host, 20);
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    rcPrintf ((host, "  Socket %d", socket));
    EmptyBlock(host, 2);
  } // socket loop
  rcPrintf ((host, "   System"));
  EmptyBlock(host, 3);
  rcPrintf ((host, "\n"));
  PrintLine(host, 86, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

  // Display Active Memory Size
  rcPrintf ((host, "Active Memory"));
  EmptyBlock(host, 7);
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket))) {

      if ((host->var.mem.socket[socket].memSize >> 4) < 100) {
        rcPrintf ((host, " ", host->var.mem.socket[socket].memSize >> 4));
      }
      if ((host->var.mem.socket[socket].memSize >> 4) < 10) {
        rcPrintf ((host, " ", host->var.mem.socket[socket].memSize >> 4));
      }
      rcPrintf ((host, "    %dGB", host->var.mem.socket[socket].memSize >> 4));
      EmptyBlock(host, 3);
    } else {
      rcPrintf ((host, "     N/A"));
      EmptyBlock(host, 4);
    }
  } // socket

  if ((host->var.mem.memSize >> 4) < 100) {
    rcPrintf ((host, " ", host->var.mem.memSize >> 4));
  }
  if ((host->var.mem.memSize >> 4) < 10) {
    rcPrintf ((host, " ", host->var.mem.memSize >> 4));
  }
  rcPrintf ((host, "    %dGB", host->var.mem.memSize >> 4));
  EmptyBlock(host, 3);
  rcPrintf ((host, "\n"));
  // Display Active Channels

  // DDR Freq
  rcPrintf ((host, "DDR Freq"));
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  //
  // Get current DCLK ratio
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    CHIP_FUNC_CALL(host, GetDclkRatio(host, socket, &reqType, &reqData));
    break;
  } // socket loop

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
    rcPrintf ((host, "  DDR3-"));
  } else {
    rcPrintf ((host, "  DDR4-"));}

  if (reqType == 0){
    switch (reqData) {
      case 6:
        rcPrintf ((host, "800 "));
        break;

      case 8:
        rcPrintf ((host, "1066"));
        break;

      case 10:
        rcPrintf ((host, "1333"));
        break;

      case 12:
        rcPrintf ((host, "1600"));
        break;

      case 14:
        rcPrintf ((host, "1866"));
        break;

      case 16:
        rcPrintf ((host, "2133"));
        break;

      case 18:
        rcPrintf ((host, "2400"));
        break;

      case 20:
        rcPrintf ((host, "2666"));
        break;

      case 22:
        rcPrintf ((host, "2933"));
        break;

      case 24:
        rcPrintf ((host, "3200"));
        break;

      case 26:
        rcPrintf ((host, "3466"));
        break;

      case 27:
        rcPrintf ((host, "3600"));
        break;

      case 28:
        rcPrintf ((host, "3733"));
        break;

      case 30:
        rcPrintf ((host, "4000"));
        break;

      default:
        rcPrintf ((host, "UNKNOWN"));
        break;
    }
  } else {
    switch (reqData) {
      case 10:
        rcPrintf ((host, "1000"));
        break;

      case 12:
        rcPrintf ((host, "1200"));
        break;

      case 14:
        rcPrintf ((host, "1400"));
        break;

      case 16:
        rcPrintf ((host, "1600"));
        break;

      case 18:
        rcPrintf ((host, "1800"));
        break;

      case 20:
        rcPrintf ((host, "2000"));
        break;

      case 22:
        rcPrintf ((host, "2200"));
        break;

      case 26:
        rcPrintf ((host, "2600"));
        break;

      case 28:
        rcPrintf ((host, "2800"));
        break;

      case 30:
        rcPrintf ((host, "3000"));
        break;

      case 34:
        rcPrintf ((host, "3400"));
        break;

      case 38:
        rcPrintf ((host, "3800"));
        break;

      case 42:
        rcPrintf ((host, "4200"));
        break;

      default:
        rcPrintf ((host, "UNKNOWN"));
        break;
    }
  } // switch
  EmptyBlock(host, 1);
  rcPrintf ((host, "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    //
    // Check if this channel is active on any socket
    //
    channelActive = 0;
    for (socket = 0; socket < MAX_SOCKET; socket++) {

      channelNvList = GetChannelNvList(host, socket);
      if ((*channelNvList)[ch].enabled == 0) continue;

      channelActive = 1;
    } // socket loop

    if (channelActive == 0) continue;

    // Ch Timing
    rcPrintf ((host, "Ch%d CL-RCD-RP-CMD", ch));
    EmptyBlock(host, 3);
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      //
      // Continue if this socket is not present
      //
      channelNvList = GetChannelNvList(host, socket);

      if ((*channelNvList)[ch].enabled) {

        // CAS latency (tCL)
        if ((*channelNvList)[ch].common.nCL < 10) rcPrintf ((host, " "));
        rcPrintf ((host, "%d-", (*channelNvList)[ch].common.nCL));

        // Row Address to Column Address Delay (tRCD)
        rcPrintf ((host, "%d-", (*channelNvList)[ch].common.nRCD));

        // Row Active Time (tRP)
        rcPrintf ((host, "%d-", (*channelNvList)[ch].common.nRP));

        // Command timing
        if (!(*channelNvList)[ch].timingMode) {
          rcPrintf ((host, "%dn", (*channelNvList)[ch].timingMode + 1));
        }
        else {
          rcPrintf ((host, "%dn", (*channelNvList)[ch].timingMode));
        }

        if ((*channelNvList)[ch].common.nRCD < 10) rcPrintf ((host, " "));
        if ((*channelNvList)[ch].common.nRP < 10) rcPrintf ((host, " "));
        EmptyBlock(host, 1);
      } else {
        EmptyBlock(host, 12);
      }
    } // socket loop

    EmptyBlock(host, 12);
    rcPrintf ((host, "\n"));
  } // ch loop

  // DDR Vdd
  rcPrintf ((host, "DDR Vdd"));
  EmptyBlock(host, 13);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);

  //
  // Get current Vdd setting
  //
  userVdd = host->setup.mem.inputMemTime.vdd;
  if (userVdd != 0) {
    if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
      if((userVdd < 1000) || (userVdd > 1850)) {
        userVdd = 0;
      }
    } else {
      if((userVdd < 900) || (userVdd > 1850)) {
        userVdd = 0;
      }
    }
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    if (userVdd) {
      rcPrintf ((host, "   %d.%dV", userVdd / 1000, userVdd % 1000));
    } else {
      switch (CHIP_FUNC_CALL(host, GetLvmode(host, socket))) {
        case 0:
          rcPrintf ((host, "   1.500V"));
          break;
        case 1:
          rcPrintf ((host, "   1.350V"));
          break;
        case 3:
          rcPrintf ((host, "   1.200V"));
          break;
        }
      break;
    }
    EmptyBlock(host, 3);
  } // socket loop
  rcPrintf ((host, "\n"));


  CHIP_FUNC_CALL(host, DimmInfoDisplayProcFeatures (host));

  // CCMRC Revision
  rcPrintf ((host, "CCMRC Revision"));
  EmptyBlock(host, 6);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  rcPrintf ((host, "  %02x.%02x.%02x", (CCMRC_REVISION >> 24) & 0xff, (CCMRC_REVISION >> 16) & 0xff, (CCMRC_REVISION >> 8) & 0xff));
  EmptyBlock(host, 2);
  rcPrintf ((host, "\n"));

  // RC Revision
  RcRevision = host->var.common.rcVersion;

  rcPrintf ((host, "RC Revision"));
  EmptyBlock(host, 9);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  EmptyBlock(host, 12);
  rcPrintf ((host, "  %02x.%02x.%02x", (RcRevision >> 24) & 0xff, (RcRevision >> 16) & 0xff, (RcRevision >> 8) & 0xff));
  EmptyBlock(host, 2);
  rcPrintf ((host, "\n"));

  PrintLine(host, 86, DOUBLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
  rcPrintf ((host, "\n"));
  rcPrintf ((host, "STOP_DIMMINFO_TABLE\n"));

  PrintDimmConfig(host);

  return SUCCESS;
} // DIMMInfo


/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
void
DisplaySize (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
{
  UINT8 ch;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);
    chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

    if ((*dimmNvList)[dimm].aepDimmPresent) {

      // Display size
      rcPrintf ((host, "        %4dGB      |", (*dimmNvList)[dimm].rawCap >> 4));

    } else if ((*dimmNvList)[dimm].dimmPresent) {
      // Display size
      rcPrintf ((host, "  %2dGB(", (*chdimmList)[dimm].memSize >> 4));

      // Display DRAM tech
      rcPrintf ((host, "%dGb", (256 << (*dimmNvList)[dimm].sdramCapacity) >> 10));

      // Display DRAM width
      rcPrintf ((host, "x%d", (4 << ((*dimmNvList)[dimm].SPDModuleOrg & 0x7))));

      // Display DRAM stack
      if ((*dimmNvList)[dimm].dieCount) {
        rcPrintf ((host, " %dH", (*dimmNvList)[dimm].dieCount));
      } else {
        rcPrintf ((host, "   "));
      }

      // Display number or ranks
      switch((*dimmNvList)[dimm].numRanks) {
        case 1:
          rcPrintf ((host, " SR)"));
          break;
        case 2:
          rcPrintf ((host, " DR)"));
          break;
        case 3:
          rcPrintf ((host, " TR)"));
          break;
        case 4:
          rcPrintf ((host, " QR)"));
          break;
        case 8:
          rcPrintf ((host, " OR)"));
          break;
      }
      EmptyBlock(host, 1);
    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplaySize


/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
void
DisplayDimmType (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
{
  UINT8 ch;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {
      // Display DIMM type
      switch ((*dimmNvList)[dimm].keyByte) {
        case SPD_TYPE_DDR3:
          rcPrintf ((host, " DDR3 "));
          break;
        case SPD_TYPE_DDR4:
          rcPrintf ((host, " DDR4 "));
          break;
        case SPD_TYPE_AEP:
          rcPrintf ((host, "NVMDIMM"));
          break;
      }
      // Display DIMM type
      switch ((*dimmNvList)[dimm].actKeyByte2) {
        case SPD_UDIMM:
          rcPrintf ((host, "UDIMM  "));
          break;
        case SPD_RDIMM:
          rcPrintf ((host, "RDIMM  "));
          break;
        case SPD_LRDIMM_DDR4:
          rcPrintf ((host, "LRDIMM "));
          break;
        case SPD_NVM_DIMM:
          rcPrintf ((host, "NVDIMM "));
          break;
        case SPD_SODIMM:
          rcPrintf ((host, "SODIMM "));
          break;
        default:
          rcPrintf ((host, "Unknown "));
          break;
      }

      // Display Raw Card
      rcPrintf ((host, "R/C-"));

      switch ((*dimmNvList)[dimm].SPDRawCard & 0x1F) {
        case RAW_CARD_A:
          rcPrintf ((host, "A "));
          break;

        case RAW_CARD_B:
          rcPrintf ((host, "B "));
          break;

        case RAW_CARD_C:
          rcPrintf ((host, "C "));
          break;

        case RAW_CARD_D:
          rcPrintf ((host, "D "));
          break;

        case RAW_CARD_E:
          rcPrintf ((host, "E "));
          break;

        case RAW_CARD_F:
          rcPrintf ((host, "F "));
          break;

        case RAW_CARD_G:
          rcPrintf ((host, "G "));
          break;

        case RAW_CARD_H:
          rcPrintf ((host, "H "));
          break;

        case RAW_CARD_J:
          rcPrintf ((host, "J "));
          break;

        case RAW_CARD_K:
          rcPrintf ((host, "K "));
          break;

        case RAW_CARD_L:
          rcPrintf ((host, "L "));
          break;

        case RAW_CARD_M:
          rcPrintf ((host, "M "));
          break;

        case RAW_CARD_N:
          rcPrintf ((host, "N "));
          break;

        case RAW_CARD_P:
          rcPrintf ((host, "P "));
          break;

        case RAW_CARD_R:
          rcPrintf ((host, "R "));
          break;

        case RAW_CARD_T:
          rcPrintf ((host, "T "));
          break;

        case RAW_CARD_U:
          rcPrintf ((host, "U "));
          break;

        case RAW_CARD_W:
          rcPrintf ((host, "W "));
          break;

        case RAW_CARD_Y:
          rcPrintf ((host, "Y "));
          break;

        case RAW_CARD_AA:
          rcPrintf ((host, "AA"));
          break;

        case RAW_CARD_AB:
          rcPrintf ((host, "AB"));
          break;

        case RAW_CARD_AC:
          rcPrintf ((host, "AC"));
          break;

        case RAW_CARD_AD:
          rcPrintf ((host, "AD"));
          break;

        case RAW_CARD_AE:
          rcPrintf ((host, "AE"));
          break;

        case RAW_CARD_AF:
          rcPrintf ((host, "AF"));
          break;

        case RAW_CARD_AG:
          rcPrintf ((host, "AG"));
          break;

        case RAW_CARD_AH:
          rcPrintf ((host, "AH"));
          break;

        case RAW_CARD_AJ:
          rcPrintf ((host, "AJ"));
          break;

        case RAW_CARD_AK:
          rcPrintf ((host, "AK"));
          break;

        case RAW_CARD_AL:
          rcPrintf ((host, "AL"));
          break;

        default:
          rcPrintf ((host, "? "));
          break;
      }
      EmptyBlock(host, 1);
    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplayDimmType


/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
void
DisplayDramMfgDate (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
{
  UINT8 ch;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {

      //
      // Module date code
      //
      rcPrintf ((host, "     ww%02d ", (UINT8) BCDtoDecimal((UINT8)((*dimmNvList)[dimm].SPDModDate >> 8))));
      rcPrintf ((host, "20%02d", BCDtoDecimal((UINT8)(*dimmNvList)[dimm].SPDModDate)));
      EmptyBlock(host, 6);
    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplayDramMfgDate

/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
void
DisplaySpeed (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
{
  UINT8 ch;
//  UINT8 bclk = 133;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {
      rcPrintf ((host, "   "));
      if ((*dimmNvList)[dimm].minTCK <= DDR_4400_TCK_MIN) {
        rcPrintf ((host, "4400"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_4266_TCK_MIN) {
        rcPrintf ((host, "4266"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_4200_TCK_MIN) {
        rcPrintf ((host, "4200"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_4000_TCK_MIN) {
        rcPrintf ((host, "4000"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3800_TCK_MIN) {
        rcPrintf ((host, "3800"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3733_TCK_MIN) {
        rcPrintf ((host, "3733"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3600_TCK_MIN) {
        rcPrintf ((host, "3600"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3466_TCK_MIN) {
        rcPrintf ((host, "3466"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3400_TCK_MIN) {
        rcPrintf ((host, "3400"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3200_TCK_MIN) {
        rcPrintf ((host, "3200"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_3000_TCK_MIN) {
        rcPrintf ((host, "3000"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2933_TCK_MIN) {
        rcPrintf ((host, "2933"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2800_TCK_MIN) {
        rcPrintf ((host, "2800"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2666_TCK_MIN) {
        rcPrintf ((host, "2666"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2600_TCK_MIN) {
        rcPrintf ((host, "2600"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2400_TCK_MIN) {
        rcPrintf ((host, "2400"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2200_TCK_MIN) {
        rcPrintf ((host, "2200"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2133_TCK_MIN) {
        rcPrintf ((host, "2133"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_2000_TCK_MIN) {
        rcPrintf ((host, "2000"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1866_TCK_MIN) {
        rcPrintf ((host, "1866"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1800_TCK_MIN) {
        rcPrintf ((host, "1800"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1600_TCK_MIN) {
        rcPrintf ((host, "1600"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1400_TCK_MIN) {
        rcPrintf ((host, "1400"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1333_TCK_MIN) {
        rcPrintf ((host, "1333"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1200_TCK_MIN) {
        rcPrintf ((host, "1200"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1066_TCK_MIN) {
        rcPrintf ((host, "1066"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_1000_TCK_MIN) {
        rcPrintf ((host, "1000"));
      } else if ((*dimmNvList)[dimm].minTCK <= DDR_800_TCK_MIN) {
        rcPrintf ((host, " 800"));
      } else {
        rcPrintf ((host, "????"));
      }
      rcPrintf ((host, " "));

      // CAS latency (tCL)
      rcPrintf ((host, "%d-", (UINT8) TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tCL, 0)));

      // Row Address to Column Address Delay (tRCD)
      rcPrintf ((host, "%d-", (UINT8) TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tRCD, 0)));

      // Row Active Time (tRP)
      rcPrintf ((host, "%d", (UINT8) TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tRP, 0)));

      if(TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tCL, 0) < 10) rcPrintf ((host, " "));
      if(TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tRCD, 0) < 10) rcPrintf ((host, " "));
      if(TimeBaseToDCLK(host, socket, ch, dimm, (*dimmNvList)[dimm].tRP, 0) < 10) rcPrintf ((host, " "));
      EmptyBlock(host, 4);

    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplaySpeed


#ifdef MEM_NVDIMM_EN
/**

Displays information about the DIMMs present

@param host:         Pointer to sysHost
@param socket:       Processor socket
@param dimm:         Specific DIMM on this Channel

@retval N/A

**/
void
DisplayNvdimmN(
PSYSHOST  host,
UINT8     socket,
UINT8     dimm
)
{
  UINT8 ch;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {
      switch ((*dimmNvList)[dimm].nvDimmType) {
        case 0:
          EmptyBlock(host, 20);
          break;
        case 1:
          rcPrintf((host, "   JEDEC NVDIMM-N"));
          EmptyBlock(host, 3);
          break;
        default:
          rcPrintf((host,"   Legacy NVDIMM-N"));
          EmptyBlock(host, 2);
          break;
      }
    } else {
      EmptyBlock(host, 20);
    }
  }
}

#endif
/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
void
DisplayModulePartNum (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm
  )
{
  UINT8 i;
  UINT8 ch;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {

      //
      // Module part number
      //
      if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
        for (i = 0; i < SPD_MODULE_PART; i++) {
          if((*dimmNvList)[dimm].SPDModPart[i]){
            rcPrintf ((host, "%c", (*dimmNvList)[dimm].SPDModPart[i]));
          } else {
            rcPrintf ((host, " "));
          }
        }
        rcPrintf ((host, " |"));
      } else {
        for (i = 0; i < SPD_MODULE_PART_DDR4; i++) {
          if((*dimmNvList)[dimm].SPDModPartDDR4[i]){
            rcPrintf ((host, "%c", (*dimmNvList)[dimm].SPDModPartDDR4[i]));
          } else {
            rcPrintf ((host, " "));
          }
        }
        EmptyBlock(host, 0);
      }
    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplayModulePartNum




void
DisplayDDRFreq (
  PSYSHOST  host,
  UINT8     socket
  )
/**

  Displays information about the device being refered to

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

**/
{
  if (checkMsgLevel(host, SDBG_MINMAX)) {
    rcPrintf ((host, "DDR-"));
    switch (host->nvram.mem.socket[socket].ddrFreq) {
      case DDR_800:
        rcPrintf ((host, "800\n"));
        break;

      case DDR_1000:
        rcPrintf ((host, "1000\n"));
        break;

      case DDR_1066:
        rcPrintf ((host, "1066\n"));
        break;

      case DDR_1200:
        rcPrintf ((host, "1200\n"));
        break;

      case DDR_1333:
        rcPrintf ((host, "1333\n"));
        break;

      case DDR_1400:
        rcPrintf ((host, "1400\n"));
        break;

      case DDR_1600:
        rcPrintf ((host, "1600\n"));
        break;

      case DDR_1800:
        rcPrintf ((host, "1800\n"));
        break;

      case DDR_1866:
        rcPrintf ((host, "1866\n"));
        break;

      case DDR_2000:
        rcPrintf ((host, "2000\n"));
        break;

      case DDR_2133:
        rcPrintf ((host, "2133\n"));
        break;

      case DDR_2200:
        rcPrintf ((host, "2200\n"));
        break;

      case DDR_2400:
        rcPrintf ((host, "2400\n"));
        break;

      case DDR_2600:
        rcPrintf ((host, "2600\n"));
        break;

      case DDR_2666:
        rcPrintf ((host, "2666\n"));
        break;

      case DDR_2800:
        rcPrintf ((host, "2800\n"));
        break;

      case DDR_2933:
        rcPrintf ((host, "2933\n"));
        break;

      case DDR_3000:
        rcPrintf ((host, "3000\n"));
        break;

      case DDR_3200:
        rcPrintf ((host, "3200\n"));
        break;

      case DDR_3400:
        rcPrintf ((host, "3400\n"));
        break;

      case DDR_3466:
        rcPrintf ((host, "3466\n"));
        break;

      case DDR_3600:
        rcPrintf ((host, "3600\n"));
        break;

      case DDR_3733:
        rcPrintf ((host, "3733n"));
        break;

      case DDR_3800:
        rcPrintf ((host, "3800\n"));
        break;

      case DDR_4000:
        rcPrintf ((host, "4000\n"));
        break;

      case DDR_4200:
        rcPrintf ((host, "4200\n"));
        break;

      case DDR_4266:
        rcPrintf ((host, "4266\n"));
        break;

      case DDR_4400:
        rcPrintf ((host, "4400\n"));
        break;

    } // switch
  }
} // DisplayDDRFreq


#ifdef  SERIAL_DBG_MSG
void
DisplayIntelSN (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     dimm
               )
/**

  Displays information about the DIMMs present

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param dimm:         Specific DIMM on this Channel

  @retval N/A

**/
{
  UINT8   i;
  UINT8   ch;
  UINT32  serialNum;
  UINT32  hiDwordSerialNum;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent) {

      serialNum = 0;

      if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
        //
        // Intel DIMM Serial Number
        //
        for (i = 0; i < 3; i++) {
          serialNum |= (UINT32)(*dimmNvList)[dimm].SPDIntelSN[i] << ((2 - i) * 8);
        }

        rcPrintf ((host, "    %8d", serialNum));
        EmptyBlock(host, 6);
      } else {
        hiDwordSerialNum = 0;
        //
        // Intel DIMM Serial Number
        //
        hiDwordSerialNum = (UINT32)(*dimmNvList)[dimm].SPDIntelSN[0];
        for (i = 1; i < 5; i++) {
          serialNum |= (UINT32)(*dimmNvList)[dimm].SPDIntelSN[i] << ((4 - i) * 8);
        }
        rcPrintf ((host, "0x%08x%08x", hiDwordSerialNum, serialNum));
        EmptyBlock(host, 2);
      }
    } else {
      EmptyBlock(host, 20);
    }
  } // ch loop
} // DisplayIntelSN
#endif  // SERIAL_DBG_MSG


void
PrintLine (
  PSYSHOST  host,
  UINT16    count,
  UINT8     doubleLine,
  UINT8     crLfBefore,
  UINT8     crLfAfter
  )
/**

  Prints a double line

  @param host:         Pointer to sysHost
  @param count:        Number of units in the line
  @param doubleLine:   0 prints '-'   1 prints '='
  @param crLfBefore:   1 = return before printing; 0 = ignore
  @param crLfAfter :   1 = return after printing; 0 = ignore

  @retval N/A

**/
{
  UINT16 loops;

  if (crLfBefore) rcPrintf ((host, "\n"));

  for (loops = 0; loops < count; loops++) {
    if (doubleLine) {
      rcPrintf ((host, "="));
    } else {
      rcPrintf ((host, "-"));
    }
  } // loops

  if (crLfAfter) rcPrintf ((host, "\n"));
} // PrintLine


void
EmptyBlock (
  PSYSHOST  host,
  UINT8     numSpaces
  )
/**

  Displays information about the device being refered to

  @param host:         Pointer to sysHost
  @param numSpaces:    Number of spaces for the empty block

  @retval N/A

**/
{
  UINT8 spaces;

  for (spaces = 0; spaces < numSpaces; spaces++) {
    rcPrintf ((host, " "));
  }
  rcPrintf ((host, "|"));
} // EmptyBlock


void
DisplayDev (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit
  )
/**

  Displays information about the device being refered to

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param ch:           Memory Channel
  @param dimm:         Specific DIMM on this Channel
  @param rank:         Rank on this DIMM
  @param strobe        Strobe
  @param bit:          Bit

  @retval N/A

**/
{
  UINT8 colon = 0;
  if (socket != NO_SOCKET) {
    if (host->setup.mem.logParsing == LOG_PARSING_EN) {
      rcPrintf ((host, "%4x.", (UINT16) (host->var.common.checkpoint >> 16)));
    }
    rcPrintf ((host, "N%d", socket));
    colon = 1;
  }

  if (ch != NO_CH) {
    if (colon) rcPrintf ((host, "."));
    rcPrintf ((host, "C%d", ch));
    colon = 1;
  }

  if (dimm != NO_DIMM) {
    if (colon) rcPrintf ((host, "."));
    rcPrintf ((host, "D%d", dimm));
    colon = 1;
  }

  if (rank != NO_RANK) {
    if (colon) rcPrintf ((host, "."));
    rcPrintf ((host, "R%d", rank));
    colon = 1;
  }

  if (strobe != NO_STROBE) {
    if (colon) rcPrintf ((host, "."));
    rcPrintf ((host, "S%02d", strobe));
    colon = 1;
  }

  if (bit != NO_BIT) {
    if (colon) rcPrintf ((host, "."));
    rcPrintf ((host, "B%02d", bit));
    colon = 1;
  }

  if (colon) {
    rcPrintf ((host, ": "));
  }
} // DisplayDev
#endif //SERIAL_DBG_MSG


UINT8
BCDtoDecimal (
  UINT8 BcdValue
  )
/**

  Function converts a BCD to a decimal value.

  @param BcdValue         - An 8 bit BCD value

  @retval UINT8            - Returns the decimal value of the BcdValue

**/
{
  UINT8 High;
  UINT8 Low;

  High  = BcdValue >> 4;
  Low   = BcdValue - (High << 4);

  return (UINT8) (Low + (High * 10));
}


#ifdef SERIAL_DBG_MSG
#define VREF_POINTS   101
void
EyeDiagram (
           PSYSHOST        host,
           UINT8           socket,
           GSM_LT          level,
           GSM_GT          group,
           UINT16          mode,
           UINT8           scope,
           UINT32          patternLength,
           UINT8           chMask,
           UINT8           rankMask,
           UINT8           dataMask[MAX_STROBE/2]
           )
/**
  Calculates positive and negative margin for the group provided

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check
  @param level:        DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group:        Parameter to be margined
                         RxDqsDelay    : Margin Rx DqDqs
                         TxDqDelay     : Margin Tx DqDqs
                         RxVref         : Margin Rx Vref
                         TxVref         : Margin Tx Vref
                         RecEnDelay    : Margin Receive Enable
                         WrLvlDelay    : Margin Write Level
                         CmdGrp0        : Margin CMD group 0
                         CmdGrp1        : Margin CMD group 1
                         CmdGrp2        : Margin CMD group 2
                         CmdAll         : Margin all CMD groups
  @param mode:         Test mode to use
                         MODE_XTALK            BIT0: Enable crosstalk (placeholder)
                         MODE_VIC_AGG          BIT1: Enable victim/aggressor
                         MODE_START_ZERO       BIT2: Start at margin 0
                         MODE_POS_ONLY         BIT3: Margin only the positive side
                         MODE_NEG_ONLY         BIT4: Margin only the negative side
                         MODE_DATA_MPR         BIT5: Enable MPR Data pattern
                         MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
                         MODE_DATA_LFSR        BIT7: Enable Data LFSR
                         MODE_ADDR_LFSR        BIT8: Enable Address LFSR
                         MODE_ADDR_CMD0_SETUP  BIT9
                         MODE_CHECK_PARITY     BIT10: Enable parity checking
                         MODE_DESELECT         BIT11: Enable deselect patterns
                         MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns
  @param scope:        Margin granularity
                         SCOPE_SOCKET      0: Margin per processor socket
                         SCOPE_CH        1: Margin per channel
                         SCOPE_DIMM      2: Margin per DIMM
                         SCOPE_RANK      3: Margin per rank
                         SCOPE_STROBE    4: Margin per strobe group
                         SCOPE_BIT       5: Margin per bit

**/
{
  UINT8               ch;
  UINT8               rank;
  UINT8               strobe;
  INT32               vIndex;
  UINT8               maxVrefOffset = 0;
  UINT64_STRUCT       startTsc  = { 0, 0};
  UINT64_STRUCT       endTsc    = { 0, 0};
  UINT32              mrcLatency = 0;
  struct bitMask      filter;
  struct rankMargin   results[VREF_POINTS];

  if (level == LrbufLevel) {
    rcPrintf ((host, "\nBackside "));
  } else {
    rcPrintf ((host, "\n"));
  }

  if (group == RxDqsDelay) {
    rcPrintf ((host, "Read Eye Diagrams\n"));
    if (level == LrbufLevel) {
      maxVrefOffset = 45;
    } else {
      maxVrefOffset = MAX_RD_VREF_OFFSET;
    }
  } else if (group == RxDqsPDelay) {
    rcPrintf ((host, "Read Eye(P) Diagrams\n"));
    maxVrefOffset = MAX_RD_VREF_OFFSET;
  } else if (group == RxDqsNDelay) {
    rcPrintf ((host, "Read Eye(N) Diagrams\n"));
    maxVrefOffset = MAX_RD_VREF_OFFSET;
  } else if (group == TxDqDelay) {
    rcPrintf ((host, "Write Eye Diagrams\n"));
    if (level == LrbufLevel) {
      maxVrefOffset = 50;
    } else {
      maxVrefOffset = MAX_WR_VREF_OFFSET_BACKSIDE;
    }
  } else if (group == CmdAll) {
    rcPrintf ((host, "Cmd Eye Diagrams\n"));
    if (level == LrbufLevel) {
      maxVrefOffset = 20;
    }
    else {
      maxVrefOffset = 50;
    }
  }
  if (checkMsgLevel(host, SDBG_MINMAX)) {
    //
    // Save initial timestamp for latency measurements
    //
    ReadTsc64 (&startTsc);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    for (rank = 0; rank < MAX_RANK_CH; rank++) {
      //
      // Set all filter bits
      //
      for (strobe = 0; strobe < MAX_STROBE/2; strobe++) {
        filter.bits[ch][rank][strobe] = dataMask[strobe];
      } // strobe loop

      //
      // Clear margin data
      //
      for (vIndex = 0; vIndex < VREF_POINTS; vIndex++) {
        results[vIndex].rank[ch][rank].n = 0;
        results[vIndex].rank[ch][rank].p = 0;
      } // vIndex
    } // rank loop
  } // ch loop

  GetMultiVref (host, socket, level, group, mode, scope, &filter, (void *)results, patternLength, 0, 0, 1, (maxVrefOffset * 2) + 1);

  if (group == RxDqsDelay) {
    rcPrintf ((host, "\nRead Eye\n"));
  } else if (group == RxDqsPDelay) {
    rcPrintf ((host, "\nRead (P) Eye\n"));
  } else if (group == RxDqsNDelay) {
    rcPrintf ((host, "\nRead (N) Eye\n"));
  } else if (group == TxDqDelay) {
    rcPrintf ((host, "\nWrite Eye\n"));
  } else if (group == CmdAll) {
    rcPrintf ((host, "\nCmd Eye\n"));
  }
  DisplayEye(host, socket, scope, level, (void *)results, maxVrefOffset);

  ReadTsc64 (&endTsc);
  mrcLatency = TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS);
  rcPrintf ((host, "\nDisplayEye time            %d ms\n", mrcLatency));
} // EyeDiagram

void
DisplayEye (
           PSYSHOST        host,
           UINT8           socket,
           UINT8           scope,
           GSM_LT          level,
           void            *results,
           UINT8           maxVrefOffset
           )
/**
  Calculates positive and negative margin for the group provided

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param scope
  @param void:         void pointer for the results
  @param maxVrefOffset

  @retval N/A

**/
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               strobeStart = 0;
  UINT8               maxStrobe = 0;
  UINT8               bit;
  UINT8               bitStart = 0;
  UINT8               maxBit = 0;
  INT16               tmpMargin;
  INT16               margin;
  INT32               vIndex;
  INT16                neg = 0;
  INT16               pos = 0;
  struct rankMargin   (*rankM)[63];
  struct strobeMargin (*strobeM)[63];
  struct bitMargin    (*bitM)[63];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  if (scope == SCOPE_RANK) {
    strobeStart = 0;
    maxStrobe = 1;
    bitStart = 0;
    maxBit = 1;
  } else if (scope == SCOPE_STROBE) {
    strobeStart = 0;
    maxStrobe = MAX_STROBE;
    bitStart = 0;
    maxBit = 1;
  } else if (scope == SCOPE_BIT) {
    strobeStart = 0;
    maxStrobe = MAX_STROBE;
    bitStart = 0;
    maxBit = 72;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    // Check if this channel is masked off

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList = GetRankNvList(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
      getPrintFControl (host);
#endif // SERIAL_DBG_MSG

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for (strobe = strobeStart; strobe < maxStrobe; strobe++) {
          for (bit = bitStart; bit < maxBit; bit++) {

            DisplayDev (host, socket, ch, dimm, rank, strobe, bit);
            rcPrintf ((host, "\n   3322222222221111111111                   1111111111222222222233\n"));
            rcPrintf ((host, "   109876543210987654321098765432101234567890123456789012345678901\n"));
            for (vIndex = (maxVrefOffset * 2) + 1; vIndex >= 0; vIndex--) {
              if ((level == LrbufLevel) && ((vIndex % 2) == 0)) {
                continue;
              }
              if (scope == SCOPE_RANK) {
                rankM = results;
                if ((*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].n >
                    (*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].p) {

                  (*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                  (*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].p = 0;
                }
                neg = (*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].n;
                pos = (*rankM)[vIndex].rank[ch][(*rankList)[rank].rankIndex].p;
              } else if (scope == SCOPE_STROBE) {
                strobeM = results;
                if ((*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].n >
                    (*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].p) {

                  (*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
                  (*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
                }
                neg = (*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].n;
                pos = (*strobeM)[vIndex].strobe[ch][(*rankList)[rank].rankIndex][strobe].p;
              } else if (scope == SCOPE_BIT) {
                bitM = results;
                if ((*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].n >
                    (*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].p) {

                  (*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].n = 0;
                  (*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].p = 0;
                }
                neg = (*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].n;
                pos = (*bitM)[vIndex].bits[ch][(*rankList)[rank].rankIndex][bit].p;
              }
              if ((neg == 0) && (pos == 0)) continue;
              if ((neg > MAX_TIMING_OFFSET) && (pos < -MAX_TIMING_OFFSET)) continue;
              rcPrintf ((host, "%3d ", vIndex - maxVrefOffset));
              //
              // Calculate the negative margin
              //
              tmpMargin = neg;
              for (margin = -MAX_TIMING_OFFSET; margin <= MAX_TIMING_OFFSET; margin++) {
                if (margin == tmpMargin) {
                  rcPrintf ((host, "#"));
                  //
                  // Calculate the positive margin
                  //
                  tmpMargin = pos;
                } else if ((vIndex == maxVrefOffset) && (margin == 0)) {
                  rcPrintf ((host, "+"));
                } else {
                  rcPrintf ((host, " "));
                }
              }
              rcPrintf ((host, "\n"));
            } // vIndex loop

          } // bit loop
        } // strobe loop
      } // rank loop
      releasePrintFControl (host);
    } // dimm loop
  } // ch loop
} // DisplayEye
#endif  // SERIAL_DBG_MSG


#ifdef SERIAL_DBG_MSG
void
DisplayTxDqDqsBitResults(
  PSYSHOST  host,
  UINT8     socket
)
/*++

  Displays the training results for Wr Dq/Dqs

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               bit;
  INT16               delay;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "START_DATA_TX_DQ_BIT\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          for (bit = 0; bit < 4; bit++) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqBitDelay, GSM_READ_ONLY, &delay));
            MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, strobe, bit,
                          "TxDq: Pi = %d\n", delay));
          } // bit loop
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "STOP_DATA_TX_DQ\n"));
}

void
DisplayMRS(
  PSYSHOST  host,
  UINT8     socket
)
/*++
  Displays the mode register settings

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  struct ddrRank(*rankList)[MAX_RANK_DIMM];
  UINT16              MR6;

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_MRS\n"));

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "               MR0  MR1  MR2"));
  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {

    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  MR3  MR4  MR5  MR6"));
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %2d ", strobe));
    } // strobe loop
  } // if DDR4/NVMDIMM

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = &(*dimmNvList)[dimm].rankList;
      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      if((host->nvram.mem.socket[socket].lrDimmPresent)) {
        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, TxVref, LrbufLevel));
      } else {
        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, TxVref, DdrLevel));
      }

      for (rank = 0; rank < (*dimmNvList)[dimm].numDramRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                      ""));
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "%5x%5x%5x", (*rankStruct)[rank].MR0, (*rankStruct)[rank].MR1,  (*rankStruct)[rank].MR2));
        if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
          MR6 = GetVrefRange(host, (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex]);
          MR6 |= (((GetTccd_L(host, socket, ch) - 4) & 7) << 10);

          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "%5x%5x%5x%5x", (*rankStruct)[rank].MR3, (*rankStruct)[rank].MR4,  (*rankStruct)[rank].MR5, MR6));

          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "%5x", (*rankStruct)[rank].MR6[strobe]));
          } // strobe loop
        } // if DDR4/NVMDIMM
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
      } // rank loop
    } // dimm loop
  } // ch loop
} // DisplayMRS
#endif  // SERIAL_DBG_MSG

void
DisplayResults(
  PSYSHOST  host,
  UINT8     socket,
  GSM_GT    group
)
/*++
  Displays the training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param group:        GSM_GT group

  @retval N/A

--*/
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               bit;
  UINT8               b;
  UINT8               maxBits;
  UINT8               encodedNibble;
  INT16               delay;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct RankCh       (*rankPerCh)[MAX_RANK_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_"));
  if(group == RxDqsDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQS\n"));
  } else if(group == RxDqDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQ\n"));
  } else if(group == RxVref) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RxVref\n"));
   } else if(group == RxDqDqsDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQ_DQS\n"));
  } else if(group == RxDqsPDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQSP\n"));
  } else if(group == RxDqsNDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQSN\n"));
  } else if(group == RecEnDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "REC_EN\n"));
  } else if(group == TxDqsDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "WR_LVL\n"));
  } else if(group == TxDqDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "TX_DQ\n"));
  } else if(group == TxVref) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "TxVref\n"));
  } else if(group == TxEq) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "TxEq\n"));
  } else if(group == RxDqsBitDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQS_PER_BIT\n"));
 } else if(group == RxDqsPBitDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQS_P_PER_BIT\n"));
 } else if(group == RxDqsNBitDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "RX_DQS_N_PER_BIT\n"));
  } else if(group == TxDqBitDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "TX_DQ_PER_BIT\n"));
  }

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "              "));

  if (host->nvram.mem.eccEn) {
    maxBits = MAX_BITS;
  } else {
    maxBits = MAX_BITS - 8;
  }

  if ((group != RxDqsBitDelay) && (group != TxDqBitDelay) && (group != RxVref) && (group != RxDqsPBitDelay) && (group != RxDqsNBitDelay)) {
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %2d ", strobe));
    } // strobe loop
  } // if not per bit
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

#ifdef SERIAL_DBG_MSG
    CHIP_FUNC_CALL(host, DisplayTcwlAdj(host, socket, ch, group));
#endif //SERIAL_DBG_MSG

    dimmNvList = GetDimmNvList(host, socket, ch);
    rankPerCh = &(*channelNvList)[ch].rankPerCh;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group, DdrLevel));

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        if ((group == RxVref) && (dimm > 0) | (rank > 0)) continue;
        if ((group == TxVref) && CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& (rank > 0)) continue;

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                      ""));

        if ((group == RxDqsBitDelay) || (group == RxDqsPBitDelay) ||(group == RxDqsNBitDelay) || (group == TxDqBitDelay) || (group == RxVref)) {
          for (bit = 0; bit < maxBits; bit++) {
            if ((bit/4)%2){
              encodedNibble = bit/8 + 9;
            } else {
              encodedNibble = bit/8;
            }
            if ((bit == 0) || (bit == (maxBits / 2))) {
              MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "\n"));
              if (bit == (maxBits / 2)){
                MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "\n"));
              }
              for(b = bit; b < (bit + (maxBits / 2)); b++) {
                MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "  %2d", b));
              }
              MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "\n"));
            }

              // call SetGDCR to update the appropriate register field
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, (bit%4), DdrLevel, group, GSM_READ_ONLY,
                               &delay));
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        " %3d", delay));

          } // bit loop
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));

        } else {
          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, group, GSM_READ_ONLY, &delay));
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "%5d", delay));
          } // strobe loop

          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\n"));

          if (group == RecEnDelay) {
            MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          "IO Latency = %d, Round Trip = %d\n", (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency,
                           (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
          }
        } // if per bit
      } // rank loop
    } // dimm loop
  } // ch loop
} // DisplayResults

#ifdef SERIAL_DBG_MSG
void
DisplayBwSerr(
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank,
             UINT32    bwSerr[3],
             INT16     margin
             )
/**
  Displays the bitwise error status

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param ch:           Memory Channel
  @param bwSerr[3]     BitWise error status

  @retval N/A

**/
{
  UINT8   bit;
  UINT8   maxBits;
  UINT8   bitIndex;
  UINT32  errResult = 0;

  getPrintFControl (host);

  if (host->nvram.mem.eccEn) {
    maxBits = MAX_BITS;
  } else {
    maxBits = MAX_BITS - 8;
  }

  DisplayDev (host, socket, ch, dimm, rank, NO_STROBE, NO_BIT);
  rcPrintf ((host, "%3d", margin));

  for (bit = 0; bit < maxBits; bit++) {
    if ((bit % 8) == 0) rcPrintf ((host, " "));

    switch (bit) {
    case 0:
      errResult = bwSerr[0];
      break;

    case 32:
      errResult = bwSerr[1];
      break;

    case 64:
      errResult = bwSerr[2];
      break;
    } // switch

    bitIndex = bit % 32;

    if (errResult & (1 << bitIndex)) {
      rcPrintf ((host, "#"));
    } else {
      rcPrintf ((host, "."));
    }

  } // bit loop

  rcPrintf ((host, "\n"));

  releasePrintFControl (host);
} // DisplayBwSerr
#endif  // SERIAL_DBG_MSG

#ifdef SERIAL_DBG_MSG
void
DisplayRxVrefResults(
  PSYSHOST  host,
  UINT8     socket
)
/*++
  Displays the training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A

--*/
{
#ifdef DISPLAY_RX_VREF_SUPPORT
  UINT8               ch;
  UINT8               strobe;
  UINT8               maxStrobe;
  INT16               vref;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, 0, 0, RxVref, DdrLevel));

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_RX_VREF\n"));

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "       "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
    //
    // Skip if this is an ECC strobe when ECC is disabled
    //
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3d ", strobe));
  } // strobe loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  ""));
    for (strobe = 0; strobe < maxStrobe; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, 0, 0, strobe, ALL_BITS, DdrLevel, RxVref, GSM_READ_ONLY, &vref));
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", vref));
    } // strobe loop

    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));
  } // ch loop
#endif // DISPLAY_RX_VREF_SUPPORT
} // DisplayRxVrefResults

#ifdef LRDIMM_SUPPORT
/**
  Displays the LRDIMM backside training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param group:        Signal group
  @param level:        IO level to access

  @retval N/A
**/

void
DisplayDdr4LrdimmTrainingResults(
  PSYSHOST  host,
  UINT8     socket,
  GSM_GT    group
)
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  INT16               offset;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  if (group == RxVref) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_RXVREF_TRAINING_RESULTS\n"));
  } else if (group == RecEnDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_RX_RCVEN_TRAINING_RESULTS\n"));
    } else if (group == RecEnDelayCycle) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_RX_CYCLE_TRAINING_RESULTS\n"));
  } else if (group == RxDqsDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_RX_DQSDELAY_TRAINING_RESULTS\n"));
  } else if (group == TxDqsDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_WRITE_LEVELING_TRAINING_RESULTS\n"));
  } else if (group == TxDqsDelayCycle) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_WRITE_CYCLE_TRAINING_RESULTS\n"));
  } else if (group == TxDqDelay) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nLRDIMM_BACKSIDE_TX_DQDELAY_TRAINING_RESULTS\n"));
  } else if (group == TxVref) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\nLRDIMM_BACKSIDE_TXVREF_TRAINING_RESULTS\n"));
  }

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "             "));
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "  %3d", strobe));
  }
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n"));
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].lrDimmPresent == 0) continue;
      maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group, LrbufLevel));

      for (rank = 0; rank < (*dimmNvList)[dimm].numDramRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT, ""));
        for (strobe = 0; strobe < maxStrobe; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, ALL_BITS, LrbufLevel, group,
                             GSM_READ_ONLY, &offset));

          // subtract 16 so values are -15 to +15 instead of 0 to 31
          if ((group == RxDqsDelay) || (group == TxDqDelay)){
            offset -= 15;
          }
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "  %3d",  offset));
        } // strobe loop
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "\n"));
      } // rank loop
    } // dimm loop
  } // ch loop
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));
} //DisplayDdr4LrdimmTrainingResults
#endif // LRDIMM_SUPPORT

/**
  Displays the training results for all parameters

  @param host:         Pointer to sysHost

  @retval N/A
**/
UINT32
DisplayTrainResults(
                   PSYSHOST  host
                   )
{
  UINT8  socket;
  UINT8  serialDebugMsgLvlsave = 0;

  if (host->setup.common.debugJumper && (host->var.mem.serialDebugMsgLvl == SDBG_MIN)) {
    serialDebugMsgLvlsave = host->var.mem.serialDebugMsgLvl;
    host->var.mem.serialDebugMsgLvl = SDBG_MAX;
  }

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  getPrintFControl (host);

  MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "START_TRAINING_REGISTER_DUMP\n"));

  CHIP_FUNC_CALL(host, DisplayXoverResults(host, socket));
  if (host->setup.mem.enableNgnBcomMargining) {
    CHIP_FUNC_CALL(host, DisplayXoverResultsFnv(host, socket));
  }
  DisplayResults(host, socket, RecEnDelay);

  if (host->setup.mem.optionsExt & DUTY_CYCLE_EN) {
    DisplayResults(host, socket, RxDqsPDelay);
    DisplayResults(host, socket, RxDqsNDelay);
  } else {
    DisplayResults(host, socket, RxDqsDelay);
  }
  DisplayResults(host, socket, RxDqDelay);
  DisplayResults(host, socket, TxDqsDelay);
  DisplayResults(host, socket, TxDqDelay);
  DisplayRxVrefResults(host, socket);
  DisplayResults(host, socket, TxVref);
  CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  DisplayMRS(host, socket);
  if (DESKEW_DELAY == RxDqsBitDelay) {
    DisplayResults(host, socket, RxDqsBitDelay);
  } else {
    DisplayResults(host, socket, RxDqsPBitDelay);
    DisplayResults(host, socket, RxDqsNBitDelay);
  }
  DisplayResults(host, socket, TxDqBitDelay);

#ifdef LRDIMM_SUPPORT
  if ((host->nvram.mem.dramType == SPD_TYPE_DDR4) && (host->nvram.mem.socket[socket].lrDimmPresent)){
    DisplayDdr4LrdimmTrainingResults(host, socket, RecEnDelay);
    DisplayDdr4LrdimmTrainingResults(host, socket, RecEnDelayCycle);
    DisplayDdr4LrdimmTrainingResults(host, socket, RxDqsDelay);
    DisplayDdr4LrdimmTrainingResults(host, socket, RxVref);
    DisplayDdr4LrdimmTrainingResults(host, socket, TxDqsDelay);
    DisplayDdr4LrdimmTrainingResults(host, socket, TxDqsDelayCycle);
    DisplayDdr4LrdimmTrainingResults(host, socket, TxDqDelay);
    DisplayDdr4LrdimmTrainingResults(host, socket, TxVref);
  }
#endif //LRDIMM_SUPPORT

  CHIP_FUNC_CALL(host, DisplayResultsChip(host, socket));


  MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "STOP_TRAINING_REGISTER_DUMP\n"));

  releasePrintFControl (host);

  if (host->setup.common.debugJumper && (serialDebugMsgLvlsave == SDBG_MIN)) {
    host->var.mem.serialDebugMsgLvl = serialDebugMsgLvlsave ;
  }

  return SUCCESS;
} // DisplayTrainResults

VOID
PrintDimmConfig(PSYSHOST host)
/**

  This routine prints out memory configuration of each socket at channel level.

  @param host  - Pointer to sysHost


  @retval VOID

**/
{
  UINT8 sckt, i;

  //Display memory sizes in serial port for debugging.
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tPlatform DIMM Configuration(num_chunks(chunk_size))\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  for(sckt=0; sckt<MAX_SOCKET; sckt++){
    if (host->nvram.mem.socket[sckt].enabled == 0)
      continue;
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Socket  : %d\n", sckt));
    for(i=0; i<MAX_CH; i++){
      if (host->var.mem.dfxMemVars.dfxDimmManagement == DIMM_MGMT_BIOS_SETUP) {
#ifdef NVMEM_FEATURE_EN
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "\tChannel   : %d  DDR4 Size  : %d(64MB)  NVMDIMM volatile Size : %d(64MB)  "
          "NVMDIMM persistent Size : %d(64MB)  NVMDIMM block Size : %d(64MB), "
          "NVmemSize (Type01 or batterybacked)  : %d(64MB)\n\n", 
          i, host->var.mem.socket[sckt].channelList[i].memSize, host->var.mem.socket[sckt].channelList[i].volSize, 
          host->var.mem.socket[sckt].channelList[i].perSize, host->var.mem.socket[sckt].channelList[i].blkSize, 
          host->var.mem.socket[sckt].channelList[i].NVmemSize));
#else
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "\tChannel   : %d  DDR4 Size  : %d(64MB)  NVMDIMM volatile Size : %d(64MB)  "
          "NVMDIMM persistent Size : %d(64MB)  NVMDIMM block Size : %d(64MB)\n\n",
          i, host->var.mem.socket[sckt].channelList[i].memSize, host->var.mem.socket[sckt].channelList[i].volSize, 
          host->var.mem.socket[sckt].channelList[i].perSize, host->var.mem.socket[sckt].channelList[i].blkSize));
#endif
      } else {
#ifdef NVMEM_FEATURE_EN
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "\tChannel   : %d  DDR4 Size  : %d(64MB)  NVMDIMM volatile Size : %d(64MB)  "
          "NVMDIMM non volatile Size : %d(64MB)  NVmemSize (Type01 or batterybacked)  : %d(64MB)\n\n", 
          i, host->var.mem.socket[sckt].channelList[i].memSize, host->var.mem.socket[sckt].channelList[i].volSize, 
          host->var.mem.socket[sckt].channelList[i].NonVolSize, host->var.mem.socket[sckt].channelList[i].NVmemSize));
#else
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "\tChannel   : %d  DDR4 Size  : %d(64MB)  NVMDIMM volatile Size : %d(64MB)  "
          "NVMDIMM non volatile Size : %d(64MB) \n\n", 
          i, host->var.mem.socket[sckt].channelList[i].memSize, host->var.mem.socket[sckt].channelList[i].volSize, 
          host->var.mem.socket[sckt].channelList[i].NonVolSize));
#endif
      }
    }
  }
}
//UINT32
//PrintDimmConfig(PSYSHOST host)
/**

  This routine prints out memory configuration of each socket at channel level.

  @param host  - Pointer to sysHost


  @retval SUCCESS

**/
//{
//  UINT8 sckt, i;

  //Display memory sizes in serial port for debugging.
//  rcPrintf ((host, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
//  rcPrintf ((host, "\t\tPlatform DIMM Configuration\n"));
//  rcPrintf ((host, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
//  for(sckt=0; sckt<MAX_SOCKET; sckt++){
//    if (host->nvram.mem.socket[sckt].enabled == 0)
//      continue;
//    rcPrintf ((host, "Socket  : %d\n", sckt));
//    for(i=0; i<MAX_CH; i++){
//      rcPrintf ((host, "\tChannel   : %d  ddr4Size  : %d  volSize : %d  perSize : %d  blkSize : %d\n", i, host->var.mem.socket[sckt].channelList[i].memSize, host->var.mem.socket[sckt].channelList[i].volSize, host->var.mem.socket[sckt].channelList[i].perSize, host->var.mem.socket[sckt].channelList[i].blkSize));
//    }
//  }
//  return SUCCESS;
//}


/**
  Displays the Power/Margin Training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket
  @param group:        Signal group

  @retval N/A
**/
void
DisplayPowerTraingResults(
  PSYSHOST          host,
  UINT8             socket,
  GSM_GT            group
)
{
  struct channelNvram        (*channelNvList)[MAX_CH];
  struct dimmNvram           (*dimmNvList)[MAX_DIMM];
  UINT8                      ch;
  UINT8                      dimm;
  UINT8                      rank;
  UINT8                      strobe;
  UINT8                      paramIndex;
  GSM_GT                     param[MAX_PARAMS];
  UINT8                      numParams = 0;
  INT16                      optimalValue;
  UINT8                      maxStrobes = 0;

  switch (group) {
    case TxImode:
      param[0]         = TxImode;
      numParams        = 1;
      break;

    case RxEq:
    case RxCtleC:
    case RxCtleR:
      param[0]          = RxEq;
      param[1]          = RxCtleC;
      param[2]          = RxCtleR;
      numParams         = 3;
      break;

    case DramDrvStr:
      param[0]         = DramDrvStr;
      numParams        = 1;
      break;

    case TxRon:
      param[0]      = TxRon;
      numParams     = 1;
      break;

    case WrOdt:
      param[0]         = WrOdt;
      numParams        = 1;
      break;

    case RxOdt:
    case ParkOdt:
    case NomOdt:
      param[0]        = RxOdt;
      param[1]        = ParkOdt;
      param[2]        = NomOdt;
      numParams       = 3;
      break;

    case TxEq:
      param[0]         = TxEq;
      numParams        = 1;
      break;

    case TxTco:
      param[0]         = TxTco;
      numParams        = 1;
      break;
    default:
      break;
  }

  channelNvList = GetChannelNvList(host, socket);

  for (paramIndex = 0; paramIndex < numParams; paramIndex++) {
    switch (param[paramIndex]){
      case RxOdt:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "START_DATA_RX_ODT\n"));
        break;
      case TxRon:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "START_DATA_TX_RON\n"));
        break;
      case DramDrvStr:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_DRAM_DRVSTR\n"));
        break;
      case WrOdt:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_WR_ODT\n"));
        break;
      case NomOdt:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_NOM_ODT\n"));
        break;
      case ParkOdt:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_PARK_ODT\n"));
        break;
      case TxImode:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_TX_IMODE\n"));
        break;
      case RxEq:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_RX_EQ\n"));
        break;
      case RxCtleC:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_RX_CTLE_C\n"));
        break;
      case RxCtleR:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_RX_CTLE_R\n"));
        break;
      case TxEq:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_TX_EQ\n"));
        break;
      case TxTco:
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "START_DATA_TX_TCO\n"));
        break;
      default:
        break;
    }

    if ((param[paramIndex] == TxEq) || (param[paramIndex] == TxImode) || (param[paramIndex] == RxEq) ||
          (param[paramIndex] == RxCtleC) || (param[paramIndex] == RxCtleR)) {

      maxStrobes = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, 0, 0, param[paramIndex], DdrLevel));

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "             "));
      for (strobe = 0; strobe < maxStrobes; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "  %3d", strobe));
      }
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
               "\n"));
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      switch (param[paramIndex]) {
      case RxOdt:
      case TxRon:
      case TxTco:
        CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_ONLY, &optimalValue));
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "%3d\n", optimalValue));
        continue;
      default:
        break;
      }
      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        //
        // Skip if no DIMM present
        //
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        maxStrobes = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, param[paramIndex], DdrLevel));

        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          //
          // Skip if no rank
          //
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
#ifdef LRDIMM_SUPPORT
          if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {
            if (rank != 0) continue;
          }
#endif
          switch (param[paramIndex]) {
            case DramDrvStr:
            case WrOdt:
            case NomOdt:
            case ParkOdt:
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_ONLY, &optimalValue));
            MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                           "%2d\n", optimalValue));
            continue;
            default:
              break;
          }

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT, ""));
          for (strobe = 0; strobe < maxStrobes; strobe++) {
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
            optimalValue = 0;
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_ONLY, &optimalValue));
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "  %3d", optimalValue));
          } // strobe loop
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\n"));
        } // rank loop
      } // dimm loop
    } // ch loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n"));
  }
} // DisplayPowerTraingResults*/

/**
  Displays the CMD Vref training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A
**/
void
DisplayCMDVrefResults(
  PSYSHOST  host,
  UINT8     socket
)
{
  INT16     offset[MAX_CH];
  UINT8     ch;
  struct channelNvram                       (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_CMD_VREF_CENTERING_OFFSETS\n"));

  for (ch = 0; ch < MAX_CH; ch ++ ) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    CHIP_FUNC_CALL(host, GetSetCmdVref (host, socket, ch, 0, DdrLevel, GSM_READ_ONLY, &offset[ch]));
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "%d\n", offset[ch] ));
  } //ch loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
} //DisplayCMDVrefResults

#endif  // SERIAL_DBG_MSG

