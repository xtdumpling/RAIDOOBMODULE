/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  HeciSmm.c

Abstract:

  HECI-3 SMM driver.
  TODO: Currently this protocol is used by UMA error injection only.
  TODO: The plan is to make it common for all SMM communication with ME.

--*/

#include <PiSmm.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MeTypeLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/MeHeci3Smm.h>

#include <MeHeci3.h>

#include "MeHeci3Core.h"


/*****************************************************************************
 * Local definitions.
 *****************************************************************************/


/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/
EFI_STATUS EFIAPI SmmHeciInit(IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
                              IN OUT UINT32                *pTimeout);
EFI_STATUS EFIAPI SmmHeciQueReset(IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
                                  IN OUT UINT32                *pTimeout);
EFI_STATUS EFIAPI SmmHeciQueState(IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
                                     OUT BOOLEAN               *pIsReady,
                                     OUT UINT32                *pSendQue,
                                     OUT UINT32                *pRecvQue);
EFI_STATUS EFIAPI SmmHeciRequest(IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
                                 IN OUT UINT32                *pTimeout,
                                 IN     HECI_MSG_HEADER       *pReqMsg,
                                    OUT HECI_MSG_HEADER       *pRspBuf,
                                 IN     UINT32                *pBufLen);


/*****************************************************************************
 * Variables.
 *****************************************************************************/


/*****************************************************************************
 * Public functions.
 *****************************************************************************/
/**
 * SMM HECI driver entry point.
 *
 * param[in] ImageHandle    Handle of driver image
 * param[in] pSysTable      Pointer to the system table
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
SmmHeci3EntryPoint(
  IN     EFI_HANDLE          ImageHandle,
  IN     EFI_SYSTEM_TABLE   *pSysTable)
{
  EFI_STATUS                 Status;
  BOOLEAN                    InSmm;
  EFI_SMM_BASE2_PROTOCOL    *pSmmBase;
  EFI_SMM_SYSTEM_TABLE2     *pSmst;
  SMM_ME_HECI3_PROTOCOL     *pSmmHeci;
  EFI_HANDLE                 Handle;
  
  if( !MeTypeIsSps() ){
    DEBUG((DEBUG_ERROR, "[HECI-3] SMM driver not installed because of non-SPS firmware.\n"));
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
  if (EFI_ERROR(Status))
  {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }
  //
  // Test if the entry point is running in SMM mode. If not just return.
  // In SMM mode install the HECI SMM service.
  //
  InSmm = FALSE;
  pSmmBase->InSmm(pSmmBase, &InSmm);
  if (!InSmm)
  {
    return EFI_UNSUPPORTED;
  }
  //
  // Create database record and add to database
  //
  Status = pSmmBase->GetSmstLocation(pSmmBase, &pSmst);
  if (EFI_ERROR (Status))
  {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }
  Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(*pSmmHeci), (VOID *)&pSmmHeci);
  if (EFI_ERROR(Status))
  {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }
  //
  // Initialize SMM HECI protocol data
  //
  pSmmHeci->HeciDev.Bus = ME_BUS;
  pSmmHeci->HeciDev.Dev = ME_DEV;
  pSmmHeci->HeciDev.Fun = ME_FUN_HECI3;
  pSmmHeci->HeciDev.Hidm = HECI_HIDM_MSI;
  pSmmHeci->HeciDev.Mbar = ME_HECI3_MBAR_DEFAULT;
  pSmmHeci->HeciInit = (SMM_ME_HECI3_INIT)SmmHeciInit;
  pSmmHeci->HeciQueReset = (SMM_ME_HECI3_QUE_RESET)SmmHeciQueReset;
  pSmmHeci->HeciQueState = (SMM_ME_HECI3_QUE_STATE)SmmHeciQueState;
  pSmmHeci->HeciRequest = (SMM_ME_HECI3_REQUEST)SmmHeciRequest;
  Handle = NULL;
  //
  // Install the SMM HECI API
  //
  Status = SmmHeciInit(pSmmHeci, NULL);
  Status = pSmst->SmmInstallProtocolInterface(&Handle, &gSmmMeHeci3ProtocolGuid,
                                                EFI_NATIVE_INTERFACE, pSmmHeci);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[HECI-3] ERROR: SMM driver not installed\n"));
    pSmst->SmmFreePool(pSmmHeci);
  }
  
  return Status;
} // SmmHeciEntryPoint()


/**
 * Initialize HECI interface.
 *
 * This function initializes host side of HECI interface. If timeout is
 * greater than zero it also waits until ME is ready to receive messages.
 *
 * @param[in,out] pThis     Pointer to protocol structure
 * @param[in,out] pTimeout  On input timeout in ms, on exit time left
 */
EFI_STATUS EFIAPI SmmHeciInit(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout)
{
  EFI_STATUS  Status;
  
  Status = HeciInit(&pThis->HeciDev, pTimeout);
  
  return Status;
} // SmmHeciInit()


/**
 * Reset HECI queue.
 *
 * This function resets HECI queue. If timeout is greater than zero it also
 * waits until ME is ready to receive messages.
 *
 * @param[in,out] pThis     Pointer to protocol data
 * @param[in,out] pTimeout  On input timeout in us, on exit time left
 */
EFI_STATUS EFIAPI SmmHeciQueReset(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout)
{
  EFI_STATUS  Status;
  
  Status = HeciQueReset(&pThis->HeciDev, pTimeout);
  
  return Status;
} // SmmHeciReset()


/**
 * Get HECI queue state.
 *
 * This function reads HECI queue state. It informs whether queue is ready for
 * communication, and whether there are some dwords in send or receive queue.
 * If SmmHeciRequest() is called and queue is not empty reset is done to clear
 * it. SmmHeciQueState() may be used to detect this situation and if possible
 * delay the SMM request, so that OS driver can finish its transaction.
 *
 * @param[in,out] pThis     Pointer to protocol data
 * @param[in,out] pTimeout  On input timeout in us, on exit time left
 */
EFI_STATUS EFIAPI SmmHeciQueState(
  IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
     OUT BOOLEAN               *pIsReady,
     OUT UINT32                *pSendQue,
     OUT UINT32                *pRecvQue)
{
  EFI_STATUS  Status;
  
  Status = HeciQueState(&pThis->HeciDev, pIsReady, pSendQue, pRecvQue);
  
  return Status;
} // SmmHeciQueState()


/**
 * Send request message to HECI queue, wait for response if needed.
 *
 * This function writes one message to HECI queue and - if receive buffer
 * was provided and timeout is greater than zero - waits for response message.
 * Fragmentation is not supported. Reqeust and response must be unfragmented.
 * Size of receive message buffer is given in bytes in (*pBufLen) on input.
 * On exit (*pBufLen) provides the number of bytes written to the message
 * buffer. If buffer is too short the message is truncated.
 *
 * @param[in]     pThis      Pointer to protocol data
 * @param[in,out] pTimeout   On input timeout in ms, on exit time left
 * @param[in]     pReqMsg    Request message
 * @param[out]    pMsgBuf    Buffer for the response message
 * @param[in,out] pBufLen    On input buffer size, on exit message, in bytes
 */
EFI_STATUS EFIAPI SmmHeciRequest(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout,
  IN     HECI_MSG_HEADER       *pReqMsg,
     OUT HECI_MSG_HEADER       *pRspBuf,
  IN     UINT32                *pBufLen)
{
  EFI_STATUS  Status;
  
  //
  // Send the request and wait for response if response expected
  //
  Status = HeciMsgSend(&pThis->HeciDev, pTimeout, pReqMsg);
  if (!EFI_ERROR(Status))
  {
    if (pRspBuf != NULL)
    {
      Status = HeciMsgRecv(&pThis->HeciDev, pTimeout, pRspBuf, pBufLen);
    }
  }
  return Status;
} // SmmHeciRequest()

