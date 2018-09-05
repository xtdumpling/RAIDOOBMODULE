//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  CpuHotAddData.h

Abstract:
  Module to relocate SMBASE addresses for all processors.

--*/
#ifndef _CPU_HOT_ADD_DATA_H_
#define _CPU_HOT_ADD_DATA_H_

//
// Includes
//
#include <Library/PcdLib.h>
#include <Library/CpuPpmLib.h>

//
// Cpu hot add data Protocol GUID
//
#define EFI_CPU_HOT_ADD_DATA_PROTOCOL_GUID \
{0x330be755, 0xfbed, 0x4f18, 0xb9, 0xa8, 0x49, 0x58, 0x56, 0xd3, 0xd7, 0xa1}

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiCpuHotAddDataProtocolGuid;

#define SBSP_MSR_NUM    4

typedef struct {
  //
  // Placeholder for MP intialization data
  //

  //
  // Data required for SMBASE relocation
  //
  UINT64    ApicId[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
  UINTN     SmBase[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
  UINT32    IEDBase;
  UINT32    SmrrBase;
  UINT32    SmrrSize;

} CPU_SMM_INIT_DATA;



#pragma pack(1)

typedef struct {
  UINT32                    Number;
  UINT32                    BIST;
} BIST_INFO;

typedef struct {
  UINT32                    MsrOffset;
  UINT64                    MsrValue;
} MSR_INFO;

typedef struct {
  UINTN             Lock;
  VOID              *StackStart;
  UINTN             StackSize;
  VOID              *ApFunction;
  UINT32            InitFlag;
  UINT32            SignalFlag;
  UINT32            PModeSignalFlag;
  UINT32            NumCpuCheckedIn;
  BIST_INFO         BistBuffer[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];    // We'll be handling only one CPU at a time during hot add. So 256 should be enough to cover all the APIC IDs
  CPU_SMM_INIT_DATA CpuSmmInitData;
  MSR_INFO          SbspMsrInfo[SBSP_MSR_NUM];
} HOT_ADD_CPU_EXCHANGE_INFO;

typedef 
VOID 
(EFIAPI *EFI_CPU_SMM_RELOCATE) (
);

typedef struct {
  EFI_PHYSICAL_ADDRESS        StartupVector;
  HOT_ADD_CPU_EXCHANGE_INFO   *MpExchangeInfoPtr;
  EFI_CPU_SMM_RELOCATE        PackageSmmRelocate;
  EFI_CPU_PM_STRUCT           *CpuPmStruct;
  EFI_PHYSICAL_ADDRESS        SmmStartupCodeArea;
} CPU_HOT_ADD_DATA;

#pragma pack()

#endif //_CPU_HOT_ADD_DATA_H_
