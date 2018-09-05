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

#ifndef _ME_STORAGE_LIB_INTERNAL_H_
#define _ME_STORAGE_LIB_INTERNAL_H_

#define SPS_CLIENTID_ME_STORAGE_SERVICES   0x23

#define SPS_ME_STORAGE_SEQ_MAX             0x7F
#define SPS_ME_STORAGE_SEQ_FLAG            0x80

typedef enum {
  SPS_ME_STORAGE_READ_STORAGE_ENTRY_CMD      = 0,
  SPS_ME_STORAGE_WRITE_STORAGE_ENTRY_CMD     = 1,
  SPS_ME_STORAGE_SET_STORAGE_ENTRY_ATRIB_CMD = 2,
  SPS_ME_STORAGE_GET_STORAGE_ENTRY_ATRIB_CMD = 3
} ME_STORAGE_CMDS;

#pragma pack(1)
typedef struct {
  UINT8           ApiFunctionSelector;
  UINT8           SeqNumber;
  UINT64          Nonce;
  UINT8           StorageEntryIndex;
} ME_READ_STORAGE_SERVICES_REQ;

typedef struct {
  UINT8           ApiFunctionSelector;
  UINT8           SeqNumber;
  UINT8           OperationStatus;
  UINT8           StorageEntryIndex;
  UINT8           Data[ME_STORAGE_MAX_BUFFER];
} ME_READ_STORAGE_SERVICES_RSP;

typedef struct {
  UINT8           ApiFunctionSelector;
  UINT8           SeqNumber;
  UINT64          Nonce;
  UINT8           StorageEntryIndex;
  UINT8           Data[ME_STORAGE_MAX_BUFFER];
} ME_WRITE_STORAGE_SERVICES_REQ;

typedef struct {
  UINT8           ApiFunctionSelector;
  UINT8           SeqNumber;
  UINT8           OperationStatus;
  UINT8           StorageEntryIndex;
} ME_WRITE_STORAGE_SERVICES_RSP;
#pragma pack()

#endif // _ME_STORAGE_LIB_INTERNAL_H_
