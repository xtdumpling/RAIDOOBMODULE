//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
#ifndef __AMI_SIO_SMM_HANDOFF_PROTOCOL__H__
#define __AMI_SIO_SMM_HANDOFF_PROTOCOL__H__

#include <AmiGenericSio.h>


// {7576CC89-8FA3-4cad-BA02-6119B46ED44A}
/// AMI SMM SIO Handoff Protocol GUID Definition
#define AMI_SIO_SMM_HANDOFF_PROTOCOL_GUID  \
    { 0x7576cc89, 0x8fa3, 0x4cad, { 0xba, 0x02, 0x61, 0x19, 0xb4, 0x6e, 0xd4, 0x4a } }
   

typedef struct _AMI_SIO_SMM_HANDOFF_PROTOCOL {
    UINTN                       SpioCount;
    GSIO2                       *SpioData;
} AMI_SIO_SMM_HANDOFF_PROTOCOL;

extern EFI_GUID gAmiSioSmmHandoffProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
