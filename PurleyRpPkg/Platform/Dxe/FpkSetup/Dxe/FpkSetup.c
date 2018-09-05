/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/UefiLib.h>
#include <Protocol/AdapterInformation.h>

#include <Protocol/PchReset.h>

#include <Library/UbaFpkConfigLib.h>

#include <Library/PcdLib.h>
#include <Library/SetupLib.h>

#include "FpkCommon.h"
#include "FpkStraps.h"
#include "FpkPortConfig.h"

static VOID                       *mAipRegistration;
static BOOLEAN                    mFpkSetupDone;
static PLATFORM_FPK_CONFIG_STRUCT mPlatformFpkConfigStruct;

/**
  Checks if specified EFI_ADAPTER_INFORMATION_PROTOCOL supports
  EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID information type.

  @param[in]  Aip   EFI_ADAPTER_INFORMATION_PROTOCOL to exercise.

  @retval     TRUE  Specified EFI_ADAPTER_INFORMATION_PROTOCOL supports
                    EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID information type.
  @retval     FALSE Specified EFI_ADAPTER_INFORMATION_PROTOCOL does not
                    support EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID information
                    type or error occurred.

**/
static
BOOLEAN
AipFpkTypeSupported (
  IN EFI_ADAPTER_INFORMATION_PROTOCOL *Aip
  )
{
  EFI_STATUS Status;
  EFI_GUID   *InfoTypesBuffer;
  UINTN      InfoTypesBufferCount;
  UINTN      InfoTypesIndex;
  EFI_GUID   AipFpkFuncEnDisGuid = EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID;

  Status = Aip->GetSupportedTypes (
                  Aip,
                  &InfoTypesBuffer,
                  &InfoTypesBufferCount
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  for (InfoTypesIndex = 0; InfoTypesIndex < InfoTypesBufferCount; InfoTypesIndex++) {
    if (CompareGuid (&InfoTypesBuffer[InfoTypesIndex], &AipFpkFuncEnDisGuid)) {
      break;
    }
  }
  FreePool (InfoTypesBuffer);
  if (InfoTypesIndex == InfoTypesBufferCount) {
    return FALSE;
  }
  return TRUE;
}

/**
  Performs conditional warm reset of the platform.

  @param  DoReset  TRUE: perform reset, FALSE: do nothing.

**/
static
VOID
FpkCondReset (
  BOOLEAN DoReset
  )
{
  if (DoReset) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkCondReset: resetting the system...\n"));
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    ASSERT(FALSE);
  }
}


/**
  Main FPK setup function.

  Called from EFI_ADAPTER_INFORMATION_PROTOCOL notification context.
**/
static
VOID
FpkSetupMain (
  IN EFI_ADAPTER_INFORMATION_PROTOCOL *Aip
)
{
  EFI_STATUS                        Status;
  EFI_GUID                          AipFpkFuncEnDisGuid = EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID;
  EFI_ADAPTER_INFO_FPK_FUNC_DISABLE *FpkFuncDisInNvm;
  UINTN                             FpkFuncDisInNvmSize;
  EFI_ADAPTER_INFO_FPK_FUNC_DISABLE FpkFuncDisInSetup;
  PCH_RESET_PROTOCOL                *PchResetProtocol;
  BOOLEAN                           ResetRequired = FALSE;
  PCH_RESET_TYPE                    PchResetType = EfiResetWarm;
  BOOLEAN                           FuncDisEqual;
  UINT8                             FpkPortConfig[FPK_NUM_SUPPORTED_PORTS];
  UINT8                             FpkPortConfigPrev[FPK_NUM_SUPPORTED_PORTS];

  DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: mFpkSetupDone: %d\n", mFpkSetupDone));
  if (mFpkSetupDone == TRUE) {
    return;
  }

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfig), FpkPortConfig, sizeof (FpkPortConfig));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: GetOptionData failed: %r\n", Status));
    return;
  }
  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfigPrev), FpkPortConfigPrev, sizeof (FpkPortConfigPrev));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: GetOptionData failed: %r\n", Status));
    return;
  }

  if (FpkDoesPortConfigEnableAllFunctionsAndPorts (FpkPortConfig)) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: target config enables all functions, nothing to do.\n"));
    mFpkSetupDone = TRUE;
    return;
  }

  DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: target config disables some functions...\n"));

  if (FpkCheckAndSetStraps (0, &ResetRequired) == FALSE) {
    return;
  }
  Status = Aip->GetInformation (Aip, &AipFpkFuncEnDisGuid, &FpkFuncDisInNvm, &FpkFuncDisInNvmSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: EFI_ADAPTER_INFORMATION_PROTOCOL->GetInformation failed: %r\n", Status));
    return;
  }
  if (FpkFuncDisInNvmSize != sizeof (FpkFuncDisInSetup)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: FpkFuncDisInNvmSize (%d) != sizeof (FpkFuncDisInSetup) (%d)\n",
                         FpkFuncDisInNvmSize,
                         sizeof (FpkFuncDisInSetup)
                         ));
    FreePool (FpkFuncDisInNvm);
    return;
  }
  FpkConvertPortConfigToFpkFuncDis (FpkPortConfig, &FpkFuncDisInSetup);
  DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: NVM port configuration (P0, P1, P2, P3): 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                       FpkFuncDisInNvm->AllowFuncDis[0],
                       FpkFuncDisInNvm->AllowFuncDis[1],
                       FpkFuncDisInNvm->AllowFuncDis[2],
                       FpkFuncDisInNvm->AllowFuncDis[3]));
  DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: Setup port configuration (P0, P1, P2, P3): 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                       FpkFuncDisInSetup.AllowFuncDis[0],
                       FpkFuncDisInSetup.AllowFuncDis[1],
                       FpkFuncDisInSetup.AllowFuncDis[2],
                       FpkFuncDisInSetup.AllowFuncDis[3]));
  FuncDisEqual = FpkAreFuncDisEqual (FpkFuncDisInNvm, &FpkFuncDisInSetup);
  FreePool (FpkFuncDisInNvm);
  if (FuncDisEqual == FALSE) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: NVM configuration change requested\n"));
    Status = Aip->SetInformation (Aip, &AipFpkFuncEnDisGuid, &FpkFuncDisInSetup, sizeof (FpkFuncDisInSetup));
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: EFI_ADAPTER_INFORMATION_PROTOCOL->SetInformation failed: %r\n", Status));
      return;
    }
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: NVM configuration change succeeded\n"));
    ResetRequired = TRUE;
    PchResetType = GlobalReset;
  }
  FpkPortConfigCopy(FpkPortConfig, FpkPortConfigPrev);

  Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfigPrev), FpkPortConfigPrev, sizeof (FpkPortConfigPrev));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupMain: ERROR: SetOptionData failed: %r\n", Status));
    return;
  }
  if (ResetRequired) {
    Status = gBS->LocateProtocol (&gPchResetProtocolGuid, NULL, &PchResetProtocol);
    if (Status == EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupMain: resetting the system... (type %d)\n", PchResetType));
      PchResetProtocol->Reset (PchResetProtocol, PchResetType, 0, NULL);
    }
    ASSERT(FALSE);
  }
  mFpkSetupDone = TRUE;
}

/**
  EFI_ADAPTER_INFORMATION_PROTOCOL notification callback.

  Tests if notified protocols support expected information type and, if so, calls FpkSetupMain.

  @param      Event    EFI_EVENT.
  @param[in]  Context  Notification context.

**/
static
VOID
EFIAPI
FpkAipCallback (
     EFI_EVENT Event,
  IN VOID      *Context
)
{
  EFI_ADAPTER_INFORMATION_PROTOCOL *Aip;
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  EFI_HANDLE                       Handle;
  UINT32                           SetupCount = 0;

  DEBUG ((DEBUG_INFO, "FpkSetup: FpkAipCallback: mAipRegistration: %llx\n", (unsigned long long) mAipRegistration));
  while (TRUE) {
    BufferSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (
                    ByRegisterNotify,
                    NULL,
                    mAipRegistration,
                    &BufferSize,
                    &Handle
                    );
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiAdapterInformationProtocolGuid,
                    (VOID **) &Aip
                    );
    ASSERT_EFI_ERROR (Status);
    if (AipFpkTypeSupported (Aip)) {
      FpkSetupMain (Aip);
      SetupCount++;
    }
  }
  DEBUG ((DEBUG_INFO, "FpkSetup: FpkAipCallback: SetupCount: %d\n", SetupCount));
}

/**
  Adjusts SYSTEM_CONFIGURATION to reflect mapping/presence of ports delivered using UBA.

  Data are subsequently used by Setup to hide ports that are not present.
  This function does nothing if no change is required.

**/
static
VOID
FpkShowMappedPorts (
  VOID
)
{
  EFI_STATUS         Status;
  UINT8              Index;
  BOOLEAN            Changes = FALSE;
  UINT8              FpkPortPresent[FPK_NUM_SUPPORTED_PORTS];

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortPresent), FpkPortPresent, sizeof (FpkPortPresent));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkShowMappedPorts: ERROR: GetOptionData failed: %r\n", Status));
    return;
  }

  for (Index = 0; Index < FPK_NUM_SUPPORTED_PORTS; Index++) {
    if (FpkIsPortMapped (Index) != FpkPortPresent[Index]) {
      FpkPortPresent[Index] = FpkIsPortMapped (Index);
      DEBUG ((DEBUG_INFO, "FpkSetup: FpkShowMappedPorts: port %d: showing: %d\n", Index, FpkPortPresent[Index]));
      Changes = TRUE;
    }
  }
  if (Changes) {
    Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortPresent), FpkPortPresent, sizeof (FpkPortPresent));
  }
}

/**
  Driver's entry point.

  Retrieves data from UBA database and configures the driver accordingly.
  Performs sanity check of GPIOs that drive FPK straps.
  Handles cases that require straps adjustment outside EFI_ADAPTER_INFORMATION_PROTOCOL notification.
  Registers notification on gEfiAdapterInformationProtocolGuid.

**/
EFI_STATUS
EFIAPI
FpkSetupEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
)
{
  EFI_STATUS                    Status;
  EFI_EVENT			Event = NULL;
  BOOLEAN                       ResetRequired = FALSE;
  UINT8                         FpkPortConfig[FPK_NUM_SUPPORTED_PORTS];
  UINT8                         FpkPortConfigPrev[FPK_NUM_SUPPORTED_PORTS];

  Status = FpkConfigGetConfigStruct (&mPlatformFpkConfigStruct);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupEntryPoint: FpkConfigGetConfigStruct failed: %r\n", Status));
    return EFI_ABORTED;
  }
  FpkSelectStrapPads (mPlatformFpkConfigStruct.PciDisNGpioPad, mPlatformFpkConfigStruct.LanDisNGpioPad);
  FpkSetPortToFuncMap (mPlatformFpkConfigStruct.PortToFuncMapPtr, (UINT8) (mPlatformFpkConfigStruct.PortToFuncMapSize / sizeof (*mPlatformFpkConfigStruct.PortToFuncMapPtr)));

  FpkShowMappedPorts ();

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfig), FpkPortConfig, sizeof (FpkPortConfig));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupEntryPoint: ERROR: GetOptionData failed: %r\n", Status));
    return EFI_ABORTED;
  }

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfigPrev), FpkPortConfigPrev, sizeof (FpkPortConfigPrev));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupEntryPoint: ERROR: GetOptionData failed: %r\n", Status));
    return EFI_ABORTED;
  }

  if (FpkSanityCheckStraps () == FALSE) {
    DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupEntryPoint: ERROR: strap sanity check failed\n"));
    return EFI_ABORTED;
  }

  // check and adjust straps if configuration change requested and new configuration enables all functions
  if (FpkDoesPortConfigEnableAllFunctionsAndPorts (FpkPortConfig) &&
      FpkArePortConfigsEqual (FpkPortConfigPrev, FpkPortConfig) == FALSE) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupEntryPoint: configuration changed, new configuration enables all functions, checking straps...\n"));
    if (FpkCheckAndSetStraps (1, &ResetRequired) == FALSE) {
      return EFI_ABORTED;
    }
    FpkPortConfigCopy (FpkPortConfig, FpkPortConfigPrev);

    Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, FpkPortConfigPrev), FpkPortConfigPrev, sizeof (FpkPortConfigPrev));

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "FpkSetup: FpkSetupEntryPoint: ERROR: SetOptionData failed: %r\n", Status));
      return EFI_ABORTED;
    }
    FpkCondReset (ResetRequired);
  }

  // check and adjust straps if configuration change requested and current NVM configuration disables all functions
  if (FpkDoesPortConfigDisableAllFunctions (FpkPortConfigPrev) &&
      FpkArePortConfigsEqual (FpkPortConfigPrev, FpkPortConfig) == FALSE) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupEntryPoint: configuration changed, all functions disabled, checking straps...\n"));
    if (FpkCheckAndSetStraps (1, &ResetRequired) == FALSE) {
      return EFI_ABORTED;
    }
    FpkCondReset (ResetRequired);
  }

  // check and adjust straps if configuration disables any functions and hasn't changed (e.g. A/C cycle)
  if (FpkDoesPortConfigDisableAnyFunctionOrPort (FpkPortConfigPrev) &&
      FpkArePortConfigsEqual (FpkPortConfigPrev, FpkPortConfig) == TRUE) {
    DEBUG ((DEBUG_INFO, "FpkSetup: FpkSetupEntryPoint: configuration unchanged, some functions disabled, checking straps...\n"));
    if (FpkCheckAndSetStraps (0, &ResetRequired) == FALSE) {
      return EFI_ABORTED;
    }
    FpkCondReset (ResetRequired);
  }

  Event = EfiCreateProtocolNotifyEvent(&gEfiAdapterInformationProtocolGuid, TPL_CALLBACK, FpkAipCallback, NULL, &mAipRegistration);
  if (Event == NULL) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}
