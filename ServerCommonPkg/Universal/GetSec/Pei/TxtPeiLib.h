/**
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
**/
/**

Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file TxtPeiLib.h

  This file contains function definitions that can determine
  the LT capabilities of a platform during PEI and perform
  certain specific platform tasks that are required for LT
  during PEI.

**/

#ifndef _TXT_PEI_LIB_H_
#define _TXT_PEI_LIB_H_

#include <CpuBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/LocalApicLib.h>
#include <Library/PcdLib.h>
#include <Ppi/TxtSclean.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/MpService.h>
#include <Guid/PlatformTxt.h>

#define MKF_TPM_PRESENT
#define EFI_CPUID_VERSION_INFO     0x1
#define EFI_MSR_LT_CONTROL         0x2E7

#define LT_ESTS                    0x0008
#define LT_E2STS                   0x08F0
#define TXT_PUBLIC_BASE            0xFED30000

// This is only defined in platform include files
//  so we need a definition at the ServerCommon level
//
// The PCD is defined in ServerCommonPkg.dec and can be overridden in
//  PlatformPkg.dsc
//
#define MAX_SOCKET_COUNT           FixedPcdGet32 (PcdMaxCpuSocketCount)

extern EFI_GUID   gPeiPostScriptTablePpiGuid;
extern EFI_GUID   gPeiTxtMemoryUnlockedPpiGuid;
extern EFI_GUID   gEfiPeiMemoryDiscoveredPpiGuid;
extern EFI_GUID   gMemoryInitDoneSentGuid;

typedef struct _LT_PEI_LIB_CONTEXT_ {
  CONST EFI_PEI_SERVICES           **PeiServices;
  EFI_PEI_CPU_IO_PPI               *CpuIoPpi;
  EFI_PEI_PCI_CFG2_PPI             *PciCfgPpi;
  EFI_PEI_STALL_PPI                *PeiStall;
  PEI_MP_SERVICES_PPI              *PeiMpServices;
  EFI_PLATFORM_TXT_DEVICE_MEMORY   *PlatformTxtDeviceMemory;
  EFI_PLATFORM_TXT_POLICY_DATA     *PlatformTxtPolicyData;
  UINT8                            ApVector;
  UINT8                            ApCount;
} LT_PEI_LIB_CONTEXT;

//
// Structure used to hold Socket information for S3 resume
//
typedef struct {
  UINT8     SocketIndex;
  UINT8     SocketFoundId;
  UINT16    ApApicId;
} LT_PEI_SOCKET_INFO;

EFI_STATUS
InitializeLtPeiLib (
  IN      CONST EFI_PEI_SERVICES   **PeiServices,
  IN OUT  LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtProcessor (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

VOID
EnableTxt (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

VOID
LockLtMsr (
  IN      VOID                    *Buffer
  );

VOID
LockLtMsrOnAll(
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtSxProcessor (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsTpmPresent (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtEstablished (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtWakeError (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtResetStatus (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

BOOLEAN
IsLtEnabled (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

VOID
PowerCycleReset (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

EFI_STATUS
DoClearSecrets (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

EFI_STATUS
DoSclean (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

EFI_STATUS
PeiTxtLockConfigForAll (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

EFI_STATUS
PeiDoLockConfigForAll (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

VOID
DoLockConfig (
  IN      VOID                    *Buffer
  );

VOID
EFIAPI
DoLockConfigOnAP (
  IN      VOID                    *Buffer
  );

// BEGIN_OVERRIDE_HSD_5331848
//
// LtPeiLibLaunchBiosAcm is an internal routine used to
// wrap the BIOS ACM initialization code for the various
// functions inside of it.
//
STATIC
EFI_STATUS
LtPeiLibLaunchBiosAcm(
IN LT_PEI_LIB_CONTEXT      *LtPeiCtx,
IN UINT32                  AcmFunction
);
// END_OVERRIDE_HSD_5331848

#endif
