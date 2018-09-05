//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA MAIN PEIM.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/


#include "UbaMainPei.h"

/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
UbaMainPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  EFI_STATUS                      Status;
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi;
  UINTN                           DummyHandle;

  DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry!\n"));

  Status = PeiServicesLocatePpi (
            &gUbaConfigDatabasePpiGuid,
            0,
            NULL,
            &UbaConfigPpi
            );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //TypeNeonCityEPECB
  Status = PeiServicesLocatePpi (
          &gEfiPeiPlatformTypeNeonCityEPECBPpiGuid,
          0,
          NULL,
          (VOID **)&DummyHandle
          );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeNeonCityEPECBPpiGuid is located!\n"));
    Status = TypeNeonCityEPECBInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeNeonCityEPECBInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeNeonCityEPECBInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPECBInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = PchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

	  Status = TypeNeonCityEPECBPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPECBIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPECBInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeNeonCityEPRP
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeNeonCityEPRPPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
      DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeNeonCityEPRPPpiGuid is located!\n"));
      Status = TypeNeonCityEPRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeNeonCityEPRPInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeNeonCityEPRPInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPRPInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPRPPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPRPPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPRPIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeNeonCityEPRPInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Status = TypeNeonCityEPRPInstallKtiEparamData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

// APTIOV_SERVER_OVERRIDE_RC_START : Support only NeonCity and LighteningRidge
#if 0
  //TypeOpalCitySTHI
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeOpalCitySTHIPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeOpalCitySTHIPpiGuid is located!\n"));
    Status = TypeOpalCitySTHIInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeOpalCitySTHIInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeOpalCitySTHIInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeOpalCitySTHIInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

     Status = TypeOpalCitySTHIPlatformUpdateUsbOcMappings(UbaConfigPpi);
     if (EFI_ERROR(Status)) {
       return Status;
     }

      Status = TypeOpalCitySTHIIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeOpalCitySTHIInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypePurleyLBGEPDVP
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypePurleyLBGEPDVPPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypePurleyLBGEPDVPPpiGuid is located!\n"));
    Status = TypePurleyLBGEPDVPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypePurleyLBGEPDVPInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypePurleyLBGEPDVPInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypePurleyLBGEPDVPInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

	  Status = TypePurleyLBGEPDVPPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypePurleyLBGEPDVPIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypePurleyLBGEPDVPInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeWolfPass
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeWolfPassPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeWolfPassPpiGuid is located!\n"));
	//Share same configuration as NeonCityEPRP to save SECPEI size.
    Status = TypeNeonCityEPRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeWolfPassInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeWolfPassInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeBuchananPass
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeBuchananPassPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeBuchananPassPpiGuid is located!\n"));
    Status = TypeBuchananPassInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeBuchananPassInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeBuchananPassInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeBuchananPassInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeBuchananPassPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeBuchananPassIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeBuchananPassInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeCrescentCity
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeCrescentCityPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeCrescentCityPpiGuid is located!\n"));
    Status = TypeCrescentCityInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeCrescentCityInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeCrescentCityInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeCrescentCityInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeCrescentCityPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeCrescentCityIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeCrescentCityInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeCrescentCityInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeHedtEV
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeHedtEVPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeHedtEVPpiGuid is located!\n"));
	//Share same configuration as HedtCRB to save SECPEI size.
    Status = TypeHedtCRBInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeHedtEVInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeHedtEVInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtEVInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtEVPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtEVIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtEVInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeHedtCRB
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeHedtCRBPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeHedtCRBPpiGuid is located!\n"));
    Status = TypeHedtCRBInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeHedtCRBInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeHedtCRBInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtCRBInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtCRBPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtCRBIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeHedtCRBInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the NeonCity EPECB KTI EparamData is installed here.
      Status = TypeNeonCityEPECBInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Support only NeonCity and LighteningRidge

  //TypeLightningRidgeEXRP
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEXRPPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEXRPPpiGuid is located!\n"));
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEXRPInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEXRPInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXRPInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXRPPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXRPIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXRPInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEXRP KTI EparamData is installed here.
      Status = TypeLightningRidgeEXRPInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeLightningRidgeEXECB1
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEXECB1PpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEXECB1PpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEXECB1InstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEXECB1InstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB1InstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB1PlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB1IioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB1InstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEXRP KTI EparamData is installed here.
      Status = TypeLightningRidgeEXRPInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeLightningRidgeEXECB2
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEXECB2PpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEXECB2PpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEXECB2InstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEXECB2InstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB2InstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

	 Status = TypeLightningRidgeEXECB2PlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB2IioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB2InstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEXRP KTI EparamData is installed here.
      Status = TypeLightningRidgeEXRPInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeLightningRidgeEXECB3
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEXECB3PpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEXECB3PpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEXECB3InstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEXECB3InstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB3InstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB3PlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB3IioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB3InstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEXRP KTI EparamData is installed here.
      Status = TypeLightningRidgeEXRPInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeLightningRidgeEXECB4
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEXECB4PpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEXECB4PpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEXECB4InstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEXECB4InstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB4InstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB4PlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB4IioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEXECB4InstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEXRP KTI EparamData is installed here.
      Status = TypeLightningRidgeEXRPInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

// APTIOV_SERVER_OVERRIDE_RC_START: Support only NeonCity and LighteningRidge
#if 0
  //TypeLightningRidgeEX8S1N
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEX8S1NPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEX8S1NPpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEX8S1NInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEX8S1NInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S1NInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Status = TypeLightningRidgeEX8S1NPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S1NIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S1NInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEX8S1N KTI EparamData is installed here.
      Status = TypeLightningRidgeEX8S1NInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeLightningRidgeEX8S2N
  Status = PeiServicesLocatePpi (
            &gEfiPeiPlatformTypeLightningRidgeEX8S2NPpiGuid,
            0,
            NULL,
            (VOID **)&DummyHandle
            );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeLightningRidgeEX8S2NPpiGuid is located!\n"));
	//Share same configuration as LightningRidgeEXRP to save SECPEI size.
    Status = TypeLightningRidgeEXRPInstallGpioData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
      return Status;
      }
      Status = TypeLightningRidgeEX8S2NInstallClockgenData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

       Status = TypeLightningRidgeEX8S2NInstallPcdData (UbaConfigPpi);
       if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S2NInstallSoftStrapData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status =  TypeWolfPassPchEarlyUpdate(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Status = TypeLightningRidgeEX8S2NPlatformUpdateUsbOcMappings(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S2NIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeLightningRidgeEX8S2NInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // To reduce code size, the LightningRidgeEX8S1N KTI EparamData is installed here.
      Status = TypeLightningRidgeEX8S1NInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

  //TypeKyanite
  Status = PeiServicesLocatePpi(
    &gEfiPeiPlatformTypeKyanitePpiGuid,
    0,
    NULL,
    (VOID **)&DummyHandle
    );
  if (Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeKyanitePpiGuid is located!\n"));
	//Share same configuration as NeonCityEPRP to save SECPEI size.
    Status = TypeNeonCityEPRPInstallGpioData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = InstallClockgenData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InstallPcdData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InstallSoftStrapData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = PchEarlyUpdate(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = TypeKyanitePlatformUpdateUsbOcMappings(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

      Status = TypeKyaniteIioPortBifurcationInit (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = TypeKyaniteInstallSlotTableData (UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }
     Status = TypeKyaniteInstallKtiEparamData(UbaConfigPpi);
      if (EFI_ERROR(Status)) {
        return Status;
      }

  }

#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Support only NeonCity and LighteningRidge
  //TypeNeonCityFPGA
  Status = PeiServicesLocatePpi (
             &gEfiPeiPlatformTypeNeonCityFPGAPpiGuid,
             0,
             NULL,
             (VOID **)&DummyHandle
             );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeNeonCityFPGAPpiGuid is located!\n"));

    Status = TypeNeonCityFPGAPlatformInstallGpioData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeNeonCityFPGAInstallPcdData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = PchEarlyUpdate(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeNeonCityFPGAPlatformUpdateUsbOcMappings(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeNeonCityFPGAIioPortBifurcationInit (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeNeonCityFPGAInstallSlotTableData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeNeonCityFPGAInstallKtiEparamData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

  }

// APTIOV_SERVER_OVERRIDE_RC_START : Support only NeonCity and LighteningRidge
#if 0
  //TypeOpalCityFPGA
  Status = PeiServicesLocatePpi (
             &gEfiPeiPlatformTypeOpalCityFPGAPpiGuid,
             0,
             NULL,
             (VOID **)&DummyHandle
             );
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "UBA :UbaMainPeimEntry: gEfiPeiPlatformTypeOpalCityFPGAPpiGuid is located!\n"));

    Status = TypeOpalCityFPGAPlatformInstallGpioData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeOpalCityFPGAInstallPcdData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = PchEarlyUpdate(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeOpalCityFPGAPlatformUpdateUsbOcMappings(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeOpalCityFPGAIioPortBifurcationInit (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeOpalCityFPGAInstallSlotTableData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeOpalCityFPGAInstallKtiEparamData(UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Support only NeonCity and LighteningRidge
  return Status;
}
