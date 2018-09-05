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
 * Copyright 2014 - 2016 Intel Corporation All Rights Reserved.
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
 *      This file contains PPR (Post Package Repair) routines.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "MemFunc.h"

UINT8
Setrankvalue(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     rank,
  UINT8     subrank
  )
{
  struct channelNvram   (*channelNvList)[MAX_CH];
  channelNvList = &host->nvram.mem.socket[socket].channelList;

  if ((host->nvram.mem.dramType == SPD_TYPE_DDR4) && ((*channelNvList)[ch].encodedCSMode == 1)) {
    rank |= (subrank << 1);
  }
  return rank;
}
/**

  Run write/read test on a row

  @param host     - Pointer to sysHost
  @param socket   - socket id
  @param chBitMask- target CHs on which PPR seq. should be executed
  @param pprAddr  - DRAM Address that need to be repaired
  @param bwSerr   - CPGC Error status

  @retval status - SUCCESS / failure

**/
UINT32
RowTestPPR (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     chBitMask,
  PPR_ADDR  pprAddr[MAX_CH],
  UINT32    bwSerr[MAX_CH][3]
  )
{
  UINT32  status = SUCCESS;
  UINT8   ch;
  UINT16  testPattern;

  //
  // Test pattern all 1s
  //
  testPattern = DDR_CPGC_MEM_TEST_ALL0 | DDR_CPGC_MEM_TEST_INVERTPAT;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
        pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_WRITE ));
  }

  CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
                                 "RowTestPPR - Row:%d Write all 1s done\n", pprAddr[ch].row));
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
        pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_READ ));
  }

  CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern | DDR_CPGC_MEM_TEST_READ));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
        "RowTestPPR - Row:%d Read all 1s done\n", pprAddr[ch].row));
  }

  status = CollectTestResults (host, socket, chBitMask, bwSerr);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "RowTestPPR - Row Read Test status :%x\n", status));

  if (status) return status;

  //
  // Test pattern all 0s
  //
  testPattern = DDR_CPGC_MEM_TEST_ALL0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
        pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_WRITE));
  }
  CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern | DDR_CPGC_MEM_TEST_WRITE));

  for (ch = 0; ch < MAX_CH; ch++) {
     if ((~chBitMask) & (1 << ch)) continue;
     MemDebugPrint((host, SDBG_MAX, socket, NO_CH, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
                               "RowTestPPR - Row:%d Write all 0s done\n", pprAddr[ch].row));
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
        pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_READ));
  }

  CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern | DDR_CPGC_MEM_TEST_READ));

  for (ch = 0; ch < MAX_CH; ch++) {
     if ((~chBitMask) & (1 << ch)) continue;
     MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
         "RowTestPPR - Row:%d Read all 0s done\n", pprAddr[ch].row));
  } // ch loop

  status = CollectTestResults (host, socket, chBitMask, bwSerr);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "RowTestPPR - Row Read Test status :%x\n", status));

  return status;
}

/**

  Run write/read test on a row for PPR Err injection testing

  @param host     - Pointer to sysHost
  @param socket   - socket id
  @param chBitMask- target CHs on which PPR seq. should be executed
  @param pprAddr  - DRAM Address that need to be repaired
  @param bwSerr   - CPGC Error status
  @param isWrite  - do a write / read test

  @retval status - SUCCESS / failure

**/
UINT32
PprErrInjTest (
   PSYSHOST  host,
   UINT8     socket,
   UINT8     chBitMask,
   PPR_ADDR  pprAddr[MAX_CH],
   UINT32    bwSerr[MAX_CH][3],
   UINT8     isWrite
   )
 {
   UINT32  status = SUCCESS;
   UINT8   ch;
   UINT16  testPattern;

   testPattern = DDR_CPGC_PPR_TEST | DDR_CPGC_MEM_TEST_INVERTPAT;

   if (isWrite) {
     //
     // Write pattern
     //
     for (ch = 0; ch < MAX_CH; ch++) {
       if ((~chBitMask) & (1 << ch)) continue;

       CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
           pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_WRITE ));
     }

     CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern | DDR_CPGC_MEM_TEST_WRITE));
     for (ch = 0; ch < MAX_CH; ch++) {
       if ((~chBitMask) & (1 << ch)) continue;
       MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
           "PprErrInjTest - Row:%d Write all 1s done\n", pprAddr[ch].row));
     }
   } else {
     //
     // Read / test pattern
     for (ch = 0; ch < MAX_CH; ch++) {
       if ((~chBitMask) & (1 << ch)) continue;

       CHIP_FUNC_CALL(host, CpgcPprTestGlobalSetup (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank,
           pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row, testPattern | DDR_CPGC_MEM_TEST_READ ));
     }

     CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, testPattern | DDR_CPGC_MEM_TEST_READ));
     for (ch = 0; ch < MAX_CH; ch++) {
       if ((~chBitMask) & (1 << ch)) continue;
       MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
           "PprErrInjTest - Row:%d Read done\n", pprAddr[ch].row));
     }
     status = CollectTestResults (host, socket, chBitMask, bwSerr);

     for (ch = 0; ch < MAX_CH; ch++) {
       if ((~chBitMask) & (1 << ch)) continue;

       MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
           "PprErrInjTest - Row:%d Read Test status :%x\n", pprAddr[ch].row, status));
     }
   }
   return status;
 }

/**

  Get fail device from CPGC error status

  @param host     - Pointer to sysHost
  @param socket   - socket id
  @param ch       - DDR CH ID
  @param dimm     - DIMM ID
  @param bwSerr   - CPGC Error status

  @retval dramMask - bit map of DRAMs that failed (BIT 0 - DRAM0, BIT1 - DRAM1...and so on)

**/
UINT32
GetFailDevice(
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch,
    UINT8     dimm,
    UINT32    bwSerr[3]
 )
{
  UINT8   dram;
  UINT8   i;
  UINT32  dramMask;
  struct  dimmNvram  (*dimmNvList)[MAX_DIMM];

  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
             "GetFailDevice - bwSerr[0]:0x%08x, bwSerr[1]:0x%08x, bwSerr[2]:0x%08x\n", bwSerr[0], bwSerr[1], bwSerr[2]));
  dimmNvList = GetDimmNvList(host, socket, ch);

  dram = DRAM_UNKNOWN;
  dramMask = 0;
  for (i = 0; i < 4; i++) {
    if (((bwSerr[0] >> (i * 8)) & 0xFF) != 0) {
      dram = 0;
      if ((*dimmNvList)[dimm].x4Present) {
        if (((bwSerr[0] >> (i * 8)) & 0x0F) != 0) {
          dram = i*2;
          if (dram != DRAM_UNKNOWN) {
            dramMask |= 1 << dram;
          }
        }
        if (((bwSerr[0] >> (i * 8)) & 0xF0) != 0) {
          dram = i*2 + 1;
          if (dram != DRAM_UNKNOWN) {
            dramMask |= 1 << dram;
          }
        }
      } else {
        dram = i;
        if (dram != DRAM_UNKNOWN) {
          dramMask |= 1 << dram;
        }
      }
    }

    if (((bwSerr[1] >> (i * 8)) & 0xFF) != 0) {
      dram = 4;
      if ((*dimmNvList)[dimm].x4Present) {
        if (((bwSerr[1] >> (i * 8)) & 0x0F) != 0) {
          dram = i*2 + 8;
          if (dram != DRAM_UNKNOWN) {
            dramMask |= 1 << dram;
          }
        }
        if (((bwSerr[1] >> (i * 8)) & 0xF0) != 0) {
          dram = i*2 + 8 + 1;
          if (dram != DRAM_UNKNOWN) {
            dramMask |= 1 << dram;
          }
        }
      } else {
        dram = i + 4;
        if (dram != DRAM_UNKNOWN) {
          dramMask |= 1 << dram;
        }
      }
    }
  }

  if ((bwSerr[2] & 0xFF) != 0) {
    dram = 8;
    if ((*dimmNvList)[dimm].x4Present) {
      if ((bwSerr[2] & 0x0F) != 0) {
        dram =  16;
        dramMask |= 1 << dram;
      }
      if ((bwSerr[2] & 0xF0) != 0) {
        dram = 17;
        dramMask |= 1 << dram;
      }
    } else {
      dram = 8;
      dramMask |= 1 << dram;
    }
  }

  return dramMask;
}

/**

  Execute PPR sequence on the specified address

  @param host     - Pointer to sysHost
  @param socket   - socket id
  @param chBitMask- target CHs on which PPR seq. should be executed
  @param pprAddr  - DRAM Address that need to be repaired
  @param pprType  - PPR type - hard / soft PPR
  @param pprStatus - Status of PPR operation
  @retval SUCCESS

**/
UINT32
ExecutePPR (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     chBitMask,
  PPR_ADDR  pprAddr[MAX_CH],
  PPR_TYPE  pprType,
  UINT32    pprStatus[MAX_CH]
  )
{
  UINT32  status = SUCCESS;
  UINT8   ch;
  UINT8 controlWordData = 0;
  UINT32  tPGM = 1;
  UINT16  mrsData;
  struct  dimmNvram  (*dimmNvList)[MAX_DIMM];
  struct  rankDevice (*rankStruct)[MAX_RANK_DIMM];
  BOOLEAN isPageTimerEnabled[MAX_CH] = {FALSE};
  UINT32  bwSerr[MAX_CH][3];
  UINT32  failDevMask;

  OutputCheckpoint (host, STS_CHANNEL_TRAINING, PPR_PRE_TEST, 0);

  if (!host->setup.mem.pprErrInjTest) {
    //
    // Test row to find device that failed (required to handle UC errors)
    //
    if (RowTestPPR (host, socket, chBitMask, pprAddr, bwSerr)) {
      //
      // Row test failed, figure out which CH / device failed.
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((~chBitMask) & (1 << ch)) continue;
        failDevMask = GetFailDevice (host, socket, ch, pprAddr[ch].dimm, bwSerr[ch]);
        MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
            "ExecutePPR - Pre-PPR Row test failed - dramMask = 0x%x\n", failDevMask));

        //
        // if pprAddr[ch].dramMask is 0, it is possible UC error and runtime handler could not find the fail devices;
        // initialize with failDevMask value
        //
        if (pprAddr[ch].dramMask == 0) {
          pprAddr[ch].dramMask = failDevMask;
          MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
              "ExecutePPR - Input dramMask is 0, initializing with detected fail mask = 0x%x\n", pprAddr[ch].dramMask));
        }
      } // ch loop
    }
  } else {
    //
    // Write a known pattern to PPR address
    //
    PprErrInjTest (host, socket, chBitMask, pprAddr, bwSerr, 1);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    pprStatus[ch] = 0;
    MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
      "ExecutePPR - Row: %d, Bank: %d, CID: %d, DramMask: %x\n", pprAddr[ch].row, pprAddr[ch].bank, pprAddr[ch].subRank, pprAddr[ch].dramMask));
  }

  OutputCheckpoint (host, STS_CHANNEL_TRAINING, PPR_SEQ_BEGIN, 0);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    //
    // Pre-charge setup
    //
    CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(pprAddr[ch].dimm, pprAddr[ch].rank)), pprAddr[ch].subRank, PRECHARGE, NORMAL_CADB_SETUP));
  } // ch loop

  //
  // Precharge all Banks
  //
  ExecuteCmdSignalTest (host, socket, chBitMask);
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "ExecutePPR - Pre-charge before PPR\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    //
    // Disable page timers
    //
    isPageTimerEnabled[ch] = CHIP_FUNC_CALL(host, IsChipPageTableTimerEnabled (host, socket, ch));
    if (isPageTimerEnabled[ch]) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "ExecutePPR - Disable Page Timer\n"));
      CHIP_FUNC_CALL(host, SetChipPageTableTimer (host, socket, ch, PGT_TIMER_DISABLE));
    }


    dimmNvList = GetDimmNvList(host, socket, ch);
    rankStruct = GetRankStruct(host, socket, ch, pprAddr[ch].dimm);

    if (pprType == PprTypeHard) {
      tPGM = 2000000;    // 2000 ms for hard PPR
      mrsData = (*rankStruct)[pprAddr[ch].rank].MR4 | BIT13;
    } else {
      tPGM = 1;         // 1us for soft PPR (actually tWR is enough, but we can do delays only in uS resolution)
      mrsData = (*rankStruct)[pprAddr[ch].rank].MR4 | BIT5;
    }

    //
    //Workaround: Disable DB snooping by writing to F0RCBx
    //
    if (IsLrdimmPresent(host, socket, ch, pprAddr[ch].dimm)) { 
      controlWordData = 8;
      WriteRC (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), controlWordData, RDIMM_RCBx);
    }
    //
    // MR command to enable PPR Mode
    //
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "ExecutePPR - MR4 write to enable PPR: 0x%0x\n", mrsData));
    WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), mrsData, BANK4);
    //
    // Issue Guard Key if hard PPR or Soft PPR on Samsung
    //    
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "ExecutePPR - Issue 4 MR0 guard key sequence\n"));
      WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), PPR_GUARD_KEY0, BANK0);
      WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), PPR_GUARD_KEY1, BANK0);
      WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), PPR_GUARD_KEY2, BANK0);
      WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), PPR_GUARD_KEY3, BANK0);
 
    
    //
    // Setup to issue WR Command with PPR DQ pattern
    //
    CHIP_FUNC_CALL(host, CpgcSetupPprPattern (host, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, pprAddr[ch].subRank, pprAddr[ch].dramMask, pprAddr[ch].bank, pprAddr[ch].row));
  } // ch loop

  //
  // Run PPR sequence
  //
  CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTest (host, socket, chBitMask, 0));

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "ExecutePPR - WR with PPR pattern issued\n"));
  //
  // Wait for tPGM - 2000 mS for hard PPR and tWR for soft PPR
  //
  FixedDelay (host, tPGM);

  //
  // Issue pre-charge
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(pprAddr[ch].dimm, pprAddr[ch].rank)), pprAddr[ch].subRank, PRECHARGE, NORMAL_CADB_SETUP));
  } // ch loop
  ExecuteCmdSignalTest (host, socket, chBitMask);
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "ExecutePPR - Pre-charge after PPR\n"));
  //
  // Wait for at least tPGM_exit - 15 nS
  //
  FixedDelay (host, 1);  // 1 uS

  //
  // MR command to disable PPR Mode
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    rankStruct = GetRankStruct(host, socket, ch, pprAddr[ch].dimm);
    WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), (*rankStruct)[pprAddr[ch].rank].MR4, BANK4);
    MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
                            "ExecutePPR - Disable PPR - MRS Command issued: %x\n", (*rankStruct)[pprAddr[ch].rank].MR4));

  } // ch loop

  //
  // Wait for at least tPGMPST - 50uS
  //
  FixedDelay (host, 50);

  //
  //Workaround: Enable DB snooping by writing to F0RCBx
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;
    
    if (IsLrdimmPresent(host, socket, ch, pprAddr[ch].dimm)) { 
      controlWordData = 0;
      WriteRC (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), controlWordData, RDIMM_RCBx);
    }
 }

  //
  // POST PPR sequence
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((~chBitMask) & (1 << ch)) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    rankStruct = GetRankStruct(host, socket, ch, pprAddr[ch].dimm);

    //
    // If Guard key was enabled, restore MR0. It could have been overwritten if PPR sequence had error
    //
    if ((pprType == PprTypeHard) || ((pprType == PprTypeSoft) && ((*dimmNvList)[pprAddr[ch].dimm].SPDDramMfgId == MFGID_SAMSUNG))) {
      WriteMRS (host, socket, ch, pprAddr[ch].dimm, Setrankvalue(host, socket, ch, pprAddr[ch].rank, pprAddr[ch].subRank), (*rankStruct)[pprAddr[ch].rank].MR0, BANK0);
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                 "ExecutePPR - Restore MR0 post PPR\n"));
    }

    //
    // Re-enable page timer if required
    //
    if (isPageTimerEnabled[ch]) {
      CHIP_FUNC_CALL(host, SetChipPageTableTimer (host, socket, ch, PGT_TIMER_ENABLE));
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "ExecutePPR - Re-enable Page Timer\n"));
    }
  } // ch loop


  //
  // Test row after repair
  //
  OutputCheckpoint (host, STS_CHANNEL_TRAINING, PPR_POST_TEST, 0);

  if (host->setup.mem.pprErrInjTest) {
    //
    // Read previously written pattern and check if data is good
    // Data written before PPR should not match with data read after PPR
    //
    PprErrInjTest (host, socket, chBitMask, pprAddr, bwSerr, 0);
    //
    // figure out which device failed.
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((~chBitMask) & (1 << ch)) continue;
      failDevMask = GetFailDevice (host, socket, ch, pprAddr[ch].dimm, bwSerr[ch]);
      //Non-target DRAMs are overwritten during hard PPR sequence in Samsung DIMMs. Skip non-target DRAM checking"
      dimmNvList = GetDimmNvList(host, socket, ch);
      if (((*dimmNvList)[pprAddr[ch].dimm].SPDMMfgId == MFGID_SAMSUNG) && ((pprType == PprTypeHard)||(pprType == PprTypeSoft))) failDevMask = (failDevMask & pprAddr[ch].dramMask);
      if (failDevMask == pprAddr[ch].dramMask) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
            "ExecutePPR - Post-PPR Row test (PprErrInjTest) passed - dramMask = 0x%x,\n", failDevMask));
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
                  "ExecutePPR - Post-PPR Row test (PprErrInjTest) failed - dramMask = 0x%x\n", failDevMask));
        pprStatus[ch] = failDevMask;
      }
    } // ch loop
  }

  if (RowTestPPR (host, socket, chBitMask, pprAddr, bwSerr)) {
    //
    // Row test failed, figure out which device failed.
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((~chBitMask) & (1 << ch)) continue;
      failDevMask = GetFailDevice (host, socket, ch, pprAddr[ch].dimm, bwSerr[ch]);
      MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
          "ExecutePPR - Post-PPR Row test (RowTestPPR) failed - dramMask = 0x%x\n", failDevMask));
      pprStatus[ch] = failDevMask;
    } // ch loop
  }

  return status;
}

/**

  Get next address from list for PPR operation

  @param host     - Pointer to sysHost
  @param socket   - socket id
  @param ch       - DDR CH
  @param currEntry - current index in the list
  @param pprAddr  - buffer to hold DRAM Address that need to be repaired
  @param chMask   - buffer to hold the ch bitmap (0 - no PPR for this CH, 1 - run PPR on this CH)

  @retval nextEntry - index of next entry in the list

**/
UINT8
GetNextPprAddrForSktCh (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch,
    UINT8     currEntry,
    UINT8     *chMask,
    PPR_ADDR  *pprAddr
  )
{
  UINT8   nextEntry = 0xFF;
  UINT8   mcId;
  UINT8   mcCh;

  if ((*chMask) & (1 << ch)) {
    mcId = CHIP_FUNC_CALL(host, GetMCID (host, socket, ch));
    mcCh = CHIP_FUNC_CALL(host, GetMCCh (mcId, ch));
    for (; currEntry < MAX_PPR_ADDR_ENTRIES; currEntry++) {
      if (((host->setup.mem.pprAddrSetup[currEntry].pprAddrStatus & PPR_STS_ADDR_VALID)) && (mcId == host->setup.mem.pprAddrSetup[currEntry].mc) &&
          (socket == host->setup.mem.pprAddrSetup[currEntry].socket) && (mcCh == host->setup.mem.pprAddrSetup[currEntry].ch)) {
        nextEntry = currEntry + 1;
        *pprAddr = host->setup.mem.pprAddrSetup[currEntry].pprAddr;
        break;
      }
    } // for ppr entry
  }
  if (nextEntry == 0xFF) {
    (*chMask) &= ~(1 << ch);
  }

  return nextEntry;
}

/**

  Main routine that runs Post Pckage Repair for all valid entries in error address list

  @param host     - Pointer to sysHost

  @retval status - SUCCESS / failure

**/
UINT32
PostPackageRepairMain (
    PSYSHOST           host
    )
{
  UINT32        chBitMask = 0;
  UINT32        status = SUCCESS;

  //host->var.mem.notRunningFromCT; the Default will be 0, set it to 1 when calling with chBitMask
  if (host->var.mem.notRunningFromCT == 0) {
    chBitMask = CH_BITMASK;
  } else {
    chBitMask = host->var.mem.chBitMask; //Use the 'chBitMask' value set in JedecInitSequence (For a SSA or non SSA I/P)
  }

  status = PostPackageRepair (host, chBitMask);

  return status;
} //PostPackageRepairMain

/**

  Routine that runs Post Package Repair for all valid entries in error address list

  @param host     - Pointer to sysHost
  @param host     - chBitMask
  @retval status - SUCCESS / failure

**/
UINT32
PostPackageRepair (
    PSYSHOST       host,
    UINT32         chBitMask
    )
{
  UINT32    status = SUCCESS;
  PPR_ADDR  pprAddr[MAX_CH];
  UINT8     chMask;
  UINT8     tempChMask;
  PPR_TYPE  pprType;
  UINT8     dimm;
  UINT8     ch;
  UINT8     socket;
  UINT8     pprEntry;
  UINT8     pprCurrEntry[MAX_CH];
  UINT32    pprStatus[MAX_CH];
  struct channelNvram   (*channelNvList)[MAX_CH];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];

  if ((host->setup.mem.pprType == PPR_DISABLED) || (host->nvram.mem.dramType != SPD_TYPE_DDR4)  || (host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast)) return status;

  if (host->setup.mem.pprType == PPR_TYPE_HARD) {
    pprType = PprTypeHard;
  } else {
    pprType = PprTypeSoft;
  }

  //
  // Display PPR capabilities
  //
  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    //Check if this channel has been masked off as each BSSA Call is per MC
    pprCurrEntry[ch] = 0;
  }

  if (host->var.mem.softPprDone[socket] == 0) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((chBitMask & (1 << ch)) == 0) continue;
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
            "SPD Other Opt features [byte 9]: 0x%02X\n", (*dimmNvList)[dimm].SPDOtherOptFeatures));

        if (((*dimmNvList)[dimm].SPDOtherOptFeatures & SPD_PPR_MASK) == SPD_PPR_HARD_1R) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "Hard PPR Supported: Yes\n"));
        } else {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "Hard PPR Supported: No\n"));
        }

        if ((*dimmNvList)[dimm].SPDOtherOptFeatures & SPD_SOFT_PPR) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "Soft PPR Supported: Yes\n"));
        } else {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "Soft PPR Supported: No\n"));
        }
      }
    }
  }

  chMask = (1 << MAX_CH) - 1;
  for (pprEntry = 0; pprEntry < MAX_PPR_ADDR_ENTRIES; pprEntry++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((chBitMask & (1 << ch)) == 0) continue;
      if ((*channelNvList)[ch].enabled == 0) {
        chMask &= ~(1 << ch);
        continue;
      }
      if ((~chMask) & (1 << ch)) continue;

      pprStatus[ch] = 0;
      pprCurrEntry[ch] = GetNextPprAddrForSktCh (host, socket, ch, pprCurrEntry[ch], &chMask, &pprAddr[ch]);
      if (chMask & (1 << ch)) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, NO_STROBE, NO_BIT,
            "PostPackageRepair - Row: %d, Bank: %d, CID: %d, DramMask: %x\n", pprAddr[ch].row, pprAddr[ch].bank, pprAddr[ch].subRank, pprAddr[ch].dramMask));
      }
    } // ch loop

    if (chMask == 0) break;

    //
     // Check if specified DIMMs support PPR type requested and adjust chBitMask
     //
     tempChMask = chMask;
     for (ch = 0; ch < MAX_CH; ch++) {
       if ((chBitMask & (1 << ch)) == 0) continue; //Exit if CH not in mask
       if ((~tempChMask) & (1 << ch)) continue;

       dimmNvList = GetDimmNvList(host, socket, ch);
       if (((((*dimmNvList)[pprAddr[ch].dimm].SPDOtherOptFeatures & SPD_PPR_MASK) != SPD_PPR_HARD_1R) && (pprType == PprTypeHard)) ||
           ((!((*dimmNvList)[pprAddr[ch].dimm].SPDOtherOptFeatures & SPD_SOFT_PPR)) && (pprType == PprTypeSoft)) ) {
         tempChMask &= ~(1 << ch);
         MemDebugPrint((host, SDBG_MAX, socket, ch, pprAddr[ch].dimm, NO_RANK, NO_STROBE, NO_BIT,
                    "PostPackageRepair - DIMM doesn't support requested PPR type; skipping it!\n"));
       }
     } // ch loop

     if (tempChMask == 0) continue;

    //
    // TODO: Read SPD to determine if unused PPR resources are available
    // Skip if no PPR resources available
    //

    ExecutePPR(host, socket, tempChMask, pprAddr, pprType, pprStatus);

    //
    // Update PPR status
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((chBitMask & (1 << ch)) == 0) continue;
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((~chMask) & (1 << ch)) continue;
      if (pprStatus[ch]) {
        host->var.mem.pprStatus[pprCurrEntry[ch]-1] = PPR_STS_FAILED;
        EwlOutputType2(host, WARN_PPR_FAILED, 0, socket, ch, pprAddr[ch].dimm, pprAddr[ch].rank, EwlSeverityWarning, NO_STROBE, NO_BIT, GsmGtDelim, DdrLevel, 0xFF);
      } else {
        host->var.mem.pprStatus[pprCurrEntry[ch]-1] = PPR_STS_SUCCESS;

        //
        // If this is soft PPR sequence, update flag
        //
        if (pprType == PprTypeSoft) {
          host->var.mem.softPprDone[socket] = 1;
        }

        //
        // TODO: Update SPD to indicate the PPR resources that have been used
        //
      }
    }
  } // for PPR loop

  //
  // Clean-up after PPR
  //
  CHIP_FUNC_CALL(host, CpgcPprCleanup (host, socket, chBitMask));

  return status;
}
