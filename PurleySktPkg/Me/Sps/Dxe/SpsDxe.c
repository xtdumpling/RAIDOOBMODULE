/**

Copyright (c) 2006 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file SpsDxe.c

  This driver implements the DXE phase of SPS support as defined in
  SPS ME-BIOS Interface Specification.
  
**/
#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/GlobalVariable.h>

#include <Guid/HobList.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/SpsInfoHobGuid.h>

#include <Protocol/MpService.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/MeTypeLib.h>
#include <Protocol/MeSpsPolicyProtocol.h>

#include <Protocol/CpuIo.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/PciIo.h>

#include <Sps.h>
#include <HeciRegs.h>
#include <MeAccess.h>
#include "SpsDxe.h"

#define DEFAULT_PCH_TEMP_REPORTING_INTERVAL     250
#define DEFAULT_PCH_TEMP_MAX_LOW_POWER_INTERVAL 1000

/******************************************************************************
 * Function prototypes
 */
EFI_STATUS SpsSendPchTempReportingCfg(VOID);


/******************************************************************************
 * Variables
 */
EFI_EVENT mSpsEndOfDxeEvent = NULL;
EFI_EVENT mSpsReadyToBootEvent = NULL;
SPS_DXE_CONTEXT *mpSpsContext = NULL;


/******************************************************************************
 * Functions
 */
/******************************************************************************
 * @brief Send PCH temperature reporting configuration to ME.
 *
 *   Note that this functionality is limited to BDX-DE only.
 *
 *  @retval EFI status 
 **/
EFI_STATUS 
SpsSendPchTempReportingCfg(VOID)
{
  EFI_STATUS         Status;
  UINT32             RspLen;
  union
  {
    HECI_MSG_SET_PCH_TEMP_REPORTING_CFG_REQ Req;
    HECI_MSG_SET_PCH_TEMP_REPORTING_CFG_RSP Rsp;
  } HeciMsg;

  HECI_PROTOCOL *pHeci;

  if (MeTypeIsSpsInRecovery())
  {
    DEBUG((DEBUG_WARN, "[SPS] WARNING: Using PCH temperature from ME is enabled, but ME is not "
                       "operational!"));
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
    return Status;
  }

  SetMem(&HeciMsg, sizeof(HeciMsg), 0);
  HeciMsg.Req.Command = SPS_CMD_SET_PCH_TEMP_REPORTING_CFG_REQ;
  HeciMsg.Req.PchTempReportingInterval = DEFAULT_PCH_TEMP_REPORTING_INTERVAL;
  HeciMsg.Req.PchTempMaximumLowPowerInterval = DEFAULT_PCH_TEMP_MAX_LOW_POWER_INTERVAL;

  DEBUG((DEBUG_INFO, "[SPS] Sending PCH temperature reporting configuration to ME\n"));
  DEBUG((DEBUG_INFO, "[SPS]   PCH Temperature Reporting Interval: 0x%04x\n", HeciMsg.Req.PchTempReportingInterval));
  DEBUG((DEBUG_INFO, "[SPS]   PCH Temperature Maximum Low Power Interval: 0x%04x\n", HeciMsg.Req.PchTempMaximumLowPowerInterval));

  RspLen = sizeof(HeciMsg);
  Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32 *)&HeciMsg.Req, sizeof(HeciMsg.Req), &RspLen,
                           SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_SPS);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send PCH temperature reporting configuration to ME (%r)\n", Status));
  }
  else if (RspLen != sizeof(HeciMsg.Rsp) || HeciMsg.Rsp.Result != 0)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: PCH temperature reporting configuration NACK (result: %d)\n", HeciMsg.Rsp.Result));
  }
  return Status;
}


/**
    TBD.
    @return SPS_INFO_HOB is returned, or NULL if the HOB not found.
**/
SPS_INFO_HOB*
SpsGetHob(VOID)
{
  EFI_HOB_GUID_TYPE     *pGuidHob;
  
  pGuidHob = GetFirstGuidHob(&gSpsInfoHobGuid);
  if (pGuidHob == NULL)
  {
    return NULL;
  }
  return GET_GUID_HOB_DATA(pGuidHob);
} // SpsGetHob()


/**
    SPS DXE driver entry point.
    @param ImageHandle - handle to SPS DXE image
    @param pSystemTable - pointer to system table
    @retval Void.
**/
EFI_STATUS
EFIAPI
SpsDxeEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE    *pSystemTable
  )
{
  EFI_STATUS              Status;
  SPS_INFO_HOB           *pSpsHob;
  SPS_POLICY_PROTOCOL    *pSpsPolicy = NULL;
  
  DEBUG((DEBUG_INFO, "[SPS] DXE PHASE \n"));
  
  pSpsHob = SpsGetHob();
  if (pSpsHob == NULL)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot find SPS info HOB\n"));
    return EFI_UNSUPPORTED;
  }
  if (pSpsHob->WorkFlow != ME_FUNCTIONAL)
  {
    DEBUG((DEBUG_WARN, "[SPS] WARNING: ME is not functional, driver will be unloaded\n"));
    return EFI_UNSUPPORTED;
  }
  DEBUG((DEBUG_INFO, "[SPS] HOB: flow %d, feature set 0x%08X, feature set 2 0x%08X, pwr opt boot %d, cores2disable %d, pwr msmt %d, hw change %d\n",
        pSpsHob->WorkFlow, pSpsHob->FeatureSet.Data.Set1, pSpsHob->FeatureSet.Data.Set2, pSpsHob->PwrOptBoot, pSpsHob->Cores2Disable,
        pSpsHob->NmPowerMsmtSupport, pSpsHob->NmHwChangeStatus));
  
  Status = gBS->LocateProtocol(&gSpsPolicyProtocolGuid, NULL, &pSpsPolicy);
  if (pSpsPolicy == NULL)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot get SPS configuration policy (%r)\n", Status));
    return Status;
  }
  
  Status = gBS->AllocatePool(EfiBootServicesData, sizeof(*mpSpsContext), (VOID**)&mpSpsContext);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Memory allocation failed (%r). Driver will be unloaded.\n", Status));
    return Status;
  }
  SetMem(mpSpsContext, sizeof(*mpSpsContext), 0);
  mpSpsContext->pSpsHob = pSpsHob;
  mpSpsContext->pSpsPolicy = pSpsPolicy;

  if (mpSpsContext->pSpsHob->FeatureSet.Bits.SocThermalReporting)
  {
    SpsSendPchTempReportingCfg();
  }

#if ME_TESTMENU_FLAG
  //
  // This is test option which can break RTC configuration so that ME can detect it and handle.
  //
#define PCAT_RTC_ADDRESS_REGISTER 0x70
#define PCAT_RTC_DATA_REGISTER    0x71
#define RTC_ADDRESS_REGISTER_A    10
#define RTC_BAD_VALUE             54
  if (pSpsPolicy->SpsConfig.BreakRtcEnabled)
  {
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (RTC_ADDRESS_REGISTER_A | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
    IoWrite8 (PCAT_RTC_DATA_REGISTER, RTC_BAD_VALUE);
  }
#endif

  if (GetBootModeHob() != BOOT_ON_S4_RESUME &&
      (pSpsPolicy->SpsConfig.NmPwrOptBoot != pSpsHob->PwrOptBoot ||
       pSpsPolicy->SpsConfig.NmCores2Disable != pSpsHob->Cores2Disable))
  {
#if ME_TESTMENU_FLAG
    if (!pSpsPolicy->SpsConfig.NmPwrOptBootOverride)
#endif
    {
      DEBUG((DEBUG_INFO, "[SPS] Updating NM boot mode %d->%d\n", pSpsPolicy->SpsConfig.NmPwrOptBoot, pSpsHob->PwrOptBoot));
      pSpsPolicy->SpsConfig.NmPwrOptBoot = pSpsHob->PwrOptBoot;
      Status = !EFI_SUCCESS;
    }
#if ME_TESTMENU_FLAG
    if (!pSpsPolicy->SpsConfig.NmCores2DisableOverride)
#endif
    {
      DEBUG((DEBUG_INFO, "[SPS] Updating NM cores to disable %d -> %d\n", pSpsPolicy->SpsConfig.NmCores2Disable, pSpsHob->Cores2Disable));
      pSpsPolicy->SpsConfig.NmCores2Disable = pSpsHob->Cores2Disable;
      Status = !EFI_SUCCESS;
    }
    if (Status != EFI_SUCCESS)
    {
      if (pSpsPolicy->SaveSpsPolicyToSetupConfiguration != NULL)
      {
        Status = pSpsPolicy->SaveSpsPolicyToSetupConfiguration(pSpsPolicy);
      }
      else
      {
        Status = EFI_INVALID_PARAMETER;
      }
      ASSERT_EFI_ERROR(Status);
    }
  }
  //
  // Send Host Configuration Info even for SiEn. This is recommended in SPS ME-BIOS Spec.
  //
  SpsNmSendHostCfgInfo(mpSpsContext);
  
  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, SpsEndOfDxeCallback,
                              mpSpsContext, &gEfiEndOfDxeEventGroupGuid, &mSpsEndOfDxeEvent);
  ASSERT_EFI_ERROR (Status);
  
  Status = EfiCreateEventReadyToBootEx(TPL_CALLBACK, SpsReadyToBootCallback,
                                       mpSpsContext, &mSpsReadyToBootEvent);
  ASSERT_EFI_ERROR(Status);
  
  return EFI_SUCCESS;
} // SpsDxeEntryPoint()


