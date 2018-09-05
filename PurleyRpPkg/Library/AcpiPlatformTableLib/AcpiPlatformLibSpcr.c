/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiPlatformLibSpcr.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


extern EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *mSpcrTable;  


/**

  Test whether DevicePath is a valid Terminal

  @param DevicePath  -   DevicePath to be checked
  @param TerminalType-   If is terminal, give its type
  @param Com         -   If is Com Port, give its type

  @retval TRUE        -   If DevicePath point to a Terminal
  @retval FALSE

**/
BOOLEAN
IsTerminalDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  OUT UINT8                    *TerminalType,
  OUT UINTN                    *Com
  )
{
  UINT8                 *Ptr;
  VENDOR_DEVICE_PATH    *Vendor;
  ACPI_HID_DEVICE_PATH  *Acpi;

  Ptr = (UINT8 *) DevicePath;
  Ptr    = Ptr + GetDevicePathSize (DevicePath) -
           sizeof (VENDOR_DEVICE_PATH) - sizeof (EFI_DEVICE_PATH_PROTOCOL);
  Vendor = (VENDOR_DEVICE_PATH *) Ptr;

  if (Vendor->Header.Type == MESSAGING_DEVICE_PATH &&
      Vendor->Header.SubType == MSG_VENDOR_DP &&
      DevicePathNodeLength (&Vendor->Header) == sizeof(VENDOR_DEVICE_PATH)) {
    //
    // There are four kinds of Terminal types
    // check to see whether this devicepath
    // is one of that type
    //
    if (CompareGuid (&Vendor->Guid, &gEfiVT100Guid)) {
      *TerminalType = 0;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVT100PlusGuid)) {
      *TerminalType = 1;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVTUTF8Guid)) {
      *TerminalType = 2;
    } else if (CompareGuid (&Vendor->Guid, &gEfiPcAnsiGuid)) {
      *TerminalType = 3;
    } else {
      return FALSE;
    }

    Ptr   = Ptr - sizeof (UART_DEVICE_PATH) - sizeof (ACPI_HID_DEVICE_PATH);
    Acpi  = (ACPI_HID_DEVICE_PATH *) Ptr;
    if (Acpi->Header.Type == ACPI_DEVICE_PATH &&
        Acpi->Header.SubType == ACPI_DP &&
        DevicePathNodeLength (&Acpi->Header) == sizeof (ACPI_HID_DEVICE_PATH) &&
        Acpi->HID == EISA_PNP_ID (0x0501)) {
      *Com = Acpi->UID;
      return TRUE;
    }
  }
  return FALSE;
}

/**

  This function updates SPCR table.

  Note that user may change CR settings via setup or other methods.
  The SPCR table must match.

  @param None

  @retval TRUE - SPCR Table updated and is valid.

**/
BOOLEAN
UpdateSpcrTable (
  VOID
  )
{
  EFI_STATUS Status;
  UINT8      BaudRate;
  UINT8      TerminalType;
  UINT32     Control;
  UINTN      Com;
  UINTN      Size;
  EFI_SERIAL_IO_PROTOCOL   *SerialIo;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *DevicePathInstance;
  EFI_HANDLE               Handle;
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove LpcPlatform component.
  //EFI_LPC_POLICY_PROTOCOL  *LpcPolicy;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove LpcPlatform component.

  if (mSpcrTable == NULL) {
    return FALSE;
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : Remove LpcPlatform component. COM port data is hard-coded as done in LpcPlatform.
  #if 0
  Status = gBS->LocateProtocol (&gEfiLpcPolicyProtocolGuid, NULL, &LpcPolicy);
  ASSERT_EFI_ERROR (Status);

  if (LpcPolicy->PolicyCom.Enable[0] != TRUE) {
    return FALSE;
  }
  mSpcrTable->BaseAddress.Address = LpcPolicy->PolicyCom.Address[0];
  mSpcrTable->Irq = (UINT8) LpcPolicy->PolicyCom.Irq[0];
  mSpcrTable->GlobalSystemInterrupt = mSpcrTable->Irq;
  #endif
  mSpcrTable->BaseAddress.Address = 0x3F8;
  mSpcrTable->Irq = 4;
  mSpcrTable->GlobalSystemInterrupt = mSpcrTable->Irq;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove LpcPlatform component. COM port data is hard-coded as done in LpcPlatform.

  //
  // Locate the COM1 terminal
  //

  Status = GetVariable2 (
              L"ConIn",
              &gEfiGlobalVariableGuid,
              &DevicePath,
              NULL);
  if (DevicePath == NULL) {
    return FALSE;
  }

  TerminalType = 0;
  TempDevicePath = DevicePath;
  do {
    DevicePathInstance = GetNextDevicePathInstance (&TempDevicePath, &Size);
    if (DevicePathInstance == NULL) {
      break;
    }
    if (IsTerminalDevicePath (DevicePathInstance, &TerminalType, &Com) && Com == 0) {
      break;
    }
    gBS->FreePool (DevicePathInstance);
    DevicePathInstance = NULL;
  } while (TempDevicePath != NULL);

  if (DevicePathInstance == NULL) {
    gBS->FreePool (DevicePath);
    return FALSE;
  }

  TempDevicePath = DevicePathInstance;
  Status = gBS->LocateDevicePath (
                  &gEfiSerialIoProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  gBS->FreePool (DevicePathInstance);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSerialIoProtocolGuid,
                  (VOID **) &SerialIo
                  );
  ASSERT_EFI_ERROR (Status);

  switch (SerialIo->Mode->BaudRate) {
  case 9600:
    BaudRate = 3;
    break;
  case 19200:
    BaudRate = 4;
    break;
  case 57600:
    BaudRate = 6;
    break;
  case 115200:
    BaudRate = 7;
    break;
  default:
    DEBUG(( EFI_D_ERROR, "\nSPCR: Unsupported baud rate!\n" ));
    goto UpdateSpcrExit;
    break;
  }
  mSpcrTable->BaudRate = BaudRate;
  if (SerialIo->Mode->Parity != NoParity) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support no parity!\n" ));
    goto UpdateSpcrExit;
  }
  if (SerialIo->Mode->DataBits != 8) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support 8 data bits!\n" ));
    goto UpdateSpcrExit;
  }
  if (SerialIo->Mode->StopBits != OneStopBit) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support 1 stop bit!\n" ));
    goto UpdateSpcrExit;
  }
  Status = SerialIo->GetControl (
                        SerialIo,
                        &Control
                        );
  ASSERT_EFI_ERROR (Status);

  if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    mSpcrTable->FlowControl |= 2;
  }
  mSpcrTable->TerminalType = TerminalType;
  return TRUE;

UpdateSpcrExit:
  return FALSE;
}


EFI_STATUS
PatchSpcrAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  if(!UpdateSpcrTable()) {
    Status = EFI_NOT_READY; 
  }
  return Status;
}