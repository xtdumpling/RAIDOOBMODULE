/**@file
  ICC is divided into 4 modules: 
  - IccOverClocking: the overclocking library, distributed as binary,
  - IccSetup: code linked with setup browser; supports overclocking and uses overclocking lib
  - IccPlatform: not linked with setup, supports all Icc features except overclocking; does not use overclocking lib
  No part of ICC belongs to BIOS Reference Code.

  IccPlatform disables unused PCI/PCIe clocks and transfers data from MBP to Icc Setup

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
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

#include <Base.h>
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PchAccess.h>
#include <Library/DxeMeLib.h>
#include "IccPlatform.h"
#include <Library/PchInfoLib.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>

#include "Library/DxeMeLib.h"
#include <Library/MemoryAllocationLib.h>
#include "Protocol/IccDataProtocol.h"
#include "Library/MeTypeLib.h"

ICC_DATA_PROTOCOL *mIccDataProtocol = NULL;
EFI_STATUS
ReadMainSetupData (
                 OUT ICC_MBP_DATA  *IccMbpDataOut
  );

EFI_STATUS GetMbpData(ICC_MBP_DATA  *IccMbpData)
{
  return ReadMainSetupData(IccMbpData);
}

EFI_STATUS
EFIAPI
IccPlatformEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS    Status;
  DEBUG ((EFI_D_INFO, "(ICC) Entry Point to ICC_Platform\n"));

  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }

  Status = ReadMainSetupData (NULL);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "(ICC) Failed to write VOLATILE configuration!  Status = %r\n", Status));
    //
    // Install gIccDataProtocol
    //
    mIccDataProtocol  =   AllocateZeroPool (sizeof (ICC_DATA_PROTOCOL));
    if (mIccDataProtocol != NULL) {
      mIccDataProtocol->GetMbpData = GetMbpData;
      Status = gBS->InstallMultipleProtocolInterfaces (&ImageHandle,
                                                       &gIccDataProtocolGuid,
                                                       mIccDataProtocol,
                                                       NULL
                                                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "(ICC) Failed to install gIccDataProtocol, Status = %r\n", Status));
        FreePool(mIccDataProtocol);
        mIccDataProtocol = NULL;
      }
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "(ICC) Failed to read setup data! Status = %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ReadMainSetupData (
                 OUT ICC_MBP_DATA  *IccMbpDataOut
  )
/*++
Routine Description:
  reads SETUP_DATA and creates IccConfig with all Icc-related setup informations
Arguments:
Returns:
  filled SETUP_DATA struct
--*/
{
  EFI_STATUS              Status;
  ICC_MBP_DATA            IccMbpData = {0};
  ME_BIOS_PAYLOAD_HOB     *MbpHob;

  MbpHob = NULL;

  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    DEBUG ((DEBUG_ERROR, "(ICC) Failed to read Mbp data!\n"));
    return EFI_NOT_FOUND;
  }
  IccMbpData.Profile = (UINT8)MbpHob->MeBiosPayload.IccProfile.IccProfileIndex.IccProfileId;

  if ( MbpHob->MeBiosPayload.IccProfile.IccProfileChangeable) {
    IccMbpData.ProfileSelectionAllowed = DISPLAY;
  } else {
    IccMbpData.ProfileSelectionAllowed = DONT_DISPLAY;
  }

  if ( MbpHob->MeBiosPayload.IccProfile.IccNumOfProfiles == 0){
    // Firmware is unconfgured, and there are no profiles to select
    IccMbpData.ProfileSelectionAllowed = DONT_DISPLAY;
  }

  IccMbpData.NumProfiles = MbpHob->MeBiosPayload.IccProfile.IccNumOfProfiles;
  IccMbpData.RegLock = MbpHob->MeBiosPayload.IccProfile.IccLockMaskType;

  Status = gRT->SetVariable (
                  ICC_MBP_DATA_NAME, 
                  &gIccGuid, 
                  EFI_VARIABLE_BOOTSERVICE_ACCESS, 
                  sizeof(IccMbpData), 
                  &IccMbpData
                  );
  return Status;
}
