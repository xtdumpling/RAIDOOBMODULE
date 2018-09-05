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
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatformIncludes/AmiTcgPlatformPpi.h 1     12/12/11 3:06p Fredericko $
//
// $Revision: 1 $
//
// $Date: 12/12/11 3:06p $
//**********************************************************************//
//**********************************************************************//
#ifndef _AMI_TCG_PLATFORM_PPI_H_
#define _AMI_TCG_PLATFORM_PPI_H_
#include <Uefi.h>


#ifdef __cplusplus
extern "C" {
#endif

#define AMI_TCG_PLATFORM_PPI_GUID \
    {0x5687f4a, 0x3ca7, 0x4d19, 0x9b, 0xc5, 0xe1, 0x80, 0xce, 0xa3, 0x56, 0x9f}

#define AMI_TCG_PLATFORM_PPI_BEFORE_MEM_GUID\
  {0xc1e6791d, 0xf35b, 0x43ef, 0x92, 0xa, 0xbe, 0x6, 0xba, 0x7f, 0x86, 0xa1}

#define AMI_TCG_PLATFORM_PPI_AFTER_MEM_GUID\
  {0x890c2cef, 0x43c8, 0x4209, 0xa7, 0x8d, 0xae, 0x14, 0xaa, 0x17, 0x98, 0xb4}

typedef struct _AMI_TCG_PLATFORM_PPI AMI_TCG_PLATFORM_PPI;

// for now all it does is return the platform security support type
typedef UINT8 (EFIAPI *GET_TCG_SUPPORT_TYPE)();

struct _AMI_TCG_PLATFORM_PPI
{
    GET_TCG_SUPPORT_TYPE                GetPlatformSecurityType;
};

typedef struct _AMI_TPM20SUPPORTTYPE_PPI AMI_TPM20SUPPORTTYPE_PPI;

// for now all it does is return the platform security support type
struct _AMI_TPM20SUPPORTTYPE_PPI
{
    GET_TCG_SUPPORT_TYPE                Tpm20SupportType;
};

#ifdef __cplusplus
}
#endif
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
