/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Memory Operation Functions for x64 Architecture.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ArchSpecific.c

**/

#include "Cpu.h"
#include "MpService.h"

MP_CPU_EXCHANGE_INFO    *mExchangeInfo;

/**
  Prepares Startup Vector for APs.

  This function prepares Startup Vector for APs.

**/
VOID
PrepareAPStartupVector (
  VOID
  )
{
  MP_ASSEMBLY_ADDRESS_MAP   AddressMap;
  UINTN                     MpCpuExchangeInfoSize;

  //
  // Get the address map of startup code for AP,
  // including code size, and offset of long jump instructions to redirect.
  //
  AsmGetAddressMap (&AddressMap);

  //
  // Allocate a 4K-aligned region under 1M for startup vector for AP.
  // The region contains AP startup code and exchange data between BSP and AP.
  // Note the BIST info per each logical processor is located immediately
  // behind MP_CPU_EXCHANGE_INFO.
  //
  MpCpuExchangeInfoSize = sizeof (MP_CPU_EXCHANGE_INFO) + sizeof (BIST_INFO) * (PcdGet32 (PcdCpuMaxLogicalProcessorNumber) - 1);
  AllocateStartupVector (AddressMap.Size + MpCpuExchangeInfoSize);

  //
  // Copy AP startup code to startup vector, and then redirect the long jump
  // instructions for mode switching.
  //
  CopyMem ((VOID *) (UINTN) mStartupVector, AddressMap.RendezvousFunnelAddress, AddressMap.Size);
  RedirectFarJump (&AddressMap, mStartupVector, mStartupVector);

  //
  // Get the start address of exchange data between BSP and AP.
  //
  mExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mStartupVector + AddressMap.Size);

  ZeroMem ((VOID *) mExchangeInfo, MpCpuExchangeInfoSize);

  PrepareGdtIdtForAP (
    (IA32_DESCRIPTOR *) (UINTN) &mExchangeInfo->GdtrProfile,
    (IA32_DESCRIPTOR *) (UINTN) &mExchangeInfo->IdtrProfile
    );

  //
  // Assign ApLoopMode during POST phase
  // Set ApLoopMode to ApInHltLoop if ApLoopMode is ApInMwaitLoop
  // because ApInMwaitLoop could be supported only after C-State enabled.
  //
  if (PcdGet8 (PcdCpuApLoopMode) != ApInMwaitLoop) {
    mExchangeInfo->ApLoopMode = (AP_LOOP_MODE) (PcdGet8 (PcdCpuApLoopMode));
  } else {
    mExchangeInfo->ApLoopMode = ApInHltLoop;
  }

  mExchangeInfo->ApFunction  = ApProcEntry;
  mExchangeInfo->BufferStart = (UINT32) mStartupVector;
  mExchangeInfo->Cr3         = (UINT32) (AsmReadCr3 ());
  mExchangeInfo->DcuModeSelection = FeaturePcdGet (PcdCpuDcuModeSelectionFlag);
  mExchangeInfo->DcuMode     = PcdGet8 (PcdCpuDcuMode);
  mExchangeInfo->EnableExecuteDisable = IsBspExecuteDisableEnabled ();
  mExchangeInfo->InitFlag    = 1;
}

/**
  Sets specified IDT entry with given function pointer.

  This function sets specified IDT entry with given function pointer.

  @param  FunctionPointer  Function pointer for IDT entry.
  @param  IdtEntry         The IDT entry to update.

  @return The original IDT entry value.

**/
UINTN
SetIdtEntry (
  IN  UINTN                       FunctionPointer,
  OUT INTERRUPT_GATE_DESCRIPTOR   *IdtEntry
)
{
  UINTN  OriginalEntry;

  OriginalEntry = ((UINT64) IdtEntry->Offset63To32 << 32) + ((UINT32) IdtEntry->Offset31To16 << 16) + IdtEntry->Offset15To0;

  IdtEntry->Offset15To0  = (UINT16) FunctionPointer;
  IdtEntry->Offset31To16 = (UINT16) (FunctionPointer >> 16);
  IdtEntry->Offset63To32 = (UINT32) (FunctionPointer >> 32);

  return OriginalEntry;
}

/**
  Fixup jump instructions in the AP startup code.

  @param AddressMap    Pointer to MP_ASSEMBLY_ADDRESS_MAP.
  @param TargetBuffer  Target address of the startup vector.
  @param StartupVector AP startup vector address.
**/
VOID
RedirectFarJump (
  IN MP_ASSEMBLY_ADDRESS_MAP *AddressMap,
  IN EFI_PHYSICAL_ADDRESS    TargetBuffer,
  IN EFI_PHYSICAL_ADDRESS    StartupVector
  )
{
  *(UINT32 *)(UINTN)(StartupVector + AddressMap->FlatJumpOffset + 3) = (UINT32)(TargetBuffer + AddressMap->PModeEntryOffset);
  *(UINT32 *)(UINTN)(StartupVector + AddressMap->LongJumpOffset + 2) = (UINT32)(TargetBuffer + AddressMap->LModeEntryOffset);
}

/**
  Thunk function to transfer AP from long mode to 32bit protected mode before 
  transfer to OS.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  MonitorAddr       Pointer to moniter data address.
**/
VOID
EFIAPI
ApProtectedModeThunk (
  IN UINTN                  ProcessorNumber,
  IN MONITOR_MWAIT_DATA     *MonitorAddr
  )
{
  MP_ASSEMBLY_ADDRESS_MAP    AddressMap;

  //
  // Get the address map of startup code for AP,
  // including code size, and offset of long jump instructions to redirect.
  //
  AsmGetAddressMap (&AddressMap);
 
  AsmDisablePaging64 (
    0x10,
    (UINT32) (mStartupVector + AddressMap.ProtectedModeEntryPointOffset),
    GetProcessorFeatureCapability (ProcessorNumber, CState, NULL) && mEnableCState,
    (UINT32) MonitorAddr->MwaitTargetCstate,
    (UINT32) (UINTN) MonitorAddr
    );
}
