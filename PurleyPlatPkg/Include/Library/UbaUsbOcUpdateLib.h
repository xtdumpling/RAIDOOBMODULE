//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA USB OC Update Library Header File.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_USBOC_UPDATE_LIB_H
#define _UBA_USBOC_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>
#include <PchPolicyCommon.h>

#define PLATFORM_USBOC_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'U', 'S', 'B')
#define PLATFORM_USBOC_UPDATE_VERSION    02

// {88238275-5922-46b6-9e35-656df55df44c}
#define   PEI_PLATFORM_USBOC_CONFIG_DATA_GUID \
{ 0x88238275, 0x5922, 0x46f6, { 0x9e, 0x35, 0x65, 0x6d, 0xf5, 0x5d, 0xf4, 0x4c } }
// {2638009e-3850-4e4b-b05d-042a32dbb9d1}
#define   DXE_PLATFORM_USBOC_CONFIG_DATA_GUID \
{ 0x2638009e, 0x3850, 0x4e4b, { 0xb0, 0x5d, 0x04, 0x2a, 0x32, 0xdb, 0xb9, 0xd1 } }

typedef
EFI_STATUS
(*USBOC_UPDATE_CALLBACK) (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE             **Usb20AfeParams
);

typedef struct _PLATFORM_USBOC_UPDATE_TABLE{
  UINT32                    Signature;
  UINT32                    Version;
  USBOC_UPDATE_CALLBACK     CallUsbOcUpdate;
} PLATFORM_USBOC_UPDATE_TABLE;

EFI_STATUS
PlatformGetUsbOcMappings (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE             **Usb20AfeParams
);
STATIC  EFI_GUID gPeiPlatformUbaOcConfigDataGuid = PEI_PLATFORM_USBOC_CONFIG_DATA_GUID;
STATIC  EFI_GUID gDxePlatformUbaOcConfigDataGuid = DXE_PLATFORM_USBOC_CONFIG_DATA_GUID;

#endif //_UBA_USBOC_UPDATE_LIB_H
