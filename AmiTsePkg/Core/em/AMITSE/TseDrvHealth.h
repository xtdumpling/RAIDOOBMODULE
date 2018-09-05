//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/TseDrvHealth.h $
//
// $Author: Arunsb $
//
// $Revision: 4 $
//
// $Date: 5/29/12 3:19a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file TseDrvHealth.h
    Header file for driver health only contains TSE specific.

**/
#if EFI_SPECIFICATION_VERSION>=0x20014           
#include "Protocol/DriverHealth.h"
#include "Protocol/DevicePathToText.h"
#else

#ifndef __DRIVER_HEALTH_PROTOCOL__H__
#define __DRIVER_HEALTH_PROTOCOL__H__

#define EFI_DRIVER_HEALTH_PROTOCOL_GUID \
    { 0x2a534210, 0x9280, 0x41d8, 0xae, 0x79, 0xca, 0xda, 0x1, 0xa2, 0xb1, 0x27 }
typedef struct _EFI_DRIVER_HEALTH_PROTOCOL EFI_DRIVER_HEALTH_PROTOCOL;

typedef enum {
    EfiDriverHealthStatusHealthy,
    EfiDriverHealthStatusRepairRequired,
    EfiDriverHealthStatusConfigurationRequired,
    EfiDriverHealthStatusFailed,
    EfiDriverHealthStatusReconnectRequired,
    EfiDriverHealthStatusRebootRequired
} EFI_DRIVER_HEALTH_STATUS;

typedef UINT16 EFI_STRING_ID; 
typedef struct {
    EFI_HII_HANDLE HiiHandle;
    EFI_STRING_ID  StringId;
    UINT64         Reserved;
} EFI_DRIVER_HEALTH_HII_MESSAGE;

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_HEALTH_REPAIR_NOTIFY) (
    IN UINTN Value,
    IN UINTN Limit
);

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_HEALTH_GET_HEALTH_STATUS) (
    IN EFI_DRIVER_HEALTH_PROTOCOL     *This,
    IN EFI_HANDLE                     ControllerHandle, OPTIONAL
    IN EFI_HANDLE                     ChildHandle, OPTIONAL
    OUT EFI_DRIVER_HEALTH_STATUS      *HealthStatus,
    OUT EFI_DRIVER_HEALTH_HII_MESSAGE **MessageList, OPTIONAL
    OUT EFI_HII_HANDLE                *FormHiiHandle OPTIONAL
);

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_HEALTH_REPAIR) (
    IN EFI_DRIVER_HEALTH_PROTOCOL       *This,
    IN EFI_HANDLE                       ControllerHandle,
    IN EFI_HANDLE                       ChildHandle, OPTIONAL
    IN EFI_DRIVER_HEALTH_REPAIR_NOTIFY  RepairNotify OPTIONAL
);

struct _EFI_DRIVER_HEALTH_PROTOCOL {
    EFI_DRIVER_HEALTH_GET_HEALTH_STATUS GetHealthStatus;
    EFI_DRIVER_HEALTH_REPAIR            Repair;
};

#endif  /* __DRIVER_HEALTH_PROTOCOL__H__ */

#ifndef __DEVICE_PATH_TO_TEXT_PROTOCOL__H__
#define __DEVICE_PATH_TO_TEXT_PROTOCOL__H__


#define EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID \
    { 0x8b843e20, 0x8132, 0x4852, 0x90, 0xcc, 0x55, 0x1a, 0x4e, 0x4a, 0x7f, 0x1c }

GUID_VARIABLE_DECLARATION(gEfiDevicePathToTextProtocolGuid, EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID);


typedef struct _EFI_DEVICE_PATH_TO_TEXT_PROTOCOL EFI_DEVICE_PATH_TO_TEXT_PROTOCOL;

typedef
CHAR16*
(EFIAPI *EFI_DEVICE_PATH_TO_TEXT_NODE) (
    IN CONST EFI_DEVICE_PATH_PROTOCOL *DeviceNode,
    IN       BOOLEAN                  DisplayOnly,
    IN       BOOLEAN                  AllowShortcuts
);

typedef
CHAR16*
(EFIAPI *EFI_DEVICE_PATH_TO_TEXT_PATH) (
    IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN       BOOLEAN                  DisplayOnly,
    IN       BOOLEAN                  AllowShortcuts
);

struct _EFI_DEVICE_PATH_TO_TEXT_PROTOCOL {
    EFI_DEVICE_PATH_TO_TEXT_NODE ConvertDeviceNodeToText;
    EFI_DEVICE_PATH_TO_TEXT_PATH ConvertDevicePathToText;
};

#endif	/* __DEVICE_PATH_TO_TEXT_PROTOCOL__H__ */
#endif  /* EFI_SPECIFICATION_VERSION */

typedef struct DRV_HEALTH_HNDLS{
	EFI_HANDLE 	ControllerHandle;
	EFI_HANDLE 	ChildHandle;
	EFI_DRIVER_HEALTH_STATUS	HealthStatus;
	struct DRV_HEALTH_HNDLS 	*Next;
}DRV_HEALTH_HNDLS;

typedef struct {
	EFI_HII_HANDLE HiiHandle;
	EFI_STRING_ID StringId;
	UINT64 MessageCode;
} TSE_EFI_DRIVER_HEALTH_HII_MESSAGE;

CHAR16 *GetDrvHlthCtrlName (DRV_HEALTH_HNDLS *);
VOID AddHandleIntoList (EFI_HANDLE, EFI_HANDLE, EFI_DRIVER_HEALTH_STATUS);
VOID DisplayMsgListMessageBox (TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *MessageList);
BOOLEAN AdvancedRepairSupported (VOID);

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
