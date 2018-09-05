/**

Copyright (c) 2010 - 2017, Intel Corporation.

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


  @file CpuInfoData.c

   This implements filling out the HECI Message responsible of passing 
   CPU Info data. 

**/
#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/SocketPowermanagementVariable.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include <Protocol/MpService.h>
#include <Library/CpuConfigLib.h>
#include <CpuPpmIncludes.h>

#include <Guid/SpsInfoHobGuid.h>
#include <Protocol/HeciProtocol.h>
#include <Sps.h>
#include <Protocol/MeSpsPolicyProtocol.h>
#include <Library/MeTypeLib.h>

#include "SpsDxe.h"


/*****************************************************************************
 * Local definitions
 *****************************************************************************/
#define CPU_TSTATES_NUM_STD    8
#define CPU_TSTATES_NUM_FG    15  // fine grained T-states


/*****************************************************************************
 @brief
  Gather and send to ME host configuration data needed by Node Manager

  @param[in] pSpsHob    Info from SPS PEI driver
  @param[in] MeNmConfig BIOS setup configuration from ME Policy

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_UNSUPPORTED         SPS in recovery
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS 
SpsNmSendHostCfgInfo(
  SPS_DXE_CONTEXT       *pSpsContext)
{
  EFI_STATUS Status;
  UINT32     HeciMsgLen;
  UINT32     RspLen;
  UINT32     Reg;
  UINT32     Index;
  UINT32     ProcMask;
  UINT32     tCoreMask[MAX_SOCKET];
  union
  {
    MKHI_MSG_NM_HOST_CFG_REQ   NMHostCfgReq;
    MKHI_MSG_NM_HOST_CFG_RSP   NMHostCfgRsp;
  } HeciMsg;
  HECI_PROTOCOL               *pHeci;

  BOOLEAN                      HwpCapable = TRUE;
  CAPID5_PCU_FUN3_STRUCT       CsrCapid5;
  EFI_CPU_CSR_ACCESS_PROTOCOL *pCpuCsrAccess;
  CPU_COLLECTED_DATA          *pCpuData;

  UINTN                        VariableSize = 0;
  SOCKET_POWERMANAGEMENT_CONFIGURATION SocketPowermanagmentConfiguration;
  SOCKET_PROCESSORCORE_CONFIGURATION   SocketProcessorcoreConfig;

  if (MeTypeIsSpsInRecovery ()) {
    DEBUG ((DEBUG_WARN, "[SPS] WARNING: SPS in recovery. Do not send Host Configuration\n"));
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
    return Status;
  }
  SetMem(&HeciMsg, sizeof(HeciMsg), 0);
  
  HeciMsg.NMHostCfgReq.Mkhi.Fields.GroupId = MKHI_GRP_NM;
  HeciMsg.NMHostCfgReq.Mkhi.Fields.Command = NM_CMD_HOSTCFG;

  VariableSize = sizeof(SOCKET_POWERMANAGEMENT_CONFIGURATION);
  Status = gRT->GetVariable (
                  SOCKET_POWERMANAGEMENT_CONFIGURATION_NAME,
                  &gEfiSocketPowermanagementVarGuid,
                  NULL,
                  &VariableSize,
                  &SocketPowermanagmentConfiguration
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read socket power management configuration (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  VariableSize = sizeof(SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                  SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                  &gEfiSocketProcessorCoreVarGuid,
                  NULL,
                  &VariableSize,
                  &SocketProcessorcoreConfig
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read socket processor core configuration (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  HeciMsg.NMHostCfgReq.Capabilities.Bits.TurboEn = SocketPowermanagmentConfiguration.TurboMode;

#if ME_TESTMENU_FLAG
  if (pSpsContext->pSpsPolicy->SpsConfig.NmPowerMsmtOverride)
  {
    HeciMsg.NMHostCfgReq.Capabilities.Bits.PowerMsmt = (UINT16)pSpsContext->pSpsPolicy->SpsConfig.NmPowerMsmtSupport;
  }
  else
#endif
  {
    HeciMsg.NMHostCfgReq.Capabilities.Bits.PowerMsmt = (UINT16)pSpsContext->pSpsHob->NmPowerMsmtSupport;
  }

#if ME_TESTMENU_FLAG
  if (pSpsContext->pSpsPolicy->SpsConfig.NmHwChangeOverride)
  {
    HeciMsg.NMHostCfgReq.Capabilities.Bits.HwChange = (UINT16)pSpsContext->pSpsPolicy->SpsConfig.NmHwChangeStatus;
  }
  else
#endif
  {
    HeciMsg.NMHostCfgReq.Capabilities.Bits.HwChange = (UINT16)pSpsContext->pSpsHob->NmHwChangeStatus;
  }
  
  HeciMsg.NMHostCfgReq.Capabilities.Bits.MsgVer = MKHI_MSG_NM_HOST_CFG_VER;
  //
  // ME wants PLATFORM_INFO MSRs sent raw.
  //
  HeciMsg.NMHostCfgReq.PlatformInfo = AsmReadMsr64(MSR_PLATFORM_INFO);
  HeciMsg.NMHostCfgReq.Altitude = (INT16)pSpsContext->pSpsPolicy->SpsConfig.SpsAltitude;
  
  Status = gBS->LocateProtocol(&gEfiCpuCsrAccessGuid, NULL, &pCpuCsrAccess);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate processor CSR access protocol (%r)\n", Status));
    pCpuCsrAccess = NULL;
  }
  ProcMask = 0;
  SetMem(&tCoreMask[0], sizeof(tCoreMask), 0);
  for (pCpuData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[0];
       pCpuData < &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[mCpuConfigLibConfigContextBuffer->NumberOfProcessors];
       pCpuData++)
  {
    ASSERT(pCpuData->ProcessorLocation.Package < sizeof(tCoreMask)/sizeof(tCoreMask[0]));
    if (!(ProcMask & (1 << pCpuData->ProcessorLocation.Package)))
    {
      ProcMask |= 1 << pCpuData->ProcessorLocation.Package;
      HeciMsg.NMHostCfgReq.ProcNumber++;
      if (pCpuCsrAccess != NULL)
      {
        CsrCapid5.Data = pCpuCsrAccess->ReadCpuCsr((UINT8)pCpuData->ProcessorLocation.Package, 0, CAPID5_PCU_FUN3_REG);
        HwpCapable &= CsrCapid5.Bits.hwp_en;
      }
    }
    if (!(tCoreMask[pCpuData->ProcessorLocation.Package] & (1 << pCpuData->ProcessorLocation.Core)))
    {
      tCoreMask[pCpuData->ProcessorLocation.Package] |= 1 << pCpuData->ProcessorLocation.Core;
      HeciMsg.NMHostCfgReq.ProcCoresEnabled++;
      HeciMsg.NMHostCfgReq.ProcThreadsEnabled++;
    }
    else
    { //
      // For some reason hyper-threads are listed in this library regardless whether
      // HT is enabled or not. Let's filter them using configuration data.
      //
      if (!SocketProcessorcoreConfig.ProcessorHyperThreadingDisable)
      {
        HeciMsg.NMHostCfgReq.ProcThreadsEnabled++;
      }
    }
    if (pCpuData->PackageBsp)
    {
      HeciMsg.NMHostCfgReq.PStatesNumber = (UINT8)pCpuData->CpuMiscData.NumberOfPStates;
      ASSERT(pCpuData->CpuMiscData.NumberOfPStates <= sizeof(HeciMsg.NMHostCfgReq.PStatesRatio)/sizeof(HeciMsg.NMHostCfgReq.PStatesRatio[0]));
      if (pCpuData->CpuMiscData.FvidTable != NULL)
      {
        for (Index = 0; Index < pCpuData->CpuMiscData.NumberOfPStates; Index++)
        {
          HeciMsg.NMHostCfgReq.PStatesRatio[Index] = (UINT8)pCpuData->CpuMiscData.FvidTable[Index].Ratio;
        }
      }
      if (SocketPowermanagmentConfiguration.TStateEnable)
      {
        HeciMsg.NMHostCfgReq.TStatesNumber = CPU_TSTATES_NUM_STD;
        if (pCpuData->CpuidData.CpuIdLeaf != NULL &&
            pCpuData->CpuidData.NumberOfBasicCpuidLeafs > EFI_CPUID_POWER_MANAGEMENT_PARAMS &&
            (pCpuData->CpuidData.CpuIdLeaf[EFI_CPUID_POWER_MANAGEMENT_PARAMS].RegEax & EFI_FINE_GRAINED_CLOCK_MODULATION))
        {
          HeciMsg.NMHostCfgReq.TStatesNumber = CPU_TSTATES_NUM_FG;
        }
      }
    }
  } // for (pProcData...)
  //
  // Hardware Power Management may be configured in one of 4 modes:
  // (0) Disabled    - legacy ACPI P-states/T-states interface used
  // (1) Native      - new _CPC and legacy objects are exposed to OS, OS decides what will be used
  // (2) Out of Band - HWPM enforced, no PM interface exposed to OS in ACPI
  // (3) Native w/o Legacy - new _CPC object exposed in ACPI without legacy objects
  //
  if (HwpCapable)
  {
    if (SocketPowermanagmentConfiguration.ProcessorHWPMEnable > 1) // if HWP OOB or Native w/o Legacy
    {
      HeciMsg.NMHostCfgReq.PStatesNumber = 0;
      HeciMsg.NMHostCfgReq.TStatesNumber = 0;
    }
  }
  //
  // Number of cores implemented can be found in CPU ID function 11 - core topology.
  // Core topology subfunction 1 gives the number of possible logical threads, subfunction 0
  // the number of threads per core. To calculate cores divide threads by threads per core.
  //
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, NULL, &Reg, NULL, NULL);
  HeciMsg.NMHostCfgReq.ProcCoresNumber = (UINT16)Reg;
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 0, NULL, &Reg, NULL, NULL);
  HeciMsg.NMHostCfgReq.ProcCoresNumber /= (UINT16)Reg;
  HeciMsg.NMHostCfgReq.ProcCoresNumber *= HeciMsg.NMHostCfgReq.ProcNumber;
  
  DEBUG((DEBUG_INFO, "[SPS] Sending Host Configuration Info to ME\n"));
  DEBUG((DEBUG_INFO, "[SPS]   Capabilities : 0x%04x\n", HeciMsg.NMHostCfgReq.Capabilities.Uint16));
  DEBUG((DEBUG_INFO, "[SPS]   P/T-states:    %d/%d\n",
                     HeciMsg.NMHostCfgReq.PStatesNumber, HeciMsg.NMHostCfgReq.TStatesNumber));
  DEBUG((DEBUG_INFO, "[SPS]   Min/max power: %d/%d\n",
                     HeciMsg.NMHostCfgReq.MinPower, HeciMsg.NMHostCfgReq.MaxPower));
  DEBUG((DEBUG_INFO, "[SPS]   Processor packages: %d\n", HeciMsg.NMHostCfgReq.ProcNumber));
  DEBUG((DEBUG_INFO, "[SPS]   Processor cores:    %d\n", HeciMsg.NMHostCfgReq.ProcCoresNumber));
  DEBUG((DEBUG_INFO, "[SPS]   Processor cores enabled:   %d\n", HeciMsg.NMHostCfgReq.ProcCoresEnabled));
  DEBUG((DEBUG_INFO, "[SPS]   processor threads enabled: %d\n", HeciMsg.NMHostCfgReq.ProcThreadsEnabled));
  DEBUG((DEBUG_INFO, "[SPS]   Platform info:     0x%08X%08X\n",
                     (UINT32)(HeciMsg.NMHostCfgReq.PlatformInfo >> 32), (UINT32)HeciMsg.NMHostCfgReq.PlatformInfo));
  DEBUG((DEBUG_INFO, "[SPS]   Altitude: %d\n", HeciMsg.NMHostCfgReq.Altitude));

  HeciMsgLen = sizeof(HeciMsg.NMHostCfgReq);
  if ((HeciMsg.NMHostCfgReq.PStatesNumber > 0))
  {
    DEBUG((DEBUG_INFO, "[SPS]   PStatesRatio[%d]: ", HeciMsg.NMHostCfgReq.PStatesNumber));
    for (Index = 0; Index < HeciMsg.NMHostCfgReq.PStatesNumber; Index++)
    {
      DEBUG((DEBUG_INFO, " %02X", HeciMsg.NMHostCfgReq.PStatesRatio[Index]));
    }
    DEBUG((DEBUG_INFO, "\n"));
  }
  RspLen = sizeof(HeciMsg.NMHostCfgRsp);
  Status = pHeci->SendwACK(HECI1_DEVICE, &HeciMsg.NMHostCfgReq.Mkhi.Data, HeciMsgLen, &RspLen,
                           SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send host configuration info to NM (%r)\n", Status));
  }
  else if (HeciMsg.NMHostCfgRsp.Mkhi.Fields.IsResponse != 1 ||
           HeciMsg.NMHostCfgRsp.Mkhi.Fields.GroupId != MKHI_GRP_NM ||
           HeciMsg.NMHostCfgRsp.Mkhi.Fields.Command != NM_CMD_HOSTCFG)
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Wrong response format for host configuration info to NM (MKHI: 0x%x)\n",
           HeciMsg.NMHostCfgRsp.Mkhi.Data));
  }
  else
  {
    switch (HeciMsg.NMHostCfgRsp.Status) 
    {
      case STATUS_NM_HOST_CFG_RSP_OK:
        DEBUG((DEBUG_INFO, "[SPS] Configuration info to NM was sent.\n"));
        break;
      case STATUS_NM_HOST_CFG_RSP_WRONG_MSG_FORMAT:
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Host configuration info response has wrong format\n"));
        break;
      case STATUS_NM_HOST_CFG_RSP_ALTITUDE_MISSING:
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Altitude was missing in host configuration info\n"));
        break;
      case STATUS_NM_HOST_CFG_RSP_PSTATESRATIO_MISSING:
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: PStatesRatio was missing in host configuration info\n"));
        break;
      default:
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Unknown host configuration info status (%d)\n", HeciMsg.NMHostCfgRsp.Status));
        break;
    } 
  }
  return Status;
} // GatherCPUInfoData()

