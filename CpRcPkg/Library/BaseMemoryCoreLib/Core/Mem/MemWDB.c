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
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"

/**

  This function fills the WDB buffer

  @param host              - Pointer to sysHost
  @param socket            - Socket number
  @param ch                - Channel number
  @param WdbLines          - Array that include data to write to the WDB
  @param NumberOfWdbLines  - The number of cachelines to write
  @param StartCachelineIndex - Start offset/cacheline number on the WDB.

  @retval N/A

**/
void
WDBFill (
        PSYSHOST host,
        UINT8    socket,
        UINT8    ch,
        TWdbLine *WdbLines,
        UINT8    NumberOfWdbLines,
        UINT8    StartCachelineIndex
        )
{
  struct ddrChannel                     (*channelList)[MAX_CH];

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WDBFill Starts\n"));
#endif

  channelList = &host->var.mem.socket[socket].channelList;

  // Make sure we do not try to write more than the WDB can hold
  if (NumberOfWdbLines > MRC_WDB_LINES) NumberOfWdbLines = MRC_WDB_LINES;

  // Compare cached values
  if (MemCompare((UINT8 *)&(*channelList)[ch].WdbLines[StartCachelineIndex], (UINT8 *)WdbLines, sizeof(TWdbLine) * NumberOfWdbLines)) {

    //
    // Fill the WDB
    //
    WdbPreLoadChip (host, socket, ch, NumberOfWdbLines, (void *) WdbLines, StartCachelineIndex);

    // Update cached values
    MemCopy((UINT8 *)&(*channelList)[ch].WdbLines[StartCachelineIndex], (UINT8 *)WdbLines, sizeof(TWdbLine) * NumberOfWdbLines);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WDBFill Ends\n"));
#endif
}


/**
  this function Write 1 cacheline worth of data to the WDB

  @param host  - Include all MRC global data.
  @param socket   - Memory Controller
  @param Patterns  - Array of bytes.  Each bytes represents 8 chunks of the cachelines for 1 lane
                     Each entry in Patterns represents a different cacheline for a different lane
  @param PMask     - Array of len Spread uint8.  Maps the patterns to the actual DQ lanes
                     DQ[0] = Patterns[PMask[0]], ... DQ[Spread-1] = Patterns[PMask[Spread-1]]
                     DQ[Spread] = DQ[0], ... DQ[2*Spread-1] = DQ[Spread-1]

  @retval None

**/
void
WriteWDBFixedPattern(
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT32    pattern,
                    UINT8     spread,
                    UINT8     cacheLine
                    )
{
  UINT8     line;
  UINT8     i;
  UINT32    *WdbLinePtr;
  //TWdbLine            WdbLines;
  TWdbLine  WdbLines[MRC_WDB_LINES];

  //
  // Fill the WDB buffer with the write content.
  //
  for (line = 0; line < cacheLine + spread; line++) {
    WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
    for (i = 0; i < (MRC_WDB_LINE_SIZE / sizeof (UINT32)); i++) {
      if (line < cacheLine) {
        WdbLinePtr[i] = 0;
      } else {
        WdbLinePtr[i] = pattern;
      }
    } // i loop
  } // line loop


  WDBFill (host, socket, ch, WdbLines, cacheLine + spread, 0);

} // WriteWDBFixedPattern

/**
  this function Write 1 cacheline worth of data to the WDB

  @param host  - Include all MRC global data.
  @param socket   - Memory Controller
  @param Patterns  - Array of bytes.  Each bytes represents 8 chunks of the cachelines for 1 lane
                     Each entry in Patterns represents a different cacheline for a different lane
  @param PMask     - Array of len Spread uint8.  Maps the patterns to the actual DQ lanes
                     DQ[0] = Patterns[PMask[0]], ... DQ[Spread-1] = Patterns[PMask[Spread-1]]
                     DQ[Spread] = DQ[0], ... DQ[2*Spread-1] = DQ[Spread-1]

  @retval None

**/
void
WriteTemporalPattern(
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT32    temporalPattern,
                    UINT8     numCachelines
                    )
{
  UINT8               ch;
  UINT8               bit;
  UINT8               line;
  UINT8               chunk;
  UINT8               maxChunk;
  UINT8               maxBits;
  UINT32              pattern = 0;
  UINT32              *WdbLinePtr;
  TWdbLine            WdbLines[32];
  struct channelNvram (*channelNvList)[MAX_CH];

  maxBits = numCachelines * 8;

  //
  // Fill the WDB buffer with the write content.
  //
  for (bit = 0; bit < maxBits; bit++) {

    if (temporalPattern & (1 << bit)) {
      pattern = 0xFFFFFFFF;
    } else {
      pattern = 0;
    }

    //
    // Get the cacheline this bit belongs to
    //
    line = bit / 8;

    //
    // Get the chunk in the current cacheline this bit belongs to
    //
    chunk = (bit * 2) % 16;
    maxChunk = chunk + 2;

    WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
    //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    //              "bit %d, line %d, chunk %d = 0x%x\n", bit, line, chunk, pattern));
    for (; chunk < maxChunk; chunk++) {
      if (chunk >= MRC_WDB_LINE_SIZE / 4) continue;
      WdbLinePtr[chunk] = pattern;
    } // i loop
  } // bit loop

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    WDBFill (host, socket, ch, WdbLines, numCachelines, 0);
  } // ch loop

} // WriteTemporalPattern

