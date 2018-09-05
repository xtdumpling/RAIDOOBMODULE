/** @file
  The PEI Library Implements ActiveManagement Support.

@copyright
  Copyright (c) 2015 -2016 Intel Corporation. All rights reserved
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

#include <Ppi/AmtPolicyPpi.h>
#include <Library/PeiAmtLib.h>
#include <MeBiosPayloadHob.h>
#include "PeiAlertStandardFormat.h"
#include "StartWatchDog.h"
#include <Library/HobLib.h>

/**
  This function performs basic initialization for AMT in PEI phase after DID.
**/
VOID
EFIAPI
AmtPostMemInit (
  VOID
  )
{
  ME_BIOS_PAYLOAD_HOB             *MbpHob;

  DEBUG ((DEBUG_INFO, "AmtPostMemInit() - Start\n"));

  MbpHob = (ME_BIOS_PAYLOAD_HOB *) GetFirstGuidHob (&gMeBiosPayloadHobGuid);

  DEBUG ((DEBUG_INFO, "IntelMeFwImageType = %x\n", MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType));
  if ((MbpHob != NULL) && (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == INTEL_ME_CORPORATE_FW)) {
    ///
    /// Performing Amt initialization from here.
    ///
    AlertStandardFormatInit ();
    
    ///
    /// Note: EfiPeiMasterBootModePpi must be installed prior to start watchdog timer.
    /// Start Watchdog timer when Boot mode is decided.
    ///
    PeiInitStartWatchDog ();
  }

  DEBUG ((DEBUG_INFO, "AmtPostMemInit() - End\n"));
}

