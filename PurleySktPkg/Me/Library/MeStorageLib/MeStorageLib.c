/** @file
  Implements ME Storage HECI Interface Library.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
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

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>
#include <Sps.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeMeLib.h>
#include <Library/HobLib.h>
#include <Library/MeTypeLib.h>
#include <Library/MeStorageLib.h>

#include "MeStorageLibInternal.h"

static BOOLEAN gSupported = FALSE;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
#include <Guid/SpsInfoHobGuid.h>
#endif //SPS_SUPPORT

/**
  The constructor function initializes the ME Storage library

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
MeStorageLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
#if defined(SPS_SUPPORT) && SPS_SUPPORT
  EFI_HOB_GUID_TYPE    *SpsHob;
  SPS_INFO_HOB         *SpsHobData;

  if (MeTypeIsSps())
  {
    SpsHob = GetFirstGuidHob(&gSpsInfoHobGuid);
    if (SpsHob != NULL)
    {
      SpsHobData = GET_GUID_HOB_DATA(SpsHob);
      if (SpsHobData->FeatureSet.Bits.MeStorageServices)
      {
        gSupported = TRUE;
      }
    }
    DEBUG((DEBUG_INFO, "[SPS] ME Storage Service %aavailable\n", (gSupported) ? "" : "NOT "));
  }
#endif //SPS_SUPPORT

  return EFI_SUCCESS;
}

/**
  Checks whether ME Storage is supported or not.

  @returns True is returned if ME is functional and supports stoarage
           functionality, otherwise false.
 **/
BOOLEAN EFIAPI
MeStorageIsSupported(
  VOID
)
{
  return gSupported;
}

/**
  Read entry from ME storage.

  @param[in]  Index    Index of the entry to read
  @param[out] pBuffer  Buffer for the entry data
  @param[io]  pBufLen  On input length of buffer, on exit length of entry data

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_INVALID_PARAMETER  Index out of range or null pointer passed to the function
  @retval EFI_PROTOCOL_ERROR     ME reports error, detailed status in pStatus
  @retval EFI_BUFFER_TOO_SMALL   Buffer too small, entry data truncated
  @retval EFI_DEVICE_ERROR       HECI communication error
  @retval EFI_UNSUPPORTED        Implementantion not supported
 **/
EFI_STATUS EFIAPI
MeStorageEntryRead(
  IN     UINT8   Index,
     OUT UINT8  *pStatus,
     OUT VOID   *pBuffer,
  IN OUT UINT32 *pBufLen)
{
  EFI_STATUS            Status;
  HECI_PROTOCOL        *pHeci;
  UINT32                ReqLen;
  UINT32                RspLen;
  UINT32                RspHeaderSize;
  union
  {
    ME_READ_STORAGE_SERVICES_REQ MeStorageSrvReq;
    ME_READ_STORAGE_SERVICES_RSP MeStorageSrvRsp;
  } HeciMsg;

  if (Index > ME_STORAGE_MAX_INDEX ||
      pStatus == NULL ||
      pBuffer == NULL ||
      pBufLen == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (!MeTypeIsSps())
  {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (status: %r)\n", Status));
    return EFI_DEVICE_ERROR;
  }

  *pStatus = 0;
  gBS->SetMem(&HeciMsg, sizeof(HeciMsg), 0);

  ReqLen = sizeof(ME_READ_STORAGE_SERVICES_REQ);

  HeciMsg.MeStorageSrvReq.ApiFunctionSelector = SPS_ME_STORAGE_READ_STORAGE_ENTRY_CMD;
  HeciMsg.MeStorageSrvReq.Nonce = 0;
  HeciMsg.MeStorageSrvReq.StorageEntryIndex = Index;
  HeciMsg.MeStorageSrvReq.SeqNumber = 0;

  DEBUG ((DEBUG_INFO, "[SPS] MeStorageServices request: \n"));
  DEBUG ((DEBUG_INFO, "[SPS]  ApiFunctionSelector: %d\n", HeciMsg.MeStorageSrvReq.ApiFunctionSelector));
  DEBUG ((DEBUG_INFO, "[SPS]  SeqNumber: %d\n", HeciMsg.MeStorageSrvReq.SeqNumber));
  DEBUG ((DEBUG_INFO, "[SPS]  Nonce: %d\n", HeciMsg.MeStorageSrvReq.Nonce));
  DEBUG ((DEBUG_INFO, "[SPS]  StorageEntryIndex: %d\n", HeciMsg.MeStorageSrvReq.StorageEntryIndex));

  RspHeaderSize = sizeof(ME_READ_STORAGE_SERVICES_RSP) - ME_STORAGE_MAX_BUFFER;
  RspLen = RspHeaderSize + *pBufLen;

  Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32 *)&HeciMsg.MeStorageSrvReq,
                              ReqLen, &RspLen,
                              SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_STORAGE_SERVICES);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send ME Storage Service request (%r)\n", Status));
    return EFI_DEVICE_ERROR;
  }
  else if (RspHeaderSize > RspLen ||
    HeciMsg.MeStorageSrvRsp.ApiFunctionSelector != SPS_ME_STORAGE_READ_STORAGE_ENTRY_CMD ||
    HeciMsg.MeStorageSrvRsp.StorageEntryIndex != Index ||
    (HeciMsg.MeStorageSrvRsp.SeqNumber & ~SPS_ME_STORAGE_SEQ_FLAG) != 0 ||
    (HeciMsg.MeStorageSrvRsp.SeqNumber & SPS_ME_STORAGE_SEQ_FLAG) != SPS_ME_STORAGE_SEQ_FLAG)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Sending ME Storage Service failed (wrong response)\n"));
    return EFI_INVALID_PARAMETER;
  }
  else if (HeciMsg.MeStorageSrvRsp.OperationStatus != 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME Storage Service operation status: %d\n",
                        HeciMsg.MeStorageSrvRsp.OperationStatus));
    *pStatus = HeciMsg.MeStorageSrvRsp.OperationStatus;
    return EFI_PROTOCOL_ERROR;
  }
  else
  {
     DEBUG ((DEBUG_INFO, "[SPS] MeStorageServices response: Success.\n"));
  }

  RspLen -= RspHeaderSize;

  if (*pBufLen < RspLen)
  {
    return EFI_BUFFER_TOO_SMALL;
  }

  gBS->CopyMem(pBuffer, &HeciMsg.MeStorageSrvRsp.Data, (UINTN)RspLen);
  *pBufLen = RspLen;

  return EFI_SUCCESS;
}

/**
  Write entry to ME storage.

  @param[in]  Index    Index of the entry to read
  @param[in]  pBuffer  Buffer with data to store
  @param[in]  BufLen   Length of buffer data to store

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_INVALID_PARAMETER  Index out of range or null pointer passed to the function
  @retval EFI_PROTOCOL_ERROR     ME reports error, detailed status in pStatus
  @retval EFI_BAD_BUFFER_SIZE    Buffer size out of range
  @retval EFI_DEVICE_ERROR       HECI communication error
  @retval EFI_UNSUPPORTED        Implementantion not supported
 **/
EFI_STATUS EFIAPI
MeStorageEntryWrite(
  IN     UINT8   Index,
     OUT UINT8  *pStatus,
  IN     VOID   *pBuffer,
  IN     UINT32  BufLen)
{
  EFI_STATUS            Status;
  HECI_PROTOCOL        *pHeci;
  UINT32                ReqLen;
  UINT32                RspLen;
  union
  {
    ME_WRITE_STORAGE_SERVICES_REQ MeStorageSrvReq;
    ME_WRITE_STORAGE_SERVICES_RSP MeStorageSrvRsp;
  } HeciMsg;

  if (Index > ME_STORAGE_MAX_INDEX ||
      pStatus == NULL ||
      pBuffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  if (BufLen < ME_STORAGE_MIN_BUFFER ||
      BufLen > ME_STORAGE_MAX_BUFFER)
  {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (!MeTypeIsSps())
  {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (status: %r)\n", Status));
    return EFI_DEVICE_ERROR;
  }

  *pStatus = 0;
  gBS->SetMem(&HeciMsg, sizeof(HeciMsg), 0);

  ReqLen = (sizeof(ME_WRITE_STORAGE_SERVICES_REQ) - ME_STORAGE_MAX_BUFFER) + BufLen;
  ASSERT (ReqLen <= sizeof(ME_WRITE_STORAGE_SERVICES_REQ));

  gBS->CopyMem(&HeciMsg.MeStorageSrvReq.Data, pBuffer, (UINTN)BufLen);

  HeciMsg.MeStorageSrvReq.ApiFunctionSelector = SPS_ME_STORAGE_WRITE_STORAGE_ENTRY_CMD;
  HeciMsg.MeStorageSrvReq.Nonce = 0;
  HeciMsg.MeStorageSrvReq.StorageEntryIndex = Index;
  HeciMsg.MeStorageSrvReq.SeqNumber = 0;

  DEBUG ((DEBUG_INFO, "[SPS] MeStorageServices request: \n"));
  DEBUG ((DEBUG_INFO, "[SPS]  ApiFunctionSelector: %d\n", HeciMsg.MeStorageSrvReq.ApiFunctionSelector));
  DEBUG ((DEBUG_INFO, "[SPS]  SeqNumber: %d\n", HeciMsg.MeStorageSrvReq.SeqNumber));
  DEBUG ((DEBUG_INFO, "[SPS]  Nonce: %d\n", HeciMsg.MeStorageSrvReq.Nonce));
  DEBUG ((DEBUG_INFO, "[SPS]  StorageEntryIndex: %d\n", HeciMsg.MeStorageSrvReq.StorageEntryIndex));

  RspLen = sizeof(ME_WRITE_STORAGE_SERVICES_RSP);
  Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32 *)&HeciMsg.MeStorageSrvReq,
                              ReqLen, &RspLen,
                              SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_STORAGE_SERVICES);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send ME Storage Service request (%r)\n", Status));
    return EFI_DEVICE_ERROR;
  }
  else if (RspLen < sizeof(ME_WRITE_STORAGE_SERVICES_RSP) ||
    HeciMsg.MeStorageSrvRsp.ApiFunctionSelector != SPS_ME_STORAGE_WRITE_STORAGE_ENTRY_CMD ||
    HeciMsg.MeStorageSrvRsp.StorageEntryIndex != Index ||
    (HeciMsg.MeStorageSrvRsp.SeqNumber & ~SPS_ME_STORAGE_SEQ_FLAG) != 0 ||
    (HeciMsg.MeStorageSrvRsp.SeqNumber & SPS_ME_STORAGE_SEQ_FLAG) != SPS_ME_STORAGE_SEQ_FLAG)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Sending ME Storage Service failed (wrong response)\n"));
    return EFI_INVALID_PARAMETER;
  }
  else if (HeciMsg.MeStorageSrvRsp.OperationStatus != 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME Storage Service operation status: %d\n",
                        HeciMsg.MeStorageSrvRsp.OperationStatus));
    *pStatus = HeciMsg.MeStorageSrvRsp.OperationStatus;
    return EFI_PROTOCOL_ERROR;
  }
  else
  {
    DEBUG ((DEBUG_INFO, "[SPS] MeStorageServices response: Success.\n"));
  }

  return EFI_SUCCESS;
}

