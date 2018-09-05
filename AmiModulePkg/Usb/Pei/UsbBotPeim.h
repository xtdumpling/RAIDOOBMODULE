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

/** @file UsbBotPeim.h
    This file belongs to "Framework".
    This file is modified by AMI to include copyright message,
    appropriate header and integration code.

**/


//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//

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

   UsbBotPeim.h

   Abstract:

   Usb BOT Peim definition

   --*/

#ifndef _PEI_USB_BOT_PEIM_H
#define _PEI_USB_BOT_PEIM_H

//
// Driver consumed PPI Prototypes
//
//#include "Include\Ppi\UsbIo.h"
#include "UsbPei.h"
#include "Atapi.h"

//
// Driver produces PPI Prototypes
//
#include <Ppi/BlockIo.h>
//#include <Token.h>

#define PEI_FAT_MAX_USB_IO_PPI  127

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.BaseClass
//---------------------------------------------------------------------------
#define BASE_CLASS_HID           0x03
#define BASE_CLASS_MASS_STORAGE  0x08
#define BASE_CLASS_HUB           0x09
//----------------------------------------------------------------------------

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

//
// BlockIo PPI prototype
//
EFI_STATUS
EFIAPI
BotGetNumberOfBlockDevices(
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    OUT UINTN                        *NumberBlockDevices
);

EFI_STATUS
EFIAPI
BotGetMediaInfo(
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN UINTN                         DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA       *MediaInfo
);

EFI_STATUS
EFIAPI
BotReadBlocks(
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN UINTN                         DeviceIndex,
    IN EFI_PEI_LBA                   StartLBA,
    IN UINTN                         BufferSize,
    OUT VOID                         *Buffer
);

//
// UsbIo PPI Notification
//
EFI_STATUS
EFIAPI
NotifyOnUsbIoPpi(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *InvokePpi
);

EFI_STATUS
InitUsbBot (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi );

#define USBFLOPPY   1
#define USBFLOPPY2  2   // for those that use ReadCapacity(0x25)
                        // command to retrieve media capacity
#define USBCDROM    3
//
// Bot device structure
//
#define PEI_BOT_DEVICE_SIGNATURE  EFI_SIGNATURE_32( 'U', 'B', 'O', 'T' )
typedef struct
{
    UINTN Signature;
    EFI_PEI_RECOVERY_BLOCK_IO_PPI BlkIoPpi;
    EFI_PEI_PPI_DESCRIPTOR BlkIoPpiList;
    EFI_PEI_BLOCK_IO_MEDIA Media;
    PEI_USB_IO_PPI *UsbIoPpi;
    EFI_USB_INTERFACE_DESCRIPTOR  *BotInterface;
    EFI_USB_ENDPOINT_DESCRIPTOR   *BulkInEndpoint;
    EFI_USB_ENDPOINT_DESCRIPTOR   *BulkOutEndpoint;
    UINTN  DeviceType;
    REQUEST_SENSE_DATA *SensePtr;
    UINT32 FdEmulOffset;
    UINT8  Lun;
} PEI_BOT_DEVICE;

#define PEI_BOT_DEVICE_FROM_THIS( a ) \
    PEI_CR( a, PEI_BOT_DEVICE, BlkIoPpi, PEI_BOT_DEVICE_SIGNATURE )

//
// USB ATATPI command
//
EFI_STATUS
PeiAtapiCommand (
    IN EFI_PEI_SERVICES       **PeiServices,
    IN PEI_BOT_DEVICE         *PeiBotDev,
    IN VOID                   *Command,
    IN UINT8                  CommandSize,
    IN VOID                   *DataBuffer,
    IN UINT32                 BufferLength,
    IN EFI_USB_DATA_DIRECTION Direction,
    IN UINT16                 TimeOutInMilliSeconds );


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
