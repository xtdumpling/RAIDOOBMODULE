//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
   Definition of the Whea Boot Protocol.

  Copyright (c) 2005 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_BOOT_PROTOCOL_H_
#define _WHEA_BOOT_PROTOCOL_H_

//
// WHEA Platform Protocol GUID
//

#define EFI_WHEA_BOOT_PROTOCOL_GUID \
  { \
    0x4a0266fe, 0xfe57, 0x4738, 0x80, 0xab, 0x14, 0x6e, 0x46, 0xf0, 0x3a, 0x65 \
  }

#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
EFI_FORWARD_DECLARATION (EFI_WHEA_BOOT_PROTOCOL);


//
// This installs the serialization actions for accessing Error Record persitant Storage.
//

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_START) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_END) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_MEMORY) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This,
    IN UINT16                   ErrorType,
    IN MEMORY_DEV_INFO          *MemInfo
  );

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_PCIE_ROOT_DEV_BRIDGE) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This,
    IN UINT16                   ErrorType,
    IN PCIE_PCI_DEV_INFO        *ErrPcieDev
  );

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_PCI_DEV) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This,
    IN UINT16                   ErrorType,
    IN PCIE_PCI_DEV_INFO        *ErrPcieDev
  );

typedef
EFI_STATUS
(EFIAPI *WHEA_BOOT_ELOG_PROCESSOR) (
    IN EFI_WHEA_BOOT_PROTOCOL   *This,
    IN UINT16                   ErrorType,
    IN CPU_DEV_INFO             *CpuDevInfo
  );

//
// WHEA Boot Protocol
//
typedef struct _EFI_WHEA_BOOT_PROTOCOL {
  UINT64                                BertBaseAddress;
  UINT32                                BertSize;
  WHEA_BOOT_ELOG_START                  WheaBootStartElog;
  WHEA_BOOT_ELOG_END                    WheaBootEndElog;
  WHEA_BOOT_ELOG_MEMORY                 WheaBootElogMemory;
  WHEA_BOOT_ELOG_PCIE_ROOT_DEV_BRIDGE   WheaBootElogPcieRootDevBridge;
  WHEA_BOOT_ELOG_PCI_DEV                WheaBootElogPciDev;
  WHEA_BOOT_ELOG_PROCESSOR              WheaBootElogProcessor;
} EFI_WHEA_BOOT_PROTOCOL;

extern EFI_GUID  gEfiWheaBootProtocolGuid;   
#endif  // _WHEA_BOOT_PROTOCOL_H_

