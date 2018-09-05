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
 *      This file contains memory detection and initialization for
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"

/************************************************************************
  --Table of Contents--
  Training:
    1) TrainDramRon
    2) TrainMcOdt
    3) TrainNonTgtOdt
    4) TrainRttWr
    5) TrainMcRon
    6) TrainTxEq
    7) TrainImode
    8) TrainCTLE
  API
    1) TrainDDROptParam
    2) CalcPowerTrend
    3) FindOptimalTradeoff
*************************************************************************/
//#define POWER_TRAINING_DEBUG 1


UINT8 InValidSettingCheck(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 param, INT16 setting);
UINT8 SkipRankCheck(PSYSHOST  host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 param);

UINT32
TrainDramRon(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS] = {{DDR4_RON_34},{DDR4_RON_40},{DDR4_RON_48}};
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if (~host->setup.mem.optionsExt & DRAM_RON_EN) return SUCCESS;
  if (~host->memFlowsExt & MF_EXT_DIMMRON_EN) return SUCCESS;

  // Parameters to Pass
  mode             = MODE_VIC_AGG;
  patternLength    = 64;
  group[0]         = RxVref;
  group[1]         = RxDqsDelay;
  numGroups        = 2;
  marginType       = GET_MARGINS;
  numSettings      = 3;
  param[0]         = DramDrvStr;
  numParams        = 1;

  paramType = CHIP_FUNC_CALL(host, GetChipParamType (host, traindramron));

  TrainDDROptParam(host, socket, mode, patternLength, group, numGroups, marginType, settingList, powerLevel, numSettings,
                   param, paramType, numParams);

  return SUCCESS;

} // TrainDramRon

UINT32
TrainMcOdt(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS] = {{ODT_45_OHMS}, {ODT_50_OHMS}, {ODT_55_OHMS}, {ODT_100_OHMS}};
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;


  //
  // Return if this step is disabled
  //
  if (~host->setup.mem.optionsExt & RX_ODT_EN) return SUCCESS;
  if (~host->memFlowsExt & MF_EXT_MCODT_EN) return SUCCESS;

  //
  // Parameters to Pass
  //
  mode            = MODE_VIC_AGG;
  patternLength   = 64;
  group[0]        = RxVref;
  group[1]        = RxDqsDelay;
  numGroups       = 2;
  marginType      = GET_MARGINS;
  numSettings     = 4;
  param[0]        = RxOdt;
  numParams       = 1;

  paramType = CHIP_FUNC_CALL(host, GetChipParamType (host, trainmcodt));

  TrainDDROptParam(host,socket,mode,patternLength,group,numGroups,marginType,settingList,powerLevel,numSettings,param,paramType,numParams);

  return SUCCESS;

} // TrainMcOdt

UINT32
TrainMcRon(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS];
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if (~host->setup.mem.optionsExt & MC_RON_EN) return SUCCESS;
  if (~host->memFlowsExt & MF_EXT_MCRON_EN) return SUCCESS;

  // Parameters to Pass
  mode          = MODE_VIC_AGG;
  patternLength = 64;
  group[0]      = TxVref;
  group[1]      = TxDqDelay;
  numGroups     = 2;
  marginType    = GET_MARGINS;
  numSettings   = 3;
  param[0]      = TxRon;
  numParams     = 1;

  paramType = CHIP_FUNC_CALL(host, GetChipParamType (host, trainmcron));

  //
  // Update the settings list
  //
  numSettings = CHIP_FUNC_CALL(host, GetMcRonSettingsList(host, socket, settingList));

  TrainDDROptParam(host,socket,mode,patternLength,group,numGroups,marginType,settingList,powerLevel,numSettings,param,paramType,numParams);

  return SUCCESS;

} // TrainMcRon

UINT32
TrainTxEq(
  PSYSHOST host
)
{
  UINT8                               socket;
  UINT16                              mode;
  UINT32                              patternLength;
  GSM_GT                              group[MAX_GROUPS];
  UINT8                               numGroups;
  INT16                               settingList[MAX_SETTINGS][MAX_PARAMS];
  UINT16                              powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8                               numSettings;
  GSM_GT                              param[MAX_PARAMS];
  UINT8                               paramType;
  UINT8                               numParams;
  UINT8                               marginType;

  socket = host->var.mem.currentSocket;

  //if ((~host->memFlows & MF_TX_EQ_EN) || (host->nvram.mem.socket[socket].lrDimmPresent)) return SUCCESS;
  if (~host->memFlows & MF_TX_EQ_EN) return SUCCESS;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if (~host->setup.mem.optionsExt & TX_EQ_EN) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  // Parameters to Pass
  mode             = MODE_VIC_AGG;
  patternLength    = 64;
  group[0]         = TxVref;
  group[1]         = TxDqDelay;
  numGroups        = 2;
  marginType       = GET_MARGINS;
  param[0]         = TxEq;
  numParams        = 1;

  paramType = CHIP_FUNC_CALL(host, GetChipParamType (host, traintxeq));

  //
  // Update the settings list
  //
  numSettings = CHIP_FUNC_CALL(host, GetTxEqSettingsList(host, socket, settingList));

  TrainDDROptParam(host, socket, mode, patternLength, group, numGroups, marginType, settingList, powerLevel, numSettings,
                   param, paramType, numParams);

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK);

  return SUCCESS;

} // TrainTxEq

UINT32
TrainImode(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT8   ch;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS];
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if ((~host->memFlows & MF_IMODE_EN) || ((host->setup.mem.optionsExt & IMODE_EN) == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Update the settings list
  //
  numSettings = CHIP_FUNC_CALL(host, GetImodeSettingsList(host, socket, settingList));

  for (ch = 0; ch < MAX_CH; ch++) {
    //
    // Make sure rfon is set so the Tx Eq changes take effect
    //
    CHIP_FUNC_CALL(host, SetRfOn(host, socket, ch, 1));

  } // ch loop

  // Parameters to Pass
  mode             = MODE_VIC_AGG;
  patternLength    = 64;
  group[0]         = TxVref;
  group[1]         = TxDqDelay;
  numGroups        = 2;
  marginType       = GET_MARGINS;
  param[0]         = TxImode;
  numParams        = 1;

  paramType = CHIP_FUNC_CALL(host, GetChipParamType (host, trainimode));

  TrainDDROptParam(host, socket, mode, patternLength, group, numGroups, marginType, settingList, powerLevel, numSettings,
                   param, paramType, numParams);

  return SUCCESS;

} // TrainImode

UINT32
TrainCTLE(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS] = {
                                                    {4,0,0},
                                                    {4,0,1},
                                                    {4,0,2},
                                                    {4,1,0},
                                                    {4,1,1},
                                                    {4,1,2},
                                                    {4,2,0},
                                                    {4,2,1},
                                                    {4,2,2}
                                                  };
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if (~host->memFlowsExt & MF_EXT_RX_CTLE_EN) return SUCCESS;
  if ((~host->setup.mem.optionsExt & RX_CTLE_TRN_EN)) return SUCCESS;

  // Parameters to Pass
  mode              = MODE_VIC_AGG;
  patternLength     = 64;
  group[0]          = RxVref;
  group[1]          = RxDqsDelay;
  numGroups         = 2;
  marginType        = GET_MARGINS;
  param[0]          = RxEq;
  param[1]          = RxCtleC;
  param[2]          = RxCtleR;
  numParams         = 3;

  paramType   = CHIP_FUNC_CALL(host, GetChipParamType (host, trainctle));
  numSettings = CHIP_FUNC_CALL(host, GetCtleSettingsList (host, socket, settingList));
  TrainDDROptParam(host, socket, mode, patternLength, group, numGroups, marginType, settingList, powerLevel, numSettings,
                   param, paramType, numParams);

  return SUCCESS;

} // TrainCTLE

UINT32
TrainTcoComp(
  PSYSHOST host
)
{
  UINT8   socket;
  UINT16  mode;
  UINT32  patternLength;
  GSM_GT  group[MAX_GROUPS];
  UINT8   numGroups;
  INT16   settingList[MAX_SETTINGS][MAX_PARAMS];
  UINT16  powerLevel[MAX_SETTINGS] = LINEAR_ARRAY;
  UINT8   numSettings;
  GSM_GT  param[MAX_PARAMS];
  UINT8   paramType;
  UINT8   numParams;
  UINT8   marginType;

  socket = host->var.mem.currentSocket;
  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // This step only applies to DDR4
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  //
  // Return if this step is disabled
  //
  if (~host->memFlowsExt & MF_EXT_TCO_COMP_EN) return SUCCESS;

  // Parameters to Pass
  mode              = MODE_VIC_AGG;
  patternLength     = 64;
  group[0]          = TxVref;
  group[1]          = TxDqDelay;
  numGroups         = 2;
  marginType        = GET_MARGINS;
  param[0]          = TxTco;
  numParams         = 1;

  paramType   = GetChipParamType (host, traintcocomp);
  numSettings = GetTcoCompSettingsList(host, socket, settingList);

  TrainDDROptParam(host, socket, mode, patternLength, group, numGroups, marginType, settingList, powerLevel, numSettings,
                   param, paramType, numParams);

  return SUCCESS;

} // TrainTcoComp


void
TrainDDROptParam(
  PSYSHOST          host,
  UINT8             socket,
  UINT16            mode,
  UINT32            patternLength,
  GSM_GT            group[MAX_GROUPS],
  UINT8             numGroups,
  UINT8             marginType,
  INT16             settingList[MAX_ODT_SETTINGS][MAX_PARAMS],
  UINT16            powerLevel[MAX_SETTINGS],
  UINT8             numSettings,
  GSM_GT            param[MAX_PARAMS],
  UINT8             paramType,
  UINT8             numParams
)
/*++

Routine Description:

  This function implements a generic 1-D parameter optimization

Arguments:

  host    - Point to sysHost

Returns:

  UINT32  - if it succeeded, return -1

--*/
{
  UINT8               ch;
  UINT8               ch2;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               setting;
  UINT8               paramIndex;
  UINT8               groupIndex;
  INT16               margins[MAX_CH][MAX_RANK_CH][MAX_STROBE][MAX_GROUPS][3];
  INT16               averageMargins[MAX_CH][MAX_RANK_CH][MAX_STROBE][MAX_GROUPS][MAX_ODT_SETTINGS];
  INT16               bestSetting[MAX_CH][MAX_RANK_CH][MAX_STROBE];
  INT16               powerTrendLine[MAX_CH][MAX_RANK_CH][MAX_STROBE][MAX_ODT_SETTINGS];
  INT16               defaultValues[MAX_CH][MAX_DIMM][MAX_RANK_DIMM][MAX_STROBE][MAX_PARAMS] = {{{{{0}}}}};
  INT16               margin;
  INT16               minusOneMargin;
  INT16               plusOneMargin;
  UINT8               slopeFactor = 0;
  INT16               curVal       = 0;
  UINT8               curParam     = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct bitMask      filter;
  struct strobeMargin strobeMargins;

  channelNvList = GetChannelNvList(host, socket);
  //host->var.mem.serialDebugMsgLvl = SDBG_MAX;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "\nPrinting initialized array of cached values...\n\n"));
  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);
  for (groupIndex = 0; groupIndex < numGroups; groupIndex++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      for (rank = 0; rank < MAX_RANK_CH; rank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            bestSetting[ch][rank][strobe]             = 0;
        } // strobe loop
      } // rank loop
    } // ch loop
  } // group loop

  //------------------------------
  // Cycle through parameter types
  //------------------------------
  for (paramIndex = 0; paramIndex < numParams; paramIndex++) {
    curParam = param[paramIndex];
    switch (curParam){
        case DramDrvStr:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "DramRon\n"));
          break;
        case RxOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "McOdt\n"));
          break;
        case TxRon:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "McRon\n"));
          break;
        case WrOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "RTT WR\n"));
          break;
        case TxEq:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Tx Eq\n"));
          break;
        case RxEq:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Rx Eq\n"));
          break;
        case RxCtleC:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "CTLE C\n"));
          break;
        case RxCtleR:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "CTLE R\n"));
          break;
        case TxImode:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Tx Imode\n"));
          break;
        case NomOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "NomOdt\n"));
          break;
        case ParkOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "ParkOdt\n"));
          break;
        case TxTco:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "TxTco\n"));
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Unknown parameter\n"));
          break;
    }
    if (paramType == PER_CH){
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][0][0][0][paramIndex]));
        curVal = defaultValues[ch][0][0][0][paramIndex];
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%d\n", curVal ));
      } // ch loop
    } // per ch

    else if (paramType == PER_MC){
      for (ch = 0; ch < MAX_CH; ch++) {    // chipset specific
        if ((*channelNvList)[ch].enabled == 0) continue;
        CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][0][0][0][paramIndex]));
        curVal = defaultValues[ch][0][0][0][paramIndex];
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%d\n", curVal ));
      } // ch loop
    } // per MC

    else if (paramType == PER_RANK) {   // DRAM Ron and RTT WR are the only test that does per rank
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][dimm][rank][0][paramIndex]));
            curVal = defaultValues[ch][dimm][rank][0][paramIndex];
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
              "%2d\n", curVal ));
          } // rank loop
        } // dimm loop
      } // ch loop
    } // per rank

    else if ((paramType == PER_CH_PER_STROBE) || (paramType == PER_CH_PER_BYTE)){
      // Fill in margin entries for strobes for per ch per strobe
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Strobe        "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "%2d  ",strobe));
      }
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        PrintLine(host, 84, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "      "));

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && (strobe == 8))  continue;
          if ((strobe > 8) && (paramType == PER_CH_PER_BYTE)) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][0][0][strobe][paramIndex] ));
          curVal = defaultValues[ch][0][0][strobe][paramIndex];
          if (param[paramIndex] == TxImode){
            curVal = curVal;
          }
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "%3d ", curVal ));
        } // strobe loop
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
      } // ch loop
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
    } // per ch per byte or per ch per strobe

    else if (paramType == PER_CH_PER_DIMM) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Skip if no DIMM present
        //
        rank    = 0;
        dimm    = paramIndex/2;     // 0 0 1 1 dimm
        // curDimm = paramIndex%2;     // 0 1 0 1 prk/nom
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][dimm][0][0][paramIndex]));

        curVal = defaultValues[ch][dimm][0][0][paramIndex];

        if (param[paramIndex] == WrOdt){
          if (0 == curVal) {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "DIS"));
          }
        }

        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
          "%3d\n", curVal ));

      } // ch loop
    } // per ch per dimm

    else if ((paramType == PER_STROBE) ||  (paramType == PER_BYTE)) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Strobe        "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "%2d  ",strobe));
      }

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        PrintLine(host, 84, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);

          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                          " " ));
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              if ((paramType == PER_BYTE) && (strobe > 8)) continue;
              CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &defaultValues[ch][dimm][rank][strobe][paramIndex] ));
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "%2d  ", defaultValues[ch][dimm][rank][strobe][paramIndex] ));
            } // strobe loop
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "\n"));
          } // rank loop
        } // dimm loop
      } // ch loop
    } // per strobe
    else {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\nUnrecognized parameter type\n"));
      continue;
    }
  } // paramIndex
  //
  // End cycling through param indices
  //

  //--------------------------------------------------------
  // Loop through settings to find the optimal Margins
  //--------------------------------------------------------
  for(setting = 0; setting < numSettings; setting++) {
    //
    // Set Parameter
    //

// #if POWER_TRAINING_DEBUG
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\nSetting %d\tParams ", setting ));
    for (paramIndex = 0; paramIndex < numParams; paramIndex++) {
      if ((param[paramIndex] == DramDrvStr) || (param[paramIndex] == RxOdt)){
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "[%d]  = %d Ohms\t", paramIndex, settingList[setting][paramIndex]));
      } else if (param[paramIndex] == WrOdt){
        if (settingList[setting][paramIndex] == 0){
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "[%d] 0x%x ==> Disable\t", paramIndex, settingList[setting][paramIndex]));
        } else {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "[%d] %d Ohms\t", paramIndex, settingList[setting][paramIndex]));
        }
      } else if (param[paramIndex] == TxRon){
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "[%d] %d Ohms \t", paramIndex, settingList[setting][paramIndex]));
      } else if ((param[paramIndex] == ParkOdt) || (param[paramIndex] == NomOdt)){
        if (paramIndex == 0){
          curVal = settingList[setting][0];
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "D0:PRK "));
        }
        if (paramIndex == 1){
          curVal = settingList[setting][1];
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\tD0:NOM "));
        }
        if (paramIndex == 2){
          curVal = settingList[setting][2];
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\tD1:PRK "));
        }
        if (paramIndex == 3){
          curVal = settingList[setting][3];
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\tD1:NOM "));
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%3d", curVal));
      }

      else if (param[paramIndex] == RxEq ||  param[paramIndex] == RxCtleC || param[paramIndex] == RxCtleR){
          if (param[paramIndex] == RxEq){
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "Rx Eq 0x%x ", settingList[setting][0]  ));
          } else if (param[paramIndex] == RxCtleC){
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "CTLE C 0x%x ", settingList[setting][1] ));
          } else {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "CTLE R 0x%x\n", settingList[setting][2] ));
          }
      }
      else if (param[paramIndex] == TxTco) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "TxTco 0x%x\n", settingList[setting][0] ));
      }
      else {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "[%d] 0x%x\n", paramIndex, settingList[setting][paramIndex]));
      }
// #endif
      //
      // Set parameters for each setting
      //
      if (paramType == PER_CH){
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
        }
      } else if (paramType == PER_MC){
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
        }
      } else if (paramType == PER_RANK) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
          for (dimm = 0; dimm < MAX_DIMM; dimm++) {
            //
            // Skip if no DIMM present
            //
            if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

            // skip this settting if its not valid (DDR4 does not support RTTWRT=60 for example)
            if (InValidSettingCheck(host, socket, ch, dimm, param[paramIndex], settingList[setting][paramIndex])) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);
            for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
              //
              // Skip if no rank
              //
              if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
              if (SkipRankCheck(host, socket, ch, dimm, rank, param[paramIndex])) continue;
              if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;
              CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
            } // rank loop
          } // dimm loop
        } // ch loop
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "\n"));
      } else if ((paramType == PER_STROBE) || (paramType == PER_BYTE)) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);

          for (dimm = 0; dimm < MAX_DIMM; dimm++) {
            //
            // Skip if no DIMM present
            //
            if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);

            for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
              //
              // Skip if no rank
              //
              if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
              if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

              for (strobe = 0; strobe < MAX_STROBE; strobe++) {
                if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
                if ((paramType == PER_BYTE) && (strobe > 8)) continue;
                CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
              } // strobe loop
            } // rank loop
          } // dimm loop
        } // ch loop
      } // per strobe

      else if ((paramType == PER_CH_PER_STROBE) || (paramType == PER_CH_PER_BYTE)) {
        // Fill in margin entries for strobes for per ch per strobe
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            if ((!host->nvram.mem.eccEn) && (strobe == 8))  continue;
            if ((paramType == PER_CH_PER_BYTE) && (strobe >8)) continue;
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
          } // strobe loop
        } // ch loop
        //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
         // "\n"));
      } // per_ch_per_strobe

      else if (paramType == PER_CH_PER_DIMM) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);
          dimm    = paramIndex/2;     // 0 0 1 1 dimm
          //for (dimm = 0; dimm < MAX_DIMM; dimm++) {
            //
            // Skip if no DIMM present
            //
            if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
            rankList = GetRankNvList(host, socket, ch, dimm);
            for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
              //
              // Skip if no rank
              //
              if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
              if (SkipRankCheck(host, socket, ch, dimm, rank, param[paramIndex])) continue;
              if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;
              CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[setting][paramIndex]));
            } // rank loop
          //} // dimm loop
        } // ch loop
      } // per_ch_per_dimm
    } // param

    //----------------------------------------
    // Get per strobe margins for each setting
    //----------------------------------------
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++ ) {
      // Initialize to 0 for all strobe values
      for (ch = 0; ch < MAX_CH; ch++) {
        for (rank = 0; rank < MAX_RANK_CH; rank++) {
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            strobeMargins.strobe[ch][rank][strobe].n = 0;
            strobeMargins.strobe[ch][rank][strobe].p = 0;
          } // strobe loop
        } // rank loop

        // populate CH0 of each IMC with max value margins in case that channel is not populated
        if (paramType == PER_MC){
          ch2 = (ch/MAX_MC_CH)*MAX_MC_CH;
          margins[ch2][0][0][groupIndex][0] = 0xff;
        }
      } // ch loop

      // Get margins
      switch (marginType) {
        case GET_MARGINS:
          GetMargins(host, socket, DdrLevel, group[groupIndex], mode, SCOPE_STROBE, &filter, &strobeMargins, patternLength, 0, 0, 0,
            WDB_BURST_LENGTH);
          break;
        case TER_MARGINS:
          //GetTERMargins(host, socket, DdrLevel, group, mode, SCOPE_STROBE, &filter, &eyeMargin, patternLength, 0, 0, 0,
          //  WDB_BURST_LENGTH);
          break;
        case BER_MARGINS:
          //GetBERMargins(host, socket, DdrLevel, group, mode, SCOPE_STROBE, &filter, &eyeMargin, patternLength, 0, 0, 0,
          //  WDB_BURST_LENGTH);
          break;
        default:
          break;
      }

      // Print results
      switch(group[groupIndex]){
        case TxDqDelay:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nTx Eye Widths\n"));
          break;
        case TxVref:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nTx Eye Heights\n"));
          break;
        case RxDqsDelay:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nRx Eye Widths\n"));
          break;
        case RxVref:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nRx Eye Heights\n"));
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nInvalid group\n"));
          break;
      }

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Strobe        "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "%2d  ",strobe));
      }
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        PrintLine(host, 84, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);

          maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group[groupIndex], DdrLevel));

          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            if ((InValidSettingCheck(host, socket, ch, dimm, param[0], settingList[setting][0]) == 0) &&
               (SkipRankCheck(host, socket, ch, dimm, rank, param[0]) == 0)){
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, ""));
            }
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;

              if (strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].n > strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].p) {
                strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
                strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
              }
              if ((maxStrobe==9) && (strobe > 8)){ // Copy over nibbble0 to nibble1 for each byte
                margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe-9][groupIndex][0];
              } else {
                margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] =
                  strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].p - strobeMargins.strobe[ch][(*rankList)[rank].rankIndex][strobe].n;
              }
              if ((InValidSettingCheck(host, socket, ch, dimm, param[0], settingList[setting][0]) == 0) &&
                 (SkipRankCheck(host, socket, ch, dimm, rank, param[0]) == 0)){
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "%3d ", margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0]));
              }

            } // strobe loop
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
          } // rank loop
        } // dimm loop
      } // ch loop


      // Fill in margin entries for strobes for each case and determine the composite eye --> place it in 0 index
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group[groupIndex], DdrLevel));

            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
              // Per ch per byte or TXVREF margin on x8 DIMM

              // zero out the margins if its an invalid setting
              if (InValidSettingCheck(host, socket, ch, dimm, param[0], settingList[setting][0]) ||
                (SkipRankCheck(host, socket, ch, dimm, rank, param[0]))) {
                margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] = 0;
              }

              if (paramType == PER_BYTE) {
                if ((strobe > 8) && (maxStrobe == MAX_STROBE)){
                  // lower strobes margins are already in the right place, this copies over upper strobes
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] <margins[ch][(*rankList)[rank].rankIndex][strobe-9][groupIndex][0]){
                    margins[ch][(*rankList)[rank].rankIndex][strobe-9][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                }
              } // per ch per byte

              if (paramType == PER_CH_PER_BYTE) {
                if (strobe < 9 ) {
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][0][strobe][groupIndex][0]) {
                    margins[ch][0][strobe][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                } else if (maxStrobe == MAX_STROBE){
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][0][strobe-9][groupIndex][0]) {
                    margins[ch][0][strobe-9][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                }
              } // per ch per byte

              // Per ch per strobe
              if (paramType == PER_CH_PER_STROBE){
                if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][0][strobe][groupIndex][0]) {
                  margins[ch][0][strobe][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                }
              } // per ch per byte

              // Per rank
              if (paramType == PER_RANK) {
                if (strobe < 9) {
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][(*rankList)[rank].rankIndex][0][groupIndex][0]) {
                    margins[ch][(*rankList)[rank].rankIndex][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                } else if (maxStrobe == MAX_STROBE){ // if all 18 strobes are in use
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][(*rankList)[rank].rankIndex][0][groupIndex][0]) {
                    margins[ch][(*rankList)[rank].rankIndex][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                }
              } // per rank

              // Per ch  save in D0:R0:S0 for each CH                                   // special case for nontarget ODT training
              if ((paramType == PER_CH) || (param[0] == NomOdt) || (param[0] == ParkOdt)){
                if (strobe < 9) {
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][0][0][groupIndex][0]) {
                    margins[ch][0][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                } else if (maxStrobe == MAX_STROBE){
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch][0][0][groupIndex][0]) {
                    margins[ch][0][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                }
              } // PER_CH

               // per MC - get composite margins and save in CH0:D0:R0:S0 for each IMC
              if (paramType == PER_MC){
                ch2 = (ch/MAX_MC_CH)*MAX_MC_CH;
                if (strobe < 9) {
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch2][0][0][groupIndex][0]) {
                    margins[ch2][0][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                } else if (maxStrobe == MAX_STROBE){
                  if (margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0] < margins[ch2][0][0][groupIndex][0]) {
                    margins[ch2][0][0][groupIndex][0] = margins[ch][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  }
                }
              } // PER_MC
            } // strobe loop
          } // rank loop
        } // dimm loop
      } // ch loop

      //
      // Calculate Average Margins
      //
      curParam = param[0];

      // need new channel loop for worste case eye purposes PER_MC, can't combine with loop above.
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            // skip unused variables/ranks
            if ((paramType == PER_MC || paramType == PER_CH_PER_BYTE) && (dimm > 0 || rank > 0)) continue;
            if ((paramType == PER_CH_PER_DIMM) && (rank > 0)) continue;

            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

              // skip unused variables/strobes
              if ((paramType == PER_RANK || paramType == PER_CH || paramType == PER_MC || paramType == PER_CH_PER_DIMM) && (strobe > 0)) continue;
              if ((paramType == PER_BYTE || paramType == PER_CH_PER_BYTE) & (strobe > 8)) continue;
              //
              // Margin for the current setting = (margin[setting - 1] + (2 * margin[setting]) + margin[setting + 1]) * 10 / 4
              // The "+ 2" is to make the rounding correct
              //
              if (paramType == PER_MC) {
                ch2 = (ch/MAX_MC_CH)*MAX_MC_CH;
              } else {
                ch2 = ch;
              }

              // can't start calculations until > 0
              if ((curParam == TxEq) || (curParam == TxImode)){
                if (setting > 0) {
                  margin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1];
                  plusOneMargin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                  if (setting == 1) {
                    minusOneMargin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1];
                  } else {
                    minusOneMargin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][2];
                  }
                  // calculate average margin
                  averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][setting-1] = ((minusOneMargin + (2 * margin) + plusOneMargin) *10) / 4;
                  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, strobe, NO_BIT, "%3d , %3d, %3d, %3d \n", minusOneMargin, margin, plusOneMargin, averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][setting-1]));
                }

                // shift margins over in saved array
                margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][2] = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1];
                margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1] = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][0];

                // last margin point
                if(setting == (numSettings - 1)) {
                  margin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1];
                  plusOneMargin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][1];
                  minusOneMargin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][2];
                  averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][setting] = ((minusOneMargin + (2 * margin) + plusOneMargin) / 4) * 10;
                  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, strobe, NO_BIT, "%3d , %3d, %3d, %3d \n", minusOneMargin, margin, plusOneMargin, averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][setting-1]));
                }

              } else {
                margin = margins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][0];
                if ((curParam != NomOdt) && (curParam != ParkOdt)){
                  margin = margin*10;
                }
                averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][groupIndex][setting] = margin;
                //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, strobe, NO_BIT, "%3d \n", margin));
              }
            } // strobe loop
          } // rank loop
        } // dimm loop
      } // ch loop
    } // group loop
  } // setting loop

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "\nPower TrendLine Calculation\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks ; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        if (SkipRankCheck(host, socket, ch, dimm, rank, param[0])) continue;
        if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

        // skip unused variables
        if ((paramType == PER_MC || paramType == PER_CH_PER_BYTE) && (dimm > 0 || rank > 0)) continue;
        if ((paramType == PER_CH_PER_DIMM) && (rank > 0)) continue;
        //if (paramType == PER_CH_PER_BYTE && rank > 0) continue;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          if ((paramType == PER_RANK || paramType == PER_CH || paramType == PER_MC || paramType == PER_CH_PER_DIMM) && (strobe > 0)) continue;
          if ((paramType == PER_BYTE || paramType == PER_CH_PER_BYTE) & (strobe > 8)) continue;

          if ((paramType == PER_RANK || paramType == PER_CH || paramType == PER_MC || paramType == PER_CH_PER_DIMM) && (strobe == 0)) {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, ""));
          } else {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT, ""));
          }

          if (paramType == PER_MC) {
            ch2 = (ch/MAX_MC_CH)*MAX_MC_CH;
          } else {
            ch2 = ch;
          }

          CalcPowerTrend(host, socket, &averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][0], powerLevel, slopeFactor,
                         powerTrendLine[ch2][(*rankList)[rank].rankIndex][strobe], numSettings, numGroups);
          //
          // Conditional for 5 tick minimum check to reset to default values
          //
          bestSetting[ch2][(*rankList)[rank].rankIndex][strobe] = FindOptimalTradeOff(host,socket,&averageMargins[ch2][(*rankList)[rank].rankIndex][strobe][0],powerTrendLine[ch2][(*rankList)[rank].rankIndex][strobe],numSettings,numGroups);

        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  //
  // Set optimal setting
  //
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "START_OPTIMAL_TRAINING_RESULTS\n"));

  for (paramIndex = 0; paramIndex < numParams; paramIndex++) {
    curParam = param[paramIndex];
    switch (curParam){
        case DramDrvStr:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "DramRon\n"));
          break;
        case RxOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "McOdt\n"));
          break;
        case TxRon:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "McRon\n"));
          break;
        case WrOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "RTT WR\n"));
          break;
        case TxEq:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Tx Eq\n"));
          break;
        case RxEq:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Rx Eq\n"));
          break;
        case RxCtleC:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "CTLE C\n"));
          break;
        case RxCtleR:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "CTLE R\n"));
          break;
        case TxImode:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Tx Imode\n"));
          break;
        case NomOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "NomOdt\n"));
          break;
        case ParkOdt:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "ParkOdt\n"));
          break;
        case TxTco:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "TxTco\n"));
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Unknown parameter\n"));
          break;
    }

    if (paramType == PER_CH){
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        if (bestSetting[ch][0][0] == POWERTRAINING_DEFAULT_SETTING){
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &defaultValues[ch][0][0][0][paramIndex]));
          curVal = defaultValues[ch][0][0][0][paramIndex];
        } else {
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &settingList[bestSetting[ch][0][0]][paramIndex]));
          curVal = settingList[bestSetting[ch][0][0]][paramIndex];
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%2d\n", curVal));
      } // ch loop
    } // per ch

    else if (paramType == PER_MC){
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        ch2 = (ch/MAX_MC_CH)*MAX_MC_CH;
        if (bestSetting[ch2][0][0] == POWERTRAINING_DEFAULT_SETTING){
          if (param[paramIndex] == TxTco) {
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &defaultValues[ch][0][0][0][paramIndex]));
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &defaultValues[ch][0][0][0][paramIndex]));
          }
          curVal = defaultValues[ch][0][0][0][paramIndex];
        } else {
          if (param[paramIndex] == TxTco) {
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &settingList[bestSetting[ch2][0][0]][paramIndex]));
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, 0, 0, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &settingList[bestSetting[ch2][0][0]][paramIndex]));
          }
          curVal = settingList[bestSetting[ch2][0][0]][paramIndex];
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%2d\n", curVal ));

      } // ch loop
    } // per MC

    else if (paramType == PER_RANK) {

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;

            if ((bestSetting[ch][(*rankList)[rank].rankIndex][0] == POWERTRAINING_DEFAULT_SETTING) ||
              SkipRankCheck(host, socket, ch, dimm, rank, param[paramIndex])){
              CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &defaultValues[ch][dimm][rank][0][paramIndex]));
              curVal = defaultValues[ch][dimm][rank][0][paramIndex];
            } else {
              CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &settingList[bestSetting[ch][(*rankList)[rank].rankIndex][0]][paramIndex]));
              curVal = settingList[bestSetting[ch][(*rankList)[rank].rankIndex][0]][paramIndex];
            }
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
              "%2d\n", curVal ));
          } // rank loop
        } // dimm loop
      } // ch loop
    } // per rank

    else if ((paramType == PER_STROBE) || (paramType == PER_BYTE)){
      curParam = param[paramIndex];

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Strobe        "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "%2d  ",strobe));
      }

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        PrintLine(host, 84, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);

          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (param[0] != TxEq) {
            if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& rank > 0) continue;
            }
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                    " "));
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {
                if (param[0] == TxEq) {
                  defaultValues[ch][dimm][rank][strobe][paramIndex] = defaultValues[ch][dimm][0][strobe][paramIndex];
                  settingList[bestSetting[ch][(*rankList)[rank].rankIndex][strobe]][paramIndex] = settingList[bestSetting[ch][(*rankList)[0].rankIndex][strobe]][paramIndex];
                }
              }
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              if ((paramType == PER_BYTE) && (strobe >8)) continue;
              if (bestSetting[ch][(*rankList)[rank].rankIndex][strobe] == POWERTRAINING_DEFAULT_SETTING){
                CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &defaultValues[ch][dimm][rank][strobe][paramIndex]));
                curVal = defaultValues[ch][dimm][rank][strobe][paramIndex];
              } else {
                CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &settingList[bestSetting[ch][(*rankList)[rank].rankIndex][strobe]][paramIndex]));
                curVal = settingList[bestSetting[ch][(*rankList)[rank].rankIndex][strobe]][paramIndex];
              }
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
               "%2d  ", curVal));
            } // strobe loop
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "\n"));
          } // rank loop
        } // dimm loop
      } // ch loop
    } // per strobe

    else if ((paramType == PER_CH_PER_STROBE) || (paramType == PER_CH_PER_BYTE)) {

      // if (param[paramIndex] == RxOdt){
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Strobe  Param  "));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
        if ((strobe>8) && (paramType == PER_CH_PER_BYTE)) continue;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "%2d  ",strobe));
      }
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        PrintLine(host, 84, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif
      // Fill in margin entries for strobes for per ch per strobe
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimm = 0;
        rank = 0;

        rankList = GetRankNvList(host, socket, ch, dimm);

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "    %d  ", paramIndex));

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17)))  continue;
          if ((strobe>8) && (paramType == PER_CH_PER_BYTE)) continue;
            // Per ch per byte
          if (bestSetting[ch][(*rankList)[rank].rankIndex][strobe] == POWERTRAINING_DEFAULT_SETTING){
            CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &defaultValues[ch][dimm][rank][strobe][paramIndex]));
            curVal = defaultValues[ch][0][0][strobe][paramIndex];
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &settingList[bestSetting[ch][(*rankList)[rank].rankIndex][strobe]][paramIndex]));
            curVal = settingList[bestSetting[ch][(*rankList)[rank].rankIndex][strobe]][paramIndex];
          }

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "%3d ", curVal));
        } // strobe loop
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "\n"));

      } // ch loop
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\n"));
    } // per ch per byte
    else if (paramType == PER_CH_PER_DIMM) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Skip if no DIMM present
        //
        dimm = paramIndex/2;     // 0 0 1 1 dimm

        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          //
          // Skip if no rank
          //
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && rank > 0) continue;
          if ((bestSetting[ch][0][0] == POWERTRAINING_DEFAULT_SETTING) || SkipRankCheck(host, socket, ch, dimm, rank, param[paramIndex])){
            curVal = defaultValues[ch][dimm][0][0][paramIndex];
          } else {
            if ((param[0] == NomOdt) || (param[0] == ParkOdt)) {
              curVal = settingList[bestSetting[ch][0][0]][paramIndex];   // use DIMM0/RANK0 composite for all ranks
            } else {
              curVal = settingList[bestSetting[ch][(*rankList)[0].rankIndex][0]][paramIndex]; // use rank0 for each dimm
            }
          }
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param[paramIndex], GSM_UPDATE_CACHE, &curVal));

        } // rank loop
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
          "%3d\n", curVal));
      } // ch loop
    } // per ch per dimm
  } // optimal setting loop

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "END_OPTIMAL_TRAINING_RESULTS\n"));
} // TrainDDROptParam

void
CalcPowerTrend(
  PSYSHOST host,
  UINT8     socket,
  INT16     averageMargins[MAX_GROUPS][MAX_ODT_SETTINGS],
  UINT16    powerLevel[MAX_SETTINGS],
  UINT8     slopeFactor,
  INT16     powerTrendLine[MAX_SETTINGS],
  UINT8     numSettings,
  UINT8     numGroups
)
{
  UINT8     setting;
  INT16     maxMargin[MAX_GROUPS] = {0};
  INT16     maxMarginAllGroups = 0;
  UINT8     group;
  UINT16    minPower;
  UINT16    maxPower;
  INT16     AveOfMax;
  INT16     slope;
  UINT16    UPMLimit = 200;
  INT16     PWRLimit = 480;

  minPower = 0xFFFF;
  maxPower = 0;

  //
  // MaxPower = MAX(Power[params]); MinPower = MIN(Power[params])
  //
  for (setting = 0; setting < numSettings; setting++) {
    if (powerLevel[setting] < minPower) {
      minPower = powerLevel[setting];
    }
    if (powerLevel[setting] > maxPower) {
      maxPower = powerLevel[setting];
    }
    for (group = 0; group < numGroups; group++) {
      if ((averageMargins[group][setting]) > maxMargin[group]) {
        maxMargin[group] = averageMargins[group][setting];
      }
    }
  }

// #if POWER_TRAINING_DEBUG
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "minPower = %2d : maxPower = %3d  :",minPower, maxPower));
// #endif

  if (maxPower == 0) {
    maxPower = 1;
  }
  //
  // AveOfMax = AVEmargins(MAX(Margin[params]))
  //
  AveOfMax = 0;
  for (group = 0; group < numGroups; group++) {
    AveOfMax = AveOfMax + maxMargin[group];
  }
  AveOfMax = AveOfMax / numGroups;  // possible truncation error?

// #if POWER_TRAINING_DEBUG
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "AveOfMax = %4d : ",AveOfMax));
// #endif

  //
  // slope = 65% * MINmargins (MAX((MAX(Margin[params] - UPMLimit),0)/PWRLimit - UPMLimit)
  //
  slope = slopeFactor;
  for (group = 0; group < numGroups; group++) {
    if (maxMargin[group] > maxMarginAllGroups) {
      maxMarginAllGroups = maxMargin[group];
    }
  }
  if ((maxMarginAllGroups - UPMLimit) >= 0) {

// #if POWER_TRAINING_DEBUG
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "maxMarginAllGroups = %4d : ",maxMarginAllGroups));
// #endif

    slope = slope * (((maxMarginAllGroups - UPMLimit)*100)/(PWRLimit - UPMLimit));
  }

// #if POWER_TRAINING_DEBUG
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "slope = %2d\n",slope));
// #endif

  //
  // NormalizedPower[param] = AveOfMax * (1 - Slope * (Power[param] - MinPower)/MaxPower))
  //
  for (setting = 0; setting < numSettings; setting++) {
    powerTrendLine[setting] = AveOfMax * (10000 - slope * (((powerLevel[setting] - minPower) * 100) / maxPower)/100)/10000;
  }
} // CalcPowerTrend

INT16
FindOptimalTradeOff(
  PSYSHOST host,
  UINT8    socket,
  INT16    averageMargins[MAX_GROUPS][MAX_ODT_SETTINGS],
  INT16    powerTrendLine[MAX_SETTINGS],
  UINT8    numSettings,
  UINT8    numGroups
)
//
// Required inputs: Margin data, Power Trendline
//
{
  UINT32  maxMargin[MAX_GROUPS];
  UINT32  maxPowerTrendLine;
  UINT32  values[MAX_GROUPS + 1][MAX_ODT_SETTINGS];
  UINT32  minValue[MAX_ODT_SETTINGS];
  UINT32  maxValue;
  UINT32  smallestValue;
  UINT8   group;
  UINT8   group2;
  UINT8   setting;
  UINT8   x;
  UINT32  total[MAX_ODT_SETTINGS];

  for (setting = 0; setting < numSettings; setting++) {
    minValue[setting] = 0xFFFF;
  }

  //
  // Get Maximum margins per group and maximum trendline
  //
  for (group = 0; group < MAX_GROUPS; group++) {
    maxMargin[group] = 0;
    if (group >= numGroups) {
      maxMargin[group] = 1;
    } else {
      for (setting = 0; setting < numSettings; setting++) {
        if (maxMargin[group] < (UINT32)averageMargins[group][setting]) {
          maxMargin[group] = (UINT32)averageMargins[group][setting];
        }
        if (minValue[setting] > ((UINT32)averageMargins[group][setting])) {
          minValue[setting] = (UINT32)averageMargins[group][setting];
        }
      } // setting
    }
  } // group

  maxPowerTrendLine = 0;
  for (setting = 0; setting < numSettings; setting++) {
    if (maxPowerTrendLine < (UINT32)powerTrendLine[setting]) {
      maxPowerTrendLine = (UINT32)powerTrendLine[setting];
    }
    if (minValue[setting] > (UINT32)powerTrendLine[setting]) {
      minValue[setting] = (UINT32)powerTrendLine[setting];
    }
  } // setting

  //
  // Generate value array based on margin * max of other margins * powertrendline per setting
  //
  for (setting = 0; setting < numSettings; setting++) {
    for (group = 0; group < MAX_GROUPS; group++) {
      if (group >= numGroups) {
        values[group][setting] = 0;
      } else {
        values[group][setting] = averageMargins[group][setting];
        for (group2 = 0; group2 < numGroups ; group2++) {
          if (group != group2) {
            values[group][setting] = values[group][setting] * maxMargin[group2];
          }
        }
        values[group][setting] = values[group][setting] * maxPowerTrendLine;
      }
    } // group
    //
    // Calculate value for powerTrendline
    //
    values[MAX_GROUPS][setting] = powerTrendLine[setting];
    for (group = 0; group < numGroups ; group++) {
      values[MAX_GROUPS][setting] = values[MAX_GROUPS][setting] * maxMargin[group];
    } // group
  } // setting

  //Figuring out the maxValue
  maxValue = 0;
  for (setting = 0; setting < numSettings; setting++) {
    total[setting] = 1;
    for (x = 0; x < numGroups + 1; x++) {
      total[setting] = total[setting] * minValue[setting];
    }
    for (group = 0; group < MAX_GROUPS + 1; group++) {
      total[setting] = total[setting] + values[group][setting];
    }

    if (total[setting] > maxValue) {
      maxValue = total[setting];
    }
  } // setting

#if POWER_TRAINING_DEBUG
  if (numGroups > 3 ){
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Param V0   V1    V2   V3  T Line   V0*Max1*TL   V1*Max0*TL      V2*Max3      V3*Max2  TL*Max0*Max1   Min^n V       Total  Total/Max\n"));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n"));
    for (setting = 0; setting < numSettings; setting++) {
      if (maxValue == 0){     // Divide by 0 error
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "maxValue is 0...Divide by 0!!\n"));
          continue;
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        " %2d  %3d  %3d   %3d  %3d     %3d  %11d  %11d  %11d  %11d  %11d  %9d  %11d    %3d\n",
        setting,
        averageMargins[0][setting],
        averageMargins[1][setting],
        averageMargins[2][setting],
        averageMargins[3][setting],
        powerTrendLine[setting],
        values[0][setting],
        values[1][setting],
        values[2][setting],
        values[3][setting],
        values[MAX_GROUPS][setting],
        total[setting] - values[0][setting] - values[1][setting] - values[2][setting] - values[3][setting] - values[4][setting],
        total[setting],
        total[setting] / (maxValue / 100)
      ));
    }
  } else {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Param GV0  GV1  T Line  GV0*Max1*TL  GV1*Max0*TL  TL*Max0*Max1      Min^3 V        Total  Total/Max\n"));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "--------------------------------------------------------------------------------------------------------------------------\n"));
    for (setting = 0; setting < numSettings; setting++) {
      if (maxValue == 0){     // Divide by 0 error check
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "maxValue is 0...Divide by 0!!\n"));
          continue;
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        " %2d   %3d  %3d     %3d  %11d  %11d   %11d  %11d  %11d        %3d\n",
        setting,
        averageMargins[0][setting],
        averageMargins[1][setting],
        powerTrendLine[setting],
        values[0][setting],
        values[1][setting],
        values[MAX_GROUPS][setting],
        total[setting] - values[0][setting] - values[1][setting] - values[4][setting],
        total[setting],
        total[setting] / (maxValue / 100)
      ));
    }
  }
#endif

  //
  // If it's the biggest, it's the best and GTFO; if the smallest value is >= PERCENTAGE_TOLERANCE(95) at the end of the check, use the default value and exit
  //
  smallestValue = 100;
  for (setting = 0; setting < numSettings; setting++) {
    if (maxValue == 0) {     // Divide by 0 error check
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "maxValue is 0...Divide by 0!!\n"));
        continue;
    }

    //
    // Check to see if the smallest is >= PERCENTAGE_TOLERANCE(95)
    //
    if ((total[setting]/ (maxValue/100)) < smallestValue) {
      smallestValue = (total[setting]/ (maxValue/100));
    }
    if (smallestValue >= PERCENTAGE_TOLERANCE && setting == (numSettings-1)) {
      //
      // The default values for all functions were stored at the beginning of TrainDDROptParam
      // Empirically demonstrated that there is little impact in margins from 95 to 100
      //
      return POWERTRAINING_DEFAULT_SETTING;
    } else continue;
  } // setting

  //
  // Return the optimal index
  //
  for (setting = 0; setting < numSettings; setting++) {
    if (maxValue == 0) {     // Divide by 0 error check
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "maxValue is 0...Divide by 0!!\n"));
        continue;
    }

    if (total[setting] / maxValue) {
      return setting;
    }
  } // setting
  return POWERTRAINING_DEFAULT_SETTING;
}

/*++

Routine Description:

  Checks to see if this is valid setting to test

Arguments:

  channel, dimm, parameter, setting

Returns:

  1 = failure
  0 = pass

--*/
UINT8
InValidSettingCheck(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    param,
  INT16    setting
)
{

  // not LRDIMM and RTT_WRT=60
  if ((CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))== 0) && (param == WrOdt) && (setting == RTTWR_60)) {
    return 1;
  }

  // not LRDIMM and DRAM_RON = 40
  if ((CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))== 0) && (param == DramDrvStr) && (setting == DDR4_RON_40)) {
    return 1;
  }

  return 0;
}

/*++

Routine Description:

  Checks to see if this is valid setting to test

Arguments:

  channel, dimm, rank, parameter, setting

Returns:

  1 = skip
  0 = dont skip

--*/
UINT8
SkipRankCheck(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     param
)
{

  INT16           rttwrt;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  // Dont train rttwr if its disabled
  if ((param == WrOdt)){
    CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, 0, ALL_BITS, DdrLevel, param, GSM_READ_CSR | GSM_READ_ONLY, &rttwrt));
    if (rttwrt == RTTWR_DIS) return 1;
  }

  // skip 1DPC SR case
  if ((param == NomOdt) || (param == ParkOdt)){
    channelNvList = GetChannelNvList(host, socket);
    dimmNvList = GetDimmNvList(host, socket, ch);
    if (((*channelNvList)[ch].maxDimm == 1) && (((*dimmNvList)[dimm].numRanks == 1) || CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))))return 1;
  }
  return 0;
}


