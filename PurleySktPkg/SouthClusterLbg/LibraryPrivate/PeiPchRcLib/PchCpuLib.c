/** @file
  This files contains Pch services for RCs usage

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

#include "PchRcLibrary.h"
#include <Ppi/Spi.h>

/**
  The function is used while doing CPU Only Reset, where PCH may be required
  to initialize strap data before soft reset.

  @param[in] Operation                  Get/Set Cpu Strap Set Data
  @param[in, out] CpuStrapSet           Cpu Strap Set Data

  @retval EFI_SUCCESS                   The function completed successfully.
  @exception EFI_UNSUPPORTED            The function is not supported.
**/
EFI_STATUS
EFIAPI
PchCpuStrapSet (
  IN      CPU_STRAP_OPERATION           Operation,
  IN OUT  UINT16                        *CpuStrapSet
  )
{
  UINT32            PchSpiBase;
  UINT32            PchSpiBar0;
  EFI_STATUS        Status;
  EFI_SPI_PROTOCOL  *SpiPpi;
  UINT8             SoftStrapValue[2];

  DEBUG ((DEBUG_INFO, "PchCpuStrapSet() - Start\n"));

  PchSpiBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_SPI,
                 PCI_FUNCTION_NUMBER_PCH_SPI
                 );
  PchSpiBar0  = MmioRead32 (PchSpiBase + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;

  switch (Operation) {
  case GetCpuStrapSetData:
    ///
    /// Get CPU Strap Settings select. 0 = from descriptor, 1 = from PCH
    ///
    if ((MmioRead8 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSMC)) & B_PCH_SPI_SSMC_SSMS) == 0) {
      ///
      /// Read Strap from Flash Descriptor
      ///
      Status = PeiServicesLocatePpi (
                  &gPeiSpiPpiGuid,
                  0,
                  NULL,
                  (VOID **) &SpiPpi
                  );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "PchCpuStrapSet is not available\n"));
        return EFI_UNSUPPORTED;
      }
      
      Status = SpiPpi->ReadCpuSoftStrap (SpiPpi, 0, sizeof (UINT16), SoftStrapValue);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Error reading SPI CPU Soft Strap\n"));
        return EFI_UNSUPPORTED;
      }
      *CpuStrapSet = SoftStrapValue[0] + (SoftStrapValue[1] << 8);
      return EFI_SUCCESS;
    } else {
      ///
      /// Read Strap from PCH Soft Strap.
      ///
      *CpuStrapSet = MmioRead16 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSMD));
    }
    break;

  case SetCpuStrapSetData:
    ///
    /// PCH BIOS Spec Section 4.3 Soft Reset Control
    /// 2. If there are CPU configuration changes, program the strap setting into the
    ///    Soft Reset Data register located at SPIBAR0 Offset F8h [15:0] (PchSpibar0 + Offset F8h [15:0])
    ///    and follow the steps outlined in the "CPU Only Reset BIOS Flow" section of the Processor
    ///    BIOS Writer's Guide and skip steps 3 and 4.
    ///    a. Program Soft Reset Data Register SPIBAR0 + F8h [13:0] (PchSpibar0 + F8h [13:0])
    ///       (details in Processor BIOS Writer's Guide)
    ///    b. Set PchSpibar0 + Offset F4h[0] = 1b
    ///    c. Set PchSpibar0 + Offset F0h[0] = 1b
    ///    d. Skip steps 3 and 4.
    ///
    MmioWrite16 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSMD), *CpuStrapSet);
    MmioOr8 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSMC), B_PCH_SPI_SSMC_SSMS);
    MmioOr8 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSML), B_PCH_SPI_SSML_SSL);
    break;

  case LockCpuStrapSetData:
    MmioOr8 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SSML), B_PCH_SPI_SSML_SSL);
    break;

  default:
    break;
  }

  DEBUG ((DEBUG_INFO, "PchCpuStrapSet() - End\n"));

  return EFI_SUCCESS;
}


/**
  Set Early Power On Configuration setting for feature change.

  @param[in] Operation                  Get or set EPOC data
  @param[in, out] CpuEPOCSet            Cpu EPOC Data

  @retval EFI_SUCCESS                   The function completed successfully.
  @exception EFI_UNSUPPORTED            The function is not supported.
**/
EFI_STATUS
EFIAPI
PchCpuEpocSet (
  IN     CPU_EPOC_OPERATION       Operation,
  IN OUT  UINT32                    *CpuEpocSet
  )
{
  UINTN                   PchPwrmBase;
  EFI_STATUS              Status;

  DEBUG ((DEBUG_INFO, "PchCpuEpocSet() - Start\n"));

  Status = PchPwrmBaseGet (&PchPwrmBase);
  ASSERT_EFI_ERROR (Status);

  switch (Operation) {
    case GetCpuEpocData:
      *CpuEpocSet = MmioRead32 ((UINTN) (PchPwrmBase + R_PCH_PWRM_CPU_EPOC));
      if (*CpuEpocSet == 0xFFFFFFFF) {
        DEBUG ((DEBUG_ERROR, "Cpu Epoc returned invalid data.\n"));
        return EFI_UNSUPPORTED;
      }
      *CpuEpocSet = *CpuEpocSet & 0x3;
      break;
    case SetCpuEpocData:
      MmioBitFieldWrite32 (
        (UINTN) (PchPwrmBase + R_PCH_PWRM_CPU_EPOC),
        0,
        1,
        *CpuEpocSet
        );
      break;
    default:
      break;
  }

  DEBUG ((DEBUG_INFO, "PchCpuEpocSet() - End\n"));
  return EFI_SUCCESS;
}
