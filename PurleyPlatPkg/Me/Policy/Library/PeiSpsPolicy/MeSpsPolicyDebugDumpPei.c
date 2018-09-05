/** @file
  Dump whole ME_SPS_POLICY_PPI and serial out.

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include "Ppi/MePolicyPpi.h"
#include "Ppi/MeSpsPolicyPei.h"
#include "Ppi/AmtPolicyPpi.h"
#include "Library/MeTypeLib.h"
#include <Library/MeSpsPolicyAccessPeiLib.h>
#include <Library/MeSpsPolicyDebugDump.h>
#include <Library/PeiMePolicyLib.h>
#include <Library/PeiAmtPolicyLib.h>


/**
  Print ME SPS Config structure

  @param[in] DebugLevel Pointer to the ME config structure
  @param[in] pSpsCfg Pointer to the ME config structure
**/
VOID EFIAPI
MeSpsPrintConfig(
  IN     UINTN           DebugLevel,
  IN     ME_SPS_CONFIG  *pSpsCfg)
{
DEBUG_CODE_BEGIN();
  DEBUG((DebugLevel, "SPS Config Revision       : %d\n", pSpsCfg->Revision));
  DEBUG((DebugLevel, " SpsAltitude              : 0x%x\n", pSpsCfg->SpsAltitude));
  DEBUG((DebugLevel, " SpsMctpBusOwner          : 0x%x\n", pSpsCfg->SpsMctpBusOwner));
  DEBUG((DebugLevel, " PreDidMeResetEnabled     : 0x%x\n", pSpsCfg->PreDidMeResetEnabled));
  DEBUG((DebugLevel, " Heci1HideInMe            : %d\n", pSpsCfg->Heci1HideInMe));
  DEBUG((DebugLevel, " Heci2HideInMe            : %d\n", pSpsCfg->Heci2HideInMe));
  DEBUG((DebugLevel, " Heci3HideInMe            : %d\n", pSpsCfg->Heci3HideInMe));
  DEBUG((DebugLevel, " NmPwrOptBoot             : %d\n", pSpsCfg->NmPwrOptBoot));
  DEBUG((DebugLevel, " NmCores2Disable          : %d\n", pSpsCfg->NmCores2Disable));
#if ME_TESTMENU_FLAG
  DEBUG((DebugLevel, " MeHmrfpoEnableEnabled    : %d\n", pSpsCfg->MeHmrfpoEnableEnabled));
  DEBUG((DebugLevel, " MeHmrfpoLockEnabled      : %d\n", pSpsCfg->MeHmrfpoLockEnabled));
  DEBUG((DebugLevel, " NmPwrOptBootOverride     : %d\n", pSpsCfg->NmPwrOptBootOverride));
  DEBUG((DebugLevel, " NmCores2DisableOverride  : %d\n", pSpsCfg->NmCores2DisableOverride));
  DEBUG((DebugLevel, " NmPowerMsmtOverride      : %d\n", pSpsCfg->NmPowerMsmtOverride));
  DEBUG((DebugLevel, " NmPowerMsmtSupport       : %d\n", pSpsCfg->NmPowerMsmtSupport));
  DEBUG((DebugLevel, " NmHwChangeOverride       : %d\n", pSpsCfg->NmHwChangeOverride));
  DEBUG((DebugLevel, " NmHwChangeStatus         : %d\n", pSpsCfg->NmHwChangeStatus));
  DEBUG((DebugLevel, " NmPtuLoadOverride        : %d\n", pSpsCfg->NmPtuLoadOverride));
  DEBUG((DebugLevel, " MeGrLockEnabled          : %d\n", pSpsCfg->MeGrLockEnabled));
  DEBUG((DebugLevel, " MeGrPromotionEnabled     : %d\n", pSpsCfg->MeGrPromotionEnabled));
  DEBUG((DebugLevel, " BreakRtcEnabled          : %d\n", pSpsCfg->BreakRtcEnabled));
#endif // ME_TESTMENU_FLAG
DEBUG_CODE_END();
}


/**

  Dump ME SPS Policy function called from Policy

  @param[in] DebugLevel - Specifies used debug level for dump output.
  @param[in] WhatToDump - Specifies what part of policy has to be dumped.

**/
VOID
MePolicyDump (
  UINTN                   DebugLevel,
  ME_POLICY_DUMP_TYPE     WhatToDump
  )
{
  EFI_STATUS      Status;

  if (DebugLevel == 0)
  {
    DebugLevel = DEBUG_INFO;
  }
  if (WhatToDump & ME_POLICY_DUMP_ME)
  {
    ME_POLICY_PPI *MePolicyPpi;
    
    DEBUG((DebugLevel, "\n---------------------- MePolicyPpi Dump Begin -----------------\n"));
    Status = PeiServicesLocatePpi (
             &gMePolicyPpiGuid,
             0,
             NULL,
             (VOID **) &MePolicyPpi
             );
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME Policy PPI not found (%r)\n", Status));
    }
    else
    {
      MePrintPolicyPpi(MePolicyPpi);
    }
    DEBUG((DebugLevel, "\n---------------------- MePolicyPpi Dump End -------------------\n"));
  }

#if AMT_SUPPORT
  if (WhatToDump & ME_POLICY_DUMP_AMT)
  {
    AMT_POLICY_PPI *AmtPolicyPpi;
    
    DEBUG((DebugLevel, "\n---------------------- AmtPolicyPpi Dump Begin ----------------\n"));
    Status = PeiServicesLocatePpi(
                    &gAmtPolicyPpiGuid,
                    0,
                    NULL,
                    &AmtPolicyPpi
                    );
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: AMT Policy PPI not found (%r)\n", Status));
    }
    else
    {
      AmtPrintPolicyPpi(AmtPolicyPpi);
      DEBUG((DebugLevel, " WatchDogOs : 0x%x\n", AmtPolicyPpi->WatchDogOs));
    }
    DEBUG((DebugLevel, "\n---------------------- AmtPolicyPpi Dump End ------------------\n"));
  }
#endif // AMT_SUPPORT

#if SPS_SUPPORT
  if (WhatToDump & ME_POLICY_DUMP_SPS)
  {
    SPS_POLICY_PPI *SpsPolicyPpi;
    
    DEBUG((DebugLevel, "\n---------------------- SpsPolicyPpi Dump Begin ----------------\n"));
    Status = PeiServicesLocatePpi(&gSpsPolicyPpiGuid, 0, NULL, &SpsPolicyPpi);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: SPS policy PPI not found (%r)\n", Status));
    }
    else
    {
      DEBUG((DebugLevel, "SPS Policy PPI Revision   : %d\n", SpsPolicyPpi->Revision));
      MeSpsPrintConfig(DebugLevel, &SpsPolicyPpi->SpsConfig);
    }
    DEBUG((DebugLevel, "\n---------------------- SpsPolicyPpi Dump End ------------------\n"));
  }
#endif // SPS_SUPPORT
}  // MePolicyDump()

