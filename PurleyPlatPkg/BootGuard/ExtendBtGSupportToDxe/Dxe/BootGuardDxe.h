//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file BootGuardDxe.h
    Header file for BootGuardDxe

**/
//**********************************************************************

#ifndef _BootGuardDxe_H_
#define _BootGuardDxe_H_

#ifdef ECP_FLAG
#include "EdkIIGlueDxe.h"
#else
#include <Base.h>
#include <Uefi.h>

#include <PiDxe.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Guid/EventGroup.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#endif

#include <Token.h>

#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
    { 0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93 }

EFI_GUID gNotifyProtocolGuid = BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;

//
// GUID for the AMI_BOOT_GUARD Module
//
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define AMI_BOOT_GUARD_HOB_GUID \
  { \
    0xb60ab175, 0x498d, 0x429d, 0xad, 0xba, 0xa, 0x62, 0x2c, 0x58, 0x16, 0xe2 \
  }
#else
#define AMI_BOOT_GUARD_HOB_GUID \
  { \
    0xb60ab175, 0x498d, 0x429d, \
    { \
      0xad, 0xba, 0xa, 0x62, 0x2c, 0x58, 0x16, 0xe2 \
    } \
  }
#endif

//
// BtG related definitions
//
#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0
#define V_TPM_PRESENT_MASK                                            0x06
#define V_TPM_PRESENT_NO_TPM                                          0
#define V_TPM_PRESENT_DTPM_12                                         1
#define V_TPM_PRESENT_DTPM_20                                         2
#define V_TPM_PRESENT_PTT                                             3
#define B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS                            BIT3
#define B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT                          BIT5
#define B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT                          BIT6
#define B_BOOT_GUARD_SACM_INFO_CAPABILITY                             BIT32
#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

#pragma pack (1)

typedef struct {
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    UINT8             AmiBootGuardVerificationforPEItoDXEFlag;
} AMI_BOOT_GUARD_HOB;

#pragma pack ()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
