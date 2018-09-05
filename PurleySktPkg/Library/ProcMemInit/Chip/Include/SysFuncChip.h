//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
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
#ifndef  _sysfuncchip_h
#define  _sysfuncchip_h

#include "FnvAccess.h"
#ifndef MINIBIOS_BUILD
#include <Library/OemProcMemInit.h>
#else
#include "OemProcMemInit.h"
#endif // MINIBIOS_BUILD
#include "MemProjectSpecific.h"
#include "KtiLib.h"
#include "RcRegs.h"
#include "CpuPciAccess.h"
#include "MemFuncChip.h"

//
// CpuPciAccess.c
//
//
// PmTimer.c
//
VOID ConfigMesh2MemCsrForMirrorRASHook (PSYSHOST host, struct SADTable *SADEntry, SAD2TAD_M2MEM_MAIN_STRUCT *meshSad2Tad);

#ifndef ASM_INC
UINT32 GetRegisterOffset (PSYSHOST host, CSR_OFFSET RegOffset, UINT8 CpuType);                      // CHIP
#endif

/**

  Read from a non-sticky scratch pad

  @param host        Pointer to sysHost, the system host (root) structure struct
  @param socket      Socket Id
  @param scratchpad  Scratchpad to write

  @retval Data read from the requested scratch pad

**/
UINT32 GetNonStickyScratchpad(PSYSHOST host, UINT8 socket, UINT8 scratchpad);

/**

  Write to a non-sticky scratch pad

  @param host       - Pointer to sysHost, the system host (root) structure struct
  @param socket     - Socket Id
  @param scratchpad - Scratchpad to write
  @param data       - Data to be written

  @retval None

**/
void   SetNonStickyScratchpad(PSYSHOST host, UINT8 socket, UINT8 scratchpad, UINT32 data);

#endif   // _sysfuncchip_h
