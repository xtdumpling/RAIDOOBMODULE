//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file DxeFrb.c
    Implemented FRB DXE protocol functions and register
    notifications to disable and enable Frb2 and OSwatchdog timer

**/

//----------------------------------------------------------------------

#include "DxeFrb.h"
#include <Include/ServerMgmtSetupVariable.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

extern  EFI_GUID                    gAmiTseEventBeforeBootGuid;
extern  EFI_GUID                    gAmiTseBeforeTimeOutGuid;
extern  EFI_GUID                    gAmiTseAfterTimeOutGuid;
extern  EFI_GUID                    gAmiTseSetupEnterGuid;
extern  EFI_GUID                    gAmiTsePasswordPromptEnterGuid;
extern  EFI_GUID                    gAmiTsePasswordPromptExitGuid;
extern  EFI_GUID                    gOpromStartEndProtocolGuid;
extern  EFI_GUID                    gAmiTseAfterFirstBootOptionGuid;

//
// Module globals
//
SERVER_MGMT_CONFIGURATION_DATA      gServerMgmtConfiguration;
FRB_DATA_BUFFER                     gFrbDataBuffer[EfiOsBootWdt + 1];
EFI_IPMI_TRANSPORT                  *gIpmiTransport;
EFI_SM_FRB_PROTOCOL                 *gFrbProtocol;
BOOLEAN                             gTimerCtlFlag = FALSE;

//----------------------------------------------------------------------

/**

    This routine gets the FRB timer status.

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on
    @param FrbStatData Timer data is returned in this structure

    @retval EFI_SUCCESS Data successfully retrieved and copied to structure
    @retval EFI_UNSUPPORTED Not supported for current FRB type.
    @retval EFI_INVALID_PARAMETER Invalid parameter passed in
    @retval EFI_NOT_FOUND Statistics not found.

**/

EFI_STATUS
EFIAPI
GetFrbStatus (
  IN  EFI_SM_FRB_PROTOCOL              *This,
  IN  EFI_FRB_TYPE                     FrbType,
  OUT EFI_FRB_STAT                     *FrbStatData )
{

    EFI_STATUS                                  Status;
    EFI_BMC_GET_WATCHDOG_TIMER_STRUCTURE        GetWatchDogTimerData = {0};
    UINT8                                       FrbTimerDataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x \n", __FUNCTION__, FrbType));
    if (FrbStatData == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (FrbType < EfiFrb2 && FrbType > EfiOsBootWdt) {
        return EFI_UNSUPPORTED;
    }

    FrbTimerDataSize = sizeof (GetWatchDogTimerData);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_WATCHDOG_TIMER,
                NULL,
                0,
                (UINT8*)&GetWatchDogTimerData,
                &FrbTimerDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_GET_WATCHDOG_TIMER Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
    //
    // check the timer use field is it actually same as the FrbType.
    //
    if (GetWatchDogTimerData.TimerUse.TimerUse != FrbType) {
        return EFI_NOT_FOUND;
    }
    FrbStatData->FrbTimeout = GetWatchDogTimerData.InitialCountdownValue * 100000;      //Timeout in MicroSeconds
    FrbStatData->FrbCurrentBootPolicy = GetWatchDogTimerData.TimerActions.TimeoutAction;
    FrbStatData->FrbTimeoutInterrupt = GetWatchDogTimerData.TimerActions.PreTimeoutIntr;
    FrbStatData->FrbEnabled = GetWatchDogTimerData.TimerUse.TimerRunning;           //this bit define whether FRB is running or not.

    switch (FrbType) {
        case EfiFrb2:
            FrbStatData->FrbFailed = GetWatchDogTimerData.TimerUseExpirationFlagsClear.Frb2ExpirationFlag;      //FRB timer expired on previous boot or not.
            break;
        case EfiOsBootWdt:
            FrbStatData->FrbFailed = GetWatchDogTimerData.TimerUseExpirationFlagsClear.OsLoadExpirationFlag;
            break;
        case EfiBiosPost:
            FrbStatData->FrbFailed = GetWatchDogTimerData.TimerUseExpirationFlagsClear.PostExpirationFlag;
            break;
        default:
            break;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... FrbStatData->FrbEnabled: %x FrbStatData->FrbFailed: %x \n", __FUNCTION__, FrbStatData->FrbEnabled, FrbStatData->FrbFailed));

    return EFI_SUCCESS;
}

/**

    This routine gets the FRB timer status.

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on
    @param Timeout Timer timeout value in microseconds

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS Timer successfully set
    @retval EFI_UNSUPPORTED Timer is not supported
    @retval EFI_INVALID_PARAMETER Invalid parameter passed in

**/

EFI_STATUS
EFIAPI
SetFrbTimerValue (
  IN EFI_SM_FRB_PROTOCOL              *This,
  IN EFI_FRB_TYPE                     FrbType,
  IN UINTN                            *Timeout )
{

   SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x *Timeout: %x \n", __FUNCTION__, FrbType, *Timeout));

   if (FrbType < EfiFrb2 && FrbType > EfiOsBootWdt) {
       return EFI_UNSUPPORTED;
   }

   if (*Timeout <= 0) {
       return EFI_INVALID_PARAMETER;
   }

   //
   // Save time out value in volatile buffer.
   //
   gFrbDataBuffer[FrbType].TimeOutValue = (UINT16)(*Timeout / 100000);  //100 millisecond per count

   SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting........\n", __FUNCTION__));

   return EFI_SUCCESS;
}

/**

    This routine set the FRB pre timeout policy

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on
    @param PretimeoutInterval Pre timeout interval in microseconds
    @param FrbTimeoutInterrupt Type of FRB pre time out interrupt

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS Pre Timeout interval successfully set
    @retval EFI_UNSUPPORTED Timer does not support pre timeout interval

**/

EFI_STATUS
EFIAPI
SetFrbPreTimeOutPolicy (
  IN EFI_SM_FRB_PROTOCOL                * This,
  IN EFI_FRB_TYPE                       FrbType,
  IN UINTN                              *PretimeoutInterval,
  IN FRB_SYSTEM_TIMEOUT_INTERRUPT       FrbTimeoutInterrupt )
{

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x FrbTimeoutInterrupt: %x \n", __FUNCTION__, FrbType, FrbTimeoutInterrupt));

    //
    //Save the Pre time out interrupt in volatile buffer.
    //
    if (FrbType == EfiFrb2 || FrbType == EfiOsBootWdt || FrbType ==  EfiBiosPost) {
        gFrbDataBuffer[FrbType].PretimeoutInterval = (PretimeoutInterval != NULL) ? (*(UINT8 *)PretimeoutInterval) : 0;
        gFrbDataBuffer[FrbType].PreTimeOutInterrupt = FrbTimeoutInterrupt;
        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
}

/**

    This routine sets the boot policy to take on an FRB watchdog timeout.

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on
    @param FrbBootPolicy Boot policy to take on FRB timeout

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS Boot policy set for selected FRB Type
    @retval EFI_UNSUPPORTED Boot policy or timer not supported

**/

EFI_STATUS
SetFrbBootPolicy (
  IN EFI_SM_FRB_PROTOCOL              *This,
  IN EFI_FRB_TYPE                     FrbType,
  IN FRB_SYSTEM_BOOT_POLICY           FrbBootPolicy )
{

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x FrbBootPolicy: %x \n", __FUNCTION__, FrbType, FrbBootPolicy));

    //
    // Save the FRB boot policy in volatile buffer.
    //
    if (FrbType == EfiFrb2 || FrbType == EfiOsBootWdt || FrbType ==  EfiBiosPost) {
        gFrbDataBuffer[FrbType].BootPolicy = FrbBootPolicy;
        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
}

/**

    This routine enables and sets up the selected FRB timer.

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on
    @param UseResidualCount Use residual count
    @param StartFrbCountDown Start the FRB count down
    @param ClearFrbGlobalStatus Clear global status of the FRB

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRB timer enabled, unless FRB2 timer and it is disabled in Setup.
    @retval EFI_DEVICE_ERROR Couldn't Enable FRB-n because of device failure.
    @retval EFI_UNSUPPORTED Returned if timer is not supported.
    @retval EFI_TIMEOUT Residual count is already 0, timed out

**/

EFI_STATUS
EnableFrb (
  IN EFI_SM_FRB_PROTOCOL                *This,
  IN EFI_FRB_TYPE                       FrbType,
  IN BOOLEAN                            UseResidualCount,
  IN BOOLEAN                            StartFrbCountDown,
  IN BOOLEAN                            ClearFrbGlobalStatus )
{
    EFI_STATUS                              Status;
    EFI_BMC_GET_WATCHDOG_TIMER_STRUCTURE    GetWatchDogTimerData = {0};
    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE    SetWatchDogTimerData = {0};
    UINT8                                   FrbTimerDataSize;
    UINT8                                   ResponseDataSize;
    EFI_PEI_HOB_POINTERS                    HobList;
    EFI_BOOT_MODE                           BootMode;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x\n", __FUNCTION__, FrbType));

    if (FrbType < EfiFrb2 && FrbType > EfiOsBootWdt) {
        return EFI_UNSUPPORTED;
    }

    if (FrbType == EfiFrb2) {
        //
        //Code added to not enable frb2 timer in recovery mode
        //
        EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList.Raw);

        BootMode = HobList.HandoffInformationTable->BootMode;
        if (HobList.Header->HobType != EFI_HOB_TYPE_HANDOFF) {
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "(DxeFrb) Hand off HOB missing!\n"));
            return EFI_NOT_FOUND;
        }
        if (BootMode == BOOT_IN_RECOVERY_MODE) {
            return EFI_UNSUPPORTED;
        }
    }

    //
    // Get watchdog timer status to preserve reserved bits
    //
    FrbTimerDataSize = sizeof(GetWatchDogTimerData);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_WATCHDOG_TIMER,
                NULL,
                0,
                (UINT8*)&GetWatchDogTimerData,
                &FrbTimerDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: IPMI_APP_GET_WATCHDOG_TIMER Status : %r\n", __FUNCTION__, Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Load configuration parameters that was set by SetFrbBootPolicy,
    // SetFrbPreTimeOutPolicy and  SetFrbTimerValue APIs. otherwise load
    // configuration parameters that was got from Disable FRB API.
    //

    //
    // check if timer is already expired or not.
    //
    if (gFrbDataBuffer[FrbType].TimeOutValue == 0) {
        return EFI_TIMEOUT;
    }
    //
    // Data Byte-1
    //
    SetWatchDogTimerData.TimerUse.TimerUse = FrbType;
    SetWatchDogTimerData.TimerUse.TimerRunning = 0;
    SetWatchDogTimerData.TimerUse.TimerUseExpirationFlagLog = 0;
    //
    // Data Byte-2
    //
    SetWatchDogTimerData.TimerActions.TimeoutAction = gFrbDataBuffer[FrbType].BootPolicy;
    SetWatchDogTimerData.TimerActions.PreTimeoutIntr = gFrbDataBuffer[FrbType].PreTimeOutInterrupt;
    //
    // Data Byte-3
    //
    SetWatchDogTimerData.PretimeoutInterval = gFrbDataBuffer[FrbType].PretimeoutInterval;
    //
    // Data Byte-4
    //
    //
    // Clear global expiration flag based If asked for
    //
    if (ClearFrbGlobalStatus) {
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.Frb2ExpirationFlag = 1;
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.PostExpirationFlag = 1;
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.OsLoadExpirationFlag = 1;
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.SmsOsExpirationFlag = 1;
        SetWatchDogTimerData.TimerUseExpirationFlagsClear.OemExpirationFlag = 1;
    } else {
        switch (FrbType) {
            case EfiFrb2:
                SetWatchDogTimerData.TimerUseExpirationFlagsClear.Frb2ExpirationFlag = 1;
                break;
            case EfiOsBootWdt:
                SetWatchDogTimerData.TimerUseExpirationFlagsClear.OsLoadExpirationFlag = 1;
                break;
            case EfiBiosPost:
                SetWatchDogTimerData.TimerUseExpirationFlagsClear.PostExpirationFlag = 1;
                break;
            default:
                break;
        }
    }
    //
    // Data Byte-5 and Byte-6
    //
    SetWatchDogTimerData.InitialCountdownValue = gFrbDataBuffer[FrbType].TimeOutValue;

    FrbTimerDataSize = sizeof (SetWatchDogTimerData);
    ResponseDataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_WATCHDOG_TIMER,
                (UINT8*)&SetWatchDogTimerData,
                FrbTimerDataSize,
                NULL,
                &ResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_APP_SET_WATCHDOG_TIMER Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    //
    // issue reset watch dog timer command to start timer
    //
    ResponseDataSize = 0;
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_RESET_WATCHDOG_TIMER,
                NULL,
                0,
                NULL,
                &ResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: IPMI_APP_RESET_WATCHDOG_TIMER Status: %r \n", __FUNCTION__, Status));

    return Status;
}

/**

    This routine disables the specified FRB timer.

    @param This Pointer to the FRB protocol
    @param FrbType Type of FRB timer to get data on

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS FRB timer was disabled
    @retval EFI_DEVICE_ERROR Couldn't Disable FRB-n because of device failure.
    @retval EFI_UNSUPPORTED This type of FRB timer is not supported.
    @retval EFI_ABORTED Timer was already stopped

**/

EFI_STATUS
EFIAPI
DisableFrb (
  IN EFI_SM_FRB_PROTOCOL              *This,
  IN EFI_FRB_TYPE                     FrbType )
{

    EFI_STATUS                                  Status;
    EFI_BMC_GET_WATCHDOG_TIMER_STRUCTURE        GetWatchDogTimerData = {0};
    EFI_BMC_SET_WATCHDOG_TIMER_STRUCTURE        SetWatchDogTimerData = {0};
    UINT8                                       FrbTimerDataSize;
    UINT8                                       ResponseDataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... FrbType: %x \n", __FUNCTION__, FrbType));

    if (FrbType < EfiFrb2 && FrbType > EfiOsBootWdt) {
        return EFI_UNSUPPORTED;
    }

    //
    // get FRB status to check whether timer is already stopped or not.
    //
    FrbTimerDataSize = sizeof (GetWatchDogTimerData);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_WATCHDOG_TIMER,
                NULL,
                0,
                (UINT8*)&GetWatchDogTimerData,
                &FrbTimerDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: IPMI_APP_GET_WATCHDOG_TIMER Status: %r \n", __FUNCTION__, Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // if timer already stopped then return EFI_ABORTED.
    //
    if (!GetWatchDogTimerData.TimerUse.TimerRunning) {
        DEBUG ((EFI_D_ERROR, "%a: EfiFrb2 already disabled  \n", __FUNCTION__));
        return EFI_ABORTED;
    }

    //
    // timer running should be same as FrbType.
    //
    if (GetWatchDogTimerData.TimerUse.TimerUse != FrbType) {
      return EFI_NOT_FOUND;
    }

    //
    // Store current count down and time out policies in volatile buffer. In case EnableFrb called
    // without calling SetFrbTimer and set FRB policy APIs to configure FRB.
    //
    gFrbDataBuffer[FrbType].TimeOutValue = GetWatchDogTimerData.PresentCountdownValue;
    gFrbDataBuffer[FrbType].PretimeoutInterval = GetWatchDogTimerData.PretimeoutInterval;
    gFrbDataBuffer[FrbType].PreTimeOutInterrupt = GetWatchDogTimerData.TimerActions.PreTimeoutIntr;
    gFrbDataBuffer[FrbType].BootPolicy = GetWatchDogTimerData.TimerActions.TimeoutAction;

    //
    // Disable timer by using set watchdog timer with timer value and DontStopTimer bit value as zero.
    // while disabling don't clear global expiration flag.
    //
    SetWatchDogTimerData.TimerUse.TimerUse     = FrbType;
    SetWatchDogTimerData.TimerUse.TimerRunning = 0;
    SetWatchDogTimerData.InitialCountdownValue = 0;

    FrbTimerDataSize = sizeof (SetWatchDogTimerData);
    ResponseDataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_WATCHDOG_TIMER,
                (UINT8*)&SetWatchDogTimerData,
                FrbTimerDataSize,
                NULL,
                &ResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: IPMI_APP_SET_WATCHDOG_TIMER Status: %r \n", __FUNCTION__, Status));
    return Status;
}

/**

    Disables FRB2. This function gets called each time the
    EFI_AMI_LEGACYBOOT_PROTOCOL_GUID gets signaled

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/
VOID
EFIAPI
FrbEndOfPostAmiLegacyBootEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

    EFI_STATUS      Status;
    VOID            *legacyBootProtocol = NULL;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Entering Legacy Boot using gAmiLegacyBootProtocolGuid - IPMI_SEPARATE_DXE_SMM_INTERFACES token is 1. Disabling FRB  and enabling OS WDT.\n"));
    Status = gBS->LocateProtocol(
                    &gAmiLegacyBootProtocolGuid,
                    NULL,
                    (VOID **)&legacyBootProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "LocateProtocol gAmiLegacyBootProtocolGuid Status = %r\n", Status));

    if( !(EFI_ERROR(Status)) ) {
      if (gServerMgmtConfiguration.Frb2Enable) {
          Status = DisableFrb (
                    gFrbProtocol,
                    EfiFrb2 );
          DEBUG ((EFI_D_INFO, "EfiFrb2 disabled Status = %r\n", Status));
      }

      if (gServerMgmtConfiguration.OsBootWdtEnable) {
          Status = EnableOsWdtTimer ();
          DEBUG ((EFI_D_INFO, "EfiOsBootWdt Enabled Status = %r\n", Status));
      }
    }
}

/**

    Disables FRB2 and enables WDT. This function gets called,
    In legacy boot. - When legacy boot event gets signaled
    In UEFI boot    - When AMITSE_EVENT_BEFORE_BOOT_GUID is signaled

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
FrbEndOfPostEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

    EFI_STATUS      Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Legacy boot with IPMI_SEPARATE_DXE_SMM_INTERFACES token as 0 or UEFI Boot - Disabling FRB  and enabling OS WDT.\n"));
    if (gServerMgmtConfiguration.Frb2Enable) {
          Status = DisableFrb (
                    gFrbProtocol,
                    EfiFrb2 );
          DEBUG ((EFI_D_INFO, "EfiFrb2 disabled Status = %r\n", Status));
    }

    if (gServerMgmtConfiguration.OsBootWdtEnable) {
          Status = EnableOsWdtTimer ();
          DEBUG ((EFI_D_INFO, "EfiOsBootWdt Enabled Status = %r\n", Status));
    }
}

/**

    Enables OsWdt Timer.

    @param VOID

    @retval EFI_SUCCESS Enabled OS Watchdog Timer
**/

EFI_STATUS
EnableOsWdtTimer (
VOID ) 
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINTN                           TimeOut;

    //
    // Enable OsWdt Timer.
    //
    if (gServerMgmtConfiguration.OsBootWdtEnable) {

        //
        // Set the OsBootWdt Timeout
        //
        TimeOut = gServerMgmtConfiguration.OsBootWdtTimeout * 1000000;
        Status = SetFrbTimerValue (gFrbProtocol, EfiOsBootWdt, &TimeOut);

        //
        // Set the OsBootWdt timeout policy
        //
        Status = SetFrbBootPolicy (gFrbProtocol, EfiOsBootWdt, gServerMgmtConfiguration.OsBootWdtTimeoutPolicy);

        //
        //Set Pre Timeout interrupt
        //
        Status = SetFrbPreTimeOutPolicy (gFrbProtocol, EfiOsBootWdt, NULL, NoTimeoutInterrupt);

        //
        // Start the OsBootWd timer
        //
        Status = EnableFrb (gFrbProtocol, EfiOsBootWdt, FALSE, FALSE, FALSE);
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " Status(Oswdt expiry function) = %x\n", Status));
    return Status;
}

/**

    Entry point of the DXE FRB Driver.

    @param ImageHandle Handle for the image of this driver
    @param SystemTable Pointer to the EFI System Table

    @retval EFI_SUCCESS Protocol successfully started and installed
    @retval EFI_INVALID_PARAMETER Protocol could not installed.

**/

EFI_STATUS
InitializeDxeFrb (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                              Status;
    EFI_HANDLE                              FrbHandle;
    UINTN                                   Size;
    //APTIOV_SERVER_OVERRIDE_START
    EFI_GUID                                gEfiShellEnvironmentGuid = SHELL_ENVIRONMENT_PROTOCOL_GUID;
    EFI_GUID                                gEfiShellProtocolGuid    = EFI_SHELL_PROTOCOL_GUID;
    //APTIOV_SERVER_OVERRIDE_END
#if (IPMI_SEPARATE_DXE_SMM_INTERFACES == 1)
    VOID                                    *Frb2PostEventRegId = NULL;

#else
    EFI_EVENT                               FrbTimerEvent = NULL;

#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));

    //
    // Locate DXE IPMI Transport protocol
    //
    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));

    //
    // If IPMI transport protocol is not available, then there is no point to produce
    // FRB protocol for later use so just return.
    //
    if (EFI_ERROR(Status)) {
        return Status;
    }

    gFrbProtocol = (EFI_SM_FRB_PROTOCOL *)AllocateZeroPool (sizeof (EFI_SM_FRB_PROTOCOL));
    if (gFrbProtocol == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gFrbProtocol! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Install FRB protocol for later use.
    //
    gFrbProtocol->GetFrbStatData            = GetFrbStatus;
    gFrbProtocol->SetFrbTimerData           = SetFrbTimerValue;
    gFrbProtocol->SetFrbPreTimeoutPolicy    = SetFrbPreTimeOutPolicy;
    gFrbProtocol->SetFrbBootPolicy          = SetFrbBootPolicy;
    gFrbProtocol->EnableFrb                 = EnableFrb;
    gFrbProtocol->DisableFrb                = DisableFrb;

    FrbHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                &FrbHandle,
                &gEfiSmFrbProtocolGuid,
                EFI_NATIVE_INTERFACE,
                gFrbProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiSgFrbProtocolGuid Install protocol : Status: %r \n", Status));
    if (EFI_ERROR (Status)) {

        //
        // Error in installing the protocol. So free the allocated memory.
        //
        FreePool (gFrbProtocol);
        return Status;
    }

    //
    // Get Server Mgmt Setup Variable
    //
    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gRT->GetVariable (
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &gServerMgmtConfiguration );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetVariable Status %r \n", Status));
    if(EFI_ERROR(Status)) {

        //
        // load default value.
        //
        gServerMgmtConfiguration.Frb2Enable = 1; // enabled
        gServerMgmtConfiguration.OsBootWdtEnable = 0; //disabled
        gServerMgmtConfiguration.OsBootWdtTimeout = 600; //10 minutes
        gServerMgmtConfiguration.OsBootWdtTimeoutPolicy = 1; //hard reset
    }

    if (gServerMgmtConfiguration.Frb2Enable) {

        //
        // Register Event for OptionRom Scan GUID to disable FRB2 Timer before OptionRom scan and Enable after OptioRom scan.
        //
        EfiNamedEventListen (
            &gOpromStartEndProtocolGuid,
            TPL_CALLBACK,
            DisableEnableFrb2DuringOpRomScan,
            NULL,
            NULL);

        //
        // Register Event for Password Prompt Enter GUID to disable FRB2 Timer before entering the password.
        //
        EfiNamedEventListen (
            &gAmiTsePasswordPromptEnterGuid,
            TPL_NOTIFY,
            DisableFrb2BeforePassword,
            NULL,
            NULL);

        //
        // Register Event for Password Prompt Exit GUID to Enable FRB2 Timer after entering the password.
        //
        EfiNamedEventListen (
            &gAmiTsePasswordPromptExitGuid,
            TPL_NOTIFY,
            EnableFrb2AfterPassword,
            NULL,
            NULL );
      
        //
        // Register Event for Setup Prompt timeout start GUID to disable FRB2 Timer before waiting for key press.
        //
        EfiNamedEventListen (
            &gAmiTseBeforeTimeOutGuid,
            TPL_NOTIFY,
            DisableFrb2BeforePromptTimeOut,
            NULL,
            NULL);

        //
        // Register Event for Setup Prompt timeout end GUID to enable FRB2 Timer after waiting for key press.
        //
        EfiNamedEventListen (
            &gAmiTseAfterTimeOutGuid,
            TPL_NOTIFY,
            EnableFrb2AfterPromptTimeOut,
            NULL,
            NULL);
    }

    //APTIOV_SERVER_OVERRIDE_START
    if ( ( gServerMgmtConfiguration.OsBootWdtEnable ) ) {
        //
        // Register Event for EDK-I Shell guid to disable OS Wdt Timer before booting to Shell.
        //
        EfiNamedEventListen (
          &gEfiShellEnvironmentGuid,
          TPL_NOTIFY,
          DisableOsWdtBeforeShell,
          NULL,
          NULL);
        
        //
        // Register Event for EDK-II Shell guid to disable OS Wdt Timer before booting to Shell.
        //
        EfiNamedEventListen (
          &gEfiShellProtocolGuid,
          TPL_NOTIFY,
          DisableOsWdtBeforeShell,
          NULL,
          NULL
          );
        //APTIOV_SERVER_OVERRIDE_END
        //
        // Register Event for AmiTseAfterFirstBootOptionGuid to clear a flag indicating first boot option 
        // is clicked and OS WDT can be started
        //
        EfiNamedEventListen (
          &gAmiTseAfterFirstBootOptionGuid,
          TPL_NOTIFY,
          ClearFlag,
          NULL,
          NULL);
    }

    if ( ( gServerMgmtConfiguration.Frb2Enable ) || ( gServerMgmtConfiguration.OsBootWdtEnable ) ) {

         //
         //Register Event for Setup enter GUID to disable FRB2/OS wdt Timer before entering to setup.
         //
         EfiNamedEventListen (
           &gAmiTseSetupEnterGuid,
           TPL_CALLBACK,
           DisableTimersBeforeSetup,
           NULL,
           NULL);
#if( IPMI_SEPARATE_DXE_SMM_INTERFACES == 1)

         //
         // If DXE and SMM interfaces use different ports, only then use gAmiLegacyBootProtocolGuid
         // to register event to disable FRB2 and Enable OS Watchdog timers before legacy boot.
         //
         EfiCreateProtocolNotifyEvent (
           &gAmiLegacyBootProtocolGuid,
           TPL_CALLBACK,
           FrbEndOfPostAmiLegacyBootEvent,
           NULL,
           &Frb2PostEventRegId );
#else

         //
         // Register events to disable FRB2 and Enable OS Watchdog timers just before legacy Boot.
         //
         // In PiSmmIpl driver, there is a callback function registered for LegacybootEvent which denies access to all the
         // UEFI services in SMM after Legacy boot event is triggered. This callback function is getting control before our EnableOsWdtTimer().
         // Because of this Callback in PiSmmIpl driver, SmmCommunicate is not working after legacy boot event.
         // Changing the TPL_CALLBACK task priority level to TPL_NOTIFY to get the control before PismmIPl driver's callback gets control.
         EfiCreateEventLegacyBootEx (
           TPL_NOTIFY,
           //TPL_CALLBACK,
           FrbEndOfPostEvent,
           NULL,
           &FrbTimerEvent );
#endif

         //
         // Register event for AMITSE_EVENT_BEFORE_BOOT_GUID to disable FRB2 timer before Uefi boot.
         //
         EfiNamedEventListen (
           &gAmiTseEventBeforeBootGuid,
           TPL_CALLBACK,
           FrbEndOfPostEvent,
           NULL,
           NULL );

    }

    return EFI_SUCCESS;
}

/**

    Disables FRB2. This function gets called each time when
    guidSetupEnter protocol installed.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
DisableTimersBeforeSetup (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

    EFI_STATUS      Status;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Entering Setup. Disabling FRB/OS WDT if they are active.\n"));
    gTimerCtlFlag = TRUE;
    if ( gServerMgmtConfiguration.Frb2Enable ) {
         Status = DisableFrb (
                   gFrbProtocol,
                   EfiFrb2 );
         DEBUG ((EFI_D_INFO, "EfiFrb2 disabled Before setup Status = %r\n", Status));

      if ( gServerMgmtConfiguration.OsBootWdtEnable ) {
           Status = DisableFrb (
                    gFrbProtocol,
                    EfiOsBootWdt );
           DEBUG ((EFI_D_INFO, "EfiOsBootWdt disabled Before setup Status = %r\n", Status));
           SERVER_IPMI_DEBUG ((EFI_D_INFO, "gTimerCtlFlag = %x\n", gTimerCtlFlag));
      }
    }
}

/**

    Disable and Enable FRB2 during OptionRom scan. This function gets called
    each time when guidOptionRomScan protocol installed.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
DisableEnableFrb2DuringOpRomScan (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

    static BOOLEAN  FrbDisabled;
    EFI_STATUS      Status;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : OPROM Scan. Disable/reenable FRB wdt.\n"));

    if (!FrbDisabled) {
        Status = DisableFrb (gFrbProtocol, EfiFrb2);
        if (!EFI_ERROR(Status)) {
            DEBUG ((EFI_D_INFO, "EfiFrb2 disabled Before OPRom Status = %r\n", Status));
            FrbDisabled = TRUE;
            return;
        }
    }
    if (FrbDisabled) {
        Status = EnableFrb (gFrbProtocol, EfiFrb2, FALSE, FALSE, FALSE);
        if (!EFI_ERROR(Status)) {
            FrbDisabled = FALSE;
            DEBUG ((EFI_D_INFO, "EfiFrb2 Enabled after OPRom Status = %r\n", Status));
        }
    }
}

/**

    Disables FRB2 timer when AMITSE password prompt enter event is signaled.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
DisableFrb2BeforePassword (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    EFI_STATUS      Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Password Prompt Enter. Disable FRB wdt.\n"));
    Status = DisableFrb (
               gFrbProtocol,
               EfiFrb2);
    DEBUG ((EFI_D_INFO, "EfiFrb2 Disabled Before Giving Password= %r\n", Status));
}

/**

    Enables FRB2 timer when AMITSE password prompt exit event is signaled.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
EnableFrb2AfterPassword (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    EFI_STATUS      Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Password Prompt Exit. Enable FRB wdt.\n"));
    Status = EnableFrb (
               gFrbProtocol,
               EfiFrb2,
               FALSE,
               FALSE,
               FALSE);
    DEBUG ((EFI_D_INFO, "EfiFrb2 Enabled after Giving Password= %r\n", Status));
}

/**
    Disables FRB2 timer when event signaled before AMITSE Key Press prompt timeout start.

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
DisableFrb2BeforePromptTimeOut (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    EFI_STATUS              Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Prompt Timeout Enter. Disable FRB wdt.\n"));
    Status = DisableFrb (
               gFrbProtocol,
               EfiFrb2);
    DEBUG ((EFI_D_INFO, "EfiFrb2 Disabled Before PromptTimeOut Start= %r\n", Status));
}

/**
    Enables FRB2 timer when event signaled after AMITSE Key Press prompt timeout ends.

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
EnableFrb2AfterPromptTimeOut (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    EFI_STATUS              Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Prompt Timeout Exit. Enable FRB wdt.\n"));
    Status = EnableFrb (
               gFrbProtocol,
               EfiFrb2,
               FALSE,
               FALSE,
               FALSE);
    DEBUG ((EFI_D_INFO, "EfiFrb2 Enabled after Prompt TimeOut Ends= %r\n", Status));
}

/**
    Disables OS WDT on entering Shell

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @retval VOID

**/
VOID
EFIAPI
DisableOsWdtBeforeShell (
  IN EFI_EVENT        Event,
  IN VOID             *Context) {

    EFI_STATUS Status;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Shell Enter. Disable OS WDT.\n"));
    Status = DisableFrb (
               gFrbProtocol,
               EfiOsBootWdt );
    DEBUG ((EFI_D_INFO, "OS WDT is disabled on entering shell. Status = %r\n",Status));
}

/**
    Disables/Enables OS WDT on entering Shell based on flag

    @param VOID

    @retval VOID

**/
VOID
EFIAPI
DisableEnableOsWdtTimer (
  VOID ) {

    EFI_STATUS Status;
    if (gServerMgmtConfiguration.OsBootWdtEnable) {
        EFI_SM_FRB_PROTOCOL    *FrbProtocol;
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Shell Exit. Enable/Disable OS WDT.\n"));

        //
        // Locate EFI_SM_FRB_PROTOCOL to send IPMI command to enable watchdog timer.
        //
        Status = gBS->LocateProtocol (
                        &gEfiSmFrbProtocolGuid,
                        NULL,
                        (VOID **)&FrbProtocol );

        if (gTimerCtlFlag == TRUE) {
          SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : gTimerCtlFlag is set.Boot was done from Setup.Stopping Os Wdt.\n"));
          Status = FrbProtocol->DisableFrb (
                     FrbProtocol,
                     EfiOsBootWdt );
          DEBUG ((EFI_D_INFO, " OS WDT is stopped in setup Status = %r\n",Status));
        } else {
            Status = FrbProtocol->EnableFrb (
                       FrbProtocol,
                       EfiOsBootWdt,
                       FALSE,
                       FALSE,
                       FALSE);
            DEBUG ((EFI_D_INFO, " OS WDT is enabled on exiting shell Status = %r\n",Status));
        }
    }
}

/**
    Clears a flag to indicate first boot option is clicked and OS WDT can be started

    @param VOID

    @retval VOID

**/
VOID
EFIAPI
ClearFlag (
  IN EFI_EVENT        Event,
  IN VOID             *Context) {
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI FRB : Boot option booted from setup.Clearing flag.\n"));
    gTimerCtlFlag = FALSE;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Flag set to False.\n"));
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
