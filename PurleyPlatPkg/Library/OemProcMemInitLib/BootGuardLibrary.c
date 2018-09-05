/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file 
  BootGuardLibrary.c

  BootGuardLibrary implementation. 

**/

#include <Library/BootGuardLib.h>
#include "EdkProcMemInit.h"
#include <HeciRegs.h>
#include <MeChipset.h>

BOOLEAN
IsBootGuardSupported (
  VOID
)
/**

@brief

  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
{
  UINT64          LtStatusRegister;
  UINT64          BootGuardBootStatus;
  UINT64          BootGuardAcmError;
  UINT64          BootGuardCapability;
  UINT64          LtExtendedStatusError;

  LtStatusRegister      = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE);                                   //0x0000
  LtExtendedStatusError = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + LT_EXTENDED_STATUS_REGISTER);     //0x0008,
  BootGuardBootStatus = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);    //0xA0, LT_SPAD
  BootGuardAcmError     = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_ERROR);    // 0x328, LT_BIOSACMCode

  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_CAPABILITY;  // Bit 32

   DEBUG((EFI_D_ERROR, "LtStatusRegister = 0x%016lx\n", LtStatusRegister));
   DEBUG((EFI_D_ERROR, "LtExtendedStatusError = 0x%016lx\n", LtExtendedStatusError));
   DEBUG((EFI_D_ERROR, "BootGuardBootStatus = 0x%016lx\n", BootGuardBootStatus));
   DEBUG((EFI_D_ERROR, "BootGuardAcmError = 0x%016lx\n", BootGuardAcmError));
   DEBUG((EFI_D_ERROR, "MSR_BOOT_GUARD_SACM_INFO = 0x%016lx\n", AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO)));

  if (BootGuardCapability != 0) {
    DEBUG ((DEBUG_INFO, "Processor supports Boot Guard.\n"));
    return TRUE;
  } else {
    DEBUG ((DEBUG_INFO, "Processor does not support Boot Guard.\n"));
    return FALSE;
  }
}

EFI_STATUS
StopPbeTimer (
  VOID
)
/**

@brief

  Stop PBE timer if system is in Boot Guard boot

  @retval EFI_SUCCESS        - Stop PBE timer
  @retval EFI_UNSUPPORTED    - Not in Boot Guard boot mode.

**/
{
  UINT64   BootGuardBootStatus;
  UINT64   BootGuardOperationMode;

  if (IsBootGuardSupported()) {
    BootGuardBootStatus = (*(UINT64 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS));
    BootGuardBootStatus &= V_CPU_BOOT_GUARD_LOAD_ACM_FAILED;
    BootGuardOperationMode = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & (B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT | B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT);

    //
    // Stop PBET if Verified/Measured/NEM bit is set in MSR 0x13A or
    // Boot Guard fails to launch or fails to execute successfully for avoiding brick platform
    //

    if (BootGuardBootStatus != V_CPU_BOOT_GUARD_LOAD_ACM_FAILED) {        // Bit 61, LT_SPAD BTG Failed Bit
      if (BootGuardOperationMode == 0) {
        DEBUG ((DEBUG_INFO, "Platform is in Legacy boot mode.\n"));
        return EFI_UNSUPPORTED;
      } else {
        DEBUG ((DEBUG_INFO, "Platform in Boot Guard Boot mode.\n"));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "Boot Guard ACM launch failed or ACM execution failed.\n"));
    }

    DEBUG ((DEBUG_INFO, "Disable PBET\n"));
//    "Write Msr to stop PBETIMER" moves to SecPei phase (ProcessorStartupCore.asm)
//    AsmWriteMsr64 (MSR_BC_PBEC, B_STOP_PBET); 
  } else {
    DEBUG ((DEBUG_ERROR, "Boot Guard is not supported.\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
Report platform specific Boot Guard information.

@param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/

VOID
GetBootGuardInfo(
  OUT BOOT_GUARD_INFO *BootGuardInfo
  )
{
  UINT32                  MsrValue;
  HECI_GS_SHDW_REGISTER   MeFwSts2;
  UINT32                  MeFwSts4;
  UINT32                  MeFwSts5;
  UINT32                  MeFwSts6;
  UINTN                   HeciBaseAddress;
  UINT32                  BootGuardAcmStatus;
  UINT32                  BootGuardBootStatus;

  ///
  /// Check if System Supports Boot Guard
  ///
  if (IsBootGuardSupported()) {
    BootGuardInfo->BootGuardCapability = TRUE;

    BootGuardAcmStatus = *(UINT32 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
    DEBUG((DEBUG_INFO, "Boot Guard ACM Status = %x\n", BootGuardAcmStatus));

    BootGuardBootStatus = *(UINT32 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
    DEBUG((DEBUG_INFO, "Boot Guard Boot Status = %x\n", BootGuardBootStatus));

    ///
    /// Read ME FWS Registers
    ///
    HeciBaseAddress = MmPciBase(
      ME_BUS,
      ME_DEVICE_NUMBER,
      HECI_FUNCTION_NUMBER
      );
    MeFwSts2.ul = MmioRead32(HeciBaseAddress + R_ME_GS_SHDW);
    DEBUG((DEBUG_INFO, "ME FW STS 2 = %x\n", MeFwSts2.r));

    MeFwSts4 = MmioRead32(HeciBaseAddress + R_ME_HFS_4);
    DEBUG((DEBUG_INFO, "ME FW STS 4 = %x\n", MeFwSts4));

    MeFwSts5 = MmioRead32(HeciBaseAddress + R_ME_HFS_5);
    DEBUG((DEBUG_INFO, "ME FW STS 5 = %x\n", MeFwSts5));

    MeFwSts6 = MmioRead32(HeciBaseAddress + R_ME_HFS_6);
    DEBUG((DEBUG_INFO, "ME FW STS 6 = %x\n", MeFwSts6));

    ///
    /// Check Bit 12 in ME FWSTS4 to check if TPM_DISCONNECT_ALL bit is set
    /// or ENF Shutdown path is taken by ME FW.
    /// Also Identify any failures in ACM
    ///
    if ((MeFwSts4 & (B_TPM_DISCONNECT | B_BOOT_GUARD_ENF_MASK)) || (BootGuardAcmStatus & B_BOOT_GUARD_ACM_ERRORCODE_MASK)) {
      DEBUG((DEBUG_INFO, "All TPM's on Platform are Disconnected\n"));
      BootGuardInfo->DisconnectAllTpms = TRUE;
    }
    BootGuardInfo->BypassTpmInit = FALSE;
    BootGuardInfo->MeasuredBoot = FALSE;

    if (MeFwSts4 & BIT10) {
      DEBUG((DEBUG_INFO, "Sx Resume Type Identified - TPM Event Log not required for ACM Measurements\n"));
      BootGuardInfo->ByPassTpmEventLog = TRUE;
    }
    ///
    /// Check bit 0 of BOOT_GUARD_SACM_INFO MSR if system is in Boot Guard boot mode
    ///
    MsrValue = (UINT32)AsmReadMsr64(MSR_BOOT_GUARD_SACM_INFO);
    DEBUG((DEBUG_INFO, "MSR_BOOT_GUARD_SACM_INFO MSR = %x\n", MsrValue));
    if ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
      DEBUG((DEBUG_INFO, "NEM is not initiated by Boot Guard ACM\n"));
    }
    if (MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) {
      BootGuardInfo->MeasuredBoot = TRUE;
      ///
      /// if measured bit is set, BIOS needs to bypass startup command
      ///
      if (MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) {
        BootGuardInfo->BypassTpmInit = TRUE;
      }
      ///
      /// Read present TPM type
      ///
      BootGuardInfo->TpmType = (TPM_TYPE)((MsrValue & V_TPM_PRESENT_MASK) >> 1);
      DEBUG((DEBUG_INFO, "TPM Type is %x\n", BootGuardInfo->TpmType));
    }
  }
  else {
    BootGuardInfo->BootGuardCapability = FALSE;
  }

  DEBUG((DEBUG_INFO, "Boot Guard Support status: %x\n", BootGuardInfo->BootGuardCapability));

  return;

}
