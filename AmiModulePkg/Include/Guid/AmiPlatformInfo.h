//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                  UNCOMPRESSEDFFSSECTION          **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  @brief The file contains definition of the AMI Platform Information data structure.

  AMI Platform Information data structure describes platform SKU. It is used by the multi-pltform aware modules.
**/
#ifndef __AMI_PLATFORM_INFO_HOB__H__
#define __AMI_PLATFORM_INFO_HOB__H__

#include <AmiPlatformInfo.h>

/// The GUID of the HOB containing information about active platform.
#define AMI_PLATFORM_INFO_HOB_GUID \
    { 0x874e9954, 0x24d4, 0x43e6, { 0x90, 0xa1, 0xc5, 0x77, 0x53, 0x40, 0xcf, 0x45 } }

/// The GUID of the FFS section containing uncompressed FV with platform-specific modules
#define AMI_PLATFORM_INFO_UNCOMPRESSED_FFS_SECTION_GUID \
	{ 0x0eecb4a5, 0x7b9f, 0x4707, { 0x81, 0xd7, 0x9c, 0x04, 0x5e, 0x98, 0xca, 0x95 } };

/// The GUID of the FFS section containing compressed FV with platform-specific modules
#define AMI_PLATFORM_INFO_COMPRESSED_FFS_SECTION_GUID \
	{ 0xee257f8f, 0xe710, 0x4181, { 0x9b, 0x01, 0xc0, 0x66, 0x98, 0x45, 0x4a, 0x16 } }

/// The GUID of the FFS file containing platform specific modules
#define AMI_PLATFORM_INFO_FFS_FILE_GUID \
	{ 0x0fc38c56, 0x500a, 0x4654, { 0x89, 0xa7, 0xf4, 0x3c, 0x1a, 0x38, 0x43, 0xf5 } }
	
/// AMI Platform Information HOB
typedef struct _AMI_PLATFORM_INFO_HOB {
    EFI_HOB_GUID_TYPE           Header;
    AMI_PLATFORM_INFO           PlatformInfo;
} AMI_PLATFORM_INFO_HOB;

/// see ::AMI_PLATFORM_INFO_HOB_GUID
extern EFI_GUID gAmiPlatformInfoHobGuid;
/// see ::AMI_PLATFORM_INFO_UNCOMPRESSED_FFS_SECTION_GUID
extern EFI_GUID gAmiPlatformInfoUncompressedFfsSectionGuid;
/// see ::AMI_PLATFORM_INFO_COMPRESSED_FFS_SECTION_GUID
extern EFI_GUID gAmiPlatformInfoCompressedFfsSectionGuid;
/// see ::AMI_PLATFORM_INFO_FFS_FILE_GUID
extern EFI_GUID gAmiPlatformInfoFfsFileGuid;

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
