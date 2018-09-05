/** @file
  PCH SPI PEI Library implements the SPI Host Controller Compatibility Interface.

@copyright
 Copyright (c) 2004 - 2017 Intel Corporation. All rights reserved
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
#include "PchSpi.h"

/**
  Hide SPI controller before OS avoid BAR0 changed.
**/
VOID
HideSpiController (
  VOID
  )
{
  UINTN                                 PchSpiBase;

  PchSpiBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_SPI,
                 PCI_FUNCTION_NUMBER_PCH_SPI
                 );

  ///
  /// BWG 3.5.1, For SKL PCH-LP/H B0 stepping and above, the SPI configuration space
  /// is to be hidden from OS by setting the SPI PCI offset DCh [9].
  ///
  MmioAndThenOr8 (
    PchSpiBase + R_PCH_SPI_BC + 1,
    (UINT8)~(B_PCH_SPI_BC_SYNC_SS >> 8),
    (B_PCH_SPI_BC_OSFH >> 8)
    );
}

/**
  Configure SPI device after memory.

  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
ConfigureSpiAfterMem (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  if (PchPolicyPpi->SpiConfig.ShowSpiController == FALSE) {
    //
    // Hide SPI controller before OS avoid BAR0 changed.
    //
    HideSpiController ();
  }
}

/**
  PCI Enumeratuion is not done till later in DXE
  Initlialize SPI BAR0 to a default value till enumeration is done
  also enable memory space decoding for SPI

**/
VOID
InitSpiBar0 (
  VOID
  )
{
  UINTN                                 PchSpiBase;
  PchSpiBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_SPI,
                 PCI_FUNCTION_NUMBER_PCH_SPI
                 );
  MmioWrite32 (PchSpiBase + R_PCH_SPI_BAR0, PCH_SPI_BASE_ADDRESS);
  MmioOr32 (PchSpiBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
}

/**
  Installs PCH SPI PPI

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
EFIAPI
InstallPchSpi (
  VOID
  )
{
  EFI_STATUS        Status;
  PEI_SPI_INSTANCE  *PeiSpiInstance;
  SPI_INSTANCE      *SpiInstance;

  DEBUG ((DEBUG_INFO, "InstallPchSpi() Start\n"));

  //
  // PCI Enumeratuion is not done till later in DXE
  // Initlialize SPI BAR0 to a default value till enumeration is done
  // also enable memory space decoding for SPI
  //
  InitSpiBar0 ();

  PeiSpiInstance = (PEI_SPI_INSTANCE *) AllocateZeroPool (sizeof (PEI_SPI_INSTANCE));
  if (NULL == PeiSpiInstance) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiInstance = &(PeiSpiInstance->SpiInstance);
  SpiProtocolConstructor (SpiInstance);

  PeiSpiInstance->PpiDescriptor.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiSpiInstance->PpiDescriptor.Guid  = &gPeiSpiPpiGuid;
  PeiSpiInstance->PpiDescriptor.Ppi   = &(SpiInstance->SpiProtocol);

  ///
  /// Install the SPI PPI
  ///
  Status = PeiServicesInstallPpi (&PeiSpiInstance->PpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "SPI PPI Installed\n"));

  DEBUG ((DEBUG_INFO, "InstallPchSpi() End\n"));

  return Status;
}

/**
  Acquire pch spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval PchSpiBar0              return SPI MMIO address
**/
UINTN
AcquireSpiBar0 (
  IN  SPI_INSTANCE                *SpiInstance
  )
{
  return MmioRead32 (SpiInstance->PchSpiBase + R_PCH_SPI_BAR0) & ~(B_PCH_SPI_BAR0_MASK);
}

/**
  Release pch spi mmio address. Do nothing.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval None
**/
VOID
ReleaseSpiBar0 (
  IN  SPI_INSTANCE                *SpiInstance
  )
{
}

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  )
{
  UINTN                           SpiBaseAddress;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SPI,
                     PCI_FUNCTION_NUMBER_PCH_SPI
                     );
  if ((MmioRead8 (SpiBaseAddress + R_PCH_SPI_BC) & B_PCH_SPI_BC_EISS) != 0) {
    return EFI_ACCESS_DENIED;
  }
  ///
  /// Enable the access to the BIOS space for both read and write cycles
  ///
  MmioOr8 (
    SpiBaseAddress + R_PCH_SPI_BC,
    B_PCH_SPI_BC_WPD
    );

  return EFI_SUCCESS;
}

/**
  This function is a hook for Spi to enable BIOS Write Protect


**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  )
{
  UINTN                           SpiBaseAddress;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SPI,
                     PCI_FUNCTION_NUMBER_PCH_SPI
                     );
  ///
  /// Disable the access to the BIOS space for write cycles
  ///
  MmioAnd8 (
    SpiBaseAddress + R_PCH_SPI_BC,
    (UINT8) (~B_PCH_SPI_BC_WPD)
    );
}