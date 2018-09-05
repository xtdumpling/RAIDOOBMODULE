/*++        

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  DriverBinding.c

Abstract:

  Implementation source file for the DeviceDriver driver binding protocol
  
--*/

#include "Tiano.h"
#include "EfiDriverLib.h"

//
// TODO: Assign a valid driver version that gets put in the
// Version field of the DriverBinding protocol interface. 
//
#define DRIVER_VERSION        0x00000001

// Include standard protocol and GUID definitions
//
#include EFI_PROTOCOL_PRODUCER (DriverBinding)

//
// Prototypes for our DriverBinding protocol functions
//
static 
EFI_STATUS
EFIAPI
Supported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath  OPTIONAL
  );

static 
EFI_STATUS
EFIAPI
Start (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath  OPTIONAL
  );

static 
EFI_STATUS
EFIAPI
Stop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer
  );


//
// Signature and structure definition for private data
// TODO: Update with reasonable signature value.
//
#define PRIVATE_DATA_SIGNATURE EFI_SIGNATURE_32('T', 'O', 'D', 'O') 

typedef struct {
  UINT32  Signature;
  EFI_DRIVER_BINDING_PROTOCOL DriverBindingProtocol;
  //
  // Add additional fields required for driver implementation
  //
} PRIVATE_DATA;

#define PRIVATE_DATA_FROM_THIS(T) CR (T, PRIVATE_DATA, Signature, PRIVATE_DATA_SIGNATURE)

//
// Create an instance of our private data. It can be used directly, or
// it can be used as a default value that can be copied to new instances.
//
static PRIVATE_DATA mPrivateData = {
  PRIVATE_DATA_SIGNATURE,
  //
  // Initializer for our DriverBinding protocol members
  //
  {
    Supported,               // EFI_DRIVER_BINDING_PROTOCOL.Supported;
    Start,                   // EFI_DRIVER_BINDING_PROTOCOL.Start;
    Stop,                    // EFI_DRIVER_BINDING_PROTOCOL.Stop;
    (UINT32)0,    // EFI_DRIVER_BINDING_PROTOCOL.Version  TODO: Set to valid value;
    (EFI_HANDLE)0,    // EFI_DRIVER_BINDING_PROTOCOL.ImageHandle  TODO: Set to valid value;
    (EFI_HANDLE)0,    // EFI_DRIVER_BINDING_PROTOCOL.DriverBindingHandle  TODO: Set to valid value;
  }
};

EFI_STATUS
DriverBindingProtocolInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN OUT EFI_HANDLE       *Handle
  )
/*++

Routine Description:

  Initialization function for the driver binding protocol implementation.

Arguments:

  ImageHandle   - image handle for this driver image
  SystemTable   - pointer to the EFI System Table
  Handle        - pointer to EFI handle on which to install the
                  protocol instance (if we produce multiple instances)

Returns:

  EFI_SUCCESS   - driver initialization completed successfully
  TODO: Add other return values  

--*/
{
  EFI_STATUS    Status;
  PRIVATE_DATA  *PrivateData;
  //
  // Allocate and initialize a private data structure. Copy the default
  // settings from our global template private data structure.
  //
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (PRIVATE_DATA), &PrivateData);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  gBS->CopyMem (PrivateData, &mPrivateData, sizeof (PRIVATE_DATA));
  PrivateData->DriverBindingProtocol.ImageHandle         = ImageHandle;
  PrivateData->DriverBindingProtocol.DriverBindingHandle = ImageHandle;
  PrivateData->DriverBindingProtocol.Version             = DRIVER_VERSION;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &PrivateData->DriverBindingProtocol,
                  NULL
                  );
  return Status;
}

static 
EFI_STATUS
EFIAPI
Supported (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
/*++

  Routine Description:
    Test to see if this driver supports ControllerHandle. 

  Arguments:
    This                - Protocol instance pointer.
    ControllerHandle    - Handle of device to test
    RemainingDevicePath - Optional parameter use to pick a specific child 
                          device to start.

  Returns:
    EFI_SUCCESS         - This driver supports this device
    EFI_ALREADY_STARTED - This driver is already running on this device
    other               - This driver does not support this device

--*/
{
  //EFI_STATUS Status;
  //
  // TODO: Make sure the protocol of interest is on the given controller handle
  // using code something like the following.
  //
  //Status = gBS->OpenProtocol (
  //                ControllerHandle,
  //                &gEfi<<ProtocolNameCx>>ProtocolGuid,
  //                (VOID **)&<<ProtocolNameInterface>>,
  //                gDriverBindingProtocol.DriverBindingHandle,
  //                ControllerHandle,
  //                EFI_OPEN_PROTOCOL_BY_DRIVER
  //                );
  //if (EFI_ERROR (Status)) {
  //  return Status;
  //}
  //gBS->CloseProtocol (
  //      ControllerHandle,
  //      &gEfi<<ProtocolNameCx>>ProtocolGuid,
  //      gDriverBindingProtocol.DriverBindingHandle,
  //      ControllerHandle
  //      );
  return EFI_SUCCESS;
}

static 
EFI_STATUS
EFIAPI
Start (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
/*++

  Routine Description:
    Start this driver on ControllerHandle.

  Arguments:
    This                - Protocol instance pointer.
    ControllerHandle    - Handle of device to bind driver to
    RemainingDevicePath - Optional parameter use to pick a specific child 
                          device to start.

  Returns:
    EFI_SUCCESS         - This driver is added to ControllerHandle
    EFI_ALREADY_STARTED - This driver is already running on ControllerHandle
    other               - This driver does not support this device

--*/
{
  EFI_STATUS    Status;
  PRIVATE_DATA  *PrivateData;
  
  PrivateData = NULL;
  //
  // TODO: Open the protocol that this driver consumes with something
  // like the following.
  //
  //Status = gBS->OpenProtocol (
  //                ControllerHandle,
  //                &gEfi<<ProtocolName>>ProtocolGuid,
  //                (VOID **)&ProtocolInterface,
  //                This->DriverBindingHandle,
  //                ControllerHandle,
  //                EFI_OPEN_PROTOCOL_BY_DRIVER
  //                );
  //if (EFI_ERROR (Status)) {
  //  return Status;
  //}
  //
  // Allocate private data to manage the device.
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (PRIVATE_DATA),
                  &PrivateData
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Copy the default settings from our global private data structure to 
  // this new private data. Then publish the protocol.
  //
  gBS->CopyMem (PrivateData, &mPrivateData, sizeof (PRIVATE_DATA));
  //Status = gBS->InstallMultipleProtocolInterfaces (
  //                ControllerHandle,
  //                &gEfi<<ProtocolName>>ProtocolGuid, 
  //                &PrivateData->ProtocolInterface,
  //                NULL
  //                );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  return EFI_SUCCESS;
ErrorExit:
  //
  // When we encounter errors, free up memory allocated and 
  // protocols opened.
  //
  if (PrivateData != NULL) {
    gBS->FreePool (PrivateData);
  }
  //
  // TODO: Close the protocol opened above with code similar to the following.
  //
  //gBS->CloseProtocol (
  //      ControllerHandle,
  //      &gEfi<<ProtocolName>>ProtocolGuid,
  //      This->DriverBindingHandle,
  //      ControllerHandle
  //      );
  return Status;
}

static 
EFI_STATUS
EFIAPI
Stop (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer
  )
/*++

  Routine Description:
    Stop this driver on ControllerHandle.

  Arguments:
    This              - Protocol instance pointer.
    ControllerHandle  - Handle of device to stop driver on 
    NumberOfChildren  - Number of Handles in ChildHandleBuffer. If number of 
                        children is zero stop the entire bus driver.
    ChildHandleBuffer - List of Child Handles to Stop.

  Returns:
    EFI_SUCCESS         - This driver is removed ControllerHandle
    other               - This driver was not removed from this device

--*/
{
  EFI_STATUS    Status;
  PRIVATE_DATA  *PrivateData;
  
  PrivateData = NULL;
  Status      = EFI_SUCCESS;
  //
  // TODO: Retrieve the interface to the protocol we produced in the Start()
  // function with code something like the following.
  //
  //Status = gBS->OpenProtocol (
  //                ControllerHandle,
  //                &gEfi<<ProtocolName>>ProtocolGuid,
  //                (VOID **)&ProtocolInterface,
  //                This->DriverBindingHandle,
  //                ControllerHandle, 
  //                EFI_OPEN_PROTOCOL_GET_PROTOCOL
  //                );
  //if (EFI_ERROR (Status)) {
  //  return EFI_UNSUPPORTED;
  //}
  //
  // TODO: Get private data with code something like the following.
  //
  // PrivateData = PRIVATA_DATA_FROM_THIS(ProtocolInterface);
  //
  // TODO: Uninstall the protocol we produced in Start() using code 
  // something like the following.
  //
  //Status = gBS->UninstallMultipleProtocolInterfaces (
  //                ControllerHandle,
  //                &gEfi<<ProtocolName>>ProtocolGuid, 
  //                &PrivateData->ProtocolInterface,
  //                NULL
  //                );
  if (!EFI_ERROR (Status)) {
    //
    // Close the protocol opened in Start()
    //
    //Status = gBS->CloseProtocol (
    //          ControllerHandle,
    //          &gEfi<<ProtocolName>>ProtocolGuid,
    //          This->DriverBindingHandle,
    //          ControllerHandle;
    //          );
    if (PrivateData != NULL) {
      gBS->FreePool (PrivateData);
    }
  }
  return Status; 
}

