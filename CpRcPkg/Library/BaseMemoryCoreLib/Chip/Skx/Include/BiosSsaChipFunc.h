//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
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

//
//  This file contains function subsets of CHIP to CORE BIOS SSA API functions
//

#ifndef __BIOS_SSA_CHIP_FUNC__
#define __BIOS_SSA_CHIP_FUNC__

#include "SysFunc.h"
#include "MrcSsaServices.h"
#include "BiosSsaFunc.h"


#ifdef SSA_FLAG

//RC SIM has MINIBIOS_BUILD defined: 'Stitched SSA code' - Included in RC Sim & excluded from the Minibios
#ifdef MINIBIOS_BUILD
#else
#define EV_STITCHED_CONTENT_ENBL  //Not skipping code for the full bios
#endif //MINIBIOS_BUILD

//
// BiosSsaMemoryConfig.c
//
BOOLEAN SSAIsLrdimmPresent (PSYSHOST host);
BOOLEAN SSAIsDdrtDimmPresent (PSYSHOST host);
BOOLEAN SSAIsRegRev2DimmPresent (PSYSHOST host);
VOID    BiosGetMemTempChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Dimm, INT32 *Temperature);
VOID    BiosIoResetChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 resetIo);
VOID    BiosInitMarginParamChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 cmdOeVal);
VOID    BiosSetZQCalConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, BOOLEAN Enable, BOOLEAN *PrevEnable);
VOID    BiosSetRCompUpdateConfigChip (PSYSHOST host, UINT8 Socket, BOOLEAN Enable, BOOLEAN *PrevEnable);
VOID    BiosSetPageOpenConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, BOOLEAN Enable, BOOLEAN *PrevEnable);
VOID    BiosInitCpgcChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, SETUP_CLEANUP SetupCleanup);
SSA_STATUS BiosGetMarginParamSpecificityChip (PSYSHOST host, GSM_LT IoLevel, GSM_GT MarginGroup, MARGIN_PARAM_SPECIFICITY *Specificity);

//
// BiosSsaMemoryConfigServer.c
//
VOID    BiosServerGet3dsRankPerCsChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Dimm, UINT8 *RankCount);
VOID    BiosServerSetCreditWaitConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, BOOLEAN Enable);
VOID    BiosServerSetWpqInOrderConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, BOOLEAN Enable);
VOID    BiosServerSetMultipleWrCreditConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, BOOLEAN Enable);
VOID    BiosServerSetDqdbIndexesChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 StartIndex, UINT8 EndIndex, DQDB_INC_RATE_MODE IncRateMode, UINT8 IncRate);
VOID    BiosServerSetDqdbEccDataSourceChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, ECC_DATA_SOURCE EccDataSource);
VOID    BiosServerSetDqdbUniseqModeChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, DQDB_UNISEQ_MODE Mode);
VOID    BiosServerSetDqdbUniseqWrSeedChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT32 Seed);
VOID    BiosServerSetDqdbUniseqRdSeedChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT32 Seed);
VOID    BiosServerGetDqdbUniseqWrSeedChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT32 *Seed);
VOID    BiosServerGetDqdbUniseqRdSeedChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT32 *Seed);
VOID    BiosServerSetDqdbUniseqLmnChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT16 L, UINT16 M, UINT16 N, UINT8 LDataSel, BOOLEAN EnableFreqSweep);
VOID    BiosServerSetDqdbUniseqLfsrConfigChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 ReloadRate, UINT8 SaveRate);
VOID    BiosServerSetDqdbInvertDcConfigChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, INVERT_DC_MODE Mode, UINT8 DcPolarity, UINT8 ShiftRateExponent, UINT32 DqMask1Low, UINT32 DqMask1High, UINT8 EccMask);
VOID    BiosServerSetCadbConfigChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN EnableAlwaysOn, BOOLEAN EnableOnDeselect, BOOLEAN EnableParityNTiming, BOOLEAN EnableOnePassAlwaysOn, BOOLEAN EnablePdaDoubleLength);
VOID    BiosServerSetCadbUniseqModeChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, CADB_UNISEQ_MODE DeselectMode);
VOID    BiosServerSetCadbUniseqSeedChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq, UINT32 DeselectSeed);
VOID    BiosServerSetCadbUniseqLmnChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Uniseq,  UINT16 L, UINT16 M, UINT16 N, UINT8 LDataSel, BOOLEAN EnableFreqSweep);
VOID    BiosServerSetDummyReadLmnChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket,  UINT16 L, UINT16 M, UINT16 N, UINT8 LDataSel, BOOLEAN EnableFreqSweep);
VOID    BiosServerSetDummyReadBankMaskChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket,  UINT16 BankMask);
VOID    BiosServerSetSubseqConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 Subseq, BURST_LENGTH_MODE BurstLengthMode, UINT32 BurstLength, UINT32 InterSubseqWait, SUBSEQ_TYPE SubseqType, BOOLEAN EnableSaveCurrentBaseAddrToStart, BOOLEAN EnableResetCurrentBaseAddrToStart, BOOLEAN EnableAddrInversion, BOOLEAN EnableDataInversion);
VOID    BiosServerSetSubseqOffsetConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 Subseq, BOOLEAN EnableBaseInvertData, BOOLEAN EnableRowIncrement, UINT8 RowIncrementOrder, BOOLEAN EnableColIncrement, UINT8 ColIncrementOrder, SUBSEQ_TYPE BaseSubseqType, UINT32 BaseSubseqRepeatRate, UINT32 OffsetAddrUpdateRate);
VOID    BiosServerSetSeqConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT32 StartDelay, UINT8 SubseqStartIndex, UINT8 SubseqEndIndex, UINT32 LoopCount, CPGC_INIT_MODE InitMode, BOOLEAN EnableGlobalControl, BOOLEAN EnableConstantWriteStrobe, BOOLEAN EnableDummyReads, ADDR_UPDATE_RATE_MODE AddrUpdateRateMode);
VOID    BiosServerSetLoopCountChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT32 LoopCount);
VOID    BiosServerSetSeqAddrConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, BOOLEAN EnableRankWrapCarry, BOOLEAN EnableBankWrapCarry, BOOLEAN EnableRowWrapCarry, BOOLEAN EnableColWrapCarry, BOOLEAN EnableRankAddrInvert, BOOLEAN EnableBankAddrInvert, BOOLEAN EnableRowAddrInvert, BOOLEAN EnableColAddrInvert, UINT8 AddrInvertRate,  UINT8 RankAddrOrder, UINT8 BankAddrOrder, UINT8 RowAddrOrder, UINT8 ColAddrOrder);
VOID    BiosServerSetSeqStartAddrChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 RankAddr, UINT8 BankAddr, UINT32 RowAddr, UINT32 ColAddr);
VOID    BiosServerSetSeqWrapAddrChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 RankAddr, UINT8 BankAddr, UINT32 RowAddr, UINT32 ColAddr);
VOID    BiosServerSetSeqAddrIncChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, ADDR_INC_MODE RankAddrIncMode, UINT32 RankAddrIncRate, INT8 RankAddrIncVal, ADDR_INC_MODE BankAddrIncMode, UINT32 BankAddrIncRate, INT8 BankAddrIncVal, ADDR_INC_MODE RowAddrIncMode, UINT32 RowAddrIncRate, INT16 RowAddrIncVal, ADDR_INC_MODE ColAddrIncMode, UINT32 ColAddrIncRate, INT16 ColAddrIncVal);
VOID    BiosServerSetSeqRankMapChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 PairCount, CONST DIMM_RANK_PAIR  *DimmRankPairs);
VOID    BiosServerSetSeqRowAddrSwizzleChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, UINT8 SwizzleCount, CONST UINT8 *Swizzles);
VOID    BiosServerGetSeqStateChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, SSA_SEQ_STATE *SeqState);
VOID    BiosServerSetStopModeChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, CPGC_STOP_MODE StopMode, UINT32 StopOnNthErrorCount);
VOID    BiosServerSetErrorCounterModeChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Counter, ERROR_COUNTER_MODE Mode, UINT32 ModeIndex);
VOID    BiosServerSetLaneValidationMaskChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT32 DqMaskLow, UINT32 DqMaskHigh, UINT32 EccMask32);
VOID    BiosServerSetChunkValidationMaskChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 ChunkPairsToCheck);
VOID    BiosServerSetCachelineValidationMaskChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 CachelineMask);
VOID    BiosServerGetLaneErrorStatusChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 ChunkMask, UINT8 *Status);
VOID    BiosServerGetChunkErrorStatusChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 *Status);
VOID    BiosServerGetErrorAddressChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, SSA_ADDRESS *Address);
VOID    BiosServerGetErrorCountChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Counter, UINT32 *Count, BOOLEAN *Overflow);
VOID    BiosServerSetDqdbErrorCaptureConfigChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN Enable, UINT8 StartCacheline, UINT8 EndCacheline);
VOID    BiosServerGetDqdbErrorCaptureStatusChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 *CurrentCacheline);
VOID    BiosServerSetLocalTestControlChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, BOOLEAN ClearErrors, BOOLEAN StartTest, BOOLEAN StopTest);
VOID    BiosServerSetGlobalTestControlChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, BOOLEAN ClearErrors, BOOLEAN StartTest, BOOLEAN StopTest);
VOID    BiosServerSetAddrConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelInSocket, BOOLEAN EnableDdr4);
VOID    BiosServerGetTestDoneChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, UINT8 ChannelMask, UINT8 *TestDoneMask);
VOID    BiosServerSetScramblerConfigChip (PSYSHOST host, UINT8 Socket, UINT8 Controller, BOOLEAN Enable, BOOLEAN *PrevEnable);
VOID    BiosServerSetRefreshOverrideChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN EnablePanicRefreshOnly, UINT8 PairCount, CONST DIMM_RANK_PAIR *DimmRankPairs);
VOID    BiosServerSetZQCalOverrideChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN EnableZQCalAfterRefresh, UINT8 PairCount, CONST DIMM_RANK_PAIR *DimmRankPairs);
VOID    BiosServerSetOdtOverrideChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN EnableMprDdrTrain, UINT8 ValCount, CONST DIMM_RANK_PAIR *DimmRankPairs, CONST UINT8 *OverrideVals);
VOID    BiosServerSetCkeOverrideChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 ValCount, CONST DIMM_RANK_PAIR *DimmRankPairs, CONST UINT8 *OverrideVals);

//
// BiosSsaMemoryDdrtConfigChip.c
//
VOID    BiosSetEridValidationMaskChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 EridMask);
VOID    BiosGetEridErrorStatusChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 *Status, UINT16 *Count);
VOID    BiosSetEridPatternChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, ERID_SIGNAL Signal, ERID_PATTERN_MODE PatternMode, BOOLEAN EnableReload, BOOLEAN EnableReset, UINT32 Seed);
VOID    BiosSetEridTrainingConfigChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, BOOLEAN EnableTraining);
VOID    BiosAepIoResetChip (PSYSHOST host, UINT8 Socket, UINT8 ChannelInSocket, UINT8 Dimm);

//
// MrcSsaServices.c
//
#ifdef SSA_LOADER_FLAG
VOID        SsaBiosLoadStatusChip (PSYSHOST host, UINT32 agentReg);
#endif //SSA_LOADER_FLAG
VOID        SsaStructAddressLocationsChip (PSYSHOST host, SSA_BIOS_SERVICES_PPI *SsaBiosServicesPpi, SSA_COMMON_CONFIG *SsaCommonConfig, SSA_RESULTS_CONFIG *SsaResultsConfig, SSA_MEMORY_CONFIG *SsaMemoryConfig, SSA_MEMORY_CLIENT_CONFIG *SsaMemoryClientConfig, SSA_MEMORY_SERVER_CONFIG *SsaMemoryServerConfig, SSA_MEMORY_DDRT_CONFIG *SsaMemoryDdrtConfig);
#ifdef EV_STITCHED_CONTENT_ENBL
VOID        SsaStichedModeRMTChip (PSYSHOST host, SSA_BIOS_SERVICES_PPI *SsaBiosServicesPpi);
#endif
SSA_STATUS  EvTestEntry (SSA_BIOS_SERVICES_PPI *SsaServicesHandle);

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // __BIOS_SSA_CHIP_FUNC__
