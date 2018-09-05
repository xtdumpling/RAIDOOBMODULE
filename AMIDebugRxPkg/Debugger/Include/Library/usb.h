//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        USB.H
//
// Description: This file belongs to "Framework" and included here for
//              compatibility purposes. This file is modified by AMI to include
//              copyright message and appropriate header
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
#ifndef _USB_H
#define _USB_H

/*++

   Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

    usb.h

   Abstract:
    Support for USB 1.1 standard.




   Revision History

   --*/

// BIT(a) defines a value with one bit set as specified by a.
#ifndef BIT
#define BIT( a )  (1 << (a))
#endif


// MASK(high,low) defines a bit mask value with bits set in the bit
// range specified from high down to low (zero-based)
#ifndef MASK
#define MAX_VAL  0xffffffff     // maximum value supported by CPU
//#define MAX_BIT  31             // highest bit position in MAX_VAL
/*
#define MASK( high, low )  (\
        ( MAX_VAL >> ( MAX_BIT - ( (high) - (low) ) ) ) \
        << (low) \
)
*/
#define MASK( high, low )  (\
		( Shr64 ( MAX_VAL, ( MAX_BIT - ( (high) - (low) ) ) ) ) \
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

// return the modified src after writing a bit field with val, where the bit
// field's range is specified by high down to low
// (Note, the user must ensure that the val fits in the bit range)
#ifndef WRITE_BITS
#define WRITE_BITS( src, val, high, low )  (\
        ( (src) & ~MASK( (high), (low) ) ) \
        | ( (val) << (low) ) \
)
#endif

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


//
// USB Transfer Results
//
#define EFI_USB_NOERROR         0x00
#define EFI_USB_ERR_NOTEXECUTE  0x01
#define EFI_USB_ERR_STALL       0x02
#define EFI_USB_ERR_BUFFER      0x04
#define EFI_USB_ERR_BABBLE      0x08
#define EFI_USB_ERR_NAK         0x10
#define EFI_USB_ERR_CRC         0x20
#define EFI_USB_ERR_TIMEOUT     0x40
#define EFI_USB_ERR_BITSTUFF    0x80
#define EFI_USB_ERR_SYSTEM      0x100

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

//#define EFI_USB_INTERRUPT_DELAY  0x10000000

#define EFI_USB_STALL_ERROR \
    (EFI_USB_ERR_STALL | EFI_USB_ERR_CRC | \
     EFI_USB_ERR_TIMEOUT | EFI_USB_ERR_BABBLE)

//
// USB transation direction
//
/*typedef enum {
    EfiUsbDataIn,
    EfiUsbDataOut,
    EfiUsbNoData
} EFI_USB_DATA_DIRECTION;*/

//
// This is how UsbData in/out described in USB spec.
//
typedef enum {
    UsbDataOut,
    UsbDataIn,
    UsbNoData
} USB_DATA_DIRECTION;

//
// Usb Data recipient type
//
typedef enum {
    EfiUsbDevice,
    EfiUsbInterface,
    EfiUsbEndpoint
} EFI_USB_RECIPIENT;

typedef enum {
    EfiUsbEndpointHalt,
    EfiUsbDeviceRemoteWakeup
} EFI_USB_STANDARD_FEATURE_SELECTOR;

#pragma pack(push,1)

//
// Usb device request structure
//
typedef struct
{
    UINT8  RequestType;
    UINT8  Request;
    UINT16 Value;
    UINT16 Index;
    UINT16 Length;
} EFI_USB_DEVICE_REQUEST;


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
// Device descriptor. refer USB1.1
//
typedef struct usb_device_descriptor
{
    UINT8  Length;
    UINT8  DescriptorType;
    UINT16 BcdUSB;
    UINT8  DeviceClass;
    UINT8  DeviceSubClass;
    UINT8  DeviceProtocol;
    UINT8  MaxPacketSize0;
    UINT16 IdVendor;
    UINT16 IdProduct;
    UINT16 BcdDevice;
    UINT8  StrManufacturer;
    UINT8  StrProduct;
    UINT8  StrSerialNumber;
    UINT8  NumConfigurations;
} EFI_USB_DEVICE_DESCRIPTOR;

//
// Endpoint descriptor
//
typedef struct
{
    UINT8  Length;
    UINT8  DescriptorType;
    UINT8  EndpointAddress;
    UINT8  Attributes;
    UINT16 MaxPacketSize;
    UINT8  Interval;
} EFI_USB_ENDPOINT_DESCRIPTOR;

//
// Interface descriptor
//
typedef struct
{
    UINT8 Length;
    UINT8 DescriptorType;
    UINT8 InterfaceNumber;
    UINT8 AlternateSetting;
    UINT8 NumEndpoints;
    UINT8 InterfaceClass;
    UINT8 InterfaceSubClass;
    UINT8 InterfaceProtocol;
    UINT8 Interface;
} EFI_USB_INTERFACE_DESCRIPTOR;
/*
//
// USB alternate setting
//
typedef struct
{
    EFI_USB_INTERFACE_DESCRIPTOR *Interface;
} USB_ALT_SETTING;
*/
//
// Configuration descriptor
//
typedef struct
{
    UINT8  Length;
    UINT8  DescriptorType;
    UINT16 TotalLength;
    UINT8  NumInterfaces;
    UINT8  ConfigurationValue;
    UINT8  Configuration;
    UINT8  Attributes;
    UINT8  MaxPower;
} EFI_USB_CONFIG_DESCRIPTOR;

//
// Supported String Languages
//
typedef struct
{
    UINT8  Length;
    UINT8  DescriptorType;
    UINT16 SupportedLanID[1];
} EFI_USB_SUPPORTED_LANGUAGES;

//
// String descriptor
//
/*typedef struct
{
    UINT8  Length;
    UINT8  DescriptorType;
    CHAR16 String[1];
} EFI_USB_STRING_DESCRIPTOR;*/

//
// Hub descriptor
//
#define MAXBYTES  8
typedef struct
{
    UINT8	Length;
    UINT8	DescriptorType;
    UINT8	NbrPorts;
    UINT8	HubCharacteristics[2];
    UINT8	PwrOn2PwrGood;
    UINT8	HubContrCurrent;
	UINT8	HubHdrDecLat;
	UINT16	HubDelay;
    UINT16	DeviceRemovable;
} EFI_USB_HUB_DESCRIPTOR;

typedef struct
{
    UINT16 PortStatus;
    UINT16 PortChangeStatus;
} EFI_USB_PORT_STATUS;

//
// Constant value for Port Status & Port Change Status
//
#define USB_PORT_STAT_CONNECTION     BIT(0)
#define USB_PORT_STAT_ENABLE         BIT(1)
#define USB_PORT_STAT_SUSPEND        BIT(2)
#define USB_PORT_STAT_OVERCURRENT    BIT(3)
#define USB_PORT_STAT_RESET          BIT(4)
#define USB_PORT_STAT_POWER          BIT(8)
#define USB_PORT_STAT_LOW_SPEED      BIT(9)
#define USB_PORT_STAT_HIGH_SPEED     BIT(10)
#define USB_PORT_STAT_SUPER_SPEED    BIT(11)
#define USB_PORT_STAT_OWNER          BIT(13)
#define USB_PORT_STAT_C_CONNECTION   BIT(0)
#define USB_PORT_STAT_C_ENABLE       BIT(1)
#define USB_PORT_STAT_C_SUSPEND      BIT(2)
#define USB_PORT_STAT_C_OVERCURRENT  BIT(3)
#define USB_PORT_STAT_C_RESET        BIT(4)

//
// Used for set/clear port feature request
//
typedef enum {
	EfiUsbPortConnection        = 0,
    EfiUsbPortEnable            = 1,
    EfiUsbPortSuspend           = 2,
    EfiUsbPortOverCurrent       = 3,
    EfiUsbPortReset             = 4,
    EfiUsbPortLinkState         = 5,
    EfiUsbPortPower             = 8,
    EfiUsbPortOwner             = 13,
    EfiUsbPortConnectChange     = 16,
    EfiUsbPortEnableChange      = 17,
    EfiUsbPortSuspendChange     = 18,
    EfiUsbPortOverCurrentChange = 19,
    EfiUsbPortResetChange       = 20,
    EfiUsbPortTest              = 21,
    EfiUsbPortIndicator         = 22,
    EfiUsbPortU1Timeout         = 23,
    EfiUsbPortU2Timeout         = 24,
    EfiUsbPortLinkStateChange   = 25,
    EfiUsbPortConfigErrorChange = 26,
    EfiUsbPortRemoteWakeMask    = 27,
    EfiUsbPortBhPortReset       = 28,
    EfiUsbPortBhPortResetChange = 29,
    EfiUsbPortForceLinkPmAccept = 30
} EFI_USB_PORT_FEATURE;

#pragma pack(pop)

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
