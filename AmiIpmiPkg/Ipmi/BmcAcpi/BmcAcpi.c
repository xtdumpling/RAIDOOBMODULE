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

/** @file BmcAcpi.c
    Server Management BmcAcpi Driver. This driver sets 
    BMC ACPI device and power states

**/

//----------------------------------------------------------------------

#include "Token.h"
#include "BmcAcpi.h"

//----------------------------------------------------------------------

static EFI_IPMI_TRANSPORT       *gIpmiTransport;

/**
    Sends a command to BMC to set the current Device and power states

    @param PowerState ACPI System Power State
    @param DeviceState ACPI System Device State

    @retval VOID

**/

VOID
SetAcpiPowerState (
  IN UINT8         PowerState,
  IN UINT8         DeviceState )
{
    EFI_STATUS          Status;
    UINT8               DataSize;
    UINT8               AcpiPowerState[2];

    AcpiPowerState[0] = (SET_SYSTEM_POWER_STATE | PowerState);
    AcpiPowerState[1] = (SET_DEVICE_POWER_STATE | DeviceState);
    DataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_ACPI_POWERSTATE,
                (UINT8 *) &AcpiPowerState,
                sizeof (AcpiPowerState),
                NULL,
                &DataSize );

    return ;
}

/**
    SMI handler to set power state in BMC

    @param DispatchHandle - The unique handle assigned to this handler by
                  SmiHandlerRegister().
    @param RegisterContext - Points to an optional
                  handler context which was specified when the handler was registered.
    @param SwContext - A pointer to a collection of data in memory
                  that will be conveyed from a non-SMM environment into an SMM environment.
    @param CommBufferSize - The size of the CommBuffer.

    @return EFI_STATUS Return Status

**/

EFI_STATUS
EFIAPI
BmcAcpiSwSmiCallback (
  IN       EFI_HANDLE   DispatchHandle,
  IN CONST VOID         *RegisterContext,
  IN OUT   VOID         *SwContext,
  IN OUT   UINTN        *CommBufferSize )
{

    switch (((EFI_SMM_SW_REGISTER_CONTEXT *)RegisterContext)->SwSmiInputValue) {

        case BMC_ACPI_SWSMI_S0:
            //
            // SW ACPI SMI parameter for S0 (set S0,D0 )
            //
            SetAcpiPowerState (EFI_ACPI_POWER_STATE_S0, EFI_ACPI_DEVICE_STATE_D0);
            //  DisableFrb2Timer ();
            break;

        case BMC_ACPI_SWSMI_S1:
            //
            // SW ACPI SMI parameter for S1 (set S1,D0)
            //
            SetAcpiPowerState (EFI_ACPI_POWER_STATE_S1, EFI_ACPI_DEVICE_STATE_D0);
            break;

        case BMC_ACPI_SWSMI_S3:
            //
            // SW ACPI SMI parameter for S3 (set S3,D3)
            //
            SetAcpiPowerState (EFI_ACPI_POWER_STATE_S3, EFI_ACPI_DEVICE_STATE_D3);
            break;

        case BMC_ACPI_SWSMI_S4:
            //
            // SW ACPI SMI parameter for S4 (set S4,D3)
            //
            SetAcpiPowerState (EFI_ACPI_POWER_STATE_S4, EFI_ACPI_DEVICE_STATE_D3);
            break;

        case BMC_ACPI_SWSMI_S5:
            //
            // SW ACPI SMI parameter for S5 (set S5,D3)
            //
            SetAcpiPowerState (EFI_ACPI_POWER_STATE_S5, EFI_ACPI_DEVICE_STATE_D3);
            break;

        default:
            break;
    }
    return EFI_SUCCESS;
} 

/**
    Registers the SW SMI for Sending the Power state to BMC.

    @param ImageHandle - Handle of this driver image
    @param SystemTable - Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeBmcAcpi (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                        Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
    EFI_HANDLE                        SwHandle;
    UINT8                             Index;

    Status = gSmst->SmmLocateProtocol (
                &gEfiSmmIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Couldn't find SMM IPMI TRANSPORT protocol: %r\n", Status));
        return Status;
    }

    Status = gSmst->SmmLocateProtocol (
                &gEfiSmmSwDispatch2ProtocolGuid,
                NULL,
                (VOID **)&SwDispatch);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Couldn't find SmgSwDispatch2 protocol: %r\n", Status));
        return Status;
    }

    //
    // Register the SW SMI for Sending the Power state to BMC.
    //
    for( Index=BMC_ACPI_SWSMI; Index<BMC_ACPI_SWSMI_MAX; Index++) {
        SwContext.SwSmiInputValue = Index;
        Status = SwDispatch->Register (
                    SwDispatch,
                    BmcAcpiSwSmiCallback,
                    &SwContext,
                    &SwHandle);
        if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "Couldn't register the BMC ACPIh SW SMI %d handler.Status: %r\n",Index,Status));
            return Status;
        }
    }
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
