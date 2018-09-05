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
  CacheSubClass definitions.
*/
#ifndef __CACHE_SUBCLASS_H__
#define __CACHE_SUBCLASS_H__

#include <Efi.h>
#include <DataHubSubClass.h>

// Include files from EDKII
// IntelFrameworkPkg:
#include <Guid/DataHubRecords.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFI_CACHE_CLASS EFI_DATA_CLASS_DATA

//Size
#define EFI_CACHE_SIZE_RECORD_NUMBER			0x00000001

//Maximum Size
#define EFI_MAXIMUM_CACHE_SIZE_RECORD_NUMBER	0x00000002

//Speed
#define EFI_CACHE_SPEED_RECORD_NUMBER			0x00000003

//Socket
#define EFI_CACHE_SOCKET_RECORD_NUMBER			0x00000004

//SRAM Type Supported
#define EFI_CACHE_SRAM_SUPPORT_RECORD_NUMBER	0x00000005


//SRAM Type Installed
#define EFI_CACHE_SRAM_INSTALL_RECORD_NUMBER	0x00000006

//Error Correction Type Supported
#define EFI_CACHE_ERROR_SUPPORT_RECORD_NUMBER	0x00000007

//Type
#define EFI_CACHE_TYPE_RECORD_NUMBER			0x00000008

//Associativity
#define EFI_CACHE_ASSOCIATIVITY_RECORD_NUMBER	0x00000009

//Configuration
#define EFI_CACHE_CONFIGURATION_RECORD_NUMBER	0x0000000A;

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
