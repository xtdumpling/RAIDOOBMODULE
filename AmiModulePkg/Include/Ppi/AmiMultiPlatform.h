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
  @brief The file contains definition of the AMI Multi-Platform PPI.
**/
#ifndef __AMI_MULTI_PLATFORM_PPI__H__
#define __AMI_MULTI_PLATFORM_PPI__H__

#include <AmiPlatformInfo.h>

/// AMI Multi-Platform PPI GUID Definition
#define AMI_MULTI_PLATFORM_PPI_GUID  \
    {0xf04c91fd, 0x5aeb, 0x45bd, { 0xbe, 0x71, 0x65, 0x43, 0x59, 0x4, 0xc5, 0x68} }

/// PPI Forward declaration
typedef struct _AMI_MULTI_PLATFORM_PPI AMI_MULTI_PLATFORM_PPI;

/**
    This function returns Current Platform GUID
        
    @param This         Self Pointer
    @param PlatformGuid Double pointer to return current platform GUID

    @retval EFI_STATUS
        EFI_SUCCESS      When everything is going on fine!
        EFI_ERROR        Something Wrong!

**/
typedef EFI_STATUS (EFIAPI *AMI_MULTI_PLATFORM_PPI_GET_PLATFORM_GUID) (
    IN AMI_MULTI_PLATFORM_PPI   *This,
    OUT EFI_GUID                **PlatformGuid
  );

/**
    This function returns Current Platform Data Structure AMI_PLATFORM_INFO
        
    @param This         Self Pointer
    @param PlatformGuid Double pointer to AMI_PLATFORM_INFO of current platform

    @retval EFI_STATUS
        EFI_SUCCESS      When everything is going on fine!
        EFI_ERROR        Something Wrong!

**/
typedef EFI_STATUS (EFIAPI *AMI_MULTI_PLATFORM_PPI_GET_PLATFORM_INFO) (
	IN AMI_MULTI_PLATFORM_PPI   *This,
    OUT AMI_PLATFORM_INFO       **PlatformInfo
  );

/// AMI Multi-platform PPI 
struct _AMI_MULTI_PLATFORM_PPI {
    AMI_MULTI_PLATFORM_PPI_GET_PLATFORM_GUID  GetPlatformGuid;
    AMI_MULTI_PLATFORM_PPI_GET_PLATFORM_INFO  GetPlatformInfo;
};

/// GUID Variable Definition
extern EFI_GUID gAmiMultiPlatformPpiGuid; 

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
