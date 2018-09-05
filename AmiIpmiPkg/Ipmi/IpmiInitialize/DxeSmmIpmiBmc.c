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

/** @file DxeSmmIpmiBmc.c
    Establishes communication to use SMM services for DXE case

**/

//----------------------------------------------------------------------

#include <Library/UefiLib.h>
#include "DxeSmmIpmiBmc.h"

//----------------------------------------------------------------------

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

//
// Extern Variables
//
extern EFI_IPMI_BMC_INSTANCE_DATA   *gIpmiInstance;

//
// Global variables
//
EFI_SMM_COMMUNICATION_PROTOCOL  *gSmmCommunication = NULL;
UINT8                           *gCommunicateBuffer = NULL;
UINTN                            gCommunicateBufferSize;

/**
    Calls the IPMI send command SMM service for Non SMM case

    @param This Pointer to IPMI Protocol/PPI
    @param NetFunction Net function of the command
    @param LUN Logical Unit number of the command
    @param Command Command to be sent to BMC
    @param CommandData Command data to be sent along with Command
    @param CommandDataSize Command Data size
    @param ResponseData Response Data from BMC
    @param ResponseDataSize Size of the Response data

    @retval EFI_SUCCESS Status returned from BMC while executing the command.

**/

EFI_STATUS
SmmEfiIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT    *This,
  IN UINT8                 NetFunction,
  IN UINT8                 Lun,
  IN UINT8                 Command,
  IN UINT8                 *CommandData,
  IN UINT8                 CommandDataSize,
  OUT UINT8                *ResponseData,
  OUT UINT8                *ResponseDataSize )
{
    EFI_STATUS                                    Status;
    EFI_SMM_COMMUNICATE_HEADER                   *SmmCommunicateHeader;
    SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA   *SmmIpmiBmcData;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...  IpmiInstance: %x \n", __FUNCTION__, gIpmiInstance));

    //
    // Copy SMM Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSmmIpmiTransportProtocolGuid);
    SmmCommunicateHeader->MessageLength = SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA_SIZE;

    //
    // Copy SMM IPMI BMC Communicate Data Here
    //
    SmmIpmiBmcData = (SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmIpmiBmcData->FunctionNumber = SMM_IPMI_BMC_FUNCTION_SEND_COMMAND;
    SmmIpmiBmcData->NetFunction = NetFunction;
    SmmIpmiBmcData->Lun = Lun;
    SmmIpmiBmcData->Command = Command;
    SmmIpmiBmcData->CommandData = (UINTN) CommandData;
    SmmIpmiBmcData->CommandDataSize = CommandDataSize;
    SmmIpmiBmcData->ResponseData = (UINTN) ResponseData;
    SmmIpmiBmcData->ResponseDataSize = (UINTN) ResponseDataSize;

    //
    // Specify some initial value to data's return status
    //
    SmmIpmiBmcData->ReturnStatus = EFI_PROTOCOL_ERROR;

    //
    // Send data to SMM.
    //
    gCommunicateBufferSize = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate (gSmmCommunication, gCommunicateBuffer, &gCommunicateBufferSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gSmmCommunication->Communicate  Status: %r \n", Status));
    ASSERT_EFI_ERROR (Status);

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmIpmiBmcData = (SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
    if (SmmIpmiBmcData->ReturnStatus == EFI_PROTOCOL_ERROR) {
        SmmIpmiBmcData->ReturnStatus = EFI_ABORTED;
    }
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmmIpmiBmcData->ReturnStatus : %r \n", SmmIpmiBmcData->ReturnStatus));

    This->CommandCompletionCode = SmmIpmiBmcData->CommandCompletionCode;

    //
    // Status returned by the SMM based routine.
    //
    return  SmmIpmiBmcData->ReturnStatus;
}

/**
    Calls the IPMI BMC status SMM service for Non SMM case

    @param This Pointer to IPMI Protocol/PPI
    @param BmcStatus State of the BMC
    @param ComAddress BMC Communication address

    @retval EFI_SUCCESS Status returned

**/

EFI_STATUS
SmmEfiIpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT     *This,
  OUT EFI_BMC_STATUS        *BmcStatus,
  OUT EFI_SM_COM_ADDRESS    *ComAddress )
{

    EFI_STATUS                                   Status;
    EFI_SMM_COMMUNICATE_HEADER                   *SmmCommunicateHeader;
    SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA     *SmmIpmiGetBmcStatusData;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...  IpmiInstance: %x \n", __FUNCTION__, gIpmiInstance));

    //
    // Copy SMM Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSmmIpmiTransportProtocolGuid);
    SmmCommunicateHeader->MessageLength = SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA_SIZE;

    //
    // Copy SMM IPMI Get BMC Communicate Data Here
    //
    SmmIpmiGetBmcStatusData = (SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmIpmiGetBmcStatusData->FunctionNumber = SMM_IPMI_BMC_FUNCTION_GET_BMC_STATUS;
    SmmIpmiGetBmcStatusData->BmcStatus = (UINTN) BmcStatus;
    SmmIpmiGetBmcStatusData->ComAddress = (UINTN) ComAddress;

    //
    // Specify some initial value to data's return status
    //
    SmmIpmiGetBmcStatusData->ReturnStatus = EFI_PROTOCOL_ERROR;

    //
    // Send data to SMM.
    //
    gCommunicateBufferSize = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate (gSmmCommunication, gCommunicateBuffer, &gCommunicateBufferSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gSmmCommunication->Communicate  Status: %r \n", Status));
    ASSERT_EFI_ERROR (Status);

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmIpmiGetBmcStatusData = (SMM_IPMI_BMC_GET_STATUS_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;

//APTIOV_SERVER_OVERRIDE_START: Security Vulnerability
    if (SmmIpmiGetBmcStatusData->ReturnStatus == EFI_PROTOCOL_ERROR) {
        SmmIpmiGetBmcStatusData->ReturnStatus = EFI_ABORTED;
    }
//APTIOV_SERVER_OVERRIDE_END: Security Vulnerability

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "SmmIpmiGetBmcStatusData->ReturnStatus : %r \n", SmmIpmiGetBmcStatusData->ReturnStatus));

    //
    // Status returned by the SMM based routine.
    //
    return  SmmIpmiGetBmcStatusData->ReturnStatus;
}

/**
    Notification function for SmmIpmi Transport protocol.
    Performs necessary steps to proxy the DXE calls to SMM

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
SmmIpmiBmcTransportProtocolReady (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context )
{
    EFI_STATUS                    Status;
    EFI_IPMI_TRANSPORT            *DummySmmmIpmiTransport = NULL;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... IpmiInstance: %x \n", __FUNCTION__, gIpmiInstance));
    Status = gBS->LocateProtocol (&gEfiSmmIpmiTransportProtocolGuid, NULL, (VOID **)&DummySmmmIpmiTransport);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBS->LocateProtocol gEfiSmmIpmiTransportProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBS->LocateProtocol gEfiSmmCommunicationProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    //
    // Allocate memory for variable store.
    //
    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_BMC_SEND_COMMAND_COMMUNICATE_DATA_SIZE;
    gCommunicateBuffer      = AllocateRuntimeZeroPool (gCommunicateBufferSize);
    ASSERT (gCommunicateBuffer != NULL);

    //
    // Override SendIpmiCommand and GetBmcStatus entries in IpmiTransport protocol
    //
    gIpmiInstance->IpmiTransport.SendIpmiCommand = SmmEfiIpmiSendCommand;
    gIpmiInstance->IpmiTransport.GetBmcStatus = SmmEfiIpmiBmcStatus;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...  IpmiInstance: %x \n", __FUNCTION__, gIpmiInstance));

    return;
}

/**
    Hook to register Notification function for SmmIpmi Transport protocol.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
InitializeIpmiPhysicalLayerHook (
  VOID )
{
    VOID        *SmmIpmiBmcRegistration;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...  gIpmiInstance: %x \n", __FUNCTION__, gIpmiInstance));

    //
    // Create Notification event for SmmDxeIpmiBmc GUID
    //
    EfiCreateProtocolNotifyEvent (
        &gEfiSmmIpmiTransportProtocolGuid,
        TPL_CALLBACK,
        SmmIpmiBmcTransportProtocolReady,
        NULL,
        &SmmIpmiBmcRegistration
        );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));
    return EFI_SUCCESS;
}

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

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
