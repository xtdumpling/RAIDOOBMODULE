/** @file
  Configures PCH Sata Controller for PCH-H

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
#include "PchInitPei.h"

/**
  Perform PCH SKU detection to check if the RST PCIe Storage Remapping feature is supported
  Prior to any PCIe device enumeration, initial the RST PCIe Storage "Cycle Router Enable (CRE)" bit to 1
  If the CRE bit value after the write is 1, it indicates the bit is RW,and is enabled by fuse
  else, it indicates the bit is RO only and is disabled by fuse
  The value after the write is stored in RST PCIe Storage Cycle Router Global Control regiser, and then CRE bit is cleared

  @retval None
**/
VOID
PchRstPcieStorageCreEarlyInit (
  )
{
  UINTN       i;
  UINT8       Data8;
  UINTN       PciSataRegBase;

  DEBUG ((DEBUG_INFO, "PchRstPcieStorageCreEarlyInit() - Start\n"));

  PciSataRegBase  = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA
                      );

  ///
  /// Read Vendor ID to check if Sata exists
  /// Return if Sata is not exist
  ///
  if (MmioRead16 (PciSataRegBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return;
  }

  ///
  /// Prior to any PCIe device enum, BIOS needs to detect the PCH SKU to
  /// make sure the RST PCIe Storage Remapping feature is supported
  ///
  for (i = 0; i < PCH_MAX_RST_PCIE_STORAGE_CR; i++) {
    ///
    /// Set the related RST PCIe Storage Cycle Router
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT8) i);

    ///
    /// Set the "Port Configuration Check Disable" bit, Sata PCI offset 304h [31]
    ///
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, B_PCH_RST_PCIE_STORAGE_GSR_PCCD);

    ///
    /// Set RST PCIe Storage's Cycle Router Enabled Bit for CR#i
    ///
    MmioOr8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT8) B_PCH_RST_PCIE_STORAGE_GCR_CRE);

    ///
    /// Write the value into SATA PCI offset FC0h[16+i], where i = 0 for RST PCIe Storage Cycle Router 1 and so on
    ///
    Data8 = MmioRead8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR) & B_PCH_RST_PCIE_STORAGE_GCR_CRE;
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT32) (Data8 << (16+i)));

    ///
    /// Clear RST PCIe Storage's Cycle Router Enabled Bit for CR#i
    ///
    MmioAnd8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT8) ~(B_PCH_RST_PCIE_STORAGE_GCR_CRE));

    ///
    /// Clear the "Port Configuration Check Disable" bit, Sata PCI offset 304h [31]
    ///
    MmioAnd32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, (UINT32) ~B_PCH_RST_PCIE_STORAGE_GSR_PCCD);

  }


  DEBUG ((DEBUG_INFO, "PchRstPcieStorageCreEarlyInit() - End\n"));
}

/**
  Check if any lanes are assigned to SATA

  @retval TRUE                    Sata lanes exist
  @retval FALSE                   Sata lanes does not exist
**/
BOOLEAN
SataLaneExist (
  VOID
  )
{
  UINT8                 Index;
  UINT8                 PortId;
  UINT8                 LaneOwner;
  UINT8                 MaxSataPorts;
  UINT8                 LaneNum;

  MaxSataPorts = GetPchMaxSataPortNum ();
    for (Index = 0; Index < MaxSataPorts; Index++) {
      PchGetSataLaneNum (Index, &LaneNum);
      PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        return TRUE;
      }
    }
  return FALSE;
}

/**
  Check if any lanes are assigned to secondary SATA

  @retval TRUE                    Sata lanes exist
  @retval FALSE                   Sata lanes does not exist
**/
BOOLEAN
sSataLaneExist (
  VOID
  )
{
  UINT8                 Index;
  UINT8                 PortId;
  UINT8                 LaneOwner;
  UINT8                 MaxsSataPorts;
  UINT8                 LaneNum;

  MaxsSataPorts = GetPchMaxsSataPortNum ();
    for (Index = 0; Index < MaxsSataPorts; Index++) {
      PchGetsSataLaneNum (Index, &LaneNum);
      PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        return TRUE;
      }
    }
  return FALSE;
}

/**
  //
  // Program the HSIO Settings Specific to the SPT Implementation - Elastic Buffer Configuration
  //
  @retval None
**/
VOID
ProgramSataTestMode (
    IN UINT8            SataControllerNo
  )
{
  UINT16                Offset;
  HSIO_LANE             HsioLane;
  UINT8                 LaneOwner;
  UINT8                 PortId;

  if (PCH_SATA_FIRST_CONTROLLER == SataControllerNo) {
    if (PchGetLaneInfo (18, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (18, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (19, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (19, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (20, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (20, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (21, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (21, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (22, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (22, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (23, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (23, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (24, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (24, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (25, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (25, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
  } else {
    if (PchGetLaneInfo (12, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (12, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (13, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (13, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (14, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (14, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (15, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (15, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (16, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (16, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
    if (PchGetLaneInfo (17, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (17, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (8 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
  }
}

/**
  Program AHCI PI register for Enabled ports
  Handle hotplug, and interlock switch setting,
  and config related GPIOs.

  @param[in] SataConfig           The PCH Policy for SATA configuration
  @param[in] AhciBarAddress       The Base Address for AHCI BAR

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigurePchSataAhci (
  IN  CONST PCH_SATA_CONFIG           *SataConfig,
  IN UINT32                           AhciBarAddress,
  IN UINT8                            SataControllerNo
  )
{
  UINT32                AhciBar;
  UINT32                Data32And;
  UINT32                Data32Or;
  UINT32                PxCMDRegister;
  UINT16                SataPortsEnabled;
  UINT8                 Index;
  UINTN                 PciSataRegBase;
  UINT16                WordReg;
  UINT8                 PortSataControl;
  UINT32                CapRegister;
  PCH_SERIES            PchSeries;
  UINT8                 PortxEnabled;
  UINTN                 PortxEnabledOffset;
  UINT16                PortxEnabledMask;
  UINT8                 MaxSataPortNum;

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "ConfigurePchHSataAhci() - First Controller - Start\n"));
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    MaxSataPortNum = GetPchMaxSataPortNum();
  }
  else if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "ConfigurePchHsSataAhci() - Second Controller - Start\n"));
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_EVA, PCI_FUNCTION_NUMBER_PCH_SSATA);
    MaxSataPortNum = GetPchMaxsSataPortNum();
  }
  else {
    DEBUG ((DEBUG_ERROR, "Error: Invalid SATA controller!\n"));
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Set the AHCI BAR
  ///
  AhciBar = AhciBarAddress;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Enable command register memory space decoding
  ///
  MmioOr16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Get Port Settings
  ///
  PchSeries = GetPchSeries ();
  if (PchSeries == PchH) {
    SataPortsEnabled    = MmioRead16 (PciSataRegBase + R_PCH_H_SATA_PCS);
    SataPortsEnabled    &= (UINT16) (B_PCH_H_SATA_PCS_PXE_MASK);
    PortxEnabled        = B_PCH_H_SATA_PCS_P0E;
    PortxEnabledOffset  = R_PCH_H_SATA_PCS;
    PortxEnabledMask    = B_PCH_H_SATA_PCS_PXE_MASK;
  } else {
    SataPortsEnabled    = MmioRead16 (PciSataRegBase + R_PCH_LP_SATA_PCS);
    SataPortsEnabled    &= (UINT16) (B_PCH_LP_SATA_PCS_PXE_MASK);
    PortxEnabled        = B_PCH_LP_SATA_PCS_P0E;
    PortxEnabledOffset  = R_PCH_LP_SATA_PCS;
    PortxEnabledMask    = B_PCH_LP_SATA_PCS_PXE_MASK;
  }

  //
  // Read the default value of the Host Capabilites Register
  // NOTE: many of the bits in this register are R/WO (Read/Write Once)
  //
  CapRegister = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_CAP);
  CapRegister &= (UINT32)~(B_PCH_SATA_AHCI_CAP_SIS | B_PCH_SATA_AHCI_CAP_SSS | B_PCH_SATA_AHCI_CAP_SALP |
                           B_PCH_SATA_AHCI_CAP_PMS | B_PCH_SATA_AHCI_CAP_SSC | B_PCH_SATA_AHCI_CAP_PSC |
                           B_PCH_SATA_AHCI_CAP_SXS);

  for (Index = 0; Index < MaxSataPortNum; Index++) {
    //
    // Check PCS.PxE to know if the SATA Port x is disabled.
    //
    if ((SataPortsEnabled & (PortxEnabled << Index)) == 0) {
      continue;
    }

    if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
      //
      // Mechanical Presence Switch is Enabled for at least one of the ports
      //
      CapRegister |= B_PCH_SATA_AHCI_CAP_SIS;
    }

    if ((SataConfig->PortSettings[Index].SpinUp == TRUE) ||
        (SataConfig->PortSettings[Index].External == TRUE)) {
      //
      // PCH BIOS Spec Section 14.1.4 Initialize Registers in AHCI Memory-Mapped Space
      // Step 1.4
      // Set SSS (ABAR + 00h[27]) to enable SATA controller supports staggered
      // spin-up on its ports, for use in balancing power spikes
      // SATA Port Spin up is supported at least one of the ports
      // If this is an extra eSATA port. It needs to be hotpluggable but it's usually empty anyway
      // so LPM is not available but Listen Mode is available, so it will have good power management.
      // If Sata Test Mode enabled, then uncoditonally clear SSS (ABAR + 00h[27])
      // to resolve Spin-donw issue with the test equiepment
      //
      if (SataConfig->TestMode == FALSE ) {
        CapRegister |= B_PCH_SATA_AHCI_CAP_SSS;
      }
    }

    if (SataConfig->PortSettings[Index].External == TRUE) {
      //
      // External SATA is supported at least one of the ports
      //
      CapRegister |= B_PCH_SATA_AHCI_CAP_SXS;
    }
  }

  //
  // PCH BIOS Spec Section 14.1.4 Initialize Registers in AHCI Memory-Mapped Space
  // Step 1
  // Set PSC (ABAR + 00h[13]). This bit informs the Windows software driver that the AHCI
  // Controller supports the partial low-power state.
  // Set SSC (ABAR + 00h[14]). This bit informs the Windows software driver that the AHCI
  // Controller supports the slumber low-power state.
  // Set SALP (ABAR + 00h[26]) to enable Aggressive Link Power Management (LPM) support.
  //
  CapRegister |= (B_PCH_SATA_AHCI_CAP_SSC | B_PCH_SATA_AHCI_CAP_PSC);

  if (SataConfig->SalpSupport == TRUE) {
    CapRegister |= B_PCH_SATA_AHCI_CAP_SALP;
  }
  //
  // Support Command List Override & PIO Multiple DRQ Block
  //
  CapRegister |= (B_PCH_SATA_AHCI_CAP_SCLO | B_PCH_SATA_AHCI_CAP_PMD);

  //
  // Configure for the max speed support 1.5Gb/s, 3.0Gb/s and 6.0Gb/s.
  //
  CapRegister &= ~B_PCH_SATA_AHCI_CAP_ISS_MASK;

  switch (SataConfig->SpeedLimit) {
  case PchSataSpeedGen1:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_1_5_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;

  case PchSataSpeedGen2:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_3_0_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;

  case PchSataSpeedGen3:
  case PchSataSpeedDefault:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_6_0_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;
  }
  if (SataConfig->EnclosureSupport == TRUE) {
    CapRegister |= B_PCH_SATA_AHCI_CAP_EMS;
  }

  //
  // Update the Host Capabilites Register
  // NOTE: Many of the bits in this register are R/WO (Read/Write Once)
  //
  MmioWrite32 (AhciBar + R_PCH_SATA_AHCI_CAP, CapRegister);

  ///
  /// Enable SATA ports by setting all PI bits to 1b
  /// If BIOS accesses any of the port specific AHCI address range before setting PI bit,
  /// BIOS is required to read the PI register before the initial write to the PI register.
  /// NOTE: The read before initial write to PI register is done by the MmioAndThenOr32 routine.
  /// NOTE: many of the bits in this register are R/WO (Read/Write Once)
  ///
  MmioOr32 (AhciBar + R_PCH_SATA_AHCI_PI, (UINT32) ~(0));

  ///
  /// After BIOS issues initial write to this register, BIOS is requested to issue two
  /// reads to this register.
  ///
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);

  ///
  /// Set ABAR + 24h[5] to 1b
  /// Set ABAR + 24h[4:2] to 111b
  ///
  Data32Or = B_PCH_SATA_AHCI_CAP2_DESO;
  Data32Or |= B_PCH_SATA_AHCI_CAP2_SDS | B_PCH_SATA_AHCI_CAP2_SADM | B_PCH_SATA_AHCI_CAP2_APST;
  MmioOr32 (AhciBar + R_PCH_SATA_AHCI_CAP2, Data32Or);

  ///
  /// Program all PCS "Port X Enabled" to all 0b,
  /// then program them again to all 1b in order to trigger COMRESET
  ///
  MmioAnd16 (PciSataRegBase + PortxEnabledOffset, (UINT16) ~PortxEnabledMask);
  MmioOr16 (PciSataRegBase + PortxEnabledOffset, (UINT16) PortxEnabledMask);

  ///
  /// Port[Max:0] Command Register update
  /// NOTE: this register must be updated after Port Implemented and Capabilities register,
  /// Many of the bits in this register are R/WO (Read/Write Once)
  ///
  for (Index = 0; Index < MaxSataPortNum; Index++) {
    ///
    /// Check PCS.PxE to know if the SATA Port x is disabled.
    ///
    if ((SataPortsEnabled & (PortxEnabled << Index)) == 0) {
      continue;
    }
    ///
    /// Initial to zero first
    ///
    PxCMDRegister = 0;

    if (SataConfig->PortSettings[Index].HotPlug == TRUE) {
      if (SataConfig->PortSettings[Index].External == FALSE) {
        ///
        /// Hot Plug of this port is enabled
        ///
        PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_HPCP;
        if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
          ///
          /// Mechanical Switch of this port is Attached
          ///
          PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_MPSP;

          ///
          /// Check the GPIO Pin is set as used for native function not a normal GPIO
          ///
          if (GpioIsSataResetPortInGpioMode (Index)) {
            DEBUG ((DEBUG_ERROR,
                    "BIOS must set the SATA%0xGP GPIO pin to native function if Inter Lock Switch is enabled!\n",
                    Index));
            ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
          }

        }
      }
    } else {
      ///
      /// When "Mechanical Switch Attached to Port" (PxCMD[19]) is set, it is expected that HPCP (PxCMD[18]) is also set.
      ///
      if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
        DEBUG ((DEBUG_ERROR, "Hot-Plug function of Port%d should be enabled while the Inter Lock Switch of it is enabled!\n",
        Index));
      }
    }

    if (SataConfig->PortSettings[Index].External == TRUE) {
      PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_ESP;
    }

    ///
    /// Set ALPE
    ///
    PxCMDRegister |= (B_PCH_SATA_AHCI_PxCMD_ALPE);

    ///
    /// If the SATA controller supports staggerred spin-up (SSS in AHCI_CAP is set to 1b),
    /// then set the SUD bit in Port[Max:0] Command Register to 1b
    ///
    if ((CapRegister & B_PCH_SATA_AHCI_CAP_SSS) != 0) {
      PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_SUD;
    }

    MmioAndThenOr32 (
      AhciBar + (R_PCH_SATA_AHCI_P0CMD + (0x80 * Index)),
      (UINT32)~(B_PCH_SATA_AHCI_PxCMD_MASK),
      (UINT32) PxCMDRegister
      );

    ///
    /// DevSleep programming
    /// Set ABAR + 144h[1], ABAR + 1C4h[1], ABAR + 244h[1], ABAR + 2C4[1] to 0b as default
    /// Board rework is required to enable DevSlp.
    /// POR settings are ABAR + 144h[1], ABAR + 1C4h[1], ABAR + 244h[1] = 1b, ABAR + 2C4[1] to 0b
    ///
    if (SataConfig->PortSettings[Index].DevSlp == TRUE) {
      Data32And = (UINT32)~(B_PCH_SATA_AHCI_PxDEVSLP_DITO_MASK | B_PCH_SATA_AHCI_PxDEVSLP_DM_MASK);
      Data32Or  = (B_PCH_SATA_AHCI_PxDEVSLP_DSP | V_PCH_SATA_AHCI_PxDEVSLP_DM_16 | V_PCH_SATA_AHCI_PxDEVSLP_DITO_625);

      if (SataConfig->PortSettings[Index].EnableDitoConfig == TRUE) {
        Data32Or &= Data32And;
        Data32Or |= ((SataConfig->PortSettings[Index].DitoVal << 15) | (SataConfig->PortSettings[Index].DmVal << 25));
      }

      MmioAndThenOr32 (
        AhciBar + (R_PCH_SATA_AHCI_P0DEVSLP + (0x80 * Index)),
        Data32And,
        Data32Or
        );
    } else {
      MmioAnd32 (
        AhciBar + (R_PCH_SATA_AHCI_P0DEVSLP + (0x80 * Index)),
        (UINT32) ~(B_PCH_SATA_AHCI_PxDEVSLP_DSP)
        );
    }

    //
    // eSATA port support only up to Gen2.
    // When enabled, BIOS will configure the PxSCTL.SPD to 2 to limit the eSATA port speed.
    // Please be noted, this setting could be cleared by HBA reset, which might be issued
    // by EFI AHCI driver when POST time, or by SATA inbox driver/RST driver after POST.
    // To support the Speed Limitation when POST, the EFI AHCI driver should preserve the
    // setting before and after initialization. For support it after POST, it's dependent on
    // driver's behavior.
    //
    if ((SataConfig->eSATASpeedLimit == TRUE) &&
        (SataConfig->PortSettings[Index].External == TRUE)) {
      PortSataControl = MmioRead8 (AhciBar + (R_PCH_SATA_AHCI_P0SCTL + (0x80 * Index)));
      PortSataControl &= (UINT8) ~(B_PCH_SATA_AHCI_PXSCTL_SPD);
      PortSataControl |= (UINT8) V_PCH_SATA_AHCI_PXSCTL_SPD_2;
      MmioWrite8 (AhciBar + (R_PCH_SATA_AHCI_P0SCTL + (0x80 * Index)), PortSataControl);
    }
  }

  /// PCH BIOS Spec Section 14.1.5.2 Enable Flexible RAID OROM Features
  /// Lynx Point with RAID capable sku is able to customize the RAID features through setting the
  /// Intel RST Feature Capabilities (RSTF) register before loading the RAID Option ROM. The RAID
  /// OROM will enable the desired features based on the setting in that register, please refer to
  /// PCH EDS for more details.
  /// For example, if the platform desired features are RAID0, RAID1, RAID5, RAID10 and
  /// RST Smart Storage caching. System BIOS should set RSTF (ABAR + C8h [15:0]) to 022Fh before
  /// loading RAID OROM.
  ///
  WordReg = 0;

  if (SataConfig->Rst.HddUnlock == TRUE) {
    ///
    /// If set to "1", indicates that the HDD password unlock in the OS is enabled
    /// while SATA is configured as RAID mode.
    ///
    WordReg |= B_PCH_SATA_AHCI_RSTF_HDDLK;
  }

  if (SataConfig->Rst.LedLocate == TRUE) {
    ///
    /// If set to "1", indicates that the LED/SGPIO hardware is attached and ping to
    /// locate feature is enabled on the OS while SATA is configured as RAID mode.
    ///
    WordReg |= B_PCH_SATA_AHCI_RSTF_LEDL;
  }

  if (SataConfig->SataMode == PchSataModeRaid) {
    if (SataConfig->Rst.Raid0 == TRUE) {
      ///
      /// If set to "1", then RAID0 is enabled in Flexible RAID Option ROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R0E;
    }

    if (SataConfig->Rst.Raid1 == TRUE) {
      ///
      /// If set to "1", then RAID1 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R1E;
    }

    if (SataConfig->Rst.Raid10 == TRUE) {
      ///
      /// If set to "1", then RAID10 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R10E;
    }

    if (SataConfig->Rst.Raid5 == TRUE) {
      ///
      /// If set to "1", then RAID5 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R5E;
    }

    if (SataConfig->Rst.Irrt == TRUE) {
      ///
      /// If set to "1", then Intel Rapid Recovery Technology is enabled.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_RSTE;
    }

    if (SataConfig->Rst.OromUiBanner == TRUE) {
      ///
      /// If set to "1" then the OROM UI is shown.  Otherwise, no OROM banner or information
      /// will be displayed if all disks and RAID volumes are Normal.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_IRSTOROM;
    }

    if (SataConfig->Rst.IrrtOnly == TRUE) {
      ///
      /// If set to "1", then only IRRT volumes can span internal and eSATA drives. If cleared
      /// to "0", then any RAID volume can span internal and eSATA drives.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_IROES;
    }
    ///
    /// Enable the RST Smart Storage caching bit to support Smart Storage caching.
    ///
    if (SataConfig->Rst.SmartStorage == TRUE) {
      WordReg |= B_PCH_SATA_AHCI_RSTF_SEREQ;
    }
    ///
    /// Program the delay of the OROM UI Splash Screen in a normal status.
    ///
    WordReg |= (UINT16) (SataConfig->Rst.OromUiDelay << N_PCH_SATA_AHCI_RSTF_OUD);
  }

  ///
  /// RSTF(RST Feature Capabilities) is a Write-Once register.
  ///
  MmioWrite16 (AhciBar + R_PCH_SATA_AHCI_RSTF, WordReg);

  ///
  /// Disable command register memory space decoding
  ///
  MmioAnd16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Set Ahci Bar to zero
  ///
  AhciBar = 0;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// if in test mode enable, perform misc programming for test mode
  ///
  if (SataConfig->TestMode == TRUE) {
    ProgramSataTestMode (SataControllerNo);
  }

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "ConfigurePchSataAhci() - First Controller - End\n"));
  } else {
    DEBUG ((DEBUG_INFO, "ConfigurePchSataAhci() - Second Controller - End\n"));
  }
  return EFI_SUCCESS;
}


/**
  Disable Sata Controller for PCH-H

  @param[in] AhciBarAddress       The Base Address for AHCI BAR

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
DisablePchHSataController (
  IN UINT32                           AhciBarAddress,
  IN UINT8                            SataControllerNo
  )
{
  UINTN                                 PciSataRegBase;
  UINT32                                AhciBar;
  UINT32                                Data32And;
  UINT32                                Data32Or;
  UINT32                                PchPwrmBase;
  UINTN                                 Index;

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    PciSataRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    DEBUG ((DEBUG_INFO, "DisablePchHSataController() - First Controller - Start\n"));
  }
  else if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    PciSataRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_EVA, PCI_FUNCTION_NUMBER_PCH_SSATA);
    DEBUG ((DEBUG_INFO, "DisablePchHsSataController() - Second Controller - Started\n"));
  }
  else {
    DEBUG ((DEBUG_ERROR, "Error: Invalid SATA controller!\n"));
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Set the AHCI BAR
  ///
  AhciBar = AhciBarAddress;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Enable command register memory space decoding
  ///
  MmioOr16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// PCH BIOS Spec Section 14.3 SATA Controller Disabling
  /// Step 1 - Disable all ports
  /// Set SATA PCI offset 94h[7:0] to all 0b
  ///
  MmioAnd16 (PciSataRegBase + R_PCH_H_SATA_PCS, (UINT16) ~(B_PCH_H_SATA_PCS_PXE_MASK));

  ///
  /// Step 2 - Disable all ports
  /// Clear PI register, ABAR + 0Ch
  ///
  Data32And = (UINT32) (~B_PCH_H_SATA_PORT_MASK);
  MmioAnd32 (AhciBar + R_PCH_SATA_AHCI_PI, Data32And);

  ///
  /// After BIOS issues initial write to this register, BIOS is requested to
  /// issue two reads to this register.
  ///
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);

  ///
  /// Step 3
  /// Clear MSE and IOE, SATA PCI offset 4h[1:0] = 00b
  ///
  MmioAnd16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE));

  ///
  /// Step 4
  /// Set Sata Port Clock Disable bits, SATA PCI offset 90h[7:0] to all 1b
  ///
  MmioOr32 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT32) B_PCH_H_SATA_MAP_PCD);

  ///
  /// Step 5 Enable all SATA dynamic clock gating and dynamic power gating features:
  ///  a. SATA SIR offset 9Ch bit[29, 23] = [1, 1]
  ///  b. SATA SIR offset 8Ch bit[23:16, 7:0] to all 1b
  ///  c. SATA SIR offset A0h bit[15] to [1]
  ///  d. SATA SIR offset 84h bit[23:16] to all 1b
  ///  e. SATA SIR offset A4h bit[26, 25, 24, 14, 5] = [0, 0, 0, 1, 1]
  ///  f. SATA SIR offset A8h bit[17, 16] = [1, 1]
  ///
  /// Step 5a
  /// SATA SIR offset 9Ch bit[29, 23] = [1, 1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (UINT32) (BIT29 | BIT23));

  ///
  /// Step 5b
  /// SATA SIR offset 8Ch bit[23:16, 7:0] to all 1b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_8C);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

  ///
  /// Step 5c
  /// SATA SIR offset A0h bit[15] to [1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A0);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (UINT32) (BIT15));

  ///
  /// Step 5d
  /// SATA SIR offset 84h bit[23:16] to all 1b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_84);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF0000);

  ///
  /// Step 5e
  /// SATA SIR offset A4h bit[26, 25, 24, 14, 5] = [0, 0, 0, 1, 1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A4);
  Data32And = (UINT32)~(BIT26 | BIT25 | BIT24 | BIT14 | BIT5);
  Data32Or  = (UINT32) (BIT14 | BIT5);
  MmioAndThenOr32 (
    PciSataRegBase + R_PCH_SATA_STRD,
    Data32And,
    Data32Or
    );

  ///
  /// Step 5g For each RST PCIe Storage Cycle Router, program the "Extended General Configuration Register" field,
  ///
  ///
  /// LBG SI WA s4930366 - do not enable TCG for NVM
  ///
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    MmioWrite8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT8) Index);
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR,(UINT32) (B_PCH_RST_PCIE_STORAGE_EGCR_CRDCGE | B_PCH_RST_PCIE_STORAGE_EGCR_ICAS | B_PCH_RST_PCIE_STORAGE_EGCR_TSCAS));
  }
  ///
  /// Step 6
  /// Disabling SATA Device by programming SATA SCFD, SATA PCI offset 9Ch[10] = 1
  ///
  MmioOr32 (PciSataRegBase + R_PCH_SATA_SATAGC, (UINT32) (BIT10));

  ///
  /// Step 7
  /// Set PCR[PSF1] AUD PCIEN[8] to 1
  ///
  Data32And = (UINT32)~0;
  Data32Or  = B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS;
  if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    PchPcrAndThenOr32 (
      PID_PSF1, R_PCH_H_PCR_PSF1_T0_SHDW_sSATA_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      Data32And,
      Data32Or
      );
  } else {
    PchPcrAndThenOr32 (
      PID_PSF1, R_PCH_H_PCR_PSF1_T0_SHDW_SATA_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      Data32And,
      Data32Or
      );
  }

  ///
  /// Set Ahci Bar to zero (Note: MSE and IO has been disabled)
  ///
  AhciBar = 0;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Set PWRMBASE + 0x628 [22] = 1b to disable SATA Controller in PMC
  /// No need to clear this non-static PG bit while IP re-enabled since it's reset to 0 when PLTRST.
  ///
  PchPwrmBaseGet (&PchPwrmBase);
  if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_NST_PG_FDIS_1, B_PCH_PWRM_NST_PG_FDIS_1_sSATA_FDIS_PMC);
  } else {
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_NST_PG_FDIS_1, B_PCH_PWRM_NST_PG_FDIS_1_SATA_FDIS_PMC);
  }

  DEBUG ((DEBUG_INFO, "DisablePchHSataController: DisablePchHSataController() Ended\n"));

  return EFI_SUCCESS;
}


/**
  Configures Sata Controller for PCH-H

  @param[in]      SataConfig                The PCH Policy for SATA configuration
  @param[in] RstPcieStorageRemapEnabled     A Boolean to indicate if RST PCIe Storage Remapping is enabled
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR

  @retval EFI_SUCCESS                       The function completed successfully
**/
EFI_STATUS
EarlyConfigurePchHSata (
  IN  CONST PCH_SATA_CONFIG                   *SataConfig,
  IN BOOLEAN                                  RstPcieStorageRemapEnabled,
  IN UINT32                                   AhciBarAddress,
  IN UINT8                                    SataControllerNo
  )
{
  EFI_STATUS              Status;
  UINT8                   SataGcReg;
  UINTN                   PciSataRegBase;
  UINTN                   PciLpcRegBase;
  UINT16                  LpcDeviceId;
  UINT32                  Data32And;
  UINT32                  Data32Or;
#ifdef FSP_FLAG
  UINT16                  SataPortsEnabled;
#endif // FSP_FLAG

  BOOLEAN                 SataLaneExists = FALSE;

  UINT8                   MaxSataPortNum;

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "EarlyConfigurePchHSata() - First Controller - Start\n"));
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    MaxSataPortNum = GetPchMaxSataPortNum();
  }
  else if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "EarlyConfigurePchHSata() - Second Controller - Start\n"));
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_EVA, PCI_FUNCTION_NUMBER_PCH_SSATA);
    MaxSataPortNum = GetPchMaxsSataPortNum();
  }
  else {
    DEBUG ((DEBUG_ERROR, "Error: Invalid SATA controller!\n"));
    return EFI_INVALID_PARAMETER;
  }

  PciLpcRegBase     = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC);
  LpcDeviceId       = MmioRead16 (PciLpcRegBase + PCI_DEVICE_ID_OFFSET);
  Status            = EFI_SUCCESS;

  ///
  /// If Sata is disabled or no lanes assigned to SATA when in AHCI mode
  /// perform the disabling steps to function disable the SATA Controller
  ///
  if (IsSimicsPlatform()){
    SataLaneExists = TRUE;
  } else {
    if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER){
      SataLaneExists = SataLaneExist();
    }
    if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER){
      SataLaneExists = sSataLaneExist();
    }
  }
  if ((SataConfig->Enable == FALSE) || ((!SataLaneExists) && (SataConfig->SataMode == PchSataModeAhci)))
  {
    DisablePchHSataController (AhciBarAddress, SataControllerNo);
    return Status;
  }

  ///
  /// PCH BIOS Spec section 14.1.6 Additional Programming Requirements during
  /// SATA Initialization
  /// Step 1
  /// System BIOS must set SATA SIR Index 98h[8:0] = 183h
  /// as part of the chipset initialization prior to SATA configuration.
  /// These bits should be restored while resuming from a S3 sleep state.
  ///
  Data32And = (UINT32)~(BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  Data32Or  = 0x183;
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_98);
  MmioAndThenOr32 (PciSataRegBase + R_PCH_SATA_STRD, Data32And, Data32Or);
  ///
  /// Step 1a
  /// Server BIOS must set SATA SIR Index 98h[15] = 1b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_98);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, BIT15);

  ///
  /// Step 2
  /// Set SATA SIR Index A4h[6] = 1b
  /// These bits should be restored while resuming from a S3 sleep state
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A4);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, BIT6);

  ///
  /// Step 3
  /// System BIOS must program SATA Hsio table as stated in Table 7-7 to 7-8 BEFORE the SATA
  /// ports are enabled.
  ///

  ///
  /// PCH BIOS Spec section 14.1.7 Additional Programming Requirements during
  /// SATA Initialization
  /// Step 4
  /// Set SATA SIR Index 9Ch[22] = 1b for desktop and mobile platform only
  /// (Server may leave as default)
  ///
  if (IS_PCH_LPC_DEVICE_ID_DESKTOP (LpcDeviceId) ||
      IS_PCH_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, BIT22);
  }

  ///
  /// Step 4
  /// Program SATA SIR Index 9Ch[29,23,20,19,18] to all 1b
  /// Program SATA SIR Index 9Ch[12:7] = 04h
  /// Program SATA SIR Index 9Ch[6:5] to 01b
  /// Program SATA SIR Index 9Ch[3:2] to 01b
  /// Note: If [3:2] value recommendation get changed, please refer to definition if Bit 1:0 and program per table accordingly
  ///
  Data32And = (UINT32) (~(BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT3 | BIT2));
  Data32Or  = (UINT32) (BIT29 | BIT23 | BIT20 | BIT19 | BIT18 | BIT9 | BIT5 | BIT2);
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
  MmioAndThenOr32 (
   PciSataRegBase + R_PCH_SATA_STRD,
   Data32And,
   Data32Or
   );

  ///
  /// Step 4.1 (Server PCH only)
  /// Disable Trunk Clock Gating on the SATA block per LBG Arch Si request s4929226 (applies only for the first controller)
  /// sSATA must have TCG enabled
  ///
  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    Data32Or  = (UINT32) (BIT30);
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
    MmioOr32 (
     PciSataRegBase + R_PCH_SATA_STRD,
     Data32Or
     );
  }

  ///
  /// Step 5
  /// Program SATA PCI offset 9Ch[5] to 1b
  /// Note: Bits 7:0 are RWO, perform byte write here, program the 9Ch[31] to 1b later in PchOnEndOfDxe ()
  ///
  SataGcReg = MmioRead8 (PciSataRegBase + R_PCH_SATA_SATAGC);
  SataGcReg |= BIT5;

  // We used to check if SATA class code is a Mass Storage RAID device(0x4).
  // Unfortunately PCIe class code is configured later in this function
  // using SataConfig->SataMode. Therefore we cannot use R_PCI_SCC_OFFSET
  // here to know if SATA device is configured as RAID and if it supports it.
  // Instead we will use same conditions than Sata class code configuration.

  if (SataConfig->SataMode == PchSataModeRaid) {
    if (IS_PCH_RAID_AVAILABLE (LpcDeviceId)) {
      if (SataConfig->Rst.RaidAlternateId == TRUE) {
        SataGcReg |= B_PCH_SATA_SATAGC_AIE;
        SataGcReg &= ~B_PCH_SATA_SATAGC_AIES;
      } else {
        SataGcReg &= ~B_PCH_SATA_SATAGC_AIE;
        SataGcReg |= B_PCH_SATA_SATAGC_AIES;
      }
    }
  }

  ///
  /// If RstPcieStorageRemapEnabled is TRUE, program Sata PCI offset 9Ch, bit[4:3] and bit[2:0] to [00'b] and [110'b]
  /// else, program Sata PCI offset 9Ch, bit[4:3] and bit[2:0] to [10'b] and [000'b]
  ///
    if (RstPcieStorageRemapEnabled) {
      SataGcReg |= V_PCH_SATA_SATAGC_ASSEL_512K;
      SataGcReg &= ~(B_PCH_SATA_SATAGC_MSS);
    } else {
      SataGcReg &= ~B_PCH_SATA_SATAGC_ASSEL;
      SataGcReg |= V_PCH_SATA_SATAGC_MSS_8K << N_PCH_SATA_SATAGC_MSS;
    }

  //
  // LBG Si WA s4930561 Set ASSEL to 512k in each case - LBG HW has a bug for other settings
  //
  SataGcReg &= ~B_PCH_SATA_SATAGC_ASSEL;
  SataGcReg |= V_PCH_SATA_SATAGC_ASSEL_512K;

  ///
  /// Unconditional write is necessary to lock the register
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SATAGC, SataGcReg);

  ///
  /// Program PCS "Port X Enabled", SATA PCI offset 94h[7:0] to all 1b.
  ///
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_PCS, (UINT16) B_PCH_H_SATA_PCS_PXE_MASK);

  ///
  /// PCH BIOS Spec section 19.10
  /// Step 4.2
  /// After configuring Port and Control Status (PCS) Register Port x Enabled (PxE) bits accordingly,
  /// wait 1.4 micro second
  ///
  MicroSecondDelay (0x02);

  DEBUG ((DEBUG_INFO, "EarlyConfigurePchHSata() End\n"));

  return Status;
}

/**
  Additional Sata configuration for PCH-H

  @param[in]      SataConfig                The PCH Policy for SATA configuration
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR
**/
VOID
ConfigurePchHSata (
  IN CONST PCH_SATA_CONFIG                    *SataConfig,
  IN UINT32                                   AhciBarAddress,
  IN UINT8                                    SataControllerNo
  )
{
  EFI_STATUS              Status;
  UINTN                   PciSataRegBase;
  UINTN                   PciLpcRegBase;
  UINT16                  LpcDeviceId;
  UINTN                   MaxSataPorts;
  UINT32                  DisableDynamicPowerGateBitMap;
  UINT8                   Index;
  UINT32                  Data32And;
  UINT32                  Data32Or;
#ifdef FSP_FLAG
  UINT16                  SataPortsEnabled;
#endif // FSP_FLAG

    if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
      DEBUG ((DEBUG_INFO, "ConfigurePchHSata() - First Controller - Start\n"));
    } else {
      DEBUG ((DEBUG_INFO, "ConfigurePchHSata() - Second Controller - Start\n"));
    }

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    MaxSataPorts = GetPchMaxSataPortNum ();
  }else{
    PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SSATA, PCI_FUNCTION_NUMBER_PCH_SSATA);
    MaxSataPorts = GetPchMaxsSataPortNum ();
  }
  PciLpcRegBase     = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC);
  LpcDeviceId       = MmioRead16 (PciLpcRegBase + PCI_DEVICE_ID_OFFSET);

  ///
  /// Configure AHCI
  ///
  Status = ConfigurePchSataAhci (
             SataConfig,
             AhciBarAddress,
             SataControllerNo
             );

  ///
  /// PCH BIOS Spec Section 14.1.6 Power Optimizer Configuration
  /// System BIOS must execute the following steps as part of System BIOS initialization
  /// of the PCH SATA controller on both cold boot (G3/S5) and S3/S4 resume path. Please
  /// refer to the PCH EDS, section 14.1.35.1 for the SATA initialization settings and
  /// the actual register indexes/values to be programmed.
  ///
  if (SataConfig->PwrOptEnable == TRUE) {

    //
    // When platform design has one of below condition,
    // BIOS has to set SATA PHYDPGE=0 and SQOFFIDLED=1 for specific the SATA port.
    //   - SATA hot-plug enabled port (PxCMD.HPCP = 1)
    //   - SATA external port (PxCMD.ESP = 1)
    //   - SATA slimline port with zero-power ODD (ZPODD) attached (or other AN capable ODD)
    //
    // Set PHYDPGE=0 and SQOFFIDLED=1 for selected ports.
    //
    // PCH-LP
    //   [MPHY Dynamic Power gating] SIR offset 50h[18:16] bit16=port0... bit18=port2
    //   [Slumber Squelch off] SIR offset 54h[2:0]  bit0=port0 ... bit2=port2
    //
    // PCH-H
    //   [MPHY Dynamic Power gating] SIR offset 90h[7:0] . bit0=port0...bit7=port7
    //   [Slumber Squelch off] SIR offset 80h[16:23] . bit16=port0... bit23=port7
    //
    DisableDynamicPowerGateBitMap = 0;
    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (SataConfig->PortSettings[Index].Enable) {
        if (SataConfig->PortSettings[Index].External ||
            SataConfig->PortSettings[Index].HotPlug  ||
            SataConfig->PortSettings[Index].ZpOdd) {
          DisableDynamicPowerGateBitMap |= 1u << Index;
        }
      }
    }

    ///
    /// Step 1
    /// Set SATA SIR Index 90h[7:0] = 1'S
    ///
    Data32Or = 0xFF;
    //
    //  [MPHY Dynamic Power gating] SIR offset 90h[7:0] . bit0=port0...bit7=port7
    //
    Data32Or &= (UINT32)~DisableDynamicPowerGateBitMap;
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_90);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, Data32Or);

    //
    //  [Slumber Squelch off] SIR offset 80h[16:23] . bit16=port0... bit23=port7
    //
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_80);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (DisableDynamicPowerGateBitMap << 16));

    ///
    /// Set SATA SIR Index 8Ch[23:16, 7:0] = 1'S
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_8C);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

    ///
    /// Set SATA SIR Index A0h[15] = 1b, [23:18] = 010110b
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A0);
    MmioAndThenOr32 (
      PciSataRegBase + R_PCH_SATA_STRD,
      (UINT32)~(BIT23 | BIT21 | BIT18),
      BIT22 | BIT20 | BIT19 | BIT15
      );

    ///
    /// Step 2
    /// Set SATA SIR Index 84h[23:16, 7:0] = 1'S
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_84);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

    ///
    /// Set SATA SIR Index A4h[14] = 1b
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A4);
    MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, BIT14);

    ///
    /// Step 3
    /// Set SATA SIR Index CCh[31:0] = 883C9001h
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_CC);
    MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x883C9001);

    ///
    /// Step 4
    /// Set SATA SIR Index D0h[15:0] = 880Ah
    ///
    Data32And = 0xFFFF0000;
    Data32Or  = 0x00008828;
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_D0);
    MmioAndThenOr32 (
      PciSataRegBase + R_PCH_SATA_STRD,
      Data32And,
      Data32Or
      );

    ///
    /// Step 5
    /// Set SATA SIR Index C8h[3] = 1b
    ///
    Data32And = 0xFFFFFFF7;
    Data32Or  = 0x00000008;
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_C8);
    MmioAndThenOr32 (
      PciSataRegBase + R_PCH_SATA_STRD,
      Data32And,
      Data32Or
      );

    ///
    /// Step 6
    /// Set SATA SIR Index C8h[0] = 1b
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_C8);
    Data32And = 0xFFFFFFFE;
    Data32Or  = 0x00000001;
    MmioAndThenOr32 (
      PciSataRegBase + R_PCH_SATA_STRD,
      Data32And,
      Data32Or
      );
  ///
  /// Step 5
  /// Set SATA SIR Index C8h[1] = 1b
  ///
    MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_C8);
    Data32And = 0xFFFFFFFD;
    Data32Or  = 0x00000002;
    MmioAndThenOr32 (
      PciSataRegBase + R_PCH_SATA_STRD,
      Data32And,
      Data32Or
      );
  }

  ///
  /// Set SATA SIR Index A8h[19:16] = 0000b
  /// All power gating policy should be disabled
  /// SATA doesn't support dynamic power gating in LBG
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A8);
  MmioAnd32 (PciSataRegBase + R_PCH_SATA_STRD, (UINT32)~(BIT19 | BIT18 | BIT17 | BIT16));

  ///
  /// Set SATA SIR Index D4h[29:24] = 101100b,
  ///                       [21:16] = 011110b,
  ///                       [12:8]  = 10001b,
  ///                       [4:0]   = 01000b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_D4);
  MmioAndThenOr32 (
    PciSataRegBase + R_PCH_SATA_STRD,
    (UINT32)~(BIT28 | BIT25 | BIT24 | BIT21 | BIT16 | BIT11 | BIT10 | BIT9 | BIT4 | BIT2 | BIT1 | BIT0),
    BIT29 | BIT27 | BIT26 | BIT20 | BIT19 | BIT18 | BIT17 | BIT12 | BIT8 | BIT3
    );

  ///
  /// Initialize the SATA mode to be in AHCI, then check the SATA mode from the policy
  /// If RAID mode is selected and the SKU supports RAID feature, set the SATA Mode Select to 1b (RAID)
  ///
  MmioAnd8 (PciSataRegBase + R_PCH_SATA_SATAGC + 2,(UINT8) ~(BIT0));

  if (SataConfig->SataMode == PchSataModeRaid) {
    if (IS_PCH_RAID_AVAILABLE (LpcDeviceId)) {
      MmioAndThenOr8 (
        PciSataRegBase + R_PCH_SATA_SATAGC + 2,
        (UINT8) ~(BIT0),
        (UINT8) (V_PCH_H_SATA_SATAGC_SMS_RAID)
        );
    } else {
      DEBUG ((DEBUG_INFO, "PCH Device ID : 0x%x\n", LpcDeviceId));
      DEBUG ((DEBUG_ERROR, "This SKU doesn't support RAID feature. Set to AHCI mode.\n"));
    }
  }
#ifdef FSP_FLAG
  //
  // If FSP build is enabled, don't perform dynamic port disable
  // Set the Sata Port Enable/Disable based on policy only
  //
  SataPortsEnabled  = 0;
  for (Index = 0; Index < MaxSataPorts; Index++) {
    SataPortsEnabled |= (SataConfig->PortSettings[Index].Enable << Index);
  }

  //
  // Set MAP."Sata PortX Disable", SATA PCI offset 90h[23:16] to 1b if SATA Port 0/1/2/3/4/5/6/7 is disabled
  //
  MmioOr32 (PciSataRegBase + R_PCH_H_SATA_MAP, ((~SataPortsEnabled << N_PCH_H_SATA_MAP_SPD) & (UINT32) B_PCH_H_SATA_MAP_SPD));

  //
  // Program PCS "Port X Enabled", SATA PCI offset 94h[7:0] = Port 0~7 Enabled bit as per SataPortsEnabled value.
  //
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_PCS, SataPortsEnabled);

  //
  // Program SATA PCI offset 9Ch [31] to 1b
  //
  MmioOr32 ((UINTN) (PciSataRegBase + R_PCH_SATA_SATAGC), BIT31);
#endif // FSP_FLAG

  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    DEBUG ((DEBUG_INFO, "ConfigurePchHSata() - First Controller - End\n"));
  } else {
    DEBUG ((DEBUG_INFO, "ConfigurePchHSata() - Second Controller - End\n"));
  }

}

