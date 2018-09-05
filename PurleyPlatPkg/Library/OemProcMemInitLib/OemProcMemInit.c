//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.06
//      Bug Fixed:  Add tRWSR w/a setup option for Samsung DIMMs.
//      Reason:     Improve memory RX margin
//      Auditor:    Stephen Chen
//      Date:       Aug/18/2017
//
//  Rev. 1.05
//   Bug Fixed:  Add tCCD_L Relaxation setup option for specific DIMMs.
//   Reason:     Requested by Micron, code reference from Jian.
//   Auditor:    Jacker Yeh
//   Date:       Mar/17/2017
//
//  Rev. 1.04
//   Bug Fixed:  Remove redundant code 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       NOV/11/2016
//
//  Rev. 1.03
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.02
//      Bug Fixed:  Fixed SMC no memory error beep can't work problem.
//      Reason:     Move function to OemPlatformFatalError.
//      Auditor:    Jimmy Chiu
//      Date:       Aug/17/2016
//
//  Rev. 1.01
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************


/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file OemProcMemInit.c

--*/

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif //_MSC_VER

#include "CoreApi.h"
#include "ChipApi.h"
#ifndef MINIBIOS_BUILD
#include "EdkProcMemInit.h"
#include <Library/OemProcMemInit.h>
#include <Library/PeiServicesLib.h>
#define FILE_NAME "OemProcMemInit.c"
#include <Platform.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Guid/PprVariable.h>
#include <Guid/VlsVariableGuid.h>
// APTIOV_SERVER_OVERRIDE_RC_START
// USE AMI TCG module
#if 0
#ifdef IA32
#include <Ppi/TpmInitialized.h>
#include <Library/Tpm12CommandLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Guid/TpmInstance.h>
#endif
#endif 

#include <Token.h>  // Use SDL token for PM base address.
// APTIOV_SERVER_OVERRIDE_RC_END
#include <Library/SetupLib.h>

#include <Library/PlatformHooksLib.h>
#if ME_SUPPORT_FLAG
#include <HeciRegs.h>
#include <MeAccess.h>
#include <MeUma.h>
#include <Library/MeTypeLib.h>
#if SPS_SUPPORT
#include <Sps.h>
#include <Ppi/HeciPpi.h>
#include <Ppi/Stall.h>
#endif // SPS_SUPPORT
#endif // ME_SUPPORT_FLAG

#ifdef IE_SUPPORT
#include <Ppi/IeHeci.h>
#endif //IE_SUPPORT

//To log resets:
#include <Ppi/PchReset.h>

#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformStatusCodes.h>
#include <Library/BootGuardLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Boot Guard changes override. Need to removed once proper changes are made.
//#include "PeiBootGuardEventLogLib.h"
// APTIOV_SERVER_OVERRIDE_RC_END : Boot Guard changes override. Need to removed once proper changes are made.

#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>

#include <Library/PchCycleDecodingLib.h>
#include <Library/BeepLib.h>
#include <Library/TimerLib.h>

// APTIOV_SERVER_OVERRIDE_RC_START
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END

#ifdef PC_HOOK
#include <Library/FastVideoPrintLib.h>
#include <Ppi/VideoPrint.h>
#include "InternalErrorString.h"
#endif

#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
#include "Ppi/IPMITransportPPi.h"
#include <SspTokens.h>
#include <SmcLibCommon.h>
#endif //SMCPKG_SUPPORT++

#pragma pack(1)
typedef struct {
  UINT8   TpmPresent;
  BOOLEAN HideTpm;
  UINT8   TpmCurrentState;
  BOOLEAN MorState;
} TCG_SETUP;
#pragma pack()

#else //MINIBIOS_BUILD
#include "OemProcMemInit.h"
#endif //MINIBIOS_BUILD

#include "KtiSetupDefinitions.h"

#include "Compression.h"

#ifndef MINIBIOS_BUILD
extern EFI_GUID gEfiMemoryConfigDataGuid;
extern EFI_GUID gEfiOcDataGuid;
EFI_GUID gEfiAfterMrcGuid = EFI_AFTER_MRC_GUID;
EFI_GUID gEfiAfterKtircGuid = EFI_AFTER_QPIRC_GUID;

#define FORCE_CMOS_BAD            BIT20
#define FORCE_CMOS_SHIFT               20
#define KTI_SETUP_OPTIONS         4

#ifdef PC_HOOK
#include "KtiMisc.h"

#define ERR_UNSUPPORTED_TOPOLOGY_CORE        0xE5
#define ERR_UNSUPPORTED_TOPOLOGY_FAMILY      0xE6 // TODO for CPU family mismatch
#endif
//
// APTIOV_SERVER_OVERRIDE_RC_START : Installs PPI to signal the TCG module to skip TPM Startup if
//                          			the TXT policy specified in the FIT (CMOS BITS) is set to enabled.

#define TXT_ACM_STATUS_HI  0xFED300A4
#define AMI_SKIP_TPM_STARTUP_GUID \
    { 0x6ee1b483, 0xa9b8, 0x4eaf, 0x9a, 0xe1, 0x3b, 0x28, 0xc5, 0xcf, 0xf3,\
      0x6b}

static EFI_GUID gAmiSkipTpmStartupGuid = AMI_SKIP_TPM_STARTUP_GUID;

static EFI_PEI_PPI_DESCRIPTOR gPpiList[] = {
    EFI_PEI_PPI_DESCRIPTOR_PPI
    | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmiSkipTpmStartupGuid,
    NULL
};

//
// APTIOV_SERVER_OVERRIDE_RC_END : Installs PPI to signal the TCG module to skip TPM Startup if
//                          		the TXT policy specified in the FIT (CMOS BITS) is set to enabled.
static EFI_PEI_PPI_DESCRIPTOR mAfterKtircPpi[] =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiAfterKtircGuid,
    NULL
};

static EFI_PEI_PPI_DESCRIPTOR mAfterMrcPpi[] =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiAfterMrcGuid,
    NULL
};

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
   &gEfiPeiMemoryDiscoveredPpiGuid,
   PublishHobDataCallbackMemoryDiscoveredPpi
};

/**
  Clear any SMI status or wake status left from boot.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
ClearSmiAndWake (
  VOID
  )
{
  UINT16              ABase;
  UINT16              Pm1Sts = 0;

  PchAcpiBaseGet (&ABase);

  //
  // Clear any SMI or wake state from the boot
  //
  Pm1Sts |= B_PCH_ACPI_PM1_STS_PWRBTN;

  IoWrite16 (ABase + R_PCH_ACPI_PM1_STS,  Pm1Sts);

  //
  // Clear the GPE and PM enable
  //
  IoWrite16 (ABase + R_PCH_ACPI_PM1_EN, 0);
  IoWrite32 (ABase + R_PCH_ACPI_GPE0_EN_127_96, 0);

  return EFI_SUCCESS;
}

//
// Function Implementations
//
void setCPLDClk(UINT8 level)
{
///TODO:Add setting CPLD clock with proper GPIOs for LBG/SPT platforms
}

void setCPLDData(UINT8 level)
{
///TODO:Add setting CPLD clock with proper GPIOs for LBG/SPT platforms
}


void spinloop(UINT32 count)
{
  volatile UINT32 i;
  for (i = 0; i < count; i++);
}

void sendCPLDByte(UINT8 data)
{
  int i;

  for (i=0; i<8; i++) {
    setCPLDClk(0);
    spinloop(50000);

    setCPLDData(((data & 0x80) ? 1 : 0));
    spinloop(50000);

    setCPLDClk(1);
    data = data << 1;
    spinloop(50000);
  }
}



#endif // MINIBIOS_BUILD
/**
  Get the size of ME Segment from the ME FW.

  @param host - pointer to the system host structure

  @retval host->var.common.meRequestedSize is updated
**/
VOID
OemGetMeRequestedSegmentSize (
  PSYSHOST host
  )
{
#ifndef MINIBIOS_BUILD
#if ME_SUPPORT_FLAG
  EFI_STATUS                  Status;
  ME_UMA_PPI                  *MeUma;

  MeUma = NULL;

  //
  // Locate MeUma PPI.
  //
  Status = PeiServicesLocatePpi (&gMeUmaPpiGuid, 0, NULL, &MeUma);

  if (IsSimicsPlatform() == FALSE) {
    ASSERT_EFI_ERROR (Status);
  }

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "UMA: PchUma couldn't be located. Set UMA to 0 and force cold boot.\n"));
    host->var.common.meRequestedSize = 0;
    host->var.mem.previousBootError = 1;
    return;
  }

  if (MeUma->isMeUmaEnabled()) {
    if (host->var.common.bootMode != S3Resume) {
      //
      // ME Stolen Size in MB units
      //
      host->var.common.meRequestedSize = MeUma->MeSendUmaSize (NULL);
      if (host->var.common.meRequestedSize > ME_UMA_SIZE_UPPER_LIMIT) {
        DEBUG ((EFI_D_ERROR, "UMA: ME UMA requested size too large.\n"));
        // Do not allow for too large UMA area allocation, but for size 0,
        // indicate error to ME FW in DID message (InitStat = 0x01)
        host->var.common.meRequestedSize = 0;
      }
      host->nvram.common.meRequestedSizeNv = host->var.common.meRequestedSize;
    } else {
      host->var.common.meRequestedSize = host->nvram.common.meRequestedSizeNv;
    }
  }
  DEBUG ((EFI_D_INFO, "ME UMA size = %d MBytes\n", host->var.common.meRequestedSize));
#endif // ME_SUPPORT_FLAG
#endif // MINIBIOS_BUILD
}
#ifndef MINIBIOS_BUILD
/**
**/
VOID
OemGetPlatformSetupValueforCommonRc(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup             *setup;
  UINT8                       Index;
#ifdef   SERIAL_DBG_MSG
  UINT8                       SerialDebugMsgLevelSetting;
  EFI_BOOT_MODE               BootMode;
#endif // SERIAL_DBG_MSG
//
// EPSDIP_SETUP_MMIO_START
//
#ifdef PC_HOOK
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  UINTN                       VariableSize;
  UINT16                      AutoMmioHSizeValue;
#endif
//
// EPSDIP_SETUP_MMIO_END
//
#if defined(SERIAL_DBG_MSG) || defined(PC_HOOK)
  EFI_STATUS                  Status;
  EFI_PEI_SERVICES            **PeiServices;
#endif
  setup = (struct sysSetup *)(&host->setup);

#if defined(SERIAL_DBG_MSG) || defined(PC_HOOK)
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
#endif

  //
  // Init input parameters here according to setup options
  //
#ifdef   SERIAL_DBG_MSG
  switch(SetupData->SystemConfig.serialDebugMsgLvl) {
  case 0:
    // Disabled
    SerialDebugMsgLevelSetting = 0;
    break;
  case 1:
    // Minimum
    SerialDebugMsgLevelSetting = SDBG_MIN;
    break;
  case 2:
    // Normal
    SerialDebugMsgLevelSetting = SDBG_MAX;
    break;
  case 3:
    // Maximum
    SerialDebugMsgLevelSetting = SDBG_MINMAX;
    break;
  case 4:
    // Auto
    if (PcdGetBool(PcdDfxAdvDebugJumper)) {
      // Normal
      SerialDebugMsgLevelSetting = SDBG_MAX;
    } else {
      // Minimum
      SerialDebugMsgLevelSetting = SDBG_MIN;
      setup->mem.memFlows = setup->mem.memFlows & ~(MF_RANK_MARGIN_EN);
    }
    break;
  default:
    // If the default was set to something else in CommonSetup, preserve it for first boot
    SerialDebugMsgLevelSetting = SetupData->SystemConfig.serialDebugMsgLvl;
    break;
  }
  if (PcdGetBool(PcdDfxAdvDebugJumper))  setup->common.debugJumper = 1;
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  ASSERT_EFI_ERROR(Status);

  //
  // 5370428: Diable Serial Debug Message Level option doesn't work for the first boot after LBG part being replaced (Opal City)
  // with default setting, let the debug msg level to default; do not force
  /*if (BootMode == BOOT_WITH_DEFAULT_SETTINGS ){
    SerialDebugMsgLevelSetting = SDBG_MAX;
    SetupData->SystemConfig.serialDebugMsgLvlTrainResults = 0;
    SetupData->SystemConfig.serialDebugTrace = 0;
  } */

  setup->common.serialDebugMsgLvl = SerialDebugMsgLevelSetting;
#ifdef PC_HOOK
  if (SetupData->SocketConfig.MemoryConfig.RankMargin == RMT_EN) {
    host->setup.common.serialDebugMsgLvl = SDBG_MAX;
  } else if (SetupData->SocketConfig.MemoryConfig.RankMargin == RMT_DIS) {
    host->setup.common.serialDebugMsgLvl = 0;
  }
#endif
  setup->common.serialDebugMsgLvl |= SetupData->SystemConfig.serialDebugTrace;
  setup->common.serialDebugMsgLvl |= SetupData->SystemConfig.serialDebugMsgLvlTrainResults;
#endif

  if (SetupData->SocketConfig.IioConfig.DFXEnable) {
    host->var.common.EVMode = 1;
    SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorSmxEnable = 0;
  }

  host->var.common.Pci64BitResourceAllocation = SetupData->SocketConfig.IioConfig.Pci64BitResourceAllocation;

  switch(SetupData->SocketConfig.CommonRcConfig.MmcfgBase){
    case 0:
      setup->common.mmCfgBase = (UINT32)1*1024*1024*1024;
      break;
    case 1:
      setup->common.mmCfgBase = (UINT32)3*512*1024*1024;
      break;
    case 2:
      setup->common.mmCfgBase = (UINT32)7*256*1024*1024;
      break;
    case 3:
      setup->common.mmCfgBase = (UINT32)2*1024*1024*1024;
      break;
    case 4:
      setup->common.mmCfgBase = (UINT32)9*256*1024*1024;
      break;
    case 5:
      setup->common.mmCfgBase = (UINT32)3*1024*1024*1024;
      break;
  }
  switch(SetupData->SocketConfig.CommonRcConfig.MmcfgSize){
    case 0:
      setup->common.mmCfgSize = 64*1024*1024;
      break;
    case 1:
      setup->common.mmCfgSize = 128*1024*1024;
      break;
    case 2:
      setup->common.mmCfgSize = 256*1024*1024;
      break;
    case 3:
      setup->common.mmCfgSize = 512*1024*1024;
      break;
    case 4:
      setup->common.mmCfgSize = (UINT32)1024*1024*1024;
      break;
    case 5:
      setup->common.mmCfgSize = (UINT32)2048*1024*1024;
      break;
  }

  setup->common.mmiolBase = setup->common.mmCfgBase + setup->common.mmCfgSize;
  setup->common.mmiolSize = MMIOL_LIMIT - setup->common.mmiolBase + 1;
  switch(SetupData->SocketConfig.CommonRcConfig.MmiohBase){
    case 1:
      setup->common.mmiohBase = 0x2800;
      break;
    case 2:
      setup->common.mmiohBase = 0x1800;
      break;
    case 3:
      setup->common.mmiohBase = 0x1000;
      break;
    case 4:
      setup->common.mmiohBase = 0x400;
      break;
    case 5:
      setup->common.mmiohBase = 0x100;
      break;
    default:
      setup->common.mmiohBase = MMIOH_BASE;
      break;
  }

  setup->common.mmiohSize = SetupData->SocketConfig.CommonRcConfig.MmiohSize;
#ifdef PC_HOOK
  if(SetupData->SocketConfig.CommonRcConfig.MmiohSize >5){
    //setup->common.mmiohSize = 4;    //Auto, set as 256GB mmiohsize
//
// EPSDIP_SETUP_MMIO_START
//
        //
        //Set the MmiohSize to MMIOH_SIZE, then use the value of Auto option to override it.
        //
        setup->common.mmiohSize = MMIOH_SIZE;

        //
        // Locate Variable PPI
        //
        (**PeiServices).LocatePpi (
                        PeiServices,
                        &gEfiPeiReadOnlyVariable2PpiGuid,
                        0,
                        NULL,
                        &PeiVariable
                        );

        VariableSize = sizeof (UINT16);
        Status = PeiVariable->GetVariable (
                          PeiVariable,
                          L"AutoMmioHSize",
                          &gEfiSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &AutoMmioHSizeValue
                          );

        if (!EFI_ERROR (Status)) {
          setup->common.mmiohSize = AutoMmioHSizeValue;
        }
//
// EPSDIP_SETUP_MMIO_END
//
  }
#endif
  setup->common.lowGap = FOUR_GB_MEM - (setup->common.mmCfgBase >> 26);
  setup->common.highGap = HIGH_GAP;            // High MMIO gap = 256 MB
  //
  // SKX_TODO: This variable needs to be extended to 32bit and updated w/ MMIOH base
  // And MRC code needs to consume it when enabling TOHM
  //
  //setup->common.highGap = (setup->common.mmiohBase >> 28);
  setup->common.maxAddrMem = MAX_MEM_ADDR;     // Supports 46-bit addressing

  if (SetupData->SocketConfig.CommonRcConfig.IsocEn >= 2) {
    //
    // Auto setting
    // SKXTODO: once board definitions are finalized, enable/disable based on platformType; for now disable Isoc if auto
    //
    setup->common.isocEn = 0;
  } else {
    setup->common.isocEn = SetupData->SocketConfig.CommonRcConfig.IsocEn;
  }
  if (SetupData->SocketConfig.MemoryConfig.volMemMode == VOL_MEM_MODE_2LM) {
    DEBUG((EFI_D_INFO, "Isoc is disabled as User has requested to enable 2LM.\n"));
    setup->common.isocEn = 0;
  }

  setup->common.numaEn = SetupData->SocketConfig.CommonRcConfig.NumaEn;

  //
  // Initialize DCA Enable variable and enable if any IIO has it enabled
  //
  setup->common.dcaEn = 0;
  for (Index = 0; Index < MAX_SOCKET; Index++){
    setup->common.dcaEn |= SetupData->SocketConfig.IioConfig.Cb3DcaEn[Index];
  }

#ifdef SERIAL_DBG_MSG
  setup->common.bsdBreakpoint = SetupData->SystemConfig.bsdBreakpoint;
#endif //SERIAL_DBG_MSG
  setup->common.BiosGuardEnabled = SetupData->SystemConfig.BiosGuardEnabled;
  setup->common.DfxAltPostCode = SetupData->SystemConfig.DfxAltPostCode;
  setup->kti.TscSyncEn = SetupData->SocketConfig.CsiConfig.TscSyncEn;

  //
  // TSC SYNC knob AUTO sets to Enable for LR board.
  //
  if ( setup->kti.TscSyncEn == KTI_AUTO){
   if ((host->nvram.common.platformType >= TypeLightningRidgeEXRP) && (host->nvram.common.platformType <= TypeLightningRidgeEX8S2N)) {
     setup->kti.TscSyncEn = KTI_ENABLE;
   } else {
     setup->kti.TscSyncEn = KTI_DISABLE;
   }
  }


}

VOID
OemGetPlatformSetupValueforCpu(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup  *setup;
  UINT8            socket;
#ifdef PC_HOOK
  UINTN            Index;
  UINT8            PlatformCores;
  UINT32           Reg;
  UINT32           CoreDisableMask;
  UINT16           CoreDisableNum;
#endif

  setup = (struct sysSetup *)(&host->setup);

  setup->cpu.debugInterfaceEn = SetupData->SocketConfig.SocketProcessorCoreConfiguration.DebugInterface;   // CPU DebugInterface
  setup->cpu.dcuModeSelect = SetupData->SocketConfig.SocketProcessorCoreConfiguration.DCUModeSelection;    // CPU DCU Mode select
  setup->cpu.flexRatioEn = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorFlexibleRatioOverrideEnable; // FlexRatio enable
  //
  // Verify that the values being set are within a valid range, if not use default.
  //
  if(SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorFlexibleRatio > MAX_PROCESSOR_CORE_RATIO){
    SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorFlexibleRatio = DEFAULT_PROCESSOR_CORE_RATIO; // If out of range set the default
  }
  setup->cpu.PchTraceHubEn = SetupData->SocketConfig.SocketProcessorCoreConfiguration.PchTraceHubEn;  //NPK_STH_ACPIBAR_BASE
  setup->cpu.flexRatioNext = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorFlexibleRatio;        // Common NewRatio for all socket
  setup->cpu.vrsOff = SetupData->SocketConfig.PowerManagementConfig.LOT26UnusedVrPowerDownEnable;
  setup->cpu.smtEnable = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorHyperThreadingDisable ? 0 : 1;
  setup->cpu.vtEnable = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable;
  setup->cpu.ltEnable = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorSmxEnable;
  setup->cpu.ltsxEnable = SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorLtsxEnable;
  setup->cpu.PpinControl = SetupData->SocketConfig.SocketProcessorCoreConfiguration.PpinControl;
  setup->cpu.EnableGv = SetupData->SocketConfig.PowerManagementConfig.ProcessorEistEnable;
  setup->WFRWAEnable = SetupData->SocketConfig.PowerManagementConfig.WFRWAEnable;
  setup->cpu.ConfigTDPLevel = SetupData->SocketConfig.PowerManagementConfig.ConfigTDPLevel;
  setup->cpu.ConfigTDP = SetupData->SocketConfig.PowerManagementConfig.ConfigTDP;
  setup->cpu.UFSDisable = SetupData->SocketConfig.PowerManagementConfig.UFSDisable;
  setup->cpu.AllowMixedPowerOnCpuRatio = SetupData->SocketConfig.SocketProcessorCoreConfiguration.AllowMixedPowerOnCpuRatio;
  setup->cpu.CheckCpuBist = SetupData->SocketConfig.SocketProcessorCoreConfiguration.CheckCpuBist;

  //
  // PMAX Disable OEM hook, set to 1 to disable
  //
  setup->PmaxDisable = 0;

  //
  // 5330679: Requesting BIST Check setup option disabled
  //

#ifdef PC_HOOK
  // Get number of cores to disable and make a CoreDisablingMask of it.
  //
  ZeroMem(&Reg, sizeof(UINT32));
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, NULL, &Reg, NULL, NULL);
  PlatformCores = (UINT8)Reg;
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 0, NULL, &Reg, NULL, NULL);
  PlatformCores /= (UINT8)Reg;

  CoreDisableMask = 0;
  CoreDisableNum  = 0;

  if (SetupData->SocketConfig.SocketProcessorCoreConfiguration.CoreDisableMask[0] == 0) {
    //
    // ActiveCoreNum is 0 means active all cores
    //
    CoreDisableMask = 0;
  } else {
    if (PlatformCores >= (UINT8)(SetupData->SocketConfig.SocketProcessorCoreConfiguration.CoreDisableMask[0])) {
      CoreDisableNum = PlatformCores - (UINT8)(SetupData->SocketConfig.SocketProcessorCoreConfiguration.CoreDisableMask[0]);
      //
      // bit set 1 indicates core masked
      //
      for (Index = 0; Index < CoreDisableNum; Index++) {
        CoreDisableMask |= (1 << Index);
      }
    }
  }
#endif
  for (socket = 0; socket<MAX_SOCKET; socket++) {
#ifdef PC_HOOK
    setup->cpu.CoreDisableMask[socket] = CoreDisableMask;
#else
    setup->cpu.CoreDisableMask[socket] = SetupData->SocketConfig.SocketProcessorCoreConfiguration.CoreDisableMask[socket];
#endif
    setup->cpu.IotEn[socket] = (UINT8) SetupData->SocketConfig.SocketProcessorCoreConfiguration.IotEn[socket];
    setup->cpu.OclaTorEntry[socket] = SetupData->SocketConfig.SocketProcessorCoreConfiguration.OclaMaxTorEntry[socket];
    setup->cpu.OclaWay[socket] = SetupData->SocketConfig.SocketProcessorCoreConfiguration.OclaMinWay[socket];
  }

}

VOID
OemGetPlatformSetupValueforMRC(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup       *setup;
  UINT8                 *pBuf;
  UINT8                 socket;
  UINT8                 ch;
  UINT8                 pagePolicy;
  UINT8                 phyRank;
  UINT8                 rank;
  UINT8                 dimm;
  UINT8                 dimmRank;
  EFI_HOB_GUID_TYPE     *OcGuidHob;
  OC_DATA_HOB           *OcData;
  TCG_SETUP             TcgSetup;
  UINT8                 MorControl;
  UINT8                 i;
  UINT8                 j;
  UINT8                 count;
  EFI_STATUS            Status;
  EFI_PEI_SERVICES                      **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  UINTN                                 VariableSize;
  PPR_ADDR_VARIABLE                     pprAddrVariable;
  PPR_ADDR_VARIABLE                     *pprAddrVariablePtr;
  UINT16                                pprCrc16;
  UINT16                                curCrc16;
#ifndef MINIBIOS_BUILD
  ADDRESS_RANGE_MIRROR_VARIABLE_DATA   MemoryRequest;
  ADDRESS_RANGE_MIRROR_VARIABLE_DATA  *AddressBasedMirrorData = NULL;
#endif
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  UINT8	Data = 0;
#endif

  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0,
                  NULL,
                  &PeiVariable
                  );

  VariableSize = sizeof (PPR_ADDR_VARIABLE);
  Status = PeiVariable->GetVariable (
                        PeiVariable,
                        PPR_VARIABLE_NAME,
                        &gEfiPprVariableGuid,
                        NULL,
                        &VariableSize,
                        &pprAddrVariable
                        );

  pprCrc16 =  pprAddrVariable.crc16;
  pprAddrVariable.crc16 = 0;
  curCrc16 = Crc16 ((char*)&pprAddrVariable, sizeof(PPR_ADDR_VARIABLE));

  if (EFI_ERROR(Status) || (pprCrc16 != curCrc16)) {
    DEBUG((EFI_D_ERROR, "PPR Variable not found or CRC mismatch - Status: 0x%0x, Crc: 0x%0x, calc. Crc: 0x%0x!\n", Status, pprCrc16, curCrc16));
    pprAddrVariablePtr = NULL;
  } else {
    pprAddrVariablePtr = &pprAddrVariable;
  }

  setup = (struct sysSetup *)(&host->setup);
  //  ddrFreqLimit
  //    0:Auto
  //    1:Force DDR-800
  //    3:Force DDR-1066
  //    5:Force DDR-1333
  //    7:Force DDR-1600
  //    9:Force DDR-1867
  //    11:Force DDR-2133
  //    13:Force DDR-2400
  //
  if (SetupData->SocketConfig.MemoryConfig.DdrFreqLimit == DDR3_FREQ_AUTO) {
    setup->mem.ddrFreqLimit = DDR3_FREQ_DEFAULT;
  } else {
    setup->mem.ddrFreqLimit = SetupData->SocketConfig.MemoryConfig.DdrFreqLimit;
  }

  setup->mem.imcBclk = SetupData->SocketConfig.MemoryConfig.imcBclk;
#if SMCPKG_SUPPORT
#if MemoryMapOut_SUPPORT
  if (SetupData->SocketConfig.MemoryConfig.MemMapOut == 0) {
    setup->mem.MemMapOut = 0;
  } else {
    setup->mem.MemMapOut = 1;
  }
#endif//MemoryMapOut_SUPPORT
  setup->mem.tCCDLRelaxation=  SetupData->SocketConfig.MemoryConfig.tCCDLRelaxation;
  setup->mem.tRWSREqualization=  SetupData->SocketConfig.MemoryConfig.tRWSREqualization;  
#endif//#if SMCPKG_SUPPORT

  if (SetupData->SocketConfig.MemoryConfig.EnforcePOR == ENFORCE_POR_AUTO) {
    setup->mem.enforcePOR = ENFORCE_POR_DEFAULT;
  } else {
    setup->mem.enforcePOR = SetupData->SocketConfig.MemoryConfig.EnforcePOR;
  }

  //setup->mem.options = 0;    // Zero out all options to start


  // DDR Reset loop Enable
  if (SetupData->SocketConfig.MemoryConfig.mrcRepeatTest) setup->mem.options |= DDR_RESET_LOOP;

  //
  // PPR options
  //
  setup->mem.pprType        = SetupData->SocketConfig.MemoryConfig.pprType;
  setup->mem.pprErrInjTest  = SetupData->SocketConfig.MemoryConfig.pprErrInjTest;

  if (pprAddrVariablePtr != NULL) {
    for (j = 0; j < MAX_PPR_ADDR_ENTRIES; j++) {
      setup->mem.pprAddrSetup[j].pprAddrStatus    = pprAddrVariable.pprAddrSetup[j].pprAddrStatus;
      setup->mem.pprAddrSetup[j].socket           = pprAddrVariable.pprAddrSetup[j].socket;
      setup->mem.pprAddrSetup[j].mc               = pprAddrVariable.pprAddrSetup[j].mc;
      setup->mem.pprAddrSetup[j].ch               = pprAddrVariable.pprAddrSetup[j].ch;
      setup->mem.pprAddrSetup[j].pprAddr.dimm     = pprAddrVariable.pprAddrSetup[j].dimm;
      setup->mem.pprAddrSetup[j].pprAddr.rank     = pprAddrVariable.pprAddrSetup[j].rank;
      setup->mem.pprAddrSetup[j].pprAddr.subRank  = pprAddrVariable.pprAddrSetup[j].subRank;
      setup->mem.pprAddrSetup[j].pprAddr.bank     = pprAddrVariable.pprAddrSetup[j].bank;
      setup->mem.pprAddrSetup[j].pprAddr.row      = pprAddrVariable.pprAddrSetup[j].row;
      setup->mem.pprAddrSetup[j].pprAddr.dramMask = pprAddrVariable.pprAddrSetup[j].dramMask;
    }
  }


  // WMM Opportunistic read disable
  if ((SetupData->SocketConfig.MemoryConfig.oppReadInWmm == OPP_RD_WMM_AUTO) && (OPP_RD_WMM_DEFAULT == OPP_RD_WMM_EN)) {
    setup->mem.options &= ~DISABLE_WMM_OPP_READ;
  } else if (SetupData->SocketConfig.MemoryConfig.oppReadInWmm == OPP_RD_WMM_EN){
    setup->mem.options &= ~DISABLE_WMM_OPP_READ;
  } else {
    setup->mem.options |= DISABLE_WMM_OPP_READ;
  }

  // ECC Support
  if ((SetupData->SocketConfig.MemoryConfig.EccSupport == ECC_AUTO) && (ECC_DEFAULT == ECC_EN)) {
    setup->mem.dfxMemSetup.dfxOptions|= ECC_CHECK_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.EccSupport == ECC_EN) {
    setup->mem.dfxMemSetup.dfxOptions |= ECC_CHECK_EN;
  } else {
    setup->mem.dfxMemSetup.dfxOptions &= ~ECC_CHECK_EN;
  }

  // Forcing ECC off for TypeHedtxxx
  if (host->nvram.common.platformType == TypeHedtCRB) {
    setup->mem.dfxMemSetup.dfxOptions &= ~ECC_CHECK_EN;
  }
  // Setup option for memory test
  if ((SetupData->SocketConfig.MemoryConfig.HwMemTest == MEM_TEST_AUTO) && (MEM_TEST_DEFAULT == MEM_TEST_EN)) {
    setup->mem.options |= MEMORY_TEST_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.HwMemTest == MEM_TEST_EN) {
    setup->mem.options |= MEMORY_TEST_EN;
  } else {
    setup->mem.options &= ~MEMORY_TEST_EN;
  }

  //
  // Setup option for memory test loops
  //
  setup->mem.memTestLoops = SetupData->SocketConfig.MemoryConfig.MemTestLoops;

  if (SetupData->SocketConfig.MemoryConfig.refreshMode){
    setup->mem.options |= ALLOW2XREF_EN;
  } else {
    setup->mem.options &= ~ALLOW2XREF_EN;
  }

  // Rank Margin Tool
  if ((SetupData->SocketConfig.MemoryConfig.RankMargin == RMT_EN) ||
      ((SetupData->SocketConfig.MemoryConfig.RankMargin == RMT_AUTO) && (RMT_DEFAULT == RMT_EN))) {
    setup->mem.options |= MARGIN_RANKS_EN;
#ifdef SERIAL_DBG_MSG
    if (SetupData->SocketConfig.MemoryConfig.RankMargin == RMT_EN) {
      host->setup.common.serialDebugMsgLvl = SDBG_MAX;
    }
#endif
  } else {
    setup->mem.options &= ~MARGIN_RANKS_EN;
#ifdef SERIAL_DBG_MSG
    host->setup.common.serialDebugMsgLvl = 0;
#endif
  }


#ifdef LRDIMM_SUPPORT
  //
  // Backside Rank Margin Tool
  //
  if (SetupData->SocketConfig.MemoryConfig.EnableBacksideRMT == BACKSIDE_RMT_ENABLE) {
    setup->mem.enableBacksideRMT = BACKSIDE_RMT_ENABLE;
  } else if((SetupData->SocketConfig.MemoryConfig.EnableBacksideRMT == BACKSIDE_RMT_AUTO) && (BACKSIDE_RMT_DEFAULT == BACKSIDE_RMT_ENABLE)) {
    setup->mem.enableBacksideRMT = BACKSIDE_RMT_ENABLE;
  } else {
    setup->mem.enableBacksideRMT = BACKSIDE_RMT_DISABLE;
  }

  //
  // Backside CMD Rank Margin Tool
  //
  if (SetupData->SocketConfig.MemoryConfig.EnableBacksideCMDRMT == RMT_EN) {
    setup->mem.enableBacksideCMDRMT = RMT_EN;
  } else {
    setup->mem.enableBacksideCMDRMT = RMT_DIS;
  }

#endif  //LRDIMM_SUPPORT

  // Scrambling DDRT
  if ((SetupData->SocketConfig.MemoryConfig.ScrambleEnDDRT == SCRAMBLE_ENABLE) ||
      ((SetupData->SocketConfig.MemoryConfig.ScrambleEnDDRT == SCRAMBLE_AUTO) && (SCRAMBLE_DEFAULT == SCRAMBLE_ENABLE))) {
    setup->mem.options |= SCRAMBLE_EN_DDRT;
  } else {
    setup->mem.options &= ~SCRAMBLE_EN_DDRT;
  }

  // Scrambling DDR4
  if ((SetupData->SocketConfig.MemoryConfig.ScrambleEn == SCRAMBLE_ENABLE) ||
      ((SetupData->SocketConfig.MemoryConfig.ScrambleEn == SCRAMBLE_AUTO) && (SCRAMBLE_DEFAULT == SCRAMBLE_ENABLE))) {
    setup->mem.options |= SCRAMBLE_EN;
  } else {
    setup->mem.options &= ~SCRAMBLE_EN;
  }

  // Allow SBE during Memory training
  if ( SetupData->SocketConfig.MemoryConfig.allowCorrectableError ){
    setup->mem.optionsExt |= ALLOW_CORRECTABLE_ERROR;
  } else {
    setup->mem.optionsExt &= ~ALLOW_CORRECTABLE_ERROR;
  }

  // SPD_CRC_CHECK
  if ((SetupData->SocketConfig.MemoryConfig.spdCrcCheck == SPD_CRC_CHECK_AUTO) && (SPD_CRC_CHECK_DEFAULT == SPD_CRC_CHECK_EN)) {
    setup->mem.optionsExt |= SPD_CRC_CHECK;
  } else if (SetupData->SocketConfig.MemoryConfig.spdCrcCheck == SPD_CRC_CHECK_EN) {
    setup->mem.optionsExt |= SPD_CRC_CHECK;
  } else {
    setup->mem.optionsExt &= ~SPD_CRC_CHECK;
  }

  // Fast boot
  if ((SetupData->SocketConfig.MemoryConfig.AttemptFastBoot == FAST_BOOT_AUTO) && (MEM_CHIP_POLICY_VALUE(host, FastBootDefault) == FAST_BOOT_EN)) {
    setup->mem.options |= ATTEMPT_FAST_BOOT;
  } else if (SetupData->SocketConfig.MemoryConfig.AttemptFastBoot == FAST_BOOT_EN) {
    setup->mem.options |= ATTEMPT_FAST_BOOT;
  } else {
    setup->mem.options &= ~ATTEMPT_FAST_BOOT;
  }

  // Fast cold boot
  if ((SetupData->SocketConfig.MemoryConfig.AttemptFastBootCold == FAST_BOOT_COLD_AUTO) && (FAST_BOOT_COLD_DEFAULT == FAST_BOOT_COLD_EN)) {
    setup->mem.options |= ATTEMPT_FAST_BOOT_COLD;
  } else if (SetupData->SocketConfig.MemoryConfig.AttemptFastBootCold == FAST_BOOT_COLD_EN) {
    setup->mem.options |= ATTEMPT_FAST_BOOT_COLD;
  } else {
    setup->mem.options &= ~ATTEMPT_FAST_BOOT_COLD;
  }

  // Grantley HSD:4987169 HEDT BCLK RATIO

  OcGuidHob       = GetFirstGuidHob (&gEfiOcDataGuid);
  if(OcGuidHob != NULL) {
    OcData          = GET_GUID_HOB_DATA (OcGuidHob);
    setup->mem.bclk = OcData->DmiPeg;
  } else {
    setup->mem.bclk = DEFAULT_BCLK;
  }

  // RMT on Fast cold boot
  if ((SetupData->SocketConfig.MemoryConfig.RmtOnColdFastBoot == RMT_COLD_FAST_BOOT_AUTO) && (MEM_CHIP_POLICY_VALUE(host, RmtColdFastBootDefault) == RMT_COLD_FAST_BOOT_EN)) {
    setup->mem.options |= RMT_COLD_FAST_BOOT;
  } else if (SetupData->SocketConfig.MemoryConfig.RmtOnColdFastBoot == RMT_COLD_FAST_BOOT_EN) {
    setup->mem.options |= RMT_COLD_FAST_BOOT;
  } else {
    setup->mem.options &= ~RMT_COLD_FAST_BOOT;
  }

  // C/A Parity
  if (SetupData->SocketConfig.MemoryConfig.caParity) {
    setup->mem.options |= CA_PARITY_EN;
  } else {
    setup->mem.options &= ~CA_PARITY_EN;
  }

  // Write CRC
  if (SetupData->SocketConfig.MemoryConfig.WrCRC) {
    setup->mem.optionsExt |= WR_CRC;
  } else {
    setup->mem.optionsExt &= ~WR_CRC;
  }

  // DIMM Isolation
  if (SetupData->SocketConfig.MemoryConfig.dimmIsolation) {
    setup->mem.optionsExt |= DIMM_ISOLATION_EN;
  } else {
    setup->mem.optionsExt &= ~DIMM_ISOLATION_EN;
  }

  // Forcing C/A Parity off for TypeHedtxxx
  if (host->nvram.common.platformType == TypeHedtCRB) {
    setup->mem.options &= ~CA_PARITY_EN;
  }
  if ((SetupData->SocketConfig.MemoryConfig.MultiThreaded == MULTI_THREADED_EN)  ||
      ((SetupData->SocketConfig.MemoryConfig.MultiThreaded == MULTI_THREADED_AUTO) && (MULTI_THREADED_DEFAULT == MULTI_THREADED_EN))) {
    setup->mem.options |= MULTI_THREAD_MRC_EN;
  } else {
    setup->mem.options &= ~MULTI_THREAD_MRC_EN;
  }

  setup->mem.options &= ~(MEM_OVERRIDE_EN);
  if (SetupData->SocketConfig.MemoryConfig.XMPMode != XMP_AUTO) {
    setup->mem.options |= MEM_OVERRIDE_EN;
    setup->mem.enforcePOR = ENFORCE_POR_DIS;
  }

  //
  // Verify that the values being set are within a valid range. They are UINTs so the
  // so the minimum value does not need to be checked.
  //
  //setup->mem.ddrFreqLimit = SetupData->SocketConfig.MemoryConfig.DdrFreqLimit;
  setup->mem.inputMemTime.vdd = SetupData->SocketConfig.MemoryConfig.Vdd;

  if (SetupData->SocketConfig.MemoryConfig.tCAS > MAX_CAS_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tCAS = 0;  // If out of range set the default.
  }
  setup->mem.inputMemTime.nCL = SetupData->SocketConfig.MemoryConfig.tCAS;

  if (SetupData->SocketConfig.MemoryConfig.tRP > MAX_TRP_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRP = 0;   // If out of range set the default.
  }
  setup->mem.inputMemTime.nRP = SetupData->SocketConfig.MemoryConfig.tRP;

  if (SetupData->SocketConfig.MemoryConfig.tRCD > MAX_TRCD_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRCD = 0;  // If out of range set the default
  }
  setup->mem.inputMemTime.nRCD = SetupData->SocketConfig.MemoryConfig.tRCD;

  if (SetupData->SocketConfig.MemoryConfig.tRRD > MAX_TRRD_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRRD = 0;   // If out of range set the default
  }
  setup->mem.inputMemTime.nRRD = SetupData->SocketConfig.MemoryConfig.tRRD;

  if (SetupData->SocketConfig.MemoryConfig.tWTR > MAX_TWTR_LATENCY) {
    SetupData->SocketConfig.MemoryConfig.tWTR = 0;    // If out of range set the default
  }
  setup->mem.inputMemTime.nWTR = SetupData->SocketConfig.MemoryConfig.tWTR;

  if (SetupData->SocketConfig.MemoryConfig.tRAS > MAX_TRAS_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRAS = 0;   // If out of range set the default
  }
  setup->mem.inputMemTime.nRAS = SetupData->SocketConfig.MemoryConfig.tRAS;

  if (SetupData->SocketConfig.MemoryConfig.tRTP > MAX_TRTP_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRTP = 0;   // If out of range set the default
  }
   setup->mem.inputMemTime.nRTP = SetupData->SocketConfig.MemoryConfig.tRTP;

  if (SetupData->SocketConfig.MemoryConfig.tWR > MAX_TWR_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tWR = 0;     // If out of range set the default
  }
  setup->mem.inputMemTime.nWR = SetupData->SocketConfig.MemoryConfig.tWR;

  if (SetupData->SocketConfig.MemoryConfig.tFAW > MAX_TFAW_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tFAW = 0;   // If out of range set the default
  }
  setup->mem.inputMemTime.nFAW = SetupData->SocketConfig.MemoryConfig.tFAW;

  if (SetupData->SocketConfig.MemoryConfig.tCWL > MAX_TCWL_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tCWL = 0;    // If out of range set the default
  }
  setup->mem.inputMemTime.nCWL = SetupData->SocketConfig.MemoryConfig.tCWL;

  if (SetupData->SocketConfig.MemoryConfig.tRC > MAX_TRC_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRC = 0;     // If out of range set the default
  }
  setup->mem.inputMemTime.nRC = SetupData->SocketConfig.MemoryConfig.tRC;

  if (SetupData->SocketConfig.MemoryConfig.tREFI > MAX_REFRESH_RATE){
    SetupData->SocketConfig.MemoryConfig.tREFI = 0;  // If out of range set the default
  }
  setup->mem.inputMemTime.tREFI = SetupData->SocketConfig.MemoryConfig.tREFI;

  if (SetupData->SocketConfig.MemoryConfig.tRFC > MAX_TRFC_LATENCY){
    SetupData->SocketConfig.MemoryConfig.tRFC = 0;    // If out of range set the default
  }
  setup->mem.inputMemTime.nRFC = SetupData->SocketConfig.MemoryConfig.tRFC;
  setup->mem.inputMemTime.nCMDRate = SetupData->SocketConfig.MemoryConfig.commandTiming;

  //
  // Memory type
  //
  setup->mem.dimmTypeSupport = SetupData->SocketConfig.MemoryConfig.DdrMemoryType;

  // Interleave and NUMA

  if (SetupData->SocketConfig.CommonRcConfig.NumaEn == NUMA_EN) {
    setup->mem.options |= NUMA_AWARE;
    setup->mem.socketInter = SOCKET_1WAY;
  } else {
    setup->mem.options &= ~NUMA_AWARE;
    setup->mem.socketInter = SOCKET_2WAY;
    if ((SetupData->SocketConfig.MemoryConfig.dfxMaxNodeInterleave == SOCKET_INTER_AUTO) && (SOCKET_INTER_AUTO == SOCKET_2WAY)) {  //to force socketInter = 2 when NUMA_DIS
      setup->mem.dfxMemSetup.dfxMaxNodeInterleave = SOCKET_2WAY;
    } else if (SetupData->SocketConfig.MemoryConfig.dfxMaxNodeInterleave == SOCKET_2WAY) {
      setup->mem.dfxMemSetup.dfxMaxNodeInterleave = SOCKET_2WAY;
    } else if (SetupData->SocketConfig.MemoryConfig.dfxMaxNodeInterleave == SOCKET_4WAY) {
      setup->mem.dfxMemSetup.dfxMaxNodeInterleave = SOCKET_4WAY;
    }
  }

  setup->mem.setTDPDIMMPower = SetupData->SocketConfig.MemoryConfig.setTDPDIMMPower;
  setup->mem.setSecureEraseAllDIMMs = SetupData->SocketConfig.MemoryConfig.setSecureEraseAllDIMMs;

  socket = 0;
  ch = 0;
  for(dimm = 0; dimm < MAX_AEP_DIMMS; dimm++) {
    if (ch == MAX_CH) {
      ch = 0;
      socket += 1;
    }
    if (socket < MAX_SOCKET) {
      setup->mem.setSecureEraseSktCh[socket][ch] = SetupData->SocketConfig.MemoryConfig.setSecureEraseSktCh[dimm];
    }
    ch++;
  }//dimm

  setup->mem.volMemMode = SetupData->SocketConfig.MemoryConfig.volMemMode ;
  setup->mem.dfxMemSetup.dfxPerMemMode = SetupData->SocketConfig.MemoryConfig.dfxPerMemMode ;
  setup->mem.dfxMemSetup.dfxDimmManagement = SetupData->SocketConfig.MemoryConfig.dfxDimmManagement ;
  setup->mem.dfxMemSetup.dfxLoadDimmMgmtDriver = SetupData->SocketConfig.MemoryConfig.dfxLoadDimmMgmtDriver ;
  setup->mem.dfxMemSetup.dfxPartitionDDRTDimm = SetupData->SocketConfig.MemoryConfig.dfxPartitionDDRTDimm ;
  for(i = 0; i < 8; i++) {
    setup->mem.dfxMemSetup.dfxPartitionRatio[i] = SetupData->SocketConfig.MemoryConfig.dfxPartitionRatio[i] ;
  }
  setup->mem.dfxMemSetup.dfxCfgMask2LM = SetupData->SocketConfig.MemoryConfig.dfxCfgMask2LM ;
  if (SetupData->SocketConfig.MemoryConfig.memInterleaveGran1LM == MEM_INT_GRAN_1LM_AUTO)  {
    setup->mem.memInterleaveGran1LM = MEM_INT_GRAN_1LM_DEFAULT ;
  } else {
    setup->mem.memInterleaveGran1LM = SetupData->SocketConfig.MemoryConfig.memInterleaveGran1LM ;
  }

  setup->mem.memInterleaveGran2LM = MEM_INT_GRAN_2LM_DEFAULT ;
  setup->mem.dfxMemSetup.dfxMemInterleaveGranPMemNUMA = MEM_INT_GRAN_PMEM_NUMA_DEFAULT ;

  if ((setup->mem.socketInter == SOCKET_1WAY) && SetupData->SocketConfig.MemoryConfig.SocketInterleaveBelow4GB == SOCKET_INTLV_BELOW_4G_EN) {
    setup->mem.options |= SPLIT_BELOW_4GB_EN;
  } else {
    setup->mem.options &= ~SPLIT_BELOW_4GB_EN;
  }

  if (SetupData->SocketConfig.MemoryConfig.ImcInterleaving == IMC_INTER_AUTO) {
    if (SetupData->SocketConfig.CsiConfig.SncEn == KTI_ENABLE){
      setup->mem.imcInter = IMC_1WAY;
    }
    else {
      setup->mem.imcInter = IMC_INTER_DEFAULT;
    }
    if (host->var.common.stepping >= B0_REV_SKX) {
      setup->mem.imcInter = IMC_2WAY;
    }

  } else {
    setup->mem.imcInter = SetupData->SocketConfig.MemoryConfig.ImcInterleaving;
  }

  if (SetupData->SocketConfig.MemoryConfig.ChannelInterleaving == CH_INTER_AUTO) {
    setup->mem.chInter = CH_INTER_DEFAULT;
  } else {
    setup->mem.chInter = SetupData->SocketConfig.MemoryConfig.ChannelInterleaving;
  }

  if (SetupData->SocketConfig.MemoryConfig.RankInterleaving == RANK_INTER_AUTO) {
    setup->mem.rankInter = RANK_INTER_DEFAULT;
  } else {
    setup->mem.rankInter = SetupData->SocketConfig.MemoryConfig.RankInterleaving;
  }

  // Page mode
  if (SetupData->SocketConfig.MemoryConfig.PagePolicy == CLOSED_PAGE_AUTO) {
    pagePolicy = CLOSED_PAGE_DEFAULT;
  } else {
    pagePolicy = SetupData->SocketConfig.MemoryConfig.PagePolicy;
  }

  if (pagePolicy == CLOSED_PAGE_EN) {
    // Closed
    setup->mem.options |= PAGE_POLICY;
    setup->mem.options &= ~ADAPTIVE_PAGE_EN;
  } else {
    // Open Adaptive
    setup->mem.options &= ~PAGE_POLICY;
    setup->mem.options |= ADAPTIVE_PAGE_EN;
  }

  setup->mem.MdllOffEn = SetupData->SocketConfig.MemoryConfig.MdllOffEn;
  setup->mem.Blockgnt2cmd1cyc = SetupData->SocketConfig.MemoryConfig.Blockgnt2cmd1cyc;
  setup->mem.Disddrtopprd = SetupData->SocketConfig.MemoryConfig.Disddrtopprd;
  setup->mem.OppSrefEn = SetupData->SocketConfig.MemoryConfig.OppSrefEn;
  setup->mem.SrefProgramming = SetupData->SocketConfig.MemoryConfig.SrefProgramming;
  setup->mem.CkMode = SetupData->SocketConfig.MemoryConfig.CkMode;
  setup->mem.DdrtSrefEn = SetupData->SocketConfig.MemoryConfig.DdrtSrefEn;

  setup->mem.CkeIdleTimer = SetupData->SocketConfig.MemoryConfig.CkeIdleTimer;
  setup->mem.ApdEn = SetupData->SocketConfig.MemoryConfig.ApdEn;
  setup->mem.PpdEn = SetupData->SocketConfig.MemoryConfig.PpdEn;
  setup->mem.CkeProgramming = SetupData->SocketConfig.MemoryConfig.CkeProgramming;
  setup->mem.DdrtCkeEn = SetupData->SocketConfig.MemoryConfig.DdrtCkeEn;
  setup->mem.PkgcSrefEn = SetupData->SocketConfig.MemoryConfig.PkgcSrefEn;

  // Setup option for memory test on fast boot
  if ((SetupData->SocketConfig.MemoryConfig.MemTestOnFastBoot == MEM_TEST_FAST_BOOT_AUTO) && (MEM_TEST_FAST_BOOT_DEFAULT == MEM_TEST_FAST_BOOT_EN)) {
    setup->mem.options |= MEMORY_TEST_FAST_BOOT_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.MemTestOnFastBoot == MEM_TEST_FAST_BOOT_EN) {
    setup->mem.options |= MEMORY_TEST_FAST_BOOT_EN;
  } else {
    setup->mem.options &= ~MEMORY_TEST_FAST_BOOT_EN;
  }

  setup->mem.dramraplbwlimittf = SetupData->SocketConfig.MemoryConfig.BwLimitTfOvrd;
  setup->mem.dramraplen = SetupData->SocketConfig.MemoryConfig.DramRaplInit;
  setup->mem.DramRaplExtendedRange = SetupData->SocketConfig.MemoryConfig.DramRaplExtendedRange;
  setup->mem.DimmTempStatValue = SetupData->SocketConfig.MemoryConfig.DimmTempStatValue;
  setup->mem.CmsEnableDramPm = SetupData->SocketConfig.MemoryConfig.CmsEnableDramPm;

#ifdef BDAT_SUPPORT
  setup->common.bdatEn = SetupData->SocketConfig.MemoryConfig.bdatEn;
#endif //BDAT_SUPPORT

  //
  // Verify that the values being set are within a valid range. They are UINTx so the
  // so the minimum value does not need to be checked.
  //
  if (SetupData->SocketConfig.MemoryConfig.ScrambleSeedLow > MAX_SCRAMBLE_SEED_LOW){
    SetupData->SocketConfig.MemoryConfig.ScrambleSeedLow = SCRAMBLE_SEED_LOW;   // If out of range set the default
  }
  setup->mem.scrambleSeedLow = SetupData->SocketConfig.MemoryConfig.ScrambleSeedLow;

  if (SetupData->SocketConfig.MemoryConfig.ScrambleSeedHigh > MAX_SCRAMBLE_SEED_HIGH){
    SetupData->SocketConfig.MemoryConfig.ScrambleSeedHigh = SCRAMBLE_SEED_HIGH; // If out of range set the default
  }
  setup->mem.scrambleSeedHigh = SetupData->SocketConfig.MemoryConfig.ScrambleSeedHigh;

  if (SetupData->SocketConfig.MemoryConfig.mcBgfThreshold > MAX_MC_BGF_THRESHOLD){
    SetupData->SocketConfig.MemoryConfig.mcBgfThreshold= 0;  // If out of range set the default
  }
  setup->mem.mcBgfThreshold = SetupData->SocketConfig.MemoryConfig.mcBgfThreshold;

  setup->mem.dllResetTestLoops = SetupData->SocketConfig.MemoryConfig.dllResetTestLoops;

  setup->mem.writePreamble = SetupData->SocketConfig.MemoryConfig.WritePreamble;
  setup->mem.readPreamble = SetupData->SocketConfig.MemoryConfig.ReadPreamble;

  setup->mem.normOppIntvl = SetupData->SocketConfig.MemoryConfig.normOppInterval;

  if (SetupData->SocketConfig.MemoryConfig.mdllSden == MDLL_SDEN_EN){
    setup->mem.options |= MDLL_SHUT_DOWN_EN;
  } else if((SetupData->SocketConfig.MemoryConfig.mdllSden == MDLL_SDEN_AUTO) && (MDLL_SDEN_DEFAULT == MDLL_SDEN_EN)) {
    setup->mem.options |= MDLL_SHUT_DOWN_EN;
  } else {
    setup->mem.options &= ~MDLL_SHUT_DOWN_EN;
  }

  setup->mem.logParsing = SetupData->SocketConfig.MemoryConfig.logParsing;

  if (SetupData->SocketConfig.MemoryConfig.DutyCycleTraining == DUTY_CYCLE_ENABLE) {
    setup->mem.optionsExt |= DUTY_CYCLE_EN;
  } else {
    setup->mem.optionsExt &= ~DUTY_CYCLE_EN;
  }

  if (SetupData->SocketConfig.MemoryConfig.readVrefCenter == RX_VREF_ENABLE){
    setup->mem.optionsExt |= RD_VREF_EN;
  } else if((SetupData->SocketConfig.MemoryConfig.readVrefCenter == RX_VREF_AUTO) && (RX_VREF_DEFAULT == RX_VREF_ENABLE)) {
    setup->mem.optionsExt |= RD_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~RD_VREF_EN;
  }

  //
  // PDA
  //
  if ((SetupData->SocketConfig.MemoryConfig.pda == PDA_ENABLE) ||
      ((SetupData->SocketConfig.MemoryConfig.pda == PDA_AUTO) && (PDA_DEFAULT == PDA_ENABLE))) {
    setup->mem.optionsExt |= PDA_EN;
  } else {
    setup->mem.optionsExt &= ~PDA_EN;
  }

  //
  // Turnaround Time Optimization
  //
  if ((SetupData->SocketConfig.MemoryConfig.turnaroundOpt == TURNAROUND_ENABLE) ||
      ((SetupData->SocketConfig.MemoryConfig.turnaroundOpt == TURNAROUND_AUTO) && (TURNAROUND_DEFAULT == TURNAROUND_ENABLE))) {
    setup->mem.optionsExt |= TURNAROUND_OPT_EN;
  } else {
    setup->mem.optionsExt &= ~TURNAROUND_OPT_EN;
  }

  //
  // One Rank Timing Mode Enable
  //
  setup->mem.oneRankTimingModeEn = SetupData->SocketConfig.MemoryConfig.oneRankTimingMode;


#ifdef SSA_FLAG
  //
  // Enabling the BIOS SSA loader
  //
  if (SetupData->SocketConfig.MemoryConfig.EnableBiosSsaLoader == BIOS_SSA_LOADER_ENABLE) {
    setup->mem.enableBiosSsaLoader = BIOS_SSA_LOADER_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.EnableBiosSsaLoader == BIOS_SSA_LOADER_AUTO) && (BIOS_SSA_LOADER_DEFAULT == BIOS_SSA_LOADER_ENABLE)) {
    setup->mem.enableBiosSsaLoader = BIOS_SSA_LOADER_ENABLE;
  } else {
    setup->mem.enableBiosSsaLoader = BIOS_SSA_LOADER_DISABLE;
  }

  //
  // Enabling the BIOS SSA Stitched Mode (RMT)
  //
  if (SetupData->SocketConfig.MemoryConfig.EnableBiosSsaRMT == BIOS_SSA_RMT_ENABLE) {
    setup->mem.enableBiosSsaRMT = BIOS_SSA_RMT_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.EnableBiosSsaRMT == BIOS_SSA_RMT_AUTO) && (BIOS_SSA_RMT_DEFAULT == BIOS_SSA_RMT_ENABLE)) {
    setup->mem.enableBiosSsaRMT = BIOS_SSA_RMT_ENABLE;
  } else {
    setup->mem.enableBiosSsaRMT = BIOS_SSA_RMT_DISABLE;
  }

  //
  // Enabling the BIOS SSA Stitched Mode (RMT) on FCB
  //
  if (SetupData->SocketConfig.MemoryConfig.EnableBiosSsaRMTonFCB == BIOS_SSA_RMT_FCB_ENABLE) {
    setup->mem.enableBiosSsaRMTonFCB = BIOS_SSA_RMT_FCB_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.EnableBiosSsaRMTonFCB == BIOS_SSA_RMT_FCB_AUTO) && (BIOS_SSA_RMT_FCB_DEFAULT == BIOS_SSA_RMT_FCB_ENABLE)) {
    setup->mem.enableBiosSsaRMTonFCB = BIOS_SSA_RMT_FCB_ENABLE;
  } else {
    setup->mem.enableBiosSsaRMTonFCB = BIOS_SSA_RMT_FCB_DISABLE;
  }

  //
  // Enabling BiosSsaPerBitMargining
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaPerBitMargining == BIOS_SSA_PER_BIT_MARGINING_ENABLE) {
    setup->mem.biosSsaPerBitMargining = BIOS_SSA_PER_BIT_MARGINING_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaPerBitMargining == BIOS_SSA_PER_BIT_MARGINING_AUTO) && (BIOS_SSA_PER_BIT_MARGINING_DEFAULT == BIOS_SSA_PER_BIT_MARGINING_ENABLE)) {
    setup->mem.biosSsaPerBitMargining = BIOS_SSA_PER_BIT_MARGINING_ENABLE;
  } else {
    setup->mem.biosSsaPerBitMargining = BIOS_SSA_PER_BIT_MARGINING_DISABLE;
  }

  //
  // Enabling BiosSsaDisplayTables
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaDisplayTables == BIOS_SSA_DISPLAY_TABLE_ENABLE) {
    setup->mem.biosSsaDisplayTables = BIOS_SSA_DISPLAY_TABLE_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaDisplayTables == BIOS_SSA_DISPLAY_TABLE_AUTO) && (BIOS_SSA_DISPLAY_TABLE_DEFAULT == BIOS_SSA_DISPLAY_TABLE_ENABLE)) {
    setup->mem.biosSsaDisplayTables = BIOS_SSA_DISPLAY_TABLE_ENABLE;
  } else {
    setup->mem.biosSsaDisplayTables = BIOS_SSA_DISPLAY_TABLE_DISABLE;
  }

  //
  // Enabling BiosSsaPerDisplayPlots
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaPerDisplayPlots == BIOS_SSA_DISPLAY_PLOTS_ENABLE) {
    setup->mem.biosSsaPerDisplayPlots = BIOS_SSA_DISPLAY_PLOTS_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaPerDisplayPlots == BIOS_SSA_DISPLAY_PLOTS_AUTO) && (BIOS_SSA_DISPLAY_PLOTS_DEFAULT == BIOS_SSA_DISPLAY_PLOTS_ENABLE)) {
    setup->mem.biosSsaPerDisplayPlots = BIOS_SSA_DISPLAY_PLOTS_ENABLE;
  } else {
    setup->mem.biosSsaPerDisplayPlots = BIOS_SSA_DISPLAY_PLOTS_DISABLE;
  }

  //
  // Enabling BiosSsaBacksideMargining;
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaBacksideMargining == BIOS_SSA_BACKSIDE_MARGINING_ENABLE) {
    setup->mem.biosSsaBacksideMargining = BIOS_SSA_BACKSIDE_MARGINING_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaBacksideMargining == BIOS_SSA_BACKSIDE_MARGINING_AUTO) && (BIOS_SSA_BACKSIDE_MARGINING_DEFAULT == BIOS_SSA_BACKSIDE_MARGINING_ENABLE)) {
    setup->mem.biosSsaBacksideMargining = BIOS_SSA_BACKSIDE_MARGINING_ENABLE;
  } else {
    setup->mem.biosSsaBacksideMargining = BIOS_SSA_BACKSIDE_MARGINING_DISABLE;
  }

  //
  // Enabling BiosSsaEarlyReadIdMargining
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaEarlyReadIdMargining == BIOS_SSA_EARLY_READ_ID_ENABLE) {
    setup->mem.biosSsaEarlyReadIdMargining = BIOS_SSA_EARLY_READ_ID_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaEarlyReadIdMargining == BIOS_SSA_EARLY_READ_ID_AUTO) && (BIOS_SSA_EARLY_READ_ID_DEFAULT == BIOS_SSA_EARLY_READ_ID_ENABLE)) {
    setup->mem.biosSsaEarlyReadIdMargining = BIOS_SSA_EARLY_READ_ID_ENABLE;
  } else {
    setup->mem.biosSsaEarlyReadIdMargining = BIOS_SSA_EARLY_READ_ID_DISABLE;
  }

  //
  // Enabling BiosSsaStepSizeOverride;
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaStepSizeOverride == BIOS_SSA_STEP_SIZE_OVERRIDE_ENABLE) {
    setup->mem.biosSsaStepSizeOverride = BIOS_SSA_STEP_SIZE_OVERRIDE_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaStepSizeOverride == BIOS_SSA_STEP_SIZE_OVERRIDE_AUTO) && (BIOS_SSA_STEP_SIZE_OVERRIDE_DEFAULT == BIOS_SSA_STEP_SIZE_OVERRIDE_ENABLE)) {
    setup->mem.biosSsaStepSizeOverride = BIOS_SSA_STEP_SIZE_OVERRIDE_ENABLE;
  } else {
    setup->mem.biosSsaStepSizeOverride = BIOS_SSA_STEP_SIZE_OVERRIDE_DISABLE;
  }

  //
  // Enabling BiosSsaDebugMessages
  //
  if (SetupData->SocketConfig.MemoryConfig.BiosSsaDebugMessages == BIOS_SSA_RMT_DEBUG_MSGS_ENABLE) {
    setup->mem.biosSsaDebugMessages = BIOS_SSA_RMT_DEBUG_MSGS_ENABLE;
  } else if ((SetupData->SocketConfig.MemoryConfig.BiosSsaDebugMessages == BIOS_SSA_RMT_DEBUG_MSGS_AUTO) && (BIOS_SSA_RMT_DEBUG_MSGS_DEFAULT == BIOS_SSA_RMT_DEBUG_MSGS_ENABLE)) {
    setup->mem.biosSsaDebugMessages = BIOS_SSA_RMT_DEBUG_MSGS_ENABLE;
  } else {
    setup->mem.biosSsaDebugMessages = BIOS_SSA_RMT_DEBUG_MSGS_DISABLE;
  }

  //
  //Exponential loop count for single rank test
  //
  setup->mem.biosSsaLoopCount = SetupData->SocketConfig.MemoryConfig.BiosSsaLoopCount;

  //
  //  Step size of RxDqs
  //
  setup->mem.biosSsaRxDqs = SetupData->SocketConfig.MemoryConfig.BiosSsaRxDqs;

  //
  //  Step size of RxVref
  //
  setup->mem.biosSsaRxVref = SetupData->SocketConfig.MemoryConfig.BiosSsaRxVref;
  //
  //  Step size of TxDq
  //
  setup->mem.biosSsaTxDq = SetupData->SocketConfig.MemoryConfig.BiosSsaTxDq;
  //
  //  Step size of TxVref
  //
  setup->mem.biosSsaTxVref = SetupData->SocketConfig.MemoryConfig.BiosSsaTxVref;
  //
  //  Step size of CmdAll
  //
  setup->mem.biosSsaCmdAll = SetupData->SocketConfig.MemoryConfig.BiosSsaCmdAll;
  //
  //  Step size of CmdVref
  //
  setup->mem.biosSsaCmdVref = SetupData->SocketConfig.MemoryConfig.BiosSsaCmdVref;
  //
  //  Step size of CtlVref
  //
  setup->mem.biosSsaCtlAll = SetupData->SocketConfig.MemoryConfig.BiosSsaCtlAll;
  //
  //  Step size of EridDelay
  //
  setup->mem.biosSsaEridDelay = SetupData->SocketConfig.MemoryConfig.BiosSsaEridDelay;
  //
  //  Step size of EridVref
  //
  setup->mem.biosSsaEridVref = SetupData->SocketConfig.MemoryConfig.BiosSsaEridVref;
#endif  //SSA_FLAG

  if (SetupData->SocketConfig.MemoryConfig.wrVrefCenter == TX_VREF_ENABLE){
    setup->mem.optionsExt |= WR_VREF_EN;
  } else if((SetupData->SocketConfig.MemoryConfig.wrVrefCenter == TX_VREF_AUTO) && (TX_VREF_DEFAULT == TX_VREF_ENABLE)) {
    setup->mem.optionsExt |= WR_VREF_EN;
  } else {
    setup->mem.optionsExt &= ~WR_VREF_EN;
  }

#ifdef MARGIN_CHECK
  if (SetupData->SocketConfig.MemoryConfig.perbitmargin == PER_BIT_MARGIN_ENABLE){
    setup->mem.optionsExt |= PER_BIT_MARGINS;
  } else if((SetupData->SocketConfig.MemoryConfig.perbitmargin == PER_BIT_MARGIN_AUTO) && (MEM_CHIP_POLICY_VALUE(host, PerBitMarginDefault) == PER_BIT_MARGIN_ENABLE)) {
    setup->mem.optionsExt |= PER_BIT_MARGINS;
  } else {
    setup->mem.optionsExt &= ~PER_BIT_MARGINS;
  }

  setup->mem.rmtPatternLength = SetupData->SocketConfig.MemoryConfig.rmtPatternLength;
  setup->mem.rmtPatternLengthExt = SetupData->SocketConfig.MemoryConfig.rmtPatternLengthExt;
#endif

  if (SetupData->SocketConfig.MemoryConfig.EnableNgnBcomMargining == RMT_EN){
    setup->mem.enableNgnBcomMargining = RMT_EN;
  } else {
    setup->mem.enableNgnBcomMargining = RMT_DIS;
  }

  setup->mem.check_pm_sts = SetupData->SocketConfig.MemoryConfig.check_pm_sts;
  setup->mem.check_platform_detect = SetupData->SocketConfig.MemoryConfig.check_platform_detect;

  //
  // common options
  //
  setup->common.options &= ~(PROMOTE_MRC_WARN_EN | HALT_ON_ERROR_EN | PROMOTE_WARN_EN);

  if (SetupData->SocketConfig.MemoryConfig.promoteMrcWarnings)  setup->common.options |= PROMOTE_MRC_WARN_EN;
  if (SetupData->SocketConfig.MemoryConfig.haltOnMemErr)        setup->common.options |= HALT_ON_ERROR_EN;
  if (SetupData->SocketConfig.MemoryConfig.promoteWarnings)     setup->common.options |= PROMOTE_WARN_EN;

  //
  // thermal throttling options
  //
  setup->mem.thermalThrottlingOptions = 0;

  if (SetupData->SocketConfig.MemoryConfig.thermalthrottlingsupport == 2) {
    setup->mem.thermalThrottlingOptions |= CLTT_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.thermalthrottlingsupport == 1) {
    setup->mem.thermalThrottlingOptions |= OLTT_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.thermalthrottlingsupport == 3) {
    setup->mem.thermalThrottlingOptions |= CLTT_PECI_EN;
  }

  //
  // thermal memtrip options
  //
  if (SetupData->SocketConfig.MemoryConfig.thermalmemtrip) {
    setup->mem.thermalThrottlingOptions |= MEMTRIP_EN;
  }

  //
  // mem hot options
  //
  if (SetupData->SocketConfig.MemoryConfig.memhotSupport == MEMHOT_OUTPUT_ONLY) {
    setup->mem.thermalThrottlingOptions |= MH_OUTPUT_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.memhotSupport == MEMHOT_INPUT_ONLY) {
    setup->mem.thermalThrottlingOptions |= MH_SENSE_EN;
  } else if (SetupData->SocketConfig.MemoryConfig.memhotSupport == MEMHOT_INPUT_OUTPUT_EN) {
    setup->mem.thermalThrottlingOptions |= (MH_OUTPUT_EN + MH_SENSE_EN);
  }

  //
  // MEMHOT_OUTPUT_ONLY options - hsd s5370942
  //
  if (SetupData->SocketConfig.MemoryConfig.MemhotOutputOnlyOpt == MEMHOT_OUTPUT_ONLY_DIS) {
    setup->mem.memhotOutputOnlyOpt = 0;
  } else if(SetupData->SocketConfig.MemoryConfig.MemhotOutputOnlyOpt == MEMHOT_OUTPUT_ONLY_HIGH) {
    setup->mem.memhotOutputOnlyOpt = 1;
  } else if(SetupData->SocketConfig.MemoryConfig.MemhotOutputOnlyOpt == MEMHOT_OUTPUT_ONLY_HIGH_MED) {
    setup->mem.memhotOutputOnlyOpt = 2;
  } else if(SetupData->SocketConfig.MemoryConfig.MemhotOutputOnlyOpt == MEMHOT_OUTPUT_ONLY_HIGH_MED_LOW) {
    setup->mem.memhotOutputOnlyOpt = 3;
  } else {
    setup->mem.memhotOutputOnlyOpt = MEMHOT_OUTPUT_ONLY_HIGH; //Default = high
  }

  //
  // LRDIMM Module delay
  //
  setup->mem.lrdimmModuleDelay = SetupData->SocketConfig.MemoryConfig.lrdimmModuleDelay;


  //
  // DDR4 SMB CLK
  //
  setup->mem.SpdSmbSpeed = SetupData->SocketConfig.MemoryConfig.smbSpeed;

  //
  // Host RAS Modes
  //
  setup->mem.RASmode &= ~(CH_MLS);
  setup->mem.spareErrTh = SetupData->SocketConfig.MemoryConfig.spareErrTh;
  if (SetupData->SocketConfig.CommonRcConfig.LockStep == LOCKSTEP_ENABLED)
    setup->mem.RASmode |= STAT_VIRT_LOCKSTEP;
  if (SetupData->SocketConfig.MemoryConfig.RankSparing == SPARING_ENABLED){
    setup->mem.RASmode |= RK_SPARE;
    setup->mem.spareRanks = SetupData->SocketConfig.MemoryConfig.multiSparingRanks;
  } else if (SetupData->SocketConfig.CommonRcConfig.MirrorMode == MIRROR_1LM_ENABLED){
    /*4930071: A0 workaround for Mirroring Configs.
    This HSD states that there cannot be channel interleaving if mirroring is enabled*/
    //5331204: Remove A-step mirroring w/a's required for error injection
    /*if (host->var.common.stepping == A0_REV_SKX) {
      DEBUG((EFI_D_INFO, "Forcing to 1Chway for A0 stepping \n"));
      setup->mem.chInter = CH_1WAY;
    }*/
    setup->mem.RASmode |= FULL_MIRROR_1LM;
  } else if (SetupData->SocketConfig.CommonRcConfig.MirrorMode == MIRROR_2LM_ENABLED){
    //5331204: Remove A-step mirroring w/a's required for error injection
    /*if (host->var.common.stepping == A0_REV_SKX) {
      setup->mem.chInter = CH_1WAY;
    }*/
    setup->mem.RASmode |= FULL_MIRROR_2LM;
  }

  //
  // Host Extended RAS Modes
  //

  // Demand Scrub Disable  shall not be used according to b311409
  /*
  if (SetupData->SocketConfig.MemoryConfig.DemandScrubMode == DEMAND_SCRUB_DIS) {
    setup->mem.RASmodeEx &= ~DMNDSCRB_EN;
  } else {
    setup->mem.RASmodeEx |= DMNDSCRB_EN;
  }
  */

  if (SetupData->SocketConfig.MemoryConfig.PatrolScrub == PATROL_SCRUB_DIS){
    setup->mem.RASmodeEx &= ~PTRLSCRB_EN;
  } else {
    setup->mem.RASmodeEx |= PTRLSCRB_EN;
    setup->mem.patrolScrubDuration = SetupData->SocketConfig.MemoryConfig.PatrolScrubDuration;
    setup->mem.patrolScrubAddrMode = SetupData->SocketConfig.MemoryConfig.PatrolScrubAddrMode;
  }

  if (SetupData->SocketConfig.MemoryConfig.SddcPlusOneEn) {
    setup->mem.RASmodeEx |= SDDC_EN;
  } else {
    setup->mem.RASmodeEx &= ~SDDC_EN;
  }

  if (SetupData->SocketConfig.MemoryConfig.ADDDCEn) {
         setup->mem.RASmodeEx |= ADDDC_EN;
  } else {
         setup->mem.RASmodeEx &= ~ADDDC_EN;
  }

  if (SetupData->SocketConfig.MemoryConfig.AdddcErrInjEn) {
         setup->mem.RASmodeEx |= ADDDC_ERR_INJ_EN;
  } else {
         setup->mem.RASmodeEx &= ~ADDDC_ERR_INJ_EN;
  }

  //
  // Leaky bucket configuration options
  //
  setup->mem.leakyBktHi = SetupData->SocketConfig.MemoryConfig.leakyBktHi;
  setup->mem.leakyBktLo = SetupData->SocketConfig.MemoryConfig.leakyBktLo;

#ifndef MINIBIOS_BUILD
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             &PeiVariable
                             );
  ASSERT_EFI_ERROR (Status);

  rcPrintf ((host, "Looking for MirrorRequest\n"));
  VariableSize = sizeof(ADDRESS_RANGE_MIRROR_VARIABLE_DATA);
  Status = PeiVariable->GetVariable(
                           PeiVariable,
                           ADDRESS_RANGE_MIRROR_VARIABLE_REQUEST,
                           &gAddressBasedMirrorGuid,
                           NULL,
                           &VariableSize,
                           &MemoryRequest
                        );
  AddressBasedMirrorData = &MemoryRequest;
  if (Status == EFI_BUFFER_TOO_SMALL) {
    (**PeiServices).AllocatePool(PeiServices,VariableSize,&AddressBasedMirrorData);
    Status = PeiVariable->GetVariable (
                            PeiVariable,
                            ADDRESS_RANGE_MIRROR_VARIABLE_REQUEST,
                            &gAddressBasedMirrorGuid,
                            NULL,
                            &VariableSize,
                            AddressBasedMirrorData
                          );
  }

  if(Status == EFI_SUCCESS) {
    rcPrintf ((host, "MirrorRequest found setting up mirror regions\n"));
    if(AddressBasedMirrorData->MirrorVersion == 1) {
      //Enable partial mirror both mdoes if requested from UEFI method in previous boot
      setup->mem.partialmirror = 1;
      setup->mem.RASmode |= PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM;
      setup->mem.partialMirrorUEFI = 1;
      setup->mem.partialmirrorsad0 = MemoryRequest.MirrorMemoryBelow4GB;
      setup->mem.partialmirrorpercent = MemoryRequest.MirroredAmountAbove4GB;
      setup->mem.partialmirrorsts = MIRROR_STATUS_SUCCESS;
    } else {
      rcPrintf ((host, "Incorrect Mirror Request Version\n"));
      setup->mem.partialmirrorsts = MIRROR_STATUS_VERSION_MISMATCH;
      setup->mem.partialMirrorUEFI = 1;
      setup->mem.partialmirrorsad0 = 0;
      setup->mem.partialmirrorpercent = 0;
    }
  } else {
    //Use setup for configuration
    setup->mem.partialmirrorsts = MIRROR_STATUS_SUCCESS;
    setup->mem.partialMirrorUEFI = 0;
    setup->mem.partialmirrorpercent = 0;
    if (SetupData->SocketConfig.MemoryConfig.partialmirror == PMIRROR_1LM) {
      setup->mem.partialmirror = 1;
      setup->mem.RASmode |= PARTIAL_MIRROR_1LM;
      setup->mem.partialmirrorsts = MIRROR_STATUS_OEM_SPECIFIC_CONFIGURATION;
    }

    if (SetupData->SocketConfig.MemoryConfig.partialmirror == PMIRROR_2LM) {
      setup->mem.partialmirror = 1;
      setup->mem.RASmode |= PARTIAL_MIRROR_2LM;
      setup->mem.partialmirrorsts = MIRROR_STATUS_OEM_SPECIFIC_CONFIGURATION;
    }

    if (SetupData->SocketConfig.MemoryConfig.partialmirrorsad0 == PMIRROR_1LM) {
      setup->mem.partialmirrorsad0 = 1;
      setup->mem.RASmode |= PARTIAL_MIRROR_1LM;
      setup->mem.partialmirrorsts = MIRROR_STATUS_OEM_SPECIFIC_CONFIGURATION;
    }

    if (SetupData->SocketConfig.MemoryConfig.PartialMirrorUefi == PMIRROR_ENABLED) {
      setup->mem.partialmirror = 1;
      setup->mem.partialMirrorUEFI = 1;
      setup->mem.RASmode |= PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM;
      setup->mem.partialmirrorpercent = SetupData->SocketConfig.MemoryConfig.PartialMirrorUefiPercent;
      setup->mem.partialmirrorsts = MIRROR_STATUS_OEM_SPECIFIC_CONFIGURATION;
    }



    for(count=0;count<MAX_PARTIAL_MIRROR;count++)
    {
      //Convert Value entered in 1 GB multiples into 64MB Chunks (1064/64 = 16)
      setup->mem.partialmirrorsize[count] = (SetupData->SocketConfig.MemoryConfig.partialmirrorsize[count])*16;
    }
  }
#endif

  //
  // Rank Disable Mask
  //
  pBuf = &SetupData->SocketConfig.MemoryConfig.sck0ch0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (socket >= 4) {
      setup->mem.socket[socket].ddrCh[ch].rankmask = 0xFF;
    } else {
      for (ch = 0; ch < MAX_CH; ch++) {
        setup->mem.socket[socket].ddrCh[ch].rankmask = (UINT8)*pBuf;
        pBuf++;
      }
    }
  }
  //
  // Update rank mapout data based on rankMask option
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      for (rank = 0; rank < MAX_RANK_CH; rank++) {
        //
        // Convert logical rank to Phy. Rank, DIMM Slot# and DIMM rank#
        // Log. Rank:       0/1/2/3/4/5/6/7
        // 2DPC Phy. Rank:  0/1/2/3/4/5/6/7
        // 3DPC Phy. Rank:  0/1/2/3/4/5/8/9
        //
        if (host->nvram.mem.socket[socket].channelList[ch].maxDimm == 3 && rank > 5) {
          phyRank = 2 + rank;
        } else {
          phyRank = rank;
        }
        dimmRank = phyRank % MAX_RANK_DIMM;
        dimm     = phyRank / MAX_RANK_DIMM;
        //DEBUG ((EFI_D_INFO, "   %d           %d         %d", phyRank, dimmRank, dimm));

        //
        // If rank is not enabled in rankMask, then  flag the rank as
        // mapped out
        //

        if (dimm >= MAX_DIMM) {
          continue;
        }

        if (!(setup->mem.socket[socket].ddrCh[ch].rankmask & (1 << rank))) {
          setup->mem.socket[socket].ddrCh[ch].dimmList[dimm].mapOut[dimmRank] = 1;
        } else {
          setup->mem.socket[socket].ddrCh[ch].dimmList[dimm].mapOut[dimmRank] = 0;
        }
      } // rank
    } // ch
  } // socket
  
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
//  for (socket = 0; socket < MAX_SOCKET; socket++) {
//    if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
//       csrReg = ReadCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD0_UBOX_MISC_REG);
//       if ((csrReg & BIT1) == 0)
//        {
//       		// If cold boot or MRC is not done, clear CMOS Mapout flag.
//           	ClearMapOut = TRUE;
//         }
//      }
//  }
//  if (ClearMapOut)
//   {
//	// Clear CMOS
//  		DEBUG((EFI_D_ERROR, "clear cmos due to coldboot or MRC is not done yet at oemprocmeminit.c\n"));			
//	   SetCmosTokensValue (MEM_MAPOUT, 0x0);
//   }else {
//	//get bad DIMM location from CMOS
//	   Data = GetCmosTokensValue (MEM_MAPOUT);
//   }
  Data = GetCmosTokensValue (MEM_MAPOUT);
  if(Data & 0x80){
	  socket = (Data >> 4) & 0x03;
	  ch = (Data >> 1) & 0x07;
	  dimm = Data & 0x01;
	  DEBUG((EFI_D_ERROR, " map out socket:%x, ch:%x, dimm:%x\n",socket,ch, dimm));
	  for(i=dimm; i<MAX_DIMM; i++)		// for bad DIMM and DIMM(s) behind it
	  {
		  setup->mem.socket[socket].ddrCh[ch].dimmList[i].Disable = 0x01; 
		  for (j = 0; j < MAX_RANK_DIMM ; j++)
		  {
		   setup->mem.socket[socket].ddrCh[ch].dimmList[i].mapOut[j] = 1;
		  }	   
	  }
  }
#endif //#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT

  if (SetupData->SocketConfig.MemoryConfig.eyeDiagram)  setup->mem.options |= DISPLAY_EYE_EN;

  setup->mem.dfxMemSetup.dfxLowMemChannel = SetupData->SocketConfig.MemoryConfig.dfxLowMemChannel;
  // Force channel interleave to 1
  if (setup->mem.dfxMemSetup.dfxLowMemChannel){
    setup->mem.chInter = CH_1WAY;
  }

  if (SetupData->SocketConfig.MemoryConfig.dfxHighAddressStartBitPosition) {
    setup->mem.dfxMemSetup.dfxOptions |= HIGH_ADDR_EN;
    setup->mem.dfxMemSetup.dfxHighAddrBitStart = SetupData->SocketConfig.MemoryConfig.dfxHighAddressStartBitPosition;
  } else {
    setup->mem.dfxMemSetup.dfxHighAddrBitStart = 0;
  }

  setup->mem.dfxMemSetup.dfxLowMemChannel = SetupData->SocketConfig.MemoryConfig.dfxLowMemChannel;

  if (SetupData->SocketConfig.MemoryConfig.crMixedSKU) {
    setup->mem.dfxMemSetup.dfxOptions |= CR_MIXED_SKU;
  } else {
    setup->mem.dfxMemSetup.dfxOptions &= ~CR_MIXED_SKU;
  }

  // Custom Refresh Rate
  if (SetupData->SocketConfig.MemoryConfig.CustomRefreshRateEn) {
    setup->mem.customRefreshRate = SetupData->SocketConfig.MemoryConfig.CustomRefreshRate;
  } else {
    setup->mem.customRefreshRate = 0;
  }

  //capture memory training steps in MRC flow - memFlows
  setup->mem.memFlows = SetupData->SocketConfig.MemoryConfig.memFlows;
  setup->mem.memFlowsExt = SetupData->SocketConfig.MemoryConfig.memFlowsExt;

  //update NGN options
  if (SetupData->SocketConfig.MemoryConfig.LockNgnCsr) {
    setup->mem.optionsNgn |= LOCK_NGN_CSR;
  } else {
    setup->mem.optionsNgn &= ~LOCK_NGN_CSR;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnCmdTime) {
    setup->mem.optionsNgn |= NGN_CMD_TIME;
  } else {
    setup->mem.optionsNgn &= ~NGN_CMD_TIME;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnEccCorr) {
    setup->mem.optionsNgn |= NGN_ECC_CORR;
  } else {
    setup->mem.optionsNgn &= ~NGN_ECC_CORR;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnEccWrChk) {
    setup->mem.optionsNgn |= NGN_ECC_WR_CHK;
  } else {
    setup->mem.optionsNgn &= ~NGN_ECC_WR_CHK;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnEccRdChk) {
    setup->mem.optionsNgn |= NGN_ECC_RD_CHK;
  } else {
    setup->mem.optionsNgn &= ~NGN_ECC_RD_CHK;
  }

  if (SetupData->SocketConfig.MemoryConfig.CrMonetization) {
    setup->mem.optionsNgn |= CR_MONETIZATION;
  } else {
    setup->mem.optionsNgn &= ~CR_MONETIZATION;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnDebugLock) {
    setup->mem.optionsNgn |= NGN_DEBUG_LOCK;
  } else {
    setup->mem.optionsNgn &= ~NGN_DEBUG_LOCK;
  }

  if (SetupData->SocketConfig.MemoryConfig.NgnArsOnBoot) {
    setup->mem.optionsNgn |= NGN_ARS_ON_BOOT;
  } else {
    setup->mem.optionsNgn &= ~NGN_ARS_ON_BOOT;
  }


  setup->mem.crQosConfig = SetupData->SocketConfig.MemoryConfig.crQosConfig;

  //
  // Check MOR to see if we need clear memory
  //
  TcgSetup.MorState = SetupData->SystemConfig.MorState;
// APTIOV_SERVER_OVERRIDE_RC_START
//  if (TcgSetup.MorState){ // Using AMI TCG Module
  if (host->var.common.bootMode != S3Resume) {  //  Fixed S3 failed since options are changed.
// APTIOV_SERVER_OVERRIDE_RC_END
    DEBUG((EFI_D_ERROR, "MEMORY_INIT: MOR Control setup Enabled!\n"));
    VariableSize   = sizeof (UINT8);
    Status = PeiVariable->GetVariable (
                          PeiVariable,
                          MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                          &gEfiMemoryOverwriteControlDataGuid,
                          NULL,
                          &VariableSize,
                          &MorControl
                          );

    if (MOR_CLEAR_MEMORY_VALUE (MorControl) == 0x1) {
      setup->mem.options |= MEMORY_TEST_EN;
      setup->mem.options |= MEMORY_TEST_FAST_BOOT_EN;

      DEBUG((EFI_D_ERROR, "MEMORY_INIT: MOR Control Clear Action requested!\n"));
      DEBUG((EFI_D_ERROR, "MEMORY_INIT: options |= (MEMORY_TEST_EN | MEMORY_TEST_FAST_BOOT_EN) \n"));
    }
  }
// APTIOV_SERVER_OVERRIDE_RC_START
//  } 
// APTIOV_SERVER_OVERRIDE_RC_END
}

/**
**/
VOID
OemGetPlatformSetupValueforKTI(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup             *setup;
  KTI_DFX_CPU_LINK_SETTING    *pKtiDfxCpuLink;
  KTI_DFX_CPU_PHY_SETTING     *pKtiDfxCpuPhy;
  UINT8                       *DfxPerPortOption;
  KTI_CPU_PHY_SETTING         *pKtiCpuPhy;
  KTI_CPU_LINK_SETTING        *pKtiCpuLink;
  UINT8                       *PerPortOption;
  UINT8                       Ctr1, Ctr2, i;
  BOOLEAN                     ForceTo1SConfigMode = FALSE;
  UINT32                      Data32;

  setup = (struct sysSetup *)(&host->setup);

  setup->kti.LegacyVgaSoc      = SetupData->SocketConfig.CsiConfig.LegacyVgaSoc;
  setup->kti.LegacyVgaStack    = SetupData->SocketConfig.CsiConfig.LegacyVgaStack;
  setup->kti.MmioP2pDis        = SetupData->SocketConfig.CsiConfig.MmioP2pDis;
  setup->kti.DebugPrintLevel   = SetupData->SocketConfig.CsiConfig.DebugPrintLevel;
  setup->kti.KtiLinkSpeedMode  = SetupData->SocketConfig.CsiConfig.QpiLinkSpeedMode;
  setup->kti.DegradePrecedence = SetupData->SocketConfig.CsiConfig.DegradePrecedence;
  setup->kti.DirectoryModeEn   = SetupData->SocketConfig.CsiConfig.DirectoryModeEn;

  if (IsSimicsPlatform() == FALSE) {
    GpioGetInputValue (GPIO_SKL_H_GPP_A17,  &Data32);
    if (!Data32){
      ForceTo1SConfigMode  = TRUE;
    }
  }

  setup->kti.KtiLinkSpeed     = SetupData->SocketConfig.CsiConfig.QpiLinkSpeed;
  setup->kti.KtiLinkL0pEn     = SetupData->SocketConfig.CsiConfig.KtiLinkL0pEn;
  setup->kti.KtiLinkL1En      = SetupData->SocketConfig.CsiConfig.KtiLinkL1En;
  setup->kti.KtiFailoverEn    = SetupData->SocketConfig.CsiConfig.KtiFailoverEn;
  setup->kti.KtiLbEn          = SetupData->SocketConfig.CsiConfig.KtiLbEn;
  setup->kti.KtiCrcMode       = SetupData->SocketConfig.CsiConfig.KtiCrcMode;
  setup->kti.SncEn                        = SetupData->SocketConfig.CsiConfig.SncEn;
  setup->kti.IoDcMode                     = SetupData->SocketConfig.CsiConfig.IoDcMode;
  setup->kti.XptPrefetchEn                = SetupData->SocketConfig.CsiConfig.XptPrefetchEn;
  setup->kti.KtiPrefetchEn                = SetupData->SocketConfig.CsiConfig.KtiPrefetchEn;
  setup->kti.KtiCpuSktHotPlugEn           = SetupData->SocketConfig.CsiConfig.QpiCpuSktHotPlugEn;
  setup->kti.KtiCpuSktHotPlugTopology     = SetupData->SocketConfig.CsiConfig.KtiCpuSktHotPlugTopology;
  setup->kti.KtiSkuMismatchCheck          = SetupData->SocketConfig.CsiConfig.KtiSkuMismatchCheck;
  setup->kti.IrqThreshold                 = SetupData->SocketConfig.CsiConfig.IrqThreshold;
  setup->kti.StaleAtoSOptEn               = SetupData->SocketConfig.CsiConfig.StaleAtoSOptEn;
  setup->kti.LLCDeadLineAlloc             = SetupData->SocketConfig.CsiConfig.LLCDeadLineAlloc;

  setup->kti.BusRatio[0] = SetupData->SocketConfig.CsiConfig.BusRatio[0];
#if (MAX_SOCKET > 1)
  setup->kti.BusRatio[1] = SetupData->SocketConfig.CsiConfig.BusRatio[1];
#endif
#if (MAX_SOCKET > 2)
  setup->kti.BusRatio[2] = SetupData->SocketConfig.CsiConfig.BusRatio[2];
#endif
#if (MAX_SOCKET > 3)
  setup->kti.BusRatio[3] = SetupData->SocketConfig.CsiConfig.BusRatio[3];
#endif
#if (MAX_SOCKET > 4)
  setup->kti.BusRatio[4] = SetupData->SocketConfig.CsiConfig.BusRatio[4];
#endif
#if (MAX_SOCKET > 5)
  setup->kti.BusRatio[5] = SetupData->SocketConfig.CsiConfig.BusRatio[5];
#endif
#if (MAX_SOCKET > 6)
  setup->kti.BusRatio[6] = SetupData->SocketConfig.CsiConfig.BusRatio[6];
#endif
#if (MAX_SOCKET > 7)
  setup->kti.BusRatio[7] = SetupData->SocketConfig.CsiConfig.BusRatio[7];
#endif

  //
  // Dfx options
  //
  setup->kti.DfxParm.DfxHaltLinkFailReset            = SetupData->SocketConfig.CsiConfig.DfxHaltLinkFailReset;
  setup->kti.DfxParm.DfxKtiMaxInitAbort              = SetupData->SocketConfig.CsiConfig.DfxKtiMaxInitAbort;
  setup->kti.DfxParm.DfxLlcShareDrdCrd               = SetupData->SocketConfig.CsiConfig.DfxLlcShareDrdCrd;
  setup->kti.DfxParm.DfxBiasFwdMode                  = SetupData->SocketConfig.CsiConfig.DfxBiasFwdMode;
  setup->kti.DfxParm.DfxSnoopFanoutEn                = SetupData->SocketConfig.CsiConfig.DfxSnoopFanoutEn;
  setup->kti.DfxParm.DfxHitMeEn                      = SetupData->SocketConfig.CsiConfig.DfxHitMeEn;
  setup->kti.DfxParm.DfxFrcfwdinv                    = SetupData->SocketConfig.CsiConfig.DfxFrcfwdinv;
  setup->kti.DfxParm.DfxDbpEnable                    = SetupData->SocketConfig.CsiConfig.DfxDbpEnable;
  setup->kti.DfxParm.DfxOsbEn                        = SetupData->SocketConfig.CsiConfig.DfxOsbEn;
  setup->kti.DfxParm.DfxHitMeRfoDirsEn               = SetupData->SocketConfig.CsiConfig.DfxHitMeRfoDirsEn;
  setup->kti.DfxParm.DfxGateOsbIodcAllocEn           = SetupData->SocketConfig.CsiConfig.DfxGateOsbIodcAllocEn;
  setup->kti.DfxParm.DfxDualLinksInterleavingMode    = SetupData->SocketConfig.CsiConfig.DfxDualLinksInterleavingMode;
  setup->kti.DfxParm.DfxSystemDegradeMode             = SetupData->SocketConfig.CsiConfig.DfxSystemDegradeMode;
  setup->kti.DfxParm.DfxD2cEn                          = SetupData->SocketConfig.CsiConfig.DfxD2cEn;
  setup->kti.DfxParm.DfxD2kEn                          = SetupData->SocketConfig.CsiConfig.DfxD2kEn;
  setup->kti.DfxParm.DfxVn1En                          = SetupData->SocketConfig.CsiConfig.DfxVn1En;


  //
  // DFX Per port option
  //
  DfxPerPortOption = ((UINT8 *)(&SetupData->SocketConfig.CsiConfig.DfxKtiCpuPerPortStartTag) + 1);
  for (Ctr1 = 0; Ctr1 < MAX_SOCKET; ++Ctr1) {
    for (Ctr2 = 0; Ctr2 < MAX_KTI_PORTS; ++Ctr2) {
      i = Ctr1 * MAX_KTI_PORTS * KTI_SETUP_OPTIONS + Ctr2 * KTI_SETUP_OPTIONS;

      pKtiDfxCpuLink = (KTI_DFX_CPU_LINK_SETTING *) &(host->setup.kti.DfxCpuCfg[Ctr1].Link[Ctr2]);
      pKtiDfxCpuPhy  = (KTI_DFX_CPU_PHY_SETTING *) &(host->setup.kti.DfxCpuCfg[Ctr1].Phy[Ctr2]);

      pKtiDfxCpuLink->DfxCrcMode      = DfxPerPortOption[i] & 0x3;
      pKtiDfxCpuLink->DfxL0pEnable    = DfxPerPortOption[i + 1] & 0x3;
      pKtiDfxCpuLink->DfxL1Enable     = DfxPerPortOption[i + 2] & 0x3;
      pKtiDfxCpuPhy->DfxKtiFailoverEn = DfxPerPortOption[i + 3] & 0x3;
    }
  }

  //
  // Per port option
  //
  PerPortOption = ((UINT8 *)(&SetupData->SocketConfig.CsiConfig.KtiCpuPerPortStartTag) + 1);
  for (Ctr1 = 0; Ctr1 < MAX_SOCKET; ++Ctr1) {
    for (Ctr2 = 0; Ctr2 < MAX_KTI_PORTS; ++Ctr2) {

      pKtiCpuLink = (KTI_CPU_LINK_SETTING *) &(host->setup.kti.PhyLinkPerPortSetting[Ctr1].Link[Ctr2]);
      pKtiCpuPhy = (KTI_CPU_PHY_SETTING *) &(host->setup.kti.PhyLinkPerPortSetting[Ctr1].Phy[Ctr2]);

      if (ForceTo1SConfigMode == TRUE) {
        pKtiCpuLink->KtiPortDisable = TRUE;                                                         // Disable ports of all sockets
      } else {
        pKtiCpuLink->KtiPortDisable = (*(PerPortOption++)) & 0x1;                                   // This option is per-port only
      }

      if (SetupData->SocketConfig.CsiConfig.QpiLinkSpeed == FREQ_PER_LINK) {                   // Implies "Per Link Setting" 7 = FREQ_PER_LINK
        pKtiCpuPhy->KtiLinkSpeed = (*(PerPortOption++)) & 0x7;                                      // Use per port option
      } else {
        pKtiCpuPhy->KtiLinkSpeed =  SetupData->SocketConfig.CsiConfig.QpiLinkSpeed & 0x7;                // Use system wide option
        PerPortOption++;
      }

      if (SetupData->SocketConfig.CsiConfig.KtiLinkVnaOverride == KTI_LL_VNA_SETUP_PER_LINK) { // Implies "Per Link Setting"
        pKtiCpuLink->KtiLinkVnaOverride = (*(PerPortOption++)) & 0x7f;                              // Use per port option
      } else {
        pKtiCpuLink->KtiLinkVnaOverride = SetupData->SocketConfig.CsiConfig.KtiLinkVnaOverride & 0x7f; // Use system wide option
        PerPortOption++;
      }
    }
  }
}

VOID
OemGetPlatformSetupValueforFPGA(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData 
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup             *setup;
  UINT8                       Soc;

  setup = (struct sysSetup *)(&host->setup);
  for (Soc = 0; Soc < MAX_SOCKET; Soc ++) {
    if (SetupData->SocketFpgaConfig.FpgaSetupEnabled & (1 << Soc)) {
      setup->kti.KtiFpgaEnable[Soc] = KTI_ENABLE;
    } else {
      setup->kti.KtiFpgaEnable[Soc] = KTI_DISABLE;
    }
  }
}

//
// Function Implementations
//
/**
**/
VOID
OemGetPlatformSetupValue(
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  OemGetPlatformSetupValueforKTI(host, SetupData);
  OemGetPlatformSetupValueforMRC(host, SetupData);
  OemGetPlatformSetupValueforCommonRc(host, SetupData);
  OemGetPlatformSetupValueforCpu(host, SetupData);
  OemGetPlatformSetupValueforFPGA(host, SetupData);
}
#endif // MINIBIOS_BUILD


/**

  OEM hook to return Memory Riser Information

  @param host - pointer to sysHost structure on stack

  @retval SUCCESS

--*/
UINT32
OemMemRiserInfo (
    struct sysHost             *host
  )
{
  struct sysSetup       *setup;
  UINT8                 socket;
  UINT8                 ch;
  setup = (struct sysSetup *)(&host->setup);
  //
  // 1 Dimm Vref per Channel
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.compId = DCP_ISL9072X;
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.address.controller    = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.address.strapAddress  = 0x06;
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.address.deviceType    = DTI_DCP_FIVE;
      setup->mem.socket[socket].ddrCh[ch].vrefDcp.address.busSegment = (ch & BIT1) >> 1;
    }
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      setup->mem.socket[socket].ddrCh[ch].numDimmSlots = 2;
    }
  }

  return SUCCESS;
}

/**
Routine Description:
  OEM Hook for resetting the platform.

@param host      - Pointer to sysHost, the system host (root) structure
@param ResetType - Warm == 0x02, Cold == 0x04

@retval None

**/
VOID
OemIssueReset(
  struct sysHost *host,
  UINT8          ResetType
)
{
#ifndef MINIBIOS_BUILD
  PCH_RESET_PPI    *PchResetPpi = NULL;
  EFI_PEI_SERVICES **PeiServices;
  EFI_STATUS       Status;

  PeiServices = (EFI_PEI_SERVICES **)host->var.common.oemVariable;

  Status = (*PeiServices)->LocatePpi(PeiServices,
                                     &gPchResetPpiGuid,
                                     0,
                                     NULL,
                                     (VOID **)&PchResetPpi
                                     );

  if (Status == EFI_SUCCESS) {
    if (ResetType == POST_RESET_POWERGOOD) {
      Status = PchResetPpi->Reset(PchResetPpi, ColdReset);
    } else if (ResetType == POST_RESET_WARM) {
      Status = PchResetPpi->Reset(PchResetPpi, WarmReset);
    }
    ASSERT_EFI_ERROR (Status);
  } else {
    //Reset PPI failed to initialize, issue reset via direct IO
    if (ResetType == POST_RESET_POWERGOOD) {
      OutPort8(host, 0xcf9, 0x0e);
    } else if (ResetType == POST_RESET_WARM) {
      OutPort8(host, 0xcf9, 0x06);
    }
  }

#else
  //Reset PPI not supported in MINIBIOS
  if (ResetType == POST_RESET_POWERGOOD) {
    OutPort8(host, 0xcf9, 0x0e);
  } else if (ResetType == POST_RESET_WARM) {
    OutPort8(host, 0xcf9, 0x06);
  }
#endif //MINIBIOS_BUILD
}

/*++

Routine Description:

  OEM hook to enable ADR related platform specific features.
  This routine enables batterybacked feature on all the supported channels.
  This routine also sets up CPLD into one of two mode (battery backed mode or C2F mode)
  Note that CPLD setup code is specific to Emerald Point platform.
  This routine needs to be updated with any other board specific changes related to ADR.

Arguments:

  host    - Pointer to SysHost structure.
  SetupData - SetupData

Returns:

  None

--*/

#ifndef MINIBIOS_BUILD
VOID OemStoragePolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  struct sysSetup       *setup;
#ifdef NVMEM_FEATURE_EN
  UINT8                 socket;
  UINT8                 ch;
#endif

  setup = (struct sysSetup *)(&host->setup);

  setup->mem.ADREn = SetupData->SocketConfig.MemoryConfig.ADREn;
  setup->mem.LegacyADRModeEn = SetupData->SocketConfig.MemoryConfig.LegacyADRModeEn;
  setup->mem.check_pm_sts = SetupData->SocketConfig.MemoryConfig.check_pm_sts;
  setup->mem.check_platform_detect = SetupData->SocketConfig.MemoryConfig.check_platform_detect;

#ifdef NVMEM_FEATURE_EN
  setup->mem.ADRDataSaveMode = SetupData->SocketConfig.MemoryConfig.ADRDataSaveMode;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if (SetupData->SocketConfig.MemoryConfig.ADREn && SetupData->SocketConfig.MemoryConfig.ADRDataSaveMode == ADR_BBU) {
        setup->mem.socket[socket].ddrCh[ch].batterybacked = 1;
      }
      else {
        setup->mem.socket[socket].ddrCh[ch].batterybacked = 0;
      }
    }
  }
#endif
#ifdef MEM_NVDIMM_EN
  if (SetupData->SocketConfig.MemoryConfig.ADRDataSaveMode == ADR_NVDIMM){
    setup->mem.check_pm_sts = 0;
    setup->mem.check_platform_detect = 0;
    setup->mem.eraseArmNVDIMMS = SetupData->SocketConfig.MemoryConfig.eraseArmNVDIMMS;
    setup->mem.restoreNVDIMMS = SetupData->SocketConfig.MemoryConfig.restoreNVDIMMS;
  }
  else {
    setup->mem.eraseArmNVDIMMS = 0;
    setup->mem.restoreNVDIMMS = 0;
  }
  setup->mem.interNVDIMMS = SetupData->SocketConfig.MemoryConfig.interNVDIMMS;
#endif
}

VOID
OemForceSetupDefault (
    struct sysHost             *host
  )
{
  UINT8                           SocketId = 0;
  UINT32                          Data32   = 0;
  UINT32                          ForceToCmos = 0;
  Data32 = ReadCpuPciCfgEx  (host, SocketId, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG);
  ForceToCmos = (Data32 & FORCE_CMOS_BAD) >> FORCE_CMOS_SHIFT;
  if (ForceToCmos) {
    DEBUG((EFI_D_INFO, "Advanced Debug Jumper set - Force Setup Defaults\n"));
    //
    // Clear Cmos Bad bit here.
    //
    Data32 &= ~FORCE_CMOS_BAD;
    WriteCpuPciCfgEx (host, SocketId, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG, Data32);
    MmioWrite16 (
      MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B,
      B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS
    );
  }
}

VOID
OemNmBootPolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData,
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  EFI_BOOT_MODE              BootMode
  )
{
#ifndef SKIP_OEMPLATFORMSETUP
#if SPS_SUPPORT
  SPS_MEFS1      MeFs1;
  SPS_NMFS       NmFs;
  UINT8          CoresDisabled, Socket;
  UINT32         Mask;
  UINT32         Timeout = 500;
  BOOLEAN        PwrOptBoot = FALSE;
  UINT8          Cores2Disable = 0;
  EFI_PEI_SERVICES  **PeiServices;
  struct sysSetup       *setup;
// APTIOV_SERVER_OVERRIDE_RC_START : ADR Not working in Release mode
#if defined ADR_RELEASE_MODE_FAILURE_WORKAROUND && ADR_RELEASE_MODE_FAILURE_WORKAROUND == 1  
  UINT16 NmBootModeTimeOutCount = 0;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : ADR Not working in Release mode

  setup = (struct sysSetup *)(&host->setup);
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  //
  // If it is SPS firmware running in ME and Node Manager is enabled
  // apply NM boot time policy, i.e. performance vs power optimized boot path
  // and processor cores disabling.
  // Note that defaults for PwrOptBoot and Cores2Disable are neutral so
  // for non SPS firmware or ME error BIOS defaults are not changed.
  //
  if (BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME)
  {
     DEBUG((EFI_D_INFO, "[SPS] Skip cores reconfiguration on S3 or S4 resume\n"));
     PwrOptBoot = SetupData->MeRcConfig.NmPwrOptBoot;
     Cores2Disable = SetupData->MeRcConfig.NmCores2Disable;
  }
  else
  {
    while (1)
    {
      if (!MeTypeIsSps()) {
        // do not proceed for non-SPS firmware
        DEBUG((EFI_D_WARN,"[ME] Non SPS firmware\n"));
        break;
      }
      MeFs1.UInt32 = MmPci32(0, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, SPS_REG_MEFS1);
      if (MeFs1.Bits.ErrorCode != 0)
      {
        DEBUG((EFI_D_ERROR, "[SPS] ERROR: Invalid ME state (MEFS1: 0x%08X)\n", MeFs1.UInt32));
        break;
      }
      NmFs.UInt32 = MmPci32(0, ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, SPS_REG_NMFS);
      if (NmFs.Bits.NmEnabled)
      {
        if (MeFs1.Bits.OperatingMode == MEFS1_OPMODE_SPS)
        {
          // APTIOV_SERVER_OVERRIDE_RC_START : ADR Not working in Release mode
#if defined ADR_RELEASE_MODE_FAILURE_WORKAROUND && ADR_RELEASE_MODE_FAILURE_WORKAROUND == 1
          DEBUG((DEBUG_INFO, "[SPS] Waiting for ME InitComplete\n"));
          NmBootModeTimeOutCount = ADR_SPS_NM_BOOTMODE_SET_DELAY * 1000; // Max wait time: ADR_SPS_NM_BOOTMODE_SET_DELAY Seconds
          while (!(((MeFs1.Bits.CurrentState == MEFS1_CURSTATE_NORMAL) && (MeFs1.Bits.InitComplete == 1)) || (NmBootModeTimeOutCount == 0))) {
            MeFs1.UInt32 = MmPci32(0, ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, SPS_REG_MEFS1);
            MicroSecondDelay(1000);
            NmBootModeTimeOutCount--;
          }
          // Read NmFs register after ME InitComplete
          NmFs.UInt32 = MmPci32(0, ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, SPS_REG_NMFS);
#endif
          // APTIOV_SERVER_OVERRIDE_RC_END : ADR Not working in Release mode
#if ME_TESTMENU_FLAG
          if (SetupData->MeRcConfig.NmPwrOptBootOverride)
          {
            PwrOptBoot = SetupData->MeRcConfig.NmPwrOptBoot;
          }
          else
#endif
          {
            PwrOptBoot = !NmFs.Bits.PerfOptBoot;
          }
#if ME_TESTMENU_FLAG
          if (SetupData->MeRcConfig.NmCores2DisableOverride)
          {
            Cores2Disable = SetupData->MeRcConfig.NmCores2Disable;
          }
          else
#endif
          {
            Cores2Disable = (UINT8)NmFs.Bits.Cores2Disable;
          }
          DEBUG((EFI_D_INFO, "[SPS] NM firmware detected in ME, "
                             "boot mode: %d, cores to disable: %d (NMFS: 0x%08X)\n",
                             !PwrOptBoot,  Cores2Disable, NmFs.UInt32));
        }
        else
        {
          DEBUG((EFI_D_WARN,"[SPS] Non SPS firmware in ME (MEFS1: 0x%08X, NMFS: 0x%08X)\n", MeFs1.UInt32, NmFs.UInt32));
        }
        break;
      }
      if (MeFs1.Bits.CurrentState > MEFS1_CURSTATE_RECOVERY)
      {
        DEBUG((EFI_D_INFO,"[SPS] NM not enabled in ME (MEFS1: 0x%08X, NMFS: 0x%08X)\n", MeFs1.UInt32, NmFs.UInt32));
        break;
      }
      if (Timeout-- == 0)
      {
       DEBUG((EFI_D_ERROR, "[SPS] ERROR: Timeout waiting for ME (MEFS1: 0x%08X)\n", MeFs1.UInt32));
       break;
      }
      MicroSecondDelay(1000);
    }
  }
  //
  // If NM requests power optimized boot disable parallel memory initialization.
  //
  if (PwrOptBoot)
  {
    setup->mem.options &= ~MULTI_THREAD_MRC_EN;
  }
  //
  // If NM requests disabling cores compare it with BIOS settings,
  // for each socket disable max(BIOS settings, NM request) cores.
  //
  if (Cores2Disable > 0 && Cores2Disable < 8 * sizeof(setup->cpu.CoreDisableMask[0]))
  {
    for (Socket = 0;
         Socket < sizeof(setup->cpu.CoreDisableMask)/sizeof(setup->cpu.CoreDisableMask[0]);
         Socket++)
    {
      CoresDisabled = 0;
      Mask = setup->cpu.CoreDisableMask[Socket];
      while (Mask != 0)
      {
        if (Mask & 1)
        {
          CoresDisabled++;
        }
        Mask >>= 1;
      }
      Mask = 0x00000002; // Start with bit 1, do not disable core 0
      while (CoresDisabled < Cores2Disable && Mask)
      {
        if (!(setup->cpu.CoreDisableMask[Socket] & Mask))
        {
          setup->cpu.CoreDisableMask[Socket] |= Mask;
          CoresDisabled++;
        }
        Mask <<= 1;
      }
    } // for (Socket...)
  }
#endif // SPS_SUPPORT
#endif //!SKIP_OEMPLATFORMSETUP
}


VOID
OemSecurityPolicy (
  struct sysHost             *host,
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 *SetupData,
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  EFI_BOOT_MODE              BootMode
  )
{
#ifndef SKIP_OEMPLATFORMSETUP
#ifdef LT_FLAG
  EFI_PEI_CPU_IO_PPI             *CpuIo;
  UINT8                          Data8;
  UINT8                          Data8_Save;
  EFI_PEI_SERVICES               **PeiServices;
  PCH_RESET_PPI                  *PchResetPpi = NULL;
  EFI_STATUS                     Status;

  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  CpuIo = (*PeiServices)->CpuIo;
  CpuIo->IoWrite8 (PeiServices, CpuIo, 0x70, CMOS_LTSX_OFFSET);
  Data8 = CpuIo->IoRead8 (PeiServices, CpuIo, 0x71);
  Data8_Save = Data8 ;
  Data8 &= (~ (BIT4 | BIT5));

  DEBUG((EFI_D_ERROR, "(Setup)ProcessorLtsxEnable = %d\n", SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorLtsxEnable));
  
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Installs PPI to signal the TCG module to skip TPM Startup if
  //
  //the TXT policy specified in the FIT (CMOS BITS) is set to enabled.  
  if (((*(volatile UINT32 *)(UINTN)LT_SPAD_EXIST))&& (*(volatile UINT32 *)(UINTN)TXT_ACM_STATUS_HI) & BIT31)
  {
  	Status = (**PeiServices).InstallPpi(PeiServices, gPpiList);
    if (Status == EFI_SUCCESS)
    	DEBUG((EFI_D_ERROR, "Disable TPM Startup: %r\n", Status));
    else
    {
    	DEBUG((EFI_D_ERROR, "Warning : Enable TPM Startup and TPM Startup command will be sent again from TCG module : %r\n", Status));
    	ASSERT_EFI_ERROR (Status);
    }
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Installs PPI to signal the TCG module to skip TPM Startup if
  //                                the TXT policy specified in the FIT (CMOS BITS) is set to enabled.    
  //
  if ((SetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorLtsxEnable)) {
      Data8 |= BIT4 | BIT5;
  }
  DEBUG((EFI_D_ERROR, "CMOS_LTSX_OFFSET(%02x)= 0x%02x: for LT/TPM policy\n", CMOS_LTSX_OFFSET, Data8));
  if( (Data8_Save & (BIT4+BIT5)) != (Data8 & (BIT4+BIT5)) ) {
    CpuIo->IoWrite8 (PeiServices, CpuIo, 0x70, CMOS_LTSX_OFFSET); // Added because DEBUG now checks CMOS and alters port 0x70
    CpuIo->IoWrite8 (PeiServices, CpuIo, 0x71, Data8);    // sync up FITEGN cmos with setup option
    //
    // Do this powergood reset only if we're not in HSX PO
    //  where a bootscript is needed to start the CPU
    //
    DEBUG((EFI_D_ERROR, "Sync up LT Policy for : do powergood reset!\n"));
    Status = (*PeiServices)->LocatePpi(PeiServices,
                                     &gPchResetPpiGuid,
                                     0,
                                     NULL,
                                     (VOID **)&PchResetPpi
                                     );
    if (Status == EFI_SUCCESS) {
      Status = PchResetPpi->Reset(PchResetPpi, ColdReset);                 // do powergood reset
    }
  }
#endif //LT_FLAG
#endif //!SKIP_OEMPLATFORMSETUP
}

VOID
OemSaveMemNvram (
  struct sysHost             *host
  )
{
  UINT32            VariableSize;
  UINTN             VarAttrib;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  CHAR16            EfiMemoryConfigVariable[] = L"MemoryConfig0";
  UINT32            RemainingSize;
  VOID              *NvramPtr = NULL;
  UINTN             *CompressTable = NULL;
  UINTN             CompressTableSize;
  SCRATCH_DATA      ScratchData;
  UINT32            ScratchDataSize;
  UINT8*            CompressedInBytes;
  EFI_PEI_SERVICES  **PeiServices;
  EFI_STATUS       Status;
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  if ((host->setup.mem.options & ATTEMPT_FAST_BOOT) || (host->setup.mem.options & ATTEMPT_FAST_BOOT_COLD) || (host->var.common.bootMode == S3Resume)) {
    //
    // Obtain variable services
    //
    Status = (*PeiServices)->LocatePpi (
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0,
                  NULL,
                  &PeiVariable
                  );
    ASSERT_EFI_ERROR (Status);

    VarAttrib = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    RemainingSize = sizeof(host->nvram);
    NvramPtr = &(host->nvram);
    DEBUG((EFI_D_INFO, "OemInitializePlatformData(): RemainingSize: %d\n", RemainingSize));
    DEBUG((EFI_D_INFO, "    NvramPtr is at address: 0x%x\n", NvramPtr));

    while(RemainingSize > 0  && !EFI_ERROR(Status)){
      VariableSize = RemainingSize;
      //
      // Reconstruct the s3 nvram variable
      //
      CompressTableSize = 0;
      Status = PeiVariable->GetVariable (
                  PeiVariable,
                  EfiMemoryConfigVariable,
                  &gEfiMemoryConfigDataGuid,
                  (UINT32*)&VarAttrib,
                  &CompressTableSize,
                  NULL
                  );

      //
      //Allocate buffer and get comressed table from NVRAM
      //
      if(Status == EFI_BUFFER_TOO_SMALL)
      {
        CompressTable = AllocatePool(CompressTableSize);
        if(CompressTable == NULL)
        {
            DEBUG((EFI_D_ERROR,"Compress Table is NULL\n"));
            ASSERT(FALSE);
            Status = EFI_OUT_OF_RESOURCES;
            continue;
        }

        Status = PeiVariable->GetVariable (
                  PeiVariable,
                  EfiMemoryConfigVariable,
                  &gEfiMemoryConfigDataGuid,
                  (UINT32*)&VarAttrib,
                  &CompressTableSize,
                  CompressTable
                  );
      }


    if (!EFI_ERROR(Status) && CompressTable != NULL)
      {
        ScratchDataSize = sizeof(ScratchData);
        CompressedInBytes = (UINT8*) CompressTable;
        VariableSize = CompressedInBytes[4] + (CompressedInBytes[5] << 8) + (CompressedInBytes[6] << 16) + (CompressedInBytes[7] << 24);

        Status = Decompress (CompressTable, *(UINT32 *)(CompressTable) + 8, NvramPtr,
                     VariableSize, &ScratchData, sizeof (SCRATCH_DATA), 1);
        //DEBUG((EFI_D_INFO, "  Status from getting data from S3 NVRAM after Decompress: %r \n", Status));
      }

      if (CompressTable!= NULL) {
        FreePool(CompressTable);
        CompressTable = NULL;
      }
      //
      // Increment pointers, Variable Name, and remaining size
      //
      EfiMemoryConfigVariable[12]++;  // Increment the number portion of the string
      NvramPtr = (VOID*)((UINT8*) NvramPtr + VariableSize);
      RemainingSize -= VariableSize;  // Decrement the actual size of the variable received
      //DEBUG((EFI_D_INFO, "   Variable# from EfiMemoryConfigVariable[12]: %c, Current RemainingSize is: %x\n",  EfiMemoryConfigVariable[12], RemainingSize));
    }
  }
}


/**

This routine reads the gEfiPrevBootNGNDimmCfgVariable and updates the input variable with the data.
This will contain empty data in the first boot after flash

    @param host = pointer to sysHost structure
    @param prevBootNGNDimmCfg = pointer to the structure to be filled
**/
EFI_STATUS
UpdatePrevBootNGNDimmCfg(
    struct sysHost             *host,
    struct prevBootNGNDimmCfg  *prevBootNGNDimmData)

{

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  EFI_PEI_SERVICES  **PeiServices;
  EFI_STATUS       Status;
  UINTN  VariableSize;

  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                &PeiVariable
                );
  ASSERT_EFI_ERROR (Status);

  VariableSize   = sizeof (struct prevBootNGNDimmCfg);
  Status = PeiVariable->GetVariable (
                        PeiVariable,
                        L"PrevBootNGNDimmCfg",
                        &gEfiPrevBootNGNDimmCfgVariableGuid,
                        NULL,
                        &VariableSize,
                        prevBootNGNDimmData
                        );

  return Status;
}

/**

This routine reads the gEfiVirtualLockstepVariable and updates the input variable with the data.
This will contain empty data in the first boot after flash

    @param host = pointer to sysHost structure

    @retval = EFI_STATUS - SUCCESS or FAILURE
**/
EFI_STATUS
UpdatePrevBootVLSInfo(
    struct sysHost             *host
    )

{

  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  EFI_PEI_SERVICES                      **PeiServices;
  VIRTUAL_LOCKSTEP_VARIABLE_DATA        VLSInfo;
  VIRTUAL_LOCKSTEP_VARIABLE_DATA        *vlsInfoVariablePtr;
  UINTN                                 VLSVarSize;
  EFI_STATUS                            Status;

  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  DEBUG ((EFI_D_ERROR, "UpdatePrevBootVLSInfo\n"));
  Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                &PeiVariable
                );
  ASSERT_EFI_ERROR (Status);

  VLSVarSize = sizeof(VIRTUAL_LOCKSTEP_VARIABLE_DATA);
  Status = PeiVariable->GetVariable (
                        PeiVariable,
                        VIRTUAL_LOCKSTEP_VARIABLE_NAME,
                        &gEfiVirtualLockstepGuid,
                        NULL,
                        &VLSVarSize,
                        &VLSInfo
                        );

  if (EFI_ERROR(Status)) {
    vlsInfoVariablePtr = NULL;
  } else {
    vlsInfoVariablePtr = &VLSInfo;
  }

  if (vlsInfoVariablePtr != NULL) {
    host->var.mem.VLSPrevBoot = VLSInfo.VirtualLockstepEstablished;
  } else{
    host->var.mem.VLSPrevBoot = 0;
    //host structure new variable = FALSE;
  }

  return Status;
}
#endif // MINIBIOS_BUILD

#if SMCPKG_SUPPORT	//SMCPKG_SUPPORT++
VOID
OemIntializeIpmi(struct sysHost  *host){
    EFI_PEI_SERVICES  **PeiServices;
    PEI_IPMI_TRANSPORT_PPI   *IpmiTransPpi = NULL;
    PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
    (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, &IpmiTransPpi);
    host->var.common.ipmiTransPpi = (UINT32)IpmiTransPpi;
}
#endif			//SMCPKG_SUPPORT++

/**

  MRC wrapper code.
  Initialize host structure with OEM specific setup data

    @param host = pointer to sysHost structure

**/
VOID
OemInitializePlatformData (
    struct sysHost             *host
  )
{
#ifndef MINIBIOS_BUILD
  EFI_STATUS        Status;
  EFI_BOOT_MODE     BootMode;
  EFI_PEI_SERVICES  **PeiServices;
#ifndef SKIP_OEMPLATFORMSETUP
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA        SetupData;
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
#endif
#endif
  struct sysSetup *setup ;

  setup = (struct sysSetup *)&host->setup;

#ifndef MINIBIOS_BUILD
  PchAcpiBaseGet(&host->var.common.pmBase);
#else
  // APTIOV_SERVER_OVERRIDE_RC_START : Use SDL token for PM base address.
  host->var.common.pmBase = PM_BASE_ADDRESS;
  // APTIOV_SERVER_OVERRIDE_RC_END : Use SDL token for PM base address.
#endif //MINIBIOS_BUILD

#ifndef MINIBIOS_BUILD
  PchPwrmBaseGet(&host->var.common.pwrmBase);
#endif //MINIBIOS_BUILD

  host->nvram.common.platformType = OemGetPlatformType(host);

#ifndef MINIBIOS_BUILD
  Status = PcdSet8S (PcdPlatformType, host->nvram.common.platformType);
  ASSERT_EFI_ERROR (Status);
  //
  // Report Status Code EFI_SW_PS_PC_INSTALL_PEI_MEMORY
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_PEI_SERVICE  | EFI_SW_PS_PC_INSTALL_PEI_MEMORY)
    );

  //
  // Determine boot mode
  //
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  ASSERT_EFI_ERROR(Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    host->var.common.bootMode = S3Resume;
  } else {
    host->var.common.bootMode = NormalBoot;
  }

#ifndef SKIP_OEMPLATFORMSETUP
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  ZeroMem(&SetupData, sizeof(INTEL_SETUP_DATA) );
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  Status = GetEntireConfig(&SetupData);

  if (PcdGetBool(PcdDfxAdvDebugJumper))  {
    OemForceSetupDefault(host);
  }
  OemGetPlatformSetupValue (host, &SetupData);
  OemNmBootPolicy (host, &SetupData, BootMode);
//
// Begin of HSD#5385129- Opal City platform shutdown at POST code 0x06 then follow by 0x9. with beep sounds when try to boot up with different LBG parts
//
  if(((host->nvram.common.platformType)==TypeOpalCitySTHI) && (PcdGet16(PcdOemSkuSubBoardID) == TypePlatformOpalCityCPV)) { // TypePlatformOpalCityCPV does not support Txt
    DEBUG ((EFI_D_ERROR, "Warnning - TypePlatformOpalCityCPV does not support Txt\n"));
  } else {
  OemSecurityPolicy (host, &SetupData, BootMode);
  }
//
// End of HSD#5385129
//  
  OemStoragePolicy(host, &SetupData);
#endif
  if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    MemSetLocal ((UINT8 *) &host->nvram.mem, 0, sizeof (struct memNvram));
  } else {
    OemSaveMemNvram (host);
  }
#else
  //
  // Initialize platform-specific setup data below
  //
  setup->common.sysHostBufferPtr = 0;           // zero:  Use default memory buffer address at 1MB
                                                // non-zero : OEM-specified 32-bit buffer address
  host->var.common.bootMode = NormalBoot;

  //
  // Example of override CPU feature related parameters
  //
  // setup->cpu.dcuModeSelect = 0;                      // 0 = keep HW default DCU_MODE (32KB 8-way)
  //
  // Get Platform Setup and Update host structure
  //
#endif // MINIBIOS_BUILD

#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
  OemIntializeIpmi(host);
#endif   				//SMCPKG_SUPPORT++
  OemMemRASHook(host, 0);
  OemMemRiserInfo(host);
  host->var.common.memHpSupport = 1;
  OemGetMeRequestedSegmentSize (host);
 
// APTIOV_SERVER_OVERRIDE_RC_START  
  AmiOemInitializePlatformData (host);
// APTIOV_SERVER_OVERRIDE_RC_END

}

/**

  OemPreMemoryInit  - OEM hook pre memory init

  @param host  - Pointer to sysHost

**/
UINT32
OemPreMemoryInit (
  PSYSHOST host
  )
{
#ifndef MINIBIOS_BUILD
  SYSTEM_CONFIGURATION                  TotalSystemConfiguration;
  SYSTEM_CONFIGURATION                  *TotalSystemConfigurationPtr = &TotalSystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  UINTN                                 VariableSize;
  EFI_STATUS                            Status;
  EFI_PEI_SERVICES                      **PeiServices;

  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  //
  // Locate Variable PPI
  //
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0,
                  NULL,
                  &PeiVariable
                  );

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = PeiVariable->GetVariable (
                        PeiVariable,
                        // APTIOV_SERVER_OVERRIDE_RC_START
                        L"IntelSetup",
                        // APTIOV_SERVER_OVERRIDE_RC_END
                        &gEfiSetupVariableGuid,
                        NULL,
                        &VariableSize,
                        TotalSystemConfigurationPtr
                        );

  //
  // BIOS Guard Initialization
  //
  BiosGuardInit (PeiServices, TotalSystemConfigurationPtr, host);
#endif // MINIBIOS_BUILD
  //
  // OEM specific Cleanup and settings required before memory init
  //

  //
  // OEM specific handling Errors from previous boot
  //

  return SUCCESS;
}

/**

  KTI wrapper code.

    @param host = pointer to sysHost structure
    KtiStatus - Return status from MRC code execution

    @retval VOID

**/
VOID
OemKtiInit (
    struct sysHost   *host,
    UINT32           KtiStatus
  )
{
#ifndef MINIBIOS_BUILD
  EFI_PEI_SERVICES  **gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  //
  // After KTI init breakpoint callback
  //
  (*gPeiServices)->InstallPpi(gPeiServices, mAfterKtircPpi);
#endif
}

/**

  Oem Init Serial Debug

    @param host = pointer to sysHost structure
    KtiStatus - Return status from MRC code execution

    @retval VOID

**/
VOID
OemInitSerialDebug (
    PSYSHOST host
    )
{
#ifdef SERIAL_DBG_MSG
    //
    // Initialize USB debug port if present
    //
    if (!DiscoverUsb2DebugPort(host))
        InitializeUsb2DebugPort(host);
#endif  //  SERIAL_DBG_MSG
}

/**

  MRC wrapper code.
  Copy host structure to system memory buffer after MRC when memory becomes available

    @param host = pointer to sysHost structure
    MrcStatus - Return status from MRC code execution

@retval VOID

**/
VOID
OemSendDramInitDoneCommand (
    struct sysHost   *host,
    UINT32           MrcStatus
  )

{
#ifndef MINIBIOS_BUILD
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  //
  // Call MeConfigDidReg when ME UMA is configured correctly
  // or MRC error occurred
  //
  if (!host->var.common.resetRequired) {
    ME_UMA_PPI        *MeUma = NULL;
    UINT8             InitStat = 0;
    EFI_STATUS        Status;
    EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

    if (MrcStatus != SUCCESS) {
      InitStat = 0x01;
    } else {
      if ((host->var.mem.previousBootError == 1) ||
          ((host->var.mem.subBootMode != WarmBootFast) && (host->var.common.bootMode == NormalBoot))) {
        DEBUG ((EFI_D_ERROR, "UMA: Memory retrain occurred during warm reset. Force ME FW reload.\n"));
        // Set the flag to tell FW that memory was not maintained InitStat bits 3:0 = (3).
        InitStat = (InitStat & 0xF0) | 0x3;
      }
    }

    //
    // ME UMA Size outside of a 0MB-64MB range is not defined or if BDF 0:22:0 is not present, exit.
    //
    if ((host->var.common.meRequestedSize > ME_UMA_SIZE_UPPER_LIMIT) || (host->var.common.meRequestedSize == 0) ||
        (PchD22PciCfg32 (0x10) == 0xffffffff) ) {
      DEBUG ((EFI_D_INFO, "UMA: Invalid ME UMA Size requested.\n"));
      InitStat = 0x01;
    }

    //
    // Locate MeUma PPI.
    //
    Status = (*PeiServices)->LocatePpi (PeiServices, &gMeUmaPpiGuid, 0, NULL, &MeUma);

    if (IsSimicsPlatform() == FALSE) {
      ASSERT_EFI_ERROR (Status);
    }

    if (!EFI_ERROR(Status)) {
      UINT8 nextStep = CBM_DIR_DO_NOTHING;

#if TESTMENU_FLAG || ME_TESTMENU_FLAG
      Status = MeUma->TestUmaLocation (host->nvram.common.MeNcMemLowBaseAddr.Data,
                                       host->nvram.common.MeNcMemHighBaseAddr.Data,
                                       host->nvram.common.MeNcMemLowLimit.Data,
                                       host->nvram.common.MeNcMemHighLimit.Data
                                      );
      if (EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "Errors in UMA configuration\n"));
        ASSERT_EFI_ERROR (Status);
      }
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
      Status = MeUma->MeConfigDidReg (NULL, InitStat, &nextStep);
      if (!EFI_ERROR(Status)) {
        DEBUG ((EFI_D_INFO, "MeDramInitDone Complete. Checking for reset...\n"));
        if (nextStep == CBM_DIR_NON_PCR) {
          // for this reset use MRC reset mechanism
          host->var.common.resetRequired = POST_RESET_POWERGOOD;
        } else {
          Status = MeUma->HandleMeBiosAction(nextStep);
        }
      } else {
        DEBUG ((EFI_D_ERROR, "MeDramInitDone FAILED (%r). Checking for reset...\n", Status));
      }
    }
  }
#endif // ME_SUPPORT_FLAG

#ifdef IE_SUPPORT
  if (!host->var.common.resetRequired) {
    PEI_IE_HECI_PPI   *IeHeciPpi = NULL;
    EFI_STATUS        Status;
    EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

    //
    // Locate IeHeci PPI.
    //
    Status = (*PeiServices)->LocatePpi (PeiServices, &gPeiIeHeciPpiGuid, 0, NULL, &IeHeciPpi);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_WARN, "[IE] WARNING: HECI PPI not found (%r), will not send MemoryInitDone\n", Status));
    }
    else
    {
      UINT8 Action = 0;
      Status = IeHeciPpi->MemoryInitDoneNotify(IeHeciPpi, MrcStatus, &Action);
      if (!EFI_ERROR(Status)) {
        DEBUG ((EFI_D_INFO, "[IE] IeMemoryInitDone Complete.\n"));
      } else {
        DEBUG ((EFI_D_ERROR, "[IE] ERROR: IeMemoryInitDone FAILED (%r).\n", Status));
      }
    }

  }
#endif //IE_SUPPORT

#endif // MINIBIOS_BUILD
}

/**

  MRC wrapper code.
  Copy host structure to system memory buffer after MRC when memory becomes available

    @param host = pointer to sysHost structure
    MrcStatus - Return status from MRC code execution

@retval VOID

**/
VOID
OemPostMemoryInit (
    struct sysHost   *host,
    UINT32           MrcStatus
  )

{
#ifndef MINIBIOS_BUILD
  EFI_STATUS        Status;
  BOOLEAN FpgaExist = FALSE;
#else // MINIBIOS_BUILD
  struct sysHost *output = (struct sysHost *) 0x100000;                // default pointer to system memory at 1MB
#endif // MINIBIOS_BUILD
  UINT8  socket;
  INT16  data16;
  UINT32 csrReg;
  char  *memNvRamPtr;

#ifndef MINIBIOS_BUILD

  //
  // Post-MRC Housekeeping
  //
  if (MrcStatus == SUCCESS && !host->var.common.resetRequired) {

    if (host->var.common.bootMode == S3Resume && host->var.mem.subBootMode != AdrResume) {
      //
      // Install S3 memory
      //
      Status = InstallS3Memory (host);
      ASSERT_EFI_ERROR (Status);
    } else {
#else // MINIBIOS_BUILD
      if (MrcStatus || host->var.common.bootMode == S3Resume || host->var.common.resetRequired)  {
        // Do not copy host structure to memory if any of these conditions is true.
      } else {
        if (host->setup.common.sysHostBufferPtr) {                                 // OEM override pointer ?
          output = (struct sysHost *) host->setup.common.sysHostBufferPtr;        // yes
        }

        MemCopy((UINT8 *)&output->var, (UINT8 *)&host->var, sizeof (output->var)); // direct copy from host.var to output.var
        MemCopy((UINT8 *)&output->nvram, (UINT8 *)&host->nvram, sizeof (output->nvram)); // direct copy from host.nvram to output.nvram
#endif //MINIBIOS_BUILD
        OemMemRASHook(host, 1);
        //
        // Set BIT1 for SSPAD0 to indicate the next reset is a warm reset.  If this bit
        // goes back to 0, it means the power was fully cycled and the next boot will be
        // a cold reset.
        //
#ifndef MINIBIOS_BUILD
        for (socket = 0; socket < MAX_SOCKET; socket++) {
          if (host->var.kti.OutKtiFpgaEnable[socket]) {
            FpgaExist = TRUE;
            break;
          }
        }
#endif
        for (socket = 0; socket < MAX_SOCKET; socket++) {
          if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
 #ifndef MINIBIOS_BUILD
            if ((FpgaExist && (ReadCpuPciCfgEx (host, host->nvram.common.sbspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG) & BIT5)) ||
                (!FpgaExist)) {
#endif
              csrReg = ReadCpuPciCfgEx (host, socket, 0, MEM_CHIP_POLICY_VALUE(host, BiosStickyScratchPad0));
              //
              // Set BIT1, indicating MRC is done and the next reset is a warm reset.
              //
              WriteCpuPciCfgEx (host, socket, 0, MEM_CHIP_POLICY_VALUE(host, BiosStickyScratchPad0), (csrReg | BIT1));
            }
#ifndef MINIBIOS_BUILD
          }
#endif
          //
          // Save processor PPIN to nvram for cold fast boot if not already saved
          //
          if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)){
            if ((host->nvram.mem.socket[socket].procPpin.hi != host->var.mem.socket[socket].procPpin.hi) ||
               (host->nvram.mem.socket[socket].procPpin.lo != host->var.mem.socket[socket].procPpin.lo)){
              host->nvram.mem.socket[socket].procPpin = host->var.mem.socket[socket].procPpin;
            }
#ifndef MINIBIOS_BUILD
            if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
              DEBUG((EFI_D_INFO,"nvram[%d].ppin.hi: 0x%x, var[%d].ppin.hi: 0x%x \n", socket, host->nvram.mem.socket[socket].procPpin.hi, socket, host->var.mem.socket[socket].procPpin.hi ));
              DEBUG((EFI_D_INFO,"nvram[%d].ppin.lo: 0x%x, var[%d].ppin.lo: 0x%x \n", socket, host->nvram.mem.socket[socket].procPpin.lo, socket, host->var.mem.socket[socket].procPpin.lo ));
            }
#endif
          }
        }

        //
        // Save socket bit map to NVRAM for cold fast boot
        //
        if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)){
          host->nvram.mem.socketBitMap = host->var.common.socketPresentBitMap;
        }

        host->nvram.mem.crc16 = 0;
        memNvRamPtr = (char *)(&(host->nvram.mem));
        data16 = Crc16 (memNvRamPtr, sizeof(host->nvram.mem));
        host->nvram.mem.crc16 = data16;
#ifndef MINIBIOS_BUILD

        //
        // Install EFI memory HOBs
        //
        DEBUG ((EFI_D_ERROR, "Install EFI Memory\n"));
        Status = InstallEfiMemory (host);
        ASSERT_EFI_ERROR (Status);
      }
    }
    DEBUG ((EFI_D_ERROR, "MRC status = %08x\n", MrcStatus));
#else // MINIBIOS_BUILD
    OutPort8(host, 0xed, 0xde);
  }
#endif // MINIBIOS_BUILD
  OemSendDramInitDoneCommand(host, MrcStatus);

#ifndef MINIBIOS_BUILD
    //
    // Disable PBET after MemInit before sending IPI to APs
    //
    // Move StopPbeTimer() to SecPei phase (ProcessorStartupCore.asm)
    // DEBUG ((EFI_D_INFO, "BtG Support: Calling StopPbeTimer()\n"));
    // Call StopPbeTimer() for just Boot Guard Information without calling AsmWriteMsr64 (MSR_BC_PBEC, B_STOP_PBET);
    // Keep the rest of code for debug information
    StopPbeTimer ();
//APTIOV_SERVER_OVERRIDE_RC_START : Boot Guard changes override. Need to removed once proper changes are made.
    //CreateBootguardEventLogEntries();
//APTIOV_SERVER_OVERRIDE_RC_END : Boot Guard changes override. Need to removed once proper changes are made.

#endif  // MINIBIOS_BUILD
}



/**

  Read KTI Nvram from variable.

    @param host = pointer to sysHost structure

    @retval

**/
BOOLEAN
OemReadKtiNvram (
  struct sysHost    *host
  )
{
#ifdef MINIBIOS_BUILD
  return FALSE;
#else

  UINT32            VariableSize;
  UINTN             VarAttrib;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  CHAR16            EfiMemoryConfigVariable[] = L"MemoryConfig0";
  UINT32            RemainingSize;
  VOID              *NvramPtr = NULL;
  struct sysNvram   hostLocal;
  struct sysNvram   *hostLocalptr;
  UINTN             *CompressTable = NULL;
  UINTN             CompressTableSize;
  SCRATCH_DATA      ScratchData;
  UINT32            ScratchDataSize;
  UINT8*            CompressedInBytes;
  EFI_PEI_SERVICES  **PeiServices;
  EFI_STATUS        Status;
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

  hostLocalptr = &hostLocal;

  //
  // Obtain variable services
  //
  Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                &PeiVariable
                );
  ASSERT_EFI_ERROR (Status);

  VarAttrib = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  RemainingSize = sizeof(host->nvram);

  NvramPtr = hostLocalptr;
  while(RemainingSize > 0  && !EFI_ERROR(Status)){
    VariableSize = RemainingSize;

    //
    // Reconstruct the nvram variable
    //
    CompressTableSize = 0;
    Status = PeiVariable->GetVariable (
                PeiVariable,
                EfiMemoryConfigVariable,
                &gEfiMemoryConfigDataGuid,
                (UINT32*)&VarAttrib,
                &CompressTableSize,
                NULL
                );
    //
    //Allocate buffer and get comressed table from NVRAM
    //
    if(Status == EFI_BUFFER_TOO_SMALL)
    {
      CompressTable = AllocatePool(CompressTableSize);

      if(CompressTable == NULL)
      {
          DEBUG((EFI_D_INFO,"Compress Table is NULL\n"));
          ASSERT(FALSE);
          Status = EFI_OUT_OF_RESOURCES;
          continue;
      }
      Status = PeiVariable->GetVariable (
                PeiVariable,
                EfiMemoryConfigVariable,
                &gEfiMemoryConfigDataGuid,
                (UINT32*)&VarAttrib,
                &CompressTableSize,
                CompressTable
                );
    } else {

      return FALSE;
    }

    if (EFI_ERROR(Status)){
      return FALSE;
    }
    if (!EFI_ERROR(Status) && CompressTable != NULL) {
      ScratchDataSize = sizeof(ScratchData);
      CompressedInBytes = (UINT8*) CompressTable;
      VariableSize = CompressedInBytes[4] + (CompressedInBytes[5] << 8) + (CompressedInBytes[6] << 16) + (CompressedInBytes[7] << 24);
      Status = Decompress (CompressTable, *(UINT32 *)(CompressTable) + 8, NvramPtr,
                   VariableSize, &ScratchData, sizeof (SCRATCH_DATA), 1);
    }

    if (CompressTable!= NULL) {
      FreePool(CompressTable);
      CompressTable = NULL;
    }
    //
    // Increment pointers, Variable Name, and remaining size
    //
    EfiMemoryConfigVariable[12]++;  // Increment the number portion of the string
    NvramPtr = (VOID*)((UINT8*) NvramPtr + VariableSize);
    RemainingSize -= VariableSize;  // Decrement the actual size of the variable received
  }

  if(!EFI_ERROR(Status)){
    ScratchDataSize = &host->nvram.kti.SaveSetupData.ColdResetRequestEnd - &host->nvram.kti.SaveSetupData.ColdResetRequestStart;
    MemCopy((UINT8 *)&host->nvram.kti.SaveSetupData.ColdResetRequestStart, (UINT8 *)&hostLocalptr->kti.SaveSetupData.ColdResetRequestStart, ScratchDataSize);
  }

  return TRUE;
#endif
}


/**

  Proc wrapper code.

    @param host = pointer to sysHost structure

    @retval VOID

**/
VOID
OemPreProcInit (
    struct sysHost   *host
  )
{
}

/**

  Proc wrapper code.

    @param host = pointer to sysHost structure
    ProcStatus - Return status from Proc RC code execution

    @retval VOID

**/
UINTN
EFIAPI
SetSocketMmcfgTable (
  IN UINT8                SocketLastBus[],
  IN UINT8                SocketFirstBus[],
  IN UINT8                segmentSocket[],
  IN UINT32               mmCfgBaseH[],
  IN UINT32               mmCfgBaseL[],
  IN UINT8                NumOfSocket
  );

VOID
OemPostProcInit (
    struct sysHost   *host,
    UINT32           ProcStatus
  )
{
#ifndef MINIBIOS_BUILD
#ifdef SEGMENT_ACCESS
  //
  SetSocketMmcfgTable(
    &host->var.common.SocketLastBus[0],
    &host->var.common.SocketFirstBus[0],
    &host->var.common.segmentSocket[0],
    &host->var.common.mmCfgBaseH[0],
    &host->var.common.mmCfgBaseL[0],
    MAX_SOCKET
    );
#endif
#endif

#ifdef PC_HOOK
  OemKtiInit(host,ProcStatus);
#endif
}

/**

  Executed by System BSP only.
  OEM hook before checking and triggering the proper type of reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
OemCheckAndHandleResetRequests (
  struct sysHost             *host
  )
{
  //
  // Perform OEM tasks before a reset might be triggered after control returns to the caller
  //

#ifndef MINIBIOS_BUILD
  if(host->var.common.resetRequired == 0)
  {
    ClearSmiAndWake();
  }
// APTIOV_SERVER_OVERRIDE_RC_START : USE AMI TCG module
#if 0
#ifdef IA32
  if (host->var.common.bootMode == S3Resume) {
    EFI_STATUS                Status;
    EFI_PEI_SERVICES          **gPeiServices;
    VOID                      *TpmInitialize;

    gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

    //
    // Locate TPM Initialized Ppi to determine TPM is present and initialized properly.
    //
    Status = (**gPeiServices).LocatePpi (gPeiServices,
                                   &gPeiTpmInitializedPpiGuid,
                                   0,
                                   NULL,
                                   &TpmInitialize
                                   );
    if (EFI_ERROR (Status))
    {
      DEBUG ((EFI_D_ERROR, "TpmInitialize: TPM is not present!!\n"));
      return;
    }
    if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)){
      DEBUG ((EFI_D_ERROR, "Running TpmSaveStateRun....\n"));
      Status = Tpm12SaveState ();
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Fail to run TpmSaveStateRun....\n"));
      }
    }
    else if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid)){
      DEBUG ((EFI_D_ERROR, "Running Tpm2SaveStateRun....\n"));
      Status = Tpm2Shutdown (TPM_SU_STATE);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Fail to run Tpm2CommSaveState....\n"));
    }
    }
    else {
      DEBUG ((EFI_D_ERROR, "No TPM12 and TPM2 DTPM instance is installed!\n"));
    }
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : USE AMI TCG module
#endif
#endif // MINIBIOS_BUILD
  return;
}

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
/**

  MEBIOS hook before checking and triggering the proper type of reset.

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
MEBIOSCheckAndHandleResetRequests(
  struct sysHost             *host
  )
{
#ifndef MINIBIOS_BUILD
  //
  // Perform MEBIOS tasks before a reset is triggered after control returns to the caller
  //
  EFI_STATUS         Status = EFI_UNSUPPORTED;
  ME_UMA_PPI         *MeUma = NULL;
  EFI_PEI_SERVICES   **PeiServices = NULL;

  //
  // Locate MeUma PPI.
  //
  PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (**PeiServices).LocatePpi (PeiServices, &gMeUmaPpiGuid, 0, NULL, &MeUma);

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot locate MeUma PPI (%r), "
           "HostResetWarning notification FAILED!\n", Status));
  } else {
    //
    // Send HostResetWarning notification to ME
    //
    DEBUG((EFI_D_INFO, "[ME] Send HostResetWarning notification to ME\n"));
    Status = MeUma->HostResetWarning (NULL);
    if (!EFI_ERROR(Status)) {
      DEBUG ((EFI_D_INFO, "[ME] HostResetWarning notification complete\n"));
    } else {
      DEBUG ((EFI_D_ERROR, "[ME] ERROR: HostResetWarning notification FAILED (%r)\n", Status));
    }
  }
#endif
  return;
}
#endif // ME_SUPPORT_FLAG


UINT32
OemMemRASHook (
  struct sysHost             *host,
  UINT8  flag                                 // flag = 0 (Pre MRC RAS hook), flag = 1 (Post MRC RAS hook)
/**

  OEM hook for OEM RAS functionality

  @param host - pointer to sysHost structure on stack
  @param flag - 0 (Pre MRC RAS hook), 1 (Post MRC RAS hook)

  @retval Status (default to SUCCESS)

--*/
  )
{
  UINT32 status = SUCCESS;
  if(flag == 0){ //Initialize VLS info before MRC training only.
#ifndef MINIBIOS_BUILD
    //Not required for a minibios
    status = UpdatePrevBootVLSInfo(host);

#endif //MINIBIOS_BUILD
  }
  return status;
}

/**

  OEM hook to return platform type

  @param host - pointer to sysHost structure on stack

  @retval UINT8 platformtype.

**/
UINT8
OemGetPlatformType (
  struct sysHost             *host
  )

//
{
#ifndef MINIBIOS_BUILD
  EFI_PLATFORM_INFO                     *PlatformInfo;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  EFI_PEI_SERVICES                      **gPeiServices;

  gPeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return TypePlatformUnknown;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);
  return PlatformInfo->BoardId;
#else // MINIBIOS_BUILD
  return TypePlatformDefault;
#endif // MINIBIOS_BUILD
}

/**

  OEM hook to do any platform specifc init

  @param host - pointer to sysHost structure on stack
  @param Node - memory controller number ( 0 based)


  @retval UINT8

**/
VOID
OemPostCpuInit (
  struct sysHost             *host,
  UINT8                      socket
  )

//
{
#ifndef MINIBIOS_BUILD
  //
  // Report Status Code EFI_CU_PC_INIT_BEGIN
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_PC_INIT_BEGIN)
    );
#endif // !MINIBIOS_BUILD
}

/**

  Puts the host structure variables for POST

  @param host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemPublishDataForPost (
  struct sysHost             *host
  )
{
#ifndef MINIBIOS_BUILD
  EFI_STATUS                 Status;

  //
  // Save SYSHOST into persistent memory
  // Register memory discovered PPI call back to create RC HOBs
  //
  Status = SaveHostToMemory (host);
  if (!EFI_ERROR(Status)) {
    Status = PeiServicesNotifyPpi (&mNotifyList);
  }
  ASSERT_EFI_ERROR (Status);
#endif // MINIBIOS_BUILD
}

#ifdef MINIBIOS_BUILD
// Define intrinsic functions to satisfy the .NET 2008 compiler with size optimization /O1
// compiler automatically inserts memcpy/memset fn calls in certain cases
void *memcpy(void *dst, void *src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);
#ifndef __GNUC__
#pragma function(memcpy,memset)
#endif //__GNUC__

void *memcpy(void *dst, void *src, size_t cnt)
{
  MemCopy((UINT8 *)dst, (UINT8 *)src, (UINT32) cnt);
  return dst;
}

void *memset(void *dst, char value, size_t cnt)
{
  MemSetLocal((UINT8 *)dst, (UINT32) value, (UINT32) cnt);
  return dst;
}
#endif

#if SMCPKG_SUPPORT

VOID BeepDelay (UINT32 Usec)
{
    UINT32   Counter = (Usec * 7)/2; // 3.58 count per microsec
    UINT32   i;
    UINT32  Data32;
    UINT32  PrevData;

    PrevData = IoRead32(PM_BASE_ADDRESS + 8);
    for (i=0; i < Counter; ) {
       Data32 = IoRead32(PM_BASE_ADDRESS + 8);
        if (Data32 < PrevData) {        // Reset if there is a overlap
            PrevData=Data32;
            continue;
        }
        i += (Data32 - PrevData);
		PrevData=Data32; // FIX need to find out the real diff betweek different count.
    }
}

VOID
SmcPostErrorNoMemBeep (
  )
{
  UINT16                                  BeepFreq = (1193180 / 1000); // 1000Hz
  UINT8                                   TempIndex;


  IoWrite8 (0x43, 0xB6);                  // Counter 2, R/W, Mode 3
  IoWrite8 (0x42, (UINT8)BeepFreq);
  IoWrite8 (0x42, (UINT8)(BeepFreq >> 8));

  while(1) {
    for (TempIndex = 0; TempIndex < 5; TempIndex++) {
      IoWrite8 (0x61, (IoRead8 (0x61) | 0x03));
      BeepDelay (300 * 1000);  // 300ms
      IoWrite8 (0x61, (IoRead8 (0x61) & 0xFC));
      BeepDelay (100 * 1000);  // 100ms
    }

    IoWrite8 (0x61, (IoRead8(0x61) | 0x03));
    BeepDelay (750 * 1000);    // 750ms
    IoWrite8 (0x61, (IoRead8(0x61) & 0xFC));
    BeepDelay (750 * 1000);    // 750ms
  }

  return;
}
#endif //SMCPKG_SUPPORT

#ifdef PC_HOOK
UINT32
FindErrorCodeIndex(
  UINT8 ErrorCode
)
{
  UINT32 Index, MaxIndex = sizeof (mErrorCodeToken) / sizeof(mErrorCodeToken[0]) - 1;

  for (Index = 0; Index < MaxIndex; Index++) {
    if (mErrorCodeToken[Index].ErrorCode == ErrorCode) {
      break;
    }
  }
  return Index;
}
#endif

/**

  OEM hook to handle a Fatal Error

  @param host        - pointer to sysHost structure on stack
  @param majorCode   - Major error code
  @param minorCode   - Minor error code
  @param haltOnError - Halt on Error input/output value

  @retval N/A

--*/
void
OemPlatformFatalError (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  )
{
#ifndef MINIBIOS_BUILD

#ifdef PC_HOOK
  EFI_STATUS                        VideoPrintStatus;
  EFI_VIDEOPRINT_PPI                *VideoPrint;
  CHAR8                             BcdAscii[VIDEO_PRINT_MAX_STRING];
  CHAR8                             PostCodeString[VIDEO_PRINT_MAX_STRING];

  VideoPrintStatus = PeiServicesLocatePpi (&gEfiVideoPrintPpiGuid, 0, NULL, (VOID **) &VideoPrint);

  if (majorCode == ERR_UNSUPPORTED_TOPOLOGY && minorCode == MINOR_ERR_INVALID_TOPOLOGY) {
    OutPort8 (host, host->setup.common.debugPort, ERR_UNSUPPORTED_TOPOLOGY_CORE);
    //
    // 3 long beep and 1 short beep when CHA cache size mismatch.
    //
    // APTIOV_SERVER_OVERRIDE_RC_START : Using AmiBeepLib we cannot support Beep
 //   LongBeep (3);
 //   Beep (1);
    // APTIOV_SERVER_OVERRIDE_RC_END : Using AmiBeepLib we cannot support Beep
    BcdAscii[0] = ConvertBcdToAscii ((majorCode & 0xF0) >> 4);
    BcdAscii[1] = ConvertBcdToAscii (majorCode & 0x0F);
    BcdAscii[2] = '\0';
    AsciiStrCpy (PostCodeString, BcdAscii);
    FastVideoPrint(POST_CODE_PRINT_SCREEN_ROWS,POST_CODE_PRINT_SCREEN_COLS,PostCodeString);
    if ( !EFI_ERROR (VideoPrintStatus) ) {
      VideoPrint->ConsolePrint("Fatal Error : System Encounter A Stopper Error!\n");
    }

    return;
  }
#endif

  OutPort8 (host,host->setup.common.debugPort,majorCode);
// APTIOV_SERVER_OVERRIDE_RC_START : Using AmiBeepLib we cannot support Beep
//  Beep(3);
// APTIOV_SERVER_OVERRIDE_RC_END : Using AmiBeepLib we cannot support Beep
#ifdef PC_HOOK
  BcdAscii[0] = ConvertBcdToAscii ((majorCode & 0xF0) >> 4);
  BcdAscii[1] = ConvertBcdToAscii (majorCode & 0x0F);
  BcdAscii[2] = '\0';
  AsciiStrCpy (PostCodeString, BcdAscii);
  FastVideoPrint(POST_CODE_PRINT_SCREEN_ROWS,POST_CODE_PRINT_SCREEN_COLS,PostCodeString);
  if ( !EFI_ERROR (VideoPrintStatus) ) {
    VideoPrint->ConsolePrint(mErrorCodeToken[FindErrorCodeIndex(majorCode)].ErrorString);
  }
#endif
#endif

#if SMCPKG_SUPPORT
  if ( majorCode == ERR_NO_MEMORY )
    SmcPostErrorNoMemBeep();
#endif

  return;
}


/**

  Determine if VRs should be turned off on HEDT

  @param host - pointer to sysHost structure on stack

@retval VOID

**/
VOID
OemTurnOffVrsForHedt (
  struct sysHost             *host
  )
{
#ifndef MINIBIOS_BUILD
  //
  // If HEDT
  //
  if (host->nvram.common.platformType == TypeHedtCRB) {
    if (host->setup.cpu.vrsOff ) {
      //
      // Determine which GPIO pins to assert based on memory configuration. These pins turn off power rails to memory VRs
      //
      /* SKX_TODO - Update GPIO
      if ( !(host->nvram.mem.socket[0].channelList[0].enabled) && !(host->nvram.mem.socket[0].channelList[1].enabled) ) {
        //
        // Channmel 0/1 is empty, so assert V_SM_C01_LOT26 (GPIO15)
        //
        IoWrite32((host->var.common.gpioBase + 0x0C),(IoRead32((host->var.common.gpioBase + 0x0C)) | (1 << 15)));
      }

      if ( !(host->nvram.mem.socket[0].channelList[2].enabled) && !(host->nvram.mem.socket[0].channelList[3].enabled) ) {
        //
        // Channmel 2/3 is empty, so assert V_SM_C23_LOT26 (GPIO28)
        //
        IoWrite32((host->var.common.gpioBase + 0x0C),(IoRead32((host->var.common.gpioBase + 0x0C)) | (1 << 28)));
      }*/
    }  // If setup = enable
  }    // If HEDT
#endif // MINIBIOS_BUILD
  return;
}

/**

  OEM hook to handle EParam

  @param host                 - pointer to sysHost
  @param SizeOfTable          - table size
  @param per_lane_sizeOfTable - table size per lane
  @param ptr                  - pointer pointer to all lanes table
  @param per_lane_ptr         - pointer pointer to per lane table

  @retval FALSE

**/
UINT32
OemPlatformSpecificEParam (
  struct sysHost             *host,
  UINT32                     *SizeOfTable,
  UINT32                     *per_lane_SizeOfTable,
  ALL_LANES_EPARAM_LINK_INFO **ptr,
  PER_LANE_EPARAM_LINK_INFO  **per_lane_ptr
  )
{
  return FALSE;
}

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param host              - pointer to sysHost
  @param socket            - socket number
  @param smbDevice dev     - SMB Device
  @param byteOffset        - byte Offset
  @param data              - data

**/
void
OemPlatformHookMst (
  PSYSHOST host,
  UINT8    socket,
  struct   smbDevice dev,
  UINT8    byteOffset,
  UINT8    *data
  )
{

#ifndef MINIBIOS_BUILD
  UINT16 mstData;
  UINT32 returnValue;


    //
    // Initialize the config register
    //
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"[MEM EVENT] platform configure MEM event pin start!\n"));

    //Below workaround is for ATMEL TS.

    //Note: Wait for critical trip flag being cleared before enbale memory trip.
    returnValue = ReadSmb (host, socket, dev, MTS_TEMP, (UINT8 *)&mstData);

    if(returnValue == 1){
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"[MEM EVENT] read MTS_TEMP fail!\n"));
    }

    while (mstData) {
      if (mstData & 0x8000) {
        ReadSmb (host, socket, dev, MTS_TEMP, (UINT8 *)&mstData);
      } else {
      mstData = 0;
      };
    };

    mstData = MTS_CFG_TCRIT_ONLY;
    returnValue = WriteSmb (host, socket, dev, MTS_CONFIG, (UINT8 *)&mstData);

    if(returnValue == 1){
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"[MEM EVENT] write MTS_CONFIG first fail!\n"));
    }

    FixedDelayMem(host, 100); // one type ATMEL TS need this this workaround.
    mstData = (MTS_CFG_TCRIT_ONLY +MTS_CFG_EVENT_CTRL);
    returnValue = WriteSmb (host, socket, dev, MTS_CONFIG, (UINT8 *)&mstData);

    if(returnValue == 1){
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"[MEM EVENT] write MTS_CONFIG first fail!\n"));
    }
#endif
  return;
}

/**

  OEM hook for reacting to Board VS CPU conflict.
  Refcode will halt execution. OEMs might prefer to allow system to boot to
  video and display error code on screen.

  @param host - pointer to sysHost

**/
VOID
OemCheckForBoardVsCpuConflicts (
  PSYSHOST host
  )
{
  if (host->var.kti.OutBoardVsCpuConflict != 0) {
    rcPrintf ((host, "\nBoard VS CPU conflict detected: 0x%x\n", host->var.kti.OutBoardVsCpuConflict));
    RcDeadLoop (1);
  }
}

/*++

Routine Description:

  OEM hook meant to be used by customers where they can use it to write their own jumper detection code
  and have it return FALSE when no jumper is present, thereby, blocking calling of the BSSA loader; else have it
  return TRUE if the concerned jumper is physically present. This check ensures that someone will have to be
  physically present by the platform to enable the BSSA Loader.

Arguments:
  host              - pointer to sysHost

Returns:
  This function returns TRUE by default as OEMs are meant to implement their own jumper detection code(DetectPhysicalPresence).

--*/
BOOLEAN
OemDetectPhysicalPresenceSSA (
  PSYSHOST         host
  )
{
#ifdef SSA_FLAG
  return TRUE;
#else
  return FALSE;
#endif  //SSA_FLAG
}

/**

  Dummy wrapper code.

    @param host = pointer to sysHost structure

    @retval VOID

**/
VOID
DummyOemHooks (
    struct sysHost   *host
  )
{
}
