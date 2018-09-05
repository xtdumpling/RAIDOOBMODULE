//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix : Fixed Cburn ON/OFF shows "MEMORY SIZE MISMATCH Configuration
//              mismatch, stopping, will not continue" on Purley DP platform.
//              (TWLAB criteria is MDELT=16 and both CPU and memory are fully populated.)
//    Reason  : Bug Fixed. (Refer to GrantleyHEDT Trunk revision #139.)
//    Auditor : Joe Jhang
//    Date    : Feb/14/2017
//
//****************************************************************************
//****************************************************************************
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  Code for processor configuration.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ProcessorConfig.c

**/

#include "MpService.h"
#include "Cpu.h"
#include "Microcode.h"
#include "X2Apic.h"
#include "MchkInit.h"
#include "ThermalManagement.h"
#include "CState.h"
#include "Vt.h"
#include "Misc.h"

#include "PCU_FUN3.h" // PURLEY_OVERRIDE
#include <Token.h>  //SMCPKG_SUPPORT

MP_SYSTEM_DATA                      mMPSystemData;
CPU_CONFIG_CONTEXT_BUFFER           mCpuConfigContextBuffer;
EFI_PHYSICAL_ADDRESS                mStartupVector;
UINT8                               mPlatformType;
ACPI_CPU_DATA                       *mAcpiCpuData;
EFI_HANDLE                          mHandle = NULL;
MTRR_SETTINGS                       *mMtrrSettings;
EFI_EVENT                           mSmmConfigurationNotificationEvent;
EFI_HANDLE                          mImageHandle;
//
// PURLEY_OVERRIDE_BEGIN
//
EFI_CPU_CSR_ACCESS_PROTOCOL         *mCpuCsrAccess;
EFI_IIO_UDS_PROTOCOL                *mIioUds;
//
// PURLEY_OVERRIDE_END
//
EFI_TIMER_ARCH_PROTOCOL             *mTimer;
UINTN                               mLocalApicTimerDivisor;
UINT32                              mLocalApicTimerInitialCount;
//ESS Override Start
// 4168379: Odin: PL1 cannot be changed greater than 200W and lock bit set.
UINT32                              mTurboOverride;

//Grantley 4986242: resolve S3 ucode load time too long
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//VOID                                *mMicrocodeBaseAddress;
//VOID                                *mMicrocodePatchAddress;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
//ESS Override End

//
// PURLEY_OVERRIDE_BEGIN
//
//Global variables from PCD values
UINT64                              mCpuIioLlcWaysBitMask;
UINT64                              mCpuExpandedIioLlcWaysBitMask;
UINT64                              mCpuRemoteWaysBitMask;
UINT64                              mCpuQlruCfgBitMask;
BOOLEAN                             mCpuHaswellFamilyFlag;
BOOLEAN                             mCpuSkylakeFamilyFlag;
UINT64                              mCpuRemoteRequestBitMask;
BOOLEAN                             mPcdMachineCheckEnable;
UINT8                               mPcdCpuCoreCStateValue;
UINT32                              mCpuCapid5PcuFun3[MAX_SOCKET] = {0};
//
// PURLEY_OVERRIDE_END
//
/**
  Prepares memory region for processor configuration.

  This function prepares memory region for processor configuration.

**/
VOID
PrepareMemoryForConfiguration (
  VOID
  )
{
  UINTN                NumberOfProcessors;
  UINTN                Index;
  MONITOR_MWAIT_DATA   *MonitorData;
//
// PURLEY_OVERRIDE_BEGIN
//
  EFI_PHYSICAL_ADDRESS  StackStartAddress = 0xffffffff;
  EFI_STATUS            Status;
//    UINTN                MaxNumberOfCpus;
//
// PURLEY_OVERRIDE_END
//
  NumberOfProcessors = mCpuConfigContextBuffer.NumberOfProcessors;

  //
  // Allocate buffer for arrays in MP_SYSTEM_DATA
  //
  mMPSystemData.CpuList = (BOOLEAN *)AllocateZeroPool (
                                       sizeof (BOOLEAN) * NumberOfProcessors
                                       );
  ASSERT (mMPSystemData.CpuList != NULL);

  mMPSystemData.CpuData = (CPU_DATA_BLOCK *)AllocateZeroPool (
                                              sizeof (CPU_DATA_BLOCK) * NumberOfProcessors
                                              );
  ASSERT (mMPSystemData.CpuData != NULL);

  mMPSystemData.DisableCause = (EFI_CPU_STATE_CHANGE_CAUSE *)AllocateZeroPool (
                                                               sizeof (EFI_CPU_STATE_CHANGE_CAUSE) * NumberOfProcessors
                                                               );
  ASSERT (mMPSystemData.DisableCause != NULL);

  mMPSystemData.CpuHealthy = (BOOLEAN *)AllocateZeroPool (
                                          sizeof (BOOLEAN) * NumberOfProcessors
                                          );
  ASSERT (mMPSystemData.CpuHealthy != NULL);

  //
  // Initialize Spin Locks for system
  //
  InitializeSpinLock (&mMPSystemData.APSerializeLock);
  InitializeSpinLock (&mMPSystemData.MsrLock);
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    InitializeSpinLock (&mMPSystemData.CpuData[Index].CpuDataLock);
  }

//
// PURLEY_OVERRIDE_BEGIN
//
  //
  // Claim memory for AP stack.
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (PcdGet32(PcdCpuMaxLogicalProcessorNumber) * PcdGet32 (PcdCpuApStackSize)),
                  &StackStartAddress
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ( (EFI_D_ERROR, "::::::: Failed to allocate pages for ProcessorConfig\n") );
    return;
  }  

  mExchangeInfo->StackStart = (VOID *)(UINTN)StackStartAddress; //AllocateAcpiNvsMemoryBelow4G (PcdGet32(PcdCpuMaxLogicalProcessorNumber) * PcdGet32 (PcdCpuApStackSize));
//
// PURLEY_OVERRIDE_END
//
  mExchangeInfo->StackSize  = PcdGet32 (PcdCpuApStackSize);

  //
  // Initialize the Monitor Data structure in APs' stack
  //
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    MonitorData = GetMonitorDataAddress (Index);
    MonitorData->ApLoopMode = ApInHltLoop;
  }

  //
  // Initialize data for CPU configuration context buffer
  //
  mCpuConfigContextBuffer.CollectedDataBuffer  = AllocateZeroPool (sizeof (CPU_COLLECTED_DATA) * NumberOfProcessors);
  mCpuConfigContextBuffer.FeatureLinkListEntry = AllocateZeroPool (sizeof (LIST_ENTRY) * NumberOfProcessors);

  //
  // Initialize Processor Feature List for all logical processors.
  //
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    InitializeListHead (&mCpuConfigContextBuffer.FeatureLinkListEntry[Index]);
  }

  mCpuConfigContextBuffer.RegisterTable   = AllocateAcpiNvsMemoryBelow4G (
                                            (sizeof (CPU_REGISTER_TABLE) + sizeof (UINTN)) * NumberOfProcessors
                                            );
  mCpuConfigContextBuffer.PreSmmInitRegisterTable = AllocateAcpiNvsMemoryBelow4G (
                                                     (sizeof (CPU_REGISTER_TABLE) + sizeof (UINTN)) * NumberOfProcessors
                                                     );

  mCpuConfigContextBuffer.SettingSequence = (UINTN *) (((UINTN) mCpuConfigContextBuffer.RegisterTable) + (sizeof (CPU_REGISTER_TABLE) * NumberOfProcessors));
  for (Index = 0; Index < NumberOfProcessors; Index++) {
#if (defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1))
    mCpuConfigContextBuffer.PreSmmInitRegisterTable[Index].RegisterTableEntry = AllocateAcpiNvsMemoryBelow4G (EFI_PAGE_SIZE );
    mCpuConfigContextBuffer.PreSmmInitRegisterTable[Index].AllocatedSize = EFI_PAGE_SIZE;
#endif  //#if (defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1))
    mCpuConfigContextBuffer.SettingSequence[Index] = Index;
  }

  //
  // Set the value for PcdCpuConfigContextBuffer.
  //
  mCpuConfigLibConfigContextBuffer = &mCpuConfigContextBuffer;
  Status = PcdSet64S (PcdCpuConfigContextBuffer, (UINT64) (UINTN) mCpuConfigLibConfigContextBuffer);
  ASSERT_EFI_ERROR (Status);

  //
  // Read the platform type from PCD
  //
  mPlatformType = PcdGet8 (PcdPlatformType);

  //
  // Allocate buffers used for feature configuration.
  //

  if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
    MachineCheckAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuThermalManagementFlag)) {
    ThermalManagementAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuCStateFlag)) {
    CStateAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    MicrocodeAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuVtLtFlag)) {
    VtLtAllocateBuffer ();
  }

}

/**
  The post phase MP initialization after SMM initialization.

**/
VOID
PostProcessorConfiguration (
  VOID
  )
{
  EFI_STATUS    Status;
  VOID          *Registration;
  //
  // Wakeup APs. Collect data of all processors. BSP polls to
  // wait for APs' completion.
  //
  DataCollectionPhase ();
  //
  // With collected data, BSP analyzes processors'configuration
  // according to user's policy.
  //
  AnalysisPhase ();

  //
  // Wakeup APs. Program registers of all processors, according to
  // the result of Analysis phase. BSP polls to wait for AP's completion.
  //
  SettingPhase ();

  //
  // Select least-feature procesosr as BSP
  //
  if (FeaturePcdGet (PcdCpuSelectLfpAsBspFlag)) {
    SelectLfpAsBsp ();
  }

  //
  // Add SMBIOS Processor Type and Cache Type tables for the CPU.
  //
  AddCpuSmbiosTables ();

  //
  // Save CPU S3 data
  //
  SaveCpuS3Data (mImageHandle);

  Status = gBS->SetTimer (
                  mMPSystemData.CheckAPsEvent,
                  TimerPeriodic,
                  10000 * MICROSECOND
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Setup notification on Legacy BIOS Protocol to reallocate AP wakeup
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiLegacyBiosProtocolGuid,
    TPL_CALLBACK,
    ReAllocateMemoryForAP,
    NULL,
    &Registration
    );
}

/**
  Event notification that is fired every time a gEfiSmmConfigurationProtocol installs.

  This function configures all logical processors with three-phase architecture.

  @param  Event                 The Event that is being processed, not used.
  @param  Context               Event Context, not used.

**/
VOID
EFIAPI
SmmConfigurationEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS    Status;
  EFI_SMM_CONFIGURATION_PROTOCOL  *SmmConfiguration;

  //
  // Make sure this notification is for this handler
  //
  Status = gBS->LocateProtocol (&gEfiSmmConfigurationProtocolGuid, NULL, (VOID **)&SmmConfiguration);
  if (EFI_ERROR (Status)) {
    return;
  }

  PostProcessorConfiguration ();
}

/**
  Event notification for gEfiDxeSmmReadyToLockProtocolGuid event.

  This function performs security locks before any 3rd party
  code can execute.

  @param  Event                 The Event that is being processed, not used.
  @param  Context               Event Context, not used.

**/
VOID
EFIAPI
SmmReadyToLockCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                Status;
  VOID                      *Interface;

  Status = gBS->LocateProtocol (&gEfiDxeSmmReadyToLockProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Call SetIaUntrusted on the BSP
  //
  SetIaUntrusted(&mMpService);

  if (mCpuConfigContextBuffer.NumberOfProcessors > 1) {
    //
    // Call SetIaUntrusted function for each AP
    //
    Status = mMpService.StartupAllAPs (
                          &mMpService,
                          (EFI_AP_PROCEDURE)SetIaUntrusted,
                          FALSE,
                          NULL,
                          0,
                          &mMpService,
                          NULL
                          );
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  Early MP Initialization.

  This function does early MP initialization, including MTRR sync and first time microcode load.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
EarlyMpInit (
  IN UINTN  ProcessorNumber
  )
{
  //
  // first time microcode load.
  // Microcode patch must be applied before collecting capability
  // of processor features.
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    MicrocodeDetect (ProcessorNumber, TRUE);
  }

  MtrrSetAllMtrrs (mMtrrSettings);

  CollectBasicProcessorData (ProcessorNumber);

  //
  // If matching microcode patch has been found for this processor, then it
  // has already been applied at earlier phase. So here simply check if
  // microcode has been applied and collect the version.
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    CollectMicrocodeRevision (ProcessorNumber);
  }
}

//
// PURLEY_OVERRIDE_BEGIN
//
/**
  Early MP package-scope Initialization.
  
  This function does early MP package-scope initialization, typically programming of packege-scope MSRs
  It is called after AssignPackageBsp() has been executed.
    
**/
VOID
EarlyMpInitPkgScope ()
{
  UINT8   SocketId;

  if (IsSimicsPlatform()) {
    return;
  }

  for (SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
    if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketId].Valid) {
      mCpuCapid5PcuFun3[SocketId] = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, CAPID5_PCU_FUN3_REG);
    }
  }

  DispatchAPAndWait (
    TRUE,
    0,
    EarlyMpInitPbsp
  );

  //
  //  Now SBSP
  //
  EarlyMpInitPbsp(mCpuConfigContextBuffer.BspNumber);
}
//
// PURLEY_OVERRIDE_END
//

/**
  Configures all logical processors with three-phase architecture.
  
  This function configures all logical processors with three-phase architecture.

**/
EFI_STATUS
ProcessorConfiguration (
  VOID
  )
{
//
// PURLEY_OVERRIDE_BEGIN
//
  EFI_STATUS Status = EFI_SUCCESS;
//
// PURLEY_OVERRIDE_END
//
  //
  // Wakeup APs for the first time, BSP stalls for arbitrary
  // time for APs' completion. BSP then collects the number
  // and BIST information of APs.
  //
  WakeupAPAndCollectBist ();
  //
  // Sort APIC ID of all processors in asending order. Processor number
  // is assigned in this order to ease MP debug. SMBIOS logic also depends on it.
  //
  SortApicId ();

  //
  // Prepare data in memory for processor configuration
  //
  PrepareMemoryForConfiguration ();

  //
  // Early MP initialization
  //
  mMtrrSettings = (MTRR_SETTINGS *)(UINTN)PcdGet64 (PcdCpuMtrrTableAddress);
  if (mMtrrSettings == NULL) {
    //
    // MtrrSettings is not set. Set it here.
    // Allocate memory for fixed MTRRs, variable MTRRs and MTRR_DEF_TYPE
    //
    mMtrrSettings = AllocateAcpiNvsMemoryBelow4G (sizeof (MTRR_SETTINGS));
    Status = PcdSet64S (PcdCpuMtrrTableAddress, (UINT64) (UINTN) mMtrrSettings);
    ASSERT_EFI_ERROR (Status);
  }
  MtrrGetAllMtrrs (mMtrrSettings);

//
// PURLEY_OVERRIDE_BEGIN
// 
  mTurboOverride = PcdGet32(PcdCpuTurboOverride);
  //DEBUG ((EFI_D_ERROR, ":: mTurboOverride = %X\n", mTurboOverride));

//BDE Override for s5330552 start
  GetTraceHubAcpiBaseAddress();
  DispatchAPAndWait (
    TRUE,
    0,
    SetTraceHubAcpiBaseAddress
    );
//BDE Override for s5330552 end
//
// PURLEY_OVERRIDE_END
//

  DispatchAPAndWait (
    TRUE,
    0,
    EarlyMpInit
    );

  EarlyMpInit (mCpuConfigContextBuffer.BspNumber);

  DEBUG_CODE (
    //
    // Verify that all processors have same APIC ID topology. New APIC IDs
    // were constructed based on this assumption.
    //
    UINTN Index;
    UINT8 PackageIdBitOffset;

    PackageIdBitOffset = mCpuConfigContextBuffer.CollectedDataBuffer[0].PackageIdBitOffset;
    for (Index = 1; Index < mCpuConfigContextBuffer.NumberOfProcessors; Index++) {
      if (PackageIdBitOffset != mCpuConfigContextBuffer.CollectedDataBuffer[Index].PackageIdBitOffset) {
        ASSERT (FALSE);
      }
    }
  );

  //
  // Check if there is legacy APIC ID conflict among all processors.
  //
  Status = CheckApicId ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Assign Package BSP for package scope programming later.
  //
  AssignPackageBsp ();

  //
  // Perform processor package-scope early configuration
  //
  EarlyMpInitPkgScope();

  //
  // Produce pre-SMM-init register table.
  //
  ProducePreSmmInitRegisterTable ();

  //
  // Early MP initialization step 2
  //
  DispatchAPAndWait (
    TRUE,
    0,
    SetPreSmmInitProcessorRegister
    );

  SetPreSmmInitProcessorRegister (mCpuConfigContextBuffer.BspNumber);

  //
  // Re-program Local APIC for virtual wire mode if socket ID for the BSP has been changed.
  //
  ReprogramVirtualWireMode ();

  //
  // If socket IDs were reassigned, update APIC ID related info collected before.
  //
  UpdateApicId ();

  // APTIOV_SERVER_OVERRIDE_RC_START : Use AMI TCG module for TCG/TPM support
  //MeasureMicrocode ();
  // APTIOV_SERVER_OVERRIDE_RC_END : Use AMI TCG module for TCG/TPM support

  //
  // Locate Timer Arch Protocol
  //
  Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &mTimer);
  ASSERT_EFI_ERROR (Status);

//
// PURLEY_OVERRIDE_BEGIN
//
 //ESS Override start

  WA_Lock ();
//
// PURLEY_OVERRIDE_END
//
  //
  // Install MP Services Protocol
  //
  InstallMpServicesProtocol ();

  return EFI_SUCCESS;
}

/**
  Installs MP Services Protocol and register related timer event.

  This function installs MP Services Protocol and register related timer event.

**/
VOID
InstallMpServicesProtocol (
  VOID
  )
{
  EFI_STATUS    Status;

  //
  // Create timer event to check AP state for non-blocking execution.
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  CheckAPsStatus,
                  NULL,
                  &mMPSystemData.CheckAPsEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Now install the MP services protocol.
  //
  Status = gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiMpServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mMpService
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Callback function for idle events.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
IdleLoopEventCallback (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
//
// PURLEY_OVERRIDE_BEGIN
//
  return;
//
// PURLEY_OVERRIDE_END
//
}

/**
  Entrypoint of CPU MP DXE module.

  This function is the entrypoint of CPU MP DXE module.
  It initializes Multi-processor configuration and installs MP Services Protocol.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The entrypoint always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
MultiProcessorInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;
  EFI_EVENT   IdleLoopEvent;
  EFI_EVENT   ExitBootServiceEvent;
  EFI_EVENT   LegacyToBootEvent;
  UINT32      RegEax;
  UINT32      FamilyId;
  UINT32      ModelId;

  mImageHandle = ImageHandle;
  FamilyId     = 0;
  ModelId      = 0;
	
//BDE Ovverride begin - copy the ucode to memory to speed up the ucode load time for S3
//details in Grantley 4986242
	//
	// Copy Microcode to reserved memory
	//
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//  mMicrocodeBaseAddress = AllocateReservedPool(PcdGet32 (PcdFlashNvStorageMicrocodeSize));
//  CopyMem(mMicrocodeBaseAddress, (VOID*)(UINTN)PcdGet32 (PcdFlashNvStorageMicrocodeBase), (UINTN)PcdGet32 (PcdFlashNvStorageMicrocodeSize));
//	mMicrocodePatchAddress = (VOID *)((UINTN)mMicrocodeBaseAddress + (UINTN)(PcdGet64 (PcdCpuMicrocodePatchAddress) - PcdGet32 (PcdFlashNvStorageMicrocodeBase)));
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
//BDE Override end

//
// PURLEY_OVERRIDE_BEGIN
//
  //
  // Store the PCD values needed in AP functions to global variables
  //
  mCpuIioLlcWaysBitMask         = PcdGet64(PcdCpuIioLlcWaysBitMask);
  mCpuExpandedIioLlcWaysBitMask = PcdGet64(PcdCpuExpandedIioLlcWaysBitMask);
  mCpuRemoteWaysBitMask         = PcdGet64(PcdCpuRemoteWaysBitMask);
  mCpuQlruCfgBitMask            = PcdGet64(PcdCpuQlruCfgBitMask);
  mCpuHaswellFamilyFlag         = FeaturePcdGet (PcdCpuHaswellFamilyFlag);
  //BUGBUGmCpuSkylakeFamilyFlag  = FeaturePcdGet (PcdCpuSkylakeFamilyFlag);
  mCpuRemoteRequestBitMask         = PcdGet64(PcdCpuRRQCountThreshold);
  mPcdMachineCheckEnable        = (PcdGet32(PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MACHINE_CHECK_BIT)? 1: 0; //OVERIDE_HSD_5330628
  mPcdCpuCoreCStateValue        = PcdGet8(PcdCpuCoreCStateValue);

  BdeHookInitialize();
//
// PURLEY_OVERRIDE_END
//
  //
  // Configure processors with three-phase architecture
  //
  Status = ProcessorConfiguration ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag) || FeaturePcdGet (PcdCpuSilvermontFamilyFlag) ||
      FeaturePcdGet (PcdCpuGoldmontFamilyFlag)) {
    //
    // Get the current logic processor information
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
    FamilyId = BitFieldRead32 (RegEax, 8, 11);
    ModelId  = BitFieldRead32 (RegEax, 4, 7);
    if (FamilyId == 0x06 || FamilyId == 0x0f) {
      ModelId += ((BitFieldRead32 (RegEax, 16, 19)) << 4);
    }
    if (FamilyId == 0x0f) {
      FamilyId += BitFieldRead32 (RegEax, 20, 27);
    }
  }

  if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag) && IS_TUNNELCREEK_PROC (FamilyId, ModelId)) {
    //
    // If this processor is TunnelCreek, do the post phase MP initialization directly and needn't wait for
    // SMM initialization.
    //
    PostProcessorConfiguration ();
  } else {
    //
    // Install notification callback on SMM Configuration Protocol
    //
    mSmmConfigurationNotificationEvent = EfiCreateProtocolNotifyEvent (
                                           &gEfiSmmConfigurationProtocolGuid,
                                           TPL_CALLBACK,
                                           SmmConfigurationEventNotify,
                                           NULL,
                                           &Registration
                                           );
  }

  if ((FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_AVOTON_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuGoldmontFamilyFlag) && IS_DENVERTON_PROC (FamilyId, ModelId))) {
    if (FeaturePcdGet (PcdCpuEnableIaUntrustedModeFlag)) {
      //
      // Install notification callback on SMM ready to lock event
      //
      EfiCreateProtocolNotifyEvent  (
        &gEfiDxeSmmReadyToLockProtocolGuid,
        TPL_CALLBACK,
        SmmReadyToLockCallback,
        NULL,
        &Registration
        );
    }
  }

  //
  // Check if there is AP existed
  //
  if (mCpuConfigContextBuffer.NumberOfProcessors > 1) {
    //
    // Create EXIT_BOOT_SERIVES Event to set AP to suitable status
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ChangeApLoopModeCallBack,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &ExitBootServiceEvent
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Create an event to be signalled when Legacy Boot occurs to set AP to suitable status
    //
    Status = EfiCreateEventLegacyBootEx(
               TPL_NOTIFY,
               ChangeApLoopModeCallBack,
               NULL,
               &LegacyToBootEvent
               );
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Setup a callback for idle events
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  IdleLoopEventCallback,
                  NULL,
                  &gIdleLoopEventGuid,
                  &IdleLoopEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Wakes up APs for the first time to count their number and collect BIST data.

  This function wakes up APs for the first time to count their number and collect BIST data.

**/
VOID
WakeupAPAndCollectBist (
  VOID
  )
{
  //
  // Save BSP's Local APIC Timer setting
  //
  GetApicTimerState (&mLocalApicTimerDivisor, NULL, NULL);
  mLocalApicTimerInitialCount = GetApicTimerInitCount ();

  //
  // Prepare code and data for APs' startup vector
  //
  PrepareAPStartupVector ();

  mCpuConfigContextBuffer.NumberOfProcessors = 1;
  mCpuConfigContextBuffer.BspNumber = 0;
  //
  // Item 0 of BistBuffer is for BSP.
  //
  mExchangeInfo->BistBuffer[0].ApicId = GetInitialApicId ();

  SendInitSipiSipiIpis (
    TRUE,
    0,
    NULL
    );

  //
  // Wait for task to complete and then exit.
  //
  MicroSecondDelay (PcdGet32 (PcdCpuApInitTimeOutInMicroSeconds));
  mExchangeInfo->InitFlag = 0;
}


/**
  Prepare ACPI NVS memory below 4G memory for use of S3 resume.

  This function allocates ACPI NVS memory below 4G memory for use of S3 resume,
  and saves data into the memory region.

  @param  Context   The Context save the info.

**/
VOID
SaveCpuS3Data (
  VOID    *Context
  )
{
  MP_CPU_SAVED_DATA       *MpCpuSavedData;
  EFI_STATUS              Status;

  //
  // Allocate ACPI NVS memory below 4G memory for use of S3 resume.
  //
  MpCpuSavedData = AllocateAcpiNvsMemoryBelow4G (sizeof (MP_CPU_SAVED_DATA));

  //
  // Set the value for CPU data
  //
  mAcpiCpuData                 = &(MpCpuSavedData->AcpiCpuData);
  mAcpiCpuData->GdtrProfile    = (EFI_PHYSICAL_ADDRESS) (UINTN) &(MpCpuSavedData->GdtrProfile);
  mAcpiCpuData->IdtrProfile    = (EFI_PHYSICAL_ADDRESS) (UINTN) &(MpCpuSavedData->IdtrProfile);
  mAcpiCpuData->StackAddress   = (EFI_PHYSICAL_ADDRESS) (UINTN) mExchangeInfo->StackStart;
  mAcpiCpuData->StackSize      = PcdGet32 (PcdCpuApStackSize);
  mAcpiCpuData->MtrrTable      = (EFI_PHYSICAL_ADDRESS) (UINTN) PcdGet64 (PcdCpuMtrrTableAddress);
  mAcpiCpuData->RegisterTable  = (EFI_PHYSICAL_ADDRESS) (UINTN) mCpuConfigContextBuffer.RegisterTable;

  mAcpiCpuData->PreSmmInitRegisterTable   = (EFI_PHYSICAL_ADDRESS) (UINTN) mCpuConfigContextBuffer.PreSmmInitRegisterTable;
  mAcpiCpuData->ApMachineCheckHandlerBase = mApMachineCheckHandlerBase;
  mAcpiCpuData->ApMachineCheckHandlerSize = mApMachineCheckHandlerSize;

  //
  // Copy GDTR and IDTR profiles
  //
  CopyMem ((VOID *) (UINTN) mAcpiCpuData->GdtrProfile, (VOID *) (UINTN) &mExchangeInfo->GdtrProfile, sizeof (IA32_DESCRIPTOR));
  CopyMem ((VOID *) (UINTN) mAcpiCpuData->IdtrProfile, (VOID *) (UINTN) &mExchangeInfo->IdtrProfile, sizeof (IA32_DESCRIPTOR));

  mAcpiCpuData->NumberOfCpus  = (UINT32) mCpuConfigContextBuffer.NumberOfProcessors;

  //
  // Set the base address of CPU S3 data to PcdCpuS3DataAddress
  //
  Status = PcdSet64S (PcdCpuS3DataAddress, (UINT64)(UINTN)mAcpiCpuData);
  ASSERT_EFI_ERROR (Status);
}
