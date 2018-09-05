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
/** @file BiosGuardDxe.c

**/
//*************************************************************************
#include <Base.h>
#include <Uefi.h>

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Cpu/CpuRegs.h>
#include <token.h>

#pragma pack (1)

typedef struct {
    //
    // AmiBiosGuardRecoveryAddress for Tool Interface
    //
    UINT64 Signature;               // _AMIBSGD
    UINT16 Version;                 // 0
    UINT16 ReoveryMode;             // 1: Recovery, 2: Capsule
    UINT32 ReoveryAddress;          // AP pointer;
} AMI_BIOS_GUARD_RECOVERY;

#pragma pack ()

EFI_STATUS
BiosGuardClearCapsuleVariable (
  VOID
)
{
    EFI_STATUS                   Status;
    AMI_BIOS_GUARD_RECOVERY      *ApRecoveryAddress;
    UINTN                        BiosGuardCapsuleVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    EFI_GUID                     BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;

    DEBUG ((DEBUG_INFO, "BiosGuardClearCapsuleVariable()\n"));
    
    ApRecoveryAddress = (AMI_BIOS_GUARD_RECOVERY*)AllocateZeroPool (sizeof(AMI_BIOS_GUARD_RECOVERY));
    Status = gRT->SetVariable(L"BiosGuardCapsuleVariable",
                              &BiosGuardCapsuleVariableGuid,
                              (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                              BiosGuardCapsuleVarSize,
                              ApRecoveryAddress);
    ASSERT_EFI_ERROR (Status);
    FreePool (ApRecoveryAddress);

    return Status;
}

EFI_STATUS
EFIAPI
BiosGuardDxeEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;

    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        DEBUG ((DEBUG_INFO, "BIOS Guard is disabled or not supported \n"));
        return EFI_SUCCESS;
    }

    Status = BiosGuardClearCapsuleVariable();
    
    return Status;
}

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
