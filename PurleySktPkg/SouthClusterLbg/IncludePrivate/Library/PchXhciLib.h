/** @file
  This files contains Pch XHCI library

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

#ifndef _PCH_XHCI_LIB_H_
#define _PCH_XHCI_LIB_H_

#include <PchAccess.h>

/**
  Enumeration of each PCH XHCI LEGACY SMI type.
  PchXhciLegacySmiEn is the major switch for XHCI legacy SMI.
  And each can be ORed with others as parameter. ex..
  PchXhciLegacySmiEnSet (PchXhciLegacySmiEn | PchXhciLegacySmiOsOwnershipEn);
**/
enum PCH_XHCI_LEGACY_SMI {
  PchXhciLegacySmiEn                 = B_PCH_XHCI_USBLEGCTLSTS_USBSMIE,
  PchXhciLegacySmiHostSysErrorEn     = B_PCH_XHCI_USBLEGCTLSTS_SMIHSEE,
  PchXhciLegacySmiOsOwnershipEn      = B_PCH_XHCI_USBLEGCTLSTS_SMIOSOE,
  PchXhciLegacySmiPciCmdEn           = B_PCH_XHCI_USBLEGCTLSTS_SMIPCICE,
  PchXhciLegacySmiBarEn              = B_PCH_XHCI_USBLEGCTLSTS_SMIBARE
};

/**
  Get XHCI USB legacy SMI control setting.

  @retval     return XHCI USB legacy SMI setting.
**/
UINT32
EFIAPI
PchXhciLegacySmiEnGet (
  VOID
  );

/**
  Set XHCI USB legacy SMI control setting.

  @param[in]  UsbLegacySmiEn            Set XHCI USB legacy SMI

**/
VOID
EFIAPI
PchXhciLegacySmiEnSet (
  UINT32                                UsbLegacySmiEn
  );

#endif // _PCH_XHCI_LIB_H_
