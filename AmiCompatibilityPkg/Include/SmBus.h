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
  SM BUS data structure declarations
*/

#ifndef __SMBUS_MAIN__H__
#define __SMBUS_MAIN__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
// Include files from EDKII
// MdePkg:
#include <Library/SmbusLib.h>
#include <IndustryStandard/SmBus.h>

#pragma pack(1)

// PEC BIT is bit 22 in SMBUS address
#define SMBUS_LIB_PEC_BIT   (1 << 22)

#pragma pack()
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
