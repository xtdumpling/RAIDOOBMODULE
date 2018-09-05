/**

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file Cpgc.h

  Memory Initialization Module.

**/

#ifndef _CPGCCHIP_H
#define _CPGCCHIP_H

#include "RcRegs.h"

//
// CPGC Init modes
//
#define Idle_Mode                           0
#define CPGC_Testing_Mode                   1
#define MRS_Mode                            2
#define NOP_Mode                            3


#define CPGC_ERR_MONITOR_ALL_CL        0xFF // monitor errors on all cachelines
#define CPGC_ERR_MONITOR_NO_CL         0x0  // do not monitor errors on any cacheline
#define CPGC_ERR_MONITOR_ALL_CHUNK     0xFF // monitor errors on all chunks
#define CPGC_ERR_MONITOR_NO_CHUNK      0x0  // do not monitor errors on any chunk
#define CPGC_ERR_NEVER_STOP_ON_ERR     0x0  // Prevent any err from causing a test to stop
#define CPGC_ERR_STOP_ON_ALL_ERR       0x3  // If all data lanes see an err (Data_ Err_Status and ECC_Err_Status) then a test is stopped.

#define CLOCK_PATTERN_DDR3 0xAAAAAAAA
#define CLOCK_PATTERN_DDR4 0xA5A5A5A5

//
// RTL Offset Delay Values
//
#define RTL_OFFSET_0   0
#define RTL_OFFSET_1   2

#define CPGC_BASE_RW_SUBSEQ_CHIP   2 // 0010: Base Read Write
#define CPGC_BASE_WR_SUBSEQ_CHIP   3 // 0011: Base Write Read

void
RPQDrainCadbMRSMode (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank
  );


UINT8
isDdrtDimm(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
);

void
DdrtWriteFlyBy(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT8    *cmd,
UINT32   *seqCtl0
);


void
SetupPmon(  
PSYSHOST        host,
UINT8           socket,
UINT8           ch,
UINT8           eventSelect,
UINT8           unitMask
);

void
CpgcSetup64(
  PSYSHOST host,
  UINT8  socket,
  UINT8  ch,
  UINT8  dimm,
  UINT32 dpa,
  UINT8  mode,
  UINT8  uiBitMask,
  UINT64_STRUCT *data
);

/**

  Setup Data Pattern Configuration and Data Pattern Sequencer registers

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Memory controller number
  @param ch               - channel
  @param sequencerMode    - SequencerMode struct
  @param sequencerPattern - Pointer to array of pattern sequences that are to be programmed

  @retval None

**/
void
SetupPatternSequencer (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         ch,
  SequencerMode sequencerMode,
  UINT32        *sequencerPattern
  );


#endif
