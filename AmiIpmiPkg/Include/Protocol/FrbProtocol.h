//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file FrbProtocol.h
    Frb protocol definitions

**/

#ifndef _EFI_FRB_H_
#define _EFI_FRB_H_

//----------------------------------------------------------------------

#include <Include/Frb.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define EFI_SM_FRB_PROTOCOL_GUID \
  { \
    0xbc5f861c, 0x86ed, 0x417e, { 0xbb, 0x7b, 0x6c, 0x2, 0x6b, 0xcd, 0x75, 0x5b } \
  }

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

/**
    FRB PROTOCOL
*/
typedef struct _EFI_SM_FRB_PROTOCOL {
  EFI_GET_FRB_STAT                GetFrbStatData;
  EFI_SET_FRB_TIMER               SetFrbTimerData;
  EFI_SET_FRB_PRE_TIMEOUT_POLICY  SetFrbPreTimeoutPolicy;
  EFI_SET_FRB_BOOT_POLICY         SetFrbBootPolicy;
  EFI_ENABLE_FRB                  EnableFrb;
  EFI_DISABLE_FRB                 DisableFrb;
} EFI_SM_FRB_PROTOCOL;

extern EFI_GUID gEfiSmFrbProtocolGuid;

//----------------------------------------------------------------------

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
