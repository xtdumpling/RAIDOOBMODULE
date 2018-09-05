/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2011 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file MemRasRoutines.c

    Mem Ras functions and platform data

---------------------------------------------------------------------------**/

#include "MemRas.h"
#include <CpuPciAccess.h>

extern EFI_CRYSTAL_RIDGE_PROTOCOL         *mCrystalRidgeProtocol;
extern SYSHOST                             mHost;
extern  MEMRAS_S3_PARAM                    mMemrasS3Param;
extern  UINT8                        mMaxRegion;

#define   RANK_INTL_MIN_CLOSE     6
#define   RANK_INTL_MIN_OPEN      13
#define   MAX_MMIIOL_SADRULES     16
#define   FORWARD_ADDRESS_TRANSLATION  1
#define   REVERSE_ADDRESS_TRANSLATION  2
// ADDDC region sizes for Static Virtual LS, ADDC
#define   ADDDC_REGION_SIZE_RANK    1
#define   ADDDC_REGION_SIZE_BANK    0
//
// Register offsets created as arrays for portability across silicon generations
//
UINT32  mDramRuleRegOffset[SAD_RULES] = {
    DRAM_RULE_CFG_0_CHABC_SAD_REG, DRAM_RULE_CFG_1_CHABC_SAD_REG, DRAM_RULE_CFG_2_CHABC_SAD_REG, DRAM_RULE_CFG_3_CHABC_SAD_REG,
    DRAM_RULE_CFG_4_CHABC_SAD_REG, DRAM_RULE_CFG_5_CHABC_SAD_REG, DRAM_RULE_CFG_6_CHABC_SAD_REG, DRAM_RULE_CFG_7_CHABC_SAD_REG,
    DRAM_RULE_CFG_8_CHABC_SAD_REG, DRAM_RULE_CFG_9_CHABC_SAD_REG, DRAM_RULE_CFG_10_CHABC_SAD_REG, DRAM_RULE_CFG_11_CHABC_SAD_REG,
    DRAM_RULE_CFG_12_CHABC_SAD_REG, DRAM_RULE_CFG_13_CHABC_SAD_REG, DRAM_RULE_CFG_14_CHABC_SAD_REG, DRAM_RULE_CFG_15_CHABC_SAD_REG,
    DRAM_RULE_CFG_16_CHABC_SAD_REG, DRAM_RULE_CFG_17_CHABC_SAD_REG, DRAM_RULE_CFG_18_CHABC_SAD_REG, DRAM_RULE_CFG_19_CHABC_SAD_REG,
    DRAM_RULE_CFG_20_CHABC_SAD_REG, DRAM_RULE_CFG_21_CHABC_SAD_REG, DRAM_RULE_CFG_22_CHABC_SAD_REG, DRAM_RULE_CFG_23_CHABC_SAD_REG
};
UINT32  mInterleaveListRegOffset[SAD_RULES] = {
    INTERLEAVE_LIST_CFG_0_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_1_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_2_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_3_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_4_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_5_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_6_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_7_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_8_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_9_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_10_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_11_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_12_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_13_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_14_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_15_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_16_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_17_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_18_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_19_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_20_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_21_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_22_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_23_CHABC_SAD_REG
};

UINT32 mDRAMRuleCfgMC[MAX_SAD_RULES] =
  { DRAM_RULE_CFG0_MC_MAIN_REG, DRAM_RULE_CFG1_MC_MAIN_REG, DRAM_RULE_CFG2_MC_MAIN_REG, DRAM_RULE_CFG3_MC_MAIN_REG,
    DRAM_RULE_CFG4_MC_MAIN_REG, DRAM_RULE_CFG5_MC_MAIN_REG, DRAM_RULE_CFG6_MC_MAIN_REG, DRAM_RULE_CFG7_MC_MAIN_REG,
    DRAM_RULE_CFG8_MC_MAIN_REG, DRAM_RULE_CFG9_MC_MAIN_REG, DRAM_RULE_CFG10_MC_MAIN_REG, DRAM_RULE_CFG11_MC_MAIN_REG,
    DRAM_RULE_CFG12_MC_MAIN_REG, DRAM_RULE_CFG13_MC_MAIN_REG, DRAM_RULE_CFG14_MC_MAIN_REG, DRAM_RULE_CFG15_MC_MAIN_REG,
    DRAM_RULE_CFG16_MC_MAIN_REG, DRAM_RULE_CFG17_MC_MAIN_REG, DRAM_RULE_CFG18_MC_MAIN_REG, DRAM_RULE_CFG19_MC_MAIN_REG,
    DRAM_RULE_CFG20_MC_MAIN_REG, DRAM_RULE_CFG21_MC_MAIN_REG, DRAM_RULE_CFG22_MC_MAIN_REG, DRAM_RULE_CFG23_MC_MAIN_REG } ;

UINT32 mInterleaveListCfgMC[MAX_SAD_RULES] =
  { INTERLEAVE_LIST_CFG0_MC_MAIN_REG, INTERLEAVE_LIST_CFG1_MC_MAIN_REG, INTERLEAVE_LIST_CFG2_MC_MAIN_REG, INTERLEAVE_LIST_CFG3_MC_MAIN_REG,
    INTERLEAVE_LIST_CFG4_MC_MAIN_REG, INTERLEAVE_LIST_CFG5_MC_MAIN_REG, INTERLEAVE_LIST_CFG6_MC_MAIN_REG, INTERLEAVE_LIST_CFG7_MC_MAIN_REG,
    INTERLEAVE_LIST_CFG8_MC_MAIN_REG, INTERLEAVE_LIST_CFG9_MC_MAIN_REG, INTERLEAVE_LIST_CFG10_MC_MAIN_REG, INTERLEAVE_LIST_CFG11_MC_MAIN_REG,
    INTERLEAVE_LIST_CFG12_MC_MAIN_REG, INTERLEAVE_LIST_CFG13_MC_MAIN_REG, INTERLEAVE_LIST_CFG14_MC_MAIN_REG, INTERLEAVE_LIST_CFG15_MC_MAIN_REG,
    INTERLEAVE_LIST_CFG16_MC_MAIN_REG, INTERLEAVE_LIST_CFG17_MC_MAIN_REG, INTERLEAVE_LIST_CFG18_MC_MAIN_REG, INTERLEAVE_LIST_CFG19_MC_MAIN_REG,
    INTERLEAVE_LIST_CFG20_MC_MAIN_REG, INTERLEAVE_LIST_CFG21_MC_MAIN_REG, INTERLEAVE_LIST_CFG22_MC_MAIN_REG, INTERLEAVE_LIST_CFG23_MC_MAIN_REG } ;

UINT32 mTadWaynessRegOffset[TAD_RULES] = {
    TADWAYNESS_0_MC_MAIN_REG, TADWAYNESS_1_MC_MAIN_REG, TADWAYNESS_2_MC_MAIN_REG, TADWAYNESS_3_MC_MAIN_REG,
    TADWAYNESS_4_MC_MAIN_REG, TADWAYNESS_5_MC_MAIN_REG, TADWAYNESS_6_MC_MAIN_REG, TADWAYNESS_7_MC_MAIN_REG
};

UINT32 mTadBaseRegOffset[TAD_RULES] = { 
    TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
    TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG 
} ;

UINT32 mTadChnlIlvOffsetRegOffset[TAD_RULES] = {
  TADCHNILVOFFSET_0_MC_MAIN_REG, TADCHNILVOFFSET_1_MC_MAIN_REG, TADCHNILVOFFSET_2_MC_MAIN_REG, TADCHNILVOFFSET_3_MC_MAIN_REG,
  TADCHNILVOFFSET_4_MC_MAIN_REG, TADCHNILVOFFSET_5_MC_MAIN_REG, TADCHNILVOFFSET_6_MC_MAIN_REG, TADCHNILVOFFSET_7_MC_MAIN_REG
};

UINT32 mFMTADChnIlvOffset[TAD_RULES] =
    { FMTADCHNILVOFFSET_0_MC_2LM_REG, FMTADCHNILVOFFSET_1_MC_2LM_REG, FMTADCHNILVOFFSET_2_MC_2LM_REG, FMTADCHNILVOFFSET_3_MC_2LM_REG,
      FMTADCHNILVOFFSET_4_MC_2LM_REG, FMTADCHNILVOFFSET_5_MC_2LM_REG, FMTADCHNILVOFFSET_6_MC_2LM_REG, FMTADCHNILVOFFSET_7_MC_2LM_REG } ;

UINT32 mRIRWaynessLimit[MAX_RIR] =
  { RIRWAYNESSLIMIT_0_MC_MAIN_REG, RIRWAYNESSLIMIT_1_MC_MAIN_REG, RIRWAYNESSLIMIT_2_MC_MAIN_REG, RIRWAYNESSLIMIT_3_MC_MAIN_REG } ;

UINT32 mRIRIlvOffset[MAX_RIR][MAX_RIR_WAYS] =
  { { RIRILV0OFFSET_0_MC_MAIN_REG, RIRILV1OFFSET_0_MC_MAIN_REG, RIRILV2OFFSET_0_MC_MAIN_REG, RIRILV3OFFSET_0_MC_MAIN_REG, RIRILV4OFFSET_0_MC_MAIN_REG, RIRILV5OFFSET_0_MC_MAIN_REG, RIRILV6OFFSET_0_MC_MAIN_REG, RIRILV7OFFSET_0_MC_MAIN_REG },
    { RIRILV0OFFSET_1_MC_MAIN_REG, RIRILV1OFFSET_1_MC_MAIN_REG, RIRILV2OFFSET_1_MC_MAIN_REG, RIRILV3OFFSET_1_MC_MAIN_REG, RIRILV4OFFSET_1_MC_MAIN_REG, RIRILV5OFFSET_1_MC_MAIN_REG, RIRILV6OFFSET_1_MC_MAIN_REG, RIRILV7OFFSET_1_MC_MAIN_REG },
    { RIRILV0OFFSET_2_MC_MAIN_REG, RIRILV1OFFSET_2_MC_MAIN_REG, RIRILV2OFFSET_2_MC_MAIN_REG, RIRILV3OFFSET_2_MC_MAIN_REG, RIRILV4OFFSET_2_MC_MAIN_REG, RIRILV5OFFSET_2_MC_MAIN_REG, RIRILV6OFFSET_2_MC_MAIN_REG, RIRILV7OFFSET_2_MC_MAIN_REG },
    { RIRILV0OFFSET_3_MC_MAIN_REG, RIRILV1OFFSET_3_MC_MAIN_REG, RIRILV2OFFSET_3_MC_MAIN_REG, RIRILV3OFFSET_3_MC_MAIN_REG, RIRILV4OFFSET_3_MC_MAIN_REG, RIRILV5OFFSET_3_MC_MAIN_REG, RIRILV6OFFSET_3_MC_MAIN_REG, RIRILV7OFFSET_3_MC_MAIN_REG } };

UINT32 mDimmMtrOffset[MAX_DIMM] = {
    DIMMMTR_0_MC_MAIN_REG, DIMMMTR_1_MC_MAIN_REG
};

UINT32 mFMRIRWaynessLimit[MAX_RIR_DDRT] =
   { FMRIRWAYNESSLIMIT_0_MC_2LM_REG, FMRIRWAYNESSLIMIT_1_MC_2LM_REG, FMRIRWAYNESSLIMIT_2_MC_2LM_REG, FMRIRWAYNESSLIMIT_3_MC_2LM_REG } ;

UINT32 mFMRIRIlvOffset[MAX_RIR_DDRT][MAX_RIR_DDRT_WAYS] =
  { { FMRIRILV0OFFSET_0_MC_2LM_REG, FMRIRILV1OFFSET_0_MC_2LM_REG },
    { FMRIRILV0OFFSET_1_MC_2LM_REG, FMRIRILV1OFFSET_1_MC_2LM_REG },
    { FMRIRILV0OFFSET_2_MC_2LM_REG, FMRIRILV1OFFSET_2_MC_2LM_REG },
    { FMRIRILV0OFFSET_3_MC_2LM_REG, FMRIRILV1OFFSET_3_MC_2LM_REG } };


UINT32  mMmiolRuleOffset[MAX_MMIIOL_SADRULES][2] = 
{ { MMIO_RULE_CFG_0_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_0_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_1_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_1_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_2_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_2_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_3_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_3_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_4_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_4_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_5_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_5_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_6_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_6_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_7_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_7_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_8_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_8_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_9_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_9_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_10_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_10_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_11_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_11_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_12_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_12_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_13_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_13_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_14_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_14_N1_CHABC_SAD1_REG },
  { MMIO_RULE_CFG_15_N0_CHABC_SAD1_REG, MMIO_RULE_CFG_15_N1_CHABC_SAD1_REG }
};
                                             
EFI_STATUS
GetNmChannelAddress (
  TRANSLATED_ADDRESS *TA
  )
{

  UINT64  SystemAddress;
  UINT8   SktId;
  UINT8   McId;
  UINT8   NmChId;
  UINT8   NodeId;
  UINT64  NmChannelAddress;
  UINT64  TempAddr;
  UINT8   NmSktWays;
  UINT8   NmChWays;
  UINT64  NmChOffset;
  UINT64  NmTargetLimit;
  UINT8   NmCapacity;
  UINT8   SktInterleaveBit;
  UINT8   ChnInterleaveBit;

  MCNMCACHINGCFG2_MC_2LM_STRUCT               McNMCachingCfg2;
  MCNMCACHINGINTLV_MC_2LM_STRUCT              McNMCachingIlv;
  MCNMCACHINGOFFSET0_MC_2LM_STRUCT            McNnCachingOffset0;

  SystemAddress = TA->SystemAddress;
  SktId      = TA->SocketId;
  NmChId     = TA->NmChannelId;
  McId = TA->MemoryControllerId;
  NodeId        = TA->Node;
  SktInterleaveBit = 12;
  ChnInterleaveBit = 8;

  McNMCachingCfg2.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGCFG2_MC_2LM_REG);
  McNMCachingIlv.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGINTLV_MC_2LM_REG);
  McNnCachingOffset0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGOFFSET0_MC_2LM_REG);

  NmChWays = 1 << (McNMCachingIlv.Bits.mcnmcaching_tad_il_granular_ximc);
  NmSktWays = 1 << (McNMCachingIlv.Bits.mcnmcachingsocketintlv);
  NmChOffset = ((UINT64)McNnCachingOffset0.Bits.mcnmcachingoffset0 << 32);

  // Find the bits to use for nm address from the system Adress
  NmCapacity = (UINT8)McNMCachingCfg2.Bits.mcnmcachingnmchncap;

  switch (NmCapacity) {
    case 0: 
      NmTargetLimit = 0x100000000; 
      break; 
    case 1: 
      NmTargetLimit = 0x200000000; 
      break; 
    case 2: 
      NmTargetLimit = 0x400000000; 
      break; 
    case 3: 
      NmTargetLimit = 0x800000000; 
      break;
    case 4: 
      NmTargetLimit = 0x1000000000; 
      break; 
    case 5: 
      NmTargetLimit = 0x2000000000; 
      break; 
    case 6: 
      NmTargetLimit = 0x4000000000; 
      break; 
    case 7: 
      NmTargetLimit = 0x8000000000; 
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  NmTargetLimit = (NmTargetLimit << (McNMCachingIlv.Bits.mcnmcaching_tad_il_granular_ximc + McNMCachingIlv.Bits.mcnmcachingsocketintlv)) - 1;

  NmChannelAddress = SystemAddress - NmChOffset;
  NmChannelAddress = NmChannelAddress & NmTargetLimit;

  TempAddr = (NmChannelAddress >> ChnInterleaveBit) / NmChWays;
  TempAddr = (TempAddr << ChnInterleaveBit) | BitFieldRead64(NmChannelAddress, 0, ChnInterleaveBit-1);
  NmChannelAddress = (TempAddr >> SktInterleaveBit) / NmSktWays;
  NmChannelAddress = (NmChannelAddress << SktInterleaveBit) | BitFieldRead64(TempAddr, 0, SktInterleaveBit-1);

  TA->NmChannelAddress = NmChannelAddress;
  
  return EFI_SUCCESS;
}


/**

   Translates Channel Address to Rank Address

  @param TRANSLATED_ADDRESS  - ptr to Address Translation structure

Modifies:
  Following fields in ADDRESS_TRANSATION structure are updated:
    PhysicalRankId
    DimmSlot
    DimmRank
    RankAddress

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
TranslateRIR (
    TRANSLATED_ADDRESS *TA
 )
{
  UINT64  physicalAddress;
  UINT64  channelAddress;
  UINT64  RankAddr;
  UINT64  rirLimit;
  UINT64  PrevLimit;
  UINT8   RirIndex;
  UINT8   node;
  UINT8   rirWays;
  UINT8   rankInterleave;
  UINT8   LogicalRank;
  UINT8   channel;
  UINT8   match;
  UINT8   pageShift;
  UINT32  rirRnkTgt0;
  UINT64  rirOffset0;
  UINT8   ChipSelect;
  UINT8   Dimm;
  UINT32  c;
  UINT8   RankGroups;

  RIRWAYNESSLIMIT_0_MC_MAIN_STRUCT  imcRIRWaynessLimit;
  FMRIRWAYNESSLIMIT_0_MC_2LM_STRUCT imcFMRIRWaynessLimit;
  RIRILV0OFFSET_0_MC_MAIN_STRUCT    imcRIRIlv0Offset;
  FMRIRILV0OFFSET_0_MC_2LM_STRUCT   imcFMRIRIlv0Offset;
  MCMTR_MC_MAIN_STRUCT              imcMcmtr;
  DIMMMTR_0_MC_MAIN_STRUCT          dimmMtr;


  node  = TA->Node;
  channelAddress  = TA->ChannelAddress;
  physicalAddress = TA->SystemAddress;
  channel         = TA->ChannelId;
  
  //
  // Initialize return values
  //
  TA->RankAddress    = (UINT64)-1;
  TA->PhysicalRankId = (UINT8)-1;
  TA->DimmSlot       = (UINT8)-1;
  TA->DimmRank       = (UINT8)-1;


  match = FALSE;
  PrevLimit = 0;

  if ((TA->MemType == MEM_TYPE_1LM) || (TA->MemType == MEM_TYPE_2LM)) {

    if (TA->MemType == MEM_TYPE_2LM) {
      channelAddress  = TA->NmChannelAddress;
      channel         = TA->NmChannelId;
    }

    DEBUG ((DEBUG_ERROR, "\nTranslateRIR:Calculating Rank Address for DDR4\n"));

    for (RirIndex = 0; RirIndex < MAX_RIR; RirIndex++) {
      if (match == TRUE) break;

      imcRIRWaynessLimit.Data = mCpuCsrAccess->ReadCpuCsr( TA->SocketId, NODECH_TO_SKTCH(node, channel), mRIRWaynessLimit[RirIndex]);
      if (imcRIRWaynessLimit.Bits.rir_val == 0) continue;

      rirLimit = (((UINT64)imcRIRWaynessLimit.Bits.rir_limit + 1) << RIR_UNIT)-1;
      rirWays = 1 << imcRIRWaynessLimit.Bits.rir_way; // 2^rir_way

      //
      // Does channel address match current RIR rule limit
      //
      // limit is 512MB granularity (bits 45:29)
      //
    
      if (((PrevLimit == 0) && (PrevLimit == channelAddress) && (channelAddress <= rirLimit)) ||
          ((PrevLimit < channelAddress) && (channelAddress <= rirLimit))) {
        match = TRUE;
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Matching Rir Entry: %d", RirIndex));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Matching Rir Limit: %x", rirLimit));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Rir Ways: %d", rirWays));

        //
        // Determine target rank interleave index for the channel address
        // Close_page uses bits 8:6; Open_page uses bits 15:13
        //
        imcMcmtr.Data = mCpuCsrAccess->ReadMcCpuCsr(TA->SocketId, TA->MemoryControllerId, MCMTR_MC_MAIN_REG );
        pageShift = (UINT8)(imcMcmtr.Bits.close_pg == 1 ? RANK_INTL_MIN_CLOSE : RANK_INTL_MIN_OPEN);
        rankInterleave = (channelAddress >> pageShift) % rirWays;
        // squeeze out the interleave
        RankAddr = (channelAddress >> pageShift) / rirWays;
        RankAddr = (RankAddr << pageShift) | BitFieldRead64(channelAddress, 0, pageShift-1);
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: shiftVal: %d", pageShift));

        //
        // Get target physical rank id & rank address
        //
        imcRIRIlv0Offset.Data = mCpuCsrAccess->ReadCpuCsr(TA->SocketId, NODECH_TO_SKTCH(node, channel), mRIRIlvOffset[RirIndex][rankInterleave]);
        rirRnkTgt0 = imcRIRIlv0Offset.Bits.rir_rnk_tgt0;
        rirOffset0 = ((UINT64)(imcRIRIlv0Offset.Bits.rir_offset0) << 26);
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_rnk_tgt0: %d", rirRnkTgt0));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_offset0: %lx", rirOffset0));

        TA->RankAddress = RankAddr - rirOffset0;
        ChipSelect = (UINT8) rirRnkTgt0;
        Dimm = ChipSelect >> 2;

        dimmMtr.Data = mCpuCsrAccess->ReadCpuCsr(TA->SocketId, NODECH_TO_SKTCH(node, channel), mDimmMtrOffset[Dimm]);

        if (dimmMtr.Bits.ddr4_3dsnumranks_cs == 0) {
          LogicalRank = ChipSelect;
          TA->PhysicalRankId = ChipSelect % MAX_RANK_DIMM;
        } else {
          TA->PhysicalRankId = (ChipSelect - 1) % MAX_RANK_DIMM;
          c = (UINT32)(RankAddr >> (30 + dimmMtr.Bits.ra_width));
          RankGroups = (UINT8)(dimmMtr.Bits.rank_cnt + 1);
          if (RankGroups == 1) LogicalRank = (UINT8) c;
          else LogicalRank = (UINT8) ((ChipSelect - 1) + (c*2));
          LogicalRank += (Dimm << 3);
        }

        if (TA->PhysicalRankId > 9) {
          //
          // error
          //
          return EFI_INVALID_PARAMETER;
        }
        TA->DimmSlot = Dimm;
        TA->DimmRank = LogicalRank;
        TA->ChipSelect = ChipSelect;
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: LogicalRank: %d", LogicalRank));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Dimm: %d", Dimm));

        DEBUG ((DEBUG_ERROR, "\nTranslateRIR - RankAddress = %0lx\n", (UINT64)(TA->RankAddress)));

        if (TA->MemType == MEM_TYPE_2LM){
          TA->NmRankAddress = TA->RankAddress;
          TA->NmPhysicalRankId = TA->PhysicalRankId;
          TA->NmDimmSlot = TA->DimmSlot;
          TA->NmChipSelect = TA->ChipSelect;
        }
      } // if rirLimit
      PrevLimit = rirLimit;
    } // for (rirIndex)

  } 
  
  if (!(TA->MemType == MEM_TYPE_1LM)) {

    DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Get Rank Info for DDRT Address\n"));
    PrevLimit = (UINT64) -1;
    for (RirIndex = 0; RirIndex < MAX_RIR_DDRT; RirIndex++) {
      if (match == TRUE) break;

      imcFMRIRWaynessLimit.Data = mCpuCsrAccess->ReadCpuCsr( TA->SocketId, NODECH_TO_SKTCH(node, channel), mFMRIRWaynessLimit[RirIndex]);
      if (imcFMRIRWaynessLimit.Bits.rir_val == 0) continue;

      rirLimit = (UINT64) imcFMRIRWaynessLimit.Bits.rir_limit << RIR_UNIT;
      rirWays = 1 << imcFMRIRWaynessLimit.Bits.rir_way; // 2^rir_way

      //
      // Does channel address match current RIR rule limit
      //
      // limit is 512MB granularity (bits 45:29)
      //
      if ((PrevLimit < channelAddress) && (channelAddress <= rirLimit)) {
        match = TRUE;
        //
        // Determine target rank interleave index for the channel address
        // Close_page uses bits 8:6; Open_page uses bits 15:13
        //
        imcMcmtr.Data = mCpuCsrAccess->ReadMcCpuCsr(TA->SocketId, TA->MemoryControllerId, MCMTR_MC_MAIN_REG );
        pageShift = (UINT8)(imcMcmtr.Bits.close_pg == 1 ? RANK_INTL_MIN_CLOSE : RANK_INTL_MIN_OPEN);
        rankInterleave = (channelAddress >> pageShift) % rirWays;

        // squeeze out the interleave
        RankAddr = (channelAddress >> pageShift) / rirWays;
        RankAddr = (RankAddr << pageShift) | BitFieldRead64(channelAddress, 0, pageShift-1);
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: shiftVal: %d", pageShift));


        imcFMRIRIlv0Offset.Data = mCpuCsrAccess->ReadCpuCsr(TA->SocketId, NODECH_TO_SKTCH(node, channel), mFMRIRIlvOffset[RirIndex][0]);
        ChipSelect = (UINT8)imcFMRIRIlv0Offset.Bits.rir_rnk_tgt0;
        rirOffset0 = ((UINT64)(imcFMRIRIlv0Offset.Bits.rir_offset0) << 26);
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_rnk_tgt0: %d", ChipSelect));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_offset0: %lx", rirOffset0));

        TA->RankAddress = RankAddr - rirOffset0;
        TA->DPA = TA->RankAddress >> 6;
        TA->PhysicalRankId = ChipSelect % MAX_RANK_DIMM;
        TA->DimmSlot = ChipSelect >> 2;
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Rank Address: 0x%lx", TA->RankAddress));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Physcial Rank: %d", TA->PhysicalRankId));
        DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Dimm: %d", TA->DimmSlot));
        DEBUG ((DEBUG_ERROR, "\nTranslate RIR DDRT DPA Address: 0x%lx", TA->DPA));
       }    
    }
  }
  if(match == TRUE) {
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_ERROR, "\nTranslateRIR:Error in getting Rank Address\n"));
    return EFI_INVALID_PARAMETER;
  }

} // TranslateRIR

EFI_STATUS
TranslateDDRTTad (
  TRANSLATED_ADDRESS *TA
) 
{
  UINT64  systemAddress;
  UINT64  channelAddress;
  UINT64  tadLimit;
  UINT64  TadBase;
  UINT64  chOffset;
  UINT8   TadId;
  UINT8   sktWays;
  UINT8   chWays;
  UINT8   sktGran;
  UINT8   chGran;
  UINT8   SocketId;
  UINT8   McId;
  UINT8   ChannelId;
  UINT8   NodeId;
  UINT8   SadId;
  UINT8   sktInterleaveBit = 0;
  UINT8   chInterleaveBit = 0;
  UINT64  CoreAddr;
  UINT64  TempAddr;

  TADWAYNESS_0_MC_MAIN_STRUCT           TadWayness;
  TADBASE_0_MC_MAIN_STRUCT              TadBaseReg;
  SAD2TAD_M2MEM_MAIN_STRUCT             Sad2TadReg;
  FMTADCHNILVOFFSET_0_MC_2LM_STRUCT     FmTadChIlvOffset;

  systemAddress = TA->SystemAddress;
  SocketId      = TA->SocketId;
  NodeId        = TA->Node;
  ChannelId     = TA->ChannelId;
  McId = TA->MemoryControllerId;
  SadId = TA->SadId;
  TA->ChannelAddress = (UINT64)-1;
  TA->TadId = (UINT8)-1;

  Sad2TadReg.Data = 0;
  Sad2TadReg.Bits.sad2tadrden = 1;
  Sad2TadReg.Bits.sad2tadwren = 0;
  Sad2TadReg.Bits.sadid = SadId;

  mCpuCsrAccess->WriteMcCpuCsr (SocketId, McId, SAD2TAD_M2MEM_MAIN_REG, Sad2TadReg.Data);
  Sad2TadReg.Data = mCpuCsrAccess->ReadMcCpuCsr (SocketId, McId, SAD2TAD_M2MEM_MAIN_REG);

  TadId = (UINT8)Sad2TadReg.Bits.ddrttadid;
  TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, mTadBaseRegOffset[TadId]);
  TadWayness.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, mTadWaynessRegOffset[TadId]);
  tadLimit = (((UINT64)TadWayness.Bits.tad_limit + 1) << SAD_UNIT)-1;
  TadBase =  (((UINT64)TadBaseReg.Bits.base) << SAD_UNIT);

  if (!((TadBase <= systemAddress) && (systemAddress <= tadLimit))) {
    DEBUG ((DEBUG_ERROR, "\nTranslateTAD: SystemAddress doesn't fall into the obtained TADID \n"));
    return EFI_INVALID_PARAMETER;
  }

  FmTadChIlvOffset.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, NODECH_TO_SKTCH(NodeId, ChannelId), mFMTADChnIlvOffset[TadId]);
  chOffset = ((UINT64)FmTadChIlvOffset.Bits.tad_offset << SAD_UNIT);
  DEBUG ((DEBUG_ERROR, "\nTranslateTAD: (chOffset shl SAD_UNIT): %lx", chOffset));
  if(chOffset & (((UINT64)MAX_MEM_ADDR << 27))) {  //  MAX_MEM_ADDR = 0x40000 ( 256Mb gran.) check bit45 to verfiy the sign bit.
    chOffset  = chOffset - ((UINT64)MAX_MEM_ADDR << 28);
    DEBUG ((DEBUG_ERROR, "\nTranslateTAD: (chOffset - 1 shl 46) %lx", chOffset));
    chOffset &= (UINT64)0x3ffffffffff;  // 46bit width maximum valid address. ( (1 << 46) -1)
  }

  if (TA->MemType == MEM_TYPE_2LM) {
    sktWays = (UINT8) (1 << FmTadChIlvOffset.Bits.skt_ways);
    chWays = (UINT8)FmTadChIlvOffset.Bits.chn_ways+1;
  } else {
    sktWays = (UINT8) (1 << TadWayness.Bits.tad_skt_way);
    chWays = (UINT8)TadWayness.Bits.tad_ch_way+1;
  }

  sktGran = (UINT8) FmTadChIlvOffset.Bits.skt_granularity;
  switch (sktGran) {
  case 0: sktInterleaveBit = 6; break; // 64B
  case 1: sktInterleaveBit = 8; break; // 256BRIR
  case 2: sktInterleaveBit = 12; break; // 4KB
  case 3: sktInterleaveBit = 30; break; // 1GB
  default: return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_ERROR, "\nTranslateTAD: SktInterleaveBit: %d", sktInterleaveBit));

  chGran = (UINT8) FmTadChIlvOffset.Bits.ch_granularity;
  switch (chGran) {
  case 0: chInterleaveBit = 6; break; // 64B
  case 1: chInterleaveBit = 8; break; // 256B
  case 2: chInterleaveBit = 12; break; // 4KB
  default: return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_ERROR, "\nTranslateTAD: chInterleaveBit: %d", chInterleaveBit));

  if (FmTadChIlvOffset.Bits.tad_offset_sign == 0) {
    CoreAddr = systemAddress - chOffset;
  } else {
      CoreAddr = systemAddress + chOffset;
  }
  DEBUG ((DEBUG_ERROR, "\nTranslateTAD: CoreAddr %lx", CoreAddr));
  TempAddr = (CoreAddr >> chInterleaveBit) / chWays;
  TempAddr = (TempAddr << chInterleaveBit) | BitFieldRead64(CoreAddr, 0, chInterleaveBit-1);
  channelAddress = (TempAddr >> sktInterleaveBit) / sktWays;
  channelAddress = (channelAddress << sktInterleaveBit) | BitFieldRead64(TempAddr, 0, sktInterleaveBit-1);

  TA->ChannelAddress = channelAddress;
  TA->TadId = TadId;
  DEBUG ((DEBUG_ERROR, "\nTranslateTAD: channelAddress: %x:%x", (UINT32)(channelAddress >> 32), (UINT32)channelAddress));

  if (TA->MemType != MEM_TYPE_2LM) {
    DEBUG ((DEBUG_ERROR, "\nTranslateTAD Successfully completed for PMEM/BMEM\n"));
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_ERROR, "\nCalculate NM Channel Address now\n"));
    if (GetNmChannelAddress (TA) == EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD Successfully completed for 2LM\n"));
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }
}

/**

   Translates System Address to Channel Address

  @param TRANSLATED_ADDRESS  - ptr to Address Translation structure

Modifies:
  Following fields in ADDRESS_TRANSATION structure are updated:
    ChannelAddress
    ChannelId

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
TranslateTAD (
  TRANSLATED_ADDRESS *TA
  )
{
  UINT64  systemAddress;
  UINT64  channelAddress;
  UINT64  tadLimit;
  UINT64  PrevLimit;
  UINT64  chOffset;
  UINT8   TadIndex, Index;
  UINT8   sktWays;
  UINT8   chWays;
  UINT8   sktGran;
  UINT8   chGran;
  UINT8   SocketId;
  UINT8   McId;
  UINT8   ChannelId;
  UINT8   NodeId;
  UINT8   SadId;
  UINT8   sktInterleaveBit = 0;
  UINT8   chInterleaveBit = 0;
  UINT8   priSecBit;
  BOOLEAN match = FALSE;
  UINT64  CoreAddr;
  UINT64  TempAddr;
  TADCHNILVOFFSET_0_MC_MAIN_STRUCT      TadChannelOffset;
  TADWAYNESS_0_MC_MAIN_STRUCT           TadWayness;
  TADBASE_0_MC_MAIN_STRUCT              TadBaseReg, TempTadBase;
  MODE_M2MEM_MAIN_STRUCT                M2MemMode;
  MIRRORFAILOVER_M2MEM_MAIN_STRUCT      MirrorFailOver;
  MIRRORCHNLS_M2MEM_MAIN_STRUCT         MirrorChannels;

  systemAddress = TA->SystemAddress;
  SocketId      = TA->SocketId;
  NodeId        = TA->Node;
  ChannelId     = TA->ChannelId;
  McId = TA->MemoryControllerId;
  SadId = TA->SadId;
  //
  // Initialize return values
  //
  TA->ChannelAddress = (UINT64)-1;
  TA->TadId = (UINT8)-1;

  if (TA->MemType == MEM_TYPE_1LM) {

    DEBUG ((DEBUG_ERROR, "\nTAD Translation for 1LM\n"));

    PrevLimit = 0;
    match = FALSE;
    priSecBit = 0;
    for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
      if (match == TRUE) break;

      TadWayness.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, mTadWaynessRegOffset[TadIndex]);
      DEBUG ((DEBUG_ERROR, "\n TadWayness.Data = 0x%lx\n",  TadWayness.Data));
      tadLimit = (((UINT64)TadWayness.Bits.tad_limit + 1) << SAD_UNIT)-1;
  
      if ((TadIndex != 0) && (PrevLimit == tadLimit)) break;

      DEBUG ((DEBUG_ERROR, "\nTranslateTad:Tad:%d limit:%08lx tadWayness:%08lx", TadIndex, tadLimit, TadWayness.Data));
      //
      // Does address match current tad rule limt
      //
      if (((PrevLimit == 0) && (PrevLimit == systemAddress) && (systemAddress <= tadLimit)) ||
            ((PrevLimit < systemAddress) && (systemAddress <= tadLimit))) {
        match = TRUE;
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: Tad Entry: %d", TadIndex));

        // Read TadBase register
        TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, mTadBaseRegOffset[TadIndex]);

        // Read M2MemMode register for mirrorddr4 field
        M2MemMode.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, MODE_M2MEM_MAIN_REG);

        if (TadBaseReg.Bits.mirror_en || M2MemMode.Bits.mirrorddr4) {

          // For A0 Stepping, partial mirroring can be enabled only on TAD0. Returning error if it enabled on other TADs
          if (mHost.var.common.stepping < B0_REV_SKX) {
            for (Index = 0; Index < TAD_RULES; Index++) {
              TempTadBase.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, McId, mTadBaseRegOffset[TadIndex]);
              if (Index != 0 && TempTadBase.Bits.mirror_en == 1) {
                DEBUG ((DEBUG_ERROR, "\nTranslateTAD error: Mirroring is enabled on Tad %d, For A0 stepping, mirroring can be enabled only on TAD0.\nIllegal configuration", TadIndex));
                return EFI_UNSUPPORTED;
              }
            }
          }

          // Read MirrorFailOver register for the ddr4chnlfailed
          MirrorFailOver.Data = mCpuCsrAccess->ReadMcCpuCsr (SocketId, McId, MIRRORFAILOVER_M2MEM_MAIN_REG);

          if ((MirrorFailOver.Bits.ddr4chnlfailed >> TA->ChannelId ) & 1) {

            // double check the failed channel information with the TAD base register
            if (TadBaseReg.Bits.failed_ch != TA->ChannelId) {
              DEBUG ((DEBUG_ERROR, "\nTranslateTAD Error: MirrorFailOver.ddr4chnlfailed is not matching with TadBase.failed_ch"));
              return EFI_UNSUPPORTED;
            } else {
              MirrorChannels.Data = mCpuCsrAccess->ReadMcCpuCsr (SocketId, McId, MIRRORCHNLS_M2MEM_MAIN_REG);

              if (TA->ChannelId == 0){
                ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl0secondary;
              } 
              else if (TA->ChannelId == 1) {
                ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl1secondary;
              }
              else if (TA->ChannelId == 2) {
                ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl2secondary;
              }
              priSecBit = 1;  //A18 bit value that is added to channel address
            }
          }
        }

        // add code here for lock step case

        sktWays = (UINT8) (1 << TadWayness.Bits.tad_skt_way);
        chWays = (UINT8)TadWayness.Bits.tad_ch_way+1;
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: sktWays: %d", sktWays));
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: chWays: %d", chWays));

        TadChannelOffset.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, NODECH_TO_SKTCH(NodeId, ChannelId), mTadChnlIlvOffsetRegOffset[TadIndex]);

        sktGran = (UINT8) TadChannelOffset.Bits.skt_granularity;
        switch (sktGran) {
        case 0: sktInterleaveBit = 6; break; // 64B
        case 1: sktInterleaveBit = 8; break; // 256BRIR
        case 2: sktInterleaveBit = 12; break; // 4KB
        case 3: sktInterleaveBit = 30; break; // 1GB
        }
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: SktInterleaveBit: %d", sktInterleaveBit));

        chGran = (UINT8) TadChannelOffset.Bits.ch_granularity;
        switch (chGran) {
        case 0: chInterleaveBit = 6; break; // 64B
        case 1: chInterleaveBit = 8; break; // 256B
        case 2: chInterleaveBit = 12; break; // 4KB
        }
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: chInterleaveBit: %d", chInterleaveBit));

        //imcBits = sktInterleaveBit - 6 + tadWayness.Bits.tad_skt_way; // check why PythonSV calculates this since it is not used at all
        chOffset = ((UINT64)TadChannelOffset.Bits.tad_offset << SAD_UNIT);
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: (chOffset shl SAD_UNIT): %lx", chOffset));
        if(chOffset & (((UINT64)MAX_MEM_ADDR << 27))) {  //  MAX_MEM_ADDR = 0x40000 ( 256Mb gran.) check bit45 to verfiy the sign bit.
          chOffset  = chOffset - ((UINT64)MAX_MEM_ADDR << 28);
          DEBUG ((DEBUG_ERROR, "\nTranslateTAD: (chOffset - 1 shl 46) %lx", chOffset));
        }
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: chOffest 46bit adj %lx", chOffset));
        CoreAddr = systemAddress - chOffset;
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: CoreAddr %lx", CoreAddr));
        TempAddr = (CoreAddr >> chInterleaveBit) / chWays;
        TempAddr = (TempAddr << chInterleaveBit) | BitFieldRead64(CoreAddr, 0, chInterleaveBit-1);
        channelAddress = (TempAddr >> sktInterleaveBit) / sktWays;
        channelAddress = (channelAddress << sktInterleaveBit) | BitFieldRead64(TempAddr, 0, sktInterleaveBit-1);

        if (TadBaseReg.Bits.mirror_en) {
          TempAddr        = BitFieldRead64(channelAddress, 0 , 17);
          channelAddress  >>= 18;
          channelAddress  <<= 1;
          channelAddress   |= priSecBit;
          channelAddress  <<= 18;
          channelAddress  |= TempAddr;
        }

        TA->ChannelAddress = channelAddress;
        TA->ChannelId = ChannelId;
        TA->TadId = TadIndex;
        DEBUG ((DEBUG_ERROR, "\nTranslateTAD: channelAddress: %x:%x", (UINT32)(channelAddress >> 32), (UINT32)channelAddress));
      } // if match
      PrevLimit = tadLimit;
    } // for (TadIndex)

    return (match == TRUE) ? EFI_SUCCESS : EFI_INVALID_PARAMETER;
  } else {
    DEBUG ((DEBUG_ERROR, "TAD translation for DDRT\n"));
    if (TranslateDDRTTad (TA) == EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "TranlsateTAD successful for DDRT\n"));
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }

  }
    
} // TranslateTAD


/**

   Given the Interleave Mode return the interleave index.
   This is equivalent to (systemAddress / interleave granularity) % Posible Nodes

  @param interleave_mode  - ptr to Address Translation structure
  @param systemAddress

Modifies:

  @retval Interleave Mode


**/
UINT8
GetInterleaveListIndex(
  UINT8   interleave_mode,
  UINT64  systemAddress
  )
{
  UINT8   interleaveListIndex = 0;
  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Interleave mode: %x", interleave_mode));
  switch(interleave_mode) {
    case 0:
      interleaveListIndex = (UINT8)((systemAddress >> 6) & 0x7);
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Interleave Index [6,7,8]: %x", interleaveListIndex));
      break;
    case 1:
      interleaveListIndex = (UINT8)((systemAddress >> 8) & 0x7);
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Interleave Index [8,9,10]: %x", interleaveListIndex));
      break;
    case 2:
      interleaveListIndex = (UINT8)((systemAddress >> 12) & 0x7);
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Interleave Index [12,13,14]: %x", interleaveListIndex));
      break;
    case 3:
      interleaveListIndex = (UINT8)((systemAddress >> 30) & 0x7);
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Interleave Index [30,31,32]: %x", interleaveListIndex));
      break;
    default:
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Invalid Value: %x", interleaveListIndex));
      break;
  }
  return interleaveListIndex;
}

/**

   Given the SAD#, socket#, InterleaveListIndex returns the target id

  @param sadId - index of the SAD
  @param interleaveListIndex - interleave list index in SAD corresponding to the logical memory node
  @param SocketId - index of the socket
  @param TargetId of the logical memory node

Modifies:

  @retval EFI_SUCCESS - when success
  @retval EFI_INVALID_PARAMETER


**/
EFI_STATUS
GetTargetId(
  UINT32   InterleaveListCfg,
  UINT8   interleaveListIndex,
  UINT8   SocketId,
  UINT8   *TargetId
  )
{
  INTERLEAVE_LIST_CFG_0_CHABC_SAD_STRUCT InterleaveList;
  UINT8 targetId = 0;

  if(TargetId == NULL || SocketId >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  InterleaveList.Data = InterleaveListCfg;

  switch (interleaveListIndex) {
    case 0:
      targetId = (UINT8)InterleaveList.Bits.package0;
      break;
    case 1:
      targetId = (UINT8)InterleaveList.Bits.package1;
      break;
    case 2:
      targetId = (UINT8)InterleaveList.Bits.package2;
      break;
    case 3:
      targetId = (UINT8)InterleaveList.Bits.package3;
      break;
    case 4:
      targetId = (UINT8)InterleaveList.Bits.package4;
      break;
    case 5:
      targetId = (UINT8)InterleaveList.Bits.package5;
      break;
    case 6:
      targetId = (UINT8)InterleaveList.Bits.package6;
      break;
    case 7:
      targetId = (UINT8)InterleaveList.Bits.package7;
      break;
  } // switch

  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: TargetId: %x for Skt: %x", targetId, SocketId));

  if(targetId & BIT3) {
    // target is local, return the Imc bit
    *TargetId = targetId;
    DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Local Skt Found: %x", SocketId));
    return EFI_SUCCESS;
  }else {
    // target is remote, return the socket number
    *TargetId = targetId;
    return EFI_NOT_FOUND;
  }
}

/**

  Get the logical channel id for the given target id

  @param systemAddress
  @param TargetId - target id in interleavelist CSR for the current sktGran
  @param DramRule - SAD copy
  @param logChId - Logical channel id

Modifies:

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER


**/
EFI_STATUS
GetLogicalChId(
  IN  UINT64  systemAddress,
  IN  UINT8   TargetId,
  IN  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT DramRule,
  OUT UINT8   *logChId
  )
{
  UINT8   mod3_asamod2=0;
  UINT8   mod3=0;
  UINT8   mod3_mode=0;
  UINT8   ch_mod=0;
  UINT8   logChIdTmp=0;
  UINT8   address_mod=0;

  mod3_asamod2    = (UINT8)DramRule.Bits.mod3_asamod2;
  mod3            = (UINT8)DramRule.Bits.mod3;
  mod3_mode       = (UINT8)DramRule.Bits.mod3_mode;

  if(logChId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: mod3: %x", mod3));
  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: mod3_asamod2: %x", mod3_asamod2));
  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: mod3_mode: %x", mod3_mode));

  if(mod3 == 1) { // either 2 chway or 3 chway mode
    if(mod3_asamod2 == 0) {
      ch_mod = 3; // 3 chway mode
    } else {
      ch_mod = 2; // 2 chway mode
    }

    DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Ch_Mod: %x", ch_mod));

    switch(mod3_mode) {
      case 0:
        address_mod = (systemAddress >> 6) % ch_mod;
        break;
      case 1:
        address_mod = (systemAddress >> 8) % ch_mod;
        break;
      case 2:
        address_mod = (systemAddress >> 12) % ch_mod;
        break;
    }

    DEBUG ((DEBUG_ERROR, "\nTranslateSAD: address_mod: %x", address_mod));

    switch(mod3_asamod2) {
      case 0: // 3 chway
        logChIdTmp = (TargetId & 0x1) | ((address_mod << 1) & 0x6);
        break;
      case 1: // 2 chway, ch 0,1
        logChIdTmp = (TargetId & 0x1) | ((address_mod << 1) & 0x2);
        break;
      case 2: // 2 chway, ch 1,2
        logChIdTmp = (TargetId & 0x1) |
                      (((address_mod ^ 0x1) << 1) & 0x2) |
                      ((address_mod << 2) & 0x4);
        break;
      case 3: // 2 chway, ch 0, 2
        logChIdTmp = (TargetId & 0x1) | ((address_mod << 2) & 0x4);
        break;

    }
  } else { // 1 chway mode
    logChIdTmp = TargetId & 0x7;
  }

  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: logChId: %x", logChIdTmp));

  *logChId = logChIdTmp;
  return EFI_SUCCESS;
}

/**

  Logical channel to physicall channel conversion

  @param SocketId - index of the socket
  @param logChId - Logical channel id
  @param mcId - physical memory node index
  @param chId - physical chanel index

Modifies:

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER


**/
EFI_STATUS
ChIdToMcCh(
  IN UINT8 SocketId,
  IN UINT8 logChId,
  OUT UINT8 *mcId,
  OUT UINT8 *chId
  )
{
  MC_ROUTE_TABLE_CHA_PMA_STRUCT         chaRouteTable;

  if(chId == NULL ||
     mcId == NULL ||
     SocketId > MAX_SOCKET
     ) {
    return EFI_INVALID_PARAMETER;
  }

  chaRouteTable.Data = mCpuCsrAccess->ReadCpuCsr(SocketId, logChId, MC_ROUTE_TABLE_CHA_PMA_REG);
  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: chaRouteTable: %lx", chaRouteTable.Data));

  switch(logChId) {
    case 0:
      *chId = (UINT8)chaRouteTable.Bits.channelid0;
      *mcId = (UINT8)chaRouteTable.Bits.ringid0;
      break;
    case 1:
      *chId = (UINT8)chaRouteTable.Bits.channelid1;
      *mcId = (UINT8)chaRouteTable.Bits.ringid1;
      break;
    case 2:
      *chId = (UINT8)chaRouteTable.Bits.channelid2;
      *mcId = (UINT8)chaRouteTable.Bits.ringid2;
      break;
    case 3:
      *chId = (UINT8)chaRouteTable.Bits.channelid3;
      *mcId = (UINT8)chaRouteTable.Bits.ringid3;
      break;
    case 4:
      *chId = (UINT8)chaRouteTable.Bits.channelid4;
      *mcId = (UINT8)chaRouteTable.Bits.ringid4;
      break;
    case 5:
      *chId = (UINT8)chaRouteTable.Bits.channelid5;
      *mcId = (UINT8)chaRouteTable.Bits.ringid5;
      break;
    default:
      return EFI_INVALID_PARAMETER;
      break;
  }

  DEBUG ((DEBUG_ERROR, "\nTranslateSAD: mcId: %x, chId: %x", *mcId ,*chId));

  return EFI_SUCCESS;
}


/**

   Determines NodeID that maps given system Address

  @param TRANSLATED_ADDRESS  - ptr to Address Translation structure

Modifies:
  Following fields in ADDRESS_TRANSATION structure are updated:
    sad
    socketId
    targetId
    MemoryControllerId
    Node
    LogChannelId
    ChannelId

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
TranslateSAD (
  TRANSLATED_ADDRESS *TA
 )
{
  EFI_STATUS  Status;
  UINT64  systemAddress;
  UINT64  limit;
  UINT64  PrevLimit;
  UINT8   sad;
  UINT8   interleaveListIndex;
  UINT8   targetId;
  UINT8   logChId;
  UINT8   socketId;
  UINT8   Channel;
  UINT8   mcId;
  UINT8   chId;
  BOOLEAN match;
  BOOLEAN DDRTDimmPresent;
  UINT8   rule_enable;
  UINT8   dram_attribute;
  UINT8   interleave_mode;
  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT  DramRule;
  SAD2TAD_M2MEM_MAIN_STRUCT         Sad2TadReg;
  MCDDRTCFG_MC_MAIN_STRUCT          McDdrtCfg;
  MODE_M2MEM_MAIN_STRUCT            M2MemMode;
  INTERLEAVE_LIST_CFG0_MC_MAIN_STRUCT  InterleaveListCfg;


  //
  // Find matching DRAM SAD
  //
  systemAddress = TA->SystemAddress;
  match = FALSE;
  targetId = (UINT8)-1;
  PrevLimit = 0;
  socketId = (UINT8)-1;
  mcId = (UINT8)-1;
  chId = (UINT8)-1;
  logChId = (UINT8)-1;
  interleaveListIndex = 0;
  DDRTDimmPresent = FALSE;

  //
  // Initialize return values
  //
  TA->TargetId = (UINT8)-1;
  TA->SocketId = (UINT8)-1;
  TA->MemoryControllerId = (UINT8)-1;
  TA->Node = (UINT8)-1;
  TA->SadId = (UINT8)-1;

  //
  // Determine the first node that is populated
  //
  for(socketId = 0; socketId < MAX_SOCKET; socketId++){

    if((mHost.var.common.socketPresentBitMap & (1 << socketId)) == 0) {
      continue;
    }

    for (sad = 0; sad < SAD_RULES; sad++) {
      if (match == TRUE) break;

      DramRule.Data = mCpuCsrAccess->ReadCpuCsr(socketId, 0, mDramRuleRegOffset[sad]);
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD:sad:%d DramRule:%08lx", sad, DramRule.Data));

      rule_enable     = (UINT8)DramRule.Bits.rule_enable;
      dram_attribute  = (UINT8)DramRule.Bits.attr;
      limit           = (((UINT64)DramRule.Bits.limit + 1) << SAD_UNIT)-1;//limit is 64MB granularity ( Addr[45:26] )
      interleave_mode = (UINT8)DramRule.Bits.interleave_mode;

      //
      // Skip rule if not enabled, or attribute not DRAM
      //
      if ((rule_enable == 0) || (dram_attribute != 0) ) continue;

      if ((sad != 0) && (PrevLimit == limit)) break;

      //
      // Does address match current sad rule limit
      //
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD:sad:%d limit:%08lx", sad, limit));
      if (((PrevLimit == 0) && (PrevLimit == systemAddress) && (systemAddress <= limit)) ||
          ((PrevLimit < systemAddress) && (systemAddress <= limit))) {
        match = TRUE;
        DEBUG ((DEBUG_ERROR, "\nTranslateSAD: Match in SAD[dram][%d]", sad));
       
        interleaveListIndex = GetInterleaveListIndex(interleave_mode,systemAddress);
        DEBUG ((DEBUG_ERROR, "\nTranslateSAD: interleaveListIndex: %d", interleaveListIndex));

        InterleaveListCfg.Data = mCpuCsrAccess->ReadCpuCsr(socketId, 0, mInterleaveListRegOffset[sad]);
        Status = GetTargetId(InterleaveListCfg.Data,interleaveListIndex,socketId, &targetId);

        if(Status == EFI_SUCCESS) {
          GetLogicalChId(systemAddress,targetId, DramRule, &logChId);
          ChIdToMcCh(socketId,logChId,&mcId,&chId);
          //Found The entry for the local socket
          TA->SadId = sad;
          TA->SocketId = socketId;
          TA->TargetId = targetId;
          TA->MemoryControllerId = mcId;
          TA->Node = SKT_TO_NODE(socketId,mcId);
          TA->LogChannelId = logChId;
          TA->ChannelId = chId;
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: node: %d", TA->Node));
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: SocketId: %d", TA->SocketId));
          DEBUG ((DEBUG_ERROR,"\nTranslateSAD: TargetId: %d", TA->TargetId));
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: MemoryControllerId: %d", TA->MemoryControllerId));
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: LogChannelId: %d", TA->LogChannelId));
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: ChannelId: %d", TA->ChannelId));
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: SadId: %d", TA->SadId));
        } else {
          //Sad match but not in the local socket. Keep Iterating.
          match = FALSE;
          break;
        }
     } // if

      PrevLimit = limit;
    } // for (sad)

    if(match == TRUE) break;
  } // for (socketId)

  if (match == TRUE) {

    // read all the Channels to find out if there is a DDRT DIMM in the system
    for (socketId = 0; socketId < MAX_SOCKET; socketId++){
      if ((mHost.var.common.socketPresentBitMap & (1 << socketId)) == 0) {
        continue;
      }

      for (Channel = 0; Channel < MAX_CH; Channel++) {
        if (!(mMemRas->SystemMemInfo->Socket[socketId].ChannelInfo[Channel].Enabled)) continue;

        McDdrtCfg.Data = mCpuCsrAccess->ReadCpuCsr (socketId, Channel, MCDDRTCFG_MC_MAIN_REG);
        if ((McDdrtCfg.Bits.slot0 == 1) || (McDdrtCfg.Bits.slot1 == 1)) {
          DDRTDimmPresent = 1;
        }
      }
    }

    if (DDRTDimmPresent == 0) {
      TA->MemType = MEM_TYPE_1LM;
      DEBUG ((DEBUG_ERROR, "\nMemType is 1LM\n"));
      DEBUG ((DEBUG_ERROR, "\nTranslateSAD SUCCESSFUL!\n"));
      return EFI_SUCCESS;
    } else {
      // Read Sad2TadRegister to know if the SAD is PMEM or BMEM
      Sad2TadReg.Data = 0;
      Sad2TadReg.Bits.sad2tadrden = 1;
      Sad2TadReg.Bits.sad2tadwren = 0;
      Sad2TadReg.Bits.sadid = TA->SadId;
      mCpuCsrAccess->WriteMcCpuCsr (socketId, mcId, SAD2TAD_M2MEM_MAIN_REG, Sad2TadReg.Data);
      Sad2TadReg.Data = mCpuCsrAccess->ReadMcCpuCsr (socketId, mcId, SAD2TAD_M2MEM_MAIN_REG);

      if ((Sad2TadReg.Bits.pmemvld == 1) || (Sad2TadReg.Bits.blkvld == 1)) {
        TA->MemType = MEM_TYPE_PMEM | MEM_TYPE_BLK_WINDOW;
        DEBUG ((DEBUG_ERROR, "\nMemType is BLK or PMEM\n"));
        DEBUG ((DEBUG_ERROR, "\nTranslateSAD SUCCESSFUL!\n"));
        return EFI_SUCCESS;
      }

      // find out if nearmemory caching is enabled
      M2MemMode.Data = mCpuCsrAccess->ReadMcCpuCsr(socketId, mcId, MODE_M2MEM_MAIN_REG);
      if (M2MemMode.Bits.nmcaching == 1) {
        
        // The ChId Found is NmChannel Id, Proceed to find out FmChannelId
        TA->MemType = MEM_TYPE_2LM;
        TA->NmChannelId = TA->ChannelId;

        // Read the MC DRAM Rule based on the obtained SADID
        DramRule.Data = mCpuCsrAccess->ReadMcCpuCsr (socketId, mcId, mDRAMRuleCfgMC[TA->SadId]);
        rule_enable     = (UINT8)DramRule.Bits.rule_enable;
        dram_attribute  = (UINT8)DramRule.Bits.attr;
        limit           = (((UINT64)DramRule.Bits.limit + 1) << SAD_UNIT)-1;//limit is 64MB granularity ( Addr[45:26] )
        interleave_mode = (UINT8)DramRule.Bits.interleave_mode;
                      
        interleaveListIndex = GetInterleaveListIndex(interleave_mode,systemAddress);
        DEBUG ((DEBUG_ERROR, "\nTranslateSAD: interleaveListIndex: %d", interleaveListIndex));

        InterleaveListCfg.Data = mCpuCsrAccess->ReadMcCpuCsr (socketId, mcId, mInterleaveListCfgMC[TA->SadId]);
        Status = GetTargetId(InterleaveListCfg.Data, interleaveListIndex, socketId, &targetId);
        if (Status != EFI_SUCCESS) {
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: FM TargetId is not matching NM TargetId\n"));
          return EFI_INVALID_PARAMETER;
        }

        Status = GetLogicalChId(systemAddress,targetId, DramRule, &logChId);
        Status = ChIdToMcCh(socketId,logChId,&mcId,&chId);

        if (mcId != TA->MemoryControllerId) {
          DEBUG ((DEBUG_ERROR, "\nTranslateSAD: FM iMCId is not matching NM iMCId\n"));
          return EFI_INVALID_PARAMETER;
        }

        TA->ChannelId = chId;
        DEBUG ((DEBUG_ERROR, "\nMemType is 2LM\n"));
        DEBUG ((DEBUG_ERROR, "\nTranslateSAD SUCCESSFUL!\n"));
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_INVALID_PARAMETER;
   
} // TranslateSAD

/**

   Translate Rank Address to Row/Dimm/Col

  @param TRANSLATED_ADDRESS  - ptr to Address Translation structure

Modifies:
  Following fields in ADDRESS_TRANSATION structure are updated:
  Row
  Col
  Bank

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
TranslateRankAddress (
  TRANSLATED_ADDRESS *TA
 )
{
  UINT8       node;
  //UINT8       bankXorEn;
  UINT8       Ddr4Dimm = 0;
  UINT64      LeftOver;
  UINT32      Col;
  UINT32      Row;
  UINT8       Bank;
  UINT8       BankGroup;
  UINT8       LowPos;
  UINT8       HighPos;
  UINT8       ChipId = 0;
  UINT64      RankAddress;

  DIMMMTR_0_MC_MAIN_STRUCT  dimmMtrReg;
  MCMTR_MC_MAIN_STRUCT      mcmtrReg;
  AMAP_MC_MAIN_STRUCT       amap;

  node  = TA->Node;
  
  if ((TA->MemType == MEM_TYPE_2LM) || (TA->MemType == MEM_TYPE_1LM)) {

    if (TA->MemType == MEM_TYPE_2LM) {
      RankAddress = TA->NmRankAddress;
    } else {
      RankAddress = TA->RankAddress;
    }
    //
    // Init. return values
    //
    TA->Row = (UINT32)-1;
    TA->Col = (UINT32)-1;
    TA->Bank = (UINT8)-1;

    //
    // Get Page Mode / lock-step config from MCMTR register
    //
    mcmtrReg.Data = mCpuCsrAccess->ReadMcCpuCsr( TA->SocketId, TA->MemoryControllerId, MCMTR_MC_MAIN_REG );

    //
    // Get the max row/col/bank bits from DIMMMTR register
    //
    dimmMtrReg.Data = mCpuCsrAccess->ReadCpuCsr(NODE_TO_SKT(node), NODECH_TO_SKTCH(node, TA->ChannelId), mDimmMtrOffset[TA->DimmSlot]);

    amap.Data = mCpuCsrAccess->ReadCpuCsr( TA->SocketId, NODECH_TO_SKTCH(node, TA->ChannelId), AMAP_MC_MAIN_REG);

    // exit is DIMM slot is not populated
    if(!dimmMtrReg.Bits.dimm_pop){
      return EFI_INVALID_PARAMETER;
    }

    // exit if rank is disabled
    if ((dimmMtrReg.Bits.rank_disable >> TA->PhysicalRankId) & 0x01) {
      return EFI_INVALID_PARAMETER;
    }

    Col = 0;

    Col |= (UINT32) BitFieldRead64(RankAddress, 3, 5);
  
    if (mcmtrReg.Bits.close_pg == 1) {
      BankGroup = (UINT8) BitFieldRead64(RankAddress, 6, 7);
      Bank = (UINT8) BitFieldRead64(RankAddress, 8, 9);
      if (mcmtrReg.Bits.bank_xor_enable == 1) {
        BankGroup ^= (UINT8) BitFieldRead64(RankAddress, 20,21);
        Bank ^= (UINT8) BitFieldRead64(RankAddress, 22, 22);
        Bank ^= (UINT8) (BitFieldRead64(RankAddress, 28, 28) << 1);
      }

      // calculate the column address
      Col |= (BitFieldRead64(RankAddress, 14, 14) << 3);
      Col |= (BitFieldRead64(RankAddress, 19, 19) << 4);
      Col |= (BitFieldRead64(RankAddress, 23, 27) << 5);

      
      // calculate the row address
      Row = (UINT32) BitFieldRead64(RankAddress, 15, 18);
      Row |= (BitFieldRead64(RankAddress, 20, 22) << 4);
      Row |= (BitFieldRead64(RankAddress, 28, 28) << 7);
      Row |= (BitFieldRead64(RankAddress, 10, 13) << 8);
      Row |= (BitFieldRead64(RankAddress, RIR_UNIT, RIR_UNIT) << 12); // update this code for lockstep

    } else {
      if (amap.Bits.fine_grain_bank_interleaving) BankGroup = (UINT8) BitFieldRead64(RankAddress, 6, 6);
      else BankGroup = (UINT8) BitFieldRead64(RankAddress, 13, 13);
      BankGroup |= (UINT8) (BitFieldRead64(RankAddress, 17, 17) << 1);
      if (mcmtrReg.Bits.bank_xor_enable) BankGroup ^= (UINT8) BitFieldRead64(RankAddress, 20, 21);
      Bank = (UINT8) BitFieldRead64(RankAddress, 18, 19);
      if (mcmtrReg.Bits.bank_xor_enable == 1) Bank ^= (UINT8) BitFieldRead64(RankAddress, 22, 23);

      Col |= (BitFieldRead64(RankAddress, 6+amap.Bits.fine_grain_bank_interleaving, 12+amap.Bits.fine_grain_bank_interleaving) << 3);

      // calculate the row address
      Row = (UINT32) BitFieldRead64(RankAddress, 14, 16);
      Row |= (UINT32) (BitFieldRead64(RankAddress, 20, 20) << 3);
      Row |= (UINT32) (BitFieldRead64(RankAddress, 28, 28) << 4);
      Row |= (UINT32) (BitFieldRead64(RankAddress, 21, 27) << 5);
      Row |= (UINT32) (BitFieldRead64(RankAddress, RIR_UNIT, RIR_UNIT) << 12); // update this code for lockstep
    }


    LowPos = RIR_UNIT + 1;
    HighPos = (UINT8)(LowPos + (dimmMtrReg.Bits.ra_width - 2));
    Row |= (BitFieldRead64(RankAddress, LowPos, HighPos) << 13);


    if (dimmMtrReg.Bits.ddr4_3dsnumranks_cs != 0) {
      LowPos = HighPos + 1;
      HighPos = LowPos + (UINT8) (dimmMtrReg.Bits.ddr4_3dsnumranks_cs - 1);
      ChipId = (UINT8) BitFieldRead64(RankAddress, LowPos, HighPos);
    } else {
      ChipId = 0;
    }

    LeftOver = RankAddress >> (HighPos+1);
    if (LeftOver) {
      return EFI_INVALID_PARAMETER;
    }

    Ddr4Dimm = (mSystemMemoryMap->DramType == SPD_TYPE_DDR4)? 1:0;

    TA->Col = Col;
    TA->Row = Row;
    TA->Bank = Bank;
    TA->BankGroup = BankGroup;
    TA->ChipId = ChipId;
    TA->DimmType = ddr4dimmType;
    if (Ddr4Dimm )  {
      TA->BankGroup = BankGroup;
    } else  {
      TA->BankGroup  = (UINT8)-1;
    }

    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: Col:%08lx",TA->Col));
    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: TA->Row:%08lx",TA->Row));
    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: TA->Bank:%08lx",TA->Bank));
    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: TA->BankGroup:%08lx",TA->BankGroup));
    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: TA->ChipId:%08lx\n",TA->ChipId));
    DEBUG ((DEBUG_ERROR, "\nTranslateRankAddress: TA->ChipSelect:%08lx\n",TA->ChipSelect));

  }
  return EFI_SUCCESS;
}

/**

   If Rank Sparing happened, set the RankSparing Bit in the RAS field
   update the bad Rank Id with the Spare Rank

  @param TRANSLATED_ADDRESS  - ptr to Address Translation structure

Modifies:
  Following fields in ADDRESS_TRANSATION structure are updated:
  Row
  Col
  Bank

  @retval EFI_SUCCESS / Error code


**/
VOID
RankSparingSupportForAddressTranslation (
  IN  PTRANSLATED_ADDRESS TA,
  IN  UINT8   TranslationMode     //Forward = 1 , Reverse = 2
)
{

  UINT8 Socket;
  UINT8 Node;
  UINT8 Mc;
  UINT8 Channel;
  UINT8 Rank, PhysicalRank;

  Socket  = TA->SocketId;
  Node    = TA->Node;
  Channel = TA->ChannelId;
  PhysicalRank    = TA->PhysicalRankId;
  Mc      = NODE_TO_MC(Node);

  Rank = (TA->DimmSlot * 4) + PhysicalRank; //Chip Select
  if (mSystemMemoryMap->RasModesEnabled & RK_SPARE) {
      if(TranslationMode == FORWARD_ADDRESS_TRANSLATION) {
          if (mMemrasS3Param.spareInUse[Node][Channel]) {
              if( mMemrasS3Param.oldLogicalRank[Node][Channel] == Rank )
              TA->ChipSelect = mMemrasS3Param.spareLogicalRank[Node][Channel];
              TA->PhysicalRankId = (TA->ChipSelect % MAX_RANK_DIMM);
              TA->RasModesApplied |= BIT0;
          }
      }
      else if (TranslationMode == REVERSE_ADDRESS_TRANSLATION) {
          if (mMemrasS3Param.spareInUse[Node][Channel]) {
              if( mMemrasS3Param.spareLogicalRank[Node][Channel] == Rank)
              TA->ChipSelect = mMemrasS3Param.oldLogicalRank[Node][Channel] ;
              TA->PhysicalRankId = (TA->ChipSelect % MAX_RANK_DIMM);
              TA->RasModesApplied |= BIT0;
          }
      }
      if (TA->RasModesApplied) {
        DEBUG ((DEBUG_ERROR, "\nRankSpareSupportForAddrTrans: TA->ChipSelect:%08lx",TA->ChipSelect));
        DEBUG ((DEBUG_ERROR, "\nRankSpareSupportForAddrTrans: TA->PhysicalRankId:%08lx",TA->PhysicalRankId));
        DEBUG ((DEBUG_ERROR, "\nRankSpareSupportForAddrTrans: TA->RasModesApplied:%08lx\n",TA->RasModesApplied));
      }
  }
  return;
}


VOID
ADDDCSupportForAddressTranslation (
  IN  PTRANSLATED_ADDRESS TA
)
{

  //
  // if ADDDC happened,  set the ADDDDC bit in the RAS field
  // update the buddyRank/buddyBank/Buddy BankGroup
  //

  UINT8 Socket;
  UINT8 Node;
  UINT8 Mc;
  UINT8 Channel;
  UINT8 Rank, PhysicalRank;
  UINT8 Bank;
  UINT8 BankGroup;
  UINT8 SktCh;
  UINT8 RegionNum;

  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT          AdddcCtrRegion;
  UINT32 AdddcRegionReg[MAX_REGION_SVL] = {ADDDC_REGION0_CONTROL_MC_MAIN_REG,ADDDC_REGION1_CONTROL_MC_MAIN_REG,ADDDC_REGION2_CONTROL_MC_MAIN_REG,ADDDC_REGION3_CONTROL_MC_MAIN_REG};

  Socket  = TA->SocketId;
  Node    = TA->Node;
  Channel = TA->ChannelId;
  PhysicalRank    = TA->PhysicalRankId;
  Bank    = TA->Bank;
  BankGroup = TA->BankGroup;
  Mc      = NODE_TO_MC(Node);
  SktCh   = NODECH_TO_SKTCH(Node, Channel);

  Rank = (TA->DimmSlot * 4) + PhysicalRank; //Chip Select

  DEBUG ((DEBUG_INFO, "\nADDDCSupportForAddrTrans\n"));
  for (RegionNum = 0; RegionNum< MAX_REGION_SVL; RegionNum++ ) {
    AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcRegionReg[RegionNum]);
    if (AdddcCtrRegion.Bits.region_enable) {  //adddc region active on this channel
      DEBUG ((DEBUG_INFO, "\nADDDCSupportForAddrTrans: Region %d enabled\n",RegionNum ));
        if  ( (Rank == AdddcCtrRegion.Bits.failed_cs ) ) {
          if (AdddcCtrRegion.Bits.region_size == ADDDC_REGION_SIZE_RANK) {
             TA->LockStepRank  = ( (UINT8)AdddcCtrRegion.Bits.nonfailed_cs & 0x3) ;
             TA->LockStepPhysicalRank = (TA->LockStepRank % MAX_RANK_DIMM);
             TA->RasModesApplied |= BIT1; // Rank VLS
             break;
          } else if (AdddcCtrRegion.Bits.region_size == ADDDC_REGION_SIZE_BANK) {
            if  ( (Bank == AdddcCtrRegion.Bits.failed_ba)&&(BankGroup == AdddcCtrRegion.Bits.failed_bg) ) {
              TA->LockStepRank = (UINT8)AdddcCtrRegion.Bits.nonfailed_cs ;
              TA->LockStepPhysicalRank = (TA->LockStepRank % MAX_RANK_DIMM);
              TA->LockStepBank           = (UINT8)AdddcCtrRegion.Bits.nonfailed_ba;
              TA->LockStepBG      = (UINT8)AdddcCtrRegion.Bits.nonfailed_bg;
              TA->RasModesApplied |= BIT2;  //Bank VLS
              break;
            }
          }
        } if  ( (AdddcCtrRegion.Bits.nonfailed_cs == Rank) ) {
          if (AdddcCtrRegion.Bits.region_size == ADDDC_REGION_SIZE_RANK) {
             TA->LockStepRank  = (UINT8)AdddcCtrRegion.Bits.failed_cs  ;
             TA->LockStepPhysicalRank = (TA->LockStepRank % MAX_RANK_DIMM);
             TA->RasModesApplied |= BIT1; // Rank VLS
             break;
          } else if (AdddcCtrRegion.Bits.region_size == ADDDC_REGION_SIZE_BANK) {
            if  ( (Bank == AdddcCtrRegion.Bits.nonfailed_ba)&&(BankGroup == AdddcCtrRegion.Bits.nonfailed_bg) ) {
              TA->LockStepRank = (UINT8)AdddcCtrRegion.Bits.failed_cs  ;
              TA->LockStepPhysicalRank = (TA->LockStepRank % MAX_RANK_DIMM);
              TA->LockStepBank           = (UINT8)AdddcCtrRegion.Bits.failed_ba;
              TA->LockStepBG      = (UINT8)AdddcCtrRegion.Bits.failed_bg;
              TA->RasModesApplied |= BIT2;  //Bank VLS
              break;
            }
          }
        }

    }  //Region Enable
  } // Region loop
  if (TA->RasModesApplied & (BIT1|BIT2)) {
    DEBUG ((DEBUG_ERROR, "\nADDDCSupportForAddrTrans: TA->LockStepRank:%08lx",TA->LockStepRank));
    DEBUG ((DEBUG_ERROR, "\nADDDCSupportForAddrTrans: TA->LockStepPhysicalRank:%08lx",TA->LockStepPhysicalRank));
    DEBUG ((DEBUG_ERROR, "\nADDDCSupportForAddrTrans: TA->LockStepBank:%08lx",TA->LockStepBank));
    DEBUG ((DEBUG_ERROR, "\nADDDCSupportForAddrTrans: TA->LockStepBG:%08lx\n",TA->LockStepBG));
    DEBUG ((DEBUG_ERROR, "\nADDDCSupportForAddrTrans: TA->RasModesApplied:%08lx\n",TA->RasModesApplied));
    }
  return;
}

/**

   Some RAS events like sparing need to be considered while doing address translation

    @param SystemAddress
    @param TranslatedAddress

    @retval NONE


**/
VOID
ApplyRASEventsForFAT (
  IN  PTRANSLATED_ADDRESS TA
)

{
  // ApplyRankSparing
  RankSparingSupportForAddressTranslation (TA , FORWARD_ADDRESS_TRANSLATION);

  // ApplyADDDC
  ADDDCSupportForAddressTranslation (TA);
  return;
}


/**

   Translate DDR4 System Address to DIMM Address

    @param SystemAddress
    @param TranslatedAddress

    @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
TranslateSystemAddress (
  IN UINTN SystemAddress,
  OUT PTRANSLATED_ADDRESS TA
) 
{

  EFI_STATUS  Status;
  
  // Initialize  the fields in TA with -1
  TA->SystemAddress      = (UINT64)SystemAddress;
  TA->SocketId           = (UINT8)-1;
  TA->MemoryControllerId = (UINT8)-1;
  TA->ChannelId          = (UINT8)-1;
  TA->DimmSlot           = (UINT8)-1;
  TA->DimmRank           = (UINT8)-1;
  TA->Row                = (UINT32)-1;
  TA->Col                = (UINT32)-1;
  TA->Bank               = (UINT8)-1;
  TA->BankGroup          = (UINT8)-1;
  TA->ChipSelect         = (UINT8)-1;

  Status = TranslateSAD(TA);
  if(Status == EFI_SUCCESS)
    Status = TranslateTAD(TA);

 if(Status == EFI_SUCCESS)
    Status = TranslateRIR(TA);

  if (Status == EFI_SUCCESS)
    Status = TranslateRankAddress(TA);

  if (Status == EFI_SUCCESS) {
    TA->DimmType = ddr4dimmType;

    // RAS support for FAT
    ApplyRASEventsForFAT(TA);
  }


  return Status;

}

/**

   Translate System Address to DIMM Address

    @param SystemAddress
    @param TranslatedAddress

    @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
EFIAPI
SystemAddressToDimmAddress (
  IN  UINT64  SystemAddress,
  OUT PTRANSLATED_ADDRESS TA
 )
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_ERROR, "\nForward Address Translation start: 0x%lx\n", SystemAddress));
  Status = TranslateSystemAddress (SystemAddress, TA);
  
  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "\nForward Address Translation Successful!!\n"));
  }
  return Status;
  
}



//
// Reverse Address Translation functions
//


VOID
GetMemoryType (
  IN  OUT PTRANSLATED_ADDRESS TA
)
{
  UINT8                             SktId, McId, ChId, DimmId, NodeId;
  MCDDRTCFG_MC_MAIN_STRUCT          McDdrtCfg;
  MODE_M2MEM_MAIN_STRUCT            M2MemMode;
  BOOLEAN                           DdrtDimm = FALSE;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  ChId = TA->ChannelId;
  DimmId = TA->DimmSlot;

  TA->Node = NodeId = SKT_TO_NODE(SktId,McId);
  
  M2MemMode.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, MODE_M2MEM_MAIN_REG);
  if (M2MemMode.Bits.nmcaching == 1) {
    TA->MemType = MEM_TYPE_2LM;
    DEBUG ((DEBUG_ERROR, "\nMemType is 2LM\n"));
  } else {

    McDdrtCfg.Data = mCpuCsrAccess->ReadCpuCsr (SktId, NODECH_TO_SKTCH(NodeId, ChId), MCDDRTCFG_MC_MAIN_REG);
  
    if ( (DimmId == 0) && (McDdrtCfg.Bits.slot0 == 1)) {
      DEBUG ((DEBUG_ERROR, "\nDimmId %x is DDRT\n", DimmId));
      DdrtDimm = TRUE;
    }

    if ((DimmId == 1) && (McDdrtCfg.Bits.slot1 == 1)) {
      DEBUG ((DEBUG_ERROR, "\nDimmId %x is DDRT\n", DimmId));
      DdrtDimm = TRUE;
    }

    if (DdrtDimm) {
      TA->MemType = MEM_TYPE_PMEM | MEM_TYPE_BLK_WINDOW;
      DEBUG ((DEBUG_ERROR, "\nMemType is BLK or PMEM\n"));
    } else {
      TA->MemType = MEM_TYPE_1LM;
      DEBUG ((DEBUG_ERROR, "\nMemType is 1LM\n"));
    }
  
  }

}

 
 
EFI_STATUS
GetRankAddress (
  IN PTRANSLATED_ADDRESS TA
 ) 
{
  UINT8   SktId, McId, ChId, DimmId, Bank, BankGroup,SktCh;
  UINT8   TempBankGroup,TempBank;
  UINT8   FineGraniBankInterleave = 0;
  UINT8   LowPos, HighPos;
  UINT32  Row,Col;
  UINT64  RankAddress;

  DIMMMTR_0_MC_MAIN_STRUCT  DimmMtrReg;
  MCMTR_MC_MAIN_STRUCT      McMtrReg;
  AMAP_MC_MAIN_STRUCT       AmapReg;
  
  LowPos  = 30;
  HighPos = 31;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  Bank = TA->Bank;
  BankGroup = TA->BankGroup;
  Row = TA->Row;
  Col = TA->Col;

  TempBankGroup = 0;
  TempBank = 0;
  RankAddress = 0;
 
    
  if ((TA->MemType == MEM_TYPE_2LM) || (TA->MemType == MEM_TYPE_1LM)) {
    
    if (TA->MemType == MEM_TYPE_2LM) {
      ChId = TA->NmChannelId;
      DimmId = TA->NmDimmSlot;
    } else {
      ChId = TA->ChannelId;
      DimmId = TA->DimmSlot;
    }
 
     SktCh = (McId * MAX_MC_CH) + ChId;
    //
    // Read MCMTR register for Page Policy and Bank_XOR Enable
    //
    McMtrReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, MCMTR_MC_MAIN_REG);

    //
    // Read DIMM MTR Register for MaxRowBits, MaxColBits
    //
    DimmMtrReg.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh,  mDimmMtrOffset[DimmId]);


    //
    // Read AMAP register
    //
    AmapReg.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh, AMAP_MC_MAIN_REG);
    FineGraniBankInterleave = (UINT8)AmapReg.Bits.fine_grain_bank_interleaving;


    if (McMtrReg.Bits.close_pg == 1) {

      // check if the bank_xor_enable
      if (McMtrReg.Bits.bank_xor_enable == 1) {
        TempBankGroup = (UINT8)(BitFieldRead32 (Row, 4, 5));
        TempBank = (UINT8)(BitFieldRead32 (Row, 6, 7));
      }

      TempBankGroup ^= BankGroup;
      TempBank ^= Bank;

  
      // Put BankGroup and Bank bits
      RankAddress = BitFieldWrite64 (RankAddress, 6, 7, TempBankGroup);
      RankAddress = BitFieldWrite64 (RankAddress, 8, 9, TempBank);


      //Put the col bits in Rank Address
      RankAddress = BitFieldWrite64 (RankAddress, 3, 5, BitFieldRead32(Col, 0, 2));
      RankAddress = BitFieldWrite64 (RankAddress, 14, 14, BitFieldRead32(Col, 3, 3));
      RankAddress = BitFieldWrite64 (RankAddress, 19, 19, BitFieldRead32(Col, 4, 4));
      RankAddress = BitFieldWrite64 (RankAddress, 23, 27, BitFieldRead32(Col, 5, 9));

      // Put the Row Bits in Rank Address
      RankAddress = BitFieldWrite64 (RankAddress, 15, 18, BitFieldRead32(Row, 0, 3));
      RankAddress = BitFieldWrite64 (RankAddress, 20, 22, BitFieldRead32(Row, 4, 6));
      RankAddress = BitFieldWrite64 (RankAddress, 28, 28, BitFieldRead32(Row, 7, 7));
      RankAddress = BitFieldWrite64 (RankAddress, 10, 13, BitFieldRead32(Row, 8, 11));
      RankAddress = BitFieldWrite64 (RankAddress, 29, 29, BitFieldRead32(Row, 12, 12));

    } else {  // open page

        // check if the bank_xor_enable
      if (McMtrReg.Bits.bank_xor_enable == 1) {
        TempBankGroup = (UINT8)(BitFieldRead32 (Row, 3, 3) | (BitFieldRead32 (Row, 5, 5)  << 1));
        TempBank = (UINT8)(BitFieldRead32 (Row, 6, 7));
      }
      TempBankGroup ^= BankGroup;
      TempBank ^= Bank;

      // Put the BankGroup Bits
      if (FineGraniBankInterleave) {
        RankAddress = BitFieldWrite64 (RankAddress, 6, 6, BitFieldRead32(TempBankGroup, 0, 0));
      }else {
        RankAddress = BitFieldWrite64 (RankAddress, 13, 13, BitFieldRead32(TempBankGroup, 0, 0));
      }
      RankAddress = BitFieldWrite64 (RankAddress, 17, 17, BitFieldRead32(TempBankGroup, 1, 1));

      //Put the Bank Bits
      RankAddress = BitFieldWrite64 (RankAddress, 18, 19, TempBank);

      // Put the Col Bits
      RankAddress = BitFieldWrite64 (RankAddress, 3, 5, BitFieldRead32(Col, 0, 2));
      RankAddress = BitFieldWrite64 (RankAddress, 6 + FineGraniBankInterleave , 12 + FineGraniBankInterleave, BitFieldRead32(Col, 3, 9));

      // Put the Row Bits in Rank Address
      RankAddress = BitFieldWrite64 (RankAddress, 14, 16, BitFieldRead32(Row, 0, 2));
      RankAddress = BitFieldWrite64 (RankAddress, 20, 20, BitFieldRead32(Row, 3, 3));
      RankAddress = BitFieldWrite64 (RankAddress, 28, 28, BitFieldRead32(Row, 4, 4));
      RankAddress = BitFieldWrite64 (RankAddress, 21, 27, BitFieldRead32(Row, 5, 11));
      RankAddress = BitFieldWrite64 (RankAddress, 29, 29, BitFieldRead32(Row, 12, 12));
    }

    // The higher row bits are at same position for both Open Page and Close Page Policy
    if (DimmMtrReg.Bits.ra_width != 0) {
      LowPos = 30;
      HighPos = (UINT8) (30 + DimmMtrReg.Bits.ra_width - 1);
      RankAddress = BitFieldWrite64 (RankAddress, LowPos, HighPos, BitFieldRead32(Row, 13, 17));
    }

    // Need to add the code for CBits for 3DS LRDIMM
    if (DimmMtrReg.Bits.ddr4_3dsnumranks_cs != 0) {
      LowPos = HighPos+1;
      HighPos = (UINT8) (LowPos + DimmMtrReg.Bits.ddr4_3dsnumranks_cs - 1);
      RankAddress = BitFieldWrite64 (RankAddress, LowPos, HighPos, TA->ChipId);
    }

    if (TA->MemType == MEM_TYPE_2LM) {
      TA->NmRankAddress = RankAddress;
    } else {
      TA->RankAddress = RankAddress;
    }
  }

  if (TA->MemType != MEM_TYPE_1LM) {
    TA->RankAddress = TA->DPA << 6;
  }

  DEBUG ((DEBUG_ERROR, "\nGetRankAddress: RankAddress:%08lx",TA->RankAddress));
  return EFI_SUCCESS;
  
}


EFI_STATUS
GetChannelAddress (
  IN PTRANSLATED_ADDRESS TA
 )
{
  UINT8   SktId, McId, ChId, SktCh, RirIndex, Index, ShiftBit, RIRWays, ChipSelect;
  UINT64  RankAddress, ChannelAddress, LowerAddrBits, RIRLimit, PrevLimit, RIROffset;
  BOOLEAN Match;
    
  RIRWAYNESSLIMIT_0_MC_MAIN_STRUCT  RIRWaynessLimit;
  RIRILV0OFFSET_0_MC_MAIN_STRUCT    RIRIlv0Offset;
  FMRIRWAYNESSLIMIT_0_MC_2LM_STRUCT imcFMRIRWaynessLimit;
  FMRIRILV0OFFSET_0_MC_2LM_STRUCT   imcFMRIRIlv0Offset;
  MCMTR_MC_MAIN_STRUCT              McMtrReg;
  
  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  ChId = TA->ChannelId;
  RankAddress = TA->RankAddress;
  ChipSelect = (TA->DimmSlot * 4) + TA->PhysicalRankId;
    
  //Read McMtr Register to get the page mode to determinte the no. of Lower order bits that are not toched by interleave
  McMtrReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, MCMTR_MC_MAIN_REG);

  if (McMtrReg.Bits.close_pg) {
    ShiftBit = 6;
  } else {
    ShiftBit = 13;
  }


  if ((TA->MemType == MEM_TYPE_2LM) || (TA->MemType == MEM_TYPE_1LM)) {
    
    if (TA->MemType == MEM_TYPE_2LM) {
      ChId = TA->NmChannelId;
      RankAddress = TA->NmRankAddress;
      ChipSelect =  (TA->NmDimmSlot * 4) + TA->NmPhysicalRankId;
      DEBUG ((DEBUG_ERROR, "\n\nIn GetChannelAddress() for 2LM NMRankAddress\n"));
    } 

    if (TA->MemType == MEM_TYPE_1LM) {
       DEBUG ((DEBUG_ERROR, "\n\nIn GetChannelAddress() for 1LM\n"));
    }

    ChannelAddress = (UINT64)-1;
    Match = FALSE;
    SktCh = (McId * MAX_MC_CH) + ChId;

    DEBUG ((DEBUG_ERROR, "SktId is 0x%x\n",SktId));
    DEBUG ((DEBUG_ERROR, "McId is 0x%x\n",McId));
    DEBUG ((DEBUG_ERROR, "ChId is 0x%x\n",ChId));
    DEBUG ((DEBUG_ERROR, "SktCh is 0x%x\n",SktCh));
    DEBUG ((DEBUG_ERROR, "TA->DimmSlot is 0x%x\n",TA->DimmSlot));
    DEBUG ((DEBUG_ERROR, "ChipSelect is 0x%x\n",ChipSelect));
    
    PrevLimit = 0;

    for (RirIndex = 0; RirIndex < MAX_RIR; RirIndex++) {
      RIRWaynessLimit.Data = mCpuCsrAccess->ReadCpuCsr (SktId, SktCh, mRIRWaynessLimit[RirIndex]);
      RIRLimit = (((UINT64)RIRWaynessLimit.Bits.rir_limit + 1) << RIR_UNIT) - 1;
      RIRWays = 1 << RIRWaynessLimit.Bits.rir_way;

      DEBUG ((DEBUG_ERROR, "RIRWays is 0x%x\n",RIRWays));
      DEBUG ((DEBUG_ERROR, "RIRLimit is 0x%lx\n",RIRLimit));
      DEBUG ((DEBUG_ERROR, "PrevLimit is 0x%lx\n",PrevLimit));

      for (Index = 0; Index < RIRWays; Index++) {

        RIRIlv0Offset.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh, mRIRIlvOffset[RirIndex][Index]);

        DEBUG ((DEBUG_ERROR, "RIRIlv0Offset.Data is 0x%x\n",RIRIlv0Offset.Data));
     
        if (RIRIlv0Offset.Bits.rir_rnk_tgt0 == ChipSelect) {
          DEBUG ((DEBUG_ERROR, "RirIndex is 0x%x\n",RirIndex));
          DEBUG ((DEBUG_ERROR, "Index is 0x%x\n",Index));
          RIROffset = ((UINT64)(RIRIlv0Offset.Bits.rir_offset0) << 26);
          DEBUG ((DEBUG_ERROR, "RIROffset is 0x%lx\n",RIROffset));
          ChannelAddress = RankAddress + RIROffset;
          LowerAddrBits = BitFieldRead64(RankAddress, 0, ShiftBit-1);
          ChannelAddress = ChannelAddress >> ShiftBit;
          ChannelAddress = (ChannelAddress * RIRWays) | Index;
          ChannelAddress = (ChannelAddress << ShiftBit) | LowerAddrBits;
          DEBUG ((DEBUG_ERROR, "ChannelAddress is 0x%lx\n",ChannelAddress));

          if (((PrevLimit == 0) && (PrevLimit == ChannelAddress) && (ChannelAddress <= RIRLimit)) ||
          ((PrevLimit < ChannelAddress) && (ChannelAddress <= RIRLimit))) {
            DEBUG ((DEBUG_ERROR, "ChannelAddress Found 0x%lx\n",ChannelAddress));
            Match = TRUE;
            break;
          }  // if Address within the limit
        } // if Physical Rank Target match
      }  // for Rir Ways

      if (Match == TRUE)
        break;
      else
        PrevLimit = RIRLimit;
    } // for RIR Index

    if (RirIndex == MAX_RIR) {
      return EFI_INVALID_PARAMETER; 
    } else {
      if (TA->MemType == MEM_TYPE_2LM) {
        TA->NmChannelAddress = ChannelAddress;
      } else {
        TA->ChannelAddress = ChannelAddress;
      }
    }
  }

  if (TA->MemType != MEM_TYPE_1LM) {
    PrevLimit = 0;
    SktCh = (McId * MAX_MC_CH) + ChId;
    ChannelAddress = (UINT64)-1;
    Match = FALSE;

    for (RirIndex = 0; RirIndex < MAX_RIR; RirIndex++) {

      imcFMRIRWaynessLimit.Data = mCpuCsrAccess->ReadCpuCsr (SktId, SktCh, mFMRIRWaynessLimit[RirIndex]);
      RIRLimit = (((UINT64)imcFMRIRWaynessLimit.Bits.rir_limit + 1) << RIR_UNIT) - 1;
      RIRWays = 1 << imcFMRIRWaynessLimit.Bits.rir_way;

      for (Index = 0; Index < RIRWays; Index++) {
        imcFMRIRIlv0Offset.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh, mFMRIRIlvOffset[RirIndex][Index]);
        if (imcFMRIRIlv0Offset.Bits.rir_rnk_tgt0 == TA->ChipSelect) {
          DEBUG ((DEBUG_ERROR, "RirIndex is 0x%x\n",RirIndex));
          DEBUG ((DEBUG_ERROR, "Index is 0x%x\n",Index));
          DEBUG ((DEBUG_ERROR, "RIRWays is 0x%x\n",RIRWays));
          RIROffset = ((UINT64)(imcFMRIRIlv0Offset.Bits.rir_offset0) << 26);
          DEBUG ((DEBUG_ERROR, "RIROffset is 0x%x\n",RIROffset));
          ChannelAddress = RankAddress + RIROffset;
          LowerAddrBits = BitFieldRead64(RankAddress, 0, ShiftBit-1);
          ChannelAddress = ChannelAddress >> ShiftBit;
          ChannelAddress = (ChannelAddress * RIRWays) | Index;
          ChannelAddress = (ChannelAddress << ShiftBit) | LowerAddrBits;
          DEBUG ((DEBUG_ERROR, "ChannelAddress is 0x%x\n",ChannelAddress));

          if ((PrevLimit < ChannelAddress) && (ChannelAddress <= RIRLimit)) {
            DEBUG ((DEBUG_ERROR, "ChannelAddress Found 0x%lx\n",ChannelAddress));
            Match = TRUE;
            break;
          }  // if Address within the limit
        } // if Physical Rank Target match
      }  // for Rir Ways

      if (Match == TRUE) {
        break;
      } else {
        PrevLimit = RIRLimit;
      }
    } // for RIR Index

    if (RirIndex == MAX_RIR) 
      return EFI_INVALID_PARAMETER; 
    else 
      TA->ChannelAddress = ChannelAddress;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
TadId2SadId (
  IN PTRANSLATED_ADDRESS TA
)
{
  UINT8 SktId, McId, SadIndex;
  UINT64 SystemAddress, SadLimit, PrevLimit;
  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT   DramRule;
  SAD2TAD_M2MEM_MAIN_STRUCT          Sad2TadReg;
  BOOLEAN MatchFound = FALSE;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  SystemAddress = TA->SystemAddress;

  DEBUG ((DEBUG_ERROR, "\nIn TadId2SadId()\n"));
  DEBUG ((DEBUG_ERROR, "TadId = 0x%x\n",TA->TadId));

  PrevLimit = 0;

  for (SadIndex = 0; SadIndex < SAD_RULES; SadIndex++) {

    DEBUG ((DEBUG_ERROR, "SadIndex = 0x%x\n", SadIndex));

    DramRule.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mDramRuleRegOffset[SadIndex]);

    if ((DramRule.Bits.rule_enable == 0) || (DramRule.Bits.attr != 0)) 
      continue;

    SadLimit = (((UINT64)DramRule.Bits.limit + 1) << SAD_UNIT) - 1;
    DEBUG ((DEBUG_ERROR, "SadLimit = 0x%x\n", SadLimit));

    if ((SadIndex != 0) && (PrevLimit == SadLimit))
      break;

    DEBUG ((DEBUG_ERROR, "PrevLimit = 0x%x\n", PrevLimit));
    DEBUG ((DEBUG_ERROR, "SystemAddress = 0x%x\n", SystemAddress));

    if (((PrevLimit == 0) && (PrevLimit == SystemAddress) && (SystemAddress <= SadLimit)) ||
          ((PrevLimit < SystemAddress) && (SystemAddress <= SadLimit))) {
      
      Sad2TadReg.Data = 0;
      Sad2TadReg.Bits.sad2tadrden = 1;
      Sad2TadReg.Bits.sad2tadwren = 0;
      Sad2TadReg.Bits.sadid = SadIndex;

      mCpuCsrAccess->WriteMcCpuCsr (SktId, McId, SAD2TAD_M2MEM_MAIN_REG, Sad2TadReg.Data);
      Sad2TadReg.Data = mCpuCsrAccess->ReadMcCpuCsr (SktId, McId, SAD2TAD_M2MEM_MAIN_REG);

      if (Sad2TadReg.Bits.sadid == SadIndex) {
        if (((Sad2TadReg.Bits.ddr4tadid == TA->TadId) && (TA->MemType == MEM_TYPE_1LM)) || ((Sad2TadReg.Bits.ddrttadid == TA->TadId) && (TA->MemType != MEM_TYPE_1LM))) {
          TA->SadId = SadIndex;
          MatchFound = TRUE;
          DEBUG ((DEBUG_ERROR, "Sad 0x%x found for the TadId 0x%x\n", SadIndex,TA->TadId));
          break;
        }
      }
    }
    PrevLimit = SadLimit;

  }

  if (MatchFound == TRUE)
    return EFI_SUCCESS;
  else
    return EFI_INVALID_PARAMETER;

}



EFI_STATUS
McChToLogChId (
IN PTRANSLATED_ADDRESS TA
)
{

  MC_ROUTE_TABLE_CHA_PMA_STRUCT McRouteTable;
  UINT8 SktId, McId, ChId, LogChId;
  BOOLEAN MatchFound = FALSE;
  
  LogChId = 0;


  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  ChId = TA->ChannelId;

  McRouteTable.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, MC_ROUTE_TABLE_CHA_PMA_REG);

  DEBUG ((DEBUG_ERROR, "\n In McChToLogChId()\n"));

  if ((McId == McRouteTable.Bits.ringid0) && (ChId == McRouteTable.Bits.channelid0)){
    LogChId = 0;
    MatchFound = TRUE;
  }
  if ((McId == McRouteTable.Bits.ringid1) && (ChId == McRouteTable.Bits.channelid1)){
    LogChId = 1;
    MatchFound = TRUE;
  } 
  if ((McId == McRouteTable.Bits.ringid2) && (ChId == McRouteTable.Bits.channelid2)){
    LogChId = 2;
    MatchFound = TRUE;
  } 
  if ((McId == McRouteTable.Bits.ringid3) && (ChId == McRouteTable.Bits.channelid3)){
    LogChId = 3;
    MatchFound = TRUE;
  } 
  if ((McId == McRouteTable.Bits.ringid4) && (ChId == McRouteTable.Bits.channelid4)){
    LogChId = 4;
    MatchFound = TRUE;
  } 
  if ((McId == McRouteTable.Bits.ringid5) && (ChId == McRouteTable.Bits.channelid5)){
    LogChId = 5;
    MatchFound = TRUE;
  } 

  if (MatchFound) {
    TA->LogChannelId = LogChId;
    DEBUG ((DEBUG_ERROR, "Logical ChannelId is 0x%x\n",TA->LogChannelId));
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
  

}

EFI_STATUS
PatchInterleaveListIndex (
  IN  PTRANSLATED_ADDRESS TA,
  IN  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT DramRule
)
{
  
  UINT8 SktId, McId, LogChId, Mod3, Mod3AsMode2, Mod3Mode, ModResult, ModGran, ChMod, TargetId, Index, InterleaveMode, InterleaveGran, AddressMod;
  UINT64 SystemAddress, LowerAddrBits;
  UINT32 TempTargetId;
  BOOLEAN MatchFound = FALSE;
  INTERLEAVE_LIST_CFG_0_CHABC_SAD_STRUCT InterleaveList;

  InterleaveGran = 6;
  TargetId = 0;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  LogChId = TA->LogChannelId;
  SystemAddress = TA->SystemAddress;

  Mod3 = (UINT8)DramRule.Bits.mod3;
  Mod3AsMode2 = (UINT8)DramRule.Bits.mod3_asamod2;
  Mod3Mode = (UINT8)DramRule.Bits.mod3_mode;
  InterleaveMode = (UINT8)DramRule.Bits.interleave_mode;
  ChMod = 0;
  ModResult = 0;
  ModGran = 0;

  DEBUG ((DEBUG_ERROR, "Mod3 is 0x%x\n",Mod3));
  DEBUG ((DEBUG_ERROR, "Mod3AsMode2 is 0x%x\n",Mod3AsMode2));
  DEBUG ((DEBUG_ERROR, "Mod3Mode is 0x%x\n",Mod3Mode));
  DEBUG ((DEBUG_ERROR, "InterleaveMode is 0x%x\n",InterleaveMode));

  switch (InterleaveMode) {
    case 0:
      InterleaveGran = 6;
      break;
    case 1:
      InterleaveGran = 8;
      break;
    case 2:
      InterleaveGran = 12;
      break;
    case 3:
      InterleaveGran = 30;
      break;
  }

  
  if (Mod3 ==  1) {
    switch (Mod3AsMode2) {
      case 0:
        ChMod = 3;
        ModResult = LogChId >> 1;
        break;

      case 1:
        ChMod = 2;
        ModResult = LogChId >> 1;
        break;

      case 2:
        ChMod = 2;
        ModResult = LogChId >> 2;
        break;

      case 3:
        ChMod = 2;
        ModResult = LogChId >> 2;
        break;
    }

    switch (Mod3Mode) {
      case 0:
        ModGran = 6;
        break;

      case 1:
        ModGran = 8;
        break;

      case 2:
       ModGran = 12;
        break;     
    }
  }

  DEBUG ((DEBUG_ERROR, "InterleaveGran is 0x%x\n",InterleaveGran));
  DEBUG ((DEBUG_ERROR, "ModGran is 0x%x\n",ModGran));
  DEBUG ((DEBUG_ERROR, "ChMod is 0x%x\n",ChMod));
  DEBUG ((DEBUG_ERROR, "ModResult is 0x%x\n",ModResult));

  if (TA->MemType != MEM_TYPE_2LM) {
    InterleaveList.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mInterleaveListRegOffset[TA->SadId]);
  } else {
    InterleaveList.Data = mCpuCsrAccess->ReadMcCpuCsr (SktId, McId, mInterleaveListCfgMC[TA->SadId]);
  }
  DEBUG ((DEBUG_ERROR, "InterleaveList.Data is 0x%x\n",InterleaveList.Data));

  for (Index = 0; Index < 8; Index++) {
    TempTargetId = InterleaveList.Data;
    TempTargetId = TempTargetId >> (Index * 4);
    TempTargetId = TempTargetId & 0xF;
    DEBUG ((DEBUG_ERROR, "TempTargetId is 0x%x\n",TempTargetId));
    
    if ((TempTargetId & BIT3) != 0) {       // Local InterleaveList
      DEBUG ((DEBUG_ERROR, "Local InterleaveList Tgt\n"));
      if (Mod3 == 0) {
        TempTargetId = TempTargetId & 7;
        if (LogChId == TempTargetId) {
          TargetId = (UINT8)TempTargetId;
          DEBUG ((DEBUG_ERROR, "Mod3 = 0, TargetId match is 0x%x\n",TempTargetId));
          MatchFound = TRUE;
        }
      } else {
        if (((UINT8)TempTargetId & 1) == (LogChId & 1)) {
          TargetId = (UINT8)(TempTargetId & 7);
          DEBUG ((DEBUG_ERROR, "Mod3 != 0, TargetId match is 0x%x\n",TempTargetId));
          MatchFound = TRUE;
        }
      }
    }

    if (MatchFound) {
      LowerAddrBits = BitFieldRead64(SystemAddress, 0, InterleaveGran-1); 
      SystemAddress = SystemAddress >> InterleaveGran;
      SystemAddress = SystemAddress | Index;
      SystemAddress = (SystemAddress << InterleaveGran) | LowerAddrBits;
      DEBUG ((DEBUG_ERROR, "SystemAddress is 0x%x\n",SystemAddress));

      if (Mod3 == 1) {
        AddressMod = (SystemAddress >> ModGran) % ChMod;
        DEBUG ((DEBUG_ERROR, "AddressMod is 0x%x\n",AddressMod));
        if (AddressMod == ModResult) {
          DEBUG ((DEBUG_ERROR, "AddressMod  and ModResult match is 0x%x\n",AddressMod));
          MatchFound = TRUE;
        }
        else {
          MatchFound = FALSE;
        }
      }
    }

    if (MatchFound == TRUE) {
      TA->SystemAddress = SystemAddress;
      return EFI_SUCCESS;
    } 
  }

  return EFI_INVALID_PARAMETER;
}



EFI_STATUS
ReverseSad (
  IN PTRANSLATED_ADDRESS TA 
 )
{
  UINT64 SystemAddress, SadBase, SadLimit, PrevSadLimit;
  UINT8 SadIndex, SktId, McId, ChId, SadId;

  MMIO_RULE_CFG_0_N0_CHABC_SAD1_STRUCT          MmioRuleLo1;
  MMIO_RULE_CFG_0_N1_CHABC_SAD1_STRUCT          MmioRuleLo2;
  MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT    MmiohInterleaveRule0;
  MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT    MmiohInterleaveRule1;
  MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_STRUCT MmiohNonInterleaveRule0;
  MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_STRUCT MmiohNonInterleaveRule1;
  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT              DramRule,PrevDramRule;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  ChId = TA->ChannelId;
  SadId = TA->SadId;
  SystemAddress = TA->SystemAddress;
  
  // check if the Address falls into MMIO_L SAD
  for (SadIndex = 0; SadIndex < MAX_MMIIOL_SADRULES; SadIndex++) {
     MmioRuleLo1.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mMmiolRuleOffset[SadIndex][0]);

     if (MmioRuleLo1.Bits.rule_enable != 1)
       continue;

     MmioRuleLo2.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mMmiolRuleOffset[SadIndex][1]);

     SadBase = ((UINT64)MmioRuleLo1.Bits.baseaddress << 26) || ((UINT64)MmioRuleLo1.Bits.baseaddress_lsb_bits << 24) || ((UINT64)MmioRuleLo2.Bits.baseaddress_vlsb_bits << 22);
     SadLimit = (((UINT64)MmioRuleLo1.Bits.limitaddress) || ((UINT64)MmioRuleLo2.Bits.limitaddress << 6)) << 26;
     SadLimit = SadLimit || ((UINT64)MmioRuleLo1.Bits.limitaddress_lsb_bits << 24) || ((UINT64)MmioRuleLo2.Bits.limitaddress_vlsb_bits << 22);

     if ((SadBase <= SystemAddress) && (SystemAddress <= SadLimit)) {
       DEBUG ((DEBUG_ERROR, "\nERROR: Address falls into MMIO_L SAD\n"));
       return EFI_INVALID_PARAMETER;
     }
  }

  // Check if the address falls into MMIO_H SAD (both Interleave Rule and nonInterleave rule)
  MmiohInterleaveRule0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, MMIOH_INTERLEAVE_RULE_N0_CHABC_SAD1_REG);
  MmiohInterleaveRule1.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, MMIOH_INTERLEAVE_RULE_N1_CHABC_SAD1_REG);
  SadBase = ((UINT64)MmiohInterleaveRule0.Bits.baseaddress << 26) || ((UINT64)MmiohInterleaveRule0.Bits.baseaddress_lsb_bits << 24);
  SadLimit = (((UINT64)MmiohInterleaveRule0.Bits.limitaddress) || ((UINT64)MmiohInterleaveRule1.Bits.limitaddress << 6)) << 26;
  SadLimit = SadLimit || ((UINT64)MmiohInterleaveRule0.Bits.limitaddress_lsb_bits << 24);

  if ( (MmiohInterleaveRule0.Bits.rule_enable) && ((SadBase <= SystemAddress) && (SystemAddress <= SadLimit)) ){
    DEBUG ((DEBUG_ERROR, "\nERROR: Address falls into MMIO_H SAD\n"));
    return EFI_INVALID_PARAMETER;
  }

  MmiohNonInterleaveRule0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, MMIOH_NONINTERLEAVE_RULE_N0_CHABC_SAD1_REG);
  MmiohNonInterleaveRule1.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, MMIOH_NONINTERLEAVE_RULE_N1_CHABC_SAD1_REG);
  SadBase = ((UINT64)MmiohNonInterleaveRule0.Bits.baseaddress << 26) || ((UINT64)MmiohNonInterleaveRule0.Bits.baseaddress_lsb_bits << 24);
  SadLimit = (((UINT64)MmiohNonInterleaveRule0.Bits.limitaddress) || ((UINT64)MmiohNonInterleaveRule1.Bits.limitaddress << 6)) << 26;
  SadLimit = SadLimit || ((UINT64)MmiohNonInterleaveRule0.Bits.limitaddress_lsb_bits << 24);

  if ( (MmiohNonInterleaveRule0.Bits.rule_enable) && ((SadBase <= SystemAddress) && (SystemAddress <= SadLimit)) ){
    DEBUG ((DEBUG_ERROR, "\nERROR: Address falls into MMIO_H SAD\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Now go through the DRAM SAD Entry

  // First get the logical Channel Id form the MCRoute Table
  if (McChToLogChId (TA) != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Logical ChannelId is not found"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_ERROR, "Logical ChannelId is 0x%x\n",TA->LogChannelId));
  // Read the DRAM Rule for the corresponding SADId, and get the SadLimit
  if (TA->MemType != MEM_TYPE_2LM) {
    DramRule.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mDramRuleRegOffset[SadId]);
  } else {
    DramRule.Data = mCpuCsrAccess->ReadMcCpuCsr (SktId, McId, mDRAMRuleCfgMC[SadId]);
  }

  if (DramRule.Bits.rule_enable != 1) {
    DEBUG ((DEBUG_ERROR, "Sad Rule for 0x%x is not valid\n",SadId));
    return EFI_INVALID_PARAMETER;
  }

  SadLimit = (((UINT64)DramRule.Bits.limit + 1) << SAD_UNIT)-1;

  // Get the limit of the Previous SAD
  if (SadId == 0) {
    PrevSadLimit = 0;
  } else {
    PrevDramRule.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, mDramRuleRegOffset[SadId - 1]);
    if (PrevDramRule.Bits.rule_enable != 1) {
      return EFI_INVALID_PARAMETER;
    }
    PrevSadLimit = (((UINT64)PrevDramRule.Bits.limit + 1) << SAD_UNIT)-1;
  }

  // Check if the system Address falls between the Previous SADLimt and the Current SAD Limit
  if (((PrevSadLimit == 0) && (PrevSadLimit == SystemAddress) && (SystemAddress <= SadLimit)) ||
          ((PrevSadLimit < SystemAddress) && (SystemAddress <= SadLimit))) {
    if (PatchInterleaveListIndex (TA,DramRule) == EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "\nMATCH MATCH MATACH - Address found\n"));
      return EFI_SUCCESS;
    } else {
      DEBUG ((DEBUG_ERROR, "\nInterleaveListIndex doesn't match"));
      return EFI_INVALID_PARAMETER;
    }
  } else {
     DEBUG ((DEBUG_ERROR, "\nThe address doesn't fall into the current SAD"));
    return EFI_INVALID_PARAMETER;
  }
  
}

EFI_STATUS
GetNmSystemAddress (
  IN PTRANSLATED_ADDRESS TA
)
{
  UINT64  SystemAddress;
  UINT8   SktId;
  UINT8   McId;
  UINT8   NmChId;
  UINT8   NodeId;
  UINT64  NmChannelAddress;
  UINT8   NmSktWays;
  UINT8   NmChWays;
  UINT8   NmChBit;
  UINT8   NmSktBit;
  UINT64  NmChOffset;
  UINT8   SktInterleaveBit;
  UINT8   ChnInterleaveBit;
  UINT64  LowerAddrBits;

  MCNMCACHINGCFG2_MC_2LM_STRUCT               McNMCachingCfg2;
  MCNMCACHINGINTLV_MC_2LM_STRUCT              McNMCachingIlv;
  MCNMCACHINGOFFSET0_MC_2LM_STRUCT            McNnCachingOffset0;

  SystemAddress = (UINT64)-1;
  SktId      = TA->SocketId;
  NmChId     = TA->NmChannelId;
  McId = TA->MemoryControllerId;
  NodeId        = TA->Node;
  NmChannelAddress = TA->NmChannelAddress;

  SktInterleaveBit = 12;
  ChnInterleaveBit = 8;

  McNMCachingCfg2.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGCFG2_MC_2LM_REG);
  McNMCachingIlv.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGINTLV_MC_2LM_REG);
  McNnCachingOffset0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, NODECH_TO_SKTCH(NodeId, NmChId), MCNMCACHINGOFFSET0_MC_2LM_REG);

  NmChWays = 1 << (McNMCachingIlv.Bits.mcnmcaching_tad_il_granular_ximc);
  NmSktWays = 1 << (McNMCachingIlv.Bits.mcnmcachingsocketintlv);

  NmChBit = (UINT8)McNMCachingIlv.Bits.mcnmcachingchanintlvval;
  NmSktBit = (UINT8)McNMCachingIlv.Bits.mcnmcachingsocketintlvval;

  NmChOffset = ((UINT64)McNnCachingOffset0.Bits.mcnmcachingoffset0 << 32);

  SystemAddress = NmChannelAddress;

  // Put the ChInterleaveBits and Chways
  LowerAddrBits = BitFieldRead64(SystemAddress, 0, ChnInterleaveBit-1);
  SystemAddress = SystemAddress >> ChnInterleaveBit;
  SystemAddress = SystemAddress * NmChWays;
  SystemAddress = SystemAddress + NmChBit;
  SystemAddress = (SystemAddress << ChnInterleaveBit) | LowerAddrBits;

  
  // Put the ChInterleaveBits and Chways
  LowerAddrBits = BitFieldRead64(SystemAddress, 0, SktInterleaveBit-1);
  SystemAddress = SystemAddress >> SktInterleaveBit;
  SystemAddress = SystemAddress * NmSktWays;
  SystemAddress = SystemAddress + NmSktBit;
  SystemAddress = (SystemAddress << SktInterleaveBit) | LowerAddrBits;
  
  SystemAddress = SystemAddress + NmChOffset;
  
  TA->NmSystemAddress = SystemAddress;
  
  return EFI_SUCCESS;
}

EFI_STATUS
GetDDRTSystemAddress (
  IN PTRANSLATED_ADDRESS TA
)
{
  UINT8   SktId, McId, ChId, SktCh, TadIndex, SktWays, ChWays, SktGran, ChGran, SktInterleaveBit, ChInterleaveBit, ChIdxOffset, CorrChIdxOffset;
  UINT64  SystemAddress, ChannelAddress, LowerAddrBits, ShiftedAddress, TADLimit, PrevLimit, TADOffset, TadBase;
  UINT8   Index, ChOffsetId, w_id, imc_id_eff, AddressMod, ModResult;
  BOOLEAN MatchFound;
  EFI_STATUS Status;

  TADWAYNESS_0_MC_MAIN_STRUCT       TadWayness;
  TADBASE_0_MC_MAIN_STRUCT          TadBaseReg;
  FMTADCHNILVOFFSET_0_MC_2LM_STRUCT     FmTadChIlvOffset;

  SktId = TA->SocketId;
  McId = TA->MemoryControllerId;
  ChId = TA->ChannelId;
  ChannelAddress = TA->ChannelAddress;
 
  TA->SystemAddress = SystemAddress = (UINT64)-1;
  SktCh = (McId * MAX_MC_CH) + ChId;
  PrevLimit = 0;
  MatchFound = FALSE;

  if (TA->MemType == MEM_TYPE_2LM){
    Status = GetNmSystemAddress (TA);
  }

  for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {

    // Read TadWayness register and TadBase register
    TadWayness.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadWaynessRegOffset[TadIndex]);
    TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadBaseRegOffset[TadIndex]);
    DEBUG ((DEBUG_ERROR, "\nFor TadIndex 0x%x, TadWayness.Data = 0x%lx\n",TadIndex, TadWayness.Data));

    if (TadWayness.Bits.tad_limit == 0)  {
      break;
    }

    // Get the TadLimit from TadWayness register
    TADLimit = (((UINT64)TadWayness.Bits.tad_limit + 1) << SAD_UNIT)-1;


    // Read TadChIlvOffset Register to get TADOffset
    FmTadChIlvOffset.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh, mFMTADChnIlvOffset[TadIndex]);
    TADOffset = ((UINT64)FmTadChIlvOffset.Bits.tad_offset << SAD_UNIT);
    DEBUG ((DEBUG_ERROR, "\nGetDDRTSystemAddress: (chOffset shl SAD_UNIT): %lx", TADOffset));
    if(TADOffset & (((UINT64)MAX_MEM_ADDR << 27))) {  //  MAX_MEM_ADDR = 0x40000 ( 256Mb gran.) check bit45 to verfiy the sign bit.
      TADOffset  = TADOffset - ((UINT64)MAX_MEM_ADDR << 28);
      DEBUG ((DEBUG_ERROR, "\nGetDDRTSystemAddress: (chOffset - 1 shl 46) %lx", TADOffset));
      TADOffset &= (UINT64)0x3ffffffffff;  // 46bit width maximum valid address. ( (1 << 46) -1)
    }

    // Get SktWays and ChWays
    if (TA->MemType == MEM_TYPE_2LM) {
      SktWays = (UINT8) (1 << FmTadChIlvOffset.Bits.skt_ways);
      ChWays = (UINT8)FmTadChIlvOffset.Bits.chn_ways+1;
    } else {
      SktWays = (UINT8) (1 << TadWayness.Bits.tad_skt_way);
      ChWays = (UINT8)TadWayness.Bits.tad_ch_way+1;
    }

    // Get SktInterleaveBit and ChInterleaveBit
    SktGran = (UINT8) FmTadChIlvOffset.Bits.skt_granularity;
    switch (SktGran) {
      case 0: SktInterleaveBit = 6; break; // 64B
      case 1: SktInterleaveBit = 8; break; // 256BRIR
      case 2: SktInterleaveBit = 12; break; // 4KB
      case 3: SktInterleaveBit = 30; break; // 1GB
      default: return EFI_INVALID_PARAMETER;
    }    
    DEBUG ((DEBUG_ERROR, "\nGetDDRTSystemAddress: SktInterleaveBit: %d", SktInterleaveBit));

    ChGran = (UINT8) FmTadChIlvOffset.Bits.ch_granularity;
    switch (ChGran) {
      case 0: ChInterleaveBit = 6; break; // 64B
      case 1: ChInterleaveBit = 8; break; // 256B
      case 2: ChInterleaveBit = 12; break; // 4KB
      default: return EFI_INVALID_PARAMETER;
    }
    DEBUG ((DEBUG_ERROR, "\nGetDDRTSystemAddress: chInterleaveBit: %d", ChInterleaveBit));

    if ((ChWays == 3) && (SktInterleaveBit > ChInterleaveBit)) {

      if (McChToLogChId (TA) != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR, "Logical ChannelId is not found"));
        return EFI_INVALID_PARAMETER;
      }
      ModResult = TA->LogChannelId >> 1;

      for (Index = 0; Index < SktWays; Index++) {

        // Get the effect IMCId
        ChOffsetId = (TADOffset >> ChInterleaveBit) % 3;
        w_id = (3 + ChId - ChOffsetId) % 3;
        imc_id_eff = Index;

        // Form the Temp System Address based on the  using Channel Address,nterleave Granularity, SktWays, ChWays, TadOffset
        SystemAddress = ChannelAddress;
        LowerAddrBits = BitFieldRead64(SystemAddress, 0, SktInterleaveBit-1);
        SystemAddress = SystemAddress >> SktInterleaveBit;
        SystemAddress = SystemAddress * SktWays;
        SystemAddress = SystemAddress + imc_id_eff;
        SystemAddress = (SystemAddress << SktInterleaveBit) | LowerAddrBits;

        LowerAddrBits = BitFieldRead64(SystemAddress, 0, ChInterleaveBit-1);
        SystemAddress = SystemAddress >> ChInterleaveBit;
        SystemAddress = SystemAddress * ChWays;
        SystemAddress = SystemAddress + w_id;
        SystemAddress = (SystemAddress << ChInterleaveBit) | LowerAddrBits;

        if (FmTadChIlvOffset.Bits.tad_offset_sign == 0) {
          SystemAddress += TADOffset;
        } else {
          SystemAddress -= TADOffset;
        }

        // Check if the AddressMod matches with ModResult
        ShiftedAddress = SystemAddress >> ChInterleaveBit;
        AddressMod = ShiftedAddress % 3;

        if (AddressMod == ModResult) {
          if ((PrevLimit < SystemAddress) && (SystemAddress <= TADLimit)) {
           break;
          }
        }
      }
    }  else {

      // Form the Temp System Address based on the Interleave Granularity, SktWays, ChWays, TadOffset, ChannelAddress
      SystemAddress = ChannelAddress;
       
      // Put the ChannelInterleaveBits and Chways
      LowerAddrBits = BitFieldRead64(SystemAddress, 0, ChInterleaveBit-1);
      SystemAddress = SystemAddress >> ChInterleaveBit;
      SystemAddress = SystemAddress * ChWays;
   
      if (ChWays == 3) {
          TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadBaseRegOffset[TadIndex]);
          TadBase =  (((UINT64)TadBaseReg.Bits.base) << SAD_UNIT);
          TadBase = TadBase + (TadBaseReg.Bits.base_offset << SktInterleaveBit);
          ChIdxOffset = TadBase % ChWays;
          CorrChIdxOffset = ((ChId + ChWays - ChIdxOffset) * SktWays) % ChWays;
          SystemAddress = SystemAddress + CorrChIdxOffset;
      } 
      SystemAddress = (SystemAddress << ChInterleaveBit) | LowerAddrBits;
      DEBUG ((DEBUG_ERROR, "(After putting in ChInterleaveBit)SystemAddress = 0x%lx\n", SystemAddress));

      // Put the SktInterleaveBits and Sktways
      LowerAddrBits = BitFieldRead64(SystemAddress, 0, SktInterleaveBit-1);
      SystemAddress = SystemAddress >> SktInterleaveBit;
      SystemAddress = SystemAddress * SktWays;
      SystemAddress = (SystemAddress << SktInterleaveBit) | LowerAddrBits;
      DEBUG ((DEBUG_ERROR, "(After putting in skt interleave bits)SystemAddress = 0x%lx\n", SystemAddress));

      if (FmTadChIlvOffset.Bits.tad_offset_sign == 0) {
        SystemAddress += TADOffset;
      } else {
        SystemAddress -= TADOffset;
      }
  
      DEBUG ((DEBUG_ERROR, "SystemAddress +/- TADoffset = 0x%lx\n", SystemAddress));
    }
      
    // Check if the limit falls into TADLimit
    if ((PrevLimit < SystemAddress) && (SystemAddress <= TADLimit)) {
      TA->TadId = TadIndex;
      TA->SystemAddress = SystemAddress;
      DEBUG ((DEBUG_ERROR, "Falls into TadId = 0x%x\n", TadIndex));

      // Get the SadId
      Status = TadId2SadId(TA);

      //
      // Call ReveseSad() function to patch in TgtId bits.
      // if it is not success, we need to go for another TAD entry
      //
      if (ReverseSad(TA) == EFI_SUCCESS) {
        MatchFound = TRUE;
        break;
      }
    } 
 
    // Match is not found, read next TAD register
    PrevLimit = TADLimit;
  }

  if (MatchFound == TRUE) {
    DEBUG ((DEBUG_ERROR, "The System Address calculated from Reverse Address Translation is 0x%lx\n", TA->SystemAddress));
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }


}


EFI_STATUS
GetSystemAddress (
  IN PTRANSLATED_ADDRESS TA
 )
{
  UINT8   SktId, McId, ChId, SktCh, TadIndex, SktWays, ChWays, SktGran, ChGran, SktInterleaveBit, ChInterleaveBit, ChIdxOffset, CorrChIdxOffset;
  UINT64  SystemAddress, ChannelAddress, LowerAddrBits, TADLimit, PrevLimit, TADOffset, TadBase, TempAddress;
  UINT8   priSecBit;
  BOOLEAN MatchFound;
  EFI_STATUS Status;

  TADCHNILVOFFSET_0_MC_MAIN_STRUCT  TadChannelOffset;
  TADWAYNESS_0_MC_MAIN_STRUCT       TadWayness;
  TADBASE_0_MC_MAIN_STRUCT          TadBaseReg;

  if (TA->MemType == MEM_TYPE_1LM) {
  
    SktInterleaveBit = 6;
    ChInterleaveBit = 6;

    DEBUG ((DEBUG_ERROR, "\nIn GetSystemAddress() for 1LM\n"));
    SktId = TA->SocketId;
    McId = TA->MemoryControllerId;
    ChId = TA->ChannelId;
    ChannelAddress = TA->ChannelAddress;
 
    TA->SystemAddress = SystemAddress = (UINT64)-1;
    SktCh = (McId * MAX_MC_CH) + ChId;
    PrevLimit = 0;
    MatchFound = FALSE;

    for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {

      // Read TadWayness register to get TadLimit, SktWays, ChWays
      TadWayness.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadWaynessRegOffset[TadIndex]);
      DEBUG ((DEBUG_ERROR, "\nFor TadIndex 0x%x, TadWayness.Data = 0x%lx\n",TadIndex, TadWayness.Data));

      if (TadWayness.Bits.tad_limit == 0)  {
        break;
      }

      TADLimit = (((UINT64)TadWayness.Bits.tad_limit + 1) << SAD_UNIT)-1;
      SktWays = (UINT8) (1 << TadWayness.Bits.tad_skt_way);
      ChWays = (UINT8)(TadWayness.Bits.tad_ch_way + 1);

      DEBUG ((DEBUG_ERROR, "TADLimit = 0x%lx\n", TADLimit));
      DEBUG ((DEBUG_ERROR, "PrevLimit = 0x%lx\n", PrevLimit));
      DEBUG ((DEBUG_ERROR, "SktWays = 0x%x\n", SktWays));
      DEBUG ((DEBUG_ERROR, "ChWays = 0x%x\n", ChWays));

      // Read TadChannelOffset Register for TADOffset
      TadChannelOffset.Data = mCpuCsrAccess->ReadCpuCsr(SktId, SktCh, mTadChnlIlvOffsetRegOffset[TadIndex]);
      TADOffset = ((UINT64)TadChannelOffset.Bits.tad_offset << SAD_UNIT);
      if (TADOffset & ((UINT64)1 << 45)) {
        TADOffset = TADOffset - ((UINT64)1 << 46);
      }

      DEBUG ((DEBUG_ERROR, "TADOffset = 0x%lx\n", TADOffset));

      // Get the SktInterleaveBit
      SktGran = (UINT8) TadChannelOffset.Bits.skt_granularity;
      switch (SktGran) {
        case 0: SktInterleaveBit = 6; break; // 64B
        case 1: SktInterleaveBit = 8; break; // 256BRIR
        case 2: SktInterleaveBit = 12; break; // 4KB
        case 3: SktInterleaveBit = 30; break; // 1GB
      }

      ChGran = (UINT8) TadChannelOffset.Bits.ch_granularity;
      switch (ChGran) {
        case 0: ChInterleaveBit = 6; break; // 64B
        case 1: ChInterleaveBit = 8; break; // 256B
        case 2: ChInterleaveBit = 12; break; // 4KB
      }

      DEBUG ((DEBUG_ERROR, "ChInterleaveBit = 0x%lx\n", ChInterleaveBit));
      DEBUG ((DEBUG_ERROR, "SktInterleaveBit = 0x%lx\n", SktInterleaveBit));

      // Form the Temp System Address based on the Interleave Granularity, SktWays, ChWays, TadOffset, ChannelAddress
      // For DDR4, ChannelInterleaveBit and SktInterleaveBit are same

      SystemAddress = ChannelAddress;
    
      //Read tadbase to know if mirroring was enabled
      TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadBaseRegOffset[TadIndex]);

      //Mirrored TAD will have a bit inserted at bit position 18 by hardware, remove this bit
      if(TadBaseReg.Bits.mirror_en) {
        TempAddress = BitFieldRead64(SystemAddress, 0, 17);
        SystemAddress >>= 18;
        priSecBit  = SystemAddress & BIT0;
        SystemAddress >>= 1;
        SystemAddress <<= 18;
        SystemAddress |= TempAddress;
      }
       
      // Put the ChannelInterleaveBits and Chways
      LowerAddrBits = BitFieldRead64(ChannelAddress, 0, ChInterleaveBit-1);
      SystemAddress = SystemAddress >> ChInterleaveBit;
      DEBUG ((DEBUG_ERROR, "SystemAddress >> ChInterbit%x = SystemAddress\n", ChInterleaveBit, SystemAddress));
      SystemAddress = SystemAddress * ChWays;
   
      if (ChWays == 3) {
        TadBaseReg.Data = mCpuCsrAccess->ReadMcCpuCsr(SktId, McId, mTadBaseRegOffset[TadIndex]);
        TadBase =  (((UINT64)TadBaseReg.Bits.base) << SAD_UNIT);
        TadBase = TadBase + (TadBaseReg.Bits.base_offset << SktInterleaveBit);
        ChIdxOffset = TadBase % ChWays;
        CorrChIdxOffset = ((ChId + ChWays - ChIdxOffset) * SktWays) % ChWays;
        DEBUG ((DEBUG_ERROR, "CorrChIdxOffset = 0x%lx\n", CorrChIdxOffset));
        SystemAddress = SystemAddress + CorrChIdxOffset;
        DEBUG ((DEBUG_ERROR, "SystemAddress = 0x%lx\n", SystemAddress));
      } 
      SystemAddress = (SystemAddress << ChInterleaveBit) | LowerAddrBits;
      DEBUG ((DEBUG_ERROR, "(After putting in ChInterleaveBit)SystemAddress = 0x%lx\n", SystemAddress));

      // Put the SktInterleaveBits and Sktways
      LowerAddrBits = BitFieldRead64(SystemAddress, 0, SktInterleaveBit-1);
      SystemAddress = SystemAddress >> SktInterleaveBit;
      SystemAddress = SystemAddress * SktWays;
      SystemAddress = (SystemAddress << SktInterleaveBit) | LowerAddrBits;
      DEBUG ((DEBUG_ERROR, "(After putting in skt interleave bits)SystemAddress = 0x%lx\n", SystemAddress));

      SystemAddress += TADOffset;
      DEBUG ((DEBUG_ERROR, "SystemAddress + TADoffset = 0x%lx\n", SystemAddress));

      // Check if the limit falls into TADLimit
      if (((PrevLimit == 0) && (PrevLimit == SystemAddress) && (SystemAddress <= TADLimit)) ||
              ((PrevLimit < SystemAddress) && (SystemAddress <= TADLimit))) {
        TA->TadId = TadIndex;
        TA->SystemAddress = SystemAddress;
        DEBUG ((DEBUG_ERROR, "Falls into TadId = 0x%x\n", TadIndex));

        // Get the SadId
        Status = TadId2SadId(TA);

        //
        // Call ReveseSad() function to patch in TgtId bits.
        // if it is not success, we need to go for another TAD entry
        //
        if (ReverseSad(TA) == EFI_SUCCESS) {
          MatchFound = TRUE;
          break;
        }
      } 
 
      // Match is not found, read next TAD register
      PrevLimit = TADLimit;
    }

    if (MatchFound == TRUE) {
      DEBUG ((DEBUG_ERROR, "The System Address calculated from Reverse Address Translation is 0x%lx\n", TA->SystemAddress));
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Get System Address for 2LM/BM/PM\n"));
    if (GetDDRTSystemAddress (TA) == EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "The System Address calculated from Reverse Address Translation is 0x%lx\n", TA->SystemAddress));
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }

  }
}

VOID
ApplyRasEventsToRAT (
  IN  PTRANSLATED_ADDRESS TA
)

{
  // ApplyRankSparing
  RankSparingSupportForAddressTranslation (TA, REVERSE_ADDRESS_TRANSLATION);

  // ApplyADDDC
  ADDDCSupportForAddressTranslation (TA);
  return;
}

EFI_STATUS
TranslateDimmAddress (
  IN  OUT PTRANSLATED_ADDRESS TA
)
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_ERROR, "\nThe SocketId is 0x%lx\n", TA->SocketId));
  DEBUG ((DEBUG_ERROR, "\nThe MemoryControllerId is 0x%lx\n", TA->MemoryControllerId));
  DEBUG ((DEBUG_ERROR, "\nThe ChannelId is 0x%lx\n", TA->ChannelId));
  DEBUG ((DEBUG_ERROR, "\nThe DimmSlot is 0x%lx\n", TA->DimmSlot));
  DEBUG ((DEBUG_ERROR, "\nThe PhysicalRankId is 0x%lx\n", TA->PhysicalRankId));
  DEBUG ((DEBUG_ERROR, "\nThe Row is 0x%lx\n", TA->Row));
  DEBUG ((DEBUG_ERROR, "\nThe Col is 0x%lx\n", TA->Col));
  DEBUG ((DEBUG_ERROR, "\nThe Bank is 0x%lx\n", TA->Bank));
  DEBUG ((DEBUG_ERROR, "\nThe BankGroup is 0x%lx\n", TA->BankGroup));

  TA->SadId = (UINT8)-1;
  TA->TadId = (UINT8)-1;
  TA->TargetId = (UINT8)-1;
  TA->LogChannelId = (UINT8)-1;
  TA->SystemAddress = (UINT64)-1;
  TA->NmSystemAddress = (UINT64)-1;
  TA->ChannelAddress = (UINT64)-1;
  TA->NmChannelAddress = (UINT64)-1;
  TA->RankAddress = (UINT64)-1;
  TA->NmRankAddress = (UINT64)-1;
    
  GetMemoryType (TA);

  Status = GetRankAddress (TA);

  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "\nThe RankAddress is 0x%lx\n", TA->RankAddress));
    Status = GetChannelAddress (TA);

    if (Status == EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "\nThe ChannelAddress is 0x%lx\n", TA->ChannelAddress));
      Status = GetSystemAddress (TA);

      if (Status == EFI_SUCCESS) {
         DEBUG ((DEBUG_ERROR, "\nThe System Address is 0x%lx\n", TA->SystemAddress));
      } else {
         DEBUG ((DEBUG_ERROR, "\nError occurred in getting the system Address\n"));
      }

    } else {
      DEBUG ((DEBUG_ERROR, "\nError occurred in getting the Channel Address\n"));
    }

  } else {
     DEBUG ((DEBUG_ERROR, "\nError occurred in getting the Rank Address\n"));
  }

  return Status;
}


EFI_STATUS
EFIAPI
DimmAddressToSystemAddress (
   IN  OUT PTRANSLATED_ADDRESS TA
 )
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_ERROR, "\nReverse Address Translation start\n"));

  Status = TranslateDimmAddress (TA);

  ApplyRasEventsToRAT(TA);

  // Rank VLS Happened,  Call Reverse Address Translation with LockStepRank info to get SpareAddress
  if (TA->RasModesApplied & BIT1) {
    DEBUG ((DEBUG_ERROR, "\nRank VLS happened,  Calling the Reverse Address Translation with the lockstep Rank\n"));
    TA->PhysicalRankId = TA->LockStepPhysicalRank;
    Status = TranslateDimmAddress (TA);
    if (Status == EFI_SUCCESS) {
      TA->SpareSystemAddress = TA->SystemAddress;
    }
  }

    // Bank VLS Happened,  Call Reverse Address Translation with LockStep Rank,Bank, Bankgroup info to get SpareAddress
  if (TA->RasModesApplied & BIT1) {
    DEBUG ((DEBUG_ERROR, "\nBank VLS happened,  Calling the Reverse Address Translation with the lockstep Rank, bank, bankgroup\n"));
    TA->PhysicalRankId = TA->LockStepPhysicalRank;
    TA->Bank = TA->LockStepBank;
    TA->BankGroup = TA->LockStepBG;
    Status = TranslateDimmAddress (TA);
    if (Status == EFI_SUCCESS) {
      TA->SpareSystemAddress = TA->SystemAddress;
    }
  }

  

   if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "\nReverse Address Translation Successful!!\n"));
  }
  return Status;

}




