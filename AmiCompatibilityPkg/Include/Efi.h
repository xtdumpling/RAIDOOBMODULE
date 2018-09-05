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
    EFI Header file. It has standard definitions defined by the UEFI Specification
*/
#ifndef __EFI__H__
#define __EFI__H__
#ifdef __cplusplus
extern "C" {
#endif

// Include files from EDKII
// MdePkg:
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiMultiPhase.h>
#include <Pi/PiStatusCode.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/StatusCode.h>
#include <Guid/Acpi.h>
#include <Guid/EventGroup.h>
#include <Guid/GlobalVariable.h>
#include <Guid/Mps.h>
#include <Guid/SalSystemTable.h>
#include <Guid/SmBios.h>






//Error Codes and Warning Codes
#define EFI_ERROR_BIT 	((UINTN)1 << (sizeof(UINTN) * 8 - 1))
#define PI_ERROR_BIT    (EFI_ERROR_BIT | (EFI_ERROR_BIT >> 2))
#define OEM_ERROR_BIT   (EFI_ERROR_BIT | (EFI_ERROR_BIT >> 1))
#define PI_WARNING_BIT  (EFI_ERROR_BIT >> 2)
//PI Error Codes
// The PI specification defines error as EFI_REQUEST_UNLOAD_IMAGE.
// EFI_UNLOAD_IMAGE is here for backward compatibility.
#define EFI_UNLOAD_IMAGE EFI_REQUEST_UNLOAD_IMAGE
// AMI Error Codes
#define EFI_DBE_EOF		               (1 | OEM_ERROR_BIT)
#define EFI_DBE_BOF		               (2 | OEM_ERROR_BIT)
//Extended event types defined in the DXE CIS
#define EFI_EVENT_TIMER EVT_TIMER
#define EFI_EVENT_RUNTIME EVT_RUNTIME
#define EFI_EVENT_RUNTIME_CONTEXT EVT_RUNTIME_CONTEXT
#define EFI_EVENT_NOTIFY_WAIT EVT_NOTIFY_WAIT
#define EFI_EVENT_NOTIFY_SIGNAL EVT_NOTIFY_SIGNAL
#define EFI_EVENT_NOTIFY_SIGNAL_ALL 0x00000400
#define EFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES EVT_SIGNAL_EXIT_BOOT_SERVICES
#define EFI_EVENT_SIGNAL_VIRTUAL_ADDRESS_CHANGE EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE
#define ACPI_20_TABLE_GUID EFI_ACPI_20_TABLE_GUID


// Uefi/UefiSpec.h
#ifndef EFI_2_50_SYSTEM_TABLE_REVISION
#pragma pack(push,1)
typedef struct{
	UINT32 Attributes;
	UINT16 FilePathListLength;
//	CHAR16 Description[];
//	EFI_DEVICE_PATH_PROTOCOL FilePathList[];
//	UINT8 OptionalData[];
} EFI_LOAD_OPTION;
#pragma pack(pop)
#endif


// Uefi/UefiSpec.h
typedef EFI_BOOT_KEY_DATA HOT_KEY_EFI_KEY_DATA;

#ifndef GUID_VARIABLE_DEFINITION
#define GUID_VARIABLE_DECLARATION(Variable, Guid) extern EFI_GUID Variable
#else
#define GUID_VARIABLE_DECLARATION(Variable, Guid) GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID Variable=Guid
#endif
#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
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
