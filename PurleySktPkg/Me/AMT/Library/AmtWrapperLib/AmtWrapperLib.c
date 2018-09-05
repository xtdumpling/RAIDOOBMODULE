/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2011-2014, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AmtWrapperLib.c

--*/
#if AMT_SUPPORT

#include <Include/Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/AmtWrapperLib.h>
#include <Protocol/AmtWrapperProtocol.h>

#define gPciRootBridge \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (0x0A03), 0 \
  }

#define gEndEntire \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 \
  }

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      IsaSerial;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ISA_SERIAL_DEVICE_PATH;

//
// Platform specific serial device path
// Original define in SnbClientX64Pkg\Library\PlatformBootManagerLib\PlatformData.c
//
PLATFORM_ISA_SERIAL_DEVICE_PATH   gNormalSerialDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0,
    0x1f
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),
    0
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8) (sizeof (UART_DEVICE_PATH)),
    (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
    (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};

/**
  Return default enable console device used by out-of-band.

  Now if user not select the device in the menu and the sol type console
  device is enabled, default select this device as out-of-band used device.

**/
EFI_DEVICE_PATH_PROTOCOL *
GetDefaultDeviceForSpcr (
  )
{
  AMT_WRAPPER_PROTOCOL     *pAmtWrapper;
  EFI_STATUS               Status;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;

  //
  // Get device path info from sol type console device.
  //
  Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, &pAmtWrapper);
  if (Status == EFI_SUCCESS) {
    if (pAmtWrapper->ActiveManagementEnableSol()) {
      if (pAmtWrapper->AmtWrapperGet(GET_SOL_DP, (VOID **) &DevicePath)) {
        return DevicePath;
      }
    }
  }

  //
  // If request to enable other console device when sol type console device not enabled
  // get the default com0 as the out-of-band device.
  // Now this logical is not enabled. May enable it per request.
  //
  // return (EFI_DEVICE_PATH_PROTOCOL *)(&gNormalSerialDevicePath);
  return NULL;
}

#endif // AMT_SUPPORT