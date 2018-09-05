//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Header file for SMM S3 Handler Driver.
   
  Copyright (c) 2009 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.               

**/

#ifndef _IIO_SMM_DRIVER_H
#define _IIO_SMM_DRIVER_H

//
// Include files
//

//#include "SmmScriptLib.h"

#include "PiDxe.h"
#include <Library/PciExpressLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
//#include <Library/EfiSmmDriverLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/IioSystem.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PlatformHooksLib.h>


//
// Related data structures definition
//
typedef struct _EFI_ACPI_SMM_DEV {
  // Parent dispatch driver returned sleep handle
  EFI_HANDLE  SleepS1Handle;
  EFI_HANDLE  SleepS3Handle;
  EFI_HANDLE  SleepS4Handle;
  EFI_HANDLE  SleepS5Handle;
} EFI_ACPI_SMM_DEV;

//
// Prototypes
//
EFI_STATUS
EfiInitializeSmmDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN OUT BOOLEAN          *InSmm
  );


EFI_STATUS
EFIAPI
IioSmmDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
RegisterDispatchDriver (
  VOID
  );

EFI_STATUS
SleepSxEntry (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST       EFI_SMM_SX_REGISTER_CONTEXT   *DispatchContext,
  IN OUT VOID    *CommBuffer           OPTIONAL,
  IN OUT UINTN   *CommBufferSize       OPTIONAL
  );

#endif
