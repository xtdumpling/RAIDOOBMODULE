/** @file
  This is a library for ME functionality.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include <PiPei.h>

#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/MeLibPei.h>
#include <MeChipset.h>
#include <Library/MeTypeLib.h>
#include <Library/PeiAmtLib.h>
#include <Library/MeSpsPolicyUpdatePeiLib.h>
#include <Library/MeSpsPolicyAccessPeiLib.h>
#include <Library/PeiMePolicyUpdateLib.h>
#include <Library/MeSpsPolicyDebugDump.h>
#include <Library/PchPmcLib.h>

/**
  @brief
  Enable/Disable All ME devices based on Policy

  @param[in] pPolicy    Pointer to ME Policy PPI
**/
EFI_STATUS
MeDeviceConfigure(
  ME_POLICY_PPI    *pPolicy
  )
{
  if (pPolicy == NULL)
  {
    ASSERT(pPolicy != NULL);
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((EFI_D_INFO, "[ME] Disabling ME functions:"));
  if (PchIsDwrFlow() == TRUE) {
    DEBUG ((EFI_D_INFO, " DWR: Exit\n"));
    return EFI_SUCCESS;
  }
  if (pPolicy->MeConfig.HeciCommunication1 == ME_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 0 (HECI-1)"));
  if (pPolicy->MeConfig.HeciCommunication2 == ME_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 1 (HECI-2)"));
  if (pPolicy->MeConfig.HeciCommunication3 == ME_DEVICE_DISABLED) DEBUG((EFI_D_INFO, " 4 (HECI-3)"));
  if (pPolicy->MeConfig.IderDeviceEnable == ME_DEVICE_DISABLED)   DEBUG((EFI_D_INFO, " 2 (IDE-R)"));
  if (pPolicy->MeConfig.KtDeviceEnable == ME_DEVICE_DISABLED)     DEBUG((EFI_D_INFO, " 3 (KT)"));
  DEBUG((EFI_D_INFO, "\n"));
  
  MeDeviceControl(HECI1, pPolicy->MeConfig.HeciCommunication1);
  MeDeviceControl(HECI2, pPolicy->MeConfig.HeciCommunication2);
  MeDeviceControl(HECI3, pPolicy->MeConfig.HeciCommunication3);
  MeDeviceControl(IDER, pPolicy->MeConfig.IderDeviceEnable);
  MeDeviceControl(SOL, pPolicy->MeConfig.KtDeviceEnable);
  
  return EFI_SUCCESS;
} // MeDeviceConfigure

/**
  This function performs basic initialization for ME
  in PEI phase after memory is initialized.
**/
VOID
EFIAPI
MePostMemInit (
  VOID
  )
{
  ME_POLICY_PPI  *MePolicyPpi;

#if AMT_SUPPORT
  if (MeTypeIsAmt())
  {
    MePostMemInitAmt();
  }
#endif // AMT_SUPPORT

#if SPS_SUPPORT
  {
    SPS_POLICY_PPI *pSpsPolicy = GetSpsPolicyPpi();
    
    if (pSpsPolicy != NULL)
    {
      UpdateSpsPolicy(pSpsPolicy);
    }
  }
#endif
  MePolicyPpi = GetMePolicyPpi();
  if (MePolicyPpi != NULL)
  {
    UpdatePeiMePolicy(MePolicyPpi);
  }

  MePolicyDump(EFI_D_INFO,
               ME_POLICY_DUMP_ME
#if SPS_SUPPORT
               + ME_POLICY_DUMP_SPS
#endif
               );
  //
  // Before potential disabling HECI1
  // We have to store Firmware Statuses in to HOB
  //
  SaveFwStsToHob();

  MeDeviceConfigure(MePolicyPpi);
}

#ifndef AMT_SUPPORT
/**
  ME End of PEI callback function. Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei(VOID)
{
}
#endif // ~AMT_SUPPORT
