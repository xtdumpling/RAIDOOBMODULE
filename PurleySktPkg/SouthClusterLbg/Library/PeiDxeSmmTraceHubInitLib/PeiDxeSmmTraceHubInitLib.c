/** @file
  Pei/Dxe/Smm TraceHub Init Lib.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Library/TraceHubInitLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/TraceHubInitLib.h>

/**
  Set TraceHub base address.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program TraceHub PCI Offset 0x70~0x77 to the 64-bit base address.
  2. Program PCR[PSF3] + TraceHub RS0 offset 0x000 and 0x004 to TraceHub 64-bit base address.
  3. Manually write 1 to MSEN, PCR[PSF3] + TraceHub RS0 offset 0x01C[1] to activate the shadow.

  @param[in] AddressHi                   High 32-bits for TraceHub base address.
  @param[in] AddressLo                   Low 32-bits for TraceHub base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
TraceHubBaseSet (
  UINT32                                AddressHi,
  UINT32                                AddressLo
  )
{
  UINTN                                 TraceHubBase;
  UINT32                                Dmic;

  //
  // check address valid
  //
  if ((AddressLo & 0x0003FFFF) != 0) {
    DEBUG((DEBUG_ERROR, "TraceHubBaseSet Error. Invalid Address: %x.\n", AddressLo));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  PchPcrRead32 (PID_DMI, R_PCH_PCR_DMI_DMIC, &Dmic);
  if ((Dmic & B_PCH_PCR_DMI_DMIC_SRL) != 0) {
    DEBUG((DEBUG_ERROR, "TraceHubBaseSet Error. DMIC.SRL is set.\n"));
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  TraceHubBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                   PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                   );
  if (MmioRead16 (TraceHubBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  //
  // Program TraceHub PCI Offset 0x70~0x77 to base address.
  //
  MmioWrite32 (TraceHubBase + R_PCH_TRACE_HUB_FW_LBAR, AddressLo);
  MmioWrite32 (TraceHubBase + R_PCH_TRACE_HUB_FW_UBAR, AddressHi);
  //
  // Program PCR[PSF3] + TraceHub ACPI BASE, offset 0x00 (BAR0) and offset 0x04 (BAR1) to TraceHub base address.
  //
  PchPcrWrite32 (PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_BAR0, AddressLo);
  PchPcrWrite32 (PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_BAR1, AddressHi);
  //
  // Manually write 1 to MSEN, PCR[PSF3] + TraceHub ACPI base, offset 0x01C[1] to activate the shadow.
  //
  PchPcrAndThenOr8 (
    PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
    0xFF,
    B_PCH_PCR_PSFX_T0_SHDW_PCIEN_MEMEN
    );

  return EFI_SUCCESS;
}

/**
  This function performs basic initialization for TraceHub
  This routine will consume address range 0xFE0C0000 - 0xFE3FFFFF for BARs usage.
  Although controller allows access to a 64bit address resource, PEI phase is a 32bit env,
  addresses greater than 4G is not allowed by CPU address space.
  So, the addresses must be limited to below 4G and UBARs should be set to 0.
  If this routine is called by platform code, it is expected EnableMode is passed in as PchTraceHubModeDisabled, 
  relying on the Intel TH debugger to enable it through the "cratchpad0 bit [24]".
  By this practice, it gives the validation team the capability to use single debug BIOS 
  to validate the early trace functionality and code path that enable/disable Intel TH using BIOS policy.

  @param[in] EnableMode                 Trace Hub Enable Mode
  @param[in] Destination                Trace Hub Trace Destination

**/
VOID
TraceHubInitialize (
  IN  UINT8                             EnableMode
 )
{
  UINTN                   TraceHubBaseAddress;
  UINT32                  PchPwrmBase;

  DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Start\n"));

  PchPwrmBaseGet (&PchPwrmBase);
  /// 
  /// Step 1. Disable power gating in case it was power gated in previous boot
  /// 
  MmioWrite8 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1, 0);
  MmioRead8 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1);
  
  //
  // Check if Trace Hub Device is present
  //
  TraceHubBaseAddress = MmPciBase (
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                          PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                          );

  if (MmioRead16 (TraceHubBaseAddress) == 0xFFFF) {
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - End. TraceHub device is not present \n"));
    return;
  }

  ///
  /// Step 2. Check MSE. Perform initialization only when it has not been set yet.
  ///
  if ((MmioRead32 (TraceHubBaseAddress + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) == EFI_PCI_COMMAND_MEMORY_SPACE) {
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - End. Early init done already \n"));
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - FW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR)));
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - MTB_LBAR = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR)));
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - SW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));
    if (MmioRead32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_CSR_MTB_SCRATCHPAD0) & BIT24) {
      DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Trace Hub enabled due to SCRPD0.24\n"));
    }
    return;
  }
  
  ///
  /// Step 3. Program the MTB Base Address Register fixed BAR and enable MSE
  ///
  // Set MTB_LBAR (PCI offset 0x10)
  DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Setting MTB_BAR\n"));
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR, PCH_TRACE_HUB_MTB_BASE_ADDRESS);
  
  //
  // Clear MTB_UBAR (PCI offset 0x14)
  //
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_UBAR, 0);
  
  //
  // Enable TraceHub to claim memory accesses
  //
  MmioOr8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  
  ///
  /// Step 4. Check if STT is disconnected and if user requested disabling of Trace Hub
  ///
  if (((MmioRead32 ((UINT32) (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_CSR_MTB_SCRATCHPAD0)) & BIT24) == 0)
      && (EnableMode == TraceHubModeDisabled)) {
    ///
    /// Step 4.1 Clear MSE
    ///
    MmioWrite8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, 0);
    ///
    /// Step 4.2 Clear MTB_BAR
    ///
    //
    // Clear MTB_LBAR (PCI offset 0x10)
    //
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Clearing MTB_BAR\n"));
    MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR, 0);
    
    DEBUG ((DEBUG_INFO, "TraceHubInitialize() - End. STT disconnected and Trace Hub requested to be disable\n"));
    return;
  }

  //
  // Program GTH_FREQ to 357MHz
  //
  MmioWrite32 ((UINT32) (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_GTH_FREQ), 0x15476340);
  
  ///
  /// Step. 5 Clear MSE
  ///
  MmioWrite8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, 0);
  
  ///
  /// Step 6. Program the SW Base Address Register with fixed BAR
  ///
  //
  // SW_LBAR (PCI offset 0x18)
  //
  DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Setting SW_BAR\n"));
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR, PCH_TRACE_HUB_SW_BASE_ADDRESS);
  //
  // SW_UBAR (PCI offset 0x1C)
  //
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_UBAR, 0);
  
  ///
  /// Step 7. Program the FW BAR and Shadow PCI Device
  /// (PCI offset 0x70+0x74) - aka FTMR
  ///
  /// At this point, a shadow PCI device (0/20/4) within the backbone PSF needs to be programmed
  /// with the value of FW BAR, have its memory space enabled, and then hide the shadow device
  ///
  DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Setting FW_BAR\n"));
  TraceHubBaseSet (0, (UINT32)PCH_TRACE_HUB_FW_BASE_ADDRESS);

  
  ///
  /// Step 8. Enable MSE and BME
  ///
  DEBUG ((DEBUG_INFO, "TraceHubInitialize() - Enabling MSE and BME\n"));
  MmioOr8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
  
  DEBUG ((DEBUG_INFO, "TraceHubInitialize () Assigned BARs:\n"));
  DEBUG ((DEBUG_INFO, "TraceHubInitialize () FW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR)));
  DEBUG ((DEBUG_INFO, "TraceHubInitialize () MTB_LBAR = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR)));
  DEBUG ((DEBUG_INFO, "TraceHubInitialize () SW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));
  
  ///
  /// Step 9. Set destination sources for BIOS debug messages
  ///
  MmioWrite32 ((UINT32) (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_SWDEST_4), 0x88888888);

  DEBUG ((DEBUG_INFO, "TraceHubInitialize () - End\n"));
}


