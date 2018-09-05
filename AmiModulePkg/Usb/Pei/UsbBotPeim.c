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

/** @file UsbBotPeim.c
    This file belongs to "Framework".
    This file is modified by AMI to include copyright message,
    appropriate header and integration code.
    This file contains generic routines needed for USB recovery
    PEIM

**/


/*++
   This file contains a 'Sample Driver' and is licensed as such
   under the terms of your license agreement with Intel or your
   vendor.  This file may be modified by the user, subject to
   the additional terms of the license agreement
   --*/

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

   UsbBotPeim.c

   Abstract:

   Usb Bus PPI

   --*/

#include "UsbBotPeim.h"
#include "BotPeim.h"
#define PAGESIZE  4096
#include "Ppi/Stall.h"
#include "Ppi/LoadFile.h"

//#include EFI_PPI_DEFINITION( Stall )
//#include EFI_PPI_DEFINITION( LoadFile )

//
// Global function
//
EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH |
    EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmiPeiUsbIoPpiGuid,
    NotifyOnUsbIoPpi
};

EFI_PEI_RECOVERY_BLOCK_IO_PPI mRecoveryBlkIoPpi = {
    BotGetNumberOfBlockDevices,
    BotGetMediaInfo,
    BotReadBlocks
};


EFI_PEI_PPI_DESCRIPTOR mPpiList = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiVirtualBlockIoPpiGuid,
    NULL
};

EFI_STATUS
PeiBotDetectMedia (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_BOT_DEVICE   *PeiBotDev );

EFI_STATUS
BotCheckDeviceReady (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_BOT_DEVICE   *PeiBotDev);

EFI_STATUS PeimInitializeUsbBot (
    IN EFI_PEI_SERVICES    **PeiServices )
{
    EFI_STATUS     Status;
    UINTN          UsbIoPpiInstance;
    EFI_PEI_PPI_DESCRIPTOR *TempPpiDescriptor;
    PEI_USB_IO_PPI *UsbIoPpi;

    //
    // locate all usb io PPIs
    //
    for (UsbIoPpiInstance = 0;
         UsbIoPpiInstance < PEI_FAT_MAX_USB_IO_PPI;
         UsbIoPpiInstance++)
    {
        Status = (**PeiServices).LocatePpi( PeiServices,
            &gAmiPeiUsbIoPpiGuid,
            UsbIoPpiInstance,
            &TempPpiDescriptor,
            &UsbIoPpi
                 );
        if ( EFI_ERROR( Status ) ) {
            break;
        }
        InitUsbBot( PeiServices, UsbIoPpi );

    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
NotifyOnUsbIoPpi(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *InvokePpi
)
{
    PEI_USB_IO_PPI *UsbIoPpi;

    UsbIoPpi = (PEI_USB_IO_PPI *)InvokePpi;

    InitUsbBot(PeiServices, UsbIoPpi);

    return EFI_SUCCESS;
}

EFI_STATUS GetMaxLun (
    IN  EFI_PEI_SERVICES    **PeiServices,
    IN  PEI_USB_IO_PPI      *UsbIoPpi,
    IN  UINT8               Port,
    OUT UINT8               *MaxLun )
{
    EFI_USB_DEVICE_REQUEST      DevReq;
    EFI_STATUS                  EfiStatus;
    UINT32                      Timeout;
    UINT32                      UsbStatus;

    (**PeiServices).SetMem(&DevReq, sizeof(EFI_USB_DEVICE_REQUEST), 0);

    //
    // Fill Device request packet
    //
    DevReq.RequestType = 0xA1;
    DevReq.Request = 0x0FE;
    DevReq.Value = 0;
    DevReq.Index = Port;
    DevReq.Length = sizeof(UINT8);

    Timeout = 3000;

    EfiStatus = UsbIoPpi->UsbControlTransfer(
        PeiServices,
        UsbIoPpi,
        &DevReq,
        EfiUsbDataIn,
        Timeout,
        MaxLun,
        sizeof(UINT8),
        &UsbStatus
                );

    return EfiStatus;
}


EFI_STATUS InitUsbBot (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_USB_IO_PPI   *UsbIoPpi )
{
    STATIC UINTN   UsbIoPpiIndex = 0;

    PEI_BOT_DEVICE *PeiBotDevice;
    EFI_STATUS     Status;
    EFI_USB_INTERFACE_DESCRIPTOR *InterfaceDesc;
    UINT8                        *AllocateAddress;
    EFI_USB_ENDPOINT_DESCRIPTOR  *EndpointDesc;
    UINT8 i;
    UINT8 MaxLun = 0;                   
    UINT8 CurrentLun;                   
    //
    // Check its interface
    //
    Status = UsbIoPpi->UsbGetInterfaceDescriptor( PeiServices,
        UsbIoPpi, &InterfaceDesc );
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    //
    // Check if it is the BOT device we support
    //

    if ( (InterfaceDesc->InterfaceClass != BASE_CLASS_MASS_STORAGE)
        || (InterfaceDesc->InterfaceProtocol != PROTOCOL_BOT) ) {

        return EFI_NOT_FOUND;
    }

    Status = GetMaxLun(PeiServices,UsbIoPpi,InterfaceDesc->InterfaceNumber,&MaxLun);
    
    for(CurrentLun = 0; CurrentLun <= MaxLun; CurrentLun++) {
    
        Status = (*PeiServices)->AllocatePool( PeiServices,
            sizeof(PEI_BOT_DEVICE), &AllocateAddress );
        if ( EFI_ERROR( Status ) ) {
            return Status;
	    }

	    PeiBotDevice = (PEI_BOT_DEVICE *)AllocateAddress;
	    (**PeiServices).SetMem(PeiBotDevice, sizeof(PEI_BOT_DEVICE), 0);

        PeiBotDevice->Signature = PEI_BOT_DEVICE_SIGNATURE;
        PeiBotDevice->UsbIoPpi = UsbIoPpi;
        PeiBotDevice->BotInterface = InterfaceDesc;
        PeiBotDevice->FdEmulOffset = 0; //bala
        //
        // Default value
        //
        PeiBotDevice->Media.DeviceType = UsbMassStorage;
        PeiBotDevice->Media.BlockSize = 0x200;
        PeiBotDevice->Lun = CurrentLun;
    
        //
        // Check its Bulk-in/Bulk-out endpoint
        //
        for (i = 0; i < 2; i++) {
            Status = UsbIoPpi->UsbGetEndpointDescriptor( PeiServices,
                UsbIoPpi, i, &EndpointDesc );
            if ( EFI_ERROR( Status ) ) {
                return Status;
            }

            if (EndpointDesc->Attributes != 2) {
                continue;
            }

            if ( (EndpointDesc->EndpointAddress & 0x80) != 0 ) {
                PeiBotDevice->BulkInEndpoint = EndpointDesc;
            }
            else {
                PeiBotDevice->BulkOutEndpoint = EndpointDesc;
            }

        }

        PeiBotDevice->BlkIoPpi = mRecoveryBlkIoPpi;
        PeiBotDevice->BlkIoPpiList = mPpiList;
        PeiBotDevice->BlkIoPpiList.Ppi = &PeiBotDevice->BlkIoPpi;

        Status = PeiUsbInquiry( PeiServices, PeiBotDevice );
        if ( EFI_ERROR( Status ) ) {
            return Status;
        }

        Status = (**PeiServices).InstallPpi( PeiServices,
            &PeiBotDevice->BlkIoPpiList );
        if ( EFI_ERROR( Status ) ) {
            return Status;
        }
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
BotGetNumberOfBlockDevices(
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    OUT UINTN                        *NumberBlockDevices
)
{
    //
    // For Usb devices, this value should be always 1
    //

    *NumberBlockDevices = 1;
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
BotGetMediaInfo(
    IN EFI_PEI_SERVICES                     **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI        *This,
    IN UINTN                                DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA              *MediaInfo
)
{
    PEI_BOT_DEVICE      *PeiBotDev;
    EFI_STATUS          Status;

    PeiBotDev = PEI_BOT_DEVICE_FROM_THIS(This);

    Status = PeiBotDetectMedia(PeiServices, PeiBotDev);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    *MediaInfo = PeiBotDev->Media;

    return EFI_SUCCESS;
}

EFI_STATUS
PeiBotDetectMedia (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_BOT_DEVICE   *PeiBotDev
)
{

    EFI_STATUS              Status;
    UINT8                   *AllocateAddress;
    REQUEST_SENSE_DATA      *SensePtr;
    UINT8                   SenseKey;
    UINT8                   Asc;
    UINT8                   Ascq;
    UINT8                   RetryCount;
    UINT8                   RetryReq = 0;
    EFI_PEI_STALL_PPI       *StallPpi;

    //PeiUsbReadCapacity fills PeiBotDev structure for
    //BlockSize, LastBlock, Media Present
    for (RetryCount = 0; RetryCount < 25; RetryCount++) {
        Status = PeiUsbReadCapacity(PeiServices, PeiBotDev);

        if (!EFI_ERROR(Status)) {
            break;
        }

        //If ReadCapcity fails, then find out type of error
        if (RetryCount == 0) {
            if (PeiBotDev->SensePtr == NULL) {
                //During the first retry allocate the memory
                Status = (**PeiServices).AllocatePool(
                                        PeiServices,
                                        sizeof(REQUEST_SENSE_DATA),
                                        &AllocateAddress
                                        );
                if (EFI_ERROR(Status)) {
	                return Status;
	            }
                PeiBotDev->SensePtr = (REQUEST_SENSE_DATA *)AllocateAddress;
            }
            SensePtr = PeiBotDev->SensePtr;
            (**PeiServices).SetMem((VOID*)SensePtr, sizeof(REQUEST_SENSE_DATA), 0);
            Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid, 
                                    0, NULL, &StallPpi );
        }

        Status = PeiUsbRequestSense(PeiServices, PeiBotDev, (UINT8 *)SensePtr);
        if (EFI_ERROR(Status)) {
            //If RequestSense also fails, then there is an serious error
            //Return to the caller with appropriate error code
            //              PeiBotDev->Media.MediaPresent = FALSE;
            //              PeiBotDev->Media.BlockSize = 0;
            //              Status = EFI_DEVICE_ERROR;
            //              return EFI_DEVICE_ERROR;
        }
        //Parse the sense buffer for the error

        SenseKey = SensePtr->sense_key;
        Asc = SensePtr->addnl_sense_code;
        Ascq = SensePtr->addnl_sense_code_qualifier;
        
        if (SenseKey == 0x02) {
            //Logical Unit Problem
            switch (Asc) {
                case    0x3A:   // //Medium Not Present.
                    if (RetryCount >= 3) {
                        Status = BotCheckDeviceReady(PeiServices, PeiBotDev);
                        if (Status == EFI_NO_MEDIA) {
                            PeiBotDev->Media.MediaPresent = FALSE;
                            return Status;
                        }  
                    }
                case    0x04:   //Becoming Ready/Init Required/ Busy/ Format in Progress.
                case    0x06:   //No ref. found
                case    0x08:   //Comm failure
                    RetryReq = 1;   //Do retry
                    break;
                default:
                    break;
            }
        } else if (SenseKey == 0x06 && Asc == 0x28) {
        	RetryReq = 1;   //Do retry
        }

        PeiBotDev->Media.MediaPresent = FALSE;
        PeiBotDev->Media.BlockSize = 0;
        Status = EFI_DEVICE_ERROR;
        if (!RetryReq) {
            return Status;
        }
        //Wait for 100 msec
        StallPpi->Stall(PeiServices, StallPpi, 100 * 1000);
    }
    return Status;
}

EFI_STATUS
EFIAPI
BotReadBlocks(
    IN EFI_PEI_SERVICES                     **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI        *This,
    IN UINTN                                DeviceIndex,
    IN EFI_PEI_LBA                          StartLBA,
    IN UINTN                                BufferSize,
    OUT VOID                                *Buffer
)
{
    PEI_BOT_DEVICE          *PeiBotDev;
    EFI_STATUS              Status = EFI_SUCCESS;
    UINTN                   BlockSize;
    UINTN                   NumberOfBlocks;
    UINT8                   *AllocateAddress;
    REQUEST_SENSE_DATA      *SensePtr;
    UINT8                   SenseKey;
    UINT8                   Asc;
    UINT8                   Ascq;
    UINT8                   RetryCount;
    EFI_PEI_STALL_PPI       *StallPpi;

    PeiBotDev = PEI_BOT_DEVICE_FROM_THIS(This);

    //StartLBA += PeiBotDev->FdEmulOffset;

    //
    // Check parameters
    //
    if (Buffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize == 0) {
        return EFI_SUCCESS;
    }

    BlockSize = PeiBotDev->Media.BlockSize;

    if (BufferSize % BlockSize != 0) {
        Status = EFI_BAD_BUFFER_SIZE;
    }

    if (!PeiBotDev->Media.MediaPresent) {
        return EFI_NO_MEDIA;
    }

    if (StartLBA > PeiBotDev->Media.LastBlock) {
        return EFI_INVALID_PARAMETER;
    }

    NumberOfBlocks = BufferSize / (PeiBotDev->Media.BlockSize);

    for (RetryCount = 0; RetryCount < 3; RetryCount++) {
        
        Status = PeiUsbRead10(
            PeiServices,
            PeiBotDev,
            Buffer,
            StartLBA,
            NumberOfBlocks
                 );

        if (!EFI_ERROR(Status)) {
            break;
        }

        if (RetryCount == 0) {
            if (PeiBotDev->SensePtr == NULL) {
                Status = (**PeiServices).AllocatePool(
	                                    PeiServices,
	                                    sizeof(REQUEST_SENSE_DATA),
	                                    &AllocateAddress
	                                    );
	            if (EFI_ERROR(Status)) {
	                return Status;
	            }
                PeiBotDev->SensePtr = (REQUEST_SENSE_DATA *)AllocateAddress;
            }
            SensePtr = PeiBotDev->SensePtr;
            (**PeiServices).SetMem((VOID*)SensePtr, sizeof(REQUEST_SENSE_DATA), 0);
            Status = (**PeiServices).LocatePpi(PeiServices, &gEfiPeiStallPpiGuid, 
                                        0, NULL, &StallPpi);
        }

        Status = PeiUsbRequestSense(PeiServices, PeiBotDev, (UINT8 *)SensePtr);
        if (EFI_ERROR(Status)) {
            //If RequestSense also fails, then there is an serious error
            //Return to the caller with appropriate error code
            return EFI_DEVICE_ERROR;
        }
        //Parse the sense buffer for the error
        
        SenseKey = SensePtr->sense_key;
        Asc = SensePtr->addnl_sense_code;
        Ascq = SensePtr->addnl_sense_code_qualifier;

        if ((SenseKey == 0x02) && (Asc == 0x3A)) {
            Status = BotCheckDeviceReady(PeiServices, PeiBotDev);
            if (EFI_ERROR(Status)) {
                if (Status == EFI_NO_MEDIA) {
                    PeiBotDev->Media.MediaPresent = FALSE;
                }
                    return Status;
            }
        }
        StallPpi->Stall(PeiServices, StallPpi, 9000);
    }
    
    if (RetryCount == 3) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;

}

EFI_STATUS
BotCheckDeviceReady (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_BOT_DEVICE   *PeiBotDev
)
{
    UINT8                   RetryCount;
    UINT8                   SenseKey;
    UINT8                   Asc;
    UINT8                   Ascq;
    EFI_STATUS              Status;
    REQUEST_SENSE_DATA      *SensePtr;
    EFI_PEI_STALL_PPI       *StallPpi;
    UINT8                   *AllocateAddress;

    if (PeiBotDev->SensePtr == NULL) {
        Status = (**PeiServices).AllocatePool(
	                            PeiServices,
	                            sizeof(REQUEST_SENSE_DATA),
	                            &AllocateAddress
	                            );
        if (EFI_ERROR(Status)) {
            return Status;
        }
		PeiBotDev->SensePtr = (REQUEST_SENSE_DATA *)AllocateAddress;
    }
    SensePtr = PeiBotDev->SensePtr;
    (**PeiServices).SetMem((VOID*)SensePtr, sizeof(REQUEST_SENSE_DATA), 0);
    
    Status = (**PeiServices).LocatePpi(PeiServices,
                    &gEfiPeiStallPpiGuid, 0, NULL, &StallPpi);

    for (RetryCount = 0; RetryCount < 3; RetryCount++) {
        
        PeiUsbTestUnitReady(PeiServices, PeiBotDev);

        Status = PeiUsbRequestSense(PeiServices, PeiBotDev, (UINT8 *)SensePtr);
        if (EFI_ERROR(Status)) {
            //If RequestSense also fails, then there is an serious error
            //Return to the caller with appropriate error code
            return EFI_DEVICE_ERROR;
        }

        Status = EFI_DEVICE_ERROR;
        
        SenseKey = SensePtr->sense_key;
        Asc = SensePtr->addnl_sense_code;
        Ascq = SensePtr->addnl_sense_code_qualifier;

        if (SenseKey == 0) {
            Status = EFI_SUCCESS;
            break;
        }
        if ((SenseKey == 0x28) && (Asc == 0x06))  {
            Status = EFI_MEDIA_CHANGED;
            StallPpi->Stall(PeiServices, StallPpi, 100 * 1000);
            continue;
        }
        if ((SenseKey == 0x02) && (Asc == 0x3A)) {
            Status = EFI_NO_MEDIA;
            StallPpi->Stall(PeiServices, StallPpi, 20 * 1000);
            continue;
        }

    }
    return Status;
}

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
