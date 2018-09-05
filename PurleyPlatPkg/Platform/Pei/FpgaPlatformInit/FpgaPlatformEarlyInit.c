/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FgpaPlatformEarlyInit.c

  Do platform specific initializations for FPGA.

**/

#include "FpgaPlatformEarlyInit.h"

//
// Local variables
//

EFI_PEI_MP_SERVICES_PPI           *mMpServices;
EFI_PROCESSOR_INFORMATION         *mProcessorLocBuf;
UINTN                             mProcessorNum;
UINTN                             mBspIndex;
UINT32                            mBspSocketId;
MRC_HOOKS_CHIP_SERVICES_PPI       *mMrcHooksChipServices;
PSYSHOST                          host = NULL;


static EFI_PEI_PPI_DESCRIPTOR       mFpgaInitPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gFpgaInitPpiGuid,
  NULL
};

//
// Local functions
//

/**
  Update Fpga Platform Configuration
  1. Set FPGA BBS Guid Index. Priority: Setup > HSSI Card Id > Default Platform Policy.
  2. Update Thermal Configuration.

  @param[in] PeiServices           General purpose services available to every PEIM.
  @param[in] FpgaConfiguration     Pointer to the FPGA Configuration data

  @retval EFI_SUCCESS              Operation completed successfully.
  @retval Otherwise                initialization failed.
**/
EFI_STATUS
EFIAPI
UpdateFpgaPlatformConfiguration (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN FPGA_CONFIGURATION        *FpgaConfiguration
  )
{
  EFI_STATUS                            Status;
  UINT8                                 i;
  FPGA_SOCKET_CONFIGURATION             FpgaSocketConfiguration;
  UINT8                                 FpgaBitStreamGuidIndex;
  UINT8                                 FpgaHssiCardID;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  UINTN                                 VariableSize;

  if (FpgaConfiguration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Update the BBS Guid Index from default platform policy.
  //
  for (i = 0; i < FPGA_MAX_SOCKET; i++) {
    Status = GetBBSIndexFromPlatformPolicy (i, &FpgaBitStreamGuidIndex);
    if (EFI_ERROR (Status)) {
      FpgaBitStreamGuidIndex = 0xFF;
    }
    if (FpgaBitStreamGuidIndex != 0xFF) {
      DEBUG((EFI_D_ERROR, "FPGA Platform Policy: Socket[%x] BBSGuidIndex = %x \n", i, FpgaBitStreamGuidIndex));
      FpgaConfiguration->FpgaBitStreamGuid[i] = FpgaBitStreamGuidIndex;
    }
  }

  //
  // Update the BBS Guid Index from Fpga Hssi Card information.
  //
  for (i = 0; i < FPGA_MAX_SOCKET; i++) {
    Status = GetBBSIndexFromHssiCardId (i, &FpgaBitStreamGuidIndex, &FpgaHssiCardID);
    if (EFI_ERROR (Status)) {
      FpgaBitStreamGuidIndex = 0xFF;
      FpgaHssiCardID = 0;
    }
    if (FpgaBitStreamGuidIndex != 0xFF) {
      DEBUG((EFI_D_ERROR, "FPGA HssiCard: Socket[%x] BBSGuidIndex = %x, FpgaHssiCardID = %x\n", i, FpgaBitStreamGuidIndex, FpgaHssiCardID));
      FpgaConfiguration->FpgaBitStreamGuid[i] = FpgaBitStreamGuidIndex;
      FpgaConfiguration->FpgaHssiCardID[i]    = FpgaHssiCardID;
    }
  }

  //
  // Get Fpga Socket Variable
  //
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             &PeiVariable
                             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (FPGA_SOCKET_CONFIGURATION);

  Status = PeiVariable->GetVariable (
                          PeiVariable,
                          FPGA_SOCKET_CONFIGURATION_NAME,
                          &gFpgaSocketVariableGuid,
                          NULL,
                          &VariableSize,
                          &FpgaSocketConfiguration
                          );

  if (!EFI_ERROR (Status)) {
      for (i = 0; i < FPGA_MAX_SOCKET; i++) {
        //
        // Update Thermal Threshold configuration from Fpga Socket Variable
        //
        FpgaConfiguration->FpgaSktThermalTH1[i] = (FpgaSocketConfiguration.FpgaThermalTH1[i]);
        FpgaConfiguration->FpgaSktThermalTH2[i] = (FpgaSocketConfiguration.FpgaThermalTH2[i]);
        //
        // Update Socket Guid Index from Fpga Socket Variable
        //
        if (FpgaSocketConfiguration.FpgaSocketGuid[i] != FPGA_GUID_INDEX_AUTO_VALUE) {
          if (FpgaSocketConfiguration.FpgaSocketGuid[i] == FPGA_GUID_INDEX_NONE_VALUE) {
            DEBUG((EFI_D_ERROR, "FPGA Configuration Vaiable: Socket[%x] BBSGuidIndex = NONE \n", i));
            FpgaConfiguration->FpgaBitStreamGuid[i] = FPGA_NULL_GUID_INDEX;
          } else {
            DEBUG((EFI_D_ERROR, "FPGA Configuration Vaiable: Socket[%x] BBSGuidIndex = %x \n", i, (FpgaSocketConfiguration.FpgaSocketGuid[i]) - 1));
            FpgaConfiguration->FpgaBitStreamGuid[i] = (FpgaSocketConfiguration.FpgaSocketGuid[i]) - 1;
          }

          if (FpgaConfiguration->FpgaBitStreamGuid[i] != FPGA_GBE_GUID_INDEX) {
            FpgaConfiguration->FpgaHssiCardID[i] = 0;
          }
        }
     }
  }
  return EFI_SUCCESS;
}


/**
  GetMpServicesData, Load the Mp services structures.

  @param PeiServices        General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Load the Mp services structures. successfully.

**/
EFI_STATUS
EFIAPI
GetMpServicesData (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  UINTN                             EnabledProcessorNum;
  UINTN                             Index;

  Status = mMpServices->GetNumberOfProcessors(
                          PeiServices,
                          mMpServices,
                          &mProcessorNum,
                          &EnabledProcessorNum
                          );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof(EFI_PROCESSOR_INFORMATION) * mProcessorNum,
                             (VOID **) &mProcessorLocBuf
                             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Get each processor Location info
  //
  for (Index = 0; Index < mProcessorNum; Index++) {
    Status = mMpServices->GetProcessorInfo(
                            PeiServices,
                            mMpServices,
                            Index,
                            &mProcessorLocBuf[Index]
                            );
    if (EFI_ERROR(Status)) {
      FreePool(mProcessorLocBuf);
      return Status;
    }
  }

  Status = mMpServices->WhoAmI (
                          PeiServices,
                          mMpServices,
                          &mBspIndex
                          );

  ASSERT_EFI_ERROR (Status);
  mBspSocketId = mProcessorLocBuf[mBspIndex].Location.Package;

  return Status;
}

/**
  Platform specific initializations for Fpga.

  @param FileHandle         Pointer to the PEIM FFS file header.
  @param PeiServices        General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Operation completed successfully.
  @retval Otherwise         Platform initialization failed.

**/
EFI_STATUS
EFIAPI
FpgaPlatformEarlyInitEntry (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                            Status;
  FPGA_CONFIGURATION                    FpgaConfiguration;

  DEBUG ((EFI_D_ERROR, "FpgaPlatformEarlyInit Entry! \n"));

  //
  // Get the FpgaConfiguration
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);

  DEBUG((EFI_D_ERROR, "FpgaConfigurationGetValues status = %r !\n", Status));
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  //
  //Retrun if FPGA feature is disabled or FPGA SKU is not detected.
  //
  if ((FpgaConfiguration.FpgaPcdValue == FALSE) || (FpgaConfiguration.FpgaSktPresent == 0 )) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_ERROR, "FpgaSktPresent is Set to 0x%X! \n", FpgaConfiguration.FpgaSktPresent));

  //
  // Get the MRC Hooks Chip Services PPI
  //
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gMrcHooksChipServicesPpiGuid,
                             0,
                             NULL,
                             &mMrcHooksChipServices
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Find MRC Hooks Chip Services Ppi failed, return it's error!\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  //
  // Get the SYSHOST
  //
  host = (SYSHOST *)(UINTN) PcdGet64 (PcdSyshostMemoryAddress);

  //
  // get the MP services PPI
  //
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiMpServicesPpiGuid,
                             0,
                             NULL,
                             &mMpServices
                             );
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Find Mp services PPI failed, return it's error!\n"));
    return Status;
  }

  GetMpServicesData (PeiServices);

  //
  // Update the Fpga Configuration.
  //
  Status = UpdateFpgaPlatformConfiguration (PeiServices, &FpgaConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "UpdateFpgaPlatformConfiguration failed.\n"));
  }

  //
  // Set the final FpgaConfiguration
  //
  Status = FpgaConfigurationSetValues (&FpgaConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FpgaConfigurationSetValues failed.\n"));
  }

  //
  // Check whether FPGA BBS have been loaded by N4PE or all the FPGA Sockets are disabled by setup, if yes, skit load the FPGA FV and skip update Fpga Configuration.
  //
  if (((mMrcHooksChipServices->ReadCpuPciCfgEx (host, (UINT8)mBspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG)) & BIT5) || (FpgaConfiguration.FpgaPlatformEnabled == 0 )) {
    //
    // Install  FPGA platform init ppi
    //
    Status = (*PeiServices)->InstallPpi (PeiServices, &mFpgaInitPpi);
    return Status;
  }

  //
  // Load FPGA BBS binaries file if FPGA BBS is not programmed.
  //
  Status = FpgaBbsInit (PeiServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FpgaBbsInit failed.\n"));
    return Status;
  }

  //
  // Install FPGA platform init ppi
  //
  Status = (*PeiServices)->InstallPpi (PeiServices, &mFpgaInitPpi);

  return Status;
}
