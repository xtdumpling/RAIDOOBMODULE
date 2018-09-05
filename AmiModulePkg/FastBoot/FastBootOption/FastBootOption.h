//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file FastBootOption.h
 *  This file contains the Includes, Definitions, typedefs,
 *  Variable and External Declarations, Structure and
 *  function prototypes needed for the IdeSecurity driver.
 */

#ifndef _FastBootOption_
#define _FastBootOption_

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Protocol/DevicePath.h>

#if defined(TSE_CAPITAL_BOOT_OPTION) && (TSE_CAPITAL_BOOT_OPTION == 1)
#define gBootName L"Boot%04X"
#else
#define gBootName L"Boot%04x"
#endif

#define TempUefiHddDevice 0x80
#define TempUefiOddDevice 0x81
#define TempUefiRemDevice 0x82
#define TempUefiNetDevice 0x83

typedef struct {
    UINT16 DeviceTypeId;
    CHAR8 *TempName;
} TempDeviceMap;

/// Hooks
typedef BOOLEAN (IS_CORRECT_BOOT_OPTION_FUNC_PTR)(UINT16 DeviceTypeId);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
