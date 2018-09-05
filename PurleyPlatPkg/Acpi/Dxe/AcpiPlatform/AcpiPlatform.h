//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AcpiPlatform.h

--*/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <PchAccess.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Acpi/DMARemappingReportingTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/Pci.h>
#include <Library/AcpiPlatformLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioUds.h>
#include <Protocol/DmaRemap.h>
#include <Protocol/PciIo.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SerialIo.h>
#include <Protocol/LpcPolicy.h>
#include <Protocol/MpService.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/PlatformHooksLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Guid/SocketVariable.h>
#include <Guid/HobList.h>
#include <Guid/MemoryMapData.h>
#include <Protocol/PlatformType.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/RasfProtocol.h>
#include <Protocol/IioSystem.h>
#include <Cpu/CpuRegs.h>
#include <Acpi/Mcfg.h>
#include <Acpi/Hpet.h>
#include <Acpi/Srat.h>
#include <Acpi/Pcct.h>
#include <Acpi/Slit.h>
#include <Acpi/Migt.h>
#include <Acpi/Svos.h>
#include <Acpi/Msct.h>
#include <Acpi/Bdat.h>
#include "Platform.h"
#include <Acpi/AcpiVTD.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include "Register/PchRegsUsb.h"
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchInfoLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformStatusCodes.h>
#include <Library/FpgaConfigurationLib.h>
#include <Fpga.h>

#include <Protocol/SystemBoard.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/NgnAcpiSmmProtocol.h>

#define R_ICH_RCRB_HPTC                             0x3404         // High Performance Timer Configuration

extern EFI_GUID gEfiAcpiTableStorageGuid;
//
// Protocol private structure definition
//
/**

  Entry point of the ACPI platform driver.


  @param ImageHandle  -  EFI_HANDLE: A handle for the image that is initializing this driver.
  @param SystemTable  -  EFI_SYSTEM_TABLE: A pointer to the EFI system table.

  @retval EFI_SUCCESS           -  Driver initialized successfully.
  @retval EFI_LOAD_ERROR        -  Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES  -  Could not allocate needed resources.

**/
EFI_STATUS
InstallAcpiPlatform (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

/**

  Get Acpi Table Version.

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_ACPI_TABLE_VERSION
GetAcpiTableVersion (
  VOID
  );

/**

  The funtion returns Oem specific information of Acpi Platform.

  @param OemId        -  OemId returned.
  OemTableId   -  OemTableId returned.
  OemRevision  -  OemRevision returned.

  @retval EFI_STATUS  -  Status of function execution.

**/
EFI_STATUS
AcpiPlatformGetOemFields (
  OUT UINT8   *OemId,
  OUT UINT64  *OemTableId,
  OUT UINT32  *OemRevision
  );

/**

  The function returns Acpi table version.

  @param None.

  @retval EFI_ACPI_TABLE_VERSION  -  Acpi table version encoded as a UINT32.

**/
EFI_ACPI_TABLE_VERSION
AcpiPlatformGetAcpiSetting (
  VOID
  );

/**

  Entry point for Acpi platform driver.

  @param ImageHandle  -  A handle for the image that is initializing this driver.
  @param SystemTable  -  A pointer to the EFI system table.

  @retval EFI_SUCCESS           -  Driver initialized successfully.
  @retval EFI_LOAD_ERROR        -  Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES  -  Could not allocate needed resources.

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
LocateSupportProtocol (
  IN   EFI_GUID       *Protocol,
  IN   EFI_GUID       gEfiAcpiMultiTableStorageGuid,
  OUT  VOID           **Instance,
  IN   UINT32         Type
  );

EFI_STATUS
VtdAcpiTablesUpdateFn (
  VOID
  );

VOID
UpdateVtdIntRemapFlag (
  UINT8 VTdSupport,
  UINT8 InterruptRemap
  );

#endif
