/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeUmaEinj.c

  Library for ME UMA (MESEG) error injection support.
  This library is used by RAS.

**/
#include <PiDxe.h>
#include <PiSmm.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MeUmaEinjLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/MeHeci3Smm.h>
#include <Library/MeTypeLib.h>


/*****************************************************************************
 * Local definitions.
 *****************************************************************************/
#define EINJ_HECI_INITIMEOUT  500  // timeout in ms for initial operations
#define EINJ_HECI_TIMEOUT     10   // timeout in ms for runtime operations

#define SPS_CLIENTID_BIOS     0
#define SPS_CLIENTID_ME_EINJ  0x22 // Error injection handler in ME

#define EINJ_CMD_CONFIG  1
#define EINJ_CMD_CONSUME 2

#pragma pack(1)
typedef struct
{
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT8           Reserved[3];
} HECI_MSG_EINJ_CONFIG_REQ;

typedef struct
{ 
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT8           Status;
  UINT8           Reserved[2];
  UINT32          Offset;
} HECI_MSG_EINJ_CONFIG_RSP;
#pragma pack()
            
#pragma pack(1)
typedef struct
{
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT8           Mode;
  UINT8           Reserved[2];
} HECI_MSG_EINJ_CONSUME_REQ;

typedef struct
{ 
  HECI_MSG_HEADER Heci;
  UINT8           Cmd;
  UINT8           Status;
  UINT8           Reserved[2];
} HECI_MSG_EINJ_CONSUME_RSP;
#pragma pack()


/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/


/*****************************************************************************
 * Variables.
 *****************************************************************************/
UINT32                 mMeEinjPageOffset = (UINT32)-1;
SMM_ME_HECI3_PROTOCOL *mpMeEinjHeci = NULL;


/*****************************************************************************
 * Public functions.
 *****************************************************************************/
/**
 * ME UMA error injection library constructor.
 *
 * param[in] ImageHandle    Handle of driver image
 * param[in] pSysTable      Pointer to the system table
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
MeUmaEinjLibConstructor(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *pSysTable)
{
  EFI_STATUS                 Status;
  BOOLEAN                    InSmm;
  UINT32                     Timeout;
  UINT32                     BufLen;
  EFI_SMM_BASE2_PROTOCOL    *pSmmBase;
  EFI_SMM_SYSTEM_TABLE2     *pSmst;
  union
  {
    HECI_MSG_EINJ_CONFIG_REQ Req;
    HECI_MSG_EINJ_CONFIG_RSP Rsp;
  } HeciMsg;
  
  Status = gBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
  if (EFI_ERROR(Status))
  {
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
  if (MeTypeIsInRecovery ()) {
    DEBUG ((DEBUG_ERROR, "[SPS] ME is in recovery - exit\n"));
    return EFI_SUCCESS;
  }
  Status = pSmst->SmmLocateProtocol(&gSmmMeHeci3ProtocolGuid, NULL, &mpMeEinjHeci);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME HECI3 SMM protocol not found (%r)\n", Status));
    return EFI_UNSUPPORTED;
  }
  DEBUG((DEBUG_INFO, "[SPS] Sending HECI EINJ_CONFIG request\n"));
  HeciMsg.Req.Heci.DWord = MeHeciMsgHeader(SPS_CLIENTID_ME_EINJ, SPS_CLIENTID_BIOS,
                                           sizeof(HeciMsg.Req) - sizeof(HeciMsg.Req.Heci), 1);
  HeciMsg.Req.Cmd = EINJ_CMD_CONFIG;
  BufLen = sizeof(HeciMsg);
  Timeout = EINJ_HECI_INITIMEOUT;
  Status = mpMeEinjHeci->HeciRequest(mpMeEinjHeci, &Timeout, &HeciMsg.Req.Heci,
                                     &HeciMsg.Rsp.Heci, &BufLen);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send EINJ_CONFIG request (%r)\n", Status));
  }
  else if (BufLen < sizeof(HeciMsg.Rsp))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid EINJ_CONFIG response length %d (expect %d)\n",
                        BufLen, sizeof(HeciMsg.Rsp)));
  }
  else if (HeciMsg.Rsp.Status != 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: EINJ_CONFIG failed (status: %d)\n", HeciMsg.Rsp.Status));
  }
  else
  {
    mMeEinjPageOffset = HeciMsg.Rsp.Offset;
    DEBUG((DEBUG_INFO, "[SPS] Got ME UMA Error Injection page offset: 0x%X\n", mMeEinjPageOffset));
  }
  return EFI_SUCCESS;
} // MeUmaEinjLibConstructor()


/**
 * Read ME UMA error injection page offset.
 *
 * param[out] pMeEinjPageOffset  Pointer to a buffer for ME error injection page offset
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
MeUmaEinjPageOffset(
  OUT UINT32                *pMeEinjPageOffset)
{
  EFI_STATUS                 Status;
  UINT32                     Timeout;
  UINT32                     BufLen;
  union
  {
    HECI_MSG_EINJ_CONFIG_REQ Req;
    HECI_MSG_EINJ_CONFIG_RSP Rsp;
  } HeciMsg;
  
  if ((pMeEinjPageOffset == NULL) || (mpMeEinjHeci == NULL)) {
    DEBUG((DEBUG_ERROR, "[SPS] Error: MeUmaEinjPageOffset: Wrong input Parameters\n"));
    ASSERT(pMeEinjPageOffset != NULL);
    ASSERT(mpMeEinjHeci != NULL);
    return EFI_INVALID_PARAMETER;
  }
  
  if (mMeEinjPageOffset == (UINT32)-1)
  {
    DEBUG((DEBUG_INFO, "[SPS] Sending HECI EINJ_CONFIG request\n"));
    HeciMsg.Req.Heci.DWord = MeHeciMsgHeader(SPS_CLIENTID_ME_EINJ, SPS_CLIENTID_BIOS,
                                        (UINT32)sizeof(HeciMsg.Req) - sizeof(HeciMsg.Req.Heci), 1);
    HeciMsg.Req.Cmd = EINJ_CMD_CONFIG;
    BufLen = sizeof(HeciMsg);
    Timeout = EINJ_HECI_TIMEOUT;
    Status = mpMeEinjHeci->HeciRequest(mpMeEinjHeci, &Timeout, &HeciMsg.Req.Heci,
                                       &HeciMsg.Rsp.Heci, &BufLen);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send EINJ_CONFIG request (%r)\n", Status));
    }
    else if (BufLen < sizeof(HeciMsg.Rsp))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid EINJ_CONFIG response length %d (expect %d)\n",
                          BufLen, sizeof(HeciMsg.Rsp)));
    }
    else if (HeciMsg.Rsp.Status != 0)
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: EINJ_CONFIG failed (status: %d)\n", HeciMsg.Rsp.Status));
    }
    else
    {
      mMeEinjPageOffset = HeciMsg.Rsp.Offset;
      DEBUG((DEBUG_INFO, "[SPS] Got ME UMA Error Injection page offset: 0x%X\n", mMeEinjPageOffset));
    }
  }
  if (mMeEinjPageOffset == (UINT32)-1)
  {
    return EFI_NOT_READY;
  }
  
  *pMeEinjPageOffset = mMeEinjPageOffset;
  
  return EFI_SUCCESS;
} // MeUmaEinjPageOffset()


/**
 * Trigger ME UMA error injection page consumption.
 *
 * param[in] EinMode    ME UMA error injection mode
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
MeUmaEinjConsume(
  IN ME_UMA_EINJ_MODE         EinMode)
{
  EFI_STATUS                  Status;
  UINT32                      Timeout;
  UINT32                      BufLen;
  union
  {
    HECI_MSG_EINJ_CONSUME_REQ Req;
    HECI_MSG_EINJ_CONSUME_RSP Rsp;
  } HeciMsg;

  if (mpMeEinjHeci == NULL) {
    DEBUG((DEBUG_ERROR, "[SPS] Error: MeUmaEinjConsume: Wrong input Parameters\n"));
    ASSERT(mpMeEinjHeci != NULL);
    return EFI_INVALID_PARAMETER;
  }
    
  DEBUG((DEBUG_INFO, "[SPS] Sending HECI EINJ_CONSUME request\n"));
  HeciMsg.Req.Heci.DWord = MeHeciMsgHeader(SPS_CLIENTID_ME_EINJ, SPS_CLIENTID_BIOS,
                                           sizeof(HeciMsg.Req) - sizeof(HeciMsg.Req.Heci), TRUE);
  HeciMsg.Req.Cmd = EINJ_CMD_CONSUME;
  HeciMsg.Req.Mode = EinMode;
  BufLen = sizeof(HeciMsg);
  Timeout = EINJ_HECI_TIMEOUT;
  Status = mpMeEinjHeci->HeciRequest(mpMeEinjHeci, &Timeout, &HeciMsg.Req.Heci,
                                     &HeciMsg.Rsp.Heci, &BufLen);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send EINJ_CONSUME request (%r)\n", Status));
  }
  else if (BufLen < sizeof(HeciMsg.Rsp))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid EINJ_CONSUME response length %d (expect %d)\n",
                        BufLen, sizeof(HeciMsg.Rsp)));
    Status = EFI_PROTOCOL_ERROR;
  }
  else if (HeciMsg.Rsp.Status != 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: EINJ_CONSUME failed (status: %d)\n", HeciMsg.Rsp.Status));
    Status = EFI_ABORTED;
  }
  return Status;
} // MeUmaEinjConsume()



