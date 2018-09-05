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
#ifdef _MSC_VER
#pragma warning(disable:4740)
#endif

#include "SysFunc.h"
#include "SysHostChip.h"
#include "KtiLib.h"
#ifndef MINIBIOS_BUILD
#include <Library/PchPmcLib.h>
#endif


#ifdef IA32
static
struct mcBankTableEntry mcBankTable[] = {
    //
    // List the MC Banks in the order of closeness to possible source of error
    // i.e., first IMC, HA, QPI/PCU, IIO, and finally CBO
    //
	{0,  0x400}, // IFU
	{1,  0x404}, // DCU
	{2,  0x408}, // DTLB
	{3,  0x40c}, // MLC
    {4,  0x410}, // PCU
    {5,  0x414}, // KTI0
    {6,  0x418}, // IIO
    {7,  0x41C}, // M2M0, IMC00, IMC01
    {8,  0x420}, // M2M1, IMC10, IMC11
    {9,  0x424}, // CHA0, CHA3, CHA6, CHA9, CHA12, CHA15, CHA18, CHA21, CHA24, CHA27
    {10, 0x428}, // CHA1, CHA4, CHA7, CHA10, CHA13, CHA16, CHA19, CHA22, CHA25
    {11, 0x42C}, // CHA2, CHA5, CHA8, CHA11, CHA14, CHA17, CHA20, CHA23, CHA26
    {12, 0x430}, // KTI1
    {13, 0x434}, // iMC00
    {14, 0x438}, // iMC01
    {15, 0x43C}, // iMC10
    {16, 0x440}, // iMC11
    {17, 0x444}, // iMC02
    {18, 0x448}, // iMC12
    {19, 0x44C}  // KTI2
};
#endif

/**

  Outputs error code hand halts if halt on fatal error, else exits reference code

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval N/A

**/
void
FatalError (
           PSYSHOST host,
           UINT8    majorCode,
           UINT8    minorCode
           )
{
  UINT8   haltOnError;
  UINT32  buffer;
  UINT8  sbspSktId;

  sbspSktId = GetSbspSktId(host);

  //
  // Read the last checkpoint
  //
  buffer = host->var.common.checkpoint & 0xFFFF0000;

  //
  // Write the major and minor error codes to the scratchpad register
  //
  buffer |= ((majorCode << 8) | minorCode);
  WriteCpuPciCfgEx (host, sbspSktId, 0, SR_ERROR_CODE_CSR, buffer);

  //
  // Default halt on error based on setup option
  //
  if (host->setup.common.options & HALT_ON_ERROR_EN) {
    haltOnError = 1;
  } else {
    haltOnError = 0;

  }

  //
  // Call platform hook to handle fatal error
  //
  PlatformFatalError (host, majorCode, minorCode, &haltOnError);

  if ((host->setup.mem.options & MULTI_THREAD_MRC_EN) && (host->var.common.socketPresentBitMap > 1)) {
    host->var.mem.socket[host->var.common.socketId].fatalError = 1;
    host->var.mem.socket[host->var.common.socketId].majorCode = majorCode;
    host->var.mem.socket[host->var.common.socketId].minorCode = minorCode;
    if (host->var.common.socketId == host->nvram.common.sbspSocketId) {
      //
      // Let all the non-BSP's know the BSP has a fatal error
      //
      rcPrintf ((host, "\nFatalError: BSP - SocketId = %d registered Major Code = 0x%2x, Minor Code = 0x%2x \n",
                host->var.common.socketId, majorCode, minorCode));

      //
      // Sync with non-BSP sockets
      //
      SyncErrors(host);

    } else {
      //
      // Let the BSP know this non-BSP had a fatal error
      //
      rcPrintf ((host, "\nFatalError: nonBSP -  SocketId = %d registered Major Code = 0x%2x, Minor Code = 0x%2x \n",
                host->var.common.socketId, majorCode, minorCode));

      SyncErrors(host);
    }
  } else {
    rcPrintf ((host, "\nFatalError: SocketId = %d registered Major Code = 0x%2x, Minor Code = 0x%2x \n",
              host->var.common.socketId, majorCode, minorCode));
  }

#ifdef SERIAL_DBG_MSG
  //
  // Flush buffer
  //
  SerialSendBuffer(host, 1);
#endif // SERIAL_DBG_MSG

  HaltOnError(host, majorCode, minorCode);
}

/**

  Checks MSRs with UC set and logs the register data for further use

  @param host

  @retval N/A

 Collect only UNCORE MC Bank information.
**/
void
CollectPrevBootFatalErrors(
   PSYSHOST host
   )
{
#ifndef MINIBIOS_BUILD
#ifdef IA32
  UINT16  noOfMcBanks = sizeof(mcBankTable)/sizeof(struct mcBankTableEntry);
  UINT8   mcIndex;
  UINT8   socket;
  UINT64_STRUCT mcStatus;
  UINT64_STRUCT mcAddress;
  UINT64_STRUCT mcMisc;
  UINT8   entry;
  UINT8   cboCount;
  UINT32  cboMap[MAX_SOCKET];
  BOOLEAN inDWR;

  inDWR = 	PchIsDwrFlow() ;
  rcPrintf ((host, "CollectPrevBootFatalErrors: inDWR: %d\n", inDWR));
  //
  // Get the list of CBOs present for each socket
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Skip skts that are not present
    //
    cboMap[socket] = 0;
    if ((host->var.common.socketPresentBitMap & (1 << socket)) == 0) continue;
    GetChaCountAndList (host, socket, &cboCount, &cboMap[socket]);
  }

  entry = 0;
  host->prevBootErr.validEntries = 0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (mcIndex = 0; mcIndex < noOfMcBanks; mcIndex++) {
      //
      // Skip skts that are not present
      //
      if ((host->var.common.socketPresentBitMap & (1 << socket)) == 0) continue;

      if (host->prevBootErr.validEntries >= MAX_PREV_BOOT_ERR_ENTRIES) {
        rcPrintf ((host, "Warning! Num of MC BANKS with UC error exceeds %d - max entries!\n", MAX_PREV_BOOT_ERR_ENTRIES));
        return;
      }


      mcStatus = ReadMsrPipe(host, socket, mcBankTable[mcIndex].mcBankAddress + MC_BANK_STATUS_REG);
      mcAddress = ReadMsrPipe(host, socket, mcBankTable[mcIndex].mcBankAddress + MC_BANK_ADDRESS_REG);
      mcMisc = ReadMsrPipe(host, socket, mcBankTable[mcIndex].mcBankAddress + MC_BANK_MISC_REG);
      if (inDWR) {
        rcPrintf ((host, "Socket: %d, Machine Check Bank %d: Status: 0x%08x%08x, Address: 0x%08x%08x, Misc: 0x%08x%08x\n",
    		     socket, mcIndex, mcStatus.hi, mcStatus.lo, mcAddress.hi, mcAddress.lo, mcMisc.hi, mcMisc.lo));
      }
      //
      // Skip if there is no fatal error / log not valid / not enabled
      //
      if ((mcStatus.hi & (MSR_LOG_VALID | MSR_LOG_UC | MSR_LOG_EN)) != (MSR_LOG_VALID | MSR_LOG_UC | MSR_LOG_EN)) continue;

      //
      // Capture log information
      //
      host->prevBootErr.mcRegs[entry].socketId  = socket;
      host->prevBootErr.mcRegs[entry].mcBankNum = mcBankTable[mcIndex].mcBankNum;
      host->prevBootErr.mcRegs[entry].mcStatus  = mcStatus;
      host->prevBootErr.mcRegs[entry].mcAddress = mcAddress;
      host->prevBootErr.mcRegs[entry].mcMisc    = mcMisc;

      entry++;
      host->prevBootErr.validEntries = entry;
    } // mcbank loop
  } // skt loop

  rcPrintf ((host, "PrevBootErrors - Valid MCA UC entries: %d\n", host->prevBootErr.validEntries));
#endif // IA32
#endif // SIM_BUILD
}
