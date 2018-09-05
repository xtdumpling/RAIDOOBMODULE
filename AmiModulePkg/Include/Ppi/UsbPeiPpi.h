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

/** @file UsbPeiPpi.h
    This is a header file with the USB PEI PPI definitions.

**/

#ifndef _USBPEI_PPI_H
#define _USBPEI_PPI_H

#include <Ppi/Usb2HostController.h>
#include <Ppi/UsbController.h>

#define AMI_PEI_USB_HOST_CONTROLLER_PPI_GUID \
    {0xbfa8e328, 0xd653, 0x4653, {0xa3, 0x28, 0x4d, 0x7e, 0x94, 0x9, 0x6a, 0xe1}}

#define AMI_PEI_USB_IO_PPI_GUID \
    {0x1998f2cb, 0x9827, 0x4e3d, {0x8d, 0x55, 0x19, 0xbb, 0x7f, 0xc9, 0x8a, 0xdb}}

typedef struct _PEI_USB_HOST_CONTROLLER_PPI PEI_USB_HOST_CONTROLLER_PPI;
typedef struct _PEI_USB_IO_PPI PEI_USB_IO_PPI;

#define USB_PORT_STAT_SUPER_SPEED_PLUS  0x1000

#pragma pack(push, 1)

typedef struct {
    UINT8  Length;
    UINT8  DescriptorType;
    UINT8  NbrPorts;
    UINT16 HubCharacteristics;
    UINT8  PwrOn2PwrGood;
    UINT8  HubContrCurrent;
	UINT8  HubHdrDecLat;
	UINT16 HubDelay;
    UINT16 DeviceRemovable;
} EFI_USB_HUB_DESCRIPTOR;

#pragma pack(pop)

typedef enum {
    EfiUsbDevice,
    EfiUsbInterface,
    EfiUsbEndpoint
} EFI_USB_RECIPIENT;

typedef enum {
    EfiUsbEndpointHalt,
    EfiUsbDeviceRemoteWakeup
} EFI_USB_STANDARD_FEATURE_SELECTOR;


typedef EFI_STATUS (EFIAPI *PEI_ASYNC_USB_TRANSFER_CALLBACK) (
  IN void         *Data,
  IN UINTN        DataLength,
  IN void         *Context,
  IN UINT32       Status);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_CONTROL_TRANSFER)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT8                       MaximumPacketLength,
    IN UINT16                      TransactionTranslator,
    IN EFI_USB_DEVICE_REQUEST      *Request,
    IN EFI_USB_DATA_DIRECTION      TransferDirection,
    IN OUT VOID *Data              OPTIONAL,
    IN OUT UINTN *DataLength       OPTIONAL,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_BULK_TRANSFER)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       EndPointAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT16                      MaximumPacketLength,
    IN UINT16                      TransactionTranslator,
    IN OUT VOID                    *Data,
    IN OUT UINTN                   *DataLength,
    IN OUT UINT8                   *DataToggle,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
);

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_ASYNC_INTERRUPT_TRANSFER) (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN     UINT8                    DeviceAddress,
    IN     UINT8                    EndPointAddress,
    IN     UINT8                    DeviceSpeed,
    IN     UINT8                    MaxiumPacketLength,
    IN UINT16                       TransactionTranslator,
    IN     BOOLEAN                  IsNewTransfer,
    IN OUT UINT8                    *DataToggle      OPTIONAL,
    IN     UINTN                    PollingInterval  OPTIONAL,
    IN     UINTN                    DataLength       OPTIONAL,
    IN     PEI_ASYNC_USB_TRANSFER_CALLBACK CallBackFunction OPTIONAL,
    IN     VOID                     *Context         OPTIONAL  
  );
  
typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_SYNC_INTERRUPT_TRANSFER) (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI  *This,
    IN     UINT8                    DeviceAddress,
    IN     UINT8                    EndPointAddress,
    IN     UINT8                    DeviceSpeed,
    IN     UINT8                    MaximumPacketLength,
    IN UINT16                       TransactionTranslator,
    IN OUT VOID                     *Data,
    IN OUT UINTN                    *DataLength,
    IN OUT UINT8                    *DataToggle,
    IN     UINTN                    TimeOut,
    OUT    UINT32                   *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_NUMBER)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_STATUS)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_SET_ROOTHUB_PORT_FEATURE)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_CLEAR_ROOTHUB_PORT_FEATURE)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_RESET)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This, 
    IN UINT16                       Attributes
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_PRECONFIGURE_DEVICE) (
    IN EFI_PEI_SERVICES            **PeiServices,
    IN VOID                        *PeiUsbDev,
    IN UINT8                       PortNumber
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_ENABLE_ENDPOINTS)(
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI      *This,
    IN UINT8                            DeviceAddress,
    IN UINT8                            *ConfigurationData
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_HOST_CONTROLLER_ACTIVATE_POLLING) (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN VOID                         *PeiUsbDev
);


typedef struct _PEI_USB_HOST_CONTROLLER_PPI
{
    PEI_USB_HOST_CONTROLLER_CONTROL_TRANSFER            ControlTransfer;
    PEI_USB_HOST_CONTROLLER_BULK_TRANSFER               BulkTransfer;
    PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_NUMBER     GetRootHubPortNumber;
    PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_STATUS     GetRootHubPortStatus;
    PEI_USB_HOST_CONTROLLER_SET_ROOTHUB_PORT_FEATURE    SetRootHubPortFeature;
    PEI_USB_HOST_CONTROLLER_CLEAR_ROOTHUB_PORT_FEATURE  ClearRootHubPortFeature;
    PEI_USB_HOST_CONTROLLER_ASYNC_INTERRUPT_TRANSFER    AsyncInterruptTransfer;
    PEI_USB_HOST_CONTROLLER_SYNC_INTERRUPT_TRANSFER     SyncInterruptTransfer;
    PEI_USB_HOST_CONTROLLER_RESET            			Reset;     
    BOOLEAN         DebugPortUsed;
    PEI_USB_HOST_CONTROLLER_PRECONFIGURE_DEVICE         PreConfigureDevice;
    PEI_USB_HOST_CONTROLLER_ENABLE_ENDPOINTS            EnableEndpoints;
    PEI_USB_HOST_CONTROLLER_ACTIVATE_POLLING            ActivatePolling;
    UINT8           CurrentAddress;
} PEI_USB_HOST_CONTROLLER_PPI;

typedef
EFI_STATUS
(EFIAPI * PEI_USB_CONTROL_TRANSFER)(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_IO_PPI               *This,
    IN EFI_USB_DEVICE_REQUEST       *Request,
    IN EFI_USB_DATA_DIRECTION       Direction,
    IN UINT32                       Timeout,
    IN OUT VOID                     *Data       OPTIONAL,
    IN UINTN                        DataLength  OPTIONAL,
    OUT UINT32                      *UsbStatus
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_BULK_TRANSFER)(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *This,
    IN UINT8            DeviceEndpoint,
    IN OUT VOID         *Data,
    IN OUT UINTN        *DataLength,
    IN UINTN            Timeout,
    OUT UINT32          *UsbStatus
);

typedef EFI_STATUS
(EFIAPI *PEI_USB_SYNC_INTERRUPT_TRANSFER) (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_IO_PPI               *This,
    IN     UINT8                    DeviceEndpoint,
    IN OUT void                     *Data,
    IN OUT UINTN                    *DataLength,
    IN     UINTN                    Timeout
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_GET_INTERFACE_DESCRIPTOR)(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_IO_PPI               *This,
    IN EFI_USB_INTERFACE_DESCRIPTOR **InterfaceDescriptor
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_GET_ENDPOINT_DESCRIPTOR)(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_IO_PPI              *This,
    IN UINT8                       EndpointIndex,
    IN EFI_USB_ENDPOINT_DESCRIPTOR **EndpointDescriptor
);

typedef
EFI_STATUS
(EFIAPI * PEI_USB_PORT_RESET)(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *This
);

typedef struct _PEI_USB_IO_PPI
{
    PEI_USB_CONTROL_TRANSFER                UsbControlTransfer;
    PEI_USB_BULK_TRANSFER                   UsbBulkTransfer;
    PEI_USB_GET_INTERFACE_DESCRIPTOR        UsbGetInterfaceDescriptor;
    PEI_USB_GET_ENDPOINT_DESCRIPTOR         UsbGetEndpointDescriptor;
    PEI_USB_PORT_RESET                      UsbPortReset;
    PEI_USB_SYNC_INTERRUPT_TRANSFER         UsbSyncInterruptTransfer;
} PEI_USB_IO_PPI;

extern EFI_GUID gPeiUsbIoPpiGuid;
extern EFI_GUID gAmiPeiUsbIoPpiGuid;
extern EFI_GUID gAmiPeiUsbHostControllerPpiGuid;

#define PEI_MAX_USB_IO_PPI  127

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
