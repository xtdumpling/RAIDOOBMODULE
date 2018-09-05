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

  HeciMsgLibPei.c

@brief:

  Implementation file for Heci Message functionality

**/
#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/Wdt.h>
#include <CoreBiosMsg.h>
#include <BupMsgs.h>
#include <Ppi/PchReset.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/PlatformMeHookPpi.h"
#include "Library/HeciMsgLibPei.h"
#include "MkhiMsgs.h"
#include "Library/MeTypeLib.h"
#include "HeciRegs.h"
#include "MeAccess.h"


/**
  Initialize HECI Interface.

  @retval Status Initialization Status
**/

EFI_STATUS
PeiHeciInitialize(VOID)
{
  EFI_STATUS   Status;
  HECI_PPI    *HeciPpi;

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &HeciPpi);
  if (!EFI_ERROR(Status)) {
    Status = HeciPpi->InitializeHeci(HECI1_DEVICE);
  }

  return Status;
}

/**
  Send DRAM init done message through HECI to inform ME of memory initialization done.

  @param[in] DidInput             ME UMA data
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
{
  EFI_STATUS              Status;
  HECI_PPI                *HeciPpi;
  DRAM_INIT_DONE_CMD_REQ  DidRequest;
  UINT32                  ReqLength;
  UINT32                  RespLength;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  DidRequest.MkhiHeader.Data               = 0;
  DidRequest.MkhiHeader.Fields.Command     = DRAM_INIT_DONE_CMD;
  DidRequest.MkhiHeader.Fields.IsResponse  = 0;
  DidRequest.MkhiHeader.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  DidRequest.MkhiHeader.Fields.Reserved    = 0;
  DidRequest.MkhiHeader.Fields.Result      = 0;
  DidRequest.DIDData.ul                    = DidInput;

  ReqLength  = sizeof (DRAM_INIT_DONE_CMD_REQ);
  RespLength = sizeof (DRAM_INIT_DONE_CMD_RESP);

  Status = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &DidRequest,
                      ReqLength,
                      &RespLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if ((((DRAM_INIT_DONE_CMD_RESP*)&DidRequest)->MkhiHeader.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (((DRAM_INIT_DONE_CMD_RESP*)&DidRequest)->MkhiHeader.Fields.Command != DRAM_INIT_DONE_CMD) ||
      (((DRAM_INIT_DONE_CMD_RESP*)&DidRequest)->MkhiHeader.Fields.IsResponse != 1) ||
      (((DRAM_INIT_DONE_CMD_RESP*)&DidRequest)->MkhiHeader.Fields.Result != ICC_STATUS_SUCCESS)) {
    return EFI_DEVICE_ERROR;
  }

  *BiosAction = ((DRAM_INIT_DONE_CMD_RESP*)&DidRequest)->BiosAction;

  return Status;
}


/**
  Send Tracing Enable request to CSME via HECI.

  @param[in] VOID

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciTracingEnableRequest (
  VOID
  )
{
  EFI_STATUS                  Status;
  HECI_PPI                    *HeciPpi;
  TRACING_ENABLE_CMD_REQ      TracingEnableReq;
  UINT32                      ReqLength;
  UINT32                      RespLength;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  TracingEnableReq.MkhiHeader.Data               = 0;
  TracingEnableReq.MkhiHeader.Fields.Command     = TRACING_ENABLE_CMD;
  TracingEnableReq.MkhiHeader.Fields.IsResponse  = 0;
  TracingEnableReq.MkhiHeader.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  TracingEnableReq.MkhiHeader.Fields.Reserved    = 0;
  TracingEnableReq.MkhiHeader.Fields.Result      = 0;
  
  ReqLength  = sizeof (TRACING_ENABLE_CMD_REQ);
  RespLength = sizeof (TRACING_ENABLE_CMD_RESP);

  Status = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &TracingEnableReq,
                      ReqLength,
                      &RespLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  if (EFI_ERROR(Status) ||
      (((TRACING_ENABLE_CMD_RESP*)&TracingEnableReq)->MkhiHeader.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (((TRACING_ENABLE_CMD_RESP*)&TracingEnableReq)->MkhiHeader.Fields.Command != TRACING_ENABLE_CMD) ||
      (((TRACING_ENABLE_CMD_RESP*)&TracingEnableReq)->MkhiHeader.Fields.IsResponse != 1) ||
      (((TRACING_ENABLE_CMD_RESP*)&TracingEnableReq)->MkhiHeader.Fields.Result != ICC_STATUS_SUCCESS)) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


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
{
  EFI_STATUS     Status;
  HECI_PPI       *HeciPpi;
  ICC_CMD_REQ    IccReq;
  UINT32         ReqLength;
  UINT32         RespLength;

  DEBUG ((DEBUG_INFO, "(ICC) PeiHeciIccBclkMsg\n"));

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  IccReq.MkhiHeader.Data               = 0;
  IccReq.MkhiHeader.Fields.Command     = ICC_CMD;
  IccReq.MkhiHeader.Fields.IsResponse  = 0;
  IccReq.MkhiHeader.Fields.GroupId     = BUP_ICC_GROUP_ID;
  IccReq.MkhiHeader.Fields.Reserved    = 0;
  IccReq.MkhiHeader.Fields.Result      = 0;
  IccReq.Data.BClkRampOnWarmResetEn    = 0;

  ReqLength  = sizeof (ICC_CMD_REQ);
  RespLength = sizeof (ICC_CMD_RESP);

  Status = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &IccReq,
                      ReqLength,
                      &RespLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  if (EFI_ERROR(Status) ||
      (((ICC_CMD_RESP*)&IccReq)->MkhiHeader.Fields.GroupId != BUP_ICC_GROUP_ID) ||
      (((ICC_CMD_RESP*)&IccReq)->MkhiHeader.Fields.Command != ICC_CMD) ||
      (((ICC_CMD_RESP*)&IccReq)->MkhiHeader.Fields.IsResponse != 1) ||
      (((ICC_CMD_RESP*)&IccReq)->MkhiHeader.Fields.Result != ICC_STATUS_SUCCESS)) {
    Status = EFI_DEVICE_ERROR;
  }

  if (((ICC_CMD_RESP*)&IccReq)->Data.FwNeedsWarmResetFlag == 0x1) {
    *WarmResetRequired = 1;
  }

  return Status;
}


/**
  Send HSIO request through HECI to get the HSIO settings version on CSME side.

  @param[in]  BiosCmd              HSIO command: 0 - close interface, 1 - report HSIO version
  @param[out] Crc                  CRC16 of ChipsetInit Table

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciHsioMsg (
  IN  UINT32                      BiosCmd,
  OUT UINT16                      *Crc
  )
{
  EFI_STATUS     Status;
  HECI_PPI       *HeciPpi;
  HSIO_CMD_REQ   HsioRequest;
  UINT32         ReqLength;
  UINT32         RespLength;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  HsioRequest.MkhiHeader.Data               = 0;
  HsioRequest.MkhiHeader.Fields.Command     = HSIO_CMD;
  HsioRequest.MkhiHeader.Fields.IsResponse  = 0;
  HsioRequest.MkhiHeader.Fields.GroupId     = BUP_HSIO_GROUP_ID;
  HsioRequest.MkhiHeader.Fields.Reserved    = 0;
  HsioRequest.MkhiHeader.Fields.Result      = 0;

  ReqLength  = sizeof (HSIO_CMD_REQ);
  RespLength = sizeof (HSIO_CMD_RESP);

  Status = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &HsioRequest,
                      ReqLength,
                      &RespLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );
  if (EFI_ERROR(Status) || 
      (((HSIO_CMD_RESP*)&HsioRequest)->MkhiHeader.Fields.GroupId != BUP_HSIO_GROUP_ID) ||
      (((HSIO_CMD_RESP*)&HsioRequest)->MkhiHeader.Fields.Command != HSIO_CMD) ||
      (((HSIO_CMD_RESP*)&HsioRequest)->MkhiHeader.Fields.IsResponse != 1) ||
      (((HSIO_CMD_RESP*)&HsioRequest)->MkhiHeader.Fields.Result != ICC_STATUS_SUCCESS)) {
    Status = EFI_DEVICE_ERROR;
  }

  *Crc = ((HSIO_CMD_RESP*)&HsioRequest)->Data.Crc;

  return Status;
}

/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] VOID

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciNpcrNotificationRequest (
  VOID
  )
{
  EFI_STATUS                  Status;
  HECI_PPI                    *HeciPpi;
  NPCR_NOTIFICATION_CMD_REQ   NpcrNotificationReq;
  UINT32                      ReqLength;
  UINT32                      RespLength;
  WDT_PPI                     *WdtPpi;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  NpcrNotificationReq.MkhiHeader.Data               = 0;
  NpcrNotificationReq.MkhiHeader.Fields.Command     = NPCR_NOTIFICATION_CMD;
  NpcrNotificationReq.MkhiHeader.Fields.IsResponse  = 0;
  NpcrNotificationReq.MkhiHeader.Fields.GroupId     = BUP_PM_GROUP_ID;
  NpcrNotificationReq.MkhiHeader.Fields.Reserved    = 0;
  NpcrNotificationReq.MkhiHeader.Fields.Result      = 0;

  ReqLength  = sizeof (NPCR_NOTIFICATION_CMD_REQ);
  RespLength = sizeof (NPCR_NOTIFICATION_CMD_RESP);

  Status = PeiServicesLocatePpi (
            &gWdtPpiGuid,              // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &WdtPpi         // PPI
            );
  ASSERT_EFI_ERROR (Status);
  WdtPpi->AllowKnownReset ();

  Status = HeciPpi->SendwAck (
                      HECI1_DEVICE,
                      (UINT32 *) &NpcrNotificationReq,
                      ReqLength,
                      &RespLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  if (EFI_ERROR(Status) ||
      (((NPCR_NOTIFICATION_CMD_RESP*)&NpcrNotificationReq)->MkhiHeader.Fields.GroupId != BUP_PM_GROUP_ID) ||
      (((NPCR_NOTIFICATION_CMD_RESP*)&NpcrNotificationReq)->MkhiHeader.Fields.Command != NPCR_NOTIFICATION_CMD) ||
      (((NPCR_NOTIFICATION_CMD_RESP*)&NpcrNotificationReq)->MkhiHeader.Fields.IsResponse != 1) ||
      (((NPCR_NOTIFICATION_CMD_RESP*)&NpcrNotificationReq)->MkhiHeader.Fields.Result != ICC_STATUS_SUCCESS)) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


//
// Interface functions of HeciMsgLib
//

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
  )
{
  HECI_PPI                       *HeciPpi;
  EFI_STATUS                     Status;
  UINT32                         ReqSize;
  PCH_RESET_PPI                  *PchResetPpi;
  HSIO_WRITE_SETTINGS_REQ        *HsioWriteSettingsReqPtr;

  DEBUG ((DEBUG_INFO, "PeiHeciChipsetInitSyncMsg(0x%08X, %d): Start\n", ChipsetInitTable, ChipsetInitTableLen));
  if(ChipsetInitTableLen > 4096) {
    ASSERT(ChipsetInitTableLen <= 4096);  // ChipsetInit table should not get too large
    return EFI_DEVICE_ERROR;
  }  

  if (MeTypeIsDisabled()) {
    DEBUG ((DEBUG_WARN, "(HSIO) WARNING: ME disabled - HSIO update is not sent.\n"));
    return EFI_UNSUPPORTED;
  }


  // When SPS or ME11+ is in recovery mode do not sent HSIO
  if (MeTypeIsInRecovery()) {
    DEBUG ((DEBUG_WARN, "(HSIO) WARNING: ME in recovery mode does not support HSIO sync\n"));
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = PeiServicesLocatePpi (
                    &gPchResetPpiGuid,
                    0,
                    NULL,
                    (VOID **) &PchResetPpi
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "(HSIO) ERROR: \n"));
    DEBUG ((DEBUG_ERROR, "PchResetPpi not found.\n"));
    return Status;
  }

  //
  // Allocate a buffer for the Request Structure and the ChipsetInit Table
  //
  ReqSize = sizeof (HSIO_WRITE_SETTINGS_REQ) + ChipsetInitTableLen;
  HsioWriteSettingsReqPtr = AllocateZeroPool (ReqSize);
  if (HsioWriteSettingsReqPtr == NULL) {
    DEBUG ((DEBUG_ERROR, "(HSIO) PeiHeciChipsetInitSyncMsg: Could not allocate Memory\n"));
    return EFI_ABORTED;
  }
  //
  // Setup the HECI message for a HSIO Write
  //
  HsioWriteSettingsReqPtr->Header.ApiVersion   = SKYLAKE_PCH_PLATFORM;
  HsioWriteSettingsReqPtr->Header.IccCommand   = ICC_SET_HSIO_SETTINGS_CMD;
  HsioWriteSettingsReqPtr->Header.BufferLength = ReqSize - sizeof (ICC_HEADER);  
  CopyMem (HsioWriteSettingsReqPtr+1, ChipsetInitTable, ChipsetInitTableLen);

  //
  // Send ChipsetInit Table to ME
  //
  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *)HsioWriteSettingsReqPtr,
                  ReqSize,
                  &ReqSize,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_ICC_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "(HSIO) ERROR: Write HSIO Settings Message failed! EFI_STATUS = %r\n", Status));
  }
  else if (HsioWriteSettingsReqPtr->Header.IccResponse != ICC_STATUS_SUCCESS ||
           HsioWriteSettingsReqPtr->Header.IccCommand != ICC_SET_HSIO_SETTINGS_CMD) {
    DEBUG ((DEBUG_ERROR,"(HSIO) ERROR: Write HSIO Settings failed!: FW Response=0x%x\n",HsioWriteSettingsReqPtr->Header.IccResponse));
    Status = EFI_DEVICE_ERROR;
  }

  FreePool (HsioWriteSettingsReqPtr);

  if (!EFI_ERROR(Status) && PchResetPpi != NULL) {
    DEBUG ((DEBUG_ERROR, "PeiHeciChipsetInitSyncMsg(): Reset required for ChipsetInit Settings synch\n"));
    PchResetPpi->Reset (PchResetPpi, ColdReset);
  }

  DEBUG ((DEBUG_INFO, "PeiHeciChipsetInitSyncMsg(): End\n"));
  return Status;
}


#if AMT_SUPPORT
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
  IN BOOLEAN                   SkipMbp
  )
{
  EFI_STATUS     Status;
  MBP_CMD_REQ    *MsgGetMbp;
  MBP_CMD_RESP   *MsgGetMbpAck;
  UINT8          MsgGetMbpAckBuffer[sizeof(MBP_CMD_RESP) + MAX_MBP_SIZE - 1];
  UINT32         Length;
  UINT32         RecvLength;
  HECI_PPI       *HeciPpi;

  ZeroMem(MsgGetMbpAckBuffer, sizeof(MsgGetMbpAckBuffer));

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  MsgGetMbp                                = (MBP_CMD_REQ*) &MsgGetMbpAckBuffer;
  MsgGetMbp->MkhiHeader.Data               = 0;
  MsgGetMbp->MkhiHeader.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  MsgGetMbp->MkhiHeader.Fields.Command     = MBP_CMD;
  MsgGetMbp->MkhiHeader.Fields.IsResponse  = 0;
  MsgGetMbp->SkipMbp                       = SkipMbp;
  Length                                   = sizeof (MBP_CMD_REQ);
  RecvLength                               = sizeof (MBP_CMD_RESP) + MAX_MBP_SIZE - 1;

  ///
  /// Send Get MBP Request to ME
  ///
  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *) MsgGetMbp,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  MsgGetMbpAck = (MBP_CMD_RESP*)MsgGetMbpAckBuffer;
  DEBUG((DEBUG_ERROR, "ReadMsg returned %r\n", Status));
  DEBUG((DEBUG_ERROR, "MsgGetMbpAck->MkhiHeader.Fields.Command = %d\n", MsgGetMbpAck->MkhiHeader.Fields.Command));
  DEBUG((DEBUG_ERROR, "MsgGetMbpAck->MkhiHeader.Fields.IsResponse = %d\n", MsgGetMbpAck->MkhiHeader.Fields.IsResponse));
  DEBUG((DEBUG_ERROR, "MsgGetMbpAck->MkhiHeader.Fields.Result = %d\n", MsgGetMbpAck->MkhiHeader.Fields.Result));

  if (!EFI_ERROR (Status) && !SkipMbp &&
      ((MsgGetMbpAck->MkhiHeader.Fields.Command) == MBP_CMD) &&
      ((MsgGetMbpAck->MkhiHeader.Fields.IsResponse) == 1) &&
      (MsgGetMbpAck->MkhiHeader.Fields.Result == 0)
      ) {
    CopyMem(MbpHeader, &MsgGetMbpAck->MbpHeader, sizeof(MBP_HEADER));
    CopyMem(MbpItems, &MsgGetMbpAck->MbpItems, RecvLength - sizeof(MBP_HEADER) - sizeof(MKHI_MESSAGE_HEADER));
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

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
{
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPS_SKU_BUFFER      MsgGenGetFwCapsSku;
  UINT32                          Length;
  UINT32                          RecvLength;
  HECI_PPI                        *HeciPpi;
  UINT32                          MeMode;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = HeciPpi->GetMeMode (&MeMode);
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
  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetFwCapsSku,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (!EFI_ERROR (Status) && ((MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Command) == FWCAPS_GET_RULE_CMD) &&
      ((MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.GroupId) == MKHI_FWCAPS_GROUP_ID) &&
      (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.IsResponse == 1) &&
      (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.IsResponse == 1) &&
      (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Result == 0)
      ) {
    *FwCapsSku = MsgGenGetFwCapsSku.Response.Data.FWCapSku;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

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
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  GEN_GET_FW_FEATURE_STATUS_ACK   GetFwFeatureStatus;
  UINT32                          MeMode;
  HECI_PPI                        *HeciPpi;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = HeciPpi->GetMeMode (&MeMode);
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

  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *) &GetFwFeatureStatus,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );

  if (!EFI_ERROR(Status) &&
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.IsResponse == 1) &&
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.GroupId == MKHI_FWCAPS_GROUP_ID) &&
      (((GEN_GET_FW_FEATURE_STATUS_ACK*)&GetFwFeatureStatus)->MKHIHeader.Fields.Command == FWCAPS_GET_RULE_CMD)) {
    RuleData->Data = GetFwFeatureStatus.RuleData.Data;
  }
  else
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

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
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  HECI_PPI                        *HeciPpi;
  GEN_GET_PLATFORM_TYPE_BUFFER    MsgGenGetPlatformType;
  UINT32                          MeMode;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = HeciPpi->GetMeMode (&MeMode);
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
  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *) &MsgGenGetPlatformType,
                  Length,
                  &RecvLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (!EFI_ERROR(Status) &&
      (((GEN_GET_PLATFORM_TYPE_ACK*)&MsgGenGetPlatformType)->MKHIHeader.Fields.IsResponse == 1) &&
      (((GEN_GET_PLATFORM_TYPE_ACK*)&MsgGenGetPlatformType)->MKHIHeader.Fields.GroupId == MKHI_FWCAPS_GROUP_ID) &&
      (((GEN_GET_PLATFORM_TYPE_ACK*)&MsgGenGetPlatformType)->MKHIHeader.Fields.Command == FWCAPS_GET_RULE_CMD)) {
    *RuleData = MsgGenGetPlatformType.Response.Data.RuleData;
  }
  else
  {    
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

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
{
  HECI_PPI                       *HeciPpi;
  EFI_STATUS                     Status;
  UINT32                         HeciLength;
  UINT32                         AckLength;
  CBM_RESET_REQ                  CbmResetRequest;
  UINT32                         MeMode;
  UINT32                         Result;

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,            // GUID
            0,                        // INSTANCE
            NULL,                     // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = HeciPpi->GetMeMode (&MeMode);
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
  AckLength = sizeof (CBM_RESET_ACK);

  Status = HeciPpi->SendwAck (
                  HECI1_DEVICE,
                  (UINT32 *) &CbmResetRequest,
                  HeciLength,
                  &AckLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PeiHeciSendCbmResetRequest() - Unable to Send Reset Request - %r\n", Status));
    return EFI_DEVICE_ERROR;
  }

  if ((((CBM_RESET_ACK*)&CbmResetRequest)->MKHIHeader.Fields.IsResponse != 1) ||
      (((CBM_RESET_ACK*)&CbmResetRequest)->MKHIHeader.Fields.GroupId != MKHI_CBM_GROUP_ID) ||
      (((CBM_RESET_ACK*)&CbmResetRequest)->MKHIHeader.Fields.Command != CBM_RESET_REQ_CMD)) {
    DEBUG ((DEBUG_ERROR, "PeiHeciSendCbmResetRequest() - Wrong response format\n"));
    return EFI_DEVICE_ERROR;
  }

  Result = ((CBM_RESET_ACK*)&CbmResetRequest)->MKHIHeader.Fields.Result;
  if (Result != 0) {
    DEBUG ((DEBUG_ERROR, "PeiHeciSendCbmResetRequest() - Result = %r\n", Result));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

#endif  // AMT_SUPPORT

