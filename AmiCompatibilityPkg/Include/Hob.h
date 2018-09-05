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
  EFI Header file. It has standard definitions defined by the 
  EFI Specification Version 1.10 12/01/02
*/

#ifndef __HOB__H__
#define __HOB__H__

#include "Efi.h"

// Include files from EDKII
// MdePkg:
#include <Pi/PiHob.h>
#include <Guid/MemoryAllocationHob.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NextHob(Hob,Type) ((Type *) ((UINT8 *) Hob + ((EFI_HOB_GENERIC_HEADER *)Hob)->HobLength))

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
