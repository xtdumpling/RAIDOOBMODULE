/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SpsRasNotify.c

  Library for SPS RAS Notification support.
  This library is used by RAS.

**/
#include <PiDxe.h>
#include <PiSmm.h>
#include <Library/UefiLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/MeHeci3Smm.h>
#include <Protocol/HeciProtocol.h>

#include <MkhiMsgs.h>
#include <Sps.h>
#include <Guid/SpsInfoHobGuid.h>
#include <Protocol/SpsSmmHmrfpoProtocol.h>
#include <SpsSmm.h>
#include <SpsRasNotify.h>


/*****************************************************************************
 * Local definitions.
 *****************************************************************************/
#define SPS_CLIENTID_ME                             0x22 // RAS Notification handler in ME

#define RAS_NOTIFY_REQ                              0x03
#define RAS_NOTIFY_RSP                              0x83

#pragma pack(1)
typedef struct
{
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT64          Nonce;
  UINT8           NotificationType;
} HECI_RAS_NOTIFY_REQ;

typedef struct
{
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT64          Nonce;
  UINT8           Status;
} HECI_RAS_NOTIFY_RSP;
#pragma pack()

/*****************************************************************************
 * Variables.
 *****************************************************************************/
extern UINT64 gNonceValue;

/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/

/**
 * Get HECI 3 protocol.
 *
 * param[in] Heci3Protocol  HECI 3 protocol
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
GetHeci3(
  SMM_ME_HECI3_PROTOCOL **Heci3Protocol
  )
{
  EFI_STATUS                 Status = EFI_INVALID_PARAMETER;
  BOOLEAN                    InSmm;
  EFI_SMM_BASE2_PROTOCOL    *pSmmBase;
  EFI_SMM_SYSTEM_TABLE2     *pSmst;

  DEBUG((DEBUG_INFO, "[SPS_SMM] GetHeci3() called\n"));
  if (Heci3Protocol == NULL) 
  {
    return Status;
  }
  
  Status = gBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: Can't locate gEfiSmmBase2ProtocolGuid\n"));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  //
  // Test if we are running in SMM mode. If not just return.
  // In SMM mode read the .
  //
  InSmm = FALSE;
  pSmmBase->InSmm(pSmmBase, &InSmm);
  if (!InSmm)
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: GetHeci3() called in non SMM mode\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Create database record and add to database
  //
  Status = pSmmBase->GetSmstLocation(pSmmBase, &pSmst);
  if (!EFI_ERROR (Status))
  {
    Status = pSmst->SmmLocateProtocol(&gSmmMeHeci3ProtocolGuid, NULL, Heci3Protocol);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: ME HECI3 SMM protocol not found (%r)\n", Status));
    }
  }

  return Status;
} // GetHeci3()

/**
 * Send RAS notification, wait for ACK.
 *
 * param[in] RasNotificationType    ME UMA error injection mode
 * param[in] Nonce                  Nonce that was  received in previous
 *                                  HMRFPO_LOCK response message
 *
 * return EFI_STATUS is returned.
 *
 *  EFI_PROTOCOL_ERROR    - invalid Nonce or response length
 *
 *  EFI_DEVICE_ERROR      - a persistent ME FW error preventing
 *                          the ME FW from fulfilling the request
 *  EFI_ABORTED           - general error
 *
 */
EFI_STATUS
EFIAPI
MeRasNotifyAck(
  IN RAS_NOTIFICATION_TYPE   RasNotificationType)
{
  EFI_STATUS                  Status;
  UINT32                      Timeout;
  UINT32                      BufLen;
  SMM_ME_HECI3_PROTOCOL      *pMeHeci3 = NULL;
  union
  {
    HECI_RAS_NOTIFY_REQ Req;
    HECI_RAS_NOTIFY_RSP Rsp;
  } HeciMsg;

  DEBUG((DEBUG_INFO, "[SPS_SMM] Sending RAS Notification (%d)\n", RasNotificationType));

  Status = GetHeci3(&pMeHeci3);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  HeciMsg.Req.Heci.DWord = MeHeciMsgHeader(SPS_CLIENTID_ME, SPS_CLIENTID_BIOS,
                                           sizeof(HeciMsg.Req) - sizeof(HeciMsg.Req.Heci), TRUE);
  HeciMsg.Req.Cmd = RAS_NOTIFY_REQ;
  HeciMsg.Req.Nonce = gNonceValue;
  HeciMsg.Req.NotificationType = (UINT8)RasNotificationType;
  BufLen = sizeof(HeciMsg);
  Timeout = HECI_RAS_TIMEOUT;
  Status = pMeHeci3->HeciRequest(pMeHeci3, &Timeout, &HeciMsg.Req.Heci,
                                 &HeciMsg.Rsp.Heci, &BufLen);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: Cannot send RAS_NOTIFY request (%r)\n", Status));
  }
  else if (BufLen < sizeof(HeciMsg.Rsp))
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: Invalid RAS_NOTIFY response length %d (expect %d)\n",
                        BufLen, sizeof(HeciMsg.Rsp)));
    Status = EFI_PROTOCOL_ERROR;
  }
  else if (HeciMsg.Rsp.Status != 0)
  {
    Status = EFI_ABORTED;
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: RAS_NOTIFY failed (status: %d) - ", HeciMsg.Rsp.Status));
    if (HeciMsg.Rsp.Status == 1)
    {
      DEBUG((DEBUG_ERROR, "invalid NONCE\n"));
      Status = EFI_PROTOCOL_ERROR;
    }
    else if (HeciMsg.Rsp.Status == 2)
    {
      DEBUG((DEBUG_ERROR, "request rejected\n"));
      Status = EFI_DEVICE_ERROR;
    }
    else
    {
      DEBUG((DEBUG_ERROR, "unknown error\n"));
    }
  }
  else if (HeciMsg.Rsp.Nonce != gNonceValue)
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: RAS_NOTIFY response has wrong NONCE (HeciMsg.Rsp.Nonce: %d) - ", HeciMsg.Rsp.Nonce));
    Status = EFI_PROTOCOL_ERROR;
  }

  return Status;
} // MeRasNotifyAck()


/*****************************************************************************
 * Public functions.
 *****************************************************************************/

/**
 * Install gSpsRasNotifyProtocolGuid
 *
 * param none
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
InstallSpsRasNotifyProtocol(
  VOID
  )
{
  EFI_STATUS                  Status;
  SMM_ME_HECI3_PROTOCOL      *pMeHeci3 = NULL;
  SPS_RAS_NOTIFY_PROTOCOL    *pSpsRasNotifyProtocol = NULL;
  EFI_HANDLE                  Handle;

  DEBUG((DEBUG_INFO, "[SPS_SMM] Install gSpsRasNotifyProtocolGuid\n"));
 
  // Check if we can install gSpsRasNotifyProtocolGuid
  Status = GetHeci3(&pMeHeci3);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: gSpsRasNotifyProtocolGuid can't be installed\n"));
    return Status;
  }

  Status = gSmst->SmmAllocatePool( EfiRuntimeServicesData, sizeof(SPS_RAS_NOTIFY_PROTOCOL), (VOID *)&pSpsRasNotifyProtocol );
  if(EFI_ERROR(Status)){
    DEBUG((DEBUG_ERROR, "[SPS_SMM] ERROR: Can't allocate memory for pSpsRasNotifyProtocol protocol!. Status=%r\n", Status));
    return Status;
  }

  if (gNonceValue == 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS SMM] ERROR: Nonce == 0, Don't install gSpsRasNotifyProtocolGuid\n"));
    Status = EFI_PROTOCOL_ERROR;
  }
  else
  {
    pSpsRasNotifyProtocol->RasNotify = MeRasNotifyAck;

    Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gSpsRasNotifyProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      pSpsRasNotifyProtocol
                      );

    DEBUG((DEBUG_INFO, "[SPS_SMM] gSpsRasNotifyProtocolGuid installed with status %r\n", Status));
    ASSERT_EFI_ERROR (Status);
  }

  if (EFI_ERROR(Status) && (pSpsRasNotifyProtocol != NULL))
  {
    gSmst->SmmFreePool (pSpsRasNotifyProtocol);
  }

  return Status;
}