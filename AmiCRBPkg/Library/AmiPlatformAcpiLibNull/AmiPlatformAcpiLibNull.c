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
    Contains the NULL functions to Install Acpi table, Uninstall Acpi table,
    Reinstall Acpi table and to Get Acpi table without locating Acpi sdt
    protocol and Acpi table protocol in driver.
*/

//---------------------------------------------------------------------------

#include <Acpi20.h>
#include <Library/AmiPlatformAcpiLib.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

//<AMI_PHDR_START>
/**

  NULL Implementation for InitAcpiLib

  @param None.


  @retval   EFI_SUCCESS

**/

EFI_STATUS InitAcpiLib (
    VOID
)
{
    return EFI_SUCCESS;

}

/**

  NULL Implementation for AcpiLibGetAcpiTable

  @param None.


  @retval   EFI_SUCCESS

**/

EFI_STATUS AcpiLibGetAcpiTable (
    IN   EFI_ACPI_TABLE_VERSION     Version,
    IN   UINT32                     Signature,
    OUT  EFI_ACPI_SDT_HEADER        **Table,
    OUT  UINTN                      *TableKey
)
{
    return EFI_SUCCESS;
}

/**

  NULL Implementation for AcpiLibInstallAcpiTable

  @param None.


  @retval   EFI_SUCCESS

**/

EFI_STATUS AcpiLibInstallAcpiTable (
    IN   VOID                 *AcpiTableBuffer,
    IN   UINTN                AcpiTableBufferSize,
    OUT  UINTN                *TableKey
)
{
    return EFI_SUCCESS;

}

/**

  NULL Implementation for AcpiLibUninstallAcpiTable

  @param None.


  @retval   EFI_SUCCESS

**/

EFI_STATUS AcpiLibUninstallAcpiTable (
    IN  UINTN                     TableKey
)
{
 
    return EFI_SUCCESS;

}

/**

  NULL Implementation for AcpiLibReinstallAcpiTable

  @param None.


  @retval   EFI_SUCCESS

**/

EFI_STATUS AcpiLibReinstallAcpiTable (
    IN      VOID                 *AcpiTableBuffer,
    IN OUT  UINTN                *TableKey
)
{

    return EFI_SUCCESS;

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
