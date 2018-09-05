
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    SmbusRecovery.h

Abstract:
    This is the header file for SmbusRecovery SMM module

-----------------------------------------------------------------------------*/


#ifndef _SMBUS_RECOVERY_
#define _SMBUS_RECOVERY_

//
// Includes
//
#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/IioUds.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include "RcRegs.h"


/*
The following is the data structure used for doing the error recovery on the SMBUS
Some of the state of the SMBUS is saved  before doing the error recovery and 
when it is done, we restore the state.  And also, an 'Attribute' filed is used to
indicate the status of the recovery process
*/


typedef union {
  struct {
    UINT8                Attribute;        // State of the data structure, whether recovery needed/started/completed
    UINT8                TsodSA:3;         // Slave address of the last SMbus transaction
    UINT8                TsodPollEn:1;     // smb_tsod_poll_en
    UINT8                SmbSbeEn:1;       // smb_sbe_en
    UINT8                ClttSetting:1;    // ddr_disable_cltt
    UINT8                Reserved:2;
  } Bits;
  UINT16 Data;
} SMBUS_RECOVERY_DATA_STRUCT;

typedef struct {
  UINT32  smbCmdReg;
  UINT32  smbStatReg;
  UINT32  smbPeriodReg;
} SMBUS_REGISTER_ADDRS;


#define MAX_SMBUS_SEGMENT        2    

#define ERROR_RECOVERY_COMPLETED  BIT0
#define ERROR_RECOVERY_NEEDED     BIT1

#define KNL_SMBUS_RECOVERY_TIME_35_MS       35

//
// States in the SmbusRecovery State machine
//

#define FirstTimeEntry                  0x1
#define AcquireSemaphore                0x2
#define SemaphoreAcquireInProgress      0x3
#define SmbusAcquired                   0x4
#define PrepareSmbusRecovery            0x5
#define IsSmbusRecoveryComplete         0x6


VOID
RestoreStateAndEndSmbusRecovery(
  VOID
);

EFI_STATUS
CheckSmbusRecoveryComplete(
  VOID
);

VOID
ConfigureSmbusForErrorRecovery(
  VOID
);

VOID
CheckForSmbusErrors (
  VOID
);

VOID
ExecuteSmbusRecoveryStateMachine (
  VOID
);


EFI_STATUS
InitiateSmbusRecovery(
  VOID
);

VOID
ReleaseSemaphore (
  VOID
);

EFI_STATUS
SmbusRecoverySmiHandler 
(
  IN  EFI_HANDLE    DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommunicationBuffer,
  IN OUT UINTN      *SourceSize
);

EFI_STATUS
SemaphoreAcquireCallBack (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  );

EFI_STATUS
SmbusRecoveryCallback (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  );


#endif 
