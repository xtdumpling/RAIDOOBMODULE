//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file BootGuardPei.h
    Header file for BootGuardPei

**/
//*************************************************************************

#ifndef _BOOT_GUARD_PEI_H_
#define _BOOT_GUARD_PEI_H_

#include <Ppi/EndOfPeiPhase.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <BaseCryptLib.h>

#include <Token.h>

#define RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID \
    {0xcbc91f44, 0xa4bc, 0x4a5b, 0x86, 0x96, 0x70, 0x34, 0x51, 0xd0, 0xb0, 0x53}

//
// BtG related definitions
//
#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0
#define V_TPM_PRESENT_MASK                                            0x06
#define V_TPM_PRESENT_NO_TPM                                          0
#define V_TPM_PRESENT_DTPM_12                                         1
#define V_TPM_PRESENT_DTPM_20                                         2
#define V_TPM_PRESENT_PTT                                             3
#define B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS                            BIT3
#define B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT                          BIT5
#define B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT                          BIT6
#define B_BOOT_GUARD_SACM_INFO_CAPABILITY                             BIT32
#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT == 1)
static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid, NULL
};
#endif

#if defined(BUILD_WITH_GLUELIB)
#undef SetMem
VOID *
SetMem (
    OUT VOID *Buffer,
    IN UINTN Length,
    IN UINT8 Value
)
{
    return GlueSetMem (Buffer, Length, Value);
}

#undef CopyMem
VOID *
EFIAPI
CopyMem (
    OUT VOID       *DestinationBuffer,
    IN CONST VOID  *SourceBuffer,
    IN UINTN       Length
)
{
    return GlueCopyMem (DestinationBuffer, SourceBuffer, Length);
}
#endif

#ifdef ECP_FLAG
//
// Hash functions definitionS
//
UINTN
EFIAPI
Sha256GetContextSize (
    VOID
);

BOOLEAN
EFIAPI
Sha256Init (
    IN OUT  VOID  *Sha256Context
);

BOOLEAN
EFIAPI
Sha256Update (
    IN OUT  VOID        *Sha256Context,
    IN      CONST VOID  *Data,
    IN      UINTN       DataLength
);

BOOLEAN
EFIAPI
Sha256Final (
    IN OUT  VOID   *Sha256Context,
    OUT     UINT8  *HashValue
);
#endif

STATIC
EFI_STATUS
BootGuardVerificationForPeiToDxeHandoffEndOfPei (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
);

//
// GUID to AMI_BOOT_GUARD Module
//
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define AMI_BOOT_GUARD_HOB_GUID \
  { \
    0xb60ab175, 0x498d, 0x429d, 0xad, 0xba, 0xa, 0x62, 0x2c, 0x58, 0x16, 0xe2 \
  }
#else
#define AMI_BOOT_GUARD_HOB_GUID \
  { \
    0xb60ab175, 0x498d, 0x429d, \
    { \
      0xad, 0xba, 0xa, 0x62, 0x2c, 0x58, 0x16, 0xe2 \
    } \
  }
#endif

#pragma pack (1)

typedef struct {
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    UINT8             AmiBootGuardVerificationforPEItoDXEFlag;
} AMI_BOOT_GUARD_HOB;

typedef struct {
    UINT8             BootGuardFvMainHashKey0[32];
    UINT32            BootGuardFvMainSegmentBase0;
    UINT32            BootGuardFvMainSegmentSize0;
    UINT8             BootGuardFvMainHashKey1[32];
    UINT32            BootGuardFvMainSegmentBase1;
    UINT32            BootGuardFvMainSegmentSize1;
} RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY;

#pragma pack ()

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
