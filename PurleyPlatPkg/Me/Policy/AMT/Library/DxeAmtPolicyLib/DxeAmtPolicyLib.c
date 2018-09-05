/** @file
  This file is DxeAmtPolicyLib library.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include "DxeAmtPolicyLibrary.h"
#include "Library/MeTypeLib.h"

GLOBAL_REMOVE_IF_UNREFERENCED UINT64                  mDefaultPciDeviceFilterOutTable[]  = { MAX_ADDRESS };

//
// Global variables
//

/**
  This function prints the AMT DXE phase policy.

  @param[in] DxeAmtPolicy - AMT DXE Policy protocol
**/
VOID
AmtPrintPolicyProtocol (
  IN  AMT_POLICY_PROTOCOL     *DxeAmtPolicy
  )
{
DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "\n------------------------ AmtPolicy Print Begin -----------------\n"));
  DEBUG ((DEBUG_INFO, " Revision : 0x%x\n", DxeAmtPolicy->Revision));
  ASSERT (DxeAmtPolicy->Revision == AMT_POLICY_PROTOCOL_REVISION);

  DEBUG ((DEBUG_INFO, "AmtConfig ---\n"));
  ///
  /// Byte 0, bit definition for functionality enable/disable
  ///
  DEBUG ((DEBUG_INFO, " AsfEnabled : 0x%x\n", DxeAmtPolicy->AmtConfig.AsfEnabled));
  DEBUG ((DEBUG_INFO, " AmtEnabled : 0x%x\n", DxeAmtPolicy->AmtConfig.AmtEnabled));
  DEBUG ((DEBUG_INFO, " AmtbxPasswordWrite : 0x%x\n", DxeAmtPolicy->AmtConfig.AmtbxPasswordWrite));
  DEBUG ((DEBUG_INFO, " WatchDog : 0x%x\n", DxeAmtPolicy->AmtConfig.WatchDog));
  DEBUG ((DEBUG_INFO, " CiraRequest : 0x%x\n", DxeAmtPolicy->AmtConfig.CiraRequest));
  DEBUG ((DEBUG_INFO, " ManageabilityMode : 0x%x\n", DxeAmtPolicy->AmtConfig.ManageabilityMode));
  DEBUG ((DEBUG_INFO, " UnConfigureMe : 0x%x\n", DxeAmtPolicy->AmtConfig.UnConfigureMe));
  DEBUG ((DEBUG_INFO, " MebxDebugMsg : 0x%x\n", DxeAmtPolicy->AmtConfig.MebxDebugMsg));

  ///
  /// Byte 1, bit definition for functionality enable/disable
  ///
  DEBUG ((DEBUG_INFO, " ForcMebxSyncUp : 0x%x\n", DxeAmtPolicy->AmtConfig.ForcMebxSyncUp));
  DEBUG ((DEBUG_INFO, " UsbrEnabled : 0x%x\n", DxeAmtPolicy->AmtConfig.UsbrEnabled));
  DEBUG ((DEBUG_INFO, " UsbLockingEnabled : 0x%x\n", DxeAmtPolicy->AmtConfig.UsbLockingEnabled));
  DEBUG ((DEBUG_INFO, " HideUnConfigureMeConfirm : 0x%x\n", DxeAmtPolicy->AmtConfig.HideUnConfigureMeConfirm));
  DEBUG ((DEBUG_INFO, " USBProvision : 0x%x\n", DxeAmtPolicy->AmtConfig.USBProvision));
  DEBUG ((DEBUG_INFO, " FWProgress : 0x%x\n", DxeAmtPolicy->AmtConfig.FWProgress));
  DEBUG ((DEBUG_INFO, " AmtbxHotKeyPressed : 0x%x\n", DxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed));
  DEBUG ((DEBUG_INFO, " AmtbxSelectionScreen  : 0x%x\n", DxeAmtPolicy->AmtConfig.AmtbxSelectionScreen));

  ///
  /// Byte 3-4 OS WatchDog Timer
  ///
  DEBUG ((DEBUG_INFO, " WatchDogTimerOs : 0x%x\n", DxeAmtPolicy->AmtConfig.WatchDogTimerOs));

  ///
  /// Byte 5-6 BIOS WatchDog Timer
  ///
  DEBUG ((DEBUG_INFO, " WatchDogTimerBios : 0x%x\n", DxeAmtPolicy->AmtConfig.WatchDogTimerBios));

  ///
  /// Byte 7 CIRA Timeout, Client Initiated Remote Access Timeout
  ///             OEM defined timeout for MPS connection to be established.
  ///
  DEBUG ((DEBUG_INFO, " CiraTimeout : 0x%x\n", DxeAmtPolicy->AmtConfig.CiraTimeout));

  ///
  /// Byte 8 CPU Replacement Timeout
  ///
  DEBUG ((DEBUG_INFO, " CPU Replacement Timeout : 0x%x\n", DxeAmtPolicy->AmtConfig.CpuReplacementTimeout));

  ///
  /// Byte 9-10 OemResolutionSettings
  ///
  DEBUG ((DEBUG_INFO, " MebxNonUiTextMode : 0x%x\n", DxeAmtPolicy->AmtConfig.MebxNonUiTextMode));
  DEBUG ((DEBUG_INFO, " MebxUiTextMode : 0x%x\n", DxeAmtPolicy->AmtConfig.MebxUiTextMode));
  DEBUG ((DEBUG_INFO, " MebxGraphicsMode : 0x%x\n", DxeAmtPolicy->AmtConfig.MebxGraphicsMode));

  ///
  /// Byte 11-14 Pointer to a list which contain on-board devices bus/device/fun number
  ///
  DEBUG ((DEBUG_INFO, " PciDeviceFilterOutTable Pointer : 0x%x\n", DxeAmtPolicy->AmtConfig.PciDeviceFilterOutTable));

  DEBUG ((DEBUG_INFO, "\n------------------------ AmtPolicy Print End -------------------\n"));
DEBUG_CODE_END();
}

/**
  AmtCreatePolicyDefaults creates the default setting of AMT Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] DxeMePolicy               The pointer to get Amt Policy protocol instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_UNSUPPORTED               The current SKU doesn't support AMT feature

**/
EFI_STATUS
EFIAPI
AmtCreatePolicyDefaults (
  IN OUT  AMT_POLICY_PROTOCOL       *DxeAmtPolicy
  )
{
  EFI_STATUS                      Status;
  UINTN                           VariableSize;
  ME_BIOS_EXTENSION_SETUP         MeBiosExtensionSetupData;

  ME_BIOS_PAYLOAD_HOB             *MbpHob;

  MbpHob = NULL;

  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    DEBUG ((DEBUG_ERROR, "No MBP Data Protocol available\n"));
  }

  // For other MEs initialize defaults
  if (MbpHob != NULL)
  if (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType != INTEL_ME_CORPORATE_FW) {
    DEBUG ((DEBUG_INFO, "Current ME FW is not Corporate SKU, Amt Policy protocol is not installed then.\n"));
    return EFI_UNSUPPORTED;
  }

  ZeroMem (DxeAmtPolicy, sizeof (AMT_POLICY_PROTOCOL));
  ///
  /// AMT DXE Policy Init
  ///
  DxeAmtPolicy->Revision = AMT_POLICY_PROTOCOL_REVISION;

  ///
  /// Initialzie the Me Configuration
  ///
  DxeAmtPolicy->AmtConfig.AsfEnabled                = 1;
  DxeAmtPolicy->AmtConfig.AmtEnabled                = 1;
  DxeAmtPolicy->AmtConfig.FWProgress                = 1;

  ///
  /// MEBX_GRAPHICS_AUTO
  ///
  DxeAmtPolicy->AmtConfig.PciDeviceFilterOutTable = (UINT32) (UINTN) mDefaultPciDeviceFilterOutTable;

  ///
  /// Please don't change the default value of ForcMebxSyncUp and
  /// This let customer to force MEBX execution if they need and
  ///
  DxeAmtPolicy->AmtConfig.ForcMebxSyncUp = 0;

  ///
  /// Get BIOS Sync-up data from MEBx to Amt Policy
  ///
  VariableSize = sizeof (MeBiosExtensionSetupData);
  Status = gRT->GetVariable (
                  ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                  &gMeBiosExtensionSetupGuid,
                  NULL,
                  &VariableSize,
                  &MeBiosExtensionSetupData
                  );
  if (EFI_ERROR (Status) || (MeBiosExtensionSetupData.InterfaceVersion == 0)) {
    DxeAmtPolicy->AmtConfig.ManageabilityMode = 0;
    ///
    /// Check if this is first boot after update BIOS or the MebxSetupData variable is destroyed.
    /// In that case, we can set the force bit to synch up data with Mebx. For the normal case,
    /// ForcMebxSyncUp is still updated per customization as above comment.
    ///
    DxeAmtPolicy->AmtConfig.ForcMebxSyncUp    = 1;
  } else {
    DxeAmtPolicy->AmtConfig.ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
  }

  return EFI_SUCCESS;
}

/**
  AmtInstallPolicyProtocol installs ME Policy.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] ImageHandle                Image handle of this driver.
  @param[in] DxeAmtPolicy               The pointer to Amt Policy Protocol instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
AmtInstallPolicyProtocol (
  IN  EFI_HANDLE                        ImageHandle,
  IN  AMT_POLICY_PROTOCOL               *DxeAmtPolicy
  )
{
  EFI_STATUS            Status;

  ///
  /// Print Amt DXE Policy
  ///

  ///
  /// Install protocol to to allow access to this Policy.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gDxeAmtPolicyGuid,
                  DxeAmtPolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

