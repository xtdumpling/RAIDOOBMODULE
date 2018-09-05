/** @file
  Header file for PEI Me functionality

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
#ifndef _PEI_ME_LIB_H_
#define _PEI_ME_LIB_H_

#include <Ppi/HeciPpi.h>
#include <Ppi/MePolicyPpi.h>
#include <MeBiosPayloadData.h>

//
// Prototype for HECI message PEI phase
//

/**
  Send DRAM init done message through HECI to inform ME of memory initialization done.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MeHgs                ME UMA data
  @param[out] MeHfs              ME response to DID

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendDid (
  IN  UINT32                      DidInput,
  OUT UINT8                       *BiosAction
  );

//
// Prototype for ME Policy PEI phase
//

/**
  Check if Me is enabled.

  @param[in] PeiMePolicy  The Me Policy protocol instance

  @retval EFI_SUCCESS     ME policy Ppi loacted
  @retval All other error conditions encountered when no ME Policy Ppi
**/
EFI_STATUS
PeiMePolicyLibInit (
  IN  ME_POLICY_PPI      **PeiMePolicy
  );

/**
  Read the ME policy to see if HECI timeouts are enabled.
  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled
**/
BOOLEAN
HeciTimeoutsEnabled (
  VOID
  );

/**
  Send ICC request through HECI to query if CSME FW requires the warm reset flow from a previous boot.

  @param[out] WarmResetRequired   1 - CSME requires a warm reset to complete BCLK ramp en flow

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciIccBclkMsg (
  OUT UINT8    *WarmResetRequired
  );

/**
  Send HSIO request through HECI to get the HSIO settings version on CSME side.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] BiosCmd              HSIO command: 0 - close interface, 1 - report HSIO version
  @param[out] FwSts               FwSts

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciHsioMsg (
  IN  UINT32                      BiosCmd,
  OUT UINT16                      *Crc
  );

/**
  Send the required system ChipsetInit Table to ME FW.

  @param[in] ChipsetInitTable     The required system ChipsetInit Table.
  @param[in] ChipsetInitTableLen  Length of the table in bytes

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciChipsetInitSyncMsg (
  IN  UINT8                       *ChipsetInitTable,
  IN  UINT32                      ChipsetInitTableLen
  );

/**
  Send Get MBP from FW

  @param[in] MbpHeader              MBP header of the response
  @param[in] MbpItems               MBP items of the response
  @paran[in] SkipMbp                Skip MBP

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetMbpMsg (
  IN OUT MBP_HEADER            *MbpHeader,
  IN OUT UINT32                *MbpItems,
  IN BOOLEAN                   SkibMbp
  );

/**
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku              ME Firmware Capability SKU

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetFwCapsSkuMsg (
  IN OUT MEFWCAPS_SKU               *FwCapsSku
  );

/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData            MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
PeiHeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  );

/**
  This message is sent by the BIOS or IntelR MEBX. One of usages is to utilize
  this command to determine if the platform runs in Consumer or Corporate SKU
  size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetUsageType

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
PeiHeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  );


/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] ResetOrigin         Reset source
  @param[in] ResetType           Global or Host reset

  @exception EFI_UNSUPPORTED     Current ME mode doesn't support this function
  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_NOT_FOUND          No ME present
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT            HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendCbmResetRequest (
  IN  UINT8                      ResetOrigin,
  IN  UINT8                      ResetType
  );


#endif
