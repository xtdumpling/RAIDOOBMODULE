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
  @brief The file contains definition of the AMI Multi-Platform Protocol.
**/
#ifndef __AMI_MULTI_PLATFORM_PROTOCOL__H__
#define __AMI_MULTI_PLATFORM_PROTOCOL__H__

#include <AmiPlatformInfo.h>

/// AMI Multi-Platform Protocol GUID Definition
#define AMI_MULTI_PLATFORM_PROTOCOL_GUID  \
    { 0x32d9116a, 0xaf15, 0x4976, { 0x90, 0x8c, 0x2c, 0x6e, 0x05, 0xcb, 0x78, 0x9a } }

/// AMI Multi-Platform SMM Driver Protocol GUID Definition
#define AMI_SMM_MULTI_PLATFORM_PROTOCOL_GUID  \
    {0x19d92c2d, 0xf089, 0x4b7c, { 0xab, 0xe6, 0x33, 0x3a, 0x12, 0x5, 0xed, 0x89} }

/// Protocol Interface Forward declaration
typedef struct _AMI_MULTI_PLATFORM_PROTOCOL AMI_MULTI_PLATFORM_PROTOCOL;

/**
    This function returns Current Platform GUID
        
    @param This         Self Pointer
    @param PlatformGuid Double pointer to return current platform GUID

    @retval EFI_STATUS
        EFI_SUCCESS      When everything is going on fine!
        EFI_ERROR        Something Wrong!

**/
typedef EFI_STATUS (EFIAPI *AMI_MULTI_PLATFORM_PROTOCOL_GET_PLATFORM_GUID) (
    IN AMI_MULTI_PLATFORM_PROTOCOL *This,     
    OUT EFI_GUID                   **PlatformGuid
  );

/**
    This function returns Current Platform GUID
        
    @param This         Self Pointer
    @param PlatformGuid Double pointer to AMI_PLATFORM_INFO of current platform

    @retval EFI_STATUS
        EFI_SUCCESS      When everything is going on fine!
        EFI_ERROR        Something Wrong!

**/
typedef EFI_STATUS (EFIAPI *AMI_MULTI_PLATFORM_PROTOCOL_GET_PLATFORM_INFO) (
    IN AMI_MULTI_PLATFORM_PROTOCOL *This,     
    OUT AMI_PLATFORM_INFO          **PlatformInfo
  );


struct _AMI_MULTI_PLATFORM_PROTOCOL {
    AMI_MULTI_PLATFORM_PROTOCOL_GET_PLATFORM_GUID  GetPlatformGuid;
    AMI_MULTI_PLATFORM_PROTOCOL_GET_PLATFORM_INFO  GetPlatformInfo;
};

extern EFI_GUID gAmiMultiPlatformProtocolGuid;
extern EFI_GUID gAmiSmmMultiPlatformProtocolGuid;
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
