/** @file

  This file contains the BIOS implementation of the BIOS-SSA Server Memory Configuration API.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights
  reserved. This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysFunc.h"
#include "BiosSsaChipFunc.h"

#ifdef SSA_FLAG

/**
  Calculates the logicalRank[MAX_RANK_CH] values for a channel  based on the Dimm/Rank pair values entered from the array “DimmRankPairs”.
  In the logicalRankMapping[8] array where the “logicalRank” values are getting saved for a channel. By default,
  all array elements are set to 0XF and are modified with the “correct” logical rank value only if that Dimm/Rank is valid/“enabled”, else we leave the value as 0XF.
  @param[in]      MrcData   - Pointer to the MRC global data area.
  @param[in]      Socket    - Zero based CPU socket number.
  @param[in]      Channel   - Zero based memory channel number.
  @param[in]      Dimm      - Zero based memory DIMM number.
  @param[in]      Rank      - Zero based memory Physical Rank number.
  @param[in]      logicalRankMapping - Pointer to an array where the logical Rank values for a channel get saved
  @retval Success.
**/
void
SSAGetLogicalRank (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 Rank,
  UINT8                 *logicalRankMapping
  )
{
//Based on the Dimm-Rank pairs we calculate the corresponding logicalRanks
  if ((host->nvram.mem.socket[Socket].channelList[Channel].enabled) &&
     (host->nvram.mem.socket[Socket].channelList[Channel].dimmList[Dimm].dimmPresent) &&
     (host->nvram.mem.socket[Socket].channelList[Channel].dimmList[Dimm].rankList[Rank].enabled) &&
     (host->nvram.mem.socket[Socket].channelList[Channel].dimmList[Dimm].mapOut[Rank] == 0)) {

     //For invalid Dimm/Rank values, that location in the logicalRankArray will have a value of 0xFF
     *logicalRankMapping = GetLogicalRank(host, Socket, Channel, Dimm , Rank);  // Get logical rank
  }
}

/**
  Function used to get information about about the CPGC capabilities.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     CpgcInfo - Pointer to buffer to be filled with CPGC capabilities information.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetCpgcInfo) (
  SSA_BIOS_SERVICES_PPI *This,
  MRC_SERVER_CPGC_INFO  *CpgcInfo
  )
{
  struct sysHost                        *host;

  const MRC_SERVER_CPGC_INFO CpgcInfoConst = {
    MAX_NUMBER_DQDB_CACHELINES,  //MaxNumberDqdbCachelines
    MAX_DQDB_INC_RATE_EXP_VALUE,
    MAX_DQDB_INC_RATE_LINEAR_VALUE, //MaxDqdbIncRateLinearVal
    MAX_NUMBER_DQDB_UNISEQS,
    MAX_DQDB_UNISEQ_SEED_VALUE,
    MAX_DQDB_UNISEQ_L_VALUE,
    MAX_DQDB_UNISEQ_M_VALUE,
    MAX_DQDB_UNISEQ_N_VALUE,  //MaxDqdbUniseqNVal
    MAX_DQDB_UNISEQ_SEED_RELOAD_RATE_VAL,  //MaxDqdbUniseqSeedReloadRateVal
    MAX_DQDB_UNISEQ_SEED_SAVE_RATE_VAL,
    MAX_DQDB_INV_DC_SHIFT_RATE_EXP_VAL,
    MAX_NUMBER_CADB_ROWS,
    MAX_NUMBER_CADB_UNISEQS,  //MaxNumberCadbUniseqs
    MAX_NUMBER_CADB_UNISEQ_SEED_VALUE,
    MAX_CADB_UNISEQ_L_VALUE,
    MAX_CADB_UNISEQ_M_VALUE,
    MAX_CADB_UNISEQ_N_VALUE,
    MAX_DUMMY_READ_L_VAL,
    MAX_DUMMY_READ_M_VAL,
    MAX_DUMMY_READ_N_VAL,  //MaxDummyReadNVal
    MAX_START_DELAY_VAL,  //MaxStartDelayVal
    TRUE,                 //BOOLEAN IsLoopCountExponential;
    MAX_LOOP_COUNT_VAL,
    MAX_NUMBER_SUBSEQS,  //MaxNumberSubseqs
    MAX_BURST_LENGHT_EXP_VAL,
    MAX_BURST_LENGHT_LIN_VAL,  //MaxBurstLengthLinearVal
    MAX_INTER_SUBSEQ_WAIT_VAL,
    MAX_OFFSET_ADDR_UPDATE_RATE_VAL,  //MaxOffsetAddrUpdateRateVal
    MAX_ADDRESS_INV_RATE_VAL,  //MaxAddrInvertRateVal
    MAX_RANK_ADDRESS_INC_RATE_EXP_VAL,  //MaxRankAddrIncRateExponentVal
    MAX_RANK_ADDRESS_INC_RATE_LIN_VAL,  //MaxRankAddrIncRateLinearVal
    MIN_RANK_ADDRESS_INC_VAL,  //MinRankAddrIncVal
    MAX_RANK_ADDRESS_INC_VAL,  //MaxRankAddrIncVal
    MAX_BANK_ADDRESS_INC_RATE_EXP_VAL,  //MaxBankAddrIncRateExponentVal
    MAX_BANK_ADDRESS_INC_RATE_LIN_VAL,
    MIN_BANK_ADDRESS_INC_VAL,  //MinBankAddrIncVal
    MAX_BANK_ADDRESS_INC_VAL,
    MAX_ROW_ADDRESS_INC_RATE_EXP_VAL,  //MaxRowAddrIncRateExponentVal
    MAX_ROW_ADDRESS_INC_RATE_LIN_VAL,
    MIN_ROW_ADDRESS_INC_VAL,
    MAX_ROW_ADDRESS_INC_VAL,  //MaxRowAddrIncVal
    MAX_COL_ADDRESS_INC_RATE_EXP_VAL,
    MAX_COL_ADDRESS_INC_RATE_LIN_VAL,
    MIN_COL_ADDRESS_INC_VAL,
    MAX_COL_ADDRESS_INC_VAL,
    MAX_NUMBER_RANK_MAP_ENTRIES,
    MAX_NUMBER_BANK_MAP_ENTRIES,
    MAX_NUMBER_ROW_ADD_SWIZZLE_ENTRIES,  //MaxNumberRowAddrSwizzleEntries
    MAX_ROW_ADD_SWIZZLE_VAL,
    MAX_STOP_ON_NTH_ERR_CNT_VAL,  //MaxStopOnNthErrorCountVal
    MAX_NUMBER_ERROR_COUNTERS
  };

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetCpgcInfo\n"));
#endif

  MemCopy((UINT8 *) CpgcInfo, (UINT8 *) &CpgcInfoConst, sizeof (MRC_SERVER_CPGC_INFO));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetCpgcInfo\n"));
#endif
}

/**
  Function used to get the chip select signal encoding status.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[out]     IsCsEncoded - Pointer to where the chip select signal encoding status will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetCsEncoding) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               *IsCsEncoded
  )
{
  UINT8                   channelInSocket;
  struct sysHost          *host;
  struct channelNvram     (*channelNvList)[MAX_CH];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetCsEncoding (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelNvList = GetChannelNvList(host, Socket);
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  if ((*channelNvList)[channelInSocket].encodedCSMode) {
    *IsCsEncoded = TRUE;
  } else {
    *IsCsEncoded = FALSE;
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetCsEncoding (IsCsEncoded: 0x%x)\n", *IsCsEncoded));
#endif
}

/**
  Function used to get the number of ranks per chip select signal for 3DS memory.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[out]     Channel    - Zero based channel number.
  @param[out]     Dimm       - Zero based DIMM number.
  @param[out]     RankCount  - Pointer to where the rank count per chip select signal will be stored.

  @retval NotAvailable if the system does not support 3DS memory or the given DIMM is not 3DS.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerGet3dsRankPerCs) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Dimm,
  UINT8                 *RankCount
  )
{
  UINT8                              channelInSocket;
  struct sysHost                     *host;
  struct channelNvram                (*channelNvList)[MAX_CH];
  SSA_STATUS                         Status;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGet3dsRankPerCs (Socket:%d, Controller:%d, Channel:%d, Dimm:%d)\n", Socket, Controller, Channel, Dimm));
#endif
  channelNvList = GetChannelNvList(host, Socket);
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  if (((*channelNvList)[channelInSocket].lrDimmPresent) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {  //3DS support only on LRDIMMs for now, RDIMMs support to be added later
    Status = Success;
    CHIP_FUNC_CALL(host, BiosServerGet3dsRankPerCsChip(host, Socket, channelInSocket, Dimm, RankCount));
  } else {
    Status = NotAvailable;  //NotAvailable if the system does not support 3DS memory or the given DIMM is not 3DS.
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGet3dsRankPerCs (RankCount:0x%x)\n", *RankCount));
#endif
  return Status;
}

/**
  Function used to set the CPGC credit-wait configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether CPGC credit-wait feature is enabled.

  @retval NotAvailable if the system does not support the credit-wait feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCreditWaitConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable
  )
{
  struct sysHost                        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCreditWaitConfig (Socket:%d, Controller:%d, Enable:%d)\n", Socket, Controller, Enable));
#endif

  CHIP_FUNC_CALL(host, BiosServerSetCreditWaitConfigChip (host, Socket, Controller, Enable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCreditWaitConfig\n"));
#endif
  return Success;
}

/**
  Function used to set the WPQ-in-order configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Enable     - Specifies whether the WPQ-in-order feature are enabled.

  @retval NotAvailable if the system does not support the WPQ-in-order feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetWpqInOrderConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               Enable
  )
{
  UINT8                              channelInSocket;
  struct sysHost                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetWpqInOrderConfig (Socket:%d, Controller:%d, Channel:%d, Enable:%d)\n", Socket, Controller, Channel, Enable));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetWpqInOrderConfigChip (host, Socket, Controller, channelInSocket, Enable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetWpqInOrderConfig\n"));
#endif

  return Success;
}

/**
  Function used to set the multiple-write-credit configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Enable     - Specifies whether multiple write credits are enabled.

  @retval NotAvailable if the system does not support the multiple-write-credit feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetMultipleWrCreditConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               Enable
  )
{
  UINT8                              channelInSocket;
  struct sysHost                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetMultipleWrCreditConfig (Socket:%d, Controller:%d, Channel:%d, Enable:%d)\n", Socket, Controller, Channel, Enable));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetMultipleWrCreditConfigChip (host, Socket, Controller, channelInSocket, Enable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetMultipleWrCreditConfig\n"));
#endif
  return Success;
}

/**
  Function used to set the CPGC engine lock state.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      State      - Specifies whether CPGC engine is locked (TRUE) or unlocked (FALSE).

  @retval NotAvailable if function is not supported by the BIOS.  Else Failure if the system cannot set the given state.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCpgcLockState) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               State
  )
{
  return NotAvailable;
}

/**
  Function used to write pattern content to the DqDB.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      Pattern             - Pointer to buffer containing the DqDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in number of cachelines.  Each cacheline has 8 UINT64 elements.  For example, if CachelineCount is 1 then the Pattern buffer size is 64 bytes.
  @param[in]      StartCachelineIndex - Start offset on the DqDB.  The sum of the CachelineCount and StartCachelineIndex values must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbPattern) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  CONST UINT64          *Pattern,
  UINT8                 CachelineCount,
  UINT8                 StartCachelineIndex
  )
{
#ifndef MINIBIOS_BUILD
  UINT8                  chunkIdx;  //representing the 8chunks (64bits) for each cacheline entered via "*Pattern"
  UINT8                  channelInSocket;
  UINT8                  cacheLine;
  UINT32                 *WdbLinePtr;
  struct sysHost         *host;
  TWdbLine               WdbLines[MRC_WDB_LINES];
  UINT64_STRUCT          patternToWrite;
  UINT8                  chunkCntPerCacheline = MRC_WDB_LINE_SIZE / (sizeof (UINT32) * 2); //8

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbPattern (Socket:%d, Controller:%d, Channel:%d, CachelineCount:%d, StartCachelineIndex:%d)\n", Socket, Controller, Channel, CachelineCount, StartCachelineIndex));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  if ((CachelineCount + StartCachelineIndex) > MRC_WDB_LINES) {
    return;
  }

  // Fill the WDB buffer with the write content.
  for (cacheLine = 0; cacheLine < CachelineCount; cacheLine++) {
    WdbLinePtr = (UINT32 *) &WdbLines[cacheLine].WdbLine[0];  //Address of base of the array WdbLines saved to WdbLinePrt

    for(chunkIdx = 0;chunkIdx < chunkCntPerCacheline; chunkIdx++) { // will make 8 iterations(512bits) as in each iteration we copy 2 x 4bytes(64bits) of the *pattern into the array WdbLine
      patternToWrite.lo = (UINT32)(Pattern[(cacheLine*chunkCntPerCacheline) + chunkIdx] & 0xFFFFFFFF);  //64bit I/P "Pattern" --> x8 chunks
      patternToWrite.hi = (UINT32)((Pattern[(cacheLine*chunkCntPerCacheline) + chunkIdx] >> 32) & 0xFFFFFFFF);  //64bit I/P "Pattern" --> x8 chunk
      WdbLinePtr[chunkIdx * 2] = patternToWrite.lo;  //representing the array location (16, 32bit chunks) where each "*Pattern" (.lo/.hi) gets saved.. so *2
      WdbLinePtr[(chunkIdx * 2) + 1] = patternToWrite.hi;
    }
  } // cacheLine loop

  //64bytes - We have an array(WdbLine) of size 8bits*64 ; We need an array of 32bits*16, Input(for each Pattern) 8arrays of 64bits    
  //Call WDBFill to save the pattern in WdbLines into the WDB
  WDBFill (host, Socket, channelInSocket, WdbLines, CachelineCount, StartCachelineIndex);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbPattern\n"));
#endif
#endif // MINIBIOS_BUILD
}

/**
  Function used to read pattern content from the DqDB.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      Pattern             - Pointer to buffer to be filled with the DqDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in number of cachelines.  Each cacheline has 8 UINT64 elements.  For example, if CachelineCount is 1 then the Pattern buffer size is 64 bytes.
  @param[in]      StartCachelineIndex - Start offset in the DqDB.  The sum of the CachelineCount and StartCachelineIndex values must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetDqdbPattern) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                     Socket,
  UINT8                     Controller,
  UINT8                     Channel,
  UINT64                    *Pattern,
  UINT8                     CachelineCount,
  UINT8                     StartCachelineIndex
  )
{
// Not available in servers as of now.
}

/**
  Function used to set the WDB start and end indexes as well as the WDB increment rate.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      StartIndex  - Starting index in the DqDB.  The value must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      EndIndex    - Ending index in the DqDB.  The value must be less than or equal to the StartIndex value and less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      IncRateMode - Increment rate mode.
  @param[in]      IncRate     - Increment rate value.  If the IncRateMode is ExponentialDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateExponentVal from the GetCpgcInfo() function.  If the IncRateMode is LinearDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateLinearVal from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbIndexes) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 StartIndex,
  UINT8                 EndIndex,
  DQDB_INC_RATE_MODE    IncRateMode,
  UINT8                 IncRate
  )
{
  UINT8                                 channelInSocket;
  struct sysHost                        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbIndexes (Socket:%d, Controller:%d, Channel:%d, StartIndex:%d, EndIndex:%d, IncRateMode:%d, IncRate:%d)\n", Socket, Controller, Channel, StartIndex, EndIndex, IncRateMode, IncRate));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbIndexesChip (host, Socket, Controller, channelInSocket, StartIndex, EndIndex, IncRateMode, IncRate));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbIndexes\n"));
#endif
}

/**
  Function used to set the WDB start and end indexes as well as the WDB increment rate.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      EccDataSource - Specifies the ECC signal data source.

  @retval NotAvailable if the system does not support the ECC Data Source feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetDqdbEccDataSource) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  ECC_DATA_SOURCE       EccDataSource
  )
{
  UINT8                                 channelInSocket;
  struct sysHost                        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbEccDataSource (Socket:%d, Controller:%d, Channel:%d, EccDataSource:%d)\n", Socket, Controller, Channel, EccDataSource));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbEccDataSourceChip (host, Socket, channelInSocket, EccDataSource));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbEccDataSource\n"));
#endif
  return Success;
}

/**
  Function used to set the DqDB unisequencer mode.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Mode       - DqDB unisequencer mode.

  @retval NotAvailable if the given Mode value is not available on the given Uniseq.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetDqdbUniseqMode) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  DQDB_UNISEQ_MODE      Mode
  )
{
  UINT8                                         channelInSocket;
  struct sysHost                                *host;

  if ((Uniseq != 0) && (Mode == LmnDqdbUniseqMode)) {
    return NotAvailable;
  }

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbUniseqMode (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, Mode:%d)\n", Socket, Controller, Channel, Uniseq, Mode));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbUniseqModeChip (host, Socket, channelInSocket, Uniseq, Mode));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbUniseqMode\n"));
#endif
  return Success;
}

/**
  Function used to set the DqDB write unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Seed       - DqDB write unisequencer seed value.  The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbUniseqWrSeed) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT32                Seed
  )
{
  UINT8                 channelInSocket;
  struct sysHost        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbUniseqWrSeed (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, Seed:%x)\n", Socket, Controller, Channel, Uniseq, Seed));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbUniseqWrSeedChip (host, Socket, channelInSocket, Uniseq, Seed));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbUniseqWrSeed\n"));
#endif
}

/**
  Function used to set the DqDB read unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Seed       - DqDB write unisequencer seed value.  The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbUniseqRdSeed) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT32                Seed
  )
{
  UINT8                 channelInSocket;
  struct sysHost        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbUniseqRdSeed (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, Seed:%x)\n", Socket, Controller, Channel, Uniseq, Seed));
#endif
  channelInSocket = SSAGetChannelInSocket (Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbUniseqRdSeedChip (host, Socket, channelInSocket, Uniseq, Seed));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbUniseqRdSeed\n"));
#endif
}

/**
  Function used to get the DqDB write unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[out]     Seed       - Pointer to where the DqDB write unisequencer seed value will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetDqdbUniseqWrSeed) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT32                *Seed
  )
{
  UINT8                 channelInSocket;
  struct sysHost        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetDqdbWrUniseqSeed (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d)\n", Socket, Controller, Channel, Uniseq));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetDqdbUniseqWrSeedChip (host, Socket, channelInSocket, Uniseq, Seed));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetDqdbWrUniseqSeed (Seed:0x%x)\n", *Seed));
#endif
}

/**
  Function used to get the DqDB read unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[out]     Seed       - Pointer to where the DqDB read unisequencer seed value will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetDqdbUniseqRdSeed) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT32                *Seed
  )
{
  UINT8                 channelInSocket;
  struct sysHost        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetDqdbRdUniseqSeed (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d)\n", Socket, Controller, Channel, Uniseq));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetDqdbUniseqRdSeedChip (host, Socket, channelInSocket, Uniseq, Seed));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetDqdbRdUniseqSeed (Seed:0x%x)\n", *Seed));
#endif
}

/**
  Function used to set the DqDB unisequencer L/M/N values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      Uniseq          - DqDB unisequencer mode.
  @param[in]      L               - DqDB unisequencer L counter value.  The value must be less than or equal to the MaxDqdbUniseqLVal value from the GetCpgcInfo() function.
  @param[in]      M               - DqDB unisequencer M counter value.  The value must be less than or equal to the MaxDqdbUniseqMVal value from the GetCpgcInfo() function.
  @param[in]      N               - DqDB unisequencer N counter value.  The value must be less than or equal to the MaxDqdbUniseqNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - DqDB unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables the Frequency Sweep feature.

  @retval NotAvailable if the LMN mode is not available on the given Uniseq.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetDqdbUniseqLmn) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  UINT8                                      channelInSocket;
  struct sysHost                             *host;

  if (Uniseq != 0) {
    return NotAvailable;
  }

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbUniseqLmn (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, L:%d, M:%d, N:%d, LDataSel:%d, EnableFreqSweep:%d)\n", Socket, Controller, Channel, Uniseq, L, M, N, LDataSel, EnableFreqSweep));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbUniseqLmnChip (host, Socket, channelInSocket, Uniseq, L, M, N, LDataSel, EnableFreqSweep));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbUniseqLmn\n"));
#endif
  return Success;
}

/**
  Function used to set the DqDB unisequencer LFSR seed reload and save rate values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      ReloadRate      - DqDB unisequencer LFSR seed reload rate.  The value must be less than or equal to the MaxDqdbUniseqSeedReloadRateVal value from the GetCpgcInfo() function.
  @param[in]      SaveRate        - DqDB unisequencer LFSR seed save rate.  The value must be less than or equal to the MaxDqdbUniseqSeedSaveRateVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbUniseqLfsrConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 ReloadRate,
  UINT8                 SaveRate
  )
{
  UINT8                                           channelInSocket;
  struct sysHost                                  *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbUniseqLfsrConfig (Socket:%d, Controller:%d, Channel:%d, ReloadRate:%d, SaveRate:%d)\n", Socket, Controller, Channel, ReloadRate, SaveRate));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbUniseqLfsrConfigChip (host, Socket, channelInSocket, ReloadRate, SaveRate));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbUniseqLfsrConfig\n"));
#endif
}

/**
  Function used to set the DqDB Invert/DC configuration.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket            - Zero based CPU socket number.
  @param[in]      Controller        - Zero based controller number.
  @param[in]      Channel           - Zero based channel number.
  @param[in]      LaneMasks         - Pointer to array of lane bitmasks.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = participate in the Invert/DC feature; bit value 0 = do not participate in the Invert/DC feature.  If all lane bitmasks are 0 then the feature is disabled.
  @param[in]      Mode              - Invert/DC mode.
  @param[in]      DcPolarity        - DC polarity (when operating in the DC mode). 0 = DC low. 1 = DC high.
  @param[in]      ShiftRateExponent - Exponent of the bitmask shift rate.  The value must be less than or equal to the MaxDqdbInvertDcShiftRateExponentVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbInvertDcConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  CONST UINT8           *LaneMasks,
  INVERT_DC_MODE        Mode,
  UINT8                 DcPolarity,
  UINT8                 ShiftRateExponent
  )
{
  UINT8                             i;
  UINT8                             channelInSocket;
  UINT8                             arrayElements = 0;
  UINT8                             EccMask = 0;
  UINT64_STRUCT                     DqMask;
  struct sysHost                    *host;

  DqMask.lo = 0;
  DqMask.hi = 0;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbInvertDcConfig (Socket:%d, Controller:%d, Channel:%d, Mode:%d, DcPolarity:%d, ShiftRateExponent:%d)\n", Socket, Controller, Channel, Mode, DcPolarity, ShiftRateExponent));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  //Getting the number of Array Elements
  if (host->nvram.mem.eccEn) { //BusWidth = 72(with Ecc), Array elements = 72/8
   arrayElements = 9;
  } else {
   arrayElements = 8;  //BusWidth = 64, Array elements = 64/8
  }

  //Separating the Array Elements into DQ Masks(lo,hi) and a ECC Mask
  for (i = 0; i < arrayElements; i++) {
    if (i < 4) {  //Moving each set of 8bits << 8 .w.r.t. "i", 0's that follow are OR'ed with the previous 8bits value
      DqMask.lo = DqMask.lo | (((UINT32) *(LaneMasks + i)) << i*8);
    } else if ((i >=4) && (i < 8)) {
      DqMask.hi = DqMask.hi | (((UINT32) *(LaneMasks + i)) << (i-4)*8);
    } else if (i == 8) {
      EccMask = *(LaneMasks + i);
    }
  }

  CHIP_FUNC_CALL(host, BiosServerSetDqdbInvertDcConfigChip (host, Socket, channelInSocket, Mode, DcPolarity, ShiftRateExponent, DqMask.lo, DqMask.hi, EccMask));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbInvertDcConfig\n"));
#endif
}

/**
  This function sets the given channels CADB pattern generation mode to the given value.  The default value is NormalCadbMode.

  @param[in, out] This                  - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                - Zero based CPU socket number.
  @param[in]      Controller            - Zero based controller number.
  @param[in]      Channel               - Zero based channel number.
  @param[in]      EnableAlwaysOn        - Specifies whether CADB patterns will be driven out on all command and address pins every Dclk while a test is running.
  @param[in]      EnableOnDeselect      - Specifies whether CADB patterns will be driven out during deselect packets.
  @param[in]      EnableParityNTiming   - Specifies parity timing.  If this parameter is TRUE, CADB parity will be driven on same clock as CMD/Address (i.e., N timing for UDIMM DDR4 only).  If this parameter is FALSE, CADB parity will be driven on the clock after CMD/Address (i.e,. N+1 timing for all other devices).
  @param[in]      EnableOnePassAlwaysOn - This parameter only applies if the EnableAlwaysOn parameter is TRUE.  If this parameter is TRUE, the test stops after all 16 entries have been issued out of CADB.
  @param[in]      EnablePdaDoubleLength - If this parameter is TRUE, any PBA (Per Buffer Addressable) /PDA (Per DRAM addressable) writes issued from CADB will be double length (i.e. if lockstep is disabled, 16 UI instead of 8 UI or if lockstep is enabled, 8 UI instead of 4 UI).  This is to allow more setup up time for PBA accesses during training.  If this parameter is FALSE, PBA/PDA writes will look exactly the same as normal writes.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetCadbConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnableAlwaysOn,
  BOOLEAN               EnableOnDeselect,
  BOOLEAN               EnableParityNTiming,
  BOOLEAN               EnableOnePassAlwaysOn,
  BOOLEAN               EnablePdaDoubleLength
  )
{
  UINT8                                         channelInSocket;
  struct sysHost                                *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCadbConfig (Socket:%d, Controller:%d, Channel:%d, EnableAlwaysOn:%d, EnableOnDeselect:%d, EnableParityNTiming:%d, EnableOnePassAlwaysOn:%d, EnablePdaDoubleLength:%d)\n",  Socket, Controller, Channel, EnableAlwaysOn, EnableOnDeselect, EnableParityNTiming, EnableOnePassAlwaysOn, EnablePdaDoubleLength));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCadbConfigChip (host, Socket, channelInSocket, EnableAlwaysOn, EnableOnDeselect, EnableParityNTiming, EnableOnePassAlwaysOn, EnablePdaDoubleLength));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCadbConfig\n"));
#endif
}

/**
  Function used to write to the CADB.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      Pattern       - Pointer to buffer containing the CADB pattern.
  @param[in]      RowCount      - Size of the buffer pattern in term of the count of rows.  Each row has 2 UINT32 elements.
  @param[in]      StartRowIndex - Start offset on the CADB.  The sum of the RowCount and StartRowIndex values must be less than or equal to the MaxNumberCadbRows value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetCadbPattern) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  CONST UINT64          *Pattern,
  UINT8                 RowCount,
  UINT8                 StartRowIndex
  )
{
#ifndef MINIBIOS_BUILD
  UINT8                          i;
  UINT8                          channelInSocket;
  struct sysHost                 *host;
  UINT64_STRUCT                  patternToWrite;
  PatCadbProg0                   progCadb0[CADB_LINES];
  PatCadbProg1                   progCadb1[CADB_LINES];

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCadbPattern (Socket:%d, Controller:%d, Channel:%d, RowCount:%d, StartRowIndex:%d)\n", Socket, Controller, Channel, RowCount, StartRowIndex));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  for (i=0; i < RowCount; i++) {
    patternToWrite.lo = (UINT32)(Pattern[i] & 0xFFFFFFFF);
    patternToWrite.hi = (UINT32)((Pattern[i] >> 32) & 0xFFFFFFFF);

    progCadb0[i].address = (patternToWrite.lo & 0x3FFFF);  //Bits[17:0]
    progCadb0[i].cid = (UINT8)((patternToWrite.lo >> 20) & 0x7);  //Bits[22:20]
    progCadb0[i].bank = (UINT8)((patternToWrite.lo >> 24) & 0xF);  //Bits[27:24]
    progCadb0[i].pdatag = (UINT8)((patternToWrite.lo >> 30) & 0x3);  //Bits[31:30]

    progCadb1[i].cs = (UINT16) (patternToWrite.hi & 0x3FF);  //Bits[9:0]
    progCadb1[i].control = (UINT8)((patternToWrite.hi >> 12) & 0xF);  //Bits[15:12]
    progCadb1[i].odt = (UINT8)((patternToWrite.hi >> 16) & 0x3F);  //Bits[21:16]
    progCadb1[i].cke = (UINT8)((patternToWrite.hi >> 24) & 0x3F);  //Bits[29:24]
    progCadb1[i].par = (UINT8)((patternToWrite.hi >> 31) & 0x1);  //Bits[31:31]
  }

  //In ProgramCADB write_pointer will be set to StartRowIndex
  CHIP_FUNC_CALL(host, ProgramCADB (host, Socket, channelInSocket, StartRowIndex, StartRowIndex + RowCount, progCadb0, progCadb1));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCadbPattern\n"));
#endif
#endif //MINIBIOS_BUILD
}

/**
  Function used to set the CADB unisequencer modes.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Uniseq       - Zero based CADB unisequencer number.
  @param[in]      DeselectMode - CADB deselect cycle unisequencer mode.  The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval NotAvailable if the given DeselectMode or SelectMode value is not available on the given Uniseq.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCadbUniseqMode) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  CADB_UNISEQ_MODE      DeselectMode
  )
{
  UINT8                                            channelInSocket;
  struct sysHost                                   *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCadbUniseqMode (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, DeselectMode:%d)\n", Socket, Controller, Channel, Uniseq, DeselectMode));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCadbUniseqModeChip (host, Socket, channelInSocket, Uniseq, DeselectMode));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCadbUniseqMode\n"));
#endif
  return Success;
}

/**
  Function used to set the CADB unisequencer seed value.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Uniseq          - Zero based CADB unisequencer number.
  @param[in]      DeselectSeed - CADB deselect cycle unisequencer seed value.The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetCadbUniseqSeed) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT32                DeselectSeed
  )
{
  UINT8                    channelInSocket;
  struct sysHost           *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCadbUniseqSeed (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, DeselectSeed:%x)\n", Socket, Controller, Channel, Uniseq, DeselectSeed));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCadbUniseqSeedChip (host, Socket, channelInSocket, Uniseq, DeselectSeed));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCadbUniseqSeed\n"));
#endif
}

/**
  Function used to set the CADB unisequencer L/M/N values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      Uniseq          - Zero based CADB unisequencer number.
  @param[in]      L               - CADB unisequencer L counter value.  The value must be less than or equal to the MaxCadbUniseqLVal value from the GetCpgcInfo() function.
  @param[in]      M               - CADB unisequencer M counter value.  The value must be less than or equal to the MaxCadbUniseqMVal value from the GetCpgcInfo() function.
  @param[in]      N               - CADB unisequencer N counter value.  The value must be less than or equal to the MaxCadbUniseqNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - CADB unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables LMN frequency sweep feature.

  @retval NotAvailable if the LMN mode is not available on the given Uniseq.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCadbUniseqLmn) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Uniseq,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  UINT8                                         channelInSocket;
  struct sysHost                                *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCadbUniseqLmn (Socket:%d, Controller:%d, Channel:%d, Uniseq:%d, L:%d, M:%d, N:%d, LDataSel:%d, EnableFreqSweep:%d)\n", Socket, Controller, Channel, Uniseq, L, M, N, LDataSel, EnableFreqSweep));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCadbUniseqLmnChip (host, Socket, channelInSocket, Uniseq, L, M, N, LDataSel, EnableFreqSweep));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetCadbUniseqLmn\n"));
#endif
  return Success;
}

/**
  Function used to set the Dummy Read LMN values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      L               - Dummy Read unisequencer L counter value.  The value must be less than or equal to the MaxDummyReadLVal value from the GetCpgcInfo() function.
  @param[in]      M               - Dummy Read unisequencer M counter value.  The value must be less than or equal to the MaxDummyReadMVal value from the GetCpgcInfo() function.
  @param[in]      N               - Dummy Read unisequencer N counter value.  The value must be less than or equal to the MaxDummyReadNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - Dummy Read unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables LMN frequency sweep feature.

  @retval NotAvailable if the system does not support the Dummy Read feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetDummyReadLmn) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  UINT8                                            channelInSocket;
  struct sysHost                                    *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDummyReadLmn (Socket:%d, Controller:%d, Channel:%d, L:%d, M:%d, N:%d, LDataSel:%d, EnableFreqSweep:%d)\n", Socket, Controller, Channel, L, M, N, LDataSel, EnableFreqSweep));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDummyReadLmnChip (host, Socket, Controller, channelInSocket, L, M, N, LDataSel, EnableFreqSweep));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDummyReadLmn\n"));
#endif
  return Success;
}

/**
  Function used to set the Dummy Read bank mask.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      BankMask        - Dummy Read bank mask.

  @retval NotAvailable if the system does not support the Dummy Read feature.  Else UnsupportedValue if the given BankMask value is out of range.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetDummyReadBankMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT16                BankMask
  )
{
  UINT8                                          channelInSocket;
  struct sysHost                                 *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDummyReadBankMask (Socket:%d, Controller:%d, Channel:%d, BankMask:%x)\n", Socket, Controller, Channel, BankMask));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  //NotAvailable if the system does not support the Dummy Read feature.
  CHIP_FUNC_CALL(host, BiosServerSetDummyReadBankMaskChip (host, Socket, Controller, channelInSocket, BankMask));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDummyReadBankMask\n"));
#endif
  return Success;
}

/**
  Function used to set the subsequence configuration.

  @param[in, out] This                              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                            - Zero based CPU socket number.
  @param[in]      Controller                        - Zero based controller number.
  @param[in]      Channel                           - Zero based channel number.
  @param[in]      Subseq                            - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      BurstLengthMode,                  - Burst length mode.
  @param[in]      BurstLength                       - Burst length in cachelines.  If BurstLengthMode is ExponentialBurstLengthMode then the Burstlength value is an exponent and the value must be less than or equal to the MaxBurstLengthExponentVal value from the in GetCpgcInfo() function.  If BurstLengthMode is LinearBurstLengthMode then the Burstlength value is linear and the value must be less than or equal to the MaxBurstLengthLinearVal value from the in GetCpgcInfo() function.
  @param[in]      InterSubseqWait                   - Inter-subsequence wait in dclks.  The value must be less than or equal to the MaxInterSubseqWaitVal value from the GetCpgcinfo() function.
  @param[in]      SubseqType                        - Subsequence type.
  @param[in]      EnableSaveCurrentBaseAddrToStart  - Specifies whether the current base address is saved to the start address with this subsequence.
  @param[in]      EnableResetCurrentBaseAddrToStart - Specifies whether the current base address is reset to the start address with this subsequence.
  @param[in]      EnableAddrInversion               - Specifies whether address inversion enabled for this subsequence.
  @param[in]      EnableDataInversion               - Specifies whether data inversion enabled for this subsequence.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSubseqConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Subseq,
  BURST_LENGTH_MODE     BurstLengthMode,
  UINT32                BurstLength,
  UINT32                InterSubseqWait,
  SUBSEQ_TYPE           SubseqType,
  BOOLEAN               EnableSaveCurrentBaseAddrToStart,
  BOOLEAN               EnableResetCurrentBaseAddrToStart,
  BOOLEAN               EnableAddrInversion,
  BOOLEAN               EnableDataInversion
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSubseqConfig (Socket:%d, Controller:%d, Channel:%d, Subseq:%d, BurstLengthMode:%d, BurstLength:%d)\n", Socket, Controller, Channel, Subseq, BurstLengthMode, BurstLength));
  rcPrintf ((host,"Start: BiosServerSetSubseqConfig (InterSubseqWait:%d, SubseqType:%d, EnableSaveCurrentBaseAddrToStart:%d, EnableResetCurrentBaseAddrToStart:%d, EnableAddrInversion:%d, EnableDataInversion:%d)\n", InterSubseqWait, SubseqType, EnableSaveCurrentBaseAddrToStart, EnableResetCurrentBaseAddrToStart, EnableAddrInversion, EnableDataInversion));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSubseqConfigChip (host, Socket, Controller, channelInSocket, Subseq, BurstLengthMode, BurstLength, InterSubseqWait, SubseqType, EnableSaveCurrentBaseAddrToStart, EnableResetCurrentBaseAddrToStart, EnableAddrInversion, EnableDataInversion));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSubseqConfig\n"));
#endif
}

/**
  Function used to set the subsequence configuration.

  @param[in, out] This                 - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket               - Zero based CPU socket number.
  @param[in]      Controller           - Zero based controller number.
  @param[in]      Channel              - Zero based channel number.
  @param[in]      Subseq               - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      EnableBaseInvertData - Specifies whether base inversion of the data is enabled.
  @param[in]      EnableRowIncrement   - Specifies whether row increment is enabled.
  @param[in]      RowIncrementOrder    - Specifies row increment order.  The value must be 0 or 1.
  @param[in]      EnableColIncrement   - Specifies whether column increment is enabled.
  @param[in]      ColIncrementOrder    - Specifies column increment order.  The value must be 0 or 1.
  @param[in]      BaseSubseqType       - Every time the BaseSubseqRepeatRate is reached a Read or Write is performed for one command based on the following fields: BaseSubseqType indicates whether a read (BaseReadSubseqType) or write (BaseWriteSubseqType) operation will occur at the current base address.  EnableBaseInvertData indicates whether the read or write (BaseSubseqType) operation at the current base address is inverted (EnableBaseInvertData = TRUE) or not (EnableBaseInvertData = FALSE).
  @param[in]      BaseSubseqRepeatRate - Specifies how often the BaseSubseqType operation occurs for 1 cacheline.  0 = No BaseSubseqType cacheline operations will occur during the Offset_Read or Offset_Write.  1 = Reserved value.  2 = Repeat the BaseSubseqType cacheline operation after every offset cacheline operation.  ...  31 = Repeat the BaseSubseqType cacheline operation after 30 offset cacheline operations.
  @param[in]      OffsetAddrUpdateRate - Specifies the rate that the Current Offset Address is updated.  The value must be less than or equal to the MaxOffsetAddrUpdateRateVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSubseqOffsetConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Subseq,
  BOOLEAN               EnableBaseInvertData,
  BOOLEAN               EnableRowIncrement,
  UINT8                 RowIncrementOrder,
  BOOLEAN               EnableColIncrement,
  UINT8                 ColIncrementOrder,
  SUBSEQ_TYPE           BaseSubseqType,
  UINT32                BaseSubseqRepeatRate,
  UINT32                OffsetAddrUpdateRate
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSubseqOffsetConfig (Socket:%d, Controller:%d, Channel:%d, Subseq:%d, EnableBaseInvertData:%d, EnableRowIncrement:%d)\n", Socket, Controller, Channel, Subseq, EnableBaseInvertData, EnableRowIncrement));
  rcPrintf ((host,"Start: BiosServerSetSubseqOffsetConfig (RowIncrementOrder:%d, EnableColIncrement:%d, ColIncrementOrder:%d, BaseSubseqType:%d, BaseSubseqRepeatRate:%d, OffsetAddrUpdateRate:%d)\n", RowIncrementOrder, EnableColIncrement, ColIncrementOrder, BaseSubseqType, BaseSubseqRepeatRate, OffsetAddrUpdateRate));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSubseqOffsetConfigChip (host, Socket, Controller, channelInSocket, Subseq, EnableBaseInvertData, EnableRowIncrement, RowIncrementOrder, EnableColIncrement, ColIncrementOrder, BaseSubseqType, BaseSubseqRepeatRate, OffsetAddrUpdateRate));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSubseqOffsetConfig\n"));
#endif
}

/**
  Function used to set the sequence configuration.

  @param[in, out] This                      - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                    - Zero based CPU socket number.
  @param[in]      Controller                - Zero based controller number.
  @param[in]      Channel                   - Zero based channel number.
  @param[in]      StartDelay                - Specifies the delay period (in dclks) between the start of the test and the actual entry to Loopback.Pattern where the first SubSequence is executed immediately.  The value must be less than or equal to the MaxStartDelayVal value from the GetCpgcInfo() function.
  @param[in]      SubseqStartIndex          - Specifies the initial SubSequence pointer where a Sequence first enters Loopback.Pattern (start of test).  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      SubseqEndIndex            - When the subsequence pointed to by the SubseqEndIndex is completed the loop count is incremented by 1 and the current subsequence index is reset to the SubseqStartIndex.  The SubseqEndIndex must be greater than or equal to the SubseqStartIndex value.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      LoopCount                 - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.
  @param[in]      InitMode                  - Initialization mode.
  @param[in]      EnableGlobalControl       - Specifies whether global control is enabled for the given channel.
  @param[in]      EnableConstantWriteStrobe - Specifies whether constant write strobe is enabled.
  @param[in]      EnableDummyReads          - Specifies whether dummy reads are enabled.
  @param[in]      AddrUpdateRateMode        - Address update rate mode.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT32                StartDelay,
  UINT8                 SubseqStartIndex,
  UINT8                 SubseqEndIndex,
  UINT32                LoopCount,
  CPGC_INIT_MODE        InitMode,
  BOOLEAN               EnableGlobalControl,
  BOOLEAN               EnableConstantWriteStrobe,
  BOOLEAN               EnableDummyReads,
  ADDR_UPDATE_RATE_MODE AddrUpdateRateMode
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqConfig (Socket:%d, Controller:%d, Channel:%d, StartDelay:%d, SubseqStartIndex:%d, SubseqEndIndex:%d)\n", Socket, Controller, Channel, StartDelay, SubseqStartIndex, SubseqEndIndex));
  rcPrintf ((host,"Start: BiosServerSetSeqConfig (LoopCount:%d, InitMode:%d, EnableGlobalControl:%d, EnableConstantWriteStrobe:%d, EnableDummyReads:%d, AddrUpdateRateMode:%d)\n", LoopCount, InitMode, EnableGlobalControl, EnableConstantWriteStrobe, EnableDummyReads, AddrUpdateRateMode));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqConfigChip (host, Socket, Controller, channelInSocket, StartDelay, SubseqStartIndex, SubseqEndIndex, LoopCount, InitMode, EnableGlobalControl, EnableConstantWriteStrobe, EnableDummyReads, AddrUpdateRateMode));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqConfig\n"));
#endif
}

/**
  Function used to set the loop count.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      LoopCount  - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetLoopCount) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT32                LoopCount
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetLoopCount (Socket:%d, Controller:%d, Channel:%d, LoopCount:%d)\n", Socket, Controller, Channel, LoopCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetLoopCountChip (host, Socket, Controller, channelInSocket, LoopCount));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetLoopCount\n"));
#endif
}

/**
  Function used to set the sequence address configuration.

  @param[in, out] This                 - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket               - Zero based CPU socket number.
  @param[in]      Controller           - Zero based controller number.
  @param[in]      Channel              - Zero based channel number.
  @param[in]      EnableRankWrapCarry  - If a base rank address wrap occurs  and EnableRankWrapCarry is set to TRUE then a carry increment or decrement from the rank address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableBankWrapCarry  - If a base bank address wrap occurs  and EnableBankWrapCarry is set to TRUE then a carry increment or decrement from the bank address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableRowWrapCarry   - If a base row address wrap occurs and EnableRowWrapCarry is set to TRUE then a carry increment or decrement from the row address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableColWrapCarry   - If a base column address wrap occurs  and EnableColWrapCarry is set to TRUE then a carry increment or decrement from the column address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableRankAddrInvert - Setting to TRUE will cause the current rank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableBankAddrInvert - Setting to TRUE will cause the current bank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableRowAddrInvert  - Setting to TRUE will cause the current row address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableColAddrInvert  - Setting to TRUE will cause the current column address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      AddrInvertRate       - Base address inversion rate.  The SetSeqConfig() function’s AddrUpdateRateMode parameter specifies whether the base address inversion logic is based on number of cachelines written and read or on the number of sequences performed.  If AddrUpdateRateMode from the SetSeqConfig() function is CachelineAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate read and write cacheline transactions.  If AddrUpdateRateMode from the SetSeqConfig() function is LoopCountAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate loops through the sequence.  The value must be less than or equal to the MaxAddrInvertRateVal value from the GetCpgcInfo() function.
  @param[in]      RankAddrOrder        - Specifies the relative ordering of the rank address update logic in the nested for loop in relation to the bank, row, and column address update logic.  Any address fields set to the same order that roll over will cause a distinct carry of +1 or -1 to the next higher order address field (multiple simultaneous carries will cause only one increment/decrement).  All fields can be arbitrarily placed in any order as long as all address order fields are set in a continuous order starting from 0 and may not skip over an order number.  Example: setting 0,1,2,1 in the 4 fields (Col,Row,Bank,Rank) is legal; setting 0,2,2,3 is illegal (not continuous).  The value must be less than or equal to 3.
  @param[in]      BankAddrOrder        - Specifies the relative ordering of the bank address update logic in the nested for loop in relation to the rank, row, and column address update logic.  The value must be less than or equal to 3.
  @param[in]      RowAddrOrder         - Specifies the relative ordering of the row address update logic in the nested for loop in relation to the rank, bank, and column address update logic.  The value must be less than or equal to 3.
  @param[in]      ColAddrOrder         - Specifies the relative ordering of the column address update logic in the nested for loop in relation to the rank, bank, and row address update logic.  The value must be less than or equal to 3.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqAddrConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnableRankWrapCarry,
  BOOLEAN               EnableBankWrapCarry,
  BOOLEAN               EnableRowWrapCarry,
  BOOLEAN               EnableColWrapCarry,
  BOOLEAN               EnableRankAddrInvert,
  BOOLEAN               EnableBankAddrInvert,
  BOOLEAN               EnableRowAddrInvert,
  BOOLEAN               EnableColAddrInvert,
  UINT8                 AddrInvertRate,
  UINT8                 RankAddrOrder,
  UINT8                 BankAddrOrder,
  UINT8                 RowAddrOrder,
  UINT8                 ColAddrOrder
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqAddrConfig (Socket:%d, Controller:%d, Channel:%d, EnableRankWrapCarry:%d, EnableBankWrapCarry:%d, EnableRowWrapCarry:%d)\n", Socket, Controller, Channel, EnableRankWrapCarry, EnableBankWrapCarry, EnableRowWrapCarry));
  rcPrintf ((host,"Start: BiosServerSetSeqAddrConfig (EnableColWrapCarry:%d, EnableRankAddrInvert:%d, EnableBankAddrInvert:%d, EnableRowAddrInvert:%d, EnableColAddrInvert:%d)\n", EnableColWrapCarry, EnableRankAddrInvert, EnableBankAddrInvert, EnableRowAddrInvert, EnableColAddrInvert));
  rcPrintf ((host,"Start: BiosServerSetSeqAddrConfig (AddrInvertRate:%d, RankAddrOrder:%d, BankAddrOrder:%d, RowAddrOrder:%d, ColAddrOrder:%d)\n", AddrInvertRate, RankAddrOrder, BankAddrOrder, RowAddrOrder, ColAddrOrder));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqAddrConfigChip (host, Socket, Controller, channelInSocket, EnableRankWrapCarry, EnableBankWrapCarry, EnableRowWrapCarry, EnableColWrapCarry, EnableRankAddrInvert, EnableBankAddrInvert, EnableRowAddrInvert, EnableColAddrInvert, AddrInvertRate, RankAddrOrder, BankAddrOrder, RowAddrOrder, ColAddrOrder));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqAddrConfig\n"));
#endif
}

/**
  Function used to set the sequence start address.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      RankAddr   - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      BankAddr   - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      RowAddr    - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      ColAddr    - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqStartAddr) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 RankAddr,
  UINT8                 BankAddr,
  UINT32                RowAddr,
  UINT32                ColAddr
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqStartAddr (Socket:%d, Controller:%d, Channel:%d, RankAddr:%d, BankAddr:%d, RowAddr:%d, ColAddr:%d)\n", Socket, Controller, Channel, RankAddr, BankAddr, RowAddr, ColAddr));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqStartAddrChip (host, Socket, Controller, channelInSocket, RankAddr, BankAddr, RowAddr, ColAddr));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqStartAddr\n"));
#endif
}

/**
  Function used to set the sequence wrap address.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      RankAddr   - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      BankAddr   - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      RowAddr    - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      ColAddr    - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqWrapAddr) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 RankAddr,
  UINT8                 BankAddr,
  UINT32                RowAddr,
  UINT32                ColAddr
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqWrapAddr (Socket:%d, Controller:%d, Channel:%d, RankAddr:%d, BankAddr:%d, RowAddr:%d, ColAddr:%d)\n", Socket, Controller, Channel, RankAddr, BankAddr, RowAddr, ColAddr));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqWrapAddrChip (host, Socket, Controller, channelInSocket, RankAddr, BankAddr, RowAddr, ColAddr));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqWrapAddr\n"));
#endif
}

/**
  Function used to set the sequence address increment.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      RankAddrIncMode - Rank address increment rate mode.
  @param[in]      RankAddrIncRate - Rank address increment rate.  In linear mode (see RankAddrIncMode), RankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RankAddrIncVal.  In exponential mode, 2^RankAddrIncRate defines how many times a specific rank address is repeated before adding the RankAddrIncVal.  If RankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateExponentVal from the GetCpgcInfo() function.  If RankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      RankAddrIncVal  - Rank start address increment value.  When dictated by RankAddrIncRate the current rank address will be modified by the RankAddrIncVal (which is a 2's compliment field).  A value of 0 means the rank address increment is effectively disabled since no offset will be applied.  Disabling the rank address increment by setting RankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRankAddrIncVal from the GetCpgcInfo() function.
  @param[in]      BankAddrIncMode - Bank address increment rate mode.
  @param[in]      BankAddrIncRate - Bank address increment rate.  In linear mode (see BankAddrIncMode), BankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the BankAddrIncVal.  In exponential mode, 2^BankAddrIncRate defines how many times a specific rank address is repeated before adding the BankAddrIncVal.  If BankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateExponentVal from the GetCpgcInfo() function.  If BankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      BankAddrIncVal  - Bank start address increment value.  When dictated by BankAddrIncRate the current bank address will be modified by the BankAddrIncVal (which is a 2's compliment field).  A value of 0 means the bank address increment is effectively disabled since no offset will be applied.  Disabling the bank address increment by setting BankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinBankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxBankAddrIncVal from the GetCpgcInfo() function.
  @param[in]      RowAddrIncMode  - Row address increment rate mode.
  @param[in]      RowAddrIncRate  - Row address increment rate.  In linear mode (see RowAddrIncMode), RowAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RowAddrIncVal.  In exponential mode, 2^RowAddrIncRate defines how many times a specific rank address is repeated before adding the RowAddrIncVal.  If RowAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateExponentVal from the GetCpgcInfo() function.  If RowAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      RowAddrIncVal   - Row start address increment value.  When dictated by RowAddrIncRate the current row address will be modified by the RowAddrIncVal (which is a 2's compliment field.  A value of 0 means the row address increment is effectively disabled since no offset will be applied.  Disabling the row address increment by setting RowAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRowAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRowAddrIncVal from the GetCpgcInfo() function.
  @param[in]      ColAddrIncMode  - Column address increment rate mode.
  @param[in]      ColAddrIncRate  - Column address increment rate.  If ColAddrIncMode is ExponentialAddrIncMode then 2^ColAddrIncRate defines how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is LinearAddrIncMode then ColAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateExponentVal from the GetCpgcInfo() function.  If ColAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      ColAddrIncVal   - Column start address increment value.  When dictated by ColAddrIncRate the current column address will be modified by the ColAddrIncVal (which is a 2's compliment field).  A value of 0 means the column address increment is effectively disabled since no offset will be applied.  Disabling the column address increment by setting column address increment to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinColAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxColAddrIncVal from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqAddrInc) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  ADDR_INC_MODE         RankAddrIncMode,
  UINT32                RankAddrIncRate,
  INT8                  RankAddrIncVal,
  ADDR_INC_MODE         BankAddrIncMode,
  UINT32                BankAddrIncRate,
  INT8                  BankAddrIncVal,
  ADDR_INC_MODE         RowAddrIncMode,
  UINT32                RowAddrIncRate,
  INT16                 RowAddrIncVal,
  ADDR_INC_MODE         ColAddrIncMode,
  UINT32                ColAddrIncRate,
  INT16                 ColAddrIncVal
  )
{
 UINT8                               channelInSocket;
 struct sysHost                      *host;

 host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqAddrInc (Socket:%d, Controller:%d, Channel:%d, RankAddrIncMode:%d, RankAddrIncRate:%d, RankAddrIncVal:%d)\n", Socket, Controller, Channel, RankAddrIncMode, RankAddrIncRate, RankAddrIncVal));
  rcPrintf ((host,"Start: BiosServerSetSeqAddrInc (BankAddrIncMode:%d, BankAddrIncRate:%d, BankAddrIncVal:%d, RowAddrIncMode:%d, RowAddrIncRate:%d, RowAddrIncVal:%d)\n", BankAddrIncMode, BankAddrIncRate, BankAddrIncVal, RowAddrIncMode, RowAddrIncRate, RowAddrIncVal));
  rcPrintf ((host,"Start: BiosServerSetSeqAddrInc (ColAddrIncMode:%d, ColAddrIncRate:%d, ColAddrIncVal:%d", ColAddrIncMode, ColAddrIncRate, ColAddrIncVal));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqAddrIncChip (host, Socket, Controller, channelInSocket, RankAddrIncMode, RankAddrIncRate, RankAddrIncVal, BankAddrIncMode, BankAddrIncRate, BankAddrIncVal, RowAddrIncMode, RowAddrIncRate, RowAddrIncVal, ColAddrIncMode, ColAddrIncRate, ColAddrIncVal));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqAddrInc\n"));
#endif
}

/**
  Function used to set the sequence rank map.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      PairCount     - Number of entries in the DimmRankPairs array.  The minimum value is 1.  The value must be less than or equal to the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs - Pointer to array of DIMM and physical rank pair values.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.  Note that the same rank may appear more than once in the map.  Note also that all ranks do not need to appear in the map.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqRankMap) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 PairCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs
  )
{
  UINT8                                                  channelInSocket;
  struct sysHost                                         *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqRankMap (Socket:%d, Controller:%d, Channel:%d, PairCount:%d)\n", Socket, Controller, Channel, PairCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetSeqRankMapChip (host, Socket, Controller, channelInSocket, PairCount, DimmRankPairs));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqRankMap\n"));
#endif
}

/**
  Function used to set the sequence lower row address swizzles.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      SwizzleCount - Number of entries in the Swizzles array.  The value must be less than or equal to the MaxNumberRowAddrSwizzleEntries value from the GetCpgcInfo() function.  If the SwizzleCount value is 0 (and the Swizzles value is NULL) then the row address swizzling will be configured so that all the lower row addresses are mapped to themselves; effectively disabling the swizzling.
  @param[in]      Swizzles     - Pointer to array of lower row address swizzle values.  The values must be unique and less than or equal to the MaxRowAddrSwizzleVal value from the GetCpgcInfo() function.  The array position indicates the row address that the specified row address will be swapped with, e.g., a value of 10 at the array index 1 means that row addresses 10 and 1 will be swapped.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSeqRowAddrSwizzle) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 SwizzleCount,
  CONST UINT8           *Swizzles
  )
{
  UINT8                              i;
  UINT8                              j;
  UINT8                              channelInSocket;
  struct sysHost                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetSeqRowAddrSwizzle (Socket:%d, Controller:%d, Channel:%d, SwizzleCount:%d)\n", Socket, Controller, Channel, SwizzleCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  //Sorting through "Swizzles array" to check to see if all swizzle values are unique
  for (i = 0; i <= SwizzleCount; i++ ) {
    for (j = i+1; j <= SwizzleCount; j++ ) {  //Compare element[0], with all other array elements, if unqiue, move onto the next element and so on, reducing the array size during each "i" loop iternation
      if (*(Swizzles + i) != *(Swizzles + j)) {
        continue;
      } else {
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
      rcPrintf ((host, "\n BiosServerSetSeqRowAddrSwizzle: Error: No two swizzle values can be the same"));
#endif
      return;
      }
    } //for-j
  } //for-i

  CHIP_FUNC_CALL(host, BiosServerSetSeqRowAddrSwizzleChip (host, Socket, Controller, channelInSocket, SwizzleCount, Swizzles));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetSeqRowAddrSwizzle\n"));
#endif
}

/**
  Function used to get the current sequence state.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[out]     SeqState    - Pointer to buffer where sequence state will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetSeqState) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  SSA_SEQ_STATE         *SeqState
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
rcPrintf ((host,"\nStart: BiosServerGetSeqState (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif

  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetSeqStateChip (host, Socket, Controller, channelInSocket, SeqState));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetSeqState \n"));
#endif
}

/**
  Function used to set the CPGC stop mode.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      StopMode            - CPGC stop mode.
  @param[in]      StopOnNthErrorCount - Error count for stop-on-nth-any-lane error mode.  Minimum value is 1.  The value must be less than or equal to the MaxStopOnNthErrorCountVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetStopMode) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  CPGC_STOP_MODE        StopMode,
  UINT32                StopOnNthErrorCount
  )
{
  UINT8                                                  channelInSocket;
  struct sysHost                                         *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetStopMode (Socket:%d, Controller:%d, Channel:%d, StopMode:%d, StopOnNthErrorCount:%d)\n", Socket, Controller, Channel, StopMode, StopOnNthErrorCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetStopModeChip (host, Socket, channelInSocket, StopMode, StopOnNthErrorCount));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetStopMode\n"));
#endif
}

/**
  Function used to set the error counter mode.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Counter     - Zero based error counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param[in]      Mode        - Error counter mode.
  @param[in]      ModeIndex   - Extra index used to provide additional information if needed by the mode selected. This indicates which lane, nibble, byte, or chunk has been selected.  If CounterMode value is LaneErrorCounterMode then the ModeIndex value must be less than the BusWidth value from the GetSystemInfo() function.  If CounterMode is NibbleErrorCounterMode then the ModeIndex value must be less than the BusWidth/4 value from the GetSystemInfo() function.  If CounterMode is ByteErrorCounterMode then the ModeIndex value must be less than the BusWidth/8 value from the GetSystemInfo() function.  If CounterMode is ChunkErrorCounterMode then the ModeIndex value must be less than 8.

  @retval NotAvailable if the given Mode value is not supported.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetErrorCounterMode) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter,
  ERROR_COUNTER_MODE    Mode,
  UINT32                ModeIndex
  )
{
  UINT8                                   channelInSocket;
  struct sysHost                          *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "Start: BiosServerSetErrorCounterMode (Socket:%d, Controller:%d, Channel:%d, Counter:%d, Mode:%d, ModeIndex:%d)\n", Socket, Controller, Channel, Counter, Mode, ModeIndex));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetErrorCounterModeChip (host, Socket, channelInSocket, Counter, Mode, ModeIndex));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetErrorCounterMode\n"));
#endif
  return Success;
}

/**
  Function used to set the lane validation mask.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      LaneMasks  - Pointer to array of lane bitmasks.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Note that this may not be the same logic as the hardware.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetLaneValidationMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  CONST UINT8           *LaneMasks
  )
{
  UINT8                                i;
  UINT8                                channelInSocket;
  UINT8                                arrayElements = 0;
  UINT32                               EccMask32 = 0;
  struct sysHost                       *host;
  UINT64_STRUCT                        DqMask1;

  DqMask1.lo = 0;
  DqMask1.hi = 0;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetLaneValidationMask (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  if (host->nvram.mem.eccEn) { //BusWidth = 72(with Ecc), Array elements = 72/8
   arrayElements = 9;
  } else {
   arrayElements = 8;  //BusWidth = 64, Array elements = 64/8
  }

  //Separating the Array Elements into DQ Masks(lo,hi) and a ECC Mask
  for (i = 0; i < arrayElements; i++) {
    if (i < 4) {
      DqMask1.lo |= (UINT32) (((*(LaneMasks + i)) << i*8) & 0xFFFFFFFF);      // Saving of first 4 array elements into a 32 bit Mask (lo)
    } else if ((i >=4) && (i < 8)) {
      DqMask1.hi |= (UINT32) (((*(LaneMasks + i)) << (i-4)*8) & 0xFFFFFFFF);  // Saving of array elements 4-7 into a 32 bit Mask (hi)
    } else if (i == 8) {
      EccMask32 |= *(LaneMasks + i);
    }
  }

  //Need to invert the mask value as the HW treats 1 as mask out
  DqMask1.lo = ~DqMask1.lo;
  DqMask1.hi = ~DqMask1.hi;
  EccMask32 = (~EccMask32) & 0x000000FF;

  CHIP_FUNC_CALL(host, BiosServerSetLaneValidationMaskChip (host, Socket, channelInSocket, DqMask1.lo, DqMask1.hi, EccMask32));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetLaneValidationMask Hi (DqMask= 0x%x) Lo (DqMask= 0x%x)\n", DqMask1.hi, DqMask1.lo));
#endif
}

/**
  Function used to set the chunk validation mask.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      ChunkMask  - Mask for the chunks. One bit for each chunk of a cacheline.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 0 = chunk 0; bit position 1 = chunk 1, etc.  Note that this may not be the same logic as the hardware.

  @retval NotAvailable if the system does not provide chunk validation control.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetChunkValidationMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 ChunkMask
  )
{
  UINT8                                   i = 0;
  UINT8                                   channelInSocket;
  UINT8                                   ChunkPairsToCheck = 0;
  struct sysHost                          *host;

  //NotAvailable if the system does not provide chunk validation control - Supported on HSX
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetChunkValidationMask (Socket:%d, Controller:%d, Channel:%d, ChunkMask:%x)\n", Socket, Controller, Channel, ChunkMask));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  //The ChunkMask I/P has all 8 bits set, where each bit corresponds to one chunk (bytes)
  //So, if either Bit 0 or 1 is set in the ChunkMask I/P, we set that bit to 1 in ChunkPairsToCheck and so on for all 8 bits
  for (i = 0; i < 4; i = i + 1) {
    if ((((ChunkMask >> (2 * i)) & 0x01) == 1) || (((ChunkMask >> (2 * i+1)) & 0x01) == 1)) {
      ChunkPairsToCheck = ((ChunkPairsToCheck) | (0x1 << i)); //0001, 0
    }
  }

  CHIP_FUNC_CALL(host, BiosServerSetChunkValidationMaskChip (host, Socket, channelInSocket, ChunkPairsToCheck));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetChunkValidationMask\n"));
#endif
  return Success;
}

/**
  Function used to set the cacheline mask.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      CachelineMask - Mask for cachelines to be enabled.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 0 = cacheline 0; bit position 1 = cacheline 1, etc.  Note that this may not be the same logic as the hardware.

  @retval NotAvailable if the system does not provide cacheline validation control.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCachelineValidationMask) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 CachelineMask
  )
{

  UINT8                                 channelInSocket = 0;
  struct sysHost                        *host;
  //NotAvailable if the system does not provide cacheline validation control

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCachelineValidationMask (Socket:%d, Controller:%d, Channel:%d, CachelineMask:%x)\n", Socket, Controller, Channel, CachelineMask));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCachelineValidationMaskChip (host, Socket, channelInSocket, CachelineMask));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetCachelineValidationMask\n"));
#endif
  return Success;
}

/**
  Function used to get the lane error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      ChunkMask  - Mask for the chunks. One bit for each chunk.  This mask should set to be the same value as the mask being passed to the SetChunkValidationMask() function.  Products don’t have separate error status for even and odd UI should ignore this parameter.
  @param[out]     Status     - Pointer to array where the lane error status values will be stored.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetLaneErrorStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 ChunkMask,
  UINT8                 *Status
  )
{
  UINT8                                channelInSocket;
  struct sysHost                       *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetLaneErrorStatus (Socket:%d, Controller:%d, Channel:%d, ChunkMask:%x)\n", Socket, Controller, Channel, ChunkMask));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetLaneErrorStatusChip (host, Socket, channelInSocket, ChunkMask, Status));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetLaneErrorStatus \n"));
#endif
}

/**
  Function used to get the chunk error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the chunk error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred. Bit position 0 = chunk 0; bit position 1 = chunk 1, etc.

  @retval NotAvailable if the system does not provide chunk error status.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerGetChunkErrorStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *Status
  )
{
  UINT8                                 channelInSocket;
  struct sysHost                        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetChunkErrorStatus (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);
  //NotAvailable if the system does not provide chunk error status.

  CHIP_FUNC_CALL(host, BiosServerGetChunkErrorStatusChip (host, Socket, channelInSocket, Status));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerGetChunkErrorStatus(Status: 0x%x)\n", *Status));
#endif
  return Success;
}

/**
  Function used to get the rank error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the phyical rank error status of the given Dimm will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred. Bit position 0 = rank 0; bit position 1 = rank 1, etc.

  @retval NotAvailable if the system does not provide rank error status.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerGetRankErrorStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *Status
  )
{
  return NotAvailable;
}


/**
  Function used to get the address associated with the last error detected.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Address    - Pointer to buffer where error address will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetErrorAddress) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                     Socket,
  UINT8                     Controller,
  UINT8                     Channel,
  SSA_ADDRESS               *Address
  )
{
  UINT8                                              channelInSocket;
  struct sysHost                                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetErrorAddress (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetErrorAddressChip (host, Socket, channelInSocket, Address));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetErrorAddress \n"));
#endif
}

/**
  Function used to get the error count.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Counter    - Zero based counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param[out]     Count      - Pointer to where the error counter value will be stored.
  @param[out]     Overflow   - Pointer to where the error counter overflow status will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetErrorCount) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 Counter,
  UINT32                *Count,
  BOOLEAN               *Overflow
  )
{
  UINT8                                         channelInSocket;
  struct sysHost                                *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetErrorCount (Socket:%d, Controller:%d, Channel:%d, Counter:%d)\n", Socket, Controller, Channel, Counter));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetErrorCountChip (host, Socket, channelInSocket, Counter, Count, Overflow));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetErrorCount\n"));
#endif
}

/**
  Function used to set the DqDB error capture configuration.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Enable         - If set to TRUE then cachelines for which errors are detected are written to the DqDB starting at the StartCacheline and continuing to the EndCacheline before wrapping around to the StartCacheline.  If set to FALSE then the DqDB error capture feature is disabled.
  @param[in]      StartCacheline - The starting cacheline in the DqDB where the error status will be captured.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      EndCacheline   - The ending cacheline in the DqDB where the error status will be captured.  The value must be greater than or equal to the StartCacheline value.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetDqdbErrorCaptureConfig) (
  SSA_BIOS_SERVICES_PPI     *This,
  UINT8                     Socket,
  UINT8                     Controller,
  UINT8                     Channel,
  BOOLEAN                   Enable,
  UINT8                     StartCacheline,
  UINT8                     EndCacheline
  )
{
  UINT8                                     channelInSocket;
  struct sysHost                            *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetDqdbErrorCaptureConfig (Socket:%d, Controller:%d, Channel:%d, Enable:%d, StartCacheline:%d, EndCacheline:%d)\n", Socket, Controller, Channel, Enable, StartCacheline, EndCacheline));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetDqdbErrorCaptureConfigChip (host, Socket, channelInSocket, Enable, StartCacheline, EndCacheline));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerSetDqdbErrorCaptureConfig \n"));
#endif
}

/**
  Function used to set the DqDB error capture configuration.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[out]     CurrentCacheline - Pointer to where the current DqDB error capture cacheline value will be stored.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetDqdbErrorCaptureStatus) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 *CurrentCacheline
  )
{
  UINT8                                 channelInSocket;
  struct sysHost                        *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetDqdbErrorCaptureStatus (Socket:%d, Controller:%d, Channel:%d)\n", Socket, Controller, Channel));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerGetDqdbErrorCaptureStatusChip (host, Socket, channelInSocket, CurrentCacheline));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "End: BiosServerGetDqdbErrorCaptureStatus (CurrentCacheline= 0x%x)\n", *CurrentCacheline));
#endif
}

/**
  Function used to set the local test control.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      ClearErrors - Setting this parameter to TRUE causes all the channel’s error counters and status to be cleared.
  @param[in]      StartTest   - Setting this parameter to TRUE starts the channel’s CPGC engine.
  @param[in]      StopTest    - Setting this parameter to TRUE stops the channel’s CPGC engine.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetLocalTestControl) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               ClearErrors,
  BOOLEAN               StartTest,
  BOOLEAN               StopTest
  )
{
  UINT8                               channelInSocket;
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetLocalTestControl (Socket:%d, Controller:%d, Channel:%d, ClearErrors:%d, StartTest:%d, StopTest:%d)\n", Socket, Controller, Channel, ClearErrors, StartTest, StopTest));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetLocalTestControlChip (host, Socket, Controller, channelInSocket, ClearErrors, StartTest, StopTest));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetLocalTestControl\n"));
#endif
}

/**
  Function used to set the global test control.  Settings apply to all channels whose EnableGlobalControl is set to TRUE (see SetSeqConfig function).

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      ClearErrors - Setting this parameter to TRUE causes all controller’s error counters and status to be cleared.
  @param[in]      StartTest   - Setting this parameter to TRUE starts the channel’s CPGC engine.
  @param[in]      StopTest    - Setting this parameter to TRUE stops the channel’s CPGC engine.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetGlobalTestControl) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               ClearErrors,
  BOOLEAN               StartTest,
  BOOLEAN               StopTest
  )
{
  struct sysHost                      *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetGlobalTestControl (Socket:%d, Controller:%d, ClearErrors:%d, StartTest:%d, StopTest:%d)\n", Socket, Controller, ClearErrors, StartTest, StopTest));
#endif

  CHIP_FUNC_CALL(host, BiosServerSetGlobalTestControlChip (host, Socket, Controller, ClearErrors, StartTest, StopTest));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetGlobalTestControl\n"));
#endif
}

/**
Function used to set the CPGC address configuration.  If the system does not support the CPGC address configuration feature then the function simply returns.

@param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
@param[in]      Socket      - Zero based CPU socket number.
@param[in]      Controller  - Zero based controller number.
@param[in]      Channel     - Zero based channel number.
@param[in]      EnableDdr4  - Setting this parameter to TRUE causes the CPGC engine to be in the DDR4 test mode, otherwise it’s in the DDRT test mode.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetAddrConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                Socket,
  UINT8                Controller,
  UINT8                Channel,
  BOOLEAN              EnableDdr4
  )
{
  //Not available on Broadwell Server(BDX); only for Skylake Server(SKX)
  UINT8                                 channelInSocket;
  struct sysHost                        *host;

  //We need to set a bit ddrt_addr_mode(per channel) to 0 when testing DDR4 DIMMs and set to 1 when testing DDRT DIMMs.
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetAddrConfig (Socket:%d, Controller:%d, Channel:%d, EnableDdr4:%d)\n", Socket, Controller, Channel, EnableDdr4));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetAddrConfigChip (host, Socket, Controller, channelInSocket, EnableDdr4));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetAddrConfig\n"));
#endif
}

/**
  Function used to get the test done status.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      ChannelMask  - Channel mask.  Each bit represents a channel to be checked.  Bit value 1 = perform check; bit value 0 = do not perform check.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.
  @param[out]     TestDoneMask - Pointer to where mask of per-channel test done status will be placed.  Bit value 1 = test complete; bit value 0 = test not complete or check not performed.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerGetTestDone) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelMask,
  UINT8                 *TestDoneMask
  )
{
  struct sysHost                               *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
  //NOTE: ChannelMask will be entered as 0,1 and Controller = 0,1
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerGetTestDone (Socket:%d, Controller:%d, ChannelMask:0x%x)\n", Socket, Controller, ChannelMask));
#endif

  CHIP_FUNC_CALL(host, BiosServerGetTestDoneChip (host, Socket, Controller, ChannelMask, TestDoneMask));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerGetTestDone (TestDoneMask:0x%x)\n", *TestDoneMask));
#endif
}

/**
  Function used to set the scrambler configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether the scrambler is enabled.
  @param[out]     PrevEnable - Pointer to where the previous scrambler enable setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetScramblerConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  //This function performs Get/Set/Restore for the Scrambler register bits
  struct sysHost                       *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetScramblerConfig (Socket:%d, Controller:%d, Enable:%d)\n", Socket, Controller, Enable));
#endif

  CHIP_FUNC_CALL(host, BiosServerSetScramblerConfigChip (host, Socket, Controller, Enable, PrevEnable));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetScramblerConfig\n"));
#endif
}

/**
  Function used to set the self-refresh configuration.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Enable     - Specifies whether self-refreshes are enabled.
  @param[out]     PrevEnable - Pointer to where the previous self-refresh enable setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
VOID
(EFIAPI BiosServerSetSelfRefreshConfig) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
// NotAvailable as of now
}

/**
  Function used to set the refresh override.

  @param[in, out] This                   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                 - Zero based CPU socket number.
  @param[in]      Controller             - Zero based controller number.
  @param[in]      Channel                - Zero based channel number.
  @param[in]      EnablePanicRefreshOnly - If set to TRUE then opportunistic and high priority auto refreshes will be disabled for all ranks and refreshes will only occur when the panic refresh watermark has been exceeded.  If set to FALSE then refreshes will occur using the existing MC functional refresh logic.
  @param[in]      PairCount              - Number of entries in the DimmRankPairs array.  If you wish to have no rank’s refresh overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs          - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any refresh to that rank while in REUT mode. If a DIMM/rank doesn’t appear in the list then the CPGC engine will enable executing a refresh (assuming the MC has refreshes enabled) to that rank while in REUT mode based on the EnablePanicRefreshOnly parameter value.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval NotAvailable if the system does not support the refresh override feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetRefreshOverride) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnablePanicRefreshOnly,
  UINT8                 PairCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs
  )
{
  UINT8                                     channelInSocket;
  struct sysHost                            *host;

  //NotAvailable if the system does not support the refresh override feature.
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetRefreshOverride (Socket:%d, Controller:%d, Channel:%d, EnablePanicRefreshOnly:%d, PairCount:%d)\n", Socket, Controller, Channel, EnablePanicRefreshOnly, PairCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetRefreshOverrideChip (host, Socket, channelInSocket, EnablePanicRefreshOnly, PairCount, DimmRankPairs));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetRefreshOverride\n"));
#endif

  return Success;
}

/**
  Function used to set the refresh override.

  @param[in, out] This                    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                  - Zero based CPU socket number.
  @param[in]      Controller              - Zero based controller number.
  @param[in]      Channel                 - Zero based channel number.
  @param[in]      EnableZQCalAfterRefresh - If set to TRUE then a ZQ operation will always take place after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.  If set to FALSE then a ZQ operation will take place at the normal programmed period after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.
  @param[in]      PairCount               - Number of entries in the DimmRankPairs array.  If you wish to have no rank’s ZQCal overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the product of the MaxNumberDimms and MaxNumberRanks values from the GetSystemInfo() function.
  @param[in]      DimmRankPairs           - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any ZQCal to that rank while in REUT mode.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval NotAvailable if the system does not support the ZQCal override feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetZQCalOverride) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnableZQCalAfterRefresh,
  UINT8                 PairCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs
  )
{
  UINT8                                     channelInSocket;
  struct sysHost                            *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetZQCalOverride (Socket:%d, Controller:%d, Channel:%d, EnableZQCalAfterRefresh:%d, PairCount:%d)\n", Socket, Controller, Channel, EnableZQCalAfterRefresh, PairCount));
#endif
  channelInSocket = SSAGetChannelInSocket (Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetZQCalOverrideChip (host, Socket, channelInSocket, EnableZQCalAfterRefresh, PairCount, DimmRankPairs));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetZQCalOverride\n"));
#endif
  return Success;
}

/**
  Function used to set the on-die-termination override.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket            - Zero based CPU socket number.
  @param[in]      Controller        - Zero based controller number.
  @param[in]      Channel           - Zero based channel number.
  @param[in]      EnableMprDdrTrain - This parameter is set to TRUE during DDR training.
  @param[in]      ValCount          - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank’s ODT overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than twice the MaxNumberDimms value from the GetSystemInfo() function.
  @param[in]      DimmRankPairs     - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its ODT value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its ODT will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param[in]      OverrideVals      - Pointer to array of override values.  Value 1 = enable ODT for that rank while in REUT mode; value 0 = disable ODT for that rank while in REUT mode.

  @retval NotAvailable if the system does not support the ODT override feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetOdtOverride) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  BOOLEAN               EnableMprDdrTrain,
  UINT8                 ValCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs,
  CONST UINT8           *OverrideVals
  )
{
  UINT8                              channelInSocket;
  struct sysHost                     *host;

  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetOdtOverride (Socket:%d, Controller:%d, Channel:%d, EnableMprDdrTrain:%d, ValCount:%d)\n", Socket, Controller, Channel, EnableMprDdrTrain, ValCount));
#endif
  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetOdtOverrideChip (host, Socket, channelInSocket, EnableMprDdrTrain, ValCount, DimmRankPairs, OverrideVals));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetOdtOverride\n"));
#endif
  return Success;
}

/**
  Function used to set the clock-enable override.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      ValCount      - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank’s CKE overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its CKE value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its CKE will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param[in]      OverrideVals  - Pointer to array of override values.  Value 1 = enable CKE for that rank while in REUT mode; value 0 = disable CKE for that rank while in REUT mode.

  @retval NotAvailable if the system does not support the CKE override feature.  Else Success.
**/
SSA_STATUS
(EFIAPI BiosServerSetCkeOverride) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 Channel,
  UINT8                 ValCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs,
  CONST UINT8           *OverrideVals
  )
{
  UINT8                                     channelInSocket;
  struct sysHost                            *host;

  //NotAvailable if the system does not support the CKE override feature- Supported on HSX
  host = (struct sysHost *) This->SsaMemoryConfig->MrcData;
#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"\nStart: BiosServerSetCkeOverride (Socket:%d, Controller:%d, Channel:%d, ValCount:%d)\n", Socket, Controller, Channel, ValCount));
#endif

  channelInSocket = SSAGetChannelInSocket(Controller, Channel);

  CHIP_FUNC_CALL(host, BiosServerSetCkeOverrideChip (host, Socket, channelInSocket, ValCount, DimmRankPairs, OverrideVals));

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host,"End: BiosServerSetCkeOverride\n"));
#endif
  return Success;
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
// end file BiosSsaMemoryServerConfig.c
