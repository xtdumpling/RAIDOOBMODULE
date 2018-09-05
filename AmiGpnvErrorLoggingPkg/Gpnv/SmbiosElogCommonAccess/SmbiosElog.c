//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2016, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//  Rev. 1.02
//    Bug Fix:  1. Fix issue that changed METW,MECI's value will not take effect at this boot to OS.
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Sep/06/2016
//
//  
//  Rev. 1.01
//    Bug Fix :  Let MECI (Multiple Event Count),METW (Multiple Event Time Window) can work with Memory correct error report and Pcie error report.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Aug/23/2016
//
//  Rev. 1.00
//    Bug Fix : Add ECC/PCI SMBIOS event log function.
//    Reason  : 
//    Auditor : Ivern Yeh
//    Date    : Jun//2016
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


/** @file SmbiosElog.c
    SmbiosElog driver Implementation

**/


//----------------------------------------------------------------------

#include "SmbiosElog.h"
#include <GpnvRtcReadHook.h>

#if OEMRtc_Enabled
extern GPNV_RTC_READ_HOOK GPNV_RTC_READ_HOOK_LIST ;
GPNV_RTC_READ_HOOK* GpnvRtcReadList[] = {GPNV_RTC_READ_HOOK_LIST, NULL };
#endif
//----------------------------------------------------------------------

BOOLEAN  gIsMultiEvent;
UINT16   gMultiEventIndex;

extern EFI_GUID gEfiSetupVariableGuid;

#if OEMRtc_Enabled
/**

    Calls RTC Read functions 

    @param Time - Pointer to the Structure to Read Time from RTC

    @return EFI_STATUS

**/
EFI_STATUS
GpnvRtcRead (
   OUT  EFI_TIME               *Time
)
{
    EFI_STATUS	Status;
    UINTN i;
    for(i=0; GpnvRtcReadList[i]; i++) {
         Status = GpnvRtcReadList[i](Time);
    }
    return Status;
}
#endif

/**

    Get the VariableDataType of Event

    @param EventType - Type of the Event
    @param VaraiableDataType - VaraiableDataType of Event is returned.

    @return EFI_STATUS
    @retval EFI_SUCCESS
    @retval EFI_INVALID_PARAMETER

**/

EFI_STATUS
GetEventVariableDataType (
  IN      UINT8    EventType,
  OUT     UINT8    *VariableDataType )
{
    UINT8 Count;
    for (Count = 0; Count < NumOfSupportedSmbiosTypes; Count++) {
        if (SupportedEventLogTypesList[Count].LogType == EventType) {
            *VariableDataType = 
            SupportedEventLogTypesList[Count].VariableDataFormatType;
            return EFI_SUCCESS;
        }
    }
// Event type is not known, but why not let the OEM use what they want?

    *VariableDataType = 0xFF;
    #if (!CustomEventTypeAndStrings)
        return EFI_INVALID_PARAMETER;
    #endif
    return EFI_SUCCESS;
}

/**

    Converts EFI status code value to legacy code

    @param EfiStatusodeValue - EFI Status code value
    @param LegacyCode - Legacy code

    @return EFI_STATUS
    @retval EFI_SUCCESS - Conversion successful

**/

EFI_STATUS
ConvertEfiToLegacy (
  IN     UINT32                       EfiStatusCodeValue,
  IN OUT LEGACY_CODE_STRUCTURE        *LegacyCode )
{
    UINT16 TableCount = 0;

    LegacyCode->Type = 0xFF;
    while (TableCount < ( sizeof (TranslationTable1) / sizeof (STATUS_CODE_TO_SMBIOS_TYPE_TABLE))) {
        if (TranslationTable1[TableCount].EfiValue == EfiStatusCodeValue) {
            *LegacyCode = TranslationTable1[TableCount].LegacyErrorCode;
            break;
        }
        TableCount++;
    }

    return EFI_SUCCESS;
}

/**

    Checks if log is full,if full Check Setup Question and erases or does 
    nothing accordingly

    @param SmbiosElogPrivateData - Gives access to Private Variables
    @param EventSize - Size of Event to be added to memory

    @return EFI_STATUS
    @retval EFI_SUCCESS
    @retval EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
CheckIfFull (
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA  *SmbiosElogPrivateData,
  IN UINT8                          EventSize )
{
    if (SmbiosElogPrivateData->EndAddress < (SmbiosElogPrivateData->NextAddress + EventSize + sizeof (EFI_GPNV_ERROR_EVENT))) {


// Not enough room left. Check Setup Question for what to do.

        if (SmbiosElogPrivateData->EraseEventLogWhenFull) {

// Need to erase event log (Type 15 bits set inside erase function)

            EfiEraseSmbiosElogDataLocal (SmbiosElogPrivateData);
            return EFI_SUCCESS;
        } else {

// Reset ME Buffer Data, so that we don't have any potential problems from it.

            SmbiosElogPrivateData->NumberOfMultipleEventRecords = 0;
            return EFI_OUT_OF_RESOURCES;
        }
    }
    return EFI_SUCCESS;
}


/**

    The function returns a pointer to the 32-bit counter associated with the
    specified event type.

    @param Data - The event for which the record size is to be determined.
    @param VariableDataType - The format type for the variable data associated
                              with the event.
    @param EventCounter - The pointer to the counter is returned.

    @return VOID

    @note  N/A

**/

VOID
GetCounterPointer (
    IN  ALL_EVENT_TYPE_DATA  *Data,
    IN  UINT8                VariableDataType,
    OUT UINT32               **EventCounter )
{

    switch (VariableDataType) {
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT:
          *EventCounter = &Data->Me.Counter;
          break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE:
          *EventCounter = &Data->MeHandle.Counter;
          break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE:
          *EventCounter = &Data->MeSystemManagement.Counter;
          break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE:
          *EventCounter = &Data->MeStatusCode.Counter;
          break;
    }
}

/**
    The function returns the size of an error log entry for the specified  
    event type.

    @param Event - Pointer to the event structure.
    @param VariableDataType - The format type for the variable data associated
                              with the event.

    @return UINT8 - The size in bytes of the record is returned.

    @note  N/A

**/

UINT8
GetEventRecordSize (
    IN  ALL_GPNV_ERROR   *Event,
    IN  UINT8            VariableDataType )
{
    UINT8 RecordSize = sizeof(Event->Header);

    switch (VariableDataType) {
      case EFI_EVENT_LOG_VAR_DATA_TYPE_NONE:
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_HANDLE:
        RecordSize += sizeof(Event->Data.Handle);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT:
        RecordSize += sizeof(Event->Data.Me);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE:
        RecordSize += sizeof(Event->Data.MeHandle);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_SYS_MNGMT_TYPE:
        RecordSize += sizeof(Event->Data.MeSystemManagement);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE:
        RecordSize += sizeof(Event->Data.SystemManagement);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE:
        RecordSize += sizeof(Event->Data.MeStatusCode);
        break;
      case EFI_EVENT_LOG_VAR_DATA_TYPE_POST_RESULTS_BITMAP:
        RecordSize += sizeof(Event->Data.PostResults);
        break;
    }
    return RecordSize;
}

/**

    The function get the internal history related data for multiple event 
    type records.

    @param SmbiosElogPrivateData - Pointer to internal data.
    @param EventToUse - Pointer to the event that was just asserted.
    @param ExtendedData - Pointer to the Extended event data.
    @param MultiEventIndex - If event is present already then index of that 
                             event is retuned this is used to get the offset 
                             of event, otherwise last index is returned in 
                             this index offset of new event is stored.

     @return BOOLEAN
     @retval TRUE - History exists for this type and the output 
                    parameters are valid.
     @retval FALSE - History does not exist and the output parameters
                     are not valid.

    @note  N/A

**/

BOOLEAN
GetHistoryInfo (
    IN  EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData,
    IN  EFI_GPNV_ERROR_EVENT          *EventToUse,
    IN  UINT8                         *ExtendedData,
    OUT UINT16                        *MultiEventIndex )
{
    BOOLEAN           RecordExists=FALSE;
    UINT32            MultiEventCount = 0;
    ALL_GPNV_ERROR   *EventPtr;
    UINT32            EventOffset;

// Search Multi Event Records for Duplicate record

    while (MultiEventCount < SmbiosElogPrivateData->NumberOfMultipleEventRecords) {
        EventOffset = SmbiosElogPrivateData->MultiEventOffset[MultiEventCount];
        EventPtr = (ALL_GPNV_ERROR *)(&SmbiosElogPrivateData->CopyOfFlashArea[EventOffset]);

// 1. Check Whether Event Type and Length are matching
// 2. Check Event Data is matching or not (Exclude Counter and Internal Counter value)

        if (MemCmp (&(EventPtr->Header), EventToUse, 2) == 0) {

// Calculate size of Extended data to be compared.
// Exclude size of the Header and Counter value from the length of the Event

            UINTN  SizeofExtendedDataTobeCompared = EventToUse->Length - \
                                                     sizeof(EFI_GPNV_ERROR_EVENT) - sizeof(UINT32);

// Check if any Extended data exists to compare
// If so, check remaining Extended data is matching or not

            if ((!SizeofExtendedDataTobeCompared) || \
                (MemCmp (&(EventPtr->Data), ExtendedData, \
                SizeofExtendedDataTobeCompared) == 0 )) {
                RecordExists = TRUE;
                break;
            }
        }
        MultiEventCount++;
    }

    *MultiEventIndex = MultiEventCount;

    return RecordExists;
}
/**

    This function will search the local records from the multiple events and
    find whether this event should simply increment a counter already 
    started or start a new counter and allow the system to record the record
    into the flash part.

    @param SmbiosElogPrivateData - Gives access to Private Variables
    @param EventToUse - Header for new event
    @param ExtendedData - Extended data for new event, size is 
                          EventToUse->Length - sizeof(EFI_GPNV_ERROR_EVENT)
    @param RecordId - RecordId of current record location
    @param VariableDataType - The variable data type associated with 
                              the record type

    @return BOOLEAN 
    @retval TRUE - The function has taken care of all needed actions
                   and the record does not need to be recorded into 
                   the flash part.
    @retval FALSE - The function requests that this record be
                    recorded into the flash part.

**/

BOOLEAN
DuplicateRecordAndAlreadyReacted (
  IN  EFI_SMBIOS_ELOG_INSTANCE_DATA   *SmbiosElogPrivateData,
  IN  EFI_GPNV_ERROR_EVENT            *EventToUse,
  IN  OUT  UINT8                      *ExtendedData,
  OUT UINT64                          *RecordId,
  IN  UINT8                           VariableDataType )
{
    UINT8             *FlashTableCopy;
    UINT8             *PtrInMemCopy;
    UINT8             *SavedPtr;
    UINT8             *DataPtr;
    UINTN             ScratchSize;
    UINT32            *LastMultiEventCounter;
    UINT8             InternalCounter;
    BOOLEAN           HistoryExists;
    UINT8             Metw = SmbiosElogPrivateData->METW;
    UINT8             Meci = SmbiosElogPrivateData->MECI;
    ALL_EVENT_TYPE_DATA *EventData;
    ALL_GPNV_ERROR      *MultiEventPtr = NULL;
    UINT32              MultiEventOffset,EventCounterOffset; 

    FlashTableCopy = &(SmbiosElogPrivateData->CopyOfFlashArea[0]);


// Post Errors handled differently, will only update the latest
// post record and only if last found is during this latest boot.
// if the last one does have a system boot record after
// it, then return DuplicateRecordAndAlreadyReacted as FALSE and let
// it be recorded at end of Event Log as a new record.

    if (EventToUse->EventType == EFI_EVENT_LOG_TYPE_POST_ERROR) {

// Start Post Errors Handling

        PtrInMemCopy = FlashTableCopy + sizeof(EFI_GPNV_ERROR_HEADER_TYPE1);
        SavedPtr = NULL;

// Search for POST record since last system boot

        while (*PtrInMemCopy != EFI_EVENT_LOG_TYPE_END_OF_LOG) {
            if (*PtrInMemCopy == EFI_EVENT_LOG_TYPE_POST_ERROR) {
                SavedPtr = PtrInMemCopy;
            } else if (*PtrInMemCopy == EFI_EVENT_LOG_TYPE_SYSTEM_BOOT) {
                SavedPtr = NULL;
        }
        PtrInMemCopy = ((EFI_GPNV_ERROR_EVENT*)PtrInMemCopy)->Length + PtrInMemCopy;
      }

// If No Record to update exit

      if (SavedPtr == NULL) {
        return FALSE;
      }


// If bit trying to set is already set, exit here with nothing left to do.

        if (*((UINT64*)ExtendedData) == 
            (*((UINT64*)ExtendedData) | 
            *((UINT64*)(SavedPtr + sizeof(EFI_GPNV_ERROR_EVENT))))) {
            return TRUE;
        }


// Update our record with bits that were set from record in flash part

        *((UINT64*)ExtendedData) = *((UINT64*)ExtendedData) | 
        *((UINT64*)(SavedPtr + sizeof(EFI_GPNV_ERROR_EVENT)));

        *((EFI_GPNV_ERROR_EVENT*)SavedPtr) = *EventToUse;
    
        ScratchSize = EventToUse->Length - sizeof(EFI_GPNV_ERROR_EVENT);
        SavedPtr = SavedPtr + sizeof(EFI_GPNV_ERROR_EVENT);
        DataPtr = ExtendedData;
        while (ScratchSize > 0) {
        *SavedPtr = *DataPtr;
        SavedPtr++;
        DataPtr++;
        ScratchSize--;
        }

        RecordMemoryCopyDifferencesToFlash(SmbiosElogPrivateData, 0,
            LOCAL_EVENT_LOG_LENGTH);

        return TRUE;

// End Post Errors Handling

    } else {

// When METW == 0, no tracking of multiple event information is needed
// because all new records will be written as new records.

        if ( Metw == 0 ) {
            return FALSE;
        }

// Start Multiple Event Type Handling

// Check for Multi Event variable data type
// For Other than Multi Event types, return FALSE

        if (VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT
            && VariableDataType != 
            EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE
            && VariableDataType != 
            EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE
            && VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE) {
            gIsMultiEvent = FALSE;
            return FALSE;
        } else {
            gIsMultiEvent = TRUE;
        }

// Get history information about the Record

        HistoryExists = GetHistoryInfo(SmbiosElogPrivateData, EventToUse, 
                                                     ExtendedData, &gMultiEventIndex );

// If No Duplicate found, so add new record at Multi Event Buffer end and return FALSE
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
        if (EventToUse->EventType == EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR ){	
            HistoryExists = FALSE;
           }  
#endif


        if ( HistoryExists == FALSE ) {
            SmbiosElogPrivateData->NumberOfMultipleEventRecords++;
            return FALSE;
        } else {

// Get the Pointer to event, if event is present in already in flash.

            MultiEventOffset = SmbiosElogPrivateData->MultiEventOffset[gMultiEventIndex];
            MultiEventPtr = (ALL_GPNV_ERROR *)&SmbiosElogPrivateData->CopyOfFlashArea[MultiEventOffset];


// Check whether previous event is present in METW min if not return FALSE

            if (TimeDifference (*EventToUse, MultiEventPtr->Header) >= Metw) {

// The internal BIOS counter specific to that log type is reset to 0.

                SmbiosElogPrivateData->InternalCounter[gMultiEventIndex] = 0;
                return FALSE;
            }
        }
        EventData =  &MultiEventPtr->Data;


// Get the Multi Event Counter of previous Multi Event.

        GetCounterPointer(EventData,
                      VariableDataType,
                      &LastMultiEventCounter );
        if ( *LastMultiEventCounter > 0 ) {


// Check for MECI Violation:
// If the internal BIOS counter reaches MECI value then 
//    1) Reset the Internal Counter to 0 and 
//    2) Increment Multi Event Counter and update it in flash and return TRUE.
// else Increment internal counter and Return TRUE 

            InternalCounter = SmbiosElogPrivateData->InternalCounter[gMultiEventIndex];

            if ( InternalCounter >= (Meci-1) ) {

// The internal BIOS counter specific to that log type is reset to 0.

                SmbiosElogPrivateData->InternalCounter[gMultiEventIndex] = 0;

// Increment current Multi Event Counter 

                *LastMultiEventCounter = *LastMultiEventCounter << 1;
                DEBUG((-1,"MultiEvent Counter Value: %x\n",*LastMultiEventCounter));

// Writing updated Event Counter into flash.

                EventCounterOffset = MultiEventOffset + MultiEventPtr->Header.Length - sizeof(UINT32);
                RecordMemoryCopyDifferencesToFlash (
                    SmbiosElogPrivateData, EventCounterOffset, EventCounterOffset + sizeof(UINT32) );
                return TRUE;
            } else {

// Increment internal counter and Return TRUE

                SmbiosElogPrivateData->InternalCounter[gMultiEventIndex] = ++InternalCounter;
                return TRUE;
            }
        } else {

// If the log's current multiple-event counter is 00000000h no recording 
// happens. Control comes here only when Event counter reaches 0x0000 and
// METW Violation is not happened. In such case, do not log any more
// events until METW violation happens.

            return TRUE;
        }
    }  
}

/**

    Compares difference in times in the event records to show number of minutes difference.

    @param EventToUse1 - Event to inspect
    @param EventToUse2 - Event to inspect

    @return UINT8 - Number of Minutes difference in Decimal (max of 99)

**/

UINT8
TimeDifference (
  IN  EFI_GPNV_ERROR_EVENT            EventToUse1,
  IN  EFI_GPNV_ERROR_EVENT            EventToUse2 )
{
    UINT32 Time1 = 0;
    UINT32 Time2 = 0;
    UINT32 Difference;
    UINT8 ReturnThis;

    if ((EventToUse1.Month != EventToUse2.Month) || (EventToUse1.Year != EventToUse2.Year))
        return 0x99;

    Time1 += EventToUse1.Second;
    Time1 += EventToUse1.Minute * 60;
    Time1 += EventToUse1.Hour * 60 * 60;
    Time1 += EventToUse1.Day * 24 * 60 * 60;

    Time2 += EventToUse2.Second;
    Time2 += EventToUse2.Minute * 60;
    Time2 += EventToUse2.Hour * 60 * 60;
    Time2 += EventToUse2.Day * 24 * 60 * 60;

    if (Time1 > Time2) {
        Difference = (Time1 - Time2) / 60;
    } else {
        Difference = (Time2 - Time1) / 60;
    }

    if (Difference > 99)
        Difference = 99;

    ReturnThis = (UINT8)((Difference % 10) + (0x10 * (Difference / 10))); // put into decimal
    return ReturnThis;

}

/**

    Checks Record to see if anything in structure indicates an invalid or 
    corrupted event.

    @param EventToUse - Pointer to Event to inspect
    @param SmbiosElogPrivateData - Gives access to Private Variable

    @return BOOLEAN 
    @retval FALSE - Invalid Record
    @retval TRUE - Valid Record

**/

BOOLEAN
IsValidEvent (
  IN  EFI_GPNV_ERROR_EVENT            *EventToUse,
  IN  EFI_SMBIOS_ELOG_INSTANCE_DATA   *SmbiosElogPrivateData )
{
    UINT8 Types;
    BOOLEAN Found = FALSE;


// Confirm Valid Type

    for (Types = 0; Types < NumOfSupportedSmbiosTypes; Types++) {
        if (EventToUse->EventType == SmbiosElogPrivateData->ValidSmbiosTypes[Types].LogType) {
            Found = TRUE;
        }
    }


// If Event Type not valid, Exit with result as "FALSE"

    if ( !Found )
        return FALSE;

// Confirm Valid Date/Time, Exit with result as "FALSE" if invalid

    if ((((EventToUse->Year) & 0xF0) > 0x90) ||
        (((EventToUse->Month) & 0xF0) > 0x10) ||
        (((EventToUse->Day) & 0xF0) > 0x30) ||
        (((EventToUse->Hour) & 0xF0) > 0x20) ||
        (((EventToUse->Minute) & 0xF0) > 0x50) ||
        (((EventToUse->Second) & 0xF0) > 0x50) ||
        (((EventToUse->Year) & 0x0F) > 0x09) ||
        (((EventToUse->Month) & 0x0F) > 0x09) ||
        (((EventToUse->Day) & 0x0F) > 0x09) ||
        (((EventToUse->Hour) & 0x0F) > 0x09) ||
        (((EventToUse->Minute) & 0x0F) > 0x09) ||
        (((EventToUse->Second) & 0x0F) > 0x09))
        return FALSE;

// Confirm Valid Length, Exit "FALSE" if shorter than header length

    if (((EventToUse->Length) & 0x7F) < sizeof(EFI_GPNV_ERROR_EVENT))
        return FALSE;

  return TRUE;
}


/**
  Convert RTC Read value of TIME in BCD Format.

  @param   Time       On Input, the time data read from RTC to convert
                      On Output, the time converted to BCD format

**/

VOID
ConvertTimetoBCD(
  OUT  EFI_TIME               *Time )
{
        Time->Year = (((Time->Year / 1000)* 0x1000) + (((Time->Year / 100) % 10 )* 0x100) + \
                 (((Time->Year % 100) / 10) * 0x10) + ((Time->Year % 100) % 10));
        Time->Month = (( (Time->Month) / 10) * 0x10) + ((Time->Month) % 10);
        Time->Day = (( (Time->Day) / 10) * 0x10) + ((Time->Day) % 10);
        Time->Hour = (( (Time->Hour) / 10) * 0x10) + ((Time->Hour) % 10);
        Time->Minute = (( (Time->Minute) / 10) * 0x10) + ((Time->Minute) % 10);
        Time->Second = (( (Time->Second) / 10) * 0x10) + ((Time->Second) % 10);
}

/**

    Takes an event structure and fills it with the appropriate date and time.
    Fills Type with 0x80 and Length as 0x00 also as defaults.

    @param EventPtr - A pointer to the event structure created here

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
FillEventWithTimeAndDefaults (
  IN OUT  EFI_GPNV_ERROR_EVENT    *EventPtr )
{
    EFI_STATUS      Status = EFI_UNSUPPORTED;
    EFI_TIME        Time = { 0 };

    if ((pRS != NULL) && (pRS->GetTime != NULL)) {

        Status = pRS->GetTime(&Time, NULL);

    }

    if (Status != EFI_SUCCESS) {

        #if OEMRtc_Enabled
        Status= GpnvRtcRead (&Time);
        #endif

        } 
    ConvertTimetoBCD (&Time);

    if ((EFI_ERROR(Status)) || ((Time.Year < 0x1980) || (Time.Year > 0x2079))) {

        EventPtr->Month  = 1;
        EventPtr->Day    = 1;
        EventPtr->Year   = 0x80;
        EventPtr->Hour   = 0;
        EventPtr->Minute = 0;
        EventPtr->Second = 0;

    } else { 
        //
        // Fill Event variables with time/date info
        //
        EventPtr->Month  = Time.Month;
        EventPtr->Day    = Time.Day;
        EventPtr->Year   = (UINT8)Time.Year;
        EventPtr->Hour   = Time.Hour;
        EventPtr->Minute = Time.Minute;
        EventPtr->Second = Time.Second;
    }

// Fill Event variables with default type and length of zero

    EventPtr->EventType = EFI_EVENT_LOG_TYPE_END_OF_LOG;
    EventPtr->Length = 0x00;

    return EFI_SUCCESS;
}

/**

    Reset and initialize all data in Memory Copy

    @param SmbiosElogPrivateData - Gives access to Private Variables 

    @return EFI_STATUS
    @retval EFI_SUCCESS Event-Log was erased successfully.

**/

EFI_STATUS
ResetAllDataInMemoryCopy (
  IN EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData )
{
    EFI_GPNV_ERROR_HEADER_TYPE1*    HeaderPtr;
    EFI_GPNV_ERROR_EVENT*           EventPtr;
    EFI_GPNV_ERROR_EVENT            Event;


// Clear whole block of memory copy

    SetMem ((VOID *)(UINTN)&(SmbiosElogPrivateData->CopyOfFlashArea[0]), LOCAL_EVENT_LOG_LENGTH, 0xFF);

// Insert new Header and Erase record to memory copy

    HeaderPtr = (EFI_GPNV_ERROR_HEADER_TYPE1*)(&(SmbiosElogPrivateData->CopyOfFlashArea[0]));
    EventPtr = (EFI_GPNV_ERROR_EVENT*)(&(SmbiosElogPrivateData->CopyOfFlashArea[0]) + sizeof(EFI_GPNV_ERROR_HEADER_TYPE1));

    FillEventWithTimeAndDefaults (&Event);
    Event.Length = sizeof (EFI_GPNV_ERROR_EVENT);
    Event.EventType = EFI_EVENT_LOG_TYPE_LOG_AREA_RESET;

    *HeaderPtr = GPNVErrorLogHeaderType1;
    *EventPtr = Event;

    HeaderPtr->MultipleEventTimeWindow = (0x10*(SmbiosElogPrivateData->METW / 10)) + (SmbiosElogPrivateData->METW % 10);
    HeaderPtr->MultipleEventCountIncrement = SmbiosElogPrivateData->MECI;

    SmbiosElogPrivateData->NextAddress = SmbiosElogPrivateData->StartAddress + sizeof(EFI_GPNV_ERROR_HEADER_TYPE1) + sizeof(EFI_GPNV_ERROR_EVENT);
    SmbiosElogPrivateData->NumberOfMultipleEventRecords = 0;

// Setting the RecordId to 1 as clearing all the data and adding the 
// LogAreaReset event

    SmbiosElogPrivateData->RecordId = 1;

    return EFI_SUCCESS;
}

/**

    Checks if logging to SMBIOS is enabled in setup

    @param VOID

    @return BOOLEAN 
    @retval TRUE - If Smbios event log is enabled in setup
    @retval FALE - If Smbios event log is enabled in setup

**/

BOOLEAN
IsSmbiosElogEnabled ( VOID )
{
    EFI_STATUS    Status;
    UINTN         VarSize;
    SETUP_DATA    mSystemConfiguration;

    VarSize = sizeof (mSystemConfiguration);
    Status = gRT->GetVariable (L"Setup",
                            &gEfiSetupVariableGuid,
                            NULL,
                            &VarSize,
                            &mSystemConfiguration );
    if (Status != EFI_SUCCESS || mSystemConfiguration.SmbiosLogging == 0) {
        return FALSE;
    } else {
        return TRUE;
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
