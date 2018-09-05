/** @file
  AMT BDS Support include file

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

#ifndef _AMT_WRAPPER_DXE_H_
#define _AMT_WRAPPER_DXE_H_

#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <Protocol/AlertStandardFormat.h>
#include <Protocol/LegacyBios.h>
#include <Library/DxeAmtLib.h>
#include <MeChipset.h>
#include <MeBiosExtensionSetup.h>
#include <Protocol/AmtPolicy.h>
#include <Protocol/ActiveManagement.h>
#include <Protocol/HeciProtocol.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>

#define R_PCI_SVID                      0x2C

#define     DISPLAY_SCREEN              0x01                // for LegacySredr CSM16
#define     SUPPORT8025                 0x02                // for LegacySredr CSM16

// define for SREDIR_DISPLAY_MODE
#define     DISPLAY_BY_SETUP            0x00
#define     DISPLAY_BY_80x24            0x01
#define     DISPLAY_BY_80x25            0x02

#define STOPB   0x4                 //      Bit2: Number of Stop Bits
#define PAREN   0x8                 //      Bit3: Parity Enable
#define EVENPAR 0x10                //      Bit4: Even Parity Select
#define STICPAR 0x20                //      Bit5: Sticky Parity
#define SERIALDB 0x3                //      Bit0-1: Number of Serial

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           SerialOverLAN;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH;

#define gPciRootBridge \
  { \
    ACPI_DEVICE_PATH, \
    ACPI_DP, \
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), \
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), \
    EISA_PNP_ID (0x0A03), 0 \
  }

#define gEndEntire \
  { \
    END_DEVICE_PATH_TYPE,\
    END_ENTIRE_DEVICE_PATH_SUBTYPE,\
    END_DEVICE_PATH_LENGTH,\
    0\
  }

EFI_STATUS
AmtWrapperInit(
    IN EFI_HANDLE ImageHandle,
    IN OUT EFI_SYSTEM_TABLE *SystemTable
);

BOOLEAN
AmtWrapperGet(
    UINTN Index,
    VOID *pVar
);

BOOLEAN
AmtWrapperSet(
    UINTN Index,
    VOID *pVar
);

BOOLEAN
ActiveManagementPauseBoot (
    IN VOID
);

BOOLEAN
ActiveManagementEnterSetup (
    IN VOID
);

BOOLEAN
ActiveManagementEnableSol (
    IN VOID
);

BOOLEAN
ActiveManagementEnableStorageRedir (
    IN VOID
);

BOOLEAN
ActiveManagementEnableSecureErase(
    IN VOID
);

EFI_STATUS
BdsAsfInitialization (
    IN VOID
);

EFI_STATUS
BdsBootViaAsf (
    IN VOID
);

BOOLEAN
AmtWrapperBootOptionExist (
    IN VOID
);

VOID
InitAmtWrapperLib(
    IN VOID
);

BOOLEAN
AmtWrapperEnableKvm(
    IN VOID
);


BOOLEAN
AmtWrapperEnableSol(
    IN VOID
);

BOOLEAN
AmtWrapperEnableStorageRedir(
    IN VOID
);

BOOLEAN
AmtWrapperPauseBoot(
    IN VOID
);

BOOLEAN
AmtWrapperEnterSetup(
    IN VOID
);

EFI_STATUS
AmtWrapperBdsBootViaAsf(
    IN VOID
);

EFI_STATUS
AmtWrapperReadyToBoot(
    EFI_EVENT           Event,
    VOID                *ParentImageHandle
);

BOOLEAN
AmtWrapperEnableSecureErase(
    IN VOID
);

UINTN IsSOL(
    IN EFI_HANDLE Controller
);

EFI_STATUS
GetSetupPrompt (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
);

EFI_STATUS
DetermineSetupHotKey(
  IN EFI_KEY_DATA       *KeyData
);

EFI_STATUS
GetSupportedHotkeys (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  );

// APTIOV_SERVER_OVERRIDE_RC_START
VOID
SetSOLCOMEnable(
    IN BOOLEAN         Enabled
);
// APTIOV_SERVER_OVERRIDE_RC_END

#endif
