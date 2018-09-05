//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Frb.h

  This file abstracts FRB Protocol.

---------------------------------------------------------------**/

#ifndef _EFI_FRB_H_
#define _EFI_FRB_H_

#include "Guid/FrbGuid.h"

typedef struct _EFI_SM_FRB_PROTOCOL EFI_SM_FRB_PROTOCOL;

typedef struct _EFI_FRB_STAT {
  BOOLEAN                       FrbEnabled;
  BOOLEAN                       FrbFailed;
  UINTN                         FrbTimeout;
  FRB_SYSTEM_BOOT_POLICY        FrbCurrentBootPolicy;
  FRB_SYSTEM_TIMEOUT_INTERRUPT  FrbTimeoutInterrupt;
} EFI_FRB_STAT;

//
//  FRB Protocol APIs
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRB_STAT) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  OUT EFI_FRB_STAT                      * FrbStatData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRB_TIMER) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  IN UINTN                              *Timeout
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRB_PRE_TIMEOUT_POLICY) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  IN UINTN                              *PreTimeoutInterval,
  IN FRB_SYSTEM_TIMEOUT_INTERRUPT       FrbTimeoutInterrupt
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRB_BOOT_POLICY) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  IN FRB_SYSTEM_BOOT_POLICY             FrbBootPolicy
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_FRB) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  IN BOOLEAN                            UseResidualCount,
  IN BOOLEAN                            StartFrbCountDown,
  IN BOOLEAN                            ClearFrbGlobalStatus
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DISABLE_FRB) (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType
  );

//
// FRB PROTOCOL
//
typedef struct _EFI_SM_FRB_PROTOCOL {
  EFI_GET_FRB_STAT                GetFrbStatData;
  EFI_SET_FRB_TIMER               SetFrbTimerData;
  EFI_SET_FRB_PRE_TIMEOUT_POLICY  SetFrbPreTimeoutPolicy;
  EFI_SET_FRB_BOOT_POLICY         SetFrbBootPolicy;
  EFI_ENABLE_FRB                  EnableFrb;
  EFI_DISABLE_FRB                 DisableFrb;
}EFI_SM_FRB_PROTOCOL;

extern EFI_GUID gEfiFrbCodeProtocolGuid;

#endif
