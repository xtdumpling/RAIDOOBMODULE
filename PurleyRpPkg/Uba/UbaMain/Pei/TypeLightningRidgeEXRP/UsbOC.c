/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbOC.c

Abstract:

--*/


#include "UbaMainPei.h"


#include <Library/PcdLib.h>
#include <Library/UbaUsbOcUpdateLib.h>
#include <PchLimits.h>
#include <PchPolicyCommon.h>

PCH_USB_OVERCURRENT_PIN TypeLightningRidgeEXRPUsb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = {
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

PCH_USB_OVERCURRENT_PIN TypeLightningRidgeEXRPUsb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = {
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

PCH_USB20_AFE         TypeLightningRidgeEXRPUsb20AfeParams[PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS] = {
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
TypeLightningRidgeEXRPPlatformUsbOcUpdateCallback (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE            **Usb20AfeParams
)
{
   *Usb20OverCurrentMappings   = &TypeLightningRidgeEXRPUsb20OverCurrentMappings[0];
   *Usb30OverCurrentMappings   = &TypeLightningRidgeEXRPUsb30OverCurrentMappings[0];
   *Usb20AfeParams             = TypeLightningRidgeEXRPUsb20AfeParams;
   return EFI_SUCCESS;
}

PLATFORM_USBOC_UPDATE_TABLE  TypeLightningRidgeEXRPUsbOcUpdate =
{
   PLATFORM_USBOC_UPDATE_SIGNATURE,
   PLATFORM_USBOC_UPDATE_VERSION,
   TypeLightningRidgeEXRPPlatformUsbOcUpdateCallback
};

EFI_STATUS
TypeLightningRidgeEXRPPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  //#
  //# USB, see PG 104 in GZP SCH
  //#

//  USB2      USB3      Port                            OC
//
//Port00:     PORT5     Back Panel                      ,OC0#
//Port01:     PORT2     Back Panel                      ,OC0#
//Port02:     PORT3     Back Panel                      ,OC1#
//Port03:     PORT0     NOT USED                        ,NA
//Port04:               BMC1.0                          ,NA
//Port05:               INTERNAL_2X5_A                  ,OC2#
//Port06:               INTERNAL_2X5_A                  ,OC2#
//Port07:               NOT USED                        ,NA
//Port08:               EUSB (AKA SSD)                  ,NA
//Port09:               INTERNAL_TYPEA                  ,OC6#
//Port10:     PORT1     Front Panel                     ,OC5#
//Port11:               NOT USED                        ,NA
//Port12:               BMC2.0                          ,NA
//Port13:     PORT4     Front Panel                     ,OC5#

  EFI_STATUS                   Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi,
                                 &gPeiPlatformUbaOcConfigDataGuid,
                                 &TypeLightningRidgeEXRPUsbOcUpdate,
                                 sizeof(TypeLightningRidgeEXRPUsbOcUpdate)
                                 );

  return Status;
}


