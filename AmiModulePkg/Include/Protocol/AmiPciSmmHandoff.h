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
  @brief The file contains definition of the AMI SMM SIO Handoff Protocol.
**/
#ifndef __AMI_PCI_SMM_HANDOFF_PROTOCOL__H__
#define __AMI_PCI_SMM_HANDOFF_PROTOCOL__H__

#include <PciHostBridge.h>


// {6FCE3BB9-9742-4cfd-8E9E-39F98DCA3271}
/// AMI PCI SMM Handoff Protocol GUID Definition
#define AMI_PCI_SMM_HANDOFF_PROTOCOL_GUID  \
    { 0x6fce3bb9, 0x9742, 0x4cfd, { 0x8e, 0x9e, 0x39, 0xf9, 0x8d, 0xca, 0x32, 0x71 } }


typedef struct _AMI_PCI_SMM_HANDOFF_PROTOCOL {
    UINTN                       HostCnt;
    PCI_HOST_BRG_DATA           *AmiHostData;
} AMI_PCI_SMM_HANDOFF_PROTOCOL;

extern EFI_GUID gAmiPciSmmHandoffProtocolGuid;

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
