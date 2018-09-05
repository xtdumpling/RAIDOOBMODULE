//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  SmmGenericElog.c
//
// Description:	  SMM Generic Elog Driver.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "GenericElog.h"
#include <Library/SmmServicesTableLib.h>

//
// Module Global variables
//
GENERIC_ELOG_LOCAL_STRUCT 		*gElogLocalStructure = NULL;

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: RebuildLinksToRedirects
//
// Description:
//	Collects all the Redir Elog protocols and stores in Elog locally maintained
//	private structure
//
// Input:
//	  None
//
// Output:
//	  None
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
RebuildLinksToRedirects (
  VOID )
{
	EFI_STATUS					Status;
	UINTN						NumberOfRedirects;
	EFI_HANDLE					*HandleBuffer = NULL;
	UINTN 						HandleBufferSize = 0;
	UINTN						HandleCount = 0;
	EFI_SM_ELOG_REDIR_PROTOCOL	*RedirectProtocol;

    gElogLocalStructure->NumberOfRedirects = 0;

    //
    // Clear all Redir log pointers in the gElogLocalStructure->RedirectProtocolList[]
    //
    ZeroMem (&gElogLocalStructure->RedirectProtocolList[0], \
    		EfiSmElogMax*sizeof (gElogLocalStructure->RedirectProtocolList[0]));

	//
	// Get all Redir Elog handles.
	//
	Status = gSmst->SmmLocateHandle (
				ByProtocol,
				&gEfiRedirElogProtocolGuid,
				NULL,
				&HandleBufferSize,
				HandleBuffer );
	DEBUG ((EFI_D_LOAD, " SmmLocateHandle: gEfiRedirElogProtocolGuid Status: %r HandleBufferSize: %x \n", Status, HandleBufferSize));
	if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
		return;
	}

	//
	// Allocate memory for Handle buffer
	//
	HandleBuffer = AllocatePool (HandleBufferSize);
	if (HandleBuffer == NULL) {
		return;
	}

	Status = gSmst->SmmLocateHandle (
				ByProtocol,
				&gEfiRedirElogProtocolGuid,
				NULL,
				&HandleBufferSize,
				HandleBuffer );
	DEBUG ((EFI_D_LOAD, " SmmLocateHandle: gEfiRedirElogProtocolGuid Status: %r HandleBufferSize: %x \n", Status, HandleBufferSize));

	if (Status != EFI_SUCCESS) {
		goto END;
	}

    NumberOfRedirects = HandleBufferSize/sizeof(EFI_HANDLE);
	DEBUG ((EFI_D_LOAD, " NumberOfRedirects: %x \n", NumberOfRedirects));

    if (NumberOfRedirects > EfiSmElogMax) {
		goto END;
	}

	HandleCount = NumberOfRedirects;
	while (HandleCount > 0) {

		Status = gSmst->SmmHandleProtocol (
					(EFI_SM_ELOG_PROTOCOL*)(HandleBuffer[HandleCount-1]),
					&gEfiRedirElogProtocolGuid,
					(VOID*)&RedirectProtocol );

		if ( (Status == EFI_SUCCESS) && (RedirectProtocol != NULL) ) {
			gElogLocalStructure->RedirectProtocolList[NumberOfRedirects-HandleCount] = RedirectProtocol;
			gElogLocalStructure->NumberOfRedirects++;
		}

		HandleCount--;
	}

END:

	if (HandleBuffer != NULL) {
		FreePool (HandleBuffer);
	}

    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  RedirElogProtocolNotificationFunction
//
// Description:
//	Notification function for Redir Elog protocol
//  Collects all the Redir Elog protocols.
//
// Input:
//   EFI_GUID	*Protocol - Points to the protocol's unique identifier.
//   VOID       *Interface - Points to the interface instance.
//   EFI_HANDLE Handle - The handle on which the interface was installed.
// Output:
//   None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
RedirElogProtocolNotificationFunction (
  IN CONST EFI_GUID  		*Protocol,
  IN       VOID            	*Interface,
  IN       EFI_HANDLE      	Handle )
{
	//
	// Rebuild all links to known redirects
	//
	RebuildLinksToRedirects();
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GenericElogSetData
//
// Description:
//	  Sends the Event-Log data to the destination. Destination can be a remote
//		target like LAN, ICMB, local targets BMC,IPMB or a Firmware-Volume.
//		A Redirect Protocol Driver resolves final destination .
//
// Input:
//	EFI_SM_ELOG_PROTOCOL	*This - Instance of EFI_SM_ELOG_PROTOCOL
//	UINT8	*ElogData - Pointer to the Event-Log data that needs to be recorded.
//	EFI_SM_ELOG_TYPE	DataType - Type of Elog Data that is being recorded.
//						Elog is redirected based upon this information.
//	BOOLEAN	AlertEvent - This is an indication that the input data type is an Alert.
//				The underlying drivers to decide if they need to listen to the
//				DataType and send it on an appropriate channel as an alert
//				use this information. Any Alert event logs are not
//				guaranteed to be retrieved or erased.
//	UINTN	Size - Size of the data to be logged.
//	UINT64	*RecordId - This is the array of record IDs sent by the target.
//			This can be used to retrieve the records or erase the records.
//
// Output:
//     EFI_STATUS - Return Status
//		EFI_SUCCESS     		- Event-Log was recorded successfully
//		EFI_NOT_FOUND   		- Event-Log target not found.
//		EFI_NO_RESPONSE    		- Event-Log Target not responding.
//		EFI_PROTOCOL_ERROR 		- Data Format Error found
//		EFI_INVALID_PARAMETER 	- Invalid Parameter
//		EFI_OUT_OF_RESOURCES  	- Not enough resources to record data.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GenericElogSetData (
  IN  EFI_SM_ELOG_PROTOCOL        		*This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,	
  IN  UINTN                             Size,
  OUT UINT64                            *RecordId )
{
	EFI_STATUS					Status;
	UINTN 						Count = 0;
	GENERIC_ELOG_LOCAL_STRUCT*	LocalDataPtr = (GENERIC_ELOG_LOCAL_STRUCT*)This;

	if (DataType >= EfiSmElogMax) {
		return EFI_INVALID_PARAMETER;
	}

	while (Count < LocalDataPtr->NumberOfRedirects) {
		Status = (LocalDataPtr->RedirectProtocolList[Count])->SetEventLogData(
					LocalDataPtr->RedirectProtocolList[Count],
					ElogData,
					DataType,
					AlertEvent,
					Size,
					RecordId );

		if (Status != EFI_NOT_FOUND) {
			return Status;
		}

		Count++;
	}

	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GenericElogGetData
//
// Description:
//	  Gets the Event-Log data from the destination. Destination can be a remote
//		target like LAN, ICMB, local targets BMC,IPMB or a Firmware-Volume.
//		A Redirect Protocol Driver resolves final destination .
//
// Input:
//	EFI_SM_ELOG_PROTOCOL	*This - Instance of EFI_SM_ELOG_PROTOCOL
//	UINT8	*ElogData - Pointer to the Event-Log data buffer that will contain
//						the data to be retrieved.
//	EFI_SM_ELOG_TYPE	DataType - Type of Elog Data that is being recorded.
//						Elog is redirected based upon this information.
//	BOOLEAN	AlertEvent - This is an indication that the input data type is an Alert.
//				The underlying drivers to decide if they need to listen to the
//				DataType and send it on an appropriate channel as an alert
//				use this information. Any Alert event logs are not
//				guaranteed to be retrieved or erased.
//	UINTN	Size - Size of the data to be retrieved. This size should be equal to
//			or more than the size of data to be retrieved. On return, it
//			contains the actual data retrieved.
//	UINT64	*RecordId - This is the RecordId of the next record. If ElogData is
//				NULL, this gives the RecordId of the first record available
//				in the database with the correct DataSize. A value of 0 on
//				return indicates the last record if the EFI_STATUS
//				indicates a success
//
// Output:
//     EFI_STATUS - Return Status
//		EFI_SUCCESS     		- Event-Log was retrieved successfully
//		EFI_NOT_FOUND   		- Event-Log target not found.
//		EFI_NO_RESPONSE    		- Event-Log Target not responding.
//		EFI_INVALID_PARAMETER 	- Invalid Parameter
//		EFI_BUFFER_TOO_SMALL   	- Target buffer is too small to retrieve the data.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GenericElogGetData(
  IN EFI_SM_ELOG_PROTOCOL         		*This,
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_TYPE                   DataType,	
  IN OUT UINTN                          *Size,
  IN OUT UINT64                         *RecordId )
{
	EFI_STATUS					Status;
	UINTN 						Count = 0;
	GENERIC_ELOG_LOCAL_STRUCT*	LocalDataPtr = (GENERIC_ELOG_LOCAL_STRUCT*)This;

	if (DataType >= EfiSmElogMax) {
		return EFI_INVALID_PARAMETER;
	}

	while (Count < LocalDataPtr->NumberOfRedirects) {
		Status = (LocalDataPtr->RedirectProtocolList[Count])->GetEventLogData(
					LocalDataPtr->RedirectProtocolList[Count],
					ElogData,
					DataType,
					Size,
					RecordId );

		if (Status != EFI_NOT_FOUND) {
			return Status;
		}
		Count++;
	}

	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GenericElogEraseData
//
// Description:
//	  Erases the Event-Log data from the destination. Destination can be a remote
//		target like LAN, ICMB, local targets BMC,IPMB or a Firmware-Volume. A
//		Redirect Protocol Driver resolves final destination .
//
// Input:
//	EFI_SM_ELOG_PROTOCOL	*This - Instance of EFI_SM_ELOG_PROTOCOL
//	EFI_SM_ELOG_TYPE	DataType - Type of Elog Data that is being Erased.
//						Elog is redirected based upon this information.
//	UINT64	*RecordId - This is the RecordId of the data to be erased.
//				If RecordId is NULL, all the records on the database are
//				erased if permitted by the target. Contains the deleted
//				RecordId on return.
//
// Output:
//     EFI_STATUS - Return Status
//		EFI_SUCCESS     		- Event-Log was Erased successfully
//		EFI_NOT_FOUND   		- Event-Log target not found.
//		EFI_NO_RESPONSE    		- Event-Log Target not responding.
//		EFI_INVALID_PARAMETER 	- Invalid Parameter
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GenericElogEraseData (
  IN EFI_SM_ELOG_PROTOCOL         		*This,
  IN EFI_SM_ELOG_TYPE                   DataType,	
  IN OUT UINT64                         *RecordId )
{
	EFI_STATUS					Status;
	UINTN 						Count = 0;
	GENERIC_ELOG_LOCAL_STRUCT*	LocalDataPtr = (GENERIC_ELOG_LOCAL_STRUCT*)This;

	if (DataType >= EfiSmElogMax) {
		return EFI_INVALID_PARAMETER;
	}

	while (Count < LocalDataPtr->NumberOfRedirects) {
		Status = (LocalDataPtr->RedirectProtocolList[Count])->EraseEventLogData(
					LocalDataPtr->RedirectProtocolList[Count],
					DataType,
					RecordId );

		if (Status != EFI_NOT_FOUND) {
			return Status;
		}

		Count++;
	}

	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: GenericElogActivateData
//
// Description:
//	  This API enables/Disables Event Log.
//
// Input:
//	EFI_SM_ELOG_PROTOCOL	*This - Instance of EFI_SM_ELOG_PROTOCOL
//	EFI_SM_ELOG_TYPE	DataType -Type of Elog Data that is being Activated.
//		Activation (Enable/Disable) is redirected based upon this information.
//	BOOLEAN	*EnableElog - Enables (TRUE)/Disables(FALSE) Event Log. If NULL
//				just returns the Current ElogStatus.
//	BOOLEAN	*ElogStatus - Current (New) Status of Event Log. Enabled (TRUE),
//							Disabled (FALSE).
//
// Output:
//     EFI_STATUS - Return Status
//		EFI_SUCCESS     		- Event-Log was activated successfully
//		EFI_NOT_FOUND   		- Event-Log target not found.
//		EFI_NO_RESPONSE    		- Event-Log Target not responding.
//		EFI_INVALID_PARAMETER 	- Invalid Parameter
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GenericElogActivateData (
  IN EFI_SM_ELOG_PROTOCOL         		*This,
  IN  EFI_SM_ELOG_TYPE                  DataType,	
  IN BOOLEAN                            *EnableElog,	
  OUT BOOLEAN                           *ElogStatus	)
{
	EFI_STATUS					Status;
	UINTN 						Count = 0;
	GENERIC_ELOG_LOCAL_STRUCT*	LocalDataPtr = (GENERIC_ELOG_LOCAL_STRUCT*)This;

	if (DataType >= EfiSmElogMax) {
		return EFI_INVALID_PARAMETER;
	}

	while (Count < LocalDataPtr->NumberOfRedirects) {
		Status = (LocalDataPtr->RedirectProtocolList[Count])->ActivateEventLog(
					LocalDataPtr->RedirectProtocolList[Count],
					DataType,
					EnableElog,
					ElogStatus );

		if (Status != EFI_NOT_FOUND) {
			return Status;
		}

		Count++;
	}

	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: InitializeSmmGenericElog
//
// Description:
//	The entry point for SMM Generic Elog Driver
//	1. Allocate memory for Generic Elog Protocol private data
//	2. Install the generic Elog protocol
//	3. Register Notification function for Redir Elog protocol
//
// Input:
// 	ImageHandle     - Handle for the image of this driver
//  SystemTable     - Pointer to the EFI System Table
//
// Output:
//      EFI_STATUS
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InitializeSmmGenericElog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{

    EFI_STATUS              Status;
    VOID      				*GenericElogProtocol = NULL;
    VOID        			*RedirElogNotifyRegistration = NULL;
    EFI_HANDLE				NewHandle;

	//
    // Allocate memory for Generic Elog Protocol private data
    //
    gElogLocalStructure = AllocateRuntimeZeroPool (sizeof (GENERIC_ELOG_LOCAL_STRUCT));
    if (gElogLocalStructure == NULL) {
        DEBUG ((EFI_D_LOAD, "Failed to allocate memory for gElogLocalStructure! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Fill the Generic Elog Protocol data
    //
    gElogLocalStructure->GenericElogProtocol.ActivateEventLog  = GenericElogActivateData;
    gElogLocalStructure->GenericElogProtocol.EraseEventLogData = GenericElogEraseData;
    gElogLocalStructure->GenericElogProtocol.GetEventLogData   = GenericElogGetData;
    gElogLocalStructure->GenericElogProtocol.SetEventLogData   = GenericElogSetData;

    //
    // Install the protocol
    //
    NewHandle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
				&NewHandle,
				&gEfiGenericElogProtocolGuid,
				EFI_NATIVE_INTERFACE,
				&gElogLocalStructure->GenericElogProtocol );
    DEBUG ((EFI_D_LOAD, "gEfiGenericElogProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
    	//
    	// Error in installing the protocol. So free the allocated memory.
    	//
    	FreePool (gElogLocalStructure);
        return Status;
    }

    //
    // Create Notification event for RedirElogProtocol GUID
    //
    Status = gSmst->SmmRegisterProtocolNotify (
				&gEfiRedirElogProtocolGuid,
				RedirElogProtocolNotificationFunction,
				&RedirElogNotifyRegistration );
    DEBUG ((EFI_D_LOAD, "Create Notification event for RedirElogProtocol GUID...Status: %r \n", Status));
    //
    // Collect all Redir protocol instances if any
    //
    RedirElogProtocolNotificationFunction (NULL, NULL, NULL);

    //
    //  Install the empty Protocol Interface in the Boot Time Space. This is required to satisfy the
    //  dependency within the drivers that are dependent upon SMM GenericElog Driver.
    //
    NewHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gEfiGenericElogSmmProtocolInstallled,
                EFI_NATIVE_INTERFACE,
                NULL );
    DEBUG ((EFI_D_LOAD, "gBS->Install: gEfiGenericElogSmmProtocolInstallled protocol  status %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
