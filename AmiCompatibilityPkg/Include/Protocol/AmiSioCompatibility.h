//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
  Defining SIO Compatibility Protocol for Aptio 4.x SIO Modules.
*/

#ifndef __AMI_SIO_COMPATIBILITY_PROTOCOL__H__
#define __AMI_SIO_COMPATIBILITY_PROTOCOL__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <GenericSio.h>


#define AMI_SIO_PORT_COMPATIBILITY_PROTOCOL_GUID \
        { 0x4250cec2, 0xdddb, 0x400b, 0x8c, 0x62, 0xcf, 0x98, 0x64, 0xf6, 0xd1, 0x54 }

extern EFI_GUID gAmiSioPortCompatibilityProtocolGuid;

typedef struct _AMI_SIO_PORT_COMPATIBILITY_PROTOCOL {
	UINTN				SioPortSioCount;
    SPIO_LIST_ITEM      **SioPortSioList;
} AMI_SIO_PORT_COMPATIBILITY_PROTOCOL;





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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
