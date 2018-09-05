/** @file
  This is the SMM driver HWP SMI interrupt 

@copyright
  Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "HwpLvtSmm.h"
#include <Library/IoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <PchAccess.h>

BIOS_ACPI_PARAM   *mAcpiParameter;

/**
  Checks for a HWP interrupt by reading MSR.

  This function must be MP safe.

  @param[in] Buffer    Pointer to Buffer -return HWP status
**/
VOID
EFIAPI
HwpLvtEventCheckMsr (
  IN VOID *Buffer
  )
{
  UINT32  *HwpEvent;
  UINT64   MsrData;

  ///
  /// Cast to enhance readability.
  ///
  HwpEvent = (UINT32 *) Buffer;
  MsrData = AsmReadMsr64 (MSR_IA32_HWP_STATUS);
  if(MsrData & (B_HWP_CHANGE_TO_GUARANTEED | B_HWP_EXCURSION_TO_MINIMUM | B_HWP_EXCURSION_TO_DESIRED)) {
    *HwpEvent = 1;
    mAcpiParameter->HwpInterrupt |= TRUE;    //update Acpi parameter 
  }
  return;
}

/**
  Clear HWP interrupt Status

  @param[in] Buffer
**/
VOID
EFIAPI
HwpLvtEventClearStatus (
  IN VOID *Buffer
  )
{
  UINT64   MsrData;

  MsrData = AsmReadMsr64 (MSR_IA32_HWP_STATUS);
  MsrData &= (~(B_HWP_CHANGE_TO_GUARANTEED | B_HWP_EXCURSION_TO_MINIMUM | B_HWP_EXCURSION_TO_DESIRED));
  AsmWriteMsr64(MSR_IA32_HWP_STATUS,MsrData);
  return;
}

/**
  Enable/Disable HWP Interrupt


  @param[in] Buffer    Pointer to Buffer -return HWP status
**/
VOID
EFIAPI
EnableDisableHwpInterrupt(
  IN VOID *HwpInterruptEnableFlag
  )
{
  
  UINT64  Ia32HwpInterrupt;
  ///
  /// Set/Clear bits 0 and 1 of MSR IA32_HWP_INTERRUPT (773h) to enable/disable HWP interrupts
  ///
  Ia32HwpInterrupt = AsmReadMsr64 (MSR_IA32_HWP_INTERRUPT);
  if( *((UINT32 *)HwpInterruptEnableFlag)== 1) {
    Ia32HwpInterrupt |= (B_IA32_HWP_CHANGE_TO_GUARANTEED | B_IA32_HWP_EXCURSION_TO_MINIMUM);
  }else {
    Ia32HwpInterrupt |= (~ (B_IA32_HWP_CHANGE_TO_GUARANTEED | B_IA32_HWP_EXCURSION_TO_MINIMUM));
  }
  AsmWriteMsr64 (MSR_IA32_HWP_INTERRUPT, Ia32HwpInterrupt);
  
  return;
}

/**
  Sets the GPE Ctrl bit to enerate a _GPE._L62 SCI to an ACPI OS

  @param  VOID

  @retval VOID
**/
VOID
EFIAPI
SetSwGpeSts (
  VOID
  )
{
  UINT8     GpeCntl;
  UINT16    AcpiBaseAddress;
  EFI_STATUS Status;

  Status = PchAcpiBaseGet (&AcpiBaseAddress);
  ASSERT_EFI_ERROR(Status);
  //
  // Check SCI enable bit
  //
  if (((IoRead8 (AcpiBaseAddress + R_PCH_ACPI_PM1_CNT)) & B_PCH_ACPI_PM1_CNT_SCI_EN) != 0) {
    //
    // Set SWGPE Status
    //
    GpeCntl = IoRead8 (AcpiBaseAddress + R_PCH_ACPI_GPE_CNTL);
    GpeCntl |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL;
    IoWrite8 (AcpiBaseAddress + R_PCH_ACPI_GPE_CNTL, GpeCntl);
  }
}

/**
  SMI handler to handle HWP CPU Local APIC SMI
  for HWP LVT interrupt

  @param[in] SmmImageHandle        Image handle returned by the SMM driver.
  @param[in] CommunicationBuffer   Pointer to the buffer that contains the communication Message
  @param[in] SourceSize            Size of the memory image to be used for handler.

  @retval EFI_SUCCESS           Callback Function Executed
**/
EFI_STATUS
EFIAPI
HwpLvtSmiCallback (
  IN EFI_HANDLE  SmmImageHandle,
  IN CONST VOID  *ContextData,         OPTIONAL
  IN OUT VOID    *CommunicationBuffer, OPTIONAL
  IN OUT UINTN   *SourceSize           OPTIONAL
)
{
  UINT32  HwpEvent;
  UINT32  HwpInterrupt;
  UINTN   Thread;

  ///
  /// Check whether SMI is due to HWP interrupt.
  ///
  HwpEvent = 0;
  mAcpiParameter->HwpInterrupt |= FALSE;     

  for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
    gSmst->SmmStartupThisAp (HwpLvtEventCheckMsr,
                             Thread, 
                             &HwpEvent);
  }

  if(HwpEvent == 1) {
    ///
    /// Disable HWP Interrupts
    ///
    HwpInterrupt = 0;
    for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
      gSmst->SmmStartupThisAp (EnableDisableHwpInterrupt,
                             Thread, 
                             &HwpInterrupt);
    }
    ///
    /// If HWP interrupt Assert SCI
    ///
    SetSwGpeSts();

    ///
    /// Clear the HWP Interrupt Status
    ///
    for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
      gSmst->SmmStartupThisAp (HwpLvtEventClearStatus,
                             Thread, 
                             NULL);
    }
    ///
    /// Enable HWP Interrupts
    ///
    HwpInterrupt = 1;
    for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
      gSmst->SmmStartupThisAp (EnableDisableHwpInterrupt,
                             Thread, 
                             NULL);
    }
  }
 
  return EFI_SUCCESS;
}

/**
  Initialize the HWP SMI Handler.

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS   The driver installes/initialized correctly.
**/

EFI_STATUS
EFIAPI
HwpLvtSmmInitialization (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  ) 
{
  EFI_STATUS              Status;
  BOOLEAN                 InSmm;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *AcpiNvsProtocol;
  EFI_HANDLE             Handle;
  EFI_SMM_BASE2_PROTOCOL *SmmBase;


  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **) &SmmBase);
  ASSERT_EFI_ERROR (Status);

  Status = SmmBase->InSmm (SmmBase, &InSmm);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  if (!InSmm) {
    return EFI_SUCCESS;
  }

  ///
  /// Locate CPU global NVS area
  ///
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &AcpiNvsProtocol);
  ASSERT_EFI_ERROR (Status);

  mAcpiParameter = AcpiNvsProtocol->Area;

  if((AcpiNvsProtocol->Area->HWPMEnable) && (AcpiNvsProtocol->Area->HwpInterrupt)) {
    Status = gSmst->SmiHandlerRegister (HwpLvtSmiCallback, NULL, &Handle);
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
