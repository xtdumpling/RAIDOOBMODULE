//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  FPGA Configuration library

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
Abstract:

  This file provides functions of FPGA  configuration values.
**/

#include <Library/FpgaConfigurationLib.h>
#include "FpgaConfigurationLibInternal.h"


// Internal Functions


EFI_STATUS
EFIAPI
InitHobCommon( FPGA_CONFIGURATION_HOB **ConfigHobData)
{
  EFI_HOB_GUID_TYPE         *Hob;
  FPGA_SOCKET_CONFIGURATION *FpgaConfig;
  UINTN                     i;
  FPGA_CONFIGURATION_HOB    *LocalConfigHobData;

  if (ConfigHobData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  LocalConfigHobData = NULL;

  //
  // get the HOB if it already exist
  //
  Hob = GetFirstGuidHob(&gFpgaSocketHobGuid);
  if (Hob != NULL) {
    *ConfigHobData = (FPGA_CONFIGURATION_HOB *) GET_GUID_HOB_DATA(Hob);
    return EFI_SUCCESS;
  }

  //
  // if the HOB does not exist, create it
  //
  if (Hob == NULL) {
    DEBUG((EFI_D_ERROR, "FPGA Configuration Get HOB-> HOB is not found, create it!\n"));
    LocalConfigHobData = InternalCreateHob();

    //
    // If we create the HOB, then set defaults
    //
    if (LocalConfigHobData != NULL) {
      DEBUG((EFI_D_ERROR, "FPGA Configuration Get HOB-> create it worked, init it!\n"));

      //
      // Init the variable fields
      //
      // set the FV as not found, FPGA as not active, Platform Enable and HSSI EQ tuning disable

      LocalConfigHobData->FpgaFvFound         = FALSE;
      LocalConfigHobData->FpgaPlatformEnabled = 0xFF;
      LocalConfigHobData->HssiEqTuningEn      = 0;

      // the bit fields get set to all 0's so all are off

      LocalConfigHobData->FpgaSktPresent    = 0;
      LocalConfigHobData->FpgaSktActive     = 0;
      LocalConfigHobData->FpgaSktError      = 0;
      LocalConfigHobData->FpgaSktProgrammed = 0;

      // Loop through the arrays, setting defaults for them

      for (i = 0; i < FPGA_MAX_SOCKET; i++) {
         LocalConfigHobData->FpgaBitStreamGuid[i] = FPGA_NULL_GUID_INDEX;
         LocalConfigHobData->FpgaBitStreamStatus[i] = FPGA_LOAD_STATUS_MAX;
         LocalConfigHobData->FpgaSktThermalTH1[i] = FPGA_INVALID_TH;
         LocalConfigHobData->FpgaSktThermalTH2[i] = FPGA_INVALID_TH;
         LocalConfigHobData->FpgaHssiCardID[i] = 0;
      }

      //
      // now look for and get the setup variables
      //

      FpgaConfig = FpgaGetVariable();

      //
      // If Variable pointer not NULL, Vaiable found.
      //
      if (FpgaConfig != NULL) {
        DEBUG((EFI_D_ERROR, "FPGA Configuration Get HOB-> Vaiable found use it!\n"));

        // Get the Platform enabled flag

        LocalConfigHobData->FpgaPlatformEnabled = FpgaConfig->FpgaSetupEnabled;
        LocalConfigHobData->HssiEqTuningEn = FpgaConfig->HssiEqTuningEn;

        // Loop through the array, moving the GUID's if they are enabled.

        for (i = 0; i < FPGA_MAX_SOCKET; i++) {
          //
          // Update Thermal Threshold configuration from Fpga Socket Variable
          //
          LocalConfigHobData->FpgaSktThermalTH1[i] = (FpgaConfig->FpgaThermalTH1[i]) ;
          LocalConfigHobData->FpgaSktThermalTH2[i] = (FpgaConfig->FpgaThermalTH2[i]) ;
          //
          // Update Socket Guid Index from Fpga Socket Variable
          //
          if (FpgaConfig->FpgaSocketGuid[i] != FPGA_GUID_INDEX_AUTO_VALUE) {
            if (FpgaConfig->FpgaSocketGuid[i] == FPGA_GUID_INDEX_NONE_VALUE) {
              // no index selected
              LocalConfigHobData->FpgaBitStreamGuid[i] = FPGA_NULL_GUID_INDEX ;
            } else {
              // move the Index to the HOB, note index in variable is +1 for 0 being not chosen
              LocalConfigHobData->FpgaBitStreamGuid[i] = (FpgaConfig->FpgaSocketGuid[i]) - 1;
            }
          }
        }

        FreePool (FpgaConfig);
      }

    }

  }

  if (LocalConfigHobData == NULL) {
    DEBUG((EFI_D_ERROR, "FPGA Configuration Get HOB-> HOB IS NULL, could not create!\n"));
    return EFI_NOT_FOUND;
  }
  ASSERT(LocalConfigHobData != NULL);
  *ConfigHobData = LocalConfigHobData;

  //
  // Init the PCD value
  //
  (*ConfigHobData)->FpgaPcdValue = PcdGetBool(PcdSktFpgaActive);

  return EFI_SUCCESS;

}
//
// Functions implemented in the Library
//


/**
  Set values in FPGA Configuration

  @param[ in ] FpgaConfigurationPointer      Pointer to the FPGA Configuration with new values

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         The FPGA is not supported / Global enable is FALSE
  @retval EFI_NOT_FOUND     The function could not locate the FPGA data
**/

EFI_STATUS
EFIAPI
FpgaConfigurationSetValues (
  IN FPGA_CONFIGURATION *FpgaConfigurationPointer
  )
{
  EFI_STATUS    Status;
  UINTN         i;
  FPGA_CONFIGURATION_HOB *ConfigHobData;

  Status = InitHobCommon(&ConfigHobData);

  if(EFI_ERROR (Status)){
   DEBUG ((EFI_D_ERROR, "FpgaConfigurationSetValues-> HOB error, return EFI_NOT_FOUND!\n"));
   return EFI_NOT_FOUND;
  }

  ConfigHobData->FpgaSktPresent       = FpgaConfigurationPointer->FpgaSktPresent;
  ConfigHobData->FpgaSktActive        = FpgaConfigurationPointer->FpgaSktActive;
  ConfigHobData->FpgaSktError         = FpgaConfigurationPointer->FpgaSktError;
  ConfigHobData->FpgaSktProgrammed    = FpgaConfigurationPointer->FpgaSktProgrammed;

  //
  // Note: we do not save the PCD value, it is read only
  //
  ConfigHobData->FpgaPlatformEnabled  = FpgaConfigurationPointer->FpgaPlatformEnabled;
  ConfigHobData->FpgaFvFound          = FpgaConfigurationPointer->FpgaFvFound;
  ConfigHobData->HssiEqTuningEn       = FpgaConfigurationPointer->HssiEqTuningEn;

  for (i = 0; i < FPGA_MAX_SOCKET; i++) {
   ConfigHobData->FpgaBitStreamGuid[i]     = FpgaConfigurationPointer->FpgaBitStreamGuid[i];
   ConfigHobData->FpgaBitStreamStatus[i]   = FpgaConfigurationPointer->FpgaBitStreamStatus[i];
   ConfigHobData->FpgaSocketErrorReturn[i] = FpgaConfigurationPointer->FpgaSocketErrorReturn[i];
   ConfigHobData->FpgaSktThermalTH1[i]     = FpgaConfigurationPointer->FpgaSktThermalTH1[i];
   ConfigHobData->FpgaSktThermalTH2[i]     = FpgaConfigurationPointer->FpgaSktThermalTH2[i];
   ConfigHobData->FpgaHssiCardID[i]        = FpgaConfigurationPointer->FpgaHssiCardID[i];
  }


  return EFI_SUCCESS;
}




/**
  Get FPGA Configuration into

  @param[ out] FpgaConfigurationPointer     Pointer to save the FPGA Configuration data to

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         The FPGA is not supported / Global enable is FALSE
  @retval EFI_INVALID_PARAMETER   The Global Enable Value changed, it's read only for this function.
  @retval EFI_NOT_FOUND     The function could not locate the FPGA data
**/

EFI_STATUS
EFIAPI
FpgaConfigurationGetValues (
  IN FPGA_CONFIGURATION *FpgaConfigurationPointer
  )
{
  EFI_STATUS    Status;
  UINTN         i;
  FPGA_CONFIGURATION_HOB *ConfigHobData;


  Status = InitHobCommon(&ConfigHobData);

  if(EFI_ERROR (Status)){
   DEBUG ((EFI_D_ERROR, "FpgaConfigurationGetValues-> HOB error, return EFI_NOT_FOUND!\n"));
   return EFI_NOT_FOUND;
  }

  FpgaConfigurationPointer->FpgaSktPresent      = ConfigHobData->FpgaSktPresent;
  FpgaConfigurationPointer->FpgaSktActive       = ConfigHobData->FpgaSktActive;
  FpgaConfigurationPointer->FpgaSktError        = ConfigHobData->FpgaSktError;
  FpgaConfigurationPointer->FpgaSktProgrammed   = ConfigHobData->FpgaSktProgrammed;

  FpgaConfigurationPointer->FpgaPlatformEnabled = ConfigHobData->FpgaPlatformEnabled;
  FpgaConfigurationPointer->FpgaPcdValue        = ConfigHobData->FpgaPcdValue;
  FpgaConfigurationPointer->FpgaFvFound         = ConfigHobData->FpgaFvFound;
  FpgaConfigurationPointer->HssiEqTuningEn      = ConfigHobData->HssiEqTuningEn;

  for (i = 0; i < FPGA_MAX_SOCKET; i++) {
    FpgaConfigurationPointer->FpgaBitStreamGuid[i]     = ConfigHobData->FpgaBitStreamGuid[i];
    FpgaConfigurationPointer->FpgaBitStreamStatus[i]   = ConfigHobData->FpgaBitStreamStatus[i];
    FpgaConfigurationPointer->FpgaSocketErrorReturn[i] = ConfigHobData->FpgaSocketErrorReturn[i];
    FpgaConfigurationPointer->FpgaSktThermalTH1[i]     = ConfigHobData->FpgaSktThermalTH1[i];
    FpgaConfigurationPointer->FpgaSktThermalTH2[i]     = ConfigHobData->FpgaSktThermalTH2[i];
    FpgaConfigurationPointer->FpgaHssiCardID[i]        = ConfigHobData->FpgaHssiCardID[i];
  }


  return EFI_SUCCESS;
}




/**
  Get FPGA Global Enable, convenience function to get value in Configuration

  @param[ out] FpgaGlobalEnable   Pointer to FPGA Global enable value to return

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_FOUND     The function could not locate the FPGA data
**/

EFI_STATUS
EFIAPI
FpgaGlobalEnableGet (
  IN  FPGA_FLOW_STAGE   FpgaFlowStage,
  OUT BOOLEAN *FpgaGlobalEnable
  )
{
  EFI_STATUS    Status;
  FPGA_CONFIGURATION_HOB *ConfigHobData;

  //
  // assume global disabled
  //
  *FpgaGlobalEnable = FALSE;

  Status = InitHobCommon(&ConfigHobData);

  if(EFI_ERROR (Status)){
     DEBUG ((EFI_D_ERROR, "FpgaGlobalEnableGet-> HOB error, return EFI_NOT_FOUND!\n"));
     return EFI_NOT_FOUND;
  }

  switch (FpgaFlowStage){
  case FPGA_FLOW_STAGE_KTI    :
    if (ConfigHobData->FpgaPcdValue        != 0 &&
        ConfigHobData->FpgaPlatformEnabled != 0
       )
      *FpgaGlobalEnable = TRUE;
    break;
  case FPGA_FLOW_STAGE_BS_LOAD:
    if (ConfigHobData->FpgaPcdValue          != 0 &&
        ConfigHobData->FpgaPlatformEnabled   != 0 &&
        ConfigHobData->FpgaSktPresent        != 0
       )
      *FpgaGlobalEnable = TRUE;
    break;
  case FPGA_FLOW_STAGE_SETUP  :
    if (ConfigHobData->FpgaPcdValue          != 0 &&
        ConfigHobData->FpgaSktPresent        != 0
       )
      *FpgaGlobalEnable = TRUE;
    break;
  default:
    break;
  }
  return EFI_SUCCESS;
}
