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
//  Rev. 1.01
//    Bug Fix :fixed Multi event time count does not follow spec.
//    Reason  : 
//    Auditor : Chen Lin
//    Date    : Dec/21/2016
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
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SmbiosElogInitFuncs.c
    SmbiosElog Initialization Functions Implementation

**/

//---------------------------------------------------------------------------

#include "SmbiosElog.h"
#include <Protocol/FlashProtocol.h>
#include "token.h"

// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMBIOS Type 17 DIMM handles
#ifndef A2_MEMORY_SOCKETS
  #define A2_MEMORY_SOCKETS 0
#endif 
#ifndef A3_MEMORY_SOCKETS
  #define A3_MEMORY_SOCKETS 0
#endif 
#ifndef A4_MEMORY_SOCKETS
  #define A4_MEMORY_SOCKETS 0
#endif 
#define MAX_DIMMS NO_OF_PHYSICAL_MEMORY_ARRAY * \
(A1_MEMORY_SOCKETS + A2_MEMORY_SOCKETS + A3_MEMORY_SOCKETS + A4_MEMORY_SOCKETS)

UINT16  Type17Data[MAX_DIMMS];
UINT16  *Type17 = NULL;

UINT32 Type17Count = MAX_DIMMS;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<


//---------------------------------------------------------------------------

// Global pointer to local information.  For Non-SMM version of component,
// Pointer should be updated when virtual memory mode is entered.

EFI_SMBIOS_ELOG_INSTANCE_DATA     *mRedirSmbiosPrivate;
EFI_DATA_HUB_PROTOCOL             *mDataHub = NULL;
EFI_SM_ELOG_REDIR_PROTOCOL        *mGenericElog = NULL;
EFI_GUID                          gEfiSetupVariableGuid = SETUP_GUID;
UINT64                            *mSmbiosRecordId;
FLASH_PROTOCOL                    *Flash=NULL;

// Prototypes for Functions Used in Init

EFI_STATUS SetFromSetupQuestions(VOID);
EFI_STATUS CheckSetupForEraseCommand(VOID);
EFI_STATUS CheckHeader(VOID);
BOOLEAN    IsSmbiosElogEnabled(VOID);
EFI_STATUS DxeSmmRedirFuncsHook (VOID);
BOOLEAN GetHistoryInfo(
    IN  EFI_SMBIOS_ELOG_INSTANCE_DATA *SmbiosElogPrivateData,
    IN  EFI_GPNV_ERROR_EVENT          *EventToUse,
    IN  UINT8                         *ExtendedData,
    OUT UINT16                        *MultiEventIndex 
);
BOOLEAN IsValidMultiEventToSave (IN EFI_GPNV_ERROR_EVENT Header);


// ENTRY POINT

/**

    Entry point of Smbios Elog Driver

    @param ImageHandle - The Image handle of this driver.
    @param SystemTable - The pointer of EFI_SYSTEM_TABLE.

    @return EFI_STATUS
    @retval EFI_SUCCESS - The driver successfully initialized
    @retval EFI_NOT_FOUND - Needed protocols were not found

**/

EFI_STATUS
InitializeSmbiosElog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{
    EFI_STATUS                        Status = EFI_SUCCESS;
    EFI_HANDLE                        NewHandle;
    BOOLEAN                           PastFirstRecord;
    EFI_GPNV_ERROR_EVENT              EventToUse;
    UINT16                            AddTypesLoopCount;
    UINT8                             *PtrInMemCopy = NULL;
    VOID                              *CallbackPtr;
    EFI_EVENT                         CallbackEvent; 
    EFI_SMBIOS_PROTOCOL               *SmbiosProtocolPtr = NULL;
    UINTN                             TempAddress = LOCAL_EVENT_LOG_PHY_ADDR;
    EFI_DATA_RECORD_HEADER            *Record=NULL;
    UINT64                            Mtc;
    UINT64                            RecordId;
    EFI_STATUS_CODE_ELOG_INPUT        ToWrite;
    DATA_HUB_STATUS_CODE_DATA_RECORD  *StatusRecord;
    EFI_RSC_HANDLER_PROTOCOL          *RscHandlerProtocol = NULL;
    EFI_EVENT                         mEfiVirtualNotifyEvent;
    ALL_GPNV_ERROR                    Event; 
    UINT16                            MultiEventIndex;
    BOOLEAN                           HistoryExists;

    InitAmiLib(ImageHandle, SystemTable);

    if (IsSmbiosElogEnabled() == FALSE) {
        return EFI_NOT_FOUND;
    }

    Status = gBS->LocateProtocol (&gFlashProtocolGuid, NULL, (VOID **)&Flash);

    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = gBS->AllocatePool (EfiRuntimeServicesData, sizeof (EFI_SMBIOS_ELOG_INSTANCE_DATA), (VOID **)&mRedirSmbiosPrivate);
    ASSERT_EFI_ERROR (Status);

// Clear all the instance data
    Status = gBS->AllocatePool(EfiRuntimeServicesData, sizeof(UINT64), (VOID **)&mSmbiosRecordId);  // Allocating memory from Runtime Services
    ASSERT_EFI_ERROR (Status);
    gBS->SetMem (mRedirSmbiosPrivate, sizeof (EFI_SMBIOS_ELOG_INSTANCE_DATA), 0);

// Assign the memcopy with default byte after erase 

    gBS->SetMem (&(mRedirSmbiosPrivate->CopyOfFlashArea[0]), LOCAL_EVENT_LOG_LENGTH, LOCAL_DEFAULT_BYTE_AFTER_ERASE);

// Add All Supported Smbios Types To Local Structure

    for (AddTypesLoopCount = 0; AddTypesLoopCount < NumOfSupportedSmbiosTypes; AddTypesLoopCount++)
        mRedirSmbiosPrivate->ValidSmbiosTypes[AddTypesLoopCount] = SupportedEventLogTypesList[AddTypesLoopCount];

// Fill in flash Values from information

    mRedirSmbiosPrivate->StartAddress = (UINT8 *)TempAddress;
    TempAddress = (LOCAL_EVENT_LOG_PHY_ADDR) + LOCAL_EVENT_LOG_LENGTH;
    mRedirSmbiosPrivate->EndAddress = (UINT8 *)TempAddress;
    mRedirSmbiosPrivate->NumberOfMultipleEventRecords = 0;

// Find Setup Question Values and Assign them to variables
// if returns EFI_NOT_FOUND, setup variables not present in NVRAM and 
// can not launch capabilities during this boot (often the very first 
// boot does not have them present, because most often they are created 
// by the setup app, and are therefore available from 2nd boot and on )
// ** Is this an acceptable trade off?? **

    if (SetFromSetupQuestions () == EFI_NOT_FOUND) {
        return EFI_SUCCESS;
    }


// Check setup question to Check if OS or user requested Erase on restart
// If returned value is EFI_SUCCESS, an erase of the entire structure has 
// been done and a valid structure has been written, making the process 
// of checking for validity here unnecessary. If erased, it also has 
// placed the correct copy of the flash area into the local memory copy, 
// so that too is unnecessary.

    if (CheckSetupForEraseCommand () != EFI_SUCCESS ) {

// Fill Memory Copy with Current Flash Part Info

        ErrorLoggingReadFlashIntoMemoryCopy();
   
// Validate The Current Table and Initialize if Needed

        PastFirstRecord = FALSE;
        mRedirSmbiosPrivate->NextAddress = mRedirSmbiosPrivate->StartAddress + sizeof (EFI_GPNV_ERROR_HEADER_TYPE1);
        PtrInMemCopy = &(mRedirSmbiosPrivate->CopyOfFlashArea[sizeof(EFI_GPNV_ERROR_HEADER_TYPE1)]);

        gBS->CopyMem (&EventToUse, PtrInMemCopy, sizeof(EFI_GPNV_ERROR_EVENT));
        while (TRUE) {

// End of Log Event Found and Not First Log, So Table is Valid

            if (EventToUse.EventType == EFI_EVENT_LOG_TYPE_END_OF_LOG && PastFirstRecord) {
              break;
            }

// Was Invalid Event, End of Event Space Reached without finding 
// "End of Log",or "End of Log" was first record ("Event Log Reset" 
// always first), so reinitialize table

#if (CustomEventTypeAndStrings == 1)
            if ( //!IsValidEvent(&EventToUse,mRedirSmbiosPrivate) ||  // Commented to avoid log reset when custom event type is found.
#else
            if ( !IsValidEvent(&EventToUse,mRedirSmbiosPrivate) ||
#endif
                EventToUse.EventType == EFI_EVENT_LOG_TYPE_END_OF_LOG ||
                ( &(mRedirSmbiosPrivate->CopyOfFlashArea[sizeof (EFI_GPNV_ERROR_HEADER_TYPE1)]) +
                LOCAL_EVENT_LOG_LENGTH <=  PtrInMemCopy ) ) {
                ResetAllDataInMemoryCopy(mRedirSmbiosPrivate);
                break;
            }


// Check whether event is of Multi Event Type and event present in METW min.

            if(IsValidMultiEventToSave(EventToUse)) {
                gBS->CopyMem (&Event, PtrInMemCopy, EventToUse.Length);

// Check whether this event is present already

                HistoryExists = GetHistoryInfo(mRedirSmbiosPrivate, &Event.Header,
                                            (UINT8 *)&Event.Data, &MultiEventIndex);

// Save the address of event 

                mRedirSmbiosPrivate->MultiEventOffset[MultiEventIndex] = \
                                   (UINT32)((UINT64)PtrInMemCopy - (UINT64)&(mRedirSmbiosPrivate->CopyOfFlashArea[0]));
                if ( HistoryExists == FALSE ) {
                    mRedirSmbiosPrivate->NumberOfMultipleEventRecords++;
                }
            }

// Was Valid Event and Not "End of Log", So Increment and Continue Loop

            PastFirstRecord = TRUE;

//Incrementing the RecordId for each record in the memory copy

            (mRedirSmbiosPrivate->RecordId)++;
            PtrInMemCopy += EventToUse.Length;
            mRedirSmbiosPrivate->NextAddress += EventToUse.Length;
            gBS->CopyMem (&EventToUse, PtrInMemCopy, sizeof (EFI_GPNV_ERROR_EVENT));
        } //while
    } // CheckSetupForEraseCommand


// Check the Header for Validity and reset if incorrect

    CheckHeader();


// Insert System Boot Record

    if ( mRedirSmbiosPrivate->SystemBootRecordLogging ) {
        InsertSystemBootRecord ();
    }

// Record all differences between Memory Copy and Flash Part to Flash Part
// (this is done so that many changes can be made during entry point to 
// the memory copy and flash part is only written once to save time.)

    RecordMemoryCopyDifferencesToFlash( mRedirSmbiosPrivate,
                                        0,
                                        LOCAL_EVENT_LOG_LENGTH - 1);

// Now install the Redirect Protocol

    mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog  = EfiActivateSmbiosElog;
    mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData = EfiEraseSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.GetEventLogData   = EfiGetSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.SetEventLogData   = EfiSetSmbiosElogData;
    NewHandle = NULL;

    Status = gBS->InstallProtocolInterface (
                                       &NewHandle,
                                       &gEfiRedirElogProtocolGuid,
                                       EFI_NATIVE_INTERFACE,
                                       &mRedirSmbiosPrivate->SmbiosElog );
    ASSERT_EFI_ERROR (Status);

// Now install the Trigger Protocol For Collection Components to start 
// Collecting Errors and passing them to the generic Elog

    NewHandle = NULL;
    mRedirSmbiosPrivate->SmbiosElogSupport.Something = (UINT8*)mRedirSmbiosPrivate;

    Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gAmiSmbiosElogSupportGuid,
                  EFI_NATIVE_INTERFACE,
                  &mRedirSmbiosPrivate->SmbiosElogSupport );

    ASSERT_EFI_ERROR (Status);


// Locate SMBIOS interface

    Status = gBS->LocateProtocol (&gAmiSmbiosProtocolGuid,
                                  NULL, 
                                 (VOID **)&SmbiosProtocolPtr);
    if (!EFI_ERROR (Status)) {

// Install SMBIOS Type 15 Structure

        AddSMBIOSType15Structure (NULL, NULL);
    } else {

// Create an event to publish SMBIOS type 15 structure 

        Status = gBS->CreateEvent ( EVT_NOTIFY_SIGNAL, 
                                    TPL_CALLBACK, 
                                   (EFI_EVENT_NOTIFY)AddSMBIOSType15Structure, 
                                    NULL, 
                                   &CallbackEvent
                                   );


// Register CallbackEvent for gAmiSmbiosProtocolGuid.

        if (!EFI_ERROR(Status))
            Status = gBS->RegisterProtocolNotify (&gAmiSmbiosProtocolGuid, CallbackEvent, &CallbackPtr);
    }


// Locate DataHub Protocol and GenericElog Protocol
// These should never fail due to Depex grammar.

    Status = gBS->LocateProtocol ( &gEfiDataHubProtocolGuid, 
                                   NULL, 
                                   (VOID **)&mDataHub
                                 );

    if(EFI_ERROR (Status)) {
        return Status;
    }

    Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &RscHandlerProtocol );

    if (!EFI_ERROR (Status)) {

// Register the callback function for ReportStatusCode() notification

        Status = RscHandlerProtocol->Register ((EFI_RSC_HANDLER_CALLBACK)GpnvStatusCodeReportWorker, 
                                                TPL_HIGH_LEVEL
                                               );
    }

// MTC of zero means return the next record that has not been read by the 
// event handler.

    Mtc = 0;
    do {
        Status = mDataHub->GetNextRecord (mDataHub, &Mtc, NULL, &Record);
        if (!EFI_ERROR (Status)) {
            if (!MemCmp (&Record->DataRecordGuid, 
                         &gEfiDataHubStatusCodeRecordGuid, 
                         sizeof (EFI_GUID))) {

// TO DO: For this Isolated record, retrieve Status Code Type and Value
// and place into the structure below. This is not implemented yet
// because the structure for logging status codes is not yet defined
// or executed anywhere.

                StatusRecord = (DATA_HUB_STATUS_CODE_DATA_RECORD *)(Record + 1);

                if ((StatusRecord->CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
                    ToWrite.StatusCodeType = StatusRecord->CodeType;
                    ToWrite.StatusCodeValue = StatusRecord->Value;

                    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE;
                    Status = EfiSetSmbiosElogData(
                                &mRedirSmbiosPrivate->SmbiosElog,
                                (UINT8*)&ToWrite,
                                EfiElogRedirSmSMBIOS,
                                FALSE,
                                sizeof (EFI_STATUS_CODE_ELOG_INPUT),
                                &RecordId);
                }
            }
        }
    } while ((Mtc != 0) && !EFI_ERROR (Status));


// Register SetVirtualAddressMap () notify function

    Status = gBS->CreateEventEx (
                     EVT_NOTIFY_SIGNAL,
                     TPL_NOTIFY,
                     SmbiosElogVirtualAddressChangeEvent,
                     NULL,
                     &gEfiEventVirtualAddressChangeGuid,
                     &mEfiVirtualNotifyEvent );
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "Create Event to gEfiEventVirtualAddressChangeGuid is Failed\n"));
    }


// This hook Registers a callback for the SMM Redir Protocol using a dummy GUID

    DxeSmmRedirFuncsHook ();

    return EFI_SUCCESS;
}

/**

    Uses Setup Question information gathered and uses it to
    store needed information locally and also to verify correct values of 
    MECI and METW in header.
    Also checks header to be sure correct static information is present.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS
    @retval EFI_NOT_FOUND

**/


EFI_STATUS
SetFromSetupQuestions ( VOID )
{
    EFI_STATUS    Status;
    UINTN         VarSize;
    SETUP_DATA    mSystemConfiguration;
    BOOLEAN       Rewrite = FALSE;
    UINT32        AttributesRead;


// Get values from setup and store in protocol

    VarSize = sizeof(mSystemConfiguration);
    Status = gRT->GetVariable (L"Setup",
                             &gEfiSetupVariableGuid,
                             &AttributesRead,
                             &VarSize,
                             &mSystemConfiguration );

    if (Status == EFI_SUCCESS) {

// Double check that values in Setup are valid values. If not, 
// change and mark for rewriting
 
        if (mSystemConfiguration.SmbiosLogging > 1) {
            mSystemConfiguration.SmbiosLogging = FALSE;
            Rewrite = TRUE;
        }
        if (mSystemConfiguration.SmbiosErase > 2) {
            mSystemConfiguration.SmbiosErase = 0;
            Rewrite = TRUE;
        }
        if (mSystemConfiguration.SmbiosEventLogFull > 1) {
            mSystemConfiguration.SmbiosEventLogFull = 0;
            Rewrite = TRUE;
        }
        if (mSystemConfiguration.LogEfiStatusCodes != 1){
            mSystemConfiguration.LogEfiStatusCodes = FALSE;
            mSystemConfiguration.ConvertEfiToLegacy = FALSE;
            Rewrite = TRUE;
        } else if (mSystemConfiguration.ConvertEfiToLegacy > 1) {
               mSystemConfiguration.ConvertEfiToLegacy = FALSE;
               Rewrite = TRUE;
        }
        if (mSystemConfiguration.METW > 99) {
            mSystemConfiguration.METW = 60;
            Rewrite = TRUE;
        }
        if ( mSystemConfiguration.MECI == 0 ) {
            mSystemConfiguration.MECI = 1;
            Rewrite = TRUE;
        }
        
        mRedirSmbiosPrivate->EraseEventLogWhenFull    = mSystemConfiguration.SmbiosEventLogFull > 0;
        mRedirSmbiosPrivate->SystemBootRecordLogging  = mSystemConfiguration.LogSystemBootRecord > 0;
        mRedirSmbiosPrivate->EfiStatusCodesEnabled    = mSystemConfiguration.LogEfiStatusCodes > 0;
        mRedirSmbiosPrivate->ConvertEfiToLegacyFlag   = mSystemConfiguration.ConvertEfiToLegacy > 0;
        mRedirSmbiosPrivate->MECI                     = mSystemConfiguration.MECI;
        mRedirSmbiosPrivate->METW                     = (0x10*(mSystemConfiguration.METW / 10)) + (mSystemConfiguration.METW % 10);

    } else {

// If Setup Variables not yet available exit 

        return EFI_NOT_FOUND;
    }


// Rewrite the NV variables if needed

    if (Rewrite) {
        VarSize = sizeof(mSystemConfiguration);

        Status = gRT->SetVariable(
                        L"Setup",
                        &gEfiSetupVariableGuid,
                        AttributesRead,   // Attributes
                        VarSize,
                        &mSystemConfiguration );
        ASSERT_EFI_ERROR (Status);
    }

    return EFI_SUCCESS;
}

/**

    Read Header values and force values should never change

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
CheckHeader ( VOID )
{
  EFI_GPNV_ERROR_HEADER_TYPE1             Header;


// Read Header values

    gBS->CopyMem ( &Header,
                &(mRedirSmbiosPrivate->CopyOfFlashArea[0]),
                sizeof(EFI_GPNV_ERROR_HEADER_TYPE1) );


// Force values that should never change, including checking MECI and METW for changes

    if (Header.PreBootEventLogResetCMOSAddress != GPNVErrorLogHeaderType1.PreBootEventLogResetCMOSAddress ||
        Header.PreBootEventLogResetCMOSAddress != GPNVErrorLogHeaderType1.PreBootEventLogResetCMOSBit ||
        Header.CMOSChecksumStartingOffset != GPNVErrorLogHeaderType1.CMOSChecksumStartingOffset ||
        Header.CMOSChecksumByteCount != GPNVErrorLogHeaderType1.CMOSChecksumByteCount ||
        Header.CMOSChecksumChecksumOffset != GPNVErrorLogHeaderType1.CMOSChecksumChecksumOffset ||
        Header.HeaderRevision             != GPNVErrorLogHeaderType1.HeaderRevision ||
        Header.RESERVED[0]                != GPNVErrorLogHeaderType1.RESERVED[0] ||
        Header.RESERVED[1]                != GPNVErrorLogHeaderType1.RESERVED[1] ||
        Header.RESERVED[2]                != GPNVErrorLogHeaderType1.RESERVED[2] ||
        Header.OEMReserved[0]                != GPNVErrorLogHeaderType1.OEMReserved[0] ||
        Header.OEMReserved[1]                != GPNVErrorLogHeaderType1.OEMReserved[1] ||
        Header.OEMReserved[2]                != GPNVErrorLogHeaderType1.OEMReserved[2] ||
        Header.OEMReserved[3]                != GPNVErrorLogHeaderType1.OEMReserved[3] ||
        Header.OEMReserved[4]                != GPNVErrorLogHeaderType1.OEMReserved[4] ||
        Header.MultipleEventCountIncrement   != mRedirSmbiosPrivate->MECI ||
        Header.MultipleEventTimeWindow       != mRedirSmbiosPrivate->METW ) {


// Create new Header info

        Header = GPNVErrorLogHeaderType1;
        Header.MultipleEventCountIncrement = mRedirSmbiosPrivate->MECI;
        Header.MultipleEventTimeWindow = mRedirSmbiosPrivate->METW;


// Write the Header bytes of Smbios Elog area on Memory Copy

        gBS->CopyMem( &(mRedirSmbiosPrivate->CopyOfFlashArea[0]),
                      &Header,
                      sizeof (EFI_GPNV_ERROR_HEADER_TYPE1) );
    }

    return EFI_SUCCESS;
}

/**

    Checks Setup Question for whether user wants to have the Smbios Log 
    Erased on restart. If So, erase here. If set to an invalid value or 
    is set to be erased on next boot, value is set to "don't erase"
    before exiting.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS - Event Log was erased successfully.
    @retval EFI_NOT_FOUND - Event Log was not erased.

**/

EFI_STATUS
CheckSetupForEraseCommand ( VOID )
{
    EFI_STATUS    Status;
    UINTN         VarSize;
    SETUP_DATA    mSystemConfiguration;
    UINT32        AttributesRead;

    VarSize = sizeof (mSystemConfiguration);
    Status = gRT->GetVariable (L"Setup", 
                               &gEfiSetupVariableGuid,
                               &AttributesRead, 
                               &VarSize,
                               &mSystemConfiguration
                              );

    if (Status == EFI_SUCCESS && mSystemConfiguration.SmbiosErase != 0) {

// Erase Log

        ResetAllDataInMemoryCopy (mRedirSmbiosPrivate);

        if ( (mSystemConfiguration.SmbiosErase | 0x01) > 0 ) {
            mSystemConfiguration.SmbiosErase = mSystemConfiguration.SmbiosErase & 0xFE;
            VarSize = sizeof(mSystemConfiguration);

            Status = gRT->SetVariable( L"Setup",
                                       &gEfiSetupVariableGuid,
	                                   AttributesRead,   // Attributes
                                       VarSize,
                                       &mSystemConfiguration
                                     );

            ASSERT_EFI_ERROR (Status);
        }

        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**

    Adds to the event log the record indicating the system has booted. This is
    always the first record on a boot (unless the log was corrupt, in which case
    the first record is the log erased record, followed by the boot record).

    @param VOID

    @return EFI_STATUS 
    @retval EFI_SUCCESS - Added to Memory Copy Properly
    @retval EFI_OUT_OF_RESOURCES - Out of space on flash part

**/

EFI_STATUS
InsertSystemBootRecord ( VOID )
{
    EFI_GPNV_ERROR_EVENT  EventToWrite;
    EFI_STATUS            Status;

    FillEventWithTimeAndDefaults (&EventToWrite);
    EventToWrite.EventType = EFI_EVENT_LOG_TYPE_SYSTEM_BOOT;
    EventToWrite.Length = sizeof (EFI_GPNV_ERROR_EVENT);


// Check if enough memory is present for the record. 
// If EFI_SUCCESS is returned, OK to place record into memory

    Status = CheckIfFull (mRedirSmbiosPrivate,EventToWrite.Length);
    if (Status != EFI_SUCCESS)
        return Status;


// Record Input Event Header into Memory Copy;

    gBS->CopyMem (
                  &( mRedirSmbiosPrivate->CopyOfFlashArea[mRedirSmbiosPrivate->NextAddress - mRedirSmbiosPrivate->StartAddress] ),
                  &EventToWrite,
                  sizeof (EFI_GPNV_ERROR_EVENT) );

    mRedirSmbiosPrivate->NextAddress += sizeof(EFI_GPNV_ERROR_EVENT);

// Increasing the RecordCount by 1 as we are adding system boot record

    (mRedirSmbiosPrivate->RecordId)++;
    return EFI_SUCCESS;
}

/**

    Fix-up internal Data pointers.

    @param Event - EFI_EVENT_NOTIFY
    @param Context - The Event relative context

    @retval VOID

**/

VOID
SmbiosElogVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    gRT->ConvertPointer (0, (VOID **) &mRedirSmbiosPrivate);
    gRT->ConvertPointer (0, (VOID **) &mSmbiosRecordId);   // To use after Virtual Address Map Change
}

/**

    This will be called when ReportStatusCode() is called and here we log 
    the events.

    @param CodeType - Type of Status Code.
    @param Value - Value to output for Status Code.
    @param Instance - Instance Number of this status code.
    @param CallerId - ID of the caller of this status code.
    @param Data - Optional data associated with this status code.

    @return EFI_STATUS

**/
EFI_STATUS
GpnvStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
    EFI_STATUS                        Status = EFI_SUCCESS;

    EFI_STATUS_CODE_ELOG_INPUT        ToWrite;

// GpnvErrorLogging module reports only SMBIOS Event log types and 
// Error codes that are defined in Platform Initialization(PI) Specification

    if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) { 

    ToWrite.StatusCodeType = CodeType;
    ToWrite.StatusCodeValue = Value;

    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE;
    *mSmbiosRecordId = 0;
    Status = mRedirSmbiosPrivate->SmbiosElog.SetEventLogData (
                                 &mRedirSmbiosPrivate->SmbiosElog,
                                 (UINT8*)&ToWrite,
                                 EfiElogRedirSmSMBIOS,
                                 FALSE,
                                 sizeof(EFI_STATUS_CODE_ELOG_INPUT),
                                 mSmbiosRecordId );

    }

    return Status;
}

/**

    Check whether it is multi event or not. If valid multi event save details.

    @param Header - Event 

    @return BOOLEAN
    @retval TRUE
    @retval FALSE

**/

BOOLEAN
IsValidMultiEventToSave (IN EFI_GPNV_ERROR_EVENT Header )
{
  UINT8      Metw = mRedirSmbiosPrivate->METW;
  UINT8      VariableDataType;
  EFI_STATUS Status;

    Status = GetEventVariableDataType (Header.EventType, &VariableDataType);
    if (EFI_ERROR (Status)) {
        return FALSE;
    }

// Check whether event is of Multi Event type or not

    if ( (VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT) &&
       (VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_HANDLE) &&
       (VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_MULTI_EVENT_SYS_MNGMT_TYPE) &&
       (VariableDataType != EFI_EVENT_LOG_VAR_DATA_TYPE_OEM_STATUS_CODE) ) {

// Return FALSE if it is not of Multi Event Type.

        return FALSE;
    } else {
        EFI_TIME           EfiTimeStamp;
        EFI_GPNV_ERROR_EVENT   CurrentTime;
        

// Get the Current time

        gRT->GetTime(&EfiTimeStamp,NULL);

        CurrentTime.Year   = (( (EfiTimeStamp.Year % 100) / 10) * 0x10) + ((EfiTimeStamp.Year % 100) % 10);
        CurrentTime.Month  = (( (EfiTimeStamp.Month)      / 10) * 0x10) + ((EfiTimeStamp.Month)      % 10);
        CurrentTime.Day    = (( (EfiTimeStamp.Day)        / 10) * 0x10) + ((EfiTimeStamp.Day)        % 10);
        CurrentTime.Hour   = (( (EfiTimeStamp.Hour)       / 10) * 0x10) + ((EfiTimeStamp.Hour)       % 10);
        CurrentTime.Minute = (( (EfiTimeStamp.Minute)     / 10) * 0x10) + ((EfiTimeStamp.Minute)     % 10);
        CurrentTime.Second = (( (EfiTimeStamp.Second)     / 10) * 0x10) + ((EfiTimeStamp.Second)     % 10);


// Check whether event is METW min or not.

        if ( TimeDifference (CurrentTime, Header) >= Metw ) {


// Return FALSE if event does not fall in METW min
 
            return FALSE;
        } else {

// Return TRUE as event is of Multi Event Type and it present in METW min.

            return TRUE;
        }
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
