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


/** @file StatusCodePei.c

	PEI StatusCode functions
**/
//**********************************************************************
#include "StatusCodeInt.h"
#include <AmiPeiLib.h>
#include <Ppi/ProgressCode.h>
#include <Ppi/Stall.h> 
#include <Token.h> 
#include <Ppi/ReportStatusCodeHandler.h> 
#include <Ppi/AmiDebugService.h> 
#include <Library/PlatformHookLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#define INITIAL_RSC_PEI_ENTRIES 8
#define MAX_PEI_STRING_BUFFER_SIZE 256

UINT16 StringMaxSize = MAX_PEI_STRING_BUFFER_SIZE;

extern EFI_GUID gStatusCodeCallbackGuid;

extern EFI_GUID gEfiPeiRscHandlerPpiGuid;

extern INIT_FUNCTION PEI_INIT_LIST EndOfInitList;
INIT_FUNCTION* InitPartsList[] = {PEI_INIT_LIST NULL};

extern STRING_FUNCTION PEI_STRING_LIST EndOfStringList;
STRING_FUNCTION* StringList[] = {PEI_STRING_LIST NULL};

extern SIMPLE_FUNCTION PEI_SIMPLE_LIST EndOfSimpleList;
SIMPLE_FUNCTION* SimpleList[] = {PEI_SIMPLE_LIST NULL};

extern MISC_FUNCTION PEI_MISC_LIST EndOfMiscList;
MISC_FUNCTION* MiscList[] = {PEI_MISC_LIST NULL};

extern CHECKPOINT_FUNCTION PEI_CHECKPOINT_LIST EndOfCheckpointList;
CHECKPOINT_FUNCTION* CheckpointList[] = {PEI_CHECKPOINT_LIST NULL};

#define ERROR_ACTION(Value,Action) Action

extern ERROR_CODE_ACTION PEI_ERROR_CODE_ACTIONS EndOfActionList;

#undef ERROR_ACTION
#define ERROR_ACTION(Value,Action) {Value,&Action}

ERROR_CODE_ACTION_MAP ErrorCodeActions[] =
{
    PEI_ERROR_CODE_ACTIONS
    {0,0}
};

#if DETAILED_ERROR_MESSAGES
// The more error messages is included in this table, the larger our executable is going to be.
// StatusCode PEIM is uncompressed and boot block space is precious.
// We are only including the most common errors here. Most of them are reported by Core components.
ERROR_MESSAGE ErrorMessage[] = {
//Reported by PEI Core
    { PEI_MEMORY_NOT_INSTALLED, "Memory not installed"},
    { PEI_MEMORY_INSTALLED_TWICE, "Memory installed twice"},

//Reported by CPU PEIM
    { PEI_CPU_SELF_TEST_FAILED, "CPU self test failed"},
    { PEI_CPU_NO_MICROCODE, "No Microcode"},
//If non of the errors above apply use this one
    { PEI_CPU_INTERNAL_ERROR, "Internal CPU error"},
//Generic CPU error. It should only be used if non of the errors above apply
    { PEI_CPU_ERROR, "CPU error"},

//Reported by DXE IPL
    { PEI_RECOVERY_PPI_NOT_FOUND, "Recovery PPI not found"},
    { PEI_RECOVERY_FAILED, "Recovery failed"},
    { PEI_S3_RESUME_PPI_NOT_FOUND, "S3 Resume PPI not found"},
    { PEI_S3_RESUME_FAILED, "S3 resume failed"},

//Reported by Recovery PEIM
    { PEI_RECOVERY_NO_CAPSULE, "Recovery capsule not found"},
    { PEI_RECOVERY_INVALID_CAPSULE, "Invalid recovery capsule"},

//Reported by S3 Resume PEIM
    { PEI_S3_BOOT_SCRIPT_ERROR, "S3 boot script execution error"},
    { PEI_S3_OS_WAKE_ERROR, "S3 OS Wake error"},
    { PEI_S3_RESUME_ERROR, "S3 resume error"},

//Reported by PEI Core
    { PEI_DXEIPL_NOT_FOUND, "DXE IPL not found"},

//Reported by DXE IPL
    { PEI_DXE_CORE_NOT_FOUND, "DXE Core not found"},

//Reported by PEI Core
    { PEI_RESET_NOT_AVAILABLE, "Reset PPI is not available"},

    {0, NULL}
};

BOOLEAN PrintDetailedErrorMessage(IN EFI_STATUS_CODE_VALUE Value, OUT UINT8 *String){
    UINT32 i;
    for(i=0; ErrorMessage[i].ErrorValue!=0; i++){
        if (ErrorMessage[i].ErrorValue == Value){
            Sprintf_s(
                String, StringMaxSize,
                "ERROR: %a\n", ErrorMessage[i].ErrorString
            );
            return TRUE;
        }
    }
    return FALSE;    
}
#endif

UINTN*
CreateRscHandlerCallbackPacket ( 
  )
{
  UINTN  *NumberOfEntries;
  EFI_HOB_GUID_TYPE *Hob;
  EFI_STATUS        Status;
  CONST EFI_PEI_SERVICES **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  //
  // Build GUID'ed HOB with PCD defined size.
  //
  
  Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, 
                            (UINT16)(sizeof (EFI_PEI_RSC_HANDLER_CALLBACK) * INITIAL_RSC_PEI_ENTRIES 
                        + sizeof (UINTN)+ sizeof (EFI_HOB_GUID_TYPE)), (VOID**)&Hob);
  //ASSERT_PEI_ERROR(PeiServices,Status);
  if (EFI_ERROR(Status)) return NULL;
  MemCpy (&Hob->Name, &gStatusCodeCallbackGuid, sizeof (EFI_GUID));

  NumberOfEntries = (UINTN*)(Hob + 1);                    

  *NumberOfEntries = 0;

  return NumberOfEntries;
}

/**
    Remove a previously registered callback function from the notification list.

        
    @param Callback A pointer to a function of type 
    @param that is to be unregistered.

    @retval 
        EFI_SUCCESS           The function was successfully unregistered.
        EFI_INVALID_PARAMETER The callback function was NULL.
        EFI_NOT_FOUND         The callback function was not found to be unregistered.

**/
EFI_STATUS
PpiUnregister (
  IN EFI_PEI_RSC_HANDLER_CALLBACK Callback
  )
{
  EFI_PEI_HOB_POINTERS            Hob;
  EFI_PEI_RSC_HANDLER_CALLBACK    *CallbackEntry;
  UINTN                           *NumberOfEntries;
  UINTN                           Index;
  EFI_STATUS                      Status;


  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Hob.Raw  = GetFirstGuidHob (&gStatusCodeCallbackGuid);
  while (Hob.Raw != NULL) {
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    CallbackEntry   = (EFI_PEI_RSC_HANDLER_CALLBACK *) (NumberOfEntries + 1);
    for (Index = 0; Index < *NumberOfEntries; Index++) {
      if (CallbackEntry[Index] == Callback) {
        CallbackEntry[Index] = CallbackEntry[*NumberOfEntries - 1];
        *NumberOfEntries -= 1;
        return EFI_SUCCESS;
      }
    }
    Status = FindNextHobByGuid (&gStatusCodeCallbackGuid, (VOID**)&Hob.Raw); 
    if (EFI_ERROR(Status)) Hob.Raw = NULL;
  }

  return EFI_NOT_FOUND;
}

/**
    Register the callback function for ReportStatusCode() notification.

        
    @param Callback A pointer to a function of type 
    @param that is to be registered.

    @retval 
        EFI_SUCCESS           The function was successfully registered.
        EFI_INVALID_PARAMETER The callback function was NULL.
        EFI_OUT_OF_RESOURCES  The internal buffer ran out of space. No more functions can be
        registered.
        EFI_ALREADY_STARTED   The function was already registered. It can't be registered again.

**/
EFI_STATUS
PpiRegister (
  IN EFI_PEI_RSC_HANDLER_CALLBACK Callback
  )
{
  EFI_PEI_HOB_POINTERS          Hob;
  EFI_PEI_RSC_HANDLER_CALLBACK  *CallbackEntry;
  UINTN                         *NumberOfEntries;
  UINTN                         Index;
  UINTN                         *FreePacket;
  EFI_STATUS                    Status;

  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Hob.Raw  = GetFirstGuidHob (&gStatusCodeCallbackGuid);
  FreePacket = NULL;
  while (Hob.Raw != NULL) {
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    CallbackEntry   = (EFI_PEI_RSC_HANDLER_CALLBACK *) (NumberOfEntries + 1);
    if (*NumberOfEntries < INITIAL_RSC_PEI_ENTRIES) {
      FreePacket = NumberOfEntries;
    }
    for (Index = 0; Index < *NumberOfEntries; Index++) {
      if (CallbackEntry[Index] == Callback) {
        //
        // If the function was already registered. It can't be registered again.
        //
        return EFI_ALREADY_STARTED;
      }
    }

    Status = FindNextHobByGuid (&gStatusCodeCallbackGuid, (VOID**)&Hob.Raw);
    if (EFI_ERROR(Status)) Hob.Raw = NULL;
  }

  if (FreePacket == NULL) {
    FreePacket = CreateRscHandlerCallbackPacket();
  }
  if (FreePacket == NULL) return EFI_OUT_OF_RESOURCES;  
  CallbackEntry = (EFI_PEI_RSC_HANDLER_CALLBACK *) (FreePacket + 1);
  CallbackEntry[*FreePacket] = Callback;
  *FreePacket += 1;

  return EFI_SUCCESS;
}


EFI_PEI_RSC_HANDLER_PPI     RscHandlerPpi = {
  PpiRegister,
  PpiUnregister
  };

EFI_PEI_PPI_DESCRIPTOR   RscHandlerPpiDescriptor[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiRscHandlerPpiGuid,
    &RscHandlerPpi
  }
};

//BOOLEAN Busy = FALSE;
//UINT8 n=0;
/**
    Top level status code reporting routine exposed by the status code protocol/PPI.
    Calls the various types of status code handlers
    (SimpleStatusReport, StringStatusReport, MiscStatusReport, PerformErrorCodeAction)
    Generates string from the status code data to pass to StringStatusReport function.
    Also pass all parameters to Registered Statuse Code Routers.

        
    @param PeiServices pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error
    @param Instance 
    @param CallerId OPTIONAL - The GUID of the caller function
    @param Data OPTIONAL - the extended data field that contains additional info

    @retval EFI_STATUS

**/
EFI_STATUS ReportStatusCodePei (
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN UINT32 Instance, IN EFI_GUID *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA *Data OPTIONAL
)
{

	UINT8 StringPei[MAX_PEI_STRING_BUFFER_SIZE];

    EFI_PEI_HOB_POINTERS            Hob;
    EFI_PEI_RSC_HANDLER_CALLBACK    *CallbackEntry;
    UINTN                           *NumberOfEntries;
    UINTN                           Index;
    EFI_STATUS                      Status;

    //if (IsItRecursiveCall(&PeiRouterRecurciveStatus, FALSE, TRUE) != TRUE) 
    //{    //return EFI_ACCESS_DENIED;
    Hob.Raw  = GetFirstGuidHob (&gStatusCodeCallbackGuid);
    while (Hob.Raw != NULL) 
    {
        NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
        CallbackEntry   = (EFI_PEI_RSC_HANDLER_CALLBACK *) (NumberOfEntries + 1);
        for (Index = 0; Index < *NumberOfEntries; Index++) 
        {
            CallbackEntry[Index](
            PeiServices,
            Type,
            Value,
            Instance,
            CallerId,
            Data
            );
        }

        Status = FindNextHobByGuid (&gStatusCodeCallbackGuid, (VOID**)&Hob.Raw);
        if (EFI_ERROR(Status)) Hob.Raw = NULL;
        
    }
//    }
//    IsItRecursiveCall (&PeiRouterRecurciveStatus, TRUE, FALSE);


    return AmiReportStatusCode (PeiServices,Type,Value,Instance,CallerId,Data, StringPei);

}

// ******************** AmiDebugService implementation ****************
VOID EFIAPI DebugPrintHelper(
    UINTN ErrorLevel, CONST CHAR8 *Format, VA_LIST VaListMarker
)
{
    UINT8 StringPei[MAX_PEI_STRING_BUFFER_SIZE];
    AmiDebigServiceDebugPrintHelper(GetPeiServicesTablePointer(),StringPei,ErrorLevel,Format,VaListMarker);
}

VOID EFIAPI DebugAssertHelper(
    CONST CHAR8  *FileName, UINTN LineNumber, CONST CHAR8  *Description
)
{
    UINT8 StringPei[MAX_PEI_STRING_BUFFER_SIZE];
    AmiDebigServiceDebugAssertHelper(GetPeiServicesTablePointer(),StringPei,FileName,LineNumber,Description);
}

static AMI_DEBUG_SERVICE_PPI AmiDebugServicePpi={DebugPrintHelper, DebugAssertHelper};

EFI_PEI_PPI_DESCRIPTOR PeiDebugPpiDescriptor[] = 
{
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmiDebugServicePpiGuid,
    &AmiDebugServicePpi 
};

VOID AmiPeiDebugServiceInitialization(IN CONST EFI_PEI_SERVICES **PeiServices)
{
    (*PeiServices)->InstallPpi(PeiServices, &PeiDebugPpiDescriptor[0]);
}
// ******************** End of AmiDebugService implementation *********

// Status Code PPI
EFI_PEI_PROGRESS_CODE_PPI  StatusCodePpi = {ReportStatusCodePei};
// Status Code PPI Descriptor
EFI_PEI_PPI_DESCRIPTOR StatusCodePpiDescriptor[] =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiStatusCodePpiGuid, &StatusCodePpi
};

/**
    Pei "entry point" for this module, called by the Pei Core.
    Control flow:
    1. Creates a RSC Hob and installs RSC PPI.
    2. Calls InitStatusCodeParts function.
    3. Installs the Status Code PPI.
    InitStatusCodeParts function calls initialization routines of status code subcomponents
    registered under StatusCodeInitialize eLink.
    Status Code PPI is initialized with the address of ReportStatusCode function.

    @param 
        *FfsHeader - pointer to the header of the current firmware file system file
        **PeiServices - pointer to the Pei Services table

    @retval EFI_STATUS

**/
EFI_STATUS PeiInitStatusCode(
    IN EFI_PEI_FILE_HANDLE  	FfsHeader,
    IN CONST EFI_PEI_SERVICES         **PeiServices 
)
{
    EFI_STATUS  Status;

    InitStatusCodeParts(FfsHeader,PeiServices);
    // install debug service PPI
    AmiPeiDebugServiceInitialization(PeiServices);
    CreateRscHandlerCallbackPacket ();
    // First install Report Status Code Handler PPI
    Status = (*PeiServices)->InstallPpi(
                PeiServices, &RscHandlerPpiDescriptor[0]
             );

    // And then install the Status Code PPI
    Status = (*PeiServices)->InstallPpi(
                 PeiServices, &StatusCodePpiDescriptor[0]
             );
    return Status;
}


/**
    Error Code Action.
    Attempts to perform a system reset. If reset fails, returns.

        
    @param PeiServices 
    @param Value Value of the error code that triggered the action.

         
    @retval VOID

**/
VOID ResetOrResume(
    IN VOID *PeiServices, IN EFI_STATUS_CODE_VALUE Value 
)
{
    (*(EFI_PEI_SERVICES **)PeiServices)->ResetSystem(PeiServices);
}
   
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
