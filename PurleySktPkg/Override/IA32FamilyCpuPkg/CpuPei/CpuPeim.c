/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
Implementation of CPU driver for PEI phase.

This PEIM is to expose the Cache Ppi and BIST hob build notification

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

Module Name: CpuPeim.c

**/

#include "CpuPei.h"
#include "Bist.h"

extern PEI_CACHE_PPI                mCachePpi;
//
// Ppis to be installed
//
EFI_PEI_PPI_DESCRIPTOR           mPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiCachePpiGuid,
    &mCachePpi
  }
};

//
// Notification to build BIST information
//
STATIC EFI_PEI_NOTIFY_DESCRIPTOR        mNotifyList[] = {
  {
// OVERIDE_HSD_5370237_BEGIN
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK),
    &gEfiEndOfPeiSignalPpiGuid,
    BuildBistAndCpuInfoHob
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK),
    &gPeiPostScriptTablePpiGuid,
    S3RestoreMsrCallback
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMpServicesPpiGuid,
    PeiMpServicesReadyCallback
  }
};

/**

  Call into Cpu library to restore the necessary MSR settings late in S3 resume.
  This callback is used for MSRs that must be set late in S3 resume such as lock bits
  or features that cannot be handled by WriteRegisterTable restore method.

  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS  - S3 Cpu MSR restore call completed successfully.

**/
EFI_STATUS
EFIAPI
S3RestoreMsrCallback (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  EFI_STATUS Status;

  //
  // Restore MSR settings required late in S3 resume process
  //
  Status = S3RestoreCpuMsrs(PeiServices);

  return Status;
}

/**

  Call into Cpu library to perform any PEI CPU Initialization needed
  in PEI phase that requires multi-threaded execution. This is executed
  on normal boots and S3 resume.

  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS  - PEI MP Services callback completed successfully.

**/
EFI_STATUS
EFIAPI
PeiMpServicesReadyCallback (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  EFI_STATUS Status;

  //
  // Hook for CPU initialization after MpServices PPI is installed
  //
  Status = LatePeiCpuInit(PeiServices);

  return Status;
}

// OVERIDE_HSD_5370237_END

/**
  Initialize SSE support.
**/
VOID
InitXMM (
  VOID
  )
{

  UINT32  RegEdx;

  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);

  //
  //Check whether SSE2 is supported
  //
  if ((RegEdx & BIT26) != 0) {
    AsmWriteCr0 (AsmReadCr0 () | BIT1);
    AsmWriteCr4 (AsmReadCr4 () | BIT9 | BIT10);
  }
}


/**
  The Entry point of the CPU PEIM

  This function is the Entry point of the CPU PEIM which will install the CachePpi and
  BuildBISTHob notifier. And also the function will deal with the relocation to memory when
  permanent memory is ready

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   CachePpi and BIST hob build notification is installed
                        successfully.

**/
EFI_STATUS
EFIAPI
CpuPeimInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  //
  // Report Status Code to indicate the start of CPU PEIM
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_HOST_PROCESSOR + EFI_CU_HP_PC_POWER_ON_INIT
    );

  //
  // Get CPU BIST information
  //
  Status = RetrieveCpuInfo (PeiServices);
  ASSERT_EFI_ERROR (Status);

  //
  // Install PPIs
  //
  Status = PeiServicesInstallPpi(&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Register for PPI Notifications
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Report Status Code to indicate the start of CPU PEI initialization
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_HOST_PROCESSOR + EFI_CU_PC_INIT_BEGIN
    );

  InitXMM ();

  return Status;
}
