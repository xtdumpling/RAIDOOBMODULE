//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Memory Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2017 Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  CoreApi.h


@brief
       This file contains API declarations defined in the MRC Core layer.

**/
#ifndef  _CORE_API_H_
#define  _CORE_API_H_

#include "DataTypes.h"
#include "SysHost.h"
#include <UsraAccessApi.h>
#include "MemFunc.h"

/**

  Memory Controller initialization entry point

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 0 - success (Memory Initialization Complete)
  @retval Other - failure (Did not complete memory initialization)

**/
UINT32
MemStart (
    PSYSHOST host          // Pointer to the system host (root) structure
    );

UINT32
DetermineBootMode(PSYSHOST host
);

UINT32
InitStructuresLate(PSYSHOST host
);
/**

  MRC wrapper code.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
MemoryInit (
    struct sysHost   *host        // Pointer to the system host (root) structure
    );

/**

  Checks to make sure there is still memory available

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval TRUE - Memory is present
  @retval FALSE - Memory is not present

**/
BOOLEAN
CheckMemPresentStatus (
    PSYSHOST host          // Pointer to the system host (root) structure
    );

/**

  Disables the specified channel

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - socket number
  @param ch      - channel number

  @retval None

**/
void
DisableChannel (
    PSYSHOST host,              // Pointer to the system host (root) structure
    UINT8    socket,              // socket number
    UINT8    ch                   // channel number
    );

/**

  Determines the fastest time of the times provided and sets all times to the fastest time.

  @param time1 - 1st time to check
  @param time2 - 2nd time to check
  @param time3 - 3rd time to check (ignored if 0)

  @retval None

**/
void
FastestCommon (
    UINT8 *time1,   // 1st time to check
    UINT8 *time2,   // 2nd time to check
    UINT8 *time3    // 3rd time to check (ignored if 0)
    );

/**

  Determines the fastest time of the 16 bit times provided and sets all times to the fastest time.

  @param time1 - 1st time to check
  @param time2 - 2nd time to check
  @param time3 - 3rd time to check (ignored if 0)

  @retval None

**/
void
FastestCommon16 (
    UINT16 *time1,  // 1st time to check
    UINT16 *time2,  // 2nd time to check
    UINT16 *time3   // 3rd time to check (ignored if 0)
    );

/**

  Returns a bitmask of channels that have the current dimm and rank enabled

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - socket number
  @param dimm    - dimm number
  @param rank    - rank number

  @retval chBitmask - Bitmask of channels present for the current rank

**/
UINT32
GetChBitmask (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // socket number
    UINT8    dimm,      // dimm number
    UINT8    rank       // rank number
    );

/**

  Returns a bitmask of channels that are enabled

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket number

  @retval chBitmask - Bitmask of enabled channels

**/
UINT32
GetChBitmaskAll (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket     // socket number
    );

/**

  Get the closest supported frequency

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket number

  @retval frequency

**/
UINT8
GetClosestFreq (
    PSYSHOST host,              // Pointer to the system host (root) structure
    UINT8    socket               // socket number
    );

/**

  Returns a bit mask of DIMMs present.

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Processor socket to check
  @param dimmMask  - Pointer to results

  @retval None

**/
void
GetDimmPresent (
    PSYSHOST host,                    // Pointer to the system host (root) structure
    UINT8     socket,                     // Processor socket to check
    UINT8     *dimmMask[MAX_CH]   // Pointer to results
    );

/**

  Returns a bit mask of logical ranks present.

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Processor socket to check
  @param rankMask  - Pointer to results

  @retval None

**/
void
GetRankPresent (
    PSYSHOST host,                    // Pointer to the system host (root) structure
    UINT8     socket,                     // Processor socket to check
    UINT8     *rankMask[MAX_CH]     // Pointer to results
    );

/**

  Wrapper to Setup Serial Debug Messages

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
SetupSerialDebugMessages(
PSYSHOST host               // Pointer to sysHost, the system host (root) structure
);

/**

  Wrapper to Restore Serial Debug Messages

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
RestoreSerialDebugMessages(
  PSYSHOST host               // Pointer to sysHost, the system host (root) structure
    );

/**

  Runs Memory Initialization

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
MemInit (
    PSYSHOST host               // Pointer to the system host (root) structure
    );

/**

  Runs optimized Memory Init = 0 with DDR scrambler enabled

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - Processor socket to check
  @param ddrChEnabled - Bitmask of channels that are enabled

  @retval SUCCESS
  @retval Error Status

**/
UINT32
MemInitOpt (
        PSYSHOST host,         // Pointer to the system host (root) structure
        UINT8    socket,       // Processor socket to check
        UINT32   ddrChEnabled  // Bitmask of channels that are enabled
        );

/**

  Detect DIMM population

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
DetectDIMMConfig (
    PSYSHOST host               // Pointer to the system host (root) structure
    );


/**

  MRC Wrapper code

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - socket number
  @param dev         - SMB device structure
  @param byteOffset  - offset of data to read
  @param data        - Pointer to data to be returned

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
UINT8
MemReadSmb (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,         // socket number
    struct smbDevice  dev,            // SMB device structure
    UINT8               byteOffset,     // offset of data to read
    UINT8               *data           // Pointer to data to be returned
    );

/**

  MRC wrapper code

  @param host        - Pointer to sysHost, the system host (root) structure
  @param dev         - SMB device structure
  @param byteOffset  - offset of data to write
  @param data        - Pointer to data to be written

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
UINT8
MemWriteSmb (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    struct smbDevice  dev,            // SMB device structure
    UINT8               byteOffset,     // offset of data to read
    UINT8               *data           // Pointer to data to be returned
    );

/**

  Converts MTB/FTB to DCLK

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Processor socket within the system (0-based)
  @param ch       - DDR channel number within the processor socket (0-based)
  @param dimm     - DIMM number within a DDR channel (0-based, farthest slot == 0)
  @param mtbTime  - Time in MTB (Medium Time Base)
  @param ftbTime  - Time in FTB (Fine Time Base)

  @retval Time in DCLKs

**/
UINT16
TimeBaseToDCLK (
    PSYSHOST   host,              // Pointer to host structure (outside scope of this spec)
    UINT8   socket,             // Processor socket within the system (0-based)
    UINT8       ch,                 // DDR channel number within the processor socket (0-based)
    UINT8     dimm,       // DIMM number within a DDR channel (0-based, farthest slot == 0)
    UINT16      mtbTime,    // Time in MTB (Medium Time Base)
    UINT16      ftbTime     // Time in FTB (Fine Time Base)
    );

/**

  Detect DDR3 configuration

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Processor socket within the system (0-based)
  @param ch     - DDR channel number within the processor socket (0-based)
  @param dimm   - DIMM number within a DDR channel (0-based, farthest slot == 0)

  @retval SUCCESS

**/
UINT32
DetectDDR3DIMMConfig (
    PSYSHOST    host, // Pointer to host structure (outside scope of this spec)
    UINT8     socket, // Processor socket within the system (0-based)
    UINT8     ch, // DDR channel number within the processor socket (0-based)
    UINT8     dimm // DIMM number within a DDR channel (0-based, farthest slot == 0)
    );

/**

  Collects data from SPD for DDR3

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
GatherSPDDataDDR3 (
    PSYSHOST host               // Pointer to the system host (root) structure
    );

/**

  Init SPD page select and cached values

  @param host       - Pointer to sysHost
  @param socket     - Socket ID

  @retval none

**/
VOID
InitSpd (
  PSYSHOST         host,                        // Pointer to sysHost, the system host (root) structure
  UINT8            socket                       // Socket Id
  );

/**

  Read SPD byte from Serial EEPROM

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket ID
  @param ch         - Channel on socket
  @param dimm       - DIMM on channel
  @param byteOffset - Byte Offset to read
  @param data       - Pointer to data

  @retval status

**/
UINT32
ReadSpd (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      byteOffset,                       // Byte Offset to read
  UINT8       *data                             // Pointer to data
  );

/**

  Initialize the ddrVoltage based on setup.....DDR3 Only

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket number

  @retval None

**/
void
InitDDRVoltageDDR3 (
    PSYSHOST  host,               // Pointer to the system host (root) structure
    UINT8     socket        // socket number
    );

/**

  Detect DDR4 configuration

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket within the system (0-based)
  @param ch      - DDR channel number within the processor socket (0-based)
  @param dimm    - DIMM number within a DDR channel (0-based, farthest slot == 0)

  @retval SUCCESS

**/
UINT32
DetectDDR4DIMMConfig (
    PSYSHOST    host, // Pointer to host structure (outside scope of this spec)
    UINT8     socket, // Processor socket within the system (0-based)
    UINT8     ch, // DDR channel number within the processor socket (0-based)
    UINT8     dimm // DIMM number within a DDR channel (0-based, farthest slot == 0)
    );

/**

  Initialize DDR4 voltage

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket number

  @retval None

**/
void
InitDDRVoltageDDR4 (
    PSYSHOST  host,               // Pointer to the system host (root) structure
    UINT8     socket        // socket number
    );

/**

  Detect DIMM population

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Processor socket within the system (0-based)
  @param ch     - DDR channel number within the processor socket (0-based)
  @param dimm   - DIMM number within a DDR channel (0-based, farthest slot == 0)

  @retval SUCCESS

**/
UINT32
DetectAepDIMMConfig (
    PSYSHOST    host, // Pointer to host structure (outside scope of this spec)
    UINT8     socket, // Processor socket within the system (0-based)
    UINT8     ch, // DDR channel number within the processor socket (0-based)
    UINT8     dimm // DIMM number within a DDR channel (0-based, farthest slot == 0)
    );

/**

  Runs MemTest

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
MemTest (
    PSYSHOST host               // Pointer to the system host (root) structure
    );

/**

  Runs Advanced Memory Test algorithms with CPGC and logs any failures

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval - SUCCESS

**/
UINT32
AdvancedMemTest (
    PSYSHOST host               // Pointer to the system host (root) structure
    );

/**

  Runs the CMD Signal testing

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Processor socket within the system (0-based)
  @param chBitmask  - List of channels that were tested

  @retval None

**/
void
ExecuteCmdSignalTest (
    PSYSHOST  host,   // Pointer to host structure (outside scope of this spec)
    UINT8     socket,   // Processor socket within the system (0-based)
    UINT32    chBitmask          // List of channels that were tested
    );

/**

  Aggressive CMD test to get a per platform group centering of CMD

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval SUCCESS

**/
UINT32
AggressiveCMDTest (
    PSYSHOST host,                    // Pointer to the system host (root) structure
    UINT8     socket                                        // Socket Id
    );

/**

  Per CMD Group Margin and Group Alignment

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param testCase - Test Case: \n  1 =  COMPOSITE_GROUPS\n  0 = Group 0

  @retval UINT32

**/
UINT32
PerCMDGroup (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8     socket,   // Socket Id
    UINT16    testCase  // Test Case:
    //  1 =  COMPOSITE_GROUPS
    //   0 = Group 0
    );

/**

  Executes a memory test for advance trainings

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket Id
  @param ddrChEnMap    - Bit-mask of enabled channels to test:\n 1 = Enabled\n 0 = Skipped
  @param dimm          - Target DIMM to test
  @param rank          - Target Rank to test
  @param group         - Parameter to be margined: \n
                           RxDqsDelay    : Margin Rx DqDqs \n
                           TxDqDelay     : Margin Tx DqDqs \n
                           RxVref         : Margin Rx Vref \n
                           TxVref         : Margin Tx Vref \n
                           RecEnDelay    : Margin Receive Enable \n
                           WrLvlDelay    : Margin Write Level \n
                           CmdGrp0        : Margin CMD group 0 \n
                           CmdGrp1        : Margin CMD group 1 \n
                           CmdGrp2        : Margin CMD group 2 \n
                           CmdAll         : Margin all CMD groups
  @param mode          - Margin mode to use: \n
                           MODE_XTALK            BIT0: Enable crosstalk (placeholder) \n
                           MODE_VIC_AGG          BIT1: Enable victim/aggressor \n
                           MODE_START_ZERO       BIT2: Start at margin 0 \n
                           MODE_POS_ONLY         BIT3: Margin only the positive side \n
                           MODE_NEG_ONLY         BIT4: Margin only the negative side \n
                           MODE_DATA_MPR         BIT5: Enable MPR Data pattern \n
                           MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern \n
                           MODE_DATA_LFSR        BIT7: Enable Data LFSR \n
                           MODE_ADDR_LFSR        BIT8: Enable Address LFSR \n
                           MODE_CHECK_PARITY     BIT10: Enable parity checking \n
                           MODE_DESELECT         BIT11: Enable deselect patterns \n
                           MODE_VA_DESELECT BIT12: Enable Victim - Aggressor deselect
  @param patternLength - Number of sequences to test (units of burstLength cache lines)
  @param burstLength   - Number of cachelines to test for each write/read operation.\n
                           It is the repeat count for the WR/RD subsequence

  @retval None

**/
UINT32
ExecuteTest (
    PSYSHOST host,              // Pointer to the system host (root) structure
    UINT8     socket,     // Socket Id
    UINT32    ddrChEnMap, // Bit-mask of enabled channels to test. 1 = Enabled, 0 = Skipped
    UINT8     dimm,               // Target DIMM to test
    UINT8     rank,               // Target Rank to test
    GSM_GT    group,            // Parameter to be margined
    //   RxDqsDelay    : Margin Rx DqDqs
    //   TxDqDelay     : Margin Tx DqDqs
    //   RxVref         : Margin Rx Vref
    //   TxVref         : Margin Tx Vref
    //   RecEnDelay    : Margin Receive Enable
    //   WrLvlDelay    : Margin Write Level
    //   CmdGrp0        : Margin CMD group 0
    //   CmdGrp1        : Margin CMD group 1
    //   CmdGrp2        : Margin CMD group 2
    //   CmdAll         : Margin all CMD groups
    UINT16    mode,               // Margin mode to use
    //   MODE_XTALK            BIT0: Enable crosstalk (placeholder)
    //   MODE_VIC_AGG          BIT1: Enable victim/aggressor
    //   MODE_START_ZERO       BIT2: Start at margin 0
    //   MODE_POS_ONLY         BIT3: Margin only the positive side
    //   MODE_NEG_ONLY         BIT4: Margin only the negative side
    //   MODE_DATA_MPR         BIT5: Enable MPR Data pattern
    //   MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
    //   MODE_DATA_LFSR        BIT7: Enable Data LFSR
    //   MODE_ADDR_LFSR        BIT8: Enable Address LFSR
    //   MODE_ADDR_CMD0_SETUP  BIT9
    //   MODE_CHECK_PARITY     BIT10: Enable parity checking
    //   MODE_DESELECT         BIT11: Enable deselect patterns
    //   MODE_VA_DESELECT BIT12: Enable Victim - Aggressor deselect
    // patterns
    UINT32   patternLength,                   // Number of sequences to test (units of burstLength cache lines)
    UINT8     burstLength                     // Number of cachelines to test for each write/read operation.
    //   It is the repeat count for the WR/RD subsequence
    );

/**

  Checks the results of the preceding memory test

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param chBitmask        - List of channels that were tested
  @param chBitmaskOut     - List of channels that were tested and still have rank enabled
  @param dimmRank         - DIMM containing logicalRank
  @param logical2Physical - Geometery for logical rank within channel
  @param logicalRank      - Logical rank
  @param logicalSubRank   - SubRank
  @param bank             - bank
  @param secondPass       - Second Pass check: 0 = 1st Pass, 1 = 2nd Pass

  @retval SUCCESS
  @retval FAILURE

**/
UINT8
CheckTestResults (
    PSYSHOST host,                    // Pointer to the system host (root) structure
    UINT8     socket,                                         // Socket Id
    UINT32    chBitmask,                                      // List of channels that were tested
    UINT32   *chBitmaskOut,                                   // List of channels that were tested and still have rank enabled
    UINT8     dimmRank[MAX_RANK_CH][MAX_CH],  // DIMM containing logicalRank
    UINT8     logical2Physical[MAX_RANK_CH][MAX_CH],  // Geometery for logical rank within channel
    UINT8     logicalRank,                                    // Logical rank
  UINT8     logicalSubRank,                                 // SubRank
  UINT8     bank,                                           // bank
    BOOLEAN   secondPass                                    // Second Pass check: 0 = 1st Pass, 1 = 2nd Pass
    );

/**

  Checks the results of the preceding advanced memory test

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param chBitmask        - List of channels that were tested
  @param dimmRank         - DIMM containing logicalRank
  @param logical2Physical - Geometery for logical rank within channel
  @param logicalRank      - Logical rank
  @param testType         - Memory test type for logging purposes

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CheckTestResultsMATS (
    PSYSHOST host,                                            // Pointer to the system host (root) structure
    UINT8     socket,                                         // Socket Id
    UINT32    chBitmask,                                      // List of channels that were tested
    UINT8     dimmRank[MAX_RANK_CH][MAX_CH],                  // DIMM containing logicalRank
    UINT8     logical2Physical[MAX_RANK_CH][MAX_CH],          // Geometery for logical rank within channel
    UINT8     logicalRank,                                    // Logical rank
    UINT8     testType                                        // Memory test type for logging purposes
    );

/**

  Collects the results of the previous test

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Processor socket within the system (0-based)
  @param chToCheck - channel to check
  @param bwSerr    - Test error status; 72 bits per channel for ECC modules

  @retval SUCCESS
  @retval Error Code - Non-zero if any failure was detected

**/
UINT32
CollectTestResults (
    PSYSHOST  host,             // Pointer to host structure (outside scope of this spec)
    UINT8     socket,           // Processor socket within the system (0-based)
    UINT32    chToCheck,        // channel to check
    UINT32    bwSerr[MAX_CH][3] // Test error status; 72 bits per channel for ECC modules
    );

/**

  Re-centers read dq timing using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RdAdvancedCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Re-centers tx dq timing using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WrAdvancedCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RdVrefCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Re-centers using a more robust test

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WrVrefCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );


/**

  Early Vref centering

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RdEarlyVrefCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Early Vref centering

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WrEarlyVrefCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Re-centers using a more robust test

  @param host    - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
CmdVrefCentering (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  VRef Centering for LRDIMM (DDR4)

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Processor socket to check
  @param group            - Parameter to be margined
  @param level            - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param perRankCentering - 0 = false, 1 = true (if false, PDA training is done)

  @retval SUCCESS

**/
UINT32
LRDIMMAdvCentering (
    PSYSHOST  host,    // Pointer to host structure (outside scope of this spec)
    UINT8     socket,             // Processor socket to check
    GSM_GT    group,      // Parameter to be margined
    GSM_LT    level,            // DDR or LRDIMM Host side, LRDIMM Backside, etc...
    UINT8     perRankCentering // 0 = false, 1 = true (if false, PDA training is done)
    );

/**

  Perform Write Leveling training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WriteLeveling (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Perform Write Leveling Cleanup.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WriteLevelingCleanUp (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Initializes the starting margins

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check

  @retval None

**/
void
InitStartingOffset (
    PSYSHOST  host,  // Pointer to host structure (outside scope of this spec)
    UINT8     socket // Processor socket to check
    );

/**

  Per Bit Deskew for the requested group

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param group   - Parameter to center \n
                     RxDqsDelay   0: Center Rx DqDqs \n
                     TxDqDelay    1: Center Tx DqDqs

  @retval None

**/
void
PerBitDeskew (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  GSM_GT      group                             // Parameter to center
                                                //   RxDqsDelay   0: Center Rx DqDqs
                                                //   TxDqDelay    1: Center Tx DqDqs
);

/**

  Receive Per Bit Deskew

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RxPerBitDeskew (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Tx Per Bit Deskew

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
TxPerBitDeskew (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Perform main loop of READ DqDqs algorithm.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Status

**/
UINT32
ReadDqDqs (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Perform Write DqDqs Training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
WrDqDqs (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Perform Write DqDqs Per Bit Training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Status

**/
UINT32
WriteDqDqsPerBit (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Detects DQ swizzling on board routing for NVM DIMMs

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
DqSwizzleDiscovery (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  The function gets number of error samples and finds the middle of zero error

  @param host              - Pointer to sysHost, the system host (root) structure
  @param socket            - Processor socket to check
  @param ch                - Current Channel
  @param dimm              - Dimm number (0-based)
  @param rank              - Rank number (0-based)
  @param strobe            - Strobe number (0-based)
  @param SdRamErrorSamples - Array with 64 sampes for each dqs pi setting
  @param ErrorSamplesSize  - Include how many sample we have in the array 72 or 64
  @param errorStart        - Pointer to location for error start
  @param errorEnd          - Pointer to location for error end
  @param piPosition        - Pointer to the PI Position

  @retval Status

**/
UINT32
FindErrorGraphMin (
    PSYSHOST  host,                 // Pointer to the system host (root) structure
    UINT8     socket,               // Processor socket to check
    UINT8     ch,                   // Current Channel
    UINT8     dimm,                 // Dimm number (0-based)
    UINT8     rank,                 // Rank number (0-based)
    UINT8     strobe,                   // Strobe number (0-based)
    UINT8     *SdRamErrorSamples,     // Array with 64 sampes for each dqs pi setting
    UINT16    ErrorSamplesSize,         // Include how many sample we have in the array 72 or 64
    UINT16    *errorStart,              // Pointer to location for error start
    UINT16    *errorEnd,                // Pointer to location for error end
    UINT16    *piPosition               // Pointer to the PI Position
    );

/**

  Perform Receive Enable Training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
ReceiveEnable (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Optimizes round trip latency

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RoundTripOptimize (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  This function find the logic delay max and logic delay min

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Processor socket to check
  @param ch              - Current Channel
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param MaxSdramInDimm  - Number of sdram in the current DIMM
  @param MaxLogicDelay   - Return logic delay max value
  @param MinLogicDelay   - Return logic delay min value

  @retval None

**/
void
FindMaxMinLogic (
    PSYSHOST  host,                 // Pointer to the system host (root) structure
    UINT8     socket,               // Processor socket to check
    UINT8     ch,                   // Current Channel
    UINT8     dimm,                 // Dimm number (0-based)
    UINT8     rank,                 // Rank number (0-based)
    UINT8     MaxSdramInDimm,     // Number of sdram in the current DIMM
    UINT8     *MaxLogicDelay,     // Return logic delay max value
    UINT8     *MinLogicDelay      // Return logic delay min value
    );

/**

  Checks if XMP is supported by all sockets

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
ConfigureXMP (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Wait for BSP to send AP continue handshake

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32 WaitForConfigureXMP (
    PSYSHOST host     // Pointer to the system host (root) structure
    );

/**

  This function calculates the number of bits set to 1 in a 32-bit value

  @param Input - The 32 bit value to check

  @retval The number of bits set to 1 in Input

**/
UINT8
CountBitsEqOne (
    UINT32 Input    // The 32 bit value to check
    );

/**

  Wrapper code to call an OEM hook to change the socket Vddq

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - processor number

  @retval SUCCESS if the Vddq change was made
  @retval FAILURE if the Vddq change was not made

**/
UINT8
CoreSetVdd (
    PSYSHOST host,    // Pointer to host structure (outside scope of this spec)
    UINT8    socket   // processor number
    );

/**

  Wrapper code to call an OEM platform hook call to release any platform clamps affecting CKE and/or DDR Reset

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - processor number

  @retval None

**/
void
CoreReleaseADRClamps (
    PSYSHOST host,    // Pointer to host structure (outside scope of this spec)
    UINT8 socket      // processor number
    );

/**

  Wrapper code to call an OEM hook to restrict supported configurations if required.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
CoreCheckPORCompat (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Wrapper code to call an OEM hook for overriding the memory POR frequency table

  @param host             - Pointer to sysHost, the system host (root) structure
  @param freqTablePtr     - Pointer to Intel POR memory frequency table.
  @param freqTableLength  - Pointer to number of DimmPOREntryStruct entries in

  @retval None

**/
void
CoreLookupFreqTable (
    PSYSHOST host,                                // Pointer to host structure (outside scope of this spec)
    struct DimmPOREntryStruct **freqTablePtr,     // Pointer to Intel POR memory frequency table.
    UINT16 *freqTableLength                       // Pointer to number of DimmPOREntryStruct entries in
    //    the POR Frequency Table.
    );

#ifdef MEM_NVDIMM_EN
/**

  Determine if NVDIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket within the system (0-based)
  @param ch      - DDR channel number within the processor socket (0-based)
  @param dimm    - DIMM number within a DDR channel (0-based, farthest slot == 0)
  @param spd     - Structure holding the SPD data

  @retval 0 - Not NVDIMM
  @retval Non-zero - Is NVDIMM

**/
UINT32
CoreDetectNVDIMM (
    PSYSHOST  host,             // Pointer to host structure (outside scope of this spec)
    UINT8     socket,           // Processor socket within the system (0-based)
    UINT8     ch,               // DDR channel number within the processor socket (0-based)
    UINT8     dimm,             // DIMM number within a DDR channel (0-based, farthest slot == 0)
    struct smbDevice  spd       // Structure holding the SPD data
);

/**

  Check NVDIMM status

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
CoreNVDIMMStatus (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Restore NVDIMM

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
CoreRestoreNVDIMMs (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Arm NVDIMM

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
CoreArmNVDIMMs (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
);
#endif

/**

  Core call to platform specific input setup

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
CoreGetSetupOptions (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Core call to platform Processor specific input setup

  @param setup - pointer to sysSetup, the setup area of sysHost structure

  @retval None

**/
void
CoreGetCpuSetupOptions (
    struct sysSetup *setup  // pointer to setup area of host structure
    );

/**

  Calls platform specific method to program input parameters for the MRC

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
GetMemSetupOptionsCore (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Wrapper code to call the platform specific CheckAndHandleResetRequests routine

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
OemCheckAndHandleResetRequestsCore (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Wrapper code to call the platform specific OemPublishDataForPost routine

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
OemPublishDataForPostCore (
    PSYSHOST host   // Pointer to host structure (outside scope of this spec)
    );

/**

  Issues the requested platform reset

  @param host      - Pointer to sysHost, the system host (root) structure
  @param resetType - Reset type

  @retval None

**/
void
ResetSystem (
    PSYSHOST host,   // Pointer to host structure (outside scope of this spec)
    UINT8    resetType    // Reset type
    );

/**

  Set the DDR clock frequency

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval SUCCESS - Clock is set properly
  @retval FAILURE - A reset is required to set the clock

**/
UINT32
SetDDRFreq (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket   // Socket Id
    );

/**

  Calculates positive and negative margin for the group provided

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket Id
  @param level         - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group         - Parameter to be margined: \n
                           RxDqsDelay     : Margin Rx DqDqs \n
                           TxDqDelay      : Margin Tx DqDqs \n
                           RxVref         : Margin Rx Vref \n
                           TxVref         : Margin Tx Vref \n
                           RecEnDelay     : Margin Receive Enable \n
                           WrLvlDelay     : Margin Write Level \n
                           CmdGrp0        : Margin CMD group 0 \n
                           CmdGrp1        : Margin CMD group 1 \n
                           CmdGrp2        : Margin CMD group 2 \n
                           CmdAll         : Margin all CMD groups
  @param mode          - Test mode to use: \n
                           MODE_XTALK            BIT0: Enable crosstalk (placeholder) \n
                           MODE_VIC_AGG          BIT1: Enable victim/aggressor \n
                           MODE_START_ZERO       BIT2: Start at margin 0 \n
                           MODE_POS_ONLY         BIT3: Margin only the positive side \n
                           MODE_NEG_ONLY         BIT4: Margin only the negative side \n
                           MODE_DATA_MPR         BIT5: Enable MPR Data pattern \n
                           MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern \n
                           MODE_DATA_LFSR        BIT7: Enable Data LFSR \n
                           MODE_ADDR_LFSR        BIT8: Enable Address LFSR \n
                           MODE_CHECK_PARITY     BIT10: Enable parity checking \n
                           MODE_DESELECT         BIT11: Enable deselect patterns \n
                           MODE_VA_DESELECT BIT12: Enable Victim - Aggressor deselect
  @param scope         - Margin granularity: \n
                           SCOPE_SOCKET    0: Margin per processor socket \n
                           SCOPE_CH        1: Margin per channel \n
                           SCOPE_DIMM      2: Margin per DIMM \n
                           SCOPE_RANK      3: Margin per rank \n
                           SCOPE_STROBE    4: Margin per strobe group \n
                           SCOPE_BIT       5: Margin per bit
  @param mask          - Mask of bits to exclude from testing
  @param marginData    - Pointer to the structure to store the margin results. The structure type varies based on scope.
  @param patternLength - Number of sequences to test (units of burstLength cache lines)
  @param update        - Update starting margins so the next pass will run faster or not
  @param chMask        - Bit Mask of channels to not be used
  @param rankMask      - Bit Mask of ranks to not be used
  @param burstLength   - Number of cachelines to test for each write/read operation.\n
                           It is the repeat count for the WR/RD subsequence

  @retval None

**/
void
GetMargins(
  PSYSHOST        host,             // Pointer to sysHost, the system host (root) structure
    UINT8    socket,    // Socket Id
    GSM_LT          level,  // DDR or LRDIMM Host side, LRDIMM Backside, etc...
    GSM_GT          group,    // Parameter to be margined:
    //   RxDqsDelay    : Margin Rx DqDqs
    //   TxDqDelay     : Margin Tx DqDqs
    //   RxVref         : Margin Rx Vref
    //   TxVref         : Margin Tx Vref
    //   RecEnDelay    : Margin Receive Enable
    //   WrLvlDelay    : Margin Write Level
    //   CmdGrp0        : Margin CMD group 0
    //   CmdGrp1        : Margin CMD group 1
    //   CmdGrp2        : Margin CMD group 2
    //   CmdAll         : Margin all CMD groups
    UINT16          mode,   // Test mode to use
    //   MODE_XTALK            BIT0: Enable crosstalk (placeholder)
    //   MODE_VIC_AGG          BIT1: Enable victim/aggressor
    //   MODE_START_ZERO       BIT2: Start at margin 0
    //   MODE_POS_ONLY         BIT3: Margin only the positive side
    //   MODE_NEG_ONLY         BIT4: Margin only the negative side
    //   MODE_DATA_MPR         BIT5: Enable MPR Data pattern
    //   MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
    //   MODE_DATA_LFSR        BIT7: Enable Data LFSR
    //   MODE_ADDR_LFSR        BIT8: Enable Address LFSR
    //   MODE_CHECK_PARITY     BIT10: Enable parity checking
    //   MODE_DESELECT         BIT11: Enable deselect patterns
    //   MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns
    UINT8           scope,    // Margin granularity
    //   SCOPE_SOCKET      0: Margin per processor socket
    //   SCOPE_CH        1: Margin per channel
    //   SCOPE_DIMM      2: Margin per DIMM
    //   SCOPE_RANK      3: Margin per rank
    //   SCOPE_STROBE    4: Margin per strobe group
    //   SCOPE_BIT       5: Margin per bit
    struct bitMask *mask, // Mask of bits to exclude from testing
    void *marginData,       // Pointer to the structure to store the margin results. The structure type varies // based on scope.
    UINT32          patternLength,    // RankMarginTest Pattern length
    UINT8           update,           // Update starting margins so the next pass will run faster or not
    UINT8           chMask,           // Bit Mask of channels to not be used
    UINT8           rankMask,         // Bit Mask of ranks to not be used
    UINT8           burstLength       // Number of cachelines to test for each write/read operation.  It is the
    // repeat count for the WR/RD subsequence
    );

/**

  Finds margins for various parameters per rank

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
RankMarginTool (
    PSYSHOST  host                // Pointer to the system host (root) structure
    );

/**

  Set the Offset

  @param host           - Pointer to sysHost, the system host (root) structure
  @param socket         - Processor socket to check
  @param ch             - Current Channel
  @param dimm           - Dimm number (0-based)
  @param rank           - Rank number (0-based)
  @param offset         - Pointer to offset
  @param level          - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group          - Test group
  @param previousMargin - Previous Margin

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
SetOffset (
    PSYSHOST  host,               // Pointer to the system host (root) structure
    UINT8     socket,             // Processor socket to check
    UINT8     ch,                 // Current Channel
    UINT8     dimm,               // Dimm number (0-based)
    UINT8     rank,               // Rank number (0-based)
    INT16     offset,             // Pointer to offset
    GSM_LT    level,              // DDR or LRDIMM Host side, LRDIMM Backside, etc...
    GSM_GT    group,            // Test group
    INT16     previousMargin    // Previous Margin
    );

/**

  Restore Offset

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - channel number
  @param dimm    - DIMM number
  @param rank    - rank number
  @param offset  - Offset
  @param level   - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group   - Test group

  @retval None

**/
void
RestoreOffset (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8     ch,       // channel number
    UINT8     dimm,     // DIMM number
    UINT8     rank,     // rank number
    INT16     offset,   // Offset
    GSM_LT    level,    // DDR or LRDIMM Host side, LRDIMM Backside, etc...
    GSM_GT    group     // Test group
    );

/**



  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - CPU Socket Node number (Socket ID)
  @param ch        - Channel number (0-based)
  @param dimm      - Current dimm
  @param rank      - Rank number (0-based)
  @param scope     - Margin granularity
  @param ranksDone - Array of bits showing if rans are done. \n
                       If the bit is set the rank is done

  @retval None

**/
void
EvalResSetRanksDone (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT8       scope,                            // Margin granularity
  UINT32      *ranksDone                        // Array of bits showing if rans are done.
                                                //   If the bit is set the rank is done
  );

/**

  Function to offset the final training results before entering the OS that would facilitate customer debug.

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Processor socket to check
  @param ch              - Current Channel
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param offset          - Pointer to offset
  @param level           - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group           - Test group

  @retval None

**/
void
TrainingResultOffsetFunction (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch,
    UINT8     dimm,
    UINT8     rank,
    INT16     offset,
    GSM_LT    level,
    GSM_GT    group
    );

/**

  Displays per bit margin results

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - current socket
  @param resultsBit  - Per bit margin results
  @param group       - Command group

  @retval None

**/
void
DisplayPerBitMargins (
    PSYSHOST          host,           // Pointer to the system host (root) structure
    UINT8             socket,         // current socket
    struct bitMargin  *resultsBit,    // Per bit margin results
    GSM_GT            group,          // Command group
    GSM_LT            level           // DDR or LRDIMM Host side, LRDIMM Backside, etc...
    );

/**

  Display Right Edge and Left Edge margin results

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - current socket
  @param current_rank  - Rank number (0-based)
  @param resultsBit    - Per bit margin results
  @param group         - Command group

  @retval None

**/
void
DisplayREandLEMargins (
  PSYSHOST          host,         // Pointer to sysHost, the system host (root) structure
  UINT8             socket,       // current socket
  UINT8               ch,
  UINT8               dimm,
  UINT8               rank,
  struct bitMarginCh  *resultsBit,
  GSM_GT            group         // Command group
  );

/**

  Clears GetMargins filter

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id
  @param filter - pointer to filter buffer

  @retval None

**/
void
ClearFilter (
    PSYSHOST host,        // Pointer to the system host (root) structure
    UINT8           socket,     // Socket Id
    struct bitMask  *filter     // pointer to filter buffer
    );

/**

  Gets the variables needed to calculate turnaround times

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param ttVars  - Structure that holds the variables

  @retval None

**/
void
GetTTVar (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function returns the value of tCK

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tCK

**/
UINT32
GettCK (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tREFI x 9

  @param host  - Pointer to sysHost, the system host (root) structure
  @param tREFI - Refresh rate

  @retval tREFI x 9

**/
UINT32
GettREFIx9 (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT16    tREFI     // Refresh rate
    );

/**

  This function returns the value of tREFI

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Current channel

  @retval tREFI

**/
UINT32
GettREFI (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket,   // Socket Id
    UINT8     ch        // Current channel
    );

/**

  This function returns the value of tCKE

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tCKE

**/
UINT32
GettCKE (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tXPDLL

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tXPDLL

**/
UINT32
GettXPDLL (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tXSOFFSET

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tXSOFFSET

**/
UINT32
GettXSOFFSET (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tXP

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tXP

**/
UINT32
GettXP (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tCKSRX

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tCKSRX

**/
UINT32
GettCKSRX (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tSTAB

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tSTAB

**/
UINT32
GettSTAB (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tMOD

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tMOD

**/
UINT32
GettMOD (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tFAW

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param pageSize  - Page size

  @retval tFAW

**/
UINT32
GettFAW (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket,   // Socket Id
    UINT8     pageSize  // Page size
    );

/**

  This function returns the value of tRTP

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tRTP

**/
UINT32
GettRTP (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tWTR

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tWTR

**/
UINT32
GettWTR (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tWTR_S

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tWTR_S

**/
UINT32
GettWTR_S (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tWR

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tWR

**/
UINT32
GettWR (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket    // Socket Id
    );

/**

  This function returns the value of tRFC

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRFC

**/
UINT32
GettRFC (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tRAS

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRAS

**/
UINT32
GettRAS (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tCWL

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tCWL

**/
UINT32
GettCWL (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tCL

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tCL

**/
UINT32
GettCL (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tRP

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRP

**/
UINT32
GettRP (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tRCD

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRCD

**/
UINT32
GettRCD (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/*++

This function returns the value of tRC

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRC

--*/
UINT32
GettRC(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/**

  This function returns the value of tRRD_L

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRRD_L

**/
UINT32
GettRRD_L (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tRRD

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tRRD

**/
UINT32
GettRRD (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch
    );

/**

  This function returns the value of tWTR_L

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval tWTR_L

**/
UINT32
GettWTR_L (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  );

/*++

This function returns the value of tPRPDEN

@param host    - Point to sysHost
@param socket  - Socket Id

@retval UINT32 Value of tWTR_L

--*/

UINT32
GettPRPDEN (
PSYSHOST  host,
UINT8     socket
);

/*++

This function returns the value of tRDA (RdCAS_AP to ACT)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRDA

--*/
UINT32
GettRDA(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/*++

This function returns the value of tWRA (WrCAS_AP to ACT)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRA

--*/
UINT32
GettWRA(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/*++

This function returns the value of tWRPRE (WrCAS to PRE)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRPRE

--*/
UINT32
GettWRPRE(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/*++

This function returns the value of tRDPDEN (RdCAS to CKE low time)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tRDPDEN

--*/
UINT32
GettRDPDEN(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/*++

This function returns the value of tWRPDEN (WrCAS to CKE low time)

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tWRPDEN

--*/
UINT32
GettWRPDEN(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
);

/**

  This function ensures we do not overflow

  @param host    - Point to sysHost
  @param timing  - Timing parameter
  @param value   - Pointer to the value to check

  @retval None

**/
void
CheckBounds (
  PSYSHOST        host,
  MEM_TIMING_TYPE timing,
  UINT32          *value
);

/**

  This function returns the value of tCCD_L

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval tCCD_L

**/
UINT8
GetTccd_L (
    PSYSHOST  host,       // Pointer to the system host (root) structure
    UINT8     socket,    // Socket Id
    UINT8     ch
    );

/**

  This function calculates the write to write turnaround time for different DIMMs


  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Processor socket to check
  @param ch       - Current Channel
  @param minWWDR  - Minimum value for WWDD
  @param ttVars   - Structure that holds the variables

  @retval tWWDD

**/
UINT8
CalctWWDD (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    UINT8       minWWDR,  // Minimum value for WWDD
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the read to write turnaround time for the same rank

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param specMin - Minimum specified value for RWDR
  @param ttVars  - Structure that holds the variables

  @retval rRWSR

**/
UINT8
CalctRWSR (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    UINT32  specMin,  // Minimum specified value for RWDR
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the read to write turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param specMin - Minimum specified value for RWDR
  @param ttVars  - Structure that holds the variables

  @retval tRWDR

**/
UINT8
CalctRWDR (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    UINT32  specMin,  // Minimum specified value for RWDR
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the read to write turnaround time for different DIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param ttVars  - Structure that holds the variables

  @retval tRWDD

**/
UINT8
CalctRWDD (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the write to read turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param ttVars  - Structure that holds the variables

  @retval tWRDR

**/
UINT8
CalctWRDR (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the write to read turnaround time for different DIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param ttVars  - Structure that holds the variables

  @retval tWRDD

**/
UINT8
CalctWRDD (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  This function calculates the write to write turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket to check
  @param ch      - Current Channel
  @param minWWDR - Minimum value for WWDR
  @param ttVars  - Structure that holds the variables

  @retval tWWDR

**/
UINT8
CalctWWDR (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch,               // Current Channel
    UINT8       minWWDR,  // Minimum value for WWDR
    TT_VARS     *ttVars   // Structure that holds the variables
    );

/**

  Restores trained timing values register

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Processor socket to check
  @param ch     - Current Channel

  @retval None

**/
void
RestoreTimings (
    PSYSHOST    host,               // Pointer to the system host (root) structure
    UINT8       socket,           // Processor socket to check
    UINT8       ch                // Current Channel
    );

/**

  Checks to see of supplied group is a member of the CMD Group.

  @param group - Group item to check

  @retval 1 - Is a member of CMD Group
  @retval 0 - Is not a member of CMD Group

**/
UINT8
CheckCMDGroup (
    GSM_GT  group // Command Group
    );

/**

  Checks to see of supplied group is a member of the CTL Group.

  @param group - Group item to check

  @retval 1 - Is a member of CTL Group
  @retval 0 - Is not a member of CTL Group

**/
UINT8
CheckCTLGroup (
    GSM_GT  group // Control Group
    );

/**

  Enables parity checking on the DIMM

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval None

**/
void
EnableParityChecking (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket   // Socket Id
    );

/**

  Disables parity checking on the DIMM

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval None

**/
void
DisableParityChecking (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket   // Socket Id
    );

/**

  Finds command margins for the given signal

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param dimm    - DIMM number
  @param rank    - rank number
  @param signal  - Signal
  @param le      - Left edge
  @param re      - Right edge

  @retval None

**/
void
GetCmdMarginsSweep (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8     dimm,       // DIMM number
    UINT8     rank,       // rank number
    GSM_CSN   signal,     // Signal
    INT16     *le,        // Left edge
    INT16     *re         // Right edge
    );

/**

  Returns the logical rank number

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - channel number
  @param dimm    - DIMM number
  @param rank    - rank number

  @retval UINT8

**/
UINT8
GetLogicalRank (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8     ch,       // channel number
    UINT8     dimm,       // DIMM number
    UINT8     rank      // rank number
    );

/**

  Normalize all PI's to reduce latency

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval UINT32

**/
UINT32
NormalizeCCC (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket   // Socket Id
    );

/**

  Set normal CMD timing

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval None

**/
void
SetNormalCMDTiming (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8     socket  // Socket Id
    );

/**

  Updates the composite edges

  @param le      - Left edge
  @param re      - Right edge
  @param compLe  - Composite left edge
  @param compRe  - Composite right edge

  @retval None

**/
void
UpdateEdges (
    INT16 le,        // Left edge
    INT16 re,        // Right edge
    INT16 *compLe,   // Composite left edge
    INT16 *compRe    // Composite right edge
    );

/**

  Updates minimum and maximum variables based on an input

  @param val    - New value to evaluate
  @param minVal - Current minimum value
  @param maxVal - Current maximum value

  @retval None

**/
void
UpdateMinMaxInt (
    INT16 val,          // New value to evaluate
    INT16 *minVal,    // Current minimum value
    INT16 *maxVal     // Current maximum value
    );

/**

  Delay time in QCLK units \n
    Note: Max delay is 1.2 seconds

  @param host - Pointer to sysHost, the system host (root) structure
  @param QClk - Number of QClks to delay

  @retval None

**/
void
FixedQClkDelay (
    PSYSHOST host,              // Pointer to the system host (root) structure
    UINT32   QClk      // Number of QClks to delay
    );

/**

  Exectues early command/clock training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
EarlyCmdClk (
    PSYSHOST host       // Pointer to the system host (root) structure
    );

/**

  Exectues early control/clock training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
EarlyCtlClk (
    PSYSHOST host       // Pointer to the system host (root) structure
    );

/**

  Exectues command/clock training

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
LateCmdClk (
    PSYSHOST host       // Pointer to the system host (root) structure
    );

/**

  Check to see if this DIMM properly supports parity

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel
  @param dimm    - DIMM

  @retval 0 if no workaround was completed
  @retval 1 if any workarounds were completed


**/
UINT8
CheckParityWorkaround (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8     ch,   // Channel
    UINT8     dimm    // DIMM
    );

/**

  Delay time in 1 us units

  @param host    - Pointer to sysHost, the system host (root) structure
  @param usDelay - number of us units to delay

  @retval None

**/
void
FixedDelay (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT32   usDelay    // number of us units to delay
    );

/**

Delay time in 1 us units

@param host    - Pointer to sysHost, the system host (root) structure
@param usDelay - number of us units to delay

@retval None

**/
void
FixedDelayMem(
PSYSHOST  host,   // Pointer to the system host (root) structure
UINT32   usDelay    // number of us units to delay
);

/**

  Gets the ACPI timer value

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 24-bit counter value

**/
UINT32
GetCount (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Determines delay since the GetCount was called

  @param host       Pointer to sysHost, the system host (root) structure
  @param startCount 24-bit counter value from GetCount()

  @retval Delay time in 1 us units

**/
UINT32
GetDelay (
    PSYSHOST  host,   // Pointer to the system host (root) structure
  UINT32   startCount   // 24-bit counter value from GetCount ()
    );

/**

  Initialize the power management timer

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
InitPmTimer (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Starts the JEDEC initialization sequence

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket to initizlize
  @param chBitMask  - Bit Mask of Channels Present on the Socket

  @retval SUCCESS

**/
UINT32
JedecInitSequence (
    PSYSHOST  host,         // Pointer to the system host (root) structure
    UINT8     socket,       // Socket to initizlize
    UINT32    chBitMask     // Bit Mask of Channels Present on the Socket
    );

/**



  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket to initizlize
  @param chBitMask - Bit Mask of Channels Present on the Socket

  @retval None

**/
void
JedecInitDdrAll (
    PSYSHOST host,           // Pointer to the system host (root) structure
    UINT8      socket,       // Socket to initizlize
    UINT32     chBitMask     // Bit Mask of Channels Present on the Socket
    );


/**

  Mirrors address bits

  @param host  - Pointer to sysHost, the system host (root) structure
  @param data  - Data for the MRS write

  @retval Mirrored data

**/
UINT16
MirrorAddrBits (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT16   data     // Data for the MRS write
    );

/**

  Mirrors bank address bits

  @param host  - Pointer to sysHost, the system host (root) structure
  @param bank  - Bank address to be mirrored

  @retval Mirrored bank address

**/
UINT8
MirrorBankBits (
    PSYSHOST  host,       // Pointer to the system host (root) structure
    UINT8     bank        // Bank address to be mirrored
    );

/**

  Inverts address bits

  @param data - Address to be inverted

  @retval Inverted address

**/
UINT16
InvertAddrBits (
    UINT16 data   // Address to be inverted
    );

/**

  Inverts bank address bits

  @param bank - Bank address to be inverted

  @retval Inverted bank address

**/
UINT8
InvertBankBits (
    UINT8 bank  // Bank address to be inverted
    );

/**

  Detect the TRR Mode(s) supported on this channel

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval TRR mode(s) supported

**/
UINT8
DetectTRRSupport (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch             // Channel number (0-based)
    );

/**

  Program register control words for RDIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Dimm number (0-based)
  @param rank    - Rank number (0-based)

  @retval None

**/
void
DoRegisterInit (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm,            // Dimm number (0-based)
    UINT8    rank           // Rank number (0-based)
    );

/**

  Program register control words for RDIMMs

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)

  @retval None

**/
void
DoRegisterInitDDR4 (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm,            // Dimm number (0-based)
    UINT8    rank           // Rank number (0-based)
    );

void
DoRegisterInitDDR4_CKE (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm,            // Dimm number (0-based)
    UINT8    rank           // Rank number (0-based)
    );

void
DoRegisterInitAep (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm           // Dimm number (0-based)
    );

void
SetRankMPR (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm,            // Dimm number (0-based)
    UINT8    rank,            // Rank number (0-based)
    UINT8     mprMode // Either 0 for Disable or MR3_MPR to Enable
    );

/**

  Read LR Buffer

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - DIMM number (0-based)
  @param lrbufAddr  - Address for the read
  @param lrbufData  - Pointer to the LR Buffer data

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
ReadLrbufSmb (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch,            // Channel number (0-based)
    UINT8    dimm,          // DIMM number (0-based)
    UINT16   lrbufAddr,     // Address for the read
    UINT32   *lrbufData     // Pointer to the LR Buffer data
    );

/**

  Write LR Buffer

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ch        - Channel number (0-based)
  @param dimm      - DIMM number (0-based)
  @param size      - number of bytes to be written
  @param lrbufAddr - Address for the read
  @param lrbufData - Pointer to the LR Buffer data

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
WriteLrbufSmb (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch,            // Channel number (0-based)
    UINT8    dimm,          // DIMM number (0-based)
    UINT8    size,          // number of bytes to be written
    UINT16   lrbufAddr,     // Address for the read
    UINT32   *lrbufData     // Pointer to the LR Buffer data
    );

/**

  Writes a nibble to DDR4 LRDIMM buffer control words

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param ch               - Channel number (0-based)
  @param dimm             - Dimm number (0-based)
  @param rank             - Rank number (0-based)
  @param controlWordData  - Control Word data
  @param controlWordFunc  - Function index in the LR buffer
  @param controlWordAddr  - Control Word Address

  @retval None

**/
void
WriteLrbuf (
    PSYSHOST host,                // Pointer to the system host (root) structure
    UINT8    socket,                // Socket Id
    UINT8    ch,                    // Channel number (0-based)
    UINT8    dimm,                  // Dimm number (0-based)
    UINT8    rank,                  // Rank number (0-based)
    UINT8    controlWordData,     // Control Word data
    UINT8    controlWordFunc,     // Function index in the LR buffer
    UINT8    controlWordAddr      // Control Word Address
    );

/**

  Writes a nibble to LRDIMM register control words using PBA mode

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket Id
  @param ch                  - Channel number (0-based)
  @param dimm                - Dimm number (0-based)
  @param rank                - Rank number (0-based)
  @param controlWordDataPtr  - Pointer to a 9 byte array of control word data
  @param controlWordFunc     - Function index in the LR buffer
  @param controlWordAddr     - Control Word Address
  @param bufferAddr          - Address of the individual buffer to target...or 'FF' for all

  @retval None

**/
void
WriteLrbufPBA (
    PSYSHOST host,                // Pointer to the system host (root) structure
    UINT8    socket,              // Socket Id
    UINT8    ch,                  // Channel number (0-based)
    UINT8    dimm,                // Dimm number (0-based)
    UINT8    rank,                // Rank number (0-based)
    UINT8    *controlWordDataPtr,   // Pointer to a 9 byte array of control word data
    UINT8    controlWordFunc,         // Function index in the LR buffer
    UINT8    controlWordAddr,         // Control Word Address
    UINT8    bufferAddr               // Address of the individual buffer to target...or 'FF' for all
    // buffers
    );

/**

  Writes DRAM mode registers

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param data    - Data for the MRS write
  @param bank    - Bank address for the MRS write

  @retval None

**/
void
WriteMRS (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8    ch,          // Channel number (0-based)
    UINT8    dimm,      // DIMM number (0-based)
    UINT8    rank,      // Rank number (0-based)
    UINT16   data,      // Data for the MRS write
    UINT8    bank       // Bank address for the MRS write
    );

/**

  Writes DRAM mode registers

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - DIMM number (0-based)
  @param rank   - Rank number (0-based)
  @param data   - Data for the MRS write
  @param bank   - Bank address for the MRS write
  @param side   - Side

  @retval None

**/
void
WriteMrsDdr4 (
    PSYSHOST host,      // Pointer to the system host (root) structure
    UINT8    socket,    // Socket Id
    UINT8    ch,          // Channel number (0-based)
    UINT8    dimm,      // DIMM number (0-based)
    UINT8    rank,      // Rank number (0-based)
    UINT16   data,      // Data for the MRS write
    UINT8    bank,      // Bank address for the MRS write
    UINT8    side       // Side
    );

/**

  Writes a nibble to RDIMM/LRDIMM register control words

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket Id
  @param ch               - Channel number (0-based)
  @param dimm             - DIMM number (0-based)
  @param rank             - Rank number (0-based)
  @param controlWordData  - Control Word data (a nibble)
  @param controlWordAddr  - Control Word Address

  @retval None

**/
void
WriteRC (
    PSYSHOST  host,               // Pointer to the system host (root) structure
    UINT8     socket,               // Socket Id
    UINT8     ch,                   // Channel number (0-based)
    UINT8     dimm,               // DIMM number (0-based)
    UINT8     rank,                 // Rank number (0-based)
    UINT8     controlWordData,    // Control Word data (a nibble)
    UINT8     controlWordAddr     // Control Word Address
    );

/**

  Writes a nibble or byte to DDR4 Buffer Control

  @param host              - Pointer to sysHost, the system host (root) structure
  @param socket            - Socket Id
  @param ch                - Channel number (0-based)
  @param dimm              - DIMM number (0-based)
  @param rank              - Rank number (0-based)
  @param controlWordData   - Control Word data (a nibble or byte)
  @param controlWordFunc   - Control Word Function
  @param controlWordAddr   - Control Word Address

  @retval None

**/
void
WriteBC (
    PSYSHOST  host,               // Pointer to the system host (root) structure
    UINT8     socket,               // Socket Id
    UINT8     ch,                   // Channel number (0-based)
    UINT8     dimm,               // DIMM number (0-based)
    UINT8     rank,                 // Rank number (0-based)
    UINT8     controlWordData,    // Control Word data (a nibble or byte)
    UINT8     controlWordFunc,    // Control Word Function
    UINT8     controlWordAddr     // Control Word Address
    );

/**

  Writes a nibble to DDR LRDIMM register control words

  @param host              - Pointer to sysHost, the system host (root) structure
  @param socket            - Socket Id
  @param ch                - Channel number (0-based)
  @param dimm              - Dimm number (0-based)
  @param rank              - Rank number (0-based)
  @param controlWordData   - Control Word data
  @param controlWordFunc   - Function index in the LR buffer
  @param controlWordAddr   - Control Word Address

  @retval None

**/
void
WriteRcLrbuf (
    PSYSHOST host,                // Pointer to the system host (root) structure
    UINT8    socket,              // Socket Id
    UINT8    ch,                  // Channel number (0-based)
    UINT8    dimm,                // Dimm number (0-based)
    UINT8    rank,                  // Rank number (0-based)
    UINT8    controlWordData,   // Control Word data
    UINT8    controlWordFunc,   // Function index in the LR buffer
    UINT8    controlWordAddr    // Control Word Address
    );

/**

  Writes a byte to LRDIMM extended register control words

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - Dimm number (0-based)
  @param rank       - Rank number (0-based)
  @param lrbufData  - 8-bit data to write
  @param lrbufAddr  - 16-bit flat address space (4-bit function, 12-bit register offset)

  @retval None

**/
void
WriteRcLrbufExtended (
    PSYSHOST host,            // Pointer to the system host (root) structure
    UINT8    socket,          // Socket Id
    UINT8    ch,              // Channel number (0-based)
    UINT8    dimm,            // Dimm number (0-based)
    UINT8    rank,            // Rank number (0-based)
    UINT8    lrbufData,       // 8-bit data to write
    UINT16   lrbufAddr        // 16-bit flat address space (4-bit function, 12-bit register offset)
    );

/**

  Determine if this is a Termination Rank

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval 0 - This is not a Termination Rank
  @retval 1 - This is a Termination Rank

**/
UINT32
IsTermRank (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch,            // Channel number (0-based)
    UINT8    dimm,    // DIMM number
    UINT8    rank     // Rank number
    );

/**

  Convert given value to and from ODT value

  @param host     - Pointer to sysHost, the system host (root) structure
  @param group    - Group type
  @param value    - Value from Register/Cache
  @param code     - CODE or DECODE ODT
  @param dimmtype - Dimm type

  @retval The converted value

**/
UINT16
ConvertOdtValue (
    PSYSHOST  host,            // Pointer to sysHos
    GSM_GT    group,           // Group type
    UINT16    value,           // Value from Register/Cache
    UINT8     code,            // CODE or DECODE ODT
    UINT8     dimmtype         // Dimm type
    );

/**

  Programs the ODT matrix

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval None

**/
void
SetOdtMatrix (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch             // Channel number (0-based)
    );

/**

  Get the ODT Value Index

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param freqLimit  - Frequency Limit

  @retval ODT_VALUE_INDEX

**/
ODT_VALUE_INDEX
GetOdtValueIndex (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch,            // Channel number (0-based)
    UINT8    freqLimit    // Frequency Limit
    );

/**

  Returns the  MCODT bit values for 50 or 100 ohms

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number
  @param ch     - Channel number (0-based)
  @param mcodt  - 0 = 50 ohms; 1 = 100 ohms (HW default)

  @retval UINT8

**/
UINT8
GetMcOdtValue (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,    // Socket number
    UINT8    ch,          // Channel number (0-based)
    UINT16   *mcodt     // 0 = 50 ohms; 1 = 100 ohms (HW default)
    );

/**

  Get the ddr4OdtValueStruct

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval struct ddr4OdtValueStruct*

**/
struct ddr4OdtValueStruct*
LookupDdr4OdtValue (
    PSYSHOST host,          // Pointer to the system host (root) structure
    UINT8    socket,        // Socket Id
    UINT8    ch             // Channel number (0-based)
    );

/**

  Adds two UINT64 values

  @param Operand1  - first UINT64 to add
  @param Operand2  - second UINT64 to add

  @retval sum of operand1 and operand2

**/
UINT64_STRUCT
AddUINT64 (
    UINT64_STRUCT Operand1, // first UINT64 to add
    UINT64_STRUCT Operand2  // second UINT64 to add
    );

/**

  Add UINT64 and UINT32

  @param Operand1  - UINT64 to add
  @param Operand2  - UINT32 to add

  @retval sum of operand1 and operand2

**/
UINT64_STRUCT
AddUINT64x32 (
    UINT64_STRUCT Operand1, // UINT64 to add
    UINT32        Operand2  // UINT32 to add
    );

/**

  Subtract two UINT64s

  @param Operand1  - first UINT64
  @param Operand2  - second UINT64

  @retval operand1 - operand2

**/
UINT64_STRUCT
SubUINT64 (
    UINT64_STRUCT Operand1, // First UINT64
    UINT64_STRUCT Operand2  // Second UINT64
    );

/**

  Subtract an UINT32 from an UINT64

  @param Operand1  -  UINT64
  @param Operand2  -  UINT32

  @retval operand1 - operand2

**/
UINT64_STRUCT
SubUINT64x32 (
    UINT64_STRUCT Operand1,
    UINT32        Operand2
    );

/**

  Multiplies two 32 bit values

  @param dw1 - first 32 bit value
  @param dw2 - second 32 bit value

  @retval 64 bit value resulting from the multiplication of two given values

**/
UINT64_STRUCT
MulDw64 (
    UINT32 dw1,   // first 32 bit value
    UINT32 dw2    // second 32 bit value
    );

/**

  This routine allows a 64 bit value to be multiplied with a 32 bit
  value returns 64bit result. No checking if the result is greater than 64bits.

  @param Multiplicand  - Multiplicand
  @param Multiplier    - what to multiply by

  @retval result of Multiplicand * Multiplier

**/
UINT64_STRUCT
MultUINT64x32 (
    UINT64_STRUCT Multiplicand, // Multiplicand
    UINT32        Multiplier    // what to multiply by
    );

/**

  divides 64 bit value by 2

  @param qw1 - 64 bit value to divide

  @retval result of qw1 / 2

**/
UINT64_STRUCT
DivBy2Qw64 (
    UINT64_STRUCT qw1    // 64 bit value to divide
    );

/**

  This routine allows a 64 bit value to be divided with a 32 bit value returns
  64bit result and the Remainder.

  @param Dividend  - Value to divide into
  @param Divisor   - Value to divide by
  @param Remainder - Pointer to location to write remainder

  @retval Remainder of Dividend / Divisor

**/
UINT64_STRUCT
DivUINT64x32 (
    UINT64_STRUCT Dividend,    // Value to divide into
    UINT32        Divisor,     // Value to divide by
    UINT32        *Remainder   // Pointer to location to write remainder
    );

/**

  This function calculates floor(log2(x)) + 1
  by finding the index of the most significant bit
  and adding 1

  @param Value     - Input value

  @retval Returns floor(log2(x)) + 1 or 0, if the input was 0 or negative

**/
UINT8
Log2x32 (
    UINT32 Value   // Input value
    );

/**

  This routine allows a 64 bit value to be right shifted by 32 bits and returns the
  shifted value. Count is valid up 63. (Only Bits 0-5 is valid for Count)

  @param Operand - Value to be shifted
  @param Count   - Number of times to shift right.

  @retval Value shifted right identified by the Count.

**/
UINT64_STRUCT
RShiftUINT64 (
    UINT64_STRUCT Operand, // Value to be shifted
    UINT32        Count    // Number of times to shift right.
    );

/**

  This routine allows a 64 bit value to be left shifted by 32 bits and returns
  the shifted value. Count is valid up 63. (Only Bits 0-5 is valid for Count)

  @param Data  - value to be shifted
  @param Count - number of time to shift

  @retval Value shifted left identified by the Count.

**/
UINT64_STRUCT
LShiftUINT64 (
    UINT64_STRUCT Data,  // value to be shifted
    UINT32        Count    // number of time to shift
    );

/**

  Calculate the 16 bit CRC

  @param ptr    - Pointer to the beginning of the data to work on
  @param count  - number of bytes to check

  @retval INT16

**/
INT16
Crc16 (
    char *ptr,    // Pointer to the beginning of the data to work on
    int count     // number of bytes to check
    );

/**

  Finds larger of two values

  @param x - First value
  @param y - Second value

  @retval largest of two values

**/
UINT32
MaxVal (
    UINT32 x,  // First value
    UINT32 y   // Second value
    );

/**

  Finds smaller of two values

  @param x - First value
  @param y - Second value

  @retval smallest of two values

**/
UINT32
MinVal (
    UINT32 x,  // First value
    UINT32 y   // Second value
    );

/**

  This function dispatches the slave processors to start MemMain

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS - Memory initialization completed successfully.
  @retval FAILURE - A reset is required

**/
UINT32
DispatchSlaves (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function syncs data across sockets

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS: Sync successfull

**/
UINT32
PipeSync (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  his function gets data from slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
GetData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function gets data from slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
GetNvramData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function gets data from slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
GetSlaveData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Get the status variable from each socket

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 0 - no reset
  @retval 2 - Power Good
  @retval 3 - Warm reset

**/
UINT32
GetStatus (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function sends data to slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
SendData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function sends data to slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
SendNvramData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

This function sends data to slave processors

@param host - Pointer to sysHost, the system host (root) structure

@retval SUCCESS

**/
UINT32
SendVarData(
PSYSHOST                  host
);

/**

This function gets data from master

@param host - Pointer to sysHost, the system host (root) structure

@retval SUCCESS

**/
UINT32
GetVarData(
PSYSHOST                  host
);
/**

  This function sends data to slave processors


  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
SendSlaveData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Send the status variable to each socket

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 0 - no reset
  @retval 2 - Power Good
  @retval 3 - Warm reset

**/
UINT32
SendStatus (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**
This function sends bootmode to slave processors

@param host:         Pointer to sysHost

@retval SUCCESS:  Memory initialization completed successfully.
**/
UINT32
Sendbootmode(
PSYSHOST  host
);

/**
This function sends bootmode to slave processors

@param host:         Pointer to sysHost

@retval SUCCESS:  Memory initialization completed successfully.
**/
UINT32
Getbootmode(
PSYSHOST  host
);

/**
This function sends bootmode to slave processors

@param host:         Pointer to sysHost

@retval SUCCESS:  Memory initialization completed successfully.
**/
UINT32
SendSocketbootmode(
PSYSHOST  host
);

/**
This function sends bootmode to slave processors

@param host:         Pointer to sysHost

@retval SUCCESS:  Memory initialization completed successfully.
**/
UINT32
GetSocketbootmode(
PSYSHOST  host
);

/**

  Send mem.var, mem.nvram common and mem.nvram data from each socket to master to sync up

  @param host - Pointer to sysHost, the system host (root) structure

**/
UINT32
SendSlaveSsaData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Get mem.var, mem.nvram common and mem.nvram data from each slave socket to sync up

  @param host - Pointer to sysHost, the system host (root) structure

**/
UINT32
GetSlaveSsaData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Send a variable to each socket to sync up

  @param host - Pointer to sysHost, the system host (root) structure

**/
UINT32
SendSsaData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Get a variable from master socket to sync up

  @param host - Pointer to sysHost, the system host (root) structure

**/
UINT32
GetSsaData (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  This function dispatches the slave processors to start MemMain

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS:  Memory initialization completed successfully.
  @retval FAILURE:  A reset is required

**/
UINT32
SlaveAck (
    PSYSHOST  host  // Pointer to host structure (outside scope of this spec)
    );

/**

  Creates a Heap for storage

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
InitHeap (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  rcFree previously rcMalloced memory

  @param host     - Pointer to sysHost, the system host (root) structure
  @param memblock - Pointer to memory to deallocate

  @retval None

**/
void
rcFree (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    void     *memblock  // Pointer to memory to deallocate
    );

/**

  Reserve memory in the heap

  @param host  - Pointer to sysHost, the system host (root) structure
  @param size  - Size of memory to allocate

  @retval Pointer to allocated memory

**/
void*
rcMalloc (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT32 size   // Size of memory to allocate
    );

/**

  compares a specified block of memory from source location to destination location

  @param dest     - pointer to first memory location for compare
  @param src      - pointer to second memory location for compare
  @param numBytes - Number of bytes to compare

  @retval 0 - if all data matches
  @retval 1 - if miscompare

**/
UINT32
MemCompare (
    UINT8 *dest,        // pointer to first memory location for compare
    UINT8 *src,           // pointer to second memory location for compare
    UINT32 numBytes     // Number of bytes to compare
    );

/**

  copies a specified block of memory from source location to destination location

  @param dest       - pointer to copy destination
  @param src        - pointer to source to copy
  @param numBytes   - size to copy

  @retval None

**/
void
MemCopy (
    UINT8  *dest,       // pointer to copy destination
    UINT8  *src,          // pointer to source to copy
    UINT32 numBytes     // size to copy
    );

/**

  Flood fill a specific memory location block with a specified value

  @param dest      - Pointer to memory location
  @param value     - Value to flood location with
  @param numBytes  - number of bytes to flood with value provided

  @retval None

**/
void
MemSetLocal (
    UINT8  *dest,         // Pointer to memory location
    UINT32 value,         // Value to flood location with
    UINT32 numBytes       // number of bytes to flood with value provided
    );

/**

  Train IMode

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
TrainImode (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

UINT32
TrainRXCTLE (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

UINT32
TrainRxOdt (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

UINT32
TrainTxEq (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Calls the correct read MSR function based on single vs multi-threaded execution

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param msr     - Address of MSR to read

  @retval UINT64_STRUCT - MSR value as read from CPU

**/
UINT64_STRUCT
ReadMsrPipe (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket,   // Socket number
    UINT32    msr       // Address of MSR to read
    );

/**

  Calls the correct write MSR function based on single vs multi-threaded execution

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number
  @param msr    - Address of MSR to read
  @param msrReg - MSR Data

  @retval None

**/
void
WriteMsrPipe (
    PSYSHOST      host,      // Pointer to the system host (root) structure
    UINT8         socket,    // Socket number
    UINT32        msr,       // Address of MSR to read
    UINT64_STRUCT msrReg   // MSR Data
    );

/**

  Master side of exiting Pipe on slave processors

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
ExitPipe (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Master side of pipe initialization

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
InitializePipe (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

INT8
RcPutchar (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    INT8     c       // character received from serial buffer
    );

INT8
getchar (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Get character from serial buffer

  @param host - Pointer to sysHost, the system host (root) structure

  @retval Character received from serial buffer

**/
INT8
getchar_echo (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Get character from serial buffer with a timeout

  @param host       - Pointer to sysHost, the system host (root) structure
  @param c          - pointer to character received from serial buffer
  @param usTimeout  - timeout in microseconds

  @retval 0 - successful
  @retval other - failure

**/
UINT32
getchar_timeout (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    INT8     *c,        // pointer to character received from serial buffer
    UINT32   usTimeout  // timeout in microseconds
    );

/**

  Put character into serial buffer

  @param host - Pointer to sysHost, the system host (root) structure
  @param c    - pointer to character received from serial buffer

  @retval 0 - Success
  @retval 1 - Failure

**/
INT8
putchar_buffer (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    INT8      c       // pointer to character received from serial buffer
    );

/**

  Put character into serial buffer

  @param host - Pointer to sysHost, the system host (root) structure
  @param c    - pointer to character received from serial buffer

  @retval 0 - Success
  @retval 1 - Failure

**/
UINT32
SerialBufferChar (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    INT8      c       // pointer to character received from serial buffer
    );

/**

  Sends a byte to the serial buffer

  @param host - Pointer to sysHost, the system host (root) structure
  @param now  - 0 wait for delay \n
                1 send immediately

  @retval 0 - Success
  @retval 1 - Failure

**/
UINT32
SerialSendBuffer (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8    now      // 0 - wait for delay
                      // 1 - send immediately
    );

/**

  Get control of a global semaphore for the calling socket BSP.\n
  This should be used with a matching releasePrintFControl()
  to gain exclusive ownership of a system semaphore for a section
  of code that blocks/yields to other socket BSPs.
  host->var.common.printfDepth is used to handle the nested calls to this function

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 1 - success

**/
UINT8
getPrintFControl (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  release ownership of semaphore. \n
  must be used to match getprintFControl()


  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
releasePrintFControl (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

#ifdef SERIAL_DBG_MSG
/**
 * rcPrintf Macro
 * Used to abstract rcPrintfunction
 */
#define rcPrintf(DbgInfo) rcPrintfunction DbgInfo
#else
#define rcPrintf(DbgInfo)
#endif

#ifdef DEBUG_SCRATCHPAD
#define SCRATCHPAD_DBG(x) rcPrintfunction x
#else
#define SCRATCHPAD_DBG(x)
#endif

/**

  Prints string to serial output

  @param host   - Pointer to sysHost, the system host (root) structure
  @param Format  - String containing characters to print and formating data.\n
                      %[flags][width]type \n
                       \n
                      [flags] '-' left align \n
                      [flags] '+' prefix with sign (+ or -) \n
                      [flags] '0' zero pad numbers \n
                      [flags] ' ' prefix black in front of postive numbers \n
                       \n
                      [width] non negative decimal integer that specifies \n
                              the width to print a value. \n
                      [width] '*' get the width from a int argument on the stack. \n
                       \n
                      type    'd'|'i' signed decimal integer \n
                      type    'u' unsinged integer \n
                      type    'x'|'X' usinged hex using "ABCDEF" \n
                      type    'c' print character \n
                      type    'p' print a pointer to void \n
                      type    'r' output a raw binary encoding

  @retval 0 - Success
  @retval 1 - Failure

**/
int
rcPrintfunction (
    PSYSHOST  host,       // Pointer to the system host (root) structure
    const INT8 *Format,   // Format string for output
    ...                   // Values matching the variables declared in
                          //   the Format parameter
    );

/**

    rcPrintf with stdargs varargs stack frame in place of .... Limited
    support for sizes other than UINT32 to save code space


  @param host    - Pointer to sysHost, the system host (root) structure
  @param Format  - String containing characters to print and formating data.\n
                      %[flags][width]type \n
                       \n
                      [flags] '-' left align \n
                      [flags] '+' prefix with sign (+ or -) \n
                      [flags] '0' zero pad numbers \n
                      [flags] ' ' prefix black in front of postive numbers \n
                       \n
                      [width] non negative decimal integer that specifies \n
                              the width to print a value. \n
                      [width] '*' get the width from a int argument on the stack. \n
                       \n
                      type    'd'|'i' signed decimal integer \n
                      type    'u' unsinged integer \n
                      type    'x'|'X' usinged hex using "ABCDEF" \n
                      type    'c' print character \n
                      type    'p' print a pointer to void \n
                      type    'r' output a raw binary encoding
  @param Marker  - va_list that points to the arguments for Format that are on the stack

  @retval Prints to putchar() - To save space we print every thing as UINT32 and
  @retval don't support type over rides like {h | I | I64 | L}

**/
int
rcVprintf (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    const INT8 *Format,     // String containing characters to print and formating data.
    //    %[flags][width]type
    //
    //    [flags] '-' left align
    //    [flags] '+' prefix with sign (+ or -)
    //    [flags] '0' zero pad numbers
    //    [flags] ' ' prefix black in front of postive numbers
    //
    //    [width] non negative decimal integer that specifies
    //            the width to print a value.
    //    [width] '*' get the width from a int argument on the stack.
    //
    //    type    'd'|'i' signed decimal integer
    //    type    'u' unsinged integer
    //    type    'x'|'X' usinged hex using "ABCDEF"
    //    type    'c' print character
    //    type    'p' print a pointer to void
    //    type    'r' output a raw binary encoding
    va_list    Marker       // va_list that points to the arguments for Format that are on the stack
    );

/**
  set screen color scheme


  @param host       - Pointer to sysHost, the system host (root) structure
  @param foreGround - color code for foreground
  @param backGround - color code for background
  @param attribute  - attribute flag \n
                        ATTRIBUTE_OFF         0 \n
                        ATTRIBUTE_BOLD        1 \n
                        ATTRIBUTE_UNDERSCORE  4 \n
                        ATTRIBUTE_BLINK       5 \n
                        ATTRIBUTE_REVERSE     7 \n
                        ATTRIBUTE_CONCEAL     8

  @retval None

**/
void
setColor (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    INT8     foreGround,    // color code for foreground
    INT8     backGround,    // color code for background
    INT8     attribute      // attribute flag
    //   ATTRIBUTE_OFF         0
    //   ATTRIBUTE_BOLD        1
    //   ATTRIBUTE_UNDERSCORE  4
    //   ATTRIBUTE_BLINK       5
    //   ATTRIBUTE_REVERSE     7
    //   ATTRIBUTE_CONCEAL     8
    );

/**

  Sets color scheme for foreground red, background black

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
setRed (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Set color scheme to background black, forground white

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
setWhite (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Compares two strings

  @param src1 - first string to compare
  @param src2 - second string to compare

  @retval 0 - strings match
  @retval 1 - strings dont match

**/
UINT32
StrCmpLocal (
    INT8 *src1, // first string to compare
    INT8 *src2  // second string to compare
    );

/**

  Copy string from source to destination

  @param dest - Pointer to destination for string copy
  @param src  - Pointer to source of string to copy

  @retval None

**/
void
StrCpyLocal (
    INT8 *dest,  // Pointer to destination for string copy
    INT8 *src    // Pointer to source of string to copy
    );

/**

  Determines the length of the string

  @param Str - Pointer to string to measure

  @retval Length of string

**/
UINT32
StrLenLocal (
    INT8 *Str     // Pointer to string to measure
    );

/**

  Convert character to lower case

  @param c - character to convert

  @retval Lower case character

**/
INT8
tolower_ (
    INT8 c // character to convert
    );

/**

  Determines if this rank is valid

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket number
  @param ch     - channel number
  @param dimm   - dimm number
  @param rank   - rank number
  @param flags  - CHECK_MAPOUT

  @retval SUCCESS - Rank is valid
  @retval FAILURE - Rank is not valid

**/
UINT8
CheckRank (
    PSYSHOST host,             // Pointer to the system host (root) structure
    UINT8    socket,              // socket number
    UINT8    ch,                  // channel number
    UINT8    dimm,                // dimm number
    UINT8    rank,                // rank number
    UINT8    flags                // CHECK_MAPOUT
    );

#ifdef SERIAL_DBG_MSG
/**

  Counts this mode's access

  @param host - Pointer to sysHost, the system host (root) structure
  @param mode -   PCI_ACCESS     0
                  JEDEC_COUNT    2
                  FIXED_DELAY    3
                  POLLING_COUNT  6
                  VREF_MOVE      7
  @param data - data to add to count

  @retval None

**/
void
countTrackingData (
    PSYSHOST  host,    // Pointer to the system host (root) structure
    UINT8     mode,    //   PCI_ACCESS     0
                       //   JEDEC_COUNT    2
                       //   FIXED_DELAY    3
                       //   POLLING_COUNT  6
                       //   VREF_MOVE      7
    UINT32    data     // data to add to count
    );

/**

  Sets the current memory phase

  @param host  - Pointer to sysHost, the system host (root) structure
  @param phase - Current memory phase

  @retval None

**/
void
SetMemPhase (
    PSYSHOST  host,    //  Pointer to the system host (root) structure
    UINT32    phase    // Current memory phase
    );

/**

  Clears the current memory phase

  @param host  - Pointer to sysHost, the system host (root) structure
  @param phase - Current memory phase

  @retval None

**/
void
ClearMemPhase (
    PSYSHOST  host,    //  Pointer to the system host (root) structure
    UINT32    phase      // Current memory phase to clear
    );

/**

  Compares the current memory phase

  @param host -  Pointer to sysHost, the system host (root) structure
  @param phase - Current memory phase

  @retval UINT8 bit test result

**/
UINT8
IsMemPhase (
    PSYSHOST host,  //  Pointer to the system host (root) structure
    UINT32 phase    // Current memory phase
    );

/**

  Prints a double line

  @param host        - Pointer to sysHost, the system host (root) structure
  @param count       - Number of units in the line
  @param doubleLine  - 0 prints '-'   1 prints '='
  @param crLfBefore  - 1 = return before printing; 0 = ignore
  @param crLfAfter   - 1 = return after printing; 0 = ignore

  @retval None

**/
void
PrintLine (
    PSYSHOST  host,         // Pointer to the system host (root) structure
    UINT16    count,        // Number of units in the line
    UINT8     doubleLine,   // 0 prints '-'   1 prints '='
    UINT8     crLfBefore,   // 1 = return before printing; 0 = ignore
    UINT8     crLfAfter     // 1 = return after printing; 0 = ignore
    );

/**

  Displays a line of 'numSpaces' spaces ending in |

  @param host       - Pointer to sysHost, the system host (root) structure
  @param numSpaces  - Number of spaces for the empty block

  @retval None

**/
void
EmptyBlock (
    PSYSHOST  host,   // Pointer to sysHost
    UINT8     numSpaces // Number of spaces for the empty block
    );

/**

  Displays information about the device being refered to

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Processor socket
  @param ch     - Memory Channel
  @param dimm   - Specific DIMM on this Channel
  @param rank   - Rank on this DIMM
  @param strobe - Strobe
  @param bit    - Bit

  @retval None

**/
void
DisplayDev (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket,    // Processor socket
    UINT8     ch,       // Memory Channel
    UINT8     dimm,     // Specific DIMM on this Channel
    UINT8     rank,     // Rank on this DIMM
    UINT8     strobe,   // Strobe
    UINT8     bit       // Bit
    );

#endif // SERIAL_DBG_MSG

/**

  Socket SBSP writes the checkpoint code to the checkpoint CSR on socket SBSP and calls
  the platform checkpoint routine.\n\n

  Socket N reads the breakpoint CSR on socket N. If the breakpoint code matches the
  checkpoint code, then execution will stall in a loop until the breakpoint CSR
  on socket N is changed via ITP or other means.

  @param host       - Pointer to sysHost, the system host (root) structure
  @param majorCode  - Major Checkpoint code to write
  @param minorCode  - Minor Checkpoint code to write
  @param data       - Data specific to the minor checkpoint is written to low word of the checkpoint CSR

  @retval None

**/
void
OutputCheckpoint (
    PSYSHOST host,           // Pointer to the system host (root) structure
    UINT8    majorCode,      // Major Checkpoint code to write
    UINT8    minorCode,      // Minor Checkpoint code to write
    UINT16   data            // Data specific to the minor checkpoint is written to
    //   low word of the checkpoint CSR
    );

#ifdef  BDAT_SUPPORT
UINT32
FillBDATStructure (
    PSYSHOST  host      // Pointer to the system host (root) structure
    );
#endif  //  BDAT_SUPPORT

/**

  Fill the WDB buffer

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - Socket number
  @param ch                   - Channel number
  @param WdbLines             - Array that include data to write to the WDB
  @param NumberOfWdbLines     - The number of cachelines to write
  @param StartCachelineIndex  - Start offset/cacheline number on the WDB.

  @retval None

**/
void
WDBFill (
    PSYSHOST host,                      // Pointer to the system host (root) structure
    UINT8    socket,                    // Socket number
    UINT8    ch,                        // Channel number
    TWdbLine *WdbLines,               // Array that include data to write to the WDB
    UINT8    NumberOfWdbLines,        // The number of cachelines to write
    UINT8    StartCachelineIndex      // Start offset/cacheline number on the WDB.
    );

/**

  Write 1 cacheline worth of data to the WDB

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Processor socket
  @param ch        - Memory Channel
  @param pattern   - Pattern to write into the cache line
  @param spread    - The number of cache lines to write
  @param cacheLine - Cache line to write

  @retval None

**/
void
WriteWDBFixedPattern (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8     socket,    // Processor socket
    UINT8     ch,       // Memory Channel
    UINT32    pattern,  // Pattern to write into the cache line
    UINT8     spread, // The number of cache lines to write
    UINT8     cacheLine // Cache line to write
    );

/**

  The internal implementation of *_DECOMPRESS_PROTOCOL.Decompress().

  @param Source      - The source buffer containing the compressed data.
  @param SrcSize     - The size of source buffer
  @param Destination - The destination buffer to store the decompressed data
  @param DstSize     - The size of destination buffer.
  @param Scratch     - The buffer used internally by the decompress routine. This  buffer is needed to store intermediate data.
  @param ScratchSize - The size of scratch buffer.
  @param Version     - The version of de/compression algorithm.\n
                            Version 1 for EFI 1.1 de/compression algorithm.\n
                            Version 2 for Tiano de/compression algorithm.

  @retval 0 - Success
  @retval Other - Failure error code

**/
UINT32
Decompress (
    void    *Source,         // The source buffer containing the compressed data.
    UINT32  SrcSize,       // The size of source buffer
    void    *Destination,    // The destination buffer to store the decompressed data
    UINT32  DstSize,         // The size of destination buffer.
    void    *Scratch,      // The buffer used internally by the decompress routine. This  buffer is
    //     needed to store intermediate data.
    UINT32  ScratchSize,     // The size of scratch buffer.
    UINT8   Version          // The version of de/compression algorithm.
    //      Version 1 for EFI 1.1 de/compression algorithm.
    //      Version 2 for Tiano de/compression algorithm.
    );

/**

  Decompress code at src to dest

  @param host - Pointer to sys host struct
  @param Src  - Pointer to the compressed code
  @param Dest - Pointer to place the uncompressed code

  @retval 0 - Success
  @retval Other - Failure error code

**/
UINT32
DecompressRc (
             PSYSHOST host,       // Pointer to sys host struct
             UINT8    *Src,       // Pointer to the compressed code
             UINT8    *Dest       // Pointer to place the uncompressed code
                                  // retval 0 - Success;  Other - Failure error code
             );

/**

  The compression routine.

  @param[in]       SrcBuffer  - The buffer containing the source data.
  @param[in]       SrcSize    - The number of bytes in SrcBuffer.
  @param[in]       DstBuffer  - The buffer to put the compressed image in.
  @param[in, out]  DstSize    - On input the size (in bytes) of DstBuffer, on return the number of bytes placed in DstBuffer.
  @param Version              - The version of de/compression algorithm.\n\n
                                     Version 1 for EFI 1.1 de/compression algorithm.\n
                                     Version 2 for Tiano de/compression algorithm.
  @param Globals              - Scratch data buffer

  @retval EFI_SUCCESS           The compression was sucessful.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.  DstSize is required.

**/
UINT32
CompressMrc (
  IN       VOID     *SrcBuffer,   // The buffer containing the source data.
  IN       UINT32   SrcSize,      // The number of bytes in SrcBuffer.
  IN       VOID     *DstBuffer,   // The buffer to put the compressed image in.
  IN OUT   UINT32   *DstSize,     // On input the size (in bytes) of DstBuffer, on
                                  // return the number of bytes placed in DstBuffer.
  IN       UINT8    Version,      // The version of de/compression algorithm.
                                  //      Version 1 for EFI 1.1 de/compression algorithm.
                                  //      Version 2 for Tiano de/compression algorithm.
                                  // retval = 0: The compression was sucessful.
                                  // retval = EFI_BUFFER_TOO_SMALL: The buffer was too small.  DstSize is required.
  IN       VOID     *Globals      // Scratch data buffer
  );

/**

  The compression routine RC wrapper.

  @param[in]       host          Pointer to host struct.
  @param[in]       SrcBuffer     The buffer containing the source data.
  @param[in]       SrcSize       The number of bytes in SrcBuffer.
  @param[in]       DstBuffer     The buffer to put the compressed image in.
  @param[in, out]  DstSize       On input the size (in bytes) of DstBuffer, on
                                return the number of bytes placed in DstBuffer.

  @retval EFI_SUCCESS           The compression was sucessful.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.  DstSize is required.

**/
UINT32
CompressRc (
  IN       PSYSHOST host,         // Pointer to sysHost, the system host (root) structure
  IN       VOID     *SrcBuffer,   // The buffer containing the source data.
  IN       UINT32   SrcSize,      // The number of bytes in SrcBuffer.
  IN       VOID     *DstBuffer,   // The buffer to put the compressed image in.
  IN OUT   UINT32   *DstSize      // On input the size (in bytes) of DstBuffer, on
                                  // return the number of bytes placed in DstBuffer.
                                  // retval = 0: The compression was sucessful.
                                  // retval = EFI_BUFFER_TOO_SMALL: The buffer was too small.  DstSize is required.
  );

/**

  Find the USB2 debug port

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval EFI_DEVICE_ERROR - couldnt find port
  @retval EFI_SUCCESS - USB2 device discovered

**/
UINT32
DiscoverUsb2DebugPort (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Initialize USB2 debug port

  @param host  - Pointer to the system host (root) structure

  @retval EFI_DEVICE_ERROR - device failed
  @retval EFI_SUCCESS - USB2 port init successful

**/
UINT32
InitializeUsb2DebugPort (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

INTN
EFIAPI
GetRegisterAddress (
    IN USRA_ADDRESS    *Address    // A pointer of the address of the Common
    //    Address Structure to read from.
    );

/**

  Checks the processor type that is installed on the system

  @param host     - Pointer to sysHost, the system host (root) structure
  @param cpuType  - CPU type (HSX, BWX,....)

  @retval TRUE - Installed processor matches cpuType
  @retval FALSE - Installed processor does not match cpuType
**/
UINT8
CheckProcessorType (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8     cpuType   // CPU type (HSX, BWX,....)
    );

/**

  Checks if the provided CPU and stepping is equal to the CPU and stepping populated

  @param host     - Pointer to sysHost, the system host (root) structure
  @param cpuType  - CPU type (HSX, BWX,....)
  @param stepping - CPU stepping

  @retval 1 - CPU is the same and the stepping is equal to the inputs
  @retval 0 - CPU is not the same and/or the stepping is not equal to the inputs

**/
UINT8
CheckSteppingEqual (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8     cpuType,  // CPU type (HSX, BWX,....)
    UINT8     stepping  // CPU stepping
    );

/**

  Checks if the provided CPU and stepping is greater than the CPU and stepping populated

  @param host      - Pointer to sysHost, the system host (root) structure
  @param cpuType   - CPU type (HSX, BWX,....)
  @param stepping  - CPU stepping

  @retval 1 - CPU is the same and the stepping is greater than the inputs
  @retval 0 - CPU is not the same and/or the stepping is not greater than the inputs

**/
UINT8
CheckSteppingGreaterThan (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8     cpuType,  // CPU type (HSX, BWX,....)
    UINT8     stepping  // CPU stepping
    );

/**

  Checks if the provided CPU and stepping is less than the CPU and stepping populated

  @param host      - Pointer to sysHost, the system host (root) structure
  @param cpuType   - CPU type (HSX, BWX,....)
  @param stepping  - CPU stepping

  @retval 1 - CPU is the same and the stepping is less than the inputs
  @retval 0 - CPU is not the same and/or the stepping is not less than the inputs

**/
UINT8
CheckSteppingLessThan (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8     cpuType,  // CPU type (HSX, BWX,....)
    UINT8     stepping  // CPU stepping
    );

/**

  C-wrapper of CPUID instruction

  @param RegEax  - ptr to EAX register
  @param RegEbx  - ptr to EBX register
  @param RegEcx  - ptr to ECX register
  @param RegEdx  - ptr to EDX register

  @retval None

**/
VOID
GetCpuID (
    UINT32 *RegEax,  // ptr to EAX register
    UINT32 *RegEbx,  // ptr to EBX register
    UINT32 *RegEcx,  // ptr to ECX register
    UINT32 *RegEdx   // ptr to EDX register
    );

/**

  Reads 64-BIT Machine status register

  @param msrAddr - Address of MSR to read

  @retval UINT64_STRUCT - MSR value as read from CPU

**/
UINT64_STRUCT
ReadMsrLocal (
    UINT32 msrAddr    // Address of MSR to read
    );

/**

  Writes 64-BIT Machine status register

  @param msrAddr - Address of MSR to write
  @param data    - 64 bit data to write to MSR

  @retval None

**/
void
WriteMsrLocal (
    UINT32        msrAddr,    // Address of MSR to write
    UINT64_STRUCT data        // 64 bit data to write to MSR
    );

/**

  Function to initialize the CPU timer\n
    host->nvram.common.cpuFreq = Cpu Frequency (in Hz) intialized\n
    host->var.common.startTsc = Current Time Stamp Counter value

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
InitCpuTimer (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Reads the time stamp counter

  @param count - location to read the time stamp counter into

  @retval None

**/
void
ReadTsc64 (
    UINT64_STRUCT *count // location to read the time stamp counter into
    );

#define  TDIFF_UNIT_S     1
#define  TDIFF_UNIT_MS    1000
#define  TDIFF_UNIT_US    1000000
#define  TDIFF_UNIT_NS    1000000000

/**

  Finds the time delta between two Time Stamp Counter values in the given time units.

  @param host       - Pointer to sysHost, the system host (root) structure
  @param startCount - First Time Stamp Counter value
  @param endCount   - Second Time Stamp Counter value
  @param unit       - For Time in Seconds (TDIFF_UNIT_S) = 1 \n
                      For Time in milliseconds (TDIFF_UNIT_MS) = 1000 \n
                      For Time in microseconds (TDIFF_UNIT_US) = 1000000 \n
                      For Time in nanoseconds (TDIFF_UNIT_NS) = 1000000000


  @retval Time delta in provided units

**/
UINT32
TimeDiff (
    PSYSHOST  host,             // Pointer to the system host (root) structure
    UINT64_STRUCT startCount,   // First Time Stamp Counter value
    UINT64_STRUCT endCount,     // Second Time Stamp Counter value
    UINT32        unit          // For Time in Seconds (TDIFF_UNIT_S) = 1
    // For Time in milliseconds (TDIFF_UNIT_MS) = 1000
    // For Time in microseconds (TDIFF_UNIT_US) = 1000000
    // For Time in nanoseconds (TDIFF_UNIT_NS) = 1000000000
    );

/**

  Checks setup option for halt on error and if set halts the platform

  @param host       - Pointer to sysHost, the system host (root) structure
  @param majorCode  - Major error code
  @param minorCode  - Minor error code

  @retval None

**/
void
HaltOnError (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8    majorCode,   // Major error code
    UINT8    minorCode    // Minor error code
    );

/**

  Log an Error to the host structure

  @param host       - Pointer to sysHost, the system host (root) structure
  @param majorCode  - Major error code
  @param minorCode  - Minor error code
  @param Socket     - Socket error occurred on
  @param BistMask   - 32 bit Bist Mask results

  @retval None

**/
void
LogCpuError (
    PSYSHOST  host,        // Pointer to the system host (root) structure
    UINT8    majorCode,    // Major error code
    UINT8    minorCode,    // Minor error code
    UINT8    Socket,       // Socket error occurred on
    UINT32   BistMask      // 32 bit Bist Mask results
    );

void
LogError (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8    majorCode,   // Major error code
    UINT8    minorCode,    // Minor error code
    UINT32   logData        // 32 bit error specific data to log
    );

/**

  Log a Warning to the host structure

  @param host       - Pointer to sysHost, the system host (root) structure
  @param majorCode  - Major error code
  @param minorCode  - Minor error code
  @param logData    - 32 bit error specific data to log

  @retval None

**/
void
LogWarning (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8    majorCode,   // Major error code
    UINT8    minorCode,    // Minor error code
    UINT32   logData        // 32 bit error specific data to log
    );

/**

  Log Error to the warning log

  @param host              - Pointer to sysHost, the system host (root) structure
  @param ErrorCode         - Major warning code
  @param minorErrorCode    - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

  @retval None

**/
void
OutputError (
    PSYSHOST  host,     // Pointer to the system host (root) structure
    UINT8    ErrorCode,         // Major warning code
    UINT8    minorErrorCode,    // Minor warning code
    UINT8    socket,              // socket number
    UINT8    ch,                  // channel number
    UINT8    dimm,                // dimm number
    UINT8    rank                 // rank number
    );

/**

  Log warning to the warning log

  @param host              - Pointer to sysHost, the system host (root) structure
  @param warningCode       - Major warning code
  @param minorWarningCode  - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

  @retval None

**/
void
OutputWarning (
    PSYSHOST host,                // pointer to sysHost
    UINT8    warningCode,       // Major warning code
    UINT8    minorWarningCode,  // Minor warning code
    UINT8    socket,              // socket number
    UINT8    ch,                  // channel number
    UINT8    dimm,                // dimm number
    UINT8    rank                 // rank number
    );

/**

  Adds error/exception to the promoteWarningList in host structure if possible

  @param host      - Pointer to sysHost, the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval 0 - Successfully added to log
  @retval 1 - Log full

**/
UINT32
SetPromoteWarningException (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT8    majorCode,     // Major error code to log
    UINT8    minorCode      // Minor error code to log
    );

/**

  Initialize GPIOs

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
InitGpio (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Reads 8 bit IO port with serial debug output

  @param host       - Pointer to sysHost, the system host (root) structure
  @param ioAddress  - IO Address to use

  @retval Value read from ioAddress

**/
UINT8
InPort8 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress    // IO Address to use
    );

/**

  Reads 16 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

**/
UINT16
InPort16 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress    // IO Address to use
    );

/**

  Reads 32 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

**/
UINT32
InPort32 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress    // IO Address to use
    );

/**

  Writes 8 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval None

**/
void
OutPort8 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress,   // IO Address to use
    UINT8 data            // Data to write to IO Address
    );

/**

  Writes 16 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval None

**/
void
OutPort16 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress,   // IO Address to use
    UINT16 data           // Data to write to IO Address
    );

/**

  Writes 32 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval None

**/
void
OutPort32 (
    PSYSHOST  host,   // Pointer to the system host (root) structure
    UINT16   ioAddress,   // IO Address to use
    UINT32 data           // Data to write to IO Address
    );

/**

  Get the PE entry point

  @param host        - Pointer to sysHost, the system host (root) structure
  @param baseAddress - Pointer to the base address

  @retval PE Entry Point Address

**/
UINT32 GetPeEntrypoint(
  PSYSHOST  host,     // Pointer to sysHost, the system host (root) structure
    UINT8 *baseAddress  // Pointer to the base address
    );

/**

  Basic Fine ERID training uses a HW generated pattern from NVMCTLR

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
EarlyReadID (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Read and return a linear date based on year, month, and day of the month from RTC

  @param host - Pointer to sysHost, the system host (root) structure

  @retval linear approximate # of day's since Jan 1 2000

**/
UINT16
MemGetCmosDate (
    PSYSHOST  host    // Pointer to the system host (root) structure
    );

/**

  Check if any processor has been changed across boots

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE - Processor has been changed
  @retval FALSE - Processor has not been changed

**/
UINT32
ProcPpinChangedCheck (
    PSYSHOST  host,    // Pointer to the system host (root) structure
    UINT8     socket
    );

/**

  Rotates 64-bit data left by given count

  @param Data   - 64-bit data to rotate
  @param Count  - Number of bits to rotate

  @retval UINT64_STRUCT - Rotated 64-bit result

**/
UINT64_STRUCT
RotateLeftUINT64 (
  UINT64_STRUCT Data,  // 64-bit data to rotate
  UINT32        Count  // Number of bits to rotate
  );

/**

  Rotates 64-bit data right by given count

  @param Data   - 64-bit data to rotate
  @param Count  - Number of bits to rotate

  @retval UINT64_STRUCT - Rotated 64-bit result

**/
UINT64_STRUCT
RotateRightUINT64 (
  UINT64_STRUCT Data,  // 64-bit data to rotate
  UINT32        Count  // Number of bits to rotate
  );

/**

  Limit check to ensure max value is not exceeded

  @param maxVal - The largest possible value that val can be
  @param val    - The value to be tested

  @retval lesser of val or maxVal

**/
UINT32
Saturate (
  UINT32    maxVal,    // The largest possible value that val can be
  UINT32    val        // The value to be tested
  );

/**

  For each setting of RX DQS PI setting (64 options): \n
    Multicast the RX DQS PI setting to all CSRs \n
    BIOS clears errors \n
    BIOS waits until all reads have finished \n
    BIOS reads the results from BW_SERR CSRs

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - Processor socket
  @param dimm         - Specific DIMM on this Channel
  @param rank         - Rank on this DIMM
  @param ErrorResult  - Pointer to the error results

  @retval SUCCESS

**/
UINT32
FindPiCompareResult (
  PSYSHOST     host,          // Pointer to sysHost, the system host (root) structure
  UINT8        socket,        // Processor socket
  UINT8        dimm,          // Specific DIMM on this Channel
  UINT8        rank,          // Rank on this DIMM
  TErrorResult *ErrorResult   // Pointer to the error results
  );


//
// HBM API
//

/**

  HBM RC wrapper code.

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval MRC_STATUS

**/
UINT32
HbmMain (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );

/**

  Initialize  HBM data structures.

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval N/A

**/
UINT32
  HbmInit (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );

/**

  Initialize  HBM IO via FSM mechanism.

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval MRC_STATUS

**/
UINT32
HbmBasicTraining (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );

/**

HBM Discovery.

@param host  - Pointer to sysHost, the system host (root) structure

@retval N/A

**/
UINT32
HbmDiscovery (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );

/**

Display HBM Memory information

@param host  - Pointer to sysHost, the system host (root) structure

@retval N/A

**/
UINT32
HbmMemDisplay (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );


/**

This training step sets the correct number of delay stages that are
enabled for RX DQ.

@param host  - Pointer to sysHost

@retval N/A

**/
UINT32
HbmRxDqDelay (
  PSYSHOST host         // Pointer to sysHost, the system host (root) structure
  );


//
// HBM Test API GetSet
//
/**

HBM RC wrapper code.

@param host  - Pointer to sysHost, the system host (root) structure

@retval MRC_STATUS

**/
UINT32
HbmTestGetSetApi (
  PSYSHOST host   // Pointer to sysHost, the system host (root) structure
  );

void EwlOutputType1 (
  PSYSHOST host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
);

void
EwlOutputType2 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        strobe,
  UINT8        bit,
  GSM_GT       group,
  GSM_LT       level,
  UINT8        eyeSize
);

void
EwlOutputType3 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  GSM_GT       group,
  GSM_LT       level,
  GSM_CSN      signal,
  UINT8        eyeSize
);

void
EwlOutputType4 (
  PSYSHOST          host,
  UINT8             warningCode,
  UINT8             minorWarningCode,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             rank,
  EWL_SEVERITY      severity,
  ADV_MT_TYPE       memtestType,
  EWL_ADV_MT_STATUS advMemtestErrorInfo,
  UINT32            count
);

void
EwlOutputType5 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        subRank,
  UINT8        bankAddress,
  UINT8*       dqBytes
);

//
// Constructs type 7 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType7 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        Socket,
  UINT32       BistMask
);

void
EwlOutputType10 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        swizzlePattern,
  UINT8        swizzledDqLanes,
  UINT8        lanesPerStrobe,
  UINT8        strobe
);

VOID
EwlPrintBasicInfo (
  PSYSHOST host,
  EWL_ENTRY_HEADER *entryHeader
);

VOID
EwlPrintMemoryLocation (
  PSYSHOST host,
  EWL_ENTRY_MEMORY_LOCATION memLoc
);

VOID
EwlPrintWarning (
  PSYSHOST host,
  EWL_ENTRY_HEADER *header
);

UINT32
CountWarningsInLog (
  PSYSHOST host
  );

void
HandleScadExit (
  PSYSHOST host,
  UINT8    socket
  );

/**

  Install the Mrc Hooks Core Layer Services PPI

  @param host     - Pointer to sysHost, the system host (root) structure

  @retval 0 - Successfully installed PPI
  @retval non-zero - Failure occurs when installing PPI

**/
UINT32
InstallMrcHooksServicesPpi (
  PSYSHOST host
  );

#endif // _CORE_API_H_
