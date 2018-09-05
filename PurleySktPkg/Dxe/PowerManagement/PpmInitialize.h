//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/** @file

  Header File for CPU Power Management

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  PpmInitialize.h

**/

#ifndef _PPM_INITIALIZE_H
#define _PPM_INITIALIZE_H

#include "Library/PcdLib.h"
#include <Protocol/PciCallback.h>
#include <Protocol/MpService.h>
#include <Setup/IioUniversalData.h>
//Libraries
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>
#include <Protocol/CpuCsrAccess.h>
#include <RcRegs.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CpuPpmLib.h>
#include "SysHost.h"
#include "UncoreCommonIncludes.h"

#define B_PCPS_SPD_ENABLE                  (1 << 16) // Bit 16
#define B_PCPS_HT_ENABLE                   (1 << 17) // Bit 17
#define LLC_SLICE_EN_MASK            ((1 << 28) - 1) // Bit[27:0]
#define SKU_18C                      0x3FFFF
//PCU_CR_CAPID4_CFG_REG.PHYSICAL_CHOP
//0x0 –LCC
//0x2 –HCC (L0)
//0x3 –XCC (A0/A1/A2/B0/B1/H0)
#define CAPID4_PHYSICAL_CHOP         ((1 << 7)|(1 << 6)) // Bits[7:6] 
#define SKX_REV_AxBxH0               (0x03 << 6)

//MSR_CORE_THREAD_COUNTbit15:0 - threadcount
#define THREAD_COUNT_MASK           (0xFFFF)

extern EFI_MP_SERVICES_PROTOCOL     *mMpService;
extern PPM_FROM_PPMINFO_HOB         *mPpmInfo;
extern EFI_CPU_PM_STRUCT            mPpmLib;

/* CPUPM POST code - Major */
#define STS_PPM_STRUCT_INIT        0xD0  // CPU PM Structure Init
#define STS_PPM_CSR_PROGRAMMING    0xD1  // CPU PM CSR programming
#define STS_PPM_MSR_PROGRAMMING    0xD2  // CPU PM MSR programming
#define STS_PPM_PSTATE_TRANSITION  0xD3  // CPU PM PSTATE transition
#define STS_PPM_EXIT               0xD4  // CPU PM driver exit
#define STS_PPM_ON_READY_TO_BOOT   0xD5  // CPU PM On ready to boot event

VOID
InitializeCpuInfoStruct(
  VOID
  );


#endif