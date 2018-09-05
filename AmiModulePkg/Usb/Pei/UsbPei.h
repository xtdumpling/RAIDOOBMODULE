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

/** @file UsbPei.h
    This is a header file with the definitions for USBPEI PEIM.

**/

#ifndef _USBPEI_H
#define _USBPEI_H

#include <Token.h>

#include <PiPei.h>
#include <Ppi/UsbPeiPpi.h>


//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16( A, B )        ( (A) | (B << 8) )
#define EFI_SIGNATURE_32( A, B, C, D )  ( EFI_SIGNATURE_16( A, B ) | \
                                         (EFI_SIGNATURE_16( C, D ) << 16) )
#define EFI_SIGNATURE_64( A, B, C, D, E, F, G, H ) \
    ( EFI_SIGNATURE_32( A, B, C, D ) | ( (UINT64) \
                                        ( EFI_SIGNATURE_32 \ (E, F, G,\
                                                              H) ) << 32 ) )
#define PEI_CR( Record, TYPE, Field, Signature ) \
    _CR( Record, TYPE, Field )
//
//  CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
//
#define _CR( Record, TYPE, Field )  ( (TYPE *) \
                                     ( (CHAR8 *) (Record) -\
                                      (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )

// BIT(a) defines a value with one bit set as specified by a.
#ifndef BIT
#define BIT( a )  (1 << (a))
#endif


// MASK(high,low) defines a bit mask value with bits set in the bit
// range specified from high down to low (zero-based)
#ifndef MASK
#define MAX_VAL  0xffffffff     // maximum value supported by CPU
//#define MAX_BIT  31             // highest bit position in MAX_VAL
#define MASK( high, low )  (\
        ( MAX_VAL >> ( MAX_BIT - ( (high) - (low) ) ) ) \
        << (low) \
)
#endif


// READ_BITS(src,high,low) extracts a bit field shifted down to bit
// position 0 from src, where high and low define the bit range
// (zero-based).
#ifndef READ_BITS
#define READ_BITS( src, high, low )  (\
        ( (src) & MASK( (high), (low) ) ) \
        >> (low) \
)
#endif

#ifndef WRITE_BITS
#define WRITE_BITS( src, val, high, low )  (\
        ( (src) & ~MASK( (high), (low) ) ) \
        | ( (val) << (low) ) \
)
#endif

// Device types
#define BIOS_DEV_TYPE_KEYBOARD          0x01
#define BIOS_DEV_TYPE_MOUSE             0x02
#define BIOS_DEV_TYPE_HUB               0x03
#define BIOS_DEV_TYPE_STORAGE           0x04

#define DEFAULT_PACKET_LENGTH           8       // Max size of packet data

//
// Standard USB request
//
#define USB_DEV_GET_STATUS                  0x00

#define USB_DEV_CLEAR_FEATURE               0x01

#define USB_DEV_SET_FEATURE                 0x03

#define USB_DEV_SET_ADDRESS                 0x05
#define USB_DEV_SET_ADDRESS_REQ_TYPE        0x00

#define USB_DEV_GET_DESCRIPTOR              0x06
#define USB_DEV_GET_DESCRIPTOR_REQ_TYPE     0x80

#define USB_DEV_SET_DESCRIPTOR              0x07
#define USB_DEV_SET_DESCRIPTOR_REQ_TYPE     0x00

#define USB_DEV_GET_CONFIGURATION           0x08
#define USB_DEV_GET_CONFIGURATION_REQ_TYPE  0x80

#define USB_DEV_SET_CONFIGURATION           0x09
#define USB_DEV_SET_CONFIGURATION_REQ_TYPE  0x00

//#define USB_DEV_GET_INTERFACE               0x0a
#define USB_DEV_GET_INTERFACE_REQ_TYPE      0x81

//#define USB_DEV_SET_INTERFACE               0x0b
#define USB_DEV_SET_INTERFACE_REQ_TYPE      0x01

//#define USB_DEV_SYNCH_FRAME                 0x0c
#define USB_DEV_SYNCH_FRAME_REQ_TYPE        0x82

//
// USB Descriptor types
//
#define USB_DT_DEVICE     0x01
#define USB_DT_CONFIG     0x02
#define USB_DT_STRING     0x03
#define USB_DT_INTERFACE  0x04
#define USB_DT_ENDPOINT   0x05
#define USB_DT_HUB        0x29
#define USB_DT_SS_HUB     0x2A
#define USB_DT_HID        0x21

#define SET_DESCRIPTOR_TYPE( x )  WRITE_BITS( 0x0, x, 15, 8 )


#define MAX_ENDPOINT          16
#define HUB_TIME_OUT          3000
#define STALL_1_MILLI_SECOND  1000      // stall 1 ms



#define USB_SLOW_SPEED_DEVICE           0x01
#define USB_FULL_SPEED_DEVICE           0x02
#define USB_HIGH_SPEED_DEVICE           0x03
#define USB_SUPER_SPEED_DEVICE          0x04
#define USB_SUPER_SPEED_PLUS_DEVICE     0x05

//----------------------------------------------------------------------------
//      Bit definitions for EndpointDescriptor.EndpointAddr
//----------------------------------------------------------------------------
#define EP_DESC_ADDR_EP_NUM     0x0F    //Bit 3-0: Endpoint number
#define EP_DESC_ADDR_DIR_BIT    0x80    //Bit 7: Direction of endpoint, 1/0 = In/Out
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  Bit definitions for EndpointDescriptor.EndpointFlags
//----------------------------------------------------------------------------
#define EP_DESC_FLAG_TYPE_BITS  0x03    //Bit 1-0: Indicate type of transfer on endpoint
#define EP_DESC_FLAG_TYPE_CONT  0x00    //Bit 1-0: 00 = Endpoint does control transfers
#define EP_DESC_FLAG_TYPE_ISOC  0x01    //Bit 1-0: 01 = Endpoint does isochronous transfers
#define EP_DESC_FLAG_TYPE_BULK  0x02    //Bit 1-0: 10 = Endpoint does bulk transfers
#define EP_DESC_FLAG_TYPE_INT   0x03    //Bit 1-0: 11 = Endpoint does interrupt transfers
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.BaseClass
//---------------------------------------------------------------------------
#define BASE_CLASS_HID           0x03
#define BASE_CLASS_MASS_STORAGE  0x08
#define BASE_CLASS_HUB           0x09
//----------------------------------------------------------------------------

#define SUB_CLASS_BOOT_DEVICE       0x01    // Boot device sub-class

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.Protocol
//---------------------------------------------------------------------------
#define PROTOCOL_KEYBOARD  0x01         // Keyboard device protocol
#define PROTOCOL_MOUSE     0x02         // Mouse device protocol?

// Mass storage related protocol equates
#define PROTOCOL_CBI         0x00       // Mass Storage Control/Bulk/Interrupt
                                        // with command completion interrupt
#define PROTOCOL_CBI_NO_INT  0x01       // MASS STORAGE Control/Bulk/Interrupt
                                        // with NO command completion interrupt
#define PROTOCOL_BOT         0x50       // Mass Storage Bulk-Only Transport
#define PROTOCOL_VENDOR      0xff       // Vendor specific mass protocol
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
//      Bit definitions for DeviceRequest.RequestType
//----------------------------------------------------------------------------
//               Bit 7:   Data direction
#define USB_REQ_TYPE_OUTPUT         0x00    //  0 = Host sending data to device
#define USB_REQ_TYPE_INPUT          0x80    //  1 = Device sending data to host

//               Bit 6-5: Type
#define USB_REQ_TYPE_STANDARD       0x00    //  00 = Standard USB request
#define USB_REQ_TYPE_CLASS          0x20    //  01 = Class specific
#define USB_REQ_TYPE_VENDOR         0x40    //  10 = Vendor specific

//               Bit 4-0: Recipient
#define USB_REQ_TYPE_DEVICE         0x00    //  00000 = Device
#define USB_REQ_TYPE_INTERFACE      0x01    //  00001 = Interface
#define USB_REQ_TYPE_ENDPOINT       0x02    //  00010 = Endpoint
#define USB_REQ_TYPE_OTHER          0x03    //  00011 = Other

//----------------------------------------------------------------------------
// Values for DeviceRequest.RequestType and DeviceRequest.RequestCode fields
// combined as a word value.
//---------------------------------------------------------------------------
#define USB_RQ_SET_ADDRESS          ((0x05 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_GET_DESCRIPTOR       ((0x06 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_GET_CONFIGURATION    ((0x08 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_SET_CONFIGURATION    ((0x09 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_SET_INTERFACE        ((0x0B << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE)
#define USB_RQ_SET_FEATURE          ((0x03 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)

#define USB_FSEL_DEV_REMOTE_WAKEUP  01

#define USB_RQ_GET_CLASS_DESCRIPTOR ((0x06 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE)
#define HID_RQ_SET_PROTOCOL         ((0x0B << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define HID_RQ_SET_REPORT           ((0x09 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define HID_RQ_SET_IDLE             ((0x0A << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)


// Hub related definitions
//
// Hub feature numbers
//
#define C_HUB_LOCAL_POWER   0
#define C_HUB_OVER_CURRENT  1


//
// Hub class code & sub class code
//
#define CLASS_CODE_HUB      0x09
#define SUB_CLASS_CODE_HUB  0

//
// Hub Status & Hub Change bit masks
//
#define HUB_STATUS_LOCAL_POWER  0x0001
#define HUB_STATUS_OVERCURRENT  0x0002

#define HUB_CHANGE_LOCAL_POWER  0x0001
#define HUB_CHANGE_OVERCURRENT  0x0002

//
// Hub Characteristics
//
#define HUB_CHAR_LPSM      0x0003
#define HUB_CHAR_COMPOUND  0x0004
#define HUB_CHAR_OCPM      0x0018

//
// Hub specific request
//
#define HUB_CLEAR_FEATURE                0x01
#define HUB_CLEAR_FEATURE_REQ_TYPE       0x20

#define HUB_CLEAR_FEATURE_PORT           0x01
#define HUB_CLEAR_FEATURE_PORT_REQ_TYPE  0x23

#define HUB_GET_BUS_STATE                0x02
#define HUB_GET_BUS_STATE_REQ_TYPE       0xa3

#define HUB_GET_DESCRIPTOR               0x06
#define HUB_GET_DESCRIPTOR_REQ_TYPE      0xa0

#define HUB_GET_HUB_STATUS               0x00
#define HUB_GET_HUB_STATUS_REQ_TYPE      0xa0

#define HUB_GET_PORT_STATUS              0x00
#define HUB_GET_PORT_STATUS_REQ_TYPE     0xa3

#define HUB_SET_DESCRIPTOR               0x07
#define HUB_SET_DESCRIPTOR_REQ_TYPE      0x20

#define HUB_SET_HUB_FEATURE              0x03
#define HUB_SET_HUB_FEATURE_REQ_TYPE     0x20

#define HUB_SET_PORT_FEATURE             0x03
#define HUB_SET_PORT_FEATURE_REQ_TYPE    0x23

#define HUB_SET_HUB_DEPTH                0x0C
#define HUB_SET_HUB_DEPTH_REQ_TYPE       0x20

// Hub port status and port change status bits
#define HUB_PORT_CONNECTION		0x0001
#define HUB_PORT_ENABLE			0x0002
#define HUB_PORT_SUSPEND		0x0004
#define HUB_PORT_OVER_CURRENT	0x0008
#define HUB_PORT_RESET			0x0010
#define HUB_PORT_POWER			0x0100
#define HUB_PORT_LOW_SPEED		0x0200
#define HUB_PORT_HIGH_SPEED		0x0400
#define HUB_PORT_TEST			0x0800
#define HUB_PORT_INDICATOR		0x1000

#define HUB_C_PORT_CONNECTION	0x0001
#define HUB_C_PORT_ENABLE		0x0002
#define HUB_C_PORT_SUSPEND		0x0004
#define HUB_C_PORT_OVER_CURRENT	0x0008
#define HUB_C_PORT_RESET		0x0010

// Super speed hub definition
#define SS_HUB_PORT_CONNECTION		0x0001
#define SS_HUB_PORT_ENABLE			0x0002
#define SS_HUB_PORT_OVER_CURRENT	0x0008
#define SS_HUB_PORT_RESET			0x0010
#define SS_HUB_PORT_LINK_STATE		0x01E0
#define SS_HUB_PORT_POWER			0x0200
#define SS_HUB_PORT_SPEED			0x1800

#define SS_HUB_C_PORT_CONNECTION	0x0001
#define SS_HUB_C_PORT_ENABLE		0x0002
#define SS_HUB_C_PORT_SUSPEND		0x0004
#define SS_HUB_C_PORT_OVER_CURRENT	0x0008
#define SS_HUB_C_PORT_RESET			0x0010
#define	SS_HUB_C_BH_PORT_RESET		0x0020
#define	SS_HUB_C_PORT_LINK_STATE	0x0040
#define	SS_HUB_C_PORT_CONFIG_ERROR	0x0080

//
//
// USB request type
//
#define USB_TYPE_STANDARD  (0x00 << 5)
#define USB_TYPE_CLASS     (0x01 << 5)
#define USB_TYPE_VENDOR    (0x02 << 5)
#define USB_TYPE_RESERVED  (0x03 << 5)

//
// USB request targer device
//
#define USB_RECIP_DEVICE     0x00
#define USB_RECIP_INTERFACE  0x01
#define USB_RECIP_ENDPOINT   0x02
#define USB_RECIP_OTHER      0x03
//
// Request target types.
//
#define USB_RT_DEVICE     0x00
#define USB_RT_INTERFACE  0x01
#define USB_RT_ENDPOINT   0x02
#define USB_RT_HUB        (USB_TYPE_CLASS | USB_RECIP_DEVICE)
#define USB_RT_PORT       (USB_TYPE_CLASS | USB_RECIP_OTHER)

#ifndef PEI_USB_SET_ADDRESS_DELAY_MS
#define PEI_USB_SET_ADDRESS_DELAY_MS    2
#endif

#ifndef PEI_USB_GET_DEVICE_DESCRIPTOR_DELAY_MS
#define PEI_USB_GET_DEVICE_DESCRIPTOR_DELAY_MS    10
#endif

#ifndef PEI_USB_GENERIC_DEVICE_REQUEST_TIMEOUT_MS
#define PEI_USB_GENERIC_DEVICE_REQUEST_TIMEOUT_MS   5000
#endif

#ifndef PEI_USB_RESET_PORT_DELAY_MS
#define PEI_USB_RESET_PORT_DELAY_MS         10
#endif

static CHAR8 *gUsbErrorStrings[] =
{
    "EFI_USB_ERR_NOTEXECUTE",
    "EFI_USB_ERR_STALL",
    "EFI_USB_ERR_BUFFER",
    "EFI_USB_ERR_BABBLE",
    "EFI_USB_ERR_NAK",
    "EFI_USB_ERR_CRC",
    "EFI_USB_ERR_TIMEOUT",
    "EFI_USB_ERR_BITSTUFF",
    "EFI_USB_ERR_SYSTEM"
};

# define PEI_MAX_USB_KEYS 16

#define PEI_USB_DEVICE_SIGNATURE  EFI_SIGNATURE_32( 'U', 's', 'b', 'D' )
typedef struct
{
    UINTN Signature;
    PEI_USB_IO_PPI UsbIoPpi;
    EFI_PEI_PPI_DESCRIPTOR AmiUsbIoPpiList;
    EFI_PEI_PPI_DESCRIPTOR UsbIoPpiList;
    UINT8 DeviceAddress;
    UINT16 MaxPacketSize0;
    UINT8 DeviceSpeed;
    UINT8 DataToggle;
    UINT8 DeviceType;
    UINT8 DownStreamPortNo;
    UINT8 Reserved[2];                       // Padding for IPF
    UINT8 SlotId;
    PEI_USB_HOST_CONTROLLER_PPI *UsbHcPpi;
    UINT8 *ConfigurationData;
    EFI_USB_CONFIG_DESCRIPTOR *ConfigDesc;
    EFI_USB_INTERFACE_DESCRIPTOR *InterfaceDesc;
    EFI_USB_ENDPOINT_DESCRIPTOR *EndpointDesc[MAX_ENDPOINT];
    UINT16 TransactionTranslator;
    UINT8  IntEndpoint;
    UINT16 IntMaxPkt;
	UINT8  HubDepth;
} PEI_USB_DEVICE;

#define PEI_USB_DEVICE_FROM_THIS( a ) \
    PEI_CR( a, PEI_USB_DEVICE, UsbIoPpi, PEI_USB_DEVICE_SIGNATURE )

// Standard PCI configuration register offsets and relevant values
//
#define PCI_REG_REVID           0x08    //PCI revision ID register

// USB Base Class Code,Sub-Class Code and Programming Interface.
//
#define PCI_CLASSC_BASE_CLASS_SERIAL        0x0c
#define PCI_CLASSC_SUBCLASS_SERIAL_USB      0x03
#define PCI_CLASSC_PI_UHCI                  0x00
#define PCI_CLASSC_PI_OHCI                  0x10
#define PCI_CLASSC_PI_EHCI                  0x20
#define PCI_CLASSC_PI_XHCI                  0x30

EFI_STATUS
EhciPeiUsbEntryPoint(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
UhciPeiUsbEntryPoint(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
UhcPeimEntry(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
OhciPeiUsbEntryPoint(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
XhciPeiUsbEntryPoint(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
PeimInitializeUsb(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

EFI_STATUS
EFIAPI
PeiUsbControlTransfer(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_USB_IO_PPI           *This,
    IN EFI_USB_DEVICE_REQUEST   *Request,
    IN EFI_USB_DATA_DIRECTION   Direction,
    IN UINT32                   Timeout,
    IN OUT VOID                 *Data       OPTIONAL,
    IN UINTN                    DataLength  OPTIONAL,
    OUT UINT32                  *UsbStatus
);

EFI_STATUS
EFIAPI
PeiUsbBulkTransfer(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *This,
    IN UINT8            DeviceEndpoint,
    IN OUT VOID         *Data,
    IN OUT UINTN        *DataLength,
    IN UINTN            Timeout,
    OUT UINT32          *UsbStatus
);

EFI_STATUS
EFIAPI
PeiUsbSyncInterruptTransfer(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *This,
    IN      UINT8       DeviceEndpoint,
    IN OUT void         *Data,
    IN OUT UINTN        *DataLength,
    IN     UINTN        Timeout
);

EFI_STATUS
EFIAPI
PeiUsbGetInterfaceDescriptor(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_IO_PPI               *This,
    IN EFI_USB_INTERFACE_DESCRIPTOR **InterfaceDescriptor
);

EFI_STATUS
EFIAPI
PeiUsbGetEndpointDescriptor(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_IO_PPI              *This,
    IN UINT8                       EndpointIndex,
    IN EFI_USB_ENDPOINT_DESCRIPTOR **EndpointDescriptor
);

EFI_STATUS
EFIAPI
PeiUsbPortReset(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *This
);

EFI_STATUS
PeiHubGetPortStatus(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            Port,
    OUT UINT32          *PortStatus );

EFI_STATUS
PeiHubSetPortFeature (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            Port,
    IN UINT8            Value );

EFI_STATUS
PeiHubSetHubFeature (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            Value );

EFI_STATUS
PeiHubGetHubStatus (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    OUT UINT32          *HubStatus );

EFI_STATUS
PeiHubClearPortFeature (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            Port,
    IN UINT8            Value );

EFI_STATUS
PeiHubClearHubFeature (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            Value );

EFI_STATUS
PeiGetHubDescriptor (
    IN EFI_PEI_SERVICES        **PeiServices,
    IN PEI_USB_IO_PPI          *UsbIoPpi,
    IN UINTN                   DescriptorSize,
    OUT EFI_USB_HUB_DESCRIPTOR *HubDescriptor );

EFI_STATUS
PeiDoHubConfig (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_DEVICE   *PeiUsbDevice );

VOID
PeiResetHubPort (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    UINT8               PortNum );

VOID
PeiCopyMem (
    OUT VOID *Destination,
    IN VOID  *Source,
    IN UINTN Length );

EFI_STATUS
PeiUsbGetDescriptor (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16           Value,
    IN UINT16           Index,
    IN UINT16           DescriptorLength,
    IN VOID             *Descriptor );

EFI_STATUS
PeiUsbSetDeviceAddress (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16           AddressValue );

EFI_STATUS
PeiUsbClearDeviceFeature (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN PEI_USB_IO_PPI    *UsbIoPpi,
    IN EFI_USB_RECIPIENT Recipient,
    IN UINT16            Value,
    IN UINT16            Target );

EFI_STATUS
PeiUsbSetConfiguration (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi );

EFI_STATUS
PeiUsbClearEndpointHalt (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8            EndpointAddress );

EFI_STATUS
PeiUsbIoControlTransfer(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT16       Request,
    IN UINT16       Index,
    IN UINT16       Value,
    IN OUT UINT8    *Buffer,
    IN UINT16       Length,
    OUT UINT32      *UsbStatus
);

BOOLEAN
IsPortConnect (
    UINT16 PortStatus );

BOOLEAN
IsPortEnable (
    UINT16 PortStatus );

BOOLEAN
IsPortLowSpeedDeviceAttached (
    UINT16 PortStatus );

BOOLEAN
IsPortHighSpeedDeviceAttached (
    UINT16 PortStatus );

BOOLEAN
IsPortSuperSpeedDeviceAttached (
    UINT16 PortStatus );

BOOLEAN
IsPortSuperSpeedPlusDeviceAttached(
    UINT16 PortStatus);

BOOLEAN
IsPortConnectChange (
    UINT16 PortChangeStatus );

VOID
ResetRootPort (
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *UsbHcPpi,
    UINT8                          PortNum );

EFI_STATUS
InitUsbKbdPPI(
    EFI_PEI_SERVICES    **PeiServices,
    PEI_USB_IO_PPI      *UsbIoPpi);

VOID
PeiUsbLibProcessKeypress(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi,
    IN UINT8   *UsbKeys);

EFI_STATUS
PeiUsbLibLightenKbLeds(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi);

EFI_STATUS
PeimInitializeUsbBot (
    IN EFI_PEI_SERVICES    **PeiServices );

EFI_STATUS
PeiHubEnumeration(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_DEVICE   *PeiUsbDevice,
    IN UINT8            *CurrentAddresss,
    IN BOOLEAN          CheckConnectChange);

EFI_STATUS
EFIAPI
NotifyOnRecoveryCapsuleLoaded(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN VOID                         *InvokePpi
);

EFI_STATUS
EFIAPI
InitializeUsbHostDevice(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN VOID                         *InvokePpi
);

EFI_STATUS
EFIAPI
InitializeUsbMassDevice(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN VOID                         *InvokePpi
);

#pragma pack(1)
typedef struct usb_hub_status
{
    UINT16 HubStatus;
    UINT16 HubChange;
} EFI_USB_HUB_STATUS;

#pragma pack()

#define PEI_MAX_USB_RECOVERY_INIT_PPI 16

// The following section should eventually go to SBPPI.H.
#ifndef PEI_USB_CHIP_INIT_PPI_GUID
#define PEI_USB_CHIP_INIT_PPI_GUID \
    { 0xdb75358d, 0xfef0, 0x4471, 0xa8, 0xd, 0x2e, 0xeb, 0x13, 0x8, 0x2d, 0x2d }

typedef EFI_STATUS (EFIAPI *PEI_ENABLE_CHIP_USB_RECOVERY) (
  IN EFI_PEI_SERVICES               **PeiServices
  );

typedef struct _PEI_USB_CHIP_INIT_PPI       PEI_USB_CHIP_INIT_PPI;

typedef struct _PEI_USB_CHIP_INIT_PPI {
    PEI_ENABLE_CHIP_USB_RECOVERY  EnableChipUsbRecovery;           
} PEI_USB_CHIP_INIT_PPI;
#endif

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
