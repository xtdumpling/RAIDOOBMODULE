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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmbiosElogSetupScreen.c
    Smbios Elog Setup Screen Implementation

**/

// Includes

#define DEBUG_ERROR     0x80000000

//---------------------------------------------------------------------------

#include <Efi.h>
#include "SmbiosElogSetupScreen.h"

//---------------------------------------------------------------------------

#if (OemSmbiosErrorLogging == 1)
#include <AmiCspLib.h>
#include "OemOperationTokenTable.h"
#include "OemSubClassTokenTable.h"
#endif

#if (CustomEventTypeAndStrings == 1)
#include "EventLogsCustomStringFunctions.h"
#endif

#include "SmbiosElogLookupTable.h"
#include <Include/AmiLib.h>

UINT64 CurrentRecordId = 0;
BOOLEAN IsCustom;

#if (OemSmbiosErrorLogging == 1)
  #define END_OF_OEM_OP_TOKEN_LIST {0xFFFFFFFF, 0, STR_ERROR_UNRECOGNIZED}
  OPERATION_LOOKUP_TABLE mOEMOperationToken[] = {
    OEM_Error_Op_Table END_OF_OEM_OP_TOKEN_LIST };

  #define END_OF_OEM_SUBCLASS_TOKEN_LIST {0xFFFFFFFF, STR_ERROR_UNRECOGNIZED}
  STATUS_CODE_LOOKUP_TABLE mOEMClassSubClassToken[] = {
    OEM_SubClass_Table END_OF_OEM_SUBCLASS_TOKEN_LIST };
#endif

#if (CustomEventTypeAndStrings == 1)
  typedef STRING_REF (EVENT_LOGS_CUSTOM_STRING_FUNCTION) (
    IN EFI_GPNV_ERROR_EVENT* GpnvEvent,
    IN BOOLEAN IsDescription,
    OUT BOOLEAN* Match
  );


  extern EVENT_LOGS_CUSTOM_STRING_FUNCTION EventLogCustomStringFunctions
    EndOfInitList;

  EVENT_LOGS_CUSTOM_STRING_FUNCTION* CustomStringFunctions[] = {
    EventLogCustomStringFunctions NULL
  };
#endif

EFI_ERROR_SETUP_SCREEN_INSTANCE mPrivateData;
EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL DynamicCallbackInfo;
EFI_GUID gErrorManagerGuid         = ERROR_MANAGER_GUID;



/**
    Entry point of the error screen setup driver.

    @param ImageHandle - Handle of binary image.
    @param SystemTable - System table pointer.

    @return EFI_STATUS
    @retval EFI_SUCCESS - Driver initialized successfully
    @retval EFI_LOAD_ERROR - Failed to Initialize or has been loaded
    @retval EFI_OUT_OF_RESOURCES - Could not allocate needed resources

**/

EFI_STATUS
InstallErrorSetupScreen (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    EFI_HANDLE TempHandle;

// Initialize the Library.
// Locate protocols and cache pointers for later use

    Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid,
                                   NULL, (VOID **)&mPrivateData.HiiString);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid,
                                   NULL, (VOID **)&mPrivateData.HiiDatabase);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    mPrivateData.HiiHandle = NULL;
    mPrivateData.SetupScreenBuffer = NULL;
    mPrivateData.TokenCount = 0;

// Save Information for using Dynamic page with callback to
// fill event log info

    DynamicCallbackInfo.CallbackFunction = ErrorScreenCallbackRoutine;
    DynamicCallbackInfo.NumberInDynamicList = 1;
    DynamicCallbackInfo.TitleStringReference = STRING_TOKEN(STR_SMBIOS_VIEW);

    TempHandle = NULL;

    Status = gBS->InstallProtocolInterface(&TempHandle,
        &gAmiEventLogsDynamicGuid, EFI_NATIVE_INTERFACE, &DynamicCallbackInfo );

    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

/**
    Function called from the Error Screen Callback.  Will compare key and call
    appropriate function.

    @param This - Pointer to the instance of ConfigAccess Protocol
    @param Action - Action, that setup browser is performing
    @param KeyValue - A unique value which identifies control that caused 
                      callback.
    @param Type - Value type of setup control data
    @param Value - Pointer to setup control data
    @param ActionRequest - Pointer where to store requested action

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
ErrorScreenCallbackRoutine (
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest )
{
  EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL*  HiiHandleProtocolPtr;
  EFI_STATUS Status;

// Make sure HII Handle for Strings in Event Log Page is present

  if (mPrivateData.HiiHandle == NULL) {

      Status = gBS->LocateProtocol( &gAmiEventLogsHiiHandleGuid,
                                    NULL,
                                    (VOID **)&HiiHandleProtocolPtr );
      if (Status != EFI_SUCCESS)
        return EFI_SUCCESS;

      mPrivateData.HiiHandle = HiiHandleProtocolPtr->HiiHandle;
      mPrivateData.SetupScreenBuffer = HiiHandleProtocolPtr->BufferPtr;
  }

// If KeyValue is SMBIOS_VIEW_FORM_KEY then make CurrentRecordId as zero.
// KeyValue other than SMBIOS_VIEW_FORM_KEY, then do not alter the 
// CurrentRecordId.

  if ( KeyValue == SMBIOS_VIEW_FORM_KEY) {
    CurrentRecordId = 1;
  }
  UpdateFormSetWithDataHub();

  return EFI_SUCCESS;
}

/**

    Search the input table for a matching value and return the token associated
    with that value.  Well formed tables will have the last value == 0 and will
    return a default token.

    @param Table - Pointer to first entry in an array of table entries.
    @param Value - Value to look up.
    @param Token - Token returned.

    @return EFI_STATUS 
    @retval EFI_SUCCESS - The function always returns success.

**/
EFI_STATUS
StatusCodeMatchString (
  IN  STATUS_CODE_LOOKUP_TABLE  *Table,
  IN  UINT32                    Value,
  OUT STRING_REF                *Token )
{
  UINTN Current;

  ASSERT (Table);
  ASSERT (Token);

  Current = 0;
  *Token = 0;

  while (!*Token) {

// Found token if values match or current entry is the last entry.

    if ( (Table[Current].Value == (-1)) || (Table[Current].Value == Value) ) {
      *Token = Table[Current].Token;
    }
    Current++;
  }

  return EFI_SUCCESS;
}

/**

    Search the operation table for a matching value and return the token 
    and associated data. Well formed tables has the last value == 0 and 
    returns a default token. If token is not found in the table created for 
    EFI error codes, then search is done in OEM defined lookup table for 
    Error operations, only when OemSmbiosErrorLogging token is turned on.

    @param Value - Value to look up.
    @param IsCustom - Boolean to indicate if operation token is created 
                      specifically for the error.
    @param OperationToken - Operation Token returned.

    @return EFI_STATUS
    @retval EFI_SUCCESS - The function always returns success.

**/
EFI_STATUS
StatusCodeMatchOperation (
  IN  UINT32     Value,
  OUT BOOLEAN    *IsCustom,
  OUT STRING_REF *OperationToken )
{
  UINTN Current;
  EFI_STATUS Status = EFI_NOT_FOUND;

  Current = 0;
  *IsCustom = FALSE;
  *OperationToken = 0;

  while (!*OperationToken) {

// Found token if values match. Else, search in the OEM operation table 
// constructed for a suitable match.

    if ( (mOperationToken[Current].Value == (-1)) ||
         (mOperationToken[Current].Value == Value) ) {
      *OperationToken = mOperationToken[Current].Token;
      *IsCustom = mOperationToken[Current].IsCustomToken;
      Status = EFI_SUCCESS;
      break;
    }
    Current++;
  }

#if (OemSmbiosErrorLogging == 1)
  if(mOperationToken[Current].Value == -1) {
    Current = 0;
    *OperationToken = 0;
    while (!*OperationToken) {

// Found token if values match or current entry is the last entry.

      if ( (mOEMOperationToken[Current].Value == (-1)) ||
         (mOEMOperationToken[Current].Value == Value) ) {
        *OperationToken = mOEMOperationToken[Current].Token;
        *IsCustom = mOEMOperationToken[Current].IsCustomToken;
        Status = EFI_SUCCESS;
        break;
      }
      Current++;
    }
  }
#endif
  return Status;
}

/**

    Inserts an error into the form set if it is required to be displayed.
    Does not always require that CodeType and Value are valid if the EventType
    in the Smbios Event Header is not of Type 0x80.

    @param EventPtr - Pointer to Smbios Event Header.
    @param ExtendedData - This is the extended data for the event. 
    @param CodeType - EFI Status Code Type.
    @param Value - EFI Status Code Value.
    @param FormBuffer - Pointer to IFR text to update.
    @param Count - Error number to add.

    @return EFI_STATUS
    @retval EFI_SUCCESS - The function completed successfully.
    @retval EFI_OUT_OF_RESOURCES - Cannot add any more errors.

**/
EFI_STATUS
AddErrorToFormSet (
  IN EFI_GPNV_ERROR_EVENT  *EventPtr,
  IN UINT8                 *ExtendedData,
  IN EFI_STATUS_CODE_TYPE  CodeType,
  IN EFI_STATUS_CODE_VALUE Value,
  IN OUT ERROR_LIST        **FormBuffer,
  IN OUT UINT16            *Count )
{
  STRING_REF ErrorDetailToken;
  STRING_REF ErrorDescriptionToken;
  EFI_STATUS Status;

// GpnvErrorLogging module reports only SMBIOS Event log types and 
// Error codes that are defined in Platform Initialization(PI) Specification


      if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
      return EFI_SUCCESS;
      }
      
//  Build error detail and description

      Status = BuildErrorDescription (EventPtr, ExtendedData, CodeType, Value, &ErrorDescriptionToken);
      if (EFI_ERROR(Status) && (Status != EFI_NOT_FOUND)) {
        return Status;
      }

      Status = BuildErrorDetails (EventPtr, ExtendedData, CodeType, Value, &ErrorDetailToken);
      if (EFI_ERROR(Status) && (Status != EFI_NOT_FOUND)) {
        return Status;
      }

      (*FormBuffer)->Handle = (VOID*)mPrivateData.HiiHandle;
      (*FormBuffer)->Summary = ErrorDetailToken;
      (*FormBuffer)->DetailInfo = ErrorDescriptionToken;

      (*Count)++;
      (*FormBuffer) = (ERROR_LIST*)( (UINT8*)(*FormBuffer) + sizeof(ERROR_LIST) );

      return EFI_SUCCESS;
}


/**

    This performs word-by-word copy of a Unicode string.

    @param Destination - Pointer to a CHAR16 string buffer to receive the 
                         contents
    @param Source - Pointer to a null-terminated CHAR16 string to be copied.

    @return VOID

    @note
    Assumes the destination string is large enough (error checking must
    be made by caller).

**/

VOID
StrCpy16 (
  IN CHAR16   *Destination,
  IN CHAR16   *Source )
{
  while (*Source) {
    *(Destination++) = *(Source++);
  }
  *Destination = 0;
}


/**

    This is a worker function to extract the SMBIOS string from the
    specified SMBIOS structure data.

    @param StructureHandle - SMBIOS structure data pointer.
    @param StringNumber - The string number to get.
    @param StringData - The string is copied here.

    @return EFI_STATUS
    @retval EFI_SUCCESS or valid error code.

    @note
    This function is called by GetSmbiosDeviceString.
    Assumes the string is large enough (error checking must be made by caller).

**/

EFI_STATUS
GetSmbiosStringByNumber (
    IN  UINT8    *StructureHandle,
    IN  UINT8    StringNumber,
    OUT UINT8    **StringData )
{
   *StringData = StructureHandle +
                ((SMBIOS_STRUCTURE_HEADER*)StructureHandle)->Length;

    while (--StringNumber) {
        while(**StringData != 0) {
            (*StringData)++;
        }
        (*StringData)++;
    }

    return EFI_SUCCESS;
}


/**

    Simple utility function to convert an ASCII string to Unicode.

    @param UnicodeStr - Converted string.
    @param AsciiStr - String to be converted

    @return CHAR16 * Same as UnicodeStr to allow use in an assignment.

    @note  N/A

**/

CHAR16 *
Ascii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  CHAR8          *AsciiStr )
{
  CHAR16  *Str;

  Str = UnicodeStr;

  while (TRUE) {

    *(UnicodeStr++) = (CHAR16) *AsciiStr;

    if (*(AsciiStr++) == '\0') {
      return Str;
    }
  }
}


/**

    The function finds the string associated with the SMBIOS handle.

    @param Event - The event for which the string is to be found
    @param ExtendedData - This is the extended data for the event, where the
                          first 16 bits contains the SMBIOS handle.
    @param StringBuffer - This buffer will be updated if the string is found.

    @return EFI_STATUS
    @retval EFI_SUCCESS
    @retval EFI_NOT_FOUND

    @note  
    Assumes call before runtime (EFI_SMBIOS_PROTOCOL is available).
    Assumes StringBuffer is large enough to hold the string.

**/

EFI_STATUS
GetSmbiosDeviceString (
    EFI_GPNV_ERROR_EVENT      *Event,
    UINT8                     *ExtendedData,
    CHAR16                    *StringBuffer)
{
    static AMI_SMBIOS_PROTOCOL *SmbiosProtocol = NULL;
    EFI_STATUS                Status = EFI_SUCCESS;
    UINT8                     *Structure;      // Generic byte pointer
    UINT16                    StructureSize;
    UINT8                     i;
    SMBIOS_MEMORY_DEVICE_INFO *Type17;
    BOOLEAN                   Found = FALSE;
    SMBIOS_SYSTEM_SLOTS_INFO  *Type9;
    CHAR8                     *StrAsciiTmpPtr;
    CHAR16                    StrUnicode[0xFF];
    UINT16                    EventHandle = *((UINT16*)(&ExtendedData[0]));

    if (SmbiosProtocol == NULL){
      Status = gBS->LocateProtocol( &gAmiSmbiosProtocolGuid, NULL, (VOID **)&SmbiosProtocol );
      if (EFI_ERROR(Status)){
        goto EXIT;
      }
    }

    if (Event->EventType == EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR
        || Event->EventType == EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR)
    {

// Find the matching Type 17 handle

        for (i = 1; !EFI_ERROR(Status); i++){
            Status = SmbiosProtocol->SmbiosReadStrucByType(
                      17, // Type 17
                      i,  // Instance
                      &Structure,
                      &StructureSize);
            if (!EFI_ERROR(Status)){
              Type17 = (SMBIOS_MEMORY_DEVICE_INFO*)Structure;

              if (Type17->StructureType.Handle == EventHandle){
                GetSmbiosStringByNumber(Structure, Type17->DeviceLocator,
                    (UINT8 **)&StrAsciiTmpPtr);
                StrCpy16(StringBuffer, Ascii2Unicode(StrUnicode, StrAsciiTmpPtr));
                gBS->FreePool(Structure);
                Found = TRUE;
                break;
              }

// Free the memory
              gBS->FreePool(Structure);
            }
        }
        if (EFI_ERROR(Status)){
            StrCpy16(StringBuffer, L"No DIMM Information");
        }
    }

    if (Event->EventType == EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR
        || Event->EventType == EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR)
    {

// Find the matching Type 9 handle

        for (i = 1; !EFI_ERROR(Status); i++){
            Status = SmbiosProtocol->SmbiosReadStrucByType(
                      9, // Type 9
                      i,  // Instance
                      &Structure,
                      &StructureSize);
            if (!EFI_ERROR(Status)){
              Type9 = (SMBIOS_SYSTEM_SLOTS_INFO*)Structure;

              if (Type9->StructureType.Handle == EventHandle){
                GetSmbiosStringByNumber(Structure, Type9->SlotDesig,
                    (UINT8 **)&StrAsciiTmpPtr);
                StrCpy16(StringBuffer, Ascii2Unicode(StrUnicode, StrAsciiTmpPtr));
                gBS->FreePool(Structure);
                Found = TRUE;
                break;
              }

// Free the memory
              gBS->FreePool(Structure);
            }
        }
        if (EFI_ERROR(Status)){
            StrCpy16(StringBuffer, L"No Slot Information");
        }
    }

EXIT:
    if (!EFI_ERROR(Status) && Found){
        Status = EFI_SUCCESS;
    }
    else {
        Status = EFI_NOT_FOUND;
    }

    return Status;
}


/**

    Builds error description to be displayed in the help field of Error
    Setup Screen page.

    @param EventPtr - Pointer to Smbios Event Header.
    @param ExtendedData - This is the extended data for the event. 
    @param CodeType - EFI Status Code Type.
    @param Value - EFI Status Code Value.
    @param Token - Returned token representing the error description message.

    @return EFI_STATUS
    @retval EFI_SUCCESS - The function completed successfully.
    @retval EFI_NOT_FOUND - Should not be viewed as an error.

**/
EFI_STATUS
BuildErrorDescription (
  IN EFI_GPNV_ERROR_EVENT        *EventPtr,
  IN UINT8                       *ExtendedData,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  OUT STRING_REF                 *Token )
{

  EFI_STATUS                        Status;
  STRING_REF                        ClassSubClassToken;
  STRING_REF                        OperationToken;
  STRING_REF                        StatusCodeDataToken;
  CHAR16                            *Buffer;
  CHAR16                            *Buffer2;
  UINTN                             BufferLength;
  UINT32                            Counter=0;
  UINT32                            FirstDWORD;
  UINT32                            SecondDWORD;

  #if SMBIOSELOG_GET_SMBIOS_DEVICE_STRING
  CHAR16                            SmbiosString[STRING_BUFFER_LENGTH]={0};
  #endif

// For every call, this boolean should be made FALSE

  IsCustom = FALSE;
  StatusCodeDataToken = STRING_TOKEN (STR_EMPTY);



// Check if identical description token was already created, if so use it to save space

  while (Counter < mPrivateData.TokenCount) {
    if (  (!mPrivateData.SavedTokens[Counter].IsDetail)
        &&  (CodeType == mPrivateData.SavedTokens[Counter].CodeType)
        &&  (Value == mPrivateData.SavedTokens[Counter].Value)
        &&  (EventPtr->EventType == mPrivateData.SavedTokens[Counter].Event.EventType)
        &&  (EventPtr->Length == mPrivateData.SavedTokens[Counter].Event.Length) ) {
        *Token = mPrivateData.SavedTokens[Counter].Token;
        return EFI_SUCCESS;
    }
    Counter++;
  }

// Allocate Buffer

  Buffer = (CHAR16 *)AllocateZeroPool(STRING_BUFFER_LENGTH);

  Buffer2 = (CHAR16 *)AllocateZeroPool(STRING_BUFFER_LENGTH);

// Create the description header

  BufferLength = STRING_BUFFER_LENGTH;
  Status = GpnvHiiGetString (
        STRING_TOKEN (STR_DESCRIPTION_HEADER),
        Buffer,
        &BufferLength,
        NULL);

  ASSERT_EFI_ERROR (Status);

// Adding the new line to the description header

  Wcscpy (Buffer2, Buffer);
  Swprintf(Buffer, L"%s\n", Buffer2);
  Wcscpy (Buffer2, Buffer);

  

  BufferLength = (UINT16) (STRING_BUFFER_LENGTH - Wcslen(Buffer));

// If not an EFI error, do work for legacy (0x80 and above is OEM)

  if (EventPtr->EventType < 0x80) {


    switch (EventPtr->EventType) {
        case EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_PARITY_MEMORY_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_PARITY_MEMORY_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_BUS_TIME_OUT:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_BUS_TIME_OUT);
            break;
        case EFI_EVENT_LOG_TYPE_IO_CHANNEL_CHECK:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_IO_CHANNEL_CHECK);
            break;
        case EFI_EVENT_LOG_TYPE_SOFTWARE_NMI:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SOFTWARE_NMI);
            break;
        case EFI_EVENT_LOG_TYPE_POST_MEMORY_RESIZE:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_POST_MEMORY_RESIZE);
            break;
        case EFI_EVENT_LOG_TYPE_POST_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_POST_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_PCI_PARITY_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_CPU_FAILURE:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_CPU_FAILURE);
            break;
        case EFI_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMER_TIME_OUT:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMER_TIME_OUT);
            break;
        case EFI_EVENT_LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED);
            break;
        case EFI_EVENT_LOG_TYPE_LOGGING_DISABLED_FOR_EVENT_TYPE:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_LOGGING_DISABLED_FOR_EVENT_TYPE);
            break;
        case EFI_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SYSTEM_LIMIT_EXCEEDED);
            break;
        case EFI_EVENT_LOG_TYPE_ASYN_HW_TIMER_EXPIRED:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_ASYN_HW_TIMER_EXPIRED);
            break;
        case EFI_EVENT_LOG_TYPE_SYSTEM_CONFIG_INFORMATION:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SYSTEM_CONFIG_INFORMATION);
            break;
        case EFI_EVENT_LOG_TYPE_HARD_DISK_INFORMATION:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_HARD_DISK_INFORMATION);
            break;
        case EFI_EVENT_LOG_TYPE_SYSTEM_RECONFIGURED:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SYSTEM_RECONFIGURED);
            break;
        case EFI_EVENT_LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR);
            break;
        case EFI_EVENT_LOG_TYPE_LOG_AREA_RESET:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_LOG_AREA_RESET);
            break;
        case EFI_EVENT_LOG_TYPE_SYSTEM_BOOT:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_SYSTEM_BOOT);
            break;
        case EFI_EVENT_LOG_TYPE_END_OF_LOG:
            ClassSubClassToken = STRING_TOKEN (STR_EVENT_LOG_TYPE_END_OF_LOG);
            break;
        default:
            ClassSubClassToken = STRING_TOKEN (STR_ERROR_UNRECOGNIZED);
            break;

    }

// Write generic title

   Status = GpnvHiiGetString (
        ClassSubClassToken,
        &Buffer[Wcslen(Buffer)],
        &BufferLength,
        NULL);

  ASSERT_EFI_ERROR (Status);
  BufferLength = (UINT16) (STRING_BUFFER_LENGTH - Wcslen(Buffer));

// If extended data needs to be added to string, do it here.

    switch (EventPtr->EventType) {

// For all events that possibly have SMBIOS handles, attempt to append
// the corresponding SMBIOS string.
    case EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR:
    case EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR:
    case EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR:
    case EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR:
        #if SMBIOSELOG_GET_SMBIOS_DEVICE_STRING
          if (!EFI_ERROR(GetSmbiosDeviceString(EventPtr, ExtendedData,
                         SmbiosString)))
          {
            Wcscpy (Buffer2, Buffer);
            Swprintf(Buffer, L"%s (%s)", Buffer2, SmbiosString);
          }
        #endif
        break;

        case EFI_EVENT_LOG_TYPE_POST_ERROR:


// Rotate through all the bits and for each set bit add string
// Adding sizeof VariableDataType(UINT8) to Extended data to get 
// First Dword and Second Dword

            FirstDWORD = *( (UINT32*)(ExtendedData + sizeof(UINT8)) );
            SecondDWORD = *( (UINT32*)(ExtendedData + sizeof(UINT8) + sizeof(UINT32) ) );

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CHANNEL2_TIMER_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Channel 2 Timer,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_MASTER_PIC_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Master PIC,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_SLAVE_PIC_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Slave PIC,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_BATTERY_FAILURE) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS Battery Failure,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_SYS_OPTIONS_NOT_SET) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS System Options Not Set,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_CKSUM_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS Cksum,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_CONFIG_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS Config,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_MOUSE_KEYBOARD_SWAP) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Mouse/KBD Swap,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_KBD_LOCKED) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s KBD Locked,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_KBD_NOT_FUNC) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s KBD Not Func,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_KBD_CONTR_NOT_FUNC) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s KBD Controller Not Func,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_MEM_SIZE_DIFF) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS Mem Size Difference,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_MEM_DECREASED_IN_SIZE) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Mem Decreased In Size,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CACHE_MEM_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Cache Mem,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_FLOPPY_DRV_0_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Floppy Drive 0,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_FLOPPY_DRV_1_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Floppy Drive 1,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_FLOPPY_CONTR_FAILURE) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Floppy Controller Failure,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_NUM_ATA_DRVS_REDUCED_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Num ATA Drives Reduced,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_CMOS_TIME_NOT_SET) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s CMOS Time Not Set,", Buffer2);
                }

            if ( (FirstDWORD & EFI_POST_BITMAP_DW1_DDC_MONITOR_CONFIG_CHANGE) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s DDC Monitor Config Change,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PCI_MEM_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PCI Mem Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PCI_IO_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PCI I/O Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PCI_IRQ_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PCI IRQ Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PNP_MEM_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PNP Mem Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PNP_32BIT_MEM_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PNP 32bit Mem Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PNP_IO_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PNP I/O Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PNP_IRQ_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PNP IRQ Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PNP_DMA_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s PNP DMA Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_BAD_PNP_SERIAL_ID) > 0 )
                {
                Wcscpy(Buffer2, Buffer);
                Swprintf(Buffer, L"%s Bad PNP Serial ID,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_BAD_PNP_RESOURCE_DATA_CKSUM) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Bad PNP Resource Data Cksum,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_STATIC_RESOURCE_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Static Resource Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_NVRAM_CKSUM_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s NVRAM Cksum,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_SYS_BOARD_DEV_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s System Board Device Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PRIMARY_OUTPUT_DEV_NOT_FOUND) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Pri Output Device Not Found,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PRIMARY_INPUT_DEV_NOT_FOUND) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Pri Input Device Not Found,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PRIMARY_BOOT_DEV_NOT_FOUND) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Primary Boot Device Not Found,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_NVRAM_CLEARED_BY_JUMPER) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s NVRAM Cleared By Jumper,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_NVRAM_DATA_INVALID) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s NVRAM Data Invalid,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_FDC_RESOURCE_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s FDC Resource Conflict,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PRI_ATA_CNTR_RESOURCE_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Pri ATA Controller Resource,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_SEC_ATA_CNTR_RESOURCE_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Sec ATA Controller Resource,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_PARALLEL_PORT_RESOURCE_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Parallel Port Resource,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_SERIAL_PORT_1_RESOURCE_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Serial Port 1 Resource,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_SERIAL_PORT_2_RESOURCE_ERROR) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Serial Port 2 Resource,", Buffer2);
                }

            if ( (SecondDWORD & EFI_POST_BITMAP_DW2_AUDIO_RESOURCE_CONFLICT) > 0 )
                {
                Wcscpy (Buffer2, Buffer);
                Swprintf(Buffer, L"%s Audio Resource,", Buffer2);
                }

            break;
        default:
            break;

    }



  } // END LEGACY STRING

  else { // START EFI STRING

// Check if this is a StatusCode event.
    if (EventPtr->EventType == EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE) {


// Extract fields from the  operation table

      Status = StatusCodeMatchOperation (
          (Value & (EFI_STATUS_CODE_CLASS_MASK | EFI_STATUS_CODE_SUBCLASS_MASK | EFI_STATUS_CODE_OPERATION_MASK)),
          &IsCustom,
          &OperationToken );

      if (EFI_ERROR(Status)) {
          gBS->FreePool(Buffer);
          gBS->FreePool(Buffer2);
          return Status;
      }
    } else {
#if (CustomEventTypeAndStrings == 1)
        UINTN i = 0;
        BOOLEAN Match = FALSE;

// Custom entry not in mOperationToken table.
// Call eLink functions and see if one of them can provide.

        for(i = 0; CustomStringFunctions[i]; i++) {
            OperationToken = CustomStringFunctions[i](EventPtr, TRUE, &Match);
            if (Match) break;
        }
        if (Match) IsCustom = TRUE;
        else OperationToken = STRING_TOKEN(STR_ERROR_UNRECOGNIZED);
#else
        OperationToken = STRING_TOKEN(STR_ERROR_UNRECOGNIZED);
#endif
    }


//  If the operation token is specific to the error, then use the
//  token message.  If the operation token is generic, create an error
//  message by concatenating the Class/SubClass with the Operation.
//  The Class/SubClass serves as the subject
//  while the Operation serves as the verb.

      if (IsCustom) {

// Get the (description text) for custom token

          Status = GpnvHiiGetString  (
                    OperationToken,
                    &Buffer[Wcslen(Buffer)],
                    &BufferLength,
                    NULL);

          ASSERT_EFI_ERROR (Status);
          BufferLength = (UINT16) (STRING_BUFFER_LENGTH - Wcslen(Buffer));
      } else {

// Build the (description text) for "generic" error messages here

          StatusCodeMatchString (
              mClassSubClassToken,
              (Value & (EFI_STATUS_CODE_CLASS_MASK | EFI_STATUS_CODE_SUBCLASS_MASK)),
              &ClassSubClassToken );
#if (OemSmbiosErrorLogging == 1)

// If a matching string for SubClass is not found, then search for a 
// match in OEM SubClass token table constructed

          if(ClassSubClassToken == STRING_TOKEN (STR_ERROR_UNRECOGNIZED)) {
            StatusCodeMatchString (
                mOEMClassSubClassToken,
                (Value & (EFI_STATUS_CODE_CLASS_MASK | EFI_STATUS_CODE_SUBCLASS_MASK)),
               &ClassSubClassToken );
          }
#endif


// Class/Subclass is the subject of the error message


          Status = GpnvHiiGetString (
                    ClassSubClassToken,
                    &Buffer[Wcslen(Buffer)],
                    &BufferLength,
                    NULL);

          ASSERT_EFI_ERROR (Status);

// Adding the new line to the description header

          Wcscpy (Buffer2, Buffer);
          Swprintf(Buffer, L"%s ", Buffer2);

          BufferLength = (UINT16)( STRING_BUFFER_LENGTH - Wcslen(Buffer) );


// Operation is the verb of the error message


          Status = GpnvHiiGetString (
                    OperationToken,
                    &Buffer[Wcslen(Buffer)],
                    &BufferLength,
                    NULL);

          ASSERT_EFI_ERROR (Status);

      } // Generic description build

      Wcscpy (Buffer2, Buffer);
      Swprintf(Buffer, L"%s ", Buffer2);

      BufferLength = (UINT16)( STRING_BUFFER_LENGTH - Wcslen(Buffer) );


// Now add the string from ErrorManagerStatusCodeDataHandler


      Status = GpnvHiiGetString (
                StatusCodeDataToken,
                &Buffer[Wcslen(Buffer)],
                &BufferLength,
                NULL);

      ASSERT_EFI_ERROR (Status);
      Wcscpy (Buffer2, Buffer);
      Swprintf(Buffer, L"%s ", Buffer2);

  } // END CREATING EFI STRING


// Create message string

  OperationToken = 0;

  OperationToken = HiiSetString( mPrivateData.HiiHandle, OperationToken, Buffer, NULL );
  ASSERT (OperationToken != 0);

  gBS->FreePool(Buffer);
  gBS->FreePool(Buffer2);
  *Token = OperationToken;

// Don't save tokens for custom strings.
  if ((mPrivateData.TokenCount +1 < MAX_TOKENS_TO_SAVE) && (!IsCustom)) {
    mPrivateData.SavedTokens[mPrivateData.TokenCount].IsDetail = FALSE;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Event = *EventPtr;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].CodeType = CodeType;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Value = Value;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Token = *Token;

    mPrivateData.TokenCount++;
  }

  return EFI_SUCCESS;
}

/**

    Builds error detail to be displayed in Error Manager page.

    @param EventPtr - Pointer to Smbios Event Header.
    @param ExtendedData - This is the extended data for the event. 
    @param CodeType - EFI Status Code Type.
    @param Value - EFI Status Code Value.
    @param Token - Returned token representing the error description message.

    @return EFI_STATUS
    @retval EFI_SUCCESS - The function completed successfully.

**/

EFI_STATUS
BuildErrorDetails (
  IN EFI_GPNV_ERROR_EVENT    *EventPtr,
  IN UINT8                   *ExtendedData,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  OUT STRING_REF              *Token )
{
  EFI_STATUS      Status;
  STRING_REF      SeverityToken;
  CHAR16          *Buffer;
  CHAR16          *TempBuffer;
  UINTN           BufferLength;
  STRING_REF      ErrorDetailToken;
  UINT32          Counter = 0;
// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
  EFI_GUID                   SmbiosGuid = EFI_SMBIOS_PROTOCOL_GUID;
  UINT16                     SmbiosHandle = 0;       // SMBIOS handle for Type 17 & 19 events  
  AMI_SMBIOS_PROTOCOL        *AmiSmbiosProtocol;
  UINT8                      *Structure;      // generic byte pointer
  UINT16                     StructureSize;
  SMBIOS_MEMORY_DEVICE_INFO  *Type17;
  CHAR8                      *StrAsciiTmpPtr;
  CHAR16                     StrUnicode[0xFF];
  UINT16                     *EventHandle = (UINT16*)&ExtendedData[0];  // SMBIOS handle for Type 17 & 19 events
  BOOLEAN                     MemoryErrorFound = FALSE;
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01
// SMC R1.00 <<

// Check if identical detail token was already created, if so use it to 
// save space

  if (!IsCustom) {
    while (Counter < mPrivateData.TokenCount)
     {
       if (  (mPrivateData.SavedTokens[Counter].IsDetail)
        &&  (CodeType == mPrivateData.SavedTokens[Counter].CodeType)
        &&  (Value == mPrivateData.SavedTokens[Counter].Value)
        &&  (EventPtr->EventType == mPrivateData.SavedTokens[Counter].Event.EventType)
        &&  (EventPtr->Length == mPrivateData.SavedTokens[Counter].Event.Length)
        &&  (EventPtr->Year == mPrivateData.SavedTokens[Counter].Event.Year)
        &&  (EventPtr->Month == mPrivateData.SavedTokens[Counter].Event.Month)
        &&  (EventPtr->Day == mPrivateData.SavedTokens[Counter].Event.Day)
        &&  (EventPtr->Hour == mPrivateData.SavedTokens[Counter].Event.Hour)
        &&  (EventPtr->Minute == mPrivateData.SavedTokens[Counter].Event.Minute)
        &&  (EventPtr->Second == mPrivateData.SavedTokens[Counter].Event.Second)    )
        {
          *Token = mPrivateData.SavedTokens[Counter].Token;
           return EFI_SUCCESS;
        }
        Counter++;
     }
  }

// Allocate String Memory

  Buffer = (CHAR16 *)AllocateZeroPool(STRING_BUFFER_LENGTH);

  TempBuffer = (CHAR16 *)AllocateZeroPool(STRING_BUFFER_LENGTH);

// Create Beginning of String With Date and Time

  BufferLength = (UINT16) STRING_BUFFER_LENGTH;

  Swprintf(Buffer, L"%02X/%02X/%02X  %02X:%02X:%02X   ", EventPtr->Month, EventPtr->Day, EventPtr->Year,
            EventPtr->Hour, EventPtr->Minute, EventPtr->Second);

// If not an EFI error, we know it's one of the Standard Smbios Errors

  if (EventPtr->EventType == EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE) {

// Put In Status Code

    Wcscpy (TempBuffer, Buffer);
    Swprintf(Buffer, L"%sEFI %08X    ", TempBuffer, Value);

// Get the severity token

    StatusCodeMatchString (
      mSeverityToken,
      (CodeType & EFI_STATUS_CODE_SEVERITY_MASK),
      &SeverityToken );

    BufferLength = (UINT16) (BufferLength - ERROR_SEVERITY_COLUMN - 1);

    Status = GpnvHiiGetString (
                SeverityToken,
                &Buffer[ERROR_SEVERITY_COLUMN],
                &BufferLength,
                NULL);

    ASSERT_EFI_ERROR (Status);

  } else {

// Add String For Smbios Standard Types

    Wcscpy (TempBuffer, Buffer);
// SMC R1.00 >>
#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01    
    // Find the SMBIOS string associated with the event type
    Status = gBS->LocateProtocol (&gAmiSmbiosProtocolGuid,
                                  NULL,
                                  &AmiSmbiosProtocol);
    ASSERT_EFI_ERROR (Status);
    
    switch (EventPtr->EventType ) {
      case EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR:
      case EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR:
        if (AmiSmbiosProtocol != NULL ){
          SmbiosHandle = ((MULTIPLE_EVENT_HANDLE_DATA*)ExtendedData)->SmbiosHandle;
          Status = AmiSmbiosProtocol->SmbiosReadStructure (
                                         SmbiosHandle,
                                         &Structure,
                                         &StructureSize);
          if (!EFI_ERROR (Status)) {
            Type17 = (SMBIOS_MEMORY_DEVICE_INFO*)Structure;

            if (Type17->StructureType.Type == 17) {
              GetSmbiosStringByNumber (Structure, Type17->DeviceLocator, &StrAsciiTmpPtr);
              Swprintf (Buffer, L"%sSMBIOS 0x%02X    %s", TempBuffer, EventPtr->EventType, Ascii2Unicode (StrUnicode, StrAsciiTmpPtr));
              MemoryErrorFound = TRUE;
            }
            FreePool (Structure);
          }                                              
        } //END..... if (AmiSmbiosProtocol != NULL ){ //
        if (!MemoryErrorFound) {
          Swprintf (Buffer, L"%sSMBIOS 0x%02X    No DIMM Information", TempBuffer, EventPtr->EventType);
        }        
        break;
      case EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR:
      case EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR:
        SmbiosHandle = ((MULTIPLE_EVENT_HANDLE_DATA*)ExtendedData)->SmbiosHandle;
        Swprintf (
          Buffer,
          L"%sSMBIOS 0x%02X    Bus%02X(DevFn%02X)",
          TempBuffer,
          EventPtr->EventType,
          (UINT8)(SmbiosHandle >> 8),
          (UINT8)(SmbiosHandle & 0xFF)
          );        
        break;  
      default:
        Swprintf(Buffer, L"%sSMBIOS 0x%02X    N/A", TempBuffer, EventPtr->EventType);      
        break;   
    }//END...    switch (EventPtr->EventType) {
    
    
  
#endif // #if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT == 0x01  
  }

// Create a string with Error Code, Severity and Instance

  ErrorDetailToken = 0;
  ErrorDetailToken = HiiSetString( mPrivateData.HiiHandle, ErrorDetailToken, Buffer, NULL );
  ASSERT (ErrorDetailToken != 0);
    *Token = ErrorDetailToken;
  gBS->FreePool(Buffer);

// Don't save tokens for custom strings.

  if ((mPrivateData.TokenCount +1 < MAX_TOKENS_TO_SAVE) && (!IsCustom)) {
    mPrivateData.SavedTokens[mPrivateData.TokenCount].IsDetail = TRUE;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Event = *EventPtr;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].CodeType = CodeType;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Value = Value;
    mPrivateData.SavedTokens[mPrivateData.TokenCount].Token = *Token;

    mPrivateData.TokenCount++;
  }

  gBS->FreePool(TempBuffer);

  return EFI_SUCCESS;
}

/**

    Reads all error data from Generic Elog with Smbios type and
    updates the form set if necessary.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/
EFI_STATUS
UpdateFormSetWithDataHub( VOID )
{
  EFI_STATUS                        Status;
  ERROR_MANAGER                     *UpdateData = NULL;
  ERROR_LIST                        *CurrentOpCode;
  EFI_STATUS_CODE_TYPE              CodeType = 0;
  EFI_STATUS_CODE_VALUE             Value = 0;
  EFI_GPNV_ERROR_EVENT              *EventPtr;
  EFI_SM_ELOG_REDIR_PROTOCOL        *RedirProtocol;
  UINT8                             ElogData[0xFF];
  UINTN                             DataSize;
  UINTN                             ErrorManagerSize;
  UINT64                            RecordId = CurrentRecordId;
  BOOLEAN                           ProtocolError = FALSE;
  BOOLEAN                           DONE = FALSE;
  BOOLEAN                           LASTRECORD = FALSE;
  UINT8                             *ExtendedData;
  UINT16                            ViewMoreRecordKeyValue=0;
  UINTN                             Size = sizeof(ViewMoreRecordKeyValue);
  EFI_HANDLE                        *HandleBuffer = NULL;
  UINT8                             Index;
  UINTN                             HandleCount, DummySize = 1;
  UINT32                            AttributesRead = EFI_VARIABLE_BOOTSERVICE_ACCESS;


// Getting the all the Redir Protocol Handles installed.

  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiRedirElogProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer
                );



  if (EFI_ERROR (Status)) {
    ProtocolError = TRUE;
  }

  UpdateData = mPrivateData.SetupScreenBuffer;
  CurrentOpCode = &UpdateData->ErrorList[UpdateData->ErrorCount];


// Read all records. Enter Status code errors into the formset.

  if (ProtocolError == TRUE) {

// If Not all Protocols available, print message onto screen

    CurrentOpCode->Handle = (VOID*)mPrivateData.HiiHandle;
    CurrentOpCode->Summary = STRING_TOKEN (STR_NO_PROTOCOL_FOUND);
    CurrentOpCode->DetailInfo = STRING_TOKEN (STR_NO_PROTOCOL_FOUND);
    UpdateData->ErrorCount++;
    CurrentOpCode = (ERROR_LIST*)( (UINT8*)(CurrentOpCode) + sizeof(ERROR_LIST) );
  } else {


// All protocols available, continue with displaying all errors

    DataSize = 0xFF;

    for (Index = 0; Index < HandleCount; Index ++) {


// Get the Instance of the Redir Protocol

        Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiRedirElogProtocolGuid,
                      (VOID **)&RedirProtocol
                      );

        if(!EFI_ERROR(Status)) {
        Status = RedirProtocol->GetEventLogData( RedirProtocol,
                                                   &ElogData[0],
                                                   EfiElogRedirSmSMBIOS,
                                                   &DataSize,
                                                   &RecordId );
        }


// If the Status is EFI_NOT_FOUND that means it is the SmbiosElogRedirProtocol instance.

        if(Status != EFI_NOT_FOUND)
            break;
    }

    if (Status != EFI_SUCCESS) {

// If no records in log or unable to get records properly, print message onto screen

        CurrentOpCode->Handle = (VOID*)mPrivateData.HiiHandle;
        CurrentOpCode->Summary = STRING_TOKEN (STR_SMBIOS_NO_RECORDS_FOUND);
        CurrentOpCode->DetailInfo = STRING_TOKEN (STR_EMPTY);
        UpdateData->ErrorCount++;
        CurrentOpCode = (ERROR_LIST*)( (UINT8*)(CurrentOpCode) + sizeof(ERROR_LIST) );

        DONE= TRUE;
    } else {
      if (RecordId == 0)
        LASTRECORD = TRUE;
    }

    while (!DONE)  {
      EventPtr = (EFI_GPNV_ERROR_EVENT*)(&ElogData[0]);

// If EventType == EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE
// Only pass along information to display errors if it is our OEM defined 
// event type of 0x80 which would be holding the code type and code value 
// in its data (checks size to be sure)

      if (     EventPtr->EventType == EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE )  {

// Increment the "EFI_GPNV_ERROR_EVENT" by Size of "VariableDataType" to 
// get "Status Code Type"

          CodeType =  *((EFI_STATUS_CODE_TYPE*)(&ElogData[sizeof(EFI_GPNV_ERROR_EVENT) + sizeof (UINT8)]));

// Increment the "EFI_GPNV_ERROR_EVENT" by Size of "VariableDataType" 
// and size of "Status Code Type" to get "Status Code Value"

          Value = *((EFI_STATUS_CODE_VALUE*)(&ElogData[sizeof(EFI_GPNV_ERROR_EVENT) + sizeof (UINT8) + sizeof(EFI_STATUS_CODE_TYPE)]));

          AddErrorToFormSet ( EventPtr,
                              NULL, 
                              CodeType, 
                              Value, 
                              &CurrentOpCode, 
                              &UpdateData->ErrorCount );
      } else {


// EventType is legacy

        ExtendedData = &ElogData[sizeof(EFI_GPNV_ERROR_EVENT)];
        AddErrorToFormSet( EventPtr,
                           ExtendedData,
                           0,
                           0,
                           &CurrentOpCode,
                           &UpdateData->ErrorCount );
      }

      if (LASTRECORD) {
        DONE = TRUE;
      } else {

          DataSize = 0xFF;

          Status = RedirProtocol->GetEventLogData( RedirProtocol,
                                                     &ElogData[0],
                                                     EfiElogRedirSmSMBIOS,
                                                     &DataSize,
                                                     &RecordId );
              if (Status != EFI_SUCCESS) {
                DONE = TRUE;
              } else {
               if (RecordId == 0)
                LASTRECORD = TRUE;
            }
      }

// Have a copy of RecordId in CurrentRecordId
// CurrentRecordId is used when Error records have more than 16KB

      CurrentRecordId = RecordId;
      ErrorManagerSize = sizeof(ERROR_MANAGER) + (UpdateData->ErrorCount) * sizeof(ERROR_LIST);

// Check the ErrorManagerSize for not to cross 4KB before setting it
// Here check for 4KB - 3*sizeof(ERROR_LIST). Because LINK should be 
// provided for this case.

      if ( ErrorManagerSize > (MAX_ERROR_MANAGER_NVRAM_VARIABLE_SIZE-(3*sizeof(ERROR_LIST))) ) {
        DONE = TRUE;

// Empty string

        CurrentOpCode->Handle = (VOID*)mPrivateData.HiiHandle;
        CurrentOpCode->Summary = STRING_TOKEN (STR_EMPTY);
        CurrentOpCode->DetailInfo = STRING_TOKEN (STR_EMPTY);
        UpdateData->ErrorCount++;
        CurrentOpCode = (ERROR_LIST*)( (UINT8*)(CurrentOpCode) + sizeof(ERROR_LIST) );

// View remaining Smbios Event logs String

        CurrentOpCode->Handle = (VOID*)mPrivateData.HiiHandle;
        CurrentOpCode->Summary = STRING_TOKEN (STR_SMBIOS_MORE_VIEW);
        CurrentOpCode->DetailInfo = STRING_TOKEN (STR_SMBIOS_MORE_VIEW_HELP);
        UpdateData->ErrorCount++;
        CurrentOpCode = (ERROR_LIST*)( (UINT8*)(CurrentOpCode) + sizeof(ERROR_LIST) );
      } // ErrorManagerSize check
    } // while loop
  } // Check for DONE variable


// When last record is read from Smbios logs, there are no more records to display.
// So, Clear "SmbiosMoreRecords" NVRAM variable
// Note : CurrentRecordId will be zero if it's the last record.

    if ( CurrentRecordId == 0x00 ) {
         Status = gRT->GetVariable (
                        L"SmbiosMoreRecords",
                        &gErrorManagerGuid,
                        NULL,
                        &Size,
                        &ViewMoreRecordKeyValue
                        );
        if ( !EFI_ERROR(Status) ) {

                    Status = gRT->SetVariable (
                            L"SmbiosMoreRecords",
                            &gErrorManagerGuid,
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS ),
                            0,
                            NULL
                            );
                    if(EFI_ERROR(Status)) {
                        DEBUG((EFI_D_ERROR,"SetVariable SmbiosMoreRecords is Failed\n"));        
                    }
       }

    } else {

// If CurrentRecordId is more than one, Set "SmbiosMoreRecords" Nvram variable with KeyValue

        ViewMoreRecordKeyValue = UpdateData->ErrorCount-1;
        Status = gRT->SetVariable (
                        L"SmbiosMoreRecords",
                        &gErrorManagerGuid,
                        (EFI_VARIABLE_BOOTSERVICE_ACCESS ),
                        Size,
                        &ViewMoreRecordKeyValue );
        if(EFI_ERROR(Status)) {
            DEBUG((EFI_D_ERROR,"SetVariable SmbiosMoreRecords is Failed\n"));        
        }
    }

// Update the form in the HII database

    ErrorManagerSize = sizeof(ERROR_MANAGER) + (UpdateData->ErrorCount) * sizeof(ERROR_LIST);

    Status = gRT->GetVariable (
                    L"ErrorManager",
                    &gErrorManagerGuid,
                    &AttributesRead,
                    &DummySize,
                    UpdateData
                    );

    if(Status == EFI_NOT_FOUND){
       AttributesRead = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    }

    Status = gRT->SetVariable (
                    L"ErrorManager",
                    &gErrorManagerGuid,
                    AttributesRead,
                    ErrorManagerSize,
                    UpdateData
                    );
    if(EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"SetVariable ErrorManager is Failed\n"));
    }


// Free the Handle buffer

    if (HandleBuffer != NULL) {
        gBS->FreePool (HandleBuffer);
     }

  return EFI_SUCCESS;

}

/**
  Retrieves a string from a string package in a specific language.  

  @param  StringId     The identifier of the string to retrieved from the string 
                         package associated with HiiHandle.
  @param  String       Pointer to store the retrieved String.
  @param  StringSize   Size of the required string.   
  @param  Language     The language of the string to retrieve.  If this parameter 
                         is NULL, then the current platform language is used.  The 
                         format of Language must follow the language format assumed 
                         the HII Database.

  @retval EF_SUCCESS  The string specified by StringId is present in the string package.
  @retval Other       The string was not returned.

**/

EFI_STATUS
EFIAPI
GpnvHiiGetString (
  IN       EFI_STRING_ID   StringId,
  IN       EFI_STRING      String,
  IN       UINTN           *StringSize,
  IN CONST CHAR8           *Language  OPTIONAL
  )
{
    EFI_STATUS  Status;
    CHAR8       *SupportedLanguages;
    CHAR8       *PlatformLanguage;
    CHAR8       *BestLanguage;
    CHAR8       *StartLang;
    CHAR8       *TempLanguage;
    UINTN       TempStrSize;

    ASSERT (StringId != 0);

    //
    // Initialize all allocated buffers to NULL
    //
    SupportedLanguages = NULL;
    PlatformLanguage   = NULL;
    BestLanguage       = NULL;

    //
    // Get the languages that the package specified by HiiHandle supports
    //
    SupportedLanguages = HiiGetSupportedLanguages (mPrivateData.HiiHandle);
    if (SupportedLanguages == NULL) {
        goto Error;
    }

    //
    // Get the current platform language setting
    //
    GetEfiGlobalVariable2 (L"PlatformLang", (VOID**)&PlatformLanguage, NULL);

    //
    // If Language is NULL, then set it to an empty string, so it will be 
    // skipped by GetBestLanguage()
    //
    if (Language == NULL) {
        Language = "";
    }

    //
    // Get the best matching language from SupportedLanguages
    //
    BestLanguage = GetBestLanguage (
                        SupportedLanguages, 
                        FALSE,                                             // RFC 4646 mode
                        Language,                                          // Highest priority 
                        PlatformLanguage != NULL ? PlatformLanguage : "",  // Next highest priority
                        SupportedLanguages,                                // Lowest priority 
                        NULL
                        );
    if(BestLanguage == NULL) {
        goto Error;
    }

    TempStrSize = *StringSize;
    //
    // Retrieve the size of the string in the string package for the BestLanguage
    //
    Status = mPrivateData.HiiString->GetString (
                         mPrivateData.HiiString,
                         BestLanguage,
                         mPrivateData.HiiHandle,
                         StringId,
                         String,
                         &TempStrSize,
                         NULL
                         );

    if(Status == EFI_INVALID_LANGUAGE) {

        //
        // Loop through each language that the string supports
        //
        for (StartLang = SupportedLanguages; *StartLang != '\0'; ) {
            //
            // Cache a pointer to the beginning of the current language in the list of languages
            //
            TempLanguage = StartLang;
            TempStrSize = *StringSize;
            //
            // Search for the next language separator and replace it with a Null-terminator
            //
            for (; *StartLang != 0 && *StartLang != ';'; StartLang++);
            if (*StartLang != 0) {
                *(StartLang++) = '\0';
            }

            //
            // Retrieve the size of the string in the string package for the BestLanguage
            //
            Status = mPrivateData.HiiString->GetString (
                                    mPrivateData.HiiString,
                                    TempLanguage,
                                    mPrivateData.HiiHandle,
                                    StringId,
                                    String,
                                    &TempStrSize,
                                    NULL
                                    );
            if(Status == EFI_SUCCESS) {
                break;
            }
        }
    }

Error:
    //
    // Free allocated buffers
    //
    if (SupportedLanguages != NULL) {
        FreePool (SupportedLanguages);
    }
    if (PlatformLanguage != NULL) {
        FreePool (PlatformLanguage);
    }
    if (BestLanguage != NULL) {
        FreePool (BestLanguage);
    }

    //
    // Return the status
    //
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
