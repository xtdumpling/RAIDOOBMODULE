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

/** @file
    Contains the functions to Install Acpi table, Uninstall Acpi table,
    Reinstall Acpi table and to Get Acpi table without locating Acpi sdt
    protocol and Acpi table protocol in driver.
*/

//---------------------------------------------------------------------------

#include <Acpi20.h>
#include <Library/AmiPlatformAcpiLib.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

static EFI_ACPI_SDT_PROTOCOL        *gAcpiSdtProtocol = NULL;
static EFI_ACPI_TABLE_PROTOCOL      *gAcpiTable = NULL;

//---------------------------------------------------------------------------

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: InitAcpiLib
//
// Description: 
//  Locates the Acpi sdt protocol and Acpi Table protocol and returns
//  EFI_SUCCESS. Otherwise causes assert based on the Status returned by
//  LocateProtocol.
//
// Input: None.
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         When Acpi sdt protocol and Acpi Table protocol
//                           are successfully located. Otherwise causes
//                           assert based on Status.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InitAcpiLib (
    VOID
)
{
    EFI_STATUS               Status;

    //
    // Locate Acpi sdt protocol.
    //
    Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **) &gAcpiSdtProtocol);
    ASSERT_EFI_ERROR (Status);

    //
    // Locate Acpi table protocol.
    //
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &gAcpiTable);
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AcpiLibGetAcpiTable
//
// Description: 
//  Returns specific ACPI table for the Signature and Version passed as
//  inputs. Otherwise returns EFI_NOT_FOUND.
//
// Input:
//  IN    EFI_ACPI_TABLE_VERSION   Version     - Acpi table version.
//  IN    UINT32                   Signature   - Signature of the Acpi
//                                               table.
//  OUT   EFI_ACPI_SDT_HEADER      **Table     - Contains a header of Acpi
//                                               table found.
//  OUT   UINTN                    *TableKey   - Contains handle of the Acpi
//                                               table found.
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS             When requested table with Version and
//                               Signature is found.
//       EFI_NOT_READY           When Acpi Sdt Protocol is not located.
//       EFI_INVALID_PARAMETER   When Signature or Version is 0.
//       EFI_NOT_FOUND           When requested table with Version and
//                               Signature is not found.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:   This function will check each Acpi table with Version and
//          Signature and returns valid Acpi table. For DSDT, it will check
//          for version. If the Version is EFI_ACPI_TABLE_VERSION_X or
//          EFI_ACPI_TABLE_VERSION_NONE, then it will return
//          XSDT->FADT->DSDT (64 bit). If the Version is
//          EFI_ACPI_TABLE_VERSION_1_0B, then it will return
//          RSDT->FADT->DSDT (32 bit).
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AcpiLibGetAcpiTable (
    IN   EFI_ACPI_TABLE_VERSION     Version,
    IN   UINT32                     Signature,
    OUT  EFI_ACPI_SDT_HEADER        **Table,
    OUT  UINTN                      *TableKey
)
{
    EFI_STATUS               Status;
    UINTN                    Index = 0;
    EFI_ACPI_TABLE_VERSION   TableVersion;
    EFI_PHYSICAL_ADDRESS     Dsdt1 = 0;
    EFI_PHYSICAL_ADDRESS     Dsdt2 = 0;
    UINTN                    DsdtHandle = 0;
    
    if (gAcpiSdtProtocol == NULL) {
        InitAcpiLib ();
        if (gAcpiSdtProtocol == NULL) {
            return EFI_NOT_READY;
        }
    }
    if (Version == 0 && Signature == 0) {
        return EFI_INVALID_PARAMETER;
    }
    do {
        Status = gAcpiSdtProtocol->GetAcpiTable (
                                    Index,
                                    Table,
                                    &TableVersion,
                                    TableKey
                                  );
        if (EFI_ERROR(Status)) {
            DEBUG ((EFI_D_ERROR, "AmiPlatformAcpiLib: Can't find Acpi table -> %r search %d Tables\n", Status, Index));
            break;
        }
        if (Signature == (*Table)->Signature) {
            switch (Version) {
                case EFI_ACPI_TABLE_VERSION_ALL:
                    return EFI_SUCCESS;
                case EFI_ACPI_TABLE_VERSION_X:
                    if (TableVersion & EFI_ACPI_TABLE_VERSION_X) {
                        return EFI_SUCCESS;
                    }
                    break;
                default:
                    if (Version == TableVersion) {
                        return EFI_SUCCESS;
                    }
            }
        } 
        Index++;
    } while (1);
    
    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AcpiLibInstallAcpiTable
//
// Description: 
//  Uses InstallAcpiTable of Acpi table protocol to install the Acpi table
//  using AcpiTableBuffer and AcpiTableBufferSize inputs and returns
//  TableKey as unique identifier and returns status returned by
//  InstallAcpiTable.
//
// Input:
//  IN   VOID    *AcpiTableBuffer      - Data of Acpi table.
//  IN   UINTN   AcpiTableBufferSize   - Size if the AcpiTableBuffer.
//  OUT  UINTN   *TableKey             - Contains a unique identifier of
//                                       Acpi table installed.
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         When requested Acpi table is installed.
//       EFI_NOT_READY       When Acpi Table Protocol is not located.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AcpiLibInstallAcpiTable (
    IN   VOID                 *AcpiTableBuffer,
    IN   UINTN                AcpiTableBufferSize,
    OUT  UINTN                *TableKey
)
{
    if (gAcpiTable == NULL) {
        InitAcpiLib ();
        if (gAcpiTable == NULL) {
            return EFI_NOT_READY;
        }
    }
    return gAcpiTable->InstallAcpiTable (gAcpiTable, AcpiTableBuffer, AcpiTableBufferSize, TableKey);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AcpiLibUninstallAcpiTable
//
// Description: 
//  Uses UninstallAcpiTable of Acpi table protocol to uninstall the Acpi
//  table using TableKey and returns status returned by UninstallAcpiTable.
//
// Input:
//  IN   UINTN   TableKey             - Contains a unique identifier of
//                                      Acpi table to be uninstalled.
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         When requested Acpi table is uninstalled.
//       EFI_NOT_READY       When Acpi Table Protocol is not located.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AcpiLibUninstallAcpiTable (
    IN  UINTN                     TableKey
)
{
    if (gAcpiTable == NULL) {
        InitAcpiLib ();
        if (gAcpiTable == NULL) {
            return EFI_NOT_READY;
        }
    }
    return gAcpiTable->UninstallAcpiTable (gAcpiTable, TableKey);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AcpiLibReinstallAcpiTable
//
// Description:
//  Uninstalls the existing Acpi table and installs the new Acpi table using
//  AcpiLibInstallAcpiTable and AcpiLibUninstallAcpiTable. Uses
//  AcpiLibUninstallAcpiTable is to uninstall the Acpi table using TableKey.
//  Based on status returned, AcpiLibInstallAcpiTable will be called to
//  install the Acpi table using AcpiTableBuffer, length of the table and
//  returns TableKey and status.
//
// Input:
//  IN      VOID    *AcpiTableBuffer      - Data of Acpi table.
//  IN OUT  UINTN   *TableKey             - Contains a unique identifier of
//                                          Acpi table installed.
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         When requested Acpi table is uninstalled and
//                           installed.
//       EFI_NOT_READY       When inputs are NULL or 0.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AcpiLibReinstallAcpiTable (
    IN      VOID                 *AcpiTableBuffer,
    IN OUT  UINTN                *TableKey
)
{
    EFI_STATUS                    Status;
    EFI_ACPI_COMMON_HEADER        *AcpiTable;

    if (AcpiTableBuffer == NULL || *TableKey == 0) {
        return EFI_NOT_READY;
    }
    AcpiTable = (EFI_ACPI_COMMON_HEADER *)AcpiTableBuffer;
    Status = AcpiLibUninstallAcpiTable (*TableKey);
    if (!EFI_ERROR (Status)) {
        *TableKey = 0;
        Status = AcpiLibInstallAcpiTable (AcpiTable, AcpiTable->Length, TableKey);
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
