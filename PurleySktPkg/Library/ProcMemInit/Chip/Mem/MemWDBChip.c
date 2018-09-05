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
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
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
 *      Chip specific routine to load processor WDB
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "RcRegs.h"
#include "MemApiSkx.h"

/**

  This function loads the WDB using PCI accesses

  @param host                 - Pointer to sysHost
  @param socket               - Socket number
  @param ch                   - Channel number
  @param NumberOfWdbLines     - Number of cachelines to load
  @param WdbLines             - Pointer to data to write to the WDB
  @param StartCachelineIndex  - Start offset/cacheline number on the WDB. 

  @retval N/A

**/
void
WdbPreLoadChip (
           PSYSHOST  host,
           UINT8     socket,
           UINT8     ch,
           UINT8     NumberOfWdbLines,
           UINT8     *wdbLines,
           UINT8     StartCachelineIndex
           )
{
  UINT8                             cacheline;
  UINT8                             totalCachelineCount;
  UINT8                             byte;
  UINT8                             chunk;
  CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_STRUCT patWDBRdWrPntr;
  UINT64_STRUCT                         patWDBWr;

  patWDBRdWrPntr.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_REG);

  cacheline = StartCachelineIndex;
  totalCachelineCount = cacheline + NumberOfWdbLines;

  // Loop for each cacheline
  for (; cacheline < totalCachelineCount; cacheline++) {
    //MemDebugPrint ((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    //                "WDBPreLoadChip: StartCachelineIndex = %d, NumberOfWdbLines = %d, cacheline = %d\n", 
    //                StartCachelineIndex, NumberOfWdbLines, cacheline));

    if (cacheline >= MRC_WDB_LINES) break;

    // Point to the WDB cacheline entry
    patWDBRdWrPntr.Bits.rdwr_pntr = cacheline;

    // Loop for each data byte - 64bytes per CacheLine, 8bytes per chunk
    for (chunk = 0; chunk < 8; chunk++) {

      // Point to the WDB cacheline entry
      patWDBRdWrPntr.Bits.rdwr_subpntr = chunk;

      patWDBWr.lo = 0;
      patWDBWr.hi = 0;

      // lower 32 bits -> 0-3bytes
      for (byte = 0; byte < 4; byte++) {
        patWDBWr.lo |= *(wdbLines + (cacheline * MRC_WDB_LINE_SIZE) + (chunk * MRC_WDB_TRANSFERS_PER_LINE) + byte) << (byte * 8);
      } // byte loop
      // upper 32 bits -> 4-7bytes
      for (byte = 4; byte < MRC_WDB_BYTES_PER_TRANSFER; byte++) {
        patWDBWr.hi |= *(wdbLines + (cacheline * MRC_WDB_LINE_SIZE) + (chunk * MRC_WDB_TRANSFERS_PER_LINE) + byte) << ((byte - 4) * 8);
      } // byte loop

      //MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      //              "CL %d, chunk %d = 0x%x%x\n", cacheline, chunk, patWDBWr.hi, patWDBWr.hi));

      MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_REG, patWDBRdWrPntr.Data);

      MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR0_MCDDC_DP_REG, patWDBWr.lo);
      MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR1_MCDDC_DP_REG, patWDBWr.hi);

    } // chunk loop
  } // cacheline loop

} // WdbPreLoadChip
 