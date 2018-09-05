/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbOcUpdateDxe.c

Abstract:

--*/
#include "UsbOcUpdateDxe.h"

#include <Library/UbaUsbOcUpdateLib.h>
#include <PchLimits.h>
#include <PchPolicyCommon.h>

PCH_USB_OVERCURRENT_PIN TypeLightningRidgeEXECB4Usb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = {
                          PchUsbOverCurrentPin0,      //Port00: Zepher        ,OC0#
                          PchUsbOverCurrentPin1,      //Port01: Read Connector,OC1#
                          PchUsbOverCurrentPinSkip,   //Port02: User bay      ,OC0#
                          PchUsbOverCurrentPinSkip,   //Port03: iBMC USB 1.1  ,no OCn#
                          PchUsbOverCurrentPinSkip,   //Port04: NONE          ,no OCn#
                          PchUsbOverCurrentPin2,      //Port05: Read Connector,OC2#
                          PchUsbOverCurrentPin1,      //Port06: Read Connector,OC1#
                          PchUsbOverCurrentPin2,      //Port07: Read Connector,OC2#
                          PchUsbOverCurrentPinSkip,   //Port08: NONE          ,no OCn#
                          PchUsbOverCurrentPinSkip,   //Port09: NONE          ,no OCn#
                          PchUsbOverCurrentPinSkip,   //Port10: iBMC USB 2.0  ,no OCn#
                          PchUsbOverCurrentPin4,      //Port11: Front Panel   ,OC4#
                          PchUsbOverCurrentPinSkip,   //Port12: NONE          ,no OCn#
                          PchUsbOverCurrentPin4      //Port13: Front Panel   ,OC4#
                       };

PCH_USB_OVERCURRENT_PIN TypeLightningRidgeEXECB4Usb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = {
                          PchUsbOverCurrentPin0,
                          PchUsbOverCurrentPin1,
                          PchUsbOverCurrentPin1,
                          PchUsbOverCurrentPin2,
                          PchUsbOverCurrentPin3,
                          PchUsbOverCurrentPin3,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip
                       };

PCH_USB20_AFE         TypeLightningRidgeEXECB4Usb20AfeParams[PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS] = {
                        {7, 0, 2, 1},   // PP0
                        {7, 0, 2, 1},   // PP1
                        {7, 0, 2, 1},   // PP2
                        {7, 0, 2, 1},   // PP3
                        {7, 0, 2, 1},   // PP4
                        {7, 0, 2, 1},   // PP5
                        {7, 0, 2, 1},   // PP6
                        {7, 0, 2, 1},   // PP7
                        {7, 0, 2, 1},   // PP8
                        {7, 0, 2, 1},   // PP9
                        {7, 0, 2, 1},   // PP10
                        {7, 0, 2, 1},   // PP11
                        {7, 0, 2, 1},   // PP12
                        {7, 0, 2, 1},   // PP13
                      };

EFI_STATUS
TypeLightningRidgeEXECB4PlatformUsbOcUpdateCallback (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE            **Usb20AfeParams
)
{
   *Usb20OverCurrentMappings   = &TypeLightningRidgeEXECB4Usb20OverCurrentMappings[0];
   *Usb30OverCurrentMappings   = &TypeLightningRidgeEXECB4Usb30OverCurrentMappings[0];
   *Usb20AfeParams             = TypeLightningRidgeEXECB4Usb20AfeParams;
   return EFI_SUCCESS;
}               

PLATFORM_USBOC_UPDATE_TABLE  TypeLightningRidgeEXECB4UsbOcUpdate =
{
   PLATFORM_USBOC_UPDATE_SIGNATURE,
   PLATFORM_USBOC_UPDATE_VERSION,
   TypeLightningRidgeEXECB4PlatformUsbOcUpdateCallback
};

/**
  The Driver Entry Point.

  The function is the driver Entry point.

  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
UsbOcUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                          Status;
  UBA_CONFIG_DATABASE_PROTOCOL        *UbaConfigProtocol = NULL;

  DEBUG((EFI_D_ERROR, "UBA:UsbOcUpdate-TypeLightningRidgeEXECB4\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gDxePlatformUbaOcConfigDataGuid,
                                     &TypeLightningRidgeEXECB4UsbOcUpdate,
                                     sizeof(TypeLightningRidgeEXECB4UsbOcUpdate)
                                     );

  return Status;
}
