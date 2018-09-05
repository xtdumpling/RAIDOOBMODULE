//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    Contains the function declarations of Install Acpi table, Uninstall
    Acpi table and to Get Acpi table.
*/

#ifndef __AmiPlatformAcpiLib__H__
#define __AmiPlatformAcpiLib__H__

//---------------------------------------------------------------------------

#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IndustryStandard/Acpi10.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#ifndef EFI_ACPI_TABLE_VERSION_X
#define EFI_ACPI_TABLE_VERSION_X (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#endif

#ifndef EFI_ACPI_TABLE_VERSION_ALL
#define EFI_ACPI_TABLE_VERSION_ALL (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)
#endif

//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS AcpiLibGetAcpiTable (
    IN   EFI_ACPI_TABLE_VERSION     Version,
    IN   UINT32                     Signature,
    OUT  EFI_ACPI_SDT_HEADER        **Table,
    OUT  UINTN                      *TableKey
);

EFI_STATUS AcpiLibInstallAcpiTable (
    IN   VOID                 *AcpiTableBuffer,
    IN   UINTN                AcpiTableBufferSize,
    OUT  UINTN                *TableKey
);

EFI_STATUS AcpiLibUninstallAcpiTable (
    IN  UINTN                       TableKey
);

EFI_STATUS AcpiLibReinstallAcpiTable (
    IN      VOID                 *AcpiTableBuffer,
    IN OUT  UINTN                *TableKey
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
