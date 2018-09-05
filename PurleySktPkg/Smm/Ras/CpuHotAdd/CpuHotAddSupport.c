/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  CpuHotAddSupport.c

Abstract:
  Module to relocate SMBASE addresses for all processors.

--*/

#include "CpuHotAdd.h"
#include <Library/IoLib.h>
#include <Library/mpsyncdatalib.h>
#include <Register/PchRegsP2sb.h>

extern ACPI_CPU_DATA                              *mAcpiCpuData;
extern volatile HOT_ADD_CPU_EXCHANGE_INFO         *mExchangeInfo;
extern EFI_CPU_PM_STRUCT                          mLocalPpmStruct;

VOID
InitialProcessorInit(
) 
{

    
  MTRR_SETTINGS             *MtrrSettings;

  LoadMicrocodeEx();

  ProgramVirtualWireMode ();
  
  MtrrSettings = (MTRR_SETTINGS *) (UINTN)mAcpiCpuData->MtrrTable;
  MtrrSetAllMtrrs (MtrrSettings);

   CpuMiscMsrProgramming();

  //
  // TO-DO  -- Need to find out how to enable other Processor Features like 
  // X2APIC, PPM, Reset CPL, TXT etc...
  //
}

VOID CpuMiscMsrProgramming (
  )
{

  UINT8   Index;
  UINT32  MsrOffset;
  UINT64  MsrValue;

  for (Index = 0; Index < SBSP_MSR_NUM; Index++) {
    MsrOffset = mExchangeInfo->SbspMsrInfo[Index].MsrOffset;
    MsrValue = mExchangeInfo->SbspMsrInfo[Index].MsrValue;

    AsmWriteMsr64 (MsrOffset, MsrValue);
  }
}



VOID PpmCsrProgramming (
  UINT8   SktId
  )
{


  mLocalPpmStruct.Info->SocketNumber = SktId;

  ProgramCSRTurboPowerLimit((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrPkgCstEntryCriteriaMask((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrPerfPlimitControl((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrDynamicPerfPowerCtl((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrSapmCtl((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrSwLtrOvrd((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrPmaxConfig((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramCsrResponseRatioCfg((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PFastRaplDutyCycle((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PPcuMiscConfig((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PHWPMMiscConfig((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PDynamicL1((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PForceUncoreAndMeshRatio((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  ProgramB2PMiscWorkaroundEnable((EFI_CPU_PM_STRUCT *)&mLocalPpmStruct);
  return;
}


VOID PpmMsrProgramming (
  EFI_CPU_PM_STRUCT *PpmStruct
  )
{
  ProgramMsrPowerCtl (PpmStruct, 0);
  ProgMsrPriPlaneCurtCfgCtrL (PpmStruct, 0);
  ProgramMsrTurboPowerLimit (PpmStruct, 0);
  ProgramMsrTurboRatioLimit(PpmStruct, 0);
  ProgramEnergyPerfBiasConfigMsr(PpmStruct, 0);
  ProgramEnergyPerfBiasConfigMsr(PpmStruct, 0);
  ProgramMsrTemperatureTarget(PpmStruct, 0); 
  ProgramMsrMiscPwrMgmt(PpmStruct, 0); 
  ProgramMsrCLatency(PpmStruct, 0);

  if(PpmStruct->Info->CpuType == CPU_FAMILY_SKX) {

    //
    //Enable HWPM modes only if CPU supports 
    //
    if(PpmStruct->Info->HwpmSupport & HWP_ENABLE) {
      EnableHwpFeatures(PpmStruct, 0);
    }
  }

  return;
}

VOID
SmmInitializeSmrrEmrr (
  VOID
  )
{
  UINT64                MtrrCap;
  UINT64                ValidMtrrAddressMask;
  EFI_CPUID_REGISTER    FeatureInfo;
  EFI_CPUID_REGISTER    FunctionInfo;
  UINT8                 PhysicalAddressBits;

  //
  // Synchup SMRR/EMRR
  //
  MtrrCap = AsmReadMsr64 (EFI_IA32_MTRR_CAP);
  if (!(MtrrCap & B_EFI_IA32_MTRR_CAP_SMRR_SUPPORT)) {
    return ;
  }

  //
  // Get physical CPU MTRR width in case of difference from BSP
  //
  AsmCpuid (EFI_CPUID_EXTENDED_FUNCTION, &FunctionInfo.RegEax, NULL, NULL, NULL);
  PhysicalAddressBits = 36;
  if (FunctionInfo.RegEax >= (UINT32)EFI_CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (EFI_CPUID_VIR_PHY_ADDRESS_SIZE, &FeatureInfo.RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) FeatureInfo.RegEax;
  } 
  ValidMtrrAddressMask = (LShiftU64(1, PhysicalAddressBits) - 1) & 0xfffffffffffff000;

  AsmWriteMsr64 (EFI_SMRR_PHYS_BASE, (mExchangeInfo->CpuSmmInitData.SmrrBase & 0xFFFFF000) | EFI_CACHE_WRITEBACK);
  AsmWriteMsr64 (EFI_SMRR_PHYS_MASK, (~(mExchangeInfo->CpuSmmInitData.SmrrSize - 1)) & ValidMtrrAddressMask | B_EFI_CACHE_MTRR_VALID);
  if (MtrrCap & B_EFI_IA32_MTRR_CAP_EMRR_SUPPORT) {
    AsmWriteMsr64 (EFI_EMRR_PHYS_BASE, ((mExchangeInfo->CpuSmmInitData.SmrrBase + mExchangeInfo->CpuSmmInitData.SmrrSize - 0x100000)  & 0xFFFFF000) | EFI_CACHE_WRITEBACK);
    AsmWriteMsr64 (EFI_EMRR_PHYS_MASK, (~(0x100000 - 1)) & ValidMtrrAddressMask | B_EFI_CACHE_MTRR_VALID);  
  }

  return;
} 

#ifdef LT_FLAG

EFI_STATUS
SetApicBSPBit (
  IN  BOOLEAN   Enable
  )
{
  UINT64  ApicBaseReg;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);

  if (Enable) {
    ApicBaseReg |= 0x100;
  } else {
    ApicBaseReg &= 0xfffffffffffffe00;
  }

  AsmWriteMsr64 (MSR_IA32_APIC_BASE, ApicBaseReg);

  return EFI_SUCCESS;
}

/**
    Program Virtual Wire Mode.  

    @param  BSP             Is this the Processor BSP.
    @param  VirtualWireMode Virtual Wire mode.

    @return None
**/
VOID
ProgramVirtualWireMode_RAS (
  BOOLEAN                       BSP,
  UINT32                        VirtualWireMode
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  volatile UINT32       *EntryAddress;
  UINT32                EntryValue;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000;

  //
  // Program the Spurious Vectore entry
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFFFD0F;
  EntryValue |= 0x10F;
  *EntryAddress = EntryValue;

  //
  // Program the LINT1 vector entry as extINT
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  if (VirtualWireMode != 0) {
    EntryValue |= 0x10000;
  } else {
    if (BSP) {
      EntryValue &= 0xFFFE00FF;
      EntryValue |= 0x700;
    } else {
      EntryValue |= 0x10000;
    }
  }
  *EntryAddress = EntryValue;

  //
  // Program the LINT1 vector entry as NMI
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFE00FF;
  if (BSP) {
    EntryValue |= 0x400;
  } else {
    EntryValue |= 0x10400;
  }
  *EntryAddress = EntryValue;

  if (VirtualWireMode && BSP) {
    //
    // Initialize the I0XApic RT table for virtual wire B
    //
    *(volatile UINT8   *)(UINTN)R_PCH_IO_APIC_INDEX = 0x10;
    *(volatile UINT32  *)(UINTN)R_PCH_IO_APIC_DATA= 0x0700;
    *(volatile UINT8   *)(UINTN)R_PCH_IO_APIC_INDEX = 0x11;
    *(volatile UINT32  *)(UINTN)R_PCH_IO_APIC_DATA  = (GetApicID () << 24);
  }
}

/**
    Send Interrupts to the Processor  

    @param  BroadcastMode             Broadcast mode.
    @param  ApicID                    Apic ID of the processor for which interrupt to be sent.
    @param  VectorNumber              Vector Number.
    @param  DeliveryMode              Delivery mode.      
    @param  TriggerMode               Trigger mode
    @param  Assert                    Assert


  @return EFI_SUCCESS                 If the interrupt sent successfully.
  @return EFI_NOT_READY               
  @return EFI_INVALID_PARAMETER
**/
EFI_STATUS
SendInterrupt (
  IN  UINT32                               BroadcastMode,
  IN  UINT32                               ApicID,
  IN  UINT32                               VectorNumber,
  IN  UINT32                               DeliveryMode,
  IN  UINT32                               TriggerMode,
  IN  BOOLEAN                              Assert
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  UINT32                ICRLow;
  UINT32                ICRHigh;

  //
  // Initialze ICR high dword, since XEON family processor needs
  // the destination field to be 0xFF when it is a broadcast
  //
  ICRHigh = 0xff000000;
  ICRLow  = VectorNumber | (DeliveryMode << 8);

  if (TriggerMode == TRIGGER_MODE_LEVEL) {
    ICRLow |= 0x8000;
  }

  if (Assert) {
    ICRLow |= 0x4000;
  }

  switch (BroadcastMode) {
  case BROADCAST_MODE_SPECIFY_CPU:
    ICRHigh = ApicID << 24;
    break;

  case BROADCAST_MODE_ALL_INCLUDING_SELF:
    ICRLow |= 0x80000;
    break;

  case BROADCAST_MODE_ALL_EXCLUDING_SELF:
    ICRLow |= 0xC0000;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  ApicBaseReg = AsmReadMsr64 (EFI_MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000;

  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_HIGH_OFFSET)  = ICRHigh;
  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET)   = ICRLow;
  
  //MicroSecondDelay  (200);

  ICRLow = *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);
  if (ICRLow & 0x1000) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}

#endif //LT_FLAG


