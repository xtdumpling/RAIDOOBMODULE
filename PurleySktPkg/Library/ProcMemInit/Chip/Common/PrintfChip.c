/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
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
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysFuncChip.h"
#include "Mmrc.h"
#include "RcRegs.h"

#ifdef SERIAL_DBG_MSG

UINT8
checkMsgLevel (
              PSYSHOST  host,
              UINT32    level
              )
/*++

Routine Description:

  Checks the serial debug message level

  @param host  - Pointer to the system host (root) structure
  @param level - Debug message level to check

  @retval 1 - Message level is set
  @retval 0 - Message level is not set

--*/
{
  UINT8 status = 0;

  if (host->var.mem.serialDebugMsgLvl & level) {
    status = 1;
  }

  return status;
} // checkMsgLevel


#endif  // SERIAL_DBG_MSG
void
disableAllMsg (
              PSYSHOST  host
              )
/*++

  Disables serial debug messages

  @param host  - Pointer to the system host (root) structure

--*/
{
#ifdef  SERIAL_DBG_MSG
  host->var.mem.serialDebugMsgLvl = 0;
#endif  // SERIAL_DBG_MSG
} // disableAllMsg

void
restoreMsg (
           PSYSHOST  host
           )
/*++

  Restores serial debug messages

  @param host  - Pointer to the system host (root) structure

--*/
{
#ifdef  SERIAL_DBG_MSG
  UINT8 sbspSktId = GetSbspSktId(host);

  host->var.mem.serialDebugMsgLvl = host->setup.common.serialDebugMsgLvl;
  if (ReadCpuPciCfgEx (host, sbspSktId, 0, BIOSSCRATCHPAD0_UBOX_MISC_REG) & BIT4) {
    host->var.mem.serialDebugMsgLvl = host->var.mem.serialDebugMsgLvl | SDBG_MAX;
  }
#endif  // SERIAL_DBG_MSG
} // restoreMsg

void
debugPrintMmrc (
              UINT32    dbgLevel,
              char*     Format,
              ...
              )
/**

  MMRC specific print to serial output

  @param dbgLevel  - debug level
  @param Format    - string format

  @retval N/A

**/
{
#ifdef SERIAL_DBG_MSG
  PSYSHOST host = NULL;
  UINT8    release = 0;
  va_list  Marker;

  host = GetMrcHostStructureAddress ();

  if (host != NULL) {
    if (dbgLevel == SDBG_MIN) {
      dbgLevel = SDBG_MINMAX;
    }

    //
    // Check the debug message level
    //
    if (!checkMsgLevel(host, dbgLevel)) return;

    //
    // Get control of rcPrintf
    //
    if (host && host->var.common.printfSync) {
      release = getPrintFControl (host);
    }

    //
    // Display device
    //
    DisplayDev (host, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT);

    va_start (Marker, Format);
    rcVprintf (host, Format, Marker);

    //
    // Release control of rcPrintf if necessary
    //
    if (host && host->var.common.printfSync && release) {
      releasePrintFControl (host);
    }
  } //host != NULL
#endif
  return;
}

void
debugPrintMemFunction (
              PSYSHOST  host,
              UINT32    dbgLevel,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     rank,
              UINT8     strobe,
              UINT8     bit,
              char*     Format,
              va_list   Marker
              )
/*++

Routine Description:

  MRC specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param dbgLevel  - debug level
  @param Format    - string format
  @param Marker    - va_list that points to the arguments for Format that are on the stack

  @retval N/A

--*/
{
#ifdef SERIAL_DBG_MSG
  UINT8   release = 0;

  //
  // Check host for null
  //
  if (host != NULL) {
  
    //
    // Check the debug message level
    //
    if (!checkMsgLevel(host, dbgLevel)) return;

    //
    // Get control of rcPrintf
    //
    if (host->var.common.printfSync) {
      release = getPrintFControl (host);
    }

    //
    // Display device
    //
    DisplayDev (host, socket, ch, dimm, rank, strobe, bit);
    rcVprintf (host, Format, Marker);

    //
    // Release control of rcPrintf if necessary
    //
    if (host->var.common.printfSync && release) {
      releasePrintFControl (host);
    }
  }
#endif
  return;
}

void
debugPrintMem (
              PSYSHOST  host,
              UINT32    dbgLevel,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     rank,
              UINT8     strobe,
              UINT8     bit,
              char*     Format,
              ...
              )
/*++

Routine Description:

  MRC specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param dbgLevel  - debug level
  @param Format    - string format

  @retval N/A

--*/
{
#ifdef SERIAL_DBG_MSG
  va_list Marker;
  va_start (Marker, Format);
  debugPrintMemFunction(host, dbgLevel, socket, ch, dimm, rank, strobe, bit, Format, Marker);
#endif
  return;
}

void
AcquireReleaseGlobalSemaphore (
   PSYSHOST  host,
   UINT8     SysSemaphoreNum,
   UINT8     OpCode
   )
/*++
  Accquire or Release the global semaphore
  Called by the BSP of a socket
  Used for ensuring exclusive access to resoruces among CPU sockets
       IMPORTANT:
           This function must be called in the sequence below:
                ......
                AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, ACQUIRE);   // Acquire ownership of semaphore
                do_something();                                                    //  Perform intended task
           AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, RELEASE);    //  Release ownership of semaphore
                ......

  @param host      - Pointer to the system host structure
  @param SysSemaphoreNum,   - SYSTEMSEMAPHORE register number (0 or 1)
  @param OpCode - 1/0   ACQUIRE_SEMAPHORE / RELEASE_SEMAPHORE

  @retval  None

--*/
{
  UINT32  RegAddr;
  UINT8   MySocId;
  UINT8   LegacySocId;
  SYSTEMSEMAPHORE0_UBOX_MISC_STRUCT   Reg32;

  MySocId = host->var.common.socketId;
  LegacySocId = host->nvram.common.sbspSocketId;
  if (SysSemaphoreNum == SYSTEMSEMAPHORE0) {
    RegAddr = SYSTEMSEMAPHORE0_UBOX_MISC_REG;
  } else {
    RegAddr = SYSTEMSEMAPHORE1_UBOX_MISC_REG;
  }
  //
  // Read SYSTEMSEMAPHORE0 CSR on legacy socket
  //
  Reg32.Data = ReadCpuPciCfgEx (host, LegacySocId, 0, RegAddr);

  //
  // Set requesternode field
  //
  Reg32.Bits.requesternode = MySocId;

  if (OpCode == RELEASE_SEMAPHORE)   {
    //
    // Release Semaphore -  Write requesternode = MySocId, and bus_aqu_release = 0
    //
    Reg32.Bits.bus_aqu_release = 0;
    WriteCpuPciCfgEx (host, LegacySocId, 0, RegAddr, Reg32.Data);
  }

  if (OpCode == ACQUIRE_SEMAPHORE)   {
    //
    // Accquire Semaphore - Write requesternode = MySocId, and bus_aqu_release = 1
    //
    Reg32.Bits.bus_aqu_release = 1;
    WriteCpuPciCfgEx (host, LegacySocId, 0, RegAddr, Reg32.Data);
    //
    // Poll until semaphore ownership is granted
    //
    do {
      Reg32.Data = ReadCpuPciCfgEx (host, LegacySocId, 0, RegAddr);
    }
    while (!((Reg32.Bits.bus_aqu_release == 1) && (Reg32.Bits.currentnode == MySocId)));
  }
}
