//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuHWPMConfig.c

**/

#include "CpuPpmIncludes.h"

extern EFI_MP_SERVICES_PROTOCOL     *mMpService;

/**
  Checks FUSE_HWP_ENABLE support for HWPM feature for specified processor.

  This function retuns of HWPM Interface supported if FUSE_HWP_ENABLE is enabled

  @param  EFI_CPU_PM_STRUCT

  @retval HWPM fuse value

**/
UINT8 
EFIAPI
GetHwpmSupport (
   EFI_CPU_PM_STRUCT *ppm 
  )
{
  UINT8  HwpSupport;
  UINT8  Socket;
  UINT32 CapId5;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  Socket = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;

  CapId5 = CpuCsrAccess->ReadCpuCsr(Socket, 0, CAPID5_PCU_FUN3_REG);
  HwpSupport = (UINT8)((CapId5 >> 25) & (ACC_ENABLE | HWP_ENABLE));	

  return HwpSupport;
}

/**
  Enables HWPM Interface feature  for specified processor.

  This function enables of HWPM Interface for specified processor.

  @param PPMPolicy       - Pointer to policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval None

**/
VOID
HWPMInterfaceReg (
  EFI_CPU_PM_STRUCT   *ppm,
  UINTN               ProcessorNumber
  )
{
  MSR_REGISTER MsrValue;

  //
  // Read MSR_MISC_PWR_MGMT
  //
  MsrValue.Qword = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
  //MsrValue.Dwords.Low |= ppm->Setup->Hwpm.HWPMNative; //get from setup
  MsrValue.Dwords.Low &= ~(B_MSR_ENABLE_HWP | B_MSR_ENABLE_HWP_NOTIFICATIONS | B_MSR_ENABLE_HWP_AUTONOMOUS);
  MsrValue.Dwords.Low &= ~B_EFI_MSR_MISC_PWR_MGMT_EPP;

  if(ppm->Setup->Hwpm.HWPMNative) { //if setup has choosen NAtive mode
    MsrValue.Dwords.Low |= B_MSR_ENABLE_HWP;
  } else if (ppm->Setup->Hwpm.HWPMOOB) {
    MsrValue.Dwords.Low |= B_MSR_ENABLE_HWP_AUTONOMOUS;
  }

  if(ppm->Setup->Hwpm.HWPMInterrupt) {
    MsrValue.Dwords.Low |= B_MSR_ENABLE_HWP_NOTIFICATIONS;
  }

  if (ppm->Setup->Hwpm.EPPEnable) {
    MsrValue.Dwords.Low |= B_EFI_MSR_MISC_PWR_MGMT_EPP;
  }

  //
  // HWPM Ineterface setup 
  //
  AsmWriteMsr64 (MSR_MISC_PWR_MGMT, MsrValue.Qword);
  WriteRegisterTable(ProcessorNumber, Msr, MSR_MISC_PWR_MGMT, 0, 64, MsrValue.Qword);

  if (ppm->Setup->Hwpm.HWPMEnable == 3)   //Native Mode with No Legacy Support
  {
    MsrValue.Qword = AsmReadMsr64 (MSR_IA32_PM_ENABLE);
    MsrValue.Dwords.Low |= B_HWP_ENABLE;
    AsmWriteMsr64 (MSR_IA32_PM_ENABLE, MsrValue.Qword);
    WriteRegisterTable(ProcessorNumber, Msr, MSR_IA32_PM_ENABLE, 0, 64, MsrValue.Qword);
  }

  return;
}


/**
  Enables Autonomous Cstate feature  for specified processor.

  This function enables of Autonomous Cstate feature  for specified processor.

  @param  setup provided user options.

  @retval None

**/
VOID
EnableAutonomousCStateControl (
  EFI_CPU_PM_STRUCT   *ppm
  )
{
  MSR_REGISTER  MsrValue;

  //
  // Read MSR_MISC_PWR_MGMT
  //
  MsrValue.Qword = AsmReadMsr64 (MSR_PMG_CST_CONFIG_CONTROL);

  MsrValue.Dwords.Low &= ~B_EFI_ACC_ENABLE;

  if ((ppm->Setup->Hwpm.AutoCState) && (ppm->Info->HwpmSupport & ACC_ENABLE)){
    //
    // HWPM Interface setup
    //
    MsrValue.Dwords.Low |=  B_EFI_ACC_ENABLE;

    AsmWriteMsr64 (MSR_PMG_CST_CONFIG_CONTROL, MsrValue.Qword);
  }

  return;
}

/**
  Enables the Thermal Interrupt in the core Local APIC.
  Enable Local APIC to generate a SMI 
  @param[in] ppm  Unused
  @retval  None
**/
VOID
EFIAPI
EnableHwpLvtThermalInterrupt(
  EFI_CPU_PM_STRUCT   *ppm,
  UINTN               ProcessorNumber
  )
{
  UINT32        RegVal;
  MSR_REGISTER  xApicBaseMsrVal;
  MSR_REGISTER  Ia32HwpInterrupt;
  UINTN         LocaApicBaseAddr;   
  BOOLEAN       x2ApicEnabled;

  ///
  /// Set bits 0 and 1 of MSR IA32_HWP_INTERRUPT (773h) to enable HWP interrupts
  ///
  Ia32HwpInterrupt.Qword = AsmReadMsr64 (MSR_IA32_HWP_INTERRUPT);
  Ia32HwpInterrupt.Dwords.Low |= (B_IA32_HWP_CHANGE_TO_GUARANTEED | B_IA32_HWP_EXCURSION_TO_MINIMUM);
  AsmWriteMsr64 (MSR_IA32_HWP_INTERRUPT, Ia32HwpInterrupt.Qword);
  //S3 save
  WriteRegisterTable(ProcessorNumber, Msr, MSR_IA32_HWP_INTERRUPT, 0, 64, Ia32HwpInterrupt.Qword);

  xApicBaseMsrVal.Qword = AsmReadMsr64 (EFI_MSR_IA32_APIC_BASE);
  LocaApicBaseAddr = xApicBaseMsrVal.Qword & 0xFFFFFFFFFFFFF000L;
  x2ApicEnabled = ((xApicBaseMsrVal.Dwords.Low & (BIT11 + BIT10)) == (BIT11 + BIT10));
  ///
  /// Configure the Local APIC to generate an SMI on Thermal events.  First,
  /// Clear BIT16, BIT10-BIT8, BIT7-BIT0.  Then, set BIT9 (delivery mode).
  /// Don't enable the interrupt if it's already enabled
  ///
  if (x2ApicEnabled) {  //if x2Apic already enabled
    RegVal = (UINT32) AsmReadMsr64 (EFI_EXT_XAPIC_LVT_THERM);
  } else {
    RegVal = *(UINT32 *) (UINTN) (LocaApicBaseAddr + APIC_REGISTER_THERMAL_DEF_OFFSET);
  }

  if ((RegVal & (B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) != V_MODE_SMI) {
    RegVal = (RegVal &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | V_MODE_SMI;
    if (x2ApicEnabled) {
      AsmWriteMsr64 (EFI_EXT_XAPIC_LVT_THERM, RegVal);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, EFI_EXT_XAPIC_LVT_THERM, 0, 64, RegVal);
    } else {
      *(UINT32 *) (UINTN) (LocaApicBaseAddr + APIC_REGISTER_THERMAL_DEF_OFFSET) = (UINT32) RegVal;
      S3BootScriptSaveMemWrite(S3BootScriptWidthUint32, \
            (LocaApicBaseAddr + APIC_REGISTER_THERMAL_DEF_OFFSET), \
             1,  \
             &RegVal);
    }
  }

  return;
}

/**
  This function check and enable the HWP features.

  @param PPMPolicy       - Pointer to policy protocol instance
  @param ProcessorNumber - Processor index that is executing

  @retval  None

**/
VOID
EFIAPI
EnableHwpFeatures(
  EFI_CPU_PM_STRUCT   *ppm,
  UINTN               ProcessorNumber
  )
{
  HWPMInterfaceReg(ppm, ProcessorNumber);  //enable HWP Native or OOB, Interrupt
  if(ppm->Setup->Hwpm.HWPMInterrupt) {
    EnableHwpLvtThermalInterrupt(ppm, ProcessorNumber); //enable Lvt 
  }
  return;
}

/**

  This routine is called to program PCU_MISC_CONFIG. It will be call multiple time passing any the SocketNumber to be programmed.

  @param PPMPolicy Pointer to PPM Policy protocol instance

  @retval EFI_SUCCESS

**/
VOID
ProgramB2PHWPMMiscConfig (
    EFI_CPU_PM_STRUCT *ppm
  )
{
  UINT32 PM_MBX_CMD = 0;
  UINT32 PM_MBX_DATA = 0;
  UINT32 data32 = 0;
  UINT32 Tempdata = 0;
  UINT8  SocketNumber;
  UINT64   RegAddr = 0; 
  UINT8    Size = 4;

  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;

  SocketNumber = ppm->Info->SocketNumber;
  CpuCsrAccess = ppm->CpuCsrAccess;
  data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);

  if (ppm->Setup->Hwpm.HWPMEnable) {

    PM_MBX_DATA = 0; 
    PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG;
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if ((Tempdata & 0x000000ff) != PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND) {
      data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, BIOS_MAILBOX_DATA_PCU_FUN1_REG);

      if (ppm->Setup->Hwpm.APSrocketing) data32 |= PCU_MISC_CONFIG_APS_ROCKETING;
      if (ppm->Setup->Hwpm.Scalability) data32 |= PCU_MISC_CONFIG_SCALABILITY;
      if (ppm->Setup->Hwpm.PPOBudget) data32 |= PCU_MISC_CONFIG_PPO_BUDGET;

      PM_MBX_CMD = (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG;
      PM_MBX_DATA |= data32; 

      Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);  
    }
  }

  if ((ppm->Setup->PowerCtl.Dwords.Low & PWR_PERF_TUNING_CFG_MODE) == 0) {
      //HSD: 4929906
      data32 = CpuCsrAccess->ReadCpuCsr(SocketNumber, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG);
      if (ppm->Setup->Hwpm.OutofBandAlternateEPB) {
        data32 |= PCU_OUT_OF_BAND_ALTERNATE_EPB;
        CpuCsrAccess->WriteCpuCsr(SocketNumber, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG, data32);
        RegAddr = CpuCsrAccess->GetCpuCsrAddress(SocketNumber, 0, DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG, &Size);
        S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &data32 );
      }
  }

  //HSD 5332565: HWP Out of band BIOS EPP initialization through B2P mailbox
  if (ppm->Setup->Hwpm.HWPMOOB && ppm->Setup->Hwpm.EPPEnable) {
    PM_MBX_DATA = (UINT32) ppm->Setup->Hwpm.EPPProfile;     
    PM_MBX_CMD = (UINT32) (MAILBOX_BIOS_CMD_OOB_INIT_EPP + BIT8);   //Bit[8] = 1, write access
    //DEBUG ( (EFI_D_ERROR, " \n :: BIOS programs MAILBOX_BIOS_CMD_OOB_INIT_EPP for ALT_EPB on socket (%d). Value = 0x%x \n", SocketNumber, PM_MBX_DATA) );
    Tempdata = ppm->CpuCsrAccess->Bios2PcodeMailBoxWrite(SocketNumber, PM_MBX_CMD, PM_MBX_DATA);
    if (Tempdata > 0) {
      DEBUG ( (EFI_D_ERROR, " \n\n :: !!! BIOS failed to program ALT_EPB on socket (%d). Pcode returns (%d) !!!. \n\n", SocketNumber, Tempdata) );
    }
  }
}
