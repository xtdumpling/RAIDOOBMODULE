/** @file
  PCH BIOS Write Protect Driver.

@copyright
 Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
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
#include "PchInitSmm.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED PCH_TCO_SMI_DISPATCH_PROTOCOL     *mPchTcoSmiDispatchProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             mSpiRegBase;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_ESPI_SMI_DISPATCH_PROTOCOL    *mEspiSmmDispatchProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             mEspiRegBase;

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchSpiBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  ///
  /// Disable BIOSWE bit to protect BIOS
  ///
  MmioAnd8 ((UINTN) (mSpiRegBase + R_PCH_SPI_BC), (UINT8) ~B_PCH_SPI_BC_WPD);
}

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchLpcBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  ///
  /// Disable BIOSWE bit to protect BIOS
  ///
  MmioAnd8 ((UINTN) (mEspiRegBase + R_PCH_LPC_BC), (UINT8) ~B_PCH_LPC_BC_WPD);
}

/**
  Entry point for Pch Bios Write Protect driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
**/
EFI_STATUS
EFIAPI
InstallPchBiosWriteProtect (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;
  UINT8                   BiosControl;

  DEBUG ((DEBUG_INFO, "InstallPchBiosWriteProtect()\n"));

  if (mPchPolicyHob->LockDownConfig.BiosLock != TRUE) {
    return EFI_SUCCESS;
  }

  mSpiRegBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_SPI,
                   PCI_FUNCTION_NUMBER_PCH_SPI
                   );
  
  mEspiRegBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );
// APTIOV_SERVER_OVERRIDE_RC_START : EISS bit can't be set before NVRAMSMI
{  
  DEBUG ((DEBUG_INFO, "PchBiosWriteProtect: SpiEiss is %x.\n", mPchPolicyHob->LockDownConfig.SpiEiss));
  if (mPchPolicyHob->LockDownConfig.SpiEiss == 1) {
    // Set SPI EISS (SPI PCI offset DCh[5])
    // Set LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5])
    MmioOr8 (mSpiRegBase + R_PCH_SPI_BC, B_PCH_SPI_BC_EISS);
    MmioOr8 (mEspiRegBase + R_PCH_LPC_BC, B_PCH_LPC_BC_EISS);
  } else {
    // Clear SMM_EISS (SPI PCI offset DCh[5])
    // Clear LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5])
    MmioAnd8 (mSpiRegBase + R_PCH_SPI_BC, (UINT8)~B_PCH_SPI_BC_EISS);
    MmioAnd8 (mEspiRegBase + R_PCH_LPC_BC, (UINT8)~B_PCH_LPC_BC_EISS);
  }
}  
// APTIOV_SERVER_OVERRIDE_RC_END : EISS bit can't be set before NVRAMSMI

  BiosControl = MmioRead8 (mEspiRegBase + R_PCH_LPC_BC);

  DEBUG ((DEBUG_INFO, "Installing BIOS Write Protect SMI handler\n"));

  if (BiosControl & B_PCH_LPC_BC_BBS) {
    if (BiosControl & B_PCH_ESPI_PCBC_ESPI_EN) {
      //
      // BIOS is behind eSPI
      //

      ///
      /// Get the PCH ESPI SMM dispatch protocol
      ///
      mEspiSmmDispatchProtocol = NULL;
      Status = gSmst->SmmLocateProtocol (&gPchEspiSmiDispatchProtocolGuid, NULL, (VOID **) &mEspiSmmDispatchProtocol);
      ASSERT_EFI_ERROR (Status);

      ///
      /// Register an ESpiBiosWp callback function to handle BIOSWR SMI
      ///
      Handle = NULL;
      Status = mEspiSmmDispatchProtocol->BiosWrProtectRegister (
                                           mEspiSmmDispatchProtocol,
                                           PchLpcBiosWpCallback,
                                           &Handle
                                           );
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG ((DEBUG_INFO, "BIOS behind LPC is not supported.\n"));
      ASSERT (FALSE);
    }
  } else {
    //
    // BIOS is behind SPI
    //

    ///
    /// Get the PCH TCO SMM dispatch protocol
    ///
    mPchTcoSmiDispatchProtocol = NULL;
    Status = gSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);
    ASSERT_EFI_ERROR (Status);
    ///
    /// Register an SpiBiosWp callback function to handle TCO BIOSWR SMI
    ///
    Handle = NULL;
    Status = mPchTcoSmiDispatchProtocol->SpiBiosWpRegister (
                                           mPchTcoSmiDispatchProtocol,
                                           PchSpiBiosWpCallback,
                                           &Handle
                                           );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

