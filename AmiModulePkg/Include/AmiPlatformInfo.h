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
  @brief The file contains definition of the AMI Platform Information data structures.

  AMI Platform Information data structures provide information about platform SKU. 
  It is used by the multi-platform module and by other multi-platform aware modules
  to implement support of multiple platform flavors (multiple SKU) in a single ROM image.
**/
#ifndef __AMI_PLATFORM_INFO__H__
#define __AMI_PLATFORM_INFO__H__

#define AMI_PLATFORM_INFO_SIG       0x54414C50494D4124    //$AMIPLAT
#define AMI_MULTI_PLATFORM_INFO_SIG 0x4649504D494D4124    //$AMIMPIF

/// AMI Platform Information data structure describes platform SKU. It is used by the multi-platform aware modules.
typedef struct _AMI_PLATFORM_INFO {
    UINT64            Signature;           ///< AMI_PLATFORM_INFO_SIG
    UINT32            PlatformSkuId;       ///< Platform SKU ID. PCD database is initialized with this SKU ID.
    UINT32            Reserved;            ///< Reserved for future use
    EFI_GUID          PlatformGuid;        ///< Platform GUID 
    UINT8             PlatformName[0x10];  ///< Human readable platform name
} AMI_PLATFORM_INFO;

/// Provides information about all platform SKU supported by the firmware.
typedef struct _AMI_MULTI_PLATFORM_INFO {
    UINT64            Signature;           ///< AMI_MULTI_PLATFORM_INFO_SIG
    UINT32            NumberOfPlatforms;   ///< Number of platforms supported by this BIOS
    UINT32            Reserved;            ///< Reserved for future use
    AMI_PLATFORM_INFO PlatformInfo[1];     ///< Array of AMI_PLATFORM_INFO structures with NumberOfPlatforms elements
} AMI_MULTI_PLATFORM_INFO;

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
