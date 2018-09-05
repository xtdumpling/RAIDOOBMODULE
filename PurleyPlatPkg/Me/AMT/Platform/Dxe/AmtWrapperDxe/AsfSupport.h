/** @file
  ASF BDS Support include file

@copyright
 Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _ASF_SUPPORT_H_
#define _ASF_SUPPORT_H_

#include <Uefi/UefiBaseType.h>
#include <Guid/GlobalVariable.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/LoadFile.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/AcpiS3Save.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimpleNetwork.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>

#include <Amt.h>
#include <Library/DxeAmtLib.h>
#include <MeChipset.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <IndustryStandard/AlertStandardFormatTable.h>
#include <Protocol/AlertStandardFormat.h>

#define USBR_STOR_PORT_NUM            0xB
#define USBR_BOOT_DEVICE_SHIFT        8
#define USBR_BOOT_DEVICE_MASK         0x1

#define SECURE_BOOT_ENABLED               1
#define SECURE_BOOT_DISABLED              0

#define RESTORE_SECURE_BOOT_NONE          0
#define RESTORE_SECURE_BOOT_ENABLED       1

#define RESTORE_SECURE_BOOT_GUID \
  { \
    0x118b3c6f, 0x98d6, 0x4d05, 0x96, 0xb2, 0x90, 0xe4, 0xcb, 0xb7, 0x40, 0x34 \
  }

#define EFI_SECURE_BOOT_ENABLE_DISABLE \
  { 0xf0a30bc7, 0xaf08, 0x4556, { 0x99, 0xc4, 0x0, 0x10, 0x9, 0xc9, 0x3a, 0x44 } }

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Xhci;
  USB_DEVICE_PATH           Usbr;
  EFI_DEVICE_PATH_PROTOCOL  End;
} STORAGE_REDIRECTION_DEVICE_PATH;

extern 
VOID
ConnectAllDriversToAllControllers (
  VOID
  );

extern
VOID
PrintBbsTable (
  IN BBS_TABLE                      *LocalBbsTable,
  IN UINT16                         BbsCount
  );

/**
  This routine makes necessary Secure Boot & CSM state changes for Storage Redirection boot

  @param[in] None.

  @retval EFI_SUCCESS      Changes applied succesfully
**/
EFI_STATUS
ManageSecureBootState (
  IN VOID
  )
;

/**
  Retrieve the ASF boot options previously recorded by the ASF driver.

  @param[in] None.

  @retval EFI_SUCCESS      Initialized Boot Options global variable and AMT protocol
**/
EFI_STATUS
BdsAsfInitialization (
  IN  VOID
  )
;

/**
  Process ASF boot options and if available, attempt the boot

  @param[in] None.

  @retval EFI_SUCCESS      The command completed successfully
**/
EFI_STATUS
BdsBootViaAsf (
  IN  VOID
  )
;

#endif
