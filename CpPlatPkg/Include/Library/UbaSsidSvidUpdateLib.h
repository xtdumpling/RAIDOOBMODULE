//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA ssid/svid Update Library Header File.

  Copyright (c) 2012 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_SSID_SVID_UPDATE_LIB_
#define _UBA_SSID_SVID_UPDATE_LIB_

#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>

#define PLATFORM_SSIDSVID_UPDATE_SIGNATURE  SIGNATURE_32 ('S', 'S', 'I', 'D')
#define PLATFORM_SSIDSVID_UPDATE_VERSION    01

// {EF363C9F-CAA1-4303-833F-E2BEB7632E50}
STATIC EFI_GUID  gPlatformSsidSvidConfigDataGuid = 
{ 0xef363c9f, 0xcaa1, 0x4303, { 0x83, 0x3f, 0xe2, 0xbe, 0xb7, 0x63, 0x2e, 0x50 } };

typedef
EFI_STATUS
(*PLATFORM_SSIDSVID_UPDATE_CALLBACK) (
  IN      EFI_PCI_IO_PROTOCOL               *PciIo,
  IN      PCI_DEVICE_INDEPENDENT_REGION     *PciHeader,
  IN  OUT UINT32                            *SsidSvid
);

typedef struct {
  UINT32                              Signature;
  UINT32                              Version;
  
  UINT32                              PlatformType;
  
  PLATFORM_SSIDSVID_UPDATE_CALLBACK   CallUpdate;       // Optional
} PLATFORM_SSIDSVID_UPDATE_DATA;

/**
  Register a SSIDSVID data update callback.

  @param RegData                  Register data.
  
  @retval EFI_SUCCESS             Register successfully.
  @retval EFI_NOT_FOUND           Depend protocol not found.
  
**/
EFI_STATUS
PlatformRegisterSsidSvidUpdate (
  IN      PLATFORM_SSIDSVID_UPDATE_DATA     *RegData  
);

/**
  Get registered SSIDSVID data update callback.

  @param RegData                  Register data.
  
  @retval EFI_SUCCESS             Get registered data successfully.
  @retval EFI_NOT_FOUND           Data not found.
  @retval EFI_INVALID_PARAMETER   Parameter not correct.
  
**/
EFI_STATUS
PlatformGetSsidSvid (
  IN  OUT PLATFORM_SSIDSVID_UPDATE_DATA     **RegData  
);

/**
  Get the SSIDSVID value for specific device by given SID&VID.

  @param SidVid                   The system id and vendor id for device.
  @param SsidSvid                 The subsystem id and subvendor id will return
  
  @retval EFI_SUCCESS             Get data successfully.
  @retval EFI_NOT_FOUND           Data not found.
  @retval EFI_INVALID_PARAMETER   Parameter not correct.
  @retval EFI_DEVICE_ERROR        Depend protocol not found
  
**/
EFI_STATUS
PlatformSearchSsidForDevice (
  IN      UINT32                            SidVid,
  OUT     UINT32                            *SsidSvid
);

#endif //_UBA_SSID_SVID_UPDATE_LIB_