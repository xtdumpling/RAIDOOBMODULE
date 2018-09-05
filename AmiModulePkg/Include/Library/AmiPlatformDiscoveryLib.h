//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file
  @brief The file contains definition of the AMI Platform Discovery Library class.

  The library class is used by the multi-pltform module to detect active platform SKU.
  The library instance is typically provided by the platform support module.
**/

#ifndef __AMI_PLATFORM_DISCOVERY_LIB__H__
#define __AMI_PLATFORM_DISCOVERY_LIB__H__

#include <AmiPlatformInfo.h>

/**
  Verifies if passed in platform information matches the active platform SKU

  @param PlatformData Platform information data structure

  @retval EFI_SUCCESS Passed in platform information matches the active platform SKU
  @retval EFI_INVALID_PARAMETER PlatformData is either NULL or contains invalid data
  @retval EFI_NOT_FOUND Passed in platform information does not match the active platform SKU
**/
EFI_STATUS AmiPlatformDiscoveryLibValidatePlatform(IN AMI_PLATFORM_INFO *PlatformData);

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
