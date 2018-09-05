//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2014, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//  Rev. 1.04
//    Bug Fix : Add code to log CMOS Battery Low event into SMBIOS(POST Error Type 08h).
//    Reason  : 
//    Auditor : Jimmy Chiu
//    Date    : Feb/23/2017
//
//  Rev. 1.03
//    Bug Fix :fixed Multi event time count does not follow spec.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Dec/21/2016
//   
//  Rev. 1.02
//    Bug Fix :  Let MECI (Multiple Event Count),METW (Multiple Event Time Window) can work with Memory correct error report and Pcie error report.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Aug/23/2016
//
//  Rev. 1.01
//    Bug Fix : Update the DIMM number of Socket.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jul/7/2016
//
//  Rev. 1.00
//    Bug Fix : Add ECC/PCI SMBIOS event log function.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jun/17/2016
//  
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

/** @file SmbiosElogRedirFuncs.c
    Smbios Elog Redirect Functions Implementation

**/

//---------------------------------------------------------------------------

#include "SmbiosElog.h"

#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01

//The follow is from PurleyPlatPkg\Ras\RuntimeErrorlog\Board\Gpnv\RtErrorLogGpnv.h
#pragma pack(push, 1)
typedef struct {
    UINT32 Correctable:1;
    UINT32 UnCorrectable:1;
    UINT32 DimmNum:2;       // DIMM 0-3
    UINT32 Socket:2;        // Socket 0-3
    UINT32 MemoryController:1; // MemoryController 0-1
    UINT32 Channel:2;       // Channel 0-3
    UINT32 Reserved:23;
} NB_ECC_ERR;

typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    UINT16 DeviceStatus;
    UINT32 Correctable:1;
    UINT32 NonFatal:1;
    UINT32 Fatal:1;
    UINT32 ParityErr:1;  // PERR
    UINT32 SystemErr:1;  // SERR
    UINT32 Reserved:27;
} NB_PCIE_ERR;

typedef union {
    NB_ECC_ERR   EccMsg;
    NB_PCIE_ERR  PcieMsg;
} ERROR_MSG;

typedef enum {
    NO_ERROR = 0,
    ECC_ERROR,
    NB_PCIE_ERROR,
    SB_PCIE_ERROR,
    SB_PCI_ERROR,
    ERROR_MAX
} HARDWARE_ID;

typedef struct _NB_ERROR_INFO {
    EFI_GPNV_RUNTIME_ERROR  Header;     // standard header ...error type
    HARDWARE_ID             HardwareId; // identify the hardware
    ERROR_MSG               Msg;        // hardware-specific error information
} ERROR_INFO;
#pragma pack(pop)

extern  UINT16                            *Type17;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<


//---------------------------------------------------------------------------

extern BOOLEAN  gIsMultiEvent;
extern UINT16   gMultiEventIndex;

// Adding Activate Event support

BOOLEAN gActivateEventLog = TRUE;

/**

    Sends the Event-Log data to the destination.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param ElogData - Pointer to the Event-Log data to be recorded.
    @param DataType - Type of Elog Data that is being recorded.
    @param AlertEvent - This is an indication that the input data type is an 
                        Alert.
    @param Size - Size of the data to be logged.
    @param RecordId - Record ID sent by the target.

    @return EFI_STATUS
    @retval EFI_SUCCESS - To indicate successful completion or valid EFI 
                          error code otherwise.
    @note  N/A

**/

EFI_STATUS
EfiSetSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL  *This,
    IN  UINT8                       *ElogData,
    IN  EFI_SM_ELOG_REDIR_TYPE      DataType,
    IN  BOOLEAN                     AlertEvent,
    IN  UINTN                       Size,
    OUT UINT64                      *RecordId )
{
    EFI_SMBIOS_ELOG_INSTANCE_DATA   *SmbiosElogPrivateData;
    EFI_GPNV_RUNTIME_ERROR          GpnvRuntimeInputStructure;
    EFI_GPNV_ERROR_EVENT            EventToWrite;
    EFI_STATUS                      Status;
    UINT8                           VariableDataType=0xff;
    UINT8                           *NewElogData;
    UINT8                           NewElogDataBuffer[0x7F];
    UINT8                           EventExtendedData[0x7F];
    EFI_STATUS_CODE_ELOG_INPUT      StatusCodeInputStructure;
    UINT16                          POSTBit;
    ALL_GPNV_ERROR                  *Event;
    ALL_EVENT_TYPE_DATA             *Data;
    UINT32                          EventOffset;
// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01    
    ERROR_INFO       *ErrorInfo = (ERROR_INFO*)ElogData;
    UINT8            Dimm = 0;
    UINT16           TempData16 = 0;
    UINT8            i;
    UINT32           SmcEfiToLegacyTable[] = SMC_EFI_TO_LEGACY_STATUS_CODE;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<

    SmbiosElogPrivateData = (EFI_SMBIOS_ELOG_INSTANCE_DATA*)This;

// Ensure Info Being Passed in is of Type Smbios

    if (DataType != EfiElogRedirSmSMBIOS) {
        return EFI_NOT_FOUND;
    }

// If ActivateEventLog is False then return EFI_UNSUPPORTED.

    if (gActivateEventLog == FALSE) {
       return EFI_UNSUPPORTED;
    }

// Check whether valid pointers are passed or not.

    if (ElogData == NULL || RecordId == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    NewElogData = &NewElogDataBuffer[0];
    FillEventWithTimeAndDefaults(&EventToWrite);

    if (*ElogData == EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME) {
        GpnvRuntimeInputStructure = *((EFI_GPNV_RUNTIME_ERROR*)ElogData);
        EventToWrite.EventType = GpnvRuntimeInputStructure.EventLogType;
        NewElogData = ElogData;
    } else {
        if (*ElogData != EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE || 
            mRedirSmbiosPrivate->ConvertEfiToLegacyFlag == FALSE ) {

// If Settings require conversion from EFI to Legacy, do it here
// Exits function with NewElogData as address to info as if it was
// passed into EfiSetSmbiosElogData originally as version desired

            NewElogData = ElogData;
	    
#if SMCPKG_SUPPORT
            //Patch for ConvertEfiToLegacyFlag == False
            StatusCodeInputStructure = *((EFI_STATUS_CODE_ELOG_INPUT*)ElogData);
            for(i=0; i<((sizeof(SmcEfiToLegacyTable)/sizeof(UINT32))-1); i++){
                if(StatusCodeInputStructure.StatusCodeValue == SmcEfiToLegacyTable[i]){
                    StatusCodeInputStructure = *((EFI_STATUS_CODE_ELOG_INPUT*)ElogData);
                    ConvertEfiToLegacy(StatusCodeInputStructure.StatusCodeValue, (LEGACY_CODE_STRUCTURE*)NewElogData);
                    if (*NewElogData == 0xFF) {
                        NewElogData = ElogData;
                    }
	        }
            }
#endif
        } else {
            StatusCodeInputStructure = *((EFI_STATUS_CODE_ELOG_INPUT*)ElogData);
            ConvertEfiToLegacy(StatusCodeInputStructure.StatusCodeValue,
                               (LEGACY_CODE_STRUCTURE*)NewElogData);

            if (*NewElogData == 0xFF) {
                NewElogData = ElogData;
            }
        }

// If after conversion stage, is still in EFI status code form and we
// are not logging status codes, set record Id as highest possible  
// and exit

        if (*NewElogData == EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE && 
            !mRedirSmbiosPrivate->EfiStatusCodesEnabled ) {
            *RecordId = 0xFFFFFFFFFFFFFFFF;
            return EFI_SUCCESS;
        }

// Create Event with proper time and event type

        EventToWrite.EventType = *NewElogData;

    }


// Returning Unsupported for following Event Types.
// EventType = 0x0 and EventType = 0x0F as they are reserved and
// EventTypes from 0x18 to 0x7f as they are not defined in SMBIOS Spec.

    if ((EventToWrite.EventType == 0x0) || 
        (EventToWrite.EventType == 0x0F) || 
        ((EventToWrite.EventType > 0x17) && (EventToWrite.EventType < 0x80))) {
       return EFI_UNSUPPORTED;
    }

// Find Variable Data Type for this EventType

    Status = GetEventVariableDataType (EventToWrite.EventType, &VariableDataType);

    if (EFI_ERROR(Status)) {
        return Status;
    }

// clear extended data

    SetMem ((VOID *)(UINTN)&EventExtendedData, 0x7F, 0);

// Use Variable Data Type to fill in length and extended data

    Event = (ALL_GPNV_ERROR*)&EventToWrite;
    EventToWrite.Length = GetEventRecordSize(Event, VariableDataType);

    Data = (ALL_EVENT_TYPE_DATA*)&EventExtendedData[0];
    switch (VariableDataType) {
      case EFI_EVENT_LOG_VAR_DATA_TYPE_NONE:
        break; // There is no extended data to update here.

      case EFI_EVENT_LOG_VAR_DATA_TYPE_HANDLE:
        Data->Handle.VariableDataType = VariableDataType;
        Data->Handle.SmbiosHandle = 
            ((GPNV_ERROR_HANDLE*)ElogData)->SmbiosHandle;
        break;

      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT:
        Data->Me.VariableDataType = VariableDataType;
        Data->Me.Counter = 0xFFFFFFFF;
        break;

      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE:
        Data->MeHandle.VariableDataType = VariableDataType;
        Data->MeHandle.SmbiosHandle = 
            ((GPNV_ERROR_HANDLE*)ElogData)->SmbiosHandle;
        Data->MeHandle.Counter = 0xFFFFFFFF;
// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
       
        switch (ErrorInfo->HardwareId) {
          case ECC_ERROR:
            Dimm = (ErrorInfo->Msg.EccMsg.Socket * A1_MEMORY_SOCKETS)   // DIMMs per socket
            				+ (ErrorInfo->Msg.EccMsg.MemoryController * DIMM_PER_CHANNEL * CHANNEL_PER_IMC)// DIMMs per IMC
                    + (ErrorInfo->Msg.EccMsg.Channel * DIMM_PER_CHANNEL) // DIMMs per channel
                    + ErrorInfo->Msg.EccMsg.DimmNum;            
            
            Data->MeHandle.SmbiosHandle = Type17[Dimm];
            break;
          case NB_PCIE_ERROR:
          case SB_PCIE_ERROR:
            TempData16 = ((ErrorInfo->Msg.PcieMsg.Bus << 8) |
                          (ErrorInfo->Msg.PcieMsg.Dev << 3) |
                          (ErrorInfo->Msg.PcieMsg.Fun));
            Data->MeHandle.SmbiosHandle = TempData16;            
            break;     
        } // end... switch (ErrorInfo->HardwareId) {

#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<
        break;

// POST ERRORS
      case EFI_EVENT_LOG_VAR_DATA_TYPE_POST_RESULTS_BITMAP:
        Data->PostResults.VariableDataType = VariableDataType;
        POSTBit = *((UINT16*)(NewElogData + 1));
        if ( POSTBit > 31 ){
            Data->PostResults.Bitmap1 = 0;
            Data->PostResults.Bitmap2 = 1 << (POSTBit - 32);
        } else {
            Data->PostResults.Bitmap2 = 0;
            Data->PostResults.Bitmap1 = 1 << (POSTBit);
        }
        Data->PostResults.Bitmap1 = Data->PostResults.Bitmap1 | 
                                    EFI_POST_BITMAP_DW1_SECOND_DWORD_VALID;
        break;

      case EFI_EVENT_LOG_VAR_DATA_TYPE_SYS_MNGMT_TYPE:
        Data->SystemManagement.VariableDataType = VariableDataType;
        Data->SystemManagement.Condition = *((UINT32*)(NewElogData));
        break;

      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE:
        Data->MeSystemManagement.VariableDataType = VariableDataType;
        Data->MeSystemManagement.Condition = *((UINT32*)(NewElogData));
        Data->MeSystemManagement.Counter = 0xFFFFFFFF;
        break;

// OEM TYPE W/ EFI STATUS CODES
      case EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE:
        Data->MeStatusCode.VariableDataType = VariableDataType;
        StatusCodeInputStructure = *((EFI_STATUS_CODE_ELOG_INPUT*)NewElogData);
        Data->MeStatusCode.Type = StatusCodeInputStructure.StatusCodeType;
        Data->MeStatusCode.Value = StatusCodeInputStructure.StatusCodeValue;
        Data->MeStatusCode.Counter = 0xFFFFFFFF;
        break;

// NOT VALID RESULTS
      default:
        #if (CustomEventTypeAndStrings == 1)
          EventToWrite.Length = sizeof(EFI_GPNV_ERROR_EVENT) + (UINT8)Size;
    
// Copy data following regular EFI_GPNV_ERROR_EVENT to 
// EventExtendedData, if any.

          if (Size != 0) {
              MemCpy(&EventExtendedData[0],  ElogData + sizeof(EFI_GPNV_ERROR_EVENT), Size);
          }
        #else
          return EFI_INVALID_PARAMETER;
        #endif
    }

// If the Elog Size is bigger than max Smbios Extended Data Size, return error

    if (EventToWrite.Length > 0x7F) {
        return EFI_INVALID_PARAMETER;
    }


// Concat Parts so there is only one write to flash part

    MemCpy(&NewElogDataBuffer[0], (UINT8*)&EventToWrite, sizeof (EFI_GPNV_ERROR_EVENT));

    if (EventToWrite.Length > sizeof (EFI_GPNV_ERROR_EVENT)) {
        MemCpy (&NewElogDataBuffer[sizeof (EFI_GPNV_ERROR_EVENT)],
                &EventExtendedData[0],
                EventToWrite.Length - sizeof (EFI_GPNV_ERROR_EVENT));
    }

// If duplicate record and is still inside METW or MECI values, internal 
// counters are noted and recordId of previous record is returned and 
// function returns TRUE else returns FALSE

    Event = (ALL_GPNV_ERROR*)&NewElogDataBuffer[0];
    if (DuplicateRecordAndAlreadyReacted (SmbiosElogPrivateData,
        &Event->Header, (UINT8*)&Event->Data, RecordId,
        VariableDataType)) {
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01            
        DEBUG((DEBUG_INFO,"Duplicate Record in METW or MECI ret\n"));       
        return EFI_ABORTED;
#else     
        return EFI_SUCCESS;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01    
    }

// Check if enough memory is present for the record...

    Status = CheckIfFull (SmbiosElogPrivateData, EventToWrite.Length);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    EventOffset = (UINT32)((UINT64)&(SmbiosElogPrivateData->CopyOfFlashArea[SmbiosElogPrivateData->NextAddress -
                          SmbiosElogPrivateData->StartAddress]) - (UINT64)&(SmbiosElogPrivateData->CopyOfFlashArea[0]));
    if (gIsMultiEvent) {

// Save offset of new event.

        SmbiosElogPrivateData->MultiEventOffset[gMultiEventIndex] = EventOffset;
    }

// Record Input Event Header into Memory Copy

    MemCpy (&(SmbiosElogPrivateData->CopyOfFlashArea[SmbiosElogPrivateData->NextAddress -
        SmbiosElogPrivateData->StartAddress]), &NewElogDataBuffer[0], EventToWrite.Length );

    RecordMemoryCopyDifferencesToFlash (SmbiosElogPrivateData, EventOffset,
                                       EventOffset+EventToWrite.Length);

// Incrementing the RecordId as new event is set

    (SmbiosElogPrivateData->RecordId)++;


// Return RecordId of the newly inserted event

    *RecordId = SmbiosElogPrivateData->RecordId;

// Update Next Record Values

    SmbiosElogPrivateData->NextAddress += EventToWrite.Length;

    return EFI_SUCCESS;
}

/**

    Gets the Event-Log data from the destination.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param ElogData - Pointer to the Event-Log data buffer that will contain  
                      the data to be retrieved.
    @param DataType - Type of Elog Data that is being recorded.
    @param Size - Size of the data to be retrieved.
    @param RecordId - This is the RecordId of the next record. If ElogData is 
                      NULL, this gives the RecordId of the first record 
                      available in the database with the correct DataSize. A 
                      value of 0 on  return indicates the last record if the 
                      EFI_STATUS indicates a success.

    @return EFI_STATUS
    @retval EFI_SUCCESS - Event-Log was recorded successfully.
    @retval EFI_INVALID_PARAMETER - Invalid parameter
    @retval EFI_NOT_FOUND - Event-Log target not found.
    @retval EFI_BUFFER_TOO_SMALL - Target buffer is too small to retrieve the 
                                   data.

**/

EFI_STATUS
EfiGetSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL  *This,
    IN  UINT8                       *ElogData,
    IN  EFI_SM_ELOG_REDIR_TYPE      DataType,
    OUT UINTN                       *Size,
    OUT UINT64                      *RecordId )
{
    EFI_SMBIOS_ELOG_INSTANCE_DATA   *SmbiosElogPrivateData;
    UINTN                           Count;
    UINT8                           *PtrInMemCopy;
    EFI_GPNV_ERROR_EVENT            EventToUse;
    VOID                            *FlashTableCopy = NULL;

    SmbiosElogPrivateData = (EFI_SMBIOS_ELOG_INSTANCE_DATA*)This;


// Ensure Info Being Passed in is of Type Smbios

    if (DataType != EfiElogRedirSmSMBIOS) {
        return EFI_NOT_FOUND;
    }


// Return EFI_INVALID_PARAMETER if RecordId pointer is NULL or record id 
// is equal to '0' as we are setting record id of events starting from '1'
// or Size pointer is equal to NULL. 

    if(RecordId == NULL || *RecordId == 0 || Size == NULL) {
       return EFI_INVALID_PARAMETER;
    }


// If ElogData is Null, requesting information not yet implemented

    if (ElogData == NULL) {

        *RecordId = 1;  // Returning RecordId of starting event.
        return EFI_INVALID_PARAMETER;
    }

    FlashTableCopy = &(SmbiosElogPrivateData->CopyOfFlashArea[0]);

    PtrInMemCopy = (UINT8*)(FlashTableCopy) + 
                   sizeof(EFI_GPNV_ERROR_HEADER_TYPE1);
    MemCpy (&EventToUse, PtrInMemCopy, sizeof(EFI_GPNV_ERROR_EVENT));
    Count = 1;
    while (Count < (*RecordId)
           && (EventToUse.EventType != EFI_EVENT_LOG_TYPE_END_OF_LOG)
           && ((UINT8*)PtrInMemCopy - (UINT8*)FlashTableCopy < 
              LOCAL_EVENT_LOG_LENGTH)) {

        PtrInMemCopy += EventToUse.Length;
        MemCpy (&EventToUse, PtrInMemCopy, sizeof (EFI_GPNV_ERROR_EVENT));
        if (EventToUse.EventType != EFI_EVENT_LOG_TYPE_OEM_DUMMY){
            Count++;
        }
    }

    if (EventToUse.EventType == EFI_EVENT_LOG_TYPE_END_OF_LOG) {
        return EFI_NOT_FOUND; //End of Table Reached before Desired Record
    }

    if (EventToUse.Length > (*Size)) {
        return EFI_BUFFER_TOO_SMALL;
    }

    MemCpy (ElogData,  PtrInMemCopy, EventToUse.Length);

    *Size = EventToUse.Length;
    *RecordId = *RecordId + 1;

// Check if next record is end of log... if so, return RecordId as zero

    PtrInMemCopy += EventToUse.Length;
    MemCpy(&EventToUse,PtrInMemCopy,sizeof(EFI_GPNV_ERROR_EVENT));

    if (EventToUse.EventType == EFI_EVENT_LOG_TYPE_END_OF_LOG) {
        *RecordId = 0;
    }

    return EFI_SUCCESS;
}


/**

    Erases the Event-Log data from the Flash Part.

    @param SmbiosElogPrivateData - Gives access to Private Variables to allow 
                                   erasing

    @return EFI_STATUS 
    @retval EFI_SUCCESS

    @note  N/A

**/

EFI_STATUS
EfiEraseSmbiosElogDataLocal (
    OUT EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData )
{


// Reset and initialize all data in Memory Copy

    ResetAllDataInMemoryCopy (SmbiosElogPrivateData);

// Write Differences between Memory Copy and Flash onto the Flash

    RecordMemoryCopyDifferencesToFlash (SmbiosElogPrivateData, 0,
                                       LOCAL_EVENT_LOG_LENGTH);

    return EFI_SUCCESS;
}

/**

    Erases the Event-Log data from the Flash Part.  This Implementation does 
    not allow the erasing of individual events, only the whole log.  Therefore
    RecordId must always be NULL to return a success.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL. 
    @param DataType - Type of Elog Data that is being Erased.   
    @param RecordId - This is the RecordId of the data to be erased. If 
                      RecordId is NULL, all the records on the database are 
                      erased if permitted by the target. Contains the deleted 
                      RecordId on return

    @return EFI_STATUS
    @retval EFI_SUCCESS - Event-Log was Erased successfully.
    @retval EFI_NOT_FOUND - Incorrect Type for this Log.
    @retval EFI_INVALID_PARAMETER - Incorrect parameter. Can not erase 
                                    individual records.

    @note  N/A

**/

EFI_STATUS EfiEraseSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL    *This,
    IN  EFI_SM_ELOG_REDIR_TYPE        DataType,
    OUT UINT64                        *RecordId )
{
    EFI_SMBIOS_ELOG_INSTANCE_DATA     *SmbiosElogPrivateData;

//  INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS (This);

    SmbiosElogPrivateData = (EFI_SMBIOS_ELOG_INSTANCE_DATA*)This;

// Ensure Info Being Passed in is of Type Smbios

    if (DataType != EfiElogRedirSmSMBIOS) {
        return EFI_NOT_FOUND;
    }

// If RecordId is not Null, requesting information not yet implemented
// This implementation only allows erasing of the whole log, not
// individual events

    if (RecordId != NULL){
        return EFI_UNSUPPORTED;
    }

// Do Actual Erasing with Local Function

    EfiEraseSmbiosElogDataLocal (SmbiosElogPrivateData);

    return EFI_SUCCESS;
}

/**
    This API enables/Disables Event Log.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param DataType - Type of Elog Data that is being Activated/Disabled.
    @param EnableElog - Enables (TRUE)/Disables(FALSE) Event Log. If NULL just
                        returns the Current ElogStatus. 
    @param ElogStatus - Current (New) Status of Event Log. Enabled (TRUE),
                        Disabled (FALSE).

    @return EFI_STATUS
    @retval EFI_SUCCESS to indicate successful completion or valid EFI error
                        code otherwise.

    @note  N/A

**/

EFI_STATUS
EfiActivateSmbiosElog (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL    *This,
    IN  EFI_SM_ELOG_REDIR_TYPE        DataType,
    IN  BOOLEAN                       *EnableElog,
    OUT BOOLEAN                       *ElogStatus )
{


// Ensure Info Being Passed in is of Type Smbios

      if (DataType != EfiElogRedirSmSMBIOS) {
        return EFI_NOT_FOUND;
      }


// Return EFI_UNSUPPORTED if ElogStatus pointer is NULL.

    if(ElogStatus == NULL) {
       return EFI_UNSUPPORTED;
    }
   

// Check whether EnableElog is NULL if so then just return current event 
// log status

    if (EnableElog == NULL) {
        *ElogStatus = gActivateEventLog;
         return EFI_SUCCESS;
    }


// Change the Activate Event Log status depending upon user request

    if (*EnableElog == TRUE) {
       gActivateEventLog = TRUE;
    } else {
       gActivateEventLog = FALSE;
    }    

    *ElogStatus = gActivateEventLog;
    return EFI_SUCCESS;
}

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
