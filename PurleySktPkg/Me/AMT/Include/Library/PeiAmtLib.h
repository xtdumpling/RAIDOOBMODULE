/** @file
  Header file for PEI AMT Policy functionality

@copyright
  Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
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
#ifndef _PEI_AMT_LIB_H_
#define _PEI_AMT_LIB_H_

#include <Ppi/HeciPpi.h>
#include <Ppi/AmtPolicyPpi.h>
#include <Ppi/AmtStatusCode.h>
#include "Library/MeSpsPolicyAccessPeiLib.h"
#include <MeBiosPayloadHob.h>


/**
  Install MbpData protocol.

#ifndef SERVER_BIOS_FLAG
  @param[in]  HeciPpi                 The pointer with Heci Ppi
#endif // SERVER_BIOS_FLAG
  @param[out] MbpHobPtr           The pointer with MbpHob installed.

  @return: None

**/
VOID
InstallMbp (
  OUT ME_BIOS_PAYLOAD_HOB  **MbpHobPtr
  )
;

/**
  This function performs basic initialization for ME in PEI phase after memory is initialized.
**/
VOID
EFIAPI
MePostMemInitAmt (
  VOID
  )
;

//APTIOV_SERVER_OVERRIDE_RC_START : Loss function declaration
/**
  This function performs basic initialization for AMT in PEI phase after DID.
**/
VOID
EFIAPI
AmtPostMemInit (
  VOID
  )
;
//APTIOV_SERVER_OVERRIDE_RC_END

/**
  Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei (
  VOID
  )
;

/**
  Save FWSTS to ME FWSTS HOB, if the HOB is not existing, the HOB will be created and publish.
  If the HOB is existing, the data will be overrided.
**/
VOID
SaveFwStsToHob (
  VOID
  )
;

/**
  Start Watch Dog Timer HECI message

  @param[in] HeciPpi              The pointer to HECI PPI
  @param[in] WaitTimerBios        The value of waiting limit in seconds

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Timeout value is invalid as 0
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciAsfStartWatchDog (
  IN  HECI_PPI                    *HeciPpi,
  IN  UINT16                      WaitTimerBios
  );

/**
  Stop ASF BIOS Watch Dog Timer HECI message. The function call in PEI phase 
  will not check WatchDog policy, always disable WDT once it is invoked to disable
  any initialized WDT.

  @param[in] HeciPpi              The pointer to HECI PPI

**/
VOID
PeiHeciAsfStopWatchDog (
  IN  HECI_PPI                    *HeciPpi
  );

/**
  Check if Amt is enabled in setup options.

  @param[in] AmtPolicyPpi The AMT Policy protocol instance

  @retval EFI_SUCCESS             AMT policy Ppi located
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PeiAmtPolicyLibInit (
  IN AMT_POLICY_PPI      **AmtPolicyPpi
  );

/**
  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
PeiAmtWatchDog (
  VOID
  );

/**
  Get WatchDog BIOS Timmer.

  @retval UINT16                  WatchDog BIOS Timer
**/
UINT16
PeiAmtWatchTimerBiosGet (
  VOID
  );

/**
  Check if AMT is enabled in setup options.

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.
**/
BOOLEAN
PeiAmtSupported (
  VOID
  );

/**
  Check if ASF is enabled in setup options.

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.
**/
BOOLEAN
PeiAsfSupported (
  VOID
  );

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  VOID
  );

/**
  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
PeiFwProgressSupport (
  VOID
  );

/**
  Provide Amt Sol setting from MEBx BIOS Sync Data

  @retval TRUE                    Amt Sol is enabled.
  @retval FALSE                   Amt Sol is disabled.
**/
BOOLEAN
AmtSolSetting (
  VOID
  );

#endif
