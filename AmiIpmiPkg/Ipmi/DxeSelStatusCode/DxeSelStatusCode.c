//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//  
//  Rev. 1.00
//    Bug Fix : Don't execute the Selstatus function when BMC not present.
//    Reason  : Refer from Grantley
//    Auditor : Kasber Chen
//    Date    : Jun/07/2016
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

/** @file DxeSelStatusCode.c
    DXE Driver that provides SEL status code reporting routines

**/

//----------------------------------------------------------------------

#include "DxeSelStatusCode.h"
#include <Include/ServerMgmtSetupVariable.h>

//
//  OEM Status Code to SEL Conversion table.
//  Entries should be added to this table using SDL tokens by OEM to convert to SEL entry.
//
#if (OEM_SEL_SUPPORT == 1)
#include "OemStatusCodeTable.h"
#endif
#if SMCPKG_SUPPORT
#include "SuperMDxeDriver.h"
#endif
//----------------------------------------------------------------------

//
// Module global values
//
EFI_IPMI_TRANSPORT                  *gIpmiTransport;
EFI_SM_SEL_STATUS_CODE_PROTOCOL     gSelProtocol = {0};
BOOLEAN                             gEraseSel;
BOOLEAN                             gLogProgressCode;
BOOLEAN                             gLogErrorCode;

//----------------------------------------------------------------------

//
// Function implementations
//

/**
    Notification function for Virtual address change event.
    Convert all addresses inside gEfiSelStatusCodeProtocol

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
DxeSelStatusCodeVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    //
    // Change all addresses inside gEfiSelStatusCodeProtocol stored locally
    //
    gRT->ConvertPointer (
            0,
            (VOID **) &(gSelProtocol.EfiSelReportStatusCode));

    //
    // Change the address pointed by gIpmiTransport as we are using this at runtime
    //
    gRT->ConvertPointer (
          0,
          (VOID **) &gIpmiTransport );

    return;
}

/**

    This function walks through the table based upon Value field of
    Status Code and extract EFI status code to SEL conversion entry.

    @param EfiStatusCode  - EFI status code Value
    @param StatusCodeType - EFI status code type

    @retval EFI_STATUS_CODE_TO_SEL_ENTRY* Pointer to conversion entry if found.
    @retval NULL if conversion entry is not found.

**/

EFI_STATUS_CODE_TO_SEL_ENTRY *
ConvertEfiStatusCodeToSelEntry (
  IN  EFI_STATUS_CODE_VALUE   EfiStatusCode,
  IN  EFI_STATUS_CODE_TYPE    StatusCodeType )
{
    UINTN         TableSize;
    UINTN         Index;

#if (OEM_SEL_SUPPORT == 1)

    //
    // OEM to SEl Status code List
    //
    TableSize = (sizeof (OemStatusCodeToSelTable) / sizeof (EFI_STATUS_CODE_TO_SEL_ENTRY));

    for (Index = 0; Index < TableSize - 1; Index++) {
        if (EfiStatusCode == OemStatusCodeToSelTable[Index].StatusCode &&\
            StatusCodeType == OemStatusCodeToSelTable[Index].StatusCodeType ) {
            return &OemStatusCodeToSelTable[Index];
        }
    }
#endif

    //
    // EFI to SEl Status code List
    //
    TableSize = (sizeof (EfiStatusCodeToSelTable) / sizeof (EFI_STATUS_CODE_TO_SEL_ENTRY));

    for (Index = 0; Index < TableSize; Index++) {
        if (EfiStatusCode == EfiStatusCodeToSelTable[Index].StatusCode &&\
            StatusCodeType == EfiStatusCodeToSelTable[Index].StatusCodeType ) {
            return &EfiStatusCodeToSelTable[Index];
        }
    }

  return NULL;
}

/**

    This function checks for the erasure status

    @param *ReserveId SEL reservation ID

    @retval EFI_SUCCESS SEL is erased successfully    
    @retval EFI_NO_RESPONSE If time out occurred and still SEL is not erased successfully

**/

EFI_STATUS
WaitTillErased (
  UINT8     *ReserveId )
{
    EFI_STATUS                Status;
    INTN                      Counter = 0x200;
    IPMI_CLEAR_SEL_REQUEST    ClearSelData;
    UINT8                     ClearSelResponse;
    UINT8                     ClearSelResponseDataSize;

    while (TRUE) {
        ClearSelData.Reserve[0] = ReserveId[0];
        ClearSelData.Reserve[1] = ReserveId[1];
        ClearSelData.AscC = C_CHAR_ASCII_VALUE;
        ClearSelData.AscL = L_CHAR_ASCII_VALUE;
        ClearSelData.AscR = R_CHAR_ASCII_VALUE;
        ClearSelData.Erase = GET_ERASE_STATUS;
        ClearSelResponseDataSize = sizeof (ClearSelResponse);

        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_CLEAR_SEL,
                    (UINT8 *) &ClearSelData,
                    sizeof (ClearSelData),
                    (UINT8 *) &ClearSelResponse,
                    (UINT8 *) &ClearSelResponseDataSize);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_CLEAR_SEL Status: %r ClearSelResponse: %x Counter: %x\n", Status, ClearSelResponse, Counter));
        if ((!EFI_ERROR(Status)) && ((ClearSelResponse & 0x0F) == ERASURE_COMPLETED)) {
              return EFI_SUCCESS;
        }
        if ((EFI_ERROR(Status)) && (gIpmiTransport->CommandCompletionCode == INVALID_RESERVATION_ID)) {
           //
           // Get New Reservation Id and process further.
           //
           Status = GetReservationId (ReserveId);
           if (EFI_ERROR(Status)) {
               return Status;
           }
        }
        //
        //  If there is not a response from the BMC controller we need to return and not hang.
        //
        --Counter;
        if (Counter == 0x0) {
            return EFI_NO_RESPONSE;
        }
    }
}

/**

    This function erases the SEL entries.

    @param VOID

    @retval EFI_SUCCESS SEL is erased successfully
    @retval EFI_NO_RESPONSE If time out occurred and still SEL is not erased successfully

**/

EFI_STATUS
EraseSelElogData (
  VOID )
{

    EFI_STATUS                    Status = EFI_SUCCESS;
    IPMI_GET_SEL_INFO_RESPONSE    SelInfo;
    UINT8                         SelInfoSize = sizeof (SelInfo);
    UINT8                         ReserveId[2] = { 0 };
    UINT8                         ReserveIdSize = sizeof (ReserveId);
    IPMI_CLEAR_SEL_REQUEST        ClearSelData;
    UINT8                         ClearSelResponse;
    UINT8                         ClearSelResponseDataSize = sizeof (ClearSelResponse);

    //
    // Before issuing  SEL reservation ID, Check whether this command is supported
    // or not by issuing the GetSelInfoCommand. If it does not supported ResvId should be 0000h
    //
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_GET_SEL_INFO,
                NULL,
                0,
                (UINT8 *) &SelInfo,
                (UINT8 *) &SelInfoSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SEL_INFO Status: %r\n", Status));
    if (EFI_ERROR (Status)) {
    return Status;
    }

    //
    // if SelReserveIdcommand not supported do not issue the RESERVE_SEL_ENTRY command,
    // and set the ResvId value to 0000h else Get the SEL reservation ID
    //
    if (SelInfo.OperationSupport & RESERVE_SEL_COMMAND_SUPPORTED_MASK) {
        //
        // Get Reservation Id value for accessing SEL area.
        //
        Status = GetReservationId (ReserveId);

        if (EFI_ERROR (Status)) {
            return Status;
        }
        ClearSelData.Reserve[0] = ReserveId[0];
        ClearSelData.Reserve[1] = ReserveId[1];
    } else {
        ClearSelData.Reserve[0] = 0x00;
        ClearSelData.Reserve[1] = 0x00;
    }
    //
    // Clear the SEL
    //
    ClearSelData.AscC = C_CHAR_ASCII_VALUE;
    ClearSelData.AscL = L_CHAR_ASCII_VALUE;
    ClearSelData.AscR = R_CHAR_ASCII_VALUE;
    ClearSelData.Erase = INITIALIZE_ERASE;

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_CLEAR_SEL,
                (UINT8 *) &ClearSelData,
                sizeof (ClearSelData),
                (UINT8 *) &ClearSelResponse,
                (UINT8 *) &ClearSelResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_CLEAR_SEL Status: %r ClearSelResponse: %x\n", Status, ClearSelResponse));
    if((!EFI_ERROR(Status)) && ((ClearSelResponse&0x0F) == ERASURE_COMPLETED)) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "DXE: SEL Cleared\n"));
        return EFI_SUCCESS;
    }
    Status =  WaitTillErased (ReserveId);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DXE: WaitTillErased Status : %r\n",Status));
    return Status;
}

/**

    This function verifies if the BMC SEL is full.

    @param VOID

    @retval TRUE If SEL is full
    @retval FALSE If SEL is not full

**/ 
BOOLEAN
IsSelFull (
 VOID )
{
    EFI_STATUS                    Status;
    IPMI_GET_SEL_INFO_RESPONSE    SelInfo;
    UINT8                         SelInfoSize = sizeof (SelInfo);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_GET_SEL_INFO,
                NULL,
                0,
                (UINT8 *) &SelInfo,
                (UINT8 *) &SelInfoSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SEL_INFO Status: %r (SelInfo.OperationSupport & OVERFLOW_FLAG_MASK): %x\n", Status, (SelInfo.OperationSupport & OVERFLOW_FLAG_MASK)));

    if (EFI_ERROR (Status)) {
        return FALSE;
    }

    //
    //If overflow flag is set report SEL is full
    //
    if ((SelInfo.OperationSupport & OVERFLOW_FLAG_MASK) == SEL_OVERFLOW_FLAG_SET) {
      return TRUE;
    }
    return FALSE;

}

/**

    This function fills the SEL record format.

    @param EfiStatusCode  - EFI status code Value
    @param StatusCodeType - EFI status code type
    @param SelRecordData - Pointer to SEL record data

    @retval EFI_SUCCESS
    @retval EFI_DEVICE_ERROR If status code is not found in the conversion table

**/

EFI_STATUS
FillSelRecord (
  IN      EFI_STATUS_CODE_VALUE         Value,
  IN      EFI_STATUS_CODE_TYPE          CodeType,
  IN OUT  IPMI_SEL_EVENT_RECORD_DATA    *SelRecordData )
{
    EFI_STATUS_CODE_TO_SEL_ENTRY         *SelEntry;

    SelEntry = ConvertEfiStatusCodeToSelEntry (Value, CodeType);

    if (SelEntry != NULL) {

        //
        // Check if SEL is full  before logging any entries.If SEL is full and user opts to erase
        // erase it,report error and log , else return without logging
        //
        if (IsSelFull() == TRUE) {
            if (gEraseSel == ERASE_WHEN_SEL_FULL) {
                EraseSelElogData();
            } else {
                return EFI_DEVICE_ERROR;
            }
        }
        SelRecordData->RecordId = 0x00;
        SelRecordData->RecordType = IPMI_SEL_SYSTEM_RECORD;   // Discrete
        SelRecordData->TimeStamp = 0x00;
        SelRecordData->GeneratorId = (UINT16) EFI_GENERATOR_ID (BIOS_SOFTWARE_ID);
        SelRecordData->EvMRevision = IPMI_EVM_REVISION;
        SelRecordData->SensorType = SelEntry->SensorTypeCode;
        SelRecordData->SensorNumber = BIOS_SOFTWARE_ID;
        SelRecordData->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
        SelRecordData->OEMEvData1 = SelEntry->EventData1;
        SelRecordData->OEMEvData2 = SelEntry->EventData2;
        SelRecordData->OEMEvData3 = SelEntry->EventData3;
        return EFI_SUCCESS;
    }
    return EFI_DEVICE_ERROR;
}

/**

    Convert status code value and extended data to BMC SEL record format and
    send the command to log SEL in BMC.

    
    @param This - Pointer to the SEL status code protocol
    @param Value - EFI status code Value
    @param CodeType - EFI status code type
    @param Instance - The enumeration of a hardware or software
                     entity within the system. A system may contain multiple
                     entities that match a class/subclass pairing.
                     The instance differentiates between them. An instance of 0
                     indicates that instance information is unavailable,
                     not meaningful, or not relevant.
                     Valid instance numbers start with 1.
    @param CallerId - This optional parameter may be used to
                      identify the caller.
                      This parameter allows the status code driver to apply different
                      rules to different callers.
    @param Data - This optional parameter may be used to pass additional data.

    @return EFI_STATUS Return Status

**/

EFI_STATUS
EFIAPI
EfiSelReportStatusCode (
  IN EFI_SM_SEL_STATUS_CODE_PROTOCOL    *This,
  IN EFI_STATUS_CODE_TYPE               CodeType,
  IN EFI_STATUS_CODE_VALUE              Value,
  IN UINT32                             Instance,
  IN EFI_GUID                           *CallerId,
  IN EFI_STATUS_CODE_DATA               *Data OPTIONAL )
{

    EFI_STATUS                  Status;
    IPMI_SEL_EVENT_RECORD_DATA  SelRecordData;
    UINT8                       ResponseDataSize;
    UINT16                      RecordId = 0;

#if USE_PLATFORM_EVENT_MESSAGE
    IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST         EventMessage;
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... Value: %x \n", __FUNCTION__, Value));

    //
    //Check if logging only progress code,only error code or both is opted by user
    //
    switch (CodeType & EFI_STATUS_CODE_TYPE_MASK) {
        case EFI_PROGRESS_CODE:
            if (!gLogProgressCode) {
                return EFI_SUCCESS;
            }
            break;
        case EFI_ERROR_CODE:
            if (!gLogErrorCode) {
                return EFI_SUCCESS;
            }
            break;
        default:
            break;
    }

    //
    //Check if status code to equivalent SEL entry found in table,If not found return.
    //
    Status = FillSelRecord ( Value, CodeType & EFI_STATUS_CODE_TYPE_MASK, &SelRecordData);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " FillSelRecord Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return EFI_SUCCESS;
    }

#if USE_PLATFORM_EVENT_MESSAGE
    EventMessage.GeneratorId = EFI_GENERATOR_ID (BIOS_SOFTWARE_ID);
    EventMessage.EvMRevision = SelRecordData.EvMRevision;
    EventMessage.SensorType = SelRecordData.SensorType;
    EventMessage.SensorNumber = SelRecordData.SensorNumber;
    EventMessage.EventDirType = SelRecordData.EventDirType;
    EventMessage.OEMEvData1 = SelRecordData.OEMEvData1;
    EventMessage.OEMEvData2 = SelRecordData.OEMEvData2;
    EventMessage.OEMEvData3 = SelRecordData.OEMEvData3;
    ResponseDataSize = 0;

    Status = gIpmiTransport-> SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_SENSOR_EVENT,
                BMC_LUN,
                IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
                (UINT8 *) &EventMessage,
                sizeof (EventMessage),
                NULL,
                (UINT8 *) &ResponseDataSize );

#else
    ResponseDataSize = sizeof (RecordId);
    Status = gIpmiTransport-> SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_ADD_SEL_ENTRY,
                (UINT8 *) &SelRecordData,
                sizeof (SelRecordData),
                (UINT8 *) &RecordId,
                &ResponseDataSize);
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gIpmiTransport-> SendIpmiCommand Status: %r \n", Status));
    return Status;
}

/**

    Calls SEL Report status code function to log SEL in BMC

    @param Value    - EFI status code Value
    @param CodeType - EFI status code type
    @param Instance - The enumeration of a hardware or software entity 
                      within the system. A system may contain multiple entities that
                      match a class/subclass pairing. The instance differentiates
                      between them. An instance of 0 indicates that instance
                      information is unavailable, not meaningful, or not relevant.
                      Valid instance numbers start with 1.
    @param CallerId - This optional parameter may be used to identify the caller.
                      This parameter allows the status code driver to apply different
                      rules to different callers.
    @param Data - This optional parameter may be used to pass additional data.

    @return EFI_STATUS
    @retval EFI_SUCCESS Status code reported to BMC successfully.

**/

EFI_STATUS
EFIAPI
DxeSelStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL )
{

    if ( ( ( CodeType & EFI_STATUS_CODE_TYPE_MASK ) == EFI_PROGRESS_CODE ) ||
         ( ( CodeType & EFI_STATUS_CODE_TYPE_MASK ) == EFI_ERROR_CODE ) ) {

        //
        // Check if status code to equivalent SEL entry found in table,If not found return.
        //
        if ( ConvertEfiStatusCodeToSelEntry (Value, CodeType & EFI_STATUS_CODE_TYPE_MASK) == NULL ) {
            return EFI_NOT_FOUND;
        }
        return gSelProtocol.EfiSelReportStatusCode(&gSelProtocol,CodeType, Value, 0, NULL, NULL);
    }

    return EFI_SUCCESS;
}

/**

    This is the standard EFI driver Entry point function. This function initializes the private
    data required for creating SEL Driver.

         
    @param ImageHandle Handle for the image of this driver
    @param SystemTable Pointer to the EFI System Table

    @retval EFI_SUCCESS Protocol successfully started and installed
    @retval EFI_UNSUPPORTED Protocol can't be started

**/

EFI_STATUS
InitializeDxeSel (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable )
{
    EFI_HANDLE                      NewHandle = NULL;
    EFI_STATUS                      Status = EFI_SUCCESS;
    SERVER_MGMT_CONFIGURATION_DATA  ServerMgmtConfiguration;
    UINTN                           Size;
    UINT32                          Attributes;
    EFI_RSC_HANDLER_PROTOCOL        *RscHandlerProtocol = NULL;
    EFI_EVENT                       EfiVirtualAddressChangeNotifyEvent = NULL;
#if SMCPKG_SUPPORT
    EFI_GUID  gEfiSuperMDriverProtocolGuid = EFI_SUPERM_DRIVER_PROTOCOL_GUID;
    SUPERMDRIVER_PROTOCOL	*gSuperMProtocol;
    SUPERMBOARDINFO		gSuperMBoardInfo;
#endif
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));


#if SMCPKG_SUPPORT
    Status = gBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);

    Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);

    if(gSuperMBoardInfo.BMC_Present == 0)
	return EFI_SUCCESS;
#endif
    
    //
    // Attempt to locate the IPMI protocol. If it is not found return status, SEL Reporting cannot be
    // supported, so do not install protocol.
    //
    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gRT->GetVariable (
                L"ServerSetup",
                &gEfiServerMgmtSetupVariableGuid,
                &Attributes,
                &Size,
                &ServerMgmtConfiguration);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiServerMgmtSetupVariableGuid Status: %r \n", Status));
    if (EFI_ERROR(Status)) {

        //
        //load default value.
        //
        ServerMgmtConfiguration.SelLogging    = SEL_LOGGING_ENABLED;
        ServerMgmtConfiguration.SelFull       = DO_NOTHING_WHEN_SEL_FULL;
        ServerMgmtConfiguration.SelErase      = DO_NOT_ERASE;
        ServerMgmtConfiguration.LogSelStatusCodes = LOG_ERROR_CODE_ONLY;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, " ServerMgmtConfiguration.SelLogging: %X \n", ServerMgmtConfiguration.SelLogging));

    //
    // Check if SEL components enabled in setup,if not enabled don't install the protocol
    //
    if ( ServerMgmtConfiguration.SelLogging == SEL_LOGGING_DISABLED ) {
        return EFI_UNSUPPORTED;
    }

    //
    // Erase Sel.Check if Erase on every boot?If not NVRAM Variable is to be set.
    //
    if ( (ServerMgmtConfiguration.SelErase == ERASE_NEXT_BOOT) || (ServerMgmtConfiguration.SelErase == ERASE_EVERY_BOOT) ) {
        EraseSelElogData();
        if (ServerMgmtConfiguration.SelErase == ERASE_NEXT_BOOT) {
            ServerMgmtConfiguration.SelErase = DO_NOT_ERASE;

            SERVER_IPMI_DEBUG ((EFI_D_INFO, " ServerMgmtConfiguration.SelErase: %X \n", ServerMgmtConfiguration.SelErase));
            Status = gRT->SetVariable (
                        L"ServerSetup",
                        &gEfiServerMgmtSetupVariableGuid,
                        Attributes,   // Attributes
                        Size,
                        &ServerMgmtConfiguration);
            ASSERT_EFI_ERROR (Status);
        }
    }

    //
    // Set Global variables according to user options for setup question, when SEL is full.
    // if user opts to erase when SEL is full, erase it
    //
    gEraseSel = ServerMgmtConfiguration.SelFull;
    if ( (gEraseSel == ERASE_WHEN_SEL_FULL) && (IsSelFull() == TRUE) ) {
        EraseSelElogData();
    }

    //
    // Check if Logging of status codes is enabled in setup, if not enabled don't install the protocol
    //
    if ( ServerMgmtConfiguration.LogSelStatusCodes == LOG_SEL_STATUSCODE_DISABLED ) {
        return EFI_UNSUPPORTED;
    }

    //
    // Check if logging only progress code,only error code or both is opted by user
    //
    gLogProgressCode = (ServerMgmtConfiguration.LogSelStatusCodes == LOG_BOTH_PROGRESS_AND_ERROR_CODES) || \
                       (ServerMgmtConfiguration.LogSelStatusCodes == LOG_PROGRESS_CODE_ONLY);
    gLogErrorCode = (ServerMgmtConfiguration.LogSelStatusCodes == LOG_BOTH_PROGRESS_AND_ERROR_CODES) || \
                    (ServerMgmtConfiguration.LogSelStatusCodes == LOG_ERROR_CODE_ONLY);

    //
    // Initialize Global variables
    //
    gSelProtocol.EfiSelReportStatusCode = EfiSelReportStatusCode;

    Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gEfiSelStatusCodeProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &gSelProtocol);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiSelStatusCodeProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Create event for the Virtual Address Change Event
    //
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                DxeSelStatusCodeVirtualAddressChangeEvent,
                NULL,
                (CONST EFI_GUID *)&gEfiEventVirtualAddressChangeGuid,
                &EfiVirtualAddressChangeNotifyEvent );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Create event for the Virtual Address Change Event status %r\n", Status));
    if (EFI_ERROR (Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Create event is failing, So returning without registering DxeSelStatusCodeReportWorker handler\n"));
        return Status;
    }

    Status = gBS->LocateProtocol (
                &gEfiRscHandlerProtocolGuid,
                NULL,
                (VOID **) &RscHandlerProtocol);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiRscHandlerProtocolGuid Status: %r \n", Status));
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR (Status)) {
        Status = RscHandlerProtocol->Register (DxeSelStatusCodeReportWorker, TPL_HIGH_LEVEL);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, " RscHandlerProtocol->Register Status: %r \n", Status));
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));

    return EFI_SUCCESS;
}
/**
    Get Reservation Id to access SEL area. 
 
    @param ReserveId - Reservation Id obtained.

    @return EFI_STATUS Return Status
    @retval EFI_SUCCESS Reservation Id is obtained successfully.

**/

EFI_STATUS
GetReservationId(
  IN OUT UINT8* ReserveId )
{
    EFI_STATUS Status;
    UINT8 ReserveIdSize = sizeof (UINT16);

    Status = gIpmiTransport->SendIpmiCommand (
                              gIpmiTransport,
                              IPMI_NETFN_STORAGE,
                              BMC_LUN,
                              IPMI_STORAGE_RESERVE_SEL,
                              NULL,
                              0,
                              ReserveId,
                              (UINT8 *) &ReserveIdSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "DXE: IPMI_STORAGE_RESERVE_SEL Status: %r ReserveId: %4x \n", Status, *((UINT16*)ReserveId)));
    return Status;
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
