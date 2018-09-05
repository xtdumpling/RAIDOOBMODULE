/** @file
  This files contains Pch Vtd services for RCs usage

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
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

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <PchAccess.h>
#include <Library/PchPcrLib.h>

/**
  Enable VTd support in PSF.
  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchPsfEnableVtd (
  VOID
  )
{
  UINT32 PsfPid;
  for (PsfPid = PID_PSF1; PsfPid <= PID_PSF4; ++PsfPid) {
    PchPcrAndThenOr32 (
      PsfPid,
      R_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0,
      ~0u,
      B_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0_ENADDRP2P | B_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0_VTDEN
      );
  }
  return EFI_SUCCESS;
}

