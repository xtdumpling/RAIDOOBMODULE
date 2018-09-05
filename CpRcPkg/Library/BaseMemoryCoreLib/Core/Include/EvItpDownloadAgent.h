/**

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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

@file
  EvItpDownloadAgent.h

@brief
  BIOS SSA loader main definitions.

**/

#ifndef __EV_ITP_DOWNLOAD_AGENT_H__
#define __EV_ITP_DOWNLOAD_AGENT_H__

#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG

#ifndef MINIBIOS_BUILD
#include <Library/DebugLib.h>
#endif //MINIBIOS_BUILD

#include "SysHost.h"
#include "MrcSsaServices.h"
#pragma warning(disable : 4214)

//
// EV agent command list
//
#define MRC_EVAGENT_CMD_PING                        0x0
#define MRC_EVAGENT_CMD_GET_CALLBACK_REV            0x1
#define MRC_EVAGENT_CMD_GET_TOOL_BUFFER             0x2
#define MRC_EVAGENT_CMD_LOAD_TOOL                   0x3
#define MRC_EVAGENT_CMD_UNLOAD_TOOL                 0x4
#define MRC_EVAGENT_CMD_GET_INPUT_BUFFER            0x5
#define MRC_EVAGENT_CMD_FREE_MEMORY                 0x6
#define MRC_EVAGENT_CMD_RUN_EV_TOOL                 0x7
#define MRC_EVAGENT_CMD_EXIT                        0x8
#define MRC_EVAGENT_CMD_GET_AGENT_VERSION           0x9
#define MRC_EVAGENT_CMD_GET_DLL_VERSION             0xA
#define MRC_EVAGENT_CMD_FLUSH_CACHE                 0xB
#define MRC_EVAGENT_CMD_CHANGE_PRINT_MASK           0xC
#define MRC_EVAGENT_CMD_SAVE_PRINT_MASK             0xD
#define MRC_EVAGENT_CMD_RESTORE_PRINT_MASK          0xE
#define MRC_EVAGENT_CMD_START_AGENT                 0xF
#define MRC_EVAGENT_CMD_START_TARGET_ONLY           0x10
#define MRC_EVAGENT_CMD_FREE_RESULTS                0x11

//
// EV agent status and error code
//
#define MRC_EVAGENT_PENDING_CMD                     1
#define MRC_EVAGENT_NO_PENDING_CMD                  0
#define MRC_EVAGENT_STATUS_READY                    0
#define MRC_EVAGENT_STATUS_BUSY                     1

#define MRC_EVAGENT_CMD_NO_ERROR                    0x0
#define MRC_EVAGENT_CMD_ERROR_UNKNOWN_CMD           0x1
#define MRC_EVAGENT_CMD_ERROR_MALLOC_FAIL           0x2
#define MRC_EVAGENT_CMD_ERROR_INVALID_TOOL_HANDLE   0x3
#define MRC_EVAGENT_CMD_ERROR_INVALID_MEMORY_HANDLE 0x4

//
// Definition of the EV agent command register
//
typedef union {
  struct{
    UINT32 status                    :1;
    UINT32 error_code                :6;
    UINT32 command_pending_execution :1;
    UINT32 command_opcode            :8;
    UINT32 reserved                  :16;
  } Bits;
  UINT32 Data;
} MRC_EV_AGENT_CMD_STRUCT;

VOID EvItpDownloadCmd(PSYSHOST  host, VOID  *CheckpointData);

typedef EFI_STATUS (EvToolEntryPoint) (SSA_BIOS_SERVICES_PPI* SsaBiosServicesPpi, void * Configuration);

#ifndef TOOL_MAJOR_VERSION
  #define TOOL_MAJOR_VERSION 0
#endif
#ifndef TOOL_MINOR_VERSION
  #define TOOL_MINOR_VERSION 0
#endif
#ifndef TOOL_REVISION
  #define TOOL_REVISION 0
#endif
#endif //SSA_LOADER_FLAG

#else
//Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
#endif //__EV_ITP_DOWNLOAD_AGENT_H__
