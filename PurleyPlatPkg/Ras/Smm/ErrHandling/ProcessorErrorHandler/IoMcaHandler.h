/** @file
  This file will contain all definitions related to IOMCA Error Handler.

  Copyright (c) 2009-2017 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _IOMCA_ERROR_HANDLER_H_
#define _IOMCA_ERROR_HANDLER_H_

#include <Base.h>
#include <emca.h>

//// PCIe r3.0 section 7.9
#define  CAP_OFFSET_BASE   0x40
#define  CAP_OFFSET_LIMIT  0x100

// PCI r2.2 figure 6-10
#define  CAP_MIN_SIZE      4
#define  MAX_CAPABILITIES (CAP_OFFSET_LIMIT - CAP_OFFSET_BASE) / CAP_MIN_SIZE
#define  CAPABILITIES_MSK  0xFF
#define  DWORD_MSK          3 // BIT1 | BIT0

//#define PCIE_CORRECTABLE_ERROR_COUNT_LIMIT          10

#define PCIE_EXT_CAP_HEADER_AERC                    0x0001
#define PCIE_ENHANCED_CAPABILITY_PTR                0x0100
#define PCI_EXP_CAP_SLCAP                           0x14                 // Slot capabilities
#define PCIE_EXT_CAP_HEADER_RPPIOERRC               0x0007
#define PCIE_EXT_CAP_HEADER_RPPIOERRC_REVISION      0x0000

#define PCIEAER_CORRERR_MSK        0x000031c1
#define PCIEAER_UNCORRERR_MSK      0x003ff030

#define PCIE_DEVSTS_OFFSET         0x9a
#define PCIE_AER_UNCERRSTS_OFFSET  0x04
#define PCIE_AER_UNCERRMSK_OFFSET  0x08
#define PCIE_AER_UNCERRSEV_OFFSET  0x0c
#define PCIE_AER_CORERRSTS_OFFSET  0x10
#define PCIE_AER_CORERRMSK_OFFSET  0x14
#define PCIE_AER_ROOTERRSTS_OFFSET 0x30

//PCIe r3.0 section 7.9
#define  EX_CAP_OFFSET_BASE  0x100
#define  EX_CAP_OFFSET_LIMIT 0x1000
#define  EX_CAP_MIN_SIZE     4
#define  MAX_EX_CAPABILITIES (EX_CAP_OFFSET_LIMIT - EX_CAP_OFFSET_BASE) / EX_CAP_MIN_SIZE
#define  EXCAPABILITIES_MSK  0xFFF

#define  PCIE_EXT_CAP_HEADER_VSEC 0x00B  // Vendor-Specific Extended Capability ID

#define  PCIE_EXT_VENDOR_SPECIFIC_HEADER_OFFSET 0x4

UINT8
GetCapabilitiesOffset (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT8   Cap
  );

UINT16
GetExtendedCapabilitiesOffset (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT16  ExtCapabilityId,
  IN      UINT16  VendorSpecificId
  );

BOOLEAN
IsHotRemoval (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  );

EFI_STATUS
HandleIoMca (
  IN EMCA_MC_BANK_SIGNATURE * McBankSig
 );

#endif /* _IOMCA_ERROR_HANDLER_H_ */
