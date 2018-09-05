/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c) 2005-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file CrystalRidgePeim.c

**/

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Register/PchRegsLpc.h>

/**
  This function handles S3 resume task at the end of PEI

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
CrystalRidgeS3EndOfPeiNotify (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mS3EndOfPeiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  CrystalRidgeS3EndOfPeiNotify
};

/**
  This function handles S3 resume task at the end of PEI

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
CrystalRidgeS3EndOfPeiNotify (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  DEBUG((EFI_D_ERROR, "CR: Generating SW SMI for Nonce restoration\n"));

  IoWrite8(R_PCH_APM_CNT, 0xFB); // Generate SW SMI to re-program Nonce for all AEP DIMMs.

  return EFI_SUCCESS;
}

/**

  Driver entry point                

  @param FfsHeader -  Not used.
  @param PeiServices - General purpose services available to every PEIM.

  @retval EFI_SUCCESS:  Memory initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
EFIAPI
CrystalRidgeInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_BOOT_MODE BootMode;  
  //
  // Register EndOfPei Notify for S3 to generate SW SMI
  //
  DEBUG((EFI_D_ERROR, "CR: Register EndOfPei Notify for Nonce restoration\n"));
  PeiServicesGetBootMode (&BootMode);
  if (BootMode == BOOT_ON_S3_RESUME) {
    Status = PeiServicesNotifyPpi (&mS3EndOfPeiNotifyDesc);
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
