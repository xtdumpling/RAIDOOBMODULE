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
/** @file BiosGuardNvs.h
    BiosGuardNvs for Tool Interface

**/

#ifndef _AMI_BIOS_GUARD_NVS_H_
#define _AMI_BIOS_GUARD_NVS_H_

//
// Forward reference for pure ANSI compatability
//
typedef struct _AMI_BIOS_GUARD_NVS_AREA_PROTOCOL AMI_BIOS_GUARD_NVS_AREA_PROTOCOL;

#define AMI_BIOS_GUARD_NVS_AREA_PROTOCOL_GUID {0x7b7913f, 0x41e, 0x4eec, 0x93, 0xef, 0xbd, 0xda, 0xd, 0xd2, 0xd1, 0xec}

//
// Extern the GUID for protocol users.
//
//extern EFI_GUID gAmiBiosGuardNvsAreaProtocolGuid;

//
// Bios Guard NVS Area Definition
//
#pragma pack (1)
typedef struct {
    //
    // BIOS Guard for Tool Interface
    //
    UINT64     AmiBiosGuardMemAddress;                         // BiosGuard Memory Address for Tool Interface
    UINT8      AmiBiosGuardMemSize;                            // BiosGuard Memory Size for Tool Interface
    UINT16     AmiBiosGuardIoTrapAddress;                      // IoTrap Address for Tool Interface
    UINT16     AmiBiosGuardIoTrapLength;                       // IoTrap Length for Tool Interface
    UINT32     AmiBiosGuardRecoveryAddress;                    // BiosGuard Recovery Address   
    UINT16     AmiBiosGuardRecoverySize;                       // BiosGuard Recovery Size       
} AMI_BIOS_GUARD_NVS_AREA;

typedef struct {
    //
    // AmiBiosGuardRecoveryAddress for Tool Interface
    //
    UINT64 Signature;               // _AMIBSGD
    UINT16 Version;                 // 0
    UINT16 ReoveryMode;             // 1: Recovery, 2: Capsule
    UINT32 ReoveryAddress;          // AP pointer;
} AMI_BIOS_GUARD_RECOVERY;

//
// Ami Bios Guard NVS Area Protocol
//
typedef struct _AMI_BIOS_GUARD_NVS_AREA_PROTOCOL {
    AMI_BIOS_GUARD_NVS_AREA     *Area;
} AMI_BIOS_GUARD_NVS_AREA_PROTOCOL;

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
