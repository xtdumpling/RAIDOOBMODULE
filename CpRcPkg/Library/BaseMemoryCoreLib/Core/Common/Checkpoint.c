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
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
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
#include "ChipApi.h"

#ifdef SSA_FLAG
#include "MrcSsaServices.h"
#include "ExecuteTargetOnly.h"
#endif //SSA_FLAG

/*
  Low Level Manager
  Tool for making BIOS function calls

  @param host - Pointer to sysHost

  @retval SUCCESS
*/
UINT32
ValRequestHandlerAtBreakPoint (
  PSYSHOST host
  )
{
  UINT8   socket;
  UINT8   currentSocket;
  UINT8   sbspSktId;
  UINT32  valFuncs = 0;
  UINT32  checkpoint = 0;

  sbspSktId = GetSbspSktId(host);

  //
  // Get the validation functions from a scratchpad register
  //
  CHIP_FUNC_CALL(host, GetSetValFunctions(host, sbspSktId, GET_REQ, &valFuncs));

  if (valFuncs & JEDEC_INIT_SEQ_REQ) {
    rcPrintf ((host, "Request to run Jedec Init\n"));
    //
    // Disable checkpoint breakpoints
    //
    checkpoint = CHIP_FUNC_CALL(host, GetBreakpointCsr(host, sbspSktId));
    CHIP_FUNC_CALL(host, SetBreakpointCsr(host, sbspSktId, 0));

    //
    // Call for each socket
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;
      JedecInitSequence(host, socket, CH_BITMASK);
    }

    //
    // Restore checkpoint breakpoint
    //
    CHIP_FUNC_CALL(host, SetBreakpointCsr(host, sbspSktId, checkpoint));

    //
    // Clear request for JEDEC init
    //
    valFuncs &= ~JEDEC_INIT_SEQ_REQ;
    CHIP_FUNC_CALL(host, GetSetValFunctions(host, sbspSktId, RESTORE_REQ, &valFuncs));
  }

  if ((valFuncs & RANK_MARGIN_TOOL_REQ) && (host->setup.mem.options & MARGIN_RANKS_EN)) {
    rcPrintf ((host, "Request to run Rank Margin tool\n"));
    //
    // Disable checkpoint breakpoints
    //
    checkpoint = CHIP_FUNC_CALL(host, GetBreakpointCsr(host, sbspSktId));
    CHIP_FUNC_CALL(host, SetBreakpointCsr(host, sbspSktId, 0));

    currentSocket = host->var.mem.currentSocket;

    //
    // Call for each socket
    //
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) continue;
      host->var.mem.currentSocket = socket;
#ifdef MARGIN_CHECK
      RankMarginTool(host);
#endif
    }

    host->var.mem.currentSocket = currentSocket;

    //
    // Restore checkpoint breakpoint
    //
    CHIP_FUNC_CALL(host, SetBreakpointCsr(host, sbspSktId, checkpoint));

    //
    // Clear request for RMT
    //
    valFuncs &= ~RANK_MARGIN_TOOL_REQ;
    CHIP_FUNC_CALL(host, GetSetValFunctions(host, sbspSktId, RESTORE_REQ, &valFuncs));
  }

  return SUCCESS;
}


//
// -----------------------------------------------------------------------------
//
/**

  Socket SBSP writes the checkpoint code to the checkpoint CSR on socket SBSP and calls
  the platform checkpoint routine.

  Socket N reads the breakpoint CSR on socket N. If the breakpoint code matches the
  checkpoint code, then execution will stall in a loop until the breakpoint CSR
  on socket N is changed via ITP or other means.

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major Checkpoint code to write
  @param minorCode - Minor Checkpoint code to write
  @param data      - Data specific to the minor checkpoint is written to
                     low word of the checkpoint CSR

  @retval N/A

**/
void
OutputCheckpoint (
                 PSYSHOST host,
                 UINT8    majorCode,
                 UINT8    minorCode,
                 UINT16   data
                 )
{
  UINT16 checkpoint;
  UINT32 postcode;
  UINT8  socket;
  UINT32 breakpointLocation = 0;
#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG
#ifndef MINIBIOS_BUILD
  MRC_EV_AGENT_CMD_STRUCT  agentCmd;
#endif //MINIBIOS_BUILD
#endif //SSA_LOADER_FLAG
#endif //SSA_FLAG
  UINT8 sbspSktId;

  sbspSktId = GetSbspSktId(host);
  checkpoint = (UINT16)((majorCode << 8) | minorCode);

  socket = host->var.common.socketId;
  postcode =  CHIP_FUNC_CALL(host, SetPostCode(host, socket, checkpoint, data));

  //
  // Cache last checkpoint value per socket
  //
  host->var.mem.lastCheckpoint[socket] = postcode;

  if (host->var.common.socketId ==  host->nvram.common.sbspSocketId) {

    //
    // Update checkpoint in the host structure
    //
    host->var.common.checkpoint = postcode;

    //
    // Call platform function to handle the major checkpoint
    //
    PlatformCheckpoint (host);

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      //
      // Output the progress code
      //
      rcPrintf ((host, "Checkpoint Code: Socket %d, 0x%02X, 0x%02X, 0x%04X\n", host->var.common.socketId, majorCode, minorCode, data));
    }
#endif
  }


  // Compare the breakpoint CSR from socket 0
  breakpointLocation = CHIP_FUNC_CALL(host, GetBreakpointCsr(host, sbspSktId));
  if (breakpointLocation == checkpoint) {

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) {

      // Output message indicating that execution is stalled
      rcPrintf ((host, "Breakpoint match found. S%d waiting...\n", host->var.common.socketId));
      SerialSendBuffer (host, 1);
    }
#endif

    // Stall the boot until breakpoint changes
    while (checkpoint == CHIP_FUNC_CALL(host, GetBreakpointCsr(host, sbspSktId))) {

      ValRequestHandlerAtBreakPoint(host);


#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG
#ifndef MINIBIOS_BUILD
      if ((host->setup.mem.enableBiosSsaLoader) && (majorCode == 0xBE) && (minorCode == 0x02)) { //Enter if SSA EV loader is invoked
        // Check for opcode to start the SSA agent.
        agentCmd.Data = CHIP_FUNC_CALL(host, GetSsaAgentData(host, 0));


        rcPrintf ((host, "Checkpoint() -- agentCmd.Bits.command_opcode: 0x%08x \n", agentCmd.Bits.command_opcode));
        switch(agentCmd.Bits.command_opcode) {

        case MRC_EVAGENT_CMD_START_AGENT:

          rcPrintf ((host, "MRC_EVAGENT_CMD_START_AGENT received. \n"));
          EvItpDownloadCmd(host, (VOID*)0);
          CHIP_FUNC_CALL(host, ClearBpAndAgentData(host, 0));// SetStickyScratchpad (host, 0, SR_BIOS_SERIAL_DEBUG, 0);
          break;

        case MRC_EVAGENT_CMD_START_TARGET_ONLY:

          rcPrintf ((host, "MRC_EVAGENT_CMD_START_TARGET_ONLY received. \n"));
          ExecuteTargetOnlyCmd(host);
          CHIP_FUNC_CALL(host, SetStickyScratchpad (host, 0, SR_BIOS_SERIAL_DEBUG, 0));
          break;
        }  //switch

        if (agentCmd.Bits.command_opcode == 0) { //To prevent a hang
          break;
        }
      } //enableBiosSsaLoader
#endif //MINIBIOS_BUILD
#endif //SSA_LOADER_FLAG
#endif //SSA_FLAG
    } //while
  }
}

