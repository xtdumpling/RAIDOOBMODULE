//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BmcAcpi.h
    Include file for BmcAcpi Modular part

**/

#ifndef _EFI_BMCACPI_H
#define _EFI_BMCACPI_H

//----------------------------------------------------------------------

#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>

//----------------------------------------------------------------------

#define SET_SYSTEM_POWER_STATE          0x80
#define SET_DEVICE_POWER_STATE          0x80

#define EFI_BMC_ACPI_STATE              0x0d
#define EFI_ACPI_POWER_STATE_S0         0x0
#define EFI_ACPI_POWER_STATE_S1         0x1
#define EFI_ACPI_POWER_STATE_S3         0x3
#define EFI_ACPI_POWER_STATE_S4         0x4
#define EFI_ACPI_POWER_STATE_S5         0x5
#define EFI_ACPI_DEVICE_STATE_D0        0x0
#define EFI_ACPI_DEVICE_STATE_D1        0x1
#define EFI_ACPI_DEVICE_STATE_D3        0x3

//
// These values need to be modified as per the BMC_ACPI_SWSMI SDL token.
//
#define BMC_ACPI_SWSMI_S0            BMC_ACPI_SWSMI + 0x00
#define BMC_ACPI_SWSMI_S1            BMC_ACPI_SWSMI + 0x01
#define BMC_ACPI_SWSMI_S3            BMC_ACPI_SWSMI + 0x03
#define BMC_ACPI_SWSMI_S4            BMC_ACPI_SWSMI + 0x04
#define BMC_ACPI_SWSMI_S5            BMC_ACPI_SWSMI + 0x05

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
