//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  @brief The file contains definition of the AMI Option ROM Policy Protocol.
**/
#ifndef __AMI_OPTION_ROM_POLICY_PROTOCOL__H__
#define __AMI_OPTION_ROM_POLICY_PROTOCOL__H__

/// AMI Option ROM Dispatch Policy Protocol GUID Definition
#define AMI_OPTION_ROM_POLICY_PROTOCOL_GUID\
    { 0x934ce8da, 0x5e2a, 0x4184, { 0x8a, 0x15, 0x8e, 0x8, 0x47, 0x98, 0x84, 0x31 } }
 
#define AMI_OPTION_ROM_POLICY_TYPE UINT8

#define AMI_OPTION_ROM_POLICY_DISABLE          0
#define AMI_OPTION_ROM_POLICY_UEFI             1
#define AMI_OPTION_ROM_POLICY_LEGACY           2
#define AMI_OPTION_ROM_POLICY_CUSTOM           3

typedef struct _AMI_OPTION_ROM_POLICY_PROTOCOL  AMI_OPTION_ROM_POLICY_PROTOCOL;

typedef
EFI_STATUS (EFIAPI *AMI_OPTION_ROM_POLICY_PROTOCOL_GET)(
    IN      AMI_OPTION_ROM_POLICY_PROTOCOL           *This,
    OUT     AMI_OPTION_ROM_POLICY_TYPE               *Policy,
    OUT     OPTIONAL BOOLEAN                         *RomStatus
  );

typedef
EFI_STATUS (EFIAPI *AMI_OPTION_ROM_POLICY_PROTOCOL_SET)(
    IN      AMI_OPTION_ROM_POLICY_PROTOCOL           *This,
    IN      AMI_OPTION_ROM_POLICY_TYPE               NewPolicy
  );

typedef struct _AMI_OPTION_ROM_POLICY_PROTOCOL {
    AMI_OPTION_ROM_POLICY_PROTOCOL_GET  GetOptionRomPolicy;
    AMI_OPTION_ROM_POLICY_PROTOCOL_SET  SetOptionRomPolicy;
} AMI_OPTION_ROM_POLICY_PROTOCOL;

extern EFI_GUID gAmiOptionRomPolicyProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
