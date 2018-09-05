/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MePolicyInitPei.c

@brief:

  This file is SampleCode for Intel ME PEI SPS Policy initialization.

**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/MeSpsPolicyPei.h"
#include "Library/MeSpsPolicyInitPeiLib.h"
#include "Library/MeSpsPolicyUpdatePeiLib.h"
#include "Library/MeTypeLib.h"

#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include "Library/PeiAmtPolicyLib.h"
#endif // AMT_SUPPORT
#include "Library/PeiMePolicyLib.h"
#include "Library/PeiMePolicyUpdateLib.h"
#include "Library/MeSpsPolicyDebugDump.h"

///
/// Function implementations
///
/**

  @brief
  Initialize Intel ME PEI SPS Policy

  @param[in] FfsHeader            Pointer to Firmware File System file header.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.

**/
EFI_STATUS
MePolicyInitPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  ME_POLICY_PPI               *MePolicyPpi = NULL;
#if AMT_SUPPORT
  AMT_POLICY_PPI              *AmtPolicyPpi = NULL;
#endif

  DEBUG((DEBUG_INFO, "[ME Policy] MePolicyInitPeiEntryPoint\n"));

  Status = MeCreatePolicyDefaults(&MePolicyPpi);
  if (EFI_ERROR(Status) || (MePolicyPpi == NULL))
  {
    DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: Cannot create ME Policy PPI (%r)\n", Status));
    ASSERT (MePolicyPpi != NULL);
  }
  else
  {
    Status = MeInstallPolicyPpi(MePolicyPpi);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: Cannot install ME Policy PPI (%r)\n", Status));
    }
    Status = UpdatePeiMePolicy(MePolicyPpi);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: ME Policy update failed (%r)\n", Status));
    }
  }
#if AMT_SUPPORT
  Status = AmtCreatePolicyDefaults(&AmtPolicyPpi);
  if (EFI_ERROR(Status) || (AmtPolicyPpi == NULL))
  {
    DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: Cannot create AMT Policy PPI (%r)\n", Status));
    ASSERT(AmtPolicyPpi != NULL);
  }
  else
  {
    Status = AmtInstallPolicyPpi(AmtPolicyPpi);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: Cannot install AMT Policy PPI (%r)\n", Status));
    }
    Status = UpdatePeiAmtPolicy(AmtPolicyPpi);
    if (EFI_ERROR(Status))
    {
      if (MeTypeIsAmt())  // This is error for AMT ME only
      {
        DEBUG((DEBUG_ERROR, "[ME Policy] ERROR: AMT Policy update failed (%r)\n", Status));
      }
      else
      {
        DEBUG((DEBUG_WARN, "[ME Policy] WARNING: AMT Policy update failed (%r)\n", Status));
      }
    }
  }
#endif // AMT_SUPPORT

#if SPS_SUPPORT
  Status = SpsPolicyInitPei(PeiServices);
  ASSERT_EFI_ERROR(Status);
#endif

  MePolicyDump(DEBUG_INFO,
               ME_POLICY_DUMP_ME
#if AMT_SUPPORT
               + ME_POLICY_DUMP_AMT
#endif
#if SPS_SUPPORT
               + ME_POLICY_DUMP_SPS
#endif
               );
  
  return Status;
}
