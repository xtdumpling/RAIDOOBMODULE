/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

  Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file TxtPeiLib.c

    This file contains an implementation of the function call interfaces
    required by the main LT PEIM file.  Hopefully, future platform porting
    tasks will be mostly limited to modifying the functions in this file.

**/

#include "TxtPeiLib.h"

#include <Library/LtPeiLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include "Universal\GetSec\Common\LaunchBiosAcm.h"
// APTIOV_SERVER_OVERRIDE_RC_END

//
// LT Configuration Space Register Definitions
//
#define LT_PUBLIC_SPACE_BASE_ADDRESS     0xfed30000
#define LT_ESTS_OFF                      0x8
#define   LT_TXT_RESET_STS               0x1
#define   LT_WAKE_ERROR_STS              (0x1 << 6)
#define LT_HEAP_BASE_REG_OFF             0x300
#define LT_HEAP_SIZE_REG_OFF             0x308

#define LT_TPM_SPACE_BASE_ADDRESS        0xfed40000
#define LT_TPM_SPACE_ACCESS0             0x0

#define CPUID_SMX_BIT                   (1 << 6)
#define N_MSR_BSP_FLAG                   8

#define GETSEC_PARAMETER_TYPE_MASK                0x1F
#define GETSEC_PARAMETER_TYPE_LT_EXTENSIONS       0x05
#define GETSEC_PARAMETER_TYPE_LT_EXTENSIONS_FLAGS 0x20

#define FEATURE_LOCK_BIT                0x0001
#define FEATURE_VMON_IN_SMX             0x0002
#define FEATURE_VMON_OUT_SMX            0x0004
#define FEATURE_SENTER_ENABLE           0xFF00

EFI_GUID mTxtApInitGuid = { 0x9b4a2f66, 0x6966, 0x4c44, { 0xa1, 0x92, 0x4f, 0xee, 0x28, 0x84, 0x4d, 0xb5 } };

STATIC
EFI_STATUS
GetApVector (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  );

/**

  This routine initializes and collects all PPIs and data required
  by the routines in this file.

  @param PeiServices - A pointer to the PEI Service Table
  @param LtPeiCtx    - A pointer to a caller allocated data structure that contains
                       all of the PPIs and data required by the routines in this file

  @retval EFI_UNSUPPORTED   - If any of the required PPIs or data are unavailable
  @retval EFI_SUCCESS       - In all cases not listed above

**/
EFI_STATUS
InitializeLtPeiLib (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN OUT  LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  EFI_STATUS              Status;
  EFI_HOB_GUID_TYPE       *GuidHob;

  //
  // Initialize all pointers
  //
  LtPeiCtx->PeiServices           = PeiServices;
  LtPeiCtx->CpuIoPpi              = (*(LtPeiCtx->PeiServices))->CpuIo;
  LtPeiCtx->PciCfgPpi             = (*(LtPeiCtx->PeiServices))->PciCfg;

  Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &(LtPeiCtx->PeiStall));
  ASSERT_EFI_ERROR(Status);

  //
  // Find PeiMpServices PPI, only need success locating at end of S3 resume
  //
  Status = GetPeiMpServices (PeiServices, &(LtPeiCtx->PeiMpServices));

  //
  // Txt memory hob
  //
  GuidHob = GetFirstGuidHob (&gEfiPlatformTxtDeviceMemoryGuid);
  if (GuidHob == NULL) {
    DEBUG (( EFI_D_ERROR, "GUID HOB gEfiPlatformTxtDeviceMemoryGuid not found!\n"));
    LtPeiCtx->PlatformTxtDeviceMemory = NULL;
  } else {
    LtPeiCtx->PlatformTxtDeviceMemory = (EFI_PLATFORM_TXT_DEVICE_MEMORY *) GET_GUID_HOB_DATA (GuidHob);
  }

  //
  // Txt Policy hob
  //
  GuidHob = GetFirstGuidHob (&gEfiPlatformTxtPolicyDataGuid);
  if (GuidHob == NULL) {
    DEBUG (( EFI_D_ERROR, "GUID HOB gEfiPlatformTxtPolicyDataGuid not found! Error when do TxtPei Init.\n"));
    return EFI_NOT_FOUND;
  }
  LtPeiCtx->PlatformTxtPolicyData = (EFI_PLATFORM_TXT_POLICY_DATA *) GET_GUID_HOB_DATA (GuidHob);


  DEBUG (( EFI_D_ERROR, "PlatformTxtPolicy:\n"));
  DEBUG (( EFI_D_ERROR, "  BiosAcmAddress             - 0x%08x\n", LtPeiCtx->PlatformTxtPolicyData->BiosAcmAddress));
  DEBUG (( EFI_D_ERROR, "  TxtScratchAddress          - 0x%08x\n", LtPeiCtx->PlatformTxtPolicyData->TxtScratchAddress));
  DEBUG (( EFI_D_ERROR, "  BiosAcmPolicy              - 0x%08x\n", LtPeiCtx->PlatformTxtPolicyData->BiosAcmPolicy));

  Status = GetApVector (LtPeiCtx);
  DEBUG (( EFI_D_ERROR, "ApVector - 0x%08x\n", LtPeiCtx->ApVector));

  return EFI_SUCCESS ;
}

/**

  Determines whether or not the current processor is LT Capable.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the current processor supports LT
  @retval FALSE         - If the current processor does not support LT

**/
BOOLEAN
IsLtProcessor (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  EFI_CPUID_REGISTER      CpuidRegs;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
  return (CpuidRegs.RegEcx & CPUID_SMX_BIT) ? TRUE : FALSE;
}

/**

  Enable TXT on BSP.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval None

**/
VOID
EnableTxt (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  //
  // msr(0x3A) = 0xFF07
  AsmWriteMsr64  (EFI_MSR_IA32_FEATURE_CONTROL, AsmReadMsr64 (EFI_MSR_IA32_FEATURE_CONTROL) | (FEATURE_VMON_IN_SMX | FEATURE_VMON_OUT_SMX));
  return ;
}

/**

  Sets the Lock Bit in the LT Control MSR.  Called by all threads.

  @param Not used

  @retval EFI_SUCCESS   - Always.

**/
VOID
LockLtMsr (
  IN      VOID      *Buffer
  )
{
  //
  // Set LT_CONTROL_LOCK
  //
  AsmWriteMsr64 (EFI_MSR_LT_CONTROL, 0x1);
  return;
}

/**

  Lock LT MSR on all CPU threads.

  @param LtPeiCtx - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS           - Multiple processors re-load microcode patch

**/
VOID
LockLtMsrOnAll(
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  EFI_STATUS  Status;

  //
  // Lock LT Control MSR on BSP, then on all APs
  //
  LockLtMsr(NULL);
  Status = LtPeiCtx->PeiMpServices->StartupAllAPs (
            LtPeiCtx->PeiServices,
            LtPeiCtx->PeiMpServices,
            LockLtMsr,
            FALSE,
            0,
            (VOID*)NULL
            );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Error: Not able to Lock LT MSR.  Status: %r\n", Status));
  }

  return;
}

#ifdef EFI_PLATFORM_TXT_SERVER_EXTENSION
#if 0
/**

  Determines whether or not the current processor is LT-SX Capable.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the current processor supports LT-SX
  @retval FALSE         - If the current processor does not support LT-SX

**/
BOOLEAN
IsLtSxProcessor (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT32   ParameterValue;
  UINT32   ParameterIndex;
  BOOLEAN  LtSxSupported;

  LtSxSupported = FALSE;
  ParameterIndex = 0;
  //
  // Go through all parameters
  //
  do {
    AsmGetSecParameters (ParameterIndex, &ParameterValue, NULL, NULL);
    if ((ParameterValue & GETSEC_PARAMETER_TYPE_MASK) == GETSEC_PARAMETER_TYPE_LT_EXTENSIONS) {
      //
      // Lt Extensions support parameter found
      //
      if (ParameterValue & GETSEC_PARAMETER_TYPE_LT_EXTENSIONS_FLAGS) {
        LtSxSupported = TRUE;
      }
      break;
    }
    ParameterIndex ++;
  } while ((ParameterValue & GETSEC_PARAMETER_TYPE_MASK) != 0);

  return LtSxSupported;
}
#endif
#endif

/**

  Determines presence of TPM in system

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the TPM is present.
  @retval FALSE         - If the TPM is not present.

**/
BOOLEAN
IsTpmPresent (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
/*              Replaced next block with line below. might need to check setup option rather than actaul TPM presence.
                TPM existance and/or functionality is not essential for DPR programming (which is part of LT code),
                nor TPM functionality is needed for LT functionality (accroding to the calling function)

  UINT8                           Access ;

  //
  // Read TPM status register
  //
  Access = LtPeiCtx->CpuIoPpi->MemRead8 (
                                 LtPeiCtx->PeiServices,
                                 LtPeiCtx->CpuIoPpi,
                                 LT_TPM_SPACE_BASE_ADDRESS + LT_TPM_SPACE_ACCESS0
                                 );
  return (Access == 0xff) ? FALSE : TRUE;
*/
  return TRUE;
}

/**

  Determines whether or not the platform has executed an LT launch by
  examining the TPM Establishment bit.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.

**/
BOOLEAN
IsLtEstablished (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
#ifdef MKF_TPM_PRESENT
  UINT8                           Access;

  //
  // Read TPM status register
  //
  Access = LtPeiCtx->CpuIoPpi->MemRead8 (
                                 LtPeiCtx->PeiServices,
                                 LtPeiCtx->CpuIoPpi,
                                 LT_TPM_SPACE_BASE_ADDRESS + LT_TPM_SPACE_ACCESS0
                                 );

  //
  // The bit we're interested in uses negative logic:
  // If bit 0 == 1 then return False
  // Else return True
  //
  return (Access & 0x1) ? FALSE : TRUE;
#else
  return TRUE;
#endif
}

/**

  Determines whether or not the platform has encountered an error during
  a sleep or power-off state.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the LT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the LT_WAKE_ERROR bit is unasserted.

**/
BOOLEAN
IsLtWakeError (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT8       Ests;

  //
  // Read LT.ESTS register
  //
  Ests = LtPeiCtx->CpuIoPpi->MemRead8(
                               LtPeiCtx->PeiServices,
                               LtPeiCtx->CpuIoPpi,
                               LT_PUBLIC_SPACE_BASE_ADDRESS + LT_ESTS_OFF
                               );


  return (Ests & LT_WAKE_ERROR_STS) ? TRUE : FALSE;
}

/**

  Determines whether TXT Reset Status is set

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the LT_RESET_STATUS bit is asserted.
  @retval FALSE         - If the LT_RESET_STATUS bit is unasserted.

**/
BOOLEAN
IsLtResetStatus (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT8       Ests;

  //
  // Read LT.ESTS register
  //
  Ests = LtPeiCtx->CpuIoPpi->MemRead8(
                               LtPeiCtx->PeiServices,
                               LtPeiCtx->CpuIoPpi,
                               LT_PUBLIC_SPACE_BASE_ADDRESS + LT_ESTS_OFF
                               );

  return (Ests & LT_TXT_RESET_STS) ? TRUE : FALSE;
}

/**

  Determines whether or not the platform requires initialization for LT use.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval TRUE          - If the the platoform should be configured for LT.
  @retval FALSE         - If LT is not to be used.

**/
BOOLEAN
IsLtEnabled (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT64  Ia32FeatureControl;

  Ia32FeatureControl = AsmReadMsr64(EFI_MSR_IA32_FEATURE_CONTROL);
  if (((Ia32FeatureControl & FEATURE_SENTER_ENABLE) != FEATURE_SENTER_ENABLE) ||
      ((Ia32FeatureControl & FEATURE_VMON_IN_SMX)   != FEATURE_VMON_IN_SMX) ) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**

  Do system power cycle reset

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval None

**/
VOID
PowerCycleReset (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  LtPeiCtx->CpuIoPpi->IoWrite8 (
                          LtPeiCtx->PeiServices,
                          LtPeiCtx->CpuIoPpi,
                          0xCF9,
                          0xA
                          );
  LtPeiCtx->CpuIoPpi->IoWrite8 (
                          LtPeiCtx->PeiServices,
                          LtPeiCtx->CpuIoPpi,
                          0xCF9,
                          0xE
                          );
  while(1);
}

/**

Re-initialize APs after running ACM commands

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval - None

**/
STATIC
VOID
InitializeAps (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT64   MsrData;
  UINT32   McuRevision;

  //
  // 1. Check BSP Microcode update done. (Minimal check)
  //
  MsrData = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  McuRevision = (UINT32)RShiftU64 (MsrData, 32);
  ASSERT (McuRevision != 0);

  //
  // 2. Send SIPI to let AP do something, currently only Microcode update.
  //    Platform-specific code could also be put here and/or ApInit code.
  //
  if (LtPeiCtx->ApVector != 0) {
    SendInitSipiSipiAllExcludingSelf(LtPeiCtx->ApVector << 12);  //Library function shifts right 12 bits

    //
    // We delay some time to let ap finish its task.
    // 40ms delay (Hardcoded now, should use semaphore or something else)
    //
    LtPeiCtx->PeiStall->Stall (LtPeiCtx->PeiServices, LtPeiCtx->PeiStall, 40000);
  }

  return ;
}

/**

  Searches FFS for file with pGuid

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure
  @param Guid          - FFS guid
  @param Module        - FFS module address

  @retval EFI_SUCCESS   - File is found.
  @retval Other status  - File not found.

**/
STATIC
EFI_STATUS
FindModuleInFlash (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx,
  IN      EFI_GUID                *Guid,
     OUT  UINTN                   *Module
  )
{
#if (PI_SPECIFICATION_VERSION < 0x00010000)
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  EFI_FFS_FILE_HEADER         *FileHeader;
#else
  EFI_PEI_FV_HANDLE           VolumeHandle;
  EFI_PEI_FILE_HANDLE         *FileHandle;
  EFI_PEI_FV_HANDLE           *FvHeader;
  EFI_FFS_FILE_HEADER         *FileHeader;
#endif
  EFI_STATUS                  Status;
  UINTN                       Instance;

  Instance = 0;
  FvHeader = NULL;
  while (TRUE) {
#if (PI_SPECIFICATION_VERSION < 0x00010000)
    Status = (*(LtPeiCtx->PeiServices))->FfsFindNextVolume (LtPeiCtx->PeiServices, Instance, &FvHeader);
#else
    VolumeHandle = NULL;
    Status = (*(LtPeiCtx->PeiServices))->FfsFindNextVolume (LtPeiCtx->PeiServices, Instance, &VolumeHandle);
    FvHeader = &VolumeHandle;
#endif
    if (EFI_ERROR(Status)) {
      return Status;
    }

    FileHeader = NULL;
#if (PI_SPECIFICATION_VERSION >= 0x00010000)
    FileHandle = (EFI_PEI_FILE_HANDLE *)&FileHeader;
#endif
    while (TRUE) {
#if (PI_SPECIFICATION_VERSION < 0x00010000)
      Status = (*(LtPeiCtx->PeiServices))->FfsFindNextFile (LtPeiCtx->PeiServices, EFI_FV_FILETYPE_RAW, FvHeader, &FileHeader);
#else
      // APTIOV_SERVER_OVERRIDE_RC_START : Following change resolves build error.
      //Status = (*(LtPeiCtx->PeiServices))->FfsFindNextFile (LtPeiCtx->PeiServices, EFI_FV_FILETYPE_RAW, FvHeader, &FileHandle);
      Status = (*(LtPeiCtx->PeiServices))->FfsFindNextFile (LtPeiCtx->PeiServices, EFI_FV_FILETYPE_RAW, FvHeader, FileHandle);
      // APTIOV_SERVER_OVERRIDE_RC_END : Following change resolves build error.
      // According to PI spec, FileHandle is FileHeader.
      FileHeader = (EFI_FFS_FILE_HEADER *)*FileHandle;
#endif
      if (EFI_ERROR (Status)) {
        break;
      }

      if (CompareGuid (&(FileHeader->Name), Guid)) {
        *Module = (UINTN)FileHeader + sizeof(EFI_FFS_FILE_HEADER);
        return EFI_SUCCESS;
      }
    }
    Instance ++;
  }
}

/**

  Find a memory in E/F segment for AP reset vector.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval ApVector      - The ApVector in E/F segment.

**/
STATIC
EFI_STATUS
GetApVector (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  EFI_STATUS                  Status;
  UINTN                       ApStartup;

  LtPeiCtx->ApVector = 0;
  Status = FindModuleInFlash (LtPeiCtx, &mTxtApInitGuid, &ApStartup);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Found ApInit module
  //
  if (((ApStartup & 0xFFF) == 0) &&
      (ApStartup >= 0xFFFE0000) &&
      (ApStartup <= 0xFFFFF000)) {
    LtPeiCtx->ApVector = (UINT8)(ApStartup >> 12);
    return EFI_SUCCESS;
  } else {
    ASSERT ((ApStartup & 0xFFF) == 0);
    ASSERT ((ApStartup >= 0xFFFE0000) &&  (ApStartup <= 0xFFFFF000));
    return EFI_NOT_FOUND;
  }
}

//
//  Start of LT SX function interface into BIOS ACM module. Note: Only function 5 is here. Function 6 is in DXE
//

//
// Function 5 interface into StartupACM
//
/**

  Invokes the LT_CLEAR_SECRETS function in the LT StartupACM.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
DoClearSecrets (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
#ifdef EFI_PLATFORM_TXT_SERVER_EXTENSION
  if (LtPeiCtx->PlatformTxtPolicyData->StartupAcmAddress!=0) {
    InitializeAps (LtPeiCtx);
    SendInitIpiAllExcludingSelf();
// BEGIN_OVERRIDE_HSD_5331848  
    LtPeiLibLaunchBiosAcm(LtPeiCtx, LT_CLEAR_SECRETS);
// END_OVERRIDE_HSD_5331848
  } else {
//    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Did not call ACM. The ACM module is not enabled ...\n"));
    DEBUG (( EFI_D_ERROR, "Did not call ACM. The ACM module is not enabled ...\n"));
  }
#endif

  return EFI_SUCCESS ;
}

#ifndef EFI_PLATFORM_TXT_SERVER_EXTENSION
/**

  Invokes the SCLEAN function from the LT BIOS ACM.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
DoSclean (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  if (LtPeiCtx->PlatformTxtPolicyData->StartupAcmAddress!=0) {
    InitializeAps (LtPeiCtx) ;
    SendInitIpiAllExcludingSelf();
// BEGIN_OVERRIDE_HSD_5331848 
    LtPeiLibLaunchBiosAcm(LtPeiCtx, LT_LAUNCH_SCLEAN);
// END_OVERRIDE_HSD_5331848
  } else {
//    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Did not call ACM. The ACM module is not enabled ...\n"));
  }

  return EFI_SUCCESS ;
}

/**

  Invokes the ClearSecretsFlag function from the LT BIOS ACM.
  This is for TXT server extension only.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
ClearSecretsFlag (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  if (LtPeiCtx->PlatformTxtPolicyData->StartupAcmAddress!=0) {
    InitializeAps (LtPeiCtx) ;
    SendInitIpiAllExcludingSelf();
    //
    // Run ClearSecretsFlag need not Check and Register policy
    //
// BEGIN_OVERRIDE_HSD_5331848 
    LtPeiLibLaunchBiosAcm(LtPeiCtx, LT_CLEAR_SECRETS);
// END_OVERRIDE_HSD_5331848
  } else {
//    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Did not call ACM. The ACM module is not enabled ...\n"));
  }

  return EFI_SUCCESS ;
}
#endif

/**

  Calls DoLockConfigForAll to run LockConfig on all sockets then restores HEAP registers.

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
PeiTxtLockConfigForAll (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINT64          *Ptr64;

  PeiDoLockConfigForAll (LtPeiCtx);

  //
  // TXTHEAP Registers reset after lockconfig
  //
  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_HEAP_BASE_REG_OFF);
  DEBUG ((EFI_D_INFO, "  Restoring TXTHeapMemoryAddress = 0x%08x... \n", LtPeiCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress));
  *Ptr64      = LtPeiCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress;

  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_HEAP_SIZE_REG_OFF);
  DEBUG ((EFI_D_INFO, "  Restoring TXTHeapMemorySize = 0x%08x... \n", LtPeiCtx->PlatformTxtDeviceMemory->TXTHeapMemorySize));
  *Ptr64      = LtPeiCtx->PlatformTxtDeviceMemory->TXTHeapMemorySize;


  return EFI_SUCCESS;
}

/**

  Performs the action of calling DoLockConfig on all sockets and all necessary initialization

  @param LtPeiCtx      - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
PeiDoLockConfigForAll (
  IN      LT_PEI_LIB_CONTEXT      *LtPeiCtx
  )
{
  UINTN                       Index, Index1;
  EFI_STATUS                  Status;
  UINTN                       CpuCount;
  UINTN                       CpuCountEnabled;
  EFI_PROCESSOR_INFORMATION   CpuInfoBuffer;
  LT_PEI_SOCKET_INFO          SocketInfo[MAX_SOCKET_COUNT];
  UINT8                       Temp_SocketID, SocketID_BSP;
  UINT32                      mNumOfBitShift;

  //
  // Initialize SocketInfo to all 0xFF
  //
  SetMem(&SocketInfo, sizeof(LT_PEI_SOCKET_INFO)*MAX_SOCKET_COUNT, 0xff);

  //
  // Get total threads to be used to find all sockets
  //
  Status = LtPeiCtx->PeiMpServices->GetNumberOfProcessors(
              LtPeiCtx->PeiServices,
              LtPeiCtx->PeiMpServices,
              &CpuCount,
              &CpuCountEnabled
              );
  ASSERT_EFI_ERROR (Status);

  //
  // Get BSP index so we can get the processor information
  //
  LtPeiCtx->PeiMpServices->WhoAmI (LtPeiCtx->PeiServices, LtPeiCtx->PeiMpServices, &Index);

  //
  // Find out the bit shift so it can be used to identify APs on other sockets
  //
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, &mNumOfBitShift, NULL, NULL, NULL);
  mNumOfBitShift &= 0x1F;
  DEBUG(( EFI_D_INFO, "mNumofBitShift! %d\n", mNumOfBitShift));

  Status = LtPeiCtx->PeiMpServices->GetProcessorInfo (
              LtPeiCtx->PeiServices,
              LtPeiCtx->PeiMpServices,
              Index,
              &CpuInfoBuffer
              );
  ASSERT_EFI_ERROR (Status);

  //
  // Set the socket ID of the BSP to be used in identifying APs on other sockets
  //
  SocketID_BSP = (UINT8)(CpuInfoBuffer.ProcessorId)>>mNumOfBitShift;

  //
  // Loop through all threads to find all the sockets present
  //
  for (Index = 0; Index < CpuCount + 1; Index++) {
    Status = LtPeiCtx->PeiMpServices->GetProcessorInfo (
              LtPeiCtx->PeiServices,
              LtPeiCtx->PeiMpServices,
              Index,
              &CpuInfoBuffer
              );
    Temp_SocketID = (UINT8)(CpuInfoBuffer.ProcessorId)>>mNumOfBitShift;  //Get socket ID
    if (Temp_SocketID == SocketID_BSP) {
      continue;
    }
    for (Index1 = 0; Index1 < MAX_SOCKET_COUNT; Index1++) {
      if (SocketInfo[Index1].SocketFoundId == Temp_SocketID) {
        break;
      }
      //
      // Make sure this entry is unused
      //
      if (SocketInfo[Index1].SocketIndex == 0xFF) {
        //
        // Found an AP on another socket, store the index, Socket ID, and APIC ID of AP
        //
        SocketInfo[Index1].SocketFoundId = Temp_SocketID;
        SocketInfo[Index1].SocketIndex = (UINT8)Index;
        SocketInfo[Index1].ApApicId = (UINT16)CpuInfoBuffer.ProcessorId;
        break;
      }
    }
  }

  //
  // Send init to all APs so they are waiting for SIPI
  //
  SendInitIpiAllExcludingSelf();

  //
  // Call BIOS ACM to lock config on BSP
  //
  DEBUG ((EFI_D_INFO, "DoLockConfig for BSP\n"));
  DoLockConfig((VOID *) LtPeiCtx);
  DEBUG ((EFI_D_INFO, "Done! \n"));


  for (Index = 0; Index < MAX_SOCKET_COUNT; Index++) {
    if (SocketInfo[Index].SocketIndex == 0xFF) {
      break;
    }
    // Index = AP on other socket
    //
    // Call StartupThisAP to run DoLockConfig
    //
    DEBUG ((EFI_D_INFO, "DoLockConfig for Socket: %d AP: %d\n", SocketInfo[Index].SocketFoundId + 1, (UINTN)SocketInfo[Index].SocketIndex));
    Status = LtPeiCtx->PeiMpServices->StartupThisAP (
              LtPeiCtx->PeiServices,
              LtPeiCtx->PeiMpServices,
              DoLockConfigOnAP,
              (UINTN)SocketInfo[Index].SocketIndex,
              0,
              (VOID*)LtPeiCtx
              );
    //
    // Set the AP we just used back into wait for SIPI state
    //
    SendInitIpi((UINT32)SocketInfo[Index].ApApicId);

    DEBUG ((EFI_D_INFO, "Done! \n"));
    if (EFI_ERROR(Status)) {
      break;
    }
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
DoLockConfigOnAP (
  IN      VOID      *Buffer
  )
/**

  Invokes the LOCK CONFIG function from the LT BIOS ACM for the AP that called.

  @param Buffer -      LtPeiCtx - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
{
  LT_PEI_LIB_CONTEXT      *LtPeiCtx = (LT_PEI_LIB_CONTEXT*)Buffer;

  //
  // Set BSP flag for this AP before launching BiosAcm
  //
  AsmMsrBitFieldWrite64 (EFI_MSR_IA32_APIC_BASE, N_MSR_BSP_FLAG, N_MSR_BSP_FLAG, 1);

  if (LtPeiCtx->PlatformTxtPolicyData->StartupAcmAddress!=0) {
// BEGIN_OVERRIDE_HSD_5331848 
   LtPeiLibLaunchBiosAcm(LtPeiCtx, LT_LOCK_CONFIG);
// END_OVERRIDE_HSD_5331848
  }

  //
  // Clear BSP flag for this AP after done launching BiosAcm
  //
  AsmMsrBitFieldWrite64 (EFI_MSR_IA32_APIC_BASE, N_MSR_BSP_FLAG, N_MSR_BSP_FLAG, 0);

  return;

}

/**

  Invokes the LOCK CONFIG function from the LT BIOS ACM.

  @param Buffer -      LtPeiCtx - A pointer to an initialized LT PEI Context data structure

  @retval EFI_SUCCESS   - Always.

**/
VOID
DoLockConfig (
  IN      VOID      *Buffer
  )
{

  LT_PEI_LIB_CONTEXT      *LtPeiCtx = (LT_PEI_LIB_CONTEXT*)Buffer;

  if (LtPeiCtx->PlatformTxtPolicyData->StartupAcmAddress!=0) {
// BEGIN_OVERRIDE_HSD_5331848 
    LtPeiLibLaunchBiosAcm(LtPeiCtx, LT_LOCK_CONFIG);
// END_OVERRIDE_HSD_5331848
  } else {
//    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Did not call ACM. The ACM module is not enabled ...\n"));
  }

  return;
}

// BEGIN_OVERRIDE_HSD_5331848 
/**

Sets up the system and then launches the LT BIOS ACM to run the function
requested by AcmFunction.

@param AcmFunction   - Constant that represents the function from the BIOS ACM
that should be executed.

@retval EFI_SUCCESS   - Always.

**/
STATIC
EFI_STATUS
LtPeiLibLaunchBiosAcm(
IN LT_PEI_LIB_CONTEXT      *LtPeiCtx,
IN UINT32                  AcmFunction
)
{
  UINT32                      Ia32_Mc9_Ctl_Org;
  UINT32                      Ia32_Mc10_Ctl_Org;
  UINT32                      Ia32_Mc11_Ctl_Org;

  UINT32                      Ia32_Mc9_Ctl_New;
  UINT32                      Ia32_Mc10_Ctl_New;
  UINT32                      Ia32_Mc11_Ctl_New;

  UINT32                      Ia32_Mcg_Contain_Org = 0;
  UINT32                      Ia32_Mcg_Contain_New = 0;

#define MC9_CTL  0x424
#define MC10_CTL 0x428
#define MC11_CTL 0x42C

#define MC9_STATUS  0x425
#define MC10_STATUS 0x429
#define MC11_STATUS 0x42D

#define MSR_MCG_CONTAIN       0x178
#define MSR_IA32_MCG_CAP      0x179

  Ia32_Mc9_Ctl_Org  = AsmReadMsr32(MC9_CTL);
  Ia32_Mc10_Ctl_Org = AsmReadMsr32(MC10_CTL);
  Ia32_Mc11_Ctl_Org = AsmReadMsr32(MC11_CTL);
//To clear bit7 SAD_ERR_NON_CORRUPTING_OTHER 
  Ia32_Mc9_Ctl_New  = Ia32_Mc9_Ctl_Org & ~BIT7;
  Ia32_Mc10_Ctl_New = Ia32_Mc10_Ctl_Org & ~BIT7;
  Ia32_Mc11_Ctl_New = Ia32_Mc11_Ctl_Org & ~BIT7;

  if ((AsmReadMsr32(MSR_IA32_MCG_CAP) & BIT24) != 0) {
  //  IF IA32_MCG_CAP.MCG_SER_P == 1:
    Ia32_Mcg_Contain_Org = AsmReadMsr32(MSR_MCG_CONTAIN);
    AsmWriteMsr32(MSR_MCG_CONTAIN, Ia32_Mcg_Contain_New);
    //  Write a value of 0 to MCG_CONTAIN;
  }

    AsmWriteMsr32(MC9_CTL,  Ia32_Mc9_Ctl_New);
    AsmWriteMsr32(MC10_CTL, Ia32_Mc10_Ctl_New);
    AsmWriteMsr32(MC11_CTL, Ia32_Mc11_Ctl_New);
    //
    // Launch the BIOS ACM to run the requested function
    //
#if 1
    DEBUG((EFI_D_ERROR, "LtPeiLibLaunchBiosAcm: BiosAcmAddress = 0x % 08x\n", (UINTN)LtPeiCtx->PlatformTxtPolicyData->BiosAcmAddress));
#endif
    LaunchBiosAcm((UINTN)LtPeiCtx->PlatformTxtPolicyData->BiosAcmAddress, (UINTN)AcmFunction);

    AsmWriteMsr32(MC9_STATUS,  0);
    AsmWriteMsr32(MC10_STATUS, 0);
    AsmWriteMsr32(MC11_STATUS, 0);

    AsmWriteMsr32(MC9_CTL,  Ia32_Mc9_Ctl_Org);
    AsmWriteMsr32(MC10_CTL, Ia32_Mc10_Ctl_Org);
    AsmWriteMsr32(MC11_CTL, Ia32_Mc11_Ctl_Org);

    if ((AsmReadMsr32(MSR_IA32_MCG_CAP) & BIT24) != 0) {
    //  IF IA32_MCG_CAP.MCG_SER_P == 1:
      AsmWriteMsr32(MSR_MCG_CONTAIN, Ia32_Mcg_Contain_Org);
      //  Write your saved value back to MCG_CONTAIN
    }

    return EFI_SUCCESS;
}
// END_OVERRIDE_HSD_5331848