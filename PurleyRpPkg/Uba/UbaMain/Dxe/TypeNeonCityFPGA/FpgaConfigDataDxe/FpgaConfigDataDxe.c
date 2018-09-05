/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaConfigDataDxe.c

Abstract:

--*/
#include "FpgaConfigDataDxe.h"

//
// Modular variables needed by this driver
//
FPGA_CONFIGURATION            FpgaConfiguration;


typedef struct {
  UINT8                               SocketId;
  UINT8                               HssiCardId;
  HSSI_CONFIG_DATA_TABLE              *HssiConfigDataTable;
  UINTN                               NumberofConfigData;
} FPGA_CONFIG_DATA_MAP;

FPGA_CONFIG_DATA_MAP          FpgaConfigDataMap[] = {
  {0x0, HSSI_CardID_2x40GbE, TypeNeonCityFPGAHssiEqParmTableSkt0_2x40G, sizeof (TypeNeonCityFPGAHssiEqParmTableSkt0_2x40G) / sizeof (HSSI_CONFIG_DATA_TABLE)},
  {0x0, HSSI_CardID_4x10GbE, TypeNeonCityFPGAHssiEqParmTableSkt0_4x10G, sizeof (TypeNeonCityFPGAHssiEqParmTableSkt0_4x10G) / sizeof (HSSI_CONFIG_DATA_TABLE)}
};

/**
  Load FPGA HSSI config data according to add-in card detection.

  @param This                       Pointer to the EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL instance.
  @param FPGAHssiConfigDataTable    The index of CPU socket.
  @param FPGAHssiConfigDataTable    The HSSI config data table.
  @param NumOfHssiConfigData        A pointer to number of HSSI config data table.

  @retval EFI_SUCCESS:              The function executed successfully.
  @retval Others                    Internal error when load HSSI config data.

**/
EFI_STATUS
EFIAPI
TypeNeonCityFPGAFpgaConfigDataDxe (
  IN EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL   *This,
  IN      UINT8                           SocketId,
  IN OUT  HSSI_CONFIG_DATA_TABLE          **FPGAHssiConfigDataTable,
  IN OUT  UINTN                           *NumOfHssiConfigData
  )
{
  UINTN                        Index;

  if ((FPGAHssiConfigDataTable == NULL) || (NumOfHssiConfigData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get HSSI config data for Socket index
  //
  *FPGAHssiConfigDataTable = NULL;
  *NumOfHssiConfigData = 0;

  if (!(FpgaConfiguration.FpgaSktActive & (1 << SocketId))) {
    return EFI_INVALID_PARAMETER;
  }

  if ((FpgaConfiguration.FpgaHssiCardID[SocketId] != HSSI_CardID_4x10GbE) &&
      (FpgaConfiguration.FpgaHssiCardID[SocketId] != HSSI_CardID_2x40GbE) &&
      (FpgaConfiguration.FpgaHssiCardID[SocketId] != HSSI_CardID_4x25GbE)) {
    DEBUG ((DEBUG_ERROR, "Unsupported HSSI Card: = %X.\n", FpgaConfiguration.FpgaHssiCardID[SocketId]));
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < sizeof (FpgaConfigDataMap) / sizeof (FPGA_CONFIG_DATA_MAP); Index++) {
    if ((FpgaConfigDataMap[Index].SocketId == SocketId) && (FpgaConfigDataMap[Index].HssiCardId == FpgaConfiguration.FpgaHssiCardID[SocketId])) {
      *FPGAHssiConfigDataTable = FpgaConfigDataMap[Index].HssiConfigDataTable;
      *NumOfHssiConfigData = FpgaConfigDataMap[Index].NumberofConfigData;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

//
// The FPGA HSSI Config Data Protocol instance produced by this driver
//
EFI_FPGA_HSSI_CONFIG_DATA_PROTOCOL  mFpgaHssiConfigData = {
  TypeNeonCityFPGAFpgaConfigDataDxe
};

/**
  The Driver Entry Point.

  The function is the driver Entry point.

  @param ImageHandle                A handle for the image that is initializing this driver
  @param SystemTable                A pointer to the EFI system table

  @retval EFI_SUCCESS               Driver initialized successfully
  @retval Others                    Internal error when install protocol.

**/
EFI_STATUS
EFIAPI
FpgaConfigDataEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                          Status;
  UINT8                               Index;

  DEBUG ((DEBUG_ERROR, "FpgaConfigData-TypeNeonCityFPGA\n"));

  //
  // BIOS detect FPGA SKU
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_ERROR, "FpgaConfigDataEntry - FpgaSktActive = 0x%X.\n", FpgaConfiguration.FpgaSktActive));
  if (FpgaConfiguration.FpgaSktActive == 0) {
    DEBUG ((DEBUG_ERROR, "FpgaConfigDataEntry() no FPGA activated.\n"));
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    DEBUG ((DEBUG_ERROR, "FpgaHssiCardID[%X] = 0x%X.\n", Index, FpgaConfiguration.FpgaHssiCardID[Index]));
    if ((FpgaConfiguration.FpgaHssiCardID[Index] != 0) && (FpgaConfiguration.FpgaHssiCardID[Index] != HSSI_CardID_PCIe)) {
      break;
    }
  }

  if (Index == MAX_SOCKET) {
    DEBUG ((DEBUG_ERROR, "FpgaConfigDataEntry() no HSSI card found.\n"));
    return EFI_SUCCESS;
  }

  //
  // Install EFI FPGA Config Data Protocol
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiFpgaHssiConfigDataProtocol,
                  EFI_NATIVE_INTERFACE,
                  &mFpgaHssiConfigData
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
