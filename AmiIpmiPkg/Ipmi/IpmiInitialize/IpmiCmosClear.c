//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fix BIOS setting load default by IPMI function fail.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Mar/22/2017
//
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

/** @file IpmiCmosClear.c
    Check CMOS Clear flag is set and save NVRAM variable

**/

//----------------------------------------------
#include "IpmiBmc.h"
#include <Protocol/Reset.h>
#include <Library/UefiLib.h>
#include <Include/IpmiNetFnChassisDefinitions.h>
//----------------------------------------------

#define IPMI_STALL          1000

#define VAR_FLAG            EFI_VARIABLE_BOOTSERVICE_ACCESS |  \
                            EFI_VARIABLE_NON_VOLATILE
//
// Extern Variables
//
extern EFI_IPMI_BMC_INSTANCE_DATA   *gIpmiInstance;
extern EFI_GUID gEfiIpmiCmosClearVariableGuid;

/**
    Wait for IPMI Get System Boot Option Set-in-Progress bit to clear

    @param VOID

    @retval EFI_SUCCESS Set in progress clear successful
    @retval EFI_TIMEOUT Set in progress clear failed

**/
EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID )
{
    EFI_STATUS                                Status;
    UINT8                                     ResponseSize;
    UINT32                                    Retries = 10;
    IPMI_GET_BOOT_OPTIONS_REQUEST             GetRequest;
    GET_SET_IN_PROGRESS_RESPONSE              GetSetInProgResponse;

    GetRequest.ParameterSelector = SetInProgress;
    GetRequest.Reserved = 0x0;
    GetRequest.SetSelector = 0x0;
    GetRequest.BlockSelector = 0x0;

    //
    // Wait for in-progress bit to clear
    //
    do {
        ResponseSize = sizeof (GetSetInProgResponse);
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                                    &gIpmiInstance->IpmiTransport,
                                    IPMI_NETFN_CHASSIS,
                                    0,
                                    IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
                                    (UINT8*) &GetRequest,
                                    sizeof (GetRequest),
                                    (UINT8*) &GetSetInProgResponse,
                                    &ResponseSize );

        if (EFI_ERROR (Status)) {
            return Status;
        }

        if (GetSetInProgResponse.Param0.SetInProgress == 0) {
            break;
        }

        gBS->Stall(IPMI_STALL);
    } while (Retries-- > 0);

    if (++Retries == 0) {  // time out after Retires and first attempt.
        return EFI_TIMEOUT;
    }

    return EFI_SUCCESS;
}

/**
    Send the Get boot options command to get the boot flags parameter

    @param BootFlagsResponse Pointer to get the boot flags response

    @return BootFlagsResponse Input pointer updated with boot flags response data

**/

EFI_STATUS
IpmiGetBootFlags (
  OUT GET_BOOT_FLAG_RESPONSE        *BootFlagsResponse )
{
    EFI_STATUS                        Status;
    UINT8                             ResponseSize;
    IPMI_GET_BOOT_OPTIONS_REQUEST     GetRequest;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IpmiWaitSetInProgressClear: Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    ResponseSize = sizeof (GET_BOOT_FLAG_RESPONSE);

    GetRequest.ParameterSelector = BootFlags;
    GetRequest.Reserved = 0x0;
    GetRequest.SetSelector = 0x0;
    GetRequest.BlockSelector = 0x0;

    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                 &gIpmiInstance->IpmiTransport,
                 IPMI_NETFN_CHASSIS,
                 0,
                 IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
                 (UINT8*) &GetRequest,
                 sizeof (GetRequest),
                 (UINT8*) BootFlagsResponse,
                 &ResponseSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS Status: %r \n", __FUNCTION__, Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... Status: %r \n", __FUNCTION__, Status));

    return Status;
}

/**
    This is called to Clear CMOS flag bit

    @param Param5 Retrieved Flags.

    @return EFI_STATUS Status of Command

**/

EFI_STATUS
IpmiClearCmosFlag (
   IN  BOOT_OPTIONS_BOOT_FLAGS    Param5 )
{

    EFI_STATUS             Status;
    SET_BOOT_FLAG_REQUEST  RequestData;
    UINT8                  RequestSize;
    UINT8                  ResponseSize;
    UINT8                  ResponseData;
    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear ();
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Param5.CmosClear = 0; // Clear CMOS Flag bit
    RequestData.Param5 = Param5; // Retain other flag value
    RequestData.Request.ParameterSelector = BootFlags; // Parameter Selector
    ResponseSize = 1;
    RequestSize = sizeof (RequestData);
    Status =gIpmiInstance->IpmiTransport.SendIpmiCommand (
                                    &gIpmiInstance->IpmiTransport,
                                    IPMI_NETFN_CHASSIS,
                                    0,
                                    IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
                                    (UINT8*) &RequestData,
                                    RequestSize,
                                    (UINT8*) & ResponseData,
                                    &ResponseSize );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    return EFI_SUCCESS;

}

/**
    Notification called when ResetArchProtocol installed

    @param Event Event 
    @param Context Detail about event

    @retval VOID

**/

VOID
EFIAPI
ResetArchProtocolNotification (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context )
{
    EFI_STATUS      Status;
    VOID            *ResetArchProtocol = NULL;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));

    Status = gBS->LocateProtocol (
                &gEfiResetArchProtocolGuid,
                NULL,
                (VOID **)&ResetArchProtocol);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBS->LocateProtocol gEfiResetArchProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    //
    // Reset System
    //
    gRT->ResetSystem (
#if IPMI_CMOS_CLEAR_RESET_TYPE
            EfiResetWarm,
#else
            EfiResetCold,
#endif
            EFI_SUCCESS,
            0,
            NULL);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));

    return;
}

/**
    Method check for CMOS Clear flag bit is set. If set Create a NVRAM  variable.

    @param VOID

    @return EFI_STATUS Status of Execution

**/

EFI_STATUS 
IpmiCmosClear (
    VOID )
{

    EFI_STATUS                 Status;
    GET_BOOT_FLAG_RESPONSE     BootFlags;
    VOID                       *NotifyRegistration;
    UINTN                      Size;
    UINT8                      Value;

    Size = sizeof (UINT8);
    Status = gRT->GetVariable (
                        L"IpmiCmosClear",
                        &gEfiIpmiCmosClearVariableGuid,
                        NULL,
                        &Size,
                        &Value);
    if (!EFI_ERROR (Status)) {
        Status = gRT->SetVariable (
                        L"IpmiCmosClear",
                        &gEfiIpmiCmosClearVariableGuid,
                        VAR_FLAG,
                        0,
                        &Value );
    }
    Status = IpmiGetBootFlags (&BootFlags);

    if (EFI_ERROR (Status)) {
        return Status;
    }
//SMCPKG_SUPPORT    if ( BootFlags.Param5.BootFlagValid ) {
        if (BootFlags.Param5.CmosClear) {
            Size = sizeof (UINT8);
            Value = 1;
            Status = gRT->SetVariable (
                            L"IpmiCmosClear",
                            &gEfiIpmiCmosClearVariableGuid,
                            VAR_FLAG,
                            Size,
                            &Value );
              
            if ( !EFI_ERROR (Status)) {
                //
                // Create Notification event for Reset arch protocol GUID
                //
                EfiCreateProtocolNotifyEvent (
                            &gEfiResetArchProtocolGuid,
                            TPL_NOTIFY,
                            ResetArchProtocolNotification,
                            NULL,
                            &NotifyRegistration
                            );
                //
                // Clear the BMC BootFlags
                //
                Status = IpmiClearCmosFlag (BootFlags.Param5);
            }
        }
//SMCPKG_SUPPORT    }

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
