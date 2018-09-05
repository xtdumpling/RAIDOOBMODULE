/** @file
  ME Storage Library

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
#ifndef _ME_STORAGE_LIB_H_
#define _ME_STORAGE_LIB_H_

#define ME_STORAGE_MAX_INDEX  255 // Max storage entry index
#define ME_STORAGE_MIN_BUFFER   1 // Min storage entry buffer length
#define ME_STORAGE_MAX_BUFFER 512 // Max storage entry buffer length

/**
  Checks whether ME Storage is supported or not.

  @returns True is returned if ME is functional and supports stoarage
           functionality, otherwise false.
 **/
BOOLEAN EFIAPI
MeStorageIsSupported(
  VOID
);

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
 **/
EFI_STATUS EFIAPI
MeStorageEntryRead(
  IN     UINT8   Index,
     OUT UINT8  *pStatus,
     OUT VOID   *pBuffer,
  IN OUT UINT32 *pBufLen);

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
 **/
EFI_STATUS EFIAPI
MeStorageEntryWrite(
  IN     UINT8   Index,
     OUT UINT8  *pStatus,
  IN     VOID   *pBuffer,
  IN     UINT32  BufLen);

#endif // _ME_STORAGE_LIB_H_

