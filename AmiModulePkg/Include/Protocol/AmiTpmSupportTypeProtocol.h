//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//**********************************************************************//
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatformIncludes/AmiTcgPlatformProtocol.h 1     12/12/11 3:03p Fredericko $
//
// $Revision: 1 $
//
// $Date: 12/12/11 3:03p $
//**********************************************************************//
//**********************************************************************//

#ifndef _AMI_TPM_SUPPORT_TYPE_PROTOCOL_H_
#define _AMI_TPM_SUPPORT_TYPE_PROTOCOL_H_

#include <Token.h>
#define AMI_TPM_SUPPORT_TYPE_PROTOCOL_GUID \
    {0x320bdc39, 0x3fa0, 0x4ba9, 0xbf, 0x2d, 0xb3, 0x3f, 0x72, 0xba, 0x9c, 0xa1}

typedef struct _AMI_TPM_SUPPORT_TYPE_PROTOCOL  AMI_TPM_SUPPORT_TYPE_PROTOCOL;

// for now all it does is return the platform security support type
typedef UINT8 (*GET_TCG_SUPPORT_TYPE)();

struct _AMI_TPM_SUPPORT_TYPE_PROTOCOL
{
    GET_TCG_SUPPORT_TYPE                GetPlatformSecurityType;
};

extern EFI_GUID gAmiTpmSupportTypeProtocolguid;

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
