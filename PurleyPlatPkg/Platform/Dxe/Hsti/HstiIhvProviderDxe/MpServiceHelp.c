/** @file
  This file contains the mpservices helper functions

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/

#include "HstiIhvProviderDxe.h"

typedef struct {
  UINT32  Index;
  UINT64  Value;
} AP_PRPCEDURE_ARGUMENT_READMSR;

typedef struct {
  UINT32  Index;
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
} AP_PRPCEDURE_ARGUMENT_CPUID;

EFI_MP_SERVICES_PROTOCOL  *mMpService;
UINTN                     mBspNumber;
UINTN                     mNumberOfProcessors;
UINTN                     mNumberOfEnabledProcessors;

/**
  Initialize MP Helper
**/
VOID
InitMp (
  VOID
  )
{
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "InitMp\n"));

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **) &mMpService);
  if (EFI_ERROR (Status)) {
    mMpService = NULL;
  }

  if (mMpService != NULL) {
    Status = mMpService->WhoAmI (mMpService, &mBspNumber);
    ASSERT_EFI_ERROR (Status);

    Status = mMpService->GetNumberOfProcessors (mMpService, &mNumberOfProcessors, &mNumberOfEnabledProcessors);
    ASSERT_EFI_ERROR (Status);
  } else {
    mBspNumber = 0;
    mNumberOfProcessors = 1;
    mNumberOfEnabledProcessors = 1;
  }

  DEBUG ((DEBUG_INFO, "BspNumber                 - 0x%x\n", mBspNumber));
  DEBUG ((DEBUG_INFO, "NumberOfProcessors        - 0x%x\n", mNumberOfProcessors));
  DEBUG ((DEBUG_INFO, "NumberOfEnabledProcessors - 0x%x\n", mNumberOfEnabledProcessors));
}

/**
  Concatenate error string.

  @retval UINTN - CpuNumber.
**/
UINTN
GetCpuNumber (
  VOID
  )
{
  return mNumberOfEnabledProcessors;
}

VOID
EFIAPI
ApReadMsr64 (
  IN OUT VOID  *Buffer
  )
{
  AP_PRPCEDURE_ARGUMENT_READMSR  *Argument;

  Argument = Buffer;
  Argument->Value = AsmReadMsr64 (Argument->Index);
}

/**
  Concatenate error string.

  @param[in, out] Buffer     - Pointer to Argument

**/
VOID
EFIAPI
ApCpuId (
  IN OUT VOID  *Buffer
  )
{
  AP_PRPCEDURE_ARGUMENT_CPUID  *Argument;

  Argument = Buffer;
  AsmCpuid (Argument->Index, &Argument->Eax, &Argument->Ebx, &Argument->Ecx, &Argument->Edx);
}

/**
  Concatenate error string.

  @param[in] ProcessorNumber     - Processor ID
  @param[in] Index               - Index

  @retval UINT64 - Msr Value.
**/
UINT64
ProcessorReadMsr64 (
  IN UINTN   ProcessorNumber,
  IN UINT32  Index
  )
{
  EFI_STATUS                      Status;
  AP_PRPCEDURE_ARGUMENT_READMSR   Argument;

  ASSERT (ProcessorNumber < mNumberOfEnabledProcessors);
  if (ProcessorNumber == mBspNumber) {
    return AsmReadMsr64 (Index);
  }

  ZeroMem (&Argument, sizeof (Argument));
  Argument.Index = Index;
  Status = mMpService->StartupThisAP (
                         mMpService,
                         ApReadMsr64,
                         ProcessorNumber,
                         NULL,   // WaitEvent
                         0,      // TimeoutInMicroseconds
                         &Argument,
                         NULL    // Finished
                         );
  ASSERT_EFI_ERROR (Status);

  return Argument.Value;
}

/**
  Concatenate error string.

  @param[in]  ProcessorNumber     - Processor ID
  @param[in]  Index               - Index
  @param[out] Eax                 - Eax
  @param[out] Ebx                 - Ebx
  @param[out] Ecx                 - Ecx
  @param[out] Edx                 - Edx
**/
VOID
ProcessorCpuid (
  IN  UINTN   ProcessorNumber,
  IN  UINT32  Index,
  OUT UINT32  *Eax,  OPTIONAL
  OUT UINT32  *Ebx,  OPTIONAL
  OUT UINT32  *Ecx,  OPTIONAL
  OUT UINT32  *Edx   OPTIONAL
  )
{
  EFI_STATUS                      Status;
  AP_PRPCEDURE_ARGUMENT_CPUID     Argument;

  ASSERT (ProcessorNumber < mNumberOfEnabledProcessors);
  if (ProcessorNumber == mBspNumber) {
    AsmCpuid (Index, Eax, Ebx, Ecx, Edx);
    return;
  }

  ZeroMem (&Argument, sizeof (Argument));
  Argument.Index = Index;
  Status = mMpService->StartupThisAP (
                         mMpService,
                         ApCpuId,
                         ProcessorNumber,
                         NULL,   // WaitEvent
                         0,      // TimeoutInMicroseconds
                         &Argument,
                         NULL    // Finished
                         );
  ASSERT_EFI_ERROR (Status);

  if (Eax != NULL) {
    *Eax = Argument.Eax;
  }
  if (Ebx != NULL) {
    *Ebx = Argument.Ebx;
  }
  if (Ecx != NULL) {
    *Ecx = Argument.Ecx;
  }
  if (Edx != NULL) {
    *Edx = Argument.Edx;
  }

  return;
}