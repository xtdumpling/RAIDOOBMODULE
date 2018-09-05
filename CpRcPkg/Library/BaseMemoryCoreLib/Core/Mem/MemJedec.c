//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//*****************************************************************************
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


extern const UINT32  preciseTCK[MAX_SUP_FREQ];
struct lrbufRefresh RefreshStaggerLookupTable[MAX_SUP_FREQ][MAX_DENSITY][MAX_RM];

//
// Local Prototypes
//
void    DoRegisterInitAep(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
void    DoRegisterInitDDR4_CKE(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
void    InitLrDimmRegisterDvidRid (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
void    ProgramMRSDDR3(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
void    ProgramMRSDDR4(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
void    ProgramMRSAEP(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
#ifdef LRDIMM_SUPPORT
void    DoLrbufInitDDR4(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
void    WriteRcLrbufExtended16 (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT16 lrbufData, UINT16 lrbufAddr);
void    LrDimmRegisterInit (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);
void    DoLrbufTraining (PSYSHOST host, UINT8 socket, UINT8 ch);
#endif // LRDIMM_SUPPORT



// -------------------------------------
// Code section
// -------------------------------------

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     Dimm number
  @param level:    Level of the bus
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum control delay
  @param *maxVal:  Current maximum control delay

  @retval status
**/
MRC_STATUS
GetSetCtlGroupDelayCore (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  GSM_LT    level,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value,
  UINT16    *minVal,
  UINT16    *maxVal
  )
{
  UINT8                                           rank;
  UINT16                                          curVal;
  UINT8                                           rcVal;
  INT16                                           tmp = 0;
  struct socketNvram                              *socketNvram;
  struct dimmNvram                                (*dimmNvList)[MAX_DIMM];
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  socketNvram = &host->nvram.mem.socket[socket];

  if (level == LrbufLevel) {
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].aepDimmPresent) {
      status = CHIP_FUNC_CALL(host, GetSetCtlDelayFnv(host, socket, ch, dimm, mode, value, minVal, maxVal));
    } else {
      curVal = socketNvram->channelList[ch].dimmList[dimm].rcCtl;

      if (mode & GSM_READ_ONLY) {
        *value = curVal;
      } else {
        // Adjust the current CLK delay value by offset
        if (mode & GSM_WRITE_OFFSET) {
          // Get the new offset (note: can be negative)
          tmp = curVal + *value;
        } else {
          // FORCE_WRITE
          tmp = *value;
        }
        // Check range
        if (!((tmp >= 0) && (tmp <= 31))) {
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_71);
        }
        //
        // Write the new value
        //
        if (tmp) {
          rcVal = (UINT8)tmp | BIT7;
        } else {
          rcVal = (UINT8)tmp;
        }
        rank = 0;
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x10);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x20);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x30);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x40);

        if (mode & GSM_UPDATE_CACHE) {
          socketNvram->channelList[ch].dimmList[dimm].rcCtl = (UINT8)tmp;
        }
      }
    }
    CHIP_FUNC_CALL(host, UpdateMinMax(*value, minVal, maxVal));
  } else {

    status = CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, group, mode, value, minVal, maxVal));
  } // level

  return status;
} // GetSetCtlGroupDelayCore


/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     Dimm number
  @param level:    Level of the bus
  @param group:    Group number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program
  @param *minVal:  Current minimum command delay
  @param *maxVal:  Current maximum command delay

  @retval status
**/
MRC_STATUS
GetSetCmdGroupDelayCore (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  GSM_LT    level,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value,
  UINT16    *minVal,
  UINT16    *maxVal
  )
{
  UINT8                                           rank;
  UINT16                                          curVal;
  UINT8                                           rcVal;
  INT16                                           tmp = 0;
  struct socketNvram                              *socketNvram;
  struct dimmNvram                                (*dimmNvList)[MAX_DIMM];
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  socketNvram = &host->nvram.mem.socket[socket];

  if (group == CmdVref) {

    status = GetSetCmdVrefCore(host, socket, ch, dimm, level, mode, value);

  } else if (level == LrbufLevel) {
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].aepDimmPresent) {
      status = CHIP_FUNC_CALL(host, GetSetCmdGroupDelayFnv(host, socket, ch, dimm, group, mode, value, minVal, maxVal));
    } else {
      curVal = socketNvram->channelList[ch].dimmList[dimm].rcCmd;

      if (mode & GSM_READ_ONLY) {
        *value = curVal;
      } else {
        // Adjust the current CLK delay value by offset
        if (mode & GSM_WRITE_OFFSET) {
          // Get the new offset (note: can be negative)
          tmp = curVal + *value;
        } else {
          // FORCE_WRITE
          tmp = *value;
        }
        // Check range
        if (!((tmp >= 0) && (tmp <= 31))) {
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_72);
        }
        //
        // Write the new value
        //
        if (tmp) {
          rcVal = (UINT8)tmp | BIT7;
        } else {
          rcVal = (UINT8)tmp;
        }
        rank = 0;
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x50);

        if (mode & GSM_UPDATE_CACHE) {
          socketNvram->channelList[ch].dimmList[dimm].rcCmd = (UINT8)tmp;
        }
      }
      CHIP_FUNC_CALL(host, UpdateMinMax(*value, minVal, maxVal));
    }
  } else {

    status = CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, group, mode, value, minVal, maxVal));
  } // level

  return status;
} // GetSetCmdGroupDelayCore


/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     Dimm number
  @param level:    Level of the bus
  @param clk:      Clock number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program

  @retval status
**/
MRC_STATUS
GetSetClkDelayCore (
   PSYSHOST host,
   UINT8    socket,
   UINT8    ch,
   UINT8    dimm,
   GSM_LT   level,
   UINT8    clk,
   UINT8    mode,
   INT16    *value
   )
{
  UINT8                               rank;
  UINT16                              curVal;
  INT16                               tmp;
  UINT8                               rcVal;
  struct socketNvram                  *socketNvram;
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  socketNvram = &host->nvram.mem.socket[socket];

  if (level == LrbufLevel) {
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].aepDimmPresent) {
      status = CHIP_FUNC_CALL(host, GetSetClkDelayFnv(host, socket, ch, dimm, mode, value));
    } else {
      curVal = socketNvram->channelList[ch].dimmList[dimm].rcClk;

      if (mode & GSM_READ_ONLY) {
        *value = curVal;
      } else {
        // Adjust the current CLK delay value by offset
        if (mode & GSM_WRITE_OFFSET) {
          // Get the new offset (note: can be negative)
          tmp = curVal + *value;
        } else {
          // FORCE_WRITE
          tmp = *value;
        }
        // Check range
        if (!((tmp >= 0) && (tmp <= 31))) {
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_73);
        }
        //
        // Write the new value
        //
        if (tmp) {
          rcVal = (UINT8)tmp | BIT7;
        } else {
          rcVal = (UINT8)tmp;
        }
        rank = 0;
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x60);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x70);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x80);
        IndirectRCWrite(host, socket, ch, dimm, rank, rcVal, 1, 0x90);

        if (mode & GSM_UPDATE_CACHE) {
          socketNvram->channelList[ch].dimmList[dimm].rcClk = (UINT8)tmp;
        }
      }
    }
  } else {

    status = CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, mode, value));
  } // level

  return status;
} // GetSetClkDelayCore


/**
  Muli-use function to either get or set command vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param dimm:     Dimm number
  @param level:    Level of the bus
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval status
**/
MRC_STATUS
GetSetCmdVrefCore (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  GSM_LT    level,
  UINT8     mode,
  INT16     *value
  )
{
  INT16                                         curVal;
  UINT8                                         controlWordData;
  UINT8                                         vrefOffset = 0;
  INT16                                         tmp;
  struct socketNvram                            *socketNvram;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  MRC_STATUS      status = MRC_STATUS_SUCCESS;

  socketNvram = &host->nvram.mem.socket[socket];
  dimmNvList = GetDimmNvList(host, socket, ch);

  controlWordData = CHIP_FUNC_CALL(host, CaVrefSelect(host));  // Enable external CA Vref.  This is a change from HSX launch collateral

  if (!((*dimmNvList)[dimm].aepDimmPresent) && (level == LrbufLevel || (level == DdrLevel && !(controlWordData & 0x8) && (host->nvram.mem.dimmTypePresent == RDIMM)))) {

    curVal = (socketNvram->channelList[ch].dimmList[dimm].rcVref >> (8 * level)) & 0xff;
    if (curVal & BIT5) {
      curVal = (curVal & 0x1f) - 32;
    }

    if (mode & GSM_READ_ONLY) {
      *value = curVal;
    } else {
      // Adjust the current CLK delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        // Get the new offset (note: can be negative)
        tmp = curVal + *value;
      } else {
        // FORCE_WRITE
        tmp = *value;
      }

      // Check range
      if ((tmp >= -20) && (tmp <= 20)) {
        vrefOffset = (UINT8) tmp & 0x3F;
      } else {
        RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_74);
      }

      //
      // Write the new value
      //
      if (level == LrbufLevel) {
        controlWordData = (controlWordData & 0x9) | 0x2;
      }
      WriteRC (host, socket, ch, dimm, 0, controlWordData, RDIMM_RC0B);
      WriteRC (host, socket, ch, dimm, 0, vrefOffset, RDIMM_RC1x);

      if (mode & GSM_UPDATE_CACHE) {
        socketNvram->channelList[ch].dimmList[dimm].rcVref &= ~(0xff << (8*level));
        socketNvram->channelList[ch].dimmList[dimm].rcVref |= (vrefOffset << (8*level));
      }
    }
  } else {

    status = CHIP_FUNC_CALL(host, GetSetCmdVref (host, socket, ch, dimm, level, mode, value));
  } // level

  return status;
} // GetSetCmdVrefCore


/**
  Function to shift backside Cmd, Ctl, Ck by +/- 31/64 clock

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param direction: +/- direction to move timings

  @retval SUCCESS
**/
UINT32
BacksideShift(
  PSYSHOST  host,
  UINT8     socket,
  INT8      direction
)
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  INT16               piDelay;
  INT16               piDelay2;
  UINT16              minVal = 0xFF;
  UINT16              maxVal = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8               ddr4Present = 0;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].aepDimmPresent) continue;

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      }

      ddr4Present |= 1 << ch;
      break;
    }
  }

  if (!ddr4Present)
    return SUCCESS;

  //  Added conditional to identify the type of normalization being done
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,    "\n"));
  if (direction == DENORMALIZE){
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Pull in backside Cmd, Ctl, Ck by 31/64 clock\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Push out backside Cmd, Ctl, Ck by 31/64 clock\n"));
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    piDelay = direction * 31;
    piDelay2 = direction * 62;

    //  Check the value and see if it is the same value as the normalization factor
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Cmd normalization Factor %d\tDirection %d\n",  host->nvram.mem.normalizationFactor[socket][ch], direction));

    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

      if ((*dimmNvList)[dimm].aepDimmPresent)
        continue;

      GetSetClkDelayCore (host, socket, ch, dimm, LrbufLevel, 0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay);
      GetSetCtlGroupDelayCore (host, socket, ch, dimm, LrbufLevel, CtlAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minVal, &maxVal);
      GetSetCmdGroupDelayCore (host, socket, ch, dimm, LrbufLevel, CmdAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minVal, &maxVal);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Shift data delays
        //
        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, (INT16 *)&piDelay2));
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, (INT16 *)&piDelay2));
        } // strobe loop

      } // rank loop
    } // dimm loop
  } // ch loop

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence(host, socket, ddr4Present);

  //
  // Re-run Receive Enable training (messes up LRDIMMs when normalizing cmd/ctl)
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "Re-run Receive Enable training\n"));
  ReceiveEnable(host);

  return SUCCESS;
} // BacksideShift


/**

  Turns on the MPR feature for read training

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
EnableMPR (
  PSYSHOST host
  )
{

  //
  // Turn on MPR pattern for all ranks
  //
  SetAllRankMPR (host, host->var.mem.currentSocket, MR3_MPR);


  return SUCCESS;
} // EnableMPR

/**

  Turns off the MPR feature for read training

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
DisableMPR (
  PSYSHOST host
  )
{
  //
  // Turn off MPR pattern for all ranks
  //
  SetAllRankMPR (host, host->var.mem.currentSocket, 0);

  return SUCCESS;
} // DisableMPR

/**

  This routine does the twr scaling for the DIMMS capable of the same in the system

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
TwrScaling (
  PSYSHOST  host
  )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8               mprPage, mprLoc;
  UINT8               MprData[MAX_STROBE];
  UINT8               Value;
  UINT8               socket;
  UINT8               tWR;
  UINT8               DisCh=0;

  //if ((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBoot)) return SUCCESS;
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;
  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "TwrScaling Starts\n"));
#endif
  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if (((*dimmNvList)[dimm].aepDimmPresent) || ((*dimmNvList)[dimm].sdramCapacity <= SPD_8Gb)){
        continue;
      }
      rank = 0;
      // MPR Page 3,  MPR2 bits 7 and 6 for tWR scaling.
      mprPage = 3; //page 3
      mprLoc = 2; //MRR 2
      CHIP_FUNC_CALL(host, ReadMprRankDdr4(host, socket, ch, dimm, rank, mprPage, mprLoc, MprData));
      Value = MprData[0];// we need only one DRAM MPR data since all the DRAM on the DIMMs are the same.
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
        "R:%d, MPR P:%d, L:%d , Value: %d \n", rank, mprPage, mprLoc,Value ));
      Value = (Value & 0xC0);
      Value = Value >> 6;
      if (Value == 0) continue;// this DIMM/rank does not support scaling. So loop to the next DIMM

      //we are here , because the DIMM supports twrscaling
      Value++;
      tWR = (UINT8)GettWR(host, socket);
      tWR = (tWR * Value);

      // Check if value exceeds the bits on HSX
      CHIP_FUNC_CALL(host, CheckTwrMargins(host, socket, ch, tWR, &DisCh));

      // In early config, we have the (*channelNvList)[ch].common.nWR normalised. Now we need to know if the tWR that we
      // have calculated in account of the scaling is more than that , if so we need that value to be the fastest common in the channel.
      if ((*channelNvList)[ch].common.nWR < tWR) {
        (*channelNvList)[ch].common.nWR = tWR;
      }
    }//dimm
    if (DisCh) {
      DisCh = 0;
      continue; // Loop to the next channel.
    }
    CHIP_FUNC_CALL(host, SetChTwr (host, socket, ch, (*channelNvList)[ch].common.nWR));
  }//ch

  // Normalise lockstep timings.
  CHIP_FUNC_CALL(host, NormalizeLockstepTiming(host, socket));

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "TwrScaling Ends\n"));
#endif

  return SUCCESS;
}// enf of twrscaling


/**

  Starts the JEDEC initialization sequence

  @param host    - Pointer to sysHost
  @param socket  - Socket to initizlize
  @param chBitMask - Bit Mask of Channels Present on the Socket

  @retval SUCCESS

**/
UINT32
JedecInitSequence (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitMask
  )
{
  // PLL lock workaround
  UINT8               ch;
  UINT8               dimm;
  UINT32              lrbufData = 0;
  UINT8               errorcnt[MAX_CH][MAX_DIMM];
  UINT8               passFlag;
  UINT8               errorFlag;
#ifdef   SERIAL_DBG_MSG
  UINT8               sdblOrg;
#endif // SERIAL_DBG_MSG
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  // PLL lock workaround
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      errorcnt[ch][dimm] = 0;
    } // dimm loop
  } // ch loop

  errorFlag = 0;
  passFlag = 1;

#ifdef   SERIAL_DBG_MSG
  sdblOrg = host->var.mem.serialDebugMsgLvl;
#endif // SERIAL_DBG_MSG

  while (passFlag) {
    passFlag = 0;

    //
    // Issue reset to the channels
    //
    CHIP_FUNC_CALL(host, ResetAllDdrChannels (host, socket, chBitMask));

    if ((host->nvram.mem.dimmTypePresent == RDIMM) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) { //RDIMM and DDR4
      for (ch = 0; ch < MAX_CH; ch++) {
        //Check if this channel has been masked off as each BSSA Call is per MC
        if ((chBitMask & (1 << ch)) == 0) continue;
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          if (((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) &&
              ((*dimmNvList)[dimm].SPDRegRev == IDT_SPD_REGREV_B0)) {

            ReadLrbufSmb (host, socket, ch, dimm, 0x1010, &lrbufData);
            if ((((lrbufData >> 9) & 0x1) != 1) || (((lrbufData >> 12) & 0x1) != 0)) {
              errorcnt[ch][dimm] += 1;
              if (errorcnt[ch][dimm] >= 5) {
                errorFlag = 1;
              }
              passFlag = 1;
            }
          } // IDT check
        } // dimm loop
      } // ch loop
    } // RDIMM/DDR4 check

    if (errorFlag) {
      passFlag = 0;
    } // if PLL lock errors
  } // while loop

  for (ch = 0; ch < MAX_CH; ch++) {
    //Check if this channel has been masked off as each BSSA Call is per MC
    if ((chBitMask & (1 << ch)) == 0) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if (errorcnt[ch][dimm]) {
        restoreMsg(host);
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"# PLL Lock errors=%d\n",errorcnt[ch][dimm]));
#if SMCPKG_SUPPORT
        OutputWarning (host, ERR_VENDOR_SPECIFIC, ERR_DIMM_PLL_LOCK_ERROR, socket, ch, dimm, NO_RANK);
#endif
      }
    } // dimm loop
  } // ch loop

  if (errorFlag) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"PLL lock errors\n"));
#if SMCPKG_SUPPORT == 0
    FatalError (host, ERR_VENDOR_SPECIFIC, ERR_DIMM_PLL_LOCK_ERROR);
#endif
  } // if PLL lock errors

#ifdef   SERIAL_DBG_MSG
  host->var.mem.serialDebugMsgLvl = sdblOrg;
#endif // SERIAL_DBG_MSG
  // PLL lock workaround

  if (((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode != WarmBootFast))) {
    //
    // Initiate JEDEC init
    //
    JedecInitDdrAll (host, socket, chBitMask);
  }

#ifdef PPR_SUPPORT
  //
  // If soft PPR has already been applied, we need to re-run
  // it every time JedecInitSequence is executed
  //

  host->var.mem.notRunningFromCT = 1; //Set when not calling from Call Table
  host->var.mem.chBitMask = chBitMask;

  if (host->var.mem.softPprDone[socket]) {
    PostPackageRepairMain (host);
  }

  host->var.mem.notRunningFromCT = 0; //Clear value

#endif // PPR_SUPPORT

  return SUCCESS;
} // JedecInitSequence

/**

  Executes the JEDEC initialization sequence for each channel on the provided socket number

  @param host      - Pointer to sysHost
  @param socket    - Socket Id
  @param chBitMask - Bit Mask of Channels Present on the Socket

  @retval N/A

**/
void
JedecInitDdrAll (
  PSYSHOST      host,
  UINT8         socket,
  UINT32        chBitMask
  )
{
  UINT8               ch;
  struct channelNvram (*channelNvList)[MAX_CH];

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "JEDEC Init\n"));
  channelNvList = GetChannelNvList(host, socket);

  //
  // Update channels
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    //Check if this channel has been masked off as each BSSA Call is per MC
    if ((chBitMask & (1 << ch)) == 0) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Reset and JEDEC init
    //
    JedecInitDdrChannel (host, socket, ch);
  } // ch loop

} // JedecInitDdrAll

/**

  Executes the JEDEC initialization sequence for the given channel

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)

  @retval N/A

**/
void
JedecInitDdrChannel (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
{
  UINT8   dimm;
  UINT8   rank;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  UINT8  cmd_stretch;
  UINT16 dateCode;

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Set 3N timing and save original setting
  //
  cmd_stretch = CHIP_FUNC_CALL(host, GetCurrentTimingMode(host, socket, ch));
  CHIP_FUNC_CALL(host, SetTimingMode(host, socket, ch, (*channelNvList)[ch].trainTimingMode));

  //
  // Initialize the register if this is an RDIMM
  //
  if (host->nvram.mem.dimmTypePresent == RDIMM) {

    if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {

      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          if (!(rank & 0x1)) {
            DoRegisterInit (host, socket, ch, dimm, rank);
          }
        } // rank loop
      } // dimm loop

    } else {

      //
      // First pass with CKE low
      //
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (CheckRank(host, socket, ch, dimm, 0, DONT_CHECK_MAPOUT)) continue;
        if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent) {
          CHIP_FUNC_CALL(host, DdrtBufferReset(host, socket, ch, dimm));
        } else {
          DoRegisterInitDDR4 (host, socket, ch, dimm, 0);
        }
      } // dimm loop

      //
      // Take CKE high to all slots
      //
      FixedDelay (host, 500);

      // Assert CKE
      CHIP_FUNC_CALL(host, SetCkeOverride (host, socket, ch, (*channelNvList)[ch].ckeMask));
      FixedDelay (host, 1);
      if (host->var.mem.socket[socket].csrUnlock == 0) {
        FnvPollingBootStatusRegisterCh(host, socket, ch, BIT19);
        host->var.mem.socket[socket].csrUnlock = 1;
      }
      //
      // Second pass with CKE high
      //
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (CheckRank(host, socket, ch, dimm, 0, DONT_CHECK_MAPOUT)) continue;
        if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
        if (!(*dimmNvList)[dimm].aepDimmPresent) {
          DoRegisterInitDDR4_CKE (host, socket, ch, dimm, 0);
        } // NVMDIMM
      } // dimm loop
    } //dramType

    CHIP_FUNC_CALL(host, SetEncodedCsMode (host, socket, ch));
  } // if RDIMM

  // Hynix caseID(RMA): R14D035 - Start
  // If Hynix DIMM is present, issue a PREA
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
    dateCode = (((*dimmNvList)[dimm].SPDModDate >> 8) & 0xFF) | (((*dimmNvList)[dimm].SPDModDate << 8) & 0xFF00);
    if ((host->nvram.mem.dramType == SPD_TYPE_DDR4) && ((*dimmNvList)[dimm].SPDDramMfgId == MFGID_HYNIX)
        && (dateCode < 0x1545) && ((*dimmNvList)[dimm].sdramCapacity == SPD_4Gb) && ((*dimmNvList)[dimm].SPDModPartDDR4[8] == 0x4D)){

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Hynix W/A On\n"));
      //
      // Loop for each rank
      //
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Setup the test for a precharge
        // 4987528: Reset noise sensitivity on RDIMM (post register) may cause boot failure
        //
        CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, PRE_CMD, 0, 0x400, 0));
      } //rank
    } //Hynix DDR4
  } //dimm
  // Hynix caseID(RMA): R14D035 - End
  //
  // Loop for each DIMM
  //
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    //
    // Loop for each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Skip if no rank
      //
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        ProgramMRSAEP (host, socket, ch, dimm, rank);
        // FatalError
      } else if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
        ProgramMRSDDR3 (host, socket, ch, dimm, rank);
      } else if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
        ProgramMRSDDR4 (host, socket, ch, dimm, rank);
      }
    } // rank loop
  } // dimm loop

  //
  // Issue ZQCAL
  //
  CHIP_FUNC_CALL(host, DoZQ(host, socket, ch, ZQ_LONG));

#ifdef  LRDIMM_SUPPORT
  //
  // LRDIMM buffer init
  //
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) &&
        (host->nvram.mem.dramType != SPD_TYPE_DDR3)) {
        if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
      DoLrbufInitDDR4(host, socket, ch, dimm, 0);
    }
  } //dimm loop

  // Train the DRAM side of the LR buffers per ch
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    DoLrbufTraining(host, socket, ch);
  }

    // Apply Inphi iMB sighting 9830
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
    dimmNvList  = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      // Inphi workarounds start
      // Check for Inphi A0/A1 Device/vendor Id
      if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_INPHI_DVID) &&
          ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A0)) {
        // Inphi iMB sighting 9830
        WriteRcLrbufExtended16(host, socket, ch, dimm, 0, 0x1002, 0x01de);
      } // Inphi A0 MB
    } // dimm
  } // lrDimmPresent

#ifdef IDT_LRBUF_WA
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_IDT_DVID) &&
          ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_RID_B0)) {
        // sighting 200012
        WriteRcLrbufExtended(host, socket, ch, dimm, 0, 1, 0x400A);
        // sighting 200013
        WriteRcLrbufExtended(host, socket, ch, dimm, 0, 0x43, 0x8027);
      } // LRBUF_IDT_DVID_B0
    } // dimm
  } // lrDimmPresent
#endif
#endif

  //
  // Restore original CMD timing
  //
  CHIP_FUNC_CALL(host, SetTimingMode(host, socket, ch, cmd_stretch));

  if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->nvram.mem.txVrefDone)){

    FixedDelay(host, 1);

    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;
      //
      // Loop for each rank
      //
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        if ((rank>0) & (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)))) continue; // only do rank=0 for lrdimm

        RestoreOffset(host, socket, ch, dimm, rank, 0, DdrLevel, TxVref);
      } // rank loop
    } // dimm loop

  }

} // JedecInitDdrChannel


/**

  Programs mode registers for DDR3 devices

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
void
ProgramMRSDDR3 (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank
  )
{
  UINT16              MR3;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

  //
  // Burst Length = 8, Read Burst Type = Interleave, Normal mode, DLL reset = yes, PPD Fast Exit
  //
  (*rankStruct)[rank].MR0 = MR0_RBT + MR0_DLL;

  //
  // PD Slow Exit (DLL off)
  //
  switch (host->setup.mem.MemPwrSave) {
    // Disabled
    case MEM_PWR_SAV_DISABLE:
    // APD Disabled, PPDF Enabled
    case MEM_PWR_SAV_FAST:
      (*rankStruct)[rank].MR0 |= MR0_PPD;
      break;

    // APD Disabled, PPDS Enabled
    case MEM_PWR_SAV_SLOW:
      (*rankStruct)[rank].MR0 &= ~MR0_PPD;
      break;

    // User Defined
    case MEM_PWR_SAV_USER:
      switch (host->setup.mem.ckeThrottling) {
        case CKE_MODE_OFF:
        case CKE_APD:
        case CKE_PPDF:
        case CKE_APD_PPDF:
          (*rankStruct)[rank].MR0 |= MR0_PPD;
          break;

        case CKE_PPDS:
        case CKE_APD_PPDS:
          (*rankStruct)[rank].MR0 &= ~MR0_PPD;
          break;
      } // switch host->setup.mem.ckeThrottling
      break;
  } // switch host->setup.mem.MemPwrSave

  //
  // Force fast exit for termination rank
  //
  if (IsTermRank(host, socket, ch, dimm, rank)) {
    (*rankStruct)[rank].MR0 |= MR0_PPD;
  }

  // Use PPDS until CmdClk training is complete
  if ((host->setup.mem.options & CMD_CLK_TRAINING_EN) && (host->nvram.mem.socket[socket].cmdClkTrainingDone == 0)) {
    (*rankStruct)[rank].MR0 &= ~MR0_PPD;
  }

  //
  // Set Write Recovery for autoprecharge
  //
  switch (GetClosestFreq(host, socket)) {
    case DDR_800:
      (*rankStruct)[rank].MR0 |= 0x400;
      //
      // 6 clocks
      //
      break;

    case DDR_1066:
      (*rankStruct)[rank].MR0 |= 0x800;
      //
      // 8 clocks
      //
      break;

    case DDR_1333:
      (*rankStruct)[rank].MR0 |= 0xA00;
      //
      // 10 clocks
      //
      break;

    case DDR_1600:
      (*rankStruct)[rank].MR0 |= 0xC00;
      //
      // 12 clocks
      //
      break;

    case DDR_1866:
      (*rankStruct)[rank].MR0 |= 0xE00;
      //
      // 14 clocks
      //
      break;

    default:
      (*rankStruct)[rank].MR0 |= 0x0;
      //
      // 16 clocks
      //
      break;
  }

  //
  // Set CAS Read latency
  //
  if((*channelNvList)[ch].common.nCL < 12) {
    (*rankStruct)[rank].MR0 |= ((*channelNvList)[ch].common.nCL - 4) << 4;
  } else {
    (*rankStruct)[rank].MR0 |= (((*channelNvList)[ch].common.nCL - 12) << 4) | BIT2;
  }

  //
  // DLL enable, Additive latency = 0, Write Leveling disabled, Output Buffer Enabled, Rtt_Nom,
  // Output Driver Impedance Control = 2 -> RZQ/7
  //
  (*rankStruct)[rank].MR1 = 2 | (*rankList)[rank].RttNom;

#ifdef  LRDIMM_SUPPORT
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {

    // LRDIMM uses SPD data for Driver Impedance Control
    (*rankStruct)[rank].MR1 = ((*rankStruct)[rank].MR1 & ~(BIT5 | BIT1)) |
          (((*dimmNvList)[dimm].spdLrBuf_MR1_2_RTT & BIT0) << 1) |
          (((*dimmNvList)[dimm].spdLrBuf_MR1_2_RTT & BIT1) << 4);
  } else
#endif  //LRDIMM_SUPPORT

  //
  // Enable tDQS if x4 and x8 DIMMs are mixed on this channel and this is a x8
  //
  if ((*dimmNvList)[dimm].x4Present == 0) {
    (*rankStruct)[rank].MR1 |= MR1_TDQS;
  }

  //
  // CAS Write latency and Rtt_WR
  //
  (*rankStruct)[rank].MR2 = (((*channelNvList)[ch].common.nWL - 5) << 3) | (*rankList)[rank].RttWr;

  //
  // Set the ASR bit approprietly
  //
  if ((*dimmNvList)[dimm].SPDThermRefsh & ASR) {
    (*rankStruct)[rank].MR2 |= MR2_ASR;
  }

  //
  // Set the SRT bit approprietly
  //
  else if ((*channelNvList)[ch].features & EXTENDED_TEMP) {
    (*rankStruct)[rank].MR2 |= MR2_SRT;
  }

  MR3 = 0;

  //
  // MR2
  //
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);

  //
  // MR3
  //
  WriteMRS (host, socket, ch, dimm, rank, MR3, BANK3);

  //
  // MR1
  //
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);

  //
  // MR0
  //
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);

} // ProgramMRSDDR3



// DDR4 CAS Latency Table, CL =     9,  10,   11,   12,   13,   14,   15,   16,   17,   18,
UINT16 DDR4CASLatencyTable[24] = {0x0, 0x4, 0x10, 0x14, 0x20, 0x24, 0x30, 0x34, 0x64, 0x40,
//  19,   20,   21,   22,   23,   24,     25,     26,     27,     28      29,     30,     31,     32
  0x70, 0x44, 0x74, 0x50, 0x60, 0x54, 0x1000, 0x1004, 0x1010, 0x1014, 0x1020, 0x1024, 0x1030, 0x1034};

/**

  Programs mode registers for DDR4 devices

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
void
ProgramMRSDDR4 (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank
  )
{
  UINT8               strobe;
  UINT16              MR6;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

  //
  // Burst Length = 8, Read Burst Type = Interleave, Normal mode, DLL reset = yes, PPD Fast Exit
  //
  (*rankStruct)[rank].MR0 = MR0_RBT + MR0_DLL;

  //
  // Set Write Recovery for autoprecharge
  //
  switch ((*channelNvList)[ch].common.nRTP) {
    case 5:
      (*rankStruct)[rank].MR0 |= 0x000;
      //
      // 10 clocks
      //
      break;

    case 6:
      (*rankStruct)[rank].MR0 |= 0x200;
      //
      // 12 clocks
      //
      break;

    case 7:
      (*rankStruct)[rank].MR0 |= 0x400;
      //
      // 14 clocks
      //
      break;

    case 8:
      (*rankStruct)[rank].MR0 |= 0x600;
      //
      // 16 clocks
      //
      break;

    case 9:
      (*rankStruct)[rank].MR0 |= 0x800;
      //
      // 18 clocks
      //
      break;

    case 10:
      (*rankStruct)[rank].MR0 |= 0xA00;
      //
      // 20 clocks
      //
      break;

    case 11:
      (*rankStruct)[rank].MR0 |= 0xE00;
      //
      // 22 clocks
      //
      break;

    case 12:
      (*rankStruct)[rank].MR0 |= 0xC00;
      //
      // 24 clocks
      //
      break;

    case 13:
      (*rankStruct)[rank].MR0 |= 0x2000;
      //
      // 26 clocks
      //
      break;

    default:
      (*rankStruct)[rank].MR0 |= 0xA00;
      //
      // 20 clocks
      //
      break;
  }

  //
  // Set CAS Read latency
  //
  (*rankStruct)[rank].MR0 |= DDR4CASLatencyTable[((*channelNvList)[ch].common.nCL - 9)];

  //
  // DLL enable, Additive latency = 0, Write Leveling disabled, Output Buffer Enabled, Rtt_Nom,
  // Output Driver Impedance Control = RZQ/7
  //
  (*rankStruct)[rank].MR1 = (*rankList)[rank].RttNom;

#ifdef LRDIMM_SUPPORT
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && ((*dimmNvList)[dimm].SPDSpecRev > 7)) {
    // LRDIMM uses SPD data for Driver Impedance Control
    (*rankStruct)[rank].MR1 &= ~(BIT2 | BIT1);
    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866)  {
      (*rankStruct)[rank].MR1 |= (((*dimmNvList)[dimm].SPDLrbufDramDs & (BIT1 + BIT0)) << 1);
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
      (*rankStruct)[rank].MR1 |= (((*dimmNvList)[dimm].SPDLrbufDramDs & (BIT3 + BIT2)) >> 1);
    } else {
      (*rankStruct)[rank].MR1 |= (((*dimmNvList)[dimm].SPDLrbufDramDs & (BIT5 + BIT4)) >> 3);
    }
  }
#endif // LRDIMM_SUPPORT

  //
  // DLL Enable
  //
  (*rankStruct)[rank].MR1 |= BIT0;

  //
  // Additive Latency - MR1 BIT[4:3]
  // 00b -> AL=0(Disable), 01b -> AL = CL-1, 10b -> AL = CL-2, 11b -> Reserved
  //
  if ((*channelNvList)[ch].common.nAL == ((*channelNvList)[ch].common.nCL - 1)) {
     (*rankStruct)[rank].MR1 |= BIT3;
  } else if ((*channelNvList)[ch].common.nAL == ((*channelNvList)[ch].common.nCL - 2)) {
     (*rankStruct)[rank].MR1 |= BIT4;
  } else if ((*channelNvList)[ch].common.nAL > 0) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Additive Latency invalid!\n"));
  }

  //
  // Enable tDQS if x4 and x8 DIMMs are mixed on this channel and this is a x8
  //
  if (((*channelNvList)[ch].features & X4_AND_X8_PRESENT) && !((*dimmNvList)[dimm].x4Present)) {
    (*rankStruct)[rank].MR1 |= MR1_TDQS;
  }

  //
  // CAS Write latency
  //
  if ((*channelNvList)[ch].common.nWL < 14) {
    (*rankStruct)[rank].MR2 = ((*channelNvList)[ch].common.nWL - 9) << 3;
  } else {
    (*rankStruct)[rank].MR2 = ((*channelNvList)[ch].common.nWL - (((*channelNvList)[ch].common.nWL - 14) / 2) - 10) << 3;
  }

  //
  // Rtt_WR
  //
  (*rankStruct)[rank].MR2 |= (*rankList)[rank].RttWr;

  //
  // All DDR4 DIMMs support ASR
  //
  (*rankStruct)[rank].MR2 |= MR2_ASR_DDR4;

  (*rankStruct)[rank].MR3 = 0;
  (*rankStruct)[rank].MR4 = 0;
  //
  // 2nCK preamble
  //
  if (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400) {
    if (host->setup.mem.writePreamble == PREAMBLE_2TCLK) {
      (*rankStruct)[rank].MR4 |= BIT12;
    }
    if (host->setup.mem.readPreamble == PREAMBLE_2TCLK) {
      (*rankStruct)[rank].MR4 |= BIT11;
    }
  }

  (*rankStruct)[rank].MR5 = (*rankList)[rank].RttPrk;

  //
  // tCCD_L
  //
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    (*rankStruct)[rank].MR6[strobe] |= (((GetTccd_L(host, socket, ch) - 4) & 7) << 10);
  } // strobe loop

  MR6 = CHIP_FUNC_CALL(host, GetVrefRange(host, (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex]));
#ifdef LRDIMM_SUPPORT
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {
    MR6 = (UINT8)(*rankList)[rank].lrbufTxVref[0];
  }
#endif
  MR6 |= (((GetTccd_L(host, socket, ch) - 4) & 7) << 10);

  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  //
  // Follow MRS programming order in the DDR4 SDRAM spec
  //
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR3, BANK3);
  //
  // The first write ensures VrefDQ Training Mode is disabled
  //
  WriteMRS (host, socket, ch, dimm, rank, MR6, BANK6);
  //
  // The second write enters VrefDQ Training Mode and sets the range
  //
  WriteMRS (host, socket, ch, dimm, rank, MR6 | BIT7, BANK6);
  //
  // The third write sets the VrefDQ value
  //
  WriteMRS (host, socket, ch, dimm, rank, MR6 | BIT7, BANK6);
  //
  // The fourth write exits VrefDq Training Mode
  //
  WriteMRS (host, socket, ch, dimm, rank, MR6, BANK6);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4, BANK4);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));   // execute stack of MRS commands and return to NORMAL_MRS

} // ProgramMRSDDR4

/**

  Programs mode registers for NVMDIMM devices

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
void
ProgramMRSAEP (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     dimm,
               UINT8     rank
               )
{
  UINT8                                 CASTemp;
  UINT8                                 CWLTemp;
  UINT8                                 tCLAdd;
  UINT8                                 tCWLAdd;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct ddrRank                        (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                     (*rankStruct)[MAX_RANK_DIMM];
  INT16                                 zeroOffset;

  channelNvList = GetChannelNvList(host, socket);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

  zeroOffset = 0;
  GetSetCmdVrefCore(host, socket, ch, dimm, DdrLevel, GSM_READ_ONLY, &zeroOffset);

  //
  // DLL reset = yes
  //
  (*rankStruct)[rank].MR0 = MR0_DLL;
  CHIP_FUNC_CALL(host, GetDdrtTiming(host, socket, ch, dimm, &CASTemp, &CWLTemp));
  CHIP_FUNC_CALL(host, GetDdrtTimingBufferDelay(host, socket, ch, dimm, &tCLAdd, &tCWLAdd));
  if ((CASTemp + tCLAdd) < 12){
    CASTemp = (12 - tCLAdd);
  }
  CASTemp = CASTemp - 9;
  if (CWLTemp < 14) {
    CWLTemp = CWLTemp - 9;
  } else {
    CWLTemp = (CWLTemp - ((CWLTemp - 14) / 2) - 10);
  }
  (*rankStruct)[rank].MR0 |= DDR4CASLatencyTable[CASTemp];

  //
  // DLL enable, Additive latency = 0, Write Leveling disabled, Output Buffer Enabled, Rtt_Nom,
  // Output Driver Impedance Control = RZQ/7
  //
  (*rankStruct)[rank].MR1 = (*rankList)[rank].RttNom;

  //
  // CAS Write latency
  //
  (*rankStruct)[rank].MR2 = (CWLTemp << 3);

  //
  // Enable NVMDIMM 2N mode
  //
  if ((*channelNvList)[ch].timingMode == TIMING_2N) {
    (*rankStruct)[rank].MR2 |= BIT6;
  }
  //
  // Rtt_WR
  //
  (*rankStruct)[rank].MR2 |= (*rankList)[rank].RttWr;

  (*rankStruct)[rank].MR3 = 0;
  (*rankStruct)[rank].MR4 = 0;

  //
  // 2nCK preamble
  //
  if (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400) {
    if (host->setup.mem.writePreamble == PREAMBLE_2TCLK) {
      (*rankStruct)[rank].MR4 |= BIT12;
    }
    if (host->setup.mem.readPreamble == PREAMBLE_2TCLK) {
      (*rankStruct)[rank].MR4 |= BIT11;
    }
  }
  (*rankStruct)[rank].MR5 = (*rankList)[rank].RttPrk;

  //
  // Follow MRS programming order in the DDR4 SDRAM spec
  //
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR3, BANK3);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4, BANK4);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

  CHIP_FUNC_CALL(host, FnvCWLWorkaround (host, socket, ch, dimm));
  if (CHIP_FUNC_CALL(host, isCmdVrefTrainingDone(host, socket))) {
    RestoreOffset(host, socket, ch, dimm, rank, zeroOffset, DdrLevel, CmdVref);
  }
} // ProgramMRSAEP


/**

  Writes DRAM mode registers

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)
  @param data  - Data for the MRS write
  @param bank  - Bank address for the MRS write

  @retval N/A

**/
void
WriteMRS (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT16   data,
  UINT8    bank
  )
{
  UINT16            orgData;
#ifdef  LRDIMM_SUPPORT
  UINT8             dr;
  UINT32            phyData;
#endif
  UINT8             orgBank;
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];

  orgData = data;
  orgBank = bank;
  dimmNvList = GetDimmNvList(host, socket, ch);

  //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
  //              "MRS%d data 0x%04X\n", bank, data));
  //
  // *** must be before the MRC command because of address swizzling bits in udimm's
  //
  switch (bank) {
  case BANK0:
    CHIP_FUNC_CALL(host, SetMR0Shadow (host, socket, ch, data & ~MR0_DLL));
    break;

  case BANK2:
    CHIP_FUNC_CALL(host, SetMR2Shadow (host, socket, ch, dimm, data));
    break;

  case BANK4:
    CHIP_FUNC_CALL(host, SetMR4Shadow (host, socket, ch, data));
    break;

  case BANK5:
    CHIP_FUNC_CALL(host, SetMR5Shadow (host, socket, ch, data));
    break;

  default:
    break;
  }

  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
    WriteMrsDdr4(host, socket, ch, dimm, rank, data, bank, A_SIDE);

    //
    // Write to the both sides of the DIMM if this is a buffered DIMM
    //
  if ((host->nvram.mem.dimmTypePresent == RDIMM) && (bank != 7) &&
      !(*dimmNvList)[dimm].aepDimmPresent) {
      WriteMrsDdr4(host, socket, ch, dimm, rank, data, bank, B_SIDE);
    }
    return;
  }
  //
  // Are the address and bank mappings mirrored?
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    data  = MirrorAddrBits (host, data);
    bank  = MirrorBankBits (host, bank);
  }

  CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, MRS_CMD, bank, data, 0));

#ifdef  LRDIMM_SUPPORT
  // Loop for physical ranks when writing MR1 in rank multiplication mode
  if (host->nvram.mem.socket[socket].channelList[ch].lrDimmPresent &&
      ((*dimmNvList)[dimm].lrRankMult > 1) &&
      (orgBank == BANK1) &&
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {

    // Disable MRS broadcast in the LR buffer
    // Set F0RC14 Register, bit-2 = 1
    WriteRcLrbuf(host, socket, ch, dimm, 0, (*dimmNvList)[dimm].lrBuf_F0RC14 | BIT2, 0, 14);

    // Loop for each physical rank in this logical rank
    for (dr = 0; dr < (*dimmNvList)[dimm].numDramRanks; dr++) {

      // Skip physical ranks that are not in this logical rank
      if ((dr ^ rank) & BIT0) continue;

      // If physical rank > 1 then clear the Rtt_nom field
      phyData = (UINT32)orgData;
      if (dr > 1) {

        // A[9,6,2] = 000b
        phyData = phyData & ~(BIT9 | BIT6 | BIT2);
      }

      // If odd rank, then do odd rank mirror
      if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
        phyData  = (UINT32)MirrorAddrBits (host, (UINT16)phyData);
        bank  = MirrorBankBits (host, orgBank);
      }

      // Use upper address encoding for physical rank MRS

      // Encode upper address bits according to LRDIMM memory buffer spec
      switch ((*dimmNvList)[dimm].lrRankMult) {
      case 2:
        // physical rank    logical rank   addr bit N[0]
        // 0                0              0
        // 2                0              1
        // 1                1              0
        // 3                1              1
        phyData = phyData | ((dr & BIT1) << ((((*dimmNvList)[dimm].actSPDSdramAddr >> 3) & 7) + 11));
        break;
      case 4:
        // physical rank    logical rank   addr bit N[1:0]
        // 0                0              00
        // 2                0              01
        // 4                0              10
        // 6                0              11
        // 1                1              00
        // 3                1              01
        // 5                1              10
        // 7                1              11
        phyData = phyData | ((dr & (BIT2 | BIT1)) << ((((*dimmNvList)[dimm].actSPDSdramAddr >> 3) & 7) + 11));
        break;
      default:
        RC_ASSERT(FALSE, ERR_DIMM_COMPAT, ERR_MISMATCH_DIMM_TYPE);
      }

      //
      // Issue the MRS command
      //
      CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, MRS_CMD, bank, phyData, 0));

    }
    // Enable MRS broadcast in the LR buffer
    // Set F0RC14 Register, bit-2 = 0
    WriteRcLrbuf(host, socket, ch, dimm, 0, (*dimmNvList)[dimm].lrBuf_F0RC14, 0, 14);
  }
#endif  //LRDIMM_SUPPORT

} // WriteMRS

/**

  Writes DRAM mode registers

  @param host  - Pointer to sysHost
  @param socket- Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)
  @param data  - Data for the MRS write
  @param bank  - Bank address for the MRS write

  @retval N/A

**/
void
WriteMrsDdr4 (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT16   data,
  UINT8    bank,
  UINT8    side
  )
{
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];

  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    FixedDelay(host, 10);
  }

  dimmNvList    = GetDimmNvList(host, socket, ch);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "DDR4 MRS write: bank 0x%02X, data 0x%04X\n", bank, data));
#endif
  //Trap any code error
  if ((side == B_SIDE) && (*dimmNvList)[dimm].aepDimmPresent) {
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_75);
  }

  //
  // Are the address and bank mappings mirrored?
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    data  = MirrorAddrBits (host, data);
    bank  = MirrorBankBits (host, bank);
  }

  //
  // Invert address/bank bits
  //
  if (side == B_SIDE) {
    data  = InvertAddrBits (data);
    bank  = InvertBankBits (bank);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "DDR4 MRS write after mirror and invert: bank 0x%02X, data 0x%04X\n", bank, data));
#endif
  CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, MRS_CMD, bank, data, 0));

  FixedDelay(host, 1);

} // WriteMrsDdr4

/**

  Mirrors address bits

  @param host  - Pointer to sysHost
  @param data  - Data for MRS write

  @retval dataNew - Mirrored data

**/
UINT16
MirrorAddrBits (
  PSYSHOST  host,
  UINT16    data
  )
{
  UINT16  dataNew;

  dataNew = data &~0x1F8;
  //
  // Swap of bits (3,4), (5,6), (7,8)
  //
  dataNew |= ((data & BIT3) << 1) | ((data & BIT4) >> 1) | ((data & BIT5) << 1) | ((data & BIT6) >> 1) | ((data & BIT7) << 1) | ((data & BIT8) >> 1);

  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
    //
    // Swap of address bits (11,13)
    //
    dataNew &= ~(BIT11 | BIT13);
    dataNew |= ((data & BIT11) << 2) | ((data & BIT13) >> 2);
  }

  return dataNew;
} // MirrorAddrBits

/**

  Mirrors bank address bits

  @param host    - Pointer to sysHost
  @param bank  - Bank address to be mirrored

  @retval bankNew - Mirrored bank address

**/
UINT8
MirrorBankBits (
  PSYSHOST  host,
  UINT8     bank
  )
{
  UINT8 bankNew = 0;

  //
  // Swap bits 0 and 1 of the bank address
  //
  bankNew |= ((bank & BIT0) << 1) | ((bank & BIT1) >> 1);

  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
    //
    // Swap bits 2 and 3 of the bank address
    //
    bankNew |= ((bank & BIT2) << 1) | ((bank & BIT3) >> 1);
  }

  return bankNew;
} // MirrorBankBits

/**

  Inverts address bits

  @param bank  - Address to be inverted

  @retval bankNew - Inverted address

**/
UINT16
InvertAddrBits (
  UINT16 data
  )
{
  return ((data ^ (BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9 | BIT11 | BIT13)) & 0xFFFF);

} // InvertAddrBits

/**

  Inverts bank address bits

  @param bank  - Bank address to be inverted

  @retval bankNew - Inverted bank address

**/
UINT8
InvertBankBits (
  UINT8 bank
  )
{
  return ((bank ^ (BIT0 | BIT1 | BIT2 | BIT3)) & 0xF);
} // InvertBankBits


/**

  Inverts bank address bits

  @param bank  - Bank address to be inverted

  @retval bankNew - Inverted bank address

**/
UINT8
InvertMprBankBits (
  UINT8 bank
  )
{
  return ((bank ^ (BIT3 | BIT2)) & 0xF);
} // InvertMprBankBits


/**

  Program register control words for RDIMMs

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
void
DoRegisterInit (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
  UINT8 controlWordData;
  UINT8 controlWordAddr;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  dimmNvList = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

#ifdef  LRDIMM_SUPPORT

  if ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {

    // Only init LRDIMM buffer for rank 0
    if (rank > 0) return;

    LrDimmRegisterInit(host, socket, ch, dimm, rank);
  } else

#endif
  {
    // Skip QR modules with only one register
    if ((rank == 2) && (((*dimmNvList)[dimm].dimmAttrib & 3) < 2))
      return;

    //
    // Get value to write to RC0
    //
    controlWordData = 2;

    //
    // Create Control Word address
    //
    controlWordAddr = 0;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    if ((((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_A) ||
        (((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_C) ||
        (((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_K) ||
        (((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_M)
        ) {
      //
      // Get value to write to RC1
      //
      controlWordData = 0x0C;
      //
      // Create Control Word address
      //
      controlWordAddr = 1;
      WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);
    }
    //
    // Get value to write to RC3
    //
    controlWordData = (*dimmNvList)[dimm].SPDControlWords[0] >> 4;
    //
    // Create Control Word address
    //
    controlWordAddr = 3;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    //
    // Get value to write to RC4
    //
    controlWordData = (*dimmNvList)[dimm].SPDControlWords[1] & 0x0F;
    //
    // Create Control Word address
    //
    controlWordAddr = 4;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    //
    // Get value to write to RC5
    //
    controlWordData = (*dimmNvList)[dimm].SPDControlWords[1] >> 4;
    //
    // Create Control Word address
    //
    controlWordAddr = 5;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    if ((((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_F) ||
        (((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_W) ||
        (((*dimmNvList)[dimm].SPDRawCard & 0x1F) == RAW_CARD_AB)) {
      //
      // Get value to write to RC8
      //
      controlWordData = 0x0A;
      //
      // Create Control Word address
      //
      controlWordAddr = 8;
      WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);
    }
    //
    // Get value to write to RC10
    //
    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_800) {
      controlWordData = 0;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1066) {
      controlWordData = 1;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1333) {
      controlWordData = 2;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
      controlWordData = 3;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
      controlWordData = 4;
    } else {
      controlWordData = 5;
    }
    //
    // Create Control Word address
    //
    controlWordAddr = 10;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    //
    // Get value to write to RC9
    //
    if ((*channelNvList)[ch].maxDimm == 1) {
      //
      // 1DPC
      //
      controlWordData = 0x0D;
    } else {
      controlWordData = 9;
    }
    //
    // Create Control Word address
    //
    controlWordAddr = 9;
    WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);

    if (host->nvram.mem.socket[socket].ddrVoltage & SPD_VDD_135) {
      //
      // Get value to write to RC11
      //
      controlWordData = 1;
      //
      // Create Control Word address
      //
      controlWordAddr = 11;
      WriteRC (host, socket, ch, dimm, rank, controlWordData, controlWordAddr);
    }
  }
} // DoRegisterInit


/*
Temporary backside drive strength settings until SPD bytes are defined:

Card type DRAM      Setting       RC03  RC04  RC05
1Rx8      4 or 5    light         0     0     0
2Rx8/1Rx4 8 or 10   moderate      5     5     5
2Rx4      16 or 20  strong        A     A     A
4Rx4      32 or 40  very strong   F     F     F
*/

UINT8   RegisterBacksideDriveStrength[4][2] = {
//  x8,  x4
  {  0,   5}, // 1R
  {  5, 0xA}, // 2R
  {  5, 0xA}, // 3R
  {  5, 0xA}  // 4R
};

/**

  Program register control words for RDIMMs

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
void
DoRegisterInitDDR4 (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
  UINT8 controlWordData;
  UINT8 tempData;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT16 rdimmFrequency = 0;
  INT16               piDelay;
  UINT16              minVal = 0xFF;
  UINT16              maxVal = 0;

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  //
  // Write to RC0
  //
  controlWordData = 0;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC00);

  //
  // Write to RC1
  //
  controlWordData = 0;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC01);

  //
  // Write to RC2
  //
  controlWordData = 0;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC02);

  if ((*dimmNvList)[dimm].SPDSpecRev >= 7) {
    //
    // Get CA and CS Signals Driver Characteristics from SPD byte 137 upper nibble
    //
    controlWordData = (*dimmNvList)[dimm].SPDODCtl >> 4;
  } else {
    controlWordData = RegisterBacksideDriveStrength[(*dimmNvList)[dimm].numDramRanks-1][(*dimmNvList)[dimm].x4Present];
  }
  //
  // Write to RC3
  //
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC03);

  if ((*dimmNvList)[dimm].SPDSpecRev >= 7) {
    //
    // Get ODT and CKE Signals Driver Characteristics from SPD byte 137 lower nibble
    //
    controlWordData = (*dimmNvList)[dimm].SPDODCtl & 0xF;
    tempData = (controlWordData << 2) & 0xC;
    controlWordData = (controlWordData >> 2) & 0x3;
    controlWordData |= tempData;
  } else {
    controlWordData = RegisterBacksideDriveStrength[(*dimmNvList)[dimm].numDramRanks-1][(*dimmNvList)[dimm].x4Present];
  }
  //
  // Write to RC4
  //
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC04);

  //s4987926 WA starts
  if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI ) {
    //Inphi test mode disable
    controlWordData = ((0x2)<< 5) | (0x0 << 4) | 0x0;           //RC4x = Func upper addr F7
    WriteRC (host, socket, ch, dimm, 0, controlWordData, 0x40); //RDIMM_RC4x);
    controlWordData = (UINT8)(0x02);                            //RC6x = Data
    WriteRC (host, socket, ch, dimm, 0, controlWordData, 0x60); //RDIMM_RC6x);
    WriteRC (host, socket, ch, dimm, 0, 0x5, 0x06);             //RDIMM_RCx6); //Issue CSR Write
  }
  //s4987926 WA ends

  if ((*dimmNvList)[dimm].SPDSpecRev >= 7) {
    //
    // Get Clock Driver Characteristics from SPD byte 138 lower nibble
    //
    controlWordData = (*dimmNvList)[dimm].SPDODCk & 0xF;
    tempData = (controlWordData << 2) & 0xC;
    controlWordData = (controlWordData >> 2) & 0x3;
    controlWordData |= tempData;
  } else {
    controlWordData = RegisterBacksideDriveStrength[(*dimmNvList)[dimm].numDramRanks-1][(*dimmNvList)[dimm].x4Present];
  }
  //
  // Write to RC5
  //
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC05);

  //
  // Write to RC4X(Set bits of reg to read (RC4X))
  //
  controlWordData = 0xf;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC4x);


  //
  // Write to RC8
  //
  //NOTE: code below assumes A17 is disabled.
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    // For 3DS, use the cidBitMap variable to enable the proper CID bits in the RCD
    if ((*channelNvList)[ch].cidBitMap & BIT2) {
      controlWordData = DA17Input_QxA17Output_disabled;
    } else if ((*channelNvList)[ch].cidBitMap & BIT1) {
      controlWordData = DA17Input_QxA17Output_disabled | QxC_1_0_enabled_QxC_2_disabled;
    } else if ((*channelNvList)[ch].cidBitMap & BIT0) {
      controlWordData = DA17Input_QxA17Output_disabled | QxC_2_1_disabled_QxC_0_enabled;
    } else {
      controlWordData = DA17Input_QxA17Output_disabled | QxC_2_0_disabled;
    }
  } else if ((*dimmNvList)[dimm].numDramRanks == 4) {
    // Enable C[1:0] because they are used as chip selects.
    controlWordData = 0x9;
  } else {
    // Disable C[2:0]
    controlWordData = 0xB;
  }
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC08);

  //
  // Write to RC0A
  //
  if (host->var.mem.socket[socket].hostRefreshStatus == 0) {
    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
      controlWordData = 0;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
      controlWordData = 1;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
      controlWordData = 2;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
      controlWordData = 3;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666) {
      controlWordData = 4;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_3200) {
      controlWordData = 5;
    }
    WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0A);

    //
    // Write to RC0B
    //
    controlWordData = CHIP_FUNC_CALL(host, CaVrefSelect(host));  // Enable external CA Vref.  This is a change from HSX launch collateral
    WriteRC(host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0B);
  }
  //
  // Write to RC0C
  //
  controlWordData = 0;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0C);

  //
  // Write to RC0E
  //
  controlWordData = 0x0;
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0E);

  //
  // Write to RC0F
  //
  if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400 || ((*channelNvList)[ch].dimmRevType == RCD_REV_2)) {
    controlWordData = 0x0;
  } else {
    controlWordData = 0x1;
  }
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && (*channelNvList)[ch].ddrtEnabled && host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
    controlWordData = 0x1;
  }
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0F);

  //
  // Write to RC1x
  //
  if (!(CHIP_FUNC_CALL(host, CaVrefSelect(host)) & 0x8)) {
    controlWordData = (UINT8)((*channelNvList)[ch].dimmList[dimm].rcVref >> (8 * DdrLevel)) & 0xff;
    WriteRC(host, socket, ch, dimm, rank, controlWordData, RDIMM_RC1x);
  }

  //
  // Write to RC3x
  //
  if (host->var.mem.socket[socket].hostRefreshStatus == 0) {

    rdimmFrequency = host->nvram.mem.socket[socket].ddrFreqMHz * 2;
    if (rdimmFrequency < 1241) rdimmFrequency = 1241;
    if (rdimmFrequency > 3200) rdimmFrequency = 3200;
    controlWordData = (UINT8)((rdimmFrequency - 1241) / 20);
    //
    // TI DDR4 Register Errata Sheet v1.8, CAB4A3 Errata #3
    //
    if (!(((*dimmNvList)[dimm].SPDRegVen == MFGID_TI) && ((*dimmNvList)[dimm].SPDRegRev == 0xA))) {
      WriteRC(host, socket, ch, dimm, rank, controlWordData, RDIMM_RC3x);
    }
  }
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

  //
  // Restore backside Cmd, Ctl, Ck delays
  //
  if (IsDdr4RegisterRev2(host, socket, ch, dimm)) {
    piDelay = 0;
    GetSetClkDelayCore (host, socket, ch, dimm, LrbufLevel, 0, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay);
    GetSetCtlGroupDelayCore (host, socket, ch, dimm, LrbufLevel, CtlAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minVal, &maxVal);
    GetSetCmdGroupDelayCore (host, socket, ch, dimm, LrbufLevel, CmdAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minVal, &maxVal);
  }
} // DoRegisterInitDDR4

void
DoRegisterInitAep (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch,
                   UINT8    dimm
                   )
{
  //
  // Write to RC0E
  // BIT 2 - Alert pulsewidth according to table 38...
  //

  CHIP_FUNC_CALL(host, WriteRCFnv (host, socket, ch, dimm, BIT2, RDIMM_RC0E));

}

void
DoRegisterInitDDR4_CKE (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
/*++

  Program register control words for RDIMMs

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)

  @retval N/A

--*/
{
  UINT8  controlWordData;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
#ifdef  LRDIMM_SUPPORT
#ifdef  MONTAGE_LRBUF_WA
  UINT8 i;
  UINT8 partNumberMatch;
  UINT16 dateCode;
  char SKHynix4Rx4PartNum[SPD_MODULE_PART_DDR4 + 1] = "HMA84GL7MMR4N-TFT1  ";
#endif
#endif

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  //
  // Write to RC9
  //
  if ((*channelNvList)[ch].maxDimm > 1) {
    controlWordData = 0x8;     // 10xx to disable IBT-off during power down
  } else {
    controlWordData = 0xC;     // 11xx to enable IBT-off during power down
  }
#ifdef MEM_NVDIMM_EN
  if (((*dimmNvList)[dimm].numRanks == 1) && ((*dimmNvList)[dimm].nvDimmType == 1))  {
    controlWordData |= 3;
  }
#endif
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC09);

  //
  // Write to RC0D
  //
  controlWordData = 0;
  if ((*dimmNvList)[dimm].SPDAddrMapp) {
    // Enable address mirroring
    controlWordData |= BIT3;
  }
  if ((*dimmNvList)[dimm].numRanks == 4 && ((*channelNvList)[ch].encodedCSMode != 2)) {
    controlWordData |= BIT0;
  }
  //
  // IDT B0 sighting 200001: Use LRDIMM mode
  //
  if (!(((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].SPDRegRev == IDT_SPD_REGREV_B0))) {
    if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) == 0) {
      // Indicate this is an RDIMM
      controlWordData |= BIT2;
    }
  }
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) && ((*channelNvList)[ch].encodedCSMode == 1)) {
    controlWordData |= (BIT1 + BIT0);
  }
  WriteRC (host, socket, ch, dimm, rank, controlWordData, RDIMM_RC0D);

#ifdef  LRDIMM_SUPPORT
#ifdef  MONTAGE_LRBUF_WA
  //
  //M88DDR4 RCD/DB Sighting Report,
  // Montage MRC code w/a for SKHynix 4R x4 LRDIMM PCB issue
  //
/* Montage note: 07/03/2013
     The following two lines are used to pull-in the output clock Y of RCD by T/8 offset.
     This is to help fix the QCA/Y hold time margin issue reported by Hynix.*/
  if ((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE &&
      CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {

    partNumberMatch = 1;
    for (i = 0; i < SPD_MODULE_PART_DDR4; i++) {
      if((*dimmNvList)[dimm].SPDModPartDDR4[i] != SKHynix4Rx4PartNum[i]) {
        partNumberMatch = 0;
        break;
      }
    }
    dateCode = (*dimmNvList)[dimm].SPDModDate >> 8;
    dateCode |= (*dimmNvList)[dimm].SPDModDate << 8;
    if ((partNumberMatch) && (dateCode < 0x1330)) {
      controlWordData = 0x07;
      WriteLrbufSmb(host, socket, ch, dimm, sizeof(UINT8), (3<<12)|0x25, (UINT32 *)&controlWordData);
    }
  }

#endif //MONTAGE_LRBUF_WA
#endif //LRDIMM_SUPPORT
} // DoRegisterInitDDR4_CKE

/**

  Writes a nibble to RDIMM/LRDIMM register control words

  @param host            - Pointer to sysHost
  @param socket            - Socket Id
  @param ch              - Channel number (0-based)
  @param rank            - Rank number (0-based)
  @param controlWordData - Control Word data (a nibble)
  @param controlWordAddr - Control Word Address

  @retval N/A

**/
void
WriteRC (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     controlWordData,
  UINT8     controlWordAddr
  )
{
  UINT8             mrs_ba;
  UINT16            mrs_addr;
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];

  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "Write RC%02x = 0x%02x\n", controlWordAddr, controlWordData));

  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((rank >= 2) && (((*dimmNvList)[dimm].dimmAttrib & 3) < 2)) {
      rank = rank & 0xFC;
    } else {
      rank = rank & 0xFE;
    }

    if (controlWordAddr >= 0x10) {
      WriteMRS(host, socket, ch, dimm, rank, ((controlWordAddr & 0xF0) << 4) | controlWordData, BANK7);
      (*dimmNvList)[dimm].rcxCache[controlWordAddr >> 4] = controlWordData;
    } else {
      WriteMRS(host, socket, ch, dimm, rank, (controlWordAddr << 4) | controlWordData, BANK7);
      (*dimmNvList)[dimm].rcCache[controlWordAddr] = controlWordData;
    }
  } else {
#ifdef  LRDIMM_SUPPORT
    // Check if RCW's have been disabled
     if (host->nvram.mem.socket[socket].wa & WA_NO_MORE_RCW && host->nvram.mem.socket[socket].channelList[ch].lrDimmPresent) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                    "RC Write after lockdown -> RC%x CS %x data %x\n",
                     controlWordAddr, rank, controlWordData));
      OutputError (host, ERR_MRC_STRUCT, 0, socket, ch, dimm, rank);
    }
#endif  // LRDIMM_SUPPORT

    mrs_addr  = (controlWordAddr & (BIT0 + BIT1 + BIT2)) | ((controlWordData & (BIT0 | BIT1)) << 3);
    mrs_ba    = ((controlWordAddr >> 1) & BIT2) | ((controlWordData >> 2) & (BIT0 | BIT1));

    //
    // Issue the RC command
    //
    CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, RCW_CMD, mrs_ba, mrs_addr, 0));

    //
    // Wait at least tMRD
    //
    FixedDelay(host, 50);
  } // if DDR4

} // WriteRC

/**

    MPR Write Pattern Rank DDR4

    @param host    - Pointer to SysHost
    @param socket  - Processor Socket
    @param ch      - Channel on socket
    @param dimm    - DIMM on Channel
    @param rank    - Rank on DIMM
    @param side    - Side
    @param pattern - Pattern

    @retval N/A

**/
void
MprWritePatternRankDdr4 (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     side,
  UINT16    pattern
  )
{
  UINT16            addr;
  UINT8             bank;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].aepDimmPresent) {

    ProgramDFnvMprPattern(host, socket, ch, dimm, pattern);

  } else {

  //
  // Issue write command with A[7:0] = desired MPR pattern
  // A7 = UI0, A6 = UI1, ..., A0 = UI7
  // BA[1:0] = MPR location 0
  //

  //
  // Create value to write to DDR3CMD
  //
    addr = pattern & 0xFF;
  bank = 0;

#ifdef  PRINT_FUNC
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "DDR4 MPR write: bank 0x%02X, data 0x%04X\n", bank, addr));
#endif

  //
  // Are the address and bank mappings mirrored?
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    addr  = MirrorAddrBits (host, addr);
    bank  = MirrorBankBits (host, bank);
  }
  //
  // Invert address/bank bits
  //
  if (side == B_SIDE) {
    addr  = InvertAddrBits (addr);
    bank  = InvertMprBankBits (bank);
  }

#ifdef  PRINT_FUNC
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "DDR4 MPR write after mirror and invert: bank 0x%02X, data 0x%04X\n", bank, addr));
#endif
  CHIP_FUNC_CALL(host, WriteCADBCmd(host, socket, ch, dimm, rank, WR_CMD, bank, addr, 0));

  FixedDelay(host, 1);
  }
} // MprWritePatternRankDdr4

#ifdef SERIAL_DBG_MSG
/**

  Prints all DRAM, Register and Buffer CSR data from all DIMMs on the socket.
  Note: this function assumes that the read training has been successfully
  completed in order to read DRAM and Buffer data.

  @param host    - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
DumpDimmStateDdr4 (
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8               mprPage, mprLoc;
  UINT8               MprData[MAX_STROBE];
  UINT32              lrbufData = 0;
  UINT16              i;
#ifdef LRDIMM_SUPPORT
  UINT8               bcwFunc = 0;
  UINT8               bcwByte, bcwNibble;
#endif

  channelNvList = GetChannelNvList(host, socket);
  maxStrobe = 9;
  rcPrintf ((host, "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

      if (host->nvram.mem.dimmTypePresent == RDIMM) {

        if (!(*dimmNvList)[dimm].aepDimmPresent) {

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DDR4 Reg CW data:\n"));
          getPrintFControl(host);
          rcPrintf ((host, "SMbus Byte: 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F\n"));

          for (i = 0; i < 0x20; i+=4) {
            if(SUCCESS == ReadLrbufSmb (host, socket, ch, dimm, i, &lrbufData)) {

              if (i % 16 == 0) {
                rcPrintf ((host, "\n      0x%02x:", i));
              }
              rcPrintf ((host, " 0x%02x 0x%02x 0x%02x 0x%02x",
                lrbufData & 0xFF, (lrbufData >> 8) & 0xFF, (lrbufData >> 16) & 0xFF, (lrbufData >> 24) & 0xFF));
            } else {
              rcPrintf ((host, "Failure: DDR4 Reg SMBus RD at off 0x%03x\n", i));
            }
          }
          rcPrintf ((host, "\n\n"));
          releasePrintFControl(host);
        }
#ifdef LRDIMM_SUPPORT
        //debug only!!!
        //disableAllMsg(host);
        //WriteLrbuf(host, socket, ch, dimm, 0, 0x1, LRDIMM_F0, LRDIMM_BC00);
        //WriteLrbuf(host, socket, ch, dimm, 0, 0x2, LRDIMM_F0, LRDIMM_BC04);
        //WriteLrbuf(host, socket, ch, dimm, 0, 0x5, LRDIMM_F0, LRDIMM_BC05);
        //restoreMsg(host);

        if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))) {

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DDR4 Buffer CW data:\n"));
          getPrintFControl(host);
          rcPrintf ((host, "DQ Index:         0     8     16    24    32    40    48    56    64\n"));

          for (bcwFunc = 0; bcwFunc < 8; bcwFunc++) {
            for (bcwByte = 0; bcwByte < 16; bcwByte++) {

              if (bcwFunc == 0 && bcwByte == 0) {

                // per nibble
                for (bcwNibble = 0; bcwNibble < 16; bcwNibble+=2) {

                  // Note: IDT buffer always returns nibble[1:0] regardless of the nibble address or auto-increment setting
                  disableAllMsg(host);
                  CHIP_FUNC_CALL(host, ReadBcwDimmDdr4 (host, socket, ch, dimm, bcwFunc, bcwByte, bcwNibble, MprData));
                  restoreMsg(host);
                  rcPrintf ((host, "F%d, Nibble 0x%02x: ", bcwFunc, bcwNibble));

                  for (strobe = 0; strobe < maxStrobe; strobe++) {
                    rcPrintf ((host, " 0x%02x ", MprData[strobe]));
                  }
                  rcPrintf ((host, "\n"));

                } //bcwNibble
              } else {

                // per byte
                disableAllMsg(host);
                CHIP_FUNC_CALL(host, ReadBcwDimmDdr4 (host, socket, ch, dimm, bcwFunc, bcwByte, 0, MprData));
                restoreMsg(host);
                rcPrintf ((host, "F%d, Byte   0x%02x: ", bcwFunc, bcwByte));

                for (strobe = 0; strobe < maxStrobe; strobe++) {
                  rcPrintf ((host, " 0x%02x ", MprData[strobe]));
                }
                rcPrintf ((host, "\n"));

              } //if bcwByte==0
            } //for bcwByte
          } //for bcwFunc
          rcPrintf ((host, "\n"));
          releasePrintFControl(host);

        }//LRDIMM
#endif //LRDIMM_SUPPORT
      }//RDIMM

      if (!(*dimmNvList)[dimm].aepDimmPresent) {

        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DDR4 DRAM MPR data:\n"));
        getPrintFControl(host);
        if ((*dimmNvList)[dimm].x4Present) {
          rcPrintf ((host, "DQ Index:       0    4    8    12   16   20   24   28   32   36   40   44   48   52   56   60   64   68\n"));
        } else {
          rcPrintf ((host, "DQ Index:       0         8         16        24        32        40        48        56        64\n"));
        }

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          for (mprPage = 0; mprPage < 4; mprPage++) {
            for (mprLoc = 0; mprLoc < 4; mprLoc++) {

              disableAllMsg(host);
              CHIP_FUNC_CALL(host, ReadMprRankDdr4(host, socket, ch, dimm, rank, mprPage, mprLoc, MprData));
              restoreMsg(host);
              rcPrintf ((host, "R%d MPR P%d, L%d: ", rank, mprPage, mprLoc));

              for (strobe = 0; strobe < maxStrobe; strobe++) {
                rcPrintf ((host, " 0x%02x", MprData[strobe]));

                if ((*dimmNvList)[dimm].x4Present) {
                  rcPrintf ((host, " 0x%02x", MprData[strobe+9]));
                } else {
                  rcPrintf ((host, "     "));
                }
              }//strobe
              rcPrintf ((host, "\n"));

            }//mprLoc
          }//mprPage
        }//rank
        rcPrintf ((host, "\n"));
        releasePrintFControl(host);
      }
    }//dimm
  }//ch
} //DumpDimmStateDdr4
#endif // SERIAL_DBG_MSG

/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param mprMode - Either 0 for Disable or MR3_MPR to Enable

  @retval Supported DDR frequency

**/
void
SetRankMprDdr4 (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     side,
  UINT8     mprPage,
  UINT16    mprMode
  )
{

  UINT16            addr;
  UINT8             bank;
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  rankStruct    = GetRankStruct(host, socket, ch, dimm);

  if (mprMode) {
    addr = (*rankStruct)[rank].MR3 | MR3_MPR | (mprMode & BIT11) | mprPage;
    bank = BANK3;
  } else {
    addr = (*rankStruct)[rank].MR3;
    bank = BANK3;
  }

  WriteMrsDdr4(host, socket, ch, dimm, rank, addr, bank, side);

} // SetRankMprDdr4

void
SetRankMPRPatternDA (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT16    mprMode,
  UINT16    pattern
  )
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].aepDimmPresent) {
    CHIP_FUNC_CALL(host, ProgramDAFnvMprPattern(host, socket, ch, dimm, pattern));
    CHIP_FUNC_CALL(host, SetRankDAMprFnv(host, socket, ch, dimm, mprMode));
  }
}

/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param mprMode - Either 0 for Disable or MR3_MPR to Enable

  @retval N/A

**/
void
SetRankMPRPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT16    mprMode,
  UINT16    pattern
  )
{
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    //
    // MR3 - Turn on/off MPR pattern
    //
    WriteMRS (host, socket, ch, dimm, rank, mprMode, BANK3);
  } else {
    if (mprMode) {
      SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, mprMode);
      MprWritePatternRankDdr4 (host, socket, ch, dimm, rank, A_SIDE, pattern);
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        CHIP_FUNC_CALL(host, SetRankDMprFnv (host, socket, ch, dimm, mprMode));
      } else if (host->nvram.mem.dimmTypePresent == RDIMM) {
        SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, 0);
        SetRankMprDdr4 (host, socket, ch, dimm, rank, B_SIDE, 0, mprMode);
        MprWritePatternRankDdr4 (host, socket, ch, dimm, rank, B_SIDE, pattern);
        SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, mprMode);
      }
    } else {
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        CHIP_FUNC_CALL(host, SetRankDMprFnv (host, socket, ch, dimm, mprMode));
      }
      SetRankMprDdr4 (host, socket, ch, dimm, rank, A_SIDE, 0, mprMode);
      if ((host->nvram.mem.dimmTypePresent == RDIMM) && !(*dimmNvList)[dimm].aepDimmPresent) {
        SetRankMprDdr4 (host, socket, ch, dimm, rank, B_SIDE, 0, mprMode);
      }
    }
    //
    // Fixup roundtrip when turning MPR on/off
    //
    CHIP_FUNC_CALL(host, AdjustRoundtripForMpr(host, socket, ch, dimm, rank, mprMode));
  }
} // SetRankMPRPattern


/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param mprMode - Either 0 for Disable or MR3_MPR to Enable

  @retval N/A

**/
void
SetRankMPR (
           PSYSHOST  host,
           UINT8     socket,
           UINT8     ch,
           UINT8     dimm,
           UINT8     rank,
           UINT8     mprMode
           )
{
  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    SetRankMPRPattern (host, socket, ch, dimm, rank, mprMode, 0x33a5);
  } else {
    SetRankMPRPattern (host, socket, ch, dimm, rank, mprMode, 0x3355);
  }
} // SetRankMPR


/**

  Enables or Disables the MPR pattern in all DIMMs/Channels/Ranks

  @param host    - Pointer to sysHost
  @param socket    - Socket Id
  @param mprMode - Either 0 for Disable or MR3_MPR to Enable

  @retval N/A

**/
void
SetAllRankMPR (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     mprMode
  )
{
  UINT8                               ch;
  UINT8                               dimm;
  UINT8                               rank;
  struct channelNvram                 (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  //
  // Set MPR pattern for all ranks
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        SetRankMPR (host, socket, ch, dimm, rank, mprMode);
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = mprMode;
        if (mprMode) {
          host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentMpr0Pattern = 0x55;
        }
#endif
      } // ch loop
    } // rank loop
  } // dimm loop
} // SetAllRankMPR


void
IndirectRCWrite (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     controlWordData,
  UINT8     controlWordFunc,
  UINT8     controlWordAddr
  )
/*++

  Writes a nibble or byte to DDR4 Register Control using the indirect method

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param rank            - Rank number (0-based)
  @param controlWordData - Control Word data (a nibble or byte)
  @param controlWordAddr - Control Word Address

  @retval N/A

--*/
{

  if (controlWordAddr >= 0x10) {
    //
    // Select the control word
    //
    WriteRC (host, socket, ch, dimm, 0, (controlWordFunc << 5) | (controlWordAddr >> 4), RDIMM_RC4x);

    //
    // Write the data
    //
    WriteRC (host, socket, ch, dimm, 0, controlWordData, RDIMM_RC6x);

  } else {
    //
    // Select the control word
    //
    WriteRC (host, socket, ch, dimm, 0, controlWordFunc << 5, RDIMM_RC4x);

    //
    // Write the data
    //
    WriteRC (host, socket, ch, dimm, 0, (controlWordAddr << 4) | controlWordData, RDIMM_RC6x);
  }

  //
  // Issue the write command
  //
  WriteRC (host, socket, ch, dimm, 0, 5, RDIMM_RC06);

} // IndirectRCWrite


UINT8
IsBacksideCmdMarginEnabled (
                    PSYSHOST host,
                    UINT8    socket
)
{
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram(*dimmNvList)[MAX_DIMM];

  if ((host->setup.mem.enableBacksideRMT) && (host->setup.mem.enableBacksideCMDRMT)){
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if (IsDdr4RegisterRev2(host, socket, ch, dimm) || ((*dimmNvList)[dimm].aepDimmPresent && (host->setup.mem.enableNgnBcomMargining))) {
          return 1;
        }
      } // dimm loop
    } // ch loop
    return 0; // if all DIMMs dont support it, return 0
  }
  return 0;
} // IsBacksideCmdMarginEnabled

UINT8
IsDdr4RegisterRev2 (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm
)
{
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->nvram.mem.dimmTypePresent == RDIMM) && ((*dimmNvList)[dimm].dimmAttrib & BIT4)) {
    return 1;
  }
  return 0;
} // IsDdr4RegisterRev2

#ifdef  LRDIMM_SUPPORT

void
DoLrbufInitDDR4 (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
/*++

  Program buffer control words for LRDIMMs

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)

  @retval N/A

--*/
{
  UINT8 controlWordData;
  UINT8 controlWordAddr;
  UINT8 fn;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  UINT8 rankBitMask = 0xE;
  UINT8 backsideRank;
  UINT8 *controlWordDataPtr;
  UINT16 lrdimmFrequency = 0;

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);
  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                "DDR4 LRDIMM Buffer Init\n"));

  // Inphi A1 W/A
  if (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1)) {

    CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
    WriteLrbuf(host, socket, ch, dimm, 0, 0x01, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x01, 0x4, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x30, LRDIMM_F6, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x01, 0x0, LRDIMM_BC0A);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x02, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x00, 0x2, LRDIMM_BCEx);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x00, 0x4, LRDIMM_BC1x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xB1, 0x1, LRDIMM_BCFx);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x03, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x80, 0x1, LRDIMM_BC8x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x1B, 0x1, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x00, LRDIMM_F7, LRDIMM_BC4x);
    CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "INPHI A1\n"));
  }

  // Montage Gen2 LRBUF W/A
  if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE)&& ((*dimmNvList)[dimm].lrbufGen == 0x01) && ((*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_GEN2_A0)) {

    CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x23, LRDIMM_F7, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x6d, LRDIMM_F7, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x22, LRDIMM_F7, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x0d, LRDIMM_F7, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x21, LRDIMM_F7, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x05, LRDIMM_F7, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x20, LRDIMM_F7, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x05, LRDIMM_F7, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC5x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x37, LRDIMM_F7, LRDIMM_BC6x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
    WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
    CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

    FixedDelay(host,1);
    WriteLrbuf(host, socket, ch, dimm, 0, 0x01, 0x2, LRDIMM_BCCx);
    FixedDelay(host,1);
  }

  // Montage Gen2 LRBUF W/A
  if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE) && ((*dimmNvList)[dimm].lrbufGen == 0x01) &&
     (((*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_GEN2_A0) || ((*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_GEN2_A1))){

    for (fn = 4; fn<8; fn++) {
      WriteLrbuf(host, socket, ch, dimm, 0, 0x77, fn, LRDIMM_BCCx);
      WriteLrbuf(host, socket, ch, dimm, 0, 0x77, fn, LRDIMM_BCDx);
      WriteLrbuf(host, socket, ch, dimm, 0, 0x77, fn, LRDIMM_BCEx);
      WriteLrbuf(host, socket, ch, dimm, 0, 0x77, fn, LRDIMM_BCFx);
    }
  }

  // Initialize the function control nibble to 0

  WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
  (*dimmNvList)[dimm].rcLrFunc = 0;
  //
  // Write to BC00
  //
#ifdef COSIM_EN
  controlWordData = 0;
#else
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC00;
#endif
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC00);
  //
  // Write to BC01
  //
#ifdef COSIM_EN
  controlWordData = 0;
#else
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC01;
#endif
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC01);
  //
  // Write to BC02
  //
#ifdef COSIM_EN
  controlWordData = 0;
#else
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC02;
#endif
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC02);
  //
  // Write to BC03
  //
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC03;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC03);
  //
  // Write to BC04
  //
#ifdef COSIM_EN
  controlWordData = 0;
#else
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC04;
#endif
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC04);
  //
  // Write to BC05
  //
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC05;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC05);
  //
  // Write to BC07
  //
  //rank present bit mask
  switch ((*dimmNvList)[dimm].numDramRanks) {
    case 1:
      rankBitMask = 0xE;
      break;

    case 2:
      rankBitMask = 0xC;
      break;

    case 4:
      rankBitMask = 0x0;
      break;
  }
  controlWordData = rankBitMask;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC07);
  //
  // Write to BC09
  //
  controlWordData = 8;  //CKE Power Down Mode Enable
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC09);
  //
  // Write to BC0A
  //
  // LRDIMM Set Frequency
  controlWordData = LRDIMM_BC0A_DDR4_1600;
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_1600)  {
    controlWordData = LRDIMM_BC0A_DDR4_1866;
  }
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_1866)  {
    controlWordData = LRDIMM_BC0A_DDR4_2133;
  }
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_2133)  {
    controlWordData = LRDIMM_BC0A_DDR4_2400;
  }
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_2400)  {
    controlWordData = LRDIMM_BC0A_DDR4_2666;
  }
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_2666)  {
    controlWordData = LRDIMM_BC0A_DDR4_3200;
  }
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC0A);

  //if not IDT 0xA0, write F0 BC6x
  if (!(((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) &&
       ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_DB_RID_A0))) {
    //
    // Write F0 BC6x
    //
    lrdimmFrequency = host->nvram.mem.socket[socket].ddrFreqMHz * 2;
    if (lrdimmFrequency < 1241) lrdimmFrequency = 1241;
    if (lrdimmFrequency > 3200) lrdimmFrequency = 3200;
    controlWordData = (UINT8) ((lrdimmFrequency - 1241)/20);
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC6x);
  }
  //
  // Write to BC0B
  //
  controlWordData = 0;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC0B);
  //
  // Write to BC0C
  //
  WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
  (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;
  //
  // Write to BC0E
  //
  WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0E);
  //
  // Write to F0, BC1x
  //
  controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
  //
  // program LRDIMM Buffer Training Control Word, Host Interface VrefDQ Training Range
  //
  controlWordData = CHIP_FUNC_CALL(host, GetVrefRange(host, (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex]));
  // (take vref range select bit 6 and put it in the LRDIMM buffer register bit 1)
  //NOTE: the backside DRAM range is written to range 1...ok for now, but may need to change later
  WriteLrbuf(host, socket, ch, dimm, 0, (((controlWordData & BIT6) >> 5) | BIT0), LRDIMM_F6, LRDIMM_BC4x);
  //
  // program LRDIMM Host Interface VREF Control Word
  //
  WriteLrbuf(host, socket, ch, dimm, 0, (controlWordData & 0x3F), LRDIMM_F5, LRDIMM_BC5x);
  //
  // program LRDIMM DRAM Interface VREF Control Word
  //
  WriteLrbuf(host, socket, ch, dimm, 0, (*rankList)[rank].lrbufRxVref[0] - DB_DRAM_VREF_RANGE2_OFFSET, LRDIMM_F5, LRDIMM_BC6x);

  for (backsideRank = 0; backsideRank < (*dimmNvList)[dimm].numDramRanks; backsideRank++) {
    //
    // Write F[3:0]BC2x/F[3:0]BC3x
    //
    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[0]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC2x, ALL_DATABUFFERS);

    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[9]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC3x, ALL_DATABUFFERS);
    //
    // Write F[3:0]BC4x/F[3:0]BC5x
    //
    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[0]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC4x, ALL_DATABUFFERS);

    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[9]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC5x, ALL_DATABUFFERS);
    //
    // Write F[3:0]BC8x/F[3:0]BC9x
    //
    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[0]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC8x, ALL_DATABUFFERS);

    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[9]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BC9x, ALL_DATABUFFERS);
    //
    // Write F[3:0]BCAx/F[3:0]BCBx
    //
    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[0]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BCAx, ALL_DATABUFFERS);

    controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[9]);
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, backsideRank, LRDIMM_BCBx, ALL_DATABUFFERS);

    //
    // Inphi A1 WA
    //
    if (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
        ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1)) {
      InphiPIWA(host, socket, ch, dimm);
    }

    //if not IDT 0xA0, write FxBCCx, FxBCDx, FxBCEx, and FxBCFx
    if (!(((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) &&
         ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_DB_RID_A0))) {
      // F0BCCx for backside rank 0, F1BCCx for backside rank 1, F0BCEx for backside rank 2, and F1BCEx for backside rank 3
      controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCCxEx[0]);
      controlWordAddr = LRDIMM_BCCx + ((backsideRank >> 1) * 0x20);
      WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, (backsideRank & BIT0), controlWordAddr, ALL_DATABUFFERS);

      // F0BCDx for backside rank 0, F1BCDx for backside rank 1, F0BCFx for backside rank 2, and F1BCFx for backside rank 3
      controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCDxFx[0]);
      controlWordAddr = LRDIMM_BCDx + ((backsideRank >> 1) * 0x20);
      WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, (backsideRank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
    }
  }
  //
  // Write to BC07 Again Per IDT DDR4 Data Buffer A0 sightings report version 2013_01_21_1349
  //sighting number 100002
  //
  //rank present bit mask
  switch ((*dimmNvList)[dimm].numDramRanks) {
    case 1:
      rankBitMask = 0xE;
      break;

    case 2:
      rankBitMask = 0xC;
      break;

    case 4:
      rankBitMask = 0x0;
      break;
  }
  controlWordData = rankBitMask;
  WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC07);

} // DoLrbufInitDDR4

//---------------------------------------------------------------------------
void
WriteBC (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     controlWordData,
  UINT8     controlWordFunc,
  UINT8     controlWordAddr
  )
/*++

  Writes a nibble or byte to DDR4 Buffer Control

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param rank            - Rank number (0-based)
  @param controlWordData - Control Word data (a nibble or byte)
  @param controlWordAddr - Control Word Address

  @retval N/A

--*/
{

  if ((controlWordAddr >= 0x10) || (controlWordFunc != 0)) {
    WriteMRS (host, socket, ch, dimm, rank, BIT12 | ((controlWordAddr & 0xF0) << 4) | controlWordData, BANK7);
  } else {
    WriteMRS (host, socket, ch, dimm, rank, BIT12 | (controlWordAddr << 4) | controlWordData, BANK7);
  }

} // WriteBC

//---------------------------------------------------------------------------
void
WriteRcLrbuf (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT8    controlWordData,
  UINT8    controlWordFunc,
  UINT8    controlWordAddr
  )
/*++

  Writes a nibble to DDR3 LRDIMM register control words

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param controlWordData - Control Word data
  @param controlWordFunc - Function index in the LR buffer
  @param controlWordAddr - Control Word Address

  @retval N/A

--*/
{
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    // Write LR function index only if needed
    if (controlWordFunc != (*channelNvList)[ch].dimmList[dimm].rcLrFunc) {
      // Write function to addr 7
      WriteRC(host, socket, ch, dimm, rank, controlWordFunc, 7);
      (*channelNvList)[ch].dimmList[dimm].rcLrFunc = controlWordFunc;
    }

    // Write LR target function, addr
    WriteRC(host, socket, ch, dimm, rank, controlWordData, controlWordAddr);
  }
}

void
WriteLrbuf (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT8    controlWordData,
  UINT8    controlWordFunc,
  UINT8    controlWordAddr
  )
/*++

  Writes a nibble to DDR4 LRDIMM buffer control words

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param controlWordData - Control Word data
  @param controlWordFunc - Function index in the LR buffer
  @param controlWordAddr - Control Word Address

  @retval N/A

--*/
{
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = &host->nvram.mem.socket[socket].channelList;

  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) == 0) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,  "Command not supported \n"));
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, 0);
  }
#ifdef LRDIMM_EXTRA_DEBUG_MESSAGES
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                "LRDIMM BCW WRITE Fn: 0x%X  Addr: 0x%X CS: 0x%x write data: 0x%X\n", controlWordFunc, controlWordAddr, rank, controlWordData));
#endif

  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    // Write LR function index only if needed
    if (controlWordFunc != (*channelNvList)[ch].dimmList[dimm].rcLrFunc) {
      // Write function to addr 7x
      WriteBC(host, socket, ch, dimm, rank, controlWordFunc, LRDIMM_F0, LRDIMM_BC7x);
      (*channelNvList)[ch].dimmList[dimm].rcLrFunc = controlWordFunc;
    }

    // Write LR target data to the addr
    WriteBC(host, socket, ch, dimm, rank, controlWordData, controlWordFunc, controlWordAddr);
  }
}



void
WriteLrbufPBA (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT8    *controlWordDataPtr,
  UINT8    controlWordFunc,
  UINT8    controlWordAddr,
  UINT8    bufferAddr
  )
/*++

  Writes a nibble to LRDIMM register control words using PBA mode

  @param host               - Pointer to sysHost
  @param socket             - Socket Id
  @param ch                 - Channel number (0-based)
  @param dimm               - Dimm number (0-based)
  @param rank               - Rank number (0-based)
  @param controlWordDataPtr - Pointer to a 9 byte array of control word data
  @param controlWordFunc    - Function index in the LR buffer
  @param controlWordAddr    - Control Word Address
  @param bufferAddr         - Address of the individual buffer to target...or 'FF' for all buffers

  @retval N/A

--*/
{
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8             ReEnableMprNeeded = 0;
  UINT8             controlWordData;
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  UINT8             i;
  UINT8             allBuffersSame;

  rankStruct  = GetRankStruct(host, socket, ch, dimm);
  dimmNvList  = GetDimmNvList(host, socket, ch);
  if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)) == 0) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,  "Command not supported \n"));
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, 0);
  }
  //
  // boot time optimization, if all buffers need to be written with the same value, just use a broadcast write
  //
  allBuffersSame = 1;
  for(i = 1; i < 9; i++) {
    if (controlWordDataPtr[0] != controlWordDataPtr[i]) {
      allBuffersSame = 0;
      break;
    }
  }
  if ((allBuffersSame) && (bufferAddr == ALL_DATABUFFERS)) {
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordDataPtr[0], controlWordFunc, controlWordAddr);
    return;
  }

// ONLY DDR4 will call this routine  if (host->nvram.mem.dramType == SPD_TYPE_DDR4)

#ifdef LRDIMM_EXTRA_DEBUG_MESSAGES
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                "LRDIMM PBA Fn 0x%x, BCW 0x%x, BufAddr 0x%x\n",controlWordFunc, controlWordAddr, bufferAddr));
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                "Data = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
                controlWordDataPtr[0],controlWordDataPtr[1],controlWordDataPtr[2],controlWordDataPtr[3],controlWordDataPtr[4],
                controlWordDataPtr[5],controlWordDataPtr[6],controlWordDataPtr[7],controlWordDataPtr[8]));
#endif
  if ((*rankStruct)[rank].CurrentDramMode == MR3_MPR) {
    SetRankMPR (host, socket, ch, dimm, rank, 0);
    ReEnableMprNeeded = 1;
  }
  //
  // force the buffer into normal mode for the duration of the PBA write
  //
  if ((*rankStruct)[rank].CurrentLrdimmTrainingMode != LRDIMM_NORMAL_MODE) {
    WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
  }

  if (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1)) {
    WriteLrbuf(host, socket, ch, dimm, 0, 2, LRDIMM_F0, LRDIMM_BC0C);
  }
  CHIP_FUNC_CALL(host, WritePBA(host, socket, ch, dimm, 0, controlWordDataPtr, controlWordFunc, controlWordAddr, bufferAddr));

  //
  // restore the buffer training mode
  //
  controlWordData = (*rankStruct)[rank].CurrentLrdimmTrainingMode;
  if ((controlWordData != LRDIMM_NORMAL_MODE) || (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1))) {
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC0C);
  }

  //
  //restore MPR mode setting
  //
  if (ReEnableMprNeeded) {
    SetRankMPRPattern (host, socket, ch, dimm, rank, MR3_MPR, (*rankStruct)[rank].CurrentMpr0Pattern);
  }
}


//---------------------------------------------------------------------------
void
WriteRcLrbufExtended (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT8    lrbufData,
  UINT16   lrbufAddr
  )
/*++

  Writes a byte to LRDIMM extended register control words

  @param host            - Pointer to sysHost
  @param socket            - Socket Id
  @param ch              - Channel number (0-based)
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param lrbufData       - 8-bit data to write
  @param lrbufAddr       - 16-bit flat address space (4-bit function, 12-bit register offset)

  @retval N/A

--*/
{
  UINT8 nibble;

  // F13-RC9: Extended register function number (SMBus function 0 - 7)
  nibble = (UINT8)(lrbufAddr >> 12) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 9);

  // F13-RC10: Extended register address bits [3:0]
  nibble = (UINT8)(lrbufAddr >> 0) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 10);

  // F13-RC11: Extended register address bits [7:4]
  nibble = (UINT8)(lrbufAddr >> 4) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 11);

  // F13-RC12: Extended register address bits [11:8] (optional for 12-bit offset)
  nibble = (UINT8)(lrbufAddr >> 8) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 12);

  // F13-RC14: LSB data port; written first
  nibble = (UINT8)(lrbufData >> 0) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 14);

  // F13-RC15: MSB data port; written last to trigger the MB register access
  nibble = (UINT8)(lrbufData >> 4) & 0xF;
  WriteRcLrbuf(host, socket, ch, dimm, rank, nibble, 13, 15);
}

//---------------------------------------------------------------------------
void
WriteRcLrbufExtended16 (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT16   lrbufData,
  UINT16   lrbufAddr
  )
/*++

  Writes a word to LRDIMM extended register control words

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param lrbufData       - 16-bit data to write
  @param lrbufAddr       - 16-bit flat address space (4-bit function, 12-bit register offset)

  @retval N/A

--*/
{
  UINT8 nibble;

  nibble = (UINT8) (lrbufData & 0x00FF);
  WriteRcLrbufExtended(host, socket, ch, dimm, rank, nibble, lrbufAddr);
  nibble = (UINT8) ((lrbufData >> 8) & 0x00FF);
  WriteRcLrbufExtended(host, socket, ch, dimm, rank, nibble, lrbufAddr+1);
}

/**
  Initialize DVID RID register

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param dimm            - Dimm number (0-based)

  @retval N/A

**/
void
InitLrDimmRegisterDvidRid (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm
  )
{
  UINT32 lrbufData;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);


  // Inphi iMB02 sighting 9552
  ReadLrbufSmb (host, socket, ch, dimm, 0, &lrbufData);


  // Read the LRDIMM buffer vendor/device ID via SMBus
  // for vendor-specific workarounds
  if (SUCCESS == ReadLrbufSmb (host, socket, ch, dimm, 0, &lrbufData)) {
    (*dimmNvList)[dimm].lrbufDVid = lrbufData;
    // Get the Revision ID
    ReadLrbufSmb (host, socket, ch, dimm, 4, &lrbufData);
    (*dimmNvList)[dimm].lrbufRid = (UINT8) lrbufData;
  }
  else {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "LRDIMM SMBus Read failed\n"));
    RC_ASSERT(FALSE, ERR_SMBUS, ERR_LRDIMM_SMBUS_READ_FAILURE);
  }

  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "LRDIMM Vid/Did = 0x%08x, Rid = 0x%02x\n",
                 (*dimmNvList)[dimm].lrbufDVid, (*dimmNvList)[dimm].lrbufRid));

} // InitLrDimmRegisterDvidRid


//---------------------------------------------------------------------------

void
LrDimmRegisterInit (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
/*++

  Program register control words for DDR3 LRDIMMs

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)

  @retval N/A

--*/
{

  UINT8 dr, i, fn;
  UINT8 numRowBits;
  UINT8 cwData, cwData1;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct odtValueStruct *odtValuePtr;
  UINT8 density;

  dimmNvList = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

  // Initialize LRDIMM Dvid and Rid for Rank 0 ONLY
  if (rank == 0) InitLrDimmRegisterDvidRid (host, socket, ch, dimm);
// START IDT_Greendale_A2_1866_Sighting_102001 section 1 of 2
// Data structure dependencies:
// 1.)  host->nvram.common.imc[node].ddrFreqMHz must be contain valid integer value of DRAM clk frequency in MHz (initialized in SetDdrFreq).
// 2.)  (*dimmNvList)[dimm].lrbufDVid / (*dimmNvList)[dimm].lrbufRid contain value values.
//        (initialized by above call to InitLrDimmRegisterDvidRid in this function).
// 3.)  LRBUF_IDTGD_DVID and LRBUF_IDTGD_RID_A2 macro definitions defined (see section 2 of 2 in MemHost.h).
// Apply only for > DDR3-1600 with IDT Greendale A2 buffer.
  if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_IDTGD_DVID)  &&
     ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDTGD_RID_A2) &&
     (host->nvram.mem.socket[socket].ddrFreq > DDR_1600)) {

    // F14-RC0 = 0xF
    cwData = 0xF;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

    // F14-RC1 = 0x5
    cwData = 0x5;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 1);

    // F14-RC2 = 0x5
    cwData = 0x5;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 2);

    // F14-RC3 = 0x5
    cwData = 0x5;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 3);

    // F14-RC15 = 0x5
    cwData = 0x5;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 15);

    // VREG settling wait.
    FixedDelay (host, 1);

    // Perform two byte write to FN 8 0x2A = 0x3555
    WriteRcLrbufExtended16 (host, socket, ch, dimm, rank, 0x3555, 0x802a);

    // Write Password for Greendale A2
    WriteRcLrbufExtended16 (host, socket, ch, dimm, rank, 0xd231, 0x700a);
    WriteRcLrbufExtended16 (host, socket, ch, dimm, rank, 0xab12, 0x7008);

    // Parameterized for arbitrary frequencies DDR3-1601 to DDR3-1866.
    // Alternatively, hard coded values could be used based on frequency-table lookup of below function.
    // Extended write to Function 8 0x15 = modulo 256 [(262144 * frequency)/1,000,000]
    // Example results with integer math (-1 code error from no roundup).
    // DDR3-2133 (unsupported) - 14
    // DDR3-1866               - 244
    // DDR3-1800               - 235
    cwData=(UINT8)(0xFF & ((262144 * (UINT32)host->nvram.mem.socket[socket].ddrFreqMHz) / 1000000) );

    // Perform byte extended write to Fn 8 0x15 = cwData calculation
    WriteRcLrbufExtended(host, socket, ch, dimm, rank, cwData, 0x8015);

    // F14-RC0 = 0xE
    cwData = 0xE;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

  } // If LRBUF_IDTGD_DVID && LRBUF_IDTGD_RID_A2 at > DDR-1600
  // END IDT_Greendale_A2_1866_Sighting_102001 section 1 of 2
  //
  // Execute JEDEC standard init sequence on the buffer

  // F0-RC0: Output Weak Drive
  cwData = 2;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 0);

  // F0-RC1: Disable unused clocks
  if (((*dimmNvList)[dimm].dimmAttrib & (BIT3 + BIT2)) == BIT2) cwData = 0xC;
  else cwData = 0;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 1);

  // F0-RC2: Addr/Cmd drive strength to DRAM
  cwData = (*dimmNvList)[dimm].spdLrBuf_F0_RC2_3 & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 2);

  // F0-RC3: CS drive strength to DRAM
  cwData = (*dimmNvList)[dimm].spdLrBuf_F0_RC2_3 >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 3);

  // F0-RC4: CKE/ODT drive strength to DRAM
  cwData = (*dimmNvList)[dimm].spdLrBuf_F0_RC4_5 & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 4);

  // F0-RC5: CK drive strength to DRAM
  cwData = (*dimmNvList)[dimm].spdLrBuf_F0_RC4_5 >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 5);

  // F0-RC6: CKE/ODT mapping
  // use default setting of 0:
  //    DCKE[0] -> QxCKE[0] and QxCKE[2]
  //    DCKE[1] -> QxCKE[1] and QxCKE[3]

  // F0-RC7: Function Select Control Word (reserved)

  // F0-RC8: Input Bus Termination for DAn,DBAn, DRAS_n, DCAS_n,DWE_n and PAR_IN
  // Use the default setting = 100 ohms

  // F0-RC9: CKE power down config
  // Note: DCKE0 or DCKE1 must stay high, else power-down occurs
  cwData = 8;

  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 9);

  // F0-RC10: operating speed
  if (host->nvram.mem.socket[socket].ddrFreq <= DDR_800) {
    cwData = 0;
  } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1066) {
    cwData = 1;
  } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1333) {
    cwData = 2;
  } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
    cwData = 3;
  } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
    cwData = 4;
  } else {
    cwData = 5;
  }
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 10);

  // F0-RC11: operating voltage, parity based on num row bits
  cwData = (host->nvram.mem.socket[socket].ddrVoltage >> 1);

  // Set parity range
  if ((*channelNvList)[ch].lrdimmExtAddrMode) {
    cwData = cwData | A17_PARITY;
  } else {
    cwData = cwData | A15_PARITY;
  }
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 11);

  // Construct physical rank encoding
  cwData = 3;
  for (i = 1; i < (*dimmNvList)[dimm].numDramRanks; i = i << 1) cwData--;

  // Construct logical rank encoding
  cwData1 = 0;
  for (i = 1; i < (*dimmNvList)[dimm].numRanks; i = i << 1) cwData1++;

  // F0-RC13: num physical dram ranks, num logical ranks
  cwData = (cwData1 << 2) + cwData;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 13);

  // LRDIMM 1/2 populated check
  // Check SPD DIMM Attributes Bit 5 if only even ranks are present
  if ((*dimmNvList)[dimm].dimmAttrib & BIT5) {
    //1/2 Pop LRDIMM -> Apply WA\n
    cwData = 0xB;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

    cwData = 0x80;
    WriteRcLrbufExtended(host, socket, ch, dimm, rank, cwData, 0xC04);

    cwData = 1;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 13);
  }

  // Check for Addr mirroring on odd ranks
  cwData = 0;
  if ((*dimmNvList)[dimm].SPDAddrMapp) {
    cwData = cwData | BIT0;
  }
  // Check for x8 devices
  if ((*dimmNvList)[dimm].SPDModuleOrg & BIT0) {
    cwData = cwData | BIT3;
  }
  // F0-RC14: Addr mirroring, precharge/refresh stagger, MRS routing, bus width
  (*dimmNvList)[dimm].lrBuf_F0RC14 = cwData;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 14);

  // F0-RC15: Rank Multiplication
  cwData = 0;
  numRowBits = (*dimmNvList)[dimm].numRowBits;

  if ((*dimmNvList)[dimm].lrRankMult == 2) {
    cwData = numRowBits - 14;
  }
  else if ((*dimmNvList)[dimm].lrRankMult == 4) {
    cwData = numRowBits - 11;
  }
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 15);

  // F1-RC0: IBT settings for DCS
  // Use the default setting = 100 ohms

  // F1-RC1: IBT settings for DCKE
  // Use the default setting = 100 ohms

  // F1-RC2: IBT settings for DODT
  // Use the default setting = 100 ohms

  // F1-RC3: Weak output drive to DRAM
  // Use the default setting = Weak drive enabled

  // F1-RC9: Refresh Stagger
  density = (((*dimmNvList)[dimm].sdramCapacity) - 2) - ((*dimmNvList)[dimm].lrRankMult >> 1);
  if ((*dimmNvList)[dimm].lrRankMult > 1) {
     // Use lookup table based on DRAM speed, density and RM ratio
     cwData = RefreshStaggerLookupTable[host->nvram.mem.socket[socket].ddrFreq]
               [density]
               [(*dimmNvList)[dimm].lrRankMult >> 2].refreshStagger;
     WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 9);

     // F1-RC10: Refresh Jitter Limit
     cwData = 0xE;     // By enabling F1RC10 bit 3, start rank is the rank after the last refresh, start rank only changes if a refresh was missed
     WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 10);
  }

  // F2-RC[4:15]: Membist interface
  // Waiting for MB spec chapter

  // Lookup ODT values for this config
  odtValuePtr = CHIP_FUNC_CALL(host, LookupOdtValue(host, socket, ch));
  if(odtValuePtr ==NULL) {
     DisableChannel(host, socket, ch);
  }

  // F3-RC0: Connector DQ Rtt_nom
  cwData = odtValuePtr->dramOdt[dimm][rank] & 7;

  // Enable tDQS if x4 and x8 DIMMs are mixed on this channel and this is a x8
  if (((*channelNvList)[ch].features & X4_AND_X8_PRESENT) && !((*dimmNvList)[dimm].x4Present)) {
    cwData |= BIT3;
  }
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 0);

  // F3-RC1: Connector DQ Rtt_wr, Vref select
  cwData = (odtValuePtr->dramOdt[dimm][rank] >> 4) & 7;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 1);

  // F3-RC2: Connector DQ drive strength
  // Set to 27 ohms for now
  cwData = 3;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 2);

  // F3-RC6: Connector Misc settings
  if (host->setup.mem.lrdimm_x4asx8 &&
      ((*dimmNvList)[dimm].SPDModuleOrg & 7) == 0) {
    cwData = BIT3;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 6);
  }

  // F[3:11]-RC12: Additive DQ/DQS read delay control per DQ byte (optional)
  // F[3:11]-RC13: Additive DQ/DQS write delay control per DQ byte (optional)


  // Program settings based on SPD module specific bytes

  //
  // LR Buffer Function 1, Control Word 8 & 11
  //
  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC8_11 & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 8);

  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC8_11 >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 11);

  //
  // LR Buffer Function 1, Control Word 12 & 13
  //
  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC12_13 & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 12);

  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC12_13 >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 13);

  //
  // LR Buffer Function 1, Control Word 14 & 15
  //
  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC14_15 & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 14);

  cwData = (*dimmNvList)[dimm].spdLrBuf_F1_RC14_15 >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 15);

  //
  // LR Buffer Function 3, Control Word 8 & 9
  //
  cwData = (*dimmNvList)[dimm].spdLrBuf_MDQ_DS_ODT & 0x0F;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 8);

  cwData = (*dimmNvList)[dimm].spdLrBuf_MDQ_DS_ODT >> 4;
  WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3, 9);


  // Loop for each DRAM rank on the backside of the buffer
  for (dr = 0; dr < (*dimmNvList)[dimm].numDramRanks; dr++) {

    //
    // Initialize ODT activations per rank in MB
    // LR Buffer Function 3 - 11, Control Word 10 & 11
    // Assume upper 2 bits per nibble are 0
    //

    cwData = ((*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[dr >> 1] >> ((dr & 1) * 2)) & 3;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3 + dr, 10);

    cwData = ((*dimmNvList)[dimm].spdLrBuf_DRAM_QODT_ACT[dr >> 1] >> (((dr & 1) * 2) + 4)) & 3;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 3 + dr, 11);

    //
    // Initialize MR1,2 values per rank in MB
    //
    cwData = (*dimmNvList)[dimm].spdLrBuf_MR1_2_RTT;
    if (dr > 1) cwData = cwData & ~(BIT4 + BIT3 + BIT2);

    WriteRcLrbufExtended(host, socket, ch, dimm, rank, cwData, 0xB8 + dr);
  }

  // F14-RC[0:6,8-15]: Vendor specific personality bytes (optional)
  // F15-RC[0:6,8-15]: Vendor specific personality bytes (optional)

  // Update Vendor specific Bytes
  // Check if Vendor specific bytes are enabled
  if ((*dimmNvList)[dimm].lrbufPersBytes[0] & 0x1) {

    // Select F14 for now
    fn = 14;
    // Enable Vendor Specific Bytes
    cwData = (*dimmNvList)[dimm].lrbufPersBytes[0] & 0x0F;
    WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, fn, 0);

    // Take care of special case P-Byte 3 for F14RC7 and F15RC7
    i = 3;
    cwData = (*dimmNvList)[dimm].lrbufPersBytes[i] & 0x0F;
    WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, 14, (i << 1) % 16);
    cwData = ((*dimmNvList)[dimm].lrbufPersBytes[i] >> 4) & 0x0F;
    WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, 15, (i << 1) % 16);

    // Write the first 11 bytes for all of Function 14 and some of Function 15
    for (i = 0; i < 11; i++) {
      cwData = (*dimmNvList)[dimm].lrbufPersBytes[i] & 0x0F;
      WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, fn, (i << 1) % 16);

      // Special case P-Byte 3 since only the lower nibble goes into F14RC6, nothing is written to F14RC7
      if (i != 3) {
        cwData = ((*dimmNvList)[dimm].lrbufPersBytes[i] >> 4) & 0x0F;
        WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, fn, 1 + ((i << 1) % 16));
      }
      // check if we need to increse the function to 15
      if (i == 7) fn = 15;
    }

    // Write the remaining P-Bytes for Function 15
    // Have to special case since Index's don't align anymore
    for (i = 11; i < SPD_LR_PERS_BYTES_TOTAL; i++) {
      cwData = (*dimmNvList)[dimm].lrbufPersBytes[i] & 0x0F;
      WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, fn, ((i+1) << 1) % 16);
      cwData = ((*dimmNvList)[dimm].lrbufPersBytes[i] >> 4) & 0x0F;
      WriteRcLrbuf (host, socket, ch, dimm, rank, cwData, fn, 1 + (((i+1) << 1) % 16));
    }
  }

#ifdef    IDT_LRBUF_WA
  // IDT Workarounds start
  if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_IDT_DVID) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_RID_B0)) {

    WriteRcLrbufExtended16 (host, socket, ch, dimm, rank, 0xd230, 0x700a);
    WriteRcLrbufExtended16 (host, socket, ch, dimm, rank, 0xab20, 0x7008);

    // sighting 200003
    WriteRcLrbufExtended(host, socket, ch, dimm, rank, 0xbf, 0x8016);

    // sighting 200004
    cwData = 0x24;
    WriteRcLrbufExtended(host, socket, ch, dimm, 0, cwData, 0x100c);

    // sighting 200006
    cwData = 1;
    WriteRcLrbufExtended(host, socket, ch, dimm, 0, cwData, 0x80b4);

    // sighting 200008
    // sighting 200009
    cwData = 0;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 0);

    // sighting 200010
    cwData = REF_STAG_0;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 9);

  }

#endif    // IDT_LRBUF_WA


  // Inphi workarounds start

  // Check for Inphi A0/A1 Device/vendor Id
  if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_INPHI_DVID) &&
     (((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A0) ||
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1))) {

    // Sightings that affect both A0 and A1 stepping
    cwData = 0xB;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

    // Inphi iMB02 sighting 9478 - V0.4
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0000, 0x0270);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0074, 0x0122);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x009b, 0x0124);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00c2, 0x0126);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00e8, 0x0128);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x5942, 0x01d2);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x836d, 0x01d4);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x5942, 0x01ce);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x836d, 0x01d0);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x017f, 0x01d6);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0000, 0x01d8);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x008e, 0x01f0);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00ba, 0x01f2);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00e8, 0x01f4);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0114, 0x01f6);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x7054, 0x0b40);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xa48a, 0x0b42);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x7054, 0x0b3c);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xa48a, 0x0b3e);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0100, 0x0b38);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0000, 0x0b3a);

    // Inphi iMB02 sighting 9553
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x55aa, 0x0274);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0080, 0x3012);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x6b80, 0x3018);

    // Inphi Updated sighting 9480
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x1081, 0x1DE);
    // WA HSD 3616944
    // Changes for 1866 operation on iMB02-GS02A. All these changes are gated by
    // Inphi iMB02-GS02A VID/RID
    if (host->nvram.mem.socket[socket].ddrFreq == DDR_1866){

      // Set F0RC10 to DDR3-1600
      //cwData = DDR3_1600;
      cwData = 3;
      WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 0, 10);

      // Program DDR3-1866 timing registers
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0100, 0x0128);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0130, 0x01f6);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x106d, 0x01d0);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x358a, 0x0b3a);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x106d, 0x01d4);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x358a, 0x0b42);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x008e, 0x0b06);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00af, 0x0b0e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00ce, 0x0b16);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00ed, 0x0b1e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x3923, 0x0b4a);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x45a8, 0x0b44);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x4838, 0x0b22);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x463a, 0x0b26);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xcc92, 0x0b2a);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xd99b, 0x0b2e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x503e, 0x0b32);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x5447, 0x0b36);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x047e, 0x02be);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00ff, 0x3012);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xb380, 0x3018);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0002, 0x011e);

      // Force CL=11 and CWL=8 due to iMB02-GS02A CL, CWL range limitations
      (*channelNvList)[ch].common.nWL = 8;
      (*channelNvList)[ch].common.nCL = 11;

      //
      //Reprogram Timing Contstraints DDR3 Bin Parameters
      //
      CHIP_FUNC_CALL(host, SetDataTiming (host, socket, ch, (*channelNvList)[ch].common.nCL, (*channelNvList)[ch].common.nWL));

    } // if 1866

    // Inphi Sighting 10864 - Same as for GS02B
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00FF, 0x3012);

    if(host->nvram.mem.socket[socket].ddrFreq == DDR_1600) {
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0002, 0x011e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x003f, 0x0c00);
    } else {
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0001, 0x011e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0007, 0x0c00);
    }

  } // A0 or A1

  // Check for Inphi A0 Device/vendor Id
  if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_INPHI_DVID) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A0)) {

    cwData = 0xB;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

    // Inphi iMB02 sighting 9471
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0xff0f, 0x0c0a);

    // Inphi iMB02 sighting 9480 - V0.3
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x11e2, 0x01dc);

    // Inphi iMB02 sighting 9507
    if ((*dimmNvList)[dimm].SPDDramMfgId == 0x2c00) {

      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0003, 0x0c00);

      if (host->nvram.mem.socket[socket].ddrFreq < DDR_1333) cwData = 0x10;
      else cwData = 0x14;
      WriteRcLrbufExtended(host, socket, ch, dimm, rank, cwData, 0x112);
      WriteRcLrbufExtended(host, socket, ch, dimm, rank, cwData, 0x113);

    } // SPDDramMfgId == Micron

    // Inphi iMB sighting 9684
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0009, 0x011e);

    // Inphi iMB sighting 9673
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0233, 0x0e00);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00e5, 0x0e02);


    // Inphi iMB sighting 9830
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x03ff, 0x026e);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x9c40, 0x0c02);
    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x1102, 0x01de);

  } // Inphi A0 only

  if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_INPHI_DVID) &&
      ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_B0)) {
    //MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
    //              "Inphi iMB B0 (GS02B) WA Applied\n"));

    cwData = 0xB;
    WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 14, 0);

    WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x00FF, 0x3012);

    if(host->nvram.mem.socket[socket].ddrFreq >= DDR_1600) {
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0002, 0x011e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x003f, 0x0c00);
    } else {
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0001, 0x011e);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0007, 0x0c00);
    }

    if ((host->nvram.mem.socket[socket].ddrFreq == DDR_1866) &&
        (host->nvram.mem.socket[socket].ddrVoltage == SPD_VDD_135)) {
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x0000, 0x0e12);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x001A, 0x0e14);
      WriteRcLrbufExtended16(host, socket, ch, dimm, rank, 0x001A, 0x0e16);
    }

    if ((*dimmNvList)[dimm].lrRankMult == 2) {
      //Program F1 RC10[3] = 0
      cwData = 0x6;
      WriteRcLrbuf(host, socket, ch, dimm, rank, cwData, 1, 10);
    }

  } // Inphi B0 only
  // Inphi workarounds end

}

#define LRBUF_VALID   BIT14
#define LRBUF_FAILURE BIT13
#define LRBUF_TCAL    10000      // 10 msec per physical rank

//---------------------------------------------------------------------------
void
DoLrbufTraining (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
/*++

  Training step for LR Buffer

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval N/A

--*/
{
  UINT8 dimm;
  UINT8 done;
  UINT32 lrbufData;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  UINT64_STRUCT startTsc;
  UINT64_STRUCT endTsc;
  UINT32 elapsedTime = 0;
  UINT8 failStatus[MAX_DIMM];
  UINT8 globalFail;
  UINT32 timeout = 0;

  if (host->nvram.mem.socket[socket].channelList[ch].lrDimmPresent &&
      !host->nvram.mem.socket[socket].channelList[ch].idtLrDimmPresent &&
      !host->nvram.mem.socket[socket].lrInitDone) {

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      // Use F2-RC3 to reset Valid and Failure bits
      WriteRcLrbuf(host, socket, ch, dimm, 0, 0, 2, 3);
      WriteRcLrbuf(host, socket, ch, dimm, 0, BIT0, 2, 3);

      // Use F0-RC12 to start DRAM interface training
      WriteRcLrbuf(host, socket, ch, dimm, 0, BIT1, 0, 12);

      // Initialize failStatus = failure
      failStatus[dimm] = 1;

      // Establish max timeout value for MB training
      if (timeout < (UINT32)(*dimmNvList)[dimm].numDramRanks * LRBUF_TCAL) {
         timeout = (UINT32)(*dimmNvList)[dimm].numDramRanks * LRBUF_TCAL;
      }
    } //dimm loop

    // Establish timeout start
    ReadTsc64 (&startTsc);
    do {
      done = 1;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        // Poll SMBus for completion
        // F2-RC3: Training and Errout control
        if (SUCCESS == ReadLrbufSmb (host, socket, ch, dimm, 0x18, &lrbufData)) {

          // ForSoftSimulation testing...
          if (host->var.common.emulation & (SOFT_SDV_FLAG | VP_FLAG | SIMICS_FLAG)) {
             lrbufData = lrbufData | LRBUF_VALID;
          }
          // Continue waiting while not valid
          if (!(lrbufData & LRBUF_VALID)) done = 0;

          // Training FSM is done
          else {
            // Update results
            if (!(lrbufData & LRBUF_FAILURE)) failStatus[dimm] = 0;
          }
        } // (SUCCESS == ...
      } //dimm

      if (done) {
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0)continue;
            // Reset F0-RC12
            WriteRcLrbuf(host, socket, ch, dimm, 0, 0, 0, 12);
         }// dimm loop
       }// if done do reset
      ReadTsc64 (&endTsc);
      elapsedTime = TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS);

    } while (!done && (elapsedTime < timeout));

    // Output MB to DRAM training results
    if (elapsedTime < timeout) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "LRDIMM training completed in %d msec\n", elapsedTime));
    } else {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "LRDIMM training timeout after %d msec\n", elapsedTime));
    }

    globalFail = 0;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if (failStatus[dimm]) {
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                      "Failed DDR3 LRDIMM MB to DRAM training\n"));
#if SMCPKG_SUPPORT
        OutputWarning (host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_TRAINING, socket, ch, dimm, NO_RANK);
#endif
        globalFail = 1;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                      "Passed DDR3 LRDIMM MB to DRAM training\n"));
      }

      // debug... dump all MB config space via SMBus reads
      //CHIP_FUNC_CALL(host, DumpLrbuf (host, socket, ch, dimm));

    }

    // map out ch on failure
    if (globalFail) {
      DisableChannel(host, socket, ch);
#if SMCPKG_SUPPORT == 0
      OutputWarning (host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_TRAINING, socket, ch, 0xFF, 0xFF);
#endif
    }

  }
}
#endif  // LRDIMM_SUPPORT

//-----------------------------------------------------------------------------
/**

  Read LR Buffer

  @param host       - Pointer to sysHost
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - DIMM number (0-based)
  @param rank       - Rank number (0-based)
  @param lrbuffAddr - Address for the read
  @param lrbuffData - Pointer to the LR Buffer data

  @retval SUCCESS
  @retval FAILURE

**/
UINT32 ReadLrbufSmb (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT16   lrbufAddr,
  UINT32   *lrbufData
  )
{

  UINT8 dc = 0;
  UINT8 func;
  UINT8 extOffset;
  UINT8 offset;
  UINT32 temp = 0xFFFFFFFF;        // Emulate master abort
  UINT32 status = FAILURE;
  struct smbDevice dev;

  dev.compId = SPD;
  dev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  dev.address.deviceType = DTI_LRDIMM;
  CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &dev));

  func = (UINT8) (lrbufAddr >> 12);
  extOffset = (UINT8) ((lrbufAddr >> 8) & 0xF);
  offset = (UINT8) (lrbufAddr & 0xFF);
  if ((CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x80, &dc)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x00, &func)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x00, &extOffset)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x40, &offset)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, 0x80, &dc)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, 0x00, (UINT8 *)&temp + 3)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, 0x00, (UINT8 *)&temp + 2)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, 0x00, (UINT8 *)&temp + 1)) == SUCCESS) &&
      (CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, 0x40, (UINT8 *)&temp)) == SUCCESS)) {

    status = SUCCESS;
  }
  *lrbufData = temp;

  return status;

}

//-----------------------------------------------------------------------------
/**

  Write LR Buffer

  @param host       - Pointer to sysHost
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - DIMM number (0-based)
  @param size       - number of bytes to be written
  @param lrbuffAddr - Address for the read
  @param lrbuffData - Pointer to the LR Buffer data

  @retval SUCCESS
  @retval FAILURE

**/
UINT32 WriteLrbufSmb (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    size,
  UINT16   lrbufAddr,
  UINT32   *lrbufData
  )
{

  UINT8 dc = 0;
  UINT8 func;
  UINT8 extOffset;
  UINT8 offset;
  UINT32 status = FAILURE;
  struct smbDevice dev;

  dev.compId = SPD;
  dev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  dev.address.deviceType = DTI_LRDIMM;
  CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &dev));

  func = (UINT8) (lrbufAddr >> 12);
  extOffset = (UINT8) ((lrbufAddr >> 8) & 0xF);
  offset = (UINT8) (lrbufAddr & 0xFF);
  switch (size) {
  case sizeof(UINT32):
    if ((CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x8C, &dc)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, &func)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, &extOffset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, &offset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, (UINT8 *)lrbufData + 3)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, (UINT8 *)lrbufData + 2)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x0C, (UINT8 *)lrbufData + 1)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x4C, (UINT8 *)lrbufData)) == SUCCESS)) {

      status = SUCCESS;
    }
    break;
  case sizeof(UINT16):
    if ((CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x88, &dc)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x08, &func)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x08, &extOffset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x08, &offset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x08, (UINT8 *)lrbufData + 1)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x48, (UINT8 *)lrbufData)) == SUCCESS)) {

      status = SUCCESS;
    }
    break;
  case sizeof(UINT8):
    if ((CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x84, &dc)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x04, &func)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x04, &extOffset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x04, &offset)) == SUCCESS) &&
        (CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, 0x44, (UINT8 *)lrbufData)) == SUCCESS)) {

      status = SUCCESS;
    }
    break;
  default:
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_76);
  }
  return status;
}

//---------------------------------------------------------------------------
void
RestoreMRS (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
/*++

  Restore MRS

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval N/A

--*/
{
  UINT8 dimm;
  UINT8 rank;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct rankDevice (*rankStruct)[MAX_RANK_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Loop for each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    rankStruct  = GetRankStruct(host, socket, ch, dimm);

    //
    // Loop for each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Skip if no rank
      //
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      //
      // Restore MR2
      //
      WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
      //
      // Restore MR1
      //
      WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
      //
      // Restore MR0
      //
      WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);
    } // rank loop
  } // dimm loop
} // RestoreMRS


UINT8
DetectTRRSupport (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
/*++

  Detect the TRR Mode(s) supported on this channel

  @param host    - Pointer to sysHost
  @param socket  - Processor socket
  @param ch      - Memory channel

  @retval TRR mode(s) supported

--*/
{
  UINT8               mode;
  UINT8               dimm;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Default both modes enabled
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    mode = 0;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if ((*dimmNvList)[dimm].SPDOptionalFeature & SPD_TRR_IMMUNE) {
        mode |= TRR_IMMUNE;
      } else if ((*dimmNvList)[dimm].SPDOptionalFeature & 7) {
#ifdef  TRR_MODE_B_SUPPORT
        if ((*dimmNvList)[dimm].SPDOptionalFeature & BIT6) {
          mode |= TRR_MODE_B;
        } else
#endif  // TRR_MODE_B_SUPPORT
        mode |= TRR_MODE_A;
      } else {
        //
        // This DIMM does not support TRR so disable support for the whole channel
        //
        mode = 0;
        break;
      }
    } // dimm loop

#ifdef  TRR_MODE_B_SUPPORT
    if ((mode & TRR_MODE_A) && (mode & TRR_MODE_B)) {
      //
      // Force mode A if mixed with mode B
      //
      mode = TRR_MODE_A;
    } else if ((mode & TRR_IMMUNE) && (mode & TRR_MODE_B)) {
      //
      // Use mode B if mixed with an immune DIMM
      //
      mode &= ~TRR_IMMUNE;
    } else
#endif  // TRR_MODE_B_SUPPORT
    if ((mode & TRR_IMMUNE) && (mode & TRR_MODE_A)) {
      //
      // Use mode A if mixed with an immune DIMM
      //
      mode &= ~TRR_IMMUNE;
    }
  } else {
    mode = 0;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if ((*dimmNvList)[dimm].SPDOptionalFeature & SPD_PTRR) {
        mode |= PTRR_MODE;
        break;
      } else if ((*dimmNvList)[dimm].SPDOptionalFeature & BIT3) {
        mode |= TRR_IMMUNE;
      } else {
        mode = 0;
        break;
      }
    } // dimm loop

    if ((mode & TRR_IMMUNE) && (mode & PTRR_MODE)) {
      //
      // Use pTRR mode if mixed with an immune DIMM
      //
      mode &= ~TRR_IMMUNE;
    }
  }

  return mode;
} // DetectTRRSupport

/**
  Perform a DLL reset

  @param host            - Pointer to sysHost
  @param socket          - Socket Id

  @retval N/A

**/
void IssueDllReset (
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  disableAllMsg (host);

  for (ch = 0; ch < MAX_CH; ch++) {
    // skip if channel not enabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Loop for each DIMM
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

      // Skip if no DIMM present
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      // Loop for each rank
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // Skip if no rank
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        // MR0
        WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);

      } // rank loop
    } // dimm loop
  } // ch loop

  FixedDelay(host, 1);

  restoreMsg (host);
}


struct lrbufRefresh RefreshStaggerLookupTable[MAX_SUP_FREQ][MAX_DENSITY][MAX_RM] = {
   // Rank Multiplication
   //          x2,                   x4
   // DDR3_800
   {{{REF_STAG_40,  0xa4},  {REF_STAG_40,  0xf4}},    // 1Gb
    {{REF_STAG_60,  0xcc},  {REF_STAG_60,  0x144}},   // 2Gb
    {{REF_STAG_100, 0x12c}, {REF_STAG_100, 0x1f4}},   // 4Gb
    {{REF_STAG_120, 0x154}, {REF_STAG_120, 0x244}}},  // 8Gb

   // DDR3_1000
   {{{REF_STAG_40,  0xa4},  {REF_STAG_40,  0xf4}},    // 1Gb
    {{REF_STAG_60,  0xdc},  {REF_STAG_60,  0x154}},   // 2Gb
    {{REF_STAG_120, 0x15e}, {REF_STAG_120, 0x24e}},   // 4Gb
    {{REF_STAG_120, 0x177}, {REF_STAG_120, 0x267}}},  // 8Gb

   // DDR3_1066
   {{{REF_STAG_60,  0xc7},  {REF_STAG_60,  0x13f}},   // 1Gb
    {{REF_STAG_80,  0xf6},  {REF_STAG_80,  0x196}},   // 2Gb
    {{REF_STAG_120, 0x168}, {REF_STAG_120, 0x258}},   // 4Gb
    {{REF_STAG_120, 0x183}, {REF_STAG_120, 0x273}}},  // 8Gb

   // DDR3_1200
   {{{REF_STAG_60,  0xce},  {REF_STAG_60,  0x146}},   // 1Gb
    {{REF_STAG_80,  0x100}, {REF_STAG_80,  0x1a0}},   // 2Gb
    {{REF_STAG_120, 0x17c}, {REF_STAG_120, 0x26c}},   // 4Gb
    {{REF_STAG_120, 0x19a}, {REF_STAG_120, 0x28a}}},  // 8Gb

   // DDR3_1333
   {{{REF_STAG_60,  0xd6},  {REF_STAG_60,  0x14e}},   // 1Gb
    {{REF_STAG_80,  0x10b}, {REF_STAG_80,  0x1ab}},   // 2Gb
    {{REF_STAG_120, 0x190}, {REF_STAG_120, 0x280}},   // 4Gb
    {{REF_STAG_120, 0x1b2}, {REF_STAG_120, 0x2a2}}},  // 8Gb

   // DDR3_1400
   {{{REF_STAG_60,  0xd9},  {REF_STAG_60,  0x151}},   // 1Gb
    {{REF_STAG_80,  0x124}, {REF_STAG_80,  0x1ec}},   // 2Gb
    {{REF_STAG_120, 0x19a}, {REF_STAG_120, 0x28a}},   // 4Gb
    {{REF_STAG_120, 0x1bd}, {REF_STAG_120, 0x2ad}}},  // 8Gb

   // DDR3_1600
   {{{REF_STAG_80,  0xf8},  {REF_STAG_80,  0x198}},   // 1Gb
    {{REF_STAG_100, 0x134}, {REF_STAG_100, 0x1fc}},   // 2Gb
    {{REF_STAG_120, 0x1b8}, {REF_STAG_120, 0x2a8}},   // 4Gb
    {{REF_STAG_120, 0x1e0}, {REF_STAG_120, 0x2d0}}},  // 8Gb

   // DDR3_1800
   {{{REF_STAG_80,  0xf8},  {REF_STAG_80,  0x198}},   // 1Gb
    {{REF_STAG_100, 0x158}, {REF_STAG_100, 0x248}},   // 2Gb
    {{REF_STAG_120, 0x1d2}, {REF_STAG_120, 0x2c6}},   // 4Gb
    {{REF_STAG_120, 0x203}, {REF_STAG_120, 0x2f3}}},  // 8Gb

   // DDR3_1866
   {{{REF_STAG_80,  0x107}, {REF_STAG_80,   0x1a7}},  // 1Gb
    {{REF_STAG_120, 0x15e}, {REF_STAG_120,  0x24e}},  // 2Gb
    {{REF_STAG_120, 0x1e0}, {REF_STAG_120,  0x2d0}},  // 4Gb
    {{REF_STAG_120, 0x20f}, {REF_STAG_120,  0x2ff}}}, // 8Gb

   // DDR3_2000
   {{{REF_STAG_100, 0x122}, {REF_STAG_100,  0x1ea}},  // 1Gb
    {{REF_STAG_120, 0x168}, {REF_STAG_120,  0x258}},  // 2Gb
    {{REF_STAG_120, 0x1f4}, {REF_STAG_120,  0x2e4}},  // 4Gb
    {{REF_STAG_120, 0x226}, {REF_STAG_120,  0x316}}}, // 8Gb

   // DDR3_2133
   {{{REF_STAG_100, 0x12a}, {REF_STAG_100,  0x1f2}},  // 1Gb
    {{REF_STAG_120, 0x173}, {REF_STAG_120,  0x263}},  // 2Gb
    {{REF_STAG_120, 0x208}, {REF_STAG_120,  0x2f8}},  // 4Gb
    {{REF_STAG_120, 0x23e}, {REF_STAG_120,  0x32e}}}, // 8Gb

   // DDR3_2200
   {{{REF_STAG_100, 0x12a}, {REF_STAG_100,  0x1f2}},  // 1Gb
    {{REF_STAG_120, 0x178}, {REF_STAG_120,  0x268}},  // 2Gb
    {{REF_STAG_120, 0x212}, {REF_STAG_120,  0x302}},  // 4Gb
    {{REF_STAG_120, 0x249}, {REF_STAG_120,  0x339}}}, // 8Gb

   // DDR3_2400
   {{{REF_STAG_100, 0x12d}, {REF_STAG_100,  0x1f5}},  // 1Gb
    {{REF_STAG_120, 0x188}, {REF_STAG_120,  0x278}},  // 2Gb
    {{REF_STAG_120, 0x230}, {REF_STAG_120,  0x320}},  // 4Gb
    {{REF_STAG_120, 0x26c}, {REF_STAG_120,  0x35c}}}, // 8Gb

   // DDR3_2600
   {{{REF_STAG_120, 0x1e6}, {REF_STAG_120,  0x2d6}},  // 1Gb
    {{REF_STAG_120, 0x198}, {REF_STAG_120,  0x288}},  // 2Gb
    {{REF_STAG_120, 0x24e}, {REF_STAG_120,  0x33e}},  // 4Gb
    {{REF_STAG_120, 0x28f}, {REF_STAG_120,  0x37f}}}, // 8Gb

   // DDR3_2666
   {{{REF_STAG_120, 0x15b}, {REF_STAG_120,  0x248}},  // 1Gb
    {{REF_STAG_120, 0x19e}, {REF_STAG_120,  0x28e}},  // 2Gb
    {{REF_STAG_120, 0x258}, {REF_STAG_120,  0x348}},  // 4Gb
    {{REF_STAG_120, 0x29b}, {REF_STAG_120,  0x38b}}},  // 8Gb

   // DDR3_2800
   {{{REF_STAG_120, 0x162}, {REF_STAG_120,  0x252}},  // 1Gb
    {{REF_STAG_120, 0x1a8}, {REF_STAG_120,  0x298}},  // 2Gb
    {{REF_STAG_120, 0x26c}, {REF_STAG_120,  0x35c}},  // 4Gb
    {{REF_STAG_120, 0x2b2}, {REF_STAG_120,  0x3a2}}},  // 8Gb

   // DDR3_2933
   {{{REF_STAG_120, 0x16a}, {REF_STAG_120,  0x25a}},  // 1Gb
    {{REF_STAG_120, 0x1b3}, {REF_STAG_120,  0x2a3}},  // 2Gb
    {{REF_STAG_120, 0x280}, {REF_STAG_120,  0x370}},  // 4Gb
    {{REF_STAG_120, 0x2ca}, {REF_STAG_120,  0x3ba}}},  // 8Gb

   // DDR3_3000
   {{{REF_STAG_120, 0x16d}, {REF_STAG_120,  0x25d}},  // 1Gb
    {{REF_STAG_120, 0x1b8}, {REF_STAG_120,  0x2a8}},  // 2Gb
    {{REF_STAG_120, 0x28a}, {REF_STAG_120,  0x37a}},  // 4Gb
    {{REF_STAG_120, 0x2d5}, {REF_STAG_120,  0x3c5}}},  // 8Gb

   // DDR3_3200
   {{{REF_STAG_120, 0x178}, {REF_STAG_120,  0x268}},  // 1Gb
    {{REF_STAG_120, 0x1c8}, {REF_STAG_120,  0x2b8}},  // 2Gb
    {{REF_STAG_120, 0x2a8}, {REF_STAG_120,  0x398}},  // 4Gb
    {{REF_STAG_120, 0x2f8}, {REF_STAG_120,  0x3e8}}}  // 8Gb
};
UINT32
GettCK(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tCK

@param host    - Point to sysHost
@param socket  - Socket number

@retval UINT32 Value of tCK

--*/
{
  //UINT32 BCLK;
  //return ((10000000 + (host->nvram.mem.socket[socket].ddrFreqMHz - 1)) / host->nvram.mem.socket[socket].ddrFreqMHz);
  //if (BCLKs[host->nvram.mem.ratioIndex] = 133){
  //  BCLK = 133333;
  //} else {
  //  BCLK = 100000;
  //}
  return preciseTCK[host->nvram.mem.ratioIndex];
} // GettCK

UINT32
GettREFIx9(
PSYSHOST host,
UINT16   tREFI
)
/*++

This function returns the value of tREFI x 9

@param host    - Point to sysHost
@param tREFI   - Refresh rate

@retval UINT32 Value of tREFI x 9

--*/
{
  UINT32  tREFIx9Local;

  tREFIx9Local = (9 * tREFI) / 1024;

  CheckBounds(host, tREFIx9, &tREFIx9Local);

  return tREFIx9Local;
} // GettREFIx9

UINT32
GettREFI(
PSYSHOST host,
UINT8    socket,
UINT8    ch
)
/*++

This function returns the value of tREFI

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tREFI

--*/
{
  UINT32              tREFILocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  //
  // Calculate tREFI
  //
  if ((*channelNvList)[ch].twoXRefreshSetPerCh >= 20) {

    // Assume temp_lo = 0, so divide requested rate by 2
    tREFILocal = (UINT16)((7800 * 20) / (*channelNvList)[ch].twoXRefreshSetPerCh * 10000 / GettCK(host, socket));

  }
  else {

    // Otherwise use 1x formula
    tREFILocal = (UINT16)((7800 * 10) / (*channelNvList)[ch].twoXRefreshSetPerCh * 10000 / GettCK(host, socket));
  }

  CheckBounds(host, tREFI, &tREFILocal);

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "Refresh rate = %d\n", tREFILocal));

  return tREFILocal;
} // GettREFI

UINT32
GettCKE(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tCKE

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tCKE

--*/
{
  UINT32  tCKELocal;
  UINT32  tCKETime;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    if (host->nvram.mem.socket[socket].ddrFreqMHz < 500) {
      tCKETime = 75000; // 7.5 ns
    } if (host->nvram.mem.socket[socket].ddrFreqMHz < 800){
      tCKETime = 56250; // 5.625 ns
    }
    else {
      tCKETime = 50000; // 5 ns
    }
  }
  else {
    tCKETime = 50000; // 5 ns
  }

  tCKELocal = (UINT8)((tCKETime + (GettCK(host, socket) - 100)) / GettCK(host, socket));

  //
  // Must be at least 3 clocks
  //
  if (tCKELocal < 3) {
    tCKELocal = 3;
  }

  CheckBounds(host, tCKE, &tCKELocal);

  return tCKELocal;
} // GettCKE

UINT32
GettXPDLL(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tXPDLL

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tXPDLL

--*/
{
  UINT32  tXPDLLLocal;

  tXPDLLLocal = (240000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  CheckBounds(host, tXPDLL, &tXPDLLLocal);

  return tXPDLLLocal;
} // GettXPDLL

UINT32
GettXSOFFSET(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tXSOFFSET

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tXSOFFSET

--*/
{
  UINT32  tXSOFFSETLocal;

  tXSOFFSETLocal = (100000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  CheckBounds(host, tXSOFFSET, &tXSOFFSETLocal);

  return tXSOFFSETLocal;
} // GettXSOFFSET

UINT32
GettXP(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tXP

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tXP

--*/
{
  UINT32  tXPLocal;
  UINT32  tXPTime;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    if (host->nvram.mem.socket[socket].ddrFreqMHz < 666) {
      tXPTime = 75000; // 7.5 ns
    }
    else {
      tXPTime = 60000; // 6 ns
    }
  }
  else {
    tXPTime = 60000; // 6 ns
  }

  tXPLocal = (tXPTime + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  //
  // Must be at least 3 clocks
  //
  if (tXPLocal < 3) {
    tXPLocal = 3;
  }

  CheckBounds(host, tXP, &tXPLocal);

  return tXPLocal;
} // GettXP

UINT32
GettCKSRX(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tCKSRX

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32Value of tCKSRX

--*/
{

  return (100000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);
} // GettCKSRX

UINT32
GettSTAB(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tSTAB

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tSTAB

--*/
{
  UINT32  tSTABLocal;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    tSTABLocal = (60000000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);
  }
  else {
    tSTABLocal = (50000000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);
  }

  CheckBounds(host, tSTAB, &tSTABLocal);

  return tSTABLocal;
} // GettSTAB

UINT32
GettMOD(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tMOD

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tMOD

--*/
{
  UINT32 tMODLocal;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    tMODLocal = (UINT8)((150000 + (GettCK(host, socket) - 100)) / GettCK(host, socket));
  }
  else {
    tMODLocal = 24;
  }

  if (tMODLocal < 12) {
    tMODLocal = 12;
  }

  CheckBounds(host, tMOD, &tMODLocal);

  return tMODLocal;
} // GettMOD

UINT32
GettFAW(
PSYSHOST host,
UINT8    socket,
UINT8    pageSize
)
/*++

This function returns the value of tFAW

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tFAW

--*/
{
  UINT32  tFAWLocal;
  UINT32  tFAWTime;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    if (pageSize == PAGE_SIZE_1K) {
      if (host->nvram.mem.socket[socket].ddrFreqMHz <= 400) {
        tFAWTime = 400000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 666) {
        tFAWTime = 357000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz <= 800) {
        tFAWTime = 300000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1066) {
        tFAWTime = 270000;
      }
      else {
        tFAWTime = 250000;
      }
    }
    else {
      if (host->nvram.mem.socket[socket].ddrFreqMHz < 666) {
        tFAWTime = 500000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 800) {
        tFAWTime = 450000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz == 800) {
        tFAWTime = 400000;
      }
      else {
        tFAWTime = 350000;
      }
    }
  }
  else {
    if (pageSize == PAGE_SIZE_HALFK) {
      if (host->nvram.mem.socket[socket].ddrFreqMHz <= 800) {
        tFAWTime = 200000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1066) {
        tFAWTime = 170000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1200) {
        tFAWTime = 150000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1333) {
        tFAWTime = 130000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1600) {
        tFAWTime = 120000;
      }
      else {
        tFAWTime = 100000;
      }
    }
    else if (pageSize == PAGE_SIZE_1K) {
      if (host->nvram.mem.socket[socket].ddrFreqMHz <= 800) {
        tFAWTime = 250000;
      }
      else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1066) {
        tFAWTime = 230000;
      }
      else {
        tFAWTime = 210000;
      }
    }
    else {
      if (host->nvram.mem.socket[socket].ddrFreqMHz <= 800) {
        tFAWTime = 350000;
      }
      else {
        tFAWTime = 300000;
      }
    }
  }

  tFAWLocal = (UINT8)((tFAWTime + (GettCK(host, socket) - 100)) / GettCK(host, socket));

  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    if (pageSize == PAGE_SIZE_HALFK) {
      if (tFAWLocal < 16) {
        tFAWLocal = 16;
      }
    }
    else if (pageSize == PAGE_SIZE_1K) {
      if (tFAWLocal < 20) {
        tFAWLocal = 20;
      }
    }
    else {
      if (tFAWLocal < 28) {
        tFAWLocal = 28;
      }
    }
  }

  CheckBounds(host, tFAW, &tFAWLocal);

  return tFAWLocal;
} // GettFAW

UINT32
GettRTP(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tRTP

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tRTP

--*/
{
  UINT32 tRTPLocal;

  tRTPLocal = (75000 + (GettCK(host, socket) - 1)) / GettCK(host, socket);

  if (tRTPLocal < 4) {
    tRTPLocal = 4;
  }

  CheckBounds(host, tRTP, &tRTPLocal);

  return tRTPLocal;
} // GettRTP

UINT32
GettWTR(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tWTR

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tWTR

--*/
{
  UINT32 tWTRLocal;

  tWTRLocal = (75000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  if (tWTRLocal < 4) {
    tWTRLocal = 4;
  }

  CheckBounds(host, tWTR, &tWTRLocal);

  return tWTRLocal;
} // GettWTR

UINT32
GettWTR_S(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tWTR_S

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tWTR_S

--*/
{
  UINT32 tWTR_SLocal;

  tWTR_SLocal = (25000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  if (tWTR_SLocal < 2) {
    tWTR_SLocal = 2;
  }

  CheckBounds(host, tWTR_S, &tWTR_SLocal);

  return tWTR_SLocal;
} // GettWTR_S

UINT32
GettWR(
PSYSHOST host,
UINT8    socket
)
/*++

This function returns the value of tWR

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tWR

--*/
{
  UINT32 tWRLocal;

  tWRLocal = (150000 + (GettCK(host, socket) - 1)) / GettCK(host, socket);

  CheckBounds(host, tWR, &tWRLocal);

  return tWRLocal;
} // GettWR

UINT32
GettRFC(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRFC

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRFC

--*/
{
  UINT32              tRFCLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRFCLocal = (*channelNvList)[ch].common.nRFC;

  CheckBounds(host, tRFC, &tRFCLocal);

  return tRFCLocal;
} // GettRFC

UINT32
GettRAS(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRAS

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRAS

--*/
{
  UINT32              tRASLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRASLocal = (*channelNvList)[ch].common.nRAS;

  CheckBounds(host, tRAS, &tRASLocal);

  return tRASLocal;
} // GettRAS

UINT32
GettCWL(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tCWL

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tCWL

--*/
{
  UINT32              tCWLLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tCWLLocal = (*channelNvList)[ch].common.nWL;

  CheckBounds(host, tCWL, &tCWLLocal);

  return tCWLLocal;
} // GettWL

UINT32
GettCL(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tCL

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tCL

--*/
{
  UINT32              tCLLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tCLLocal = (*channelNvList)[ch].common.nCL;

  CheckBounds(host, tCL, &tCLLocal);

  return tCLLocal;
} // GettCL

UINT32
GettRP(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRP

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRP

--*/
{
  UINT32              tRPLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRPLocal = (*channelNvList)[ch].common.nRP;

  CheckBounds(host, tRP, &tRPLocal);

  return tRPLocal;
} // GettRP

UINT32
GettRC(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRC

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRC

--*/
{
  UINT32              tRCLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRCLocal = (*channelNvList)[ch].common.nRC;

  CheckBounds(host, tRC, &tRCLocal);

  return tRCLocal;
} // GettRC

UINT32
GettRCD(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRP

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRCD

--*/
{
  UINT32              tRCDLocal = 0;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRCDLocal = (*channelNvList)[ch].common.nRCD;

  CheckBounds(host, tRCD, &tRCDLocal);

  return tRCDLocal;
} // GettRCD

UINT32
GettRRD_L(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRRD_L

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRRD_L

--*/
{
  UINT32              tRRD_LLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRRD_LLocal = (*channelNvList)[ch].common.nRRD_L;

  CheckBounds(host, tRRD_L, &tRRD_LLocal);

  return tRRD_LLocal;
} // GettRRD_L

UINT32
GettRRD(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRRD

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRRD

--*/
{
  UINT32              tRRDLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tRRDLocal = (*channelNvList)[ch].common.nRRD;

  CheckBounds(host, tRRD, &tRRDLocal);

  return tRRDLocal;
} // GettRRD

UINT32
GettWTR_L(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tWTR_L

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWTR_L

--*/
{
  UINT32              tWTR_LLocal;

  tWTR_LLocal = (75000 + (GettCK(host, socket) - 100)) / GettCK(host, socket);

  if (tWTR_LLocal < 4) {
    tWTR_LLocal = 4;
  }

  CheckBounds(host, tWTR_L, &tWTR_LLocal);

  return tWTR_LLocal;
} // GettWTR_L

UINT32
GettPRPDEN (
PSYSHOST  host,
UINT8     socket
)
/*++

This function returns the value of tPRPDEN

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tWTR_L

--*/
{
  UINT32              tPRPDEN_local;

  tPRPDEN_local = 1;

  if (host->nvram.mem.socket[socket].ddrFreq >= DDR_2133) {
    tPRPDEN_local = 2;
  }
  CheckBounds(host, tPRPDEN, &tPRPDEN_local);

  return tPRPDEN_local;
} // GettPRPDEN

UINT32
GettRDA(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRDA (RdCAS_AP to ACT)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRDA

--*/
{
  UINT32                tRDA_Local;
  struct channelNvram   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tRDA_Local = (*channelNvList)[ch].common.nRP + (*channelNvList)[ch].common.nRTP;

  CheckBounds(host, tRDA, &tRDA_Local);

  return tRDA_Local;
} // GettRDA

UINT32
GettWRA(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tWRA (WrCAS_AP to ACT)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRA

--*/
{
  UINT32                tWRA_Local;
  UINT8                 BL = 8;                     // Burst length
  struct channelNvram   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tWRA_Local = (*channelNvList)[ch].common.nWL + BL/2 + (*channelNvList)[ch].common.nWR + (*channelNvList)[ch].common.nRP;

  CheckBounds(host, tWRA, &tWRA_Local);

  return tWRA_Local;
} // GettWRA

UINT32
GettWRPRE(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tWRPRE (WrCAS to PRE)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRPRE

--*/
{
  UINT32                tWRPRE_Local;
  UINT8                 BL = 8;                   // Burst length
  struct channelNvram   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tWRPRE_Local = (*channelNvList)[ch].common.nWL + BL/2 + (*channelNvList)[ch].common.nWR;

  CheckBounds(host, tWRPRE, &tWRPRE_Local);

  return tWRPRE_Local;
} // GettWRPRE

UINT32
GettRDPDEN(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tRDPDEN (RdCAS to CKE low time)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRDPDEN

--*/
{
  UINT8                 BL = 8;     // Burst length
  UINT8                 timingMode;
  UINT32                tRDPDEN_Local;
  struct channelNvram   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  // Get 1-based CMD timing mode
  switch((*channelNvList)[ch].timingMode) {
    case TIMING_1N:
      timingMode = 1;
      break;
    case TIMING_2N:
      timingMode = 2;
      break;
    case TIMING_3N:
    default:
      timingMode = 3;
      break;
  }

  //RdCAS to CKE low time : tCL + BL/2 + N (1N = 1, 2N = 2, 3N = 3)
  tRDPDEN_Local = (*channelNvList)[ch].common.nCL + BL/2 + timingMode;

  CheckBounds(host, tRDPDEN, &tRDPDEN_Local);

  return tRDPDEN_Local;
} // GettRDPDEN

UINT32
GettWRPDEN(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tWRPDEN (WrCAS to CKE low time)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRPDEN

--*/
{
  UINT32                tWRPDEN_Local;
  UINT8                 BL = 8;                   // Burst length
  struct channelNvram   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  // WrCAS to CKE low time : tWL + BL/2 + tWR
  tWRPDEN_Local = (*channelNvList)[ch].common.nWL + BL/2 + (*channelNvList)[ch].common.nWR;

  CheckBounds(host, tWRPDEN, &tWRPDEN_Local);

  return tWRPDEN_Local;
} // GettWRPDEN

void
CheckBounds(
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
  CHIP_FUNC_CALL(host, CheckBoundsChip(host, timing, value));
} // CheckBounds
