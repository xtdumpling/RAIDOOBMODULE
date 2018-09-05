/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009-2015, Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file DimmIsolationFlow.h

    Faulty dimm isolation flow implementation header file.
 **/
#include "WheaSiliconHooksLib.h"

#define MAX_POISON_RECORD   20
#define CACHELINE_MASK      0xFFFFFFFFFFFFFFC0

#define IFU_POISON_DETECTED 0xC
#define DCU_POISON_DETECTED 0x10
#define MC_UC_PATROL_SCRUB  0x10

typedef enum _MCE_CLASS {
  FATAL,
  DUE,
  UCNA,
  SRAO,
  SRAR,
  CORRECTED,
  INVALID
} MCE_CLASS;

typedef enum _MCE_SOURCE {
  IMC,
  M2M,
  IFU,
  DCU,
  DONT_CARE
} MCE_SOURCE;

typedef struct {
  BOOLEAN    Valid;
  UINT8      Socket;
  UINT8      Mc;
  UINT8      Ch;
  UINT8      DIMM;
  UINT64     Address;
} POISON_RECORD;

typedef struct {
  POISON_RECORD   Records[MAX_POISON_RECORD];
  BOOLEAN         Overflow;
  UINT32          Pivot;
} POISON_TRACK;

typedef struct {
  UINT32    Index;
  UINT64    Value;
} MSR_REGISITER;

typedef UINT64
(EFIAPI * READ_MSR)(
  IN UINT8   Socket,
  IN UINT32  Index
  );

typedef
UINT32
(EFIAPI *READ_CSR) (
  IN UINT8    SocId,
  IN UINT8    BoxInst,
  IN UINT32   Offset
  );

typedef
EFI_STATUS
(EFIAPI *LOG_ACTION) (
  MEMORY_DEV_INFO * pMemInfo
  );

MCE_CLASS
GetMceClass (
  CONST IA32_MCI_STATUS_MSR_STRUCT * CONST pMcStatus
  );

EFI_STATUS
MpReadMsr (
  MSR_REGISITER * CONST pMsrReg
  );

EFI_STATUS
EFIAPI
SetPoisonRecord (
  MEMORY_DEV_INFO * pMemInfo
  );

EFI_STATUS
EFIAPI
FinalizeUELog (
  MEMORY_DEV_INFO * pMemInfo
  );

EFI_STATUS
EFIAPI
LookUpPoisonRecord(
  CONST MCBANK_ERR_INFO * CONST pMcBankInfo,
  OUT   MEMORY_DEV_INFO * CONST pMemInfo
  );

EFI_STATUS
InitializeIsolationFlow(
  );

EFI_STATUS
EFIAPI
IsolateFaultyDimm (
  MCBANK_ERR_INFO * pMcBankInfo,
  MEMORY_DEV_INFO * CONST pLogBuffer
  );

EFI_STATUS
EFIAPI
IsolateFaultyDimmForErrSig (
  UINT8                          McBank,
  UINT8                          Socket,
  EMCA_MC_SIGNATURE_ERR_RECORD * CONST pMcSigRecord,
  MEMORY_ERROR_SECTION * CONST   MemErrSection
  );
