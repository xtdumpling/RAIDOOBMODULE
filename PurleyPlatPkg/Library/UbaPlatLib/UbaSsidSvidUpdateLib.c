//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaSsidSvidUpdateLib implementation.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Library/UbaSsidSvidUpdateLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/UbaCfgDb.h>

/**
  Register a SSIDSVID data update callback.

  @param RegData                  Register data.
  
  @retval EFI_SUCCESS             Register successfully.
  @retval EFI_NOT_FOUND           Depend protocol not found.
  
**/
EFI_STATUS
PlatformRegisterSsidSvidUpdate (
  IN      PLATFORM_SSIDSVID_UPDATE_DATA     *RegData  
)
{
  EFI_STATUS                                    Status;
  STATIC  UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol = NULL;

  if (UbaConfigProtocol == NULL) {
    
    Status = gBS->LocateProtocol (
                    &gUbaConfigDatabaseProtocolGuid,
                    NULL,
                    &UbaConfigProtocol
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  RegData->Signature  = PLATFORM_SSIDSVID_UPDATE_SIGNATURE;
  RegData->Version    = PLATFORM_SSIDSVID_UPDATE_VERSION;

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol, 
                                     &gPlatformSsidSvidConfigDataGuid, 
                                     RegData, 
                                     sizeof(PLATFORM_SSIDSVID_UPDATE_DATA)
                                     );

  return Status;
}

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
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_SSIDSVID_UPDATE_DATA           *NewRegData;

  if (RegData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NewRegData = AllocateZeroPool (sizeof (PLATFORM_SSIDSVID_UPDATE_DATA));
  if (NewRegData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );

  if (EFI_ERROR(Status)) {
    gBS->FreePool(NewRegData);
    return Status;
  }

  DataLength  = sizeof (PLATFORM_SSIDSVID_UPDATE_DATA);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformSsidSvidConfigDataGuid,
                                    NewRegData,
                                    &DataLength
                                    );
  if (!EFI_ERROR(Status)) {

    ASSERT (NewRegData->Signature == PLATFORM_SSIDSVID_UPDATE_SIGNATURE);
    ASSERT (NewRegData->Version == PLATFORM_SSIDSVID_UPDATE_VERSION);
  
    *RegData =  NewRegData;
    return EFI_SUCCESS;
  }

  gBS->FreePool(NewRegData);
  return EFI_NOT_FOUND;
}

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
)
{
  EFI_STATUS                                Status;
  EFI_PCI_IO_PROTOCOL                       *PciIo = NULL;
  
  UINTN                                     HandleCount = 0;
  EFI_HANDLE                                *HandleBuffer = NULL;
  UINTN                                     Index;

  PCI_TYPE00                                PciDevice;
  UINT32                                    ThisSidVid = 0;

  if (SsidSvid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol, 
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR(Status)) {
    return EFI_DEVICE_ERROR;
  }
  
  for (Index = 0; Index < HandleCount; Index++) {
  
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, &PciIo);
    if (!EFI_ERROR (Status)) {
      Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof (PciDevice)/sizeof (UINT32),
                          &PciDevice
                          );
      ThisSidVid = (PciDevice.Hdr.DeviceId << 16) | PciDevice.Hdr.VendorId;

      if (ThisSidVid == SidVid) {
        *SsidSvid = (PciDevice.Device.SubsystemID << 16) | PciDevice.Device.SubsystemVendorID;
        return EFI_SUCCESS;
      }
    }
  }
  
  return EFI_NOT_FOUND;
}

