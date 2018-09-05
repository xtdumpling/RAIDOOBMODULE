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

/** @file SmmIpmiBmcInitialize.c
    SMM IPMI Transport Driver.

**/

//---------------------------------------------------------------------------

#include "DxeSmmIpmiBmc.h"
//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
#include <Library/AmiBufferValidationLib.h>
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability
#include <Library/SmmServicesTableLib.h>
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
#include "IpmiInitHooks.h"
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//
// Global Variable
//
EFI_IPMI_BMC_INSTANCE_DATA  *gSmmIpmiInstance = NULL;
EFI_I2C_MASTER_PROTOCOL     *gMasterTransmit;
EFI_SMBUS_HC_PROTOCOL       *gEfiSmbusHcProtocol;

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_SMM_INIT_HOOK IPMI_OEM_SMM_INIT_HOOK_LIST EndOfIpmiOemSmmInitHookListFunctions;
IPMI_OEM_SMM_INIT_HOOK* gIpmiOemSmmInitHookList[] = {IPMI_OEM_SMM_INIT_HOOK_LIST NULL};
#endif

//---------------------------------------------------------------------------

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the DXE SMM hooks of OEM.

    @param BootServices     - Pointer the set of Boot services.
    @param MmioBaseAddress  - Pointer to the MMIO base address.
    @param BaseAddressRange - Pointer to the base address range.

    @return EFI_STATUS      - Return status of the Hook.
*/

EFI_STATUS
IpmiOemSmmInitHook (
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
)
{
    EFI_STATUS  Status = EFI_DEVICE_ERROR;
    UINTN       Index;
    for (Index = 0; gIpmiOemSmmInitHookList[Index]; Index++) {
        Status = gIpmiOemSmmInitHookList[Index] (BootServices, MmioBaseAddress, BaseAddressRange);
    }
    return Status;
}
#endif

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

/**
    Software SMI callback which proxies the DXE calls to SMM IPMI protocol

    @param DispatchHandle The unique handle assigned to this handler by
                           SmiHandlerRegister().
    @param RegisterContext Points to an optional handler context which was
                          specified when the handler was registered.
    @param CommBuffer A pointer to a collection of data in memory that will
                      be conveyed from a non-SMM environment into an SMM environment.
    @param CommBufferSize The size of the CommBuffer.

    @retval EFI_STATUS Return Status

**/

EFI_STATUS
EFIAPI
SmmIpmiBmcHandler (
  IN     EFI_HANDLE         DispatchHandle,
  IN     CONST VOID         *RegisterContext,
  IN OUT VOID               *CommBuffer,
  IN OUT UINTN              *CommBufferSize )
{
    EFI_STATUS                                      Status = EFI_SUCCESS;
    SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA      *SmmIpmiBmcData;
    SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA        *SmmIpmiGetBmcStatusData;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered  status %r gSmmIpmiInstance: %x \n", __FUNCTION__, Status, gSmmIpmiInstance));

    ASSERT (CommBuffer != NULL);

    SmmIpmiBmcData = (SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA *)CommBuffer;
//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
    ///
    /// Validate if CommBuffer is of Non-SMRAM region before using it,
    /// in order to avoid SMRAM data corruption in SMI handlers.
    ///
    Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiBmcData, sizeof (SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA));
    if (EFI_ERROR (Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiBmcData Status: %r\n", __FUNCTION__, Status));
        return EFI_SUCCESS;
    }
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

    switch (SmmIpmiBmcData->FunctionNumber) {

        case SMM_IPMI_BMC_FUNCTION_SEND_COMMAND:
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "SendIpmiCommand function is called... \n"));
//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
            if ((VOID*)SmmIpmiBmcData->CommandData != NULL) {
                ///
                /// Validate if CommandData is of Non-SMRAM region before using it,
                /// in order to avoid SMRAM data corruption in SMI handlers.
                ///
                Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiBmcData->CommandData, SmmIpmiBmcData->CommandDataSize);
                if (EFI_ERROR (Status)) {
                    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiBmcData->CommandData Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }

            if ((VOID*)SmmIpmiBmcData->ResponseDataSize != NULL) {
                ///
                /// Validate if ResponseDataSize is of Non-SMRAM region before using it,
                /// in order to avoid SMRAM data corruption in SMI handlers.
                ///
                Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiBmcData->ResponseDataSize, sizeof (SmmIpmiBmcData->ResponseDataSize));
                if (EFI_ERROR (Status)) {
                    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiBmcData->ResponseDataSize Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }

                if ((VOID*)SmmIpmiBmcData->ResponseData != NULL) {
                    ///
                    /// Validate if ResponseData is of Non-SMRAM region before using it,
                    /// in order to avoid SMRAM data corruption in SMI handlers.
                    ///
                    Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiBmcData->ResponseData, (UINTN)*((UINT8 *)SmmIpmiBmcData->ResponseDataSize));
                    if (EFI_ERROR (Status)) {
                        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiBmcData->ResponseData Status: %r\n", __FUNCTION__, Status));
                        return EFI_SUCCESS;
                    }
                }
            }
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

            Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                        &gSmmIpmiInstance->IpmiTransport,
                        (UINT8)SmmIpmiBmcData->NetFunction,
                        (UINT8)SmmIpmiBmcData->Lun,
                        (UINT8)SmmIpmiBmcData->Command,
                        (UINT8*)SmmIpmiBmcData->CommandData,
                        (UINT8)SmmIpmiBmcData->CommandDataSize,
                        (UINT8*)SmmIpmiBmcData->ResponseData,
                        (UINT8*)SmmIpmiBmcData->ResponseDataSize );

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
            SmmIpmiBmcData->CommandCompletionCode = gSmmIpmiInstance->IpmiTransport.CommandCompletionCode;
            SmmIpmiBmcData->ReturnStatus = Status;
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability
            break;

        case SMM_IPMI_BMC_FUNCTION_GET_BMC_STATUS:
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "GetBmcStatus function is called... \n"));
            SmmIpmiGetBmcStatusData = (SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA *)CommBuffer;

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
            ///
            /// Validate if BmcStatus is of Non-SMRAM region before using it,
            /// in order to avoid SMRAM data corruption in SMI handlers.
            ///
            if ((VOID*)SmmIpmiGetBmcStatusData->BmcStatus != NULL) {
                Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiGetBmcStatusData->BmcStatus, sizeof (SmmIpmiGetBmcStatusData->BmcStatus));
                if (EFI_ERROR (Status)) {
                    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiGetBmcStatusData->BmcStatus Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }

            ///
            /// Validate if ComAddress is of Non-SMRAM region before using it,
            /// in order to avoid SMRAM data corruption in SMI handlers.
            ///
            if ((VOID*)SmmIpmiGetBmcStatusData->ComAddress != NULL) {
                Status = AmiValidateMemoryBuffer ((VOID*)SmmIpmiGetBmcStatusData->ComAddress, sizeof (EFI_SM_COM_ADDRESS));
                if (EFI_ERROR (Status)) {
                    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a AmiValidateMemoryBuffer for SmmIpmiGetBmcStatusData->ComAddress Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

            Status = gSmmIpmiInstance->IpmiTransport.GetBmcStatus (
                        &gSmmIpmiInstance->IpmiTransport,
                        (EFI_BMC_STATUS*)SmmIpmiGetBmcStatusData->BmcStatus,
                        (EFI_SM_COM_ADDRESS*)SmmIpmiGetBmcStatusData->ComAddress );
//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
            SmmIpmiGetBmcStatusData->ReturnStatus = Status;
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability
            break;

        default:
            ASSERT (FALSE);
            Status = EFI_UNSUPPORTED;
    }

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
//    SmmIpmiBmcData->CommandCompletionCode = gSmmIpmiInstance->IpmiTransport.CommandCompletionCode;
//    SmmIpmiBmcData->ReturnStatus = Status;
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... Status %r\n", __FUNCTION__, Status));

    return EFI_SUCCESS;
}

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

/**
    Execute the Get System Interface Capability command and update Ipmi Instance.

    @param  VOID

    @return  VOID

**/
VOID
GetSystemInterfaceCapability (
  IN VOID
)
{
    EFI_STATUS                                     Status;
    EFI_GET_SYSTEM_INTERFACE_CAPABILITY_COMMAND    GetSystemInterfaceCapabilityCmd;
    EFI_IPMI_SSIF_INTERFACE_CAPABILITY             GetSsifInterfaceCapability;
    UINT8                                          DataSize = sizeof (GetSsifInterfaceCapability);

    GetSystemInterfaceCapabilityCmd.SystemInterfaceType = 0x0; // SSIF
    GetSystemInterfaceCapabilityCmd.Reserved = 0x0;

    Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gSmmIpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES,
                (UINT8*) &GetSystemInterfaceCapabilityCmd,
                sizeof (GetSystemInterfaceCapabilityCmd),
                (UINT8*) &GetSsifInterfaceCapability,
                &DataSize);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Status %r\n", __FUNCTION__, Status));

    if (!EFI_ERROR (Status)) {
        gSmmIpmiInstance->Interface.SSIF.RwSupport = GetSsifInterfaceCapability.TransactionSupport;
        gSmmIpmiInstance->Interface.SSIF.PecSupport = GetSsifInterfaceCapability.PecSupport;
    }

}

/**
    Execute the Set Global Enable command to enable receive message queue interrupt.

    @return VOID
**/
VOID
SetGlobalEnable ()
{
    EFI_STATUS                      Status;
    EFI_BMC_GLOBAL_ENABLES          BmcGlobalEnables;
    UINT32                          ResponseDataSize = sizeof (BmcGlobalEnables);

    //
    // Get Global Enable Information.
    //
    Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gSmmIpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_BMC_GLOBAL_ENABLES,
                NULL,
                0,
                (UINT8 *) (&BmcGlobalEnables),
                (UINT8 *) &ResponseDataSize
                );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get BMC Global Enable status %r\n", Status));

    if (EFI_ERROR(Status)){
        return;
    }

    //
    // Set Smb alert pin based on ReceiveMsgQueueInterrupt bit
    //

    gSmmIpmiInstance->Interface.SSIF.SmbAlertSupport = BmcGlobalEnables.ReceiveMsgQueueInterrupt;
}

/**
    Setup and initialize the BMC for the SMM phase.
    1. In order to verify the BMC is functioning as expected, the BMC Self-test
    is performed. Updates the BMC status in Private data
    2. Installs SMM IPMI transport protocol.
    3. Registers Software SMI callback to proxy the DXE calls to SMM IPMI protocol.
    This applicable only when both DXE and SMM uses same KCS interface.
    4. Notify the DxeIpmiBmc driver that SmmIpmiBmcTransport protocol is ready.

        
    @param ImageHandle Handle of this driver image
    @param SystemTable Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeIpmiSmmPhysicalLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
    EFI_STATUS                            Status = EFI_SUCCESS;
    EFI_HANDLE                            NewHandle = NULL;
    UINT8                                 DataSize;
    UINT8                                 Index;
    IPMI_SELF_TEST_RESULT_RESPONSE        BstStatus;

    //
    // Allocate memory for IPMI Instance
    //
    gSmmIpmiInstance = AllocateRuntimeZeroPool (sizeof (EFI_IPMI_BMC_INSTANCE_DATA));
    if (gSmmIpmiInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gSmmIpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gSmmIpmiInstance: %x \n", gSmmIpmiInstance));

    //
    // Initialize IPMI Instance Data
    //
    if (IPMI_DEFAULT_HOOK_SUPPORT) {
#if IPMI_SYSTEM_INTERFACE == KCS_INTERFACE
        gSmmIpmiInstance->Interface.KCS.DataPort = IPMI_SMM_KCS_DATA_PORT;          // KCS Data Port
        gSmmIpmiInstance->Interface.KCS.CommandPort = IPMI_SMM_KCS_COMMAND_PORT;    // KCS Command Port
#elif IPMI_SYSTEM_INTERFACE == BT_INTERFACE
        gSmmIpmiInstance->Interface.BT.CtrlPort = IPMI_BT_CTRL_PORT;     // BT Control Port
        gSmmIpmiInstance->Interface.BT.ComBuffer = IPMI_BT_BUFFER_PORT;  // BT Buffer Port
        gSmmIpmiInstance->Interface.BT.IntMaskPort = IPMI_BT_INTERRUPT_MASK_PORT; // BT IntMask Port
#endif
        gSmmIpmiInstance->MmioBaseAddress = 0;
        gSmmIpmiInstance->AccessType = IPMI_IO_ACCESS;
        gSmmIpmiInstance->BaseAddressRange = 0;
    } else {
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
        gSmmIpmiInstance->AccessType = IPMI_MMIO_ACCESS;
        Status = IpmiOemSmmInitHook (gBS, &gSmmIpmiInstance->MmioBaseAddress, &gSmmIpmiInstance->BaseAddressRange);
        if (EFI_ERROR (Status)) {
            //
            // IpmiOemSmmInitHook has failed, so free the allocated memory and return Status.
            //
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemSmmInitHook is failed and Status is %r.\n", Status));
            FreePool (gSmmIpmiInstance);
            return Status;
        }
#endif
    }
#if IPMI_SYSTEM_INTERFACE == KCS_INTERFACE
    gSmmIpmiInstance->Interface.KCS.KcsRetryCounter = IPMI_SMM_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmiSendCommand;       // KCS SendCommand
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmiSendCommandEx; // KCS SendCommand Extension
#elif IPMI_SYSTEM_INTERFACE == BT_INTERFACE
    gSmmIpmiInstance->Interface.BT.BtDelay = BT_DELAY;       // BT Delay
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommand = EfiBtIpmiSendCommand;   // BT SendCommand
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiBtIpmiSendCommandEx; // BT SendCommand Extension
#elif IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE
    Status = gSmst->SmmLocateProtocol (
                &gEfiSmbusHcProtocolGuid,
                NULL,
                (VOID **)&gEfiSmbusHcProtocol);

    if (EFI_ERROR(Status)) {
        FreePool (gSmmIpmiInstance);
        return Status;
    }
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommand = EfiSsifIpmiSendCommand;   // SSIF SendCommand
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiSsifIpmiSendCommandEx; // SSIF SendCommand Extension
    gSmmIpmiInstance->Interface.SSIF.SsifRetryCounter = IPMI_SSIF_COMMAND_REQUEST_RETRY_COUNTER; // SSIF retry counter
    gSmmIpmiInstance->Interface.SSIF.PecSupport = FALSE; // SSIF PEC support
    gSmmIpmiInstance->Interface.SSIF.RwSupport = 0x0; // SSIF multi-part reads/writes support
    gSmmIpmiInstance->Interface.SSIF.SmbAlertSupport = FALSE; // Smb alert pin support
#else
    Status = gSmst->SmmLocateProtocol(
                &gEfiI2cMasterProtocolGuid,
                NULL,
                (VOID **)&gMasterTransmit);
    if(EFI_ERROR(Status)) {
        FreePool (gSmmIpmiInstance);
        return Status;
    }
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmbIpmiSendCommand;   // IPMB SendCommand
    gSmmIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmbIpmiSendCommandEx; // IPMB SendCommand Extension
#endif

    gSmmIpmiInstance->Signature     = SM_IPMI_BMC_SIGNATURE;
    gSmmIpmiInstance->SlaveAddress  = IPMI_BMC_SLAVE_ADDRESS;
    gSmmIpmiInstance->BmcStatus     = EFI_BMC_OK;
    gSmmIpmiInstance->IpmiTransport.GetBmcStatus = EfiIpmiBmcStatus;

    //
    // Check SSIF interface capability
    //
    if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) {
        GetSystemInterfaceCapability();
        SetGlobalEnable();
    }

    //
    // Get the BMC SELF TEST Results.
    //

    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {
        DataSize = sizeof (BstStatus);
        Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gSmmIpmiInstance->IpmiTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&BstStatus,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "SMM: Get Self test results status %r\n", Status));
        if (Status != EFI_NOT_READY) {
            break;
        }
    }

    //
    // If Status indicates a Device error, then the BMC is not responding, so send an error.
    //
    if (EFI_ERROR (Status)) {
        gSmmIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {

        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (BstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                gSmmIpmiInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                gSmmIpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                gSmmIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        }
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SMM BMC status %x\n", gSmmIpmiInstance->BmcStatus));

    //
    // Now install the Protocol if the BMC is not in a Hard Fail State
    //
    if (gSmmIpmiInstance->BmcStatus != EFI_BMC_HARDFAIL) {
        NewHandle = NULL;
        Status = gSmst->SmmInstallProtocolInterface (
                    &NewHandle,
                    &gEfiSmmIpmiTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSmmIpmiInstance->IpmiTransport );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "SMM gEfiSmmIpmiTransportProtocolGuid protocol status %r\n", Status));
        if (EFI_ERROR (Status)) {

            //
            // Error in installing the protocol. So free the allocated memory.
            //
            FreePool (gSmmIpmiInstance);
            return Status;
        }

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Same Interface used for both SMM and DXE phases so proxy the DXE calls to SMM
        //
        if (!EFI_ERROR(Status)) {

            //
            // Register SMM IPMI BMC SMI handler
            //
            NewHandle = NULL;
            Status = gSmst->SmiHandlerRegister (
                        SmmIpmiBmcHandler,
                        &gEfiSmmIpmiTransportProtocolGuid,
                        &NewHandle);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "gSmst->SmiHandlerRegister  status %r\n", Status));
            ASSERT_EFI_ERROR (Status);

            if ( !EFI_ERROR(Status) ) {

                //
                // Notify the DxeIpmiBmc driver that SmmIpmiBmcTransport protocol is ready
                //
                NewHandle = NULL;
                Status = gBS->InstallProtocolInterface (
                            &NewHandle,
                            &gEfiSmmIpmiTransportProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL );
                SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBS->Install: gEfiSmmIpmiTransportProtocolGuid protocol  status %r\n", Status));
                ASSERT_EFI_ERROR (Status);
            }
        }
#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        return EFI_SUCCESS;

    } else {

        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        gSmst->SmmFreePool (gSmmIpmiInstance);
        return EFI_UNSUPPORTED;
    }

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
