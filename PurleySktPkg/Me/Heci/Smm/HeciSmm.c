/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciSmm.c

  SMM HECI driver

**/

#include "Library/HeciCoreLib.h"
#include <Protocol/HeciSmm.h>

#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>


#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmBase2.h>
#include <IndustryStandard/Pci22.h>
#include "Protocol/HeciProtocol.h"

#include <PchAccess.h>
#include <HeciRegs.h>
#include <Library/MeTypeLib.h>

HECI_PROTOCOL           *HeciCtlr;

STATIC EFI_STATUS
SmmUnsupported(VOID)
{
  return EFI_UNSUPPORTED;
}


/**

    GC_TODO: add routine description

    @param ImageHandle - GC_TODO: add arg description
    @param SystemTable - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
HeciSmmEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
)
{
  EFI_STATUS                Status;
  BOOLEAN                   InSmm;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase;

  if (MeTypeIsDfx() || MeTypeIsDisabled()) {
    // Dfx firmware doesn't need Smm Heci driver
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &SmmBase);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmBase->InSmm (SmmBase, &InSmm);
  if (InSmm) {
    //
    // In SMM
    //
    EFI_SMM_SYSTEM_TABLE2     *gSmst;
    EFI_HANDLE                Handle;

    //
    // Create database record and add to database
    //
    Status = SmmBase->GetSmstLocation (SmmBase, &gSmst);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (HECI_PROTOCOL), (VOID *)&HeciCtlr);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Initialize HECI protocol pointers
    //
    HeciCtlr->ResetHeci    = HeciResetInterface;
    HeciCtlr->SendwACK     = HeciSendwAck;
    HeciCtlr->ReadMsg      = HeciReceive;
    HeciCtlr->SendMsg      = HeciSend;
    HeciCtlr->InitHeci     = HeciInitialize;
    HeciCtlr->ReInitHeci   = HeciReInitialize;
    HeciCtlr->MeResetWait  = HeciMeResetWait;
    HeciCtlr->GetMeStatus  = HeciGetMeStatus;
    HeciCtlr->GetMeMode    = HeciGetMeMode;

    Handle = NULL;

    //
    // Install the HECI interface
    //
    Status = gSmst->SmmInstallProtocolInterface (
                  &Handle,
                  &gSmmHeciProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  HeciCtlr
                 );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      gSmst->SmmFreePool(HeciCtlr);
      HeciCtlr = NULL;
      return Status;
    }
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}
