/** @file
  The PEI Library Implements ME Init.

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

#include <Library/HeciInitLib.h>
#include "MbpData.h"
#include <Library/DebugLib.h>
#include <MeChipset.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/MeSpsPolicyAccessLib.h>
#include <Ppi/MeSpsPolicyPei.h>
#include <MeBiosPayloadHob.h>
#include <Library/PeiAmtLib.h>
#include <MeFwHob.h>
#include <Library/MmPciBaseLib.h>
#include <Library/MeShowBufferLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PchPmcLib.h>
#include "IndustryStandard/Pci.h"
#include "Library/MeTypeLib.h"


/**
  Save FWSTS to ME FWSTS HOB, if the HOB is not existing, the HOB will be created and publish.
  If the HOB is existing, the data will be overrided.
**/
VOID
SaveFwStsToHob (
  VOID
  )
{
  ME_FW_HOB                       *MeFwHob;
  static UINT32                    DeviceList [] = {
    HECI1_DEVICE,
    HECI2_DEVICE,
    HECI3_DEVICE
  };
  static UINT32                    FwStsOffsetTable[] = {
    R_ME_HFS,
    R_ME_GS_SHDW,
    R_ME_HFS_3,
    R_ME_HFS_4,
    R_ME_HFS_5,
    R_ME_HFS_6
  };
  UINT8                           Index;
  UINT8                           Count;
  EFI_STATUS                      Status;

  if (PchIsDwrFlow() == TRUE) {
    DEBUG ((DEBUG_INFO, "DWR detected : ME FW HOB not installed\n"));
    return;
  }

  MeFwHob = GetFirstGuidHob (&gMeFwHobGuid);
  if (MeFwHob == NULL) {
    ///
    /// Create HOB for ME Data
    ///
    Status = PeiServicesCreateHob (
               EFI_HOB_TYPE_GUID_EXTENSION,
               sizeof (ME_FW_HOB),
               (VOID **) &MeFwHob
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return;
    }

    DEBUG_CODE (
      DEBUG ((DEBUG_INFO, "ME FW HOB installed\n"));
    );

    //
    // Initialize default HOB data
    //
    ZeroMem (&(MeFwHob->Revision), (sizeof (ME_FW_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
    MeFwHob->EfiHobGuidType.Name = gMeFwHobGuid;
    MeFwHob->Revision = 1;
    MeFwHob->Count = sizeof (DeviceList) / sizeof (UINT32);
  }

  DEBUG_CODE (
    DEBUG ((DEBUG_INFO, "ME FW HOB data updated\n"));
  );
  ///
  /// Save the FWSTS registers set for each MEI device.
  ///
  for (Count = 0; Count < (sizeof (DeviceList) / sizeof (UINT32)); Count++) {
    for (Index = 0; Index < (sizeof (FwStsOffsetTable) / sizeof (UINT32)); Index++) {
      MeFwHob->Group[Count].Reg[Index] =
        MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, DeviceList[Count]) + FwStsOffsetTable[Index]);
    }
    MeFwHob->Group[Count].FunNumber = DeviceList[Count];
  }

#ifndef MDEPKG_NDEBUG
  DEBUG ((DEBUG_INFO, "Current ME FW HOB data printed - \n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *)MeFwHob, sizeof (ME_FW_HOB));
  );
#endif
}

#if AMT_SUPPORT


/**
  ME End of PEI callback function. Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei(VOID)
{
  //
  // Set D0I3 bits if resuming from S3
  //
  if (GetBootModeHob () == BOOT_ON_S3_RESUME) {


    DEBUG ((DEBUG_INFO, "Setting D0I3 bits for HECI devices on S3 resume path\n"));
    SetD0I3Bit (HECI1);
//    SetD0I3Bit (HECI2);
//    SetD0I3Bit (HECI3);
  }

  return;
}

/**
  This function performs basic initialization for ME in PEI phase after Policy produced.
**/
VOID
EFIAPI
MePostMemInitAmt (
   VOID
  )
{
  EFI_STATUS                 Status;
  HECI_PPI                   *HeciPpi;
  ME_BIOS_PAYLOAD_HOB        *MbpHob;
  BOOLEAN                    IntegratedTouchEnabled;
  DEBUG ((DEBUG_INFO, "MePostMemInitAmt() - Start\n"));
  if (!MeTypeIsAmt()) {
    return;
  }

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,         // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return ;
  }

  IntegratedTouchEnabled = FALSE;

  ///
  /// Install Mbp in POST Mem unless it's S3 resume boot. If MbpHob exists, reflect IntegratedTouch state
  /// from Mbp directly. Otherwise, refer to MePolicy and Me Bios boot path to disable/enable the support.
  ///
  MbpHob = NULL;

  if (GetBootModeHob () != BOOT_ON_S3_RESUME) {
    InstallMbp (&MbpHob);
    if ((MbpHob != NULL) && (MbpHob->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntegratedTouch == 1)) {
      IntegratedTouchEnabled = TRUE;
    }
  }

  // On server side we have no IntegratedTouch support

  DEBUG ((DEBUG_INFO, "MePostMemInitAmt() - End\n"));

}

/**
  Check ME Boot path. The function provides ME BIOS boot path required based on current
  HECI1 FW Status Register (MEI1 HFSTS1 used in BWG, R_FWSTATE used in code definition).
  HECI1 must be enabled before invoke the function. The FW registers may report multiple
  status to reflect Me Bios boot path, BIOS will follow the prioritized Me Bios boot path to
  continue boot. If the priority will be changed, then BOOT_PATH enumerated type shall be
  adjusted as well to reflect real priority.

  @param[out] MeBiosPath          Pointer for ME BIOS boot path report

  @retval EFI_SUCCESS             MeBiosPath copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeBiosPath is invalid
**/
EFI_STATUS
CheckMeBootPath (
  OUT ME_BIOS_BOOT_PATH           *MeBiosPath
  )
{

  HECI_FWS_REGISTER     Hfsts1;
  HECI_FW_STS4_REGISTER Hfsts4;

  if (MeBiosPath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Hfsts1.ul = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFS);
  Hfsts4.ul = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFS_4);

  DEBUG ((DEBUG_INFO, "HECI1 Hfsts1 = %X, Hfsts4 = %x \n", Hfsts1.ul, Hfsts4.ul));

  *MeBiosPath = MeNormalBiosPath;

  if (Hfsts1.r.ErrorCode == ME_ERROR_CODE_NO_ERROR) {
    if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SPS) {

      *MeBiosPath = (Hfsts1.r.CurrentState > ME_STATE_RECOVERY) ? MeSpsOprBiosPath : MeSpsRcvBiosPath;

    } else if (Hfsts1.r.CurrentState == ME_STATE_NORMAL) {

      if (Hfsts1.r.FwUpdateInprogress == 0x01) {
        *MeBiosPath = (*MeBiosPath > MeFwUpdateBiosPath) ? *MeBiosPath : MeFwUpdateBiosPath;
      }
    }
  }
  if (Hfsts1.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE) {
    *MeBiosPath = (*MeBiosPath > MeErrorWithoutDidMsgBiosPath) ? *MeBiosPath : MeErrorWithoutDidMsgBiosPath;
  } else if (Hfsts1.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) {
    *MeBiosPath = (*MeBiosPath > MeErrorBiosPath) ? *MeBiosPath : MeErrorBiosPath;
  }

  if (Hfsts1.r.FptBad == 0x01) {
    *MeBiosPath = (*MeBiosPath > MeErrorBiosPath) ? *MeBiosPath : MeErrorBiosPath;
  }

  if (Hfsts1.r.CurrentState == ME_STATE_RECOVERY) {
    *MeBiosPath = (*MeBiosPath > MeRecoveryBiosPath) ? *MeBiosPath : MeRecoveryBiosPath;
  }

  if (Hfsts4.r.FwInEnfFlow == 0x01) {
    *MeBiosPath = (*MeBiosPath > MeEnforcementWithoutDidMsgBiosPath) ? *MeBiosPath : MeEnforcementWithoutDidMsgBiosPath;
  }

  if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_DEBUG) {
    *MeBiosPath = (*MeBiosPath > MeDebugModeBiosPath) ? *MeBiosPath : MeDebugModeBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE) {
    *MeBiosPath = (*MeBiosPath > MeSwTempDisableBiosPath) ? *MeBiosPath : MeSwTempDisableBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR) {
    *MeBiosPath = (*MeBiosPath > MeSecoverJmprBiosPath) ? *MeBiosPath : MeSecoverJmprBiosPath;
  } else if (Hfsts1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_HECI_MSG) {
    *MeBiosPath = (*MeBiosPath > MeSecoverMeiMsgBiosPath) ? *MeBiosPath : MeSecoverMeiMsgBiosPath;
  }

  if (*MeBiosPath != MeNormalBiosPath && *MeBiosPath != MeSpsOprBiosPath) {
    DEBUG ((DEBUG_ERROR, "CheckMeBootPath () - ME Bios Boot Path is %r\n", *MeBiosPath));
  }

  return EFI_SUCCESS;
}
#endif // AMT_SUPPORT