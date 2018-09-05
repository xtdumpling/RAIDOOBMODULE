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
  DXE definitions
*/

#ifndef __DXE__H__
#define __DXE__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// Include files from EDKII
// MdePkg:
#include <Guid/Apriori.h>
#include <Guid/DxeServices.h>
#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/HobList.h>
#include <Pi/PiDxeCis.h>


// Additional event types defined by DXE CIS
#define EVT_NOTIFY_SIGNAL_ALL               0x00000400
#define EVT_SIGNAL_READY_TO_BOOT            0x00000203
#define EVT_SIGNAL_LEGACY_BOOT              0x00000204

#define EFI_EVENT_GROUP_DXE_DISPATCH EFI_EVENT_GROUP_DXE_DISPATCH_GUID

//Define default memory attributes for MMIO and System Memory 
#define GCD_COMMON_MMIO_CAPS (EFI_MEMORY_UC | EFI_MEMORY_WT | EFI_MEMORY_WB | EFI_MEMORY_WP | EFI_MEMORY_RUNTIME)

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
