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
 *
 ************************************************************************/
//#pragma warning (disable: 4127 4100)     // disable C4127: constant conditional expression
#include "SysFunc.h"
#include "Pipe.h"

/**

  Get socket's imc mem variable segment of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
GetMemVarData (
              PSYSHOST host,
              UINT8    socketId
              )
{

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  GetPipePackage(host, socketId, (UINT8*)&host->var.mem.socket[socketId], sizeof(struct Socket));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

}

/**

  Get socket's imc nvram segment of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
GetMemNvramCommonData (
                      PSYSHOST host,
                      UINT8    socketId
                      )
{

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.socket[socketId], sizeof(struct socketNvram));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

}

/**

  Get socket's imc nvram segment(not covered by socketNvram) of the SYSHOST structure from the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
GetMemNvramData (
                PSYSHOST host,
                UINT8    socketId
                )
{
  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;

  host->var.common.rcWriteRegDump = 0;
  //GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem, sizeof(struct memNvram));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.RASmode, sizeof(UINT8));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.RASmodeEx, sizeof(UINT8));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.ratioIndex, sizeof(UINT8));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.eccEn, sizeof(UINT8));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.dimmTypePresent, sizeof(UINT8));
  //GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.dramType, sizeof(UINT16));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.XMPProfilesSup, sizeof(UINT8));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.crc16, sizeof(INT16));
  GetPipePackage(host, socketId, (UINT8*)&host->nvram.mem.normalizationFactor[socketId][0], sizeof(INT16) * MAX_CH); // normalizationFactor[MAX_SOCKET][MAX_CH]
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

}

/**

  Send BSP's socket imc mem variable segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
SendMemVarData (
               PSYSHOST host,
               UINT8    socketId
               )
{

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  SendPipePackage(host, socketId, (UINT8*)&host->var.mem.socket[socketId], sizeof(struct Socket));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

}

/**

  Send BSP socket imc nvram segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
SendMemNvramCommonData (
                       PSYSHOST host,
                       UINT8    socketId
                       )
{

  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.socket[socketId], sizeof(struct socketNvram));
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;

}

/**

  Send BSP socket imc nvram (not covered by socketNvram) segment of the SYSHOST structure to the specified slave processor

  @param host      - Pointer to the system host (root) structure
  @param socketId  - PIPE Slave Id

  @retval N/A

**/
void
SendMemNvramData (
                 PSYSHOST host,
                 UINT8    socketId
                 )
{
  UINT8 rcWriteRegDumpCurrent = host->var.common.rcWriteRegDump;

  host->var.common.rcWriteRegDump = 0;
  //SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem, sizeof(struct memNvram));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.RASmode, sizeof(UINT8));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.RASmodeEx, sizeof(UINT8));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.ratioIndex, sizeof(UINT8));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.eccEn, sizeof(UINT8));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.dimmTypePresent, sizeof(UINT8));
  //SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.dramType, sizeof(UINT16));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.XMPProfilesSup, sizeof(UINT8));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.crc16, sizeof(INT16));
  SendPipePackage(host, socketId, (UINT8*)&host->nvram.mem.normalizationFactor[socketId][0], sizeof(INT16) * MAX_CH); // normalizationFactor[MAX_SOCKET][MAX_CH]
  host->var.common.rcWriteRegDump = rcWriteRegDumpCurrent;
}
