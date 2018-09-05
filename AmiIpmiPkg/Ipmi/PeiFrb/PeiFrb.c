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

/** @file PeiFrb.c
    Does FRB initialization for POST
    Enables FRB2 timer and reports timer expiration Events to BMC if any

**/

//------------------------------------------------------------------------

#include "PeiFrb.h"

//------------------------------------------------------------------------

//
// Creating Notify descriptor for callback
//
static EFI_PEI_NOTIFY_DESCRIPTOR gMasterBootModeCallbackNotify[] =
{
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMasterBootModePpiGuid,
    StopFrb2Timer
};

/**
    If system is in S3 or recovery state, then stop frb2 timer

    @param PeiServices Pointer to PEI service
    @param NotifyDescriptor Pointer to Notify descriptor
    @param Ppi 

    @return EFI_STATUS
    @retval EFI_SUCCESS FRB2 timer is stopped on S3 resume or recovery
                        mode.
    @retval EFI_UNSUPPORTED System is not in recovery or S3 resume mode.
    @retval EFI_ABORTED Timer is already stopped or type of FRB is not FRB2

**/

EFI_STATUS
EFIAPI
StopFrb2Timer (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi )
{
    PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi;
    EFI_STATUS              Status;
    UINT8                   FrbTimerDataSize;
    UINT8                   ResponseDataSize;
    EFI_BOOT_MODE           BootMode;

    EFI_BMC_GET_WATCHDOG_TIMER_STRUCTURE    GetWatchDogTimerData = {0};
    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE    *SetWatchdogTimerDataPtr = NULL;

    Status = (*PeiServices)->GetBootMode((CONST EFI_PEI_SERVICES**)PeiServices, &BootMode);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "GetBootMode Status: %r BootMode: %x \n", Status, BootMode));
    ASSERT_EFI_ERROR(Status);

    //
    //Checking if boot mode is S3 resume or recovery mode
    //
    if ((BootMode == BOOT_ON_S3_RESUME) || (BootMode == BOOT_IN_RECOVERY_MODE)) {

        //
        //Locate IPMI Transport protocol to send commands to BMC.
        //
        Status = (*PeiServices)->LocatePpi (
                  (CONST EFI_PEI_SERVICES**)PeiServices,
                  &gEfiPeiIpmiTransportPpiGuid,
                  0,
                  NULL,
                  (VOID **)&IpmiTransportPpi );
        if (EFI_ERROR(Status)) {
            return Status;
        }

        //
        // get FRB status to check whether timer is already stopped or not.
        //
        FrbTimerDataSize = sizeof (GetWatchDogTimerData);
        Status = IpmiTransportPpi->SendIpmiCommand (
                  IpmiTransportPpi,
                  IPMI_NETFN_APP,
                  BMC_LUN,
                  IPMI_APP_GET_WATCHDOG_TIMER,
                  NULL,
                  0,
                  (UINT8*)&GetWatchDogTimerData,
                  &FrbTimerDataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_GET_WATCHDOG_TIMER Status: %r \n", Status));
        if (EFI_ERROR(Status)) {
            return Status;
        }

        //
        // If timer is already stopped or timer type is FRB2 then return EFI_ABORTED.
        //
        if ((!GetWatchDogTimerData.TimerUse.TimerRunning) || (GetWatchDogTimerData.TimerUse.TimerUse != EfiFrb2)) {
            return EFI_ABORTED;
        } else {

        //
        // Disable timer by using set watchdog timer with timer value and DontStopTimer bit value as zero.
        // while disabling don't clear global expiration flag. Also retaining previous other values
        //
        SetWatchdogTimerDataPtr = (EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE *)&GetWatchDogTimerData;
        SetWatchdogTimerDataPtr->TimerUse.TimerRunning = 0;
        SetWatchdogTimerDataPtr->InitialCountdownValue = 0;

        FrbTimerDataSize = sizeof(EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE);
        ResponseDataSize = 0;

        Status = IpmiTransportPpi->SendIpmiCommand (
                  IpmiTransportPpi,
                  IPMI_NETFN_APP,
                  BMC_LUN,
                  IPMI_APP_SET_WATCHDOG_TIMER,
                  (UINT8*)SetWatchdogTimerDataPtr,
                  FrbTimerDataSize,
                  NULL,
                  &ResponseDataSize);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_SET_WATCHDOG_TIMER Status: %r \n", Status));
        return Status;
        } 
    }
    return EFI_UNSUPPORTED;
}

/**
    Enables FRB2 timer

    @param FileHandle Pointer to image file handle.
    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS Status return by driver
    @retval EFI_SUCCESS FRB initialization completed successfully.
    @retval EFI_NOT_FOUND Could not find required Ppis.
    @retval EFI_DEVICE_ERROR Couldn't Interact with FRB-n because of device failure.

**/

EFI_STATUS
EFIAPI
InitializePeiFrb ( 
  IN        EFI_PEI_FILE_HANDLE    FileHandle,
  IN  CONST EFI_PEI_SERVICES       **PeiServices )
{

    EFI_STATUS                              Status;
    PEI_IPMI_TRANSPORT_PPI                  *IpmiTransportPpi;
    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE    SetWatchDogTimerData = {0};
    UINT8                                   FrbTimerDataSize;
    BOOLEAN                                 FrbEnabled = TRUE; //Default is Enabled
    UINT8                                   ResponseDataSize;

    //
    //Locate IPMI Transport PPI to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiIpmiTransportPpiGuid,
                0,
                NULL,
                (VOID **)&IpmiTransportPpi );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiPeiIpmiTransportPpiGuid Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }


    //
    // Hook to disable Frb3 timer.
    //
    DisableFrb3Timer(PeiServices);

    //
    // Get Frb2 info from setup.
    //
    GetFrbSetupOptions(PeiServices, &SetWatchDogTimerData, &FrbEnabled);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " FrbEnabled: %x \n", FrbEnabled));

    //
    // Enable Frb2 timer if enabled in Setup.
    //
    if (FrbEnabled) {
        //
        // Data Byte-1
        //
        SetWatchDogTimerData.TimerUse.TimerUse = EfiFrb2;
        SetWatchDogTimerData.TimerUse.TimerRunning = 0;
        SetWatchDogTimerData.TimerUse.TimerUseExpirationFlagLog = 0;

        //
        // Data Byte-2
        //
        SetWatchDogTimerData.TimerActions.PreTimeoutIntr = NoTimeoutInterrupt;

        //
        // Data Byte-3
        //
        SetWatchDogTimerData.PretimeoutInterval = 0;

        //
        // Data Byte-4
        //
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.Frb2ExpirationFlag = 1;

        FrbTimerDataSize = sizeof (SetWatchDogTimerData);
        ResponseDataSize = 0;

        Status = IpmiTransportPpi->SendIpmiCommand (
                    IpmiTransportPpi,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_SET_WATCHDOG_TIMER,
                    (UINT8*)&SetWatchDogTimerData,
                    FrbTimerDataSize,
                    NULL,
                    &ResponseDataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_SET_WATCHDOG_TIMER Status: %r \n", Status));
        if (EFI_ERROR(Status)) {
            return Status;
        }

        //
        // Issue reset watchdog timer command to start timer.
        //
        ResponseDataSize = 0;
        Status = IpmiTransportPpi->SendIpmiCommand (
                    IpmiTransportPpi,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_RESET_WATCHDOG_TIMER,
                    NULL,
                    0,
                    NULL,
                    &ResponseDataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_RESET_WATCHDOG_TIMER Status: %r \n", Status));

        //
        // Registering Notify for Master Boot Mode ppi if frb2 timer has started.
        //
        if (!EFI_ERROR(Status)) {
            Status = (*PeiServices)->NotifyPpi ( PeiServices, gMasterBootModeCallbackNotify );
        }
        return Status;
    }

    return EFI_UNSUPPORTED;
}

/**
    Read the FRB setup options and update Set watch dog timer with it.

    @param PeiServices - Pointer to the PEI Core data Structure
    @param SetWatchDogTimerData - Pointer to Set
            Watch dog timer structure file handle.
    @param FrbEnabled - Signifies whether FRB2 is enabled in setup or not

    @return VOID

**/

VOID
GetFrbSetupOptions (
  IN CONST  EFI_PEI_SERVICES                        **PeiServices,
  IN OUT    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE    *SetWatchDogTimerData,
  IN OUT    BOOLEAN                                 *FrbEnabled )
{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI        *PeiVariable;
    SERVER_MGMT_CONFIGURATION_DATA          ServerMgmtConfiguration;
    UINTN                                   Size;
    EFI_STATUS                              Status;

    //
    // Locate the Setup configuration value.
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID **)&PeiVariable );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiPeiReadOnlyVariable2PpiGuid status %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR(Status)) {
        Size = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
        Status = PeiVariable->GetVariable (
                    PeiVariable,
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &ServerMgmtConfiguration );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " PeiVariable->GetVariable status %r \n", Status));
    }
    if (EFI_ERROR(Status)) {

        //
        // Data Byte-2 TimeoutAction
        //
        SetWatchDogTimerData->TimerActions.TimeoutAction = EfiNormalBoot;

        //
        // Data Byte-5 and Byte-6
        //
        SetWatchDogTimerData->InitialCountdownValue = 3600;  //Default 6 minutes
    } else {
        *FrbEnabled = (BOOLEAN)ServerMgmtConfiguration.Frb2Enable;

        //
        // Data Byte-2 Field
        //
        SetWatchDogTimerData->TimerActions.TimeoutAction = ServerMgmtConfiguration.Frb2TimeoutPolicy;

        //
        // Data Byte-5 and Byte-6
        //
        SetWatchDogTimerData->InitialCountdownValue = ServerMgmtConfiguration.Frb2Timeout * 10;
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
