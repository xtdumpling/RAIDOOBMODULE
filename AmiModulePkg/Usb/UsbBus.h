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

/** @file UsbBus.h
    AMI USB bus driver header file

**/

#ifndef _USBBUS_INC_
#define _USBBUS_INC_

#include "Tree.h"
#include "Rt/UsbDef.h"
#include <Protocol/UsbHc.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DevicePath.h>
#if USB_IAD_PROTOCOL_SUPPORT
#include <Protocol/AmiUsbIad.h>
#endif


#define USB_MAXLANID          16
#define USB_MAXCHILDREN       8
#define USB_MAXCONTROLLERS    4

#define USB_US_LAND_ID   0x0409

#pragma pack(push, 1)

typedef struct {
  UINT8  Length;
  UINT8  DescriptorType;
} USB_DESCRIPTOR_T;

typedef struct {
  UINT8  len;
  UINT8  desctype;
  UINT16 langID[1];
} lang_table_t;

#pragma pack(pop)

enum node_type_enum { NodeHC, NodeDevice, NodeGroup };


typedef struct {
    int                                 type;
    EFI_HANDLE                          handle; // handle of the controller
    EFI_DEVICE_PATH_PROTOCOL            *dp;
    EFI_USB2_HC_PROTOCOL                *hc;     // USB_HC_ installed on controller
} usbbus_data_t;

typedef struct _USBBUS_HC_T {
    int                                 type;
    EFI_HANDLE                          handle; // handle of the controller
    EFI_DEVICE_PATH_PROTOCOL            *dp;
    EFI_USB2_HC_PROTOCOL                *hc;     // USB_HC_ installed on controller
    HC_STRUC                            *hc_data;
    TREENODE_T                          node;
} USBBUS_HC_T;

#define COMPRESS_EP_ADR(a)              ( a & 0xF )

#if USB_IAD_PROTOCOL_SUPPORT

typedef struct _USBIAD_DATA_T {
    EFI_HANDLE  Handle;
    UINTN       DataSize;
} USBIAD_DATA_T;

typedef struct _IAD_DETAILS {
    UINT8       ConfigIndex;
    AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR    *Descriptor;
    EFI_HANDLE              IadHandle;
    AMI_USB_IAD_PROTOCOL    Iad;
    USBIAD_DATA_T           *Data;
} IAD_DETAILS;

#endif

typedef struct _DEVGROUP_T {
    int                                 type;
    EFI_HANDLE                          handle; // handle of the controller
    EFI_DEVICE_PATH_PROTOCOL            *dp;
    EFI_USB2_HC_PROTOCOL                *hc;    //  USB_HC_ that the controller is attached to
    DEV_INFO                            *dev_info;
    HC_STRUC                            *hc_info;
    lang_table_t                        *lang_table;
    EFI_USB_STRING_DESCRIPTOR           *ManufacturerStrDesc;
    EFI_USB_STRING_DESCRIPTOR           *ProductStrDesc;
    EFI_USB_STRING_DESCRIPTOR           *SerialNumberStrDesc;
    EFI_USB_DEVICE_DESCRIPTOR           dev_desc;
    EFI_USB_CONFIG_DESCRIPTOR           **configs;
    EFI_USB_ENDPOINT_DESCRIPTOR*        endpoints[0x20];
    EFI_USB_ENDPOINT_DESCRIPTOR*        a2endpoint[0x20];
    int                                 endpoint_count;

    int                                 active_config; // index in configs
    int                                 config_count;
    int                                 f_DevDesc;
    TREENODE_T                          node;
#if USB_IAD_PROTOCOL_SUPPORT
    UINT8                               iad_count;
    IAD_DETAILS                         *iad_details;
#endif
} DEVGROUP_T;

#define USB_MAX_ALT_IF 16

typedef struct _USBDEV_T {
    int                                 type;
    EFI_HANDLE                          handle; // handle of the controller
    EFI_DEVICE_PATH_PROTOCOL            *dp;
    EFI_USB2_HC_PROTOCOL                *hc; //USB_HC_ that the controller is attached to
    DEV_INFO                            *dev_info;
    HC_STRUC                            *hc_info;
    //UINT8                             toggle; //toggle param for bulk transfer
    CHAR16*                             name;
    int                                 f_connected; //was ConnectControllers successful?
    int                                 first_endpoint[USB_MAX_ALT_IF];
    int                                 end_endpoint[USB_MAX_ALT_IF];
    EFI_USB_INTERFACE_DESCRIPTOR*       descIF[USB_MAX_ALT_IF];
    UINT32                              LoadedAltIfMap;
    UINT8                               speed;
    EFI_USB_IO_PROTOCOL                 io;
    TREENODE_T                          node;
	int									async_endpoint;
} USBDEV_T;

EFI_STATUS 
EFIAPI
UsbBusSupported (
  EFI_DRIVER_BINDING_PROTOCOL     *pThis,
  EFI_HANDLE                      controller,
  EFI_DEVICE_PATH_PROTOCOL        * );

EFI_STATUS
EFIAPI
UsbBusStart (
  EFI_DRIVER_BINDING_PROTOCOL     *pThis,
  EFI_HANDLE                      controller,
  EFI_DEVICE_PATH_PROTOCOL        * );

EFI_STATUS
EFIAPI
UsbBusStop (
  EFI_DRIVER_BINDING_PROTOCOL     *pThis,
  EFI_HANDLE                      controller,
  UINTN                           NumberOfChildren,
  EFI_HANDLE                      *ChildHandleBuffer );

EFI_STATUS UsbBusInit(EFI_HANDLE  ImageHandle,EFI_HANDLE  ServiceHandle);

USBDEV_T* UsbIo2Dev(EFI_USB_IO_PROTOCOL* p);
DEVGROUP_T* UsbDevGetGroup(USBDEV_T* Dev);
DEV_INFO* FindDevStruc(EFI_HANDLE Controller);

UINT8*
UsbSmiGetDescriptor(
    HC_STRUC* Hc,
    DEV_INFO* Dev,
    UINT8*    Buf,
    UINT16    Len,
    UINT8     DescType,
    UINT8     DescIndex
);

UINT16
UsbSmiControlTransfer (
    HC_STRUC*   HCStruc,
    DEV_INFO*   DevInfo,
    UINT16      Request,
    UINT16      Index,
    UINT16      Value,
    UINT8       *Buffer,
    UINT16      Length
);

UINT32
UsbSmiIsocTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length,
    UINT8       *Async
);

UINT8
UsbResetAndReconfigDev(
    HC_STRUC*   HostController,
    DEV_INFO*   Device
);

UINT8
UsbDevDriverDisconnect(
    HC_STRUC*   HostController,
    DEV_INFO*   Device
);

UINT8
UsbSmiActivatePolling (
    HC_STRUC* HostController,
    DEV_INFO* DevInfo 
);

UINT8
UsbSmiDeactivatePolling (
    HC_STRUC* HostController,
    DEV_INFO* DevInfo 
);

#define GETBIT(bitarray,value,bit) \
    ((value) =  (UINT8)(((bitarray) & (1 << (bit)))>>(bit)))\

#define SETBIT(bitarray,value,bit) \
    (bitarray) =  (((bitarray) & ~(1 << (bit))) | (((value)&1) << (bit)) )\

#define IsSlow(dev) dev->speed
#define GetSpeed(dev) dev->speed

VOID InstallDevice(DEV_INFO* DevInfo);
int eUninstallDevice(VOID* Node, VOID* Context);
EFI_STATUS RemoveDevInfo(DEV_INFO* pDevInfo);


#endif //_USBBUS_INC_

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
