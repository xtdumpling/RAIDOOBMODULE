/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  HeciMsgLib.h

@brief
  Header file for Heci Message functionality

**/
#ifndef _HECI_MESSAGE_LIB_H_
#define _HECI_MESSAGE_LIB_H_

#include <PiPei.h>
#include <Protocol/HeciProtocol.h>
#include <CoreBiosMsg.h>
#include <Ppi/MeSpsPolicyPei.h>
#include <Ppi/MePolicyPpi.h>
#include <BupMsgs.h>
#if AMT_SUPPORT
#include <MeBiosPayloadData.h>
#endif //AMT_SUPPORT
#include "Library/DxeMeLib.h"

//
// End of Post Codes
//
#define HECI_EOP_STATUS_SUCCESS       0x0
#define HECI_EOP_PERFORM_GLOBAL_RESET 0x1
#define MAX_EOP_SEND_RETRIES          0x2

//
// Heci Bus Disable defines
//
#define HECI_BUS_DISABLE_OPCODE     0x0C
#define HECI_BUS_DISABLE_ACK_OPCODE 0x8C
#define ME_HECI_FIXED_ADDRESS       0x0

#define EFI_ME_FW_SKU_VARIABLE_GUID \
  { \
    0xe1a21d94, 0x4a20, 0x4e0e, 0xae, 0x9, 0xa9, 0xa2, 0x1f, 0x24, 0xbb, 0x9e \
  }
//
// Defines the HECI request buffer format for the ICC_MPHY_WRITE_SETTINGS_CMD.
//

#pragma pack(1)

typedef struct _ICC_MPHY_WRITE_SETTINGS_REQ {
   ICC_HEADER  Header;           // Standard ICC HECI Header
   UINT32      Reserved    : 4;  // Reserved for future use
   UINT32      PostedWrite : 1;  // 0-Response returned, 1-No Response returned
   UINT32      Reserved2   : 23; // Reserved for future use
} MPHY_WRITE_SETTINGS_REQ;

typedef union _HBM_COMMAND {
  UINT8 Data;
  struct {
    UINT8 Command    : 7;
    UINT8 IsResponse : 1;
  } Fields;
} HBM_COMMAND;

typedef struct _HECI_BUS_DISABLE_CMD {
  HBM_COMMAND Command;
  UINT8       Reserved[3];
} HECI_BUS_DISABLE_CMD;

typedef struct _HECI_BUS_DISABLE_CMD_ACK {
  HBM_COMMAND Command;
  UINT8       Status;
  UINT8       Reserved[2];
} HECI_BUS_DISABLE_CMD_ACK;

#pragma pack()

/**

  @brief
  Sets or reads Lock mask on ICC registers.
  @param[in] AccessMode           0 - set, 1 - get
  @param[in] ResponseMode         0 - firmware will answer, 1 - firmware will not answer
  @param[in][out] LockRegInfo     bundle count info and mask of registers to become (for 'set' mode) or are
                                  (for 'get' mode) locked. Each bit represents a register. 0=lock, 1=don't lock

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_INVALID_PARAMETER   ResponseMode or pointer of Mask is invalid value
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciLockIccRegisters (
  IN UINT8       AccessMode,
  IN UINT8       ResponseMode,
  IN OUT ICC_LOCK_REGS_INFO  *LockRegInfo
  )
;


#ifdef AMT_SUPPORT

/**
  Send Get MBP from FW

  @param[in] MbpHeader     MBP header of the response
  @param[in] MbpItems       MBP items of the response

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
  )
;

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
  IN OUT MEFWCAPS_SKU             *FwCapsSku
  )
;

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
  )
;

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
  )
;


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
  )
;
#endif // AMT_SUPPORT

/**
  Dump PEI ME SPS Policy

  @param[in] SpsPolicy  The Me SPS Policy instance
**/
VOID
SpsPolicyDebugDump (
  IN  SPS_POLICY_PPI      *SpsPolicyPpi
  );

/**

  @brief
  Send HSIO request through HECI to get the HSIO settings version on CSME side.

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
  )
;

/**

  @brief
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
  )
;

/**
  Initialize HECI Interface.

  @param [in] None

  @retval Status Initialization Status
**/

EFI_STATUS
PeiHeciInitialize (
  VOID
  )
;

/**
  Send DRAM init done message through HECI to inform ME of memory initialization done.

  @param[in] DidInput             ME UMA data
  @param [in] WaitDidAck          Wait for DID Ack
  @param[out] BiosAction          ME response to DID

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendDid (
  IN  UINT32                      DidInput,
  OUT UINT8                       *BiosAction
  )
;

/**
  Signal CSME FW BUP to leave the MANUF disabled state.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciClearManufDisableRequest (
  VOID
  )
;

/**
  Send Tracing Enable request to CSME via HECI.


  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciTracingEnableRequest (
  VOID
  )
;

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
  )
;

#endif // _HECI_MESSAGE_LIB_H_
