/**
//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MemApiSkx.h

  Prototypes for functions assigned to DDR_FUNCTION_PTRS

**/

#ifndef _MEM_API_SKX_H_
#define _MEM_API_SKX_H_

#include "SysFunc.h"
#include "DataTypes.h"
#include "SysHost.h"
#include <UsraAccessType.h>

#include "MemHostChipCommon.h"
#include "MrcCommonTypes.h"
#include "Cpgc.h"


void   DisplayTcwlAdj(PSYSHOST host, UINT8 socket, UINT8 ch, GSM_GT group);
void   DisplayXoverResults(PSYSHOST  host, UINT8     socket);
void   DisplayXoverResultsFnv(PSYSHOST host, UINT8 socket);
void   DisplayResultsChip (PSYSHOST host, UINT8 socket);

MRC_STATUS
GetSetDataGroup(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     dimm,                             // DIMM number within a DDR channel (0-based, farthest slot == 0)
    UINT8     rank,                             // Logical rank number within a DIMM (0-based)
    UINT8     strobe,                           // Dqs data group within the rank
    UINT8     bit,                              // Bit index within the data group
    GSM_LT    level,                            // IO level to access
    GSM_GT    group,                            // Data group to access
    UINT8     mode,                             // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                                                //   program register (used for fast boot or S3)
    INT16     *value                            // Pointer to absolute value or offset based on mode
    );

MRC_STATUS
GetSetTxDelay (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT8       strobe,                           // Strobe number (0-based)
  UINT8       bit,                              // Bit number
  GSM_LT      level,                            // CSM_LT - Level to program (DDR, VMSE, Buffer)
  GSM_GT      group,                            // CSM_GT - Parameter to program
  UINT8       mode,                             // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                                                //   program register (used for fast boot or S3)
  INT16       *value                            // Pointer to delay value or offset based on mode
  );

MRC_STATUS
GetSetRxDelay (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT8       strobe,                           // Strobe number (0-based)
  UINT8       bit,                              // Bit number
  GSM_LT      level,                            // CSM_LT - Level to program (DDR, VMSE, Buffer)
  GSM_GT      group,                            // CSM_GT - Parameter to program
  UINT8       mode,                             // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                                                //   program register (used for fast boot or S3)
  INT16       *value                            // Pointer to delay value or offset based on mode
  );

MRC_STATUS
GetSetRxDelayBit (
    PSYSHOST    host,                           // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                         // Socket Id
    UINT8       ch,                             // Channel number (0-based)
    UINT8       dimm,                           // Current dimm
    UINT8       rank,                           // Rank number (0-based)
    UINT8       strobe,                         // Strobe number (0-based)
    UINT8       bit,                            // Bit number
    GSM_LT      level,                          // CSM_LT - Level to program (DDR, VMSE, Buffer)
    GSM_GT      group,                          // CSM_GT - Parameter to program
    UINT8       mode,                           // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                                                //   program register (used for fast boot or S3)
    INT16       *value                          // Pointer to delay value or offset based on mode
  );

MRC_STATUS
GetSetRxVref (
  PSYSHOST    host,                           // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                         // Socket Id
  UINT8       ch,                             // Channel number (0-based)
  UINT8       dimm,                           // Current dimm
  UINT8       rank,                           // Rank number (0-based)
  UINT8       strobe,                         // Strobe number (0-based)
  UINT8       bit,                            // Bit number
  GSM_LT      level,                          // CSM_LT - Level to program (DDR, VMSE, Buffer)
  GSM_GT      group,                          // CSM_GT - Parameter to program
  UINT8       mode,                           // 0 - do not read from cache, 1 - read from cache, 2 - use cache to
                                              //   program register (used for fast boot or S3)
  INT16       *value                          // Pointer to delay value or offset based on mode
  );

UINT16
CodeDeCodeRxOdt (
  UINT16 value,                                 // Value to be converted
  UINT8  code                                   // CODE_ODT or DECODE_ODT
);

UINT16
CodeDeCodeTxRon (
  UINT16 value,                                 // Value to be converted
  UINT8  code                                   // CODE_ODT or DECODE_ODT
);

UINT8
GetVrefRange (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       vref                              // Vref value to convert
  );

void
UpdateSafeTxVref (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  GSM_LT      level                             // CSM_LT - Level to program (DDR, VMSE, Buffer)
  );

MRC_STATUS
GetDataGroupLimits(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    GSM_LT    level,                            // IO level to access
    GSM_GT    group,                            // Data group to access
    UINT16    *minLimit,                        // Minimum delay value allowed
    UINT16    *maxLimit,                        // Maximum absolute value allowed
    UINT16    *usdelay                          // Time in usec required for new setting to take effect
    );

void
IO_Reset (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

void
SetTrainingMode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rank,                             // Rank number (0-based)
  UINT8       mode                              // Training mode
  );

void
DoZQ (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       zqType                            // Type of ZQ Calibration: Long or Short
  );

VOID
GetOriginalRtlChip(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT8       RtlInitializationMode,            // RTL Initialization Mode: RTL_INITIALIZE_ONCE or RTL_RE_INITIALIZE
  UINT8       *roundTripLatency                 // Pointer to the round trip latency
  );

void
SetIOLatency (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rank,                             // Rank number (0-based)
  UINT32      IOLatencyValue                    // New IO Latency value
  );

UINT8
GetIOLatency (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rank                              // Rank number (0-based)
  );

UINT8
SetRoundTrip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rank,                             // Rank number (0-based)
  UINT8       RoundTripValue                    // New Round Trip Latency value
  );

UINT8
GetRoundTrip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rank                              // Rank number (0-based)
  );

void   ClearBWErrorStatus(PSYSHOST host, UINT8 socket, UINT8 ch);                                 // CHIP
CHAR   *GetGroupStr(GSM_GT group, CHAR *strBuf);
void   SetRestoreTimings(PSYSHOST host, UINT8 socket, UINT8 ch);
MRC_STATUS GetDdr4OdtValueTable (PSYSHOST host, struct ddr4OdtValueStruct **tablePtr, UINT16 *tableSize);
BOOLEAN InCsrTraceRegsSkipList (PSYSHOST host, UINT32 RegOffset);

void CheckBoundsChip(PSYSHOST host, MEM_TIMING_TYPE timing, UINT32 *value);
MRC_STATUS GetCallTablePtrs (PSYSHOST host, CallTableEntryType **MasterCallTable, UINT8 *MasterTableSize, CallTableEntryType **SlaveCallTable, UINT8 *SlaveTableSize);

/**

  Get the current timing mode

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)

  @retval (UINT8)Current timing mode

**/
UINT8
GetCurrentTimingMode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Set Timing Mode

  @param host            - Pointer to sysHost
  @param socket          - Socket Id
  @param ch              - Channel number (0-based)
  @param cmdTiming       - Timing value to set

  @retval (UINT8)CMD Timing

**/
UINT8
SetTimingMode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       cmdTiming                         // Timing value to set
  );

/**

  This routine performs the Memory Init on all the DIMMs present on MCs using patrol scrub
  engine and returns the operation result in the supplied MEM_IT_STATUS_STRUC array.

  NOTE: The memory Init is performed in Flat Memory Mode and All2All Memory Model.

  @param  host     - Pointer to sysHost, the system host (root) structure
  @param  socket   - Socket number

  @retval None

**/
VOID
MemoryInitDdr (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  For certain DCLK to UCLK settings we need to add bubbles

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket to switch

  @retval None

**/
void
ProgramBgfTable (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

void
UpdateDdrtGroupLimits (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  GSM_LT   level,                            // IO level to access
  GSM_GT   group,                            // Data group to access
  UINT16   *minLimit,                        // Minimum delay value allowed
  UINT16   *maxLimit                         // Maximum absolute value allowed
);

void
FnvCWLWorkaround (
                  PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch,
                  UINT8    dimm
);

/**

  Offsets CCC IO delays

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval void

**/
void
OffsetCCC(
          PSYSHOST  host,
          UINT8     socket,
          INT16     offset,
          UINT8     jedecinit
);

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param group    - Group number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program
  @param *minVal  - Current minimum command delay
  @param *maxVal  - Current maximum command delay

  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdGroupDelay (
    PSYSHOST host,                              // Pointer to sysHost, the system host (root) structure
    UINT8 socket,                               // Processor socket within the system (0-based)
    UINT8 ch,                                   // DDR channel number within the processor socket (0-based)
    GSM_GT group,                               // Platform Control Group
    UINT8 mode,                                 // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16 *value,                               // Value to program or offset
    UINT16 *minVal,                             // Updated if any value in the group is less than minVal
    UINT16 *maxVal                              // Updated if any value in the group is more than maxVal
    );

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - DIMM number
  @param group    - Group number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program
  @param *minVal  - Current minimum command delay
  @param *maxVal  - Current maximum command delay

  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdGroupDelayFnv (
    PSYSHOST host,                              // Pointer to sysHost, the system host (root) structure
    UINT8 socket,                               // Processor socket within the system (0-based)
    UINT8 ch,                                   // DDR channel number within the processor socket (0-based)
    UINT8 dimm,                                 // Current dimm
    GSM_GT group,                               // Platform Control Group
    UINT8 mode,                                 // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16 *value,                               // Value to program or offset
    UINT16 *minVal,                             // Updated if any value in the group is less than minVal
    UINT16 *maxVal                              // Updated if any value in the group is more than maxVal
    );

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param group    - Group number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program
  @param *minVal  - Current minimum command delay
  @param *maxVal  - Current maximum command delay

  @retval minVal, maxVal and MRC_STATUS

**/
MRC_STATUS
GetSetCtlGroupDelay (
    PSYSHOST host,                              // Pointer to sysHost, the system host (root) structure
    UINT8 socket,                               // Processor socket within the system (0-based)
    UINT8 ch,                                   // DDR channel number within the processor socket (0-based)
    GSM_GT group,                               // Platform Control Group
    UINT8 mode,                                 // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16 *value,                               // Value to program or offset
    UINT16 *minVal,                             // Updated if any value in the group is less than minVal
    UINT16 *maxVal                              // Updated if any value in the group is more than maxVal
    );

/**
  Muli-use function to either get or set command vref

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - DIMM number within a DDR channel (0-based, farthest slot == 0)
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program

  @retval minVal and maxVal

**/
MRC_STATUS
GetSetCmdVref (
    PSYSHOST host,                              // Pointer to sysHost, the system host (root) structure
    UINT8 socket,                               // Processor socket within the system (0-based)
    UINT8 ch,                                   // DDR channel number within the processor socket (0-based)
    UINT8 dimm,                                 // DIMM number within a DDR channel (0-based, farthest slot == 0)
    GSM_LT level,                               // IO level to access
    UINT8 mode,                                 // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16 *value                                // Value to program or offset
    );

/**
  Multi-use function to either get or set ERID vref

  @param host:     Pointer to SYSHOST
  @param socket:   Socket number
  @param ch:       Channel number
  @param mode:     GSM_READ_CSR - Read the data from hardware and not cache
                   GSM_READ_ONLY - Do not write
                   GSM_WRITE_OFFSET - Write offset and not value
                   GSM_FORCE_WRITE - Force the write
  @param value:    Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetEridVref(
    PSYSHOST host,                              // Pointer to sysHost, the system host (root) structure
    UINT8 socket,                               // Processor socket within the system (0-based)
    UINT8 ch,                                   // DDR channel number within the processor socket (0-based)
    UINT8 mode,                                 // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16 *value                                // Value to program or offset
);

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param ioGroup  - Group number
  @param side     - Side number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetCmdDelay (
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     ioGroup,                          // Group number
    UINT8     side,                             // Side number
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16     *value                            // Value to program or offset
    );

/**
  Muli-use function to either get or set command delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param ioGroup  - Group number
  @param side     - Side number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetCmdDelayFnv (
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     dimm,
    GSM_GT    ioGroup,                          // Group number
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16     *value                            // Value to program or offset
    );

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param ioGroup  - Group number
  @param side     - Side number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program or offset

  @retval Pi delay value

**/
MRC_STATUS
GetSetCtlDelay(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     ioGroup,                          // Group number
    UINT8     side,                             // Side number
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16     *value                            // Value to program or offset
    );

/**
  Muli-use function to either get or set control delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param dimm     - DIMM number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program or offset
  @param *minVal  - Current minimum control delay
  @param *maxVal  - Current maximum control delay

  @retval Pi delay value

**/
MRC_STATUS
GetSetCtlDelayFnv(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     dimm,                             // Current dimm
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16     *value,                           // Value to program or offset
    UINT16    *minVal,                          // Updated if any value in the group is less than minVal
    UINT16    *maxVal                           // Updated if any value in the group is more than maxVal
    );

/**
  Muli-use function to either get or set clock delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param clk      - Clock number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program

  @retval Pi delay value

**/
MRC_STATUS
GetSetClkDelay(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     clk,                              // Clock number
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16      *value                           // Value to program
    );

/**
  Muli-use function to either get or set clock delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param clk      - Clock number
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Value to program

  @retval Pi delay value

**/
MRC_STATUS
GetSetClkDelayFnv(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    UINT8     clk,                              // Clock number
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16      *value                           // Value to program
    );

/**
  Multi-use function to either get or set signal delays based on the provided group number

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number
  @param level    - IO level to access
  @param sig      - Enumerated signal name
  @param mode     - GSM_READ_CSR - Read the data from hardware and not cache
                  -  GSM_READ_ONLY - Do not write
                  -  GSM_WRITE_OFFSET - Write offset and not value
                  -  GSM_FORCE_WRITE - Force the write
  @param value    - Absolute value or offset selected by GSM_WRITE_OFFSET

  @retval MRC_STATUS

**/
MRC_STATUS
GetSetSignal(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    GSM_LT    level,                            // IO level to access
    GSM_CSN   sig,                              // Enumerated signal name
    UINT8     mode,                             // GSM_READ_CSR - Read the data from hardware and not cache
                                                // GSM_READ_ONLY - Do not write
                                                // GSM_WRITE_OFFSET - Write offset and not value
                                                // GSM_FORCE_WRITE - Force the write
    INT16      *value                           // Value to program
    );

/**

  Quick centering of Vref

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param group   - Parameter to center
                   RxDqsDelay   0: Center Rx DqDqs
                   TxDqDelay    1: Center Tx DqDqs

  @retval SUCCESS

**/
UINT8
CmdVrefQuick(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    GSM_GT    group                             // Data group to access
    );

/**

  Set starting/initial values for Clock and Control signals
  Initial values come from Analog Design

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Processor socket within the system (0-based)
  @param level     - IO level to access
  @param group     - Command, clock or control group to access
  @param *minLimit - Minimum delay value allowed
  @param *maxLimit - Maximum delay value allowed

  @retval MRC_STATUS

**/
MRC_STATUS
GetCmdGroupLimits(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    GSM_LT    level,                            // IO level to access
    GSM_GT    group,                            // Command, clock or control group to access
    UINT16    *minLimit,                         // Minimum delay value allowed
    UINT16    *maxLimit                         // Maximum absolute value allowed
    );

/**

  Given an array of command delays relative to current clock and control delays,
  this function will combine shared settings in the DDRIO design and normalize the
  lowest command, clock or control value to their minimum limits.

  @param host      - Pointer to sysHost, the system host (root) structure struct
  @param socket    - Socket
  @param ch        - Channel
  @param listSize  - Number of entries in each list
  @param value     - Array of pointers to listType

  @retval MRC_STATUS_SUCCESS
  @retval MRC_STATUS_SIGNAL_NOT_SUPPORTED

**/
MRC_STATUS SetCombinedCtlGroup(PSYSHOST host, UINT8 socket, UINT8 ch, UINT16 listSize, VOID *value);                              // CHIP

/**

  Given an array of command delays relative to current clock and control delays,
  this function will combine shared settings in the DDRIO design and normalize the
  lowest command, clock or control value to their minimum limits.

  @param host                 - Pointer to host structure
  @param socket               - Processor socket within the system (0-based)
  @param ch                   - DDR channel number within the processor socket (0-based)
  @param level                - IO level to access
  @param mode                 - Bit-field of different modes
  @param listType             - Selects type of each list
  @param listSize             - Number of entries in each list
  @param *value               - VOID pointer that points to an Array of pointers to listType

  @retval MRC_STATUS

**/
MRC_STATUS
SetCombinedCmdGroup(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // Processor socket within the system (0-based)
    UINT8     ch,                               // DDR channel number within the processor socket (0-based)
    GSM_LT    level,                            // IO level to access
    UINT8     mode,                             // Bit-field of different modes
    UINT8     listType,                         // Selects type of each list
    UINT16    listSize,                         // Number of entries in each list
    VOID      *value                            // Array of pointers to listType
    );

/**

  Get the DCLKs

  @param index         - Index into the DCLK array
  @param ddrFreqLimit  - The current bclk setting

  @retval DCLKs
**/
UINT8
GetDCLKs(
    UINT8 index,                                // Index into the DCLK array
    UINT8 ddrFreqLimit                          // The current bclk setting
    );

/**

    Get the DCLK Ratio

    @param host     - Pointer to sysHost, the system host (root) structure
    @param socket   - Current Socket
    @param req_type - Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_type \n
                         0h     MC frequency request for 133 Mhz \n
                         1h     MC frequency request for 100 Mhz \n
                         All other values are reserved.
    @param req_data - Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_data \n
                        The data for the request. \n
                        The encoding of this field is the 133 MHz multiplier for DCLK/QCLK: \n
                        Binary     Dec       DCLK Equation      DCLK Freq         QCLK Equation        QCLK Freq \n
                       ... \n
                        00110b    6d         3 * 133.33          400.00 MHz       6  * 133.33 MHz      800.00 MHz  \n
                        01000b    8d         4 * 133.33          533.33 MHz       8  * 133.33 MHz      1066.67 MHz \n
                        01010b    10d        5 * 133.33          666.67 MHz       10 * 133.33 MHz      1333.33 MHz \n
                        01100b    12d        6 * 133.33          800.00 MHz       12 * 133.33 MHz      1600.00 MHz \n
                        01110b    14d        7 * 133.33          933.33 MHz       14 * 133.33 MHz      1866.67 MHz \n
                        10000b    16d        8 * 133.33          1066.67 MHz      16 * 133.33 MHz      2133.33 MHz \n
                       ...

    @retval None

**/
VOID
GetDclkRatio (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket within the system (0-based)
  UINT8       *req_type,                        // Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_type
                                                //    0h     MC frequency request for 133 Mhz
                                                //    1h     MC frequency request for 100 Mhz
                                                //    All other values are reserved.
  UINT8       *req_data                         // Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_data
                                                //   The data for the request.
                                                //   The encoding of this field is the 133 MHz multiplier for DCLK/QCLK:
                                                //   Binary     Dec       DCLK Equation      DCLK Freq         QCLK Equation        QCLK Freq
                                                //  ...
                                                //   00110b    6d         3 * 133.33          400.00 MHz       6  * 133.33 MHz      800.00 MHz
                                                //   01000b    8d         4 * 133.33          533.33 MHz       8  * 133.33 MHz      1066.67 MHz
                                                //   01010b    10d        5 * 133.33          666.67 MHz       10 * 133.33 MHz      1333.33 MHz
                                                //   01100b    12d        6 * 133.33          800.00 MHz       12 * 133.33 MHz      1600.00 MHz
                                                //   01110b    14d        7 * 133.33          933.33 MHz       14 * 133.33 MHz      1866.67 MHz
                                                //   10000b    16d        8 * 133.33          1066.67 MHz      16 * 133.33 MHz      2133.33 MHz
                                                //  ...
  );

/**

    Set the DCLK Ratio

    @param host     - Pointer to sysHost, the system host (root) structure
    @param socket   - Current Socket
    @param req_type - Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_type \n
                         0h     MC frequency request for 133 Mhz \n
                         1h     MC frequency request for 100 Mhz \n
                         All other values are reserved.
    @param req_data - Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_data \n
                        The data for the request. \n
                        The encoding of this field is the 133 MHz multiplier for DCLK/QCLK: \n
                        Binary     Dec       DCLK Equation      DCLK Freq         QCLK Equation        QCLK Freq \n
                       ... \n
                        00110b    6d         3 * 133.33          400.00 MHz       6  * 133.33 MHz      800.00 MHz  \n
                        01000b    8d         4 * 133.33          533.33 MHz       8  * 133.33 MHz      1066.67 MHz \n
                        01010b    10d        5 * 133.33          666.67 MHz       10 * 133.33 MHz      1333.33 MHz \n
                        01100b    12d        6 * 133.33          800.00 MHz       12 * 133.33 MHz      1600.00 MHz \n
                        01110b    14d        7 * 133.33          933.33 MHz       14 * 133.33 MHz      1866.67 MHz \n
                        10000b    16d        8 * 133.33          1066.67 MHz      16 * 133.33 MHz      2133.33 MHz \n
                       ...

    @retval None

**/
VOID
SetDclkRatio (
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                             // Processor socket within the system (0-based)
  UINT8     req_type,                           // Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_type
                                                //    0h     MC frequency request for 133 Mhz
                                                //    1h     MC frequency request for 100 Mhz
                                                //    All other values are reserved.
  UINT8     req_data                            // Settings based on MC_BIOS_REQ_PCU_FUN1_STRUCT req_data
                                                //   The data for the request.
                                                //   The encoding of this field is the 133 MHz multiplier for DCLK/QCLK:
                                                //   Binary     Dec       DCLK Equation      DCLK Freq         QCLK Equation        QCLK Freq
                                                //  ...
                                                //   00110b    6d         3 * 133.33          400.00 MHz       6  * 133.33 MHz      800.00 MHz
                                                //   01000b    8d         4 * 133.33          533.33 MHz       8  * 133.33 MHz      1066.67 MHz
                                                //   01010b    10d        5 * 133.33          666.67 MHz       10 * 133.33 MHz      1333.33 MHz
                                                //   01100b    12d        6 * 133.33          800.00 MHz       12 * 133.33 MHz      1600.00 MHz
                                                //   01110b    14d        7 * 133.33          933.33 MHz       14 * 133.33 MHz      1866.67 MHz
                                                //   10000b    16d        8 * 133.33          1066.67 MHz      16 * 133.33 MHz      2133.33 MHz
                                                //  ...
  );

/**

    Get Maximum Frequency

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Current Socket

    @retval maxDdrFreq

**/
UINT8
GetMaxFreq (
  PSYSHOST   host,                              // Pointer to sysHost, the system host (root) structure
  UINT8      socket                             // Processor socket ID
  );

/**

    Set tWR for this channel

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Current socket
    @param ch     - Current channel
    @param nWR    - nWR value

    @retval None

**/
void
SetChTwr (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket within the system (0-based)
  UINT8       ch,                               // DDR channel number within the processor socket (0-based)
  UINT8       nWR                               // nWR value
  );

/**

    Set Data Timing

    @param host       - Pointer to sysHost, the system host (root) structure
    @param socket     - Socket Id
    @param ch         - Channel number (0-based)
    @param nCL        - nCL value
    @param nCWL       - nCWL value

    @retval None

**/
VOID
SetDataTiming (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket within the system (0-based)
  UINT8       ch,                               // DDR channel number within the processor socket (0-based)
  UINT8       nCL,                              // nCL value
  UINT8       nCWL                              // nCWL value
  );

/**

  Chip specific early control clock for LRDIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval None

**/
void
PerformChipLRDIMMEarlyCtlClk(
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket                              // Processor socket ID
  );

/**

  Turn on command add operation enable

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket number
  @param ch                  - DDR channel (0-based)
  @param chipErlyCmdCKStruct - Pointer to Early CMD CK structure

  @retval None

**/
void
TrnOnChipCMDADDOpEn(
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                             // Processor socket ID
  UINT8     ch,                                 // DDR channel (0-based)
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct  // Pointer to Early CMD CK structure
  );

/**

  Turn off command add operation enable

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket number
  @param ch                  - DDR channel (0-based)
  @param chipErlyCmdCKStruct - Pointer to Early CMD CK structure

  @retval None

**/
void
TrnOffChipCMDADDOpEn(
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                             // Processor socket ID
  UINT8     ch,                                 // DDR channel (0-based)
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct  // Pointer to Early CMD CK structure
  );

/**

  Detect SPD Type DIMM configuration

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket ID
  @param ch      - Channel on socket
  @param dimm    - DIMM on channel
  @param keyByte - Key byte

  @retval SUCCESS

**/
UINT32
DetectSpdTypeDIMMConfig (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       keyByte                           // Key byte holds the DIMM type information
  );

/**

    Get the Serial Number of the requested module

    @param host       - Pointer to sysHost, the system host (root) structure
    @param socket     - Socket ID
    @param ch         - Channel on socket
    @param dimm       - DIMM on channel
    @param i          - The number of the Module serial byte to check
    @param SPDReg     - Pointer to the SPD Register

    @retval None

**/
void
GetModuleSerialNumber(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       i,                                // The number of the module serial byte to check
  UINT8       *SPDReg                           // Pointer to the SPD Register
  );

/**

    GetDdr4SpdPageEn - Detect DDR4 vs. DDR3 SPD devices on SMBus

    @param host       - Pointer to sysHost, the system host (root) structure
    @param socket     - Socket ID

    @retval 0 - DDR3 SPD
    @retval 1 - DDR4 SPD

**/
UINT8
GetDdr4SpdPageEn (
  PSYSHOST         host,                        // Pointer to sysHost, the system host (root) structure
  UINT8            socket                       // Socket Id
  );

/**

    display processor abbreviation, stepping and socket

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Processor socket ID

    @retval None

**/
VOID
DimmInfoDisplayProc (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Processor socket ID
  );

/**

    Display DIMM Info

    @param host   - Pointer to sysHost, the system host (root) structure

    @retval None

**/
VOID
DimmInfoDisplayProcFeatures (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Enables CMD/ADDR Parity

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket

  @retval None

**/
void
EnableCAParityRuntime (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Processor socket ID
  );

/**

    Get LV Mode

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Processor socket

    @retval (UINT8)ddrCRCompCtl0.Bits.lvmode

**/
UINT8
GetLvmode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Processor socket ID
  );

#ifdef SERIAL_DBG_MSG
/**
  Return a copy of the string for the given signal

  @param signal - The signal name to access
  @param strBuf - Buffer to copy the signal string to

  @retval Pointer to the copy of the string
**/
char
*GetSignalStr(
  GSM_CSN     signal,                           // Signal name to access
  char        *strBuf                           // String buffer
);

/**
  Return a copy of the string for the given group

  @param host   - Pointer to sysHost, the system host (root) structure
  @param group  - The group name to access
  @param strBuf - Buffer to copy the signal string to

  @retval Pointer to the copy of the string
**/
char
*GetPlatformGroupStr(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  GSM_GT      group,                            // Platform Command group to test
  char        *strBuf                           // String buffer
);

char
*GetLevelStr(
  GSM_LT      level,
  char        *strBuf
);
#endif  // SERIAL_DBG_MSG

/**

    Print Memory Setup Options

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Current socket

    @retval None

**/
VOID
PrintMemSetupOptionsChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Processor socket ID
  );

/**

    Adjust roundtrip for MPR mode

    @param host     - Pointer to sysHost, the system host (root) structure
    @param socket   - Socket Id
    @param ch       - Channel number (0-based)
    @param dimm     - Current dimm
    @param rank     - Current rank
    @param mprMode  - MPR Mode

    @retval None

**/
void
AdjustRoundtripForMpr(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank
  UINT16      mprMode                           // MPR Mode
  );

/**

    Check to see if Rank for each socket is populated

    @param host - Pointer to sysHost, the system host (root) structure

    @retval None

**/
void
CheckRankPop (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**
  Hook for Early Command CK

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - Socket Id
  @param chipErlyCmdCKStruct  - Pointer to Early CMD CK structure

  @retval None

**/
void
ChipEarlyCmdCKSvHook(
  PSYSHOST      host,                           // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct  // Pointer to Early CMD CK structure
  );

/**

  Handles any register interaction on GetMargins entry/exit.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param entry   - TRUE when entering GetMargins, FALSE when exiting.

  @retval SUCCESS

**/
UINT32
GetMarginsHook (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  BOOLEAN     entry                             // TRUE when entering GetMargins, FALSE when exiting.
  );

/**

  Set Rank DA

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param mprMode  - Either 0 for Disable or MR3_MPR to Enable

  @retval None

 **/
void
SetRankDAMprFnv (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      mprMode                           // Either 0 for Disable or MR3_MPR to Enable
  );

/**

  Set Rank D

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param mprMode  - Either 0 for Disable or MR3_MPR to Enable

  @retval None

 **/
void
SetRankDMprFnv (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      mprMode                           // Either 0 for Disable or MR3_MPR to Enable
  );

/**

  Program DA Pattern

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param pattern  - Pattern to program

  @retval None

 **/
void
ProgramDAFnvMprPattern (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      pattern                           // Pattern to program
  );

/**

  Program D Pattern

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param pattern  - Pattern to program

  @retval None

 **/
void
ProgramDFnvMprPattern (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      pattern                           // Pattern to program
  );

/**

    Set the DIMM is populated bit

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Current socket
    @param ch     - Current channel
    @param dimm   - Current dimm

    @retval None

**/
VOID
SetDimmPop (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

    Set Encoded CS Mode

    @param host     - Pointer to sysHost, the system host (root) structure
    @param socket   - Socket Id
    @param ch       - Channel number (0-based)

    @retval None

**/
VOID
SetEncodedCsMode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Gets the logical rank number

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Processor socket to check
  @param ch          - channel number
  @param dimm        - DIMM number
  @param rank        - rank number
  @param dataControl - Pointer to data control structure

  @retval None

**/
void
GetChipLogicalRank(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank
  PCHIP_DDRIO_STRUCT dataControl                // Pointer to data control structure
  );

/**

  Returns the memory controller ID

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval MCID

**/
UINT8
GetMCID (
  PSYSHOST   host,                              // Pointer to sysHost, the system host (root) structure
  UINT8      socket,                            // CPU Socket Node number (Socket ID)
  UINT8      ch                                 // Channel number
  );

/**

    Determine if rank is disabled

    @param host   - Pointer to sysHost, the system host (root) structure
    @param socket - Current socket
    @param ch     - Current channel
    @param dimm   - Current dimm
    @param rank   - Current rank

    @retval 1 - rank is disabled
    @retval 0 - rank is enabled

**/
UINT32
IsRankDisabled (
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                             // Socket Id
  UINT8     ch,                                 // Channel number (0-based)
  UINT8     dimm,                               // Current dimm
  UINT16    rank                                // Current rank
  );

/**

  Reads BCW location and updates associated MprData per DRAM device

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - Current dimm
  @param bcwFunc    - BWC Function
  @param bcwByte    - BWC Byte
  @param bcwNibble  - BWC Nibble
  @param MprData    - each byte is updated with UI[0-7] data per DRAM device

  @retval None

**/
void
ReadBcwDimmDdr4 (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       bcwFunc,                          // BWC Function
  UINT8       bcwByte,                          // BWC Byte
  UINT8       bcwNibble,                        // BWC Nibble
  UINT8       MprData[MAX_STROBE]               // each byte is updated with UI[0-7] data per DRAM device
  );

/**

  Reads MPR page/location from given rank and updates associated MprData per DRAM device

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param ch         - Channel number (0-based)
  @param dimm       - Current dimm
  @param rank       - Current rank
  @param mprPage    - MPR page
  @param mprLoc     - MPR loc
  @param MprData    - each byte is updated with UI[0-7] data per DRAM device

  @retval None

**/
void
ReadMprRankDdr4 (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank
  UINT8       mprPage,                          // MPR page
  UINT8       mprLoc,                           // MPR loc
  UINT8       MprData[MAX_STROBE]               // each byte is updated with UI[0-7] data per DRAM device
  );

/**

  Chip specific code path to updateChipMCDelay

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - processor socket ID
  @param ch                  - Processor socket Channel number
  @param channelNvList       - Pointer to the Channel nvram
  @param CRAddDelay          - Add delays
  @param WLChipCleanUpStruct - Pointer to CHIP_WL_CLEANUP_STRUCT

  @retval None

**/
void
UpdateChipMCDelay(
  PSYSHOST                host,                     // Pointer to sysHost, the system host (root) structure
  UINT8                   socket,                   // Socket Id
  UINT8                   ch,                       // Channel number (0-based)
  struct channelNvram     (*channelNvList)[MAX_CH], // Host channel list in NVRam
  INT16                   *CRAddDelay,              // Add delays
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct       // Write Leveling clean up structure pointer
  );

/**

  Update NVMCTLR access mode if supported

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket ID

  @retval None
**/
void
UpdateAccessMode(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Wait for the Mailbox interface to indicate it is ready

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Processor socket ID

  @retval None
**/
void
WaitForMailboxReady (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  A hook into chip-level code that is called during DIMM detect
  phase in MRC before warm reset. This function is called once per
  socket, after the DIMM detect is complete.

  @param host  - Pointer to sysHost, the system host (root) structure
  @param socket - socket ID

  @retval None
**/
VOID
PostDimmDetectChipHook (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Reset the DDR channels for the given socket number (or per MC for BSSA)

  host      - Pointer to sysHost, the system host (root) structure
  socket    - Socket Id
  chBitMask - Bitmask of channels to reset

  @retval None

**/
void
ResetAllDdrChannels (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT32      chBitMask                         // Bitmask of channels to reset
  );

void
DdrtBufferReset (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm
);

/**

  FIFO Train Reset

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval None
**/
void
FifoTrainReset (
                PSYSHOST host,                  // Pointer to sysHost, the system host (root) structure
                UINT8    socket                 // Socket Id
);

/**

  Check socket if MemRasFlag is set and inComingSktId is not set

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @return 1 if conditions are met
  @return 0 if conditions are not met
**/
UINT8
CheckSktId (
            PSYSHOST host,                     // Pointer to sysHost, the system host (root) structure
            UINT8    socket                    // Socket Id
);

/**

  Check if socket is BSP and if MemRasFlag is not set

  @param host    - Pointer to sysHost, the system host (root) structure

  @return 1 if conditions are met
  @return 0 if conditions are not met
**/
UINT8
CheckMemRas (
            PSYSHOST    host                   // Pointer to sysHost, the system host (root) structure
);

/**

  Init CAPID's that are used to track nested call levels for semaphores

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval None
**/
void
LoadCapId (
            PSYSHOST host,                     // Pointer to sysHost, the system host (root) structure
            UINT8    socket                    // Socket Id
);

/**

  Set the bit indicating memory initialization is complete

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
SetMemDone (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  DDR parity based command training

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param rank    - Logical rank number
  @param sig     - Signal to test
  @param mode    - CADB mode

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcAdvCmdParity (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT8       ch,                               // DDR channel to setup
  UINT8       rank,                             // Logical rank number
  UINT8       subrank,                          // Subrank rank number
  GSM_CSN     sig,                              // Signal to test
  UINT8       mode                              // CADB mode
  );

/**

  Collect DDR advanced command CPGC error status

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket to train
  @param ddrChEnMap          - 4-bit bit mask of ddr channels to test - (1 = active channel)
  @param cpgcErrorStatus     - error status for each channel

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcAdvTrainingErrorStatus (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap,                       // 4-bit bit mask of ddr channels to stop
  CPGC_ERROR_STATUS cpgcErrorStatus[MAX_CH]     // error status for each channel
  );

/*++

  DDR advanced command CPGC execution

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Memory controller to train
  @param ddrChEnMap  - 4 bit bit mask of ddr channels to test (1 = channel is selected to run tests)
  @param pattern     - ddr pattern ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2, 3 = cmd group 3)

  @retval SUCCESS
  @retval FAILURE

--*/
UINT32
CpgcAdvTrainingExecTestGroup (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap,                       // 4-bit bit mask of ddr channels to stop
  UINT8       pattern,                          // ddr pattern (0-3) ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2,
                                                //   3 = cmd  group 3)
  GSM_GT      group                             // Parameter to be margined
  );

/**

  DDR advanced command CPGC execution

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Memory controller to train
  @param ddrChEnMap  - 4-bit bit mask of ddr channels to test - (1 = channel is selected to run tests)
  @param pattern     - ddr pattern (0-3) ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2, 3 = cmd group 3)

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcAdvTrainingExecTest (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT16    pattern
  );

/**

  DDR loopback control training

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param signal  - Signal to test

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcCtlClkPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  GSM_CSN   signal
  );

/**

  DDR advanced training CPGC setup


  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket to train
  @param ddrChEnMap  - 4 bit bit mask of ddr channels to test (1 = channel is selected to run tests)
  @param rank        - logical rank number (0-7)
  @param subRank     - Sub rank
  @param group       - Parameter to be margined
  @param pattern     - ddr pattern ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2, 3 = cmd group 3)
  @param dqLoopCount - dq sequence loop count, number of times dq sequence is executed (2^(dqLoopCount - 1))

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcAdvTrainingSetup (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap,                       // 4-bit bit mask of ddr channels to stop
  UINT8       rank,                             // logical rank number (0-7)
  UINT8       subRank,                          // Sub rank
  GSM_GT      group,                            // Parameter to be margined
  UINT32      pattern,                          // ddr pattern (0-3) ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2,
                                                //   3 = cmd  group 3)
  UINT8     dqLoopCount                         // dq sequence loop count - number of times dq sequence is
                                                //   executed (2^(dqLoopCount - 1))
  );

/**

  Disable CADB AlwaysOn Mode

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - Socket number
  @param ch           - Channel number

  @retval void

**/
VOID
DisableCADBAlwaysOnMode(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  );

/**

    CPGC Global Test Start

    @param host       - Pointer to sysHost, the system host (root) structure
    @param socket     - socket number
    @param ddrChEnMap - bit mask of channels to use

    @retval SUCCESS

**/
UINT32
CpgcGlobalTestStart (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap                        // 4-bit bit mask of ddr channels to test - (1 = active channel)
  );

/**

    CPGC Global Test Stop

    @param host       - Pointer to sysHost, the system host (root) structure
    @param socket     - socket number
    @param ddrChEnMap - bit mask of channels to use

    @retval SUCCESS

**/
UINT32
CpgcGlobalTestStop (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap                        // 4-bit bit mask of ddr channels to stop
  );

/**

  CPGC based Memory test

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Memory controller number
  @param ddrChEnMap          - channel mask
  @param flags               - Read or Write and data pattern flags

  @retval None
**/
void
CpgcMemTest (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap,                       // 4-bit bit mask of ddr channels to stop
  UINT32      flags                             // Read or Write and data pattern flags
  );

/**

  Polls for cpgc test done on ddr channels indicated with mask

  @param host                        - Pointer to sysHost, the system host (root) structure
  @param socket                      - socket to train
  @param ddrChEnMap                  - 4-bit bit mask of ddr channels to stop

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcPollGlobalTestDone (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap                        // 4-bit bit mask of ddr channels to stop
  );

/**

  Enable deselect patterns

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket to train
  @param ddrChEnMap  - DDR channels to setup
  @param mode        - MODE_DESELECT\n
                       MODE_VA_DESELECT

  @retval SUCCESS
  @retval FAILURE

**/
void
EnableCADBDeselect (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket to train
  UINT32      ddrChEnMap,                       // 4-bit bit mask of ddr channels to stop
  UINT16      mode                              // MODE_DESELECT
                                                // MODE_VA_DESELECT
  );

/**

   Set CPGC pattern

   @param host        - Pointer to sysHost, the system host (root) structure
   @param socket      - Socket Id
   @param ch          - Channel number (0-based)
   @param mux0        - Mux 0
   @param mux1        - Mux 1
   @param mux2        - Mux 2
   @param patbuf0     - Pattern 0
   @param patbuf1     - Pattern 1
   @param patbuf2     - Pattern 2

   @retval None
**/
VOID
SetCpgcPatControl (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       mux0,                             // Mux 0
  UINT8       mux1,                             // Mux 1
  UINT8       mux2,                             // Mux 2
  UINT32      patbuf0,                          // Pattern 0
  UINT32      patbuf1,                          // Pattern 1
  UINT32      patbuf2                           // Pattern 2
 );

/**

    Loop based on PatternLength

    @param PatternLength - Number of loops

    @retval Number of loops completed

**/
UINT8
PatternLengthToCpgcLoopCnt(
  UINT32 PatternLength                          // Number of loops
  );

/**

  Gets the Tx Eq settings list

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket number
  @param settingList - Settings List

  @retval number of Tx EQ settings

**/
UINT8
GetTxEqSettingsList(
    PSYSHOST    host,                               // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                             // CPU Socket Node number (Socket ID)
    INT16 settingList[MAX_ODT_SETTINGS][MAX_PARAMS] // Settings List
    );

/**

  This gets the TCO COMP settings list

  @param host    - Pointer to sysHost
  @param socket    - Socket number
  @param settingLIst - pointer to list of settings

  @retval numTcoCompSettings - number of TCO COMP settings

**/
UINT8
GetTcoCompSettingsList (
    PSYSHOST host,
    UINT8    socket,
    INT16    settingList[MAX_SETTINGS][MAX_PARAMS]
    );
/**

  Gets  CTLE settings list

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param settingList - Settings List

  @retval number of CTLE settings

**/
UINT8
GetCtleSettingsList(
    PSYSHOST    host,                               // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                             // CPU Socket Node number (Socket ID)
    INT16 settingList[MAX_ODT_SETTINGS][MAX_PARAMS] // Settings List
    );

/**

  This gets MC_RON settings list

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param settingList - Settings List

  @retval number of MC_RON settings

**/
UINT8
GetMcRonSettingsList(
    PSYSHOST    host,                               // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                             // CPU Socket Node number (Socket ID)
    INT16 settingList[MAX_ODT_SETTINGS][MAX_PARAMS] // Settings List
    );

/**

  Chip specific call to Decrement IO Latency if necessary

  @param host           - Pointer to sysHost, the system host (root) structure
  @param socket         - Socket Id
  @param ch             - Channel number (0-based)
  @param dimm           - Current dimm
  @param rank           - Current rank
  @param centerPoint    - Center point

  @retval None
 */
void
DecIoLatency(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank
  UINT16      centerPoint[MAX_CH][MAX_STROBE]   // Center point
  );

/**

  Chip specific call to update the CtlCKLatency

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number
  @param rank    - Rank
  @param logRank - Logical rank

  @retval None

**/
void
StepChipCtlCKIOLatency(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT8       rank,                             // Rank
  UINT8       logRank                           // Logical rank
  );

/**

  Chip specific call to restore the CtlCKLatency

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number
  @param rank    - Rank
  @param logRank - Logical rank

  @retval None
**/
void
RestoreChipCtlCKIOLatency(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT8       rank,                             // Rank
  UINT8       logRank                           // Logical rank
  );

/**

  Chip specific call to perform the ODT MPR Dimm Activation Step

  @param host                           - Pointer to sysHost, the system host (root) structure
  @param socket                         - CPU Socket Node number (Socket ID)
  @param ch                             - Channel number (0-based)
  @param dimm                           - Current dimm
  @param executeCtlCLKTestChipStruct    - Pointer to CTL CLK structure

  @retval None
**/
void
PerformChipODTMPRDimmActivationStep(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                                               // Channel number (0-based)
  UINT8       dimm,                                             // Current dimm
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct // Pointer to CTL CLK structure
  );

/**


  @author calingle (9/23/2015)

  @param host              - Pointer to sysHost, the system host (root) structure
  @param odtActTableSize   - Size of the ODT Act Table
  @param odtActTablePtr    - Pointer to the ODT Act Table

  @retval None
**/
void
GetOdtActTablePointers (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT32      *odtActTableSize,                 // Size of the ODT Act Table
  struct odtActStruct **odtActTablePtr          // Pointer to the ODT Act Table
);

/**

  Look up ODT values

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number (0-based)

  @return Array of ODT values
**/
struct odtValueStruct *
LookupOdtValue (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Set ODT Matrix for DDR3

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number
  @param rank    - Rank

  @retval None
**/
void
SetDdr3OdtMatrix (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT8       rank                              // Rank
  );

/**

  Write the Read and Write ODT Activates to the Matrix registers

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - Socket number
  @param ch               - Channel number (0-based)
  @param dimm             - DIMM number
  @param rdOdtTableData   - Read ODT table data
  @param wrOdtTableData   - Write ODT table data

  @retval None
 */
void
SetOdtActMatrixRegs (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // DIMM number
  UINT32      rdOdtTableData,                   // Read ODT table data
  UINT32      wrOdtTableData                    // Write ODT table data
  );

/**

  Construct config index based on the closest frequency index

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @return Config Data
**/
UINT16
SetOdtConfigInOdtValueIndex (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // CPU Socket Node number (Socket ID)
  );

/**

  Get ODT Timing

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket number
  @param ch            - Channel number (0-based)
  @param rdOdtTiming   - Pointer to Read ODT Timings
  @param wrOdtTiming   - Pointer to Write ODT Timings

  @retval None

**/
void
GetOdtTiming(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  ODT_TIMING  *odtTiming                        // Pointer to ODT Timings structure
  );

/**

  Program ODT Timing

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket number
  @param ch          - Channel number (0-based)

  @retval None

**/
void
SetOdtTiming (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Set Sense Amp ODT Delays

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket number

  @retval None

**/
void
SetSenseAmpODTDelays (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // CPU Socket Node number (Socket ID)
  );

/**

  Chip specific code path to getChipTimingData

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - processor socket ID
  @param ch                   - Processor socket Channel number
  @param WLChipCleanUpStruct - Pointer to CHIP_WL_CLEANUP_STRUCT

  @retval None

**/
void
GetChipTimingData(
  PSYSHOST                host,                 // Pointer to sysHost, the system host (root) structure
  UINT8                   socket,               // Socket Id
  UINT8                   ch,                   // Channel number (0-based)
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct   // Pointer to CHIP_WL_CLEANUP_STRUCT
  );

/**

  Chip specific code path to initialize "GlobalByteOff"

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - processor socket ID
  @param ch                   - Processor socket Channel number
  @param offsetValue          - Offset Value
  @param CRAddDelay           - Return value from UpdateGlobalOffsets()
  @param GlobalByteOff        - Return value from UpdateGlobalOffsets()

  @retval None
**/
void
SetChipDQDQSTiming(
  PSYSHOST                host,                 // Pointer to sysHost, the system host (root) structure
  UINT8                   socket,               // Socket Id
  UINT8                   ch,                   // Channel number (0-based)
  INT16                   offsetValue,          // Offset Value
  INT16*                  CRAddDelay,           // Return value from UpdateGlobalOffsets10nm()
  INT16*                  GlobalByteOff,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct   // Pointer to WL Cleanup structure
  );

/**

  setup to do before TxPiSample() main body. Grantley does not have any special setup to do

  @param dataControl  - Training mode data structure

  @retval None

**/
void
TxPiSampleChipInit(
  PCHIP_DDRIO_STRUCT dataControl                // Training mode data structure
  );

/**

  Chip specific setup to set Pi training mode

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - processor socket ID
  @param ch               - processor socket channel number
  @param dataControl      - Training mode data structure

  @retval None

**/
void
SetChipTxPiSampleTrainingMode(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  PCHIP_DDRIO_STRUCT dataControl                // Training mode data structure
  );

/**

  Chip specific setup for TxPiSample TXDQS Strobe training, part 1

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - processor socket ID
  @param ch          - Processor socket Channel number
  @param dimm        - Processor socket channel dimm number
  @param rank        - Processor socket channel dim rank number
  @param piDelay     - piDelay for nibble
  @param dataControl - training mode data

  @retval None

**/
void
SetChipTxPiSampleTrainingTXDQSStrobesI(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank
  INT16       piDelay,                          // piDelay for nibble
  PCHIP_DDRIO_STRUCT dataControl                // Training mode data structure
  );

/**

  Chip specific setup for TxPiSample TXDQS Strobe training, part 2

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - processor socket ID
  @param ch               - Processor socket Channel number
  @param dimm             - Processor socket channel dimm number
  @param dataControl      - Training mode data structure

  @retval None

**/
void
SetChipTxPiSampleTrainingTXDQSStrobesII(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  PCHIP_DDRIO_STRUCT dataControl                // Training mode data structure
  );

/**

  Setup PI settings parameters

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - CPU Socket Node number (Socket ID)
  @param step             - The PI step to work on
  @param piSetting        - The PI setting returned
  @param piStart          - Test starting PI
  @param NumberOfCycle    - Number of cycles
  @param trainRes         - Training results

  @retval None

**/
void
SetupPiSettingsParams(
  PSYSHOST  host,                                           // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                                         // CPU Socket Node number (Socket ID)
  UINT8     step,                                           // The PI step to work on
  UINT16    *piSetting,                                     // The PI setting returned
  UINT16    *piStart,                                       // Test starting PI
  UINT16    *NumberOfCycle,                                 // Number of cycles
  struct  TrainingResults  (**trainRes)[MAX_CH][MAX_STROBE] // Training results
  );

/**

  Checks if DQ swizzling is supported

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - CPU Socket Node number (Socket ID)

  @retval 0 Swizzle not supported
  @retval 1 Swizzle supported

**/
UINT8
CheckDqSwizzleSupport (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // CPU Socket Node number (Socket ID)
  );

/**

  Write control word data

  @param host             - Pointer to sysHost, the system host (root) structure
  @param socket           - CPU Socket Node number (Socket ID)
  @param ch               - Channel number (0-based)
  @param dimm             - Current dimm
  @param controlWordData  - Data to be written
  @param controlWordAddr  - Control word address (RDIMM_RC0A or RDIMM_RC0E)

  @retval None
**/
void
WriteRCFnv(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       controlWordData,                  // Data to be written
  UINT8       controlWordAddr                   // Control word address (RDIMM_RC0A or RDIMM_RC0E)
  );

/**

  Chip specific code path to support each platforms individual set of RTT_NOM values

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor socket ID
  @param ch      - Processor socket Channel number
  @param dimm    - Processor socket channel dimm number
  @param rank    - Processor socket channel dimm rank number

  @retval None

**/
void
SetChipRTTPark(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Current rank
  );

/**

  Chip specific code path to get the Wr DQDQS offset

  @param host - Pointer to sysHost, the system host (root) structure

  @return WR DQDQS
**/
UINT8
GetWrDqDqsOffset (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Chip specific code path. differences in the sequencer setup were moved to hooks

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor socket ID
  @param ch      - Processor socket Channel number
  @param dimm    - Processor socket channel dimm number
  @param rank    - Processor socket channel dimm rank number

  @retval None

**/
void
SetupChipPatternSequencer(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Current rank
  );

/**

  Update the Global Offsets

  @param host            - Pointer to sysHost, the system host (root) structure
  @param ch              - processor socket ID
  @param TargetOffset    - Target offset for byte lanes
  @param CRAddDelay      - Return value from UpdateGlobalOffsets()
  @param GlobalByteOff   - Return value from UpdateGlobalOffsets()

  @retval None

**/
void
UpdateGlobalOffsets (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       ch,                               // Channel number (0-based)
  INT16       TargetOffset,                     // Target offset for byte lanes
  INT16       *CRAddDelay,                      // Return value from UpdateGlobalOffsets()
  INT16       *GlobalByteOff                    // Return value from UpdateGlobalOffsets()
  );

/**

  Chip specific code path to perform WL Clean up

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor socket ID

  @retval None

**/
void
DoFinalChipSpecificWLCleanUp(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Initialize Receive Enable Parameters per channel

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval None

**/
VOID
ReceiveEnableInitPerCh (
    PSYSHOST    host,                           // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                         // Socket Id
    UINT8       ch,                             // Channel number (0-based)
    UINT8       dimm,                           // Current dimm
    UINT8       rank                            // Current rank
    );

/**

  Initialize Receive Enable Parameters the same for all ranks

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval None

**/
VOID
ReceiveEnableInitAllRanks (
  PSYSHOST host,                                // Pointer to sysHost, the system host (root) structure
  UINT8    socket                               // Socket Id
  );

/**

  Set the initial round trip value

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval None

**/
void
SetInitRoundTrip(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Current rank
  );

/*++

  Initialize channel for Recieve Enable training.\n
  This initialization is needed each time training is performed on a new rank.

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Socket number
  @param ch              - Channel number (0-based)
  @param dimm            - DIMM number
  @param rank            - Rank number

  @retval  N/A

--*/
VOID
RankRoundTrip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Current rank
  );

/**

  IO Latency Compensation starting point

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket number
  @param ch       - Channel number (0-based)

  @retval  N/A

**/
void
SetIoLatComp(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/**

  set IO Latency Compensation for normal operation

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket Id
  @param ch        - Channel number (0-based)
  @param rtError   - Error status on current channel
  @param tcrapOrg  - tCRAP data to set

  @retval None

**/
void
SetIOLatencyComp(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       rtError,                          // Error status on current channel
  UINT32      tcrapOrg[MAX_CH]                  // tCRAP data to set
  );

/**

  Set the Long Preamble

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param set     - Set or clear long preamble enable bit

  @retval None

**/
void
SetLongPreamble(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  BOOLEAN     set                               // Set or clear long preamble enable bit
  );

/**

  Setup Receive Enable fine parameter

  @param host         - Pointer to sysHost, the system host (root) structure
  @param CPGCAddress  - CPGC address array
  @param WDBPattern   - WDB Pattern
  @param loopCount    - Number of cycles
  @param numCl        - Number of cache lines

  @retval None

**/
void
SetupRecEnFineParam(
  PSYSHOST     host,                            // Pointer to sysHost, the system host (root) structure
  TCPGCAddress *CPGCAddress,                    // CPGC address array
  TWDBPattern  *WDBPattern,                     // WDB Pattern
  UINT8        *loopCount,                      // Number of cycles
  UINT8        *numCl                           // Number of cache lines
  );

/**

  Chip specific code path to enable Receive Enable mode

  @param host                          - Pointer to sysHost, the system host (root) structure
  @param socket                        - CPU Socket Node number (Socket ID)
  @param ch                            - Channel number (0-based)
  @param strobe                        - Strobe (0-based)
  @param executeCtlCLKTestChipStruct   - Pointer to CTL CLK structure

  @retval None

**/
void
EnableChipRecieveEnableMode(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                                               // Channel number (0-based)
  UINT8       strobe,                                           // Strobe (0-based)
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct // Pointer to CTL CLK structure
  );

/**

  Enable Recieve Enable Mode using settings from the Channel NVlist

  @param host                         - Pointer to sysHost, the system host (root) structure
  @param socket                       - CPU Socket Node number (Socket ID)
  @param ch                           - Channel number (0-based)
  @param strobe                       - Strobe (0-based)
  @param executeCtlCLKTestChipStruct  - Pointer to CTL CLK structure

  @retval None

**/
void
EnableChipRecieveEnableModeII(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                                               // Channel number (0-based)
  UINT8       strobe,                                           // Strobe (0-based)
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct // Pointer to CTL CLK structure
  );

/**

  Set the ActToAct Period

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - CPU Socket Node number (Socket ID)
  @param ch         - Channel number (0-based)
  @param tcrapOrg   - Array of per-channel tCRAP data

  @retval None

**/
void
SetActToActPeriod(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      tRRD_s_org[MAX_CH]                // Array of per-channel tRRD_s values
  );

/**



  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - CPU Socket Node number (Socket ID)
  @param ch       - Channel number (0-based)
  @param strobe   - Strobe (0-based)
  @param csrReg   - CSR Register
  @param piDelay  - Pi Delay

  @retval None

**/
void
RecEnTrainFeedback (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       strobe,                           // Strobe (0-based)
  UINT32      csrReg,                           // CSR Register
  UINT16      piDelay                           // Pi Delay
  );

/**

  Chip specific code path to disable Receive Enable Offset Training Mode

  @param host                         - Pointer to sysHost, the system host (root) structure
  @param socket                       - CPU Socket Node number (Socket ID)
  @param ch                           - Channel number (0-based)
  @param executeCtlCLKTestChipStruct  - Pointer to CTL CLK structure

  @retval None

**/
void
DisableChipChlRecEnOffsetTrainingMode(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // Socket Id
  UINT8       ch,                                               // Channel number (0-based)
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct // Pointer to CTL CLK structure
  );

/*++

  RCVEN is normally run twice - once in Early CTL/CLK and again after Early CMD/CLK.
  Return success if running RCVEN the second time.

  @param host            - Pointer to sysHost, the system host (root) structure

  @retval   0 - RCVEN has not been run
  @retval   1 - RCVEN has been run once already

--*/
UINT8
SkipReceiveEnable (
  PSYSHOST      host                            // Pointer to sysHost, the system host (root) structure
  );

/**

  Restore settings that were temporarily adjusted for Fine Receive Enable training

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)

  @retval None

**/
void
ReceiveEnableCleanup(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

void
InitRtlOffsetChip(
  UINT8     rtlOffset[2]
);

/*++

  Attempt Cold Boot Fast

  @param host   - Pointer to sysHost, the system host (root) structure

  @retval TRUE
  @retval FALSE

--*/
UINT32
AttemptColdBootFastChip (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

  Detect Boot Date old

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
DetectBootDateOldChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Detect IMC Enabled Change

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
DetectIMCEnabledChangeChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Detect MCA Errors

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - socket id
  @param logData - pointer to 32-bit lag data

  @retval TRUE
  @retval FALSE

--*/
UINT32
DetectMCAErrorsChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT32      *logData                          // Pointer to log data
  );

/*++

  Detect if the processor has been changed

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval TRUE @retval FALSE

--*/
UINT32
DetectProcessorRemovedChip (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

  Detect if the socket population has changed

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
DetectSocketPopulatedChangedChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Check if channel disabled in HW

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id
  @param ch     - ch id

  @retval TRUE
  @retval FALSE

--*/
UINT32
GetChnDisableChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/*++

  Check if 2 DPC is disabled in HW

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
GetDisable2dpcChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Check if 3 DPC is disabled in HW

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
GetDisable3dpcChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Check if QR DIMM is disabled in HW

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval TRUE
  @retval FALSE

**/
UINT32
GetDisableQRDimmChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Gets Processor PPINs

  @param host  - Pointer to sysHost, the system host (root) structure

  @retval (UINT64_STRUCT)procPpin

--*/
UINT64_STRUCT
GetPpinChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Initialize DDR Frequench

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval None

--*/
VOID
InitDdrFreqChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Initialize internal data structures

  @param host   - Pointer to sysHost, the system host (root) structure

  @retval TRUE
  @retval FALSE

--*/
UINT32
SavedSetupDataChangedChip (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

  Initialize internal data structures

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval TRUE
  @retval FALSE

--*/
UINT32
SavedSetupDataChangedIMCChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Set IMC Enabled

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval None

--*/
VOID
SetIMCEnabledChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/*++

  Set Setup Options

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval None

--*/
VOID
SetSetupOptionsChip (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/*++

  Set Steppings

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - socket id

  @retval None

--*/
VOID
SetSteppingsChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Calculate the final gap

  @param gap      - Pointer to the final gap value
  @param maxGap   - Maximum gap
  @param gapDimm  - Gap calculated based on current DIMM

  @retval None

**/
void
CalcFinalGap(
  UINT16      *gap,                             // Pointer to the final gap value
  UINT16      maxGap,                           // Maximum gap
  UINT16      gapDimm                           // Gap calculated based on current DIMM
  );

/**

  This function calculates the read to read turnaround time for different DIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRRDD channel value

**/
UINT8
CalctRRDD(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // CPU Socket Node number (Socket ID)
    UINT8     ch,                               // Channel number (0-based)
    TT_VARS   *ttVars                           // Pointer to the structure for timing variables
    );

/**

  This function calculates the read to read turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRRDR channel value

**/
UINT8
CalctRRDR(
    PSYSHOST  host,                             // Pointer to sysHost, the system host (root) structure
    UINT8     socket,                           // CPU Socket Node number (Socket ID)
    UINT8     ch,                               // Channel number (0-based)
    TT_VARS   *ttVars                           // Pointer to the structure for timing variables
    );

/**

  This function calculates the read to write turnaround time for different ranks on the different DIMM

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - CPU Socket Node number (Socket ID)
  @param ch     - Channel number (0-based)
  @param ttVars - Pointer to the structure for timing variables
  @param tRWDD  - Pointer to the return value for tRWDD

  @retval None

**/
void
DramSpecCalctRWDD(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tRWDD                            // Pointer to the return value for tRWDD
  );

/**

  This function calculates the read to write turnaround time for different ranks on the same DIMM

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - CPU Socket Node number (Socket ID)
  @param ch     - Channel number (0-based)
  @param ttVars - Pointer to the structure for timing variables
  @param tRWDR  - Pointer to the return value for tRWDR

  @retval None

**/
void
DramSpecCalctRWDR(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tRWDR                            // Pointer to the return value for tRWDR
  );

/**

  This function calculates the read to write turnaround time for same rank

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - CPU Socket Node number (Socket ID)
  @param ch     - Channel number (0-based)
  @param ttVars - Pointer to the structure for timing variables
  @param tRWSR  - Pointer to the return value for tRWSR

  @retval None

**/
void
DramSpecCalctRWSR(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tRWSR                            // Pointer to the return value for tRWSR
  );

/*++

  This function calculates the write to read turnaround time for different DIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param ttVars  - Pointer to the structure for timing variables
  @param tWRDD   - Pointer to the return value for tWRDD

  @retval None

  --*/
void
DramSpecCalctWRDD(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tWRDD                            // Pointer to the return value for tWRDD
  );

/**

  This function calculates the write to read turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param ttVars  - Pointer to the structure for timing variables
  @param tWRDR   - Pointer to the return value for tWRDR

  @retval None
**/
void
DramSpecCalctWRDR(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tWRDR                            // Pointer to the return value for tWRDR
  );

/**

  This function calculates the write to write turnaround time for different DIMMs

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - CPU Socket Node number (Socket ID)
  @param ch     - Channel number (0-based)
  @param ttVars - Pointer to the structure for timing variables
  @param tWWDD  - Pointer to the return value for tWWDD

  @retval None

**/
void
DramSpecCalctWWDD(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tWWDD                            // Pointer to the return value for tWWDD
  );

/**

  This function calculates the write to write turnaround time for different ranks on the same DIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param ttVars  - Pointer to the structure for timing variables
  @param tWWDR   - Pointer to the return value for tWWDR

  @retval None

**/
void
DramSpecCalctWWDR(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  TT_VARS     *ttVars,                          // Pointer to the structure for timing variables
  UINT8       *tWWDR                            // Pointer to the return value for tWWDR
  );

/**

  Programs the MR0 shadow register

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param MR0Data - MR0 Data to set

  @retval None
**/
void
SetMR0Shadow (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      MR0Data                           // MR0 Data to set
  );

/*++

  Programs the MR2 shadow register

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param MR2Data - MR2 Data to set

  @retval None

--*/
void
SetMR2Shadow (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT32      MR2Data                           // MR2 Data to set
  );

/**

  Program the MR4 shadow register

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param MR4Data - MR4 data to set

  @retval None

**/
void
SetMR4Shadow (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      MR4Data                           // MR4 data to set
  );

/**


  Program the MR5 shadow register

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param MR5Data - MR5 Data to set

  @retval None

**/
void
SetMR5Shadow (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      MR5Data                           // MR5 Data to set
  );

/**

  Check TxVREF support

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - CPU Socket Node number (Socket ID)
  @param ch         - Channel number (0-based)
  @param dimm       - Current dimm
  @param rank       - Current rank (0-based)
  @param strobe     - Strobe number (0-based)
  @param zeroValue  - Pointer that will return the current TxVref value

  @retval None

**/
void
CheckTxVrefSupport(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank (0-based)
  UINT8       strobe,                           // Strobe number (0-based)
  INT16       *zeroValue                        // Pointer that will return the current TxVref value
  );

/**

  Chip specific code path to initialize the effective bus delay

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - CPU Socket Node number (Socket ID)
  @param ch                  - Channel number (0-based)
  @param CwlAdj              - Pointer to CwlAdj
  @param IOComp              - Pointer to IOComp
  @param IntDelta            - Pointer to IntDelta
  @param longPreambleEnable  - Pointer to the long preamble enable

  @retval None

**/
void
EffectiveBusDelayInit(
  PSYSHOST    host,                                   // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                 // CPU Socket Node number (Socket ID)
  UINT8       ch,                                     // Channel number (0-based)
  INT16       *CwlAdj,                                // Pointer to CwlAdj
  UINT32      *IOComp,                                // Pointer to IOComp
  UINT16      *IntDelta,                              // Pointer to IntDelta
  UINT32      *longPreambleEnable                     // Pointer to the long preamble enable
  );

/**

  Program the Fast Boot timings

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Current rank (0-based)
  @param strobe  - Strobe number (0-based)

  @retval None

**/
void
WriteFastBootTimings(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Current rank (0-based)
  UINT8       strobe                            // Strobe number (0-based)
  );

/**

  Get the results of test execution

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param strobe  - Strobe number (0-based)

  @retval  Feedback data

**/
UINT32
GetDataTrainFeedback (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       strobe                            // Strobe number (0-based)
  );

/**

  This function applies RfOn setting to given channel.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param rfOn    - rfOn setting to apply

@retval previous value

**/
UINT32
SetRfOn (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      rfOn                              // rfOn setting to apply
  );

/**

  This function applies the given CKE override

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param ckeMask - CKE settings to apply

@retval None

**/
void
SetCkeOverride (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       ckeMask                           // CKE settings to apply
  );

/**

  Chip specific code path to assert ODT Override

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - CPU Socket Node number (Socket ID)
  @param ch              - Channel number (0-based)
  @param wrOdtTableData  - ODT Signals

  @retval None

**/
void
AssertChipODTOveride(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT32      wrOdtTableData                    // ODT Signals
  );

/**

  Chip specific code path to deassert ODT Override

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - CPU Socket Node number (Socket ID)
  @param ch              - Channel number (0-based)

  @retval None

**/
void
DeAssertChipODTOveride(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Exectues command/clock training

  @param host      - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
ChipLateCmdClk(
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Aggressive CMD test to get a rough centering of CMD

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval SUCCESS

**/
UINT32
ChipAggressiveCMDTest(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Set 3N timing and save original setting

  @param host                            - Pointer to sysHost, the system host (root) structure
  @param socket                          - Socket Id
  @param ch                              - Channel number (0-based)
  @param chipEnableParityCheckingStruct  - Pointer to parity checking

  @retval None

**/
void
ChipSet3NTimingandSaveOrigSetting(
  PSYSHOST    host,                                           // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                         // Socket Id
  UINT8       ch,                                             // Channel number (0-based)
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct // Pointer to parity checking
  );

/**

  Restore original CMD timing

  @param host                           - Pointer to sysHost, the system host (root) structure
  @param socket                         - Socket Id
  @param ch                             - Channel number (0-based)
  @param chipEnableParityCheckingStruct - Pointer to parity checking

  @retval None

**/
void
ChipRestoreOrigCMDtiming(
  PSYSHOST    host,                                            // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                          // Socket Id
  UINT8       ch,                                              // Channel number (0-based)
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct  // Pointer to parity checking
  );

/**

  Enable FNV

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - Socket Id
  @param ch                   - Channel number (0-based)
  @param chipErlyCmdCKStruct  - Pointer to Early CMD CK structure

  @retval None

**/
void
EnableChipFNV(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct  // Pointer to Early CMD CK structure
  );

void
Toggle3DSMode(
  PSYSHOST host,                             // Pointer to the system host (root) structure
  UINT8    socket,                           // Socket Id
  UINT8    ch,                               // Channel number (0-based)
  UINT8    value                             // value to be programmed
  );

/**

  Start Early CMD/CLK tests

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param chBitmask  - Channels to execute test on

  @retval None

**/
void
EarlyCmdSignalTest(
  PSYSHOST    host ,                            // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT32      chBitmask                         // Channels to execute test on
  );

/**

  Stop Early CMD/CLK tests

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket Id
  @param chBitmask  - Channels to execute test on
  @param dimm       - Current dimm
  @param bwSerr     - Bitwise error array

  @retval ChipCollectParityResults

**/
UINT32
EarlyCmdSignalStop(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT32      chBitmask,                        // Channels to execute test on
  UINT8       dimm,                             // Current dimm
  UINT32      bwSerr[MAX_CH][3]                 // Bitwise error array
  );

/**

  Disable FNV

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - Socket Id
  @param ch                   - Channel number (0-based)
  @param chipErlyCmdCKStruct  - Pointer to Early CMD CK structure

  @retval None

**/
void
DisableChipFNV(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct  // Pointer to Early CMD CK structure
  );

/**

  Chip specific code path to disable LRDIMM MPR Override Mode

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval None

**/
void
DisableChipLRDIMMMPROverrideMode(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Chip specific code path to enable LRDIMM Preamble TR Mode

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void
EnableChipLRDIMMPreambleTrMode(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Chip specific code path to disable LRDIMM Preamble TR Mode

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void
DisableChipLRDIMMPreambleTrMode(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Chip specific code path. Platforms supporting DDR3 require seperate code path for DDR4 that to restore RTT_PARK. DDR3 does not need to do this.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Rank number (0-based)

  @retval None

**/
void
DoChipCompatibleRTT_PARKRestore(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Rank number (0-based)
  );

/**

  Chip specific code path to determine if encoded CS is used

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval TRUE for Encoded CS
  @retval FALSE
**/
BOOLEAN
DoesChipDetectCSorLRRnkDimm(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Chip specific code path to get the write leveling cleanup offset

  @param host - Pointer to sysHost, the system host (root) structure

  @retval (UINT8) Offset

**/
UINT8
GetChipWriteLevelingCleanUpOffset(
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Gets the type of parameter

  @param host  - Pointer to sysHost, the system host (root) structure
  @param Param - Parameter input to convert to parameter type

  @retval (UINT8) Parameter type

**/
UINT8
GetChipParamType (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       Param                             // Parameter input to convert to parameter type
  );

/**

  Chip specific setup to initialize loop indices when performing a pidelay step

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - CPU Socket Node number (Socket ID)
  @param piDelayStart  - Pointer to the loop start index
  @param piDelayEnd    - Pointer to the loop end index

  @retval None

**/
void
GetChipTiPiSamplepiDelayStartandEnd(
  PSYSHOST  host,                               // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                             // CPU Socket Node number (Socket ID)
  INT16     *piDelayStart,                      // Pointer to the loop start index
  UINT16    *piDelayEnd                         // Pointer to the loop end index
  );

/**

  Initialize the requested structure

  @param chipGetCmdMgnSwpStruct  - Pointer to struct to initialize

  @retval None

**/
void
GetCmdMarginsSweepInit(
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct // Pointer to struct to initialize
  );

/**

  Update the CMD Margin Masks

  @param host                   - Pointer to sysHost, the system host (root) structure
  @param socket                 - Socket Id
  @param ch                     - Channel number (0-based)
  @param piIndex                - PI index
  @param offset                 - Max PI index
  @param errorResult            - Error results
  @param chipGetCmdMgnSwpStruct - Pointer to structure

  @retval None
**/
void
UpdateCmdMarginMasks(
  PSYSHOST    host,                                   // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                 // Socket Id
  UINT8       ch,                                     // Channel number (0-based)
  INT16       piIndex,                                // PI index
  INT16       offset,                                 // Max PI index
  UINT8 errorResult[MAX_CH][PAR_TEST_RANGE],          // Error results
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct // Pointer to structure
  );

/**

  Chip specific code path to Disable any legacy LRDRIM WriteLeveling and return false, or return true if should disable DDR4 write leveling.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Current rank

  @retval TRUE if the chip should disable DDR4 write leveling
  @retval FALSE if the chip should not disable DDR4 write leveling

**/
BOOLEAN
ShouldChipDisableDDR4LRDIMMWriteLeveling(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Current rank
  );

/**

  Chip specific code path to determine if DDR4 LRDIM Host side training should be enabled. Disables legacy host side training if it has been enabled

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Current rank

  @retval TRUE if the chip should disable DDR4 LRDIM Host side training
  @retval FALSE if the chip should not disable DDR4 LRDIM Host side training

**/
BOOLEAN
ShouldChipEnableDDR4LRDIMMHostSideTraining(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank                              // Rank number (0-based)
  );

/**

  Chip specific code path to determine if Early CTL CLK should be executed

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE Early CTL CLK should be executed
  @retval FALSE Early CTL CLK should not be executed

**/
BOOLEAN
ShouldChipPerformEarlyCtlClk(
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Chip specific code path to determine if chip should set normal CMD timing

  @param host                         - Pointer to sysHost, the system host (root) structure
  @param socket                       - Socket Id
  @param ch                           - Channel number (0-based)
  @param chipSetNormalCMDTimingStruct - Normal Command Timing structure

  @retval TRUE chip should set normal CMD timing
  @retval FALSE chip should not set normal CMD timing

**/
BOOLEAN
ShouldChipSetNormalCMDTiming(
  PSYSHOST    host,                                                // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                              // Socket Id
  UINT8       ch,                                                  // Channel number (0-based)
  PCHIP_SET_NORMAL_CMD_TIMING_STRUCT chipSetNormalCMDTimingStruct  // Normal Command Timing structure
  );

/**

  Chip specific code path to determine if Early CMD Training should be run or not based on Parity Support

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm

  @retval TRUE Early CMD Training should be run
  @retval FALSE Early CMD Training should not be run

**/
BOOLEAN
ShouldChipSkipElyCMDTrAsPrtyNotSprtd(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Set a termination rank for each DIMM

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id
  @param ch     - Channel number (0-based)

  @retval None

**/
void
SetRankTerminationMask(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Chip specific code path to write Chip Wr Addr delays

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket Id
  @param ch                  - Channel number (0-based)
  @param offsetValue         - Offset for byte lanes
  @param CRAddDelay          - Add delays
  @param WLChipCleanUpStruct - Pointer to CHIP_WL_CLEANUP_STRUCT
  @retval None

**/
void
WriteChipWrADDDelays(
  PSYSHOST                host,                             // Pointer to sysHost, the system host (root) structure
  UINT8                   socket,                           // Socket Id
  UINT8                   ch,                               // Channel number (0-based)
  INT16                   offsetValue,                      // Offset for byte lanes
  INT16                   *CRAddDelay,                      // Add delays
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  );

/**

  Checks if the requested Vdd is supported

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param vdd     - Vdd (1.5v or 1.35V)

  @retval Supported DDR frequency

**/
UINT8
CheckPORDDRVdd (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       vdd                               // Vdd (1.5v or 1.35V)
  );

/**

  Returns the DDR frequency supported by this config

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param vdd     - Vdd (1.5v or 1.35V)

  @retval Supported DDR frequency

**/
UINT8
GetPORDDRFreq (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       vdd                               // Vdd (1.5v or 1.35V)
  );

/**

  Check the DIMM Type

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS - DDR4
  @retval FAILURE - Anything except DDR4

**/
UINT32
CheckDimmType (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

 Determine if we train a given rank for a given group

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket
  @param ch      - Channel
  @param dimm    - Dimm
  @param rank    - Rank
  @param level   - DdrLevel or LrbufLevel
  @param group   - Tx/RxVref, Tx/RxDq/Dqs, etc

  @retval 1 - skip
  @retval 0 - train

**/
UINT8  CheckSkipRankTrain(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, GSM_LT level, GSM_GT group);

/**

  Chip specific code path to check if the channel is done

  @param host                    - Pointer to sysHost, the system host (root) structure
  @param chipGetCmdMgnSwpStruct  - Mask data to check

  @retval 0 the channel is done
  @retval 1 the channel is not done

**/
UINT8
CheckChDoneMask(
  PSYSHOST    host,                                   // Pointer to sysHost, the system host (root) structure
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct // Mask data to check
  );

/**

  Chip specific code to determine if parity is supported for the DIMM

  @param host - Pointer to sysHost, the system host (root) structure

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
DoesChipSupportParityChecking(
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Chip specific code path to enable parity

  @param host                           - Pointer to sysHost, the system host (root) structure
  @param socket                         - CPU Socket Node number (Socket ID)
  @param ch                             - Channel number (0-based)
  @param chipEnableParityCheckingStruct - Pointer to Parity struct

  @retval None

**/
void
ChipEnableParity(
  PSYSHOST    host,                                           // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                         // CPU Socket Node number (Socket ID)
  UINT8       ch,                                             // Channel number (0-based)
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct // Pointer to Parity struct
);

/**

  Chip specific code path to enable check parity mode

  @param host - Pointer to sysHost, the system host (root) structure
  @param mode - mode is either MODE_VA_DESELECT or MODE_DESELECT

  @retval None

**/
void
ChipEnableCheckParity(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT16      *mode                             // mode is either MODE_VA_DESELECT or MODE_DESELECT
  );

/**

  Chip specific path to workaround parity issues

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Socket Id
  @param ch       - Channel number (0-based)
  @param dimm     - Current dimm
  @param dateCode - Data code for parity test

  @retval 0 if no workaround was completed
  @retval 1 if any workarounds were completed

**/
UINT8
CheckParityWorkaroundChipSpecific(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT16      dateCode                          // Data code for parity test
  );

/**

  Collects the results of the previous parity based test

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - CPU Socket Node number (Socket ID)
  @param chToCheck - Channel number (0-based)
  @param dimm      - Current dimm
  @param bwSerr    - Bit wise status structure

  @retval 0 if no failures were detected
  @retval Non-zero if any failure was detected

**/
UINT32
ChipCollectParityResults (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      chToCheck,                        // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT32      bwSerr[MAX_CH][3]                 // Bit wise status structure
  );

/**

  Chip specific code path to determine if errors should be cleared if a parity error occurred

  @param host   - Pointer to sysHost, the system host (root) structure
  @param status - Status of existing parity error

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
ShouldChipClearCMDMGNParityError(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT32      status                            // Status of existing parity error
  );

/**

  Clears the results of the previous parity based test

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - CPU Socket Node number (Socket ID)
  @param chToCheck  - Channel number (0-based)
  @param dimm       - Current dimm

  @retval None
**/
void
ChipClearParityResults (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      chToCheck,                        // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Clears the results of the previous parity based test

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - CPU Socket Node number (Socket ID)
  @param chToCheck  - Channel number (0-based)
  @param rank       - Rank number (0-based)
  @param dimm       - Current dimm

  @retval None

**/
void
ChipClearParityResultsAlt (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT32      chToCheck,                        // Channel number (0-based)
  UINT8       rank,                             // Rank number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Chip specific code path to enable specific error conditions

  @param host                            - Pointer to sysHost, the system host (root) structure
  @param socket                          - Socket Id
  @param ch                              - Channel number (0-based)
  @param chipDisableParityCheckingStruct - Pointer to parity checking

  @retval None

**/
void
ChipSpecificErrorEnable(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // Socket Id
  UINT8       ch,                                               // Channel number (0-based)
  PCHIP_PARITY_CHECKING_STRUCT chipDisableParityCheckingStruct  // Pointer to parity checking
  );

/**

  Chip specific path to disable parity checking for all channels

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval None

**/
void
ChipSpecificParityCheckDisable(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                                            // Socket Id
  );

/**

  Chip specific path to disable parity checking per channel

  @param host                            - Pointer to sysHost, the system host (root) structure
  @param socket                          - Socket Id
  @param ch                              - Channel number (0-based)
  @param chipDisableParityCheckingStruct - Pointer to parity checking

  @retval None

**/
void
ChipSpecificParityCheckDisablePerCh(
  PSYSHOST    host,                                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                                           // Socket Id
  UINT8       ch,                                               // Channel number (0-based)
  PCHIP_PARITY_CHECKING_STRUCT chipDisableParityCheckingStruct  // Pointer to parity checking
  );

/**

  Chip specific code path to clear DIMM Error Results

  @param ch                      - Channel number (0-based)
  @param errorResult             - Error results
  @param chBitmask               - Bit mask of channels
  @param chipGetCmdMgnSwpStruct  - Channel data

  @retval None

**/
void
ChipClearDimmErrRslts(
  UINT8       ch,                                             // Channel number (0-based)
  UINT8       errorResult[MAX_CH][CHIP_CMD_CLK_TEST_RANGE],   // Error results
  UINT32      *chBitmask,                                     // Bit mask of channels
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct         // Channel data
  );

/**

  Chip specific code path to get the feedback from the correct register

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param strobe  - Strobe (0-based)

  @retval (UINT32)feedback

**/
UINT32
ReadTrainFeedback (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       strobe                            // Strobe (0-based)
  );

/**

  Chip specific code path to get the correct index into the training results

  @param step      - Pointer to sysHost, the system host (root) structure
  @param piSetting - CPU Socket Node number (Socket ID)
  @param piStart   - Channel number (0-based)
                     Strobe (0-based)
  @retval (UINT8)index value

**/
UINT8
GetTrainingResultIndex(
  UINT8       step,                             // Current training step
  UINT16      piSetting,                        // Pi Setting
  UINT16      piStart                           // Pi Start
  );

/**

  Chip specific code path to get the error results

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)

  @retval (UINT32) Error results

**/
UINT32
GetErrorResults (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch                                // Channel number (0-based)
  );

/**

  Chip specific code path to collect and store the results from the current test

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param dimm    - Current dimm
  @param rank    - Rank number (0-based)
  @param piDelay - Pi Delay

  @retval None

**/
void
GetResultsWL (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT16      piDelay                           // Pi Delay
  );

/**

  Chip specific code path to read training feedback for LRDIMMs

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param ch      - Channel number (0-based)
  @param strobe  - Strobe (0-based)
  @param bit     - Bit

  @retval (UINT32) feedback

**/
UINT32
LrDimmReadTrainFeedback (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       strobe,                           // Strobe (0-based)
  UINT8       bit                               // Bit
  );

/**

  Chip specific code path to execute step for lower nibble

  @param host           - Pointer to sysHost, the system host (root) structure
  @param socket         - CPU Socket Node number (Socket ID)
  @param ch             - Channel number (0-based)
  @param strobe         - Strobe (0-based)
  @param csrReg         - CSR Register
  @param strobeFeedback - Pointer to or in the lower nibble

  @retval None

**/
void
SetChipGetCtlClkResLowerNibbleStrobeFeedback(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       ch,                               // Channel number (0-based)
  UINT8       strobe,                           // Strobe (0-based)
  UINT32      *csrReg,                          // CSR Register
  UINT32      *strobeFeedback                   // Pointer to or in the lower nibble
  );

/**

  Chip specific code path to execute step for upper nibble

  @param strobe          - Strobe (0-based)
  @param csrReg          - CSR Register
  @param strobeFeedback  - Pointer to or in the upper nibble

  @retval None

**/
void
SetChipGetCtlClkResUpperNibbleStrobeFeedback(
  UINT8       strobe,                           // Strobe (0-based)
  UINT32      csrReg,                           // CSR Register
  UINT32      *strobeFeedback                   // Pointer to or in the upper nibble
  );

/**

  Chip specific code path to get the data stored in the requested sticky scratchpad

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket Id
  @param scratchpad  - Scratch pad to read

  @retval (UINT32) Scratchpad data

**/
UINT32
GetStickyScratchpad (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       scratchpad                        // Scratch pad to read
  );

/**

  Chip specific code path to set the data into the requested sticky scratchpad

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Socket Id
  @param scratchpad  - Scratch pad to read
  @param data        - Data to be written

  @retval None

**/
void
SetStickyScratchpad (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       scratchpad,                       // Scratch pad to read
  UINT32      data                              // Data to be written
 );

/**

    Checks if the processor is BDX EP

    @param host - Pointer to the system host (root) structure

    @retval TRUE if BDX EP SKU is detected
    @retval FALSE if not BDX EP SKU detected.

**/
UINT8
IsBDXEP (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

    Checks if the processor is DE

    @param host - Pointer to the system host (root) structure

    @retval TRUE if DE SKU is detected
    @retval FALSE if not DE SKU detected.

**/
UINT8
IsDESku (
  PSYSHOST    host                              // Pointer to sysHost, the system host (root) structure
  );

/**

  Chip specific code path to disable 2 cycle bypass

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - CPU Socket Node number (Socket ID)
  @param data    - Data for disable 2cyc bypass

  @retval None

**/
void
SetDisable2cycBypass (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // CPU Socket Node number (Socket ID)
  UINT8       data                              // Data for disable 2cyc bypass
  );

/**

  Chip specific code path to detect if current DIMM is LRDIMM

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval 1 LRDIMM is present
  @retval 0 LRDIMM is not present

**/
UINT8
IsLrdimmPresent (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm                              // Current dimm
  );

/**

  Updates provided min and max values based on an input

  @param val     - New value to evaluate
  @param minVal  - Current minimum value
  @param maxVal  - Current maximum value

  @retval None

**/
void
UpdateMinMax(
  UINT16      val,                              // New value to evaluate
  UINT16      *minVal,                          // Current minimum value
  UINT16      *maxVal                           // Current maximum value
  );

/**

  Set the configuration before RMT

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id

  @retval None

**/
VOID
SetConfigBeforeRmt(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**

  Chip specific code path to set the MPR training mode

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param enable  - Enable or disable iMC to issue activates and precharges

  @retval None

**/
VOID
SetMprTrainMode (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       enable                            // Enable or disable iMC to issue activates and precharges
  );

/**

  Chip specific code path to set the VDD

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket Id

  @retval None

**/
void
SetVdd (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // Socket Id
  );

/**
  Chip specific code path to get the starting value requested


  @param step     - Current PI step
  @param piStart  - PI start value

  @retval (UINT16)Starting value

**/
UINT16
StartBackOver (
  UINT8       step,                             // Current PI step
  UINT16      piStart                           // PI start value
  );

/**

  Execute a platform reset

  @param host       - Pointer to sysHost, the system host (root) structure
  @param resetType  - Reset type

  @retval None

**/
void
ResetSystemEx (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       resetType                         // Reset type
  );

UINT8
GetCmdMarginsSweepErrorResFeedback(
    GSM_CSN   signal,
    UINT8     dimm,
    INT16     piIndex,
    UINT8     CurError
    );

/**

  Chip specific code path to deassert CKE

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param ckeMask - CKE override mask

  @retval None

**/
void
ChipDeassertCKE(
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       ckeMask                           // CKE override mask
);

/**

  Apply settings before AdvancedMemTest starts

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval (UINT32) scrambleConfigOrg

**/
UINT32
PreAdvancedMemTest (
  PSYSHOST    host,                            // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                          // Socket Id
  UINT8       ch                               // Channel number (0-based)
  );

/**

  Apply settings after AdvancedMemTest ends

  @param host               - Pointer to sysHost, the system host (root) structure
  @param socket             - Socket Id
  @param ch                 - Channel number (0-based)
  @param scrambleConfigOrg  - DDR scrambler config returned by PreAdvancedMemTest

  @retval None

**/
VOID
PostAdvancedMemTest (
  PSYSHOST    host,                            // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                          // Socket Id
  UINT8       ch,                              // Channel number (0-based)
  UINT32      scrambleConfigOrg                // DDR scrambler config returned by PreAdvancedMemTest
  );

/**

  Restore Scrambler after AdvancedMemTest ends

  @param host               - Pointer to sysHost, the system host (root) structure
  @param socket             - Socket Id
  @param ch                 - Channel number (0-based)
  @param scrambleConfigOrg  - DDR scrambler config returned by PreAdvancedMemTest

  @retval None

**/
VOID
PostAdvancedMemTestRestoreScrambler (
  PSYSHOST    host,                            // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                          // Socket Id
  UINT8       ch,                              // Channel number (0-based)
  UINT32      scrambleConfigOrg                // DDR scrambler config returned by PreAdvancedMemTest
  );

/**

  Executes a step of Advanced Memory test using CPGC engine and logs results

  @param host          - Pointer to sysHost, the system host (root) structure
  @param socket        - Socket Id
  @param ddrChEnMap    - Bit-map of enabled channels
  @param testType      - Type of Advanced Memory Test
  @param mode          - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  @param seqDataInv    - Enables pattern inversion per subsequence
  @param patternQW     - 64-bit wide data pattern to use per UI
  @param uiShl         - Enables pattern rotation per UI
  @param numCacheLines - Number of cachelines to use in WDB
  @param patternDepth  - Number of UI to use in patternQW
  @param direction     - Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcMemTestMATS (
  PSYSHOST  host,                            // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                          // Socket Id
  UINT32    ddrChEnMap,                      // Bit-map of enabled channels
  UINT8     testType,                        // Type of Advanced Memory Test
  UINT8     mode,                            // Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  UINT8     seqDataInv[MT_MAX_SUBSEQ],       // Enables pattern inversion per subsequence
  UINT64_STRUCT patternQW[MT_PATTERN_DEPTH], // 64-bit wide data pattern to use per UI
  UINT8     uiShl[MT_PATTERN_DEPTH],         // Enables pattern rotation per UI
  UINT8     numCacheLines,                   // Number of cachelines to use in WDB
  UINT8     patternDepth,                    // Number of UI to use in patternQW
  UINT8     direction                        // Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN
  );

/**

  Get page table timer status

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval TRUE if enabled
  @retval FALSE if disabled

**/
BOOLEAN
IsChipPageTableTimerEnabled(
  PSYSHOST  host,                            // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                          // Socket Id
  UINT8     ch                               // Channel number (0-based)
  );

/**

  Enable/Disable page table timer

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param state   - PGT_TIMER_DISABLE / PGT_TIMER_ENABLE

  @retval None

**/
void
SetChipPageTableTimer(
  PSYSHOST  host,                            // Pointer to sysHost, the system host (root) structure
  UINT8     socket,                          // Socket Id
  UINT8     ch,                              // Channel number (0-based)
  UINT8     state                            // PGT_TIMER_DISABLE / PGT_TIMER_ENABLE
  );

/**

  Chip specific code path to enforce the WWDR settings

  @param host  - Pointer to sysHost, the system host (root) structure
  @param ch    - Channel number (0-based)
  @param tWWDR - Pointer to the resulting enforcement decision

  @retval None

**/
void
EnforceWWDR(
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    ch,                               // Channel number (0-based)
  UINT8    *tWWDR                            // Pointer to the resulting enforcement decision
  );

/**

  Chip specific code path to enforce the WWDD settings

  @param host  - Pointer to sysHost, the system host (root) structure
  @param ch    - Channel number (0-based)
  @param tWWDD - Pointer to the resulting enforcement decision

  @retval None

**/
void
EnforceWWDD(
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    ch,                               // Channel number (0-based)
  UINT8    *tWWDD                            // Pointer to the resulting enforcement decision
  );

/**

  Chip specific code path to compare PI results

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - CPU Socket Node number (Socket ID)
  @param ch           - Channel number (0-based)
  @param dimm         - Current dimm
  @param rank         - Current rank
  @param ErrorResult  - Pointer to error results
  @param rankPresent  - Rank present = 1 not present = 0

  @retval (UINT32) status

**/
UINT32
FindPiCompareResultChip(
  PSYSHOST     host,                         // Pointer to sysHost, the system host (root) structure
  UINT8        socket,                       // CPU Socket Node number (Socket ID)
  UINT8        ch,                           // Channel number (0-based)
  UINT8        dimm,                         // Current dimm
  UINT8        rank,                         // Current rank
  TErrorResult *ErrorResult,                 // Pointer to error results
  UINT8        rankPresent                   // Rank present = 1 not present = 0
  );

/**

  Determines if only a single rank or the default number of
  ranks should be trained.

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - CPU Socket Node number (Socket ID)
  @param numRanksDefault - The default number of ranks to train

  @retval (UINT8) Number of ranks to train

**/
UINT8
NumRanksToTrain (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8 socket,                              // CPU Socket Node number (Socket ID)
  UINT8 numRanksDefault                      // The default number of ranks to train
  );

/**

  Clears Prefetch Cache miss for NVMCTLR

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - CPU Socket Node number (Socket ID)

  @retval SUCCESS

**/
UINT32
ClrPcheMiss (
  PSYSHOST host,                             // Pointer to sysHost, the system host (root) structure
  UINT8    socket                            // CPU Socket Node number (Socket ID)
  );

void
CheckMCPPresent (
  PSYSHOST host,
  UINT8    socket
);

void
ChipOverrideFreq (
  PSYSHOST host,
  UINT8    socket,
  UINT8*   ddrFreq
);

UINT8
ChipLockstepEn (
  PSYSHOST host
);

UINT8
ChipMirrorEn (
  PSYSHOST host
);

/**

  Get or Set RxDelay for HBM

  @param host    - Pointer to the system host (root) structure
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm
  @param rank    - Current rank
  @param strobe  - Dqs data group within the rank
  @param offset  - Pointer to offset which needs to be updated

  @retval None

**/
void
AddChipRxDqsOffset(
  PSYSHOST host,         // Pointer to the system host (root) structure
  UINT8    socket,       // Socket Id
  UINT8    ch,           // Channel number (0-based)
  UINT8    dimm,         // Current dimm
  UINT8    rank,         // Current rank
  UINT8    strobe,       // current strobe
  UINT16   *offset       // offset to be updated
);


void
DisplayFnvInfo(
  PSYSHOST host
);

/**

    Disables the specified channel in HW

    @param host   - Pointer to sysHost, the system host (root) structure struct
    @param socket - Current socket
    @param ch     - Current channel

  @retval None

**/
VOID   SetChannelDisable (PSYSHOST host,UINT8 socket, UINT8 ch);

/**

  Enables/Disables Training Mode for FNV


  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param mode    - Training step

  @retval SUCCESS

**/
UINT32 SetAepTrainingMode (PSYSHOST host, UINT8 socket, UINT8 mode);

/**

  Enables/Disables Training Mode for FNV

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param mode    - Training step

  @retval SUCCESS

**/
UINT32 SetAepTrainingModeSMB (PSYSHOST host, UINT8 socket, UINT8 mode);

/**

  This resets the FNV DDRTIO FIFO Pointers

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param dimm    - Current dimm

  @retval None

**/
void   IODdrtReset(PSYSHOST host, UINT8 socket, UINT8 dimm);                                      // CORE / CHIP
void   IODdrtResetAll(PSYSHOST host, UINT8 socket);

/**

  Returns the memory controller ID

  @param mcId - Memory controller ID
  @param ch   - Channel

  @retval MC ID

**/
UINT8  GetMCCh(UINT8 mcId, UINT8 ch);                                     // CHIP

/**

  Set gnt2erid register

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm
  @param data   - Data to set

  @retval None

**/
void   SetSxpGnt2Erid (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 data);

/**

  Restores other timing parameters

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number

  @retval None

**/
void   RestoreTimingsChip (PSYSHOST host, UINT8 socket, UINT8 ch);                                  // CHIP

/**

  Enable/Disable DDRT Dq BCOM Ctrl

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - current socket
  @param value   - Enable/Disable

  @retval None

**/
void ToggleBcom(PSYSHOST host,UINT8 socket, UINT8 value);

void ToggleBcomFlopEn(PSYSHOST host, UINT8 socket, UINT8 value);

/**

  Displays the training results

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id

  @retval None

**/
void   DisplayCCCResults (PSYSHOST host, UINT8 socket);

/**

  This sets the CTL loopback mode in the buffer

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval None

**/
void ChipSetCTLLoopback(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  This gets the parity error status during CTL loopback training

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm

  @retval Alert Feedback Sticky Bit

**/
UINT8 ChipGetParityErrorStatus(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Get DIMM slot specific latency to be added to round trip

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)
  @param dimm    - Current dimm

  @retval Round Trip Latency Adjustment

**/
UINT8  GetChipAdjForRTL(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Compute contents for memory map CSRs to allocate physical address space to sockets,
  channels, and dimm ranks. Writes necessary memory map CSRs. Displays memory map
  configuration for debug.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 InitMemoryMap(PSYSHOST host);                                              // CALLTAB / CHIP

/**

  Enable RAS modes
  NOTE: Default settings are unchanged if feature not explicitly enabled by BIOS

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 SetRASConfig(struct sysHost *host);                                      // CALLTAB

void   GetPairedRank(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 *pairRankNumber,
                     UINT8 *pairChannel, UINT8 *pairDimm, UINT8 *pairRank);

/**

  Get the maximum number of data groups based on DIMM and group type

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket
  @param ch     - Channel
  @param dimm   - DIMM
  @param group  - Group type
  @param level  - Level type

  @retval Max strobe

**/
UINT8
GetMaxStrobe (
  PSYSHOST  host,       // Pointer to host structure (outside scope of this spec)
  UINT8     socket,     // Socket
  UINT8     ch,         // Channel
  UINT8     dimm,       // DIMM
  GSM_GT    group,      // Group type
  GSM_LT    level       // Level type
  );

void   CadbRcvenTrainingCleanup(PSYSHOST host, UINT8 socket);

/**

  This functions enables/disables InternalClocks On in datacontrol0

  @param host       - Pointer to sysHost, the system host (root) structure struct
  @param socket     - Socket Id
  @param chBitmask  - Bit mask of enabled channels
  @param dimm       - Current dimm
  @param value      - Enable or Disable

  @retval None

**/
void ToggleTxOnFnv(PSYSHOST host, UINT8 socket, UINT32 chBitmask, UINT8 dimm, UINT8 value);

/**

  This functions enables/disables InternalClocks On in datacontrol0

  @param host       - Pointer to sysHost, the system host (root) structure struct
  @param socket     - Socket Id
  @param chBitmask  - Bit mask of enabled channels
  @param dimm       - Current dimm
  @param value      - Enable or Disable

  @retval None

**/
void ToggleInternalClocksOnFnv(PSYSHOST host, UINT8 socket, UINT32 chBitmask, UINT8 dimm, UINT8 value);

/**

  Clear Errors before running test each time

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)

  @retval None

**/
void   ClearEridLfsrErr (PSYSHOST host, UINT8 socket, UINT8 ch);

/**

  Set gnt2erid register

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm
  @param data   - Data to set

  @retval None

**/
void   GetSetSxpGnt2Erid (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 mode, INT16 *data);
void   GetWpqRidtoRt (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Get the DDRT training results

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm

  @retval DDRT training results

**/
UINT8  GetDdrtTrainResult(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Collect ERID test results

  @param host               - Pointer to sysHost, the system host (root) structure struct
  @param socket             - Socket Id
  @param ch                 - Channel number (0-based)
  @param eridCombinedResult - Array of ERID results

  @retval None

**/
void   GetEridCombineResult (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 *eridCombinedResult);

/**

  Setup to perform Coarse ERID training

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm

  @retval None

**/
void   SetupCoerseEridTraining (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Setup CPGC WDB Buffer ERID

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval None

**/
void   SetupCpgcWdbBuffErid (PSYSHOST host, UINT8 socket, UINT8 ch);

/**

  Collect ERID test results

  @param host             - Pointer to sysHost, the system host (root) structure struct
  @param socket           - Socket Id
  @param ch               - Channel number (0-based)
  @param dimm             - Current dimm
  @param offset           - Offset to stored result
  @param index            - Index of the result to get
  @param eridOffset       - Offset into the ERID results
  @param gnt2eridPassed   - Status of the requested result. 1 = PASS, 0 = FAIL

  @retval None

**/
void   CollectTestResultsErid (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT16 offset, UINT8 index, UINT16 eridOffset, UINT8 *gnt2eridPassed);

/**

  Disable ERID LFSR training mode

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param ch      - Channel number (0-based)

  @retval None

**/
void   DisableEridLfsrMode (PSYSHOST host, UINT8 socket, UINT8 ch);

/**

  Disable ERID return

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id

  @retval None

**/
void   DisableEridReturn(PSYSHOST host, UINT8 socket);

/**

  RPQ Draining Sequence

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id
  @param ch     - Channel number (0-based)
  @param dimm   - Current dimm
  @param rank   - Rank number (0-based)

  @retval None

**/
void   RPQDrain(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank);

/**

  This programs nodata_trn_en

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param value   - Value to program nodata_trn_en

  @retval None

**/
void   ProgramNoDataTrnEn(PSYSHOST host, UINT8 socket, UINT8 value);

/**

  Gets the current value for CA VREF

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval CA VREF

**/
UINT8  CaVrefSelect (PSYSHOST host);
// MemWrLeveling.c helper functions

/**

  Chip specific code to adjust the risingEdge[ch][strobe].
  On Groveport only, simulation doesn't work with - 64.

  @param risingEdge - Current setting for risingEdge
  @param ch         - Channel
  @param strobe     - Strobe

  @retval None

**/
void   SetChipSimRisingEdge(UINT16 risingEdge[MAX_CH][MAX_STROBE], UINT8 ch, UINT8 strobe);
// MemCmdClk.c helper functions

/**

  Based on Mem Flows, Set Normal CMD Timings

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id

  @retval None

**/
void   ChipSimEnableNmlCMDTrng(PSYSHOST host, UINT8 socket);

/**

  Checks if Rx Vref tuning is required

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval TRUE - Enable RxVref tuning
  @retval FALSE - Do not enable RxVref tuning

**/
BOOLEAN ShouldChipEnableRxVrefTuning(PSYSHOST host);

/**

  RX Vref/SA offset Tuning

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param group   - Signal group

  @retval None

**/
VOID RxVrefTuning (PSYSHOST host, UINT8 socket, GSM_GT group);

/**

  Clean up any chipset bugs related to ReadDqDqsPerBit training

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket Id

  @retval None

**/
void   ReadDqDqsCleanup(PSYSHOST host, UINT8 socket);
void   ChipCtlClkFeedBack (PSYSHOST host, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 sign, INT16 *ctlMargins);
/**

  Initialize the imode setting list

  @param host        - Pointer to sysHost, the system host (root) structure struct
  @param socket      - Socket Id
  @param settingList - pointer to list of settings

  @retval number of Imode settings

**/
UINT8  GetImodeSettingsList(PSYSHOST host, UINT8 socket, INT16 settingList[MAX_ODT_SETTINGS][MAX_PARAMS]);

/**

  Check if value exceeds the bits

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Current socket
  @param ch      - Current channel
  @param tWR     - tWR
  @param DisCh   - Return value of 1 if channel should be disabled

  @retval None

**/
void   CheckTwrMargins (PSYSHOST  host, UINT8 socket, UINT8 ch, UINT8 tWR, UINT8 *DisCh);                     // CORE

/**

  Normalise lockstep timings

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Current socket

  @retval NoneisCmdVrefTrainingDone

**/
void   NormalizeLockstepTiming (PSYSHOST host, UINT8 socket);                                                 // CORE

/**

  Get the current CAS and CWL timings.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param CASTemp - Pointer to hold the CAS timing.
  @param CWLTemp - Pointer to hold the CWL timing.

  @retval None

**/
void GetDdrtTiming(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *CASTemp, UINT8 *CWLTemp);

/**

  Get the current delays for tCL and tCWL

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Socket number
  @param ch        - Channel number (0-based)
  @param dimm      - DIMM number (0-based)
  @param tCLAdd    - Pointer to hold the delay for tCL
  @param tCWLAdd   - Pointer to hold the delay for tCWL

  @retval None

**/
void GetDdrtTimingBufferDelay(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 *tCLAdd, UINT8 *tCWLAdd);

/**

  Determine if CMD VREF Training is complete

  @param host   - Pointer to sysHost, the system host (root) structure
  @param socket - Socket number

  @retval TRUE - CMD VREF training is complete
  @retval FALSE - CMD VREF training is not complete

**/
UINT8 isCmdVrefTrainingDone (PSYSHOST host, UINT8 socket);

/**

  Check if CTL loopback mode should be used

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval TRUE - CTL loopback mode should be used
  @retval TRUE - CTL loopback mode should not be used

**/
BOOLEAN CheckCTLLoopback(PSYSHOST host);

/**

  Initialize socket SMBUS controller

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Processor socket

  @retval N/A

**/
void
InitSmb (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket                            // CPU Socket Node number (Socket ID)
  );

void InitSmbSim(PSYSHOST host, UINT8 socket);                                            // CORE

/**

  Read SMBUS device at a given device and offset

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Processor socket ID
  @param dev         - Device address on SMBUS
  @param byteOffset  - Offset in Device
  @param data        - Pointer to store value\n
  NOTE: For a PCH block read, the array must be 33bytes in length,
  and the first data byte pointed to is the number of bytes read.
  This implementation only allows a maximum of 32bytes to be read.

  @retval 0 - Success
  @retval 1 - failure

**/
UINT32
ReadSmb (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Processor socket ID
  struct smbDevice  dev,                        // Device address on SMBUS
  UINT8       byteOffset,                       // Offset in Device
  UINT8       *data                             // Pointer to store value
                                                //   NOTE: For a PCH block read, the array must be 33bytes in length,
                                                //   and the first data byte pointed to is the number of bytes read.
                                                //   This implementation only allows a maximum of 32bytes to be read.
  );

/**

  Write SMBUS dev/offset with specified data

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Processor socket ID
  @param dev         - SMBUS device address
  @param byteOffset  - byte offset within device
  @param data        - pointer to data to write\n
  NOTE: For a PCH block write, the first data byte pointed to must be the number of bytes to write.
  This implementation only allows a maximum of 32bytes to be written.

  @retval 0 - success
  @retval 1 - failure

**/
UINT32
WriteSmb (
    PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
    UINT8       socket,                           // Processor socket ID
    struct smbDevice  dev,                        // Device address on SMBUS
    UINT8       byteOffset,                       // Offset in Device
    UINT8       *data                             // Pointer to store value
                                                  //   NOTE: For a PCH block read, the array must be 33bytes in length,
                                                  //   and the first data byte pointed to is the number of bytes read.
                                                  //   This implementation only allows a maximum of 32bytes to be read.
  );

/**

  Returns the SMBus strap address of the DIMM slot

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number
  @param dimm    - Dimm number
  @param dev     - Pointer to smbDevice

  @retval None

**/
void
GetSmbAddress (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket number
  UINT8       ch,                               // Channel number
  UINT8       dimm,                             // Dimm number
  struct smbDevice *dev                         // Pointer to smbDevice
  );

/**

  Find Index Resource Data

  @param strobe - Strobe number (0-based)

  @retval Index

**/
UINT8  FindIndexResData (UINT8 strobe);                                         // CORE

/**

  Checks to see if the TxDqDqs is too close to the zero encoding
  Adjusts TxDqDqs if this is the case for both directions

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - current socket

  @retval SUCCESS

**/
UINT32 WLPushOut(PSYSHOST  host, UINT8 socket);


/**

  Clears the bitwise error mask registers

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket to train

  @retval None

**/
void
ClearBwErrorMask (
                 PSYSHOST host,
                 UINT8 socket
                 );

/**

  Collect DDR advanced command CPGC error status

  @param host                    - Pointer to sysHost, the system host (root) structure
  @param socket                  - Memory controller number
  @param ddrChEnMap              - channel mask
  @param cpgcErrorStatus         - array of error status

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcAdvTrainingErrorStatusMATS (
  PSYSHOST host,
  UINT8  socket,
  UINT32 ddrChEnMap,
  CPGC_ERROR_STATUS_MATS cpgcErrorStatus[MAX_CH]
  );

/**

  Clean-up after PPR sequence is done

  @param host      - Pointer to sysHost, the system host (root) structure
  @param socket    - Memory controller to train
  @param chBitMask - Bit map of channels to mask off

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcPprCleanup (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitMask
  );

UINT32
GetTemporalPatternChip(
  PSYSHOST  host
);

/**

  DDR Mem test / Mem Init CPGC setup.

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - Memory controller to train
  @param ch          - DDR channel to setup
  @param dimm        - DIMM id
  @param rank        - Rank id
  @param subRank     - Sub rank ID
  @param dramMask    - Bitmap of dram that is target of PPR sequence
  @param bank        - Bank id
  @param row         - Row id
  @param pattern     - Flag to indicate mem init vs. mem test and read vs. write and data pattern

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcPprTestGlobalSetup (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     subRank,
  UINT32    dramMask,
  UINT8     bank,
  UINT32    row,
  UINT32    pattern
  );

  /**

  CPGC setup for PPR Sequence

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - DDR channel to setup
  @param dimm        - DIMM id
  @param rank        - Rank id
  @param subRank     - Sub rank ID
  @param dramMask    - Bitmap of dram that is target of PPR sequence
  @param bank        - Bank id
  @param row         - Row id

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
CpgcSetupPprPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     subRank,
  UINT32    dramMask,
  UINT8     bank,
  UINT32    row
  );

/**

  This function allows stacking of MRS commands in cadb buffer in writeCADBCmd function. Stacking commands in CADB
  is more efficient as the MRS are only ~32 clks apart instead of spread out by a bunch of CSR accesses

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Memory Controller
  @param ch      - Ch to send command to
  @param dimm    - DIMM to send command to
  @param Command - NORMAL_MRS  - How writeCADBcmd usually works
                   LOAD_MRS - writeCADBcmd will load commands into cadb buffer but not execute
                   EXECUTE_MRS - writeCADBcmd will execute the stack of commands

  @retval None

**/
void
MRSCmdStacking(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     Command
);

/**

  Loads the CADB

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket to train
  @param ch         - DDR channel to setup
  @param startLine  - Start line
  @param numLines   - Number of lines
  @param prog0      - Pattern 0
  @param prog1      - Pattern 1

  @retval None

**/
void
ProgramCADB (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         ch,
  UINT8         startLine,
  UINT8         numLines,
  PatCadbProg0  prog0[16],
  PatCadbProg1  prog1[16]
  );

/**

  Runs one or more CPGC tests

  @param host         - Pointer to sysHost, the system host (root) structure
  @param Node         - Socket Number
  @param ChbitMask    - Bit mask of enabled channels
  @param DQPat        - 0: BasicVA, 1: SegmentWDB, 2: CADB, 3: TurnAround, 4: LMNVa
  @param SeqLCs       - An array of one or more loopcounts
  @param ClearErrors  - TBD
  @param Mode         - mode allows for different types of modes for margining: \n
                              mode is {Bit0: PhLock (keep all bytes within in ch in phase), \n
                                      Bit1: Ch2Ch Data out of phase (LFSR seed), Bit 15:2: Reserved}

  @retval TRUE - Channel is done
  @retval FALSE - Channel is not done

**/
UINT8
RunIOTest(
  PSYSHOST host,
  UINT8  Node,
  UINT32 ChbitMask,
  UINT8  DQPat,
  UINT8  *SeqLCs,
  UINT8  ClearErrors,
  UINT16  Mode
  );

/**

  Programs CPGC to run on the selected physical ranks

  @param host              - PPointer to sysHost, the system host (root) structure
  @param socket            - Socket from which we are going to read the registers
  @param ch                - Channel
  @param RankBitMask       - Bit mask of ranks to work on
  @param RankFeatureEnable - RankFeatureEnable is a bit mask that can enable CKE, Refresh or ZQ
                             RankFeatureEnable[0] enables Refresh on all non-selected ranks
                             RankFeatureEnable[1] enables Refresh on all ranks
                             RankFeatureEnable[2] enables ZQ on all non-selected ranks
                             RankFeatureEnable[3] enables ZQ on all ranks
                             RankFeatureEnable[4] enables CKE on all non-selected ranks
                             RankFeatureEnable[5] enables CKE on all ranks
  @param SubRankBitMask    - Bit mask of logical ranks

  @retval 0
  @retval (1 << ch)

**/
UINT8
SelectCPGCRanks(
  PSYSHOST host,
  UINT8  socket,
  UINT8  ch,
  UINT8  RankBitMask,
  UINT8  RankFeatureEnable,
  UINT8  SubRankBitMask
  );

/**



  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id
  @param dimm    - Current dimm
  @param rank    - Rank number (0-based)
  @param mask    - Bits to be masked

  @retval None

**/
void   SetBwErrorMask(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank, struct bitMask *mask);   // CHIP

/**

  Programs all the key registers to define a CPCG test

  @param host         - Pointer to sysHost, the system host (root) structure
  @param socket       - Memory Controller
  @param ChbitMask    - ch Bit mak for which test should be setup for.
  @param CmdPat       - [0: PatWrRd (Standard Write/Read Loopback),
                               1: PatWr (Write Only),
                               2: PatRd (Read Only),
                               3: PatRdWrTA (ReadWrite Turnarounds),
                               4: PatWrRdTA (WriteRead Turnarounds),
                               5: PatODTTA (ODT Turnaround]
  @param NumCL        - Number of Cache lines
  @param LC           - Loop Count
  @param CPGCAddress  - Structure that stores start, stop and increment details for address
  @param SOE          - [0: Never Stop, 1: Stop on Any Lane, 2: Stop on All Byte, 3: Stop on All Lane]
  @param WDBPattern   - Structure that stores start, Stop, IncRate and Dqpat for pattern.
  @param EnCADB       - Enable test to write random deselect packages on bus to create xtalk/isi
  @param EnCKE        - Enable CKE power down by adding 64
  @param SubSeqWait   - # of Dclks to stall at the end of a sub-sequence

  @retval None

**/
void
SetupIOTest(
  PSYSHOST host,
  UINT8  socket,
  UINT32 ChbitMask,
  UINT8  CmdPat,
  UINT16 NumCL,
  UINT8  LC,
  TCPGCAddress *CPGCAddress,
  UINT8  SOE,
  TWDBPattern  *WDBPattern,
  UINT8  EnCADB,
  UINT8  EnCKE,
  UINT16 SubSeqWait
  );

/**

  Setup IO Test Basic VA

  @param host       - Pointer to sysHost, the system host (root) structure
  @param socket     - Socket from which we are going to read the registers
  @param ChbitMask  - ch Bit mask for which test should be setup for.
  @param LC         - Loop Count
  @param SOE        - [0: Never Stop, 1: Stop on Any Lane, 2:Stop on All Byte, 3: Stop on All Lane]
  @param EnCADB     - Enable test to write random deselect packages on bus to create xtalk/isi
  @param EnCKE      - Enable CKE power down by adding 64
  @param Spread     - Pattern Spread

  @retval None

**/
void
SetupIOTestBasicVA(
  PSYSHOST host,
  UINT8  socket,
  UINT32 ChbitMask,
  UINT8  LC,
  UINT8  SOE,
  UINT8  EnCADB,
  UINT8  EnCKE,
  UINT32 Spread
  );

/**

  Setup IO Test MPR

  @param host       - Pointer to sysHost, the system host (root) structure
  @param Node       - Socket from which we are going to read the registers
  @param ChbitMask  - ch Bit mask for which test should be setup for.
  @param LC         - Loop Count
  @param SOE        - [0: Never Stop, 1: Stop on Any Lane, 2:Stop on All Byte, 3: Stop on All Lane]
  @param EnCADB     - Enable test to write random deselect packages on bus to create xtalk/isi
  @param EnCKE      - Enable CKE power down by adding 64

  @retval None

**/
void
SetupIOTestMPR(
  PSYSHOST host,
  UINT8  Node,
  UINT32 ChbitMask,
  UINT8  LC,
  UINT8  SOE,
  UINT8  EnCADB,
  UINT8  EnCKE
  );

  /**

  Write to CADB

  @param host     - Pointer to sysHost, the system host (root) structure
  @param socket   - Memory Controller
  @param ch       - ch to send command to
  @param dimm     - CS to send the command to
  @param rank     - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param Command  - MRS command to be sent
  @param Bank     - Value to be sent
  @param Data     - Delay in Dclocks
  @param Delay    -

  @retval SUCCESS

**/
UINT32
WriteCADBCmd (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     Command,
  UINT8     Bank,
  UINT32    Data,
  UINT8     Delay
  );

/**

  This function writes to CADB

  @param host                - Pointer to sysHost, the system host (root) structure struct
  @param socket              - Socket Id
  @param ch                  - Channel number (0-based)
  @param dimm                - Current dimm
  @param rank                - Rank number (0-based)
  @param controlWordDataPtr  - Pointer to a 9 byte array of control word data
  @param controlWordFUnc     - Function index in the LR buffer
  @param controlWordAddr     - Control Word Address
  @param bufferAddr          - Address of the individual buffer to target...or 'FF' for all buffers

  @retval None

**/
void   WritePBA (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 *controlWordDataPtr,
              UINT8 controlWordFUnc, UINT8 controlWordAddr, UINT8 bufferAddr);                                            // CORE

/**

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 RASCheckDimmRanks(struct sysHost *host);                                 // CORE

VOID
MultiThreadS3ResumeScratchPadRestore (
  PSYSHOST              host,
  UINT8                 socket
  );

UINT32
GetDispatchPipeCsr (
  PSYSHOST                host,
  UINT8                   socket
  );

VOID
WriteDispatchPipeCsr (
    PSYSHOST              host,
    UINT8                 socket,
    UINT32                data
    );

UINT32
GetSetValFunctions(
  PSYSHOST                host,
  UINT8                   socket,
  UINT8                   mode,
  UINT32                  *valFuncs
  );

/**

  Abstract all knowledge of scratchpad registers from the core

  @param host       - Pointer to sysHost
  @param socket     - Socket number
  @param checkpoint - The major and minor codes combined in a UINT16 value

  @retval UINT32 - returns the value of the data that was written to the scratchpad register.

**/
UINT32
SetPostCode (
  PSYSHOST                host,
  UINT8                   socket,
  UINT16                  checkpoint,
  UINT16                  data
  );

/**

  Abstract all knowledge of scratchpad registers from the core

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval UINT32 - returns the scratchpad value for the breakpoint.

**/
UINT32
GetBreakpointCsr (
  PSYSHOST                host,
  UINT8                   socket
  );

void
SetBreakpointCsr(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    spReg
  );

/**

  Abstract all knowledge of scratchpad registers from the core

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval UINT32 - returns the scratchpad value for the SSA Agent Opcode.

**/
UINT32
GetSsaAgentData (
  PSYSHOST                host,
  UINT8                   socket
  );

/**

  Abstract all knowledge of scratchpad registers from the core

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval VOID

**/
VOID
ClearBpAndAgentData (
  PSYSHOST                host,
  UINT8                   socket
  );

VOID   GetDclkBclk (PSYSHOST host, UINT8 socket, UINT8 *DCLKRatio, UINT8 *bclk);                              // CORE

UINT8  GetPhyRank(UINT8 dimm, UINT8 rank);                                // CORE / CHIP

/**
  This function loads the WDB using PCI accesses

  @param host                 - Pointer to sysHost, the system host (root) structure
  @param socket               - Socket Id
  @param ch                   - Channel number (0-based)
  @param NumberOfWdbLines     - Number of cachelines to load
  @param wdbLines             - Pointer to data to write to the WDB
  @param StartCachelineIndex  - Start offset/cacheline number on the WDB.

  @retval None

**/
void
WdbPreLoadChip (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       NumberOfWdbLines,                 // Number of cachelines to load
  UINT8       *wdbLines,                        // Pointer to data to write to the WDB
  UINT8       StartCachelineIndex               // Start offset/cacheline number on the WDB.
  );

#ifdef SSA_FLAG
//
// Routines to access scratch pad register
//
UINT32
readScratchPad_CMD (
  PSYSHOST        host
 );

VOID
writeScratchPad_CMD (
  PSYSHOST        host,
  UINT32          val
);


UINT32
readScratchPad_DATA0 (
  PSYSHOST        host
  );

VOID
writeScratchPad_DATA0 (
  PSYSHOST         host,
  UINT32           val
  );

UINT32
readScratchPad_DATA1 (
  PSYSHOST         host
  );

VOID
writeScratchPad_DATA1 (
  PSYSHOST        host,
  UINT32          val
  );

VOID
writeScratchPad_currentCheckPoint (
  PSYSHOST        host,
  UINT32          val
  );
#endif // SSA_FLAG

#endif // _MEM_API_SKX_H_
