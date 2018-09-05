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
/** @file BiosGuardCpuPolicyOverride.h
    Header file for BiosGuardCpuPolicyOverride sub-component

**/

#ifndef _BIOS_GUARD_SMM_H_
#define _BIOS_GUARD_SMM_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <token.h>
#include <Cpu/CpuRegs.h>
#include <PchAccess.h>

#include <FlashUpd.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_VARIABLE_PROTOCOL   *mSmmVariable;

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
