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

/** @file ByPassUserPassWord.c
    Password prompt is not displayed in post when IPMI boot
    "UserPasswordBypass" flag is set for user level access.

**/

//----------------------------------------------------------------------

#include "IpmiBoot.h"
#include <Include/AMIVfr.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
#define AMI_PASSWORD_NONE       0x00
#define AMI_PASSWORD_USER       0x01
#define AMI_PASSWORD_ADMIN      0x02
#define AMI_PASSWORD_ANY        0x03

#define COMMAND_RETRY_COUNT     0x0A
#define BIT_CLEAR               0x00
#define SELECTOR_NONE          0x00 // No Selector is used


//
// Extern Variables
//
extern EFI_SYSTEM_TABLE         *gST;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_RUNTIME_SERVICES     *gRT;

extern UINT32 gPasswordType; //Provides what is the current access level

//
// Global Variables
//
EFI_IPMI_TRANSPORT  *gIpmiTransport;

//
// Function Prototypes
//
BOOLEAN ProcessConInAvailability (VOID);
UINT32 PasswordCheckInstalled (VOID);

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
    EFI_STATUS                      Status;
    UINT8                           ResponseSize;
    UINT32                          Retries = COMMAND_RETRY_COUNT;
    IPMI_GET_BOOT_OPTIONS_REQUEST   GetRequest;
    GET_SET_IN_PROGRESS_RESPONSE    GetSetInProgResponse;

    GetRequest.ParameterSelector = SetInProgress;
    GetRequest.Reserved = SELECTOR_NONE;
    GetRequest.SetSelector = SELECTOR_NONE;
    GetRequest.BlockSelector = SELECTOR_NONE;

    //
    // Wait for in-progress bit to clear
    //
    do {
        ResponseSize = sizeof (GetSetInProgResponse);
        Status = gIpmiTransport->SendIpmiCommand (
                     gIpmiTransport,
                     IPMI_NETFN_CHASSIS,
                     BMC_LUN,
                     IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
                     (UINT8*) &GetRequest,
                     sizeof (GetRequest),
                     (UINT8*) &GetSetInProgResponse,
                     &ResponseSize );
        if (EFI_ERROR (Status)) {
            return Status;
        }
        //
        // Check for SetInProgress bit
        //
        if (GetSetInProgResponse.Param0.SetInProgress ==  BIT_CLEAR) {
            break;
        }
        //
        // Give some delay before sending the command again
        //
        gBS->Stall(IPMI_STALL);
    } while ( Retries-- > 0 );

    //
    // Return for time out case
    //
    if (++Retries == 0) {  // time out after Retires and first attempt.
        return EFI_TIMEOUT;
    }

    return EFI_SUCCESS;
}

/**
    Send the Get boot options command to get boot flags parameter

    @param BootFlagsResponse Pointer to get the boot flags 
           response

    @retval BootFlagsResponse Input pointer updated with boot flags response data
    @retval EFI_STATUS Return status of function calls used in this function

**/


EFI_STATUS
IpmiGetBootFlags (
  OUT GET_BOOT_FLAG_RESPONSE        *BootFlagsResponse )
{
    EFI_STATUS                        Status;
    UINT8                             ResponseSize;
    IPMI_GET_BOOT_OPTIONS_REQUEST     GetRequest;

    //
    // Check the status of SetInProgress bit
    //
    Status = IpmiWaitSetInProgressClear ();
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Read Boot options command request
    //
    ResponseSize = sizeof (GET_BOOT_FLAG_RESPONSE);

    GetRequest.ParameterSelector = BootFlags;
    GetRequest.Reserved = 0x0;
    GetRequest.SetSelector = SELECTOR_NONE;
    GetRequest.BlockSelector = SELECTOR_NONE;

    Status = gIpmiTransport->SendIpmiCommand (
                 gIpmiTransport,
                 IPMI_NETFN_CHASSIS,
                 BMC_LUN,
                 IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
                 (UINT8*) &GetRequest,
                 sizeof (GetRequest),
                 (UINT8*) BootFlagsResponse,
                 &ResponseSize );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    return EFI_SUCCESS;
}

/**
    This function reads the Boot Info Acknowledge data bytes from BMC.

    @param BootInfoAcknowledge Buffer to return
             Boot Info Acknowledge.

    @retval EFI_SUCCESS Read Boot Info Acknowledge data successfully.

**/

EFI_STATUS
IpmiGetBootInfoAcknowledgeData(
  IN IPMI_BOOT_INFO_ACKNOWLEDGE     *Param4 )
{

    EFI_STATUS                              Status;
    UINT8                                   ResponseDataSize;
    IPMI_GET_BOOT_OPTIONS_REQUEST           GetRequest;
    IPMI_BOOT_INFO_ACKNOWLEDGE_RESPONSE     BootInfoAcknowledgeResponse;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear ();
    if (EFI_ERROR (Status)) {
        return Status;
    }

    ResponseDataSize = sizeof (BootInfoAcknowledgeResponse);

    GetRequest.ParameterSelector = BootInfoAck;
    GetRequest.Reserved = 0x0;
    GetRequest.SetSelector = 0x0;
    GetRequest.BlockSelector = 0x0;

    Status = gIpmiTransport->SendIpmiCommand (
                 gIpmiTransport,
                 IPMI_NETFN_CHASSIS,
                 0,
                 IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
                 (UINT8*) &GetRequest,
                 sizeof (GetRequest),
                 (UINT8*)&BootInfoAcknowledgeResponse,
                 &ResponseDataSize);
    if (!EFI_ERROR(Status)) {
        EfiCopyMem(
            Param4,
            &(BootInfoAcknowledgeResponse.Param4),
            sizeof(IPMI_BOOT_INFO_ACKNOWLEDGE));
    }

    return Status;

}

/**
    This is a replacement for the ProcessConInAvailability
    hook in TSE, to By pass password prompt for user level
    based on IPMI boot "UserPasswordBypass" flag

        
    @param VOID

         
    @return BOOLEAN 
    @retval TRUE if the screen was used to ask password.
    @retval FALSE if the screen was not used to ask password.

**/

BOOLEAN
IpmiByPassUserPassWord (
  VOID )
{
    BOOLEAN                     ByPassUserPassWord = FALSE;
    EFI_STATUS                  Status;
    IPMI_BOOT_INFO_ACKNOWLEDGE  Param4;
    UINTN                       BootFlagsDataSize;
    GET_BOOT_FLAG_RESPONSE      BootFlags;
    UINT8                       SysAccessValue;
    UINT8                       CurrentPasswordType;
    EFI_GUID                    SysAccessGuid = SYSTEM_ACCESS_GUID;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entry...\n", __FUNCTION__));

    //
    // Locate the IPMI Transport Protocol
    //
    Status = gBS->LocateProtocol (
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 (VOID **)&gIpmiTransport);
    SERVER_IPMI_TRACE((TRACE_ALWAYS, "LocateProtocol gEfiDxeIpmiTransportProtocolGuid. Status: %r\n", Status));
    if ( EFI_ERROR (Status) ) {
        goto CallOriginalHook;
    }

    //
    // Read the boot info acknowledge bytes from BMC
    //
    Status = IpmiGetBootInfoAcknowledgeData (&Param4);

    if(EFI_ERROR(Status)) {
        goto CallOriginalHook;
    }
 
    //
    // Support user password bypass only if BootInitiatorAcknowledgeData is zero
    // or BiosOrPostBit is set in BootInitiatorAcknowledgeData
    //
    if ((Param4.BootInitiatorAcknowledgeData.RawData == 0)
        || (Param4.BootInitiatorAcknowledgeData.BitFields.BiosOrPostBit)) {
        //
        // Read the boot flag bytes from BMC
        //
        Status = IpmiGetBootFlags (&BootFlags);
        if ( BootFlags.Param5.BootFlagValid ) {
            ByPassUserPassWord = BootFlags.Param5.UserPasswordBypass;
        } else {
            //
            // Get the IpmiBootFlags NVRAM Variable stored in previous persistent boots.
            //
            BootFlagsDataSize = sizeof(GET_BOOT_FLAG_RESPONSE);
            Status = gRT->GetVariable (
                             L"IpmiBootFlags",
                             &gEfiIpmiBootGuid,
                             NULL,
                             &BootFlagsDataSize,
                             &BootFlags);

            if ( EFI_ERROR(Status) ) {
                goto CallOriginalHook;
            } else {
                ByPassUserPassWord = BootFlags.Param5.UserPasswordBypass;
            }
        }
    } else {
        goto CallOriginalHook;
    }

    CurrentPasswordType = PasswordCheckInstalled ();

    if ( (ByPassUserPassWord == TRUE) &&\
        (AMI_PASSWORD_USER & CurrentPasswordType) ) {
        //
        // If User Password is set,By Pass Password Prompt.
        // Set Access privilege as Administrator.
        //
        if ( AMI_PASSWORD_USER == CurrentPasswordType ) {
            gPasswordType = AMI_PASSWORD_ADMIN;
            SysAccessValue = SYSTEM_PASSWORD_ADMIN;
        } else {
            //
            // If Both Administrator and User Password set, By Pass Password Prompt.
            // Set Access privilege as User.
            //
            gPasswordType = AMI_PASSWORD_USER;
            SysAccessValue = SYSTEM_PASSWORD_USER;
        }
        Status = gRT->SetVariable (
                    L"SystemAccess",
                    &SysAccessGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (SysAccessValue),
                    &SysAccessValue );
        return FALSE;
    }

    //
    // User by pass password bit is not set so call Original Hook
    //
CallOriginalHook:
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting..... User by pass password bit is not set so calling Original Hook \n", __FUNCTION__));
    return ProcessConInAvailability ();

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
