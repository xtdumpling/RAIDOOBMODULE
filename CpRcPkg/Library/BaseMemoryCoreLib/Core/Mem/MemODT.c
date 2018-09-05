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
 *
 ************************************************************************/

#include "SysFunc.h"

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

#define  MCODT_50       0
#define  MCODT_100      1

#define  MCODT_ACT      1
#define  ODT_ACT        1
#define  NO_ACT         0

UINT16 LrdimmCodeDeCodeDrvStr (
                          UINT16    value,
                          UINT8     code
                        )
{
  UINT16 convertODT = 0;

  if (CODE_ODT == code) {
    // get Register value and convert it to ODT value
   switch (value){
     case 0:
       convertODT = DDR4_RON_40;
       break;
     case 1:
       convertODT = DDR4_RON_34;
       break;
     case 2:
       convertODT = DDR4_RON_48;
       break;
     default:
       convertODT = 0;
       break;
   } // end switch value
  } else {
    // get ODT value and convert it to Register value
    switch (value){
      case DDR4_RON_40:
        convertODT = 0;
        break;
      case DDR4_RON_34:
        convertODT = 1;
        break;
      case DDR4_RON_48:
        convertODT = 2;
        break;
      default:
        convertODT = 0;
        break;
    } // end switch value
  } // if code

  return convertODT;
} //LrdimmCodeDeCodeDrvStr


UINT16 CodeDeCodeDrvStr (
                         UINT16    value,
                         UINT8     code,
                         UINT8     dimmtype
                        )
{
  UINT16 convertODT = 0;

#ifdef  LRDIMM_SUPPORT
  if (dimmtype == LRDIMM) {
    convertODT = LrdimmCodeDeCodeDrvStr (value, code);
  } else
#endif
  {
    if (CODE_ODT == code) {
      // get Register value and convert it to ODT value
      switch (value){
        case 0:
          convertODT = DDR4_RON_34;
          break;
        case 1:
          convertODT = DDR4_RON_48;
          break;
        default:
          convertODT = 0;
          break;
      } // end switch value
    } else {
      // get ODT value and convert it to Register value
      switch (value){
        case DDR4_RON_34:
          convertODT = 0;
          break;
        case DDR4_RON_48:
          convertODT = 1;
          break;
        default:
          convertODT = 0;
          break;
      } // end switch value
    } // if code
  }


  return convertODT;
} // CodeDeCodeDrvStr

UINT16  CodeDeCodeNomParkOdt (
                               UINT16 value,
                               UINT8  code
                             )
{
  UINT16 convertODT = 0;

  if (CODE_ODT == code) {
    // get Register value and convert it to ODT value
    switch (value){
      case 0:
        convertODT = 0;
        break;
      case DDR4_RTT_NOM_60:
        convertODT = 60;
        break;
      case DDR4_RTT_NOM_120:
        convertODT = 120;
        break;
      case DDR4_RTT_NOM_40:
        convertODT = 40;
        break;
      case DDR4_RTT_NOM_240:
        convertODT = 240;
        break;
      case DDR4_RTT_NOM_48:
        convertODT = 48;
        break;
      case DDR4_RTT_NOM_80:
        convertODT = 80;
        break;
      case DDR4_RTT_NOM_34:
        convertODT = 34;
        break;
      default:
        convertODT = 0;
    } // end switch value
  } else {
    // get ODT value and convert it to Register value
    switch (value){
      case 0:
        convertODT = 0;
        break;
      case 60:
        convertODT = DDR4_RTT_NOM_60;
        break;
      case 120:
        convertODT = DDR4_RTT_NOM_120;
        break;
      case 40:
        convertODT = DDR4_RTT_NOM_40;
        break;
      case 240:
        convertODT = DDR4_RTT_NOM_240;
        break;
      case 48:
        convertODT = DDR4_RTT_NOM_48;
        break;
      case 80:
        convertODT = DDR4_RTT_NOM_80;
        break;
      case 34:
        convertODT = DDR4_RTT_NOM_34;
        break;
      default:
        convertODT = 0;
    } // end switch value
  } // if code

  return convertODT;
} // CodeDeCodeNomParkOdt


UINT16 LrdimmCodeDeCodeWrOdt (
                         UINT16    value,
                         UINT8     code
                       )
{
  UINT16 convertODT = 0;

  if (CODE_ODT == code) {
  // get Register value and convert it to ODT value
    switch (value){
      case 0:
        convertODT = RTTWR_DIS;
        break;
      case 1:
        convertODT = RTTWR_60;
        break;
      case 2:
        convertODT = RTTWR_120;
        break;
      case 4:
        convertODT = RTTWR_240;
        break;
      case 6:
        convertODT = RTTWR_80;
        break;
      case 7: 
        convertODT = RTTWR_INF;
        break;
      default:
        convertODT = 0;
        break;
    } // end switch value
  } else {
    // get ODT value and convert it to Register value
    switch (value){
      case RTTWR_DIS:
        convertODT = 0;
        break;
      case RTTWR_60:
        convertODT = 1;
        break;
      case RTTWR_120:
        convertODT = 2;
        break;
      case RTTWR_240:
        convertODT = 4;
        break;
      case RTTWR_80:
        convertODT = 6;
        break;
      case RTTWR_INF:
        convertODT = 7;
        break;
      default:
        convertODT = 0;
        break;
    } // end switch value
  } // if code
  return convertODT;
} //LrdimmCodeDeCodeWrOdt



UINT16 CodeDeCodeWrOdt (
                         UINT16    value,
                         UINT8     code,
                         UINT8     dimmtype
                       )
{
  UINT16 convertODT = 0;

#ifdef  LRDIMM_SUPPORT
  if (dimmtype == LRDIMM) {
    convertODT = LrdimmCodeDeCodeWrOdt (value, code);
  } else
#endif
  {
    if (CODE_ODT == code) {
     // get Register value and convert it to ODT value
     switch (value){
        case 0:
          convertODT = RTTWR_DIS;
          break;
        case 1:
          convertODT = RTTWR_120;
          break;
        case 2:
          convertODT = RTTWR_240;
          break;
        case 4: 
          convertODT = RTTWR_80;
          break;
        case 3: 
          convertODT = RTTWR_INF;
          break;
        default:
          convertODT = 0;
          break;
      } // end switch value
    } else {
      // get ODT value and convert it to Register value 
      switch (value){
        case RTTWR_DIS:
          convertODT = 0;
          break;
        case RTTWR_120:
          convertODT = 1;
          break;
        case RTTWR_240:
          convertODT = 2;
          break;
        case RTTWR_80:
          convertODT = 4;
          break;
        case RTTWR_INF:
          convertODT = 3;
          break;
        default:
          convertODT = 0;
          break;
      } // end switch value
    } // if code
  }

  return convertODT;
} // CodeDeCodeWrOdt

UINT16 ConvertOdtValue (
                         PSYSHOST  host,
                         GSM_GT    group,
                         UINT16    value,
                         UINT8     code,
                         UINT8     dimmtype
                       )
{
  UINT16 convertODT = 0;

  switch (group) {
    case RxOdt:
      convertODT = CHIP_FUNC_CALL(host, CodeDeCodeRxOdt     (value, code));
      break;
    case TxRon:
      convertODT = CHIP_FUNC_CALL(host, CodeDeCodeTxRon     (value, code));
      break;
    case DramDrvStr:
      convertODT = CodeDeCodeDrvStr    (value, code, dimmtype);
      break;
    case NomOdt:
    case ParkOdt:
      convertODT = CodeDeCodeNomParkOdt (value, code);
      break;
    case WrOdt:
      convertODT = CodeDeCodeWrOdt      (value, code, dimmtype);
      break;
    default:
      convertODT = value;
      break;
  } // end switch group

  return convertODT;
} // ConvertOdtValue


//---------------------------------------------------------------
/**
  Get the ODT Value Index

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param ch          - Channel number (0-based)
  @param freqLimit   - Frequency Limit

  @retval N/A
**/
ODT_VALUE_INDEX GetOdtValueIndex (
                                 PSYSHOST host,
                                 UINT8    socket,
                                 UINT8    ch,
                                 UINT8    freqLimit
                                 )
{
  ODT_VALUE_INDEX config;
  UINT8 d;
  UINT8 slot = 0;

  config.Data = CHIP_FUNC_CALL(host, SetOdtConfigInOdtValueIndex ( host, socket));

  if (config.Bits.freq > freqLimit) config.Bits.freq = freqLimit;
  for (d = 0; d < MAX_DIMM; d++) {

    if (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].dimmPresent) {

      if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d))){
        slot = LR_DIMM;
      } else {
        switch (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].numRanks) {
        case 1:
          slot = SR_DIMM;
          break;
        case 2:
          slot = DR_DIMM;
          break;
        case 4:
          slot = QR_DIMM;
          break;
        default:
          // error
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_80);
          } // end switch
        } // if LRDIMM
    } else slot = EMPTY_DIMM;

    switch (d) {
    case 0:
      config.Bits.slot0 = slot;
      break;
    case 1:
      config.Bits.slot1 = slot;
      break;
    case 2:
      config.Bits.slot2 = slot;
      break;
    default:
      // error
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_81);
      } // end switch
   } // for dimm
   return config;
} // GetOdtValueIndex

//---------------------------------------------------------------
/**
  Get the ddr4OdtValueStruct

  @param host            - Pointer to sysHost
  @param socket          - Socket number
  @param ch              - Channel number

  @retval Pointer to the ddr4OdtValueStruct
**/
struct ddr4OdtValueStruct *LookupDdr4OdtValue (
                                              PSYSHOST host,
                                              UINT8    socket,
                                              UINT8    ch
  )
{
  MRC_STATUS                  status;
  ODT_VALUE_INDEX             config;
  UINT32                      i;
  UINT16                      ddr4OdtValueStructSize;
  struct ddr4OdtValueStruct   *ddr4OdtValueStructPtr;

  status = CHIP_FUNC_CALL(host, GetDdr4OdtValueTable(host, &ddr4OdtValueStructPtr, &ddr4OdtValueStructSize));
  if (status != MRC_STATUS_SUCCESS) {
    return NULL;
  }

  OemLookupDdr4OdtTable(host, &ddr4OdtValueStructPtr, &ddr4OdtValueStructSize);

  // Construct config index
  config = GetOdtValueIndex (host, socket, ch, DDR_FREQ_LIMIT);

  // Now lookup the config...
  for (i = 0; i < ddr4OdtValueStructSize; i++) {
    if (ddr4OdtValueStructPtr->config == config.Data) {
      break;
    }
    ddr4OdtValueStructPtr++;
  }
  // Check for error
  if (i >= ddr4OdtValueStructSize) {
    OutputWarning (host, ERR_MRC_STRUCT, ERR_ODT_STRUCT, socket, ch, 0xFF, 0xFF);
    return NULL;
  }

  // Return pointer
  return ddr4OdtValueStructPtr;
}

//---------------------------------------------------------------
/**
  Get the odtActStruct

  @param host            - Pointer to sysHost
  @param socket          - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number

  @retval Pointer to the structure for odtActStruct
**/
struct odtActStruct *LookupOdtAct (
                                  PSYSHOST host,
                                  UINT8    socket,
                                  UINT8    ch,
                                  UINT8    dimm,
                                  UINT8    rank
  )
{
  ODT_ACT_INDEX   config;
  ODT_VALUE_INDEX configVal;
  UINT8 d;
  UINT8 slot = 0;
  UINT32 i;
  UINT32 odtActTableSize;
  struct odtActStruct *odtActTablePtr = NULL;

  CHIP_FUNC_CALL(host, GetOdtActTablePointers (host, &odtActTableSize, &odtActTablePtr));

  configVal.Data = CHIP_FUNC_CALL(host, SetOdtConfigInOdtValueIndex (host, socket));
  config.Data = 0;
  config.Bits.slot0 = configVal.Bits.slot0;
  config.Bits.slot1 = configVal.Bits.slot1;
  config.Bits.slot2 = configVal.Bits.slot2;
  config.Bits.dimmNum = dimm;
  config.Bits.rankNum = rank;
  for (d = 0; d < MAX_DIMM; d++) {

    if (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].dimmPresent) {

      //SKX TODO: Fix Emulation
      if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d))){
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].aepDimmPresent) {
          slot = AEP_DIMM_TYPE;
        } else {
          slot = LR_DIMM;
        }
        if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
            config.Bits.rankNum = 0;
        } // if DDR3
      } else {

        switch (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].numRanks) {
          case 1:
            slot = SR_DIMM;
            break;
          case 2:
            slot = DR_DIMM;
            break;
          case 4:
            slot = QR_DIMM;
            break;
          default:
            // error
            RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_82);
        } // end switch
      } // if LRDIMM
    } else slot = EMPTY_DIMM;

    switch (d) {
    case 0:
      config.Bits.slot0 = slot;
      break;
    case 1:
      config.Bits.slot1 = slot;
      break;
    case 2:
      config.Bits.slot2 = slot;
      break;
    default:
      // error
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_83);
    }
  }

  // Now lookup the config...
  for (i = 0; i < odtActTableSize / sizeof(struct odtActStruct); i++) {
    if ((odtActTablePtr + i)->config == config.Data) {
      break;
    }
  }
  // Check for error
  if (i >= odtActTableSize / sizeof(struct odtActStruct)) {
    OutputError (host, ERR_DIMM_COMPAT, ERR_INVALID_POP, socket, ch, dimm, rank);
    return NULL;
  }

  // Return pointer
  return(odtActTablePtr + i);
} // LookupOdtAct

//---------------------------------------------------------------
/**
  Determine if this is a Termination Rank

  @param host            - Pointer to sysHost
  @param socket          - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number

  @retval 0: This is not a Termination Rank
  @retval 1: This is a Termination Rank
**/
UINT32 IsTermRank (
                  PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch,
                  UINT8    dimm,
                  UINT8    rank
                  )
{
  struct odtActStruct *odtActPtr;
  UINT8 d;
  UINT8 r;

  // Check for non-target termination on this rank
  if ((rank < 2) && CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))){
    return 1;
  }
  for (d = 0; d < MAX_DIMM; d++) {
    for (r = 0; r < MAX_RANK_DIMM; r++) {

      if (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].rankList[r].enabled == 0) continue;

      // Skip target rank
      if ((d == dimm) && (r == rank)) continue;

      // Lookup ODT activations for this config
      odtActPtr = LookupOdtAct(host, socket, ch, d, r);

      if (odtActPtr != NULL) {
        if ((odtActPtr->actBits[0] & (BIT0 << ((dimm * MAX_RANK_DIMM) + rank))) ||
            (odtActPtr->actBits[1] & (BIT0 << ((dimm * MAX_RANK_DIMM) + rank)))) {
          return 1;
        }
      }
    }
  }

  return 0;
}

//---------------------------------------------------------------
/**

  Set MCODT value to 50 or 100 ohms

  @param host    - Pointer to sysHost
  @param socket    - Socket number
  @param ch      - Channel number (0-based)
  @param mcodt   - 0 = 50 ohms; 1 = 100 ohms (HW default)

  @retval N/A

**/
void
SetMcOdtValue (
              PSYSHOST host,
              UINT8    socket,
              UINT8    ch,
              UINT8    mcodt
              )
{
} // SetMcOdtValue

//---------------------------------------------------------------
UINT8
GetMcOdtValue (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT16   *mcodt
  )
/*++

  Returns the  MCODT bit values for 50 or 100 ohms

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param ch          - Channel number (0-based)
  @param mcodt       - 0 = 50 ohms; 1 = 100 ohms (HW default)

  @retval The MCODT bit values for 50 or 100 ohms

--*/
{
  UINT8 mcodtBitValue = 0;
  struct ddr4OdtValueStruct *ddr4OdtValuePtr = NULL;

  // Lookup ODT values for this config
  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    ddr4OdtValuePtr = LookupDdr4OdtValue(host, socket, ch);
    if (ddr4OdtValuePtr == NULL) {
       DisableChannel(host, socket, ch);
    } else {
      if (host->setup.mem.setMCODT == MC_ODT_50_OHMS) {
        mcodtBitValue = 3;  // 3 = 50 ohms for DDR4
      } else if (host->setup.mem.setMCODT == MC_ODT_100_OHMS) {
        mcodtBitValue = 1; // 1 = 100 ohms for DDR4
      } else if (host->setup.mem.setMCODT == MC_ODT_AUTO) {
        if (ddr4OdtValuePtr->mcOdt == 0) { // 0 = 50ohms, so converting to 3 to write that to the register
          mcodtBitValue = 3;
        } else {
         mcodtBitValue = 1;  // 1 = 100ohms
        }
      }
    }// end if null ptr
  } else {
     // Set MCODT values for this channel
     mcodtBitValue = 1; // 1 = 50ohms for DDR3
  }

  if (mcodt != NULL) {
    switch(mcodtBitValue) {
    case 1:
      *mcodt = 100;
      break;
    case 3:
      *mcodt = 50;
      break;
    default:
      *mcodt = 0;
      break;
    } // end switch
  } // if null

  return (mcodtBitValue);
} // GetMcOdtValue


//---------------------------------------------------------------
/**

  Programs the ODT matrix

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel number (0-based)

  @retval N/A

**/
void
SetOdtMatrix (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch
             )
{
  UINT8  dimm;
  UINT8  rank;
  UINT8  imcRank;
  UINT8  d;
  UINT8  r;
  UINT8  odtPin;
  UINT32 rdOdtTableData;
  UINT32 wrOdtTableData;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct odtActStruct *odtActPtr;
  struct ddr4OdtValueStruct *ddr4OdtValuePtr = NULL;
  ODT_VALUE_RTT_DDR4  ddr4TableVal;
#ifdef SERIAL_DBG_MSG
  UINT8 ddr4_rtt_wr_lookup[] = {0, 120, 240, 0, 80, 60};
  UINT8 ddr4_rtt_nom_lookup[] = {0, 60, 120, 40, 240, 48, 80, 34};
#endif // SERIAL_DBG_MSG

#ifdef SERIAL_DBG_MSG
  // Debug only!
  /*
if (checkMsgLevel(host, SDBG_MAX)) {
  rcPrintf ((host, "\n"));
  rcPrintf ((host, "Sizeof odtValueStruct: %d\n", sizeof(struct odtValueStruct)));
  rcPrintf ((host, "Sizeof odtValueTable: %d\n", sizeof(odtValueTable)));
  rcPrintf ((host, "Calculated num entries: %d\n", sizeof(odtValueTable) / sizeof(struct odtValueStruct)));

  rcPrintf ((host, "\n"));
  rcPrintf ((host, "Sizeof odtActStruct: %d\n", sizeof(struct odtActStruct)));
  rcPrintf ((host, "Sizeof odtActTable: %d\n", sizeof(odtActTable)));
  rcPrintf ((host, "Calculated num entries: %d\n", sizeof(odtActTable) / sizeof(struct odtActStruct)));
}
   */
#endif // SERIAL_DBG_MSG

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  //
  // Loop for each DIMM
  //
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) {
      continue;
    }

    rdOdtTableData  = 0;
    wrOdtTableData  = 0;

    //
    // Loop for each rank
    //
    rankList = GetRankNvList(host, socket, ch, dimm);
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
        // Lookup ODT values for this config
        ddr4OdtValuePtr = LookupDdr4OdtValue(host, socket, ch);
        if (ddr4OdtValuePtr == NULL) {
          DisableChannel(host, socket, ch);
          continue;
        } else {
          // Set MCODT values for this channel
          SetMcOdtValue (host, socket, ch, ddr4OdtValuePtr->mcOdt);
        }// end if null ptr

        // Get the ODT values
        ddr4TableVal = *(ODT_VALUE_RTT_DDR4 *)&ddr4OdtValuePtr->dramOdt[dimm][rank];

#ifdef  LRDIMM_SUPPORT
        if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))){
          if (rank == 0) {
            //DDR4 LRDIMM, the ODT settings from the table are for the host side ODT
            //The BIOS will only program these values in the NVRAM structure for this DIMM here,
            // the programming of the values is done during the JEDEC init.
            (*channelNvList)[ch].dimmList[dimm].lrBuf_BC00 = (UINT8) ddr4TableVal.Bits.rttNom;
            (*channelNvList)[ch].dimmList[dimm].lrBuf_BC02 = (UINT8) ddr4TableVal.Bits.rttPrk;
            switch (ddr4TableVal.Bits.rttWr) {
            case DDR4_RTT_WR_DIS:
              if ((*channelNvList)[ch].maxDimm == 1) {
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC01 = DDR4_RTT_NOM_60;
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC00 = DDR4_RTT_NOM_60;
              } else {
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC01 = DDR4_RTT_NOM_DIS;
              }
              break;

            case DDR4_RTT_WR_60:
              (*dimmNvList)[dimm].lrBuf_BC01 = DDR4_RTT_NOM_60;
              break;

            case DDR4_RTT_WR_80:
              (*dimmNvList)[dimm].lrBuf_BC01 = DDR4_RTT_NOM_80;
              break;

            case DDR4_RTT_WR_120:
              (*dimmNvList)[dimm].lrBuf_BC01 = DDR4_RTT_NOM_120;
              break;

            case DDR4_RTT_WR_240:
              (*dimmNvList)[dimm].lrBuf_BC01 = DDR4_RTT_NOM_240;
              break;

            case DDR4_RTT_WR_INF:
              (*dimmNvList)[dimm].lrBuf_BC01 = (BIT2 | BIT1 | BIT0);
              break;
            }
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                           "LRDIMM Host Side Rtt_wr = %d, Rtt_park = %d, Rtt_nom = %d\n",
                           ddr4_rtt_nom_lookup[(*channelNvList)[ch].dimmList[dimm].lrBuf_BC01],
                           ddr4_rtt_nom_lookup[ddr4TableVal.Bits.rttPrk],
                           ddr4_rtt_nom_lookup[ddr4TableVal.Bits.rttNom]));
            //Host Interface DQ/DQS Output Driver Impedance Control, set to 34 Ohms  = value of 1.
            (*channelNvList)[ch].dimmList[dimm].lrBuf_BC03 = 1;

            if (((*dimmNvList)[dimm].SPDSpecRev > 7) || ((*dimmNvList)[dimm].aepDimmPresent)) { //4986147: BIOS not setting LR-DIMM backside Vref and ODT according to SPD values
              if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866)  {
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC04 = ((*dimmNvList)[dimm].SPDLrbufDbDsRttLe1866 & (BIT2 + BIT1 + BIT0));
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC05 = (((*dimmNvList)[dimm].SPDLrbufDbDsRttLe1866 >> 4) & (BIT2 + BIT1 + BIT0));
              } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC04 = ((*dimmNvList)[dimm].SPDLrbufDbDsRttGt1866Le2400 & (BIT2 + BIT1 + BIT0));
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC05 = (((*dimmNvList)[dimm].SPDLrbufDbDsRttGt1866Le2400 >> 4) & (BIT2 + BIT1 + BIT0));
              } else {
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC04 = ((*dimmNvList)[dimm].SPDLrbufDbDsRttGt2400Le3200 & (BIT2 + BIT1 + BIT0));
                (*channelNvList)[ch].dimmList[dimm].lrBuf_BC05 = (((*dimmNvList)[dimm].SPDLrbufDbDsRttGt2400Le3200 >> 4) & (BIT2 + BIT1 + BIT0));
              }
            } else {
              //Buffer ODT = 48 Ohms
              (*channelNvList)[ch].dimmList[dimm].lrBuf_BC04 = 5;
              // backside buffer driver impedance control = 34 Ohms = RZQ/7
              (*channelNvList)[ch].dimmList[dimm].lrBuf_BC05 = 1;
            } // if SPDSpecRev
          } // end switch

          if (((*dimmNvList)[dimm].SPDSpecRev > 7) || ((*dimmNvList)[dimm].aepDimmPresent)) { //4986147: BIOS not setting LR-DIMM backside Vref and ODT according to SPD values
            // For SPD Rev > 0.7, read the DRAM ODT values from SPD, otherwise used fixed values based on number of backside ranks
            if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866)  {
              (*rankList)[rank].RttWr = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomLe1866 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_WR_ODT_SHIFT - 3));
              if (rank > 1) {
                //ODT pin on ranks 2 and 3 is GND which makes RttNom = Disabled
                (*rankList)[rank].RttNom = DDR4_RTT_NOM_DIS << DDR4_RTT_NOM_ODT_SHIFT;
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkLe1866 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_PRK_ODT_SHIFT - 3));
              } else {
                (*rankList)[rank].RttNom = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomLe1866 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_NOM_ODT_SHIFT);
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkLe1866 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_PRK_ODT_SHIFT);
              } // if rank
            } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
              (*rankList)[rank].RttWr = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt1866Le2400 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_WR_ODT_SHIFT - 3));
              if (rank > 1) {
                //ODT pin on ranks 2 and 3 is GND which makes RttNom = Disabled
                (*rankList)[rank].RttNom = DDR4_RTT_NOM_DIS << DDR4_RTT_NOM_ODT_SHIFT;
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkGt1866Le2400 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_PRK_ODT_SHIFT - 3));
              } else {
                (*rankList)[rank].RttNom = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt1866Le2400 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_NOM_ODT_SHIFT);
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkGt1866Le2400 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_PRK_ODT_SHIFT);
              } // if rank
            } else {
              (*rankList)[rank].RttWr = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt2400Le3200 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_WR_ODT_SHIFT - 3));
              if (rank > 1) {
                //ODT pin on ranks 2 and 3 is GND which makes RttNom = Disabled
                (*rankList)[rank].RttNom = DDR4_RTT_NOM_DIS << DDR4_RTT_NOM_ODT_SHIFT;
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkGt2400Le3200 & (BIT5 + BIT4 + BIT3)) << (DDR4_RTT_PRK_ODT_SHIFT - 3));
              } else {
                (*rankList)[rank].RttNom = (((*dimmNvList)[dimm].SPDLrbufDramOdtWrNomGt2400Le3200 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_NOM_ODT_SHIFT);
                (*rankList)[rank].RttPrk = (((*dimmNvList)[dimm].SPDLrbufDramOdtParkGt2400Le3200 & (BIT2 + BIT1 + BIT0)) << DDR4_RTT_PRK_ODT_SHIFT);
              } // if rank
            } // if DDR freq
          } else {
             //
             // Ranks 0 and 1 dual rank RTT_WR=240, RTT_NOM=240, RTT_PAR=60
             // Ranks 0 and 1 Quad rank (the ones with ODT pins) RTT_WR=240, RTT_NOM=240, RTT_PAR=34
             // Ranks 2 and 3 (the ones without ODT pins) RTT_WR=240, RTT_NOM=NA, RTT_PAR=240
             //

             //RttWr = 240 Ohms
             (*rankList)[rank].RttWr = DDR4_RTT_WR_240 << DDR4_RTT_WR_ODT_SHIFT;
             if (rank > 1) {
               // ranks 2 and 3
               //RttPrk = 240 Ohms
               (*rankList)[rank].RttPrk = DDR4_RTT_PARK_240 << DDR4_RTT_PRK_ODT_SHIFT;
               //RttNom = Disabled
               (*rankList)[rank].RttNom = DDR4_RTT_NOM_DIS << DDR4_RTT_NOM_ODT_SHIFT;
             } else {
               //RttNom = 240 Ohms
               (*rankList)[rank].RttNom = DDR4_RTT_NOM_240 << DDR4_RTT_NOM_ODT_SHIFT;

               if ((*dimmNvList)[dimm].numDramRanks == 4) {
                 // ranks 0 and 1 for a quad rank dimm
                 //RttPrk = 48 Ohms
                 (*rankList)[rank].RttPrk = DDR4_RTT_PARK_48 << DDR4_RTT_PRK_ODT_SHIFT;
               } else {
                 // ranks 0 and 1 for a dual rank dimm
                 //RttPrk = 60 Ohms
                 (*rankList)[rank].RttPrk = DDR4_RTT_PARK_60 << DDR4_RTT_PRK_ODT_SHIFT;
               } // if numDramRanks
             } // if rank
           } // if SPDSpecRev

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "DRAM Rtt_wr = %d, Rtt_park = %d, Rtt_nom = %d\n",
                         ddr4_rtt_wr_lookup[(*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT],
                         ddr4_rtt_nom_lookup[(*rankList)[rank].RttPrk >> DDR4_RTT_PRK_ODT_SHIFT],
                         ddr4_rtt_nom_lookup[(*rankList)[rank].RttNom >> DDR4_RTT_NOM_ODT_SHIFT]));

        } else
#endif // LRDIMM_SUPPORT
        {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "DRAM Rtt_wr = %d, Rtt_park = %d, Rtt_nom = %d\n",
                       ddr4_rtt_wr_lookup[ddr4TableVal.Bits.rttWr],
                       ddr4_rtt_nom_lookup[ddr4TableVal.Bits.rttPrk],
                       ddr4_rtt_nom_lookup[ddr4TableVal.Bits.rttNom]));

          // Set encodings for DRAM Mode Registers
          (*rankList)[rank].RttWr = ddr4TableVal.Bits.rttWr << DDR4_RTT_WR_ODT_SHIFT;
          (*rankList)[rank].RttPrk = ddr4TableVal.Bits.rttPrk << DDR4_RTT_PRK_ODT_SHIFT;
          (*rankList)[rank].RttNom = ddr4TableVal.Bits.rttNom << DDR4_RTT_NOM_ODT_SHIFT;
        } // if LRDIMM
     } else { // DDR4
       CHIP_FUNC_CALL(host, SetDdr3OdtMatrix (host, socket, ch, dimm, rank));
     } // end if DDR3

      // Lookup ODT activations for this config
      odtActPtr = LookupOdtAct(host, socket, ch, dimm, rank);

      // Set encodings for ODT signals in IMC registers
      for (d = 0; d < (*channelNvList)[ch].maxDimm; d++) {
        if ((*dimmNvList)[d].dimmPresent == 0) continue;
        for (r = 0; r < host->var.mem.socket[socket].maxRankDimm; r++) {

          // Skip if no rank
          if ((*rankList)[r].enabled == 0) continue;
          // Get ODT signal associated with this rank
          odtPin = host->nvram.mem.socket[socket].channelList[ch].dimmList[d].rankList[r].ODTIndex;
          imcRank = d * MAX_RANK_DIMM + r;

          // Set ODT signals for target read
          if ((odtActPtr !=  NULL) && (odtActPtr->actBits[RD_TARGET] & (1 << imcRank))) {
             if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) &&
                 CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d))) {
               if ((*channelNvList)[ch].encodedCSMode == 1) {
                 rdOdtTableData |= (1 << ((rank & BIT0) * 8 + odtPin));
               } else if ((*channelNvList)[ch].encodedCSMode != 2) {
                 rdOdtTableData |= (1 << (rank * 8 + odtPin));
               }
          } else {
              rdOdtTableData |= (1 << (rank * 8 + odtPin));
            }
          }
          // Set ODT signals for target write
          if ((odtActPtr !=  NULL) && (odtActPtr->actBits[WR_TARGET] & (1 << imcRank))) {
             if ((host->nvram.mem.dramType != SPD_TYPE_DDR3) &&
                 CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d))) {
               if ((*channelNvList)[ch].encodedCSMode == 1) {
                 wrOdtTableData |= (1 << ((rank & BIT0) * 8 + odtPin));
               } else if ((*channelNvList)[ch].encodedCSMode != 2) {
                 wrOdtTableData |= (1 << (rank * 8 + odtPin));
               }
            } else {
              wrOdtTableData |= (1 << (rank * 8 + odtPin));
            }
          }

        } // matrix rank
      } // matrix dimm
    } // target rank

     if ((*channelNvList)[ch].encodedCSMode == 2){
       switch (dimm) {
         case 0:
           rdOdtTableData = 0x102;
           break;
         case 1:
           rdOdtTableData = 0x408;
           break;
         case 2:
           rdOdtTableData = 0x1020;
           break;
       }
       wrOdtTableData = 0;
    }
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Activations: Read: 0x%x Write: 0x%x\n", rdOdtTableData,wrOdtTableData));
    CHIP_FUNC_CALL(host, SetOdtActMatrixRegs (host, socket, ch, dimm, rdOdtTableData, wrOdtTableData));
  } // target dimm
}
