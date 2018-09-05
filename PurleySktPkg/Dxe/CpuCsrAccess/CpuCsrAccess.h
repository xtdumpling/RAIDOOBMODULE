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
#ifndef  _CPU_CSR_ACCESS_H_
#define  _CPU_CSR_ACCESS_H_

#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/IioUds.h>
#include <Protocol/SmmBase2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "SysFunc.h"
#include "RcRegs.h"
#include "CpuPciAccess.h"
#include "SysHostChip.h"

UINT64 GetCpuCsrAddress (
  UINT8 SocId, 
  UINT8 BoxInst, 
  UINT32 Offset, 
  UINT8* Size
  );

UINT32 ReadCpuCsr (
  UINT8 SocId, 
  UINT8 BoxInst, 
  UINT32 Offset
  );

VOID WriteCpuCsr (
  UINT8 SocId, 
  UINT8 BoxInst, 
  UINT32 Offset, 
  UINT32 Data
  );

UINT32 ReadMcCpuCsr (
  UINT8 SocId, 
  UINT8 McId, 
  UINT32 Offset
  );

VOID WriteMcCpuCsr (
  UINT8 SocId, 
  UINT8 McId, 
  UINT32 Offset, 
  UINT32 Data
  );

UINTN
GetMcCpuCsrAddress (
  UINT8    SocId,
  UINT8    McId,
  UINT32   Offset
  );

UINT32 ReadPciCsr (
  UINT8 socket, 
  UINT32 reg
  );

VOID WritePciCsr (
  UINT8 socket, 
  UINT32 reg, 
  UINT32 data
  );

UINT32 GetPciCsrAddress (
  UINT8 socket,
  UINT32 reg
  );

VOID BreakAtCheckpoint (
  UINT8    majorCode,
  UINT8    minorCode,
  UINT16   data
  );

UINT32 Bios2PcodeMailBoxWrite (
  UINT8  socket, 
  UINT32 command, 
  UINT32 data
  );

UINT64 Bios2VcodeMailBoxWrite (
  UINT8  socket, 
  UINT32 command, 
  UINT32 data
  );

#endif   // _CPU_CSR_ACCESS_H_
