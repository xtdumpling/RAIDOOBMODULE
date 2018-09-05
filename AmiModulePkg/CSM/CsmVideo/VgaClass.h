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

    VgaClass.h
    
Abstract: 
    

Revision History
--*/

#ifndef _VGA_CLASS_H
#define _VGA_CLASS_H

//*** AMI PORTING BEGIN ***//
//#include "AmiMapping.h"
//#include "Tiano.h"
//#include "EfiDriverLib.h"
//*** AMI PORTING END *****//
#include <IndustryStandard/Pci22.h>
//*** AMI PORTING BEGIN ***//
//#include "EfiStatusCode.h"
//#include "EfiCompNameSupport.h"
//#include "ComponentName.h"
//*** AMI PORTING END *****//

//
// Driver Consumed Protocol Prototypes
//
//#include EFI_PROTOCOL_DEFINITION (DevicePath)
#include <Protocol/DevicePath.h>
//#include EFI_PROTOCOL_DEFINITION (PciIo)
#include <Protocol/PciIo.h>

//*** AMI PORTING BEGIN ***//
//#include EFI_PROTOCOL_DEFINITION (VgaMiniPort)
#include <Protocol/VgaMiniPort.h>
//*** AMI PORTING END *****//
//#include EFI_PROTOCOL_DEFINITION (SimpleTextIn)
#include <Protocol/SimpleTextIn.h>

//*** AMI PORTING BEGIN ***//
//#include EFI_GUID_DEFINITION (StatusCodeCallerId)
//#include EFI_GUID_DEFINITION (StatusCodeDataTypeId)
//*** AMI PORTING END *****//

//
// Driver Produced Protocol Prototypes
//
//#include EFI_PROTOCOL_DEFINITION (DriverBinding)
#include <Protocol/DriverBinding.h>

//#include EFI_PROTOCOL_DEFINITION (SimpleTextOut)
#include <Protocol/SimpleTextOut.h>


//
// VGA specific registers
//
#define CRTC_CURSOR_START         0xA
#define CRTC_CURSOR_END           0xB

#define CRTC_CURSOR_LOCATION_HIGH 0xE
#define CRTC_CURSOR_LOCATION_LOW  0xF

#define EFI_MAX_ATTRIBUTE         0x7f

//
// VGA Class Device Structure
//
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define VGA_CLASS_DEV_SIGNATURE EFI_SIGNATURE_32 ('V', 'G', 'A', 'C')

typedef struct {
  UINTN                         Signature;
  EFI_HANDLE                    Handle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  SimpleTextOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE   SimpleTextOutputMode;
  EFI_VGA_MINI_PORT_PROTOCOL    *VgaMiniPort;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
} VGA_CLASS_DEV;

#ifndef CR
#define CR(pField, OutterType, Field, Signature) OUTTER(pField, Field, OutterType)
#endif
#define VGA_CLASS_DEV_FROM_THIS(a)  CR (a, VGA_CLASS_DEV, SimpleTextOut, VGA_CLASS_DEV_SIGNATURE)

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL  gVgaClassDriverBinding;

//
// Driver Binding Protocol functions
//
EFI_STATUS
EFIAPI
VgaClassDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
;

EFI_STATUS
EFIAPI
VgaClassDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
;

EFI_STATUS
EFIAPI
VgaClassDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
;

//
// Simple Text Output Protocol functions
//
EFI_STATUS
EFIAPI
VgaClassReset (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  BOOLEAN                      ExtendedVerification
  )
;

EFI_STATUS
EFIAPI
VgaClassOutputString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  CHAR16                       *WString
  )
;

EFI_STATUS
EFIAPI
VgaClassTestString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  CHAR16                       *WString
  )
;

EFI_STATUS
EFIAPI
VgaClassClearScreen (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
  )
;

EFI_STATUS
EFIAPI
VgaClassSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  UINTN                        Attribute
  )
;

EFI_STATUS
EFIAPI
VgaClassSetCursorPosition (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  UINTN                        Column,
  IN  UINTN                        Row
  )
;

EFI_STATUS
EFIAPI
VgaClassEnableCursor (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  BOOLEAN                      Visible
  )
;

EFI_STATUS
EFIAPI
VgaClassQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  UINTN                        ModeNumber,
  OUT UINTN                        *Columns,
  OUT UINTN                        *Rows
  )
;

EFI_STATUS
EFIAPI
VgaClassSetMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN  UINTN                        ModeNumber
  )
;

#endif
