//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA INIT PEIM.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UbaInitPei.h"


/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
UbaInitPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  EFI_STATUS                            Status;
  
  EFI_HOB_GUID_TYPE                     *GuidHob;
  EFI_PLATFORM_INFO                     *PlatformInfo;
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi;

  Status = PeiServicesLocatePpi (
            &gUbaConfigDatabasePpiGuid, 
            0,
            NULL,
            &UbaConfigPpi
            );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob); 

  DEBUG ((EFI_D_ERROR, "UbaInitPeim: PlatformType=0x%X\n", PlatformInfo->BoardId));

  Status = UbaConfigPpi->InitSku (
                         UbaConfigPpi,
                         PlatformInfo->BoardId, 
                         NULL,
                         NULL
                         );
  ASSERT_EFI_ERROR (Status);

  //according to the platform type to install different dummy maker.
  //later, the PEIM will be loaded by the dependency.
  switch(PlatformInfo->BoardId)
  {
    case TypeNeonCityEPRP:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeNeonCityEPRPPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeNeonCityEPECB:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeNeonCityEPECBPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeOpalCitySTHI:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeOpalCitySTHIPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypePurleyLBGEPDVP:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypePurleyLBGEPDVPPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeWolfPass:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeWolfPassPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeBuchananPass:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeBuchananPassPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeCrescentCity:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeCrescentCityPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeHedtEV:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeHedtEVPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeHedtCRB:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeHedtCRBPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEXRP:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEXRPPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEXECB1:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEXECB1Ppi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEXECB2:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEXECB2Ppi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEXECB3:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEXECB3Ppi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEXECB4:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEXECB4Ppi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEX8S1N:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEX8S1NPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeLightningRidgeEX8S2N:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeLightningRidgeEX8S2NPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeKyanite:
      Status = (**PeiServices).InstallPpi(PeiServices, &mPlatformTypeKyanitePpi);
      ASSERT_EFI_ERROR(Status);
    break;
    case TypeNeonCityFPGA:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeNeonCityFPGAPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    case TypeOpalCityFPGA:
      Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformTypeOpalCityFPGAPpi);
      ASSERT_EFI_ERROR (Status);
      break;
    default:
      // CAN'T GO TO HERE.
      ASSERT(FALSE);
  }

  return Status;
}