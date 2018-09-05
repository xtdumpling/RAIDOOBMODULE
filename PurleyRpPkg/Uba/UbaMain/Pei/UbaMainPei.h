//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA MAIN PEIM.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/


#ifndef _UBA_MAIN_PEI_PEIM_H_
#define _UBA_MAIN_PEI_PEIM_H_

#include <PiPei.h>
#include <Uefi/UefiSpec.h>

#include <Ppi/UbaCfgDb.h>

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/UbaPchEarlyUpdateLib.h>
#include <Ppi/UbaMakerPpi.h>
#include <Platform.h>
#include <BoardTypes.h>
#include <Library/GpioNativeLib.h>


//PCD
EFI_STATUS
TypeNeonCityEPRPPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
PlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeHedtCRBPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBPlatformUpdateAcpiTablePcds (
  VOID
);


EFI_STATUS
TypeHedtEVPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVPlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeBuchananPassPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBuchananPassPlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeCrescentCityPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeCrescentCityPlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeLightningRidgeEX8S1NPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S1NPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEX8S1NInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEXECB1PlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB1PlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEXECB2PlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB2PlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEXECB3PlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB3PlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEXECB4PlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB4PlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeLightningRidgeEXRPPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXRPPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeNeonCityEPECBPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPECBPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeOpalCitySTHIPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCitySTHIPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypePurleyLBGEPDVPPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypePurleyLBGEPDVPPlatformUpdateAcpiTablePcds (
  VOID
);
EFI_STATUS
TypeWolfPassPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeWolfPassPlatformUpdateAcpiTablePcds (
  VOID
);

// TypeKyanite
EFI_STATUS
InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
PchEarlyUpdate (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeKyanitePlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeKyanitePlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeKyaniteInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

// TypeNeonCityFPGA
EFI_STATUS
TypeNeonCityFPGAPlatformInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeNeonCityFPGAPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityFPGAPlatformUpdateAcpiTablePcds (
  VOID
);

// TypeOpalCityFPGA
EFI_STATUS
TypeOpalCityFPGAPlatformInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeOpalCityFPGAPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCityFPGAPlatformUpdateAcpiTablePcds (
  VOID
);

// TypeNeonCityEPRP
EFI_STATUS
TypeNeonCityEPRPInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeNeonCityEPRPInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPPchEarlyUpdate (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPRPInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeHedtCRB
EFI_STATUS
TypeHedtCRBInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeHedtCRBInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtCRBInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeHedtEV
EFI_STATUS
TypeHedtEVInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeHedtEVInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeHedtEVInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeBuchananPass
EFI_STATUS
TypeBuchananPassInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeBuchananPassInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBuchananPassInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBuchananPassInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBuchananPassIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBuchananPassInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeBuchananPassInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeCrescentCity
EFI_STATUS
TypeCrescentCityInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeCrescentCityInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeCrescentCityInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeCrescentCityInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeCrescentCityIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeCrescentCityInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeCrescentCityInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeLightningRidgeEX8S1N
EFI_STATUS
TypeLightningRidgeEX8S1NInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEX8S1NInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S1NInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S1NInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S1NIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S1NInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeLightningRidgeEX8S2N
EFI_STATUS
TypeLightningRidgeEX8S2NInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEX8S2NInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEX8S2NInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEX8S2NInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeLightningRidgeEXECB1
EFI_STATUS
TypeLightningRidgeEXECB1InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEXECB1InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB1InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB1InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB1IioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB1InstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEXECB1InstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeLightningRidgeEXECB2
EFI_STATUS
TypeLightningRidgeEXECB2InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEXECB2InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB2InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB2InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB2IioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB2InstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEXECB2InstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
////TypeLightningRidgeEXECB3
EFI_STATUS
TypeLightningRidgeEXECB3InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEXECB3InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB3InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB3InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB3IioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB3InstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEXECB3InstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeLightningRidgeEXECB4
EFI_STATUS
TypeLightningRidgeEXECB4InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEXECB4InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB4InstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB4InstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB4IioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXECB4InstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEXECB4InstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeLightningRidgeEXRP
EFI_STATUS
TypeLightningRidgeEXRPInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeLightningRidgeEXRPInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXRPInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXRPInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXRPIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeLightningRidgeEXRPInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeLightningRidgeEXRPInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeNeonCityEPECB
EFI_STATUS
TypeNeonCityEPECBInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeNeonCityEPECBInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPECBInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPECBInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPECBIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityEPECBInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeNeonCityEPECBInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypeOpalCitySTHI
EFI_STATUS
TypeOpalCitySTHIInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeOpalCitySTHIInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCitySTHIInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCitySTHIInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCitySTHIIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCitySTHIInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeOpalCitySTHIInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
//TypePurleyLBGEPDVP
EFI_STATUS
TypePurleyLBGEPDVPInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypePurleyLBGEPDVPInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypePurleyLBGEPDVPInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypePurleyLBGEPDVPInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypePurleyLBGEPDVPIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypePurleyLBGEPDVPInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypePurleyLBGEPDVPInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeWolfPass
EFI_STATUS
EFIAPI
TypeWolfPasschEarlyUpdate(
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
  );

EFI_STATUS
TypeWolfPassInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeWolfPassInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeWolfPassInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeWolfPassInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
EFIAPI
TypeWolfPassPchEarlyUpdate(
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
  );

EFI_STATUS
TypeWolfPassIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);  

EFI_STATUS
TypeWolfPassInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeWolfPassInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeKyanite
EFI_STATUS
TypeKyaniteIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);  

EFI_STATUS
TypeKyaniteInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeNeonCityFPGA
EFI_STATUS
TypeNeonCityFPGAInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeNeonCityFPGAIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);  

EFI_STATUS
TypeNeonCityFPGAInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
EFI_STATUS
TypeNeonCityFPGAInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

//TypeOpalCityFPGA
EFI_STATUS
TypeOpalCityFPGAInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCityFPGAIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);  

EFI_STATUS
TypeOpalCityFPGAInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeOpalCityFPGAInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

#endif // _UBA_MAIN_PEI_PEIM_H_
