/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2008 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file MemRas.c

    Mem Ras functions and platform data, which needs to be
    ported.

---------------------------------------------------------------------------**/

//
// Includes
//
#include "MemRas.h"
#include "PatrolScrub.h"
#include "RankSparing.h"
#include "DramDeviceCorrection.h"
#include "AdddcSparing.h"
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks Support

extern   EFI_QUIESCE_SUPPORT_PROTOCOL           *mQuiesceSupport;

EFI_MEM_RAS_DRIVER_PRIVATE                      *mMemRasPrivate;
struct SystemMemoryMapHob                       *mSystemMemoryMap = NULL;
MEMRAS_S3_PARAM                                 mMemrasS3Param;
EFI_MEM_RAS_PROTOCOL                            *mMemRas;
EFI_CPU_CSR_ACCESS_PROTOCOL                     *mCpuCsrAccess;
EFI_SMM_CPU_SERVICE_PROTOCOL                    *m2SmmCpuServiceProtocol;
EFI_CRYSTAL_RIDGE_PROTOCOL                      *mCrystalRidgeProtocol;
EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL       *mSmmPeriodicTimerDispatch = NULL;
EFI_HANDLE                                      mPeriodicTimerHandle = NULL;
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
UEFI_MEM_ERROR_RECORD                           *gMemRasMemoryErrorRecord;
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
UINT8 OppSelfRefresh = 0;

#define EFI_MEM_RAS_SIGNATURE  SIGNATURE_32 ('R', 'M', 'E', 'M')
#define MRC_STACK 0x8000

UINT8    *mMrcPrivateStack;
UINT8    *mCurrHostStack;
UINT8    *mCurrGuestStack;
UINT8    *mGuestStackTop;
VOID     *mScheduleFunc;
struct   sysHost mHost;
UINT64   mRasFlags;
UINT8    mMaxBusNumber;

//
// Driver entry point
//

/**

  This function Is entry point for Memory Ras driver

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InitializeMemRas (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_IIO_UDS_PROTOCOL  *mIioUds;
  UINT8 Socket;
  UINTN StackSize;
  EFI_HOB_GUID_TYPE         *GuidHob;


  //
  // Allocate the protocol instance and install the protocol
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (EFI_MEM_RAS_DRIVER_PRIVATE), &mMemRasPrivate);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (mMemRasPrivate, sizeof (EFI_MEM_RAS_DRIVER_PRIVATE));

  //
  // Allocate the protocol instance and install the protocol
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (EFI_MEM_RAS_PROTOCOL), &mMemRas);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (mMemRas, sizeof (EFI_MEM_RAS_PROTOCOL));

  mMemRasPrivate->mMemRas = mMemRas;

  //
  // Allocate the MRC stack for 32K
  //
  StackSize = MRC_STACK;
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, StackSize, &mMrcPrivateStack);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (mMrcPrivateStack, StackSize);

  mGuestStackTop = mMrcPrivateStack + StackSize - sizeof(UINTN);
  mCurrHostStack = NULL;
  mCurrHostStack = NULL;

  //
  // Allocate the protocol instance and install the protocol
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (struct SystemMemoryMapHob), &mMemRas->SystemMemInfo);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (mMemRas->SystemMemInfo, sizeof (struct SystemMemoryMapHob ));

// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (UEFI_MEM_ERROR_RECORD), &gMemRasMemoryErrorRecord);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (gMemRasMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support

  Status = gSmst->SmmLocateProtocol (&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, NULL, (VOID **)&mSmmPeriodicTimerDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize our protocol
  //
  mMemRasPrivate->Signature = 0x12345678;
  Status = InitializeMemRasData( mMemRas );
  ASSERT_EFI_ERROR (Status);

  mMemRasPrivate->Signature = EFI_MEM_RAS_SIGNATURE;

  mMemRas->pSysHost                           = (PSYSHOST)&mHost;
  mMemRas->IsMemNodeEnabled                   = IsMemNodeEnabled;
  mMemRas->EnableErrorSignallingtoBIOS        = EnableErrorSignallingtoBIOS;
  mMemRas->ReEnablePatrolScrubEngine          = ReEnablePatrolScrubEngine;
  mMemRas->DisablePatrolScrubEngine           = DisablePatrolScrubEngine;
  mMemRas->SavePatrolScrubEngine              = SavePatrolScrubEngine;
  mMemRas->GetCurrentMemoryNodeState          = GetCurrentMemoryNodeState;
  mMemRas->InitializeRankSparing              = InitializeRankSparing;
  mMemRas->InitializeAdddcMR                  = InitializeAdddcMR;
  mMemRas->InitializeAdddcAfterMirror         = InitializeAdddcAfterMirror;
  mMemRas->InitializeSddcPlusOne              = InitializeSddcPlusOne;
  mMemRas->CheckAndHandleRankSparing          = CheckAndHandleRankSparing;
  mMemRas->CheckAndHandleAdddcSparing         = CheckAndHandleAdddcSparing;
  mMemRas->CheckAndHandleSddcPlusOneSparing   = CheckAndHandleSddcPlusOneSparing;
  mMemRas->ClearUnhandledCorrError            = ClearUnhandledCorrError;
  mMemRas->GetLastErrInfo                     = GetLastErrInfo;
  mMemRas->OpenPamRegion                      = OpenPamRegion;
  mMemRas->RestorePamRegion                   = RestorePamRegion;
  mMemRas->SystemAddressToDimmAddress         = SystemAddressToDimmAddress;
  mMemRas->DimmAddressToSystemAddress         = DimmAddressToSystemAddress;
  mMemRas->GetBitMapOfNodeWithErrors          = GetBitmapOfNodeWithErrors;
  mMemRas->GetBitMapOfNodeWithEventInProgress = GetBitMapOfNodeWithEventInProgress;

  mMemRas->GetSupportedMemRasModes            = GetSupportedMemRasModes;
  mMemRas->GetSpareInProgressState            = GetSpareInProgressState;
  mMemRas->ModifyRasFlag                      = ModifyRasFlag;
  mMemRas->MigrationPossible                  = MigrationPossible;
  mMemRas->SetupMemoryMigration               = SetupMemoryMigration;
  mMemRas->DisableMemoryMigration             = DisableMemoryMigration;
  mMemRas->InjectPoison                       = InjectPoison;
  mMemRas->MemoryNodeOnline                   = MemoryNodeOnline;
  mMemRas->MemoryNodeOffline                  = MemoryNodeOffline;
  mMemRas->GetErrorLogSysAddr                 = GetErrorLogSysAddr;
  mMemRas->GetLogicalToPhysicalRankId         = GetLogicalToPhysicalRankId;
  mMemRas->GetFailedDimmErrorInfo             = GetFailedDimmErrorInfo;
  Status = gSmst->SmmLocateProtocol (&gEfiQuiesceProtocolGuid, NULL, &mQuiesceSupport);
  ASSERT_EFI_ERROR (Status);


  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, &m2SmmCpuServiceProtocol);
  ASSERT_EFI_ERROR (Status);

  mMaxBusNumber = mIioUds->IioUdsPtr->PlatformData.MaxBusNumber;

  Status = gSmst->SmmLocateProtocol (&gEfiCrystalRidgeSmmGuid, NULL, &mCrystalRidgeProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Search for the Memory Map GUID HOB.  If it is not present, then there's nothing we can do. It may not exist on the update path.
  // Dont forget to update this structure after mem hotAdd/hotRemove/Migration otherwise PatrolScrub and MemCopy engine would be screwed
  //
  mSystemMemoryMap = NULL;
  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  mSystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);

  CopyMem(mMemRas->SystemMemInfo, mSystemMemoryMap, sizeof(struct SystemMemoryMapHob));

  //
  // mSystemMemoryMap is used by runtime RAS modules.  It is pointing to the HOB now.
  // Reassign mSystemMemoryMap to mMemRas->SystemMemInfo so it can be used at runtime
  //
  mSystemMemoryMap = mMemRas->SystemMemInfo;

  //
  // Update Host structure for using for Mem RAS driver
  // Need to bring struct sysHost to DXE SMM module!
  //
  ZeroMem (&mHost, sizeof (struct sysHost));
  mHost.var.common.cpuType = mIioUds->IioUdsPtr->SystemStatus.cpuType;
  mHost.var.common.SystemRasType = mIioUds->IioUdsPtr->SystemStatus.SystemRasType;
  mHost.var.common.stepping = mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping;
  mHost.var.common.socketPresentBitMap = mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap;
  mHost.var.common.mmCfgBase = (UINT32)mIioUds->IioUdsPtr->PlatformData.PciExpressBase;

  // Force all the branch enable for runtime online
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    mMemRas->SystemMemInfo->MemSetup.socket[Socket].enabled = 1;
  }

  mMemRasPrivate->Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
      &mMemRasPrivate->Handle,
      &gEfiMemRasProtocolGuid,
      EFI_NATIVE_INTERFACE,
      mMemRasPrivate->mMemRas
  );
  ASSERT_EFI_ERROR (Status);

  IniMemRasS3();

  //Initialize the EFI variable used to track if VLS was established
  InitializeVLSInfo();

  //
  // Initialize Sddc Sparing
  //
  InitializeSddcPlusOne();

  //
  //initialize spare rank
  //
  InitializeRankSparing();
  //
  // A0 stepping Si WA: 4929145
  // For SKX A0 (plan to fix on B0) - for >1S configs, do *not* enable ADDDC (RTL bug on bit location delta on Rd vs. Wr will cause false errors in >1S w/ ADDDC)
  // MCDECS_CRNODE_CHx_MCx_CR_ADDDC_REGIONx_CONTROL.region_enable should never be 1 (along w/ other ADDDC config) for >1S usage.
  //
  if (mHost.var.common.stepping < B0_REV_SKX) {
    //
    //Initialize adddc sparing
    //
    if (!(mHost.var.common.socketPresentBitMap > 0x1)){
      InitializeAdddcMR();
    }

  } else {
    InitializeAdddcMR();
  }
  //
  //initialize patrol scrub
  //
  InstallPatrolSmiHandler();


  return Status;
}
