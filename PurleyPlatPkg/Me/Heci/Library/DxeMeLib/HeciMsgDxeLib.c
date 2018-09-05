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

@file:

  HeciMsgDxeLib.c

@brief:

  Implementation file for Heci Message functionality

**/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/TimerLib.h>

#include "CoreBiosMsg.h"
#include "Protocol/Wdt.h"
#include "Library/DebugLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Library/BaseLib.h"
#include "Library/BaseMemoryLib.h"
#include "Guid/StatusCodeDataTypeId.h"
#include "Library/PerformanceLib.h"
#include "Library/HeciMsgLib.h"
#include "HeciRegs.h"
#include "MeAccess.h"
#include "MeState.h"
#if AMT_SUPPORT
#include "Protocol/PlatformMeHook.h"
#include "BupMsgs.h"
#endif // AMT_SUPPORT


BOOLEAN
MeIsAfterEndOfPost (
  VOID
  );

//
// Interface functions of HeciMsgLib
//

/**

  @brief
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
  IN  UINT8                             ResetOrigin,
  IN  UINT8                             ResetType
  )
{
  HECI_PROTOCOL             *Heci;
  EFI_STATUS                Status;
  UINT32                    HeciLength;
  CBM_RESET_REQ             CbmResetRequest;
#if AMT_SUPPORT
  PLATFORM_ME_HOOK_PROTOCOL *PlatformMeHook;
#endif // AMT_SUPPORT
  WDT_PROTOCOL              *WdtProtocol;
  UINT32                    MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSendCbmResetRequest.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

#if AMT_SUPPORT
  Status = gBS->LocateProtocol (
                  &gPlatformMeHookProtocolGuid,
                  NULL,
                  (VOID **) &PlatformMeHook
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to Locate PlatformMeHook Protocol for Global Reset Hook, so skip instead.- %r\n", Status));
  } else {
    PlatformMeHook->PreGlobalReset ();
  }
#endif // AMT_SUPPORT

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
   DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
   return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  CbmResetRequest.MKHIHeader.Data               = 0;
  CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
  CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
  CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
  CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
  CbmResetRequest.MKHIHeader.Fields.Result      = 0;
  CbmResetRequest.Data.RequestOrigin            = ResetOrigin;
  CbmResetRequest.Data.ResetType                = ResetType;

  HeciLength = sizeof (CBM_RESET_REQ);

  Status = gBS->LocateProtocol (&gWdtProtocolGuid, NULL, (VOID **) &WdtProtocol);
  ASSERT_EFI_ERROR (Status);
  WdtProtocol->AllowKnownReset ();

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &CbmResetRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Send Reset Request - %r\n", Status));
  }

  return Status;
}

#if AMT_SUPPORT
/**

  @brief
  Send Hardware Asset Tables to Firmware

  @param[in] Handle               A handle for this module
  @param[in] AssetTableData       Hardware Asset Table Data
  @param[in] TableDataSize        Size of Asset table

  @retval EFI_SUCCESS             Table sent
  @retval EFI_ABORTED             Could not allocate Memory

**/
EFI_STATUS
HeciAssetUpdateFwMsg (
  IN EFI_HANDLE      Handle,
  IN TABLE_PUSH_DATA *AssetTableData,
  IN UINT16          TableDataSize
  )
{
  AU_TABLE_PUSH_MSG *SendAssetTableDataMsg;
  EFI_STATUS        Status;
  HECI_PROTOCOL     *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciAssetUpdateFwMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }
  ///
  /// Subtract off single byte from TABLE_PUSH_DATA.TableData[1]
  ///
  SendAssetTableDataMsg = AllocateZeroPool (sizeof (AU_TABLE_PUSH_MSG) + MAX_ASSET_TABLE_ALLOCATED_SIZE - 1);
  if (SendAssetTableDataMsg == NULL) {
    DEBUG ((DEBUG_ERROR, "AssetUpdateFwMsg Error: Could not allocate Memory\n"));
    return EFI_ABORTED;
  }

  if (TableDataSize > MAX_ASSET_TABLE_ALLOCATED_SIZE) {
    TableDataSize = MAX_ASSET_TABLE_ALLOCATED_SIZE;
  }

  SendAssetTableDataMsg->Header.Data = 0;
  ///
  /// Subtract off single byte from TABLE_PUSH_DATA.TableData[1]
  ///
  SendAssetTableDataMsg->Header.Fields.MessageLength      = TableDataSize + sizeof (TABLE_PUSH_DATA) - 1;
  SendAssetTableDataMsg->Header.Fields.Command            = HWA_TABLE_PUSH_CMD;
  SendAssetTableDataMsg->Header.Fields.FRUTablePresent    = 1;
  SendAssetTableDataMsg->Header.Fields.SMBIOSTablePresent = 1;
  SendAssetTableDataMsg->Header.Fields.ASFTablePresent    = 1;
  if (AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Length == 0) {
    SendAssetTableDataMsg->Header.Fields.MediaTablePresent = 0;
  } else {
    SendAssetTableDataMsg->Header.Fields.MediaTablePresent = 1;
  }

  CopyMem (&SendAssetTableDataMsg->Data, AssetTableData, SendAssetTableDataMsg->Header.Fields.MessageLength);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) SendAssetTableDataMsg,
                  SendAssetTableDataMsg->Header.Fields.MessageLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_HWA_CLIENT_ID
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "AssetUpdateFwMsg: Failed to Send SendAssetTableDataMsg\n"));
  }

  FreePool (SendAssetTableDataMsg);

  return Status;

}
#endif // AMT_SUPPORT

/**

  @brief
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku            Return Data from Get Firmware Capabilities MKHI Request

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciGetFwCapsSkuMsg (
  IN OUT MEFWCAPS_SKU        *FwCapsSku
  )
{
  EFI_STATUS                  Status;
  GEN_GET_FW_CAPS_SKU_BUFFER      MsgGenGetFwCapsSku;
  UINT32                      Length;
  UINT32                      RecvLength;
  HECI_PROTOCOL              *Heci;
  UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetFwCapsSkuMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetFwCapsSku.Request.MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetFwCapsSku.Request.Data.RuleId                   = 0;
  Length                                                   = sizeof (GEN_GET_FW_CAPSKU);
  RecvLength                                               = sizeof (GEN_GET_FW_CAPS_SKU_ACK);

  ///
  /// Send Get FW SKU Request to ME
  ///
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetFwCapsSku,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetFwCapsSkuMsg: message failed! - %r\n", Status));
    return Status;
  }
  
  if ((MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.IsResponse != 1) ||
	  (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Result != 0)
     ) {
    DEBUG ((DEBUG_ERROR, "[ME] No ACK for sent message.\n"));
    return EFI_DEVICE_ERROR;
  }

  *FwCapsSku = MsgGenGetFwCapsSku.Response.Data.FWCapSku;

  return Status;
}

/**

  @brief
  Send Get Firmware Version Request to ME

  @param[in][out] MsgGenGetFwVersionAckData   Return the message of FW version

  @exception EFI_UNSUPPORTED        Current ME mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK_DATA  *MsgGenGetFwVersionAckData
  )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_VER_ACK              MsgGenGetFwVersion;
  UINT32                          Length;
  UINT32                          RecvLength;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetFwVersionMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Initialize MsgGenGetFwVersion data structure
  ///
  MsgGenGetFwVersion.MKHIHeader.Data              = 0;
  MsgGenGetFwVersion.MKHIHeader.Fields.GroupId    = MKHI_GEN_GROUP_ID;
  MsgGenGetFwVersion.MKHIHeader.Fields.Command    = GEN_GET_FW_VERSION_CMD;
  MsgGenGetFwVersion.MKHIHeader.Fields.IsResponse = 0;
  Length                                          = sizeof (GEN_GET_FW_VER);
  RecvLength                                      = sizeof (GEN_GET_FW_VER_ACK);
  ///
  /// Send Get Firmware Version Request to ME
  ///
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetFwVersion,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetFwVersionMsg: message failed! - %r\n", Status));
    return Status;
  }
  
  if ((MsgGenGetFwVersion.MKHIHeader.Fields.Command != GEN_GET_FW_VERSION_CMD) ||
      (MsgGenGetFwVersion.MKHIHeader.Fields.GroupId != MKHI_GEN_GROUP_ID) ||
      (MsgGenGetFwVersion.MKHIHeader.Fields.IsResponse != 1) ||
      (MsgGenGetFwVersion.MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for GET_FW_VERSION (MKHI: %08X)\n", ((GEN_GET_FW_VER_ACK*)&MsgGenGetFwVersion)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  *MsgGenGetFwVersionAckData = MsgGenGetFwVersion.Data;

  return Status;
}


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
  )
{
  EFI_STATUS                      Status;
  GEN_SET_ME_RESET_HALT_ACK       MsgGenSetMeResetAndHalt;
  HECI_FWS_REGISTER               MeFirmwareStatus;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetManufacturingMeResetAndHalt\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Check for Manufacturing Mode.
  ///
  MeFirmwareStatus.ul = MmioRead32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_FWSTATE);

  if(!(MeFirmwareStatus.r.ManufacturingMode)) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Allocate MsgGenSetMeResetAndHalt data structure
  ///
  MsgGenSetMeResetAndHalt.MKHIHeader.Data              = 0;
  MsgGenSetMeResetAndHalt.MKHIHeader.Fields.GroupId    = MKHI_GEN_GROUP_ID;
  MsgGenSetMeResetAndHalt.MKHIHeader.Fields.Command    = GEN_SET_ME_RESET_HALT_CMD;
  MsgGenSetMeResetAndHalt.MKHIHeader.Fields.IsResponse = 0;
  HeciSendLength                                       = sizeof (GEN_SET_ME_RESET_HALT);
  HeciRecvLength                                       = sizeof (GEN_SET_ME_RESET_HALT_ACK);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenSetMeResetAndHalt,
                  HeciSendLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Send error for HeciSetManufacturingMeResetAndHalt() (%r)\n", Status));
    return Status;
  }
  
  Status = Heci->ReadMsg (
                  HECI1_DEVICE,
                  BLOCKING,
                  (UINT32 *) &MsgGenSetMeResetAndHalt,
                  &HeciRecvLength
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Response error for HeciSetManufacturingMeResetAndHalt() (%r)\n",
                         Status));
  } else if (MsgGenSetMeResetAndHalt.MKHIHeader.Data != 0x000090FF) {
    DEBUG ((DEBUG_ERROR, "[ME] Wrong for HeciSetManufacturingMeResetAndHalt() (Mhi: 0x%x)\n",
                         MsgGenSetMeResetAndHalt.MKHIHeader.Data));
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**

  @brief
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in] Nonce                Nonce received in previous HMRFPO_ENABLE Response Message
  @param[in] Result               HMRFPO_ENABLE response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoEnable (
  IN  UINT64                          Nonce,
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                  Status;
  HECI_PROTOCOL               *Heci;
  MKHI_HMRFPO_ENABLE          HmrfpoEnableRequest;
  MKHI_HMRFPO_ENABLE_RESPONSE HmrfpoEnableResponse;
  UINT32                      HeciLength;
  UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciHmrfpoEnable.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoEnableRequest.MkhiHeader.Data               = 0;
  HmrfpoEnableRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.Command     = HMRFPO_ENABLE_CMD_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.IsResponse  = 0;
  HmrfpoEnableRequest.Nonce                         = Nonce;

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &HmrfpoEnableRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Send HMRFPO_ENABLE_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE_RESPONSE);
  Status = Heci->ReadMsg (
                  HECI1_DEVICE,
                  BLOCKING,
                  (UINT32 *) &HmrfpoEnableResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Read HMRFPO_ENABLE_CMD_ID Result - %r\n", Status));
    return Status;
  }

  if ((((MKHI_HMRFPO_ENABLE_RESPONSE*)&HmrfpoEnableResponse)->MkhiHeader.Fields.Command != HMRFPO_ENABLE_CMD_ID) ||
      (((MKHI_HMRFPO_ENABLE_RESPONSE*)&HmrfpoEnableResponse)->MkhiHeader.Fields.GroupId != MKHI_SPI_GROUP_ID) ||
      (((MKHI_HMRFPO_ENABLE_RESPONSE*)&HmrfpoEnableResponse)->MkhiHeader.Fields.IsResponse != 1) ||
      (((MKHI_HMRFPO_ENABLE_RESPONSE*)&HmrfpoEnableResponse)->MkhiHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for HMRFPO_ENABLE (MKHI: %08X)\n", ((MKHI_HMRFPO_ENABLE_RESPONSE*)&HmrfpoEnableResponse)->MkhiHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  *Result = HmrfpoEnableResponse.Status;

  return Status;
}

/**
  @brief
  Send HMRFPO disable MKHI to CSME.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoDisable (
  VOID
  )
{
  EFI_STATUS                  Status;
  HECI_PROTOCOL               *Heci;
  HMRFPO_DISABLE_CMD_REQ      HmrfpoDisableReq;
  UINT32                      Length;
  UINT32                      RespLength;
  UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciHmrfpoDisable.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoDisableReq.MkhiHeader.Data               = 0;
  HmrfpoDisableReq.MkhiHeader.Fields.Command     = HMRFPO_DISABLE_CMD;
  HmrfpoDisableReq.MkhiHeader.Fields.IsResponse  = 0;
  HmrfpoDisableReq.MkhiHeader.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  HmrfpoDisableReq.MkhiHeader.Fields.Reserved    = 0;
  HmrfpoDisableReq.MkhiHeader.Fields.Result      = 0;

  Length     = sizeof (HMRFPO_DISABLE_CMD_REQ);
  RespLength = sizeof (HMRFPO_DISABLE_CMD_RESP);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &HmrfpoDisableReq,
                  Length,
                  &RespLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciHmrfpoDisable: message failed! - %r\n", Status));
	return Status;
  }
  
  if ((((HMRFPO_DISABLE_CMD_RESP*)&HmrfpoDisableReq)->MkhiHeader.Fields.Command != HMRFPO_DISABLE_CMD) ||
      (((HMRFPO_DISABLE_CMD_RESP*)&HmrfpoDisableReq)->MkhiHeader.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (((HMRFPO_DISABLE_CMD_RESP*)&HmrfpoDisableReq)->MkhiHeader.Fields.IsResponse != 1) ||
      (((HMRFPO_DISABLE_CMD_RESP*)&HmrfpoDisableReq)->MkhiHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for HMRFPO_DISABLE (MKHI: %08X)\n", ((HMRFPO_DISABLE_CMD_RESP*)&HmrfpoDisableReq)->MkhiHeader.Data));
    return EFI_UNSUPPORTED;
  }

  return Status;
}


/**

  @brief
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register!¦s "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register!¦s "Protected Range Limit" field with this value
  @param[out] Result              Status report

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoLock (
  OUT UINT64                          *Nonce,
  OUT UINT32                          *FactoryDefaultBase,
  OUT UINT32                          *FactoryDefaultLimit,
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                Status;
  HECI_PROTOCOL             *Heci;
  MKHI_HMRFPO_LOCK          HmrfpoLockRequest;
  MKHI_HMRFPO_LOCK_RESPONSE HmrfpoLockResponse;
  UINT32                    HeciLength;
  UINT32                    MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciHmrfpoLock.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoLockRequest.MkhiHeader.Data               = 0;
  HmrfpoLockRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.Command     = HMRFPO_LOCK_CMD_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.IsResponse  = 0;

  HeciLength = sizeof (MKHI_HMRFPO_LOCK);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &HmrfpoLockRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Send HMRFPO_LOCK_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_LOCK_RESPONSE);
  Status = Heci->ReadMsg (
                  HECI1_DEVICE,
                  BLOCKING,
                  (UINT32 *) &HmrfpoLockResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to read HMRFPO_LOCK_CMD_ID response - %r.\n", Status));
    return Status;
  }

  if ((((MKHI_HMRFPO_LOCK_RESPONSE*)&HmrfpoLockResponse)->MkhiHeader.Fields.Command != HMRFPO_LOCK_CMD_ID) ||
      (((MKHI_HMRFPO_LOCK_RESPONSE*)&HmrfpoLockResponse)->MkhiHeader.Fields.GroupId != MKHI_SPI_GROUP_ID) ||
      (((MKHI_HMRFPO_LOCK_RESPONSE*)&HmrfpoLockResponse)->MkhiHeader.Fields.IsResponse != 1) ||
      (((MKHI_HMRFPO_LOCK_RESPONSE*)&HmrfpoLockResponse)->MkhiHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for HMRFPO_LOCK (MKHI: %08X)\n", ((MKHI_HMRFPO_LOCK_RESPONSE*)&HmrfpoLockResponse)->MkhiHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  *Nonce                = HmrfpoLockResponse.Nonce;
  *FactoryDefaultBase   = HmrfpoLockResponse.FactoryDefaultBase;
  *FactoryDefaultLimit  = HmrfpoLockResponse.FactoryDefaultLimit;
  *Result               = HmrfpoLockResponse.Status;

  return Status;
}

/**

  @brief
  System BIOS sends this message to get status for HMRFPO_LOCK message.

  @param[out] Result              HMRFPO_GET_STATUS response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciHmrfpoGetStatus (
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                      Status;
  HECI_PROTOCOL                   *Heci;
  MKHI_HMRFPO_GET_STATUS          HmrfpoGetStatusRequest;
  MKHI_HMRFPO_GET_STATUS_RESPONSE HmrfpoGetStatusResponse;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciHmrfpoGetStatus.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoGetStatusRequest.MkhiHeader.Data              = 0;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.GroupId    = MKHI_SPI_GROUP_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.Command    = HMRFPO_GET_STATUS_CMD_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.IsResponse = 0;

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &HmrfpoGetStatusRequest,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Send HMRFPO_GET_STATUS_CMD_ID - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS_RESPONSE);
  Status = Heci->ReadMsg (
                  HECI1_DEVICE,
                  BLOCKING,
                  (UINT32 *) &HmrfpoGetStatusResponse,
                  &HeciLength
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Unable to Read HMRFPO_GET_STATUS_CMD_ID Result - %r\n", Status));
	return Status;
  }

  if ((HmrfpoGetStatusResponse.MkhiHeader.Fields.Command != HMRFPO_GET_STATUS_CMD_ID) ||
      (HmrfpoGetStatusResponse.MkhiHeader.Fields.GroupId != MKHI_SPI_GROUP_ID) ||
      (HmrfpoGetStatusResponse.MkhiHeader.Fields.IsResponse != 1) ||
      (HmrfpoGetStatusResponse.MkhiHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for HMRFPO_GET_STATUS (MKHI: %08X)\n", ((MKHI_HMRFPO_GET_STATUS_RESPONSE*)&HmrfpoGetStatusResponse)->MkhiHeader.Data));
    return EFI_DEVICE_ERROR;
  }
  
  *Result = HmrfpoGetStatusResponse.Status;

  return Status;
}

/**

  @brief
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
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciQueryKvmRequest (
  IN  UINT32                         QueryType,
  OUT UINT32                         *ResponseCode
  )
{
  EFI_STATUS              Status;
  HECI_PROTOCOL           *Heci;
  AMT_QUERY_KVM_REQUEST   QueryKvmRequest;
  AMT_QUERY_KVM_RESPONSE  QueryKvmResponse;
  UINT32                  HeciLength;
  UINT16                  TimeOut;
  UINT32                  MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciQueryKvmRequest.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  QueryKvmRequest.Command       = KVM_MESSAGE_COMMAND;
  QueryKvmRequest.ByteCount     = KVM_BYTE_COUNT;
  QueryKvmRequest.SubCommand    = KVM_QUERY_REQUES;
  QueryKvmRequest.VersionNumber = KVM_VERSION;
  QueryKvmRequest.QueryType     = QueryType;

  HeciLength                    = sizeof (AMT_QUERY_KVM_REQUEST);

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &QueryKvmRequest,
                  HeciLength,
                  BIOS_ASF_HOST_ADDR,
                  HECI_ASF_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "Query KVM failed %r\n", Status));
	return Status;
  }

  TimeOut     = 0;
  HeciLength  = sizeof (AMT_QUERY_KVM_RESPONSE);
  Status = Heci->ReadMsg (
                  HECI1_DEVICE,
                  BLOCKING,
                  (UINT32 *) &QueryKvmResponse,
                  &HeciLength
                  );

  if (QueryType == QUERY_REQUEST) {
    while (EFI_ERROR (Status)) {
      MicroSecondDelay (KVM_STALL_1_SECOND);
      TimeOut++;

      if (TimeOut > KVM_MAX_WAIT_TIME) {
        break;
      }

      HeciLength = sizeof (AMT_QUERY_KVM_RESPONSE);
      Status = Heci->ReadMsg (
                      HECI1_DEVICE,
                      NON_BLOCKING,
                      (UINT32 *) &QueryKvmResponse,
                      &HeciLength
                      );
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Read AMT_QUERY_KVM_RESPONSE Result - %r\n", Status));
	return Status;
  }

  if ((QueryKvmResponse.Command != KVM_MESSAGE_COMMAND) ||
      (QueryKvmResponse.ResponseCode != 1)
      ) {
    DEBUG ((DEBUG_ERROR, "[ME] No ACK for sent message.\n"));
    return EFI_DEVICE_ERROR;
  }

  *ResponseCode = QueryKvmResponse.ResponseCode;

  return Status;
}

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to query the local firmware update interface status.

  @param[out] RuleData            1 - local firmware update interface enable
                                  0 - local firmware update interface disable

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
HeciGetLocalFwUpdate (
  OUT UINT8         *RuleData
  )
{
  EFI_STATUS                  Status;
  UINT32                      Length;
  UINT32                      RecvLength;
  HECI_PROTOCOL               *Heci;
  GEN_GET_LOCAL_FW_UPDATE_ACK MsgGenGetLocalFwUpdate;
  UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetLocalFwUpdate.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetLocalFwUpdate.Data.RuleId                  = 7;
  Length                                              = sizeof (GEN_GET_LOCAL_FW_UPDATE);
  RecvLength                                          = sizeof (GEN_GET_LOCAL_FW_UPDATE_ACK);

  ///
  /// Send Get Local FW update Request to ME
  ///
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetLocalFwUpdate,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetLocalFwUpdate: message failed! - %r\n", Status));
	return Status;
  }

  if ((MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (MsgGenGetLocalFwUpdate.MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (MsgGenGetLocalFwUpdate.MKHIHeader.Fields.IsResponse != 1) ||
      (MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n", ((GEN_GET_LOCAL_FW_UPDATE_ACK*)&MsgGenGetLocalFwUpdate)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  *RuleData = MsgGenGetLocalFwUpdate.Data.RuleData;


  return Status;
}

/**

  @brief
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
  IN UINT8         RuleData
  )
{
  EFI_STATUS                  Status;
  UINT32                      Length;
  UINT32                      RecvLength;
  HECI_PROTOCOL               *Heci;
  GEN_SET_LOCAL_FW_UPDATE     MsgGenSetLocalFwUpdate;
  UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetLocalFwUpdate.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenSetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_SET_RULE_CMD;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenSetLocalFwUpdate.Data.RuleId                  = 7;
  MsgGenSetLocalFwUpdate.Data.RuleDataLen             = 1;
  MsgGenSetLocalFwUpdate.Data.RuleData                = RuleData;
  Length                                              = sizeof (GEN_SET_LOCAL_FW_UPDATE);
  RecvLength                                          = sizeof (GEN_SET_LOCAL_FW_UPDATE_ACK);

  ///
  /// Send Get Local FW update Request to ME
  ///
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenSetLocalFwUpdate,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSetLocalFwUpdate: message failed! - %r\n", Status));
  }

  if ((((GEN_SET_LOCAL_FW_UPDATE_ACK*)&MsgGenSetLocalFwUpdate)->MKHIHeader.Fields.Command != FWCAPS_SET_RULE_CMD) ||
      (((GEN_GET_LOCAL_FW_UPDATE_ACK*)&MsgGenSetLocalFwUpdate)->MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (((GEN_SET_LOCAL_FW_UPDATE_ACK*)&MsgGenSetLocalFwUpdate)->MKHIHeader.Fields.IsResponse != 1) ||
      (((GEN_SET_LOCAL_FW_UPDATE_ACK*)&MsgGenSetLocalFwUpdate)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for FWCAPS_SET_RULE (MKHI: %08X)\n",
        ((GEN_SET_LOCAL_FW_UPDATE_ACK*)&MsgGenSetLocalFwUpdate)->MKHIHeader.Data
       ));
    return EFI_DEVICE_ERROR;
  }
  
  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to enable the ME State. The firmware allows a single
  update once it receives the enable command. Once firmware receives this message,
  the firmware will be in normal mode after a global reset.

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME enabled message sent

**/
EFI_STATUS
HeciSetMeEnableMsg (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RespLength;
  HECI_PROTOCOL                   *Heci;
  ME_ENABLE_CMD_REQ               MeEnableReq;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetMeEnableMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (MeMode == ME_MODE_NORMAL) {
    return EFI_SUCCESS;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (Status) || (MeMode != ME_MODE_TEMP_DISABLED)) {
    return EFI_UNSUPPORTED;
  }

  MeEnableReq.MkhiHeader.Data               = 0;
  MeEnableReq.MkhiHeader.Fields.Command     = ME_ENABLE_CMD;
  MeEnableReq.MkhiHeader.Fields.IsResponse  = 0;
  MeEnableReq.MkhiHeader.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  MeEnableReq.MkhiHeader.Fields.Reserved    = 0;
  MeEnableReq.MkhiHeader.Fields.Result      = 0;

  Length     = sizeof (ME_ENABLE_CMD_REQ);
  RespLength = sizeof (ME_ENABLE_CMD_RESP);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MeEnableReq,
                  Length,
                  &RespLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSetMeEnableMsg: message failed! - %r\n", Status));
	return Status;
  }
  
  if ((((ME_ENABLE_CMD_RESP*)&MeEnableReq)->MkhiHeader.Fields.Command != ME_ENABLE_CMD) ||
      (((ME_ENABLE_CMD_RESP*)&MeEnableReq)->MkhiHeader.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (((ME_ENABLE_CMD_RESP*)&MeEnableReq)->MkhiHeader.Fields.IsResponse != 1) ||
      (((ME_ENABLE_CMD_RESP*)&MeEnableReq)->MkhiHeader.Fields.Result != 0)
      ) {
    DEBUG((DEBUG_ERROR, "[ME] Invalid response received for ME_ENABLE (MKHI: %08X)\n", ((ME_ENABLE_CMD_RESP*)&MeEnableReq)->MkhiHeader.Data));
    return EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to disable the ME State. The firmware allows a single
  update once it receives the disable command Once firmware receives this message,
  the firmware will work in "Soft Temporary Disable" mode (HFS[19:16] = 3) after a
  global reset. Note, this message is not allowed when AT firmware is enrolled/configured.

  @param[in] None

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             ME is disabled

**/
EFI_STATUS
HeciSetMeDisableMsg (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            Length;
  HECI_PROTOCOL     *Heci;
  GEN_SET_FW_CAPSKU MsgMeStateControl;
  UINT32            MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetMeDisableMsg.\n"));

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (MeMode == ME_MODE_TEMP_DISABLED) {
    return EFI_SUCCESS;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgMeStateControl.MKHIHeader.Data               = 0;
  MsgMeStateControl.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgMeStateControl.MKHIHeader.Fields.Command     = FWCAPS_SET_RULE_CMD;
  MsgMeStateControl.MKHIHeader.Fields.IsResponse  = 0;
  MsgMeStateControl.Data.RuleId.Data              = 6;
  MsgMeStateControl.Data.RuleDataLen              = 1;
  MsgMeStateControl.Data.RuleData                 = 0;

  Length = sizeof (GEN_SET_FW_CAPSKU);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgMeStateControl,
                  Length,
                  &Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSetMeDisableMsg: message failed! - %r\n", Status));
  }

  if ((((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->MKHIHeader.Fields.Command != FWCAPS_SET_RULE_CMD) ||
      (((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->Data.RuleId.Data != 6) ||
      (((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->MKHIHeader.Fields.IsResponse != 1) ||
      (((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] Invalid response received for FWCAPS_SET_RULE (MKHI: %08X)\n", ((GEN_SET_FW_CAPSKU*)&MsgMeStateControl)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**

  @brief
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  Consumer or Corporate size firmware.

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
HeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA   *RuleData
  )
{
  EFI_STATUS                Status;
  UINT32                    Length;
  UINT32                    RecvLength;
  HECI_PROTOCOL             *Heci;
  GEN_GET_PLATFORM_TYPE_BUFFER    MsgGenGetPlatformType;
  UINT32                    MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetPlatformTypeMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetPlatformType.Request.MKHIHeader.Data               = 0;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetPlatformType.Request.Data.RuleId                   = 0x1D;
  Length                                                      = sizeof (GEN_GET_PLATFORM_TYPE);
  RecvLength                                                  = sizeof (GEN_GET_PLATFORM_TYPE_ACK);

  ///
  /// Send Get Platform Type Request to ME
  ///
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetPlatformType,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetPlatformTypeMsg: message failed! - %r\n", Status));
	return Status;
  }

  if ((MsgGenGetPlatformType.Response.MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (MsgGenGetPlatformType.Response.MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (MsgGenGetPlatformType.Response.MKHIHeader.Fields.IsResponse != 1) ||
      (MsgGenGetPlatformType.Response.MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n",
        ((GEN_GET_PLATFORM_TYPE_ACK*)&MsgGenGetPlatformType)->MKHIHeader.Data
       ));
    return EFI_DEVICE_ERROR;
  }

  *RuleData = MsgGenGetPlatformType.Response.Data.RuleData;

  return Status;
}

/**

  @brief
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
  OUT UINT32         *RuleData
  )
{
  EFI_STATUS                  Status;
  UINT32                      Length;
  UINT32                      RecvLength;
  HECI_PROTOCOL               *Heci;
  GEN_AMT_BIOS_SYNCH_INFO_ACK     MsgGenAmtBiosSynchInfoMsg;
 UINT32                      MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciAmtBiosSynchInfo.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Data              = 0;
  MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.IsResponse = 0;
  MsgGenAmtBiosSynchInfoMsg.RuleId                       = 0x30005;
  Length                                                 = sizeof (GEN_AMT_BIOS_SYNCH_INFO);
  RecvLength                                             = sizeof (GEN_AMT_BIOS_SYNCH_INFO_ACK);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenAmtBiosSynchInfoMsg,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciAmtBiosSynchInfo: message failed! - %r\n", Status));
	return Status;
  }

  if ((MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.IsResponse != 1) ||
      (MsgGenAmtBiosSynchInfoMsg.MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n",
        ((GEN_AMT_BIOS_SYNCH_INFO_ACK*)&MsgGenAmtBiosSynchInfoMsg)->MKHIHeader.Data
       ));
    return EFI_DEVICE_ERROR;
  }

  *RuleData = MsgGenAmtBiosSynchInfoMsg.RuleData;

  return Status;
}

/**

  @brief
  The firmware will respond to GET OEM TAG message even after the End of Post (EOP).

  @param[in] RuleData             Default is zero. Tool can create the OEM specific OEM TAG data.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
HeciGetOemTagMsg (
  OUT UINT32         *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  HECI_PROTOCOL                   *Heci;
  GEN_GET_OEM_TAG_MSG_ACK         MsgGenGetOemTagMsg;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetOemTagMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetOemTagMsg.MKHIHeader.Data              = 0;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetOemTagMsg.RuleId                       = 0x2B;
  Length                                          = sizeof (GEN_GET_OEM_TAG_MSG);
  RecvLength                                      = sizeof (GEN_GET_OEM_TAG_MSG_ACK);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetOemTagMsg,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetOemTagMsg: message failed! - %r\n", Status));
	return Status;
  }

  if ((MsgGenGetOemTagMsg.MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (MsgGenGetOemTagMsg.MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (MsgGenGetOemTagMsg.MKHIHeader.Fields.IsResponse != 1) ||
      (MsgGenGetOemTagMsg.MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n",
        ((GEN_GET_OEM_TAG_MSG_ACK*)&MsgGenGetOemTagMsg)->MKHIHeader.Data
       ));
    return EFI_DEVICE_ERROR;
  }
  
  *RuleData = MsgGenGetOemTagMsg.RuleData;

  return Status;
}
/**

  @brief
  Retrieves the current FW Register Lock State

  @param[out] LockState             FW Lock State (default, locked, unlocked)

  @exception EFI_UNSUPPORTED   Incorrect ME Mode
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetIccRegLock (
  OUT UINT8                       *LockState
  )
{
  EFI_STATUS                      Status;
  ICC_GET_REG_LOCK_BUFFER         Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetIccRegLock.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  CommandSize                         = sizeof (ICC_GET_REG_LOCK_MESSAGE);
  ResponseSize                        = sizeof (ICC_GET_REG_LOCK_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_GET_REGISTERS_LOCK_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) GetIccRegLock: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_GET_REGISTERS_LOCK_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) GetIccRegLock: Wrong response! IccHeader.Response = %X\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) GetIccRegLock: Current LockState = %X\n", Buffer.response.LockState));
  }

  if (LockState != NULL) {
    *LockState = Buffer.response.LockState;
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) GetIccRegLock: LockState = %X\n", *LockState));    
  }


  return Status;
}

/**

  @brief
  Sets ICC clock profile to be used on next and following boots

  @param[in] LockState              Requested FW LockState

  @exception EFI_UNSUPPORTED      Incorrect ME Mode/ After EOP
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciSetIccRegLock (
  IN UINT8                        LockState
  )
{
  EFI_STATUS                  Status;
  ICC_SET_REG_LOCK_BUFFER     Buffer;
  UINT32                      CommandSize;
  UINT32                      ResponseSize;
  HECI_PROTOCOL               *Heci;
  UINT32                      MeMode;
  UINT32                      MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetIccRegLock.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) SetIccRegLock\n"));

  CommandSize                         = sizeof (ICC_SET_REG_LOCK_MESSAGE);
  ResponseSize                        = sizeof (ICC_SET_REG_LOCK_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_SET_REGISTERS_LOCK_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.LockState            = LockState;
  Buffer.message.PaddingA             = 0;
  Buffer.message.PaddingB             = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "(ICC) SetIccRegLock: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_SET_REGISTERS_LOCK_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) SetIccRegLock: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  }


  return Status;
}

/**

  @brief
  retrieves the number of currently used ICC clock profile
  @param[out] Profile             number of current ICC clock profile

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciGetIccProfile (
  OUT UINT8                       *Profile,
  OUT UINT8                       *NumProfiles,
  OUT ICC_PROFILE_DESC            *ProfileDescriptions
  )
{
  EFI_STATUS              Status;
  ICC_GET_PROFILE_BUFFER  Buffer;
  UINT32                  CommandSize;
  UINT32                  ResponseSize;
  HECI_PROTOCOL           *Heci;
  UINT32                  MeMode;
  UINT32                  MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetIccProfile.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) GetIccProfile\n"));
  ///
  /// IccGetProfile response size will vary based on the amount of ICC profiles installed on the system.
  ///
  CommandSize                         = sizeof (ICC_GET_PROFILE_MESSAGE);
  ResponseSize                        = sizeof (ICC_GET_PROFILE_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_GET_PROFILE_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] (ICC) GetIccProfile: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_GET_PROFILE_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) GetIccProfile: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) GetIccProfile: Current profile = 0x%x\n", Buffer.response.IccProfileIndex));
  }

  ///
  /// Copy ICC Profile information
  ///
  if (Profile != NULL) {
    *Profile = Buffer.response.IccProfileIndex;
  }

  if (NumProfiles != NULL) {
    *NumProfiles = Buffer.response.IccNumOfProfiles;
  }

  if (ProfileDescriptions != NULL) {
    CopyMem(ProfileDescriptions, Buffer.response.IccProfileDesc, sizeof(ICC_PROFILE_DESC)*MAX_NUM_ICC_PROFILES);
  }


  return Status;
}

/**

  @brief
  Sets ICC clock profile to be used on next and following boots
  @param[in] Profile              number of profile to be used

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet

**/
EFI_STATUS
HeciSetIccProfile (
  IN UINT8 Profile
  )
{
  EFI_STATUS              Status;
  ICC_SET_PROFILE_BUFFER  Buffer;
  UINT32                  CommandSize;
  UINT32                  ResponseSize;
  HECI_PROTOCOL           *Heci;
  UINT32                  MeMode;
  UINT32                  MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetIccProfile.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) SetIccProfile\n"));

  CommandSize                         = sizeof (ICC_SET_PROFILE_MESSAGE);
  ResponseSize                        = sizeof (ICC_SET_PROFILE_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_SET_PROFILE_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.BiosIccProfile       = Profile;
  Buffer.message.PaddingA             = 0;
  Buffer.message.PaddingB             = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "(ICC) SetIccProfile: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_SET_PROFILE_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) SetIccProfile: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**

  @brief
  retrieves the number of currently used ICC clock profile

  @param[out] Profile             number of current ICC clock profile

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
  @retval EFI_INVALID_PARAMETER   IccClockSettings is NULL
**/
EFI_STATUS
HeciGetIccClockSettings (
  OUT ICC_GETSET_CLOCK_SETTINGS_REQRSP  *IccClockSettings
  )
{
  EFI_STATUS                      Status;
  ICC_GET_CLOCK_SETTINGS_BUFFER   Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetIccClockSettings.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if (IccClockSettings == NULL) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG((DEBUG_ERROR, "(ICC) ERROR: IccClockSetting is a NULL in HeciGetIccClockSettings()\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) HeciGetIccClockSettings\n"));
  CommandSize                         = sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP);
  ResponseSize                        = sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_GET_CLOCK_SETTINGS_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.ReqClock             = IccClockSettings->ReqClock;
  Buffer.message.SettingType          = IccClockSettings->SettingType;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) HeciGetIccClockSettings: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_GET_CLOCK_SETTINGS_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) HeciGetIccClockSettings: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) HeciGetIccClockSettings: Current profile = 0x%x\n"));
  }

  if (Status == EFI_SUCCESS) {
    CopyMem(IccClockSettings, &Buffer.response.Header, sizeof(ICC_GETSET_CLOCK_SETTINGS_REQRSP));
  }


  return Status;
}

/**

  @brief
  Sets ICC clock profile to be used on next and following boots

  @param[in] Profile              number of profile to be used

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           Heci device hasn't ready yet
  @retval EFI_INVALID_PARAMETER   Invalid pointe
**/
EFI_STATUS
HeciSetIccClockSettings (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP *IccClockSettings
  )
{
  EFI_STATUS                      Status;
  ICC_SET_CLOCK_SETTINGS_BUFFER   Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetIccClockSettings.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if (IccClockSettings == NULL) {
    DEBUG((DEBUG_ERROR, "[ME] (ICC) ERROR: IccClockSetting is a NULL in HeciGetIccClockSettings()\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) HeciSetIccClockSettings\n"));

  CommandSize                         = sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP);
  ResponseSize                        = sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP);

  ///
  ///  Copy input clock settings data to Heci message clock settings buffer
  ///
  CopyMem(&Buffer.message, IccClockSettings, sizeof(ICC_GETSET_CLOCK_SETTINGS_REQRSP));

  ///
  ///  Update HECI buffer header
  ///
  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_SET_CLOCK_SETTINGS_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "(ICC) HeciSetIccClockSettings: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_SET_CLOCK_SETTINGS_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "(ICC) HeciSetIccClockSettings: Wrong response! IccHeader.Response = 0x%x\n", Buffer.response.Header.IccResponse));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**

  @brief
  retrieves the Clock Range Definition Record

  @param[out] IccClockRangeRecords     Clock Range Definition Records containing 2 single CRDR

  @exception EFI_UNSUPPORTED      ICC clock profile doesn't support
  @retval EFI_NOT_READY           Heci device hasn't ready yet
**/
EFI_STATUS
HeciGetClockRangeDefinitionRecord (
  OUT ICC_CLK_RANGE_DEF_REC  *IccClockRangeRecords
  )
{
  EFI_STATUS                      Status;
  ICC_GET_CLOCK_RANGE_BUFFER      Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetClockRangeDefinitionRecord.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

    DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_INFO, "(ICC) HeciGetClockRangeDefinitionRecord\n"));
  CommandSize                         = sizeof (ICC_GET_CLOCK_RANGE_MESSAGE);
  ResponseSize                        = sizeof (ICC_GET_CLOCK_RANGE_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_GET_CLOCK_RANGE_DEF_REC_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.Mode                 = 0;
  Buffer.message.RequiredRecord       = 0;
  Buffer.message.Reserved             = 0;

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  &ResponseSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) HeciGetClockRangeDefinitionRecord: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if ((Buffer.response.Header.IccCommand != ICC_GET_CLOCK_RANGE_DEF_REC_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) HeciGetClockRangeDefinitionRecord: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  } else {
    DEBUG ((DEBUG_INFO, "[ME] "));
    DEBUG ((DEBUG_INFO, "(ICC) HeciGetClockRangeDefinitionRecord: Current profile = 0x%x\n"));
  }

  if (IccClockRangeRecords != NULL) {
    CopyMem(IccClockRangeRecords, Buffer.response.Crdr.SingleClkRangeDef, sizeof(ICC_CLK_RANGE_DEF_REC));
  }


  return Status;
}

/**

  @brief
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
  )
{
  EFI_STATUS                      Status;
  ICC_WRITE_ICC_REG_BUFFER        Buffer;
  UINT32                          CommandSize;
  UINT32                          ResponseSize;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;
  UINT32                          MeStatus;

  DEBUG ((DEBUG_INFO, "[ME] HeciWriteIccRegDword.\n"));
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if ((ResponseMode != ICC_RESPONSE_MODE_SKIP) && (ResponseMode != ICC_RESPONSE_MODE_WAIT)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
    return EFI_NOT_READY;
  }

  CommandSize                         = sizeof (ICC_WRITE_ICC_REG_DWORD_MESSAGE);
  ResponseSize                        = sizeof (ICC_WRITE_ICC_REG_DWORD_RESPONSE);

  Buffer.message.Header.ApiVersion    = SKYLAKE_PCH_PLATFORM;
  Buffer.message.Header.IccCommand    = ICC_SET_RECORD_CMD;
  Buffer.message.Header.IccResponse   = 0;
  Buffer.message.Header.BufferLength  = CommandSize - sizeof (ICC_HEADER);
  Buffer.message.Header.Reserved      = 0;
  Buffer.message.Reserved             = 0;
  Buffer.message.Reserved1            = 0;
  Buffer.message.Params               = ResponseMode;
  Buffer.message.RecordDword.RecordFlags                  = WRITE_ICC_RECORD_FLAGS;
  Buffer.message.RecordDword.BundleCount.BundlesCnt       = WRITE_ICC_REG_BUNDLE_COUNT;
  Buffer.message.RecordDword.BundleCount.AU               = 0;
  Buffer.message.RecordDword.BundleCount.Reserved         = 0;
  Buffer.message.RecordDword.AddressMask.AddressMaskData  = RegOffset | ADDRESS_MASK_FIXED_DATA;
  Buffer.message.RecordDword.RegValue                     = RegData;

  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) &Buffer,
                  CommandSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] (ICC) HeciWriteIccRegDword: message failed! - %r\n", Status));
    return Status;
  }

  if (ResponseMode == ICC_RESPONSE_MODE_WAIT) {
    Status = Heci->ReadMsg (
                    HECI1_DEVICE,
                    BLOCKING,
                    (UINT32 *) &Buffer,
                    &ResponseSize
                    );

  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG ((DEBUG_ERROR, "(ICC) HeciWriteIccRegDword: Message failed! EFI_STATUS = %r\n", Status));
    return Status;
  }

  if (ResponseMode == ICC_RESPONSE_MODE_SKIP) {
    return EFI_SUCCESS;
  }

  if ((Buffer.response.Header.IccCommand != ICC_SET_RECORD_CMD) ||
      (Buffer.response.Header.IccResponse != ICC_STATUS_SUCCESS)) {
    DEBUG ((DEBUG_ERROR, "[ME] "));
    DEBUG (
      (DEBUG_ERROR,
      "(ICC) HeciWriteIccRegDword: Wrong response! IccHeader.Response = 0x%x\n",
      Buffer.response.Header.IccResponse)
      );
    return EFI_DEVICE_ERROR;
  }

  return Status;
}


/**

  @brief
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
  IN UINT32  EnableState,
  IN UINT32  DisableState
  )
{
  EFI_STATUS                        Status;
  UINT32                            HeciLength;
  UINT32                            HeciRecvLength;
  UINT32                            MeMode;
  FIRMWARE_CAPABILITY_OVERRIDE      MngStateCmd;
  HECI_PROTOCOL                     *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciFwFeatureStateOverride.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MngStateCmd.MKHIHeader.Data               = 0;
  MngStateCmd.MKHIHeader.Fields.Command     = FIRMWARE_CAPABILITY_OVERRIDE_CMD;
  MngStateCmd.MKHIHeader.Fields.IsResponse  = 0;
  MngStateCmd.MKHIHeader.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MngStateCmd.MKHIHeader.Fields.Reserved    = 0;
  MngStateCmd.MKHIHeader.Fields.Result      = 0;
  MngStateCmd.FeatureState.EnableFeature    = EnableState;
  MngStateCmd.FeatureState.DisableFeature   = DisableState;
  HeciLength                                = sizeof (FIRMWARE_CAPABILITY_OVERRIDE);
  HeciRecvLength                            = sizeof (FIRMWARE_CAPABILITY_OVERRIDE_ACK);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &MngStateCmd,
                  HeciLength,
                  &HeciRecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciFwFeatureStateOverride: message failed! - %r\n", Status));
  }

  if ((((FIRMWARE_CAPABILITY_OVERRIDE_ACK*)&MngStateCmd)->Header.Fields.Command != FIRMWARE_CAPABILITY_OVERRIDE_CMD) ||
      (((FIRMWARE_CAPABILITY_OVERRIDE_ACK*)&MngStateCmd)->Header.Fields.GroupId != MKHI_GEN_GROUP_ID) ||
      (((FIRMWARE_CAPABILITY_OVERRIDE_ACK*)&MngStateCmd)->Header.Fields.IsResponse != 1) ||
      (((FIRMWARE_CAPABILITY_OVERRIDE_ACK*)&MngStateCmd)->Header.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FIRMWARE_CAPABILITY_OVERRIDE(MKHI: %08X)\n",
        ((FIRMWARE_CAPABILITY_OVERRIDE_ACK*)&MngStateCmd)->Header.Data
       ));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**

  @brief
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData            MEFWCAPS_SKU message

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function

**/
EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT MEFWCAPS_SKU                    *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  GEN_GET_FW_FEATURE_STATUS_ACK   GetFwFeatureStatus;
  UINT32                          MeMode;
  HECI_PROTOCOL                   *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetFwFeatureStateMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetFwFeatureStatus.MKHIHeader.Data              = 0;
  GetFwFeatureStatus.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  GetFwFeatureStatus.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  GetFwFeatureStatus.MKHIHeader.Fields.IsResponse = 0;
  GetFwFeatureStatus.RuleId                       = 0x20;
  Length                                          = sizeof (GEN_GET_FW_FEATURE_STATUS);
  RecvLength                                      = sizeof (GEN_GET_FW_FEATURE_STATUS_ACK);

  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &GetFwFeatureStatus,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetFwFeatureStateMsg: message failed! - %r\n", Status));
  }

  if ((((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.IsResponse != 1) ||
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n",
        ((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Data
       ));
    return EFI_DEVICE_ERROR;
  }
  
  RuleData->Data = GetFwFeatureStatus.RuleData.Data;

  return Status;
}

/**
  This message is sent by the BIOS to inform ME FW whether or not to take the 
  TPM 1.2 Deactivate flow

  @param[in] UINT8 TpmDeactivate  0 - ME FW should not take the
                                      deactivate flow.
                                  1 - ME FW should take the deactivate
                                      flow.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciSendTpmData (
  IN UINT8               TpmDeactivate
  )
{
  EFI_STATUS             Status;
  UINT32                 HeciSendLength;
  UINT32                 HeciRecvLength;
  HECI_PROTOCOL          *Heci;
  BIOSNV_SET_ACM_TPM     SetAcmTpmMsg;

  DEBUG ((DEBUG_INFO, "[ME] "));
  DEBUG ((DEBUG_ERROR, "HeciSendTpmData Message.  TpmDeactivate Setup Data = %d\n", TpmDeactivate));

  ZeroMem(&SetAcmTpmMsg, sizeof(BIOSNV_SET_ACM_TPM));

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  SetAcmTpmMsg.MKHIHeader.Data                          = ACM_TPM_DATA_MKHI_DATA;
  SetAcmTpmMsg.AcmTpmData.RuleId.Fields.RuleTypeId      = ACM_TPM_DATA_RULE_TYPE_ID;
  SetAcmTpmMsg.AcmTpmData.RuleDataLen                   = ACM_TPM_DATA_RULE_DATA_LENGTH;
  SetAcmTpmMsg.AcmTpmData.TpmState.Fields.TpmDeactivate = FALSE;

  if (TpmDeactivate == 1) {
    SetAcmTpmMsg.AcmTpmData.TpmState.Fields.TpmDeactivate = TRUE;
  }

  //
  // Send Set ACM TPM Data MKHI message
  //
  HeciSendLength = sizeof (BIOSNV_SET_ACM_TPM);
  HeciRecvLength = sizeof (BIOSNV_SET_ACM_TPM_ACK);
  
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &SetAcmTpmMsg,
                  HeciSendLength,
                  &HeciRecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSendTpmData: message failed! - %r\n", Status));
  }

  if ((((BIOSNV_SET_ACM_TPM_ACK*)&SetAcmTpmMsg)->MKHIHeader.Fields.Command != ((ACM_TPM_DATA_MKHI_DATA & (UINT32)(0x7F00)) >> 8)) ||
      (((BIOSNV_SET_ACM_TPM_ACK*)&SetAcmTpmMsg)->MKHIHeader.Fields.GroupId != (ACM_TPM_DATA_MKHI_DATA & (UINT32)(0xFF))) ||
      (((BIOSNV_SET_ACM_TPM_ACK*)&SetAcmTpmMsg)->MKHIHeader.Fields.IsResponse != 1) ||
      (((BIOSNV_SET_ACM_TPM_ACK*)&SetAcmTpmMsg)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for ACM_TPM_DATA_RULE (MKHI: %08X)\n", ((BIOSNV_SET_ACM_TPM_ACK*)&SetAcmTpmMsg)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }
  
  return Status;
}

/**

  @brief
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
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Length;
  UINT32                                    RespLength;
  ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_BUFFER GetMeUnconfigOnRtcClearState;  
  UINT32                                    MeMode;
  HECI_PROTOCOL                             *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciGetUnconfigOnRtcClearDisableMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetMeUnconfigOnRtcClearState.Request.MKHIHeader.Data              = 0;
  GetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  GetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  GetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.IsResponse = 0;
  GetMeUnconfigOnRtcClearState.Request.Data.RuleId                  = 0x30;

  Length = sizeof (ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE);
  RespLength = sizeof (ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK);
  
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &GetMeUnconfigOnRtcClearState,
                  Length,
                  &RespLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciGetUnconfigOnRtcClearDisableMsg: message failed! - %r\n", Status));
    return Status;
  }
  
  if ((((ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&GetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.Command != FWCAPS_GET_RULE_CMD) ||
      (((ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&GetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (((ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&GetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.IsResponse != 1) ||
      (((ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&GetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_GET_RULE (MKHI: %08X)\n", ((ME_GET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&GetMeUnconfigOnRtcClearState)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }
  
  *RuleData = GetMeUnconfigOnRtcClearState.Response.Data.RuleData;

  return Status;
}


/**

  @brief
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
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Length;
  UINT32                                    RespLength;
  ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_BUFFER SetMeUnconfigOnRtcClearState;
  UINT32                                    MeMode;
  HECI_PROTOCOL                             *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciSetUnconfigOnRtcClearDisableMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  SetMeUnconfigOnRtcClearState.Request.MKHIHeader.Data              = 0;
  SetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  SetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.Command    = FWCAPS_SET_RULE_CMD;
  SetMeUnconfigOnRtcClearState.Request.MKHIHeader.Fields.IsResponse = 0;
  SetMeUnconfigOnRtcClearState.Request.Data.RuleId                  = 0x30;
  SetMeUnconfigOnRtcClearState.Request.Data.RuleDataLen             = 4;
  SetMeUnconfigOnRtcClearState.Request.Data.RuleData                = RuleData;

  Length = sizeof (ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE);
  RespLength = sizeof (ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK);
  Status = Heci->SendwACK (
                  HECI1_DEVICE,
                  (UINT32 *) &SetMeUnconfigOnRtcClearState,
                  Length,
                  &RespLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSetUnconfigOnRtcClearDisableMsg: message failed! - %r\n", Status));
  }

  if ((((ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&SetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.Command != FWCAPS_SET_RULE_CMD) ||
      (((ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&SetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.GroupId != MKHI_FWCAPS_GROUP_ID) ||
      (((ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&SetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.IsResponse != 1) ||
      (((ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&SetMeUnconfigOnRtcClearState)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for FWCAPS_SET_RULE (MKHI: %08X)\n", ((ME_SET_UNCONFIG_ON_RTC_CLEAR_STATE_ACK*)&SetMeUnconfigOnRtcClearState)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}


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
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Length;
  UINT32                                    RespLength;
  CORE_BIOS_DONE                            CoreBiosDone;
  UINT32                                    MeMode;
  HECI_PROTOCOL                             *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciCoreBiosDoneMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Length = sizeof (CORE_BIOS_DONE);
  RespLength = sizeof (CORE_BIOS_DONE_ACK);

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  CoreBiosDone.MKHIHeader.Data              = 0;
  CoreBiosDone.MKHIHeader.Fields.GroupId    = MKHI_MCA_GROUP_ID;
  CoreBiosDone.MKHIHeader.Fields.Command    = GEN_CORE_BIOS_DONE_CMD;
  CoreBiosDone.MKHIHeader.Fields.IsResponse = 0;

  Status = Heci->SendwACK(
                  HECI1_DEVICE,
                  (UINT32 *) &CoreBiosDone,
                  Length,
                  &RespLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Core Bios Done Message: message failed! - %r\n", Status));
  }

  if ((((CORE_BIOS_DONE_ACK*)&CoreBiosDone)->MKHIHeader.Fields.Command != GEN_CORE_BIOS_DONE_CMD) ||
      (((CORE_BIOS_DONE_ACK*)&CoreBiosDone)->MKHIHeader.Fields.GroupId != MKHI_MCA_GROUP_ID) ||
      (((CORE_BIOS_DONE_ACK*)&CoreBiosDone)->MKHIHeader.Fields.IsResponse != 1) ||
      (((CORE_BIOS_DONE_ACK*)&CoreBiosDone)->MKHIHeader.Fields.Result != 0)
      ) {
	DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid response received for CORE_BIOS_DONE (MKHI: %08X)\n", ((CORE_BIOS_DONE_ACK*)&CoreBiosDone)->MKHIHeader.Data));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

