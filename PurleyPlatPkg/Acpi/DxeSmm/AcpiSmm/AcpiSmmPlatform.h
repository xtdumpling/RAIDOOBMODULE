//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2009 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AcpiSmmPlatform.h

  Header file for SMM S3 Handler Driver.

**/

#ifndef _ACPI_SMM_DRIVER_H
#define _ACPI_SMM_DRIVER_H
//
// Include files
//
#include "SmmVariable.h"
//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmSxDispatch.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/IioUds.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Acpi/Fadt.h>
#include <Protocol/Spi.h>
#include <Library/SmmScriptLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/LockBoxLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/S3IoLib.h>
#include <Library/S3BootScriptLib.h>
#include <Guid/Acpi.h>
#include <Guid/GlobalVariable.h>
#include <Guid/MemoryOverwriteControl.h>
// APTIOV_SERVER_OVERRIDE_RC_START
// #include <Guid/PhysicalPresenceData.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include <Library/SmmServicesTableLib.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Guid/SmramMemoryReserve.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/HobLib.h>
#include <Library/SetupLib.h>
#include "Platform.h"
#include <Library/MmPciBaseLib.h>

#include <Library/PchInfoLib.h>
#include <UsraAccessApi.h>


#define CMOS_ADDR_PORT          0x70
#define CMOS_DATA_PORT          0x71

#define RTC_ADDRESS_REGISTER_D    13

#define PCI_DEVICE(Bus, Dev, Func)  \
          Bus, Dev, Func

#define PCI_REG_MASK(Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7) \
          Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7

#define PCI_DEVICE_END    0xFF

//
// Related data structures definition
//
typedef struct _EFI_ACPI_SMM_DEV {

  //
  // Parent dispatch driver returned sleep handle
  //
  EFI_HANDLE  PowerButtonHandle;
  
  EFI_HANDLE  S3SleepEntryHandle;

  EFI_HANDLE  S4SleepEntryHandle;

  EFI_HANDLE  S1SleepEntryHandle;

  EFI_HANDLE  S5SoftOffEntryHandle;

  EFI_HANDLE  EnableAcpiHandle;

  EFI_HANDLE  DisableAcpiHandle;

  EFI_HANDLE  PpCallbackHandle;

  EFI_HANDLE  MorCallbackHandle;

  //
  // PCH Power Management I/O base (aka ACPI Base)
  //
  UINT16      PchPmBase;

  UINT32      BootScriptSaved;

} EFI_ACPI_SMM_DEV;

//
// Prototypes
//
EFI_STATUS
InitPlatformAcpiSmm (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
SxSleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SX_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );

EFI_STATUS
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );

EFI_STATUS
RegisterToDispatchDriver (
  VOID
  );

EFI_STATUS
SaveRuntimeScriptTable (
  IN EFI_SMM_SYSTEM_TABLE2       *Smst
  );

EFI_STATUS
EFIAPI
BmcAcpiSwChildCallbackFunction (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );
  
EFI_STATUS
SetBmcAcpiPowerStateCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  ); 

EFI_STATUS
PowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  );
 
EFI_STATUS
RestorePchS3SwCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  );

extern EFI_GUID gPchS3CodeInLockBoxGuid;
extern EFI_GUID gPchS3ContextInLockBoxGuid;

#endif
