/*--
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
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "MemFuncChip.h"
#include "MemAddrMap.h"
#include "MemApiSkx.h"
#include "KtiSetupDefinitions.h"
#include "MmrcProjectDefinitionsGenerated.h"
#include "MemAddrMap.h"

#define RAS_SKIP_CHANNEL                          0
#define RAS_IS_CAPABLE                            1
#define RAS_NOT_CAPABLE                           2
#define MAX_PARTIAL_MIRROR                        4
#define SAD0INDEX                                 0
#define TAD0INDEX                                 0
#define MIN_TAD_GRAN                              2 //64MB units
#define PCUDATA_CLOCK_UNITS                       64
#define TOTAL_TIME_MEM_SCRUB                      95367
#define DAY_FACTOR                                10
#define DCLK_DAY_FACTOR                           100
#define DCLK_TIME_UNIT                            8
#define SAD_NOT_ENABLED                           0
#define SAD_NOT_ELIGIBLE_MIRROR                   1
#define SAD_ELIGIBLE_MIRROR                       2
#define MIRROR_STATUS_SUCCESS                     0
#define MIRROR_STATUS_MIRROR_INCAPABLE            1
#define MIRROR_STATUS_VERSION_MISMATCH            2
#define MIRROR_STATUS_INVALID_REQUEST             3
#define MIRROR_STATUS_UNSUPPORTED_CONFIG          4
#define MIRROR_STATUS_OEM_SPECIFIC_CONFIGURATION  5
#define STATIC_VLS                                0
#define ADAPTIVE_VLS                              1
#define TAD_MIRRORED                              1
#define TAD_NON_MIRRORED                          2

#define ECC_MODE_REG_LIST      {ECC_MODE_RANK0_MC_MAIN_REG, ECC_MODE_RANK1_MC_MAIN_REG, ECC_MODE_RANK2_MC_MAIN_REG, ECC_MODE_RANK3_MC_MAIN_REG, ECC_MODE_RANK4_MC_MAIN_REG, ECC_MODE_RANK5_MC_MAIN_REG, ECC_MODE_RANK6_MC_MAIN_REG, ECC_MODE_RANK7_MC_MAIN_REG}
#define ECC_MODE_REG_EXT_LIST  {ECC_MODE_RANK0_MC_MAINEXT_REG, ECC_MODE_RANK1_MC_MAINEXT_REG, ECC_MODE_RANK2_MC_MAINEXT_REG, ECC_MODE_RANK3_MC_MAINEXT_REG, ECC_MODE_RANK4_MC_MAINEXT_REG, ECC_MODE_RANK5_MC_MAINEXT_REG, ECC_MODE_RANK6_MC_MAINEXT_REG, ECC_MODE_RANK7_MC_MAINEXT_REG}

//SAD with memory hole and limit of 4GB. 4 << 4 is actually (4 << 30) >> 26. i.e., 4GB >> 26.
#define SAD_4GB   (4 << 4)
//
// Disable warning for unused input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4702)
#endif

typedef struct {
  UINT8  dimm;
  UINT8  rank;
}SparedRank;


//
// Local Prototypes
//
void   SyncronizeTimings(PSYSHOST host, UINT8 socket);
void   SetMCLinkFail(struct sysHost *host, UINT8 socket);
static void   SetMirrorMode(struct sysHost *host, UINT8 socket);
static void   SetErrorThreshold (struct sysHost *host,  UINT8 socket, UINT8 ch, UINT32 imcCORRERRTHRSHLD01Reg, UINT32 ErrThreshold);
static void   SetSpareMode(struct sysHost *host, UINT8 socket);
static void   SetADDDCMirrorMode(struct sysHost *host, UINT8 socket);
static UINT8  checkSupportedRASModes(struct sysHost *host);

/**

  Find out the spare ranks per channel, and sets the rank size in rankList

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param ch          - Channel number (0-based)
  @retval N/A

**/
static void
SetRankSizeForSpare(
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch
                   )
{
  UINT8               spareRankNum;
  UINT8               rank;
  UINT16              size;
  UINT8               dimm;
  UINT8               k;
  UINT8               j;
  SparedRank    spareLogicalRank[MAX_SPARE_RANK];
  UINT8               sparedimm;
  UINT8               sparerank;
  UINT8               chRankEnabled = 0;
  UINT16              sparesize;
  struct ddrRank      *rankNvList0;
  struct dimmNvram    *dimmNvList0;
  struct channelNvram *channelNvList;

  channelNvList = &(*GetChannelNvList(host, socket))[ch];

  //
  // Find how many ranks need to be spared (must be <= 50% of available ranks)
  //
  for (dimm = 0; (dimm < channelNvList->maxDimm && dimm < MAX_DIMM); dimm++) {
    dimmNvList0 = &(*GetDimmNvList(host, socket, ch))[dimm];
    if (dimmNvList0->dimmPresent == 0) continue;
    if (dimmNvList0->aepDimmPresent == 1) continue;

    for (rank = 0; (rank < dimmNvList0->numRanks && rank < MAX_RANK_DIMM); rank++) {
      rankNvList0 = &channelNvList->dimmList[dimm].rankList[rank];
      if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank] == 0) chRankEnabled++;
    }
  }

  //
  // Max 2 spare ranks per channel
  //
  spareRankNum = chRankEnabled / 2;

  if(host->setup.mem.spareRanks < spareRankNum) {
    spareRankNum = host->setup.mem.spareRanks;
  }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "SpareRankNum:%d, SetupSpareNum: %d, ch Ranks: %d, chRankEnabled: %d\n",
                spareRankNum, host->setup.mem.spareRanks, channelNvList->numRanks, chRankEnabled));

  for (j = 0; j < MAX_SPARE_RANK; j++) {
    spareLogicalRank[j].dimm = 0xff;
    spareLogicalRank[j].rank = 0xff;
  }

  //
  // search all DIMM and ranks to find spare ranks.
  //
  for (k = 0; k < spareRankNum; k++) {

    sparedimm     = 0xff;
    sparerank     = 0xff;
    sparesize     = 0;

    for (dimm = 0; (dimm < channelNvList->maxDimm && dimm < MAX_DIMM); dimm++) {

      dimmNvList0 = &(*GetDimmNvList(host, socket, ch))[dimm];

      if (dimmNvList0->dimmPresent == 0) continue;

      //
      // SKX IMC Rank Sparing is for DDR4 ranks only, skip NVM DIMM/ranks
      //
      if (dimmNvList0->aepDimmPresent == 1) continue;

      //
      // Correct rank size for RAS mode
      // Loop for each rank
      //
      for (rank = 0; (rank < dimmNvList0->numRanks && rank < MAX_RANK_DIMM); rank++) {
        rankNvList0 = &channelNvList->dimmList[dimm].rankList[rank];

        //
        // Skip ranks that are mapped out
        //
        if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

        //
        // If this rank has been selected, go ahead.
        //
        for (j = 0; j < MAX_SPARE_RANK; j++) {
          if ((spareLogicalRank[j].dimm == dimm) && (spareLogicalRank[j].rank == rank)) break;
        }

        //
        // This rank has been logged, skip it.
        //
        if (j < MAX_SPARE_RANK) continue;

        size = rankNvList0->rankSize;

        if (size > sparesize) {
          sparesize = size;
          sparedimm = dimm;
          sparerank = rank;
        }
      } // rank loop
    } // dimm loop

    if (sparedimm != 0xff) {
      spareLogicalRank[k].dimm = sparedimm;
      spareLogicalRank[k].rank = sparerank;
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     " spared dimm and rank:0x%x  0x%x \n", sparedimm, sparerank));
    }
  } //spare rank loop


  for (k = 0; k < (MAX_SPARE_RANK); k++) {
    //
    // Spare is rank on largest dimm. If more than one largest choose rank 0 on the minimum largest dimm
    //
    if (spareLogicalRank[k].dimm == 0xff) {
      channelNvList->spareDimm[k]  = 0xff;
      channelNvList->spareRankSize[k] = 0;
      channelNvList->spareLogicalRank[k] = 0xff;
      channelNvList->sparePhysicalRank[k] = 0xff;
      continue;
    }
    sparedimm                 = spareLogicalRank[k].dimm;
    sparerank                 = spareLogicalRank[k].rank;
    rankNvList0               = &channelNvList->dimmList[sparedimm].rankList[sparerank];
    channelNvList->spareDimm[k]  = sparedimm;
    channelNvList->spareRank[k]  = sparerank;
    channelNvList->spareRankSize[k] = rankNvList0->rankSize;
    channelNvList->spareLogicalRank[k] = rankNvList0->logicalRank;
    channelNvList->sparePhysicalRank[k] = rankNvList0->phyRank;
    rankNvList0->rankSize     = 0;
    rankNvList0->remSize      = 0;


    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Spare DIMM %d, Spare logical rank %d, Spare phy rank id %d, ch spare logical rank:0x%x \n",
                   sparedimm, rankNvList0->logicalRank, rankNvList0->phyRank,
                   channelNvList->spareLogicalRank[k]
                  ));
  } // spare rank loop

}


/**

  Sets the rank size in rankList

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param ch          - Channel number (0-based)
  @param sizeFactor  - Size factor based on the RAS mode
  @param RASMode     - Current RAS mode

  @retval N/A

**/
static void
SetRankSize (
            PSYSHOST host,
            UINT8    socket,
            UINT8    ch,
            UINT8    sizeFactor,
            UINT8    RASMode
            )
{

  UINT8               i;
  UINT16              size;
  UINT8               dimm;

  struct ddrRank      *rankNvList0;
  struct dimmNvram    *dimmNvList0;
  struct channelNvram *channelNvList;


  channelNvList = &(*GetChannelNvList(host, socket))[ch];

  //
  // skip if channel doesn't exist
  //
  if (!channelNvList->enabled) return ;

  for (dimm = 0; dimm < channelNvList->maxDimm; dimm++) {

    dimmNvList0 = &(*GetDimmNvList(host, socket, ch))[dimm];

    if (dimmNvList0->dimmPresent == 0) continue;

    //
    // Correct rank size for RAS mode
    // Loop for each rank
    //
    for (i = 0; i < dimmNvList0->numDramRanks; i++) {
      rankNvList0 = &channelNvList->dimmList[dimm].rankList[i];

      //
      // Skip ranks that are mapped out
      //
      if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[i] == 1) continue;

      //
      // Determine rank size
      //
      size = rankNvList0->rankSize;

      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, i, NO_STROBE, NO_BIT,
                     "size %d\nTechIndex 0x%x, size 0x%x\n", size, dimmNvList0->techIndex, rankSizeDDR4[dimmNvList0->techIndex]));
      //
      // Correct for RAS mode
      //
      rankNvList0->rankSize = sizeFactor * size;
      rankNvList0->remSize  = sizeFactor * size;
    } // rank loop
  } // dimm loop
}
/**

  Verifes the the rank cfg for SVLS setup..

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval Success if cfg is supported
          UnSupported if any issue with rank setup.

**/
UINT32
CheckSvlsCfgSupport(  PSYSHOST host,
  UINT8    socket
){

  UINT8 dimm = 00;
  UINT8 rank = 00;
  UINT8 ch = 00;
  UINT8 X4PresentFlag=0;
  UINT32  Status = SUCCESS;

  for (ch = 0; ch < MAX_CH; ch++) {
    if (host->nvram.mem.socket[socket].channelList[ch].enabled == 0) continue; // Channel not enabled, skip it

    //
    // no svls support for x4 rank.
    //
    if((host->nvram.mem.socket[socket].channelList[ch].features & X4_PRESENT)== X4_PRESENT) {
      X4PresentFlag = 1;
    }

    if ( (host->nvram.mem.socket[socket].channelList[ch].dimmList[0].dimmPresent == 1) &&
        (host->nvram.mem.socket[socket].channelList[ch].dimmList[1].dimmPresent == 1) ) {

      if ( (host->nvram.mem.socket[socket].channelList[ch].dimmList[0].aepDimmPresent) || (host->nvram.mem.socket[socket].channelList[ch].dimmList[1].aepDimmPresent)){
        //both dimms are present and one of them is AEP  - cannot enable VLS here
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "CheckSvlsCfgSupport: Both AEP and DDR4 dimm on same channel:%d No VLS\n",ch));
        Status = FAILURE;
        break;
      }

      //
      //both dimms populated, if ranks are not equal or rank map out return
      //
      dimm = 00;
      if( (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numRanks) !=
          (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm+1].numRanks) ) return FAILURE;

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        //
        // if rank is map out return
        //
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled ) {
          if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank]) return FAILURE;
        }
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm+1].rankList[rank].enabled ) {
          if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm+1].mapOut[rank]) return FAILURE;
        }
      } // rank loop
    } else {

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        // single dimm case check rank is not map out and minimum 2 ranks exist.

        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmPresent == 0) {
          continue;
        }

        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].aepDimmPresent == 1) {
          if(X4PresentFlag) {
            X4PresentFlag = 0;
          }
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "CheckSvlsCfgSupport: AEP dimm in this channel:%d skipping the channel\n",ch));
          break;//This is a single dimm case if one of the Slot is aep then ignore that sVLS enabled on ch with ddr4dimm
        }
        if(host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numRanks <= 1) return FAILURE;
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled ) {
            if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank]) return FAILURE;   // return if rank is map out
          }
        } // rank loop
      } // dimm loop
    }

    if (X4PresentFlag) {
      return FAILURE;
    }
  } // ch loop
  return Status;
}

/**

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param VLS_MODE     - Which VLS Mode are we checking the populatin for SVLS or Adaptive VLS

  @retval CAPABLE     - Population meets requirements or not

**/
UINT8
checkVLSpopulation (
    PSYSHOST host,
    UINT8    socket,
    UINT8    VLS_MODE
  )
{
  UINT8 ch;
  struct channelNvram *channelNvList;
  UINT8   capable = RAS_IS_CAPABLE;

  if(VLS_MODE == STATIC_VLS) { //Static VLS only x8 dimms in system
    if ((CheckSvlsCfgSupport(host, socket)) != EFI_SUCCESS) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t sVLS is not support supported\n"));
      capable = RAS_NOT_CAPABLE;
    }
  } else if(VLS_MODE == ADAPTIVE_VLS) { // Adaptive VLS  only x4 dimms in the system
    for (ch = 0; ch < MAX_CH; ch++) {
      channelNvList = &host->nvram.mem.socket[socket].channelList[ch];

      if (channelNvList->enabled == 0) continue;

      if ((channelNvList->features & X8_PRESENT)==X8_PRESENT) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\t No SDDC/ADDDC because of X8 Dimms \n"));
        capable = RAS_NOT_CAPABLE;
      }
    }
  }

  return capable;
}


static void
CheckExtendedRASModes (PSYSHOST host)
{
  UINT8 setupRASmodeEx;
  UINT8 supportedRASModesEx;
  UINT8 missingRASModesEx;
  UINT8 socket;
  UINT8 imc;
  UINT8 channel;
  UINT8 dimm;
  UINT8 imcCount = 0;
  UINT8 channelCount = 0;
  BOOLEAN exactly2channels = 0;

  // Get supported RAS Modes
  setupRASmodeEx = host->setup.mem.RASmodeEx;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\t setupRASModes = %d\n", setupRASmodeEx));
  supportedRASModesEx = host->var.mem.RASModesEx;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\t supportedRASModesEx = %d\n", supportedRASModesEx));

  //
  // Disable SDDC plus one if in Independent mode with X8 DIMMs
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    if (checkVLSpopulation (host, socket, ADAPTIVE_VLS) == RAS_NOT_CAPABLE) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t Disable ADDDC DIMM Config BAD \n"));
      supportedRASModesEx &= ~ADDDC_EN;
      supportedRASModesEx &= ~ADDDC_ERR_INJ_EN;
      if (!(host->var.mem.RASModes & STAT_VIRT_LOCKSTEP)) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\t Disable SDDC DIMM Config BAD \n"));
        supportedRASModesEx &= ~SDDC_EN;
      }
    }
  }

  //If Mirroring is enabled, we disable ADDDC
  if (host->setup.mem.RASmode & CH_ALL_MIRROR) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\t Warning!! Mirror and ADDDC Cannot be enabled together! Will attempt to enable Mirroring only \n", host->nvram.mem.RASmodeEx));

    supportedRASModesEx &= ~ADDDC_EN;
    supportedRASModesEx &= ~SDDC_EN;
    supportedRASModesEx &= ~ADDDC_ERR_INJ_EN;
  }

  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    if(host->setup.kti.XptPrefetchEn == KTI_ENABLE) {
      for(socket = 0; socket < MAX_SOCKET; socket++) {
        for(imc = 0; imc < MAX_IMC; imc++) {
          if(host->nvram.mem.socket[socket].imc[imc].enabled == 1) {
            //more than one IMC
            imcCount++;
          }
        } // imc loop
        channelCount = 0;
        for(channel = 0; channel < MAX_CH; channel++) {
          //If we are looking at channels 3, 4, and 5, start the count over, we are on a different IMC
          if((channel % MAX_MC_CH) == 0) {
            channelCount = 0;
          }
          if(host->nvram.mem.socket[socket].channelList[channel].enabled == 1) {
            for(dimm = 0; dimm < MAX_DIMM; dimm++){
              if((host->nvram.mem.socket[socket].channelList[channel].dimmList[dimm].dimmPresent == 1) &&
                 (host->nvram.mem.socket[socket].channelList[channel].dimmList[dimm].aepDimmPresent == 0)) {
                //at least 1 IMC has exactly 2 ddr4 channels populated
                channelCount++;
                break;
              }
            } // dimm
          }
          //check for 2 populated channels only after looking at the last channel
          if ((channelCount == 2) && ((channel % MAX_MC_CH) == (MAX_MC_CH - 1))) {
            exactly2channels = 1;
          }
        } // channel
      } // socket
    } // prefetch
    if((imcCount > 1) && (exactly2channels == 1)) {
      supportedRASModesEx &= ~ADDDC_EN;
      supportedRASModesEx &= ~SDDC_EN;
      supportedRASModesEx &= ~PTRLSCRB_EN;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t Disable ADDDC: more than 1 IMC and an IMC has exactly two channels populated\n"));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t Disable SDDC: more than 1 IMC and an IMC has exactly two channels populated\n"));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t Disable Patrol Scrub: more than 1 IMC and an IMC has exactly two channels populated\n"));
    }
  }

  host->nvram.mem.RASmodeEx = setupRASmodeEx & supportedRASModesEx;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\t host->nvram.mem.RASmodeEx = %d\n", host->nvram.mem.RASmodeEx));

  missingRASModesEx = setupRASmodeEx & (~supportedRASModesEx);

  // Check if patrol scrubbing supported
  if (missingRASModesEx & PTRLSCRB_EN) {
    // Indicate RAS mode disabled
    //LogWarning(host, WARN_PTRLSCRB_DISABLE, 0, (UINT32)-1);
  }

  if (missingRASModesEx & ADDDC_EN) {
    OutputWarning(host, WARN_ADDDC_DISABLE, WARN_ADDDC_MINOR_DISABLE, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  if (missingRASModesEx & SDDC_EN) {
    OutputWarning(host, WARN_SDDC_DISABLE, WARN_SDDC_MINOR_DISABLE, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  return;
}

/**

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RASCheckDimmRanks (
                  PSYSHOST host
                  )
{
  UINT8 socket;
  UINT8 ch;
  UINT8 setupRASModes;
  UINT8 nvramRASModes;
  UINT8 supportedRASModes;
  UINT8 missingRASModes;
  UINT8 RASMode;

  //
  // Get RAS Modes supported by the current DIMM configuration
  //
  supportedRASModes       = checkSupportedRASModes (host);

  //
  // Save supported modes in host
  //
  host->var.mem.RASModes  = supportedRASModes;

  //
  // this function updates the EX RAS MODE based on the setup and supported values.
  //
  CheckExtendedRASModes (host);

  //
  // Check which mode to run by comparing supported RAS capabilities and setup
  //
  setupRASModes = host->setup.mem.RASmode;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nRASCheckDimmRanks: setupRASModes = %d\n", setupRASModes));
  //
  // Sparing and mirror are mutually exclusive: should not both be set in setup
  // If both set, turn off mirroring and issue warning
  //
  if ((setupRASModes & CH_MS) == CH_MS) {
    setupRASModes &= ~FULL_MIRROR_1LM;
    setupRASModes &= ~FULL_MIRROR_2LM;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nSparing is ON, turn off mirroring\n"));
    OutputWarning (host, WARN_MIRROR_DISABLE, WARN_MIRROR_DISABLE_MINOR_RAS_DISABLED, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  nvramRASModes = setupRASModes & supportedRASModes;
  missingRASModes = setupRASModes & (~supportedRASModes);

  //
  // If MIRROR is missing clear both (set to CH_INDEPENDENT) (note: Lock-step is no longer between channels)
  //
  if (missingRASModes & FULL_MIRROR_1LM) {
    nvramRASModes = nvramRASModes &~FULL_MIRROR_1LM;
  }

  RASMode = nvramRASModes;

  if (missingRASModes & FULL_MIRROR_2LM) {
    nvramRASModes = nvramRASModes &~FULL_MIRROR_2LM;
  }

  if (missingRASModes & PARTIAL_MIRROR_1LM) {
    nvramRASModes = nvramRASModes &~PARTIAL_MIRROR_1LM;
  }

  if (missingRASModes & PARTIAL_MIRROR_2LM) {
    nvramRASModes = nvramRASModes &~PARTIAL_MIRROR_2LM;
  }

  RASMode = nvramRASModes;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nRASCheckDimmRanks: RASMode = %d\n", RASMode));
  //
  // Save operational RAS mode in host (nvram and volatile)
  //
  //host->var.mem.OpRASmode = RASMode;
  host->nvram.mem.RASmode = RASMode;

  //
  // Output warning for non-supported modes
  //
  if ((missingRASModes & FULL_MIRROR_1LM) || (missingRASModes & FULL_MIRROR_2LM)) {
    OutputWarning (host, WARN_MIRROR_DISABLE, WARN_MIRROR_DISABLE_MINOR_RAS_DISABLED, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  if ((missingRASModes & PARTIAL_MIRROR_1LM)||(missingRASModes & PARTIAL_MIRROR_2LM)) {
    OutputWarning (host, WARN_PMIRROR_DISABLE, WARN_PMIRROR_DISABLE_MINOR_RAS_DISABLED, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  if (missingRASModes & RK_SPARE) {
    OutputWarning (host, WARN_SPARE_DISABLE, WARN_SPARE_DISABLE_MINOR_RK_SPARE, 0xFF, 0xFF, 0xFF, 0xFF);
  }

  if (missingRASModes & STAT_VIRT_LOCKSTEP) {
    OutputWarning (host, WARN_LOCKSTEP_DISABLE, WARN_LOCKSTEP_DISABLE_MINOR_RAS_MODE, 0xFF, 0xFF, 0xFF, 0xFF);
  }

// Set RANK size for selected mode, includes size accounting for the spare rank size
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    //
    // Set rank size in each ddr channel (MAX_CH will cover all IMCs)
    // and SKX do not lock-step two ddr channels, so no LS adjustment to rank size required
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      SetRankSize (host, socket, ch, 1, RASMode);
    }
  } // socket loop

  return SUCCESS;
}

UINT32
CheckRASSupportAfterMemInit (
  PSYSHOST host
  )
/*++

Routine Description:

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

Arguments:

  host  - Pointer to sysHost

Returns:

  SUCCESS

--*/
{

  UINT8                     dimm;
  UINT8                     rank;
  UINT8                     ch;
  UINT8                     socket;
  UINT32                    dimmAmap;
  AMAP_MC_MAIN_STRUCT       imcAMAPReg;
  DIMMMTR_0_MC_MAIN_STRUCT  dimmMtr;
  struct channelNvram       (*channelNvList)[MAX_CH];
  struct dimmNvram          (*dimmNvList)[MAX_DIMM];

  socket = host->var.mem.currentSocket;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Update dimmmtr.rank_disable setting on mapout results after memtest
  //
  CheckRankPopLocal(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    dimmAmap = 0;

    //
    // If more than 1+ rank is mapped out on a channel use AMAP dimmX_pat_rank_disable to disable scrubbing
    //
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      dimmMtr.Data = MemReadPciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4));

      for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

        // Disable Patrol Scrub when ranks are mapped out and rank_disable is 0

        if ((*dimmNvList)[dimm].mapOut[rank] && ((dimmMtr.Bits.rank_disable & (1 << rank)) == 0)) {

          dimmAmap |= (1 << (dimm * 8 + rank));

          // Update AMAP register
          imcAMAPReg.Data = MemReadPciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG);
          imcAMAPReg.Bits.dimm0_pat_rank_disable |= (UINT8)(dimmAmap & 0xff);
          imcAMAPReg.Bits.dimm1_pat_rank_disable |= (UINT8)((dimmAmap >> 8) & 0xff);
          MemWritePciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG,imcAMAPReg.Data);
        }
      }// rank
    }// dimm
    MemDebugPrint ((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "DimmAmap: 0x%x\n", dimmAmap));
  } // ch

  //
  // Check for spare support after training
  //
  if (host->nvram.mem.RASmode & RK_SPARE) {
    //
    // Adjust for spare ranksize and set spare ranks
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      SetRankSizeForSpare(host, socket, ch);
    }
  }

  return SUCCESS;
}

/**

  Set Primary and secondary channel based on the channel participating in
  SAD and the other channel enabled in the imc.
  Only one other channel is enabled in IMC as A0 Restriction

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param imc   - IMC number

  @retval RAS_IS_CAPABLE
  @retval RAS_NOT_CAPABLE

**/

static VOID
GetPrimarySecondaryMirrorCh (
               PSYSHOST host,
               UINT8    socket,
               UINT8    SadIndex,
               UINT8    Imc,
               UINT8    *PrimaryCh,
               UINT8    *SecondaryCh
               )
{
  UINT8                 ch;
  UINT8                 SktCh;
  UINT8                 TempVal;
  struct socketNvram    *socketNv;
  struct channelNvram   (*channelNvList)[MAX_CH];
  struct SADTable       *SADTablePtr;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "<GetPrimarySecondaryMirrorCh>\n"));

  socketNv  = &host->nvram.mem.socket[socket];
  channelNvList = &socketNv->channelList;

  SADTablePtr = host->var.mem.socket[socket].SAD;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\t SadIndex : %d ,Imc: %d \n", SadIndex, Imc));

  TempVal = SADTablePtr[SadIndex].channelInterBitmap[Imc];
  for(ch = 0; ch < MAX_MC_CH; ch++){
    if(TempVal == 1)
      break;
    TempVal = TempVal >> 1;
  } // ch loop

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\t Primary ch Mirroring: %d \n", ch));
  *PrimaryCh = ch;

  //Find the channel other than primary that is enabled and make it secondary
  for(ch = 0; ch < MAX_MC_CH; ch++){

    if (ch == *PrimaryCh)
      continue;

    SktCh = NODECH_TO_SKTCH(Imc,ch);

    if ((*channelNvList)[SktCh].enabled) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\t Secondary ch Mirroring: %d \n", ch));
      *SecondaryCh = ch;
      break;
    }

  }
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "/<GetPrimarySecondaryMirrorCh>\n"));
  return;
}

/**

  Check if two channels in a IMC is capable for mirroring by checking if DIMMs
  populated in all of those channels are good for mirroring mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param imc   - IMC number

  @retval RAS_IS_CAPABLE
  @retval RAS_NOT_CAPABLE

**/
static UINT8
CheckMirrorPopulation2 (
               PSYSHOST host,
               UINT8    socket,
               UINT8    imc,
               UINT8    ch0,        // at imc level
               UINT8    ch1         // at imc level
               )
{
  UINT8   capable;
  UINT8   dimm;
  UINT8   rank;

  struct socketNvram      *socketNv;
  struct ddrRank          (*rankList0)[MAX_RANK_DIMM];
  struct ddrRank          (*rankList1)[MAX_RANK_DIMM];
  struct dimmNvram        (*dimmNvList0)[MAX_DIMM];
  struct dimmNvram        (*dimmNvList1)[MAX_DIMM];
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct dimmDevice       (*chdimmList0)[MAX_DIMM];
  struct dimmDevice       (*chdimmList1)[MAX_DIMM];

  // convert chx to socket level
  if (imc > 0) {
    ch0 += MAX_MC_CH;
    ch1 += MAX_MC_CH;
  }

  capable = RAS_IS_CAPABLE;

  socketNv  = &host->nvram.mem.socket[socket];
  channelNvList = GetChannelNvList(host, socket);

  //
  // If both channel ch0 and ch1 not enabled then skip channel pair for determining system RAS capability
  //
  if ((*channelNvList)[ch0].enabled == 0 && (*channelNvList)[ch1].enabled == 0) {
    capable = RAS_SKIP_CHANNEL;
  } else if ((*channelNvList)[ch0].enabled != (*channelNvList)[ch1].enabled) {
    capable = RAS_NOT_CAPABLE;
  } else {

    dimmNvList0 = GetDimmNvList(host, socket, ch0);
    dimmNvList1 = GetDimmNvList(host, socket, ch1);
    chdimmList0 = &host->var.mem.socket[socket].channelList[ch0].dimmList;
    chdimmList1 = &host->var.mem.socket[socket].channelList[ch1].dimmList;

    //
    // Compare each pair of DIMMs to ensure not present or match
    //
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList0   = &socketNv->channelList[ch0].dimmList[dimm].rankList;
      rankList1   = &socketNv->channelList[ch1].dimmList[dimm].rankList;

      //
      // Ensure DIMMs in both channels both present or both not present
      //
      if (((*dimmNvList0)[dimm].dimmPresent ^ (*dimmNvList1)[dimm].dimmPresent) != 0) {
        capable = RAS_NOT_CAPABLE;
        break;
      }

      //
      // Ensure DIMMs in both channels both NVMDIMMs or both not NVMDIMMs
      //
      if (((*dimmNvList0)[dimm].aepDimmPresent ^ (*dimmNvList1)[dimm].aepDimmPresent) != 0) {
        capable = RAS_NOT_CAPABLE;
        break;
      }

      //
      // Exactly same 2LM size means NM channel & FM channel of each address will be same (validation scope reduction)
      //
      if ((*dimmNvList0)[dimm].aepDimmPresent) {
        if ((*dimmNvList0)[dimm].volCap != (*dimmNvList1)[dimm].volCap) {
          capable = RAS_NOT_CAPABLE;
          break;
        }
      } else {
        if ((*chdimmList0)[dimm].memSize != (*chdimmList1)[dimm].memSize) {
          capable = RAS_NOT_CAPABLE;
          break;
        }
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (((*rankList0)[rank].enabled ^ (*rankList1)[rank].enabled) != 0) {
            capable = RAS_NOT_CAPABLE;
            break;
          }

          //
          // Ensure ranks in both channels both mapped out or not mapped out
          //
          if (((*dimmNvList0)[dimm].mapOut[rank] ^ (*dimmNvList1)[dimm].mapOut[rank]) != 0) {
            capable = RAS_NOT_CAPABLE;
            break;
          }
        } // rank loop
      }
    } // for (dimm)
  } // if ()

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nCheckMirrorPopulation (Socket = %d, Imc = %d) - ", socket, imc));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 (capable == RAS_IS_CAPABLE)? "Yes\n" : "No\n" ));

  return capable;
}

/**

  Check if a IMC is capable for mirroring by checking if DIMMs populated in all of its
  channels is good for mirroring mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval RAS_IS_CAPABLE
  @retval RAS_NOT_CAPABLE

**/
static UINT8
CheckMirrorPopulation (
               PSYSHOST host,
               UINT8    socket
               )
{
  UINT8 Capable = RAS_NOT_CAPABLE;
  UINT8 ChWay, ChInterBitmap;
  UINT8 SocketImcPresent;
  UINT8 SadIndex;
  UINT8 imc;
  struct SADTable *SADTablePtr;

  SADTablePtr = host->var.mem.socket[socket].SAD;
  for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
    if (!SADTablePtr[SadIndex].Enable)
      break;

    // Determine which IMC channel bitmap to use
    imc = GetMcIndexFromBitmap (SADTablePtr[SadIndex].imcInterBitmap);

    if(SADTablePtr[SadIndex].type == MEM_TYPE_1LM) {
      ChWay = CalculateWaysfromBitmap (SADTablePtr[SadIndex].channelInterBitmap[imc]);
    } else if(SADTablePtr[SadIndex].type == MEM_TYPE_2LM) {
      ChWay = CalculateWaysfromBitmap (SADTablePtr[SadIndex].FMchannelInterBitmap[imc]);
    } else continue; //Do not check the SADs that are not 1LM or 2LM for mirror eligibility


    switch (ChWay) {
      case ONE_WAY: // channel 0; 1 chway
        Capable = RAS_NOT_CAPABLE;
        return Capable;
        break;
      case THREE_WAY: // channel 2, channel 1, channel 0; 3 chway
        for (imc = 0; imc < MAX_IMC; imc++) {
          SocketImcPresent = host->var.mem.socket[socket].SADintList[SadIndex][(socket * host->var.mem.maxIMC)+ imc];

          if (SocketImcPresent) {
            if ((CheckMirrorPopulation2(host, socket, imc, 0, 1) == RAS_IS_CAPABLE) &&
                (CheckMirrorPopulation2(host, socket, imc, 1, 2) == RAS_IS_CAPABLE)) {
                  Capable = RAS_IS_CAPABLE;
            }
          }
        } // imc loop
        break;
      case TWO_WAY:
        for (imc = 0; imc < MAX_IMC; imc++) {
          SocketImcPresent = host->var.mem.socket[socket].SADintList[SadIndex][(socket * host->var.mem.maxIMC)+ imc];

          if (SocketImcPresent == 0) {
            continue;
          }
          if(SADTablePtr[SadIndex].type == MEM_TYPE_1LM) {
            ChInterBitmap = SADTablePtr[SadIndex].channelInterBitmap[imc];
          } else {
            ChInterBitmap = SADTablePtr[SadIndex].FMchannelInterBitmap[imc];
          }
          switch (ChInterBitmap) {
            case BITMAP_CH0_CH1: // channel 1, channel 0; 2 chway
              if (CheckMirrorPopulation2(host, socket, imc, 0, 1) == RAS_IS_CAPABLE) {
                Capable = RAS_IS_CAPABLE;
              }
              break;

            case BITMAP_CH0_CH2: // channel 2, channel 0; 2 chway
              if (CheckMirrorPopulation2(host, socket, imc, 0, 2) == RAS_IS_CAPABLE) {
                Capable = RAS_IS_CAPABLE;
              }
              break;

            case BITMAP_CH1_CH2: // channel 2, channel 1; 2 chway
              if (CheckMirrorPopulation2(host, socket, imc, 1, 2) == RAS_IS_CAPABLE) {
                Capable = RAS_IS_CAPABLE;
              }
              break;

            default:
              Capable = RAS_NOT_CAPABLE;
              break;
          }
        } // imc loop
        break;
      case 0: // empty IMC
        Capable = RAS_SKIP_CHANNEL;
        break;
    }
    //Break out of SAD loop even if one SAD fails capability check
    if (Capable == RAS_NOT_CAPABLE)
      break;
  } //SadIndex

  return Capable;
}


/**

Displays the SADTable data structure in the host structure

@param host  - Pointer to sysHost

**/
static VOID DisplaySADTableTemp(PSYSHOST host) {
  UINT8  socketNum, sadIndex, TadIndex;
  UINT8 Imc, channelWays, imcWays, totalWays;
  struct memVar *mem;
  struct Socket *socket;
  UINT8   chInter = 0;
  struct SADTable *SADTablePtr;
  struct TADTable *TADTablePtr;
  struct IMC *imc;
  UINT32 chAddr;

  //Print SAD teble for each socket
  for (socketNum = 0; socketNum < MAX_SOCKET; socketNum++) {
    // skip empty SAD tables
    if (host->var.mem.socket[socketNum].SAD[0].Enable == 0) continue;


    mem = &host->var.mem;
    socket = &mem->socket[socketNum];
    SADTablePtr = host->var.mem.socket[socketNum].SAD;


    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n********SAD table for socket %d*******\n", socketNum));


    for(sadIndex=0; sadIndex<MAX_SAD_RULES; sadIndex++){
      if(host->var.mem.socket[socketNum].SAD[sadIndex].Enable == 0) continue;

      //Initialize local variables
      channelWays = 0;
      imcWays = socket->SAD[sadIndex].ways;
      // Determine which IMC channel bitmap to use
      Imc = GetMcIndexFromBitmap (SADTablePtr[sadIndex].imcInterBitmap);
      switch(socket->SAD[sadIndex].type) {
        case MEM_TYPE_1LM :
          chInter = socket->SAD[sadIndex].channelInterBitmap[Imc];
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_2LM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          chInter = socket->SAD[sadIndex].FMchannelInterBitmap[Imc] ;
          break;
      }

      //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
      channelWays = CalculateWaysfromBitmap (chInter);

      totalWays = imcWays * channelWays ;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"SAD ID\tLimit\tLocal\n"));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t\t%05d\t\t%d\n",sadIndex, (host->var.mem.socket[socketNum].SAD[sadIndex].Limit), host->var.mem.socket[socketNum].SAD[sadIndex].local));


      for(Imc = 0; Imc < host->var.mem.maxIMC; Imc++) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "IMC %d \n", Imc));
        imc = &host->var.mem.socket[socketNum].imc[Imc];
        TADTablePtr = host->var.mem.socket[socketNum].imc[Imc].TAD;
        if((SADTablePtr[sadIndex].imcInterBitmap) & (1<<Imc)) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"TAD ID\tLimit\tSadID\tTADChOffset\tCh0Address\n" ));
          for(TadIndex=0;TadIndex < TAD_RULES; TadIndex++) {
            if (!TADTablePtr[TadIndex].Enable)
              break;
            if(TADTablePtr[TadIndex].SADId == sadIndex) {
              chAddr = (imc->TAD[TadIndex].Limit - imc->TADOffset[TadIndex][0])/totalWays;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t\t%d\t\t%d\t\t%d\t\t\t%d\n",TadIndex ,(imc->TAD[TadIndex].Limit),imc->TAD[TadIndex].SADId,(imc->TADOffset[TadIndex][0]), (chAddr)));
            }
          }
        }
      }
    }
  }
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n</SADTable>\n"));
}

/**

Adjust memory address map on all the other sockets when mirroring is enabled

Routine figures out the adjustment required in SAD, TAD tables
when we enable mirroring this is required on all the sockets and
not only the socket on which the mirroring was enabled

@param host      - Pointer to sysHost
@param Socket    - Socket on which memory was adjusted for mirroring
@param Limit     - The Limit of the SAD that was adjusted this will be the maximum SAD limit in full mirroring
@param MirrorSize- the size of the mirror region

@retval SUCCESS

**/
static VOID
AdjustMemAddrMapRemote(
                        PSYSHOST host,
                        UINT8 socket,
                        UINT32 Limit,
                        UINT32 MirrorSize)
{
  UINT8 SocketIndex;
  UINT8 SadIndex;
  BOOLEAN PrevSadAdjusted = FALSE;

  struct SADTable *SADTablePtr;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "<AdjustMemAddrMapRemote>\n"));

  //
  //For all the sockets in the system
  //
  for(SocketIndex = 0;SocketIndex < MAX_SOCKET; SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0)
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;

    //
    //Skip the socket on which the mirroring was done, this is already taken care of
    //
    if (SocketIndex == socket)
        continue;

    for(SadIndex=0; SadIndex<MAX_SAD_RULES; SadIndex++) {
      if (!SADTablePtr[SadIndex].Enable) {
        break;
      }

      //Size of NXM SAD Cannot change it can only move.
      if(SADTablePtr[SadIndex].Attr == SAD_NXM_ATTR) {
        if (!PrevSadAdjusted) continue;
      }

      if(SADTablePtr[SadIndex].Limit >=Limit){
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\t Socket: %d , SadIndex; %d, Limit: %d, AdjustSize: %d \n", SocketIndex, SadIndex, SADTablePtr[SadIndex].Limit, MirrorSize));

        if (SADTablePtr[SadIndex].Limit == SAD_4GB) {
          PrevSadAdjusted = FALSE;
        } else {
          SADTablePtr[SadIndex].Limit -= MirrorSize;
          PrevSadAdjusted = TRUE;
        }

        if ((host->nvram.mem.RASmode & FULL_MIRROR_1LM) || (host->nvram.mem.RASmode & FULL_MIRROR_2LM))
          SADTablePtr[SadIndex].mirrored = 1;
      } else {
        continue;
      }
    }
  }
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "/<AdjustMemAddrMapRemote>\n"));
  return;
}


/**

  This routine inserts new Sad and corresponding TAD for partial mirroring.
  The insertion happens at location SadIndex+1

  @param host         - Pointer to sysHost
  @param SocketIndex  - Socket number
  @param SadIndex     - IMC number

**/
static UINT32
InsertUpdateSADTAD(
          PSYSHOST  host,
          UINT8     SocketIndex,
          UINT8     SadIndex,
          UINT32    PMirrorSize,
          UINT32    MemHoleSize
          )
{
  UINT8   sadcount;
  UINT8   package;
  struct  SADTable *SADTablePtr;
  UINT32  Limit;
  UINT32  Status = SUCCESS;

  SADTablePtr = host->var.mem.socket[SocketIndex].SAD;

  //Cannot insert if we have reached last DRAM rule
  if(SadIndex == (MAX_SAD_RULES-1))
    return ERROR_RESOURCE_CALCULATION_FAILURE;

  Limit = SADTablePtr[SadIndex].Limit;
  //
  //Locate last enabled SAD and move them by one position to insert a SAD
  //
  for (sadcount = (MAX_SAD_RULES-2); sadcount >= SadIndex; sadcount--) {
    if (SADTablePtr[sadcount].Enable == 0)
      continue;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nCopying SAD %d to SAD %d\n", sadcount, sadcount+1));
    SADTablePtr[sadcount+1] = SADTablePtr[sadcount];
    SADTablePtr[sadcount+1].Limit-=PMirrorSize;

    //
    // Adjust SAD Interleave list
    //
    for (package = 0; package < MC_MAX_NODE; package++) {
      host->var.mem.socket[SocketIndex].SADintList[sadcount+1][package] = host->var.mem.socket[SocketIndex].SADintList[sadcount][package];
    }
  }// Sadcount loop

  //
  //After inserting SAD in desired location, set the limit of SAD
  //
  SADTablePtr[SadIndex].Limit=(SADTablePtr[SadIndex - 1].Limit) + PMirrorSize;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nAdjustMemAddrMap for SAD %d Limit\n", SadIndex));
  AdjustMemAddrMapRemote(host,SocketIndex,Limit,PMirrorSize);

  return Status;
}

/**

  Adjust SAD limits for mirroring

  Routine figures out the adjustment required in SAD tables.

  @param host      - Pointer to sysHost
  @param SocketIndex      - Socket number
  @param SadIndex         -Sad number above which limits have to be adjusted
  @pmirrorsize            -Partial mirror size used to adjust the sad limits.s

  @retval SUCCESS

**/
static UINT32
UpdateSADTAD(
    PSYSHOST  host,
    UINT8     SocketIndex,
    UINT8     SadIndex,
    UINT32    MirrorSize
    ) {
  UINT8   sadcount;
  UINT32   Limit;
  UINT32  Status = SUCCESS;
  struct    SADTable *SADTablePtr;
  BOOLEAN PrevSadAdjusted=FALSE;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "<UpdateSADTAD>\n"));
  SADTablePtr = host->var.mem.socket[SocketIndex].SAD;

  Limit = SADTablePtr[SadIndex].Limit;
  for (sadcount = SadIndex ; sadcount < (MAX_SAD_RULES-1); sadcount++) {
    if(SADTablePtr[sadcount].Enable == 0)
      break;

    //Size of NXM SAD Cannot change it can only move.
    if(SADTablePtr[sadcount].Attr == SAD_NXM_ATTR) {
      if (!PrevSadAdjusted) continue;
    }

    if (SADTablePtr[sadcount].Limit == SAD_4GB) {
      PrevSadAdjusted = FALSE;
      continue;
    }

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "UpdateSADTAD: Socket: %x,Reducing sad %d Limit =%d by %d\n", SocketIndex, sadcount, SADTablePtr[sadcount].Limit, MirrorSize ));
    SADTablePtr[sadcount].Limit-= MirrorSize;
    PrevSadAdjusted = TRUE;

  } // Sad Index

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "AdjustMemAddrMap Remote for SAD %d Limit\n", SadIndex));
  AdjustMemAddrMapRemote(host,SocketIndex,Limit, MirrorSize);

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "/<UpdateSADTAD>\n"));
  return Status;
}

/**

Routine figures out if a SAD can be mirrored

  @param host      - Pointer to sysHost
  @param SAD       - Pointer to the Sad Table
  @param SadIndex  - Index into the Sad Table

  @retval 0 Sad not local or not enabled
  @retval 1 Sad can not be mirrored
  @retval 2 Sad can be mirrored
  **/
static UINT8
CheckSADForMirroring(
    PSYSHOST  host,
    struct   SADTable *SADTablePtr,
    UINT8     SadIndex
    )
{
  UINT8    mcIndex, channelWays;

  if (SADTablePtr[SadIndex].Enable == 0) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD Not Enabled: %d\n", SadIndex));
    return SAD_NOT_ENABLED; // end of SAD table
  }

  if (SADTablePtr[SadIndex].local != 1) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD Not Local: %d\n", SadIndex));
    return SAD_NOT_ELIGIBLE_MIRROR; // Use only local SAD for creating partial mirror regions.
  }

  if (SADTablePtr[SadIndex].Attr == SAD_NXM_ATTR) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Cannot mirror Non Existant Mem %d\n", SadIndex));
    return SAD_NOT_ELIGIBLE_MIRROR;
  }

  //Find If channel interleave bitmap is power of 2 -- No interleaving and single channel way
  mcIndex = GetMcIndexFromBitmap (SADTablePtr[SadIndex].imcInterBitmap);
  channelWays = CalculateWaysfromBitmap (SADTablePtr[SadIndex].channelInterBitmap[mcIndex]);
  if(channelWays == ONE_WAY) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD %d is single chway\n", SadIndex));
    return SAD_NOT_ELIGIBLE_MIRROR;
  } //Cannot mirror 1 chway in SKX

  if (SADTablePtr[SadIndex].type != MEM_TYPE_1LM && SADTablePtr[SadIndex].type != MEM_TYPE_2LM) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD %d is not 1LM or 2LM\n", SadIndex));
    return SAD_NOT_ELIGIBLE_MIRROR;
  } //skip SAD if, PMEM/PMEM$

  if (SADTablePtr[SadIndex].mirrored == 1) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD: %d is already mirrored\n", SadIndex));
    return SAD_NOT_ELIGIBLE_MIRROR; // Skip SAD if its already mirrored
  }
  return SAD_ELIGIBLE_MIRROR;
}

static UINT32
GetMemBlocksBelow4GB(
  PSYSHOST  host
)
{

  UINT8   TadIndex;
  UINT8   Imc;
  BOOLEAN TadFound = FALSE;
  UINT32  TOLM;
  UINT8     SocketIndex;
  UINT8     SadIndex=SAD0INDEX;
  struct SADTable *SADTablePtr;
  struct TADTable *TADTablePtr;

  TOLM = (UINT16)(FOUR_GB_MEM - host->setup.common.lowGap);

  for(SocketIndex=0;SocketIndex<MAX_SOCKET;SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0)
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[SadIndex].Limit == SAD_4GB) { //Find the 4GB sad and its corresponding TAD to get correct value of TOLM

      for(Imc = 0; Imc < host->var.mem.maxIMC; Imc++){
        TADTablePtr = host->var.mem.socket[SocketIndex].imc[Imc].TAD;

        // If this Imc is part of the interleave
        if((SADTablePtr[SadIndex].imcInterBitmap) & (1 << Imc)) {

          for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
            if(TADTablePtr[TadIndex].SADId == SadIndex && TADTablePtr[TadIndex].Enable == 1) {
              TadFound = TRUE;
              break;
            }
          } // for TadIndex
          if(TadFound) {
            return TADTablePtr[TadIndex].Limit;
          }
        }
      } // for (Imc)
    } // if SAD_4GB
  }
  //if Tad Not Found return TOLM
  return TOLM;
}

/*Calculate if enough memory is available to create the mirror region completely*/
BOOLEAN CalculateMirrorMem(
    PSYSHOST  host,
    UINT32    MirrorSize
    )
{
  UINT8     SocketIndex;
  UINT8     SadIndex;
  UINT32    MemoryAvailable;
  BOOLEAN   MEM_AVAILABLE = FALSE;
  struct    SADTable *SADTablePtr;

  MemoryAvailable = 0;
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n MirrorSize :%d\n", MirrorSize));
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0)
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;

    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
      if (SADTablePtr[SadIndex].Enable == 0) continue;
      if (SADTablePtr[SadIndex].Limit == SAD_4GB) continue; //4gb SAD is mirrored separately

      if(CheckSADForMirroring(host, SADTablePtr, SadIndex) == SAD_ELIGIBLE_MIRROR){
        if(SadIndex == 0) {
          MemoryAvailable += (SADTablePtr[SadIndex].Limit);
        } else{
          MemoryAvailable += (SADTablePtr[SadIndex].Limit - SADTablePtr[SadIndex-1].Limit);
        }
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Socket:%d Sad :%d Mem Avl so far:%d\n", SocketIndex, SadIndex, MemoryAvailable));
      if(MemoryAvailable >= (MirrorSize *2)){
        MEM_AVAILABLE = TRUE;
        break;
      }
    }
    if(MEM_AVAILABLE) break;
  }
  if(MEM_AVAILABLE) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n Enough Memory available for Mirroring \n"));
    return TRUE;
  }
  else {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n Not Enough memory for Mirroring \n"));
    return FALSE;
  }
}

/**

Adjust memory address map for mirroring SAD0

Routine figures out the adjustment required in SAD, SAD2TAD, TAD tables
when we enable mirroring the 4GB SAD0. This is different on account of
having the memory hole in SAD0

@param host      - Pointer to sysHost
@param TOLM      - Top of low memory

@retval SUCCESS

**/

static VOID
AdjustMemAddrMapForSad0Mirror (
    PSYSHOST host,
    UINT16 TOLM)
{
  UINT8      SocketIndex;
  UINT32     PMirrorSize;
  UINT8      Status;
  struct     SADTable *SADTablePtr;


  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[0].Enable == 0) continue; // skip empty sockets

    Status = CheckSADForMirroring(host,SADTablePtr,SAD0INDEX);
    if(Status == SAD_NOT_ELIGIBLE_MIRROR)
      continue;

    PMirrorSize = GetMemBlocksBelow4GB(host);
    SADTablePtr[SAD0INDEX].mirrored = 1;
    UpdateSADTAD(host,SocketIndex,SAD0INDEX,PMirrorSize);

  } // Socket Loop
  return;
}

static VOID
NewTADInterleave(PSYSHOST host,
                 UINT16   TOLM)
{

  UINT8     SocketIndex;
  UINT8     SadIndex, TadIndex;
  UINT8     Imc;
  UINT8     ch;
  UINT16    memSize;

  struct    SADTable *SADTablePtr;
  struct    TADTable *TADTablePtr;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n <NewTADInterleave> \n"));

  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0) continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[0].Enable == 0) continue; // skip empty sockets

    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
      if (SADTablePtr[SadIndex].Enable == 0) break; // End of SAD tables
      for(Imc = 0; Imc < host->var.mem.maxIMC; Imc++){
        // SKip if IMC not enabled
        if (host->nvram.mem.socket[SocketIndex].imc[Imc].enabled == 0) continue;

        // If this mc is part of the interleave
        if((SADTablePtr[SadIndex].imcInterBitmap) & (1 << Imc)) {
          //clear TAD tables The TAD tables are recreated in TADInterleave function call.
          for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
            TADTablePtr = &host->var.mem.socket[SocketIndex].imc[Imc].TAD[TadIndex];
            if(TADTablePtr->Enable == 1) {
              TADTablePtr->Enable = 0;
            } else {
              break; //reached end of TAD tables
            }
          } // for (TadIndex)

          //This is to ensure all the channel and imc Rem Size
          //Is Initialized to the correct value before recreating TAD tables

          if(SADTablePtr[SadIndex].type == MEM_TYPE_1LM) {
            for(ch=0; ch< MAX_MC_CH; ch++) {
              memSize = host->var.mem.socket[SocketIndex].channelList[NODECH_TO_SKTCH(Imc, ch)].memSize;
              host->var.mem.socket[SocketIndex].channelList[NODECH_TO_SKTCH(Imc, ch)].ddr4RemSize = memSize;
            }
          }
          else if(SADTablePtr[SadIndex].type == MEM_TYPE_2LM){
            for(ch=0; ch< MAX_MC_CH; ch++) {
              memSize = host->var.mem.socket[SocketIndex].channelList[NODECH_TO_SKTCH(Imc, ch)].volSize;
              host->var.mem.socket[SocketIndex].channelList[NODECH_TO_SKTCH(Imc, ch)].volRemSize = memSize;
            }
          }
        }

      } // for (Imc)
    } // for (SadIndex)
  } // for (SocketIndex)
  //clear TAD tables and call tad interleave to create it again.

  TADInterleave(host,TOLM);
  return;
}

UINT32
AdjustMemAddrMapUEFI(
    PSYSHOST  host,
    UINT16    TOLM
    )
{
  UINT8     SocketIndex;
  UINT8     SadIndex;
  UINT32    MemHoleSize;
  UINT32    PMirrorSize;
  UINT32    CurrentSadSize;
  UINT32    TotalMemoryAvailable;
  UINT32    BlocksBelow4G = 0;
  UINT8     SADCheckReturnStatus;
  struct    SADTable *SADTablePtr;
  UINT32    Status = SUCCESS;
  BOOLEAN   PMIRROR_CREATED =FALSE;
  UINT8     ch;
  UINT8     chInter = 0;
  UINT8     channelWays, imcWays, totalSadWays, imcIndex;

  MemHoleSize = (FOUR_GB_MEM - TOLM);

  //The percentage value was entered from OS instead of setup option
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "UEFI Variable for address range mirroring found \n"));

  if(host->setup.mem.partialmirrorpercent > 5000) {
    return MIRROR_STATUS_INVALID_REQUEST; //invalid request
  }

  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    TotalMemoryAvailable = 0;
    //several nvram structures that have info on dimms in each channel DDR4 or NGN
    // use structure like struct ddrChannel to calculate total memory memhost.h  also make sure to use the volatile size and not persistent size

    if(host->nvram.mem.socket[SocketIndex].enabled) {
      for(ch=0;ch<MAX_CH;ch++) {
        if(host->nvram.mem.socket[SocketIndex].channelList[ch].enabled) {
          TotalMemoryAvailable += host->var.mem.socket[SocketIndex].channelList[ch].memSize + host->var.mem.socket[SocketIndex].channelList[ch].volSize;
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Socket 0x%x ch:0x%x memSize:0x%x volSize: 0x%x\n",
              SocketIndex, ch, host->var.mem.socket[SocketIndex].channelList[ch].memSize,
              host->var.mem.socket[SocketIndex].channelList[ch].volSize));
        }
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Socket %x - TotalMemoryAvailable : 0x%x\n", SocketIndex, TotalMemoryAvailable));

      SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
      if(SADTablePtr[0].Limit == SAD_4GB && SADTablePtr[0].Enable == 1 && SADTablePtr[0].local == 1) {
        BlocksBelow4G = GetMemBlocksBelow4GB(host);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Socket %x -BlocksBelow4G : 0x%x\n", SocketIndex, BlocksBelow4G));
        TotalMemoryAvailable -= BlocksBelow4G;//do not count the amount of memory below 4GB
        if(host->setup.mem.partialmirrorsad0) { //take twice the size of TOLM because it need that amount to mirror.
          TotalMemoryAvailable -= BlocksBelow4G;
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Socket %x - TotalMemoryAvailable after subtraction of TOLM : 0x%x\n", SocketIndex, TotalMemoryAvailable));
      }
      //Find the partial mirror size from percentage given

      PMirrorSize  = (TotalMemoryAvailable * (UINT32)host->setup.mem.partialmirrorpercent)/10000;
      PMirrorSize  +=  (((TotalMemoryAvailable * (UINT32)host->setup.mem.partialmirrorpercent) % 10000) != 0); //Ceil to next 64MB
      if(PMirrorSize > (TotalMemoryAvailable / 2)) { //make sure it doesn't exceed 50% due to the ceiling.
        PMirrorSize = (TotalMemoryAvailable / 2);
      }

      SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
      for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {

        //4GB SAD has memory hole and is mirrored separately
        if (SADTablePtr[SadIndex].Limit == SAD_4GB)
          continue;
        else {
          if (SadIndex == 0)
            CurrentSadSize = SADTablePtr[SadIndex].Limit;
          else
            CurrentSadSize = SADTablePtr[SadIndex].Limit - SADTablePtr[SadIndex-1].Limit;

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "SAD %d\n", SadIndex));
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Requested partial mirror size 0x%x \n", PMirrorSize));
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Current SAD size 0x%x\n", CurrentSadSize));

          SADCheckReturnStatus = CheckSADForMirroring(host, SADTablePtr, SadIndex);
          if(SADCheckReturnStatus == SAD_NOT_ENABLED){
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD_NOT_ENABLED\n"));
           break;
          } else if (SADCheckReturnStatus == SAD_NOT_ELIGIBLE_MIRROR){
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD_NOT_ELIGIBLE_MIRROR\n"));
            continue;
          }

          //Following calculation of TotalSadWays is required to round off the partial mirror size
          //to a value that is exactly divisible by number of channels interleaved in that SAD
          channelWays = 0;
          imcWays = SADTablePtr[SadIndex].ways;

          // Determine which IMC channel bitmap to use
          imcIndex = GetMcIndexFromBitmap (SADTablePtr[SadIndex].imcInterBitmap);

          switch(SADTablePtr[SadIndex].type) {
            case MEM_TYPE_1LM :
              chInter = SADTablePtr[SadIndex].channelInterBitmap[imcIndex];
              break;
            case MEM_TYPE_PMEM :
            case MEM_TYPE_2LM :
            case MEM_TYPE_PMEM_CACHE :
            case MEM_TYPE_BLK_WINDOW :
            case MEM_TYPE_CTRL :
              chInter = SADTablePtr[SadIndex].FMchannelInterBitmap[imcIndex] ;
              break;
          }

          //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
          channelWays = CalculateWaysfromBitmap (chInter);
          totalSadWays = imcWays*channelWays;

          if (CurrentSadSize == (PMirrorSize*2)) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD %d Size == (PMirrorSize*2)\n", SadIndex));
            SADTablePtr[SadIndex].mirrored = 1;
            Status = UpdateSADTAD(host,SocketIndex,SadIndex,PMirrorSize);
            PMirrorSize = 0;
            //break;
          }
          else if (CurrentSadSize > (PMirrorSize*2)) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD %d Size > (PMirrorSize*2) \n", SadIndex));
           if(SadIndex < (MAX_SAD_RULES-1)) {

             //make size of Partial mirror a multiple of the interleave ways to divide memory equally between the channels
             if(totalSadWays > 1) {
               //Limit of SAD should be GIGA BIT Aligned for partial mirorring
               MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "GB Alignment required!\n"));
               PMirrorSize = PMirrorSize & GB_BOUNDARY_ALIGN;
               if (!(totalSadWays%3)) {
                 //The Size of the SAD should always be a multiple of number of channels interleaved.
                 while (PMirrorSize%totalSadWays) {
                   PMirrorSize = PMirrorSize - 0x10;
                   MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                  "PMirrorSize:%d \n", PMirrorSize));

                   if( PMirrorSize <0x10) { // Cannot create a mirror region that is GB aligned
                     MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     "No Mirror!! Cannot create mirror region that is GB Aligned.\n"));
                     PMirrorSize = 0;
                     break;
                   }
                 }
               }
             }

             Status = InsertUpdateSADTAD(host,SocketIndex,SadIndex,PMirrorSize,MemHoleSize);
             SADTablePtr[SadIndex].mirrored = 1;
             PMirrorSize = 0;
           }
           //break;
          }
          else if(CurrentSadSize < (PMirrorSize*2)){
            if(SadIndex < (MAX_SAD_RULES-1)) {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "SAD %d Size < (PMirrorSize*2) \n", SadIndex));
              PMirrorSize -= (CurrentSadSize/2); //only part of requested pmirror is done
              SADTablePtr[SadIndex].mirrored = 1;
              Status = UpdateSADTAD(host,SocketIndex,SadIndex,(CurrentSadSize/2));
            }
          }
        }

        if (PMirrorSize == 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Mirror done on socket %d\n", SocketIndex));
          PMIRROR_CREATED = TRUE;
          break;
        }
      } // SAD loop
      if (PMIRROR_CREATED){
        //Requested PMirror was created successfully
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Calling newtadinterleave\n"));
        NewTADInterleave(host,TOLM);
      }
    }
  } //Socket loop

  return Status;
}


/**

Adjust memory address map for partial mirroring

Routine figures out the adjustment required in SAD, SAD2TAD, TAD tables for various partial mirror regions and sizes.

@param host      - Pointer to sysHost
@param TOLM      - Top of low memory

@retval SUCCESS

**/
static UINT32
AdjustMemAddrMapForPMirror (
          PSYSHOST  host,
          UINT16    TOLM
          )
{
  UINT8     SocketIndex;
  UINT8     SadIndex;
  UINT32    count;
  UINT32    MemHoleSize;
  UINT32    PMirrorCount;
  UINT32    PMirrorSize;
  UINT32    CurrentSadSize;
  UINT8     SADCheckReturnStatus;
  struct    SADTable *SADTablePtr;
  UINT32    Status = SUCCESS;
  UINT8     chInter = 0;
  UINT8     channelWays, imcWays, totalSadWays, imcIndex;
  BOOLEAN   PMIRROR_CREATED =FALSE;
  //
  // Calculate the size of PCIe MMIO hole just below 4GB
  //
  MemHoleSize = (FOUR_GB_MEM - TOLM);

  // Cannot create mirror regions if rank sparing is enabled
  if (host->nvram.mem.RASmode & RK_SPARE) {
    Status = MIRROR_STATUS_UNSUPPORTED_CONFIG; //Unsopported_mirror_config
    return Status;
  }
  //
  //Partial mirror SAD0 is enabled - mirror the region below 4gb
  //
  if(host->setup.mem.partialmirrorsad0) {
    AdjustMemAddrMapForSad0Mirror(host,TOLM);
    NewTADInterleave(host,TOLM);
  }

  //The percentage value was entered from OS instead of setup option
  if(host->setup.mem.partialMirrorUEFI) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "UEFI Variable for address range mirroring found \n"));

    Status = AdjustMemAddrMapUEFI(host, TOLM);
    return Status;

  } //If UEFI enabled address range mirror

  //if address range mirror is enabled in setup
  else{
    PMirrorCount = MAX_PARTIAL_MIRROR;
    //Loop for number of regions that can be created
    for(count=0;count<PMirrorCount;count++){
      if(host->setup.mem.partialmirrorsize[count] == 0) break;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Partial mirror size : %d\n", host->setup.mem.partialmirrorsize[count]));
      PMirrorSize = 0;
      for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
        SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
        if (SADTablePtr[0].Enable == 0) break; // skip empty sockets

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\nAt Socket %d\n", SocketIndex));

        for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "At Sad Index : %d\n", SadIndex));

          //Non Existant Memory cannot be considered for Mirroring related adjustments
          if (SADTablePtr[SadIndex].Attr == SAD_NXM_ATTR)
             continue;

          //Following calculation of TotalSadWays is required to round off the partial mirror size
          //to a value that is exactly divisible by number of channels interleaved in that SAD
          channelWays = 0;
          imcWays = SADTablePtr[SadIndex].ways;

          // Determine which IMC channel bitmap to use
          imcIndex = GetMcIndexFromBitmap (SADTablePtr[SadIndex].imcInterBitmap);

          switch(SADTablePtr[SadIndex].type) {
            case MEM_TYPE_1LM :
              chInter = SADTablePtr[SadIndex].channelInterBitmap[imcIndex];
              break;
            case MEM_TYPE_PMEM :
            case MEM_TYPE_2LM :
            case MEM_TYPE_PMEM_CACHE :
            case MEM_TYPE_BLK_WINDOW :
            case MEM_TYPE_CTRL :
              chInter = SADTablePtr[SadIndex].FMchannelInterBitmap[imcIndex] ;
              break;
          }

          //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
          channelWays = CalculateWaysfromBitmap (chInter);
          totalSadWays = imcWays*channelWays;

          //Initialize PMirrorSize
          if (PMirrorSize == 0) {
            PMirrorSize = host->setup.mem.partialmirrorsize[count];
          }

          //
          // skip adjustment for SAD with 2G-4G memory hole.
          //4 << 4 is actually (4 << 30) >> 26. i.e., 4GB >> 26.
          //
          if (SADTablePtr[SadIndex].Limit == SAD_4GB) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "AdjustMemAddrMapForPMirror: Ignore 4GB Sad %d\n", SadIndex));
            continue;
          }

          SADCheckReturnStatus = CheckSADForMirroring(host, SADTablePtr, SadIndex);
          if(SADCheckReturnStatus == SAD_NOT_ENABLED){
            break;
          }else if (SADCheckReturnStatus == SAD_NOT_ELIGIBLE_MIRROR){
            continue;
          }

          if (SadIndex == 0)
            CurrentSadSize = SADTablePtr[SadIndex].Limit;
          else
            CurrentSadSize = SADTablePtr[SadIndex].Limit - SADTablePtr[SadIndex-1].Limit;

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Current SAD size %d\n", CurrentSadSize));

          if (CurrentSadSize == (PMirrorSize*2)) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD %d Size == (PMirrorSize*2)\n", SadIndex));
            SADTablePtr[SadIndex].mirrored = 1;
            Status = UpdateSADTAD(host,SocketIndex,SadIndex,PMirrorSize); //This updates SAD ONLY

            PMirrorSize = 0;
            break;
          }
          else if (CurrentSadSize > (PMirrorSize*2)) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "SAD %d Size > (PMirrorSize*2) \n", SadIndex));
           if(SadIndex < (MAX_SAD_RULES-1)) {

             //make size of Partial mirror a multiple of the interleave ways to divide memory equally between the channels
             if(totalSadWays > 1) {
               PMirrorSize = PMirrorSize & GB_BOUNDARY_ALIGN;
               if(!(totalSadWays%3)){ // SAD is 3way or 6way then also ensure the pmirror size is muliple of chways
                 //The Size of the SAD should always be a multiple of number of channels interleaved.
                 while (PMirrorSize%totalSadWays) {
                   MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                  "PMirrorSize:%d, totalsadways:%d\n", PMirrorSize, totalSadWays));
                   PMirrorSize = PMirrorSize - 0x10;
                   MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                  "PMirrorSize:%d \n", PMirrorSize));

                   if( PMirrorSize <0x10) { // Cannot create a mirror region that is GB aligned
                     PMirrorSize = 0;
                     break;
                   }
                 }
               }
             }

             Status = InsertUpdateSADTAD(host,SocketIndex,SadIndex,PMirrorSize,MemHoleSize);
             SADTablePtr[SadIndex].mirrored = 1;

             PMirrorSize = 0;
           }
           break;
          }
          else if(CurrentSadSize < (PMirrorSize*2)){
            if(SadIndex < (MAX_SAD_RULES-1)) {
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "SAD %d Size < (PMirrorSize*2) \n", SadIndex));

              if(CalculateMirrorMem(host, PMirrorSize)){ //Check if enough memory is available to create mirror
                MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "Enough memory for mirror\n"));
                PMirrorSize -= (CurrentSadSize/2); //only part of requested pmirror is done
                SADTablePtr[SadIndex].mirrored = 1;
                if (CurrentSadSize % 2) {
                  Status = UpdateSADTAD(host,SocketIndex,SadIndex,((CurrentSadSize/2)+1));
                } else {
                  Status = UpdateSADTAD(host,SocketIndex,SadIndex,(CurrentSadSize/2));
                }

                continue;
              }
              else {
                Status = SUCCESS; //Identify the right way to handle this in memory mapping code
              }
            }
            break;
          }
        } // for (SadIndex)
        //
        /* Break from the socket loop if requested partial mirror
        was created in current socket. If not continue to next socket*/
        //
        if (PMirrorSize == 0) {
          PMIRROR_CREATED = TRUE;
          Status = SUCCESS;
          break;
        }
      } // for (SocketIndex)
      if (PMIRROR_CREATED){
        //Requested PMirror was created successfully
        NewTADInterleave(host,TOLM);
      }
    } // Number of partial mirror to be created
  }//Setup enabled address range mirror
  if(Status != EFI_SUCCESS) {
    Status = MIRROR_STATUS_UNSUPPORTED_CONFIG; //UNSUPPORTED_CONFIG
  }
  return Status;
}

VOID
DoSncGBAlignmentForMirror(
    PSYSHOST  host
    )
{
  UINT8   SocketIndex;
  UINT8   SadIndex;
  struct  SADTable *SADTablePtr;
  UINT32  GBAlignSize, NewSadLimit;
  UINT64  PrevHigherSadLimit = 0;
  UINT8   chInter = 0;
  UINT8   channelWays, imcWays, totalSadWays, imcIndex;

  if (host->var.kti.OutSncGbAlignRequired != 1) { //determined that the SAD limites have to be GB aligned
    return;
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "<DoSncGBAlignmentForMirror>\n"));
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0)
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[0].Enable == 0) continue;

    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {

      //Break if Sad is not enabled
      if(SADTablePtr[SadIndex].Enable == 0) break;
      //Skip this SAD if the SAD is not local
      if(SADTablePtr[SadIndex].local == 0) continue;
      //Cannot Mirror Non Existant Memory
      if(SADTablePtr[SadIndex].Attr == SAD_NXM_ATTR) continue;
      // might be needed incase if mirroring is supported with diff memory socket sizes.
      if(((host->setup.mem.options & NUMA_AWARE) == 00) && (SADTablePtr[SadIndex].Limit <= PrevHigherSadLimit))
        continue;

      //Following calculation of TotalSadWays is required to round off the SAD size
      //to a value that is exactly divisible by number of channels interleaved in that SAD
      channelWays = 0;
      imcWays = SADTablePtr[SadIndex].ways;

      // Determine which IMC channel bitmap to use
      imcIndex = GetMcIndexFromBitmap (SADTablePtr[SadIndex].imcInterBitmap);

      switch(SADTablePtr[SadIndex].type) {
        case MEM_TYPE_1LM :
          chInter = SADTablePtr[SadIndex].channelInterBitmap[imcIndex];
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_2LM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          chInter = SADTablePtr[SadIndex].FMchannelInterBitmap[imcIndex] ;
          break;
      }

      //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
      channelWays = CalculateWaysfromBitmap (chInter);
      totalSadWays = imcWays*channelWays;

      switch(SADTablePtr[SadIndex].type) {
        case MEM_TYPE_1LM :
        case MEM_TYPE_2LM :

          if (SADTablePtr[SadIndex].Limit == SAD_4GB) {
            continue;
          } // SAD_4GB
          NewSadLimit = SADTablePtr[SadIndex].Limit & GB_BOUNDARY_ALIGN;
          if (!(totalSadWays%3)) { //3chway or 6chway interleave

            if (SadIndex > 0) {
              while ((NewSadLimit - SADTablePtr[SadIndex-1].Limit) % totalSadWays) {
                // Each channel did not have an equal contribution, so try next GB boundary
                NewSadLimit = NewSadLimit - 0x10;
              }
            }
          }
          GBAlignSize = SADTablePtr[SadIndex].Limit - NewSadLimit;
          UpdateSADTAD(host,SocketIndex,SadIndex,GBAlignSize);
          PrevHigherSadLimit = SADTablePtr[SadIndex].Limit;
          break;

        case MEM_TYPE_PMEM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
        default:
          /*Mirroring from Setup can be requested only for 1LM/2LM memory
          PMEM/PMEM$ mirroring is dictated by platform config data in NGN DIMMs*/
          break;
      } // switch
    } // for (SadIndex)
  } // for (SocketIndex)
}

/**

  Adjust memory address map for mirroring. Routine figures out the adjustment
  required in SAD, SAD2TAD, TAD tables when mirroring or ARM is enabled

  @param host      - Pointer to sysHost
  @param TOLM      - Top of low memory

  @retval SUCCESS

**/
UINT32
AdjustMemAddrMapForFullMirror (
    PSYSHOST  host,
    UINT32  MemHoleSize,
    UINT16  TOLM
    )
{

  UINT8   SocketIndex;
  UINT8   SadIndex;
  UINT32  Status = SUCCESS;
  struct  SADTable *SADTablePtr;
  UINT32  MirrorSize;
  UINT64  PrevHigherSadLimit = 0;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "<AdjustMemAddrMapForFullMirror>\n"));
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    if (host->nvram.mem.socket[SocketIndex].enabled == 0)
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[0].Enable == 0) continue;

    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {

      //Break if Sad is not enabled
      if(SADTablePtr[SadIndex].Enable == 0) break;
      //Skip this SAD if the SAD is not local
      if(SADTablePtr[SadIndex].local == 0) continue;
      //Cannot Mirror Non Existant Memory
      if(SADTablePtr[SadIndex].Attr == SAD_NXM_ATTR) continue;
      // might be needed incase if mirroring is supported with diff memory socket sizes.
      if(((host->setup.mem.options & NUMA_AWARE) == 00) && (SADTablePtr[SadIndex].Limit <= PrevHigherSadLimit))
        continue;

      switch(SADTablePtr[SadIndex].type) {
        case MEM_TYPE_1LM :
        case MEM_TYPE_2LM :

          //Special case when the 4GB SAD is not halved but we borrow memory from SAD above
          if (SADTablePtr[SadIndex].Limit == SAD_4GB) {
            SADTablePtr[SadIndex].mirrored=1;
            MirrorSize = SAD_4GB - MemHoleSize;
            UpdateSADTAD(host,SocketIndex,SadIndex,MirrorSize);
            continue;
          } // SAD_4GB

          SADTablePtr[SadIndex].mirrored=1;
          if(SadIndex == 0) {
            MirrorSize = (SADTablePtr[SadIndex].Limit)/2;
          } else {
            MirrorSize = ((SADTablePtr[SadIndex].Limit - SADTablePtr[SadIndex-1].Limit) /2);
          }
          UpdateSADTAD(host,SocketIndex,SadIndex,MirrorSize);
          PrevHigherSadLimit = SADTablePtr[SadIndex].Limit;
          break;

        case MEM_TYPE_PMEM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          /*Mirroring from Setup can be requested only for 1LM/2LM memory
          PMEM/PMEM$ mirroring is dictated by platform config data in NGN DIMMs*/
          break;
      }

    } // for (SadIndex)
  } // for (SocketIndex)
  DoSncGBAlignmentForMirror(host);
  NewTADInterleave(host,TOLM);
  return Status;
  return Status;
}

/**

  After mirroring, this call will adjust the memory size fields to accurately reflect the
  value of the reduced memory in system

  @param host      - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
AdjustMemorySizeFieldsforMirror(
    PSYSHOST  host
    ) {

  UINT8     imc;
  UINT8     SocketIndex;
  UINT8     SadIndex;
  UINT32    Status;
  UINT16    CurrentSadSize;
  UINT16    CurrentSadInterleaves;
  UINT16    sadInterleaveSizePerMc;

  struct    memVar *mem;
  struct    SADTable *SADTablePtr;

  Status = SUCCESS;
  mem = &host->var.mem;

  for(SocketIndex=0;SocketIndex<MAX_SOCKET;SocketIndex++) {
    MemDebugPrint((host, SDBG_MAX, SocketIndex, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n<AdjustMemorySizeFieldsforMirror> \n"));
    /*Verify the socket has memory populated and enabled*/
    if ((host->nvram.mem.socket[SocketIndex].enabled == 0) || (host->nvram.mem.socket[SocketIndex].maxDimmPop == 0))
      continue;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
      if(SADTablePtr[SadIndex].Enable == 0)
        break;

      if(! SADTablePtr[SadIndex].local)
        continue;

      if( (SADTablePtr[SadIndex].type != MEM_TYPE_1LM)&&(SADTablePtr[SadIndex].type != MEM_TYPE_2LM)  )
        continue;

      if(! SADTablePtr[SadIndex].mirrored)
        continue;

      CurrentSadSize= 0;
      /*Find the size of current sad*/
      if (SadIndex == 0) {
        if (SADTablePtr[SadIndex].Limit == SAD_4GB)
          CurrentSadSize = (UINT16)GetMemBlocksBelow4GB(host);
        else
          CurrentSadSize = (UINT16)SADTablePtr[SadIndex].Limit;
      }
      else
        CurrentSadSize = (UINT16)SADTablePtr[SadIndex].Limit - (UINT16)SADTablePtr[SadIndex-1].Limit;

      /*Initialize number of imcs interleaved in the SAD*/
      CurrentSadInterleaves = (UINT16)SADTablePtr[SadIndex].ways;
      sadInterleaveSizePerMc = CurrentSadSize /CurrentSadInterleaves;
      for (imc=0; imc< host->var.mem.maxIMC; imc++) {
        if((SADTablePtr[SadIndex].imcInterBitmap) & (1 << imc)) { //is this imc part of the interleave for this SAD?
          /*for each mirrored SAD, reduce the memory contributed by each imc to that SAD from memSize structure of the imc*/

          if(SADTablePtr[SadIndex].type == MEM_TYPE_1LM) {
            mem->socket[SocketIndex].imc[imc].memSize -= sadInterleaveSizePerMc;
            mem->memSize -= sadInterleaveSizePerMc; // Also reduce total physical memory size
          }
          else if (SADTablePtr[SadIndex].type == MEM_TYPE_2LM) {
            mem->socket[SocketIndex].imc[imc].volSize -= sadInterleaveSizePerMc;
            mem->memSize -= sadInterleaveSizePerMc; // Also reduce total physical memory size
          }
        }
      }

    }

  } // For Socket Index
  return Status;
}

/**

  Adjust memory address map for mirroring. Routine figures out the adjustment
  required in SAD, SAD2TAD, TAD tables when mirroring or ARM is enabled

  @param host      - Pointer to sysHost
  @param TOLM      - Top of low memory

  @retval SUCCESS

**/
UINT32
AdjustMemAddrMapForMirror (
    PSYSHOST  host
    ) {
  UINT8   SocketIndex;
  UINT8   SadIndex;
  UINT16  TOLM;
  UINT32  MemHoleSize;
  UINT32  Status = SUCCESS;
  BOOLEAN Type2LMFlag;

  struct SADTable *SADTablePtr;

  // Top of Low Memory equals 4GB - size of the low Gap
  // TOLM in 64MB units
  //
  TOLM = (UINT16)(FOUR_GB_MEM - host->setup.common.lowGap);
  // Has to be on a 256MB boundary
  TOLM = TOLM & ~0x3;

  // Calculate the size of PCIe MMIO hole just below 4GB
  MemHoleSize = (FOUR_GB_MEM - (UINT16)GetMemBlocksBelow4GB(host));
  //DisplaySADTableTemp(host);
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nAdjustMemAddrMapForMirror: In the function\n"));

  //Verify the socket populated has memory population for mirroring. The memory population
  //requirements are the same for both full and address based mirror

  for(SocketIndex=0;SocketIndex<MAX_SOCKET;SocketIndex++) {

    if ((host->nvram.mem.socket[SocketIndex].enabled == 0) || (host->nvram.mem.socket[SocketIndex].maxDimmPop == 0))
      continue;
    if (CheckMirrorPopulation (host, SocketIndex) == RAS_NOT_CAPABLE) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\nMirroring population not met\n"));
      host->nvram.mem.RASmode &= ~(CH_ALL_MIRROR);
      host->var.mem.RASModes &= ~(CH_ALL_MIRROR);
      host->setup.mem.partialmirrorsts = MIRROR_STATUS_MIRROR_INCAPABLE;
      return Status;
    }

    Type2LMFlag = FALSE;

    SADTablePtr = host->var.mem.socket[SocketIndex].SAD;
    if (SADTablePtr[0].Enable == 0) continue; // skip empty sockets
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nAdjustMemAddrMapForMirror - Socket: %d \n", SocketIndex));

    //The following two for loops check for 2LM/PMEM$. In that case, mirroring cannot exist
    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
      if(SADTablePtr[SadIndex].Enable == 0)
        break;
      if(SADTablePtr[SadIndex].type == MEM_TYPE_2LM) {
        Type2LMFlag = TRUE;
        break;
      }
    }

    for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
      if(SADTablePtr[SadIndex].Enable == 0)
        break;
      if((SADTablePtr[SadIndex].type == MEM_TYPE_PMEM_CACHE) && (Type2LMFlag)){
        host->nvram.mem.RASmode &= ~FULL_MIRROR_2LM;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\t AdjustMemAddrMapForMirror: Memory mirror not possible in 2LM mode with PMEM$\n"));
        return Status;
      }
    }
  } // For Socket Index

  // Perform Address Range Mirroring and exit if enabled
  if ((host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)||(host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM)) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "AdjustMemAddrMapForMirror: Address Range Mirror enabled\n"));
    Status = AdjustMemAddrMapForPMirror(host,TOLM);
    if(host->setup.mem.partialMirrorUEFI) {
      host->setup.mem.partialmirrorsts = (UINT8)Status;
    }
  }

  //Else, Perform Full Mirroring if enabled
  else if ((host->nvram.mem.RASmode & FULL_MIRROR_1LM) || (host->nvram.mem.RASmode & FULL_MIRROR_2LM)) {

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\tAdjustMemAddrMapForMirror: Full Mirror enabled\n"));
    AdjustMemAddrMapForFullMirror (host, MemHoleSize, TOLM);
  }

  return Status;
}


/**

  Description: Fill in SAD2TAD bits for the given SAD based on memory mode and mirroring setup policy,
  capability of platform and current memory population etc

  @param host  - Pointer to sysHost
  @param SADEntry - Pointer to the SAD Table Entry for which we are configuring SAD2TAD bits
  @param meshSad2Tad - Pointer to the SAD2TAD entry that is going to be configured

  @retval N/A

**/
VOID
ConfigMesh2MemCsrForMirrorRASHook (
  PSYSHOST                     host,
  struct SADTable              *SADEntry,
  SAD2TAD_M2MEM_MAIN_STRUCT    *meshSad2Tad
  )
{

  // Same mode covers 1LM and 2LM because 1LM and 2LM are mutually exclusive
  if (SADEntry->mirrored == 1) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n<ConfigMesh2MemCsrForMirrorRASHook: Setting mirror bit>\n"));
    meshSad2Tad->Bits.mirrorddr4 = 1;
  }
  return ;
}


/**

  SetVLSModePerChannel Configure on the fly burst mode for Static Virtual Lockstep mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
SetVLSModePerChannel(
  PSYSHOST host,
  UINT8    socket
)
{
  UINT8                                   ch;
  UINT8                                   roundTripDiff;
  struct channelNvram                     (*channelNvList)[MAX_CH];
  DEFEATURES0_M2MEM_MAIN_STRUCT           m2mDefeatures0;
  MCSCHED_CHKN_BIT2_MCDDC_CTL_STRUCT      chknBit2;
  VMSE_ERROR_MCDDC_DP_STRUCT              vmseerrordata;
  M2MBIOSQUIESCE_M2MEM_MAIN_STRUCT        M2mBiosQuiesce;

  if (((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) == STAT_VIRT_LOCKSTEP)) {

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n<SetVLSModePerChannel(socket = %d)>\n", socket));
    channelNvList = GetChannelNvList(host, socket);

    if (host->var.common.bootMode == NormalBoot) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\nch=%d Channel not enabled, skipping it \n", ch));
          continue;            // Channel not enabled, does not disable sVLS though
        }

        m2mDefeatures0.Data = MemReadPciCfgMC (host, socket, (ch/MAX_MC_CH), DEFEATURES0_M2MEM_MAIN_REG);
        m2mDefeatures0.Bits.wait4bothhalves = 1;
        MemWritePciCfgMC (host, socket, (ch/MAX_MC_CH), DEFEATURES0_M2MEM_MAIN_REG, m2mDefeatures0.Data);

        roundTripDiff = (*channelNvList)[ch].maxRoundTrip - (*channelNvList)[ch].minRoundTrip;
        chknBit2.Data = MemReadPciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
        //5332728: CLONE SKX Sighting: SVL running 1 thread of multiWriteCacheline causes Buddy to return stale data

          chknBit2.Bits.x8_wpq_dly = 8 + roundTripDiff;

        //End 5332728
        MemWritePciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG, chknBit2.Data);

        //5371638: CLONE SKX Sighting: SVL failing on supercollider w/CECC
        if((host->var.common.stepping == B0_REV_SKX) || (host->var.common.stepping == L0_REV_SKX)|| (host->var.common.stepping == B1_REV_SKX)) {
          vmseerrordata.Data = MemReadPciCfgEp(host, socket, ch, VMSE_ERROR_MCDDC_DP_REG);
          vmseerrordata.Bits.vmse_err_latency += (chknBit2.Bits.x8_wpq_dly - 4);
          MemWritePciCfgEp (host, socket, ch , VMSE_ERROR_MCDDC_DP_REG, vmseerrordata.Data);
        }
        // End 5371638

      } // ch loop
    } // NormalBoot
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n</SetVLSModePerChannel>\n"));
  } // Is sVLS enabled
  else { // SVL disabled
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      chknBit2.Data = MemReadPciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
      chknBit2.Bits.x8_wpq_dly = 8;

      MemWritePciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG, chknBit2.Data);
    }
  }

  channelNvList = GetChannelNvList(host, socket);
  if (host->var.common.bootMode == NormalBoot) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n Program drainrttimer\n"));
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\nch=%d Channel not enabled \n", ch));
          continue;            // Channel not enabled
        }
        M2mBiosQuiesce.Data = MemReadPciCfgMC (host, socket, (ch/MAX_MC_CH), M2MBIOSQUIESCE_M2MEM_MAIN_REG);
        M2mBiosQuiesce.Bits.drainrttimer =  0x78;
        MemWritePciCfgMC (host, socket, (ch/MAX_MC_CH), M2MBIOSQUIESCE_M2MEM_MAIN_REG, M2mBiosQuiesce.Data);
      }
  }

  if( (host->nvram.mem.RASmodeEx & ADDDC_EN) == ADDDC_EN) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\t ADDDC enabled\n", socket));
    channelNvList = GetChannelNvList(host, socket);

    if (host->var.common.bootMode == NormalBoot) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\nch=%d Channel not enabled, skipping it \n", ch));
           continue;            // Channel not enabled, does not disable sVLS though
        }

        roundTripDiff = (*channelNvList)[ch].maxRoundTrip - (*channelNvList)[ch].minRoundTrip;
        chknBit2.Data = MemReadPciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
        chknBit2.Bits.adddc_wpq_dly = 5 + roundTripDiff;
        MemWritePciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG, chknBit2.Data);
      } // ch loop
    } // if (host->var.common.bootMode == NormalBoot)
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n</ADDDC enabled>\n"));
  } // if LS enabled in setup
  return;
}

/**

  Configure Lockstep mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
SetVLSRegionPerChannel (
  PSYSHOST host,
  UINT8    socket
)
{
  UINT8                             ch;
  struct channelNvram               (*channelNvList)[MAX_CH];
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT  RegCtrl;
  MCMTR_MC_MAIN_STRUCT       McMtrMain;
  UINT8 RegionNum;
  UINT8 RankOffset = 00;
  UINT8 RankLoop = 00;

  if ((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) != STAT_VIRT_LOCKSTEP) return;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Configure Lockstep mode
  // Note: lockstep capability bit checked in checkSupportedRASModes() function
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n<SetVLSRegionPerChannel\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n<SetVLSRegionPerChannel(Enable:%d)>\n", host->nvram.mem.socket[socket].channelList[ch].enabled));
    // sVLS is enabled on system-wide level.
    // Ignore disabled channels though
    if ((*channelNvList)[ch].enabled == 0) {
      continue;            // Channel not enabled, skip it
    }

    if ( (host->nvram.mem.socket[socket].channelList[ch].dimmList[0].dimmPresent == 1) &&
        (host->nvram.mem.socket[socket].channelList[ch].dimmList[1].dimmPresent == 1) ) {
      RankOffset = 04;
      RankLoop = (*channelNvList)[ch].dimmList[0].numRanks;
    } else {
      RankOffset = (*channelNvList)[ch].dimmList[0].numRanks /2;
      RankLoop = (*channelNvList)[ch].dimmList[0].numRanks /2;
    }

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "<SetVLSRegionPerChannel>\n"));

    McMtrMain.Data = MemReadPciCfgMC(host, socket, (ch/MAX_MC_CH), MCMTR_MC_MAIN_REG);
    McMtrMain.Bits.adddc_mode = 1;
    MemWritePciCfgMC (host, socket, (ch/MAX_MC_CH), MCMTR_MC_MAIN_REG, McMtrMain.Data);
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "<MCMTR_MC_MAIN.adddc_mode = %d>\n",McMtrMain.Bits.adddc_mode));

    for(RegionNum=0;RegionNum<RankLoop;RegionNum++) {

      RegCtrl.Data = MemReadPciCfgEp (host, socket, ch, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + (RegionNum*4)));
      RegCtrl.Bits.region_enable = 1;
      RegCtrl.Bits.region_size = ADDDC_REGION_SIZE_RANK;
      RegCtrl.Bits.failed_cs = RegionNum;
      RegCtrl.Bits.failed_c = 0;
      RegCtrl.Bits.failed_ba = 0;
      RegCtrl.Bits.failed_bg = 0;
      RegCtrl.Bits.nonfailed_cs = RegionNum + RankOffset;
      RegCtrl.Bits.nonfailed_c = 0;
      RegCtrl.Bits.nonfailed_ba = 0;
      RegCtrl.Bits.nonfailed_bg = 0;
      RegCtrl.Bits.copy_in_progress = 0;
      MemWritePciCfgEp (host, socket, ch, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + (RegionNum*4)), RegCtrl.Data);

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n <ADDDC_REGION%d_CONTROL.region_enable = %d>\n",RegionNum, RegCtrl.Bits.region_enable));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.failed_cs = %d>\n",RegionNum, RegCtrl.Bits.failed_cs));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.failed_c = %d>\n",RegionNum, RegCtrl.Bits.failed_c));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.failed_ba = %d>\n",RegionNum, RegCtrl.Bits.failed_ba));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.failed_bg = %d>\n",RegionNum, RegCtrl.Bits.failed_bg));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.nonfailed_cs = %d>\n",RegionNum, RegCtrl.Bits.nonfailed_cs));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.nonfailed_c = %d>\n",RegionNum, RegCtrl.Bits.nonfailed_c));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.nonfailed_ba = %d>\n",RegionNum, RegCtrl.Bits.nonfailed_ba));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.nonfailed_bg = %d>\n",RegionNum, RegCtrl.Bits.nonfailed_bg));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "<ADDDC_REGION%d_CONTROL.copy_in_progress = %d>\n",RegionNum, RegCtrl.Bits.copy_in_progress));
    }

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "</SetVLSRegionPerChannel ch>\n"));
  } // mcId loop

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "Lockstep enabled\n"));
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "</SetVLSRegionPerChannel>\n"));
  return;
}


/**

  Configure MC Link Fail feature

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/

VOID
SetMCLinkFail (
    PSYSHOST host,
    UINT8    socket
    )
{
  // Iterators
  UINT8  ch;
  UINT8  Imc;
  UINT8  Dimm;
  UINT8  SktCh;
  UINT8  NumRanks;

  // Aux vars
  UINT32  ZqLongDclk;
  UINT32  MaxRoundTrip;
  UINT32  WriteReplayDclk;
  UINT32  WorstCaseCapDclk;
  UINT32  ObservationLimitDclk;
  UINT8   PopulatedDimmsOnChannel;

  // Registers structures
  TCDBP_MCDDC_CTL_STRUCT                    TcdbpMcddcCtl;
  TCRFTP_MCDDC_CTL_STRUCT                   TcrftpMcddcCtl;
  TCSRFTP_MCDDC_CTL_STRUCT                  TcsrftpMcddcCtl;
  LINK_LINK_FAIL_MCDDC_DP_STRUCT            LinkLinkFailMcddc;
  LINK_MCA_CTL_MCDDC_DP_STRUCT              LinkMcaCtlMcddc;
  LINK_RETRY_TIMER_MCDDC_DP_STRUCT          LinkRetryTimerMcddc;
  LINK_RETRY_ERR_LIMITS_MCDDC_DP_STRUCT     LinkRetryErrLimits;


  //
  // Enable Link Fail/Retry/Retry Error Limits for each IMC
  //
  for (Imc = 0; Imc < host->var.mem.maxIMC; Imc++) {

    // SKip if IMC not enabled
    if (host->nvram.mem.socket[socket].imc[Imc].enabled == 0) continue;

    // Check all channels for current IMC
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      // Looping over all the imcs calculate channel number for socket
      SktCh = NODECH_TO_SKTCH(Imc, ch);

      // Skip if any channel is not populated
      if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n ===========Socket[%d] - SktCh[%d]===========\n", socket, SktCh));

      // Set err1 and err4 Regardless Full Mirroring
      LinkMcaCtlMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_MCA_CTL_MCDDC_DP_REG);
      LinkMcaCtlMcddc.Bits.err0_en = 0;
      LinkMcaCtlMcddc.Bits.err0_log  = 1;
      LinkMcaCtlMcddc.Bits.err0_smi  = 1;
      LinkMcaCtlMcddc.Bits.err4_en   = 1;
      LinkMcaCtlMcddc.Bits.err4_log  = 1;
      LinkMcaCtlMcddc.Bits.err4_smi  = 1;
      MemWritePciCfgEp (host, socket, SktCh, LINK_MCA_CTL_MCDDC_DP_REG, LinkMcaCtlMcddc.Data);

      LinkMcaCtlMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_MCA_CTL_MCDDC_DP_REG);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\nReading back LinkMcaCtlMcddc data\nerr0_en:%d\nerr0_log:%d\nerr0_smi:%d\nerr4_en:%d\nerr4_log:%d\nerr4_smi:%d\n",
                         LinkMcaCtlMcddc.Bits.err0_en,
                         LinkMcaCtlMcddc.Bits.err0_log,
                         LinkMcaCtlMcddc.Bits.err0_smi,
                         LinkMcaCtlMcddc.Bits.err4_en,
                         LinkMcaCtlMcddc.Bits.err4_log,
                         LinkMcaCtlMcddc.Bits.err4_smi));

      // Disable Link Retry Error Limits in case Full Mirroring not enabled
      LinkRetryErrLimits.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
      LinkRetryErrLimits.Bits.sb_err_enable = 0;
      MemWritePciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG, LinkRetryErrLimits.Data);

      LinkRetryErrLimits.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Reading back LinkRetryErrLimits.sb_err_enable: %d\n", LinkRetryErrLimits.Bits.sb_err_enable));

      // Skip Link Fail/Retry/Retry Error Limits and TCRFTP cfg if Full Mirroring not enabled
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\nFULL_MIRROR: %d\nCAP: %d", ((host->nvram.mem.RASmode & FULL_MIRROR_1LM) || (host->nvram.mem.RASmode & FULL_MIRROR_2LM)), (host->setup.mem.options & CA_PARITY_EN)));

      if ( !((host->nvram.mem.RASmode & FULL_MIRROR_1LM) ||
             (host->nvram.mem.RASmode & FULL_MIRROR_2LM)) ) continue;

      //
      // Update Link Fail cfg
      //
      LinkLinkFailMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_LINK_FAIL_MCDDC_DP_REG);
      LinkLinkFailMcddc.Bits.fail_threshold = 1;
      MemWritePciCfgEp (host, socket, SktCh, LINK_LINK_FAIL_MCDDC_DP_REG, LinkLinkFailMcddc.Data);

      LinkLinkFailMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_LINK_FAIL_MCDDC_DP_REG);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Reading back LinkLinkFailMcddc.Bits.fail_threshold: %d\n", LinkLinkFailMcddc.Bits.fail_threshold));

      //
      // Get TCRFTP value to calculate Obersvation Time limits for Dclk
      //
      TcrftpMcddcCtl.Data = MemReadPciCfgEp (host, socket, SktCh, TCRFTP_MCDDC_CTL_REG);
      ObservationLimitDclk  = (TcrftpMcddcCtl.Bits.t_refi * 9 * 90) / 100;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n ObservationLimitDclk: %d\n", ObservationLimitDclk));

      //
      // Update Link Retry Timer cfg
      //
      LinkRetryTimerMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_TIMER_MCDDC_DP_REG);
      LinkRetryTimerMcddc.Bits.link_fail_observation_time = 0;
      LinkRetryTimerMcddc.Bits.sb_err_observation_time    = 7;
      LinkRetryTimerMcddc.Bits.link_err_flow_time_unit    = ObservationLimitDclk / 128;
      MemWritePciCfgEp (host, socket, SktCh, LINK_RETRY_TIMER_MCDDC_DP_REG, LinkRetryTimerMcddc.Data);

      LinkRetryTimerMcddc.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_TIMER_MCDDC_DP_REG);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Reading back LinkRetryTimerMcddc Data \nlink_fail_observation_time: %d\nsb_err_observation_time: %d\nlink_err_flow_time_unit: %d\n",
                          LinkRetryTimerMcddc.Bits.link_fail_observation_time,
                          LinkRetryTimerMcddc.Bits.sb_err_observation_time,
                          LinkRetryTimerMcddc.Bits.link_err_flow_time_unit));

      //
      // Calculate worst case CAP in Dclk
      //
      NumRanks                = host->nvram.mem.socket[socket].channelList[SktCh].numRanks;
      MaxRoundTrip            = host->nvram.mem.socket[socket].channelList[SktCh].maxRoundTrip * 2; // qClk to dClk
      PopulatedDimmsOnChannel = 0;

      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (host->nvram.mem.socket[socket].channelList[SktCh].dimmList[Dimm].dimmPresent == 0) continue;
        PopulatedDimmsOnChannel++;
      } // Dimm

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n NumRanks: %d\nMaxRoundTrip: %d\nPopulatedDimmsOnChannel: %d\n",
                     NumRanks, MaxRoundTrip, PopulatedDimmsOnChannel));

      //
      // Calculate ZqLongDclk
      //
      TcsrftpMcddcCtl.Data = MemReadPciCfgEp (host, socket, SktCh, TCSRFTP_MCDDC_CTL_REG);
      ZqLongDclk = TcsrftpMcddcCtl.Bits.t_zqoper * NumRanks;

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n ZqLongDclk: %d\n", ZqLongDclk));
      //
      // There are 39 WPQ entries, thus calculate WriteReplayDclk
      //
      TcdbpMcddcCtl.Data = MemReadPciCfgEp (host, socket, SktCh, TCDBP_MCDDC_CTL_REG);
      WriteReplayDclk = TcdbpMcddcCtl.Bits.t_cwl * 39;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n WriteReplayDclk: %d\n", WriteReplayDclk));


      // MAX Round Trip doubled to account for both read drain and read replay states
      // Time for precharge, refresh and RC6 ignored.
      WorstCaseCapDclk = (2 * MaxRoundTrip) + ZqLongDclk + WriteReplayDclk;

      if (host->setup.mem.optionsExt & DIMM_ISOLATION_EN) {
        WorstCaseCapDclk += (650 * PopulatedDimmsOnChannel);  // Empirical estimate, 650 dClk per Dimm installed
      }

      //
      // Update Link Retry Error Limits cfg
      //
      LinkRetryErrLimits.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
      LinkRetryErrLimits.Bits.sb_err_limit  = ObservationLimitDclk / WorstCaseCapDclk;
      if ((host->setup.mem.options & CA_PARITY_EN)) {
        LinkRetryErrLimits.Bits.sb_err_enable = 1;
      }
      MemWritePciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG, LinkRetryErrLimits.Data);
      LinkRetryErrLimits.Data = MemReadPciCfgEp (host, socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Reading back LinkRetryErrLimits Data\nsb_err_limit: %d\nsb_err_enable: %d\n",
                          LinkRetryErrLimits.Bits.sb_err_limit,
                          LinkRetryErrLimits.Bits.sb_err_enable));
    } // ch
  } // Imc
}

/**

  Configure Mirror mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static VOID
SetMirrorMode (
    PSYSHOST host,
    UINT8    socket
    )
{
  UINT8   SadIndex;
  UINT8   TadIndex;
  UINT8   Imc;
  UINT8   ch;
  UINT8   SktCh;
  UINT8   Ddr4ChnlFailed;
  UINT8   DdrtChnlFailed;

  RASENABLES_MC_MAIN_STRUCT         RasEnables;
  MODE_M2MEM_MAIN_STRUCT            RasModes;
  MIRRORCHNLS_M2MEM_MAIN_STRUCT     MirrorChannels;
  DEFEATURES0_M2MEM_MAIN_STRUCT     M2mDefeatures0;
  DEFEATURES1_M2MEM_MAIN_STRUCT     M2mDefeatures1;
  SYSFEATURES0_M2MEM_MAIN_STRUCT    SysFeatures0;
  MIRRORFAILOVER_M2MEM_MAIN_STRUCT  MirrorFailover;
  ERR_CNTR_CTL_M2MEM_MAIN_STRUCT    ErrCtrlM2MCtl;
  TADBASE_0_MC_MAIN_STRUCT          mcTADBase;
  AMAP_MC_MAIN_STRUCT               AmapMain;
  TIMEOUT_M2MEM_MAIN_STRUCT         TimeoutM2M;
  UINT32                            TimeOutLockVal;
  PREFETCHSAD_M2MEM_MAIN_STRUCT     PrefM2MMain;
  WRTRKRALLOC_M2MEM_MAIN_STRUCT     WrTrkrAlloc;
  DDRT_MISC_CTL_MC_2LM_STRUCT       DDRT2LMMisc;
  MCNMCACHINGCFG_MC_MAIN_STRUCT     McNMCachingCfg;
  DDR4_VIRAL_CTL_MCDDC_DP_STRUCT    Ddr4ViralCtl;
  MCDECS_CHICKEN_BITS_MC_MAIN_STRUCT  McChickenBits;

  UINT32 TADBase[TAD_RULES] =
    { TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
      TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG } ;

  struct SADTable   *SAD;
  struct TADTable   *TAD;

  // Set Link Fail if Full Mirroring
  SetMCLinkFail (host, socket);

  // Check if full mirroring (1LM ^ 2LM) is supported by hardware or enabled in BIOS setup

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n<SetMirrorMode(socket = %d)>\n", socket));
  for (SadIndex = 0; SadIndex < MAX_SAD_RULES; SadIndex++) {
    SAD = &host->var.mem.socket[socket].SAD[SadIndex];

    if(SAD->Enable == 0) break;
    if (!SAD->local) continue;

    //skip SAD that is not mirrored if its partial mirroring
    if(!SAD->mirrored) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n SetMirrorMode:SAD not mirrored %d\n", SadIndex));
      continue;
    }

    //
    // Program Mirror Enables for each IMC
    //
    for (Imc = 0; Imc < host->var.mem.maxIMC; Imc++) {

    // SKip if IMC not enabled
      if (host->nvram.mem.socket[socket].imc[Imc].enabled == 0) continue;

      // Skip IMC that is not part of the SAD
      if ((SAD->imcInterBitmap & (1 << Imc)) == 0) continue;

      //
      // 5331216 : CLONE SKX Sighting: A1: TOR TO on MP5 MIRROR config
      // set the threshold for non_TGR mirror writes and partial mirror writes
      //
      WrTrkrAlloc.Data = MemReadPciCfgMC (host, socket, Imc, WRTRKRALLOC_M2MEM_MAIN_REG);
      WrTrkrAlloc.Bits.mirrtrkrpwrthresh = 0x13;
      WrTrkrAlloc.Bits.mirrtrkrthresh  = 0x15;
      if ( CheckSteppingGreaterThan (host, CPU_SKX, A2_REV_SKX) ) {
        WrTrkrAlloc.Bits.akwrcmphysthi=0xA;
        WrTrkrAlloc.Bits.akwrcmphystlo=0x9;
      }
      MemWritePciCfgMC (host, socket, Imc, WRTRKRALLOC_M2MEM_MAIN_REG, WrTrkrAlloc.Data);

      //
      // Update MIRRORCHNLS_M2MEM for mirroring
      //
      MirrorChannels.Data = MemReadPciCfgMC (host, socket, Imc, MIRRORCHNLS_M2MEM_MAIN_REG);
      RasModes.Data = MemReadPciCfgMC (host, socket, Imc, MODE_M2MEM_MAIN_REG);
      RasEnables.Data = MemReadPciCfgMC (host, socket, Imc, RASENABLES_MC_MAIN_REG);

      for(ch = 0; ch< MAX_MC_CH; ch++) {
        //Looping over all the imcs calculate channel number for socket
        SktCh = NODECH_TO_SKTCH(Imc,ch);
        //skip if any channel is not populated
        if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;
        AmapMain.Data = MemReadPciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG);
        AmapMain.Bits.force_lat = 1;
        MemWritePciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG, AmapMain.Data);

        AmapMain.Data = MemReadPciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n Reading back AmapMain.Bits.force_lat %d\n", AmapMain.Bits.force_lat));
      }

      //HSD 4929175 WA Cloned From SKX Si Bug Eco: Timeout tracker does not deallocate entries correctly in mirror mode
      if(host->var.common.stepping < B0_REV_SKX) {
        TimeoutM2M.Data = MemReadPciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG);
        TimeOutLockVal = TimeoutM2M.Bits.timeoutlock;
        TimeoutM2M.Bits.timeoutlock = 0;
        MemWritePciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG, TimeoutM2M.Data);

        TimeoutM2M.Data = MemReadPciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG);
        TimeoutM2M.Bits.timeouten = 0;
        MemWritePciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG, TimeoutM2M.Data);

        TimeoutM2M.Data = MemReadPciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG);
        TimeoutM2M.Bits.timeoutlock = TimeOutLockVal;
        MemWritePciCfgMC (host, socket, Imc, TIMEOUT_M2MEM_MAIN_REG, TimeoutM2M.Data);
      }
      //HSD 4929093: WA for 1LM Mirror Hang
      M2mDefeatures1.Data = MemReadPciCfgMC (host, socket, Imc, DEFEATURES1_M2MEM_MAIN_REG);


      //5371633: CLONE SKX Sighting: Persistent CAP errors causes Windows BSOD when in channel mirror mode
      for(ch = 0; ch< MAX_MC_CH; ch++)
      {
        SktCh = NODECH_TO_SKTCH(Imc,ch);
        if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;
        Ddr4ViralCtl.Data =  MemReadPciCfgEp (host, socket, SktCh, DDR4_VIRAL_CTL_MCDDC_DP_REG);
        Ddr4ViralCtl.Bits.dis_viral_link_fail = 1;
        MemWritePciCfgEp (host, socket, SktCh, DDR4_VIRAL_CTL_MCDDC_DP_REG, Ddr4ViralCtl.Data);
      }

      if (SAD->type == MEM_TYPE_1LM) {

        switch (SAD->channelInterBitmap[Imc]) {
          case BITMAP_CH0_CH1:
            MirrorChannels.Bits.ddr4chnl0secondary = 1;
            MirrorChannels.Bits.ddr4chnl1secondary = 0;
            MirrorChannels.Bits.ddr4chnl2secondary = 2;
            RasEnables.Bits.ch0_mirror = 1;
            RasEnables.Bits.ch1_mirror = 0;
            RasEnables.Bits.ch2_mirror = 2;
            break;
          case BITMAP_CH1_CH2:
            MirrorChannels.Bits.ddr4chnl0secondary = 0;
            MirrorChannels.Bits.ddr4chnl1secondary = 2;
            MirrorChannels.Bits.ddr4chnl2secondary = 1;
            RasEnables.Bits.ch0_mirror = 0;
            RasEnables.Bits.ch1_mirror = 2;
            RasEnables.Bits.ch2_mirror = 1;
            break;
          case BITMAP_CH0_CH2:
            MirrorChannels.Bits.ddr4chnl0secondary = 2;
            MirrorChannels.Bits.ddr4chnl1secondary = 1;
            MirrorChannels.Bits.ddr4chnl2secondary = 0;
            RasEnables.Bits.ch0_mirror = 2;
            RasEnables.Bits.ch1_mirror = 1;
            RasEnables.Bits.ch2_mirror = 0;
            break;
          case BITMAP_CH0_CH1_CH2:
            MirrorChannels.Bits.ddr4chnl0secondary = 1;
            MirrorChannels.Bits.ddr4chnl1secondary = 2;
            MirrorChannels.Bits.ddr4chnl2secondary = 0;
            RasEnables.Bits.ch0_mirror = 1;
            RasEnables.Bits.ch1_mirror = 2;
            RasEnables.Bits.ch2_mirror = 0;
            break;
          default:
            // control should never come here
            ;
        } // switch
        //}
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n SetMirrorMode:mirrorddr4 sad %d\n", SadIndex));
        RasModes.Bits.mirrorddr4 = 1; // enable DDR4 mirror mode

        // Program iMC RAS Enables to enable Mirror mode
        RasEnables.Bits.mirroren = 1;

        //HSD 4929093: WA for 1LM Mirr Hang
        M2mDefeatures1.Bits.ddrtmirrscrubwrdis = 0;
        for(ch = 0; ch< MAX_MC_CH; ch++)
        {
          SktCh = NODECH_TO_SKTCH(Imc,ch);
          McChickenBits.Data  = (UINT8)MemReadPciCfgEp (host, socket, SktCh, MCDECS_CHICKEN_BITS_MC_MAIN_REG);
          if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;
          McChickenBits.Bits.defeature_2 = 1;
          MemWritePciCfgEp (host, socket, SktCh, MCDECS_CHICKEN_BITS_MC_MAIN_REG, McChickenBits.Data);
        }

      } else if ((SAD->type == MEM_TYPE_2LM) || (SAD->type == MEM_TYPE_PMEM) || (SAD->type == MEM_TYPE_PMEM_CACHE)) {

        switch (SAD->FMchannelInterBitmap[Imc]) {
          case BITMAP_CH0_CH1:
            MirrorChannels.Bits.ddrtchnl0secondary = 1;
            MirrorChannels.Bits.ddrtchnl1secondary = 0;
            MirrorChannels.Bits.ddrtchnl2secondary = 2;
            RasEnables.Bits.ch0_mirror = 1;
            RasEnables.Bits.ch1_mirror = 0;
            RasEnables.Bits.ch2_mirror = 2;
            break;
          case BITMAP_CH1_CH2:
            MirrorChannels.Bits.ddrtchnl0secondary = 0;
            MirrorChannels.Bits.ddrtchnl1secondary = 2;
            MirrorChannels.Bits.ddrtchnl2secondary = 1;
            RasEnables.Bits.ch0_mirror = 0;
            RasEnables.Bits.ch1_mirror = 2;
            RasEnables.Bits.ch2_mirror = 1;
            break;
          case BITMAP_CH0_CH2:
            MirrorChannels.Bits.ddrtchnl0secondary = 2;
            MirrorChannels.Bits.ddrtchnl1secondary = 1;
            MirrorChannels.Bits.ddrtchnl2secondary = 0;
            RasEnables.Bits.ch0_mirror = 2;
            RasEnables.Bits.ch1_mirror = 1;
            RasEnables.Bits.ch2_mirror = 0;
            break;
          case BITMAP_CH0_CH1_CH2:
            MirrorChannels.Bits.ddrtchnl0secondary = 1;
            MirrorChannels.Bits.ddrtchnl1secondary = 2;
            MirrorChannels.Bits.ddrtchnl2secondary = 0;
            RasEnables.Bits.ch0_mirror = 1;
            RasEnables.Bits.ch1_mirror = 2;
            RasEnables.Bits.ch2_mirror = 0;
            break;
          default:
            // control should never come here
            ;
        } // switch
        //}
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n SetMirrorMode:mirrorddrt sad %d\n", SadIndex));
        RasModes.Bits.mirrorddrt = 1; // enable NGN mirror mode

        // HSD 5332028
        // for 2LM
        // s0.M2M_MC*_CR_SAD2TAD.MirrorDDR4 = 1
        // M2M[*].MODE.mirrorddr4 = 0
        // M2M[*].MODE.mirrorddrt = 1


        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n ClearMirrorMode:mirrordd4 sad %d\n", SadIndex));
        RasModes.Bits.mirrorddr4 = 0;

        // Program iMC RAS Enables to enable Mirror mode
        RasEnables.Bits.mirroren = 1;
        McNMCachingCfg.Data = MemReadPciCfgMC (host, socket, Imc, MCNMCACHINGCFG_MC_MAIN_REG);
        McNMCachingCfg.Bits.mcmirrormode = 1;
        MemWritePciCfgMC (host, socket, Imc, MCNMCACHINGCFG_MC_MAIN_REG, McNMCachingCfg.Data);

        for(ch = 0; ch< MAX_MC_CH; ch++)
        {
          SktCh = NODECH_TO_SKTCH(Imc,ch);

          //skip if any channel is not populated
          if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;

          AmapMain.Data = MemReadPciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG);
          AmapMain.Bits.force_lat = 0;
          MemWritePciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG, AmapMain.Data);

          DDRT2LMMisc.Data = MemReadPciCfgEp (host, socket, SktCh, DDRT_MISC_CTL_MC_2LM_REG);
          DDRT2LMMisc.Bits.ddrt_dec_force_lat = 1;
          MemWritePciCfgEp (host, socket, SktCh, DDRT_MISC_CTL_MC_2LM_REG, DDRT2LMMisc.Data);
        }

      } // if (SAD->type)

      for(TadIndex=0;TadIndex<TAD_RULES;TadIndex++){
        TAD = &host->var.mem.socket[socket].imc[Imc].TAD[TadIndex];
        if(TAD->SADId==SadIndex) {
          if(TAD->Enable)
          {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "TadBase enable here. TADIndex:%d \n",TadIndex));
            mcTADBase.Data = MemReadPciCfgMC (host, socket, Imc, TADBase[TadIndex]);
            mcTADBase.Bits.mirror_en = 1;
            mcTADBase.Bits.ign_ptrl_uc = 1;
            MemWritePciCfgMC (host, socket, Imc, TADBase[TadIndex], mcTADBase.Data);
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "mcTADBase[%d].Bits.mirror_en:%d \n", TadIndex, mcTADBase.Bits.mirror_en));
          }
        }
      }

      M2mDefeatures0.Data = MemReadPciCfgMC (host, socket, Imc, DEFEATURES0_M2MEM_MAIN_REG);
      M2mDefeatures0.Bits.wait4bothhalves = 1;
      M2mDefeatures0.Bits.ingbypdis = 1;
      M2mDefeatures0.Bits.egrbypdis = 1;
      M2mDefeatures0.Bits.ecconretry = 0;
      M2mDefeatures0.Bits.demandscrubwrdis = 0;

      /*4930340: Cloned From SKX Si Bug Eco: mirror tracker
      entry not retired after corrected/transient errors on both channels*/

      if((host->var.common.stepping < B0_REV_SKX) && (SAD->type == MEM_TYPE_1LM)) {
        M2mDefeatures0.Bits.badchnlfirstforpwr = 1;
      }

      //HSD 5331396: w/a
      if((SAD->type == MEM_TYPE_PMEM) || (SAD->type == MEM_TYPE_2LM)) {
        M2mDefeatures0.Bits.badchnlfirstforpwr = 1;
      }

      M2mDefeatures0.Bits.demandscrubwrdis = 0;

      // Demand Scrub Disable  shall not be used according to b311409
      //M2mDefeatures0.Bits.demandscrubwrdis = 0;

      /*4930340: Cloned From SKX Si Bug Eco: mirror tracker
      entry not retired after corrected/transient errors on both channels*/

      if((host->var.common.stepping < B0_REV_SKX) && (SAD->type == MEM_TYPE_1LM)) {
        M2mDefeatures0.Bits.badchnlfirstforpwr = 1;
      }

      //HSD 5331396: w/a
      if((SAD->type == MEM_TYPE_PMEM) || (SAD->type == MEM_TYPE_2LM)) {
        M2mDefeatures0.Bits.badchnlfirstforpwr = 1;
      }

      MemWritePciCfgMC (host, socket, Imc, DEFEATURES0_M2MEM_MAIN_REG, M2mDefeatures0.Data);

      M2mDefeatures1.Bits.skipbadchnlforrd = 0;

      //HSD 5331396: w/a
      if((SAD->type == MEM_TYPE_PMEM) || (SAD->type == MEM_TYPE_2LM)) {
        M2mDefeatures1.Bits.skipbadchnlforpwr = 0;
      }

      //Si W/A: 307536
      if(SAD->type == MEM_TYPE_1LM) {
        M2mDefeatures1.Bits.egreco |= 0x08;
      }
      else {
        M2mDefeatures1.Bits.egreco &= ~(1 << BIT3);
      }

      PrefM2MMain.Data = MemReadPciCfgMC (host, socket, Imc, PREFETCHSAD_M2MEM_MAIN_REG);
      PrefM2MMain.Bits.prefsadvld=0;
      MemWritePciCfgMC (host, socket, Imc, PREFETCHSAD_M2MEM_MAIN_REG, PrefM2MMain.Data);

      SysFeatures0.Data = MemReadPciCfgMC (host, socket, Imc, SYSFEATURES0_M2MEM_MAIN_REG);
      SysFeatures0.Bits.pcommitforcebroadcast = 1;
      //
      // By default - enable BIOS supported mirror fail over when a mirror scrub fails to correct error
      // note: after permanant failover a failed mirrored channel is not access further by IMC
      //
      SysFeatures0.Bits.immediatefailoveractionena = 0;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "BIOS assisted failover enabled = %d\n", SysFeatures0.Bits.immediatefailoveractionena));

      MemWritePciCfgMC (host, socket, Imc, SYSFEATURES0_M2MEM_MAIN_REG, SysFeatures0.Data);

      ErrCtrlM2MCtl.Data = MemReadPciCfgMC(host, socket, Imc, ERR_CNTR_CTL_M2MEM_MAIN_REG);
      ErrCtrlM2MCtl.Bits.cntena = 1; // Enable counting by all the M2M internal error counters
      ErrCtrlM2MCtl.Bits.clrcntrs = 1; // Clear all the M2M internal error counters
      ErrCtrlM2MCtl.Bits.cntridx = MIRRSCRUBRD_ERR_TYPE;
      ErrCtrlM2MCtl.Bits.thresholdidx = MIRRSCRUBRD_ERR_TYPE;
      ErrCtrlM2MCtl.Bits.thresholdwr = 1; // Indicate that we want to trigger because of MIRRSCRUBRD_ERR_TYPE
      MemWritePciCfgMC (host, socket, Imc, ERR_CNTR_CTL_M2MEM_MAIN_REG, ErrCtrlM2MCtl.Data);

      MemWritePciCfgMC (host, socket, Imc, MIRRORCHNLS_M2MEM_MAIN_REG, MirrorChannels.Data);
      MemWritePciCfgMC (host, socket, Imc, MODE_M2MEM_MAIN_REG, RasModes.Data);
      MemWritePciCfgMC (host, socket, Imc, RASENABLES_MC_MAIN_REG, RasEnables.Data);
      MemWritePciCfgMC (host, socket, Imc, DEFEATURES1_M2MEM_MAIN_REG, M2mDefeatures1.Data);

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddr4chnl0secondary = %d\n", Imc, MirrorChannels.Bits.ddr4chnl0secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddr4chnl1secondary = %d\n", Imc, MirrorChannels.Bits.ddr4chnl1secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddr4chnl2secondary = %d\n", Imc, MirrorChannels.Bits.ddr4chnl2secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddrtchnl0secondary = %d\n", Imc, MirrorChannels.Bits.ddrtchnl0secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddrtchnl1secondary = %d\n", Imc, MirrorChannels.Bits.ddrtchnl1secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MIRRORCHNLS.ddrtchnl2secondary = %d\n", Imc, MirrorChannels.Bits.ddrtchnl2secondary));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "RasEnables.Bits.ch0_mirror = %d\n", RasEnables.Bits.ch0_mirror));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "RasEnables.Bits.ch1_mirror = %d\n", RasEnables.Bits.ch1_mirror));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "RasEnables.Bits.ch2_mirror = %d\n", RasEnables.Bits.ch2_mirror));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MODE.mirrorddr4 = %d\n", Imc, RasModes.Bits.mirrorddr4));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].MODE.mirrorddrt = %d\n", Imc, RasModes.Bits.mirrorddrt));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].ERR_CNTR_CTL_M2MEM_MAIN_REG =0x%x\n", Imc, ErrCtrlM2MCtl.Data));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "M2M[%d].RASENABLES_MC_MAINEXT.mirroren = 0x%x\n", Imc, RasEnables.Bits.mirroren));

      //
      // On S3 RESUME, set M2M fail over status from NVRAM
      //
      if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast) {
        Ddr4ChnlFailed = DdrtChnlFailed = 0;

        // Find ddr4 and ngn channels disabled due to UC (MFO)
        for (ch = 0; ch < MAX_CH; ch++) {
          if (host->nvram.mem.socket[socket].channelList[ch].enabled == 0) continue;

          if (host->nvram.mem.socket[socket].channelList[ch].chFailed == 1) {
            Ddr4ChnlFailed |= (1 << (ch % host->var.mem.numChPerMC));
          }
          if (host->nvram.mem.socket[socket].channelList[ch].ngnChFailed == 1) {
            DdrtChnlFailed |= (1 << (ch % host->var.mem.numChPerMC));
          }

        } // for (ch)
        MirrorFailover.Data = MemReadPciCfgEp (host, socket, Imc, MIRRORFAILOVER_M2MEM_MAIN_REG);
        MirrorFailover.Bits.ddr4chnlfailed = Ddr4ChnlFailed;
        MirrorFailover.Bits.ddrtchnlfailed = DdrtChnlFailed;
        MemWritePciCfgEp (host, socket, Imc, MIRRORFAILOVER_M2MEM_MAIN_REG, MirrorFailover.Data);
      } // if (S3resume)

    } // Imc loop
  } // for (SadIndex)

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "</SetMirrorMode>\n"));
  return;
}

/**

  Configure Spare error threshold

  @param host                    - Pointer to sysHost
  @param socket                    - Socket number
  @param ch                      - Channel number (0-based)
  @param imcCORRERRTHRSHLD01Reg  -
  @param ErrThreshold            -

  @retval N/A

**/
static void
SetErrorThreshold (
                  PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch,
                  UINT32   imcCORRERRTHRSHLD01Reg,
                  UINT32   ErrThreshold
                  )
{
  UINT8                               rank;
  CORRERRTHRSHLD_0_MCDDC_DP_STRUCT  imcCORRERRTHRSHLD01;

  if (host->nvram.mem.socket[socket].channelList[ch].enabled == 1) {
    //
    // Set Error Threshold
    //
    for (rank = 0; rank < MAX_RANK_CH; rank += 2) {
      imcCORRERRTHRSHLD01.Data              = MemReadPciCfgEp (host, socket, ch, imcCORRERRTHRSHLD01Reg + (rank * 2));
      //
      // first rank
      //
      imcCORRERRTHRSHLD01.Bits.cor_err_th_0 = ErrThreshold;
      //
      // second rank
      //
      imcCORRERRTHRSHLD01.Bits.cor_err_th_1 = ErrThreshold;
      MemWritePciCfgEp (host, socket, ch, imcCORRERRTHRSHLD01Reg + (rank * 2), imcCORRERRTHRSHLD01.Data);
    }
  }

  return;
}

/**

  5370684: CLONE SKX Sighting: [SKX B0 PO] [ADDDC x Partial Mirroring] System sees CECC's when doing device sparing
  If Mirroring is enabled then all channels in the systems should have atleast 2 Ranks. Else adddc is nto supported

  @param host  - Pointer to sysHost

  @retval  SUCCESS - if the system supports ADDDC and Mirroring

**/
UINT32
CheckAdddcMirrorSupport(
  PSYSHOST host
 )
{
  UINT32 Status = SUCCESS;
  UINT8  Skt, ch;

 if ( ((host->nvram.mem.RASmodeEx & ADDDC_EN) != ADDDC_EN) || ((host->nvram.mem.RASmodeEx & SDDC_EN) != SDDC_EN) )
    return FAILURE;

  if (host->nvram.mem.RASmode & CH_ALL_MIRROR) {
    for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if (host->nvram.mem.socket[Skt].channelList[ch].enabled) {
          if (host->nvram.mem.socket[Skt].channelList[ch].numRanks < 2) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "\n System doesn't satisfy the ADDDC X Mirroring support \n"));
            host->nvram.mem.RASmodeEx &= ~(ADDDC_EN);
            host->nvram.mem.RASmodeEx &= ~(SDDC_EN);
            return FAILURE;
          }
        }
      }
    }
  }

  return Status;
}

/**

  Configure ADDDC X Mirror mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/


VOID
SetADDDCMirrorMode (
    PSYSHOST host,
    UINT8    socket
    )
{
  UINT8                                   iMc, ch, SktCh, TadIndex;
  UINT8                                   MirroredTadCount, MinMirTadRule, MaxMirTadRule, MinNoMirTadRule, MaxNoMirTadRule;
  UINT8                                   TadConfig[TAD_RULES];
  UINT8                                   Index1, Index2;
  SPARING_CONTROL_MC_MAIN_STRUCT          iMcSparingCtrl;
  AMAP_MC_MAIN_STRUCT                     iMcAMAPMain;
  TADBASE_0_MC_MAIN_STRUCT                imcTADBase;
  TADWAYNESS_0_MC_MAIN_STRUCT             iMcTADWayness;
  SPARING_CONTROL_TAD_MC_MAIN_STRUCT      iMcSparingControlTad;

  UINT32 TADBase[TAD_RULES] = {
    TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
    TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG
   } ;

  UINT32 TADWayness[TAD_RULES] = {
    TADWAYNESS_0_MC_MAIN_REG, TADWAYNESS_1_MC_MAIN_REG, TADWAYNESS_2_MC_MAIN_REG, TADWAYNESS_3_MC_MAIN_REG,
    TADWAYNESS_4_MC_MAIN_REG, TADWAYNESS_5_MC_MAIN_REG, TADWAYNESS_6_MC_MAIN_REG, TADWAYNESS_7_MC_MAIN_REG
  };


  if (CheckAdddcMirrorSupport(host) != SUCCESS)
    return;

  if(host->nvram.mem.RASmode & FULL_MIRROR_1LM) {

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   " Enabling ADDDC in Full Mirror mode\n"));

    for(iMc = 0; iMc < MAX_IMC; iMc++) {
      if(host->nvram.mem.socket[socket].imc[iMc].enabled == 0) continue;

      // Programming Sparing_Contorl register
      iMcSparingCtrl.Data = MemReadPciCfgMC (host, socket, iMc, SPARING_CONTROL_MC_MAIN_REG);
      iMcSparingCtrl.Bits.mirr_adddc_en = 1;
      MemWritePciCfgMC (host, socket, iMc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     " iMcSparingCtrl.Data after is 0x%x\n", iMcSparingCtrl.Data));

      // Programming AMAP register
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        SktCh = NODECH_TO_SKTCH(iMc,ch);
        if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;

        iMcAMAPMain.Data = MemReadPciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG);
        iMcAMAPMain.Bits.mirr_adddc_en = 1;
        MemWritePciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG, iMcAMAPMain.Data);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " iMcAMAPMain.Data after is 0x%x\n", iMcAMAPMain.Data));
      }// ch loop
    } // iMc for loop
  } // Full Mirroring loop

  if(host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM) { // remove the condition check for 2lm

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   " Enabling ADDDC in Partial Mirror mode\n"));

    for(iMc = 0; iMc < MAX_IMC; iMc++) {
      if(host->nvram.mem.socket[socket].imc[iMc].enabled == 0) continue;

      //Initialize the following variabls for each IMC.
      MirroredTadCount = 0;
      MinMirTadRule = 0xF;
      MinNoMirTadRule = 0xF;
      MaxMirTadRule = 0;
      MaxNoMirTadRule = 0;
      for(TadIndex=0; TadIndex < TAD_RULES; TadIndex++) {
        TadConfig[TadIndex] = 0;
      }

      //
      // Parse all TADS for each IMC, find out the which TAD is mirrored and which is not mirrored
      // Also get the minTadRule, MaxTadRule in Mirrored and NonMirrored case
      //

      for(TadIndex=0; TadIndex < TAD_RULES; TadIndex++) {

        iMcTADWayness.Data = MemReadPciCfgMC (host, socket, iMc, TADWayness[TadIndex]);

        // Tad is not enabled, if tad_limit is 0, Break out, since the next TADS will be not enabled as well
        if (iMcTADWayness.Bits.tad_limit == 0) {
          break;
        }

        // Read TadBase Register to check if the TAD is mirrored or not
        imcTADBase.Data = MemReadPciCfgMC (host, socket, iMc, TADBase[TadIndex]);
        if (imcTADBase.Bits.mirror_en == 1) {
          if (MinMirTadRule == 0xF) {
            MinMirTadRule=  TadIndex;
          }
          TadConfig[TadIndex] = TAD_MIRRORED;
          MirroredTadCount++;
          MaxMirTadRule = TadIndex;
        } else {
          if (MinNoMirTadRule == 0xF){
            MinNoMirTadRule=  TadIndex;
          }
          TadConfig[TadIndex] = TAD_NON_MIRRORED;
          MaxNoMirTadRule = TadIndex;
        }
      }

      for(TadIndex=0; TadIndex < TAD_RULES; TadIndex++) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " Tad 0x%x,  TadConfig[TadIndex] = 0x%x\n", TadIndex, TadConfig[TadIndex]));
      }

      // MirroredTadCount > 0 says that Partial mirroring is enabled in the system,  Program the registers.
      if (MirroredTadCount > 0) {

        // Programming Sparing_Contorl register
        iMcSparingCtrl.Data = MemReadPciCfgMC (host, socket, iMc, SPARING_CONTROL_MC_MAIN_REG);
        iMcSparingCtrl.Bits.mirr_adddc_en = 1;
        iMcSparingCtrl.Bits.partial_mirr_en = 1;
        iMcSparingCtrl.Bits.minimum_tad_rule = MinMirTadRule;
        iMcSparingCtrl.Bits.maximum_tad_rule = MaxMirTadRule;
        iMcSparingCtrl.Bits.minimum_tad_rule_nonmirr = MinNoMirTadRule;
        iMcSparingCtrl.Bits.maximum_tad_rule_nonmirr = MaxNoMirTadRule;
        MemWritePciCfgMC (host, socket, iMc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "iMcSparingCtrl.Data after is 0x%x\n", iMcSparingCtrl.Data));

        // Programming AMAP register
        for (ch = 0; ch < MAX_MC_CH; ch++) {
          SktCh = NODECH_TO_SKTCH(iMc,ch);
          if (host->nvram.mem.socket[socket].channelList[SktCh].enabled == 0) continue;
          iMcAMAPMain.Data = MemReadPciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG);
          iMcAMAPMain.Bits.mirr_adddc_en = 1;
          MemWritePciCfgEp (host, socket, SktCh, AMAP_MC_MAIN_REG, iMcAMAPMain.Data);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         " iMcAMAPMain.Data after is 0x%x\n", iMcAMAPMain.Data));
        }


        // Programming Sparing_Control_TAD register
        iMcSparingControlTad.Data = MemReadPciCfgMC (host, socket, iMc, SPARING_CONTROL_TAD_MC_MAIN_REG);

        for (Index1 = 0; Index1 < TAD_RULES; Index1++) {
          if (TadConfig[Index1] == 0) break;

          for (Index2 = Index1; Index2 < TAD_RULES; Index2++) {
            if (TadConfig[Index2] == 0) break;

            if (TadConfig[Index1] == TadConfig[Index2]) {
              switch (Index1) {
                case 0:
                  iMcSparingControlTad.Bits.nxt_tad_0 = Index2;
                  break;
                case 1:
                  iMcSparingControlTad.Bits.nxt_tad_1 = Index2;
                  break;
                case 2:
                  iMcSparingControlTad.Bits.nxt_tad_2 = Index2;
                  break;
                case 3:
                  iMcSparingControlTad.Bits.nxt_tad_3 = Index2;
                  break;
                case 4:
                  iMcSparingControlTad.Bits.nxt_tad_4 = Index2;
                  break;
                case 5:
                  iMcSparingControlTad.Bits.nxt_tad_5 = Index2;
                  break;
                case 6:
                  iMcSparingControlTad.Bits.nxt_tad_6 = Index2;
                  break;
              }

              if (Index1 != Index2) {
                break;
              }
            }
          }
        }

        MemWritePciCfgMC (host, socket, iMc, SPARING_CONTROL_TAD_MC_MAIN_REG, iMcSparingControlTad.Data);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " iMcSparingControlTad.Data after is 0x%x\n", iMcSparingControlTad.Data));
      }
    }
  }
}

/**

  Update RIR with new physical rank id for spare

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel
  @param oldSparePhysRank - original physical rank id
  @param newSparePhysRank - new physical rank id of spare

  @retval N/A

**/
static void
S3UpdateRIR (
            PSYSHOST host,
            UINT8    socket,
            UINT8    ch,
            UINT8    oldSparePhysRank,
            UINT8    newSparePhysRank
            )
{
  UINT8 rir;
  UINT8 rirway;
  struct ddrChannel *channelList;

  //
  // Search RIR interleave list and replace old physical rank id with new physical rank id
  //

  channelList = &host->var.mem.socket[socket].channelList[ch];

  //
  // For each RIR in the channel replace old rank target with new spare target in the interleave list
  // Note: the RIR offsets do not need to be updated
  //
  for (rir = 0; rir < MAX_RIR; rir++) {
    for (rirway = 0; rirway < MAX_RIR_WAYS; rirway++) {
      if (channelList->rirIntList[rir][rirway] == oldSparePhysRank) {
        channelList->rirIntList[rir][rirway] = newSparePhysRank;
      }
    }
  }

  WriteRIRForChannel (host, socket, ch);

  return;
}


/**

  Configure Sparing on S3 resume
  Note: S3 / Sparing is not POR and may not be fully validated

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static void
S3SpareResume (
              PSYSHOST host,
              UINT8    socket
              )
{
  return;
}


/**

  Configure Spare mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static void
SetSpareMode (
             PSYSHOST host,
             UINT8    socket
             )
{
  UINT8   ch;
  UINT8   rankIndex;
  UINT8   physicalRank;
  UINT8   logicalRank;
  UINT32  DimmAmap;
  UINT8   Dimm = 0;
  UINT8   RankPerDimm;
  UINT8   mcId;
  SMISPARECTL_MC_MAIN_STRUCT                imcSMISPARECTL;
  SPAREINTERVAL_MC_MAIN_STRUCT              imcSPAREINTERVAL;
  SPARECTL_MC_MAIN_STRUCT                   imcSpareCtlReg;
  SPARING_MCDDC_CTL_STRUCT                  imcSparingReg;
  struct channelNvram                       *channelNvList;
  AMAP_MC_MAIN_STRUCT                       AMAPReg;
  SCRATCHPAD2_MC_MAIN_STRUCT                ChScratchPad2;
  TIMEOUT_M2MEM_MAIN_STRUCT                 m2mTimeout;
  QPI_TIMEOUT_CTRL_CHABC_SAD_STRUCT         KtiTorTimeout;
  LINK_CFG_READ_1_MCDDC_DP_STRUCT           linkCfgRead;
  UINT32                                    TimeoutLockVal;
  //
  // Configure Spare mode
  // Note: spare capability bit checked in checkSupportedRASModes() function
  //
  if ((host->nvram.mem.RASmode & RK_SPARE) != RK_SPARE) return;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n<SetSpareMode(socket = %d)>\n", socket));

  //
  // Program iMC for sparing
  //
  imcSMISPARECTL.Data                 = MemReadPciCfgMain (host, socket, SMISPARECTL_MC_MAIN_REG);
  imcSMISPARECTL.Bits.intrpt_sel_cmci = 0;
  imcSMISPARECTL.Bits.intrpt_sel_smi  = 0;
  imcSMISPARECTL.Bits.intrpt_sel_pin  = 0;
  MemWritePciCfgMain (host, socket, SMISPARECTL_MC_MAIN_REG, imcSMISPARECTL.Data);

  for (ch = 0; ch < MAX_CH; ch++) {
    imcSparingReg.Data = MemReadPciCfgEp (host, socket, ch, SPARING_MCDDC_CTL_REG);
    imcSparingReg.Bits.sparecrdts = 0x0;
    imcSparingReg.Bits.wrfifohwm = 0x0; // HSD 4929575
    MemWritePciCfgEp (host, socket, ch, SPARING_MCDDC_CTL_REG, imcSparingReg.Data);
    imcSparingReg.Data = MemReadPciCfgEp (host, socket, ch, SPARING_MCDDC_CTL_REG);
  } // ch loop

  imcSpareCtlReg.Data = MemReadPciCfgMain (host, socket, SPARECTL_MC_MAIN_REG);
  imcSpareCtlReg.Bits.hafifowm = 0x02;
  imcSpareCtlReg.Bits.diswpqwm = 0x01;
  MemWritePciCfgMain (host, socket, SPARECTL_MC_MAIN_REG, imcSpareCtlReg.Data);

  //
  // Set spare interval - to recommended value (from imc design)
  //
  imcSPAREINTERVAL.Data           = MemReadPciCfgMain (host, socket, SPAREINTERVAL_MC_MAIN_REG);
  imcSPAREINTERVAL.Bits.normopdur = host->setup.mem.normOppIntvl;
  MemWritePciCfgMain (host, socket, SPAREINTERVAL_MC_MAIN_REG, imcSPAREINTERVAL.Data);

  if (host->var.common.bootMode == S3Resume) {
    S3SpareResume (host, socket);
  }

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Sparing configured\n"));

  if ((host->nvram.mem.RASmodeEx & PTRLSCRB_EN) ) {
    for (ch = 0; ch < MAX_CH; ch++) {
      channelNvList = &(*GetChannelNvList(host, socket))[ch];
      if (channelNvList->enabled == 0) continue;

      // exclude spare ranks from patrol scrub
      DimmAmap = 0;
      for (rankIndex = 0; rankIndex <  MAX_SPARE_RANK; rankIndex ++) {
        physicalRank = channelNvList->sparePhysicalRank[rankIndex];
        if (physicalRank == 0xff) continue;
        Dimm = physicalRank / 4;
        RankPerDimm  = physicalRank % 4;
        DimmAmap |= (1 << (Dimm * 8 + RankPerDimm));
      } // rankIndex loop

      MemDebugPrint ((host, SDBG_MAX, socket, ch, Dimm, NO_RANK, NO_STROBE, NO_BIT,
            "DimmAmap: 0x%x\n", DimmAmap));

      // update the AMAP
      AMAPReg.Data = MemReadPciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG);
      AMAPReg.Bits.dimm0_pat_rank_disable |= (UINT8)(DimmAmap & 0xff);
      AMAPReg.Bits.dimm1_pat_rank_disable |= (UINT8)((DimmAmap >> 8) & 0xff);
      MemWritePciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG,AMAPReg.Data);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n<ch=%d, AMAP.Data=0x%x>\n", ch, AMAPReg.Data));
    }
  }

  //
  // update scratch pad register
  // SCRATCHPAD2 in each DDR channel
  // bits 0-7     one-hot encoding of channel's spare ranks, each bit position represents a logical rank id
  // bits 8       sparing enabled
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    channelNvList = &(*GetChannelNvList(host, socket))[ch];
    if (channelNvList->enabled == 0) continue;
      ChScratchPad2.Data = MemReadPciCfgEp (host, socket, ch, SCRATCHPAD2_MC_MAIN_REG);
      ChScratchPad2.Data &= 0xFFFFFF00; // clear LSB
      for (rankIndex = 0; rankIndex < MAX_SPARE_RANK; rankIndex ++) {
        logicalRank = channelNvList->spareLogicalRank[rankIndex];
        if (logicalRank == 0xff)continue;
        ChScratchPad2.Data |= (1 << logicalRank);
      } // rankIndex loop
      // if spare ranks available, set enable flag
      if (ChScratchPad2.Data & 0xFF) ChScratchPad2.Data |= (1 << MAX_RANK_CH);
      MemWritePciCfgEp(host, socket, ch, SCRATCHPAD2_MC_MAIN_REG, ChScratchPad2.Data);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n<ch=%d, SCRATCHPAD2.Data=0x%x>\n", ch, ChScratchPad2.Data));
      //
      //SKX Bug ECO 305877 - Spare window can be violated if SSA FSM cannot match on a spare address potentially causing timeouts
      //
      if (ChScratchPad2.Data & 0xFF) {
        linkCfgRead.Data = MemReadPciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG);
        if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
          linkCfgRead.Bits.read_data |= BIT7;
        } else {
          linkCfgRead.Bits.read_data &= ~BIT7;
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "<ch=%d, linkCfgRead.Data=0x%x>\n", ch, linkCfgRead.Data));
        MemWritePciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG, linkCfgRead.Data);
      }
    } // ch loop

  //
  //s5371826 : M2M TO and TOR TO not disabled when rank sparing enabled on SKX B1
  //
  if( !( CheckSteppingGreaterThan(host, CPU_SKX, B1_REV_SKX)) ) {

    //
    //s4930086, disable m2mem TO for A&B stepping with rank sparing enable.
    //
    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
        m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
        TimeoutLockVal = m2mTimeout.Bits.timeoutlock;
        m2mTimeout.Bits.timeoutlock = 0;
        MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

        m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
        m2mTimeout.Bits.timeouten = 0;
        MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

        m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
        m2mTimeout.Bits.timeoutlock = TimeoutLockVal;
        MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);
    }

    //
    //s5332944, disable TOR TO for A & B stepping.
    //
    KtiTorTimeout.Data = ReadCpuPciCfgEx (host, socket, 0, QPI_TIMEOUT_CTRL_CHABC_SAD_REG);
    KtiTorTimeout.Bits.enabletortimeout = 0;
    WriteCpuPciCfgEx (host, socket, 0, QPI_TIMEOUT_CTRL_CHABC_SAD_REG, KtiTorTimeout.Data);

  }
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n</SetSpareMode>\n"));

  return ;
} // SetSpareMode

/**

  Configure SDDC plus one

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static void
SetSddcPlusOne (
                 PSYSHOST host,
                 UINT8    socket
                 )
{
  UINT8   ch;
  UINT8   dimm;
  UINT8   rank;
  UINT8   numRanks;

  //
  // Configure SDDC plus one, if on S3 RESUME path
  //
  if (((host->nvram.mem.RASmodeEx & SDDC_EN) == SDDC_EN) &&
      (host->var.common.bootMode == S3Resume)) {
    //
    // Program iMC for Sddc plus one
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmPresent == 0) continue;

        numRanks = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numRanks;

        for (rank = 0; rank < numRanks; rank++) {
          //SKX_TODO: Implement This function if required for Work Station

        } // rank
      } // dimm
    } // ch
  } // if S3
  return;
}

/**

  Program corrected error thresholds for all channels in socket

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static VOID
SetErrorThresholdNode (
                      PSYSHOST host,
                      UINT8    socket
                      )
{
  UINT8                 ch;
  UINT32                ErrThreshold;

  //
  // Set Error Threshold
  //
  ErrThreshold = host->setup.mem.spareErrTh;

  if (ErrThreshold != 0) {
    for (ch = 0; ch < MAX_CH; ch++) {
      SetErrorThreshold (host, socket, ch, CORRERRTHRSHLD_0_MCDDC_DP_REG, ErrThreshold);
    }
  }
  return;
}

static const UINT8 timingTable[MAX_SUP_FREQ] = {20, 17, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 2, 2, 2, 2, 1, 1};

/**

  Configure Patrol Scrubbing

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static void
SetPatrolScrub (
    PSYSHOST host,
    UINT8    socket
)
{
  UINT8                             ch, chIndex;
  UINT8                             mcId;
  UINT8                             TADIndex;
  UINT8                             SadIndex;
  UINT8                             MaxTadIndex;
  UINT8                             scrubch_mask;
  UINT16                            memSize;
  UINT32                            scrubInterval;
  UINT32                            scrubFactor;
  //UINT8                             mcRdCredits;
  UINT8                             mcWrCredits;
  UINT8                             chNum;
  UINT8                             tempCh;
  UINT32                            Offset;
  SCRUBCTL_MC_MAIN_STRUCT           imcSCRUBCTL;
  SCRUBMASK_MC_MAIN_STRUCT          imcSCRUBMASK;
  SCRUBADDRESSHI_MC_MAIN_STRUCT     scrubAddrHi;
  DEFEATURES0_M2MEM_MAIN_STRUCT     imcDEFEATURES;
  MC0_DP_CHKN_BIT_MCDDC_DP_STRUCT   dpCkhnBit;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT   mcMainChknBits;
  CREDITS_M2MEM_MAIN_STRUCT         creditsM2mem;
  LINK_MCA_CTL_MCDDC_DP_STRUCT      mcaCtl;

  struct channelNvram               (*channelNvList)[MAX_CH];
  struct ddrChannel                 (*channelList)[MAX_CH];
  struct SADTable                   *SADEntry;
  UINT32 PcodeMailboxData = 0;
  UINT32 PcodeMailboxStatus = 0;

  channelNvList = GetChannelNvList(host, socket);

  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    mcMainChknBits.Data = MemReadPciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG);
    mcMainChknBits.Bits.frc_spr_end = 1;
    MemWritePciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcMainChknBits.Data);
  }

  //
  // Default values
  //
  //mcRdCredits = 48;
  mcWrCredits = 40;

  //
  // Enable patrol scrubs
  // Note: Patrol scrub capability bit checked in checkSupportedRASModes() function
  //
  if ((host->nvram.mem.RASmodeEx & PTRLSCRB_EN) && (host->var.mem.socket[socket].memSize != 0)) {

    //
    // Save one write credit for patrol scrub
    //
    mcWrCredits -= 1;

    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

      if (host->var.common.bootMode == S3Resume) {
        MemWritePciCfgMC (host, socket, mcId, SCRUBMASK_MC_MAIN_REG, host->nvram.mem.socket[socket].imc[mcId].scrubMask);

        MemWritePciCfgMC (host, socket, mcId, SCRUBMASK2_MC_MAIN_REG, host->nvram.mem.socket[socket].imc[mcId].scrubMask2);

      }

      //
      // Equation to get a scrub to every line in 24 hours is..
      // (86400/(MEMORY CAPACITY/64))/CYCLE TIME of DCLK
      // Seconds it takes to scrub all of memory (86400 seconds in a day)
      //
      if ((host->setup.mem.patrolScrubDuration != 0) && (host->setup.mem.patrolScrubDuration <= PATROL_SCRUB_DURATION_MAX)) {
        scrubFactor = 3600 * host->setup.mem.patrolScrubDuration;
      } else {
        scrubFactor = 86400;
      }

      // Program System Physical Address (or) Reverse Address mode based on BIOS setup option
      scrubAddrHi.Data = MemReadPciCfgMC (host, socket, mcId, SCRUBADDRESSHI_MC_MAIN_REG);
      scrubAddrHi.Bits.ptl_sa_mode = host->setup.mem.patrolScrubAddrMode;
      scrubAddrHi.Bits.minimum_tad_rule = 0;

      MaxTadIndex = 0;
      for(TADIndex = 0; TADIndex < TAD_RULES; TADIndex ++){
        if(host->var.mem.socket[socket].imc[mcId].TAD[TADIndex].Enable == 0){
            break;
        }

        SadIndex = host->var.mem.socket[socket].imc[mcId].TAD[TADIndex].SADId;
        SADEntry = &host->var.mem.socket[socket].SAD[SadIndex];

        if ((SADEntry->type == MEM_TYPE_1LM) || (SADEntry->type == MEM_TYPE_2LM)) {
          MaxTadIndex++;
        }
      }
      scrubAddrHi.Bits.maximum_tad_rule = MaxTadIndex - 1;

      MemWritePciCfgMC (host, socket, mcId, SCRUBADDRESSHI_MC_MAIN_REG, scrubAddrHi.Data);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Socket[%d] Imc[%d] - scrubAddrHi.Bits.ptl_sa_mode = %d, scrubAddrHi.Bits.maximum_tad_rule = %d\n",
                     socket, mcId, scrubAddrHi.Bits.ptl_sa_mode, scrubAddrHi.Bits.maximum_tad_rule));

      channelList = &host->var.mem.socket[socket].channelList;
      memSize = 0;
      scrubch_mask = 0;
      for (chIndex = 0; chIndex < MAX_MC_CH; chIndex++) {
        ch = NODECH_TO_SKTCH(mcId,chIndex);
        if ((*channelNvList)[ch].enabled == 0)
        {
          //WA 5332430: CLONE SKX Sighting: Patrol scrub hang w/ 2-way MC interleaving and empty mc0 ch0
          if (host->var.common.stepping < B0_REV_SKX)  {
            imcSCRUBMASK.Data = MemReadPciCfgMC (host, socket, mcId, SCRUBMASK_MC_MAIN_REG);
            scrubch_mask |= (1<<chIndex);
            imcSCRUBMASK.Bits.ch_mask |= scrubch_mask;
            MemWritePciCfgMC (host, socket, mcId, SCRUBMASK_MC_MAIN_REG, imcSCRUBMASK.Data);
          }

          //End WA 5332430
          continue;
        }

        if ((*channelList)[ch].mcId == mcId) {
          memSize += (*channelList)[ch].memSize;
        }
      } // chIndex loop


      if (memSize) {
        //
        // Calculate the interval value to be programmed for Patrol Scrub.
        //
        //scrubInterval = (scrubFactor:86400 / memCacheLineSize) * (RCLK_FREQ:100000000 / PCUDATA_CLOCK_UNITS:64);
        //    =  scrubFactor:86400/ (MemoryByteSize/64)   * (RCLK_FREQ:100000000 / PCUDATA_CLOCK_UNITS:64);
        //    =  scrubFactor:86400/memSize  * (100000000/64MB)
        //    ~  scrubFactor:86400/memSize  * 1.49

        scrubInterval = (scrubFactor * 149)/((UINT32)memSize * 100) ;

        //
        //PCU interval has 8 bit width.
        //
        if(scrubInterval > 0xff){
          scrubInterval = 0xff;
        }


        //
        //if memSize is too big, the value will round to 0, under such case, set to 1.
        //
        if(scrubInterval == 0){
          scrubInterval = 1;
        }


      } else {
        scrubInterval = 0;
      }

      PcodeMailboxData = mcId; // BIT0 MC_INDEX (0 or 1)
      PcodeMailboxData |= scrubInterval << 8;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) {
        ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      }

      //
      // Enable scrub here only for non S3 resume. For S3 resume the setting is restored by S3BootScript
      //
      if (host->var.common.bootMode != S3Resume) {
        imcSCRUBCTL.Data = MemReadPciCfgMC (host, socket, mcId, SCRUBCTL_MC_MAIN_REG);
        imcSCRUBCTL.Bits.scrub_en       = 1;
        imcSCRUBCTL.Bits.scrubinterval  = scrubInterval;

        MemWritePciCfgMC (host, socket, mcId, SCRUBCTL_MC_MAIN_REG, imcSCRUBCTL.Data);
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Patrol scrub enabled and started, interval value:%x\n", scrubInterval));
      }
    } // mcId loop
  } // if patrol scrubbing enabled

  if ((host->var.common.stepping >= B0_REV_SKX) && (host->var.mem.RASModesEx & PTRLSCRB_EN)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      mcaCtl.Data = MemReadPciCfgEp(host, socket, ch, LINK_MCA_CTL_MCDDC_DP_REG);
      mcaCtl.Bits.ptl_sa_mode = host->setup.mem.patrolScrubAddrMode;
      MemWritePciCfgEp(host, socket, ch, LINK_MCA_CTL_MCDDC_DP_REG, mcaCtl.Data);
    }
  }


  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      chNum = mcId * MAX_MC_CH + ch;
      if ((*channelNvList)[chNum].enabled == 0) continue;
      creditsM2mem.Data = MemReadPciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG);
      creditsM2mem.Bits.crdttype = 0x1 | (ch << 4);
      creditsM2mem.Bits.crdtwren = 0;
      creditsM2mem.Bits.crdtcnt = mcWrCredits;
      MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      creditsM2mem.Bits.crdtwren = 1;
      MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      creditsM2mem.Bits.crdtwren = 0;
      MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      //Leave RdCredits default to 48 for now.
      //creditsM2mem.Bits.crdttype = 0x0 | (ch << 4);
      //creditsM2mem.Bits.crdtwren = 0;
      //creditsM2mem.Bits.crdtcnt = mcRdCredits;
      //MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      //creditsM2mem.Bits.crdtwren = 1;
      //MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      //creditsM2mem.Bits.crdtwren = 0;
      //MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
    }
  }
  //
  // Enable/Disable Demand Scrubs
  // Note: Hardware default is demand scrub enabled
  //
  if (host->nvram.mem.RASmodeEx & DMNDSCRB_EN) {
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Demand scrub enabled\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Demand scrub disabled\n"));
  }

  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    imcDEFEATURES.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG);

    // Demand Scrub Disable  shall not be used according to b311409
    /*
    if (host->nvram.mem.RASmodeEx & DMNDSCRB_EN) {
      imcDEFEATURES.Bits.demandscrubwrdis = 0;
    } else {
      imcDEFEATURES.Bits.demandscrubwrdis = 1;
    }
    */

    //
    // Scrub Check read is only enabled when system has mirroring
    //
    if ((host->nvram.mem.RASmode & CH_ALL_MIRROR)) {
      imcDEFEATURES.Bits.scrubcheckrddis = 0;
    } else {
      imcDEFEATURES.Bits.scrubcheckrddis = 1;
    }
    MemWritePciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG, imcDEFEATURES.Data);
  } // mcId

  //
  // Convert uncorrectable patrol scrub errors to correctable if mirror is enabled
  //
  if ((  (host->nvram.mem.RASmode & FULL_MIRROR_1LM) == FULL_MIRROR_1LM ||
         (host->nvram.mem.RASmode & FULL_MIRROR_2LM) == FULL_MIRROR_2LM ) &&
      (host->nvram.mem.RASmodeEx & PTRLSCRB_EN)  ) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        tempCh = ch;
        Offset = MC0_DP_CHKN_BIT_MCDDC_DP_REG;
        if (ch & BIT0) {
          Offset = Offset + 4;
          tempCh = (ch & BIT1);
        }
        dpCkhnBit.Data = MemReadPciCfgEp (host, socket, tempCh, Offset);
        dpCkhnBit.Bits.ign_ptrl_uc = 1;
        MemWritePciCfgEp (host, socket, tempCh, Offset, dpCkhnBit.Data);
      } // ch loop
  } // if mirroring and patrol scrubbing enabled
}

/**

  Configure Leaky Bucket

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static VOID
SetLeakyBucketConfig (
                     PSYSHOST host,
                     UINT8    socket
                     )
{
  LEAKY_BUCKET_CFG_MC_MAIN_STRUCT     imcLEAKY_BUCKET_CFG;

  //
  // Program leaky bucket rate
  // Set to a rate of about 1 leaky bucket pulse per 10 days at a dclk rate of 1GHz
  //
  imcLEAKY_BUCKET_CFG.Data                  = MemReadPciCfgMain (host, socket, LEAKY_BUCKET_CFG_MC_MAIN_REG);
  imcLEAKY_BUCKET_CFG.Bits.leaky_bkt_cfg_hi = host->setup.mem.leakyBktHi;
  imcLEAKY_BUCKET_CFG.Bits.leaky_bkt_cfg_lo = host->setup.mem.leakyBktLo;
  MemWritePciCfgMain (host, socket, LEAKY_BUCKET_CFG_MC_MAIN_REG, imcLEAKY_BUCKET_CFG.Data);

  return;
}

/**
  Disable DDR4 and DDRT parity for A0 stepping.
  See sigthing 4929966: Cloned From SKX Si Bug Eco: DDRTRdViralDnnnH is not qualified with DDRT mode and 5330730: SKX Sighting: Machine Check on windows boot with ucode w/a for iMC parity bug
  @param host - Pointer to sysHost
  @param socket  - Socket number

  @retval VOID
**/
static VOID
DisableParityCheck(
              PSYSHOST host,
              UINT8    socket
              )
{
  UINT8   ch;
  struct channelNvram (*channelNvList)[MAX_CH];
  MC0_DP_CHKN_BIT_MCDDC_DP_STRUCT         MC0DpChknBit;
  DDRT_WDB_PAR_ERR_CTL_MC_2LM_STRUCT      WdbParErrCtl;

  channelNvList = &host->nvram.mem.socket[socket].channelList;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0)
      continue;

    MC0DpChknBit.Data = MemReadPciCfgEp(host, socket, ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG);
    MC0DpChknBit.Bits.dis_ha_par_chk = 1;
    MemWritePciCfgEp(host, socket, ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG, MC0DpChknBit.Data);

    //Read back to confirm setting
    MC0DpChknBit.Data = MemReadPciCfgEp(host, socket, ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG);
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "DisableParityCheck: Socket = %d ch = %d MC0DpChknBit = %x\n", socket, ch, MC0DpChknBit.Data));
    WdbParErrCtl.Data = MemReadPciCfgEp(host, socket, ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG);
    WdbParErrCtl.Bits.dis_ha_par_chk = 1;
    MemWritePciCfgEp(host, socket, ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG, WdbParErrCtl.Data);

    WdbParErrCtl.Data = MemReadPciCfgEp(host, socket, ch, DDRT_WDB_PAR_ERR_CTL_MC_2LM_REG);
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "DisableParityCheck: Socket = %d ch = %d WdbParErrCtl = %x\n", socket, ch, WdbParErrCtl.Data));
  }
}

/**

  Enable RAS modes
  NOTE: Default settings are unchanged if feature not explicitly enabled by BIOS

  @param host - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
SetRASConfig (
             PSYSHOST host
             )
{
  UINT8   socket;
  UINT8   mcId;
  UINT8   ch;
  UINT8   i;
  ECC_MODE_RANK0_MC_MAINEXT_STRUCT    EccModeMcDecs;
  ECC_MODE_RANK0_MC_MAIN_STRUCT       EccModeMcMain;
  UINT32                              EccModeReg[]  = ECC_MODE_REG_LIST;
  UINT32                              EccModeRegExt[] = ECC_MODE_REG_EXT_LIST;



  struct channelNvram (*channelNvList)[MAX_CH];


#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  SetMemPhase(host, RAS_CONFIG);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Set RAS Config\n"));

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) {
      continue;
    }

    OutputExtendedCheckpoint((host, STS_RAS_CONFIG, SUB_MIRROR_MODE, socket));
    SetMirrorMode (host, socket);
    OutputExtendedCheckpoint((host, STS_RAS_CONFIG, SUB_SPARE_MODE, socket));
    SetSpareMode (host, socket);
    OutputExtendedCheckpoint((host, STS_RAS_CONFIG, SUB_DEVICE_TAG, socket));
    SetSddcPlusOne (host, socket);
    OutputExtendedCheckpoint((host, STS_RAS_CONFIG, SUB_ERR_THRESH, socket));
    SetErrorThresholdNode (host, socket);
    OutputExtendedCheckpoint((host, STS_RAS_CONFIG, SUB_ADDDC_MIRROR, socket));
    SetADDDCMirrorMode (host, socket);



    if ((host->nvram.mem.RASmode & CH_ML) == CH_INDEPENDENT) {

      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Independent ch mode enabled\n"));
    }

    SetPatrolScrub (host, socket);

    //
    // Set leaky bucket config to non-zero, per architect request
    //
    SetLeakyBucketConfig (host, socket);

    //A0 Si WA 4929966: Cloned From SKX Si Bug Eco: DDRTRdViralDnnnH is not qualified with DDRT mode
    //A0 WA 5330730: SKX Sighting: Machine Check on windows boot with ucode w/a for iMC parity bug
    if (host->var.common.stepping < B0_REV_SKX)  {
      DisableParityCheck(host, socket);
    }

    channelNvList = GetChannelNvList(host, socket);
    if ((host->var.common.bootMode == S3Resume) && (host->var.mem.subBootMode != AdrResume)){
      //
      // during S3 resume, restore corrected error counters/thresholds/signalmode
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        for (i = 0; i < MAX_RANK_CH/2; i++) {
          MemWritePciCfgEp (host, socket, ch, CORRERRCNT_0_MCDDC_DP_REG + (i*4), host->nvram.mem.socket[socket].channelList[ch].rankErrCountInfo[i]);
          MemWritePciCfgEp (host, socket, ch, CORRERRTHRSHLD_0_MCDDC_DP_REG + (i*4), host->nvram.mem.socket[socket].channelList[ch].rankErrThresholdInfo[i]);
        }
        for (i = 0; i < MAX_RANK_CH ; i++) {
          // Save Plus 1 data
          MemWritePciCfgEp (host, socket, ch, PLUS1_RANK0_MCDDC_DP_REG+ i, host->nvram.mem.socket[socket].channelList[ch].Plus1Failover[i]);

          //Save ECC modes
          EccModeMcDecs.Bits.mode =  host->nvram.mem.socket[socket].channelList[ch].EccModeMcDecs[i];
          EccModeMcMain.Bits.mode =  host->nvram.mem.socket[socket].channelList[ch].EccModeMcMain[i];

          MemWritePciCfgEp (host, socket, ch, EccModeRegExt[i],(UINT32)EccModeMcDecs.Data );
          MemWritePciCfgEp (host, socket, ch, EccModeReg[i],(UINT32)EccModeMcMain.Data );

        } // for (i)

        MemWritePciCfgEp (host, socket, ch, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG, host->nvram.mem.socket[socket].channelList[ch].LinkRetryErrLimits);
        MemWritePciCfgEp (host, socket, ch, LINK_LINK_FAIL_MCDDC_DP_REG, host->nvram.mem.socket[socket].channelList[ch].LinkLinkFail);
        //imc0_c1_imc0_mc_ctl2
      } // for (ch)

      WriteCpuPciCfgEx (host, socket, 0, SMICTRL_UBOX_MISC_REG, host->nvram.mem.socket[socket].smiCtrlUboxMisc);

      for (mcId = 0; mcId < MAX_IMC; mcId++) {
        // Skip inactive IMC
        if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

        MemWritePciCfgMC (host, socket, mcId, EMCALTCTL_MC_MAIN_REG, host->nvram.mem.socket[socket].imc[mcId].EmcaLtCtlMcMainExt);
        MemWritePciCfgMC (host, socket, mcId, EXRAS_CONFIG_M2MEM_MAIN_REG, host->nvram.mem.socket[socket].imc[mcId].ExRasConfigHaCfg);
        MemWritePciCfgMC (host, socket, mcId, SMISPARECTL_MC_MAIN_REG, host->nvram.mem.socket[socket].imc[mcId].SmiSpareCtlMcMainExt);
      } // mcId
    }
  } // socket loop

  if (host->nvram.mem.eccEn != 0) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "ECC is enabled\n"));
  }

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  ClearMemPhase(host, RAS_CONFIG);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS
  return SUCCESS;
}

/**

  Check if channel supports sparing by checking if occupied channel has a spare rank

  @param host    - Pointer to sysHost


  @retval RAS_IS_CAPABLE or RAS_NOT_CAPABLE

**/
static UINT8
CheckSparingPopulation (
    PSYSHOST host
)
{
  UINT8                 socket;
  UINT8                 channel;
  UINT8                 dimm;
  UINT8                 rank;
  UINT8                 capable;
  UINT8                 numranks;
  UINT8                 numdimms;
  struct dimmNvram (*dimmNvList0)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];


  //
  //HSD:5332666 change spare enable to channel level.
  //
  capable = RAS_NOT_CAPABLE;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    // Skip disabled sockets
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0))
      continue;

    channelNvList = GetChannelNvList(host, socket);

    for (channel=0; channel<MAX_CH; channel++) {
      if ((*channelNvList)[channel].enabled == 0) continue;
      dimmNvList0 = GetDimmNvList(host, socket, channel);
      numranks    = 0;
      numdimms    = 0;

      // Count number of ranks in channel
      for (dimm = 0; dimm < (*channelNvList)[channel].maxDimm; dimm++) {
        if (((*dimmNvList0)[dimm].dimmPresent) == 0) continue;

        // SKX IMC Rank Sparing is only for DDR4 Ranks, Skip NVM DIMMs
        if (((*dimmNvList0)[dimm].aepDimmPresent) == 1) continue;

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (CheckRank(host, socket, channel, dimm, rank, CHECK_MAPOUT)) continue;
          numranks++;
        } // rank loop
        if (numranks) numdimms++;
      } // dimm loop

      // Rank sparing possible only if there are two or more DDR4 ranks
  //    if ((numdimms == 1) && (numranks < 2)) {
  //      capable = RAS_NOT_CAPABLE;
  //      break;
  //    }

     if((numdimms == 2) || (numranks >= 2)){
        capable = RAS_IS_CAPABLE;
        break;
      }

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\nCheckSparingPopulation (Socket = %d, ch = %d) - ", socket, channel));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     (capable == RAS_IS_CAPABLE)? "Yes\n" : "No\n" ));
    } // for (channel)

    if(capable == RAS_IS_CAPABLE){
      break;
    }
  }
  return capable;
}

/**

  Determine if RAS features are supportabled given current memory configuration
  and user input

  @param host  - Pointer to sysHost

  @retval RAS_IS_CAPABLE or RAS_NOT_CAPABLE

**/
static UINT8
checkSupportedRASModes (
                       PSYSHOST host
                       )
{
  UINT8                     socket;
  UINT8                     mode;
  CAPID1_PCU_FUN3_STRUCT    pcuCAPID1;
  CAPID3_PCU_FUN3_STRUCT    pcuCAPID3;
  CAPID5_PCU_FUN3_STRUCT    pcuCAPID5;
  UINT8 imc;
  UINT8 channel;
  UINT8 dimm;
  UINT8 imcCount = 0;
  UINT8 channelCount = 0;
  BOOLEAN exactly2channels = 0;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n<checkSupportedRASModes>\n"));

  //
  // Assume max capability and reduce when cannot support
  //
  mode = FULL_MIRROR_1LM | FULL_MIRROR_2LM | STAT_VIRT_LOCKSTEP | RK_SPARE | PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM;

  if ( (host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot) ) {
    host->var.mem.RASModesEx = PTRLSCRB_EN | DMNDSCRB_EN | SDDC_EN | ADDDC_EN | ADDDC_ERR_INJ_EN;
  } else {
    host->var.mem.RASModesEx = host->nvram.mem.RASmodeEx;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
           "RASModesEx = 0x%x \n", host->nvram.mem.RASmodeEx));
  }

  //
  // Check for correctly configured DIMMs on each socket
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    // Skip disabled sockets
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0))
      continue;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "<Socket[%d]>\n", socket));
    //
    // Mirror capability/population check
    //

    pcuCAPID1.Data = ReadCpuPciCfgEx(host, socket, 0, CAPID1_PCU_FUN3_REG);
    pcuCAPID5.Data = ReadCpuPciCfgEx(host, socket, 0, CAPID5_PCU_FUN3_REG);

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t<CAPID1_PCU_FUN3_REG value for socket %d =0x%x>\n",socket, pcuCAPID1.Data));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t<CAPID5_PCU_FUN3 value for socket %d =0x%x>\n",socket, pcuCAPID5.Data));

    //Verify the socket populated has the capabilities for full mirroring
    if (pcuCAPID1.Bits.disable_mc_mirror_mode == 0) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\t<pcuCAPID5.Bits.cap_mirror_ddr4_en=%d>\n", pcuCAPID5.Bits.cap_mirror_ddr4_en));
      if (pcuCAPID5.Bits.cap_mirror_ddr4_en == 0) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\tSocket %d Full Mirror cap 1LM not available\n", socket));
        mode &= ~FULL_MIRROR_1LM;
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\t<pcuCAPID5.Bits.cap_mirror_ddrt_en=%d>\n", pcuCAPID5.Bits.cap_mirror_ddrt_en));
      if (pcuCAPID5.Bits.cap_mirror_ddrt_en == 0) {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\tSocket %d Full Mirror cap 2LM not available\n", socket));
        mode &= ~FULL_MIRROR_2LM;
      }
    }
    else {
      mode &= ~(FULL_MIRROR_1LM | FULL_MIRROR_2LM);
    }

    //Verify the socket populated has the capabilities for address based mirroring
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t<CAPID5_PCU_FUN3.addr_based_mem_mirror=%d>\n", pcuCAPID5.Bits.addr_based_mem_mirror));
    if (pcuCAPID5.Bits.addr_based_mem_mirror == 1) {
      if (pcuCAPID5.Bits.cap_mirror_ddr4_en == 0) {
         mode &= ~(PARTIAL_MIRROR_1LM);
      }
      if (pcuCAPID5.Bits.cap_mirror_ddrt_en == 0) {
         mode &= ~(PARTIAL_MIRROR_2LM);
      }
    }
    else {
      mode &= ~(PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM);
    }

    //Static Virtual Lockstep capability population check

    if ( checkVLSpopulation (host, socket, STATIC_VLS) == RAS_NOT_CAPABLE ) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\t Disable sVLS DIMM Config BAD \n"));
      mode &= ~STAT_VIRT_LOCKSTEP;
    }


    //
    // Rank Sparing capability/population check
    //
    pcuCAPID3.Data = ReadCpuPciCfgEx(host, 0, 0, CAPID3_PCU_FUN3_REG);
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t<CAPID3_PCU_FUN3.disable_sparing=%d>\n", pcuCAPID3.Bits.disable_sparing));
    // Processor populated does not support rank sparing?
    if ((pcuCAPID3.Bits.disable_sparing == 1)) {
      mode &= ~RK_SPARE;
    } else if (CheckSparingPopulation (host) == RAS_NOT_CAPABLE) {
      mode &= ~RK_SPARE;
    }

    //
    // Disable patrol scrub if any socket does not support patrol scrub
    // Check patrol scrub capability bit from PCU
    //
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t<CAPID3_PCU_FUN3.disable_patrol_scrub=%d>\n", pcuCAPID3.Bits.disable_patrol_scrub));
    if (pcuCAPID3.Bits.disable_patrol_scrub == 1) {
      host->var.mem.RASModesEx &= ~PTRLSCRB_EN;
    }

    //
    // Disable patrol scrub & Disable demand scrub if ECC is not supported
    //
    if (host->nvram.mem.eccEn == 0) {
      host->var.mem.RASModesEx &= ~PTRLSCRB_EN;
      host->var.mem.RASModesEx &= ~DMNDSCRB_EN;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "</Socket[%d]>\n", socket));
  } // socket loop

  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    if(host->setup.kti.XptPrefetchEn == KTI_ENABLE) {
      for(socket = 0; socket < MAX_SOCKET; socket++) {
        for(imc = 0; imc < MAX_IMC; imc++) {
          if(host->var.mem.socket[socket].imcEnabled[imc] == 1) {
            //more than one IMC
            imcCount++;
          }
        }
        channelCount = 0;
        for(channel = 0; channel < MAX_CH; channel++) {
          //If we are looking at channels 3, 4, and 5, start the count over, we are on a different IMC
          if((channel % MAX_MC_CH) == 0) {
            channelCount = 0;
          }
          if(host->nvram.mem.socket[socket].channelList[channel].enabled == 1) {
            for(dimm = 0; dimm < MAX_DIMM; dimm++){
              if((host->nvram.mem.socket[socket].channelList[channel].dimmList[dimm].dimmPresent == 1) &&
                 (host->nvram.mem.socket[socket].channelList[channel].dimmList[dimm].aepDimmPresent == 0)) {
                //at least 1 IMC has exactly 2 ddr4 channels populated
                channelCount++;
                break;
              }
            }//dimm
          }
          //check for 2 populated channels only after looking at the last channel
          if ((channelCount == 2) && ((channel % MAX_MC_CH) == (MAX_MC_CH - 1))) {
            exactly2channels = 1;
          }
        }//channel
      }//socket
    }//prefetch
    if((imcCount > 1) && (exactly2channels == 1)) {
      mode &= ~RK_SPARE;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\t Disable Rank Sparing: more than 1 IMC and an IMC has exactly two channels populated\n"));
    }
  }

  //
  // s4929717: CRB BIOS and Unified Stack with SuperSKU impacts Isoc feature
  //
  if (host->var.kti.OutIsocEn == 1) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t OutIsocEn = 0x%x\n", host->var.kti.OutIsocEn));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\t Disabling Mirroring and Rank Sparing - Isocen enabled \n"));
    mode &= ~(FULL_MIRROR_1LM | FULL_MIRROR_2LM | RK_SPARE | PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM);
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n</checkSupportedRASModes>\n"));

  return mode;
}

/**

  Returns the paired rank

  @param host           - Pointer to sysHost
  @param socket         - Socket number
  @param ch             - Pointer to sysHost
  @param dimm           - Pointer to sysHost
  @param rank           - Pointer to sysHost
  @param pairRankNumber - Pointer to sysHost
  @param pairChannel    - Pointer to sysHost
  @param pairDimm       - Pointer to sysHost
  @param pairRank       - Pointer to sysHost

  @retval VOID

**/
void
GetPairedRank (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     *pairRankNumber,
  UINT8     *pairChannel,
  UINT8     *pairDimm,
  UINT8     *pairRank
  )
{

  if (ChipMirrorEn(host)){

    *pairRank = rank;
    *pairDimm = dimm;
    if (*pairRankNumber == 1) {
      if (ch == 0) {
        *pairChannel = 1;
      } else if (ch == 1) {
        *pairChannel = 0;
      } else if (ch == 2) {
        *pairChannel = 0;
      } else if (ch == 3) {
        *pairChannel = 4;
      } else if (ch == 4) {
        *pairChannel = 3;
      } else if (ch == 5) {
        *pairChannel = 3;
      }
    } else if (*pairRankNumber == 2) {
      if (ch == 0) {
        *pairChannel = 2;
      } else if (ch == 1) {
        *pairChannel = 2;
      } else if (ch == 2) {
        *pairChannel = 1;
      } else if (ch == 3) {
        *pairChannel = 5;
      } else if (ch == 4) {
        *pairChannel = 5;
      } else if (ch == 5) {
        *pairChannel = 4;
      }
    } else {
      *pairRankNumber = 0;
    }

  } else if (ChipLockstepEn(host)) {

    if (*pairRankNumber == 1) {
      *pairChannel = ch;
      *pairRank = rank;
      if (dimm == 0) {
        *pairDimm = 1;
      } else if (dimm == 1) {
        *pairDimm = 0;
      }
    } else {
      *pairRankNumber = 0;
    }
  }
} // GetPairedRank

