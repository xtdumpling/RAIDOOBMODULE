//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  [Enhancements]Remove the " System Firmware Error(POST ERROR)" error log to BMC and "EFI 01030006" in BIOS event log.
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Jul/31/2017
//
//****************************************************************************
//****************************************************************************
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/
/*++

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
--*/

/** @file UefiBiosVideo.c
  ConsoleOut Routines that speak VGA.

**/
#include "Token.h"//SMC
#include "AmiDxeLib.h"
//*** AMI PORTING BEGIN ***//
#include "AcpiRes.h"
//*** AMI PORTING END *****//
#include "UefiBiosVideo.h"
#include "Protocol/AmiCsmVideoPolicy.h"
#include "Library/ReportStatusCodeLib.h"

#define TRACE_BIOS_VIDEO TRACE_ALWAYS
//#define TRACE_BIOS_VIDEO TRACE_NEVER

EFI_STATUS VgaClassDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS AllocateTheBuffers(
  IN BIOS_VIDEO_DEV *BiosVideoPrivate
  );

extern EFI_COMPONENT_NAME_PROTOCOL gCsmVideoComponentName;

//
// EFI Driver Binding Protocol Instance
//
EFI_DRIVER_BINDING_PROTOCOL gBiosVideoDriverBinding = {
  BiosVideoDriverBindingSupported,
  BiosVideoDriverBindingStart,
  BiosVideoDriverBindingStop,
  0x00000024,
  NULL,
  NULL
};

// The following is copied from GraphicsConsole: share the type and variable definitions between these two drivers
#define MAX_RES             0
#define MIN_RES             -1
typedef struct _TEXT_MODE  {
	INT32	ModeNum;
	INT32	Col;
	INT32	Row;
	UINT32	VideoCol; // horizontal pixels
	UINT32	VideoRow; // vertical pixels
} TEXT_MODE;

const TEXT_MODE TextModeArray[] = {GC_MODE_LIST};
const INT32 MaxTextMode=(sizeof(TextModeArray)/sizeof(TEXT_MODE));

//*** AMI PORTING BEGIN ***//
#if INT10_VGA_GO_SUPPORT==1
//*** AMI PORTING END *****//
//
// Global lookup tables for VGA graphics modes
//
UINT8                       mVgaLeftMaskTable[]   = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

UINT8                       mVgaRightMaskTable[]  = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

UINT8                       mVgaBitMaskTable[]    = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

EFI_GRAPHICS_OUTPUT_BLT_PIXEL  mVgaColorToGraphicsOutputColor[] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x98, 0x00, 0x00, 0x00 },
  { 0x00, 0x98, 0x00, 0x00 },
  { 0x98, 0x98, 0x00, 0x00 },
  { 0x00, 0x00, 0x98, 0x00 },
  { 0x98, 0x00, 0x98, 0x00 },
  { 0x00, 0x98, 0x98, 0x00 },
  { 0x98, 0x98, 0x98, 0x00 },
  { 0x10, 0x10, 0x10, 0x00 },
  { 0xff, 0x10, 0x10, 0x00 },
  { 0x10, 0xff, 0x10, 0x00 },
  { 0xff, 0xff, 0x10, 0x00 },
  { 0x10, 0x10, 0xff, 0x00 },
  { 0xf0, 0x10, 0xff, 0x00 },
  { 0x10, 0xff, 0xff, 0x00 },
  { 0xff, 0xff, 0xff, 0x00 }
};

//*** AMI PORTING BEGIN ***//
#endif //if INT10_VGA_GO_SUPPORT==1
#if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//

//
// Standard timing defined by VESA EDID
//
VESA_BIOS_EXTENSIONS_EDID_TIMING mEstablishedEdidTiming[] = {
  //
  // Established Timing I
  //
  {800, 600, 60},
  {800, 600, 56},
  {640, 480, 75},
  {640, 480, 72},
  {640, 480, 67},
  {640, 480, 60},
  {720, 400, 88},
  {720, 400, 70},
  //
  // Established Timing II
  //
  {1280, 1024, 75},
  {1024,  768, 75},
  {1024,  768, 70},
  {1024,  768, 60},
  {1024,  768, 87},
  {832,   624, 75},
  {800,   600, 75},
  {800,   600, 72},
  //
  // Established Timing III
  //
  {1152, 870, 75}
};
//*** AMI PORTING BEGIN ***//
#endif //if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//

UINT32 SupportedResolutions[29] = { 0 };

EFI_STATUS
BiosVideoChildHandleInstall (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ParentHandle,
  IN  EFI_PCI_IO_PROTOCOL          *ParentPciIo,
  IN  EFI_LEGACY_BIOS_PROTOCOL     *ParentLegacyBios,
  IN  EFI_DEVICE_PATH_PROTOCOL     *ParentDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
;

EFI_STATUS
BiosVideoChildHandleUninstall (
  EFI_DRIVER_BINDING_PROTOCOL    *This,
  EFI_HANDLE                     Controller,
  EFI_HANDLE                     Handle
  )
;

VOID
BiosVideoDeviceReleaseResource (
  BIOS_VIDEO_DEV  *BiosVideoPrivate
  )
;

//*** AMI PORTING BEGIN ***//
//CsmVideo Policy Protocol support
//The protocol can be used to set the driver policy.
// The two supported policies are:
// SimpleTextOut over the text mode 3 (TextMode=TRUE)
// GOP over graphical mode (TextMode=FALSE)
EFI_HANDLE VgaControllerHandle = NULL; 
BOOLEAN TextModePolicy = FALSE;


EFI_STATUS CsmVideoPolicySetMode(
    IN AMI_CSM_VIDEO_POLICY_PROTOCOL *This,
    IN BOOLEAN TextMode, IN BOOLEAN ForceModeChange
){
#if INT10_SIMPLE_TEXT_SUPPORT==0
    return (TextMode) ? EFI_UNSUPPORTED : EFI_SUCCESS;
#else
    EFI_STATUS Status;
    BOOLEAN OriginalPolicy=TextModePolicy;
    EFI_HANDLE Handle; 

    TextModePolicy = TextMode;
    if (   (VgaControllerHandle == NULL )
        || ((OriginalPolicy==TextModePolicy) && !ForceModeChange)
    ) return EFI_SUCCESS;
    
    // save the VGA handle
    // the global variable VgaControllerHandle will be nullified
    // by the Stop function during Disconnect.
    Handle = VgaControllerHandle; 
    Status = pBS->DisconnectController( Handle, NULL, NULL );
    if (!EFI_ERROR(Status))
        Status = pBS->ConnectController( Handle, NULL, NULL, TRUE );
    else
        TextModePolicy = OriginalPolicy;
    if (EFI_ERROR(Status)) return Status;
    return (TextModePolicy==TextMode) ? EFI_SUCCESS : EFI_UNSUPPORTED;
#endif
}

EFI_STATUS CsmVideoPolicyGetMode(
    IN AMI_CSM_VIDEO_POLICY_PROTOCOL *This,IN BOOLEAN *IsTextMode
){
    if (IsTextMode!=NULL) *IsTextMode = TextModePolicy;
    return (VgaControllerHandle == NULL) ? EFI_NOT_STARTED : EFI_SUCCESS;
}

AMI_CSM_VIDEO_POLICY_PROTOCOL CsmVideoPolicyProtocol = {
    CsmVideoPolicyGetMode, CsmVideoPolicySetMode
};

//*** AMI PORTING END *****//

EFI_STATUS GetBadEdid (
  IN  EFI_EDID_OVERRIDE_PROTOCOL        *This,
  IN  EFI_HANDLE                        *ChildHandle,
  OUT UINT32                            *Attributes,
  IN OUT UINTN                          *EdidSize,
  IN OUT UINT8                          **Edid
  )
{
  static UINT8 EdidData[0x80] = {0};
  EdidData[0x7f] = 1; // invalid checksum

  *Edid = EdidData;
  *EdidSize = 0x80;
  *Attributes = 0;

  return EFI_SUCCESS;
}


//
// Driver Entry Point
//
EFI_STATUS
EFIAPI
BiosVideoDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

  Routine Description:

    Driver Entry Point.

  Arguments:

  ImageHandle - Handle of driver image.
  SystemTable - Pointer to system table.

  Returns:

    EFI_STATUS

--*/
{
  EFI_STATUS  Status;

  InitAmiLib(ImageHandle, SystemTable);

  gBiosVideoDriverBinding.DriverBindingHandle = ImageHandle;
  gBiosVideoDriverBinding.ImageHandle = ImageHandle;

  Status = pBS->InstallMultipleProtocolInterfaces(
             &gBiosVideoDriverBinding.DriverBindingHandle,
             &gEfiDriverBindingProtocolGuid, &gBiosVideoDriverBinding,
			 &gEfiComponentNameProtocolGuid, &gCsmVideoComponentName,
			 &gAmiCsmThunkDriverGuid, NULL,
             NULL
             );

#if INT10_SIMPLE_TEXT_SUPPORT==1
{
	// Create new handle
	EFI_HANDLE Handle = NULL;
	VgaClassDriverEntryPoint(Handle, SystemTable);
}
#endif

#if INT10_TRUST_EDID_INFORMATION == 0
{
  // Install EdidOverride with the dummy EDID information; this is needed to ignore
  // bad EDID information
  EFI_STATUS Status;
  static EFI_EDID_OVERRIDE_PROTOCOL  EdidOverride = { GetBadEdid };

  Status = pBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gEfiEdidOverrideProtocolGuid, &EdidOverride,
    NULL
  );
}
#endif

//Install CsmVideoPolicy protocol
  Status = pBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gAmiCsmVideoPolicyProtocolGuid, &CsmVideoPolicyProtocol,
    NULL
  );

  return Status;
}


/*++

Routine Description:

  Callback function for exit boot service event

Arguments:

  Event   - EFI_EVENT structure
  Context - Event context

Returns:

  None

--*/
/*{
  BIOS_VIDEO_DEV        *BiosVideoPrivate;
  EFI_IA32_REGISTER_SET Regs;

  //
  // Get our context
  //
  BiosVideoPrivate = (BIOS_VIDEO_DEV *) Context;

  //
  // Set the 80x25 Text VGA Mode
  //
  Regs.H.AH = 0x00;
  Regs.H.AL = 0x83;
  BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

  Regs.H.AH = 0x11;
  Regs.H.AL = 0x14;
  Regs.H.BL = 0;
  BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);
}
*/
//*** AMI PORTING END *****//

EFI_STATUS
EFIAPI
BiosVideoDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
/*++

  Routine Description:

    Supported.

  Arguments:

  This - Pointer to driver binding protocol
  Controller - Controller handle to connect
  RemainingDevicePath - A pointer to the remaining portion of a device path


  Returns:

  EFI_STATUS - EFI_SUCCESS:This controller can be managed by this driver,
               Otherwise, this controller cannot be managed by this driver

--*/
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  PCI_TYPE00                Pci;

  //
  // See if the Legacy BIOS Protocol is available
  //
  Status = pBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = pBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // See if this is a PCI Graphics Controller by looking at the Command register and
  // Class Code Register
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  Status = EFI_UNSUPPORTED;
  if (Pci.Hdr.ClassCode[2] == 0x03 || (Pci.Hdr.ClassCode[2] == 0x00 && Pci.Hdr.ClassCode[1] == 0x01)) {
    Status = EFI_SUCCESS;
  }

Done:
  pBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

EFI_STATUS
EFIAPI
BiosVideoDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
/*++

  Routine Description:

    Install Graphics Output Protocol onto VGA device handles

  Arguments:

  This - Pointer to driver binding protocol
  Controller - Controller handle to connect
  RemainingDevicePath - A pointer to the remaining portion of a device path

  Returns:

    EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *ParentDevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINTN                     Flags;
  UINT64 Capabilities = 0;	// See comments below regarding the Capabilities usage

  PciIo = NULL;
  //
  // Prepare for status code
  //
  Status = pBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Open the IO Abstraction(s) needed
  //
  Status = pBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // See if the Legacy BIOS Protocol is available
  //
  Status = pBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
/*
  ReportStatusCodeWithDevicePath (
    EFI_PROGRESS_CODE,
    EFI_P_PC_ENABLE,
    0,
    &gEfiCallerIdGuid,
    ParentDevicePath
    );*/
  //
  // Enable the device and make sure VGA cycles are being forwarded to this VGA device
  //
// We need to check what is supported by the hardware before enabling attributes
  Status = PciIo->Attributes (
      PciIo,
      EfiPciIoAttributeOperationSupported,
      0,
      &Capabilities
      );

  if ( EFI_ERROR(Status) ) {
    goto Done;
  }

  Status = PciIo->Attributes (
      PciIo,
      EfiPciIoAttributeOperationEnable,
      (Capabilities & EFI_PCI_DEVICE_ENABLE) | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
      NULL
      );
/*
  Status = PciIo->Attributes (
             PciIo,
             EfiPciIoAttributeOperationEnable,
             EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
             NULL
             );
*/

  if (EFI_ERROR (Status)) {
    /*ReportStatusCodeWithDevicePath (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,
      0,
      &gEfiCallerIdGuid,
      ParentDevicePath
      );*/
    goto Done;
  }
  //
  // Check to see if there is a legacy option ROM image associated with this PCI device
  //
  Status = LegacyBios->CheckPciRom (
                         LegacyBios,
                         Controller,
                         NULL,
                         NULL,
                         &Flags
                         );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Post the legacy option ROM if it is available.
  //
  /*ReportStatusCodeWithDevicePath (
    EFI_PROGRESS_CODE,
    EFI_P_PC_RESET,
    0,
    &gEfiCallerIdGuid,
    ParentDevicePath
    );*/
  Status = LegacyBios->InstallPciRom (
                         LegacyBios,
                         Controller,
                         NULL,
                         &Flags,
                         NULL,
                         NULL,
                         NULL,
                         NULL
                         );
  if (EFI_ERROR (Status)) {
#if (SMCPKG_SUPPORT == 0)     
      REPORT_STATUS_CODE_WITH_DEVICE_PATH(
              EFI_ERROR_CODE | EFI_ERROR_MINOR,
              EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,
              ParentDevicePath
              );
#endif              
    /*ReportStatusCodeWithDevicePath (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,
      0,
      &gEfiCallerIdGuid,
      ParentDevicePath
      );*/
    goto Done;
  }

  //
  // Create child handle and install GraphicsOutputProtocol on it
  //
  Status = BiosVideoChildHandleInstall (
             This,
             Controller,
             PciIo,
             LegacyBios,
             ParentDevicePath,
             RemainingDevicePath
             );

Done:
  if (EFI_ERROR (Status)) {
    if (PciIo != NULL) {
      /*ReportStatusCodeWithDevicePath (
        EFI_PROGRESS_CODE,
        EFI_P_PC_DISABLE,
        0,
        &gEfiCallerIdGuid,
        ParentDevicePath
        );*/
      //
      // Turn off the PCI device and disable forwarding of VGA cycles to this device
      //
      if (Capabilities != 0)
      {
          PciIo->Attributes (
                  PciIo,
                  EfiPciIoAttributeOperationDisable,
                  (Capabilities & EFI_PCI_DEVICE_ENABLE) | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
           //               EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                  NULL
          );
      }
      //
      // Release PCI I/O Protocols on the controller handle.
      //
      pBS->CloseProtocol (
             Controller,
             &gEfiPciIoProtocolGuid,
             This->DriverBindingHandle,
             Controller
             );
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
BiosVideoDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
/*++

  Routine Description:

    Stop.

  Arguments:

  This - Pointer to driver binding protocol
  Controller - Controller handle to connect
  NumberOfChilren - Number of children handle created by this driver
  ChildHandleBuffer - Buffer containing child handle created

  Returns:

  EFI_SUCCESS - Driver disconnected successfully from controller
  EFI_UNSUPPORTED - Cannot find BIOS_VIDEO_DEV structure

--*/
{
  EFI_STATUS                   Status;
  BOOLEAN                      AllChildrenStopped;
  UINTN                        Index;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT64                        Capabilities;

  if (NumberOfChildren == 0) {
//*** AMI PORTING BEGIN ***//
//In text mode, the child handle is not created 
//and the mini port protocol is installed on the PciIo handle.
//Original implementation does not stop controller in this case
//Here is the fix:
#if INT10_SIMPLE_TEXT_SUPPORT==1
    Status = pBS->OpenProtocol (
                  Controller,
                  &gEfiVgaMiniPortProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
    if (!EFI_ERROR(Status)){
        return BiosVideoChildHandleUninstall (This, Controller, Controller);
    }
#endif

    Status = pBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
    ASSERT_EFI_ERROR(Status);

    Status = PciIo->Attributes (
      PciIo,
      EfiPciIoAttributeOperationSupported,
      0,
      &Capabilities
      );
    ASSERT_EFI_ERROR(Status);    

    Status = PciIo->Attributes (
        PciIo,
        EfiPciIoAttributeOperationDisable,
        (Capabilities & EFI_PCI_DEVICE_ENABLE) | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
        NULL
        );

    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return EFI_DEVICE_ERROR;
//*** AMI PORTING END *****//
    //
    // Close PCI I/O protocol on the controller handle
    //
    pBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );

    return EFI_SUCCESS;
  }

  AllChildrenStopped = TRUE;
  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = BiosVideoChildHandleUninstall (This, Controller, ChildHandleBuffer[Index]);

    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (!AllChildrenStopped) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
BiosVideoChildHandleInstall (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ParentHandle,
  IN  EFI_PCI_IO_PROTOCOL          *ParentPciIo,
  IN  EFI_LEGACY_BIOS_PROTOCOL     *ParentLegacyBios,
  IN  EFI_DEVICE_PATH_PROTOCOL     *ParentDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
/*++

Routine Description:
  Install child handles if the Handle supports MBR format.

Arguments:       
  This       - Calling context.
  Handle     - Parent Handle 
  PciIo      - Parent PciIo interface
  LegacyBios  - Parent LegacyBios interface
  DevicePath - Parent Device Path

Returns:
  EFI_SUCCESS - If a child handle was added
  other       - A child handle was not added

--*/
{
  EFI_STATUS               Status;
  BIOS_VIDEO_DEV           *BiosVideoPrivate;
  PCI_TYPE00               Pci;
  ACPI_ADR_DEVICE_PATH     AcpiDeviceNode;

  //
  // Allocate the private device structure for video device
  //
  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (BIOS_VIDEO_DEV),
                  &BiosVideoPrivate
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  pBS->SetMem (BiosVideoPrivate, sizeof (BIOS_VIDEO_DEV), 0);

  //
  // See if this is a VGA compatible controller or not
  //
  Status = ParentPciIo->Pci.Read (
                          ParentPciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof (Pci) / sizeof (UINT32),
                          &Pci
                          );
  if (EFI_ERROR (Status)) {
      REPORT_STATUS_CODE_WITH_DEVICE_PATH(
              EFI_ERROR_CODE | EFI_ERROR_MINOR,
              EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,
              ParentDevicePath
              );
    /*ReportStatusCodeWithDevicePath (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,
      0,
      &gEfiCallerIdGuid,
      ParentDevicePath
      );*/
    goto Done;
  }
  BiosVideoPrivate->VgaCompatible = FALSE;
  if (Pci.Hdr.ClassCode[2] == 0x00 && Pci.Hdr.ClassCode[1] == 0x01) {
    BiosVideoPrivate->VgaCompatible = TRUE;
  }

  if (Pci.Hdr.ClassCode[2] == 0x03 && Pci.Hdr.ClassCode[1] == 0x00 && Pci.Hdr.ClassCode[0] == 0x00) {
    BiosVideoPrivate->VgaCompatible = TRUE;
  }

  //
  // Initialize the child private structure
  //
  BiosVideoPrivate->Signature = BIOS_VIDEO_DEV_SIGNATURE;
  BiosVideoPrivate->Handle = NULL;

//*** AMI PORTING BEGIN ***//
//Exit Boot Services callback is not needed.
/*
  Status = pBS->CreateEvent (
                  EFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES,
                  EFI_TPL_NOTIFY,
                  BiosVideoExitBootServices,
                  BiosVideoPrivate,
                  &BiosVideoPrivate->ExitBootServicesEvent
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
*/
//*** AMI PORTING END *****//

  //
  // Fill in Graphics Output specific mode structures
  //
  BiosVideoPrivate->HardwareNeedsStarting = TRUE;
  BiosVideoPrivate->ModeData              = NULL;
  BiosVideoPrivate->LineBuffer            = NULL;
  BiosVideoPrivate->VgaFrameBuffer        = NULL;
  BiosVideoPrivate->VbeFrameBuffer        = NULL;

//*** AMI PORTING BEGIN ***//
//Let's do it only if VgaMiniPort is going to be installed
/*
  //
  // Fill in the VGA Mini Port Protocol fields
  //
  BiosVideoPrivate->VgaMiniPort.SetMode                   = BiosVideoVgaMiniPortSetMode;
  BiosVideoPrivate->VgaMiniPort.VgaMemoryOffset           = 0xb8000;
  BiosVideoPrivate->VgaMiniPort.CrtcAddressRegisterOffset = 0x3d4;
  BiosVideoPrivate->VgaMiniPort.CrtcDataRegisterOffset    = 0x3d5;
  BiosVideoPrivate->VgaMiniPort.VgaMemoryBar              = EFI_PCI_IO_PASS_THROUGH_BAR;
  BiosVideoPrivate->VgaMiniPort.CrtcAddressRegisterBar    = EFI_PCI_IO_PASS_THROUGH_BAR;
  BiosVideoPrivate->VgaMiniPort.CrtcDataRegisterBar       = EFI_PCI_IO_PASS_THROUGH_BAR;
*/
  //
  // Assume that Graphics Output Protocol will be produced until proven otherwise
  //
  BiosVideoPrivate->ProduceGraphicsOutput = TRUE;

  //
  // Child handle need to consume the Legacy Bios protocol
  //
  BiosVideoPrivate->LegacyBios = ParentLegacyBios;

  //
  // When check for VBE, PCI I/O protocol is needed, so use parent's protocol interface temporally
  //
  BiosVideoPrivate->PciIo = ParentPciIo;

  //
  // Check for VESA BIOS Extensions for modes that are compatible with Graphics Output
  //
//*** AMI PORTING BEGIN ***//
  Status = EFI_UNSUPPORTED;
  if ( !TextModePolicy ){
      // install device path
      if (RemainingDevicePath == NULL) {
        pBS->SetMem (&AcpiDeviceNode, sizeof (ACPI_ADR_DEVICE_PATH), 0);
        AcpiDeviceNode.Header.Type = ACPI_DEVICE_PATH;
        AcpiDeviceNode.Header.SubType = ACPI_ADR_DP;
        AcpiDeviceNode.ADR = ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0);
        SetDevicePathNodeLength (&AcpiDeviceNode.Header, sizeof (ACPI_ADR_DEVICE_PATH));

        BiosVideoPrivate->DevicePath = AppendDevicePathNode (ParentDevicePath, (EFI_DEVICE_PATH_PROTOCOL *)&AcpiDeviceNode);
      } else {
        BiosVideoPrivate->DevicePath = AppendDevicePathNode (ParentDevicePath, RemainingDevicePath);
      }

      pBS->InstallMultipleProtocolInterfaces (
                      &BiosVideoPrivate->Handle,
                      &gEfiDevicePathProtocolGuid, BiosVideoPrivate->DevicePath,
                      NULL
                      );
      
#if INT10_VESA_GO_SUPPORT==1    
      Status = BiosVideoCheckForVbe (BiosVideoPrivate, &Pci);
#endif      
#if INT10_VGA_GO_SUPPORT==1
      if (EFI_ERROR (Status) && BiosVideoPrivate->VgaCompatible) {
        Status = BiosVideoCheckForVga (BiosVideoPrivate);
      }
#endif
      if (EFI_ERROR (Status)) {
         // uninstall device path
          pBS->UninstallMultipleProtocolInterfaces (
                          BiosVideoPrivate->Handle,
                          &gEfiDevicePathProtocolGuid, BiosVideoPrivate->DevicePath,
                          NULL
                          );
          
      }
  }
  
//*** AMI PORTING END *****//

  if (EFI_ERROR (Status)) {
    //
    // Neither VBE nor the standard 640x480 16 color VGA mode are supported, so do
    // not produce the Graphics Output protocol.  Instead, produce the VGA MiniPort Protocol.
    //
    BiosVideoPrivate->ProduceGraphicsOutput = FALSE;

    //
    // INT services are available, so on the 80x25 and 80x50 text mode are supported
    //
    BiosVideoPrivate->VgaMiniPort.MaxMode = 2;
  }

  if (BiosVideoPrivate->ProduceGraphicsOutput) {
    //
    // Creat child handle and install Graphics Output Protocol,EDID Discovered/Active Protocol
    //
    Status = pBS->InstallMultipleProtocolInterfaces (
                    &BiosVideoPrivate->Handle,
                    &gEfiGraphicsOutputProtocolGuid, &BiosVideoPrivate->GraphicsOutput,
                    &gEfiEdidDiscoveredProtocolGuid, &BiosVideoPrivate->EdidDiscovered,
                    &gEfiEdidActiveProtocolGuid, &BiosVideoPrivate->EdidActive,
                    NULL
                    );

    if (!EFI_ERROR (Status)) {
      //
      // Open the Parent Handle for the child
      //
      Status = pBS->OpenProtocol (
                      ParentHandle,
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &BiosVideoPrivate->PciIo,
                      This->DriverBindingHandle,
                      BiosVideoPrivate->Handle,
                      EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                      );
    }
  } else {
//*** AMI PORTING BEGIN ***//
#if INT10_SIMPLE_TEXT_SUPPORT==1
//The code if moved here from BiosVideoChildHandleInstall
      //
      // Fill in the VGA Mini Port Protocol fields
      //
      BiosVideoPrivate->VgaMiniPort.SetMode                   = BiosVideoVgaMiniPortSetMode;
      BiosVideoPrivate->VgaMiniPort.VgaMemoryOffset           = 0xb8000;
      BiosVideoPrivate->VgaMiniPort.CrtcAddressRegisterOffset = 0x3d4;
      BiosVideoPrivate->VgaMiniPort.CrtcDataRegisterOffset    = 0x3d5;
      BiosVideoPrivate->VgaMiniPort.VgaMemoryBar              = EFI_PCI_IO_PASS_THROUGH_BAR;
      BiosVideoPrivate->VgaMiniPort.CrtcAddressRegisterBar    = EFI_PCI_IO_PASS_THROUGH_BAR;
      BiosVideoPrivate->VgaMiniPort.CrtcDataRegisterBar       = EFI_PCI_IO_PASS_THROUGH_BAR;
//*** AMI PORTING END *****//
//*** AMI PORTING BEGIN ***//
    //This is needed! Otherwise VgaClass driver rejects VgaMiniPort
    //device due to the lack of PciIo protocol
      BiosVideoPrivate->Handle=ParentHandle;
//*** AMI PORTING END *****//
      //
      // Install VGA Mini Port Protocol
      //
      Status = pBS->InstallMultipleProtocolInterfaces (
                    &BiosVideoPrivate->Handle,
                    &gEfiVgaMiniPortProtocolGuid,
                    &BiosVideoPrivate->VgaMiniPort,
                    NULL
                    );
//*** AMI PORTING BEGIN ***//
#else
#endif //INT10_SIMPLE_TEXT_SUPPORT==1
//*** AMI PORTING END *****//
  }

Done:
  if (EFI_ERROR (Status)) {
    //
    // Free private data structure
    //
    BiosVideoDeviceReleaseResource (BiosVideoPrivate);
  }
//*** AMI PORTING BEGIN ***//
  else {
    // The protocols are installed.
    // Update global variables used by the CsmVideoPolicy protocol implementation.
      TextModePolicy = !BiosVideoPrivate->ProduceGraphicsOutput;
      VgaControllerHandle = ParentHandle;
  }
//*** AMI PORTING END *****//
  return Status;
}

EFI_STATUS
BiosVideoChildHandleUninstall (
  EFI_DRIVER_BINDING_PROTOCOL    *This,
  EFI_HANDLE                     Controller,
  EFI_HANDLE                     Handle
  )
/*++

Routine Description:

  Deregister an video child handle and free resources

Arguments:

  This            - Protocol instance pointer.
  Controller      - Video controller handle
  Handle          - Video child handle

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                   Status;
  EFI_IA32_REGISTER_SET        Regs;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
  EFI_VGA_MINI_PORT_PROTOCOL   *VgaMiniPort;
  BIOS_VIDEO_DEV               *BiosVideoPrivate;
  EFI_PCI_IO_PROTOCOL          *PciIo;

  BiosVideoPrivate = NULL;

  Status = pBS->OpenProtocol (
                  Handle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput,
                  This->DriverBindingHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    BiosVideoPrivate = BIOS_VIDEO_DEV_FROM_GRAPHICS_OUTPUT_THIS (GraphicsOutput);
  }

  Status = pBS->OpenProtocol (
                  Handle,
                  &gEfiVgaMiniPortProtocolGuid,
                  (VOID **) &VgaMiniPort,
                  This->DriverBindingHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    BiosVideoPrivate = BIOS_VIDEO_DEV_FROM_VGA_MINI_PORT_THIS (VgaMiniPort);
  }

  if (BiosVideoPrivate == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Close PCI I/O protocol that opened by child handle
  //
  Status = pBS->CloseProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  This->DriverBindingHandle,
                  Handle
                  );

  //
  // Uninstall protocols on child handle
  //
  if (BiosVideoPrivate->ProduceGraphicsOutput) {
    Status = pBS->UninstallMultipleProtocolInterfaces (
                    BiosVideoPrivate->Handle,
                    &gEfiDevicePathProtocolGuid,
                    BiosVideoPrivate->DevicePath,
                    &gEfiGraphicsOutputProtocolGuid,
                    &BiosVideoPrivate->GraphicsOutput,
                    &gEfiEdidDiscoveredProtocolGuid,
                    &BiosVideoPrivate->EdidDiscovered,
                    &gEfiEdidActiveProtocolGuid,
                    &BiosVideoPrivate->EdidActive,
                    NULL
                    );
  } else {
    Status = pBS->UninstallMultipleProtocolInterfaces (
                    BiosVideoPrivate->Handle,
                    &gEfiVgaMiniPortProtocolGuid,
                    &BiosVideoPrivate->VgaMiniPort,
                    NULL
                    );
  }
  if (EFI_ERROR (Status)) {
    pBS->OpenProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           (VOID **) &PciIo,
           This->DriverBindingHandle,
           Handle,
           EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
           );
    return Status;
  }

  //
  // Set the 80x25 Text VGA Mode
  //
  Regs.H.AH = 0x00;
  Regs.H.AL = 0x03;
  BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

  Regs.H.AH = 0x11;
  Regs.H.AL = 0x14;
  Regs.H.BL = 0;
  BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

  //
  // Do not disable IO/memory decode since that would prevent legacy ROM from working
  //

  //
  // Release all allocated resources
  //
  BiosVideoDeviceReleaseResource (BiosVideoPrivate);
//*** AMI PORTING BEGIN ***//
// The protocols are uninstalled.
// Update global variables used by the CsmVideoPolicy protocol implementation.
  VgaControllerHandle = NULL;
//*** AMI PORTING END *****//
  return EFI_SUCCESS;
}

VOID
BiosVideoDeviceReleaseResource (
  BIOS_VIDEO_DEV  *BiosVideoPrivate
  )
/*++
Routing Description:

  Release resources of an video child device before stopping it.

Arguments:

  BiosVideoPrivate  -  Video child device private data structure

Returns:

    NONE
    
---*/
{
  if (BiosVideoPrivate == NULL) {
    return ;
  }

  //
  // Release all the resourses occupied by the BIOS_VIDEO_DEV
  //
  
  //
  // Free VGA Frame Buffer
  //
  if (BiosVideoPrivate->VgaFrameBuffer != NULL) {
    pBS->FreePool (BiosVideoPrivate->VgaFrameBuffer);
    BiosVideoPrivate->VgaFrameBuffer = NULL;
  }
  //
  // Free VBE Frame Buffer
  //
  if (BiosVideoPrivate->VbeFrameBuffer != NULL) {
    pBS->FreePool (BiosVideoPrivate->VbeFrameBuffer);
    BiosVideoPrivate->VbeFrameBuffer = NULL;
  }
  //
  // Free line buffer
  //
  if (BiosVideoPrivate->LineBuffer != NULL) {
    pBS->FreePool (BiosVideoPrivate->LineBuffer);
    BiosVideoPrivate->LineBuffer = NULL;
  }
  //
  // Free mode data
  //
  if (BiosVideoPrivate->ModeData != NULL) {
    pBS->FreePool (BiosVideoPrivate->ModeData);
    BiosVideoPrivate->ModeData = NULL;
  }
  //
  // Free memory allocated below 1MB
  //
  if (BiosVideoPrivate->PagesBelow1MB != 0) {
    pBS->FreePages (BiosVideoPrivate->PagesBelow1MB, BiosVideoPrivate->NumberOfPagesBelow1MB);
  }

  if (BiosVideoPrivate->VbeSaveRestorePages != 0) {
    pBS->FreePages (BiosVideoPrivate->VbeSaveRestoreBuffer, BiosVideoPrivate->VbeSaveRestorePages);
  }
  //
  // Free graphics output protocol occupied resource
  //
  if (BiosVideoPrivate->GraphicsOutput.Mode != NULL) {
    if (BiosVideoPrivate->GraphicsOutput.Mode->Info != NULL) {
        pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode->Info);
        BiosVideoPrivate->GraphicsOutput.Mode->Info = NULL;
    }
    pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode);
    BiosVideoPrivate->GraphicsOutput.Mode =  NULL;
  }
  //
  // Free EDID discovered protocol occupied resource
  //
  if (BiosVideoPrivate->EdidDiscovered.Edid != NULL) {
    pBS->FreePool (BiosVideoPrivate->EdidDiscovered.Edid);
    BiosVideoPrivate->EdidDiscovered.Edid = NULL;
  }
  //
  // Free EDID active protocol occupied resource
  //
  if (BiosVideoPrivate->EdidActive.Edid != NULL) {
    pBS->FreePool (BiosVideoPrivate->EdidActive.Edid);
    BiosVideoPrivate->EdidActive.Edid = NULL;
  }

  if (BiosVideoPrivate->DevicePath!= NULL) {
    pBS->FreePool (BiosVideoPrivate->DevicePath);
    BiosVideoPrivate->DevicePath = NULL;
  }

  //
  // Close the ExitBootServices event
  //
//*** AMI PORTING BEGIN ***//
//ExitBootServicesEvent is not used
/*
  if (BiosVideoPrivate->ExitBootServicesEvent != NULL) {
    pBS->CloseEvent (BiosVideoPrivate->ExitBootServicesEvent);
  }
*/
//*** AMI PORTING END *****//

  pBS->FreePool (BiosVideoPrivate);

  return ;
}

//*** AMI PORTING BEGIN ***//
#if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//
STATIC
UINT32
CalculateEdidKey (
  VESA_BIOS_EXTENSIONS_EDID_TIMING       *EdidTiming
  )
/*++

  Routine Description:

  Generate a search key for a specified timing data.

  Arguments:

  EdidTiming       - Pointer to EDID timing

  Returns:
  The 32 bit unique key for search.

--*/
{
  UINT32 Key;

  //
  // Be sure no conflicts for all standard timing defined by VESA.
  //
  Key = (EdidTiming->HorizontalResolution * 2) + EdidTiming->VerticalResolution + EdidTiming->RefreshRate;
  return Key;
}

STATIC
BOOLEAN
ParseEdidData (
  UINT8                                      *EdidBuffer
  )
/*++

  Routine Description:

  Parse the Established Timing and Standard Timing in EDID data block.

  Arguments:

  EdidBuffer       - Pointer to EDID data block
  ValidEdidTiming  - Valid EDID timing information

  Returns:
  TRUE              - The EDID data is valid.
  FALSE             - The EDID data is invalid.

--*/
{
  UINT8  CheckSum;
  UINT32 Index;
  UINT32 ValidNumber;
  UINT32 TimingBits;
  UINT8  *BufferIndex;
  UINT16 HorizontalResolution;
  UINT16 VerticalResolution;
  UINT8  AspectRatio;
  VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK *EdidDataBlock;

  EdidDataBlock = (VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK *) EdidBuffer;

  //
  // Check the checksum of EDID data
  //
  CheckSum = 0;
  for (Index = 0; Index < VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE; Index ++) {
    CheckSum = CheckSum + EdidBuffer[Index];
  }
  if (CheckSum != 0) {
    TRACE((TRACE_BIOS_VIDEO, "EDID checksum is invalid, EDID will be ignored.\n"));
    return FALSE;
  }

  ValidNumber = 0;
    //
    // Established timing data
    //
    TimingBits = EdidDataBlock->EstablishedTimings[0] |
                 (EdidDataBlock->EstablishedTimings[1] << 8) |
                 ((EdidDataBlock->EstablishedTimings[2] & 0x80) << 9) ;
    for (Index = 0; Index < VESA_BIOS_EXTENSIONS_EDID_ESTABLISHED_TIMING_MAX_NUMBER; Index ++) {
      if (TimingBits & 0x1) {
        SupportedResolutions[Index] = mEstablishedEdidTiming[Index].VerticalResolution |
                                      (mEstablishedEdidTiming[Index].HorizontalResolution << 16);
        ValidNumber ++;
      }
      TimingBits = TimingBits >> 1;
    }

    //
    // If no Established timing data, read the standard timing data
    //
    BufferIndex = &EdidDataBlock->StandardTimingIdentification[0];
    for (Index = 0; Index < 8; Index ++) {
      if ((BufferIndex[0] != 0x1) && (BufferIndex[1] != 0x1)){
        //
        // A valid Standard Timing
        //
        HorizontalResolution = BufferIndex[0] * 8 + 248;
        AspectRatio = BufferIndex[1] >> 6;
        switch (AspectRatio) {
          case 0:
            VerticalResolution = HorizontalResolution / 16 * 10;
            break;
          case 1:
            VerticalResolution = HorizontalResolution / 4 * 3;
            break;
          case 2:
            VerticalResolution = HorizontalResolution / 5 * 4;
            break;
          case 3:
            VerticalResolution = HorizontalResolution / 16 * 9;
            break;
          default:
            VerticalResolution = HorizontalResolution / 4 * 3;
            break;
        }

        SupportedResolutions[Index + 17] = VerticalResolution |(HorizontalResolution << 16);
        ValidNumber ++;
      }
      BufferIndex += 2;
    }

    if ( (EdidDataBlock->FeatureSupport & 2) == 2) {
      // Preferred timing mode is indicated in the first detailed timing block
      for (Index = 0; Index < 4; Index ++) {
        if ((EdidDataBlock->DetailedTimingDescriptions[Index*18 + 0] |
                EdidDataBlock->DetailedTimingDescriptions[Index*18 + 1]) &&
            (EdidDataBlock->DetailedTimingDescriptions[Index*18 + 2] |
                (EdidDataBlock->DetailedTimingDescriptions[Index*18 + 4] & 0xf0))) {

          SupportedResolutions[Index + 25] = 
                (EdidDataBlock->DetailedTimingDescriptions[Index*18 + 5] | ((UINT16)(EdidDataBlock->DetailedTimingDescriptions[Index*18 + 7] & 0xF0) << 4))
                | ((UINT32)(EdidDataBlock->DetailedTimingDescriptions[Index*18 + 2] | ((UINT16)(EdidDataBlock->DetailedTimingDescriptions[Index*18 + 4] & 0xF0) << 4)) << 16);

TRACE((TRACE_BIOS_VIDEO, "EDID Detailed timing[%d]: inserted resolution 0x%x (%dx%d)\n", Index, SupportedResolutions[Index + 25],
 EdidDataBlock->DetailedTimingDescriptions[Index*18 + 2] | ((EdidDataBlock->DetailedTimingDescriptions[Index*18 + 4] & 0xF0) << 4),
 EdidDataBlock->DetailedTimingDescriptions[Index*18 + 5] | ((EdidDataBlock->DetailedTimingDescriptions[Index*18 + 7] & 0xF0) << 4)
));

          ValidNumber ++;
        }
      }
    }

//*** AMI PORTING BEGIN ***//
// Bug fix. The function was returning TRUE even when no valid timings found.
//  return TRUE;
  return (ValidNumber != 0);
//*** AMI PORTING END ***//
}

STATIC
BOOLEAN
SearchEdidTiming (
    UINT32 ResolutionKey
  )
/*++

  Routine Description:

  Search a specified Timing in all the valid EDID timings.

  Arguments:

  ValidEdidTiming  - All valid EDID timing information.
  EdidTiming       - The Timing to search for.

  Returns:

  TRUE  - Found.
  FALSE - Not found.

--*/
{
  UINT32 Index;
  for (Index = 0; Index < 29; Index ++) {
    if (ResolutionKey == SupportedResolutions[Index]) {
      return TRUE;
    }
  }

  return FALSE;
}

//*** AMI PORTING BEGIN ***//
#if CSM_VGA_64BITBAR_WORKAROUND
BOOLEAN CheckAbove4g(PCI_TYPE00	*PciConfSpace){
	UINTN	i;
//---------------------
	for(i=0; i<PCI_MAX_BAR-1;i++){
		//check  all six bars if
		if((PciConfSpace->Device.Bar[i] & 0x04) && ( PciConfSpace->Device.Bar[i+1]!=0) ) return TRUE;
	}
	return FALSE;
}


EFI_STATUS Update64BitLinearBufferAddr(EFI_PCI_IO_PROTOCOL *PciIo, UINT64 *LowerAddrPart, PCI_TYPE00 *PciConfSpace){
	UINTN			i,j;
	EFI_STATUS		Status;
	ASLR_QWORD_ASD	*BarRes=NULL;
	UINT64			start, end, bar, test;
	UINT32			*pUp32;
//-----------------------------

	//pBS->SetMem(&BarRes[0], sizeof(BarRes), 0);
	//Get PciIo Bar ACPI QW Resource Descriptor and Count...

	for(i=0; i<PCI_MAX_BAR; i++){
		//Free Memory allocated for us by PciBus Driver...
		//if we got here by continue statement...
		if(BarRes!=NULL){
			pBS->FreePool(BarRes);
			BarRes=NULL;
		}

		Status = PciIo->GetBarAttributes(PciIo,(UINT8)i, NULL, &BarRes);
        if(EFI_ERROR(Status)) return Status;

    	//care only about 64 bit resources end filter out Unused BARs and NOT64bit BARs
        if(BarRes->Hdr.Name==ASLV_SR_EndTag) continue;
        if(BarRes->_GRA < 64) continue;

        start=BarRes->_MIN;
        end=BarRes->_MAX;
        test=*LowerAddrPart;

		//Free Memory allocated for us by PciBus Driver...
        if(BarRes!=NULL){
			pBS->FreePool(BarRes);
			BarRes=NULL;
		}

        //now try to match what we read from PCI Config to Res Descriptor returned by the call to PciIo...
        for(j=0, bar=0; j<PCI_MAX_BAR-1;j++){
        	if(PciConfSpace->Device.Bar[j] & 0x04){
        		//here we got 64 bit BAR....
        		bar=PciConfSpace->Device.Bar[j] & (~0xF);  //Mask read only Bar Type bits...
        		//fill Upper Part of BAR address
        		j++;
                pUp32=((UINT32*)&bar)+1;
        		*pUp32=PciConfSpace->Device.Bar[j];
                pUp32=((UINT32*)&test)+1;
        		*pUp32=PciConfSpace->Device.Bar[j];

        		//Check if we found match?
            	if(start==bar){
            		if((start <= test) && (test < end)) {
            			*LowerAddrPart=test;
            			return EFI_SUCCESS;
            		} else break;
            	}
        	}
        }
 	}
	return EFI_SUCCESS;
}
#endif
//*** AMI PORTING END *****//

EFI_STATUS CheckForDuplicateMode (BIOS_VIDEO_DEV  *VideoDev, UINTN ModeNumber)
{
    UINT16 Xres = VideoDev->VbeModeInformationBlock->XResolution;
    UINT16 Yres = VideoDev->VbeModeInformationBlock->YResolution;
    UINTN i;
    BIOS_VIDEO_MODE_DATA *ModeData = VideoDev->ModeData;

    // walk through the list of published modes, see if there is a match
    for (i = 0; i < ModeNumber; i++)
    {
        if (ModeData->HorizontalResolution == Xres
            && ModeData->VerticalResolution == Yres)
        {
            return EFI_SUCCESS;
        }
        ModeData++;
    }
    return EFI_NOT_FOUND;
}

EFI_STATUS
BiosVideoCheckForVbe (
  IN OUT BIOS_VIDEO_DEV  *BiosVideoPrivate,
//*** AMI PORTING BEGIN ***//
  PCI_TYPE00			 *PciConfSpace
//*** AMI PORTING END *****//
  )
/*++

  Routine Description:

  Check for VBE device

  Arguments:

  BiosVideoPrivate - Pointer to BIOS_VIDEO_DEV structure

  Returns:

  EFI_SUCCESS - VBE device found

--*/
{
  EFI_STATUS                             Status;
  EFI_IA32_REGISTER_SET                  Regs;
  UINT16                                 *ModeNumberPtr;
  BOOLEAN                                ModeFound;
  BOOLEAN                                EdidFound;
  BOOLEAN                                EdidMatch;
  BIOS_VIDEO_MODE_DATA                   *ModeBuffer;
  BIOS_VIDEO_MODE_DATA                   *CurrentModeData;
  UINTN                                  PreferMode;
  UINTN                                  ModeNumber;
  UINT32 ResolutionKey;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE      *GraphicsOutputMode;
  INT32                                  i;
  EFI_EDID_OVERRIDE_PROTOCOL             *EdidOverride;
  UINT32                                 EdidAttributes;
  BOOLEAN                                EdidOverrideFound;
  UINTN                                  EdidOverrideDataSize;
  UINT8                                  *EdidOverrideDataBlock;
  UINTN                                  EdidActiveDataSize;
  UINT8                                  *EdidActiveDataBlock;
//*** AMI PORTING BEGIN ***//
#if CSM_VGA_64BITBAR_WORKAROUND
  BOOLEAN 								 Above4g;
#endif
//*** AMI PORTING END *****//

  EdidOverrideFound     = FALSE;
  EdidOverrideDataBlock = NULL;
  EdidActiveDataSize    = 0;
  EdidActiveDataBlock   = NULL;

//*** AMI PORTING BEGIN ***//
#if CSM_VGA_64BITBAR_WORKAROUND
  Above4g=CheckAbove4g(PciConfSpace);
#endif
//*** AMI PORTING END *****//

  //
  // Allocate buffer under 1MB for VBE data structures
  //
  BiosVideoPrivate->NumberOfPagesBelow1MB = EFI_SIZE_TO_PAGES (
                                              sizeof (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK) +
                                              sizeof (VESA_BIOS_EXTENSIONS_MODE_INFORMATION_BLOCK) +
                                              sizeof (VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK) +
                                              sizeof (VESA_BIOS_EXTENSIONS_CRTC_INFORMATION_BLOCK)
                                              );

  BiosVideoPrivate->PagesBelow1MB = 0x00100000 - 1;

  Status = pBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  BiosVideoPrivate->NumberOfPagesBelow1MB,
                  &BiosVideoPrivate->PagesBelow1MB
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Fill in the Graphics Output Protocol
  //
  BiosVideoPrivate->GraphicsOutput.QueryMode = BiosVideoGraphicsOutputQueryMode;
  BiosVideoPrivate->GraphicsOutput.SetMode = BiosVideoGraphicsOutputSetMode;
  BiosVideoPrivate->GraphicsOutput.Blt     = BiosVideoGraphicsOutputVbeBlt;
  BiosVideoPrivate->GraphicsOutput.Mode = NULL;

  //
  // Fill in the VBE related data structures
  //
  BiosVideoPrivate->VbeInformationBlock = (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK *) (UINTN) (BiosVideoPrivate->PagesBelow1MB);
  BiosVideoPrivate->VbeModeInformationBlock = (VESA_BIOS_EXTENSIONS_MODE_INFORMATION_BLOCK *) (BiosVideoPrivate->VbeInformationBlock + 1);
  BiosVideoPrivate->VbeEdidDataBlock = (VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK *) (BiosVideoPrivate->VbeModeInformationBlock + 1);
  BiosVideoPrivate->VbeCrtcInformationBlock = (VESA_BIOS_EXTENSIONS_CRTC_INFORMATION_BLOCK *) (BiosVideoPrivate->VbeModeInformationBlock + 1);
  BiosVideoPrivate->VbeSaveRestorePages   = 0;
  BiosVideoPrivate->VbeSaveRestoreBuffer  = 0;

  //
  // Test to see if the Video Adapter is compliant with VBE 3.0
  //
  pBS->SetMem (&Regs, sizeof (Regs), 0);
  Regs.X.AX = VESA_BIOS_EXTENSIONS_RETURN_CONTROLLER_INFORMATION;
  pBS->SetMem (BiosVideoPrivate->VbeInformationBlock, sizeof (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK), 0);
  BiosVideoPrivate->VbeInformationBlock->VESASignature  = VESA_BIOS_EXTENSIONS_VBE2_SIGNATURE;
  Regs.X.ES = EFI_SEGMENT ((UINTN) BiosVideoPrivate->VbeInformationBlock);
  Regs.X.DI = EFI_OFFSET ((UINTN) BiosVideoPrivate->VbeInformationBlock);

  BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

  Status = EFI_DEVICE_ERROR;

  //
  // See if the VESA call succeeded
  //
  if (Regs.X.AX != VESA_BIOS_EXTENSIONS_STATUS_SUCCESS) {
    return Status;
  }
  //
  // Check for 'VESA' signature
  //
  if (BiosVideoPrivate->VbeInformationBlock->VESASignature != VESA_BIOS_EXTENSIONS_VESA_SIGNATURE) {
    return Status;
  }
  //
  // Check to see if this is VBE 2.0 or higher
  //
  if (BiosVideoPrivate->VbeInformationBlock->VESAVersion < VESA_BIOS_EXTENSIONS_VERSION_2_0) {
    return Status;
  }

  EdidFound            = FALSE;
  EdidAttributes       = 0xff;
  EdidOverrideDataSize = 0;

  //
  // Check if EDID Override protocol is installed by platform.
  //
  Status = pBS->LocateProtocol (
                   &gEfiEdidOverrideProtocolGuid,
                   NULL,
                   (VOID **) &EdidOverride
                   );

  if (!EFI_ERROR (Status)) {
    //
    // Allocate double size of VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE to avoid overflow
    //
    Status = pBS->AllocatePool (
                    EfiBootServicesData,
                    VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE * 2,
                    &EdidOverrideDataBlock
                    );

    if (EFI_ERROR(Status)) {
      goto Done;
    }

    Status = EdidOverride->GetEdid (
                             EdidOverride,
                             BiosVideoPrivate->Handle,
                             &EdidAttributes,
                             &EdidOverrideDataSize,
                             (UINT8 **) &EdidOverrideDataBlock
                             );
    if (!EFI_ERROR (Status)  &&
//*** AMI PORTING BEGIN ***//
//         EdidAttributes == 0 &&
//*** AMI PORTING END ***//
         EdidOverrideDataSize != 0) {
      //
      // Succeeded to get EDID Override Data
      //
      TRACE((TRACE_BIOS_VIDEO, "EDID override protocol found: data size %x, attribute %x\n", EdidOverrideDataSize, EdidAttributes));
      EdidOverrideFound = TRUE;
    }
  }


  // "EdidFound" is forcibly FALSE,
  // because some SSUs(Server Switch Unit) return invalid response.
  if (!EdidOverrideFound || EdidAttributes == EFI_EDID_OVERRIDE_DONT_OVERRIDE) {
    //
    // If EDID Override data doesn't exist or EFI_EDID_OVERRIDE_DONT_OVERRIDE returned,
    // read EDID information through INT10 call and fill in EdidDiscovered structure
    //
    pBS->SetMem (&Regs, sizeof (Regs), 0);
    Regs.X.AX = VESA_BIOS_EXTENSIONS_EDID;
    Regs.X.BX = 1;
    Regs.X.CX = 0;
    Regs.X.DX = 0;
    Regs.X.ES = EFI_SEGMENT ((UINTN) BiosVideoPrivate->VbeEdidDataBlock);
    Regs.X.DI = EFI_OFFSET ((UINTN) BiosVideoPrivate->VbeEdidDataBlock);
  
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);
    if (Regs.X.AX == VESA_BIOS_EXTENSIONS_STATUS_SUCCESS) {
  
      BiosVideoPrivate->EdidDiscovered.SizeOfEdid = VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE;
      Status = pBS->AllocatePool (
                      EfiBootServicesData,
                      VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE,
                      &BiosVideoPrivate->EdidDiscovered.Edid
                      );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      pBS->CopyMem (
             BiosVideoPrivate->EdidDiscovered.Edid,
             BiosVideoPrivate->VbeEdidDataBlock,
             VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE
             );

      EdidFound = TRUE;
    }
  }

  //
  // Set up ActiveEdid data pointer and size
  //
  if (EdidFound) {
    EdidActiveDataSize  = VESA_BIOS_EXTENSIONS_EDID_BLOCK_SIZE;
    EdidActiveDataBlock = BiosVideoPrivate->EdidDiscovered.Edid;
  } else if (EdidOverrideFound) {
    EdidActiveDataSize  = EdidOverrideDataSize;
    EdidActiveDataBlock = EdidOverrideDataBlock;
    EdidFound = TRUE;
  }

  if (EdidFound) {
    //
    // Parse EDID data structure to retrieve modes supported by monitor
    //
    if (ParseEdidData ((UINT8 *) EdidActiveDataBlock) == TRUE) {
      //
      // Copy EDID Override Data to EDID Active Data
      //
      BiosVideoPrivate->EdidActive.SizeOfEdid = (UINT32)EdidActiveDataSize;
      Status = pBS->AllocatePool (
                    EfiBootServicesData,
                    EdidActiveDataSize,
                    &BiosVideoPrivate->EdidActive.Edid
                    );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      pBS->CopyMem (
             BiosVideoPrivate->EdidActive.Edid,
             EdidActiveDataBlock,
             EdidActiveDataSize
             );
    }
  } else {
    BiosVideoPrivate->EdidActive.SizeOfEdid = 0;
    BiosVideoPrivate->EdidActive.Edid = NULL;
    EdidFound = FALSE;
  }

  //
  // Walk through the mode list to see if there is at least one mode the is compatible with the EDID mode
  //
  ModeNumberPtr = (UINT16 *)
    (
      (((UINTN) BiosVideoPrivate->VbeInformationBlock->VideoModePtr & 0xffff0000) >> 12) |
        ((UINTN) BiosVideoPrivate->VbeInformationBlock->VideoModePtr & 0x0000ffff)
    );

  PreferMode = 0;
  ModeNumber = 0;

  TRACE((TRACE_BIOS_VIDEO, "VESA: fetching the list of VESA modes supported by the controller from %x\n", ModeNumberPtr));

  for (; *ModeNumberPtr != VESA_BIOS_EXTENSIONS_END_OF_MODE_LIST; ModeNumberPtr++) {
    //
    // Make sure this is a mode number defined by the VESA VBE specification.  If it isn'tm then skip this mode number.
    //
    TRACE((TRACE_BIOS_VIDEO,"VESA mode %x ", *ModeNumberPtr));

    if ((*ModeNumberPtr & VESA_BIOS_EXTENSIONS_MODE_NUMBER_VESA) == 0) {
      TRACE((TRACE_BIOS_VIDEO,".. skipping as it is not a proper VESA mode number\n"));
      continue;
    }

    //
    // Get the information about the mode
    //
    pBS->SetMem (&Regs, sizeof (Regs), 0);
    Regs.X.AX = VESA_BIOS_EXTENSIONS_RETURN_MODE_INFORMATION;
    Regs.X.CX = *ModeNumberPtr;
    pBS->SetMem (BiosVideoPrivate->VbeModeInformationBlock, sizeof (VESA_BIOS_EXTENSIONS_MODE_INFORMATION_BLOCK), 0);
    Regs.X.ES = EFI_SEGMENT ((UINTN) BiosVideoPrivate->VbeModeInformationBlock);
    Regs.X.DI = EFI_OFFSET ((UINTN) BiosVideoPrivate->VbeModeInformationBlock);

    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

    //
    // See if the call succeeded.  If it didn't, then try the next mode.
    //
    if (Regs.X.AX != VESA_BIOS_EXTENSIONS_STATUS_SUCCESS) {
      TRACE((TRACE_BIOS_VIDEO,".. skipping as we can not retrieve mode details\n"));
      continue;
    }

    TRACE((TRACE_BIOS_VIDEO, "(%dx%d)  ", BiosVideoPrivate->VbeModeInformationBlock->XResolution, BiosVideoPrivate->VbeModeInformationBlock->YResolution));

    //
    // See if the mode supported in hardware.  If it doesn't then try the next mode.
    //
    if ((BiosVideoPrivate->VbeModeInformationBlock->ModeAttributes & VESA_BIOS_EXTENSIONS_MODE_ATTRIBUTE_HARDWARE) == 0) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as the mode is not supported in hardware...\n"));
      continue;
    }

    //
    // See if the mode supports color.  If it doesn't then try the next mode.
    //
    if ((BiosVideoPrivate->VbeModeInformationBlock->ModeAttributes & VESA_BIOS_EXTENSIONS_MODE_ATTRIBUTE_COLOR) == 0) {
      TRACE((TRACE_BIOS_VIDEO,"is invalid, skipping...\n"));
      continue;
    }
    //
    // See if the mode supports graphics.  If it doesn't then try the next mode.
    //
    if ((BiosVideoPrivate->VbeModeInformationBlock->ModeAttributes & VESA_BIOS_EXTENSIONS_MODE_ATTRIBUTE_GRAPHICS) == 0) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as the mode is not graphical...\n"));
      continue;
    }
    //
    // See if the mode supports a linear frame buffer.  If it doesn't then try the next mode.
    //
    if ((BiosVideoPrivate->VbeModeInformationBlock->ModeAttributes & VESA_BIOS_EXTENSIONS_MODE_ATTRIBUTE_LINEAR_FRAME_BUFFER) == 0) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as the mode has not linear frame buffer...\n"));
      continue;
    }
    //
    // See if the mode supports 32 bit color.  If it doesn't then try the next mode.
    // 32 bit mode can be implemented by 24 Bits Per Pixel. Also make sure the
    // number of bits per pixel is a multiple of 8 or more than 32 bits per pixel
    //
    if (BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel < 24) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as BPP (%d) is less than 24...\n", BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel));
      continue;
    }

    if (BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel > 32) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as BPP (%d) is more than 32...\n", BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel));
      continue;
    }

    if ((BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel % 8) != 0) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as BPP (%d) modulo 8 is non-zero...\n", BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel));
      continue;
    }
    //
    // See if the physical base pointer for the linear mode is valid.  If it isn't then try the next mode.
    //
    if (BiosVideoPrivate->VbeModeInformationBlock->PhysBasePtr == 0) {
      TRACE((TRACE_BIOS_VIDEO,"skipping as PhysBasePtr is zero...\n"));
      continue;
    }
    EdidMatch = FALSE;
    if (EdidFound) {
      //
      // EDID exist, check whether this mode match with any mode in EDID
      //
      ResolutionKey = BiosVideoPrivate->VbeModeInformationBlock->YResolution | 
                      (BiosVideoPrivate->VbeModeInformationBlock->XResolution << 16);

      if (SearchEdidTiming (ResolutionKey) == TRUE) {
        EdidMatch = TRUE;
        TRACE((TRACE_BIOS_VIDEO, "EDID match found.\n"));
      }
    }

    //
    // Select a reasonable mode to be set for current display mode
    //
    ModeFound = FALSE;

    for(i = 0; i < MaxTextMode; i++)
    {
        if(TextModeArray[i].VideoCol == BiosVideoPrivate->VbeModeInformationBlock->XResolution && 
           TextModeArray[i].VideoRow == BiosVideoPrivate->VbeModeInformationBlock->YResolution)
        {
            ModeFound = TRUE;
            TRACE((TRACE_BIOS_VIDEO, "MODE match found (%d).\n", i));
        }
    }
    if ((!EdidMatch) && (!ModeFound)) {
      //
      // When EDID exist and if the timing matches with VESA add it.
      // And also add three possible resolutions, i.e. 1024x768, 800x600, 640x480
      //
      TRACE((TRACE_BIOS_VIDEO, "neither EDID nor MODE match is found.\n"));
      continue;
    }

    if (CheckForDuplicateMode(BiosVideoPrivate, ModeNumber) == EFI_SUCCESS)
    {
        TRACE((TRACE_BIOS_VIDEO, "skipping as the same resolution (%dx%d) is already available\n",
            BiosVideoPrivate->VbeModeInformationBlock->XResolution,
            BiosVideoPrivate->VbeModeInformationBlock->YResolution));
        continue;
    }

    //
    // Add mode to the list of available modes
    //
    ModeNumber ++;
    Status = pBS->AllocatePool (
                    EfiBootServicesData,
                    ModeNumber * sizeof (BIOS_VIDEO_MODE_DATA),
                    (VOID **) &ModeBuffer
                    );
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (ModeNumber > 1) {
      pBS->CopyMem (
            ModeBuffer,
            BiosVideoPrivate->ModeData,
            (ModeNumber - 1) * sizeof (BIOS_VIDEO_MODE_DATA)
            );
    }

    if (BiosVideoPrivate->ModeData != NULL) {
      pBS->FreePool (BiosVideoPrivate->ModeData);
      BiosVideoPrivate->ModeData = NULL;
    }

    CurrentModeData = &ModeBuffer[ModeNumber - 1];
    CurrentModeData->VbeModeNumber = *ModeNumberPtr;
    if (BiosVideoPrivate->VbeInformationBlock->VESAVersion >= VESA_BIOS_EXTENSIONS_VERSION_3_0) {
      CurrentModeData->BytesPerScanLine = BiosVideoPrivate->VbeModeInformationBlock->LinBytesPerScanLine;
      CurrentModeData->Red.Position = BiosVideoPrivate->VbeModeInformationBlock->LinRedFieldPosition;
      CurrentModeData->Red.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->LinRedMaskSize) - 1);
      CurrentModeData->Blue.Position = BiosVideoPrivate->VbeModeInformationBlock->LinBlueFieldPosition;
      CurrentModeData->Blue.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->LinBlueMaskSize) - 1);
      CurrentModeData->Green.Position = BiosVideoPrivate->VbeModeInformationBlock->LinGreenFieldPosition;
      CurrentModeData->Green.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->LinGreenMaskSize) - 1);
      CurrentModeData->Reserved.Position = BiosVideoPrivate->VbeModeInformationBlock->LinRsvdFieldPosition;
      CurrentModeData->Reserved.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->LinRsvdMaskSize) - 1);
    } else {
      CurrentModeData->BytesPerScanLine = BiosVideoPrivate->VbeModeInformationBlock->BytesPerScanLine;
      CurrentModeData->Red.Position = BiosVideoPrivate->VbeModeInformationBlock->RedFieldPosition;
      CurrentModeData->Red.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->RedMaskSize) - 1);
      CurrentModeData->Blue.Position = BiosVideoPrivate->VbeModeInformationBlock->BlueFieldPosition;
      CurrentModeData->Blue.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->BlueMaskSize) - 1);
      CurrentModeData->Green.Position = BiosVideoPrivate->VbeModeInformationBlock->GreenFieldPosition;
      CurrentModeData->Green.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->GreenMaskSize) - 1);
      CurrentModeData->Reserved.Position = BiosVideoPrivate->VbeModeInformationBlock->RsvdFieldPosition;
      CurrentModeData->Reserved.Mask = (UINT8) ((1 << BiosVideoPrivate->VbeModeInformationBlock->RsvdMaskSize) - 1);
    }
//*** AMI PORTING BEGIN ***//
//Bug fix: The original logic did not work for a modes with 24-bit pixels.
// For a 24-bit pixels, the PixelFormat must be set to PixelBitMask, which
// was not happening with the original "if" statement
//    if ((CurrentModeData->Red.Mask == 0xff) && (CurrentModeData->Green.Mask == 0xff) && (CurrentModeData->Blue.Mask == 0xff)) {
    if (   (BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel == 32) 
        && (CurrentModeData->Red.Mask == 0xff) && (CurrentModeData->Green.Mask == 0xff) && (CurrentModeData->Blue.Mask == 0xff)
    ) {
//*** AMI PORTING END *****//
      if ((CurrentModeData->Red.Position == 0) && (CurrentModeData->Green.Position == 8)) {
        CurrentModeData->PixelFormat = PixelRedGreenBlueReserved8BitPerColor;
      } else if ((CurrentModeData->Blue.Position == 0) && (CurrentModeData->Green.Position == 8)) {
        CurrentModeData->PixelFormat = PixelBlueGreenRedReserved8BitPerColor;
      }
    } else {
      CurrentModeData->PixelFormat = PixelBitMask;
    }
    CurrentModeData->PixelBitMask.RedMask = ((UINT32) CurrentModeData->Red.Mask) << CurrentModeData->Red.Position;
    CurrentModeData->PixelBitMask.GreenMask = ((UINT32) CurrentModeData->Green.Mask) << CurrentModeData->Green.Position;
    CurrentModeData->PixelBitMask.BlueMask = ((UINT32) CurrentModeData->Blue.Mask) << CurrentModeData->Blue.Position;
    CurrentModeData->PixelBitMask.ReservedMask = ((UINT32) CurrentModeData->Reserved.Mask) << CurrentModeData->Reserved.Position;

//*** AMI PORTING BEGIN ***//
#if CSM_VGA_64BITBAR_WORKAROUND
    if(Above4g){
   	    UINT64			 PhysBasePtr4g=BiosVideoPrivate->VbeModeInformationBlock->PhysBasePtr;
   	//------------------------------------
   	    Status=Update64BitLinearBufferAddr(BiosVideoPrivate->PciIo, &PhysBasePtr4g, PciConfSpace);
    	if (EFI_ERROR (Status)) {
    	      goto Done;
   	    }

    	CurrentModeData->LinearFrameBuffer = (VOID *) (UINTN)PhysBasePtr4g;
    }else
#endif
    {
    	CurrentModeData->LinearFrameBuffer = (VOID *) (UINTN)BiosVideoPrivate->VbeModeInformationBlock->PhysBasePtr;
    }
	//CurrentModeData->LinearFrameBuffer = (VOID *) (UINTN)BiosVideoPrivate->VbeModeInformationBlock->PhysBasePtr;

//*** AMI PORTING END *****//
    CurrentModeData->HorizontalResolution = BiosVideoPrivate->VbeModeInformationBlock->XResolution;
    CurrentModeData->VerticalResolution = BiosVideoPrivate->VbeModeInformationBlock->YResolution;

    CurrentModeData->BitsPerPixel  = BiosVideoPrivate->VbeModeInformationBlock->BitsPerPixel;

//*** AMI PORTING BEGIN ***//
// UEFI SCT 2.3.1 Summer Summit version reports that framebuffersize mismatch in query mode of SCT test 
// Calculated the framebuffersize based for each mode configuration
    if(CurrentModeData->PixelFormat == PixelRedGreenBlueReserved8BitPerColor || 
        CurrentModeData->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) { 
        CurrentModeData->FrameBufferSize=((CurrentModeData->BytesPerScanLine * 8) / CurrentModeData->BitsPerPixel) * 
                                         CurrentModeData->VerticalResolution *
                                         sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    } else {
        CurrentModeData->FrameBufferSize = BiosVideoPrivate->VbeInformationBlock->TotalMemory * 64 * 1024;
    }
//*** AMI PORTING END *****//

    CurrentModeData->RefreshRate   = 60;

    BiosVideoPrivate->ModeData = ModeBuffer;
  }
  //
  // Check to see if we found any modes that are compatible with GRAPHICS OUTPUT
  //
  if (ModeNumber == 0) {
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

  //
  // Allocate buffer for Graphics Output Protocol mode information
  //
  Status = pBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE),
                (VOID **) &BiosVideoPrivate->GraphicsOutput.Mode
                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  GraphicsOutputMode = BiosVideoPrivate->GraphicsOutput.Mode;
  Status = pBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION),
                (VOID **) &GraphicsOutputMode->Info
                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  GraphicsOutputMode->MaxMode = (UINT32) ModeNumber;
  //
  // Current mode is still unknown, set it to an invalid mode.
  //
  GraphicsOutputMode->Mode = GRAPHICS_OUTPUT_INVALIDE_MODE_NUMBER;
  TRACE((TRACE_BIOS_VIDEO, "Total number of GOP modes: %d\n", ModeNumber));
  //
  // Find the best mode to initialize
  //
  AllocateTheBuffers(BiosVideoPrivate);
  
  Status = BiosVideoGraphicsOutputSetMode (&BiosVideoPrivate->GraphicsOutput, (UINT32) PreferMode);
  if (EFI_ERROR (Status)) {
    for (PreferMode = 0; PreferMode < ModeNumber; PreferMode ++) {
      Status = BiosVideoGraphicsOutputSetMode (
                &BiosVideoPrivate->GraphicsOutput,
                (UINT32) PreferMode
                );
      if (!EFI_ERROR (Status)) {
        break;
      }
    }
    if (PreferMode == ModeNumber) {
      //
      // None mode is set successfully.
      //
      goto Done;
    }
  }

Done:
  //
  // If there was an error, then free the mode structure
  //
  if (EFI_ERROR (Status)) {
    if (BiosVideoPrivate->ModeData != NULL) {
      pBS->FreePool (BiosVideoPrivate->ModeData);
      BiosVideoPrivate->ModeData = NULL;
    }
    if (BiosVideoPrivate->GraphicsOutput.Mode != NULL) {
      if (BiosVideoPrivate->GraphicsOutput.Mode->Info != NULL) {
        pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode->Info);
        BiosVideoPrivate->GraphicsOutput.Mode->Info = NULL;
      }
      pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode);
      BiosVideoPrivate->GraphicsOutput.Mode = NULL;
    }
  }

  return Status;
}

//*** AMI PORTING BEGIN ***//
#endif//if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//
//*** AMI PORTING BEGIN ***//
#if INT10_VGA_GO_SUPPORT==1
//*** AMI PORTING END *****//
EFI_STATUS
EFIAPI
BiosVideoCheckForVga (
  IN OUT BIOS_VIDEO_DEV  *BiosVideoPrivate
  )
/*++

  Routine Description:

    Check for VGA device

  Arguments:

    BiosVideoPrivate - Pointer to BIOS_VIDEO_DEV structure

  Returns:

    EFI_SUCCESS - Standard VGA device found

--*/
{
  EFI_STATUS            Status;
  BIOS_VIDEO_MODE_DATA  *ModeBuffer;
  
  //
  // Fill in the Graphics Output Protocol
  //
  BiosVideoPrivate->GraphicsOutput.QueryMode = BiosVideoGraphicsOutputQueryMode;
  BiosVideoPrivate->GraphicsOutput.SetMode = BiosVideoGraphicsOutputSetMode;
  BiosVideoPrivate->GraphicsOutput.Blt     = BiosVideoGraphicsOutputVgaBlt;

  //
  // Allocate buffer for Graphics Output Protocol mode information
  //
  Status = pBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE),
                (VOID **) &BiosVideoPrivate->GraphicsOutput.Mode
                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = pBS->AllocatePool (
                EfiBootServicesData,
                sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION),
                (VOID **) &BiosVideoPrivate->GraphicsOutput.Mode->Info
                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Add mode to the list of available modes
  //
  BiosVideoPrivate->GraphicsOutput.Mode->MaxMode = 1;

  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (BIOS_VIDEO_MODE_DATA),
                  (VOID **) &ModeBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  ModeBuffer->VbeModeNumber         = 0x0012;
  ModeBuffer->BytesPerScanLine      = 640;
  ModeBuffer->LinearFrameBuffer     = (VOID *) (UINTN) (0xa0000);
  ModeBuffer->FrameBufferSize       = 0;
  ModeBuffer->HorizontalResolution  = 640;
  ModeBuffer->VerticalResolution    = 480;
  ModeBuffer->RefreshRate           = 60;
  ModeBuffer->PixelFormat           = PixelBltOnly;
//*** AMI PORTING BEGIN ***//
//Bug fix: initialize BitsPerPixel
  ModeBuffer->BitsPerPixel          = 4;
//*** AMI PORTING END *****//

  BiosVideoPrivate->ModeData = ModeBuffer;

  //
  // Test to see if the Video Adapter support the 640x480 16 color mode
  //
  BiosVideoPrivate->GraphicsOutput.Mode->Mode = GRAPHICS_OUTPUT_INVALIDE_MODE_NUMBER;
  Status = BiosVideoGraphicsOutputSetMode (&BiosVideoPrivate->GraphicsOutput, 0);

Done:
  //
  // If there was an error, then free the mode structure
  //
  if (EFI_ERROR (Status)) {
    if (BiosVideoPrivate->ModeData != NULL) {
      pBS->FreePool (BiosVideoPrivate->ModeData);
      BiosVideoPrivate->ModeData = NULL;
    }
    if (BiosVideoPrivate->GraphicsOutput.Mode != NULL) {
      if (BiosVideoPrivate->GraphicsOutput.Mode->Info != NULL) {
        pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode->Info);
        BiosVideoPrivate->GraphicsOutput.Mode->Info = NULL;
      }
      pBS->FreePool (BiosVideoPrivate->GraphicsOutput.Mode);
      BiosVideoPrivate->GraphicsOutput.Mode = NULL;
    }
  }
  return Status;
}
//*** AMI PORTING BEGIN ***//
#endif//if INT10_VGA_GO_SUPPORT==1
//*** AMI PORTING END *****//

//
// Graphics Output Protocol Member Functions for VESA BIOS Extensions
//
EFI_STATUS
EFIAPI
BiosVideoGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
/*++

Routine Description:

  Graphics Output protocol interface to get video mode

  Arguments:
    This                  - Protocol instance pointer.
    ModeNumber            - The mode number to return information on.
    Info                  - Caller allocated buffer that returns information about ModeNumber.
    SizeOfInfo            - A pointer to the size, in bytes, of the Info buffer.

  Returns:
    EFI_SUCCESS           - Mode information returned.
    EFI_DEVICE_ERROR      - A hardware error occurred trying to retrieve the video mode.
    EFI_NOT_STARTED       - Video display is not initialized. Call SetMode ()
    EFI_INVALID_PARAMETER - One of the input args was NULL.

--*/
{
  BIOS_VIDEO_DEV        *BiosVideoPrivate;
  EFI_STATUS            Status;
  BIOS_VIDEO_MODE_DATA  *ModeData;

  BiosVideoPrivate = BIOS_VIDEO_DEV_FROM_GRAPHICS_OUTPUT_THIS (This);

  if (BiosVideoPrivate->HardwareNeedsStarting) {
    /*ReportStatusCodeWithDevicePath (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,
      0,
      &gEfiCallerIdGuid,
      BiosVideoPrivate->DevicePath
      );*/
    return EFI_NOT_STARTED;
  }

  if (This == NULL || Info == NULL || SizeOfInfo == NULL || ModeNumber >= This->Mode->MaxMode) {
    return EFI_INVALID_PARAMETER;
  }

  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION),
                  Info
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *SizeOfInfo = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  ModeData = &BiosVideoPrivate->ModeData[ModeNumber];
  (*Info)->Version = 0;
  (*Info)->HorizontalResolution = ModeData->HorizontalResolution;
  (*Info)->VerticalResolution   = ModeData->VerticalResolution;
  (*Info)->PixelFormat = ModeData->PixelFormat;
  (*Info)->PixelInformation = ModeData->PixelBitMask;

  (*Info)->PixelsPerScanLine =  (ModeData->BytesPerScanLine * 8) / ModeData->BitsPerPixel;

  return EFI_SUCCESS;
}

//*** AMI PORTING BEGIN ***//
// The AllocateTheBuffers function below and certain modifications of 
// the BiosVideoGraphicsOutputVbeBlt nd BiosVideoVgaMiniPortSetMode functions 
// are made to address the issues reaised in EIP 35682.
//
// The BLT function (BiosVideoGraphicsOutputVbeBlt) uses several memory buffers.
// Default implementation allocates the buffers in the SetMode(BiosVideoVgaMiniPortSetMode)
// function. We are changing implementation to allocate the buffers right in the BLT function
// before the first use. The code that releases the buffers is still in the SetMode function.
//
// This is a workaround for the "UEFI Windows 7 Startup Repair" bug.
// When Startup Repair option is selected, it crashes with the 0xc0000225 error prior 
// to the call to ExitBootServices.
// The problem is caused by the Windows inability to handle memory map changes.
// The memory map changes when Windows calls Gop->SetMode and our implementation of the 
// SetMode(BiosVideoGraphicsOutputVbeBlt) allocates the buffers.
// This workaround moves buffer allocation to the BLT function (BiosVideoGraphicsOutputVbeBlt), 
// which Windows never calls and therefore memory map never changes.
EFI_STATUS AllocateTheBuffers(BIOS_VIDEO_DEV *BiosVideoPrivate){
  EFI_STATUS Status;
  BIOS_VIDEO_MODE_DATA *ModeData;
  UINT16 MaxBytesPerScanLine = 0;
  UINT32 MaxVerticalResolution = 0;
  UINT32 CurrentMode = (BiosVideoPrivate->GraphicsOutput).Mode->Mode;
  UINT32 MaxMode = (BiosVideoPrivate->GraphicsOutput).Mode->MaxMode;
  UINT32 Index;

  TRACE((TRACE_BIOS_VIDEO, "UefiBiosVideo AllocateTheBuffers()\n"));
  for(Index = 0; Index < MaxMode; Index++) {
    ModeData = &BiosVideoPrivate->ModeData[Index];

    if (MaxBytesPerScanLine < ModeData->BytesPerScanLine) {
      MaxBytesPerScanLine = ModeData->BytesPerScanLine;
    }
    if (MaxVerticalResolution < ModeData->VerticalResolution) {
      MaxVerticalResolution = ModeData->VerticalResolution;
    }

    TRACE((TRACE_BIOS_VIDEO, "VbeModeNumber: 0x%x\n", ModeData->VbeModeNumber));
    TRACE((TRACE_BIOS_VIDEO, "BytesPerScanLine: 0x%x\n", \
      ModeData->BytesPerScanLine));
    TRACE((TRACE_BIOS_VIDEO, "HorizontalResolution: 0x%x\n", \
      ModeData->HorizontalResolution));
    TRACE((TRACE_BIOS_VIDEO, "VerticalResolution: 0x%x\n", \
      ModeData->VerticalResolution));
  }

  TRACE((TRACE_BIOS_VIDEO, "MaxBytesPerScanLine: 0x%x\n", MaxBytesPerScanLine));
  TRACE((TRACE_BIOS_VIDEO, "MaxVerticalResolution: 0x%x\n", \
    MaxVerticalResolution));

  ModeData = &BiosVideoPrivate->ModeData[CurrentMode];

  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  MaxBytesPerScanLine,
                  &BiosVideoPrivate->LineBuffer
                  );
  if (EFI_ERROR (Status)) {
    BiosVideoPrivate->LineBuffer=NULL;
    return Status;
  }
  MemSet(BiosVideoPrivate->LineBuffer, MaxBytesPerScanLine, 0);

  //
  // Allocate a working buffer for BLT operations to the VGA frame buffer
  //
  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  4 * 480 * 80,
                  &BiosVideoPrivate->VgaFrameBuffer
                  );
  if (EFI_ERROR (Status)) {
    pBS->FreePool (BiosVideoPrivate->LineBuffer);
    BiosVideoPrivate->LineBuffer=NULL;
    return Status;
  }
  MemSet(BiosVideoPrivate->VgaFrameBuffer, 4 * 480 * 80, 0);

  //
  // Allocate a working buffer for BLT operations to the VBE frame buffer
  //
  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  MaxBytesPerScanLine * MaxVerticalResolution,
                  &BiosVideoPrivate->VbeFrameBuffer
                  );
  if (EFI_ERROR (Status)) {
    pBS->FreePool (BiosVideoPrivate->LineBuffer);
    BiosVideoPrivate->LineBuffer=NULL;
    return Status;
  }
  MemSet(
    BiosVideoPrivate->VbeFrameBuffer, 
    MaxBytesPerScanLine * MaxVerticalResolution, 
    0
  );
/*
  //
  // Initialize the state of the VbeFrameBuffer
  //
  if (ModeData->VbeModeNumber >= 0x100) {
    pBS->CopyMem (
            BiosVideoPrivate->VbeFrameBuffer,
            ModeData->LinearFrameBuffer,
            (ModeData->BytesPerScanLine * ModeData->VerticalResolution));

  }
*/
  return Status;
}

//*** AMI PORTING END *****//

EFI_STATUS
EFIAPI
BiosVideoGraphicsOutputSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * This,
  IN  UINT32                       ModeNumber
  )
/*++

Routine Description:

  Graphics Output protocol interface to set video mode

  Arguments:
    This             - Protocol instance pointer.
    ModeNumber       - The mode number to be set.

  Returns:
    EFI_SUCCESS      - Graphics mode was changed.
    EFI_DEVICE_ERROR - The device had an error and could not complete the request.
    EFI_UNSUPPORTED  - ModeNumber is not supported by this device.

--*/
{
//*** AMI PORTING BEGIN ***//
//Workaround for EIP 35682. See comments above 
//AllocateTheBuffers function for additional details.
//  EFI_STATUS              Status;
//*** AMI PORTING END *****//
  BIOS_VIDEO_DEV          *BiosVideoPrivate;
  EFI_IA32_REGISTER_SET   Regs;
  BIOS_VIDEO_MODE_DATA    *ModeData;

  BiosVideoPrivate = BIOS_VIDEO_DEV_FROM_GRAPHICS_OUTPUT_THIS (This);

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ModeNumber >= This->Mode->MaxMode) {
    return EFI_UNSUPPORTED;
  }
/*
  if (ModeNumber == This->Mode->Mode) {
    return EFI_SUCCESS;
  }
*/
  ModeData = &BiosVideoPrivate->ModeData[ModeNumber];
/*
  if (BiosVideoPrivate->LineBuffer) {
    pBS->FreePool (BiosVideoPrivate->LineBuffer);
    BiosVideoPrivate->LineBuffer = NULL;
  }

  if (BiosVideoPrivate->VgaFrameBuffer) {
    pBS->FreePool (BiosVideoPrivate->VgaFrameBuffer);
    BiosVideoPrivate->VgaFrameBuffer = NULL;
  }

  if (BiosVideoPrivate->VbeFrameBuffer) {
    pBS->FreePool (BiosVideoPrivate->VbeFrameBuffer);
    BiosVideoPrivate->VbeFrameBuffer = NULL;
  }

  BiosVideoPrivate->LineBuffer = NULL;
*/
//*** AMI PORTING BEGIN ***//
// Workaround for EIP 35682. 
// Buffer allocation code is moved to AllocateTheBuffers function,
// which is called from the BiosVideoGraphicsOutputVbeBlt.
// See comments above AllocateTheBuffers function for additional details.
/*
  Status = pBS->AllocatePool (
                  EfiBootServicesData,
                  ModeData->BytesPerScanLine,
                  &BiosVideoPrivate->LineBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
*/
//*** AMI PORTING END *****//
  //
  // Clear all registers
  //
  pBS->SetMem (&Regs, sizeof (Regs), 0);

  if (ModeData->VbeModeNumber < 0x100) {
    //
    // Allocate a working buffer for BLT operations to the VGA frame buffer
    //
    //BiosVideoPrivate->VgaFrameBuffer = NULL;
//*** AMI PORTING BEGIN ***//
//*** AMI PORTING BEGIN ***//
// Workaround for EIP 35682. 
// Buffer allocation code is moved to AllocateTheBuffers function,
// which is called from the BiosVideoGraphicsOutputVbeBlt.
// See comments above AllocateTheBuffers function for additional details.
/*
    Status = pBS->AllocatePool (
                    EfiBootServicesData,
                    4 * 480 * 80,
                    &BiosVideoPrivate->VgaFrameBuffer
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
*/
//*** AMI PORTING END *****//
    //
    // Set VGA Mode
    //
    Regs.X.AX = ModeData->VbeModeNumber;
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

  } else {
    //
    // Allocate a working buffer for BLT operations to the VBE frame buffer
    //
    //BiosVideoPrivate->VbeFrameBuffer = NULL;
//*** AMI PORTING BEGIN ***//
//*** AMI PORTING BEGIN ***//
// Workaround for EIP 35682. 
// Buffer allocation code is moved to AllocateTheBuffers function,
// which is called from the BiosVideoGraphicsOutputVbeBlt.
// See comments above AllocateTheBuffers function for additional details.
/*
    Status = pBS->AllocatePool (
                    EfiBootServicesData,
                    ModeData->BytesPerScanLine * ModeData->VerticalResolution,
                    &BiosVideoPrivate->VbeFrameBuffer
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
*/
//*** AMI PORTING END *****//
    //
    // Set VBE mode
    //
    Regs.X.AX = VESA_BIOS_EXTENSIONS_SET_MODE;
    Regs.X.BX = (UINT16) (ModeData->VbeModeNumber | VESA_BIOS_EXTENSIONS_MODE_NUMBER_LINEAR_FRAME_BUFFER);
    pBS->SetMem (BiosVideoPrivate->VbeCrtcInformationBlock, sizeof (VESA_BIOS_EXTENSIONS_CRTC_INFORMATION_BLOCK), 0);
    Regs.X.ES = EFI_SEGMENT ((UINTN) BiosVideoPrivate->VbeCrtcInformationBlock);
    Regs.X.DI = EFI_OFFSET ((UINTN) BiosVideoPrivate->VbeCrtcInformationBlock);
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

    //
    // Check to see if the call succeeded
    //
    if (Regs.X.AX != VESA_BIOS_EXTENSIONS_STATUS_SUCCESS) {
      /*ReportStatusCodeWithDevicePath (
        EFI_ERROR_CODE | EFI_ERROR_MINOR,
        EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_OUTPUT_ERROR,
        0,
        &gEfiCallerIdGuid,
        BiosVideoPrivate->DevicePath
        );*/
      return EFI_DEVICE_ERROR;
    }
//*** AMI PORTING BEGIN ***//
    //
    // Initialize the state of the VbeFrameBuffer
    //

/*    
    Status = BiosVideoPrivate->PciIo->Mem.Read (
                                            BiosVideoPrivate->PciIo,
                                            EfiPciIoWidthUint32,
                                            EFI_PCI_IO_PASS_THROUGH_BAR,
                                            (UINT64) (UINTN) ModeData->LinearFrameBuffer,
                                            (ModeData->BytesPerScanLine * ModeData->VerticalResolution) >> 2,
                                            BiosVideoPrivate->VbeFrameBuffer
                                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
*/    
// Workaround for EIP 35682. 
// Buffer allocation and initialization code is moved to AllocateTheBuffers function,
// which is called from the BiosVideoGraphicsOutputVbeBlt.
// See comments above AllocateTheBuffers function for additional details.
/*
    pBS->CopyMem (
            BiosVideoPrivate->VbeFrameBuffer,
            ModeData->LinearFrameBuffer,
            (ModeData->BytesPerScanLine * ModeData->VerticalResolution));
*/
  }
//*** AMI PORTING END ***//

  This->Mode->Mode = ModeNumber;
  This->Mode->Info->Version = 0;
  This->Mode->Info->HorizontalResolution = ModeData->HorizontalResolution;
  This->Mode->Info->VerticalResolution = ModeData->VerticalResolution;
  This->Mode->Info->PixelFormat = ModeData->PixelFormat;
  This->Mode->Info->PixelInformation = ModeData->PixelBitMask;
  This->Mode->Info->PixelsPerScanLine =  (ModeData->BytesPerScanLine * 8) / ModeData->BitsPerPixel;
  This->Mode->SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  //
  // Frame BufferSize remain unchanged
  //
  This->Mode->FrameBufferBase = (EFI_PHYSICAL_ADDRESS)(UINTN)ModeData->LinearFrameBuffer;
  This->Mode->FrameBufferSize = ModeData->FrameBufferSize;

  // Clear both Hardware Frame buffer and local frame buffer
  MemSet(
      BiosVideoPrivate->VbeFrameBuffer, 
      ModeData->BytesPerScanLine * ModeData->VerticalResolution, 
      0
    );
  
  pBS->CopyMem ((UINT8*)ModeData->LinearFrameBuffer, 
                (UINT8*)BiosVideoPrivate->VbeFrameBuffer, 
                ModeData->BytesPerScanLine * ModeData->VerticalResolution);
  
  
  BiosVideoPrivate->HardwareNeedsStarting = FALSE;

  return EFI_SUCCESS;
}

//*** AMI PORTING BEGIN ***//
#if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//
//
// BUGBUG : Add Blt for 16 bit color, 15 bit color, and 8 bit color modes
//
EFI_STATUS
EFIAPI
BiosVideoGraphicsOutputVbeBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL       *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION  BltOperation,
  IN  UINTN                              SourceX,
  IN  UINTN                              SourceY,
  IN  UINTN                              DestinationX,
  IN  UINTN                              DestinationY,
  IN  UINTN                              Width,
  IN  UINTN                              Height,
  IN  UINTN                              Delta
  )
/*++

Routine Description:

  Graphics Output protocol instance to block transfer for VBE device

Arguments:

  This          - Pointer to Graphics Output protocol instance
  BltBuffer     - The data to transfer to screen
  BltOperation  - The operation to perform
  SourceX       - The X coordinate of the source for BltOperation
  SourceY       - The Y coordinate of the source for BltOperation
  DestinationX  - The X coordinate of the destination for BltOperation
  DestinationY  - The Y coordinate of the destination for BltOperation
  Width         - The width of a rectangle in the blt rectangle in pixels
  Height        - The height of a rectangle in the blt rectangle in pixels
  Delta         - Not used for EfiBltVideoFill and EfiBltVideoToVideo operation.
                  If a Delta of 0 is used, the entire BltBuffer will be operated on.
                  If a subrectangle of the BltBuffer is used, then Delta represents
                  the number of bytes in a row of the BltBuffer.

Returns:

  EFI_INVALID_PARAMETER - Invalid parameter passed in
  EFI_SUCCESS - Blt operation success

--*/
{
  BIOS_VIDEO_DEV                 *BiosVideoPrivate;
  BIOS_VIDEO_MODE_DATA           *Mode;
  //EFI_PCI_IO_PROTOCOL            *PciIo;
  EFI_TPL                        OriginalTPL;
  UINTN                          DstY;
  UINTN                          SrcY;
  UINTN                          DstX;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *Blt;
  VOID                           *MemAddress;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *VbeFrameBuffer;
  UINTN                          BytesPerScanLine;
  UINTN                          Index;
  UINT8                          *VbeBuffer;
  UINT8                          *VbeBuffer1;
  UINT8                          *BltUint8;
  UINT32                         VbePixelWidth;
  UINT32                         Pixel;

//*** AMI PORTING BEGIN ***//
//Bug fix: This is accessed before it's checked on NULL
  if (This == NULL || ((UINTN) BltOperation) >= EfiGraphicsOutputBltOperationMax) {
    return EFI_INVALID_PARAMETER;
  }
//*** AMI PORTING END *****//
  BiosVideoPrivate  = BIOS_VIDEO_DEV_FROM_GRAPHICS_OUTPUT_THIS (This);
//*** AMI PORTING BEGIN ***//
// Workaround for EIP 35682. 
// Allocate memory buffers during the first call of the funciton.
// See comments above AllocateTheBuffers function for additional details.
  if (BiosVideoPrivate->LineBuffer==NULL){
    EFI_STATUS Status = AllocateTheBuffers(BiosVideoPrivate);
    if (EFI_ERROR(Status)) return Status;
  }
//*** AMI PORTING END *****//
  Mode              = &BiosVideoPrivate->ModeData[This->Mode->Mode];
  //PciIo             = BiosVideoPrivate->PciIo;

  VbeFrameBuffer    = BiosVideoPrivate->VbeFrameBuffer;
  MemAddress        = Mode->LinearFrameBuffer;
  BytesPerScanLine  = Mode->BytesPerScanLine;
  VbePixelWidth     = Mode->BitsPerPixel / 8;
  BltUint8          = (UINT8 *) BltBuffer;

//*** AMI PORTING BEGIN ***//
//These parameter checks are moved to the beginning of the function
/*
  if (This == NULL || ((UINTN) BltOperation) >= EfiGraphicsOutputBltOperationMax) {
    return EFI_INVALID_PARAMETER;
  }
*/
//*** AMI PORTING END *****//

  if (Width == 0 || Height == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // We need to fill the Virtual Screen buffer with the blt data.
  // The virtual screen is upside down, as the first row is the bootom row of
  // the image.
  //
  if (BltOperation == EfiBltVideoToBltBuffer) {
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    if (SourceY + Height > Mode->VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (SourceX + Width > Mode->HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // BltBuffer to Video: Source is BltBuffer, destination is Video
    //
    if (DestinationY + Height > Mode->VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (DestinationX + Width > Mode->HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }
  //
  // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  OriginalTPL = pBS->RaiseTPL (TPL_NOTIFY);

  switch (BltOperation) {
  case EfiBltVideoToBltBuffer:
    for (SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); SrcY++, DstY++) {
      Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (BltUint8 + DstY * Delta + DestinationX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      //
      // Shuffle the packed bytes in the hardware buffer to match EFI_GRAPHICS_OUTPUT_BLT_PIXEL
      //
      VbeBuffer = ((UINT8 *) VbeFrameBuffer + (SrcY * BytesPerScanLine + SourceX * VbePixelWidth));
      for (DstX = DestinationX; DstX < (Width + DestinationX); DstX++) {
        Pixel         = *(UINT32 *) (VbeBuffer);
        Blt->Red      = (UINT8) ((Pixel >> Mode->Red.Position) & Mode->Red.Mask);
        Blt->Blue     = (UINT8) ((Pixel >> Mode->Blue.Position) & Mode->Blue.Mask);
        Blt->Green    = (UINT8) ((Pixel >> Mode->Green.Position) & Mode->Green.Mask);
        Blt->Reserved = 0;
        Blt++;
        VbeBuffer += VbePixelWidth;
      }

    }
    break;

  case EfiBltVideoToVideo:
    for (Index = 0; Index < Height; Index++) {
      if (DestinationY <= SourceY) {
        SrcY  = SourceY + Index;
        DstY  = DestinationY + Index;
      } else {
        SrcY  = SourceY + Height - Index - 1;
        DstY  = DestinationY + Height - Index - 1;
      }

      VbeBuffer   = ((UINT8 *) VbeFrameBuffer + DstY * BytesPerScanLine + DestinationX * VbePixelWidth);
      VbeBuffer1  = ((UINT8 *) VbeFrameBuffer + SrcY * BytesPerScanLine + SourceX * VbePixelWidth);

      pBS->CopyMem (
            VbeBuffer,
            VbeBuffer1,
            Width * VbePixelWidth
            );

      if (VbePixelWidth == 4) {
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with CopyMem to optimize performance
/*
        PciIo->Mem.Write (
                    PciIo,
                    EfiPciIoWidthUint32,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) ((UINTN) MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth),
                    Width,
                    VbeBuffer
                    );
*/
        pBS->CopyMem ((UINT8*)MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth, 
                      (UINT8*)VbeBuffer, 
                      Width * VbePixelWidth);
//*** AMI PORTING END *****//
      } else {
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with CopyMem to optimize performance
/*
        PciIo->Mem.Write (
                    PciIo,
                    EfiPciIoWidthUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) ((UINTN) MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth),
                    Width * VbePixelWidth,
                    VbeBuffer
                    );
*/
        pBS->CopyMem ((UINT8*)MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth, 
                      (UINT8*)VbeBuffer, 
                      Width * VbePixelWidth);
      }
//*** AMI PORTING END *****//
    }
    break;

  case EfiBltVideoFill:
    VbeBuffer = (UINT8 *) ((UINTN) VbeFrameBuffer + (DestinationY * BytesPerScanLine) + DestinationX * VbePixelWidth);
    Blt       = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) BltUint8;
    //
    // Shuffle the RGB fields in EFI_GRAPHICS_OUTPUT_BLT_PIXEL to match the hardware buffer
    //
    Pixel = ((Blt->Red & Mode->Red.Mask) << Mode->Red.Position) |
      (
        (Blt->Green & Mode->Green.Mask) <<
        Mode->Green.Position
      ) |
          ((Blt->Blue & Mode->Blue.Mask) << Mode->Blue.Position);

    for (Index = 0; Index < Width; Index++) {
      pBS->CopyMem (
            VbeBuffer,
            &Pixel,
            VbePixelWidth
            );
      VbeBuffer += VbePixelWidth;
    }

    VbeBuffer = (UINT8 *) ((UINTN) VbeFrameBuffer + (DestinationY * BytesPerScanLine) + DestinationX * VbePixelWidth);
    for (DstY = DestinationY + 1; DstY < (Height + DestinationY); DstY++) {
      pBS->CopyMem (
            (VOID *) ((UINTN) VbeFrameBuffer + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth),
            VbeBuffer,
            Width * VbePixelWidth
            );
    }

    for (DstY = DestinationY; DstY < (Height + DestinationY); DstY++) {
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with CopyMem to optimize performance
/*
      PciIo->Mem.Write (
                  PciIo,
                  EfiPciIoWidthUint8,
                  EFI_PCI_IO_PASS_THROUGH_BAR,
                  (UINT64) ((UINTN) MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth),
                  Width * VbePixelWidth,
                  VbeBuffer
                  );
*/
      pBS->CopyMem ((UINT8*)MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth, 
                    (UINT8*)VbeBuffer, 
                    Width * VbePixelWidth);
//*** AMI PORTING END *****//
    }
    break;

  case EfiBltBufferToVideo:
//*** AMI PORTING BEGIN ***//
//The original BLT loop is replaced to optimize performance by:
// replacing PciIo->Mem.Write with CopyMem and
// replacing multiplication with addition
/*
    for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {
      Blt       = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (BltUint8 + (SrcY * Delta) + (SourceX) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      VbeBuffer = ((UINT8 *) VbeFrameBuffer + (DstY * BytesPerScanLine + DestinationX * VbePixelWidth));
      for (DstX = DestinationX; DstX < (Width + DestinationX); DstX++) {
        //
        // Shuffle the RGB fields in EFI_GRAPHICS_OUTPUT_BLT_PIXEL to match the hardware buffer
        //
        Pixel = ((Blt->Red & Mode->Red.Mask) << Mode->Red.Position) |
          ((Blt->Green & Mode->Green.Mask) << Mode->Green.Position) |
            ((Blt->Blue & Mode->Blue.Mask) << Mode->Blue.Position);
        pBS->CopyMem (
              VbeBuffer,
              &Pixel,
              VbePixelWidth
              );
        Blt++;
        VbeBuffer += VbePixelWidth;
      }

      VbeBuffer = ((UINT8 *) VbeFrameBuffer + (DstY * BytesPerScanLine + DestinationX * VbePixelWidth));
      PciIo->Mem.Write (
                  PciIo,
                  EfiPciIoWidthUint8,
                  EFI_PCI_IO_PASS_THROUGH_BAR,
                  (UINT64) ((UINTN) MemAddress + (DstY * BytesPerScanLine) + DestinationX * VbePixelWidth),
                  Width * VbePixelWidth,
                  VbeBuffer
                  );
    }
*/
{
    UINTN VbeBufferOffset = DestinationY * BytesPerScanLine + DestinationX * VbePixelWidth;
    UINT8* BltBufferAddress = BltUint8 + SourceY * Delta + SourceX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    UINTN VbeLineWidth = VbePixelWidth*Width;
    UINTN BufferOffset = VbeBufferOffset;    

    for (SrcY = SourceY; SrcY < (Height + SourceY); SrcY++) {
      
      VbeBuffer = (UINT8 *) VbeFrameBuffer + BufferOffset;

      if (Mode->PixelFormat != PixelBlueGreenRedReserved8BitPerColor){
          Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)BltBufferAddress;
          for (DstX = DestinationX; DstX < (Width + DestinationX); DstX++) {
            //
            // Shuffle the RGB fields in EFI_GRAPHICS_OUTPUT_BLT_PIXEL to match the hardware buffer
            //
        Pixel = ((Blt->Red & Mode->Red.Mask) << Mode->Red.Position) |
          ((Blt->Green & Mode->Green.Mask) << Mode->Green.Position) |
            ((Blt->Blue & Mode->Blue.Mask) << Mode->Blue.Position);
            pBS->CopyMem (
                  VbeBuffer,
                  &Pixel,
                  VbePixelWidth
                  );
            Blt++;
            VbeBuffer += VbePixelWidth;
          }
          VbeBuffer = (UINT8 *) VbeFrameBuffer + BufferOffset;
      }else{
          pBS->CopyMem (VbeBuffer, BltBufferAddress, VbeLineWidth);
      }
      BufferOffset += BytesPerScanLine;
      BltBufferAddress += Delta;
    }
    BufferOffset = VbeBufferOffset;
    for (SrcY = SourceY; SrcY < (Height + SourceY); SrcY++) {
      pBS->CopyMem ((UINT8*)MemAddress + BufferOffset, 
                    (UINT8 *)VbeFrameBuffer + BufferOffset, 
                    VbeLineWidth);
      BufferOffset += BytesPerScanLine;
    }
}
//*** AMI PORTING END *****//
    break;
  }

  pBS->RestoreTPL (OriginalTPL);

  return EFI_SUCCESS;
}
//*** AMI PORTING BEGIN ***//
#endif //if INT10_VESA_GO_SUPPORT==1
//*** AMI PORTING END *****//
//*** AMI PORTING BEGIN ***//
#if INT10_VGA_GO_SUPPORT==1
//*** AMI PORTING END *****//
STATIC
VOID
WriteGraphicsController (
  IN  EFI_PCI_IO_PROTOCOL  *PciIo,
  IN  UINTN                Address,
  IN  UINTN                Data
  )
/*++

Routine Description:

  Write graphics controller registers

Arguments:

  PciIo   - Pointer to PciIo protocol instance of the controller
  Address - Register address
  Data    - Data to be written to register

Returns:

  None

--*/
{
  Address = Address | (Data << 8);
  PciIo->Io.Write (
              PciIo,
              EfiPciIoWidthUint16,
              EFI_PCI_IO_PASS_THROUGH_BAR,
              VGA_GRAPHICS_CONTROLLER_ADDRESS_REGISTER,
              1,
              &Address
              );
}

VOID
VgaReadBitPlanes (
  EFI_PCI_IO_PROTOCOL  *PciIo,
  UINT8                *HardwareBuffer,
  UINT8                *MemoryBuffer,
  UINTN                WidthInBytes,
  UINTN                Height
  )
/*++

Routine Description:

  Read the four bit plane of VGA frame buffer

Arguments:

  PciIo           - Pointer to PciIo protocol instance of the controller
  HardwareBuffer  - Hardware VGA frame buffer address
  MemoryBuffer    - Memory buffer address
  WidthInBytes    - Number of bytes in a line to read
  Height          - Height of the area to read

Returns:

  None

--*/
{
  UINTN BitPlane;
  UINTN Rows;
  UINTN FrameBufferOffset;
  UINT8 *Source;
  UINT8 *Destination;

  //
  // Program the Mode Register Write mode 0, Read mode 0
  //
  WriteGraphicsController (
    PciIo,
    VGA_GRAPHICS_CONTROLLER_MODE_REGISTER,
    VGA_GRAPHICS_CONTROLLER_READ_MODE_0 | VGA_GRAPHICS_CONTROLLER_WRITE_MODE_0
    );

  for (BitPlane = 0, FrameBufferOffset = 0;
       BitPlane < VGA_NUMBER_OF_BIT_PLANES;
       BitPlane++, FrameBufferOffset += VGA_BYTES_PER_BIT_PLANE
      ) {
    //
    // Program the Read Map Select Register to select the correct bit plane
    //
    WriteGraphicsController (
      PciIo,
      VGA_GRAPHICS_CONTROLLER_READ_MAP_SELECT_REGISTER,
      BitPlane
      );

    Source      = HardwareBuffer;
    Destination = MemoryBuffer + FrameBufferOffset;

    for (Rows = 0; Rows < Height; Rows++, Source += VGA_BYTES_PER_SCAN_LINE, Destination += VGA_BYTES_PER_SCAN_LINE) {
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with CopyMem to optimize performance
/*
      PciIo->Mem.Read (
                  PciIo,
                  EfiPciIoWidthUint8,
                  EFI_PCI_IO_PASS_THROUGH_BAR,
                  (UINT64) Source,
                  WidthInBytes,
                  (VOID *) Destination
                  );
*/
        pBS->CopyMem ((UINT8*)Source,
                      (UINT8*)Destination, 
                      WidthInBytes);
//*** AMI PORTING END *****//
    }
  }
}

VOID
VgaConvertToGraphicsOutputColor (
  UINT8                          *MemoryBuffer,
  UINTN                          X,
  UINTN                          Y,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltBuffer
  )
/*++

Routine Description:

  Internal routine to convert VGA color to Grahpics Output color

Arguments:

  MemoryBuffer  - Buffer containing VGA color
  X             - The X coordinate of pixel on screen
  Y             - The Y coordinate of pixel on screen
  BltBuffer     - Buffer to contain converted Grahpics Output color

Returns:

  None

--*/
{
  UINTN Mask;
  UINTN Bit;
  UINTN Color;

  MemoryBuffer += ((Y << 6) + (Y << 4) + (X >> 3));
  Mask = mVgaBitMaskTable[X & 0x07];
  for (Bit = 0x01, Color = 0; Bit < 0x10; Bit <<= 1, MemoryBuffer += VGA_BYTES_PER_BIT_PLANE) {
    if (*MemoryBuffer & Mask) {
      Color |= Bit;
    }
  }

  *BltBuffer = mVgaColorToGraphicsOutputColor[Color];
}

UINT8
VgaConvertColor (
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL          *BltBuffer
  )
/*++

Routine Description:

  Internal routine to convert Grahpics Output color to VGA color

Arguments:

  BltBuffer - buffer containing Grahpics Output color

Returns:

  Converted VGA color

--*/
{
  UINT8 Color;

  Color = (UINT8) ((BltBuffer->Blue >> 7) | ((BltBuffer->Green >> 6) & 0x02) | ((BltBuffer->Red >> 5) & 0x04));
//*** AMI PORTING BEGIN ***//
//  if ((BltBuffer->Red + BltBuffer->Green + BltBuffer->Blue) > 0x180) {
  if ((BltBuffer->Red >= 0xC0) || (BltBuffer->Blue >= 0xC0) || (BltBuffer->Green >= 0xC0)) {
    Color |= 0x08;
  }
  // Prevent "washing out" of single color
  if (((BltBuffer->Red >= 0xC0) && (BltBuffer->Blue == 0) && (BltBuffer->Green == 0)) || \
      ((BltBuffer->Blue >= 0xC0) && (BltBuffer->Red == 0) && (BltBuffer->Green == 0)) || \
      ((BltBuffer->Green >= 0xC0) && (BltBuffer->Blue == 0) && (BltBuffer->Red == 0))) {
    Color &= 0x07;
  }
//*** AMI PORTING END ***//

  return Color;
}

EFI_STATUS
EFIAPI
BiosVideoGraphicsOutputVgaBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL       *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION  BltOperation,
  IN  UINTN                              SourceX,
  IN  UINTN                              SourceY,
  IN  UINTN                              DestinationX,
  IN  UINTN                              DestinationY,
  IN  UINTN                              Width,
  IN  UINTN                              Height,
  IN  UINTN                              Delta
  )
/*++

Routine Description:

  Grahpics Output protocol instance to block transfer for VGA device

Arguments:

  This          - Pointer to Grahpics Output protocol instance
  BltBuffer     - The data to transfer to screen
  BltOperation  - The operation to perform
  SourceX       - The X coordinate of the source for BltOperation
  SourceY       - The Y coordinate of the source for BltOperation
  DestinationX  - The X coordinate of the destination for BltOperation
  DestinationY  - The Y coordinate of the destination for BltOperation
  Width         - The width of a rectangle in the blt rectangle in pixels
  Height        - The height of a rectangle in the blt rectangle in pixels
  Delta         - Not used for EfiBltVideoFill and EfiBltVideoToVideo operation.
                  If a Delta of 0 is used, the entire BltBuffer will be operated on.
                  If a subrectangle of the BltBuffer is used, then Delta represents
                  the number of bytes in a row of the BltBuffer.

Returns:

  EFI_INVALID_PARAMETER - Invalid parameter passed in
  EFI_SUCCESS - Blt operation success

--*/
{
  BIOS_VIDEO_DEV      *BiosVideoPrivate;
  EFI_TPL             OriginalTPL;
  UINT8               *MemAddress;
  UINTN               BytesPerScanLine;
  UINTN               BytesPerBitPlane;
  UINTN               Bit;
  UINTN               Index;
  UINTN               Index1;
  UINTN               StartAddress;
  UINTN               Bytes;
  UINTN               Offset;
  UINT8               LeftMask;
  UINT8               RightMask;
  UINTN               Address;
  UINTN               AddressFix;
  UINT8               *Address1;
  UINT8               *SourceAddress;
  UINT8               *DestinationAddress;
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINT8               Data;
  UINT8               PixelColor;
  UINT8               *VgaFrameBuffer;
  UINTN               SourceOffset;
  UINTN               SourceWidth;
  UINTN               Rows;
  UINTN               Columns;
  UINTN               X;
  UINTN               Y;
  UINTN               CurrentMode;

  BiosVideoPrivate  = BIOS_VIDEO_DEV_FROM_GRAPHICS_OUTPUT_THIS (This);

//*** AMI PORTING BEGIN ***//
// Workaround for EIP 35682. 
// Allocate memory buffers during the first call of the funciton.
// See comments above AllocateTheBuffers function for additional details.
  if (BiosVideoPrivate->LineBuffer==NULL){
    EFI_STATUS Status = AllocateTheBuffers(BiosVideoPrivate);
    if (EFI_ERROR(Status)) return Status;
  }
//*** AMI PORTING END *****//

  CurrentMode = This->Mode->Mode;
  PciIo             = BiosVideoPrivate->PciIo;
  MemAddress        = BiosVideoPrivate->ModeData[CurrentMode].LinearFrameBuffer;
  BytesPerScanLine  = BiosVideoPrivate->ModeData[CurrentMode].BytesPerScanLine >> 3;
  BytesPerBitPlane  = BytesPerScanLine * BiosVideoPrivate->ModeData[CurrentMode].VerticalResolution;
  VgaFrameBuffer    = BiosVideoPrivate->VgaFrameBuffer;

  if (This == NULL || ((UINTN) BltOperation) >= EfiGraphicsOutputBltOperationMax) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width == 0 || Height == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // We need to fill the Virtual Screen buffer with the blt data.
  // The virtual screen is upside down, as the first row is the bootom row of
  // the image.
  //
  if (BltOperation == EfiBltVideoToBltBuffer) {
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    if (SourceY + Height > BiosVideoPrivate->ModeData[CurrentMode].VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (SourceX + Width > BiosVideoPrivate->ModeData[CurrentMode].HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // BltBuffer to Video: Source is BltBuffer, destination is Video
    //
    if (DestinationY + Height > BiosVideoPrivate->ModeData[CurrentMode].VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (DestinationX + Width > BiosVideoPrivate->ModeData[CurrentMode].HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }
  //
  // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  OriginalTPL = pBS->RaiseTPL (EFI_TPL_NOTIFY);

  //
  // Compute some values we need for VGA
  //
  switch (BltOperation) {
  case EfiBltVideoToBltBuffer:

    SourceOffset  = (SourceY << 6) + (SourceY << 4) + (SourceX >> 3);
    SourceWidth   = ((SourceX + Width - 1) >> 3) - (SourceX >> 3) + 1;

    //
    // Read all the pixels in the 4 bit planes into a memory buffer that looks like the VGA buffer
    //
    VgaReadBitPlanes (
      PciIo,
      MemAddress + SourceOffset,
      VgaFrameBuffer + SourceOffset,
      SourceWidth,
      Height
      );

    //
    // Convert VGA Bit Planes to a Graphics Output 32-bit color value
    //
    BltBuffer += (DestinationY * (Delta >> 2) + DestinationX);
    for (Rows = 0, Y = SourceY; Rows < Height; Rows++, Y++, BltBuffer += (Delta >> 2)) {
      for (Columns = 0, X = SourceX; Columns < Width; Columns++, X++, BltBuffer++) {
        VgaConvertToGraphicsOutputColor (VgaFrameBuffer, X, Y, BltBuffer);
      }

      BltBuffer -= Width;
    }

    break;

  case EfiBltVideoToVideo:
    //
    // Check for an aligned Video to Video operation
    //
    if ((SourceX & 0x07) == 0x00 && (DestinationX & 0x07) == 0x00 && (Width & 0x07) == 0x00) {
      //
      // Program the Mode Register Write mode 1, Read mode 0
      //
      WriteGraphicsController (
        PciIo,
        VGA_GRAPHICS_CONTROLLER_MODE_REGISTER,
        VGA_GRAPHICS_CONTROLLER_READ_MODE_0 | VGA_GRAPHICS_CONTROLLER_WRITE_MODE_1
        );

      SourceAddress       = (UINT8 *) (MemAddress + (SourceY << 6) + (SourceY << 4) + (SourceX >> 3));
      DestinationAddress  = (UINT8 *) (MemAddress + (DestinationY << 6) + (DestinationY << 4) + (DestinationX >> 3));
      Bytes               = Width >> 3;
      for (Index = 0, Offset = 0; Index < Height; Index++, Offset += BytesPerScanLine) {
        PciIo->CopyMem (
                PciIo,
                EfiPciIoWidthUint8,
                EFI_PCI_IO_PASS_THROUGH_BAR,
                (UINT64) (DestinationAddress + Offset),
                EFI_PCI_IO_PASS_THROUGH_BAR,
                (UINT64) (SourceAddress + Offset),
                Bytes
                );
      }
    } else {
      SourceOffset  = (SourceY << 6) + (SourceY << 4) + (SourceX >> 3);
      SourceWidth   = ((SourceX + Width - 1) >> 3) - (SourceX >> 3) + 1;

      //
      // Read all the pixels in the 4 bit planes into a memory buffer that looks like the VGA buffer
      //
      VgaReadBitPlanes (
        PciIo,
        MemAddress + SourceOffset,
        VgaFrameBuffer + SourceOffset,
        SourceWidth,
        Height
        );
    }

    break;

  case EfiBltVideoFill:
    StartAddress  = (UINTN) (MemAddress + (DestinationY << 6) + (DestinationY << 4) + (DestinationX >> 3));
    Bytes         = ((DestinationX + Width - 1) >> 3) - (DestinationX >> 3);
    LeftMask      = mVgaLeftMaskTable[DestinationX & 0x07];
    RightMask     = mVgaRightMaskTable[(DestinationX + Width - 1) & 0x07];
    if (Bytes == 0) {
      LeftMask &= RightMask;
      RightMask = 0;
    }

    if (LeftMask == 0xff) {
      StartAddress--;
      Bytes++;
      LeftMask = 0;
    }

    if (RightMask == 0xff) {
      Bytes++;
      RightMask = 0;
    }

    PixelColor = VgaConvertColor (BltBuffer);

    //
    // Program the Mode Register Write mode 2, Read mode 0
    //
    WriteGraphicsController (
      PciIo,
      VGA_GRAPHICS_CONTROLLER_MODE_REGISTER,
      VGA_GRAPHICS_CONTROLLER_READ_MODE_0 | VGA_GRAPHICS_CONTROLLER_WRITE_MODE_2
      );

    //
    // Program the Data Rotate/Function Select Register to replace
    //
    WriteGraphicsController (
      PciIo,
      VGA_GRAPHICS_CONTROLLER_DATA_ROTATE_REGISTER,
      VGA_GRAPHICS_CONTROLLER_FUNCTION_REPLACE
      );

    if (LeftMask != 0) {
      //
      // Program the BitMask register with the Left column mask
      //
      WriteGraphicsController (
        PciIo,
        VGA_GRAPHICS_CONTROLLER_BIT_MASK_REGISTER,
        LeftMask
        );

      for (Index = 0, Address = StartAddress; Index < Height; Index++, Address += BytesPerScanLine) {
        //
        // Read data from the bit planes into the latches
        //
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with direct memory access to optimize performance
/*
        PciIo->Mem.Read (
                    PciIo,
                    EfiPciIoWidthUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) Address,
                    1,
                    &Data
                    );
*/
        Data = *(volatile UINT8*)(UINTN)Address;
//*** AMI PORTING END *****//
        //
        // Write the lower 4 bits of PixelColor to the bit planes in the pixels enabled by BitMask
        //
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with direct memory access to optimize performance
/*
        PciIo->Mem.Write (
                    PciIo,
                    EfiPciIoWidthUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) Address,
                    1,
                    &PixelColor
                    );
*/
        *(volatile UINT8*)(UINTN)Address = PixelColor;
//*** AMI PORTING END *****//
      }
    }

    if (Bytes > 1) {
      //
      // Program the BitMask register with the middle column mask of 0xff
      //
      WriteGraphicsController (
        PciIo,
        VGA_GRAPHICS_CONTROLLER_BIT_MASK_REGISTER,
        0xff
        );

      for (Index = 0, Address = StartAddress + 1; Index < Height; Index++, Address += BytesPerScanLine) {
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with SetMem to optimize performance
/*
        PciIo->Mem.Write (
                    PciIo,
                    EfiPciIoWidthFillUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) Address,
                    Bytes - 1,
                    &PixelColor
                    );
*/
        pBS->SetMem((VOID*)Address, Bytes - 1, PixelColor);
//*** AMI PORTING END ***//
      }
    }

    if (RightMask != 0) {
      //
      // Program the BitMask register with the Right column mask
      //
      WriteGraphicsController (
        PciIo,
        VGA_GRAPHICS_CONTROLLER_BIT_MASK_REGISTER,
        RightMask
        );

      for (Index = 0, Address = StartAddress + Bytes; Index < Height; Index++, Address += BytesPerScanLine) {
        //
        // Read data from the bit planes into the latches
        //
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with direct memory access to optimize performance
/*
        PciIo->Mem.Read (
                    PciIo,
                    EfiPciIoWidthUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) Address,
                    1,
                    &Data
                    );
*/
        Data = *(volatile UINT8*)(UINTN)Address;
//*** AMI PORTING END *****//
        //
        // Write the lower 4 bits of PixelColor to the bit planes in the pixels enabled by BitMask
        //
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with direct memory access to optimize performance
/*
        PciIo->Mem.Write (
                    PciIo,
                    EfiPciIoWidthUint8,
                    EFI_PCI_IO_PASS_THROUGH_BAR,
                    (UINT64) Address,
                    1,
                    &PixelColor
                    );
*/
        *(volatile UINT8*)(UINTN)Address = PixelColor;
//*** AMI PORTING END *****//
      }
    }
    break;

  case EfiBltBufferToVideo:
    StartAddress  = (UINTN) (MemAddress + (DestinationY << 6) + (DestinationY << 4) + (DestinationX >> 3));
    LeftMask      = mVgaBitMaskTable[DestinationX & 0x07];

    //
    // Program the Mode Register Write mode 2, Read mode 0
    //
    WriteGraphicsController (
      PciIo,
      VGA_GRAPHICS_CONTROLLER_MODE_REGISTER,
      VGA_GRAPHICS_CONTROLLER_READ_MODE_0 | VGA_GRAPHICS_CONTROLLER_WRITE_MODE_2
      );

    //
    // Program the Data Rotate/Function Select Register to replace
    //
    WriteGraphicsController (
      PciIo,
      VGA_GRAPHICS_CONTROLLER_DATA_ROTATE_REGISTER,
      VGA_GRAPHICS_CONTROLLER_FUNCTION_REPLACE
      );

    for (Index = 0, Address = StartAddress; Index < Height; Index++, Address += BytesPerScanLine) {
      for (Index1 = 0; Index1 < Width; Index1++) {
        BiosVideoPrivate->LineBuffer[Index1] = VgaConvertColor (&BltBuffer[(SourceY + Index) * (Delta >> 2) + SourceX + Index1]);
      }
      AddressFix = Address;

      for (Bit = 0; Bit < 8; Bit++) {
        //
        // Program the BitMask register with the Left column mask
        //
        WriteGraphicsController (
          PciIo,
          VGA_GRAPHICS_CONTROLLER_BIT_MASK_REGISTER,
          LeftMask
          );

        for (Index1 = Bit, Address1 = (UINT8 *) AddressFix; Index1 < Width; Index1 += 8, Address1++) {
          //
          // Read data from the bit planes into the latches
          //
//*** AMI PORTING BEGIN ***//
// Replace PciIo.Mem with direct memory access to optimize performance
/*
          PciIo->Mem.Read (
                      PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      (UINT64) Address1,
                      1,
                      &Data
                      );
*/
          Data = *(volatile UINT8*)(UINTN)Address1;

/*
          PciIo->Mem.Write (
                      PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      (UINT64) Address1,
                      1,
                      &BiosVideoPrivate->LineBuffer[Index1]
                      );
*/
          *(volatile UINT8*)(UINTN)Address1 = BiosVideoPrivate->LineBuffer[Index1];
//*** AMI PORTING END *****//
        }

        LeftMask = (UINT8) (LeftMask >> 1);
        if (LeftMask == 0) {
          LeftMask = 0x80;
          AddressFix++;
        }
      }
    }

    break;
  }

  pBS->RestoreTPL (OriginalTPL);

  return EFI_SUCCESS;
}
//*** AMI PORTING BEGIN ***//
#endif //if INT10_VGA_GO_SUPPORT==1
//*** AMI PORTING END *****//
//*** AMI PORTING BEGIN ***//
#if INT10_SIMPLE_TEXT_SUPPORT==1
//*** AMI PORTING END *****//
//
// VGA Mini Port Protocol Functions
//
EFI_STATUS
EFIAPI
BiosVideoVgaMiniPortSetMode (
  IN  EFI_VGA_MINI_PORT_PROTOCOL  *This,
  IN  UINTN                       ModeNumber
  )
/*++

Routine Description:

  VgaMiniPort protocol interface to set mode

Arguments:

  This        - Pointer to VgaMiniPort protocol instance
  ModeNumber  - The index of the mode

Returns:

  EFI_UNSUPPORTED - The requested mode is not supported
  EFI_SUCCESS - The requested mode is set successfully

--*/
{
  BIOS_VIDEO_DEV        *BiosVideoPrivate;
  EFI_IA32_REGISTER_SET Regs;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Make sure the ModeNumber is a valid value
  //
  if (ModeNumber >= This->MaxMode) {
    return EFI_UNSUPPORTED;
  }
  //
  // Get the device structure for this device
  //
  BiosVideoPrivate = BIOS_VIDEO_DEV_FROM_VGA_MINI_PORT_THIS (This);

  switch (ModeNumber) {
  case 0:
    //
    // Set the 80x25 Text VGA Mode
    //
    Regs.H.AH = 0x00;
    Regs.H.AL = 0x83;
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);

    Regs.H.AH = 0x11;
    Regs.H.AL = 0x14;
    Regs.H.BL = 0;
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);
    break;

  case 1:
    //
    // Set the 80x50 Text VGA Mode
    //
    Regs.H.AH = 0x00;
    Regs.H.AL = 0x83;
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);
    Regs.H.AH = 0x11;
    Regs.H.AL = 0x12;
    Regs.H.BL = 0;
    BiosVideoPrivate->LegacyBios->Int86 (BiosVideoPrivate->LegacyBios, 0x10, &Regs);
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
//*** AMI PORTING BEGIN ***//
#endif //if INT10_SIMPLE_TEXT_SUPPORT==1
//*** AMI PORTING END *****//
