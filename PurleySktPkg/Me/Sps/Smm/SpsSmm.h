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

  @file SpsSmm.h

Abstract:

  Structures and macros for SPS SMM driver
**/

#ifndef SPSSMM_H_
#define SPSSMM_H_

#define HECI_RSP_STATUS_SUCCESS       0x0

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT64              Nonce;
} HECI_MSG_HMRFPO_ENABLE_REQ;

typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT32              FactoryBase;
  UINT32              FactoryLimit;
  UINT8               Status;
  UINT8               Reserved[3];
} HECI_MSG_HMRFPO_ENABLE_RSP;
#pragma pack()

#define HECI_MSG_ENABLE_REQ_MKHI_HDR  0x00000105
#define HECI_MSG_ENABLE_RSP_MKHI_HDR  0x00008105

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT64              Nonce;
} HECI_MSG_HMRFPO_MERESET_REQ;

typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  UINT8               Status;
  UINT8               Reserved[3];
} HECI_MSG_HMRFPO_MERESET_RSP;
#pragma pack()

#define HECI_MSG_MERESET_REQ_MKHI_HDR  0x00000005
#define HECI_MSG_MERESET_RSP_MKHI_HDR  0x00008005

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER   Mkhi;
} HECI_MSG_HMRFPO_LOCK_REQ;

typedef struct
{
  MKHI_MESSAGE_HEADER   Mkhi;
  UINT64                Nonce;
  UINT32                FactoryBase;
  UINT32                FactoryLimit;
  UINT8                 Status;
  UINT8                 Reserved[3];
} HECI_MSG_HMRFPO_LOCK_RSP;
#pragma pack()

#define HECI_MSG_LOCK_REQ_MKHI_HDR  0x00000205
#define HECI_MSG_LOCK_RSP_MKHI_HDR  0x00008205

#pragma pack(1)
typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
} HECI_MSG_MKHI_GET_FW_VERSION_REQ;

typedef struct
{
  MKHI_MESSAGE_HEADER Mkhi;
  MKHI_FW_VERSION     Act;
  MKHI_FW_VERSION     Rcv;
  MKHI_FW_VERSION     Bkp;
} HECI_MSG_MKHI_GET_FW_VERSION_RSP;
#pragma pack()

#define HECI_MSG_GET_FW_VERSION_REQ_MKHI_HDR   0x000002FF
#define HECI_MSG_GET_FW_VERSION_RSP_MKHI_HDR   0x000082FF

EFI_STATUS
HeciReq_HmrfpoLock(
  IN HECI_PROTOCOL *pSmmHeci,
  IN OUT UINT64    *Nonce
  );

EFI_STATUS
HeciReq_HmrfpoEnable(
  IN HECI_PROTOCOL *pSmmHeci,
  IN UINT64        Nonce
  );

EFI_STATUS
HeciReq_HmrfpoMeReset (
  IN HECI_PROTOCOL *pSmmHeci,
  IN UINT64        Nonce
  );

VOID
RetrieveMeVersions(
  IN HECI_PROTOCOL               *pSmmHeci,
  IN OUT SPS_SMM_HMRFPO_PROTOCOL *pSpsSmmHmrfpo
  );

EFI_STATUS
Hmrfpo_Lock (VOID);

EFI_STATUS
Hmrfpo_MeReset (VOID);

EFI_STATUS
Hmrfpo_Enable (VOID);

// APTIOV_SERVER_OVERRIDE_RC_START : Provide Nonce in SpsSmm.
UINT64
Hmrfpo_GetNonce(VOID);
// APTIOV_SERVER_OVERRIDE_RC_END : Provide Nonce in SpsSmm.
EFI_STATUS
SpsSmmExitBootServicesCallback(
  IN CONST EFI_GUID     *Protocol,
  IN VOID               *Interface,
  IN EFI_HANDLE         Handle
  );

#endif /* SPSSMM_H_ */
