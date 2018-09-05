/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

  Copyright (c) 2007 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


  @file TxtPei.c

  This is the main PEIM file for LT.  It represents an abstract outline of the
  steps required during PEI for enabling LT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize
  the need to modify this file when porting LT to future platforms.

**/
#include "TxtPeiLib.h"
#include <Library/LtPeiLib.h>
#include <Library/PchPmcLib.h>

EFI_STATUS
EFIAPI
ScleanRun (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN PEI_TXT_SCLEAN_PPI             *This
  );

PEI_TXT_SCLEAN_PPI  mScleanPpi = {ScleanRun};

STATIC
EFI_PEI_PPI_DESCRIPTOR mScleanPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTxtScleanPpiGuid,
  &mScleanPpi
};

STATIC
EFI_PEI_PPI_DESCRIPTOR mTxtMemoryUnlockPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiTxtMemoryUnlockedPpiGuid,
  NULL
};

/**

  Ensure memory secrets are cleared before lock

  @param PeiServices   - Pointer to PEI Services Table.
  @param This          - PPI pointer

  @retval EFI_SUCCESS - Always

**/
EFI_STATUS
EFIAPI
ScleanRun (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN PEI_TXT_SCLEAN_PPI             *This
  )
{
  EFI_STATUS                        Status;
  LT_PEI_LIB_CONTEXT                LtPeiCtx;

  Status = InitializeLtPeiLib (PeiServices, &LtPeiCtx);
  ASSERT_EFI_ERROR (Status) ;

// If bios needs to clear memory secrets then do so even if the CPU has specail fuse setting
//  if (!IsLtEnabled (&LtPeiCtx)) {
//    return EFI_SUCCESS;
//  }

  DEBUG ((EFI_D_ERROR, "DoClearSecrets in TXT_SCLEAN_PPI ...\n"));
  DoClearSecrets (&LtPeiCtx) ;
  //
  // Should never get here
  //
  while(1);

  return EFI_SUCCESS ;
}


#ifndef EFI_PLATFORM_TXT_POLICY_NO_SCHECK_IN_RESUME

EFI_STATUS
EFIAPI
BuildScheckCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
ScheckCallback(
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDesc,
  IN VOID                       *Ppi
  );

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        mBootModeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  BuildScheckCallback
};

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        mScheckCallbackNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  ScheckCallback
};

EFI_STATUS
EFIAPI
LtSxMemoryInitializedPpiNotifyCallback(
IN EFI_PEI_SERVICES           **PeiServices,
IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
IN VOID                       *Ppi
);

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        mMemoryInitializedCallbackNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gMemoryInitDoneSentGuid,
  LtSxMemoryInitializedPpiNotifyCallback
};


/**

  Executes the SCHECK function from the LT BIOS ACM on S3 Resume.


  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
ScheckCallback(
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  EFI_STATUS                      Status;
  LT_PEI_LIB_CONTEXT              LtPeiCtx;
  UINT8                           ProcessorLtsxEnableBit;
  UINT8                           LockChipsetBit;
  UINT8                           EVModeBit = 1;

  //
  // Initialize LtPeiCtx
  //
  DEBUG ((EFI_D_INFO, "Pei InitializeLtPeiLib ...\n"));
  Status = InitializeLtPeiLib (PeiServices, &LtPeiCtx);

  //
  // If conditions are met that we used to set LT Lock Config on initial boot, then lock again on S3.
  // Even if InitializeLtPeiLib returns error, we need to lock LtMsr if needed, check Status after locking
  //
  Status = GetProcessorLtsxEnable (PeiServices, &ProcessorLtsxEnableBit);

  //
  // If the helper lib is installed at the platform level
  //  then these GetBit procs will return valid values, else just return
  //
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_INFO, "TxtPei - No helper lib installed at platform level, cannot do LT lock\n"));

  } else {
    Status = GetLockChipset (PeiServices, &LockChipsetBit);
    if (Status == EFI_SUCCESS) { 
      Status = GetEVMode (PeiServices, &EVModeBit);
    }
    if (ProcessorLtsxEnableBit || (!ProcessorLtsxEnableBit && (Status == EFI_SUCCESS) && LockChipsetBit && !EVModeBit)) {
      DEBUG ((EFI_D_INFO, "TxtPei Locking ConfigLock MSR.\n"));
      LockLtMsrOnAll(&LtPeiCtx);
    }
// OVERRIDE_HSD_5332860_BEGIN
	if (!ProcessorLtsxEnableBit || !IsLtEnabled(&LtPeiCtx) || EFI_ERROR(Status)) {
// OVERRIDE_HSD_5332860_END
      return EFI_SUCCESS;
    }
    DEBUG ((EFI_D_INFO, "IsLtEnabled is enabled ...\n"));

    DEBUG ((EFI_D_INFO, "DoLockCongigForAll ...\n"));
    PeiTxtLockConfigForAll (&LtPeiCtx) ;
    DEBUG ((EFI_D_INFO, "DoLockCongigForAll Done!\n"));
  }

  return EFI_SUCCESS ;
}

/**

  Build ScheckCallback

  @param PeiServices       - Pointer to the PEI Services Table.
  @param NotifyDescriptor  - Pointer to the instance of the EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi               - Pointer to the instance of the interface

  @retval EFI_SUCCESS - ScheckCallback is generated successfully

**/
EFI_STATUS
EFIAPI
BuildScheckCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_BOOT_MODE                   BootMode;
  EFI_STATUS                      Status;

  //
  // Get Boot Mode to see if we're coming back from S3
  //
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // If we're resuming from S3 and LT is enabled, we need to run SCHECK
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    //
    // Register SCHECK to be called when BootScript is finished
    // restoring the platform configuration
    //
    Status = (*PeiServices)->NotifyPpi (PeiServices, &mScheckCallbackNotifyList);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

#endif



#ifndef EFI_PLATFORM_TXT_SERVER_EXTENSION
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        mMemoryDiscovertedCallbackNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  MemoryDiscoveredPpiNotifyCallback
};

#endif

#ifdef EFI_PLATFORM_TXT_SERVER_EXTENSION
#if 0

EFI_GUID  gPeiMemoryInitializedPpiGuid =
// {BD039510-6167-4d8c-8FDA-268292515766}
{ 0xbd039510, 0x6167, 0x4d8c, 0x8f, 0xda, 0x26, 0x82, 0x92, 0x51, 0x57, 0x66 };

EFI_STATUS
EFIAPI
LtSxMemoryInitializedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

STATIC
EFI_PEI_NOTIFY_DESCRIPTOR        mMemoryInitializedCallbackNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiMemoryInitializedPpiGuid,
  LtSxMemoryInitializedPpiNotifyCallback
};
#endif
#endif

#ifndef EFI_PLATFORM_TXT_SERVER_EXTENSION
/**

  Executes the ClearSecretsFlag after memory is discovered.
  System should reset in this routine.

  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS              Status;
  LT_PEI_LIB_CONTEXT      LtPeiCtx;

//
//  Debug note:  This function does not get called ??
//

  Status = InitializeLtPeiLib (PeiServices, &LtPeiCtx);
  ASSERT_EFI_ERROR (Status) ;

  //
  // TXT is not enabled in PEI phase, so we enable it here for LT-SX detection.
  //
  EnableTxt (&LtPeiCtx);

  if (IsLtProcessor (&LtPeiCtx)) {
    DEBUG ((EFI_D_ERROR, "DoSclean within notify callback...\n"));
    DoSclean (&LtPeiCtx) ;

    //
    // Reset platform, should not get here
    //
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR (Status);
  }

  return Status ;
}
#endif

#ifdef EFI_PLATFORM_TXT_SERVER_EXTENSION
/**

  Executes the ClearSecretsFlag after memory is discovered.
  System should reset in this routine.

  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
LtSxMemoryInitializedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS              Status;
  LT_PEI_LIB_CONTEXT      LtPeiCtx;
  UINT8                   ProcessorLtsxEnableBit = 0;
  UINT8                   ScleanRequired;

  DEBUG((EFI_D_INFO, "Enter LtSxMemoryInitializedPpiNotifyCallback()\n"));
  Status = InitializeLtPeiLib (PeiServices, &LtPeiCtx);
  ASSERT_EFI_ERROR (Status) ;

  Status = GetProcessorLtsxEnable(PeiServices, &ProcessorLtsxEnableBit);
  ASSERT_EFI_ERROR (Status) ;
  //
  // TXT is not enabled in PEI phase, so we enable it here for LT-SX detection.
  //
  //  EnableTxt (&LtPeiCtx);
  if (ProcessorLtsxEnableBit) {
    ScleanRequired = (((*(volatile UINT32 *)(UINTN)(TXT_PUBLIC_BASE + LT_ESTS)) & BIT6) == 0) ? 0 : 1;
    ScleanRequired |= ((((*(volatile UINT32 *)(UINTN)(TXT_PUBLIC_BASE + LT_E2STS)) & BIT1) == 0) ? 0 : 1);
    DEBUG((EFI_D_INFO, "ScleanRequired (In OemProcMemInit = 0x%02x\n", ScleanRequired));
    if (ScleanRequired)
    {
      // Force LTSX code regardless of CPU fusing as long as system/ bios has LTSX enabled (through FIT table)
      // if (IsLtSxProcessor (&LtPeiCtx)) {
      DEBUG ((EFI_D_ERROR, "ClearSecretsFlag ...\n"));
      DoClearSecrets (&LtPeiCtx) ;
      //
      // Reset platform, should not get here
      //
      Status = EFI_UNSUPPORTED;
      ASSERT_EFI_ERROR ( Status);
      //  }
    }
  }
  return Status ;
}
#endif




EFI_STATUS
EFIAPI
PeimEntry (
  IN          EFI_PEI_FILE_HANDLE   FileHandle,
  IN    CONST EFI_PEI_SERVICES      **PeiServices
  )
/**

  This is the entry point to the LT PEIM.  The LT PEIM checks for an LT
  capable platform and determines whether SCLEAN should be run.  If so,
  it launches the BIOS ACM to run SCLEAN (which will reset the platform).
  If not, the PEIM checks to see if the platform is resuming from S3.

  If the platform is resuming from S3, this code will register a callback
  so that SCHECK will be run when BootScript is done restoring the platform's
  configuration.

  @param FfsHeader     - A pointer the the FFS File containing this PEIM.
  @param PeiServices   - A Pointer to the PEI Services Table.


  @retval EFI_UNSUPPORTED   - If the platform is not LT capable.
  @retval EFI_SUCCESS       - In all other cases not listed above.

**/
{
  EFI_STATUS                      Status;
  LT_PEI_LIB_CONTEXT              LtPeiCtx;

  DEBUG((EFI_D_INFO, "Enter TxtPei \n"));
  //
  // Need to install S3 Callback for any situation on resume
  // The callback will determine what is needed to be done after Boot Script
  //
#ifndef EFI_PLATFORM_TXT_POLICY_NO_SCHECK_IN_RESUME
  Status = (*PeiServices)->NotifyPpi (PeiServices, &mBootModeNotifyList);
  ASSERT_EFI_ERROR (Status);
#endif

  //
  // Initialize the LT PEI Lib functions
  //
  Status = InitializeLtPeiLib (PeiServices, &LtPeiCtx);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install TXT SClean PPI
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mScleanPpiList);
  ASSERT_EFI_ERROR (Status);

  //
  // Install PPI to tell memory code that it can run.
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mTxtMemoryUnlockPpiList);
  ASSERT_EFI_ERROR (Status);

  //
  // Determine TPM presense. If not present - nothing to do.
  // Nothing but presense is precondition of this module since all
  // commands executed by BIOS ACM don't depend on TPM state.
  // TPM_NV_read will be successfully executed even if TPM is disabled
  // and/or deactivated because all indices defined for BIOS ACM
  // usage don't require authorization. TPM_ResetEstablishmentBit
  // doesn't depend on TPM state at all and can
  // be executed with disabled/deactivated TPM always.
  // Case when TPM is comletely not functional is not cinsidered.
  //
  if (!IsTpmPresent (&LtPeiCtx)) {
    DEBUG ((EFI_D_ERROR, "LtPeim Error: TPM is not present\n"));
#ifdef MKF_TPM_PRESENT
    return EFI_UNSUPPORTED;
#endif
  }

#if 0
  _asm {
     jmp $
  }
#endif

  //
  // Detect LT capable Processor
  //
  if (!IsLtProcessor (&LtPeiCtx)) {
    DEBUG ((EFI_D_ERROR, "LtPeim Error: CPU does not support TXT\n"));
    return EFI_SUCCESS;
  }
#if 0
  //
  // Check reset status
  //
  if (IsLtResetStatus (&LtPeiCtx)) {
    DEBUG (( EFI_D_ERROR, "IsLtResetStatus: Reset system ...\n"));
    PowerCycleReset (&LtPeiCtx);

    //
    // Reset platform, should not get here
    //
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  }
#endif
  //
  // Check if we need to run SCLEAN
  //
  if (PchIsDwrFlow()) {
    DEBUG((EFI_D_ERROR, "[TxtPei] WARNING: DWR detected - skip Tpm detection in TxtPei module\n"));
    return EFI_UNSUPPORTED;
  }

  if (IsLtEstablished (&LtPeiCtx) && IsLtWakeError (&LtPeiCtx)) {
#ifndef EFI_PLATFORM_TXT_SERVER_EXTENSION
    //
    // Setup callback for DoSclean
    //
    Status = (*PeiServices)->NotifyPpi (PeiServices, &mMemoryDiscovertedCallbackNotifyList);
  ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
#else


    //
    // Setup callback for ClearSecretsFlag
    //
    Status = (*PeiServices)->NotifyPpi (PeiServices, &mMemoryInitializedCallbackNotifyList);
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
#endif
  }

  return EFI_SUCCESS ;
}

//EFI_PEIM_ENTRY_POINT(PeimEntry) ;
