//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add USB 20AfeParams by token.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Sep/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Add USB OC pin override by token.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/18/2016
//
//**********************************************************************
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

#if SMCPKG_SUPPORT
PCH_USB_OVERCURRENT_PIN TypeNeonCityFPGAUsb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = USB2_OC_PIN;
PCH_USB_OVERCURRENT_PIN TypeNeonCityFPGAUsb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = USB3_OC_PIN;
PCH_USB20_AFE         TypeNeonCityFPGAUsb20AfeParams[PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS] = USB2_AFE;
#else
PCH_USB_OVERCURRENT_PIN TypeNeonCityFPGAUsb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = {
                          PchUsbOverCurrentPin0,
                          PchUsbOverCurrentPin1,
                          PchUsbOverCurrentPin1,
                          PchUsbOverCurrentPin2,
                          PchUsbOverCurrentPin3,
                          PchUsbOverCurrentPin3,
                          PchUsbOverCurrentPin7,
                          PchUsbOverCurrentPin7,
                          PchUsbOverCurrentPin6,
                          PchUsbOverCurrentPin4,
                          PchUsbOverCurrentPin6,
                          PchUsbOverCurrentPin4,
                          PchUsbOverCurrentPin5,
                          PchUsbOverCurrentPin4,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip
                       };

PCH_USB_OVERCURRENT_PIN TypeNeonCityFPGAUsb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = {
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

PCH_USB20_AFE         TypeNeonCityFPGAUsb20AfeParams[PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS] = {
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
#endif
EFI_STATUS
TypeNeonCityFPGAPlatformUsbOcUpdateCallback (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE            **Usb20AfeParams
)
{
   *Usb20OverCurrentMappings   = &TypeNeonCityFPGAUsb20OverCurrentMappings[0];
   *Usb30OverCurrentMappings   = &TypeNeonCityFPGAUsb30OverCurrentMappings[0];
   *Usb20AfeParams             = TypeNeonCityFPGAUsb20AfeParams;
   return EFI_SUCCESS;
}

PLATFORM_USBOC_UPDATE_TABLE  TypeNeonCityFPGAUsbOcUpdate =
{
   PLATFORM_USBOC_UPDATE_SIGNATURE,
   PLATFORM_USBOC_UPDATE_VERSION,
   TypeNeonCityFPGAPlatformUsbOcUpdateCallback
};

EFI_STATUS
TypeNeonCityFPGAPlatformUpdateUsbOcMappings (
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
                                 &TypeNeonCityFPGAUsbOcUpdate,
                                 sizeof(TypeNeonCityFPGAUsbOcUpdate)
                                 );

  return Status;
}


