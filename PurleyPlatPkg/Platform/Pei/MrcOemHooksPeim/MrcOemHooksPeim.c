/** @file
  This file is for Intel MRC OEM HOOKS PPI initialization.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "MrcOemHooksPeim.h"

//
// This PPI must be a static global variable in a memory mapped location
// This is because it may be called from AP.
//
// The other thing to note is that these functions are leveraged directly
// from the existing OEM hooks library implementations of 
// OemProcMemInitLib and OemIioInitLib.  
//
static MRC_OEM_HOOKS_PPI                mMrcOemHooksPpi = {
  MRC_OEM_HOOKS_PPI_REVISION,
  OemInitializePlatformData,
  OemPreMemoryInit,
  OemPostMemoryInit,
  OemPreProcInit,
  OemPostProcInit,
  OemCheckForBoardVsCpuConflicts,
  OemCheckAndHandleResetRequests,
  MEBIOSCheckAndHandleResetRequests,
  OemPostCpuInit,
  OemPublishDataForPost,
  OemPlatformFatalError,
  OemTurnOffVrsForHedt,
  OemDetectPhysicalPresenceSSA,
  OemHookPreTopologyDiscovery,
  OemHookPostTopologyDiscovery,
  // APTIOV_SERVER_OVERRIDE_RC_START
  PlatformInitializeData, //DummyOemHooks,                       // PLATFORM_INITIALIZE_DATA
  // APTIOV_SERVER_OVERRIDE_RC_END
  PlatformMemInitGpio,
  PlatformInitGpio,
  PlatformReadGpio,
  PlatformWriteGpio,
  PlatformMemSelectSmbSeg,
  PlatformMemReadDimmVref,
  PlatformMemWriteDimmVref,
  PlatformSetVdd,
  PlatformReleaseADRClamps,
  PlatformDetectADR,
  PlatformSetErrorLEDs,
  OemUpdatePlatformConfig,
  OemInitThrottlingEarly,
  OemDetectDIMMConfig,
  OemInitDdrClocks,
  OemSetDDRFreq,
  OemConfigureXMP,
  OemCheckVdd,
  OemEarlyConfig,
  OemLateConfig,
  OemInitThrottling,
  PlatformCheckPORCompat,
  PlatformHookMst,
  PlatformDramMemoryTest,
  OemLookupDdr4OdtTable,
  OemLookupFreqTable,
  DummyOemHooks,                       // PLATFORM_EX_MEMORY_LINK_RESET                 
  PlatformVRsSVID,
  DummyOemHooks,                       // GET_MEM_SETUP_OPTIONS                         
  OemSendCompressedPacket,
  OemKtiGetEParams,
  OemKtiGetMmioh,
  OemKtiGetMmiol,
  OemCheckCpuPartsChangeSwap,
  OemGetAdaptedEqSettings,
  OemWaitTimeForPSBP,
  OemReadKtiNvram,
  UpdatePrevBootNGNDimmCfg,
  OemIssueReset,
  OemInitSerialDebug,
  PlatformCheckpoint,
  PlatformOutputWarning,
  PlatformOutputError,
  PlatformLogWarning,
  PlatformFatalError,
  PlatformEwlInit,
  PlatformMemInitWarning,
  PlatformEwlLogEntry,
  OemGetBoardTypeBitmask,
  OemGetResourceMapUpdate,
  ValidateCurrentConfigOemHook,
  PlatformReadSmb,
  PlatformWriteSmb,
  OemGetIioPlatformInfo,
  OemIioUplinkPortDetails,
  OemIioEarlyWorkAround,
  OemOverrideIioRxRecipeSettings,
  OemDefaultIioPortBifurationInfo,
  OemInitIioPortBifurcationInfo,
  OemDmiPreInit,
  OemDmiDeviceInit,
  OemDmiResourceAssignedInit,
  OemSetIioDefaultValues,
  OemSetPlatformDataValues,
  OemPublishOpaSocketMapHob,
  OemAfterAddressMapConfigured
};

static EFI_PEI_PPI_DESCRIPTOR       mInstallMrcOemHooksPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gMrcOemHooksPpiGuid,
  &mMrcOemHooksPpi
};


//
// Function implementations
//

/**
  MrcOemHooksPeim entry point
  
  This driver simply publishes a PPI that provides the OEM hooks for silicon
  initialization code.

  @param FileHandle         Pointer to the PEIM FFS file header.
  @param PeiServices        General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Operation completed successfully.
  @retval Otherwise         Mrc Plat Ppi Init failed.
**/
EFI_STATUS
EFIAPI
MrcOemHooksInitEntry (
  IN EFI_PEI_FILE_HANDLE          FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices
)
{
  EFI_STATUS                   Status;

  //
  //
  // Initialize platform PPIs
  //
  Status = PeiServicesInstallPpi (&mInstallMrcOemHooksPpi);
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}
