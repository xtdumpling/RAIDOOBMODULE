/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2012 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeSpsPolicyDebugDumpDxe.c

@brief:
  Dump whole SPS_POLICY_PROTOCOL.
**/
#include "Library/DebugLib.h"
#include "Library/MeTypeLib.h"
#include <Protocol/MePolicy.h>
#include "Protocol/MeSpsPolicyProtocol.h"
#include <Library/DxeMePolicyLib.h>
#include <Library/UefiBootServicesTableLib.h>
#if AMT_SUPPORT
#include "Protocol/AmtPolicy.h"
#include "Library/DxeAmtPolicyLib.h"
#endif // AMT_SUPPORT


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
  DEBUG((DebugLevel, " MeHmrfpoLockEnabled      : %d\n", pSpsCfg->MeHmrfpoLockEnabled));
  DEBUG((DebugLevel, " MeHmrfpoEnableEnabled    : %d\n", pSpsCfg->MeHmrfpoEnableEnabled));
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

  Dump DXE ME Platform Policy

  @param[in] DebugLevel - Specifies used debug level for dump output.
  @param[in] WhatToDump - Specifies what part of policy has to be dumped.

**/
VOID
MePolicyDump (
  UINTN                  DebugLevel,
  ME_POLICY_DUMP_TYPE    WhatToDump
  )
{
  EFI_STATUS Status;

  if (DebugLevel == 0) {
    DebugLevel = DEBUG_INFO;
  }

  DEBUG ((DebugLevel, "\n------------------------ Me/Amt/SpsPolicy Dump Begin (DXE) -------------------\n"));
  if (WhatToDump & ME_POLICY_DUMP_ME)
  {
    ME_POLICY_PROTOCOL *MePolicy;
    
    Status = gBS->LocateProtocol (&gDxeMePolicyGuid, NULL, &MePolicy);
    DEBUG((DebugLevel, "\n------------------------ ME Policy Dump Begin -------------------\n"));
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: SPS policy not found (%r)\n", Status));
    }
    else
    {
      MePrintPolicyProtocol(MePolicy);
    }
    DEBUG((DebugLevel, "\n------------------------ ME Policy Dump End ---------------------\n"));
  }
  
#if SPS_SUPPORT
  if (WhatToDump & ME_POLICY_DUMP_SPS)
  {
    SPS_POLICY_PROTOCOL *SpsPolicy;
    
    Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
    DEBUG ((DebugLevel, "\n------------------------ SpsPolicy Dump Begin ------------------\n"));
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: SPS policy not found (%r)\n", Status));
    }
    else
    {
      DEBUG((DebugLevel, "SPS Policy Revision       : %d\n", SpsPolicy->Revision));
      DEBUG((DebugLevel, " ME Type                  : %d\n", SpsPolicy->MeType));
      
      MeSpsPrintConfig(DebugLevel, &SpsPolicy->SpsConfig);
    }
    DEBUG((DebugLevel, "\n---------------------- SpsPolicyPpi Dump End --------------------\n"));
  }
#endif // SPS_SUPPORT

#if AMT_SUPPORT
  if ((WhatToDump & ME_POLICY_DUMP_AMT) && (MeTypeIsAmt()))
  {
    SPS_POLICY_PROTOCOL  *SpsPolicy;
    AMT_POLICY_PROTOCOL  *DxeAmtPolicy;

    Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
    DEBUG((DebugLevel, "\n------------------------ AmtPolicy Dump Begin -------------------\n"));
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: SPS policy not found (%r)\n", Status));
    }
    else
    {
      DEBUG((DebugLevel, "MeAmtConfig ---\n"));
      DEBUG((DebugLevel, " FviSmbiosType             : 0x%X\n", SpsPolicy->MeAmtConfig.FviSmbiosType));
      DEBUG((DebugLevel, " PreviousMeStateControl    : 0x%X\n", SpsPolicy->MeAmtConfig.PreviousMeStateControl));
      DEBUG((DebugLevel, " PreviousLocalFwUpdEnabled : 0x%X\n", SpsPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled));
    }
    Status = gBS->LocateProtocol(&gDxeAmtPolicyGuid, NULL, (VOID **) &DxeAmtPolicy);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: AMT policy not found (%r)\n", Status));
    }
    else
    {
      AmtPrintPolicyProtocol(DxeAmtPolicy);
    }
    DEBUG((DebugLevel, "\n------------------------ AmtPolicy Dump End ---------------------\n"));
  }
#endif // AMT_SUPPORT
  DEBUG ((DebugLevel, "\n------------------------ Me/Amt/SpsPolicy Dump End (DXE) -------------------\n"));
}

