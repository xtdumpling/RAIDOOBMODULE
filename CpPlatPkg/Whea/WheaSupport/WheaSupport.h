//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This is an implementation of the Whea Support protocol.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_SUPPORT_H_
#define _WHEA_SUPPORT_H_

//
// Statements that include other header files
//
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/WheaPlatformHooksLib.h>
#include <Library/UefiBootServicesTableLib.h>
//
// Consumed protocols
//
#include <Protocol/AcpiTable.h>
#include <Protocol/IsPlatformSupportWhea.h>


// Produced protocols
//
#include <Protocol/WheaSupport.h>

//
// Data definitions & structures
//

typedef struct _WHEA_SUPPORT_INST {
  UINTN                                         Signature;
  EFI_ACPI_WHEA_ERROR_SOURCE_TABLE              *Hest;
  EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE       *Erst;
  EFI_ACPI_WHEA_BOOT_ERROR_TABLE                *Bert;
  EFI_ACPI_WHEA_ERROR_INJECTION_TABLE           *Einj;
  UINTN                                         CurErrorStatusBlockSize;
  UINTN                                         ErrorStatusBlockSize;
  VOID                                          *ErrorStatusBlock;
  UINTN                                         ErrorLogAddressRangeSize;
  VOID                                          *ErrorLogAddressRange;
  UINTN                                         BootErrorRegionLen;
  VOID                                          *BootErrorRegion;
  BOOLEAN                                       TablesInstalled;
  UINTN                                         InstalledErrorInj;

  EFI_WHEA_SUPPORT_PROTOCOL                     WheaSupport;
} WHEA_SUPPORT_INST;

// WHEA support driver's structure definition
#define EFI_WHEA_SUPPORT_INST_SIGNATURE  SIGNATURE_32 ('W', 'H', 'E', 'A')

#define WHEA_SUPPORT_INST_FROM_THIS(a)   CR ( a,  WHEA_SUPPORT_INST,  WheaSupport, EFI_WHEA_SUPPORT_INST_SIGNATURE)


// Eswtemp
#define EFI_EVENT_SIGNAL_READY_TO_BOOT  0x0203

// Maximum buffer size for WHEA ACPI tables
#define MAX_HEST_SIZE               0x8000
#define MAX_BERT_SIZE               0x8000
#define MAX_ERST_SIZE               0x2000
#define MAX_EINJ_SIZE               0x2000

#define MAX_ERROR_STATUS_SIZE       0x4000
#define MAX_ERROR_LOG_RANGE_SIZE    0x2000
#define MAX_BOOT_ERROR_LOG_SIZE     0x8000

#define WHEA_ACPI_VERSION_SUPPORT   (EFI_ACPI_TABLE_VERSION_3_0)

/*++

Routine Description:

  Constructor for the Whea support protocol.  Initializes instance
  data.

Arguments:

  WheaSupportInstance   Instance to construct

Returns:

  EFI_SUCCESS             Instance initialized.
  EFI_OUT_OF_RESOURCES    Unable to allocate required resources.

--*/
EFI_STATUS
EFIAPI
InstallWheaSupport (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ImageHandle - GC_TODO: add argument description
  SystemTable - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

/*++

Routine Description: 

  Entry point of the Whea support driver.
  Creates and initializes an instance of the Whea Support 
  Protocol and installs it on a new handle.

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
#endif

