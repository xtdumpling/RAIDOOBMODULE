//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA INIT DXE Driver.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UbaInitDxe.h"


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
UbaInitDxeDriverEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                              Status = EFI_SUCCESS;
  UBA_CONFIG_DATABASE_PROTOCOL           *UbaConfigProtocol = NULL;
  UINT32                                  PlatformType = 0;
  EFI_HANDLE                              Hanlde = NULL;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->GetSku(
                         UbaConfigProtocol,
                         &PlatformType, 
                         NULL,
                         NULL
                         );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "Uba init Dxe driver:PlatformType=%d\n", PlatformType));

  //according to the platform type to install different dummy maker.
  //later, the PEIM will be loaded by the dependency.
  switch(PlatformType)
  {
    case TypeNeonCityEPRP:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeNeonCityEPRPProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeNeonCityEPECB:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeNeonCityEPECBProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeOpalCitySTHI:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeOpalCitySTHIProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypePurleyLBGEPDVP:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypePurleyLBGEPDVPProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeWolfPass:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeWolfPassProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeBuchananPass:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeBuchananPassProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeCrescentCity:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeCrescentCityProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeHedtEV:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeHedtEVProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeHedtCRB:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeHedtCRBProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEXRP:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEXRPProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEXECB1:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEXECB1ProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEXECB2:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEXECB2ProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEXECB3:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEXECB3ProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEXECB4:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEXECB4ProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEX8S1N:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEX8S1NProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break; 
    case TypeLightningRidgeEX8S2N:
      Status = gBS->InstallProtocolInterface (
            &Hanlde, 
            &gEfiPlatformTypeLightningRidgeEX8S2NProtocolGuid, 
            EFI_NATIVE_INTERFACE, 
            NULL
            );
      ASSERT_EFI_ERROR (Status);
      break;
  case TypeKyanite:
    Status = gBS->InstallProtocolInterface(
      &Hanlde,
      &gEfiPlatformTypeKyaniteProtocolGuid,
      EFI_NATIVE_INTERFACE,
      NULL
      );
    ASSERT_EFI_ERROR(Status);
    break;

  case TypeNeonCityFPGA:
    Status = gBS->InstallProtocolInterface(
      &Hanlde,
      &gEfiPlatformTypeNeonCityFPGAProtocolGuid,
      EFI_NATIVE_INTERFACE,
      NULL
      );
    ASSERT_EFI_ERROR(Status);
    break;

  case TypeOpalCityFPGA:
    Status = gBS->InstallProtocolInterface(
      &Hanlde,
      &gEfiPlatformTypeOpalCityFPGAProtocolGuid,
      EFI_NATIVE_INTERFACE,
      NULL
      );
    ASSERT_EFI_ERROR(Status);
    break;

    default:
      // CAN'T GO TO HERE.
      ASSERT_EFI_ERROR(FALSE);
  }
  return Status;
}
