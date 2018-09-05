/** @file
  Header file for functions to get Intel ME information

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
#ifndef _DXE_ME_LIB_H_
#define _DXE_ME_LIB_H_

#include <Protocol/HeciProtocol.h>
#include <Protocol/PlatformMeHook.h>
#include <Protocol/MePolicy.h>
#include <CoreBiosMsg.h>
#include <BupMsgs.h>
#include <IshMsgs.h>

//
// Reset Request Origin Codes.
//
#define PEI_HECI_REQ_ORIGIN_BIOS_MEMORY_INIT  0x01
#define PEI_HECI_REQ_ORIGIN_BIOS_POST         0x02
#define PEI_HECI_REQ_ORIGIN_AMTBX_LAN_DISABLE 0x03

//
// IFR Update states
//
#define IFR_UPDATE_ENABLE   1
#define IFR_UPDATE_DISABLE  0

#define MAX_ASSET_TABLE_ALLOCATED_SIZE  0x3000
#define HECI_HWA_CLIENT_ID              11
#define HWA_TABLE_PUSH_CMD              0

typedef struct {
  UINT32  MeEnabled : 1;          ///< [0]     ME enabled/Disabled
  UINT32  Reserved : 2;           ///< [2:1]   Reserved, must set to 0
  UINT32  IntelAmtFw : 1;         ///< [3]     Intel AMT FW support
  UINT32  IntelAmtFwStandard : 1; ///< [4]     Intel AMT Standard FW support
  UINT32  IntelSmallBusiness : 1; ///< [5]     Intel Small Business Technology support
  UINT32  Reserved1 : 7;          ///< [12:6]  Reserved
  UINT32  AtSupported : 1;        ///< [13]    AT Support
  UINT32  IntelKVM : 1;           ///< [14]    Intel KVM supported
  UINT32  LocalWakeupTimer : 1;   ///< [15]    Local Wakeup Timer support
  UINT32  Reserved2 : 16;         ///< [31:16] Reserved, must set to 0
  UINT32  MeMinorVer : 16;        ///< [47:32] ME FW Minor Version.
  UINT32  MeMajorVer : 16;        ///< [63:48] ME FW Major Version.
  UINT32  MeBuildNo : 16;         ///< [79:64] ME FW Build Number.
  UINT32  MeHotFixNo : 16;        ///< [95:80] ME FW Hotfix Number
} ME_CAP;

#pragma pack(1)

typedef enum _HWAI_TABLE_TYPE_INDEX {
  HWAI_TABLE_TYPE_INDEX_FRU_DEVICE  = 0,
  HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE,
  HWAI_TABLE_TYPE_INDEX_SMBIOS,
  HWAI_TABLE_TYPE_INDEX_ASF,
  HWAI_TABLE_TYPE_INDEX_MAX         = 4,
} HWAI_TABLE_TYPE_INDEX;

typedef struct _SINGLE_TABLE_POSITION {
  UINT16  Offset;
  UINT16  Length;
} SINGLE_TABLE_POSITION;

typedef struct _TABLE_PUSH_DATA {
  SINGLE_TABLE_POSITION Tables[HWAI_TABLE_TYPE_INDEX_MAX];
  UINT8                 TableData[1];
} TABLE_PUSH_DATA;

typedef union {
  UINT32  Data;
  struct {
    UINT32  MessageLength : 16;
    UINT32  Command : 4;          ///< only supported command would be HWA_TABLE_PUSH=0;
    UINT32  FRUTablePresent : 1;
    UINT32  MediaTablePresent : 1;
    UINT32  SMBIOSTablePresent : 1;
    UINT32  ASFTablePresent : 1;
    UINT32  Reserved : 8;
  } Fields;
} AU_MESSAGE_HEADER;

typedef struct _AU_TABLE_PUSH_MSG {
  AU_MESSAGE_HEADER Header;
  TABLE_PUSH_DATA   Data;
} AU_TABLE_PUSH_MSG;

typedef enum {
  SEV_NO_ERROR                      = 0,
  SEV_LOW_ERROR,
  SEV_HIGH_ERROR,
  SEV_CRITICAL_ERROR
} MDES_SEVERITY_LEVEL;

typedef enum {
  EFI_STATUS_CODE                   = 1,
  RAW_BUFFER,
  ASCII_DATA,
} MDES_BIOS_PAYLOAD_TYPE;

#pragma pack()

//
// Prototype for HECI message DXE phase
//

/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] ResetOrigin         Reset source
  @param[in] ResetType           Global or Host reset

  @exception EFI_UNSUPPORTED     Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciSendCbmResetRequest (
  IN  UINT8                      ResetOrigin,
  IN  UINT8                      ResetType
  );

/**
  Send Hardware Asset Tables to Firmware

  @param[in] Handle               A handle for this module
  @param[in] AssetTableData       Hardware Asset Table Data
  @param[in] TableDataSize        Size of Asset table

  @retval EFI_SUCCESS             Table sent
  @retval EFI_ABORTED             Could not allocate Memory
**/
EFI_STATUS
HeciAssetUpdateFwMsg (
  IN EFI_HANDLE                   Handle,
  IN TABLE_PUSH_DATA              *AssetTableData,
  IN UINT16                       TableDataSize
  );

/**
  Send Get Firmware SKU Request to ME

  @param[in, out] *FwCapsSku        ADD DESCRIPTION 

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetFwCapsSkuMsg (
  IN OUT MEFWCAPS_SKU               *FwCapsSku
  );

/**
  Send Get Firmware Version Request to ME

  @param[in, out] *MsgGenGetFwVersionAckData  Return themessage of FW version

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK_DATA  *MsgGenGetFwVersionAckData
  );

// APTIOV_SERVER_OVERRIDE_RC_START
/**
  Sends the MKHI Enable/Disable manageability message.
  The message will only work if bit 2 in the bitmasks is toggled.
  To enable manageability:
    EnableState = 0x00000004, and
    DisableState = 0x00000000.
  To disable manageability:
    EnableState = 0x00000000, and
    DisableState = 0x00000004

  @param[in] EnableState          Enable Bit Mask
  @param[in] DisableState         Disable Bit Mask

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciFwFeatureStateOverride (
  IN UINT32                       EnableState,
  IN UINT32                       DisableState
  );

/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] *RuleData           MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  );

// APTIOV_SERVER_OVERRIDE_RC_END
#if AMT_SUPPORT

/**
   Send Set Manufacturing Me Reset and Halt Request to ME 

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciSetManufacturingMeResetAndHalt(
  VOID
  );

/**
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in]  Nonce               Nonce received in previous HMRFPO_ENABLE Response Message
  @param[out] *Result             HMRFPO_ENABLE response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoEnable (
  IN  UINT64                      Nonce,
  OUT UINT8                       *Result
  );

/**
  Send HMRFPO disable MKHI to CSME.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoDisable (
  VOID
  );

/**
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register "Protected Range Limit" field with this value
  @param[out] Result              Status report

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoLock (
  OUT UINT64                      *Nonce,
  OUT UINT32                      *FactoryDefaultBase,
  OUT UINT32                      *FactoryDefaultLimit,
  OUT UINT8                       *Result
  );

/**
  System BIOS sends this message to get status for HMRFPO_LOCK message.

  @param[out] Result              HMRFPO_GET_STATUS response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoGetStatus (
  OUT UINT8                       *Result
  );

/**
  This is used to send KVM request message to Intel ME. When
  Bootoptions indicate that a KVM session is requested then BIOS
  will send this message before any graphical display output to
  ensure that FW is ready for KVM session.

  @param[in] QueryType            0 - Query Request
                                  1 - Cancel Request
  @param[out] ResponseCode        1h - Continue, KVM session established.
                                  2h - Continue, KVM session cancelled.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciQueryKvmRequest (
  IN  UINT32                      QueryType,
  OUT UINT32                      *ResponseCode
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to query the local firmware update interface status.

  @param[out] RuleData            1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetLocalFwUpdate (
  OUT UINT8                       *RuleData
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to enable or disable the local firmware update interface.
  The firmware allows a single update once it receives the enable command

  @param[in] RuleData             1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciSetLocalFwUpdate (
  IN UINT8                        RuleData
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to enable the ME State. The firmware allows a single
  update once it receives the enable command. Once firmware receives this message,
  the firmware will be in normal mode after a global reset.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME enabled message sent
**/
EFI_STATUS
HeciSetMeEnableMsg (
  VOID
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to disable the ME State. The firmware allows a single
  update once it receives the disable command Once firmware receives this message,
  the firmware will work in "Soft Temporary Disable" mode (HFS[19:16] = 3) after a
  global reset. Note, this message is not allowed when AT firmware is enrolled/configured.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME is disabled
**/
EFI_STATUS
HeciSetMeDisableMsg (
  VOID
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  Consumer or Corporate SKU size firmware.

  @param[out] RuleData            PlatformBrand,
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
HeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  );

/**
  This message is sent by the BIOS on the boot where the host wants to get the firmware provisioning state.
  The firmware will respond to AMT BIOS SYNCH INFO message even after the End of Post.

  @param[out] RuleData            Bit [2:0] Reserved
                                  Bit [4:3] Provisioning State
                                    00 - Pre -provisioning
                                    01 - In -provisioning
                                    02 - Post !Vprovisioning
                                  Bit [31:5] Reserved

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Firmware provisioning state returned
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciAmtBiosSynchInfo (
  OUT UINT32                      *RuleData
  );

/**
  The firmware will respond to GET OEM TAG message even after the End of Post (EOP).

  @param[out] RuleData            Default is zero. Tool can create the OEM specific OEM TAG data.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetOemTagMsg (
  OUT UINT32                      *RuleData
  );

/**
  Retrieves the current FW Register Lock State

  @param[out] LockState           FW Lock State (default, locked, unlocked)

  @exception EFI_UNSUPPORTED      Incorrect ME Mode
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetIccRegLock (
  OUT UINT8                       *LockState
  );

/**
  Sets ICC clock profile to be used on next and following boots

  @param[in] LockState              Requested FW LockState

  @exception EFI_UNSUPPORTED      Message attempted after EOP
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccRegLock (
  IN UINT8                        LockState
  );

/**
  retrieves the number of currently used ICC clock profile

  @param[out] Profile             number of current ICC clock profile
  @param[out] NumProfiles
  @param[out] ProfileDescriptions

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetIccProfile (
  OUT UINT8                       *Profile,
  OUT UINT8                       *NumProfiles,
  OUT ICC_PROFILE_DESC            *ProfileDescriptions
  );

/**
  Sets ICC clock profile to be used on next and following boots

  @param[in] Profile              number of profile to be used

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccProfile (
  IN UINT8                        Profile
  );

/**
  retrieves the number of currently used ICC clock profile

  @param[out] IccClockSettings    ICC Clock Settings of the requested clock

  @retval EFI_UNSUPPORTED         ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
  @retval EFI_INVALID_PARAMETER   IccClockSettings is a NULL pointer 
**/
EFI_STATUS
HeciGetIccClockSettings (
  OUT ICC_GETSET_CLOCK_SETTINGS_REQRSP  *IccClockSettings
  );

/**
  Sets ICC clock profile to be used on next and following boots

  @param[in] IccClockSettings     Pointer for requested ICC Clock Settings

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccClockSettings (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP  *IccClockSettings
  );


/**
  retrieves the Clock Range Definition Record

  @param[out] IccClockRangeRecords     Clock Range Definition Records containing 2 single CRDR

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetClockRangeDefinitionRecord (
  OUT ICC_CLK_RANGE_DEF_REC  *IccClockRangeRecords
  );

/**
  Writes 1 dword of data to the icc register offset specified by RegOffset in the ICC Aux space
  @param[in] RegOffset            Register Offset in ICC Aux Space to write
  @param[in] RegData              Dword ICC register data to write
  @param[in] ResponseMode 0       Wait for response, 1 - skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_INVALID_PARAMETER   ResponseMode is invalid value
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
HeciWriteIccRegDword (
  IN UINT16                       RegOffset,
  IN UINT32                       RegData,
  IN UINT8                        ResponseMode
  );

#endif // AMT_SUPPORT

/**
  Sends the MKHI Enable/Disable manageability message.
  The message will only work if bit 2 in the bitmasks is toggled.
  To enable manageability:
    EnableState = 0x00000004, and
    DisableState = 0x00000000.
  To disable manageability:
    EnableState = 0x00000000, and
    DisableState = 0x00000004

  @param[in] EnableState          Enable Bit Mask
  @param[in] DisableState         Disable Bit Mask

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciFwFeatureStateOverride (
  IN UINT32                       EnableState,
  IN UINT32                       DisableState
  );

/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] *RuleData           MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                *RuleData
  );

#if AMT_SUPPORT
/**
  This message is sent by the BIOS to inform ME FW whether or not to take the 
  TPM 1.2 Deactivate flow

  @param[in] TpmDeactivate  0 - ME FW should not take the deactivate flow.
                            1 - ME FW should take the deactivate flow.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciSendTpmData (
  IN UINT8               TpmDeactivate
  );

/**
  This message is sent by the BIOS when it wants to query
  ME Unconfig on RTC Clear Disable state.

  @param[out] RuleData             1 - Unconfig on RTC clear is disabled
                                  0 - Unconfig on RTC clear is not disabled

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciGetUnconfigOnRtcClearDisableMsg (
  OUT UINT32                *RuleData
  );

/**
  This message is sent by the BIOS when it wants to set
  ME Unconfig on RTC Clear Disable state.

  @param[in] RuleData             1 - Disable Unconfig on RTC clear
                                  0 - Enable Unconfig on RTC clear

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function or EOP was sent
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciSetUnconfigOnRtcClearDisableMsg (
  IN UINT32                RuleData
  );
/**
  Send Get Firmware SKU Request to ME

  @param[in][out] MbpHeader
  @param[in][out] MbpItems

  @exception EFI_UNSUPPORTED             Current ME mode doesn't support this function
  @retval EFI_SUCCESS                    Command succeeded
  @retval EFI_DEVICE_ERROR               HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT                    HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL           Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetMbpMsg (
  IN OUT MBP_HEADER            *MbpHeader,
  IN OUT UINT32                *MbpItems  
  );

//
// Prototype for ME Policy DXE phase
//
/**
  Check if Me is enabled.

  @retval EFI_SUCCESS             ME Policy pointer is initialized.
  @retval All other error conditions encountered when no ME Policy available.
**/
EFI_STATUS
MePolicyLibInit (
  VOID
  );
#endif // AMT_SUPPORT

/**
  Check if End of Post Message is enabled in setup options.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.
**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  );

/**
  Show Me Error message. This is used to support display error message on the screen for localization
  description

  @param[in] MsgId                Me error message ID for displaying on screen message
**/
VOID
MeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  );

#if AMT_SUPPORT
/**
  Check if MeFwDowngrade is enabled in setup options.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.
**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  );

/**
  This message is sent by the BIOS when it wants to send
  PDT Unlock Message to ISH.

  @param[in] FileName             Filename of bios2ish file.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function or EOP was sent
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/

EFI_STATUS
HeciPdtUnlockMsg (
  IN CONST CHAR8*                FileName
  );

/**
  This message is sent by the BIOS when it wants to notify that the BIOS is starting
  to run 3rd party code.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function or EOP was sent
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
HeciCoreBiosDoneMsg (
  VOID
  );

/**
  Check if Core Bios Done Message is enabled in setup options.

  @retval FALSE                   Core Bios Done is disabled.
  @retval TRUE                    Core Bios Done is enabled.
**/

BOOLEAN
CoreBiosDoneEnabled (
    VOID
  );

#endif // AMT_SUPPORT

/**
  Checks if EndOfPost event already happened


  @retval TRUE                    if end of post happened
  @retval FALSE                   if not yet
**/
BOOLEAN
MeIsAfterEndOfPost (
  VOID
  )
;




/**

  Sets EndOfPostDone value.

  @param[in]
  @retval TRUE    EndOfPostDone has been set
  @retval FALSE   Cannot set EndOfPostDone

**/
BOOLEAN
SetEndOfPostDone (
  VOID
  )
;
#endif // _DXE_ME_LIB_H_