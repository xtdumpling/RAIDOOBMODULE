/** @file
  Contains protocol information for the Platform Error Handling Protocol.

  Copyright (c) 2009-2015 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _PLATFORM_ERROR_HANDLING_PROTOCOL_H_
#define _PLATFORM_ERROR_HANDLING_PROTOCOL_H_
#include "ErrorRecords.h"
#include <Library/SynchronizationLib.h>
#include <mca_msr.h>
#include <Cpu/LockAlign.h>

//
// Forward reference for pure ANSI compatability
//

#define EFI_PLATFORM_ERROR_HANDLING_PROTOCOL_GUID \
   { \
      0x6820abd4, 0xa292, 0x4817, 0x91, 0x47, 0xd9, 0x1d, 0xc8, 0xc5, 0x35, 0x42 \
   }

typedef enum {
  CorrectableSev0,
  RecoverableSev1,
  UncorrectableSev2,
  MaxSeverity
} ERROR_SEVERITY; 
   
typedef 
EFI_STATUS  
(EFIAPI *EFI_ERROR_LISTENER_FUNCTION) (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  );

typedef struct _LISTENER_LIST_ENTRY {
  EFI_ERROR_LISTENER_FUNCTION   Listener;
  struct _LISTENER_LIST_ENTRY   *NextListener;
} LISTENER_LIST_ENTRY;

typedef 
EFI_STATUS  
(EFIAPI *EFI_NOTIFIER_FUNCTION) (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  );

typedef struct _NOTIFIER_LIST_ENTRY {
  EFI_NOTIFIER_FUNCTION         Notifier;
  struct _NOTIFIER_LIST_ENTRY   *NextNotifier;
  UINT8                         Priority;
} NOTIFIER_LIST_ENTRY;

// Protocol Interfaces

typedef 
EFI_STATUS  
(EFIAPI *EFI_PLATFORM_ERROR_HANDLING_REGISTER_ERROR_LISTENER) (
  IN      EFI_ERROR_LISTENER_FUNCTION  ErrorListener
);

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_ERROR_HANDLING_LOG_REPORTED_ERROR) (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
);

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_ERROR_HANDLING_REGISTER_NOTIFIER) (
  IN      EFI_NOTIFIER_FUNCTION  FunctionPtr,
  IN      UINT8                  Priority
);

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_ERROR_HANDLING_NOTIFY_OS) (
  IN      UINT8   ErrorSeverity
);

#define MAX_ERROR_EVENTS MaxErrSrc
#define MAX_ERROR_SEVERITY    3
#define SEVERITY_MAP_FATAL 2
#define SEVERITY_MAP_RECOVERABLE 1
#define SEVERITY_MAP_CORRECTED 0


typedef enum {
  MceHdrRestart= 0,
  MceHdrReset,
  MceHdrAssert
} SMM_MCE_ENDING_ACTION;

typedef enum {
  McbankSrc,
  CsrSrc,
  PcieSrc,
  PlatformSrc,
  MaxErrSrc  
} SMM_ERROR_SRC;

typedef struct {
  UINT32 BankIdx;
  UINT32 ApicId;
} SMM_ERROR_EVENT;

typedef struct {
  BOOLEAN ErrorFound;
  UINT32 SeverityMap;
  SMM_ERROR_EVENT Log[MAX_ERROR_SEVERITY];
} SMM_ERROR_EVENT_DATA;

typedef struct {
  BOOLEAN ErrorsFound;
  SMM_ERROR_EVENT_DATA Events[MAX_ERROR_EVENTS];
} SMM_ERROR_EVENT_LOG;

typedef struct {
  volatile BOOLEAN Lmce;
  volatile UINT64 CoreProcessingBank;
  volatile UINT64 UncoreProcessingBank;
  volatile UINT64 CoreSmiSrcCp;
  volatile UINT64 UncoreSmiSrcCp;
  volatile UINT64 SmmMceSyncState;  
  volatile SMM_ERROR_EVENT_LOG EventLog;
  volatile UINT8 CmciMcBankErrCount[MCBANK_MAX];
} SMM_ERROR_CPU_DATA_BLOCK;

typedef struct {
  SMM_ERROR_CPU_DATA_BLOCK      CpuData[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
  SPIN_LOCK                     SerialLock; //For Debug Purpose.
  volatile BOOLEAN              Broadcast;
} SMM_ERROR_MP_SYNC_DATA;

typedef struct _EFI_PLATFORM_ERROR_HANDLING_PROTOCOL {
  EFI_PLATFORM_ERROR_HANDLING_REGISTER_ERROR_LISTENER   RegisterErrorListener;
  EFI_PLATFORM_ERROR_HANDLING_LOG_REPORTED_ERROR        LogReportedError;
  EFI_PLATFORM_ERROR_HANDLING_REGISTER_NOTIFIER         RegisterNotifier;
  EFI_PLATFORM_ERROR_HANDLING_NOTIFY_OS                 NotifyOs;
  SMM_ERROR_MP_SYNC_DATA                                *SmmErrorMpSyncData;
} EFI_PLATFORM_ERROR_HANDLING_PROTOCOL;

extern EFI_GUID gEfiPlatformErrorHandlingProtocolGuid;

#endif
