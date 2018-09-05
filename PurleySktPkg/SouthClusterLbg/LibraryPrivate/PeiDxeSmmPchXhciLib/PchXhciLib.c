/** @file
  This files contains Pch XHCI library for RCs usage

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

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MmPciBaseLib.h>
#include <PchAccess.h>

/**
  Get XHCI USB legacy SMI control setting.

  @retval     return XHCI USB legacy SMI setting.
**/
UINT32
EFIAPI
PchXhciLegacySmiEnGet (
  VOID
  )
{
  UINTN           XhciPciMmBase;
  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_XHCI,
                    PCI_FUNCTION_NUMBER_PCH_XHCI
                    );
  return MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USBLEGCTLSTS) & 0xFFFF;
}

/**
  Set XHCI USB legacy SMI control setting.

  @param[in]  UsbLegacySmiEn            Set XHCI USB legacy SMI
**/
VOID
EFIAPI
PchXhciLegacySmiEnSet (
  UINT32                                UsbLegacySmiEn
  )
{
  UINTN           XhciPciMmBase;
  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_XHCI,
                    PCI_FUNCTION_NUMBER_PCH_XHCI
                    );
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_USBLEGCTLSTS, UsbLegacySmiEn & 0xFFFF);
}

