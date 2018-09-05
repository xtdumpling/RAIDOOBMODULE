/** @file
  SEC PCH library in C.

@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved
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
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/PchSerialIoUartLib.h>
#include <PchAccess.h>

#ifdef FSP_FLAG
#include <FspUpdVpdInternal.h>
#include <FspInfoHeader.h>

/**
  This interface gets FspInfoHeader pointer

  @return   FSP binary base address.

**/
UINT32
EFIAPI
AsmGetFspInfoHeader (
  VOID
  );
#endif

/**
  This is helper function to initialize SerialIoUart in early init.
**/

/**
  This function do the PCH cycle decoding initialization.
**/
VOID
EFIAPI
EarlyCycleDecoding (
  VOID
  )
{
  UINT32   P2sbBase;
  UINT32   SmbusBase;

  //
  // Enable PCR base address in PCH
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
  //
  // Enable P2SB MSE
  //
  MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  //
  // Program ACPI Base.
  //
  PchAcpiBaseSet (PcdGet16 (PcdPchAcpiIoPortBaseAddress));

  //
  // Program PWRM BASE
  //
  PchPwrmBaseSet (PCH_PWRM_BASE_ADDRESS);

  //
  // Program and Enable TCO Base
  //
  PchTcoBaseSet (PcdGet16 (PcdTcoBaseAddress));

  SmbusBase = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_SMBUS,
                PCI_FUNCTION_NUMBER_PCH_SMBUS
                );
  MmioWrite32 (SmbusBase + R_PCH_SMBUS_64, 0x0A0A0000);
  //
  // Initialize SMBUS IO BAR
  //
  MmioWrite16 (SmbusBase + R_PCH_SMBUS_BASE, PcdGet16 (PcdSmbusBaseAddress));
  //
  // Enable the Smbus I/O Enable
  //
  MmioOr8 (SmbusBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);
  //
  // Enable the SMBUS Controller
  //
  MmioOr8 (SmbusBase + R_PCH_SMBUS_HOSTC, B_PCH_SMBUS_HOSTC_HST_EN);

  //
  // Enable the upper 128-byte bank of RTC RAM.
  // PCR [RTC] + 0x3400 [2] = 1
  //
  PchPcrAndThenOr32 (PID_RTC, R_PCH_PCR_RTC_CONF, (UINT32)~0, B_PCH_PCR_RTC_CONF_UCMOS_EN);
}