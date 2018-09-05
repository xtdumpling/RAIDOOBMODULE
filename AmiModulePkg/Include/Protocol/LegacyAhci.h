//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file legacyAhci.h
    AHCI INT13 initialization protocol

**/

#ifndef __LEGACY_AHCI_PROTOCOL_H__
#define __LEGACY_AHCI_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AMI_AHCI_INT13_INIT_PROTOCOL AMI_AHCI_INT13_INIT_PROTOCOL;

typedef 
EFI_STATUS 
(EFIAPI *AMI_INIT_AHCI_INT13_SUPPORT) ();

/**
    AHCI Int13 initialization protocol definition

**/

struct _AMI_AHCI_INT13_INIT_PROTOCOL {
  AMI_INIT_AHCI_INT13_SUPPORT InitAhciInt13Support;
};

extern EFI_GUID gAint13ProtocolGuid;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************