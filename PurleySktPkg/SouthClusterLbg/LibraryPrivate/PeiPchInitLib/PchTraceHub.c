/** @file
  PCH TraceHub Initialization file

@copyright
 Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
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

#include "PchInitPei.h"
#include <Ppi/Cache.h>
#include <CpuRegs.h>
#include <Library/PeiServicesTablePointerLib.h>


/**
  This function attempts to set allocated memory as UNCACHEABLE.

  @param[in] PchPolicy              The PCH Policy PPI instance

  @retval EFI_SUCCESS               Memory has been successfuly set as uncacheable.
  @retval EFI_NOT_FOUND             The PPI descriptor is not found in the database.
  @retval EFI_LOAD_ERROR            No empty MTRRs to use.
  @retval EFI_INVALID_PARAMETER     The input parameter to SetCache is not valid.
**/
EFI_STATUS
PchTraceHubSetMemoryAsUncacheable (
  IN EFI_PHYSICAL_ADDRESS Address,
  IN UINT32 Size)
{
   UINT32 Status;
   PEI_CACHE_PPI             *CachePpi;
   CONST EFI_PEI_SERVICES   **PeiServices;

   /// Set this memory regions as uncacheable
   ///
   /// Load Cache PPI
   ///
   Status = PeiServicesLocatePpi (
              &gPeiCachePpiGuid,  // GUID
              0,                  // Instance
              NULL,               // PEI_PPI_DESCRIPTOR
              (VOID **) &CachePpi // PPI
              );
   
   if (Status == EFI_SUCCESS) {
      PeiServices = GetPeiServicesTablePointer ();
      ///
      /// Set the allocated memory area as UNCACHEABLE
      ///
      Status = CachePpi->SetCache (
                  (EFI_PEI_SERVICES **)PeiServices,
                  CachePpi,
                  Address,
                  Size,
                  EFI_CACHE_UNCACHEABLE
                  );
   } else {
     DEBUG ((DEBUG_INFO, "PchTraceHubSetMemoryAsUncached() PeiServicesLocatePpi failed\n"));
     ASSERT_EFI_ERROR (Status);
     
   }

   return Status;
}

/**
  This function performs TraceHub PCH initialization stage on end of PEI phase, when DRAM is available.

  @param[in] PchPolicy         The PCH Policy PPI instance

  @retval EFI_SUCCESS               Memory has been successfuly set as uncachable.
  @retval EFI_OUT_OF_RESOURCES      Requested amount of memory could not be allocated.
  @retval EFI_NOT_FOUND             The Cache PPI descriptor is not found in the database.
  @retval EFI_LOAD_ERROR            No empty MTRRs to use.
  @retval EFI_INVALID_PARAMETER     The input parameter to SetCache is not valid.
**/
EFI_STATUS
PchTraceHubOnEndOfPei (
  IN  PCH_POLICY_PPI          *PchPolicy
  )
{
  UINTN                     TraceHubBaseAddress;
  UINT32                    MemSize = 0;
  EFI_PHYSICAL_ADDRESS      MemRegBuff = 0;
  EFI_PHYSICAL_ADDRESS      MemReg0Buff = 0;
  EFI_PHYSICAL_ADDRESS      MemReg1Buff = 0;
  EFI_BOOT_MODE             BootMode;
  EFI_STATUS                Status;
  UINT32                    ScratchpadReg;

  DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() Start\n"));

  TraceHubBaseAddress = MmPciBase (
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                          PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                          );
  if (MmioRead16 (TraceHubBaseAddress) == 0xFFFF) {
    DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() TraceHub Device not present\n"));
    return EFI_SUCCESS;
  }

  ///
  /// Step 1. On S3 resume skip the rest
  ///
  Status = PeiServicesGetBootMode (&BootMode);
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  ///
  /// Step 2. In case MSE is 0, skip the rest - if MSE = 0 it means Trace Hub has not been initialized earlier
  /// 
  
  if ((MmioRead32 (TraceHubBaseAddress + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) == 0) {
    return EFI_SUCCESS;
  }
  
  ScratchpadReg = MmioRead32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_CSR_MTB_SCRATCHPAD0);
  DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() - R_PCH_TRACE_HUB_CSR_MTB_SCRATCHPAD0 = 0x%08x\n", ScratchpadReg));
  
  if ((ScratchpadReg & BIT24) || (PchPolicy->PchTraceHubConfig.EnableMode != TraceHubModeDisabled)) {
    DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei()- Trace Hub configured to Host Debugger or SCRATCHPAD0_BIT24 set\n"));
  }
  
  ///
  /// Step 3. In case we come here after G3/S5/warm reset, the MemReg0 needs to be allocated according to PchPolicy settings
  ///
  if (((BootMode != BOOT_ON_S4_RESUME) && ((MmioRead32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0CTL) & B_PCH_TRACE_HUB_MTB_MSCNEN) == 0))
     || ((BootMode == BOOT_ON_S4_RESUME) && (MmioRead32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0SIZE)))) {
    MemSize = PchPolicy->PchTraceHubConfig.MemReg0Size + PchPolicy->PchTraceHubConfig.MemReg1Size;
    if (MemSize > 0) {
      if (PchPolicy->PchTraceHubConfig.MemReg0Size > PchPolicy->PchTraceHubConfig.MemReg1Size && PchPolicy->PchTraceHubConfig.MemReg1Size > 0) {
        MemSize = 2 * PchPolicy->PchTraceHubConfig.MemReg0Size;
      }
      if (PchPolicy->PchTraceHubConfig.MemReg1Size > PchPolicy->PchTraceHubConfig.MemReg0Size && PchPolicy->PchTraceHubConfig.MemReg0Size > 0) {
        MemSize = 2 * PchPolicy->PchTraceHubConfig.MemReg1Size;
      }
      MemRegBuff = (EFI_PHYSICAL_ADDRESS) (UINTN) AllocateAlignedReservedPages (EFI_SIZE_TO_PAGES (MemSize), MemSize);
      if (MemRegBuff == 0) {
        DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() Memory allocation failed!\n"));
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      } else {
        Status = PchTraceHubSetMemoryAsUncacheable (MemRegBuff, MemSize);
        if (EFI_ERROR(Status)) {
          DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() Setting memory UNCACHEABLE failed!\n"));
          ASSERT_EFI_ERROR (Status);
          return Status;
        }
        
        ///     Write the base address and buffer size for Memory Region 0 to the the MSC0BAR and MSC0SIZE registers.
        ///     Write the base address and buffer size for Memory Region 1 to the the MSC1BAR and MSC1SIZE registers.
        ///     Note that BAR and Size are shifted left 12 bits by the hardware to arrive at physical starting address and size.
        ///     BIOS, therefore, should shift-right by 12 bits before writing a value to these registers.
        ///     This also forces the starting address and size to be on 4kB boundaries.
        ///
        if (PchPolicy->PchTraceHubConfig.MemReg0Size) {
          MemReg0Buff = MemRegBuff;
          MmioWrite32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0BAR, ((UINT32)(MemReg0Buff)) >> 12);
          DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() MTB_MSC0BAR  = 0x%08x\n", MmioRead32(PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0BAR)));
        
          MmioWrite32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0SIZE, (UINT32) (PchPolicy->PchTraceHubConfig.MemReg0Size >> 12));
          DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() MTB_MSC0SIZE  = 0x%08x\n", MmioRead32(PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC0SIZE)));
        }
          
        if (PchPolicy->PchTraceHubConfig.MemReg1Size) {
          MemReg1Buff = MemRegBuff + PchPolicy->PchTraceHubConfig.MemReg0Size;
          MmioWrite32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC1BAR, ((UINT32)(MemReg1Buff)) >> 12);
          DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() MTB_MSC1BAR  = 0x%08x\n", MmioRead32(PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC1BAR)));
        
          MmioWrite32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC1SIZE, (UINT32) (PchPolicy->PchTraceHubConfig.MemReg1Size >> 12));
          DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() MTB_MSC1SIZE  = 0x%08x\n", MmioRead32(PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_MTB_MSC1SIZE)));
        }
      }
    }
  }

  ///
  /// Hide TraceHub Device using PSF (Config Disable)
  ///
  DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() TraceHub Config Disable\n"));
  PchPcrAndThenOr32 (
    PID_PSF3,
    R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
    (UINT32)~BIT0,
    B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS
    );

  DEBUG ((DEBUG_INFO, "PchTraceHubOnEndOfPei() End\n"));
  return Status;
}

/**
  This function completes power gating management for TraceHub and ensures locking of HSWPGCR1 register.
**/
VOID
TraceHubManagePowerGateControl (
  VOID
  )
{
  UINTN                   TraceHubBaseAddress;
  UINT32                  PchPwrmBase;

  ///
  /// Check if Trace Hub Device is present
  ///
  TraceHubBaseAddress = MmPciBase (
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                          PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                          );
  
  if (MmioRead16 (TraceHubBaseAddress) == 0xFFFF) {
    DEBUG ((DEBUG_INFO, "TraceHubManagePowerGateControl() TraceHub device is not present \n"));
    return;
  }

  PchPwrmBaseGet (&PchPwrmBase);
  
  // If MSE is 0, it hints that the TraceHub is to be disabled
  if ((MmioRead32 (TraceHubBaseAddress + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) == 0) {
    //
    // Program the MTB BAR and set MSE to allow MMIO access
    //
    MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR, PCH_TRACE_HUB_MTB_BASE_ADDRESS);
    MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_UBAR, 0);
    MmioOr8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    
    //
    // Set CTC resync on Tracehub function disable flow to avoid SLP_S0 issues
    //
    MmioOr32 (PCH_TRACE_HUB_MTB_BASE_ADDRESS + R_PCH_TRACE_HUB_CSR_MTB_TSUCTRL, B_PCH_TRACE_HUB_CSR_MTB_TSUCTRL_CTCRESYNC);
    //
    // Clear MTB BAR and MSE
    //
    MmioWrite8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET, 0);
    MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR, 0);
    
    ///
    /// Step 11. Set PCR[PSF_3] + "AGNT_T0_SHDW_PCIEN"[TraceHub][8]
    ///
    DEBUG ((DEBUG_INFO, "TraceHubManagePowerGateControl() Hide config space of Trace Hub device\n"));
    PchPcrAndThenOr32 (
      PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      (UINT32)~0,
      B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
      );
    ///
    /// Step 12. Set PCR[PSF_3] + "AGNT_T0_SHDW_CFG_DIS"[TraceHub_ACPI][0]
    ///
    DEBUG ((DEBUG_INFO, "TraceHubManagePowerGateControl() Disable config space of Trace Hub ACPI device\n"));
    PchPcrAndThenOr32 (
      PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
      (UINT32)~0,
      B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS
      );
    /// If DCI has been enabled, HSWPGCR1 register has already been locked in PchDciConfiguration()
    if ((MmioRead32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1) & B_PCH_PWRM_SW_PG_CTRL_LOCK) == 0) {
      ///
      /// Step 10. Set HSWPGCR1 (PWRMBASE + 0x5D0) [0] and [31] to power gate TraceHub and lock the register
      ///
      MmioOr32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1, (UINT32) (B_PCH_PWRM_SW_PG_CTRL_LOCK));
    }
      
  } else {
    /// If DCI has been enabled, HSWPGCR1 register has already been locked in PchDciConfiguration()
    if ((MmioRead32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1) & (UINT32) B_PCH_PWRM_SW_PG_CTRL_LOCK) == 0) {
      // Step 13. Clear HSWPGCR1 (PWRMBASE+0x5D0)[0] to disable Trace Hub power gating and lock the register
      DEBUG ((DEBUG_INFO, "TraceHubManagePowerGateControl() Locking HSWPGCR1\n"));
      MmioWrite32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1, (UINT32) B_PCH_PWRM_SW_PG_CTRL_LOCK);
    }
  }
}


