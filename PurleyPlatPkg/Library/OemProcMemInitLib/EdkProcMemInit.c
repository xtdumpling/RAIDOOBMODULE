//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//
//  Rev. 1.01
//   Bug Fixed:  Also check Mbank 9 to 11
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Dec/30/2016
//
//  Rev. 1.03
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.02
//   Bug Fixed:  Checking MC bank range is incorrect.
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/09/2016
//
//  Rev. 1.01
//   Bug Fixed:  Support SMC Memory map-out function.
//   Reason:     
//   Auditor:    Ivern Yeh
//   Date:       Jul/07/2016
//
//  Rev. 1.00
//   Bug Fix:  1.Add Manufacturing Date in Part number field 's tail (date :Year/Week) of Smbios Type 17.  
//   Reason:   
//   Auditor:  Chen Lin
//   Date:     Jul/01/2016
//
//***************************************************************************
//
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file EdkProcMemInit.c

**/

#include "NGNRecordsData.h"
#include <IioPciePortInfo.h>
#include <Library/SetupLib.h>

#pragma warning(disable : 4100)

#include "EdkProcMemInit.h"

// APTIOV_SERVER_OVERRIDE_RC_START : To support Capsule update in Purley
#if Capsule_SUPPORT
#include <Ppi/Capsule.h>
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To support Capsule update in Purley

#include "Token.h" // Supermicro
#ifndef MAX_HOB_ENTRY_SIZE
#define MAX_HOB_ENTRY_SIZE  60*1024
#endif

#ifdef BDAT_SUPPORT
#include "bdat.h"
EFI_GUID gEfiMemoryMapDataHobBdatGuid  = BDAT_MEMORY_DATA_4B_GUID;
EFI_GUID gEfiRmtDataHobBdatGuid        = RMT_SCHEMA_5_GUID;
#endif //BDAT_SUPPORT

#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
#include <SmcLibCommon.h>
#include <SspTokens.h>
#include <mca_msr.h>
#include <Protocol/MemRasProtocol.h>
#define   RANK_INTL_MIN_CLOSE     6
#define   RANK_INTL_MIN_OPEN      13
#endif //#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT

extern EFI_GUID gEfiSiliconRcHobsReadyPpi;
extern BOOLEAN IsSimicsPlatform(VOID);

static EFI_PEI_PPI_DESCRIPTOR mSiliconRcHobsReadyPpi[] =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiSiliconRcHobsReadyPpi,
    NULL
};
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
#define MEM_M2M_0_BANK          7
#define MEM_M2M_1_BANK          8
//Memory mcbank offsets
#define MEM_M2M_MCBANK_NUM_START	MEM_M2M_0_BANK
#define MEM_M2M_MCBANK_NUM_END		MEM_M2M_1_BANK
#define CHA_MCBANK_NUM_START	9
#define CHA_MCBANK_NUM_END		11
#define MEM_IMC0_CH0_BANK    13
#define MEM_IMC0_CH1_BANK    14
#define MEM_IMC1_CH0_BANK    15
#define MEM_IMC1_CH1_BANK    16
#define MEM_IMC0_CH2_BANK    17
#define MEM_IMC1_CH2_BANK    18
#define MEM_IMC_MCBANK_NUM_START	MEM_IMC0_CH0_BANK
#define MEM_IMC_MCBANK_NUM_END		MEM_IMC1_CH2_BANK

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

UINT32  mInterleaveListRegOffset[SAD_RULES] = {
    INTERLEAVE_LIST_CFG_0_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_1_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_2_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_3_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_4_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_5_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_6_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_7_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_8_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_9_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_10_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_11_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_12_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_13_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_14_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_15_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_16_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_17_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_18_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_19_CHABC_SAD_REG,
    INTERLEAVE_LIST_CFG_20_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_21_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_22_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_23_CHABC_SAD_REG
};

UINT32  mDramRuleRegOffset[SAD_RULES] = {
    DRAM_RULE_CFG_0_CHABC_SAD_REG, DRAM_RULE_CFG_1_CHABC_SAD_REG, DRAM_RULE_CFG_2_CHABC_SAD_REG, DRAM_RULE_CFG_3_CHABC_SAD_REG,
    DRAM_RULE_CFG_4_CHABC_SAD_REG, DRAM_RULE_CFG_5_CHABC_SAD_REG, DRAM_RULE_CFG_6_CHABC_SAD_REG, DRAM_RULE_CFG_7_CHABC_SAD_REG,
    DRAM_RULE_CFG_8_CHABC_SAD_REG, DRAM_RULE_CFG_9_CHABC_SAD_REG, DRAM_RULE_CFG_10_CHABC_SAD_REG, DRAM_RULE_CFG_11_CHABC_SAD_REG,
    DRAM_RULE_CFG_12_CHABC_SAD_REG, DRAM_RULE_CFG_13_CHABC_SAD_REG, DRAM_RULE_CFG_14_CHABC_SAD_REG, DRAM_RULE_CFG_15_CHABC_SAD_REG,
    DRAM_RULE_CFG_16_CHABC_SAD_REG, DRAM_RULE_CFG_17_CHABC_SAD_REG, DRAM_RULE_CFG_18_CHABC_SAD_REG, DRAM_RULE_CFG_19_CHABC_SAD_REG,
    DRAM_RULE_CFG_20_CHABC_SAD_REG, DRAM_RULE_CFG_21_CHABC_SAD_REG, DRAM_RULE_CFG_22_CHABC_SAD_REG, DRAM_RULE_CFG_23_CHABC_SAD_REG
};

BOOLEAN
  PostUCE(
  struct sysHost  *host
)
{
  BOOLEAN IsPostUCE = FALSE;
  UINT8   i;

 for (i = 0; i < MAX_LOG; i++)
  {
  	if ( WARN_FPT_UNCORRECTABLE_ERROR == ( (host->var.common.status.log[i].code >>8 ) & 0xFF ) )
	{
       IsPostUCE = TRUE;
       break;
	}
  } 
   
 return IsPostUCE;
     
}


UINT64
MCAddress (
  UINT64 Addr,
  UINT64 Misc)
{
  UINT64 Mask = 0;
  UINT64 McAddress = 0;
  UINT8 Lsb = 0;

  Mask = MCA_ADDR_BIT_MASK;
  McAddress = Addr & Mask;

  Lsb = Misc & MCA_MISC_LSB_MASK;
  Mask = ~((1 << Lsb) - 1);

  McAddress &= Mask;

  return McAddress;
};

/**

Routine Description:
   Check if current memory node is enabled

Arguments:

    Memory Node ID
    ptr to memory node enabled flag

Modifies:
    Memory node enabled flag

Returns:
  EFI_SUCCESS / Error code


--*/
EFI_STATUS
EFIAPI
IsMemNodeEnabled(
    struct sysHost  *host,
    IN UINT8 Node,
    OUT BOOLEAN *IsMemNodeEnabledFlag
)
{
    UINT8    socket,mcNum;
    
    socket = NODE_TO_SKT(Node);
    mcNum = NODE_TO_MC(Node);
    *IsMemNodeEnabledFlag = host->nvram.mem.socket[socket].imc[mcNum].enabled;
       
  return EFI_SUCCESS;
}

UINT8
HaNodeIdToNode (
  UINT8 HaNodeId
)
{
  UINT8 Ha,SktId,Node;

  Ha    = (HaNodeId & BIT2) >> 2;
  SktId = (HaNodeId & 3);
  Node  = (SktId << 1) + Ha;

  return Node;

}

UINT8
HaNodeIdToSkt (
  UINT8 haNodeId
)
{
  UINT8 ha,SktId;

  ha     = (haNodeId & BIT2) >> 2;
  SktId = (haNodeId & 3);

  return SktId;

}

UINT8
HaNodeIdToHa (
  UINT8 haNodeId
)
{
UINT8 ha;

ha     = (haNodeId & BIT2) >> 2;

return ha;

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
    TRANSLATED_ADDRESS *TA,
    struct sysHost             *host
 )
{
  UINT64  physicalAddress;
  UINT64  channelAddress;
  UINT64  RankAddr;
  UINT64  rirLimit;
  UINT64  PrevLimit, tempV;
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
  UINT32  c, tempI;
  UINT8   RankGroups;

  RIRWAYNESSLIMIT_0_MC_MAIN_STRUCT  imcRIRWaynessLimit;
  FMRIRWAYNESSLIMIT_0_MC_2LM_STRUCT imcFMRIRWaynessLimit;
  RIRILV0OFFSET_0_MC_MAIN_STRUCT    imcRIRIlv0Offset;
  MCMTR_MC_MAIN_STRUCT              imcMcmtr;
  DIMMMTR_0_MC_MAIN_STRUCT          dimmMtr;
  MCDDRTCFG_MC_MAIN_STRUCT          mcDdrtCfg;

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
  for (RirIndex = 0; RirIndex < MAX_RIR; RirIndex++) {
    if (match == TRUE) break;

    imcRIRWaynessLimit.Data = ReadCpuPciCfgEx( host, TA->SocketId, NODECH_TO_SKTCH(node, channel), mRIRWaynessLimit[RirIndex]);
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
      imcMcmtr.Data = ReadCpuPciCfgEx(host, TA->SocketId, TA->MemoryControllerId, MCMTR_MC_MAIN_REG );
      pageShift = (UINT8)(imcMcmtr.Bits.close_pg == 1 ? RANK_INTL_MIN_CLOSE : RANK_INTL_MIN_OPEN);
      tempV = channelAddress >> pageShift;
      tempI = (UINT32)(channelAddress >> pageShift);
      rankInterleave = tempI % rirWays;
      // squeeze out the interleave
      RankAddr = tempV / rirWays;
      //RankAddr = (channelAddress >> pageShift) / rirWays;
      RankAddr = (RankAddr << pageShift) | BitFieldRead64(channelAddress, 0, pageShift-1);
      DEBUG ((DEBUG_ERROR, "\nTranslateRIR: shiftVal: %d", pageShift));

      //
      // Get target physical rank id & rank address
      //
      imcRIRIlv0Offset.Data = ReadCpuPciCfgEx(host, TA->SocketId, NODECH_TO_SKTCH(node, channel), mRIRIlvOffset[RirIndex][rankInterleave]);
      rirRnkTgt0 = imcRIRIlv0Offset.Bits.rir_rnk_tgt0;
      rirOffset0 = ((UINT64)(imcRIRIlv0Offset.Bits.rir_offset0) << 26);
      DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_rnk_tgt0: %d", rirRnkTgt0));
      DEBUG ((DEBUG_ERROR, "\nTranslateRIR: rir_offset0: %lx", rirOffset0));

      TA->RankAddress = RankAddr - rirOffset0;
      ChipSelect = (UINT8) rirRnkTgt0;
      Dimm = ChipSelect >> 2;

      dimmMtr.Data =ReadCpuPciCfgEx(host, TA->SocketId, NODECH_TO_SKTCH(node, channel), mDimmMtrOffset[Dimm]);

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
      DEBUG ((DEBUG_ERROR, "\nTranslateRIR: LogicalRank: %d", LogicalRank));
      DEBUG ((DEBUG_ERROR, "\nTranslateRIR: Dimm: %d", Dimm));

      DEBUG ((DEBUG_ERROR, "\nTranslateRIR - RankAddress = %0lx\n", (UINT64)(TA->RankAddress)));
    } // if rirLimit
    PrevLimit = rirLimit;
  } // for (rirIndex)

  if (match == TRUE) {
    return EFI_SUCCESS;
  } else {
    PrevLimit = (UINT64) -1;
    for (RirIndex = 0; RirIndex < MAX_RIR_DDRT; RirIndex++) {
      if (match == TRUE) break;

      imcFMRIRWaynessLimit.Data = ReadCpuPciCfgEx( host, TA->SocketId, NODECH_TO_SKTCH(node, channel), mFMRIRWaynessLimit[RirIndex]);
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
      }      
    }

    if(match == TRUE) {
      mcDdrtCfg.Data = ReadCpuPciCfgEx(host, TA->SocketId, NODECH_TO_SKTCH(node, channel), MCDDRTCFG_MC_MAIN_REG);
      if(mcDdrtCfg.Bits.slot0 == 1) {
        TA->DimmType = ddrtdimmType;
        TA->DimmSlot = 0;
      } else if(mcDdrtCfg.Bits.slot1) {
        TA->DimmType = ddrtdimmType;
        TA->DimmSlot = 1;
      } else {
        EFI_INVALID_PARAMETER;
      }
      return EFI_SUCCESS;      
    }    
  }

  return EFI_INVALID_PARAMETER;

} // TranslateRIR

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
  struct sysHost             *host,
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
  UINT8   sktWaysXchWays;
  UINT8   sktGran;
  UINT8   chGran;
  UINT8   SocketId;
  UINT8   MemoryControllerId;
  UINT8   ChannelId;
  UINT8   NodeId;
  UINT8   sktInterleaveBit = 0;
  UINT8   chInterleaveBit = 0;
  BOOLEAN match = FALSE;
  UINT64  CoreAddr;
  UINT64  TempAddr;


  TADCHNILVOFFSET_0_MC_MAIN_STRUCT  TadChannelOffset;
  TADWAYNESS_0_MC_MAIN_STRUCT       tadWayness;
  TADBASE_0_MC_MAIN_STRUCT          TadBase, TempTadBase;
  MODE_M2MEM_MAIN_STRUCT            M2MemMode;
  MIRRORFAILOVER_M2MEM_MAIN_STRUCT  MirrorFailOver;
  MIRRORCHNLS_M2MEM_MAIN_STRUCT     MirrorChannels;


  systemAddress = TA->SystemAddress;
  SocketId      = TA->SocketId;
  NodeId        = TA->Node;
  ChannelId     = TA->ChannelId;
  MemoryControllerId = TA->MemoryControllerId;
  //
  // Initialize return values
  //
  TA->ChannelAddress = (UINT64)-1;
  TA->TadId = (UINT8)-1;

  PrevLimit = 0;
  match = FALSE;
  for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
    if (match == TRUE) break;

    tadWayness.Data = ReadCpuPciCfgEx(host,SocketId, MemoryControllerId, mTadWaynessRegOffset[TadIndex]);
    tadLimit = (((UINT64)tadWayness.Bits.tad_limit + 1) << SAD_UNIT)-1;
  
    if ((TadIndex != 0) && (PrevLimit == tadLimit)) break;

    DEBUG ((DEBUG_ERROR, "\nTranslateTad:Tad:%d limit:%08lx tadWayness:%08lx", TadIndex, tadLimit, tadWayness.Data));
    //
    // Does address match current tad rule limt
    //
    if (((PrevLimit == 0) && (PrevLimit == systemAddress) && (systemAddress <= tadLimit)) ||
          ((PrevLimit < systemAddress) && (systemAddress <= tadLimit))) {
      match = TRUE;
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: Tad Entry: %d", TadIndex));

      // Read TadBase register
      TadBase.Data = ReadCpuPciCfgEx(host,SocketId, MemoryControllerId, mTadBaseRegOffset[TadIndex]);

      // Read M2MemMode register for mirrorddr4 field
      M2MemMode.Data = ReadCpuPciCfgEx(host,SocketId, MemoryControllerId, MODE_M2MEM_MAIN_REG);

      if (TadBase.Bits.mirror_en || M2MemMode.Bits.mirrorddr4) {

        // For A0 Stepping, partial mirroring can be enabled only on TAD0. Returning error if it enabled on other TADs
        if (host->var.common.stepping < B0_REV_SKX) {
          for (Index = 0; Index < TAD_RULES; Index++) {
            TempTadBase.Data = ReadCpuPciCfgEx(host,SocketId, MemoryControllerId, mTadBaseRegOffset[TadIndex]);
            if (Index != 0 && TempTadBase.Bits.mirror_en == 1) {
              DEBUG ((DEBUG_ERROR, "\nTranslateTAD error: Mirroring is enabled on Tad %d, For A0 stepping, mirroring can be enabled only on TAD0.\nIllegal configuration", TadIndex));
              return EFI_UNSUPPORTED;
            }
          }
        }

        // Read MirrorFailOver register for the ddr4chnlfailed
        MirrorFailOver.Data = ReadCpuPciCfgEx(host, SocketId, MemoryControllerId, MIRRORFAILOVER_M2MEM_MAIN_REG);

        if ((MirrorFailOver.Bits.ddr4chnlfailed >> TA->ChannelId ) & 1) {

          // double check the failed channel information with the TAD base register
          if (TadBase.Bits.failed_ch != TA->ChannelId) {
            DEBUG ((DEBUG_ERROR, "\nTranslateTAD Error: MirrorFailOver.ddr4chnlfailed is not matching with TadBase.failed_ch"));
            return EFI_UNSUPPORTED;
          } else {
            MirrorChannels.Data = ReadCpuPciCfgEx(host, SocketId, MemoryControllerId, MIRRORCHNLS_M2MEM_MAIN_REG);

            if (TA->ChannelId == 0){
              ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl0secondary;
            } 
            else if (TA->ChannelId == 1) {
              ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl1secondary;
            }
            else if (TA->ChannelId == 2) {
              ChannelId = (UINT8)MirrorChannels.Bits.ddr4chnl2secondary;
            }
          }
        }
      }

      // add code here for lock step case

      sktWays = (UINT8) (1 << tadWayness.Bits.tad_skt_way);
      chWays = (UINT8)tadWayness.Bits.tad_ch_way+1;
      sktWaysXchWays = sktWays * chWays;
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: sktWays: %d", sktWays));
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: sktWaysXchWays: %d", sktWaysXchWays));

      TadChannelOffset.Data = ReadCpuPciCfgEx(host,SocketId, NODECH_TO_SKTCH(NodeId, ChannelId), mTadChnlIlvOffsetRegOffset[TadIndex]);

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
        chOffset &= (UINT64)0x3ffffffffff;  // 46bit width maximum valid address. ( (1 << 46) -1)
      }
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: chOffest 46bit adj %lx", chOffset));
      CoreAddr = systemAddress - chOffset;
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: CoreAddr %lx", CoreAddr));
      TempAddr = (CoreAddr >> chInterleaveBit);
      TempAddr =  TempAddr / chWays;
      TempAddr = (TempAddr << chInterleaveBit) | BitFieldRead64(CoreAddr, 0, chInterleaveBit-1);
      channelAddress = (TempAddr >> sktInterleaveBit);
      channelAddress = channelAddress/ sktWays;
      channelAddress = (channelAddress << sktInterleaveBit) | BitFieldRead64(TempAddr, 0, sktInterleaveBit-1);

      TA->ChannelAddress = channelAddress;
      TA->ChannelId = ChannelId;
      TA->TadId = TadIndex;
      DEBUG ((DEBUG_ERROR, "\nTranslateTAD: channelAddress: %x:%x", (UINT32)(channelAddress >> 32), (UINT32)channelAddress));
    } // if match
  } // for (TadIndex)

  return (match == TRUE) ? EFI_SUCCESS : EFI_INVALID_PARAMETER;

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
  struct sysHost             *host,
  UINT8   sadId,
  UINT8   interleaveListIndex,
  UINT8   SocketId,
  UINT8   *TargetId
  )
{
  INTERLEAVE_LIST_CFG_0_CHABC_SAD_STRUCT InterleaveList;
  UINT8 targetId = 0;

  if(TargetId == NULL || SocketId >= MAX_SOCKET || sadId >= MAX_SAD_RULES) {
    return EFI_INVALID_PARAMETER;
  }

  InterleaveList.Data = ReadCpuPciCfgEx(host,SocketId, 0, mInterleaveListRegOffset[sadId]);

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
        address_mod = (UINT8) (systemAddress >> 6) % ch_mod;
        break;
      case 1:
        address_mod = (UINT8) (systemAddress >> 8) % ch_mod;
        break;
      case 2:
        address_mod = (UINT8) (systemAddress >> 12) % ch_mod;
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
  struct sysHost             *host,
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

  chaRouteTable.Data = ReadCpuPciCfgEx(host,SocketId, logChId, MC_ROUTE_TABLE_CHA_PMA_REG);
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
  struct sysHost     *host,
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
  UINT8   mcId;
  UINT8   chId;
  BOOLEAN match;
  UINT8   rule_enable;
  UINT8   dram_attribute;
  UINT8   interleave_mode;
  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT       DramRule;

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

    if((host->var.common.socketPresentBitMap & (1 << socketId)) == 0) {
      continue;
    }

    for (sad = 0; sad < SAD_RULES; sad++) {
      if (match == TRUE) break;

      DramRule.Data = ReadCpuPciCfgEx(host,socketId, 0, mDramRuleRegOffset[sad]);
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

        Status = GetTargetId(host, sad,interleaveListIndex,socketId, &targetId);
        if(Status == EFI_SUCCESS) {
          GetLogicalChId(systemAddress,targetId, DramRule, &logChId);
          ChIdToMcCh(host, socketId,logChId,&mcId,&chId);
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
          return EFI_SUCCESS;
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

  return EFI_INVALID_PARAMETER;
} // TranslateSAD
EFI_STATUS
SystemAddressToDimmAddress (
        IN  UINT64       SystemAddress,
        OUT UINT8*       SocketId,
        OUT UINT8*       MemoryControllerId,
        OUT UINT8*       ChannelId,
        OUT UINT8*       DimmSlot,
        struct sysHost  *host
 )
{
  EFI_STATUS  status = EFI_SUCCESS;
  TRANSLATED_ADDRESS  fwdTranslatedAddress;
  
  // 
  // Make sure the system address is cache-line aligned
  // clear bits 5:0
  //
  fwdTranslatedAddress.SystemAddress = SystemAddress & (~(UINT64)0x3F);
  
  //DEBUG((EFI_D_ERROR, "SystemAddressToDimmAddress: SystemAddress: %lx \n\n", fwdTranslatedAddress.SystemAddress));
  status = TranslateSAD(host, &fwdTranslatedAddress);
  if(!status){    
  status = TranslateTAD(host, &fwdTranslatedAddress);
  }
  
  //mDdr4Dimm = (mSystemMemoryMap->NodeInfo[fwdTranslatedAddress.Node].DramType == SPD_TYPE_DDR4)? 1 :0;
  
  if(!status){
    status = TranslateRIR(&fwdTranslatedAddress, host);
  }
  //if(!status){
  //  status = TranslateRankAddress(&fwdTranslatedAddress);
  //}
  
  //
  // Update return values
  //
  *SocketId           = fwdTranslatedAddress.SocketId;
  *MemoryControllerId = fwdTranslatedAddress.MemoryControllerId;
  *ChannelId          = fwdTranslatedAddress.ChannelId;
  *DimmSlot           = fwdTranslatedAddress.DimmSlot;

  
  DEBUG((EFI_D_ERROR, "\n SystemAddressToDimmAddress conversion (Fwd translation) results \n:"));
  DEBUG((EFI_D_ERROR,  "\n SocketId = %x \n", *SocketId));
  DEBUG((EFI_D_ERROR,  "MemoryControllerId = %x \n", *MemoryControllerId));
  DEBUG((EFI_D_ERROR,  "ChannelId = %x \n", *ChannelId));
  DEBUG((EFI_D_ERROR,  "DimmSlot = %x \n", *DimmSlot));

  //DEBUG((EFI_D_ERROR, "smcdbg\n"));

  return status;
}

 EFI_STATUS
 LogMemUCE(
  struct sysHost  *host,
  MCBANK_ERR_INFO         *mcBankErrInfo
)
{
   //UINT16				   Length;
   BOOLEAN                                      FoundError; 
   UINT64                                       SystemAddress;  
   UINT8                                        SocketId = 0xFF;
   UINT8                                        MemoryControllerId = 0xFF;
   UINT8                                        ChannelId = 0xFF;
   UINT8                                        DimmSlot = 0xFF;   
   UINT8	                                      temp,DdrChannel;      
//   RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT             RetryRdErrlog;
//   UINT8                                        TempChannel  = 0;
//   UINT8                                        TempMemoryid = 0;
//   UINT8                                        RetryRedErrlog_Dimm = 0xFF;    
//Ret if we have logged it.
   DEBUG((EFI_D_ERROR,  "logged UCE, entry\n"));  

  if ( GetCmosTokensValue(MEM_MAPOUT) & 0x80  ){ 
   DEBUG((EFI_D_ERROR,  "logged UCE, ret\n"));  
   return EFI_SUCCESS;    
  }   
  
  // Don't log if MRC has UCE during POST because sysetm memory map will be changed after MRC map out DIMM.
  if ( PostUCE(host)){
    DEBUG((EFI_D_ERROR,  "Has UCE in MRC, ret\n"));
    return EFI_SUCCESS;   
  }
  
  FoundError = FALSE;   
  
	//
	// if MC bank is from 13 to 18 (MC bank for IMCx, refer mcBankTable at ErrorChip.c)
	//  {7,  0x41C}, // M2M0, IMC00, IMC01
	//  {8,  0x420}, // M2M1, IMC10, IMC11
	//  {13, 0x434}, // iMC00
	//  {14, 0x438}, // iMC01
	//  {15, 0x43C}, // iMC10
	//  {16, 0x440}, // iMC11
	//  {17, 0x444}, // iMC02
	//  {18, 0x448}, // iMC12
  DEBUG((EFI_D_ERROR,  " mcBankErrInfo->McBankNum : %x\n",mcBankErrInfo->McBankNum));

  if (  ((mcBankErrInfo->McBankNum >= MEM_IMC_MCBANK_NUM_START) && (mcBankErrInfo->McBankNum <= MEM_IMC_MCBANK_NUM_END)) || 
        ((mcBankErrInfo->McBankNum >= CHA_MCBANK_NUM_START) && (mcBankErrInfo->McBankNum <= CHA_MCBANK_NUM_END)) || 
  	(mcBankErrInfo->McBankNum == MEM_M2M_MCBANK_NUM_START) || (mcBankErrInfo->McBankNum == MEM_M2M_MCBANK_NUM_END)){
      FoundError = TRUE;
  }else{
     DEBUG((EFI_D_ERROR,  "No Mmeory UCE ret\n"));
     return  EFI_SUCCESS;   
  }
  //Length = 2;
  //mcBankErrInfo	= (MCBANK_ERR_INFO *)( (UINTN)prevBootErrorHob + Length);

  if ( FoundError ){          
     DEBUG((EFI_D_ERROR,  " MCBANK socket=0x%x Num=0x%x McbankAddr=0x%x, MCMisc=0x%x\n",mcBankErrInfo->Socket,mcBankErrInfo->McBankNum,mcBankErrInfo->McbankAddr,mcBankErrInfo->McBankMisc ));
                                                               
//    // check this is reverse translation sparing and scrub address for recoverable errors                              
//    // check REC_ERR_LSB MC MISC Bits5:0 if MC bank is from 13 to 18 (MC bank for IMCx, refer mcBankTable at ErrorChip.c)     
//    if (  ((mcBankErrInfo->McBankNum >= MEM_IMC_MCBANK_NUM_START) && (mcBankErrInfo->McBankNum <= MEM_IMC_MCBANK_NUM_END)) 
//         && (mcBankErrInfo->McBankMisc & 0x3F) ){
// 
//       DEBUG((EFI_D_ERROR, "Patrol scrub detect error, ret\n"));
//       return  EFI_SUCCESS;    
//    } 
    // get DIMM slot from MCbank 's Addr
    SystemAddress =  MCAddress(mcBankErrInfo->McbankAddr,mcBankErrInfo->McBankMisc);
    DEBUG((EFI_D_ERROR,  " SystemAddr 0x%x\n",SystemAddress));
    SystemAddressToDimmAddress(SystemAddress,&SocketId,&MemoryControllerId,&ChannelId,&DimmSlot,host);         
 
    
    // Can not find any correct DIMM location  
    if ( SocketId == 0xFF || MemoryControllerId == 0xFF || ChannelId == 0xFF || DimmSlot == 0xFF ){
        DEBUG((EFI_D_ERROR,  "no correct DIMM ,ret \n"));
        return  EFI_SUCCESS; 
    }
    
    DdrChannel = (UINT8)(  MemoryControllerId * 3 + ChannelId );
    DEBUG((EFI_D_ERROR,  "DdrChannel=%d \n",DdrChannel));
	//BIT 6 : MemoryConrolledId
	temp = 0x80 |( MemoryControllerId & 0x1 ) << 6 | ((SocketId & 0x03) << 4) | ((DdrChannel & 0x07) << 1) | (DimmSlot & 0x01);
    //set cmos
  SetCmosTokensValue (MEM_MAPOUT, temp);

	  
    DEBUG((EFI_D_ERROR,  "Log MemUCE in cmos: %x,reboot\n",temp));  
    IoWrite8(0xCF9, 0x6);  
    while(1);    
  }

  return EFI_SUCCESS;    
}
#endif //#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT

//
//  PCIE port layout
//

VOID
SaveBootErrorRecordToHob (
  struct sysHost  *host
  )
{
  PREV_BOOT_ERR_SRC_HOB                 prevBootErrorHob;
  MCBANK_ERR_INFO                       *mcBankErrInfo;
  UINT8                                 i;

  MemSetLocal ((UINT8 *) &prevBootErrorHob, 0, sizeof(prevBootErrorHob));

  prevBootErrorHob.Length = (UINT16) host->prevBootErr.validEntries * sizeof(MCBANK_ERR_INFO) + 2;  // 2 is for the length field in the hob
  mcBankErrInfo = (MCBANK_ERR_INFO*) ((UINTN)&prevBootErrorHob + 2);

  for (i = 0; i < host->prevBootErr.validEntries; i++) {
    mcBankErrInfo->Type         = McBankType;
    mcBankErrInfo->Socket       = host->prevBootErr.mcRegs[i].socketId;
    mcBankErrInfo->McBankNum    = host->prevBootErr.mcRegs[i].mcBankNum;
    *(UINT64_STRUCT*)(UINTN)&mcBankErrInfo->McBankStatus = host->prevBootErr.mcRegs[i].mcStatus;
    *(UINT64_STRUCT*)(UINTN)&mcBankErrInfo->McbankAddr   = host->prevBootErr.mcRegs[i].mcAddress;
    *(UINT64_STRUCT*)(UINTN)&mcBankErrInfo->McBankMisc   = host->prevBootErr.mcRegs[i].mcMisc;
#ifdef SERIAL_DBG_MSG
    /* rcPrintf (host, "PrevBootErrLog - skt: %d, MCBANK: %d, Status = %08x%08x, Addr = %08x%08x, Misc = %08x%08x\n",
        mcBankErrInfo->Socket, mcBankErrInfo->McBankNum,
        *(UINT32*)((UINTN)&mcBankErrInfo->McBankStatus + 4), *(UINT32*)((UINTN)&mcBankErrInfo->McBankStatus),
        *(UINT32*)((UINTN)&mcBankErrInfo->McbankAddr + 4), *(UINT32*)((UINTN)&mcBankErrInfo->McbankAddr),
        *(UINT32*)((UINTN)&mcBankErrInfo->McBankMisc + 4), *(UINT32*)((UINTN)&mcBankErrInfo->McBankMisc)
        ); */
#endif // SERIAL_DBG_MSG

#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
    LogMemUCE(host,mcBankErrInfo);
#endif	

    mcBankErrInfo++; // Move to next entry
  }
  //
  // Build HOB for prev boot errors
  //
  BuildGuidDataHob (
    &gPrevBootErrSrcHobGuid,
    &prevBootErrorHob,
    sizeof(PREV_BOOT_ERR_SRC_HOB)
    );
  return;
}

VOID
SaveNGNCfgCurRecordsToHob (
  struct sysHost  *host
  )
{

  UINT8 i;
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8 numCfgCurRecords = 0 ;
  struct NGNCfgCurRecordHob ngnCfgCurRecordHob ;
  struct memNvram *nvramMem ;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct cfgCurEntry *entry;
  struct dimmDevice *dimmDevice;

  nvramMem = &host->nvram.mem;
  mem = &(host->var.mem);

  DEBUG((EFI_D_ERROR, "NG NVM DIMM Current Configuration Records - HOB\n"));

  MemSetLocal ((UINT8 *) &ngnCfgCurRecordHob, 0, sizeof( struct NGNCfgCurRecordHob ));

  for( sckt = 0; sckt < MAX_SOCKET; sckt++ ) {

    if( nvramMem->socket[sckt].enabled == 0 ) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

    for(ch = 0; ch < MAX_CH; ch++) {

      if((*channelNvList)[ch].enabled == 0) continue;

      for(dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

        dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;

        if((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if(!(*dimmNvList)[dimm].aepDimmPresent) continue;

        chdimmList = &mem->socket[sckt].channelList[ch].dimmList;
        if( !( ( (*chdimmList)[dimm].ngnCfgCurPresent == 1 ) && ( (*chdimmList)[dimm].ngnCfgCurValid == 1 ) ) ) continue ;

        entry = &ngnCfgCurRecordHob.cfgCurRecord[numCfgCurRecords];
        numCfgCurRecords = numCfgCurRecords + 1;

        entry->socket = sckt ;
        entry->channel = ch ;
        entry->dimm = dimm ;

        for( i = 0 ; i < NGN_MAX_MANUFACTURER_STRLEN ; i++ )  {
          entry->manufacturer[i] = (*dimmNvList)[dimm].manufacturer[i] ;
        }

        for( i = 0 ; i < NGN_MAX_SERIALNUMBER_STRLEN ; i++ )  {
          entry->serialNumber[i] = (*dimmNvList)[dimm].serialNumber[i]  ;
        }

        for( i = 0 ; i < NGN_MAX_MODELNUMBER_STRLEN ; i++ ) {
          entry->modelNumber[i] = (*dimmNvList)[dimm].modelNumber[i];
        }

        dimmDevice = ( struct dimmDevice * ) &(mem->socket[sckt].channelList[ch].dimmList[dimm]);

        CopyMem((VOID *)&(entry->cfgCurData), (VOID *)&(dimmDevice->ngnCfgCur), sizeof( NGN_DIMM_CFG_CUR_HOST ));
		break; // This is because we support only one AEP dimm per channel, so once first dimm discovered we can break out of Dimm loop
      }
    }
  }
  DEBUG((EFI_D_ERROR, "numCfgCurRecords = %d\n", numCfgCurRecords));
  ngnCfgCurRecordHob.numCfgCurRecords = numCfgCurRecords ;

  BuildGuidDataHob( &gEfiNGNCfgCurGuid, &ngnCfgCurRecordHob, sizeof( struct NGNCfgCurRecordHob ) );
  return;
}

VOID
SaveNGNCfgOutRecordsToHob (
  struct sysHost  *host
  )
{

  UINT8 i;
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8 numCfgOutRecords = 0 ;
  struct NGNCfgOutRecordHob ngnCfgOutRecordHob ;
  struct memNvram *nvramMem ;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct cfgOutEntry *entry;
  struct dimmDevice *dimmDevice;

  nvramMem = &host->nvram.mem;
  mem = &(host->var.mem);

  DEBUG((EFI_D_ERROR, "NG NVM DIMM Output Configuration Records - HOB\n"));

  MemSetLocal ((UINT8 *) &ngnCfgOutRecordHob, 0, sizeof( struct NGNCfgOutRecordHob ));


  for( sckt = 0; sckt < MAX_SOCKET; sckt++ ) {

    if( nvramMem->socket[sckt].enabled == 0 ) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

    for(ch = 0; ch < MAX_CH; ch++) {

      if((*channelNvList)[ch].enabled == 0) continue;

      for(dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

        dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;

        if((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if(!(*dimmNvList)[dimm].aepDimmPresent) continue;

        chdimmList = &mem->socket[sckt].channelList[ch].dimmList;
        if( !( (*chdimmList)[dimm].ngnCfgOutPresent == 1 ) ) continue ;

        entry = &ngnCfgOutRecordHob.cfgOutRecord[numCfgOutRecords];
        numCfgOutRecords = numCfgOutRecords + 1;

        entry->socket = sckt ;
        entry->channel = ch ;
        entry->dimm = dimm ;

        for( i = 0 ; i < NGN_MAX_MANUFACTURER_STRLEN ; i++ )  {
          entry->manufacturer[i] = (*dimmNvList)[dimm].manufacturer[i] ;
        }

        for( i = 0 ; i < NGN_MAX_SERIALNUMBER_STRLEN ; i++ )  {
          entry->serialNumber[i] = (*dimmNvList)[dimm].serialNumber[i]  ;
        }

        for( i = 0 ; i < NGN_MAX_MODELNUMBER_STRLEN ; i++) {
          entry->modelNumber[i] = (*dimmNvList)[dimm].modelNumber[i];
        }

        dimmDevice = ( struct dimmDevice * ) &(mem->socket[sckt].channelList[ch].dimmList[dimm]);

        CopyMem((VOID *)&(entry->cfgOutData), (VOID *)&(dimmDevice->ngnCfgOut), sizeof( NGN_DIMM_CFG_OUT_HOST ));
     	break; // This is because we support only one AEP dimm per channel, so once first dimm discovered we can break out of Dimm loop
      }
    }
  }
  DEBUG((EFI_D_ERROR, "numCfgOutRecords = %d\n", numCfgOutRecords));
  ngnCfgOutRecordHob.numCfgOutRecords = numCfgOutRecords ;

  BuildGuidDataHob( &gEfiNGNCfgOutGuid, &ngnCfgOutRecordHob, sizeof( struct NGNCfgOutRecordHob ) );
  return;
}

VOID
SaveSystemMemoryMapToHob (
  struct sysHost  *host
  )
{
  struct SystemMemoryMapHob             systemMemoryMap;
  UINT8                                 socket, i, mc, ch, dimm, rank;
  UINT16                                intDimm;
  UINT16                                MemInfoMemFreq;
  UINT8                                 MemInfoDdrVoltage;
  UINT8                                 ScratchCounter = 0;
  struct memVar                         *mem;
  UINT32                                MemInfoTotalMem;
  UINT8                                 profile;
#ifdef BDAT_SUPPORT
#ifdef SSA_FLAG
  UINT8                                 GuidIdx;
#endif //SSA_FLAG
#endif //BDAT_SUPPORT
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  UINTN                                 VariableSize;
  EFI_STATUS                            Status;
  BOOLEAN                               DimmEnabled;
  struct dimmDevice                     *pDimmDevice;
  UINT8                                 Node;

  MemSetLocal ((UINT8 *) &systemMemoryMap, 0, sizeof(struct SystemMemoryMapHob));

  MemInfoMemFreq = 0;
  MemInfoDdrVoltage = 0;

  for(socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;

    MemInfoMemFreq = host->nvram.mem.socket[socket].ddrFreqMHz*2;
    MemInfoDdrVoltage = host->nvram.mem.socket[socket].ddrVoltage;

    break;

  } // node loop

  MemInfoTotalMem = (host->var.mem.memSize << 6) ;  // Memory size in 1MB units

  mem = &(host->var.mem);
  for(socket = 0; socket < MAX_SOCKET; socket++) {

    //Init Memory map structure here for NUMA
    if (host->setup.mem.options & NUMA_AWARE) {
      // For each SAD entry in system
      for(i = 0; i < SAD_RULES; i++) {
        // If SADTable entry is active, populate information into HOB
        if((mem->socket[socket].SAD[i].Enable) && (mem->socket[socket].SAD[i].IotEnabled != 1)) { //To ensure that IOT is not reported to systemMemoryMap/SRAT, other tables

          //Skip for remote socket
          if (mem->socket[socket].SAD[i].local == 0) continue;

          //If this SAD is created to map to Control region of the AEP dimms, we need to ignore it.
          if (mem->socket[socket].SAD[i].type == MEM_TYPE_CTRL) continue;

          //Initialize the socketId variable based on the socket number. This will be used in SRAT creation
          systemMemoryMap.Element[ScratchCounter].SocketId = socket;
          systemMemoryMap.Element[ScratchCounter].ImcInterBitmap = mem->socket[socket].SAD[i].imcInterBitmap;

          for (Node = 0; Node < MC_MAX_NODE; Node++) {
            if (mem->socket[socket].SADintList[i][Node] == 1) {
               systemMemoryMap.Element[ScratchCounter].NodeId = Node;
               break;
            }
          }
          
          if(i) {
            systemMemoryMap.Element[ScratchCounter].BaseAddress =  mem->socket[socket].SAD[i-1].Limit;
            systemMemoryMap.Element[ScratchCounter].ElementSize = (mem->socket[socket].SAD[i].Limit - mem->socket[socket].SAD[i-1].Limit); // Get size of current element
          } else {
            //
            // If its the First SAD, then base is always zero and limit is the size for the range.
            //
            systemMemoryMap.Element[ScratchCounter].BaseAddress = 0;
            systemMemoryMap.Element[ScratchCounter].ElementSize = mem->socket[socket].SAD[i].Limit - host->setup.common.lowGap; // Get size of current element
          }
          DEBUG((EFI_D_INFO, "SocketId:%d NodeId:%d ImcInterBitmap:%d BaseAddress:%08x ElementSize:%08x\n",
               systemMemoryMap.Element[ScratchCounter].SocketId,
               systemMemoryMap.Element[ScratchCounter].NodeId,
               systemMemoryMap.Element[ScratchCounter].ImcInterBitmap,
               systemMemoryMap.Element[ScratchCounter].BaseAddress,
               systemMemoryMap.Element[ScratchCounter].ElementSize));

          ScratchCounter++;    // Adding a valid entry
        }
      }
    }
    //
    // Copy SAD information to HOB, NOTE: In Purley the SAD & SADintList is in socket loop of the Hob.
    //
    CopyMem((VOID *)(&systemMemoryMap.Socket[socket].SADintList), (VOID *)(&(host->var.mem.socket[socket].SADintList)), sizeof(host->var.mem.socket[socket].SADintList));
    CopyMem((VOID *)(&systemMemoryMap.Socket[socket].SAD), (VOID *)(&(host->var.mem.socket[socket].SAD)), sizeof(host->var.mem.socket[socket].SAD));
  }
  systemMemoryMap.numberEntries = ScratchCounter;

  //systemMemoryMap.SADNum       = host->var.mem.SADNum;
  systemMemoryMap.lowMemBase   = host->var.mem.lowMemBase;
  systemMemoryMap.lowMemSize   = host->var.mem.lowMemSize;
  systemMemoryMap.highMemBase  = host->var.mem.highMemBase;
  systemMemoryMap.highMemSize  = host->var.mem.highMemSize;
  systemMemoryMap.memSize      = host->var.mem.memSize;
  systemMemoryMap.maxIMC       = host->var.mem.maxIMC;
  systemMemoryMap.maxCh        = MAX_CH;
  // Do we have at least one Aep Dimm Installed?
  systemMemoryMap.AepDimmPresent = host->nvram.mem.aepDimmPresent;

  CopyMem((VOID *)(&systemMemoryMap.WarningLog), (VOID *)(&(host->var.kti.WarningLog)), sizeof(host->var.kti.WarningLog));
#if ENHANCED_WARNING_LOG == 0
  CopyMem((VOID *)(&systemMemoryMap.status), (VOID *)(&(host->var.common.status)), sizeof(host->var.common.status));
#endif // ENHANCED_WARNING_LOG
  systemMemoryMap.memFreq = MemInfoMemFreq;
  systemMemoryMap.DdrVoltage = MemInfoDdrVoltage;

  systemMemoryMap.memMode = 0;
  if (host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP)
    systemMemoryMap.memMode = 1;
  if (host->nvram.mem.RASmode & CH_ALL_MIRROR)
    systemMemoryMap.memMode = 2;

  systemMemoryMap.XMPProfilesSup = host->nvram.mem.XMPProfilesSup;

  systemMemoryMap.cpuType        = host->var.common.cpuType;
  systemMemoryMap.cpuStepping    = host->var.common.stepping;
  systemMemoryMap.DimmType       = host->nvram.mem.dimmTypePresent;
  systemMemoryMap.DramType       = host->nvram.mem.dramType;
  systemMemoryMap.NumChPerMC     = host->var.mem.numChPerMC;
  systemMemoryMap.SystemRasType  = host->var.common.SystemRasType;
  systemMemoryMap.RasModesEnabled   = host->nvram.mem.RASmode;
  systemMemoryMap.ExRasModesEnabled = host->nvram.mem.RASmodeEx;
  systemMemoryMap.RasModesSupported =host->var.mem.RASModes;
  systemMemoryMap.sncEnabled     = host->var.kti.OutSncEn;
  systemMemoryMap.NumOfCluster   = host->var.kti.OutNumOfClusterPerSystem;
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  systemMemoryMap.IsColdBoot = host->var.mem.IsColdBoot;
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.

#ifdef BDAT_SUPPORT
#ifdef SSA_FLAG
    systemMemoryMap.bssaNumberDistinctGuids   = host->var.mem.bssaNumberDistinctGuids;
    DEBUG((EFI_D_ERROR, "BSSA: Disctinct result GUIDs: %d\n",systemMemoryMap.bssaNumberDistinctGuids));
    systemMemoryMap.bssaBdatSize = host->var.mem.bssaBdatSize;
    DEBUG((EFI_D_ERROR, "BSSA: Total BDAT size: %d\n",systemMemoryMap.bssaBdatSize));
    for (GuidIdx = 0; GuidIdx < MAX_NUMBER_SSA_BIOS_RESULTS; GuidIdx++) {
      systemMemoryMap.bssaBdatGuid[GuidIdx]   = host->var.mem.bssaBdatGuid[GuidIdx];
      systemMemoryMap.bssaNumberHobs[GuidIdx]   = host->var.mem.bssaNumberHobs[GuidIdx];
      DEBUG((EFI_D_ERROR, "BSSA: GuidIdx %d has %d HOBs\n", GuidIdx, systemMemoryMap.bssaNumberHobs[GuidIdx]));
    }
#endif //SSA_FLAG
#endif //BDAT_SUPPORT

  for (profile = 0; profile < 2; profile++) {
    if (host->nvram.mem.XMPProfilesSup & (1 << profile)) {
      systemMemoryMap.profileMemTime[profile] = host->nvram.mem.profileMemTime[profile];
    }
  }
  // Now fill Channel/DIMM info/TAD info
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for(mc = 0; mc < MAX_IMC; mc++) {
      systemMemoryMap.Socket[socket].imcEnabled[mc] = host->var.mem.socket[socket].imcEnabled[mc];
      systemMemoryMap.Socket[socket].imc[mc].MemSize = host->var.mem.socket[socket].imc[mc].memSize;
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].imc[mc].TAD), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TAD)),
                              sizeof(host->var.mem.socket[socket].imc[mc].TAD));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].imc[mc].TADChnIndex), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADChnIndex)),
                              sizeof(host->var.mem.socket[socket].imc[mc].TADChnIndex));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].imc[mc].TADOffset), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADOffset)),
                              sizeof(host->var.mem.socket[socket].imc[mc].TADOffset));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].imc[mc].TADintList), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADintList)),
                              sizeof(host->var.mem.socket[socket].imc[mc].TADintList));
    }
    for (ch = 0; ch < MAX_CH; ch++) {
      systemMemoryMap.Socket[socket].ChannelInfo[ch].Enabled  = host->nvram.mem.socket[socket].channelList[ch].enabled;
      systemMemoryMap.Socket[socket].ChannelInfo[ch].Features = host->nvram.mem.socket[socket].channelList[ch].features;
      systemMemoryMap.Socket[socket].ChannelInfo[ch].MaxDimm  = host->nvram.mem.socket[socket].channelList[ch].maxDimm;
      systemMemoryMap.Socket[socket].ChannelInfo[ch].NumRanks = host->nvram.mem.socket[socket].channelList[ch].numRanks;
      systemMemoryMap.Socket[socket].ChannelInfo[ch].chFailed = host->nvram.mem.socket[socket].channelList[ch].chFailed;
      systemMemoryMap.Socket[socket].ChannelInfo[ch].ngnChFailed = host->nvram.mem.socket[socket].channelList[ch].ngnChFailed;
      for(rank = 0; rank < MAX_SPARE_RANK; rank++){
        systemMemoryMap.Socket[socket].ChannelInfo[ch].SpareLogicalRank[rank] = host->nvram.mem.socket[socket].channelList[ch].spareLogicalRank[rank];
        systemMemoryMap.Socket[socket].ChannelInfo[ch].SpareRankSize[rank] = host->nvram.mem.socket[socket].channelList[ch].spareRankSize[rank];
      }
      systemMemoryMap.Socket[socket].ChannelInfo[ch].EnabledLogicalRanks = 0;
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].Present           = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmPresent;
        DimmEnabled = FALSE;
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmPresent == 1) {
          for (rank = 0;  rank < host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numDramRanks; rank++) {
            if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank] == 0) {
              DimmEnabled = TRUE;
            }
          }
        }
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].Enabled               = DimmEnabled;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].aepDimmPresent        = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].aepDimmPresent;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].VendorID              = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].VendorID;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].DeviceID              = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].DeviceID;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].RevisionID            = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].RevisionID;
#ifdef MEM_NVDIMM_EN
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].funcdesc              = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].funcdesc;
#endif
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].InterfaceFormatCode   = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].InterfaceFormatCode;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].subSysVendorID        = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].subSysVendorID;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].subSysDeviceID        = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].subSysDeviceID;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].subSysRevisionID      = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].subSysRevisionID;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].manufacturingLocation = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDMMfLoc;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].manufacturingDate     = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDModDate;
//APTIOV_SERVER_OVERRIDE_RC_START
        //RSD type 197 Memory Ext Info Start
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].firmwareRevision.majorVersion = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].firmwareRevision.majorVersion;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].firmwareRevision.minorVersion = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].firmwareRevision.minorVersion;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].firmwareRevision.hotfixVersion = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].firmwareRevision.hotfixVersion;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].firmwareRevision.buildVersion = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].firmwareRevision.buildVersion;
        
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].ApiVersion   = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].ApiVersion;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].MaxTdp   = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].MaxTdp;
//APTIOV_SERVER_OVERRIDE_RC_END
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].perRegionDPA        = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].perRegionDPA;

        pDimmDevice = ( struct dimmDevice * ) &(mem->socket[socket].channelList[ch].dimmList[dimm]);

        if (pDimmDevice->ngnCfgCurPresent && pDimmDevice->ngnCfgCurValid) {
          for (i = 0; i < MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
            for (intDimm = 0; intDimm < pDimmDevice->ngnCfgCur.interleave[i].NumOfDimmsInInterleaveSet; intDimm++) {
              if (
                (pDimmDevice->ngnCfgCur.interleaveId[i][intDimm].socket  == socket) &&
                (pDimmDevice->ngnCfgCur.interleaveId[i][intDimm].channel == ch) &&
                (pDimmDevice->ngnCfgCur.interleaveId[i][intDimm].dimm    == dimm)
                ) {

                systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].perDPAMap[i].perRegionDPAOffset = pDimmDevice->ngnCfgCur.interleaveId[i][intDimm].PartitionOffset;

                DEBUG(( EFI_D_ERROR, "\nsoc:%d; ch:%d; dimm:%d; Interleave:%d; intDimm:%d; perRegionDPAOffset: %02X ",
                  socket, ch, dimm, i, intDimm, systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].perDPAMap[i].perRegionDPAOffset )) ;

                break;
              }
            }
            systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].perDPAMap[i].SPALimit = pDimmDevice->ngnCfgCur.interleave[i].SadLimit;

            DEBUG(( EFI_D_ERROR, "\nsoc:%d; ch:%d; dimm:%d; Interleave:%d; intDimm:%d; SPALimit: %02X ",
              socket, ch, dimm, i, intDimm, systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].perDPAMap[i].SPALimit )) ;
          }
        }
        // If running on a Simics platform and AEP DIMM, then we want to get the serial number and manufacturer from the IdentifyDIMM command.  This is because it is easy to override these values in Simcis without
        // having to provide a new SPD image
        if(( IsSimicsPlatform() == TRUE ) && ( host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].aepDimmPresent ) )  {

          for (i = 0; i < NGN_MAX_SERIALNUMBER_STRLEN; i++)  {
            systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].serialNumber[i] = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].serialNumber[i];
            DEBUG(( EFI_D_ERROR, "%02X ", host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].serialNumber[i] )) ;
          }

          for (i = 0; i < NGN_MAX_MODELNUMBER_STRLEN; i++)  {
             systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].modelNumber[i] = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].modelNumber[i];
             DEBUG(( EFI_D_ERROR, "%02X ", host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].modelNumber[i] )) ;
          }
        }

        // Otherwise the serial number and manufacturer should be obtained from the SPD
        else  {
          // Update SerialNumber
          for (i = 0; i < 4; i++) {
            systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].serialNumber[i] =
              host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDModSN[i];
          }
          // Update PartNumber
          if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
            for (i = 0; i < SPD_MODULE_PART; i++) {
              systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].modelNumber[i] =
                host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDModPartDDR4[i];
            }
          } else {
            for (i = 0; i < SPD_MODULE_PART_DDR4; i++) {
              systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].modelNumber[i] =
                host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDModPartDDR4[i];          }
          }
        }


        for (i = 0; i < NGN_MAX_MANUFACTURER_STRLEN; i++)  {
          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].manufacturer[i] = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].manufacturer[i];
          DEBUG(( EFI_D_ERROR, "%02X ", host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].manufacturer[i] )) ;
        }


        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NumRanks          = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numRanks;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].X4Present         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].x4Present;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMMfgId         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDMMfgId;
      // Supermicro+  
#if SMCPKG_SUPPORT
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDModDate         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDModDate;  
#endif
      // Supermicro-
        if(host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].aepDimmPresent)
          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].DimmSize          = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rawCap;
        else
          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].DimmSize          = host->var.mem.socket[socket].channelList[ch].dimmList[dimm].memSize;
#ifdef NVMEM_FEATURE_EN
        if (host->setup.mem.ADREn && host->setup.mem.ADRDataSaveMode == ADR_NVDIMM)
          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NVmemSize          = host->var.mem.socket[socket].channelList[ch].dimmList[dimm].NVmemSize;
        else
          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NVmemSize = 0;
#else
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NVmemSize		    = 0;
#endif
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDSDRAMBanks     = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDSDRAMBanks;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actSPDModuleOrg   = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actSPDModuleOrg;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].keyByte           = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].keyByte;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actKeyByte2       = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actKeyByte2;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actSPDSDRAMBanks  = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actSPDSDRAMBanks;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMemBusWidth    = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDMemBusWidth;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].newDimm           = host->var.mem.socket[socket].channelList[ch].dimmList[dimm].newDimm;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].nonVolCap         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].nonVolCap;
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].dimmTs            = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmTs;
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled == 0) continue;
          if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank]) continue;
          systemMemoryMap.Socket[socket].ChannelInfo[ch].EnabledLogicalRanks |=
            1 <<  host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].logicalRank;

          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].enabled =
            host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled;

          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].logicalRank =
            host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].logicalRank;

          systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].rankSize =
            host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].rankSize;
        } // rank loop
        //fill dimm minTCK, serial number and part number for smbios type17
        systemMemoryMap.Socket[socket].ChannelInfo[ch].DimmInfo[dimm].commonTck =
          host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].minTCK;


      } // dimm loop

      //Copy Address Range Partial Mirroring Information
      Status = PeiServicesLocatePpi (
                    &gEfiPeiReadOnlyVariable2PpiGuid,
                    0,
                    NULL,
                    &PeiVariable
                  );
      if(Status == EFI_SUCCESS) {
        VariableSize = sizeof(systemMemoryMap.RasMeminfo.MirrorRequestType);
        Status = PeiVariable->GetVariable (
                                  PeiVariable,
                                  ADDRESS_RANGE_MIRROR_VARIABLE_REQUEST,
                                  &gAddressBasedMirrorGuid,
                                  NULL,
                                  &VariableSize,
                                  &systemMemoryMap.RasMeminfo.MirrorRequestType
                                  );
        if(Status != EFI_SUCCESS) {
          MemSetLocal((UINT8 *) &systemMemoryMap.RasMeminfo.MirrorRequestType, 0, (UINT32)VariableSize); //MemoryMapOut_SUPPORT
        }
        systemMemoryMap.RasMeminfo.MirrorCurrentType.MirrorVersion = ADDRESS_RANGE_MIRROR_VARIABLE_VERSION;
        if(host->setup.mem.partialMirrorUEFI == 1) {
          systemMemoryMap.RasMeminfo.MirrorCurrentType.MirrorMemoryBelow4GB = host->setup.mem.partialmirrorsad0;
          systemMemoryMap.RasMeminfo.MirrorCurrentType.MirroredAmountAbove4GB = (UINT16)host->setup.mem.partialmirrorpercent;
        } else {
          systemMemoryMap.RasMeminfo.MirrorCurrentType.MirrorMemoryBelow4GB = 0;
          systemMemoryMap.RasMeminfo.MirrorCurrentType.MirroredAmountAbove4GB = 0;
        }
        systemMemoryMap.RasMeminfo.MirrorCurrentType.MirrorStatus = host->setup.mem.partialmirrorsts;
      }



      //
      // Copy RIR information
      //
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].ChannelInfo[ch].rirIntList),
                              (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirIntList)), sizeof(host->var.mem.socket[socket].channelList[ch].rirIntList));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].ChannelInfo[ch].rirLimit),
                              (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirLimit)), sizeof(host->var.mem.socket[socket].channelList[ch].rirLimit));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].ChannelInfo[ch].rirWay),
                              (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirWay)), sizeof(host->var.mem.socket[socket].channelList[ch].rirWay));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].ChannelInfo[ch].rirValid),
                              (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirValid)), sizeof(host->var.mem.socket[socket].channelList[ch].rirValid));
      CopyMem((VOID *)(&systemMemoryMap.Socket[socket].ChannelInfo[ch].rirOffset),
                              (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirOffset)), sizeof(host->var.mem.socket[socket].channelList[ch].rirOffset));
    } // ch loop

  } // socket loop
  MemCopy ((UINT8 *)&(systemMemoryMap.MemSetup), (UINT8 *)&host->setup.mem, sizeof (struct memSetup));
  CopyMem((VOID *)(&systemMemoryMap.pprStatus), (VOID *)(&(host->var.mem.pprStatus)), sizeof(host->var.mem.pprStatus));
  //
  // Build HOB for system memory map
  //
  BuildGuidDataHob (
    &gEfiMemoryMapGuid,
    &systemMemoryMap,
    sizeof (struct SystemMemoryMapHob)
    );
  return;
}

#ifdef LT_FLAG
VOID
SavePlatformTxtDataToHob (
  IIO_UDS           *IioUds,
  EFI_PLATFORM_INFO *PlatformInfo
  )
{
  EFI_PLATFORM_TXT_DEVICE_MEMORY        PlatformTxtDeviceMemory;
  EFI_PHYSICAL_ADDRESS                  TxtDprAddress;
  EFI_PHYSICAL_ADDRESS                  TxtDevAddress;
  UINT32                                LtMem = 0;
  UINT32                                LtMemSize = 0;
  UINT32                                DprSize = 0;
  if (IioUds->SystemStatus.LtsxEnable || PlatformInfo->MemData.BiosGuardMemSize != 0) {
    DEBUG((EFI_D_ERROR, "Reserve LT Mem!Should equal (MemTolm - MemTsegSize - LtMemSize) = 0x%08x\n", IioUds->PlatformData.MemTolm - PlatformInfo->MemData.MemTsegSize - PlatformInfo->MemData.MemLtMemSize));
    DprSize = PlatformInfo->MemData.MemDprMemSize;
    LtMem = PlatformInfo->MemData.MemLtMemAddress;
    DEBUG((EFI_D_ERROR, "LtMem: 0x%x\n",    LtMem));
    LtMem &= (~(LT_DEV_MEM_ALIGNMENT-1));
    LtMemSize = PlatformInfo->MemData.MemLtMemSize;
    if (PcdGetBool(PcdDprSizeFeatureSupport)) {
      DprSize   = PcdGet32 (PcdDprMemSize);
      LtMemSize = PcdGet32 (PcdLtMemSize);
    }
    TxtDprAddress = LtMem + LtMemSize - DprSize;
    TxtDevAddress = LtMem;
    PlatformTxtDeviceMemory.NoDMATableAddress                = TxtDevAddress;
    PlatformTxtDeviceMemory.NoDMATableSize                   = LtMemSize - DprSize;
    PlatformTxtDeviceMemory.SINITMemoryAddress               = TxtDprAddress + DprSize - LT_HEAP_SIZE - SINIT_MAX_SIZE;
    PlatformTxtDeviceMemory.SINITMemorySize                  = SINIT_MAX_SIZE;
    PlatformTxtDeviceMemory.TXTHeapMemoryAddress             = TxtDprAddress + DprSize - LT_HEAP_SIZE;
    PlatformTxtDeviceMemory.TXTHeapMemorySize                = LT_HEAP_SIZE;
    PlatformTxtDeviceMemory.DMAProtectionMemoryRegionAddress = LtMem - (UINT32)LShiftU64 (PlatformInfo->MemData.BiosGuardMemSize, 20); //TxtDprAddress;
    PlatformTxtDeviceMemory.DMAProtectionMemoryRegionSize    = (UINT32)LShiftU64 (PlatformInfo->MemData.BiosGuardMemSize, 20) + LtMemSize;
    DEBUG((EFI_D_ERROR, "========== LT Memory space ========== \n"));
    DEBUG((EFI_D_ERROR, "NoDMATableAddress: 0x%x\n",    PlatformTxtDeviceMemory.NoDMATableAddress));
    DEBUG((EFI_D_ERROR, "NoDMATableSize: 0x%x\n",       PlatformTxtDeviceMemory.NoDMATableSize));
    DEBUG((EFI_D_ERROR, "SINITMemoryAddress: 0x%x\n",   PlatformTxtDeviceMemory.SINITMemoryAddress));
    DEBUG((EFI_D_ERROR, "SINITMemorySize: 0x%x\n",      PlatformTxtDeviceMemory.SINITMemorySize));
    DEBUG((EFI_D_ERROR, "TXTHeapMemoryAddress: 0x%x\n", PlatformTxtDeviceMemory.TXTHeapMemoryAddress));
    DEBUG((EFI_D_ERROR, "TXTHeapMemorySize: 0x%x\n",    PlatformTxtDeviceMemory.TXTHeapMemorySize));
    DEBUG((EFI_D_ERROR, "DMAProtectionMemoryRegionAddress: 0x%x\n", PlatformTxtDeviceMemory.DMAProtectionMemoryRegionAddress));
    DEBUG((EFI_D_ERROR, "DMAProtectionMemoryRegionSize: 0x%x\n\n",  PlatformTxtDeviceMemory.DMAProtectionMemoryRegionSize));
    BuildGuidDataHob (&gEfiPlatformTxtDeviceMemoryGuid, &PlatformTxtDeviceMemory, sizeof(PlatformTxtDeviceMemory));
  }
  return;
}
#endif //LT_FLAG

VOID
SaveIioUdsDataToHob (
  struct sysHost             *host
  )
{
  IIO_UDS                               IioUds;         // Module Universal Data Store!
  EFI_GUID                              UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_PLATFORM_INFO                     *PlatformInfo;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  UINT8                                 Ctr1,Ctr2;
  UINT8                                 socket, Stack, port;
  UINT8                                 ch, mc;
  UINT16                                ioLimit;
  UINT32                                mmiolLimit;
  UINT64                                mmiohLimit;
  UINT32                                CapId5;
  EFI_STATUS                            Status;

  //
  // Update the platform info hob with system PCI resource info based on CSI output structure
  //
  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    return;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  // Initialize UDS stack variables to zero
  MemSetLocal ((UINT8 *) &IioUds, 0, sizeof(IioUds));

  // Initialize CpuAccSupport, it will be set false if any sockets do not support
  IioUds.SystemStatus.CpuAccSupport = TRUE;

  // report CPU 0 resource into PlatformInfo
  IioUds.PlatformData.PlatGlobalIoBase      = 0x1000;//host->var.kti.CpuInfo[0].CpuRes.IoBase;
  IioUds.PlatformData.PlatGlobalIoLimit     = host->var.kti.CpuInfo[0].CpuRes.IoLimit;
  //
  // Get the highest IO limit in the system
  //
  for(Ctr1 = 1; Ctr1 < MAX_SOCKET; Ctr1++) {
    ioLimit = host->var.kti.CpuInfo[Ctr1].CpuRes.IoLimit;
    if(IioUds.PlatformData.PlatGlobalIoLimit < ioLimit)
      IioUds.PlatformData.PlatGlobalIoLimit = ioLimit;
  }
  IioUds.PlatformData.PlatGlobalMmiolBase   = host->var.kti.CpuInfo[0].CpuRes.MmiolBase;
  IioUds.PlatformData.PlatGlobalMmiolLimit  = host->var.kti.CpuInfo[0].CpuRes.MmiolLimit;
  //
  // Get the highest MMIOL limit in the system
  //
  for(Ctr1 = 1; Ctr1 < MAX_SOCKET; Ctr1++) {
    mmiolLimit = host->var.kti.CpuInfo[Ctr1].CpuRes.MmiolLimit;
    if(IioUds.PlatformData.PlatGlobalMmiolLimit < mmiolLimit)
      IioUds.PlatformData.PlatGlobalMmiolLimit = mmiolLimit;
  }
  IioUds.PlatformData.PlatGlobalMmiohBase   = LShiftU64((UINT64)(host->var.kti.CpuInfo[0].CpuRes.MmiohBase.hi), 32);
  IioUds.PlatformData.PlatGlobalMmiohBase   |= (UINT64)host->var.kti.CpuInfo[0].CpuRes.MmiohBase.lo;
  IioUds.PlatformData.PlatGlobalMmiohLimit  = LShiftU64((UINT64)(host->var.kti.CpuInfo[0].CpuRes.MmiohLimit.hi), 32);
  IioUds.PlatformData.PlatGlobalMmiohLimit  |= (UINT64)host->var.kti.CpuInfo[0].CpuRes.MmiohLimit.lo;
  //
  // Get the highest MMIOH limit in the system
  //
  for(Ctr1 = 1; Ctr1 < MAX_SOCKET; Ctr1++) {
    mmiohLimit = LShiftU64((UINT64)(host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohLimit.hi), 32);
    mmiohLimit |= (UINT64)host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohLimit.lo;
    if(IioUds.PlatformData.PlatGlobalMmiohLimit < mmiohLimit) {
      IioUds.PlatformData.PlatGlobalMmiohLimit = mmiohLimit;
    }
  }

  IioUds.PlatformData.IoGranularity    = host->var.kti.OutIoGranularity;
  IioUds.PlatformData.MmiolGranularity = host->var.kti.OutMmiolGranularity;
  IioUds.PlatformData.MmiohGranularity = host->var.kti.OutMmiohGranularity;


  IioUds.PlatformData.PciExpressBase         = host->var.common.mmCfgBase;
  IioUds.PlatformData.PciExpressSize         = host->setup.common.mmCfgSize;
  DEBUG((EFI_D_ERROR, "mmCfgBase  %x\n",host->var.common.mmCfgBase));
  IioUds.PlatformData.MemTolm                = host->var.mem.lowMemSize * CONVERT_64MB_TO_BYTE;
  IioUds.PlatformData.Pci64BitResourceAllocation = host->var.common.Pci64BitResourceAllocation;

  // Init UDS data here
  IioUds.PlatformData.MemTsegSize             = PlatformInfo->MemData.MemTsegSize;
  IioUds.PlatformData.MemIedSize              = PlatformInfo->MemData.MemIedSize;
  IioUds.PlatformData.numofIIO                = 0;
  IioUds.PlatformData.MaxBusNumber            = 0xFF;

  for (Ctr1 = 0; Ctr1 < MAX_SOCKET; ++Ctr1) {
    if((host->var.common.socketPresentBitMap & (1 << Ctr1))) {
      IioUds.PlatformData.packageBspApicID[Ctr1]                   = host->var.cpu.packageBspApicID[Ctr1];  // for CpuSv <AS>
      IioUds.PlatformData.CpuQpiInfo[Ctr1].Valid                   = 1;
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocId                   = Ctr1;
      DEBUG((EFI_D_ERROR, "Socket = %x\n",Ctr1 ));
      for (Ctr2 = 0; Ctr2 < MAX_IIO_STACK; ++Ctr2) {
        IioUds.PlatformData.CpuQpiInfo[Ctr1].StackBus[Ctr2] = (UINT8)host->var.common.StackBus[Ctr1][Ctr2];
        DEBUG((EFI_D_ERROR, "  StackBus = %x\n",IioUds.PlatformData.CpuQpiInfo[Ctr1].StackBus[Ctr2] ));
      }
      for (Ctr2 = 0; Ctr2 < MAX_KTI_PORTS; ++Ctr2) {
        IioUds.PlatformData.CpuQpiInfo[Ctr1].PeerInfo[Ctr2].Valid = (UINT8)host->var.kti.CpuInfo[Ctr1].LepInfo[Ctr2].Valid;
        IioUds.PlatformData.CpuQpiInfo[Ctr1].PeerInfo[Ctr2].PeerSocId = (UINT8)host->var.kti.CpuInfo[Ctr1].LepInfo[Ctr2].PeerSocId;
        IioUds.PlatformData.CpuQpiInfo[Ctr1].PeerInfo[Ctr2].PeerSocType = (UINT8)host->var.kti.CpuInfo[Ctr1].LepInfo[Ctr2].PeerSocType;
        IioUds.PlatformData.CpuQpiInfo[Ctr1].PeerInfo[Ctr2].PeerPort = (UINT8)host->var.kti.CpuInfo[Ctr1].LepInfo[Ctr2].PeerPort;
      }

      IioUds.PlatformData.CpuQpiInfo[Ctr1].TotM3Kti = (UINT8)host->var.kti.CpuInfo[Ctr1].TotM3Kti;
      IioUds.PlatformData.CpuQpiInfo[Ctr1].ChaList = (UINT8)host->var.kti.CpuInfo[Ctr1].ChaList;
      IioUds.PlatformData.CpuQpiInfo[Ctr1].TotCha = (UINT8)host->var.kti.CpuInfo[Ctr1].TotCha;

      IioUds.PlatformData.CpuQpiInfo[Ctr1].M2PciePresentBitmap = (UINT8)host->var.common.M2PciePresentBitmap[Ctr1];
      IioUds.PlatformData.CpuQpiInfo[Ctr1].stackPresentBitmap = (UINT8)host->var.common.stackPresentBitmap[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d stackPresentBitmap = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].stackPresentBitmap ));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketFirstBus = (UINT8)host->var.common.SocketFirstBus[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d FirstBus = %x\n",Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketFirstBus ));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketLastBus = (UINT8)host->var.common.SocketLastBus[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d LastBus = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketLastBus ));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].PcieSegment = host->var.kti.CpuInfo[Ctr1].CpuRes.PciSegment;
      DEBUG((EFI_D_ERROR, "  Socket %2d PcieSegment = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].PcieSegment ));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].segmentSocket = (UINT8)host->var.common.segmentSocket[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d segmentSocket = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].segmentSocket ));

      IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase = host->var.kti.CpuInfo[Ctr1].CpuRes.SegMmcfgBase;
      DEBUG((EFI_D_ERROR, "  Socket %2d SegMmcfgBase = %x %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase.hi, IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase.lo));

      IioUds.PlatformData.IioQpiInfo[Ctr1].Valid = 1;
      IioUds.PlatformData.IioQpiInfo[Ctr1].SocId = Ctr1;

      IioUds.PlatformData.numofIIO++;
      IioUds.PlatformData.IIO_resource[Ctr1].Valid       = 1;
      IioUds.PlatformData.IIO_resource[Ctr1].SocketID    = Ctr1;
      IioUds.PlatformData.IIO_resource[Ctr1].BusBase     = host->var.kti.CpuInfo[Ctr1].CpuRes.BusBase;
      IioUds.PlatformData.IIO_resource[Ctr1].BusLimit    = host->var.kti.CpuInfo[Ctr1].CpuRes.BusLimit;
      DEBUG((EFI_D_ERROR, "KTI: IIO[%d] busbase = %x Limit=%x\n",Ctr1,IioUds.PlatformData.IIO_resource[Ctr1].BusBase,
                 IioUds.PlatformData.IIO_resource[Ctr1].BusLimit));

      //
      // Check CapId5 to see if ACC is enabled on the socket
      //
      CapId5 = ReadCpuPciCfgEx (host, Ctr1, 0, CAPID5_PCU_FUN3_REG);
      if((UINT8)((CapId5 >> 25) & ACC_ENABLE) != (UINT8)ACC_ENABLE) {
        IioUds.SystemStatus.CpuAccSupport = FALSE;
      }

      //
      // TODO : Hardcode this to 0x1000 if we're the legacy IIO to make sure that PCI root ports are
      // not trying to decode legacy IO ports (ACPI, GPIO, etc).
      //
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoBase     = host->var.kti.CpuInfo[Ctr1].CpuRes.IoBase;
      if(IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoBase == 0)
         IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoBase = 0x1000; //Iobase 0 was set to 0 from RC, need to reserve legacy IO
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoLimit    = host->var.kti.CpuInfo[Ctr1].CpuRes.IoLimit;
      DEBUG((EFI_D_ERROR, "KTI: IIO[%d] IoBase = %x IoLimit=%x\n",Ctr1,IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoBase,
                       IioUds.PlatformData.IIO_resource[Ctr1].PciResourceIoLimit));

      IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase            = host->var.kti.CpuInfo[Ctr1].CpuRes.IoApicBase;
      IioUds.PlatformData.IIO_resource[Ctr1].IoApicLimit           = host->var.kti.CpuInfo[Ctr1].CpuRes.IoApicLimit;
      if((IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase < IOAPIC_BASE) ||
         (IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase >= (IOAPIC_BASE + 0x100000))) {
         IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase = IOAPIC_BASE + Ctr1 * 0x40000;
         DEBUG((EFI_D_ERROR, "KTI: IoApicBase = 0! Change it to %x\n",IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase ));
      }
      DEBUG((EFI_D_ERROR, "KTI: IIO[%d] IoApicBase = %x IoApicLimit=%x\n",Ctr1,IioUds.PlatformData.IIO_resource[Ctr1].IoApicBase,
                 IioUds.PlatformData.IIO_resource[Ctr1].IoApicLimit ));
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem32Base  = host->var.kti.CpuInfo[Ctr1].CpuRes.MmiolBase;
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem32Limit = host->var.kti.CpuInfo[Ctr1].CpuRes.MmiolLimit;
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem64Base  = LShiftU64((UINT64)(host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohBase.hi), 32);
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem64Base  |= (UINT64)host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohBase.lo;
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem64Limit = LShiftU64((UINT64)(host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohLimit.hi), 32);
      IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem64Limit |= (UINT64)host->var.kti.CpuInfo[Ctr1].CpuRes.MmiohLimit.lo;
      DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Mem32Base = %x Mem32Limit=%x\n",Ctr1,IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem32Base,
                 IioUds.PlatformData.IIO_resource[Ctr1].PciResourceMem32Limit ));

      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        if((host->var.common.stackPresentBitmap[Ctr1] & (1 << Stack))) {
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].Personality = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].Personality;

          //
          // Update BusBase and BusLimit per Stack
          //
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].BusBase     = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].BusBase;
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].BusLimit    = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].BusLimit;
          DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Stack[%d] busbase = %x Limit=%x\n",Ctr1,Stack,IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].BusBase,
                     IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].BusLimit));
          //
          // Update IoBase and IoLimit per Stack
          // Hardcode this to 0x1000 if we're the legacy IIO to make sure that PCI root ports are
          // not trying to decode legacy IO ports (ACPI, GPIO, etc).
          //
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoBase     = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].IoBase;
          if(IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoBase == 0)
             IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoBase = 0x1000; //Iobase 0 was set to 0 from RC, need to reserve legacy IO
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoLimit    = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].IoLimit;
          DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Stack[%d] IoBase = %x IoLimit=%x\n",Ctr1,Stack,IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoBase,
                           IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceIoLimit));

          //
          // Update IoApicBase and IoApicLimit per Stack
          //
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase            = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].IoApicBase;
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicLimit           = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].IoApicLimit;
          if((IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase < IOAPIC_BASE) ||
             (IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase >= (IOAPIC_BASE + 0x100000))) {
             IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase = IOAPIC_BASE + Ctr1 * 0x40000;
             DEBUG((EFI_D_ERROR, "KTI: IoApicBase = 0! Change it to %x\n",IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase));
          }
          DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Stack[%d] IoApicBase = %x IoApicLimit=%x\n",Ctr1,Stack,IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicBase,
                     IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].IoApicLimit));

          //
          // Update Mmiol limit\base, Mmioh limit\base per Stack
          //
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem32Base  = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiolBase;
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem32Limit = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiolLimit;
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem64Base  = LShiftU64((UINT64)(host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiohBase.hi), 32);
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem64Base  |= (UINT64)host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiohBase.lo;
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem64Limit = LShiftU64((UINT64)(host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiohLimit.hi), 32);
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem64Limit |= (UINT64)host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiohLimit.lo;
          DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Stack[%d] Mem32Base = %x Mem32Limit=%x\n",Ctr1,Stack,IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem32Base,
                     IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].PciResourceMem32Limit));

          // Reserve Vt-d BAR Address of size V_VT_D_IOH_BAR_SIZE, naturally aligned on 8KB boundary as required by the chipset
          IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].VtdBarAddress = host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiolLimit & (~0x3FFF);
          // Reserve Vt-d BAR Address for FPGA
          if ((host->var.kti.OutKtiFpgaEnable[Ctr1]) && (Stack == IIO_PSTACK3 )){
            IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].VtdBarAddress = (host->var.kti.CpuInfo[Ctr1].CpuRes.StackRes[Stack].MmiolLimit - FPGA_PREALLOCATE_MEM_SIZE) & (~0x3FFF);
          }
          DEBUG((EFI_D_ERROR, "KTI: IIO[%d] Stack[%x] VtdBarAddress = %x \n",Ctr1, Stack, IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].VtdBarAddress));
          // If DMI calculate DMIRCBAR Address
          if ((Ctr1 == 0) && (Stack == 0)){
            IioUds.PlatformData.IIO_resource[Ctr1].RcBaseAddress   = IioUds.PlatformData.IIO_resource[Ctr1].StackRes[Stack].VtdBarAddress + V_VT_D_IIO_BAR_SIZE;
            DEBUG((EFI_D_ERROR, "KTI: RcBaseAddress = %x\n",IioUds.PlatformData.IIO_resource[Ctr1].RcBaseAddress));
          }
          // Disable the IOAPIC in the stack where FPGA connected.
          if ((host->var.kti.OutKtiFpgaEnable[Ctr1]) && (Stack == IIO_PSTACK3 )){
#if MAX_SOCKET > 4
            PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET) + Stack) + PCH_IOAPIC)));
#else
            PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * MAX_IIO_STACK + Stack) + PCH_IOAPIC)));
#endif
            DEBUG((EFI_D_ERROR, "KTI: disable IoApic[%d][%d] For FPGA \n",Ctr1, Stack));
          }

        } else {
#if MAX_SOCKET > 4
          if (Stack >= (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET)) continue;
          PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET) + Stack) + PCH_IOAPIC)));
#else
          PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * MAX_IIO_STACK + Stack) + PCH_IOAPIC)));
#endif
          DEBUG((EFI_D_ERROR, "KTI: IoApic[%d][%d] is invalid\n",Ctr1, Stack));
        }
      }
    } else if (host->var.common.FpgaPresentBitMap & (1 << Ctr1)) {
      //
      // Update common structures w/ updated bus numbers for FPGA to be used in CSR access routines after DXE
      //
      DEBUG((EFI_D_ERROR, "FPGA = %x\n", Ctr1));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketFirstBus = (UINT8)host->var.common.SocketFirstBus[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d FirstBus = %x\n",Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketFirstBus));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketLastBus = (UINT8)host->var.common.SocketLastBus[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d LastBus = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SocketLastBus));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase = host->var.kti.CpuInfo[Ctr1].CpuRes.SegMmcfgBase;
      DEBUG((EFI_D_ERROR, "  Socket %2d SegMmcfgBase = %x %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase.hi, IioUds.PlatformData.CpuQpiInfo[Ctr1].SegMmcfgBase.lo));
      IioUds.PlatformData.CpuQpiInfo[Ctr1].segmentSocket = (UINT8)host->var.common.segmentSocket[Ctr1];
      DEBUG((EFI_D_ERROR, "  Socket %2d segmentSocket = %x\n", Ctr1, IioUds.PlatformData.CpuQpiInfo[Ctr1].segmentSocket));

      for (Ctr2 = 0; Ctr2 < MAX_IIO_STACK; Ctr2++) {
        IioUds.PlatformData.CpuQpiInfo[Ctr1].StackBus[Ctr2] = (UINT8)host->var.common.StackBus[Ctr1][Ctr2];
        DEBUG((EFI_D_ERROR, "  StackBus = %x\n", IioUds.PlatformData.CpuQpiInfo[Ctr1].StackBus[Ctr2]));
        //
        // Disable the unavaiable IoApic for FPGA
        //
#if MAX_SOCKET > 4
        if (Ctr2 >= (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET)) continue;
        PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET) + Ctr2) + PCH_IOAPIC)));
#else
        PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * MAX_IIO_STACK + Ctr2) + PCH_IOAPIC)));
#endif
      }
      DEBUG((EFI_D_ERROR, "KTI: IoApic[%d] is invalid\n",Ctr1));

    } else {
      IioUds.PlatformData.CpuQpiInfo[Ctr1].Valid      = 0;
      IioUds.PlatformData.CpuQpiInfo[Ctr1].SocId      = Ctr1;
      IioUds.PlatformData.IioQpiInfo[Ctr1].Valid      = 0;
      IioUds.PlatformData.IioQpiInfo[Ctr1].SocId      = Ctr1;
      IioUds.PlatformData.IIO_resource[Ctr1].Valid    = 0;
      IioUds.PlatformData.IIO_resource[Ctr1].SocketID = Ctr1;
      for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
#if MAX_SOCKET > 4
        if (Stack >= (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET)) continue;
        PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET) + Stack) + PCH_IOAPIC)));
#else
        PlatformInfo->SysData.SysIoApicEnable &= (~(1 << ((Ctr1 * MAX_IIO_STACK + Stack) + PCH_IOAPIC)));
#endif
      }
      DEBUG((EFI_D_ERROR, "KTI: IoApic[%d] is invalid\n",Ctr1));
      DEBUG((EFI_D_ERROR, "KTI: CPU[%d] is invalid\n",Ctr1));
    }
    //
    // Set PCIE port information for later usage
    //
    CopyMem (&(IioUds.PlatformData.IIO_resource[Ctr1].PcieInfo), &(host->var.iio.PreLinkData.PcieInfo), sizeof (IIO_DMI_PCIE_INFO));
  }

  DEBUG((EFI_D_ERROR, "KTI: System ACPI IOAPICs enabled = %08x\n", PlatformInfo->SysData.SysIoApicEnable));

  IioUds.SystemStatus.numCpus                  = host->var.common.numCpus;
  DEBUG((EFI_D_ERROR, "KTI: num of Cpus = %d\n",IioUds.SystemStatus.numCpus));
  DEBUG((EFI_D_ERROR, "KTI: num of IIOs = %d\n",IioUds.PlatformData.numofIIO));
  IioUds.SystemStatus.cpuType                  = host->var.common.cpuType;
  IioUds.SystemStatus.SystemRasType            = host->var.common.SystemRasType;
  IioUds.SystemStatus.socketPresentBitMap      = host->var.common.socketPresentBitMap;
  IioUds.SystemStatus.FpgaPresentBitMap        = host->var.common.FpgaPresentBitMap;
  IioUds.SystemStatus.tolmLimit                = host->var.common.tolmLimit;
  IioUds.SystemStatus.tohmLimit                = host->var.common.tohmLimit;
  IioUds.SystemStatus.mmCfgBase                = host->var.common.mmCfgBase;

  IioUds.SystemStatus.CurrentCsiLinkSpeed      = host->var.kti.KtiCurrentLinkSpeedMode;
  IioUds.SystemStatus.CurrentCsiLinkFrequency  = host->var.kti.OutKtiLinkSpeed;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    IioUds.SystemStatus.OutKtiPerLinkL1En[socket] = 0;
    for (port = 0; port < host->var.common.KtiPortCnt; port++) {
      IioUds.SystemStatus.OutKtiPerLinkL1En[socket] |= (host->var.kti.OutKtiPerLinkL1En[socket][port] << port);
    }
  }

  IioUds.SystemStatus.RcVersion                = host->nvram.common.rcVersion;
  IioUds.SystemStatus.MinimumCpuStepping       = host->var.common.stepping;
  IioUds.SystemStatus.IsocEnable               = host->var.kti.OutIsocEn;
  IioUds.SystemStatus.meRequestedSize          = host->var.common.meRequestedSize;
  // RAS
  IioUds.SystemStatus.bootMode                 = host->var.common.bootMode;
  IioUds.SystemStatus.maxCh                    = MAX_CH;
  IioUds.SystemStatus.numChPerMC               = host->var.mem.numChPerMC;
  IioUds.SystemStatus.maxIMC                   = host->var.mem.maxIMC;
  IioUds.SystemStatus.LlcSizeReg               = (UINT16)((host->var.common.procCom[0].capid0 >> 9) & 0x7);    //CAPID0.Bits[11:9]  - LLC size
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    IioUds.PlatformData.SkuPersonality[socket]   = host->var.iio.IioVData.SkuPersonality[socket];
    IioUds.PlatformData.MaxCoreToBusRatio[socket] = host->var.common.MaxCoreToBusRatio[socket];
    IioUds.PlatformData.MinCoreToBusRatio[socket] = host->var.common.MinCoreToBusRatio[socket];
    IioUds.SystemStatus.memNode[socket] = host->nvram.mem.socket[socket].enabled;
    for (mc = 0; mc < MAX_IMC; mc++) {
      IioUds.SystemStatus.imcEnabled[socket][mc] = host->var.mem.socket[socket].imcEnabled[mc];
    }
    for (ch = 0; ch < MAX_CH; ch++) {
      IioUds.SystemStatus.mcId[socket][ch] = host->var.mem.socket[socket].channelList[ch].mcId;
      IioUds.SystemStatus.chEnabled[socket][ch] = host->nvram.mem.socket[socket].channelList[ch].enabled;
    }
  } // socket

  IioUds.PlatformData.EVMode           = host->var.common.EVMode;
  // Fill in SNC Enabled bit
  IioUds.SystemStatus.OutSncEn = host->var.kti.OutSncEn;
  IioUds.SystemStatus.OutNumOfCluster = host->var.kti.OutNumOfClusterPerSystem;
  IioUds.SystemStatus.IoDcMode        = host->var.kti.OutIoDcMode;
  IioUds.PlatformData.RemoteRequestThreshold = host->var.kti.OutRrqThreshold;

#ifdef LT_FLAG
  if(IioUds.PlatformData.EVMode)
    IioUds.SystemStatus.LtsxEnable      = 0;
  else
    IioUds.SystemStatus.LtsxEnable      = host->setup.cpu.ltsxEnable;
  DEBUG((EFI_D_ERROR, "Var LtEnable = %d, EVmode = %d\n",IioUds.SystemStatus.LtsxEnable, IioUds.PlatformData.EVMode));
  SavePlatformTxtDataToHob(&IioUds, PlatformInfo);
#endif

  // Update SMBUS Error Recovery in IIOUDS
  Status = GetOptionData (&gEfiSocketProcessorCoreVarGuid, OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, SmbusErrorRecovery), &IioUds.SystemStatus.SmbusErrorRecovery, sizeof(UINT8));
  if (EFI_ERROR(Status)) {
    //Not able to read Setup, initialize to default
    IioUds.SystemStatus.SmbusErrorRecovery = 0;
  }

  // Build HOB data to forward to DXE phase now that all initialization is complete!
  BuildGuidDataHob (&UniversalDataGuid, (VOID *) &IioUds, sizeof(IIO_UDS));

  return;
}

/**

  Build RAS Host Structure and save it into a HOB.

  @param host - pointer to sysHost structure on stack.

**/
VOID
SaveRasHostStructureToHob (
  struct sysHost  *host
  )
{
  struct sysRasHost rasHost;

  MemCopy ((UINT8 *)&(rasHost.var.cpu), (UINT8 *)&host->var.cpu, sizeof (struct cpuVar));
  MemCopy ((UINT8 *)&(rasHost.var.kti), (UINT8 *)&host->var.kti, sizeof (KTI_HOST_OUT));
  MemCopy ((UINT8 *)&(rasHost.var.common), (UINT8 *)&host->var.common, sizeof (struct commonVar));

  MemCopy ((UINT8 *)&(rasHost.setup.cpu), (UINT8 *)&host->setup.cpu, sizeof (struct cpuSetup));
  MemCopy ((UINT8 *)&(rasHost.setup.kti), (UINT8 *)&host->setup.kti, sizeof (KTI_HOST_IN));
  MemCopy ((UINT8 *)&(rasHost.setup.common), (UINT8 *)&host->setup.common, sizeof (struct commonSetup));

  BuildGuidDataHob (
               &gEfiRasHostGuid,
               &rasHost,
               sizeof (struct sysRasHost)
           );
}

/**

  Build EWL Host Structure and save it into a HOB.

  @param host - pointer to sysHost structure on stack.

**/
VOID
SaveEWLHostStructureToHob (
  struct sysHost  *host
  )
{
  EWL_PRIVATE_DATA          ewlPrivateData;
  EFI_GUID                  EWLDataGuid = EWL_ID_GUID;

  DEBUG((EFI_D_ERROR, "SaveEWLHostStructureToHob - HOB\n"));

  MemSetLocal ((UINT8 *) &ewlPrivateData, 0, sizeof(ewlPrivateData));
#if ENHANCED_WARNING_LOG
  CopyMem( &(ewlPrivateData), &(host->var.common.ewlPrivateData), sizeof(ewlPrivateData));
#endif
  BuildGuidDataHob (
               &EWLDataGuid,
               &ewlPrivateData,
               sizeof (ewlPrivateData)
           );
}


/**

  Build FPGA Host Structure and save it into a HOB.

  @param host - pointer to sysHost structure on stack.

**/
VOID
SaveFpgaDataToHob (
  struct sysHost  *host
  )
{
  FPGA_CONFIGURATION FpgaConfigurationPointer;
  UINT8              Soc;
  EFI_STATUS         Status;

  //
  // Get the FpgaConfiguration
  //
  Status = FpgaConfigurationGetValues(&FpgaConfigurationPointer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "FpgaConfigurationGetValues returns error :%r\n", Status));
    return;
  }

  for (Soc = 0; Soc < MAX_SOCKET; Soc ++) {
    if (host->var.kti.OutKtiFpgaPresent[Soc]) {
      FpgaConfigurationPointer.FpgaSktPresent |= (1 << Soc);
    }
    if (host->var.kti.OutKtiFpgaEnable[Soc]) {
      FpgaConfigurationPointer.FpgaSktActive |= (1 << Soc);
    }
  }

  DEBUG((EFI_D_ERROR, "Create or Update FpgaConfigurationPointer HOB about Kti Output\n"));
  DEBUG((EFI_D_ERROR, "FpgaSktPresent: %x; FpgaSktActive: %x; ", FpgaConfigurationPointer.FpgaSktPresent, FpgaConfigurationPointer.FpgaSktActive));
  FpgaConfigurationSetValues (&FpgaConfigurationPointer);
}

/**

  Puts the host structure into HOB variable

  @param host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
PublishHobData(
   struct sysHost             *host
   ) {
  EFI_STATUS                       Status;
  SaveIioUdsDataToHob (host);

  SaveSystemMemoryMapToHob (host);

  SaveBootErrorRecordToHob (host);

  SaveRasHostStructureToHob (host);

  SaveNGNCfgCurRecordsToHob(host);

  SaveNGNCfgOutRecordsToHob(host);

  SaveFpgaDataToHob (host);
#ifdef BDAT_SUPPORT
  if ((host->var.common.bootMode != S3Resume) && (host->setup.common.bdatEn))
    SaveBdatDataToHob(host);
#endif // BDAT_SUPPORT

  SaveEWLHostStructureToHob(host);

  //
  // Save S3 restore Data into Hob
  //
  DEBUG((EFI_D_ERROR, "Save NVRAM restore data into Hob\n"));
  Status = MrcSaveS3RestoreDataToHob (host);
  ASSERT_EFI_ERROR (Status);

  //
  // Signal RC HOBs are ready
  //
  PeiServicesInstallPpi(mSiliconRcHobsReadyPpi);
}


/**
  This function is called after PEI core discover memory and finish migration.

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PublishHobDataCallbackMemoryDiscoveredPpi (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  SYSHOST         *Host;
  UINT64          MsrValue;

  //
  // Re-enable Fast Strings on BSP after CAR is disabled
  //
  MsrValue = AsmReadMsr64(EFI_MSR_IA32_MISC_ENABLE);
  MsrValue |= B_EFI_MSR_IA32_MISC_ENABLE_FSE;
  AsmWriteMsr64(EFI_MSR_IA32_MISC_ENABLE, MsrValue);

  Host = (SYSHOST *)(UINTN) PcdGet64 (PcdSyshostMemoryAddress);
  if (Host == NULL) {
    DEBUG ((EFI_D_ERROR, "PcdSyshostMemoryAddress value is NULL!\n"));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  // APTIOV_SERVER_OVERRIDE_RC_START
  Host->var.common.oemVariable = (UINT32) PeiServices; // Update PeiServices pointer in Host structure
  PublishHobData (Host);
  AmiOemPublishHobDataWrapper(Host); // OEM Hooks Support
  // APTIOV_SERVER_OVERRIDE_RC_END

  return EFI_SUCCESS;
}

/**
  This function is called to save MRC host structure into memory.

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS           The function complete successfully.
  @retval EFI_INVALID_PARAMETER Type is not equal to AllocateAnyPages.
  @retval EFI_NOT_AVAILABLE_YET Called with permanent memory not available.
  @retval EFI_OUT_OF_RESOURCES  The pages could not be allocated.
**/
EFI_STATUS
SaveHostToMemory(
  IN SYSHOST                       *Host
  )
{
  EFI_STATUS                       Status;
  EFI_PHYSICAL_ADDRESS             Buffer;

  Status = PeiServicesAllocatePages (
             EfiBootServicesData,
             EFI_SIZE_TO_PAGES (sizeof(SYSHOST)),
             &Buffer
             );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "[SaveHostToMemory]: Allocate pages size = %x, status = %r, Address = %x\n",EFI_SIZE_TO_PAGES (sizeof(SYSHOST)), Status, (UINT8 *)(UINTN) Buffer));

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[SaveHostToMemory]: Failed to allocate %x pages\n",EFI_SIZE_TO_PAGES (sizeof(SYSHOST))));
    return Status;
  }

  //
  // Copy host structure to memory
  //
  CopyMem ((VOID *)(UINTN) Buffer, (VOID *) Host, sizeof(SYSHOST));

  //
  // Save host memory address to PCD
  //
  Status = PcdSet64S (PcdSyshostMemoryAddress, (UINTN) Buffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;

  return EFI_SUCCESS;
}

/**

Routine Description: Check if a given memory range is persistent,
reserved, or volatile in the SAD. This is bit complicated as ONLY 
part of the given range could be persistent and this routine need 
to handle all cases and return the actual result of this check.
  @param  StartAddr    - Start Address of the Range.
  @param  *Size        - Contains the size of the Range, also
          will carry the size of the result returned.

  @retval Result      - Will contain the result of this check
          details as follows:
          MEM_RANGE_RESERVED when the given range is reserved.

          MEM_RANGE_PERSISTENT when the given range is persistent.
 
          MEM_RANGE_NON_PERSISTENT when the given range is not
          persistent. 

          MEM_RANGE_CTRL_REGION when the given range is a control
          region

          MEM_RANGE_BLK_WINDOW when the given range is a block window

          MEM_RANGE_PART_RESERVED when only part of the Range is 
          in a SAD that is reserved.

          MEM_RANGE_PART_PERSISTENT when only part of the Range is 
          in a SAD that is PM.

          MEM_RANGE_PART_NON_PERSISTENT when only part of the range 
          is in a SAD that is not PM. 

          MEM_RANGE_PART_CTRL_REGION when only part of the given range 
          is a control region

          MEM_RANGE_PART_BLK_WINDOW when only part of the given range 
          is a block window

          In the cases of Result being part, new Size of this found
          will be in Size.
**/
UINT32
CheckIfMemRangePersistentOrReserved(
  struct sysHost      *host,
  UINT64              StartAddr,
  UINT64              *Size
  )
{
  UINT8   socket;
  UINT8   i;
  UINT32  Result;
  UINT64  SADStartAddr;
  UINT64  SADLimit;
  UINT64  BlkSize;
  UINT64  NewSize;

  BlkSize = *Size;
  //
  // Check thru all of the SADs of all sockets and see if this range exists,
  // and if so, check and see if this range is persistent
  DEBUG((EFI_D_ERROR, "RESOURCEDESCRIPTOR: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.mem.socket[socket].SAD[0].Enable == 0) continue;
    // Traverse thru all SAD entries to build the Nfit.
    for (i = 0; i < SAD_RULES; i++) {
      if (host->var.mem.socket[socket].SAD[i].Enable == 0) continue;
      if (host->var.mem.socket[socket].SAD[i].local == 0) continue;
      if (i == 0) {
        SADStartAddr = 0;
      } else {
        SADStartAddr = LShiftU64(host->var.mem.socket[socket].SAD[i - 1].Limit, 26);
      }
      SADLimit = LShiftU64(host->var.mem.socket[socket].SAD[i].Limit, 26);
      if ((StartAddr >= SADStartAddr) && (StartAddr < SADLimit)) {
        if (host->var.mem.socket[socket].SAD[i].Attr == SAD_NXM_ATTR) {
          // Current SAD type is Reserved
          DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches (Reserved) - StartAddr: 0x%lx, Limit: 0x%lx, Type: %d\n", 
            socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
          if ((StartAddr + BlkSize) <= SADLimit) {
            return MEM_RANGE_RESERVED;
          } else {
            // Possibly spawns over multiple SADS
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangePersistentOrReserved(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_RESERVED) { 
              return MEM_RANGE_RESERVED;
            } else {
              // Adjust region size
              *Size -= BlkSize;

              return MEM_RANGE_PART_RESERVED;
            }
          }
        } else if (host->var.mem.socket[socket].SAD[i].type & MEM_TYPE_CTRL) {
          // Current SAD type is CTRL REGION
          DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches (CTRL Region) - StartAddr: 0x%lx, Limit: 0x%lx, Type: %d\n", 
            socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
          if ((StartAddr + BlkSize) <= SADLimit) {
            return MEM_RANGE_CTRL_REGION;
          } else {
            // Possibly spawns over multiple SADS
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangePersistentOrReserved(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_CTRL_REGION) { 
              return MEM_RANGE_CTRL_REGION;
            } else {
              // Adjust region size
              *Size -= BlkSize;

              return MEM_RANGE_PART_CTRL_REGION;
            }
          }
        } else if (host->var.mem.socket[socket].SAD[i].type & MEM_TYPE_BLK_WINDOW) {
          // Current SAD type is Block Window
          DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches (Block Window) - StartAddr: 0x%lx, Limit: 0x%lx, Type: %d\n", 
            socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
          if ((StartAddr + BlkSize) <= SADLimit) {
            return MEM_RANGE_BLK_WINDOW;
          } else {
            // Possibly spawns over multiple SADS
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangePersistentOrReserved(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_BLK_WINDOW) { 
              return MEM_RANGE_BLK_WINDOW;
            } else {
              // Adjust region size
              *Size -= BlkSize;

              return MEM_RANGE_PART_BLK_WINDOW;
            }
          }
        } else if ((host->var.mem.socket[socket].SAD[i].type & MEM_TYPE_PMEM) ||
            (host->var.mem.socket[socket].SAD[i].type & MEM_TYPE_PMEM_CACHE)) {
          // Current SAD type is Persistent
          DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches (PMEM) - StartAddr: 0x%lx, Limit: 0x%lx, Type: %d\n", 
            socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
          if ((StartAddr + BlkSize) <= SADLimit) {
            return MEM_RANGE_PERSISTENT;
          } else {
            // Possibly spawns over multiple SADS
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangePersistentOrReserved(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_PERSISTENT) {
              return MEM_RANGE_PERSISTENT;
            } else {
              // Adjust region size
              *Size -= BlkSize;

              return MEM_RANGE_PART_PERSISTENT;
            }
          }
        } else {
          // Current SAD Type is non-persistent
          DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches (Non-PM) - StartAddr: 0x%lx, Limit: 0x%lx, Type: %d\n", 
            socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
          if ((StartAddr + BlkSize) <= SADLimit) {
            return MEM_RANGE_NON_PERSISTENT;
          } else {
            // Possibly spawns over mulltiple SADs
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangePersistentOrReserved(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_NON_PERSISTENT) {
              return MEM_RANGE_NON_PERSISTENT;
            } else {
              // Adjust region size
              *Size -= BlkSize;

              return  MEM_RANGE_PART_NON_PERSISTENT;
            }
          }
        }
      } // if ((StartAddr >= SADStartAddr) && (StartAddr < SADLimit)) {
    } // for (i = 0; i < SAD_RULES; i++)
  } // for (socket = 0; socket < MAX_SOCKET; socket++) {
  //
  // If this gets here, then the range is not found in the SAD.
  return MEM_RANGE_INVALID;
}


/**

Routine Description: Check if a given memory range is persistent
or not in the SAD. This is bit complicated as ONLY part of the
given range could be persistent and this routine need to handle
all cases and return the actual result of this check.
  @param  StartAddr    - Start Address of the Range.
  @param  *Size        - Contains the size of the Range, also
          will carry the size of the result returned.

  @retval Result      - Will contain the result of this check
          details as follows:
          MEM_RANGE_PERSISTENT when the given range is persistent.
          MEM_RANGE_NON_PERSISTENT when the given range is not
          persistent. MEM_RANGE_PART_PERSISTENT when only part
          of the Range is in a SAD Range but is PM. Finally,
          MEM_RANGE_PART_NON_PERSISTENT when only part of the
          Range is in a SAD but is not PM. In the cases of
          Result being part, new Size of this found
          will be in Size.
**/
UINT32
CheckIfMemRangeMirrored (
  struct sysHost      *host,
  UINT64              StartAddr,
  UINT64              *Size
  )
{
  UINT8   socket;
  UINT8   i;
  UINT32  Result;
  UINT64  SADStartAddr;
  UINT64  SADLimit;
  UINT64  BlkSize;
  UINT64  NewSize;

  BlkSize = *Size;
  //
  // Check thru all of the SADs of all sockets and see if this range exists,
  // and if so, check and see if this range is persistent
  DEBUG((EFI_D_ERROR, "RESOURCEDESCRIPTOR: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.mem.socket[socket].SAD[0].Enable == 0) continue;
    // Traverse thru all SAD entries to build the Nfit.
    for (i = 0; i < SAD_RULES; i++) {
      if (host->var.mem.socket[socket].SAD[i].Enable == 0) continue;
      if (host->var.mem.socket[socket].SAD[i].local == 0) continue;
      if (i == 0) {
        SADStartAddr = 0;
      } else {
        SADStartAddr = LShiftU64(host->var.mem.socket[socket].SAD[i - 1].Limit, 26);
      }
      SADLimit = LShiftU64(host->var.mem.socket[socket].SAD[i].Limit, 26);
      if ((StartAddr >= SADStartAddr) && (StartAddr < SADLimit)) {
        // If the current SAD type is a mirrored SAD
        DEBUG((EFI_D_ERROR, "      - SAD[%x][%x] matches - StartAddr: 0x%16lx, Limit: 0x%16lx, Type: %d\n", socket, i, SADStartAddr, SADLimit, host->var.mem.socket[socket].SAD[i].type));
        if (host->var.mem.socket[socket].SAD[i].mirrored == 1) {
          if ((StartAddr + BlkSize) <= SADLimit) { //entire region is mirrored
            DEBUG((EFI_D_ERROR, "      - Mirror Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
            return MEM_RANGE_MIRRORED;
          } else {
            // Possibly spawns over multiple SADS
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangeMirrored(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_MIRRORED) {
              DEBUG((EFI_D_ERROR, "      - Full MIRROR in Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
              return MEM_RANGE_MIRRORED;
            } else {
              DEBUG((EFI_D_ERROR, "      - Partial MIRROR in Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));

              // Adjust region size
              *Size -= BlkSize;

              return MEM_RANGE_PART_MIRRORED;
            }
          }
        } else {
          // Current SAD Type is non-mirrored
          if ((StartAddr + BlkSize) <= SADLimit) {
            DEBUG((EFI_D_ERROR, "      - Full Non-MIRROR in the Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
            return MEM_RANGE_NON_MIRRORED;
          } else {
            // Possibly spawns over mulltiple SADs
            // Continue scan recursively
            BlkSize -= SADLimit - StartAddr;
            NewSize = BlkSize;
            Result = CheckIfMemRangeMirrored(host, SADLimit, &NewSize);
            if (Result == MEM_RANGE_NON_MIRRORED) {
              DEBUG((EFI_D_ERROR, "      - Full Non-MIRROR in the Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));
              return MEM_RANGE_NON_MIRRORED;
            } else {
              DEBUG((EFI_D_ERROR, "      - Partial Non-MIRROR in the Region: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, *Size));

              // Adjust region size
              *Size -= BlkSize;

              return  MEM_RANGE_PART_NON_MIRRORED;
            }
          }
        }
      } // if ((StartAddr >= SADStartAddr) && (StartAddr < SADLimit)) {
    } // for (i = 0; i < SAD_RULES; i++)
  } // for (socket = 0; socket < MAX_SOCKET; socket++) {
  //
  // If this gets here, then the range is not found in the SAD.
  return MEM_RANGE_INVALID;
}

/**

  This function Builds Memory Descriptor Hob for a
  given Range of Memory and its size.
  @param StartAddr     - Starting Address of the Memory Range
  @param MemSize       - Size of the Memory that needs to be
                         added in the Hob
  @param PMEM          - Boolean to tell if Persistent Mem or
                         not
  @retval None         - void

**/
VOID
AddADescriptor(
   UINT64           StartAddr,
   UINT64           BlockSize,
   UINT8            AttrType   //0 = Not PMEM or Mirror, 1 = PMEM, 2 = Mirror
   )
{
  switch (AttrType) {
    default:  
    case INDEP_VOL_REGION_DESC:
      BuildResourceDescriptorHob(
        EFI_RESOURCE_SYSTEM_MEMORY,
        (
         EFI_RESOURCE_ATTRIBUTE_PRESENT |
         EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
         EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
         ),
        StartAddr,
        BlockSize
        );
      break;
    //Persistent memory
    case PMEM_REGION_DESC:
      BuildResourceDescriptorHob(
        EFI_RESOURCE_SYSTEM_MEMORY,
        (
         EFI_RESOURCE_ATTRIBUTE_PRESENT |
         EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
         EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_PERSISTENT |
         EFI_RESOURCE_ATTRIBUTE_PERSISTABLE
         ),
        StartAddr,
        BlockSize
        );
      break;
    //Mirrored memory
    case MIRROR_REGION_DESC:
      BuildResourceDescriptorHob(
        EFI_RESOURCE_SYSTEM_MEMORY,
        (
         EFI_RESOURCE_ATTRIBUTE_PRESENT |
         EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
         EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_MORE_RELIABLE
         ),
        StartAddr,
        BlockSize
        );
      break;
    case RESERVED_REGION_DESC:
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_RESERVED,
        0,
        StartAddr,
        BlockSize
        );
      break;
    case UC_MMIO_REGION_DESC:
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_MAPPED_IO,
        (
         EFI_RESOURCE_ATTRIBUTE_PRESENT |
         EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
         EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE
         ),
        StartAddr,
        BlockSize
        );
      BuildMemoryAllocationHob (
        StartAddr,
        BlockSize,
        EfiMemoryMappedIO
        );
      break;
    case MMIO_REGION_DESC:
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_MAPPED_IO,
        (
         EFI_RESOURCE_ATTRIBUTE_PRESENT |
         EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
         EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
         EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
         ),
        StartAddr,
        BlockSize
        );
      BuildMemoryAllocationHob (
        StartAddr,
        BlockSize,
        EfiMemoryMappedIO
        );
      break;
  }
}

/**

  This function Build Persistent Memory Descriptor Hobs for a
  given Range of Memory and its size. Also this function handles
  the cases for both Persistent Memory Types, Reserved Memory, 
  and Non-Persistent Memory types

  @param host          - pointer to sysHost structure on stack
  @param StartAddr     - Starting Address of the Memory Range
  @param MemSize       - Size of the Memory that needs to be
                         added in the Hob
  @param Tested        - Boolean to tell if this tested or
                         untested memory.
  @retval               None
  @retval

**/
VOID
BldPMemResourceDescHobs(
  struct sysHost   *host,
  UINT64           StartAddr,
  UINT64           MemSize
  )
{
  UINT64            BlockSize;
  UINT32            Result;

  BlockSize = MemSize;
  Result = CheckIfMemRangePersistentOrReserved(host, StartAddr, &BlockSize);

  // Create descriptor for initial range
  if ((Result == MEM_RANGE_RESERVED) || (Result == MEM_RANGE_PART_RESERVED)) {
    DEBUG((EFI_D_ERROR, "      - Creating Reserved Resource Descriptor: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, BlockSize));
    AddADescriptor(StartAddr, BlockSize, RESERVED_REGION_DESC);
  } else if ((Result == MEM_RANGE_PERSISTENT) || (Result == MEM_RANGE_PART_PERSISTENT)) {
    DEBUG((EFI_D_ERROR, "      - Creating PMEM Resource Descriptor: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, BlockSize));
    AddADescriptor(StartAddr, BlockSize, PMEM_REGION_DESC);
  } else if ((Result == MEM_RANGE_CTRL_REGION) || (Result == MEM_RANGE_PART_CTRL_REGION)) {
    DEBUG((EFI_D_ERROR, "      - Creating UC MMIO Resource Descriptor: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, BlockSize));
    AddADescriptor(StartAddr, BlockSize, UC_MMIO_REGION_DESC);
  } else if ((Result == MEM_RANGE_BLK_WINDOW) || (Result == MEM_RANGE_PART_BLK_WINDOW)) {
    DEBUG((EFI_D_ERROR, "      - Creating MMIO Resource Descriptor: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, BlockSize));
    AddADescriptor(StartAddr, BlockSize, MMIO_REGION_DESC);
  } else {
    DEBUG((EFI_D_ERROR, "      - Creating System Memory Resource Descriptor: StartAddr: 0x%16lx  Size: 0x%16lx\n", StartAddr, BlockSize));
    AddADescriptor(StartAddr, BlockSize, INDEP_VOL_REGION_DESC);
  }
  
  if (BlockSize < MemSize) {
    // Handle the remainder of the memory range
    BldPMemResourceDescHobs(host, StartAddr + BlockSize, MemSize - BlockSize);
  }
}

/**

  This function Build Persistent Memory Descriptor Hobs for a
  given Range of Memory and its size. Also this function handles
  the cases for both Persistent Memory Types and Non-Persistent
  Memory types

  @param host          - pointer to sysHost structure on stack
  @param StartAddr     - Starting Address of the Memory Range
  @param MemSize       - Size of the Memory that needs to be
                         added in the Hob
  @param Tested        - Boolean to tell if this tested or
                         untested memory.
  @retval               None
  @retval

**/
VOID
BldMirrResourceDescHobs(
  struct sysHost   *host,
  UINT64           StartAddr,
  UINT64           MemSize
  )
{
  UINT64            BlockSize;
  UINT64            RemSize;
  UINT64            XlatedSize;
  UINT32            Result;

  BlockSize = MemSize;
  Result = CheckIfMemRangeMirrored(host, StartAddr, &BlockSize);
  if (Result == MEM_RANGE_MIRRORED) {
    AddADescriptor(StartAddr, BlockSize, MIRROR_REGION_DESC);
  } else if (Result == MEM_RANGE_NON_MIRRORED) {
    AddADescriptor(StartAddr, BlockSize, INDEP_VOL_REGION_DESC);
  } else if (Result == MEM_RANGE_PART_MIRRORED) {
    // Add an Resource Descriptor for the persistent portion
    RemSize = MemSize - BlockSize;
    AddADescriptor(StartAddr, BlockSize, MIRROR_REGION_DESC);
    XlatedSize = BlockSize;
    while (RemSize) {
      BlockSize = RemSize;
      Result = CheckIfMemRangeMirrored(host, (StartAddr + XlatedSize), &BlockSize);
      if ((Result == MEM_RANGE_NON_MIRRORED) || (Result == MEM_RANGE_PART_NON_MIRRORED)) {
        AddADescriptor((StartAddr + XlatedSize), BlockSize, INDEP_VOL_REGION_DESC);
        XlatedSize += BlockSize;
        RemSize = MemSize - XlatedSize;
      } else if ((Result == MEM_RANGE_MIRRORED) || (Result == MEM_RANGE_PART_MIRRORED)) {
        AddADescriptor((StartAddr + XlatedSize), BlockSize, MIRROR_REGION_DESC);
        XlatedSize += BlockSize;
        RemSize = MemSize - XlatedSize;
      }
    }
  } else if (Result == MEM_RANGE_PART_NON_MIRRORED) {
    // Add an Resource Descriptor for the non-persistent portion
    AddADescriptor(StartAddr, BlockSize, INDEP_VOL_REGION_DESC);
    RemSize = MemSize - BlockSize;
    XlatedSize = BlockSize;
    while (RemSize) {
      BlockSize = RemSize;
      Result = CheckIfMemRangeMirrored(host, (StartAddr + XlatedSize), &BlockSize);
      if ((Result == MEM_RANGE_MIRRORED) || (Result == MEM_RANGE_PART_MIRRORED)) {
        AddADescriptor((StartAddr + XlatedSize), BlockSize, MIRROR_REGION_DESC);
        XlatedSize += BlockSize;
        RemSize = MemSize - XlatedSize;
      } else if ((Result == MEM_RANGE_NON_MIRRORED) || (Result == MEM_RANGE_PART_NON_MIRRORED)) {
        AddADescriptor((StartAddr + XlatedSize), BlockSize, INDEP_VOL_REGION_DESC);
        XlatedSize += BlockSize;
        RemSize = MemSize - XlatedSize;
      }
    }
  }
}

VOID
PrintSADInfo(
   struct sysHost      *host
   ) {
  UINT8                socket;
  UINT8                i;
  UINT64               SADStartAddr;
  UINT64               SADLimit;

  DEBUG((EFI_D_ERROR, "-------------SAD Info-------------\n"));
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->var.mem.socket[socket].SAD[0].Enable == 0) continue;
    // Traverse thru all SAD entries to build the Nfit.
    for (i = 0; i < SAD_RULES; i++) {
      if (host->var.mem.socket[socket].SAD[i].Enable == 0) continue;
      if (host->var.mem.socket[socket].SAD[i].local == 0) continue;
      if (i == 0) {
        SADStartAddr = 0;
      } else {
        SADStartAddr = LShiftU64(host->var.mem.socket[socket].SAD[i - 1].Limit, 26);
      }
      SADLimit = LShiftU64(host->var.mem.socket[socket].SAD[i].Limit, 26);
      DEBUG((EFI_D_ERROR, "E820 - CR: SAD - Type: %d\n", host->var.mem.socket[socket].SAD[i].type));
      DEBUG((EFI_D_ERROR, "E820 - CR: SAD - StartAddr: 0x%16lx,  Limit:0x%16lx\n", SADStartAddr, SADLimit));
    }
  }
}

//
// Function Implementations
//
/**

  This function installs memory.

  @param host - pointer to sysHost structure on stack

  @retval 0 - success
  @retval Other -  failure

**/
EFI_STATUS
InstallEfiMemory (
  struct sysHost             *host
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Index;
  UINT8                                 NumRanges;
  UINT8                                 SmramIndex;
  UINT8                                 SmramRanges;
  UINT64                                PeiMemoryLength;
  UINT64                                BlockSize;
  UINTN                                 BufferSize;
  UINTN                                 PeiMemoryIndex;
  UINTN                                 RequiredMemSize;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_PHYSICAL_ADDRESS                  PeiMemoryBaseAddress;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemStart;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemSize;
  PEI_MEMORY_TEST_OP                    MemoryTestOp;
  PEI_BASE_MEMORY_TEST_PPI              *BaseMemoryTestPpi;
  PEI_PLATFORM_MEMORY_SIZE_PPI          *MemSize;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE MemoryMap[MAX_RANGES];
  EFI_PHYSICAL_ADDRESS                  BadMemoryAddress;
  EFI_BOOT_MODE                         BootMode;
  EFI_PEI_SERVICES                      **gPeiServices;
  EFI_SMRAM_DESCRIPTOR                  DescriptorAcpiVariable;
#ifdef NVMEM_FEATURE_EN
  UINT32                                NormalMemSize64 = 0;
  UINT32                                NVMemSize64 = 0;
  EFI_PHYSICAL_ADDRESS                  NormalMemSize=0;
  EFI_PHYSICAL_ADDRESS                  NVMemSize = 0;
  EFI_PHYSICAL_ADDRESS                  TotIntMemSize = 0;
  EFI_PHYSICAL_ADDRESS                  TotalMemSizeAbove4G = 0;
  UINT8                                 sckt = 0;
  //UINT8                                 sckt2 = 0;
#endif
// APTIOV_SERVER_OVERRIDE_RC_START : To support Capsule update in Purley
#if Capsule_SUPPORT
  PEI_CAPSULE_PPI                       *Capsule;
  VOID                                  *CapsuleBuffer;
  UINTN                                 CapsuleBufferLength;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To support Capsule update in Purley

  gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  DEBUG ((EFI_D_ERROR, "InstallEfiMemory()\n"));

  if(host->var.mem.subBootMode == AdrResume) {
    // Change Boot Mode to skip the memory init.
    BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
  } else {
    //
    // Determine boot mode
    //
    Status = PeiServicesGetBootMode (&BootMode);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Get the Memory Map
  //
  NumRanges = MAX_RANGES;
  MemSetLocal ((UINT8 *) MemoryMap, 0, (sizeof(PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges));
  Status = GetMemoryMap ((PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *) MemoryMap, &NumRanges, host);
  ASSERT_EFI_ERROR (Status);

  //
  // Find the highest memory range in processor native address space to give to
  // PEI. Then take the top.
  // If this algorithm changes, then we need to fix the capsule memory
  // selection algorithm below.
  //
  PeiMemoryBaseAddress = 0;

  //
  // Query the platform for the minimum memory size
  //
  Status = PeiServicesLocatePpi (&gPeiPlatformMemorySizePpiGuid, 0, NULL, &MemSize);
  ASSERT_EFI_ERROR (Status);

  Status = MemSize->GetPlatformMemorySize (gPeiServices, MemSize, &PeiMemoryLength);
  ASSERT_EFI_ERROR (Status);

  //
  // Get required memory size for ACPI use. This helps to put ACPI memory on the top
  //
  RequiredMemSize = 0;
  // APTIOV_SERVER_OVERRIDE_RC_START : WA.to disallow platform code to subtract the BS memory from platform memory.
  //RetrieveRequiredMemorySize (host, &RequiredMemSize);
  // APTIOV_SERVER_OVERRIDE_RC_END : WA.to disallow platform code to subtract the BS memory from platform memory.
  DEBUG ((EFI_D_ERROR, " RequiredMemSize for ACPI = 0x%X bytes\n", RequiredMemSize));

  PeiMemoryIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    DEBUG ((EFI_D_ERROR, "Found 0x%016lX bytes at ", MemoryMap[Index].RangeLength));
    DEBUG ((EFI_D_ERROR, "0x%016lX\n", MemoryMap[Index].PhysicalAddress));
    if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
        (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS) &&
        (MemoryMap[Index].PhysicalAddress >= PeiMemoryBaseAddress) &&
        (MemoryMap[Index].RangeLength >= PeiMemoryLength)
        ) {
      PeiMemoryBaseAddress  = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
      PeiMemoryIndex        = Index;
    }
  }
  //
  // Test only the PEI memory if necessary. Some platforms do not require the
  // Base Memory PEIM to be present
  //
  Status = PeiServicesLocatePpi (&gPeiBaseMemoryTestPpiGuid, 0, NULL, &BaseMemoryTestPpi);

  switch (BootMode) {

  case BOOT_WITH_FULL_CONFIGURATION:
    MemoryTestOp = Quick;
    break;

  case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    MemoryTestOp = Extensive;
    break;

  default:
    MemoryTestOp = Ignore;
    break;
  }


  Status = BaseMemoryTestPpi->BaseMemoryTest (
                                gPeiServices,
                                BaseMemoryTestPpi,
                                PeiMemoryBaseAddress,
                                PeiMemoryLength,
                                MemoryTestOp,
                                &BadMemoryAddress
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Memory test failure at 0x%016lX.\n", BadMemoryAddress));
  }

  ASSERT_EFI_ERROR (Status);

// APTIOV_SERVER_OVERRIDE_RC_START : To support Capsule update in Purley
#if Capsule_SUPPORT
    Capsule             = NULL;
    CapsuleBuffer       = NULL;
    CapsuleBufferLength = 0;

    if (BootMode == BOOT_ON_FLASH_UPDATE) {
      Status = PeiServicesLocatePpi (
                      &gEfiPeiCapsulePpiGuid,
                      0,
                      NULL,
                      (VOID **) &Capsule
                      );
      ASSERT_EFI_ERROR (Status);

      if (Status == EFI_SUCCESS) {
        //
        // Find the largest memory range excluding that given to PEI.
        //
        for (Index = 0; Index < NumRanges; Index++) {
          if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
            (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS)) {
            if (Index != PeiMemoryIndex) {
              if (MemoryMap[Index].RangeLength > CapsuleBufferLength) {
                CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
                CapsuleBufferLength = (UINTN) MemoryMap[Index].RangeLength;
              }
            } else {
              if ((MemoryMap[Index].RangeLength - PeiMemoryLength) >= CapsuleBufferLength) {
                CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
                CapsuleBufferLength = (UINTN) (MemoryMap[Index].RangeLength - PeiMemoryLength);
              }
            }
          }
        }
        //
        // Call the Capsule PPI Coalesce function to coalesce the capsule data.
        //
        Status = Capsule->Coalesce ((EFI_PEI_SERVICES **)gPeiServices, &CapsuleBuffer, &CapsuleBufferLength);
      }
      //
      // If it failed, then NULL out our capsule PPI pointer so that the capsule
      // HOB does not get created below.
      //
      if (Status != EFI_SUCCESS) {
        Capsule = NULL;
      }
    }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To support Capsule update in Purley
    
  //
  // Carve out the top memory reserved for ACPI
  //
  Status = PeiServicesInstallPeiMemory (PeiMemoryBaseAddress, PeiMemoryLength - RequiredMemSize);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "Building RESOURCE_SYSTEM_MEMORY Hob:\n"));
  DEBUG ((
    EFI_D_ERROR,
    " PeiMemoryBaseAddress = 0x%lX, PeiMemoryLength = 0x%lX\n",
    PeiMemoryBaseAddress,
    PeiMemoryLength)
    );
  BlockSize = PeiMemoryLength;
  if (PeiMemoryBaseAddress != 0) {
    if (CheckIfMemRangeMirrored(host, MemoryMap[PeiMemoryIndex].PhysicalAddress, &BlockSize) == MEM_RANGE_MIRRORED) {
      BuildResourceDescriptorHob (
              EFI_RESOURCE_SYSTEM_MEMORY,
              (
              EFI_RESOURCE_ATTRIBUTE_PRESENT |
              EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
              EFI_RESOURCE_ATTRIBUTE_TESTED |
              EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE|
              EFI_RESOURCE_ATTRIBUTE_MORE_RELIABLE
              ),
              PeiMemoryBaseAddress,
              PeiMemoryLength
              );
    } else {
      BuildResourceDescriptorHob (
                EFI_RESOURCE_SYSTEM_MEMORY,
                (
                EFI_RESOURCE_ATTRIBUTE_PRESENT |
                EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                EFI_RESOURCE_ATTRIBUTE_TESTED |
                EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                ),
                PeiMemoryBaseAddress,
                PeiMemoryLength
                );
    }
  }
  //
  // Install physical memory descriptor hobs for each memory range.
  //
  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if (MemoryMap[Index].Type == DualChannelDdrMainMemory) {

      if (Index == PeiMemoryIndex) {
        //
        // This is a partially tested Main Memory range, give it to EFI
        //
        BlockSize = MemoryMap[Index].RangeLength - PeiMemoryLength;
        if ( CheckIfMemRangeMirrored(host, MemoryMap[Index].PhysicalAddress, &BlockSize) == MEM_RANGE_MIRRORED) {
          BuildResourceDescriptorHob (
                  EFI_RESOURCE_SYSTEM_MEMORY,
                  (
                  EFI_RESOURCE_ATTRIBUTE_PRESENT |
                  EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                  EFI_RESOURCE_ATTRIBUTE_TESTED |
                  EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_MORE_RELIABLE
                  ),
                  MemoryMap[Index].PhysicalAddress,
                  MemoryMap[Index].RangeLength - PeiMemoryLength
                  );
        } else {
          BuildResourceDescriptorHob (
                  EFI_RESOURCE_SYSTEM_MEMORY,
                  (
                  EFI_RESOURCE_ATTRIBUTE_PRESENT |
                  EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                  EFI_RESOURCE_ATTRIBUTE_TESTED |
                  EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                  ),
                  MemoryMap[Index].PhysicalAddress,
                  MemoryMap[Index].RangeLength - PeiMemoryLength
                  );
        }
        DEBUG ((EFI_D_ERROR, "PEI- Partially tested Main Memory Base:0x%016lX Length:0x%016lX End::0x%016lX \n",MemoryMap[Index].PhysicalAddress,MemoryMap[Index].RangeLength - PeiMemoryLength, MemoryMap[Index].PhysicalAddress+MemoryMap[Index].RangeLength - PeiMemoryLength));

        //
        // Test this memory range
        //
        Status = BaseMemoryTestPpi->BaseMemoryTest (
                                      gPeiServices,
                                      BaseMemoryTestPpi,
                                      MemoryMap[Index].PhysicalAddress,
                                      MemoryMap[Index].RangeLength - PeiMemoryLength,
                                      MemoryTestOp,
                                      &BadMemoryAddress
                                      );
        ASSERT_EFI_ERROR (Status);
      } else {
		// APTIOV_SERVER_OVERRIDE_RC_START : memory range 0-9FFFF is not available in the GetMemoryMap() service
	     EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute;
		 if((MemoryMap[Index].PhysicalAddress==0)&&(MemoryMap[Index].RangeLength==0xa0000)) {
		     ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_TESTED;
		 } else {
		     ResourceAttribute = 0x00;
		 }
        // APTIOV_SERVER_OVERRIDE_RC_END : memory range 0-9FFFF is not available in the GetMemoryMap() service  
        //
        // This is an untested Main Memory range, give it to EFI
        //
        BlockSize = MemoryMap[Index].RangeLength;
        if ( CheckIfMemRangeMirrored(host, MemoryMap[Index].PhysicalAddress, &BlockSize) == MEM_RANGE_MIRRORED) {
          BuildResourceDescriptorHob (
                    EFI_RESOURCE_SYSTEM_MEMORY,
                    (
					// APTIOV_SERVER_OVERRIDE_RC_START
                    ResourceAttribute |
					// APTIOV_SERVER_OVERRIDE_RC_END
                    EFI_RESOURCE_ATTRIBUTE_PRESENT |
                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
                    EFI_RESOURCE_ATTRIBUTE_MORE_RELIABLE
                    ),
                    MemoryMap[Index].PhysicalAddress, // MemoryBegin
                    MemoryMap[Index].RangeLength      // MemoryLength
                    );
        } else {
        BuildResourceDescriptorHob (
                  EFI_RESOURCE_SYSTEM_MEMORY,
                  (
					// APTIOV_SERVER_OVERRIDE_RC_START
                    ResourceAttribute |
					// APTIOV_SERVER_OVERRIDE_RC_END
                  EFI_RESOURCE_ATTRIBUTE_PRESENT |
                  EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                  EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                  EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                  ),
                  MemoryMap[Index].PhysicalAddress, // MemoryBegin
                  MemoryMap[Index].RangeLength      // MemoryLength
                  );
        }
        DEBUG ((EFI_D_ERROR, "Untested Main Memory range Base:0x%016lX Length:0x%016lX End:0x%016lX  \n",MemoryMap[Index].PhysicalAddress,MemoryMap[Index].RangeLength, MemoryMap[Index].PhysicalAddress+MemoryMap[Index].RangeLength));

        //
        // Test this memory range
        //
        Status = BaseMemoryTestPpi->BaseMemoryTest (
                                      gPeiServices,
                                      BaseMemoryTestPpi,
                                      MemoryMap[Index].PhysicalAddress,
                                      MemoryMap[Index].RangeLength,
                                      MemoryTestOp,
                                      &BadMemoryAddress
                                      );
        ASSERT_EFI_ERROR (Status);
      }
    } else {
      if ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
          (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
          ) {
        SmramRanges++;
      }
      //
      // Make sure non-system memory is marked as reserved
      //
      BuildResourceDescriptorHob (
                EFI_RESOURCE_MEMORY_RESERVED,     // MemoryType,
                0,                                // MemoryAttribute
                MemoryMap[Index].PhysicalAddress, // MemoryBegin
                MemoryMap[Index].RangeLength      // MemoryLength
                );
    }
  }

  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  Hob.Raw = BuildGuidHob (
              &gEfiSmmPeiSmramMemoryReserveGuid,
              BufferSize
              );
  ASSERT (Hob.Raw);

  SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;

  for (Index = 0; Index < NumRanges; Index++) {

    if ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
        ) {
      //
      // This is an SMRAM range, create an SMRAM descriptor
      //
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalStart = MemoryMap[Index].PhysicalAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].CpuStart      = MemoryMap[Index].CpuAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalSize  = MemoryMap[Index].RangeLength;

      if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
      } else {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED;
      }
      SmramIndex++;
    }
  }

  //
  // Build a HOB with the location of the reserved memory range.
  //
  CopyMem(&DescriptorAcpiVariable, &SmramHobDescriptorBlock->Descriptor[SmramRanges-1], sizeof(EFI_SMRAM_DESCRIPTOR));
  DescriptorAcpiVariable.CpuStart += RESERVED_CPU_S3_SAVE_OFFSET;
  BuildGuidDataHob (
    &gEfiAcpiVariableGuid,
    &DescriptorAcpiVariable,
    sizeof (EFI_SMRAM_DESCRIPTOR)
    );

  //
  // Get the current "Top of Upper Usable Memory" address from TOUUD.
  // If TOUUD > 4G, it means memory re-mapped.
  //
  TopUseableMemSize = ((EFI_PHYSICAL_ADDRESS)host->var.mem.highMemSize) * CONVERT_64MB_TO_BYTE;
  TopUseableMemStart = ((EFI_PHYSICAL_ADDRESS)host->var.mem.highMemBase) * CONVERT_64MB_TO_BYTE;

  DEBUG((EFI_D_ERROR, "TOHM:0x%016lX\n ", TopUseableMemSize));

#ifdef NVMEM_FEATURE_EN
  // Calculate the Non volatile memory behind each socket
  if (host->var.mem.NVmemSize) {
    DEBUG((EFI_D_ERROR, "Calculating ADR Memory regions\n")); 
    TotalMemSizeAbove4G = TopUseableMemSize;
    for(sckt = 0; sckt < MAX_SOCKET; sckt++ ) {
      if(!(host->setup.mem.options & NUMA_AWARE)) {
        NormalMemSize64 = host->var.mem.TotalInterleavedMemSize;
        NVMemSize64 = host->var.mem.TotalInterleavedNVMemSize;
        DEBUG((EFI_D_ERROR, "NormalMemSize64(host->var.mem.socket[%d].TotalInterleavedMemSize) = 0x%x\n", sckt, NormalMemSize64));
        DEBUG((EFI_D_ERROR, "NVMemSize64(host->var.mem.socket[%d].TotalInterleavedNVMemSize) = 0x%x\n", sckt, NVMemSize64));
      } else if (host->nvram.mem.socket[sckt].enabled == 0 || host->var.mem.socket[sckt].memSize == 0) continue;
      else
      {
        NormalMemSize64 = host->var.mem.socket[sckt].TotalInterleavedMemSize;
        NVMemSize64 = host->var.mem.socket[sckt].TotalInterleavedNVMemSize;
        DEBUG((EFI_D_ERROR, "NormalMemSize64(host->var.mem.socket[%d].TotalInterleavedMemSize) = 0x%x\n", sckt, NormalMemSize64));
        DEBUG((EFI_D_ERROR, "NVMemSize64(host->var.mem.socket[%d].TotalInterleavedNVMemSize) = 0x%x\n", sckt, NVMemSize64));
      }

      NormalMemSize = ((EFI_PHYSICAL_ADDRESS)NormalMemSize64) * CONVERT_64MB_TO_BYTE;
      NVMemSize = ((EFI_PHYSICAL_ADDRESS)NVMemSize64) * CONVERT_64MB_TO_BYTE;
      TotIntMemSize += (NormalMemSize + NVMemSize);

      DEBUG((EFI_D_ERROR, "Before TOHM Adjust: NormalMemSize = 0x%016lX, NVMemSize = 0x%016lX, TotIntMemSize = 0x%016lx\n", NormalMemSize, NVMemSize, TotIntMemSize));

      // Cap the maximum memory to TopUseableMem      
      if (TotIntMemSize > TotalMemSizeAbove4G) {
        if (NVMemSize) // NVMem present in the system(or this socket in case of NUMA)
          NVMemSize -= (TotIntMemSize - TotalMemSizeAbove4G);
        else //  This socket does not have NVMEM so reduce normal mem.
          NormalMemSize -= (TotIntMemSize - TotalMemSizeAbove4G);
      }
      DEBUG((EFI_D_ERROR, "After TOHM Adjust: NormalMemSize = 0x%016lX, NVMemSize = 0x%016lX\n", NormalMemSize, NVMemSize));


      while (NormalMemSize >= MEM_EQU_4GB) {
        BuildResourceDescriptorHob(
          EFI_RESOURCE_SYSTEM_MEMORY,
          (
          EFI_RESOURCE_ATTRIBUTE_PRESENT |
          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
          ),
          TopUseableMemStart,
          MEM_EQU_4GB
          );
        DEBUG((EFI_D_ERROR, "Just added 4GB of System memory at =0x%lx\n", TopUseableMemStart));
        NormalMemSize = NormalMemSize - MEM_EQU_4GB;
        TopUseableMemStart = TopUseableMemStart + MEM_EQU_4GB;
      }
      //
      // Create hob for remain memory which above 4G memory.
      //
      if (NormalMemSize) {
        BuildResourceDescriptorHob(
          EFI_RESOURCE_SYSTEM_MEMORY,
          (
          EFI_RESOURCE_ATTRIBUTE_PRESENT |
          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
          ),
          TopUseableMemStart,
          NormalMemSize
          );
        DEBUG((EFI_D_ERROR, "Just added 0x%lx of System memory at =0x%lx\n", NormalMemSize, TopUseableMemStart));
        TopUseableMemStart = TopUseableMemStart + NormalMemSize;
      }

      while (NVMemSize >= MEM_EQU_4GB) {
        BuildResourceDescriptorHob(
          EFI_RESOURCE_SYSTEM_MEMORY,
          EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE,
          TopUseableMemStart,
          MEM_EQU_4GB
          );
        DEBUG((EFI_D_ERROR, "Just added 4G of Persistent (ADR) memory at =0x%lx\n",TopUseableMemStart));
        NVMemSize = NVMemSize - MEM_EQU_4GB;
        TopUseableMemStart = TopUseableMemStart + MEM_EQU_4GB;
      }
      //
      // Create hob for remaining NV Memory
      //
      if (NVMemSize) {
        BuildResourceDescriptorHob(
          EFI_RESOURCE_SYSTEM_MEMORY,
          EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE,
          TopUseableMemStart,
          NVMemSize
          );
        DEBUG((EFI_D_ERROR, "Just added 0x%lx of Persistent (ADR) memory at =0x%lx\n", NVMemSize, TopUseableMemStart));
        TopUseableMemStart = TopUseableMemStart + NVMemSize;
      }
      if(!(host->setup.mem.options & NUMA_AWARE)) break;
    }
  }
  else
  {
#endif
    //Add descriptor hob for memory above 4GB
    if (TopUseableMemSize > 0) {
      //
      // if memory hob length is above 4G length, split it and add multple of descriptor hobs each 4gb long.
      //
      while (TopUseableMemSize > MEM_EQU_4GB) {
        if ( (host->nvram.mem.aepDimmPresent) || (host->nvram.mem.RASmode & CH_ALL_MIRROR) ) {
          if (host->nvram.mem.aepDimmPresent){
            // Build Memory Descriptor Hob for given memory range and Size, handle configs with AEP dimm only
            BldPMemResourceDescHobs(host, TopUseableMemStart, MEM_EQU_4GB);
          }
          if  (host->nvram.mem.RASmode & CH_ALL_MIRROR) {
            // Build Memory Descriptor Hob for given memory range and Size, handle configs with Mirroring only
            BldMirrResourceDescHobs(host, TopUseableMemStart, MEM_EQU_4GB);
          }
        }
        else {
          BuildResourceDescriptorHob(
            EFI_RESOURCE_SYSTEM_MEMORY,
            (
            EFI_RESOURCE_ATTRIBUTE_PRESENT |
            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
            ),
           TopUseableMemStart,
           MEM_EQU_4GB
           );
        }
        TopUseableMemSize = TopUseableMemSize - MEM_EQU_4GB;
        TopUseableMemStart = TopUseableMemStart + MEM_EQU_4GB;
      }
      //
      // Create hob for remain memory which above 4G memory.
      //
      if ( (host->nvram.mem.aepDimmPresent) || (host->nvram.mem.RASmode & CH_ALL_MIRROR) ) {
        if (host->nvram.mem.aepDimmPresent){
          // Build Memory Descriptor Hob for given memory range and Size, handle configs with AEP dimm only
          BldPMemResourceDescHobs(host, TopUseableMemStart, TopUseableMemSize);
        }
        if  (host->nvram.mem.RASmode & CH_ALL_MIRROR) {
          DEBUG((EFI_D_ERROR, "MIRROR Enabled, build resource descriptor hobs separately\n "));
          // Build Memory Descriptor Hob for given memory range and Size, handle configs with Mirroring only
          BldMirrResourceDescHobs(host, TopUseableMemStart, TopUseableMemSize);
        }
      } else {
        BuildResourceDescriptorHob(
           EFI_RESOURCE_SYSTEM_MEMORY,
           (
            EFI_RESOURCE_ATTRIBUTE_PRESENT |
            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
            ),
            TopUseableMemStart,
            TopUseableMemSize
            );
      }
    }
#ifdef NVMEM_FEATURE_EN
  }
#endif

// APTIOV_SERVER_OVERRIDE_RC_START : To support Capsule update in Purley
#if Capsule_SUPPORT
  //
  // If we found the capsule PPI (and we didn't have errors), then
  // call the capsule PEIM to allocate memory for the capsule.
  //
  if (Capsule != NULL) {
    Status = Capsule->CreateState ((EFI_PEI_SERVICES **)gPeiServices, CapsuleBuffer, CapsuleBufferLength);
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To support Capsule update in Purley

  return EFI_SUCCESS;
}

/**

  Find memory that is reserved so PEI has some to use.

  @param host - pointer to sysHost structure on stack

  @retval 0 - Success
  @retval Other - failure

**/
EFI_STATUS
InstallS3Memory (
  struct sysHost             *host
  )
{
  EFI_STATUS                  Status;
  UINTN                       BufferSize;
  UINT8                       Index;
  UINT8                       IioIndex;
  UINT8                       NumRanges;
  UINT8                       SmramIndex;
  UINT8                       SmramRanges;
  EFI_PHYSICAL_ADDRESS        TopUseableMemStart;
  EFI_PHYSICAL_ADDRESS        TopUseableMemSize;
  EFI_PHYSICAL_ADDRESS        LowMemSize;
  UINTN                       S3MemoryBase;
  UINTN                       S3MemorySize;
  EFI_PEI_HOB_POINTERS        Hob;
  RESERVED_ACPI_S3_RANGE      *S3MemoryRangeData;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE MemoryMap[MAX_RANGES];
  EFI_PEI_SERVICES            **gPeiServices;
  EFI_SMRAM_DESCRIPTOR        DescriptorAcpiVariable;
  UINT8                       Socket;

  gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  DEBUG ((EFI_D_ERROR, "InstallS3Memory()\n"));

  //
  // Get the Memory Map and calculate TSEG place
  //
  NumRanges = MAX_RANGES;
  ZeroMem (MemoryMap, sizeof (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges);
  Status = GetMemoryMap ((PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *) MemoryMap, &NumRanges, host);
  ASSERT_EFI_ERROR (Status);

  //
  // Get how manage SMRAM desc
  //
  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if  ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
          (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
          ) {
      SmramRanges++;
    }
  }
  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);

  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  Hob.Raw = BuildGuidHob (
              &gEfiSmmPeiSmramMemoryReserveGuid,
              BufferSize
                              );
  ASSERT (Hob.Raw);

  SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
        ) {
      //
      // This is an SMRAM range, create an SMRAM descriptor
      //
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalStart = MemoryMap[Index].PhysicalAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].CpuStart      = MemoryMap[Index].CpuAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalSize  = MemoryMap[Index].RangeLength;
      if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
      } else {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED;
      }

      SmramIndex++;
    }
  }

  //
  // Build a HOB with the location of the reserved memory range.
  //
  CopyMem(&DescriptorAcpiVariable, &SmramHobDescriptorBlock->Descriptor[SmramRanges-1], sizeof(EFI_SMRAM_DESCRIPTOR));
  DescriptorAcpiVariable.CpuStart += RESERVED_CPU_S3_SAVE_OFFSET;

  BuildGuidDataHob (
            &gEfiAcpiVariableGuid,
            &DescriptorAcpiVariable,
            sizeof (EFI_SMRAM_DESCRIPTOR)
            );

  DEBUG((DEBUG_INFO, "Installs3Memory: After Hob build.\n"));

  //
  // Get the location and size of the S3 memory range in the reserved page and
  // install it as PEI Memory.
  //

  DEBUG ((EFI_D_ERROR, "TSEG Base = 0x%08x\n", SmramHobDescriptorBlock->Descriptor[SmramRanges-1].PhysicalStart));
  S3MemoryRangeData = (RESERVED_ACPI_S3_RANGE*)(UINTN)
    (SmramHobDescriptorBlock->Descriptor[SmramRanges-1].PhysicalStart + RESERVED_ACPI_S3_RANGE_OFFSET);

  S3MemoryBase  = (UINTN) (S3MemoryRangeData->AcpiReservedMemoryBase);
  DEBUG ((EFI_D_ERROR, "S3MemoryBase = 0x%08x\n", S3MemoryBase));
  S3MemorySize  = (UINTN) (S3MemoryRangeData->AcpiReservedMemorySize);
  DEBUG ((EFI_D_ERROR, "S3MemorySise = 0x%08x\n", S3MemorySize));

  DEBUG((DEBUG_INFO, "Installs3Memory: S3MemoryBase: %x, S3MemorySize: %x.\n", S3MemoryBase, S3MemorySize));

  //
  // Program TSEG Base/Limit in IIO
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) == 0) continue;

    for (IioIndex = 0; IioIndex < MAX_IIO_STACK; IioIndex++) {
      if (host->var.common.stackPresentBitmap[Socket] & (1 << IioIndex)) {
          //
          // Program TSEG Base/Limit
          //
          WriteCpuPciCfgEx (host, Socket, IioIndex, TSEG_N0_IIO_VTD_REG, host->nvram.common.TsegBase.Data);
          WriteCpuPciCfgEx (host, Socket, IioIndex, TSEG_N1_IIO_VTD_REG, host->nvram.common.TsegLimit.Data);
      }
    }
  }

#if ME_SUPPORT_FLAG
  //
  // MESEG size requested and MESEG enabled in KTI before programming ME registers
  if (host->var.common.meRequestedSize != 0) {
    //
    // Setup ME SEG
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      //
      // Continue if this socket is not present
      //
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) == 0) continue;

      //
      // Program ME SEG Base/Limit in CHA
      //
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N0_CHABC_SAD_REG, host->nvram.common.MeMemLowBaseAddr.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N1_CHABC_SAD_REG, host->nvram.common.MeMemHighBaseAddr.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N0_CHABC_SAD_REG, host->nvram.common.MeMemLowLimit.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N1_CHABC_SAD_REG, host->nvram.common.MeMemHighLimit.Data);

      //
      // Program ME SEG Base/Limit in IIO
      //
      for (IioIndex = 0; IioIndex < MAX_IIO_STACK; IioIndex++) {
        if (host->var.common.stackPresentBitmap[Socket] & (1 << IioIndex)) {
           WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_BASE_0_IIO_VTD_REG, host->nvram.common.MeNcMemLowBaseAddr.Data);
           WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_BASE_1_IIO_VTD_REG, host->nvram.common.MeNcMemHighBaseAddr.Data);
           WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_LIMIT_0_IIO_VTD_REG, host->nvram.common.MeNcMemLowLimit.Data);
           WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_LIMIT_1_IIO_VTD_REG, host->nvram.common.MeNcMemHighLimit.Data);
        }
      }
    }
  }
#endif // ME_SUPPORT_FLAG

  Status        = PeiServicesInstallPeiMemory (S3MemoryBase, S3MemorySize);

  DEBUG((DEBUG_INFO, "Installs3Memory: After InstallPeiMemory. \n"));

  ASSERT_EFI_ERROR (Status);

  //
  // Retrieve the system memory length and build memory hob for the system
  // memory above 1MB. So Memory Callback can set cache for the system memory
  // correctly on S3 boot path, just like it does on Normal boot path.
  //
  LowMemSize = ((EFI_PHYSICAL_ADDRESS)host->var.mem.lowMemSize) * CONVERT_64MB_TO_BYTE;

  ASSERT ((S3MemoryRangeData->SystemMemoryLength - 0x100000) > 0);
  BuildResourceDescriptorHob (
            EFI_RESOURCE_SYSTEM_MEMORY,
            (
            EFI_RESOURCE_ATTRIBUTE_PRESENT |
            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
            ),
            0x100000,
            (S3MemoryRangeData->SystemMemoryLength - LowMemSize) - 0x100000
            );

  for (Index = 0; Index < NumRanges; Index++) {
    if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
        (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < 0x100000)) {
      BuildResourceDescriptorHob (
                EFI_RESOURCE_SYSTEM_MEMORY,
                (
                EFI_RESOURCE_ATTRIBUTE_PRESENT |
                EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                ),
                MemoryMap[Index].PhysicalAddress,
                MemoryMap[Index].RangeLength
                );
      DEBUG ((EFI_D_INFO, "Build resource HOB for Legacy Region on S3 patch :"));
      DEBUG ((EFI_D_INFO, " Memory Base:0x%lX Length:0x%lX\n", MemoryMap[Index].PhysicalAddress, MemoryMap[Index].RangeLength));
    }
  }

  //
  // Get the current "Top of Upper Usable Memory" address from TOUUD.
  // If TOUUD > 4G, it means memory re-mapped.
  //
  TopUseableMemSize = ((EFI_PHYSICAL_ADDRESS)host->var.mem.highMemSize) * CONVERT_64MB_TO_BYTE;
  TopUseableMemStart = ((EFI_PHYSICAL_ADDRESS)host->var.mem.highMemBase) * CONVERT_64MB_TO_BYTE;

  //Add descriptor hob for memory above 4GB
  if (TopUseableMemSize > 0) {
    //
    // if memory hob length is above 4G length,cut and separate it.
    //
    while (TopUseableMemSize > MEM_EQU_4GB) {
      BuildResourceDescriptorHob (
                EFI_RESOURCE_SYSTEM_MEMORY,
                (
                EFI_RESOURCE_ATTRIBUTE_PRESENT |
                EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                ),
           TopUseableMemStart,
                MEM_EQU_4GB
                );

      TopUseableMemSize = TopUseableMemSize - MEM_EQU_4GB;
      TopUseableMemStart = TopUseableMemStart + MEM_EQU_4GB;
    }
    //
    // Create hob for remain memory which above 4G memory.
    //
    BuildResourceDescriptorHob (
              EFI_RESOURCE_SYSTEM_MEMORY,
              (
              EFI_RESOURCE_ATTRIBUTE_PRESENT |
              EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
              EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
              EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
              ),
         TopUseableMemStart,
              TopUseableMemSize
              );
  }

  return EFI_SUCCESS;
}

/**

  Determine the memory size desired.  Based on HOB memory information.

  @param host - pointer to sysHost structure on stack
  @param size - pointer for reporting memory size required

  @retval None

**/
VOID
RetrieveRequiredMemorySize (
  PSYSHOST host,
  UINTN    *Size
  )
{
  EFI_MEMORY_TYPE_INFORMATION *MemoryData;
  UINT8                       Index;
  UINTN                       TempPageNum;
  EFI_PEI_SERVICES            **gPeiServices;
  EFI_HOB_GUID_TYPE           *GuidHob;

  gPeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  *Size         = 0;
  MemoryData    = NULL;

  GuidHob     = GetFirstGuidHob (&gEfiMemoryTypeInformationGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    return;
  }
  MemoryData  = GET_GUID_HOB_DATA(GuidHob);

  //
  // Platform PEIM should supply such a information. Generic PEIM doesn't assume any default value
  //
  if (MemoryData == NULL) {
    return ;
  }

  TempPageNum = 0;
  for (Index = 0; MemoryData[Index].Type != EfiMaxMemoryType; Index++) {
    //
    // Accumulate default memory size requirements
    //
    TempPageNum += MemoryData[Index].NumberOfPages;
  }

  if (TempPageNum == 0) {
    return ;
  }
  //
  // Two more pages are used by DXE memory manager
  //
  *Size = (TempPageNum + 4) * EFI_PAGE_SIZE;

  return ;
}

/**

  This function returns the memory ranges to be enabled, along with information
  describing how the range should be used.

  @param MemoryMap: Buffer to record details of the memory ranges to be enabled.

  NumRanges: On input, this contains the maximum number of memory ranges that
             can be described in the MemoryMap buffer.

  host: pointer to sysHost structure on stack


  @retval The MemoryMap buffer will be filled in and NumRanges will contain the actual
  @retval number of memory ranges that are to be anabled.

  0 - completed successfully.

**/
EFI_STATUS
GetMemoryMap (
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE                   *MemoryMap,
  UINT8                                                   *NumRanges,
  struct sysHost                                          *host
  )
{
  BOOLEAN                           EnableSmram;
  EFI_PHYSICAL_ADDRESS              MemorySize;
  EFI_PHYSICAL_ADDRESS              RowLength;
  EFI_PHYSICAL_ADDRESS              AlignedTsegBase = 0;
  EFI_PHYSICAL_ADDRESS              AlignedIedBase = 0;
  PEI_MEMORY_RANGE_SMRAM            SmramMask;
  PEI_MEMORY_RANGE_SMRAM            TsegMask;
  UINT32                            BlockNum;
  UINT8                             ExtendedMemoryIndex;
  UINT8                             Index;
  UINT8                             IioIndex;
  UINT8                             Socket;
  TSEG_N0_IIO_VTD_STRUCT            TsegBase;
  TSEG_N1_IIO_VTD_STRUCT            TsegLimit;
  EFI_PEI_SERVICES                  **gPeiServices;
  EFI_PLATFORM_INFO                 *PlatformInfo;
  EFI_HOB_GUID_TYPE                 *GuidHob;
  UINT32                            BiosGuardMemSizeMB;
#if ME_SUPPORT_FLAG
  UINT32                            SMRRSize;
  UINT32                            GapForSMRRAlign;
  EFI_PHYSICAL_ADDRESS              MemorySize_gap;
  EFI_PHYSICAL_ADDRESS              AlignedTsegBase_gap;
  UINT8                             MesegIndex = 0;
  EFI_PHYSICAL_ADDRESS              AlignedMesegBase;
  MESEG_BASE_N0_CHABC_SAD_STRUCT MeMemLowBaseAddr;
  MESEG_BASE_N1_CHABC_SAD_STRUCT MeMemHighBaseAddr;
  MESEG_LIMIT_N0_CHABC_SAD_STRUCT  MeMemLowLimit;
  MESEG_LIMIT_N1_CHABC_SAD_STRUCT  MeMemHighLimit;
  MENCMEM_BASE_0_IIO_VTD_STRUCT     MeNcMemLowBaseAddr;
  MENCMEM_BASE_1_IIO_VTD_STRUCT     MeNcMemHighBaseAddr;
  MENCMEM_LIMIT_0_IIO_VTD_STRUCT    MeNcMemLowLimit;
  MENCMEM_LIMIT_1_IIO_VTD_STRUCT    MeNcMemHighLimit;
#endif  // ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
  EFI_STATUS                        Status;
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

  gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  if ((*NumRanges) < MAX_RANGES) {
    return EFI_BUFFER_TOO_SMALL;
  }

  *NumRanges = 0;

  //
  // Find out which memory ranges to reserve on this platform
  //
  SmramMask = (PlatformInfo->MemData.MemTsegSize >> 17) | PEI_MR_SMRAM_CACHEABLE_MASK | PEI_MR_SMRAM_TSEG_MASK;

  //
  //  DEBUG ((EFI_D_ERROR, " SmramMask          = 0x%X\n", SmramMask));
  //
  // Generate memory ranges for the memory map.
  //
  EnableSmram = FALSE;
  MemorySize  = 0;
  Index       = 0;

  //
  // Get the current "max usable memory" address from TOLUD because we will not
  // support any memory above 4Gig.  Will ignore the memory between 4G and TOUUD.
  //
  RowLength = (host->var.mem.lowMemBase + host->var.mem.lowMemSize) * CONVERT_64MB_TO_BYTE;
  //
  // Debug code because the MRC isnt filling this data in just yet!
  //
  if (!RowLength) {
    RowLength = 512 * 1024 * 1024;
    //
    // hardcode the $%^&**$%^ to 512MB!
    //
  }
  //
  // System is very unlikely to work with less than 32MB
  //
  ASSERT (RowLength >= (32 * 1024 * 1024));

  DEBUG((EFI_D_ERROR, "MRC: lowMemBase:%x lowMemSize:%x\n",host->var.mem.lowMemBase,host->var.mem.lowMemSize ));
  DEBUG((EFI_D_ERROR, "MRC: highMemBase:%x highMemSize:%x\n",host->var.mem.highMemBase,host->var.mem.highMemSize));


  //
  // Add memory below 640KB to the memory map. Make sure memory between
  // 640KB and 1MB are reserved, even if not used for SMRAM
  //
  MemoryMap[*NumRanges].RowNumber       = Index;
  MemoryMap[*NumRanges].PhysicalAddress = MemorySize;
  MemoryMap[*NumRanges].CpuAddress      = MemorySize;
  MemoryMap[*NumRanges].RangeLength     = 0xA0000;
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // Reserve ABSEG or HSEG SMRAM if needed
  //
  if (SmramMask & (PEI_MR_SMRAM_ABSEG_MASK | PEI_MR_SMRAM_HSEG_MASK)) {
    EnableSmram = TRUE;
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].RangeLength     = MC_ABSEG_HSEG_LENGTH;

    if (SmramMask & PEI_MR_SMRAM_ABSEG_MASK) {
      MemoryMap[*NumRanges].CpuAddress = MC_ABSEG_CPU_START;
    } else {
      MemoryMap[*NumRanges].CpuAddress = MC_HSEG_CPU_START;
    }
    //
    // Chipset only supports cacheable SMRAM
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramCacheable;
  } else {
    //
    // Just mark this range reserved
    //
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].CpuAddress      = MC_ABSEG_CPU_START;
    MemoryMap[*NumRanges].RangeLength     = 0x60000;
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
  }

  MemoryMap[*NumRanges].RowNumber = Index;
  (*NumRanges)++;

  RowLength -= 0x100000;
  MemorySize = 0x100000;

  //
  // Add remaining memory to the memory map
  //
  if (RowLength > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = MemorySize;
    MemoryMap[*NumRanges].CpuAddress      = MemorySize;
    MemoryMap[*NumRanges].RangeLength     = RowLength;
    MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
    (*NumRanges)++;
    MemorySize += RowLength;
  }

  ExtendedMemoryIndex = (UINT8) (*NumRanges - 1);

#if ME_SUPPORT_FLAG
  //
  // See if we need to trim MESEG out of the highest memory range below 4GB
  //
  PlatformInfo->MemData.MemMESEGSize = 0;
  PlatformInfo->MemData.MemMESEGBase = 0;

  if (host->var.common.meRequestedSize != 0) {
    DEBUG((EFI_D_ERROR, "ME FW: meRequestedSize in MB: %d\n", host->var.common.meRequestedSize));

    //
    // Remove MESEG DRAM range from the available memory map
    //
    MemoryMap[*NumRanges].RangeLength = (host->var.common.meRequestedSize * 1024 * 1024);
    MemoryMap[*NumRanges].RowNumber   = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize -= MemoryMap[*NumRanges].RangeLength;
    //
    // MESEG base requires 1MB alignment.
    // Need to adjust memory map accordingly.
    //
    AlignedMesegBase = MemorySize &~(0xFFFFF);
    MemoryMap[*NumRanges].RangeLength += (MemorySize - AlignedMesegBase);

    MemoryMap[*NumRanges].PhysicalAddress = AlignedMesegBase;
    MemoryMap[*NumRanges].CpuAddress      = AlignedMesegBase;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type = DualChannelDdrReservedMemory;

    MesegIndex = (UINT8) (*NumRanges);
    PlatformInfo->MemData.MemMESEGBase = (UINT64)MemoryMap[MesegIndex].PhysicalAddress;
    PlatformInfo->MemData.MemMESEGSize = (UINT64)MemoryMap[MesegIndex].RangeLength;
    DEBUG((EFI_D_ERROR, "MRC: PhysicalAddress:%x; RangeLength:%x\n",(UINT32)MemoryMap[MesegIndex].PhysicalAddress,MemoryMap[MesegIndex].RangeLength));

    (*NumRanges)++;


  }
#endif //ME_SUPPORT_FLAG

  //
  // See if we need to trim TSEG out of the highest memory range
  //
  if (SmramMask & PEI_MR_SMRAM_TSEG_MASK) {
    //
    // Create the new range for TSEG and remove that range from the previous SdrDdrMainMemory range
    //
    TsegMask  = (SmramMask & PEI_MR_SMRAM_SIZE_MASK);

    BlockNum  = 1;
    while (TsegMask) {
      TsegMask >>= 1;
      BlockNum <<= 1;
    }

    BlockNum >>= 1;

    switch (BlockNum) {
    case PEI_MR_SMRAM_SIZE_1024K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_2048K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_8192K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_16M_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_32M_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_64M_MASK:
      break;

  case PEI_MR_SMRAM_SIZE_128M_MASK:
    break;

    default:
      //
      // Non supported size. Set to 0.
      //
      BlockNum = 0;
      break;
    }
    DEBUG((EFI_D_ERROR, "MRC: TSEG BlockNum:%d\n",BlockNum));

    if (BlockNum) {
      EnableSmram                       = TRUE;

#if ME_SUPPORT_FLAG
  //
  // See if we need to build a hob for memory gap caused by SMRR alignment
  //
  SMRRSize = (BlockNum * 128 * 1024);
  DEBUG((EFI_D_ERROR, "MRC: SMRR Size: %x\n", SMRRSize));
  MemorySize_gap = MemorySize - SMRRSize;
  AlignedTsegBase_gap = MemorySize_gap &~(SMRRSize-1);
  GapForSMRRAlign = (UINT32)(MemorySize_gap - AlignedTsegBase_gap);         // If MESeg is at top, then we possibly have SMRR alignment issue
  if( GapForSMRRAlign )
  {
    //
    // Remove SMRR alignment gap reserved range from the available memory map
    //
    MemoryMap[*NumRanges].RangeLength = GapForSMRRAlign;
    MemoryMap[*NumRanges].RowNumber   = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].PhysicalAddress = AlignedTsegBase_gap + SMRRSize;
    MemoryMap[*NumRanges].CpuAddress      = AlignedTsegBase_gap + SMRRSize;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type = DualChannelDdrReservedMemory;

    DEBUG((EFI_D_ERROR, "GetMemoryMap: SMRR gap reserved base  : %08X\n", (UINT32)MemoryMap[*NumRanges].PhysicalAddress));
    DEBUG((EFI_D_ERROR, "GetMemoryMap: SMRR gap reserved length: %08X\n", (UINT32)MemoryMap[*NumRanges].RangeLength));
    (*NumRanges)++;
  } //if( GapForSMRRAlign )

#endif //ME_SUPPORT_FLAG

      MemoryMap[*NumRanges].RangeLength = (BlockNum * 128 * 1024);
      MemoryMap[*NumRanges].RowNumber   = MemoryMap[ExtendedMemoryIndex].RowNumber;
      MemorySize -= MemoryMap[*NumRanges].RangeLength;

      //
      // TSEG base requires TSEG-Size alignment, done at last step.
      //
//    AlignedTsegBase = MemorySize &~(0xFFFFF);
      AlignedTsegBase = MemorySize &~(MemoryMap[*NumRanges].RangeLength-1);
      MemoryMap[*NumRanges].RangeLength += (MemorySize - AlignedTsegBase);
      MemoryMap[*NumRanges].PhysicalAddress = AlignedTsegBase;
      MemoryMap[*NumRanges].CpuAddress      = AlignedTsegBase;
      //
      // Need to reserve the IED from TSEG
      //
      MemoryMap[*NumRanges].RangeLength -= PlatformInfo->MemData.MemIedSize;
      MemorySize += PlatformInfo->MemData.MemIedSize;
      MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
      AlignedIedBase = AlignedTsegBase + MemoryMap[*NumRanges].RangeLength;

    } // if (BlockNum)

    //
    // Chipset only supports cacheable SMRAM
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramCacheable;

    DEBUG((EFI_D_ERROR, "GetMemoryMap: TsegBase: %08X\n", (UINT32)AlignedTsegBase));
    DEBUG((EFI_D_ERROR, "GetMemoryMap: TsegRange: %08X\n", (UINT32)MemoryMap[*NumRanges].RangeLength));

    //
    // Program TSEG Base/Limit  in IIO
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      //
      // Continue if this socket is not present
      //
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) == 0) continue;

      for (IioIndex = 0; IioIndex < MAX_IIO_STACK; IioIndex++) {
        if (host->var.common.stackPresentBitmap[Socket] & (1 << IioIndex)) {
          //
          // Program TSEG Base
          //
          TsegBase.Data = ReadCpuPciCfgEx (host, Socket, IioIndex, TSEG_N0_IIO_VTD_REG);
          TsegBase.Bits.base = (UINT32)(MemoryMap[*NumRanges].PhysicalAddress) >> 20; // In MegaBytes
          WriteCpuPciCfgEx (host, Socket, IioIndex, TSEG_N0_IIO_VTD_REG, TsegBase.Data);
          //
          // Program TSEG Limit
          //
          TsegLimit.Data = ReadCpuPciCfgEx (host, Socket, IioIndex, TSEG_N1_IIO_VTD_REG);
          TsegLimit.Bits.limit = (UINT32)((MemoryMap[*NumRanges].PhysicalAddress + MemoryMap[*NumRanges].RangeLength + PlatformInfo->MemData.MemIedSize - 1)) >> 20;
          WriteCpuPciCfgEx (host, Socket, IioIndex, TSEG_N1_IIO_VTD_REG, TsegLimit.Data);
        }
      }
    }
    host->nvram.common.TsegBase.Data = ReadCpuPciCfgEx (host, 0, 0, TSEG_N0_IIO_VTD_REG);
    host->nvram.common.TsegLimit.Data = ReadCpuPciCfgEx (host, 0, 0, TSEG_N1_IIO_VTD_REG);

    (*NumRanges)++;

    //
    // And make an entry just for IED RAM, if used
    //
    if (PlatformInfo->MemData.MemIedSize) {
      MemoryMap[*NumRanges].RangeLength = PlatformInfo->MemData.MemIedSize;
      MemoryMap[*NumRanges].RowNumber   = MemoryMap[ExtendedMemoryIndex].RowNumber;
      MemorySize -= MemoryMap[*NumRanges].RangeLength;

      MemoryMap[*NumRanges].PhysicalAddress = AlignedIedBase;
      MemoryMap[*NumRanges].CpuAddress      = AlignedIedBase;
      MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;

      MemoryMap[*NumRanges].Type = DualChannelDdrReservedMemory;

      DEBUG((EFI_D_ERROR, "GetMemoryMap: IedRamBase: %08X\n", (UINT32)AlignedIedBase));
      DEBUG((EFI_D_ERROR, "GetMemoryMap: IedRamRange: %08X\n", (UINT32)MemoryMap[*NumRanges].RangeLength));

      (*NumRanges)++;
    }
  }


#if ME_SUPPORT_FLAG
  //
  // MESEG size requested and MESEG enabled in KTI before programming ME registers
  if (host->var.common.meRequestedSize != 0) {
    //
    // Setup ME SEG
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      //
      // Continue if this socket is not present
      //
      if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) == 0) continue;


      MeMemLowBaseAddr.Data = ReadCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N0_CHABC_SAD_REG);
      MeMemHighBaseAddr.Data = ReadCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N1_CHABC_SAD_REG);
      MeMemLowLimit.Data = ReadCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N0_CHABC_SAD_REG);
      MeMemHighLimit.Data = ReadCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N1_CHABC_SAD_REG);

      MeNcMemLowBaseAddr.Data = ReadCpuPciCfgEx (host, Socket, 0, MENCMEM_BASE_0_IIO_VTD_REG);
      MeNcMemHighBaseAddr.Data = ReadCpuPciCfgEx (host, Socket, 0, MENCMEM_BASE_1_IIO_VTD_REG);
      MeNcMemLowLimit.Data = ReadCpuPciCfgEx (host, Socket, 0, MENCMEM_LIMIT_0_IIO_VTD_REG);
      MeNcMemHighLimit.Data = ReadCpuPciCfgEx (host, Socket, 0, MENCMEM_LIMIT_1_IIO_VTD_REG);

      MeMemHighBaseAddr.Bits.mebase = 0;
      MeNcMemHighBaseAddr.Bits.addr = 0;
      MeMemHighLimit.Bits.melimit = 0;
      MeNcMemHighLimit.Bits.addr = 0;
      //
      // get ME base address from previous calculations
      //
      MeMemLowBaseAddr.Bits.mebase = (UINT32)(MemoryMap[MesegIndex].PhysicalAddress) >> 19;
      MeNcMemLowBaseAddr.Bits.addr = (UINT32)(MemoryMap[MesegIndex].PhysicalAddress) >> 19;
      //
      // get ME length from previous calculations
      //
      MeMemLowLimit.Bits.melimit = (UINT32)((MemoryMap[MesegIndex].PhysicalAddress + MemoryMap[MesegIndex].RangeLength - 1)) >> 19;
      MeMemLowLimit.Bits.en = 1;
      MeNcMemLowLimit.Bits.addr = (UINT32)((MemoryMap[MesegIndex].PhysicalAddress + MemoryMap[MesegIndex].RangeLength - 1)) >> 19;

      //
      // Program ME SEG Base/Limit in CHA
      //
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N0_CHABC_SAD_REG, MeMemLowBaseAddr.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_BASE_N1_CHABC_SAD_REG, MeMemHighBaseAddr.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N0_CHABC_SAD_REG, MeMemLowLimit.Data);
      WriteCpuPciCfgEx (host, Socket, 0, MESEG_LIMIT_N1_CHABC_SAD_REG, MeMemHighLimit.Data);

      //
      // Program ME SEG Base/Limit in IIO
      //
    for (IioIndex = 0; IioIndex < MAX_IIO_STACK; IioIndex++) {
    if (host->var.common.stackPresentBitmap[Socket] & (1 << IioIndex)) {
            WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_BASE_0_IIO_VTD_REG, MeNcMemLowBaseAddr.Data);
            WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_BASE_1_IIO_VTD_REG, MeNcMemHighBaseAddr.Data);
            WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_LIMIT_0_IIO_VTD_REG, MeNcMemLowLimit.Data);
            WriteCpuPciCfgEx (host, Socket, IioIndex, MENCMEM_LIMIT_1_IIO_VTD_REG, MeNcMemHighLimit.Data);
        }
    }
    }
  //
  //  Save CSR register values in host->nvram struc for use on S3 resume
  //
    host->nvram.common.MeMemLowBaseAddr.Data = ReadCpuPciCfgEx (host, 0, 0, MESEG_BASE_N0_CHABC_SAD_REG);
    host->nvram.common.MeMemHighBaseAddr.Data = ReadCpuPciCfgEx (host, 0, 0, MESEG_BASE_N1_CHABC_SAD_REG);
    host->nvram.common.MeMemLowLimit.Data = ReadCpuPciCfgEx (host, 0, 0, MESEG_LIMIT_N0_CHABC_SAD_REG);
    host->nvram.common.MeMemHighLimit.Data = ReadCpuPciCfgEx (host, 0, 0, MESEG_LIMIT_N1_CHABC_SAD_REG);

    host->nvram.common.MeNcMemLowBaseAddr.Data = ReadCpuPciCfgEx (host, 0, 0, MENCMEM_BASE_0_IIO_VTD_REG);
    host->nvram.common.MeNcMemHighBaseAddr.Data = ReadCpuPciCfgEx (host, 0, 0, MENCMEM_BASE_1_IIO_VTD_REG);
    host->nvram.common.MeNcMemLowLimit.Data = ReadCpuPciCfgEx (host, 0, 0, MENCMEM_LIMIT_0_IIO_VTD_REG);
    host->nvram.common.MeNcMemHighLimit.Data = ReadCpuPciCfgEx (host, 0, 0, MENCMEM_LIMIT_1_IIO_VTD_REG);

  }
#endif // ME_SUPPORT_FLAG

#ifdef LT_FLAG
  if (PcdGetBool(PcdDprSizeFeatureSupport)) {
    MemorySize -= PcdGet32 (PcdLtMemSize);
  } else {
    MemorySize -= PlatformInfo->MemData.MemLtMemSize;
  }
  MemoryMap[*NumRanges].RowNumber       = MemoryMap[ExtendedMemoryIndex].RowNumber;
  MemoryMap[*NumRanges].PhysicalAddress = MemorySize &~(0xFFFFF);
  MemoryMap[*NumRanges].CpuAddress      = MemorySize &~(0xFFFFF);
  MemoryMap[*NumRanges].RangeLength     = PlatformInfo->MemData.MemLtMemSize;
  if (PcdGetBool(PcdDprSizeFeatureSupport)) {
    MemoryMap[*NumRanges].RangeLength   = PcdGet32 (PcdLtMemSize);
  }
  MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
  MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
  PlatformInfo->MemData.MemLtMemAddress = (UINT32)(MemoryMap[*NumRanges].PhysicalAddress);
  DEBUG((EFI_D_ERROR, "Reserve LT Mem[0x%016lx] Size = 0x%016lx\n", MemoryMap[*NumRanges].PhysicalAddress, MemoryMap[*NumRanges].RangeLength));
  (*NumRanges)++;
#endif

  //
  // Reserve BIOS Guard Memory based on the size requested if enabled
  //
  BiosGuardMemSizeMB = PlatformInfo->MemData.BiosGuardMemSize;

  if (BiosGuardMemSizeMB != 0) {

    MemoryMap[*NumRanges].RangeLength           = (UINT64) LShiftU64 (BiosGuardMemSizeMB, 20);
    MemoryMap[*NumRanges].RowNumber             = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize                                 -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].PhysicalAddress       = MemorySize;
    MemoryMap[*NumRanges].CpuAddress            = MemorySize;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type                  = DualChannelDdrReservedMemory;

    PlatformInfo->MemData.BiosGuardPhysBase = (UINT32)MemorySize;
    DEBUG((EFI_D_ERROR, "Reserve BIOS Guard Mem[0x%08x] Size = 0x%08x\n", MemoryMap[*NumRanges].PhysicalAddress, MemoryMap[*NumRanges].RangeLength));
    (*NumRanges)++;
  }
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
  Status = PcdSet32S (PcdAvailableMemoryBelow4G , (UINT32)MemoryMap[ExtendedMemoryIndex].RangeLength);
  ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

  ASSERT((*NumRanges) <= MAX_RANGES);
  return EFI_SUCCESS;
}

#ifdef BDAT_SUPPORT
/**
  This function saves the Bdat Data from sysHost to the HOB for later use when it will be copied over to the ACPI table in memory.
  It also saves the BDAT Data Blob entered via the BSSA function - BiosSaveToBdat()

  @param host - pointer to sysHost structure on stack

  @retval 0 - Successful
  @retval Other - Failure

**/
EFI_STATUS
SaveBdatDataToHob (
  struct sysHost      *host
  )
{
  VOID                  *CurrentHobLocation;
  UINT32                csrReg = 0;
  UINTN                 RemainingHobSizeMemSchema, RemainingHobSizeRmtSchema;
  UINTN                 CurrentHobSize;

  RemainingHobSizeMemSchema = sizeof(BDAT_MEMORY_DATA_STRUCTURE);
  RemainingHobSizeRmtSchema = sizeof(BDAT_RMT_STRUCTURE);

  //
  // Save BDAT to Scratchpad
  //
  csrReg = (UINT32)&host->bdat;
  WriteCpuPciCfgEx (host, 0, 0, SR_BDAT_STRUCT_PTR_CSR, csrReg);

  CurrentHobLocation = &host->bdat;
  //Putting the Memory Schema into a HOB/HOBs
  while (RemainingHobSizeMemSchema) {
    if (RemainingHobSizeMemSchema >= MAX_HOB_ENTRY_SIZE) {
      CurrentHobSize = MAX_HOB_ENTRY_SIZE;
    } else {
      CurrentHobSize = RemainingHobSizeMemSchema;
    }
    //An instance of the BDAT Data HOB getting created in each loop iteration with the GUID
    BuildGuidDataHob (&gEfiMemoryMapDataHobBdatGuid, CurrentHobLocation, CurrentHobSize);

    RemainingHobSizeMemSchema -= CurrentHobSize;
    CurrentHobLocation = (VOID *)((UINT32) CurrentHobLocation + CurrentHobSize);
  }

  CurrentHobLocation = &host->bdatRmt;
  //Putting the RMT Schema into a HOB
  while (RemainingHobSizeRmtSchema) {
    if (RemainingHobSizeRmtSchema >= MAX_HOB_ENTRY_SIZE) {
      CurrentHobSize = MAX_HOB_ENTRY_SIZE;
    } else {
      CurrentHobSize = RemainingHobSizeRmtSchema;
    }
    //An instance of the BDAT Data HOB getting created in each loop iteration with the GUID
    BuildGuidDataHob (&gEfiRmtDataHobBdatGuid, CurrentHobLocation, CurrentHobSize);

    RemainingHobSizeRmtSchema -= CurrentHobSize;
    CurrentHobLocation = (VOID *)((UINT32) CurrentHobLocation + CurrentHobSize);
  }

  return EFI_SUCCESS;
}
#endif //BDAT_SUPPORT

/**

  This function saves the S3 Restore Data to Hob for later use.

  @param host - pointer to sysHost structure on stack

  @retval 0 - Successful
  @retval Other - Failure

**/
EFI_STATUS
MrcSaveS3RestoreDataToHob (
  struct sysHost      *host
  )
{
  VOID                  *CurrentHob;
  UINTN                 RemainingHobSize;
  UINTN                 CurrentHobSize;

  RemainingHobSize = sizeof(struct sysNvram);
  CurrentHob = &(host->nvram);
  DEBUG((EFI_D_INFO, "MrcSaveS3RestoreDataToHob(): RemainingHobSize Size(bytes): %d\n", RemainingHobSize));
  DEBUG((EFI_D_INFO, "    CurrentHob - address of nvram: 0x%x\n", CurrentHob));

  while (RemainingHobSize) {

    if (RemainingHobSize >= MAX_HOB_ENTRY_SIZE){
      CurrentHobSize = MAX_HOB_ENTRY_SIZE;
    } else {
      CurrentHobSize = RemainingHobSize;
    }

    BuildGuidDataHob (&gEfiMemoryConfigDataHobGuid, CurrentHob, CurrentHobSize);

    RemainingHobSize -= CurrentHobSize;
    CurrentHob = (VOID *) ((UINT32) CurrentHob + CurrentHobSize);
  }

  return EFI_SUCCESS;
}

