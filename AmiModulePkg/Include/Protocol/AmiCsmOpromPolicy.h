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
  @brief The file contains definition of the AmiCsmOpromPolicy protocol. 
  The protocol defines OpROM execution policy for a given PCI handle.
**/
#ifndef __AMI_CSM_OPROM_POLICY_PROTOCOL__H__
#define __AMI_CSM_OPROM_POLICY_PROTOCOL__H__
#include <Protocol/PciIo.h>

#define AMI_CSM_OPROM_POLICY_PROTOCOL_GUID \
    { 0xdc14e697, 0x775a, 0x4c3b, { 0xa1, 0x1a, 0xed, 0xc3, 0x8e, 0x1b, 0xe3, 0xe6 } }

typedef struct _AMI_CSM_PLATFORM_POLICY_DATA {
    EFI_PCI_IO_PROTOCOL *PciIo;
    VOID                **Rom;
    BOOLEAN             ExecuteThisRom;
    EFI_HANDLE          PciHandle;
} AMI_CSM_PLATFORM_POLICY_DATA;

typedef struct _AMI_CSM_OPROM_POLICY_PROTOCOL AMI_CSM_OPROM_POLICY_PROTOCOL;

typedef EFI_STATUS (EFIAPI *AMI_CSM_OPROM_POLICY_CHECK) (
    IN AMI_CSM_OPROM_POLICY_PROTOCOL *This, IN  EFI_HANDLE PciHandle
);

typedef VOID (EFIAPI *AMI_CSM_OPROM_POLICY_PROCESS)(
    IN OPTIONAL AMI_CSM_PLATFORM_POLICY_DATA *OpRomStartEndProtocol
);

struct _AMI_CSM_OPROM_POLICY_PROTOCOL {
    AMI_CSM_OPROM_POLICY_CHECK CheckOpromPolicy;
    AMI_CSM_OPROM_POLICY_PROCESS ProcessOpRom;
};

extern EFI_GUID gAmiCsmOpromPolicyProtocolGuid;
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
