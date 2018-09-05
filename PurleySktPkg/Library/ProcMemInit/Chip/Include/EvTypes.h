/** @file
  This file contains the definition of the BIOS implementation of the BIOS-SSA Common Configuration API.

@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef __EV_TYPES_H__
#define __EV_TYPES_H__

#ifdef SSA_FLAG
#include "MrcSsaServices.h"

///
/// Execution stop mode.
///
typedef enum {
  NeverStopMode,                          ///< Let the test run to completion regardless of errors.
  AnyLaneFailStopMode,                    ///< Stop the test when an error is detected on any lane.
  AllBytesFailStopMode,                   ///< Stop the test when an error is detected on all byte groups.
  AllLanesFailStopMode,                   ///< Stop the test when an error is detected on all lanes.
  StopModeMax,                            ///< STOP_MODE structure maximum value.
  StopModeDelim = INT32_MAX               ///< STOP_MODE structure delimiter value.
} STOP_MODE;

///
/// Memory traffic mode.
///
typedef enum {
  LoopbackTrafficMode,                    ///< This traffic mode produces a simple burst of N cacheline writes followed by a burst on N cacheline reads where N is the corresponding BurstLength parameter value.
  WritesAndReads,                         ///< This traffic mode produces a repeating series of the following sequence of operations: four bursts of write operations followed by four bursts of read operations.
  WriteOnlyTrafficMode,                   ///< This traffic mode produces a series of bursts of write operations.
  ReadOnlyTrafficMode,                    ///< This traffic mode produces a repeating series of bursts of read operations.
  IdleTrafficMode,                        ///< No traffic will be generated for the given channel.
  TrafficModeMax,                         ///< TRAFFIC_MODE structure maximum value.
  TrafficModeDelim = INT32_MAX            ///< TRAFFIC_MODE structure delimiter value.
} TRAFFIC_MODE;

///
/// Memory addressing mode.
///
typedef enum {
  LinearAddrMode,                         ///< Within a burst, the addresses will increment linearly.
  ActiveAddrMode,                         ///< Within a burst, the addresses will increment non-linearly. The idea is to make all of the address signals wiggle.
  AddrModeMax,                            ///< ADDR_MODE structure maximum value.
  AddrModeDelim = INT32_MAX               ///< ADDR_MODE structure delimiter value.
} ADDR_MODE;

///
/// CADB modes.
///
typedef enum {
  NormalCadbMode,                         ///< Normal CADB mode.
  OnDeselectCadbMode,                     ///< On deselect CADB mode.
  AlwaysOnCadbMode,                       ///< Always on CADB mode.
  CadbModeMax,                            ///< CADB mode selection structure maximum value.
  CadbModeDelim = INT32_MAX               ///< CADB mode selection structure delimiter value.
} CADB_MODE;

///
/// Rank combinations
///
typedef enum {
  Singles,                                ///< Single rank selection.
  Turnarounds,                            ///< Turnaround ranks selection.
  RankTestModeMax,                        ///< RANK_TEST_MODE enumeration maximum value.
  RankTestModeDelim = INT32_MAX           ///< RANK_TEST_MODE enumeration delimiter value.
} RANK_TEST_MODE;

///
/// Turnarounds rank test modes
///
typedef enum {
  AllRankTurnarounds,                     ///< Test all rank-to-rank turnarounds.
  OnlyInterDimmTurnarounds,               ///< Only test inter-DIMM rank-to-rank turnarounds.
  OnlyIntraDimmTurnarounds,               ///< Only test intra-DIMM rank-to-rank turnarounds.
  TurnaroundsTestModeMax,                 ///< TURNAROUNDS_TEST_MODE enumeration maximum value.
  TurnaroundsTestModeDelim = INT32_MAX    ///< TURNAROUNDS_TEST_MODE enumeration delimiter value.
} TURNAROUNDS_TEST_MODE;

///
/// DIMM initialization type
///
typedef enum {
  Jedec,                                  ///< JEDEC initialization is performed
  IoReset,                                ///< IO Reset initialization is performed
  DimmInitModeMax,                        ///< DIMM_INIT_MODE enumeration maximum value.
  DimmInitModeDelim = INT32_MAX           ///< DIMM_INIT_MODE enumeration delimiter value.
} DIMM_INIT_MODE;

// from the CpgcPointTest\InitPlatform.h
typedef enum {
  DisableScrambler   = 0,  ///< disable the scrambler during setup; restore it during cleanup
  EnableScrambler    = 1,  ///< enable the scrambler during setup; restore it during cleanup
  DontTouchScrambler = 2,  ///< don't touch the scrambler value during setup and cleanup
  OverrideScramblerMax,    ///< SCRAMBLER_OVERRIDE_MODE enumeration maximum value.
  SCRAMBLER_OVERRIDE_MODE_DELIM = INT32_MAX ///< This value ensures the enum size is consistent on both sides of the PPI.
} SCRAMBLER_OVERRIDE_MODE;

#endif //SSA_FLAG
#endif //__EV_TYPES_H__